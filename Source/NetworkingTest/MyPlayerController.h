// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"
#include "MyPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class NETWORKINGTEST_API AMyPlayerController : public APlayerController {
	GENERATED_BODY()

public:
	AMyPlayerController(const FObjectInitializer& ObjectInitializer);

	void Move(float Value);
	
	virtual void SetupInputComponent() override;
};
