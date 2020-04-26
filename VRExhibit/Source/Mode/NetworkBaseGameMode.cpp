// By Paul Graham <p@ul.ms>

#include "NetworkBaseGameMode.h"

APlayerController* ANetworkBaseGameMode::SpawnPlayerController(ENetRole InRemoteRole, const FString& Options) {
	// We want to set PlayerControllerClass & DefaultPawnClass here based on what Options contain
	PlayerControllerClass = NetworkPlayerControllerClass;
	DefaultPawnClass = NetworkCharacterClass;
	 
	// Call with `MyGame.exe 127.0.0.1?VRMode=1 -vr`
	if(Options.Contains(FString(TEXT("VRMode=1")))) {
		PlayerControllerClass = NetworkVRPlayerControllerClass;
		DefaultPawnClass = NetworkVRCharacterClass;
	}
	
	// Then call our super
	return Super::SpawnPlayerController(InRemoteRole, Options);
}