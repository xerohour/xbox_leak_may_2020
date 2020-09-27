/*************************************************************************

Copyright (C) Microsoft Corporation, 1996 - 1999

Module Name:

    Win32.h

Abstract:

    Win32-specific data structures.

Author:

    Raymond Cheng (raych)       Aug 25, 1999

Revision History:


*************************************************************************/


#ifndef _WMA_OS_WIN32_H_
#define _WMA_OS_WIN32_H_

// Verify that our target is a WIN32 machine
#ifndef WMA_OSTARGET_WIN32
#error This header file does not match the currently select OS target
#endif

#include <xtl.h>

// **************************************************************************
// COMMON FUNCTIONS
// **************************************************************************
#include "macros.h"



// **************************************************************************
// ENCODER FUNCTIONS
// **************************************************************************

#ifdef ENCODER

// Following functions are for non-serialized code only
#ifndef WMA_SERIALIZE
typedef void *HANDLE; // Stolen from WinNT.h

// Type declaration
typedef struct CAudioObjectEncoder CAudioObjectEncoder;

typedef struct THREADINFO
{
    HANDLE m_hEncEvent0, m_hEncDone0;
    HANDLE m_hEncEvent1, m_hEncDone1;
    HANDLE m_hThreadEnc0, m_hThreadEnc1;
    Bool   m_bShutdownThreads;
    Bool   m_fMultiProc;
    CAudioObjectEncoder *m_pauenc;
} THREADINFO;
#endif  // !WMA_SERIALZE
#endif  // ENCODER


// **************************************************************************
// WinCE FUNCTIONS
// **************************************************************************
// time and clock are not available in WinCE.  Provide them.

#ifdef UNDER_CE
#	ifndef _TIME_T_DEFINED
		typedef long time_t;        /* time value */
#		define _TIME_T_DEFINED     /* avoid multiple def's of time_t */
#	endif
#	ifndef _CLOCK_T_DEFINED
		typedef long clock_t;
#		define _CLOCK_T_DEFINED
#	endif
#	ifndef CLOCKS_PER_SEC
#		define CLOCKS_PER_SEC  1000
#	endif

#ifdef __cplusplus
extern "C"
#endif  // __cplusplus
	clock_t __cdecl clock(void);

#ifdef __cplusplus
extern "C"
#endif  // __cplusplus
	time_t __cdecl time(time_t * pt);

#endif

// **************************************************************************
// MACROS
// **************************************************************************

//-------------------------------------------------------
#ifdef PLATFORM_SPECIFIC_DEBUGBREAK
// Normally we do NOT provide function prototypes here, because win32 fns are
// usually confined to individual files such as win32.c. In such a case, we include
// windows.h or ansiwin.h in that file only, and save ourselves a LOT of compilation time.
// DebugBreak is widespread and therefore we break the rule and prototype it here.

#ifdef __cplusplus
extern "C"
#endif  // __cplusplus
__declspec(dllimport) void __stdcall DebugBreak(void); // From winbase.h

#define DEBUG_BREAK()   DebugBreak()
#endif  // PLATFORM_SPECIFIC_DEBUGBREAK


#endif  // _WMA_OS_WIN32_H_
