// Fill out your copyright notice in the Description page of Project Settings.

#include "MyPlayerController.h"
#include "ConstructorHelpers.h"
#include "Engine.h"
#include "UnrealNetwork.h"

// Constructor
AMyPlayerController::AMyPlayerController(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
	isPawn = false;
	isHidden = false;
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
		InputComponent->BindAction("Hide", IE_Pressed, this, &AMyPlayerController::Hide);
	}
}

// Client side spawn function
void AMyPlayerController::Spawn() {
	if(!isHidden) ServerSpawn();
}

// Hide all signs for the client
void AMyPlayerController::Hide() {
	if (!isPawn) {
		// Toggle isHidden
		isHidden = !isHidden;

		// Set all actors to hide or not
		for (TActorIterator<ACubePawn> CubeItr(GetWorld()); CubeItr; ++CubeItr) {
			ACubePawn* ItrPawn = Cast<ACubePawn>(*CubeItr);
			if (ItrPawn) {
				ItrPawn->HideActor(isHidden);
			}
		}
	}
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

		// Find the pawn without an instigator closest to Actor
		ACubePawn* FoundPawn = NULL;
		float min = FindDistance + 1.0f;
		for(TActorIterator<ACubePawn> CubeItr(GetWorld()); CubeItr; ++CubeItr) {
			ACubePawn* ItrPawn = Cast<ACubePawn>(*CubeItr);
			if(ItrPawn) {
				float distance = FVector::Dist(ItrPawn->GetActorLocation(), Actor->GetActorLocation());
				if(distance <= FindDistance && !ItrPawn->GetInstigator() && distance < min) {
					min = distance;
					FoundPawn = ItrPawn;
				}
			}
		}

		// If we find a pawn, then set its instigator and possess it
		if (min < FindDistance + 1.0f) {
			ThePawn = FoundPawn;
			ThePawn->SetInstigator(Actor);

			// Possess, update state and return
			Possess(ThePawn);
			isPawn = true;
			return;
		}
		
		// Otherwise Make the pawn from blueprinted pawn
		// Set the location etc of the new pawn
		const FVector Location = Actor->GetActorLocation() + FVector(0.f, 500.f, 0).RotateAngleAxis(Actor->GetActorRotation().Roll, FVector(0, 0, 1));
		FActorSpawnParameters Params = FActorSpawnParameters();
		Params.Instigator = Actor;

		// Spawn the pawn to the world & update status
		ThePawn = GetWorld()->SpawnActor<APawn>(SpawnableClass->GetAuthoritativeClass(), Location, FRotator(0.f, 0.f, 360.f - Actor->GetActorRotation().Roll), Params);
		Possess(ThePawn);
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
