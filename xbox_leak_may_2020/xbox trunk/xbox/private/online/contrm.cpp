/*++

Copyright (c) Microsoft Corporation

Author:
    Keith Lau (keithlau)

Description:
	Module implementing content removal

Module Name:

    contrm.c

--*/

#include "xonp.h"
#include "xonver.h"

//#define CHK_CORRUPT
#ifdef CHK_CORRUPT

#define CHK_CORRUPT_SIZE		65536

VOID CXo::CheckCorruption(
			PXONLINETASK_CONTENT_REMOVE	pcontrm
			)
{
	PBYTE	pbEnd = (PBYTE)(pcontrm + 1);
	PDWORD	pdwGate;
	DWORD	dw;
	
	pbEnd += (XONLINECONTENT_MAX_PATH * 2);
	pdwGate = (PDWORD)pbEnd;
	
	for (dw = 0; dw < (CHK_CORRUPT_SIZE >> 2); dw++, pdwGate++)
		if (*pdwGate)
		{
			AssertSz(FALSE, "CORRUPT");
		}
}

#endif

#if DBG

const char * const CXo::s_rgszRemovePhases[rmphaseDone] =
{
	"rmphaseRemoveDrm",
	"rmphaseRemoveTitleData",
	"rmphaseRemoveUserData"
};

#endif

//
// Function to Delete the DRM file
//
HRESULT CXo::contrmRemoveDrm(
			PXONLINETASK_CONTENT_REMOVE	pcontrm
			)
{
	HRESULT				hr = S_OK;
	DWORD				dwError;
	PXONLINETASK_DIROPS	pdirops = &pcontrm->dirops;

	// Just plain delete the file and move on
	if (!DeleteFile(pdirops->szPath))
	{
		// We will proceed if the DRM file is not found. This
		// lets us to reclaim disk space for an incomplete 
		// content package
		dwError = GetLastError();
		if ((dwError != ERROR_FILE_NOT_FOUND) &&
			(dwError != ERROR_PATH_NOT_FOUND))
		{
			// Cannot remove DRM file
			hr = HRESULT_FROM_WIN32(dwError);
			goto Error;
		}
	}

	// Figure out the path to title content data
	pdirops->cbPath = pdirops->cbMax;
	hr = XOnlineContentGetRootDirectory(
				pcontrm->OfferingId, FALSE,	// fUserData
				&(pdirops->cbPath),
				pdirops->szPath);
	if (FAILED(hr))
		goto Error;
		
	// Set up the directory operation to recursively remove the
	// title content data for this package
	hr = RemoveDirectoryInitializeContext(
				pdirops->szPath, 
				pdirops->cbPath,
				pdirops->cbMax,
				pdirops->szDir,
				0,
				pcontrm->xontask.hEventWorkAvailable,
				pdirops);
	if (FAILED(hr))
		goto Error;

	// Next phase is to remove all title data
	pcontrm->rmphase = rmphaseRemoveTitleData;

Error:
	return(hr);
}

//
// Function to Delete all title content
//
HRESULT CXo::contrmRemoveTitleData(
			PXONLINETASK_CONTENT_REMOVE	pcontrm
			)
{
	HRESULT				hr = S_OK;
	PXONLINETASK_DIROPS	pdirops = &pcontrm->dirops;

	// Wait for the subtask to complete
	hr = XOnlineTaskContinue((XONLINETASK_HANDLE)pdirops);
	if (XONLINETASK_STATUS_AVAILABLE(hr))
	{
		// Returned results, see if it succeeded
		if (FAILED(hr))
			goto Error;

		// OK, all title data is deleted, now try to delete all
		// user content data
		pdirops->cbPath = pdirops->cbMax;
		hr = XOnlineContentGetRootDirectory(
					pcontrm->OfferingId, TRUE,	// fUserData
					&(pdirops->cbPath),
					pdirops->szPath);
		if (SUCCEEDED(hr))
		{
			// Set up the directory operation to recursively remove the
			// title content data for this package
			hr = RemoveDirectoryInitializeContext(
						pdirops->szPath, 
						pdirops->cbPath,
						pdirops->cbMax,
						pdirops->szDir,
						0,
						pcontrm->xontask.hEventWorkAvailable,
						pdirops);
			if (FAILED(hr))
				goto Error;

			// Next phase is to remove all title data
			pcontrm->rmphase = rmphaseRemoveUserData;
		}
		else
		{
			// No user data, we are done
			hr = S_OK;
			pcontrm->rmphase = rmphaseDone;
		}
	}
	
Error:
	return(hr);
}

//
// Function to Delete all user content
//
HRESULT CXo::contrmRemoveUserData(
			PXONLINETASK_CONTENT_REMOVE	pcontrm
			)
{
	HRESULT				hr = S_OK;
	PXONLINETASK_DIROPS	pdirops = &pcontrm->dirops;

	// Wait for the subtask to complete
	hr = XOnlineTaskContinue((XONLINETASK_HANDLE)pdirops);
	if (XONLINETASK_STATUS_AVAILABLE(hr))
	{
		// Returned results, see if it succeeded
		if (FAILED(hr))
			goto Error;

		// We're done.
		hr = S_OK;
		pcontrm->rmphase = rmphaseDone;
	}
	
Error:
	return(hr);
}

//
// Function to cleanup
//
HRESULT CXo::contrmCleanup(
			PXONLINETASK_CONTENT_REMOVE	pcontrm
			)
{
	HRESULT	hr = S_OK;

	// Make sure we cancel any directory operations
	if ((pcontrm->rmphase == rmphaseRemoveTitleData) ||
		(pcontrm->rmphase == rmphaseRemoveUserData))
	{
		DiropsCleanup(&pcontrm->dirops);
	}
	return(hr);
}

//
// Implement the do work function for XOnlineContentRemove
//

HRESULT CXo::contrmContinue(
			XONLINETASK_HANDLE	hTask
			)
{
	HRESULT						hr = S_OK;
	PXONLINETASK_CONTENT_REMOVE	pcontrm = NULL;
	PXONLINETASK_DIROPS			pdirops = NULL;

#if DBG
	LARGE_INTEGER				liTimerTotal;
	LARGE_INTEGER				liTimerEnd;
	LARGE_INTEGER				liTimerStart;
	XONLINECONTENT_RMPHASE		rmphase;

	// Track service count
	m_lirmServiceCount.QuadPart += 1;

	// Start the total timer
	QueryPerformanceCounter(&liTimerTotal);
#endif

	Assert(hTask != NULL);

	// hTask points to the xontask field of XONLINETASK_CONTENT_REMOVE,
	// find the containing pcontrm structure
	pcontrm = CONTAINING_RECORD(hTask, XONLINETASK_CONTENT_REMOVE, xontask);
	pdirops = &pcontrm->dirops;

	// Make sure the event is always signalled
	if (pcontrm->xontask.hEventWorkAvailable != NULL)
		SetEvent(pcontrm->xontask.hEventWorkAvailable);

#ifdef CHK_CORRUPT
	CheckCorruption(pcontrm);
#endif

#if DBG
	// Start the loop timer
	QueryPerformanceCounter(&liTimerStart);
	rmphase = pcontrm->rmphase;
#endif

	// Figure out what phase we are in ...
	switch (pcontrm->rmphase)
	{
	case rmphaseRemoveDrm:
		hr = contrmRemoveDrm(pcontrm);
		break;
		
	case rmphaseRemoveTitleData:
		hr = contrmRemoveTitleData(pcontrm);
		break;
		
	case rmphaseRemoveUserData:
		hr = contrmRemoveUserData(pcontrm);
		break;
		
	default:
		// This is a bad state. We force it to end so we don't get stuck
		// here forever.
		AssertSz(FALSE, "contrmContinue: invalid phase!");
		hr = E_FAIL;
	}

#if DBG	
	// Stop timer
	QueryPerformanceCounter(&liTimerEnd);

    m_lirmTotalTime.QuadPart += (liTimerEnd.QuadPart - liTimerStart.QuadPart);
	m_lirmAverageTime.QuadPart = m_lirmTotalTime.QuadPart / m_lirmServiceCount.QuadPart;

//	TraceSz3(Verbose, "%s, %I64u us", s_rgszRemovePhases[rmphase], (liTimerEnd.QuadPart - liTimerStart.QuadPart));
	
#endif	

	// Bail if failed.
	if (FAILED(hr))
	{
		contrmCleanup(pcontrm);
		pcontrm->rmphase = rmphaseDone;
	}

	// When the next state is the final state, we set the flag so we don't
	// continue.
	if (pcontrm->rmphase == rmphaseDone)
	{
		// Indicate that we are done and return the final code
		XONLINETASK_SET_COMPLETION_STATUS(hr);

#if DBG
		TraceSz1(Verbose, "%I64u services elapsed", m_lirmServiceCount.QuadPart);
		TraceSz1(Verbose, "Total time: %I64u ticks", m_lirmTotalTime.QuadPart);
		TraceSz1(Verbose, "Average service time: %I64u ticks", m_lirmAverageTime.QuadPart);
		m_lirmServiceCount.QuadPart = 0;
		m_lirmTotalTime.QuadPart = 0;
		m_lirmAverageTime.QuadPart = 0;
#endif		
	}

#ifdef CHK_CORRUPT
	CheckCorruption(pcontrm);
#endif

	return(hr);
}				

//
// Function to close the task handle
//
VOID CXo::contrmClose(
			XONLINETASK_HANDLE	hTask
			)
{			
	PXONLINETASK_CONTENT_REMOVE	pcontrm = (PXONLINETASK_CONTENT_REMOVE)hTask;
	
	contrmCleanup(pcontrm);
	SysFree(pcontrm);
}

//
// Function to kick off the removal of a content package. This is 
// the internal/Dash version because it allows a Title ID to be 
// specified.
//
HRESULT CXo::ContentRemoveInternal(
			DWORD					dwTitleId,
			XONLINEOFFERING_ID		OfferingId,
			DWORD					dwFlags,
			HANDLE					hWorkEvent,
			XONLINETASK_HANDLE		*phTask
			)
{
	HRESULT						hr = S_OK;
	PXONLINETASK_CONTENT_REMOVE pcontrm = NULL;
	PXONLINETASK_DIROPS			pdirops = NULL;
	PBYTE						pbPath = NULL;
	PBYTE						pbDir = NULL;
	DWORD						cbSize;

    Assert(NULL != phTask);

	// We are responsible for allocating the context and any other
	// buffers. These buffers immediately follow the context structure
	cbSize = sizeof(XONLINETASK_CONTENT_REMOVE) + (XONLINECONTENT_MAX_PATH * 2);
	
#ifdef CHK_CORRUPT	
	cbSize += CHK_CORRUPT_SIZE;
#endif

	pcontrm = (PXONLINETASK_CONTENT_REMOVE)SysAlloc(cbSize, PTAG_XONLINETASK_CONTENT_REMOVE);
	if (!pcontrm)
	{
		hr = E_OUTOFMEMORY;
		goto Error;
	}

	ZeroMemory(pcontrm, cbSize);
	pdirops = &pcontrm->dirops;
	pbPath = (PBYTE)(pcontrm + 1);
	pbDir = pbPath + XONLINECONTENT_MAX_PATH;

	// Fill in the blanks
	TaskInitializeContext(&pcontrm->xontask);
	pcontrm->xontask.hEventWorkAvailable = hWorkEvent;
	pcontrm->xontask.pfnContinue = contrmContinue;
	pcontrm->xontask.pfnClose = contrmClose;
	pcontrm->rmphase = rmphaseRemoveDrm;
	pcontrm->dwFlags = dwFlags;
	pcontrm->dwTitleId = dwTitleId;
	pcontrm->OfferingId = OfferingId;

	// Build the path to the DRM file, it must be in the
	// title data area
	cbSize = XONLINECONTENT_MAX_PATH;
	hr = BuildExistingContentPath(
				dwTitleId, OfferingId, 
				XONLINECONTENT_MANIFEST_FILE_NAME,
				dirtypeTitleContent,
				&cbSize, (PSTR)pbPath);
	if (FAILED(hr))
		goto Error;

	// Set up the dirops context
	pdirops->szPath = (PSTR)pbPath;
	pdirops->cbPath = cbSize;
	pdirops->szDir = (PSTR)pbDir;
	pdirops->cbMax = XONLINECONTENT_MAX_PATH;

	// We need to set the event here to signal more work is
	// immediately required
	if ((hWorkEvent != NULL) && (SetEvent(hWorkEvent) == FALSE))
	{
		hr = HRESULT_FROM_WIN32(GetLastError());
		goto Error;
	}

	// Return the context as an opaque handle
	*phTask = (XONLINETASK_HANDLE)pcontrm;

Cleanup:	
	return(hr);

Error:

	// Failed setup, make sure we clean up everything
	if (pcontrm)
	{
		contrmCleanup(pcontrm);
		SysFree(pcontrm);
	}	
	goto Cleanup;
}

//
// Function to kick off a content removal. This is the public version 
// that removes the specified content package for the current title.
//
HRESULT CXo::XOnlineContentRemove(
			XONLINEOFFERING_ID		OfferingId,
			HANDLE					hWorkEvent,
			XONLINETASK_HANDLE		*phTask
			)
{
    XoEnter("XOnlineContentRemove");
	return(XoLeave(ContentRemoveInternal(m_dwTitleId, OfferingId, 0, hWorkEvent, phTask)));
}

