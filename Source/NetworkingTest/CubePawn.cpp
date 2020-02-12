// Fill out your copyright notice in the Description page of Project Settings.
#include "CubePawn.h"
#include "UnrealNetwork.h"
#include "MyPlayerController.h"

// Sets default values
ACubePawn::ACubePawn() {
	bAddDefaultMovementBindings = false;
	SetReplicates(true);
	SetActorEnableCollision(false);
}

// Called when the game starts or when spawned
void ACubePawn::BeginPlay() {
	Super::BeginPlay();

	// If we are the server, then spawn the text actor
	if (GetLocalRole() == ROLE_Authority) {
		TextActor = GetWorld()->SpawnActor<AMyTextRenderActor>(AMyTextRenderActor::StaticClass(), 
					GetActorLocation() + FVector(14.f, 75.f, 20.f).RotateAngleAxis(GetActorRotation().Roll, FVector(0, 0, 1)), GetActorRotation());
		TextActor->text = FText::FromString("hello");
	}

	// Hide the actor if we need to
	if (GetLocalRole() != ROLE_Authority) {
		APlayerController* controller = GetWorld()->GetFirstPlayerController();
		if(controller) SetActorHiddenInGame(Cast<AMyPlayerController>(controller)->isHidden);
	}
}

// Called to disable the actor from the client
void ACubePawn::HideActor(bool toHide) {
	SetActorHiddenInGame(toHide);
	if (TextActor) TextActor->HideActor(toHide);
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

// Called when the text actor replicates
void ACubePawn::OnRep_TextActor() {
	// Set the actor to hide
	if (GetLocalRole() != ROLE_Authority) {
		APlayerController* controller = GetWorld()->GetFirstPlayerController();
		if(controller && TextActor) TextActor->HideActor(Cast<AMyPlayerController>(controller)->isHidden);
	}
}

// Sets text to replicate
void ACubePawn::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Tell the engine to call the OnRep_Text each time text changes
	DOREPLIFETIME(ACubePawn, TextActor);
}