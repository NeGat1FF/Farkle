// Fill out your copyright notice in the Description page of Project Settings.


#include "DiceSphereComponent.h"

int32 UDiceSphereComponent::GetValue() const
{
    return Value;
}

void UDiceSphereComponent::SetValue(int32 NewValue)
{
    Value = NewValue;
}

