﻿#pragma once
#include "RmlUiFwd.h"
#include "RmlTextureEntry.h"

class FRmlDrawer;

class UNREALRMLUI_API FRmlSystemInterface : public Rml::SystemInterface
{
	using Super = Rml::SystemInterface;
public:
	FRmlSystemInterface();

	EMouseCursor::Type CachedCursorState() const { return CachedCursor; }
protected:
	// ~Begin Rml::SystemInterface API 
	virtual double GetElapsedTime() override;
	// virtual int TranslateString(Rml::String& translated, const Rml::String& input) override;
	virtual void JoinPath(Rml::String& translated_path, const Rml::String& document_path, const Rml::String& path) override;
	virtual bool LogMessage(Rml::Log::Type type, const Rml::String& message) override;
	virtual void SetMouseCursor(const Rml::String& cursor_name) override;
	virtual void SetClipboardText(const Rml::String& text) override;
	virtual void GetClipboardText(Rml::String& text) override;
	virtual void ActivateKeyboard(Rml::Vector2f caret_position, float line_height) override;
	virtual void DeactivateKeyboard() override;
	// ~End Rml::SystemInterface API
	private:
	EMouseCursor::Type	CachedCursor;
};
