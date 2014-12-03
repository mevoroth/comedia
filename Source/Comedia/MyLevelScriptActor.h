// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/LevelScriptActor.h"
#include "MyLevelScriptActor.generated.h"

/**
 * 
 */
UCLASS()
class COMEDIA_API AMyLevelScriptActor : public ALevelScriptActor
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = SolusLevel)
	FName SolusLevelName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TweakingValues")
	float RadiusSpawnKnifeArea;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TweakingValues")
	float DelayFirstKnifeSpawn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TweakingValues")
	float DelayBetweenKnifeSpawn;
};
