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

	RandomNode = CurrentLevelPathGraph.GetRandomNode();

	PathMainCharacter.World = GetWorld();
	PathMainCharacter.CurrentPathGraph = &CurrentLevelPathGraph;

	PathMainCharacter.SetCharacterNode(CurrentLevelPathGraph.GetRandomNode());
}

void AMainLevelScriptActor::Tick(float DeltaSeconds)
{
	//CurrentLevelPathGraph.DrawNodes();
	//CurrentLevelPathGraph.DrawPath(RandomNode);
	PathMainCharacter.UpdateCharacter(DeltaSeconds);
}