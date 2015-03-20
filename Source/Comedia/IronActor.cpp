// Fill out your copyright notice in the Description page of Project Settings.

#include "Comedia.h"
#include "IwacLevelScriptActor.h"
#include "IronActor.h"
#include "LiyaCharacter.h"

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

FName AIronActor::GetSpatialization() const
{
	ALiyaCharacter* Character = Cast<ALiyaCharacter>(GetWorld()->GetFirstPlayerController()->GetCharacter());

	if (Character)
	{
		FVector Dir = (GetLiyaTree() - GetActorLocation()).SafeNormal();
		FVector LiyaDir = (Character->GetActorLocation() - GetLiyaTree()).SafeNormal();
		float DotDir = FVector::DotProduct(Dir, LiyaDir);
		float DotDirLeft = FVector::DotProduct(Dir.RotateAngleAxis(5.f, FVector::UpVector), LiyaDir);
		if (DotDirLeft < DotDir)
		{
			return FName(TEXT("Left"));
		}
		else
		{
			return FName(TEXT("Right"));
		}
	}

	return TEXT("");
}
