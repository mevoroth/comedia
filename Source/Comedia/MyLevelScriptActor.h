// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/LevelScriptActor.h"
#include "Comedia/KnifeCharacter.h"
#include "MyLevelScriptActor.generated.h"

/**
 * 
 */
UCLASS()
class COMEDIA_API AMyLevelScriptActor : public ALevelScriptActor
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TweakingValues")
	float RadiusSpawnKnifeArea;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TweakingValues")
	float DelayFirstKnifeSpawn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TweakingValues")
	float DelayBetweenKnifeSpawn;

	virtual void Tick(float DeltaSeconds) override;

private:
	/** Class to instance when spawning Knife character */
	TSubclassOf<AKnifeCharacter> KnifeClass;

	float TotalElapsedTime;
};
