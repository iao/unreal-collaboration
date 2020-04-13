// By Paul Graham <p@ul.ms>

#pragma once

#include "CoreMinimal.h"
#include "../BaseNetworkPlayerController.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "../../Sign/SignPawn.h"
#include "NetworkCharacter.h"
#include "../../Sign/NetworkHUD.h"
#include "../../HTTP/HTTPStructs.h"
#include "Runtime/Online/HTTP/Public/Http.h"
#include "NetworkPlayerController.generated.h"

UCLASS()
class UNREALCOLLABORATION_API ANetworkPlayerController : public ABaseNetworkPlayerController {
	GENERATED_BODY()

protected:
	/** The sign pawn, which we can possess */
	ASignPawn* ThePawn;

	/** If we are currently possessing a sign or not */
	UPROPERTY(Transient, ReplicatedUsing = OnRep_Pawn)
		bool isPawn;

	/** The sign blueprint class to spawn */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = SpawnInfo)
		TSubclassOf<ASignPawn> SpawnableClass;

	/** Where to spawn the SpawnableClass relative to the camera. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SpawnInfo)
		float SpawnDistance;

	/** The distance which we look for a sign around the user instead of spawning a new one */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = SpawnInfo)
		float FindDistance;

public:
	ANetworkPlayerController(const FObjectInitializer& ObjectInitializer);
	void SetupInputComponent() override;
	
	/** Client side spawn function */
	void Spawn();

	/** Hide signs */
	void Hide() override;

	/** Client side delete function to remove a sign */
	UFUNCTION()
		void Delete();

	/** Called after isPawn has replicated, ensures new pawns are hidden */
	void OnRep_Pawn();

	/** Spawn a new sign on the server & possess it */
	UFUNCTION(Server, Reliable, WithValidation)
		void ServerSpawn();

	/** Delete a sign and possess the actor */
	UFUNCTION(Server, Reliable, WithValidation)
		void ServerDelete();
};
