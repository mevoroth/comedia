#include "ComediaEngine.h"
#include "PaperSurfaceRender.h"
#include "PaperCS.h"
#include "PaperSurfaceComponent.h"

UPaperSurfaceComponent::UPaperSurfaceComponent(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	PrimaryComponentTick.bCanEverTick = true;
	jobdone = false;
	FRHIResourceCreateInfo ResCreateInfo;
	ComputedColors.SetAllowCPUAccess(true);
	ComputedColors.Init(FVector4(1.f, 0.f, 0.f, 1.f), 512 * 512);
	ResCreateInfo.ResourceArray = &ComputedColors;
	TextureRes = RHICreateTexture2D(512, 512, PF_A32B32G32R32F, 1, 1, TexCreate_ShaderResource | TexCreate_UAV, ResCreateInfo);
	PaperSurfaceUAV = RHICreateUnorderedAccessView(TextureRes);
}
//
//FPrimitiveSceneProxy* UPaperSurfaceComponent::CreateSceneProxy()
//{
//	return new FPaperSurfaceSceneProxy(this);
//	//return Data ? new FPaperSurfaceSceneProxy(this) : 0;
//	//return 0;
//}

void UPaperSurfaceComponent::SendRenderDynamicData_Concurrent()
{
	Super::SendRenderDynamicData_Concurrent();
	ENQUEUE_UNIQUE_RENDER_COMMAND_TWOPARAMETER(
		FSendPaperSurfaceDynamicData,
		UActorComponent*, PaperComponent, this,
		FUnorderedAccessViewRHIRef, PaperSurfaceUAV, PaperSurfaceUAV,
		{
			FRHICommandListImmediate& RHICmdList = GRHICommandList.GetImmediateCommandList();

			TShaderMapRef<FPaperCS> PaperCS(GetGlobalShaderMap(PaperComponent->GetScene()->GetFeatureLevel()));
			RHICmdList.SetComputeShader(PaperCS->GetComputeShader());

			PaperCS->SetOutput(RHICmdList, PaperSurfaceUAV);
			DispatchComputeShader(RHICmdList, *PaperCS, 8, 1, 1);
			PaperCS->UnbindBuffers(RHICmdList);
			//PaperSurfaceSceneProxy->SetDynamicData_RenderThread(DynamicData);
			//PaperSurfaceSceneProxy->ExecComputeShader();
		});
	FlushRenderingCommands();
	ENQUEUE_UNIQUE_RENDER_COMMAND_THREEPARAMETER(
		FGetPaperSurfaceData,
		TResourceArray<FVector4>&, ComputedColors, ComputedColors,
		FTexture2DRHIRef, TextureRes, TextureRes,
		bool&, done, jobdone,
		{
			uint32 Stride;
			uint8* TextureData = (uint8*)RHILockTexture2D(TextureRes, 0, RLM_ReadOnly, Stride, false);
			uint8* ArrayData = (uint8*)ComputedColors.GetData();
			FMemory::Memcpy(ArrayData, TextureData, GPixelFormats[PF_A32B32G32R32F].BlockBytes * 512 * 512);
			RHIUnlockTexture2D(TextureRes, 0, false);

			done = true;
		});
	FlushRenderingCommands();
}

void UPaperSurfaceComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!jobdone)
	{
		MarkRenderDynamicDataDirty();
	}
	else
	{
		UE_LOG(LogEngineCode, Warning, TEXT("test"));
	}
}
