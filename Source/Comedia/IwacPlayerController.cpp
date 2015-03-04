// All rights reserved

#include "Comedia.h"
#include "IwacPlayerController.h"

AIwacPlayerController::AIwacPlayerController(const FObjectInitializer& FOI)
	: Super(FOI)
{
}

void AIwacPlayerController::GetAudioListenerPosition(FVector& OutLocation, FVector& OutFrontDir, FVector& OutRightDir)
{
	Super::GetAudioListenerPosition(OutLocation, OutFrontDir, OutRightDir);

	OutLocation = AudioListenerLocation;
}

void AIwacPlayerController::SetAudioListenerLocation(const FVector AudioListenerLocation)
{
	this->AudioListenerLocation = AudioListenerLocation;
}