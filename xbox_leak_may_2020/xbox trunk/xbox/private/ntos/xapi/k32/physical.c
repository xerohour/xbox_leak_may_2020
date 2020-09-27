/*++

Copyright (c) 2001  Microsoft Corporation

Module Name:

    physical.c

Abstract:

    This module implements the XBOX physical memory management services.

--*/

#include "basedll.h"
#pragma hdrstop

LPVOID
WINAPI
XPhysicalAlloc(
    IN SIZE_T dwSize,
    IN ULONG_PTR ulPhysicalAddress,
    IN ULONG_PTR ulAlignment,
    IN DWORD flProtect
    )
{
    ULONG ulLowestAcceptableAddress;
    ULONG ulHighestAcceptableAddress;
    LPVOID lpAddress;

    RIP_ON_NOT_TRUE("XPhysicalAlloc()", (dwSize != 0));
    RIP_ON_NOT_TRUE("XPhysicalAlloc()", ((ulAlignment & (ulAlignment - 1)) == 0));
    RIP_ON_NOT_TRUE("XPhysicalAlloc()",
        ((flProtect & ~(PAGE_READWRITE | PAGE_READONLY | PAGE_WRITECOMBINE | PAGE_NOCACHE)) == 0));

    if (ulPhysicalAddress != MAXULONG_PTR) {

        RIP_ON_NOT_TRUE("XPhysicalAlloc()", (BYTE_OFFSET(ulPhysicalAddress) == 0));

        ulLowestAcceptableAddress = ulPhysicalAddress;
        ulHighestAcceptableAddress = ulPhysicalAddress + dwSize - 1;

        //
        // Ignore the alignment parameter; the caller gave us a physical address
        // and we assume it has the appropriate alignment already applied.
        //

        ulAlignment = 0;

    } else {
        ulLowestAcceptableAddress = 0;
        ulHighestAcceptableAddress = MAXULONG_PTR;
    }

    lpAddress = MmAllocateContiguousMemoryEx(dwSize, ulLowestAcceptableAddress,
        ulHighestAcceptableAddress, ulAlignment, flProtect);

    if (lpAddress == NULL) {
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
    }

    return lpAddress;
}

SIZE_T
WINAPI
XPhysicalSize(
    IN LPVOID lpAddress
    )
{
    RIP_ON_NOT_TRUE("XPhysicalSize()", ((ULONG_PTR)lpAddress >= 0x80000000));

    return MmQueryAllocationSize(lpAddress);
}

VOID
WINAPI
XPhysicalProtect(
    IN LPVOID lpAddress,
    IN SIZE_T dwSize,
    IN DWORD flNewProtect
    )
{
    RIP_ON_NOT_TRUE("XPhysicalProtect()", ((ULONG_PTR)lpAddress >= 0x80000000));
    RIP_ON_NOT_TRUE("XPhysicalProtect()",
        ((flNewProtect & ~(PAGE_READWRITE | PAGE_READONLY | PAGE_WRITECOMBINE | PAGE_NOCACHE)) == 0));

    if (dwSize != 0) {
        MmSetAddressProtect(lpAddress, dwSize, flNewProtect);
    }
}

VOID
WINAPI
XPhysicalFree(
    IN LPVOID lpAddress
    )
{
    RIP_ON_NOT_TRUE("XPhysicalFree()", ((ULONG_PTR)lpAddress >= 0x80000000));

    MmFreeContiguousMemory(lpAddress);
}

DWORD
WINAPI
XQueryMemoryProtect(
    IN LPVOID lpAddress
    )
{
    return MmQueryAddressProtect(lpAddress);
}
