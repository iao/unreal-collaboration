// By Paul Graham <p@ul.ms>

#include "NetworkTextRenderActor.h"
#include "Net/UnrealNetwork.h"

// Make the object
ANetworkTextRenderActor::ANetworkTextRenderActor() {
	GetTextRender()->SetHorizontalAlignment(EHTA_Center);
	SetReplicates(true);
	SetReplicateMovement(true);
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