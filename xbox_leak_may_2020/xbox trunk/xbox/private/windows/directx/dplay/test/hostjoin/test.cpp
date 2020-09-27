/***************************************************************************
 *
 *  Copyright (C) 2000 Microsoft Corporation.  All Rights Reserved.
 *
 ****************************************************************************/

#include <xtl.h>
#include <dplay8.h>
#include <dpaddr.h>

HANDLE				g_hSyncEvent = NULL;
HANDLE				g_hConnectEvent = NULL;
LPDIRECTPLAY8PEER		g_pDPHost;
LPDIRECTPLAY8PEER		g_pDPJoin;
GUID				g_guidApp = { 0x2ae835d, 0x9179, 0x485f, { 0x83, 0x43, 0x90, 0x1d, 0x32, 0x7c, 0xe7, 0x94 } };

DPNID				g_dpnidLocalPlayer            = 0;
LONG				g_lNumberOfActivePlayers      = 0;
TCHAR				g_strLocalPlayerName[MAX_PATH];

BOOL				g_fThreadRun = FALSE;

#define LOCALADDRESS L"157.56.10.87"
#define HOSTPORT 2350


#define MAX_PLAYER_NAME         14

struct APP_PLAYER_INFO
{
    LONG  lRefCount;                        // Ref count so we can cleanup when all threads 
                                            // are done w/ this object
    DPNID dpnidPlayer;                      // DPNID of player
    TCHAR strPlayerName[MAX_PLAYER_NAME];   // Player name
};

#define GAME_MSGID_WAVE        1

struct GAMEMSG_GENERIC
{
    DWORD dwType;
    CHAR blah[2048];
};

typedef HRESULT (WINAPI *LPFNDIRECTPLAYMESSAGEHANDLER)(PVOID, DWORD, PVOID);

BOOL GlobalInit(void);
BOOL InitDPlayPeer(LPDIRECTPLAY8PEER *pDirectPlay8Peer, LPFNDIRECTPLAYMESSAGEHANDLER pMessageHandler);
BOOL HostGame(LPDIRECTPLAY8PEER pDirectPlay8Peer);
BOOL JoinGame(LPDIRECTPLAY8PEER pDirectPlay8Peer);
BOOL SendMessage(LPDIRECTPLAY8PEER pDirectPlay8Peer);
BOOL CloseDPlayPeer(LPDIRECTPLAY8PEER *ppDirectPlay8Peer);
DWORD WINAPI WorkerThread( void *pParam );

HRESULT WINAPI DirectPlayHostMessageHandler( PVOID pvUserContext, DWORD dwMessageId, PVOID pMsgBuffer );
HRESULT WINAPI DirectPlayJoinMessageHandler( PVOID pvUserContext, DWORD dwMessageId, PVOID pMsgBuffer );

void __cdecl main()
{
    __asm int 3;

    if (GlobalInit() == FALSE)
    {
        __asm int 3;
        goto exit;
    }

    if (InitDPlayPeer(&g_pDPHost, DirectPlayHostMessageHandler) == FALSE)
    {
        __asm int 3;
        goto exit;
    }

    if (InitDPlayPeer(&g_pDPJoin, DirectPlayJoinMessageHandler) == FALSE)
    {
        __asm int 3;
        goto exit;
    }

    g_fThreadRun = TRUE;

    if (HostGame(g_pDPHost) == FALSE)
    {
        __asm int 3;
        goto exit;
    }

    if (JoinGame(g_pDPJoin) == FALSE)
    {
        __asm int 3;
        goto exit;
    }

    ResetEvent(g_hConnectEvent);

    if (SendMessage(g_pDPJoin) == FALSE)
    {
        __asm int 3;
        goto exit;
    }

    WaitForSingleObject(g_hConnectEvent, INFINITE);

    CloseDPlayPeer(&g_pDPHost);
    CloseDPlayPeer(&g_pDPJoin);

    g_fThreadRun = FALSE;

  exit:

    HRESULT hr = DPlayCleanup();

    if (FAILED(hr))
    {
        __asm int 3;
    }

    __asm int 3;
}

BOOL GlobalInit(void)
{
    INT err = 0;
    HRESULT hr = S_OK;
    DWORD dwThreadID = 0;
    HANDLE hThread = NULL;

    g_hSyncEvent = CreateEvent(NULL, TRUE, FALSE,NULL);
    g_hConnectEvent = CreateEvent(NULL, TRUE, FALSE,NULL);

    ResetEvent(g_hSyncEvent);    
    ResetEvent(g_hConnectEvent);    

    err = XnetInitialize(NULL, TRUE);
    if (err != NO_ERROR)
    {
        __asm int 3;
        return FALSE;
    }

    hr = DPlayInitialize();

    if (FAILED(hr))
    {
        __asm int 3;
    }

    hThread = CreateThread( NULL, 0, WorkerThread, 0, 0, &dwThreadID );

    return TRUE;
}

BOOL InitDPlayPeer(LPDIRECTPLAY8PEER *ppDirectPlay8Peer, LPFNDIRECTPLAYMESSAGEHANDLER pMessageHandler)
{
    HRESULT hr;

    hr = DirectPlay8Create(IID_IDirectPlay8Peer, (void **) ppDirectPlay8Peer, NULL);
    if( FAILED(hr) )
    {
        __asm int 3;
        return FALSE;
    }

    hr = (*ppDirectPlay8Peer)->Initialize( NULL, pMessageHandler, 0 );
    if( FAILED(hr) )
    {
        __asm int 3;
        return FALSE;
    }

    return TRUE;
}

BOOL HostGame(LPDIRECTPLAY8PEER pDirectPlay8Peer)
{
    HRESULT hr;
    DPN_APPLICATION_DESC dnAppDesc;
    LPDIRECTPLAY8ADDRESS pDeviceAddress;

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

    hr = pDirectPlay8Peer->Host( &dnAppDesc, &pDeviceAddress, 1, NULL, NULL, NULL, 0 );
    if( FAILED( hr ) )
    {
        __asm int 3;
        return FALSE;
    }

    pDeviceAddress->Release();

    return TRUE;
}

BOOL JoinGame(LPDIRECTPLAY8PEER pDirectPlay8Peer)
{
    HRESULT hr;
    DPN_APPLICATION_DESC dnAppDesc;
    DPN_PLAYER_INFO dpPlayerInfo;
    DPNHANDLE hConnectAsyncOp;
    LPDIRECTPLAY8ADDRESS pDeviceAddress;
    LPDIRECTPLAY8ADDRESS pHostAddress;

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

    ZeroMemory( &dpPlayerInfo, sizeof(DPN_PLAYER_INFO) );
    dpPlayerInfo.dwSize = sizeof(DPN_PLAYER_INFO);
    dpPlayerInfo.dwInfoFlags = DPNINFO_NAME;
    dpPlayerInfo.pwszName = L"MarkXbox";

    hr = pDirectPlay8Peer->SetPeerInfo( &dpPlayerInfo, NULL, NULL, 0 );
    if( FAILED( hr ) )
    {
        __asm int 3;
        return FALSE;
    }

    hr = pDirectPlay8Peer->Connect( &dnAppDesc, pHostAddress, pDeviceAddress, NULL, NULL, NULL, 0, NULL, NULL, &hConnectAsyncOp, 0 );
    if( hr != E_PENDING && FAILED(hr) )
    {
        __asm int 3;
        return FALSE;
    }

    WaitForSingleObject(g_hConnectEvent, INFINITE);

    pHostAddress->Release();
    pDeviceAddress->Release();

    return TRUE;
}

BOOL SendMessage(LPDIRECTPLAY8PEER pDirectPlay8Peer)
{
    HRESULT hr;
    GAMEMSG_GENERIC msgWave;
    DPN_BUFFER_DESC bufferDesc;
    DPNHANDLE hAsync;

    msgWave.dwType = GAME_MSGID_WAVE;
    bufferDesc.dwBufferSize = sizeof(GAMEMSG_GENERIC);
    bufferDesc.pBufferData  = (BYTE*) &msgWave;

    while (1)
        pDirectPlay8Peer->SendTo( DPNID_ALL_PLAYERS_GROUP, &bufferDesc, 1, 0, NULL, &hAsync, DPNSEND_NOLOOPBACK );

    return TRUE;
}

BOOL CloseDPlayPeer(LPDIRECTPLAY8PEER *ppDirectPlay8Peer)
{
    HRESULT hr;
    LPDIRECTPLAY8PEER pTempPeer = NULL;

    hr = (*ppDirectPlay8Peer)->Close(0);
    if( FAILED(hr) )
    {
        __asm int 3;
        return FALSE;
    }

    pTempPeer = *ppDirectPlay8Peer;
    *ppDirectPlay8Peer = NULL;

    SleepEx(10, FALSE);

    hr = (pTempPeer)->Release();
    if( FAILED(hr) )
    {
        __asm int 3;
        return FALSE;
    }

    return TRUE;
}


DWORD WINAPI WorkerThread( void *pParam )
{
    while (1)
    {
        if (g_fThreadRun && g_pDPHost->DoWork(0) == S_FALSE && g_pDPJoin->DoWork(0) == S_FALSE)
        {
            SleepEx(10, FALSE);
        }
    }

    return 0;
}


HRESULT WINAPI DirectPlayHostMessageHandler( PVOID pvUserContext, DWORD dwMessageId, PVOID pMsgBuffer )
{
    switch( dwMessageId )
    {
        case DPN_MSGID_CREATE_PLAYER:
        {
            HRESULT hr;
            PDPNMSG_CREATE_PLAYER pCreatePlayerMsg;
            pCreatePlayerMsg = (PDPNMSG_CREATE_PLAYER)pMsgBuffer;

            // Get the peer info and extract its name
            DWORD dwSize = 0;
            DPN_PLAYER_INFO* pdpPlayerInfo = NULL;
            hr = g_pDPHost->GetPeerInfo( pCreatePlayerMsg->dpnidPlayer, pdpPlayerInfo, &dwSize, 0 );
            if( FAILED(hr) && hr != DPNERR_BUFFERTOOSMALL )
                return hr;

            pdpPlayerInfo = (DPN_PLAYER_INFO*) new BYTE[ dwSize ];
            ZeroMemory( pdpPlayerInfo, dwSize );
            pdpPlayerInfo->dwSize = sizeof(DPN_PLAYER_INFO);
            hr = g_pDPHost->GetPeerInfo( pCreatePlayerMsg->dpnidPlayer, pdpPlayerInfo, &dwSize, 0 );
            if( FAILED(hr) )
                return hr;

            // Create a new and fill in a APP_PLAYER_INFO
            APP_PLAYER_INFO* pPlayerInfo = new APP_PLAYER_INFO;
            ZeroMemory( pPlayerInfo, sizeof(APP_PLAYER_INFO) );
            pPlayerInfo->lRefCount   = 1;
            pPlayerInfo->dpnidPlayer = pCreatePlayerMsg->dpnidPlayer;

            if( pdpPlayerInfo->dwPlayerFlags & DPNPLAYER_LOCAL )
                g_dpnidLocalPlayer = pCreatePlayerMsg->dpnidPlayer;

            delete pdpPlayerInfo;

            // Tell DirectPlay to store this pPlayerInfo 
            // pointer in the pvPlayerContext.
            pCreatePlayerMsg->pvPlayerContext = pPlayerInfo;

            // Update the number of active players, and 
            // post a message to the dialog thread to update the 
            // UI.  This keeps the DirectPlay message handler 
            // from blocking
            InterlockedIncrement( &g_lNumberOfActivePlayers );

            break;
        }

        case DPN_MSGID_DESTROY_PLAYER:
        {
            PDPNMSG_DESTROY_PLAYER pDestroyPlayerMsg;
            pDestroyPlayerMsg = (PDPNMSG_DESTROY_PLAYER)pMsgBuffer;
            APP_PLAYER_INFO* pPlayerInfo = (APP_PLAYER_INFO*) pDestroyPlayerMsg->pvPlayerContext;

            if( pPlayerInfo )
            {
                pPlayerInfo->lRefCount--;
                if( pPlayerInfo->lRefCount <= 0 )
                    delete pPlayerInfo;
            }
            pPlayerInfo = NULL;

            // Update the number of active players, and 
            // post a message to the dialog thread to update the 
            // UI.  This keeps the DirectPlay message handler 
            // from blocking
            InterlockedDecrement( &g_lNumberOfActivePlayers );

            break;
        }

        case DPN_MSGID_HOST_MIGRATE:
        {
            PDPNMSG_HOST_MIGRATE pHostMigrateMsg;
            pHostMigrateMsg = (PDPNMSG_HOST_MIGRATE)pMsgBuffer;

            break;
        }

        case DPN_MSGID_TERMINATE_SESSION:
        {
            PDPNMSG_TERMINATE_SESSION pTerminateSessionMsg;
            pTerminateSessionMsg = (PDPNMSG_TERMINATE_SESSION)pMsgBuffer;

            break;
        }

        case DPN_MSGID_RECEIVE:
        {
            HRESULT hr;
            PDPNMSG_RECEIVE pReceiveMsg;
            pReceiveMsg = (PDPNMSG_RECEIVE)pMsgBuffer;
            APP_PLAYER_INFO* pPlayerInfo = (APP_PLAYER_INFO*) pReceiveMsg->pvPlayerContext;
            APP_PLAYER_INFO* pPlayerInfo2 = NULL;

            GAMEMSG_GENERIC* pMsg = (GAMEMSG_GENERIC*) pReceiveMsg->pReceiveData;
            if( pMsg->dwType == GAME_MSGID_WAVE )
            {
                // This message is sent when a player has waved to us, so 
                // post a message to the dialog thread to update the UI.  
                // This keeps the DirectPlay threads from blocking, and also
                // serializes the recieves since DirectPlayMessageHandler can
                // be called simultaneously from a pool of DirectPlay threads.
                hr = g_pDPHost->GetPlayerContext( pPlayerInfo->dpnidPlayer, (LPVOID* const) &pPlayerInfo2, 0);

                if( pPlayerInfo2 )
                    pPlayerInfo2->lRefCount++;

                if( FAILED(hr) || pPlayerInfo2 == NULL )
                {
                    // The player who sent this may have gone away before this 
                    // message was handled, so just ignore it
                    break;
                }
            
                // Make wave message and display it.
                TCHAR szWaveMessage[MAX_PATH];
                wsprintf( szWaveMessage, TEXT("%s just waved at you, %s!\r\n"), 
                          pPlayerInfo2->strPlayerName, g_strLocalPlayerName );

                if( pPlayerInfo2 )
                {
                    pPlayerInfo2->lRefCount--;
                    if( pPlayerInfo2->lRefCount <= 0 )
                         delete pPlayerInfo2;
                }
                pPlayerInfo2 = NULL;
            }

            SetEvent(g_hConnectEvent);
            break;
        }

        case DPN_MSGID_ASYNC_OP_COMPLETE:
        {
            PDPNMSG_ASYNC_OP_COMPLETE pAsyncOpCompleteMsg;
            pAsyncOpCompleteMsg = (PDPNMSG_ASYNC_OP_COMPLETE)pMsgBuffer;

//            if( pAsyncOpCompleteMsg->hAsyncOp == hEnumAsyncOp )
//            {
//            }
            break;
        }

    }

    return S_OK;
}

HRESULT WINAPI DirectPlayJoinMessageHandler( PVOID pvUserContext, DWORD dwMessageId, PVOID pMsgBuffer )
{
    switch( dwMessageId )
    {
        case DPN_MSGID_CREATE_PLAYER:
        {
            HRESULT hr;
            PDPNMSG_CREATE_PLAYER pCreatePlayerMsg;
            pCreatePlayerMsg = (PDPNMSG_CREATE_PLAYER)pMsgBuffer;

            // Get the peer info and extract its name
            DWORD dwSize = 0;
            DPN_PLAYER_INFO* pdpPlayerInfo = NULL;
            hr = g_pDPJoin->GetPeerInfo( pCreatePlayerMsg->dpnidPlayer, pdpPlayerInfo, &dwSize, 0 );
            if( FAILED(hr) && hr != DPNERR_BUFFERTOOSMALL )
                return hr;

            pdpPlayerInfo = (DPN_PLAYER_INFO*) new BYTE[ dwSize ];
            ZeroMemory( pdpPlayerInfo, dwSize );
            pdpPlayerInfo->dwSize = sizeof(DPN_PLAYER_INFO);
            hr = g_pDPJoin->GetPeerInfo( pCreatePlayerMsg->dpnidPlayer, pdpPlayerInfo, &dwSize, 0 );
            if( FAILED(hr) )
                return hr;

            // Create a new and fill in a APP_PLAYER_INFO
            APP_PLAYER_INFO* pPlayerInfo = new APP_PLAYER_INFO;
            ZeroMemory( pPlayerInfo, sizeof(APP_PLAYER_INFO) );
            pPlayerInfo->lRefCount   = 1;
            pPlayerInfo->dpnidPlayer = pCreatePlayerMsg->dpnidPlayer;

            if( pdpPlayerInfo->dwPlayerFlags & DPNPLAYER_LOCAL )
                g_dpnidLocalPlayer = pCreatePlayerMsg->dpnidPlayer;

            delete pdpPlayerInfo;

            // Tell DirectPlay to store this pPlayerInfo 
            // pointer in the pvPlayerContext.
            pCreatePlayerMsg->pvPlayerContext = pPlayerInfo;

            // Update the number of active players, and 
            // post a message to the dialog thread to update the 
            // UI.  This keeps the DirectPlay message handler 
            // from blocking
            InterlockedIncrement( &g_lNumberOfActivePlayers );

            break;
        }

        case DPN_MSGID_DESTROY_PLAYER:
        {
            PDPNMSG_DESTROY_PLAYER pDestroyPlayerMsg;
            pDestroyPlayerMsg = (PDPNMSG_DESTROY_PLAYER)pMsgBuffer;
            APP_PLAYER_INFO* pPlayerInfo = (APP_PLAYER_INFO*) pDestroyPlayerMsg->pvPlayerContext;

            if( pPlayerInfo )
            {
                pPlayerInfo->lRefCount--;
                if( pPlayerInfo->lRefCount <= 0 )
                    delete pPlayerInfo;
            }
            pPlayerInfo = NULL;

            // Update the number of active players, and 
            // post a message to the dialog thread to update the 
            // UI.  This keeps the DirectPlay message handler 
            // from blocking
            InterlockedDecrement( &g_lNumberOfActivePlayers );

            break;
        }

        case DPN_MSGID_HOST_MIGRATE:
        {
            PDPNMSG_HOST_MIGRATE pHostMigrateMsg;
            pHostMigrateMsg = (PDPNMSG_HOST_MIGRATE)pMsgBuffer;

            break;
        }

        case DPN_MSGID_TERMINATE_SESSION:
        {
            PDPNMSG_TERMINATE_SESSION pTerminateSessionMsg;
            pTerminateSessionMsg = (PDPNMSG_TERMINATE_SESSION)pMsgBuffer;

            break;
        }

        case DPN_MSGID_RECEIVE:
        {
            HRESULT hr;
            PDPNMSG_RECEIVE pReceiveMsg;
            pReceiveMsg = (PDPNMSG_RECEIVE)pMsgBuffer;
            APP_PLAYER_INFO* pPlayerInfo = (APP_PLAYER_INFO*) pReceiveMsg->pvPlayerContext;
            APP_PLAYER_INFO* pPlayerInfo2 = NULL;

            GAMEMSG_GENERIC* pMsg = (GAMEMSG_GENERIC*) pReceiveMsg->pReceiveData;
            if( pMsg->dwType == GAME_MSGID_WAVE )
            {
                // This message is sent when a player has waved to us, so 
                // post a message to the dialog thread to update the UI.  
                // This keeps the DirectPlay threads from blocking, and also
                // serializes the recieves since DirectPlayMessageHandler can
                // be called simultaneously from a pool of DirectPlay threads.
                hr = g_pDPJoin->GetPlayerContext( pPlayerInfo->dpnidPlayer, (LPVOID* const) &pPlayerInfo2, 0);

                if( pPlayerInfo2 )
                    pPlayerInfo2->lRefCount++;

                if( FAILED(hr) || pPlayerInfo2 == NULL )
                {
                    // The player who sent this may have gone away before this 
                    // message was handled, so just ignore it
                    break;
                }
            
                // Make wave message and display it.
                TCHAR szWaveMessage[MAX_PATH];
                wsprintf( szWaveMessage, TEXT("%s just waved at you, %s!\r\n"), 
                          pPlayerInfo2->strPlayerName, g_strLocalPlayerName );

                if( pPlayerInfo2 )
                {
                    pPlayerInfo2->lRefCount--;
                    if( pPlayerInfo2->lRefCount <= 0 )
                         delete pPlayerInfo2;
                }
                pPlayerInfo2 = NULL;
            }
            break;
        }

        case DPN_MSGID_CONNECT_COMPLETE:
        {
            HRESULT hrConnectComplete;
            PDPNMSG_CONNECT_COMPLETE pConnectCompleteMsg;
            pConnectCompleteMsg = (PDPNMSG_CONNECT_COMPLETE)pMsgBuffer;

            hrConnectComplete = pConnectCompleteMsg->hResultCode;

            SetEvent(g_hConnectEvent);

            break;
        }

        case DPN_MSGID_ASYNC_OP_COMPLETE:
        {
            PDPNMSG_ASYNC_OP_COMPLETE pAsyncOpCompleteMsg;
            pAsyncOpCompleteMsg = (PDPNMSG_ASYNC_OP_COMPLETE)pMsgBuffer;

//            if( pAsyncOpCompleteMsg->hAsyncOp == hEnumAsyncOp )
//            {
//            }

            break;
        }

    }

    return S_OK;
}
