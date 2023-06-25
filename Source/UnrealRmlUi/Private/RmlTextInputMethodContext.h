#pragma once

#include "CoreMinimal.h"
#include "GenericPlatform/ITextInputMethodSystem.h"

class SRmlWidget;

class FRmlTextInputMethodContext : public ITextInputMethodContext
{
public:
	FRmlTextInputMethodContext(const TSharedRef<SRmlWidget>& InRmlWidget)
		: RmlWidget(InRmlWidget)
	{
	}
	void AbortComposition();

private:
	void ResetComposition();

public:
	virtual bool IsComposing() override { return bIsComposing; }
	virtual bool IsReadOnly() override { return false; }
	virtual uint32 GetTextLength() override;
	virtual void GetSelectionRange(uint32& BeginIndex, uint32& Length, ECaretPosition& CaretPosition) override;
	virtual void SetSelectionRange(const uint32 BeginIndex, const uint32 Length, const ECaretPosition CaretPosition) override;
	virtual void GetTextInRange(const uint32 BeginIndex, const uint32 Length, FString& OutString) override;
	virtual void SetTextInRange(const uint32 BeginIndex, const uint32 Length, const FString& InString) override;
	virtual int32 GetCharacterIndexFromPoint(const FVector2D& Point) override;
	virtual bool GetTextBounds(const uint32 BeginIndex, const uint32 Length, FVector2D& Position, FVector2D& Size) override;
	virtual void GetScreenBounds(FVector2D& Position, FVector2D& Size) override;
	virtual TSharedPtr<FGenericWindow> GetWindow() override;
	virtual void BeginComposition() override;
	virtual void UpdateCompositionRange(const int32 InBeginIndex, const uint32 InLength) override;
	virtual void EndComposition() override;

private:
	TWeakPtr<SRmlWidget> RmlWidget;
	TWeakPtr<SWindow> CachedWindow;

	FGeometry CachedGeometry;
	bool bIsComposing = false;
	int32 CompositionBeginIndex = 0;
	uint32 CompositionLength = 0;

	uint32 SelectionRangeBeginIndex = 0;
	uint32 SelectionRangeLength = 0;

	ECaretPosition SelectionCaretPosition = ECaretPosition::Ending;
	FString CompositionString;
};
