// Fill out your copyright notice in the Description page of Project Settings.

#include "Comedia.h"
#include "ComediaGameMode.h"


AComediaGameMode::AComediaGameMode(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	//Set default pawn class to our blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/Blueprints/FPSCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;


}


