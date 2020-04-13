// By Paul Graham <p@ul.ms>

#pragma once

#include "CoreMinimal.h"
#include "../BaseNetworkPlayerController.h"
#include "VRNetworkPlayerController.generated.h"

UCLASS()
class UNREALCOLLABORATION_API AVRNetworkPlayerController : public ABaseNetworkPlayerController {
	GENERATED_BODY()

	AVRNetworkPlayerController(const FObjectInitializer& ObjectInitializer);
	void SetupInputComponent() override;

	/** Hide signs */
	void Hide() override;
};
