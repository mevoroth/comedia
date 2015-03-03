// All rights reserved

#pragma once

#include "GameFramework/PlayerController.h"
#include "IwacPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class COMEDIA_API AIwacPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	FVector AudioListenerLocation;

	AIwacPlayerController(const FObjectInitializer& FOI);
	
	virtual void GetAudioListenerPosition(FVector& OutLocation, FVector& OutFrontDir, FVector& OutRightDir) override;

	void SetAudioListenerLocation(const FVector AudioListenerLocation);
};
