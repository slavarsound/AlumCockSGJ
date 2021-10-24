// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class AlumCockSGJ : ModuleRules
{
	public AlumCockSGJ(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay" });

		PublicIncludePaths.AddRange(new string[] { "AlumCockSGJ/Characters", "AlumCockSGJ/Controllers", "AlumCockSGJ/Components" });
	}
}
