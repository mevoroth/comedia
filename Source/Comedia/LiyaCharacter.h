// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "LiyaCamera.h"

#include "LowPassFilterComponent.h"

#include "GameFramework/Character.h"
#include "LiyaCharacter.generated.h"

/**
 * 
 */
UCLASS()
class COMEDIA_API ALiyaCharacter : public ACharacter
{
	GENERATED_UCLASS_BODY()

	/** How many footsteps to buffer for filtering footsteps height */
	const int FOOTSTEP_LATENCY = 3;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Comedia Settings")
	float CharacterSpeed;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Comedia Settings")
	float CameraSpeed;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Comedia Settings")
	float MinCamPitch;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Comedia Settings")
	float MaxCamPitch;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Comedia Settings")
	USceneComponent* Camera;

	inline float GetCameraSpeed() const
	{
		return CameraSpeed;
	}

	UFUNCTION(BlueprintImplementableEvent, Category = "[Comedia]Events")
	virtual void LeftFootStep(const FVector& Pos);
	UFUNCTION(BlueprintImplementableEvent, Category = "[Comedia]Events")
	virtual void RightFootStep(const FVector& Pos);

	/** Handle footsteps in Tick event */
	void FootSteps(float DeltaSeconds);

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
	virtual void AddControllerYawInput(float Val) override;
private:
	enum FootAnimationState
	{
		DOWN, // Foot is reaching the ground
		UP // Foot is leaving the ground
	};

	static const float WAIT_BEFORE_THRESHOLD;
	
	float ElapsedTime;
	/** Last time left foot emits sound+particle */
	float LeftFootLastTime;
	/** Last time right foot emits sound+particle */
	float RightFootLastTime;
	/** Previous Z diff left */
	float DeltaZLeft;
	/** Previous Z diff right */
	float DeltaZRight;
	/** Previous Z diff derivative left */
	float DeltaDerivZLeft;
	/** Previous Z diff derivative right */
	float DeltaDerivZRight;

	float LeftFootThreshold;
	float RightFootThreshold;

	FootAnimationState LeftFootState;
	FootAnimationState RightFootState;

	/** Low Pass Filter for left footstep Z */
	TSubobjectPtr<ULowPassFilterComponent> LeftFootFiltered;
	/** Low Pass Filter for right footstep Z */
	TSubobjectPtr<ULowPassFilterComponent> RightFootFiltered;

	bool Internal_CheckFootstep(const FVector& Footstep, float Threshold, TSubobjectPtr<ULowPassFilterComponent>& Filter, FootAnimationState& FootState, float& oldZ, float& oldZDerivative);
	float Internal_FootStepDistance(const FVector& FootPos) const;
};
