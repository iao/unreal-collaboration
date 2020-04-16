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

	/** Launch speed for the player */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = VRMovement)
		float LaunchSpeed = 100.f;
	
	/** Hide signs */
	void Hide() override;

	/* Handles turning left VR */
	void TurnLeft();

	/* Handles turning right VR */
	void TurnRight();
	
	/** Resets HMD orientation and position in VR. */
	void OnResetVR();

	/* Handles moving in VR for the client */
	void MoveVR(float Value);

	/* Handles moving in VR for the server */
	UFUNCTION(Server, Reliable, WithValidation)
		void ServerMoveVR(FVector vector);
};
