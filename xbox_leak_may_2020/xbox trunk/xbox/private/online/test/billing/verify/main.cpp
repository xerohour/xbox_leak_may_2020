/***************************************************************************
 *
 *  Copyright (C) 2000 Microsoft Corporation.  All Rights Reserved.
 *
 ****************************************************************************/

#include <xtl.h>
#include <xonlinep.h>
#include <xdbg.h>
#include <stdio.h>
#include "testutil.h"
#include "testui.h"



//
// Define the services needed
//
#define NUM_SERVICES	1
#define NUM_SERVICES	1
DWORD			g_rgServices[NUM_SERVICES] =
{
	{ XONLINE_BILLING_OFFERING_SERVICE }
};


void __cdecl main()
{
    HRESULT              hr                = S_OK;
    DWORD                dwError           = 0;
    HANDLE               hEvent            = NULL;
    WSADATA              WsaData;
    DWORD                dwWorkFlags;
    DWORD i;
    XNADDR xna;
    XONLINETASK_HANDLE   hTask;
    XONLINETASK_HANDLE   hLogon;
    DWORD n;
    XONLINE_USER Users[8];
    DWORD                cUsers = 0;

    XNetStartupParams 	xnsp = { sizeof(XNetStartupParams), 
                            XNET_STARTUP_BYPASS_SECURITY };


	while (true)
    {
    DebugPrint("Loading XBox network stack...\n");
    dwError = XNetStartup(&xnsp);
    if (dwError != NO_ERROR)
    {
        DebugPrint("Failed XNetStartup with %d\n", dwError);
        return;
    }

    DebugPrint( "Starting up XONLINE facilities\n");
    dwError = XOnlineStartup(NULL);
    if(dwError != S_OK)
    {
        DebugPrint("Failed XOnlineStartup with %x\n", dwError);
        return;
    } 


    	// Initialize services
	hr = XOnlineLogonWrapper(g_rgServices, NUM_SERVICES);
	if (FAILED(hr))
	{
            DebugPrint("Cannot Logon (%08x), check c:\\xonline.ini\n", hr);
//        RIP("Failed to initialize services");
        return;
	}


#if 0
    hr = XOnlineGetUsersFromHD( Users, &cUsers );
    if (FAILED(hr))
        __asm int 3;

    hr = XOnlineLogon( Users, g_rgServices, NUM_SERVICES, NULL, &hLogon );
    if (FAILED(hr))
        __asm int 3;
 
    do
    {
        hr = XOnlineTaskContinue(hLogon, GetTickCount()+5000);
        if ( hr != XONLINETASK_S_RUNNING )
        {
            if (hr == XLOGON_S_CONNECTION_ESTABLISHED)
            {
                break;
            }
            else if (hr == XLOGON_E_SERVICE_UNAVAILABLE)
            {
                __asm int 3;
            }
            else if (hr == XLOGON_E_UPDATE_REQUIRED)
            {
                __asm int 3;
            }
            else
            {
                __asm int 3;
            }
        }
    } while (1);
#endif

    __asm int 3;

    
    
    // 
    // now the interesting stuff..
    //
    
    hr = XOnlineOfferingVerifyLicense(0x4, XONLINE_LOGON_ALL, NULL, &hTask);
    if (FAILED(hr))
    {
        __asm int 3;
    }
    
    do
    {
        hr = XOnlineTaskContinue(hTask, GetTickCount() + 2);
    
        if (FAILED(hr))
        {
            __asm int 3;
        }
    } while (hr == XONLINETASK_S_RUNNING);

    
    XOnlineTaskClose(hTask);
    hTask = NULL;
    
    
    WSACleanup();
    XNetCleanup();
    }
}


