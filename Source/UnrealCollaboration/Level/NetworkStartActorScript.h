// By Paul Graham <p@ul.ms>

#pragma once

#include "CoreMinimal.h"
#include "Engine/LevelScriptActor.h"
#include "NetworkStartActorScript.generated.h"

UCLASS()
class UNREALCOLLABORATION_API ANetworkStartActorScript : public ALevelScriptActor {
	GENERATED_BODY()

public:
	/** Open level at on remote server */
	virtual void BeginPlay() override;
};
