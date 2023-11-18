// Fill out your copyright notice in the Description page of Project Settings.


#include "DiceActor.h"
#include "Net/UnrealNetwork.h"
#include "DiceSphereComponent.h"
#include "TransformToComponent.h"
#include "Components/AudioComponent.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
ADiceActor::ADiceActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	DiceMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DiceMesh"));
	SetRootComponent(DiceMesh);

	HitSound = CreateDefaultSubobject<UAudioComponent>(TEXT("HitSound"));
	HitSound->SetupAttachment(DiceMesh);

	RollingSound = CreateDefaultSubobject<UAudioComponent>(TEXT("RollingSound"));
	RollingSound->SetupAttachment(DiceMesh);

	RollingSound->OnAudioFinished.AddDynamic(this, &ADiceActor::PlayRollingSound);

	SelectSound = CreateDefaultSubobject<UAudioComponent>(TEXT("SelectSound"));
	SelectSound->SetupAttachment(DiceMesh);

	TransformToComponent = CreateDefaultSubobject<UTransformToComponent>(TEXT("TransformToComponent"));

	for(int i = 1; i <= 6; i++)
	{
		FString ComponentName = FString::Printf(TEXT("DiceFace%d"), i);
		UDiceSphereComponent* DiceFace = CreateDefaultSubobject<UDiceSphereComponent>(*ComponentName);
		DiceFace->SetupAttachment(DiceMesh);
		DiceFace->SetValue(i);

		DiceFace->OnComponentBeginOverlap.AddDynamic(this, &ADiceActor::UpdateValue);

		DiceFaces.Add(DiceFace);
	}

	bIsOnHold = true;
	bIsRolling = false;
	bIsSelected = false;
	bIsOverlapping = false;

	OnClicked.AddDynamic(this, &ADiceActor::OnClick);

	// Disable physics simulation
	DiceMesh->SetSimulatePhysics(false);
}

void ADiceActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADiceActor, bIsSelected);
	DOREPLIFETIME(ADiceActor, bIsOnHold);
}

void ADiceActor::NotifyActorBeginOverlap(AActor *OtherActor)
{
	if(ADiceActor* Dice = Cast<ADiceActor>(OtherActor))
	{
		return;
	}
	bIsOverlapping = true;

	if(GetVelocity().Size() > 0){
		bIsRolling = true;
	}

	PlayRollingSound();
}

void ADiceActor::NotifyActorEndOverlap(AActor *OtherActor)
{
	if(ADiceActor* Dice = Cast<ADiceActor>(OtherActor))
	{
		return;
	}
	bIsOverlapping = false;
}

void ADiceActor::NotifyHit(UPrimitiveComponent *MyComp, AActor *Other, UPrimitiveComponent *OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult &Hit)
{
	if(ADiceActor* Dice = Cast<ADiceActor>(Other))
	{
		return;
	}
	if(!bIsOnHold && !HitSound->IsPlaying())
	{
		HitSound->Play();
	}
}

void ADiceActor::Select(bool shouldPlaySound)
{
	SetSelected(true);

	if(shouldPlaySound)
	{
		SelectSound->Play();
	}
}

void ADiceActor::Deselect(bool shouldPlaySound)
{
	SetSelected(false);

	if(shouldPlaySound)
	{
		SelectSound->Play();
	}
}

void ADiceActor::OnClick(AActor* actor, FKey Key)
{
	double Velocity = GetVelocity().Size();
	// If dice is not rolling and is not on hold
	if(Velocity < 1 && !bIsOnHold)
	{
		// If dice is selected, deselect it
		bIsSelected ? Deselect() : Select();
	}
}

void ADiceActor::TransformTo(FVector NewTargetPosition, FRotator NewTargetRotation, float NewTransitionDuration)
{
	TransformToComponent->StartTransform(NewTargetPosition, NewTargetRotation, NewTransitionDuration);
}

void ADiceActor::PlayRollingSound()
{
	if(GetVelocity().Size() == 0){
		bIsRolling = false;
		RollingSound->Stop();
		return;
	}

	if(bIsRolling && bIsOverlapping && !RollingSound->IsPlaying() && DiceMesh->IsSimulatingPhysics())
	{
		RollingSound->Play();
	}
}

void ADiceActor::SetSelected(bool NewIsSelected)
{
	if(GetLocalRole() < ROLE_Authority){
		ServerSetSelected(NewIsSelected);
		return;
	}
	MulticastSetSelected(NewIsSelected);
}

void ADiceActor::ServerSetSelected_Implementation(bool NewIsSelected){
	SetSelected(NewIsSelected);
}

void ADiceActor::MulticastSetSelected_Implementation(bool NewIsSelected)
{
	bIsSelected = NewIsSelected;
	DiceMesh->SetRenderCustomDepth(NewIsSelected);
	OnDiceSelected.Broadcast(this);
}

void ADiceActor::UpdateValue(UPrimitiveComponent *OverlappedComp, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
{
	if(ADiceActor* Dice = Cast<ADiceActor>(OtherActor)){
		return;
	}
	Value = Cast<UDiceSphereComponent>(OverlappedComp)->GetValue();
}
