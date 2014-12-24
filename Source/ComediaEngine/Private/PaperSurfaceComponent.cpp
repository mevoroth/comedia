#include "ComediaEngine.h"
#include "Shaders/PaperCS.h"
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
	//FTexture2DRHIRef
	//DXGI_FORMAT_BC1_UNORM
	TextureRes = RHICreateTexture2D(512, 512, PF_A32B32G32R32F, 1, 1, TexCreate_ShaderResource | TexCreate_UAV, ResCreateInfo);
	PaperData.OutTex = RHICreateUnorderedAccessView(TextureRes);

	//Tex0Arr.SetAllowCPUAccess(true);
	//Tex0Arr.Init(FVector4(1.f, 1.f, 1.f, 1.f), 512 * 512);
	//ResCreateInfo.BulkData = 0;
	//ResCreateInfo.ResourceArray = &Tex0Arr;
	//Tex0Res = RHICreateTexture2D(512, 512, PF_A32B32G32R32F, 1, 1, TexCreate_ShaderResource | TexCreate_UAV, ResCreateInfo);
	//PaperData.InTex0 = RHICreateUnorderedAccessView(Tex0Res);

	// Generate Noise
	//TResourceArray<float> RandomNoise(true);
	/**
	[] {
	rotate,	// Rotate
	sX, sY	// Scale
	dX, dY,	// Translate
	}
	*/
	//FRandomStream random;
	//random.GenerateNewSeed();
	//for (int i = 0; i < 16; ++i)
	//{
	//	RandomNoise.Add(random.GetFraction() * 6.28318530718f);
	//	RandomNoise.Add(random.GetFraction() * 2.f);
	//	RandomNoise.Add(random.GetFraction() * 2.f);
	//	RandomNoise.Add(random.GetFraction() * 0.5f);
	//	RandomNoise.Add(random.GetFraction() * 0.5f);
	//}
	//FRHIResourceCreateInfo ResNoise;
	//ResNoise.ResourceArray = &RandomNoise;
	//FStructuredBufferRHIRef NoiseBuff = RHICreateStructuredBuffer(5 * 4, 5 * 16 * 4, BUF_UnorderedAccess | BUF_ShaderResource, ResNoise);
	//PaperData.InNoise = RHICreateUnorderedAccessView(NoiseBuff, true, false);
	PaperData.InNoise.Initialize(sizeof(float), 5 * 16, PF_R32_FLOAT);
	ENQUEUE_UNIQUE_RENDER_COMMAND_ONEPARAMETER(
		PaperSurfacePopulateRandom,
		FVertexBufferRHIParamRef, Buffer, PaperData.InNoise.Buffer,
		{
			FRandomStream random;
			random.GenerateNewSeed();
			float* res = (float*)RHILockVertexBuffer(Buffer, 0, Buffer->GetSize(), RLM_WriteOnly);
			for (int i = 0; i < 16; ++i)
			{
				res[i * 5 + 0] = random.GetFraction() * 6.28318530718f;
				res[i * 5 + 1] = random.GetFraction() * 2.f;
				res[i * 5 + 2] = random.GetFraction() * 2.f;
				res[i * 5 + 3] = random.GetFraction() - 0.5f;
				res[i * 5 + 4] = random.GetFraction() - 0.5f;
			}
			RHIUnlockVertexBuffer(Buffer);
		});
	//RHILockVertexBuffer(PaperData.InNoise.Buffer)
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
		PaperCSData&, PaperData, PaperData,
		{
			FRHICommandListImmediate& RHICmdList = GRHICommandList.GetImmediateCommandList();

			TShaderMapRef<FPaperCS> PaperCS(GetGlobalShaderMap(PaperComponent->GetScene()->GetFeatureLevel()));
			RHICmdList.SetComputeShader(PaperCS->GetComputeShader());

			PaperCS->SetParameters(
				RHICmdList,
				PaperData.InTex0Tex,
				PaperData.InTex1Tex,
				PaperData.InTex2Tex,
				PaperData.InTex3Tex,
				PaperData.InRippedNoiseTex
			);

			PaperCS->SetParameters(
				RHICmdList,
				PaperData.InTex0Sampler, ((FTexture2DResource*)PaperData.InTex0Tex)->GetTexture2DRHI(),
				PaperData.InTex1Sampler, ((FTexture2DResource*)PaperData.InTex1Tex)->GetTexture2DRHI(),
				PaperData.InTex2Sampler, ((FTexture2DResource*)PaperData.InTex2Tex)->GetTexture2DRHI(),
				PaperData.InTex3Sampler, ((FTexture2DResource*)PaperData.InTex3Tex)->GetTexture2DRHI(),
				PaperData.InRippedNoiseSampler, ((FTexture2DResource*)PaperData.InRippedNoiseTex)->GetTexture2DRHI()
			);

			PaperCS->SetParameters(
				RHICmdList,
				PaperData.InNoise.SRV
			);

			//PaperCS->SetParameters(
			//	RHICmdList,
			//	PaperData.InTex0,
			//	PaperData.InTex1,
			//	PaperData.InTex2,
			//	PaperData.InTex3,
			//	PaperData.InRippedNoise,
			//	PaperData.InNoise
			//);
			FPaperSurfaceParameters params;
			params.Size = FVector2D(512.f, 512.f);
			params.ThresholdCutoff = 0.1f;
			PaperCS->SetUniformBuffers(RHICmdList, params);
			PaperCS->SetOutput(RHICmdList, PaperData.OutTex);
			DispatchComputeShader(RHICmdList, *PaperCS, 32, 32, 1);
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

	if (!jobdone && Tex0 && Tex1 && Tex2 && Tex3 && RippedPaperTexture)
	{

		PaperData.InTex0Tex = Tex0->CreateResource();
		PaperData.InTex1Tex = Tex1->CreateResource();
		PaperData.InTex2Tex = Tex2->CreateResource();
		PaperData.InTex3Tex = Tex3->CreateResource();
		PaperData.InRippedNoiseTex = RippedPaperTexture->CreateResource();

		ENQUEUE_UNIQUE_RENDER_COMMAND_ONEPARAMETER(
			PaperSurfaceInitCSData,
			PaperCSData&, PaperData, PaperData,
			{
				((FTexture2DResource*)PaperData.InTex0Tex)->InitRHI();
				((FTexture2DResource*)PaperData.InTex1Tex)->InitRHI();
				((FTexture2DResource*)PaperData.InTex2Tex)->InitRHI();
				((FTexture2DResource*)PaperData.InTex3Tex)->InitRHI();
				((FTexture2DResource*)PaperData.InRippedNoiseTex)->InitRHI();
			});

		FSamplerStateInitializerRHI SamplerInit(SF_AnisotropicLinear);
		PaperData.InTex0Sampler = RHICreateSamplerState(SamplerInit);
		PaperData.InTex1Sampler = RHICreateSamplerState(SamplerInit);
		PaperData.InTex2Sampler = RHICreateSamplerState(SamplerInit);
		PaperData.InTex3Sampler = RHICreateSamplerState(SamplerInit);
		PaperData.InRippedNoiseSampler = RHICreateSamplerState(SamplerInit);
		MarkRenderDynamicDataDirty();
	}
	else
	{
		UE_LOG(LogEngineCode, Warning, TEXT("test"));
	}
}

void UPaperSurfaceComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	if (!(Tex0 && Tex1 && Tex2 && Tex3 && RippedPaperTexture))
	{
		return;
	}
	
	//FTexture2DRHIRef Texres0;// , *Texres1, *Texres2, *Texres3, *Rippedres;
	//Tex0->InitRHI();
	//FTexture2DResource* tmp = (FTexture2DResource*)Tex0->CreateResource();

	//uint32 stride;
	//RHILockTexture2D(tmp->GetTexture2DRHI(), 0, RLM_ReadOnly, stride, false);
	
	//RHIUnlockTexture2D(tmp->GetTexture2DRHI(), 0, false);
	//Tex0->GetData(0, )

	//UTexture2D* tmp = Tex0->CreateTransient(Tex0->GetSizeX(), Tex0->GetSizeY(), PF_A32B32G32R32F);
	//FTexture2DResource* res = (FTexture2DResource*)Tex0->CreateResource();
	//FTexture2DResource* res2 = (FTexture2DResource*)Tex0->CreateResource();

	//ENQUEUE_UNIQUE_RENDER_COMMAND_ONEPARAMETER(
	//	PaperSurfaceInitTexture2,
	//	FTexture2DResource*, res, res2,
	//	{
	//		res->InitRHI();
	//	});
	//FlushRenderingCommands();
	//RHICreateUnorderedAccessView(res2->GetTexture2DRHI());
	//RHICreateUnorderedAccessView(res->GetTexture2DRHI());

	//ENQUEUE_UNIQUE_RENDER_COMMAND_TWOPARAMETER(
	//	PaperSurfaceSetTexture0,
	//	//uint8*, ArrayData, (uint8*)tmp,
	//	//TResourceArray<FVector4>&, Arr, Tex0Arr,
	//	UTexture2D*, Tex, Tex0,
	//	FTexture2DRHIRef, Texres, Tex0Res,
	//	{
	//		//uint32 Stride;
	//		//uint8* TextureData = (uint8*)RHILockTexture2D(Texres, 0, RLM_WriteOnly, Stride, false);
	//		//Tex->GetMipData(0, (void**)&TextureData);
	//		////FMemory::Memcpy(TextureData, ArrayData, GPixelFormats[PF_A32B32G32R32F].BlockBytes * 512 * 512);
	//		//RHIUnlockTexture2D(Texres, 0, false);
	//		//Tex->InitRHI();
	//		//RHICreateUnorderedAccessView(Tex->GetTexture2DRHI());
	//	});
	//FlushRenderingCommands();
	//FRHIResourceCreateInfo info;
	//info.BulkData = Tex0->ResourceMem;
	//Texres0 = RHICreateTexture2D(Tex0->GetSizeX(), Tex0->GetSizeY(), Tex0->GetPixelFormat(), 1, 1, TexCreate_ShaderResource | TexCreate_UAV, info);
	//PaperData.InTex0 = RHICreateUnorderedAccessView(Texres0);
	//ENQUEUE_UNIQUE_RENDER_COMMAND_TWOPARAMETER(
	//	PaperSurfaceSetTexture1,
	//	UTexture2D*, Tex, Tex1,
	//	FUnorderedAccessViewRHIRef&, UAV, PaperData.InTex1,
	//	{
	//		FTexture2DResource* tmp = (FTexture2DResource*)Tex->CreateResource();
	//		tmp->InitRHI();
	//		UAV = RHICreateUnorderedAccessView(tmp->GetTexture2DRHI());
	//	});
}
