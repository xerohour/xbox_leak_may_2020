// ---------------------------------------------------------------------------------------
// rebldcab.c
//
// Copyright (C) Microsoft Corporation
// ---------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------
// Includes
// ---------------------------------------------------------------------------------------

#include <windows.h>
#include <winsockx.h>
#include <xbox.h>
#include <xonlinep.h>

#include <stdio.h>
#include <string.h>

#include <fci.h>
#include <fdi.h>
#include <sha.h>

#include "cryptcab.h"
#include "rebldcab.h"


//
// File insertion callbacks
//

/*
 * File placed function called when a file has been committed
 * to a cabinet
 */
FNFCIFILEPLACED(RebldFilePlacedCallback)
{
	return 0;
}


/*
 * Function to obtain temporary files
 */
FNFCIGETTEMPFILE(RebldGetTempFileCallback)
{
    char    *psz;

    psz = _tempnam("","xx");            // Get a name
    if ((psz != NULL) && (strlen(psz) < (unsigned)cbTempName)) {
        strcpy(pszTempName,psz);        // Copy to caller's buffer
        free(psz);                      // Free temporary name buffer
        return TRUE;                    // Success
    }
    //** Failed
    if (psz) {
        free(psz);
    }

    return FALSE;
}

/*
 * Progress function
 */
FNFCISTATUS(RebldProgressCallback)
{
	return 0;
}


FNFCIGETNEXTCABINET(RebldGetNextCabinetCallback)
{
	RIP_ON_NOT_TRUE("RebldGetNextCabinetCallback", FALSE);
	return -1;
}


FNFCIGETOPENINFO(RebldGetOpenInfoCallback)
{
	BY_HANDLE_FILE_INFORMATION	finfo;
	FILETIME						filetime;
	HANDLE						handle;
	PREBLDCBK_CONTEXT			pContext = (PREBLDCBK_CONTEXT)pv;

	handle = CreateFile(
		pszName,
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN,
		NULL
	);

	if (handle == INVALID_HANDLE_VALUE)
	{
        *err = GetLastError();
		return -1;
	}

	if (GetFileInformationByHandle(handle, &finfo) == FALSE)
	{
        *err = GetLastError();
		CloseHandle(handle);
		return -1;
	}

	FileTimeToLocalFileTime(
		&finfo.ftLastWriteTime,
		&filetime
	);

	FileTimeToDosDateTime(
		&filetime,
		pdate,
		ptime
	);

	// CABARCX: Force the timestamp to be 0, we have other plans
	*ptime = 0;

    *pattribs = (USHORT) finfo.dwFileAttributes &
        (FILE_ATTRIBUTE_READONLY |
         FILE_ATTRIBUTE_HIDDEN   |
         FILE_ATTRIBUTE_SYSTEM   |
         FILE_ATTRIBUTE_ARCHIVE);

	// printf("  -- adding %s as %s\n", pszName, pContext->szFullName);

	return (INT_PTR)handle;
}

//
// Extraction callbacks
//
FNFDINOTIFY(RebldNewFileNotifyCallback)
{
	HRESULT				hr;
	int					ret = TRUE;
	DWORD				attrs;
	DWORD				dwSize;
	FILETIME				datetime;
	CHAR				*filename;
	INT_PTR				handle;
	BYTE				rgbSignature[XCALCSIG_SIGNATURE_SIZE];
	DGST_FILE_DATA		dfd;
	PREBLDCBK_CONTEXT	pContext = (PREBLDCBK_CONTEXT)(pfdin->pv);

	switch (fdint)
	{
		case fdintCABINET_INFO: // general information about the cabinet
			return 0;

		case fdintNEXT_CABINET:	// file continued to next cabinet
			return -1;

		case fdintPARTIAL_FILE: // first file in cabinet is continuation
			return 0;

		case fdintCOPY_FILE:	// file to be copied

			/*
			 * Ignore files which have a colon in them (e.g. C:\CONFIG.SYS)
			 */
			if (strchr(pfdin->psz1, ':'))
			{
				printf("   File '%s' contains illegal character (colon) -- skipping\n", pfdin->psz1);
				return 0;
			}

			filename = pfdin->psz1;
			handle = callback_file_open(
				pContext->szTempFile,
				_O_BINARY | _O_CREAT | _O_RDWR | _O_SEQUENTIAL,
				_S_IREAD | _S_IWRITE 
			);

			if (handle == -1)
			{
				printf("   -- Error opening '%s' for output\n", filename);
			}
			else 
			{
				//printf("   processing: %s\n", filename);
			}

			return handle;

		case fdintCLOSE_FILE_INFO:	// close the file, set relevant info

			filename = pfdin->psz1;
			attrs = 0;
			
			if (pfdin->attribs & _A_RDONLY)
			    attrs |= FILE_ATTRIBUTE_READONLY;

			if (pfdin->attribs & _A_ARCH)
			    attrs |= FILE_ATTRIBUTE_ARCHIVE;

			if (pfdin->attribs & _A_HIDDEN)
			    attrs |= FILE_ATTRIBUTE_HIDDEN;

			if (pfdin->attribs & _A_SYSTEM)
			    attrs |= FILE_ATTRIBUTE_SYSTEM;

			if (attrs == 0)
				attrs = FILE_ATTRIBUTE_NORMAL;

			/*
			 * Set file attributes and date/time
			 */
			if (TRUE == DosDateTimeToFileTime(
				pfdin->date,
				pfdin->time,
				&datetime))
			{
				FILETIME	local_filetime;

				if (TRUE == LocalFileTimeToFileTime(
					&datetime, 
					&local_filetime))
				{
					(void) SetFileTime(
						(HANDLE) pfdin->hf,
						&local_filetime,
						NULL,
						&local_filetime
					);
				}
			}

			// Before we close the file, we will compute the SHA1
			if (callback_file_seek(pfdin->hf, 0, SEEK_SET) != -1)
			{
				LARGE_INTEGER	liFileSize;
				PBYTE			pbBuffer;
				A_SHA_CTX		Sha;

				ret = -1;
				if (GetFileSizeEx((HANDLE)pfdin->hf, &liFileSize) == TRUE)
				{
					RIP_ON_NOT_TRUE("RebldNewFileNotifyCallback", (liFileSize.u.HighPart == 0));
					dwSize = liFileSize.u.LowPart;
					 pbBuffer = LocalAlloc(0, dwSize);
					 if (pbBuffer)
				 	{
				 		if (callback_file_read(pfdin->hf, pbBuffer, dwSize) == dwSize)
				 		{
				 			A_SHAInit(&Sha);
				 			A_SHAUpdate(&Sha, pbBuffer, dwSize);
				 			A_SHAFinal(&Sha, rgbSignature);
				 			ret = TRUE;
				 		}
				 		LocalFree(pbBuffer);
				 	}

				 	// Add this to the expected install size, in terms of clusters
				 	liFileSize.QuadPart += (XBOX_CLUSTER_SIZE - 1);
				 	liFileSize.QuadPart /= XBOX_CLUSTER_SIZE;
				 	liFileSize.QuadPart *= XBOX_CLUSTER_SIZE;
				 	pContext->InstallSize += liFileSize.QuadPart;
				}
			}

			// In any case we will need to close the file
			(void) callback_file_close(pfdin->hf);

			// Continue processing only if ret is still TRUE
			if (ret == TRUE)
			{
				(void) SetFileAttributes(pContext->szTempFile, attrs);

				// Now we insert this file into the new cabinet
				ret = -1;
				strcpy(pContext->szFullName, filename);
				if (FCIAddFile(pContext->hfci, pContext->szTempFile,
							filename, FALSE,
							RebldGetNextCabinetCallback,
							RebldProgressCallback,
							RebldGetOpenInfoCallback,
							TCOMPfromLZXWindow(15)) == TRUE)
				{
					// Insert the file information into the digest hash
					dfd.dwFileSize = dwSize;
					dfd.bFlags = 0;
					dfd.bReservedSize = 0;
					dfd.wSections = 1;
					dfd.rgSectionData[0].dwSectionStart = 0;
					dfd.rgSectionData[0].dwSectionSize = dwSize;
					CopyMemory(dfd.rgSectionData[0].rgbSignature, 
								rgbSignature, XCALCSIG_SIGNATURE_SIZE);

					strcpy(dfd.szFileName, filename);
					hr = InsertFileIntoDigest(pContext->pdc, &dfd);
					if (SUCCEEDED(hr))
						ret = TRUE;
				}
			}

			DeleteFile(pContext->szTempFile);

			return ret;
	}

	return 0;
}

//
// Decryption callback, this is mostly for tracking folder changes
//
FNFDIDECRYPT(RebldDecryptCallback)
{
	PREBLDCBK_CONTEXT	pContext = (PREBLDCBK_CONTEXT)(pfdid->pvUser);

	// Only handle new folder event
	if (pfdid->fdidt == fdidtNEW_FOLDER)
	{
		// This is where the old cabinet jumps to a new folder, follow
		// suite for the new cabinet
		if (!FCIFlushFolder(
					pContext->hfci,
					RebldGetNextCabinetCallback,
					RebldProgressCallback))
			return(-1);
	}
	return TRUE;
}

//
// Function to rebuild a cabinet file and insert a digest file
//
HRESULT RebuildCabinetWithDigest(
			char *szOldCabinet,
			char *szNewCabinet,
			DWORD cFiles, 
			char *szTempFile,
			char *szDigestFileName,
			PDGSTFILE_IDENTIFIER pdfi,
			PBYTE pbSymmetricKey,
			DWORD cbSymmetricKey,
			LARGE_INTEGER *pliPackageSize,
			LARGE_INTEGER *pliInstallSize
			)
{
	HRESULT	hr = S_OK;
	HFCI	hfci = NULL;
	HFDI	hfdi = NULL;
	BOOL	fRet;
	ERF		erf;
	CHAR	*p;
	CCAB	CabParams;
	CHAR	szCabinetName[MAX_PATHNAME_LENGTH];
	CHAR	szCabinetPath[MAX_PATHNAME_LENGTH];
	REBLDCBK_CONTEXT	Context;
	DGSTFILE_HEADER		Header;
	HANDLE				hFile;

	// Set up the context
	RIP_ON_NOT_TRUE("RebuildCabinetWithDigest", 
				(cbSymmetricKey == XONLINE_KEY_LENGTH));
	CopyMemory(Context.rgbSymmetricKey, pbSymmetricKey, cbSymmetricKey);	
	strcpy(Context.szTempFile, szTempFile);
	Context.InstallSize = 0;

	// Initialize the digest builder
	hr = CreateDigest(cFiles, 0, 0, &(Context.pdc));
	if (FAILED(hr))
		goto Error;

	// Create the FCI and FDI interfaces
	hfdi = FDICreate(
		callback_alloc,
		callback_free,
		callback_file_open,
		callback_file_read,
		callback_file_write,
		callback_file_close,
		callback_file_seek,
		cpu80386,
		&erf);
	if (hfdi == NULL)
	{
		printf("FDICreate() failed: code %d [%s]\n",
			erf.erfOper, return_fdi_error_string(erf.erfOper));
		hr = HRESULT_FROM_WIN32(erf.erfOper);
		goto Error;
	}

	// Setup CAB parameters
	ZeroMemory(&CabParams, sizeof(CCAB));
	CabParams.cb = LONG_MAX;
	CabParams.cbFolderThresh = LONG_MAX;
	CabParams.cbReserveCFHeader = 344;
	CabParams.cbReserveCFFolder = 28;
	CabParams.cbReserveCFData = 0;
	CabParams.iCab = 1;
	CabParams.iDisk = 0;
	CabParams.setID = 0;
	strcpy(CabParams.szDisk, "");
	strcpy(CabParams.szCab, szNewCabinet);

	hfci = FCICreate(
		&erf,
		RebldFilePlacedCallback,
		callback_fci_alloc,
		callback_fci_free,
		callback_fci_file_open,
		callback_fci_file_read,
		callback_fci_file_write,
		callback_fci_file_close,
		callback_fci_file_seek,
		callback_fci_file_delete,
		RebldGetTempFileCallback,
		&CabParams,
		&Context);
	if (hfci == NULL)
	{
		printf("FCICreate() failed: code %d [%s]\n",
			erf.erfOper, return_fci_error_string(erf.erfOper));
		hr = HRESULT_FROM_WIN32(erf.erfOper);
		goto Error;
	}

	Context.hfci = hfci;

	p = strrchr(szOldCabinet, '\\');
	if (p == NULL)
	{
		strcpy(szCabinetName, szOldCabinet);
		strcpy(szCabinetPath, "");
	}
	else
	{
		strcpy(szCabinetName, p+1);
		strncpy(szCabinetPath, szOldCabinet, (int) (p-szOldCabinet)+1);
		szCabinetPath[ (int) (p-szOldCabinet)+1 ] = 0;
	}

	if (TRUE != FDICopy(
		hfdi,
		szCabinetName,
		szCabinetPath,
		0,
		RebldNewFileNotifyCallback,
		RebldDecryptCallback,
		&Context))
	{
		printf("FDICopy() failed: code %u\n", GetLastError());
		hr = HRESULT_FROM_WIN32(GetLastError());
		goto Error;
	}

	// Alright now build the digest file
	Header.wReservedSize = 0;
	Header.wRating = 0;
	Header.wFlags = 0;

	// Special case: if all fields in dfi are zero, this means
	// the package can be shared
	if (!pdfi->OfferingId && !pdfi->dwTitleId && 
		!pdfi->wTitleVersionHi && !pdfi->wTitleVersionLo)
	{
		printf(" * No title and offering ID, marking content as shared\n");
		Header.wFlags |= DGSTFILE_HEADER_FLAG_SHARED;
	}
	
	CopyMemory(&(Header.dfi), pdfi, sizeof(DGSTFILE_IDENTIFIER));
	hr = SerializeDigestToFile(Context.pdc, szTempFile, &Header,
				pbSymmetricKey, cbSymmetricKey, NULL);
	if (FAILED(hr))
		goto Error;

	FreeDigest(Context.pdc);

#if 0
	// Verify digest	
	hr = DumpDigest(szTempFile, pbSymmetricKey, cbSymmetricKey);
	if (FAILED(hr))
		goto Error;
#endif

	// Now insert the digest into the cabinet, keep it in a separate
	// folder to minimize resume costs
	if (!FCIFlushFolder(
				hfci,
				RebldGetNextCabinetCallback,
				RebldProgressCallback))
	{
		printf("FCIFlushFolder() failed: code %u\n", GetLastError());
		hr = HRESULT_FROM_WIN32(GetLastError());
		goto Error;
	}
	
	strcpy(Context.szFullName, szDigestFileName);
	if (!FCIAddFile(
		hfci,
		szTempFile,
		szDigestFileName,
		FALSE,
		RebldGetNextCabinetCallback,
		RebldProgressCallback,
		RebldGetOpenInfoCallback,
		TCOMPfromLZXWindow(15)))
	{
		printf("FCIAddFile() failed: code %u\n", GetLastError());
		hr = HRESULT_FROM_WIN32(GetLastError());
		goto Error;
	}

	if (!FCIFlushCabinet(hfci, FALSE, RebldGetNextCabinetCallback,
				RebldProgressCallback))
	{
		printf("FCIFlushCabinet() failed: code %u\n", GetLastError());
		hr = HRESULT_FROM_WIN32(GetLastError());
		goto Error;
	}

Cleanup:

	if (hfdi != NULL)
		FDIDestroy(hfdi);
	if (hfci != NULL)
		FCIDestroy(hfci);

	DeleteFile(szTempFile);
	DeleteFile(szDigestFileName);
	
	if (FAILED(hr))
		DeleteFile(szNewCabinet);
	else
	{
		// Get the accurate package size
		hFile = CreateFile(
					szNewCabinet,
					GENERIC_READ, 
					FILE_SHARE_READ, 
					NULL, 
					OPEN_EXISTING, 
					FILE_ATTRIBUTE_NORMAL, 
					NULL);
		if (hFile == INVALID_HANDLE_VALUE)
		{
			hr = HRESULT_FROM_WIN32(GetLastError());
			printf("Cannot open package %s (%u)\n", szNewCabinet, hr);
			goto Error;
		}

		if (!GetFileSizeEx(hFile, pliPackageSize))
		{
			hr = HRESULT_FROM_WIN32(GetLastError());
			printf("Cannot get size of package %s (%u)\n", szNewCabinet, hr);
			goto Error;
		}

		CloseHandle(hFile);
		
		// Set the calculated Install size
		pliInstallSize->QuadPart = Context.InstallSize;
	}

	return(hr);

Error:
	goto Cleanup;
}



