/*++

Copyright (c) 1999

Module Name:

    debug.h

Abstract:

    Macros used for debugging purposes

Revision History:

    12/16/1999 davidx
        Created it.

--*/

#ifndef _DEBUG_H
#define _DEBUG_H

#include <xdbg.h>

//
// These macros are used for debugging purposes.
// They expand to nop on a free build.
//
#ifndef XNET_DEBUG_COMPILE_LEVEL
#define XNET_DEBUG_COMPILE_LEVEL XDBG_COMPILE_LEVEL
#endif

#if DBG && (XNET_DEBUG_COMPILE_LEVEL >= XDBG_WARNING)
extern VOID XnetDbgWarn(CHAR* format, ...);
#define WARNING_ XnetDbgWarn
#else 
#define WARNING_ 1 ? (void)0 : (void)
#endif

#if DBG && (XNET_DEBUG_COMPILE_LEVEL >= XDBG_TRACE)
extern VOID XnetDbgVerbose(CHAR* format, ...);
#define VERBOSE_ XnetDbgVerbose
#else 
#define VERBOSE_ 1 ? (void)0 : (void)
#endif

#if DBG && (XNET_DEBUG_COMPILE_LEVEL >= XDBG_ENTRY)
extern VOID XnetDbgTrace(CHAR* format, ...);
#define TRACE_ XnetDbgTrace
#else 
#define TRACE_ 1 ? (void)0 : (void)
#endif

#if DBG

#define ASSERT_DISPATCH_LEVEL() \
        ASSERT(KeGetCurrentIrql() == DISPATCH_LEVEL)

//
// This macro is used as a reminder that a particular function
// needs to run at DISPATCH_LEVEL.
//
#define RUNS_AT_DISPATCH_LEVEL

#else // !DBG

#define ASSERT_DISPATCH_LEVEL()
#define RUNS_AT_DISPATCH_LEVEL

#endif // !DBG

#endif // !_DEBUG_H
