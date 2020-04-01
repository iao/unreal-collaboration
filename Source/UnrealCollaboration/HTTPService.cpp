// Fill out your copyright notice in the Description page of Project Settings.

#include "HTTPService.h"
#include "GameFramework/Actor.h"

// Sets default values
AHTTPService::AHTTPService() { PrimaryActorTick.bCanEverTick = false; }
void AHTTPService::BeginPlay() {
	AActor::BeginPlay();
}

TSharedRef<IHttpRequest> AHTTPService::PostRequest(FString URL, FString ContentJsonString) {
	TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(URL);
	Request->SetHeader(TEXT("User-Agent"), TEXT("X-UnrealEngine-Agent"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	Request->SetHeader(TEXT("Accepts"), TEXT("application/json"));
	Request->SetVerb("POST");
	Request->SetContentAsString(ContentJsonString);
	return Request;
}

void AHTTPService::Send(TSharedRef<IHttpRequest>& Request) {
	Request->ProcessRequest();
}

bool AHTTPService::ResponseIsValid(FHttpResponsePtr Response, bool bWasSuccessful) {
	if (!bWasSuccessful || !Response.IsValid()) return false;
	if (EHttpResponseCodes::IsOk(Response->GetResponseCode())) return true;
	UE_LOG(LogTemp, Warning, TEXT("Http Response returned error code: %d"), Response->GetResponseCode());
	return false;
}

template <typename StructType>
void AHTTPService::GetJsonStringFromStruct(StructType FilledStruct, FString& StringOutput) {
	FJsonObjectConverter::UStructToJsonObjectString(StructType::StaticStruct(), &FilledStruct, StringOutput, 0, 0);
}

template <typename StructType>
void AHTTPService::GetStructFromJsonString(FString content, StructType& StructOutput) {
	StructType StructData;
	FJsonObjectConverter::JsonObjectStringToUStruct<StructType>(content, &StructOutput, 0, 0);
}

FString AHTTPService::LoadInfoJSON() {
	FString path = FPaths::ConvertRelativePathToFull(FPaths::ProjectContentDir() + "/info.json");
	FString data = "";

	if (!FPlatformFileManager::Get().GetPlatformFile().FileExists(*path)) {
		UE_LOG(LogTemp, Warning, TEXT("Cannot load info.json at '%s'!"), *path);
		return data;
	} FFileHelper::LoadFileToString(data, *path);
	return data;
}

FInfoJSONStruct AHTTPService::GetInfoJSON() {
	FString JSON = LoadInfoJSON();
	if (JSON != "") {
		FInfoJSONStruct info;
		GetStructFromJsonString<FInfoJSONStruct>(JSON, info);
		return info;
	}

	FInfoJSONStruct toreturn;
	toreturn.port = -1;
	toreturn.session = "";
	toreturn.title = "";
	toreturn.url = "";
	return toreturn;
}

void AHTTPService::KeepAlive(FString URL, FKeepAliveStruct keepalive, ANetworkPlayerController* controller) {
	FString ContentJsonString;
	GetJsonStringFromStruct<FKeepAliveStruct>(keepalive, ContentJsonString);

	TSharedRef<IHttpRequest> Request = PostRequest(URL, ContentJsonString);
	Request->OnProcessRequestComplete().BindUObject(controller, &ANetworkPlayerController::KeepAliveResponce);
	Send(Request);
}

void AHTTPService::Info(FString URL, FInfoStruct_Request info, ANetworkPlayerController* controller) {
	FString ContentJsonString;
	GetJsonStringFromStruct<FInfoStruct_Request>(info, ContentJsonString);

	TSharedRef<IHttpRequest> Request = PostRequest(URL, ContentJsonString);
	Request->OnProcessRequestComplete().BindUObject(controller, &ANetworkPlayerController::InfoResponce);
	Send(Request);
}