// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "TcpLink.generated.h"

/**
*
*/
UCLASS()
class VREXHIBIT_API UTcpLink : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	/** Initialise the socket to the external application */
	UFUNCTION(BlueprintCallable, Category = "TcpLink")
		static bool InitPipe(int32 ChimeraPort, AActor *anyActor, bool debug = false);

	/** Send a string through the TCP socket to the external application */
	UFUNCTION(BlueprintCallable, Category = "TcpLink")
		static bool SendString(FString str);

	/** Close the TCP socket which connects to the external application */
	UFUNCTION(BlueprintCallable, Category = "TcpLink")
		static bool ClosePipe();

	/** True whenever a string has been received off the network and hasn't been consumed. */
	UFUNCTION(BlueprintCallable, Category = "TcpLink")
		static bool HasString();

	/** Triggered when a string has been received via the TCP socket */
	UFUNCTION(BlueprintCallable, Category = "TcpLink")
		static FString GetString();

};
