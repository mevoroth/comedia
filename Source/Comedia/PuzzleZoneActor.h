// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "PuzzleZoneActor.generated.h"

/**
 * 
 */
UCLASS()
class COMEDIA_API APuzzleZoneActor : public AActor
{
	GENERATED_BODY()
	
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "[Comedia]PuzzleZone")
	UBoxComponent* TriggerZone;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "[Comedia]PuzzleZone")
	AActor* CameraPosition;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "[Comedia]PuzzleZone")
	float RatioCameraFollow = 0.0f;

	APuzzleZoneActor(const FObjectInitializer& FOI);
	
	UFUNCTION()
	void OnBeginOverlap(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const struct FHitResult& SweepResult);
	UFUNCTION()
	void OnEndOverlap(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
