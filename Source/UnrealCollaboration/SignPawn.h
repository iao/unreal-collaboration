// By Paul Graham <p@ul.ms>

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/DefaultPawn.h"
#include "Engine/TextRenderActor.h"
#include "NetworkTextRenderActor.h"
#include "SignPawn.generated.h"

UCLASS()
class UNREALCOLLABORATION_API ASignPawn : public ADefaultPawn {
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ASignPawn();

	UFUNCTION(BlueprintCallable, Category = "Disable")
		void HideActor(bool toHide);

	/** Camera component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* CameraComponent;
	
	/** The box where the text render actor is attached to */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UBoxComponent* BoxComponent;
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_TextActor)
		ANetworkTextRenderActor* TextActor;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = SpawnInfo)
		TSubclassOf<ANetworkTextRenderActor> TextActorClass;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = SpawnInfo)
		FString DefaultText;

	UFUNCTION()
		void OnRep_TextActor();

	void SetText(FText text);

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerSetText(const FText& text);

protected:
	/** Called every frame */
	virtual void Tick(float DeltaTime) override;
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

};
