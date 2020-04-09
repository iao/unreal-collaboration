// By Paul Graham <p@ul.ms>

#include "SignPawn.h"
#include "Net/UnrealNetwork.h"
#include "NetworkHUD.h"
#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "NetworkPlayerController.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ASignPawn::ASignPawn() {
	// Setup Defaults
	if(!TextActorClass) TextActorClass = ANetworkTextRenderActor::StaticClass();

	// Clear the MeshComponent
	GetMeshComponent()->SetStaticMesh(nullptr);
	
	// Setup CameraComponent
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->SetupAttachment(RootComponent);
	CameraComponent->SetRelativeLocation(FVector(180.0f, 0.0f, 80.0f)); // Position the camera
	CameraComponent->SetRelativeRotation(FRotator(0.0f, 340.0f, 180.0f)); // Rotate the camera
	CameraComponent->bUsePawnControlRotation = false; // Keep camera at a fixed rotation
	
	// Setup BoxComponent
	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	BoxComponent->SetupAttachment(GetMeshComponent());
	BoxComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 175.0f)); // Position the box
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
		TextActor = GetWorld()->SpawnActor<ANetworkTextRenderActor>(TextActorClass->GetAuthoritativeClass(), BoxComponent->GetComponentLocation(), BoxComponent->GetComponentRotation());
		TextActor->text = FText::FromString(DefaultText);
	}
	
	// Hide the actor if we need to
	if (GetLocalRole() != ROLE_Authority) {
		APlayerController* controller = GetWorld()->GetFirstPlayerController();
		if (controller) SetActorHiddenInGame(Cast<ANetworkPlayerController>(controller)->isHidden);
	}
}

void ASignPawn::Tick(float DeltaTime) {
	// Face toward player in X
	if (GetLocalRole() != ROLE_Authority && TextActor) {
		FVector PlayerLocation = GetWorld()->GetFirstPlayerController()->PlayerCameraManager->GetCameraLocation();
		FRotator NewRotationAll = UKismetMathLibrary::FindLookAtRotation(PlayerLocation, GetActorLocation());
		FRotator NewRotation = FRotator(GetActorRotation().Pitch, NewRotationAll.Yaw, NewRotationAll.Roll);
		GetMeshComponent()->SetWorldRotation(NewRotation);

		FRotator TextRotationAll = UKismetMathLibrary::FindLookAtRotation(PlayerLocation, TextActor->GetActorLocation());
		FRotator TextRotation = FRotator(TextActor->GetActorRotation().Pitch, 180 + TextRotationAll.Yaw, TextRotationAll.Roll);
		TextActor->SetActorRotation(TextRotation);
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