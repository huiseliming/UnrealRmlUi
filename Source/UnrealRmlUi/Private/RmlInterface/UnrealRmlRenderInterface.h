#pragma once

#include "CoreMinimal.h"
#include "IMediaEventSink.h"
#include "MediaSoundComponent.h"
#include "MediaTexture.h"
#include "RmlFwd.h"

class SRmlWidget;

class FTextureReferencer : public TSharedFromThis<FTextureReferencer>, public FGCObject
{
public:
	FTextureReferencer(UTexture* InTexture, FString InResourcePath = FString());
	virtual ~FTextureReferencer();
	const FSlateResourceHandle& GetResourceHandle() { return CachedResourceHandle; }
protected:
	FString ResourcePath;
	FSlateBrush Brush;
	FSlateResourceHandle CachedResourceHandle;
	UTexture* Texture;
	friend class CUnrealRmlRenderInterface;
protected:
	virtual FString GetReferencerName() const override { return TEXT("FSlateResourceReferencer"); }
	virtual void AddReferencedObjects(FReferenceCollector& InCollector) override
	{
		InCollector.AddReferencedObject(Texture);
	}
};

class FMediaTextureReferencer : public FTextureReferencer
{
public:
	FMediaTextureReferencer(UMediaTexture* InMediaTexture, FString InResourcePath = FString());
	virtual ~FMediaTextureReferencer();
	void OnMediaEvent(EMediaEvent Event);
	void Tick(float DeltaTime);
public:
	TWeakObjectPtr<UMediaPlayer> WeakPlayerPtr;
	UMediaSoundComponent* SoundComponent;
	UMaterialInstanceDynamic* Material;
protected:
	virtual FString GetReferencerName() const override { return TEXT("FSlateResourceReferencer"); }
	virtual void AddReferencedObjects(FReferenceCollector& InCollector) override
	{
		FTextureReferencer::AddReferencedObjects(InCollector);
		//InCollector.AddReferencedObject(Player);
		InCollector.AddReferencedObject(SoundComponent);
		InCollector.AddReferencedObject(Material);
	}
};

class CUnrealRmlRenderInterface : public Rml::RenderInterface, public FTickableGameObject
{
public:
	void BeginRender(
		//SRmlWidget* InRmlWidget,
		//const FPaintArgs& Args,
		const FGeometry& _AllottedGeometry,
		//const FSlateRect& _MyCullingRect,
		FSlateWindowElementList& _OutDrawElements,
		int32 _LayerId
		//const FWidgetStyle& InWidgetStyle,
		//bool bParentEnabled
	)
	{
		//RmlWidget = InRmlWidget;
		//this->Args = &Args;
		this->AllottedGeometry = &_AllottedGeometry;
		//this->MyCullingRect = &_MyCullingRect;
		this->OutDrawElements = &_OutDrawElements;
		this->LayerId = _LayerId;
		//this->InWidgetStyle = &InWidgetStyle;
		//this->bParentEnabled = bParentEnabled;
	}
	void EndRender()
	{
		//RmlWidget = nullptr;
	}

public:
	virtual void RenderGeometry(Rml::Vertex* vertices, int num_vertices, int* indices, int num_indices, Rml::TextureHandle texture, const Rml::Vector2f& translation) override;
	//virtual Rml::CompiledGeometryHandle CompileGeometry(Rml::Vertex* vertices, int num_vertices, int* indices, int num_indices, Rml::TextureHandle texture);
	//virtual void RenderCompiledGeometry(Rml::CompiledGeometryHandle geometry, const Rml::Vector2f& translation);
	//virtual void ReleaseCompiledGeometry(Rml::CompiledGeometryHandle geometry);
	virtual void EnableScissorRegion(bool enable) override;
	virtual void SetScissorRegion(int x, int y, int width, int height) override;
	virtual bool LoadTexture(Rml::TextureHandle& texture_handle, Rml::Vector2i& texture_dimensions, const Rml::String& source) override;
	virtual bool GenerateTexture(Rml::TextureHandle& texture_handle, const Rml::byte* source, const Rml::Vector2i& source_dimensions) override;
	virtual void ReleaseTexture(Rml::TextureHandle texture) override;
	virtual void SetTransform(const Rml::Matrix4f* transform) override;

	//SRmlWidget* RmlWidget;
	//const FPaintArgs*        Args;
	const FGeometry*         AllottedGeometry;
	//const FSlateRect*        MyCullingRect;
	FSlateWindowElementList* OutDrawElements;
	int32                    LayerId;
	//const FWidgetStyle*      InWidgetStyle;
	//bool                     bParentEnabled;

	bool bUseTransformMatrix;
	FMatrix TransformMatrix;
	bool bUseClipRect;
	FSlateRect ClipRect;

	TMap<FString, TSharedPtr<FTextureReferencer>> TextureReferencerMap;
	TSet<TSharedPtr<FMediaTextureReferencer>> MediaTextureReferencerSet;
	TArray<TSharedPtr<FTextureReferencer>> TextureReferencers;

	// load texture function
	static UTexture2D* LoadTextureFromRaw(const uint8* InSource, FIntPoint InSize, EPixelFormat InFormat = EPixelFormat::PF_R8G8B8A8);
	static UTexture2D* LoadTextureFromFile(const FString& InResourceFilePath);
	static UTexture2D* LoadTextureFromAsset(const FString& InAssetPath, UObject* InOuter = nullptr);

protected:
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;
	virtual bool IsTickableWhenPaused() const override { return true; }
	virtual bool IsTickableInEditor() const override { return true; }

};

