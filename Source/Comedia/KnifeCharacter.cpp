// Fill out your copyright notice in the Description page of Project Settings.

#include "Comedia.h"
#include "KnifeCharacter.h"


AKnifeCharacter::AKnifeCharacter(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
}

void AKnifeCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}
