// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "PosterActor.generated.h"

/**
 * 
 */
UCLASS()
class COMEDIA_API APosterActor : public AActor
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "[Comedia]Poster")
	UPoseableMeshComponent* PosterMesh;

	void UpdateChain();

	void SetEffector(const FTransform& Effector);
	UFUNCTION(BlueprintCallable, Category = "[Comedia]Poster")
	void Grab(bool Grabbed, bool HeadIsRoot);

	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;
	virtual void Tick(float DeltaSeconds) override;

	/** Precision */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "[Comedia]Poster")
	float Precision;

	/** Max Iterations */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "[Comedia]Poster")
	float MaxIterations;

private:
	/** Root to target distance */
	float _MaxDistance;

	/** Effector (Hands) */
	FTransform _Effector;

	/** Distance between each bone starting from head */
	float* _DistanceFromHead;
	/** Distance between each bone starting from head */
	float* _DistanceFromTail;

	UPROPERTY()
	bool _HeadIsRoot;

	/** Set Liya's Hands Socket to effector */
	void _UpdateEffector();

	FTransform* _BonesBuff;
	FTransform* _BonesInit;

	UPROPERTY()
	bool _Grabbed;
};
