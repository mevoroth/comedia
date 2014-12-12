#pragma once

#include "GlobalShader.h"

BEGIN_UNIFORM_BUFFER_STRUCT(FPaperSurfaceParameters,)
	DECLARE_UNIFORM_BUFFER_STRUCT_MEMBER(FVector2D, Size)
	DECLARE_UNIFORM_BUFFER_STRUCT_MEMBER(float, ThresholdCutoff)
END_UNIFORM_BUFFER_STRUCT(FPaperSurfaceParameters)

typedef TUniformBufferRef<FPaperSurfaceParameters> FPaperSurfaceUniformBufferRef;

class FPaperCS : public FGlobalShader
{
	DECLARE_SHADER_TYPE(FPaperCS, Global);
	FPaperCS();
	explicit FPaperCS(const ShaderMetaType::CompiledShaderInitializerType& Initializer);
	//	: FGlobalShader(Initializer);

	static bool ShouldCache(EShaderPlatform Platform)
	{
		return IsFeatureLevelSupported(Platform, ERHIFeatureLevel::SM5);
	}

	virtual bool Serialize(FArchive& Ar) override
	{
		bool bShaderHasOutdatedParams = FGlobalShader::Serialize(Ar);

		Ar << OutTextureRes;

		return bShaderHasOutdatedParams;
	}

	static void ModifyCompilationEnvironment(EShaderPlatform Platform, FShaderCompilerEnvironment& OutEnvironment)
	{
		FGlobalShader::ModifyCompilationEnvironment(Platform, OutEnvironment);
		OutEnvironment.CompilerFlags.Add(CFLAG_StandardOptimization);
	}

	void UnbindBuffers(FRHICommandList& RHICmdList);
	void SetOutput(FRHICommandList& RHICmdList, FUnorderedAccessViewRHIRef OutPaperSurfaceUAV);
	void SetParameters(FRHICommandList& RHICmdList,
		FUnorderedAccessViewRHIRef InTex0UAV,
		FUnorderedAccessViewRHIRef InTex1UAV,
		FUnorderedAccessViewRHIRef InTex2UAV,
		FUnorderedAccessViewRHIRef InTex3UAV,
		FUnorderedAccessViewRHIRef InRippedNoise,
		FUnorderedAccessViewRHIRef InNoiseUAV);
	void SetUniformBuffers(FRHICommandList& RHICmdList, FPaperSurfaceParameters& PaperSurfaceParams);

private:
	FShaderResourceParameter OutTextureRes;

	FShaderResourceParameter InTex0Res;
	FShaderResourceParameter InTex1Res;
	FShaderResourceParameter InTex2Res;
	FShaderResourceParameter InTex3Res;
	FShaderResourceParameter InNoiseRes;
	FShaderResourceParameter InRippedNoiseRes;
};
