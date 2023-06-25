// Copyright Epic Games, Inc. All Rights Reserved.

using System;
using System.IO;
using UnrealBuildTool;

public class RmlUiDebugger : ModuleRules
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
							string SourceCode = 
								"#include \"CoreMinimal.h\"\n" + 
								"THIRD_PARTY_INCLUDES_START\n" +
								"#include \"" + RmlSourceFileRelativePath + "\"\n" +
								"THIRD_PARTY_INCLUDES_END\n";
							File.WriteAllText(UESourceFile, SourceCode);
						}
					}
				}
			}
		}
	}
	
	public RmlUiDebugger(ReadOnlyTargetRules Target) : base(Target)
	{
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
        bUseRTTI = true;
        bUseUnity = false;
        PublicSystemIncludePaths.Add(Path.Combine(ModuleDirectory, "..", "RmlUi", "Include"));
        PublicDependencyModuleNames.Add("Core");
        PublicDependencyModuleNames.Add("RmlUiCore");
        if (!Directory.Exists(Path.Combine(ModuleDirectory, "Private", "Debugger")))
        {
		    Directory.CreateDirectory(Path.Combine(ModuleDirectory, "Private", "Debugger"));
	        Console.WriteLine("GENERATE PRIVATE SOURCES : " + Path.Combine(ModuleDirectory, "Private", "Debugger"));
	        GeneratePrivateSources(
		        Path.Combine(ModuleDirectory, "..", "RmlUi", "Source", "Debugger"), 
		        Path.Combine(ModuleDirectory, "Private", "Debugger"));
        }
    }

}

