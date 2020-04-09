// By Paul Graham <p@ul.ms>

#pragma once

#include "CoreMinimal.h"
#include "Engine/LevelScriptActor.h"
#include "NetworkStartActorScript.generated.h"

UCLASS()
class UNREALCOLLABORATION_API ANetworkStartActorScript : public ALevelScriptActor {
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
};
