/*++

Copyright (c) Microsoft Corporation

Author:
    Keith Lau (keithlau)

Description:
	Module implementing Differential compression and online updates

Module Name:

    difpatch.c

--*/

#include "xonp.h"
#include "xonver.h"

#ifdef XONLINE_FEATURE_XBOX

#include "xboxp.h"
#include "xlaunch.h"

//
// Online Autoupdate states
//
// Note: these are executed as subtasks of XOnlineContentInstall
//
typedef enum
{
	// Pre-install custom task phases
	xoupdPreInitialize = 0,
	xoupdCountFiles,			// Dash update only
	xoupdFormatTemp,
	xoupdDoneDVDPreInit,		// DVD update only

	// Post-install custom task phases
	xoupdPostInitialize,
	xoupdVerifyPatchDigest,
	xoupdVerifyTargetDigest,
	xoupdWipeTarget,
	xoupdOpenPatch,
	xoupdOpenSource,
	xoupdCreateTarget,
	xoupdCreateDirectory,
	xoupdReadSource,
	xoupdReadPatch,
	xoupdDecodeData,
	xoupdWriteTarget,
	xoupdCloseFiles,
	xoupdCopyDigest,
	xoupdCopyOtherFiles, 		// Dash update only
	xoupdSignDigest,
	xoupdWriteConfig,
	xoupdCleanupTemp,
	xoupdCleanupOfflineDashFiles,
	xoupdDone,

} AUTOUPD_ONLINE_STATES;

#if DBG
//
// String names for states
//
const char * const g_rgszxoupdStates[xoupdDone] =
{
	"xoupdPreInitialize",
	"xoupdCountFiles",
	"xoupdFormatTemp",
	"xoupdDoneDVDPreInit", 
	"xoupdPostInitialize",
	"xoupdVerifyPatchDigest",
	"xoupdVerifyTargetDigest",
	"xoupdWipeTarget",
	"xoupdOpenPatch",
	"xoupdOpenSource",
	"xoupdCreateTarget",
	"xoupdCreateDirectory",
	"xoupdReadSource",
	"xoupdReadPatch",
	"xoupdDecodeData",
	"xoupdWriteTarget",
	"xoupdCloseFiles",
	"xoupdCopyDigest",
	"xoupdCopyOtherFiles",
	"xoupdSignDigest",
	"xoupdWriteConfig",
	"xoupdCleanupTemp",
	"xoupdCleanupOfflineDashFiles"
};
#endif

//
// History window size boundaries. These should match those used 
// in the patch generation tool
//
#define AUTOUPD_MIN_WINDOW_SIZE			(1 << 17)	// 128KB
#define AUTOUPD_MAX_WINDOW_SIZE			(1 << 25)	// 32MB

//
// Max size for patch file header (64KB)
// 
// Note: this is chosen to be enough records to fill the
// hard drive twice using the formula:
// (8GB / AUTOUPD_WINDOW_SIZE) * 2 * AUTOUPD_INTERLEAVE_ENTRY + slack
//
#define AUTOUPD_FILE_MAX_HEADER_SIZE	(1 << 16)

//
// We'll set 4 chunks for the patch data buffer
//
#define AUTOUPD_PATCH_BUFFER_SIZE		(CHUNK_SIZE << 2)

//
// Patch application subtask for online autoupdate
//
struct XONLINEAUTOUPD_ONLINE
{
	XONLINETASK_CONTEXT			xontask;

	AUTOUPD_ONLINE_STATES		State;
	AUTOUPD_ONLINE_STATES		NextState;
	XONLINEDIR_TYPE				dirtypeTarget;	// Diff source (none for DVD)
	XONLINEDIR_TYPE				dirtypeSource;	// Diff/update target
	WORD						wOldVersionHi;
	WORD						wOldVersionLo;

	// Bit flags
	BOOL						fFirstDashUpdate:1;
	BOOL						fFDriveMounted:1;

	// Current patch file header information
	PAUTOUPD_PATCH_FILE_INFO	papfInfo;
	DWORD						cbapfInfo;

	// Current file info
	PSTR						szCurrentFile;
	PDGSTFILE_FILE_DATA			pdfdPatch;
	PDGSTFILE_FILE_DATA			pdfdTarget;
	DWORD						dwSourceSize;
	DWORD						cbDecoded;

	// Contexts for enumerating digest entries
	DWORD						cbPatchDigest;
	DWORD						cbTargetDigest;
	DGSTFILE_ENUM_CONTEXT		dgstenumPatch;
	DGSTFILE_ENUM_CONTEXT		dgstenumTarget;

	// File handles for the three-way patch files
	HANDLE						hPatch;
	HANDLE						hSource;
	HANDLE						hTarget;

	// Context for tracking progress
	AUTOUPD_PROGRESS_CONTEXT	progress;

	// Context for offline dash removal
	XONLINETASK_DEL_OLD_DASH	xondod;
	
	// Task context for verifying digest files
	XONLINETASK_CONTENT_VERIFY	contver;

	// Task context for directory create/remove operations
	XONLINETASK_DIROPS			dirops;

	// Task context for copying directories
	XONLINETASK_DIRCOPY			dircopy;

	// LZX decoder context
	XONLINE_LZXDECODER_CONTEXT	lzx;
	DWORD						dwWindowSize;

	// Current raw title key
	BYTE	rgbTitleKey[XBOX_KEY_LENGTH];

	// Context for calulcating SHA1 signatures
	BYTE	shactxPatch[XC_SERVICE_SHA_CONTEXT_SIZE];
	BYTE	shactxTarget[XC_SERVICE_SHA_CONTEXT_SIZE];

	// Various data buffers
	BYTE	rgbHeaderInfo[AUTOUPD_FILE_MAX_HEADER_SIZE];
	BYTE	rgbPatchDigest[XONLINECONTENT_MANIFEST_MAX_FILE_SIZE];
	BYTE	rgbTargetDigest[XONLINECONTENT_MANIFEST_MAX_FILE_SIZE];
	BYTE	rgbPatchData[AUTOUPD_PATCH_BUFFER_SIZE];
	BYTE	rgbOutput[CHUNK_SIZE];
	BYTE	rgbDecodeWindow[AUTOUPD_MAX_WINDOW_SIZE];
};

//
// Online autoupdate task context
//
struct XONLINETASK_AUTOUPD_ONLINE
{
	XONLINETASK_CONTENT_DOWNLOAD	contdl;
	XONLINEAUTOUPD_ONLINE			autoupd;	
	
};

//
// Function to return the required window size for patch application
//
#ifndef ROUNDUP2
#define ROUNDUP2(x, n) ((((ULONG)(x)) + (((ULONG)(n)) - 1 )) & ~(((ULONG)(n)) - 1 ))
#endif

DWORD AutoupdateLzxWindowSize(
			DWORD OldDataSize,
			DWORD NewDataSize
			)
{
	ULONG WindowSize;
	ULONG DataSize;

	DataSize = ROUNDUP2(OldDataSize, CHUNK_SIZE) + NewDataSize;

	if (DataSize <= AUTOUPD_MIN_WINDOW_SIZE)
		return(AUTOUPD_MIN_WINDOW_SIZE);

	WindowSize = AUTOUPD_MAX_WINDOW_SIZE;

	while ((WindowSize >> 1) >= DataSize)
		WindowSize = WindowSize >> 1;

	return(WindowSize);
}

//
// Function to kick off an autoupdate referral request given a pointer 
// to the XONLINETASK_AUTOUPD_ONLINE data structure that is initiating 
// this autoupdate referral action, the title ID, machine ID, and the
// current dash version (dash update only).
//
// Note that the xrlasync member of pxoupd->contdl must already been 
// filled in because this function will refer to members in 
// pxoupd->contdl.xrlasync. In particular, the hEventWorkAvailable field.
//
HRESULT CXo::InitiateAutoupdateReferralRequest(
			DWORD						dwTitleId,
			ULONGLONG					qwMachineId,
			DWORD						dwCurrentDashVersion,
			PXONLINETASK_AUTOUPD_ONLINE	pxoupd
			)
{
    HRESULT hr;
	PXONLINETASK_CONTENT_DOWNLOAD	pcontdl;
	PXRL_ASYNC_EXTENDED				pxrlasyncextCatref;
	XONLINE_SERVICE_INFO			serviceCatref;
	PXONLINEUPDATE_REFERRAL_REQ		prefreq;
	PXONLINE_USER					pusers;
	CHAR							szContentType[XONLINE_MAX_CONTENT_TYPE_SIZE];
	DWORD							cbContentType;
	DWORD							i;
	
	Assert(pxoupd != NULL);

	// Set up the async context and the service
	pcontdl = &pxoupd->contdl;
	pxrlasyncextCatref = &pcontdl->xrlasyncCatref;

	// Fill in the request data. We hijack the general buffer as the
	// request buffer
	prefreq = (PXONLINEUPDATE_REFERRAL_REQ)(pcontdl->install.rgbBuffer);
	prefreq->Header.cbSize = sizeof(XONLINEUPDATE_REFERRAL_REQ);
	prefreq->Header.dwReserved = 0;
	prefreq->dwTitleId = dwTitleId;
	prefreq->dwDashVersion = dwCurrentDashVersion;
	prefreq->qwMachineId = qwMachineId;

	// Build the content type header
	cbContentType = sprintf(szContentType, 
				"Content-type: xon/%x\r\n", XONLINE_AUTO_UPDATE_SERVICE);

	// Kick off the data upload, note we use the LZX decompression
	// buffer as the scratch pad
	return(UploadFromMemoryInternal(
				XONLINE_AUTO_UPDATE_SERVICE,
				XONLINEAUTOUPD_REFERRAL_XRL,
				pcontdl->install.rgbDecompress,
				XONLINECONTENT_LZX_OUTPUT_SIZE,
				(PBYTE)szContentType,
				cbContentType,
				(PBYTE)prefreq,
				prefreq->Header.cbSize,
				XONLINEAUTOUPD_REFERRAL_TIMEOUT,
				pcontdl->xrlasync.xontask.hEventWorkAvailable,
				pxrlasyncextCatref));
}

//
// Per-file task handler to count the number of files and total size
//
HRESULT CXo::UpdateOnlineCountFilesContinue(
			XONLINETASK_HANDLE	hTask
			)
{
	ULARGE_INTEGER				uliAlignedSize;
	PXONLINETASK_DIRCOPY		pdircopy;
	PXONLINEAUTOUPD_ONLINE		pautoupd;
	PAUTOUPD_PROGRESS_CONTEXT	papc;
	WIN32_FIND_DATA				*pwfd;

	Assert(hTask != NULL);

	// Locate the containing object
	pdircopy = CONTAINING_RECORD(hTask, XONLINETASK_DIRCOPY, xontaskOnFile);
	pautoupd = CONTAINING_RECORD(pdircopy, XONLINEAUTOUPD_ONLINE, dircopy);
	papc = &pautoupd->progress;
	pwfd = &(pdircopy->dircrawl.wfd);

	// Get the context (which is a progress context) and add the values
	(papc->dwFilesTotal)++;
	uliAlignedSize.u.LowPart = pwfd->nFileSizeLow;
	uliAlignedSize.u.HighPart = pwfd->nFileSizeHigh;
	papc->uliBytesTotal.QuadPart += uliAlignedSize.QuadPart;
	
	uliAlignedSize.QuadPart += (XBOX_CLUSTER_SIZE - 1);
	uliAlignedSize.QuadPart /= XBOX_CLUSTER_SIZE;
	uliAlignedSize.QuadPart *= XBOX_CLUSTER_SIZE;
	papc->uliActualBytesRequired.QuadPart += uliAlignedSize.QuadPart;

	return(XONLINETASK_S_SUCCESS);
}

//
// Implement all the state handlers
// 
HRESULT CXo::xoupdPreInitializeHandler(
			PXONLINEAUTOUPD_ONLINE	pautoupd
			)
{
	HRESULT	hr = S_OK;
	DWORD	cbSize;
	PXONLINETASK_AUTOUPD_ONLINE	ptask;
	PXONLINECONTENT_INSTALL		pinstall;

	Assert(pautoupd);

	// We need to reach in and get the content download context
	ptask = CONTAINING_RECORD(pautoupd, XONLINETASK_AUTOUPD_ONLINE, autoupd);
	pinstall = &(ptask->contdl.install);

	// Set up the progress context
	ZeroMemory(&pautoupd->progress, sizeof(AUTOUPD_PROGRESS_CONTEXT));
	pautoupd->progress.uliBytesTotal.QuadPart = pinstall->dwInstallSize;

	// Well, we need to clean up the old dash files, so
	// set up the context anyway
	hr = XOnlineUpdateDeleteOfflineDash(
				pautoupd->xontask.hEventWorkAvailable, &pautoupd->xondod);
	if (FAILED(hr))
		goto Error;

	// If we are updating from DVD, the source will be dirtypeNone. In
	// this case, we are just done preinit
	if (pautoupd->dirtypeSource == dirtypeNone)
	{
		pautoupd->State = xoupdDoneDVDPreInit;
		goto Exit;
	}

	// We do different things depending on whether this is a title update
	if (pautoupd->dirtypeTarget == dirtypeTitleUpdateOnF)
	{
		// Title update
		pautoupd->State = xoupdFormatTemp;
	}
	else
	{
		// Make sure we are doing a dash update
		Assert((pautoupd->dirtypeTarget == dirtypeDashUpdate0) ||
				(pautoupd->dirtypeTarget == dirtypeDashUpdate1));

		// Make sure the source is the other dash partition
		Assert(((pautoupd->dirtypeTarget == dirtypeDashUpdate0) &&
				(pautoupd->dirtypeSource == dirtypeDashUpdate1)) ||
				(pautoupd->dirtypeSource == dirtypeDashUpdate0));

		// Dash update, this means we will need to copy the dash
		// files over to the update target. Since this copy may take
		// quite a while, it would be nice to include this in the
		// progress indicator. We will do a quick crawl of the 
		// source dash installation to calculate how many bytes will
		// be copied so we can factor this into the progress.
		TaskInitializeContext(&pautoupd->dircopy.xontaskOnFile);
		pautoupd->dircopy.xontaskOnFile.pfnContinue = UpdateOnlineCountFilesContinue;
		pautoupd->dircopy.xontaskOnFile.hEventWorkAvailable = pautoupd->xontask.hEventWorkAvailable;

		// Compute the source location
		sprintf((char *)pautoupd->rgbOutput, AUTOUPD_DASH_TARGET_PATH, 
				(pautoupd->dirtypeSource == dirtypeDashUpdate1)?'1':'0');

		// Set up the dircrawl context to count the number of files to install
		hr = DircrawlInitializeContext(
					(char *)pautoupd->rgbOutput, NULL, MAX_PATH, 0, NULL, 
					(XONLINETASK_HANDLE)&(pautoupd->dircopy.xontaskOnFile),
					pautoupd->xontask.hEventWorkAvailable,
					&(pautoupd->dircopy.dircrawl));
		if (FAILED(hr))
			goto Error;

		pautoupd->State = xoupdCountFiles;
	}

Exit:
	return(hr);

Error:
	goto Exit;
}

HRESULT CXo::xoupdCountFilesHandler(
			PXONLINEAUTOUPD_ONLINE	pautoupd
			)
{
	HRESULT			hr = S_OK;

	Assert(pautoupd);

	// Keep pumping until the task is done
	hr = XOnlineTaskContinue((XONLINETASK_HANDLE)&(pautoupd->dircopy));
	if (XONLINETASK_STATUS_AVAILABLE(hr))
	{
		// See if it succeeded
		if (FAILED(hr))
			goto Error;
		hr = S_OK;
		
		// Move on to the next state
		pautoupd->State = xoupdFormatTemp;
	}

Exit:
	return(hr);

Error:
	goto Exit;
}

HRESULT CXo::xoupdFormatTempHandler(
			PXONLINEAUTOUPD_ONLINE	pautoupd
			)
{
	HRESULT	hr;
	PXONLINETASK_AUTOUPD_ONLINE	ptask;
	PXONLINECONTENT_INSTALL		pinstall;

	Assert(pautoupd);

	// We need to reach in and get the content download context
	ptask = CONTAINING_RECORD(pautoupd, XONLINETASK_AUTOUPD_ONLINE, autoupd);
	pinstall = &(ptask->contdl.install);

	// We will format the temp partition to make sure we will have
	// enough space if the package is not already installed and that
	// a resume is not in progress
	if ((pinstall->dwFlags & (XONLINECONTENT_FLAG_RESUME | XONLINECONTENT_FLAG_NOT_INSTALLED)) 
				== XONLINECONTENT_FLAG_NOT_INSTALLED)
	{
		TraceSz(Verbose, "AUTOUPD: Formatting utility drive");
	
		// Blow away the utility drive
		if (!XFormatUtilityDrive())
		{
			hr = HRESULT_FROM_WIN32(GetLastError());
			goto Error;
		}
	}

	// We are done pre-install processing. Pre-set the next state
	// to Post-install initialization
	pautoupd->State = xoupdPostInitialize;
	hr = XONLINETASK_S_SUCCESS;

Exit:
	return(hr);

Error:
	goto Exit;
}

HRESULT CXo::xoupdDoneDVDPreInitHandler(
			PXONLINEAUTOUPD_ONLINE	pautoupd
			)
{
	HRESULT	hr = S_OK;

	Assert(pautoupd);

	// Based on the type of DVD update (we won't get here if it's
	// an online update), figure out what the next state should be
	// for the post-processing
	if (pautoupd->dirtypeTarget == dirtypeTitleUpdateOnF)
		pautoupd->State = xoupdDone;
	else
		pautoupd->State = xoupdWriteConfig;

	// Done all pre-init stuff
	return(XONLINETASK_S_SUCCESS);
}

HRESULT CXo::xoupdPostInitializeHandler(
			PXONLINEAUTOUPD_ONLINE	pautoupd
			)
{
	HRESULT	hr = S_OK;

	Assert(pautoupd);

	// Kick of a verify
	pautoupd->cbPatchDigest = XONLINECONTENT_MANIFEST_MAX_FILE_SIZE;
	hr = DigestVerify(
				XONLINECONTENT_MANIFEST_FILE_NAME,
				dirtypeTempDiff, 
				pautoupd->rgbPatchDigest,
				&(pautoupd->cbPatchDigest), NULL, // pbAlternateKey
				pautoupd->xontask.hEventWorkAvailable,
				&(pautoupd->contver));
	if (FAILED(hr))
		goto Exit;

	// Go to the next state
	pautoupd->State = xoupdVerifyPatchDigest;

Exit:	
	return(hr);

}

HRESULT CXo::xoupdVerifyPatchDigestHandler(
			PXONLINEAUTOUPD_ONLINE	pautoupd
			)
{
	Assert(pautoupd);

	HRESULT				hr = S_OK;
	DWORD				cbSize;
	XONLINETASK_HANDLE	hTask = (XONLINETASK_HANDLE)&(pautoupd->contver);

	// Wait for the subtask to complete
	hr = XOnlineTaskContinue(hTask);
	if (XONLINETASK_STATUS_AVAILABLE(hr))
	{
		// Returned results, see if it succeeded
		if (FAILED(hr))
			goto Exit;

		Assert(hr == XONLINETASK_S_SUCCESS);

		// Cleanup the previous task
		contverCleanup(&pautoupd->contver);

		// Set up the enumeration context for the patch digest
		hr = EnumerateDigestInitializeContext(
					pautoupd->rgbPatchDigest,
					pautoupd->cbPatchDigest,
					&(pautoupd->dgstenumPatch));
		if (FAILED(hr))
			goto Exit;

		// Get the title key
		cbSize = XBOX_KEY_LENGTH;
		hr = XoUpdateGetRawTitleKey(m_dwTitleId, pautoupd->rgbTitleKey, &cbSize);
		if (FAILED(hr))
			goto Exit;

		// Okay, now verify the target digest
		pautoupd->cbTargetDigest = XONLINECONTENT_MANIFEST_MAX_FILE_SIZE;
		hr = DigestVerify(
					XONLINEAUTOUPD_TARGET_MANIFEST_FILE_NAME,
					dirtypeTempDiff, 
					pautoupd->rgbTargetDigest,
					&(pautoupd->cbTargetDigest),
					pautoupd->rgbTitleKey,
					pautoupd->xontask.hEventWorkAvailable,
					&(pautoupd->contver));
		if (FAILED(hr))
			goto Exit;

		// Go to the next state
		pautoupd->State = xoupdVerifyTargetDigest;
	}

Exit:	
	return(hr);
}

HRESULT CXo::xoupdVerifyTargetDigestHandler(
			PXONLINEAUTOUPD_ONLINE	pautoupd
			)
{
	Assert(pautoupd);

	HRESULT				hr = S_OK;
	DWORD				cbSize;
	XONLINETASK_HANDLE	hTask = (XONLINETASK_HANDLE)&(pautoupd->contver);

	// Wait for the subtask to complete
	hr = XOnlineTaskContinue(hTask);
	if (XONLINETASK_STATUS_AVAILABLE(hr))
	{
		// Returned results, see if it succeeded
		if (FAILED(hr))
			goto Exit;

		Assert(hr == XONLINETASK_S_SUCCESS);

		// Cleanup the verification task
		contverCleanup(&pautoupd->contver);

		// Set up the enumeration context for the target digest
		hr = EnumerateDigestInitializeContext(
					pautoupd->rgbTargetDigest,
					pautoupd->cbTargetDigest,
					&(pautoupd->dgstenumTarget));
		if (FAILED(hr))
			goto Exit;

		// Build the target path, use rgbOutput as the scratchpad
		cbSize = MAX_PATH;
		hr = BuildTargetDirectory(0, 0, 0, 
					pautoupd->dirtypeTarget,
					&cbSize, (char *)pautoupd->rgbOutput);
		if (FAILED(hr))
			goto Exit;
		
		// Wipe the target location
		hr = RemoveDirectoryInitializeContext(
					(char *)pautoupd->rgbOutput, cbSize, 
					MAX_PATH, (char *)pautoupd->rgbPatchData, 0,
					pautoupd->xontask.hEventWorkAvailable,
					&(pautoupd->dirops));
		if (FAILED(hr))
			goto Exit;

		// Go to the next state
		pautoupd->NextState = xoupdOpenPatch;
		pautoupd->State = xoupdWipeTarget;
	}

Exit:	
	return(hr);
}

HRESULT CXo::xoupdWipeTargetHandler(
			PXONLINEAUTOUPD_ONLINE	pautoupd
			)
{
	Assert(pautoupd);

	HRESULT				hr = S_OK;
	DWORD				cbSize;
	XONLINETASK_HANDLE	hTask = (XONLINETASK_HANDLE)&(pautoupd->dirops);

	// Wait for the subtask to complete
	hr = XOnlineTaskContinue(hTask);
	if (XONLINETASK_STATUS_AVAILABLE(hr))
	{
		// Returned results, see if it succeeded
		if (FAILED(hr))
			goto Exit;

		Assert(hr == XONLINETASK_S_SUCCESS);

		// Now create the root directory
		cbSize = MAX_PATH;
		hr = BuildTargetDirectory(0, 0, 0, 
					pautoupd->dirtypeTarget,
					&cbSize, (char *)pautoupd->rgbOutput);
		if (FAILED(hr))
			goto Exit;

		pautoupd->rgbOutput[--cbSize] = '\0';
		if (!CreateDirectory((LPCSTR)pautoupd->rgbOutput, NULL))
		{
			hr = HRESULT_FROM_WIN32(GetLastError());
			goto Exit;
		}

		// Go to the next state
		pautoupd->State = pautoupd->NextState;
		pautoupd->NextState = xoupdDone;
	}

Exit:	
	return(hr);
}

HRESULT CXo::xoupdOpenPatchHandler(
			PXONLINEAUTOUPD_ONLINE	pautoupd
			)
{
	HRESULT				hr = S_OK;
	HANDLE				hFile = INVALID_HANDLE_VALUE;
	DWORD				cbSize;
	LARGE_INTEGER		liSize;

	Assert(pautoupd);

	// Get the next entry in the digest file
	hr = EnumerateDigestNext(
			&(pautoupd->dgstenumPatch), 
			&(pautoupd->szCurrentFile), &(pautoupd->pdfdPatch));
	if (FAILED(hr))
	{
		if (hr == HRESULT_FROM_WIN32(ERROR_NO_MORE_ITEMS))
		{
			// We are done, move on
			hr = S_OK;
			pautoupd->State = xoupdCopyDigest;
		}
		goto Error;
	}

	// Skip over digest files
	if (!_stricmp(pautoupd->szCurrentFile, XONLINECONTENT_MANIFEST_FILE_NAME) ||
		!_stricmp(pautoupd->szCurrentFile, XONLINEAUTOUPD_TARGET_MANIFEST_FILE_NAME))
	{
		goto Exit;
	}

	// Build the location
	Assert(pautoupd->szCurrentFile);
	Assert(pautoupd->pdfdPatch);
	cbSize = MAX_PATH;
	hr = BuildTargetPath(0, 0, 0, pautoupd->szCurrentFile, 
				dirtypeTempDiff, &cbSize, (char *)pautoupd->rgbOutput);
	if (FAILED(hr))
		goto Error;

	// Attempt to open the file
	TraceSz1(Verbose, "AUTOUPD: Opening patch file %s.", pautoupd->rgbOutput);

	hFile = CreateFile((LPCSTR)pautoupd->rgbOutput,
				GENERIC_READ, 
				0, 	// No sharing to prevent mishaps
				NULL, 
				OPEN_EXISTING, 
				FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, 
				NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		hr = HRESULT_FROM_WIN32(GetLastError());
		goto Error;
	}

	// Figure out how big the patch file is ...
	if (!GetFileSizeEx(hFile, &liSize))
	{
		hr = HRESULT_FROM_WIN32(GetLastError());
		goto Error;
	}

	// Make sure the size is the same as in the digest file record
	Assert(pautoupd->pdfdPatch->wSections == 1);
	if (liSize.QuadPart != pautoupd->pdfdPatch->dwFileSize)
	{
		Assert(FALSE);
		hr = HRESULT_FROM_WIN32(ERROR_INVALID_DATA);
		goto Error;
	}

	// Read the patch file header
	if (!ReadFile(hFile, pautoupd->rgbPatchData, 
				AUTOUPD_FILE_MAX_HEADER_SIZE, &cbSize, NULL))
	{
		hr = HRESULT_FROM_WIN32(GetLastError());
		goto Error;
	}

	// Parse the header info into our struct
	pautoupd->cbapfInfo = AUTOUPD_FILE_MAX_HEADER_SIZE;
	pautoupd->papfInfo = (PAUTOUPD_PATCH_FILE_INFO)pautoupd->rgbHeaderInfo;
	hr = ParsePatchHeader(
				pautoupd->rgbPatchData, &cbSize, 
				pautoupd->rgbHeaderInfo, &(pautoupd->cbapfInfo));
	if (FAILED(hr))
		goto Error;

	// Run the checksum for the header data
	Assert(pautoupd->pdfdPatch->rgSectionData[0].dwSectionStart == 0);
	Assert(pautoupd->pdfdPatch->rgSectionData[0].dwSectionSize == liSize.QuadPart);
	XcSHAInit(pautoupd->shactxPatch);
	XcSHAUpdate(pautoupd->shactxPatch, pautoupd->rgbPatchData, cbSize);

	AUTOUPD_BUMP_PROGRESS(&pautoupd->progress, cbSize);

	// Figure out what size to use for the LZX decoder for this file
	if (pautoupd->papfInfo->dwWindowSizeBits)
	{
		// Window size is explicitly specified, use it as long as it is
		// in range
		pautoupd->dwWindowSize = (1 << pautoupd->papfInfo->dwWindowSizeBits);
		if ((pautoupd->papfInfo->dwWindowSizeBits > 31) ||
			(pautoupd->dwWindowSize > AUTOUPD_MAX_WINDOW_SIZE))
		{
			hr = HRESULT_FROM_WIN32(ERROR_INVALID_DATA);
			goto Error;
		}
	}
	else
	{
		// Figure out based on the new and old data sizes
		pautoupd->dwWindowSize = AutoupdateLzxWindowSize(
					pautoupd->papfInfo->dwOldFileSize,
					pautoupd->papfInfo->dwNewFileSize);
	}

	TraceSz1(Verbose, "AUTOUPD: Using %u bytes for decode window", 
				pautoupd->dwWindowSize);

	// Set up the LZX decoder; every file can have its own buffer size
	LzxDecodeInit(pautoupd->rgbDecodeWindow,
				pautoupd->dwWindowSize,
				&(pautoupd->lzx));

	// Enable extra long matches for the differential patch engine
	LzxEnableExtraLongMatches(&(pautoupd->lzx));
	
	// Set the file pointer where the patch data starts
	liSize.QuadPart = cbSize;
	if (!SetFilePointerEx(hFile, liSize, &liSize, FILE_BEGIN))
	{
		hr = HRESULT_FROM_WIN32(GetLastError());
		goto Error;
	}

	// Hand over the handle to pautoupd
	pautoupd->hPatch = hFile;

	// Move to the next state
	if (!pautoupd->papfInfo->dwPatchDataSize)
		pautoupd->State = xoupdCloseFiles;
	else
		pautoupd->State = xoupdOpenSource;

Exit:	
	return(hr);

Error:
	if (hFile != INVALID_HANDLE_VALUE)
		CloseHandle(hFile);
	goto Exit;
}

HRESULT CXo::xoupdOpenSourceHandler(
			PXONLINEAUTOUPD_ONLINE	pautoupd
			)
{
	HRESULT			hr = S_OK;
	HANDLE			hFile = INVALID_HANDLE_VALUE;
	LARGE_INTEGER	liSize;
	DWORD			cbSize;

	Assert(pautoupd);

	// Build the location
	Assert(pautoupd->szCurrentFile);
	cbSize = MAX_PATH;
	hr = BuildTargetPath(0, 0, 0, pautoupd->szCurrentFile, pautoupd->dirtypeSource, 
				&cbSize, (char *)pautoupd->rgbOutput);
	if (FAILED(hr))
		goto Error;

	// Attempt to open the file
	TraceSz1(Verbose, "AUTOUPD: Opening source file %s.", pautoupd->rgbOutput);

	hFile = CreateFile((LPCSTR)pautoupd->rgbOutput,
				GENERIC_READ, 
				0, 	// No sharing to prevent mishaps
				NULL, 
				OPEN_EXISTING, 
				FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, 
				NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		hr = HRESULT_FROM_WIN32(GetLastError());
		goto Error;
	}

	// Figure out how big the patch file is ...
	if (!GetFileSizeEx(hFile, &liSize))
	{
		hr = HRESULT_FROM_WIN32(GetLastError());
		goto Error;
	}

	// Save the file size for later
	pautoupd->dwSourceSize = liSize.u.LowPart;

	// Hand over the file handle to pautoupd
	pautoupd->hSource = hFile;

	// Move to the next state
	pautoupd->State = xoupdCreateTarget;
	
Exit:	
	return(hr);

Error:
	if (hFile != INVALID_HANDLE_VALUE)
		CloseHandle(hFile);
	goto Exit;
}

HRESULT CXo::xoupdCreateTargetHandler(
			PXONLINEAUTOUPD_ONLINE	pautoupd
			)
{
	HRESULT	hr = S_OK;
	HANDLE	hFile = INVALID_HANDLE_VALUE;
	DWORD	cbSize;
	PAUTOUPD_PATCH_FILE_INFO	papfInfo;

	Assert(pautoupd);

	// Get the next entry in the digest file
	hr = EnumerateDigestFind(
			&(pautoupd->dgstenumTarget), 
			pautoupd->szCurrentFile, &(pautoupd->pdfdTarget));
	if (FAILED(hr))
		goto Error;

	// Make sure the digest file size and the expected size match
	Assert(pautoupd->pdfdTarget);
	papfInfo = pautoupd->papfInfo;
	if (papfInfo->dwNewFileSize != pautoupd->pdfdTarget->dwFileSize)
	{
		Assert(FALSE);
		hr = HRESULT_FROM_WIN32(ERROR_INVALID_DATA);
		goto Error;
	}

	// Build the target path
	cbSize = MAX_PATH;
	hr = BuildTargetPath(0, 0, 0, pautoupd->szCurrentFile, 
				pautoupd->dirtypeTarget, &cbSize, (char *)pautoupd->rgbOutput);
	if (FAILED(hr))
		goto Error;

	// We will always create a new file.
	hFile = CreateFile((LPCSTR)pautoupd->rgbOutput, 
				GENERIC_READ | GENERIC_WRITE, 
				FILE_SHARE_READ, 
				NULL, 
				CREATE_ALWAYS, 
				FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, 
				NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		DWORD	dwError = GetLastError();
		if (dwError == ERROR_PATH_NOT_FOUND)
		{
			// Directory does not exist, go ahead and create it
			hr = CreateDirectoryInitializeContext(
						(char *)pautoupd->rgbOutput, 
						cbSize,
						XONLINETASK_CDFLAG_FILE_PATH,
						pautoupd->xontask.hEventWorkAvailable,
						&pautoupd->dirops);
			if (FAILED(hr))
				goto Error;

			// Process this subtask
			pautoupd->State = xoupdCreateDirectory;
			goto Exit;
		}

		hr = HRESULT_FROM_WIN32(dwError);
		goto Error;
	}

	// Initialize the SHA context
	XcSHAInit(pautoupd->shactxTarget);

	// Hand over the file to pautoupd
	pautoupd->hTarget = hFile;
	
	// File created, move on to next state
	pautoupd->State = xoupdReadSource;

Exit:
	return(hr);

Error:
	if (hFile != INVALID_HANDLE_VALUE)
		CloseHandle(hFile);
	goto Exit;
}

HRESULT CXo::xoupdCreateDirectoryHandler(
			PXONLINEAUTOUPD_ONLINE	pautoupd
			)
{
	HRESULT	hr = S_OK;
	XONLINETASK_HANDLE	hdirops;

	Assert(pautoupd);

	// Retrieve the dirops structure
	hdirops = (XONLINETASK_HANDLE)&(pautoupd->dirops);
	hr = XOnlineTaskContinue(hdirops);
	if (XONLINETASK_STATUS_AVAILABLE(hr))
	{
		// Create dir had returned results, see if it succeeded
		if (FAILED(hr))
			goto Exit;

		// Go back to create target phase
		hr = S_OK;
		pautoupd->State = xoupdCreateTarget;
	}

Exit:
	return(hr);
}

HRESULT CXo::xoupdReadSourceHandler(
			PXONLINEAUTOUPD_ONLINE	pautoupd
			)
{
	HRESULT	hr = S_OK;
	DWORD			cbRead;
	LARGE_INTEGER		liSize;
	LARGE_INTEGER		liOffset;
	PAUTOUPD_PATCH_FILE_INFO	papfInfo;
	PAUTOUPD_INTERLEAVE_ENTRY	paie;

	Assert(pautoupd);
	Assert(pautoupd->hSource != INVALID_HANDLE_VALUE);

	// Make sure what we are reading makes sense
	papfInfo = pautoupd->papfInfo;
	Assert(papfInfo);	
	if (papfInfo->dwInterleaveCurrent >= papfInfo->dwInterleaveEntries)
	{
		Assert(FALSE);
		hr = HRESULT_FROM_WIN32(ERROR_INVALID_DATA);
		goto Error;
	}
	
	paie = papfInfo->rgInterleaveInfo + papfInfo->dwInterleaveCurrent;
	if (((paie->dwOldOffset + paie->dwOldLength) > pautoupd->dwSourceSize) ||
		(paie->dwOldLength > pautoupd->dwWindowSize))
	{
		Assert(FALSE);
		hr = HRESULT_FROM_WIN32(ERROR_INVALID_DATA);
		goto Error;
	}

	// OK, read the specified region
	liOffset.QuadPart = paie->dwOldOffset;
	if (!SetFilePointerEx(pautoupd->hSource, liOffset, &liOffset, FILE_BEGIN))
	{
		hr = HRESULT_FROM_WIN32(GetLastError());
		goto Error;
	}

	if (!ReadFile(pautoupd->hSource, pautoupd->rgbDecodeWindow, 
				paie->dwOldLength, &cbRead, NULL))
	{
		hr = HRESULT_FROM_WIN32(GetLastError());
		goto Error;
	}
	Assert(paie->dwOldLength == cbRead);

	// Insert this into the LZX dictionary
	if (!LzxPopulateDictionary(&pautoupd->lzx, 
				pautoupd->rgbDecodeWindow, paie->dwOldLength))
	{
		hr = HRESULT_FROM_WIN32(GetLastError());
		goto Error;
	}

	AUTOUPD_BUMP_PROGRESS(&pautoupd->progress, paie->dwOldLength);

	// Reset file progress
	papfInfo->dwTargetWritten = 0;

	// Move on to the next state
	pautoupd->State = xoupdReadPatch;
	
Exit:
	return(hr);

Error:
	goto Exit;
}

HRESULT CXo::xoupdReadPatchHandler(
			PXONLINEAUTOUPD_ONLINE	pautoupd
			)
{
	HRESULT	hr = S_OK;
	DWORD	cbRead;
	PAUTOUPD_PATCH_FILE_INFO	papfInfo;

	Assert(pautoupd);
	Assert(pautoupd->hPatch != INVALID_HANDLE_VALUE);

	papfInfo = pautoupd->papfInfo;
	Assert(papfInfo);

	// We can come here as a result of xoupdReadSource, but we might
	// have already read all the data. In this case, we will jump directly
	// to the next state
	if (papfInfo->dwPatchDataSize == 0)
		goto Skip;

	// If we have any unused patch data, move it to the front of
	// the buffer and account for it ...
	Assert(papfInfo->dwDataSize >= papfInfo->dwCurrent);
	papfInfo->dwDataSize -= papfInfo->dwCurrent;
	if (papfInfo->dwDataSize)
	{
		MoveMemory(pautoupd->rgbPatchData, 
					pautoupd->rgbPatchData + papfInfo->dwCurrent,
					papfInfo->dwDataSize);
	}
	papfInfo->dwCurrent = 0;

	// Read in the patch data, one buffer at a time
	if (!ReadFile(pautoupd->hPatch, 
				pautoupd->rgbPatchData + papfInfo->dwDataSize,
				AUTOUPD_PATCH_BUFFER_SIZE - papfInfo->dwDataSize, 
				&cbRead, NULL))
	{
		hr = HRESULT_FROM_WIN32(GetLastError());
		goto Error;
	}

	// Run the checksum for the NEW patch data
	XcSHAUpdate(pautoupd->shactxPatch, 
				pautoupd->rgbPatchData + papfInfo->dwDataSize, cbRead);

	AUTOUPD_BUMP_PROGRESS(&pautoupd->progress, cbRead);

	// Adjust our markers
	// Note: the patch generation program is known to create patch files that
	// are slightly larger than needed. We will allow for that with caution.
	if (cbRead > papfInfo->dwPatchDataSize)
		cbRead = papfInfo->dwPatchDataSize;
	papfInfo->dwDataSize += cbRead;
	papfInfo->dwPatchDataSize -= cbRead;

	// Another sanity check. This is important because this should
	// never happen
	if (*(WORD *)(pautoupd->rgbPatchData) >= AUTOUPD_PATCH_BUFFER_SIZE)
	{
		Assert(FALSE);
		hr = HRESULT_FROM_WIN32(ERROR_INVALID_DATA);
		goto Error;
	}

Skip:
	// Move to the next state
	pautoupd->State = xoupdDecodeData;

Exit:	
	return(hr);

Error:
	goto Exit;
}

HRESULT CXo::xoupdDecodeDataHandler(
			PXONLINEAUTOUPD_ONLINE	pautoupd
			)
{
	HRESULT	hr = S_OK;
	DWORD	dwRemaining;
	DWORD	cbPatchData;
	PAUTOUPD_PATCH_FILE_INFO	papfInfo;
	PAUTOUPD_INTERLEAVE_ENTRY	paie;

	Assert(pautoupd);

	// Figure out what we need to do next
	papfInfo = pautoupd->papfInfo;
	Assert(papfInfo);	
	Assert(papfInfo->dwInterleaveCurrent <= papfInfo->dwInterleaveEntries);
	
	paie = papfInfo->rgInterleaveInfo + papfInfo->dwInterleaveCurrent;
	if (papfInfo->dwTargetWritten >= paie->dwNewLength)
	{
		Assert(papfInfo->dwTargetWritten == paie->dwNewLength);

		// See if we are done with this file
		(papfInfo->dwInterleaveCurrent)++;
		if (papfInfo->dwInterleaveCurrent == papfInfo->dwInterleaveEntries)
		{
			// Done with this file. Close all resources
			pautoupd->State = xoupdCloseFiles;
		}
		else
		{
			// Not done yet, load the next interleave entry
			pautoupd->State = xoupdReadSource;
		}
		goto Exit;
	}

	// We are still working on this file.
	dwRemaining = paie->dwNewLength - papfInfo->dwTargetWritten;
	if (dwRemaining > CHUNK_SIZE)
		dwRemaining = CHUNK_SIZE;

	cbPatchData = *(WORD *)(pautoupd->rgbPatchData + papfInfo->dwCurrent);
	if ((papfInfo->dwCurrent + cbPatchData + sizeof(WORD)) > papfInfo->dwDataSize)
	{
		// There is not enough patch data left in the buffer, we will
		// need to load more patch data
		pautoupd->State = xoupdReadPatch;
		goto Exit;
	}

	// Skip past the length WORD
	papfInfo->dwCurrent += 2;

	// Decode tis chunk of data
	if (!LzxDecode(&pautoupd->lzx, dwRemaining, 
				pautoupd->rgbPatchData + papfInfo->dwCurrent,
				cbPatchData, pautoupd->rgbOutput, &pautoupd->cbDecoded))
	{
		hr = HRESULT_FROM_WIN32(ERROR_INVALID_DATA);
		goto Error;
	}
	Assert(pautoupd->cbDecoded == dwRemaining);

	// Consume the patch data
	papfInfo->dwCurrent += cbPatchData;

	// Write out this data
	pautoupd->State = xoupdWriteTarget;
	
Exit:	
	return(hr);

Error:
	goto Exit;
}

HRESULT CXo::xoupdWriteTargetHandler(
			PXONLINEAUTOUPD_ONLINE	pautoupd
			)
{
	HRESULT	hr = S_OK;
	DWORD	cbSize;

	Assert(pautoupd);
	Assert(pautoupd->hTarget != INVALID_HANDLE_VALUE);

	// Write out the data
	if (!WriteFile(pautoupd->hTarget, pautoupd->rgbOutput,
				pautoupd->cbDecoded, &cbSize, NULL))
	{
		hr = HRESULT_FROM_WIN32(GetLastError());
		goto Error;
	}
	Assert(cbSize == pautoupd->cbDecoded);

	// Run the checksum
	XcSHAUpdate(pautoupd->shactxTarget, 
				pautoupd->rgbOutput, pautoupd->cbDecoded);

	// Update the pointers
	pautoupd->papfInfo->dwTargetWritten += pautoupd->cbDecoded;

	// Return to decode pahse
	pautoupd->State = xoupdDecodeData;

Exit:
	return(hr);

Error:
	goto Exit;
}

HRESULT CXo::xoupdCloseFilesHandler(
			PXONLINEAUTOUPD_ONLINE	pautoupd
			)
{
	HRESULT	hr = S_OK;
	BYTE	rgbDigest[XCALCSIG_SIGNATURE_SIZE];

	Assert(pautoupd);

	if (pautoupd->papfInfo->dwPatchDataSize)
	{
		Assert(pautoupd->pdfdPatch);
		Assert(pautoupd->pdfdTarget);

		// Compute the final checksum for the patch file and make
		// sure it is authentic
		XcSHAFinal(pautoupd->shactxPatch, rgbDigest);
		if (memcmp(pautoupd->pdfdPatch->rgSectionData[0].rgbSignature,
					rgbDigest, XCALCSIG_SIGNATURE_SIZE) != 0)
		{
			hr = HRESULT_FROM_WIN32(ERROR_INVALID_DATA);
			goto Error;
		}

		// Compute the final checksum for the Target file and make
		// sure it is authentic
		XcSHAFinal(pautoupd->shactxTarget, rgbDigest);
		if (memcmp(pautoupd->pdfdTarget->rgSectionData[0].rgbSignature,
					rgbDigest, XCALCSIG_SIGNATURE_SIZE) != 0)
		{
			hr = HRESULT_FROM_WIN32(ERROR_INVALID_DATA);
			goto Error;
		}
	}

	TraceSz1(Verbose, "AUTOUPD: Files verified for %s.", pautoupd->szCurrentFile);

	// Move on to the next patch file
	pautoupd->State = xoupdOpenPatch;

Exit:

	// In any case, close all files
	if (pautoupd->hPatch != INVALID_HANDLE_VALUE)
	{
		CloseHandle(pautoupd->hPatch);
		pautoupd->hPatch = INVALID_HANDLE_VALUE;
	}
	if (pautoupd->hSource != INVALID_HANDLE_VALUE)
	{
		CloseHandle(pautoupd->hSource);
		pautoupd->hSource = INVALID_HANDLE_VALUE;
	}
	if (pautoupd->hTarget != INVALID_HANDLE_VALUE)
	{
		CloseHandle(pautoupd->hTarget);
		pautoupd->hTarget = INVALID_HANDLE_VALUE;
	}

	return(hr);

Error:
	goto Exit;
}

HRESULT CXo::xoupdCopyDigestHandler(
			PXONLINEAUTOUPD_ONLINE	pautoupd
			)
{
	HRESULT	hr = S_OK;
	HANDLE	hFile = INVALID_HANDLE_VALUE;
	DWORD	cbSize;
	LARGE_INTEGER		liSize;
	DGSTFILE_IDENTIFIER	dfi;

	// Copy the unsigned digest over to the destination
	cbSize = MAX_PATH;
	hr = BuildTargetPath(0, 0, 0, 
				XONLINEAUTOUPD_TARGET_MANIFEST_FILE_NAME,
				dirtypeTempDiff, &cbSize, (char *)pautoupd->rgbDecodeWindow);
	if (FAILED(hr))
		goto Error;

	cbSize = MAX_PATH;
	hr = BuildTargetPath(0, 0, 0, 
				XONLINECONTENT_MANIFEST_FILE_NAME,
				pautoupd->dirtypeTarget, &cbSize, (char *)pautoupd->rgbOutput);
	if (FAILED(hr))
		goto Error;

	if (!CopyFile((LPCSTR)pautoupd->rgbDecodeWindow, (LPCSTR)pautoupd->rgbOutput, TRUE))		
	{
		hr = HRESULT_FROM_WIN32(GetLastError());
		goto Error;
	}

	// Open the digest file
	TraceSz1(Verbose, "AUTOUPD: Opening digest %s for signing.", pautoupd->rgbOutput);

	hFile = CreateFile((LPCSTR)pautoupd->rgbOutput,
				GENERIC_READ | GENERIC_WRITE, 
				0, 	// No sharing to prevent mishaps
				NULL, 
				OPEN_EXISTING, 
				FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN |
				FILE_FLAG_OVERLAPPED | FILE_FLAG_NO_BUFFERING, 
				NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		hr = HRESULT_FROM_WIN32(GetLastError());
		goto Error;
	}

	if (!GetFileSizeEx(hFile, &liSize))
	{
		hr = HRESULT_FROM_WIN32(GetLastError());
		goto Error;
	}

	// Now set up to sign this digest later
	dfi.dwTitleId = m_dwTitleId;
	dfi.OfferingId = 0;
	dfi.wTitleVersionHi = pautoupd->wOldVersionHi;
	dfi.wTitleVersionLo = pautoupd->wOldVersionLo;
	hr = ContentVerifyInitializeContext(
				hFile, liSize.u.LowPart,
				pautoupd->rgbTargetDigest, 
				XONLINECONTENT_MANIFEST_MAX_FILE_SIZE,
				pautoupd->rgbTitleKey,
				XONLINECONTENT_VERIFY_FLAG_SIGN,
				&dfi, pautoupd->dirtypeTarget,
				pautoupd->xontask.hEventWorkAvailable,
				&pautoupd->contver);
	if (FAILED(hr))
		goto Error;

	// Kick it off!
	hr = ContentVerifyBegin(&pautoupd->contver);
	if (FAILED(hr))
		goto Error;

	// Figure out what to do next
	if (pautoupd->dirtypeTarget == dirtypeTitleUpdateOnF)
		pautoupd->State = xoupdSignDigest;
	else
	{
		// Dash update, now we need to copy the other files.
		// We set the noreplace file to make sure we don't 
		// overwrite any existing patched files.
		sprintf((char *)pautoupd->rgbDecodeWindow, AUTOUPD_DASH_TARGET_PATH, 
				(pautoupd->dirtypeSource == dirtypeDashUpdate1)?'1':'0');
		sprintf((char *)pautoupd->rgbPatchData, AUTOUPD_DASH_TARGET_PATH, 
				(pautoupd->dirtypeTarget == dirtypeDashUpdate1)?'1':'0');
		hr = CopyDirectoryInitializeContext(
					(char *)pautoupd->rgbDecodeWindow, 
					(char *)pautoupd->rgbPatchData, MAX_PATH, 
					XONLINETASK_DCFLAG_NOREPLACE,
					pautoupd->xontask.hEventWorkAvailable,
					&(pautoupd->dircopy));
		if (FAILED(hr))
			goto Error;
			
		pautoupd->State = xoupdCopyOtherFiles;
	}

Exit:	
	return(hr);

Error:
	if (hFile != INVALID_HANDLE_VALUE)
		CloseHandle(hFile);
	goto Exit;
}

HRESULT CXo::xoupdCopyOtherFilesHandler(
			PXONLINEAUTOUPD_ONLINE	pautoupd
			)
{
	HRESULT	hr = S_OK;

	Assert(pautoupd);

	// Wait for the subtask to complete
	hr = XOnlineTaskContinue((XONLINETASK_HANDLE)&(pautoupd->dircopy));
	if (XONLINETASK_STATUS_AVAILABLE(hr))
	{
		// Returned results, see if it succeeded
		if (FAILED(hr))
			goto Error;
		hr = S_OK;

		// Move on to sign the digest
		pautoupd->State = xoupdSignDigest;
	}

Exit:	
	return(hr);

Error:
	goto Exit;
}

HRESULT CXo::xoupdSignDigestHandler(
			PXONLINEAUTOUPD_ONLINE	pautoupd
			)
{
	HRESULT				hr = S_OK;
	AUTOUPD_DASH_CONFIG	adc;
	
	Assert(pautoupd);

	// Wait for the subtask to complete
	hr = XOnlineTaskContinue((XONLINETASK_HANDLE)&(pautoupd->contver));
	if (XONLINETASK_STATUS_AVAILABLE(hr))
	{
		// Returned results, see if it succeeded
		if (FAILED(hr))
			goto Error;
		hr = S_OK;

		// Cleanup the task
		contverCleanup(&pautoupd->contver);

		// Now the update is completely installed, update the boot
		// switcher to boot to this version
		if (pautoupd->dirtypeTarget != dirtypeTitleUpdateOnF)
		{
			adc.dwActiveDirectory = 
					(pautoupd->dirtypeTarget == dirtypeDashUpdate1)?1:0;
			hr = XoUpdateWriteDashConfig(&adc);
			if (FAILED(hr))
				goto Error;
		}

		// Move on to sign the digest
		pautoupd->State = xoupdCleanupTemp;
	}

Exit:	
	return(hr);

Error:
	goto Exit;
}

HRESULT CXo::xoupdWriteConfigHandler(
			PXONLINEAUTOUPD_ONLINE	pautoupd
			)
{
	HRESULT				hr = S_OK;
	AUTOUPD_DASH_CONFIG	adc;

	Assert(pautoupd);

	// If this is the first dash update, we will also need to install 
	// the switcher xbe in the root
	if (pautoupd->fFirstDashUpdate)
	{
		// We will clobber the old one if it exists
		hr = XoUpdateInstallSwitcher(dirtypeDashUpdate0);
		if (FAILED(hr))
			goto Error;
	}

	// Now the update is completely installed, update the boot
	// switcher to boot to this version
	adc.dwActiveDirectory = 
			(pautoupd->dirtypeTarget == dirtypeDashUpdate1)?1:0;
	hr = XoUpdateWriteDashConfig(&adc);
	if (FAILED(hr))
		goto Error;

	// Cleanup temp
	pautoupd->State = xoupdCleanupTemp;

Exit:
	return(hr);

Error:
	goto Exit;
}

HRESULT CXo::xoupdCleanupTempHandler(
			PXONLINEAUTOUPD_ONLINE	pautoupd
			)
{
	HRESULT	hr = S_OK;

	// Just to try to hide our patch mechanism, we will 
	// format our temp partition before we complete
	if (pautoupd->dirtypeSource != dirtypeNone)
	{
		if (!XFormatUtilityDrive())
		{
			hr = HRESULT_FROM_WIN32(GetLastError());
			goto Error;
		}
	}

	if (pautoupd->fFirstDashUpdate)
	{
		pautoupd->NextState = xoupdDone;
		pautoupd->State = xoupdCleanupOfflineDashFiles;
	}
	else
		pautoupd->State = xoupdDone;		

Exit:		
	return(hr);

Error:
	goto Exit;
}

HRESULT CXo::xoupdCleanupOfflineDashFilesHandler(
			PXONLINEAUTOUPD_ONLINE	pautoupd
			)
{
	HRESULT	hr = S_OK;

	// Wait for the subtask to complete
	hr = XOnlineTaskContinue((XONLINETASK_HANDLE)&(pautoupd->xondod));
	if (XONLINETASK_STATUS_AVAILABLE(hr))
	{
		// Returned results, see if it succeeded
		if (FAILED(hr))
			goto Error;
		hr = S_OK;

		// Figure out next state
		pautoupd->State = pautoupd->NextState;
		pautoupd->NextState = xoupdDone;
	}

Exit:
	return(S_OK);

Error:
	goto Exit;
}

// =================================================================

//
// Task handler type
//
typedef HRESULT (CXo::*PFNAUTOUPD_ONLINE_HANDLER)(PXONLINEAUTOUPD_ONLINE);

//
// Define the handler vector
//
const PFNAUTOUPD_ONLINE_HANDLER CXo::s_rgpfnxoupdHandlers[] =
{
	// Pre-install handlers
	CXo::xoupdPreInitializeHandler,
	CXo::xoupdCountFilesHandler,
	CXo::xoupdFormatTempHandler,
	CXo::xoupdDoneDVDPreInitHandler,

	// Post-install handlers
	CXo::xoupdPostInitializeHandler,
	CXo::xoupdVerifyPatchDigestHandler,
	CXo::xoupdVerifyTargetDigestHandler,
	CXo::xoupdWipeTargetHandler,
	CXo::xoupdOpenPatchHandler,
	CXo::xoupdOpenSourceHandler,
	CXo::xoupdCreateTargetHandler,
	CXo::xoupdCreateDirectoryHandler,
	CXo::xoupdReadSourceHandler,
	CXo::xoupdReadPatchHandler,
	CXo::xoupdDecodeDataHandler,
	CXo::xoupdWriteTargetHandler,
	CXo::xoupdCloseFilesHandler,
	CXo::xoupdCopyDigestHandler,
	CXo::xoupdCopyOtherFilesHandler,
	CXo::xoupdSignDigestHandler,
	CXo::xoupdWriteConfigHandler,
	CXo::xoupdCleanupTempHandler,
	CXo::xoupdCleanupOfflineDashFilesHandler
};

//
// Implement the top-level do work function
//
// This is shared among the contdlPreCustom and contdlPostCustom
// custom task handlers in XOnlineContentInstall
//
HRESULT CXo::xoupdContinue(
			XONLINETASK_HANDLE	hTask
			)
{
	HRESULT						hr = S_OK;
	PXONLINETASK_AUTOUPD_ONLINE	pxoupd;
	PXONLINEAUTOUPD_ONLINE		pautoupd = (PXONLINEAUTOUPD_ONLINE)hTask;
	PXRL_ASYNC					pxrlasync;
	AUTOUPD_ONLINE_STATES		State;

	Assert(pautoupd);

	pxoupd = CONTAINING_RECORD(pautoupd, XONLINETASK_AUTOUPD_ONLINE, autoupd);
	pxrlasync = &(pxoupd->contdl.xrlasync);

	State = pautoupd->State;

	// Figure out what phase we are in ...
	if (pautoupd->State < xoupdDone)
	{
		hr = (this->*(s_rgpfnxoupdHandlers[State]))(pautoupd);

#if DBG
		if (FAILED(hr))
		{
			DWORD	tagOld = t_Verbose;
			t_Verbose = TAG_ENABLE;
			TraceSz2(Verbose, "%s, %08x", g_rgszxoupdStates[State], hr);
			t_Verbose = tagOld;
		}
#endif		
	}
	else if (pautoupd->State != xoupdDone)
	{
		// This is a bad state. We force it to end so we don't get stuck
		// here forever.
		AssertSz(FALSE, "xoupdContinue: invalid phase!");
		hr = E_FAIL;
	}

	// Bail if failed.
	if (FAILED(hr))
	{
		pxrlasync->hrFinal = hr;
		pautoupd->State = xoupdDone;
	}

	// When the next state is the final state, we set the flag so we don't
	// continue.
	if (pautoupd->State >= xoupdDone)
	{
		// Indicate that we are done and return the final code
		hr = pxrlasync->hrFinal;
		XONLINETASK_SET_COMPLETION_STATUS(hr);
	}

	// If we are making progress, make sure we update the last 
	// state change timestamp so XRL will not timeout
	if (pautoupd->State != State)
		pxrlasync->dwLastStateChange = GetTickCount();
	
	return(hr);
}				

//
// Function to do a master cleanup. This always comes from XRL
//
VOID CXo::AutoupdateCleanup(
			PVOID	pvxrlasync
			)
{
	PXRL_ASYNC						pxrlasync = (PXRL_ASYNC)pvxrlasync;
	PXONLINETASK_AUTOUPD_ONLINE		pxoupd;
	PXONLINETASK_CONTENT_DOWNLOAD	pcontdl;
	PXONLINEAUTOUPD_ONLINE			pautoupd;

	Assert(pxrlasync);

	pcontdl = CONTAINING_RECORD(pxrlasync, XONLINETASK_CONTENT_DOWNLOAD, xrlasync);
	pxoupd = CONTAINING_RECORD(pcontdl, XONLINETASK_AUTOUPD_ONLINE, contdl);
	pautoupd = &pxoupd->autoupd;

	// Close all explicit handles
	if (pautoupd->hPatch != INVALID_HANDLE_VALUE)
	{
		CloseHandle(pautoupd->hPatch);
		pautoupd->hPatch = INVALID_HANDLE_VALUE;
	}
	if (pautoupd->hSource != INVALID_HANDLE_VALUE)
	{
		CloseHandle(pautoupd->hSource);
		pautoupd->hSource = INVALID_HANDLE_VALUE;
	}
	if (pautoupd->hTarget != INVALID_HANDLE_VALUE)
	{
		CloseHandle(pautoupd->hTarget);
		pautoupd->hTarget = INVALID_HANDLE_VALUE;
	}

	// Unmount any mapped drives
	if (pautoupd->fFDriveMounted)
	{
		XoUpdateUnmountPrivateDrive();
		pautoupd->fFDriveMounted = FALSE;
	}

	// Cleanup any subtask contexts
	contverCleanup(&pautoupd->contver);
	DiropsCleanup(&pautoupd->dirops);
	DircrawlCleanup(&(pautoupd->dircopy.dircrawl));

	contdlCleanup(pcontdl);
}

//
// Function to initialize the context for an online autoupdate
//
// Note: this assumes ContentInstallInitializeContext is already called
//
HRESULT CXo::AutoupdateInitializeContext(
			PXONLINETASK_AUTOUPD_ONLINE	pxoupd,
			DWORD						dwTitleId,
			DWORD						dwTitleOldVersion,
			BOOL						fOnline,
			PBYTE						pbTitleKey,
			HANDLE						hWorkEvent
			)
{
	HRESULT							hr = S_OK;
	PXONLINETASK_CONTENT_DOWNLOAD	pcontdl = NULL;
	PXONLINEAUTOUPD_ONLINE			pautoupd = NULL;
	AUTOUPD_DASH_CONFIG				adc;

	Assert(pxoupd);

	pcontdl = &pxoupd->contdl;
	pautoupd = &pxoupd->autoupd;

	// Determine what kind of update this is, and set the target
	// locations appropriately.
	if (dwTitleId == AUTOUPD_DASH_TITLE_ID)
	{
		// We are updating the dash, we will need to find the active
		// partition and then set the target to the inactive partition.
		hr = XoUpdateMountDashPartition();
		if (FAILED(hr))
			goto Error;

		// Figure out the currently active copy of the dash
		hr = XoUpdateReadDashConfig(&adc, TRUE);
		if (FAILED(hr))
		{
			if (hr == HRESULT_FROM_WIN32(ERROR_NOT_FOUND))
			{
				// This is the offline dash
				adc.dwActiveDirectory = 1;
				pautoupd->fFirstDashUpdate = TRUE;
				hr = S_OK;
			}
			else
				goto Error;
		}

		// Set the target directory, fall back to 0 if necessary
		if (adc.dwActiveDirectory == 0)
			pautoupd->dirtypeTarget = dirtypeDashUpdate1;
		else
			pautoupd->dirtypeTarget = dirtypeDashUpdate0;

		if (fOnline)
		{
			// The offline dash cannot be updated via Online
			if (pautoupd->fFirstDashUpdate)
			{
				hr = HRESULT_FROM_WIN32(XONLINE_E_LOGON_UPDATE_REQUIRED);
				goto Error;
			}

			// Set the appropriate source dirtype
			pautoupd->dirtypeSource =
					(pautoupd->dirtypeTarget == dirtypeDashUpdate0)?
						dirtypeDashUpdate1:dirtypeDashUpdate0;
		}
		else
		{
			// Source directory type for offline updates is none
			pautoupd->dirtypeSource = dirtypeNone;
		}
				
	}
	else
	{
		// Title update is straightforward
		pautoupd->dirtypeTarget = dirtypeTitleUpdateOnF;
		
		if (fOnline)
			pautoupd->dirtypeSource = dirtypeDVDTitleRoot;
		else
			pautoupd->dirtypeSource = dirtypeNone;
	}

	// Online updates require a title key
	if (!fOnline)
		memcpy(pautoupd->rgbTitleKey, pbTitleKey, XBOX_KEY_LENGTH);				

	// One more caveat: we will override the cleanup function for
	// download to use our cleanup handler, which will cleanup
	// everything for download and for our stuff.
	pcontdl->xrlasync.xrlext.dl.pfnCleanup = AutoupdateCleanup;

	// Set up the autoupdate context
	TaskInitializeContext(&pautoupd->xontask);
	pautoupd->xontask.pfnContinue = xoupdContinue;
	pautoupd->xontask.hEventWorkAvailable = hWorkEvent;
	pautoupd->State = xoupdPreInitialize;
	pautoupd->NextState = xoupdDone;
	pautoupd->dwWindowSize = 0;

	MAKE_COMPOSITE_VERSION(dwTitleOldVersion,
				&pautoupd->wOldVersionHi, &pautoupd->wOldVersionLo);

	pautoupd->hPatch = INVALID_HANDLE_VALUE;
	pautoupd->hSource = INVALID_HANDLE_VALUE;
	pautoupd->hTarget = INVALID_HANDLE_VALUE;

	// If the target is to the title area, we will have to mount
	// the title to F drive
	if (pautoupd->dirtypeTarget == dirtypeTitleUpdateOnF)
	{
		hr = XoUpdateMountTitleTDataToPrivateDrive(dwTitleId);
		if (FAILED(hr))
			goto Error;

		pautoupd->fFDriveMounted = TRUE;
	}
	else
		pautoupd->fFDriveMounted = FALSE;

Exit:
	return(hr);

Error:
	AutoupdateCleanup(&pxoupd->contdl.xrlasync);
	goto Exit;
}

//
// Function to kick off an async online autoupdate
//
HRESULT CXo::AutoupdateOnline(
			DWORD				dwTitleId,
			DWORD				dwTitleOldVersion,
			PBYTE				pbTitleKey,
			HANDLE				hWorkEvent,
			PXONLINETASK_HANDLE	phTask
			)
{
	HRESULT							hr = S_OK;
	LARGE_INTEGER					liTemp;
	ULONGLONG						qwMachineId;
	LPCSTR							szDummy = "";
	PXONLINETASK_AUTOUPD_ONLINE		pxoupd = NULL;
	PXONLINETASK_CONTENT_DOWNLOAD	pcontdl;
	PXONLINEAUTOUPD_ONLINE			pautoupd;

	XoEnter("AutoupdateOnline");

	Assert(phTask);

	// First, allocate the context
	pxoupd = (PXONLINETASK_AUTOUPD_ONLINE)SysAllocZ(
			sizeof(XONLINETASK_AUTOUPD_ONLINE), PTAG_XONLINETASK_AUTOUPD_ONLINE);
	if (!pxoupd)
	{
		hr = E_OUTOFMEMORY;
		goto Error;
	}

	TraceSz1(Verbose, "AUTOUPD: Allocating %u bytes for context.", 
				sizeof(XONLINETASK_AUTOUPD_ONLINE));

	// Initialize the content download task
	pcontdl = &pxoupd->contdl;
	pautoupd = &pxoupd->autoupd;
	hr = ContentInstallInitializeContext(pcontdl, 
				dwTitleId, 0, dirtypeTempDiff, 0,
				XONLINECONTENT_INSTALL_TIMEOUT, 
				(XONLINETASK_HANDLE)pautoupd,	// hTaskPreCustom
				(XONLINETASK_HANDLE)pautoupd,	// hTaskPostCustom
				hWorkEvent);
	if (FAILED(hr))
		goto Error;

	// Content install will create an event if one was not specified
	// we pick it out from pcontdl->xrlasync.xontask
	hWorkEvent = pcontdl->xrlasync.xontask.hEventWorkAvailable;

	// Set up the autoupd context
	hr = AutoupdateInitializeContext(pxoupd, 
				dwTitleId, dwTitleOldVersion, TRUE,	// fOnline
				pbTitleKey, hWorkEvent);
	if (FAILED(hr))
		goto Error;

	// Get the current machine ID
	hr = _XOnlineGetMachineID(&qwMachineId);
	if (FAILED(hr))
		goto Error;

	// First, kick off an autoupdate referral request
	hr = InitiateAutoupdateReferralRequest(dwTitleId, 
				qwMachineId, dwTitleOldVersion, pxoupd);
	if (FAILED(hr))
		goto Error;

	// Kick off the generic download. At this point, we don't
	// care about the timestamp or resume point, because we 
	// will have to come up with that later.
	liTemp.QuadPart = 0;
	hr = Download(szDummy, NULL, 0, NULL, liTemp, &(pxoupd->contdl.xrlasync));
	if (FAILED(hr))
		goto Error;

	*phTask = (XONLINETASK_HANDLE)pxoupd;

Exit:
	return(XoLeave(hr));

Error:
	if (pxoupd)
		SysFree(pxoupd);
	goto Exit;
}

//
// Function to kick off an async DVD autoupdate
//
HRESULT CXo::AutoupdateFromDVD(
			PSTR				szUpdatePath,
			DWORD				dwTitleId,
			DWORD				dwTitleOldVersion,
			PBYTE				pbTitleKey,
			PBYTE				pbSymmetricKey,
			DWORD				cbSymmetricKey,
			PBYTE				pbPublicKey,
			DWORD				cbPublicKey,
			HANDLE				hWorkEvent,
			PXONLINETASK_HANDLE	phTask
			)
{
	HRESULT							hr = S_OK;
	LARGE_INTEGER					liTemp;
	PXONLINETASK_AUTOUPD_ONLINE		pxoupd = NULL;
	PXONLINETASK_CONTENT_DOWNLOAD	pcontdl;
	PXONLINEAUTOUPD_ONLINE			pautoupd;

	XoEnter("AutoupdateFromDVD");

	Assert(phTask);
	Assert(pbSymmetricKey);
	Assert(cbSymmetricKey == XONLINE_KEY_LENGTH);
	Assert(pbPublicKey);
	Assert(cbPublicKey >= XONLINECONTENT_PK_SIGNATURE_SIZE);
	Assert(cbPublicKey <= XONLINECONTENT_MAX_PUBLIC_KEY_SIZE);

	// First, allocate the context
	pxoupd = (PXONLINETASK_AUTOUPD_ONLINE)SysAllocZ(
			sizeof(XONLINETASK_AUTOUPD_ONLINE), PTAG_XONLINETASK_AUTOUPD_ONLINE);
	if (!pxoupd)
	{
		hr = E_OUTOFMEMORY;
		goto Error;
	}

	TraceSz1(Verbose, "AUTOUPD: Allocating %u bytes for context.", 
				sizeof(XONLINETASK_AUTOUPD_ONLINE));

	// Initialize the content download task
	pcontdl = &pxoupd->contdl;
	pautoupd = &pxoupd->autoupd;
	hr = ContentInstallInitializeContext(pcontdl, 
				dwTitleId, 0, dirtypeTempDiff,
				XONLINECONTENT_FLAG_OFFLINE_INSTALL,
				XONLINECONTENT_INSTALL_TIMEOUT, 
				(XONLINETASK_HANDLE)pautoupd,	// hTaskPreCustom
				(XONLINETASK_HANDLE)pautoupd,	// hTaskPostCustom
				hWorkEvent);
	if (FAILED(hr))
		goto Error;

	// Content install will create an event if one was not specified
	// we pick it out from pcontdl->xrlasync.xontask
	hWorkEvent = pcontdl->xrlasync.xontask.hEventWorkAvailable;

	// Set up the autoupd context
	hr = AutoupdateInitializeContext(pxoupd, 
				dwTitleId, dwTitleOldVersion, FALSE, // fOnline
				pbTitleKey, hWorkEvent);
	if (FAILED(hr))
		goto Error;

	// Force the target dirtype
	pcontdl->install.dirtypeTarget = pautoupd->dirtypeTarget;

	// Set up the cryptographic keys
	memcpy(pcontdl->install.rgbSymmetricKey, pbSymmetricKey, XONLINE_KEY_LENGTH);
	memcpy(pcontdl->install.rgbPublicKey, pbPublicKey, cbPublicKey);

	// Emulate a "download" from file
	liTemp.QuadPart = 0;
	hr = DownloadFromDVD(szUpdatePath, NULL, liTemp, &(pxoupd->contdl.xrlasync));
	if (FAILED(hr))
		goto Error;

	*phTask = (XONLINETASK_HANDLE)pxoupd;

Exit:
	return(XoLeave(hr));

Error:
	if (pxoupd)
		SysFree(pxoupd);
	goto Exit;
}

//
// Method to get the current update progress
//
HRESULT CXo::AutoupdateGetProgress(
			XONLINETASK_HANDLE	hTask,
			DWORD				*pdwPercentDone,
			ULONGLONG			*pqwNumerator,
			ULONGLONG			*pqwDenominator
			)
{
	HRESULT	hr = S_OK;
	PXONLINETASK_AUTOUPD_ONLINE		pxoupd = (PXONLINETASK_AUTOUPD_ONLINE)hTask;
	PXONLINETASK_CONTENT_DOWNLOAD	pcontdl;
	PXONLINEAUTOUPD_ONLINE			pautoupd;
	PAUTOUPD_PROGRESS_CONTEXT		papc;
	ULONGLONG						qwNumerator;
	ULONGLONG						qwDenominator;
	DWORD							dwPercentDone;

	XoEnter("AutoupdateGetProgress");

	Assert(pxoupd);

	pcontdl = &pxoupd->contdl;
	pautoupd = &pxoupd->autoupd;
	papc = &pautoupd->progress;

	// This is a perfect example of progress aggregation. 
	//
	// Progress is calculated on three phases:
	// i) Download & install
	// ii) Patch application
	// iii) Copying unchanged files (dash only)
	
	// First, we get the progress for the download
	hr = XOnlineContentInstallGetProgress(
				(XONLINETASK_HANDLE)pcontdl,
				&dwPercentDone,
				&qwNumerator, 
				&qwDenominator);
	if (FAILED(hr))
		goto Error;

	// We will need to add the patch application to the fraction
	qwDenominator += papc->uliBytesTotal.QuadPart;
	qwNumerator += papc->uliBytesProcessed.QuadPart;

	// We will also adjust the progress if unpatched files are to
	// be copied as well (will remain 0 if title update)
	qwNumerator += pautoupd->dircopy.uliBytesCopied.QuadPart;

	// Update the perect completed
	if (qwDenominator)
	{
		dwPercentDone = (DWORD)
					((qwNumerator * 100) / qwDenominator);
	}
	else
		dwPercentDone = 0;

	if (pdwPercentDone)
		*pdwPercentDone = dwPercentDone;
	if (pqwNumerator)
		*pqwNumerator = qwNumerator;
	if (pqwDenominator)
		*pqwDenominator = qwDenominator;
		
Exit:
	return(XoLeave(hr));

Error:
	goto Exit;
}

const OBJECT_STRING	   g_DDosDevicePrefix = CONSTANT_OBJECT_STRING(OTEXT("\\??\\D:"));
const CHAR             g_szDVDDevicePrefix[] = "\\Device\\Cdrom0";
const CHAR             g_szDashUpdater[] = "y:\\update.xbe";
const CHAR             g_szTitleUpdater[] = "d:\\update.xbe";

#ifndef ARRAYSIZE
#define ARRAYSIZE(a)    (sizeof(a)/sizeof((a)[0]))
#endif

DWORD RebootToUpdaterWhilePreservingDDrive(
		    LPCSTR lpTitlePath,
		    PLAUNCH_UPDATE_INFO plui
		    )
{
    NTSTATUS Status;
    OBJECT_ATTRIBUTES Obja;
    HANDLE LinkHandle;
    OCHAR szLinkTarget[MAX_PATH * 2];
    OCHAR szDDrivePath[MAX_PATH * 2];
    OBJECT_STRING LinkTarget;
    int cch;

    szDDrivePath[0] = '\0';

    //
    // If the D: drive is mapped in some unique fashion, preserve that mapping
    // across the reboot
    //

    InitializeObjectAttributes(&Obja,
                               (POBJECT_STRING) &g_DDosDevicePrefix,
                               OBJ_CASE_INSENSITIVE,
                               NULL,
                               NULL);

    Status = NtOpenSymbolicLinkObject(&LinkHandle, &Obja);

    if (NT_SUCCESS(Status))
    {
        LinkTarget.Buffer = szLinkTarget;
        LinkTarget.Length = 0;
        LinkTarget.MaximumLength = sizeof(szLinkTarget);

        Status = NtQuerySymbolicLinkObject(LinkHandle, &LinkTarget, NULL);

        NtClose(LinkHandle);
    }

    if (NT_SUCCESS(Status))
    {
        lstrcpynA(szDDrivePath,
                  szLinkTarget,
                  min(ARRAYSIZE(szDDrivePath), (LinkTarget.Length / sizeof(CHAR)) + 1));
    }
    else
    {
        /* Got no path.  The D: drive should always be mapped to something, so
         * we're in trouble if we get here.  Fall back on the DVD device
         * path */
        ASSERT(FALSE);
        strcpy(szDDrivePath, g_szDVDDevicePrefix);
    }

    return(XWriteTitleInfoAndReboot(&(lpTitlePath[3]),
                                    szDDrivePath,
                                    LDT_TITLE_UPDATE,
                                    XeImageHeader()->Certificate->TitleID,
                                    (PLAUNCH_DATA)plui));
}

#endif

DWORD CXo::XOnlineLaunchNewImage(
			LPCSTR			lpTitlePath,
			PLAUNCH_DATA	pLaunchData
			)
{
#ifdef XONLINE_FEATURE_XBOX
    //
    // We require the path is NULL or D:\something
    //

    if ((NULL != lpTitlePath) &&
        ((('D' != lpTitlePath[0]) && ('d' != lpTitlePath[0])) ||
         (':' != lpTitlePath[1]) ||
         ('\\' != lpTitlePath[2])))
    {
        XDBGWRN("XAPI", "XLaunchNewImage() ignoring invalid lpTitlePath (%s)", lpTitlePath);
        return ERROR_INVALID_PARAMETER;
    }

    if (NULL == lpTitlePath)
    {
        //
        // NULL lpTitlePath means reboot to the dashboard - don't preserve D: drive mapping
        //
		// Update the dash if necessary. Note this API will not return if 
		// the Dash needs updating.
        UpdateDashIfNecessary();

        return XWriteTitleInfoAndReboot(NULL,
                                        NULL,
                                        (NULL != pLaunchData) ? LDT_LAUNCH_DASHBOARD : LDT_NONE,
                                        XeImageHeader()->Certificate->TitleID,
                                        pLaunchData);
    }
    else
    {
        NTSTATUS Status;
        OBJECT_ATTRIBUTES Obja;
        HANDLE LinkHandle;
        OCHAR szLinkTarget[MAX_PATH * 2];
        OCHAR szDDrivePath[MAX_LAUNCH_PATH];
        OBJECT_STRING LinkTarget;
        int cch;

        szDDrivePath[0] = '\0';

        //
        // If the D: drive is mapped in some unique fashion, preserve that mapping
        // across the reboot
        //

        InitializeObjectAttributes(&Obja,
                                   (POBJECT_STRING) &g_DDosDevicePrefix,
                                   OBJ_CASE_INSENSITIVE,
                                   NULL,
                                   NULL);

        Status = NtOpenSymbolicLinkObject(&LinkHandle, &Obja);

        if (NT_SUCCESS(Status))
        {
            LinkTarget.Buffer = szLinkTarget;
            LinkTarget.Length = 0;
            LinkTarget.MaximumLength = sizeof(szLinkTarget);

            Status = NtQuerySymbolicLinkObject(LinkHandle, &LinkTarget, NULL);

            NtClose(LinkHandle);
        }

        if (NT_SUCCESS(Status))
        {
            lstrcpynA(szDDrivePath,
                      szLinkTarget,
                      min(ARRAYSIZE(szDDrivePath), (LinkTarget.Length / sizeof(CHAR)) + 1));
        }
        else
        {
            /* Got no path.  The D: drive should always be mapped to something, so
             * we're in trouble if we get here.  Fall back on the DVD device
             * path */
            ASSERT(FALSE);
            strcpy(szDDrivePath, g_szDVDDevicePrefix);
        }

        return XWriteTitleInfoAndReboot(&(lpTitlePath[3]),
                                        szDDrivePath,
                                        (NULL != pLaunchData) ? LDT_TITLE : LDT_NONE,
                                        XeImageHeader()->Certificate->TitleID,
                                        pLaunchData);
    }
#else
	return(ERROR_NOT_SUPPORTED);
#endif        
}


//
// Autoupdate API
//
HRESULT CXo::XOnlineTitleUpdate(
			DWORD				dwContext
			)
{
	HRESULT	hr = S_OK;
	DWORD	dwError;
	DWORD	cbTitleKey;

	XoEnter("XOnlineTitleUpdate");

#ifdef XONLINE_FEATURE_XBOX

	LAUNCH_UPDATE_INFO	lui;

	// Set up the launch info
	lui.dwReason = 0;
	lui.dwContext = dwContext;
	lui.dwParameter1 = 0;
	lui.dwParameter2 = 0;
	lui.dwTitleId = m_dwTitleId;
	lui.dwBaseVersion = m_dwTitleVersion;
	lui.dwUpdateFlags = LAUNCH_UPDATE_FLAG_ONLINE;

	cbTitleKey = XONLINE_KEY_LENGTH;
	hr = XoUpdateGetRawTitleKey(m_dwTitleId, lui.rgbTitleKey, &cbTitleKey);
	if (FAILED(hr))
		goto Error;

	// Reboot; this shouldn't come back
	dwError = RebootToUpdaterWhilePreservingDDrive(
				(m_dwTitleId == AUTOUPD_DASH_TITLE_ID)?
					g_szDashUpdater:g_szTitleUpdater, &lui);
	hr = HRESULT_FROM_WIN32(dwError);

#else
	hr = E_NOTIMPL;
	goto Error;
#endif

Exit:	
	return(XoLeave(hr));

Error:
	goto Exit;
}

HRESULT CXo::XOnlineTitleUpdateFromDVD(
			DWORD				dwContext,
			PSTR				szUpdatePath,
			DWORD				dwTitleId,
			PBYTE				pbSymmetricKey,
			DWORD				cbSymmetricKey,
			PBYTE				pbPublicKey,
			DWORD				cbPublicKey
			)
{
	HRESULT	hr = S_OK;
	DWORD	dwError;
	DWORD	cbTitleKey;

	XoEnter("XOnlineTitleUpdateFromDVD");
	XoCheck(szUpdatePath != NULL);
	XoCheck(pbSymmetricKey != NULL);
	XoCheck(cbSymmetricKey == XONLINE_KEY_LENGTH);
	XoCheck(pbPublicKey != NULL);
	XoCheck(cbPublicKey >= XONLINECONTENT_PK_SIGNATURE_SIZE);

#ifdef XONLINE_FEATURE_XBOX

	LAUNCH_UPDATE_INFO	lui;

	XoCheck(cbPublicKey <= sizeof(lui.rgbPublicKey));

	// Set up the launch info
	lui.dwReason = 0;
	lui.dwContext = dwContext;
	lui.dwParameter1 = 0;
	lui.dwParameter2 = 0;
	lui.dwTitleId = dwTitleId;
	lui.dwBaseVersion = m_dwTitleVersion;
	lui.dwUpdateFlags = 0;
	lui.cbPublicKey = cbPublicKey;

	cbTitleKey = XONLINE_KEY_LENGTH;
	hr = XoUpdateGetRawTitleKey(m_dwTitleId, lui.rgbTitleKey, &cbTitleKey);
	if (FAILED(hr))
		goto Error;

	memcpy(lui.rgbSymmetricKey, pbSymmetricKey, XONLINE_KEY_LENGTH);
	memcpy(lui.rgbPublicKey, pbPublicKey, cbPublicKey);

	strncpy(lui.szUpdatePath, szUpdatePath, MAX_PATH);

	// Reboot; this shouldn't come back
	// Always use the updater in the DVD because it is newer
	dwError = RebootToUpdaterWhilePreservingDDrive(g_szTitleUpdater, &lui);
	hr = HRESULT_FROM_WIN32(dwError);

#else
	hr = E_NOTIMPL;
	goto Error;
#endif

Exit:	
	return(XoLeave(hr));

Error:
	goto Exit;
}

HRESULT CXo::XOnlineTitleUpdateGetProgress(
			XONLINETASK_HANDLE	hTask,
			DWORD				*pdwPercentDone,
			ULONGLONG			*pqwNumerator,
			ULONGLONG			*pqwDenominator
			)
{
	HRESULT	hr = S_OK;

	XoEnter("XOnlineTitleUpdateGetProgress");
	XoCheck(hTask != NULL);
	
#ifdef XONLINE_FEATURE_XBOX

	hr = AutoupdateGetProgress(hTask, pdwPercentDone,
				pqwNumerator, pqwDenominator);
	if (FAILED(hr))
		goto Error;

#else
	hr = E_NOTIMPL;
	goto Error;
#endif

Exit:	
	return(XoLeave(hr));

Error:
	goto Exit;
}

//
// Intenral Autoupdate API
//
HRESULT CXo::XOnlineTitleUpdateInternal(
			DWORD				dwTitleId,
			DWORD				dwTitleOldVersion,
			PBYTE				pbTitleKey,
			HANDLE				hWorkEvent,
			PXONLINETASK_HANDLE	phTask
			)
{
	HRESULT	hr = S_OK;

	XoEnter("XOnlineTitleUpdateInternal");
	XoCheck(phTask != NULL);

#ifdef XONLINE_FEATURE_XBOX

	hr = AutoupdateOnline(dwTitleId, dwTitleOldVersion, 
				pbTitleKey, hWorkEvent, phTask);
	if (FAILED(hr))
		goto Error;

#else
	hr = E_NOTIMPL;
	goto Error;
#endif

Exit:	
	return(XoLeave(hr));

Error:
	goto Exit;
}

HRESULT CXo::XOnlineTitleUpdateFromDVDInternal(
			PSTR				szUpdatePath,
			DWORD				dwTitleId,
			DWORD				dwTitleOldVersion,
			PBYTE				pbTitleKey,
			PBYTE				pbSymmetricKey,
			DWORD				cbSymmetricKey,
			PBYTE				pbPublicKey,
			DWORD				cbPublicKey,
			HANDLE				hWorkEvent,
			PXONLINETASK_HANDLE	phTask
			)
{
	HRESULT	hr = S_OK;

	XoEnter("XOnlineTitleUpdateFromDVDInternal");
	XoCheck(phTask != NULL);

#ifdef XONLINE_FEATURE_XBOX

	hr = AutoupdateFromDVD(szUpdatePath,
				dwTitleId, dwTitleOldVersion, pbTitleKey, 
				pbSymmetricKey, cbSymmetricKey,
				pbPublicKey, cbPublicKey,
				hWorkEvent, phTask);
	if (FAILED(hr))
		goto Error;

#else
	hr = E_NOTIMPL;
	goto Error;
#endif

Exit:	
	return(XoLeave(hr));

Error:
	goto Exit;
}



