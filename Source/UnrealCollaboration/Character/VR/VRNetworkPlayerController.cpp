// By Paul Graham <p@ul.ms>

#include "VRNetworkPlayerController.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine.h"
#include "../../HTTP/HTTPService.h"
#include "Math/UnrealMathUtility.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "MotionControllerComponent.h"
#include "Kismet/GameplayStatics.h"

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

void AVRNetworkPlayerController::OnResetVR() {
	// Reset VR
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void AVRNetworkPlayerController::TurnLeft() {
	// Turn left 45 degrees
	//AddYawInput(-45.f / InputYawScale);
	FRotator currentRot;
	FVector currentPos;
	UHeadMountedDisplayFunctionLibrary::GetOrientationAndPosition(currentRot, currentPos);
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition(currentRot.Yaw - 45);
}

void AVRNetworkPlayerController::TurnRight() {
	// Turn right 45 degrees
	//AddYawInput(45.f / InputYawScale);
	FRotator currentRot;
	FVector currentPos;
	UHeadMountedDisplayFunctionLibrary::GetOrientationAndPosition(currentRot, currentPos);
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition(currentRot.Yaw + 45);
}