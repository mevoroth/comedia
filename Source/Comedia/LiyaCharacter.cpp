// Fill out your copyright notice in the Description page of Project Settings.

#include "Comedia.h"
#include "LiyaCharacter.h"
#include "DamonFalconActor.h"

#include <limits>

const float ALiyaCharacter::WAIT_BEFORE_THRESHOLD = 1.f;

ALiyaCharacter::ALiyaCharacter(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
	, Camera(0)
	, DeltaZLeft(0)
	, DeltaZRight(0)
	, LeftFootThreshold(std::numeric_limits<float>::quiet_NaN())
	, RightFootThreshold(std::numeric_limits<float>::quiet_NaN())
	, LeftFootLastTime(-std::numeric_limits<float>::infinity())
	, RightFootLastTime(-std::numeric_limits<float>::infinity())
{
	LeftFootFiltered = PCIP.CreateDefaultSubobject<ULowPassFilterComponent>(this, "Left Foot (low pass-filtered)");
	LeftFootFiltered->InitRecord(FOOTSTEP_LATENCY);

	RightFootFiltered = PCIP.CreateDefaultSubobject<ULowPassFilterComponent>(this, "Right Foot (right pass-filtered)");
	RightFootFiltered->InitRecord(FOOTSTEP_LATENCY);
}

void ALiyaCharacter::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	// set up gameplay key bindings
	check(InputComponent);
	
	// Disable jump
	//InputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	//InputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	
	InputComponent->BindAxis("MoveForward", this, &ALiyaCharacter::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &ALiyaCharacter::MoveRight);
	InputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);

	/*******************************************/
	/*           <3 MONSEIGNEUR LOUIS LOUIS    */
	/*******************************************/

	InputComponent->BindAxis("Turn", this, &ALiyaCharacter::AddControllerYawInput);
	InputComponent->BindAxis("LookUp", this, &ALiyaCharacter::AddCameraPitch);
	//InputComponent->BindAxis("LookDown", this, &ALiyaCharacter::AddControllerPitchInput);
}

void ALiyaCharacter::MoveForward(float Val)
{
	if (Val != 0.0f)
	{
		AddMovementInput(GetActorForwardVector(), Val * GetWorld()->GetDeltaSeconds() * CharacterSpeed);
	}
}

void ALiyaCharacter::MoveRight(float Val)
{
	if (Val != 0.0f)
	{
		AddMovementInput(GetActorRightVector(), Val * GetWorld()->GetDeltaSeconds() * CharacterSpeed);
	}
}

void ALiyaCharacter::AddControllerYawInput(float Val)
{
	Super::AddControllerYawInput(Val * GetWorld()->GetDeltaSeconds() * CameraSpeed);
}

void ALiyaCharacter::AddCameraPitch(float Val)
{
	check(Camera); // Camera not set in BP

	Camera->AddRelativeRotation(FRotator(
		Val * GetWorld()->GetDeltaSeconds() * CameraSpeed, 0.f, 0.f
	));

	if (Camera->RelativeRotation.Pitch < MinCamPitch)
	{
		Camera->RelativeRotation.Pitch = MinCamPitch;
	}
	else if (Camera->RelativeRotation.Pitch > MaxCamPitch)
	{
		Camera->RelativeRotation.Pitch = MaxCamPitch;
	}
}

void ALiyaCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	FootSteps(DeltaSeconds);
}

float ALiyaCharacter::Internal_FootStepDistance(const FVector& FootPos) const
{
	FHitResult Hit(ForceInit);
	FCollisionQueryParams Trace(TEXT("FootStepTrace"), false, GetOwner());

	GetWorld()->LineTraceSingle(Hit, FootPos, FootPos + FVector::UpVector * -100.f, ECC_Visibility, Trace);

	//UE_LOG(LogGPCode, Warning, TEXT("Now: %f"), FootPos.Z - Hit.Location.Z);
	return FootPos.Z - Hit.Location.Z;
}

bool ALiyaCharacter::Internal_CheckFootstep(const FVector& FootPos, float Threshold, TSubobjectPtr<ULowPassFilterComponent>& Filter, FootAnimationState& FootState, float& oldZ, float& oldZDerivative)
{
	Filter->Push(Internal_FootStepDistance(FootPos));
	float currentZ = Filter->GetCurrentRecord();
	float currentZDerivative = currentZ - oldZ;

	bool bFootStep = currentZDerivative > 0.01f && oldZDerivative < -0.01f && currentZ < Threshold;
	//if (bFootStep)
	//	UE_LOG(LogGPCode, Warning, TEXT("Prev : %f; Next : %f"), oldZDerivative, currentZDerivative);
	
	oldZ = currentZ;
	oldZDerivative = currentZDerivative;

	//UE_LOG(LogGPCode, Warning, TEXT("%f;%f;%s"), currentZ, currentZDerivative, (bFootStep ? TEXT("YES") : TEXT("NO")));

	return bFootStep;
}
void ALiyaCharacter::FootSteps(float DeltaSeconds)
{
	ElapsedTime += DeltaSeconds;
	if (ElapsedTime < WAIT_BEFORE_THRESHOLD)
	{
		return;
	}
	else if (LeftFootThreshold != LeftFootThreshold) // means isNaN()
	{
		LeftFootThreshold = Internal_FootStepDistance(Mesh->GetSocketLocation(TEXT("LeftFootSocket")));
		RightFootThreshold = Internal_FootStepDistance(Mesh->GetSocketLocation(TEXT("RightFootSocket")));
		return;
	}
	FVector Pos;
	if (Internal_CheckFootstep(Pos = Mesh->GetSocketLocation(TEXT("LeftFootSocket")), LeftFootThreshold, LeftFootFiltered, LeftFootState, DeltaZLeft, DeltaDerivZLeft))
	{
		if (ElapsedTime - LeftFootLastTime > 0.2f)
		{
			LeftFootStep(Pos);
			LeftFootLastTime = ElapsedTime;
		}
	}
	else if (Internal_CheckFootstep(Pos = Mesh->GetSocketLocation(TEXT("RightFootSocket")), RightFootThreshold, RightFootFiltered, RightFootState, DeltaZRight, DeltaDerivZRight))
	{
		if (ElapsedTime - RightFootLastTime > 0.2f)
		{
			RightFootStep(Pos);
			RightFootLastTime = ElapsedTime;
		}
	}
}

//
//void ALiyaCharacter::LeftFootStep()
//{
//	UE_LOG(LogGPCode, Warning, TEXT("Internal Code Left Foot"));
//}
//
//void ALiyaCharacter::RightFootStep()
//{
//	UE_LOG(LogGPCode, Warning, TEXT("Internal Code Right Foot"));
//}
