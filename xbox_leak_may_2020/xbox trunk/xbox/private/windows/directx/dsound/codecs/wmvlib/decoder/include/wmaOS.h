/*************************************************************************

Copyright (C) Microsoft Corporation, 1996 - 1999

Module Name:

    WmaOS.h

Abstract:

    OS-specific data types and defines.

Author:

    Raymond Cheng (raych)       Aug 25, 1999

Revision History:


*************************************************************************/


#ifndef _WMA_OS_H_
#define _WMA_OS_H_


// Specify the OS target here


#if defined(_XBOX)
#define WMA_OSTARGET_XBOX
#elif  defined(_Embedded_x86)
#define WMA_OSTARGET_LINUX
#elif defined(S_SUNOS5)
#define WMA_OSTARGET_SUNOS5
#elif defined(__QNX__)
#define WMA_OSTARGET_QNX
#elif defined(_WIN32) 
#ifndef macintosh
#define WMA_OSTARGET_WIN32
#endif 
#endif





// **************************************************************************
// OS-Specific #ifdefs
// **************************************************************************

// ======================================================
// Win32 OS
// ======================================================
#if defined(WMA_OSTARGET_WIN32)

#ifdef COMMONMACROS_OUTPUT_TARGET
#pragma COMPILER_MESSAGE("-----WMAOS.H: Compiling for Win32 OS")
#endif // COMMONMACROS_OUTPUT_TARGET

#define _SCRUNCH_INLINE_ASM
#define PLATFORM_SPECIFIC_OUTPUTDEBUGSTRING
#define PLATFORM_SPECIFIC_PERFTIMER
#define PLATFORM_SPECIFIC_DEBUGINFRASTRUCTURE

#ifndef DISABLE_ASSERTS
#define PLATFORM_SPECIFIC_DEBUGBREAK

/* CE x86em platform does not have DebugBreak */
#ifdef _WIN32_WCE
#undef PLATFORM_SPECIFIC_DEBUGBREAK
#endif  // _WIN32_WCE
#endif  // DISABLE_ASSERTS

#ifdef UNDER_CE
// avoid threading on WinCE 
// TODO: verify if it can be used and how it effects performance)
#define WMA_SERIALIZE 
#endif

#include "win32.h"


// ======================================================
// Generic OS
// ======================================================
#else

#ifdef COMMONMACROS_OUTPUT_TARGET
#pragma COMPILER_MESSAGE("-----WMAOS.H: Compiling for GENERIC OS")
#endif // COMMONMACROS_OUTPUT_TARGET

#define WMA_SERIALIZE // No threading services are available

#endif // WMA_OSTARGET_WIN32

#ifdef WMA_OSTARGET_XBOX
#undef PLATFORM_SPECIFIC_DEBUGBREAK
#endif

// **************************************************************************
// Common to All OS's
// **************************************************************************

// ----- Performance Timer Functions -----
#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus
typedef struct tagPERFTIMERINFO PERFTIMERINFO;

PERFTIMERINFO *PerfTimerNew(long lSamplesPerSecOutput);
void PerfTimerStart(PERFTIMERINFO *pInfo);
void PerfTimerStop(PERFTIMERINFO *pInfo, long lSamplesDecoded);
void PerfTimerStopElapsed(PERFTIMERINFO *pInfo);
void PerfTimerReport(PERFTIMERINFO *pInfo);
float fltPerfTimerDecodeTime(PERFTIMERINFO *pInfo);
void PerfTimerFree(PERFTIMERINFO *pInfo);
#ifdef __cplusplus
}
#endif  // __cplusplus

// ----- Debug Infrastructure Functions and Macros -----
void TraceInfo(const char *pszFmt, ...);

#ifdef _DEBUG
#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus
void TraceInfoHelper(const char *pszFile, int iLine, const char *pszFmt, ...);
WMARESULT MyOutputDbgStr(WMARESULT wmaReturn, const char *pszFmt, ...);
#ifdef __cplusplus
}
#endif  // __cplusplus

// raych: The __FILE__ and __LINE__ might cause cross-platform compiler problems. Fix later...
#define TraceInfo0(text)            TraceInfoHelper(__FILE__, __LINE__, text)
#define TraceInfo1(text,one)        TraceInfoHelper(__FILE__, __LINE__, text, one)
#define TraceInfo2(text,one,two)    TraceInfoHelper(__FILE__, __LINE__, text, one, two)
#define TraceInfo3(text,one,two,three)                                      \
    TraceInfoHelper(__FILE__, __LINE__, text, one, two, three)

#define TraceInfo4(text,one,two,three,four)                                 \
    TraceInfoHelper(__FILE__, __LINE__, text, one, two, three, four)

#define TraceInfo5(text,one,two,three,four,five)                            \
    TraceInfoHelper(__FILE__, __LINE__, text, one, two, three, four, five)

#ifdef WANT_TRACE_ONHOLD
// Tracing ONHOLD for decapp produces too many benign error messages.  But Wei-ge assures us that
// for wmadec_s, it can be useful.  If you want it, define the symbol above.
#define TraceResult(code)   MyOutputDbgStr((code), "%s(%d) : *** TRACE *** code = 0x%x!\n", \
                                    __FILE__, __LINE__, (code))
#else   // WANT_TRACE_ONHOLD
#define TraceResult(code)   ( ((code)==WMA_E_ONHOLD || (code)==WMA_OK) ?  (code) :              \
                                MyOutputDbgStr((code), "%s(%d) : *** TRACE *** code = 0x%x!\n", \
                                    __FILE__, __LINE__, (code)) )
#endif  // WANT_TRACE_ONHOLD

#define TraceError(code)    {   /* wmaFOOFOO to avoid naming collisions */                      \
                                WMARESULT wmaFOOFOO = (code);                                   \
                                if (WMA_FAILED(wmaFOOFOO))                                      \
                                    TraceResult(wmaFOOFOO);                                     \
                            }

#else   // _DEBUG
#define TraceInfo0(text)
#define TraceInfo1(text,one)
#define TraceInfo2(text,one,two)
#define TraceResult(code)   (code)
#define TraceError(code)
#endif  // _DEBUG

#define TRACEWMA_EXIT(wmaResult, action) \
                            (wmaResult) = (action);                                             \
                            if (WMA_FAILED(wmaResult))                                          \
                            {                                                                   \
                                TraceResult(wmaResult);                                         \
                                goto exit;                                                      \
                            } else {}
    
#define ASSERTWMA_EXIT(wmaResult, action) \
                            (wmaResult) = (action);                                             \
                            if (WMA_FAILED(wmaResult))                                          \
                            {                                                                   \
                                DEBUG_ONLY(DEBUG_BREAK()); /* assert(WMAB_FALSE); */            \
                                TraceResult(wmaResult);                                         \
                                goto exit;                                                      \
                            } else {}

#define CHECKWMA_EXIT(wmaResult) \
                            if (WMA_FAILED(wmaResult))                                          \
                            {                                                                   \
                                TraceResult(wmaResult);                                         \
                                goto exit;                                                      \
                            } else {}

#define TRACEWMA_RET(wmaResult, action) \
                            (wmaResult) = (action);                                             \
                            if (WMA_FAILED(wmaResult))                                          \
                            {                                                                   \
                                return TraceResult(wmaResult);                                  \
                            } else {}


// **************************************************************************
// Overridable Macros (optimized for some platforms, but not all)
// **************************************************************************
// The following can be overridden within a particular platform OS file

//---------------------------------------------------------------------------
#ifndef PLATFORM_SPECIFIC_OUTPUTDEBUGSTRING
#define OUTPUT_DEBUG_STRING(sz) printf(sz)
#endif  // PLATFORM_SPECIFIC_OUTPUTDEBUGSTRING

//---------------------------------------------------------------------------
#if defined(_DEBUG) && !defined(DISABLE_ASSERTS)
#ifndef PLATFORM_SPECIFIC_DEBUGBREAK
#define DEBUG_BREAK()   *((U32 *)0) = 55    // Brute-force debug break
#endif // PLATFORM_SPECIFIC_DEBUGBREAK
#else   // defined(_DEBUG) && !defined(DISABLE_ASSERTS)
#undef DEBUG_BREAK
#define DEBUG_BREAK()
#endif  // defined(_DEBUG) && !defined(DISABLE_ASSERTS)

#endif // _WMA_OS_H_

