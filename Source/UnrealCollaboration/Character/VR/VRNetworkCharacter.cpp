// By Paul Graham <p@ul.ms>

#include "VRNetworkCharacter.h"
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

AVRNetworkCharacter::AVRNetworkCharacter() : ABaseNetworkCharacter() {
	// Create a mesh component that will be used when being viewed from a '3rd person' view (when not controlling this pawn)
	Mesh3P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh3P"));
	Mesh3P->SetupAttachment(GetCapsuleComponent()); // TODO:
	Mesh3P->SetRelativeRotation(FRotator(0.0f, 270.0f, 0.0f));
	Mesh3P->SetRelativeLocation(FVector(40.0f, 0.0f, -155.7f)); // Position the mesh so that it aligns with the capsule

	
	// TODO: In controller - Setup motion controllers
	L_MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("L_MotionController"));
	L_MotionController->SetupAttachment(RootComponent);
	L_MotionController->MotionSource = FName("Left");
	L_MotionController->SetTrackingSource(EControllerHand::Left);
	L_MotionController->bDisplayDeviceModel = false;
	L_MotionController->SetHiddenInGame(true);

	R_MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("R_MotionController"));
	R_MotionController->SetupAttachment(RootComponent); // Change to VROrigin
	R_MotionController->MotionSource = FName("Right");
	R_MotionController->SetTrackingSource(EControllerHand::Right);
	R_MotionController->bDisplayDeviceModel = false;
	R_MotionController->SetHiddenInGame(true);
}

void AVRNetworkCharacter::BeginPlay() {
	Super::BeginPlay();

	// Allow for free camera movement if we are in VR.
	// Move mesh back to same position it was before
	Mesh3P->SetRelativeLocation(FVector(15.44f, -1.75f, -155.7f));

	// Show or hide meshes based on if we are locally controlling the character
	Mesh3P->SetVisibility(!IsLocallyControlled());
	
	// Show motion controllers
	L_MotionController->SetHiddenInGame(false);
	R_MotionController->SetHiddenInGame(false);
}

void AVRNetworkCharacter::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	if (GetLocalRole() == ROLE_Authority) {
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

void AVRNetworkCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) {
	// Bind VR events (binding is always allowed on the server)
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	// Setup VR Reset
	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &AVRNetworkCharacter::OnResetVR);

	// Setup VR movement
	PlayerInputComponent->BindAxis("Move", this, &AVRNetworkCharacter::MoveVR);

	// Setup VR turning
	PlayerInputComponent->BindAction("TurnLeft", IE_Pressed, this, &ABaseNetworkCharacter::TurnLeft);
	PlayerInputComponent->BindAction("TurnRight", IE_Pressed, this, &ABaseNetworkCharacter::TurnRight);
}

void AVRNetworkCharacter::MoveVR(float Value) {
	if (Value != 0.0f) {
		// Launch the player in the direction of the left motion controller
		APlayerCameraManager* manager = GetWorld()->GetFirstPlayerController()->PlayerCameraManager;
		FVector launch = R_MotionController->GetComponentRotation().RotateVector(manager->GetActorForwardVector()) * LaunchSpeed * Value;
		ServerMoveVR(launch);
	}
}

void AVRNetworkCharacter::ServerMoveVR_Implementation(FVector vector) {
	// Launch the player in the direction of the left motion controller
	LaunchCharacter(vector, false, !(GetMovementComponent()->Velocity.Z > 0.0f));
}

bool AVRNetworkCharacter::ServerMoveVR_Validate(FVector vector) {
	return true;
}

void AVRNetworkCharacter::OnResetVR() {
	// Reset VR
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}