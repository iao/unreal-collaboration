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

	/** A customization string from Unreal Selector, should match options there */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = PlayerInfo)
		int info;

	/** The rank of the user as a string, used to find if the user can do privileged operations */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = PlayerInfo)
		FString rank;

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
	/** The users username from Unreal Selector */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = PlayerInfo)
		FString username;

	UPROPERTY(EditAnywhere)
		bool isHidden;

	void KeepAliveResponce(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	void InfoResponce(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	void TimeResponce(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	/** Ask Unreal Selector for information about the player, `UponInfoChanged()` is the callback function */
	UFUNCTION(BlueprintCallable, Category = "Character")
		void RequestInfo();

	/** Callback function for `RequestInfo()` */
	UFUNCTION(BlueprintImplementableEvent, Category = "Character")
		void UponInfoChanged();

	/** Simple function checking if the rank is suitable for administrative controls */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Character")
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
