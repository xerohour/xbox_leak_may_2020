/*--

Copyright (c) 1992  Microsoft Corporation

Module Name:

    precomp.h

Abstract:

    Header file that is pre-compiled into a .pch file

Author:

    Wesley Witt (wesw) 21-Sep-1993

Environment:

    Win32, User Mode

--*/
#undef UNICODE
#undef _UNICODE
#include <nt.h>
#include <ntrtl.h>
#include <nturtl.h>
#include <ntdbg.h>
#include <windows.h>
#ifdef XBOX
#include <windbapi.h>
#include <xbdm.h>
#include <ctx386.h>
#else
#include <winsock.h>
#define DMSOCKETS
#include <xboxdbg.h>
#endif

#if 0
#undef DBG
#undef NDEBUG
#ifdef VCDBG
#define DBG 1
#else
#define NDEBUG
#endif // VCDBG
#endif

#if !defined(DBG) && !defined(NDEBUG)
#define NDEBUG
#endif

#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>

#include <od.h>
#include <odp.h>
#include <cvinfo.h>
#include <emdm.h>
#include <win32dm.h>

#ifdef XBOX
#include <xbdm.h>
#else
#include <winsock.h>
#define DMSOCKETS
#include <xboxdbg.h>
#endif

#include "dm.h"
#include "list.h"
#include "bp.h"
#include "funccall.h"
#include "debug.h"
#include "dbgver.h"
#ifndef XBOX
#include "resource.h"
#endif

/* Here are the functions we want to prevent from working */
#define NOWIN32
#ifdef NOWIN32
#define WaitForDebugEvent()
#define ContinueDebugEvent()
#define CreateProcessA()
#define CreateProcessW()
#define OpenProcess()
#define GetExitCodeThread()
#define DebugActiveProcess()
#define GetThreadContext()
#define GetThreadSelectorEntry()
#define ReadProcessMemory()
#define SetThreadContext()
#define SuspendThread()
#define ResumeThread()
#endif

#ifdef XBOX
#define malloc(cb) ExAllocatePoolWithTag(PagedPool, cb, 'cvsm')
#define free(p) ExFreePoolWithTag(p, 'cvsm')
#define GetTickCount NtGetTickCount

#define assert(f) ASSERT(f)
// no spew until we have spew functions
#define OutputDebugStringA(str)
#endif

extern LPDM_MSG LpDmMsg;
