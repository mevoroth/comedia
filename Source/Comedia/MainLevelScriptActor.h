// All rights reserved

#pragma once

#include "Engine/LevelScriptActor.h"
#include "PathGraph.h"
#include "PathCharacter.h"
#include "Matinee/MatineeActor.h"
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
	PathCharacter PathMainCharacter;
	PathNode* RandomNode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "[Comedia]LevelBP")
	AMatineeActor* RespawnMatinee;

	FORCEINLINE PathGraph GetPathGraph()
	{
		return CurrentLevelPathGraph;
	}

	AMainLevelScriptActor(const FObjectInitializer& FOI);
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
};
