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
	UFUNCTION(BlueprintCallable, Category = "Disable")
		void HideActor(bool toHide);

	UPROPERTY(Transient, ReplicatedUsing = OnRep_Text)
		FText text;

	ANetworkTextRenderActor();

	UFUNCTION()
		void OnRep_Text();

};
