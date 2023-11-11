// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "FarklePlayerState.generated.h"

//Game states enum
UENUM(BlueprintType)
enum class EPlayerState : uint8
{
	EPS_WaitingForThrow		UMETA(DisplayName = "Waiting for throw"),
	EPS_Rolling				UMETA(DisplayName = "Rolling"),
	EPS_Selecting			UMETA(DisplayName = "Selecting"),
	EPS_Selected			UMETA(DisplayName = "Selected"),
	EPS_WaitingForOpponent	UMETA(DisplayName = "Waiting for opponent"),
	EPS_GameOver			UMETA(DisplayName = "Game over")
};

// Declare delegate
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStateChanged, EPlayerState, PlayerState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerEndTurn);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerStartTurn);

/**
 * 
 */
UCLASS()
class FARKLE_API AFarklePlayerState : public APlayerState
{
	GENERATED_BODY()

public:

	AFarklePlayerState();

	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;

	// Declare the Server RPC for ending the turn
    UFUNCTION(Server, Reliable)
    void ServerEndTurn();

    // Declare the Server RPC for starting the turn
    UFUNCTION(Server, Reliable)
    void ServerStartTurn();

    // Client-specific function to handle the start of the turn
    UFUNCTION(Client, Reliable)
    void ClientHandleStartTurn();

	// Client-specific function to handle the end of the turn
	UFUNCTION(Client, Reliable)
	void ClientHandleEndTurn();

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "State")
	EPlayerState PlayerState;

	UPROPERTY(BlueprintAssignable, Category = "Turn")
	FOnPlayerStartTurn OnPlayerStartTurn;

	UPROPERTY(BlueprintAssignable, Category = "Turn")
	FOnPlayerEndTurn OnPlayerEndTurn;
	
	UPROPERTY(BlueprintAssignable, Category = "State")
	FOnStateChanged OnStateChanged;

	UFUNCTION(BlueprintCallable, Category = "State")
	void SetPlayerState(EPlayerState NewPlayerState);

	UFUNCTION(BlueprintCallable, Category = "State")
	EPlayerState GetPlayerState() const;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Score")
	int32 TurnScore;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Score")
	int32 SelectedScore;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Score")
	bool bIsMyTurn;

	UFUNCTION(BlueprintCallable, Category = "Turn")
	void EndTurn();

	UFUNCTION(BlueprintCallable, Category = "Turn")
	void StartTurn();

	UFUNCTION(BlueprintCallable, Category = "Score")
	void AddTurnScore();

	UFUNCTION(BlueprintCallable, Category = "Score")
	void UpdateSelectedScore(int32 SelectedScoreToAdd);
	
};
