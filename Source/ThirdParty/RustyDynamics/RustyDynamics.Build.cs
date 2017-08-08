// Fill out your copyright notice in the Description page of Project Settings.

using System.IO;
using UnrealBuildTool;

public class RustyDynamics : ModuleRules
{
	public RustyDynamics(ReadOnlyTargetRules Target) : base(Target)
	{
		Type = ModuleType.External;

		if (Target.Platform == UnrealTargetPlatform.Win64)
		{
            PublicLibraryPaths.Add(Path.Combine(ModuleDirectory, "target", "Debug", "deps"));
            PublicAdditionalLibraries.Add("RustyDynamics.dll.lib");
            PublicDelayLoadDLLs.Add("RustyDynamics.dll");
        }
        else if (Target.Platform == UnrealTargetPlatform.Mac)
        {
            PublicDelayLoadDLLs.Add(Path.Combine(ModuleDirectory, "target", "Debug", "libRustyDynamics.dylib"));
        }
	}
}
