/*++

Copyright (c) Microsoft Corporation

Author:
    Keith Lau (keithlau)

Description:
	Module implementing content download

Module Name:

    contdl.c

--*/

#include "xonp.h"
#include "xonver.h"

//#define NO_SIGNATURE

//#define CHK_CORRUPT
#ifdef CHK_CORRUPT

#define CHK_CORRUPT_SIZE		65536

VOID CXo::CheckCorruption(
			PXONLINECONTENT_INSTALL	pinstall
			)
{
	PDWORD	pdwGate = (PDWORD)(pinstall + 1);
	DWORD	dw;
	for (dw = 0; dw < (CHK_CORRUPT_SIZE >> 2); dw++, pdwGate++)
		if (*pdwGate)
		{
			AssertSz(FALSE, "CORRUPT");
		}
}

#endif

#if DBG

const char * const CXo::s_rgszPhases[dlphaseDone] =
{
	// Pre-Connect phases
	"dlphaseCatref",
	"dlphaseCheckCache",
	"dlphaseReadCache",
	"dlphaseWipeTarget",
	"dlphaseCreateCache",
	"dlphaseSetupCahce",
	"dlphaseVerifyInstall",
	"dlphasePreCustom",

	// Process data phases
	"dlphaseRecvHeader",
	"dlphaseInstallFiles",
	"dlphaseCreateTarget",
	"dlphaseWriteTarget",
	"dlphaseCheckpoint",
	"dlphaseCreateDirectory",
	"dlphaseVerifyCffiles",

	// Post-Download phases
	"dlphaseSignDrm",
	"dlphaseDeleteCache",
	"dlphasePostCustom",
};

#endif

//
// Define the name of the cache file
//
#define XONLINECONTENT_CACHE_FILE_NAME		"cache.xcd"

//
// Helper functions
//

//
// Function to begin a task to verify and/or sign the manifest
//
HRESULT CXo::VerifyOrSignManifestBegin(
			PXONLINECONTENT_INSTALL	pinstall,
			PBYTE					pbPackageKey
			)
{
	HRESULT				hr = S_OK;
	HANDLE				hFile = INVALID_HANDLE_VALUE;
	LARGE_INTEGER		liSize;
	DWORD				cbPath;
	CHAR				*szPath;
	DGSTFILE_IDENTIFIER	dfi;
	
	Assert(pinstall);

	// Open the manifest file
	szPath = (CHAR *)pinstall->rgbDecompress;
	cbPath = XONLINECONTENT_MAX_PATH;
	hr = BuildTargetPath(
				pinstall->dwTitleId, 
				pinstall->OfferingId,
				pinstall->dwBitFlags,
				XONLINECONTENT_MANIFEST_FILE_NAME,
				pinstall->dirtypeTarget,
				&cbPath,
				szPath);
	if (FAILED(hr))
		goto Error;

	hFile = CreateFile(
				szPath, 
				GENERIC_READ | GENERIC_WRITE, 
				FILE_SHARE_READ, 
				NULL, 
				OPEN_ALWAYS, 
				FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN |
				FILE_FLAG_OVERLAPPED | FILE_FLAG_NO_BUFFERING, 
				NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		// We've extracted everything and now we cannot open
		// the manifest file.
		hr = HRESULT_FROM_WIN32(GetLastError());
		goto Error;
	}

	if (!GetFileSizeEx(hFile, &liSize))
	{
		hr = HRESULT_FROM_WIN32(GetLastError());
		goto Error;
	}

	// File should be reasonably small
	Assert(liSize.u.HighPart == 0);

	// Reset the counters
	pinstall->dwExpected = liSize.u.LowPart;
	pinstall->dwWritten = 0;

	// Initialize the verification context
	dfi.dwTitleId = pinstall->dwTitleId;
	dfi.OfferingId = pinstall->OfferingId;
	dfi.wTitleVersionHi = 0;
	dfi.wTitleVersionLo = 0;
	hr = ContentVerifyInitializeContext(
				hFile, liSize.u.LowPart,
				pinstall->rgbDecompress, 
				XONLINECONTENT_LZX_OUTPUT_SIZE,
				pbPackageKey,
				pbPackageKey?XONLINECONTENT_VERIFY_FLAG_SIGN:0,
				&dfi,
				pinstall->dirtypeTarget,
				pinstall->xontask.hEventWorkAvailable,
				&pinstall->verify);
	if (FAILED(hr))
		goto Error;

	// Kick it off!
	hr = ContentVerifyBegin(&pinstall->verify);
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
// Function to sanity check the received CAB header
//
HRESULT CXo::VerifyCabHeader(
			PXONLINECONTENT_HEADER	pheader
			)
{
	CFHEADER	*pcfheader = &pheader->cfheader;
	
	// Make sure the signature on the decrypted header checks up
	if (pcfheader->sig != sigCFHEADER)
	{
		AssertSz(FALSE, "VerifyCabHeader: Invalid CAB signature");
		goto Error;
	}

	// Check version
	if (pcfheader->version != verCF)
	{
		AssertSz(FALSE, "VerifyCabHeader: Invalid CAB version");
		goto Error;
	}

	// Assert some important assumptions
	// 1: flags only contain RESERVE_PRESENT
	if ((pcfheader->flags & cfhdrFLAGS_ALL) != cfhdrRESERVE_PRESENT)
	{
        AssertSz(FALSE, "VerifyCabHeader: Invalid flags");
		goto Error;
	}

	// 2: Cabinet set only contains 1 cabinet
	if (pcfheader->iCabinet != 0)
	{
		AssertSz(FALSE, "VerifyCabHeader: iCabinet != 0");
		goto Error;
	}

	// 3: The reserved sizes are as expected
	if (pheader->cfreserve.cbCFHeader != XONLINECONTENT_HEADER_RESERVE_SIZE)
	{
        AssertSz(FALSE, "VerifyCabHeader: Invalid header reserve size");
		goto Error;
	}

	if (pheader->cfreserve.cbCFFolder != XONLINECONTENT_PERFOLDER_RESERVE_SIZE)
	{				 
        AssertSz(FALSE, "VerifyCabHeader: Invalid per-folder reserve size");
		goto Error;
	}

	return(S_OK);		
	
Error:	
	return(HRESULT_FROM_WIN32(ERROR_INVALID_DATA));
}

//
// Function to verify the PK signature of the header
//
HRESULT CXo::VerifyHeaderSignature(
			PXONLINECONTENT_INSTALL	pinstall,
			PXONLINECONTENT_HEADER	pheader,
			PBYTE					pbSignature
			)
{
    PBYTE		pbPublicKey = pinstall->rgbPublicKey;
    BYTE		rgbDigest[XC_DIGEST_LEN];
    BYTE        SHAHashContext[XC_SERVICE_SHA_CONTEXT_SIZE];
    
	// Run an SHA1 hash over the header to get the digest
    XcSHAInit(SHAHashContext);
    XcSHAUpdate(SHAHashContext, (PBYTE)pheader, XONLINECONTENT_FIXED_HEADER_SIZE_WITH_HMAC);
    XcSHAFinal(SHAHashContext, rgbDigest);

	// We will avoid using allocations here. We will use the LZX
	// history window as the workspace to verify the digest. This
	// window is at least 32k bytes so we should never overrun this
	if ((XcPKGetKeyLen(pbPublicKey) << 1) > XONLINECONTENT_LZX_WINDOW_SIZE)
	{
		AssertSz(FALSE, "VerifyHeaderSignature: public key length > 16k!");
		goto Error;
	}

	// Verify the digest against the encrypted signature, using the
	// LZX buffer as the workspace
	if (XcVerifyPKCS1Signature(pbSignature, pbPublicKey, rgbDigest) == FALSE)
		goto Error;

	return(S_OK);
	
Error:
	return(HRESULT_FROM_WIN32(ERROR_INVALID_DATA));
}			

//
// Function to process fixed header data
//
HRESULT CXo::ProcessFixedHeader(
			PXONLINETASK_CONTENT_DOWNLOAD	pcontdl,
			PBYTE							pbheader
			)
{
	HRESULT						hr = S_OK;
	PXONLINECONTENT_INSTALL		pinstall = NULL;
	PXONLINECONTENT_HEADER		pheader = NULL;
	PRC4_SHA_HEADER				phmac = NULL;

	Assert(pcontdl != NULL);

	pinstall = &pcontdl->install;

#ifndef NO_SIGNATURE

	// Verify the signed portion 
	hr = VerifyHeaderSignature(pinstall, (PXONLINECONTENT_HEADER)pbheader, 
				pbheader + 
				XONLINECONTENT_FIXED_HEADER_SIZE_WITH_HMAC_AND_CHECKPOINT);
	if (FAILED(hr))
		goto Error;

#endif

	// Copy the data into the final destination
	memcpy(&pinstall->header, pbheader, 
				XONLINECONTENT_FIXED_HEADER_SIZE);

	// Point to the destination
	pheader = &pinstall->header;
	phmac = (PRC4_SHA_HEADER)(pbheader + XONLINECONTENT_FIXED_HEADER_SIZE);

#ifndef NO_ENCRYPT
	
	// OK, we have enough to start, initialize RC4 decryption
	hr = SymmdecInitialize(
				pinstall->rgbSymmetricKey, 
				XONLINE_KEY_LENGTH,
				XONLINECONTENT_FIXED_HEADER_SIZE,
				phmac,
				&pinstall->symmdecData);
	if (FAILED(hr))
		goto Error;

	// Decrypt the fixed header portion
	hr = SymmdecDecrypt(&pinstall->symmdecData,
				(PBYTE)pheader, XONLINECONTENT_FIXED_HEADER_SIZE);
	if (FAILED(hr))
		goto Error;

	// Make sure the Hmac checks out correct
	hr = SymmdecVerify(&pinstall->symmdecData);
	if (FAILED(hr))
		goto Error;

#endif

	// Verify the header
	hr = VerifyCabHeader(pheader);
	if (FAILED(hr))
		goto Error;

	return(S_OK);

Error:
	return(hr);
}

//
// Function to process CFFOLDER entries
//
HRESULT CXo::ProcessCffolderEntries(
			PXONLINETASK_CONTENT_DOWNLOAD	pcontdl,
			PBYTE							pbDataBegin
			)
{
	HRESULT						hr = S_OK;
	PXONLINECONTENT_INSTALL		pinstall = NULL;
	PXONLINECONTENT_HEADER		pheader = NULL;
	DWORD						cbSize;

	Assert(pcontdl != NULL);

	pinstall = &pcontdl->install;
	pheader = &pinstall->header;

	// Make sure the correct size is reserved per-folder
    Assert(pheader->cfreserve.cbCFFolder == sizeof(RC4_SHA_HEADER));
	cbSize = pheader->cfheader.cFolders;
	cbSize *= sizeof(CFFOLDER_HMAC);

	// Make sure the folder count makes sense
	if (pheader->cfheader.cFolders > XONLINECONTENT_MAX_CFFOLDER)
	{
		AssertSz(FALSE, "ProcessCffolderEntries: invalid folder count");
		hr = HRESULT_FROM_WIN32(ERROR_INVALID_DATA);
		goto Error;
	}

	// Copy this data into its final destination
	memcpy(pinstall->rgcffolderhmac, pbDataBegin, cbSize);

#ifndef NO_ENCRYPT

	// Initialize RC4 decryption to decrypt CFFOLDER entries
	hr = SymmdecInitialize(
				pinstall->rgbSymmetricKey, 
				XONLINE_KEY_LENGTH,
				cbSize,
				&pheader->digestFolders,
				&pinstall->symmdecData);
	if (FAILED(hr))
		goto Error;

	// Decrypt the CFFOLDER entries
	hr = SymmdecDecrypt(&pinstall->symmdecData,
				(PBYTE)pinstall->rgcffolderhmac, cbSize);
	if (FAILED(hr))
		goto Error;

	// Make sure the Hmac checks out correct
	hr = SymmdecVerify(&pinstall->symmdecData);
	if (FAILED(hr))
		goto Error;

#endif

	// Now calculate the expected number of bytes of the whole
	// cached portion, including CFFILE entries. This is actually
	// just coffCabStart from the first CFFOLDER entry
	pinstall->dwExpected = pinstall->rgcffolderhmac[0].cffolder.coffCabStart;

	// Make sure the expected length is also more than the offset
	// to the first CFFILE entry
	if (pinstall->dwExpected < pinstall->header.cfheader.coffFiles)
	{
		AssertSz(FALSE, "ProcessCffolderEntries: invalid CFFILE offset");
		hr = HRESULT_FROM_WIN32(ERROR_INVALID_DATA);
		goto Error;
	}

	return(S_OK);

Error:
	return(hr);
}

//
// Function to set up the state to install files in the specified folder
//
HRESULT CXo::SetupInstallFolder(
			PXONLINECONTENT_INSTALL	pinstall,
			DWORD					dwFolderIndex
			)
{
	HRESULT			hr;
	PCFFOLDER_HMAC	pcffolder = pinstall->rgcffolderhmac + dwFolderIndex;
	DWORD			cbFolderLength;

	// Validate folder index
	if (dwFolderIndex >= pinstall->header.cfheader.cFolders)
		return(HRESULT_FROM_WIN32(ERROR_INVALID_DATA));
	
	// Reset the CFFILE and CFDATA data
	ZeroMemory(&pinstall->cfdata, sizeof(CFDATA));
	pinstall->cbUncompData = 0;
	pinstall->dwTemp = 0;

	// Figure out the exact length of the folder data, including all
	// intermittent CFDATA headers
	if (dwFolderIndex == (DWORD)(pinstall->header.cfheader.cFolders - 1))
	{
		// Last one, use the CAB length as reference
		cbFolderLength = pinstall->header.cfheader.cbCabinet;
	}
	else
	{
		// Point to the beginning of the next folder
		cbFolderLength = pcffolder[1].cffolder.coffCabStart;
	}
    Assert(cbFolderLength >= pcffolder->cffolder.coffCabStart);
	cbFolderLength -= pcffolder->cffolder.coffCabStart;

	// Initialize the decryption context for the specified folder
	hr = SymmdecInitialize(
				pinstall->rgbSymmetricKey, 
				XONLINE_KEY_LENGTH,
				cbFolderLength,
				&pcffolder->hmac,
				&pinstall->symmdecData);
	if (FAILED(hr))
		return(hr);
	
	// Reset the decompressor 
	LzxResetDecoder(&pinstall->lzx);
	
	// Set up for processing the specified folder
	pinstall->dwCurrentFolder = dwFolderIndex;
	pinstall->dwFolderOffset = 0;

	return(S_OK);	
}

//
// Function to reset the state after a failed resume attempt
//
VOID CXo::ResetAfterUnsuccessfulResume(
			PXONLINECONTENT_INSTALL	pinstall
			)
{
	// We set the expected size back to zero
	pinstall->dwExpected = 0;
	pinstall->dwWritten = 0;

	// Reset the CFFILE pointer so we reload the cache
	pinstall->pcffile = NULL;

	// We will not waste the cycles to open the cache
	// file again, we will yank it back out form our
	// CFFILE cache
	if ((pinstall->cffilecache.hFile != NULL) && 
		(pinstall->cffilecache.hFile != INVALID_HANDLE_VALUE))
	{
		pinstall->fileioData.hFile = pinstall->cffilecache.hFile;
		pinstall->cffilecache.hFile = INVALID_HANDLE_VALUE;
	}

	// Turn off the resume flag
	pinstall->dwFlags &= (~XONLINECONTENT_FLAG_RESUME);

	// Then the next state is to create a cache and download from scratch
	pinstall->dlphase = dlphaseCheckCache;
}

//
// Define the extension functions for ContentInstall
//

//
// Master cleanup function
//
VOID CXo::contdlCleanup(
			PXONLINETASK_CONTENT_DOWNLOAD	pcontdl
			)
{
	PXONLINECONTENT_INSTALL	pinstall = &pcontdl->install;

	// Cleanup absolutely everything in XONLINECONTENT_INSTALL
	if (pinstall->fileioData.hFile != INVALID_HANDLE_VALUE)
	{
		CloseHandle(pinstall->fileioData.hFile);
		pinstall->fileioData.hFile = INVALID_HANDLE_VALUE;
	}

	if (pinstall->verify.fileio.hFile != INVALID_HANDLE_VALUE)
	{
		CloseHandle(pinstall->verify.fileio.hFile);
		pinstall->verify.fileio.hFile = INVALID_HANDLE_VALUE;
	}

	CffileCacheDeinitialize(&pinstall->cffilecache);

	// Cleanup any XRL resources
	XRL_CleanupHandler(&(pcontdl->xrlasyncCatref.xrlasync));

	// If we mounted the private drive, we will need to unmount it
	XoUpdateUnmountPrivateDrive();

	// Wipe out all the data, especially the cryptographic keys
	ZeroMemory(pinstall, sizeof(XONLINECONTENT_INSTALL));
}			

//
// This function is called after all headers are received
//
HRESULT CXo::contdlDoneHeaders(
			PVOID	pvxrlasync
			)
{
	HRESULT							hr = S_OK;
	PXRL_ASYNC						pxrlasync = (PXRL_ASYNC)pvxrlasync;
	PXONLINETASK_CONTENT_DOWNLOAD	pcontdl;
	PXONLINECONTENT_INSTALL			pinstall;
	LARGE_INTEGER					liTemp;

	Assert(pxrlasync != NULL);
	
	// Extract the containg structure
	pcontdl = CONTAINING_RECORD(pxrlasync, 
					XONLINETASK_CONTENT_DOWNLOAD, xrlasync);
	pinstall = &pcontdl->install;					
	
	// Analyze the response code
	switch (pxrlasync->dwExtendedStatus)
	{
	case 200:
		// A 200 is coming back, which means the entire 
		// resource is returned by the server. 
		pinstall->dlphase = dlphaseRecvHeader;

		// Now we were expecting to resume, we have to make sure
		// everything is reverted back to its clean state
		if (pinstall->dwFlags & XONLINECONTENT_FLAG_RESUME)
		{
			// Reset the state
			ResetAfterUnsuccessfulResume(pinstall);
		}
		break;
		
	case 206:
		// This code is returned if a resume is attempted, and
		// that the server resource was not modified since the
		// date we supplied. This means that the server will 
		// return only the bytes that we don't already have.
		//
		// Just make sure we were expecting to resume
        Assert(pinstall->dwFlags & XONLINECONTENT_FLAG_RESUME);
		break;

	case 416:
		// This code means that the specified range is not 
		// satisfiable. This usually means that we already got 
		// the entire file to start with.
		//
		// We can move on to check the digest file and sign the
		// per-machine DRM
		pinstall->dwFlags |= XONLINECONTENT_FLAG_DONE_FILES;
		pinstall->dlphase = dlphaseSignDrm;
		break;
		
	default:
		hr = HRESULT_FROM_WIN32(ERROR_BAD_NET_RESP);
		TraceSz1(Verbose, "HTTP status: %u", pxrlasync->dwExtendedStatus);
	}

    return(hr);
}

//
// This function returnes whether the received data should be processed
// by the process data extension
//
BOOL CXo::contdlProcessData(
			PVOID	pvxrlasync
			)
{
	PXRL_ASYNC	pxrlasync = (PXRL_ASYNC)pvxrlasync;
	
	if (!IS_HEADER_MODE(pxrlasync))
		return(TRUE);
	return(FALSE);
}

HRESULT CXo::contdlCompleted(
			PVOID	pvxrlasync
			)
{
	HRESULT							hr = S_OK;
	PXRL_ASYNC						pxrlasync = (PXRL_ASYNC)pvxrlasync;
	PXONLINETASK_CONTENT_DOWNLOAD	pcontdl;
	LARGE_INTEGER					liTemp;

	Assert(pxrlasync != NULL);
	
	// Extract the containg structure
	pcontdl = CONTAINING_RECORD(pxrlasync, 
					XONLINETASK_CONTENT_DOWNLOAD, xrlasync);
	
	if (pcontdl->install.dwFlags & XONLINECONTENT_FLAG_DONE_FILES)
		return(S_OK);
	
	return(S_FALSE);
}

//
// Clenaup function for Download
//
VOID CXo::contdlCleanupHandler(
			PVOID	pvxrlasync
			)
{
	PXRL_ASYNC						pxrlasync = (PXRL_ASYNC)pvxrlasync;
	PXONLINETASK_CONTENT_DOWNLOAD	pcontdl;

	Assert(pxrlasync != NULL);
	
	// Extract the containg structure
	pcontdl = CONTAINING_RECORD(pxrlasync, 
					XONLINETASK_CONTENT_DOWNLOAD, xrlasync);
	contdlCleanup(pcontdl);	
}			

//
// Function to process header data
//
HRESULT CXo::contdlRecvHeader(
			PXONLINETASK_CONTENT_DOWNLOAD	pcontdl
			)
{
	HRESULT							hr = S_OK;
	PXRL_ASYNC						pxrlasync = NULL;
	PXONLINECONTENT_INSTALL			pinstall = NULL;
	PXONLINETASK_FILEIO				pfileio = NULL;
	DWORD							dwSize;

	Assert(pcontdl != NULL);

	pinstall = &pcontdl->install;
	pxrlasync = &pcontdl->xrlasync;

	// First figure out if this is the first time we are called.
	// This function may be called multiple times if the header
	// is large enough. We need to do some special initialization
	// the first time through
	if (pinstall->dwExpected == 0)
	{
		// We don't really want to waste time here. We want to at least
		// have the header and all the CFFOLDER entries in our buffer
		// before we even start to process the data
		dwSize = XONLINECONTENT_RESUME_MAX_READ_SIZE;
		if (pxrlasync->uliContentLength.QuadPart < dwSize)
			dwSize = pxrlasync->uliContentLength.u.LowPart;
		if (pxrlasync->dwCurrent < dwSize)
			goto Done;

		// By now, we would have received the entire header, plus all
		// CFFOLDER entries.

		// Call function to process fixed header
		hr = ProcessFixedHeader(pcontdl, pxrlasync->pBuffer);
		if (FAILED(hr))
			goto Error;

		// Make sure cabinet size equals reported content length
		if ((DWORD)pxrlasync->uliContentLength.u.LowPart != (DWORD)pinstall->header.cfheader.cbCabinet) 
		{
			AssertSz(FALSE, "Content length does not match package size");
			hr = HRESULT_FROM_WIN32(ERROR_INVALID_DATA);
			goto Error;
		}

		// Call another function to process the CFFOLDER entries
		// this also fills in pinstall->dwExpected
		hr = ProcessCffolderEntries(pcontdl,
					pxrlasync->pBuffer + XONLINECONTENT_COMPLETE_HEADER_SIZE);
		if (FAILED(hr))
			goto Error;

		// Make sure the size makes sense
		if (pinstall->dwExpected > pxrlasync->uliContentLength.QuadPart)
		{
			AssertSz(FALSE, "ProcessCffolderEntries: invalid data offset");
			hr = HRESULT_FROM_WIN32(ERROR_INVALID_DATA);
			goto Error;
		}

		// We nput some initiali random junk for the checkpoint in our 
		// original package to confound the prying eyes. Make sure our 
		// checkpoint is set to zero
		*(LPDWORD)(pxrlasync->pBuffer + XONLINECONTENT_CHECKPOINT) = 0;

		// We know exactly how much data to cache (pinstall->dwExpected).
		// grow the file to that size to not less than the epxected size
		hr = SetEndOfFileNt(
					pinstall->fileioData.hFile, 
					SECTOR_ALIGNMENT_ROUND_UP(pinstall->dwExpected));
		if (FAILED(hr))
			goto Error;

		// Fall thru ...
	}

	// See if we have a pending write
	if ((pinstall->dwFlags & XONLINECONTENT_FLAG_WRITE_PENDING) != 0)
	{
		XONLINETASK_HANDLE	hfileio = (XONLINETASK_HANDLE)&(pinstall->fileioData);
		DWORD				dwWritten;
		
		// We have a write pending, pump the subtask until
		// the write completes
		hr = XOnlineTaskContinue(hfileio);
		if (XONLINETASK_STATUS_AVAILABLE(hr))
		{
			// Turn off the pending flag
			pinstall->dwFlags &= (~XONLINECONTENT_FLAG_WRITE_PENDING);
		
			// Write had returned results, see if it succeeded
			if (FAILED(hr))
				goto Error;

			// Succeeded, now get the results and analyze the data
			hr = GetReadWriteFileResults(hfileio, &dwWritten, NULL);
			if (FAILED(hr))
				goto Error;

			// Written bytes should be a multiple of the sector size
			Assert(IS_HD_SECTOR_ALIGNED(dwWritten));

			// We consume the bytes that were successfully written.
			// Update the buffer state to reflect this
			pxrlasync->dwCurrent -= pinstall->dwTemp;
			MoveMemory(pxrlasync->pBuffer, 
						pxrlasync->pBuffer + pinstall->dwTemp,
						pxrlasync->dwCurrent);

			// Update written bytes
			dwWritten += pinstall->dwWritten;
			pinstall->dwWritten += pinstall->dwTemp;

			// OK, see if we have written all we need ...
			if (dwWritten >= pinstall->dwExpected)
			{
				// Make sure the actual bytes written match up
                Assert(pinstall->dwExpected == pinstall->dwWritten);

				// Set up for processing first folder
				hr = SetupInstallFolder(pinstall, 0);
				if (FAILED(hr))
					goto Error;

				// Now we have all the header data checked and verified, 
				// the cache file written and closed, we can set
				// up our CFFILE cache. It will inherit the file handle
				// from pinstall->fileioData.hFile.
				// We will first verify the CFFILE entries before we
				// use them to prevent using invalid data. We will use
				// the decompress buffer for this because it is much
				// larger and still available at this point.

				// Figure out the size of CFFILE entries
				dwWritten = pinstall->dwExpected - 
								pinstall->header.cfheader.coffFiles;
				hr = CffileCacheInitialize(
							pinstall->fileioData.hFile,
							pinstall->header.cfheader.coffFiles,
							pinstall->header.cfheader.cFiles,
							pinstall->rgbSymmetricKey, 
							XONLINE_KEY_LENGTH,
							dwWritten,
							pinstall->rgbDecompress,
							XONLINECONTENT_LZX_OUTPUT_SIZE,
							&(pinstall->header.digestFiles),
							pinstall->xontask.hEventWorkAvailable, 
							&(pinstall->cffilecache));
				if (FAILED(hr))
					goto Error;

				// The CFFILE cache had taken over the file handle, 
				// mark this one as invalid.
				pinstall->fileioData.hFile = INVALID_HANDLE_VALUE;

				// We will move on to verify the CFFILE entries
				pinstall->dlphase = dlphaseVerifyCffiles;

				// Now we may have more data already in our buffer,
				// if we do, we will continue processing until we have
				// processing all data in the buffer
				if (pxrlasync->dwCurrent)
					goto Exit;
			}

			// Set the don't continue flag to get more data
			goto Done;
		}
	}

	if ((pinstall->dwFlags & XONLINECONTENT_FLAG_WRITE_PENDING) == 0)
	{
		PXONLINETASK_FILEIO	pfileio2 = &pinstall->fileioData;
		ULARGE_INTEGER		uliOffset;
	
		// We came in with no pending write, write out as much as we can
		// do all the math to do the sector alignment and stuff
		if ((pinstall->dwWritten + pxrlasync->dwCurrent) >= pinstall->dwExpected)
		{
			pinstall->dwTemp = pinstall->dwExpected - pinstall->dwWritten;
			dwSize = SECTOR_ALIGNMENT_ROUND_UP(pinstall->dwTemp);
		}
		else
		{
			dwSize = SECTOR_ALIGNMENT_ROUND_DOWN(pxrlasync->dwCurrent);
			pinstall->dwTemp = dwSize;
		}

		Assert(dwSize > 0);

		// Kick off async write
		uliOffset.QuadPart = pinstall->dwWritten;
		ReadWriteFileInitializeContext(pfileio2->hFile,
					pxrlasync->pBuffer, dwSize, uliOffset, 
					pinstall->xontask.hEventWorkAvailable, 
					pfileio2);

		hr = WriteFileInternal(pfileio2);
		if (FAILED(hr))
			goto Error;

		// Set the flag to indicate that we are pending write complete
		pinstall->dwFlags |= XONLINECONTENT_FLAG_WRITE_PENDING;		
	}
	
Exit:
	return(S_OK);

Error:
	return(hr);

Done:
	return(XONLINETASK_S_SUCCESS);
}

//
// Function to process raw CAB data and install files
//
HRESULT CXo::contdlInstallFiles(
			PXONLINETASK_CONTENT_DOWNLOAD	pcontdl
			)
{
	HRESULT							hr = S_OK;
	PXRL_ASYNC						pxrlasync;
	PXONLINECONTENT_INSTALL			pinstall;
	PBYTE							pbBuffer;
	DWORD							cbAvailable;
	DWORD							dwIndex;
	WORD							wCompType;

	Assert(pcontdl != NULL);

	pinstall = &pcontdl->install;
	pxrlasync = &pcontdl->xrlasync;

	// Figure out if we are starting a new target file.
	if (!pinstall->pcffile)
	{
		// Yup, we have no current CFFILE, which means we are either
		// called for the first time, or we have just completely processed 
		// the previous target file. 

		// We try to get the next CFFILE entry from the cache.
		hr = CffileCacheGetNextEntry(
					&pinstall->cffilecache,
					&dwIndex,
					&pinstall->pcffile,
					&pinstall->szFileName,
					&pinstall->cbFileName);
		if (FAILED(hr))
			goto Error;

		// Now see if we got it right away ...
		if (hr == S_CFFILECACHE_IO_PENDING)
		{
			// Nope, the next entry happens to be on disk and we have
			// to load it up. There's nothing we can do now, so might
			// as well return to the dispatcher. 
			hr = S_OK;
			goto Exit;
		}

		if (hr == S_CFFILECACHE_NO_MORE_ITEMS)
		{
#ifndef NO_ENCRYPT
		
			// There are no more items, we are done with the CAB. Make
			// sure the data in the last folder is authentic.
			hr = SymmdecVerify(&pinstall->symmdecData);
			if (FAILED(hr))
				goto Error;

			// Also make sure the file cache is good
			hr = CffileCacheVerifyChecksum(
						&pinstall->cffilecache);
			if (FAILED(hr))
				goto Error;
						
#endif

			// We are done with installation phase
			pinstall->dwFlags |= XONLINECONTENT_FLAG_DONE_FILES;

			// Close the file
			if (pinstall->fileioData.hFile != INVALID_HANDLE_VALUE)
			{
				CloseHandle(pinstall->fileioData.hFile);
				pinstall->fileioData.hFile = INVALID_HANDLE_VALUE;
			}
			
			// Move on to the next phase ...
			pinstall->dlphase = dlphaseSignDrm;
			hr = XONLINETASK_S_SUCCESS;
			goto Exit;
		}
		
		// Got new CFFILE entry, now check to see if we need to create
		// the file.
		if (pinstall->pcffile->time & XONLINECONTENT_A_CONTINUED_FROM_PREV)
		{
			// This folder is actually a continuation of the same file from
			// a previous folder (note: this is an Xbox extension of the CAB
			// format and is not supported in the standard CAB format).
			// Instead of creating a new file, we will continue with the
			// same file handle.
			pinstall->dwExpected += pinstall->pcffile->cbFile;
			TraceSz3(Verbose, "[%u] Continuing %s, %u bytes", dwIndex, 
                     pinstall->szFileName, pinstall->dwExpected);

			// Must be first file in folder
            Assert(pinstall->pcffile->uoffFolderStart == 0);

			// Mark the fact that the file is to be continued
			pinstall->dwFlags |= XONLINECONTENT_FLAG_CONTINUE_FILE;
		}
		else
		{
			// This is a fresh file, go and create it.
			pinstall->dwExpected = pinstall->pcffile->cbFile;
			pinstall->dwWritten = 0;
//			TraceSz3(Verbose, "[%u] Installing %s, %u bytes", 
//					dwIndex, pinstall->szFileName, pinstall->dwExpected);

			// Create the new file
			pinstall->dlphase = dlphaseCreateTarget;
		}

		// OK, we got the next CFFILE entry. Figure out if we're in the same
		// folder
		dwIndex = pinstall->pcffile->iFolder;
		if (dwIndex != pinstall->dwCurrentFolder)
		{
#ifndef NO_ENCRYPT
		
			// Different folder. This means we should move on to the next
			// one. But this also means that we have extracted all files
			// from this folder. We will run the Hmac checksum to make sure
			// all data is authentic.
			hr = SymmdecVerify(&pinstall->symmdecData);
			if (FAILED(hr))
				goto Error;

#endif

            Assert(dwIndex == (pinstall->dwCurrentFolder + 1));

			// Set up for next folder
			hr = SetupInstallFolder(pinstall, dwIndex);
			if (FAILED(hr))
				goto Error;

			// Write out thie checkpoint
			pinstall->dlphase = dlphaseCheckpoint;
			goto Exit;
		}

		// Make sure the folder offsets match up
        Assert(pinstall->pcffile->uoffFolderStart == pinstall->dwFolderOffset);

		// Break out if we are going to another phase
		if (pinstall->dlphase != dlphaseInstallFiles)
			goto Exit;
	}
	
	// If we get here, we can safely assume that we have a CFFILE
	// entry to process, and that the folder is set to the correct
	// index.

	pbBuffer = pxrlasync->pBuffer;
	cbAvailable = pxrlasync->dwCurrent;

	// Now see if we need a new CFDATA block
	if (pinstall->cfdata.cbUncomp == 0)
	{
		// Yup, we need to get the next CFDATA entry
		// If we don't even have the minimum required amount of data, just
		// return and ask for more data
		if (cbAvailable < sizeof(CFDATA))
		{
			hr = XONLINETASK_S_SUCCESS;
			goto Exit;
		}

#ifndef NO_ENCRYPT

		// Decrypt the CFDATA header
		hr = SymmdecDecrypt(&pinstall->symmdecData,
					pbBuffer, sizeof(CFDATA));
		if (FAILED(hr))
			goto Error;

#endif

		// Copy into pinstall->cfdata
		memcpy(&pinstall->cfdata, pbBuffer, sizeof(CFDATA));

		// Adjust the values
		pbBuffer += sizeof(CFDATA);
		cbAvailable -= sizeof(CFDATA);

		// Mark the fact that the data is not decrypted/decompressed
		// yet.
		pinstall->cbUncompData = 0;
	}

	// See if we need to decrypt/decompress that data
	if (pinstall->cbUncompData == 0)
	{
		// Decompression needs to work on full blocks. The block size varies
		// from block to block. To overcome this, we will wait until we have
		// the full block, and then we will decrypt and decompress at the same
		// time.
		if (cbAvailable >= pinstall->cfdata.cbData)
		{
#ifndef NO_ENCRYPT
		
			// Yes, we have the full block. We can decrypt and decompress
			hr = SymmdecDecrypt(&pinstall->symmdecData,
						pbBuffer, pinstall->cfdata.cbData);
			if (FAILED(hr))
				goto Error;

#endif

//			TraceSz1(Verbose, "typeCompress = %04x", 
//					pinstall->rgcffolderhmac[pinstall->dwCurrentFolder].cffolder.typeCompress);

			// Decompress this data if necessary
			wCompType = pinstall->rgcffolderhmac[pinstall->dwCurrentFolder].
								cffolder.typeCompress;
			switch (CompressionTypeFromTCOMP(wCompType))
			{
			case tcompTYPE_NONE:
				// Just copy into our decompressed buffer
				memcpy(pinstall->rgbDecompress, pbBuffer, pinstall->cfdata.cbData);
				break;
				
			case tcompTYPE_LZX:
				// Make sure the our window is big enough to handle the specified
				// window bits ...
				dwIndex = 1 << LZXCompressionWindowFromTCOMP(wCompType);
				if (dwIndex > XONLINECONTENT_LZX_WINDOW_SIZE)
				{
					// Our window is not big enough to decode this data, we must
					// fail
					hr = HRESULT_FROM_WIN32(ERROR_INVALID_DATA);
					goto Error;
				}

				// Decompress the data
				if (!LzxDecode(
							&pinstall->lzx, 
							pinstall->cfdata.cbUncomp,
							pbBuffer, 
							pinstall->cfdata.cbData, 
							pinstall->rgbDecompress + pinstall->dwTemp, 
							&pinstall->cbUncompData) ||
					(pinstall->cfdata.cbUncomp != pinstall->cbUncompData))
				{
					hr = HRESULT_FROM_WIN32(ERROR_INVALID_DATA);
					goto Error;
				}
				break;

			default:
				// Don't support anything else for now
				AssertSz(FALSE, "Unsupported compression type!");
				hr = HRESULT_FROM_WIN32(ERROR_INVALID_DATA);
				goto Error;
			}

			// Adjust the amount of uncompressed data to include any residue
			// carried over from a previous CFDATA
			pinstall->cbUncompData += pinstall->dwTemp;
			pinstall->cfdata.cbUncomp = (WORD)pinstall->cbUncompData;
			pinstall->dwTemp = 0;

			// Skip over all data that we processed
			pbBuffer += pinstall->cfdata.cbData;
			cbAvailable -= pinstall->cfdata.cbData;
			
			// Write out this data
			pinstall->dlphase = dlphaseWriteTarget;
		}
		else
		{
			// Well, we might have more data but not enough to fill
			// the whole block. We will have to receive more data to
			// continue ...
			hr = XONLINETASK_S_SUCCESS;
		}
	}
	else
	{
		// Well, we got some more data, write it out!
		pinstall->dlphase = dlphaseWriteTarget;
	}

	// Move unprocessed data up to the front
	pxrlasync->dwCurrent = cbAvailable;
	MoveMemory(pxrlasync->pBuffer, pbBuffer, cbAvailable);
	
Exit:
	return(hr);

Error:
	goto Exit;
}

//
// Function to create the target file and grow it to a certain size
//
HRESULT CXo::contdlCreateTarget(
			PXONLINETASK_CONTENT_DOWNLOAD	pcontdl
			)
{
	HRESULT					hr = S_OK;
	PXONLINECONTENT_INSTALL	pinstall = NULL;
	PXONLINETASK_FILEIO		pfileio = NULL;
	XONLINEDIR_TYPE			dirtype;
	LARGE_INTEGER			liFileSize;
	DWORD					dwAlignedSize;
	DWORD					cbPath;

	Assert(pcontdl != NULL);

	pinstall = &pcontdl->install;
	pfileio = &pinstall->fileioData;

	// Close the previous file if opened
	if (pfileio->hFile != INVALID_HANDLE_VALUE)
	{
		CloseHandle(pfileio->hFile);
		pfileio->hFile = INVALID_HANDLE_VALUE;
	}

	// Figure out the target directory based on the type of content
	// currently installing
	dirtype = pinstall->dirtypeTarget;
	
	// CFFILE.time contains the flag that tells us whether this 
	// file should be in the title or user partition
	if ((pinstall->pcffile->time & XONLINECONTENT_A_USER_DATA) != 0)
		dirtype = dirtypeUserContent;
	
	// Build the target path
	cbPath = XONLINECONTENT_MAX_PATH;
	hr = BuildTargetPath(pinstall->dwTitleId, 
				pinstall->OfferingId,
				pinstall->dwBitFlags,
				pinstall->szFileName, 
				dirtype,
				&cbPath, pinstall->szPath);
	if (FAILED(hr))
		return(hr);

	// We will open this file if it exists, or create if not.
	pfileio->hFile = CreateFile(
				pinstall->szPath, 
				GENERIC_READ | GENERIC_WRITE, 
				FILE_SHARE_READ, 
				NULL, 
				OPEN_ALWAYS, 
				FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN |
				FILE_FLAG_OVERLAPPED | FILE_FLAG_NO_BUFFERING, 
				NULL);
	if (pfileio->hFile == INVALID_HANDLE_VALUE)
	{
		DWORD	dwError = GetLastError();
		if (dwError == ERROR_PATH_NOT_FOUND)
		{
			// Directory does not exists, go ahead and create it
			hr = CreateDirectoryInitializeContext(
						pinstall->szPath, 
						cbPath,
						XONLINETASK_CDFLAG_FILE_PATH,
						pinstall->xontask.hEventWorkAvailable,
						&pinstall->dirops);
			if (FAILED(hr))
				return(hr);
				
			pinstall->dlphase = dlphaseCreateDirectory;
			goto Exit;
		}

		return(HRESULT_FROM_WIN32(dwError));
	}

	// Now figure out the current file size
	if (!GetFileSizeEx(pfileio->hFile, &liFileSize))
		return(HRESULT_FROM_WIN32(GetLastError()));

	// Figure out what the sector aligned round up size is
	dwAlignedSize = SECTOR_ALIGNMENT_ROUND_UP(pinstall->dwExpected);
	
	// If file is too small, figure out a good growth size
	if (liFileSize.QuadPart < dwAlignedSize)
	{
		if ((dwAlignedSize - liFileSize.u.LowPart) >
			XONLINECONTENT_INCREMENTAL_GROW_FILE_SIZE)
		{
			// Grow by XONLINECONTENT_INCREMENTAL_GROW_FILE_SIZE
			dwAlignedSize = SECTOR_ALIGNMENT_ROUND_UP(
						liFileSize.u.LowPart) + 
						XONLINECONTENT_INCREMENTAL_GROW_FILE_SIZE;
		}
	}

	// Set the end of file position at dwAlignedSize
	hr = SetEndOfFileNt(pfileio->hFile, dwAlignedSize);
	if (FAILED(hr))
		return(hr);

	// Go back to installing files
	pinstall->dwFileSize = dwAlignedSize;
	pinstall->dlphase = dlphaseInstallFiles;
	
Exit:
	return(S_OK);
}

//
// Function to write to the target file and grow it if necessary
//
HRESULT CXo::contdlWriteTarget(
			PXONLINETASK_CONTENT_DOWNLOAD	pcontdl
			)
{
	HRESULT					hr = S_OK;
	PXONLINECONTENT_INSTALL	pinstall = NULL;
	PXONLINETASK_FILEIO		pfileio = NULL;
	DWORD					dwToWrite;
	DWORD					dwRemaining;
	DWORD					dwNewSize;

	Assert(pcontdl != NULL);

	pinstall = &pcontdl->install;
	pfileio = &pinstall->fileioData;

    Assert(IS_HD_SECTOR_ALIGNED(pinstall->dwWritten));
    Assert(IS_HD_SECTOR_ALIGNED(pinstall->dwFileSize));

	// See if we have a pending write
	if ((pinstall->dwFlags & XONLINECONTENT_FLAG_WRITE_PENDING) != 0)
	{
		XONLINETASK_HANDLE	hfileio = (XONLINETASK_HANDLE)pfileio;
		DWORD				dwWritten;
		
		// We have a write pending, pump the subtask until
		// the write completes
		hr = XOnlineTaskContinue(hfileio);
		if (XONLINETASK_STATUS_AVAILABLE(hr))
		{
			// Turn off the pending flag
			pinstall->dwFlags &= (~XONLINECONTENT_FLAG_WRITE_PENDING);
		
			// Write had returned results, see if it succeeded
			if (FAILED(hr))
				goto Error;

			// Succeeded, now get the results and analyze the data
			hr = GetReadWriteFileResults(hfileio, &dwWritten, NULL);
			if (FAILED(hr))
				goto Error;

			// Written bytes should be a multiple of the sector size
			Assert(IS_HD_SECTOR_ALIGNED(dwWritten));
            Assert(pinstall->dwTemp <= pinstall->cbUncompData);

			// We consume the bytes that were successfully written.
			// Move unprocessed data up to the front
			pinstall->cbUncompData -= pinstall->dwTemp;
			pinstall->cfdata.cbUncomp = (WORD)pinstall->cbUncompData;
			pinstall->dwFolderOffset += pinstall->dwTemp;
			MoveMemory(pinstall->rgbDecompress, 
						pinstall->rgbDecompress + pinstall->dwTemp,
						pinstall->cbUncompData);

			// Update written bytes
			pinstall->dwWritten += dwWritten;

			// OK, see if we have written all we need ...
			if (pinstall->dwWritten >= pinstall->dwExpected)
			{
				// Yup, written everything, chop off extra bytes if we
				// have written too much (a la unbuffered write). 
				if (pinstall->dwWritten > pinstall->dwExpected)
				{
					hr = SetEndOfFileNt(
								pinstall->fileioData.hFile, 
								pinstall->dwExpected);
					if (FAILED(hr))
						goto Error;
				}

				// We know we are done with this CFFILE, move to next
				pinstall->pcffile = NULL;
				pinstall->dlphase = dlphaseInstallFiles;
				goto Exit;
			}

			// OK, we're not quite done yet, and we're at the end of
			// this CFDATA block (maybe with some residue as well)
            Assert(pinstall->cbUncompData < XBOX_HD_SECTOR_SIZE);

			// We mark the residue with pinstall->cbUncompDone, and reset
			// CFDATA.cbUncomp to zero. This will force a new CFDATA to
			// be read, and we can combine this data together
			pinstall->dwTemp = pinstall->cbUncompData;
			pinstall->cfdata.cbUncomp = 0;

			// Back to install files
			pinstall->dlphase = dlphaseInstallFiles;
		}
	}
	else
	{
		ULARGE_INTEGER		uliOffset;

		// Figure out how much we are writing out this time, as well
		// as how many effective bytes were written (not including
		// filler for sector alignment). However, if this is a 
		// continuation file, we don't write past end.
		dwRemaining = pinstall->dwExpected - pinstall->dwWritten;

		if ((pinstall->cbUncompData >= dwRemaining) &&
			!(pinstall->pcffile->time & XONLINECONTENT_A_CONTINUED_INTO_NEXT))
		{
			pinstall->dwTemp = dwRemaining;
			dwToWrite = SECTOR_ALIGNMENT_ROUND_UP(dwRemaining);
		}
		else
		{
			dwToWrite = SECTOR_ALIGNMENT_ROUND_DOWN(pinstall->cbUncompData);
			pinstall->dwTemp = dwToWrite;
		}

		// Figure out if we need to grow the file ...
		if (pinstall->dwFileSize < (pinstall->dwWritten + dwToWrite))
		{
			// We need to grow the file anyway, figure out by how much
			dwNewSize = SECTOR_ALIGNMENT_ROUND_UP(pinstall->dwExpected);
			if ((dwNewSize - pinstall->dwFileSize) > 
						XONLINECONTENT_INCREMENTAL_GROW_FILE_SIZE)
			{
				// Grow by XONLINECONTENT_INCREMENTAL_GROW_FILE_SIZE
				dwNewSize = pinstall->dwFileSize + XONLINECONTENT_INCREMENTAL_GROW_FILE_SIZE;
			}

			hr = SetEndOfFileNt(pfileio->hFile, dwNewSize);
			if (FAILED(hr))
				return(hr);

			// Update the file size
			pinstall->dwFileSize = dwNewSize;
		}
	
		// We came in with no pending write, write out the data
		uliOffset.QuadPart = pinstall->dwWritten;
		ReadWriteFileInitializeContext(pfileio->hFile,
					pinstall->rgbDecompress,
					dwToWrite,
					uliOffset,
					pinstall->xontask.hEventWorkAvailable, 
					pfileio);

		hr = WriteFileInternal(pfileio);
		if (FAILED(hr))
			goto Error;

		// Set the flag to indicate that we are pending write complete
		pinstall->dwFlags |= XONLINECONTENT_FLAG_WRITE_PENDING;		
	}

Exit:
	return(S_OK);

Error:
	return(hr);
}

//
// Function to recursively create the directories for the target
//
HRESULT CXo::contdlCreateDirectory(
			PXONLINETASK_CONTENT_DOWNLOAD	pcontdl
			)
{
	HRESULT				hr = S_OK;
	XONLINETASK_HANDLE	hdirops;

	Assert(pcontdl != NULL);

	// Retrieve the dirops structure
	hdirops = (XONLINETASK_HANDLE)&(pcontdl->install.dirops);
	hr = XOnlineTaskContinue(hdirops);
	if (XONLINETASK_STATUS_AVAILABLE(hr))
	{
		// Create dir had returned results, see if it succeeded
		if (FAILED(hr))
			goto Exit;

		// Go back to create target phase
		hr = S_OK;
		pcontdl->install.dlphase = dlphaseCreateTarget;
	}

Exit:
	return(hr);
}

//
// Function to wipe the target location before a clean install
//
HRESULT CXo::contdlWipeTarget(
			PXONLINETASK_CONTENT_DOWNLOAD	pcontdl
			)
{
	HRESULT					hr = S_OK;
	DWORD					cbPath;
	PXONLINECONTENT_INSTALL	pinstall;

	Assert(pcontdl != NULL);

	// Retrieve the dirops structure
	pinstall = &pcontdl->install;

	// Make sure the directories are created if they are not
	// already there
	hr = XOnlineTaskContinue((XONLINETASK_HANDLE)&(pcontdl->install.dirops));
	if (XONLINETASK_STATUS_AVAILABLE(hr))
	{
		// Create dir had returned results, see if it succeeded
		if (FAILED(hr))
			goto Error;

		// Build the root target path for this package
		cbPath = XONLINECONTENT_MAX_PATH;
		hr = BuildTargetPath(
					pinstall->dwTitleId, 
					pinstall->OfferingId,
					pinstall->dwBitFlags,
					XONLINECONTENT_CACHE_FILE_NAME,
					pinstall->dirtypeTarget,
					&cbPath,
					pinstall->szPath);
		if (FAILED(hr))
			goto Error;
			
		// Make sure all the directories are created
		hr = CreateDirectoryInitializeContext(
					pinstall->szPath, 
					cbPath,
					XONLINETASK_CDFLAG_FILE_PATH,
					pinstall->xontask.hEventWorkAvailable,
					&pinstall->dirops);
		if (FAILED(hr))
			goto Error;

		// Now we can create the cache
		pinstall->dlphase = dlphaseCreateCache;
	}

Exit:
	return(hr);

Error:
	// Deal with errors swiftly
	goto Exit;
}

//
// Function to create the cache
//
HRESULT CXo::contdlCreateCache(
			PXONLINETASK_CONTENT_DOWNLOAD	pcontdl
			)
{
	HRESULT					hr = S_OK;
	PXONLINECONTENT_INSTALL	pinstall;
	PXONLINETASK_FILEIO		pfileio;
	XONLINETASK_HANDLE		hdirops;
	PXRL_ASYNC				pxrlasync;
	LARGE_INTEGER			liTemp;

	Assert(pcontdl != NULL);

	// Retrieve the dirops structure
	pinstall = &pcontdl->install;
	pxrlasync = &pcontdl->xrlasync;
	pfileio = &pinstall->fileioData;
	hdirops = (XONLINETASK_HANDLE)&(pinstall->dirops);

	// Make sure the directories are created if they are not
	// already there
	hr = XOnlineTaskContinue(hdirops);
	if (XONLINETASK_STATUS_AVAILABLE(hr))
	{
		// Create dir had returned results, see if it succeeded
		if (FAILED(hr))
			goto Error;

		// Create the cache file. This should succeed
		hr = S_OK;
		TraceSz1(Verbose, "Creating cache file: %s", pinstall->szPath);
		pfileio->hFile = CreateFile(
					pinstall->szPath, 
					GENERIC_READ | GENERIC_WRITE, 
					FILE_SHARE_READ, 
					NULL, 
					CREATE_ALWAYS, 
					FILE_ATTRIBUTE_NORMAL |
					FILE_FLAG_OVERLAPPED | 
					FILE_FLAG_NO_BUFFERING, 
					NULL);
		if (pfileio->hFile == INVALID_HANDLE_VALUE)
		{
			hr = HRESULT_FROM_WIN32(GetLastError());
			goto Error;
		}

		// Cache the next phase, and see if we have custom tasks
		pinstall->dlphaseNext = dlphaseRecvHeader;
		pinstall->dlphase = dlphasePreCustom;
	}

Exit:
	return(hr);

Error:
	// Deal with errors swiftly
	goto Exit;
}

//
// Function to wait for catref request to return
//
HRESULT CXo::contdlCatref(
			PXONLINETASK_CONTENT_DOWNLOAD	pcontdl
			)
{
	HRESULT					hr = S_OK;
	PXRL_ASYNC				pxrlasync;
	PXRL_ASYNC_EXTENDED		pxrlasyncextCatref;
	XONLINETASK_HANDLE		hCatref;
	PBYTE					pbBuffer;
	DWORD					cbBuffer;
	DWORD					dwHttpStatus = 0;
	ULARGE_INTEGER			uliContentLength;	

	Assert(pcontdl != NULL);

	pxrlasyncextCatref = &pcontdl->xrlasyncCatref;
	pxrlasync = &pxrlasyncextCatref->xrlasync;
	hCatref = (XONLINETASK_HANDLE)pxrlasyncextCatref;

	// Wait for the CATREF request to complete
	hr = XOnlineTaskContinue(hCatref);
	if (XONLINETASK_STATUS_AVAILABLE(hr))
	{
		// CATREF had returned results, see if it succeeded
		if (FAILED(hr))
			goto Error;

		// We got the entire cache, process it.
		hr = XOnlineUploadGetResults(
					hCatref, &pbBuffer, &cbBuffer, 
					NULL, &uliContentLength,
					&dwHttpStatus, NULL);
		if (FAILED(hr))
			goto Error;

		// Check if we have a favorable result from the CATREF server
		// We expect a 200 return code on success, any other code will be 
		// deemed an error.
		if (dwHttpStatus != XONLINEUPLOAD_EXSTATUS_SUCCESS)
		{
			// The CATREF request did not succeed, we will return a specific
			// return code indicating that the server is unavailable.
			hr = HRESULT_FROM_WIN32(ERROR_SERVICE_SPECIFIC_ERROR);
			goto Error;
		}

		// Now check whether the package is found in CATREF
		if (pxrlasync->hrXErr == S_FALSE)
		{
			// CATREF request succeeded, but the package is not found.
			// so we return a relevant error code
			hr = HRESULT_FROM_WIN32(ERROR_NOT_FOUND);
			goto Error;
		}

		// Make sure the info returned makes sense. This also implies
		// that everything that comes back fits in our buffer
		Assert(pbBuffer != NULL);
		Assert(uliContentLength.u.HighPart == 0);
		Assert(cbBuffer == uliContentLength.u.LowPart);

		// Parse the returned buffer to get the required fields
		hr = ParseReferralResults(pcontdl, pbBuffer, cbBuffer);
		if (FAILED(hr))
			goto Error;

		// OK, got the referral info, move on to check if we have
		// an existing and valid cache.
		pcontdl->install.dlphase = dlphaseCheckCache;
	}					

Exit:
	return(hr);

Error:
	TraceSz2(Verbose, "Failed to get CATREF referral (%h), HTTP response code %u", 
				hr, dwHttpStatus);

	// BUGBUG: We could possibly implement retry logic here ...
	
	// Change the error code to specifically indicate that the failure
	// occurred during CATREF referral.
	hr = HRESULT_FROM_WIN32(ERROR_SERVICE_SPECIFIC_ERROR);
	goto Exit;
}

//
// Function to see if we have a valid cache file on the hard disk (resume)
//
HRESULT CXo::contdlCheckCache(
			PXONLINETASK_CONTENT_DOWNLOAD	pcontdl
			)
{
	HRESULT					hr = S_OK;
	PXRL_ASYNC				pxrlasync = NULL;
	PXONLINECONTENT_INSTALL	pinstall = NULL;
	PXONLINETASK_FILEIO		pfileio = NULL;
	LARGE_INTEGER			liFileSize;
	ULARGE_INTEGER			uliTemp;
	DWORD					dwAlignedSize;
	DWORD					cbPath;

	Assert(pcontdl != NULL);

	pinstall = &pcontdl->install;
	pxrlasync = &pcontdl->xrlasync;
	pfileio = &pinstall->fileioData;

	// Build the root target path for this package
	cbPath = XONLINECONTENT_MAX_PATH;
	hr = BuildTargetPath(
				pinstall->dwTitleId, 
				pinstall->OfferingId,
				pinstall->dwBitFlags,
				XONLINECONTENT_CACHE_FILE_NAME,
				pinstall->dirtypeTarget,
				&cbPath,
				pinstall->szPath);
	if (FAILED(hr))
		goto Error;

	// Just create a new cache on an offline install 
	// (Don't handle resume in this case)
	if (pinstall->dwFlags & XONLINECONTENT_FLAG_OFFLINE_INSTALL)
		goto NoCache;

	// We will skip this if the file is already open. For example, if
	// we are here bacause of an aborted resume, the file is already open
	// and we would have to skip this mumbo-jumbo anyway.
	if ((pfileio->hFile == NULL) || (pfileio->hFile == INVALID_HANDLE_VALUE))
	{
		// Attempt to open the cache file
		pfileio->hFile = CreateFile(
					pinstall->szPath, 
					GENERIC_READ | GENERIC_WRITE, 
					FILE_SHARE_READ, 
					NULL, 
					OPEN_EXISTING, 
					FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN |
					FILE_FLAG_OVERLAPPED | FILE_FLAG_NO_BUFFERING, 
					NULL);
		if (pfileio->hFile != INVALID_HANDLE_VALUE)
		{
			// We got a cache file here, see if we can actually use it to
			// resume from somewhere

			// Get the file size
			if (!GetFileSizeEx(pfileio->hFile, &liFileSize))
			{
				// This is a recoverable error: we will just assume there
				// is no cache file
				goto NoCache;
			}
			
			// Make sure cache size makes sense
			if (liFileSize.u.LowPart > XONLINECONTENT_COMPLETE_HEADER_SIZE)
			{
				// Figure out how much to read and sector-align
				pinstall->dwExpected = XONLINECONTENT_RESUME_MAX_READ_SIZE;
				if (liFileSize.u.LowPart < XONLINECONTENT_RESUME_MAX_READ_SIZE)
					pinstall->dwExpected = liFileSize.u.LowPart;
				dwAlignedSize = SECTOR_ALIGNMENT_ROUND_UP(pinstall->dwExpected);

				// Read in the entire cache file
				uliTemp.QuadPart = 0;
				ReadWriteFileInitializeContext(
							pfileio->hFile,
							pinstall->rgbDecompress,
							dwAlignedSize,
							uliTemp,
							pinstall->xontask.hEventWorkAvailable, 
							pfileio);

				hr = ReadFileInternal(pfileio);
				if (FAILED(hr))
					goto Error;

				// Use the existing cache
				pinstall->dlphase = dlphaseReadCache;
				pinstall->dwFlags |= XONLINECONTENT_FLAG_RESUME;
			}
		}
		else
		{
			DWORD	dwError = GetLastError();
			
			// Make sure we can't open it because it's not there
			if ((dwError == ERROR_FILE_NOT_FOUND) ||
				(dwError == ERROR_PATH_NOT_FOUND))
			{
				// See if we need to check if the package was actually properly
				// installed
				if ((pinstall->dwFlags & XONLINECONTENT_FLAG_NOT_INSTALLED) == 0)
				{
					// See if a manifest exists, and if so, whether it is signed
					hr = VerifyOrSignManifestBegin(pinstall, NULL);
					if (FAILED(hr))
					{
						// Can't open the manifest, we will re-install
						goto NoCache;
					}

					// Got a manifest, see if it's valid
					pinstall->dlphase = dlphaseVerifyInstall;					
					goto Exit;
				}
			}
			
			// This is a recoverable error: we will just assume there
			// is no cache file
			goto NoCache;
		}
	}

CreateCache:

	// See if we are going to resume
	if ((pinstall->dwFlags & XONLINECONTENT_FLAG_RESUME) == 0)
	{
		// If we get here, we are doing a straight download. Now 
		// we build the proper request from the CATREF information
		pxrlasync->wsabuf.len = pxrlasync->cbBuffer;
		pxrlasync->wsabuf.buf = (char *)pxrlasync->pBuffer;
		liFileSize.QuadPart = 0;
		
	    hr = XRL_BuildGetRequestWithResume(
						pxrlasync->serviceInfo.dwServiceID,
	    				pinstall->szResourcePath,
	    				pxrlasync->serviceInfo.serviceIP.s_addr,
	    				pxrlasync->wsabuf.buf, 
	    				&(pxrlasync->wsabuf.len), 
	    				NULL, 0, NULL, liFileSize);
	    if (FAILED(hr))
	        goto Error;

		// If the cache file is open (i.e. retry after detecting corrupt
		// cache file), then close it first.
		if (pfileio->hFile != INVALID_HANDLE_VALUE)
		{
			CloseHandle(pfileio->hFile);
			pfileio->hFile = INVALID_HANDLE_VALUE;
		}

		// Build the target path to wipe
		cbPath = XONLINECONTENT_MAX_PATH;
		hr = BuildTargetDirectory(
					pinstall->dwTitleId, 
					pinstall->OfferingId,
					pinstall->dwBitFlags,		
					pinstall->dirtypeTarget,
					&cbPath, pinstall->szPath);
		if (FAILED(hr))
			goto Exit;
		
		// Wipe the target location
		hr = RemoveDirectoryInitializeContext(
					pinstall->szPath, cbPath, 
					XONLINECONTENT_MAX_PATH, 
					(char *)pinstall->rgbDecompress, 0,
					pinstall->xontask.hEventWorkAvailable,
					&pinstall->dirops);
		if (FAILED(hr))
			goto Exit;

		pinstall->dlphase = dlphaseWipeTarget;
	}

	// Make sure something is happening
	Assert(pinstall->dlphase != dlphaseCheckCache);

Exit:
	return(hr);

Error:
	TraceSz2(Verbose, "contdlCheckCache non-recoverable error (%h, %u)", 
             hr, GetLastError());
	goto Exit;

NoCache:	
	TraceSz2(Verbose, "contdlCheckCache recoverable error (%h, %u). Creating cache", 
             hr, GetLastError());
	hr = S_OK;
	pinstall->dwFlags &= (~XONLINECONTENT_FLAG_RESUME);
	goto CreateCache;
}	

//
// Function to read the cache from disk
//
HRESULT CXo::contdlReadCache(
			PXONLINETASK_CONTENT_DOWNLOAD	pcontdl
			)
{
	HRESULT					hr = S_OK;
	PXRL_ASYNC				pxrlasync = NULL;
	PXONLINECONTENT_INSTALL	pinstall = NULL;
	PXONLINETASK_FILEIO		pfileio = NULL;
	PBYTE					pbBuffer = NULL;
	LARGE_INTEGER			liResumeFrom;
	FILETIME				ftLastWritten;
	DWORD					dwCheckpoint;
	DWORD					dwRead;

	Assert(pcontdl != NULL);

	pinstall = &pcontdl->install;
	pxrlasync = &pcontdl->xrlasync;
	pfileio = &pinstall->fileioData;

	// Wait for the read to complete
	hr = XOnlineTaskContinue((XONLINETASK_HANDLE)pfileio);
	if (XONLINETASK_STATUS_AVAILABLE(hr))
	{
		// Read had returned results, see if it succeeded
		if (FAILED(hr))
			goto Error;

		// We got the entire cache, process it.
		hr = GetReadWriteFileResults(
					(XONLINETASK_HANDLE)pfileio, &dwRead, &pbBuffer);
		if (FAILED(hr))
			goto Error;

		// Make sure we got what we expected
		Assert(pinstall->dwExpected <= dwRead);

		// Make sure the buffers are still lined up
        Assert(pbBuffer == pinstall->rgbDecompress);

		// Call function to process fixed header
		hr = ProcessFixedHeader(pcontdl, pbBuffer);
		if (FAILED(hr))
			goto Error;

		// Call another function to process the CFFOLDER entries
		// this also fills in pinstall->dwExpected
		hr = ProcessCffolderEntries(pcontdl, 
					pbBuffer + XONLINECONTENT_COMPLETE_HEADER_SIZE);
		if (FAILED(hr))
			goto Error;

		// Basic sanity check on our checkpoint
		dwCheckpoint = *(LPDWORD)(pbBuffer + XONLINECONTENT_CHECKPOINT);
		if ((dwCheckpoint >= pinstall->header.cfheader.cFolders) ||
			(dwCheckpoint >= XONLINECONTENT_MAX_CFFOLDER))
		{
			AssertSz(FALSE, "contdlReadCache: Invalid checkpoint!");
			hr = HRESULT_FROM_WIN32(ERROR_INVALID_DATA);
			goto Error;
		}

		// Set up system to start from specified folder
		hr = SetupInstallFolder(pinstall, dwCheckpoint);
		if (FAILED(hr))
			goto Error;

		// Get the file time
		if (!GetFileTime(pfileio->hFile, NULL, NULL, &ftLastWritten))
		{
			hr = HRESULT_FROM_WIN32(GetLastError());
			goto Error;
		}

		// Now we have all the header data checked and verified, 
		// the cache file written and closed, we can set
		// up our CFFILE cache. It will inherit the file handle
		// from pinstall->fileioData.hFile.
		// We will first verify the CFFILE entries before we
		// use them to prevent using invalid data. We will use
		// the decompress buffer for this because it is much
		// larger and still available at this point.

		// Figure out the size of CFFILE entries
		dwRead = pinstall->dwExpected - 
						pinstall->header.cfheader.coffFiles;
		hr = CffileCacheInitialize(
					pfileio->hFile,
					pinstall->header.cfheader.coffFiles,
					pinstall->header.cfheader.cFiles,
					pinstall->rgbSymmetricKey, 
					XONLINE_KEY_LENGTH,
					dwRead,
					pinstall->rgbDecompress,
					XONLINECONTENT_LZX_OUTPUT_SIZE,
					&(pinstall->header.digestFiles),
					pinstall->xontask.hEventWorkAvailable, 
					&(pinstall->cffilecache));
		if (FAILED(hr))
			goto Error;

		// The CFFILE cache had taken over the file handle, 
		// mark this one as invalid.
		pinstall->fileioData.hFile = INVALID_HANDLE_VALUE;

		// OK, rebuild the GET request
		pxrlasync->wsabuf.len = pxrlasync->cbBuffer;
		pxrlasync->wsabuf.buf = (char *)pxrlasync->pBuffer;
		
		liResumeFrom.QuadPart = 
				pinstall->rgcffolderhmac[dwCheckpoint].cffolder.coffCabStart;
	    hr = XRL_BuildGetRequestWithResume(
	    				pxrlasync->serviceInfo.dwServiceID,
	    				pinstall->szResourcePath,
	    				pxrlasync->serviceInfo.serviceIP.s_addr,
	    				pxrlasync->wsabuf.buf, 
	    				&(pxrlasync->wsabuf.len), 
	    				NULL, 0,
	    				&ftLastWritten, 
	    				liResumeFrom);
	    if (FAILED(hr))
	        goto Error;
		
		// Next phase is to set up CFFILE cache to the correct position
		pinstall->dlphase = dlphaseVerifyCffiles;
	}

Exit:
	return(hr);

Error:
	// Now, if for any reason we fail during the cache preparation 
	// phase, we will just go back and download everything from scratch
	ResetAfterUnsuccessfulResume(pinstall);

	TraceSz1(Verbose, "Failed to read cache (%h), downloading from scratch", hr);
	
	hr = S_OK;
	goto Exit;
}

//
// Function to setup CFFILE cache
//
HRESULT CXo::contdlSetupCache(
			PXONLINETASK_CONTENT_DOWNLOAD	pcontdl
			)
{
	HRESULT					hr = S_OK;
	PXONLINECONTENT_INSTALL	pinstall = NULL;
	DWORD					dwIndex;

	Assert(pcontdl != NULL);

	pinstall = &pcontdl->install;

	// We try to get the next CFFILE entry from the cache.
	hr = CffileCacheGetNextEntry(
				&pinstall->cffilecache,
				&dwIndex,
				&pinstall->pcffile,
				&pinstall->szFileName,
				&pinstall->cbFileName);
	if (FAILED(hr))
		goto Error;

	// Now see if we got it right away ...
	if (hr == S_CFFILECACHE_IO_PENDING)
	{
		// Nope, the next entry happens to be on disk and we have
		// to load it up. There's nothing we can do now, so might
		// as well return to the dispatcher. 
		goto Exit;
	}

	if (hr == S_CFFILECACHE_NO_MORE_ITEMS)
	{
		AssertSz(FALSE, "contdlSetupCache: Invalid CFFILE data!");
		hr = HRESULT_FROM_WIN32(ERROR_INVALID_DATA);
		goto Error;
	}
	
	if (pinstall->pcffile->iFolder == pinstall->dwCurrentFolder)
	{
		// Set up to install this file
		pinstall->dwExpected = pinstall->pcffile->cbFile;
		pinstall->dwWritten = 0;
		TraceSz3(Verbose, "[%u] Installing %s, %u bytes", 
                 dwIndex, pinstall->szFileName, pinstall->dwExpected);

		// Cache the next phase, and see if we have custom tasks
		pinstall->dlphaseNext = dlphaseCreateTarget;
		pinstall->dlphase = dlphasePreCustom;
	}

Exit:
	return(S_OK);

Error:
	// Now, if for any reason we fail during the cache preparation 
	// phase, we will just go back and download everything from scratch
	ResetAfterUnsuccessfulResume(pinstall);

	TraceSz1(Verbose, "Failed to read cache (%h), downloading from scratch", hr);
	goto Exit;
}

//
// Function to perform any pre-install custom tasks
//
HRESULT CXo::contdlPreCustom(
			PXONLINETASK_CONTENT_DOWNLOAD	pcontdl
			)
{
	HRESULT					hr = S_OK;
	PXONLINECONTENT_INSTALL	pinstall;

	Assert(pcontdl != NULL);

	pinstall = &pcontdl->install;

	// Proceed if we don't have any custom task
	if (pinstall->hTaskPreCustom != NULL)
	{
		// We have a task, pump it until it is done
		hr = XOnlineTaskContinue(pinstall->hTaskPreCustom);
		if (XONLINETASK_STATUS_AVAILABLE(hr))
		{
			// See if it succeeded
			if (FAILED(hr))
				goto Cleanup;

			goto Done;
		}
	}
	else
		goto Done;

Cleanup:
	return(hr);

Done:
	// Restore the cached next state
	Assert((pinstall->dlphaseNext == dlphaseRecvHeader) ||
			(pinstall->dlphaseNext == dlphaseCreateTarget) ||
			(pinstall->dlphaseNext == dlphasePostCustom));
	pinstall->dlphase = pinstall->dlphaseNext;

	// We are done with pre-download
	if (pinstall->dlphaseNext != dlphasePostCustom)
		hr = XONLINETASK_S_SUCCESS;
	else
	{
		// Return a specific code to communicate to skip the
		// actual download and go to post processing
		hr = XONLINE_S_XRL_EXTENSION_SKIP_TO_POST;
	}
	
	goto Cleanup;
}

//
// Function to see if the package is already installed
//
HRESULT CXo::contdlVerifyInstall(
			PXONLINETASK_CONTENT_DOWNLOAD	pcontdl
			)
{
	HRESULT					hr = S_OK;
	PXONLINECONTENT_INSTALL	pinstall;

	Assert(pcontdl != NULL);

	pinstall = &pcontdl->install;
	
	// We have a task, pump it until it is done
	hr = XOnlineTaskContinue((XONLINETASK_HANDLE)&(pinstall->verify));
	if (XONLINETASK_STATUS_AVAILABLE(hr))
	{
		// Close the manifest file
		CloseHandle(pinstall->verify.fileio.hFile);
		pinstall->verify.fileio.hFile = INVALID_HANDLE_VALUE;
	
		// See if it succeeded
		if (FAILED(hr))
		{
			// The manifest was there, but somehow it's not valid
			// This time, we will create the cache and download from scratch
			pinstall->dwFlags |= XONLINECONTENT_FLAG_NOT_INSTALLED;
			pinstall->dlphase = dlphaseCheckCache;
		}
		else
		{
			// Make sure success is returned
			Assert(hr == XONLINETASK_S_SUCCESS);

			// We are trying to install a package that has already
			// been installed. We are done with the download and
			// install part. Now we jump to the post installation
			// phase to complete all unfinished business
			pinstall->dlphaseNext = dlphasePostCustom;
			pinstall->dlphase = dlphasePreCustom;

			hr = S_OK;
		}		
	}

	return(hr);
}

//
// Function to verify Hmac for the CFFILE entries before
// we actually use it
//
HRESULT CXo::contdlVerifyCffiles(
			PXONLINETASK_CONTENT_DOWNLOAD	pcontdl
			)
{
	HRESULT					hr = S_OK;
	PXONLINECONTENT_INSTALL	pinstall;
	DWORD					cbLength;

	Assert(pcontdl != NULL);

	pinstall = &pcontdl->install;

	// Call the API
	hr = CffileCacheVerify(&pinstall->cffilecache);
	if (SUCCEEDED(hr))
	{
		if (hr == S_OK)
		{
			// Verification OK, now we reset the CFFILE cache
			// for enumeration
			cbLength = pinstall->dwExpected - 
							pinstall->header.cfheader.coffFiles;
			hr = CffileCacheInitialize(
						pinstall->cffilecache.hFile,
						pinstall->header.cfheader.coffFiles,
						pinstall->header.cfheader.cFiles,
						pinstall->rgbSymmetricKey, 
						XONLINE_KEY_LENGTH,
						cbLength,
						NULL,	// Use default buffer
						0,		// Use default buffer
						&(pinstall->header.digestFiles),
						pinstall->xontask.hEventWorkAvailable, 
						&(pinstall->cffilecache));
			if (FAILED(hr))
				goto Exit;

			// Figure out where to go next
			if (pinstall->dwFlags & XONLINECONTENT_FLAG_RESUME)
			{
				// We are resuming, need to set up the CFFILE cache
				pinstall->dlphase = dlphaseSetupCache;
			}
			else
			{
				// Go straight to installing files
				pinstall->dlphase = dlphaseInstallFiles;
			}
		}
		else if (hr != S_CFFILECACHE_IO_PENDING)
		{
			AssertSz(FALSE, "contdlVerifyCffiles: Unexpected return code");
		}

		hr = S_OK;
	}

Exit:
	return(hr);
}

//
// Function to write checkpoint
//
HRESULT CXo::contdlCheckpoint(
			PXONLINETASK_CONTENT_DOWNLOAD	pcontdl
			)
{
	HRESULT					hr = S_OK;
	PXONLINECONTENT_INSTALL	pinstall;
	PXONLINETASK_FILEIO		pfileio;
	XONLINETASK_HANDLE		hfileio;
	ULARGE_INTEGER			uliOffset;
	DWORD					dwCheckpoint;
	DWORD					dwSize;
	PBYTE					pbBuffer;

	Assert(pcontdl != NULL);

	pinstall = &pcontdl->install;

	// We steal the CFFILE cache context for our file I/O
	pfileio = &pinstall->cffilecache.fileio;
	hfileio = (XONLINETASK_HANDLE)pfileio;

	// In addition, we will steal the path buffer to use
	pbBuffer = (PBYTE)pinstall->szPath;

	// This is  essentially a read-modify-write operation
	if (pinstall->dwFlags & XONLINECONTENT_FLAG_READ_PENDING)
	{
		// A read is pending, see if the read has completed
		hr = XOnlineTaskContinue(hfileio);
		if (XONLINETASK_STATUS_AVAILABLE(hr))
		{
			// Turn off the flag immediately
			pinstall->dwFlags &= (~XONLINECONTENT_FLAG_READ_PENDING);
		
			// Read had returned results, see if it succeeded
			if (FAILED(hr))
				goto Error;

			hr = GetReadWriteFileResults(
						(XONLINETASK_HANDLE)pfileio, &dwSize, &pbBuffer);
			if (FAILED(hr))
				goto Error;

			if (dwSize != XBOX_HD_SECTOR_SIZE)
			{
				hr = HRESULT_FROM_WIN32(ERROR_INVALID_DATA);
				goto Error;
			}

			// Set the checkpoint
			*(PDWORD)(pbBuffer + XONLINECONTENT_CHECKPOINT) = pinstall->dwCurrentFolder;

			// Write out the checkpoint
			uliOffset.QuadPart = 0;
			ReadWriteFileInitializeContext(pfileio->hFile,
						pbBuffer,
						XBOX_HD_SECTOR_SIZE,
						uliOffset,
						pinstall->xontask.hEventWorkAvailable, 
						pfileio);

			hr = WriteFileInternal(pfileio);
			if (FAILED(hr))
				goto Error;

			// Set the flag to indicate that we are pending write complete
			pinstall->dwFlags |= XONLINECONTENT_FLAG_WRITE_PENDING;		
		}
	}
	else if (pinstall->dwFlags & XONLINECONTENT_FLAG_WRITE_PENDING)
	{
		// A write is pending, see if the write has completed
		hr = XOnlineTaskContinue(hfileio);
		if (XONLINETASK_STATUS_AVAILABLE(hr))
		{
			// Turn off the flag immediately
			pinstall->dwFlags &= (~XONLINECONTENT_FLAG_WRITE_PENDING);
		
			// Read had returned results, see if it succeeded
			if (FAILED(hr))
				goto Error;

			hr = GetReadWriteFileResults(
						(XONLINETASK_HANDLE)pfileio, &dwSize, &pbBuffer);
			if (FAILED(hr))
				goto Error;

            Assert(dwSize == XBOX_HD_SECTOR_SIZE);

			// Okay, go back to where we were ....
			if (pinstall->dwFlags & XONLINECONTENT_FLAG_CONTINUE_FILE)
			{
				pinstall->dlphase = dlphaseInstallFiles;
				pinstall->dwFlags &= (~XONLINECONTENT_FLAG_CONTINUE_FILE);
			}
			else
				pinstall->dlphase = dlphaseCreateTarget;
		}
	}
	else
	{
		// We need to read in the first sector that contains the 
		// checkpoint, given the fact that the cache size is always
		// rounded up to the next sector, we know the cache must
		// be at least 1 sector in size.
		uliOffset.QuadPart = 0;
		ReadWriteFileInitializeContext(pfileio->hFile,
					pbBuffer,
					XBOX_HD_SECTOR_SIZE,
					uliOffset,
					pinstall->xontask.hEventWorkAvailable, 
					pfileio);

		hr = ReadFileInternal(pfileio);
		if (FAILED(hr))
			goto Error;

		// Set the flag to indicate that we are pending read complete
		pinstall->dwFlags |= XONLINECONTENT_FLAG_READ_PENDING;		
	}
	
	return(S_OK);

Error:
	return(hr);
}

//
// Function to write DRM
//
HRESULT CXo::contdlSignDrm(
			PXONLINETASK_CONTENT_DOWNLOAD	pcontdl
			)
{
	HRESULT					hr = S_OK;
	PXONLINECONTENT_INSTALL	pinstall;
	PXONLINETASK_FILEIO		pfileio;
	XONLINETASK_HANDLE		hverify;
	LARGE_INTEGER			liSize;
	DWORD					cbPath;
	DGSTFILE_IDENTIFIER		dfi;

	Assert(pcontdl != NULL);

	pinstall = &pcontdl->install;
	pfileio = &pinstall->verify.fileio;

	// If the manifest file is not already open, open it and 
	// do some initialization
	if (!pfileio->hFile || (pfileio->hFile == INVALID_HANDLE_VALUE))
	{
		// Verify and sign the manifest
		hr = VerifyOrSignManifestBegin(pinstall, pinstall->rgbSymmetricKey);
		if (FAILED(hr))
			goto Error;

		goto Exit;
	}

	// Pump the verification task
	hverify = (XONLINETASK_HANDLE)&pinstall->verify;
	hr = XOnlineTaskContinue(hverify);
	if (XONLINETASK_STATUS_AVAILABLE(hr))
	{
		// Close the manifest file
		CloseHandle(pfileio->hFile);
		pfileio->hFile = INVALID_HANDLE_VALUE;

		// See if it succeeded
		if (FAILED(hr))
			goto Error;

		// Succeeded. Go to remove the cache file
		Assert(hr == XONLINETASK_S_SUCCESS);
		hr = S_OK;
		pinstall->dlphase = dlphaseDeleteCache;
	}

Exit:
Error:
	return(hr);
}

//
// Function to delete the cache file
//
HRESULT CXo::contdlDeleteCache(
			PXONLINETASK_CONTENT_DOWNLOAD	pcontdl
			)
{
	HRESULT					hr = S_OK;
	PXONLINECONTENT_INSTALL	pinstall;
	DWORD					cbPath;

	Assert(pcontdl != NULL);

	pinstall = &pcontdl->install;

	// Make sure the cache is closed
	CffileCacheDeinitialize(&pinstall->cffilecache);

	// Synchronously delete the cache file
	cbPath = XONLINECONTENT_MAX_PATH;
	hr = BuildTargetPath(
				pinstall->dwTitleId, 
				pinstall->OfferingId,
				pinstall->dwBitFlags,
				XONLINECONTENT_CACHE_FILE_NAME,
				pinstall->dirtypeTarget,
				&cbPath,
				pinstall->szPath);
	if (FAILED(hr))
		return(hr);

	if (!DeleteFile(pinstall->szPath))
	{
		hr = HRESULT_FROM_WIN32(GetLastError());
		return(hr);
	}

	// Move on to next phase
	pinstall->dlphase = dlphasePostCustom;

	return(S_OK);
}

//
// Function to perform any post-install custom tasks
//
HRESULT CXo::contdlPostCustom(
			PXONLINETASK_CONTENT_DOWNLOAD	pcontdl
			)
{
	HRESULT					hr = S_OK;
	PXONLINECONTENT_INSTALL	pinstall;

	Assert(pcontdl != NULL);

	pinstall = &pcontdl->install;

	// Proceed if we don't have any custom task
	if (pinstall->hTaskPostCustom != NULL)
	{
		// We have a task, pump it until it is done
		hr = XOnlineTaskContinue(pinstall->hTaskPostCustom);
		if (XONLINETASK_STATUS_AVAILABLE(hr))
		{
			// See if it succeeded
			if (FAILED(hr))
				goto Cleanup;

			hr = S_OK;
			goto Done;
		}
	}
	else
		goto Done;

Cleanup:
	return(hr);

Done:
	// We are completely done at this point!
	pinstall->dlphase = dlphaseDone;
	goto Cleanup;
}

// =================================================================

//
// Task handler type
//
typedef HRESULT (CXo::*PFNCONTDL_HANDLER)(PXONLINETASK_CONTENT_DOWNLOAD);

//
// Define the handler vector
//
const PFNCONTDL_HANDLER CXo::s_rgpfncontdlHandlers[] =
{
	CXo::contdlCatref,
	CXo::contdlCheckCache,
	CXo::contdlReadCache,
	CXo::contdlWipeTarget,
	CXo::contdlCreateCache,
	CXo::contdlSetupCache,
	CXo::contdlVerifyInstall,
	CXo::contdlPreCustom,
	CXo::contdlRecvHeader,
	CXo::contdlInstallFiles,
	CXo::contdlCreateTarget,
	CXo::contdlWriteTarget,
	CXo::contdlCheckpoint,
	CXo::contdlCreateDirectory,
	CXo::contdlVerifyCffiles,
	CXo::contdlSignDrm,
	CXo::contdlDeleteCache,
	CXo::contdlPostCustom,
	0
};

//
// Implement the do work function for ContentInstall
//
// This is shared among xrldlextPreConnect, xrldlextProcessData,
// and xrldlextPostDownload
//

HRESULT CXo::contdlContinue(
			XONLINETASK_HANDLE	hTask
			)
{
	HRESULT							hr = S_OK;
	PXONLINETASK_CONTENT_DOWNLOAD	pcontdl = NULL;
	PXONLINECONTENT_INSTALL			pinstall = NULL;
	PXRL_ASYNC						pxrlasync = NULL;
	XONLINECONTENT_DLPHASE			dlphase;

#if DBG
	LARGE_INTEGER					liTimerTotal;
	LARGE_INTEGER					liTimerStart;
	LARGE_INTEGER					liTimerEnd;

	// Track service count
    m_liServiceCount.QuadPart += 1;

	// Start the total timer
	QueryPerformanceCounter(&liTimerTotal);
#endif

	Assert(hTask != NULL);

	// hTask points to the xontask field of XRL_ASYNC_EXTENDED,
	// find the containing pxrlasyncext structure
	pcontdl = CONTAINING_RECORD(hTask, XONLINETASK_CONTENT_DOWNLOAD, install);
	pinstall = &pcontdl->install;
	pxrlasync = &pcontdl->xrlasync;

#ifdef CHK_CORRUPT
	CheckCorruption(pinstall);
#endif

#if DBG
	// Start the loop timer
	QueryPerformanceCounter(&liTimerStart);
#endif

	dlphase = pinstall->dlphase;

	// Figure out what phase we are in ...
	if (pinstall->dlphase < dlphaseDone)
	{
		hr = (this->*(s_rgpfncontdlHandlers[dlphase]))(pcontdl);

#if DBG
		if (FAILED(hr))
		{
			DWORD	tagOld = t_Verbose;
			t_Verbose = TAG_ENABLE;
			TraceSz2(Verbose, "%s, %08x", s_rgszPhases[dlphase], hr);
			t_Verbose = tagOld;
		}
#endif		
	}
	else if (pinstall->dlphase != dlphaseDone)
	{
		// This is a bad state. We force it to end so we don't get stuck
		// here forever.
		AssertSz(FALSE, "contdlContinue: invalid phase!");
		hr = E_FAIL;
	}

#if DBG	
	// Stop timer
	QueryPerformanceCounter(&liTimerEnd);

	m_liTotalTime.QuadPart += (liTimerEnd.QuadPart - liTimerStart.QuadPart);
	m_liAverageTime.QuadPart = m_liTotalTime.QuadPart / m_liServiceCount.QuadPart;

#endif	

	// Bail if failed.
	if (FAILED(hr))
	{
#if DBG		
		pinstall->dlphaseError = pinstall->dlphase;
#endif		
		contdlCleanup(pcontdl);
		pxrlasync->hrFinal = hr;
		pinstall->dlphase = dlphaseDone;
	}

	// When the next state is the final state, we set the flag so we don't
	// continue.
	if (pinstall->dlphase == dlphaseDone)
	{
		// Indicate that we are done and return the final code
		hr = pxrlasync->hrFinal;
		XONLINETASK_SET_COMPLETION_STATUS(hr);

#if DBG
		TraceSz1(Verbose, "%I64u services elapsed", m_liServiceCount.QuadPart);
		TraceSz1(Verbose, "Total time: %I64u ticks", m_liTotalTime.QuadPart);
		TraceSz1(Verbose, "Average service time: %I64u ticks", m_liAverageTime.QuadPart);
		m_liServiceCount.QuadPart = 0;
		m_liTotalTime.QuadPart = 0;
		m_liAverageTime.QuadPart = 0;
#endif		
	}

#ifdef CHK_CORRUPT
	CheckCorruption(pinstall);
#endif

	// Detect state changes so we will not timeout
	if (dlphase != pinstall->dlphase)
		pxrlasync->dwLastStateChange = GetTickCount();

	return(hr);
}				

//
// Function to kick off downloading and installing a
// content package. This is the internal/Dash version because it
// allows a Title ID to be specified.
//
HRESULT CXo::ContentInstallInitializeContext(
			PXONLINETASK_CONTENT_DOWNLOAD 	pcontdl,
			DWORD							dwTitleId,
			XONLINEOFFERING_ID				OfferingId,
			XONLINEDIR_TYPE					dirtypeTarget,
			DWORD							dwFlags,
			DWORD							dwTimeout,
			XONLINETASK_HANDLE				hTaskPreCustom,
			XONLINETASK_HANDLE				hTaskPostCustom,
			HANDLE							hWorkEvent
			)
{
	HRESULT							hr = S_OK;
	PXONLINECONTENT_INSTALL			pinstall = NULL;
	PXRL_ASYNC						pxrlasync = NULL;
	PXRL_DOWNLOAD_EXTENSION			pxrldlext = NULL;

	Assert(NULL != pcontdl);

	ZeroMemory(pcontdl, sizeof(XONLINETASK_CONTENT_DOWNLOAD));
	pinstall = &pcontdl->install;
	pxrlasync = &pcontdl->xrlasync;
	pxrldlext = &(pxrlasync->xrlext.dl);

	// Fill in the blanks
	TaskInitializeContext(&pxrlasync->xontask);
	pxrlasync->xontask.hEventWorkAvailable = hWorkEvent;
	pxrlasync->pBuffer = pinstall->rgbBuffer;
	pxrlasync->cbBuffer = XONLINECONTENT_DL_BUFFER_SIZE;
	pxrlasync->hrFinal = S_OK;
	pxrlasync->uliTotalWritten.QuadPart = 0;
	pxrlasync->uliTotalReceived.QuadPart = 0;
	pxrlasync->uliContentLength.QuadPart = 0;
	pxrlasync->dwTimeoutMs = dwTimeout;

	// The socket API requires that we have an event handle in order
	// for the async I/O to happen. If the caller has not provided us
	// with an event, we have to create it here
	hr = XRL_CreateWorkEventIfNecessary(pxrlasync, &hWorkEvent);
	if (FAILED(hr))
		goto Error;

	// Initialize our subtask
	TaskInitializeContext(&pinstall->xontask);
	pinstall->xontask.pfnContinue = contdlContinue;
	pinstall->xontask.hEventWorkAvailable = hWorkEvent;
	pinstall->dlphase = dlphaseCatref;
	pinstall->dlphaseNext = dlphaseDone;
	pinstall->dwTitleId = dwTitleId;
	pinstall->OfferingId = OfferingId;
	pinstall->dirtypeTarget = dirtypeTarget;
	pinstall->dwPackageSize = 0;
	pinstall->dwInstallSize = 0;
	pinstall->dwLastXrl = 0;
	pinstall->dwFlags = dwFlags;
	pinstall->hTaskPreCustom = hTaskPreCustom;
	pinstall->hTaskPostCustom = hTaskPostCustom;

	// We need to do some special work if we are installing offline
	if (dwFlags & XONLINECONTENT_FLAG_OFFLINE_INSTALL)
	{
		// Skip over the cetref stuff
		pinstall->dlphase = dlphaseCheckCache;
	}
	
	// Set up the extension block
	pxrldlext->pfnDoneHeaders = contdlDoneHeaders;
	pxrldlext->pfnProcessData = contdlProcessData;
	pxrldlext->pfnCompleted = contdlCompleted;
	pxrldlext->pfnInitializeTask = NULL;
	pxrldlext->pfnCleanup = contdlCleanupHandler;
	pxrldlext->hTaskPreConnect = (XONLINETASK_HANDLE)pinstall;
	pxrldlext->hTaskProcessData = (XONLINETASK_HANDLE)pinstall;
	pxrldlext->hTaskPostDownload = (XONLINETASK_HANDLE)pinstall;

	// Initialize the decompressor
	LzxDecodeInit(pinstall->rgbLzxWindow,
				XONLINECONTENT_LZX_WINDOW_SIZE,
				&(pinstall->lzx));

Cleanup:	
	return(hr);

Error:

	// Failed setup, make sure we clean up everything
	if (pcontdl)
		contdlCleanup(pcontdl);
	goto Cleanup;
}

//
// Function to kick off a content download and install. This 
// is the public version that installs for the current title.
//
HRESULT CXo::XOnlineContentInstall(
			XONLINEOFFERING_ID	OfferingId,
			HANDLE				hWorkEvent,
			XONLINETASK_HANDLE	*phTask
			)
{
	HRESULT							hr = S_OK;
	LARGE_INTEGER					liTemp;
	LPCSTR							szDummy = "";
	PXONLINETASK_CONTENT_DOWNLOAD	pcontdl = NULL;

	XoEnter("XOnlineContentInstall");

	// We are responsible for allocating the context and any other
	// buffers. These buffers immediately follow the context structure
#ifndef CHK_CORRUPT	
	pcontdl = (PXONLINETASK_CONTENT_DOWNLOAD)SysAllocZ(sizeof(XONLINETASK_CONTENT_DOWNLOAD), PTAG_XONLINETASK_CONTENT_DOWNLOAD);
#else
	pcontdl = (PXONLINETASK_CONTENT_DOWNLOAD)SysAllocZ(sizeof(XONLINETASK_CONTENT_DOWNLOAD) + CHK_CORRUPT_SIZE, PTAG_XONLINETASK_CONTENT_DOWNLOAD);
#endif
	if (!pcontdl)
	{
		hr = E_OUTOFMEMORY;
		goto Error;
	}

	hr = ContentInstallInitializeContext(pcontdl, 
				m_dwTitleId, OfferingId, dirtypeTitleContent, 0,
				XONLINECONTENT_INSTALL_TIMEOUT, NULL, NULL,
				hWorkEvent);
	if (FAILED(hr))
		goto Error;

	// First, kick off a referral request
	hr = InitiateReferralRequest(OfferingId, pcontdl);
	if (FAILED(hr))
		goto Error;

	// Kick off the generic download. At this point, we don't
	// care about the timestamp or resume point, because we 
	// will have to come up with that later.
	//
	// Note: The following flag makes the download insecure
	pcontdl->xrlasync.dwFlags |= XRL_CONNECTION_INSECURE;
	liTemp.QuadPart = 0;
	hr = Download(szDummy, NULL, 0, NULL, liTemp, &pcontdl->xrlasync);
	if (FAILED(hr))
		goto Error;

	*phTask = (XONLINETASK_HANDLE)pcontdl;

Exit:
	return(XoLeave(hr));

Error:
	if (pcontdl != NULL)
		SysFree(pcontdl);
	goto Exit;
}

//
// Function to kick off a content download and install from a 
// package on the DVD to the specified title (most likely the
// title initiating this is not the recipient title of the content)
//
// The install location is:
// c:\TDATA\<dwTitleID>\$c\<OfferingId>.<dwBitFlags>\
//
HRESULT CXo::XOnlineContentInstallFromDVD(
			PSTR				szResourcePath,
			DWORD				dwTitleId,
			XONLINEOFFERING_ID	OfferingId,
			DWORD				dwBitFlags,
			PBYTE				pbSymmetricKey,
			DWORD				cbSymmetricKey,
			PBYTE				pbPublicKey,
			DWORD				cbPublicKey,
			HANDLE				hWorkEvent,
			XONLINETASK_HANDLE	*phTask
			)
{
	HRESULT							hr = S_OK;
	LARGE_INTEGER					liTemp;
	PXONLINETASK_CONTENT_DOWNLOAD	pcontdl = NULL;

	XoEnter("InstallFromDVD");

	Assert(szResourcePath);
	Assert(phTask);
	Assert(pbSymmetricKey);
	Assert(cbSymmetricKey == XONLINE_KEY_LENGTH);
	Assert(pbPublicKey);
	Assert(cbPublicKey >= XONLINECONTENT_PK_SIGNATURE_SIZE);
	Assert(cbPublicKey <= XONLINECONTENT_MAX_PUBLIC_KEY_SIZE);

	// We are responsible for allocating the context and any other
	// buffers. These buffers immediately follow the context structure
#ifndef CHK_CORRUPT	
	pcontdl = (PXONLINETASK_CONTENT_DOWNLOAD)SysAllocZ(sizeof(XONLINETASK_CONTENT_DOWNLOAD), PTAG_XONLINETASK_CONTENT_DOWNLOAD);
#else
	pcontdl = (PXONLINETASK_CONTENT_DOWNLOAD)SysAllocZ(sizeof(XONLINETASK_CONTENT_DOWNLOAD) + CHK_CORRUPT_SIZE, PTAG_XONLINETASK_CONTENT_DOWNLOAD);
#endif
	if (!pcontdl)
	{
		hr = E_OUTOFMEMORY;
		goto Error;
	}

	// Need to mount the target title TData to private drive
	hr = XoUpdateMountTitleTDataToPrivateDrive(dwTitleId);
	if (FAILED(hr))
		goto Error;

	hr = ContentInstallInitializeContext(pcontdl, 
				dwTitleId, OfferingId, dirtypeTitleContentOnF,
				XONLINECONTENT_FLAG_OFFLINE_INSTALL,
				XONLINECONTENT_INSTALL_TIMEOUT, NULL, NULL,
				hWorkEvent);
	if (FAILED(hr))
		goto Error;

	// Set the bitflags
	pcontdl->install.dwBitFlags = dwBitFlags;

	// Set up the cryptographic keys
	memcpy(pcontdl->install.rgbSymmetricKey, pbSymmetricKey, XONLINE_KEY_LENGTH);
	memcpy(pcontdl->install.rgbPublicKey, pbPublicKey, cbPublicKey);

	// Do a "download" from file
	liTemp.QuadPart = 0;
	hr = DownloadFromDVD(szResourcePath, NULL, liTemp, &pcontdl->xrlasync);
	if (FAILED(hr))
		goto Error;

	*phTask = (XONLINETASK_HANDLE)pcontdl;

Exit:
	return(XoLeave(hr));

Error:
	XoUpdateUnmountPrivateDrive();

	if (pcontdl != NULL)
		SysFree(pcontdl);
	goto Exit;
}

//
// Function to get the progress of an active content installation
//
HRESULT
CXo::XOnlineContentInstallGetProgress(
			XONLINETASK_HANDLE hTask,
			DWORD *pdwPercentDone,
			ULONGLONG *pqwNumerator,
			ULONGLONG *pqwDenominator
			)
{
	HRESULT	hr = S_OK;
	ULARGE_INTEGER	uliNumerator, uliDenominator;

    XoEnter("XOnlineContentInstallGetProgress");
    XoCheck(hTask != NULL);

	PXONLINETASK_CONTENT_DOWNLOAD pcontdl = (PXONLINETASK_CONTENT_DOWNLOAD)hTask;

	hr = XOnlineDownloadGetProgress((XONLINETASK_HANDLE)&(pcontdl->xrlasync),
                                              pdwPercentDone, &uliNumerator, &uliDenominator);
	if (pqwNumerator)
		*pqwNumerator = uliNumerator.QuadPart;
	if (pqwDenominator)
		*pqwDenominator = uliDenominator.QuadPart;
	return(XoLeave(hr));
}

HRESULT CXo::XOnlineContentGetRootDirectory(
			XONLINEOFFERING_ID	OfferingId,
			BOOL				fUserData,
			DWORD				*pcbRootDirectory,
			CHAR				*szRootDirectory
			)
{
    XoEnter("XOnlineContentGetRootDirectory");
    XoCheck(pcbRootDirectory != NULL);
    XoCheck(szRootDirectory != NULL);

	HRESULT				hr = S_OK;
	HANDLE				hFind = INVALID_HANDLE_VALUE;
	WIN32_FIND_DATA		wfd;
	DWORD				cbSize;
    CHAR				szTemplate[64];

	// Find the first directory that matches the template
	sprintf(szTemplate, XONLINECONTENT_OPEN_TEMPLATE,
				fUserData?'u':'t', OfferingId);
	hFind = FindFirstFile(szTemplate, &wfd);
	if (hFind == INVALID_HANDLE_VALUE)
		goto NotFound;

	// Skip past all files
	while ((wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
	{
		if (!FindNextFile(hFind, &wfd))
			goto NotFound;
	}

	// Use this directory name to build the path
	cbSize = strlen(XONLINECONTENT_OPEN_PATH) + 
				strlen(wfd.cFileName) - 3;
	if (*pcbRootDirectory < cbSize)
	{
		*pcbRootDirectory = cbSize;
		hr = HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);
		goto Exit;
	}
	sprintf(szRootDirectory, XONLINECONTENT_OPEN_PATH,
				fUserData?'u':'t', wfd.cFileName);
	*pcbRootDirectory = cbSize;

Exit:
	if (hFind != INVALID_HANDLE_VALUE)
		FindClose(hFind);

    return(XoLeave(hr));

NotFound:
	hr = HRESULT_FROM_WIN32(ERROR_PATH_NOT_FOUND);
	goto Exit;
}
