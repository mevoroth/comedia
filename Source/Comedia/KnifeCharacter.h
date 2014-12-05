// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Character.h"
#include "KnifeCharacter.generated.h"

/**
 * 
 */
UCLASS()
class COMEDIA_API AKnifeCharacter : public ACharacter
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "KnifeParams")
	float KnifeSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "KnifeParams")
	float MaxRotationSpeed;

	virtual void Tick(float DeltaSeconds) override;
};
