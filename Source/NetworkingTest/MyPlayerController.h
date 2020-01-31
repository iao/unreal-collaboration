// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "CubePawn.h"
#include "NetworkingTestCharacter.h"
#include "NetworkingTestHUD.h"
#include "MyPlayerController.generated.h"

UCLASS()
class NETWORKINGTEST_API AMyPlayerController : public APlayerController {
	GENERATED_BODY()

protected:
	APawn* ThePawn;

	UPROPERTY(Transient, ReplicatedUsing = OnRep_Pawn)
	bool isPawn;
	
public:
	AMyPlayerController(const FObjectInitializer& ObjectInitializer);
	virtual void SetupInputComponent() override;
	void Spawn();
	void OnRep_Pawn();
	
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSpawn();
};
