// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EditableTextBox.h"
#include "Button.h"
#include "Internationalization/Text.h"
#include "MyUserWidget.generated.h"

UCLASS()
class NETWORKINGTEST_API UMyUserWidget : public UUserWidget {
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UEditableTextBox* TheBox;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UButton* ConfirmButton;

	UPROPERTY(BlueprintReadOnly)
	class ANetworkingTestHUD* Owner;
	
	FText GetText();
	void SetText(FText Text);
	
protected:
	virtual bool Initialize() override;

	UFUNCTION()
	void Confirm();
};
