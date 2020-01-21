// Fill out your copyright notice in the Description page of Project Settings.

#include "MyUserWidget.h"

bool UMyUserWidget::Initialize() {
    bool isInit = Super::Initialize();
    if (isInit) {
    	if(TheButton) TheButton->OnClicked.AddDynamic(this, &UMyUserWidget::Test);
	} return isInit;
}

void UMyUserWidget::Test() {
    UE_LOG(LogTemp, Warning, TEXT("Pressed!!"));
}