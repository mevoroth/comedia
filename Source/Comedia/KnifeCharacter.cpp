// Fill out your copyright notice in the Description page of Project Settings.

#include "Comedia.h"
#include "KnifeCharacter.h"
#include "MyLevelScriptActor.h"


AKnifeCharacter::AKnifeCharacter(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	//Enable tick function
	PrimaryActorTick.bCanEverTick = true;
}

void AKnifeCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	//Compute rotation stuff
	FVector KnifeToPlayerDirVector = GetWorld()->GetFirstPlayerController()->GetCharacter()->GetActorLocation() - GetActorLocation();
	KnifeToPlayerDirVector.Normalize();
	float AngleForwardToPlayer = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(KnifeToPlayerDirVector, GetActorForwardVector())));
	float DirectionAngleForwardPlayer = FMath::Sign<float>(FVector::CrossProduct(GetActorForwardVector(), KnifeToPlayerDirVector).Z);
	
	//Apply rotation stuff
	float MaxRotationAngle = MaxRotationSpeed * DeltaSeconds;
	FRotator RotationToPlayer;
	if (AngleForwardToPlayer < MaxRotationAngle)
	{
		RotationToPlayer = FRotator(0.0f, AngleForwardToPlayer * DirectionAngleForwardPlayer, 0.0f);
	}
	else
	{
		RotationToPlayer = FRotator(0.0f, MaxRotationAngle * DirectionAngleForwardPlayer, 0.0f);
	}
	AddActorLocalRotation(RotationToPlayer);

	//Set character move
	CharacterMovement->SetMovementMode(EMovementMode::MOVE_Flying);
	CharacterMovement->MaxFlySpeed = KnifeSpeed;
	CharacterMovement->RequestDirectMove(GetActorForwardVector(), true);

	//Check knife destroyed
	AMyLevelScriptActor* MyLevelScript = Cast<AMyLevelScriptActor>(GetWorld()->GetLevelScriptActor());
	if (MyLevelScript)
	{
		//Check if distance player/knife more than ComputedRadiusSpawnKnifeArea and if player is behind knife
		if ((GetWorld()->GetFirstPlayerController()->GetCharacter()->GetActorLocation() - GetActorLocation()).Size() > MyLevelScript->ComputedRadiusSpawnKnifeArea && AngleForwardToPlayer > 90.0f)
		{
			UE_LOG(LogGPCode, Log, TEXT("Destroy Knife"));
			MyLevelScript->bHasKnifeSpawned = false;
			GetWorld()->DestroyActor(this);
		}
	}
}
