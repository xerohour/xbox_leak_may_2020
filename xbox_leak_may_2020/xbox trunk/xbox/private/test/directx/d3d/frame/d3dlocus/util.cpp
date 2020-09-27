/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    util.cpp

Author:

    Matt Bronder

Description:

    Utility functions.

*******************************************************************************/

#include "d3dlocus.h"

//******************************************************************************
// Globals
//******************************************************************************

static HANDLE               g_hHeap = NULL;
static HINSTANCE            g_hInstance = NULL;
static TESTSTARTUPCONTEXT   g_tscontext = 0;
static CDisplay*            g_pDisplay = NULL;
static BOOL                 g_bDisplayStable = TRUE;

//******************************************************************************
// Debugging functions
//******************************************************************************

//#if defined(DEBUG) || defined(_DEBUG) || defined(DBG)
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
    UINT    i, uLen, uSize;
    TCHAR   szTemp[81];
#ifndef UNDER_XBOX
    TCHAR   szBuffer[18432];
#else
    TCHAR*  szBuffer;
#endif // UNDER_XBOX

#ifdef UNDER_XBOX
    szBuffer = (LPTSTR)MemAlloc32(18432 * sizeof(TCHAR));
    if (!szBuffer) {
        OutputDebugString(TEXT("Insufficient memory for logging buffer allocation\n"));
//        __asm int 3;
        return;
    }
#endif // UNDER_XBOX

    szTemp[80] = '\0';

    va_start(vl, szFormat);
//    wvsprintf(szBuffer, szFormat, vl);
    _vstprintf(szBuffer, szFormat, vl);
    va_end(vl);

    _tcscat(szBuffer, TEXT("\r\n"));

    for (i = 0, uSize = _tcslen(szBuffer) + 1; i < uSize; i += 80) {
        uLen = i + 80 < uSize ? 80 : uSize - i;
        memcpy(szTemp, (LPTSTR)szBuffer + i, uLen * sizeof(TCHAR));
//        xDebugString(szTemp);
        xtprintf(szTemp);
        if (!(GetStartupContext() & TSTART_STRESS)) {
            OutputDebugString(szTemp);
        }
    }

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
        Log(LOG_FAIL, TEXT("%s failed with %s [0x%X]"), sz, szError, hr);
        return TRUE;
    }
    else {
        Log(LOG_WARN, TEXT("%s returned %s [0x%X]"), sz, szError, hr);
    }

    return FALSE;
}
//#endif // DEBUG || _DEBUG || DBG

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
// Test startup information
//******************************************************************************

//******************************************************************************
//
// Function:
//
//     RegisterStartupContext
//
// Description:
//
//     Set the startup information for the test.
//
// Arguments:
//
//     TESTSTARTUPCONTEXT tscontext   - Startup information
//
// Return Value:
//
//     None.
//
//******************************************************************************
void RegisterStartupContext(TESTSTARTUPCONTEXT tscontext) {

    g_tscontext = tscontext;
}

//******************************************************************************
//
// Function:
//
//     GetStartupContext
//
// Description:
//
//     Get the startup information for the test.
//
// Arguments:
//
//     None.
//
// Return Value:
//
//     Startup information for the test.
//
//******************************************************************************
TESTSTARTUPCONTEXT GetStartupContext() {

    return g_tscontext;
}


//******************************************************************************
// Display information
//******************************************************************************

//******************************************************************************
void RegisterDisplay(CDisplay* pDisplay) {

    g_pDisplay = pDisplay;
}

//******************************************************************************
CDisplay* GetDisplay() {

    return g_pDisplay;
}

//******************************************************************************
void EnableDisplayStability(BOOL bEnable) {

    g_bDisplayStable = bEnable;
}

//******************************************************************************
BOOL IsDisplayStable() {

    return g_bDisplayStable;
}

//******************************************************************************
// Test instance
//******************************************************************************

//******************************************************************************
//
// Function:
//
//     RegisterTestInstance
//
// Description:
//
//     Set the module instance of the test.
//
// Arguments:
//
//     HINSTANCE hInstance      - Test instance handle
//
// Return Value:
//
//     None.
//
//******************************************************************************
void RegisterTestInstance(HINSTANCE hInstance) {

    g_hInstance = hInstance;
}

//******************************************************************************
//
// Function:
//
//     GetTestInstance
//
// Description:
//
//     Get the module instance of the test.
//
// Arguments:
//
//     None.
//
// Return Value:
//
//     Instance handle to the test module.
//
//******************************************************************************
HINSTANCE GetTestInstance() {

    return g_hInstance;
}

//******************************************************************************
//
// Function:
//
//     GetModuleName
//
// Description:
//
//     Get the name of the module with the given instance.
//
// Arguments:
//
//     HMODULE hModule        - Instance of the module
//
//     LPTSTR szModuleName    - Pointer to a buffer to receive the module name
//
//     DWORD dwSize           - Size of the buffer in characters
//
// Return Value:
//
//     Number of characters copied to the module name buffer on success, zero
//     on failure.
//
//******************************************************************************
DWORD GetModuleName(HMODULE hModule, LPTSTR szModuleName, DWORD dwSize) {

#ifndef UNDER_XBOX

    return GetModuleFileName(hModule, szModuleName, dwSize);

#else

    PD3DTESTFUNCTIONDESC    pd3dtfd = (PD3DTESTFUNCTIONDESC)hModule;
    LPTSTR                  szSrc, szDst;
    UINT                    i;

    if (pd3dtfd) {
        for (szSrc = pd3dtfd->szModule, szDst = szModuleName, i = 0;
             *szSrc && i < dwSize - 1; szSrc++, szDst++, i++)
        {
            *szDst = *szSrc;
        }
        *szDst = '\0';
        return i;
    } 
    else {
        *szModuleName = '\0';
        return 0;
    }

/*
    PD3DTESTFUNCTIONDESC    pd3dtfd;
    LPTSTR                  szSrc, szDst;
    UINT                    i;

    for (pd3dtfd = (&D3D_BeginTestFunctionTable) + 1;
        pd3dtfd < &D3D_EndTestFunctionTable; pd3dtfd++)
    {
        if ((PD3DTESTFUNCTIONDESC)hModule == pd3dtfd) {
            for (szSrc = pd3dtfd->szModule, szDst = szModuleName, i = 0;
                 *szSrc && i < dwSize - 1; szSrc++, szDst++, i++)
            {
                *szDst = *szSrc;
            }
            *szDst = '\0';
            return i;
        }
    }

    *szModuleName = '\0';
    return 0;
*/

#endif // UNDER_XBOX
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
