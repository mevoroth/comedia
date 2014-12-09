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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "KnifeParams")
	float DecalWidth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "KnifeParams")
	float DecalLength;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "KnifeParams")
	UMaterial* DecalMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "KnifeParams")
	float DecalLifeSpan;

	virtual void Tick(float DeltaSeconds) override;

	virtual void ReceiveHit(class UPrimitiveComponent* MyComp, AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;

	void InitOriginalPosition();

private:
	TSubclassOf<ADecalActor> DecalClass;

	FVector LastDecalPosition;

	void DestroyKnife();

	void SpawnDecal();
};
