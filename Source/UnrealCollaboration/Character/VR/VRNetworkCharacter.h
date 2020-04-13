// By Paul Graham <p@ul.ms>

#pragma once

#include "CoreMinimal.h"
#include "../BaseNetworkCharacter.h"
#include "VRNetworkCharacter.generated.h"

/**
 * 
 */
UCLASS()
class UNREALCOLLABORATION_API AVRNetworkCharacter : public ABaseNetworkCharacter {
	GENERATED_BODY()

public:
	AVRNetworkCharacter();

protected:
	/** Motion controller (right hand) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = VR, meta = (AllowPrivateAccess = "true"))
		class UMotionControllerComponent* R_MotionController;

	/** Motion controller (left hand) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = VR, meta = (AllowPrivateAccess = "true"))
		class UMotionControllerComponent* L_MotionController;

	/** Maximum absolute speed in XY */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = VRMovement)
		float XYMaxSpeed = 500.f;

	/** Maximum absolute speed in Z */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = VRMovement)
		float ZMaxSpeed = 1000.f;

	/** Launch speed for the player */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = VRMovement)
		float LaunchSpeed = 100.f;

	/** Begins play for this actor */
	void BeginPlay() override;

	/** Called every frame */
	void Tick(float DeltaTime) override;

	/** Setup inputs */
	void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	
	/** Resets HMD orientation and position in VR. */
	void OnResetVR();

	/* Handles moving in VR for the client */
	void MoveVR(float Value);

	/* Handles moving in VR for the server */
	UFUNCTION(Server, Reliable, WithValidation)
		void ServerMoveVR(FVector vector);

};
