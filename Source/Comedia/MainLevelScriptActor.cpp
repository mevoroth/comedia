// All rights reserved

#include "Comedia.h"
#include "MainLevelScriptActor.h"

AMainLevelScriptActor::AMainLevelScriptActor(const FObjectInitializer& FOI)
	: Super(FOI)
{
	PrimaryActorTick.bCanEverTick = true;
}

void AMainLevelScriptActor::BeginPlay()
{
	CurrentLevelPathGraph = PathGraph();
	CurrentLevelPathGraph.InitNodes(GetWorld());
}
