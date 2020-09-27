/*++

Copyright (c) Microsoft Corporation

Author:
    Keith Lau (keithlau), Ben Lewis (t-blewis)

Description:
	Module implementing content enumeration

Module Name:

    contenum.c

--*/

#include "xonp.h"
#include "xonver.h"

//
// Define the search path for title enumeration
//
#define XONLINECONTENT_TITLES_SEARCH_PATH	"\\\\.\\c:\\tdata\\*.*"

//
// Define a macro to do offset to pointer conversion
//
#define CONVERT_IN_PLACE(pitem, citem, type)	\
		{\
			DWORD dwOffset;\
			dwOffset = (DWORD)(pitem);\
			if (dwOffset)\
			{\
				if ((dwOffset > cbRecord) ||\
					((dwOffset + (citem * sizeof(type))) > (cbRecord + 1)))\
					goto InvalidData;\
				pitem = (type *)((PBYTE)prechdr + dwOffset);\
			}\
			else\
				pitem = NULL;\
		}

//
// Function to validate and relocate a single metadata record
//
// The validation is mainly length validation and does not really 
// include checking to see the values are 'good'. Relocation refers
// to the process of converting relative offsets within the structure
// to memory pointers.
//
// IMPORTANT: The following code assumes the following:
//    sizeof(DWORD) == sizeof(WCHAR *)
// 
// If we decide to upgrade to a 64-bit platform, we will need to
// fix this.
//
HRESULT CXo::ValidateAndRelocateCatalogRecord(
			PXONLINEOFFERING_ENUM_RESP_RECORD	prechdr,
			DWORD								cbRecord
			)
{
	PXONLINEOFFERING_INFO	pInfo;
	
	// Sanity check of the record size
	if (cbRecord < sizeof(XONLINEOFFERING_ENUM_RESP_RECORD))
		goto InvalidData;

	// Do in-place swap from relative offsets to pointers
	pInfo = &(prechdr->OfferingInfo);
	CONVERT_IN_PLACE(pInfo->pbTitleSpecificData, 
				pInfo->cbTitleSpecificData, BYTE);
	return(S_OK);

InvalidData:
	return(HRESULT_FROM_WIN32(ERROR_INVALID_DATA));
}

//
// Function to parse offering enumeration results
//
HRESULT CXo::ParseEnumerationResults(
			PXONLINETASK_OFFERING_ENUM	pcontenum
			)
{
	HRESULT						hr = S_OK;
	PBYTE						pbBuffer;
	DWORD						cbReceived;
	DWORD						cbCurrent = 0;
	DWORD						cbRecord;
	DWORD						iIndex = 0;
	DWORD						dwOffset;
	PXRL_ASYNC					pxrlasync;
	PXONLINEOFFERING_INFO		*rgInfoSlot;
	PXONLINEOFFERING_INFO		pInfo;
	PXONLINEOFFERING_ENUM_SERVER		penum;
	PXONLINEOFFERING_ENUM_REQ			penumreq;
	PXONLINEOFFERING_ENUM_RESP_HEADER	presphdr;
	PXONLINEOFFERING_ENUM_RESP_RECORD	prechdr;

	Assert(pcontenum != NULL);

	penum = &(pcontenum->enumdata.server);
	penumreq = &(penum->enumreq);
	pxrlasync = &(penum->xrlasyncext.xrlasync);

	pcontenum->dwResults = 0;
	pbBuffer = pxrlasync->pBuffer;
	cbReceived = pxrlasync->dwCurrent;

	// If content length is zero, then there are zero results
	if (!cbReceived)
	{
		if (pxrlasync->uliContentLength.QuadPart == 0)
			goto Cleanup;
		else
			goto InvalidData;
	}

	// Sanity check
	if ((cbReceived < sizeof(XONLINEOFFERING_ENUM_RESP_HEADER)) ||
		(cbReceived > pxrlasync->cbBuffer))
		goto InvalidData;

	// Initialize our pointers
	presphdr = (PXONLINEOFFERING_ENUM_RESP_HEADER)pbBuffer;
	cbCurrent = sizeof(XONLINEOFFERING_ENUM_RESP_HEADER);
	pbBuffer += cbCurrent;
	prechdr = (PXONLINEOFFERING_ENUM_RESP_RECORD)pbBuffer;

	// Verify information
	if (cbReceived != presphdr->Header.cbSize)
		goto InvalidData;
	if (presphdr->cRecords > penumreq->Params.dwMaxResults)
		goto InvalidData;

	// Find the result vector
	rgInfoSlot = (PXONLINEOFFERING_INFO *)(pcontenum + 1);

	//
	// Process records
	//
	while (iIndex < presphdr->cRecords)
	{
		if (presphdr->Header.cbSize < 
				(cbCurrent + sizeof(XONLINEOFFERING_ENUM_RESP_RECORD)))
			goto InvalidData;

		cbRecord = prechdr->cbRecordSize;
		cbCurrent += cbRecord;
		if (presphdr->Header.cbSize < cbCurrent)
			goto InvalidData;

		// Sanity check of the record size
		if (cbRecord < sizeof(XONLINEOFFERING_ENUM_RESP_RECORD))
			goto InvalidData;
			
		// Fill in the result vector
		pInfo = &(prechdr->OfferingInfo);
		rgInfoSlot[iIndex] = pInfo;

		// Validate and relocate record
		hr = ValidateAndRelocateCatalogRecord(prechdr, cbRecord);
		if (FAILED(hr))
			goto Error;
		
		// Move to the next record
		iIndex++;
		pbBuffer += cbRecord;
		prechdr = (PXONLINEOFFERING_ENUM_RESP_RECORD)pbBuffer;
	}

	// Keep track of the last offering ID parsed so for 
	// continuation requests
	penum->OfferingIdLast = pInfo->OfferingId;

	// Mark how many items are returned
	pcontenum->dwResults = iIndex;

	// Finally, figure out if we have more results
	pcontenum->fPartial = ((presphdr->fFlags & XONLINEOFFERING_RESP_FLAG_MORE_RESULTS) != 0);

Cleanup:
	return(hr);

Error:
	goto Cleanup;

InvalidData:
	hr = HRESULT_FROM_WIN32(ERROR_INVALID_DATA);
	goto Error;
}

//
// Implement the do work function for enumeration post-upload subtask
//
HRESULT CXo::contenumPostUploadContinue(
			XONLINETASK_HANDLE	hTask
			)
{
	HRESULT								hr;
	PXONLINETASK_OFFERING_ENUM			pcontenum;
	PXONLINEOFFERING_ENUM_SERVER		penum;
	PXONLINEOFFERING_ENUM_RESP_HEADER	pHeader;

	// We can make very tight assumptions and validate here
	Assert(hTask != NULL);

	// hTask points to the xontaskPostUpload field of XONLINEOFFERING_ENUM_SERVER,
	// find the containing XONLINEOFFERING_ENUM_SERVER structure
	penum = CONTAINING_RECORD(hTask, XONLINEOFFERING_ENUM_SERVER, xontaskPostUpload);
	pcontenum = CONTAINING_RECORD(penum, XONLINETASK_OFFERING_ENUM, enumdata);

	// Just skip this if the HTTP response is a failure
	if (XONLINEUPLOAD_EXSTATUS_ANYERROR(
				penum->xrlasyncext.xrlasync.dwExtendedStatus))
	{
		return(penum->xrlasyncext.xrlasync.hrXErr);
	}
	else
	{
		// Parse and validate the enumeration data here; this 
		// handles the case where there are no results.
		hr = ParseEnumerationResults(pcontenum);
		if (FAILED(hr))
			return(hr);
	}

	return(XONLINETASK_S_SUCCESS);
}				

//
// Function to kick off an enumeration request given an
// XONLINETASK_CONTENT_ENUM structure that is filled in.
//
HRESULT CXo::InitiateEnumerationRequest(
			PXONLINEOFFERING_ENUM_SERVER	penum,
			PBYTE							pbBuffer,
			DWORD							cbBuffer,
			HANDLE							hWorkEvent
			)
{
	HRESULT						hr;
	PXRL_ASYNC_EXTENDED			pxrlasyncext;
	PXONLINETASK_CONTEXT		pxontaskPost;
	PXONLINETASK_CONTEXT		pxontaskEnum;
	PXONLINEOFFERING_ENUM_REQ	penumreq;
	XONLINE_SERVICE_INFO		serviceCatref;
	CHAR						szContentType[XONLINE_MAX_CONTENT_TYPE_SIZE];
	DWORD						cbContentType;

	pxrlasyncext = &(penum->xrlasyncext);
	pxontaskEnum = &(penum->xontaskEnum);
	pxontaskPost = &(penum->xontaskPostUpload);
	penumreq = &(penum->enumreq);


	// Set up enum task context
	TaskInitializeContext(pxontaskEnum);
	pxontaskEnum->hEventWorkAvailable = hWorkEvent;
	pxontaskEnum->pfnContinue = contenumContinue;
	pxontaskEnum->pfnClose    = (PFNXONLINE_TASK_CLOSE)contenumClose;

	// Set up the enumeration post-upload task context
	TaskInitializeContext(pxontaskPost);
	pxontaskPost->hEventWorkAvailable = hWorkEvent;
	pxontaskPost->pfnContinue = contenumPostUploadContinue;

	// Build the content type header
	cbContentType = sprintf(szContentType, 
				"Content-type: xon/%x\r\n", XONLINE_BILLING_OFFERING_SERVICE);
				
	// The enumeration is actually a POST request. Just call
	// UploadFromMemoryInternal
	hr = UploadFromMemoryInternal(
				XONLINE_BILLING_OFFERING_SERVICE,
				XONLINEOFFERING_ENUMERATE_XRL,
				pbBuffer,
				cbBuffer,
				(PBYTE)szContentType,
				cbContentType,
				(PBYTE)penumreq,
				penumreq->Header.cbSize,
				XONLINEOFFERING_ENUMERATE_TIMEOUT,
				hWorkEvent,
				pxrlasyncext);
	if (FAILED(hr))
		return(hr);

	// Minor pre-meditated hack: essentially, an enumeration request
	// is almost the same as a plain upload, except that we want to
	// install a post-upload task to parse and validate the results.
	// 
	// I didn't want to duplicate most of the code here, so I decided
	// to call UploadFromMemoryInternal, and add a post-upload
	// task here.
	pxrlasyncext->xrlasync.xrlext.ul.hTaskPostUpload = 
				(XONLINETASK_HANDLE)pxontaskPost;

	return(S_OK);
}

//
// Implement the do work function for enumeration
//
HRESULT CXo::contenumContinue(
			XONLINETASK_HANDLE	hTask
			)
{
	HRESULT							hr;
	PXONLINETASK_OFFERING_ENUM		pcontenum;
	PXONLINEOFFERING_ENUM_SERVER	penum;
	PXRL_ASYNC						pxrlasync;

	// We can make very tight assumptions and validate here
	Assert(hTask != NULL);

	pcontenum = (PXONLINETASK_OFFERING_ENUM)hTask;
	penum = &(pcontenum->enumdata.server);
	pxrlasync = &(penum->xrlasyncext.xrlasync);

	switch (penum->State)
	{
	case enumsrvUpload:

		// We are in the middle of an upload, let's pump the upload task
		hr = XOnlineTaskContinue(
					(XONLINETASK_HANDLE)&(penum->xrlasyncext));
		if (XONLINETASK_STATUS_AVAILABLE(hr))
		{
			// If we hit a hard failure, then just abort
			if (FAILED(hr))
				goto Exception;
		
			// See if the upload succeeded
			if (XONLINETASK_STATUS_SUCCESSFUL_COMPLETION(hr))
			{
				// Yes, this upload succeeded. If the fPartial flag is
				// set (by contenumPostUploadContinue), then we know we have 
				// more results and might have to post another upload to 
				// get them. If not, then we are done
				if (!pcontenum->fPartial)
					goto Done;

				// Set next state to enumsrvResults phase and return 
				// special exception code. We will stay in this phase 
				// until the title calls XOnlineOfferingEnumerateGetResults.
				penum->State = enumsrvResults;
				hr = XONLINETASK_S_RESULTS_AVAIL;
				goto Exception;
			}

			// XRL stuff should not return any intermediate exceptions.
			// Just to be safe we will Assert if we get here
			AssertSz(FALSE, "contenumContinue: unexpected XRL result");
		}
		break;

	case enumsrvResults:

		// We will not get out of this state until the caller calls 
		// XOnlineOfferingEnumerateGetResults.
		hr = XONLINETASK_S_RESULTS_AVAIL;
		goto Exception;

	case enumsrvContinue:

		// We have more results to gather. Kick off another upload
		// Place the last encountered offering ID in the resume slot
		penum->enumreq.ResumePosition = penum->OfferingIdLast;

		pcontenum->fPartial = FALSE;

		// Initiate the enumeration request
		hr = InitiateEnumerationRequest(
					penum, 
					pxrlasync->pBuffer, 
					pxrlasync->cbBuffer, 
					pxrlasync->xontask.hEventWorkAvailable);
		if (FAILED(hr))
			goto Exception;

		// Set the state back to upload
		penum->State = enumsrvUpload;
		goto Exit;

	case enumsrvDone:
		goto Done;

	default:
		AssertSz(FALSE, "contdlContinue: invalid state!");
		hr = E_FAIL;
		goto Exception;
	}

Exit:	
	return(XONLINETASK_S_RUNNING);

Done:
	return(XONLINETASK_S_SUCCESS);

Exception:
	return(hr);
}				

//
// Implement the close function for enumeration
//
HRESULT CXo::contenumClose(
			XONLINETASK_HANDLE	hTask
			)
{
	PXONLINETASK_OFFERING_ENUM		pcontenum;
	PXONLINEOFFERING_ENUM_SERVER	penum;

	// We can make very tight assumptions and validate here
	Assert(hTask != NULL);

	pcontenum = (PXONLINETASK_OFFERING_ENUM)hTask;
	penum = &(pcontenum->enumdata.server);
	
	// The upload task controls all resources, so turn around and
	// close the upload task, then free ourself
	XRL_CleanupHandler(&(penum->xrlasyncext.xrlasync));
	SysFree(hTask);
	
	return(S_OK);
}

HRESULT CXo::ContentEnumerateServerBegin(
			DWORD							dwTitleId,
			DWORD							dwUserIndex,
			PXONLINEOFFERING_ENUM_PARAMS	pEnumParams,
			PBYTE							pbBuffer,
			DWORD							cbBuffer,
			HANDLE							hWorkEvent,
			PXONLINETASK_OFFERING_ENUM		pcontenum
			)
{
	HRESULT							hr = S_OK;
	PXONLINE_USER					pUsers;
	PXONLINEOFFERING_ENUM_SERVER	penum;
	PXONLINEOFFERING_ENUM_REQ		penumreq;

	Assert(pcontenum != NULL);
	Assert(pbBuffer != NULL);
	Assert(pEnumParams != NULL);
        Assert(dwUserIndex < XONLINE_MAX_LOGON_USERS);

	// Get the server enumeration data context
	penum = &(pcontenum->enumdata.server);
	penumreq = &(penum->enumreq);


	// Set up the enumeration request message
	penumreq->Header.cbSize = sizeof(XONLINEOFFERING_ENUM_REQ);
	penumreq->Header.dwReserved = 0;

	penumreq->dwTitleId = dwTitleId;
	penumreq->cbClientBuffer = cbBuffer;
	penumreq->dwRating = XGetParentalControlSetting();
	penumreq->ResumePosition = 0;	// Begin new enumeration

	// Get the PUID
	pUsers = XOnlineGetLogonUsers();
	if (pUsers)
	{
		penumreq->Puid = pUsers[dwUserIndex].xuid.qwUserID;

		// Make sure the user is not a guest
		if (XOnlineIsUserGuest(pUsers[dwUserIndex].xuid.dwUserFlags))
		{
			hr = XONLINE_E_NO_GUEST_ACCESS;
			goto Exit;
		}
	}
	else
		penumreq->Puid = 0;

	// Copy the enum params over
	memcpy(&(penumreq->Params), pEnumParams, sizeof(XONLINEOFFERING_ENUM_PARAMS));
	
	// Initiate the enumeration request
	hr = InitiateEnumerationRequest(penum, pbBuffer, cbBuffer, hWorkEvent);

Exit:	
	return(hr);
}

//
// Implement the OnData do work function for Local enumeration 
//
HRESULT CXo::ContentEnumerateLocalContinue(
			XONLINETASK_HANDLE	hTask
			)
{
	PXONLINETASK_OFFERING_ENUM	pcontenum;
	PXONLINEOFFERING_ENUM_LOCAL	plocalenum;
	PXONLINEOFFERING_INFO		*rgInfoSlot;
	PXONLINEOFFERING_INFO		pInfo;

	// We can make very tight assumptions and validate here
	Assert(hTask != NULL);

	// hTask points to the xontaskPostUpload field of XONLINECONTENT_ENUM_HD,
	// find the containing XONLINECONTENT_ENUM_HD structure
	plocalenum = CONTAINING_RECORD(hTask, XONLINEOFFERING_ENUM_LOCAL, xontask);
	pcontenum = CONTAINING_RECORD(plocalenum, XONLINETASK_OFFERING_ENUM, enumdata);

	// If we are continuing to a new set of records, reset the counter
	if (pcontenum->fPartial)
	{
		pcontenum->fPartial = FALSE;
		pcontenum->dwResults = 0;
	}

	rgInfoSlot = (PXONLINEOFFERING_INFO *)(pcontenum + 1);
	pInfo = ((PXONLINEOFFERING_INFO)plocalenum->pbBuffer) + pcontenum->dwResults;
	ZeroMemory(pInfo, sizeof(XONLINEOFFERING_INFO));

	// Make sure this is a valid content directory name
	if (SUCCEEDED(ParseContentDirName(plocalenum->szPath, 
					&(pInfo->OfferingId), &(pInfo->dwBitFlags))))
	{
		// Now apply the bit filter to see if this is a match
		if ((!plocalenum->dwBitFilter) ||
			((plocalenum->dwBitFilter & pInfo->dwBitFlags) != 0))
		{
			// This one's a match, so save it
			rgInfoSlot[pcontenum->dwResults++] = pInfo;

			// See if we have reached our capacity yet
			if ((pcontenum->dwResults >= pcontenum->dwMaxResults) ||
				(((pcontenum->dwResults + 1) * sizeof(XONLINEOFFERING_INFO)) > plocalenum->cbBuffer))
			{
				// Indicate a set of partial results is available to the caller
				pcontenum->fPartial = TRUE;
				goto ResultsAvail;
			}
		}
	}
	
	return(XONLINETASK_S_SUCCESS);

ResultsAvail:	
	return(XONLINETASK_S_RESULTS_AVAIL);
}	

//
// Implement the OnDone handler for Local enumeration 
//
HRESULT CXo::ContentEnumerateLocalDone(
			XONLINETASK_HANDLE	hTask
			)
{
	PXONLINETASK_OFFERING_ENUM	pcontenum;
	PXONLINEOFFERING_ENUM_LOCAL	plocalenum;

	Assert(hTask != NULL);

	// hTask points to the xontaskPostUpload field of XONLINECONTENT_ENUM_HD,
	// find the containing XONLINECONTENT_ENUM_HD structure
	plocalenum = CONTAINING_RECORD(hTask, XONLINEOFFERING_ENUM_LOCAL, xontaskDone);
	pcontenum = CONTAINING_RECORD(plocalenum, XONLINETASK_OFFERING_ENUM, enumdata);

	// If we are continuing to a new set of records, reset the counter
	if (pcontenum->fPartial)
	{
		pcontenum->fPartial = FALSE;
		pcontenum->dwResults = 0;
	}

	// This task must return success immediately
	return(XONLINETASK_S_SUCCESS);
}

HRESULT CXo::ContentEnumerateLocalBegin(
			PSTR							szRootPath,
			PXONLINEOFFERING_ENUM_PARAMS	pEnumParams,
			PBYTE							pbBuffer,
			DWORD							cbBuffer,
			HANDLE							hWorkEvent,
			PXONLINETASK_OFFERING_ENUM		pcontenum
			)
{
	HRESULT						hr = S_OK;
	PXONLINEOFFERING_ENUM_LOCAL	plocalenum;
	DWORD						cbPath;
	PSTR						szPath;

	// There's a minimum buffer requirement here
	Assert(pcontenum != NULL);
	Assert(pEnumParams != NULL);
    Assert(cbBuffer >= (sizeof(XONLINEOFFERING_INFO) + XBOX_HD_SECTOR_SIZE));

	// Set up the specific search path for titles
	cbPath = strlen(szRootPath);
	if (cbPath >= XONLINECONTENT_MAX_PATH)
	{
		hr = HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);
		goto Exit;
	}
	
	// Initialize plocalenum
	plocalenum = &(pcontenum->enumdata.local);
	szPath = plocalenum->szPath;
	strcpy(szPath, szRootPath);
	plocalenum->pbBuffer = pbBuffer;
	plocalenum->cbBuffer = cbBuffer;
	plocalenum->dwBitFilter = pEnumParams->dwBitFilter;

	// Initialize the OnData task
	TaskInitializeContext(&plocalenum->xontask);
	plocalenum->xontask.pfnContinue = ContentEnumerateLocalContinue;
	plocalenum->xontask.hEventWorkAvailable = hWorkEvent;

	// Initialize the OnDone task
	TaskInitializeContext(&plocalenum->xontaskDone);
	plocalenum->xontaskDone.pfnContinue = ContentEnumerateLocalDone;
	plocalenum->xontaskDone.hEventWorkAvailable = hWorkEvent;

	if (hWorkEvent)
		SetEvent(hWorkEvent);

	// Kick off the directory enumeration
	hr = EnumerateDirectoryInitializeContext(
				szPath,
				cbPath,
				XONLINECONTENT_MAX_PATH-1,
				XONLINETASK_EDFLAG_ENUMERATE_DIRS,
				(XONLINETASK_HANDLE)&(plocalenum->xontask),
				(XONLINETASK_HANDLE)&(plocalenum->xontaskDone),
				hWorkEvent, 
				&(plocalenum->dirops));
	if (FAILED(hr))
		goto Exit;

Exit:
	return(hr);
}

HRESULT 
CXo::XOnlineOfferingEnumerate(
			XONLINEOFFERING_ENUM_DEVICE		edDevice,
			DWORD							dwUserIndex,
			PXONLINEOFFERING_ENUM_PARAMS	pEnumParams,
			PBYTE							pbBuffer,
			DWORD							cbBuffer,
			HANDLE							hWorkEvent,
			XONLINETASK_HANDLE				*phTask
			)
{
    XoEnter("XOnlineOfferingEnumerate");
    XoCheck(edDevice < XONLINEOFFERING_ENUM_DEVICE_MAX);
	XoCheck(pEnumParams != NULL);
	XoCheck(cbBuffer != 0);
	XoCheck(phTask != NULL);

	HRESULT						hr = S_OK;
	DWORD						cbResultVector;
	PXONLINETASK_OFFERING_ENUM	pcontenum;
	CHAR						szTemp[XONLINECONTENT_MAX_PATH];

	// We are responsible for allocating the context and the work buffer
	// The work buffer are the specified bytes immediately following the
	// context structure
	cbResultVector = sizeof(PXONLINEOFFERING_INFO *) * pEnumParams->dwMaxResults;
	pcontenum = (PXONLINETASK_OFFERING_ENUM)SysAlloc(sizeof(XONLINETASK_OFFERING_ENUM) +
        cbResultVector + (pbBuffer?0:cbBuffer), PTAG_XONLINETASK_OFFERING_ENUM);
	if (!pcontenum)
	{
		hr = E_OUTOFMEMORY;
		goto Error;
	}

	// Initialize the context
	ZeroMemory(pcontenum, sizeof(XONLINETASK_OFFERING_ENUM));
	
	// Set up the buffer 
	if (!pbBuffer)
		pbBuffer = (PBYTE)(pcontenum + 1) + cbResultVector;

	// Save the device type for later
	pcontenum->edDevice = edDevice;
	pcontenum->fPartial = FALSE;
	pcontenum->dwResults = 0;
	pcontenum->dwMaxResults = pEnumParams->dwMaxResults;

	// This essentially finds out what kind of enumeration is requested
	// and calls the appropriate function
	switch (edDevice)
	{
	case XONLINEOFFERING_ENUM_DEVICE_ONLINE:
		hr = ContentEnumerateServerBegin(
					m_dwTitleId,
					dwUserIndex,
					pEnumParams,
					pbBuffer, 
					cbBuffer, 
					hWorkEvent,
					pcontenum);
		break;

	case XONLINEOFFERING_ENUM_DEVICE_HD:
		hr = ContentEnumerateLocalBegin(
					XONLINECONTENT_HD_SEARCH_PATH,
					pEnumParams,
					pbBuffer, 
					cbBuffer, 
					hWorkEvent,
					pcontenum);
		break;

	case XONLINEOFFERING_ENUM_DEVICE_DVD:
		sprintf(szTemp, XONLINECONTENT_DVD_SEARCH_PATH, m_dwTitleId);
		hr = ContentEnumerateLocalBegin(
					szTemp,
					pEnumParams,
					pbBuffer, 
					cbBuffer, 
					hWorkEvent,
					pcontenum);
		break;

	default:
		AssertSz(FALSE, "XOnlineOfferingEnumerate: Invalid device!");
		hr = E_INVALIDARG;
	}

	if (FAILED(hr))
		goto Error;

	// OK< now return the context as the task handle
	*phTask = (XONLINETASK_HANDLE)pcontenum;

Cleanup:	
	return(XoLeave(hr));

Error:
	if (pcontenum)
		SysFree(pcontenum);
	goto Cleanup;
}

HRESULT 
CXo::XOnlineOfferingEnumerateGetResults(
			XONLINETASK_HANDLE		hTask,
		    PXONLINEOFFERING_INFO	**prgpOfferingInfo,
		    DWORD					*pdwReturnedResults,
		    BOOL					*pfMoreResults
		    )
{
    XoEnter("XOnlineOfferingEnumerateGetResults");
    XoCheck(hTask != NULL);

	HRESULT						hr;
	PXONLINETASK_OFFERING_ENUM	pcontenum;

	pcontenum = (PXONLINETASK_OFFERING_ENUM)hTask;
	if (prgpOfferingInfo)
		*prgpOfferingInfo = (PXONLINEOFFERING_INFO *)(pcontenum + 1);
	if (pdwReturnedResults)
		*pdwReturnedResults = pcontenum->dwResults;
	if (pfMoreResults)
		*pfMoreResults = pcontenum->fPartial;

	// Let's clear the results so that the caller can get the next batch
	if (pcontenum->edDevice == XONLINEOFFERING_ENUM_DEVICE_ONLINE)
		pcontenum->enumdata.server.State = enumsrvContinue;

	return(XoLeave(S_OK));
}

DWORD CXo::XOnlineOfferingEnumerateMaxSize(
    		PXONLINEOFFERING_ENUM_PARAMS	pEnumParams,
		    DWORD							cbTitleSpecificDataMaxSize
			)
{
    XoEnter_("XOnlineOfferingEnumerateMaxSize", 0);
    XoCheck_(pEnumParams != NULL, 0);

	// Need a little bit of fluff for hrad-disk enum because of 
	// non-buffered reads
	if (!cbTitleSpecificDataMaxSize)
		cbTitleSpecificDataMaxSize = XONLINEOFFERING_ENUM_MAX_TITLE_DATA_SIZE;

	cbTitleSpecificDataMaxSize += sizeof(XONLINEOFFERING_ENUM_RESP_RECORD);
	return((pEnumParams->dwMaxResults * cbTitleSpecificDataMaxSize) + XBOX_HD_SECTOR_SIZE);
}

// =================================================================
// Title enumeration stuff
//

HRESULT CXo::EnumerateTitlesContinue(
			XONLINETASK_HANDLE	hTask
			)
{
	HRESULT	hr = XONLINETASK_S_SUCCESS;
	PXONLINECONTENT_ENUM_TITLES ptitleenum;
	PXONLINETASK_DIROPS			pdirops;

	Assert(hTask != NULL);

	// The the containing record
	ptitleenum = CONTAINING_RECORD(hTask, XONLINECONTENT_ENUM_TITLES, xontask);
	pdirops = &(ptitleenum->dirops);

	if (SUCCEEDED(ConvertToTitleId(pdirops->szPath, 
				ptitleenum->rgdwTitleId + ptitleenum->dwTitleId)))
	{
		// This one,s a real title ID, so save it
		ptitleenum->dwTitleId += 1;

		// See if we have reached our capacity yet
		if (ptitleenum->dwTitleId >= ptitleenum->dwMaxResults)
		{
			// Yeah, pause the operation so the caller can get this
			// batch of results
			hr = XONLINETASK_S_RESULTS_AVAIL;
		}
	}

	return(hr);
}

HRESULT
CXo::XOnlineEnumerateTitlesBegin(
			DWORD dwMaxResults,
			OPTIONAL HANDLE hWorkEvent,
			XONLINETASK_HANDLE *phTask
			)
{
    XoEnter("XOnlineEnumerateTitlesBegin");
    XoCheck(dwMaxResults != 0);

	HRESULT						hr = S_OK;
	PXONLINECONTENT_ENUM_TITLES ptitleenum;
	DWORD						cbPath;
	PSTR						szPath;
	
	// Allocate structure plus space to hold the specified number of title IDs.
	ptitleenum = (PXONLINECONTENT_ENUM_TITLES)SysAlloc(sizeof(XONLINECONTENT_ENUM_TITLES) + 
        (dwMaxResults * sizeof(DWORD)) + XONLINECONTENT_MAX_PATH, PTAG_XONLINECONTENT_ENUM_TITLES);

	if (!ptitleenum)
	{
		hr = E_OUTOFMEMORY;
		goto Error;
	}

	// Initialize ptitleenum
	ZeroMemory(ptitleenum, sizeof(XONLINECONTENT_ENUM_TITLES));
	ptitleenum->rgdwTitleId = (DWORD *)(ptitleenum + 1);
	ptitleenum->dwTitleId = 0;
	ptitleenum->dwMaxResults = dwMaxResults;

	// Initialize the OnData task
	TaskInitializeContext(&ptitleenum->xontask);
	ptitleenum->xontask.pfnContinue = EnumerateTitlesContinue;
	ptitleenum->xontask.hEventWorkAvailable = hWorkEvent;
	if (hWorkEvent)
		SetEvent(hWorkEvent);

	// Set up the specific search path for titles
	szPath = (PSTR)(ptitleenum->rgdwTitleId + dwMaxResults);
	strcpy(szPath, XONLINECONTENT_TITLES_SEARCH_PATH);
	cbPath = strlen(szPath);

	// Kick off the directory enumeration
	hr = EnumerateDirectoryInitializeContext(
				szPath,
				cbPath,
				XONLINECONTENT_MAX_PATH-1,
				XONLINETASK_EDFLAG_ENUMERATE_DIRS,
				(XONLINETASK_HANDLE)&(ptitleenum->xontask),
				NULL,
				hWorkEvent, 
				&(ptitleenum->dirops));
	if (FAILED(hr))
		goto Error;

	// Return the task handle
	*phTask = (XONLINETASK_HANDLE)ptitleenum;

Exit:
	return(XoLeave(hr));

Error:
	// Abort the operation
	if (ptitleenum)
		SysFree(ptitleenum);
		
	goto Exit;
}

HRESULT 
CXo::XOnlineEnumerateTitlesContinue(
			XONLINETASK_HANDLE hTask
			)
{
    XoEnter("XOnlineEnumerateTitlesContinue");
    XoCheck(hTask != NULL);
    
	PXONLINECONTENT_ENUM_TITLES ptitleenum;

	ptitleenum = (PXONLINECONTENT_ENUM_TITLES)hTask;

	// Reset the counter and continue the enumeration
	ptitleenum->dwTitleId = 0;

	return(XoLeave(S_OK));
}	

HRESULT 
CXo::XOnlineEnumerateTitlesGetResults(
			XONLINETASK_HANDLE hTask,
			DWORD **prgTitleID,
			DWORD *pdwTitleID
			)
{
    XoEnter("XOnlineEnumerateTitlesGetResults");
    XoCheck(hTask != NULL);

	PXONLINECONTENT_ENUM_TITLES ptitleenum;

	ptitleenum = (PXONLINECONTENT_ENUM_TITLES)hTask;

	if (prgTitleID)
		*prgTitleID = ptitleenum->rgdwTitleId;
	if (pdwTitleID)
		*pdwTitleID = ptitleenum->dwTitleId;

	return(XoLeave(S_OK));
}

#if 0

/*
	This downloads all the headers for the current title into pBuffer. 	
*/

DWORD offset;

#define ADVANCE(buf, length, end) \
		{\
			Assert(buf != NULL); \
			if (buf + length > end) return E_BADNETWORKDATA; \
			buf += length; \
			offset += length; \
		}

#define ACCESS(pvar, buf, type, end) \
		{\
			pvar = (type *)buf; \
			ADVANCE(buf, sizeof(type), end); \
		}
			

HRESULT CatalogDVD(
			PBYTE pbBuffer,
			DWORD cbBuffer,
			DWORD dwMaxResults,
			DWORD dwStart,
			XONLINETASK_HANDLE *phTask, 
			HANDLE hWorkEvent 
			)
{
	HRESULT hr;
	HANDLE hFile;
	ULARGE_INTEGER ul;

	cbBuffer -= (cbBuffer % XBOX_HD_SECTOR_SIZE); //make it fit to multiple of sector size

	
// Remarks:
// The file to read from must have been opened with FILE_FLAG_OVERLAPPED
// and FILE_FLAG_NO_BUFFERING. This also means that the following must be true:
// 1) the buffer must be DWORD aligned,
// 2) the starting offset of the read must be sector aligned (XBOX_HD_SECTOR_SIZE)
// 3) the number of bytes to read must be a multiple of XBOX_HD_SECTOR_SIZE.
//
	hFile = CreateFile(XONLINE_DVD_ENUM_FILE, GENERIC_READ, FILE_SHARE_READ, NULL,
						OPEN_EXISTING, FILE_FLAG_OVERLAPPED | FILE_FLAG_NO_BUFFERING, NULL);

	if (INVALID_HANDLE_VALUE == hFile)
		return HRESULT_FROM_WIN32(GetLastError());


	ul.QuadPart = 0;

	hr = ReadFile(hFile, pbBuffer, cbBuffer, ul, hWorkEvent, phTask);

	return hr;
}
							 
//
// Main do work function
//
HRESULT EnumerateTitlesWork(
			XONLINETASK_OP		op,
			XONLINETASK_HANDLE	hTask, 
			DWORD				dwUntilTickCount,
			DWORD				*pdwWorkFlags
			)
{
	HRESULT				hr = S_OK;
	WIN32_FIND_DATA		wfd;
	DWORD				dwError;

	LARGE_INTEGER lFreq, lStart, lNow, lMax;
	PXONLINETASK_HDENUM	phdenum;

	QueryPerformanceCounter(&lStart); //to init GoodTime
	QueryPerformanceFrequency(&lFreq);

	lMax.QuadPart = (dwUntilTickCount * lFreq.QuadPart / (__int64)1000000);

	phdenum = (PXONLINETASK_HDENUM)hTask;
	
	Assert(op < XONLINETASK_MAX_OP);
	Assert(hTask != NULL);

	// Reset the work flags
	XONLINETASK_WORKFLAGS_RESET(pdwWorkFlags);

	// Always signal the event. If we actually do any async work, we 
	// will reset the event. This makes sure we don't stall
	if (phdenum->xontask.hEventWorkAvailable)
		SetEvent(phdenum->xontask.hEventWorkAvailable);

	switch (op)
	{
	case XONLINETASK_DOWORK:

		//loop until out of time
		do
		{
			// get the next file
			if (phdenum->hFind == INVALID_HANDLE_VALUE)
			{
				phdenum->hFind = FindFirstFile(XONLINE_ENUMERATE_HD_DIR, &wfd);
				
				if (phdenum->hFind == INVALID_HANDLE_VALUE)
				{
					dwError = GetLastError();
					if ((dwError == ERROR_PATH_NOT_FOUND) ||
						(dwError == ERROR_FILE_NOT_FOUND))
					{
						// No files are found. that's cool - just means no results
						EnumerateTitlesCleanup(phdenum);
						XONLINETASK_WORKFLAGS_SET_DONT_CONTINUE(pdwWorkFlags);
						//don't free task just yet - need data to work
						goto Exit; 
					}

					hr = HRESULT_FROM_WIN32(dwError);
					goto Error;
				}
			}
			else
			{
				if (!FindNextFile(phdenum->hFind, &wfd))
				{
					//no more files....buwahahaha - end of function
					EnumerateTitlesCleanup(phdenum);
					XONLINETASK_WORKFLAGS_SET_DONT_CONTINUE(pdwWorkFlags);
					//don't free task just yet - need data to work
					goto Exit; 
				}
			}

			//add directory
			if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				phdenum->rgTitleID[phdenum->iCurrent++] = atoi(wfd.cFileName);
				if (phdenum->iCurrent >= phdenum->cTitleID)
				{
					//got max results
					EnumerateTitlesCleanup(phdenum);
					XONLINETASK_WORKFLAGS_SET_DONT_CONTINUE(pdwWorkFlags);
					//don't free task just yet - need data to work
					goto Exit; 
				}
			}

			QueryPerformanceCounter(&lNow);

		} while (lNow.QuadPart - lStart.QuadPart < lMax.QuadPart);

		break;
		
	case XONLINETASK_CANCEL:
		hr = E_ABORT;
		EnumerateTitlesCleanup(phdenum);
		XONLINETASK_WORKFLAGS_SET_DONT_CONTINUE(pdwWorkFlags);
		break;
	
	case XONLINETASK_RELEASE:
		EnumerateTitlesCleanup(phdenum);
		SysFree(phdenum);
		XONLINETASK_WORKFLAGS_SET_DONT_CONTINUE(pdwWorkFlags);
		break;
		
	default:
		AssertSz(FALSE, "EnumerateTitlesWork: Bad OP");
		hr = E_NOTIMPL;
	}

Exit:
	return(S_OK);

Error:
	// Error, abort the task.
	XONLINETASK_WORKFLAGS_SET_DONT_CONTINUE(pdwWorkFlags);
	return(hr);
}



//
// Cleanup function
//
VOID EnumerateTitlesCleanup(
			PXONLINETASK_HDENUM phdenum
			)
{
	if (phdenum->hFind != INVALID_HANDLE_VALUE)
	{
		CloseHandle(phdenum->hFind);
		phdenum->hFind = INVALID_HANDLE_VALUE;
	}
}

//
// Implement the OnData do work function for HD enumeration 
//
HRESULT ContentEnumerateHDContinue(
			XONLINETASK_HANDLE	hTask, 
			DWORD				dwUntilTickCount
			)
{
	HRESULT						hr = S_OK;
	PXONLINETASK_CONTENT_ENUM	pcontenum;
	PXONLINECONTENT_ENUM_HD		phdenum;
	PXON_CATREF_ENUM_REC_HDR	prechdr;
	PXONLINETASK_FILEIO			pfileio;
	PXONLINECATALOG				*rgCatalogSlot;
	PXONLINECATALOG				pcatalog;
	ULARGE_INTEGER				uliOrigin;
	DWORD						dwWorkFlags;
	DWORD						cbPath;
	DWORD						dwSize;
	PBYTE						pbBuffer;
	PSTR						szPath;

	// We can make very tight assumptions and validate here
	Assert(hTask != NULL);

	XONLINETASK_WORKFLAGS_RESET(pdwWorkFlags);

	// Only service do work
	if (op == XONLINETASK_DOWORK)
	{
		// hTask points to the xontaskPostUpload field of XONLINECONTENT_ENUM_HD,
		// find the containing XONLINECONTENT_ENUM_HD structure
		phdenum = CONTAINING_RECORD(hTask, XONLINECONTENT_ENUM_HD, xontask);
		pcontenum = CONTAINING_RECORD(penum, XONLINETASK_CONTENT_ENUM, enumdata);
		pfileio = &(phdenum->fileio);

		// There are two states here: open file and kick of async read; and 
		// parse read results and close file. This is switched through the
		// hFile value. This means it is very important to make sure the file
		// is properly closed and set back to INVALID_HANDLE_VALUE every time
		// immediately after use.
		if (pfileio->hFile == INVALID_HANDLE_VALUE)
		{
			szPath = phdenum->szPath;
			// Yup, new directory entry found. Validate the directory name and
			// attempt to open the metadata file.
			hr = ConvertToLowerContentId(szPath, &(phdenum->ContentId));
			if (FAILED(hr))
				goto Done;

			// Build the file path in-place
			cbPath = strlen(szPath);
			szPath[cbPath++] = '\\';
			strcpy(szPath + cbPath, XONLINECONTENT_MANIFEST_FILE_NAME);
			cbPath += strlen(XONLINECONTENT_MANIFEST_FILE_NAME);
			dwSize = strlen(XONLINECONTENT_HD_ROOT_PATH);
			memmove(szPath + dwSize, szPath, cbPath + 1);
			memcpy(szPath, XONLINECONTENT_HD_ROOT_PATH, dwSize);

			// Open the file, skip it if it can't be opened
			pfileio->hFile = CreateFile(
						szPath,
						GENERIC_READ,
						FILE_SHARE_READ,
						NULL,
						OPEN_EXISTING,
						FILE_FLAG_OVERLAPPED | 
						FILE_FLAG_NO_BUFFERING |
						FILE_FLAG_SEQUENTIAL_SCAN,
						NULL);
			if (pfileio->hFile == INVALID_HANDLE_VALUE)
				goto Done;

			// Make sure our read buffer is DWORD-aligned
			dwSize = pfileio->pbBuffer & (sizeof(DWORD) - 1);
			if (dwSize)
			{
				dwSize = sizeof(DWORD) - dwSize;
				pfileio->pbBuffer += dwSize;
				pfileio->cbBuffer -= dwSize;
			}

			// Make sure buffer size if sector-aligned
			dwSize = pfileio->cbBuffer & ~(XBOX_HD_SECTOR_SIZE - 1);
			if (dwSize == 0)
			{
				// Not enough buffer to read, so we are actually done
				// with the current batch. We will close the file so that
				// the next continuation will retry this item. This is
				// not optimal because we will try to call CreateFile
				// twice, but this helps us keep the directory enumeration
				// framework much cleaner.
				XONLINETASK_WORKFLAGS_SET_PARTIAL_RESULTS(pdwWorkFlags);
				pcontenum->fPartial = TRUE;
				hr = S_FALSE;
				goto Cleanup;
			}

			// Now kick off a read to load its contents
			uliOrigin.QuadPart = 0;
			ReadWriteFileInitializeContext(pfileio->hFile,
						pfileio->pbBuffer, dwSize, uliOrigin, 
						phdenum->xontask.hEventWorkAvailable, 
						pfileio);

			hr = ReadFileInternal(pfileio);
			if (FAILED(hr))
				goto Error;
		}
		else
		{
			// We will pump until the read returns
			hr = XOnlineTaskContinue((XONLINETASK_HANDLE)pfileio, 0, &dwWorkFlags);
			if (XONLINETASK_WORKFLAGS_CONTAINS_DONT_CONTINUE(dwWorkFlags))
			{
				// Skip this entry if the read had failed
				if (FAILED(hr))
					goto Done;

				// Get the read results
				hr = GetReadWriteFileResults(
							(XONLINETASK_HANDLE)pfileio, &dwSize, &pbBuffer);
				if (FAILED(hr))
					goto Done;

				// Validate and relocate the record, skip this record if failed
				prechdr = (PXON_CATREF_ENUM_REC_HDR)pbBuffer;
				pcatalog = &(prechdr->Catalog);
				
				hr = ValidateAndRelocateCatalogRecord(prechdr, dwSize);
				if (FAILED(hr))
					goto Done;

				// This is a good record, we actually add this to our result list
				rgCatalogSlot = (PXONLINECATALOG *)(pcontenum + 1);
				rgCatalogSlot[pcontenum->dwResults++] = pcatalog;

				// See if we reached the desired number of records yet
				if (pcontenum->dwResults == pcontenum->dwMaxResults)
				{
					XONLINETASK_WORKFLAGS_SET_PARTIAL_RESULTS(pdwWorkFlags);
					pcontenum->fPartial = TRUE;
					goto Cleanup;
				}

				// Done this directory item, move on to the next one
				goto Done;
			}
		}
	}

Exit:
	return(hr);

Done:	
	// Clear out the error and fall thru the error path
	hr = S_OK;

Cleanup:
Error:
	if (pfileio->hFile != INVALID_HANDLE_VALUE)
	{
		// Make sure the file is closed promptly
		CloseHandle(pfileio->hFile);
		pfileio->hFile = INVALID_HANDLE_VALUE;
	}
	
	// Set the don't continue flag to indicate we are done with this
	// directory
	XONLINETASK_WORKFLAGS_SET_DONT_CONTINUE(pdwWorkFlags);
	goto Exit;
}				


#endif

