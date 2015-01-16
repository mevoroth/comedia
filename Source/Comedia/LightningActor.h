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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "[Comedia] LightningParams")
	UDecalComponent* LightningFullDecal;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "[Comedia] LightningParams")
	UDecalComponent* LightningTimerDecal;

	ALightningActor(const FObjectInitializer& ObjectInitializer);
	void SetDecalsScale(float Width, float Height);
	virtual void Tick(float DeltaSeconds);
	virtual void LifeSpanExpired();

private:
	float _ComputedRadiusDamageLightningArea;

	float _GetRatioRemainingTime();
};
