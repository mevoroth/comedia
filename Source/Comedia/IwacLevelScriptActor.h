// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/LevelScriptActor.h"
#include "Comedia/KnifeCharacter.h"
#include "IwacLevelScriptActor.generated.h"

/**
 * 
 */
UCLASS()
class COMEDIA_API AIwacLevelScriptActor : public ALevelScriptActor
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "KnifePhase")
	float DelayFirstKnifeSpawn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "KnifePhase")
	float DelayBetweenKnifeSpawn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "KnifePhase")
	float RadiusSpawnKnifeArea;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "KnifePhase")
	int32 NbSpawnedKnife;

	/** ComputedRadiusSpawnKnifeArea depending of player character height */
	float ComputedRadiusSpawnKnifeArea;

	/** true indicates a knife is present */
	bool bHasKnifeSpawned;

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

private:
	/** Class to instance when spawning Knife character */
	TSubclassOf<AKnifeCharacter> KnifeClass;

	float RemainingTime;
	ACharacter* PlayerCharacter;

	void KnifeSpawning(float ComputedRadiusSpawnKnifeArea);
};
