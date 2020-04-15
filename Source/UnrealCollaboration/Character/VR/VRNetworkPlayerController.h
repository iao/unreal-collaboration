// By Paul Graham <p@ul.ms>

#pragma once

#include "CoreMinimal.h"
#include "../BaseNetworkPlayerController.h"
#include "VRNetworkPlayerController.generated.h"

UCLASS()
class UNREALCOLLABORATION_API AVRNetworkPlayerController : public ABaseNetworkPlayerController {
	GENERATED_BODY()

protected:
	AVRNetworkPlayerController(const FObjectInitializer& ObjectInitializer);
	void SetupInputComponent() override;
	
	/** Hide signs */
	void Hide() override;

	/* Handles turning left VR */
	void TurnLeft();

	/* Handles turning right VR */
	void TurnRight();

	/** Resets HMD orientation and position in VR. */
	void OnResetVR();
};
