// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "DamonFalconActor.generated.h"

/**
 * 
 */
UCLASS()
class COMEDIA_API ADamonFalconActor : public AActor
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Comedia Settings")
	FRotator RotationSpeed;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Comedia Internal")
	USceneComponent* RotatedObject;

	virtual void ReceiveTick(float DeltaSeconds) override;
};
