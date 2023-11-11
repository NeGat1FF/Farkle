// Fill out your copyright notice in the Description page of Project Settings.


#include "TransformToComponent.h"

// Sets default values for this component's properties
UTransformToComponent::UTransformToComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
	Duration = 1.0f;
    ElapsedTime = 0.0f;
    bInterpolatePosition = true;
    bInterpolateRotation = true;
}

void UTransformToComponent::StartTransform(FVector NewTargetPosition, FRotator NewTargetRotation, float NewTransitionDuration)
{
	TargetPosition = NewTargetPosition;
    TargetRotation = NewTargetRotation;
    Duration = NewTransitionDuration;
    ElapsedTime = 0.0f;
    InitialPosition = GetOwner()->GetActorLocation();
    InitialRotation = GetOwner()->GetActorRotation();

    // Disable physics
    Cast<UStaticMeshComponent>(GetOwner()->GetRootComponent())->SetSimulatePhysics(false);
    // Disable collision
    Cast<UStaticMeshComponent>(GetOwner()->GetRootComponent())->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    SetComponentTickEnabled(true);
}

// Called when the game starts
void UTransformToComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	InitialPosition = GetOwner()->GetActorLocation();
    InitialRotation = GetOwner()->GetActorRotation();

	SetComponentTickEnabled(false);
}


// Called every frame
void UTransformToComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
	ElapsedTime += DeltaTime;
    float Alpha = FMath::Clamp(ElapsedTime / Duration, 0.0f, 1.0f);

    if (bInterpolatePosition)
    {
        FVector NewPosition = FMath::Lerp(InitialPosition, TargetPosition, Alpha);
        GetOwner()->SetActorLocation(NewPosition);
    }

    if (bInterpolateRotation)
    {
        FRotator NewRotation = FMath::Lerp(InitialRotation, TargetRotation, Alpha);
        GetOwner()->SetActorRotation(NewRotation);
    }

    if (Alpha >= 1.0f)
    {
        SetComponentTickEnabled(false);
        // Enable physics
        Cast<UStaticMeshComponent>(GetOwner()->GetRootComponent())->SetSimulatePhysics(true);
        // Enable collision
        Cast<UStaticMeshComponent>(GetOwner()->GetRootComponent())->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    }

}

