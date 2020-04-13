// By Paul Graham <p@ul.ms>

#pragma once

#include "CoreMinimal.h"
#include "../BaseNetworkCharacter.h"
#include "NetworkCharacter.generated.h"

UCLASS()
class UNREALCOLLABORATION_API ANetworkCharacter : public ABaseNetworkCharacter {
	GENERATED_BODY()
	
	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USkeletalMeshComponent* Mesh1P;
	
	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FirstPersonCameraComponent;

protected:
	/** Called when unpossessed */
	void UnPossessed() override;

	/** Setup inputs */
	void SetupPlayerInputComponent(UInputComponent* InputComponent) override;

public:
	ANetworkCharacter();
	void BeginPlay() override;
	
	/** Returns FirstPersonCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

	/** Returns Mesh1P subobject **/
	FORCEINLINE class USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
};
