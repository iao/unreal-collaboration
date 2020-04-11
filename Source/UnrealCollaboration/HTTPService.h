// By Paul Graham <p@ul.ms>

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Runtime/Online/HTTP/Public/Http.h"
#include "Json.h"
#include "JsonUtilities.h"
#include "HTTPStructs.h"
#include "NetworkPlayerController.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "HTTPService.generated.h"

// https://wiki.unrealengine.com/Http-requests
UCLASS(Blueprintable, hideCategories = (Rendering, Replication, Input, Actor, "Actor Tick"))
class UNREALCOLLABORATION_API AHTTPService : public AActor {
	GENERATED_BODY()

private:
	static TSharedRef<IHttpRequest> PostRequest(FString Subroute, FString ContentJsonString);
	static void Send(TSharedRef<IHttpRequest>& Request);

	static FString LoadInfoJSON();

public:
	static bool ResponseIsValid(FHttpResponsePtr Response, bool bWasSuccessful);

	template <typename StructType>
	static void GetJsonStringFromStruct(StructType FilledStruct, FString& StringOutput);
	template <typename StructType>
	static void GetStructFromJsonString(FString content, StructType& StructOutput);

	static FInfoJSONStruct GetInfoJSON();

	AHTTPService();
	virtual void BeginPlay() override;

	static void KeepAlive(FString URL, FKeepAliveStruct keepalive, ANetworkPlayerController* controller);
	static void Info(FString URL, FInfoStruct_Request info, class ANetworkCharacter* character);
	static void Time(FString URL, FTimeStruct_Request time, ANetworkPlayerController* controller);
};
