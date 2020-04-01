// Fill out your copyright notice in the Description page of Project Settings.

#include "SignWidget.h"
#include "NetworkHUD.h"

bool USignWidget::Initialize() {
    bool isInit = Super::Initialize();
    if (isInit) {
    	if(ConfirmButton) ConfirmButton->OnClicked.AddDynamic(this, &USignWidget::Confirm);
	} return isInit;
}

FText USignWidget::GetText() {
    return TheBox->GetText();
}

void USignWidget::SetText(FText Text) {
    TheBox->SetText(Text);
}

void USignWidget::Confirm() {
    Owner->Confirm();
}
