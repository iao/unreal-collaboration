// By Paul Graham <p@ul.ms>

#include "NetworkCharacter.h"
#include "NetworkPlayerController.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Components/TextRenderComponent.h"
#include "Components/BoxComponent.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"
#include "HTTPService.h"

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

ANetworkCharacter::ANetworkCharacter() {
	// Setup Defaults
	if (!TextActorClass) TextActorClass = ANetworkTextRenderActor::StaticClass();
	
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.0f, 96.0f);
	GetCapsuleComponent()->SetHiddenInGame(true, true);
	GetCapsuleComponent()->SetIsReplicated(true);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

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

	// TODO: VR Support fully :)
	// TODO: Comments!!!
	
	// Allow ticks
	PrimaryActorTick.bCanEverTick = true;

	// Setup movement
	JumpMaxHoldTime = 100000.0f;
	GetCharacterMovement()->GravityScale = 0.35f;
	GetCharacterMovement()->AirControl = 1;
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
		
	// Show or hide the the mesh based on whether or not we're using motion controllers.
	Mesh1P->SetHiddenInGame(bUsingMotionControllers, true);
	
	// Request info from Unreal Selector
	RequestInfo();
}

// Allow blueprints to ask for player info
void ANetworkCharacter::RequestInfo() {
	if (GetLocalRole() != ROLE_Authority) {
		FInfoStruct_Request info_r;
		info_r.session = AHTTPService::GetInfoJSON().session;
		FString URL = AHTTPService::GetInfoJSON().url + "/api/account/info";

		if (info_r.session != "" && URL != "") {
			AHTTPService::Info(URL, info_r, this);
		} else {
			UE_LOG(LogTemp, Warning, TEXT("Failed to get info.json! Setting defaults."));
			
			FInfoStruct_Responce responce;
			responce.username = DefaultUsername;
			responce.rank = DefaultRank;
			responce.info = DefaultInfo;
			responce.isAdmin = isAdminByDefault;
			Change(responce);
		}
	}
}

// Update the info variable from the request
void ANetworkCharacter::InfoResponce(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful) {
	if (!AHTTPService::ResponseIsValid(Response, bWasSuccessful)) {
		UE_LOG(LogTemp, Warning, TEXT("Failed to get info! Setting defaults."));

		// Send response to myself & the server
		FInfoStruct_Responce responce;
		responce.username = DefaultUsername;
		responce.rank = DefaultRank;
		responce.info = DefaultInfo;
		responce.isAdmin = isAdminByDefault;
		Change(responce);
		return;
	}

	// Send response to myself & the server
	FInfoStruct_Responce responce;
	FJsonObjectConverter::JsonObjectStringToUStruct<FInfoStruct_Responce>(Response->GetContentAsString(), &responce, 0, 0);
	Change(responce);
}

// Client-side change function
void ANetworkCharacter::Change(FInfoStruct_Responce responce) {
	// Set variables on client
	username = responce.username;
	info = responce.info;
	rank = responce.rank;
	isAdmin = responce.isAdmin;

	// Let Blueprint know, then update the server
	ServerChange(responce);
}

// Server side change function
void ANetworkCharacter::ServerChange_Implementation(FInfoStruct_Responce responce) {
	// Set variables on server
	username = responce.username;
	info = responce.info;
	rank = responce.rank;
	isAdmin = responce.isAdmin;

	// Update the controller
	ANetworkPlayerController* controller = Cast<ANetworkPlayerController>(GetController());
	if (controller) controller->isAdmin = isAdmin;

	// Call info changed for that user
	UponInfoChanged();
	
	// Update our name
	TextActor->text = FText::FromString(username);
}

// Validation functions
bool ANetworkCharacter::ServerChange_Validate(FInfoStruct_Responce responce) {
	return true;
}

void ANetworkCharacter::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	// Have the text actor follow the player
	if (GetLocalRole() == ROLE_Authority) {
		TextActor->SetActorLocation(GetActorLocation() + BoxComponent->GetRelativeLocation());
	}
	
	// Have the text actor rotate with the player
	if(HasActiveCameraComponent() && TextActor && GetWorld()->GetFirstPlayerController()) {
		FVector PlayerLocation = GetWorld()->GetFirstPlayerController()->PlayerCameraManager->GetCameraLocation();
		FRotator TextRotationAll = UKismetMathLibrary::FindLookAtRotation(PlayerLocation, TextActor->GetActorLocation());
		FRotator TextRotation = FRotator(TextActor->GetActorRotation().Pitch, 180 + TextRotationAll.Yaw, TextRotationAll.Roll);
		TextActor->SetActorRotation(TextRotation);
	}

	// Slow down when not jumping
	if (!bWasJumping && GetMovementComponent()->Velocity.Z > 1.5f) {
		GetMovementComponent()->Velocity.Z -= GetMovementComponent()->GetMaxSpeed();
	}
}

bool ANetworkCharacter::CanJumpInternal_Implementation() const {
	return true;
}

void ANetworkCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) {
	// set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	// Setup VR Reset
	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &ANetworkCharacter::OnResetVR);

	// Bind movement events
	PlayerInputComponent->BindAxis("MoveForward", this, &ANetworkCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ANetworkCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ANetworkCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ANetworkCharacter::LookUpAtRate);
}

/** Called when unpossessed */
void ANetworkCharacter::UnPossessed() {
	Super::UnPossessed();

	// Stop movement
	GetCharacterMovement()->StopMovementImmediately();
}

void ANetworkCharacter::MoveForward(float Value) {
	if (Value != 0.0f) {
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void ANetworkCharacter::MoveRight(float Value) {
	if (Value != 0.0f) {
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void ANetworkCharacter::TurnAtRate(float Rate) {
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ANetworkCharacter::LookUpAtRate(float Rate) {
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void ANetworkCharacter::OnResetVR() {
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

// Replication of info, rank, user
void ANetworkCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ANetworkCharacter, TextActor);
	DOREPLIFETIME(ANetworkCharacter, info);
	DOREPLIFETIME(ANetworkCharacter, rank);
	DOREPLIFETIME(ANetworkCharacter, username);
	DOREPLIFETIME(ANetworkCharacter, isAdmin);
}