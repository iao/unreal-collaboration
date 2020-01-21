// Fill out your copyright notice in the Description page of Project Settings.

#include "MyUserWidget.h"
#include "MyPlayerController.h"

bool UMyUserWidget::Initialize() {
    bool isInit = Super::Initialize();
    if (isInit) {
    	if(ConfirmButton) ConfirmButton->OnClicked.AddDynamic(this, &UMyUserWidget::Confirm);
	} return isInit;
}

FText UMyUserWidget::GetText() {
    return TheBox->GetText();
}

void UMyUserWidget::SetText(FText Text) {
    TheBox->SetText(Text);
}

void UMyUserWidget::Confirm() {
    Controller->Confirm();
}
