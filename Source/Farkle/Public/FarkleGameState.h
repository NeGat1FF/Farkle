// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "FarkleGameState.generated.h"

// Create delegate for game over
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGameOver, bool, bIsHumanWinner);

class AFarklePlayerState;

/**
 * 
 */
UCLASS()
class FARKLE_API AFarkleGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	AFarkleGameState();

	UFUNCTION(BlueprintCallable, Category = "Game State")
	virtual void HandleBeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "Game State")
	void EndPlayerTurn(AFarklePlayerState *PlayerState, AFarklePlayerState *NextPlayerState);

	UFUNCTION(BlueprintCallable, Category = "Game State")
	void onPlayerEndTurn();

	UFUNCTION(BlueprintCallable, Category = "Game State")
	void onAIEndTurn();

	UPROPERTY(BlueprintReadWrite, Category = "Game State")
	AFarklePlayerState *HumanPlayerState;

	UPROPERTY(BlueprintReadWrite, Category = "Game State")
	AFarklePlayerState *AIPlayerState;

	UPROPERTY(BlueprintAssignable, Category = "Game State")
	FOnGameOver OnGameOver;
	
};
