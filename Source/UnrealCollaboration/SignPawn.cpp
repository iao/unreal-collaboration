// Fill out your copyright notice in the Description page of Project Settings.
#include "SignPawn.h"
#include "UnrealNetwork.h"
#include "NetworkHUD.h"
#include "NetworkPlayerController.h"

// Sets default values
ASignPawn::ASignPawn() {
	bAddDefaultMovementBindings = false;
	SetReplicates(true);
	SetActorEnableCollision(false);
}

// Called when the game starts or when spawned
void ASignPawn::BeginPlay() {
	Super::BeginPlay();

	// If we are the server, then spawn the text actor
	if (GetLocalRole() == ROLE_Authority) {
		TextActor = GetWorld()->SpawnActor<ANetworkTextRenderActor>(ANetworkTextRenderActor::StaticClass(),
					GetActorLocation() + FVector(14.f, 75.f, 20.f).RotateAngleAxis(GetActorRotation().Roll, FVector(0, 0, 1)), GetActorRotation());
		TextActor->text = FText::FromString(DefaultText);
	}

	// Hide the actor if we need to
	if (GetLocalRole() != ROLE_Authority) {
		APlayerController* controller = GetWorld()->GetFirstPlayerController();
		if(controller) SetActorHiddenInGame(Cast<ANetworkPlayerController>(controller)->isHidden);
	}
}

// Called to disable the actor from the client
void ASignPawn::HideActor(bool toHide) {
	SetActorHiddenInGame(toHide);
	if (TextActor) TextActor->HideActor(toHide);
}

// Called to set the text from clients
void ASignPawn::SetText(FText text) {
	ServerSetText(text);
}

// Set the text on the server
void ASignPawn::ServerSetText_Implementation(const FText& text) {
	TextActor->text = text;
}

// Validate
bool ASignPawn::ServerSetText_Validate(const FText& text) {
	return true;
}

// Called when the text actor replicates
void ASignPawn::OnRep_TextActor() {
	// Set the actor to hide
	if (GetLocalRole() != ROLE_Authority) {
		APlayerController* controller = GetWorld()->GetFirstPlayerController();
		if(controller && TextActor) TextActor->HideActor(Cast<ANetworkPlayerController>(controller)->isHidden);
	}
}

// Sets text to replicate
void ASignPawn::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Tell the engine to call the OnRep_Text each time text changes
	DOREPLIFETIME(ASignPawn, TextActor);
}