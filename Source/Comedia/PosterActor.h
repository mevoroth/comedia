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

private_subobject:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "[Comedia]Poster", meta = (ExposeFunctionCategories = "Mesh,Components|SkeletalMesh,Animation,Physics", AllowPrivateAccess = "true"))
	UPoseableMeshComponent* PosterMesh;

public:
	void UpdateChain();

	void SetEffector(const FTransform& Effector);
	UFUNCTION(BlueprintCallable, Category = "[Comedia]Poster")
	void Grabbing(bool Grabbing);
	UFUNCTION(BlueprintCallable, Category = "[Comedia]Poster")
	void InRange(bool HeadIsRoot);
	UFUNCTION(BlueprintCallable, Category = "[Comedia]Poster")
	void OutRange();

	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;
	virtual void Tick(float DeltaSeconds) override;

	/** Precision */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "[Comedia]Poster")
	float Precision;

	/** Max Iterations */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "[Comedia]Poster")
	float MaxIterations;

	/** If grab is used */
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "[Comedia]Poster")
	bool Grabbed;

	/** If grab is enabled */
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "[Comedia]Poster")
	bool GrabEnabled;

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
};
