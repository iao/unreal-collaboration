// By Paul Graham <p@ul.ms>

#include "NetworkStartActorScript.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "HAL/PlatformFilemanager.h"

void ANetworkStartActorScript::BeginPlay() {
	Super::BeginPlay();
	
	// Find hostname.txt, and open that level
	FString path = FPaths::ConvertRelativePathToFull(FPaths::ProjectContentDir() + "/hostname.txt");
	FString hostname = "127.0.0.1";

	if (!FPlatformFileManager::Get().GetPlatformFile().FileExists(*path)) {
		UE_LOG(LogTemp, Warning, TEXT("Cannot load hostname.txt at '%s'! Assuming localhost."), *path);
	} else {
		FFileHelper::LoadFileToString(hostname, *path);
		UE_LOG(LogTemp, Warning, TEXT("Found hostname '%s', loading."), *hostname);
	} UGameplayStatics::OpenLevel(this, FName(*hostname));
}
