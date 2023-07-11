// Fill out your copyright notice in the Description page of Project Settings.


#include "RmlWidget.h"
#include "Interfaces/IPluginManager.h"

void URmlWidget::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);
}

TSharedRef<SWidget> URmlWidget::RebuildWidget()
{
	FString PluginBaseDir = IPluginManager::Get().FindPlugin("UnrealRmlUi")->GetBaseDir();
	DocumentFilePath = FPlatformFileManager::Get().GetPlatformFile().ConvertToAbsolutePathForExternalAppForWrite(*(PluginBaseDir / TEXT("Resources") / TEXT("Rml") / TEXT("hello_world.html")));
	UE_LOG(LogTemp, Warning, TEXT("DocumentFilePath: %s"), *DocumentFilePath);
	TSharedPtr<SRmlWidget> RmlWidgetPtr =  RmlWidget.Pin();
	if (!RmlWidgetPtr.IsValid())
		RmlWidgetPtr = SAssignNew(RmlWidget, SRmlWidget);
	if (!DocumentFilePath.IsEmpty())
		RmlWidgetPtr->LoadDocument(DocumentFilePath);
	return RmlWidgetPtr.ToSharedRef();
}

#if WITH_EDITOR
TSharedRef<SWidget> URmlWidget::RebuildDesignWidget(TSharedRef<SWidget> Content)
{
	TSharedRef<SRmlWidget> RmlWidgetRef = StaticCastSharedRef<SRmlWidget>(Content);
	auto& DocumentFilePaths = RmlWidgetRef->GetDocumentFilePaths();
	if (DocumentFilePaths.Num() == 1)
	{
		if (DocumentFilePaths[0].IsEmpty() && DocumentFilePath.IsEmpty())
		{
			return Content;
		}
		if (DocumentFilePaths[0] == DocumentFilePath)
		{
			return Content;
		}
	}
	RmlWidgetRef->LoadDocument(DocumentFilePath);
	return Content;
}
#endif

void URmlWidget::ReloadDocuments()
{
	if (TSharedPtr<SRmlWidget> RmlWidgetPtr = RmlWidget.Pin())
	{
		RmlWidgetPtr->ReloadDocuments();
	}
}

void URmlWidget::SaveDocumentToFile()
{
	if (TSharedPtr<SRmlWidget> RmlWidgetPtr = RmlWidget.Pin())
	{
		RmlWidgetPtr->SaveDocumentToFile("");
	}
}
