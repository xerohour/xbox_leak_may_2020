/*++

Copyright (c) Microsoft Corporation

Author:
    Keith Lau (keithlau)

Description:
	Unit test shell for DVD Title Update

Module Name:

    DVDTitleUpdate.cpp

--*/

#include "xonp.h"
#include "testutil.h"
#include "testui.h"

#ifdef DASH_UPDATE

#include "dash\package\symmkey.c"

const WCHAR	wszRebootMsg[] = 
			L"The Dashboard has been successfully updated from DVD.\n"
			L"Reboot your Xbox into the dashboard, and if\n"
			L"this worked, you will see a message telling you\n"
			L"that Autoreboot worked for DASH.\n";

#else

#include "title\package\symmkey.c"

const WCHAR	wszRebootMsg[] = 
			L"The current title has been successfully updated from DVD.\n"
			L"Reboot your Xbox and run this app again, and if\n"
			L"this worked, you will see a message telling you\n"
			L"that Autoreboot worked for TITLE.\n";

#endif

DWORD dwPumpCalls = 0;

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

#if 0 
//
// Implement verification routines to make sure the update happened
// correctly
//
HRESULT AutoupdCheckDirsDoWork(
			XONLINETASK_OP		op,
			XONLINETASK_HANDLE	hTask, 
			DWORD				dwMaxExecutionTimeSlice,
			DWORD				*pdwWorkFlags
			)
{
	HRESULT					hr = S_OK;
	PXONLINETASK_DIRCOPY	pdircopy;

	Assert(op == XONLINETASK_DOWORK);
	Assert(hTask != NULL);

	// Locate the containing object
	pdircopy = CONTAINING_RECORD(hTask, XONLINETASK_DIRCOPY, xontaskOnDir);

	// All we do is create the directory
	if (!CreateDirectory(pdircopy->dircrawl.szTarget, NULL))
		hr = HRESULT_FROM_WIN32(GetLastError());

	XONLINETASK_WORKFLAGS_SET_DONT_CONTINUE(pdwWorkFlags);
	return(hr);
}

//
// Simple per-file task handler to do a directory copy
//
HRESULT AutoupdCheckFilesDoWork(
			XONLINETASK_OP		op,
			XONLINETASK_HANDLE	hTask, 
			DWORD				dwMaxExecutionTimeSlice,
			DWORD				*pdwWorkFlags
			)
{
	HRESULT					hr = S_OK;
	PXONLINETASK_DIRCOPY	pdircopy;
	PXONLINETASK_DIRCRAWL	pdircrawl;
	ULARGE_INTEGER			uliSize;
	DWORD					cbSize;
	DWORD					cbTarget;

	Assert(op == XONLINETASK_DOWORK);
	Assert(hTask != NULL);

	// Locate the containing object
	pdircopy = CONTAINING_RECORD(hTask, XONLINETASK_DIRCOPY, xontaskOnFile);
	pdircrawl = &pdircopy->dircrawl;

	// Build the source and target paths for copy, making sure not to overrun
	// the buffers
	pdircrawl = &pdircopy->dircrawl;
	cbSize = strlen(pdircrawl->wfd.cFileName);
	Assert(pdircrawl->szTarget != NULL);
    Assert((pdircrawl->szPath + cbSize) < pdircrawl->cbMax);
    Assert((pdircrawl->szTarget + cbSize) < pdircrawl->cbMax);

	// Build the fill copy paths
	cbSize = pdircrawl->cbPath;
	cbTarget = pdircrawl->cbTarget;
	strcpy(pdircrawl->szPath + cbSize, pdircrawl->wfd.cFileName);
	strcpy(pdircrawl->szTarget + cbTarget, pdircrawl->wfd.cFileName);

	// All we do is create the directory
	if (!CopyFile(pdircrawl->szPath, pdircrawl->szTarget, FALSE))
		hr = HRESULT_FROM_WIN32(GetLastError());
	else
	{
		// Update the statistics
		(pdircopy->dwFilesCopied)++;
		uliSize.u.LowPart = pdircrawl->wfd.nFileSizeLow;
		uliSize.u.HighPart = pdircrawl->wfd.nFileSizeHigh;
		pdircopy->uliBytesCopied.QuadPart += uliSize.QuadPart;
	}

	// Restore the path strings
	pdircrawl->cbPath = cbSize;
	pdircrawl->szPath[cbSize] = '\0';
	pdircrawl->cbTarget = cbTarget;
	pdircrawl->szTarget[cbTarget] = '\0';

	XONLINETASK_WORKFLAGS_SET_DONT_CONTINUE(pdwWorkFlags);
	return(hr);
}

#endif

#define NUM_SERVICES	1
DWORD			g_rgServices[NUM_SERVICES] =
{
	XONLINE_AUTO_UPDATE_SERVICE
};

//======================== The main function
void __cdecl main()
{
    HRESULT				hr = S_OK;
    DWORD				dwTitleId;
    DWORD				dwError;
    DWORD				dwLastStatusUpdate = 0;
    HANDLE				hEvent = NULL;
	XONLINETASK_HANDLE	hTask = NULL;
    XONLINETASK_HANDLE	hLogon = NULL;
    WCHAR				wszReboot[MAX_PATH];
    CHAR				szPath[MAX_PATH];

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

	sprintf(szPath, "d:\\$u\\%08x\\update.xcp", dwTitleId);

	// Just kick off the title update
	hr = XOnlineTitleUpdateFromDVDInternal(
				szPath,
				dwTitleId,
				dwVersion, 
				rgbTitleKey,
				(PBYTE)rgbSymmetricKey,
				sizeof(rgbSymmetricKey),
				g_rgbPublicKey,
				g_cbPublicKey,
				hEvent, &hTask);
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

