// Fill out your copyright notice in the Description page of Project Settings.

#include "FarkleLocalMPGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "BaseBoardPawn.h"

void AFarkleLocalMPGameMode::OnFirstPlayerEndTurn()
{
    Super::OnFirstPlayerEndTurn();

    ToggleClickEvents(GetFirstPlayer(), true);

    Cast<APlayerController>(GetFirstPlayer()->GetController())->SetViewTargetWithBlend(GetSecondPlayer(), 2.0f, EViewTargetBlendFunction::VTBlend_Cubic);
}

void AFarkleLocalMPGameMode::OnSecondPlayerEndTurn()
{
    Super::OnSecondPlayerEndTurn();

    Cast<APlayerController>(GetFirstPlayer()->GetController())->SetViewTargetWithBlend(GetFirstPlayer(), 2.0f, EViewTargetBlendFunction::VTBlend_Cubic);
}

void AFarkleLocalMPGameMode::PostLogin(APlayerController *NewPlayer)
{
    Super::PostLogin(NewPlayer);

    if(GetNumPlayers() == 1){
        SetFirstPlayer(Cast<ABaseBoardPawn>(NewPlayer->GetPawn()));
    }

    if (GetNumPlayers() == 2)
    {
        SetSecondPlayer(Cast<ABaseBoardPawn>(NewPlayer->GetPawn()));
        BindEvents(); 
    }
}
