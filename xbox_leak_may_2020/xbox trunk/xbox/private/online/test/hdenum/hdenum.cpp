/*++

Copyright (c) Microsoft Corporation

Author:
    Keith Lau (keithlau)

Description:
	Unit test shell for asynchronous content enumeration

Module Name:

    hdenum.c

--*/

#include "xonp.h"
#include "testutil.h"
#include "testui.h"

VOID DisplayResults(
			ULARGE_INTEGER			*puliIterations,
			DWORD					dwEnumerations,
			PXONLINEOFFERING_INFO	*rgpEnumInfo,
			DWORD					dwResults,
			DWORD					dwBitFilter
			)
{
	WCHAR	wsz[2048];
	DWORD	i;
	SYSTEMTIME	st;

	BeginNewScene();

	wsprintf(wsz, L"%I64u Pump calls made\r\n"\
				L"Enumeration #%u returned %u results.\r\n \r\n"\
				L"Bitfilter = 0x%08x\r\n \r\n"\
				L"=============================================\r\n",
				*puliIterations, dwEnumerations, dwResults, dwBitFilter);
	Xputs(wsz);

	if (dwResults)
	{
		for (i = 0; i < dwResults; i++)
		{
			FileTimeToSystemTime(&(rgpEnumInfo[i]->ftActivationDate), &st);
			
			wsprintf(wsz, L"%u (0x%08x)   %08x\r\n",
						rgpEnumInfo[i]->OfferingId, rgpEnumInfo[i]->OfferingId,
						rgpEnumInfo[i]->dwBitFlags
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

//======================== The main function
void __cdecl main()
{
    HRESULT				hr;
    DWORD				dwError;

	XONLINETASK_HANDLE	hTask = NULL;
    BOOL				fPartial = FALSE;

	PBYTE				pbBuffer = NULL;
	DWORD				cbBuffer;
	DWORD				dwMaxResults;
	DWORD				dwResults;
    ULARGE_INTEGER		uliIterations = {0, 0};
	DWORD				dwEnumerations = 0;

	XONLINEOFFERING_ENUM_PARAMS	enumparams;
	PXONLINEOFFERING_INFO		*rgpEnumInfo;

	dwError = XOnlineStartup(NULL);
	if(dwError != S_OK)
    {
	    DebugPrint("Failed XOnlineStartup with %x\n", dwError);
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

	// Set up the enumeration params
	enumparams.dwOfferingType = 0;
	enumparams.dwBitFilter = 0;
	enumparams.dwMaxResults = 10;
	enumparams.dwDescriptionIndex = 0;

	// Allocate buffer to do work
	dwMaxResults = 10;
	cbBuffer = XOnlineOfferingEnumerateMaxSize(&enumparams, 0);
	pbBuffer = (PBYTE)LocalAlloc(LMEM_FIXED, cbBuffer);
	if (!pbBuffer)
	{
        DebugPrint("Cannot allocate memory (%u)\n", GetLastError());
        return;
	}

	// Loop downloading files
	while (1)
	{
		(uliIterations.QuadPart)++;

		// Start the task if completed
		if (!hTask)
		{
			enumparams.dwBitFilter = dwEnumerations;
			
			// Begin a new enumeration
			hr = XOnlineOfferingEnumerate(
						XONLINEOFFERING_ENUM_DEVICE_HD,
						0, &enumparams,
						pbBuffer,
						cbBuffer,
						NULL,
						&hTask);

	    	AssertSz(SUCCEEDED(hr), "Cannot start enumeration");
		    Assert(hTask != NULL);
		}

		// Continue if required
		if (hTask)
		{
			hr = XOnlineTaskContinue(hTask);
			if (XONLINETASK_STATUS_AVAILABLE(hr))
			{
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
							rgpEnumInfo, dwResults, 
							enumparams.dwBitFilter);

				if (!fPartial)
				{
					dwEnumerations++;
			
					// Close the handle
					XOnlineTaskClose(hTask);
					hTask = NULL;
				}
			}
		}
	}

	// Break here
	_asm int 3;

	return;
}

