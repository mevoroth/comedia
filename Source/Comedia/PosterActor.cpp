// Fill out your copyright notice in the Description page of Project Settings.

#include "Comedia.h"
#include "PosterActor.h"

APosterActor::APosterActor(const FObjectInitializer& FOI)
	: Super(FOI)
	, PosterMesh(0)
	, Precision(0.001f)
	, MaxIterations(10)
	, _DistanceFromHead(0)
	, _DistanceFromTail(0)
	, _HeadIsRoot(false)
	, Grabbed(false)
{
	PosterMesh = FOI.CreateDefaultSubobject<UPoseableMeshComponent>(this, TEXT("Poster"));
	PosterMesh->Activate(true);
	RootComponent = PosterMesh;

	PrimaryActorTick.bCanEverTick = true;
}

void APosterActor::BeginPlay()
{
	Super::BeginPlay();
	if (!PosterMesh)
	{
		return;
	}

	_BonesBuff = new FTransform[PosterMesh->SkeletalMesh->RefSkeleton.GetNum() - 1];
	_BonesInit = new FTransform[PosterMesh->SkeletalMesh->RefSkeleton.GetNum() - 1];

	// Start from 1 to avoid getting "master" bone
	_DistanceFromHead = new float[PosterMesh->SkeletalMesh->RefSkeleton.GetNum() - 1];
	FVector Parent = PosterMesh->GetBoneLocationByName(PosterMesh->GetBoneName(1), EBoneSpaces::WorldSpace);
	for (int32 i = 1, c = PosterMesh->SkeletalMesh->RefSkeleton.GetNum(); i < c; ++i)
	{
		FVector CurrBoneLoc = PosterMesh->GetBoneLocationByName(PosterMesh->GetBoneName(i), EBoneSpaces::WorldSpace);
		_DistanceFromHead[i - 1] = FVector::Dist(Parent, CurrBoneLoc);
		_BonesInit[i - 1] = PosterMesh->GetBoneTransform(i);
		Parent = CurrBoneLoc;
	}

	_DistanceFromTail = new float[PosterMesh->SkeletalMesh->RefSkeleton.GetNum()];
	// Take last bone
	Parent = PosterMesh->GetBoneLocationByName(PosterMesh->GetBoneName(PosterMesh->SkeletalMesh->RefSkeleton.GetNum() - 1), EBoneSpaces::WorldSpace);
	for (int32 i = PosterMesh->SkeletalMesh->RefSkeleton.GetNum() - 1; i >= 1; --i)
	{
		FVector CurrBoneLoc = PosterMesh->GetBoneLocationByName(PosterMesh->GetBoneName(i), EBoneSpaces::WorldSpace);
		_DistanceFromTail[i - 1] = FVector::Dist(Parent, CurrBoneLoc);
		Parent = CurrBoneLoc;
	}

	_MaxDistance = FVector::Dist(
		PosterMesh->GetBoneLocation(PosterMesh->GetBoneName(1)),
		PosterMesh->GetBoneLocation(PosterMesh->GetBoneName(PosterMesh->SkeletalMesh->RefSkeleton.GetNum() - 1))
	);
}

void APosterActor::BeginDestroy()
{
	Super::BeginDestroy();

	if (_DistanceFromHead)
	{
		delete[] _DistanceFromHead;
		_DistanceFromHead = 0;
	}
	if (_DistanceFromTail)
	{
		delete[] _DistanceFromTail;
		_DistanceFromTail = 0;
	}
}

void APosterActor::UpdateChain()
{
	check(PosterMesh);

	int32 Count = PosterMesh->SkeletalMesh->RefSkeleton.GetNum();
	int32 First = (_HeadIsRoot ? 1 : Count - 1);
	int32 Second = (_HeadIsRoot ? 2 : Count - 2);
	int32 Last = (_HeadIsRoot ? Count - 1 : 1);
	int32 End = (_HeadIsRoot ? Count : 0);
	int32 It = (_HeadIsRoot ? 1 : -1);
	float** Dist = (_HeadIsRoot ? &_DistanceFromHead : &_DistanceFromTail);
	bool bBonesUpdated = false;

	for (int32 BoneIndex = 0, BoneCount = Count - 1; BoneIndex < BoneCount; ++BoneIndex)
	{
		_BonesBuff[BoneIndex] = PosterMesh->GetBoneTransform(BoneIndex + 1);
	}
	
	// Effector further than poster range
	if (FVector::Dist(_Effector.GetLocation(), _BonesBuff[First - 1].GetLocation()) > _MaxDistance)
	{
		FVector Dir = (_Effector.GetLocation() - _BonesBuff[First - 1].GetLocation()).UnsafeNormal();
		for (int32 BoneIndex = Second; BoneIndex != End; BoneIndex += It)
		{
			FVector Parent = _BonesBuff[BoneIndex - It - 1].GetLocation();

			_BonesBuff[BoneIndex - 1].SetLocation(Parent + Dir * (*Dist)[BoneIndex - 1]);
		}
		bBonesUpdated = true;
	}
	else // Effector within poster range
	{
		float Slop = FVector::Dist(_Effector.GetLocation(), _BonesBuff[Last - 1].GetLocation());
		if (Slop > Precision)
		{
			_BonesBuff[Last - 1].SetLocation(_Effector.GetLocation());

			int32 IterationsCount = 0;
			while (Slop > Precision && (IterationsCount++ < MaxIterations))
			{
				// FW Reaching
				for (int32 BoneIndex = Last - It; BoneIndex != First; BoneIndex -= It)
				{
					FVector Current = _BonesBuff[BoneIndex - 1].GetLocation();
					FVector Child = _BonesBuff[BoneIndex + It - 1].GetLocation();

					_BonesBuff[BoneIndex - 1].SetLocation(Child - (Child - Current).UnsafeNormal() * (*Dist)[BoneIndex - 1]);
				}

				// BW Reaching
				for (int32 BoneIndex = Second; BoneIndex != Last; BoneIndex += It)
				{
					FVector Current = _BonesBuff[BoneIndex - 1].GetLocation();
					FVector Parent = _BonesBuff[BoneIndex - It - 1].GetLocation();

					_BonesBuff[BoneIndex - 1].SetLocation(Parent + (Current - Parent).UnsafeNormal() * (*Dist)[BoneIndex - 1]);
				}

				Slop = FMath::Abs((*Dist)[Last - 1] - FVector::Dist(_BonesBuff[Last - It - 1].GetLocation(), _Effector.GetLocation()));

				{
					FVector Current = _BonesBuff[Last - 1].GetLocation();
					FVector Parent = _BonesBuff[Last - It - 1].GetLocation();

					_BonesBuff[Last - 1].SetLocation(Parent + (Current - Parent).UnsafeNormal() * (*Dist)[Last - 1]);
				}
			}
		}

		bBonesUpdated = true;
	}

	if (bBonesUpdated)
	{
		for (int32 BoneIndex = 0, BoneLast = Count - 1; BoneIndex < BoneLast; ++BoneIndex)
		{
			DrawDebugSphere(GetWorld(), _BonesBuff[BoneIndex].GetLocation(), 10.f, 24, FColor(255 * (float)BoneIndex / BoneLast, 0, 0));
			PosterMesh->SetBoneTransformByName(PosterMesh->GetBoneName(BoneIndex + 1), _BonesBuff[BoneIndex], EBoneSpaces::WorldSpace);
		}

		for (int32 BoneIndex = First, BoneLast = Last; BoneIndex != BoneLast; BoneIndex += It)
		{
			FVector Current = PosterMesh->GetBoneLocation(PosterMesh->GetBoneName(BoneIndex));
			FVector Child = PosterMesh->GetBoneLocation(PosterMesh->GetBoneName(BoneIndex + It));
			FVector OldDir = (Child - Current).UnsafeNormal();

			Current = _BonesBuff[BoneIndex - 1].GetLocation();
			Child = _BonesBuff[BoneIndex + It - 1].GetLocation();
			FVector NewDir = (Child - Current).UnsafeNormal();

			FVector RotationAxis = FVector::CrossProduct(OldDir, NewDir).SafeNormal();
			float RotationAngle = FMath::Acos(FVector::DotProduct(OldDir, NewDir));
			FQuat DeltaRotation = FQuat(RotationAxis, RotationAngle);
			check(DeltaRotation.IsNormalized());

			_BonesBuff[BoneIndex - 1].SetRotation(DeltaRotation * _BonesBuff[BoneIndex - 1].GetRotation());
		}
	}
}

void APosterActor::SetEffector(const FTransform& Effector)
{
	_Effector = Effector;
}

void APosterActor::Grabbing(bool Grabbing)
{
	Grabbed = false;
	if (GrabEnabled)
	{
		Grabbed = Grabbing;
	}
}

void APosterActor::InRange(bool HeadIsRoot)
{
	if (!Grabbed)
	{
		_HeadIsRoot = HeadIsRoot;
		GrabEnabled = true;
	}
}

void APosterActor::OutRange()
{
	if (!Grabbed)
	{
		GrabEnabled = false;
	}
}

void APosterActor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (Grabbed)
	{
		_UpdateEffector();

		UpdateChain();
	}
}

void APosterActor::_UpdateEffector()
{
	ACharacter* Player = 0;
	if (GetWorld()
		&& GetWorld()->GetFirstPlayerController())
	{
		Player = GetWorld()->GetFirstPlayerController()->GetCharacter();
	}

	if (Player)
	{
		FTransform RHSocketTransform = Player->GetMesh()->GetSocketTransform(FName(TEXT("RHandSocket")));
		FTransform LHSocketTransform = Player->GetMesh()->GetSocketTransform(FName(TEXT("LHandSocket")));

		SetEffector(FTransform(
			FQuat::Slerp(LHSocketTransform.GetRotation(), RHSocketTransform.GetRotation(), 0.5f),
			FMath::Lerp<FVector>(LHSocketTransform.GetLocation(), RHSocketTransform.GetLocation(), 0.5f),
			FVector(1.f)
		));
	}
}
