// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "PrimitiveSceneProxy.h"
#include "Shaders/PaperCSData.h"

#include "Components/MeshComponent.h"
#include "PaperSurfaceComponent.generated.h"

struct FPaperSurfaceDynamicData;

/**
*
*/
UCLASS(ClassGroup = (Rendering, Common), HideCategories = (Object, LOD, Physics, Activation, "Components|Activation"), EditInLineNew, Meta = (BlueprintSpawnableComponent), MinimalAPI)
class UPaperSurfaceComponent : public UMeshComponent
{
	GENERATED_UCLASS_BODY()

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
	//virtual FPrimitiveSceneProxy* CreateSceneProxy();
	virtual void SendRenderDynamicData_Concurrent() override;
	FPaperSurfaceDynamicData* CreateDynamicData();

	bool jobdone;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Comedia Paper Settings", FriendlyName="Texture 0")
	UTexture2D* Tex0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Comedia Paper Settings", FriendlyName = "Texture 1")
	UTexture2D* Tex1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Comedia Paper Settings", FriendlyName = "Texture 2")
	UTexture2D* Tex2;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Comedia Paper Settings", FriendlyName = "Texture 3")
	UTexture2D* Tex3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Comedia Paper Settings", FriendlyName = "Ripped Paper Texture")
	UTexture2D* RippedPaperTexture;

	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

	TResourceArray<FVector4> ComputedColors;
private:
	/** Output data */
	FTexture2DRHIRef TextureRes;

	TResourceArray<float> RandomNoise;

	/** Shaders params */
	PaperCSData PaperData;
};
