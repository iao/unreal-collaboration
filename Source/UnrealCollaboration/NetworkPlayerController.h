// By Paul Graham <p@ul.ms>

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "SignPawn.h"
#include "NetworkCharacter.h"
#include "NetworkHUD.h"
#include "HTTPStructs.h"
#include "Runtime/Online/HTTP/Public/Http.h"
#include "NetworkPlayerController.generated.h"

UCLASS()
class UNREALCOLLABORATION_API ANetworkPlayerController : public APlayerController {
	GENERATED_BODY()

protected:
	/** The timer, calling /keepalive to keep the server alive */
	FTimerHandle KeepAliveTimer;

	/** Variables for KeepAliveTimer */
	long int counter_max, random_num;

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
	/** If the player is an admin */
	UPROPERTY(Replicated, Transient)
		bool isAdmin;

	/** If the player has chosen to hide signs in the world */
	UPROPERTY(EditAnywhere)
		bool isHidden;

	/** Called when /keepalive responds */
	void KeepAliveResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	/** Called when /time responds */
	void TimeResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	/** Simple function checking if the rank is suitable for administrative controls */
	UFUNCTION(BlueprintCallable, Category = "Character")
		bool HasSelectorAuthority();

	ANetworkPlayerController(const FObjectInitializer& ObjectInitializer);
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	virtual void Tick(float DeltaTime) override;

	/** Send a keepalive request */
	void KeepAlive();

	/** Client side spawn function */
	void Spawn();

	/** Toggle hiding the signs on the client */
	void Hide();

	/** Quit the game */
	void Quit();

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
