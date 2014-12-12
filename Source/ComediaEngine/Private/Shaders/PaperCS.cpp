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
	FUnorderedAccessViewRHIRef InTex0UAV,
	FUnorderedAccessViewRHIRef InTex1UAV,
	FUnorderedAccessViewRHIRef InTex2UAV,
	FUnorderedAccessViewRHIRef InTex3UAV,
	FUnorderedAccessViewRHIRef InRippedNoiseUAV,
	FUnorderedAccessViewRHIRef InNoiseUAV)
{
	FComputeShaderRHIParamRef ComputeShaderRHI = GetComputeShader();

	if (InTex0Res.IsBound())
	{
		RHICmdList.SetUAVParameter(ComputeShaderRHI, InTex0Res.GetBaseIndex(), InTex0UAV);
	}
	if (InTex1Res.IsBound())
	{
		RHICmdList.SetUAVParameter(ComputeShaderRHI, InTex1Res.GetBaseIndex(), InTex1UAV);
	}
	if (InTex2Res.IsBound())
	{
		RHICmdList.SetUAVParameter(ComputeShaderRHI, InTex2Res.GetBaseIndex(), InTex2UAV);
	}
	if (InTex3Res.IsBound())
	{
		RHICmdList.SetUAVParameter(ComputeShaderRHI, InTex3Res.GetBaseIndex(), InTex3UAV);
	}
	if (InNoiseRes.IsBound())
	{
		RHICmdList.SetUAVParameter(ComputeShaderRHI, InNoiseRes.GetBaseIndex(), InNoiseUAV);
	}
	if (InRippedNoiseRes.IsBound())
	{
		RHICmdList.SetUAVParameter(ComputeShaderRHI, InRippedNoiseRes.GetBaseIndex(), InRippedNoiseUAV);
	}

}
void FPaperCS::SetUniformBuffers(FRHICommandList& RHICmdList, FPaperSurfaceParameters& PaperSurfaceParams)
{
	FPaperSurfaceUniformBufferRef PaperSurfaceUniformBuffer;
	PaperSurfaceUniformBuffer = FPaperSurfaceUniformBufferRef::CreateUniformBufferImmediate(PaperSurfaceParams, UniformBuffer_SingleDraw);

	SetUniformBufferParameter(RHICmdList, GetComputeShader(), GetUniformBufferParameter<FPaperSurfaceParameters>(), PaperSurfaceUniformBuffer);
}

IMPLEMENT_SHADER_TYPE(, FPaperCS, TEXT("PaperSurface"), TEXT("PaperSurfaceCS"), SF_Compute);
