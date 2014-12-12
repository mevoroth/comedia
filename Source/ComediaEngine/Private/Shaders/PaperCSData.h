#pragma once

#include "Shader.h"

struct PaperCSData
{
	FUnorderedAccessViewRHIRef OutTex;
	FUnorderedAccessViewRHIRef InTex0;
	FUnorderedAccessViewRHIRef InTex1;
	FUnorderedAccessViewRHIRef InTex2;
	FUnorderedAccessViewRHIRef InTex3;
	FUnorderedAccessViewRHIRef InRippedNoise;
	FUnorderedAccessViewRHIRef InNoise;
};
