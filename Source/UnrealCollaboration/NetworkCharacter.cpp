// By Paul Graham <p@ul.ms>

#include "NetworkCharacter.h"
#include "NetworkPlayerController.h"
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
#include "HTTPService.h"
#include "MotionControllerComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

ANetworkCharacter::ANetworkCharacter() {
	// Setup Defaults
	if (!TextActorClass) TextActorClass = ANetworkTextRenderActor::StaticClass();
	
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.0f, 96.0f);
	GetCapsuleComponent()->SetHiddenInGame(true, true);
	GetCapsuleComponent()->SetIsReplicated(true);

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetRelativeRotation(FRotator(1.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-0.5f, -4.4f, -155.7f));

	// Create a mesh component that will be used when being viewed from a '3rd person' view (when not controlling this pawn)
	Mesh3P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh3P"));
	Mesh3P->SetupAttachment(FirstPersonCameraComponent);
	Mesh3P->SetRelativeRotation(FRotator(0.0f, 270.0f, 0.0f));
	Mesh3P->SetRelativeLocation(FVector(40.0f, 0.0f, -155.7f)); // Position the mesh so that it aligns with the capsule

	// Setup BoxComponent
	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	BoxComponent->SetupAttachment(GetCapsuleComponent());
	BoxComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 100.0f)); // Position the box
	BoxComponent->SetBoxExtent(FVector(10.0f, 10.0f, 10.0f));
	BoxComponent->SetIsReplicated(true);

	// Setup motion controllers
	L_MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("L_MotionController"));
	L_MotionController->SetupAttachment(FirstPersonCameraComponent);
	L_MotionController->MotionSource = FName("Left");
	L_MotionController->bDisplayDeviceModel = true;
	L_MotionController->SetIsReplicated(true);
	
	R_MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("R_MotionController"));
	R_MotionController->SetupAttachment(FirstPersonCameraComponent);
	R_MotionController->MotionSource = FName("Right");
	R_MotionController->bDisplayDeviceModel = true;

	// TODO: Fix VR Rotation issue
	// TODO: Comments!!!
	
	// Allow ticks
	PrimaryActorTick.bCanEverTick = true;

	// Setup movement
	JumpMaxHoldTime = 100000.0f;
	GetCharacterMovement()->GravityScale = 0.35f;
	GetCharacterMovement()->AirControl = 1;
}

bool ANetworkCharacter::isInVR() {
	// Return if we are in VR
	FName HMDName = UHeadMountedDisplayFunctionLibrary::GetHMDDeviceName();
	return !(HMDName.IsEqual(FName("None")) || HMDName.IsEqual(FName("Unknown")));
}

void ANetworkCharacter::BeginPlay() {
	// Call the base class  
	Super::BeginPlay();

	// Spawn the text actor
	if (GetLocalRole() == ROLE_Authority) {
		TextActor = GetWorld()->SpawnActor<ANetworkTextRenderActor>(TextActorClass->GetAuthoritativeClass(), BoxComponent->GetComponentLocation(), BoxComponent->GetComponentRotation());
	}
	
	// Show or hide meshes based on if we are locally controlling the character
	Mesh3P->SetVisibility(!IsLocallyControlled());
	Mesh1P->SetVisibility(IsLocallyControlled());
		
	// Allow for free camera movement if we are in VR.
	if(isInVR()) {
		// Move the camera to the center so we dont get sick when rotating
		FirstPersonCameraComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 64.f)); // Position the camera to the center
		Mesh1P->SetVisibility(false); // Cannot see arms in VR - Only controllers
		Mesh3P->SetRelativeLocation(FVector(0.44f, -1.75f, -155.7f)); // Move mesh back to same position
	}

	// Show/hide motion controllers
	L_MotionController->SetHiddenInGame(isInVR());
	R_MotionController->SetHiddenInGame(isInVR());
	
	// Request info from Unreal Selector
	RequestInfo();
}

// Allow blueprints to ask for player info
void ANetworkCharacter::RequestInfo() {
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
void ANetworkCharacter::InfoResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful) {
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
void ANetworkCharacter::Change(FInfoStruct_Response response) {
	// Set variables on client
	username = response.username;
	info = response.info;
	rank = response.rank;
	isAdmin = response.isAdmin;

	// Let Blueprint know, then update the server
	ServerChange(response);
}

// Server side change function
void ANetworkCharacter::ServerChange_Implementation(FInfoStruct_Response response) {
	// Set variables on server
	username = response.username;
	info = response.info;
	rank = response.rank;
	isAdmin = response.isAdmin;

	// Update the controller
	ANetworkPlayerController* controller = Cast<ANetworkPlayerController>(GetController());
	if (controller) controller->isAdmin = isAdmin;

	// Call info changed for that user
	UponInfoChanged();
	
	// Update our name
	TextActor->text = FText::FromString(username);
}

// Validation functions
bool ANetworkCharacter::ServerChange_Validate(FInfoStruct_Response response) {
	return true;
}

void ANetworkCharacter::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	// Have the text actor follow the player
	if (GetLocalRole() == ROLE_Authority) {
		TextActor->SetActorLocation(GetActorLocation() + BoxComponent->GetRelativeLocation());

		// Slow down when not jumping
		if (!bWasJumping && GetMovementComponent()->Velocity.Z > 1.5f) {
			GetMovementComponent()->Velocity.Z -= GetMovementComponent()->GetMaxSpeed();
		}

		// Slow down when moving too fast
		if (isInVR()) {
			if (GetMovementComponent()->Velocity.X > XYMaxSpeed) {
				GetMovementComponent()->Velocity.X = XYMaxSpeed - (XYMaxSpeed / 10);
			} if (GetMovementComponent()->Velocity.X < -XYMaxSpeed) {
				GetMovementComponent()->Velocity.X = -XYMaxSpeed + (XYMaxSpeed / 10);
			} if (GetMovementComponent()->Velocity.Y > XYMaxSpeed) {
				GetMovementComponent()->Velocity.Y = XYMaxSpeed - (XYMaxSpeed / 10);
			} if (GetMovementComponent()->Velocity.Y < -XYMaxSpeed) {
				GetMovementComponent()->Velocity.Y = -XYMaxSpeed + (XYMaxSpeed / 10);
			} if (GetMovementComponent()->Velocity.Z > ZMaxSpeed) {
				GetMovementComponent()->Velocity.Z = ZMaxSpeed - (ZMaxSpeed / 10);
			} if (GetMovementComponent()->Velocity.Z < -ZMaxSpeed) {
				GetMovementComponent()->Velocity.Z = -ZMaxSpeed + (ZMaxSpeed / 10);
			}
		}
	}
	
	// Have the text actor rotate with the player
	if(HasActiveCameraComponent() && TextActor && GetWorld()->GetFirstPlayerController()) {
		FVector PlayerLocation = GetWorld()->GetFirstPlayerController()->PlayerCameraManager->GetCameraLocation();
		FRotator TextRotationAll = UKismetMathLibrary::FindLookAtRotation(PlayerLocation, TextActor->GetActorLocation());
		FRotator TextRotation = FRotator(TextActor->GetActorRotation().Pitch, 180 + TextRotationAll.Yaw, TextRotationAll.Roll);
		TextActor->SetActorRotation(TextRotation);
	}
}

bool ANetworkCharacter::CanJumpInternal_Implementation() const {
	return true;
}

void ANetworkCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) {
	// set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind independent events
	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	// Bind keyboard movement events
	PlayerInputComponent->BindAxis("MoveForward", this, &ANetworkCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ANetworkCharacter::MoveRight);

	// Bind mouse movement events
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	
	// Bind non-rv events
	if(isInVR()) {
		// Setup VR Reset
		PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &ANetworkCharacter::OnResetVR);

		// Setup VR movement
		PlayerInputComponent->BindAxis("Move", this, &ANetworkCharacter::MoveVR);
		
		// Setup VR turning
		PlayerInputComponent->BindAction("TurnLeft", IE_Pressed, this, &ANetworkCharacter::TurnLeft);
		PlayerInputComponent->BindAction("TurnRight", IE_Pressed, this, &ANetworkCharacter::TurnRight);

	}
}

/** Called when unpossessed */
void ANetworkCharacter::UnPossessed() {
	Super::UnPossessed();

	// Stop movement
	GetCharacterMovement()->StopMovementImmediately();
}

void ANetworkCharacter::MoveVR_Implementation(float Value) {
	if (Value != 0.0f) {
		// Launch the player in the direction of the left motion controller
		APlayerCameraManager* manager = GetWorld()->GetFirstPlayerController()->PlayerCameraManager;
		FVector launch = L_MotionController->GetComponentRotation().RotateVector(manager->GetActorForwardVector()) * LaunchSpeed * Value;
		LaunchCharacter(launch, false, !(GetMovementComponent()->Velocity.Z > 0.0f));
	}
}

bool ANetworkCharacter::MoveVR_Validate(float Value) {
	return true;
}

void ANetworkCharacter::TurnLeft() {
	// Turn left 45 degrees
	UE_LOG(LogTemp, Warning, TEXT("TurnLeft"));
	GetWorld()->GetFirstPlayerController()->AddYawInput( -45.f / GetWorld()->GetFirstPlayerController()->InputYawScale);
}

void ANetworkCharacter::TurnRight() {
	// Turn right 45 degrees
	UE_LOG(LogTemp, Warning, TEXT("TurnRight"));
	GetWorld()->GetFirstPlayerController()->AddYawInput(45.f / GetWorld()->GetFirstPlayerController()->InputYawScale);
}

void ANetworkCharacter::MoveForward(float Value) {
	if (Value != 0.0f) {
		// Add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void ANetworkCharacter::MoveRight(float Value) {
	if (Value != 0.0f) {
		// Add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void ANetworkCharacter::OnResetVR() {
	// Reset VR
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

// Replication of variables
void ANetworkCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ANetworkCharacter, TextActor);
	DOREPLIFETIME(ANetworkCharacter, info);
	DOREPLIFETIME(ANetworkCharacter, rank);
	DOREPLIFETIME(ANetworkCharacter, username);
	DOREPLIFETIME(ANetworkCharacter, isAdmin);
	DOREPLIFETIME(ANetworkCharacter, L_MotionController);
}