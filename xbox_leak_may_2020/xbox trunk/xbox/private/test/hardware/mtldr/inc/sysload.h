/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    sysload.h

Abstract:

    Prototypes  of routines used to load EXEs into the system portion of
    the address space and calls the EXE at its initialization entry point.

--*/

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef _MTLDR_SYSLOAD

#define _MTLDR_SYSLOAD

#ifdef __cplusplus
extern "C" {
#endif

VOID
NTAPI
LdrInitializeSystemLoader(
    PVOID KernelLoaderBlock
    );

ULONG
NTAPI
LdrLoadSystemImage(
    IN PVOID ImageBuffer,
    IN PCSTR ImageFileName
    );

#define ALIGN_DOWN_POINTER(address, type) \
    ((PVOID)((ULONG_PTR)(address) & ~((ULONG_PTR)sizeof(type) - 1)))

#define ALIGN_UP_POINTER(address, type) \
    (ALIGN_DOWN_POINTER(((ULONG_PTR)(address) + sizeof(type) - 1), type))

#ifdef __cplusplus
}
#endif

#endif  // _MTLDR_SYSLOAD
