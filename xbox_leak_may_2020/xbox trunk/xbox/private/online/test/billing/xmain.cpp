/*++

Copyright (c) Microsoft Corporation

Author:
    Ben Zotto (benzotto), d'apres Keith Lau (keithlau)

Description:
	Dumb Test shell for billing functionality

Module Name:

    xmain.c

--*/

#include <xtl.h>
#include <xonlinep.h>

#include <xdbg.h>
#include <stdio.h>

#include "testutil.h"
#include "testui.h"


//
// Enable this macro if we want to wait for event before pumping
//
//#define WAIT_FOR_EVENT


DWORD					dwPumpCalls = 0;


WCHAR StatusBuffer[2400];


void Initialize()
{
	// do nothing 
}


#define NUM_SERVICES	1
DWORD			g_rgServices[NUM_SERVICES] =
{
	XONLINE_BILLING_OFFERING_SERVICE
};


//======================== The main function
void __cdecl main()
{
    HRESULT				hr=S_OK;

    DWORD				dwWorkFlags;   

    WSADATA				WsaData;
	DWORD				dwError;

	// test stuff -- garbage values for now.
	ULONGLONG qwUserPUID = 0xCAFEBABE;
	DWORD dwOfferingId = 0x3;
	XONLINETASK_HANDLE	hTask;
	XONLINE_STARTUP_PARAMS startParams;
	PXONLINE_USER	rgUsers;
    XONLINETASK_HANDLE	hLogon;

	XNetStartupParams 	xnsp = { sizeof(XNetStartupParams), 
								XNET_STARTUP_BYPASS_SECURITY };

    //_asm int 3;

    DebugPrint("Loading XBox network stack...\n");
    dwError = XNetStartup(&xnsp);
    if (dwError != NO_ERROR)
    {
	    DebugPrint("Failed XNetStartup with %d\n", dwError);
        return;
    }

	DebugPrint( "Starting up XONLINE facilities\n");
	startParams.dwReserved = XNET_STARTUP_BYPASS_SECURITY;

	dwError = XOnlineStartup(NULL);
	if(dwError != S_OK)
    {
	    DebugPrint("Failed XOnlineStartup with %x\n", dwError);
        return;
    } 

	// Initialize services
	hr = XOnlineLogonWrapper(g_rgServices, NUM_SERVICES, &hLogon);
	if (FAILED(hr))
	{
        DebugPrint("Cannot Logon (%08x), check c:\\xonline.ini\n", hr);
        return;
	}

	// Initialize test specific stuff
	Initialize();

	// Initialize graphics stuff
	hr = InitializeGraphics();
	if (FAILED(hr))
	{
        DebugPrint("Cannot initialize graphics (%08x)\n", hr);
        return;
	}

	// Initialize text font  (be sure the tahoma.tff file is on the XDK box)
	hr = InitializeFont(0xffffffff, 0xff000000, XFONT_OPAQUE);
	if (FAILED(hr))
	{
        DebugPrint("Cannot initialize text (%08x)\n", hr);
        return;
	}


	// start by sending the request 

	// AHEM! Hack to force the billing client to allow this user.
	rgUsers = XOnlineGetLogonUsers();		
	rgUsers[0].xuid.qwUserID = qwUserPUID;
	rgUsers[0].xuid.dwUserFlags = 0;

	goto Cancel;
// *** ATTEMPT A DETAILS
	hr = XOnlineOfferingGetDetails( 0, 
									dwOfferingId, 
									0,
									0,  
									NULL,
									XOnlineOfferingDetailsMaxSize(0),
									NULL, 
									&hTask );
	if (FAILED(hr)) 
	{
		DebugPrint("  Details failed (%08x)", hr );
		return;
	}

	while (1) 
	{
		if (hTask)
		{
			dwPumpCalls++;
			hr = XOnlineTaskContinue( hTask );
			if( hr != XONLINETASK_S_RUNNING )
			{
				// task completed, success?
				if( FAILED(hr) ) 
				{
					//bugger.  this shouldn't fail!
					BeginNewScene();
					wsprintf(StatusBuffer, L"   Details failed (%08x) after %u pump calls\r\n",
										   hr, dwPumpCalls );
				
					Xputs(StatusBuffer);				
                    goto Die;
				}
				
				// otherwise, success!
				
				BeginNewScene();
				wsprintf(StatusBuffer, L"   Details succeeded (%08x) after %u pump call(s)\r\n",
										   hr, dwPumpCalls );

				PBYTE details;
				DWORD detlen;
				DWORD scrap;
				XONLINE_PRICE prgross, prnet;
				WCHAR strGross[50];
				WCHAR strNet[50];
				XOnlineOfferingDetailsGetResults( hTask, 
													 &details, 
													 &detlen,
													 &prgross,
													 &prnet );
															
				XOnlineOfferingPriceFormat( &prgross, strGross, &scrap, 0 );
				XOnlineOfferingPriceFormat( &prnet, strNet, &scrap, 0 );
				wsprintf(StatusBuffer, L"   Details succeeded (%08x) after %u pump call(s)\r\n"
									   L"    Blob size: %d\r\n    Currency:%c%c%c\r\n    Gross: %s\r\n    Net: %s\r\n"
									   L"\r\n%s", hr, dwPumpCalls, detlen, prgross.rgchISOCurrencyCode[0], prgross.rgchISOCurrencyCode[1],
									   prgross.rgchISOCurrencyCode[2],strGross, strNet, (WCHAR *)details );

                DebugPrint("Details call succeeded!\n");
				Xputs(StatusBuffer);		
				break;
			}
		}
	}

	// we're done
	XOnlineTaskClose( hTask );
	hTask = NULL;



// *** DO A PURCHASE	
	wsprintf(StatusBuffer, L"   -----------------------------------------------------  \r\n" );
    Xputs(StatusBuffer);		
	
    hr = XOnlineOfferingPurchase( 0, dwOfferingId, NULL, &hTask );
	if (FAILED(hr)) 
	{
		DebugPrint("  Purchase failed (%08x)", hr );
		return;
	}

	while (1) 
	{
		if (hTask)
		{
			dwPumpCalls++;
			hr = XOnlineTaskContinue( hTask );
			if( hr != XONLINETASK_S_RUNNING )
			{
				// task completed, success?
				if( FAILED(hr) ) 
				{
					//bugger.  this shouldn't fail!
					wsprintf(StatusBuffer, L"   ... but Purchase failed (%08x) after %u pump calls\r\n",
										   hr, dwPumpCalls );
				    DebugPrint("Purchase failed!\n");
					Xputs(StatusBuffer);		
					goto Die;
				}
				
				// otherwise, success!
				
				// close task handle
				wsprintf(StatusBuffer, L"   ... and Purchase succeeded (%08x) after %u pump call(s)\r\n",
										   hr, dwPumpCalls );
				DebugPrint("purchase call succeeded!\n");
				Xputs(StatusBuffer);		
				break;
			}
		}
	}

	// we're done
	XOnlineTaskClose( hTask );
	hTask = NULL;

// *** DO A PURCHASE	
	wsprintf(StatusBuffer, L"   -----------------------------------------------------  \r\n" );
    Xputs(StatusBuffer);		

Cancel:
//attempt cancellation of previous offer
    hr = XOnlineOfferingCancel( 0, dwOfferingId, NULL, &hTask );
	if (FAILED(hr)) 
	{
		DebugPrint("  purchase failed (%08x)", hr );
		return;
	}

	while (1) 
	{
		if (hTask)
		{
			dwPumpCalls++;
			hr = XOnlineTaskContinue( hTask );
			if( hr != XONLINETASK_S_RUNNING )
			{
				// task completed, success?
				if( FAILED(hr) ) 
				{
					//bugger.  this shouldn't fail!
					wsprintf(StatusBuffer, L"   Cancel failed (%08x) after %u pump calls\r\n",
										   hr, dwPumpCalls );
				
					Xputs(StatusBuffer);		
					goto Die;
				}
				
				// otherwise, success!
				
				// close task handle
				wsprintf(StatusBuffer, L"   Cancel succeeded (%08x) after %u pump call(s)\r\n",
										   hr, dwPumpCalls );
				
				Xputs(StatusBuffer);		
				goto Die;
			}
		}
	}



Die:
	// we're done
	XOnlineTaskClose( hTask );
	hTask = NULL;
    PresentScene();
    DebugPrint("Unloading XBox network stack...\n");
	XOnlineCleanup();

	// Break here
	_asm int 3;

	return;
}

