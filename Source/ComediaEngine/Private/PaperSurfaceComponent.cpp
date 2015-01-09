#include "ComediaEngine.h"
#include "Shaders/PaperCS.h"
#include "PaperSurfaceComponent.h"

UPaperSurfaceComponent::UPaperSurfaceComponent(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
	, TextureSize(FVector2D(512, 512))
	, ThresholdCutoff(0.2f)
{
	PrimaryComponentTick.bCanEverTick = true;
	jobdone = false;
	ComputedColors.SetAllowCPUAccess(true);

	// Generate Noise
	/**
	[] {
	rotate,	// Rotate
	sX, sY	// Scale
	dX, dY,	// Translate
	}
	*/
	PaperData.InNoise.Initialize(sizeof(float), 5 * 16, PF_R32_FLOAT);

	// Init textures
	PaperData.InTex0Tex = 0;
	PaperData.InTex1Tex = 0;
	PaperData.InTex2Tex = 0;
	PaperData.InTex3Tex = 0;
	PaperData.InRippedNoiseTex = 0;

	// Creating sampling states
	FSamplerStateInitializerRHI SamplerInit(ESamplerFilter::SF_AnisotropicLinear);
	PaperData.InTex0Sampler = RHICreateSamplerState(SamplerInit);
	PaperData.InTex1Sampler = RHICreateSamplerState(SamplerInit);
	PaperData.InTex2Sampler = RHICreateSamplerState(SamplerInit);
	PaperData.InTex3Sampler = RHICreateSamplerState(SamplerInit);
	PaperData.InRippedNoiseSampler = RHICreateSamplerState(SamplerInit);

	UpdateOutputAndRandom();
}

void UPaperSurfaceComponent::SendRenderDynamicData_Concurrent()
{
	Super::SendRenderDynamicData_Concurrent();
	ENQUEUE_UNIQUE_RENDER_COMMAND_ONEPARAMETER(
		FSendPaperSurfaceDynamicData,
		UPaperSurfaceComponent*, PaperComponent, this,
		{
			FRHICommandListImmediate& RHICmdList = GRHICommandList.GetImmediateCommandList();

			TShaderMapRef<FPaperCS> PaperCS(GetGlobalShaderMap(PaperComponent->GetScene()->GetFeatureLevel()));
			RHICmdList.SetComputeShader(PaperCS->GetComputeShader());

			PaperCS->SetParameters(
				RHICmdList,
				PaperComponent->PaperData.InTex0Tex,
				PaperComponent->PaperData.InTex1Tex,
				PaperComponent->PaperData.InTex2Tex,
				PaperComponent->PaperData.InTex3Tex,
				PaperComponent->PaperData.InRippedNoiseTex
			);

			PaperCS->SetParameters(
				RHICmdList,
				PaperComponent->PaperData.InTex0Sampler, ((FTexture2DResource*)PaperComponent->PaperData.InTex0Tex)->GetTexture2DRHI(),
				PaperComponent->PaperData.InTex1Sampler, ((FTexture2DResource*)PaperComponent->PaperData.InTex1Tex)->GetTexture2DRHI(),
				PaperComponent->PaperData.InTex2Sampler, ((FTexture2DResource*)PaperComponent->PaperData.InTex2Tex)->GetTexture2DRHI(),
				PaperComponent->PaperData.InTex3Sampler, ((FTexture2DResource*)PaperComponent->PaperData.InTex3Tex)->GetTexture2DRHI(),
				PaperComponent->PaperData.InRippedNoiseSampler, ((FTexture2DResource*)PaperComponent->PaperData.InRippedNoiseTex)->GetTexture2DRHI()
			);

			PaperCS->SetParameters(
				RHICmdList,
				PaperComponent->PaperData.InNoise.SRV
			);

			FPaperSurfaceParameters params;
			params.Size = PaperComponent->TextureSize;
			params.ThresholdCutoff = PaperComponent->ThresholdCutoff;
			PaperCS->SetUniformBuffers(RHICmdList, params);
			PaperCS->SetOutput(RHICmdList, PaperComponent->PaperData.OutTex);
			DispatchComputeShader(RHICmdList, *PaperCS, PaperComponent->TextureSize.X, PaperComponent->TextureSize.Y, 1);
			PaperCS->UnbindBuffers(RHICmdList);
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
			FMemory::Memcpy(ArrayData, TextureData, GPixelFormats[PF_A32B32G32R32F].BlockBytes * TextureRes->GetSizeX() * TextureRes->GetSizeY());
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
		UpdateInputs();
		MarkRenderDynamicDataDirty();
	}
	else
	{
		UE_LOG(LogEngineCode, Warning, TEXT("test"));
	}
}

void UPaperSurfaceComponent::UpdateInputs()
{
	FTexture** TexToBeCleaned[5];
	int TexCount = 0;
	if (PaperData.InTex0Tex)
	{
		TexToBeCleaned[TexCount++] = &PaperData.InTex0Tex;
	}
	if (PaperData.InTex1Tex)
	{
		TexToBeCleaned[TexCount++] = &PaperData.InTex1Tex;
	}
	if (PaperData.InTex2Tex)
	{
		TexToBeCleaned[TexCount++] = &PaperData.InTex2Tex;
	}
	if (PaperData.InTex3Tex)
	{
		TexToBeCleaned[TexCount++] = &PaperData.InTex3Tex;
	}
	if (PaperData.InRippedNoiseTex)
	{
		TexToBeCleaned[TexCount++] = &PaperData.InRippedNoiseTex;
	}

	ENQUEUE_UNIQUE_RENDER_COMMAND_TWOPARAMETER(
		PaperSurfaceCleanTex,
		FTexture***, Textures, TexToBeCleaned,
		int, Count, TexCount,
		{
			for (int i = 0; i < Count; ++i)
			{
				(*(Textures[i]))->ReleaseRHI();
				(*(Textures[i]))->ReleaseResource();
				(*(Textures[i])) = 0;
			}
		});
	FlushRenderingCommands();

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
}

void UPaperSurfaceComponent::UpdateOutputAndRandom()
{
	if (TextureRes)
	{
		TextureRes->Release();
		TextureRes = 0;
	}
	if (PaperData.OutTex)
	{
		PaperData.OutTex->Release();
		PaperData.OutTex = 0;
	}
	
	FRHIResourceCreateInfo ResCreateInfo;
	ComputedColors.Init(TextureSize.X * TextureSize.Y);
	ResCreateInfo.ResourceArray = &ComputedColors;
	TextureRes = RHICreateTexture2D(TextureSize.X, TextureSize.Y, PF_A32B32G32R32F, 1, 1, TexCreate_ShaderResource | TexCreate_UAV, ResCreateInfo);
	PaperData.OutTex = RHICreateUnorderedAccessView(TextureRes);

	ENQUEUE_UNIQUE_RENDER_COMMAND_ONEPARAMETER(
		PaperSurfacePopulateRandom,
		FVertexBufferRHIParamRef, Buffer, PaperData.InNoise.Buffer,
		{
			FRandomStream random;
			random.GenerateNewSeed();
			random.GenerateNewSeed();
			float* res = (float*)RHILockVertexBuffer(Buffer, 0, Buffer->GetSize(), RLM_WriteOnly);
			for (int i = 0, c = 80/*16 * 5*/; i < c; i += 5)
			{
				res[i + 0] = random.GetFraction() * 6.28318530718f;
				res[i + 1] = random.GetFraction() * 2.f;
				res[i + 2] = random.GetFraction() * 2.f;
				res[i + 3] = random.GetFraction() - 0.5f;
				res[i + 4] = random.GetFraction() - 0.5f;
			}
			RHIUnlockVertexBuffer(Buffer);
		});
}

void UPaperSurfaceComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	if (!(Tex0 && Tex1 && Tex2 && Tex3 && RippedPaperTexture))
	{
		return;
	}
}
