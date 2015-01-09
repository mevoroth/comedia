// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "PaperSurfaceComponent.h"

#include "GameFramework/Actor.h"
#include "PaperAssetActor.generated.h"

/**
 * 
 */
UCLASS()
class COMEDIA_API APaperAssetActor : public AActor
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Paper Surface")
	UPaperSurfaceComponent* PaperSurfaceComponent;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Paper Surface")
	UStaticMeshComponent* MeshComponent;

	virtual void Tick(float DeltaSeconds) override;
private:
	UTexture2D* MainTex;
	bool Updated;
};
