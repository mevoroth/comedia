// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/LevelScriptActor.h"
#include "KnifeCharacter.h"
#include "LightningActor.h"
#include "IronActor.h"
#include "Matinee/MatineeActor.h"
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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "[Comedia] TorturePhase")
	AActor* TreeActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "[Comedia] TorturePhase")
	TEnumAsByte<ETorturePhase::Type> TorturePhase;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "[Comedia] TorturePhase")
	float DelayFirstSpawn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "[Comedia] TorturePhase")
	AMatineeActor* MatineeIntro;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "[Comedia] TorturePhase")
	AMatineeActor* MatineeLightningToIron;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "[Comedia] TorturePhase")
	AMatineeActor* MatineeKnifeToLightning;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "[Comedia] TorturePhase")
	AMatineeActor* MatineeOutro;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "[Comedia] TorturePhase")
	AMatineeActor* MatineePlayerFailed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "[Comedia] TorturePhase")
	bool bPlayerHasFailed;
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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "[Comedia] KnifePhase")
	int32 CurrentNbHitKnife;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "[Comedia] KnifePhase")
	int32 NbHitKnifeBeforeDie;

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
	float EndDelayLightningPhase;

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "[Comedia] LightningPhase")
	int32 CurrentNbHitLightning;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "[Comedia] LightningPhase")
	int32 NbHitLightningBeforeDie;

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "[Comedia] IronPhase")
	float MinReverseLengthIron;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "[Comedia] IronPhase")
	float MaxReverseLengthIron;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "[Comedia] IronPhase")
	float RemainingReverseTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "[Comedia] IronPhase")
	float BreakReverseLength;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "[Comedia] IronPhase")
	float RemainingBreakReverseTime;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "[Comedia] IronPhase")
	float CurrentTimeHitIron;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "[Comedia] IronPhase")
	float TimeHitIronBeforeDie;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "[Comedia] IronPhase")
	float TimeSpendIronPhase;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "[Comedia] IronPhase")
	bool bTouchedIronLastTick;


#pragma endregion IronPhase

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION(BlueprintNativeEvent, Category = "[Comedia] Events")
	void PlayerTouchByKnife();

	UFUNCTION(BlueprintNativeEvent, Category = "[Comedia] Events")
	void PlayerTouchByLightning();

	UFUNCTION(BlueprintNativeEvent, Category = "[Comedia] Events")
	void PlayerTouchByIron();

	UFUNCTION(BlueprintNativeEvent, Category = "[Comedia] Events")
	void PlayerFailed();

	UFUNCTION(BlueprintImplementableEvent, Category = "[Comedia] Events")
	void PlayMatineeIntro();

	UFUNCTION(BlueprintImplementableEvent, Category = "[Comedia] Events")
	void PlayMatineeLightningToIron();

	UFUNCTION(BlueprintImplementableEvent, Category = "[Comedia] Events")
	void PlayMatineeKnifeToLightning();

	UFUNCTION(BlueprintImplementableEvent, Category = "[Comedia] Events")
	void PlayMatineeOutro();

	UFUNCTION(BlueprintImplementableEvent, Category = "[Comedia] Events")
	void PlayMatineePlayerFailed();


	UFUNCTION(BlueprintCallable, Category = "[Comedia] TorturePhase")
	void ReinitCurrentPhase();

	bool bFirstIronSpawn;

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


#pragma region Liya Wounds
	/** Wounds */
	float _CurrentWounds;
	/** Previous wounds */
	float _PreviousPhaseWounds;
#pragma endregion Liya Wounds

	void _UpdateWounds(float WoundAlpha);

	void _TickKnifePhase(float DeltaSeconds);
	void _TickLightningPhase(float DeltaSeconds);
	void _TickIronPhase(float DeltaSeconds);
	void _KnifeSpawning(float ComputedRadiusSpawnKnifeArea);
	void _LightningSpawning(float ComputedRadiusSpawnLightningArea);
	void _IronSpawning();
};
