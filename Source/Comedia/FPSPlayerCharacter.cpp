// Fill out your copyright notice in the Description page of Project Settings.

#include "Comedia.h"
#include "FPSPlayerCharacter.h"


AFPSPlayerCharacter::AFPSPlayerCharacter(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{

}

void AFPSPlayerCharacter::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	// set up gameplay key bindings
	check(InputComponent);

	InputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	InputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	InputComponent->BindAxis("MoveForward", this, &AFPSPlayerCharacter::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &AFPSPlayerCharacter::MoveRight);
	InputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	InputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
}

void AFPSPlayerCharacter::MoveForward(float Val)
{
	if (Val != 0.0f)
	{
		AddMovementInput(GetActorForwardVector(), Val);
	}
}

void AFPSPlayerCharacter::MoveRight(float Val)
{
	if (Val != 0.0f)
	{
		AddMovementInput(GetActorRightVector(), Val);
	}
}


