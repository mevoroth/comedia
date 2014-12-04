// Fill out your copyright notice in the Description page of Project Settings.

#include "Comedia.h"
#include "LiyaCamera.h"
#include <string>
using namespace std;

ULiyaCamera::ULiyaCamera(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	
}

//void ULiyaCamera::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
//{
//	
//}
//
//void ULiyaCamera::AddRoll(float Val)
//{
//	AddLocalRotation(FRotator(Val, 0.f, 0.f));
//	if (RelativeRotation.Pitch < MinCamPitch)
//	{
//		//AddRelativeRotation(FRotator(RelativeRotation.Roll - MinCamRoll, 0.f, 0.f));
//		SetRelativeRotation(FRotator(MinCamPitch, RelativeRotation.Yaw, RelativeRotation.Roll));
//	}
//	else if (RelativeRotation.Pitch > MaxCamPitch)
//	{
//		//AddRelativeRotation(FRotator(MaxCamRoll - RelativeRotation.Roll, 0.f, 0.f));
//		SetRelativeRotation(FRotator(MaxCamPitch, RelativeRotation.Yaw, RelativeRotation.Roll));
//	}
//	float dist = RelativeLocation.Size();
//	SetRelativeLocation(FVector::ZeroVector);
//	SetRelativeLocation(-GetForwardVector() * dist);
//}
//
//void ULiyaCamera::AddYaw(float Val)
//{
//	AddLocalRotation(FRotator(0.f, Val, Val));
//}
//
//void ULiyaCamera::Reset()
//{
//	//SetRelativeRotation(FRotator(RelativeRotation.Pitch, 0.f, 0.f));
//	UE_LOG(LogGPCode, Warning, TEXT("test"));
//}
