#include "ComediaEngine.h"
#include "PaperCS.h"

#include "ShaderParameterUtils.h"

//void FPaperCS::ModifyCompilationEnvironment(EShaderPlatform Platform, FShaderCompilerEnvironment& OutEnvironment)
//{
//
//}

IMPLEMENT_UNIFORM_BUFFER_STRUCT(FPaperSurfaceParameters, TEXT("PaperSurfaceParameters"))

FPaperCS::FPaperCS()
{
	UE_LOG(LogEngineCode, Warning, TEXT("TEST"));
}

FPaperCS::FPaperCS(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
	: FGlobalShader(Initializer)
{
	OutTextureRes.Bind(Initializer.ParameterMap, TEXT("OutTex"));
	InTex0Res.Bind(Initializer.ParameterMap, TEXT("InTex0"));
	InTex1Res.Bind(Initializer.ParameterMap, TEXT("InTex1"));
	InTex2Res.Bind(Initializer.ParameterMap, TEXT("InTex2"));
	InTex3Res.Bind(Initializer.ParameterMap, TEXT("InTex3"));
	InNoiseRes.Bind(Initializer.ParameterMap, TEXT("InNoise"));
	InRippedNoiseRes.Bind(Initializer.ParameterMap, TEXT("RippedNoise"));
	InTex0SamplerRes.Bind(Initializer.ParameterMap, TEXT("InTex0Sampler"));
	InTex1SamplerRes.Bind(Initializer.ParameterMap, TEXT("InTex1Sampler"));
	InTex2SamplerRes.Bind(Initializer.ParameterMap, TEXT("InTex2Sampler"));
	InTex3SamplerRes.Bind(Initializer.ParameterMap, TEXT("InTex3Sampler"));
	InRippedNoiseSamplerRes.Bind(Initializer.ParameterMap, TEXT("RippedNoiseSampler"));
	InSize.Bind(Initializer.ParameterMap, TEXT("Size"));
	InThresholdCutoff.Bind(Initializer.ParameterMap, TEXT("ThresholdCutoff"));
}

void FPaperCS::SetOutput(FRHICommandList& RHICmdList, FUnorderedAccessViewRHIRef OutPaperSurfaceUAV)
{
	FComputeShaderRHIParamRef ComputeShaderRHI = GetComputeShader();
	if (OutTextureRes.IsBound())
	{
		RHICmdList.SetUAVParameter(ComputeShaderRHI, OutTextureRes.GetBaseIndex(), OutPaperSurfaceUAV);
	}
}

void FPaperCS::UnbindBuffers(FRHICommandList& RHICmdList)
{
	FComputeShaderRHIParamRef ComputeShaderRHI = GetComputeShader();
	if (OutTextureRes.IsBound())
	{
		RHICmdList.SetUAVParameter(ComputeShaderRHI, OutTextureRes.GetBaseIndex(), FUnorderedAccessViewRHIRef());
	}

	if (InTex0Res.IsBound())
	{
		RHICmdList.SetUAVParameter(ComputeShaderRHI, InTex0Res.GetBaseIndex(), FUnorderedAccessViewRHIRef());
	}
	if (InTex1Res.IsBound())
	{
		RHICmdList.SetUAVParameter(ComputeShaderRHI, InTex1Res.GetBaseIndex(), FUnorderedAccessViewRHIRef());
	}
	if (InTex2Res.IsBound())
	{
		RHICmdList.SetUAVParameter(ComputeShaderRHI, InTex2Res.GetBaseIndex(), FUnorderedAccessViewRHIRef());
	}
	if (InTex3Res.IsBound())
	{
		RHICmdList.SetUAVParameter(ComputeShaderRHI, InTex3Res.GetBaseIndex(), FUnorderedAccessViewRHIRef());
	}
	if (InNoiseRes.IsBound())
	{
		RHICmdList.SetUAVParameter(ComputeShaderRHI, InNoiseRes.GetBaseIndex(), FUnorderedAccessViewRHIRef());
	}
	if (InRippedNoiseRes.IsBound())
	{
		RHICmdList.SetUAVParameter(ComputeShaderRHI, InRippedNoiseRes.GetBaseIndex(), FUnorderedAccessViewRHIRef());
	}
}

void FPaperCS::SetParameters(FRHICommandList& RHICmdList,
	const FTexture* InTex0Tex,
	const FTexture* InTex1Tex,
	const FTexture* InTex2Tex,
	const FTexture* InTex3Tex,
	const FTexture* InRippedNoiseTex
	)
{
	FComputeShaderRHIParamRef compute = GetComputeShader();
	SetTextureParameter(RHICmdList, compute, InTex0Res, InTex0SamplerRes, InTex0Tex);
	SetTextureParameter(RHICmdList, compute, InTex1Res, InTex1SamplerRes, InTex1Tex);
	SetTextureParameter(RHICmdList, compute, InTex2Res, InTex2SamplerRes, InTex2Tex);
	SetTextureParameter(RHICmdList, compute, InTex3Res, InTex3SamplerRes, InTex3Tex);
	SetTextureParameter(RHICmdList, compute, InRippedNoiseRes, InRippedNoiseSamplerRes, InRippedNoiseTex);
}
void FPaperCS::SetParameters(FRHICommandList& RHICmdList,
	FShaderResourceViewRHIRef InNoiseSRV)
{
	FComputeShaderRHIParamRef compute = GetComputeShader();
	SetSRVParameter(RHICmdList, compute, InNoiseRes, InNoiseSRV);
	//SetUAVParameter(RHICmdList, GetComputeShader(), InNoiseRes, InNoiseUAV);
}
void FPaperCS::SetParameters(FRHICommandList& RHICmdList,
	FSamplerStateRHIParamRef InTex0SamplerStateRHI, FTextureRHIParamRef InTex0RHI,
	FSamplerStateRHIParamRef InTex1SamplerStateRHI, FTextureRHIParamRef InTex1RHI,
	FSamplerStateRHIParamRef InTex2SamplerStateRHI, FTextureRHIParamRef InTex2RHI,
	FSamplerStateRHIParamRef InTex3SamplerStateRHI, FTextureRHIParamRef InTex3RHI,
	FSamplerStateRHIParamRef InRippedNoiseSamplerStateRHI, FTextureRHIParamRef InRippedNoiseRHI)
{
	FComputeShaderRHIParamRef compute = GetComputeShader();
	SetTextureParameter(RHICmdList, compute, InTex0Res, InTex0SamplerRes, InTex0SamplerStateRHI, InTex0RHI);
	SetTextureParameter(RHICmdList, compute, InTex1Res, InTex1SamplerRes, InTex1SamplerStateRHI, InTex1RHI);
	SetTextureParameter(RHICmdList, compute, InTex2Res, InTex2SamplerRes, InTex2SamplerStateRHI, InTex2RHI);
	SetTextureParameter(RHICmdList, compute, InTex3Res, InTex3SamplerRes, InTex3SamplerStateRHI, InTex3RHI);
	SetTextureParameter(RHICmdList, compute, InRippedNoiseRes, InRippedNoiseSamplerRes, InRippedNoiseSamplerStateRHI, InRippedNoiseRHI);
}

//void FPaperCS::SetParameters(FRHICommandList& RHICmdList,
//	FUnorderedAccessViewRHIRef InTex0UAV,
//	FUnorderedAccessViewRHIRef InTex1UAV,
//	FUnorderedAccessViewRHIRef InTex2UAV,
//	FUnorderedAccessViewRHIRef InTex3UAV,
//	FUnorderedAccessViewRHIRef InRippedNoiseUAV,
//	FUnorderedAccessViewRHIRef InNoiseUAV)
//{
//	FComputeShaderRHIParamRef ComputeShaderRHI = GetComputeShader();
//
//	if (InTex0Res.IsBound())
//	{
//		RHICmdList.SetUAVParameter(ComputeShaderRHI, InTex0Res.GetBaseIndex(), InTex0UAV);
//	}
//	if (InTex1Res.IsBound())
//	{
//		RHICmdList.SetUAVParameter(ComputeShaderRHI, InTex1Res.GetBaseIndex(), InTex1UAV);
//	}
//	if (InTex2Res.IsBound())
//	{
//		RHICmdList.SetUAVParameter(ComputeShaderRHI, InTex2Res.GetBaseIndex(), InTex2UAV);
//	}
//	if (InTex3Res.IsBound())
//	{
//		RHICmdList.SetUAVParameter(ComputeShaderRHI, InTex3Res.GetBaseIndex(), InTex3UAV);
//	}
//	if (InNoiseRes.IsBound())
//	{
//		RHICmdList.SetUAVParameter(ComputeShaderRHI, InNoiseRes.GetBaseIndex(), InNoiseUAV);
//	}
//	if (InRippedNoiseRes.IsBound())
//	{
//		RHICmdList.SetUAVParameter(ComputeShaderRHI, InRippedNoiseRes.GetBaseIndex(), InRippedNoiseUAV);
//	}
//
//}
void FPaperCS::SetUniformBuffers(FRHICommandList& RHICmdList, FPaperSurfaceParameters& PaperSurfaceParams)
{
	FPaperSurfaceUniformBufferRef PaperSurfaceUniformBuffer;
	PaperSurfaceUniformBuffer = FPaperSurfaceUniformBufferRef::CreateUniformBufferImmediate(PaperSurfaceParams, UniformBuffer_SingleDraw);

	SetUniformBufferParameter(RHICmdList, GetComputeShader(), GetUniformBufferParameter<FPaperSurfaceParameters>(), PaperSurfaceUniformBuffer);
}

IMPLEMENT_SHADER_TYPE(, FPaperCS, TEXT("PaperSurface"), TEXT("PaperSurfaceCS"), SF_Compute);
