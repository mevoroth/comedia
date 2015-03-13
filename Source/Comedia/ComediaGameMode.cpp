// Fill out your copyright notice in the Description page of Project Settings.

#include "Comedia.h"
#include "IwacPlayerController.h"
#include "ComediaGameMode.h"
#include "Scalability.h"


AComediaGameMode::AComediaGameMode(const class FObjectInitializer& PCIP)
	: Super(PCIP)
{
	//Set default pawn class to our blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/Blueprints/Liya"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	static ConstructorHelpers::FClassFinder<AHUD> HUDClassFinder(TEXT("/Game/Blueprints/BP_HUD"));
	HUDClass = HUDClassFinder.Class;

	PlayerControllerClass = AIwacPlayerController::StaticClass();

}

void AComediaGameMode::BeginPlay()
{
	if (GEngine)
	{
		FDisplayMetrics CurrentDisplayMetric;
		FDisplayMetrics::GetDisplayMetrics(CurrentDisplayMetric);
		UGameUserSettings* Settings = GEngine->GetGameUserSettings();
		Settings->SetScreenResolution(FIntPoint(CurrentDisplayMetric.PrimaryDisplayWidth, CurrentDisplayMetric.PrimaryDisplayHeight));
		Settings->SetFullscreenMode(EWindowMode::Fullscreen);
		Settings->SetVSyncEnabled(true);
		Settings->ScalabilityQuality = Scalability::FQualityLevels();
		Settings->ApplySettings(true);
	}
}

