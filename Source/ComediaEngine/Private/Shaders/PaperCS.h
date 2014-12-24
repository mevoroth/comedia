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

		Ar << OutTextureRes
			<< InTex0Res
			<< InTex0SamplerRes
			<< InTex1Res
			<< InTex1SamplerRes
			<< InTex2Res
			<< InTex2SamplerRes
			<< InTex3Res
			<< InTex3SamplerRes
			<< InNoiseRes
			<< InRippedNoiseRes
			<< InRippedNoiseSamplerRes;

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
		const FTexture* InTex0Tex,
		const FTexture* InTex1Tex,
		const FTexture* InTex2Tex,
		const FTexture* InTex3Tex,
		const FTexture* InRippedNoiseTex
		);
	void SetParameters(FRHICommandList& RHICmdList,
		FShaderResourceViewRHIRef InNoiseSRV);
	void SetParameters(FRHICommandList& RHICmdList,
		FSamplerStateRHIParamRef InTex0SamplerStateRHI, FTextureRHIParamRef InTex0RHI,
		FSamplerStateRHIParamRef InTex1SamplerStateRHI, FTextureRHIParamRef InTex1RHI, 
		FSamplerStateRHIParamRef InTex2SamplerStateRHI, FTextureRHIParamRef InTex2RHI, 
		FSamplerStateRHIParamRef InTex3SamplerStateRHI, FTextureRHIParamRef InTex3RHI,
		FSamplerStateRHIParamRef InRippedNoiseSamplerStateRHI, FTextureRHIParamRef InRippedNoiseRHI);
	void SetUniformBuffers(FRHICommandList& RHICmdList, FPaperSurfaceParameters& PaperSurfaceParams);

private:
	FShaderResourceParameter OutTextureRes;

	FShaderResourceParameter InTex0Res;
	FShaderResourceParameter InTex0SamplerRes;
	FShaderResourceParameter InTex1Res;
	FShaderResourceParameter InTex1SamplerRes;
	FShaderResourceParameter InTex2Res;
	FShaderResourceParameter InTex2SamplerRes;
	FShaderResourceParameter InTex3Res;
	FShaderResourceParameter InTex3SamplerRes;
	FShaderResourceParameter InNoiseRes;
	FShaderResourceParameter InRippedNoiseRes;
	FShaderResourceParameter InRippedNoiseSamplerRes;
	FShaderResourceParameter InSize;
	FShaderResourceParameter InThresholdCutoff;
};
