// Fill out your copyright notice in the Description page of Project Settings.

#include "Comedia.h"
#include "PuzzleZoneActor.h"
#include "LiyaCharacter.h"


APuzzleZoneActor::APuzzleZoneActor(const FObjectInitializer& FOI)
	: Super(FOI)
{
	TriggerZone = FOI.CreateDefaultSubobject<UBoxComponent>(this, TEXT("Trigger Zone"));
	RootComponent = TriggerZone;

	TriggerZone->OnComponentBeginOverlap.__Internal_AddDynamic(this, &APuzzleZoneActor::OnBeginOverlap, TEXT("APuzzleZoneActor::OnBeginOverlap"));
	TriggerZone->OnComponentEndOverlap.__Internal_AddDynamic(this, &APuzzleZoneActor::OnEndOverlap, TEXT("APuzzleZoneActor::OnEndOverlap"));
}

void APuzzleZoneActor::OnBeginOverlap(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const struct FHitResult& SweepResult)
{
	ALiyaCharacter* Character = (ALiyaCharacter*)GetWorld()->GetFirstPlayerController()->GetCharacter();

	//Get puzzle zone overlapping player
	TArray<AActor*> OverlappingActors;
	Character->GetOverlappingActors(OverlappingActors, TSubclassOf<APuzzleZoneActor>());

	if (OverlappingActors.Num() <= 1 || !bCantOverride)
	{
		if (Character == OtherActor)
		{
			//Player enter in puzzle zone
			Character->GrabbingPlayerLocation = Character->GetActorLocation();
			Character->ElapsedTravellingScriptedCamera = 0.0f;
			Character->RatioCameraFollow = RatioCameraFollow;

			//Compute position relative to liya corresponding to camera position relative to puzzle zone
			FTransform OverrideScriptedCameraPosition = CameraPosition->FindComponentByClass<USceneComponent>()->ComponentToWorld;
			FVector VectPuzzleZoneToPlayer = Character->GetActorLocation() - GetActorLocation();
			FVector OverrideLocation = OverrideScriptedCameraPosition.GetLocation();
			OverrideLocation.Set(OverrideLocation.X + VectPuzzleZoneToPlayer.X, OverrideLocation.Y + VectPuzzleZoneToPlayer.Y, OverrideLocation.Z);
			OverrideScriptedCameraPosition.SetLocation(OverrideLocation);
			Character->OverrideScriptedCameraPosition = OverrideScriptedCameraPosition;
			Character->StartTravellingPosition = Character->Camera->ComponentToWorld;
			Character->LengthTravellingScriptedCamera = LengthTravellingScriptedCamera;
		}
	}
}

void APuzzleZoneActor::OnEndOverlap(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ALiyaCharacter* Character = (ALiyaCharacter*)GetWorld()->GetFirstPlayerController()->GetCharacter();

	if (Character == OtherActor)
	{
		//Get puzzle zone overlapping player
		UpdateOverlaps(false);
		TArray<AActor*> OverlappingActors;
		Character->GetOverlappingActors(OverlappingActors, TSubclassOf<APuzzleZoneActor>());

		//If player was in only one puzzle zone
		if (OverlappingActors.Num() == 0)
		{
			//Player exit puzzle zone
			_CancelCameraOverride(Character);
		}
		else
		{
			//Set overiding cam of other overlapping puzzle zone
			bool bOtherPuzzleZoneSet = false;
			for (int i = 0; i < OverlappingActors.Num() && !bOtherPuzzleZoneSet; i++)
			{
				if (OverlappingActors[i] != this && !Cast<APuzzleZoneActor>(OverlappingActors[i])->bCantOverride)
				{
					Cast<APuzzleZoneActor>(OverlappingActors[i])->OnBeginOverlap(Character, nullptr, 0, false, FHitResult());
					bOtherPuzzleZoneSet = true;
				}
			}

			if (!bOtherPuzzleZoneSet)
			{
				_CancelCameraOverride(Character);
			}
		}
	}
}

void APuzzleZoneActor::_CancelCameraOverride(ALiyaCharacter* Character)
{
	Character->StartTravellingPosition = Character->Camera->GetRelativeTransform();
	Character->LengthTravellingBackScriptedCamera = Character->ElapsedTravellingScriptedCamera;
	Character->OverrideScriptedCameraPosition = FTransform();
	Character->LengthTravellingScriptedCamera = LengthTravellingScriptedCamera;
}
