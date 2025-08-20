// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ShimuraDuel : ModuleRules
{
	public ShimuraDuel(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput" });
	}
}
