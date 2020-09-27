/*++

Copyright (c) Microsoft Corporation

Author:
    Keith Lau (keithlau)

Description:
	Module implementing XRL download APIs:
		XOnlineDownloadToMemory
		XOnlineDownloadFile

Module Name:

    download.c

--*/

#include "xonp.h"
#include "xonver.h"

//
// Define enumerated types that describe the states for each operation
//
typedef enum
{
	xrldlPreConnect = 0,
	xrldlConnect,
	xrldlSend,
	xrldlReceive,
	xrldlProcessData,
	xrldlPostDownload,
	xrldlCleanup,
	xrldlDone

} XON_DOWNLOAD_STATES;

typedef enum
{
	xrldfGrowFile,
	xrldfWriteFile, 
	xrldfDone

} XON_DOWNLOAD_FILE_STATES;

//
// ==============================================================
//   S T A T E   M A C H I N E   D E F I N I T I O N S
// ==============================================================
//

// Define the state machines for XOnlineDownload

//
// State 0: Pre-connect extension state
//
HRESULT CXo::xrldlPreConnectHandler(
			PXRL_ASYNC	pxrlasync
			)
{
	HRESULT					hr = S_OK;
	PXRL_DOWNLOAD_EXTENSION	pxrldlext = &(pxrlasync->xrlext.dl);

    Assert(pxrldlext->hTaskPreConnect != NULL);

	// Simple logic: keep pumping the handle until we are told to 
	// stop, then move on to the connect phase
	hr = XOnlineTaskContinue(
				pxrldlext->hTaskPreConnect);
	if (XONLINETASK_STATUS_AVAILABLE(hr))
	{
		// The task had returned results, see if it succeeded
		if (FAILED(hr))
			goto Error;

		// See if the pre-connect task actually tells us to skip
		// the wire talk and skip right to the post-process
		if (hr == XONLINE_S_XRL_EXTENSION_SKIP_TO_POST)
		{
			hr = S_OK;
			
			// see if there is any post-download work
			if (pxrldlext->hTaskPostDownload)
			{
				// Make sure we initialize the task using the supplied callback
				if (pxrldlext->pfnInitializeTask)
				{
					hr = (this->*(pxrldlext->pfnInitializeTask))(
								xrldlextPostDownload, pxrlasync, 
								pxrldlext->hTaskPostDownload);
					if (FAILED(hr))
						goto Error;
				}

				// Got post download work, do that
				pxrlasync->dwCurrentState = xrldlPostDownload;
			}
			else
			{
				// Nope, jump right to cleanup
				pxrlasync->dwCurrentState = xrldlCleanup;
			}

			goto Cleanup;
		}

	    // Connect to the service asynchronously
	    hr = XRL_AsyncConnect(pxrlasync);
	    if (FAILED(hr))
    	    goto Error;

		// Set the next state
		pxrlasync->dwCurrentState = xrldlConnect;
	}

Cleanup:
	return(S_OK);

Error:
	// We encountered a hard error. Set next state to done.
	pxrlasync->hrFinal = hr;
	pxrlasync->dwCurrentState = xrldlCleanup;
	goto Cleanup;
}

//
// State 1: wait for connect loop
//
HRESULT CXo::xrldlConnectHandler(
			PXRL_ASYNC	pxrlasync
			)
{
	HRESULT				hr = S_OK;
	XONLINETASK_HANDLE	hsockio = (XONLINETASK_HANDLE)&(pxrlasync->sockio);

	// Continue until the connect completes asynchronously
	hr = XOnlineTaskContinue(hsockio);
	if (XONLINETASK_STATUS_AVAILABLE(hr))
	{
		// Connect had returned results, see if it succeeded
		if (FAILED(hr))
			goto Error;

		// Succeeded, now we can kick off sending the request. Note that wsabuf
		// was already set up during XRL_SetupDownload time.
		SendRecvInitializeContext(pxrlasync->sockio.socket,
					&pxrlasync->wsabuf, 1, 0, 
					pxrlasync->xontask.hEventWorkAvailable, &pxrlasync->sockio);

		hr = SendInternal(&pxrlasync->sockio);
		if (FAILED(hr))
			goto Error;

		// Set the next state
		pxrlasync->dwCurrentState = xrldlSend;
	}

Cleanup:
	return(S_OK);

Error:
	// We encountered a hard error. Set next state to done.
	pxrlasync->hrFinal = hr;
	pxrlasync->dwCurrentState = xrldlCleanup;
	goto Cleanup;
}

//
// State 2: send request loop
//
HRESULT CXo::xrldlSendHandler(PXRL_ASYNC pxrlasync)
{
	HRESULT				hr = S_OK;
	XONLINETASK_HANDLE	hsockio = (XONLINETASK_HANDLE)&(pxrlasync->sockio);

	// Continue until the send completes asynchronously
	hr = XOnlineTaskContinue(hsockio);
	if (XONLINETASK_STATUS_AVAILABLE(hr))
	{
		// Send had returned results, see if it succeeded
		if (FAILED(hr))
			goto Error;

		// Succeeded, now we can start receiving data from the socket.
		pxrlasync->wsabuf.len = pxrlasync->cbBuffer;
		SendRecvInitializeContext(pxrlasync->sockio.socket,
					&pxrlasync->wsabuf, 1, 0, 
					pxrlasync->xontask.hEventWorkAvailable, &pxrlasync->sockio);

		hr = RecvInternal(&pxrlasync->sockio);
		if (FAILED(hr))
			goto Error;

		// Set the next state to receive, and initialize some variables
		pxrlasync->dwCurrentState = xrldlReceive;
		pxrlasync->dwCurrent = 0;
		pxrlasync->dwTemp = XRL_FLAG_HEADER_MODE;
	}

Cleanup:
	return(S_OK);

Error:
	// We encountered a hard error. Set next state to done.
	pxrlasync->hrFinal = hr;
	pxrlasync->dwCurrentState = xrldlCleanup;
	goto Cleanup;
}

//
// State 3: receive loop
//
HRESULT CXo::xrldlReceiveHandler(PXRL_ASYNC pxrlasync)
{
	HRESULT					hr = S_OK;
	DWORD					dwBytesRead;
	BOOL					fDone;
	XONLINETASK_HANDLE		hsockio = (XONLINETASK_HANDLE)&(pxrlasync->sockio);
	PXRL_DOWNLOAD_EXTENSION pxrldlext = &(pxrlasync->xrlext.dl);

	// Continue until the receive completes asynchronously
	hr = XOnlineTaskContinue(hsockio);
	if (XONLINETASK_STATUS_AVAILABLE(hr))
	{
		// Receive had returned results, see if it succeeded
		if (FAILED(hr))
			goto Error;

		// Succeeded, now get the results and analyze the data
		hr = GetSendRecvResults(hsockio, &dwBytesRead, NULL, NULL);
		if (FAILED(hr))
			goto Error;

		// If zero bytes were read, then the server has closed connection
		if (dwBytesRead)
		{
			// We read something, analyze the data
			hr = XRL_ProcessHttp(pxrlasync, dwBytesRead, &fDone);
			if (SUCCEEDED(hr))
			{
				if (fDone)
				{
					// Call extension to process the header information
					hr = (this->*(pxrldlext->pfnDoneHeaders))(pxrlasync);
					if (FAILED(hr))
						goto Error;

				    // Adjust CONTENT read to exclude header stuff
					pxrlasync->uliTotalReceived.QuadPart += pxrlasync->dwCurrent;
				}
				else if (!IS_HEADER_MODE(pxrlasync))
				{
					// Track how much CONTENT we've downloaded
					pxrlasync->uliTotalReceived.QuadPart += dwBytesRead;
					pxrlasync->dwCurrent += dwBytesRead;
				}
			}
		}
		else
		{
			// Zero bytes read, this means that the server has closed connection.
			// this cannot be a graceful case, because if we already got all the 
			// data we expected, will will not be here asking for more data. We
			// treat this as an error case.
			hr = HRESULT_FROM_WIN32(WSAECONNRESET);
		}

		if (FAILED(hr))
			goto Error;

		// Maybe an application does not need to process the data. In this
		// case, check if we're done
		hr = (this->*(pxrldlext->pfnCompleted))(pxrlasync);
		if (FAILED(hr))
			goto Error;
			
		if (hr == S_OK)
		{
			// see if there is any post-download work
			if (pxrldlext->hTaskPostDownload)
			{
				// Make sure we initialize the task using the supplied callback
				if (pxrldlext->pfnInitializeTask)
				{
					hr = (this->*(pxrldlext->pfnInitializeTask))(xrldlextPostDownload, pxrlasync, pxrldlext->hTaskPostDownload);
					if (FAILED(hr))
						goto Error;
				}

				// Got post download work, do that
				pxrlasync->dwCurrentState = xrldlPostDownload;
			}
			else
			{
				// Nope, jump right to cleanup
				pxrlasync->dwCurrentState = xrldlCleanup;
			}

			goto Cleanup;
		}
		
		// See if we can proceed with processing the data. If this returns true
		// then we will jump to the process data phase, otherwise, we will try to
		// read more from the socket.
		if ((this->*(pxrldlext->pfnProcessData))(pxrlasync))
		{		
			// Make sure we initialize the task using the supplied callback
			if (pxrldlext->pfnInitializeTask)
			{
				hr = (this->*(pxrldlext->pfnInitializeTask))(xrldlextProcessData, pxrlasync, pxrldlext->hTaskProcessData);
				if (FAILED(hr))
					goto Error;
			}

			// Jump to the process data state
			pxrlasync->dwCurrentState = xrldlProcessData;
		}
		else
		{
			// We need more data, adjust the read buffer position and post 
			// another receive
			pxrlasync->wsabuf.buf = (char *)(pxrlasync->pBuffer + pxrlasync->dwCurrent);
			pxrlasync->wsabuf.len = pxrlasync->cbBuffer - pxrlasync->dwCurrent;
			
			SendRecvInitializeContext(pxrlasync->sockio.socket,
						&pxrlasync->wsabuf, 1, 0, 
						pxrlasync->xontask.hEventWorkAvailable, &pxrlasync->sockio);

			hr = RecvInternal(&pxrlasync->sockio);
			if (FAILED(hr))
				goto Error;
		}
	}

Cleanup:
	return(S_OK);

Error:
	// We encountered a hard error. Set next state to done.
	pxrlasync->hrFinal = hr;
	pxrlasync->dwCurrentState = xrldlDone;
	goto Cleanup;
}

//
// State 4: Process Data extension state
//
HRESULT CXo::xrldlProcessDataHandler(
			PXRL_ASYNC	pxrlasync
			)
{
	HRESULT					hr = S_OK;
	PXRL_DOWNLOAD_EXTENSION pxrldlext = &(pxrlasync->xrlext.dl);

    Assert(pxrldlext->hTaskProcessData != NULL);

	// Simple logic: keep pumping the handle until we are told to 
	// stop, then move on to the connect phase
	hr = XOnlineTaskContinue(
				pxrldlext->hTaskProcessData);
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
			// The download actually finished. Gracefully close the connection
			shutdown(pxrlasync->sockio.socket, SD_BOTH);
			
			// see if there is any post-download work
			if (pxrldlext->hTaskPostDownload)
			{
				// Make sure we initialize the task using the supplied callback
				if (pxrldlext->pfnInitializeTask)
				{
					hr = (this->*(pxrldlext->pfnInitializeTask))(xrldlextPostDownload, pxrlasync, pxrldlext->hTaskPostDownload);
					if (FAILED(hr))
						goto Error;
				}

				// Got post download work, do that
				pxrlasync->dwCurrentState = xrldlPostDownload;
			}
			else
			{
				// Nope, jump right to cleanup
				pxrlasync->dwCurrentState = xrldlCleanup;
			}
		}
		else
		{
			// We need more data, adjust the read buffer position and post 
			// another receive			
			pxrlasync->wsabuf.buf = (char *)(pxrlasync->pBuffer + pxrlasync->dwCurrent);
			pxrlasync->wsabuf.len = pxrlasync->cbBuffer - pxrlasync->dwCurrent;
			
			SendRecvInitializeContext(pxrlasync->sockio.socket,
						&pxrlasync->wsabuf, 1, 0, 
						pxrlasync->xontask.hEventWorkAvailable, &pxrlasync->sockio);

			hr = RecvInternal(&pxrlasync->sockio);
			if (FAILED(hr))
				goto Error;

			// Go back to receive state
			pxrlasync->dwCurrentState = xrldlReceive;
		}
	}

Cleanup:
	return(S_OK);

Error:
	// We encountered a hard error. Set next state to done.
	pxrlasync->hrFinal = hr;
	pxrlasync->dwCurrentState = xrldlCleanup;
	goto Cleanup;
}

//
// State 5: Post-download extension state
//
HRESULT CXo::xrldlPostDownloadHandler(
			PXRL_ASYNC	pxrlasync
			)
{
	HRESULT					hr = S_OK;
	PXRL_DOWNLOAD_EXTENSION pxrldlext = &(pxrlasync->xrlext.dl);

    Assert(pxrldlext->hTaskPostDownload != NULL);

	// Simple logic: keep pumping the handle until we are told to 
	// stop, then move on to the connect phase
	hr = XOnlineTaskContinue(
				pxrldlext->hTaskPostDownload);
	if (XONLINETASK_STATUS_AVAILABLE(hr))
	{
		// The task had returned results, see if it succeeded
		if (FAILED(hr))
			goto Error;

		// Alright, we're all done, jump right to cleanup
		pxrlasync->dwCurrentState = xrldlCleanup;
	}

Cleanup:
	return(S_OK);

Error:
	// We encountered a hard error. Set next state to done.
	pxrlasync->hrFinal = hr;
	pxrlasync->dwCurrentState = xrldlCleanup;
	goto Cleanup;
}

//
// State 6: cleanup
//
HRESULT CXo::xrldlCleanupHandler(PXRL_ASYNC pxrlasync)
{
	// Call the generic handler
	XRL_CleanupHandler(pxrlasync);
	
	// We are done
	pxrlasync->dwCurrentState = xrldlDone;

	return(S_OK);
}

//
// Array
// of handlers for XOnlineDownload
//
const PFNXRL_HANDLER CXo::s_rgpfnxrldlHandlers[] =
{
    CXo::xrldlPreConnectHandler,
    CXo::xrldlConnectHandler,
    CXo::xrldlSendHandler,
    CXo::xrldlReceiveHandler,
    CXo::xrldlProcessDataHandler,
    CXo::xrldlPostDownloadHandler,
    CXo::xrldlCleanupHandler
};

//
// Implement the top-level do work function
//
HRESULT CXo::xrldlContinue(
			XONLINETASK_HANDLE	hTask
			)
{
	return(XRL_MainContinue(hTask, s_rgpfnxrldlHandlers, xrldlDone));
}				

//
// Function to kick off downloading
//
HRESULT CXo::Download(
			LPCSTR				szResourcePath, 
			PBYTE				pbExtraHeaders,
			DWORD				cbExtraHeaders,
			FILETIME			*pftModifiedSince, 
			LARGE_INTEGER		liResumeFrom,
			PXRL_ASYNC			pxrlasync
			)
{
	HRESULT					hr = S_OK;
	PXRL_DOWNLOAD_EXTENSION pxrldlext = &(pxrlasync->xrlext.dl);

    Assert(NULL != pxrlasync);
    Assert(NULL != szResourcePath);
    Assert(!cbExtraHeaders || pbExtraHeaders);
    Assert(!cbExtraHeaders || cbExtraHeaders < XRL_MAX_EXTRA_HEADER_SIZE);

    // Make sure the required callbacks are supplied
    Assert(NULL != pxrlasync->xrlext.dl.pfnDoneHeaders);
    Assert(NULL != pxrlasync->xrlext.dl.pfnProcessData);
    Assert(NULL != pxrlasync->xrlext.dl.pfnCompleted);

	// Modify nothing except fill in our work function
	pxrlasync->xontask.pfnContinue = xrldlContinue;
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
	if (pxrldlext->hTaskPreConnect)
	{
		// Make sure we initialize the task using the supplied callback
		if (pxrldlext->pfnInitializeTask)
		{
			hr = (this->*(pxrldlext->pfnInitializeTask))(xrldlextPreConnect, pxrlasync, pxrldlext->hTaskPreConnect);
			if (FAILED(hr))
				goto Error;
		}

    	// Next state is the pre-connect handler
		pxrlasync->dwCurrentState = xrldlPreConnect;
	}
	else
	{
	    // We have no pre-connect work, so just connect to the
	    // service asynchronously
	    hr = XRL_AsyncConnect(pxrlasync);
	    if (FAILED(hr))
    	    goto Error;

    	// Next state is the connect handler
		pxrlasync->dwCurrentState = xrldlConnect;
	}

    // Build the request headers, save the request size (i.e. send
    // buffer size) in pxrlasync->wsabuf for transmission later
	pxrlasync->wsabuf.len = pxrlasync->cbBuffer;
	pxrlasync->wsabuf.buf = (char *)pxrlasync->pBuffer;
	
    //
    // There are cases where the IP address is not known until 
    // after the PreConnect phase. We will allow that. 
    if (pxrlasync->serviceInfo.dwServiceID != XONLINE_INVALID_SERVICE)
    {
	    hr = XRL_BuildGetRequestWithResume(
	    				pxrlasync->serviceInfo.dwServiceID,
	    				szResourcePath,
	    				pxrlasync->serviceInfo.serviceIP.s_addr,
	    				(char *)pxrlasync->pBuffer, 
	    				&(pxrlasync->wsabuf.len), 
	    				pbExtraHeaders,
	    				cbExtraHeaders,
	    				pftModifiedSince, 
	    				liResumeFrom);
	}	    				
	else if (pxrldlext->hTaskPreConnect == NULL)
	{
        AssertSz(FALSE, "Uninitialized service");
		hr = E_INVALIDARG;
	}
    if (FAILED(hr))
        goto Error;

    // Initialize the last state change
    pxrlasync->dwLastStateChange = GetTickCount();

Cleanup:	
	return(hr);

Error:
	XRL_CleanupHandler(pxrlasync);
	goto Cleanup;
}

//
// Define the extension functions for XOnlineDownloadToMemory
//

//
// This function is called after all headers are received
//
HRESULT CXo::xrldtmDoneHeaders(
			PVOID	pvxrlasync
			)
{
	PXRL_ASYNC	pxrlasync = (PXRL_ASYNC)pvxrlasync;

	// Make sure the response is not an error
	if (XONLINEDOWNLOAD_EXSTATUS_ANYERROR(pxrlasync->dwExtendedStatus))
		return(HRESULT_FROM_WIN32(ERROR_BAD_NET_RESP));
		
    // Make sure we have enough space to hold the declared
    // content length
    if (pxrlasync->uliContentLength.QuadPart > pxrlasync->cbBuffer)
    {
		// Tell the caller how much buffers space is needed
		pxrlasync->cbBuffer = pxrlasync->uliContentLength.u.LowPart;
    
		// return insufficient buffer.
		return(HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER));
    }

    return(S_OK);
}

//
// This function returnes whether the received data should be processed
// by the process data extension
//
BOOL CXo::xrldtmProcessData(
			PVOID	pvxrlasync
			)
{
	// Never need any processing
	return(FALSE);
}

HRESULT CXo::xrldtmCompleted(
			PVOID	pvxrlasync
			)
{
	PXRL_ASYNC	pxrlasync = (PXRL_ASYNC)pvxrlasync;
	
	if (pxrlasync->uliTotalReceived.QuadPart >= pxrlasync->uliContentLength.QuadPart)
	{
		// Yes we are done
		return(S_OK);
	}
	
	return(S_FALSE);
}

//
// Function to kick off downloading a resource to memory
//
HRESULT 
CXo::XOnlineDownloadToMemory(
			DWORD		        dwServiceID,
			LPCSTR				szResourcePath, 
			PBYTE				pbBuffer,
			DWORD				cbBuffer, 
			PBYTE				pbExtraHeaders,
			DWORD				cbExtraHeaders,
			DWORD				dwTimeout,
			HANDLE				hWorkEvent,
			XONLINETASK_HANDLE	*phTask
			)
{
    XoEnter("XOnlineDownloadToMemory");
    XoCheck(NULL != szResourcePath);
    XoCheck(NULL != phTask);
    XoCheck(0 < cbBuffer);

	HRESULT					hr = S_OK;
	PXRL_ASYNC				pxrlasync = NULL;
	PXRL_ASYNC_EXTENDED		pxrlasyncext = NULL;
	PXRL_DOWNLOAD_EXTENSION pxrldlext = NULL;
	LARGE_INTEGER			liTemp;

	// We are responsible for allocating the context and the work buffer
	// The work buffer are the specified bytes immediately following the
	// context structure
	pxrlasyncext = (PXRL_ASYNC_EXTENDED)SysAlloc(sizeof(XRL_ASYNC_EXTENDED) + (pbBuffer?0:cbBuffer),
        PTAG_XONLINETASK_DOWNLOAD_MEMORY);
	if (!pxrlasyncext)
	{
		hr = E_OUTOFMEMORY;
		goto Error;
	}

	ZeroMemory(pxrlasyncext, sizeof(XRL_ASYNC_EXTENDED));
	pxrlasync = &(pxrlasyncext->xrlasync);
	pxrlasync->pBuffer = pbBuffer?pbBuffer:(PBYTE)(pxrlasyncext + 1);
	pxrlasync->cbBuffer = cbBuffer;

	// Fill in the blanks
	TaskInitializeContext(&pxrlasync->xontask);
	pxrlasync->xontask.hEventWorkAvailable = hWorkEvent;
	pxrlasync->hrFinal = S_OK;
	pxrlasync->hrXErr = S_OK;
	hr = XOnlineGetServiceInfo(dwServiceID, &pxrlasync->serviceInfo);
	Assert(SUCCEEDED(hr));
	pxrlasync->dwTimeoutMs = dwTimeout;

	// Set up the extension block
	pxrldlext = &(pxrlasync->xrlext.dl);
	pxrldlext->pfnDoneHeaders = xrldtmDoneHeaders;
	pxrldlext->pfnProcessData = xrldtmProcessData;
	pxrldlext->pfnCompleted = xrldtmCompleted;
	pxrldlext->pfnCleanup = NULL;
	pxrldlext->pfnInitializeTask = NULL;
	pxrldlext->hTaskPreConnect = NULL;
	pxrldlext->hTaskProcessData = NULL;
	pxrldlext->hTaskPostDownload = NULL;

	// Call the more generic XOnlineDownload
	liTemp.QuadPart = 0;
	hr = Download(szResourcePath, 
				pbExtraHeaders, cbExtraHeaders, 
				NULL, liTemp, pxrlasync);
	if (FAILED(hr))
		goto Error;

	// Return the context as an opaque handle
	*phTask = (XONLINETASK_HANDLE)pxrlasync;

Cleanup:	
	return(XoLeave(hr));

Error:
	// Failed setup, make sure we clean up everything
	if (pxrlasyncext)
		SysFree(pxrlasyncext);

	goto Cleanup;
}

//
// Define the extension functions for XOnlineDownloadFile
//

//
// This function is called after all headers are received
//
HRESULT CXo::xrldfDoneHeaders(
			PVOID	pvxrlasync
			)
{
	HRESULT			hr = S_OK;
	PXRL_ASYNC		pxrlasync = (PXRL_ASYNC)pvxrlasync;
	LARGE_INTEGER	liTemp;
	
	// Analyze the response code
	switch (pxrlasync->dwExtendedStatus)
	{
	case 200:
		// A 200 is coming back, which means the entire 
		// resource is returned by the server. We make sure that 
		// the file pointer is set to the beginning (a la resume)
		if (pxrlasync->uliFileSize.QuadPart > 0)
		{
			liTemp.QuadPart = 0;
			if (!SetFilePointerEx(
						pxrlasync->fileio.hFile, liTemp, &liTemp, FILE_BEGIN))
			{
				hr = HRESULT_FROM_WIN32(GetLastError());
				goto Error;
			}
		}

		// Reset these counters, note that we don't have to change
		// content length and the file size
		pxrlasync->uliTotalWritten.QuadPart = 0;
		pxrlasync->uliTotalReceived.QuadPart = 0;
		break;
		
	case 206:
		// This code is returned if a resume is attempted, and
		// that the server resource was not modified since the
		// date we supplied. This means that the server will 
		// return only the bytes that we don't already have.
		// If we get this code, the file pointer would already have
		// been set to the end of file, so we can simply let
		// WriteFile to its work.
		//
		// The content length returned will only be the remainder
		// of the data, so we add back what we already have
		pxrlasync->uliContentLength.QuadPart += 
					pxrlasync->uliTotalWritten.QuadPart;
		break;

	case 416:
		// This code means that the specified range is not 
		// satisfiable. This usually means that we already got 
		// the entire file to start with.
		//
		// This means that we are done; set the content length and
		// total written counters to the current file size
		pxrlasync->uliContentLength.QuadPart = pxrlasync->uliFileSize.QuadPart;
		pxrlasync->uliTotalWritten.QuadPart = pxrlasync->uliContentLength.QuadPart;
		break;
		
	default:
		hr = HRESULT_FROM_WIN32(ERROR_BAD_NET_RESP);
		AssertSz(SUCCEEDED(hr), "xrldfDoneHeaders");
	}

Error:
    return(hr);
}

//
// This function returnes whether the received data should be processed
// by the process data extension
//
BOOL CXo::xrldfProcessData(PVOID pvxrlasync)
{
	PXRL_ASYNC	pxrlasync = (PXRL_ASYNC)pvxrlasync;
	
	// We will proceed to writing data to disk if the following two conditions
	// are true:
	// 1) No longer parsing headers AND
	// 2a) There is at least XBOX_HD_SECTOR_SIZE bytes to write OR
	// 2b) This is the last block of data expected
	if (!IS_HEADER_MODE(pxrlasync) &&
		((pxrlasync->dwCurrent >= XBOX_HD_SECTOR_SIZE) ||
		 (pxrlasync->uliTotalReceived.QuadPart >= pxrlasync->uliContentLength.QuadPart)))
		return(TRUE);

	return(FALSE);
}

HRESULT CXo::xrldfCompleted(PVOID pvxrlasync)
{
	PXRL_ASYNC	pxrlasync = (PXRL_ASYNC)pvxrlasync;
	
	if (pxrlasync->uliTotalWritten.QuadPart >= pxrlasync->uliContentLength.QuadPart)
	{
		return(S_OK);
	}
	
	return(S_FALSE);
}

HRESULT CXo::xrldfInitializeTask(
			XRLDOWNLOAD_EXTTYPE	xrldlext,
			PVOID				pvxrlasync,
			XONLINETASK_HANDLE	hTask
			)
{
	HRESULT				hr = S_OK;
	PXRL_ASYNC			pxrlasync = (PXRL_ASYNC)pvxrlasync;
	PXRL_ASYNC_EXTENDED	pxrlasyncext = NULL;

	Assert(pxrlasync != NULL);
	Assert(hTask != NULL);

	// XOnlineDownloadFile only services xrldlextProcessData
	if (xrldlext == xrldlextProcessData)
	{
		// Find the outer containing XRL_ASYNC_EXTENDED
		pxrlasyncext = CONTAINING_RECORD(pxrlasync, XRL_ASYNC_EXTENDED, xrlasync);

		// Caveat emptor: now we need to see if the current file size needs to
		// be grown
		if (pxrlasync->uliTotalReceived.QuadPart > pxrlasync->uliFileSize.QuadPart)
		{
			// Growing the file is a synchronous operation, there is a dedicated state 
			// for that ... set up the subtask to start with growing the file
			pxrlasyncext->dwCurrentState = xrldfGrowFile;
		}
		else
		{
			DWORD	dwContentRead = pxrlasync->dwCurrent;
			
			// Make sure the write size is sector-aligned. Usually, we will write less
			// than or up to the data we have, but if this is already the last chunk, 
			// then we will write out all data (or more).
			if (pxrlasync->uliTotalReceived.QuadPart >= pxrlasync->uliContentLength.QuadPart)
				dwContentRead += (XBOX_HD_SECTOR_SIZE - 1);
			dwContentRead &= ~(XBOX_HD_SECTOR_SIZE - 1);
			
			// Write data
			ReadWriteFileInitializeContext(pxrlasync->fileio.hFile,
						pxrlasync->pBuffer, dwContentRead, 
						pxrlasync->uliTotalWritten, 
						pxrlasync->xontask.hEventWorkAvailable, &pxrlasync->fileio);

			hr = WriteFileInternal(&pxrlasync->fileio);
			if (FAILED(hr))
				goto Error;

			// Set the initial state of the subtask to wait for write complete
			pxrlasyncext->dwCurrentState = xrldfWriteFile;
		}
	}

Error:
	return(hr);
}

//
// State xrldfGrowFile: grow file
//
HRESULT CXo::xrldfGrowFileHandler(
			PXRL_ASYNC_EXTENDED	pxrlasyncext
			)
{
	HRESULT				hr = S_OK;
	PXRL_ASYNC			pxrlasync = &pxrlasyncext->xrlasync;
	LARGE_INTEGER		liGrowSize;
	DWORD				dwAlignedSize;

	// Double check that we are still sector-aligned
    Assert((pxrlasync->uliFileSize.u.LowPart & (XBOX_HD_SECTOR_SIZE - 1)) == 0);

	// Figure out how much to grow
	liGrowSize.QuadPart = pxrlasync->uliContentLength.QuadPart - 
							pxrlasync->uliFileSize.QuadPart;

	// Round up to sector alignment
	liGrowSize.QuadPart += (XBOX_HD_SECTOR_SIZE - 1);
	liGrowSize.u.LowPart &= ~(XBOX_HD_SECTOR_SIZE - 1); 

	// Limit growth if necessary
	if (liGrowSize.QuadPart > XRL_INCREMENTAL_GROW_FILE_SIZE)
		liGrowSize.QuadPart = XRL_INCREMENTAL_GROW_FILE_SIZE;

	// Seek to that offset
	if (!SetFilePointerEx(pxrlasync->fileio.hFile, 
				liGrowSize, &liGrowSize, FILE_CURRENT))
	{
		hr = HRESULT_FROM_WIN32(GetLastError());
		goto Error;
	}

	// Set end of file at that
	if (!SetEndOfFile(pxrlasync->fileio.hFile))
	{
		hr = HRESULT_FROM_WIN32(GetLastError());
		goto Error;
	}

	TraceSz1(Verbose, "Grow file to %I64u bytes", liGrowSize.QuadPart);

	// Set new file size
	pxrlasync->uliFileSize.QuadPart = liGrowSize.QuadPart;

	// Make sure the write size is sector-aligned
	dwAlignedSize = pxrlasync->dwCurrent;

	// If this is the last chunk, then make sure all data is written
	if (pxrlasync->uliTotalReceived.QuadPart >= pxrlasync->uliContentLength.QuadPart)
		dwAlignedSize += (XBOX_HD_SECTOR_SIZE - 1);
	dwAlignedSize &= ~(XBOX_HD_SECTOR_SIZE - 1);
	
	// Now we kick off the write
	ReadWriteFileInitializeContext(pxrlasync->fileio.hFile,
				pxrlasync->pBuffer, dwAlignedSize, 
				pxrlasync->uliTotalWritten, 
				pxrlasync->xontask.hEventWorkAvailable, &pxrlasync->fileio);

	hr = WriteFileInternal(&pxrlasync->fileio);
	if (FAILED(hr))
		goto Error;

	// Set next state	
	pxrlasyncext->dwCurrentState = xrldfWriteFile;

Cleanup:
	return(S_OK);

Error:
	// We encountered a hard error. End the subtask
	pxrlasync->hrFinal = hr;
	pxrlasyncext->dwCurrentState = xrldfDone;
	goto Cleanup;
}

//
// State xrldfWriteFile: write file state
//
HRESULT CXo::xrldfWriteFileHandler(
			PXRL_ASYNC_EXTENDED	pxrlasyncext
			)
{
	HRESULT				hr = S_OK;
	DWORD				dwBytesWritten;
	PXRL_ASYNC			pxrlasync = &pxrlasyncext->xrlasync;
	XONLINETASK_HANDLE	hfileio = (XONLINETASK_HANDLE)&(pxrlasync->fileio);

	// Continue until the receive completes asynchronously
	hr = XOnlineTaskContinue(hfileio);
	if (XONLINETASK_STATUS_AVAILABLE(hr))
	{
		// Receive had returned results, see if it succeeded
		if (FAILED(hr))
			goto Error;

		// Succeeded, now get the results and analyze the data
		hr = GetReadWriteFileResults(hfileio, &dwBytesWritten, NULL);
		if (FAILED(hr))
			goto Error;

//		TraceSz1(Verbose, "Wrote %u bytes", dwBytesWritten);
		
		// Track how much we have written so far
		pxrlasync->uliTotalWritten.QuadPart += dwBytesWritten;
		Assert((dwBytesWritten & (XBOX_HD_SECTOR_SIZE - 1)) == 0);

		// See how much unwritten data we have
		if (pxrlasync->dwCurrent > dwBytesWritten)
			pxrlasync->dwCurrent -= dwBytesWritten;
		else
			pxrlasync->dwCurrent = 0;
		
		// See if we have written everything we expect to receive
		if (pxrlasync->uliTotalWritten.QuadPart >= pxrlasync->uliContentLength.QuadPart)
		{
			// Done writing, see if we need to truncate the file
			if (pxrlasync->uliTotalWritten.QuadPart > pxrlasync->uliContentLength.QuadPart)
			{
				FILE_END_OF_FILE_INFORMATION	feofi;
				IO_STATUS_BLOCK					iosb;
				NTSTATUS						Status;
				
				// Truncate to real end of file using the Nt API
				feofi.EndOfFile.QuadPart = pxrlasync->uliContentLength.QuadPart;
				Status = NtSetInformationFile(pxrlasync->fileio.hFile, &iosb, 
							(PVOID)&feofi, sizeof(FILE_END_OF_FILE_INFORMATION),
							FileEndOfFileInformation);
			    if (!NT_SUCCESS(Status)) 
			    {
					hr = HRESULT_FROM_WIN32(RtlNtStatusToDosError(Status));
					goto Error;
				}
			}

			// See if we received the same number of bytes as the content length
			// had advertised
			if (pxrlasync->uliTotalReceived.QuadPart != pxrlasync->uliContentLength.QuadPart)
			{
				// We got something other than expected, we will be very strict and
				// fail the operation. 
				hr = HRESULT_FROM_WIN32(ERROR_BAD_LENGTH);
				goto Error;
			}
		}
		else
		{
			// Move any unwritten data to the front of the buffer
			if (pxrlasync->dwCurrent)
			{
//				TraceSz2(Verbose, "[%I64u] %u bytes unwritten", 
//							pxrlasync->uliTotalWritten.QuadPart,
//							pxrlasync->dwCurrent);
				
				MoveMemory(pxrlasync->pBuffer, 
							pxrlasync->pBuffer + dwBytesWritten,
							pxrlasync->dwCurrent);
			}
		}

		// We're done this subtask, return to the main download task
		pxrlasyncext->dwCurrentState = xrldfDone;
	}

Cleanup:
	return(S_OK);

Error:
	// We encountered a hard error. Set next state to cleanup.
	pxrlasync->hrFinal = hr;
	pxrlasyncext->dwCurrentState = xrldfDone;
	goto Cleanup;
}

//
// Implement the do work function for XOnlineDownloadFile subtask
//
HRESULT CXo::xrldfContinue(
			XONLINETASK_HANDLE	hTask
			)
{
	HRESULT				hr = S_OK;
	PXRL_ASYNC_EXTENDED	pxrlasyncext = NULL;
	PXRL_ASYNC			pxrlasync = NULL;

	// We can make very tight assumptions and validate here
	Assert(hTask != NULL);

	// hTask points to the xontask field of XRL_ASYNC_EXTENDED,
	// find the containing pxrlasyncext structure
	pxrlasyncext = CONTAINING_RECORD(hTask, XRL_ASYNC_EXTENDED, xontask);
	pxrlasync = &pxrlasyncext->xrlasync;

	// Figure out what state we are in ...
	switch (pxrlasyncext->dwCurrentState)
	{
	case xrldfGrowFile:
		hr = xrldfGrowFileHandler(pxrlasyncext);
		break;

	case xrldfWriteFile:
		hr = xrldfWriteFileHandler(pxrlasyncext);
		break;

	default:
		// This is a bad state. We force it to end so we don't get stuck
		// here forever.
		AssertSz(FALSE, "xrldfContinue: invalid current state!");
		pxrlasyncext->dwCurrentState = xrldfDone;
		hr = pxrlasync->hrFinal = E_FAIL;
	}

	// When the next state is the final state, we set the flag so we don't
	// continue.
	if (pxrlasyncext->dwCurrentState == xrldfDone)
	{
		// Indicate that we are done and return the final code
		hr = pxrlasync->hrFinal;
		XONLINETASK_SET_COMPLETION_STATUS(hr);
	}
	
	return(hr);
}				

//
// Function to kick off downloading a resoruce to a file
//
HRESULT 
CXo::XOnlineDownloadFile(
			DWORD       		dwServiceID,
			LPCSTR				szResourcePath, 
			PBYTE				pbBuffer,
			DWORD				cbBuffer, 
			PBYTE				pbExtraHeaders,
			DWORD				cbExtraHeaders,
			DWORD				dwFlags,
			LPCSTR				szLocalPath, 
			DWORD				dwTimeout,
			HANDLE				hWorkEvent,
			XONLINETASK_HANDLE	*phTask
			)
{
    XoEnter("XOnlineDownloadFile");
    XoCheck(szResourcePath != NULL);
    XoCheck(cbBuffer >= XBOX_HD_SECTOR_SIZE);
    XoCheck(szLocalPath != NULL);
    XoCheck(phTask != NULL);

	HRESULT						hr = S_OK;
	PXRL_ASYNC					pxrlasync = NULL;
	PXRL_ASYNC_EXTENDED			pxrlasyncext = NULL;
	BY_HANDLE_FILE_INFORMATION	bhfi;
	PXONLINETASK_FILEIO			pfileio = NULL;
	LARGE_INTEGER				liResumeFrom;
	FILETIME					ftLastWritten;
	FILETIME					*pftLastModified = NULL;
	BOOL						fForceDownload;
	PXRL_DOWNLOAD_EXTENSION 	pxrldlext = NULL;

	// The buffer size needs more validation:
	// 1) Sector size must be power of 2
	// 2) The size will be rounded down to multiples of Xbox HD sector size
    Assert((XBOX_HD_SECTOR_SIZE & (XBOX_HD_SECTOR_SIZE - 1)) == 0);
    cbBuffer &= (~(XBOX_HD_SECTOR_SIZE - 1));

	fForceDownload = ((dwFlags & XON_DF_FORCE_DOWNLOAD) != 0);

	// We are responsible for allocating the context and the work buffer
	// The work buffer are the specified bytes immediately following the
	// context structure
	pxrlasyncext = (PXRL_ASYNC_EXTENDED)SysAlloc(sizeof(XRL_ASYNC_EXTENDED) + (pbBuffer?0:cbBuffer),
        PTAG_XONLINETASK_DOWNLOAD_FILE);

	if (!pxrlasyncext)
	{
		hr = E_OUTOFMEMORY;
		goto Error;
	}

	// Initialize the context
	ZeroMemory(pxrlasyncext, sizeof(XRL_ASYNC_EXTENDED));
	pxrlasync = &pxrlasyncext->xrlasync;
	pxrlasync->pBuffer = pbBuffer?pbBuffer:(PBYTE)(pxrlasyncext + 1);
	pxrlasync->cbBuffer = cbBuffer;

	// Fill in the blanks
	TaskInitializeContext(&pxrlasync->xontask);
	pxrlasync->xontask.hEventWorkAvailable = hWorkEvent;
	pxrlasync->hrFinal = S_OK;
	pxrlasync->hrXErr = S_OK;
	hr = XOnlineGetServiceInfo(dwServiceID, &pxrlasync->serviceInfo);
	Assert(SUCCEEDED(hr));
	pxrlasync->uliTotalWritten.QuadPart = 0;
	pxrlasync->dwTimeoutMs = dwTimeout;
	pfileio = &pxrlasync->fileio;

	// Initialize our ProcessData subtask
	TaskInitializeContext(&pxrlasyncext->xontask);
	pxrlasyncext->xontask.pfnContinue = xrldfContinue;
	pxrlasyncext->xontask.hEventWorkAvailable = hWorkEvent;
	pxrlasyncext->dwCurrentState = xrldfDone;

	// Set up the extension block
	pxrldlext = &(pxrlasync->xrlext.dl);
	pxrldlext->pfnDoneHeaders = xrldfDoneHeaders;
	pxrldlext->pfnProcessData = xrldfProcessData;
	pxrldlext->pfnCompleted = xrldfCompleted;
	pxrldlext->pfnCleanup = NULL;
	pxrldlext->pfnInitializeTask = xrldfInitializeTask;
	pxrldlext->hTaskPreConnect = NULL;
	pxrldlext->hTaskProcessData = 
					(XONLINETASK_HANDLE)&(pxrlasyncext->xontask);
	pxrldlext->hTaskPostDownload = NULL;

	// Create the target file
	pfileio->hFile = CreateFile(
				szLocalPath, 
				GENERIC_READ | GENERIC_WRITE, 
				FILE_SHARE_READ, 
				NULL, 
				fForceDownload?CREATE_ALWAYS:OPEN_ALWAYS, 
				FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN |
				FILE_FLAG_OVERLAPPED | FILE_FLAG_NO_BUFFERING, 
				NULL);
	if (pfileio->hFile == INVALID_HANDLE_VALUE)
	{
		hr = HRESULT_FROM_WIN32(GetLastError());
		goto Error;
	}

	// We will figure out if we will try to resume a possibly-partially
	// downloaded file ...
	liResumeFrom.QuadPart = 0;
	if (!fForceDownload)
	{
		// Get the file size
		if (!GetFileSizeEx(pfileio->hFile, &liResumeFrom))
		{
			hr = HRESULT_FROM_WIN32(GetLastError());
			goto Error;
		}

		// No point resuming if the file size is zero anyway
		if (liResumeFrom.QuadPart != 0)
		{
			// Our asynchronous writes must be sector aligned, so we will
			// round it down to the sector boundary if it is somehow not
			// aligned right.
			if ((liResumeFrom.u.LowPart & (XBOX_HD_SECTOR_SIZE - 1)) != 0)
				liResumeFrom.u.LowPart &= ~(XBOX_HD_SECTOR_SIZE - 1);

			// Adjust the file pointer to the right offset
			if (!SetFilePointerEx(pfileio->hFile, 
							liResumeFrom, &liResumeFrom, FILE_BEGIN))
			{
				hr = HRESULT_FROM_WIN32(GetLastError());
				goto Error;
			}

			// Set up all our counters as if we already got liResume bytes
			pxrlasync->uliTotalReceived.QuadPart = liResumeFrom.QuadPart;
			pxrlasync->uliTotalWritten.QuadPart = liResumeFrom.QuadPart;
			pxrlasync->uliFileSize.QuadPart = liResumeFrom.QuadPart;
		
			// This will cause an attempt to resume, now we need 
			// the last write date of the file. We should not use the
			// creation time, because if the file alsready had been
			// restored before, we might be forced to download the
			// whole thing again.
			if (GetFileTime(pfileio->hFile, NULL, NULL, &ftLastWritten))
				pftLastModified = &ftLastWritten;
		}
	}	

	// Call the more generic XOnlineDownload
	hr = Download(szResourcePath, 
				pbExtraHeaders, cbExtraHeaders, 
				pftLastModified, liResumeFrom, pxrlasync);
	if (FAILED(hr))
		goto Error;

	// Return the context as an opaque handle
	*phTask = (XONLINETASK_HANDLE)pxrlasync;

Cleanup:	
	return(XoLeave(hr));

Error:

	// Failed setup, make sure we clean up everything
	if (pfileio->hFile && (pfileio->hFile != INVALID_HANDLE_VALUE))
		CloseHandle(pfileio->hFile);
	if (pxrlasyncext)
		SysFree(pxrlasyncext);
	goto Cleanup;
}

// Function to get the download progress
HRESULT
CXo::XOnlineDownloadGetProgress(
			XONLINETASK_HANDLE	hTask,
			DWORD				*pdwPercentDone,
			ULARGE_INTEGER		*puliNumerator,
			ULARGE_INTEGER		*puliDenominator
			)
{
    XoEnter("XOnlineDownloadGetProgress");
    XoCheck(hTask != NULL);

	PXRL_ASYNC	pxrlasync = (PXRL_ASYNC)hTask;

	if (puliNumerator)
		puliNumerator->QuadPart = pxrlasync->uliTotalReceived.QuadPart;
	if (puliDenominator)
		puliDenominator->QuadPart = pxrlasync->uliContentLength.QuadPart;
	if (pdwPercentDone)
	{
		if (pxrlasync->uliContentLength.QuadPart != 0)
		{
			*pdwPercentDone = 
				(DWORD)((pxrlasync->uliTotalReceived.QuadPart * 100)/
					pxrlasync->uliContentLength.QuadPart);
			if (*pdwPercentDone > 100)
				*pdwPercentDone = 100;
		}
		else
			*pdwPercentDone = 0;
	}

	return(XoLeave(S_OK));
}

// Helper function to return the results of a download
HRESULT 
CXo::XOnlineDownloadGetResults(
			XONLINETASK_HANDLE	hTask,
			LPBYTE				*ppbBuffer,
			DWORD				*pcbBuffer,
			ULARGE_INTEGER		*puliTotalReceived,
			ULARGE_INTEGER		*puliContentLength,
			DWORD				*pdwExtendedStatus,
			FILETIME			*pftTimestamp,
			FILETIME			*pftLastModified
			)
{
    XoEnter("XOnlineDownloadGetResults");
    XoCheck(hTask != NULL);

	PXRL_ASYNC	pxrlasync = (PXRL_ASYNC)hTask;

	TaskVerifyContext(&(pxrlasync->xontask));

	// Fill in all the values the caller is interested in
	if (ppbBuffer)
		*ppbBuffer = pxrlasync->pBuffer;
	if (pcbBuffer)
		*pcbBuffer = pxrlasync->dwCurrent;
	if (puliTotalReceived)
		puliTotalReceived->QuadPart = pxrlasync->uliTotalReceived.QuadPart;
	if (puliContentLength)
		puliContentLength->QuadPart = pxrlasync->uliContentLength.QuadPart;
	if (pdwExtendedStatus)
		*pdwExtendedStatus = pxrlasync->dwExtendedStatus;
	if (pftTimestamp)
		memcpy(pftTimestamp, &(pxrlasync->ftResponse), sizeof(FILETIME));
	if (pftLastModified)
		memcpy(pftLastModified, &(pxrlasync->ftLastModified), sizeof(FILETIME));

	return(XoLeave(pxrlasync->hrFinal));
}


