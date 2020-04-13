// By Paul Graham <p@ul.ms>

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "../Sign/SignPawn.h"
#include "BaseNetworkCharacter.h"
#include "../Sign/NetworkHUD.h"
#include "../HTTP/HTTPStructs.h"
#include "Runtime/Online/HTTP/Public/Http.h"
#include "BaseNetworkPlayerController.generated.h"

UCLASS()
class UNREALCOLLABORATION_API ABaseNetworkPlayerController : public APlayerController {
	GENERATED_BODY()

protected:
	/** The timer, calling /keepalive to keep the server alive */
	FTimerHandle KeepAliveTimer;

	/** Variables for KeepAliveTimer */
	long int counter_max, random_num;

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

	ABaseNetworkPlayerController(const FObjectInitializer& ObjectInitializer);
	void BeginPlay() override;
	virtual void SetupInputComponent() override;
	void Tick(float DeltaTime) override;

	/** Send a keepalive request */
	void KeepAlive();

	/** Toggle hiding the signs on the client */
	virtual void Hide() {};

	/** Quit the game */
	void Quit();

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerQuit();
};
