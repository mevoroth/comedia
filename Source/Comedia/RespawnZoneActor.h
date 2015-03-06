// All rights reserved

#pragma once

#include "GameFramework/Actor.h"
#include "Matinee/MatineeActor.h"
#include "RespawnZoneActor.generated.h"
class APosterActor;
/**
 * 
 */
UCLASS()
class COMEDIA_API ARespawnZoneActor : public AActor
{
	GENERATED_BODY()
	
public:
	ARespawnZoneActor(const FObjectInitializer& FOI);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "[Comedia]RespawnZone")
	AMatineeActor* RespawnMatinee;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "[Comedia]RespawnZone")
	FVector PlayerRespawnLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "[Comedia]RespawnZone")
	APosterActor* PrinceRespawnPoster;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "[Comedia]RespawnZone")
	TArray<APosterActor*> PostersToReinit;

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "[Comedia]RespawnZone")
	void StartRespawn();

	UFUNCTION(BlueprintCallable, Category = "[Comedia]RespawnZone")
	void Respawn();
};
