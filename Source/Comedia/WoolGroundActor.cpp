// All rights reserved

#include "Comedia.h"
#include "WoolGroundActor.h"

AWoolGroundActor::AWoolGroundActor(const FObjectInitializer& FOI)
	: Super(FOI)
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
	LFootPos.Z = 1.f - FMath::Clamp(LFootPos.Z / 15.f, 0.f, 1.f);

	RFootPos.X = 1.f - RFootPos.X / 20000.f;
	RFootPos.Y = 1.f - RFootPos.Y / 20000.f;
	RFootPos.Z = 1.f - FMath::Clamp(RFootPos.Z / 15.f, 0.f, 1.f);

	//UE_LOG(LogGPCode, Warning, TEXT("%s"), *LFootPos.ToString());
	//UE_LOG(LogGPCode, Warning, TEXT("%s"), *RFootPos.ToString());

	UMaterialInstanceDynamic* GroundMatInst = GroundMesh->CreateDynamicMaterialInstance(0, MeshMaterialInst);
	GroundMatInst->SetScalarParameterValue(FName(TEXT("LeftX")), LFootPos.X);
	GroundMatInst->SetScalarParameterValue(FName(TEXT("LeftY")), LFootPos.Y);
	GroundMatInst->SetScalarParameterValue(FName(TEXT("LeftDepthFootStep")), LFootPos.Z);
	
	GroundMatInst->SetScalarParameterValue(FName(TEXT("RightX")), RFootPos.X);
	GroundMatInst->SetScalarParameterValue(FName(TEXT("RightY")), RFootPos.Y);
	GroundMatInst->SetScalarParameterValue(FName(TEXT("RightDepthFootStep")), RFootPos.Z);

	//GroundMesh->GetMaterial(0)->OverrideScalarParameterDefault(FName(TEXT("LeftX")), LFootPos.X, true, ERHIFeatureLevel::SM5);
	//GroundMesh->GetMaterial(0)->OverrideScalarParameterDefault(FName(TEXT("LeftY")), LFootPos.Y, true, ERHIFeatureLevel::SM5);
	//GroundMesh->GetMaterial(0)->OverrideScalarParameterDefault(FName(TEXT("LeftDepthFootStep")), LFootPos.Z, true, ERHIFeatureLevel::SM5);
	//GroundMesh->GetMaterial(0)->OverrideScalarParameterDefault(FName(TEXT("RightX")), LFootPos.X, true, ERHIFeatureLevel::SM5);
	//GroundMesh->GetMaterial(0)->OverrideScalarParameterDefault(FName(TEXT("RightY")), LFootPos.Y, true, ERHIFeatureLevel::SM5);
	//GroundMesh->GetMaterial(0)->OverrideScalarParameterDefault(FName(TEXT("RightDepthFootStep")), LFootPos.Z, true, ERHIFeatureLevel::SM5);

	//float OutLPos;
	//GroundMesh->GetMaterial(0)->GetScalarParameterValue(FName(TEXT("LeftX")), OutLPos);
	//UE_LOG(LogGPCode, Warning, TEXT("LeftX : %f : %f"), LFootPos.X, OutLPos);
	//GroundMesh->GetMaterial(0)->GetScalarParameterValue(FName(TEXT("LeftY")), OutLPos);
	//UE_LOG(LogGPCode, Warning, TEXT("LeftY : %f : %f"), LFootPos.Y, OutLPos);
	//GroundMesh->GetMaterial(0)->GetScalarParameterValue(FName(TEXT("LeftDepthFootStep")), OutLPos);
	//UE_LOG(LogGPCode, Warning, TEXT("LeftDepthFootStep : %f : %f"), LFootPos.Z, OutLPos);

	//GroundMesh->GetMaterial(0)->GetScalarParameterValue(FName(TEXT("RightX")), OutLPos);
	//UE_LOG(LogGPCode, Warning, TEXT("RightX : %f : %f"), RFootPos.X, OutLPos);
	//GroundMesh->GetMaterial(0)->GetScalarParameterValue(FName(TEXT("RightY")), OutLPos);
	//UE_LOG(LogGPCode, Warning, TEXT("RightY : %f : %f"), RFootPos.Y, OutLPos);
	//GroundMesh->GetMaterial(0)->GetScalarParameterValue(FName(TEXT("RightDepthFootStep")), OutLPos);
	//UE_LOG(LogGPCode, Warning, TEXT("RightDepthFootStep : %f : %f"), RFootPos.Z, OutLPos);
	//GroundMesh->GetMaterial(0)->GetScalarParameterValue(FName(TEXT("SizeFootStep")), OutLPos);
	//UE_LOG(LogGPCode, Warning, TEXT("SizeFootStep : %f"), OutLPos);
}

