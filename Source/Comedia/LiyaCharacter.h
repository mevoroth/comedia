// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "LiyaCamera.h"
#include "RespawnZoneActor.h"
#include "LowPassFilterComponent.h"

#include "GameFramework/Character.h"
#include "LiyaCharacter.generated.h"

UENUM(BlueprintType)
namespace EListenerOverrideType
{
	enum Type
	{
		COT_None				UMETA(DisplayName = "No override"),
		COT_BetweenCamAndLiya	UMETA(DisplayName = "Between Camera and Liya")
	};
}

/**
 * 
 */
UCLASS()
class COMEDIA_API ALiyaCharacter : public ACharacter
{
	GENERATED_UCLASS_BODY()

	///** How many footsteps to buffer for filtering footsteps height */
	//const int FOOTSTEP_LATENCY = 3;

	//UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "[Comedia]Settings")
	//float CharacterSpeed;

#pragma region GD Settings
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "[Comedia]Settings")
	float CameraSpeed;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "[Comedia]Settings")
	float MinCamPitch;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "[Comedia]Settings")
	float MaxCamPitch;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "[Comedia]Settings")
	bool bInvertXAxis;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "[Comedia]Settings")
	bool bInvertYAxis;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "[Comedia]Settings")
	float MaxAngularSpeed;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "[Comedia]Settings")
	float MaxAngularSpeedWhenGrab;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "[Comedia]Settings")
	float MaxSpeed;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "[Comedia]Settings")
	float MaxSpeedWhenGrab;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "[Comedia]Settings")
	float DeadZone;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "[Comedia]Settings")
	float DeccelMultiplier;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "[Comedia]Settings")
	float AccelMultiplier;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "[Comedia]Settings")
	float GrabSpeedAlphaTimer;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "[Comedia]Settings")
	float JumpHeight;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "[Comedia]Settings")
	float CallCooldown;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "[Comedia]Settings")
	float WalkingRatio;
#pragma endregion GD Settings

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "[Comedia]Settings")
	USceneComponent* Camera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "[Comedia]Settings")
	ARespawnZoneActor* CurrentRespawnZone;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "[Comedia]Settings")
	bool bDialogRunning;

	float LengthTravellingScriptedCamera = 2.0f;
	float LengthTravellingBackScriptedCamera = 0.0f;
	float ElapsedTravellingScriptedCamera = 0.0f;

	float RatioCameraFollow = 1.0f;

	FTransform OverrideScriptedCameraPosition;
	FTransform LastCamPosition;
	FTransform StartTravellingPosition;
	FVector GrabbingPlayerLocation;

	FORCEINLINE float GetCameraSpeed() const
	{
		return CameraSpeed;
	}

#pragma region Grab Events
	/** Event for when grab poster */
	void NotifyGrab(float PosterMaxDistance);
	/** Update grab point */
	void UpdateGrabPoint(const FVector& GrabPoint);
	/** Event for when release poster */
	void NotifyReleasePoster();
	/** Grab Pivot (for distance max) */
	void UpdateGrabPivot(const FVector& GrabPivot);
#pragma endregion Grab Events

#pragma region Sound
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "[Comedia]Sound")
	TEnumAsByte<EListenerOverrideType::Type> OverrideType;
#pragma endregion Sound

	UFUNCTION(BlueprintImplementableEvent, Category = "[Comedia]Events")
	virtual void LeftFootStep(const FVector& Pos);
	UFUNCTION(BlueprintImplementableEvent, Category = "[Comedia]Events")
	virtual void RightFootStep(const FVector& Pos);

#pragma region AnimBlueprint
	UFUNCTION(BlueprintCallable, Category = "[Comedia]AnimBlueprint")
	float GetRunningSpeedAnimBP() const;
	UFUNCTION(BlueprintCallable, Category = "[Comedia]AnimBlueprint")
	bool GetGrabbing() const;
	UFUNCTION(BlueprintCallable, Category = "[Comedia]AnimBlueprint")
	void SetHeightDisplacement(float Height);

	UFUNCTION(BlueprintImplementableEvent, Category = "[Comedia]Prince")
	bool UpdateRunAnim(bool Run);

	UFUNCTION(BlueprintImplementableEvent, Category = "[Comedia]Prince")
	bool UpdateHidding(bool Hidding);

	UFUNCTION(BlueprintImplementableEvent, Category = "[Comedia]Prince")
	bool ResetPrinceAnims();
#pragma endregion AnimBlueprin
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	void SwitchWalkingState();
protected:
#pragma region Controls
	/** Handles moving forward/backward */
	void MoveForward(float Val);

	/** Handles strafing movement, left and right */
	void MoveRight(float Val);

	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;
	// End of APawn interface

	/** Controls Mouse Y axis */
	void AddCameraPitch(float Val);
	/** Controls Mouse X axis */
	void AddCameraRoll(float Val);

	/** Call character */
	void CallCharacter();
#pragma endregion Controls

	UFUNCTION(BlueprintImplementableEvent, Category = "[Comedia]Events")
	bool OncallCharacter();

private:
	FVector2D _Accel;
	FVector2D _Speed;
	float _Rotation;
	bool _bWalking;

	ULowPassFilterComponent* FilterComponent;
	FVector _GrabPivot;
	float _GrabMaxDistance;
	float _GrabPreviousDistance;
	FVector _GrabDirection;
	float _CurrentSpeedMultiplier;
	float _GrabSpeedAlpha;
	float _GrabSpeedAlphaIt;
	float _GrabArmLength;

	float _CallCooldown;

	float _RunningSpeedAnimBP;
	float _InitHeight;

	FVector _OriginalPivotCamPosition;
	FRotator _OriginalPivotCamRotation;

	UMaterialInstanceDynamic* _PosterMaterialInst;

	void _Controls(float DeltaSeconds);
	void _LerpGrab(float DeltaSeconds);
	void _OverridingCamera(float DeltaSeconds);
	void _OverridingAudioListener();
	void _AttachAudioListenerToLiya();

	void _ControlsMove(const FVector2D& Speed);
};
