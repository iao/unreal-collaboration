// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "NetworkHUD.h"
#include "Engine/Canvas.h"
#include "CanvasItem.h"
#include "UObject/ConstructorHelpers.h"

ANetworkHUD::ANetworkHUD() {
	// TODO: Allow this to be changed
	ConstructorHelpers::FClassFinder<USignWidget> UIClassFinder(TEXT("/Game/Blueprints/SignWidget"));
	PlayerUIClass = UIClassFinder.Class;
	
	text = FText::FromString(FString(TEXT("Please type text here")));
}

void ANetworkHUD::BeginPlay() {
	Super::BeginPlay();

	PlayerUI = CreateWidget<USignWidget>(GetOwningPlayerController(), PlayerUIClass);
	if (PlayerUI) {
		PlayerUI->Owner = this;
		PlayerUI->SetText(text);
		PlayerUI->AddToViewport();
		PlayerUI->SetVisibility(ESlateVisibility::Hidden);
	} 
}

void ANetworkHUD::SetVisible(bool visible) {
	if (PlayerUI) {
		if (visible) PlayerUI->SetVisibility(ESlateVisibility::Visible);
		else PlayerUI->SetVisibility(ESlateVisibility::Hidden);
	}
}

void ANetworkHUD::Confirm() {
	text = PlayerUI->GetText();
	PlayerUI->SetText(text);

	ASignPawn* pawn = Cast<ASignPawn>(GetOwningPawn());
	pawn->SetText(text);
}