/***************************************************************************
 *
 *  Copyright (c) Microsoft Corporation.  All rights reserved
 *
 ****************************************************************************/

#include <xtl.h>
#include <xonline.h>
#include <xdbg.h>

HRESULT GamerStuff(PXONLINE_USER pUsers);
HRESULT WorkUntilDone(XONLINETASK_HANDLE hTask);
HRESULT WorkForABit(XONLINETASK_HANDLE hTask, DWORD dwTime);

XONLINETASK_HANDLE g_hLogon;

void __cdecl main()
{
    HRESULT      hr     = S_OK;
    DWORD        cUsers = 0;
    XONLINE_USER Users[XONLINE_MAX_STORED_ONLINE_USERS];

    XInitDevices(0,NULL);

    hr = XOnlineStartup(NULL);
    ASSERT(SUCCEEDED(hr));
    
    hr = XOnlineGetUsers( Users, &cUsers );
    ASSERT(SUCCEEDED(hr));
    ASSERT(cUsers == 4);

    hr = XOnlineNotificationSetUserData( 0, 4, (PBYTE) "ABC", NULL, NULL);
    ASSERT(SUCCEEDED(hr));

    hr = XOnlineNotificationSetUserData( 1, 4, (PBYTE) "XYZ", NULL, NULL);
    ASSERT(SUCCEEDED(hr));

    hr = XOnlineLogon( Users, NULL, 0, NULL, &g_hLogon );
    ASSERT(SUCCEEDED(hr));
 
    do
    {
        hr = XOnlineTaskContinue(g_hLogon);
        if ( hr != XONLINETASK_S_RUNNING )
        {
            if (hr == XONLINE_S_LOGON_CONNECTION_ESTABLISHED)
            {
                break;
            }
            else if (hr == XONLINE_E_LOGON_SERVICE_UNAVAILABLE)
            {
                __asm int 3;
            }
            else if (hr == XONLINE_E_LOGON_UPDATE_REQUIRED)
            {
                __asm int 3;
            }
            else
            {
                __asm int 3;
            }
        }
    } while (1);

    __asm int 3;

    hr = GamerStuff( Users );
    ASSERT(SUCCEEDED(hr));

    hr = XOnlineCleanup();
    ASSERT(SUCCEEDED(hr));

    __asm int 3;
}

HRESULT GamerStuff(PXONLINE_USER pUsers)
{
    HRESULT                     hr              = S_OK;
    DWORD                       i, j            = 0;
    XONLINETASK_HANDLE          hTask           = NULL;
    XONLINETASK_HANDLE          hGamerTask      = NULL;
    XONLINETASK_HANDLE          hMuteTask       = NULL;
    PXONLINE_GAMER              pGamers         = NULL;
    DWORD                       dwNumGamers     = 0;
    PXONLINE_MUTEDUSER          pMutelist       = NULL;
    DWORD                       dwNumMutedUsers = 0;
    XNKID                       SessionID       = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    XONLINE_ACCEPTED_GAMEINVITE GameInvite;

    pGamers   = (PXONLINE_GAMER)     LocalAlloc(0, MAX_GAMERS * sizeof(XONLINE_GAMER));
    pMutelist = (PXONLINE_MUTEDUSER) LocalAlloc(0, MAX_MUTEDUSERS * sizeof(XONLINE_MUTEDUSER));

    hr = XOnlineGamersStartup( NULL, &hGamerTask );
    ASSERT(SUCCEEDED(hr));

    for (i = 0; i < 4; i += 1)
    {
        hr = XOnlineGamersEnumerate( i, NULL, &hTask );
        ASSERT(SUCCEEDED(hr));

        hr = WorkForABit(hTask, 1000);
        ASSERT(SUCCEEDED(hr));

        XOnlineTaskClose(hTask);

        DWORD dwNumGamers = XOnlineGamersGetLatest( i, MAX_GAMERS, pGamers );

        for (j = 0; j < dwNumGamers; j += 1)
        {
            hr = XOnlineGamersRemove( i, &(pGamers[j]) );
            ASSERT(SUCCEEDED(hr));
        }

        hr = WorkForABit(hGamerTask, 2000);
        ASSERT(SUCCEEDED(hr));
    }

    hr = XOnlineGamersRequest( 0, pUsers[1].xuid );
    ASSERT(SUCCEEDED(hr));

    hr = XOnlineGamersRequest( 0, pUsers[2].xuid );
    ASSERT(SUCCEEDED(hr));

    hr = XOnlineGamersRequest( 0, pUsers[3].xuid );
    ASSERT(SUCCEEDED(hr));

    hr = WorkForABit(hGamerTask, 4000);
    ASSERT(SUCCEEDED(hr));

    hr = XOnlineGamersAnswerRequest( 1, pUsers[0].xuid, XONLINE_REQUEST_YES );
    ASSERT(SUCCEEDED(hr));

    hr = XOnlineGamersAnswerRequest( 2, pUsers[0].xuid, XONLINE_REQUEST_NO );
    ASSERT(SUCCEEDED(hr));

    hr = WorkForABit(hGamerTask, 4000);
    ASSERT(SUCCEEDED(hr));

    hr = XOnlineMutelistStartup( NULL, &hMuteTask );
    ASSERT(SUCCEEDED(hr));

    hr = XOnlineMutelistAdd( 0, pUsers[1].xuid, pUsers[1].name );
    ASSERT(SUCCEEDED(hr));

    hr = XOnlineMutelistAdd( 0, pUsers[2].xuid, pUsers[2].name );
    ASSERT(SUCCEEDED(hr));

    hr = WorkForABit(hMuteTask, 4000);
    ASSERT(SUCCEEDED(hr));

    hr = XOnlineMutelistRemove( 0, pUsers[1].xuid );
    ASSERT(SUCCEEDED(hr));

    hr = WorkForABit(hMuteTask, 4000);
    ASSERT(SUCCEEDED(hr));

    dwNumMutedUsers = XOnlineMutelistGet( 0, MAX_MUTEDUSERS, pMutelist );

    hr = XOnlineNotificationSetState(0, XONLINE_GAMERSTATE_FLAG_ONLINE | XONLINE_GAMERSTATE_FLAG_PLAYING, SessionID, 4, (PBYTE) "DEF");
    ASSERT(SUCCEEDED(hr));

    hr = XOnlineNotificationSetUserData( 0, 4, (PBYTE) "GHI", NULL, &hTask);
    ASSERT(SUCCEEDED(hr));

    hr = WorkUntilDone(hTask);
    ASSERT(SUCCEEDED(hr));

    XOnlineTaskClose(hTask);

    hr = XOnlineGamersEnumerate( 0, NULL, &hTask );
    ASSERT(SUCCEEDED(hr));

    hr = WorkForABit(hTask, 4000);
    ASSERT(SUCCEEDED(hr));

    dwNumGamers = XOnlineGamersGetLatest( 0, MAX_GAMERS, pGamers );

    hr = XOnlineGamersGameInvite( 0, SessionID, 1, &pGamers[0] );
    ASSERT(SUCCEEDED(hr));

    XOnlineTaskClose(hTask);

    hr = WorkForABit(hGamerTask, 4000);
    ASSERT(SUCCEEDED(hr));

    hr = XOnlineGamersEnumerate( 1, NULL, &hTask );
    ASSERT(SUCCEEDED(hr));

    hr = WorkForABit(hTask, 4000);
    ASSERT(SUCCEEDED(hr));

    dwNumGamers = XOnlineGamersGetLatest( 1, MAX_GAMERS, pGamers );

    hr = XOnlineGamersAnswerGameInvite( 1, &pGamers[0], XONLINE_GAMEINVITE_YES);
    ASSERT(SUCCEEDED(hr));

    XOnlineTaskClose(hTask);

    hr = WorkForABit(hGamerTask, 4000);
    ASSERT(SUCCEEDED(hr));

    hr = XOnlineGamersGetAcceptedGameInvite( NULL, &hTask );

    hr = WorkUntilDone(hTask);
    ASSERT(SUCCEEDED(hr));

    hr = XOnlineGamersGetAcceptedGameInviteResult( hTask, &GameInvite );

    XOnlineTaskClose(hTask);

    __asm int 3;

    XOnlineTaskClose(hGamerTask);

    XOnlineTaskClose(hMuteTask);

    LocalFree((HLOCAL) pGamers);

    LocalFree((HLOCAL) pMutelist);

    return hr;
}

HRESULT WorkUntilDone(XONLINETASK_HANDLE hTask)
{
    HRESULT hr = S_OK;

    while (1)
    {
        hr = XOnlineTaskContinue(hTask);

        if (hr != XONLINETASK_S_RUNNING)
        {
            if (FAILED(hr))
                __asm int 3;

            break;
        }

        if (FAILED(XOnlineTaskContinue(g_hLogon)))
        {
            __asm int 3;
        }
    }

    return hr;
}

HRESULT WorkForABit(XONLINETASK_HANDLE hTask, DWORD dwTime)
{
    HRESULT hr        = S_OK;
    DWORD   dwEndTime = GetTickCount() + dwTime;

    while (GetTickCount() < dwEndTime)
    {
        hr = XOnlineTaskContinue(hTask);

        if (hr != XONLINETASK_S_RUNNING && hr != XONLINE_S_NOTIFICATION_UPTODATE)
        {
            if (FAILED(hr))
                __asm int 3;

            break;
        }

        if (FAILED(XOnlineTaskContinue(g_hLogon)))
        {
            __asm int 3;
        }
    }

    return hr;
}

