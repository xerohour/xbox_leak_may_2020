/***************************************************************************
 *
 *  Copyright (C) 2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       test.cpp
 *  Content:    DirectPlay Host Test App
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  06/08/00    mgere  Created.
 *
 ****************************************************************************/

#include <xtl.h>
#include <dplay8.h>
#ifdef DPLAY_DOWORK_STATEMN
#include <statemn.h>
#endif

DWORD WINAPI WorkerThread( void *pParam );
HRESULT WINAPI DirectPlayMessageHandler( PVOID pvUserContext, DWORD dwMessageId, PVOID pMsgBuffer );

HANDLE			g_hEvent = NULL;
HANDLE			g_hConnectEvent = NULL;
LPDIRECTPLAY8ADDRESS        g_pConnectDeviceAddress;
LPDIRECTPLAY8ADDRESS        g_pConnectHostAddress;
DPN_APPLICATION_DESC        g_pConnectAppDesc;
LPDIRECTPLAY8PEER           pDP;
DPNHANDLE                   g_hEnumAsyncOp;
DPNID              g_dpnidLocalPlayer            = 0;       // DPNID of local player
LONG               g_lNumberOfActivePlayers      = 0;       // Number of players currently in game
TCHAR              g_strLocalPlayerName[MAX_PATH];          // Local player name
DPNID		g_dpnidOneOfThePlayers = 0;
BOOL            g_fThreadRun = FALSE;

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
    CHAR blah[256];
};

void __cdecl main()
{
    HRESULT                     hr;
    LPDIRECTPLAY8ADDRESS        pDeviceAddress;
    LPDIRECTPLAY8ADDRESS        pHostAddress;
    DWORD                       dwItems     = 0;
    DWORD                       dwSize      = 0;
    DPN_SP_CAPS                 dpspCaps;
    DPN_APPLICATION_DESC        dnAppDesc;
    GUID                        guidApp = { 0x2ae836d, 0x9179, 0x485f, { 0x83, 0x43, 0x90, 0x1d, 0x32, 0x7c, 0xe7, 0x94 } };
    INT                         err = 0;
    DWORD			dwPort = 2350;
    DPN_PLAYER_INFO dpPlayerInfo;
    WCHAR wszSessionName[MAX_PATH];
    DPNHANDLE                   hConnectAsyncOp;
    GAMEMSG_GENERIC msgWave;
    DPN_BUFFER_DESC bufferDesc;
    DPNHANDLE hAsync;
    DWORD dwThreadID = 0;
    HANDLE hThread = NULL;
    DPN_GROUP_INFO dpngi;

    __asm int 3

    g_hEvent = CreateEvent(NULL, TRUE, FALSE,NULL);
    g_hConnectEvent = CreateEvent(NULL, TRUE, FALSE,NULL);

    ResetEvent(g_hEvent);    
    ResetEvent(g_hConnectEvent);    

    err = XnetInitialize(NULL, TRUE);
    if (err != NO_ERROR)
        __asm int 3

    hr = DPlayInitialize();
    if (FAILED(hr))
    {
        __asm int 3;
    }

    hr = DirectPlay8Create(IID_IDirectPlay8Peer, (void **) &pDP, NULL);
    if( FAILED(hr) )
        __asm int 3

    g_fThreadRun = TRUE;

    hr = pDP->Initialize( NULL, DirectPlayMessageHandler, 0 );
    if( FAILED(hr) )
        __asm int 3

    dpspCaps.dwSize = sizeof( DPN_SP_CAPS );

    pDP->GetSPCaps(NULL, &dpspCaps,0);
    dpspCaps.dwSystemBufferSize = 8192;
    pDP->SetSPCaps(NULL, &dpspCaps,0);
    pDP->SetSPCaps(NULL, &dpspCaps,0);

    hThread = CreateThread( NULL, 0, WorkerThread, 0, 0, &dwThreadID );

    // Create a device address
    hr = DirectPlay8AddressCreate(IID_IDirectPlay8Address, (void **) &pDeviceAddress, NULL);
    if( FAILED(hr) )
        __asm int 3

    pDeviceAddress->AddComponent( L"port", (void*) &dwPort, sizeof(dwPort), DPNA_DATATYPE_DWORD );

    ZeroMemory( &dnAppDesc, sizeof(DPN_APPLICATION_DESC) );
    dnAppDesc.dwSize          = sizeof(DPN_APPLICATION_DESC);
    dnAppDesc.guidApplication = guidApp;
    dnAppDesc.pwszSessionName = L"MyGamexboxblah";
    dnAppDesc.dwMaxPlayers    = 5;
    dnAppDesc.dwFlags         = 0;

    hr = pDP->Host( &dnAppDesc, &pDeviceAddress, 1, NULL, NULL, NULL, 0 );
    if( FAILED( hr ) )
        __asm int 3

    WaitForSingleObject(g_hConnectEvent, INFINITE);

    msgWave.dwType = GAME_MSGID_WAVE;
    bufferDesc.dwBufferSize = sizeof(GAMEMSG_GENERIC);
    bufferDesc.pBufferData  = (BYTE*) &msgWave;

//    while (1)
    pDP->SendTo( DPNID_ALL_PLAYERS_GROUP, &bufferDesc, 1, 0, NULL, &hAsync, DPNSEND_NOLOOPBACK );

//    pDP->TerminateSession(NULL, 0, 0);

    SleepEx(15000, FALSE);

    g_fThreadRun = FALSE;
    pDP->Close(0);
//    SleepEx(1000, FALSE);
    pDP->Release();
    pDeviceAddress->Release();

    hr = DPlayCleanup();
    if (FAILED(hr))
    {
        __asm int 3;
    }

    __asm int 3
}

DWORD WINAPI WorkerThread( void *pParam )
{
    while (1)
    {
        if (g_fThreadRun && pDP->DoWork(0) == S_FALSE)
	{
            SleepEx(10, FALSE);
	}
    }

    return 0;
}

HRESULT WINAPI DirectPlayMessageHandler( PVOID pvUserContext, DWORD dwMessageId, PVOID pMsgBuffer )
{
    static HRESULT hrConnectResult = E_FAIL;

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
            hr = pDP->GetPeerInfo( pCreatePlayerMsg->dpnidPlayer, pdpPlayerInfo, &dwSize, 0 );
            if( FAILED(hr) && hr != DPNERR_BUFFERTOOSMALL )
                return hr;

            pdpPlayerInfo = (DPN_PLAYER_INFO*) new BYTE[ dwSize ];
            ZeroMemory( pdpPlayerInfo, dwSize );
            pdpPlayerInfo->dwSize = sizeof(DPN_PLAYER_INFO);
            hr = pDP->GetPeerInfo( pCreatePlayerMsg->dpnidPlayer, pdpPlayerInfo, &dwSize, 0 );
            if( FAILED(hr) )
                return hr;

            // Create a new and fill in a APP_PLAYER_INFO
            APP_PLAYER_INFO* pPlayerInfo = new APP_PLAYER_INFO;
            ZeroMemory( pPlayerInfo, sizeof(APP_PLAYER_INFO) );
            pPlayerInfo->lRefCount   = 1;
            pPlayerInfo->dpnidPlayer = pCreatePlayerMsg->dpnidPlayer;

g_dpnidOneOfThePlayers = pPlayerInfo->dpnidPlayer;

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

            if (g_lNumberOfActivePlayers == 2){
                SetEvent(g_hConnectEvent);
            }

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

            SetEvent(g_hEvent);

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
/*
                hr = pDP->GetPlayerContext( pPlayerInfo->dpnidPlayer, (LPVOID* const) &pPlayerInfo2, 0);

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
*/
            }

            SetEvent(g_hEvent);

            break;
        }

        case DPN_MSGID_INDICATE_CONNECT:
        {
//            SetEvent(g_hConnectEvent);

		if (hrConnectResult == E_FAIL)
		{
			hrConnectResult = S_OK;
			return E_FAIL;
		}
            break;
        }

        case DPN_MSGID_ASYNC_OP_COMPLETE:
        {
            PDPNMSG_ASYNC_OP_COMPLETE pAsyncOpCompleteMsg;
            pAsyncOpCompleteMsg = (PDPNMSG_ASYNC_OP_COMPLETE)pMsgBuffer;

            break;
        }

    }

    return S_OK;
}
