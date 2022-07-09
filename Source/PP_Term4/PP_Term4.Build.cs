// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class PP_Term4 : ModuleRules
{
    public PP_Term4(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "UMG" });
    }
}
