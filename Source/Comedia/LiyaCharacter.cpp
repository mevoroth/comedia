// Fill out your copyright notice in the Description page of Project Settings.

#include "Comedia.h"
#include "LiyaCharacter.h"
#include "DamonFalconActor.h"
#include "IwacPlayerController.h"
#include "MainLevelScriptActor.h"

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
	, JumpHeight(17.5f)
	, CallCooldown(1.f)
	, _CallCooldown(0.f)
{
}

void ALiyaCharacter::BeginPlay()
{
	Super::BeginPlay();
	_GrabArmLength = FVector::Dist(GetMesh()->GetSocketLocation(FName(TEXT("ArmLengthStart"))), GetMesh()->GetSocketLocation(FName(TEXT("ArmLengthEnd"))));
	_GrabArmLength *= 2;
	_InitHeight = GetMesh()->GetComponentLocation().Z;
	_OriginalPivotCamPosition = Camera->RelativeLocation;
}

void ALiyaCharacter::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	// set up gameplay key bindings
	check(InputComponent);
	
	InputComponent->BindAxis(FName(TEXT("MoveForward")), this, &ALiyaCharacter::MoveForward);
	InputComponent->BindAxis(FName(TEXT("MoveRight")), this, &ALiyaCharacter::MoveRight);
	InputComponent->BindAxis(FName(TEXT("Turn")), this, &ALiyaCharacter::AddCameraRoll);
	InputComponent->BindAxis(FName(TEXT("LookUp")), this, &ALiyaCharacter::AddCameraPitch);

	InputComponent->BindAction(FName(TEXT("Call")), EInputEvent::IE_Pressed, this, &ALiyaCharacter::CallCharacter);
}

void ALiyaCharacter::MoveForward(float Val)
{
	if (Val != 0.0f)
	{
		_Accel.X = Val;
	}
}

void ALiyaCharacter::MoveRight(float Val)
{
	if (Val != 0.0f)
	{
		_Accel.Y = Val;
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

	_OverridingCamera(DeltaSeconds);

	_OverridingAudioListener();

	_CallCooldown -= DeltaSeconds;
}

void ALiyaCharacter::_OverridingAudioListener()
{
	AIwacPlayerController* PlayerController = Cast<AIwacPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	FVector CameraLocation = Camera->GetChildComponent(0)->GetComponentLocation();

	if (PlayerController)
	{
		FVector AudioListenerPosition = (CameraLocation + GetActorLocation()) / 2.0f;
		PlayerController->SetAudioListenerLocation(AudioListenerPosition);
	}
}

void ALiyaCharacter::_OverridingCamera(float DeltaSeconds)
{
	//Check if cam position overrided
	if (OverrideScriptedCameraPosition.GetLocation() != FVector::ZeroVector) //Travelling cam
	{

		if (ElapsedTravellingScriptedCamera < LengthTravellingScriptedCamera)
		{
			//Interpolate cam position and rotation
			ElapsedTravellingScriptedCamera += DeltaSeconds;
			float AlphaTravelling = FMath::Clamp(ElapsedTravellingScriptedCamera / LengthTravellingScriptedCamera, 0.0f, 1.0f);
			FVector CurrentCamLocation = FMath::Lerp<FVector>(StartTravellingPosition.GetLocation(), OverrideScriptedCameraPosition.GetLocation(), AlphaTravelling);
			FQuat CurrentCamQuat = FQuat::Slerp(StartTravellingPosition.GetRotation(), OverrideScriptedCameraPosition.GetRotation(), AlphaTravelling);
			Camera->SetWorldLocationAndRotation(CurrentCamLocation, CurrentCamQuat);
			Camera->AddRelativeLocation((GetActorLocation() - GrabbingPlayerLocation) * RatioCameraFollow);
			UE_LOG(LogGPCode, Log, TEXT("Fuck: %f"), AlphaTravelling);
		}
		else
		{
			Camera->SetWorldLocationAndRotation(OverrideScriptedCameraPosition.GetLocation(), OverrideScriptedCameraPosition.GetRotation());
			//Camera->SetWorldTransform(OverrideScriptedCameraPosition);
			Camera->AddRelativeLocation((GetActorLocation() - GrabbingPlayerLocation) * RatioCameraFollow);
		}

		//Ray cast to check collision
		FHitResult Hit(ForceInit);
		FCollisionQueryParams Trace(TEXT("CamTrace"), false, GetOwner());
		if (GetWorld()->LineTraceSingle(Hit, (GetActorLocation() + _OriginalPivotCamPosition), Camera->GetComponentLocation(), ECC_Visibility, Trace))
		{
			Camera->SetWorldLocation(Hit.ImpactPoint);
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
			StartTravellingPosition = Camera->ComponentToWorld;
			LastCamPosition = Camera->GetRelativeTransform();
		}
	}
}

void ALiyaCharacter::_Controls(float DeltaSeconds)
{
	FVector2D TmpSpeed;
	if (_Accel.SizeSquared() > DeadZone)
	{
		float Size = _Speed.Size();
		_Speed = _Accel.SafeNormal() * (Size + _Accel.SizeSquared() * AccelMultiplier * DeltaSeconds);
		//_Speed += _Accel.SafeNormal() * DeltaSeconds * AccelMultiplier;
		Size = _Speed.SizeSquared();
		if (Size > MaxSpeed*MaxSpeed)
		{
			_Speed = _Speed.SafeNormal() * MaxSpeed;
		}

		//if (_Speed.SizeSquared() < DeadZone)
		//{
		//	TmpSpeed = FVector2D(0.f, 0.f);
		//}
		//else
		//{

		float StickValue = FMath::Pow(_Accel.Size(), 1.5f);
		if (_Speed.Size() > StickValue)
		{
			TmpSpeed = _Speed.SafeNormal() * StickValue;
		}
		else
		{
			TmpSpeed = _Speed;
		}
		//}

		float RotationFromCamera = Camera->GetComponentRotation().Yaw + FMath::RadiansToDegrees(FMath::Atan2(_Speed.Y, _Speed.X));
		float RotationFromPoster = FMath::Atan2(_GrabDirection.Y, _GrabDirection.X);

		FQuat CameraOriented(FVector::UpVector, FMath::DegreesToRadians(RotationFromCamera));
		FQuat PosterOriented(FVector::UpVector, RotationFromPoster);
		
		float LerpedRotation = FQuat::Slerp(CameraOriented, PosterOriented, _GrabSpeedAlpha / GrabSpeedAlphaTimer).Rotator().Yaw - 90.f;
		LerpedRotation = FMath::Fmod(LerpedRotation + 360.f, 360.f);
		
		float FinalRotation = (_GrabSpeedAlphaIt < 0.f ? RotationFromCamera : FMath::RadiansToDegrees(RotationFromPoster)) - 90.f;
		FinalRotation = FMath::Fmod(FinalRotation + 360.f, 360.f);

		if (FMath::Abs(LerpedRotation - FinalRotation) < FMath::Abs(_Rotation - FinalRotation))
		{
			float AngularDelta = _Rotation - LerpedRotation;
			AngularDelta = FMath::Fmod(AngularDelta + 360.f, 360.f);
			if (AngularDelta > 180.f)
			{
				AngularDelta -= 360.f;
			}
			if (FMath::Abs(AngularDelta) / DeltaSeconds > MaxAngularSpeed)
			{
				_Rotation -= FMath::Sign(AngularDelta) * MaxAngularSpeed * DeltaSeconds;
			}
			else
			{
				_Rotation = LerpedRotation;
			}
		}
	}
	else
	{
		_Speed -= _Speed.SafeNormal() * DeccelMultiplier * DeltaSeconds;
		if (_Speed.SizeSquared() < DeadZone)
		{
			_Speed = FVector2D(0.f, 0.f);
		}
		TmpSpeed = _Speed;
	}

	TmpSpeed *= _CurrentSpeedMultiplier;
	_RunningSpeedAnimBP = TmpSpeed.Size();

	_ControlsMove(TmpSpeed);

	GetMesh()->SetRelativeRotation(FRotator(
		0.f, _Rotation, 0.f
	));


	_Accel = FVector2D(0.f, 0.f);
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
	_GrabMaxDistance = PosterMaxDistance;
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
		_InitHeight + Height * (_RunningSpeedAnimBP / (_CurrentSpeedMultiplier * MaxSpeed)) * JumpHeight
	));
}

void ALiyaCharacter::CallCharacter()
{
	if (_CallCooldown <= 0.f)
	{
		AMainLevelScriptActor* LevelScript = Cast<AMainLevelScriptActor>(GetWorld()->GetLevelScriptActor());

		if (!LevelScript)
		{
			UE_LOG(LogGPCode, Error, TEXT("MainLevelScriptActor unfound"));
			return;
		}

		TArray<AActor*> Posters;
		GetOverlappingActors(Posters, APosterActor::StaticClass());

		if (Posters.Num() > 1)
		{
			UE_LOG(LogGPCode, Error, TEXT("Poster triggers overlapping"));
			return;
		}

		if (Posters.Num() == 1)
		{
			APosterActor* Poster = Cast<APosterActor>(Posters[0]);
			if (LevelScript->PathMainCharacter.MoveTo(LevelScript->CurrentLevelPathGraph.GetNode(GetActorLocation(), Poster)))
			{
				OncallCharacter();
				_CallCooldown = CallCooldown;
			}
		}

		//AMainLevelScriptActor* LevelScript = Cast<AMainLevelScriptActor>(GetWorld()->GetLevelScriptActor());
		//if (LevelScript)
		//{
		//	const PathNode* Node = LevelScript->GetPathGraph().GetNearestNode(GetActorLocation());
		//	if (LevelScript->GetPathGraph().MoveCharacterTo(Node))
		//	{
		//		_CallCooldown = CallCooldown;
		//	}
		//}
	}
}

//void ALiyaCharacter::Oncall_Implementation()
//{
//
//}
