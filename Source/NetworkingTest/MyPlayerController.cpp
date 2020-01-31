// Fill out your copyright notice in the Description page of Project Settings.

#include "MyPlayerController.h"
#include "ConstructorHelpers.h"
#include "Engine.h"
#include "UnrealNetwork.h"

// Constructor
AMyPlayerController::AMyPlayerController(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
	isPawn = false;
}

// Setup inputs for the controller
void AMyPlayerController::SetupInputComponent() {
	if (InputComponent == NULL) {
		InputComponent = NewObject<UInputComponent>(this, TEXT("Cool_InputComponent"));
		InputComponent->RegisterComponent();
	}
	
	check(InputComponent);
	if (InputComponent) {
		// Spawn a sign
		InputComponent->BindAction("Spawn", IE_Pressed, this, &AMyPlayerController::Spawn);
	}
}

// Client side spawn function
void AMyPlayerController::Spawn() {
	ServerSpawn();
}

// Server side spawn function
void AMyPlayerController::ServerSpawn_Implementation() {
	if (isPawn) {
		// Find the owner
		ANetworkingTestCharacter* Actor = Cast<ANetworkingTestCharacter>(ThePawn->GetInstigator());

		// Possess the owner
		Possess(Actor);

		// Update state
		isPawn = false;
	} else {
		// Spawn the blueprinted pawn
		// Get a reference to the owner
		ANetworkingTestCharacter* Actor = Cast<ANetworkingTestCharacter>(GetPawnOrSpectator());
		
		// Try to find a pawn within a distance, ensure it doesnt have an owner

		// Else Make the pawn from blueprinted pawn
		UObject* PawnActor = Cast<UObject>(StaticLoadObject(UObject::StaticClass(), NULL, TEXT("/Game/FirstPersonCPP/Blueprints/MyCubePawn")));
		UBlueprint* GeneratedBP = Cast<UBlueprint>(PawnActor);
		UClass* PawnClass = PawnActor->StaticClass();

		// Set the location etc of the new actor
		const FVector Location = Actor->GetActorLocation() + FVector(10.f, 10.f, 0);
		FActorSpawnParameters Params = FActorSpawnParameters();
		Params.Owner = Cast<AActor>(Actor);
		Params.Instigator = Cast<APawn>(Actor);

		// Spawn the pawn to the world
		ThePawn = GetWorld()->SpawnActor<APawn>(GeneratedBP->GeneratedClass, Location, Actor->GetActorRotation(), Params);
		
		// Possess the pawn
		Possess(ThePawn);

		// Update state
		isPawn = true;
	}
}

// Validation
bool AMyPlayerController::ServerSpawn_Validate() {
	return true;
}

// Called when isPawn replicates
void AMyPlayerController::OnRep_Pawn() {
	bShowMouseCursor = isPawn;
	Cast<ANetworkingTestHUD>(GetHUD())->SetVisible(isPawn);
}

// Replication of isPawn
void AMyPlayerController::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AMyPlayerController, isPawn);
}
