#include "RmlTextInputMethodContext.h"
#include "SRmlWidget.h"
#include "RmlInterface/UnrealRmlSystemInterface.h"
#include "RmlUi/Core/Elements/ElementFormControlTextArea.h"

void FRmlTextInputMethodContext::AbortComposition()
{
	bIsComposing = false;
	//Owner->InternalCefBrowser->GetHost()->ImeCancelComposition();
	ResetComposition();
}

void FRmlTextInputMethodContext::ResetComposition()
{
	CompositionString.Empty();
	CompositionBeginIndex = 0;
	CompositionLength = 0;
	SelectionRangeBeginIndex = 0;
	SelectionRangeLength = 0;
}

uint32 FRmlTextInputMethodContext::GetTextLength()
{
	return CompositionString.Len();
}

void FRmlTextInputMethodContext::GetSelectionRange(uint32& BeginIndex, uint32& Length, ECaretPosition& CaretPosition)
{
	BeginIndex = SelectionRangeBeginIndex;
	Length = SelectionRangeLength;
	CaretPosition = SelectionCaretPosition;
}

void FRmlTextInputMethodContext::SetSelectionRange(const uint32 BeginIndex, const uint32 Length, const ECaretPosition CaretPosition)
{
	SelectionRangeBeginIndex = BeginIndex;
	SelectionRangeLength = Length;
	SelectionCaretPosition = CaretPosition;
}

void FRmlTextInputMethodContext::GetTextInRange(const uint32 BeginIndex, const uint32 Length, FString& OutString)
{
	OutString = CompositionString.Mid(BeginIndex, Length);
}

void FRmlTextInputMethodContext::SetTextInRange(const uint32 BeginIndex, const uint32 Length, const FString& InString)
{
	FString NewString;
	if (BeginIndex > 0)
	{
		NewString = CompositionString.Mid(0, BeginIndex);
	}

	NewString += InString;

	if ((int32)(BeginIndex + Length) < CompositionString.Len())
	{
		NewString += CompositionString.Mid(BeginIndex + Length, CompositionString.Len() - (BeginIndex + Length));
	}
	CompositionString = NewString;
}

int32 FRmlTextInputMethodContext::GetCharacterIndexFromPoint(const FVector2D& Point)
{
	return INDEX_NONE;
}

bool FRmlTextInputMethodContext::GetTextBounds(const uint32 BeginIndex, const uint32 Length, FVector2D& Position, FVector2D& Size)
{
	TSharedPtr<SRmlWidget> LockedRmlWidget = RmlWidget.Pin();
	if (LockedRmlWidget.IsValid())
	{
		CUnrealRmlSystemInterface* SystemInterface = static_cast<CUnrealRmlSystemInterface*>(Rml::GetSystemInterface());
		FVector2D WindowSpacePosition = LockedRmlWidget->GetPaintSpaceGeometry().LocalToAbsolute(FVector2D(SystemInterface->CachedCaretPosition.x, SystemInterface->CachedCaretPosition.y));
		TSharedPtr<SWindow> ParentWindow = FSlateApplication::Get().FindWidgetWindow(LockedRmlWidget.ToSharedRef());
		Position = ParentWindow->GetPositionInScreen() + WindowSpacePosition;
	}
	return true;
}

void FRmlTextInputMethodContext::GetScreenBounds(FVector2D& Position, FVector2D& Size)
{
	//TSharedPtr<SRmlWidget> LockedRmlWidget = RmlWidget.Pin();
	//if (LockedRmlWidget.IsValid())
	//{
	//	Position = LockedRmlWidget->GetPaintSpaceGeometry().GetAbsolutePosition();
	//	Size = LockedRmlWidget->GetPaintSpaceGeometry().GetDrawSize();
	//}
}

TSharedPtr<FGenericWindow> FRmlTextInputMethodContext::GetWindow()
{
	TSharedPtr<SWindow> SlateWindow = CachedWindow.Pin();
	if (!SlateWindow.IsValid())
	{
		TSharedPtr<SRmlWidget> LockedRmlWidget = RmlWidget.Pin();
		if (!LockedRmlWidget.IsValid()) 
			return nullptr;
		SlateWindow = FSlateApplication::Get().FindWidgetWindow(LockedRmlWidget.ToSharedRef());
		if (!SlateWindow.IsValid()) 
			return nullptr;
		CachedWindow = SlateWindow;
	}
	return SlateWindow->GetNativeWindow();
}

void FRmlTextInputMethodContext::BeginComposition()
{
 	if (!bIsComposing)
	{
		bIsComposing = true;
	}
}

void FRmlTextInputMethodContext::UpdateCompositionRange(const int32 InBeginIndex, const uint32 InLength)
{
	CompositionBeginIndex = InBeginIndex;
	CompositionLength = InLength;
}

void FRmlTextInputMethodContext::EndComposition()
{
	if (bIsComposing)
	{
		bIsComposing = false;
		TSharedPtr<SRmlWidget> LockedRmlWidget = RmlWidget.Pin();
		if (LockedRmlWidget.IsValid())
		{
			if (!CompositionString.IsEmpty())
			{
				LockedRmlWidget->GetContext()->ProcessTextInput(TCHAR_TO_UTF8(*CompositionString));
			}
			ResetComposition();
		}
	}
}