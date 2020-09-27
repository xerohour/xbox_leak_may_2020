/***************************************************************************
 *
 *  Copyright (C) 1995-1998 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       debug.h
 *  Content:    Debugger helper functions.
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  1/21/97     dereks  Created.
 *
 ***************************************************************************/

#ifndef __DEBUG_H__
#define __DEBUG_H__

#include "macros.h"

#if defined(DEBUG) && !defined(USEDPF)
#define USEDPF
#endif // USEDPF

//
// Preprocessor hacks
//

#define QUOTE0(a) \
    #a

#define QUOTE1(a) \
    QUOTE0(a)

//
// DPF definitions
//
                                
#ifndef DPF_LIBRARY
#error DPF_LIBRARY not defined
#endif // DPF_LIBRARY

BEGIN_DEFINE_STRUCT()
    DWORD   dwFlags;
    DWORD   dwLevel;
    LPCSTR  pszFile;
    UINT    nLine;
    LPCSTR  pszFunction;
    LPCSTR  pszLibrary;
END_DEFINE_STRUCT(DPFCONTEXT);

typedef void (CALLBACK *LPFNDPFCALLBACK)(DWORD dwLevel, LPCSTR pszString);

EXTERN_C DWORD g_dwXactDebugLevel;
EXTERN_C DWORD g_dwXactDebugBreakLevel;

EXTERN_C DPFCONTEXT g_XactDebugContext[HIGH_LEVEL + 1];
EXTERN_C BOOL g_fXactDebugBreak;


#pragma warning(disable:4002)

//
// #pragma Reminders
//

#define MESSAGE(msg) \
    message(__FILE__ "(" QUOTE1(__LINE__) "): " msg)

#define TODO(msg) \
    MESSAGE("TODO: " msg)

#define BUGBUG(msg) \
    MESSAGE("BUGBUG: " msg)

#define HACKHACK(msg) \
    MESSAGE("HACKHACK: " msg)

//
// Debug flags
//

#define DPF_FLAGS_LIBRARY           0x00000001
#define DPF_FLAGS_FILELINE          0x00000002
#define DPF_FLAGS_PROCESSTHREADID   0x00000004
#define DPF_FLAGS_FUNCTIONNAME      0x00000008

#ifndef DPF_FLAGS_DEFAULT
#ifdef _XBOX
#define DPF_FLAGS_DEFAULT           (DPF_FLAGS_LIBRARY | DPF_FLAGS_FUNCTIONNAME)
#else // _XBOX
#define DPF_FLAGS_DEFAULT           (DPF_FLAGS_LIBRARY | DPF_FLAGS_FUNCTIONNAME)
#endif // _XBOX
#endif // DPF_FLAGS_DEFAULT

//
// Debug levels
//

#define DPFLVL_ABSOLUTE         0   // Disregard level
#define DPFLVL_ERROR            1   // Errors
#define DPFLVL_RESOURCE         2   // Resource allocation failures
#define DPFLVL_WARNING          3   // Warnings
#define DPFLVL_INFO             4   // General info
#define DPFLVL_BLAB             5   // Mostly useless info
#define DPFLVL_ENTERLEAVE       9   // Function enter/leave

#define DPFLVL_FIRST            DPFLVL_ABSOLUTE
#define DPFLVL_LAST             DPFLVL_ENTERLEAVE

#ifndef DPFLVL_DEFAULT
#define DPFLVL_DEFAULT          DPFLVL_WARNING
#endif // DPFLVL_DEFAULT

#ifndef DPFLVL_DEFAULT_BREAK
#define DPFLVL_DEFAULT_BREAK    DPFLVL_RESOURCE
#endif // DPFLVL_DEFAULT_BREAK

//
// Debug API
//

#undef ASSERTMSG
#undef ASSERT
#undef BREAK

#ifdef USEDPF

void __cdecl DwDbgSetContext(DWORD dwFlags, DWORD dwLevel, LPCSTR pszFile, UINT nLine, LPCSTR pszFunction, LPCSTR pszLibrary);
void __cdecl DwDbgPrint(LPCSTR pszFormat, ...);
void __cdecl DwDbgPrintStatic(DWORD dwFlags, DWORD dwLevel, LPCSTR pszFile, UINT nLine, LPCSTR pszFunction, LPCSTR pszLibrary, LPCSTR pszFormat, ...);
void __cdecl DwDbgPrintStaticV(DWORD dwFlags, DWORD dwLevel, LPCSTR pszFile, UINT nLine, LPCSTR pszFunction, LPCSTR pszLibrary, LPCSTR pszFormat, va_list va);
void __cdecl DwDbgAssert(BOOL fAssert, LPCSTR pszExpression, LPCSTR pszFile, UINT nLine);
void __cdecl DwDbgBreak(void);

#define DPF_ABSOLUTE \
    DwDbgSetContext(DPF_FLAGS_DEFAULT, DPFLVL_ABSOLUTE, __FILE__, __LINE__, DPF_FNAME, DPF_LIBRARY), \
    DwDbgPrint

#define DPF_ERROR \
    DwDbgSetContext(DPF_FLAGS_DEFAULT, DPFLVL_ERROR, __FILE__, __LINE__, DPF_FNAME, DPF_LIBRARY), \
    DwDbgPrint

#define DPF_RESOURCE \
    DwDbgSetContext(DPF_FLAGS_DEFAULT, DPFLVL_RESOURCE, __FILE__, __LINE__, DPF_FNAME, DPF_LIBRARY), \
    DwDbgPrint

#define DPF_WARNING \
    DwDbgSetContext(DPF_FLAGS_DEFAULT, DPFLVL_WARNING, __FILE__, __LINE__, DPF_FNAME, DPF_LIBRARY), \
    DwDbgPrint

#define DPF_INFO \
    DwDbgSetContext(DPF_FLAGS_DEFAULT, DPFLVL_INFO, __FILE__, __LINE__, DPF_FNAME, DPF_LIBRARY), \
    DwDbgPrint

#define DPF_BLAB \
    DwDbgSetContext(DPF_FLAGS_DEFAULT, DPFLVL_BLAB, __FILE__, __LINE__, DPF_FNAME, DPF_LIBRARY), \
    DwDbgPrint

#ifdef DPF_ENABLE_ENTERLEAVE

#define DPF_ENTERLEAVE \
    DwDbgSetContext(DPF_FLAGS_DEFAULT, DPFLVL_ENTERLEAVE, __FILE__, __LINE__, DPF_FNAME, DPF_LIBRARY), \
    DwDbgPrint

#else // DPF_ENTERLEAVE
                                
#define DPF_ENTERLEAVE(a)

#endif // DPF_ENTERLEAVE
                                
#define ASSERT(a) \
    DwDbgAssert(!(a), #a, __FILE__, __LINE__)

#define ASSERTMSG(a) \
    DwDbgAssert(TRUE, a, __FILE__, __LINE__)

#define BREAK() \
    DwDbgBreak()

#else // USEDPF

#define DPF_ABSOLUTE(a)
#define DPF_ERROR(a)
#define DPF_RESOURCE(a)
#define DPF_WARNING(a)
#define DPF_INFO(a)
#define DPF_BLAB(a)
#define DPF_ENTERLEAVE(a)

#define ASSERT(a)
#define ASSERTMSG(a)
#define BREAK()

#endif // USEDPF
                                
#define DPF_ENTER()             DPF_ENTERLEAVE("Enter")
#define DPF_LEAVE(a)            DPF_ENTERLEAVE("Leave, returning %#x", (DWORD)(a))
#define DPF_LEAVE_VOID()        DPF_ENTERLEAVE("Leave")

#define DPF_LEAVE_HRESULT(a)    DPF_LEAVE(a)
#define DPF_LEAVE_WINERROR(a)   DPF_LEAVE(a)
#define DPF_LEAVE_NTSTATUS(a)   DPF_LEAVE(a)

#undef DPF_FNAME
#define DPF_FNAME               NULL

//
// Debug log
//

#ifdef USEDEBUGLOG

#define DLOG_ENTRY_LENGTH   64
#define DLOG_ENTRY_COUNT    1024

EXTERN_C CHAR g_aszDirectSoundDebugLog[DLOG_ENTRY_COUNT][DLOG_ENTRY_LENGTH];
EXTERN_C DWORD g_dwDirectSoundDebugLogIndex;

EXTERN_C void __cdecl DwDbgLog(LPCSTR pszFormat, ...);

#define DLOG DwDbgLog

#else // USEDEBUGLOG

#define DLOG(a)

#endif // USEDEBUGLOG

#endif // __DEBUG_H__
