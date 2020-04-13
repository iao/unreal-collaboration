// By Paul Graham <p@ul.ms>

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/MultiLineEditableTextBox.h"
#include "Components/Button.h"
#include "Internationalization/Text.h"
#include "Components/TextBlock.h"
#include "SignWidget.generated.h"

UCLASS()
class UNREALCOLLABORATION_API USignWidget : public UUserWidget {
	GENERATED_BODY()

public:
	/** The text block to place error messages within */
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UTextBlock* ErrorBox;

	/** The box where users can enter their message on the sign */
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UMultiLineEditableTextBox* TheBox;

	/** The button to confirm that message */
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UButton* ConfirmButton;

	/** The button to delete the sign */
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UButton* DeleteButton;

	/** The owner of the widget, where actions will occur */
	UPROPERTY(BlueprintReadOnly)
		class ANetworkHUD* Owner;

	/** Get the text in TheBox */
	FText GetText();

protected:
	virtual bool Initialize() override;

	/** Function called when confirm button is pressed */
	UFUNCTION()
		void Confirm();

	/** Function called when delete button is pressed */
	UFUNCTION()
		void Delete();
};
