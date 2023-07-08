// Copyright Epic Games, Inc. All Rights Reserved.

#include "UnrealRmlUi.h"

#include "Interfaces/IPluginManager.h"
#include "Misc/Paths.h"

#include "SRmlWidget.h"

#define LOCTEXT_NAMESPACE "FUnrealRmlUiModule"

void FUnrealRmlUiModule::StartupModule()
{
	FString BaseDir = IPluginManager::Get().FindPlugin("UnrealRmlUi")->GetBaseDir();

	UnrealRmlSystemInterface = MakeUnique<CUnrealRmlSystemInterface>();
	UnrealRmlRenderInterface = MakeUnique<CUnrealRmlRenderInterface>();
	Rml::SetSystemInterface(UnrealRmlSystemInterface.Get());
	Rml::SetRenderInterface(UnrealRmlRenderInterface.Get());

	Rml::Initialise();

	auto LoadFontFace = [](const Rml::String& file_path, bool fallback_face = false)
	{
		if (!Rml::LoadFontFace(file_path, fallback_face))
		{
			if (fallback_face)
			{
				UE_LOG(LogTemp, Fatal, TEXT("FATAL > Rml::LoadFontFace(\"%s\", %s)"), UTF8_TO_TCHAR(file_path.c_str()), fallback_face ? "true" : "false");
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("ERROR > Rml::LoadFontFace(\"%s\", %s)"), UTF8_TO_TCHAR(file_path.c_str()), fallback_face ? "true" : "false");
			}
		}
	};
	// TODO: IMPL RML FILE INTERFACE FOR ANDROID LOAD FILE 
	// load 'Consolas' font
	LoadFontFace(TCHAR_TO_UTF8(*FPaths::Combine(FPaths::ProjectContentDir(), TEXT("Fonts"), TEXT("consola.ttf"))), false);
	LoadFontFace(TCHAR_TO_UTF8(*FPaths::Combine(FPaths::ProjectContentDir(), TEXT("Fonts"), TEXT("consolab.ttf"))), false);
	LoadFontFace(TCHAR_TO_UTF8(*FPaths::Combine(FPaths::ProjectContentDir(), TEXT("Fonts"), TEXT("consolaz.ttf"))), false);
	LoadFontFace(TCHAR_TO_UTF8(*FPaths::Combine(FPaths::ProjectContentDir(), TEXT("Fonts"), TEXT("consolai.ttf"))), false);

	LoadFontFace(TCHAR_TO_UTF8(*FPaths::Combine(FPaths::ProjectContentDir(), TEXT("Fonts"), TEXT("simsun.ttc"))), true);

	////load 'Microsoft YaHei' font
	//LoadFontFace("C:\\Windows\\Fonts\\msyhbd.ttc", false);
	//LoadFontFace("C:\\Windows\\Fonts\\msyhl.ttc", false);
	//LoadFontFace("C:\\Windows\\Fonts\\msyh.ttc", true);

#define TEST 1
#if TEST
	//static struct ApplicationData {
	//	bool show_text = true;
	//	Rml::String animal = "dog";
	//} my_data;
	//{
	//	TSharedRef<SWindow> Window = SNew(SWindow)
	//		.ClientSize(FVector2D(1600, 900));
	//	FSlateApplication::Get().AddWindow(Window);
	//	Window->ShowWindow();
	//	TSharedRef<SRmlWidget> RmlWidget = SNew(SRmlWidget);
	//	Window->SetContent(RmlWidget);

	//	if (Rml::DataModelConstructor constructor = RmlWidget->GetContext()->CreateDataModel("animals"))
	//	{
	//		constructor.Bind("show_text", &my_data.show_text);
	//		constructor.Bind("animal", &my_data.animal);
	//	}
	//	FString DocumentPath = FPaths::ConvertRelativePathToFull(FPaths::Combine(BaseDir, TEXT("Content"), TEXT("HelloWorld"), TEXT("hello_world.rml")));
	//	RmlWidget->LoadDocument(DocumentPath);
	//	//Rml::Element* element = Document->GetElementById("world");
	//	//element->SetInnerRML(reinterpret_cast<const char*>(u8"🌍"));
	//	//element->SetProperty("font-size", "1.5em");
	//}
	//{
	//	TSharedRef<SWindow> Window = SNew(SWindow)
	//		.ClientSize(FVector2D(1600, 900));
	//	FSlateApplication::Get().AddWindow(Window);
	//	Window->ShowWindow();
	//	TSharedRef<SRmlWidget> RmlWidget = SNew(SRmlWidget);
	//	Window->SetContent(RmlWidget);
	//	Rml::ElementDocument* Document = RmlWidget->LoadDocument(FPaths::Combine(TEXT("tutorial"), TEXT("template"), TEXT("data"), TEXT("tutorial.rml")));
	//}
#endif // TEST

}

void FUnrealRmlUiModule::ShutdownModule()
{
	Rml::Shutdown();
	UnrealRmlRenderInterface.Reset();
	UnrealRmlSystemInterface.Reset();
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FUnrealRmlUiModule, UnrealRmlUi)