// By Paul Graham <p@ul.ms>

#include "NetworkHUD.h"
#include "../Character/Main/NetworkPlayerController.h"
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

	// Create the widget and setup options
	PlayerUI = CreateWidget<USignWidget>(GetOwningPlayerController(), PlayerUIClass->GetAuthoritativeClass());
	if (PlayerUI) {
		PlayerUI->Owner = this;
		PlayerUI->AddToViewport();
		PlayerUI->SetVisibility(ESlateVisibility::Hidden);
	}
}

// Changes a boolean to ESlateVisibility
ESlateVisibility ANetworkHUD::fromBoolean(bool boolean) {
	return boolean ? ESlateVisibility::Visible : ESlateVisibility::Hidden;
}

// Sets the visibility of the HUD and components inside of it
void ANetworkHUD::SetVisible(bool visible) {
	ESlateVisibility visability = fromBoolean(visible);
	ANetworkPlayerController* controller = Cast<ANetworkPlayerController>(GetOwningPlayerController());

	if (PlayerUI) {
		PlayerUI->SetVisibility(visability);
		
		if (visible) {
			// Set the text in the box to that of the pawn if it isn't the default text
			ASignPawn* pawn = Cast<ASignPawn>(GetOwningPawn());
			if (pawn) {
				if (pawn->TextActor && !pawn->TextActor->text.EqualTo(FText::FromString(pawn->DefaultText))) PlayerUI->TheBox->SetText(pawn->TextActor->text);
				else PlayerUI->TheBox->SetText(FText::FromString(""));
			} else PlayerUI->TheBox->SetText(FText::FromString(""));

			// Allow the player to delete the sign at anytime
			PlayerUI->DeleteButton->SetVisibility(fromBoolean(useSelectorControl ? controller->HasSelectorAuthority() : true));
		}
	}
}

// Called when the confirm button is clicked
void ANetworkHUD::Confirm() {
	text = PlayerUI->GetText();

	// Ensure the text is under the maximum amount of characters
	if(text.ToString().Len() > MaximumTextSize) {
		if(PlayerUI) {
			FString message = "Text is over the maximum of ";
			message.AppendInt(MaximumTextSize);
			message.Append(" characters.");
			PlayerUI->ErrorBox->SetText(FText::FromString(message));
		}
	} else {
		// Set the text of the pawn
		ASignPawn* pawn = Cast<ASignPawn>(GetOwningPawn());
		pawn->SetText(text);
	}
}

// Called when the delete button is pressed
void ANetworkHUD::Delete() {
	// NOTE: If selector control is enabled, this is only allowed when controller->HasSelectorAuthority() is true :)
	ANetworkPlayerController* controller = Cast<ANetworkPlayerController>(GetOwningPlayerController());
	controller->Delete();
}