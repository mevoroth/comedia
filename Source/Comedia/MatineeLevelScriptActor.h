// All rights reserved

#pragma once

#include "Engine/LevelScriptActor.h"
#include "MatineeLevelScriptActor.generated.h"

/**
 * Dirty class for handling events for flipbook
 * Hard patch to avoid touching actor
 */
UCLASS()
class COMEDIA_API AMatineeLevelScriptActor : public ALevelScriptActor
{
	GENERATED_UCLASS_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "[Comedia]Flipbook Patch")
	bool TickFlipbook(float DeltaSeconds, float& CurrentFrame);
	
	UFUNCTION(BlueprintCallable, Category = "[Comedia]Flipbook Patch")
	void SetFrameRate(float FrameRate);

	UFUNCTION(BlueprintCallable, Category = "[Comedia]Flipbook Patch")
	void SetKeyFrames(const TArray<float>& KeyFrames);

	UFUNCTION(BlueprintCallable, Category = "[Comedia]Flipbook Patch")
	void SetLength(float Length);

private:
	float _ElapsedTime;
	float _FrameRate;
	TArray<float> _KeyFrames;
	float _Length;
	int32 _Triggered;
};
