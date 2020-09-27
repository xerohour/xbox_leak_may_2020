/*++

Copyright (c) Microsoft Corporation

Author:
    Keith Lau (keithlau)

Description:
	Unit test shell for asynchronous content enumeration

Module Name:

    xmain.c

--*/

#include "xonp.h"
#include "testutil.h"
#include "testui.h"

#define MAX_TITLE_ID			4
#define MAX_LANG_ID				4

DWORD dwCurTitle = 0;
DWORD dwCurLang = 0;

DWORD rgdwTitleId[MAX_TITLE_ID] =
{
	0, 100, 101, 102
};

DWORD rgdwLangId[MAX_LANG_ID] =
{
	1, 2, 3, 4
};

VOID DisplayTitles()
{
	HRESULT				hr;
	XONLINETASK_HANDLE	hTask;
    XONLINETASK_HANDLE	hLogon;
	WCHAR				wsz[2048];
	DWORD				*rgTitleId;
	DWORD				dwTitleId;
	DWORD				i;

	BeginNewScene();

	Xputs(L"Titles found on this machine:");

	hr = XOnlineEnumerateTitlesBegin(2, NULL, &hTask);
	Assert(SUCCEEDED(hr));
	Assert(hTask != NULL);

	do
	{
		hr = XOnlineTaskContinue(hTask);
		if (XONLINETASK_STATUS_AVAILABLE(hr))
		{
			// Bail if the subtask indicated failure
			Assert(SUCCEEDED(hr));

			hr = XOnlineEnumerateTitlesGetResults(hTask, &rgTitleId, &dwTitleId);
			Assert(SUCCEEDED(hr));

			Xputs(L"============================");

			if (dwTitleId)
			{
				for (i = 0; i < dwTitleId; i++)
				{
					wsprintf(wsz, L"%08x (%u)", rgTitleId[i], rgTitleId[i]);
					Xputs(wsz);
				}
			}
			else
			{
				Xputs(L"No more results.");
				break;
			}
		}
		
	} while (1);

	PresentScene();
}

VOID DisplayResults(
			ULARGE_INTEGER			*puliIterations,
			DWORD					dwEnumerations,
			PXONLINEOFFERING_INFO	*rgpEnumInfo,
			DWORD					dwResults
			)
{
	WCHAR	wsz[2048];
	DWORD	i;
	SYSTEMTIME	st;

	BeginNewScene();

	wsprintf(wsz, L"%I64u Pump calls made\r\n"\
				L"Title ID: %u\r\n"\
				L"Enumeration #%u returned %u results.\r\n \r\n"\
				L"==========================================================\r\n",
				*puliIterations, rgdwTitleId[dwCurTitle], 
				dwEnumerations, dwResults);
	Xputs(wsz);

	if (dwResults)
	{
		for (i = 0; i < dwResults; i++)
		{
			FileTimeToSystemTime(&(rgpEnumInfo[i]->ftActivationDate), &st);
			
			wsprintf(wsz, L"%u  %02d/%02d/%04d  %08x  %08x  %u  %u  %u\r\n",
						rgpEnumInfo[i]->OfferingId,
						st.wMonth, st.wDay, st.wYear,
						rgpEnumInfo[i]->dwOfferingType,
						rgpEnumInfo[i]->dwBitFlags,
						rgpEnumInfo[i]->dwRating,
						rgpEnumInfo[i]->cbPackageSize,
						rgpEnumInfo[i]->cbInstallSize,
						rgpEnumInfo[i]->cbTitleSpecificData
						);
			Xputs(wsz);
		}
	}
	else
	{
		Xputs(L"No results.");
	}

	PresentScene();
}

#define NUM_SERVICES	1
DWORD			g_rgServices[NUM_SERVICES] =
{
	XONLINE_BILLING_OFFERING_SERVICE
};

//======================== The main function
void __cdecl main()
{
    HRESULT				hr;

	XONLINETASK_HANDLE	hTask = NULL;
    BOOL				fPartial = FALSE;

    WSADATA				WsaData;
	DWORD				dwError;

	PBYTE				pbBuffer = NULL;
	DWORD				cbBuffer;
	DWORD				dwMaxResults;
	DWORD				dwResults;
	SYSTEMTIME			st;
	FILETIME			ftCreatedAfter;

    ULARGE_INTEGER		uliIterations = {0, 0};
	DWORD				dwEnumerations = 0;

	DWORD				dwInitialized;

	XONLINEOFFERING_ENUM_PARAMS	enumparams;
	PXONLINEOFFERING_INFO		*rgpEnumInfo;
    XONLINETASK_HANDLE	hLogon;

	XONLINE_STARTUP_PARAMS	xosp = { 0 };
	
	XNetStartupParams 	xnsp = { sizeof(XNetStartupParams), 
								XNET_STARTUP_BYPASS_SECURITY };


	//  this lets us bypass auth for testing purposes.
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
    	_asm int 3;
        return;
    }

	// Initialize services
	hr = XOnlineLogonWrapper(g_rgServices, NUM_SERVICES, &hLogon);
	if (FAILED(hr))
	{
        DebugPrint("Cannot Logon (%08x), check c:\\xonline.ini\n", hr);
        AssertSz(SUCCEEDED(hr), "Failed to initialize services");
        return;
	}

	// Initialize graphics stuff
	hr = InitializeGraphics();
	if (FAILED(hr))
	{
        DebugPrint("Cannot initialize graphics (%08x)\n", hr);
        AssertSz(SUCCEEDED(hr), "Failed to initialize graphics");
        return;
	}

	// Initialize text font
	hr = InitializeFont(0xffffffff, 0xff000000, XFONT_OPAQUE);
	if (FAILED(hr))
	{
        DebugPrint("Cannot initialize text (%08x)\n", hr);
        AssertSz(SUCCEEDED(hr), "Failed to initialize text");
        return;
	}

	// Set some time for filtering
	st.wYear = 2000;
	st.wMonth = 1;
	st.wDay = 1;
	st.wDayOfWeek = 6;
	st.wHour = 0;
	st.wMinute = 0;
	st.wSecond = 0;
	st.wMilliseconds = 0;
	SystemTimeToFileTime(&st, &ftCreatedAfter);

	// Set up the enumeration params
	enumparams.dwOfferingType = 0xffffffff;	// All types
	enumparams.dwBitFilter = 0xffffffff;	// All offerings
	enumparams.dwMaxResults = 10;			// Up to 10 records at a time
	enumparams.ftActiveAfter = ftCreatedAfter;
	enumparams.dwDescriptionIndex = 0;		// only look at the "english language" description!

	// Allocate buffer to do work
	dwMaxResults = 10;
	cbBuffer = XOnlineOfferingEnumerateMaxSize(&enumparams, 0);
	pbBuffer = (PBYTE)LocalAlloc(LMEM_FIXED, cbBuffer);
	if (!pbBuffer)
	{
        DebugPrint("Cannot allocate memory (%u)\n", GetLastError());
        return;
	}

	ChangeCurrentTitleId(rgdwTitleId[dwCurTitle]);

	// Test title enumeration
	// DisplayTitles();

	// Loop downloading files
	while (1)
	{
		(uliIterations.QuadPart)++;

		// Start the task if completed
		if (!hTask)
		{

			// force this process to go through, avoiding the checks.
			PXONLINE_USER pUsers;
			pUsers = XOnlineGetLogonUsers();
			pUsers[0].xuid.qwUserID = 0xCAFEBABE;
			pUsers[0].xuid.dwUserFlags =0;

			if (dwCurTitle >= MAX_TITLE_ID)
			   	dwCurTitle = 0;
			ChangeCurrentTitleId(rgdwTitleId[dwCurTitle]);

						ChangeCurrentTitleId(100);
			// Begin a new enumeration
			hr = XOnlineOfferingEnumerate(
						XONLINEOFFERING_ENUM_DEVICE_ONLINE,
						0, &enumparams,
						pbBuffer,
						cbBuffer,
						NULL,
						&hTask);

			DebugPrint("Enumerating content for title %u, %u records (hTask = %p)\n", 
						rgdwTitleId[dwCurTitle], dwMaxResults, hTask);
		    
	    	AssertSz(SUCCEEDED(hr), "Cannot start enumeration");
		    Assert(hTask != NULL);
		}

		// Continue if required
		if (hTask)
		{
			hr = XOnlineTaskContinue(hTask);
			if (XONLINETASK_STATUS_AVAILABLE(hr))
			{
				dwEnumerations++;
			
				// Enumerate complete ...
				hr = XOnlineOfferingEnumerateGetResults(
							hTask, 
							&rgpEnumInfo,
							&dwResults,
							&fPartial);
			    Assert(SUCCEEDED(hr));

				// Display the results
				DisplayResults(
							&uliIterations, dwEnumerations, 
							rgpEnumInfo, dwResults);

				if (!fPartial)
				{
					// Close the handle
					XOnlineTaskClose(hTask);
					hTask = NULL;

				    // Move on to next title
					dwCurTitle++;		

				}
			}
		}
	}

    DebugPrint("Unloading XOnline ...\n");
    XOnlineCleanup();

	// Break here
	_asm int 3;

	return;
}

