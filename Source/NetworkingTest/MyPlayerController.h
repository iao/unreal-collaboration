// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"
#include "UserWidget.h"
#include "MyUserWidget.h"
#include "CubePawn.h"
#include "MyPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class NETWORKINGTEST_API AMyPlayerController : public APlayerController {
	GENERATED_BODY()

public:
	AMyPlayerController(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;

	UFUNCTION()
	void Confirm();
	
protected:
	TSubclassOf<class UMyUserWidget> PlayerUIClass;
	class UMyUserWidget* PlayerUI;
	
	FText text;
};
