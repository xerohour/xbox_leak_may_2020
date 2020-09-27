/*++

Copyright (c) Microsoft Corporation

Author:
    Keith Lau (keithlau)

Description:
	Unit test shell for XCalculateSignature* APIs

Module Name:

    xmain.c

--*/

#include <xapip.h>
#include <winbasep.h>
#include <xbox.h>
#include <xcrypt.h>
#include <xdbg.h>
#include <stdio.h>
#include <stdlib.h>

ULONG DebugPrint(PCHAR Format, ...);

//
// Use this to control whether to test romable
//
BOOL g_fTestRoamable = TRUE;

void DumpRow(PBYTE pb1, DWORD cbb1, PBYTE pb2, DWORD cbb2)
{
	BYTE	rgbRow[82];
	BYTE	b;
	DWORD	i;

	memset(rgbRow, ' ', 79);
	rgbRow[79] = '\r';
	rgbRow[80] = '\n';
	rgbRow[81] = '\0';

	for (i = 0; i < 16; i++)
	{
		if (cbb1 > i)
		{
			if ((cbb2 <= i) || (pb1[i] != pb2[i]))
				rgbRow[i*2] = '*';
			else
				rgbRow[i*2] = ' ';

			rgbRow[i*2 + 1] = ((pb1[i] >= 32) && (pb1[i] != '%'))?pb1[i]:'#';
		}

		if (cbb2 > i)
		{
			if ((cbb1 <= i) || (pb1[i] != pb2[i]))
				rgbRow[i*2 + 40] = '*';
			else
				rgbRow[i*2 + 40] = ' ';
			rgbRow[i*2 + 41] = ((pb2[i] >= 32) && (pb2[i] != '%'))?pb2[i]:'#';
		}
	}

	DebugPrint(rgbRow);
	return;
}	
	
void DumpBuffers(PBYTE pb1, DWORD cbb1, PBYTE pb2, DWORD cbb2)
{
	while ((cbb1 > 0) || (cbb2 > 0))
	{
		DumpRow(pb1, cbb1, pb2, cbb2);
		pb1 += 16;
		pb2 += 16;
		if (cbb1 >= 16)
			cbb1 -= 16;
		else
			cbb1 = 0;
			
		if (cbb2 >= 16)
			cbb2 -= 16;
		else
			cbb2 = 0;
	}
	return;	
}

HRESULT GenerateSignatureFromRandomChunks(
			PBYTE	pbBuffer, 
			DWORD	dwLen, 
			PXCALCSIG_SIGNATURE pSignature
			)
{
	HANDLE			hCalcSig = INVALID_HANDLE_VALUE;
	ULARGE_INTEGER	uliProcess;
	DWORD			dwError;

	hCalcSig = XCalculateSignatureBegin(
				g_fTestRoamable?0:XCALCSIG_FLAG_NON_ROAMABLE);
	if (hCalcSig == INVALID_HANDLE_VALUE)
	{
		RIP("Cannot init calcsig");
		return(HRESULT_FROM_WIN32(GetLastError()));
	}
	
	while (dwLen)
	{
		if (dwLen > 20000)
		{
			do {
				uliProcess.QuadPart = rand();
				uliProcess.QuadPart *= dwLen;
				uliProcess.QuadPart /= RAND_MAX;
			} while (uliProcess.QuadPart == 0);
		}	
		else
			uliProcess.QuadPart = dwLen;

		dwError = XCalculateSignatureUpdate(hCalcSig, pbBuffer, uliProcess.u.LowPart);
		if (dwError != ERROR_SUCCESS)
		{
			RIP("Cannot update calcsig");
			return(HRESULT_FROM_WIN32(dwError));
		}

		dwLen -= uliProcess.u.LowPart;
		pbBuffer += uliProcess.u.LowPart;
	}

	dwError = XCalculateSignatureEnd(hCalcSig, pSignature);
	if (dwError != ERROR_SUCCESS)
	{
		RIP("Cannot end calcsig");
		return(HRESULT_FROM_WIN32(dwError));
	}

	return(S_OK);
}

#define DATA_FILE		"t:\\test.dat"

//======================== The main function
void __cdecl main()
{
	HRESULT				hr;
	HANDLE				hFile;
	LARGE_INTEGER		liFileSize;
	ULARGE_INTEGER		uliOffset;
	ULARGE_INTEGER		uliIterations;
	BYTE				bOldValue;
	BYTE				bNewValue;
	PBYTE				pbReference = NULL;
	PBYTE				pbBuffer = NULL;
	DWORD				dwLen, dwRead, i;
	XCALCSIG_SIGNATURE	xcsReference;
	XCALCSIG_SIGNATURE	xcsSignature;

	uliIterations.QuadPart = 0;

	// Break here
	_asm int 3;
	
	// Read in the test file
	hFile = CreateFile(DATA_FILE, 
				GENERIC_READ, 
				FILE_SHARE_READ, NULL,
				OPEN_EXISTING, 
				FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		RIP("Cannot open file");
	}

	if (!GetFileSizeEx(hFile, &liFileSize))
	{
		RIP("Cannot get file size");
	}

	dwLen = liFileSize.u.LowPart;
	pbBuffer = (PBYTE)LocalAlloc(LMEM_FIXED, dwLen);
	if (!pbBuffer)
	{
		RIP("Cannot allocatre memory");
	}

	if (!ReadFile(hFile, pbBuffer, dwLen, &dwRead, NULL) ||
		(dwRead != dwLen))
	{
		RIP("Cannot read data file");
	}

	CloseHandle(hFile);

	// This is the non-roamable case, we use the ROM to calculate
	// the reference so we have a second opinion
	XcHMAC(XeImageHeader()->Certificate->SignatureKey, 
				XC_SYMMETRIC_KEY_SIZE,
				pbBuffer, dwLen, NULL, 0, 
				g_fTestRoamable?xcsReference.Signature:xcsSignature.Signature);
	if (!g_fTestRoamable)
	{
		// Non-roamable, run it through the HMAC again
		XcHMAC((LPBYTE)(*XboxHDKey), XBOX_KEY_LENGTH,
					xcsSignature.Signature, XCALCSIG_SIGNATURE_SIZE, 
					NULL, 0, 
					xcsReference.Signature);
	}

/*
	hr = GenerateSignatureFromRandomChunks(
				pbBuffer, dwLen, &xcsReference);
	if (FAILED(hr))
	{
		RIP("Cannot generate signature");
	}
*/	

	// Keep generateing signature from random chunks, and make sure 
	// all come to the same signature
	while (1)
	{
		// Do a straight run
		hr = GenerateSignatureFromRandomChunks(
					pbBuffer, dwLen, &xcsSignature);
		if (FAILED(hr))
		{
			RIP("Cannot generate signature");
		}

		if (memcmp(xcsReference.Signature, 
					xcsSignature.Signature, XCALCSIG_SIGNATURE_SIZE) != 0)
		{
			DumpBuffers(xcsReference.Signature, XCALCSIG_SIGNATURE_SIZE,
						xcsSignature.Signature, XCALCSIG_SIGNATURE_SIZE);
			RIP("Signatures don't match");
		}

		// Now corrupt the data randomly and make sure the
		// signature validation fails.
		for (i = 0; i < 100; i++)
		{
			uliOffset.QuadPart = rand();
			uliOffset.QuadPart *= (liFileSize.QuadPart - 1);
			uliOffset.QuadPart /= RAND_MAX;

			// Save the old value and corrupt the data
			bOldValue = pbBuffer[uliOffset.QuadPart];
			
			do {
				bNewValue = (BYTE)(rand() & 0xff);
			} while (bNewValue == bOldValue);
			
			pbBuffer[uliOffset.QuadPart] = bNewValue;

			// Generate signature with corrupt data
			hr = GenerateSignatureFromRandomChunks(
						pbBuffer, dwLen, &xcsSignature);
			if (FAILED(hr))
			{
				RIP("Cannot generate signature");
			}

			if (memcmp(xcsReference.Signature, 
						xcsSignature.Signature, XCALCSIG_SIGNATURE_SIZE) == 0)
			{
				DumpBuffers(xcsReference.Signature, XCALCSIG_SIGNATURE_SIZE,
							xcsSignature.Signature, XCALCSIG_SIGNATURE_SIZE);
				RIP("Signatures of corrupt data match!");
			}			

			// Restore the original value
			pbBuffer[uliOffset.QuadPart] = bOldValue;
		}

		uliIterations.QuadPart++;
	}

	// Break here
	_asm int 3;

	return;
}

