// By Paul Graham <p@ul.ms>

#include "VRNetworkPlayerController.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine.h"
#include "../../HTTP/HTTPService.h"
#include "Math/UnrealMathUtility.h"
#include "HeadMountedDisplayFunctionLibrary.h"

AVRNetworkPlayerController::AVRNetworkPlayerController(const FObjectInitializer& ObjectInitializer) : ABaseNetworkPlayerController(ObjectInitializer) {}

void AVRNetworkPlayerController::SetupInputComponent() {
	Super::SetupInputComponent();

	// Bind VR Actions
	check(InputComponent);
	if (InputComponent) {
		InputComponent->BindAction("Hide", IE_Pressed, this, &ABaseNetworkPlayerController::Hide);
		InputComponent->BindAction("Quit", IE_Pressed, this, &ABaseNetworkPlayerController::Quit);
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