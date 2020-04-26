// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class VRExhibit : ModuleRules {
	public VRExhibit(ReadOnlyTargetRules Target) : base(Target) {
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		// List of packages we use
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "Http", "Json", "JsonUtilities" });
	}
}
