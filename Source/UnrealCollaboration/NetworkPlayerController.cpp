// Fill out your copyright notice in the Description page of Project Settings.

#include "NetworkPlayerController.h"
#include "ConstructorHelpers.h"
#include "Engine.h"
#include "HTTPService.h"
#include "UnrealNetwork.h"
#include "Math/UnrealMathUtility.h"

// Constructor
ANetworkPlayerController::ANetworkPlayerController(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
	isPawn = false;
	isHidden = false;
	counter_max = (60 * 60 * 1); // TODO: Allow for this to be set via config JSON
	random_num = FMath::RandRange(1, 100);
}

// Simple warning if we failed to keep alive
void ANetworkPlayerController::KeepAliveResponce(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful) {
	if (!AHTTPService::ResponseIsValid(Response, bWasSuccessful)) {
		UE_LOG(LogTemp, Warning, TEXT("Failed to keep server alive! May be kicked soon!"));
	}
}

// Allow blueprints to ask for player info
void ANetworkPlayerController::RequestInfo() {
	if (GetLocalRole() != ROLE_Authority) {
		FInfoStruct_Request info_r;
		info_r.session = AHTTPService::GetInfoJSON().session;
		FString URL = AHTTPService::GetInfoJSON().url + "/api/account/info";

		if (info_r.session != "" && URL != "") {
			AHTTPService::Info(URL, info_r, this);
		}
	}
}

// Update the info variable from the request
void ANetworkPlayerController::InfoResponce(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful) {
	if (!AHTTPService::ResponseIsValid(Response, bWasSuccessful)) {
		UE_LOG(LogTemp, Warning, TEXT("Failed to get info! Assuming info is 1."));
		info = 1;
		UponInfoChanged();
		return;
	}

	FInfoStruct_Responce responce;
	FJsonObjectConverter::JsonObjectStringToUStruct<FInfoStruct_Responce>(Response->GetContentAsString(), &responce, 0, 0);
	info = responce.info;
	rank = responce.rank;
	UponInfoChanged();
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

	if (GetLocalRole() != ROLE_Authority) {
		counter++;
		if (counter > 3 * counter_max) counter = 0;
		if ((counter - random_num) % counter_max == 0) {
			FKeepAliveStruct keepalive;
			keepalive.session = AHTTPService::GetInfoJSON().session;
			keepalive.title = AHTTPService::GetInfoJSON().title;
			FString URL = AHTTPService::GetInfoJSON().url + "/api/project/keepalive";

			if (keepalive.session != "" && keepalive.title != "" && URL != "") {
				AHTTPService::KeepAlive(URL, keepalive, this);
			}
		}
	}
}

// Client side spawn function
void ANetworkPlayerController::Spawn() {
	if(!isHidden) ServerSpawn();
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
		for(TActorIterator<ASignPawn> CubeItr(GetWorld()); CubeItr; ++CubeItr) {
			ASignPawn* ItrPawn = Cast<ASignPawn>(*CubeItr);
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
bool ANetworkPlayerController::ServerSpawn_Validate() {
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
