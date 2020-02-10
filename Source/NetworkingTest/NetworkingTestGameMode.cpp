// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "NetworkingTestGameMode.h"
#include "NetworkingTestCharacter.h"
#include "MyPlayerController.h"
#include "UObject/ConstructorHelpers.h"

ANetworkingTestGameMode::ANetworkingTestGameMode() : Super() {
	// set our player controller to our own one
	static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerClassFinder(
		TEXT("/Game/FirstPersonCPP/Blueprints/MyPlayerController"));
	PlayerControllerClass = PlayerControllerClassFinder.Class;

	// use our custom HUD class
	HUDClass = ANetworkingTestHUD::StaticClass();

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(
		TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;
}
