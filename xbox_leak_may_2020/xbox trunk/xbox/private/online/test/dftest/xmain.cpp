/*++

Copyright (c) Microsoft Corporation

Author:
    Keith Lau (keithlau)

Description:
	Unit test shell for asynchronous XOnlineDownloadFile

Module Name:

    xmain.c

--*/

#include "xonp.h"
#include "testutil.h"
#include "testui.h"

#define DOWNLOAD_LOCATION		"/xboxfiles/%s"
#define LOCAL_PATH				"t:\\test\\%s"
#define REFERENCE_PATH			"t:\\ref\\%s"

#define MAX_DOWNLOAD_FILES		3

char *rgszFilenames[] =
{
	"wincrypt.h", "xmain.c", "xnetacc.c", "makefile", "asyncxrl.c"
};

DWORD rgdwFileSizes[] = 
{
	576501, 7616, 75968, 247, 56283
};

DWORD	dwPumpCalls = 0;

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
	CHAR				szPath[MAX_PATH];
	CHAR				szReference[MAX_PATH];
	CHAR				szDownloadLocation[MAX_PATH];
	CHAR				szTime1[MAX_TIME_SIZE];
	CHAR				szTime2[MAX_TIME_SIZE];
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

	PBYTE				pbBuffer1 = NULL;
	PBYTE				pbBuffer2 = NULL;

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

	pbBuffer1 = (PBYTE)LocalAlloc(LMEM_FIXED, 2048);
	if (!pbBuffer1)
	{
        DebugPrint("Cannot allocate memory (%u)\n", GetLastError());
        return;
	}
	pbBuffer2 = (PBYTE)LocalAlloc(LMEM_FIXED, 2048);
	if (!pbBuffer2)
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
			sprintf(szPath, LOCAL_PATH, rgszFilenames[dwCurTask]);
			sprintf(szDownloadLocation, DOWNLOAD_LOCATION, rgszFilenames[dwCurTask]);
			
		    hr = XOnlineDownloadFile(
		    			XONLINE_USER_ACCOUNT_SERVICE, 
		    			szDownloadLocation, NULL, 
		    			65536, (PBYTE) NULL, 0, 0, szPath, 
		    			0, hEventWorkAvailable, 
 		    			&(rghTask[dwCurTask]));

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
				// Download complete ...
				hr = XOnlineDownloadGetResults(rghTask[dwCurTask], NULL, 
							&cbBuffer, &uliTotalReceived,
							&uliContentLength,
							&dwStatus, &ftTime, &ftLastModified);
			    Assert(SUCCEEDED(hr));

				i = sizeof(szTime1); 
    			ConvertFileTimeAsString(szTime1, &i, &ftTime);
				i = sizeof(szTime2); 
    			ConvertFileTimeAsString(szTime2, &i, &ftLastModified);

				sprintf(szPath, LOCAL_PATH, rgszFilenames[dwCurTask]);
				sprintf(szReference, REFERENCE_PATH, rgszFilenames[dwCurTask]);

			    DebugPrint("[%3u] %s, %I64u bytes. Time: %s   Last modified: %s", 
			    			dwStatus, szPath, uliContentLength.QuadPart, szTime1, szTime2);

				// Make sure reference file and downloaded file match
				hr = HrFileCompare(szPath, szReference, pbBuffer1, pbBuffer2, 2048);

		    	AssertSz(hr == S_OK, "Files dont match");

				// Truncate file and redo
				if (dwIterations & 1)
				{
					hr = HrTruncateFile(szPath, rgdwFileSizes[dwCurTask] >> 1);
				}
				else
				{
					hr = S_OK;
					if (!DeleteFile(szPath))
						hr = HRESULT_FROM_WIN32(GetLastError());
				}
				
		    	AssertSz(SUCCEEDED(hr), "Cannot truncate/delete file");

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

