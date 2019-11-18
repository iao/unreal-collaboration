// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "NetworkingTestGameMode.h"
#include "NetworkingTestHUD.h"
#include "NetworkingTestCharacter.h"
#include "UObject/ConstructorHelpers.h"

ANetworkingTestGameMode::ANetworkingTestGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = ANetworkingTestHUD::StaticClass();
}
