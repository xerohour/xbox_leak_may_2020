//-----------------------------------------------------------------------------
// File: dpmemtest.cpp
//
// Desc: Basic executable to measure dynamic DirectPlay memory usage
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#define INITGUID
#include <xtl.h>
#include <dplay8.h>
#include <dpaddr.h>
#include <stdio.h>
#include <xbdm.h>

#define USE_PERF_COUNTERS

// {ECD3B292-B7B6-447f-AA2A-D2F6ABF34CB1}
DEFINE_GUID(GUID_BASEAPP,
0xecd3b292, 0xb7b6, 0x447f, 0xaa, 0x2a, 0xd2, 0xf6, 0xab, 0xf3, 0x4c, 0xb1);

// Define only one of the following
#define CALL_PEER_FUNCTIONS
//#define CALL_CLIENT_FUNCTIONS
//#define CALL_SERVER_FUNCTIONS
//#define CALL_ADDRESS_FUNCTIONS

typedef enum _MEMORY_STAT_TYPE
{
	MIN_STAT_TYPE = 0,
	FREE_PAGES = 0,
	CACHE_PAGES,
	VIRT_PAGES,
	POOL_PAGES,
	STACK_PAGES,
	IMAGE_PAGES,
	MAX_STAT_TYPE
} MEMORY_STAT_TYPE;

HANDLE g_MemStatHandles[MAX_STAT_TYPE];
LPCSTR g_MemStatStrings[] =
{
	"Free Pages",
	"Cache Pages",
	"Virtual Pages",
	"Pool Pages",
	"Stack Pages",
	"Image Pages"
};



//==================================================================================
// TRACE
//----------------------------------------------------------------------------------
//
// Description: Logs a string with variable parameters to the debugger
//
// Arguments:
//	LPCSTR		szFormat		Formatting string for debugger output
//
// Returns: nothing
//==================================================================================
void WINAPI TRACE(LPCSTR szFormat, ...) {
	CHAR szBuffer[1024] = "";
	va_list pArgs; 
	va_start(pArgs, szFormat);
	
	vsprintf(szBuffer, szFormat, pArgs);
	
	va_end(pArgs);
	OutputDebugString(szBuffer);
}

//==================================================================================
// InitMemStatHandles
//----------------------------------------------------------------------------------
//
// Description: Looks up all the memory stat handles that we need for perf counter APIs
//
// Arguments: none
//
// Returns: nothing
//==================================================================================
void InitMemStatHandles()
{
	for(INT i = MIN_STAT_TYPE;i < MAX_STAT_TYPE;++i)
	{
		g_MemStatHandles[i] = NULL;
		DmOpenPerformanceCounter(g_MemStatStrings[i], &(g_MemStatHandles[i]));
	}
}

//==================================================================================
// CloseMemStatHandles
//----------------------------------------------------------------------------------
//
// Description: Closes all the memory stat handles that we needed for perf counter APIs
//
// Arguments: none
//
// Returns: nothing
//==================================================================================
void CloseMemStatHandles()
{
	for(INT i = MIN_STAT_TYPE;i < MAX_STAT_TYPE;++i)
	{
		DmClosePerformanceCounter(g_MemStatHandles[i]);
		g_MemStatHandles[i] = NULL;
	}
}

//==================================================================================
// LogMemoryStatus
//----------------------------------------------------------------------------------
//
// Description: Logs the current memory status to the debugger
//
// Arguments: none
//
// Returns: nothing
//==================================================================================
void LogMemoryStatus()
{
#ifdef USE_PERF_COUNTERS
	DM_COUNTDATA		CountData;
	HANDLE				hCounter = NULL;

	TRACE("Memory status:\n");
	for(INT i = MIN_STAT_TYPE;i < MAX_STAT_TYPE;++i)
	{
		DmQueryPerformanceCounter(g_MemStatHandles[i], DMCOUNT_VALUE | DMCOUNT_AVERAGE, &CountData);

		TRACE("    %s: %u\n", g_MemStatStrings[i], CountData.CountValue.QuadPart);

		hCounter = NULL;
	}
#elif
	MEMORYSTATUS MemoryStatus;

	memset(&MemoryStatus, 0, sizeof(MEMORYSTATUS));
	MemoryStatus.dwLength = sizeof(MEMORYSTATUS);

	GlobalMemoryStatus (&MemoryStatus);

	TRACE("Memory status:\n");
	TRACE("    Available physical: %u\n", MemoryStatus.dwAvailPhys);

#endif
}

//==================================================================================
// DirectPlayMessageHandler
//----------------------------------------------------------------------------------
//
// Description: Callback that DirectPlay8 will use to notify us of events.
//
// Arguments:
//	PVOID pvContext		Pointer to context specified when this handler was
//						specified.
//	DWORD dwMsgType		Type of message being indicated.
//	PVOID pvMsg			Depends on type of message.
//
// Returns: depends on message (usually DPN_OK).
//==================================================================================
HRESULT DirectPlayMessageHandler(PVOID pvContext, DWORD dwMsgType, PVOID pvMsg)
{
	HRESULT						hr = DPN_OK;

	switch (dwMsgType)
	{
		case DPN_MSGID_CONNECT_COMPLETE:
		case DPN_MSGID_CREATE_PLAYER:
		case DPN_MSGID_DESTROY_PLAYER:
		case DPN_MSGID_INDICATE_CONNECT:
			break;
		default:
			break;
	}

	return (hr);
}

//-----------------------------------------------------------------------------
// Name: main()
// Desc: everyone knows what main does...
//-----------------------------------------------------------------------------
void __cdecl main()
{
	DPN_APPLICATION_DESC	dpnAppDesc;
	PDIRECTPLAY8ADDRESS		pDeviceAddress = NULL, pHostAddress = NULL;
	DPN_BUFFER_DESC			dpnBufferDesc;
	DPNHANDLE				dpnHandle = NULL;
	WCHAR					szHostname[] = L"xboxnetsync1";
	BYTE					DataBuffer[100];
	HRESULT					hr = DPN_OK;
	INT						n = 0;

	InitMemStatHandles();

	memset(DataBuffer, 0, 100);
	memset(&dpnBufferDesc, 0, sizeof(DPN_BUFFER_DESC));
	memset(&dpnAppDesc, 0, sizeof(DPN_APPLICATION_DESC));

	TRACE("\nTest started...\n");
	LogMemoryStatus();

	XnetInitialize(NULL, TRUE);

	TRACE("\nXnet initialized...\n");
	LogMemoryStatus();

	hr = DPlayInitialize(1024 * 200);
	if(hr != DPN_OK)
		goto Exit;

	TRACE("\nDPlay initialized...\n");
	LogMemoryStatus();

	hr = DirectPlay8AddressCreate(IID_IDirectPlay8Address, (VOID **) &pDeviceAddress, NULL);
	if(hr != DPN_OK)
		goto Exit;

	hr = DirectPlay8AddressCreate(IID_IDirectPlay8Address, (VOID **) &pHostAddress, NULL);
	if(hr != DPN_OK)
		goto Exit;

	TRACE("\nDevice and host addresses created...\n");
	LogMemoryStatus();

#ifdef CALL_PEER_FUNCTIONS
	IDirectPlay8Peer *pPeer = NULL;

	hr = DirectPlay8Create(IID_IDirectPlay8Peer, (LPVOID *) &pPeer, NULL);
	if(hr != DPN_OK)
		goto Exit;

	TRACE("\nPeer object created...\n");
	LogMemoryStatus();

	hr = pPeer->Initialize(NULL, DirectPlayMessageHandler, 0);
	if(hr != DPN_OK)
		goto Exit;

	TRACE("\nInitialized peer object...\n");
	LogMemoryStatus();

	memset(&dpnAppDesc, 0, sizeof(DPN_APPLICATION_DESC));
	dpnAppDesc.dwSize = sizeof(DPN_APPLICATION_DESC);
	
	hr = pPeer->EnumHosts(&dpnAppDesc, NULL, pDeviceAddress, (PVOID) DataBuffer, 100, 0, 0, 0, NULL, &dpnHandle, 0);
	if((hr != DPNSUCCESS_PENDING) && (hr != DPN_OK))
		goto Exit;

	TRACE("\nStarted host enumeration...\n");
	LogMemoryStatus();

	while((hr = pPeer->DoWork(0)) != S_FALSE)
	{
		if(hr !=  DPN_OK)
			goto Exit;
	}

	TRACE("\nCalled DoWork until S_FALSE after enumeration...\n");
	LogMemoryStatus();

	hr = pPeer->CancelAsyncOperation(NULL, DPNCANCEL_ENUM);
	if(hr != DPN_OK)
		goto Exit;

	TRACE("\nCalled CancelAsyncOperation on enumeration...\n");
	LogMemoryStatus();

	while((hr = pPeer->DoWork(0)) != S_FALSE)
	{
		if(hr !=  DPN_OK)
			goto Exit;
	}

	TRACE("\nCalled DoWork until S_FALSE after canceling enumeration...\n");
	LogMemoryStatus();

	hr = pPeer->Close(0);
	if(hr != DPN_OK)
		goto Exit;
	
	TRACE("\nClosed peer object...\n");
	LogMemoryStatus();

	hr = pPeer->Initialize(NULL, DirectPlayMessageHandler, 0);
	if(hr != DPN_OK)
		goto Exit;

	TRACE("\nInitialized peer object...\n");
	LogMemoryStatus();

	hr = pHostAddress->BuildAddress(szHostname, 2302);
	if(hr != DPN_OK)
		goto Exit;

	TRACE("\nCalled BuildAddress...\n");
	LogMemoryStatus();

	memset(&dpnAppDesc, 0, sizeof(DPN_APPLICATION_DESC));
	dpnAppDesc.dwSize = sizeof(DPN_APPLICATION_DESC);
	dpnAppDesc.guidApplication = GUID_BASEAPP;

	hr = pPeer->Connect(&dpnAppDesc, pHostAddress, pDeviceAddress, NULL, NULL, (PVOID) DataBuffer, 100, NULL, NULL, &dpnHandle, 0);
	if((hr != DPNSUCCESS_PENDING) && (hr != DPN_OK))
		goto Exit;

	TRACE("\nStarted connection...\n");
	LogMemoryStatus();

	while((hr = pPeer->DoWork(0)) != S_FALSE)
	{
		if(hr !=  DPN_OK)
			goto Exit;
	}

	TRACE("\nCalled DoWork until S_FALSE after connection...\n");
	LogMemoryStatus();

	hr = pHostAddress->Clear();
	if(hr != DPN_OK)
		goto Exit;

	TRACE("\nCleared host address...\n");
	LogMemoryStatus();

	hr = pPeer->CancelAsyncOperation(NULL, DPNCANCEL_CONNECT);
	if(hr != DPN_OK)
		goto Exit;

	TRACE("\nCalled CancelAsyncOperation on connection...\n");
	LogMemoryStatus();

	while((hr = pPeer->DoWork(0)) != S_FALSE)
	{
		if(hr !=  DPN_OK)
			goto Exit;
	}

	TRACE("\nCalled DoWork until S_FALSE after canceling connection...\n");
	LogMemoryStatus();

	hr = pPeer->Close(0);
	if(hr != DPN_OK)
		goto Exit;
	
	TRACE("\nClosed peer object...\n");
	LogMemoryStatus();

	hr = pPeer->Initialize(NULL, DirectPlayMessageHandler, 0);
	if(hr != DPN_OK)
		goto Exit;

	TRACE("\nInitialized peer object...\n");
	LogMemoryStatus();

	memset(&dpnAppDesc, 0, sizeof(DPN_APPLICATION_DESC));
	dpnAppDesc.dwSize = sizeof(DPN_APPLICATION_DESC);
	dpnAppDesc.guidApplication = GUID_BASEAPP;

	hr = pPeer->Host(&dpnAppDesc, &pDeviceAddress, 1, NULL, NULL, NULL, 0);
	if(hr != DPN_OK)
		goto Exit;

	TRACE("\nStarted hosting...\n");
	LogMemoryStatus();

	while((hr = pPeer->DoWork(0)) != S_FALSE)
	{
		if(hr !=  DPN_OK)
			goto Exit;
	}

	TRACE("\nCalled DoWork until S_FALSE after hosting...\n");
	LogMemoryStatus();

	memset(&dpnBufferDesc, 0, sizeof(DPN_BUFFER_DESC));
	dpnBufferDesc.dwBufferSize = 100;
	dpnBufferDesc.pBufferData = DataBuffer;

	hr = pPeer->SendTo(0, &dpnBufferDesc, 1, 0, NULL, &dpnHandle, 0);
	if((hr != DPNSUCCESS_PENDING) && (hr != DPN_OK))
		goto Exit;

	TRACE("\nStarted sending...\n");
	LogMemoryStatus();

	while((hr = pPeer->DoWork(0)) != S_FALSE)
	{
		if(hr !=  DPN_OK)
			goto Exit;
	}

	TRACE("\nCalled DoWork until S_FALSE after sending...\n");
	LogMemoryStatus();

	hr = pPeer->Close(0);
	if(hr != DPN_OK)
		goto Exit;
	
	TRACE("\nClosed peer object...\n");
	LogMemoryStatus();
#endif

#ifdef CALL_CLIENT_FUNCTIONS
	IDirectPlay8Client *pClient = NULL;

	hr = DirectPlay8Create(IID_IDirectPlay8Client, (LPVOID *) &pClient, NULL);
	if(hr != DPN_OK)
		goto Exit;

	TRACE("\nClient object created...\n");
	LogMemoryStatus();

	hr = pClient->Initialize(NULL, DirectPlayMessageHandler, 0);
	if(hr != DPN_OK)
		goto Exit;

	TRACE("\nInitialized client object...\n");
	LogMemoryStatus();

	memset(&dpnAppDesc, 0, sizeof(DPN_APPLICATION_DESC));
	dpnAppDesc.dwSize = sizeof(DPN_APPLICATION_DESC);
	
	hr = pClient->EnumHosts(&dpnAppDesc, NULL, pDeviceAddress, (PVOID) DataBuffer, 100, 0, 0, 0, NULL, &dpnHandle, 0);
	if((hr != DPNSUCCESS_PENDING) && (hr != DPN_OK))
		goto Exit;

	TRACE("\nStarted host enumeration...\n");
	LogMemoryStatus();

	while((hr = pClient->DoWork(0)) != S_FALSE)
	{
		if(hr !=  DPN_OK)
			goto Exit;
	}

	TRACE("\nCalled DoWork until S_FALSE after enumeration...\n");
	LogMemoryStatus();

	hr = pClient->CancelAsyncOperation(NULL, DPNCANCEL_ENUM);
	if(hr != DPN_OK)
		goto Exit;

	TRACE("\nCalled CancelAsyncOperation on enumeration...\n");
	LogMemoryStatus();

	while((hr = pClient->DoWork(0)) != S_FALSE)
	{
		if(hr !=  DPN_OK)
			goto Exit;
	}

	TRACE("\nCalled DoWork until S_FALSE after canceling enumeration...\n");
	LogMemoryStatus();

	hr = pClient->Close(0);
	if(hr != DPN_OK)
		goto Exit;
	
	TRACE("\nClosed client object...\n");
	LogMemoryStatus();

	hr = pClient->Initialize(NULL, DirectPlayMessageHandler, 0);
	if(hr != DPN_OK)
		goto Exit;

	TRACE("\nInitialized client object...\n");
	LogMemoryStatus();

	hr = pHostAddress->BuildAddress(szHostname, 2302);
	if(hr != DPN_OK)
		goto Exit;

	TRACE("\nCalled BuildAddress...\n");
	LogMemoryStatus();

	memset(&dpnAppDesc, 0, sizeof(DPN_APPLICATION_DESC));
	dpnAppDesc.dwSize = sizeof(DPN_APPLICATION_DESC);
	dpnAppDesc.guidApplication = GUID_BASEAPP;

	hr = pClient->Connect(&dpnAppDesc, pHostAddress, pDeviceAddress, NULL, NULL, (PVOID) DataBuffer, 100, NULL, &dpnHandle, 0);
	if((hr != DPNSUCCESS_PENDING) && (hr != DPN_OK))
		goto Exit;

	TRACE("\nStarted connection...\n");
	LogMemoryStatus();

	while((hr = pClient->DoWork(0)) != S_FALSE)
	{
		if(hr !=  DPN_OK)
			goto Exit;
	}

	TRACE("\nCalled DoWork until S_FALSE after connection...\n");
	LogMemoryStatus();

	hr = pHostAddress->Clear();
	if(hr != DPN_OK)
		goto Exit;

	TRACE("\nCleared host address...\n");
	LogMemoryStatus();

	hr = pClient->CancelAsyncOperation(NULL, DPNCANCEL_CONNECT);
	if(hr != DPN_OK)
		goto Exit;

	TRACE("\nCalled CancelAsyncOperation on connection...\n");
	LogMemoryStatus();

	while((hr = pClient->DoWork(0)) != S_FALSE)
	{
		if(hr !=  DPN_OK)
			goto Exit;
	}

	TRACE("\nCalled DoWork until S_FALSE after canceling connection...\n");
	LogMemoryStatus();

	hr = pClient->Close(0);
	if(hr != DPN_OK)
		goto Exit;
	
	TRACE("\nClosed client object...\n");
	LogMemoryStatus();
#endif

#ifdef CALL_SERVER_FUNCTIONS
	IDirectPlay8Server *pServer = NULL;
	
	hr = DirectPlay8Create(IID_IDirectPlay8Server, (LPVOID *) &pServer, NULL);
	if(hr != DPN_OK)
		goto Exit;

	TRACE("\nServer object created...\n");
	LogMemoryStatus();

	hr = pServer->Initialize(NULL, DirectPlayMessageHandler, 0);
	if(hr != DPN_OK)
		goto Exit;

	TRACE("\nInitialized server object...\n");
	LogMemoryStatus();

	memset(&dpnAppDesc, 0, sizeof(DPN_APPLICATION_DESC));
	dpnAppDesc.dwSize = sizeof(DPN_APPLICATION_DESC);
	dpnAppDesc.guidApplication = GUID_BASEAPP;
	dpnAppDesc.dwFlags = DPNSESSION_CLIENT_SERVER;

	hr = pServer->Host(&dpnAppDesc, &pDeviceAddress, 1, NULL, NULL, NULL, 0);
	if(hr != DPN_OK)
		goto Exit;

	TRACE("\nStarted hosting...\n");
	LogMemoryStatus();

	while((hr = pServer->DoWork(0)) != S_FALSE)
	{
		if(hr !=  DPN_OK)
			goto Exit;
	}

	TRACE("\nCalled DoWork until S_FALSE after hosting...\n");
	LogMemoryStatus();

	memset(&dpnBufferDesc, 0, sizeof(DPN_BUFFER_DESC));
	dpnBufferDesc.dwBufferSize = 100;
	dpnBufferDesc.pBufferData = DataBuffer;

	hr = pServer->SendTo(0, &dpnBufferDesc, 1, 0, NULL, &dpnHandle, 0);
	if((hr != DPNSUCCESS_PENDING) && (hr != DPN_OK))
		goto Exit;

	TRACE("\nStarted sending...\n");
	LogMemoryStatus();

	while((hr = pServer->DoWork(0)) != S_FALSE)
	{
		if(hr !=  DPN_OK)
			goto Exit;
	}

	TRACE("\nCalled DoWork until S_FALSE after sending...\n");
	LogMemoryStatus();

	hr = pServer->Close(0);
	if(hr != DPN_OK)
		goto Exit;
	
	TRACE("\nClosed server object...\n");
	LogMemoryStatus();
#endif

#ifdef CALL_ADDRESS_FUNCTIONS
	IDirectPlay8Address *pAddress = NULL;

	hr = DirectPlay8AddressCreate(NULL, NULL, NULL);
	if(hr != DPN_OK)
		goto Exit;

	TRACE("\nAddress object created...\n");
	LogMemoryStatus();
#endif
	
	goto Exit;

Exit:

#ifdef CALL_PEER_FUNCTIONS
	pPeer->Release();

	TRACE("\nPeer object released...\n");
	LogMemoryStatus();

	pPeer = NULL;
#endif

#ifdef CALL_CLIENT_FUNCTIONS
	pClient->Release();

	TRACE("\nClient object released...\n");
	LogMemoryStatus();

	pClient = NULL;
#endif

#ifdef CALL_SERVER_FUNCTIONS
	pServer->Release();

	TRACE("\nServer object released...\n");
	LogMemoryStatus();

	pServer = NULL;
#endif

#ifdef CALL_ADDRESS_FUNCTIONS
	pAddress->Release();

	TRACE("\nAddress object released...\n");
	LogMemoryStatus();

	pAddress = NULL;
#endif

	pDeviceAddress->Release();
	pDeviceAddress = NULL;

	pHostAddress->Release();
	pHostAddress = NULL;

	TRACE("\nDevice and host address released...\n");
	LogMemoryStatus();

	DPlayCleanup();

	TRACE("\nDirectPlay cleaned up...\n");
	LogMemoryStatus();

	XnetCleanup();

	TRACE("\nXnet cleaned up...\n");
	LogMemoryStatus();

	CloseMemStatHandles();

	while(TRUE)
	{
		// Loop to infinity
	}
	
	__asm int 3;

}


