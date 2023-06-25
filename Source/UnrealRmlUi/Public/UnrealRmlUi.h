// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "RmlInterface/UnrealRmlRenderInterface.h"
#include "RmlInterface/UnrealRmlSystemInterface.h"


class FUnrealRmlUiModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	TUniquePtr<CUnrealRmlSystemInterface> UnrealRmlSystemInterface;
	TUniquePtr<CUnrealRmlRenderInterface> UnrealRmlRenderInterface;

	//void* FreeTypeDllHandle = nullptr;
	//void* RmlCoreDllHandle = nullptr;
	//void* RmlDebuggerDllHandle = nullptr;
};

