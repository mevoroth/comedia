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

	enum PosterState
	{
		INIT = 0,
		GRABBED = 1,
		ONSTICK = 2,
		STICKED = 3,
		GRABBABLE = 0x1 << 30,
		HEADISROOT = 0x1 << 31
	};
private_subobject:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "[Comedia]Poster", meta = (ExposeFunctionCategories = "Mesh,Components|SkeletalMesh,Animation,Physics", AllowPrivateAccess = "true"))
	UPoseableMeshComponent* PosterMesh;

public:
	UFUNCTION(BlueprintCallable, Category = "[Comedia]Poster")
	bool IsDetached() const;

	void UpdateChain();

	void SetEffector(const FTransform& Effector);
	UFUNCTION(BlueprintCallable, Category = "[Comedia]Poster")
	void Grabbing(bool Grabbing);
	UFUNCTION(BlueprintCallable, Category = "[Comedia]Poster")
	void InRange(bool HeadIsRoot);
	UFUNCTION(BlueprintCallable, Category = "[Comedia]Poster")
	void OutRange();
	UFUNCTION(BlueprintCallable, Category = "[Comedia]Poster")
	FVector GetGripHeadUpdated() const;
	UFUNCTION(BlueprintCallable, Category = "[Comedia]Poster")
	FVector GetGripTailUpdated() const;
	UFUNCTION(BlueprintCallable, Category = "[Comedia]Poster")
	void Stick(bool Sticked);
	//UFUNCTION(BlueprintCallable, Category = "[Comedia]Poster")
	//FVector GetPosterForward() const;
	UFUNCTION(BlueprintCallable, Category = "[Comedia]Poster")
	bool IsInFireRange(const FVector& Position) const;

	UFUNCTION(BlueprintImplementableEvent, Category = "[Comedia]Poster")
	virtual FVector GetGripHead() const;
	UFUNCTION(BlueprintImplementableEvent, Category = "[Comedia]Poster")
	virtual FVector GetGripTail() const;

	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;
	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "[Comedia]Poster")
	USceneComponent* LeftGrabbedCamPosition;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "[Comedia]Poster")
	USceneComponent* RightGrabbedCamPosition;

	/** Precision */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "[Comedia]Poster")
	float Precision;

	/** Max Iterations */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "[Comedia]Poster")
	float MaxIterations;

	/** Reset Poster Animation */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "[Comedia]Poster")
	float ResetSpeed;

	/** Fire Range Distance */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "[Comedia]Poster")
	float FireRangeDistance;

	/** Fire Range Radius in degrees */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "[Comedia]Poster")
	float FireRangeRadius;

	/** Delay between bones when reset */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "[Comedia]Poster")
	float DelayBetweenBones;

	/** If grab is used */
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "[Comedia]Poster")
	bool Grabbed;

	/** If grab is enabled */
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "[Comedia]Poster")
	bool GrabEnabled;

	bool Sticked;

private:
	PosterState State;
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

	/** Reset Poster to origin */
	void _Reset(float DeltaSeconds);
	float _ResetAlpha;

	FVector _PlaneForward;

	float _HeadDist;
	float _TailDist;

	FTransform* _BonesBuff;
	FTransform* _BonesInit;
};
