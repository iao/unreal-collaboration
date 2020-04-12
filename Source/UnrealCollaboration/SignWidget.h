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
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UTextBlock* ErrorBox;
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UMultiLineEditableTextBox* TheBox;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UButton* ConfirmButton;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UButton* DeleteButton;

	UPROPERTY(BlueprintReadOnly)
		class ANetworkHUD* Owner;

	FText GetText();

protected:
	virtual bool Initialize() override;

	UFUNCTION()
		void Confirm();

	UFUNCTION()
		void Delete();
};
