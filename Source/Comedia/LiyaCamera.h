// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Camera/CameraComponent.h"
#include "LiyaCamera.generated.h"

/**
 * Liya's camera
 */
UCLASS(HideCategories = (Mobility, Rendering, LOD), ClassGroup = Camera, meta = (BlueprintSpawnableComponent))
class COMEDIA_API ULiyaCamera : public UCameraComponent
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Settings")
	float CameraSpeed;

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	inline float GetCameraSpeed() const
	{
		return CameraSpeed;
	}
	void AddRoll(float Val);
};
