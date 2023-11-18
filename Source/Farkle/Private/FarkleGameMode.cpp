// Fill out your copyright notice in the Description page of Project Settings.


#include "FarkleGameMode.h"
#include "BaseBoardPawn.h"

AFarkleGameMode::AFarkleGameMode()
{
    TargetScore = 2500;
}

void AFarkleGameMode::SetTargetScore(int32 Score)
{
    if(Score > 0){
        TargetScore = Score;
    }
}

int32 AFarkleGameMode::GetTargetScore() const
{
    return TargetScore;
}

void AFarkleGameMode::BindEvents()
{
    if(FirstPlayer && SecondPlayer){
        FirstPlayer->OnEndTurn.AddDynamic(this, &AFarkleGameMode::OnFirstPlayerEndTurn);
        SecondPlayer->OnEndTurn.AddDynamic(this, &AFarkleGameMode::OnSecondPlayerEndTurn);

        FTimerHandle Handle;
        GetWorldTimerManager().SetTimer(Handle, SecondPlayer, &ABaseBoardPawn::EndTurn, 0.5f, false);
    }
}

void AFarkleGameMode::OnFirstPlayerEndTurn()
{
    SecondPlayer->StartTurn();

    if(FirstPlayer->GetTotalScore() >= TargetScore){
        EndGame(FirstPlayer, SecondPlayer);
    }
}

void AFarkleGameMode::OnSecondPlayerEndTurn()
{
    FirstPlayer->StartTurn();

    if(SecondPlayer->GetTotalScore() >= TargetScore){
        EndGame(SecondPlayer, FirstPlayer);
    }
}

void AFarkleGameMode::SetFirstPlayer(ABaseBoardPawn *Pawn)
{
    if(Pawn){
        FirstPlayer = Pawn;
    }
}

void AFarkleGameMode::SetSecondPlayer(ABaseBoardPawn *Pawn)
{
    if(Pawn){
        SecondPlayer = Pawn;
    }
}

ABaseBoardPawn *AFarkleGameMode::GetFirstPlayer() const
{
    return FirstPlayer;
}

ABaseBoardPawn *AFarkleGameMode::GetSecondPlayer() const
{
    return SecondPlayer;
}
