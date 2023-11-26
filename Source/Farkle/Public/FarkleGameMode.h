// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "FarkleGameMode.generated.h"

class ABaseBoardPawn;
class UUserWidget;

/**
 * 
 */
UCLASS()
class FARKLE_API AFarkleGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:

	AFarkleGameMode();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UUserWidget> GameOverWidgetClass;

	UFUNCTION(BlueprintCallable)
	void SetTargetScore(int32 Score);

	UFUNCTION(BlueprintCallable)
	int32 GetTargetScore() const;

protected:
	UFUNCTION(BlueprintImplementableEvent)
	void EndGame(ABaseBoardPawn* winner, ABaseBoardPawn* loser);

	virtual void BindEvents();

	UFUNCTION()
	void ToggleClickEvents(ABaseBoardPawn* Player, bool bEnable);

	UFUNCTION()
	virtual void OnFirstPlayerEndTurn();

	UFUNCTION()
	virtual void OnSecondPlayerEndTurn();

	UFUNCTION(BlueprintCallable)
	void SetFirstPlayer(ABaseBoardPawn* Pawn);

	UFUNCTION(BlueprintCallable)
	void SetSecondPlayer(ABaseBoardPawn* Pawn);

	UFUNCTION(BlueprintGetter)
	ABaseBoardPawn* GetFirstPlayer() const;

	UFUNCTION(BlueprintGetter)
	ABaseBoardPawn* GetSecondPlayer() const;

private:
	UPROPERTY(BlueprintGetter = GetTargetScore, BlueprintSetter = SetTargetScore)
	int32 TargetScore;

	UPROPERTY(BlueprintGetter = GetFirstPlayer, BlueprintSetter = SetFirstPlayer) 
	ABaseBoardPawn* FirstPlayer;

	UPROPERTY(BlueprintGetter = GetSecondPlayer, BlueprintSetter = SetSecondPlayer) 
	ABaseBoardPawn* SecondPlayer;
	
};
