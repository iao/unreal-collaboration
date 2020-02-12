// Fill out your copyright notice in the Description page of Project Settings.


#include "MyTextRenderActor.h"
#include "UnrealNetwork.h"

// Make the object
AMyTextRenderActor::AMyTextRenderActor() {
	GetTextRender()->SetTextRenderColor(FColor::White);
	
	SetReplicates(true);
}

// Called whenever text is replicated
void AMyTextRenderActor::OnRep_Text() {
	GetTextRender()->SetText(text);
}

// Called to disable the actor from the client
void AMyTextRenderActor::HideActor(bool toHide) {
	SetActorHiddenInGame(toHide);
}

// Sets text to replicate
void AMyTextRenderActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Tell the engine to call the OnRep_Text each time text changes
	DOREPLIFETIME(AMyTextRenderActor, text);
}