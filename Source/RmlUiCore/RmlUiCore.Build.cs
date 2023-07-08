// Copyright Epic Games, Inc. All Rights Reserved.

using System;
using System.IO;
using System.Text;
using UnrealBuildTool;

public class RmlUiCore : ModuleRules
{	
	public string GetPluginDirectory()
	{
		string PluginDirectory = Directory.GetParent(ModuleDirectory).Parent.ToString();
		return PluginDirectory;
	}
	
	void GeneratePrivateSources(string InSourceDirectory, string OutUESourceDirectory)
	{
		DirectoryInfo SourceDirectoryInfo = new DirectoryInfo(InSourceDirectory);
		FileSystemInfo[] SourceFileSystemInfos = SourceDirectoryInfo.GetFileSystemInfos();
		foreach (FileSystemInfo SourceFileSystemInfo in SourceFileSystemInfos)
		{
			Console.WriteLine("CREATE SOURCE FILE : " + SourceFileSystemInfo.FullName);
			if (SourceFileSystemInfo is DirectoryInfo)
			{
				string UESourceDirectory = Path.Combine(OutUESourceDirectory, SourceFileSystemInfo.Name);
				if (!Directory.Exists(UESourceDirectory))
				{
					Directory.CreateDirectory(UESourceDirectory);
				}
				GeneratePrivateSources(SourceFileSystemInfo.FullName, UESourceDirectory);
			}
			else
			{
				if (SourceFileSystemInfo.Extension == ".c" || 
				    SourceFileSystemInfo.Extension == ".cc" ||
				    SourceFileSystemInfo.Extension == ".cpp" || 
				    SourceFileSystemInfo.Extension == ".cxx") 
				{
					string UESourceFile = Path.Combine(OutUESourceDirectory, SourceFileSystemInfo.Name);
					if (!File.Exists(UESourceFile))
					{
						//using (FileStream FStream = File.Create(UESourceFile))
						{
							string RmlSourceFileRelativePath = Path.GetRelativePath(OutUESourceDirectory, SourceFileSystemInfo.FullName);
							RmlSourceFileRelativePath.Replace("\\", "/");
							string SourceCode;
							if (SourceFileSystemInfo.Name == "SystemInterface.cpp")
							{
								SourceCode = 
									"#include \"CoreMinimal.h\"\n" + 
									"THIRD_PARTY_INCLUDES_START\n" + 
									"#if PLATFORM_WINDOWS\n" + 
									"#include \"Windows/AllowWindowsPlatformTypes.h\"\n" + 
									"#endif\n" + 
									"#include \"" + RmlSourceFileRelativePath + "\"\n" +  
									"#if PLATFORM_WINDOWS\n" + 
									"#include \"Windows/HideWindowsPlatformTypes.h\"\n" + 
									"#endif\n" + 
									"THIRD_PARTY_INCLUDES_END\n";
							}
							else
							{
								SourceCode = 
									"#include \"CoreMinimal.h\"\n" + 
									"THIRD_PARTY_INCLUDES_START\n" +
									"#define RML_CUSTOM_INCLUDE_WINDOWS_H\n" +
									"#include \"" + RmlSourceFileRelativePath + "\"\n" +
									"THIRD_PARTY_INCLUDES_END\n";
							}
							File.WriteAllText(UESourceFile, SourceCode);
						}
					}
				}
			}
		}
	}
	
	public RmlUiCore(ReadOnlyTargetRules Target) : base(Target)
	{
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
        bUseRTTI = true;
        bUseUnity = false;
        PublicSystemIncludePaths.Add(Path.Combine(ModuleDirectory, "..", "RmlUi", "Include"));
        PublicDependencyModuleNames.Add("Core");
        PrivateDependencyModuleNames.Add("UElibPNG");
        PrivateDependencyModuleNames.Add("zlib");
        PrivateDependencyModuleNames.Add("FreeType2");
        PublicDefinitions.Add("RMLUI_NO_THIRDPARTY_CONTAINERS");
        PublicDefinitions.Add("RMLUI_USE_CUSTOM_RTTI");
        if (!Directory.Exists(Path.Combine(ModuleDirectory, "Private", "Core")))
        {
	        Directory.CreateDirectory(Path.Combine(ModuleDirectory, "Private", "Core"));
	        Console.WriteLine("GENERATE PRIVATE SOURCES : " + Path.Combine(ModuleDirectory, "Private", "Core"));
	        GeneratePrivateSources(
		        Path.Combine(ModuleDirectory, "..", "RmlUi", "Source", "Core"), 
		        Path.Combine(ModuleDirectory, "Private", "Core"));
        }
    }
}
