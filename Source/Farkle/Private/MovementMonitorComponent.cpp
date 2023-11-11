// Fill out your copyright notice in the Description page of Project Settings.


#include "MovementMonitorComponent.h"

// Sets default values for this component's properties
UMovementMonitorComponent::UMovementMonitorComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
	bIsTracking = false;
}

void UMovementMonitorComponent::AddActorToTrack(AActor *ActorToAdd)
{
	if(ActorToAdd)
	{
		ActorsToTrack.Add(ActorToAdd);
	}
}

void UMovementMonitorComponent::RemoveActorToTrack(AActor *ActorToRemove)
{
	if(ActorToRemove)
	{
		ActorsToTrack.Remove(ActorToRemove);
	}
}

void UMovementMonitorComponent::StartTracking()
{
	bIsTracking = true;
	SetComponentTickEnabled(true);
}

// Called when the game starts
void UMovementMonitorComponent::BeginPlay()
{
	Super::BeginPlay();

	SetComponentTickEnabled(false);
}

// Called every frame
void UMovementMonitorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
	if (bIsTracking)
	{
		bool isAllStopped = true;
		for (AActor *ActorToTrack : ActorsToTrack)
		{
			int32 ActorLocation = ActorToTrack->GetVelocity().Size();
			if (ActorLocation > 0)
			{
				isAllStopped = false;
				break;
			}
		}

		if (isAllStopped)
		{
			bIsTracking = false;
			OnAllActorsStopped.Broadcast();
			ActorsToTrack.Empty();
			SetComponentTickEnabled(false);
		}
	}
}
