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

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Comedia Camera Settings")
	float CameraSpeed;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Comedia Camera Settings")
	float MinCamPitch;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Comedia Camera Settings")
	float MaxCamPitch;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
	USceneComponent* Camera;

	inline float GetCameraSpeed() const
	{
		return CameraSpeed;
	}
	//ULiyaCamera* Camera;

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
};
