/*++

Copyright (c) Microsoft Corporation

Author:
    Keith Lau (keithlau)

Description:
	Unit test shell for asynchronous XOnlineContentInstall

Module Name:

    xmain.c

--*/

#include "xonp.h"
#include "testutil.h"
#include "testui.h"

//
// Enable this macro if we want to wait for event before pumping
//
//#define WAIT_FOR_EVENT

#define MAX_DOWNLOAD_FILES		4

XONLINEOFFERING_ID	rgOfferingId[] =
{
	100000, 100001, 100002, 100003
};

DWORD	g_dwTitleId = 100;

#define CDTEST_ALL_FILES		"\\*.*"

HRESULT HrFindFiles(
			XONLINEOFFERING_ID	OfferingId,
			DWORD				*pdwFiles
			)
{
	HRESULT	hr = S_OK;
	DWORD	dwTitleId;
	DWORD	cbPath;
	DWORD	dwFiles = 0;
	BOOL	fResult;
	BYTE	szPath[XONLINECONTENT_MAX_PATH];
	HANDLE	hFind = INVALID_HANDLE_VALUE;
	WIN32_FIND_DATA	wfd;

    dwTitleId = XeImageHeader()->Certificate->TitleID;

	cbPath = XONLINECONTENT_MAX_PATH;
	hr = XOnlineContentGetRootDirectory(
				OfferingId, FALSE,	// fUserData
				&cbPath, (CHAR *)szPath);
	if (FAILED(hr))
		return(hr);

	if ((cbPath + strlen(CDTEST_ALL_FILES)) >= XONLINECONTENT_MAX_PATH)
		return(HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER));

	strcpy((char *)szPath + cbPath, CDTEST_ALL_FILES);
	cbPath += strlen(CDTEST_ALL_FILES);

	hFind = FindFirstFile((LPCSTR)szPath, &wfd);
	if (hFind == INVALID_HANDLE_VALUE)
	{
		if ((GetLastError() == ERROR_FILE_NOT_FOUND) ||
			(GetLastError() == ERROR_PATH_NOT_FOUND))
		{
			*pdwFiles = 0;
			return(S_OK);
		}
		
		return(HRESULT_FROM_WIN32(GetLastError()));
	}
	fResult = TRUE;

	while (fResult)
	{
		if (wfd.cFileName[0] != '.')
			dwFiles++;

		fResult = FindNextFile(hFind, &wfd);
	}

	FindClose(hFind);

	if (GetLastError() != ERROR_NO_MORE_FILES)
	{
		FindClose(hFind);
		return(HRESULT_FROM_WIN32(GetLastError()));
	}

	FindClose(hFind);
	*pdwFiles = dwFiles;
	return(S_OK);
}

typedef enum
{
	INSTALL_DOWNLOAD = 0,
	INSTALL_VERIFY,
	INSTALL_REMOVE,
	INSTALL_MAX
	
} INSTALL_STATES;

const WCHAR *rgwszStates[INSTALL_MAX] =
{
	L"INSTALLING", L"VERIFYING", L"REMOVING"
};

DWORD					dwPumpCalls = 0;
DWORD					dwIterations = 0;
XONLINETASK_HANDLE		rghTask[MAX_DOWNLOAD_FILES];
INSTALL_STATES			rgInstallState[MAX_DOWNLOAD_FILES];

WCHAR StatusBuffer[2400];

VOID DisplayStatus(
			XONLINEOFFERING_ID	OfferingId,
			INSTALL_STATES		is,
			XONLINETASK_HANDLE	hTask
			)
{
	HRESULT			hr;
	ULARGE_INTEGER	ulin, ulid;
	DWORD			dwPercentDone;

	hr = XOnlineContentInstallGetProgress(hTask, 
				&dwPercentDone, &(ulin.QuadPart), &(ulid.QuadPart));

	BeginNewScene();

	wsprintf(StatusBuffer,
		L"%u pump calls made\r\n"\
		L"%u packages downloaded, verified and removed\r\n"\
		L"Content ID: %u\r\n"\
		L"Processing Status: %s\r\n",
		dwPumpCalls, dwIterations, OfferingId, rgwszStates[is]);
	Xputs(StatusBuffer);

	if (is ==INSTALL_DOWNLOAD)
	{
		wsprintf(StatusBuffer, 
				L"  Install progress %u percent (%I64u / %I64u)\r\n",
				dwPercentDone, ulin, ulid);
		Xputs(StatusBuffer);				
	}

	PresentScene();
}

void Initialize()
{
	DWORD i;

	for (i = 0; i < MAX_DOWNLOAD_FILES; i++)
	{
		rghTask[i] = NULL;
		rgInstallState[i] = INSTALL_DOWNLOAD;
	}
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

	ULARGE_INTEGER		uliContentLength;
	CHAR				szPath[MAX_PATH];
	CHAR				szReference[MAX_PATH];
	CHAR				szDownloadLocation[MAX_PATH];
	CHAR				szTime1[MAX_TIME_SIZE];
	CHAR				szTime2[MAX_TIME_SIZE];
	DWORD				dwInitialized;
    DWORD				dwCurTask = 0;
    DWORD				dwReason = 0;
    DWORD				dwError;
    DWORD				dwFiles;
    DWORD				dwSize;
    DWORD				i;

	DWORD				dwStatus;
	FILETIME			ftTime;
	FILETIME			ftLastModified;

	PBYTE				pbBuffer1 = NULL;
	PBYTE				pbBuffer2 = NULL;
    XONLINETASK_HANDLE	hLogon;

	XONLINE_STARTUP_PARAMS	xosp = { 0 };

	XNetStartupParams 	xnsp = { sizeof(XNetStartupParams), 
								XNET_STARTUP_BYPASS_SECURITY };

	// Set up the 'current' Title ID
	ChangeCurrentTitleId(g_dwTitleId);
	
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
        dwReason = 1;
        goto Error;
    }

	// Dump the size of the content download context for info
	DebugPrint("Size of context: %u bytes", 
				sizeof(XONLINETASK_CONTENT_DOWNLOAD));

#if 0
	// Initialize services
	hr = XOnlineLogonWrapper(g_rgServices, NUM_SERVICES, &hLogon);
	if (FAILED(hr))
	{
        DebugPrint("Cannot Logon (%08x), check c:\\xonline.ini\n", hr);
        RIP("Failed to initialize services");
        dwReason = 2;
        goto Error;
	}
#endif	

	// Initialize test specific stuff
	Initialize();

	// Initialize graphics stuff
	hr = InitializeGraphics();
	if (FAILED(hr))
	{
        DebugPrint("Cannot initialize graphics (%08x)\n", hr);
        RIP("Failed to initialize graphics");
        dwReason = 3;
        goto Error;
	}

	// Initialize text font
	hr = InitializeFont(0xffffffff, 0xff000000, XFONT_OPAQUE);
	if (FAILED(hr))
	{
        DebugPrint("Cannot initialize text (%08x)\n", hr);
        RIP("Failed to initialize text");
        dwReason = 4;
        goto Error;
	}

	// Loop downloading packages
	while (1)
	{
		// Start the task if completed
		if (!rghTask[dwCurTask])
		{
			switch (rgInstallState[dwCurTask])
			{
			case INSTALL_DOWNLOAD:

				// Install the content package
			    hr = XOnlineContentInstall(
			    			rgOfferingId[dwCurTask],
							NULL, 
	 		    			&(rghTask[dwCurTask]));

//			    DebugPrint("Installing %u  (hTask = %p)\n", 
//			    			rgOfferingId[dwCurTask], rghTask[dwCurTask]);
			    
		    	AssertSz(SUCCEEDED(hr), "Cannot start install");
			    Assert(rghTask[dwCurTask] != NULL);

			    if (FAILED(hr))
			    {
			    	dwReason = 100;
			    	goto Error;
			    }

				dwIterations++;
//				DebugPrint("Iteration #%u\n", dwIterations);
			    break;

			case INSTALL_VERIFY:
				// Verify the content
				dwSize = XBOX_HD_SECTOR_SIZE * 2;
				hr = XOnlineContentVerify(
			    			rgOfferingId[dwCurTask],
							NULL, &dwSize,
							NULL, 
	 		    			&(rghTask[dwCurTask]));
							
//			    DebugPrint("Verifying %u  (hTask = %p)\n", 
//			    			rgOfferingId[dwCurTask], rghTask[dwCurTask]);
			    
		    	AssertSz(SUCCEEDED(hr), "Cannot start verification");
			    Assert(rghTask[dwCurTask] != NULL);

			    if (FAILED(hr))
			    {
			    	dwReason = 110;
			    	goto Error;
			    }
			    break;
				
			case INSTALL_REMOVE:
				// Remove the content
				hr = XOnlineContentRemove(
			    			rgOfferingId[dwCurTask],
							NULL, 
	 		    			&(rghTask[dwCurTask]));
							
//			    DebugPrint("Removing %u  (hTask = %p)\n", 
//			    			rgOfferingId[dwCurTask], rghTask[dwCurTask]);
			    
		    	AssertSz(SUCCEEDED(hr), "Cannot start removal");
                Assert(rghTask[dwCurTask] != NULL);

			    if (FAILED(hr))
			    {
			    	dwReason = 120;
			    	goto Error;
			    }
			    break;

			default:
				RIP("INVALID State");
			}
		}

		// Display status every x pumps
		if ((dwPumpCalls & 0xff) == 0)
		{
			DisplayStatus(
						rgOfferingId[dwCurTask],
						rgInstallState[dwCurTask], 
						rghTask[dwCurTask]);
		}
		
		// Continue if required
		if (rghTask[dwCurTask])
		{
			hr = XOnlineTaskContinue(rghTask[dwCurTask]);
			if (XONLINETASK_STATUS_AVAILABLE(hr))
			{
				switch (rgInstallState[dwCurTask])
				{
				case INSTALL_DOWNLOAD:
				
					// Download complete ...
				    AssertSz(SUCCEEDED(hr), "Download");

				    if (FAILED(hr))
				    {
				    	dwReason = 200;
				    	goto Error;
				    }

				    hr = HrFindFiles(rgOfferingId[dwCurTask], &dwFiles);
				    Assert(SUCCEEDED(hr) && dwFiles);

//				    DebugPrint("Download got %u files\n", dwFiles);

				    rgInstallState[dwCurTask] = INSTALL_VERIFY;

				    if (FAILED(hr))
				    {
				    	dwReason = 201;
				    	goto Error;
				    }
				    break;

				case INSTALL_VERIFY:

				    if (FAILED(hr))
				    {
				    	dwReason = 210;
				    	goto Error;
				    }

					// Verification complete ...
				    Assert(SUCCEEDED(hr));
				    rgInstallState[dwCurTask] = INSTALL_REMOVE;
					break;

				case INSTALL_REMOVE:
				
				    if (FAILED(hr))
				    {
				    	dwReason = 220;
				    	goto Error;
				    }
				    	
					// Remove complete ...
				    Assert(SUCCEEDED(hr));

				    hr = HrFindFiles(rgOfferingId[dwCurTask], &dwFiles);
				    Assert(FAILED(hr) || !dwFiles);
				    hr = S_OK;

//				    DebugPrint("Remove left %u files\n", dwFiles);
					rgInstallState[dwCurTask] = INSTALL_DOWNLOAD;
				    break;

				default:
					RIP("INVALID State");
				}

				// Close the handle
				XOnlineTaskClose(rghTask[dwCurTask]);
				rghTask[dwCurTask] = NULL;

				if (rgInstallState[dwCurTask] == INSTALL_DOWNLOAD)
				{
					// Next Task
					dwCurTask++;
					if (dwCurTask >= MAX_DOWNLOAD_FILES)
						dwCurTask = 0;
				}
			}
		}

		dwPumpCalls++;
	}

Exit:

    DebugPrint("Shutting down XOnline...\n");
    XOnlineCleanup();

	return;

Error:
	DebugPrint("Error occurred: %08x at %u\n\n", hr, dwReason);
	goto Exit;
}


