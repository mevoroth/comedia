// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/TriggerBase.h"
#include "TriggerStickPoint.generated.h"

/**
 * 
 */
UCLASS()
class COMEDIA_API ATriggerStickPoint : public ATriggerBase
{
	GENERATED_UCLASS_BODY()

	UFUNCTION(BlueprintCallable, Category = "[Comedia]StickPoint")
	void SetCollisionRadius(float Radius);
	
	UFUNCTION()
	void OnBeginOverlap(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const struct FHitResult& SweepResult);
	UFUNCTION()
	void OnEndOverlap(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
