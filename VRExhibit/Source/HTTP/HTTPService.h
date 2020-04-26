// By Paul Graham <p@ul.ms>

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Runtime/Online/HTTP/Public/Http.h"
#include "Json.h"
#include "JsonUtilities.h"
#include "HTTPStructs.h"
#include "../Character/BaseNetworkPlayerController.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "HTTPService.generated.h"

class ABaseNetworkPlayerController;

// https://wiki.unrealengine.com/Http-requests
UCLASS(Blueprintable, hideCategories = (Rendering, Replication, Input, Actor, "Actor Tick"))
class VREXHIBIT_API AHTTPService : public AActor {
	GENERATED_BODY()

private:
	/** Send out a POST Request to a URL with JSON */
	static TSharedRef<IHttpRequest> PostRequest(FString URL, FString ContentJsonString);

	/** Send the request to the server */
	static void Send(TSharedRef<IHttpRequest>& Request);

	/** Load info.json as a string */
	static FString LoadInfoJSON();

public:
	/** Check if a response is valid (i.e. code 200) */
	static bool ResponseIsValid(FHttpResponsePtr Response, bool bWasSuccessful);

	/** Convert between struct -> string */
	template <typename StructType>
	static void GetJsonStringFromStruct(StructType FilledStruct, FString& StringOutput);

	/** Convert between string -> struct */
	template <typename StructType>
	static void GetStructFromJsonString(FString content, StructType& StructOutput);

	/** Get info.json as a struct */
	static FInfoJSONStruct GetInfoJSON();

	/** Constructor */
	AHTTPService();

	/** BeginPlay */
	virtual void BeginPlay() override;

	/** Call /api/project/keepalive on UnrealSelector */
	static void KeepAlive(FString URL, FKeepAliveStruct keepalive, ABaseNetworkPlayerController* controller);

	/** Call /api/project/info on UnrealSelector */
	static void Info(FString URL, FInfoStruct_Request info, class ABaseNetworkCharacter* character);

	/** Call /api/project/time on UnrealSelector */
	static void Time(FString URL, FTimeStruct_Request time, ABaseNetworkPlayerController* controller);
};
