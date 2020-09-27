//-----------------------------------------------------------------------------
// File: dpmemtest.cpp
//
// Desc: Basic executable to measure dynamic DirectPlay memory usage
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include <xtl.h>
#include <dplay8.h>
#include <dpaddr.h>

GUID g_guidApp = { 0x2ae835d, 0x9179, 0x485f, { 0x83, 0x43, 0x90, 0x1d, 0x32, 0x7c, 0xe7, 0x94 } };
BOOL g_fThreadRun = FALSE;

#define LOCALADDRESS L"157.56.11.67"
#define HOSTPORT 2350

#define PEER_TYPE_HOST 1
#define PEER_TYPE_JOIN 2

typedef struct _APP_DP_PEER
{
	LPDIRECTPLAY8PEER pDP;
	DWORD dwPeerType;
	DPNID dpnidLocalPlayer;
	DPNID dpnidSecondPlayer;
	LONG lNumPlayers;
	HANDLE hEvent1;
	HANDLE hEvent2;
	HANDLE hEvent3;
	HANDLE hEvent4;
} APP_DP_PEER, *LPAPP_DP_PEER;

LPAPP_DP_PEER g_pDPPeer1;
LPAPP_DP_PEER g_pDPPeer2;

#define GAME_MSGID_WAVE        1

struct GAMEMSG_GENERIC
{
    DWORD dwType;
    CHAR blah[32];
};

BOOL GlobalInit(BOOL fThreadEnabled);
BOOL InitDPlayPeer(LPAPP_DP_PEER pDirectPlay8Peer);
BOOL CloseDPlayPeer(LPAPP_DP_PEER pDirectPlay8Peer);
BOOL ReleaseDPlayPeer(LPAPP_DP_PEER pDirectPlay8Peer);
BOOL HostGame(LPAPP_DP_PEER pDirectPlay8Peer);
BOOL JoinGame(LPAPP_DP_PEER pDirectPlay8Peer);
BOOL SendMessage(LPAPP_DP_PEER pDirectPlay8Peer);
DWORD WINAPI WorkerThread( void *pParam );
VOID WorkForAwhile( HANDLE hEvent, DWORD dwIterations );

HRESULT DirectPlayMessageHandler( PVOID pvUserContext, DWORD dwMessageId, PVOID pMsgBuffer );

void __cdecl main()
{
    LPDIRECTPLAY8ADDRESS pPeerAddress;

    __asm int 3;

    if(GlobalInit(FALSE) == FALSE)
		OutputDebugString("GlobalInit failure\n");
	else
		OutputDebugString("GlobalInit success\n");

/*
    if (InitDPlayPeer(g_pDPPeer1) == FALSE)
		OutputDebugString("InitDPlayPeer failure\n");
	else
		OutputDebugString("InitDPlayPeer success\n");

    if (FAILED(g_pDPPeer1->pDP->GetPeerAddress(1, &pPeerAddress, 0)))
		OutputDebugString("GetPeerAddress failure\n");
	else
		OutputDebugString("GetPeerAddress success\n");

    if (HostGame(g_pDPPeer1) == FALSE)
		OutputDebugString("HostGame failure\n");
	else
		OutputDebugString("HostGame success\n");

    if (FAILED(g_pDPPeer1->pDP->GetPeerAddress(222, &pPeerAddress, 0)))
		OutputDebugString("GetPeerAddress failure\n");
	else
		OutputDebugString("GetPeerAddress success\n");

	OutputDebugString("WorkForAwhile\n");
    WorkForAwhile(g_pDPPeer1->hEvent1, 0);
	OutputDebugString("WorkForAwhile\n");
    WorkForAwhile(NULL, 100000);

    if (FAILED(g_pDPPeer1->pDP->GetPeerAddress(g_pDPPeer1->dpnidLocalPlayer, &pPeerAddress, 0)))
		OutputDebugString("GetPeerAddress failure\n");
	else
		OutputDebugString("GetPeerAddress success\n");

    if (CloseDPlayPeer(g_pDPPeer1) == FALSE)
		OutputDebugString("CloseDPlayPeer failure\n");
	else
		OutputDebugString("CloseDPlayPeer success\n");
*/

    if (InitDPlayPeer(g_pDPPeer1) == FALSE)
		OutputDebugString("InitDPlayPeer failure\n");
	else
		OutputDebugString("InitDPlayPeer success\n");

    if (InitDPlayPeer(g_pDPPeer2) == FALSE)
		OutputDebugString("InitDPlayPeer failure\n");
	else
		OutputDebugString("InitDPlayPeer success\n");

    if (HostGame(g_pDPPeer2) == FALSE)
		OutputDebugString("HostGame failure\n");
	else
		OutputDebugString("HostGame success\n");

	if (JoinGame(g_pDPPeer1) == FALSE)
		OutputDebugString("JoinGame failure\n");
	else
		OutputDebugString("JoinGame success\n");

 	OutputDebugString("WorkForAwhile\n");
    WorkForAwhile(g_pDPPeer1->hEvent2, 0);
 	OutputDebugString("WorkForAwhile\n");
    WorkForAwhile(g_pDPPeer2->hEvent3, 0);

// NOTE HERE:  Should I use g_pDPPeer1->dpnidSecondPlayer or g_pDPPeer2->dpnidLocalPlayer
/*
    if (FAILED(g_pDPPeer1->pDP->GetPeerAddress(g_pDPPeer1->dpnidSecondPlayer, &pPeerAddress, 0)))
		OutputDebugString("GetPeerAddress failure\n");
	else
		OutputDebugString("GetPeerAddress success\n");
*/

    if (FAILED(g_pDPPeer2->pDP->GetPeerAddress(g_pDPPeer2->dpnidSecondPlayer, &pPeerAddress, 0)))
		OutputDebugString("GetPeerAddress failure\n");
	else
		OutputDebugString("GetPeerAddress success\n");

    if (CloseDPlayPeer(g_pDPPeer1) == FALSE)
		OutputDebugString("CloseDPlayPeer failure\n");
	else
		OutputDebugString("CloseDPlayPeer success\n");

    if (CloseDPlayPeer(g_pDPPeer2) == FALSE)
		OutputDebugString("CloseDPlayPeer failure\n");
	else
		OutputDebugString("CloseDPlayPeer success\n");

    if (ReleaseDPlayPeer(g_pDPPeer1) == FALSE)
		OutputDebugString("ReleaseDPlayPeer failure\n");
	else
		OutputDebugString("ReleaseDPlayPeer success\n");

    if (ReleaseDPlayPeer(g_pDPPeer2) == FALSE)
		OutputDebugString("ReleaseDPlayPeer failure\n");
	else
		OutputDebugString("ReleaseDPlayPeer success\n");

    HRESULT hr = DPlayCleanup();

    if (FAILED(hr))
        __asm int 3;

    __asm int 3;
}

BOOL GlobalInit(BOOL fThreadEnabled)
{
    DWORD dwThreadID = 0;
    HANDLE hThread = NULL;

    if (XnetInitialize(NULL, TRUE) != NO_ERROR)
    {
        __asm int 3;
        return FALSE;
    }

    if (FAILED(DPlayInitialize(1024 * 200)))
    {
        __asm int 3;
        return FALSE;
    }

    if (fThreadEnabled)
        hThread = CreateThread( NULL, 0, WorkerThread, 0, 0, &dwThreadID );

    g_pDPPeer1 = new APP_DP_PEER;
    g_pDPPeer2 = new APP_DP_PEER;

    g_pDPPeer1->pDP = NULL;
    g_pDPPeer2->pDP = NULL;

    return TRUE;
}

BOOL InitDPlayPeer(LPAPP_DP_PEER pDirectPlay8Peer)
{
    HRESULT hr;

    pDirectPlay8Peer->dwPeerType = 0;
    pDirectPlay8Peer->dpnidLocalPlayer = -1;
    pDirectPlay8Peer->dpnidSecondPlayer = -1;
    pDirectPlay8Peer->lNumPlayers = 0;
    pDirectPlay8Peer->hEvent1 = NULL;
    pDirectPlay8Peer->hEvent2 = NULL;
    pDirectPlay8Peer->hEvent3 = NULL;
    pDirectPlay8Peer->hEvent4 = NULL;

    pDirectPlay8Peer->hEvent1 = CreateEvent(NULL, TRUE, FALSE, NULL);
    pDirectPlay8Peer->hEvent2 = CreateEvent(NULL, TRUE, FALSE, NULL);
    pDirectPlay8Peer->hEvent3 = CreateEvent(NULL, TRUE, FALSE, NULL);
    pDirectPlay8Peer->hEvent4 = CreateEvent(NULL, TRUE, FALSE, NULL);

    ResetEvent(pDirectPlay8Peer->hEvent1);
    ResetEvent(pDirectPlay8Peer->hEvent2);
    ResetEvent(pDirectPlay8Peer->hEvent3);
    ResetEvent(pDirectPlay8Peer->hEvent4);

    if (pDirectPlay8Peer->pDP == NULL)
    {
        hr = DirectPlay8Create(IID_IDirectPlay8Peer, (void **) &(pDirectPlay8Peer->pDP), NULL);
        if( FAILED(hr) )
        {
            __asm int 3;
            return FALSE;
        }
    }

    hr = pDirectPlay8Peer->pDP->Initialize( (void*)pDirectPlay8Peer, DirectPlayMessageHandler, 0 );
    if( FAILED(hr) )
    {
        __asm int 3;
        return FALSE;
    }

    return TRUE;
}

BOOL CloseDPlayPeer(LPAPP_DP_PEER pDirectPlay8Peer)
{
    HRESULT hr;

    hr = pDirectPlay8Peer->pDP->Close(0);
    if( FAILED(hr) )
    {
        __asm int 3;
        return FALSE;
    }

    CloseHandle(pDirectPlay8Peer->hEvent1);
    CloseHandle(pDirectPlay8Peer->hEvent2);
    CloseHandle(pDirectPlay8Peer->hEvent3);
    CloseHandle(pDirectPlay8Peer->hEvent4);

    return TRUE;
}


BOOL ReleaseDPlayPeer(LPAPP_DP_PEER pDirectPlay8Peer)
{
    LPDIRECTPLAY8PEER pTempPeer = NULL;
    HRESULT hr = S_OK;

    pTempPeer = pDirectPlay8Peer->pDP;
    pDirectPlay8Peer->pDP = NULL;

    SleepEx(10, FALSE);

    hr = pTempPeer->Release();
    if( FAILED(hr) )
    {
        __asm int 3;
        return FALSE;
    }

    return TRUE;
}


BOOL HostGame(LPAPP_DP_PEER pDirectPlay8Peer)
{
    HRESULT hr;
    DPN_APPLICATION_DESC dnAppDesc;
    LPDIRECTPLAY8ADDRESS pDeviceAddress;

    pDirectPlay8Peer->dwPeerType = PEER_TYPE_HOST;

    // Create a device address
    hr = DirectPlay8AddressCreate(IID_IDirectPlay8Address, (void **) &pDeviceAddress, NULL);
    if( FAILED(hr) )
    {
        __asm int 3;
        return FALSE;
    }

    pDeviceAddress->BuildAddress(LOCALADDRESS, HOSTPORT);

    ZeroMemory( &dnAppDesc, sizeof(DPN_APPLICATION_DESC) );
    dnAppDesc.dwSize          = sizeof(DPN_APPLICATION_DESC);
    dnAppDesc.guidApplication = g_guidApp;
    dnAppDesc.pwszSessionName = L"MyGamexbox";
    dnAppDesc.dwMaxPlayers    = 5;
    dnAppDesc.dwFlags         = 0;

    hr = pDirectPlay8Peer->pDP->Host( &dnAppDesc, &pDeviceAddress, 1, NULL, NULL, NULL, 0 );
    if( FAILED( hr ) )
    {
        __asm int 3;
        return FALSE;
    }

    pDeviceAddress->Release();

    return TRUE;
}

BOOL JoinGame(LPAPP_DP_PEER pDirectPlay8Peer)
{
    HRESULT hr;
    DPN_APPLICATION_DESC dnAppDesc;
//    DPN_PLAYER_INFO dpPlayerInfo;
    DPNHANDLE hConnectAsyncOp;
    LPDIRECTPLAY8ADDRESS pDeviceAddress;
    LPDIRECTPLAY8ADDRESS pHostAddress;

    pDirectPlay8Peer->dwPeerType = PEER_TYPE_JOIN;

    // Create a device address
    hr = DirectPlay8AddressCreate(IID_IDirectPlay8Address, (void **) &pDeviceAddress, NULL);
    if( FAILED(hr) )
    {
        __asm int 3;
        return FALSE;
    }

    // Create a host address
    hr = DirectPlay8AddressCreate(IID_IDirectPlay8Address, (void **) &pHostAddress, NULL);
    if( FAILED(hr) )
    {
        __asm int 3;
        return FALSE;
    }

    pHostAddress->BuildAddress(LOCALADDRESS, HOSTPORT);

    ZeroMemory( &dnAppDesc, sizeof(DPN_APPLICATION_DESC) );
    dnAppDesc.dwSize          = sizeof(DPN_APPLICATION_DESC);
    dnAppDesc.guidApplication = g_guidApp;

/*
    ZeroMemory( &dpPlayerInfo, sizeof(DPN_PLAYER_INFO) );
    dpPlayerInfo.dwSize = sizeof(DPN_PLAYER_INFO);
    dpPlayerInfo.dwInfoFlags = DPNINFO_NAME;
    dpPlayerInfo.pwszName = L"MarkXbox";

    hr = pDirectPlay8Peer->pDP->SetPeerInfo( &dpPlayerInfo, NULL, &hConnectAsyncOp, 0 );
    if( FAILED( hr ) )
    {
        __asm int 3;
        return FALSE;
    }
*/

    hr = pDirectPlay8Peer->pDP->Connect( &dnAppDesc, pHostAddress, pDeviceAddress, NULL, NULL, NULL, 0, NULL, NULL, &hConnectAsyncOp, 0 );
    if( hr != E_PENDING && FAILED(hr) )
    {
        __asm int 3;
        return FALSE;
    }

    pHostAddress->Release();
    pDeviceAddress->Release();

    return TRUE;
}

BOOL SendMessage(LPAPP_DP_PEER pDirectPlay8Peer)
{
    GAMEMSG_GENERIC msgWave;
    DPN_BUFFER_DESC bufferDesc;
    DPNHANDLE hAsync;

    msgWave.dwType = GAME_MSGID_WAVE;
    bufferDesc.dwBufferSize = sizeof(GAMEMSG_GENERIC);
    bufferDesc.pBufferData  = (BYTE*) &msgWave;

    pDirectPlay8Peer->pDP->SendTo( DPNID_ALL_PLAYERS_GROUP, &bufferDesc, 1, 0, NULL, &hAsync, DPNSEND_NOLOOPBACK );

    return TRUE;
}

DWORD WINAPI WorkerThread( void *pParam )
{
    while (1)
    {
        if (g_fThreadRun)
        {
            g_pDPPeer1->pDP->DoWork(0);
            g_pDPPeer2->pDP->DoWork(0);
            SleepEx(10, FALSE);
        }
    }

    return 0;
}


VOID WorkForAwhile( HANDLE hEvent, DWORD dwIterations )
{

	if( WaitForSingleObjectEx( hEvent, 0, TRUE ) == WAIT_OBJECT_0 )
		return;

	if (hEvent != NULL)
	{
		while (1)
		{
			
			if (g_pDPPeer1->pDP != NULL)
			{
				g_pDPPeer1->pDP->DoWork(0);

				switch ( WaitForSingleObjectEx( hEvent, 0, TRUE ) )
				{
				case WAIT_OBJECT_0:
					{
						goto Exit;
						break;
					}
					
				case WAIT_TIMEOUT:
					{
						break;
					}
				}
			}
			
			if (g_pDPPeer2->pDP != NULL)
			{
				g_pDPPeer2->pDP->DoWork(0);

				switch ( WaitForSingleObjectEx( hEvent, 0, TRUE ) )
				{
				case WAIT_OBJECT_0:
					{
						goto Exit;
						break;
					}
					
				case WAIT_TIMEOUT:
					{
						break;
					}
				}
			}
		}
	}

	if (dwIterations == 0)
	{
        HRESULT hr = S_OK;

		while (hr == S_OK)
        {
            if (g_pDPPeer1->pDP != NULL)
                hr = g_pDPPeer1->pDP->DoWork(0);

            if (g_pDPPeer2->pDP != NULL)
                 if (g_pDPPeer2->pDP->DoWork(0) == S_OK)
                     hr = S_OK;
        }
		goto Exit;
	}

	
	for (DWORD i=0; i<dwIterations; i+=1)
	{
        if (g_pDPPeer1->pDP != NULL)
    		g_pDPPeer1->pDP->DoWork(0);

        if (g_pDPPeer2->pDP != NULL)
	    	g_pDPPeer2->pDP->DoWork(0);
	}

Exit:

    return;
}

HRESULT DirectPlayMessageHandler( PVOID pvUserContext, DWORD dwMessageId, PVOID pMsgBuffer )
{
    LPAPP_DP_PEER pDirectPlay8Peer = (LPAPP_DP_PEER) pvUserContext;
    HRESULT hr = S_OK;

	OutputDebugString("DirectPlayMessageHandler\n");

    switch( dwMessageId )
    {
        case DPN_MSGID_CREATE_PLAYER:
        {
            PDPNMSG_CREATE_PLAYER pCreatePlayerMsg = (PDPNMSG_CREATE_PLAYER)pMsgBuffer;

            if( pDirectPlay8Peer->lNumPlayers == 0)
            {
                pDirectPlay8Peer->dpnidLocalPlayer = pCreatePlayerMsg->dpnidPlayer;
                SetEvent(pDirectPlay8Peer->hEvent1);
            }
            else
            {
                pDirectPlay8Peer->dpnidSecondPlayer = pCreatePlayerMsg->dpnidPlayer;
                SetEvent(pDirectPlay8Peer->hEvent3);
            }

            pDirectPlay8Peer->lNumPlayers += 1;

            break;
        }

        case DPN_MSGID_DESTROY_PLAYER:
        {
            PDPNMSG_DESTROY_PLAYER pDestroyPlayerMsg = (PDPNMSG_DESTROY_PLAYER)pMsgBuffer;

            pDirectPlay8Peer->lNumPlayers -= 1;

            break;
        }

        case DPN_MSGID_TERMINATE_SESSION:
        {
            PDPNMSG_TERMINATE_SESSION pTerminateSessionMsg = (PDPNMSG_TERMINATE_SESSION)pMsgBuffer;

            break;
        }

        case DPN_MSGID_RECEIVE:
        {
            PDPNMSG_RECEIVE pReceiveMsg = (PDPNMSG_RECEIVE)pMsgBuffer;

            break;
        }

        case DPN_MSGID_INDICATE_CONNECT:
        {
            SetEvent(pDirectPlay8Peer->hEvent2);

            break;
        }

        case DPN_MSGID_CONNECT_COMPLETE:
        {
            PDPNMSG_CONNECT_COMPLETE pConnectCompleteMsg = (PDPNMSG_CONNECT_COMPLETE)pMsgBuffer;

            hr = pConnectCompleteMsg->hResultCode;

            SetEvent(pDirectPlay8Peer->hEvent2);

            break;
        }

        case DPN_MSGID_ASYNC_OP_COMPLETE:
        {
            PDPNMSG_ASYNC_OP_COMPLETE pAsyncOpCompleteMsg = (PDPNMSG_ASYNC_OP_COMPLETE)pMsgBuffer;

            break;
        }

    }

    return S_OK;
}