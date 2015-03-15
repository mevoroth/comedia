// Fill out your copyright notice in the Description page of Project Settings.

#include "Comedia.h"
#include "IwacLevelScriptActor.h"
#include "LightningActor.h"


ALightningActor::ALightningActor(const FObjectInitializer& ObjectInitializer) 
	: Super(ObjectInitializer)
{
	//Init full decal component and set it as root component
	LightningFullDecal = ObjectInitializer.CreateDefaultSubobject<UDecalComponent>(this, TEXT("FullDecaleComponent"));
	LightningFullDecal->RelativeScale3D = FVector(512.0f, 10.0f, 10.0f);
	LightningFullDecal->SetWorldRotation(FRotator(-90, 0, 0));
	LightningFullDecal->SortOrder = 0.0f;
	RootComponent = LightningFullDecal;
	LightningFullDecal->bHiddenInGame = true;

	//Init second decal compo
	LightningTimerDecal = ObjectInitializer.CreateDefaultSubobject<UDecalComponent>(this, TEXT("DecaleComponent"));
	LightningFullDecal->SetWorldRotation(FRotator(-90, 0, 0));
	LightningTimerDecal->SortOrder = 1.0f;
	LightningTimerDecal->AttachTo(RootComponent);

	bCanBeDamaged = false;
	PrimaryActorTick.bCanEverTick = true;
}

void ALightningActor::SetDecalsScale(float Width, float Height)
{
	_ComputedRadiusDamageLightningArea = (Width + Height) / 2.0f;
	LightningFullDecal->SetWorldScale3D(FVector(LightningFullDecal->GetComponentScale().X, Width, Height));
	float RatioRemainingTime = _GetRatioRemainingTime();
	LightningTimerDecal->SetWorldScale3D(FVector(LightningTimerDecal->GetComponentScale().X, Width * RatioRemainingTime, Height * RatioRemainingTime));
}

void ALightningActor::InitImpactTarget()
{
	FHitResult Hit(ForceInit);
	FCollisionQueryParams Trace(TEXT("LightningTrace"), false, GetOwner());
	AIwacLevelScriptActor* IwacLevelScript = Cast<AIwacLevelScriptActor>(GetWorld()->GetLevelScriptActor());
	if (IwacLevelScript)
	{
		Trace.AddIgnoredActor(IwacLevelScript->TreeActor);
	}
	GetWorld()->LineTraceSingle(Hit, GetActorLocation() + FVector::UpVector * 1000.0f, GetActorLocation() + FVector::UpVector * -2000.0f, ECC_Visibility, Trace);
	_ImpactPosition = Hit.ImpactPoint;

	//Spawn emitter
	UGameplayStatics::SpawnEmitterAtLocation(this, PreThunderParticleSystem, _ImpactPosition);
}

void ALightningActor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	//Rescale timer decal
	float RatioRemainingTime = _GetRatioRemainingTime();
	LightningTimerDecal->SetWorldScale3D(FVector(LightningFullDecal->GetComponentScale().X, LightningFullDecal->GetComponentScale().Y * RatioRemainingTime, LightningFullDecal->GetComponentScale().Z * RatioRemainingTime));
}

void ALightningActor::LifeSpanExpired()
{
	Super::LifeSpanExpired();

	AIwacLevelScriptActor* IwacLevelScript = Cast<AIwacLevelScriptActor>(GetWorld()->GetLevelScriptActor());
	if (IwacLevelScript)
	{
		//UE_LOG(LogGPCode, Log, TEXT("Lightning explode"));
		IwacLevelScript->CurrentNbLightning--;

		//Check if player touch by lightning
		ACharacter* PlayerCharacter = GetWorld()->GetFirstPlayerController()->GetCharacter();
		FVector PlayerPosition = PlayerCharacter->GetActorLocation();
		FVector LightningPosition = GetActorLocation();
		PlayerPosition.Z = LightningPosition.Z = 0.0f;
		float DistanceLightningPlayer = FVector::Dist(PlayerPosition, LightningPosition);
		if (DistanceLightningPlayer <= _ComputedRadiusDamageLightningArea)
		{
			UE_LOG(LogGPCode, Log, TEXT("Player touch by lightning"));
			//Spawn particle emitter
			UParticleSystemComponent* ParticleSystem = UGameplayStatics::SpawnEmitterAtLocation(this, LightningParticleSystem, GetActorLocation() + FVector::UpVector * 1000.0f);
			ParticleSystem->SetBeamTargetPoint(0, PlayerCharacter->GetActorLocation() - FVector::UpVector * PlayerCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight(), 0);
			ParticleSystem->TranslucencySortPriority = 3;

			//Call PlayerTouchByLightning event in LevelBlueprint
			AIwacLevelScriptActor* IwacLevelScript = Cast<AIwacLevelScriptActor>(GetWorld()->GetLevelScriptActor());
			if (IwacLevelScript)
			{
				IwacLevelScript->PlayerTouchByLightning();
			}
		}
		else
		{
			//Spawn particle emitter
			UParticleSystemComponent* ParticleSystem = UGameplayStatics::SpawnEmitterAtLocation(this, LightningParticleSystem, GetActorLocation() + FVector::UpVector * 1000.0f);
			ParticleSystem->SetBeamTargetPoint(0, _ImpactPosition, 0);
			ParticleSystem->TranslucencySortPriority = 3;
		}
	}
}

float ALightningActor::_GetRatioRemainingTime()
{
	return (1 - GetLifeSpan() / InitialLifeSpan);
}