// Fill out your copyright notice in the Description page of Project Settings.


#include "NetworkTextRenderActor.h"
#include "UnrealNetwork.h"

// Make the object
ANetworkTextRenderActor::ANetworkTextRenderActor() {
	GetTextRender()->SetTextRenderColor(FColor::White);
	
	SetReplicates(true);
}

// Called whenever text is replicated
void ANetworkTextRenderActor::OnRep_Text() {
	GetTextRender()->SetText(text);
}

// Called to disable the actor from the client
void ANetworkTextRenderActor::HideActor(bool toHide) {
	SetActorHiddenInGame(toHide);
}

// Sets text to replicate
void ANetworkTextRenderActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Tell the engine to call the OnRep_Text each time text changes
	DOREPLIFETIME(ANetworkTextRenderActor, text);
}