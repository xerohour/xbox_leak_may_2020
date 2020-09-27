/*++

Copyright (c) Microsoft Corporation

Author:
    Keith Lau (keithlau)

Description:
	Module implementing content verification

Module Name:

    contver.c

--*/

#include "xonp.h"
#include "xonver.h"
#include <shahmac.h>

//#define CHK_CORRUPT
#ifdef CHK_CORRUPT

#define CHK_CORRUPT_SIZE		65536

VOID CXo::CheckCorruption(
			PXONLINETASK_CONTENT_VERIFY	pcontver
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

const char * const CXo::s_rgszVerifyPhases[verphaseDone] =
{
	"verphaseReadDrm",
	"verphaseWriteDrm"
};

#endif

#ifdef XONLINE_FEATURE_XBOX

//
// Global function to check if a manifest matches the
// supplied title and version info. This is also needed 
// by the global version of DigestVerifySynchronous
//
HRESULT DigestVerifyHeader(
			XONLINEDIR_TYPE			dirtype,
			PDGSTFILE_IDENTIFIER	pdfiDigest,
			PDGSTFILE_HEADER		pdfh,
			DWORD					dwFlags
			)
{
	HRESULT					hr = S_OK;
	BOOL					fFailed = FALSE;
	PDGSTFILE_IDENTIFIER	pdfi;

	Assert(pdfiDigest);
	Assert(pdfh);
	
	pdfi = &(pdfh->dfi);

	// Here we do different checks depending on what
	// type of operations we are doing
	switch (dirtype)
	{
	case dirtypeTitleContent:
	case dirtypeTitleContentOnF:
	
		// Skip this if content is shareable
		if ((pdfh->wFlags & DGSTFILE_HEADER_FLAG_SHARED) == 0)
		{
			// Everything's gotta match
			if (memcmp(pdfi, pdfiDigest, 
						sizeof(DGSTFILE_IDENTIFIER)) != 0)
				fFailed = TRUE;
		}
		break;

	case dirtypeTitleUpdate:
	case dirtypeTitleUpdateOnF:
	case dirtypeDashUpdate0:
	case dirtypeDashUpdate1:
	case dirtypeTempDiff:

		// Make sure the digest is for the same title
		fFailed = TRUE;
		if (pdfi->dwTitleId == pdfiDigest->dwTitleId)
		{
			DWORD	dwVerCurrent, dwVerNew;
			
			// Then do a version check - the digest must be a newer version
			// than the caller's current version
			dwVerNew = MAKE_DWORD_VERSION(
								pdfi->wTitleVersionHi, 
								pdfi->wTitleVersionLo);
			dwVerCurrent = MAKE_DWORD_VERSION(
								pdfiDigest->wTitleVersionHi, 
								pdfiDigest->wTitleVersionLo);
			if (dwFlags & XONLINECONTENT_VERIFY_FLAG_ALLOW_SAME_VERSION)
			{
				// We allow the same version, this should be only used
				// to verify and sign a new update
				if (dwVerNew >= dwVerCurrent)
					fFailed = FALSE;
			}
			else
			{
				// By default, the version must be newer than the 
				// current version
				if (dwVerNew > dwVerCurrent)
					fFailed = FALSE;
			}
		}
		break;
		
	default:
		AssertSz(FALSE, "DigestVerifyHeader: bad dirtype");
		fFailed = TRUE;
	}

	if (fFailed)
	{
		// This is not the right digest
		hr = HRESULT_FROM_WIN32(ERROR_INVALID_DATA);
		goto Error;
	}

	// Now we need to make sure the parental control rating
	// on this box allows the content to be installed/run
	if (XGetParentalControlSetting() < pdfh->wRating)
	{
		// Parental control rating of this content exceeds the
		// setting on the box - cannot install/run content
		hr = HRESULT_FROM_WIN32(ERROR_ACCESS_DENIED);
		goto Error;
	}

Exit:
	return(hr);

Error:
	goto Exit;
}

//
// Refactored global function to verify content digests. This is
// for code that exists outside of CXo
//
HRESULT DigestVerifySynchronous(
			PSTR					szManifestFilePath,
			XONLINEDIR_TYPE			dirtype,
			PDGSTFILE_IDENTIFIER	pdfi,
			DWORD					dwFlags,
			PBYTE					pbAlternateKey
			)
{
	HRESULT				hr = S_OK;
	HANDLE				hFile = INVALID_HANDLE_VALUE;
	PBYTE				pbBuffer = NULL;
	DWORD				cbSize;
	LARGE_INTEGER		liSize;
	PDGSTFILE_HEADER	pdfh;
    BYTE				shactx[XC_SERVICE_SHA_CONTEXT_SIZE];
    BYTE				rgbDigest[XC_DIGEST_LEN];

	Assert(szManifestFilePath);

	// Open the digest file
	hFile = CreateFile(szManifestFilePath, 
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

	if (!GetFileSizeEx(hFile, &liSize))
	{
		hr = HRESULT_FROM_WIN32(GetLastError());
		goto Error;
	}

	if (liSize.QuadPart > XONLINECONTENT_MANIFEST_MAX_FILE_SIZE)
	{
		hr = HRESULT_FROM_WIN32(ERROR_INVALID_DATA);
		goto Error;
	}
	
	// Allocate the buffer
	cbSize = liSize.u.LowPart;
	pbBuffer = (PBYTE)LocalAlloc(LMEM_FIXED, cbSize);
	if (!pbBuffer)
	{
		hr = E_OUTOFMEMORY;
		goto Error;
	}

	// Read in the file
	if (!ReadFile(hFile, pbBuffer, cbSize, &cbSize, NULL))
	{
		hr = HRESULT_FROM_WIN32(GetLastError());
		goto Error;
	}

	// Now make sure the digest file identifier matches up
    if (cbSize < sizeof(DGSTFILE_HEADER))
    {
    	hr = HRESULT_FROM_WIN32(ERROR_INVALID_DATA);
    	goto Error;
    }

	// Point to the header
	pdfh = (PDGSTFILE_HEADER)(pbBuffer);

	// Run the digest
	XShaHmacInitialize(pbAlternateKey?pbAlternateKey:(BYTE *)(*XboxHDKey),
				XBOX_KEY_LENGTH, shactx);
	XShaHmacUpdate(shactx, 
				pbBuffer + XC_DIGEST_LEN, cbSize - XC_DIGEST_LEN);
	XShaHmacComputeFinal(shactx, 
				pbAlternateKey?pbAlternateKey:(BYTE *)(*XboxHDKey), 
				XBOX_KEY_LENGTH, rgbDigest);
	if (memcmp(rgbDigest, pdfh->rgbSignature, XC_DIGEST_LEN) != 0)
    {
    	hr = HRESULT_FROM_WIN32(ERROR_INVALID_DATA);
    	goto Error;
    }

	// Verify info
	hr = DigestVerifyHeader(dirtype, pdfi, pdfh, dwFlags);
	if (FAILED(hr))
		goto Error;

Exit:
	if (hFile != INVALID_HANDLE_VALUE)
		CloseHandle(hFile);

	if (pbBuffer)
		LocalFree(pbBuffer);
	return(hr);

Error:
	goto Exit;
}

#else

HRESULT DigestVerifyHeader(
			XONLINEDIR_TYPE			dirtype,
			PDGSTFILE_IDENTIFIER	pdfiDigest,
			PDGSTFILE_HEADER		pdfh,
			DWORD					dwFlags
			)
{
	return(E_NOTIMPL);
}

#endif

//
// Function to read DRM
//
HRESULT CXo::contverReadDrm(
			PXONLINETASK_CONTENT_VERIFY	pcontver
			)
{
	HRESULT				hr = S_OK;
	PXONLINETASK_FILEIO	pfileio = &pcontver->fileio;
	XONLINETASK_HANDLE	hfileio = (XONLINETASK_HANDLE)pfileio;
	ULARGE_INTEGER		uliOffset;
	DWORD				dwRead;
	PBYTE				pbRead;
	BYTE				rgbDigestPerBox[XC_DIGEST_LEN];
	BYTE				rgbDigestPackage[XC_DIGEST_LEN];
	PDGSTFILE_HEADER		pdfh;

	// Wait for the subtask to complete
	hr = XOnlineTaskContinue(hfileio);
	if (XONLINETASK_STATUS_AVAILABLE(hr))
	{
		// Returned results, see if it succeeded
		if (FAILED(hr))
			goto Exit;

		// Succeeded, now get the results and analyze the data
		hr = GetReadWriteFileResults(hfileio, &dwRead, &pbRead);
		if (FAILED(hr))
			goto Exit;

		pcontver->dwBytesRead += dwRead;

		// First, we need to skip the first XC_DIGEST_LEN bytes
		// because this is the digest slot
		if (!pcontver->pbHeader)
		{
			// Now make sure the digest file identifier matches up
            if (dwRead < sizeof(DGSTFILE_HEADER))
            {
            	hr = HRESULT_FROM_WIN32(ERROR_INVALID_DATA);
            	goto Exit;
            }
			pdfh = (PDGSTFILE_HEADER)pbRead;
			
			dwRead -= XC_DIGEST_LEN;
			pbRead += XC_DIGEST_LEN;
			pcontver->pbHeader = pcontver->pbBuffer;

			// Advance the read buffer if necessary
			if (pcontver->dwFileSize > XBOX_HD_SECTOR_SIZE)
			{
				pcontver->pbBuffer += XBOX_HD_SECTOR_SIZE;
				pcontver->cbBuffer -= XBOX_HD_SECTOR_SIZE;
			}

			// Check the digest header information
			hr = DigestVerifyHeader(pcontver->dirtype, 
						&(pcontver->dfi), pdfh, pcontver->dwFlags);
			if (FAILED(hr))
				goto Exit;
		}

		// Update the SHA HMAC
		XShaHmacUpdate(pcontver->shactxPerBox, pbRead, dwRead);
		if (pcontver->pbKeyPackage)
		{
			XShaHmacUpdate(pcontver->shactxPackage, pbRead, dwRead);
		}

		// Now, see if we need to read more 
		if (pcontver->dwFileSize > pcontver->dwBytesRead)
		{
			// Still need to read more
			uliOffset.QuadPart = pcontver->dwBytesRead;
			ReadWriteFileInitializeContext(pfileio->hFile,
						pcontver->pbBuffer, pcontver->cbBuffer, uliOffset, 
						pcontver->xontask.hEventWorkAvailable, 
						pfileio);

			hr = ReadFileInternal(pfileio);
			goto Exit;
		}
		else if (pcontver->dwFileSize == pcontver->dwBytesRead)
		{
			// Done reading, now calculate the final SHA HMAC value
			XShaHmacComputeFinal(pcontver->shactxPerBox,
						pcontver->pbKeyPerBox, XBOX_KEY_LENGTH,
						rgbDigestPerBox);

			// Now, if the digest matches the per-box digest, then 
			// this is a valid manifest
			if (memcmp(rgbDigestPerBox, pcontver->pbHeader, XC_DIGEST_LEN) == 0)
			{
				// Done
				hr = S_OK;
				pcontver->verphase = verphaseDone;
				goto Exit;
			}

			// If we are not in signing mode, this is a bad manifest
			if (pcontver->pbKeyPackage &&
				((pcontver->dwFlags & 
					(XONLINECONTENT_VERIFY_FLAG_SIGN | 
					 XONLINECONTENT_VERIFY_FLAG_USER_KEY)) != 0))
			{
				XShaHmacComputeFinal(pcontver->shactxPackage,
							pcontver->pbKeyPackage, XC_SYMMETRIC_KEY_SIZE,
							rgbDigestPackage);

				// Now, if the digest matches the package digest, then 
				// we will need to re-sign this manifest with the per-box key
				if (memcmp(rgbDigestPackage, pcontver->pbHeader, XC_DIGEST_LEN) == 0)
				{
					// OK, if we only wanted to verify, then we are done
					if (pcontver->dwFlags & XONLINECONTENT_VERIFY_FLAG_USER_KEY)
					{
						hr = S_OK;
						pcontver->verphase = verphaseDone;
						goto Exit;
					}
					
					// Replace the header digest with the per-box digest
					memcpy(pcontver->pbHeader, rgbDigestPerBox, XC_DIGEST_LEN);

					// Write it back out
					uliOffset.QuadPart = 0;
					ReadWriteFileInitializeContext(pfileio->hFile,
								pcontver->pbHeader, XBOX_HD_SECTOR_SIZE, 
								uliOffset, 
								pcontver->xontask.hEventWorkAvailable, 
								pfileio);

					hr = WriteFileInternal(pfileio);
					if (FAILED(hr))
						goto Exit;
				
					// Next phase is to wait for the write to complete
					pcontver->verphase = verphaseWriteDrm;
					goto Exit;
				}
			}

			// If we get here, the check has failed; fall thru ...
		}
		
		// Bad state, fail on the safe side
		hr = HRESULT_FROM_WIN32(ERROR_INVALID_DATA);
		goto Exit;
	}

Exit:
	return(hr);
}

//
// Function to wait for write DRM to complete
//
HRESULT CXo::contverWriteDrm(
			PXONLINETASK_CONTENT_VERIFY	pcontver
			)
{
	HRESULT				hr = S_OK;
	XONLINETASK_HANDLE	hfileio = (XONLINETASK_HANDLE)&(pcontver->fileio);
	DWORD				dwWritten;

	// Wait for the subtask to complete
 	hr = XOnlineTaskContinue(hfileio);
	if (XONLINETASK_STATUS_AVAILABLE(hr))
	{
		// Returned results, see if it succeeded
		if (FAILED(hr))
			goto Error;

		// Succeeded, now get the results and analyze the data
		hr = GetReadWriteFileResults(hfileio, &dwWritten, NULL);
		if (FAILED(hr))
			goto Error;

		Assert(dwWritten == XBOX_HD_SECTOR_SIZE);

		// Well, if the file size is less than one sector, chop it off
		if (pcontver->dwFileSize < XBOX_HD_SECTOR_SIZE)
		{
			hr = SetEndOfFileNt(pcontver->fileio.hFile, pcontver->dwFileSize);
			if (FAILED(hr))
				goto Error;
		}
			
		// We're done.
		pcontver->verphase = verphaseDone;
	}
	
Error:
	return(hr);
}

//
// Function to cleanup
//
HRESULT CXo::contverCleanup(
			PXONLINETASK_CONTENT_VERIFY	pcontver
			)
{
	if (pcontver->fileio.hFile != INVALID_HANDLE_VALUE)
	{
		CloseHandle(pcontver->fileio.hFile);
		pcontver->fileio.hFile = INVALID_HANDLE_VALUE;
	}
	return(S_OK);
}

//
// Implement the do work function for XOnlineContentVerify
//
HRESULT CXo::contverContinue(
			XONLINETASK_HANDLE	hTask
			)
{
	HRESULT						hr = S_OK;
	PXONLINETASK_CONTENT_VERIFY	pcontver = NULL;
	PXONLINETASK_FILEIO			pfileio = NULL;

#if DBG
	LARGE_INTEGER				liTimerTotal;
	LARGE_INTEGER				liTimerEnd;
	LARGE_INTEGER				liTimerStart;
	XONLINECONTENT_VERPHASE		verphase;

	// Track service count
	m_liverServiceCount.QuadPart += 1;

	// Start the total timer
	QueryPerformanceCounter(&liTimerTotal);
#endif

	Assert(hTask != NULL);

	// hTask points to the xontask field of XONLINETASK_CONTENT_VERIFY
	// find the containing pcontver structure
	pcontver = CONTAINING_RECORD(hTask, XONLINETASK_CONTENT_VERIFY, xontask);
	pfileio = &pcontver->fileio;

#ifdef CHK_CORRUPT
	CheckCorruption(pcontver);
#endif

#if DBG
	// Start the loop timer
	QueryPerformanceCounter(&liTimerStart);
	verphase = pcontver->verphase;
#endif

	// Figure out what phase we are in ...
	switch (pcontver->verphase)
	{
	case verphaseReadDrm:
		hr = contverReadDrm(pcontver);
		break;
		
	case verphaseWriteDrm:
		hr = contverWriteDrm(pcontver);
		break;
		
	default:
		// This is a bad state. We force it to end so we don't get stuck
		// here forever.
		AssertSz(FALSE, "contverContinue: invalid phase!");
		hr = E_FAIL;
	}

#if DBG	
	// Stop timer
	QueryPerformanceCounter(&liTimerEnd);

	m_liverTotalTime.QuadPart += (liTimerEnd.QuadPart - liTimerStart.QuadPart);
	m_liverAverageTime.QuadPart = m_liverTotalTime.QuadPart / m_liverServiceCount.QuadPart;

//	TraceSz2(Verbose, "%s, %I64u us", s_rgszVerifyPhases[verphase], (liTimerEnd.QuadPart - liTimerStart.QuadPart));
	
#endif	

	// Bail if failed.
	if (FAILED(hr))
	{
		contverCleanup(pcontver);
		pcontver->verphase = verphaseDone;
	}

	// When the next state is the final state, we set the flag so we don't
	// continue.
	if (pcontver->verphase == verphaseDone)
	{
		// Indicate that we are done and return the final code
		XONLINETASK_SET_COMPLETION_STATUS(hr);

#if DBG
		TraceSz1(Verbose, "%I64u services elapsed", m_liverServiceCount.QuadPart);
		TraceSz1(Verbose, "Total time: %I64u ticks", m_liverTotalTime.QuadPart);
		TraceSz1(Verbose, "Average service time: %I64u ticks", m_liverAverageTime.QuadPart);
		m_liverServiceCount.QuadPart = 0;
		m_liverTotalTime.QuadPart = 0;
		m_liverAverageTime.QuadPart = 0;
#endif		
	}

#ifdef CHK_CORRUPT
	CheckCorruption(pcontver);
#endif

	return(hr);
}				

//
// Function to close the task handle
//
VOID CXo::contverClose(
			XONLINETASK_HANDLE	hTask
			)
{
	PXONLINETASK_CONTENT_VERIFY	pcontver = (PXONLINETASK_CONTENT_VERIFY)hTask;

	contverCleanup(pcontver);
	SysFree(pcontver);
}

//
// Function to initialize the context for content verification
//
HRESULT CXo::ContentVerifyInitializeContext(
			HANDLE						hFile,
			DWORD						dwFileSize,
			PBYTE						pbBuffer,
			DWORD						cbBuffer,
			PBYTE						pbKeyPackage,
			DWORD						dwFlags,
			PDGSTFILE_IDENTIFIER		pdfi,
			XONLINEDIR_TYPE				dirtype,
			HANDLE						hWorkEvent,
			PXONLINETASK_CONTENT_VERIFY	pcontver
			)
{
	HRESULT	hr = S_OK;

    Assert(hFile != INVALID_HANDLE_VALUE);
    Assert(NULL != pbBuffer);
    Assert(cbBuffer >= (XBOX_HD_SECTOR_SIZE * 2));
    Assert(NULL != pcontver);

	// If the file is too small, fail immediately
    if (dwFileSize < XC_DIGEST_LEN)
    	return(HRESULT_FROM_WIN32(ERROR_INVALID_DATA));

	// Align buffer size with sector size
	cbBuffer &= (~(XBOX_HD_SECTOR_SIZE - 1));

	ZeroMemory(pcontver, sizeof(XONLINETASK_CONTENT_VERIFY));

	// Fill in the blanks
	TaskInitializeContext(&pcontver->xontask);
	pcontver->xontask.hEventWorkAvailable = hWorkEvent;
	pcontver->xontask.pfnContinue = contverContinue;
	pcontver->xontask.pfnClose = contverClose;
	pcontver->verphase = verphaseReadDrm;
	pcontver->fileio.hFile = hFile;
	pcontver->dwFileSize = dwFileSize;
	pcontver->dwBytesRead = 0;
	pcontver->pbHeader = NULL;
	pcontver->pbBuffer = pbBuffer;
	pcontver->cbBuffer = cbBuffer;
	pcontver->pbKeyPackage = pbKeyPackage;
	pcontver->dwFlags = dwFlags;
	pcontver->dirtype = dirtype;

	CopyMemory(&(pcontver->dfi), pdfi, sizeof(DGSTFILE_IDENTIFIER));

	// Get the real per-box key
	pcontver->pbKeyPerBox = GetHdKey();

	// Initialize our contexts for SHA HMAC calculations
	XShaHmacInitialize(
				pcontver->pbKeyPerBox,
				XBOX_KEY_LENGTH,
				pcontver->shactxPerBox);

	// Initialize for per-package if the key is supplied
	if (pbKeyPackage)
	{
		XShaHmacInitialize(
					pbKeyPackage,
					XC_SYMMETRIC_KEY_SIZE,
					pcontver->shactxPackage);
	}

	return(S_OK);
}

//
// Function to begin content verification
//
HRESULT CXo::ContentVerifyBegin(
			PXONLINETASK_CONTENT_VERIFY	pcontver
			)
{
	HRESULT						hr = S_OK;
	ULARGE_INTEGER				uliOffset;
	DWORD						dwReadSize;
	PXONLINETASK_FILEIO			pfileio = NULL;

    Assert(NULL != pcontver);
	pfileio = &pcontver->fileio;

	// OK, now kick off a file read. Take extra care to not overrun
	// the buffer
	dwReadSize = (pcontver->dwFileSize > pcontver->cbBuffer)?
					pcontver->cbBuffer:pcontver->dwFileSize;
	dwReadSize = SECTOR_ALIGNMENT_ROUND_UP(dwReadSize);
    Assert(dwReadSize <= pcontver->cbBuffer);
	
	uliOffset.QuadPart = 0;
	ReadWriteFileInitializeContext(pfileio->hFile,
				pcontver->pbBuffer, dwReadSize, uliOffset, 
				pcontver->xontask.hEventWorkAvailable, 
				pfileio);

	hr = ReadFileInternal(pfileio);
	if (FAILED(hr))
		goto Error;

	// Next phase is read Drm
	pcontver->verphase = verphaseReadDrm;

Error:
	return(hr);
}

//
// Function to kick off content verification. This is 
// the internal/Dash version because it allows a Title ID to be 
// specified.
//
HRESULT CXo::ContentVerifyInternal(
			PDGSTFILE_IDENTIFIER	pdfi,
			PBYTE					pbBuffer,
			DWORD					*pcbBuffer,
			DWORD					dwFlags,
			XONLINEDIR_TYPE			dirtype,
			HANDLE					hWorkEvent,
			XONLINETASK_HANDLE		*phTask
			)
{
	HRESULT						hr = S_OK;
	PXONLINETASK_CONTENT_VERIFY pcontver = NULL;
	PXONLINETASK_FILEIO			pfileio = NULL;
	ULARGE_INTEGER				uliOffset;
	HANDLE						hFile = INVALID_HANDLE_VALUE;
	BYTE						szPath[XONLINECONTENT_MAX_PATH];
	DWORD						cbSize;
	DWORD						cbActual;

	Assert(NULL != phTask);
	Assert(NULL != pcbBuffer);

    cbSize = XBOX_HD_SECTOR_SIZE * 2;
    if (*pcbBuffer && (*pcbBuffer < cbSize))
    {
    	// Buffer too small, tell caller what the minimum is
    	*pcbBuffer = cbSize;
    	return(HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER));
    }

	// Build the path to the DRM file, it must be in the
	// title data area
 	cbSize = XONLINECONTENT_MAX_PATH;
	hr = BuildExistingContentPath(
				pdfi->dwTitleId,
				pdfi->OfferingId, 
				XONLINECONTENT_MANIFEST_FILE_NAME,
				dirtypeTitleContent,
				&cbSize,
				(CHAR *)szPath);
	if (FAILED(hr))
	{
		*pcbBuffer = cbSize;
		goto Error;
	}

	// Open the manifest
	hFile = CreateFile(
				(LPSTR)szPath, 
				GENERIC_READ | GENERIC_WRITE, 
				0, 	// No sharing to prevent mishaps
				NULL, 
				OPEN_EXISTING, 
				FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN |
				FILE_FLAG_OVERLAPPED | FILE_FLAG_NO_BUFFERING, 
				NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		// If the manifest is not found then the installation 
		// is invalid
		hr = HRESULT_FROM_WIN32(GetLastError());
		goto Error;
	}

	if (!GetFileSizeEx(hFile, (PLARGE_INTEGER)&uliOffset))
	{
		hr = HRESULT_FROM_WIN32(GetLastError());
		goto Error;
	}

	// Make sure the file size makes sense
	if ((uliOffset.QuadPart <= XC_DIGEST_LEN) ||
		(uliOffset.QuadPart > XONLINECONTENT_MANIFEST_MAX_FILE_SIZE))
	{
		hr = HRESULT_FROM_WIN32(ERROR_INVALID_DATA);
		goto Error;
	}

	// Return the manifest size to the caller in case this is
	// interesting information
	*pcbBuffer = uliOffset.u.LowPart;

    // Don't allocate for buffer if caller already specified one
    if (pbBuffer)
    {
    	cbSize = 0;
    	cbActual = SECTOR_ALIGNMENT_ROUND_DOWN(*pcbBuffer);
    }
    else if (*pcbBuffer > 0)
    {
    	cbSize = SECTOR_ALIGNMENT_ROUND_DOWN(*pcbBuffer);
    	if (cbSize < (XBOX_HD_SECTOR_SIZE * 2))
    		cbSize = (XBOX_HD_SECTOR_SIZE * 2);
    	cbActual = cbSize;
    }
    else
    {
    	// Zero *pcbBuffer is a special case to say "allocate a buffer
    	// that holds the whole file"
    	cbSize = SECTOR_ALIGNMENT_ROUND_UP(uliOffset.u.LowPart);
    	*pcbBuffer = cbActual = cbSize;
    }

	// We are responsible for allocating the context
	cbSize += sizeof(XONLINETASK_CONTENT_VERIFY);
	
#ifdef CHK_CORRUPT	
	cbSize += CHK_CORRUPT_SIZE;
#endif

	pcontver = (PXONLINETASK_CONTENT_VERIFY)SysAlloc(cbSize, PTAG_XONLINETASK_CONTENT_VERIFY);
	if (!pcontver)
	{
		hr = E_OUTOFMEMORY;
		goto Error;
	}

	if (!pbBuffer)
		pbBuffer = (PBYTE)(pcontver + 1);

	// Now transfer the handle over
	pfileio = &pcontver->fileio;
	pfileio->hFile = hFile;
	hFile = INVALID_HANDLE_VALUE;

	// Initialize the context
	hr = ContentVerifyInitializeContext(
				pfileio->hFile, uliOffset.u.LowPart, 
				pbBuffer, cbActual, 
				NULL, dwFlags, pdfi, dirtype,
				hWorkEvent, pcontver);
	if (FAILED(hr))
		goto Error;

	// Kick off the verification
	hr = ContentVerifyBegin(pcontver);
	if (FAILED(hr))
		goto Error;

	// Return the context as an opaque handle
	*phTask = (XONLINETASK_HANDLE)pcontver;

Cleanup:
	return(hr);

Error:

	// Failed setup, make sure we clean up everything
	if (pcontver)
	{
		contverCleanup(pcontver);
		SysFree(pcontver);
	}
	if (hFile != INVALID_HANDLE_VALUE)
		CloseHandle(hFile);
	goto Cleanup;
}

//
// Function to kick off a content verification
//
HRESULT CXo::XOnlineContentVerify(
			XONLINEOFFERING_ID		OfferingId,
			PBYTE					pbBuffer,
			DWORD					*pcbBuffer,
			HANDLE					hWorkEvent,
			XONLINETASK_HANDLE		*phTask
			)
{
    XoEnter("XOnlineContentVerify");

	DGSTFILE_IDENTIFIER	dfi;

	dfi.dwTitleId = m_dwTitleId;
	dfi.OfferingId = OfferingId;
	dfi.wTitleVersionHi = 0;
	dfi.wTitleVersionLo = 0;

	return(XoLeave(ContentVerifyInternal(&dfi, pbBuffer, pcbBuffer, 0, dirtypeTitleContent, hWorkEvent, phTask)));
}

//
// Function to kick off a digest verification task
//
HRESULT CXo::DigestVerify(
			PSTR						szManifestFileName,
			XONLINEDIR_TYPE				dirtype,
			PBYTE						pbBuffer,
			DWORD						*pcbBuffer,
			PBYTE						pbAlternateKey,
			HANDLE						hWorkEvent,
			PXONLINETASK_CONTENT_VERIFY	pcontver
			)
{
	HRESULT	hr = S_OK;
	HANDLE	hFile = INVALID_HANDLE_VALUE;
	CHAR	szPath[MAX_PATH];
	DWORD	cbPath = MAX_PATH;
	DWORD	dwFlags = pbAlternateKey?XONLINECONTENT_VERIFY_FLAG_USER_KEY:0;
	LARGE_INTEGER		liSize;
	DGSTFILE_IDENTIFIER	dfi;

	Assert(szManifestFileName);
	Assert(pbBuffer);
	Assert(pcbBuffer);
	Assert(*pcbBuffer > sizeof(DGSTFILE_HEADER));
	Assert(pcontver);

	// Compute where the digest file is
	hr = BuildTargetPath(0, 0, 0, szManifestFileName,
				dirtype, &cbPath, szPath);
	if (FAILED(hr))
		goto Error;

	// Open the digest file
	hFile = CreateFile(szPath, 
				GENERIC_READ, 
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

	// The digest file should be smaller than our buffer
	cbPath = *pcbBuffer;
	*pcbBuffer = liSize.u.LowPart;
	if (liSize.QuadPart > cbPath)
	{
		hr = HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);
		goto Error;
	}

	// Simply kick off a content verify
	dfi.OfferingId = 0;
	dfi.dwTitleId = m_dwTitleId;
	MAKE_COMPOSITE_VERSION(m_dwTitleVersion, &dfi.wTitleVersionHi, &dfi.wTitleVersionLo);
	
	hr = ContentVerifyInitializeContext(
				hFile, liSize.u.LowPart,
				pbBuffer, cbPath,
				pbAlternateKey, dwFlags,
				&dfi, dirtype, hWorkEvent, pcontver);
	if (FAILED(hr))
		goto Error;

	hr = ContentVerifyBegin(pcontver);
	if (FAILED(hr))
		goto Error;

Exit:
	return(hr);

Error:
	if (hFile != INVALID_HANDLE_VALUE)
		CloseHandle(hFile);
	goto Exit;
}

//
// ====================================================
// Functions to access SHA1 signatures in digest files
//
// Note: in order to use these functions, the caller must load the 
// digest file in its entirety into memory. The preferred method
// to do this is by figuring out the digest file size and calling 
// ContentVerifyInitializeContext and supplying a buffer large 
// enough to fit the entire file. This is because ContentVerify
// actually checks the signature of the digest file to make sure
// the digest file is authentic.
//

//
// API to begin a digest file enumeration
//
HRESULT EnumerateDigestInitializeContext(
			PBYTE					pbDigestData,
			DWORD					cbDigestData,
			PDGSTFILE_ENUM_CONTEXT	pContext
			)
{
	DWORD	cEntries;
	DWORD	cbSize = sizeof(DGSTFILE_HEADER);
	
	Assert(cbDigestData > cbSize);
	Assert(pbDigestData);
	Assert(pContext);

	pContext->dwCurrent = 0;
	pContext->cbDigestData = cbDigestData;
	pContext->pbDigestData = pbDigestData;

	// Overlay the header
	pContext->pHeader = (PDGSTFILE_HEADER)pbDigestData;
	cbDigestData -= cbSize;
	pbDigestData += cbSize;

	// Find the offset vector
	cEntries = pContext->pHeader->cDigestEntries;
	Assert(cEntries > 0);
	cbSize = cEntries * sizeof(DWORD);
	Assert(cbSize < cbDigestData);
	pContext->rgdwOffsetVector = (DWORD *)pbDigestData;
	cbDigestData -= cbSize;
	pbDigestData += cbSize;

	return(S_OK);
}

//
// API to get entries in a digest file (returns a reference to the
// data instead of a copy)
//
HRESULT EnumerateDigestNext(
			PDGSTFILE_ENUM_CONTEXT	pContext,
			PSTR					*ppszFileName,
			PDGSTFILE_FILE_DATA		*ppData
			)
{
	DWORD	dwBaseOffset;

	Assert(pContext);
	Assert(ppszFileName);
	Assert(ppData);

	if (pContext->dwCurrent >= pContext->pHeader->cDigestEntries)
		return(HRESULT_FROM_WIN32(ERROR_NO_MORE_ITEMS));

	// The fixed structure comes first
	dwBaseOffset = pContext->rgdwOffsetVector[pContext->dwCurrent];
	*ppData = (PDGSTFILE_FILE_DATA)(pContext->pbDigestData + dwBaseOffset);
	dwBaseOffset += sizeof(DGSTFILE_FILE_DATA);

	// Figure out where the string is ...
	dwBaseOffset += (((*ppData)->wSections - 1) * sizeof(DGST_SECTION_DATA));
	dwBaseOffset += (*ppData)->bReservedSize;
	*ppszFileName = (PSTR)(pContext->pbDigestData + dwBaseOffset);

	dwBaseOffset += (strlen(*ppszFileName) + 1);
	Assert(pContext->cbDigestData >= dwBaseOffset);
	(pContext->dwCurrent)++;
	return(S_OK);
}

//
// API to find a specific entry in a digest file (returns a reference to the
// data instead of a copy)
//
HRESULT EnumerateDigestFind(
			PDGSTFILE_ENUM_CONTEXT	pContext,
			PSTR					pszFileName,
			PDGSTFILE_FILE_DATA		*ppData
			)
{
	PDWORD	pdwOffset;
	DWORD	dwBaseOffset;
	DWORD	i;

	Assert(pContext);
	Assert(pszFileName);
	Assert(ppData);

	for (i = 0, pdwOffset = pContext->rgdwOffsetVector; 
		 i < pContext->pHeader->cDigestEntries; 
		 i++, pdwOffset++)
	{
		// The fixed structure comes first
		dwBaseOffset = *pdwOffset;
		*ppData = (PDGSTFILE_FILE_DATA)(pContext->pbDigestData + dwBaseOffset);
		dwBaseOffset += sizeof(DGSTFILE_FILE_DATA);

		// Figure out where the string is ...
		dwBaseOffset += (((*ppData)->wSections - 1) * sizeof(DGST_SECTION_DATA));
		dwBaseOffset += (*ppData)->bReservedSize;
		if (!_stricmp(pszFileName, (PSTR)(pContext->pbDigestData + dwBaseOffset)))
			return(S_OK);
			
		dwBaseOffset += (strlen(pszFileName) + 1);
		Assert(pContext->cbDigestData >= dwBaseOffset);
	}

	return(HRESULT_FROM_WIN32(ERROR_NOT_FOUND));
}

