// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "PropagandaHandActor.generated.h"

/**
 * 
 */
UCLASS()
class COMEDIA_API APropagandaHandActor : public AActor
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "[Comedia] PropagandaHandParams")
	UArrowComponent* ForwardArrowComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "[Comedia] PropagandaHandParams")
	USphereComponent* SphereTriggerZoneComponent;

	APropagandaHandActor(const FObjectInitializer& ObjectInitializer);


};
