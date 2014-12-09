// Fill out your copyright notice in the Description page of Project Settings.

#include "Comedia.h"
#include "KnifeCharacter.h"
#include "LiyaCharacter.h"
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
			DestroyKnife();
		}
	}

	//Check if need to spawn a new decal
	if ((GetActorLocation() - LastDecalPosition).Size() >= DecalLength)
	{
		SpawnDecal();
	}
}

void AKnifeCharacter::ReceiveHit(class UPrimitiveComponent* MyComp, AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::ReceiveHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);

	//Check if receive hit from player
	if (Cast<ALiyaCharacter>(Other))
	{
		UE_LOG(LogGPCode, Log, TEXT("Knife collide with player"));
		DestroyKnife();
	}
}

void AKnifeCharacter::InitOriginalPosition()
{
	LastDecalPosition = GetActorLocation();
}

void AKnifeCharacter::DestroyKnife()
{
	AMyLevelScriptActor* MyLevelScript = Cast<AMyLevelScriptActor>(GetWorld()->GetLevelScriptActor());
	if (MyLevelScript)
	{
		UE_LOG(LogGPCode, Log, TEXT("Destroy Knife"));
		MyLevelScript->bHasKnifeSpawned = false;
		GetWorld()->DestroyActor(this);
	}
}

void AKnifeCharacter::SpawnDecal()
{
	//Spawn, set position and set scale
	ADecalActor* NewDecal = GetWorld()->SpawnActor<ADecalActor>();
	NewDecal->SetLifeSpan(DecalLifeSpan);

	NewDecal->SetActorLocation((LastDecalPosition + GetActorLocation()) / 2.0f);
	NewDecal->SetActorScale3D(FVector(512.0f, DecalWidth, DecalLength));

	//Compute rotation
	FVector DecalDirection = GetActorLocation() - NewDecal->GetActorLocation();
	DecalDirection.Normalize();
	float AngleDecal = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(DecalDirection, FVector::ForwardVector)));
	float SignAngleDecal = FMath::Sign<float>(FVector::CrossProduct(FVector::ForwardVector, DecalDirection).Z);
	FRotator RotationDecal = FRotator(0.0f, 0.0f, AngleDecal * SignAngleDecal);
	NewDecal->AddActorLocalRotation(RotationDecal);

	//Set decal material
	NewDecal->Decal->SetDecalMaterial(DecalMaterial);

	LastDecalPosition = GetActorLocation();
}
