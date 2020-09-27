/*******************************************************************************

Copyright (c) 2001 Microsoft Corporation.  All rights reserved.

File Name:

    util.cpp

Description:

    Helper functions.

*******************************************************************************/

#include <ntos.h>
#include <xapip.h>
#include <xtl.h>
#include <xgraphics.h>
#include <xgmath.h>
#include <math.h>
#include <tchar.h>
#include <stdio.h>
#include "util.h"

namespace XBOX11113 {

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
    TCHAR*  szBuffer = (LPTSTR)HeapAlloc(GetProcessHeap(), 0, 2304 * sizeof(TCHAR));
    if (!szBuffer) {
        OutputDebugString(TEXT("Insufficient memory for logging buffer allocation\n"));
        __asm int 3;
    }

    va_start(vl, szFormat);
    _vstprintf(szBuffer, szFormat, vl);
    va_end(vl);

    _tcscat(szBuffer, TEXT("\r\n"));

    OutputDebugString(szBuffer);

    HeapFree(GetProcessHeap(), 0, szBuffer);
}

#endif

//******************************************************************************
BOOL LoadResourceFile(LPCSTR szName, LPVOID* ppvData, LPDWORD pdwSize) {

    PVOID pvFile;
    BOOL bRet = FALSE;

    pvFile = XLoadSection(szName);
    if (pvFile) {
        if (ppvData) {
            *ppvData = pvFile;
        }
        if (pdwSize) {
            HANDLE hSection = XGetSectionHandle(szName);
            *pdwSize = XGetSectionSize(hSection);
        }
        bRet = TRUE;
    }

    return bRet;
}

//******************************************************************************
void FreeResourceFile(LPCSTR szName) {

    XFreeSection(szName);
}

#include <d3dx8.h>
//******************************************************************************
BOOL LoadTexture(LPDIRECT3DDEVICE8 pDevice, LPCSTR szName, LPDIRECT3DTEXTURE8* ppd3dt) {

    LPVOID pvData;
    DWORD dwSize;
    HRESULT hr;

    if (!LoadResourceFile(szName, &pvData, &dwSize)) {
        return FALSE;
    }

    hr = D3DXCreateTextureFromFileInMemoryEx(pDevice, pvData, dwSize, D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, D3DFMT_R5G6B5, 0, D3DX_FILTER_POINT, D3DX_FILTER_BOX, 0, NULL, NULL, ppd3dt);
    if (FAILED(hr)) {
        FreeResourceFile(szName);
        return FALSE;
    }

    FreeResourceFile(szName);

    return TRUE;
}

}