// By Paul Graham <p@ul.ms>

#include "NetworkPlayerController.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine.h"
#include "HTTPService.h"
#include "Net/UnrealNetwork.h"
#include "Math/UnrealMathUtility.h"

// Constructor
ANetworkPlayerController::ANetworkPlayerController(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
	// Setup Defaults
	if (!SpawnableClass) SpawnableClass = ASignPawn::StaticClass();
	isPawn = false;
	isHidden = false;
	counter_max = (60 * 1000 * 15); // Default is 15 minutes between calls
	random_num = FMath::RandRange(1, 100);
}

// Called after constructor
void ANetworkPlayerController::BeginPlay() {
	// Get how long between /keepalive calls
	if (GetLocalRole() != ROLE_Authority) {
		FTimeStruct_Request time_r;
		time_r.session = AHTTPService::GetInfoJSON().session;
		FString timeURL = AHTTPService::GetInfoJSON().url + "/api/project/time";

		if (time_r.session != "" && timeURL != "") {
			AHTTPService::Time(timeURL, time_r, this);
		}
	}
}

// Simple warning if we failed to keep alive
void ANetworkPlayerController::KeepAliveResponce(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful) {
	if (!AHTTPService::ResponseIsValid(Response, bWasSuccessful)) {
		UE_LOG(LogTemp, Warning, TEXT("Failed to keep server alive! May be kicked soon!"));
	}
}

// Update the counter_max variable from the request
void ANetworkPlayerController::TimeResponce(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful) {
	if (!AHTTPService::ResponseIsValid(Response, bWasSuccessful)) {
		UE_LOG(LogTemp, Warning, TEXT("Failed to get time! Assuming time is 15 minutes."));
		return;
	}

	FTimeStruct_Responce responce;
	FJsonObjectConverter::JsonObjectStringToUStruct<FTimeStruct_Responce>(Response->GetContentAsString(), &responce, 0, 0);
	counter_max = responce.time * 1000;
}

// Check if the player matches the admin rank
bool ANetworkPlayerController::HasSelectorAuthority() {
	// Get a reference to the current actor
	ANetworkCharacter* Actor = Cast<ANetworkCharacter>(GetPawnOrSpectator());

	// Check if they are admin
	if (Actor) return Actor->isAdmin;
	else return false;
}

// Setup inputs for the controller
void ANetworkPlayerController::SetupInputComponent() {
	if (InputComponent == NULL) {
		InputComponent = NewObject<UInputComponent>(this, TEXT("Cool_InputComponent"));
		InputComponent->RegisterComponent();
	}

	check(InputComponent);
	if (InputComponent) {
		// Spawn a sign
		InputComponent->BindAction("Spawn", IE_Pressed, this, &ANetworkPlayerController::Spawn);
		InputComponent->BindAction("Hide", IE_Pressed, this, &ANetworkPlayerController::Hide);
	}
}

// Send keep-alive to the selector server
void ANetworkPlayerController::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	// Ensure the server stays alive
	if (GetLocalRole() != ROLE_Authority) {
		counter += (DeltaTime * 1000);
		if (counter > 2 * counter_max) counter = 0;

		// Call /keepalive every counter_max milliseconds, with a random offset
		if ((counter - random_num) % counter_max <= (DeltaTime * 1000)) {
			FKeepAliveStruct keepalive;
			keepalive.session = AHTTPService::GetInfoJSON().session;
			keepalive.title = AHTTPService::GetInfoJSON().title;
			FString URL = AHTTPService::GetInfoJSON().url + "/api/project/keepalive";

			if (keepalive.session != "" && keepalive.title != "" && URL != "") {
				AHTTPService::KeepAlive(URL, keepalive, this);
			}

			// Hide our text actor
			ANetworkCharacter* Actor = Cast<ANetworkCharacter>(GetPawnOrSpectator());
			if (Actor && Actor->TextActor) Actor->TextActor->HideActor(true);
		}
	}
}

// Client side spawn function
void ANetworkPlayerController::Spawn() {
	if (!isHidden) ServerSpawn();
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

// Delete the pawn
void ANetworkPlayerController::Delete() {
	if (isPawn) ServerDelete();
}

// Server side spawn function
void ANetworkPlayerController::ServerSpawn_Implementation() {
	if (isPawn) {
		// Find the instigator and possess
		ANetworkCharacter* Actor = Cast<ANetworkCharacter>(ThePawn->GetInstigator());
		ThePawn->SetInstigator(NULL);

		// Possess and update state
		Possess(Actor);
		
		isPawn = false;
	} else {
		// Spawn the blueprinted pawn
		// Get a reference to the current actor (owner)
		ANetworkCharacter* Actor = Cast<ANetworkCharacter>(GetPawnOrSpectator());

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
	ANetworkCharacter* Actor = Cast<ANetworkCharacter>(ThePawn->GetInstigator());

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
