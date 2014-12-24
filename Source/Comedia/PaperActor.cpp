// Fill out your copyright notice in the Description page of Project Settings.

#include "Comedia.h"
#include "PaperActor.h"

APaperActor::APaperActor(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	PaperSurface = PCIP.CreateDefaultSubobject<class UPaperSurfaceComponent>(this, TEXT("Paper Surface"));
	PaperSurface->AttachTo(RootComponent);
	PaperSurface->SetRelativeLocation(FVector::ZeroVector);
	BillboardComponent = PCIP.CreateDefaultSubobject<class UBillboardComponent>(this, TEXT("Billboard"));
	BillboardComponent->AttachTo(RootComponent);
	PrimaryActorTick.bCanEverTick = true;
}

void APaperActor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	static bool updated = false;
	if (PaperSurface->jobdone && !updated)
	{
		//TArray<FColor> colors(PaperSurface->ComputedColors);
		//TArray<uint8> img;
		//FImageUtils::CompressImageArray(512, 512, colors, img);
		UTexture2D* tex = UTexture2D::CreateTransient(512, 512, EPixelFormat::PF_A32B32G32R32F);
		FTexture2DMipMap& Mip = tex->PlatformData->Mips[0];
		void* Data = Mip.BulkData.Lock(LOCK_READ_WRITE);
		FMemory::Memcpy(Data, PaperSurface->ComputedColors.GetData(), sizeof(FVector4) * 512 * 512);
		Mip.BulkData.Unlock();
		tex->UpdateResource();
		BillboardComponent->SetSprite(tex);
		updated = true;
	}
}
