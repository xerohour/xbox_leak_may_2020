/*++

Copyright (c) Microsoft Corporation

Author:
    Keith Lau (keithlau)

Description:
	Module implementing utility functions for unti tests

Module Name:

    testutil.c

--*/

#include <xapip.h>
#include <xtl.h>
#include <stdio.h>
#include <xonlinep.h>

#include "testutil.h"

#include "pubkey.c"

static char *rgszDays[] = 
{
	"Sun", "Mon", "Tue", "Wed", 
	"Thu", "Fri", "Sat" 
};

static char *rgszMonths[] = 
{
	"", "Jan", "Feb", "Mar", "Apr", "May", "Jun", 
	"Jul", "Aug", "Sep", "Oct", "Nov", "Dec" 
};

HRESULT ConvertFileTimeAsString(LPSTR pBuffer, DWORD* pcbBuffer, PFILETIME pft)
{
    // We'll use RFC 1123 (actually a subset)
    // Sun, 06 Nov 1994 08:49:37 GMT
    //
    SYSTEMTIME  st;

    // check the buffer size
    if (MAX_TIME_SIZE > *pcbBuffer)
        return(HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER));


    // BUGBUG (michaelw) We should probably make sure the following succeeds but it sucks for callers
    // to have to check our return.  Consider using an Assert
    FileTimeToSystemTime(pft, &st);

    // assemble the final string
    *pcbBuffer = sprintf(pBuffer, "%s, %02d %s %4d %02d:%02d:%02d GMT", 
    			rgszDays[st.wDayOfWeek], st.wDay, rgszMonths[st.wMonth], 
    			st.wYear, st.wHour, st.wMinute, st.wSecond);
    pBuffer[*pcbBuffer] = '\0';

    return(S_OK);
}

//
// Function to change the Title ID
//
VOID WINAPI ChangeCurrentTitleId(
			DWORD	dwTitleId
			)
{
	XeImageHeader()->Certificate->TitleID = dwTitleId;
}

//
// Wrapper function for XOnlineLogon
//
HRESULT XOnlineLogonWrapper(
            DWORD* pdwServiceIDs,
			DWORD cServices,
            XONLINETASK_HANDLE *phLogon
			)
{
	HRESULT				hr = S_OK;
	DWORD				dwWorkFlags;
	XONLINE_USER Users[4];
        DWORD i;
                                                
	for (i = 0; i < 4; i += 1)
	{
		Users[i].xuid.qwUserID = 0;
	}
	
	// Initialize services
	hr = XOnlineLogon(Users, pdwServiceIDs, cServices, NULL, phLogon);
	if (FAILED(hr))
        goto Exit;

	// Pump until logon returns
	while( (hr = XOnlineTaskContinue(*phLogon)) == XONLINETASK_S_RUNNING );
	 
Exit:
	return(hr);
}			

void DumpRow(PBYTE pb1, DWORD cbb1, PBYTE pb2, DWORD cbb2)
{
	BYTE	rgbRow[80];
	BYTE	b;
	DWORD	i;

	memset(rgbRow, ' ', 79);
	rgbRow[79] = '\0';

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

	DebugPrint((CHAR *)rgbRow);
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

HRESULT HrAllocateAndReadFile(char *szFile, PBYTE *ppbBuffer, DWORD *pdwLength)
{
	HRESULT	hr = S_OK;
	HANDLE	hFile = INVALID_HANDLE_VALUE;
	LARGE_INTEGER liFileSize;
	PBYTE	pbTemp = NULL;
	DWORD	dwRead;

	hFile = CreateFile(szFile, GENERIC_READ, FILE_SHARE_READ, NULL,
				OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		goto Error;

	if (!GetFileSizeEx(hFile, &liFileSize))
		goto Error;

	pbTemp = (PBYTE) LocalAlloc(LMEM_FIXED, liFileSize.u.LowPart);
	if (!pbTemp)
		goto Error;

	if (!ReadFile(hFile, pbTemp, liFileSize.u.LowPart, &dwRead, NULL))
		goto Error;

	*pdwLength = dwRead;
	*ppbBuffer = pbTemp;

Cleanup:
	if (hFile != INVALID_HANDLE_VALUE)
		CloseHandle(hFile);
	return(hr);

Error:
	hr = HRESULT_FROM_WIN32(GetLastError());
	goto Cleanup;
}

HRESULT HrFileCompare(char *szFile1, char *szFile2, 
			PBYTE pbBuffer1, PBYTE pbBuffer2, DWORD dwLen)
{
	HRESULT	hr = S_OK;
	HANDLE	hFile1 = INVALID_HANDLE_VALUE;
	HANDLE	hFile2 = INVALID_HANDLE_VALUE;
	DWORD	dwRead1, dwRead2, dwTotalRead = 0;

	hFile1 = CreateFile(szFile1, GENERIC_READ, FILE_SHARE_READ, NULL,
				OPEN_EXISTING, FILE_FLAG_NO_BUFFERING | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	if (hFile1 == INVALID_HANDLE_VALUE)
		goto Error;

	hFile2 = CreateFile(szFile2, GENERIC_READ, FILE_SHARE_READ, NULL,
				OPEN_EXISTING, FILE_FLAG_NO_BUFFERING | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	if (hFile2 == INVALID_HANDLE_VALUE)
		goto Error;

	do
	{
		if (!ReadFile(hFile1, pbBuffer1, dwLen, &dwRead1, NULL))
			goto Error;
		if (!ReadFile(hFile2, pbBuffer2, dwLen, &dwRead2, NULL))
			goto Error;
		if ((dwRead1 != dwRead2) ||
			(memcmp(pbBuffer1, pbBuffer2, dwRead1) != 0))
		{
			DumpBuffers(pbBuffer1, dwRead1, pbBuffer2, dwRead2);
			hr = S_FALSE;
			goto Cleanup;
		}

		dwTotalRead += dwRead1;
		
	} while (dwRead1);

Cleanup:
	if (hFile1 != INVALID_HANDLE_VALUE)
		CloseHandle(hFile1);
	if (hFile2 != INVALID_HANDLE_VALUE)
		CloseHandle(hFile2);
	return(hr);
Error:
	hr = HRESULT_FROM_WIN32(GetLastError());
	goto Cleanup;
}

HRESULT HrFileCompareWithBuffer(char *szFile, 
			PBYTE pbBuffer1, DWORD dwLen1, 
			PBYTE pbBuffer2, DWORD dwLen2)
{
	HRESULT	hr = S_OK;
	HANDLE	hFile = INVALID_HANDLE_VALUE;
	DWORD	dwRead, dwTotalRead = 0;

	hFile = CreateFile(szFile, GENERIC_READ, FILE_SHARE_READ, NULL,
				OPEN_EXISTING, FILE_FLAG_NO_BUFFERING | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		goto Error;

	do
	{
		if (!ReadFile(hFile, pbBuffer1, dwLen1, &dwRead, NULL))
			goto Error;
			
		if ((dwRead > dwLen2) ||
			(memcmp(pbBuffer1, pbBuffer2, dwRead) != 0))
		{
			DumpBuffers(pbBuffer1, dwRead, pbBuffer2, dwRead);
			hr = S_FALSE;
			goto Cleanup;
		}

		dwLen2 -= dwRead;
		pbBuffer2 += dwRead;
		dwTotalRead += dwRead;
		
	} while (dwRead);

Cleanup:
	if (hFile != INVALID_HANDLE_VALUE)
		CloseHandle(hFile);
	return(hr);
Error:
	hr = HRESULT_FROM_WIN32(GetLastError());
	goto Cleanup;
}

HRESULT HrTruncateFile(char *szFile, long lBytesToCut)
{
	HRESULT	hr = S_OK;
	HANDLE	hFile = INVALID_HANDLE_VALUE;

	hFile = CreateFile(szFile, GENERIC_WRITE, FILE_SHARE_READ, NULL,
				OPEN_EXISTING, 0, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		goto Error;

	if (SetFilePointer(hFile, -lBytesToCut, NULL, FILE_END) == 
				INVALID_SET_FILE_POINTER)
		goto Error;

	if (!SetEndOfFile(hFile))
		goto Error;

Cleanup:
	if (hFile != INVALID_HANDLE_VALUE)
		CloseHandle(hFile);
	return(hr);
Error:
	hr = HRESULT_FROM_WIN32(GetLastError());
	goto Cleanup;
}



