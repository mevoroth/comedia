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
	, ResetSpeed(1.f)
	, State(INIT)
	, DelayBetweenBones(0.15f)
	, DelayBeforeReset(1.f)
	, _StickedAlpha(0.f)
	, _ResetCalled(false)
	, _LastAnimatedObjectPosition(-std::numeric_limits<float>::infinity())
	, _LastOrientation(1.f)
	, _SoldierEnabled(true)
	, SoldierPatrolEnabled(false)
	, _SoldierElapsedTime(0.f)
	, _SoldierPreviousPos(0.f)
	, _MaxDistanceBetweenGrip(0.f)
	, FireRangeAngle(90.f)
	, DoorEnabled(false)
	, bShowFeedback(false)
	, EndingTrigger(0)
	, _WayFound(false)
{
	PosterRoot = FOI.CreateDefaultSubobject<USceneComponent>(this, TEXT("PosterRoot"));
	RootComponent = PosterRoot;

	PosterMesh = FOI.CreateDefaultSubobject<UPoseableMeshComponent>(this, TEXT("Poster"));
	PosterMesh->Activate(true);
	PosterMesh->AttachTo(PosterRoot);

	FeedbackMesh = FOI.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("FeedbackMesh"));
	FeedbackMesh->Activate(true);
	FeedbackMesh->AttachTo(PosterMesh);
	FeedbackMesh->SetRelativeLocation(FVector::ZeroVector);
	FeedbackMesh->SetVisibility(false);

	//ConstructorHelpers::FObjectFinder<UMaterialInstance> MeshMaterial(TEXT("/Game/Materials/MI_Poster"));
	//MeshMaterialInst = MeshMaterial.Object;

	DoorComponent = FOI.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("DoorComponent"));
	DoorComponent->TranslucencySortPriority = 100;
	DoorComponent->AttachTo(RootComponent);

	LeftGrabbedCamPosition = FOI.CreateDefaultSubobject<USceneComponent>(this, TEXT("Left Grabbed Cam Position"));
	LeftGrabbedCamPosition->AttachTo(PosterMesh);

	RightGrabbedCamPosition = FOI.CreateDefaultSubobject<USceneComponent>(this, TEXT("Right Grabbed Cam Position"));
	RightGrabbedCamPosition->AttachTo(PosterMesh);

	CallTrigger = FOI.CreateDefaultSubobject<UBoxComponent>(this, TEXT("CallTrigger"));
	CallTrigger->SetRelativeLocation(FVector(0.f, 550.f, 0.f));
	CallTrigger->SetBoxExtent(FVector(200.f, 200.f, 200.f));
	CallTrigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CallTrigger->SetCollisionProfileName(FName(TEXT("Trigger")));
	CallTrigger->AttachTo(RootComponent);

	PrimaryActorTick.bCanEverTick = true;
}

#if !defined UE_BUILD_SHIPPING
void APosterActor::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	UE_LOG(LogGPCode, Log, TEXT("PostEditChangeProperty"));
	//Check if there is as much element in KeyPoints than in KeyNodeTypes
	if (KeyPoints.Num() != KeyNodeTypes.Num())
	{
		KeyNodeTypes.SetNumZeroed(KeyPoints.Num());
	}
}
#endif

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

	_HeadDist = (GetGripHead() - _BonesInit[First].GetLocation()).Size() * 0.95f;
	_TailDist = (GetGripTail() - _BonesInit[Last].GetLocation()).Size() * 0.95f;

	//_MaxDistanceBetweenGrip = FVector::DistSquared(GetGripHead(), GetGripTail());
	_MaxDistanceBetweenGrip = _MaxDistance;

	FireRangeAngle = FMath::DegreesToRadians(FireRangeAngle);
	FireRangeDistance *= FireRangeDistance; // Square

	//Check if Poster is door linked to another poster and set it ungrabbable if needed
	if (DoorLinkedPoster != nullptr)
	{
		bIsGrabbable = false;
	}
}

void APosterActor::SetFeedbackObject(USceneComponent* FeedbackObject)
{
	_FeedbackObject = FeedbackObject;
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
			FVector Effector = _Effector.GetLocation();
			Effector.Z = _BonesBuff[Last - 1].GetLocation().Z;
			_BonesBuff[Last - 1].SetLocation(Effector);

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
			//DrawDebugSphere(GetWorld(), _BonesBuff[BoneIndex].GetLocation(), 10.f, 24, FColor((FMath::Rand() % 256), (FMath::Rand() % 256), (FMath::Rand() % 256)));
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
			FRotator R = _BonesBuff[BoneIndex - 1].GetRotation().Rotator();
			R.Pitch = FMath::Fmod(R.Pitch + 360.f, 360.f);
			if (R.Pitch > 180.f)
			{
				R.Pitch -= 360.f;
			}
			R.Pitch = FMath::Clamp(R.Pitch, -5.f, 5.f);
			R.Roll = FMath::Clamp(R.Roll, 85.f, 95.f);
			_BonesBuff[BoneIndex - 1].SetRotation(R.Quaternion());

			if (BoneIndex != First)
			{
				PosterMesh->SetBoneTransformByName(PosterMesh->GetBoneName(BoneIndex), _BonesBuff[BoneIndex - 1], EBoneSpaces::WorldSpace);
			}
		}
	}
}

void APosterActor::ResetPoster()
{
	State = RESET_FIRST_FRAME;
}

void APosterActor::SetEffector(const FTransform& Effector)
{
	int32 First = (State & HEADISROOT) ? PosterMesh->SkeletalMesh->RefSkeleton.GetNum() - 1 : 1;
	float Dist = (State & HEADISROOT) ? _HeadDist : _TailDist;

	_Effector = Effector;
	//_Effector.AddToTranslation(
	//	(State & HEADISROOT ? 1 : -1) * PosterMesh->GetBoneTransform(First).GetUnitAxis(EAxis::X).SafeNormal() * Dist
	//);
}

void APosterActor::Grabbing(bool Grabbing)
{
	ALiyaCharacter* Character = (ALiyaCharacter*)GetWorld()->GetFirstPlayerController()->GetCharacter();
	if ((State & GRABBABLE) && Grabbing && bIsGrabbable)
	{
		switch (State & ~(GRABBABLE | HEADISROOT))
		{
		case INIT:
			if (!_ResetCalled)
			{
				return;
			}
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
			Character->NotifyGrab(_MaxDistanceBetweenGrip);
			break;
		}
	}
	else if (!Grabbing)
	{

		AMainLevelScriptActor* LevelScriptActor = Cast<AMainLevelScriptActor>(GetWorld()->GetLevelScriptActor());
		switch (State & ~(GRABBABLE | HEADISROOT))
		{
		case GRABBED:
			State = (PosterState)((State & HEADISROOT) | GRABBABLE | INIT);
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
			_CancelOverridingCamPosition();
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
			//Remove camera override
			_CancelOverridingCamPosition();
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
	if (!bIsGrabbable)
	{
		return;
	}

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
		bShowFeedback = true;
		break;
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
	if (!bIsGrabbable)
	{
		return;
	}

	switch (State & ~(GRABBABLE | HEADISROOT))
	{
	case INIT:
	case STICKED:
		State = (PosterState)(State & ~GRABBABLE);
		bShowFeedback = false;
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

	if (EndingTrigger)
	{
		EndingTrigger->SetActorEnableCollision((State & ~(HEADISROOT | GRABBABLE)) == STICKED);
	}

	ALiyaCharacter* Character = 0;
	Character = (ALiyaCharacter*)GetWorld()->GetFirstPlayerController()->GetCharacter();
	switch (State & ~(GRABBABLE | HEADISROOT))
	{
	case INIT:
		_Reset(DeltaSeconds);
		_UpdateCompositedTexture();
		break;
	case ONSTICK:
	case GRABBED:
		_SoldierEnabled = false;
		_UpdateEffector();
		UpdateChain();
		if (!Character)
		{
			UE_LOG(LogGPCode, Error, TEXT("No Character"));
			return;
		}
		Character->UpdateGrabPoint(State & HEADISROOT ? GetGripHeadUpdated() : GetGripTailUpdated());
		Character->UpdateGrabPivot(State & HEADISROOT ? GetGripTailUpdated() : GetGripHeadUpdated());
		{
			int32 MiddleBoneInd = (PosterMesh->SkeletalMesh->RefSkeleton.GetNum() - 1) / 2 + 1;
			FVector Pos = PosterMesh->GetBoneLocation(PosterMesh->GetBoneName(MiddleBoneInd));
			FVector Orig = _BonesInit[MiddleBoneInd - 1].GetLocation();
			Pos.Z = 0.f;
			Orig.Z = 0.f;
			UE_LOG(LogGPCode, Warning, TEXT("%f"), FVector::DistSquared(
				Orig, Pos
				));
			if (FVector::DistSquared(
				Orig, Pos
			) > 10000)
			{
				if (AssociatedBlockingVolume && !_WayFound)
				{
					OnWayFound();
					_WayFound = true;
				}
			}
		}
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
		_UpdateCompositedTexture();
		break;
	case RESET_FIRST_FRAME:
		_ResetAlpha = 1.f + (PosterMesh->SkeletalMesh->RefSkeleton.GetNum() - 1) * DelayBetweenBones + DelayBeforeReset;
		_Reset(_ResetAlpha);

		if (Character)
		{
			Character->ResetPrinceAnims();
			Character->NotifyReleasePoster();
		}

		State = RESET_SECOND_FRAME;
		break;
	case RESET_SECOND_FRAME:
		State = (PosterState)((State & HEADISROOT) | INIT);
		break;
	}

	// Update feedbacks
	bShowFeedback = bShowFeedback || (bIsGrabbable && (State & GRABBABLE) != 0 && _ResetCalled);
	if (FeedbackMesh)
	{
		PosterMesh->SetRenderCustomDepth(bIsGrabbable && (State & GRABBABLE) != 0 && _ResetCalled);
		FeedbackMesh->SetVisibility(bIsGrabbable && (State & GRABBABLE) != 0 && _ResetCalled, true);
	}
	if (_FeedbackObject)
	{
		_FeedbackObject->SetVisibility(bShowFeedback, true);
		Character = (ALiyaCharacter*)GetWorld()->GetFirstPlayerController()->GetCharacter();
		_FeedbackObject->SetRelativeRotation(FRotator(0.f, 25.0f, 0.f));
	}

	if (DoorComponent)
	{
		DoorComponent->SetComponentTickEnabled(DoorEnabled);
		DoorComponent->SetVisibility(DoorEnabled, true);
		DoorComponent->SetHiddenInGame(!DoorEnabled, true);
	}

	if (DoorEnabled && DoorComponent)
	{
		FTransform MiddleBone = PosterMesh->GetBoneTransform((PosterMesh->SkeletalMesh->RefSkeleton.GetNum() - 2) / 2 + 1);

		FVector Loc = MiddleBone.GetLocation();
		Loc.Z -= 150.f;

		DoorComponent->SetWorldTransform(
			FTransform(
				MiddleBone.Rotator() + FRotator(0.f, -90.f, -90.f),
				Loc,
				FVector(3.f, 3.f, 3.f)
			)
		);
		DoorComponent->AddRelativeLocation(FVector(10.f, 0.f, 0.f));
	}

	if (_SoldierComponent)
	{
		_SoldierComponent->SetComponentTickEnabled(SoldierPatrolEnabled);
		_SoldierComponent->SetVisibility(SoldierPatrolEnabled, true);
		_SoldierComponent->SetHiddenInGame(!SoldierPatrolEnabled, true);
	}

	//Update callzone position
	FTransform TransformMiddlePosterBone = PosterMesh->GetBoneTransformByName(PosterMesh->GetBoneName(((PosterMesh->SkeletalMesh->RefSkeleton.GetNum() - 1) / 2) + 1), EBoneSpaces::WorldSpace);
	CallTrigger->SetWorldLocation(TransformMiddlePosterBone.GetLocation());

	//Update pathgraph on every tick
	AMainLevelScriptActor* MainLevelScriptActor = Cast<AMainLevelScriptActor>(GetWorld()->GetLevelScriptActor());
	if (MainLevelScriptActor)
	{
		MainLevelScriptActor->CurrentLevelPathGraph.UpdatePosterNodes(this);
	}

	if (SoldierPatrolEnabled)
	{
		_Soldier(DeltaSeconds);
		if (SoldierKills())
		{
			UE_LOG(LogGPCode, Warning, TEXT("IL EST MOURRU"));
			return;
		}
	}
}

void APosterActor::_UpdateCompositedTexture()
{
	AMainLevelScriptActor* LevelScriptActor = Cast<AMainLevelScriptActor>(GetWorld()->GetLevelScriptActor());
	ALiyaCharacter* Character = Cast<ALiyaCharacter>(GetWorld()->GetFirstPlayerController()->GetCharacter());
	if (LevelScriptActor)
	{
		float Ratio = LevelScriptActor->PathMainCharacter.GetCharacterPosition(this);
		UMaterialInstanceDynamic* MatInstance = PosterMesh->CreateDynamicMaterialInstance(0, MeshMaterialInst);
		MatInstance->SetScalarParameterValue(FName(TEXT("SpritePosX")), Ratio);
		PosterState FilteredState = (PosterState)(State & ~(GRABBABLE | HEADISROOT));
		if (Ratio != -1.f)
		{
			bool bLastIsSideNode = (LevelScriptActor->PathMainCharacter.LastCrossedNode->NodeType == ENodeType::NT_SideNode);
			if (bLastIsSideNode
				|| !FMath::IsNearlyEqual(Ratio, _LastAnimatedObjectPosition))
			{
				_LastOrientation = FMath::Sign(Ratio - _LastAnimatedObjectPosition);
				if (!bLastIsSideNode)
					MatInstance->SetScalarParameterValue(FName(TEXT("Orientation")), _LastOrientation);
				_LastAnimatedObjectPosition = Ratio;
				if (Character)
					Character->UpdateRunAnim(true);
			}
			else
			{
				if (Character)
					Character->UpdateRunAnim(false);
			}
		}
	}
}

float APosterActor::_GetSoldierDirection() const
{
	float Diff = _SoldierCurrentPos - _SoldierPreviousPos;
	if (FMath::IsNearlyZero(Diff))
	{
		return 0.f;
	}
	return FMath::Sign(Diff);
}

bool APosterActor::SoldierKills()
{
	ACharacter* Player = 0;
	if (GetWorld()
		&& GetWorld()->GetFirstPlayerController())
	{
		Player = GetWorld()->GetFirstPlayerController()->GetCharacter();
	}

	// Liya killed
	if ((State & ~(GRABBABLE | HEADISROOT)) != GRABBED && IsInFireRange(Player->GetActorLocation()))
	{
		OnKillLiyah();
		return true;
	}

	// Prince killed
	if (PrinceIsInFireRange())
	{
		OnKillPrince();
		return true;
	}
	return false;
}

bool APosterActor::PrinceIsInFireRange()
{
	AMainLevelScriptActor* LevelScriptActor = Cast<AMainLevelScriptActor>(GetWorld()->GetLevelScriptActor());

	if (LevelScriptActor)
	{
		if (LevelScriptActor->PathMainCharacter.bIsHidden)
		{
			return false;
		}

		float PrincePosition = -1.f;

		// Cone is disabled
		int32 ToggleCount = 0;
		float Min, Max;
		_TimelineComponent->GetTimeRange(Min, Max);
		float NormalizedElapsedTime = FMath::Fmod(_SoldierElapsedTime, Max - Min);
		for (int32 i = 0, c = ConeToggle.Num(); i < c && ConeToggle[i] < NormalizedElapsedTime; ++i)
		{
			++ToggleCount;
		}

		if (ToggleCount % 2 == 1)
		{
			TArray<APosterActor*> PosterKeys;
			LevelScriptActor->CurrentLevelPathGraph.MapHeadNodes.GetKeys(PosterKeys);
			for (int i = 0; i < PosterKeys.Num(); i++)
			{
				APosterActor* Poster = PosterKeys[i];
				PrincePosition = LevelScriptActor->PathMainCharacter.GetCharacterPosition(Poster);
				if (PrincePosition > 0.f && PrincePosition < 1.f)
				{
					FVector PrincePosterHead = Poster->PosterMesh->GetBoneLocation(PosterMesh->GetBoneName(1));
					FVector PrincePosterTail = Poster->PosterMesh->GetBoneLocation(PosterMesh->GetBoneName(PosterMesh->SkeletalMesh->RefSkeleton.GetNum() - 1));
					FVector PrincePos = FMath::Lerp<FVector>(PrincePosterHead, PrincePosterTail, PrincePosition);
					//UE_LOG(LogGPCode, Warning, TEXT("PRINCE MY BALLZ"));
					//UE_LOG(LogGPCode, Warning, TEXT("%s"), *Poster->GetName());
					//DrawDebugSphere(GetWorld(), PrincePos, 200.f, 64, FColor::Red);
					if (IsInFireRange(PrincePos))
					{
						return true;
					}
					break;
				}
			}
		}

		PrincePosition = LevelScriptActor->PathMainCharacter.GetCharacterPosition(this);
		// Prince is in same poster
		if (PrincePosition >= 0.f && PrincePosition <= 1.f)
		{
			return true;
		}

		PathNode* Node = LevelScriptActor->CurrentLevelPathGraph.GetLastNode(this);
		APosterActor* LeftPoster = 0;
		APosterActor* RightPoster = 0;
		if (Node->RightNode)
		{
			RightPoster = Node->RightNode->PosterOwner;
		}

		while (Node->LeftNode && Node->LeftNode->PosterOwner == this)
		{
			Node = Node->LeftNode;
		}
		if (Node->LeftNode && Node->LeftNode->PosterOwner != this)
		{
			LeftPoster = Node->LeftNode->PosterOwner;
		}

		if (LeftPoster)
		{
			PrincePosition = LevelScriptActor->PathMainCharacter.GetCharacterPosition(LeftPoster);
			float SoldierDir = _GetSoldierDirection();
			if (PrincePosition >= 0.f && PrincePosition <= 1.f
				&& _LastOrientation * SoldierDir >  0 && SoldierDir > 0)
			{
				return true;
			}
		}

		if (RightPoster)
		{
			PrincePosition = LevelScriptActor->PathMainCharacter.GetCharacterPosition(RightPoster);
			float SoldierDir = _GetSoldierDirection();
			if (PrincePosition >= 0.f && PrincePosition <= 1.f
				&& _LastOrientation * SoldierDir > 0 && SoldierDir < 0)
			{
				return true;
			}
		}

		return false;
	}
	return false;
}

void APosterActor::_Soldier(float DeltaSeconds)
{
	float Nodes = PosterMesh->SkeletalMesh->RefSkeleton.GetNum() - 2;
	float Min, Max;
	_TimelineComponent->GetTimeRange(Min, Max);
	float NormalizedElapsedTime = FMath::Fmod(_SoldierElapsedTime, Max - Min);
	float SampledSoldier = _TimelineComponent->GetFloatValue(FMath::Fmod(_SoldierElapsedTime, Max - Min));

	//Update soldier state
	if (SoldierState == ESoldierState::ST_Idle || SoldierState == ESoldierState::ST_Walking)
	{
		if (_SoldierPreviousPos == _SoldierCurrentPos)
		{
			SoldierState = ESoldierState::ST_Idle;
		}
		else
		{
			SoldierState = ESoldierState::ST_Walking;

			bSoldierFlipped = _SoldierCurrentPos > _SoldierPreviousPos;

			if ((State & ~(GRABBABLE | HEADISROOT)) != STICKED)
			{
				bSoldierFlipped = !bSoldierFlipped;
			}
		}
	}

	_SoldierPreviousPos = _SoldierCurrentPos;
	_SoldierCurrentPos = SampledSoldier;
		
	SampledSoldier *= Nodes;
	SampledSoldier = FMath::Clamp(SampledSoldier, 0.f, Nodes);
	FVector A = PosterMesh->GetBoneLocation(PosterMesh->GetBoneName(FMath::FloorToInt(SampledSoldier) + 1));
	A.Z = _SoldierComponent->GetComponentLocation().Z;
	FVector B = PosterMesh->GetBoneLocation(PosterMesh->GetBoneName(FMath::CeilToInt(SampledSoldier) + 1));
	B.Z = A.Z;
	FRotator AR = PosterMesh->GetBoneRotationByName(PosterMesh->GetBoneName(FMath::FloorToInt(SampledSoldier) + 1), EBoneSpaces::WorldSpace);
	FRotator BR = PosterMesh->GetBoneRotationByName(PosterMesh->GetBoneName(FMath::CeilToInt(SampledSoldier) + 1), EBoneSpaces::WorldSpace);
	
	if (PreviousSoldierState != SoldierState)
	{
		if (SoldierState == ESoldierState::ST_Walking)
		{
			OnSoldierWalk(FMath::Lerp<FVector>(
				A, B,
				SampledSoldier - FMath::FloorToFloat(SampledSoldier)
			));
		}
		else
		{
			OnSoldierStopWalking();
		}
	}

	//MiddleBone.Rotator() + FRotator(0.f, -90.f, -90.f)
	_SoldierComponent->SetWorldLocationAndRotation(
		FMath::Lerp<FVector>(
			A, B,
			SampledSoldier - FMath::FloorToFloat(SampledSoldier)
		),
		FMath::Lerp<FRotator>(
			AR, BR,
			SampledSoldier - FMath::FloorToFloat(SampledSoldier)
		) + FRotator(0.f, -90.f, -90.f)
	);
	//_SoldierComponent->AddRelativeLocation(FVector(10.f, 0.f, 0.f));
	int32 ToggleCount = 0;
	for (int32 i = 0, c = ConeToggle.Num(); i < c && ConeToggle[i] < NormalizedElapsedTime; ++i)
	{
		++ToggleCount;
	}

	for (int32 i = 0, c = _SoldierComponent->GetNumChildrenComponents(); i < c; ++i)
	{
		if (_SoldierComponent->GetChildComponent(i)->GetName() == FString(TEXT("Vision")))
		{
			_SoldierComponent->GetChildComponent(i)->SetVisibility(((State & GRABBED) != 0) || ToggleCount % 2 == 0 ? false : true, true);
			break;
		}
	}

	if (_SoldierEnabled)
	{
		_SoldierElapsedTime += DeltaSeconds;
	}

	PreviousSoldierState = SoldierState;
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

		//DrawDebugSphere(GetWorld(),
		//	FMath::Lerp<FVector>(LHSocketTransform.GetLocation(), RHSocketTransform.GetLocation(), 0.5f),
		//	10.f, 32, FColor((FMath::Rand() % 256), (FMath::Rand() % 256), (FMath::Rand() % 256)));

		SetEffector(FTransform(
			FQuat::Slerp(LHSocketTransform.GetRotation(), RHSocketTransform.GetRotation(), 0.5f),
			FMath::Lerp<FVector>(LHSocketTransform.GetLocation(), RHSocketTransform.GetLocation(), 0.5f),
			FVector(1.f)
		));
	}
}

float APosterActor::DetachRatio() const
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
	return FMath::Clamp(Dist / (c * 100.f), 0.f, 1.f);
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
	int32 ToggleCount = 0;
	float Min, Max;
	_TimelineComponent->GetTimeRange(Min, Max);
	float NormalizedElapsedTime = FMath::Fmod(_SoldierElapsedTime, Max - Min);
	for (int32 i = 0, c = ConeToggle.Num(); i < c && ConeToggle[i] < NormalizedElapsedTime; ++i)
	{
		++ToggleCount;
	}

	if (ToggleCount % 2 == 0)
	{
		return false;
	}

	FVector Head = PosterMesh->GetBoneLocation(PosterMesh->GetBoneName(1));
	FVector Tail = PosterMesh->GetBoneLocation(PosterMesh->GetBoneName(PosterMesh->SkeletalMesh->RefSkeleton.GetNum() - 1));
	//FVector SoldierPos = FMath::Lerp<FVector>(Head, Tail, _SoldierCurrentPos);

	float Nodes = PosterMesh->SkeletalMesh->RefSkeleton.GetNum() - 2;
	float CurrentPosInd = FMath::Clamp(_SoldierCurrentPos * Nodes, 0.f, Nodes);

	FVector A = PosterMesh->GetBoneLocation(PosterMesh->GetBoneName(FMath::FloorToInt(CurrentPosInd) + 1));
	A.Z = _SoldierComponent->GetComponentLocation().Z;
	FVector B = PosterMesh->GetBoneLocation(PosterMesh->GetBoneName(FMath::CeilToInt(CurrentPosInd) + 1));
	B.Z = A.Z;

	FVector SoldierPos = FMath::Lerp<FVector>(A, B, CurrentPosInd - FMath::FloorToInt(CurrentPosInd));

	SoldierPos.Z = Position.Z;
	//UE_LOG(LogGPCode, Warning, TEXT("ANGLE : %s"), *(Position - SoldierPos).UnsafeNormal().ToString());
	//UE_LOG(LogGPCode, Warning, TEXT("BALLZ : %s"), *FVector::CrossProduct(Tail - Head, FVector::UpVector).UnsafeNormal().ToString());
	//UE_LOG(LogGPCode, Warning, TEXT("SOLDIER : %s"), *SoldierPos.ToString());
	//UE_LOG(LogGPCode, Warning, TEXT("SOLDIER : %s"), *GetName());
	//UE_LOG(LogGPCode, Warning, TEXT("PRINCE MES COUILLES : %s"), *Position.ToString());

	//DrawDebugSphere(GetWorld(), SoldierPos, 500.f, 64, FColor::Red);
	return FVector::DotProduct(
		FVector::CrossProduct(Tail - Head, FVector::UpVector).UnsafeNormal(),
		(Position - SoldierPos).UnsafeNormal()
	) > FMath::Cos(FireRangeAngle / 2.f) && FVector::DistSquared(Position, SoldierPos) < FireRangeDistance;
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
			UE_LOG(LogGPCode, Warning, TEXT("ON : %s"), *AssociatedBlockingVolume->GetName());
		}
		_WayFound = false;
		_ResetCalled = true;
	}
	_SoldierEnabled = true;
}

void APosterActor::OnGrab_Implementation(const FVector& Position)
{
}

void APosterActor::OnWayFound_Implementation()
{
	if (AssociatedBlockingVolume)
	{
		UE_LOG(LogGPCode, Warning, TEXT("OFF : %s"), *AssociatedBlockingVolume->GetName());
		AssociatedBlockingVolume->SetActorEnableCollision(false);
	}
}

void APosterActor::OnKillLiyah_Implementation()
{
	UE_LOG(LogGPCode, Warning, TEXT("LIYA EST MOURRU"));
	ALiyaCharacter* Character = Cast<ALiyaCharacter>(GetWorld()->GetFirstPlayerController()->GetCharacter());
	if (Character && Character->CurrentRespawnZone)
	{
		Character->CurrentRespawnZone->StartRespawn();
	}

	SoldierState = ESoldierState::ST_ShootingFace;
}

void APosterActor::OnKillPrince_Implementation()
{
	UE_LOG(LogGPCode, Warning, TEXT("PRINCE EST MOURRU"));
	ALiyaCharacter* Character = Cast<ALiyaCharacter>(GetWorld()->GetFirstPlayerController()->GetCharacter());
	if (Character && Character->CurrentRespawnZone)
	{
		Character->CurrentRespawnZone->StartRespawn();
	}

	SoldierState = ESoldierState::ST_ShootingSide;
}

void APosterActor::_CancelOverridingCamPosition()
{
	//Remove camera override
	ALiyaCharacter* Character = Cast<ALiyaCharacter>(GetWorld()->GetFirstPlayerController()->GetCharacter());
	if (Character && Character->OverrideScriptedCameraPosition.GetLocation() == LeftGrabbedCamPosition->ComponentToWorld.GetLocation() || Character->OverrideScriptedCameraPosition.GetLocation() == RightGrabbedCamPosition->ComponentToWorld.GetLocation())
	{
		Character->StartTravellingPosition = Character->Camera->GetRelativeTransform();
		Character->LengthTravellingBackScriptedCamera = Character->ElapsedTravellingScriptedCamera;
		Character->OverrideScriptedCameraPosition = FTransform();

	}
}

bool APosterActor::IsPrinceCallable() const
{
	return KeyPoints.Num() > 0;
}

bool APosterActor::IsGrabbing() const
{
	return (State & GRABBED) != 0;
}
