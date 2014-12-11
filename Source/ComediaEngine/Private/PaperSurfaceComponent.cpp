#include "ComediaEngine.h"
#include "PaperSurfaceRender.h"
#include "PaperSurfaceComponent.h"

UPaperSurfaceComponent::UPaperSurfaceComponent(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	UE_LOG(LogEngineCode, Warning, TEXT("test"));
	PrimaryComponentTick.bCanEverTick = true;
}

FPrimitiveSceneProxy* UPaperSurfaceComponent::CreateSceneProxy()
{
	return new FPaperSurfaceSceneProxy(this);
	return Data ? new FPaperSurfaceSceneProxy(this) : 0;
	return 0;
}

void UPaperSurfaceComponent::SendRenderDynamicData_Concurrent()
{
	Super::SendRenderDynamicData_Concurrent();
	ENQUEUE_UNIQUE_RENDER_COMMAND_TWOPARAMETER(
		FSendPaperSurfaceDynamicData,
		FPaperSurfaceSceneProxy*, PaperSurfaceSceneProxy, (FPaperSurfaceSceneProxy*)SceneProxy,
		FPaperSurfaceDynamicData*, DynamicData, 0,
		{
			//PaperSurfaceSceneProxy->SetDynamicData_RenderThread(DynamicData);
			PaperSurfaceSceneProxy->ExecComputeShader();
		});
}

void UPaperSurfaceComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	MarkRenderDynamicDataDirty();
}
