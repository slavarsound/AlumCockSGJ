// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class AlumCockSGJ : ModuleRules
{
	public AlumCockSGJ(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "Niagara", "UMG", 
			"NavigationSystem",
			"GameplayTasks",
			"OnlineSubsystem",
			"OnlineSubsystemUtils"
		});

		DynamicallyLoadedModuleNames.Add("OnlineSubsystemSteam");

		PrivateDependencyModuleNames.AddRange(new string[] {  });

		PrivateIncludePaths.AddRange(new []{ Name });
	}
}
