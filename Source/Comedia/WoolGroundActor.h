// All rights reserved

#pragma once

#include "Components/StaticMeshComponent.h"

#include "GameFramework/Actor.h"
#include "WoolGroundActor.generated.h"

/**
 * 
 */
UCLASS()
class COMEDIA_API AWoolGroundActor : public AActor
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "[Comedia]Wool Shader")
	float Deepness;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "[Comedia]Wool Shader")
	float InitialDepth;

	UStaticMeshComponent* GroundMesh;

	virtual void Tick(float DeltaSeconds) override;
private:
	UMaterialInstance* MeshMaterialInst;
	
};
