// By Paul Graham <p@ul.ms>

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "NetworkCharacter.generated.h"

class UInputComponent;

UCLASS(config = Game)
class ANetworkCharacter : public ACharacter {
	GENERATED_BODY()

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		class USkeletalMeshComponent* Mesh1P;

	/** Pawn mesh: 3rd person view (seen only by others) */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		class USkeletalMeshComponent* Mesh3P;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* FirstPersonCameraComponent;

	/** The box where the text render component is attached to */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UBoxComponent* BoxComponent;

	/** Text Render Component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Selector, meta = (AllowPrivateAccess = "true"))
		class UTextRenderComponent* TextRenderComponent;

	// TODO: Set these up again
	/** Motion controller (right hand) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class UMotionControllerComponent* R_MotionController;

	/** Motion controller (left hand) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class UMotionControllerComponent* L_MotionController;

public:
	ANetworkCharacter();

protected:
	virtual void BeginPlay();

public:
	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseLookUpRate;

	/** Whether to use motion controller location for aiming. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		uint32 bUsingMotionControllers : 1;

	/** Allow the player to always jump */
	virtual bool CanJumpInternal_Implementation() const override;

protected:
	/** Called every frame */
	virtual void Tick(float DeltaTime) override;

	/** Called when unpossessed */
	void UnPossessed() override;

	/** Resets HMD orientation and position in VR. */
	void OnResetVR();

	/* Handles moving vertically */
	void MoveUp(float Value);

	/** Handles moving forward/backward */
	void MoveForward(float Value);

	/** Handles stafing movement, left and right */
	void MoveRight(float Value);

	/**
	 * Called via input to turn at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

public:
	/** Returns Mesh1P subobject **/
	FORCEINLINE class USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }

	/** Returns Mesh1P subobject **/
	FORCEINLINE class USkeletalMeshComponent* GetMesh3P() const { return Mesh3P; }

	/** Returns FirstPersonCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

};
