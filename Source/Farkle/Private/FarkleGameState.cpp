// Fill out your copyright notice in the Description page of Project Settings.

#include "FarkleGameState.h"
#include "FarklePlayerState.h"
#include "Logging/StructuredLog.h"
#include "Kismet/GameplayStatics.h"
#include "BaseBoardPawn.h"

AFarkleGameState::AFarkleGameState()
{

}

void AFarkleGameState::HandleBeginPlay()
{
    // Call the base class
    Super::HandleBeginPlay();
}

void AFarkleGameState::EndPlayerTurn(AFarklePlayerState *PlayerState, AFarklePlayerState *NextPlayerState)
{

}

void AFarkleGameState::onPlayerEndTurn()
{
}

void AFarkleGameState::onAIEndTurn()
{

}
