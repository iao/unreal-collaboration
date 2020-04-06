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
	UFUNCTION()
	void Confirm();

	UFUNCTION()
	void Delete();
	
	void SetVisible(bool visible);
	
protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = SpawnInfo)
	TSubclassOf<USignWidget> PlayerUIClass;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = SpawnInfo)
	FString DefaultText;
	
	ESlateVisibility fromBoolean(bool boolean);
	
	class USignWidget* PlayerUI;

	FText text;

};
