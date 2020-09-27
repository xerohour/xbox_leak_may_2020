/*++

Copyright (c) Microsoft Corporation

Author:
    Keith Lau (keithlau)

Description:
	Unit test shell for asynchronous XOnlineUploadFromMemory

Module Name:

    xmain.c

--*/

#include "xonp.h"
#include "testui.h"
#include "testutil.h"

//
// Enable this macro if we want to wait for event before pumping
//
#define WAIT_FOR_EVENT

#define UPLOAD_LOCATION			"/xboxupload/xrl.xbp?loc=/xboxupload/%s"
#define DOWNLOAD_LOCATION		"/xboxupload/%s"
#define LOCAL_PATH				"t:\\test\\%s"
#define REFERENCE_PATH			"t:\\ref\\%s"

#define MAX_UPLOAD_FILES		1

char *rgszFilenames[] =
{
	"makefile", "xmain.c", "wincrypt.h", "xnetacc.c", "asyncxrl.c"
};

DWORD rgdwFileSizes[] = 
{
	247, 7616, 576501, 75968, 56283
};

typedef enum 
{
	STATE_UPLOAD = 0,
	STATE_DOWNLOAD
	
} UF_STATES;

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
	DWORD				rgdwState[] = { STATE_UPLOAD, STATE_UPLOAD, 
								STATE_UPLOAD, STATE_UPLOAD, STATE_UPLOAD };
	PBYTE				rgpbBuffers[] = { NULL, NULL, NULL, NULL, NULL };
	DWORD				rgcbBuffers[] = { 0, 0, 0, 0, 0 };
	CHAR				szPath[MAX_PATH];
	CHAR				szReference[MAX_PATH];
	CHAR				szLocation[MAX_PATH];
	CHAR				szTime1[MAX_TIME_SIZE];
	CHAR				szTime2[MAX_TIME_SIZE];
    DWORD				dwCurTask = 0;
    DWORD				dwIterations = 0;
    DWORD				cbBuffer = 1024;
    DWORD				i;

    WSADATA				WsaData;
	DWORD				dwError;

	DWORD				dwStatus;
	FILETIME			ftTime;
	FILETIME			ftLastModified;

	HANDLE				hEventWorkAvailable = NULL;

	PBYTE				pbBuffer1 = NULL;
	PBYTE				pbBuffer2 = NULL;

	PBYTE				pbUpload = NULL;
	DWORD				cbUpload;
	ULARGE_INTEGER		uliTotalReceived;
	ULARGE_INTEGER		uliContentLength;

	DWORD				dwInitialized;
    XONLINETASK_HANDLE	hLogon;

	XNetStartupParams	xnsp;

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

	for (i = 0; i < MAX_UPLOAD_FILES; i++)
	{
		sprintf(szReference, REFERENCE_PATH, rgszFilenames[i]);
		if (!SUCCEEDED(HrAllocateAndReadFile(szReference, &(rgpbBuffers[i]), &(rgcbBuffers[i]))))
		{
			AssertSz(FALSE, "Cannot read files");
			return;
		}
	}

	// Loop downloading files
	while (1)
	{
		dwIterations++;

		// Start the task if completed
		if (!rghTask[dwCurTask])
		{
			if (rgdwState[dwCurTask] == STATE_UPLOAD)
			{
				// Upload to server
				sprintf(szReference, REFERENCE_PATH, rgszFilenames[dwCurTask]);
				sprintf(szLocation, UPLOAD_LOCATION, rgszFilenames[dwCurTask]);
				
			    hr = XOnlineUploadFromMemory(
			    			XONLINE_USER_ACCOUNT_SERVICE, 
			    			szLocation, NULL, 
			    			&cbBuffer, (PBYTE) NULL, 0,
			    			rgpbBuffers[dwCurTask], rgcbBuffers[dwCurTask],
			    			0, hEventWorkAvailable, 
	 		    			&(rghTask[dwCurTask]));

			    DebugPrint("Uploading %s  (hTask = %p)\n", 
			    			szLocation, rghTask[dwCurTask]);
			    
		    	AssertSz(SUCCEEDED(hr), "Cannot start upload");
			    Assert(rghTask[dwCurTask] != NULL);
			}
			else
			{
				// Download the local file
				sprintf(szPath, LOCAL_PATH, rgszFilenames[dwCurTask]);
				sprintf(szLocation, DOWNLOAD_LOCATION, rgszFilenames[dwCurTask]);
				
			    hr = XOnlineDownloadFile(
			    			XONLINE_USER_ACCOUNT_SERVICE, 
			    			szLocation, NULL, 
			    			65536, (PBYTE) NULL, 0,
			    			0, szPath, 0, hEventWorkAvailable, 
	 		    			&(rghTask[dwCurTask]));

			    DebugPrint("Downloading %s  (hTask = %p)\n", 
			    			szLocation, rghTask[dwCurTask]);
			    
		    	AssertSz(SUCCEEDED(hr), "Cannot start download");
			    Assert(rghTask[dwCurTask] != NULL);
			}
		}

		// Continue if required
		if (rghTask[dwCurTask])
		{
#ifdef WAIT_FOR_EVENT
			if (WaitForSingleObject(hEventWorkAvailable, INFINITE) != WAIT_OBJECT_0)
			{
				// Wha?
				AssertSz(FALSE, "Failed to wait for event?");
			}
#endif

			hr = XOnlineTaskContinue(rghTask[dwCurTask]);
			if (XONLINETASK_STATUS_AVAILABLE(hr))
			{
				if (rgdwState[dwCurTask] == STATE_UPLOAD)
				{
					// Upload complete ...
					hr = XOnlineUploadGetResults(rghTask[dwCurTask], 
								&pbUpload, &cbUpload,
								&uliTotalReceived, &uliContentLength,
								&dwStatus, &ftTime);
				    Assert(SUCCEEDED(hr));
				    Assert(cbUpload == uliContentLength.QuadPart);

					i = sizeof(szTime1); 
	    			ConvertFileTimeAsString(szTime1, &i, &ftTime);

				    DebugPrint("[%3u] %s Time: %s  Response: %I64u bytes", 
				    			dwStatus, rgszFilenames[dwCurTask], szTime1, 
				    			uliContentLength.QuadPart);
				    
			    	// Set the next state to download
			    	rgdwState[dwCurTask] = STATE_DOWNLOAD;
				}
				else
				{
					// Download complete ...
					hr = XOnlineDownloadGetResults(rghTask[dwCurTask], 
								NULL, NULL, NULL, NULL,
								&dwStatus, &ftTime, &ftLastModified);
				    Assert(SUCCEEDED(hr));

					i = sizeof(szTime1); 
	    			ConvertFileTimeAsString(szTime1, &i, &ftTime);
					i = sizeof(szTime2); 
	    			ConvertFileTimeAsString(szTime2, &i, &ftLastModified);

					sprintf(szPath, LOCAL_PATH, rgszFilenames[dwCurTask]);
					sprintf(szReference, REFERENCE_PATH, rgszFilenames[dwCurTask]);

				    DebugPrint("[%3u] %s Time: %s   Last modified: %s", 
				    			dwStatus, szPath, szTime1, szTime2);

					// Make sure reference file and downloaded file match
					hr = HrFileCompare(szPath, szReference, pbBuffer1, pbBuffer2, 2048);

			    	AssertSz(hr == S_OK, "Files dont match");

					if (!DeleteFile(szPath))
						hr = HRESULT_FROM_WIN32(GetLastError());
					
			    	AssertSz(SUCCEEDED(hr), "Cannot truncate/delete file");

			    	// Set the next state back to upload
			    	rgdwState[dwCurTask] = STATE_UPLOAD;
			    }

				// Close the handle
				XOnlineTaskClose(rghTask[dwCurTask]);
				rghTask[dwCurTask] = NULL;
			}
		}

		// Next Task
		dwCurTask++;
		if (dwCurTask >= MAX_UPLOAD_FILES)
			dwCurTask = 0;
	}

    DebugPrint("Unloading XOnline ...\n");
    XOnlineCleanup();

	// Break here
	_asm int 3;

	return;
}

