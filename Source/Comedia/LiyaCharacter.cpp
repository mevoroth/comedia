// Fill out your copyright notice in the Description page of Project Settings.

#include "Comedia.h"
#include "LiyaCharacter.h"
#include "DamonFalconActor.h"


ALiyaCharacter::ALiyaCharacter(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
	, Camera(0)
	, DeltaZLeft(0)
	, DeltaZRight(0)
{
	LeftFootFiltered = PCIP.CreateDefaultSubobject<ULowPassFilterComponent>(this, "Left Foot (low pass-filtered)");
	RightFootFiltered = PCIP.CreateDefaultSubobject<ULowPassFilterComponent>(this, "Right Foot (right pass-filtered)");
	LeftFootFiltered->InitRecord(FOOTSTEP_LATENCY);
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

	FootSteps(/*DeltaSeconds*/);
}

bool ALiyaCharacter::Internal_CheckFootstep(const FVector& FootPos, TSubobjectPtr<ULowPassFilterComponent>& Filter, float& oldZ, float& oldZDerivative)
{
	FHitResult Hit(ForceInit);
	FCollisionQueryParams Trace(TEXT("FootStepTrace"), false, GetOwner());

	GetWorld()->LineTraceSingle(Hit, FootPos, FootPos + FVector::UpVector * -100.f, ECC_Visibility, Trace);

	Filter->Push(FootPos.Z - Hit.Location.Z);
	float currentZ = Filter->GetCurrentRecord();
	float currentZDerivative = currentZ - oldZ;

	bool bFootStep = currentZDerivative >= 0 && oldZDerivative < 0;
	
	oldZ = currentZ;
	oldZDerivative = currentZDerivative;

	UE_LOG(LogGPCode, Warning, TEXT("%f;%f;%s"), currentZ, currentZDerivative, (bFootStep ? TEXT("YES") : TEXT("NO")));

	return bFootStep;
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

void ALiyaCharacter::FootSteps(/*float DeltaSeconds*/)
{
	FVector Pos;
	//Mesh->GetSocketWorldLocationAndRotation(TEXT("LeftFootSocket"), Pos, Rot);
	//UE_LOG(LogGPCode, Warning, TEXT("%f"), Mesh->GetSocketLocation(TEXT("LeftFootSocket")).Z);
	if (Internal_CheckFootstep(Mesh->GetSocketLocation(TEXT("LeftFootSocket")), LeftFootFiltered, DeltaZLeft, DeltaDerivZLeft))
	{
		LeftFootStep();
	}
	//else if (Internal_CheckFootstep(Mesh->GetSocketLocation(TEXT("RightFootSocket")), DeltaZLeft, DeltaDerivZLeft))
	//{
	//	RightFootStep();
	//}
}
