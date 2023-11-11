// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TransformToComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FARKLE_API UTransformToComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTransformToComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transform")
	FVector TargetPosition;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transform")
	FRotator TargetRotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transform")
	float Duration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transform")
	bool bInterpolateRotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transform")
	bool bInterpolatePosition;

	UFUNCTION(BlueprintCallable, Category = "Transform")
	void StartTransform(FVector NewTargetPosition, FRotator NewTargetRotation, float NewTransitionDuration);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


private:

	FVector InitialPosition;
	FRotator InitialRotation;
	float ElapsedTime;
		
};
