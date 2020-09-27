/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    debug.h

Abstract:

    This module contains various debugging macros used by mtldr

--*/

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef _MTLDR_DEBUG

#define _MTLDR_DEBUG

#ifdef __cplusplus
extern "C" {
#endif

ULONG
_cdecl
DbgPrint(
    PCH Format,
    ...
    );

#if defined(DBG) || defined(_DEBUG)

#define MtDbgPrint(_x_)     DbgPrint _x_
#define MtDbgBreak()        __asm int 3

#else   // DBG

#define MtDbgPrint(_x_)
#define MtDbgBreak()

#endif  // DBG

#if defined(DBG) && !defined(_DEBUG)
#define _DEBUG
#endif

#ifdef __cplusplus
}
#endif

#endif  // _MTLDR_DEBUG
