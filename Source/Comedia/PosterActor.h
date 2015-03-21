// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "RespawnZoneActor.h"
#include "Engine/TriggerBox.h"
#include "GameFramework/Actor.h"
#include "PosterActor.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
namespace ENodeType
{
	enum Type
	{
		NT_BasicNode   UMETA(DisplayName = "BasicNode"),
		NT_HiddingNode UMETA(DisplayName = "HiddingNode"),
		NT_DoorNode    UMETA(DisplayName = "DoorNode"),
		NT_SideNode    UMETA(DisplayName = "SideNode"),
		NT_StartNode   UMETA(DisplayName = "StartNode"),
		NT_RespawnNode UMETA(DisplayName = "RespawnNode"),
		NT_CheckpointNode UMETA(DisplayName = "CheckpointNode")
	};
}

UENUM(BlueprintType)
namespace ESoldierState
{
	enum Type
	{
		ST_Idle          UMETA(DisplayName = "Idle"),
		ST_Walking       UMETA(DisplayName = "Walking"),
		ST_ShootingFace  UMETA(DisplayName = "ShootingFace"),
		ST_ShootingSide  UMETA(DisplayName = "ShootingSide")
	};
}

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
		RESET_FIRST_FRAME = 4,
		RESET_SECOND_FRAME = 5,
		GRABBABLE = 0x1 << 30,
		HEADISROOT = 0x1 << 31
	};
private_subobject:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "[Comedia]Poster", meta = (ExposeFunctionCategories = "Transform", AllowPrivateAccess = "true"))
	USceneComponent*  PosterRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "[Comedia]Poster", meta = (ExposeFunctionCategories = "Mesh,Components|SkeletalMesh,Animation,Physics", AllowPrivateAccess = "true"))
	UPoseableMeshComponent* PosterMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "[Comedia]Poster", meta = (ExposeFunctionCategories = "Mesh,Components|SkeletalMesh,Animation,Physics", AllowPrivateAccess = "true"))
	USkeletalMeshComponent* FeedbackMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "[Comedia]Poster", meta = (ExposeFunctionCategories = "Mesh,Components|SkeletalMesh,Animation,Physics", AllowPrivateAccess = "true"))
	USkeletalMeshComponent* CorruptedFeedbackMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "[Comedia]Call", meta = (ExposeFunctionCategories = "Shape,Collision,Rendering,Transform", AllowPrivateAccess = true, MakeEditWidget))
	UBoxComponent* CallTrigger;

public:
	UFUNCTION(BlueprintImplementableEvent, Category = "[Comedia]Events")
	void PrinceGoThroughDoor(FVector Position);

	UFUNCTION(BlueprintImplementableEvent, Category = "[Comedia]Events")
	void PrinceHiding(bool bHiding, FVector Position);

	UFUNCTION(BlueprintCallable, Category = "[Comedia]Poster")
	float DetachRatio() const;

	void UpdateChain();
	void ResetPoster();

	void SetEffector(const FTransform& Effector);
	UFUNCTION(BlueprintCallable, Category = "[Comedia]Poster")
	void Grabbing(bool Grabbing);
	UFUNCTION(BlueprintCallable, Category = "[Comedia]Poster")
	bool IsGrabbing() const;
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
	UFUNCTION(BlueprintCallable, Category = "[Comedia]Poster")
	FVector GetPosterForward() const;

	UFUNCTION(BlueprintCallable, Category = "[Comedia]Poster")
	void UpdateStickPoint(const FVector& StickPointPos);

#pragma region Poster Events
	UFUNCTION(BlueprintNativeEvent, Category = "[Comedia]Poster")
	void OnGrab(const FVector& Position);
	UFUNCTION(BlueprintImplementableEvent, Category = "[Comedia]Poster")
	virtual bool OnRelease(const FVector& Position);
	UFUNCTION(BlueprintImplementableEvent, Category = "[Comedia]Poster")
	virtual bool OnStick(const FVector& Position);
	UFUNCTION(BlueprintNativeEvent, Category = "[Comedia]Poster")
	void OnResetFinished();
	UFUNCTION(BlueprintImplementableEvent, Category = "[Comedia]Poster")
	virtual bool ToggleFootStep();
	UFUNCTION(BlueprintNativeEvent, Category = "[Comedia]Poster")
	void OnWayFound();
#pragma endregion Poster Events

	UFUNCTION(BlueprintImplementableEvent, Category = "[Comedia]Poster")
	virtual FVector GetGripHead() const;
	UFUNCTION(BlueprintImplementableEvent, Category = "[Comedia]Poster")
	virtual FVector GetGripTail() const;

	UFUNCTION(BlueprintImplementableEvent, Category = "[Comedia]Poster")
	virtual void InitGripReferences();

#pragma region Soldier
	UFUNCTION(BlueprintCallable, Category = "[Comedia]Soldier")
	virtual void SetSoldier(USceneComponent* SoldierComponent);
	UFUNCTION(BlueprintCallable, Category = "[Comedia]Soldier")
	virtual void SetSoldierTimelineComponent(UCurveFloat* TimelineComponent);
	UFUNCTION(BlueprintCallable, Category = "[Comedia]Soldier")
	virtual bool SoldierKills();
	UFUNCTION(BlueprintImplementableEvent, Category = "[Comedia]Soldier")
	virtual bool OnSoldierWalk(const FVector& Position);
	UFUNCTION(BlueprintImplementableEvent, Category = "[Comedia]Soldier")
	virtual bool OnSoldierStopWalking();
	UFUNCTION(BlueprintImplementableEvent, Category = "[Comedia]Soldier")
	virtual bool OnConeInactive();
	UFUNCTION(BlueprintImplementableEvent, Category = "[Comedia]Soldier")
	virtual bool OnConeActive();
	UFUNCTION(BlueprintNativeEvent, Category = "[Comedia]Soldier")
	void OnKillLiyah();
	UFUNCTION(BlueprintNativeEvent, Category = "[Comedia]Soldier")
	void OnKillPrince();
	UFUNCTION(BlueprintCallable, Category = "[Comedia]Poster")
	bool IsInFireRange(const FVector& Position) const;
	bool PrinceIsInFireRange();
#pragma endregion Soldier

#pragma region Feedbacks
	UFUNCTION(BlueprintCallable, Category = "[Comedia]Feedbacks")
	virtual void SetFeedbackObject(USceneComponent* FeedbackObject);
	UFUNCTION(BlueprintCallable, Category = "[Comedia]Feedbacks")
	virtual bool IsPrinceCallable() const;
#pragma endregion Feedbacks

	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;
	virtual void Tick(float DeltaSeconds) override;
#if !defined UE_BUILD_SHIPPING
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "[Comedia]Poster")
	USceneComponent* LeftGrabbedCamPosition;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "[Comedia]Poster")
	USceneComponent* RightGrabbedCamPosition;

	/** For Prince Navigation */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "[Comedia]PosterPath")
	TArray<float> KeyPoints;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "[Comedia]PosterPath")
	TArray<TEnumAsByte<ENodeType::Type>> KeyNodeTypes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "[Comedia]PosterPath")
	APosterActor* DoorLinkedPoster;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "[Comedia]PosterPath")
	ARespawnZoneActor* RespawnZone;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "[Comedia]PosterPath")
	float DelayCall;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "[Comedia]Poster")
	bool bIsGrabbable;

	/** Precision */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "[Comedia]Poster")
	float Precision;

	/** Max Iterations */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "[Comedia]Poster")
	int32 MaxIterations;

	/** Reset Poster Animation */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "[Comedia]Poster")
	float ResetSpeed;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "[Comedia]Soldier")
	TEnumAsByte<ESoldierState::Type> SoldierState;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "[Comedia]Soldier")
	TEnumAsByte<ESoldierState::Type> PreviousSoldierState;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "[Comedia]Soldier")
	float TimeBeforeSoldierActive = 1.0f;

	float RemainingTimeBeforeSoldierActive = 0.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "[Comedia]Soldier")
	bool bSoldierFlipped;

	/** Fire Range Distance */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "[Comedia]Soldier")
	float FireRangeDistance;

	/** Fire Range Radius in degrees */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "[Comedia]Soldier")
	float FireRangeAngle;

	/** Delay between bones when reset */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "[Comedia]Poster")
	float DelayBetweenBones;

	/** Delay before reset poster */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "[Comedia]Poster")
	float DelayBeforeReset;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "[Comedia]Poster")
	float LengthTravellingScriptedCamera = 2.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "[Comedia]Poster")
	ABlockingVolume* AssociatedBlockingVolume;

	/** Detach threshold for blocking volume */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "[Comedia]Poster")
	float DetachThreshold;

	/** For Soldier cone toggle */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "[Comedia]Soldier")
	TArray<float> ConeToggle;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "[Comedia]Soldier")
	bool SoldierPatrolEnabled;

	bool Sticked;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "[Comedia]Poster")
	USphereComponent* GripHeadComponent;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "[Comedia]Poster")
	USphereComponent* GripTailComponent;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "[Comedia]Poster")
	UStaticMeshComponent* DoorComponent;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "[Comedia]Door")
	bool DoorEnabled;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "[Comedia]Poster")
	UMaterialInstance* MeshMaterialInst;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "[Comedia]Feedbacks")
	bool bShowFeedback;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "[Comedia]Ending")
	ATriggerBox* EndingTrigger;

	bool bOriginalIsGrabbable;

	bool bVisionIsActive;
private:

	PosterState State;
	/** Root to target distance */
	float _MaxDistance;

	float _MaxDistanceBetweenGrip;

	/** If stick point, variable indicates absolute position */
	FVector _StickPointPos;
	FTransform _GrabbedCurrentPosition;
	float _StickedAlpha;

	/** Effector (Hands) */
	FTransform _Effector;

	/** Distance between each bone starting from head */
	float* _DistanceFromHead;
	/** Distance between each bone starting from head */
	float* _DistanceFromTail;

	/** Set Liya's Hands Socket to effector */
	void _UpdateEffector();

	/** Reset Poster to origin */
	void _Reset(float DeltaSeconds);
	float _ResetAlpha;
	bool _bResetHeadIsRoot;

	FVector _PlaneForward;

	float _HeadDist;
	float _TailDist;

	float _LastAnimatedObjectPosition;
	float _LastOrientation;

	FTransform* _BonesBuff;
	FTransform* _BonesInit;

	USceneComponent* _SoldierComponent;
	UCurveFloat* _TimelineComponent;

#pragma region Soldier
	bool _SoldierEnabled;
	void _Soldier(float DeltaSeconds);
	float _SoldierElapsedTime;
	float _SoldierCurrentPos;
	float _SoldierPreviousPos;
	float _GetSoldierDirection() const;
#pragma endregion Soldier

	void _UpdateCompositedTexture();

	void _CancelOverridingCamPosition();

	USceneComponent* _FeedbackObject;

#pragma region Poster Events
	bool _ResetCalled;
	bool _WayFound;
#pragma endregion Poster Events
};
