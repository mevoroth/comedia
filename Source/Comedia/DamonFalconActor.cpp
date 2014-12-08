// Fill out your copyright notice in the Description page of Project Settings.

#include "Comedia.h"
#include "DamonFalconActor.h"
#include "LiyaCharacter.h"

ADamonFalconActor::ADamonFalconActor(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	RotatedObject = 0;
	PrimaryActorTick.bCanEverTick = true;
}

void ADamonFalconActor::ReceiveTick(float DeltaSeconds)
{
	check(RotatedObject); // Set by BP

	Super::ReceiveTick(DeltaSeconds);

	TActorIterator<ALiyaCharacter> it(GetWorld());
	check(it); // Liya not in scene
	//it->SetActorLocation()
	SetActorLocation(it->GetActorLocation());

	AddActorLocalRotation(RotationSpeed * DeltaSeconds);
	//RotatedObject->AddLocalRotation(RotationSpeed);
	//AddActorLocalRotation(RotationSpeed * DeltaSeconds);
}
