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
};
