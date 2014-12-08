// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "LowPassFilterComponent.generated.h"

/**
 * 
 */
UCLASS()
class COMEDIA_API ULowPassFilterComponent : public UActorComponent
{
	GENERATED_UCLASS_BODY()

	virtual ~ULowPassFilterComponent();

	/** @pre InitRecord() not called yet */
	void InitRecord(int Latency);
	void Push(float Value);
	float GetCurrentRecord() const;

private:
	TCircularBuffer<float>* LastValues;
	int32 CurrentInd;
	uint32 FrameCount;
};
