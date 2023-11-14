// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseBoardPawn.h"

#include "DiceActor.h"
#include "TimerManager.h"
#include "Net/UnrealNetwork.h"
#include "Algo/RandomShuffle.h"
#include "Logging/StructuredLog.h"
#include "MovementMonitorComponent.h"
#include "Kismet/GameplayStatics.h"
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
	bIsMyTurn = false;

	PlayState = EPlayState::EPS_WaitingForOpponent;
}

void ABaseBoardPawn::RollDice()
{
	if (PlayState == EPlayState::EPS_WaitingForThrow)
	{
		ServerThrowDices(DiceArray);
		// Set the player state to rolling
	}
	else if (PlayState == EPlayState::EPS_Selected)
	{
		if (SelectedDices.Num() > 0)
		{
			if (SelectedScore != 0)
			{
				TurnScore += SelectedScore;
				SelectedScore = 0;

				if (SelectedDices.Num() == 6 || SelectedDices.Num() + OnHolderDices.Num() == 6)
				{
					ServerThrowDices(DiceArray);
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
					TimerDelegate.BindUFunction(this, FName("ServerThrowDices"), NotSelectedDices);
					// Set the timer
					GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, 1.0f, false);
				}
			}
		}
		DeselectAllDices();
	}

	// Update the texts
	UpdateTexts();
}

void ABaseBoardPawn::PassDice()
{
	if (GetLocalRole() < ROLE_Authority)
	{
		ServerPassDice();
		return;
	}
	if (!bIsMyTurn)
	{
		return;
	}
	if (SelectedDices.Num() != 0 && SelectedScore != 0)
	{
		EndTurn();
	}
}

void ABaseBoardPawn::ServerPassDice_Implementation()
{
	PassDice();
}

void ABaseBoardPawn::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABaseBoardPawn, TotalScore);
	DOREPLIFETIME(ABaseBoardPawn, TurnScore);
	DOREPLIFETIME(ABaseBoardPawn, SelectedScore);

	DOREPLIFETIME(ABaseBoardPawn, bIsMyTurn);
	DOREPLIFETIME(ABaseBoardPawn, PlayState);

	DOREPLIFETIME(ABaseBoardPawn, DiceArray);
	DOREPLIFETIME(ABaseBoardPawn, OnHolderDices);
	DOREPLIFETIME(ABaseBoardPawn, SelectedDices);
}

void ABaseBoardPawn::OnDiceSelected(ADiceActor *DiceActor)
{
	UE_LOGFMT(LogTemp, Warning, "ABaseBoardPawn::OnDiceSelected  LOCAL ROLE{0}", GetLocalRole());

	if (DiceActor->bIsSelected)
	{
		SelectedDices.Add(DiceActor);
	}
	else
	{
		SelectedDices.Remove(DiceActor);
	}

	int32 Score = UFarkleUtils::CountScore(SelectedDices);

	SelectedScore = Score;
	if (Score)
	{
		PlayState = EPlayState::EPS_Selected;
	}
	else
	{
		PlayState = EPlayState::EPS_Selecting;
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
		PlayState = EPlayState::EPS_Selecting;
	}
	else
	{
		TurnScore = 0;
		SelectedScore = 0;
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

void ABaseBoardPawn::ThrowDices(TArray<ADiceActor *> Dices)
{
	PlayState = EPlayState::EPS_Rolling;

	for (int i = 0; i < Dices.Num(); i++)
	{
		ADiceActor *DiceActor = Dices[i];

		UStaticMeshComponent *DiceMesh = Cast<UStaticMeshComponent>(DiceActor->GetRootComponent());

		DiceActor->bIsOnHold = false;

		// Move the dice to the throw position
		DiceActor->SetActorLocation(ThrowPositions[i]->GetComponentLocation());

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

	Algo::RandomShuffle(ThrowPositions);
}

void ABaseBoardPawn::HoldDices(const TArray<ADiceActor *>& Dices,const TArray<USphereComponent *> &HoldTo, int32 Index)
{
	if(GetLocalRole() < ROLE_Authority){
		ServerHoldDices(Dices, HoldTo, Index);
		return;
	}

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

void ABaseBoardPawn::ServerHoldDices_Implementation(const TArray<ADiceActor *> &Dices, const TArray<USphereComponent *> &HoldTo, int32 Index)
{
	HoldDices(Dices, HoldTo, Index);
}

void ABaseBoardPawn::UpdateTexts()
{
	// Update the score text
	ScoreText->SetText(FText::FromString(FString::Printf(TEXT("Score:%d"), TotalScore)));

	// Update the turn score text
	TurnScoreText->SetText(FText::FromString(FString::Printf(TEXT("Turn Score:%d"), TurnScore)));

	// Update the selected score text
	SelectedScoreText->SetText(FText::FromString(FString::Printf(TEXT("Selected Score:%d"), SelectedScore)));
}

void ABaseBoardPawn::SetPlayState(EPlayState NewPlayState)
{
	PlayState = NewPlayState;
}

EPlayState ABaseBoardPawn::GetPlayState() const
{
	return PlayState;
}

int32 ABaseBoardPawn::GetTotalScore() const
{
	return TotalScore;
}

int32 ABaseBoardPawn::GetTurnScore() const
{
	return TurnScore;
}

int32 ABaseBoardPawn::GetSelectedScore() const
{
	return SelectedScore;
}

void ABaseBoardPawn::StartTurn()
{
	if (GetLocalRole() < ROLE_Authority)
	{
		ServerEndTurn();
	}
	else
	{
		bIsMyTurn = true;
		PlayState = EPlayState::EPS_WaitingForThrow;

		// Inform the client that the turn has ended
		ClientStartTurn();
	}
}

void ABaseBoardPawn::ServerStartTurn_Implementation()
{
	// Perform the end turn actions on the server
	StartTurn();
}

bool ABaseBoardPawn::ServerStartTurn_Validate()
{
	// Perform any necessary validation here
	return true;
}

void ABaseBoardPawn::ClientStartTurn_Implementation()
{
	// This will be executed on the client only
	UE_LOG(LogTemp, Warning, TEXT("ABaseBoardPawn::StartTurn: Client"));
	OnStartTurn.Broadcast();
}

void ABaseBoardPawn::EndTurn()
{
	UE_LOG(LogTemp, Warning, TEXT("ABaseBoardPawn::EndTurn"));
	if (GetLocalRole() < ROLE_Authority)
	{
		ServerEndTurn();
		return;
	}
	bIsMyTurn = false;
	PlayState = EPlayState::EPS_WaitingForOpponent;

	TotalScore += TurnScore + SelectedScore;

	TurnScore = SelectedScore = 0;

	HoldDices(DiceArray, HolderPositions, 0);

	// Inform the client that the turn has ended
	ClientEndTurn();

	OnEndTurn.Broadcast();
}

void ABaseBoardPawn::SpawnDices(int32 PlayerId)
{
	//ServerSpawnDices(PlayerId);

	for (int i = 0; i < 6; i++)
	{
		ADiceActor *Dice = GetWorld()->SpawnActor<ADiceActor>(BP_DiceActor, HolderPositions[i]->GetComponentLocation(), FRotator::ZeroRotator);
		Dice->SetOwner(UGameplayStatics::GetPlayerController(GetWorld(), PlayerId));
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


void ABaseBoardPawn::ServerSpawnDices_Implementation(int32 PlayerId)
{
	for (int i = 0; i < 6; i++)
	{
		ADiceActor *Dice = GetWorld()->SpawnActor<ADiceActor>(BP_DiceActor, HolderPositions[i]->GetComponentLocation(), FRotator::ZeroRotator);
		Dice->SetOwner(UGameplayStatics::GetPlayerController(GetWorld(), PlayerId));
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

bool ABaseBoardPawn::ServerSpawnDices_Validate(int32 PlayerId)
{
	return true;
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
	UpdateTexts();
	OnEndTurn.Broadcast();
}

void ABaseBoardPawn::ServerThrowDices_Implementation(const TArray<ADiceActor *> &Dices)
{
	ThrowDices(Dices);
}

bool ABaseBoardPawn::ServerThrowDices_Validate(const TArray<ADiceActor *> &Dices)
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
