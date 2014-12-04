// Fill out your copyright notice in the Description page of Project Settings.

#include "Comedia.h"
#include "MyLevelScriptActor.h"


AMyLevelScriptActor::AMyLevelScriptActor(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	//Get blueprint class for knife instantiation
	static ConstructorHelpers::FClassFinder<AKnifeCharacter> KnifeCharacterClassFinder(TEXT("/Game/Blueprints/Knife"));
	KnifeClass = KnifeCharacterClassFinder.Class;

	//Enable tick function
	PrimaryActorTick.bCanEverTick = true;
}

void AMyLevelScriptActor::BeginPlay()
{
	Super::BeginPlay();

	//Get player character
	PlayerCharacter = GetWorld()->GetFirstPlayerController()->GetCharacter();
}

void AMyLevelScriptActor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	//Increase total elapsed time
	TotalElapsedTime += DeltaSeconds;

	//Spawn a knife
	if (TotalElapsedTime > DelayFirstKnifeSpawn) //FIXME: Will spawn when previous knife done
	{
		TotalElapsedTime -= DelayFirstKnifeSpawn; //FIXME: Will spawn when previous knife done

		//Draw spawn area around the player
		float ComputedRadiusSpawnKnifeArea = PlayerCharacter->CapsuleComponent->GetScaledCapsuleHalfHeight() * RadiusSpawnKnifeArea;
		DrawDebugSphere(GetWorld(), PlayerCharacter->GetActorLocation(), ComputedRadiusSpawnKnifeArea, 32, FColor::Red);

		KnifeSpawning(ComputedRadiusSpawnKnifeArea);
	}
}

void AMyLevelScriptActor::KnifeSpawning(float ComputedRadiusSpawnKnifeArea)
{
	//Spawn knife
	AKnifeCharacter* SpawnedKnifeCharacter = GetWorld()->SpawnActor<AKnifeCharacter>(KnifeClass);
	SpawnedKnifeCharacter->SetActorLocation(PlayerCharacter->GetActorLocation() + FVector::UpVector * 500.0f);
	SpawnedKnifeCharacter->SpawnDefaultController();

	//Rotate to a random angle
	FRotator RotationKnife(0.0f, FMath::FRand() * 360.0f, 0.0f);
	SpawnedKnifeCharacter->SetActorRelativeRotation(RotationKnife);

	//Move to border of the area
	SpawnedKnifeCharacter->SetActorLocation(SpawnedKnifeCharacter->GetActorLocation() + SpawnedKnifeCharacter->GetActorForwardVector() * ComputedRadiusSpawnKnifeArea);

	//Look at player character
	FRotator RotationBackKnife(0.0f, 180.0f, 0.0f);
	SpawnedKnifeCharacter->AddActorLocalRotation(RotationBackKnife);

	//Set character velocity
	FVector DirectionKnifeCharacter = PlayerCharacter->GetActorLocation() - SpawnedKnifeCharacter->GetActorLocation();
	DirectionKnifeCharacter.Normalize();
	float KnifeCharacterVelocity = 10000.0f;
	SpawnedKnifeCharacter->CharacterMovement->RequestDirectMove(DirectionKnifeCharacter * KnifeCharacterVelocity, false);
}
