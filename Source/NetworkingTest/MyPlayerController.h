// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "CubePawn.h"
#include "NetworkingTestCharacter.h"
#include "MyPlayerController.generated.h"

UCLASS()
class NETWORKINGTEST_API AMyPlayerController : public APlayerController {
	GENERATED_BODY()

protected:
	APawn* ThePawn;
	bool isPawn;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSubclassOf<APawn> SpawnableClass;
	
public:
	AMyPlayerController(const FObjectInitializer& ObjectInitializer);
	virtual void SetupInputComponent() override;
	void Spawn();
	
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSpawn();
};
