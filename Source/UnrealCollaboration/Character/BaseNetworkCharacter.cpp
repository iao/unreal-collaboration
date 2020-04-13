// By Paul Graham <p@ul.ms>

#include "BaseNetworkCharacter.h"
#include "BaseNetworkPlayerController.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Components/BoxComponent.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"
#include "../HTTP/HTTPService.h"
#include "MotionControllerComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

ABaseNetworkCharacter::ABaseNetworkCharacter() {
	// Setup Defaults
	if (!TextActorClass) TextActorClass = ANetworkTextRenderActor::StaticClass();
	
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.0f, 96.0f);
	GetCapsuleComponent()->SetHiddenInGame(true, true);
	GetCapsuleComponent()->SetIsReplicated(true);
	
	// Setup BoxComponent
	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	BoxComponent->SetupAttachment(GetCapsuleComponent());
	BoxComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 100.0f)); // Position the box
	BoxComponent->SetBoxExtent(FVector(10.0f, 10.0f, 10.0f));
	BoxComponent->SetIsReplicated(true);
	
	// Allow ticks
	PrimaryActorTick.bCanEverTick = true;

	// Setup movement
	JumpMaxHoldTime = 100000.0f;
	GetCharacterMovement()->GravityScale = 0.35f;
	GetCharacterMovement()->AirControl = 1;
}

void ABaseNetworkCharacter::BeginPlay() {
	// Call the base class  
	Super::BeginPlay();

	// Spawn the text actor
	if (GetLocalRole() == ROLE_Authority) {
		TextActor = GetWorld()->SpawnActor<ANetworkTextRenderActor>(TextActorClass->GetAuthoritativeClass(), BoxComponent->GetComponentLocation(), BoxComponent->GetComponentRotation());
	}
		
	// Request info from Unreal Selector
	RequestInfo();
}

// Allow blueprints to ask for player info
void ABaseNetworkCharacter::RequestInfo() {
	if (GetLocalRole() != ROLE_Authority) {
		// Setup the request
		FInfoStruct_Request info_r;
		info_r.session = AHTTPService::GetInfoJSON().session;
		FString URL = AHTTPService::GetInfoJSON().url + "/api/account/info";

		// Deal with not being able to find info.json
		if (info_r.session != "" && URL != "") {
			AHTTPService::Info(URL, info_r, this);
		} else {
			// If we can't, set values to default
			UE_LOG(LogTemp, Warning, TEXT("Failed to get info.json! Setting defaults."));
			
			FInfoStruct_Response response;
			response.username = DefaultUsername;
			response.rank = DefaultRank;
			response.info = DefaultInfo;
			response.isAdmin = isAdminByDefault;
			Change(response);
		}
	}
}

// Update the info variable from the request
void ABaseNetworkCharacter::InfoResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful) {
	if (!AHTTPService::ResponseIsValid(Response, bWasSuccessful)) {
		UE_LOG(LogTemp, Warning, TEXT("Failed to get info! Setting defaults."));

		// Send response to myself & the server
		FInfoStruct_Response response;
		response.username = DefaultUsername;
		response.rank = DefaultRank;
		response.info = DefaultInfo;
		response.isAdmin = isAdminByDefault;
		Change(response);
		return;
	}

	// Send response to myself & the server
	FInfoStruct_Response response;
	FJsonObjectConverter::JsonObjectStringToUStruct<FInfoStruct_Response>(Response->GetContentAsString(), &response, 0, 0);
	Change(response);
}

// Client-side change function
void ABaseNetworkCharacter::Change(FInfoStruct_Response response) {
	// Set variables on client
	username = response.username;
	info = response.info;
	rank = response.rank;
	isAdmin = response.isAdmin;

	// Let Blueprint know, then update the server
	ServerChange(response);
}

// Server side change function
void ABaseNetworkCharacter::ServerChange_Implementation(FInfoStruct_Response response) {
	// Set variables on server
	username = response.username;
	info = response.info;
	rank = response.rank;
	isAdmin = response.isAdmin;

	// Update the controller
	ABaseNetworkPlayerController* controller = Cast<ABaseNetworkPlayerController>(GetController());
	if (controller) controller->isAdmin = isAdmin;

	// Call info changed for that user
	UponInfoChanged();
	
	// Update our name
	TextActor->text = FText::FromString(username);
}

// Validation functions
bool ABaseNetworkCharacter::ServerChange_Validate(FInfoStruct_Response response) {
	return true;
}

void ABaseNetworkCharacter::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	// Have the text actor follow the player
	if (GetLocalRole() == ROLE_Authority) {
		if(TextActor) TextActor->SetActorLocation(GetActorLocation() + BoxComponent->GetRelativeLocation());

		// Slow down when not jumping
		if (!bWasJumping && GetMovementComponent()->Velocity.Z > 1.5f) {
			GetMovementComponent()->Velocity.Z -= GetMovementComponent()->GetMaxSpeed();
		}
	}
	
	// Have the text actor rotate with the player
	if(TextActor && GetWorld()->GetFirstPlayerController()) {
		FVector PlayerLocation = GetWorld()->GetFirstPlayerController()->PlayerCameraManager->GetCameraLocation();
		FRotator TextRotationAll = UKismetMathLibrary::FindLookAtRotation(PlayerLocation, TextActor->GetActorLocation());
		FRotator TextRotation = FRotator(TextActor->GetActorRotation().Pitch, 180 + TextRotationAll.Yaw, TextRotationAll.Roll);
		TextActor->SetActorRotation(TextRotation);
	}
}

bool ABaseNetworkCharacter::CanJumpInternal_Implementation() const {
	return true;
}

void ABaseNetworkCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) {
	// set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind independent events
	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	// Bind keyboard movement events
	PlayerInputComponent->BindAxis("MoveForward", this, &ABaseNetworkCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ABaseNetworkCharacter::MoveRight);

	// Bind mouse movement events
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
}

void ABaseNetworkCharacter::TurnLeft() {
	// Turn left 45 degrees
	UE_LOG(LogTemp, Warning, TEXT("TurnLeft"));
	GetWorld()->GetFirstPlayerController()->AddYawInput( -45.f / GetWorld()->GetFirstPlayerController()->InputYawScale);
}

void ABaseNetworkCharacter::TurnRight() {
	// Turn right 45 degrees
	UE_LOG(LogTemp, Warning, TEXT("TurnRight"));
	GetWorld()->GetFirstPlayerController()->AddYawInput(45.f / GetWorld()->GetFirstPlayerController()->InputYawScale);
}

void ABaseNetworkCharacter::MoveForward(float Value) {
	if (Value != 0.0f) {
		// Add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void ABaseNetworkCharacter::MoveRight(float Value) {
	if (Value != 0.0f) {
		// Add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

// Replication of variables
void ABaseNetworkCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABaseNetworkCharacter, TextActor);
	DOREPLIFETIME(ABaseNetworkCharacter, info);
	DOREPLIFETIME(ABaseNetworkCharacter, rank);
	DOREPLIFETIME(ABaseNetworkCharacter, username);
	DOREPLIFETIME(ABaseNetworkCharacter, isAdmin);
}