// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Runtime/Online/HTTP/Public/Http.h"
#include "Json.h"
#include "JsonUtilities.h"
#include "HTTPStructs.h"
#include "MyPlayerController.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "HTTPService.generated.h"

// https://wiki.unrealengine.com/Http-requests
UCLASS(Blueprintable, hideCategories = (Rendering, Replication, Input, Actor, "Actor Tick"))
class NETWORKINGTEST_API AHTTPService : public AActor {
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

	static void KeepAlive(FString URL, FKeepAliveStruct keepalive, AMyPlayerController* controller);
	static void Info(FString URL, FInfoStruct_Request info, AMyPlayerController* controller);
};
