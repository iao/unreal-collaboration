// By Paul Graham <p@ul.ms>

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MotionControllerComponent.h"
#include "VRControllerActor.generated.h"

UCLASS()
class UNREALCOLLABORATION_API AVRControllerActor : public AActor {
	GENERATED_BODY()
	
public:
	/** Sets default values for this actor's properties */
	AVRControllerActor(const FObjectInitializer& ObjectInitializer);
	
	/** Scene Component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = VR, meta = (AllowPrivateAccess = "true"))
		USceneComponent* Scene;

	/** Motion controller */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = VR, meta = (AllowPrivateAccess = "true"))
		UMotionControllerComponent* MotionController;

	/** Set the motion source for this actor */
	void SetSource(FName source);
	
	void BeginPlay() override;
	void Tick(float DeltaTime) override;
};
