// Copyright Epic Games, Inc. All Rights Reserved.

#include "UnrealRmlUi.h"

#include "Interfaces/IPluginManager.h"
#include "Misc/Paths.h"

#include "SRmlWidget.h"

#define LOCTEXT_NAMESPACE "FUnrealRmlUiModule"

void CopyAssetsToExternal(TArray<TFunction<void()>>& PlatformFileTasks, FString InDirectory)
{
#if PLATFORM_ANDROID
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	IFileManager* FileManager = &IFileManager::Get();
	class FVisitor : public IPlatformFile::FDirectoryVisitor
	{
	public:
 		FVisitor(const TFunction<bool (const TCHAR* FilenameOrDirectory, bool bIsDirectory)>& InVisitor) : Visitor(InVisitor) {}
 		virtual bool Visit(const TCHAR* FilenameOrDirectory, bool bIsDirectory) { return Visitor(FilenameOrDirectory, bIsDirectory); }
 		TFunction<bool (const TCHAR* FilenameOrDirectory, bool bIsDirectory)> Visitor;
	};
	UE_LOG(LogTemp, Warning, TEXT("IterateDirectory: %s"), *InDirectory);
	FileManager->IterateDirectoryRecursively(
 		*InDirectory,
 		[=, &PlatformFile, &PlatformFileTasks] (const TCHAR* FilenameOrDirectory, bool bIsDirectory) mutable
 		{
			UE_LOG(LogTemp, Warning, TEXT("FilenameOrDirectory: %s"), FilenameOrDirectory);
			//UE_LOG(LogTemp, Warning, TEXT("FPaths::ConvertRelativePathToFull(*BaseDir): %s"), *FPaths::ConvertRelativePathToFull(*InDirectory));
			//UE_LOG(LogTemp, Warning, TEXT("FPaths::ConvertRelativePathToFull(*InDirectory): %s"), *FPaths::ConvertRelativePathToFull(*InDirectory));
			//UE_LOG(LogTemp, Warning, TEXT("PlatformFile.ConvertToAbsolutePathForExternalAppForRead(*InDirectory): %s"), *PlatformFile.ConvertToAbsolutePathForExternalAppForRead(*InDirectory));
			//UE_LOG(LogTemp, Warning, TEXT("PlatformFile.ConvertToAbsolutePathForExternalAppForWrite(*InDirectory): %s"), *PlatformFile.ConvertToAbsolutePathForExternalAppForWrite(*InDirectory));
			//UE_LOG(LogTemp, Warning, TEXT("PlatformFile.ConvertToAbsolutePathForExternalAppForWrite(FilenameOrDirectory): %s"), *PlatformFile.ConvertToAbsolutePathForExternalAppForWrite(FilenameOrDirectory));
 			if (bIsDirectory)
 			{
				UE_LOG(LogTemp, Warning, TEXT("Directory Task Add: %s"), FilenameOrDirectory);
				PlatformFileTasks.Add([Directory  = PlatformFile.ConvertToAbsolutePathForExternalAppForWrite(FilenameOrDirectory)] {
					UE_LOG(LogTemp, Warning, TEXT("Directory: %s"),*Directory);
					FPlatformFileManager::Get().GetPlatformFile().CreateDirectoryTree(*Directory);; 
				});

				//CopyAssetsToExternal(InDirectory / FPaths::GetCleanFilename(FilenameOrDirectory));
			}
 			else
 			{
				bool bWriteFileToExternalAbsolutePath = true;
				FString ExternalAbsolutePath = PlatformFile.ConvertToAbsolutePathForExternalAppForWrite(FilenameOrDirectory);
				if (PlatformFile.FileExists(*ExternalAbsolutePath))
				{
					if (PlatformFile.GetTimeStamp(FilenameOrDirectory) > PlatformFile.GetTimeStamp(*ExternalAbsolutePath))
					{
						bWriteFileToExternalAbsolutePath = true;
					}
				}
				else
				{
					bWriteFileToExternalAbsolutePath = true;
				}
				if (bWriteFileToExternalAbsolutePath)
				{
					UE_LOG(LogTemp, Warning, TEXT("Filename Task Add: %s"), FilenameOrDirectory);
					PlatformFileTasks.Add([Filename = FString(FilenameOrDirectory), ExternalAbsolutePath] {
						UE_LOG(LogTemp, Warning, TEXT("Filename: %s"), *Filename);
						UE_LOG(LogTemp, Warning, TEXT("ExternalAbsolutePath: %s"), *ExternalAbsolutePath);
						TArray<uint8> TempBytes;
						if (FFileHelper::LoadFileToArray(TempBytes, *Filename, 0))
						{
							//FString DestFilename = GExternalFilePath / FPaths::GetCleanFilename(*SourceFilename);
							FFileHelper::SaveArrayToFile(TempBytes, *ExternalAbsolutePath);
						}
					});
				}
 			}
 			return true;
 		});
#endif
}


void FUnrealRmlUiModule::StartupModule()
{
	FString PluginBaseDir = IPluginManager::Get().FindPlugin("UnrealRmlUi")->GetBaseDir();
	UE_LOG(LogTemp, Warning, TEXT("PluginBaseDir: %s"), *PluginBaseDir);
	TArray<TFunction<void()>> PlatformFileTasks;
	CopyAssetsToExternal(PlatformFileTasks, PluginBaseDir / TEXT("Resources") / TEXT("Rml"));
	UE_LOG(LogTemp, Warning, TEXT("PlatformFileTask BG"));
	for (auto& PlatformFileTask : PlatformFileTasks)
	{
		UE_LOG(LogTemp, Warning, TEXT("PlatformFileTask EXEC"));
		PlatformFileTask();
	}
	UE_LOG(LogTemp, Warning, TEXT("PlatformFileTask ED"));

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
				UE_LOG(LogTemp, Fatal, TEXT("FATAL > Rml::LoadFontFace(\"%s\", %s)"), UTF8_TO_TCHAR(file_path.c_str()), fallback_face ? TEXT("true") : TEXT("false"));
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("ERROR > Rml::LoadFontFace(\"%s\", %s)"), UTF8_TO_TCHAR(file_path.c_str()), fallback_face ? TEXT("true") : TEXT("false"));
			}
		}
	};
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	// TODO: IMPL RML FILE INTERFACE FOR ANDROID LOAD FILE 
	// load 'Consolas' font
	LoadFontFace(TCHAR_TO_UTF8(*PlatformFile.ConvertToAbsolutePathForExternalAppForWrite(*(PluginBaseDir / TEXT("Resources") / TEXT("Rml") / TEXT("Fonts") / TEXT("consola.ttf")))), false);
	LoadFontFace(TCHAR_TO_UTF8(*PlatformFile.ConvertToAbsolutePathForExternalAppForWrite(*(PluginBaseDir / TEXT("Resources") / TEXT("Rml") / TEXT("Fonts") / TEXT("consolab.ttf")))), false);
	LoadFontFace(TCHAR_TO_UTF8(*PlatformFile.ConvertToAbsolutePathForExternalAppForWrite(*(PluginBaseDir / TEXT("Resources") / TEXT("Rml") / TEXT("Fonts") / TEXT("consolaz.ttf")))), false);
	LoadFontFace(TCHAR_TO_UTF8(*PlatformFile.ConvertToAbsolutePathForExternalAppForWrite(*(PluginBaseDir / TEXT("Resources") / TEXT("Rml") / TEXT("Fonts") / TEXT("consolai.ttf")))), false);
	LoadFontFace(TCHAR_TO_UTF8(*PlatformFile.ConvertToAbsolutePathForExternalAppForWrite(*(PluginBaseDir / TEXT("Resources") / TEXT("Rml") / TEXT("Fonts") / TEXT("simsun.ttc")))), true);

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