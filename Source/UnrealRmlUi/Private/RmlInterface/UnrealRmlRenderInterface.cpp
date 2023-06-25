#include "UnrealRmlRenderInterface.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "MediaPlayer.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


FTextureReferencer::FTextureReferencer(UTexture* InTexture, FString InResourcePath)
	: ResourcePath(InResourcePath)
	, Texture(InTexture)
{
	check(InTexture);
	Brush.SetResourceObject(InTexture);
	CachedResourceHandle = FSlateApplication::Get().GetRenderer()->GetResourceHandle(Brush);
}

FTextureReferencer::~FTextureReferencer()
{
	if (Brush.HasUObject() && FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().GetRenderer()->ReleaseDynamicResource(Brush);
	}
}

FMediaTextureReferencer::FMediaTextureReferencer(UMediaTexture* InMediaTexture, FString InResourcePath)
	: FTextureReferencer(InMediaTexture, InResourcePath)
	, Player(nullptr)
	, SoundComponent(nullptr)
	, Material(nullptr)
	, TextureSampler(nullptr)
{
	Player = NewObject<UMediaPlayer>();
	SoundComponent = NewObject<UMediaSoundComponent>();
	SoundComponent->bIsUISound = true;
	SoundComponent->bIsPreviewSound = true;
	SoundComponent->SetMediaPlayer(Player);
	SoundComponent->Initialize();
	Texture = InMediaTexture;
	InMediaTexture->AutoClear = true;
	InMediaTexture->SetMediaPlayer(Player);
	Texture->UpdateResource();
	Texture->AddToRoot();
	Material = NewObject<UMaterial>();
	TextureSampler = NewObject<UMaterialExpressionTextureSample>(Material);
	{
		TextureSampler->Texture = Texture;
		TextureSampler->AutoSetSampleType();
	}
	FExpressionOutput& Output = TextureSampler->GetOutputs()[0];
	FExpressionInput& Input = Material->EmissiveColor;
	{
		Input.Expression = TextureSampler;
		Input.Mask = Output.Mask;
		Input.MaskR = Output.MaskR;
		Input.MaskG = Output.MaskG;
		Input.MaskB = Output.MaskB;
		Input.MaskA = Output.MaskA;
	}

	FExpressionInput& Opacity = Material->Opacity;
	{
		Opacity.Expression = TextureSampler;
		Opacity.Mask = Output.Mask;
		Opacity.MaskR = 0;
		Opacity.MaskG = 0;
		Opacity.MaskB = 0;
		Opacity.MaskA = 1;
	}

	Material->BlendMode = BLEND_AlphaComposite;

	Material->Expressions.Add(TextureSampler);
	Material->MaterialDomain = EMaterialDomain::MD_UI;
	Material->PostEditChange();

	Player->OnMediaEvent().AddRaw(this, &FMediaTextureReferencer::OnMediaEvent);

	Brush.SetResourceObject(Material);
	CachedResourceHandle = FSlateApplication::Get().GetRenderer()->GetResourceHandle(Brush);

	if (Player->OpenFile(TEXT("C:\\Users\\Hslm\\Desktop\\aqaq.mp4")))
		Player->Play();
}

FMediaTextureReferencer::~FMediaTextureReferencer()
{
	if (Player)
	{
		Player->OnMediaEvent().RemoveAll(this);
	}
	if (SoundComponent)
	{
		SoundComponent->Stop();
		SoundComponent->SetMediaPlayer(nullptr);
		SoundComponent = nullptr;
	}
}

void FMediaTextureReferencer::OnMediaEvent(EMediaEvent Event)
{
	if (SoundComponent == nullptr)
		return;
	if (Event == EMediaEvent::PlaybackSuspended)
		SoundComponent->Stop();
	else if (Event == EMediaEvent::PlaybackResumed)
		SoundComponent->Start();
}

void FMediaTextureReferencer::Tick(float DeltaTime)
{
	if (SoundComponent)
	{
		SoundComponent->UpdatePlayer();
	}
}

void CUnrealRmlRenderInterface::RenderGeometry(Rml::Vertex* vertices, int num_vertices, int* indices, int num_indices, Rml::TextureHandle texture, const Rml::Vector2f& translation)
{
	FTextureReferencer* TextureReferencer = reinterpret_cast<FTextureReferencer*>(texture);

	const FSlateRenderTransform& AccumulatedRenderTransform = AllottedGeometry->GetAccumulatedRenderTransform(); // WidgetToScreen
	if (bUseTransformMatrix)
	{
		// TODO:
		//Transforms allow the rendered location and size of any geometry to be modified, and is necessary to support the transform RCSS property. RmlUi will call the following function when the transform matrix needs to change, and expects all geometry rendered afterwards to apply the given transform.
		//
		//// Called by RmlUi when it wants the renderer to use a new transform matrix.
		//// If no transform applies to the current element, nullptr is submitted. Then it expects the renderer to use
		//// an identity matrix or otherwise omit the multiplication with the transform.
		//// @param[in] transform The new transform to apply, or nullptr if no transform applies to the current element.
		//virtual void SetTransform(const Rml::Matrix4f* transform);
		//A nullptr will be submitted when RmlUi wants to set the transform back to identity. This function is never called if there are no transform properties present.
		//
		//Important things to be aware of to get correct results:
		//
		//The Matrix4f type is in column-major ordering. If your graphics API of choice takes row-major matrices, the matrix must first be transposed before submitting it to the graphics API.
		//When a draw call is received through one of the Render...() calls, the translation vector should first be applied to the position of the vertices. Then the resulting 2d-vector should be extended to a 4d-vector with elements z = 0 and w = 1 for correct results of the translation and perspective parts of the transform.
		//The provided transform matrix does not include projection to the user��s window, thus, the user should create their own projection matrix project and use the product of project * transform to produce the vertex position output.
		//Make sure to set the zfar and znear planes of your projection matrix sufficiently far away from the document plane (z=0) so that geometry is not clipped when it is rotated or translated into the z-axis.
		//When both a transform and scissor region is active, the scissor region must also have the transform matrix applied for correct results. One solution to this is to draw a rectangle with the position and dimensions of the scissor region into a stencil buffer with the transform applied. Then, when rendering vertices, use this stencil buffer to reject pixels outside of the drawn parts of the stencil buffer.
		//Pseudo vertex shader code for the vertex positions:
		//
		//input Vec2 vertex_pos;
		//input Vec2 translation;
		//input Mat4 transform;
		//input Mat4 project;
		//
		//output Vec4 frag_pos;
		//
		//void main() {
		//	Vec4 pos_document = Vec4(vertex_pos + translation, 0, 1);
		//	frag_pos = project * transform * pos_document; 
		//}
	}
	TArray<FSlateVertex> VertexBuffer;
	VertexBuffer.SetNum(num_vertices, false);
	for (size_t i = 0; i < num_vertices; i++)
	{
		Rml::Vertex& RmlVertex = vertices[i];
		FSlateVertex& SlateVertex = VertexBuffer[i];
		SlateVertex.TexCoords[0] = RmlVertex.tex_coord.x;
		SlateVertex.TexCoords[1] = RmlVertex.tex_coord.y;
		SlateVertex.TexCoords[2] = SlateVertex.TexCoords[3] = 1.f;
		SlateVertex.Position = (FVector2f)AccumulatedRenderTransform.TransformPoint(FVector2D(translation.x + RmlVertex.position.x, translation. y + RmlVertex.position.y));
		SlateVertex.Color.R = RmlVertex.colour.red;
		SlateVertex.Color.G = RmlVertex.colour.green;
		SlateVertex.Color.B = RmlVertex.colour.blue;
		SlateVertex.Color.A = RmlVertex.colour.alpha;
	}

	TArray<SlateIndex> IndexBuffer;
	IndexBuffer.SetNumUninitialized(num_indices, false);
	for (size_t i = 0; i < num_indices; i++)
	{
		IndexBuffer[i] = indices[i];
	}

	if (bUseClipRect)
	{
		FVector2D ClipRectLeftTop     = AccumulatedRenderTransform.TransformPoint(FVector2D(ClipRect.Left, ClipRect.Top));
		FVector2D ClipRectRightBottom = AccumulatedRenderTransform.TransformPoint(FVector2D(ClipRect.Right, ClipRect.Bottom));
		OutDrawElements->PushClip(FSlateClippingZone{ FSlateRect(ClipRectLeftTop, ClipRectRightBottom) });
	}

	static auto ResourceHandle = FSlateApplication::Get().GetRenderer()->GetResourceHandle(FSlateBrush());

	FSlateDrawElement::MakeCustomVerts(
		*OutDrawElements, 
		LayerId, 
		TextureReferencer ? TextureReferencer->GetResourceHandle() : ResourceHandle,
		VertexBuffer,
		IndexBuffer,
		nullptr,
		0,
		0);
	if (bUseClipRect)
		OutDrawElements->PopClip();
}

void CUnrealRmlRenderInterface::EnableScissorRegion(bool enable)
{
	bUseClipRect = enable;
}

void CUnrealRmlRenderInterface::SetScissorRegion(int x, int y, int width, int height)
{
	ClipRect.Left = static_cast<float>(FMath::Max(x, 0));
	ClipRect.Top = static_cast<float>(FMath::Max(y, 0));
	ClipRect.Right = static_cast<float>(x + width);
	ClipRect.Bottom = static_cast<float>(y + height);
}

bool CUnrealRmlRenderInterface::LoadTexture(Rml::TextureHandle& texture_handle, Rml::Vector2i& texture_dimensions, const Rml::String& source)
{
	TSharedPtr<FTextureReferencer>* FindTextureReferencerResult;
	FString ResourcePath = source.c_str();
	if (source.size() >= 8 && 0 == FCStringAnsi::Strncmp(source.c_str(), "media://", 8))
	{
		TSharedPtr<FMediaTextureReferencer> MediaTextureReferencer = MakeShared<FMediaTextureReferencer>(NewObject<UMediaTexture>(), ResourcePath);
		MediaTextureReferencerSet.Add(MediaTextureReferencer);
		FindTextureReferencerResult = &TextureReferencerMap.Add(ResourcePath, MediaTextureReferencer);
	}
	else
	{
		if (FPaths::IsRelative(ResourcePath))
		{
			ResourcePath = FPaths::Combine(FPaths::ProjectContentDir(), TEXT("Rml"), ResourcePath);
		}
		ResourcePath = FPaths::ConvertRelativePathToFull(ResourcePath);
		FindTextureReferencerResult = TextureReferencerMap.Find(ResourcePath);
		if (!FindTextureReferencerResult)
		{
			UTexture2D* LoadedTexture = nullptr;
			auto& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
			if (PlatformFile.FileExists(*ResourcePath))
				LoadedTexture = LoadTextureFromFile(ResourcePath);
			else
				LoadedTexture = LoadTextureFromAsset(ResourcePath);
			if (!LoadedTexture) return false;
			FindTextureReferencerResult = &TextureReferencerMap.Add(ResourcePath, MakeShared<FTextureReferencer>(LoadedTexture, ResourcePath));
		}
	}
	texture_handle = reinterpret_cast<Rml::TextureHandle>((*FindTextureReferencerResult).Get());
	texture_dimensions.x = (*FindTextureReferencerResult)->Texture->GetSurfaceWidth();
	texture_dimensions.y = (*FindTextureReferencerResult)->Texture->GetSurfaceHeight();
	return true;
}

bool CUnrealRmlRenderInterface::GenerateTexture(Rml::TextureHandle& texture_handle, const Rml::byte* source, const Rml::Vector2i& source_dimensions)
{
	UTexture2D* Texture = LoadTextureFromRaw(
		(const uint8*)source,
		FIntPoint(source_dimensions.x, source_dimensions.y));
	TextureReferencers.Add(MakeShared<FTextureReferencer>(Texture));
	texture_handle = reinterpret_cast<Rml::TextureHandle>(TextureReferencers.Last().Get());
	return true;
}

void CUnrealRmlRenderInterface::ReleaseTexture(Rml::TextureHandle texture)
{
	FTextureReferencer* TextureReferencer = reinterpret_cast<FTextureReferencer*>(texture);
	if (TextureReferencer->ResourcePath.IsEmpty())
		TextureReferencers.RemoveSwap(TextureReferencer->AsShared());
	else
	{
		TSharedPtr<FTextureReferencer> TextureReferencerSharedPtr = TextureReferencerMap.FindAndRemoveChecked(TextureReferencer->ResourcePath);
		TSharedPtr<FMediaTextureReferencer> MediaTextureReferencerSharedPtr = StaticCastSharedPtr<FMediaTextureReferencer>(TextureReferencerSharedPtr);
		if (MediaTextureReferencerSharedPtr) MediaTextureReferencerSet.Remove(MediaTextureReferencerSharedPtr);
	}
}

void CUnrealRmlRenderInterface::SetTransform(const Rml::Matrix4f* transform)
{
	bUseTransformMatrix = (transform != nullptr);
	if (bUseTransformMatrix)
	{
		for (size_t i = 0; i < 4; i++)
		{
			for (size_t j = 0; j < 4; j++)
			{
				TransformMatrix.M[i][j] = (reinterpret_cast<const Rml::Vector4f*>(transform->data())[i][j]);
			}
		}
	}
	else
	{
		TransformMatrix = FMatrix::Identity;
	}
}

UTexture2D* CUnrealRmlRenderInterface::LoadTextureFromRaw(const uint8* InSource, FIntPoint InSize, EPixelFormat InFormat)
{
	UTexture2D* Texture = UTexture2D::CreateTransient(InSize.X, InSize.Y, InFormat);
	Texture->UpdateResource();
	FUpdateTextureRegion2D* TextureRegion = new FUpdateTextureRegion2D(
		0,
		0,
		0,
		0,
		InSize.X,
		InSize.Y);

	int32 Size = InSize.X * InSize.Y * 4;
	uint8* Data = new uint8[Size];
	FMemory::Memcpy(Data, InSource, Size);

	auto DataCleanup = [](uint8* Data, const FUpdateTextureRegion2D* UpdateRegion)
	{
		delete Data;
		delete UpdateRegion;
	};
	Texture->UpdateTextureRegions(0, 1u, TextureRegion, 4 * InSize.X, 4, Data, DataCleanup);
	return Texture;
}

UTexture2D* CUnrealRmlRenderInterface::LoadTextureFromFile(const FString& InResourceFilePath)
{
	UTexture2D* LoadedTexture = nullptr;
	TArray64<uint8>* Data = new TArray64<uint8>();
	FString ExtensionName = FPaths::GetExtension(InResourceFilePath);
	if (ExtensionName.ToUpper() == "TGA")
	{
		int ImgX, ImgY, ImgComp;
		uint8* LoadedImageData = stbi_load(TCHAR_TO_ANSI(*InResourceFilePath), &ImgX, &ImgY, &ImgComp, 4);
		//TArray<uint8> RawData;
		//RawData.Append(LoadedImageData, ImgX * ImgY * GPixelFormats[PF_R8G8B8A8].BlockBytes);
		LoadedTexture = LoadTextureFromRaw(LoadedImageData, FIntPoint(ImgX, ImgY));
		stbi_image_free(LoadedImageData);
	}
	else
	{
		FFileHelper::LoadFileToArray(*Data, *InResourceFilePath);
		if (Data->Num() == 0)
			return nullptr;

		IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>("ImageWrapper");
		EImageFormat ImageFormat = ImageWrapperModule.DetectImageFormat(Data->GetData(), Data->Num());
		if (ImageFormat == EImageFormat::Invalid)
			return nullptr;

		FIntPoint Size;
		TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(ImageFormat);
		if (!ImageWrapper->SetCompressed(Data->GetData(), Data->Num()))
			return nullptr;

		Size.X = ImageWrapper->GetWidth();
		Size.Y = ImageWrapper->GetHeight();
		ImageWrapper->GetRaw(ERGBFormat::RGBA, 8, *Data);

		LoadedTexture = UTexture2D::CreateTransient(Size.X, Size.Y, EPixelFormat::PF_R8G8B8A8);
		LoadedTexture->UpdateResource();
		FUpdateTextureRegion2D* TextureRegion = new FUpdateTextureRegion2D(
			0,
			0,
			0,
			0,
			Size.X,
			Size.Y);

		auto DataCleanup = [FileData = Data](uint8* Data, const FUpdateTextureRegion2D* UpdateRegion)
		{
			delete FileData;
			delete UpdateRegion;
		};
		LoadedTexture->UpdateTextureRegions(0, 1u, TextureRegion, 4 * Size.X, 4, Data->GetData(), DataCleanup);
	}
	return LoadedTexture;
}

UTexture2D* CUnrealRmlRenderInterface::LoadTextureFromAsset(const FString& InAssetPath, UObject* InOuter)
{
	UObject* LoadedObject = StaticLoadObject(UTexture2D::StaticClass(), InOuter, *InAssetPath);
	return LoadedObject ? (UTexture2D*)LoadedObject : nullptr;
}

void CUnrealRmlRenderInterface::Tick(float DeltaTime)
{
	for (auto& MediaTextureReferencer : MediaTextureReferencerSet)
	{
		MediaTextureReferencer->Tick(DeltaTime);
	}
}

TStatId CUnrealRmlRenderInterface::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(CUnrealRmlRenderInterface, STATGROUP_Tickables);
}
