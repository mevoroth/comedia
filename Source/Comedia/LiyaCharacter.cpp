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
	, MaxAngularSpeed(200.f)
	, _RunningSpeedAnimBP(0.f)
{
}

void ALiyaCharacter::BeginPlay()
{
	Super::BeginPlay();
	_GrabArmLength = FVector::Dist(GetMesh()->GetSocketLocation(FName(TEXT("ArmLengthStart"))), GetMesh()->GetSocketLocation(FName(TEXT("ArmLengthEnd"))));
	_GrabArmLength *= 2;
	_InitHeight = GetMesh()->GetComponentLocation().Z;
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

	//Check if cam position overrided
	if (OverrideScriptedCameraPosition) //Travelling cam
	{

		if (ElapsedTravellingScriptedCamera < LengthTravellingScriptedCamera)
		{
			//Interpolate cam position and rotation
			ElapsedTravellingScriptedCamera += DeltaSeconds;
			float AlphaTravelling = ElapsedTravellingScriptedCamera / LengthTravellingScriptedCamera;
			FVector CurrentCamLocation = FMath::Lerp<FVector>(StartTravellingPosition.GetLocation(), OverrideScriptedCameraPosition->ComponentToWorld.GetLocation(), AlphaTravelling);
			FQuat CurrentCamQuat = FQuat::Slerp(StartTravellingPosition.GetRotation(), OverrideScriptedCameraPosition->GetComponentQuat(), AlphaTravelling);
			Camera->SetWorldLocationAndRotation(CurrentCamLocation, CurrentCamQuat);
			GrabbingPlayerLocation = GetActorLocation();
		}
		else
		{
			Camera->SetWorldTransform(OverrideScriptedCameraPosition->ComponentToWorld);
			Camera->AddRelativeLocation((GetActorLocation() - GrabbingPlayerLocation) * RatioCameraFollow);
		}
	}
	else
	{
		if (ElapsedTravellingScriptedCamera > 0.0f) //Travelling back
		{
			//Interpolate cam position and rotation
			ElapsedTravellingScriptedCamera -= DeltaSeconds;
			float AlphaTravelling = (LengthTravellingBackScriptedCamera - ElapsedTravellingScriptedCamera) / LengthTravellingBackScriptedCamera;
			FVector CurrentCamLocation = FMath::Lerp<FVector>(StartTravellingPosition.GetLocation(), LastCamPosition.GetLocation(), AlphaTravelling);
			FQuat CurrentCamQuat = FQuat::Slerp(StartTravellingPosition.GetRotation(), LastCamPosition.GetRotation(), AlphaTravelling);

			Camera->SetRelativeTransform(FTransform(CurrentCamQuat, CurrentCamLocation));
		}
		else
		{
			LastCamPosition = Camera->GetRelativeTransform();
			StartTravellingPosition = Camera->ComponentToWorld;
		}
	}
}

void ALiyaCharacter::_Controls(float DeltaSeconds)
{
	FVector2D TmpSpeed;
	if (Accel.SizeSquared() > 0.1f)
	{
		float Size = Speed.Size();
		Speed = Accel.SafeNormal() * (Size + Accel.SizeSquared() * AccelMultiplier * DeltaSeconds);
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

		float RotationFromCamera = Camera->GetComponentRotation().Yaw + FMath::RadiansToDegrees(FMath::Atan2(Speed.Y, Speed.X));
		float RotationFromPoster = FMath::Atan2(_GrabDirection.Y, _GrabDirection.X);

		FQuat CameraOriented(FVector::UpVector, FMath::DegreesToRadians(RotationFromCamera));
		FQuat PosterOriented(FVector::UpVector, RotationFromPoster);
		
		float LerpedRotation = FQuat::Slerp(CameraOriented, PosterOriented, _GrabSpeedAlpha / GrabSpeedAlphaTimer).Rotator().Yaw - 90.f;
		LerpedRotation = FMath::Fmod(LerpedRotation + 360.f, 360.f);
		
		float FinalRotation = (_GrabSpeedAlphaIt < 0.f ? RotationFromCamera : FMath::RadiansToDegrees(RotationFromPoster)) - 90.f;
		FinalRotation = FMath::Fmod(FinalRotation + 360.f, 360.f);

		if (FMath::Abs(LerpedRotation - FinalRotation) < FMath::Abs(Rotation - FinalRotation))
		{
			float AngularDelta = Rotation - LerpedRotation;
			AngularDelta = FMath::Fmod(AngularDelta + 360.f, 360.f);
			if (AngularDelta > 180.f)
			{
				AngularDelta -= 360.f;
			}
			if (FMath::Abs(AngularDelta) / DeltaSeconds > MaxAngularSpeed)
			{
				Rotation -= FMath::Sign(AngularDelta) * MaxAngularSpeed * DeltaSeconds;
			}
			else
			{
				Rotation = LerpedRotation;
			}
		}
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
	_RunningSpeedAnimBP = TmpSpeed.Size();

	_ControlsMove(TmpSpeed);

	Mesh->SetRelativeRotation(FRotator(
		0.f, Rotation, 0.f
	));


	Accel = FVector2D(0.f, 0.f);
}

void ALiyaCharacter::_ControlsMove(const FVector2D& Speed)
{
	FVector Fw = Camera->GetForwardVector();
	FVector Right = Camera->GetRightVector();
	Fw.Z = 0.f;
	Right.Z = 0.f;
	Fw.Normalize();
	Right.Normalize();
	AddMovementInput(Fw, Speed.X);
	AddMovementInput(Right, Speed.Y);

	FVector NewPos = ConsumeMovementInputVector() + GetActorLocation();
	FVector GrabPivot = _GrabPivot;
	GrabPivot.Z = 0.f;
	NewPos.Z = 0.f;

	float CurrentDist = FVector::Dist(NewPos, GrabPivot) - _GrabArmLength;
	if (!GetGrabbing() || CurrentDist < FMath::Sqrt(_GrabMaxDistance) || CurrentDist < _GrabPreviousDistance)
	{
		AddMovementInput(Fw, Speed.X);
		AddMovementInput(Right, Speed.Y);
	}
	_GrabPreviousDistance = CurrentDist;
}

void ALiyaCharacter::NotifyGrab(float PosterMaxDistance)
{
	_GrabSpeedAlphaIt = 1.f;
	_GrabMaxDistance = PosterMaxDistance * PosterMaxDistance;
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

float ALiyaCharacter::GetRunningSpeedAnimBP() const
{
	return _RunningSpeedAnimBP;
}

bool ALiyaCharacter::GetGrabbing() const
{
	return _GrabSpeedAlphaIt > 0.f;
}

void ALiyaCharacter::UpdateGrabPivot(const FVector& GrabPivot)
{
	_GrabPivot = GrabPivot;
}

void ALiyaCharacter::SetHeightDisplacement(float Height)
{
	GetMesh()->SetWorldLocation(FVector(
		GetMesh()->GetComponentLocation().X,
		GetMesh()->GetComponentLocation().Y,
		_InitHeight + Height * (_RunningSpeedAnimBP / _CurrentSpeedMultiplier) * 15.f
	));
	UE_LOG(LogGPCode, Warning, TEXT("%f"), );
}
