// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/LevelScriptActor.h"
#include "Comedia/KnifeCharacter.h"
#include "IwacLevelScriptActor.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
namespace ETorturePhase
{
	enum Type
	{
		TP_EmptyPhase       UMETA(DisplayName = "EmptyPhase"),
		TP_KnifePhase       UMETA(DisplayName = "KnifePhase"),
		TP_LightningPhase   UMETA(DisplayName = "LightningPhase"),
		TP_IronPhase        UMETA(DisplayName = "IronPhase"),
	};
}

UCLASS()
class COMEDIA_API AIwacLevelScriptActor : public ALevelScriptActor
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TorturePhase")
	TEnumAsByte<ETorturePhase::Type> TorturePhase;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TorturePhase")
	float DelayFirstSpawn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "KnifePhase")
	float DelayBetweenKnifeSpawn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "KnifePhase")
	float RadiusSpawnKnifeArea;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "KnifePhase")
	int32 NbSpawnedKnife;

	/** ComputedRadiusSpawnKnifeArea depending of player character height */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "KnifePhase")
	float ComputedRadiusSpawnKnifeArea;

	/** true indicates a knife is present */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "KnifePhase")
	bool bHasKnifeSpawned;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LightningPhase")
	int32 MaxNbLightning;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LightningPhase")
	float LengthLightningPhase;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LightningPhase")
	float RadiusSpawnLightningArea;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LightningPhase")
	float PercentCriticalLightning;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LightningPhase")
	float MinCooldownLightning;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LightningPhase")
	float MaxCooldownLightning;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LightningPhase")
	float DelayDamageLightning;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LightningPhase")
	float RadiusDamageLightningArea;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LightningPhase")
	int32 CurrentNbLightning;


	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

private:
	/** Class to instance when spawning Knife character */
	TSubclassOf<AKnifeCharacter> KnifeClass;

	float RemainingTime;
	ACharacter* PlayerCharacter;

	void TickKnifePhase(float DeltaSeconds);
	void TickLightningPhase(float DeltaSeconds);
	void KnifeSpawning(float ComputedRadiusSpawnKnifeArea);
};
