# Script to allow us to merge with another prroject
# Automates https://www.ue4community.wiki/Legacy/Migrate_content_between_projects
import argparse, sys, os
from distutils.dir_util import copy_tree
from pathlib import Path

# Function for argsparse
def dir_path(path):
    # Check the path is valid
    if not os.path.isdir(path):
        raise NotADirectoryError(path)
    
    # Return if file endsin .uproject -> unreal project
    for file in os.listdir(path):
        if file.endswith(".uproject"):
            return path

    # raise if not returned
    raise argparse.ArgumentTypeError("{} doesn\'t contain .uproject file".format(path))

# Find the name of the project
def find_name(path):
    for file in os.listdir(os.path.join(path, "Source")):
        if file.endswith(".Target.cs"):
            return file[:-len(".Target.cs")]

# Function to write a line
def write_action(file, name, key):
    f.write("+ActionMappings=(ActionName=\"" + name + "\",bShift=False,bCtrl=False,bAlt=False,bCmd=False,Key=" + key + ")\n")

def write_axis(file, name, scale, key):
    f.write("+AxisMapping=(AxisName=\"" + name + "\",Scale=" + str(scale) + ".000000,Key=" + key + ")\n")

 # Main program
if __name__ == "__main__":
    # Setup parser
    parser = argparse.ArgumentParser(description="Merge this project with another")
    parser.add_argument('path', type=dir_path, help="Path to the directory to merge. Must be contain .uproject")
    parser.add_argument('--add-inputs', dest='inputs', action='store_true', help="Include default input bindings")
    parser.add_argument('--no-add-inputs', dest='inputs', action='store_false', help="Include default input bindings")
    parser.set_defaults(inputs=True)

    # Get arguments
    args = parser.parse_args()

    # Start by getting the name of their project
    name = find_name(args.path)

    # Inside of Source there should be Project.Target.cs
    with open(os.path.join(args.path, "Source/" + name + "Server.Target.cs"), "w") as f:
        f.write("// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.\n\n")
        f.write("using UnrealBuildTool;\nusing System.Collections.Generic;\n\n")
        f.write("public class " + name + "ServerTarget : TargetRules {\n")
        f.write("    public " + name + "ServerTarget(TargetInfo Target) : base(Target) {\n")
        f.write("        Type = TargetType.Server;\n")
        f.write("        DefaultBuildSettings = BuildSettingsVersion.V2;\n")
        f.write("        ExtraModuleNames.Add(\"" + name + "\");\n")
        f.write("    }\n")
        f.write("}\n")

    # Copy files from UnrealCollaboration -> Project
    copy_tree("Source/UnrealCollaboration", os.path.join(args.path, "Source", name))

    # Replace instances of UNREALCOLLABORATION_API to PROJECT_API
    for file in list(Path(os.path.join(args.path, "Source", name)).rglob("*.[hH]")):
        with open(file, "r") as read:
            read_data = read.read()

        new_data = read_data.replace("UNREALCOLLABORATION_API", name.upper() + "_API")

        with open(file, "w") as write:
            write.write(new_data)

    # Input the things we need to into Config
    # DefaultEngine -> [HTTP] & [CoreRedirects]
    with open(os.path.join(args.path, "Config/DefaultEngine.ini"), "a") as f:
        # Write redirects
        f.write("\n[CoreRedirects]\n")
        f.write("+ClassRedirects=(MatchSubstring=True,OldName=\"/UnrealCollaboration\",NewName=\"/" + name + "\")\n")
        f.write("+EnumRedirects=(MatchSubstring=True,OldName=\"/UnrealCollaboration\",NewName=\"/" + name + "\")\n")
        f.write("+FunctionRedirects=(MatchSubstring=True,OldName=\"/UnrealCollaboration\",NewName=\"/" + name + "\")\n")
        f.write("+StructRedirects=(MatchSubstring=True,OldName=\"/UnrealCollaboration\",NewName=\"/" + name + "\")\n")

        # Write HTTP
        f.write("\n[HTTP]\n")
        f.write("HttpTimeout=300\n")
        f.write("HttpConnectionTimeout=-1\n")
        f.write("HttpReceiveTimeout=-1\n")
        f.write("HttpSendTimeout=-1\n")
        f.write("HttpMaxConnectionsPerServer=16\nbEnableHttp=true\nbUseNullHttp=false\n")
        f.write("HttpDelayTime=0")

    # Include inputs if asked
    if args.inputs:
        with open(os.path.join(args.path, "Config/DefaultInput.ini"), "a") as f:
            # Write actions
            write_action(f, "Jump", "SpaceBar")
            write_action(f, "Jump", "Vive_Left_Trigger_Click")
            write_action(f, "Spawn", "E")
            write_action(f, "ResetVR", "R")
            write_action(f, "ResetVR", "Vive_Left_Grip_Click")
            write_action(f, "ResetVR", "Vive_Right_Grip_Click")
            write_action(f, "Hide", "End")
            write_action(f, "Hide", "Vive_Left_Menu_Click")
            write_action(f, "Hide", "Vive_Right_Menu_Click")
            write_action(f, "TurnLeft", "Vive_Left_Trackpad_Left")
            write_action(f, "TurnLeft", "Vive_Right_Trackpad_Left")
            write_action(f, "TurnRight", "Vive_Left_Trackpad_Right")
            write_action(f, "TurnRight", "Vive_Right_Trackpad_Right")
            write_action(f, "Quit", "Escape")
            write_action(f, "Quit", "Vive_Left_System_Click")
            write_action(f, "Quit", "Vive_Right_System_Click")

            # Write axes
            write_axis(f, "MoveForward", 1, "W")
            write_axis(f, "MoveForward", -1, "S")
            write_axis(f, "MoveRight", 1, "D")
            write_axis(f, "MoveRight", -1, "A")
            write_axis(f, "Move", 1, "Vive_Right_Trigger_Click")
            write_axis(f, "Turn", 1, "MouseX")
            write_axis(f, "LookUp", -1, "MouseY")

    # Delete old UnrealCollaboration files
    os.remove(os.path.join(args.path, "Source", name + "/UnrealCollaboration.Build.cs"))
    os.remove(os.path.join(args.path, "Source", name + "/UnrealCollaboration.cpp"))
    os.remove(os.path.join(args.path, "Source", name + "/UnrealCollaboration.h"))

    # Edit Project.Build.cs
    with open(os.path.join(args.path, "Source", name + "/" + name + ".Build.cs"), "w") as f:
        f.write("// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.\n\n")
        f.write("using UnrealBuildTool;\n\n")
        f.write("public class " + name + " : ModuleRules {\n")
        f.write("    public " + name + "(ReadOnlyTargetRules Target) : base(Target) {\n")
        f.write("        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;\n\n")
        f.write("        // List packaged used\n")
        f.write("        PublicDependencyModuleNames.AddRange(new string[] { \"Core\", \"CoreUObject\", \"Engine\", \"InputCore\", \"HeadMountedDisplay\", \"Http\", \"Json\", \"JsonUtilities\", \"UMG\", \"Slate\", \"SlateCore\" });\n")
        f.write("    }\n")
        f.write("}\n")

    # Should be us almost all done!
    print("Completed importing!")
    print("Please rebuild VS project files...")
    print("Including starter content? Please migrate from Unreal Collaboration - https://www.ue4community.wiki/Legacy/Migrate_content_between_projects")
    print("Please read documentation to use UnrealCollaboration within your project!")