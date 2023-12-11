// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class RTS_Game : ModuleRules
{
	public RTS_Game(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "NavigationSystem", "AIModule", "Niagara", "EnhancedInput" , "GameplayTasks", "Foliage"});
    }
}
