// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EditableTextBox.h"
#include "Button.h"
#include "Internationalization/Text.h"
#include "SignWidget.generated.h"

UCLASS()
class UNREALCOLLABORATION_API USignWidget : public UUserWidget {
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UEditableTextBox* TheBox;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UButton* ConfirmButton;

	UPROPERTY(BlueprintReadOnly)
	class ANetworkHUD* Owner;
	
	FText GetText();
	void SetText(FText Text);
	
protected:
	virtual bool Initialize() override;

	UFUNCTION()
	void Confirm();
};
