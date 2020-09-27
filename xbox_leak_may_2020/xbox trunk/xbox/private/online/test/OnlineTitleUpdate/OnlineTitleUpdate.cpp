/*++

Copyright (c) Microsoft Corporation

Author:
    Keith Lau (keithlau)

Description:
	Unit test shell for Online Autoupdate

Module Name:

    DVDTitleUpdate.cpp

--*/

#include "xonp.h"
#include "testutil.h"
#include "testui.h"

#ifdef DASH_UPDATE

#include "Dash\package\symmkey.c"

const WCHAR	wszRebootMsg[] = 
			L"The current Dashboard has been successfully updated ONLINE.\n"
			L"Reboot your Xbox into the Dashboard, and if\n"
			L"this worked, you will see a message telling you\n"
			L"that Autoreboot worked for DASH.\n";

#else

#include "Title\package\symmkey.c"

const WCHAR	wszRebootMsg[] = 
			L"The current title has been successfully updated ONLINE.\n"
			L"Reboot your Xbox and run this app again, and if\n"
			L"this worked, you will see a message telling you\n"
			L"that Autoreboot worked for TITLE.\n";

#endif

DWORD					dwPumpCalls = 0;

WCHAR StatusBuffer[2400];

VOID DisplayStatus(
			DWORD				dwTitleId,
			XONLINETASK_HANDLE	hTask
			)
{
	HRESULT			hr;
	ULONGLONG		qwn, qwd;
	DWORD			dwPercentDone;

	hr = XOnlineTitleUpdateGetProgress(hTask, 
		&dwPercentDone, &qwn, &qwd);

	BeginNewScene();

	wsprintf(StatusBuffer,
		L"%u pump calls made\r\n"\
		L"Title ID: %u (%08x)\r\n",
		dwPumpCalls, dwTitleId, dwTitleId);
	Xputs(StatusBuffer);

	wsprintf(StatusBuffer, 
			L"  Autoupdate progress %u percent (%I64u / %I64u)\r\n",
			dwPercentDone, qwn, qwd);
	Xputs(StatusBuffer);				

	PresentScene();
}

#define NUM_SERVICES	1
DWORD			g_rgServices[NUM_SERVICES] =
{
	XONLINE_AUTO_UPDATE_SERVICE
};

//======================== The main function
void __cdecl main()
{
    HRESULT				hr = S_OK;
    DWORD				dwError;
    DWORD				dwTitleId;
    DWORD				dwLastStatusUpdate = 0;
    HANDLE				hEvent = NULL;
	XONLINETASK_HANDLE	hTask = NULL;
    XONLINETASK_HANDLE	hLogon = NULL;
    WCHAR				wszReboot[MAX_PATH];

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

	// Dump the size of the content download context for info
	// DebugPrint("Size of context: %u bytes", sizeof(XONLINETASK_AUTOUPD_ONLINE));

	hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	// Initialize graphics stuff
	hr = InitializeGraphics();
	if (FAILED(hr))
	{
        DebugPrint("Cannot initialize graphics (%08x)\n", hr);
        RIP("Failed to initialize graphics");
        return;
	}

	// Initialize text font
	hr = InitializeFont(0xffffffff, 0xff000000, XFONT_OPAQUE);
	if (FAILED(hr))
	{
        DebugPrint("Cannot initialize text (%08x)\n", hr);
        RIP("Failed to initialize text");
        return;
	}

#if 0
	// Initialize services
	hr = XOnlineLogonWrapper(g_rgServices, NUM_SERVICES, &hLogon);
	if (FAILED(hr))
	{
        DebugPrint("Cannot Logon (%08x), check c:\\xonline.ini\n", hr);
        RIP("Failed to initialize services");
        return;
	}
#endif	

	DWORD	dwVersion;
	DWORD	cbTitleKey;
	BYTE	rgbTitleKey[XONLINE_KEY_LENGTH];
	
	dwTitleId = XeImageHeader()->Certificate->TitleID;
	dwVersion = XeImageHeader()->Certificate->Version;
	
	cbTitleKey = XONLINE_KEY_LENGTH;
	hr = XoUpdateGetRawTitleKey(dwTitleId, rgbTitleKey, &cbTitleKey);
	if (FAILED(hr))
	if (FAILED(hr))
	{
		_asm int 3;
	}

	// Just kick off the title update
	hr = XOnlineTitleUpdateInternal(
				dwTitleId, dwVersion, rgbTitleKey, hEvent, &hTask);
	if (FAILED(hr))
	{
		_asm int 3;
	}

	dwLastStatusUpdate = GetTickCount();
	while (1)
	{
		hr = XOnlineTaskContinue(hTask);
		if (XONLINETASK_STATUS_AVAILABLE(hr))
		{
			if (FAILED(hr))
			{
				_asm int 3;
			}

			// Branch out here to tell the user to reboot
			goto PromptReboot;
		}
		
		dwPumpCalls++;

		// Display status every x pumps
		if ((GetTickCount() - dwLastStatusUpdate) > 100)
		{
			DisplayStatus(dwTitleId, hTask);
			dwLastStatusUpdate = GetTickCount();
		}
	}

PromptReboot:

	BeginNewScene();

	wcscpy((WCHAR *)wszReboot, (WCHAR *)wszRebootMsg);
	Xputs(wszReboot);

	PresentScene();

	while (1)
		Sleep(1000);

	// Break here
	_asm int 3;

	return;
}


