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

	//Get blueprint class for Iron instantiation
	static ConstructorHelpers::FClassFinder<AIronActor> IronActorClassFinder(TEXT("/Game/Blueprints/BP_Iron"));
	_IronClass = IronActorClassFinder.Class;

	//Enable tick function
	PrimaryActorTick.bCanEverTick = true;

	_PreviousTorturePhase = ETorturePhase::TP_EmptyPhase;

	bPlayerHasFailed = false;
	bHasKnifeSpawned = false;
	CurrentNbLightning = 0;
}

void AIwacLevelScriptActor::BeginPlay()
{
	Super::BeginPlay();

	//Check if TreeActor set in Level blueprint
	check(TreeActor);

	//Check if Matinees set in Level Blueprint
	check(MatineeKnifeToLightning);
	check(MatineeLightningToIron);
	check(MatineeIntro);
	check(MatineeOutro);
	check(MatineePlayerFailed);

	//Get player character
	_PlayerCharacter = GetWorld()->GetFirstPlayerController()->GetCharacter();

	//Init remaining time with DelayFirstKnifeSpawn
	_RemainingTime = DelayFirstSpawn;

	MatineeIntro->Play();
}

void AIwacLevelScriptActor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	//Check if phase changed
	if (TorturePhase != _PreviousTorturePhase)
	{
		bPlayerHasFailed = false;

		//Destroy _SpawnedIronActor if existing and phase switching
		if (_SpawnedIronActor)
		{
			GetWorld()->DestroyActor(_SpawnedIronActor);
			_SpawnedIronActor = nullptr;
		}

		const UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("ETorturePhase"), true);
		if (EnumPtr)
		{
			UE_LOG(LogGPCode, Log, TEXT("Change to phase: %s"), *EnumPtr->GetEnumName(TorturePhase));

			//Reset new phase
			switch (TorturePhase)
			{
				case ETorturePhase::TP_KnifePhase:
					NbSpawnedKnife = 0;
					CurrentNbHitKnife = 0;
					break;

				case ETorturePhase::TP_LightningPhase:
					CurrentNbHitLightning = 0;
					TimeSpendLightningPhase = 0.0f;
					MatineeKnifeToLightning->Play();
					break;

				case ETorturePhase::TP_IronPhase:
					CurrentTimeHitIron = 0.0f;
					TimeSpendIronPhase = 0.0f;
					_IronSpawning();
					MatineeLightningToIron->Play();
					break;

				case ETorturePhase::TP_EmptyPhase:
				default:
					break;
			}
		}
		_PreviousTorturePhase = TorturePhase;

		//Reinit delay first spawn
		_RemainingTime = DelayFirstSpawn;
	}

	//Don't tick phases when MatineeIntro or MatineeOutro playing
	if (!MatineeIntro->bIsPlaying && !MatineeOutro->bIsPlaying && !MatineePlayerFailed->bIsPlaying)
	{
		//Tick current torture phase
		switch (TorturePhase)
		{
		case ETorturePhase::TP_KnifePhase:
			_TickKnifePhase(DeltaSeconds);
			break;

		case ETorturePhase::TP_LightningPhase:
			if (!MatineeKnifeToLightning->bIsPlaying)
			{
				_TickLightningPhase(DeltaSeconds);
			}
			break;

		case ETorturePhase::TP_IronPhase:
			if (!MatineeLightningToIron->bIsPlaying)
			{
				_TickIronPhase(DeltaSeconds);
			}
			break;

		case ETorturePhase::TP_EmptyPhase:
		default:
			break;
		}
	}
}

void AIwacLevelScriptActor::PlayerTouchByKnife_Implementation()
{
	CurrentNbHitKnife++;

	if (CurrentNbHitKnife >= NbHitKnifeBeforeDie)
	{
		PlayerFailed();
	}
}

void AIwacLevelScriptActor::PlayerTouchByLightning_Implementation()
{
	CurrentNbHitLightning++;

	if (CurrentNbHitLightning >= NbHitLightningBeforeDie)
	{
		PlayerFailed();
	}
}

void AIwacLevelScriptActor::PlayerTouchByIron_Implementation()
{
	CurrentTimeHitIron += GetWorld()->DeltaTimeSeconds;

	if (CurrentTimeHitIron >= TimeHitIronBeforeDie)
	{
		PlayerFailed();
	}
}

void AIwacLevelScriptActor::PlayerFailed_Implementation()
{
	UE_LOG(LogGPCode, Log, TEXT("Player failed!"));
	bPlayerHasFailed = true;
	_PreviousTorturePhase = ETorturePhase::TP_EmptyPhase;
	MatineePlayerFailed->Play();
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
	check(_SpawnedIronActor); //Check if IronActor has spawned

	//Check if waiting time before phase beginning elapsed
	if (_RemainingTime > 0)
	{
		_RemainingTime -= DeltaSeconds;
	}
	else
	{
		//Increase time spend in Iron Phase
		TimeSpendIronPhase += DeltaSeconds;

		//Increase rotation speed depending of time spend in Iron phase
		_SpawnedIronActor->RotationSpeed = FMath::Lerp(LightRotationSpeedMin, LightRotationSpeedMax, TimeSpendIronPhase / LengthIronPhase);

		//Raycast from Iron point light to player to check if player is behind the tree
		FHitResult Hit(ForceInit);
		FCollisionQueryParams Trace(TEXT("IronTrace"), false, GetOwner());
		FVector IronPointLightPosition = _SpawnedIronActor->FindComponentByClass<UPointLightComponent>()->GetComponentLocation();
		GetWorld()->LineTraceSingle(Hit, IronPointLightPosition, _PlayerCharacter->GetActorLocation(), ECC_Visibility, Trace);

		if (Hit.Actor == NULL || Hit.Actor != TreeActor)
		{
			//UE_LOG(LogGPCode, Log, TEXT("Actor hit by red light at %s"), *_PlayerCharacter->GetActorLocation().ToString());
			PlayerTouchByIron();
		}
	}
}

void AIwacLevelScriptActor::_KnifeSpawning(float ComputedRadiusSpawnKnifeArea)
{
	//UE_LOG(LogGPCode, Log, TEXT("Spawn Knife"));

	//Spawn knife
	FActorSpawnParameters SpawnParams;
	SpawnParams.bNoCollisionFail = true;
	AKnifeCharacter* SpawnedKnifeCharacter = GetWorld()->SpawnActor<AKnifeCharacter>(_KnifeClass, SpawnParams);
	if (SpawnedKnifeCharacter)
	{
		//Set spawned knifes variables
		NbSpawnedKnife++;
		bHasKnifeSpawned = true;

		//Set remaining time with DelayBetweenKnifeSpawn for next knife spawn
		_RemainingTime = DelayBetweenKnifeSpawn;

		UE_LOG(LogGPCode, Log, TEXT("Spawn position: %s"), *SpawnedKnifeCharacter->GetActorLocation().ToString());
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
	else
	{
		UE_LOG(LogGPCode, Log, TEXT("KNIFE SPAWN FAILED"));
	}
	
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

	//Init impact target position and prethunder emitter
	SpawnedLightningActor->InitImpactTarget();
}

void AIwacLevelScriptActor::_IronSpawning()
{
	if (!_SpawnedIronActor)
	{
		//Spawn Iron actor and set location on tree, to rotate around it
		_SpawnedIronActor = GetWorld()->SpawnActor<AIronActor>(_IronClass);
		_SpawnedIronActor->SetActorLocation(TreeActor->GetActorLocation());

		//Rotate it to be at the opposite side of the player
		//Compute angle
		FVector VectTreeIron = _SpawnedIronActor->FindComponentByClass<UPointLightComponent>()->GetComponentLocation() - TreeActor->GetActorLocation();
		FVector VectTreePlayer = _PlayerCharacter->GetActorLocation() - TreeActor->GetActorLocation();
		VectTreeIron.Z = VectTreePlayer.Z = 0.0f;
		VectTreeIron.Normalize();
		VectTreePlayer.Normalize();
		float AngleIronPlayer = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(VectTreeIron, VectTreePlayer)));
		float SignAngleIronPlayer = FMath::Sign<float>(FVector::CrossProduct(VectTreePlayer, VectTreeIron).Z);
		//Rotate
		_SpawnedIronActor->AddActorWorldRotation(FRotator(0.0f, 180.0f - AngleIronPlayer * SignAngleIronPlayer, 0.0f));

		//Set initial rotation speed
		_SpawnedIronActor->RotationSpeed = 0.0f;
	}
}
