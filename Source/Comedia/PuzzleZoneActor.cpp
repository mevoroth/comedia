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
	Character->ElapsedTravellingScriptedCamera = 0.0f;
	Character->RatioCameraFollow = RatioCameraFollow;
	Character->OverrideScriptedCameraPosition = CameraPosition->FindComponentByClass<USceneComponent>();
}

void APuzzleZoneActor::OnEndOverlap(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ALiyaCharacter* Character = (ALiyaCharacter*)GetWorld()->GetFirstPlayerController()->GetCharacter();
	Character->StartTravellingPosition = Character->Camera->GetRelativeTransform();
	Character->LengthTravellingBackScriptedCamera = Character->ElapsedTravellingScriptedCamera;
	Character->OverrideScriptedCameraPosition = nullptr;
}
