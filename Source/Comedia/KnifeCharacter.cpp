// Fill out your copyright notice in the Description page of Project Settings.

#include "Comedia.h"
#include "KnifeCharacter.h"
#include "LiyaCharacter.h"
#include "IwacLevelScriptActor.h"


AKnifeCharacter::AKnifeCharacter(const class FObjectInitializer& PCIP)
	: Super(PCIP)
{
	//Get blueprint class for trail instantiation
	static ConstructorHelpers::FClassFinder<AActor> TrailClassFinder(TEXT("/Game/Blueprints/Bp_Fente_knfie_fx"));
	_TrailClass = TrailClassFinder.Class;

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
	
	//Check if player not behind the knife
	if (AngleForwardToPlayer < 90.0f)
	{
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
	}
	

	//Set character move
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);
	GetCharacterMovement()->MaxFlySpeed = KnifeSpeed;
	GetCharacterMovement()->RequestDirectMove(GetActorForwardVector(), true);

	//Check knife destroyed
	AIwacLevelScriptActor* IwacLevelScript = Cast<AIwacLevelScriptActor>(GetWorld()->GetLevelScriptActor());
	if (IwacLevelScript)
	{
		//Check if distance player/knife more than ComputedRadiusSpawnKnifeArea and if player is behind knife
		if ((GetWorld()->GetFirstPlayerController()->GetCharacter()->GetActorLocation() - GetActorLocation()).Size() > IwacLevelScript->ComputedRadiusSpawnKnifeArea && AngleForwardToPlayer > 90.0f)
		{
			_DestroyKnife();
		}
	}

	//Check if need to spawn a new decal
	if ((GetActorLocation() - _LastDecalPosition).Size() >= _TrailLength)
	{
		_SpawnTrail();
	}

	//Update position to be on ground with trace
	FHitResult Hit(ForceInit);
	FCollisionQueryParams Trace(TEXT("KnifeTrace"), false, GetOwner());
	Trace.AddIgnoredActor(IwacLevelScript->TreeActor);
	GetWorld()->LineTraceSingle(Hit, GetActorLocation(), GetActorLocation() + FVector::UpVector * -1000.0f, ECC_Visibility, Trace);
	FVector CurrentKnifePosition = GetActorLocation();
	CurrentKnifePosition.Z = Hit.Location.Z + GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	SetActorLocation(CurrentKnifePosition);
}

void AKnifeCharacter::ReceiveHit(class UPrimitiveComponent* MyComp, AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::ReceiveHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);

	//Check if receive hit from player
	if (Cast<ALiyaCharacter>(Other))
	{
		UE_LOG(LogGPCode, Log, TEXT("Knife collide with player"));

		//Call PlayerTouchByKnife event in LevelBlueprint
		AIwacLevelScriptActor* IwacLevelScript = Cast<AIwacLevelScriptActor>(GetWorld()->GetLevelScriptActor());
		if (IwacLevelScript)
		{
			IwacLevelScript->PlayerTouchByKnife();
		}

		_DestroyKnife();
	}
}

void AKnifeCharacter::InitOriginalPosition()
{
	_LastDecalPosition = GetActorLocation();
}

void AKnifeCharacter::_DestroyKnife()
{
	AIwacLevelScriptActor* IwacLevelScript = Cast<AIwacLevelScriptActor>(GetWorld()->GetLevelScriptActor());
	if (IwacLevelScript)
	{
		//UE_LOG(LogGPCode, Log, TEXT("Destroy Knife"));
		IwacLevelScript->bHasKnifeSpawned = false;
		//GetWorld()->DestroyActor(this);
		OnKnifeDestroy();
	}
}

void AKnifeCharacter::_SpawnTrail()
{
	//Get ground height
	FHitResult Hit(ForceInit);
	FCollisionQueryParams Trace(TEXT("KnifeTrace"), false, GetOwner());
	AIwacLevelScriptActor* IwacLevelScript = Cast<AIwacLevelScriptActor>(GetWorld()->GetLevelScriptActor());
	if (IwacLevelScript)
	{
		Trace.AddIgnoredActor(IwacLevelScript->TreeActor);
	}
	GetWorld()->LineTraceSingle(Hit, GetActorLocation() + FVector::UpVector * 1000.0f, GetActorLocation() + FVector::UpVector * -2000.0f, ECC_Visibility, Trace);

	//Spawn, set position and set scale
	FActorSpawnParameters SpawnParams;
	SpawnParams.bNoCollisionFail = true;
	AActor* NewTrail = GetWorld()->SpawnActor<AActor>(_TrailClass, SpawnParams);

	//Get BoxComponent to compute TrailLength
	if (_TrailLength == 0)
	{
		UBoxComponent* BoundingBoxComponent = NewTrail->FindComponentByClass<UBoxComponent>();
		if (BoundingBoxComponent)
		{
			_TrailLength = BoundingBoxComponent->GetScaledBoxExtent().Y * 2.0f * RatioTrailLength;
		}
		else
		{
			_TrailLength = NewTrail->GetComponentsBoundingBox(true).GetSize().Y;
		}
	}

	FVector NewTrailPosition = GetActorLocation();
	NewTrailPosition.Z = Hit.ImpactPoint.Z + VerticalTrailOffset;
	NewTrail->SetActorLocation(NewTrailPosition);

	//Compute rotation
	FVector NewTrailDirection = GetActorLocation() - _LastDecalPosition;
	NewTrailDirection.Normalize();
	float AngleNewTrail = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(NewTrailDirection, FVector::ForwardVector)));
	float SignAngleNewTrail = FMath::Sign<float>(FVector::CrossProduct(FVector::ForwardVector, NewTrailDirection).Z);
	FRotator RotationNewTrail = FRotator(0.0f, -90.0f + (AngleNewTrail * SignAngleNewTrail), 0.0f);
	NewTrail->SetActorRelativeRotation(RotationNewTrail);

	_LastDecalPosition = GetActorLocation();
}
