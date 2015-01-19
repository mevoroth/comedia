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

	///** How many footsteps to buffer for filtering footsteps height */
	//const int FOOTSTEP_LATENCY = 3;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "[Comedia]Settings")
	float CharacterSpeed;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "[Comedia]Settings")
	float CameraSpeed;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "[Comedia]Settings")
	float MinCamPitch;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "[Comedia]Settings")
	float MaxCamPitch;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "[Comedia]Settings")
	USceneComponent* Camera;

	inline float GetCameraSpeed() const
	{
		return CameraSpeed;
	}

	UFUNCTION(BlueprintImplementableEvent, Category = "[Comedia]Events")
	virtual void LeftFootStep(const FVector& Pos);
	UFUNCTION(BlueprintImplementableEvent, Category = "[Comedia]Events")
	virtual void RightFootStep(const FVector& Pos);

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
	float Rotation;
};
