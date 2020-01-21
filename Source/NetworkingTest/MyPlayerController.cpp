// Fill out your copyright notice in the Description page of Project Settings.

#include "MyPlayerController.h"
#include "ConstructorHelpers.h"

AMyPlayerController::AMyPlayerController(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
	ConstructorHelpers::FClassFinder<UMyUserWidget> UIClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/UserWidget"));
	PlayerUIClass = UIClassFinder.Class;

	text = FText::FromString(FString(TEXT("Hello There")));
}

void AMyPlayerController::BeginPlay() {
	Super::BeginPlay();

	PlayerUI = CreateWidget<UMyUserWidget>(this, PlayerUIClass);
	PlayerUI->Controller = this;
	PlayerUI->SetText(text);
	PlayerUI->AddToViewport();
	bShowMouseCursor = true;
}

void AMyPlayerController::Confirm() {
	UE_LOG(LogTemp, Warning, TEXT("Pressed!! Got %s!"), *text.ToString());
	text = PlayerUI->GetText();
	PlayerUI->SetText(text);
	UE_LOG(LogTemp, Warning, TEXT("Done!! Got %s!"), *text.ToString());

	ACubePawn* pawn = Cast<ACubePawn>(GetPawnOrSpectator());
	pawn->SetText(text);
}