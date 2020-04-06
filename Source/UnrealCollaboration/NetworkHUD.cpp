// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "NetworkHUD.h"
#include "NetworkPlayerController.h"
#include "Engine/Canvas.h"
#include "CanvasItem.h"
#include "UObject/ConstructorHelpers.h"

void ANetworkHUD::BeginPlay() {
	Super::BeginPlay();

	PlayerUI = CreateWidget<USignWidget>(GetOwningPlayerController(), PlayerUIClass->GetAuthoritativeClass());
	if (PlayerUI) {
		PlayerUI->Owner = this;
		PlayerUI->SetText(FText::FromString(DefaultText));
		PlayerUI->AddToViewport();
		PlayerUI->SetVisibility(ESlateVisibility::Hidden);
	} 
}

ESlateVisibility ANetworkHUD::fromBoolean(bool boolean) {
	return boolean ? ESlateVisibility::Visible : ESlateVisibility::Hidden;
}

void ANetworkHUD::SetVisible(bool visible) {
	ESlateVisibility visability = fromBoolean(visible);
	ANetworkPlayerController* controller = Cast<ANetworkPlayerController>(GetOwningPlayerController());
	
	if (PlayerUI) {
		PlayerUI->SetVisibility(visability);
		if(visible) PlayerUI->DeleteButton->SetVisibility(fromBoolean(controller->HasSelectorAuthority()));
	}
}

void ANetworkHUD::Confirm() {
	text = PlayerUI->GetText();
	PlayerUI->SetText(text);

	ASignPawn* pawn = Cast<ASignPawn>(GetOwningPawn());
	pawn->SetText(text);
}

void ANetworkHUD::Delete() {
	ANetworkPlayerController* controller = Cast<ANetworkPlayerController>(GetOwningPlayerController());
	controller->Delete();
}