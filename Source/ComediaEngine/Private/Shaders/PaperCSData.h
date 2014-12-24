#pragma once

#include "Shader.h"

struct PaperCSData
{
	FUnorderedAccessViewRHIRef OutTex;
	FTexture* InTex0Tex;
	FTexture* InTex1Tex;
	FTexture* InTex2Tex;
	FTexture* InTex3Tex;
	FTexture* InRippedNoiseTex;
	FSamplerStateRHIParamRef InTex0Sampler;
	FSamplerStateRHIParamRef InTex1Sampler;
	FSamplerStateRHIParamRef InTex2Sampler;
	FSamplerStateRHIParamRef InTex3Sampler;
	FSamplerStateRHIParamRef InRippedNoiseSampler;
	//FUnorderedAccessViewRHIRef InTex0;
	//FUnorderedAccessViewRHIRef InTex1;
	//FUnorderedAccessViewRHIRef InTex2;
	//FUnorderedAccessViewRHIRef InTex3;
	//FUnorderedAccessViewRHIRef InRippedNoise;
	FReadBuffer InNoise;
};
