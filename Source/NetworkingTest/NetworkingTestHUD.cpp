// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "NetworkingTestHUD.h"
#include "Engine/Canvas.h"
#include "Engine/Texture2D.h"
#include "TextureResource.h"
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
	UE_LOG(LogTemp, Warning, TEXT("Pressed!! Got %s!"), *text.ToString());
	text = PlayerUI->GetText();
	PlayerUI->SetText(text);
	UE_LOG(LogTemp, Warning, TEXT("Done!! Got %s!"), *text.ToString());

	ACubePawn* pawn = Cast<ACubePawn>(GetOwningPawn());
	pawn->SetText(text);
}