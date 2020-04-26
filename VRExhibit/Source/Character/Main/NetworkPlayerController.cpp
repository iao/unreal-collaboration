// By Paul Graham <p@ul.ms>


#include "NetworkPlayerController.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine.h"
#include "Net/UnrealNetwork.h"
#include "Math/UnrealMathUtility.h"
#include "HeadMountedDisplayFunctionLibrary.h"

ANetworkPlayerController::ANetworkPlayerController(const FObjectInitializer& ObjectInitializer) : ABaseNetworkPlayerController(ObjectInitializer) {
	// Setup Defaults
	if (!SpawnableClass) SpawnableClass = ASignPawn::StaticClass();
	isPawn = false;
}

void ANetworkPlayerController::SetupInputComponent() {
	Super::SetupInputComponent();

	// Bind non-vr actions
	check(InputComponent);
	if (InputComponent) {
		InputComponent->BindAction("Spawn", IE_Pressed, this, &ANetworkPlayerController::Spawn);
		InputComponent->BindAction("Hide", IE_Pressed, this, &ABaseNetworkPlayerController::Hide);
		InputComponent->BindAction("Quit", IE_Pressed, this, &ABaseNetworkPlayerController::Quit);
	}
}

// Hide all signs for the client
void ANetworkPlayerController::Hide() {
	if (!isPawn) {
		// Toggle isHidden
		isHidden = !isHidden;

		// Set all actors to hide or not
		for (TActorIterator<ASignPawn> CubeItr(GetWorld()); CubeItr; ++CubeItr) {
			ASignPawn* ItrPawn = Cast<ASignPawn>(*CubeItr);
			if (ItrPawn) {
				ItrPawn->HideActor(isHidden);
			}
		}
	}
}

// Client side spawn function
void ANetworkPlayerController::Spawn() {
	if (!isHidden) ServerSpawn();
}

// Delete the pawn
void ANetworkPlayerController::Delete() {
	if (isPawn) ServerDelete();
}

// Server side spawn function
void ANetworkPlayerController::ServerSpawn_Implementation() {
	if (isPawn) {
		// Find the instigator and possess
		ABaseNetworkCharacter* Actor = Cast<ANetworkCharacter>(ThePawn->GetInstigator());
		ThePawn->SetInstigator(NULL);

		// Possess and update state
		Possess(Actor);

		isPawn = false;
	} else {
		// Spawn the blueprinted pawn
		// Get a reference to the current actor (owner)
		ABaseNetworkCharacter* Actor = Cast<ANetworkCharacter>(GetPawnOrSpectator());

		// Find the pawn without an instigator closest to Actor
		ASignPawn* FoundPawn = NULL;
		float min = FindDistance + 1.0f;
		for (TActorIterator<ASignPawn> CubeItr(GetWorld()); CubeItr; ++CubeItr) {
			ASignPawn* ItrPawn = Cast<ASignPawn>(*CubeItr);
			if (ItrPawn) {
				float distance = FVector::Dist(ItrPawn->GetActorLocation(), Actor->GetActorLocation());
				if (distance <= FindDistance && !ItrPawn->GetInstigator() && distance < min) {
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
		FVector NewLocation = PlayerCameraManager->GetCameraLocation() + (PlayerCameraManager->GetCameraRotation().RotateVector(PlayerCameraManager->GetActorForwardVector()) * SpawnDistance);
		FVector Location = FVector(NewLocation.X, NewLocation.Y, Actor->GetActorLocation().Z);
		FActorSpawnParameters Params = FActorSpawnParameters();
		Params.Instigator = Actor;

		// Spawn the pawn to the world & update status
		ThePawn = GetWorld()->SpawnActor<ASignPawn>(SpawnableClass->GetAuthoritativeClass(), Location, FRotator(0.0f, 0.0f, 0.0f), Params);
		UE_LOG(LogTemp, Warning, TEXT("Successfully spawned a sign @ %f, %f"), ThePawn->GetActorLocation().X, ThePawn->GetActorLocation().Y);
		Possess(ThePawn);
		isPawn = true;
	}
}

// Server side delete function
void ANetworkPlayerController::ServerDelete_Implementation() {
	// Find the instigator and possess
	ABaseNetworkCharacter* Actor = Cast<ANetworkCharacter>(ThePawn->GetInstigator());

	// Possess and update state
	Possess(Actor);
	isPawn = false;

	// Delete the pawn & text actors
	GetWorld()->DestroyActor(ThePawn->TextActor);
	GetWorld()->DestroyActor(ThePawn);
}

bool ANetworkPlayerController::ServerSpawn_Validate() {
	return true;
}

bool ANetworkPlayerController::ServerDelete_Validate() {
	return true;
}

// Called when isPawn replicates
void ANetworkPlayerController::OnRep_Pawn() {
	bShowMouseCursor = isPawn;
	Cast<ANetworkHUD>(GetHUD())->SetVisible(isPawn);
}

// Replication of isPawn
void ANetworkPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ANetworkPlayerController, isPawn);
}
