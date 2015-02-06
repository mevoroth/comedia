// Fill out your copyright notice in the Description page of Project Settings.

#include "Comedia.h"
#include "LiyaCharacter.h"
#include "DamonFalconActor.h"

ALiyaCharacter::ALiyaCharacter(const class FObjectInitializer& FOI)
	: Super(FOI)
	, Camera(0)
	, bInvertXAxis(true)
	, bInvertYAxis(true)
	, MaxSpeed(1.f)
	, DeccelMultiplier(0.2f)
	, AccelMultiplier(1.f)
	, DeadZone(0.05f)
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
		Accel.X = Val;
	}
}

void ALiyaCharacter::MoveRight(float Val)
{
	if (Val != 0.0f)
	{
		Accel.Y = Val;
	}
}

void ALiyaCharacter::AddCameraRoll(float Val)
{
	check(Camera); // Camera not set in BP

	Camera->AddRelativeRotation(FRotator(
		0.f, Val * GetWorld()->GetDeltaSeconds() * CameraSpeed * (bInvertXAxis ? -1.f : 1.f), 0.f
	));
}

void ALiyaCharacter::AddCameraPitch(float Val)
{
	check(Camera); // Camera not set in BP

	float Pitch = Camera->GetComponentRotation().Pitch;
	Val *= GetWorld()->GetDeltaSeconds() * CameraSpeed * (bInvertYAxis ? -1.f : 1.f);

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

void ALiyaCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	FVector2D TmpSpeed;
	if (Accel.SizeSquared() > 0.01f)
	{
		Speed += Accel.SafeNormal() * DeltaSeconds * AccelMultiplier;
		Speed = Speed.ClampAxes(-MaxSpeed, MaxSpeed);

		if (Speed.SizeSquared() < DeadZone)
		{
			TmpSpeed = FVector2D(0.f, 0.f);
		}
		else
		{
			TmpSpeed = Speed;
		}
	}
	else
	{
		UE_LOG(LogGPCode, Warning, TEXT("ET CEST PAS LE GG"));
		Speed -= Speed.SafeNormal() * DeccelMultiplier;
		if (Speed.SizeSquared() < DeadZone)
		{
			Speed = FVector2D(0.f, 0.f);
		}
		TmpSpeed = Speed;
	}

	AddMovementInput(Camera->GetForwardVector(), TmpSpeed.X * GetWorld()->GetDeltaSeconds() * CharacterSpeed);
	AddMovementInput(Camera->GetRightVector(), TmpSpeed.Y * GetWorld()->GetDeltaSeconds() * CharacterSpeed);

	if (!(Accel.X == 0.f && Accel.Y == 0.f))
	{
		Rotation = Camera->GetComponentRotation().Yaw + FMath::Atan2(Speed.Y, Speed.X) * 180.f / PI - 90.0f;
	}

	Mesh->SetRelativeRotation(FRotator(
		0.f, Rotation, 0.f
	));


	Accel = FVector2D(0.f, 0.f);
}
