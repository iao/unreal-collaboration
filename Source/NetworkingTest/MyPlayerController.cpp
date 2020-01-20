// Fill out your copyright notice in the Description page of Project Settings.

#include "MyPlayerController.h"

AMyPlayerController::AMyPlayerController(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
	// Do things here
}

void AMyPlayerController::SetupInputComponent() {
	Super::SetupInputComponent();
	check(InputComponent);

	InputComponent->BindAxis("Move", this, &AMyPlayerController::Move);
}

void AMyPlayerController::Move(float Value) {
	if (Value != 0.0f && (GetPawn() != NULL)) {
		// transform to world space and add it
		GetPawnOrSpectator()->AddMovementInput(GetPawnOrSpectator()->GetActorForwardVector(), Value);
		UE_LOG(LogTemp, Warning, TEXT(":)"));
	}
}