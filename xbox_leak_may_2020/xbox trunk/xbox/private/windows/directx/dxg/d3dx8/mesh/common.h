#pragma once

#ifndef __COMMON_H
#define __COMMON_H

/*/////////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 1999 Microsoft Corporation.  All Rights Reserved.
//
//  File: common.h
//
// @@BEGIN_MSINTERNAL
//
// -@- 03/17/99 (mikemarr)  - created for Luciform99
// -@- 08/23/99 (mikemarr)  - HRESULTS for library - snarfed from gxutils.h
//
// @@END_MSINTERNAL
//
/////////////////////////////////////////////////////////////////////////////*/

#include <xtl.h>
#include "d3dx8dbg.h"

#ifndef FALSE
    #define FALSE 0
#endif

#ifndef TRUE
    #define TRUE 1
#endif

// These are copied from basetsd.h
typedef signed char         INT8, *PINT8;
typedef signed short        INT16, *PINT16;
typedef signed int          INT32, *PINT32;
typedef signed __int64      INT64, *PINT64;
typedef unsigned char       UINT8, *PUINT8;
typedef unsigned short      UINT16, *PUINT16;
typedef unsigned int        UINT32, *PUINT32;
typedef unsigned __int64    UINT64, *PUINT64;


#define GXMEMALLOC(__nBytes__) HeapAlloc(GetProcessHeap(), 0, __nBytes__)
#define GXMEMFREE(__p__) HeapFree(GetProcessHeap(), 0, __p__)

#ifndef MAKE_USERERROR
    #define MAKE_USERERROR(code)    MAKE_HRESULT(1,FACILITY_ITF,code)
#endif
#ifndef E_NOTINITIALIZED
    #define E_NOTINITIALIZED        MAKE_USERERROR(0xFFFC)
#endif
#ifndef E_ALREADYINITIALIZED
    #define E_ALREADYINITIALIZED    MAKE_USERERROR(0xFFFB)
#endif
#ifndef E_NOTFOUND
    #define E_NOTFOUND              MAKE_USERERROR(0xFFFA)
#endif
#ifndef E_INSUFFICIENTDATA
    #define E_INSUFFICIENTDATA      MAKE_USERERROR(0xFFF9)
#endif


#if defined(DBG) || defined(DEBUG) || defined (_DEBUG)
    #ifndef _DEBUG
        #define _DEBUG
    #endif
    #ifndef DBG
        #define DBG 1
    #endif
#endif

#if defined(WIN32) || defined(_WIN32)
    #ifndef WIN32
        #define WIN32
    #endif
    #ifndef _WIN32
        #define _WIN32
    #endif
#endif // #if defined(WIN32) || defined(_WIN32)


#define MACSTART do {
#define MACEND } while (0)

#if defined(_WINDOWS) || defined(WIN32)
    #ifndef _INC_WINDOWS
        #ifndef WIN32_EXTRA_LEAN
            #define WIN32_EXTRA_LEAN
        #endif
        #ifndef WIN32_LEAN_AND_MEAN
            #define WIN32_LEAN_AND_MEAN
        #endif
        #include <xtl.h>
    #endif

    #ifndef _WINBASE_
        #include <winbase.h>
    #endif
    #define ISVALIDINTERFACE(__p)               ((__p) && !::IsBadReadPtr(__p, sizeof(*(__p))) && !::IsBadCodePtr((FARPROC) ((PDWORD)(__p))[0]))
    #define ISVALIDREAD(__p)                    ((__p) && !::IsBadReadPtr(__p, sizeof(*(__p))))
    #define ISVALIDWRITE(__p)                   ((__p) && !::IsBadWritePtr(__p, sizeof(*(__p))))
    #define ISVALIDREADWRITE(__p)               ((__p) && !::IsBadReadPtr(__p, sizeof(*(__p))) && !::IsBadWritePtr(__p, sizeof(*(__p))))
    #define ISRGVALIDREAD(__p, __cItems)        ((__p) && !::IsBadReadPtr(__p, (__cItems) * sizeof(*(__p))))
    #define ISRGVALIDWRITE(__p, __cItems)       ((__p) && !::IsBadWritePtr(__p, (__cItems) * sizeof(*(__p))))
    #define ISRGVALIDREADWRITE(__p, __cItems)   ((__p) && !::IsBadReadPtr(__p, (__cItems) * sizeof(*(__p))) && !::IsBadWritePtr(__p, (__cItems) * sizeof(*(__p))))

    #define ISVALIDWINDOW(__hwnd)               ((__hwnd) && ::IsWindow(__hwnd))
    #define ISVHWND(__hwnd)                     ISVALIDWINDOW(__hwnd)

    #include <winerror.h>
#else
    #define ISVALIDINTERFACE(__p)               ((__p) != NULL)
    #define ISVALIDREAD(__p)                    ((__p) != NULL)
    #define ISVALIDWRITE(__p)                   ((__p) != NULL)
    #define ISVALIDREADWRITE(__p)               ((__p) != NULL)
    #define ISRGVALIDREAD(__p, __cItems)        ((__p) != NULL)
    #define ISRGVALIDWRITE(__p, __cItems)       ((__p) != NULL)
    #define ISRGVALIDREADWRITE(__p, __cItems)   ((__p) != NULL)
#endif

#define ISVI(__p)                               ISVALIDINTERFACE(__p)
#define ISVR(__p)                               ISVALIDREAD(__p)
#define ISVW(__p)                               ISVALIDWRITE(__p)
#define ISVRW(__p)                              ISVALIDREADWRITE(__p)
#define ISRGVR(__p, __cItems)                   ISRGVALIDREAD(__p, __cItems)
#define ISRGVW(__p, __cItems)                   ISRGVALIDWRITE(__p, __cItems)
#define ISRGVRW(__p, __cItems)                  ISRGVALIDREADWRITE(__p, __cItems)


// Macro: GXRELEASE
//    Safe release for COM objects
// ***this code should never change - there is stuff that relies on the pointer being
//    set to NULL after being released
#ifndef GXRELEASE
    #define GXRELEASE(_p) MACSTART if ((_p) != NULL) {(_p)->Release(); (_p) = NULL;} MACEND
#endif
#ifndef GXULTRARELEASE
    #define GXULTRARELEASE(_p) MACSTART if (ISVI(_p)) {(_p)->Release();} (_p) = NULL; MACEND
#endif

// @@BEGIN_MSINTERNAL
#if 0
#ifndef GXDELETE
    #define GXDELETE(__ptr) MACSTART delete (__ptr); (__ptr) = NULL; MACEND
#endif
#ifndef GXULTRADELETE
    #define GXULTRADELETE(__ptr) MACSTART if (ISVW(__ptr)) { delete (__ptr);} (__ptr) = NULL; MACEND
#endif
#ifndef GXDELETERG
    #define GXDELETERG(__ptr) MACSTART delete [] (__ptr); (__ptr) = NULL; MACEND
#endif
#endif // #if 0
// @@END_MSINTERNAL

#ifndef GXCOPYI
    #define GXCOPYI(_pDstI, _pSrcI) MACSTART if ((_pDstI) = (_pSrcI)) (_pDstI)->AddRef(); MACEND
#endif
#ifndef CHECK_HR
    #define CHECK_HR(__hr) MACSTART if (FAILED(__hr)) goto e_Exit; MACEND
#endif
#ifndef CHECK_MEM
    #define CHECK_MEM(__p) MACSTART if ((__p) == NULL) { hr = E_OUTOFMEMORY; goto e_Exit; } MACEND
#endif
#ifndef CHECK_OUTPTR
    #define CHECK_OUTPTR(__p) MACSTART if (!ISVW(__p)) return E_POINTER; else *(__p) = NULL; MACEND
#endif


//
// DEBUG STUFF
//

#ifdef _DEBUG
    #include <stdio.h>
    #include <crtdbg.h>
    inline void __cdecl
        _GXTrace(const char *szFmt, ...)
    {
        char szMsgBuf[1024];
        va_list alist;
        va_start( alist, szFmt );
        _vsnprintf(szMsgBuf, 1024 - 1, szFmt, alist );
    #if defined(_WINDOWS) || defined(WIN32) || defined(_WIN32)
        // OutputDebugString(szMsgBuf);
    #endif // #if defined(_WINDOWS) || defined(WIN32) || defined(_WIN32)
        //fprintf(stderr, "%s", szMsgBuf);
        //fflush(stderr);
    }

    //#define GXASSERT(exp)       ((void) 0)
    #define GXASSERT(exp)       D3DXASSERT(exp)
    #define GXVERIFY(exp)       GXASSERT(exp)
    #define GXDEBUG_ONLY(exp)   (exp)
    #define GXTRACE             ::_GXTrace
#else // #ifdef _DEBUG
    inline void __cdecl _GXTrace(const char *szFmt, ...) {}
    #define GXASSERT(exp)       ((void) 0)
    #define GXVERIFY(exp)       ((void) (exp))
    #define GXDEBUG_ONLY(exp)   ((void) 0)
    #define GXTRACE             1 ? (void) 0 : ::_GXTrace
#endif // #else // #ifdef _DEBUG


#ifndef GXSETMAX
    #define GXSETMAX(dst, src1, src2)   MACSTART if ((src1) < (src2)) (dst) = (src2); else (dst) = (src1); MACEND
    #define GXSETMIN(dst, src1, src2)   MACSTART if ((src1) > (src2)) (dst) = (src2); else (dst) = (src1); MACEND
    #define GXUPDATEMAX(dst, src)       MACSTART if ((src) > (dst)) (dst) = (src); MACEND
    #define GXUPDATEMIN(dst, src)       MACSTART if ((src) < (dst)) (dst) = (src); MACEND
#endif

#ifndef GXISWAP
    #define GXISWAP(a,b)                MACSTART (a)^=(b); (b)^=(a); (a)^=(b); MACEND
#endif

// Macro: GXPSWAP
//    This macro swaps two pointers in place using 3 xor's.
#ifndef GXPSWAP
#define GXPSWAP(a,b,type) MACSTART \
    a = (type *)(int(a) ^ int(b)); \
    b = (type *)(int(a) ^ int(b)); \
    a = (type *)(int(a) ^ int(b)); \
MACEND
#endif


template<class T>
inline void
swap(T &a, T &b)
{
    T c(a);
    a = b;
    b = c;
}

static const int x_rgmod3[6]={0,1,2,0,1,2};
inline int MOD3(int j)
{
    GXASSERT(j>=0 && j<6);
    return x_rgmod3[j];
}


// Pointer checking
#if DBG
inline BOOL DWORD_PTR_OK(DWORD *p) {return !IsBadWritePtr(p, sizeof(DWORD));}
#else
inline BOOL DWORD_PTR_OK(DWORD *p) {return (p? TRUE:FALSE);}
#endif


// For X-file loading code
typedef HRESULT (STDAPICALLTYPE *LPDIRECTXFILECREATE)(LPDIRECTXFILE*);

#endif  // #ifndef __COMMON_H
