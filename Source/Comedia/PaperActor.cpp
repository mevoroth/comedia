// Fill out your copyright notice in the Description page of Project Settings.

#include "Comedia.h"
#include "PaperActor.h"


APaperActor::APaperActor(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	PaperSurface = PCIP.CreateDefaultSubobject<class UPaperSurfaceComponent>(this, TEXT("Paper Surface"));
	PaperSurface->AttachTo(RootComponent);
	PrimaryActorTick.bCanEverTick = true;
}

//void APaperActor::Tick(float DeltaSeconds)
//{
//	Super::Tick(DeltaSeconds);
//}
