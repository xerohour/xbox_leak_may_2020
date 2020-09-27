/*++

Copyright (c) Microsoft Corporation

Module Name:

    debug.h

Abstract:
    
    Define Debug Macros and Declare Debug Facilities.
    
--*/

#ifndef __DMXBOX_DEBUG__
#define __DMXBOX_DEBUG__

// We use DPRINT all over the place, it is kind of silly, but we will clean it up later.
#if DBG

#include <crtdbg.h>

extern ULONG DBG_uDebugLevel;
extern void  DebugPrint(char *, ...);
#define DPRINT(level, args) if (DBG_uDebugLevel >= level) { DebugPrint args; }
#define TR_RANGESTEP    0x00000001 //Debug level for range and step

// The DM's DispatchRequest method may not be called recursively. (This
// is why you shouldn't call the EM back in the context of a DM call.)
// These macros use TLS to keep us honest by causing assertion failures in
// debug builds.
extern DWORD g_dbg_RecursionCheck;
#define DMDBG_NO_RECURSION_DECLARE  DWORD g_dbg_RecursionCheck = 0;
#define DMDBG_NO_RECURSION_ALLOC    g_dbg_RecursionCheck = TlsAlloc();
#define DMDBG_NO_RECURSION_ENTER    _ASSERT(0==TlsGetValue(g_dbg_RecursionCheck)); TlsSetValue(g_dbg_RecursionCheck, (PVOID)1);
#define DMDBG_NO_RECURSION_LEAVE    TlsSetValue(g_dbg_RecursionCheck, NULL);
#define DMDBG_NO_RECURSION_ASSERT   _ASSERT(0==TlsGetValue(g_dbg_RecursionCheck));
#define DMDBG_NO_RECURSION_FREE     TlsFree(g_dbg_RecursionCheck);

#else

#define DPRINT(level, args)
#define DMDBG_NO_RECURSION_DECLARE
#define DMDBG_NO_RECURSION_INIT
#define DMDBG_NO_RECURSION_ENTER
#define DMDBG_NO_RECURSION_LEAVE
#define DMDBG_NO_RECURSION_ASSERT
#define DMDBG_NO_RECURSION_FREE

#endif

#endif //__DMXBOX_DEBUG__