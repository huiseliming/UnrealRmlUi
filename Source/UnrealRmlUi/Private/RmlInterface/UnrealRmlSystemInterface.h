#pragma once

#include "CoreMinimal.h"
#include "RmlFwd.h"

class CUnrealRmlSystemInterface : public Rml::SystemInterface
{
public:
	CUnrealRmlSystemInterface();
	EMouseCursor::Type GetCachedMouseCursorType() const { return CachedMouseCursorType; }

protected:
	virtual double GetElapsedTime() override { return FSlateApplication::Get().GetCurrentTime(); }
	virtual bool LogMessage(Rml::Log::Type type, const Rml::String& message) override;
	virtual void SetMouseCursor(const Rml::String& cursor_name) override;
	virtual void SetClipboardText(const Rml::String& text) override;
	virtual void GetClipboardText(Rml::String& text) override;
	virtual void ActivateKeyboard(Rml::Vector2f caret_position, float line_height) override;
	virtual void DeactivateKeyboard() override;
	friend class FRmlTextInputMethodContext;

protected:
	EMouseCursor::Type	CachedMouseCursorType = EMouseCursor::Default;

public:
	Rml::Vector2f CachedCaretPosition;
	float CachedLineHeight;
};





