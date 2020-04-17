// By Paul Graham <p@ul.ms>

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "UnrealCollaboration/Character/VR/VRNetworkCharacter.h"
#include "UnrealCollaboration/Character/VR/VRNetworkPlayerController.h"
#include "UnrealCollaboration/Character/Main/NetworkCharacter.h"
#include "UnrealCollaboration/Character/Main/NetworkPlayerController.h"
#include "NetworkBaseGameMode.generated.h"

UCLASS()
class UNREALCOLLABORATION_API ANetworkBaseGameMode : public AGameModeBase {
	GENERATED_BODY()

public:
	/** The default player controller */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = SpawnInfo)
		TSubclassOf<ANetworkPlayerController> NetworkPlayerControllerClass;

	/** The default player controller for players in VR */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = SpawnInfo)
		TSubclassOf<AVRNetworkPlayerController> NetworkVRPlayerControllerClass;

	/** The default pawn */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = SpawnInfo)
		TSubclassOf<ANetworkCharacter> NetworkCharacterClass;
	
	/** The default pawn for players in VR */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = SpawnInfo)
		TSubclassOf<AVRNetworkCharacter> NetworkVRCharacterClass;

	/** Allow us to set the player controller based on what connection we have coming in */
	APlayerController* SpawnPlayerController(ENetRole InRemoteRole, const FString& Options) override;
	// Plan of attack - override this (^) function to provide us the ability to set the player controller (VR / Our own) & DefaultPawn (accordingly)
};
