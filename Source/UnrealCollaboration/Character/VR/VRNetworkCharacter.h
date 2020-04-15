// By Paul Graham <p@ul.ms>

#pragma once

#include "CoreMinimal.h"
#include "../BaseNetworkCharacter.h"
#include "VRControllerActor.h"
#include "Camera/CameraComponent.h"
#include "VRNetworkCharacter.generated.h"

UCLASS()
class UNREALCOLLABORATION_API AVRNetworkCharacter : public ABaseNetworkCharacter {
	GENERATED_BODY()

public:
	AVRNetworkCharacter();

	/** VR Origin Component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = VR, meta = (AllowPrivateAccess = "true"))
		USceneComponent* VROrigin;

	/** Camera component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = VR, meta = (AllowPrivateAccess = "true"))
		UCameraComponent* Camera;

	/** Class to spawn the controllers */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = VR, meta = (AllowPrivateAccess = "true"))
		TSubclassOf<AVRControllerActor> ControllerClass;

	/** Motion controller (left hand) */
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = VR, meta = (AllowPrivateAccess = "true"))
		AVRControllerActor* LController;

	/** Motion controller (right hand) */
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = VR, meta = (AllowPrivateAccess = "true"))
		AVRControllerActor* RController;
	
	/** Maximum absolute speed in XY */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = VRMovement)
		float XYMaxSpeed = 500.f;

	/** Maximum absolute speed in Z */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = VRMovement)
		float ZMaxSpeed = 1000.f;

	/** Launch speed for the player */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = VRMovement)
		float LaunchSpeed = 100.f;

protected:
	/** Begins play for this actor */
	void BeginPlay() override;

	/** Called every frame */
	void Tick(float DeltaTime) override;

	/** Setup inputs */
	void SetupPlayerInputComponent(UInputComponent* InputComponent) override;

	/* Handles moving in VR for the client */
	void MoveVR(float Value);

	/* Handles moving in VR for the server */
	UFUNCTION(Server, Reliable, WithValidation)
		void ServerMoveVR(FVector vector);
};
