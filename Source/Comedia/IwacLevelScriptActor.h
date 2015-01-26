// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/LevelScriptActor.h"
#include "KnifeCharacter.h"
#include "LightningActor.h"
#include "IronActor.h"
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

#pragma region TorturePhase
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "[Comedia] Torture Phase")
	AActor* TreeActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "[Comedia] TorturePhase")
	TEnumAsByte<ETorturePhase::Type> TorturePhase;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "[Comedia] TorturePhase")
	float DelayFirstSpawn;
#pragma endregion TorturePhase

#pragma region KnifePhase
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "[Comedia] KnifePhase")
	float DelayBetweenKnifeSpawn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "[Comedia] KnifePhase")
	float RadiusSpawnKnifeArea;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "[Comedia] KnifePhase")
	int32 NbSpawnedKnife;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "[Comedia] KnifePhase")
	int32 MaxNbSpawnedKnife;

	/** ComputedRadiusSpawnKnifeArea depending of player character height */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "[Comedia] KnifePhase")
	float ComputedRadiusSpawnKnifeArea;

	/** true indicates a knife is present */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "[Comedia] KnifePhase")
	bool bHasKnifeSpawned;
#pragma endregion KnifePhase

#pragma region LightningPhase
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "[Comedia] LightningPhase")
	int32 MaxNbLightning;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "[Comedia] LightningPhase")
	float LengthLightningPhase;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "[Comedia] LightningPhase")
	float RadiusSpawnLightningArea;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "[Comedia] LightningPhase")
	float PercentCriticalLightning;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "[Comedia] LightningPhase")
	float MinCooldownLightning;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "[Comedia] LightningPhase")
	float MaxCooldownLightning;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "[Comedia] LightningPhase")
	float DelayDamageLightning;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "[Comedia] LightningPhase")
	float RadiusDamageLightningArea;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "[Comedia] LightningPhase")
	float TimeSpendLightningPhase;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "[Comedia] LightningPhase")
	int32 CurrentNbLightning;
#pragma endregion LightningPhase

#pragma region IronPhase
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "[Comedia] IronPhase")
	float LightRotationSpeedMin;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "[Comedia] IronPhase")
	float LightRotationSpeedMax;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "[Comedia] IronPhase")
	float LengthIronPhase;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "[Comedia] IronPhase")
	float TimeSpendIronPhase;
#pragma endregion IronPhase

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION(BlueprintImplementableEvent, Category = "[Comedia] Events")
	virtual void PlayerTouchByKnife();

	UFUNCTION(BlueprintImplementableEvent, Category = "[Comedia] Events")
	virtual void PlayerTouchByLightning();

	UFUNCTION(BlueprintImplementableEvent, Category = "[Comedia] Events")
	virtual void PlayerTouchByIron();

private:
	TEnumAsByte<ETorturePhase::Type> _PreviousTorturePhase;

	/** Class to instance when spawning Knife character */
	TSubclassOf<AKnifeCharacter> _KnifeClass;

	/** Class to instance when spawning Lightning */
	TSubclassOf<ALightningActor> _LightningClass;

	/** Class to instance when spawning Iron Actor */
	TSubclassOf<AIronActor> _IronClass;

	float _RemainingTime;
	ACharacter* _PlayerCharacter;
	AIronActor* _SpawnedIronActor;

	void _TickKnifePhase(float DeltaSeconds);
	void _TickLightningPhase(float DeltaSeconds);
	void _TickIronPhase(float DeltaSeconds);
	void _KnifeSpawning(float ComputedRadiusSpawnKnifeArea);
	void _LightningSpawning(float ComputedRadiusSpawnLightningArea);
	void _IronSpawning();
};
