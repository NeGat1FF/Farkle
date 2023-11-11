// Fill out your copyright notice in the Description page of Project Settings.


#include "FarklePlayerState.h"
#include "BaseBoardPawn.h"
#include "Net/UnrealNetwork.h"

AFarklePlayerState::AFarklePlayerState()
{
    TurnScore = 0;
    SelectedScore = 0;

    bIsMyTurn = false;
    PlayerState = EPlayerState::EPS_WaitingForOpponent;
}

void AFarklePlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AFarklePlayerState, PlayerState);
    DOREPLIFETIME(AFarklePlayerState, TurnScore);
    DOREPLIFETIME(AFarklePlayerState, SelectedScore);
    DOREPLIFETIME(AFarklePlayerState, bIsMyTurn);
}

void AFarklePlayerState::EndTurn()
{
    if(GetLocalRole() < ROLE_Authority)
    {
        ServerEndTurn();
        return;
    }

    AddTurnScore();
    SetScore(GetScore() + TurnScore);
    SetPlayerState(EPlayerState::EPS_WaitingForOpponent);

    TurnScore = 0;
    bIsMyTurn = false;

    
    OnPlayerEndTurn.Broadcast();
}

void AFarklePlayerState::ServerEndTurn_Implementation()
{
    EndTurn();
}

void AFarklePlayerState::StartTurn()
{
    if(GetLocalRole() < ROLE_Authority){
        ServerStartTurn();
        return;
    }
    else{
        ClientHandleStartTurn();
    }

    SetPlayerState(EPlayerState::EPS_WaitingForThrow);
    bIsMyTurn = true;

    Cast<ABaseBoardPawn>(GetPawn())->StartTurn();

    OnPlayerStartTurn.Broadcast();
}

void AFarklePlayerState::ServerStartTurn_Implementation()
{
    StartTurn();
}

void AFarklePlayerState::ClientHandleStartTurn_Implementation()
{
    // Client-side logic to handle the start of the turn, like updating the UI
    Cast<ABaseBoardPawn>(GetPawn())->StartTurn();
}

void AFarklePlayerState::ClientHandleEndTurn_Implementation()
{
    // Client-side logic to handle the start of the turn, like updating the UI
    Cast<ABaseBoardPawn>(GetPawn())->EndTurn();
}

void AFarklePlayerState::SetPlayerState(EPlayerState NewPlayerState)
{
    PlayerState = NewPlayerState;

    OnStateChanged.Broadcast(PlayerState);
}

EPlayerState AFarklePlayerState::GetPlayerState() const
{
    return PlayerState;
}

void AFarklePlayerState::AddTurnScore()
{
    TurnScore += SelectedScore;
    SelectedScore = 0;
}

void AFarklePlayerState::UpdateSelectedScore(int32 SelectedScoreToAdd)
{
    SelectedScore = SelectedScoreToAdd;
}