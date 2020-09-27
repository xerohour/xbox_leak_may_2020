//-----------------------------------------------------------------------------
// File: xdppeer.cpp
//
// Desc: DirectPlay peer routines
//
// Copyright (c) 1998-2000 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

// TODO : do 'connect' timeout

#include <assert.h>
#include "xdppeer.h"

LPDIRECTPLAY8PEER g_pDP;						// DirectPlay peer object
LPDIRECTPLAY8ADDRESS g_pDeviceAddress;			// device address of SP
LPDIRECTPLAY8ADDRESS g_pHostAddress;			// host address of SP

HRESULT (WINAPI *g_AppDPMsgHandler)(void *, DWORD, void *); // apps message handler
HRESULT WINAPI XDPMsgHandler(void *, DWORD, void *);

DPNHANDLE g_hAsyncOp;

DWORD g_dwConnectStatus;						// connection status
WCHAR g_wszXDPPlayerName[32];					// players name
WCHAR g_wszXDPSessionName[32];					// session name

// active game list stuff
ActiveGame *ActiveGames[MAX_ACTIVEGAMES];
DWORD g_dwNumActiveGames;
void XDPDeleteGames();

// DPNID stuff
DPNID g_dpnidLocalPlayer;
DPNID g_dpnidHost;

DWORD g_dwNumPlayers;

//-----------------------------------------------------------------------------
// Name: DPInit
// Desc: Initialize DirectPlay and other relevant stuff.
//-----------------------------------------------------------------------------
HRESULT XDPInit(HRESULT (WINAPI *DPMsgHandler)(void *, DWORD, void *))
{
    HRESULT hr;
	int err;

    OutputDebugStringA("xdppeer: initializing\n");

	// Save message handler pointer
	g_AppDPMsgHandler = DPMsgHandler;

	// initialize Xnet
	err = XNetStartup(NULL);
	assert(err==NO_ERROR);

	// initialize DPlay
	hr = DPlayInitialize(200000);
	assert(SUCCEEDED(hr));

	// create DPlay Peer object & get DVoice interface
    hr = DirectPlay8Create(IID_IDirectPlay8Peer, (void **) &g_pDP, NULL);
	assert(SUCCEEDED(hr));

    // Init IDirectPlay8Peer
    hr = g_pDP->Initialize(NULL, XDPMsgHandler, 0);
	assert(SUCCEEDED(hr));

	// create device address
    hr = DirectPlay8AddressCreate(IID_IDirectPlay8Address, (void **) &g_pDeviceAddress, NULL);
	assert(SUCCEEDED(hr));

	// create host address
    hr = DirectPlay8AddressCreate(IID_IDirectPlay8Address, (void **) &g_pHostAddress, NULL);
	assert(SUCCEEDED(hr));

	// clear connect state
	g_dwConnectStatus = GAME_NOTCONNECTED;

	// clear active games list
	ZeroMemory(ActiveGames, MAX_ACTIVEGAMES*sizeof(DPN_APPLICATION_DESC *));
	g_dwNumActiveGames = 0;

	// clear # of players
	g_dwNumPlayers = 0;

	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: XDPShutdown
// Desc: Close existing connection, do cleanup, release DPlay object,
//       and shut down COM.
//-----------------------------------------------------------------------------
void XDPShutdown()
{
	// close the connection
	// this blocks until all callbacks are completed
	g_pDP->Close(0);

	// release the address objects we are using
    if(g_pDeviceAddress)
		g_pDeviceAddress->Release();
    if(g_pHostAddress)
		g_pHostAddress->Release();

	// release our dplay object
	g_pDP->Release();

	// shut down DPlay
	DPlayCleanup();

	// clean up Xnet
    XNetCleanup();
}

//-----------------------------------------------------------------------------
// Name: XDPDoWork
// Desc: Does DPlay work.
//-----------------------------------------------------------------------------
HRESULT XDPDoWork(DWORD msec)
{
	DWORD time;
	static DWORD dwExpireTime = 0;
	HRESULT hr;

	// get current time in msec
	time = GetTickCount();

	// expire games if we are enumerating
	if(g_dwConnectStatus==GAME_ENUMERATING)
	{
		if(time>dwExpireTime)
		{
			dwExpireTime += 2000;	// 2 sec
			XDPExpireGames();
		}
	}

	// call the dplay DoWork function until work is done
	// or we have used up all available time
	if(msec==0)
	{
		do
			hr = g_pDP->DoWork(0);
		while(hr!=S_FALSE);
	}
	else
	{
		do
			hr = g_pDP->DoWork(0);
		while(hr!=S_FALSE && ((GetTickCount()-time)<msec));
	}

	// check for disconnect
	if(g_dwConnectStatus==GAME_DISCONNECTING)
		return XDPDisconnect();

	return hr;
}

//-----------------------------------------------------------------------------
// Name: XDPMsgHandler
// Desc: Handles some DPlay messages
//-----------------------------------------------------------------------------
HRESULT WINAPI XDPMsgHandler(PVOID pvUserContext, DWORD dwMsgType, PVOID pMsg)
{
	PDPNMSG_HOST_MIGRATE pHostMigrateMsg;

	switch(dwMsgType)
	{
        case DPN_MSGID_CONNECT_COMPLETE:
			g_dwConnectStatus = GAME_CONNECTED;
			break;

        case DPN_MSGID_TERMINATE_SESSION:
			g_dwConnectStatus = GAME_DISCONNECTING;
			break;

        case DPN_MSGID_HOST_MIGRATE:
            pHostMigrateMsg = (DPNMSG_HOST_MIGRATE *)pMsg;
			g_dpnidHost = pHostMigrateMsg->dpnidNewHost;
            if(g_dpnidHost==g_dpnidLocalPlayer)
				g_dwConnectStatus = GAME_HOSTING;
			break;
	}

	// call app DP message handler
	return g_AppDPMsgHandler(pvUserContext, dwMsgType, pMsg);
}

//-----------------------------------------------------------------------------
// Name: XDPEnumHosts
// Desc: Searches for all active games on the network
//-----------------------------------------------------------------------------
HRESULT XDPEnumHosts(DWORD port, GUID *pGuid)
{
    HRESULT hr;
	DWORD dwPort = port;

	// Delete active games
	XDPDeleteGames();

    // Enumerate hosts
    DPN_APPLICATION_DESC dnAppDesc;
    ZeroMemory(&dnAppDesc, sizeof(DPN_APPLICATION_DESC));
    dnAppDesc.dwSize = sizeof(DPN_APPLICATION_DESC);
    dnAppDesc.guidApplication = *pGuid;

	// set port
	hr = g_pHostAddress->AddComponent(DPNA_KEY_PORT, (void *)&dwPort, sizeof(dwPort), DPNA_DATATYPE_DWORD);
	assert(SUCCEEDED(hr));

    // Enumerate all the active DirectPlay games on the selected connection
    hr = g_pDP->EnumHosts(&dnAppDesc,                  // application description
                          g_pHostAddress,              // host address
                          g_pDeviceAddress,            // device address
                          NULL,                        // pointer to user data
                          0,                           // user data size
                          INFINITE,                    // retry count (forever)
                          0,                           // retry interval (0=default)
                          INFINITE,                    // time out (forever)
                          NULL,                        // user context
                          &g_hAsyncOp,		           // async handle
                          0							   // flags
                          );

	assert(SUCCEEDED(hr));

	g_dwConnectStatus = GAME_ENUMERATING;

    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: XDPCancelEnumHosts
// Desc: Stops searching for active games on the network.
//-----------------------------------------------------------------------------
void XDPCancelEnumHosts()
{
	g_pDP->CancelAsyncOperation(g_hAsyncOp, 0);				// stop the game search operation
	g_dwConnectStatus = GAME_NOTCONNECTED;
}

//-----------------------------------------------------------------------------
// Name: XDPAddGame
// Desc: Add an active game to the game list
//-----------------------------------------------------------------------------
ActiveGame *XDPAddGame(DPNMSG_ENUM_HOSTS_RESPONSE *pHost)
{
	DWORD i;
	const DPN_APPLICATION_DESC *pApp;
	HRESULT hr;
	ActiveGame *pGame;

	pApp = pHost->pApplicationDescription;

	// See if game already in list
	pGame = NULL;
	for(i=0; i<g_dwNumActiveGames; i++)
		if(ActiveGames[i])
			if(ActiveGames[i]->AppDesc.guidInstance==pApp->guidInstance)
			{
				pGame = ActiveGames[i];
				break;
			}

	// if game not already in list, add a new one
	if(!pGame)
	{
		ActiveGames[g_dwNumActiveGames] = new ActiveGame;
		pGame = ActiveGames[g_dwNumActiveGames];

		// set address information
		hr = pHost->pAddressSender->Duplicate(&pGame->pHostAddr);
		assert(SUCCEEDED(hr));
		hr = pHost->pAddressDevice->Duplicate(&pGame->pDevAddr);
		assert(SUCCEEDED(hr));

		// bump active game count
		g_dwNumActiveGames++;
	}

	// update game information
	memcpy(&pGame->AppDesc, pApp, sizeof(DPN_APPLICATION_DESC));
	pGame->dwPing = pHost->dwRoundTripLatencyMS;

	// set report time
	pGame->dwReportTime = GetTickCount();

	return pGame;
}

//-----------------------------------------------------------------------------
// Name: XDPDeleteGames
// Desc: Deletes active games list.
//-----------------------------------------------------------------------------
void XDPDeleteGames()
{
	DWORD i;

	for(i=0; i<g_dwNumActiveGames; i++)
		if(ActiveGames[i])
		{
			ActiveGames[i]->pHostAddr->Release();		// release address objects
			ActiveGames[i]->pDevAddr->Release();

			delete ActiveGames[i];						// free up the memory
			ActiveGames[i] = NULL;
		}

	g_dwNumActiveGames = 0;
}

//-----------------------------------------------------------------------------
// Name: XDPCreate
// Desc: Creates a multiplayer game with us as a host.
//-----------------------------------------------------------------------------
HRESULT XDPCreate(DWORD port, GUID *pGuid, DWORD dwMaxPlayers, DWORD flags)
{
	HRESULT hr;
    DPN_PLAYER_INFO dpPlayerInfo;
    DPN_APPLICATION_DESC dnAppDesc;
	DWORD dwPort = port;
    DPNHANDLE hAsync;

    OutputDebugStringA("xdppeer: hosting\n");

	// set up player info for ourself as we create the game
    ZeroMemory(&dpPlayerInfo, sizeof(DPN_PLAYER_INFO));
    dpPlayerInfo.dwSize = sizeof(DPN_PLAYER_INFO);
    dpPlayerInfo.dwInfoFlags = DPNINFO_NAME;
    dpPlayerInfo.pwszName = g_wszXDPPlayerName;

	// set peer info. this is sent to all other machines via
	// the CREATE_PLAYER message
    hr = g_pDP->SetPeerInfo(&dpPlayerInfo, NULL, &hAsync, 0);
	assert(SUCCEEDED(hr));

    // Setup the application description
    ZeroMemory(&dnAppDesc, sizeof(DPN_APPLICATION_DESC));
    dnAppDesc.dwSize = sizeof(DPN_APPLICATION_DESC);
    dnAppDesc.guidApplication = *pGuid;
    dnAppDesc.dwMaxPlayers = dwMaxPlayers;
    dnAppDesc.dwFlags = flags;

	// set the name of the game
    dnAppDesc.pwszSessionName = g_wszXDPSessionName;

    // Host a game on m_pDeviceAddress as described by dnAppDesc
	g_pDeviceAddress->AddComponent(DPNA_KEY_PORT, (void *)&dwPort, sizeof(dwPort), DPNA_DATATYPE_DWORD);
	assert(SUCCEEDED(hr));

    hr = g_pDP->Host(&dnAppDesc, &g_pDeviceAddress, 1, NULL, NULL, NULL, 0);
	assert(SUCCEEDED(hr));

	g_dwConnectStatus = GAME_HOSTING;

	return hr;
}

//-----------------------------------------------------------------------------
// Name: XDPConnect
// Desc: Connect to an existing game.
//-----------------------------------------------------------------------------
HRESULT XDPConnect(DPN_APPLICATION_DESC *pAppDesc, IDirectPlay8Address *pHostAddr, IDirectPlay8Address *pDevAddr)
{
	static float time = 0.0f;
	static TCHAR s[80];
	HRESULT hr;
    DPNHANDLE hAsync;

    OutputDebugStringA("xdppeer: connecting\n");

	// cancel host enumeration
	XDPCancelEnumHosts();

	// Set player info
	DPN_PLAYER_INFO dpPlayerInfo;
	ZeroMemory(&dpPlayerInfo, sizeof(DPN_PLAYER_INFO));
	dpPlayerInfo.dwSize = sizeof(DPN_PLAYER_INFO);
	dpPlayerInfo.dwInfoFlags = DPNINFO_NAME;
	dpPlayerInfo.pwszName = g_wszXDPPlayerName;

	// set the peer info for the client
	hr = g_pDP->SetPeerInfo(&dpPlayerInfo, NULL, &hAsync, 0);
	assert(SUCCEEDED(hr));

	// connect to the game
	hr = g_pDP->Connect( pAppDesc,								// the application description
						 pHostAddr,								// address of the host of the session
						 pDevAddr,								// address of the local device the enum responses were received on
						 NULL, NULL,							// DPN_SECURITY_DESC, DPN_SECURITY_CREDENTIALS
						 NULL, 0,								// user data, user data size
						 NULL,									// player context,
						 NULL, &g_hAsyncOp,						// async context, async handle,
						 0);									// flags

	assert(hr==E_PENDING || SUCCEEDED(hr));

	g_dwConnectStatus = GAME_CONNECTING;

	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: XDPDisconnect
// Desc: Disconnect from the current game
//-----------------------------------------------------------------------------
HRESULT XDPDisconnect()
{
	HRESULT hr;

    OutputDebugStringA("xdppeer: disconnecting\n");

	g_pDP->Close(0);										// close the dplay connection
    hr = g_pDP->Initialize(NULL, XDPMsgHandler, 0);			// reinitialize dplay
	assert(SUCCEEDED(hr));

	g_dwConnectStatus = GAME_NOTCONNECTED;

	return hr;
}

//-----------------------------------------------------------------------------
// Name: XDPTerminate
// Desc: Terminate the current game as the host
//-----------------------------------------------------------------------------
HRESULT XDPTerminate()
{
	// send the terminate session message
	if(g_dwConnectStatus==GAME_HOSTING)
	{
	    OutputDebugStringA("xdppeer: terminating session\n");
		g_pDP->TerminateSession(NULL, 0, 0);
	}

	// and reset DPlay
	return XDPDisconnect();
}

//-----------------------------------------------------------------------------
// Name: XDPExpireGames
// Desc: Thread that expires old games in the ActiveGames list.
//-----------------------------------------------------------------------------
void XDPExpireGames()
{
	DWORD i;

	// search for expired games
	for(i=0; i<g_dwNumActiveGames; i++)
	{
		if((ActiveGames[i]->dwReportTime+3000)<GetTickCount())
		{
			// send delete game message to message handler
			g_AppDPMsgHandler(NULL, DPPEER_MSGID_DELETE_GAME, ActiveGames[i]);

			// delete the expired active game
			ActiveGames[i]->pHostAddr->Release();
			ActiveGames[i]->pDevAddr->Release();
			delete ActiveGames[i];

			// and remove it from our list
			ActiveGames[i] = ActiveGames[g_dwNumActiveGames-1];
			ActiveGames[g_dwNumActiveGames-1] = NULL;
			g_dwNumActiveGames--;
		}
	}
}