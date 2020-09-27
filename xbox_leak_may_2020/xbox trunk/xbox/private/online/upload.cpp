/*++

Copyright (c) Microsoft Corporation

Author:
    Keith Lau (keithlau)

Description:
	Module implementing XRL upload APIs:
		XOnlineUploadFromMemory
		XOnlineUploadFile

Module Name:

    upload.c

--*/

#include "xonp.h"
#include "xonver.h"

//
// Define enumerated types that describe the states for each operation
//
typedef enum
{
	xrlulPreConnect = 0,
	xrlulConnect,
	xrlulReadData,
	xrlulSendData,
	xrlulReceiveStatus,
	xrlulPostUpload,
	xrlulCleanup,
	xrlulDone

} XON_UPLOAD_STATES;

typedef enum
{
	xrlufmDone = 0,

} XON_UPLOAD_FROM_MEMORY_STATES;

typedef enum
{
	xrlufReadData = 0,
	xrlufDone

} XON_UPLOAD_FILE_STATES;

//
// ==============================================================
//   S T A T E   M A C H I N E   D E F I N I T I O N S
// ==============================================================
//

// Define the state machines for XOnlineUpload

//
// State 0: Pre-connect extension state
//
HRESULT CXo::xrlulPreConnectHandler(
			PXRL_ASYNC	pxrlasync
			)
{
	HRESULT					hr = S_OK;
	PXRL_UPLOAD_EXTENSION	pxrlulext = &(pxrlasync->xrlext.ul);

    Assert(pxrlulext->hTaskPreConnect != NULL);

	// Simple logic: keep pumping the handle until we are told to 
	// stop, then move on to the connect phase
	hr = XOnlineTaskContinue(
				pxrlulext->hTaskPreConnect
				);
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
			
			// see if there is any post-upload work
			if (pxrlulext->hTaskPostUpload)
			{
				// Make sure we initialize the task using the supplied callback
				if (pxrlulext->pfnInitializeTask)
				{
					hr = (this->*(pxrlulext->pfnInitializeTask))(
								xrlulextPostUpload, pxrlasync, 
								pxrlulext->hTaskPostUpload);
					if (FAILED(hr))
						goto Error;
				}

				// Got post download work, do that
				pxrlasync->dwCurrentState = xrlulPostUpload;
				goto Cleanup;
			}
			else
			{
				// Nope, jump right to cleanup
				pxrlasync->dwCurrentState = xrlulCleanup;
			}

			goto Cleanup;
		}
		
	    // Connect to the service asynchronously
	    hr = XRL_AsyncConnect(pxrlasync);
	    if (FAILED(hr))
    	    goto Error;

		// Set the next state
		pxrlasync->dwCurrentState = xrlulConnect;
	}

Cleanup:
	return(S_OK);

Error:
	// We encountered a hard error. Set next state to done.
	pxrlasync->hrFinal = hr;
	pxrlasync->dwCurrentState = xrlulCleanup;
	goto Cleanup;
}

//
// State 1: wait for connect loop
//
HRESULT CXo::xrlulConnectHandler(
			PXRL_ASYNC	pxrlasync
			)
{
	HRESULT					hr = S_OK;
	XONLINETASK_HANDLE		hsockio = (XONLINETASK_HANDLE)&(pxrlasync->sockio);
	PXRL_UPLOAD_EXTENSION	pxrlulext = &(pxrlasync->xrlext.ul);

	// Continue until the connect completes asynchronously
	hr = XOnlineTaskContinue(hsockio);
	if (XONLINETASK_STATUS_AVAILABLE(hr))
	{
		// Connect had returned results, see if it succeeded
		if (FAILED(hr))
			goto Error;

		// Reset the read and send counters
		pxrlasync->uliTotalReceived.QuadPart = 0;
		pxrlasync->uliTotalWritten.QuadPart = 0;
		pxrlasync->dwCurrent = 0;

		// Mark the fact that this is the first packet being sent
		// and save the header size in dwTemp. 
		//
		// NOTE: This means that the headers must not change from
		// this point on.
		pxrlasync->dwFlags |= XRL_SEND_DATA_CONTAINS_HEADERS;
		pxrlasync->dwTemp = pxrlasync->wsabuf.len;

		// Move on the the read data phase. This is mandatory for uploads
		// Make sure we initialize the task using the supplied callback
		if (pxrlulext->pfnInitializeTask)
		{
			hr = (this->*(pxrlulext->pfnInitializeTask))(xrlulextReadData, pxrlasync, pxrlulext->hTaskReadData);
			if (FAILED(hr))
				goto Error;
		}

		// Jump to the process data state
		pxrlasync->dwCurrentState = xrlulReadData;
	}

Cleanup:
	return(S_OK);

Error:
	// We encountered a hard error. Set next state to done.
	pxrlasync->hrFinal = hr;
	pxrlasync->dwCurrentState = xrlulCleanup;
	goto Cleanup;
}

//
// State 2: Read Data extension state
//
// Note: upon completing this state, the subtask must have read in the
// data to send to the server and set pxrlasync->wsabuf.buf to point to
// the first byte to send, and pxrlasync->wsabuf.len to the size of
// the send buffer. Upload will loop through ReadData-SendData until the
// number of bytes specified in uliContentLength
//
HRESULT CXo::xrlulReadDataHandler(
			PXRL_ASYNC	pxrlasync
			)
{
	HRESULT					hr = S_OK;
	PXRL_UPLOAD_EXTENSION	pxrlulext = &(pxrlasync->xrlext.ul);

    Assert(pxrlulext->hTaskReadData != NULL);

	// Simple logic: keep pumping the handle until we are told to 
	// stop, then move on to the connect phase
	hr = XOnlineTaskContinue(
				pxrlulext->hTaskReadData);
	if (XONLINETASK_STATUS_AVAILABLE(hr))
	{
		// The task had returned results, see if it succeeded
		if (FAILED(hr))
			goto Error;

		// We expect the data to be sent out to be in pxrlasync->wsabuf
		// so we just kick off the send now
		SendRecvInitializeContext(pxrlasync->sockio.socket,
					&pxrlasync->wsabuf, 1, 0, 
					pxrlasync->xontask.hEventWorkAvailable, &pxrlasync->sockio);

		hr = SendInternal(&pxrlasync->sockio);
		if (FAILED(hr))
			goto Error;

		// Go to send data state
		pxrlasync->dwCurrentState = xrlulSendData;
	}

Cleanup:
	return(S_OK);

Error:
	// We encountered a hard error. Set next state to done.
	pxrlasync->hrFinal = hr;
	pxrlasync->dwCurrentState = xrlulCleanup;
	goto Cleanup;
}

//
// State 3: send data loop
//
HRESULT CXo::xrlulSendDataHandler(PXRL_ASYNC pxrlasync)
{
	HRESULT					hr = S_OK;
	DWORD					dwBytesSent;
	DWORD					dwContentBytesSent;
	XONLINETASK_HANDLE		hsockio = (XONLINETASK_HANDLE)&(pxrlasync->sockio);
	PXRL_UPLOAD_EXTENSION	pxrlulext = &(pxrlasync->xrlext.ul);

	// Continue until the send completes asynchronously
	hr = XOnlineTaskContinue(hsockio);
	if (XONLINETASK_STATUS_AVAILABLE(hr))
	{
		// Send had returned results, see if it succeeded
		if (FAILED(hr))
			goto Error;

		// Succeeded, now get the results and analyze the data
		hr = GetSendRecvResults(hsockio, &dwBytesSent, NULL, NULL);
		if (FAILED(hr))
			goto Error;

		// We make a distinction between how many bytes sent, and how
		// many content bytes (i.e. excluding headers) sent.
		dwContentBytesSent = dwBytesSent;

		// If the sent packet contains headers, we will subtract the
		// header size because what we are really tracking is content
		if (pxrlasync->dwFlags & XRL_SEND_DATA_CONTAINS_HEADERS)
		{
            Assert(dwContentBytesSent >= pxrlasync->dwTemp);
			dwContentBytesSent -= pxrlasync->dwTemp;
			pxrlasync->dwTemp = 0;
			pxrlasync->dwFlags &= (~XRL_SEND_DATA_CONTAINS_HEADERS);
		}

		// Track how much we've sent
		pxrlasync->uliTotalWritten.QuadPart += dwContentBytesSent;

		// See if we're done completely
		hr = (this->*(pxrlulext->pfnCompleted))((PVOID)pxrlasync);
		if (FAILED(hr))
			goto Error;
		
		if (hr == S_OK)
		{
			// Yup, we've uploaded everything. Now kick off a receive to
			// get the final server response
			pxrlasync->wsabuf.buf = (char *)pxrlasync->pBuffer;
			pxrlasync->wsabuf.len = pxrlasync->cbBuffer;
			
			SendRecvInitializeContext(pxrlasync->sockio.socket,
						&pxrlasync->wsabuf, 1, 0, 
						pxrlasync->xontask.hEventWorkAvailable, &pxrlasync->sockio);

			hr = RecvInternal(&pxrlasync->sockio);
			if (FAILED(hr))
				goto Error;

			// Reset the receive counter
			pxrlasync->uliTotalReceived.QuadPart = 0;
			pxrlasync->uliContentLength.QuadPart = 0;
			pxrlasync->dwCurrent = 0;
			pxrlasync->dwTemp = XRL_FLAG_HEADER_MODE;

			// Set next state	
			pxrlasync->dwCurrentState = xrlulReceiveStatus;
			goto Cleanup;
		}

		// Not done yet, see if we sent everything already
		if (dwBytesSent < pxrlasync->wsabuf.len)
		{
			// The whole buffer was not sent out yet, adjust the buffer and 
			// do another send.
			pxrlasync->wsabuf.buf += dwBytesSent;
			pxrlasync->wsabuf.len -= dwBytesSent;
			
			SendRecvInitializeContext(pxrlasync->sockio.socket,
						&pxrlasync->wsabuf, 1, 0, 
						pxrlasync->xontask.hEventWorkAvailable, &pxrlasync->sockio);

			hr = SendInternal(&pxrlasync->sockio);
			if (FAILED(hr))
				goto Error;
		}
		else
		{
			// We're done sneding the entire buffer. Now since we still have
			// more data to upload, we will jump back to the Read data state
			pxrlasync->wsabuf.buf = (char *)pxrlasync->pBuffer;
			pxrlasync->wsabuf.len = 0;
			
			if (pxrlulext->pfnInitializeTask)
			{
				hr = (this->*(pxrlulext->pfnInitializeTask))(xrlulextReadData, pxrlasync, pxrlulext->hTaskReadData);
				if (FAILED(hr))
					goto Error;
			}

			pxrlasync->dwCurrentState = xrlulReadData;
		}
	}

Cleanup:
	return(S_OK);

Error:
	// We encountered a hard error. Set next state to done.
	pxrlasync->hrFinal = hr;
	pxrlasync->dwCurrentState = xrlulCleanup;
	goto Cleanup;
}

//
// State 4: Receive final HTTP response loop
//
HRESULT CXo::xrlulReceiveStatusHandler(PXRL_ASYNC pxrlasync)
{
	HRESULT					hr = S_OK;
	DWORD					dwBytesRead;
	BOOL					fDoneHeaders;
	XONLINETASK_HANDLE		hsockio = (XONLINETASK_HANDLE)&(pxrlasync->sockio);
	PXRL_UPLOAD_EXTENSION	pxrlulext = &(pxrlasync->xrlext.ul);

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
			hr = XRL_ProcessHttp(pxrlasync, dwBytesRead, &fDoneHeaders);
			if (SUCCEEDED(hr))
			{
				if (fDoneHeaders)
				{
					// Call extension to process the header information
					hr = (this->*(pxrlulext->pfnResponseHeaders))(pxrlasync);
					if (FAILED(hr))
						goto Error;

				    // Adjust response size to exclude header stuff
					pxrlasync->uliTotalReceived.QuadPart += pxrlasync->dwCurrent;
				}
				else if (!IS_HEADER_MODE(pxrlasync))
				{
					// Track how much response data we received
					pxrlasync->uliTotalReceived.QuadPart += dwBytesRead;
					pxrlasync->dwCurrent = dwBytesRead;
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

		// We will proceed to read data from disk if the following two conditions
		// are true:
		// 1) No longer parsing headers AND
		// 2a) The expected content has been read OR
		// 2b) We already filled up the receive buffer
		if (!IS_HEADER_MODE(pxrlasync) &&
			((pxrlasync->uliTotalReceived.QuadPart >= pxrlasync->uliContentLength.QuadPart) ||
			 (pxrlasync->uliTotalReceived.u.LowPart >= pxrlasync->cbBuffer)))
		{
			// The upload actually finished. Gracefully close the connection
			shutdown(pxrlasync->sockio.socket, SD_BOTH);
		
			// see if there is any post-upload work
			if (pxrlulext->hTaskPostUpload)
			{
				// Make sure we initialize the task using the supplied callback
				if (pxrlulext->pfnInitializeTask)
				{
					hr = (this->*(pxrlulext->pfnInitializeTask))(xrlulextPostUpload, pxrlasync, pxrlulext->hTaskPostUpload);
					if (FAILED(hr))
						goto Error;
				}

				// Got post download work, do that
				pxrlasync->dwCurrentState = xrlulPostUpload;
				goto Cleanup;
			}
			else
			{
				// Nope, jump right to cleanup
				pxrlasync->dwCurrentState = xrlulCleanup;
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
		}
	}

Cleanup:
	return(S_OK);

Error:
	// We encountered a hard error. Set next state to cleanup.
	pxrlasync->hrFinal = hr;
	pxrlasync->dwCurrentState = xrlulCleanup;
	goto Cleanup;
}

//
// State 5: Post-upload extension state
//
HRESULT CXo::xrlulPostUploadHandler(
			PXRL_ASYNC	pxrlasync
			)
{
	HRESULT					hr = S_OK;
	PXRL_UPLOAD_EXTENSION	pxrlulext = &(pxrlasync->xrlext.ul);

    Assert(pxrlulext->hTaskPostUpload != NULL);

	// Simple logic: keep pumping the handle until we are told to 
	// stop, then move on to the connect phase
	hr = XOnlineTaskContinue(pxrlulext->hTaskPostUpload);
	if (XONLINETASK_STATUS_AVAILABLE(hr))
	{
		// The task had returned results, see if it succeeded
		if (FAILED(hr))
			goto Error;

		// Alright, we're all done, jump right to cleanup
		pxrlasync->dwCurrentState = xrlulCleanup;
	}

Cleanup:
	return(S_OK);

Error:
	// We encountered a hard error. Set next state to done.
	pxrlasync->hrFinal = hr;
	pxrlasync->dwCurrentState = xrlulCleanup;
	goto Cleanup;
}

//
// State 6: cleanup
//
HRESULT CXo::xrlulCleanupHandler(PXRL_ASYNC pxrlasync)
{
	// Call the generic handler
	XRL_CleanupHandler(pxrlasync);
	
	// We are done
	pxrlasync->dwCurrentState = xrlulDone;

	return(S_OK);
}

//
// Array
// of handlers for XOnlineUploadFromMemory
//
const PFNXRL_HANDLER CXo::s_rgpfnxrlulHandlers[] =
{
    CXo::xrlulPreConnectHandler,
    CXo::xrlulConnectHandler,
    CXo::xrlulReadDataHandler,
    CXo::xrlulSendDataHandler,
    CXo::xrlulReceiveStatusHandler,
    CXo::xrlulPostUploadHandler,
    CXo::xrlulCleanupHandler
};

//
// Implement the top-level do work function
//
HRESULT CXo::xrlulContinue(XONLINETASK_HANDLE hTask)
{
	return(XRL_MainContinue(hTask, s_rgpfnxrlulHandlers, xrlulDone));
}				

// Function to kick off uploading a memory buffer to the server
HRESULT CXo::Upload(
			LPCSTR			szTargetPath, 
			PBYTE			pbExtraHeaders,
			DWORD			cbExtraHeaders,
			PXRL_ASYNC		pxrlasync
			)
{
	HRESULT					hr = S_OK;
	PXRL_UPLOAD_EXTENSION	pxrlulext = &(pxrlasync->xrlext.ul);

    Assert(NULL != szTargetPath);
    Assert(NULL != pxrlasync);
    Assert(!cbExtraHeaders || pbExtraHeaders);
    Assert(!cbExtraHeaders || cbExtraHeaders < XRL_MAX_EXTRA_HEADER_SIZE);

    // Make sure the required callbacks are supplied
    Assert(NULL != pxrlasync->xrlext.ul.pfnCompleted);
    Assert(NULL != pxrlasync->xrlext.ul.pfnResponseHeaders);
    Assert(NULL != pxrlasync->xrlext.ul.hTaskReadData);

	// Modify nothing except fill in our work function
	pxrlasync->xontask.pfnContinue = xrlulContinue;
	pxrlasync->xontask.pfnClose = XRL_MainClose;

	// The socket API requires that we have an event handle in order
	// for the async I/O to happen. If the caller has not provided us
	// with an event, we have to create it here
	hr = XRL_CreateWorkEventIfNecessary(pxrlasync, NULL);
	if (FAILED(hr))
		goto Error;

	// Make sure the event is set
	SetEvent(pxrlasync->xontask.hEventWorkAvailable);

	// Mark as upload
	pxrlasync->fDownload = FALSE;

	// Determine the next state depending on whether we have
	// any pre-connect work to do ...
	if (pxrlulext->hTaskPreConnect)
	{
		// Make sure we initialize the task using the supplied callback
		if (pxrlulext->pfnInitializeTask)
		{
			hr = (this->*(pxrlulext->pfnInitializeTask))(xrlulextPreConnect, pxrlasync, pxrlulext->hTaskPreConnect);
			if (FAILED(hr))
				goto Error;
		}

    	// Next state is the pre-connect handler
		pxrlasync->dwCurrentState = xrlulPreConnect;
	}
	else
	{
	    // We have no pre-connect work, so just connect to the
	    // service asynchronously
	    hr = XRL_AsyncConnect(pxrlasync);
	    if (FAILED(hr))
    	    goto Error;

    	// Next state is the connect handler
		pxrlasync->dwCurrentState = xrlulConnect;
	}

    // Build the request headers, save the request size (i.e. send
    // buffer size) in pxrlasync->wsabuf for transmission later
	pxrlasync->wsabuf.len = pxrlasync->cbBuffer;
	pxrlasync->wsabuf.buf = (char *)pxrlasync->pBuffer;

    // Build the request headers, we pull most of the data from 
    // pxrlasync.
    //
    // There are cases where the IP address is not known until 
    // after the PreConnect phase. We will allow that. 
    if (pxrlasync->serviceInfo.dwServiceID != XONLINE_INVALID_SERVICE)
    {
	    hr = XRL_BuildPostRequest(
	   				pxrlasync->serviceInfo.dwServiceID,
	    			szTargetPath,
	   				pxrlasync->serviceInfo.serviceIP.s_addr,
	    			(char *)pxrlasync->pBuffer, 
	    			&(pxrlasync->wsabuf.len), 
	    			pbExtraHeaders,
	    			cbExtraHeaders,
	    			pxrlasync->uliFileSize);
	}
	else if (pxrlulext->hTaskPreConnect == NULL)
	{
		AssertSz(FALSE, "Uninitialized Service");
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
// Define the extension functions for XOnlineUploadFromMemory
//

//
// This determines if the entire upload is complete
//
// Note: this is shared between XOnlineUploadFile and 
// XOnlineUploadFromMemory
//
HRESULT CXo::xrlulsCompleted(PVOID pvxrlasync)
{
	PXRL_ASYNC	pxrlasync = (PXRL_ASYNC)pvxrlasync;

	if (pxrlasync->uliTotalWritten.QuadPart >= pxrlasync->uliFileSize.QuadPart)
		return(S_OK);
	return(S_FALSE);
}

//
// This function is called after all headers are received from
// the server final response
//
// Note: this is shared between XOnlineUploadFile and 
// XOnlineUploadFromMemory
//
HRESULT CXo::xrlulsResponseHeaders(PVOID pvxrlasync)
{
#if 0
	PXRL_ASYNC	pxrlasync = (PXRL_ASYNC)pvxrlasync;

	if (pfAbortWithSuccess)
		*pfAbortWithSuccess = FALSE;

	// Hint an abort with success on 400 or 500 level HTTP server error
	if (XONLINEUPLOAD_EXSTATUS_ANYERROR(pxrlasync->dwExtendedStatus) &&
		pfAbortWithSuccess)
		*pfAbortWithSuccess = TRUE;
#endif	
    return(S_OK);
}

HRESULT CXo::xrlufmInitializeTask(XRLUPLOAD_EXTTYPE xrlulext, PVOID pvxrlasync, XONLINETASK_HANDLE hTask)
{
	HRESULT				hr = S_OK;
	PXRL_ASYNC			pxrlasync = (PXRL_ASYNC)pvxrlasync;
	PXRL_ASYNC_EXTENDED	pxrlasyncext = NULL;
	DWORD				dwAvailable;
	DWORD				dwLeft;
	PBYTE				pbSource;
	PBYTE				pbTarget;

	Assert(pxrlasync != NULL);
	Assert(hTask != NULL);

	// XOnlineUploadFromMemory services xrlulReadData
	if (xrlulext == xrlulextReadData)
	{
		// Figure out how much data we can put into the send buffer
		dwAvailable = pxrlasync->cbBuffer - pxrlasync->wsabuf.len;

		// Copy over the data into our send buffer
		pbSource = pxrlasync->fileio.pbBuffer + pxrlasync->dwCurrent;
		pbTarget = (BYTE *)(pxrlasync->wsabuf.buf + pxrlasync->wsabuf.len);
		dwLeft = pxrlasync->fileio.cbBuffer - pxrlasync->dwCurrent;
		
		if (dwLeft < dwAvailable)
			dwAvailable = dwLeft;
		memcpy(pbTarget, pbSource, dwAvailable);

		// Adjust our pointers
		pxrlasync->dwCurrent += dwAvailable;
		pxrlasync->wsabuf.len += dwAvailable;

		// Find the outer containing XRL_ASYNC_EXTENDED
		pxrlasyncext = CONTAINING_RECORD(pxrlasync, XRL_ASYNC_EXTENDED, xrlasync);
		
		// Set next subtask state to complete immediately
		pxrlasyncext->dwCurrentState = xrlufmDone;
	}

	return(hr);
}

//
// Implement the do work function for XOnlineUploadFromMemory subtask
//
HRESULT CXo::xrlufmContinue(XONLINETASK_HANDLE hTask)
{
	PXRL_ASYNC_EXTENDED	pxrlasyncext = NULL;

	// We can make very tight assumptions and validate here
	Assert(hTask != NULL);

	// hTask points to the xontask field of XRL_ASYNC_EXTENDED,
	// find the containing pxrlasyncext structure
	pxrlasyncext = CONTAINING_RECORD(hTask, XRL_ASYNC_EXTENDED, xontask);
	Assert(pxrlasyncext->dwCurrentState == xrlufmDone);

	return(XONLINETASK_S_SUCCESS);
}				

// Function to kick off uploading a memory buffer to the server.
// This is an internal version that takes a context structure 
// instead of allocating a context.
HRESULT CXo::UploadFromMemoryInternal(
			DWORD	                dwServiceID,
			LPCSTR					szTargetPath, 
			PBYTE					pbBuffer,
			DWORD					cbBuffer,
			PBYTE					pbExtraHeaders,
			DWORD					cbExtraHeaders,
			PBYTE					pbDataToUpload,
			DWORD					cbDataToUpload,
			DWORD					dwTimeout,
			HANDLE					hWorkEvent,
			PXRL_ASYNC_EXTENDED		pxrlasyncext
			)
{
	HRESULT					hr = S_OK;
	PXRL_ASYNC				pxrlasync = NULL;
	PXRL_UPLOAD_EXTENSION	pxrlulext = NULL;

    Assert(NULL != szTargetPath);
    Assert(!cbDataToUpload || pbDataToUpload);
    Assert(NULL != pxrlasyncext);
    
	// Initialize our ReadData subtask
	TaskInitializeContext(&pxrlasyncext->xontask);
	pxrlasyncext->xontask.hEventWorkAvailable = hWorkEvent;
	pxrlasyncext->xontask.pfnContinue = xrlufmContinue;
	pxrlasyncext->dwCurrentState = xrlufmDone;

	pxrlasync = &pxrlasyncext->xrlasync;
	pxrlulext = &(pxrlasync->xrlext.ul);

	// Set up the extension block
	pxrlulext->pfnCompleted = xrlulsCompleted;
	pxrlulext->pfnResponseHeaders = xrlulsResponseHeaders;
	pxrlulext->pfnInitializeTask = xrlufmInitializeTask;
	pxrlulext->pfnCleanup = NULL;
	pxrlulext->hTaskPreConnect = NULL;
	pxrlulext->hTaskReadData = 
					(XONLINETASK_HANDLE)&(pxrlasyncext->xontask);;
	pxrlulext->hTaskPostUpload = NULL;

	// Fill in the blanks
	TaskInitializeContext(&pxrlasync->xontask);
	pxrlasync->xontask.hEventWorkAvailable = hWorkEvent;
	pxrlasync->hrFinal = S_OK;
	pxrlasync->hrXErr = S_OK;
	hr = XOnlineGetServiceInfo(dwServiceID, &pxrlasync->serviceInfo);
	Assert(SUCCEEDED(hr));
	pxrlasync->uliTotalWritten.QuadPart = 0;
	pxrlasync->pBuffer = pbBuffer;
	pxrlasync->cbBuffer = cbBuffer;
	pxrlasync->dwCurrent = 0;
	pxrlasync->dwTimeoutMs = dwTimeout;
    
    XoOverrideServiceIP(pxrlasync);

	// We will use fileio to store the data pointers
	pxrlasync->fileio.pbBuffer = pbDataToUpload;
	pxrlasync->fileio.cbBuffer = cbDataToUpload;

	// Set file length for future reference
	pxrlasync->uliFileSize.QuadPart = cbDataToUpload;

	// Just call Upload
	hr = Upload(szTargetPath, 
				pbExtraHeaders, cbExtraHeaders,
				pxrlasync);
	return(hr);				
}

// Function to kick off uploading a memory buffer to the server
HRESULT 
CXo::XOnlineUploadFromMemory(
			DWORD		        dwServiceID,
			LPCSTR				szTargetPath, 
			PBYTE				pbBuffer,
			DWORD				*pcbBuffer, 
			PBYTE				pbExtraHeaders,
			DWORD				cbExtraHeaders,
			PBYTE				pbDataToUpload,
			DWORD				cbDataToUpload,
			DWORD				dwTimeout,
			HANDLE				hWorkEvent,
			XONLINETASK_HANDLE	*phTask
			)
{
    XoEnter("XOnlineUploadFromMemory");
    XoCheck(szTargetPath != NULL);
    XoCheck(pcbBuffer != NULL);
    XoCheck(!cbDataToUpload || pbDataToUpload);
    XoCheck(phTask != NULL);

	HRESULT					hr = S_OK;
	PXRL_ASYNC_EXTENDED		pxrlasyncext = NULL;
	PXRL_ASYNC				pxrlasync = NULL;
	PXRL_UPLOAD_EXTENSION	pxrlulext = NULL;
	DWORD					cbBuffer = *pcbBuffer;

	// We are responsible for allocating the context and the work buffer
	// The work buffer are the specified bytes immediately following the
	// context structure.	
	pxrlasyncext = (PXRL_ASYNC_EXTENDED)SysAlloc(sizeof(XRL_ASYNC_EXTENDED) + (pbBuffer?0:cbBuffer),
        PTAG_XONLINETASK_UPLOAD_MEMORY);
	if (!pxrlasyncext)
	{
		hr = E_OUTOFMEMORY;
		goto Error;
	}
	ZeroMemory(pxrlasyncext, sizeof(XRL_ASYNC_EXTENDED));
	
	hr = UploadFromMemoryInternal(
				dwServiceID,
				szTargetPath,
				pbBuffer?pbBuffer:(PBYTE)(pxrlasyncext + 1),
				cbBuffer,
				pbExtraHeaders,
				cbExtraHeaders,
				pbDataToUpload,
				cbDataToUpload,
				dwTimeout,
				hWorkEvent,
				pxrlasyncext);
	if (FAILED(hr))
    {
    	if (hr == HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER))
    		*pcbBuffer = pxrlasyncext->xrlasync.wsabuf.len;
		goto Error;
	}

	// Return the context as an opaque handle
	*phTask = (XONLINETASK_HANDLE)pxrlasyncext;

Cleanup:	
	return(XoLeave(hr));

Error:

	// Failed setup, make sure we clean up everything
	if (pxrlasyncext)
		SysFree(pxrlasyncext);
	goto Cleanup;
}

//
// Define the extension functions for XOnlineUploadFile
//

HRESULT CXo::xrlufInitializeTask(
			XRLUPLOAD_EXTTYPE	xrlulext,
			PVOID				pvxrlasync,
			XONLINETASK_HANDLE	hTask
			)
{
	HRESULT				hr = S_OK;
	PXRL_ASYNC			pxrlasync = (PXRL_ASYNC)pvxrlasync;
	PXRL_ASYNC_EXTENDED	pxrlasyncext = NULL;
	DWORD				dwAvailable;
	DWORD				dwStart;

	Assert(pxrlasync != NULL);
	Assert(hTask != NULL);

	// XOnlineUploadFile services xrlulextReadData
	if (xrlulext == xrlulextReadData)
	{
		// Find the starting position for the file read. It's not as simple
		// as pxrlasync->wsabuf.len because the buffer for file read must be 
		// DWORD aligned. So we round it up to the next DWORD boundary.
		//
		// Note: this might leave a gap between the end of the HTTP headers and
		// the actual data. We will fix this up in xrlufReadDataHandler().
		dwStart = (pxrlasync->wsabuf.len + (sizeof(DWORD) - 1)) & ~(sizeof(DWORD) - 1);

		// Figure out how much data we can put into the send buffer
		dwAvailable = pxrlasync->cbBuffer - dwStart;

		// Round that down to an integer multiple of the sector size
		dwAvailable = SECTOR_ALIGNMENT_ROUND_DOWN(dwAvailable);

		// Find the outer containing XRL_ASYNC_EXTENDED
		pxrlasyncext = CONTAINING_RECORD(pxrlasync, XRL_ASYNC_EXTENDED, xrlasync);

		// Read data
		ReadWriteFileInitializeContext(pxrlasync->fileio.hFile,
					pxrlasync->pBuffer + dwStart, dwAvailable, 
					pxrlasync->uliTotalReceived, 
					pxrlasync->xontask.hEventWorkAvailable, &pxrlasync->fileio);

		hr = ReadFileInternal(&pxrlasync->fileio);
		if (FAILED(hr))
			goto Error;

		// Set next subtask state to read data
		pxrlasyncext->dwCurrentState = xrlufReadData;
	}

Error:
	return(hr);
}

//
// State xrlufReadData: Read file loop
//
HRESULT CXo::xrlufReadDataHandler(
			PXRL_ASYNC_EXTENDED	pxrlasyncext
			)
{
	HRESULT				hr = S_OK;
	DWORD				dwBytesRead;
	DWORD				dwShift;
	PBYTE				pbMove;
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
		hr = GetReadWriteFileResults(hfileio, &dwBytesRead, NULL);
		if (FAILED(hr))
			goto Error;

		// Track how much we have read so far
		pxrlasync->uliTotalReceived.QuadPart += dwBytesRead;

		// Fix up any gaps in the data
		dwShift = (pxrlasync->wsabuf.len + (sizeof(DWORD) - 1)) & ~(sizeof(DWORD) - 1);
		if (dwShift != pxrlasync->wsabuf.len)
		{
			// We need to shift the data up a few bytes to cover up the gap
			dwShift -= pxrlasync->wsabuf.len;
			pbMove = (BYTE *)(pxrlasync->wsabuf.buf + pxrlasync->wsabuf.len);

			// Make sure we use MoveMemory here because the move regions 
			// are almost certain to be overlapping
			MoveMemory(pbMove, pbMove + dwShift, dwBytesRead);
		}

		// Send out whatever we read
		pxrlasync->wsabuf.buf = (char *)pxrlasync->pBuffer;
		pxrlasync->wsabuf.len += dwBytesRead;

		// End this subtask
		pxrlasyncext->dwCurrentState = xrlufDone;
	}

Cleanup:
	return(S_OK);

Error:
	// We encountered a hard error. Set next state to cleanup.
	pxrlasync->hrFinal = hr;
	pxrlasyncext->dwCurrentState = xrlufDone;
	goto Cleanup;
}

//
// Implement the do work function for XOnlineUploadFile subtask
//
HRESULT CXo::xrlufContinue(
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

	// Only one possible state ...
	Assert(pxrlasyncext->dwCurrentState == xrlufReadData);

	if (pxrlasyncext->dwCurrentState == xrlufReadData)
		hr = xrlufReadDataHandler(pxrlasyncext);
	else
	{
		// This is a bad state. We force it to end so we don't get stuck
		// here forever.
		AssertSz(FALSE, "xrlufContinue: invalid current state!");
		pxrlasyncext->dwCurrentState = xrlufDone;
		pxrlasync->hrFinal = E_FAIL;
	}

	// When the next state is the final state, we set the flag so we don't
	// continue.
	if (pxrlasyncext->dwCurrentState == xrlufDone)
	{
		// Indicate that we are done and return the final code
		hr = pxrlasync->hrFinal;
		XONLINETASK_SET_COMPLETION_STATUS(hr);
	}
	
	return(hr);
}				

// Function to kick off uploading a file to the server
HRESULT 
CXo::XOnlineUploadFile(
			DWORD		    dwServiceID,
			LPCSTR			szTargetPath, 
			PBYTE			pbBuffer,
			DWORD			*pcbBuffer, 
			PBYTE			pbExtraHeaders,
			DWORD			cbExtraHeaders,
			LPCSTR			szLocalPath, 
			DWORD			dwTimeout,
			HANDLE			hWorkEvent,
			XONLINETASK_HANDLE	*phTask
			)
{
    XoEnter("XOnlineUploadFile");
    XoCheck(szTargetPath != NULL);
    XoCheck(pcbBuffer != NULL);
    XoCheck(*pcbBuffer >= XBOX_HD_SECTOR_SIZE);
    XoCheck(szLocalPath != NULL);
    XoCheck(phTask != NULL);

	HRESULT					hr = S_OK;
	PXRL_ASYNC_EXTENDED		pxrlasyncext = NULL;
	PXRL_ASYNC				pxrlasync = NULL;
	PXRL_UPLOAD_EXTENSION	pxrlulext = NULL;
	LARGE_INTEGER			liFileSize;
	PXONLINETASK_FILEIO		pfileio = NULL;
	DWORD					cbBuffer = *pcbBuffer;

	// The buffer size needs more validation:
	// 1) Sector size must be power of 2
	// 2) The size will be rounded down to multiples of Xbox HD sector size
    Assert((XBOX_HD_SECTOR_SIZE & (XBOX_HD_SECTOR_SIZE - 1)) == 0);
    cbBuffer &= ~(XBOX_HD_SECTOR_SIZE - 1);

	// We are responsible for allocating the context and the work buffer
	// The work buffer are the specified bytes immediately following the
	// context structure.	
	pxrlasyncext = (PXRL_ASYNC_EXTENDED)SysAlloc(sizeof(XRL_ASYNC_EXTENDED) + (pbBuffer?0:cbBuffer),
        PTAG_XONLINETASK_UPLOAD_FILE);
	if (!pxrlasyncext)
	{
		hr = E_OUTOFMEMORY;
		goto Error;
	}
	
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
	pxrlasyncext->xontask.pfnContinue = xrlufContinue;
	pxrlasyncext->xontask.hEventWorkAvailable = hWorkEvent;
	pxrlasyncext->dwCurrentState = xrlufDone;

	// Set up the extension block
	pxrlulext = &(pxrlasync->xrlext.ul);
	pxrlulext->pfnCompleted = xrlulsCompleted;
	pxrlulext->pfnResponseHeaders = xrlulsResponseHeaders;
	pxrlulext->pfnInitializeTask = xrlufInitializeTask;
	pxrlulext->pfnCleanup = NULL;
	pxrlulext->hTaskPreConnect = NULL;
	pxrlulext->hTaskReadData = 
					(XONLINETASK_HANDLE)&(pxrlasyncext->xontask);;
	pxrlulext->hTaskPostUpload = NULL;

	// Open the source file with overlapped access and no buffering
	pfileio->hFile = CreateFile(
				szLocalPath, 
				GENERIC_READ, 
				FILE_SHARE_READ, 
				NULL, 
				OPEN_EXISTING, 
				FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN |
				FILE_FLAG_OVERLAPPED | FILE_FLAG_NO_BUFFERING, 
				NULL);
	if (pfileio->hFile == INVALID_HANDLE_VALUE)
	{
		hr = HRESULT_FROM_WIN32(GetLastError());
		goto Error;
	}

	// Get the size of the file
	if (!GetFileSizeEx(pfileio->hFile, &liFileSize))
	{
		hr = HRESULT_FROM_WIN32(GetLastError());
		goto Error;
	}

	// Save the file size
	pxrlasync->uliFileSize.QuadPart = liFileSize.QuadPart;

	// Just call Upload
	hr = Upload(szTargetPath, 
				pbExtraHeaders, cbExtraHeaders,
				pxrlasync);
	if (FAILED(hr))
    {
    	if (hr == HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER))
    		*pcbBuffer = pxrlasync->wsabuf.len;
		goto Error;
	}

	// Return the context as an opaque handle
	*phTask = (XONLINETASK_HANDLE)pxrlasync;

Cleanup:	
	return(XoLeave(hr));

Error:

	// Failed setup, make sure we clean up everything
	if (pxrlasyncext)
		SysFree(pxrlasyncext);
	if (pfileio->hFile && (pfileio->hFile != INVALID_HANDLE_VALUE))
		CloseHandle(pfileio->hFile);
	goto Cleanup;
}

// Function to get the download progress
HRESULT
CXo::XOnlineUploadGetProgress(
			XONLINETASK_HANDLE	hTask,
			DWORD				*pdwPercentDone,
			ULARGE_INTEGER		*puliNumerator,
			ULARGE_INTEGER		*puliDenominator
			)
{
    XoEnter("XOnlineUploadGetProgress");
    XoCheck(hTask != NULL);

	PXRL_ASYNC	pxrlasync = (PXRL_ASYNC)hTask;

	if (puliNumerator)
		puliNumerator->QuadPart = pxrlasync->uliTotalWritten.QuadPart;
	if (puliDenominator)
		puliDenominator->QuadPart = pxrlasync->uliFileSize.QuadPart;
	if (pdwPercentDone)
	{
		if (pxrlasync->uliFileSize.QuadPart != 0)
		{
			*pdwPercentDone = 
				(DWORD)((pxrlasync->uliTotalWritten.QuadPart * 100)/
					pxrlasync->uliFileSize.QuadPart);
			if (*pdwPercentDone > 100)
				*pdwPercentDone = 100;
		}
		else
			*pdwPercentDone = 0;
	}

	return(XoLeave(S_OK));
}

// Helper function to return the results of an upload
HRESULT 
CXo::XOnlineUploadGetResults(
			XONLINETASK_HANDLE	hTask,
			LPBYTE				*ppbBuffer,
			DWORD				*pcbBuffer,
			ULARGE_INTEGER		*puliTotalReceived,
			ULARGE_INTEGER		*puliContentLength,
			DWORD				*pdwExtendedStatus,
			FILETIME			*pftTimestamp
			)
{
    XoEnter("XOnlineUploadGetResults");
    XoCheck(hTask != NULL);

	PXRL_ASYNC pxrlasync = (PXRL_ASYNC)hTask;

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

	return(XoLeave(pxrlasync->hrFinal));
}

