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
	long int counter, counter_max, random_num;
	ASignPawn* ThePawn;

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
	UPROPERTY(EditAnywhere)
		bool isHidden;

	void KeepAliveResponce(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	void TimeResponce(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	/** Simple function checking if the rank is suitable for administrative controls */
	UFUNCTION(BlueprintCallable, Category = "Character")
		bool HasSelectorAuthority();

	ANetworkPlayerController(const FObjectInitializer& ObjectInitializer);
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	virtual void Tick(float DeltaTime) override;
	void Spawn();
	void Hide();

	UFUNCTION()
		void Delete();
	void OnRep_Pawn();
	
	UFUNCTION(Server, Reliable, WithValidation)
		void ServerSpawn();

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerDelete();
};
