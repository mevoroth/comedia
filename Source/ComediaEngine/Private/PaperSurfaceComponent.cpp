#include "ComediaEngine.h"
#include "PaperSurfaceRender.h"
#include "PaperSurfaceComponent.h"

UPaperSurfaceComponent::UPaperSurfaceComponent(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{

}

FPrimitiveSceneProxy* UPaperSurfaceComponent::CreateSceneProxy()
{
	return Data ? new FPaperSurfaceSceneProxy(this) : 0;
	return 0;
}

void UPaperSurfaceComponent::SendRenderDynamicData_Concurrent()
{
	Super::SendRenderDynamicData_Concurrent();
	check(0);
}
