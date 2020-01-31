// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "GameFramework/Pawn.h"
#include "UserWidget.h"
#include "MyUserWidget.h"
#include "CubePawn.h"
#include "NetworkingTestHUD.generated.h"

UCLASS()
class ANetworkingTestHUD : public AHUD {
	GENERATED_BODY()

	virtual void BeginPlay() override;

public:
	ANetworkingTestHUD();

	UFUNCTION()
	void Confirm();
	void SetVisible(bool visible);
protected:
	TSubclassOf<class UMyUserWidget> PlayerUIClass;
	class UMyUserWidget* PlayerUI;

	FText text;

};
