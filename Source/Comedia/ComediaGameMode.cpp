// Fill out your copyright notice in the Description page of Project Settings.

#include "Comedia.h"
#include "ComediaGameMode.h"
#include "Scalability.h"


AComediaGameMode::AComediaGameMode(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	//Set default pawn class to our blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/Blueprints/Liya"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;


}

void AComediaGameMode::BeginPlay()
{
	if (GEngine)
	{
		UGameUserSettings* Settings = GEngine->GetGameUserSettings();
		Settings->SetScreenResolution(FIntPoint(1920, 1200));
		Settings->SetFullscreenMode(EWindowMode::Fullscreen);
		Settings->SetVSyncEnabled(true);
		Settings->ScalabilityQuality = Scalability::FQualityLevels();
		Settings->ApplySettings();
	}
}

