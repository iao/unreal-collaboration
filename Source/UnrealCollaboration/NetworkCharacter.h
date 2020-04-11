// By Paul Graham <p@ul.ms>

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "HTTPStructs.h"
#include "Runtime/Online/HTTP/Public/Http.h"
#include "NetworkTextRenderActor.h"
#include "NetworkCharacter.generated.h"

struct FInfoStruct_Responce;
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
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class UMotionControllerComponent* R_MotionController;

	/** Motion controller (left hand) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class UMotionControllerComponent* L_MotionController;

public:
	UPROPERTY(Replicated, BlueprintReadWrite, Category = PlayerInfo)
		ANetworkTextRenderActor* TextActor;

protected:
	/** The Actor to be spawned for the users text */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = SpawnInfo)
		TSubclassOf<ANetworkTextRenderActor> TextActorClass;
	
	/** A default customization int from Unreal Selector */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PlayerInfo)
		int DefaultInfo;

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
	ANetworkCharacter();
	
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

	void InfoResponce(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	
protected:
	virtual void BeginPlay();
	
	/** Ask Unreal Selector for information about the player, `UponInfoChanged()` is the callback function */
	UFUNCTION(BlueprintCallable, Category = "Character")
		void RequestInfo();

	/** Callback function for `RequestInfo()` */
	UFUNCTION(BlueprintImplementableEvent, Category = "Character")
		void UponInfoChanged();

	/** Sets internal variables & calls ServerChange */
	void Change(FInfoStruct_Responce responce);
	
	UFUNCTION(Server, Reliable, WithValidation)
		void ServerChange(FInfoStruct_Responce responce);
	
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
