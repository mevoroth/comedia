// Fill out your copyright notice in the Description page of Project Settings.

#include "Comedia.h"
#include "LiyaCharacter.h"


ALiyaCharacter::ALiyaCharacter(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	Camera = 0;
	//Camera = PCIP.CreateDefaultSubobject<ULiyaCamera>(this, TEXT("Camera"));
	//Camera->SetRelativeLocation(FVector(-190.f, 0.f, 190.f));
	//Camera->SetRelativeRotation(FRotator(0.f, -40.f, 0.f));
	//Camera->AttachTo(RootComponent);
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

	//InputComponent->BindAxis("Turn", this, &ALiyaCharacter::AddControllerYawInput);
	//InputComponent->BindAxis("LookUp", this, &ALiyaCharacter::AddControllerRollInput);
	//InputComponent->BindAxis("LookDown", this, &ALiyaCharacter::AddControllerRollInput);
}

void ALiyaCharacter::MoveForward(float Val)
{
	if (Val != 0.0f)
	{
		AddMovementInput(GetActorForwardVector(), Val);
	}
}

void ALiyaCharacter::MoveRight(float Val)
{
	if (Val != 0.0f)
	{
		AddMovementInput(GetActorRightVector(), Val);
	}
}

void ALiyaCharacter::AddControllerYawInput(float Val)
{
	Super::AddControllerYawInput(Val * Camera->GetCameraSpeed());

}

void ALiyaCharacter::AddControllerRollInput(float Val)
{
	Super::AddControllerRollInput(Val * Camera->GetCameraSpeed());
	Camera->AddRoll(Val);
}
