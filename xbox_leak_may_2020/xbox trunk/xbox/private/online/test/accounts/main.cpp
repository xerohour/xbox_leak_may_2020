/***************************************************************************
 *
 *  Copyright (C) 2000 Microsoft Corporation.  All Rights Reserved.
 *
 ****************************************************************************/

#include "xonp.h"
#include "testutil.h"
#include "testui.h"
#include "time.h"


void CreateTempUsers()
{
    HRESULT              hr     = S_OK;
    XONLINETASK_HANDLE   hTask;
    DWORD                cUsers = 0;
    
    
    while (true)
    {
    
        __asm int 3;
    
        DebugPrint("Creating user %d", ++cUsers);
        
        XONLINE_USER user;
        WCHAR buf[16] = {0};
    
        ZeroMemory(&user, sizeof(XONLINE_USER));
        
        srand(time(NULL));
        wsprintfA(user.name, "test%x", rand());
        strcpy(user.kingdom, "Earth");
    
    
        hr = _XOnlineAccountTempCreate(&user, NULL, &hTask);
        if (FAILED(hr))
        {
             __asm int 3;
        }
        
        do
        {
            hr = XOnlineTaskContinue(hTask);
               
            if (FAILED(hr))
    
            {
    	            __asm int 3;
            }
        } while (hr == XONLINETASK_S_RUNNING);
    
    
        if (SUCCEEDED(hr))
        {
            _XOnlineAccountTempCreateGetResults(hTask, &user);
        }
        
        XOnlineTaskClose(hTask);
    
    }
    
}


void GetTags()
{
    HRESULT              hr                  = S_OK;
    XONLINETASK_HANDLE   hTask;
    LPWSTR               rgszTags            = NULL;
    WORD                 wTagCount           = 0;
    
    while (true)
    {
        __asm int 3;

		//hr = _XOnlineGetTags(103, 5, L"slamb", NULL, &hTask);
		hr = _XOnlineGetTags(103, 20, NULL, NULL, &hTask);

        if (FAILED(hr))
        {
             __asm int 3;
        }
        
        do
        {
            hr = XOnlineTaskContinue(hTask);
        
            if (FAILED(hr))
            {
    	            __asm int 3;
            }
        } 
        while (hr == XONLINETASK_S_RUNNING);
		
    
        if (SUCCEEDED(hr))
        {
            hr = _XOnlineGetTagsResults(hTask, &rgszTags, &wTagCount);
            if (FAILED(hr))
            {
                __asm int 3;
            }

            for (int i = 0; i < wTagCount; i++)
            {
                DebugPrint("name %d: %ls\n", i, rgszTags + i*XONLINE_KINGDOM_SIZE);
            }
        }

        
        XOnlineTaskClose(hTask);


    }
}



void ReserveName()
{
    HRESULT              hr                 = S_OK;
    XONLINETASK_HANDLE   hTask;
    PUSER_XNAME          pNames              = NULL;
    DWORD                dwNameCount          = 0;
    
    while (true)
    {
        hr = _XOnlineReserveName(L"slamb", L"Dev", 5, 103, NULL, &hTask);

        if (FAILED(hr))
        {
             __asm int 3;
        }
        
        do
        {
            hr = XOnlineTaskContinue(hTask);
        
            if (FAILED(hr))
            {
    	            __asm int 3;
            }
        } 
        while (hr == XONLINETASK_S_RUNNING);
    
    
        if (SUCCEEDED(hr))
        {
            hr = _XOnlineGetReserveNameResults(hTask, &pNames, &dwNameCount);
            if (FAILED(hr))
            {
                __asm int 3;
            }

            for (DWORD i = 0; i < dwNameCount; i++)
            {
                DebugPrint("tag %d: %ls.%ls\n", i, pNames[i].wszNickname, pNames[i].wszRealm);
            }
        }

        
        XOnlineTaskClose(hTask);


    }
}

void CreateUsers()
{
    HRESULT              hr                 = S_OK;
    XONLINETASK_HANDLE   hTask;
    USER_ACCOUNT_INFO    acctInfo;
    XONLINE_USER         user;

    
    while (true)
    {
        ZeroMemory(&acctInfo, sizeof(USER_ACCOUNT_INFO));

        __asm int 3;
    
        
        srand(time(NULL));
        wsprintfW(acctInfo.wszNickname, L"User%x", rand());
        wcscpy(acctInfo.wszKingdom, L"Dev");

        wcscpy(acctInfo.wszFirstName, L"Joe");
        wcscpy(acctInfo.wszLastName, L"Shmo");
        wcscpy(acctInfo.wszStreet1, L"1 Microsoft Way");
        wcscpy(acctInfo.wszCity, L"Redmond");
        wcscpy(acctInfo.wszState, L"WA");
        acctInfo.bCountryId = 103;
        wcscpy(acctInfo.wszPostalCode, L"98072");
        wcscpy(acctInfo.wszPhonePrefix, L"425");
        wcscpy(acctInfo.wszPhoneNumber, L"703-8716");
        
        wcscpy(acctInfo.wszCardHolder, L"Joe Shmo");
        wcscpy(acctInfo.wszCardNumber, L"4111111111111111");

        SYSTEMTIME st;
        ZeroMemory(&st, sizeof(SYSTEMTIME));
        st.wYear = 2003;
        st.wMonth = 2;
        
        SystemTimeToFileTime(&st, &(acctInfo.ftCardExpiration));
        
        hr = _XOnlineCreateAccount(&acctInfo, NULL, &hTask);
        if (FAILED(hr))
        {
             __asm int 3;
        }
        
        do
        {
            hr = XOnlineTaskContinue(hTask);
        
            if (FAILED(hr))
            {
    	            __asm int 3;
            }
        } 
        while (hr == XONLINETASK_S_RUNNING);
    
    
        if (SUCCEEDED(hr))
        {
            
            ZeroMemory(&user, sizeof(XONLINE_USER));
            hr = _XOnlineGetCreateAccountResults(hTask, &user);
            if (FAILED(hr))
            {
                __asm int 3;
            }

            DebugPrint("Created user %s.%s (0x%016x)\n", user.name, user.kingdom, user.xuid.qwUserID);
        }

        
        XOnlineTaskClose(hTask);


    }
}



//
// Define the services needed
//
#define NUM_SERVICES	1
DWORD			g_rgServices[NUM_SERVICES] =
{
	XONLINE_USER_ACCOUNT_SERVICE
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
    XONLINETASK_HANDLE   hLogon;
    DWORD n;
    XONLINE_USER Users[8];

	XONLINE_STARTUP_PARAMS	xosp = { 0 };

	XNetStartupParams 	xnsp = { sizeof(XNetStartupParams), 
								XNET_STARTUP_BYPASS_SECURITY };

    DebugPrint("Loading XBox network stack...\n");
    dwError = XNetStartup(&xnsp);
    if (dwError != NO_ERROR)
    {
	    DebugPrint("Failed XNetStartup with %d\n", dwError);
        return;
    }

    DebugPrint("Calling XOnlineStartup...\n");
    hr = XOnlineStartup(&xosp);
    if (FAILED(hr))
    {
        DebugPrint("Error %08x returned by XOnlineStartup\n", hr);
        return;
    }

    	// Initialize services
	hr = XOnlineLogonWrapper(g_rgServices, NUM_SERVICES, &hLogon);
	if (FAILED(hr))
	{
        DebugPrint("Cannot Logon (%08x), check c:\\xonline.ini\n", hr);
        return;
	}

    // 
    // tests
    //

    //CreateTempUsers();
    //CreateUsers();
    //GetTags();
    ReserveName();


    

    // 
    // cleanup
    //
   
    XOnlineTaskClose(hLogon);
    
    WSACleanup();
    XNetCleanup();
}


