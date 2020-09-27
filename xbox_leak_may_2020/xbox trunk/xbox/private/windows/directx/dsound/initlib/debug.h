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

#ifndef DPF_LIBRARY
#error DPF_LIBRARY not defined
#endif // DPF_LIBRARY

#pragma warning(disable:4002)

#if defined(DEBUG) && !defined(NODPF) && !defined(USEDPF)
#define USEDPF
#endif // defined(DEBUG) && !defined(NODPF)

//
// Preprocessor hacks
//

#define QUOTE0(a)               #a
#define QUOTE1(a)               QUOTE0(a)
                                
//
// #pragma Reminders
//

#define MESSAGE(a)              message(__FILE__ "(" QUOTE1(__LINE__) "): " a)
#define TODO(a)                 MESSAGE("TODO: " a)
#define BUGBUG(a)               MESSAGE("BUGBUG: " a)
#define HACKHACK(a)             MESSAGE("HACKHACK: " a)

//
// Debug flags
//

#define DPRINTF_LIBRARY         0x00000001
#define DPRINTF_FILELINE        0x00000002
#define DPRINTF_PROCESSTHREADID 0x00000004
#define DPRINTF_FUNCTIONNAME    0x00000008

#ifndef DPRINTF_DEFAULT
#define DPRINTF_DEFAULT         (DPRINTF_LIBRARY | DPRINTF_FUNCTIONNAME)
#endif // DPRINTF_DEFAULT

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

#ifdef USEDPF

#ifndef DPFLVL_DEFAULT
#define DPFLVL_DEFAULT          DPFLVL_WARNING
#endif // DPFLVL_DEFAULT

#ifndef DPFLVL_DEFAULT_BREAK
#define DPFLVL_DEFAULT_BREAK    DPFLVL_RESOURCE
#endif // DPFLVL_DEFAULT_BREAK

EXTERN_C BOOL g_fDirectSoundBreakPoint;

#endif // USEDPF

//
// Debug API
//

#undef ASSERTMSG
#undef ASSERT
#undef BREAK

#ifdef USEDPF

EXTERN_C void __cdecl DwDbgSetContext(DWORD dwFlags, DWORD dwLevel, LPCSTR pszFile, UINT nLine, LPCSTR pszFunction, LPCSTR pszLibrary);
EXTERN_C void __cdecl DwDbgPrint(LPCSTR pszFormat, ...);
EXTERN_C void __cdecl DwDbgPrintStatic(DWORD dwFlags, DWORD dwLevel, LPCSTR pszFile, UINT nLine, LPCSTR pszFunction, LPCSTR pszLibrary, LPCSTR pszFormat, ...);
EXTERN_C void __cdecl DwDbgPrintStaticV(DWORD dwFlags, DWORD dwLevel, LPCSTR pszFile, UINT nLine, LPCSTR pszFunction, LPCSTR pszLibrary, LPCSTR pszFormat, va_list va);
EXTERN_C void __cdecl DwDbgAssert(BOOL fAssert, LPCSTR pszExpression, LPCSTR pszFile, UINT nLine);
EXTERN_C void __cdecl DwDbgBreak(void);

#define DPF_ABSOLUTE \
    DwDbgSetContext(DPRINTF_DEFAULT, DPFLVL_ABSOLUTE, __FILE__, __LINE__, DPF_FNAME, DPF_LIBRARY), \
    DwDbgPrint

#define DPF_ERROR \
    DwDbgSetContext(DPRINTF_DEFAULT, DPFLVL_ERROR, __FILE__, __LINE__, DPF_FNAME, DPF_LIBRARY), \
    DwDbgPrint

#define DPF_RESOURCE \
    DwDbgSetContext(DPRINTF_DEFAULT, DPFLVL_RESOURCE, __FILE__, __LINE__, DPF_FNAME, DPF_LIBRARY), \
    DwDbgPrint

#define DPF_WARNING \
    DwDbgSetContext(DPRINTF_DEFAULT, DPFLVL_WARNING, __FILE__, __LINE__, DPF_FNAME, DPF_LIBRARY), \
    DwDbgPrint

#define DPF_INFO \
    DwDbgSetContext(DPRINTF_DEFAULT, DPFLVL_INFO, __FILE__, __LINE__, DPF_FNAME, DPF_LIBRARY), \
    DwDbgPrint

#define DPF_BLAB \
    DwDbgSetContext(DPRINTF_DEFAULT, DPFLVL_BLAB, __FILE__, __LINE__, DPF_FNAME, DPF_LIBRARY), \
    DwDbgPrint

#define DPF_ENTERLEAVE \
    DwDbgSetContext(DPRINTF_DEFAULT, DPFLVL_ENTERLEAVE, __FILE__, __LINE__, DPF_FNAME, DPF_LIBRARY), \
    DwDbgPrint
                                
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
                                
#define DPF_ENTER()             // DPF_ENTERLEAVE("Enter")
#define DPF_LEAVE(a)            // DPF_ENTERLEAVE("Leave, returning %#x", (DWORD)(a))
#define DPF_LEAVE_VOID()        // DPF_ENTERLEAVE("Leave")

#define DPF_LEAVE_HRESULT(a)    // DPF_LEAVE(a)
#define DPF_LEAVE_WINERROR(a)   // DPF_LEAVE(a)
#define DPF_LEAVE_NTSTATUS(a)   // DPF_LEAVE(a)

#undef DPF_FNAME
#define DPF_FNAME               NULL

#endif // __DEBUG_H__
