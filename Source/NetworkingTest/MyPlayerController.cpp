// Fill out your copyright notice in the Description page of Project Settings.

#include "MyPlayerController.h"
#include "ConstructorHelpers.h"
#include "Engine.h"

AMyPlayerController::AMyPlayerController(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
	// stuff
	bShowMouseCursor = true;
}

void AMyPlayerController::SetupInputComponent() {
	if (InputComponent == NULL) {
		InputComponent = NewObject<UInputComponent>(this, TEXT("Cool_InputComponent"));
		InputComponent->RegisterComponent();
	}
	
	check(InputComponent);
	if (InputComponent) {
		InputComponent->BindAction("Spawn", IE_Pressed, this, &AMyPlayerController::Spawn);
	}
}

void AMyPlayerController::Spawn() {
	ServerSpawn();
	UE_LOG(LogTemp, Warning, TEXT("Spawn called"));
}

void AMyPlayerController::ServerSpawn_Implementation() {
	UE_LOG(LogTemp, Warning, TEXT("Server Spawn called"));
}

bool AMyPlayerController::ServerSpawn_Validate() {
	return true;
}