/*++

Copyright (c) Microsoft Corporation

Author:
    Keith Lau (keithlau)

Description:
	Unit test shell for Online Autoupdate

Module Name:

    autoupdo.cpp

--*/

#include "xonp.h"
#include "testutil.h"
#include "testui.h"

DWORD					dwPumpCalls = 0;

const WCHAR	wszRebootMsg[] = 
			L"The current title has been successfully updated ONLINE.\n"
			L"Reboot your Xbox and run this app again, and if\n"
			L"this worked, you will see a message telling you\n"
			L"that Autoreboot worked for TITLE.\n";

WCHAR StatusBuffer[2400];

//======================== The main function
void __cdecl main()
{
    HRESULT				hr = S_OK;
    DWORD				dwError;
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

	// Just kick off the title update
	hr = XOnlineTitleUpdate(0);

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

