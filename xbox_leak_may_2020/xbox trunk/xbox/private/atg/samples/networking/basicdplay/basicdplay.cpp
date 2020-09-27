//-----------------------------------------------------------------------------
// File: basicdplay.cpp
//
// Desc: illustrates the absolute basics of how to get a dplay session going
//
// Note: In order for this to run correctly, the LOCALIP define must be
//       set to the IP address of the XBOX running this sample.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include <xtl.h>
#include <dplay8.h>
#include <assert.h>

GUID g_guidApp = {0x88e835d, 0x9179, 0x485f, { 0x83, 0x43, 0x90, 0x1d, 0x32, 0x7c, 0xe7, 0x94 }};

LPDIRECTPLAY8PEER g_pHostDP;					// DirectPlay peer object for host
LPDIRECTPLAY8PEER g_pClientDP;					// DirectPlay peer object for client
LPDIRECTPLAY8ADDRESS g_pDeviceAddress;			// client device address
LPDIRECTPLAY8ADDRESS g_pHostAddress;			// client host address

BOOL g_bClientConnected = FALSE;				// connected flag

#define LOCALIP L"157.56.11.134"
#define HOSTPORT 2350							// port to host game on


//-----------------------------------------------------------------------------
// DPlay message handlers for the host and client
//-----------------------------------------------------------------------------
HRESULT WINAPI HostDPMsgHandler(PVOID pvUserContext, DWORD dwMsgType, PVOID pMsg)
{
	switch(dwMsgType)
	{
        case DPN_MSGID_CONNECT_COMPLETE:
			OUTPUT_DEBUG_STRING( "host: connect complete\n" );
			break;

        case DPN_MSGID_CREATE_PLAYER:
			OUTPUT_DEBUG_STRING( "host: create player\n" );
			break;

        case DPN_MSGID_DESTROY_PLAYER:
			OUTPUT_DEBUG_STRING( "host: destroy player\n" );
			break;

        case DPN_MSGID_RECEIVE:
			OUTPUT_DEBUG_STRING( "host: receive\n" );
			break;
	}

	return S_OK;
}

HRESULT WINAPI ClientDPMsgHandler(PVOID pvUserContext, DWORD dwMsgType, PVOID pMsg)
{
	switch(dwMsgType)
	{
        case DPN_MSGID_CONNECT_COMPLETE:
			OUTPUT_DEBUG_STRING( "client: connect complete\n" );
			g_bClientConnected = TRUE;
			break;

        case DPN_MSGID_CREATE_PLAYER:
			OUTPUT_DEBUG_STRING( "client: create player\n" );
			break;

        case DPN_MSGID_DESTROY_PLAYER:
			OUTPUT_DEBUG_STRING( "client: destroy player\n" );
			break;

        case DPN_MSGID_RECEIVE:
			OUTPUT_DEBUG_STRING( "client: receive\n" );
			break;
	}

	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: DPDoWork()
// Desc: Does DPlay processing until there is no work to be done
//-----------------------------------------------------------------------------
HRESULT DPDoWork(LPDIRECTPLAY8PEER pDP)
{
	HRESULT hr;
	DWORD time;

	time = GetTickCount();

	do
		hr = pDP->DoWork(0);
	while((GetTickCount()-time)<20);

	return hr;
}


//-----------------------------------------------------------------------------
// Name: DPInit()
// Desc: Performs essential DPlay initialization
//-----------------------------------------------------------------------------
HRESULT DPInit()
{
	HRESULT hr;
	int err;

    OUTPUT_DEBUG_STRING( "dplay: initializing\n" );

	// initialize Xnet
	err = XnetInitialize(NULL, TRUE);
	assert(err==NO_ERROR);

	// initialize DPlay
	hr = DPlayInitialize(200000);
	assert(SUCCEEDED(hr));

	// create DPlay Peer object for host and client
    hr = DirectPlay8Create(IID_IDirectPlay8Peer, (void **)&g_pHostDP, NULL);
	assert(SUCCEEDED(hr));
    hr = DirectPlay8Create(IID_IDirectPlay8Peer, (void **)&g_pClientDP, NULL);
	assert(SUCCEEDED(hr));

    // Init IDirectPlay8Peer for host and client
    hr = g_pHostDP->Initialize(NULL, HostDPMsgHandler, 0);
	assert(SUCCEEDED(hr));
    hr = g_pClientDP->Initialize(NULL, ClientDPMsgHandler, 0);
	assert(SUCCEEDED(hr));

	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: DPHost()
// Desc: Hosts a game
//-----------------------------------------------------------------------------
HRESULT DPHost()
{
	HRESULT hr;
    DPN_PLAYER_INFO dpPlayerInfo;
    DPN_APPLICATION_DESC dnAppDesc;
	DWORD dwPort = HOSTPORT;
    DPNHANDLE hAsync;
	LPDIRECTPLAY8ADDRESS pDeviceAddress;

    OUTPUT_DEBUG_STRING( "dplay: hosting\n" );

	// set up player info for ourself as we create the game
    ZeroMemory(&dpPlayerInfo, sizeof(DPN_PLAYER_INFO));
    dpPlayerInfo.dwSize = sizeof(DPN_PLAYER_INFO);
    dpPlayerInfo.dwInfoFlags = DPNINFO_NAME;
    dpPlayerInfo.pwszName = L"Host";

	// set peer info. this is sent to all other machines via
	// the CREATE_PLAYER message
    hr = g_pHostDP->SetPeerInfo(&dpPlayerInfo, NULL, &hAsync, 0);
	assert(SUCCEEDED(hr));

    // Setup the application description
    ZeroMemory(&dnAppDesc, sizeof(DPN_APPLICATION_DESC));
    dnAppDesc.dwSize = sizeof(DPN_APPLICATION_DESC);
    dnAppDesc.guidApplication = g_guidApp;
    dnAppDesc.dwMaxPlayers = 2;
    dnAppDesc.dwFlags = 0;

	// set the name of the game
    dnAppDesc.pwszSessionName = L"Basic DPlay";

	// create device address
    hr = DirectPlay8AddressCreate(IID_IDirectPlay8Address, (void **)&pDeviceAddress, NULL);
	assert(SUCCEEDED(hr));

    // Host a game on g_pDeviceAddress as described by dnAppDesc
	pDeviceAddress->AddComponent(DPNA_KEY_PORT, (void *)&dwPort, sizeof(dwPort), DPNA_DATATYPE_DWORD);
	assert(SUCCEEDED(hr));

	// make the call to host
    hr = g_pHostDP->Host(&dnAppDesc, &pDeviceAddress, 1, NULL, NULL, NULL, 0);
	assert(SUCCEEDED(hr));

	// release the device address
	pDeviceAddress->Release();

	return hr;
}

//-----------------------------------------------------------------------------
// Name: DPConnect()
// Desc: Connects to the hosted game
//-----------------------------------------------------------------------------
HRESULT DPConnect()
{
	HRESULT hr;
    DPNHANDLE hAsync;
	DPN_APPLICATION_DESC dnAppDesc;

    OUTPUT_DEBUG_STRING( "dplay: connecting\n" );

	// Set player info
	DPN_PLAYER_INFO dpPlayerInfo;
	ZeroMemory(&dpPlayerInfo, sizeof(DPN_PLAYER_INFO));
	dpPlayerInfo.dwSize = sizeof(DPN_PLAYER_INFO);
	dpPlayerInfo.dwInfoFlags = DPNINFO_NAME;
	dpPlayerInfo.pwszName = L"Client";

	// set the peer info for the client
	hr = g_pClientDP->SetPeerInfo(&dpPlayerInfo, NULL, &hAsync, 0);
	assert(SUCCEEDED(hr));

    // Setup the application description
    ZeroMemory(&dnAppDesc, sizeof(DPN_APPLICATION_DESC));
    dnAppDesc.dwSize = sizeof(DPN_APPLICATION_DESC);
    dnAppDesc.guidApplication = g_guidApp;

	// create device address
    hr = DirectPlay8AddressCreate(IID_IDirectPlay8Address, (void **)&g_pDeviceAddress, NULL);
	assert(SUCCEEDED(hr));

	// create host address
    hr = DirectPlay8AddressCreate(IID_IDirectPlay8Address, (void **)&g_pHostAddress, NULL);
	assert(SUCCEEDED(hr));

	// build address to find the host
    g_pHostAddress->BuildAddress(LOCALIP, HOSTPORT);

	// connect to the game
	hr = g_pClientDP->Connect( 
				&dnAppDesc,						// the application description
				g_pHostAddress,					// address of the host of the session
				g_pDeviceAddress,				// address of the local device
				NULL, NULL,						// DPN_SECURITY_DESC, DPN_SECURITY_CREDENTIALS
				NULL, 0,						// user data, user data size
				NULL,							// player context,
				NULL, &hAsync,					// async context, async handle,
				0);								// flags

	assert(hr==E_PENDING || SUCCEEDED(hr));

	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: DPSendMessage()
// Desc: Sends a message to all players in the game
//-----------------------------------------------------------------------------
HRESULT DPSendMessage(LPDIRECTPLAY8PEER pDP)
{
	HRESULT hr;
	DPN_BUFFER_DESC bufferDesc;
	DPNHANDLE hAsync;
	DWORD dwMsg;

	dwMsg = 11;
	bufferDesc.dwBufferSize = sizeof(DWORD);
	bufferDesc.pBufferData = (BYTE *)&dwMsg;

	hr = pDP->SendTo(DPNID_ALL_PLAYERS_GROUP, &bufferDesc, 1, 
			0, NULL, &hAsync, DPNSEND_NOLOOPBACK );
	assert(SUCCEEDED(hr));

    return hr;
}

//-----------------------------------------------------------------------------
// Name: DPDisconnect()
// Desc: Disconnects the client from the session
//-----------------------------------------------------------------------------
HRESULT	DPDisconnect()
{
	HRESULT hr;

    OUTPUT_DEBUG_STRING( "client: disconnecting\n" );

	hr = g_pClientDP->Close(0);
	assert(SUCCEEDED(hr));

	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: DPTerminate()
// Desc: Terminates the session by the host
//-----------------------------------------------------------------------------
HRESULT DPTerminate()
{
	HRESULT hr;

    OUTPUT_DEBUG_STRING( "host: terminating session\n" );

	hr = g_pHostDP->TerminateSession(NULL, 0, 0);
	assert(SUCCEEDED(hr));

	return S_OK;
}


//-----------------------------------------------------------------------------
// Name: main()
// Desc: everyone knows what main does...
//-----------------------------------------------------------------------------
void __cdecl main()
{
	// init dplay
	DPInit();

	// host a game
	DPHost();
	DPDoWork(g_pHostDP);

	// connect to hosted game
	DPConnect();

	// wait for connect to complete
	while(!g_bClientConnected)
		DPDoWork(g_pClientDP);

	// send a message from the client to the host
	DPSendMessage(g_pClientDP);
	DPDoWork(g_pClientDP);

	// send a message from the host to the client
	DPSendMessage(g_pHostDP);
	DPDoWork(g_pHostDP);

	// disconnect the client from the game
	DPDisconnect();

	// have the host terminate the session
	DPTerminate();

	// wait here a sec...
	_asm int 3;
}


