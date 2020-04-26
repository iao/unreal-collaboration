#include "SocketListener.h"
#include "VRExhibit.h"
// TODO: Commented out to compile - please update 
//#include "OutputDeviceConsole.h"

#if PLATFORM_WINDOWS
#include "Windows/AllowWindowsPlatformTypes.h"
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <ws2tcpip.h>
#include "Windows/HideWindowsPlatformTypes.h"
#endif

#define CHIMERA_PORT "5001"

//***********************************************************
//Thread Worker Starts as NULL, prior to being instanced
//		This line is essential! Compiler error without it
FSocketListener* FSocketListener::Runnable = NULL;
//***********************************************************

#if PLATFORM_WINDOWS
UWorld *FSocketListener::sWorld = NULL;
FString *FSocketListener::sNextString = NULL;
TQueue<FString*> FSocketListener::sInputQueue;
bool FSocketListener::sWasStopped = false;
bool FSocketListener::sDebug = false;
int FSocketListener::sPort = 5000;
#endif

FSocketListener::FSocketListener()
{
#if PLATFORM_WINDOWS
	//TODO , bAutoDeleteSelf, bAutoDeleteRunnable - do these manually
	Thread = FRunnableThread::Create(this, TEXT("FSocketListener"), 0, TPri_BelowNormal); //windows default = 8mb for thread, could specify more
#endif
}

FSocketListener::~FSocketListener()
{
#if PLATFORM_WINDOWS
	delete Thread;
	Thread = NULL;
#endif
}

#if PLATFORM_WINDOWS
//Init
bool FSocketListener::Init()
{
	Debug(FString::Printf(TEXT("**********************************")));
	Debug(FString::Printf(TEXT("UDP Socket Listener thread started!")));
	Debug(FString::Printf(TEXT("**********************************")));

	return true;
}

//Run
uint32 FSocketListener::Run()
{
	sWasStopped = false;

	StartSocket();

	return 0;
}

//stop
void FSocketListener::Stop()
{
	if (!sWasStopped)
	{
		Debug(FString::Printf(TEXT("Closing socket...")));
		closesocket(mSocket);
	}
	WSACleanup();
	Debug(FString::Printf(TEXT("Socket closed, thread finished")));
	return;
}

void FSocketListener::InitialisePipe(int remotePort, AActor *anyActor, bool debug)
{
	sDebug = debug;
	sPort = remotePort;

	GetPipe();
	if (anyActor)
		sWorld = anyActor->GetWorld();
}

FSocketListener* FSocketListener::GetPipe()
{
	if (!Runnable && FPlatformProcess::SupportsMultithreading())
	{
		Runnable = new FSocketListener();
	}
	return Runnable;
}

void FSocketListener::ForceShutdown()
{
	Send(FString(TEXT("Unreal Stopping")));

	sWasStopped = true;
	closesocket(mSocket);

	StopTaskCounter.Increment();

	Thread->WaitForCompletion();
}

void FSocketListener::Shutdown()
{
	if (Runnable)
	{

		Runnable->ForceShutdown();
		delete Runnable;
		Runnable = NULL;
	}
}

void FSocketListener::StartSocket()
{
	mSocket = INVALID_SOCKET;

	WSADATA wsaData;
	struct addrinfo *server = NULL, hints;
	char *sendbuf = "this is a test";
	int iResult;
	int recvbuflen = DEFAULT_BUFLEN;

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0)
	{
		Debug(FString::Printf(TEXT("WSAStartup failed with error: %d\n"), iResult));
		return;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	char portStr[7];
	_itoa_s(sPort, portStr, 10);
	// Resolve the server address and port
	iResult = getaddrinfo("127.0.0.1", portStr, &hints, &server);
	if (iResult != 0)
	{
		Debug(FString::Printf(TEXT("getaddrinfo failed with error: %d\n"), iResult));
		WSACleanup();
		return;
	}

	// Create a SOCKET for connecting to server
	mSocket = socket(server->ai_family, server->ai_socktype, server->ai_protocol);
	if (mSocket == INVALID_SOCKET)
	{
		Debug(FString::Printf(TEXT("socket failed with error: %ld\n"), WSAGetLastError()));
		WSACleanup();
		return;
	}

	// Connect to server.
	iResult = connect(mSocket, server->ai_addr, (int)server->ai_addrlen);
	if (iResult == SOCKET_ERROR)
	{
		closesocket(mSocket);
		mSocket = INVALID_SOCKET;
	}

	freeaddrinfo(server);

	if (mSocket == INVALID_SOCKET)
	{
		Debug(FString::Printf(TEXT("Unable to connect to server!\n")));
		WSACleanup();
		return;
	}

	Send(FString(TEXT("Unreal Started")));

	// Receive until the peer closes the connection or application is shut down
	do
	{
		if (recv(mSocket, recvbuf, recvbuflen, 0) > 0)
		{
			FString buffStr(recvbuf);
			Debug(FString::Printf(TEXT("Received packet : %s\n"), *buffStr));
			sInputQueue.Enqueue(new FString(recvbuf));
		}
		else
		{
			int lastError = WSAGetLastError();
			if (lastError == 10054)
			{
				Debug(FString::Printf(TEXT("Connection closed by chimera\n")));
			}
			else if (lastError == 10053)
			{
				Debug(FString::Printf(TEXT("Connection closed as part of Unreal shutdown\n")));
			}
			else
				Debug(FString::Printf(TEXT("recv failed with error: %d\n"), lastError));
			StopTaskCounter.Increment();
		}

	} while (StopTaskCounter.GetValue() == 0);
}

bool FSocketListener::CheckNextPacket()
{
	if (sInputQueue.IsEmpty())
	{
		return false;
	}

	if (sNextString != NULL)
	{
		delete sNextString;
		sNextString = NULL;
	}

	if (!sInputQueue.Dequeue(sNextString))
	{
		return false;
	}


	if (sNextString->Equals(FString(TEXT("ExitUnreal"))) && sWorld)
	{
		FGenericPlatformMisc::RequestExit(false);
		sWorld->Exec(sWorld, TEXT("exit"));
		return false;
	}

	else if (sNextString->StartsWith(FString(TEXT("~Position"))))
	{
		FString xStr, yStr, pos, arg;
		sNextString->Split(TEXT(" "), &arg, &pos);
		pos.Split(TEXT(","), &xStr, &yStr);
		int32 x = FCString::Atoi(*xStr);
		int32 y = FCString::Atoi(*yStr);
		Debug(FString::Printf(TEXT("Positioning window at %i,%i"), x, y));
		if (sDebug && GEngine)
		{
			GEngine->GameUserSettings->SetWindowPosition(x, y);
			//GEngine->GameUserSettings->ConfirmVideoMode();
			//GEngine->GameUserSettings->SaveSettings();
			GEngine->GameUserSettings->ApplySettings(false);
		}
	}
	else if (sNextString->StartsWith(FString(TEXT("~console "))))
	{
		FString cmd = sNextString->RightChop(9);
		Debug(FString::Printf(TEXT("Executing: %s"), *cmd));
		//sWorld->Exec(sWorld, *cmd);
		if (sWorld)
			sWorld->GetFirstPlayerController()->ConsoleCommand(*cmd);
		return false;
	}
	else if (sNextString->Equals(FString(TEXT("C# Started"))))
	{
		Debug(FString::Printf(TEXT("Received startup ack from Chimera.")));
		return false;
	}

	return true;
}

void FSocketListener::Debug(FString str)
{
	UE_LOG(LogTemp, Warning, TEXT("%s"), *str);
	//sWorld->GetGameViewport()->ViewportConsole->OutputText("Positioning window at %i,%i");
	//UGameViewportClient *viewport = sWorld->GetGameViewport();
	//UConsole *console = viewport->ViewportConsole;
	//console->OutputText("Positioning window at %i,%i");
	if (GEngine && sDebug)
	{
		//key, time, colour, msg
		GEngine->AddOnScreenDebugMessage(-1, 10, FColor::White, *str);
	}

}

void FSocketListener::Send(FString str)
{
	Send(TCHAR_TO_ANSI(*str), str.Len());
}

void FSocketListener::Send(char *data, int length)
{
	if (StopTaskCounter.GetValue() != 0)
	{
		Debug(FString::Printf(TEXT("Unable to send %s, socket has been closed."), *FString(data)));
		return;
	}

	//if (sendto(mSocket, data, length, 0, (struct sockaddr*) &si_other, slen) == SOCKET_ERROR)
	if (send(mSocket, data, length, 0) == SOCKET_ERROR)
	{
		Debug(FString::Printf(TEXT("send() failed with error code : %d"), WSAGetLastError()));
		return;
	}
	else
	{
		// TODO: Commented out to compile - please update 
		//FString sNextString(data);
		//Debug(FString::Printf(TEXT("Sent %s to %i"), *sNextString, sPort));
	}
}

bool FSocketListener::HasString()
{
	return CheckNextPacket();
}


FString FSocketListener::NextString()
{
	return *sNextString;
}
#endif