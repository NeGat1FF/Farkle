// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include <map>
#include "BaseBoardPawn.generated.h"

class ADiceActor;
class USphereComponent;
class AFarklePlayerState;
class USpringArmComponent;
class UTextRenderComponent;
class UStaticMeshComponent;
class UMovementMonitorComponent;
class AController;

//Declare delegate for end turn with no parameters
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEndTurn);


UCLASS()
class FARKLE_API ABaseBoardPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ABaseBoardPawn();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BoardPawn")
	bool bIsAI;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Components)
	UStaticMeshComponent *BoardMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Components)
	TArray<USphereComponent *> HolderPositions;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Components)
	TArray<USphereComponent *> ThrowPositions;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Components)
	UMovementMonitorComponent *MovementMonitor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Components)
	TSubclassOf<ADiceActor> BP_DiceActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Components)
	TArray<ADiceActor *> DiceArray;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Texts")
	UTextRenderComponent *ScoreText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Texts")
	UTextRenderComponent *TurnScoreText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Texts")
	UTextRenderComponent *SelectedScoreText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerState")
	AFarklePlayerState *FarklePlayerState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BoardPawn")
	FVector ThrowDirection;

	UPROPERTY(BlueprintAssignable, Category = "Board Events")
	FOnEndTurn OnEndTurn;

	UFUNCTION(BlueprintImplementableEvent)
	void StartTurn();

	UFUNCTION(BlueprintCallable)
	void EndTurn();

	UFUNCTION(BlueprintCallable)
	void SpawnDices();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerEndTurn();

	UFUNCTION(Client, Reliable)
	void ClientEndTurn();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerThrowDices(const TArray<ADiceActor*> &Dices, const TArray<USphereComponent*> &ThrowFrom);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSpawnDices();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void PossessedBy(AController *NewController) override;

public:

	UFUNCTION(BlueprintCallable, Category = "BoardPawn")
	void RollDice();

	UFUNCTION(BlueprintCallable, Category = "BoardPawn")
	void PassDice();

private:
	UFUNCTION()
	void OnDiceSelected(ADiceActor* DiceActor);

	UFUNCTION()
	void OnAllActorsStopped();

	UFUNCTION()
	void DeselectAllDices();

	UFUNCTION()
	void ThrowDices(TArray<ADiceActor*> Dices, TArray<USphereComponent*> &ThrowFrom);

	UFUNCTION()
	void HoldDices(TArray<ADiceActor*> Dices, TArray<USphereComponent*> &HoldTo, int32 Index);

	UFUNCTION()
	void UpdateTexts();

	UFUNCTION(BlueprintCallable, Category = "BoardPawn", meta = (AllowPrivateAccess = "true"))
	TArray<ADiceActor*> GetNotSelectedDices();

	UPROPERTY(BlueprintReadOnly, Category = "BoardPawn", meta = (AllowPrivateAccess = "true"))
	TArray<ADiceActor*> SelectedDices;

	UPROPERTY(BlueprintReadOnly, Category = "BoardPawn", meta = (AllowPrivateAccess = "true"))
	TArray<ADiceActor*> OnHolderDices;
};
