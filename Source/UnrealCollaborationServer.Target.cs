// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class UnrealCollaborationServerTarget : TargetRules {
	public UnrealCollaborationServerTarget(TargetInfo Target) : base(Target) {
		Type = TargetType.Server;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		ExtraModuleNames.Add("UnrealCollaboration");
	}
}
