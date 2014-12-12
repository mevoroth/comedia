// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "PrimitiveSceneProxy.h"

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

private:

	FUnorderedAccessViewRHIRef PaperSurfaceUAV;
	TResourceArray<FVector4> ComputedColors;
	FTexture2DRHIRef TextureRes;
};
