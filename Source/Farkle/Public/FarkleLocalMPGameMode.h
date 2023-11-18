// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FarkleGameMode.h"
#include "FarkleLocalMPGameMode.generated.h"

class ABaseBoardPawn;

/**
 * 
 */
UCLASS()
class FARKLE_API AFarkleLocalMPGameMode : public AFarkleGameMode
{
	GENERATED_BODY()
	
protected:
	virtual void PostLogin(APlayerController* NewPlayer) override;

	virtual void OnFirstPlayerEndTurn() override;

	virtual void OnSecondPlayerEndTurn() override;
};
