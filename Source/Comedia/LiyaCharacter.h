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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Camera)
	ULiyaCamera* Camera;

protected:
	/** Handles moving forward/backward */
	void MoveForward(float Val);

	/** Handles stafing movement, left and right */
	void MoveRight(float Val);

	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;
	// End of APawn interface

	virtual void AddControllerRollInput(float Val) override;
	virtual void AddControllerYawInput(float Val) override;
};
