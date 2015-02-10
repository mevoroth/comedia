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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "[Comedia] KnifeParams")
	float KnifeSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "[Comedia] KnifeParams")
	float MaxRotationSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "[Comedia] KnifeParams")
	float RatioTrailLength = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "[Comedia] KnifeParams")
	float VerticalTrailOffset = 1.0f;

	virtual void Tick(float DeltaSeconds) override;

	virtual void ReceiveHit(class UPrimitiveComponent* MyComp, AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;

	void InitOriginalPosition();

private:
	TSubclassOf<AActor> _TrailClass;

	FVector _LastDecalPosition;

	float _TrailLength;

	void _DestroyKnife();

	void _SpawnTrail();
};
