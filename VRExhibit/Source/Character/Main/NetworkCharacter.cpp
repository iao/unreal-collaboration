// By Paul Graham <p@ul.ms>

#include "NetworkCharacter.h"
#include "../BaseNetworkCharacter.h"
#include "../BaseNetworkPlayerController.h"
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
#include "../../HTTP/HTTPService.h"
#include "MotionControllerComponent.h"

ANetworkCharacter::ANetworkCharacter() : ABaseNetworkCharacter() {
	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f)); // Position the camera (note this will be changed to the origin for VR)
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '3rd person' view (when not controlling this pawn)
	Mesh3P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh3P"));
	Mesh3P->SetupAttachment(FirstPersonCameraComponent);
	Mesh3P->SetRelativeRotation(FRotator(0.0f, 270.0f, 0.0f));
	Mesh3P->SetRelativeLocation(FVector(40.0f, 0.0f, -155.7f)); // Position the mesh so that it aligns with the capsule
	
	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetRelativeRotation(FRotator(1.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-0.5f, -4.4f, -155.7f));

	// Setup meshes
	HairMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("HairMesh"));
	HairMesh->SetupAttachment(Mesh3P);
	HairMesh->SetRelativeLocation(FVector(0.0f, -0.0f, -68.0f));
	HairMesh->SetIsReplicated(true);

	HeadMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("HeadMesh"));
	HeadMesh->SetupAttachment(Mesh3P);
	HeadMesh->SetRelativeLocation(FVector(0.0f, -0.0f, -68.0f));
	HeadMesh->SetIsReplicated(true);

	ChestMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ChestMesh"));
	ChestMesh->SetupAttachment(Mesh3P);
	ChestMesh->SetRelativeLocation(FVector(0.0f, -0.0f, -68.0f));
	ChestMesh->SetIsReplicated(true);

	ArmMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ArmMesh"));
	ArmMesh->SetupAttachment(Mesh3P);
	ArmMesh->SetRelativeLocation(FVector(0.0f, -0.0f, -68.0f));
	ArmMesh->SetIsReplicated(true);

	LegMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("LegMesh"));
	LegMesh->SetupAttachment(Mesh3P);
	LegMesh->SetRelativeLocation(FVector(0.0f, -0.0f, -68.0f));
	LegMesh->SetIsReplicated(true);
}

void ANetworkCharacter::BeginPlay() {
	Super::BeginPlay();

	// Show or hide meshes based on if we are locally controlling the character
	Mesh1P->SetVisibility(IsLocallyControlled(), true);
	Mesh3P->SetVisibility(!IsLocallyControlled(), true);
}

void ANetworkCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) {
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

/** Called when unpossessed */
void ANetworkCharacter::UnPossessed() {
	Super::UnPossessed();

	// Stop movement
	GetCharacterMovement()->StopMovementImmediately();
}

