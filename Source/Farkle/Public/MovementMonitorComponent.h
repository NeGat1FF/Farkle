// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MovementMonitorComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAllActorsStopped);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FARKLE_API UMovementMonitorComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMovementMonitorComponent();

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void AddActorToTrack(AActor* ActorToAdd);

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void RemoveActorToTrack(AActor* ActorToRemove);

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void StartTracking();

	UPROPERTY(BlueprintAssignable, Category = "Movement")
	FOnAllActorsStopped OnAllActorsStopped;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	UPROPERTY()
	TArray<AActor*> ActorsToTrack;

	bool bIsTracking;
		
};
