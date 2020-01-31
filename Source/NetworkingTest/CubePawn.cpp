// Fill out your copyright notice in the Description page of Project Settings.
#include "CubePawn.h"
#include "UnrealNetwork.h"

// Sets default values
ACubePawn::ACubePawn() {
	bAddDefaultMovementBindings = false;
	SetReplicates(true);
}

// Called when the game starts or when spawned
void ACubePawn::BeginPlay() {
	Super::BeginPlay();

	// If we are the server, then spawn the text actor
	if (GetLocalRole() == ROLE_Authority) {
		TextActor = GetWorld()->SpawnActor<AMyTextRenderActor>(AMyTextRenderActor::StaticClass(), GetActorLocation(), GetActorRotation());
		TextActor->text = FText::FromString("Please enter text");
	}
}

// Called to set the text from clients
void ACubePawn::SetText(FText text) {
	ServerSetText(text);
}

// Set the text on the server
void ACubePawn::ServerSetText_Implementation(const FText& text) {
	TextActor->text = text;
}

// Validate
bool ACubePawn::ServerSetText_Validate(const FText& text) {
	return true;
}