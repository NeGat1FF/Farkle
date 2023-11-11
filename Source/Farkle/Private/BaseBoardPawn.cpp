// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseBoardPawn.h"

#include "DiceActor.h"
#include "TimerManager.h"
#include "Algo/RandomShuffle.h"
#include "FarklePlayerState.h"
#include "Logging/StructuredLog.h"
#include "MovementMonitorComponent.h"
#include "Components/AudioComponent.h"
#include "Components/SphereComponent.h"
#include "Components/TextRenderComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "FarkleUtils.h"

// Sets default values
ABaseBoardPawn::ABaseBoardPawn()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	BoardMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BoardMesh"));
	SetRootComponent(BoardMesh);

	MovementMonitor = CreateDefaultSubobject<UMovementMonitorComponent>(TEXT("MovementMonitor"));

	ScoreText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("ScoreText"));
	ScoreText->SetupAttachment(RootComponent);

	TurnScoreText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("TurnScoreText"));
	TurnScoreText->SetupAttachment(RootComponent);

	SelectedScoreText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("SelectedScoreText"));
	SelectedScoreText->SetupAttachment(RootComponent);

	for (int i = 1; i <= 6; i++)
	{
		FString ComponentName = FString::Printf(TEXT("HolderPosition%d"), i);
		USphereComponent *HolderPosition = CreateDefaultSubobject<USphereComponent>(*ComponentName);
		HolderPosition->SetupAttachment(BoardMesh);
		HolderPosition->SetSphereRadius(1.0f);
		HolderPositions.Add(HolderPosition);
	}

	for (int i = 1; i <= 6; i++)
	{
		FString ComponentName = FString::Printf(TEXT("ThrowPosition%d"), i);
		USphereComponent *ThrowPosition = CreateDefaultSubobject<USphereComponent>(*ComponentName);
		ThrowPosition->SetupAttachment(BoardMesh);
		ThrowPosition->SetSphereRadius(1.0f);
		ThrowPositions.Add(ThrowPosition);
	}

	MovementMonitor->OnAllActorsStopped.AddDynamic(this, &ABaseBoardPawn::OnAllActorsStopped);

	bIsAI = false;
}

// Called when the game starts or when spawned
void ABaseBoardPawn::BeginPlay()
{
	Super::BeginPlay();
}

void ABaseBoardPawn::PossessedBy(AController *NewController)
{
	Super::PossessedBy(NewController);

	// Get player state
	FarklePlayerState = Cast<AFarklePlayerState>(GetPlayerState());

	if (!FarklePlayerState)
	{
		UE_LOG(LogTemp, Error, TEXT("ABoardPawn::BeginPlay: PlayerState is not an AFarklePlayerState"));
		return;
	}
	UE_LOGFMT(LogTemp, Warning, "{0}: BoardPawn Possessed", GetPlayerState()->GetPlayerName());
}

void ABaseBoardPawn::RollDice()
{
	if (!FarklePlayerState)
	{
		FarklePlayerState = Cast<AFarklePlayerState>(GetPlayerState());

		if (!FarklePlayerState)
		{
			UE_LOGFMT(LogTemp, Error, "{0}: PlayerState is not an AFarklePlayerState", GetPlayerState()->GetPlayerName());
		}
	}

	if (!FarklePlayerState->bIsMyTurn)
	{
		UE_LOGFMT(LogTemp, Warning, "{0}: Not my turn", GetPlayerState()->GetPlayerName());
		return;
	}
	EPlayerState CurrentPlayerState = FarklePlayerState->GetPlayerState();

	if (CurrentPlayerState == EPlayerState::EPS_WaitingForThrow)
	{
		ServerThrowDices(DiceArray, ThrowPositions);
		// Set the player state to rolling
	}
	else if (CurrentPlayerState == EPlayerState::EPS_Selected)
	{
		if (SelectedDices.Num() > 0)
		{
			if (FarklePlayerState->SelectedScore != 0)
			{
				FarklePlayerState->AddTurnScore();

				if (SelectedDices.Num() == 6 || SelectedDices.Num() + OnHolderDices.Num() == 6)
				{
					ServerThrowDices(DiceArray, ThrowPositions);
				}
				else
				{
					TArray<ADiceActor *> NotSelectedDices = GetNotSelectedDices();
					// Move the selected dices to the holder positions
					HoldDices(SelectedDices, HolderPositions, OnHolderDices.Num());

					// Timer handle
					FTimerHandle TimerHandle;
					// Timer delegate
					FTimerDelegate TimerDelegate;
					// Bind the timer delegate to the function
					TimerDelegate.BindUFunction(this, FName("ServerThrowDices"), NotSelectedDices, ThrowPositions);
					// Set the timer
					GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, 1.0f, false);
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("ABoardPawn::RollDice: Selected wrong combination"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("ABoardPawn::RollDice: No dices selected"));
		}
		DeselectAllDices();
	}
	UpdateTexts();
}

void ABaseBoardPawn::PassDice()
{
	if (!FarklePlayerState->bIsMyTurn)
	{
		return;
	}
	if (SelectedDices.Num() != 0 && FarklePlayerState->SelectedScore != 0)
	{
		EndTurn();
	}
}

void ABaseBoardPawn::OnDiceSelected(ADiceActor *DiceActor)
{
	if (DiceActor->bIsSelected)
	{
		SelectedDices.Add(DiceActor);
	}
	else
	{
		SelectedDices.Remove(DiceActor);
	}

	int32 Score = UFarkleUtils::CountScore(SelectedDices);

	FarklePlayerState->UpdateSelectedScore(Score);
	if (Score)
	{
		FarklePlayerState->SetPlayerState(EPlayerState::EPS_Selected);
	}
	else
	{
		FarklePlayerState->SetPlayerState(EPlayerState::EPS_Selecting);
	}

	UpdateTexts();
}

void ABaseBoardPawn::OnAllActorsStopped()
{
	for (ADiceActor *Dice : DiceArray)
	{
		Dice->RollingSound->Stop();
	}
	if (UFarkleUtils::CanScore(GetNotSelectedDices()))
	{
		FarklePlayerState->SetPlayerState(EPlayerState::EPS_Selecting);
	}
	else
	{
		FarklePlayerState->TurnScore = 0;
		EndTurn();
	}
}

void ABaseBoardPawn::DeselectAllDices()
{
	for (ADiceActor *Dice : DiceArray)
	{
		if (Dice->bIsSelected)
		{
			Dice->Deselect(false);
		}
	}
}

void ABaseBoardPawn::ThrowDices(TArray<ADiceActor *> Dices, TArray<USphereComponent *> &ThrowFrom)
{
	FarklePlayerState->SetPlayerState(EPlayerState::EPS_Rolling);

	for (int i = 0; i < Dices.Num(); i++)
	{
		ADiceActor *DiceActor = Dices[i];

		UStaticMeshComponent *DiceMesh = Cast<UStaticMeshComponent>(DiceActor->GetRootComponent());

		DiceActor->bIsOnHold = false;

		// Move the dice to the throw position
		DiceActor->SetActorLocation(ThrowFrom[i]->GetComponentLocation());

		// Enable physics
		DiceMesh->SetSimulatePhysics(true);

		// Set a random rotation to start
		FVector RandomRotation = FVector(FMath::RandRange(-180.f, 180.f), FMath::RandRange(-180.f, 180.f), FMath::RandRange(-180.f, 180.f));
		DiceMesh->AddLocalRotation(FQuat::MakeFromEuler(RandomRotation));

		// Apply a random force or torque to simulate a roll
		float RandomForce = FMath::RandRange(-500.f, 500.f);
		DiceMesh->AddForce(ThrowDirection * (4500.0f + RandomForce));

		MovementMonitor->AddActorToTrack(DiceActor);

		// Remove the dice from the holder dices
		OnHolderDices.Remove(DiceActor);
	}

	FTimerHandle TimerHandle;
	FTimerDelegate TimerDelegate;
	TimerDelegate.BindUFunction(MovementMonitor, FName("StartTracking"));

	GetWorldTimerManager().SetTimer(TimerHandle, TimerDelegate, 0.2f, false);

	// Shuffle ThrowFrom array
	Algo::RandomShuffle(ThrowFrom);
}

void ABaseBoardPawn::HoldDices(TArray<ADiceActor *> Dices, TArray<USphereComponent *> &HoldTo, int32 Index)
{
	for (int i = 0; i < Dices.Num(); i++)
	{
		ADiceActor *DiceActor = Dices[i];

		DiceActor->bIsOnHold = true;

		Cast<UStaticMeshComponent>(DiceActor->GetRootComponent())->SetSimulatePhysics(false);

		DiceActor->TransformTo(HoldTo[Index + i]->GetComponentLocation(), FRotator::ZeroRotator, 1.0f);

		OnHolderDices.Add(DiceActor);
	}

	DeselectAllDices();
}

void ABaseBoardPawn::UpdateTexts()
{
	// Update the score text
	ScoreText->SetText(FText::FromString(FString("Score:").Append(FString::FromInt(FarklePlayerState->GetScore()))));

	// Update the turn score text
	TurnScoreText->SetText(FText::FromString(FString::Printf(TEXT("Turn Score:%d"), FarklePlayerState->TurnScore)));

	// Update the selected score text
	SelectedScoreText->SetText(FText::FromString(FString::Printf(TEXT("Selected Score:%d"), FarklePlayerState->SelectedScore)));
}

void ABaseBoardPawn::EndTurn()
{
	if (GetLocalRole() < ROLE_Authority)
	{
		ServerEndTurn();
	}
	else
	{
		// Server-side logic
		FarklePlayerState->EndTurn();
		HoldDices(DiceArray, HolderPositions, 0);
		UpdateTexts();

		// Inform the client that the turn has ended
		ClientEndTurn();
	}
}

void ABaseBoardPawn::SpawnDices()
{
	ServerSpawnDices();
}

void ABaseBoardPawn::ServerEndTurn_Implementation()
{
	// Perform the end turn actions on the server
	EndTurn();
}

bool ABaseBoardPawn::ServerEndTurn_Validate()
{
	// Perform any necessary validation here
	return true;
}

void ABaseBoardPawn::ClientEndTurn_Implementation()
{
	// This will be executed on the client only
	UE_LOG(LogTemp, Warning, TEXT("ABaseBoardPawn::EndTurn: Client"));
	OnEndTurn.Broadcast();
}

void ABaseBoardPawn::ServerThrowDices_Implementation(const TArray<ADiceActor *> &Dices, const TArray<USphereComponent *> &ThrowFrom)
{
	ThrowDices(DiceArray, ThrowPositions);
}

bool ABaseBoardPawn::ServerThrowDices_Validate(const TArray<ADiceActor *> &Dices, const TArray<USphereComponent *> &ThrowFrom)
{
	return true;
}

void ABaseBoardPawn::ServerSpawnDices_Implementation()
{
	for (int i = 0; i < 6; i++)
	{
		ADiceActor *Dice = GetWorld()->SpawnActor<ADiceActor>(BP_DiceActor, HolderPositions[i]->GetComponentLocation(), FRotator::ZeroRotator);
		Dice->OnDiceSelected.AddDynamic(this, &ABaseBoardPawn::OnDiceSelected);
		DiceArray.Add(Dice);
	}

	OnHolderDices = DiceArray;

	// Calculate the direction vector
	ThrowDirection = (HolderPositions[0]->GetComponentLocation() - ThrowPositions[0]->GetComponentLocation()).GetSafeNormal();
	ThrowDirection.Z = 0.0f;

	// round the numbers to 1 or -1
	ThrowDirection.X = FMath::RoundToInt(ThrowDirection.X);
	ThrowDirection.Y = FMath::RoundToInt(ThrowDirection.Y);
}

bool ABaseBoardPawn::ServerSpawnDices_Validate()
{
	return true;
}

TArray<ADiceActor *> ABaseBoardPawn::GetNotSelectedDices()
{
	TArray<ADiceActor *> NotSelectedDices;

	for (ADiceActor *Dice : DiceArray)
	{
		if (!Dice->bIsSelected && !Dice->bIsOnHold)
		{
			NotSelectedDices.Add(Dice);
		}
	}

	return NotSelectedDices;
}
