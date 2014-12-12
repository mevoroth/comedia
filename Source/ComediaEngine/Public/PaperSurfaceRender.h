#pragma once
//#include "PaperSurfacePrivatePCH.h"
#include "PrimitiveSceneProxy.h"
#include "PaperSurfaceComponent.h"

struct FPaperSurfaceDynamicData
{
	TResourceArray<FVector4> ComputedColors;
	FTexture2DRHIRef TextureRes;
};

class FPaperSurfaceSceneProxy : public FPrimitiveSceneProxy
{
public:
	FPaperSurfaceSceneProxy() : FPrimitiveSceneProxy(0) {}
	FPaperSurfaceSceneProxy(UPaperSurfaceComponent* Component);

	void ExecComputeShader();
	void DebugColors();

	virtual void DrawDynamicElements(FPrimitiveDrawInterface* PDI, const FSceneView* View) override;
	virtual FPrimitiveViewRelevance GetViewRelevance(const FSceneView* View) override;
	void SetDynamicData_RenderThread(FPaperSurfaceDynamicData* NewDynamicData);

	virtual uint32_t GetMemoryFootprint() const;

	FUnorderedAccessViewRHIRef PaperSurfaceUAV;

	TArray<FVector4> ComputedColors;
	FTexture2DRHIRef TextureRes;
	UTexture2D* Tex;

	FPaperSurfaceDynamicData* DynamicData;
};
