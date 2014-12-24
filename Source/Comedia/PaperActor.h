// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "PaperSurfaceComponent.h"
#include "GameFramework/Actor.h"
#include "PaperActor.generated.h"

/**
 * 
 */
UCLASS()
class COMEDIA_API APaperActor : public AActor
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Paper Surface")
	TSubobjectPtr<UPaperSurfaceComponent> PaperSurface;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Paper Surface")
	TSubobjectPtr<UBillboardComponent> BillboardComponent;
	//UPaperSurfaceComponent* PaperSurface;

	virtual void Tick(float DeltaSeconds) override;
};
