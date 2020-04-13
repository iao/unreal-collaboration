// By Paul Graham <p@ul.ms>

#pragma once

#include "CoreMinimal.h"
#include "Engine/TextRenderActor.h"
#include "Components/TextRenderComponent.h"
#include "NetworkTextRenderActor.generated.h"

UCLASS()
class UNREALCOLLABORATION_API ANetworkTextRenderActor : public ATextRenderActor {
	GENERATED_BODY()

public:
	/** Called to hide the actor */
	UFUNCTION(BlueprintCallable, Category = "Disable")
		void HideActor(bool toHide);

	/** The text this actor is rendering */
	UPROPERTY(Transient, ReplicatedUsing = OnRep_Text)
		FText text;

	/** The color of the text */
	UPROPERTY(BlueprintReadWrite, Transient, ReplicatedUsing = OnRep_TextColor, Category = TextProp)
		FColor textColor;

	ANetworkTextRenderActor();

	/** Called after the text replicates */
	UFUNCTION()
		void OnRep_Text();

	/** Called after the text color replicates */
	UFUNCTION()
		void OnRep_TextColor();

};
