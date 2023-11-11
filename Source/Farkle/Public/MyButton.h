// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Button.h"
#include "MyButton.generated.h"

/**
 * 
 */
UCLASS()
class FARKLE_API UMyButton : public UButton
{
	GENERATED_BODY()

public:
	UMyButton(const FObjectInitializer& ObjectInitializer);

private:
    UFUNCTION()
	void OnHoveredFunc();

	UFUNCTION()
	void OnUnhoveredFunc();
	
};
