// Fill out your copyright notice in the Description page of Project Settings.

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
	int counter, counter_max, random_num;
	APawn* ThePawn;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = PlayerInfo)
	int info;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = PlayerInfo)
	FString rank;

	UPROPERTY(Transient, ReplicatedUsing = OnRep_Pawn)
	bool isPawn;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSubclassOf<APawn> SpawnableClass;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float FindDistance;
public:
	UPROPERTY(EditAnywhere)
	bool isHidden;

	void KeepAliveResponce(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	void InfoResponce(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	UFUNCTION(BlueprintCallable, Category = "Character")
	void RequestInfo();
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Character")
	void UponInfoChanged();
	
	ANetworkPlayerController(const FObjectInitializer& ObjectInitializer);
	virtual void SetupInputComponent() override;
	virtual void Tick(float DeltaTime) override;
	void Spawn();
	void Hide();
	void OnRep_Pawn();
	
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSpawn();
};
