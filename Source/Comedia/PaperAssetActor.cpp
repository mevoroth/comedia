// Fill out your copyright notice in the Description page of Project Settings.

#include "Comedia.h"
#include "PaperAssetActor.h"

APaperAssetActor::APaperAssetActor(const FObjectInitializer& OI)
	: Super(OI)
	, Updated(false)
{
	PrimaryActorTick.bCanEverTick = true;

	PaperSurfaceComponent = OI.CreateDefaultSubobject<UPaperSurfaceComponent>(this, TEXT("Paper Surface"));
	PaperSurfaceComponent->AttachTo(RootComponent);

	MainTex = UTexture2D::CreateTransient(PaperSurfaceComponent->TextureSize.X, PaperSurfaceComponent->TextureSize.Y, PF_A32B32G32R32F);
}

void APaperAssetActor::Tick(float DeltaSeconds)
{
	check(MeshComponent);
	Super::Tick(DeltaSeconds);

	if (PaperSurfaceComponent->jobdone && !Updated)
	{
		FTexture2DMipMap& Mip = MainTex->PlatformData->Mips[0];
		void* Data = Mip.BulkData.Lock(LOCK_READ_WRITE);
		FMemory::Memcpy(Data, PaperSurfaceComponent->ComputedColors.GetData(), GPixelFormats[PF_A32B32G32R32F].BlockBytes * PaperSurfaceComponent->TextureSize.X * PaperSurfaceComponent->TextureSize.Y);
		Mip.BulkData.Unlock();
		MainTex->UpdateResource();
		Updated = true;
	}
	UTexture* Tex;
	MeshComponent->GetMaterial(0)->GetTextureParameterValue(TEXT("MainTex"), Tex);
	MeshComponent->GetMaterial(0)->OverrideTexture(Tex, MainTex, ERHIFeatureLevel::SM5);
}
