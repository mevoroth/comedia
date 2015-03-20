// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "IronActor.generated.h"

/**
 * 
 */
UCLASS()
class COMEDIA_API AIronActor : public APawn
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "[Comedia]Iron Property")
	bool bFirstSpawn;

	bool bInverseRotation;

	float RotationSpeed = 1.0f;

	AIronActor(const FObjectInitializer& ObjectInitializer);
	virtual void Tick(float DeltaSeconds);

	UFUNCTION(BlueprintCallable, Category = "[Comedia]Iron Property")
	void UpdateFirstSpawnState();

	UFUNCTION(BlueprintCallable, Category = "[Comedia]Iron Property")
	FName GetSpatialization() const;

	UFUNCTION(BlueprintImplementableEvent, Category = "[Comedia] Iron")
	FVector GetLiyaTree() const;
};
