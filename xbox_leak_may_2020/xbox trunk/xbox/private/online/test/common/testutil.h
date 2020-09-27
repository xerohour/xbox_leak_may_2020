/*++

Copyright (c) Microsoft Corporation

Author:
	Keith Lau (keithlau)

Description:
	Private definitions for the content delivery implementation

Module Name:

    testutil.h

--*/

#ifndef __TESTUTIL_H__
#define __TESTUTIL_H__

#ifdef __cplusplus
extern "C" {
#endif

extern BYTE *g_rgbPublicKey;
extern const DWORD g_cbPublicKey;

ULONG DebugPrint(PCHAR Format, ...);

#define	MAX_TIME_SIZE				40

HRESULT ConvertFileTimeAsString(LPSTR pBuffer, DWORD* pcbBuffer, PFILETIME pft);

VOID WINAPI ChangeCurrentTitleId(DWORD dwTitleId);

HRESULT XOnlineLogonWrapper(DWORD* pdwServiceIDs, DWORD cServices, XONLINETASK_HANDLE *phHandle);

void DumpRow(PBYTE pb1, DWORD cbb1, PBYTE pb2, DWORD cbb2);

void DumpBuffers(PBYTE pb1, DWORD cbb1, PBYTE pb2, DWORD cbb2);

HRESULT HrAllocateAndReadFile(char *szFile, PBYTE *ppbBuffer, DWORD *pdwLength);

HRESULT HrFileCompare(char *szFile1, char *szFile2, 
			PBYTE pbBuffer1, PBYTE pbBuffer2, DWORD dwLen);

HRESULT HrFileCompareWithBuffer(char *szFile, 
			PBYTE pbBuffer1, DWORD dwLen1, 
			PBYTE pbBuffer2, DWORD dwLen2);

HRESULT HrTruncateFile(char *szFile, long lBytesToCut);


#ifdef __cplusplus
}
#endif

#endif  //__TESTUTIL_H__

