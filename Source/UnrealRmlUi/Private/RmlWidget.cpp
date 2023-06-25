// Fill out your copyright notice in the Description page of Project Settings.


#include "RmlWidget.h"

void URmlWidget::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);
}

TSharedRef<SWidget> URmlWidget::RebuildWidget()
{
	TSharedPtr<SRmlWidget> RmlWidgetPtr =  RmlWidget.Pin();
	if (!RmlWidgetPtr.IsValid())
		RmlWidgetPtr = SAssignNew(RmlWidget, SRmlWidget);
	if (!DocumentFilePath.IsEmpty())
		RmlWidgetPtr->LoadDocument(DocumentFilePath);
	return RmlWidgetPtr.ToSharedRef();
}

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
