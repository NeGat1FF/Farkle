// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "DiceSphereComponent.generated.h"

/**
 * 
 */
UCLASS()
class FARKLE_API UDiceSphereComponent : public USphereComponent
{
	GENERATED_BODY()
private:
	int32 Value = 0;
public:

	UFUNCTION(BlueprintCallable, Category = "Dice")
	int32 GetValue() const;

	UFUNCTION(BlueprintCallable, Category = "Dice")
	void SetValue(int32 NewFaceValue);
	
};
