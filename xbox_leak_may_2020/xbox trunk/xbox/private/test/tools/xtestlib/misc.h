/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    misc.h

Abstract:

    Header file

Author:

    Sakphong Chanbai (schanbai) 07-Mar-2000

Environment:

    XBox

Revision History:

--*/


#ifndef _MISC_H_
#define _MISC_H_


#ifdef __cplusplus
extern "C" {
#endif


ULONG
NTAPI
xxx_RtlGetLongestNtPathLength(
    VOID
    );


BOOLEAN
NTAPI
xxx_RtlIsTextUnicode(
    IN PVOID Buffer,
    IN ULONG Size,
    IN OUT PULONG Result OPTIONAL
    );


PVOID
AllocateKernelPool(
    SIZE_T Size
    );


VOID
FreeKernelPool(
    PVOID p
    );


#ifdef __cplusplus
}
#endif


#endif // _MISC_H_
