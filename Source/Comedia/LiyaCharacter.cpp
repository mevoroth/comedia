// Fill out your copyright notice in the Description page of Project Settings.

#include "Comedia.h"
#include "LiyaCharacter.h"
#include "DamonFalconActor.h"

ALiyaCharacter::ALiyaCharacter(const class FObjectInitializer& FOI)
	: Super(FOI)
	, Camera(0)
{
}

void ALiyaCharacter::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	// set up gameplay key bindings
	check(InputComponent);
	
	InputComponent->BindAxis("MoveForward", this, &ALiyaCharacter::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &ALiyaCharacter::MoveRight);
	InputComponent->BindAxis("Turn", this, &ALiyaCharacter::AddCameraRoll);
	InputComponent->BindAxis("LookUp", this, &ALiyaCharacter::AddCameraPitch);
}

void ALiyaCharacter::MoveForward(float Val)
{
	if (Val != 0.0f)
	{
		Mesh->SetRelativeRotation(FRotator(
			0.f, Camera->GetComponentRotation().Yaw, 0.f
		));
		AddMovementInput(Camera->GetForwardVector(), Val * GetWorld()->GetDeltaSeconds() * CharacterSpeed);
	}
}

void ALiyaCharacter::MoveRight(float Val)
{
	if (Val != 0.0f)
	{
		Mesh->SetRelativeRotation(FRotator(
			0.f, Camera->GetComponentRotation().Yaw, 0.f
		));
		AddMovementInput(Camera->GetRightVector(), Val * GetWorld()->GetDeltaSeconds() * CharacterSpeed);
	}
}

void ALiyaCharacter::AddCameraRoll(float Val)
{
	check(Camera); // Camera not set in BP

	Camera->AddRelativeRotation(FRotator(
		0.f, Val * GetWorld()->GetDeltaSeconds() * CameraSpeed, 0.f
	));
}

void ALiyaCharacter::AddCameraPitch(float Val)
{
	check(Camera); // Camera not set in BP

	float Pitch = Camera->GetComponentRotation().Pitch;
	Val *= GetWorld()->GetDeltaSeconds() * CameraSpeed;

	if (Pitch + Val < MinCamPitch)
	{
		Val = MinCamPitch - Pitch;
	}
	else if (Pitch + Val > MaxCamPitch)
	{
		Val = MaxCamPitch - Pitch;
	}

	Camera->AddRelativeRotation(FRotator(
		Val, 0.f, 0.f
	));
}
