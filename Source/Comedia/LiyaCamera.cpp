// Fill out your copyright notice in the Description page of Project Settings.

#include "Comedia.h"
#include "LiyaCamera.h"
#include <string>
using namespace std;

ULiyaCamera::ULiyaCamera(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	
}

void ULiyaCamera::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	
}

void ULiyaCamera::AddRoll(float Val)
{
	float dist = GetRelativeTransform().GetTranslation().Size();
	SetRelativeLocation(FVector::ZeroVector);
	AddLocalRotation(FRotator(0.f, 0.f, Val));
	SetRelativeLocation(-GetForwardVector() * dist);
	UE_LOG(LogGPCode, Warning, TEXT("%s"), *GetRelativeTransform().GetLocation().ToString());
}
