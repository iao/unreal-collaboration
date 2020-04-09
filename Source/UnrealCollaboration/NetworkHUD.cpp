// By Paul Graham <p@ul.ms>

#include "NetworkHUD.h"
#include "NetworkPlayerController.h"
#include "Engine/Canvas.h"
#include "CanvasItem.h"
#include "UObject/ConstructorHelpers.h"

// Constructor
ANetworkHUD::ANetworkHUD() {
	// Setup Defaults
	if (!PlayerUIClass) PlayerUIClass = USignWidget::StaticClass();
}

// Called after Constructor
void ANetworkHUD::BeginPlay() {
	Super::BeginPlay();

	PlayerUI = CreateWidget<USignWidget>(GetOwningPlayerController(), PlayerUIClass->GetAuthoritativeClass());
	if (PlayerUI) {
		PlayerUI->Owner = this;
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
		if (visible) PlayerUI->DeleteButton->SetVisibility(fromBoolean(controller->HasSelectorAuthority()));
	}
}

void ANetworkHUD::Confirm() {
	text = PlayerUI->GetText();

	ASignPawn* pawn = Cast<ASignPawn>(GetOwningPawn());
	pawn->SetText(text);
}

void ANetworkHUD::Delete() {
	// NOTE: This is only allowed when controller->HasSelectorAuthority() is true :)
	ANetworkPlayerController* controller = Cast<ANetworkPlayerController>(GetOwningPlayerController());
	controller->Delete();
}