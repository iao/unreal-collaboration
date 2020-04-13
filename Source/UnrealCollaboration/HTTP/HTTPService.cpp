// By Paul Graham <p@ul.ms>

#include "HTTPService.h"
#include "../Character/BaseNetworkCharacter.h"
#include "GameFramework/Actor.h"

// Sets default values
AHTTPService::AHTTPService() { PrimaryActorTick.bCanEverTick = false; }

// Call super BeginPlay
void AHTTPService::BeginPlay() {
	AActor::BeginPlay();
}

// Sets content for the HTTP Request we'll make to Unreal Selector
TSharedRef<IHttpRequest> AHTTPService::PostRequest(FString URL, FString ContentJsonString) {
	// Create request & set headers
	TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(URL);
	Request->SetHeader(TEXT("User-Agent"), TEXT("X-UnrealEngine-Agent"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	Request->SetHeader(TEXT("Accepts"), TEXT("application/json"));
	Request->SetVerb("POST");

	// Set content as our JSON String
	Request->SetContentAsString(ContentJsonString);
	return Request;
}

// Send the content to Unreal Selector
void AHTTPService::Send(TSharedRef<IHttpRequest>& Request) {
	Request->ProcessRequest();
}

// Check if the response was valid (i.e. code 200)
bool AHTTPService::ResponseIsValid(FHttpResponsePtr Response, bool bWasSuccessful) {
	if (!bWasSuccessful || !Response.IsValid()) return false;
	if (EHttpResponseCodes::IsOk(Response->GetResponseCode())) return true;
	UE_LOG(LogTemp, Warning, TEXT("Http Response returned error code: %d"), Response->GetResponseCode());
	return false;
}

// Transform Struct to JSON String
template <typename StructType>
void AHTTPService::GetJsonStringFromStruct(StructType FilledStruct, FString& StringOutput) {
	FJsonObjectConverter::UStructToJsonObjectString(StructType::StaticStruct(), &FilledStruct, StringOutput, 0, 0);
}

// Transform JSON String to sturct
template <typename StructType>
void AHTTPService::GetStructFromJsonString(FString content, StructType& StructOutput) {
	StructType StructData;
	FJsonObjectConverter::JsonObjectStringToUStruct<StructType>(content, &StructOutput, 0, 0);
}

// Load info.json as a string which Unreal Selector places in `Content`
FString AHTTPService::LoadInfoJSON() {
	FString path = FPaths::ConvertRelativePathToFull(FPaths::ProjectContentDir() + "/info.json");
	FString data = "";

	// Deal with errors
	if (!FPlatformFileManager::Get().GetPlatformFile().FileExists(*path)) {
		UE_LOG(LogTemp, Warning, TEXT("Cannot load info.json at '%s'!"), *path);
		return data;
	} FFileHelper::LoadFileToString(data, *path);
	return data;
}

// Load info.json as a struct
FInfoJSONStruct AHTTPService::GetInfoJSON() {
	FString JSON = LoadInfoJSON();
	if (JSON != "") {
		FInfoJSONStruct info;
		GetStructFromJsonString<FInfoJSONStruct>(JSON, info);
		return info;
	}

	// Defaults if we cannot open info.json
	FInfoJSONStruct toreturn;
	toreturn.port = -1;
	toreturn.session = "";
	toreturn.title = "";
	toreturn.url = "";
	return toreturn;
}

// Send out a keepalive request, call the player controller with the response
void AHTTPService::KeepAlive(FString URL, FKeepAliveStruct keepalive, ABaseNetworkPlayerController* controller) {
	FString ContentJsonString;
	GetJsonStringFromStruct<FKeepAliveStruct>(keepalive, ContentJsonString);

	TSharedRef<IHttpRequest> Request = PostRequest(URL, ContentJsonString);
	Request->OnProcessRequestComplete().BindUObject(controller, &ABaseNetworkPlayerController::KeepAliveResponse);
	Send(Request);
}

// Send out an info request, call the character with the response
void AHTTPService::Info(FString URL, FInfoStruct_Request info, ABaseNetworkCharacter* character) {
	FString ContentJsonString;
	GetJsonStringFromStruct<FInfoStruct_Request>(info, ContentJsonString);

	TSharedRef<IHttpRequest> Request = PostRequest(URL, ContentJsonString);
	Request->OnProcessRequestComplete().BindUObject(character, &ABaseNetworkCharacter::InfoResponse);
	Send(Request);
}

// Send out a time request, call the player controller with the response
void AHTTPService::Time(FString URL, FTimeStruct_Request time, ABaseNetworkPlayerController* controller) {
	FString ContentJsonString;
	GetJsonStringFromStruct<FTimeStruct_Request>(time, ContentJsonString);

	TSharedRef<IHttpRequest> Request = PostRequest(URL, ContentJsonString);
	Request->OnProcessRequestComplete().BindUObject(controller, &ABaseNetworkPlayerController::TimeResponse);
	Send(Request);
}