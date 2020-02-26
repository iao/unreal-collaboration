// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "CubePawn.h"
#include "NetworkingTestCharacter.h"
#include "NetworkingTestHUD.h"
#include "HTTPStructs.h"
#include "Runtime/Online/HTTP/Public/Http.h"
#include "MyPlayerController.generated.h"

UCLASS()
class NETWORKINGTEST_API AMyPlayerController : public APlayerController {
	GENERATED_BODY()

protected:
	int counter, counter_max, random_num;
	APawn* ThePawn;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = PlayerInfo)
	int info;

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
	
	AMyPlayerController(const FObjectInitializer& ObjectInitializer);
	virtual void SetupInputComponent() override;
	virtual void Tick(float DeltaTime) override;
	void Spawn();
	void Hide();
	void OnRep_Pawn();
	
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSpawn();
};
