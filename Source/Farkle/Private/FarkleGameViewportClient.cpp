// Fill out your copyright notice in the Description page of Project Settings.


#include "FarkleGameViewportClient.h"
#include "GameFramework/PlayerInput.h"

bool UFarkleGameViewportClient::InputKey(const FInputKeyEventArgs & EventArgs)
{
    Super::InputKey(EventArgs);

    TArray<APlayerController*> Controllers;
    GEngine->GetAllLocalPlayerControllers(Controllers);

    bool bResult = false;

    for (auto Controller : Controllers)
    {
        if(Controller->GetLocalPlayer() != GEngine->GetLocalPlayerFromInputDevice(this, EventArgs.InputDevice)){
            bResult = Controller->InputKey(FInputKeyParams(EventArgs.Key, EventArgs.Event, static_cast<double>(EventArgs.AmountDepressed), EventArgs.IsGamepad(), EventArgs.InputDevice));
        }
    }

    return bResult;
}
