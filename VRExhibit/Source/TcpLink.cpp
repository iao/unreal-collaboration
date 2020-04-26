// Fill out your copyright notice in the Description page of Project Settings.
#include "TcpLink.h"
#include "SocketListener.h"
#include "VRExhibit.h"
#include "GameFramework/Actor.h"

bool UTcpLink::InitPipe(int32 ChimeraPort, AActor *anyActor, bool debug)
{
#if PLATFORM_WINDOWS
	FSocketListener::InitialisePipe(ChimeraPort, anyActor, debug);
#endif
	return true;
}

bool UTcpLink::SendString(FString str)
{
#if PLATFORM_WINDOWS
	FSocketListener::GetPipe()->Send(str);
#endif
	return true;
}

bool UTcpLink::ClosePipe()
{
#if PLATFORM_WINDOWS
	FSocketListener::Shutdown();
#endif
	return true;
}

bool UTcpLink::HasString()
{
#if PLATFORM_WINDOWS
	return FSocketListener::HasString();
	//return false;
#else
	return false;
#endif
}

FString UTcpLink::GetString()
{
#if PLATFORM_WINDOWS
	return FSocketListener::NextString();
	//return TEXT("Nothing");
#else
	return TEXT("Nothing");
#endif
}
