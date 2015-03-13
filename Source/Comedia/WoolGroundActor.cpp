// All rights reserved

#include "Comedia.h"
#include "WoolGroundActor.h"

AWoolGroundActor::AWoolGroundActor(const FObjectInitializer& FOI)
	: Super(FOI)
	, Deepness(20.f)
	, InitialDepth(9.375f)
{
	PrimaryActorTick.bCanEverTick = true;
	GroundMesh = FOI.CreateAbstractDefaultSubobject<UStaticMeshComponent>(this, TEXT("GroundMesh"));
	
	static ConstructorHelpers::FObjectFinder<UStaticMesh> StaticMesh(TEXT("/Game/Meshes/SM_plane.SM_plane"));
	GroundMesh->SetStaticMesh(StaticMesh.Object);

	ConstructorHelpers::FObjectFinder<UMaterialInstance> MeshMaterial(TEXT("/Game/Materials/MI_Wool.MI_Wool"));
	MeshMaterialInst = MeshMaterial.Object;
	GroundMesh->SetMaterial(0, MeshMaterialInst);
	GroundMesh->GetMaterial(0)->OverrideScalarParameterDefault(FName(TEXT("SizeFootStep")), 224.f, true, ERHIFeatureLevel::SM5);

	RootComponent = GroundMesh;
}

void AWoolGroundActor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	check(GetWorld());
	
	ACharacter* Character = 0;
	if (GetWorld()->GetFirstPlayerController())
	{
		Character = GetWorld()->GetFirstPlayerController()->GetCharacter();
	}

	if (!Character)
	{
		UE_LOG(LogGPCode, Warning, TEXT("No Character"));
	}

	FVector LFootPos = Character->GetMesh()->GetSocketLocation(FName(TEXT("LFootSocket"))) - GetActorLocation() + FVector(10000.f, 10000.f, 11.f);
	FVector RFootPos = Character->GetMesh()->GetSocketLocation(FName(TEXT("RFootSocket"))) - GetActorLocation() + FVector(10000.f, 10000.f, 11.f);


	LFootPos.X = 1.f - LFootPos.X / 20000.f;
	LFootPos.Y = 1.f - LFootPos.Y / 20000.f;
	LFootPos.Z = 1.f - FMath::Clamp((LFootPos.Z - InitialDepth) / Deepness, 0.f, 1.f);

	RFootPos.X = 1.f - RFootPos.X / 20000.f;
	RFootPos.Y = 1.f - RFootPos.Y / 20000.f;
	RFootPos.Z = 1.f - FMath::Clamp((RFootPos.Z - InitialDepth) / Deepness, 0.f, 1.f);

	UMaterialInstanceDynamic* GroundMatInst = GroundMesh->CreateDynamicMaterialInstance(0, MeshMaterialInst);
	GroundMatInst->SetScalarParameterValue(FName(TEXT("LeftX")), LFootPos.X);
	GroundMatInst->SetScalarParameterValue(FName(TEXT("LeftY")), LFootPos.Y);
	GroundMatInst->SetScalarParameterValue(FName(TEXT("LeftDepthFootStep")), LFootPos.Z);
	
	GroundMatInst->SetScalarParameterValue(FName(TEXT("RightX")), RFootPos.X);
	GroundMatInst->SetScalarParameterValue(FName(TEXT("RightY")), RFootPos.Y);
	GroundMatInst->SetScalarParameterValue(FName(TEXT("RightDepthFootStep")), RFootPos.Z);
}

