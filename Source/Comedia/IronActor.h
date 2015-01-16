// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "IronActor.generated.h"

/**
 * 
 */
UCLASS()
class COMEDIA_API AIronActor : public AActor
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "[Comedia] IronParams")
	float RotationSpeed = 1.0f;

	AIronActor(const FObjectInitializer& ObjectInitializer);
	virtual void Tick(float DeltaSeconds);
};
