// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SRmlWidget.h"
#include "Components/Widget.h"
#include "RmlWidget.generated.h"

/**
 * 
 */
UCLASS()
class UNREALRMLUI_API URmlWidget : public UWidget
{
	GENERATED_BODY()
public:

public:
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;
	
protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual TSharedRef<SWidget> RebuildDesignWidget(TSharedRef<SWidget> Content) override;

protected:
	TWeakPtr<SRmlWidget> RmlWidget;
	UFUNCTION(CallInEditor, Category = RmlWidget)
	void ReloadDocuments();
	UFUNCTION(CallInEditor, Category = RmlWidget)
	void SaveDocumentToFile();

	UPROPERTY(EditAnywhere, Category = RmlWidget)
	FString DocumentFilePath;
};
