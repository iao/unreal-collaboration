// By Paul Graham <p@ul.ms>

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HTTPStructs.generated.h"

/** Info.json struct */
USTRUCT()
struct FInfoJSONStruct {
	GENERATED_BODY()
		UPROPERTY() FString session;
	UPROPERTY() FString title;
	UPROPERTY() FString url;
	UPROPERTY() int port;

	FInfoJSONStruct() {}
};

/** Keepalive request struct */
USTRUCT()
struct FKeepAliveStruct {
	GENERATED_BODY()
		UPROPERTY() FString session;
	UPROPERTY() FString title;

	FKeepAliveStruct() {}
};

/** Time request struct */
USTRUCT()
struct FTimeStruct_Request {
	GENERATED_BODY()
		UPROPERTY() FString session;

	FTimeStruct_Request() {}
};

/** Time response struct */
USTRUCT()
struct FTimeStruct_Response {
	GENERATED_BODY()
		UPROPERTY() int time;

	FTimeStruct_Response() {}
};

/** Info request struct */
USTRUCT()
struct FInfoStruct_Request {
	GENERATED_BODY()
		UPROPERTY() FString session;

	FInfoStruct_Request() {}
};

/**Indo response struct */
USTRUCT()
struct FInfoStruct_Response {
	GENERATED_BODY()
		UPROPERTY() FString username;
	UPROPERTY() int info;
	UPROPERTY() FString rank;
	UPROPERTY() bool isAdmin;

	FInfoStruct_Response() {}
};

UCLASS()
class UNREALCOLLABORATION_API AHTTPStructs : public AActor {
	GENERATED_BODY()

public:
	/** Constructor */
	AHTTPStructs();

protected:
	/** Called when the game starts or when spawned */
	virtual void BeginPlay() override;

public:
	/** Called every frame */
	virtual void Tick(float DeltaTime) override;

};
