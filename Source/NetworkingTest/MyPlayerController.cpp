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
		// Find the instigator and possess
		ANetworkingTestCharacter* Actor = Cast<ANetworkingTestCharacter>(ThePawn->GetInstigator());
		ThePawn->SetInstigator(NULL);
		
		// Possess and update state
		Possess(Actor);
		isPawn = false;
	} else {
		// Spawn the blueprinted pawn
		// Get a reference to the current actor (owner)
		ANetworkingTestCharacter* Actor = Cast<ANetworkingTestCharacter>(GetPawnOrSpectator());

		// Try to find a pawn within a distance, ensure it doesnt have an instigator
		for(TActorIterator<ACubePawn> CubeItr(GetWorld()); CubeItr; ++CubeItr) {
			ACubePawn* FoundPawn = Cast<ACubePawn>(*CubeItr);
			if(FoundPawn) {
				float distance = FVector::Dist(FoundPawn->GetActorLocation(), Actor->GetActorLocation());
				if(distance <= 500.0f && !FoundPawn->GetInstigator()) {
					// When we have found a pawn which we can use, set it's instigator and possess it
					ThePawn = FoundPawn;
					ThePawn->SetInstigator(Actor);

					// Possess, update state and return
					Possess(ThePawn);
					isPawn = true;
					return;
				}
			}
		}
		
		// Otherwise Make the pawn from blueprinted pawn
		// Set the location etc of the new act
		const FVector Location = Actor->GetActorLocation() + FVector(0.f, 500.f, 0).RotateAngleAxis(Actor->GetActorRotation().Roll, FVector(0, 0, 1));
		FActorSpawnParameters Params = FActorSpawnParameters();
		Params.Instigator = Actor;

		// Spawn the pawn to the world
		ThePawn = GetWorld()->SpawnActor<APawn>(SpawnableClass->GetAuthoritativeClass(), Location, FRotator(0.f, 0.f, 360.f - Actor->GetActorRotation().Roll), Params);
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
