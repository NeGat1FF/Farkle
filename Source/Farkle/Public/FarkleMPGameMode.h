// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FarkleGameMode.h"
#include "FarkleMPGameMode.generated.h"

class ABaseBoardPawn;

/**
 * 
 */
UCLASS()
class FARKLE_API AFarkleMPGameMode : public AFarkleGameMode
{
	GENERATED_BODY()

protected:
	virtual void PostLogin(APlayerController *NewPlayer) override;
	
};
