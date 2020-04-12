// By Paul Graham <p@ul.ms>

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "HTTPStructs.h"
#include "Runtime/Online/HTTP/Public/Http.h"
#include "NetworkTextRenderActor.h"
#include "NetworkCharacter.generated.h"

struct FInfoStruct_Response;
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

protected:
	/** Motion controller (right hand) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = VR, meta = (AllowPrivateAccess = "true"))
		class UMotionControllerComponent* R_MotionController;

	/** Motion controller (left hand) */
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = VR, meta = (AllowPrivateAccess = "true"))
		class UMotionControllerComponent* L_MotionController;

public:
	/** The actor which will display the username of the user */
	UPROPERTY(Replicated, BlueprintReadWrite, Category = PlayerInfo)
		ANetworkTextRenderActor* TextActor;

protected:
	/** The Actor to be spawned for the users text */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = SpawnInfo)
		TSubclassOf<ANetworkTextRenderActor> TextActorClass;

	/** Maximum absolute speed in XY */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = VRMovement)
		float XYMaxSpeed = 500.f;

	/** Maximum absolute speed in Z */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = VRMovement)
		float ZMaxSpeed = 1000.f;

	/** Launch speed for the player */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = VRMovement)
		float LaunchSpeed = 100.f;
	
	/** A default customization int from Unreal Selector */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PlayerInfo)
		int DefaultInfo = 1;

	/** The default rank of the user as a string, used to find if the user can do privileged operations */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PlayerInfo)
		FString DefaultRank;

	/** The default users username from Unreal Selector */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PlayerInfo)
		FString DefaultUsername;

	/** If the user is an admin or not by default */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PlayerInfo)
		bool isAdminByDefault;
	
public:
	/** Constructor */
	ANetworkCharacter();

	/** Gets if the player is in VR or not */
	UFUNCTION(BlueprintCallable, Category = VR)
		bool isInVR();
	
	/** A customization int from Unreal Selector, should match options there */
	UPROPERTY(Replicated, BlueprintReadWrite, Category = PlayerInfo)
		int info;

	/** The rank of the user as a string, used to find if the user can do privileged operations */
	UPROPERTY(Replicated, BlueprintReadWrite, Category = PlayerInfo)
		FString rank;

	/** The users username from Unreal Selector */
	UPROPERTY(Replicated, BlueprintReadWrite, Category = PlayerInfo)
		FString username;

	/** If the user is an admin or not */
	UPROPERTY(Replicated, BlueprintReadWrite, Category = PlayerInfo)
		bool isAdmin;

	/** Deals with responses from unreal selector */
	void InfoResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	
protected:
	/** Begins play for this actor */
	virtual void BeginPlay();
	
	/** Ask Unreal Selector for information about the player, `UponInfoChanged()` is the callback function */
	UFUNCTION(BlueprintCallable, Category = "Character")
		void RequestInfo();

	/** Callback function for `RequestInfo()` */
	UFUNCTION(BlueprintImplementableEvent, Category = "Character")
		void UponInfoChanged();

	/** Sets internal variables & calls ServerChange */
	void Change(FInfoStruct_Response responce);

	/** Sets internal variables on the server */
	UFUNCTION(Server, Reliable, WithValidation)
		void ServerChange(FInfoStruct_Response responce);
	
public:
	/** Allow the player to always jump */
	virtual bool CanJumpInternal_Implementation() const override;

protected:
	/** Called every frame */
	virtual void Tick(float DeltaTime) override;

	/** Called when unpossessed */
	void UnPossessed() override;

	/** Resets HMD orientation and position in VR. */
	void OnResetVR();

	/* Handles moving in VR */
	UFUNCTION(Server, Reliable, WithValidation)
		void MoveVR(float Value);
	
	/* Handles turning left VR */
	void TurnLeft();

	/* Handles turning right VR */
	void TurnRight();

	/** Handles moving forward/backward */
	void MoveForward(float Value);

	/** Handles stafing movement, left and right */
	void MoveRight(float Value);

	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;

public:
	/** Returns Mesh1P subobject **/
	FORCEINLINE class USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }

	/** Returns Mesh1P subobject **/
	FORCEINLINE class USkeletalMeshComponent* GetMesh3P() const { return Mesh3P; }

	/** Returns FirstPersonCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

};
