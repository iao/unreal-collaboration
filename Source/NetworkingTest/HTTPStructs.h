// Fill out your copyright notice in the Description page of Project Settings.

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
struct FInfoStruct_Request {
	GENERATED_BODY()
		UPROPERTY() FString session;

	FInfoStruct_Request() {}
};

USTRUCT()
struct FInfoStruct_Responce {
	GENERATED_BODY()
		UPROPERTY() int info;

	FInfoStruct_Responce() {}
};

UCLASS()
class NETWORKINGTEST_API AHTTPStructs : public AActor {
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
