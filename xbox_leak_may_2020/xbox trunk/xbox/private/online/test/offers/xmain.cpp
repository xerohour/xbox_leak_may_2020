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
XONLINE_SERVICE			g_rgServices[NUM_SERVICES] =
{
	{ XONLINE_BILLING_OFFERING_SERVICE, S_OK, NULL }
};


//======================== The main function
void __cdecl main()
{
    HRESULT				hr=S_OK;
    DWORD				dwWorkFlags;   

    WSADATA				WsaData;
	DWORD				dwError;

	// test stuff -- garbage values for now.
	
/*


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

    DebugPrint("Calling WSAStartup...\n");
    if(WSAStartup(0x0101, &WsaData) == SOCKET_ERROR)
    {
        DebugPrint("Error %d returned by WSAStartup\n", GetLastError());
        return;
    }

	// Dump the size of the billing context for info
	DebugPrint("Size of context: %u bytes", 
				sizeof(XONLINE_BILLING_OFFERING_SERVICE));

	// Initialize services
	hr = XOnlineLogonWrapper(g_rgServices, NUM_SERVICES);
	if (FAILED(hr))
	{
        DebugPrint("Cannot Logon (%08x), check c:\\xonline.ini\n", hr);
//        RIP("Failed to initialize services");
        return;
	}
*/
	// Initialize test specific stuff
	Initialize();

	// Initialize graphics stuff
	hr = InitializeGraphics();
	if (FAILED(hr))
	{
        DebugPrint("Cannot initialize graphics (%08x)\n", hr);
 //       RIP("Failed to initialize graphics");
        return;
	}

	// Initialize text font  (be sure the tahoma.tff file is on the XDK box)
	hr = InitializeFont(0xffffffff, 0xff000000, XFONT_OPAQUE);
	if (FAILED(hr))
	{
        DebugPrint("Cannot initialize text (%08x)\n", hr);
//        RIP("Failed to initialize text");
        return;
	}

//	Do a little conversion-tastic price display! HOOWAH!
	XONLINE_PRICE pr;
	WCHAR pricestr[50];
	DWORD cbpricestr = 50;

	BeginNewScene();

	// US$15, in the united states
	pr.dwWholePart = 15;
	pr.dwFractionalPart = 0;
	pr.bCurrencyFormat = 0xDA;
	pr.rgchISOCurrencyCode[0] = L'U';
    pr.rgchISOCurrencyCode[1] = L'S';
    pr.rgchISOCurrencyCode[2] = L'D';

	XOnlineOfferingPriceFormat(&pr, pricestr, &cbpricestr, 0x0);
	wsprintf(StatusBuffer, L"Should be: $15.00. Formatted as: %s", pricestr);
	Xputs(StatusBuffer);		
	
	
	// US$15.26, displayed in Canada
	pr.dwWholePart = 15;
	pr.dwFractionalPart = 26;
	pr.bCurrencyFormat = 0xBE;
	pr.rgchISOCurrencyCode[0] = L'U';
    pr.rgchISOCurrencyCode[1] = L'S';
    pr.rgchISOCurrencyCode[2] = L'D';

	cbpricestr = 50;
	XOnlineOfferingPriceFormat(&pr, pricestr, &cbpricestr, 0x0);
	wsprintf(StatusBuffer, L"Should be: US$ 15.26. Formatted as: %s", pricestr);
	Xputs(StatusBuffer);		
	
	// Sfr, displayed in CH.  SHould be SFr 12,00
	pr.dwWholePart = 12;
	pr.dwFractionalPart = 0;
	pr.bCurrencyFormat = 0x2F;
	pr.rgchISOCurrencyCode[0] = L'C';
    pr.rgchISOCurrencyCode[1] = L'H';
    pr.rgchISOCurrencyCode[2] = L'F';

	cbpricestr = 50;
	XOnlineOfferingPriceFormat(&pr, pricestr, &cbpricestr, 0x0);
	wsprintf(StatusBuffer, L"Should be: SFr 12,00. Formatted as: %s", pricestr);
	Xputs(StatusBuffer);		


	// euros, displayed in france. SHould be 19,00E [euro]
	pr.dwWholePart = 19;
	pr.dwFractionalPart = 0;
	pr.bCurrencyFormat = 0x43;
	pr.rgchISOCurrencyCode[0] = L'E';
    pr.rgchISOCurrencyCode[1] = L'U';
    pr.rgchISOCurrencyCode[2] = L'R';

	cbpricestr = 50;
	XOnlineOfferingPriceFormat(&pr, pricestr, &cbpricestr, XO_CURRENCY_EUR);
	wsprintf(StatusBuffer, L"Should be: 19,00[euro]. Formatted as: %s", pricestr);
	Xputs(StatusBuffer);	


	// yen, displayed in japan(?). SHould be [yen]10000
	pr.dwWholePart = 10000;
	pr.dwFractionalPart = 0;
	pr.bCurrencyFormat = 0x68;
	pr.rgchISOCurrencyCode[0] = L'J';
    pr.rgchISOCurrencyCode[1] = L'P';
    pr.rgchISOCurrencyCode[2] = L'Y';

	cbpricestr = 50;
	XOnlineOfferingPriceFormat(&pr, pricestr, &cbpricestr, XO_CURRENCY_JPY);
	wsprintf(StatusBuffer, L"Should be: [yen]10000. Formatted as: %s", pricestr);
	Xputs(StatusBuffer);	

	// pounds, displayed in gb. SHould be [pounds]32.00
	pr.dwWholePart = 32;
	pr.dwFractionalPart = 0;
	pr.bCurrencyFormat = 0x5A;
	pr.rgchISOCurrencyCode[0] = L'G';
    pr.rgchISOCurrencyCode[1] = L'B';
    pr.rgchISOCurrencyCode[2] = L'P';

	cbpricestr = 50;
	XOnlineOfferingPriceFormat(&pr, pricestr, &cbpricestr, XO_CURRENCY_GBP);
	wsprintf(StatusBuffer, L"Should be: [pounds]32.00. Formatted as: %s", pricestr);
	Xputs(StatusBuffer);	

	// won, displayed in soko. SHould be [won]1500
	pr.dwWholePart = 1500;
	pr.dwFractionalPart =230;
	pr.bCurrencyFormat = 0x78;
	pr.rgchISOCurrencyCode[0] = L'K';
    pr.rgchISOCurrencyCode[1] = L'R';
    pr.rgchISOCurrencyCode[2] = L'W';

	cbpricestr = 50;
	XOnlineOfferingPriceFormat(&pr, pricestr, &cbpricestr, XO_CURRENCY_KRW);
	wsprintf(StatusBuffer, L"Should be: [won]1500. Formatted as: %s", pricestr);
	Xputs(StatusBuffer);
	
	// won, displayed somewhere outside soko with no char. SHould be [won]1500
	pr.dwWholePart = 1500;
	pr.dwFractionalPart =230;
	pr.bCurrencyFormat = 0x78;
	pr.rgchISOCurrencyCode[0] = L'K';
    pr.rgchISOCurrencyCode[1] = L'R';
    pr.rgchISOCurrencyCode[2] = L'W';

	cbpricestr = 50;
	XOnlineOfferingPriceFormat(&pr, pricestr, &cbpricestr, 0);
	wsprintf(StatusBuffer, L"Should be: KRW 1500. Formatted as: %s", pricestr);
	Xputs(StatusBuffer);	


	// totally foreign currency XON, displayed in the US. SHould be XON 14.22
	pr.dwWholePart = 14;
	pr.dwFractionalPart = 22;
	pr.bCurrencyFormat = 0xFE;
	pr.rgchISOCurrencyCode[0] = L'X';
    pr.rgchISOCurrencyCode[1] = L'O';
    pr.rgchISOCurrencyCode[2] = L'N';

	cbpricestr = 50;
	XOnlineOfferingPriceFormat(&pr, pricestr, &cbpricestr, 0x0);
	wsprintf(StatusBuffer, L"Should be: XON 14.22. Formatted as: %s", pricestr);
	Xputs(StatusBuffer);	

	// totally foreign currency XON, displayed in france. SHould be 14,22 XON
	pr.dwWholePart = 14;
	pr.dwFractionalPart = 22;
	pr.bCurrencyFormat = 0xF7;
	pr.rgchISOCurrencyCode[0] = L'X';
    pr.rgchISOCurrencyCode[1] = L'O';
    pr.rgchISOCurrencyCode[2] = L'N';

	cbpricestr = 50;
	XOnlineOfferingPriceFormat(&pr, pricestr, &cbpricestr, 0x0);
	wsprintf(StatusBuffer, L"Should be: 14,22 XON. Formatted as: %s", pricestr);
	Xputs(StatusBuffer);	


	PresentScene();										   








#if 0

	// start by sending the request 

	hr = XOnlineOfferingPurchase( 0, dwOfferingId, NULL, &hTask );
	if (FAILED(hr)) 
	{
		DebugPrint("  Purchase failed (%08x)", hr );
//		RIP("Purchase failed");
		return;
	}

	while (1) 
	{
		if (hTask)
		{
			dwPumpCalls++;
			hr = XOnlineTaskContinue( hTask, GetTickCount()+1 );
			if( hr != XONLINETASK_S_RUNNING )
			{
				// task completed, success?
				if( FAILED(hr) ) 
				{
					//bugger.  this shouldn't fail!
					BeginNewScene();
					wsprintf(StatusBuffer, L"   Purchase failed (%08x) after %u pump calls\r\n",
										   hr, dwPumpCalls );
				
					Xputs(StatusBuffer);		
					PresentScene();

					break;
				}
				
				// otherwise, success!
				
				// close task handle
				BeginNewScene();
				wsprintf(StatusBuffer, L"   Purchase succeeded (%08x) after %u pump call(s)\r\n",
										   hr, dwPumpCalls );
				
				Xputs(StatusBuffer);		
				PresentScene();
				break;
			}
		}
	}

	// we're done
	XOnlineTaskClose( hTask );
	hTask = NULL;

#endif
/*
    // Shutdown WinSock subsystem.
    WSACleanup();

    DebugPrint("Unloading XBox network stack...\n");
    XNetCleanup();
*/
	// Break here
	_asm int 3;

	return;
}

