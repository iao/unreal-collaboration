// By Paul Graham <p@ul.ms>

#include "SignPawn.h"
#include "Net/UnrealNetwork.h"
#include "NetworkHUD.h"
#include "Components/BoxComponent.h"
#include "NetworkPlayerController.h"

// Sets default values
ASignPawn::ASignPawn() {
	// Setup Defaults
	if(!TextActorClass) TextActorClass = ANetworkTextRenderActor::StaticClass();
	
	// Setup BoxComponent
	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	BoxComponent->SetupAttachment(GetParentComponent());
	BoxComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f)); // Position the box
	BoxComponent->SetBoxExtent(FVector(10.0f, 10.0f, 10.0f));

	// Allow ticks
	PrimaryActorTick.bCanEverTick = true;
	
	// Set defaults
	bAddDefaultMovementBindings = false;
	SetReplicates(true);
	SetActorEnableCollision(false);
}

// Called when the game starts or when spawned
void ASignPawn::BeginPlay() {
	Super::BeginPlay();

	// If we are the server, then spawn the text actor
	if (GetLocalRole() == ROLE_Authority) {
		// TODO: Rotate to camera?
		TextActor = GetWorld()->SpawnActor<ANetworkTextRenderActor>(TextActorClass->GetAuthoritativeClass(), GetActorLocation() + BoxComponent->GetRelativeLocation(), BoxComponent->GetComponentRotation());
		TextActor->text = FText::FromString(DefaultText);
	}
	
	// Hide the actor if we need to
	if (GetLocalRole() != ROLE_Authority) {
		APlayerController* controller = GetWorld()->GetFirstPlayerController();
		if (controller) SetActorHiddenInGame(Cast<ANetworkPlayerController>(controller)->isHidden);
	}
}

void ASignPawn::Tick(float DeltaTime) {
	// Face toward player
	if (GetLocalRole() != ROLE_Authority && TextActor) {
		FRotator NewRotation = GetActorRotation() + FRotator(0.f, 1.f, 0.f);
		SetActorRotation(NewRotation);

		FRotator TextNewRotation = TextActor->GetActorRotation() + FRotator(0.f, 1.f, 0.f);
		TextActor->SetActorRotation(NewRotation);
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
		if (controller && TextActor) TextActor->HideActor(Cast<ANetworkPlayerController>(controller)->isHidden);
	}
}

// Sets text to replicate
void ASignPawn::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Tell the engine to call the OnRep_Text each time text changes
	DOREPLIFETIME(ASignPawn, TextActor);
}