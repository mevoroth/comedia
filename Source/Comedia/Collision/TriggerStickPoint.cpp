// Fill out your copyright notice in the Description page of Project Settings.

#include "Comedia.h"
#include "TriggerStickPoint.h"
#include "PosterActor.h"

namespace
{
	static const FColor TriggerBaseColor(255, 0, 127, 255);
}
ATriggerStickPoint::ATriggerStickPoint(const FObjectInitializer& FOI)
	: Super(FOI.SetDefaultSubobjectClass<USphereComponent>(TEXT("CollisionComp")))
{
	USphereComponent* SphereCollisionComponent = CastChecked<USphereComponent>(GetCollisionComponent());

	SphereCollisionComponent->ShapeColor = TriggerBaseColor;
	SphereCollisionComponent->BodyInstance.bEnableCollision_DEPRECATED = true;
	static FName CollisionProfileName(TEXT("Trigger"));
	SphereCollisionComponent->SetCollisionProfileName(CollisionProfileName);
	//SphereCollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	//SphereCollisionComponent->SetCollisionResponseToAllChannels(ECR_Overlap);

	if (GetSpriteComponent())
	{
		GetSpriteComponent()->AttachParent = SphereCollisionComponent;
	}
	SphereCollisionComponent->OnComponentBeginOverlap.__Internal_AddDynamic(this, &ATriggerStickPoint::OnBeginOverlap, TEXT("ATriggerStickPoint::OnBeginOverlap"));
	SphereCollisionComponent->OnComponentEndOverlap.__Internal_AddDynamic(this, &ATriggerStickPoint::OnEndOverlap, TEXT("ATriggerStickPoint::OnEndOverlap"));
}

void ATriggerStickPoint::SetCollisionRadius(float Radius)
{
	USphereComponent* SphereCollisionComponent = CastChecked<USphereComponent>(GetCollisionComponent());
	SphereCollisionComponent->InitSphereRadius(Radius);
}

void ATriggerStickPoint::OnBeginOverlap(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const struct FHitResult& SweepResult)
{
	APosterActor* PosterActor = Cast<APosterActor>(OtherActor);
	if (PosterActor)
	{
		PosterActor->Stick(true);
		PosterActor->UpdateStickPoint(GetActorLocation());
	}
}

void ATriggerStickPoint::OnEndOverlap(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	APosterActor* PosterActor = Cast<APosterActor>(OtherActor);
	if (PosterActor)
	{
		PosterActor->Stick(false);
	}
}

