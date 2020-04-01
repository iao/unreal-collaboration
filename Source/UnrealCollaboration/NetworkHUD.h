// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "GameFramework/Pawn.h"
#include "UserWidget.h"
#include "SignWidget.h"
#include "SignPawn.h"
#include "NetworkHUD.generated.h"

UCLASS()
class ANetworkHUD : public AHUD {
	GENERATED_BODY()

	virtual void BeginPlay() override;

public:
	ANetworkHUD();

	UFUNCTION()
	void Confirm();
	void SetVisible(bool visible);
protected:
	TSubclassOf<class USignWidget> PlayerUIClass;
	class USignWidget* PlayerUI;

	FText text;

};
