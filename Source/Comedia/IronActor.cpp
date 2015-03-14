// Fill out your copyright notice in the Description page of Project Settings.

#include "Comedia.h"
#include "IwacLevelScriptActor.h"
#include "IronActor.h"

AIronActor::AIronActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	bInverseRotation = false;
}

void AIronActor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	AddActorLocalRotation(FRotator(0.0f, RotationSpeed * DeltaSeconds * ((bInverseRotation) ? -1.0f : 1.0f), 0.0f));
}

void AIronActor::UpdateFirstSpawnState()
{
	UWorld* World = GetWorld();
	if (World)
	{
		AIwacLevelScriptActor* LevelScriptActor = Cast<AIwacLevelScriptActor>(World->GetLevelScriptActor());
		if (LevelScriptActor)
		{
			bFirstSpawn = LevelScriptActor->bFirstIronSpawn;
		}
	}
}