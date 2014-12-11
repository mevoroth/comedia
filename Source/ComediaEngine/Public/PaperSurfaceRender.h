#pragma once
//#include "PaperSurfacePrivatePCH.h"
#include "PrimitiveSceneProxy.h"
#include "PaperSurfaceComponent.h"

struct FPaperSurfaceDynamicData
{
};

class FPaperSurfaceSceneProxy : public FPrimitiveSceneProxy
{
public:
	FPaperSurfaceSceneProxy() : FPrimitiveSceneProxy(0) {}
	FPaperSurfaceSceneProxy(UPaperSurfaceComponent* Component);

	void ExecComputeShader();

	virtual void DrawDynamicElements(FPrimitiveDrawInterface* PDI, const FSceneView* View) override;

	virtual uint32_t GetMemoryFootprint() const;

	FUnorderedAccessViewRHIRef PaperSurfaceUAV;

	FTexture2DRHIRef TextureRes;
	UTexture2D* Tex;
	volatile void* resource;
};
