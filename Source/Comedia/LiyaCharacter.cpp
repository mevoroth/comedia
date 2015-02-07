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
	, MaxSpeedWhenGrab(0.2f)
	, DeccelMultiplier(2.25f)
	, AccelMultiplier(1.75f)
	, DeadZone(0.05f)
	, _GrabSpeedAlpha(1.f)
	, _GrabSpeedAlphaIt(-1.f)
	, GrabSpeedAlphaTimer(1.f)
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

	_LerpGrab(DeltaSeconds);

	_Controls(DeltaSeconds);
}

void ALiyaCharacter::_Controls(float DeltaSeconds)
{
	FVector2D TmpSpeed;
	if (Accel.SizeSquared() > 0.1f)
	{
		float Size = Speed.Size();
		Speed = Accel.SafeNormal() * (Size + Accel.Size() * AccelMultiplier * DeltaSeconds);
		//Speed += Accel.SafeNormal() * DeltaSeconds * AccelMultiplier;
		Size = Speed.SizeSquared();
		if (Size > MaxSpeed*MaxSpeed)
		{
			Speed = Speed.SafeNormal() * MaxSpeed;
		}

		if (Speed.SizeSquared() < DeadZone)
		{
			TmpSpeed = FVector2D(0.f, 0.f);
		}
		else
		{
			if (Speed.SizeSquared() > Accel.SizeSquared())
			{
				TmpSpeed = Speed.SafeNormal() * Accel.Size();
			}
			else
			{
				TmpSpeed = Speed;
			}
		}
		FQuat CameraOriented(FVector::UpVector, FMath::DegreesToRadians(Camera->GetComponentRotation().Yaw + FMath::RadiansToDegrees(FMath::Atan2(Speed.Y, Speed.X))));
		FQuat PosterOriented(FVector::UpVector, FMath::Atan2(_GrabDirection.Y, _GrabDirection.X));
		float LerpedRotation = FQuat::Slerp(CameraOriented, PosterOriented, _GrabSpeedAlpha / GrabSpeedAlphaTimer).Rotator().Yaw - 90.f;
		LerpedRotation = FMath::Fmod(LerpedRotation + 360.f, 360.f);
		//UE_LOG(LogGPCode, Warning, TEXT("LERPED : %f"), LerpedRotation);
		//float LerpedRotation = FMath::Lerp(
		//	Camera->GetComponentRotation().Yaw + FMath::RadiansToDegrees(FMath::Atan2(Speed.Y, Speed.X)),
		//	FMath::RadiansToDegrees(FMath::Atan2(_GrabDirection.Y, _GrabDirection.X)),
		//	_GrabSpeedAlpha / GrabSpeedAlphaTimer
		//) - 90.f;
		float FinalRotation = (_GrabSpeedAlphaIt < 0.f ? Camera->GetComponentRotation().Yaw + FMath::RadiansToDegrees(FMath::Atan2(Speed.Y, Speed.X)) : FMath::RadiansToDegrees(FMath::Atan2(_GrabDirection.Y, _GrabDirection.X))) - 90.f;
		FinalRotation = FMath::Fmod(FinalRotation + 360.f, 360.f);
		//UE_LOG(LogGPCode, Warning, TEXT("FINAL : %f"), FinalRotation);
		Rotation = (FMath::Abs(LerpedRotation - FinalRotation) < FMath::Abs(FMath::Fmod(Rotation + 360.f, 360.f) - FinalRotation) ? LerpedRotation : Rotation);
		//Rotation = Camera->GetComponentRotation().Yaw + FMath::RadiansToDegrees(FMath::Atan2(Speed.Y, Speed.X)) - 90.0f;
		//Rotation = FMath::RadiansToDegrees(FMath::Atan2(_GrabDirection.Y, _GrabDirection.X)) - 90.f;

		//UE_LOG(LogGPCode, Warning, TEXT("%s"), *_GrabDirection.ToString());
		//UE_LOG(LogGPCode, Warning, TEXT("%s"), *FVector(GetActorForwardVector().X, GetActorForwardVector().Y, 0.f).ToString());

		//Rotation = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(
		//	_GrabDirection,
		//	FVector(GetActorForwardVector().X, GetActorForwardVector().Y, 0.f)
		//)));

		//UE_LOG(LogGPCode, Warning, TEXT("%f"), Rotation);
	}
	else
	{
		Speed -= Speed.SafeNormal() * DeccelMultiplier * DeltaSeconds;
		if (Speed.SizeSquared() < DeadZone)
		{
			Speed = FVector2D(0.f, 0.f);
		}
		TmpSpeed = Speed;
	}

	TmpSpeed *= _CurrentSpeedMultiplier;

	AddMovementInput(Camera->GetForwardVector(), TmpSpeed.X);
	AddMovementInput(Camera->GetRightVector(), TmpSpeed.Y);

	Mesh->SetRelativeRotation(FRotator(
		0.f, Rotation, 0.f
	));


	Accel = FVector2D(0.f, 0.f);
}

void ALiyaCharacter::NotifyGrab()
{
	_GrabSpeedAlphaIt = 1.f;
}

void ALiyaCharacter::NotifyReleasePoster()
{
	_GrabSpeedAlphaIt = -1.f;
}

void ALiyaCharacter::_LerpGrab(float DeltaSeconds)
{
	_GrabSpeedAlpha = FMath::Clamp(_GrabSpeedAlpha + DeltaSeconds * _GrabSpeedAlphaIt, 0.f, GrabSpeedAlphaTimer);

	_CurrentSpeedMultiplier = FMath::Lerp(MaxSpeed, MaxSpeedWhenGrab, _GrabSpeedAlpha / GrabSpeedAlphaTimer);
}

void ALiyaCharacter::UpdateGrabPoint(const FVector& GrabPoint)
{
	_GrabDirection = GrabPoint - GetActorLocation();
	_GrabDirection.Z = 0.f;
	_GrabDirection = _GrabDirection.SafeNormal();
}
