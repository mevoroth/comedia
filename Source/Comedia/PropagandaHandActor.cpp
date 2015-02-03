// Fill out your copyright notice in the Description page of Project Settings.

#include "Comedia.h"
#include "PropagandaHandActor.h"

APropagandaHandActor::APropagandaHandActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	//Add Arrow Component to see propaganda hand orientation
	ForwardArrowComponent = ObjectInitializer.CreateDefaultSubobject<UArrowComponent>(this, TEXT("ForwardArrowComponent"));
	RootComponent = ForwardArrowComponent;

	//Add SphereTriggerZoneComponent to detect player presence
	SphereTriggerZoneComponent = ObjectInitializer.CreateDefaultSubobject<USphereComponent>(this, TEXT("SphereTriggerZoneComponent"));
	SphereTriggerZoneComponent->AttachTo(RootComponent);

	PrimaryActorTick.bCanEverTick = true;
}