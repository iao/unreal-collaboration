// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "NetworkGameMode.h"
#include "NetworkCharacter.h"
#include "NetworkPlayerController.h"
#include "UObject/ConstructorHelpers.h"

ANetworkGameMode::ANetworkGameMode() : Super() {
	// TODO: Allow these to be changed
	// set our player controller to our own one
	static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerClassFinder(TEXT("/Game/Blueprints/PlayerController"));
	PlayerControllerClass = PlayerControllerClassFinder.Class;

	// use our custom HUD class
	HUDClass = ANetworkHUD::StaticClass();

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/Blueprints/NetworkCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;
}
