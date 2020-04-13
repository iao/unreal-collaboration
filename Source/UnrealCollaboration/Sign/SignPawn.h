// By Paul Graham <p@ul.ms>

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/DefaultPawn.h"
#include "Engine/TextRenderActor.h"
#include "../Shared/NetworkTextRenderActor.h"
#include "SignPawn.generated.h"

UCLASS()
class UNREALCOLLABORATION_API ASignPawn : public ADefaultPawn {
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ASignPawn();

	/** Hides the sign */
	UFUNCTION(BlueprintCallable, Category = "Disable")
		void HideActor(bool toHide);

	/** Camera component to view the sign */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* CameraComponent;
	
	/** The box where the text render actor is attached to */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UBoxComponent* BoxComponent;

	/** The text actor, where the text will be */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_TextActor)
		ANetworkTextRenderActor* TextActor;

	/** If the actor and text should rotate around XY to face the player */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = SpawnInfo)
		bool ShouldRotate = true;

	/** The class to use for the text actor */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = SpawnInfo)
		TSubclassOf<ANetworkTextRenderActor> TextActorClass;

	/** The default text to spawn this pawn with */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = SpawnInfo)
		FString DefaultText;

	/** Called after the actor replicates, ensure it is hidden if the player asks for it to be */
	UFUNCTION()
		void OnRep_TextActor();

	/** Set the text on the client */
	void SetText(FText text);

	/** Set the text on the server */
	UFUNCTION(Server, Reliable, WithValidation)
		void ServerSetText(const FText& text);

protected:
	/** Called every frame */
	virtual void Tick(float DeltaTime) override;
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

};
