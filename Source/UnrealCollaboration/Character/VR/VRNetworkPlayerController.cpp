// By Paul Graham <p@ul.ms>

#include "VRNetworkPlayerController.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine.h"
#include "../../HTTP/HTTPService.h"
#include "Math/UnrealMathUtility.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "MotionControllerComponent.h"
#include "Kismet/GameplayStatics.h"
#include "VRNetworkCharacter.h"

AVRNetworkPlayerController::AVRNetworkPlayerController(const FObjectInitializer& ObjectInitializer) : ABaseNetworkPlayerController(ObjectInitializer) {}

void AVRNetworkPlayerController::SetupInputComponent() {
	Super::SetupInputComponent();

	// Bind VR Actions
	check(InputComponent);
	if (InputComponent) {
		// Bind hide & quit
		InputComponent->BindAction("Hide", IE_Pressed, this, &ABaseNetworkPlayerController::Hide);
		InputComponent->BindAction("Quit", IE_Pressed, this, &ABaseNetworkPlayerController::Quit);

		// Bind VR events
		InputComponent->BindAction("ResetVR", IE_Pressed, this, &AVRNetworkPlayerController::OnResetVR);

		// Setup VR turning
		InputComponent->BindAction("TurnLeft", IE_Pressed, this, &AVRNetworkPlayerController::TurnLeft);
		InputComponent->BindAction("TurnRight", IE_Pressed, this, &AVRNetworkPlayerController::TurnRight);

		// Setup VR movement
		InputComponent->BindAxis("Move", this, &AVRNetworkPlayerController::MoveVR);
	}
}

// Hide all signs for the client
void AVRNetworkPlayerController::Hide() {
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

void AVRNetworkPlayerController::MoveVR(float Value) {
	if (Value != 0.0f) {
		// Launch the player in the direction of the right motion controller
		AVRNetworkCharacter* character = Cast<AVRNetworkCharacter>(GetPawnOrSpectator());
		if (character) {
			FVector launch = character->RController->MotionController->GetComponentRotation().RotateVector(FVector(1.f, 1.f, 1.f)) * LaunchSpeed * Value;
			ServerMoveVR(launch);
		}
	}
}

void AVRNetworkPlayerController::ServerMoveVR_Implementation(FVector vector) {
	// Launch the player in the direction of the right motion controller
	AVRNetworkCharacter* character = Cast<AVRNetworkCharacter>(GetPawnOrSpectator());
	if(character) character->LaunchCharacter(vector, false, !(character->GetMovementComponent()->Velocity.Z > 0.0f));
}

bool AVRNetworkPlayerController::ServerMoveVR_Validate(FVector vector) {
	return true;
}

void AVRNetworkPlayerController::OnResetVR() {
	// Reset VR
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void AVRNetworkPlayerController::TurnLeft() {
	// Turn left 45 degrees
	AddYawInput(-45.0f / InputYawScale);
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void AVRNetworkPlayerController::TurnRight() {
	// Turn right 45 degrees
	AddYawInput(45.0f / InputYawScale);
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}