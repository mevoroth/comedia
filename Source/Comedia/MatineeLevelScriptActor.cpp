// All rights reserved

#include "Comedia.h"
#include "MatineeLevelScriptActor.h"


AMatineeLevelScriptActor::AMatineeLevelScriptActor(const FObjectInitializer& FOI)
	: Super(FOI)
	, _ElapsedTime(0.f)
	, _Triggered(0)
{
}

bool AMatineeLevelScriptActor::TickFlipbook(float DeltaSeconds, float& CurrentFrame)
{
	_ElapsedTime += DeltaSeconds * 0.75f;
	if (_ElapsedTime >= _Length)
	{
		_ElapsedTime -= _Length;
		_Triggered = 0;
	}
	CurrentFrame = _ElapsedTime;
	int32 Count = 0;
	bool bCall = false;
	for (; Count < _KeyFrames.Num() && _KeyFrames[Count] < _ElapsedTime; ++Count);

	if (Count > _Triggered)
	{
		bCall = true;
		_Triggered = Count;
	}
	return bCall;
}

void AMatineeLevelScriptActor::SetFrameRate(float FrameRate)
{
	_FrameRate = FrameRate;
}

void AMatineeLevelScriptActor::SetKeyFrames(const TArray<float>& KeyFrames)
{
	for (int32 i = 0; i < KeyFrames.Num(); ++i)
	{
		_KeyFrames.Add(KeyFrames[i] / _FrameRate);
	}
}

void AMatineeLevelScriptActor::SetLength(float Length)
{
	_Length = Length;
}


