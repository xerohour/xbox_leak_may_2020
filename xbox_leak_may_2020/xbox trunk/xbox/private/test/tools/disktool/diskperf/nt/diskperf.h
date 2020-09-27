/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    diskperf.h

Abstract:

    disk perf for xbox

written by:

    John Daly (johndaly) 13-july-2000

Environment:

    XBox

Revision History:


--*/

#ifndef _DISKPERF_
#define _DISKPERF_

#include <nt.h>
#include <ntrtl.h>
#include <nturtl.h>
#include <windef.h>
#include <winbase.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>

//
// Heap handle from HeapCreate
//

HANDLE HeapHandle;
HANDLE LogHandle;
LARGE_INTEGER Frequency;
LARGE_INTEGER PerformanceCount;

#define ARRAYSIZE(a)        (sizeof(a) / sizeof(a[0]))

#define CREATE_FLAG  1
#define DELETE_FLAG  2

//
// function declarations
//

VOID
run_var1(
    VOID
    );

VOID
run_var2(
    VOID
    );

VOID
run_var3(
    VOID
    );

VOID
run_var4(
    VOID
    );

VOID
run_var5(
    VOID
    );

VOID
run_var6(
    VOID
    );

VOID
run_var7(
    VOID
    );

VOID
run_var8(
    VOID
    );

VOID
run_var9(
    VOID
    );

VOID
run_var10(
    VOID
    );

VOID
run_var11(
    VOID
    );

VOID
run_var12(
    VOID
    );

VOID
StartTimer(
    VOID
    );

DWORD
EndTimer(
    VOID
    );

VOID
RecursivelyDiskCopy(
    PWCHAR source, 
    PWCHAR target,
    PULONGLONG pTotalBytesCopied,
    PDWORD pTotalFilesCopied,
    DWORD CreateDeleteFlag
    );

PVOID
CreateTestDataBuffer(
    DWORD
    );

VOID
CleanCaches(
    VOID
    );

#endif // _DISKPERF_
