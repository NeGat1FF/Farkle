// Fill out your copyright notice in the Description page of Project Settings.


#include "MyButton.h"

UMyButton::UMyButton(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    // Set the default color
    SetColorAndOpacity(FLinearColor::White);

    // Bind the hover events
    OnHovered.AddDynamic(this, &UMyButton::OnHoveredFunc);
    OnUnhovered.AddDynamic(this, &UMyButton::OnUnhoveredFunc);
}

void UMyButton::OnHoveredFunc()
{
    SetColorAndOpacity(FLinearColor{0.5f, 1.0f, 0.0f, 1.0f});
}

void UMyButton::OnUnhoveredFunc()
{
    SetColorAndOpacity(FLinearColor::White);
}
