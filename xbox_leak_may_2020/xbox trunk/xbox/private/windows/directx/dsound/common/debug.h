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

#undef ASSERTMSG
#undef ASSERT
#undef BREAK

#pragma warning(disable:4002)

#if defined(DEBUG) && !defined(USEDPF)
#define USEDPF
#endif // USEDPF

#ifndef DPF_LIBRARY
#error DPF_LIBRARY not defined
#endif // DPF_LIBRARY

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
                                
BEGIN_DEFINE_STRUCT()
    DWORD   dwFlags;
    DWORD   dwLevel;
    LPCSTR  pszFile;
    UINT    nLine;
    LPCSTR  pszFunction;
    LPCSTR  pszLibrary;
END_DEFINE_STRUCT(DPFCONTEXT);

typedef void (CALLBACK *LPFNDPFCALLBACK)(DWORD dwLevel, LPCSTR pszString);

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

#ifdef _XBOX
#define DEBUG_CONTEXT_COUNT     (HIGH_LEVEL + 1)
#else // _XBOX
#define DEBUG_CONTEXT_COUNT     1
#endif // _XBOX

#define DEBUGLOG_ENTRY_LENGTH   64
#define DEBUGLOG_ENTRY_COUNT    1024

#ifdef __cplusplus

//
// Debug object
//

namespace DirectSound
{
    class CDebug
    {
    public:

#ifdef USEDPF

#ifdef BUILDING_DSOUND

        static DWORD &          m_dwDpfLevel;
        static DWORD &          m_dwDpfBreakLevel;
        static BOOL &           m_fDebugBreak;

#else // BUILDING_DSOUND

        static DWORD            m_dwDpfLevel;
        static DWORD            m_dwDpfBreakLevel;
        static BOOL             m_fDebugBreak;

#endif // BUILDING_DSOUND

        static LPFNDPFCALLBACK  m_pfnDpfCallback;

#endif // USEDPF

    protected:

#ifdef USEDPF

        static DPFCONTEXT       m_aContext[DEBUG_CONTEXT_COUNT];

#endif // USEDPF

#ifdef USEDEBUGLOG

        static CHAR             m_aszLog[DEBUGLOG_ENTRY_COUNT][DEBUGLOG_ENTRY_LENGTH];
        static DWORD            m_dwLogIndex;

#endif // USEDEBUGLOG

    public:

#ifdef USEDPF
        
        // DPF functions
        static void __cdecl SetContext(DWORD dwFlags, DWORD dwLevel, LPCSTR pszFile, UINT nLine, LPCSTR pszFunction, LPCSTR pszLibrary);
        static void __cdecl Print(LPCSTR pszFormat, ...);
        static void __cdecl PrintStatic(DWORD dwFlags, DWORD dwLevel, LPCSTR pszFile, UINT nLine, LPCSTR pszFunction, LPCSTR pszLibrary, LPCSTR pszFormat, ...);
        static void __cdecl PrintStaticV(DWORD dwFlags, DWORD dwLevel, LPCSTR pszFile, UINT nLine, LPCSTR pszFunction, LPCSTR pszLibrary, LPCSTR pszFormat, va_list va);
        static void __cdecl Assert(BOOL fAssert, LPCSTR pszExpression, LPCSTR pszFile, UINT nLine);
        static void __cdecl Break(void);

#endif // USEDPF

#ifdef USEDEBUGLOG

        // Debug log functions
        static void __cdecl Log(LPCSTR pszFormat, ...);

#endif // USEDEBUGLOG

    };
};

#ifdef USEDPF

#define DPF_ABSOLUTE \
    DirectSound::CDebug::SetContext(DPF_FLAGS_DEFAULT, DPFLVL_ABSOLUTE, __FILE__, __LINE__, DPF_FNAME, DPF_LIBRARY), \
    DirectSound::CDebug::Print

#define DPF_ERROR \
    DirectSound::CDebug::SetContext(DPF_FLAGS_DEFAULT, DPFLVL_ERROR, __FILE__, __LINE__, DPF_FNAME, DPF_LIBRARY), \
    DirectSound::CDebug::Print

#define DPF_RESOURCE \
    DirectSound::CDebug::SetContext(DPF_FLAGS_DEFAULT, DPFLVL_RESOURCE, __FILE__, __LINE__, DPF_FNAME, DPF_LIBRARY), \
    DirectSound::CDebug::Print

#define DPF_WARNING \
    DirectSound::CDebug::SetContext(DPF_FLAGS_DEFAULT, DPFLVL_WARNING, __FILE__, __LINE__, DPF_FNAME, DPF_LIBRARY), \
    DirectSound::CDebug::Print

#define DPF_INFO \
    DirectSound::CDebug::SetContext(DPF_FLAGS_DEFAULT, DPFLVL_INFO, __FILE__, __LINE__, DPF_FNAME, DPF_LIBRARY), \
    DirectSound::CDebug::Print

#define DPF_BLAB \
    DirectSound::CDebug::SetContext(DPF_FLAGS_DEFAULT, DPFLVL_BLAB, __FILE__, __LINE__, DPF_FNAME, DPF_LIBRARY), \
    DirectSound::CDebug::Print

#ifdef DPF_ENABLE_ENTERLEAVE

#define DPF_ENTERLEAVE \
    DirectSound::CDebug::SetContext(DPF_FLAGS_DEFAULT, DPFLVL_ENTERLEAVE, __FILE__, __LINE__, DPF_FNAME, DPF_LIBRARY), \
    DirectSound::CDebug::Print

#else // DPF_ENTERLEAVE
                                
#define DPF_ENTERLEAVE(a)

#endif // DPF_ENTERLEAVE
                                
#define ASSERT(a) \
    DirectSound::CDebug::Assert(!(a), #a, __FILE__, __LINE__)

#define ASSERTMSG(a) \
    DirectSound::CDebug::Assert(TRUE, a, __FILE__, __LINE__)

#define BREAK() \
    DirectSound::CDebug::Break()

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

#ifdef USEDEBUGLOG

#define DEBUGLOG DirectSound::CDebug::Log

#else // USEDEBUGLOG

#define DEBUGLOG(a)

#endif // USEDEBUGLOG

#endif // __cplusplus

#endif // __DEBUG_H__
