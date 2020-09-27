/*++

Copyright (c) Microsoft Corporation

Author:
    Keith Lau (keithlau)

Description:
	Unit test shell for asynchronous XOnlineDownloadToMemory

Module Name:

    xmain.c

--*/

#include "xonp.h"
#include "testutil.h"
#include "testui.h"

#define DOWNLOAD_LOCATION		"/xboxfiles/%s"
#define REFERENCE_PATH			"t:\\ref\\%s"

#define MAX_DOWNLOAD_FILES		3

char *rgszFilenames[] =
{
	"asyncxrl.c", "makefile", "xmain.c", 
};

DWORD rgdwFileSizes[] = 
{
	56283, 247, 7616, 
};

#define NUM_SERVICES	1
DWORD			g_rgServices[NUM_SERVICES] =
{
	XONLINE_USER_ACCOUNT_SERVICE
};

//======================== The main function
void __cdecl main()
{
    HRESULT				hr;

	XONLINETASK_HANDLE	rghTask[] = { NULL, NULL, NULL, NULL, NULL };
	CHAR				szReference[MAX_PATH];
	CHAR				szDownloadLocation[MAX_PATH];
	CHAR				szTime1[MAX_PATH];
	CHAR				szTime2[MAX_PATH];
    DWORD				dwCurTask = 0;
    DWORD				dwIterations = 0;
    DWORD				i;

    WSADATA				WsaData;
	DWORD				dwError;

	DWORD				dwStatus;
	DWORD				cbBuffer;
	FILETIME			ftTime;
	FILETIME			ftLastModified;
	ULARGE_INTEGER		uliContentLength;
	ULARGE_INTEGER		uliTotalReceived;

	HANDLE				hEventWorkAvailable = NULL;

	PBYTE				pbBuffer = NULL;
	PBYTE				pbDownload = NULL;

	DWORD				dwDownloaded = 0;

	DWORD				dwInitialized;
    XONLINETASK_HANDLE	hLogon;

	XNetStartupParams	xnsp;

    _asm int 3;

    DebugPrint("Loading XBox network stack...\n");

    ZeroMemory(&xnsp, sizeof(XNetStartupParams));
    xnsp.cfgSizeOfStruct = sizeof(XNetStartupParams);
    xnsp.cfgFlags = XNET_STARTUP_BYPASS_SECURITY;

    dwError = XNetStartup(&xnsp);
    if (dwError != NO_ERROR)
    {
            DebugPrint("Failed XNetStartup with %d\n", dwError);
        return;
    }
	
	XONLINE_STARTUP_PARAMS	xosp = { 0 };
	
    DebugPrint("Calling XOnlineStartup...\n");
    hr = XOnlineStartup(&xosp);
    if (FAILED(hr))
    {
        DebugPrint("Error %08x returned by XOnlineStartup\n", hr);
    	_asm int 3;
        return;
    }

	pbBuffer = (PBYTE)LocalAlloc(LMEM_FIXED, 2048);
	if (!pbBuffer)
	{
        DebugPrint("Cannot allocate memory (%u)\n", GetLastError());
        return;
	}

	// Create the work event
	hEventWorkAvailable = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (hEventWorkAvailable == NULL)
	{
        DebugPrint("Cannot create work event (%u)\n", GetLastError());
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

	// Create out WriteFile thread
	// hThread = CreateThread(NULL, 0, ThreadProc, NULL, 0, NULL);

	// Loop downloading files
	while (1)
	{
		dwIterations++;

		// Start the task if completed
		if (!rghTask[dwCurTask])
		{
			// Download the local file
			sprintf(szDownloadLocation, DOWNLOAD_LOCATION, rgszFilenames[dwCurTask]);
			
		    hr = XOnlineDownloadToMemory(
		    			XONLINE_USER_ACCOUNT_SERVICE, 
		    			szDownloadLocation, NULL, 65536, 
		    			(PBYTE) NULL, 0,
		    			0, hEventWorkAvailable, &(rghTask[dwCurTask]));

		    DebugPrint("Downloading %s  (hTask = %p)\n", 
		    			szDownloadLocation, rghTask[dwCurTask]);
		    
	    	AssertSz(SUCCEEDED(hr), "Cannot start download");
		    Assert(rghTask[dwCurTask] != NULL);
		}

		// Continue if required
		if (rghTask[dwCurTask])
		{
			hr = XOnlineTaskContinue(rghTask[dwCurTask]);
			if (XONLINETASK_STATUS_AVAILABLE(hr))
			{
				dwDownloaded++;
			
				// Download complete ...
				hr = XOnlineDownloadGetResults(rghTask[dwCurTask], &pbDownload, 
							&cbBuffer, &uliTotalReceived,
							&uliContentLength,
							&dwStatus, &ftTime, &ftLastModified);
			    Assert(SUCCEEDED(hr));

				i = sizeof(szTime1); 
    			ConvertFileTimeAsString(szTime1, &i, &ftTime);
				i = sizeof(szTime2); 
    			ConvertFileTimeAsString(szTime2, &i, &ftLastModified);

				sprintf(szReference, REFERENCE_PATH, rgszFilenames[dwCurTask]);

			    DebugPrint("[%3u] %s, %I64u bytes. Time: %s   Last modified: %s", 
			    			dwStatus, rgszFilenames[dwCurTask], uliContentLength.QuadPart, 
			    			szTime1, szTime2);

				// Make sure reference file and downloaded file match
				hr = HrFileCompareWithBuffer(szReference, pbBuffer, 2048, 
							pbDownload, uliContentLength.u.LowPart);

		    	AssertSz(hr == S_OK, "Files dont match");

				// Close the handle
				XOnlineTaskClose(rghTask[dwCurTask]);
				rghTask[dwCurTask] = NULL;
			}
		}

		// Next Task
		dwCurTask++;
		if (dwCurTask >= MAX_DOWNLOAD_FILES)
			dwCurTask = 0;
	}

    DebugPrint("Unloading XOnline ...\n");
    XOnlineCleanup();
 
	// Break here
	_asm int 3;

	return;
}

