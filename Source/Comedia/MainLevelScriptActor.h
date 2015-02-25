// All rights reserved

#pragma once

#include "Engine/LevelScriptActor.h"
#include "PathGraph.h"
#include "MainLevelScriptActor.generated.h"

/**
 * 
 */
UCLASS()
class COMEDIA_API AMainLevelScriptActor : public ALevelScriptActor
{
	GENERATED_BODY()

public:
	PathGraph CurrentLevelPathGraph;
	PathNode* RandomNode;

	AMainLevelScriptActor(const FObjectInitializer& FOI);
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
};
