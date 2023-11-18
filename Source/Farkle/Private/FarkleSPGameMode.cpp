// Fill out your copyright notice in the Description page of Project Settings.


#include "FarkleSPGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "BaseBoardPawn.h"

void AFakrleSPGameMode::PostLogin(APlayerController *NewPlayer)
{
    Super::PostLogin(NewPlayer);

    TArray<AActor *> PawnList;

    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABaseBoardPawn::StaticClass(), PawnList);

    for (AActor *Actor : PawnList)
    {
        ABaseBoardPawn *Pawn = Cast<ABaseBoardPawn>(Actor);
        if (Pawn->bIsAI)
        {
            SetSecondPlayer(Pawn);
        }
        else
        {
            SetFirstPlayer(Pawn);
        }
    }

    BindEvents();
}