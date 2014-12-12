#include "ComediaEngine.h"
#include "PaperSurfaceRender.h"
#include "PaperCS.h"

//#include "Developer/ImageWrapper/Public/Interfaces/IImageWrapper.h"
//#include "Developer/ImageWrapper/Public/Interfaces/IImageWrapperModule.h"

FPaperSurfaceSceneProxy::FPaperSurfaceSceneProxy(UPaperSurfaceComponent* Component)
	: FPrimitiveSceneProxy(Component)
{
	//RHICreateTexture2D()
	FRHIResourceCreateInfo ResCreateInfo;
	TResourceArray<FVector4> buffData;
	buffData.SetAllowCPUAccess(true);
	buffData.Init(FVector4(1.f, 0.f, 0.f, 1.f), 512*512);
	ResCreateInfo.ResourceArray = &buffData;
	TextureRes = RHICreateTexture2D(512, 512, PF_A32B32G32R32F, 1, 1, TexCreate_ShaderResource | TexCreate_UAV, ResCreateInfo);
	PaperSurfaceUAV = RHICreateUnorderedAccessView(TextureRes);
	ComputedColors.Init(512 * 512);
}

uint32_t FPaperSurfaceSceneProxy::GetMemoryFootprint() const
{
	return sizeof(*this) + GetAllocatedSize();
}

void FPaperSurfaceSceneProxy::DrawDynamicElements(FPrimitiveDrawInterface* PDI, const FSceneView* View)
{
	QUICK_SCOPE_CYCLE_COUNTER(STAT_PaperSurfaceSceneProxy_DrawDynamicElements);
}

FPrimitiveViewRelevance FPaperSurfaceSceneProxy::GetViewRelevance(const FSceneView* View)
{
	FPrimitiveViewRelevance ViewRelevance;
	ViewRelevance.bDrawRelevance = IsShown(View);
	ViewRelevance.bShadowRelevance = IsShadowCast(View);
	ViewRelevance.bDynamicRelevance = true;
	return ViewRelevance;
}
void FPaperSurfaceSceneProxy::ExecComputeShader()
{
	FRHICommandListImmediate& RHICmdList = GRHICommandList.GetImmediateCommandList();

	TShaderMapRef<FPaperCS> PaperCS(GetGlobalShaderMap(GetScene()->GetFeatureLevel()));
	RHICmdList.SetComputeShader(PaperCS->GetComputeShader());

	PaperCS->SetOutput(RHICmdList, PaperSurfaceUAV);
	DispatchComputeShader(RHICmdList, *PaperCS, 8, 1, 1);
	PaperCS->UnbindBuffers(RHICmdList);
}

void FPaperSurfaceSceneProxy::SetDynamicData_RenderThread(FPaperSurfaceDynamicData* NewDynamicData)
{
	check(IsInRenderingThread());

	/* Free existing dynamic data */
	if (DynamicData)
	{
		delete DynamicData;
		DynamicData = NULL;
	}

	/* Assign new dynamic data */
	DynamicData = NewDynamicData;

	/* Execute the compute shader */
	ExecComputeShader();
}

void FPaperSurfaceSceneProxy::DebugColors()
{
	//static bool jobdone = false;
	//if (!jobdone)
	//{
	//	ENQUEUE_UNIQUE_RENDER_COMMAND_THREEPARAMETER(
	//		DoDebugColorsCommand,
	//		TArray<FVector4>&, dest, ComputedColors,
	//		FTexture2DRHIRef, source, TextureRes,
	//		bool&, done, jobdone,
	//		{
	//		uint32 stride; //ignored
	//		// Lock texture for reading
	//		uint8* TextureData = (uint8*)RHILockTexture2D(source, 0, RLM_ReadOnly, stride, false);
	//		// Reference pointer to first element for our destination ComputedColors
	//		uint8* ArrayData = (uint8 *)dest.GetData();
	//		// Copy from GPU to main memory
	//		FMemory::Memcpy(ArrayData, TextureData, GPixelFormats[PF_A32B32G32R32F].BlockBytes * 256);
	//		//Unlock texture
	//		RHIUnlockTexture2D(source, 0, false);

	//		done = true;
	//		});
	//}
}
