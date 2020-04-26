// By Paul Graham <p@ul.ms>

#include "BaseNetworkPlayerController.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine.h"
#include "../HTTP/HTTPService.h"
#include "Net/UnrealNetwork.h"
#include "Math/UnrealMathUtility.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Kismet/KismetMathLibrary.h"

// Constructor
ABaseNetworkPlayerController::ABaseNetworkPlayerController(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
	// Setup Defaults
	isHidden = false;
	counter_max = (60 * 15); // Default is 15 minutes between calls
	random_num = FMath::RandRange(0, 30); // Clients will call at an offset of up to 30 seconds
}

// Called after constructor
void ABaseNetworkPlayerController::BeginPlay() {
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
void ABaseNetworkPlayerController::KeepAliveResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful) {
	// Log if unsuccessful
	if (!AHTTPService::ResponseIsValid(Response, bWasSuccessful)) {
		UE_LOG(LogTemp, Warning, TEXT("Failed to keep server alive! May be kicked soon!"));
	}
}

// Update the counter_max variable from the request
void ABaseNetworkPlayerController::TimeResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful) {
	// Log if unsuccessful
	if (!AHTTPService::ResponseIsValid(Response, bWasSuccessful)) {
		UE_LOG(LogTemp, Warning, TEXT("Failed to get time! Assuming time is 15 minutes."));
		return;
	}

	// Set timer to send /keepalive requests every time seconds, with a random offset (so that many clients don't call at the same time)
	FTimeStruct_Response responce;
	FJsonObjectConverter::JsonObjectStringToUStruct<FTimeStruct_Response>(Response->GetContentAsString(), &responce, 0, 0);
	counter_max = responce.time;
	GetWorldTimerManager().SetTimer(KeepAliveTimer, this, &ABaseNetworkPlayerController::KeepAlive, counter_max, true, random_num);
}

// Check if the player matches the admin rank
bool ABaseNetworkPlayerController::HasSelectorAuthority() {
	return isAdmin;
}

// Setup inputs for the controller
void ABaseNetworkPlayerController::SetupInputComponent() {
	if (InputComponent == NULL) {
		InputComponent = NewObject<UInputComponent>(this, TEXT("Cool_InputComponent"));
		InputComponent->RegisterComponent();
	}
}

// Hide our own text actor
void ABaseNetworkPlayerController::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
	
	if(GetLocalRole() != ROLE_Authority) {
		// Hide our own text actor
		ABaseNetworkCharacter* Actor = Cast<ABaseNetworkCharacter>(GetPawnOrSpectator());
		if (Actor && Actor->TextActor) Actor->TextActor->HideActor(true);
	}
}

// Called every counter_max seconds
void ABaseNetworkPlayerController::KeepAlive() {
	if (GetLocalRole() != ROLE_Authority) {
		// Call /keepalive every counter_max milliseconds, with a random offset
		FKeepAliveStruct keepalive;
		keepalive.session = AHTTPService::GetInfoJSON().session;
		keepalive.title = AHTTPService::GetInfoJSON().title;
		FString URL = AHTTPService::GetInfoJSON().url + "/api/project/keepalive";

		if (keepalive.session != "" && keepalive.title != "" && URL != "") {
			AHTTPService::KeepAlive(URL, keepalive, this);
		}
	}
}

// Request to quit the game
void ABaseNetworkPlayerController::Quit() {
	// Ask server to destroy us
	ServerQuit();

	// Request exit from the engine
	FGenericPlatformMisc::RequestExit(false);
}

// Destroy current actor so they leave the game
void ABaseNetworkPlayerController::ServerQuit_Implementation() {
	ABaseNetworkCharacter* Actor = Cast<ABaseNetworkCharacter>(GetPawnOrSpectator());
	GetWorld()->DestroyActor(Actor->TextActor);
	GetWorld()->DestroyActor(Actor);
}

bool ABaseNetworkPlayerController::ServerQuit_Validate() {
	return true;
}

// Replication of isAdmin
void ABaseNetworkPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABaseNetworkPlayerController, isAdmin);
}
