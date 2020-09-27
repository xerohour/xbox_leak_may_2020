/*++

Copyright (c) 2001  Microsoft Corporation

Module Name:

    memory.cpp

Abstract:

    This module implements memory allocation related routines for the utility
    program to build an Xbox executable image.

--*/

#include "imgbldp.h"

LPVOID
ImgbAllocateMemory(
    SIZE_T cbBytes
    )
{
    LPVOID lpvMemory;

    lpvMemory = (LPVOID)LocalAlloc(LMEM_FIXED | LMEM_ZEROINIT, cbBytes);

    if (lpvMemory == NULL) {
        ImgbResourcePrintErrorAndExit(IDS_NO_MEMORY);
    }

    return lpvMemory;
}

VOID
ImgbFreeMemory(
    LPVOID lpMemory
    )
{
    LocalFree((HLOCAL)lpMemory);
}
