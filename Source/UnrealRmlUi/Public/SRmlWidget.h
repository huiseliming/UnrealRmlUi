// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RmlFwd.h"
#include "Widgets/SCompoundWidget.h"
#include "RmlTextInputMethodContext.h"

/**
 * 
 */
class UNREALRMLUI_API SRmlWidget : public SCompoundWidget
{
public:
	~SRmlWidget();

	SLATE_BEGIN_ARGS(SRmlWidget)
		: _bAutoloadFileChanges(false)
	{}
	SLATE_ATTRIBUTE(bool, bAutoloadFileChanges)
	SLATE_END_ARGS()


	TAttribute<bool> bAutoloadFileChanges;

public:
	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

	//
	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;
	virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;

	// 
	virtual FReply OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent) override;
	virtual FReply OnKeyUp(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent) override;
	virtual FReply OnKeyChar(const FGeometry& MyGeometry, const FCharacterEvent& InCharacterEvent) override;
	virtual FReply OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply OnMouseWheel(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FCursorReply OnCursorQuery(const FGeometry& MyGeometry, const FPointerEvent& CursorEvent) const override;
	virtual bool SupportsKeyboardFocus() const override { return true;}

	virtual FReply OnFocusReceived(const FGeometry & MyGeometry, const FFocusEvent & InFocusEvent) override;
	virtual void OnFocusLost(const FFocusEvent& InFocusEvent) override;
	static TSharedPtr<SRmlWidget> FocusedRmlWidget;

protected:
	Rml::Context* Context = nullptr;
	TArray<FString> DocumentFilePaths;

public:
	Rml::ElementDocument* LoadDocument(const FString& InDocumentPath, bool bUnloadOldDocuments = true);
	TArray<Rml::ElementDocument*> LoadDocuments(const TArray<FString>& InDocumentPaths, bool bUnloadOldDocuments = true);
	void ReloadDocuments();
	void SaveDocumentToFile(const FString& InFilePath);


	Rml::Context* GetContext() { return Context; }
	const TArray<FString>& GetDocumentFilePaths() { return DocumentFilePaths; }



	// IME
public:
	void ActivateIME();
	void DeactivateIME();

protected:
	TSharedPtr<FRmlTextInputMethodContext> TextInputMethodContext;
	TSharedPtr<ITextInputMethodChangeNotifier> TextInputMethodChangeNotifier;

};

