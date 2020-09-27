/*******************************************************************************

Copyright (c) 1998 Microsoft Corporation.  All rights reserved.

File Name:

    util.cpp

Description:

    Utility functions.

*******************************************************************************/

#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <d3dx8.h>
#include "util.h"

static HANDLE g_hHeap = NULL;

//******************************************************************************
// Debugging function
//******************************************************************************

#ifdef _DEBUG

//******************************************************************************
//
// Function:
//
//     DebugString
//
// Description:
//
//     Take the formatted output, prepend the application name, and send the
//     output to the debugger.
//
// Arguments:
//
//     LPCTSTR szFormat         - Formatting string describing the output
//
//     Variable argument list   - Data to be placed in the output string
//
// Return Value:
//
//     None.
//
//******************************************************************************
void DebugString(LPCTSTR szFormat, ...) {

    TCHAR       szBuffer[2048];
    va_list     vl;

    _tcscpy(szBuffer, TEXT("D3DAPP: "));

    va_start(vl, szFormat);
    _vstprintf(szBuffer+8, szFormat, vl);
    va_end(vl);

    _tcscat(szBuffer, TEXT("\r\n"));

    OutputDebugString(szBuffer);
}

#endif // DEBUG

//******************************************************************************
// Memory allocation routines
//******************************************************************************

//******************************************************************************
//
// Function:
//
//     CreateHeap
//
// Description:
//
//     Create a heap for the application.
//
// Arguments:
//
//     None.
//
// Return Value:
//
//     TRUE on success, FALSE on failure.
//
//******************************************************************************
BOOL CreateHeap() {

    if (g_hHeap) {
        return FALSE;
    }

    g_hHeap = HeapCreate(0, 0, 0);
    if (!g_hHeap) {
        DebugString(TEXT("HeapCreate failed [%d]"), GetLastError());
        return FALSE;
    }

    return TRUE;
}

//******************************************************************************
//
// Function:
//
//     ReleaseHeap
//
// Description:
//
//     Destroy the application heap.
//
// Arguments:
//
//     None.
//
// Return Value:
//
//     None.
//
//******************************************************************************
void ReleaseHeap() {

    if (g_hHeap) {
        if (!HeapDestroy(g_hHeap)) {
            DebugString(TEXT("HeapDestroy failed [%d]"), GetLastError());
        }
        g_hHeap = NULL;
    }
}

//******************************************************************************
//
// Function:
//
//     MemAlloc
//
// Description:
//
//     Allocate the given amount of memory from the application heap.  This 
//     memory should later be freed using the MemFree function.
//
// Arguments:
//
//     UINT uNumBytes           - Number of bytes to allocate
//
// Return Value:
//
//     Base address of the allocated memory.
//
//******************************************************************************
LPVOID MemAlloc(UINT uNumBytes) {

    LPVOID pvMem = NULL;
    if (g_hHeap) {
        pvMem = HeapAlloc(g_hHeap, 0, uNumBytes);
    }
    return pvMem;
}

//******************************************************************************
//
// Function:
//
//     MemFree
//
// Description:
//
//     Free a block of memory that was allocated using the MemAlloc function.
//
// Arguments:
//
//     LPVOID pvMem             - Base address of the memory
//
// Return Value:
//
//     None.
//
//******************************************************************************
void MemFree(LPVOID pvMem) {

    if (g_hHeap) {
        if (!HeapFree(g_hHeap, 0, pvMem)) {
            DebugString(TEXT("HeapFree failed [%d]"), GetLastError());
        }
    }
}

//******************************************************************************
//
// Function:
//
//     MemAlloc32
//
// Description:
//
//     Allocate a given amount of memory whose base address is
//     aligned along a 32 byte boundary.  This memory must later be freed
//     using the MemFree32 function.
//
// Arguments:
//
//     UINT uNumBytes           - Number of bytes to allocate
//
// Return Value:
//
//     Base address of the allocated memory.
//
//******************************************************************************
LPVOID MemAlloc32(UINT uNumBytes)
{
	LPBYTE pbyAllocated, pbyAligned = NULL;

	pbyAllocated = (LPBYTE)MemAlloc(uNumBytes + 32);
    if (pbyAllocated) {
	    pbyAligned   = (LPBYTE)((((DWORD)pbyAllocated) + 32) & ~31);
	    *(((LPDWORD)pbyAligned)-1) = (DWORD)pbyAllocated;
    }
	return pbyAligned;
}

//******************************************************************************
//
// Function:
//
//     MemFree32
//
// Description:
//
//     Free an aligned block of memory that was allocated using the MemAlloc32
//     function.
//
// Arguments:
//
//     LPVOID pvAligned         - Base address of the aligned memory
//
// Return Value:
//
//     None.
//
//******************************************************************************
void MemFree32(LPVOID pvMem)
{
	LPBYTE pbyFree;

    if (pvMem) {
	    pbyFree = (LPBYTE)*(((LPDWORD)pvMem)-1);
	    MemFree(pbyFree);
    }
}

//******************************************************************************
// CMap32
//******************************************************************************

//******************************************************************************
CMap32::CMap32() {

    m_ppairList = NULL;
}

//******************************************************************************
CMap32::~CMap32() {

    PPAIR32 ppair;

    for (ppair = m_ppairList; m_ppairList; ppair = m_ppairList) {
        m_ppairList = m_ppairList->ppairNext;
        MemFree(ppair);
    }
}

//******************************************************************************
BOOL CMap32::Add(DWORD dwKey, DWORD dwData) {

    PPAIR32 ppair;
    ppair = (PPAIR32)MemAlloc(sizeof(PAIR32));
    if (!ppair) {
        return FALSE;
    }
    Remove(dwKey);
    ppair->dwKey = dwKey;
    ppair->dwData = dwData;
    ppair->ppairNext = m_ppairList;
    m_ppairList = ppair;
    return TRUE;
}

//******************************************************************************
BOOL CMap32::Remove(DWORD dwKey) {

    PPAIR32 ppair, ppairDel;

    if (m_ppairList) {

        if (m_ppairList->dwKey == dwKey) {
            ppair = m_ppairList->ppairNext;
            MemFree(m_ppairList);
            m_ppairList = ppair;
            return TRUE;
        }

        else {

            for (ppair = m_ppairList; 
                 ppair->ppairNext && ppair->ppairNext->dwKey != dwKey; 
                 ppair = ppair->ppairNext
            );

            if (ppair->ppairNext) {
                ppairDel = ppair->ppairNext;
                ppair->ppairNext = ppairDel->ppairNext;
                MemFree(ppairDel);
                return TRUE;
            }
        }
    }

    return FALSE;
}

//******************************************************************************
BOOL CMap32::Find(DWORD dwKey, LPDWORD pdwData) {

    PPAIR32 ppair;

    for (ppair = m_ppairList; 
         ppair && ppair->dwKey != dwKey;
         ppair = ppair->ppairNext
    );

    if (ppair) {
        if (pdwData) {
            *pdwData = ppair->dwData;
        }
        return TRUE;
    }

    return FALSE;
};

//******************************************************************************
BOOL CMap32::Find(LPDWORD pdwKey, DWORD dwData) {

    PPAIR32 ppair;

    for (ppair = m_ppairList; 
         ppair && ppair->dwData != dwData;
         ppair = ppair->ppairNext
    );

    if (ppair) {
        if (pdwKey) {
            *pdwKey = ppair->dwKey;
        }
        return TRUE;
    }

    return FALSE;
};

//******************************************************************************
//
// Function:
//
//     FormatToBitDepth
//
// Description:
//
//     Convert the given format to the number of bits needed to represent it.
//
// Arguments:
//
//     D3DFORMAT d3dfmt         - Resource format
//
// Return Value:
//
//     The number of bits required to represent the given format in memory.
//
//******************************************************************************
DWORD FormatToBitDepth(D3DFORMAT d3dfmt) {

    switch (d3dfmt) {
        case D3DFMT_DXT1:
            return 4;
        case D3DFMT_R3G3B2:
        case D3DFMT_A8:
        case D3DFMT_P8:
        case D3DFMT_L8:
        case D3DFMT_A4L4:
        case D3DFMT_DXT2:
        case D3DFMT_DXT4:
        case D3DFMT_DXT3:
        case D3DFMT_DXT5:
            return 8;
        case D3DFMT_R5G6B5:
        case D3DFMT_X1R5G5B5:
        case D3DFMT_A1R5G5B5:
        case D3DFMT_A4R4G4B4:
        case D3DFMT_A8R3G3B2:
        case D3DFMT_X4R4G4B4:
        case D3DFMT_A8P8:
        case D3DFMT_A8L8:
        case D3DFMT_V8U8:
        case D3DFMT_L6V5U5:
        case D3DFMT_D16_LOCKABLE:
        case D3DFMT_D16:
        case D3DFMT_D15S1:
            return 16;
        case D3DFMT_R8G8B8:
            return 24;
        case D3DFMT_A8R8G8B8:
        case D3DFMT_X8R8G8B8:
        case D3DFMT_X8L8V8U8:
        case D3DFMT_Q8W8V8U8:
        case D3DFMT_V16U16:
        case D3DFMT_W11V11U10:
        case D3DFMT_D32:
        case D3DFMT_D24S8:
        case D3DFMT_D24X8:
        case D3DFMT_D24X4S4:
            return 32;
        default:
            return 0;
    }
}

//******************************************************************************
BOOL IsCompressedFormat(D3DFORMAT d3dfmt) {

    switch (d3dfmt) {
        case D3DFMT_DXT1:
        case D3DFMT_DXT2:
        case D3DFMT_DXT4:
        case D3DFMT_DXT3:
        case D3DFMT_DXT5:
            return TRUE;
        default:
            return FALSE;
    }
}

//******************************************************************************
float DW2F(DWORD dw) {

    return *((float*)&dw);
}
