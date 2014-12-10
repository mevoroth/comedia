// Fill out your copyright notice in the Description page of Project Settings.

#include "Comedia.h"
#include "IwacLevelScriptActor.h"


AIwacLevelScriptActor::AIwacLevelScriptActor(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	//Get blueprint class for knife instantiation
	static ConstructorHelpers::FClassFinder<AKnifeCharacter> KnifeCharacterClassFinder(TEXT("/Game/Blueprints/Knife"));
	KnifeClass = KnifeCharacterClassFinder.Class;

	//Enable tick function
	PrimaryActorTick.bCanEverTick = true;

	bHasKnifeSpawned = false;
	NbSpawnedKnife = 0;
}

void AIwacLevelScriptActor::BeginPlay()
{
	Super::BeginPlay();

	//Get player character
	PlayerCharacter = GetWorld()->GetFirstPlayerController()->GetCharacter();

	//Init remaining time with DelayFirstKnifeSpawn
	RemainingTime = DelayFirstKnifeSpawn;
}

void AIwacLevelScriptActor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	//Draw spawn area around the player
	ComputedRadiusSpawnKnifeArea = PlayerCharacter->CapsuleComponent->GetScaledCapsuleHalfHeight() * RadiusSpawnKnifeArea;
	DrawDebugSphere(GetWorld(), PlayerCharacter->GetActorLocation(), ComputedRadiusSpawnKnifeArea, 32, FColor::Red);

	//Check if no knife spawned
	if (!bHasKnifeSpawned)
	{
		//Decreased remaining time before next knife spawn
		RemainingTime -= DeltaSeconds;

		//Knife spawn when no more remaining time
		if (RemainingTime <= 0.0f)
		{
			KnifeSpawning(ComputedRadiusSpawnKnifeArea);
		}
	}
}

void AIwacLevelScriptActor::KnifeSpawning(float ComputedRadiusSpawnKnifeArea)
{
	UE_LOG(LogGPCode, Log, TEXT("Spawn Knife"));
	bHasKnifeSpawned = true;
	NbSpawnedKnife++;

	//Set remaining time with DelayBetweenKnifeSpawn for next knife spawn
	RemainingTime = DelayBetweenKnifeSpawn;

	//Spawn knife
	AKnifeCharacter* SpawnedKnifeCharacter = GetWorld()->SpawnActor<AKnifeCharacter>(KnifeClass);
	FVector SpawningKnifePosition = PlayerCharacter->GetActorLocation();
	SpawnedKnifeCharacter->SetActorLocation(SpawningKnifePosition);
	SpawnedKnifeCharacter->SpawnDefaultController();

	//Rotate to a random angle
	FRotator RotationKnife(0.0f, FMath::FRand() * 360.0f, 0.0f);
	SpawnedKnifeCharacter->SetActorRelativeRotation(RotationKnife);

	//Move to border of the area
	SpawnedKnifeCharacter->SetActorLocation(SpawnedKnifeCharacter->GetActorLocation() + SpawnedKnifeCharacter->GetActorForwardVector() * ComputedRadiusSpawnKnifeArea);

	//Look at player character
	FRotator RotationBackKnife(0.0f, 180.0f, 0.0f);
	SpawnedKnifeCharacter->AddActorLocalRotation(RotationBackKnife);

	SpawnedKnifeCharacter->InitOriginalPosition();
}
