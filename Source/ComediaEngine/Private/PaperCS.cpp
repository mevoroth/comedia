#include "ComediaEngine.h"
#include "PaperCS.h"

//void FPaperCS::ModifyCompilationEnvironment(EShaderPlatform Platform, FShaderCompilerEnvironment& OutEnvironment)
//{
//
//}

FPaperCS::FPaperCS()
{
	UE_LOG(LogEngineCode, Warning, TEXT("TEST"));
}

FPaperCS::FPaperCS(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
	: FGlobalShader(Initializer)
{
	OutTextureFile.Bind(Initializer.ParameterMap, TEXT("OutTextureFile"));
}

void FPaperCS::SetOutput(FRHICommandList& RHICmdList, FUnorderedAccessViewRHIRef OutTextureFileUAV)
{
	FComputeShaderRHIParamRef ComputeShaderRHI = GetComputeShader();
	if (OutTextureFile.IsBound())
	{
		RHICmdList.SetUAVParameter(ComputeShaderRHI, OutTextureFile.GetBaseIndex(), OutTextureFileUAV);
	}
}

void FPaperCS::UnbindBuffers(FRHICommandList& RHICmdList)
{
	FComputeShaderRHIParamRef ComputeShaderRHI = GetComputeShader();
	if (OutTextureFile.IsBound())
		RHICmdList.SetUAVParameter(ComputeShaderRHI, OutTextureFile.GetBaseIndex(), FUnorderedAccessViewRHIRef());
}

IMPLEMENT_SHADER_TYPE(, FPaperCS, TEXT("PaperSurface"), TEXT("PaperSurfaceCS"), SF_Compute);
