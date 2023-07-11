// Copyright Epic Games, Inc. All Rights Reserved.

using System;
using System.IO;
using UnrealBuildTool;

public class UnrealRmlUi : ModuleRules
{
	void AddRmlRuntimeDependencies(string InRmlSearchDirectory)
	{
		DirectoryInfo RmlSearchDirectoryInfo = new DirectoryInfo(InRmlSearchDirectory);
		FileSystemInfo[] RmlFileSystemInfos = RmlSearchDirectoryInfo.GetFileSystemInfos();
		foreach (FileSystemInfo RmlFileSystemInfo in RmlFileSystemInfos)
		{
			Console.WriteLine("AddRmlRuntimeDependencies : " + RmlFileSystemInfo.FullName);
			if (RmlFileSystemInfo is DirectoryInfo)
			{
				AddRmlRuntimeDependencies(RmlFileSystemInfo.FullName);
			}
			else
			{
				RuntimeDependencies.Add(RmlFileSystemInfo.FullName);
			}
		}
	}
	
	public UnrealRmlUi(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		bUseUnity = false;
		
        PublicSystemIncludePaths.Add(Path.Combine(ModuleDirectory, "..", "RmlUi", "Include"));

        PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"RmlUiCore",
                "RmlUiDebugger",
				// ... add other public dependencies that you statically link with here ...
			}
            );
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
                "InputCore",
                "ApplicationCore",
                "UMG",
				"Projects", 
				"MediaAssets",
                "AudioMixer",
                "TinyXML2",
				// ... add private dependencies that you statically link with here ...	
			}
            );
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
		AddRmlRuntimeDependencies(Path.Combine(ModuleDirectory, "..", "..", "Resources", "Rml"));
	}
}
