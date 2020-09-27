/*++

Copyright (c) Microsoft Corporation

Author:
    Keith Lau (keithlau)

Description:
	Module implementing DVD "download":
		DownloadFromDVD

	This is the "catch-all" function to do any "content download"
	kind of operation using a DVD as the source instead of going
	online.

Module Name:

    dvdload.c

--*/

#include "xonp.h"
#include "xonver.h"

//
// Define enumerated types that describe the states for each operation
//
typedef enum
{
	dvddlPreConnect = 0,
	dvddlEmulateHeaders,
	dvddlProcessData,
	dvddlPostDownload,
	dvddlCleanup,
	dvddlDone

} DVD_DOWNLOAD_STATES;

//
// ==============================================================
//   S T A T E   M A C H I N E   D E F I N I T I O N S
// ==============================================================
//

//
// Note we will reuse the XRL_ASYNC struct because this provides maximum 
// compatibility for the content download code. It also has everything
// we will need anyway.
//

// Define the state machines for XOnlineDVDDownload

//
// Pre "connect" extension state
//
HRESULT CXo::dvddlPreConnectHandler(
			PXRL_ASYNC	pxrlasync
			)
{
	HRESULT					hr = S_OK;
	PXRL_DOWNLOAD_EXTENSION	pxrldlext = &(pxrlasync->xrlext.dl);

    if (!pxrldlext->hTaskPreConnect)
    	goto SkipPre;

	// Simple logic: keep pumping the handle until we are told to 
	// stop, then move on to the connect phase
	hr = XOnlineTaskContinue(pxrldlext->hTaskPreConnect);
	if (XONLINETASK_STATUS_AVAILABLE(hr))
	{
		// The task had returned results, see if it succeeded
		if (FAILED(hr))
			goto Error;

		// See if the pre-connect task actually tells us to skip
		// the "downlaod" and go right to the post-process
		if (hr == XONLINE_S_XRL_EXTENSION_SKIP_TO_POST)
		{
			hr = S_OK;
			
			// see if there is any post-download work
			if ((pxrldlext->hTaskPostDownload != NULL) &&
				(pxrldlext->pfnInitializeTask != NULL))
			{
				hr = (this->*(pxrldlext->pfnInitializeTask))(
							xrldlextPostDownload, pxrlasync, 
							pxrldlext->hTaskPostDownload);
				if (FAILED(hr))
					goto Error;
			}

			// Got post download work, do that
			pxrlasync->dwCurrentState = dvddlPostDownload;

			goto Cleanup;
		}

SkipPre:
		// Set the next state to read and process data
		pxrlasync->dwCurrentState = dvddlEmulateHeaders;
	}

Cleanup:
	return(S_OK);

Error:
	// We encountered a hard error. Set next state to done.
	pxrlasync->hrFinal = hr;
	pxrlasync->dwCurrentState = dvddlCleanup;
	goto Cleanup;
}

//
// Emulate headers extension state
//
HRESULT CXo::dvddlEmulateHeadersHandler(
			PXRL_ASYNC	pxrlasync
			)
{
	HRESULT			hr = S_OK;
	DWORD			cbRead;
	FILETIME		ftZero = { 0, 0 };
	PXRL_DOWNLOAD_EXTENSION	pxrldlext = &(pxrlasync->xrlext.dl);

	// Straightforward "download" from scratch, emulate an HTTP 200
	pxrlasync->dwExtendedStatus = 200;

	// Now see if we have to deal with the resume stuff
	if (CompareFileTime(&(pxrlasync->ftLastModified), &ftZero) != 0)
	{
		// Now see if the file on DVD is older than our specified timestamp
		if (CompareFileTime(&(pxrlasync->ftLastModified), &pxrlasync->ftResponse) > 0)
		{
			// Timestamp newer than DVD file, resume
			pxrlasync->uliContentLength.QuadPart -= pxrlasync->uliFileSize.QuadPart;
			if (pxrlasync->uliContentLength.QuadPart > 0)
				pxrlasync->dwExtendedStatus = 206;
			else
				pxrlasync->dwExtendedStatus = 416;
		}
	}

	// Enulate a "Done headers" callback
	hr = (this->*(pxrldlext->pfnDoneHeaders))(pxrlasync);
	if (FAILED(hr))
		goto Error;

	// OK, start reading data from the file, synchronously  :-)
	if (!ReadFile(pxrlasync->fileio.hFile, 
				pxrlasync->pBuffer, 
				pxrlasync->cbBuffer, &cbRead, NULL))
	{
		HRESULT_FROM_WIN32(GetLastError());
		goto Error;
	}

	// Now massage the pointers
	pxrlasync->dwCurrent = cbRead;
	pxrlasync->uliTotalReceived.QuadPart += cbRead;

	// Make sure we are NOT in header mode
	pxrlasync->dwTemp = 0;

	// Initialize the process data subtask if there is an initialization
	// function
	if (pxrldlext->pfnInitializeTask)
	{
		hr = (this->*(pxrldlext->pfnInitializeTask))(
					xrldlextProcessData, pxrlasync, 
					pxrldlext->hTaskProcessData);
		if (FAILED(hr))
			goto Error;
	}

	// Process data
	pxrlasync->dwCurrentState = dvddlProcessData;

Cleanup:
	return(S_OK);

Error:
	// We encountered a hard error. Set next state to done.
	pxrlasync->hrFinal = hr;
	pxrlasync->dwCurrentState = dvddlCleanup;
	goto Cleanup;
}

//
// Process Data extension state
//
HRESULT CXo::dvddlProcessDataHandler(
			PXRL_ASYNC	pxrlasync
			)
{
	HRESULT					hr = S_OK;
	DWORD					cbRead;
	PXRL_DOWNLOAD_EXTENSION pxrldlext = &(pxrlasync->xrlext.dl);

    Assert(pxrldlext->hTaskProcessData != NULL);

	// Simple logic: keep pumping the handle until we are told to 
	// stop, then move on to the connect phase
	hr = XOnlineTaskContinue(pxrldlext->hTaskProcessData);
	if (XONLINETASK_STATUS_AVAILABLE(hr))
	{
		// The task had returned results, see if it succeeded
		if (FAILED(hr))
			goto Error;

		// See if we're done completely
		hr = (this->*(pxrldlext->pfnCompleted))(pxrlasync);
		if (FAILED(hr))
			goto Error;

		if (hr == S_OK)
		{
			// see if there is any post-download work
			if ((pxrldlext->hTaskPostDownload != NULL) &&
				(pxrldlext->pfnInitializeTask != NULL))
			{
				hr = (this->*(pxrldlext->pfnInitializeTask))(
							xrldlextPostDownload, 
							pxrlasync, pxrldlext->hTaskPostDownload);
				if (FAILED(hr))
					goto Error;
			}

			// Got post download work, do that
			pxrlasync->dwCurrentState = dvddlPostDownload;
		}
		else
		{
			// OK, we will need to read more data into the buffer, just read
			// as much as we can fit in the remaining bytes. Synchronously   :-)
			if (!ReadFile(pxrlasync->fileio.hFile, 
						pxrlasync->pBuffer + pxrlasync->dwCurrent, 
						pxrlasync->cbBuffer - pxrlasync->dwCurrent, &cbRead, NULL))
			{
				HRESULT_FROM_WIN32(GetLastError());
				goto Error;
			}

			// If we get zero bytes, then the pacakge is invalid
			Assert(cbRead > 0);
			if (cbRead == 0)
			{
				hr = HRESULT_FROM_WIN32(ERROR_INVALID_DATA);
				goto Error;
			}

			// Now massage the pointers
			pxrlasync->dwCurrent += cbRead;
			pxrlasync->uliTotalReceived.QuadPart += cbRead;

			// Stay in this state ...
		}
	}

Cleanup:
	return(S_OK);

Error:
	// We encountered a hard error. Set next state to done.
	pxrlasync->hrFinal = hr;
	pxrlasync->dwCurrentState = dvddlCleanup;
	goto Cleanup;
}

//
// Post "download" extension state
//
HRESULT CXo::dvddlPostDownloadHandler(
			PXRL_ASYNC	pxrlasync
			)
{
	HRESULT					hr = S_OK;
	PXRL_DOWNLOAD_EXTENSION pxrldlext = &(pxrlasync->xrlext.dl);

    if (!pxrldlext->hTaskPostDownload)
    	goto SkipPost;

	// Simple logic: keep pumping the handle until we are told to 
	// stop, then move on to the connect phase
	hr = XOnlineTaskContinue(pxrldlext->hTaskPostDownload);
	if (XONLINETASK_STATUS_AVAILABLE(hr))
	{
		// The task had returned results, see if it succeeded
		if (FAILED(hr))
			goto Error;

SkipPost:
		// Alright, we're all done, jump right to cleanup
		pxrlasync->dwCurrentState = dvddlCleanup;
	}

Cleanup:
	return(S_OK);

Error:
	// We encountered a hard error. Set next state to done.
	pxrlasync->hrFinal = hr;
	pxrlasync->dwCurrentState = dvddlCleanup;
	goto Cleanup;
}

//
// Cleanup
//
HRESULT CXo::dvddlCleanupHandler(PXRL_ASYNC pxrlasync)
{
	// Call the generic handler
	XRL_CleanupHandler(pxrlasync);
	
	// We are done
	pxrlasync->dwCurrentState = dvddlDone;

	return(S_OK);
}

//
// Array
// of handlers for DownloadFromDVD
//
const PFNXRL_HANDLER CXo::s_rgpfndvddlHandlers[] =
{
    CXo::dvddlPreConnectHandler,
	CXo::dvddlEmulateHeadersHandler,
	CXo::dvddlProcessDataHandler,
    CXo::dvddlPostDownloadHandler,
    CXo::dvddlCleanupHandler
};

//
// Implement the top-level do work function
//
HRESULT CXo::dvddlContinue(
			XONLINETASK_HANDLE	hTask
			)
{
	return(XRL_MainContinue(hTask, s_rgpfndvddlHandlers, dvddlDone));
}				

//
// Function to kick off "downloading" from DVD
//
HRESULT CXo::DownloadFromDVD(
			LPCSTR				szResourcePath,
			FILETIME			*pftModifiedSince, 
			LARGE_INTEGER		liResumeFrom,
			PXRL_ASYNC			pxrlasync
			)
{
	HRESULT					hr = S_OK;
	HANDLE					hFile = INVALID_HANDLE_VALUE;
	PXRL_DOWNLOAD_EXTENSION pxrldlext = &(pxrlasync->xrlext.dl);
	BY_HANDLE_FILE_INFORMATION bhfi;

    Assert(NULL != pxrlasync);
    Assert(NULL != szResourcePath);

    // Make sure the required callbacks are supplied
    Assert(NULL != pxrlasync->xrlext.dl.pfnDoneHeaders);
    Assert(NULL != pxrlasync->xrlext.dl.pfnProcessData);
    Assert(NULL != pxrlasync->xrlext.dl.pfnCompleted);
    Assert(NULL != pxrldlext->hTaskProcessData);

	// Modify nothing except fill in our work function
	pxrlasync->xontask.pfnContinue = dvddlContinue;
	pxrlasync->xontask.pfnClose = XRL_MainClose;

	// The socket API requires that we have an event handle in order
	// for the async I/O to happen. If the caller has not provided us
	// with an event, we have to create it here
	hr = XRL_CreateWorkEventIfNecessary(pxrlasync, NULL);
	if (FAILED(hr))
		goto Error;

	// Make sure the event is set
	SetEvent(pxrlasync->xontask.hEventWorkAvailable);

	// Mark as download
	pxrlasync->fDownload = TRUE;

	// Determine the next state depending on whether we have
	// any pre-connect work to do ...
	if ((pxrldlext->hTaskPreConnect != NULL) && 
		(pxrldlext->pfnInitializeTask != NULL))
	{
		hr = (this->*(pxrldlext->pfnInitializeTask))(
					xrldlextPreConnect, pxrlasync, 
					pxrldlext->hTaskPreConnect);
		if (FAILED(hr))
			goto Error;
	}

	// Next state is the pre-connect handler
	pxrlasync->dwCurrentState = dvddlPreConnect;

    // Build the request headers, save the request size (i.e. send
    // buffer size) in pxrlasync->wsabuf for transmission later
	pxrlasync->wsabuf.len = pxrlasync->cbBuffer;
	pxrlasync->wsabuf.buf = (char *)pxrlasync->pBuffer;

	// Now, open the source file for synchronous reading
	hFile = CreateFile(szResourcePath,
				GENERIC_READ, 
				FILE_SHARE_READ, 
				NULL, 
				OPEN_EXISTING, 
				FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, 
				NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		hr = HRESULT_FROM_WIN32(GetLastError());
		goto Error;
	}

	// Get file attributes
	if (!GetFileInformationByHandle(hFile, &bhfi))
	{
		hr = HRESULT_FROM_WIN32(GetLastError());
		goto Error;
	}

	// Save the file size
	pxrlasync->uliContentLength.u.LowPart = bhfi.nFileSizeLow;
	pxrlasync->uliContentLength.u.HighPart = bhfi.nFileSizeHigh;
	pxrlasync->uliTotalReceived.QuadPart = 0;

	if (pftModifiedSince)
	{
		// Well, try to emulate a resume, first check if the resume
		// point makes sense
		if ((pxrlasync->uliContentLength.QuadPart <= pxrlasync->uliFileSize.QuadPart))
		{
			hr = HRESULT_FROM_WIN32(ERROR_INVALID_PARAMETER);
			goto Error;
		}

		// Save the info for later
		pxrlasync->ftLastModified = *pftModifiedSince;
		pxrlasync->ftResponse = bhfi.ftCreationTime;
		pxrlasync->uliFileSize.QuadPart = (ULONGLONG)liResumeFrom.QuadPart;
	}
	else
	{
		// Don't try to resume
		ZeroMemory(&pxrlasync->ftLastModified, sizeof(FILETIME));
	}

	// Hand over the file handle to xrlasync
	pxrlasync->fileio.hFile = hFile;
	hFile = INVALID_HANDLE_VALUE;

    // Initialize the last state change
    pxrlasync->dwLastStateChange = GetTickCount();

Cleanup:	
	return(hr);

Error:
	if (hFile != INVALID_HANDLE_VALUE)
		CloseHandle(hFile);

	XRL_CleanupHandler(pxrlasync);
	goto Cleanup;
}



