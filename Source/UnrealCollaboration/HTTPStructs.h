// By Paul Graham <p@ul.ms>

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HTTPStructs.generated.h"

USTRUCT()
struct FInfoJSONStruct {
	GENERATED_BODY()
		UPROPERTY() FString session;
	UPROPERTY() FString title;
	UPROPERTY() FString url;
	UPROPERTY() int port;

	FInfoJSONStruct() {}
};

USTRUCT()
struct FKeepAliveStruct {
	GENERATED_BODY()
		UPROPERTY() FString session;
	UPROPERTY() FString title;

	FKeepAliveStruct() {}
};

USTRUCT()
struct FTimeStruct_Request {
	GENERATED_BODY()
		UPROPERTY() FString session;

	FTimeStruct_Request() {}
};

USTRUCT()
struct FTimeStruct_Responce {
	GENERATED_BODY()
		UPROPERTY() int time;

	FTimeStruct_Responce() {}
};

USTRUCT()
struct FInfoStruct_Request {
	GENERATED_BODY()
		UPROPERTY() FString session;

	FInfoStruct_Request() {}
};

USTRUCT()
struct FInfoStruct_Responce {
	GENERATED_BODY()
		UPROPERTY() FString username;
	UPROPERTY() int info;
	UPROPERTY() FString rank;

	FInfoStruct_Responce() {}
};

UCLASS()
class UNREALCOLLABORATION_API AHTTPStructs : public AActor {
	GENERATED_BODY()

public:
	AHTTPStructs();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
