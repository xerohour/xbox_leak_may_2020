/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    util.cpp

Author:

    Matt Bronder

Description:

    Utility functions.

*******************************************************************************/

#include "d3dbase.h"

//******************************************************************************
// Globals
//******************************************************************************

static HANDLE               g_hHeap = NULL;
static HINSTANCE            g_hInstance = NULL;

//******************************************************************************
// Debugging functions
//******************************************************************************

#if defined(DEBUG) || defined(_DEBUG) || defined(DBG)
//******************************************************************************
//
// Function:
//
//     DebugString
//
// Description:
//
//     Take the formatted output and send the output to the debugger.
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

    va_list vl;
#ifndef UNDER_XBOX
    TCHAR   szBuffer[2304];
#else
    TCHAR*  szBuffer = (LPTSTR)MemAlloc32(2304 * sizeof(TCHAR));
    if (!szBuffer) {
        OutputDebugString(TEXT("Insufficient memory for logging buffer allocation\n"));
        __asm int 3;
    }
#endif // UNDER_XBOX

    va_start(vl, szFormat);
//    wvsprintf(szBuffer, szFormat, vl);
    _vstprintf(szBuffer, szFormat, vl);
    va_end(vl);

    _tcscat(szBuffer, TEXT("\r\n"));

    OutputDebugString(szBuffer);

#ifdef UNDER_XBOX
    MemFree32(szBuffer);
#endif // UNDER_XBOX
}

//******************************************************************************
//
// Function:
//
//     ResultFailed
//
// Description:
//
//     Test a given return code: if the code is an error, output a debug 
//     message with the error value.  If the code is a warning, output
//     a debug message with the warning value.
//
// Arguments:
//
//     HRESULT hr               - Return code to test for an error
//
//     LPCTSTR sz               - String describing the method that produced 
//                                the return code
//
// Return Value:
//
//     TRUE if the given return code is an error, FALSE otherwise.
//
//******************************************************************************
BOOL ResultFailed(HRESULT hr, LPCTSTR sz) {

    TCHAR szError[256];

    if (SUCCEEDED(hr)) {
        return FALSE;
    }

    D3DXGetErrorString(hr, szError, 256);

    if (FAILED(hr)) {
        DebugString(TEXT("%s failed with %s [0x%X]"), sz, szError, hr);
        return TRUE;
    }
    else {
        DebugString(TEXT("%s returned %s [0x%X]"), sz, szError, hr);
    }

    return FALSE;
}
#endif // DEBUG || _DEBUG || DBG

//******************************************************************************
//
// Function:
//
//     OutputString
//
// Description:
//
//     Take the formatted output and send the output to the debugger.
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
void OutputString(LPCTSTR szFormat, ...) {

    va_list vl;
#ifndef UNDER_XBOX
    TCHAR   szBuffer[2304];
#else
    TCHAR*  szBuffer = (LPTSTR)MemAlloc32(2304 * sizeof(TCHAR));
    if (!szBuffer) {
        OutputDebugString(TEXT("Insufficient memory for logging buffer allocation\n"));
        __asm int 3;
    }
#endif // UNDER_XBOX

    va_start(vl, szFormat);
//    wvsprintf(szBuffer, szFormat, vl);
    _vstprintf(szBuffer, szFormat, vl);
    va_end(vl);

    _tcscat(szBuffer, TEXT("\r\n"));

    OutputDebugString(szBuffer);

#ifdef UNDER_XBOX
    MemFree32(szBuffer);
#endif // UNDER_XBOX
}

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
// Test instance
//******************************************************************************

//******************************************************************************
//
// Function:
//
//     RegisterInstance
//
// Description:
//
//     Set the module instance of the application.
//
// Arguments:
//
//     HINSTANCE hInstance      - Instance handle
//
// Return Value:
//
//     None.
//
//******************************************************************************
void RegisterInstance(HINSTANCE hInstance) {

    g_hInstance = hInstance;
}

//******************************************************************************
//
// Function:
//
//     GetInstance
//
// Description:
//
//     Get the module instance of the application.
//
// Arguments:
//
//     None.
//
// Return Value:
//
//     Instance handle of the application module.
//
//******************************************************************************
HINSTANCE GetInstance() {

    return g_hInstance;
}

