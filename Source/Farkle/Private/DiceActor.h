// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DiceActor.generated.h"

class UAudioComponent;
class UStaticMeshComponent;
class UDiceSphereComponent;
class UTransformToComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDiceSelected, ADiceActor*, DiceActor);

UCLASS()
class ADiceActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADiceActor();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Components)
	UStaticMeshComponent* DiceMesh;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Components)
	TArray<UDiceSphereComponent*> DiceFaces;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Components)
	UTransformToComponent* TransformToComponent;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Components)
	UAudioComponent* HitSound;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Components)
	UAudioComponent* RollingSound;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Components)
	UAudioComponent* SelectSound;

    UPROPERTY(BlueprintAssignable, Category = "Dice Events")
    FOnDiceSelected OnDiceSelected;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Dice")
	bool bIsSelected;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Dice")
	bool bIsOnHold;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	virtual void NotifyActorEndOverlap(AActor* OtherActor) override;

	virtual void NotifyHit(class UPrimitiveComponent* MyComp, AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit);

public:

	UFUNCTION(BlueprintCallable)
	void Select(bool shouldPlaySound = true);

	UFUNCTION(BlueprintCallable)
	void Deselect(bool shouldPlaySound = true);

	UFUNCTION(BlueprintCallable)
	void OnClick(AActor* actor, FKey Key);

	UFUNCTION(BlueprintCallable)
	void TransformTo(FVector NewTargetPosition, FRotator NewTargetRotation, float NewTransitionDuration);

	UFUNCTION(BlueprintCallable)
	int32 GetValue() const { return Value; }


private:

	UPROPERTY()
	int32 Value;

	UPROPERTY()
	bool bIsOverlapping;

	UPROPERTY()
	bool bIsRolling;

	UFUNCTION()
	void PlayRollingSound();

	UFUNCTION()
	void SetSelected(bool NewIsSelected);

	UFUNCTION()
	void UpdateValue(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	                    int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
