// Fill out your copyright notice in the Description page of Project Settings.

#include "FarkleMPGameMode.h"
#include "BaseBoardPawn.h"
#include "Kismet/GameplayStatics.h"

void AFarkleMPGameMode::PostLogin(APlayerController *NewPlayer)
{
    Super::PostLogin(NewPlayer);

    TArray<AActor *> PawnList;

    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABaseBoardPawn::StaticClass(), PawnList);
    for (AActor *Actor : PawnList)
    {
        ABaseBoardPawn *Pawn = Cast<ABaseBoardPawn>(Actor);
        if (!Pawn->IsPawnControlled())
        {
            NewPlayer->Possess(Pawn);
            Pawn->SetOwner(NewPlayer);
            if (!GetFirstPlayer())
            {
                SetFirstPlayer(Pawn);
                break;
            }
            else
            {
                SetSecondPlayer(Pawn);
                BindEvents();
                break;
            }
        }
    }
}
