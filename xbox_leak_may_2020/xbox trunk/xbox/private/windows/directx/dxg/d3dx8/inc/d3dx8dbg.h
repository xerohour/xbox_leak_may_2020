///////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 1999 Microsoft Corporation.  All Rights Reserved.
//
//  File:       d3dx8dbg.h
//  Content:    D3DX debugging functions
//
///////////////////////////////////////////////////////////////////////////


#ifndef __D3DX8DBG_H__
#define __D3DX8DBG_H__

#include <xdbg.h>

#if DBG
#define SURFACE_PORT_WARNING() \
{ \
    static BOOL bWarned = FALSE; \
    if (!bWarned) { \
        XDebugWarning("D3DX8", "Loading from resource is more efficient than using D3DX"); \
        XDebugWarning("D3DX8", "See bundler tool in XDK for examples on how to do it"); \
        bWarned = TRUE; \
    } \
}
#else
#define SURFACE_PORT_WARNING()
#endif

//
// DPF
//

#if DBG

    void cdecl D3DXDebugPrintf(UINT lvl, LPSTR szFormat, ...);
    void cdecl D3DXDebugPrintfHR(UINT lvl, HRESULT hr, LPSTR szFormat, ...);

    #define DPF D3DXDebugPrintf
    #define DPFHR D3DXDebugPrintfHR

#else // !DBG

    #pragma warning(disable:4002)
    #define DPF()
    #define DPFHR()

#endif // !DBG




//
// D3DXASSERT
//

#if DBG

    int WINAPI D3DXDebugAssert(LPCSTR szFile, int nLine, LPCSTR szCondition);

    #define D3DXASSERT(condition) \
        do { if(!(condition) && D3DXDebugAssert(__FILE__, __LINE__, #condition)) DebugBreak(); } while(0)

#else // !DBG

    #define D3DXASSERT(condition) 0

#endif // !DBG


//
// Memory leak checking
//
#ifdef DBG
//#define MEM_DEBUG
#endif

#ifdef MEM_DEBUG

    BOOL WINAPI D3DXDumpUnfreedMemoryInfo();
    void *operator new(size_t stAllocateBlock, const UINT32 uiLineNumber, const char *szFilename);
    void operator delete(void *pvMem, size_t stAllocateBlock, const UINT32 uiLineNumber, const char *szFilename);
    //void operator delete( void *pvMem );


#define new new(__LINE__, __FILE__)

#else // !DBG

    BOOL WINAPI D3DXDumpUnfreedMemoryInfo();

#define New new

#endif // !DBG

#endif // __D3DX8DBG_H__
