// All rights reserved

#include "Comedia.h"
#include "RespawnZoneActor.h"
#include "PathNode.h"
#include "MainLevelScriptActor.h"
#include "PosterActor.h"

ARespawnZoneActor::ARespawnZoneActor(const FObjectInitializer& FOI)
	: Super(FOI)
{
}

void ARespawnZoneActor::BeginPlay()
{
	AMainLevelScriptActor* LevelScriptActor = Cast<AMainLevelScriptActor>(GetWorld()->GetLevelScriptActor());
	RespawnMatinee = LevelScriptActor->RespawnMatinee;
}

void ARespawnZoneActor::StartRespawn()
{
	if (RespawnMatinee)
	{
		RespawnMatinee->Play();
	}
	else
	{
		UE_LOG(LogGPCode, Error, TEXT("NO RESPAWN MATINEE FOUND"));
		Respawn();
	}
}

void ARespawnZoneActor::Respawn()
{
	//Replace player respawn position
	GetWorld()->GetFirstPlayerController()->GetCharacter()->SetActorLocation(PlayerRespawnLocation);

	//Replace Prince respawn position
	AMainLevelScriptActor* MainLevelScriptActor = Cast<AMainLevelScriptActor>(GetWorld()->GetLevelScriptActor());
	if (MainLevelScriptActor)
	{
		PathNode* RespawnNode = MainLevelScriptActor->CurrentLevelPathGraph.GetRespawnNode(PrinceRespawnPoster);
		if (PrinceRespawnPoster != nullptr)
		{
			MainLevelScriptActor->PathMainCharacter.SetCharacterNode(RespawnNode);
		}
	}

	//Replace posters
	for (int i = 0; i < PostersToReinit.Num(); i++)
	{
		PostersToReinit[i]->ResetPoster();
	}
}
