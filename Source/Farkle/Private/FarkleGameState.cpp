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

    UE_LOGFMT(LogTemp, Log, "PlayerArray length: {0}", PlayerArray.Num());

    for (APlayerState *PlayerState : PlayerArray)
    {
        AFarklePlayerState *FarklePlayerState = Cast<AFarklePlayerState>(PlayerState);
        if (Cast<ABaseBoardPawn>(PlayerState->GetPawn())->bIsAI)
        {
            FarklePlayerState->OnPlayerEndTurn.AddDynamic(this, &AFarkleGameState::onAIEndTurn);
            AIPlayerState = FarklePlayerState;
        }
        else
        {
            FarklePlayerState->OnPlayerEndTurn.AddDynamic(this, &AFarkleGameState::onPlayerEndTurn);
            HumanPlayerState = FarklePlayerState;
        }
    }

    HumanPlayerState->StartTurn();
}

void AFarkleGameState::EndPlayerTurn(AFarklePlayerState *PlayerState, AFarklePlayerState *NextPlayerState)
{

    if(PlayerState->GetScore() >= 2500){
        OnGameOver.Broadcast(PlayerState == HumanPlayerState);
        return;
    }

    FTimerHandle TimerHandle;
    // Timer delegate
    FTimerDelegate TimerDelegate;
    // Bind the timer delegate to the function
    TimerDelegate.BindLambda([=]() {
        NextPlayerState->StartTurn();
    });
    // Set the timer
    if(NextPlayerState == AIPlayerState){
        GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, 2.0f, false);
    }
    else{
        NextPlayerState->StartTurn();
    }

    PlayerState->SetPlayerState(EPlayerState::EPS_WaitingForOpponent);
}

void AFarkleGameState::onPlayerEndTurn()
{
    EndPlayerTurn(HumanPlayerState, AIPlayerState);
}

void AFarkleGameState::onAIEndTurn()
{
    EndPlayerTurn(AIPlayerState, HumanPlayerState);
}
