// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "LightningActor.generated.h"

/**
 * 
 */
UCLASS()
class COMEDIA_API ALightningActor : public AActor
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "[Comedia] LightningParams")
	UParticleSystem* LightningParticleSystem;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "[Comedia] LightningParams")
	UParticleSystem* PreThunderParticleSystem;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "[Comedia] LightningParams")
	UDecalComponent* LightningFullDecal;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "[Comedia] LightningParams")
	UDecalComponent* LightningTimerDecal;

	ALightningActor(const FObjectInitializer& ObjectInitializer);
	void SetDecalsScale(float Width, float Height);
	virtual void Tick(float DeltaSeconds) override;
	virtual void LifeSpanExpired() override;
	void InitImpactTarget();

private:
	FVector _ImpactPosition;
	float _ComputedRadiusDamageLightningArea;

	float _GetRatioRemainingTime();
};
