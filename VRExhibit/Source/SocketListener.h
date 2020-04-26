#pragma once

#include "VRExhibit.h"
#include "HAL/ThreadingBase.h"
#include "GameFramework/Actor.h"

#if PLATFORM_WINDOWS
#include "Windows/AllowWindowsPlatformTypes.h"
#include <winsock2.h>
#include "Windows/HideWindowsPlatformTypes.h"
#endif

#define DEFAULT_BUFLEN 512

/*~~~~~ Multi Threading ~~~ */
class FSocketListener : public FRunnable
{
	/* Singleton instance, can access the thread any time via static accessor, if it is active! */
	static  FSocketListener* Runnable;

#if PLATFORM_WINDOWS
	/* Thread to run the worker FRunnable on */
	FRunnableThread* Thread;

	// Stop this thread? Uses Thread Safe Counter */
	FThreadSafeCounter StopTaskCounter;

	/* Start a socket to listen for incoming packets */
	void StartSocket();

private:

	/* Socket that will request a connection with the external application.  */
	SOCKET mSocket;

	/* The buffer where the messages will be stored on receipt. */
	char recvbuf[DEFAULT_BUFLEN];

	/* The port for the external application that this socket will connect to. */
	static int sPort;

	/* */
	static bool sDebug;

	/* The next string that has been received. */
	static FString *sNextString;

	/* Queue of strings waiting to be processed. */
	static TQueue<FString*> sInputQueue;

	/* Whether the process was stopped. */
	static bool sWasStopped;

	/* The world object for interacting with the individual world running. */
	static UWorld *sWorld;


	/* Makes sure this thread has stopped properly */
	void ForceShutdown();

	/** Make sure the socket is properly disposed of */
	void ShutdownSocket();


	/**
	Process a packet once it has been received.
	Turn it into a string and find out if it is a command packet or one that should be passed to the blueprint.
	*/
	static bool CheckNextPacket();

	/** Print a string for debug purposes. */
	static void Debug(FString str);

public:

	// Begin FRunnable interface.
	virtual bool Init();
	virtual uint32 Run();
	virtual void Stop();
	// End FRunnable interface

	//~~~ Starting and Stopping Thread ~~~
	/** Get the listener that will communicate with Chimera. If the thread doesn't exist start it.  */
	static FSocketListener* GetPipe();

	/** Initialise the socket, specifying the port that it should send packets TO */
	static void InitialisePipe(int remotePort, AActor *anyActor, bool debug);

	/** Shuts down the thread. Static so it can easily be called from outside the thread context */
	static void Shutdown();


	/** Send raw bytes to Chimera */
	void Send(char *data, int length);

	/** Send a string to Chimera */
	void Send(FString str);


	/** A string has been received and is waiting on the queue */
	static bool HasString();

	/** Dequeue the next string from the queue and return it */
	static FString NextString();
#endif
public:

	//~~~ Thread Core Functions ~~~

	//Constructor / Destructor
	FSocketListener();
	virtual ~FSocketListener();
};

