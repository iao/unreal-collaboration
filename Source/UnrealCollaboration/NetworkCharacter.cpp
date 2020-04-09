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

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

ANetworkCharacter::ANetworkCharacter() {
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.0f, 96.0f);
	if (IsRunningClientOnly()) GetCapsuleComponent()->SetHiddenInGame(false, true);
	else GetCapsuleComponent()->SetHiddenInGame(true, true);

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

	// Setup BoxComponent
	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	BoxComponent->SetupAttachment(GetCapsuleComponent());
	BoxComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 90.0f)); // Position the box
	BoxComponent->SetIsReplicated(true);

	// Setup TextRenderActor
	// TODO: New users are not given correct name
	TextRenderComponent = CreateDefaultSubobject<UTextRenderComponent>(TEXT("TextRenderComponent"));
	TextRenderComponent->SetupAttachment(BoxComponent);
	TextRenderComponent->SetIsReplicated(true); // TODO: Something is wrong if we have no info.json?
	TextRenderComponent->SetHorizontalAlignment(EHTA_Center);

	// Allow ticks
	PrimaryActorTick.bCanEverTick = true;

	// TODO: Setup movement
	JumpMaxHoldTime = 100000.0f;
	GetCharacterMovement()->GravityScale = 0.35f;
	GetCharacterMovement()->AirControl = 1;
}

void ANetworkCharacter::BeginPlay() {
	// Call the base class  
	Super::BeginPlay();

	// Show or hide meshes based on if we are locally controlling the character
	if (IsLocallyControlled()) {
		if(TextRenderComponent) TextRenderComponent->SetVisibility(false);
		if(Mesh3P) Mesh3P->SetVisibility(false);
		if(Mesh1P) Mesh1P->SetVisibility(true);
	} else {
		if (TextRenderComponent) TextRenderComponent->SetVisibility(true);
		if (Mesh3P) Mesh3P->SetVisibility(true);
		if (Mesh1P) Mesh1P->SetVisibility(false);
	}

	// Show or hide the the mesh based on whether or not we're using motion controllers.
	//Mesh1P->SetHiddenInGame(bUsingMotionControllers, true);
}

void ANetworkCharacter::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	// Set the text to the controllers text
	if (GetLocalRole() != ROLE_Authority) {
		ANetworkPlayerController* controller = Cast<ANetworkPlayerController>(GetWorld()->GetFirstPlayerController());
		if (TextRenderComponent) TextRenderComponent->SetText(FText::FromString(controller->username));
	}
	
	// Replicate the pitch of the camera
	if (!IsLocallyControlled()) {
		FRotator NewRot = FirstPersonCameraComponent->GetRelativeRotation();
		NewRot.Pitch = RemoteViewPitch * 360.0f / 255.0f;

		FirstPersonCameraComponent->SetRelativeRotation(NewRot);
	}

	// Face name toward player
	if(HasActiveCameraComponent()) {
		FVector PlayerLocation = GetWorld()->GetFirstPlayerController()->PlayerCameraManager->GetCameraLocation();
		FRotator NewRotationAll = UKismetMathLibrary::FindLookAtRotation(PlayerLocation, GetActorLocation());
		FRotator NewRotation = FRotator(GetActorRotation().Pitch, 180 + NewRotationAll.Yaw, NewRotationAll.Roll);
		if (TextRenderComponent) TextRenderComponent->SetWorldRotation(NewRotation);
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