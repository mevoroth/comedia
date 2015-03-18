// Fill out your copyright notice in the Description page of Project Settings.

#include "Comedia.h"
#include "LowPassFilterComponent.h"


ULowPassFilterComponent::ULowPassFilterComponent(const class FObjectInitializer& PCIP)
	: Super(PCIP)
	, CurrentInd(-1)
	, FrameCount(0)
	, LastValues(0)
{
}

ULowPassFilterComponent::~ULowPassFilterComponent()
{
	delete LastValues;
	LastValues = 0;
}

void ULowPassFilterComponent::InitRecord(uint32 Latency)
{
	check(!LastValues); // Multiple Call at Init
	LastValues = new TCircularBuffer<float>(Latency);
	this->Latency = Latency;
	//UE_LOG(LogGPCode, Warning, TEXT("SIZEOF CIRCULAR BUFFER : %d"), LastValues->Capacity());
	check(LastValues); // Buffer overflow
}
void ULowPassFilterComponent::Push(float Value)
{
	check(LastValues);
	CurrentInd = LastValues->GetNextIndex(CurrentInd);
	(*LastValues)[CurrentInd] = Value;
	++FrameCount;
}
float ULowPassFilterComponent::GetCurrentRecord() const
{
	check(LastValues && CurrentInd >= 0);

	if (FrameCount < Latency)
	{
		return (*LastValues)[CurrentInd];
	}

	float record = 0.f;
	for (uint32 i = 0; i < Latency; ++i)
	{
		record += (*LastValues)[i];
	}
	return record / (float)Latency;
}
void ULowPassFilterComponent::Reset()
{
	check(LastValues);
	FrameCount = 0;
	CurrentInd = -1;
}
