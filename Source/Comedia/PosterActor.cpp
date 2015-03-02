// Fill out your copyright notice in the Description page of Project Settings.

#include "Comedia.h"

#include <limits>

#include "PosterActor.h"
#include "LiyaCharacter.h"
#include "MainLevelScriptActor.h"

APosterActor::APosterActor(const FObjectInitializer& FOI)
	: Super(FOI)
	, PosterMesh(0)
	, Precision(0.001f)
	, MaxIterations(10)
	, _DistanceFromHead(0)
	, _DistanceFromTail(0)
	, Grabbed(false)
	, ResetSpeed(1.f)
	, State(INIT)
	, DelayBetweenBones(0.15f)
	, DelayBeforeReset(1.f)
	, _StickedAlpha(0.f)
	, _ResetCalled(false)
	, _LastAnimatedObjectPosition(-std::numeric_limits<float>::infinity())
	, _LastOrientation(1.f)
	, _SoldierEnabled(true)
	, _SoldierElapsedTime(0.f)
{
	PosterMesh = FOI.CreateDefaultSubobject<UPoseableMeshComponent>(this, TEXT("Poster"));
	PosterMesh->Activate(true);
	RootComponent = PosterMesh;

	ConstructorHelpers::FObjectFinder<UMaterialInstance> MeshMaterial(TEXT("/Game/Materials/MI_Poster"));
	_MeshMaterialInst = MeshMaterial.Object;

	LeftGrabbedCamPosition = FOI.CreateDefaultSubobject<USceneComponent>(this, TEXT("Left Grabbed Cam Position"));
	LeftGrabbedCamPosition->AttachTo(RootComponent);

	RightGrabbedCamPosition = FOI.CreateDefaultSubobject<USceneComponent>(this, TEXT("Right Grabbed Cam Position"));
	RightGrabbedCamPosition->AttachTo(RootComponent);

	CallTrigger = FOI.CreateDefaultSubobject<UBoxComponent>(this, TEXT("CallTrigger"));
	CallTrigger->SetRelativeLocation(FVector(0.f, 550.f, 0.f));
	CallTrigger->SetBoxExtent(FVector(200.f, 200.f, 200.f));
	CallTrigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CallTrigger->SetCollisionProfileName(FName(TEXT("Trigger")));
	CallTrigger->AttachTo(RootComponent);

	PrimaryActorTick.bCanEverTick = true;
}

void APosterActor::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	UE_LOG(LogGPCode, Log, TEXT("PostEditChangeProperty"));
	//Check if there is as much element in KeyPoints than in KeyNodeTypes
	if (KeyPoints.Num() != KeyNodeTypes.Num())
	{
		KeyNodeTypes.SetNumZeroed(KeyPoints.Num());
	}
}

void APosterActor::BeginPlay()
{
	Super::BeginPlay();
	if (!PosterMesh)
	{
		return;
	}

	_ResetAlpha = 1.f + (PosterMesh->SkeletalMesh->RefSkeleton.GetNum() - 1) * DelayBetweenBones + DelayBeforeReset;
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
	bool HeadIsRoot = (State & HEADISROOT) != 0;
	int32 First = (HeadIsRoot ? 0 : PosterMesh->SkeletalMesh->RefSkeleton.GetNum() - 2);
	int32 Last = (HeadIsRoot ? PosterMesh->SkeletalMesh->RefSkeleton.GetNum() - 2 : 0);

	_DistanceFromTail = new float[PosterMesh->SkeletalMesh->RefSkeleton.GetNum()];
	// Take last bone
	Parent = PosterMesh->GetBoneLocationByName(PosterMesh->GetBoneName(PosterMesh->SkeletalMesh->RefSkeleton.GetNum() - 1), EBoneSpaces::WorldSpace);
	for (int32 i = PosterMesh->SkeletalMesh->RefSkeleton.GetNum() - 1; i >= 1; --i)
	{
		FVector CurrBoneLoc = PosterMesh->GetBoneLocationByName(PosterMesh->GetBoneName(i), EBoneSpaces::WorldSpace);
		_DistanceFromTail[i - 1] = FVector::Dist(Parent, CurrBoneLoc);
		Parent = CurrBoneLoc;
	}

	_MaxDistance = FVector::DistSquared(
		PosterMesh->GetBoneLocation(PosterMesh->GetBoneName(1)),
		PosterMesh->GetBoneLocation(PosterMesh->GetBoneName(PosterMesh->SkeletalMesh->RefSkeleton.GetNum() - 1))
	);

	_HeadDist = (GetGripHead() - _BonesInit[First].GetLocation()).Size();
	_TailDist = (GetGripTail() - _BonesInit[Last].GetLocation()).Size();

	FireRangeRadius = FMath::DegreesToRadians(FireRangeRadius);
	FireRangeDistance *= FireRangeDistance; // Square
}

void APosterActor::SetSoldier(USceneComponent* SoldierComponent)
{
	_SoldierComponent = SoldierComponent;
}

void APosterActor::SetSoldierTimelineComponent(UCurveFloat* TimelineComponent)
{
	_TimelineComponent = TimelineComponent;
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
	if (_BonesInit)
	{
		delete[] _BonesInit;
		_BonesInit = 0;
	}
	if (_BonesBuff)
	{
		delete[] _BonesBuff;
		_BonesBuff = 0;
	}
}

void APosterActor::UpdateChain()
{
	check(PosterMesh);

	bool HeadIsRoot = (State & HEADISROOT) != 0;
	int32 Count = PosterMesh->SkeletalMesh->RefSkeleton.GetNum();
	int32 First = (HeadIsRoot ? 1 : Count - 1);
	int32 Second = (HeadIsRoot ? 2 : Count - 2);
	int32 Last = (HeadIsRoot ? Count - 1 : 1);
	int32 End = (HeadIsRoot ? Count : 0);
	int32 It = (HeadIsRoot ? 1 : -1);
	float** Dist = (HeadIsRoot ? &_DistanceFromHead : &_DistanceFromTail);
	bool bBonesUpdated = false;

	for (int32 BoneIndex = 0, BoneCount = Count - 1; BoneIndex < BoneCount; ++BoneIndex)
	{
		_BonesBuff[BoneIndex] = PosterMesh->GetBoneTransform(BoneIndex + 1);
	}
	
	// Effector further than poster range
	if (FVector::DistSquared(_Effector.GetLocation(), _BonesBuff[First - 1].GetLocation()) > _MaxDistance)
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
#if defined WITH_EDITOR
			//DrawDebugSphere(GetWorld(), _BonesBuff[BoneIndex].GetLocation(), 10.f, 24, FColor(255 * (float)BoneIndex / BoneLast, 0, 0));
#endif
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
	int32 First = (State & HEADISROOT) ? PosterMesh->SkeletalMesh->RefSkeleton.GetNum() - 1 : 1;
	float Dist = (State & HEADISROOT) ? _HeadDist : _TailDist;

	_Effector = Effector;
	_Effector.AddToTranslation(
		(State & HEADISROOT ? -1 : 1) * PosterMesh->GetBoneTransform(First).GetUnitAxis(EAxis::X).SafeNormal() * Dist
	);
}

void APosterActor::Grabbing(bool Grabbing)
{
	ALiyaCharacter* Character = (ALiyaCharacter*)GetWorld()->GetFirstPlayerController()->GetCharacter();
	if ((State & GRABBABLE) && Grabbing)
	{
		switch (State & ~(GRABBABLE | HEADISROOT))
		{
		case INIT:
		case STICKED:
			State = (PosterState)((State & HEADISROOT) | GRABBED);
			if (!Character)
			{
				UE_LOG(LogGPCode, Error, TEXT("No Character"));
				return;
			}

			//Set character override camera position when poster grabbed
			if (Character->OverrideScriptedCameraPosition.GetLocation() == FVector::ZeroVector)
			{
				Character->GrabbingPlayerLocation = Character->GetActorLocation();
				Character->LengthTravellingScriptedCamera = LengthTravellingScriptedCamera;
				Character->ElapsedTravellingScriptedCamera = 0.0f;
				Character->RatioCameraFollow = 0.5f;
				if (State & HEADISROOT)
				{
					Character->OverrideScriptedCameraPosition = LeftGrabbedCamPosition->ComponentToWorld;
				}
				else
				{
					Character->OverrideScriptedCameraPosition = RightGrabbedCamPosition->ComponentToWorld;
				}
			}

			OnGrab(Character->GetActorLocation());
			ToggleFootStep();
			Character->NotifyGrab(_MaxDistance);
			break;
		}
	}
	else if (!Grabbing)
	{

		AMainLevelScriptActor* LevelScriptActor = Cast<AMainLevelScriptActor>(GetWorld()->GetLevelScriptActor());
		switch (State & ~(GRABBABLE | HEADISROOT))
		{
		case GRABBED:
			State = (PosterState)((State & HEADISROOT) | INIT);
			for (int32 BoneIndex = 0, BoneCount = PosterMesh->SkeletalMesh->RefSkeleton.GetNum() - 1; BoneIndex < BoneCount; ++BoneIndex)
			{
				_BonesBuff[BoneIndex] = PosterMesh->GetBoneTransform(BoneIndex + 1);
			}

			_ResetAlpha = 0.f;
			_ResetCalled = false;

			if (!Character)
			{
				UE_LOG(LogGPCode, Error, TEXT("No Character"));
				return;
			}

			OnRelease(Character->GetActorLocation());
			Character->NotifyReleasePoster();
			//Remove camera override
			if (Character->OverrideScriptedCameraPosition.GetLocation() == LeftGrabbedCamPosition->ComponentToWorld.GetLocation() || Character->OverrideScriptedCameraPosition.GetLocation() == RightGrabbedCamPosition->ComponentToWorld.GetLocation())
			{
				Character->StartTravellingPosition = Character->Camera->GetRelativeTransform();
				Character->LengthTravellingBackScriptedCamera = Character->ElapsedTravellingScriptedCamera;
				Character->OverrideScriptedCameraPosition = FTransform();

			}
			//Update graph nodes
			if (LevelScriptActor)
			{
				LevelScriptActor->CurrentLevelPathGraph.UpdatePosterNodes(this);
			}
			break;
		case ONSTICK:
			State = (PosterState)((State & (GRABBABLE | HEADISROOT)) | STICKED);
			OnRelease(Character->GetActorLocation());
			OnStick(Character->GetActorLocation());
			Character->NotifyReleasePoster();
			_GrabbedCurrentPosition = _Effector;
			_StickedAlpha = 0.f;
			//Update graph nodes
			if (LevelScriptActor)
			{
				LevelScriptActor->CurrentLevelPathGraph.UpdatePosterNodes(this);
			}
			break;
		}
	}
}

void APosterActor::InRange(bool HeadIsRoot)
{
	switch (State & ~(GRABBABLE | HEADISROOT))
	{
	case INIT:
		if (HeadIsRoot)
		{
			State = (PosterState)(State | HEADISROOT);
		}
		else
		{
			State = (PosterState)(State & ~HEADISROOT);
		}
		State = (PosterState)(State | GRABBABLE);
	case STICKED:
		if (HeadIsRoot && (State & HEADISROOT)
			|| !(HeadIsRoot || (State | ~HEADISROOT)))
		{
			State = (PosterState)(State | GRABBABLE);
		}
		break;
	}
}

void APosterActor::OutRange()
{
	switch (State & ~(GRABBABLE | HEADISROOT))
	{
	case INIT:
	case STICKED:
		State = (PosterState)(State & ~GRABBABLE);
		break;
	}
}


void APosterActor::Stick(bool Sticked)
{
	switch (State & ~(GRABBABLE | HEADISROOT))
	{
	case GRABBED:
		if (Sticked)
		{
			State = (PosterState)((State & HEADISROOT) | ONSTICK);
		}
		break;
	case INIT:
	case STICKED:
		if (!Sticked)
		{
			State = (PosterState)((State & HEADISROOT) | GRABBED);
		}
	}
}

FVector APosterActor::GetGripHeadUpdated() const
{
	int32 First = PosterMesh->SkeletalMesh->RefSkeleton.GetNum() - 1;

	FVector Loc = PosterMesh->GetBoneLocation(PosterMesh->GetBoneName(First));
	return Loc + PosterMesh->GetBoneTransform(First).GetUnitAxis(EAxis::X).SafeNormal() * _HeadDist;
}
FVector APosterActor::GetGripTailUpdated() const
{
	int32 First = 1;

	FVector Loc = PosterMesh->GetBoneLocation(PosterMesh->GetBoneName(First));
	return Loc - PosterMesh->GetBoneTransform(First).GetUnitAxis(EAxis::X).SafeNormal() * _TailDist;
}

void APosterActor::Tick(float DeltaSeconds)
{
	check(GetWorld());

	Super::Tick(DeltaSeconds);
	ALiyaCharacter* Character = 0;
	switch (State & ~(GRABBABLE | HEADISROOT))
	{
	case INIT:
		_Reset(DeltaSeconds);
		{
			AMainLevelScriptActor* LevelSCriptActor = Cast<AMainLevelScriptActor>(GetWorld()->GetLevelScriptActor());
			if (LevelSCriptActor)
			{
				float Ratio = LevelSCriptActor->GetPathGraph().GetCharacterPosition(this);
				UMaterialInstanceDynamic* MatInstance = PosterMesh->CreateDynamicMaterialInstance(0, _MeshMaterialInst);
				MatInstance->SetScalarParameterValue(FName(TEXT("SpritePosX")), Ratio);
				if (!FMath::IsNearlyEqual(Ratio, _LastAnimatedObjectPosition))
				{
					_LastOrientation = FMath::Sign(Ratio - _LastAnimatedObjectPosition);
					MatInstance->SetScalarParameterValue(FName(TEXT("Orientation")), _LastOrientation);
					_LastAnimatedObjectPosition = Ratio;
				}
			}
		}
		break;
	case ONSTICK:
	case GRABBED:
		_SoldierEnabled = false;
		_UpdateEffector();
		UpdateChain();
		Character = (ALiyaCharacter*)GetWorld()->GetFirstPlayerController()->GetCharacter();
		if (!Character)
		{
			UE_LOG(LogGPCode, Error, TEXT("No Character"));
			return;
		}
		Character->UpdateGrabPoint(State & HEADISROOT ? GetGripHeadUpdated() : GetGripTailUpdated());
		Character->UpdateGrabPivot(State & HEADISROOT ? GetGripTailUpdated() : GetGripHeadUpdated());
		break;
	case STICKED:
		SetEffector(FTransform(
			FQuat::Slerp(_GrabbedCurrentPosition.GetRotation(), FQuat::Identity, _StickedAlpha),
			FMath::Lerp(_GrabbedCurrentPosition.GetLocation(), _StickPointPos, _StickedAlpha),
			FVector(1.f, 1.f, 1.f)
		));
		_StickedAlpha = FMath::Clamp(_StickedAlpha + DeltaSeconds, 0.f, 1.f);
		UpdateChain();
		_SoldierEnabled = true;
		{
			AMainLevelScriptActor* LevelSCriptActor = Cast<AMainLevelScriptActor>(GetWorld()->GetLevelScriptActor());
			if (LevelSCriptActor)
			{
				float Ratio = LevelSCriptActor->GetPathGraph().GetCharacterPosition(this);
				UMaterialInstanceDynamic* MatInstance = PosterMesh->CreateDynamicMaterialInstance(0, _MeshMaterialInst);
				MatInstance->SetScalarParameterValue(FName(TEXT("SpritePosX")), Ratio);
				if (!FMath::IsNearlyEqual(Ratio, _LastAnimatedObjectPosition))
				{
					_LastOrientation = FMath::Sign(Ratio - _LastAnimatedObjectPosition);
					MatInstance->SetScalarParameterValue(FName(TEXT("Orientation")), _LastOrientation);
					_LastAnimatedObjectPosition = Ratio;
				}
			}
		}
		break;
	}

	_Soldier(DeltaSeconds);
}

void APosterActor::_Soldier(float DeltaSeconds)
{
	if (_SoldierEnabled)
	{
		float Nodes = PosterMesh->SkeletalMesh->RefSkeleton.GetNum() - 2;
		float Min, Max;
		_TimelineComponent->GetTimeRange(Min, Max);
		float NormalizedElapsedTime = FMath::Fmod(_SoldierElapsedTime, Max - Min);
		float SampledSoldier = _TimelineComponent->GetFloatValue(FMath::Fmod(_SoldierElapsedTime, Max - Min));
		
		SampledSoldier *= Nodes;
		SampledSoldier = FMath::Clamp(SampledSoldier, 0.f, Nodes);
		FVector A = PosterMesh->GetBoneLocation(PosterMesh->GetBoneName(FMath::FloorToInt(SampledSoldier) + 1));
		A.Z = _SoldierComponent->GetComponentLocation().Z;
		FVector B = PosterMesh->GetBoneLocation(PosterMesh->GetBoneName(FMath::CeilToInt(SampledSoldier) + 1));
		B.Z = A.Z;

		_SoldierComponent->SetWorldLocation(
			FMath::Lerp<FVector>(
				A, B,
				SampledSoldier - FMath::FloorToFloat(SampledSoldier)
			)
		);
		int32 ToggleCount = 0;
		for (int32 i = 0, c = ConeToggle.Num(); i < c && ConeToggle[i] < NormalizedElapsedTime; ++i)
		{
			++ToggleCount;
		}

		for (int32 i = 0, c = _SoldierComponent->GetNumChildrenComponents(); i < c; ++i)
		{
			if (_SoldierComponent->GetChildComponent(i)->GetName() == FString(TEXT("Vision")))
			{
				_SoldierComponent->GetChildComponent(i)->SetVisibility(ToggleCount % 2 == 0 ? false : true, true);
				break;
			}
		}

		_SoldierElapsedTime += DeltaSeconds;
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

bool APosterActor::IsDetached() const
{
	float Dist = 0.f;
	int32 c = PosterMesh->SkeletalMesh->RefSkeleton.GetNum();
	for (int32 i = 1; i < c; ++i)
	{
		FVector a = PosterMesh->GetBoneLocation(PosterMesh->GetBoneName(i));
		a.Z = 0;
		FVector b = _BonesInit[i - 1].GetLocation();
		b.Z = 0;
		Dist += FVector::Dist(a, b);
	}
	return Dist / c > 100.f;
}

void APosterActor::_Reset(float DeltaSeconds)
{
	float ResetAlphaNormalized = _ResetAlpha / ResetSpeed;

	bool bHeadIsRoot = (State & HEADISROOT) != 0;
	int32 BoneIndex = (bHeadIsRoot ? 0 : PosterMesh->SkeletalMesh->RefSkeleton.GetNum() - 2);
	int32 Last = (bHeadIsRoot ? PosterMesh->SkeletalMesh->RefSkeleton.GetNum() - 1 : -1);
	int32 It = (bHeadIsRoot ? 1 : -1);
	int32 Reverse = PosterMesh->SkeletalMesh->RefSkeleton.GetNum() - 2;

	ResetAlphaNormalized -= DelayBeforeReset;

	if (ResetAlphaNormalized - DelayBetweenBones * Reverse > 1.f)
	{
		OnResetFinished();
	}

	for (; BoneIndex != Last; BoneIndex += It)
	{
		float CurrentAlpha = FMath::Clamp(ResetAlphaNormalized - DelayBetweenBones * (bHeadIsRoot ? BoneIndex : Reverse - BoneIndex), 0.f, 1.f);
		PosterMesh->SetBoneTransformByName(
			PosterMesh->GetBoneName(BoneIndex + 1),
			FTransform(
				FQuat::Slerp(_BonesBuff[BoneIndex].GetRotation(), _BonesInit[BoneIndex].GetRotation(), CurrentAlpha),
				FMath::Lerp(_BonesBuff[BoneIndex].GetLocation(), _BonesInit[BoneIndex].GetLocation(), CurrentAlpha),
				FMath::Lerp(_BonesBuff[BoneIndex].GetScale3D(), _BonesInit[BoneIndex].GetScale3D(), CurrentAlpha)
			),
			EBoneSpaces::WorldSpace
		);
	}
	_ResetAlpha += DeltaSeconds;
}

FVector APosterActor::GetPosterForward() const
{
	FVector Head = PosterMesh->GetBoneLocation(PosterMesh->GetBoneName(1));
	FVector Tail = PosterMesh->GetBoneLocation(PosterMesh->GetBoneName(PosterMesh->SkeletalMesh->RefSkeleton.GetNum() - 1));
	return FVector::CrossProduct(Tail - Head, FVector::UpVector).UnsafeNormal();
}

bool APosterActor::IsInFireRange(const FVector& Position) const
{
	FVector Head = PosterMesh->GetBoneLocation(PosterMesh->GetBoneName(1));
	FVector Tail = PosterMesh->GetBoneLocation(PosterMesh->GetBoneName(PosterMesh->SkeletalMesh->RefSkeleton.GetNum() - 1));

	FVector Center = (Head + Tail) / 2.f;
	return FVector::DotProduct(
		FVector::CrossProduct(Tail - Head, FVector::UpVector).UnsafeNormal(),
		(Position - Center).UnsafeNormal()
	) && FVector::DistSquared(Position, Center) < FireRangeDistance;
}

void APosterActor::UpdateStickPoint(const FVector& StickPointPos)
{
	_StickPointPos = StickPointPos;
}

void APosterActor::OnResetFinished_Implementation()
{
	if (!_ResetCalled)
	{
		if (AssociatedBlockingVolume)
		{
			AssociatedBlockingVolume->SetActorEnableCollision(true);
		}
		_ResetCalled = true;
	}
	_SoldierEnabled = true;
}

void APosterActor::OnGrab_Implementation(const FVector& Position)
{
	if (AssociatedBlockingVolume)
	{
		AssociatedBlockingVolume->SetActorEnableCollision(false);
	}
}
