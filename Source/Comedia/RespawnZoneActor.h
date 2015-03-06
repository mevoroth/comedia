// All rights reserved

#pragma once

#include "PosterActor.h"
#include "GameFramework/Actor.h"
#include "RespawnZoneActor.generated.h"

/**
 * 
 */
UCLASS()
class COMEDIA_API ARespawnZoneActor : public AActor
{
	GENERATED_BODY()
	
public:
	ARespawnZoneActor(const FObjectInitializer& FOI);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "[Comedia]RespawnZone")
	FVector PlayerRespawnLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "[Comedia]RespawnZone")
	APosterActor* PrinceRespawnPoster;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "[Comedia]RespawnZone")
	TArray<APosterActor*> PostersToReinit;

	UFUNCTION(BlueprintCallable, Category = "[Comedia]RespawnZone")
	void Respawn();
};
