// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "LiyaCamera.h"

#include "GameFramework/Character.h"
#include "LiyaCharacter.generated.h"

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "[Comedia]Settings")
	USceneComponent* Camera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "[Comedia]Settings")
	float LengthTravellingScriptedCamera = 2.0f;
	float LengthTravellingBackScriptedCamera = 0.0f;
	float ElapsedTravellingScriptedCamera = 0.0f;

	USceneComponent* OverrideScriptedCameraPosition;
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

	UFUNCTION(BlueprintImplementableEvent, Category = "[Comedia]Events")
	virtual void LeftFootStep(const FVector& Pos);
	UFUNCTION(BlueprintImplementableEvent, Category = "[Comedia]Events")
	virtual void RightFootStep(const FVector& Pos);

#pragma region AnimBlueprint
	UFUNCTION(BlueprintCallable, Category = "[Comedia]AnimBlueprint")
	float GetRunningSpeedAnimBP() const;
	UFUNCTION(BlueprintCallable, Category = "[Comedia]AnimBlueprint")
	bool GetGrabbing() const;
#pragma endregion AnimBlueprint

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;


protected:
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

private:
	FVector2D Accel;
	FVector2D Speed;
	float Rotation;

	FVector _GrabPivot;
	float _GrabMaxDistance;
	float _GrabPreviousDistance;
	FVector _GrabDirection;
	float _CurrentSpeedMultiplier;
	float _GrabSpeedAlpha;
	float _GrabSpeedAlphaIt;
	float _GrabArmLength;

	float _RunningSpeedAnimBP;

	void _Controls(float DeltaSeconds);
	void _LerpGrab(float DeltaSeconds);

	void _ControlsMove(const FVector2D& Speed);

};
