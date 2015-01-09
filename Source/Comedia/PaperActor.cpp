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
	BillboardComponent->SetRelativeLocation(FVector::ZeroVector);
	//StaticMeshComponent = PCIP.CreateDefaultSubobject<class UStaticMeshComponent>(this, TEXT("StaticMeshComponent"));
	//StaticMeshComponent->SetRelativeLocation(FVector::ZeroVector);
	//ConstructorHelpers::FClassFinder<UStaticMesh> CubeFinder(TEXT("/Game/Meshes/Cube"));
	//StaticMeshComponent->SetStaticMesh((UStaticMesh*)CubeFinder.Class->GetClass());
	//StaticMeshComponent->AttachTo(RootComponent);
	PrimaryActorTick.bCanEverTick = true;
}

void APaperActor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	static bool updated = false;
	static UTexture2D* tex = UTexture2D::CreateTransient(PaperSurface->TextureSize.X, PaperSurface->TextureSize.Y, EPixelFormat::PF_A32B32G32R32F);
	if (PaperSurface->jobdone && !updated)
	{
		FTexture2DMipMap& Mip = tex->PlatformData->Mips[0];
		void* Data = Mip.BulkData.Lock(LOCK_READ_WRITE);
		FMemory::Memcpy(Data, PaperSurface->ComputedColors.GetData(), GPixelFormats[PF_A32B32G32R32F].BlockBytes * PaperSurface->TextureSize.X * PaperSurface->TextureSize.Y);
		Mip.BulkData.Unlock();
		tex->UpdateResource();
		updated = true;
	}
	BillboardComponent->SetSprite(tex);
}
