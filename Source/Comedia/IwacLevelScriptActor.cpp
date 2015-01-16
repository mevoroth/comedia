// Fill out your copyright notice in the Description page of Project Settings.

#include "Comedia.h"
#include "IwacLevelScriptActor.h"


AIwacLevelScriptActor::AIwacLevelScriptActor(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	//Get blueprint class for knife instantiation
	static ConstructorHelpers::FClassFinder<AKnifeCharacter> KnifeCharacterClassFinder(TEXT("/Game/Blueprints/Knife"));
	_KnifeClass = KnifeCharacterClassFinder.Class;

	//Get blueprint class for lightning instantiation
	static ConstructorHelpers::FClassFinder<ALightningActor> LightningActorClassFinder(TEXT("/Game/Blueprints/Lightning"));
	_LightningClass = LightningActorClassFinder.Class;

	//Enable tick function
	PrimaryActorTick.bCanEverTick = true;

	_PreviousTorturePhase = ETorturePhase::TP_EmptyPhase;

	bHasKnifeSpawned = false;
	CurrentNbLightning = 0;
}

void AIwacLevelScriptActor::BeginPlay()
{
	Super::BeginPlay();

	//Get player character
	_PlayerCharacter = GetWorld()->GetFirstPlayerController()->GetCharacter();

	//Init remaining time with DelayFirstKnifeSpawn
	_RemainingTime = DelayFirstSpawn;
}

void AIwacLevelScriptActor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	//Check if phase changed
	if (TorturePhase != _PreviousTorturePhase)
	{
		const UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("ETorturePhase"), true);
		if (EnumPtr)
		{
			UE_LOG(LogGPCode, Log, TEXT("Change to phase: %s"), *EnumPtr->GetEnumName(TorturePhase));

			//Reset new phase
			switch (TorturePhase)
			{
				case ETorturePhase::TP_KnifePhase:
					NbSpawnedKnife = 0;
					break;

				case ETorturePhase::TP_LightningPhase:
					TimeSpendLightningPhase = 0.0f;
					break;

				case ETorturePhase::TP_IronPhase:
					TimeSpendIronPhase = 0.0f;
					break;

				case ETorturePhase::TP_EmptyPhase:
				default:
					break;
			}
		}
		_PreviousTorturePhase = TorturePhase;
	}

	//Tick current torture phase
	switch (TorturePhase)
	{
		case ETorturePhase::TP_KnifePhase:
			_TickKnifePhase(DeltaSeconds);
			break;

		case ETorturePhase::TP_LightningPhase:
			_TickLightningPhase(DeltaSeconds);
			break;

		case ETorturePhase::TP_IronPhase:
			_TickIronPhase(DeltaSeconds);
			break;

		case ETorturePhase::TP_EmptyPhase:
		default:
			break;
	}
}

void AIwacLevelScriptActor::_TickKnifePhase(float DeltaSeconds)
{
	//Draw spawn area around the player
	ComputedRadiusSpawnKnifeArea = _PlayerCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * RadiusSpawnKnifeArea;
	DrawDebugSphere(GetWorld(), _PlayerCharacter->GetActorLocation(), ComputedRadiusSpawnKnifeArea, 32, FColor::Red);

	//Check if no knife spawned
	if (!bHasKnifeSpawned)
	{
		//Decreased remaining time before next knife spawn
		_RemainingTime -= DeltaSeconds;

		//Knife spawn when no more remaining time
		if (_RemainingTime <= 0.0f)
		{
			_KnifeSpawning(ComputedRadiusSpawnKnifeArea);
		}
	}
}

void AIwacLevelScriptActor::_TickLightningPhase(float DeltaSeconds)
{
	 //Draw spawn area around the player
	float ComputedRadiusSpawnLightningArea = _PlayerCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * RadiusSpawnLightningArea;
	DrawDebugSphere(GetWorld(), _PlayerCharacter->GetActorLocation(), ComputedRadiusSpawnLightningArea, 32, FColor::Blue);

	//Increase time spend in lightning phase
	TimeSpendLightningPhase += DeltaSeconds;

	//Check if lightning limit not reached
	if (CurrentNbLightning < MaxNbLightning)
	{
		//Decrease remaining time before next lightning spawn
		_RemainingTime -= DeltaSeconds;

		if (_RemainingTime <= 0.0f)
		{
			_LightningSpawning(ComputedRadiusSpawnLightningArea);
		}
	}
}

void AIwacLevelScriptActor::_TickIronPhase(float DeltaSeconds)
{
	//Increase time spend in Iron Phase
	TimeSpendIronPhase += DeltaSeconds;

	//Make the sun rotate
	/*
	if (SunDirectionalLightActor)
		SunDirectionalLightActor->AddActorWorldRotation(FRotator(0.0f, SunRotationSpeed * DeltaSeconds, 0.0f));
	*/
}

void AIwacLevelScriptActor::_KnifeSpawning(float ComputedRadiusSpawnKnifeArea)
{
	//UE_LOG(LogGPCode, Log, TEXT("Spawn Knife"));
	
	NbSpawnedKnife++;
	bHasKnifeSpawned = true;

	//Set remaining time with DelayBetweenKnifeSpawn for next knife spawn
	_RemainingTime = DelayBetweenKnifeSpawn;

	//Spawn knife
	AKnifeCharacter* SpawnedKnifeCharacter = GetWorld()->SpawnActor<AKnifeCharacter>(_KnifeClass);
	FVector SpawningKnifePosition = _PlayerCharacter->GetActorLocation();
	SpawnedKnifeCharacter->SetActorLocation(SpawningKnifePosition);
	SpawnedKnifeCharacter->SpawnDefaultController();

	//Rotate to a random angle
	SpawnedKnifeCharacter->SetActorRelativeRotation(FRotator(0.0f, FMath::FRand() * 360.0f, 0.0f));

	//Move to border of the area
	SpawnedKnifeCharacter->SetActorLocation(SpawnedKnifeCharacter->GetActorLocation() + SpawnedKnifeCharacter->GetActorForwardVector() * ComputedRadiusSpawnKnifeArea);

	//Look at player character
	SpawnedKnifeCharacter->AddActorLocalRotation(FRotator(0.0f, 180.0f, 0.0f));

	SpawnedKnifeCharacter->InitOriginalPosition();
}

void AIwacLevelScriptActor::_LightningSpawning(float ComputedRadiusSpawnLightningArea)
{
	//UE_LOG(LogGPCode, Log, TEXT("Spawn Lightning"));
	CurrentNbLightning++;

	//Set remaining time with MinCooldownLightning and MaxCooldownLightning for the next lightning spawn
	_RemainingTime = FMath::FRandRange(MinCooldownLightning, MaxCooldownLightning);

	//Spawn lightning
	ALightningActor* SpawnedLightningActor = GetWorld()->SpawnActor<ALightningActor>(_LightningClass);
	SpawnedLightningActor->SetLifeSpan(DelayDamageLightning);
	float ComputedRadiusDamageLightningArea = _PlayerCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * RadiusDamageLightningArea;
	SpawnedLightningActor->SetActorLocation(_PlayerCharacter->GetActorLocation());
	SpawnedLightningActor->SetDecalsScale(ComputedRadiusDamageLightningArea, ComputedRadiusDamageLightningArea);

	//Check if critical lightning
	if (FMath::FRand() * 100.0f > PercentCriticalLightning)
	{
		//Rotate to a random angle
		SpawnedLightningActor->SetActorRelativeRotation(FRotator(0.0f, FMath::FRand() * 360.0f, 0.0f));

		//Move in spawn area
		SpawnedLightningActor->SetActorLocation(SpawnedLightningActor->GetActorLocation() + SpawnedLightningActor->GetActorForwardVector() * (ComputedRadiusSpawnLightningArea * FMath::FRand()));

		//Rotate to look at ground
		SpawnedLightningActor->SetActorRotation(FRotator(-90.0f, 0.0f, 0.0f));
	}
	else
	{
		//UE_LOG(LogGPCode, Log, TEXT("Critical!"));
	}
}
