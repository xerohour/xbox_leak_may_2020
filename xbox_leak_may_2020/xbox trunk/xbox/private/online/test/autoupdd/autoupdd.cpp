/*++

Copyright (c) Microsoft Corporation

Author:
    Keith Lau (keithlau)

Description:
	Unit test shell for Autoupdate from DVD

Module Name:

    autouppd.cpp

--*/

#include "xonp.h"
#include "testutil.h"
#include "testui.h"

class CXoTest : public CXo
{
public:

    void DisplayStatus(DWORD dwTitleId, XONLINETASK_HANDLE hTask);
    void Test();

};

DWORD					dwPumpCalls = 0;
DWORD					dwIterations = 0;
DWORD					dwCurTitle = 0;

#define MAX_TITLES				4

DWORD	rgdwTitleIds[MAX_TITLES] =
{
	0, 1, 2, 3
};

const WCHAR	wszRebootMsg[] = 
			L"The current title has been successfully updated.\n"
			L"Reboot your Xbox and run this app again, and if\n"
			L"this worked, you will see a message telling you\n"
			L"that Autoreboot worked for TITLE.\n";

WCHAR StatusBuffer[2400];

VOID CXoTest::DisplayStatus(
			DWORD				dwTitleId,
			XONLINETASK_HANDLE	hTask
			)
{
	HRESULT			hr;
	ULARGE_INTEGER	ulin, ulid;
	DWORD			dwPercentDone;

	hr = UpdateFromDVDGetProgress(hTask, &dwPercentDone, &ulin, &ulid);

	BeginNewScene();

	wsprintf(StatusBuffer,
		L"%u pump calls made\r\n"\
		L"%u DVD title Autoupdates performed\r\n"\
		L"Title ID: %u (%08x)\r\n",
		dwPumpCalls, dwIterations, dwTitleId, dwTitleId);
	Xputs(StatusBuffer);

	wsprintf(StatusBuffer, 
			L"  Autoupdate progress %u percent (%I64u / %I64u)\r\n",
			dwPercentDone, ulin, ulid);
	Xputs(StatusBuffer);				

	PresentScene();
}

//
// Implement verification routines to make sure the update happened
// correctly
//
#if 0
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

//======================== The main function
void __cdecl main()
{
    CXoTest XoTest;
    XoTest.Test();
}

void CXoTest::Test()
{
    HRESULT				hr = S_OK;
    HANDLE				hEvent = NULL;
	XONLINETASK_HANDLE	hTask = NULL;
    WCHAR				wszReboot[MAX_PATH];

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

	while (1)
	{
		// Set the title ID
		ChangeCurrentTitleId(rgdwTitleIds[dwCurTitle]);

		// Kick off a title DVD autoupdate ... as simple as that!
		hr = UpdateTitleFromDVD(
					rgdwTitleIds[dwCurTitle], 1, 0, hEvent, &hTask);
		if (FAILED(hr))
		{
			_asm int 3;
		}

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

				// Switch to other title
				dwCurTitle++;
				if (dwCurTitle >= MAX_TITLES)
					dwCurTitle = 0;
				break;
			}
			
			dwPumpCalls++;

			// Display status every x pumps
			DisplayStatus(rgdwTitleIds[dwCurTitle], hTask);
		}

		dwIterations++;
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

