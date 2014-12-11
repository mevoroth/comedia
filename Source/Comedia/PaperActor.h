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

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Paper Surface")
	UPaperSurfaceComponent* PaperSurface;
	
};
