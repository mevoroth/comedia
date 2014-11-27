// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Character.h"
#include "FPSPlayerCharacter.generated.h"

/**
 * 
 */
UCLASS()
class COMEDIA_API AFPSPlayerCharacter : public ACharacter
{
	GENERATED_UCLASS_BODY()

protected:
	/** Handles moving forward/backward */
	void MoveForward(float Val);

	/** Handles stafing movement, left and right */
	void MoveRight(float Val);

	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;
	// End of APawn interface
};
