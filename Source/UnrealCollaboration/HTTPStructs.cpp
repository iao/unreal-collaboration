// By Paul Graham <p@ul.ms>

#include "HTTPStructs.h"

// Sets default values
AHTTPStructs::AHTTPStructs() { PrimaryActorTick.bCanEverTick = false; }
void AHTTPStructs::BeginPlay() {
	Super::BeginPlay();
}

// Called every frame
void AHTTPStructs::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

