#pragma once

#include "GlobalShader.h"

class FPaperCS : public FGlobalShader
{
	DECLARE_SHADER_TYPE(FPaperCS, Global);
	FPaperCS() {}
	explicit FPaperCS(const ShaderMetaType::CompiledShaderInitializerType& Initializer);
	//	: FGlobalShader(Initializer);

	static bool ShouldCache(EShaderPlatform Platform)
	{
		return IsFeatureLevelSupported(Platform, ERHIFeatureLevel::SM5);
	}
	//static void ModifyCompilationEnvironment(EShaderPlatform Platform, FShaderCompilerEnvironment& OutEnvironment);
	virtual bool Serialize(FArchive& Ar) override
	{
		bool bShaderHasOutdatedParams = FGlobalShader::Serialize(Ar);
		return bShaderHasOutdatedParams;
	}

	void UnbindBuffers(FRHICommandList& RHICmdList);
	void SetOutput(FRHICommandList& RHICmdList, FUnorderedAccessViewRHIRef OutFluidSurfaceUAV);

private:
	FShaderResourceParameter OutTextureFile;

};
