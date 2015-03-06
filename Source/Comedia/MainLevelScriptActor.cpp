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
	//Check respawn matinee set
	check(RespawnMatinee);

	CurrentLevelPathGraph = PathGraph();
	CurrentLevelPathGraph.InitNodes(GetWorld());

	RandomNode = CurrentLevelPathGraph.GetRandomNode();

	PathMainCharacter.World = GetWorld();
	PathMainCharacter.CurrentPathGraph = &CurrentLevelPathGraph;
	if (CurrentLevelPathGraph.StartNode)
	{
		PathMainCharacter.SetCharacterNode(CurrentLevelPathGraph.StartNode);
	}
	else
	{
		UE_LOG(LogGPCode, Warning, TEXT("No start node set!"));
		PathMainCharacter.SetCharacterNode(RandomNode);
	}
}

void AMainLevelScriptActor::Tick(float DeltaSeconds)
{
	//CurrentLevelPathGraph.DrawNodes();
	//CurrentLevelPathGraph.DrawPath(RandomNode);
	PathMainCharacter.UpdateCharacter(DeltaSeconds);
}