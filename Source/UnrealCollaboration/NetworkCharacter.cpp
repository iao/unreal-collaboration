// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "NetworkCharacter.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "MotionControllerComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

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

	// TODO: Setup Mesh3P

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

	// Show or hide the the mesh based on whether or not we're using motion controllers.
	Mesh1P->SetHiddenInGame(bUsingMotionControllers, true);
}

void ANetworkCharacter::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	if (!IsLocallyControlled()) {
		FRotator NewRot = FirstPersonCameraComponent->GetRelativeRotation();
		NewRot.Pitch = RemoteViewPitch * 360.0f / 255.0f;

		FirstPersonCameraComponent->SetRelativeRotation(NewRot);
	}

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
	PlayerInputComponent->BindAxis("MoveUp", this, &ANetworkCharacter::MoveUp);
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

void ANetworkCharacter::MoveUp(float Value) {
	if (Value != 0.0f) {
		// add movement in that direction
		AddMovementInput(GetActorUpVector(), Value);
	}
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