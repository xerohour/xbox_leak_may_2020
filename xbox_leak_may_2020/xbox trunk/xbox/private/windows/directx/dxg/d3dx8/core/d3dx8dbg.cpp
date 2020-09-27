///////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 1999 Microsoft Corporation.  All Rights Reserved.
//
//  File:       d3dx8dbg.cpp
//  Content:    D3DX debugging functions
//
///////////////////////////////////////////////////////////////////////////

#include "pchcore.h"
#if DBG


//
// DPF
//

void cdecl D3DXDebugPrintf(UINT lvl, LPSTR szFormat, ...)
{
    char strA[256];
    char strB[256];

//    if(lvl > (UINT) g_dwDebugLevel)
//        return;

    va_list ap;
    va_start(ap, szFormat);
    _vsnprintf(strA, sizeof(strA), szFormat, ap);
    strA[255] = '\0';
    va_end(ap);

    _snprintf(strB, sizeof(strB), "D3DX: %s\r\n", strA);
    strB[255] = '\0';

    OutputDebugStringA(strB);
}


//
// DPFHR
//

void cdecl D3DXDebugPrintfHR(UINT lvl, HRESULT hr, LPSTR szFormat, ...)
{
    char strA[256];
    char strB[256];

    va_list ap;
    va_start(ap, szFormat);
    _vsnprintf(strA, sizeof(strA), szFormat, ap);
    strA[255] = '\0';
    va_end(ap);

    D3DXGetErrorStringA(hr, strB, sizeof(strB));
    D3DXDebugPrintf(lvl, "%s: %s", strA, strB);
}



//
// D3DXASSERT
//

int WINAPI D3DXDebugAssert(LPCSTR szFile, int nLine, LPCSTR szCondition)
{
	return 0;
}


#endif // DBG