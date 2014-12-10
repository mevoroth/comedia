#include "ComediaEngine.h"
#include "PaperSurfaceRender.h"
#include "PaperCS.h"

//#include "Developer/ImageWrapper/Public/Interfaces/IImageWrapper.h"
//#include "Developer/ImageWrapper/Public/Interfaces/IImageWrapperModule.h"

FPaperSurfaceSceneProxy::FPaperSurfaceSceneProxy(UPaperSurfaceComponent* Component)
	: FPrimitiveSceneProxy(Component)
{
	//RHICreateTexture2D()
	FRHIResourceCreateInfo ResCreateInfo;
	TextureRes = RHICreateTexture2D(512, 512, PF_A32B32G32R32F, 1, 1, TexCreate_ShaderResource | TexCreate_UAV, ResCreateInfo);
	PaperSurfaceUAV = RHICreateUnorderedAccessView(TextureRes);


	//FString pngfile = "myimage.png";
	//IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
	//IImageWrapperPtr ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::PNG);

	//TArray<uint8> Result;

	//if (FFileHelper::LoadFileToArray(Result, *pngfile))
	//{
	//	if (ImageWrapper.IsValid() && ImageWrapper->SetCompressed(Result.GetData(), Result.Num()))
	//	{
	//		const TArray<uint8>* UncompressedBGRA = 0;
	//		if (ImageWrapper->GetRaw(ERGBFormat::BGRA, 8, UncompressedBGRA))
	//		{
	//			Tex = UTexture2D::CreateTransient(ImageWrapper->GetWidth(), ImageWrapper->GetHeight());
	//			void* TextureData = Tex->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
	//			FMemory::Memcpy(TextureData, UncompressedBGRA->GetTypedData(), UncompressedBGRA->Num());
	//			Tex->PlatformData->Mips[0].BulkData.Unlock();

	//			Tex->UpdateResource();
	//		}
	//	}
	//}

	//PaperSurfaceUAV = RHICreateUnorderedAccessView()
}

uint32_t FPaperSurfaceSceneProxy::GetMemoryFootprint() const
{
	return sizeof(*this) + GetAllocatedSize();
}

void FPaperSurfaceSceneProxy::DrawDynamicElements(FPrimitiveDrawInterface* PDI, const FSceneView* View)
{

}

void FPaperSurfaceSceneProxy::ExecComputeShader()
{
	FRHICommandListImmediate& RHICmdList = GRHICommandList.GetImmediateCommandList();

	TShaderMapRef<FPaperCS> PaperCS(GetGlobalShaderMap(GetScene()->GetFeatureLevel()));
	RHICmdList.SetComputeShader(PaperCS->GetComputeShader());

	//PaperCS->Set
	//PaperCS->SetParameters(RHICmdList, )
	PaperCS->SetOutput(RHICmdList, PaperSurfaceUAV);
	DispatchComputeShader(RHICmdList, *PaperCS, 8, 1, 1);
	PaperCS->UnbindBuffers(RHICmdList);
	//FRHITexture2D* tex = TextureRes->GetTexture2D();
	//TextureRes->GetNativeResource();
}
