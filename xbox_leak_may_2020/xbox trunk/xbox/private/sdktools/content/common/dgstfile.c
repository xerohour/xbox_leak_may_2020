// ---------------------------------------------------------------------------------------
// dgstfile.c
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
#include <sha.h>

#include <stdio.h>

#include "cryptcab.h"
#include "dgstfile.h"

// ---------------------------------------------------------------------------------------

//
// API to initialize the digest builder
//
HRESULT CreateDigest(
			DWORD			dwMaxEntries,
			DWORD			dwHeaderReservedSize,
			DWORD			dwPerFileReservedSize,
			PDGST_CONTEXT	*ppContext
			)
{
	HRESULT			hr = S_OK;
	PDGST_CONTEXT	pContext;

	RIP_ON_NOT_TRUE("CreateDigest", (ppContext != NULL));

	pContext = LocalAlloc(0, sizeof(DGST_CONTEXT) + (dwMaxEntries * sizeof(DGST_FILE_DATA)));
	if (!pContext)
	{
		hr = E_OUTOFMEMORY;
		goto Error;
	}

	// Set up the context
	pContext->dwHeaderReserved = dwHeaderReservedSize;
	pContext->dwPerFileReserved = dwPerFileReservedSize;
	pContext->dwEntriesAlloc = dwMaxEntries;
	pContext->dwEntriesCur = 0;
	pContext->rgdfd = (PDGST_FILE_DATA)(pContext + 1);

	*ppContext = pContext;
	
Cleanup:
	return(hr);

Error:
	if (pContext)
		LocalFree(pContext);
	goto Cleanup;
}

//
// API to Insert a file record into the digest
//
HRESULT InsertFileIntoDigest(
			PDGST_CONTEXT		pContext,
			PDGST_FILE_DATA		pdfd
			)
{
	HRESULT	hr = S_OK;
	DWORD	i;
	int		iCompare;
	PDGST_FILE_DATA	pdfdCur;

	RIP_ON_NOT_TRUE("InsertFileIntoDigest", (pContext != NULL));
	RIP_ON_NOT_TRUE("InsertFileIntoDigest", (pdfd != NULL));

	// Just do a linear search
	for (i = 0, pdfdCur = pContext->rgdfd; i < pContext->dwEntriesCur; i++, pdfdCur++)
	{
		iCompare = _stricmp(pdfdCur->szFileName, pdfd->szFileName);
		if (iCompare == 0)
		{
			// Duplicates are nto allowed. This is a collision
			hr = HRESULT_FROM_WIN32(ERROR_ALREADY_EXISTS);
			goto Error;
		}
		else if (iCompare > 0)
		{
			// The list is sorted, the current list entry is greater than the insert entry
			// this is the insertion point
			break;
		}
	}

	// The current value of i marks the insertion point
	if (i >= pContext->dwEntriesAlloc)
	{
		hr = HRESULT_FROM_WIN32(ERROR_TOO_MANY_NAMES);
		goto Error;
	}

	// Shift entries back if necessary
	if (i < pContext->dwEntriesCur)
	{
		MoveMemory(pdfdCur + 1, pdfdCur, 
			((pContext->dwEntriesCur - i) * sizeof(DGST_FILE_DATA)));
	}

	CopyMemory(pdfdCur, pdfd, sizeof(DGST_FILE_DATA));
	pContext->dwEntriesCur++;
	
Cleanup:
	return(hr);

Error:
	goto Cleanup;
}

//
// API to serialze an in-memory digest to a file
//
// Note: the caller needs to fill in the wReservedSize, wFlags,
// and dfi members prior to calling this API. All other fields
// will be set automatically.
//
HRESULT SerializeDigestToFile(
			PDGST_CONTEXT		pContext,
			CHAR				*szDigestFile,
			PDGSTFILE_HEADER	pHeader,
			PBYTE				pbSymmetricKey,
			DWORD				cbSymmetricKey,
			PBYTE				pbHeaderReservedData
			)
{
	HRESULT				hr = S_OK;
	HANDLE				hFile = INVALID_HANDLE_VALUE;
	DWORD				i;
	DWORD				dwVectorOffset;
	DWORD				dwOffset = 0;
	DWORD				dwFixedSize;
	DWORD				dwNameLength;
	DWORD				dwSize;
	DWORD				*rgdwOffsets = NULL;
	PBYTE				pbBuffer = NULL;
	PDGST_FILE_DATA		pdfd;
	BYTE				rgbSignature[XCALCSIG_SIGNATURE_SIZE];

	RIP_ON_NOT_TRUE("SerializeDigestToFile", (pContext != NULL));
	RIP_ON_NOT_TRUE("SerializeDigestToFile", (pHeader != NULL));
	RIP_ON_NOT_TRUE("SerializeDigestToFile", (szDigestFile != NULL));
	RIP_ON_NOT_TRUE("SerializeDigestToFile", (pbSymmetricKey != NULL));
	RIP_ON_NOT_TRUE("SerializeDigestToFile", (cbSymmetricKey == XONLINE_KEY_LENGTH));

	RIP_ON_NOT_TRUE("SerializeDigestToFile", (pHeader->wReservedSize == 0));

	// First, create the file
	hFile = CreateFile(
				szDigestFile,
				GENERIC_READ | GENERIC_WRITE, 
				FILE_SHARE_READ, 
				NULL, 
				CREATE_ALWAYS, 
				FILE_ATTRIBUTE_NORMAL, 
				NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		hr = HRESULT_FROM_WIN32(GetLastError());
		printf("Cannot create digest file (%u)\n", hr);
		goto Error;
	}

	// Set up the header structure
	pHeader->bDigestVersionHi = DGSTFILE_VERSION_HI;
	pHeader->bDigestVersionLo = DGSTFILE_VERSION_LO;
	pHeader->cDigestEntries = pContext->dwEntriesCur;
	ZeroMemory(pHeader->rgbSignature, XCALCSIG_SIGNATURE_SIZE);

	// Write out the header
	if (!WriteFile(hFile, pHeader, sizeof(DGSTFILE_HEADER), &dwSize, NULL))
	{
		hr = HRESULT_FROM_WIN32(GetLastError());
		printf("Cannot write digest file header (%u)\n", hr);
		goto Error;
	}

	if (pContext->dwHeaderReserved && pbHeaderReservedData)
	{
		// Write out the reserved data
		if (!WriteFile(hFile, pbHeaderReservedData, 
					pContext->dwHeaderReserved, &dwSize, NULL))
		{
			hr = HRESULT_FROM_WIN32(GetLastError());
			printf("Cannot write reserved header data (%u)\n", hr);
			goto Error;
		}
	}

	// Allocate the offset vector
	dwSize = sizeof(DWORD) * pContext->dwEntriesCur;
	rgdwOffsets = LocalAlloc(0, dwSize);
	if (!rgdwOffsets)
	{
		hr = E_OUTOFMEMORY;
		goto Error;
	}

	// Serialize out the data
	dwVectorOffset = SetFilePointer(hFile, 0, NULL, FILE_CURRENT);
	dwOffset = SetFilePointer(hFile, dwSize, NULL, FILE_CURRENT);
	if (dwOffset == INVALID_SET_FILE_POINTER)
	{
		hr = HRESULT_FROM_WIN32(GetLastError());
		printf("Failed preparing to write digest data (%u)\n", hr);
		goto Error;
	}

	// Note dwOffset has the current file offset

	//printf("Writing digest file ...\n");

	// Now figure out the size of each file record.
	// Note: thisonly works because we assume each file only has one section. This needs
	// to be changed if we support more sections.
	dwFixedSize = sizeof(DGST_FILE_DATA) - MAX_PATH;

	for (i = 0, pdfd = pContext->rgdfd; i < pContext->dwEntriesCur; i++, pdfd++)
	{
		// Write out the fixed portion of the file record
		if (!WriteFile(hFile, &(pdfd->dwFileSize), dwFixedSize, &dwSize, NULL))
		{
			hr = HRESULT_FROM_WIN32(GetLastError());
			printf("Cannot write digest fixed data (%u)\n", hr);
			goto Error;
		}

		// Figure out the length of the file name (including NULL)
		dwNameLength = strlen(pdfd->szFileName);
		dwNameLength++;

		// Write out the string
		if (!WriteFile(hFile, pdfd->szFileName, dwNameLength, &dwSize, NULL))
		{
			hr = HRESULT_FROM_WIN32(GetLastError());
			printf("Cannot write digest file data (%u)\n", hr);
			goto Error;
		}

		//printf("  Added %s to digest\n", pdfd->szFileName);

		// Save the offset
		rgdwOffsets[i] = dwOffset;
		dwSize = dwNameLength + dwFixedSize;
		dwOffset += dwSize;
		
	}

	// Write out the offset vector
	if (SetFilePointer(hFile, dwVectorOffset, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
	{
		hr = HRESULT_FROM_WIN32(GetLastError());
		printf("Failed preparing to write offset data (%u)\n", hr);
		goto Error;
	}

	if (!WriteFile(hFile, rgdwOffsets, sizeof(DWORD) * pContext->dwEntriesCur, &dwSize, NULL))
	{
		hr = HRESULT_FROM_WIN32(GetLastError());
		printf("Cannot write to write offset data (%u)\n", hr);
		goto Error;
	}

	// Read the whole file and compute the signature
	dwSize = GetFileSize(hFile, NULL);
	if (dwSize == INVALID_FILE_SIZE)
	{
		hr = HRESULT_FROM_WIN32(GetLastError());
		printf("Failed preparing to compute signature (%u)\n", hr);
		goto Error;
	}
	RIP_ON_NOT_TRUE("SerializeDigestToFile", (dwSize == dwOffset));

	if (SetFilePointer(hFile, XCALCSIG_SIGNATURE_SIZE, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
	{
		hr = HRESULT_FROM_WIN32(GetLastError());
		printf("Failed preparing to compute signature (%u)\n", hr);
		goto Error;
	}
	dwSize -= XCALCSIG_SIGNATURE_SIZE;

	pbBuffer = LocalAlloc(0, dwSize);
	if (!pbBuffer)
	{
		hr = E_OUTOFMEMORY;
		goto Error;
	}

	if (!ReadFile(hFile, pbBuffer, dwSize, &i, NULL))
	{
		hr = HRESULT_FROM_WIN32(GetLastError());
		printf("Cannot verify digest data (%u)\n", hr);
		goto Error;
	}

	shaHmac(pbSymmetricKey, cbSymmetricKey, pbBuffer, dwSize, NULL, 0, rgbSignature);

	// Finally, write out the signature
	if (SetFilePointer(hFile, 0, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
	{
		hr = HRESULT_FROM_WIN32(GetLastError());
		printf("Failed preparing to write signature (%u)\n", hr);
		goto Error;
	}

	if (!WriteFile(hFile, rgbSignature, XCALCSIG_SIGNATURE_SIZE, &dwSize, NULL))
	{
		hr = HRESULT_FROM_WIN32(GetLastError());
		printf("Cannot write to write offset data (%u)\n", hr);
		goto Error;
	}

Cleanup:

	if (hFile != INVALID_HANDLE_VALUE)
		CloseHandle(hFile);
	
	if (FAILED(hr))
	{
		DeleteFile(szDigestFile);
	}

	if (rgdwOffsets)
		LocalFree(rgdwOffsets);
	if (pbBuffer)
		LocalFree(pbBuffer);
	
	return(hr);

Error:
	goto Cleanup;
}

//
// API to free a digest context
//
VOID FreeDigest(
			PDGST_CONTEXT	pContext
			)
{
	LocalFree(pContext);
}

//
// API to create a digest file from a file list
//
HRESULT CreateDigestFromFileList(
			cab_file_list_header	*pFileList,
			PDGSTFILE_HEADER		pdfh,
			PSTR					szDigestPath,
			PBYTE					pbSymmetricKey,
			DWORD					cbSymmetricKey
			)
{
	HRESULT			hr = S_OK;
	PDGST_CONTEXT	pDigest = NULL;
	DGST_FILE_DATA	dfd;
	DGSTFILE_HEADER	dfh;
	LARGE_INTEGER	liSize;
	HANDLE			hFile = INVALID_HANDLE_VALUE;
	PBYTE			pbBuffer= NULL;
	DWORD			cItems = 0;
	DWORD			cbSize;
	A_SHA_CTX		Sha;
	CHAR			szStrippedName[MAX_PATH];
	struct cab_file_link	*item;

	// Count how many items in the file list
	for (item = pFileList->flh_head; item; item = item->fl_next, cItems++)
		;

	// Create a digest
	hr = CreateDigest(cItems, 0, 0, &pDigest);
	if (FAILED(hr))
		goto Error;
	
	for (item = pFileList->flh_head; item; item = item->fl_next)
	{
        if (item->fl_logical_filename != NULL)
        {
            // the user already supplied a logical filename
            strcpy(szStrippedName, item->fl_logical_filename);
        }
        else
        {
        	// Get the relative path to store
            strcpy(szStrippedName, item->fl_filename);
        }

		// Fill in the digest file data for this file
		hFile = CreateFile(item->fl_filename,
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
		// Get the file size
		if (!GetFileSizeEx(hFile, &liSize))
		{
			hr = HRESULT_FROM_WIN32(GetLastError());
			goto Error;
		}

		// Allocate a buffer
		pbBuffer = (PBYTE)LocalAlloc(0, liSize.u.LowPart);
		if (!pbBuffer)
		{
			hr = E_OUTOFMEMORY;
			goto Error;
		}

		// Read in the entire file
		if (!ReadFile(hFile, pbBuffer, liSize.u.LowPart, &cbSize, NULL))
		{
			hr = HRESULT_FROM_WIN32(GetLastError());
			goto Error;
		}

		// Set up the DGST_FILE_DATA struct
		strcpy(dfd.szFileName, szStrippedName);
		dfd.dwFileSize = cbSize;
		dfd.bFlags = 0;
		dfd.bReservedSize = 0;
		dfd.wSections = 1;
		dfd.rgSectionData[0].dwSectionStart = 0;
		dfd.rgSectionData[0].dwSectionSize = cbSize;

		// Run the SHA1 checksum
		A_SHAInit(&Sha);
		A_SHAUpdate(&Sha, pbBuffer, cbSize);
		A_SHAFinal(&Sha, dfd.rgSectionData[0].rgbSignature);

        // Insert this file to the digest.
        hr = InsertFileIntoDigest(pDigest, &dfd);
        if (FAILED(hr))
        	goto Error;

       	// Free the buffer
       	LocalFree(pbBuffer);
       	pbBuffer = NULL;
	}

	// OK, now serialize the digest out to the specified file
	hr = SerializeDigestToFile(pDigest, 
				szDigestPath, 
				pdfh, 
				pbSymmetricKey, cbSymmetricKey, NULL);
	if (FAILED(hr))
		goto Error;

Exit:
	if (hFile != INVALID_HANDLE_VALUE)
		CloseHandle(hFile);

	if (pbBuffer)
		LocalFree(pbBuffer);

	if (pDigest)
		FreeDigest(pDigest);

	return(hr);

Error:
	goto Exit;
}

//
// API to dump a digest file
//
HRESULT DumpDigest(
			CHAR	*szDigestFile,
			BOOL	fVerify,
			PBYTE	pbSymmetricKey,
			DWORD	cbSymmetricKey
			)
{
	HRESULT				hr = S_OK;
	HANDLE				hFile = INVALID_HANDLE_VALUE;
	HANDLE				hVerify = INVALID_HANDLE_VALUE;
	DWORD				i, c;
	DWORD				dwSize;
	DWORD				*rgdwOffsets = NULL;
	PBYTE				pbBuffer = NULL;
	PBYTE				pbVerify = NULL;
	DWORD				cbMax;
	DWORD				cbVerify;
	CHAR				*szFileName;
	PDGSTFILE_HEADER	pHeader;
	PDGSTFILE_FILE_DATA	pdffd;
	BYTE				rgbSignature[XCALCSIG_SIGNATURE_SIZE];
	A_SHA_CTX			Sha;

	RIP_ON_NOT_TRUE("SerializeDigestToFile", (szDigestFile != NULL));

	// First, open the file
	hFile = CreateFile(
				szDigestFile,
				GENERIC_READ, 
				FILE_SHARE_READ, 
				NULL, 
				OPEN_EXISTING, 
				FILE_ATTRIBUTE_NORMAL, 
				NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		hr = HRESULT_FROM_WIN32(GetLastError());
		printf("Cannot open digest file (%u)\n", hr);
		goto Error;
	}

	// Figure out how big the file is
	dwSize = GetFileSize(hFile, NULL);
	if (dwSize == INVALID_FILE_SIZE)
	{
		hr = HRESULT_FROM_WIN32(GetLastError());
		printf("Failed to get file size (%u)\n", hr);
		goto Error;
	}

	// Allocate the buffer
	pbBuffer = LocalAlloc(0, dwSize);
	if (!pbBuffer)
	{
		hr = E_OUTOFMEMORY;
		goto Error;
	}

	// Read the whole file
	if (!ReadFile(hFile, pbBuffer, dwSize, &dwSize, NULL))
	{
		hr = HRESULT_FROM_WIN32(GetLastError());
		printf("Cannot Read digest file (%u)\n", hr);
		goto Error;
	}

	// Calculate the signature and make sure it matches
	shaHmac(pbSymmetricKey, cbSymmetricKey,
				pbBuffer + XCALCSIG_SIGNATURE_SIZE,
				dwSize - XCALCSIG_SIGNATURE_SIZE,
				NULL, 0, rgbSignature);

	if (memcmp(rgbSignature, pbBuffer, XCALCSIG_SIGNATURE_SIZE) != 0)
	{
		hr = HRESULT_FROM_WIN32(ERROR_INVALID_DATA);
		printf("File digest does not match (%u)\n", hr);
		goto Error;
	}

	// Display header info
	pHeader = (PDGSTFILE_HEADER)pbBuffer;
	printf("Digest version: %u.%02u\n", 
				pHeader->bDigestVersionHi, pHeader->bDigestVersionLo);
	printf("Digest file entries: %u\n", pHeader->cDigestEntries);
	printf("Reserved size: %u\n", pHeader->wReservedSize);
	printf("Flags: %4x\n\n", pHeader->wFlags);

	printf("Content ID: %08x\n\n", pHeader->dfi.OfferingId);

	// Find the offset vector
	rgdwOffsets = (PDWORD)(pbBuffer + (sizeof(DGSTFILE_HEADER) + pHeader->wReservedSize));

	// Find the max amount of memory needed
	for (i = 0, cbMax = 0; i < pHeader->cDigestEntries; i++)
	{
		pdffd = (PDGSTFILE_FILE_DATA)(pbBuffer + rgdwOffsets[i]);

		for (c = 0; c < pdffd->wSections; c++)
		{
			if (pdffd->rgSectionData[c].dwSectionSize > cbMax)
				cbMax = pdffd->rgSectionData[c].dwSectionSize;
		}
	}
	pbVerify = LocalAlloc(0, cbMax);
	if (!pbVerify)
	{
		hr = E_OUTOFMEMORY;
		goto Error;
	}

	for (i = 0; i < pHeader->cDigestEntries; i++)
	{
		// Dump each entry
		pdffd = (PDGSTFILE_FILE_DATA)(pbBuffer + rgdwOffsets[i]);
		szFileName = (CHAR *)(pdffd + 1);

		printf("File name: %s\n", szFileName);
		printf("    File Size: %u\n", pdffd->dwFileSize);
		printf("    File Flags: %08x\n", pdffd->bFlags);
		printf("    Reserved size: %u\n\n", pdffd->bReservedSize);
		printf("    Number of checksum sections: %u\n", pdffd->wSections);

		if (fVerify)
		{
			hVerify = CreateFile(
						szFileName,
						GENERIC_READ, 
						FILE_SHARE_READ, 
						NULL, 
						OPEN_EXISTING, 
						FILE_ATTRIBUTE_NORMAL, 
						NULL);
			if (hFile == INVALID_HANDLE_VALUE)
			{
				hr = HRESULT_FROM_WIN32(GetLastError());
				printf("Cannot open file %s for verify (%08x)\n", hr);
				goto Error;
			}

			// Figure out how big the file is
			dwSize = GetFileSize(hVerify, NULL);
			if (dwSize == INVALID_FILE_SIZE)
			{
				hr = HRESULT_FROM_WIN32(GetLastError());
				printf("Failed to get file size (%u)\n", hr);
				goto Error;
			}

			for (c = 0; c < pdffd->wSections; c++)
			{
				printf("    -- Section %u - Start: %u, Size: %u  ", c,
						pdffd->rgSectionData[c].dwSectionStart,
						pdffd->rgSectionData[c].dwSectionSize);

				if (SetFilePointer(hVerify, 
							pdffd->rgSectionData[c].dwSectionStart,
							NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
				{
					hr = HRESULT_FROM_WIN32(GetLastError());
					goto Error;
				}
				if (!ReadFile(hVerify, pbVerify, 
							pdffd->rgSectionData[c].dwSectionSize, 
							&cbVerify, NULL))
				{
					hr = HRESULT_FROM_WIN32(GetLastError());
					goto Error;
				}

				// Run the checksum (minus signature)
				A_SHAInit(&Sha);
				A_SHAUpdate(&Sha, pbVerify, cbVerify);
				A_SHAFinal(&Sha, rgbSignature);

				// Verify
				if (memcmp(pdffd->rgSectionData[c].rgbSignature,
							rgbSignature, XCALCSIG_SIGNATURE_SIZE) != 0)
				{
					puts("[FAILED]\n");
					hr = HRESULT_FROM_WIN32(ERROR_INVALID_DATA);
					goto Error;
				}
				else
				{
					puts("[OK]\n");
				}
			}
			puts("");

			CloseHandle(hVerify);
			hVerify = INVALID_HANDLE_VALUE;
		}
		else
		{
			for (c = 0; c < pdffd->wSections; c++)
			{
				printf("    -- Section %u - Start: %u, Size: %u\n", c,
						pdffd->rgSectionData[c].dwSectionStart,
						pdffd->rgSectionData[c].dwSectionSize);

				
			}
			puts("");
		}
	}

Cleanup:

	if (hFile != INVALID_HANDLE_VALUE)
		CloseHandle(hFile);

	if (hVerify != INVALID_HANDLE_VALUE)
		CloseHandle(hVerify);

	if (pbBuffer)
		LocalFree(pbBuffer);

	if (pbVerify)
		LocalFree(pbVerify);
	
	return(hr);

Error:
	goto Cleanup;
}


