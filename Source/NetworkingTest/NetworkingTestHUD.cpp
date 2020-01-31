// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "NetworkingTestHUD.h"
#include "Engine/Canvas.h"
#include "CanvasItem.h"
#include "UObject/ConstructorHelpers.h"

ANetworkingTestHUD::ANetworkingTestHUD() {
	ConstructorHelpers::FClassFinder<UMyUserWidget> UIClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/UserWidget"));
	PlayerUIClass = UIClassFinder.Class;

	text = FText::FromString(FString(TEXT("Hello There")));
}

void ANetworkingTestHUD::BeginPlay() {
	Super::BeginPlay();

	PlayerUI = CreateWidget<UMyUserWidget>(GetOwningPlayerController(), PlayerUIClass);
	if (PlayerUI) {
		PlayerUI->Owner = this;
		PlayerUI->SetText(text);
		PlayerUI->AddToViewport();
	}
}

void ANetworkingTestHUD::Confirm() {
	text = PlayerUI->GetText();
	PlayerUI->SetText(text);

	ACubePawn* pawn = Cast<ACubePawn>(GetOwningPawn());
	pawn->SetText(text);
}