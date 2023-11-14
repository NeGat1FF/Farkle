// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include <map>
#include "BaseBoardPawn.generated.h"

class ADiceActor;
class USphereComponent;
class AFarklePlayState;
class USpringArmComponent;
class UTextRenderComponent;
class UStaticMeshComponent;
class UMovementMonitorComponent;
class AController;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStartTurn);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEndTurn);

UENUM(BlueprintType)
enum class EPlayState : uint8
{
	EPS_WaitingForThrow		UMETA(DisplayName = "Waiting for throw"),
	EPS_Rolling				UMETA(DisplayName = "Rolling"),
	EPS_Selecting			UMETA(DisplayName = "Selecting"),
	EPS_Selected			UMETA(DisplayName = "Selected"),
	EPS_WaitingForOpponent	UMETA(DisplayName = "Waiting for opponent"),
	EPS_GameOver			UMETA(DisplayName = "Game over")
};

UCLASS()
class FARKLE_API ABaseBoardPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ABaseBoardPawn();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BoardPawn")
	bool bIsAI;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "BoardPawn")
	bool bIsMyTurn;

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

	UPROPERTY(Replicated, BlueprintReadOnly, Category = Components)
	TArray<ADiceActor *> DiceArray;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Texts")
	UTextRenderComponent *ScoreText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Texts")
	UTextRenderComponent *TurnScoreText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Texts")
	UTextRenderComponent *SelectedScoreText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BoardPawn")
	FVector ThrowDirection;

	UPROPERTY(BlueprintAssignable, Category = "Board Events")
	FOnEndTurn OnEndTurn;

	UPROPERTY(BlueprintAssignable, Category = "Board Events")
	FOnStartTurn OnStartTurn;

	UFUNCTION(BlueprintCallable, Category = "PlayState")
	void SetPlayState(EPlayState NewPlayState);

	UFUNCTION(BlueprintCallable, Category = "PlayState")
	EPlayState GetPlayState() const;

	UFUNCTION(BlueprintGetter, Category = "Score")
	int32 GetTotalScore() const;

	UFUNCTION(BlueprintGetter, Category = "Score")
	int32 GetTurnScore() const;

	UFUNCTION(BlueprintGetter, Category = "Score")
	int32 GetSelectedScore() const;

	UFUNCTION(BlueprintCallable)
	void StartTurn();

	UFUNCTION(BlueprintCallable)
	void EndTurn();

	UFUNCTION(BlueprintCallable)
	void SpawnDices(int32 PlayerId);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSpawnDices(int32 PlayerId);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerStartTurn();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerEndTurn();

	UFUNCTION(Client, Reliable)
	void ClientStartTurn();

	UFUNCTION(Client, Reliable)
	void ClientEndTurn();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerThrowDices(const TArray<ADiceActor*> &Dices);

public:

	UFUNCTION()
	void UpdateTexts();

	UFUNCTION(BlueprintCallable, Category = "BoardPawn")
	void RollDice();

	UFUNCTION(BlueprintCallable, Category = "BoardPawn")
	void PassDice();

	UFUNCTION(Server, Reliable)
	void ServerPassDice();

	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;

private:
	UFUNCTION()
	void OnDiceSelected(ADiceActor* DiceActor);

	UFUNCTION()
	void OnAllActorsStopped();

	UFUNCTION()
	void DeselectAllDices();

	UFUNCTION()
	void ThrowDices(TArray<ADiceActor*> Dices);

	UFUNCTION()
	void HoldDices(const TArray<ADiceActor*>& Dices,const TArray<USphereComponent*> &HoldTo, int32 Index);

	UFUNCTION(Server, Reliable)
	void ServerHoldDices(const TArray<ADiceActor*>& Dices,const TArray<USphereComponent*>& HoldTo, int32 Index);

	UFUNCTION(BlueprintCallable, Category = "BoardPawn", meta = (AllowPrivateAccess = "true"))
	TArray<ADiceActor*> GetNotSelectedDices();

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "BoardPawn", meta = (AllowPrivateAccess = "true"))
	TArray<ADiceActor*> SelectedDices;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "BoardPawn", meta = (AllowPrivateAccess = "true"))
	TArray<ADiceActor*> OnHolderDices;

	UPROPERTY(ReplicatedUsing = UpdateTexts, BlueprintGetter = GetTotalScore, Category = "Score")
	int32 TotalScore;

	UPROPERTY(ReplicatedUsing = UpdateTexts, BlueprintGetter = GetTurnScore, Category = "Score")
	int32 TurnScore;

	UPROPERTY(ReplicatedUsing = UpdateTexts, BlueprintGetter = GetSelectedScore, Category = "Score")
	int32 SelectedScore;

	UPROPERTY(Replicated)
	EPlayState PlayState;

};
