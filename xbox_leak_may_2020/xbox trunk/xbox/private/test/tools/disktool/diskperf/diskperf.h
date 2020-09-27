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
#include <ntexapi.h>
#include <ntos.h>
#include <xlog.h>
#include <xtestlib.h>
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

DWORD
WINAPI
SizeCache(
    DWORD Size
    );

VOID
WINAPI
run_var1(
    VOID
    );

VOID
WINAPI
run_var2(
    VOID
    );

VOID
WINAPI
run_var3(
    VOID
    );

VOID
WINAPI
run_var4(
    VOID
    );

VOID
WINAPI
run_var5(
    VOID
    );

VOID
WINAPI
run_var6(
    VOID
    );

VOID
WINAPI
run_var7(
    VOID
    );

VOID
WINAPI
run_var8(
    VOID
    );

VOID
WINAPI
run_var9(
    VOID
    );

VOID
WINAPI
run_var10(
    VOID
    );

VOID
WINAPI
run_var11(
    VOID
    );

VOID
WINAPI
run_var12(
    VOID
    );

VOID
WINAPI
run_var13(
    VOID
    );

VOID
WINAPI
run_var14(
    VOID
    );

VOID
WINAPI
run_var15(
    VOID
    );

VOID
WINAPI
run_var16(
    VOID
    );

VOID
WINAPI
run_var17(
    VOID
    );

VOID
WINAPI
StartTimer(
    VOID
    );

DWORD
WINAPI
EndTimer(
    VOID
    );

VOID
WINAPI
RecursivelyDiskCopy(
    POCHAR source, 
    POCHAR target,
    PULONGLONG pTotalBytesCopied,
    PDWORD pTotalFilesCopied,
    DWORD CreateDeleteFlag
    );

PVOID
WINAPI
CreateTestDataBuffer(
    DWORD
    );

VOID
WINAPI
CleanCaches(
    VOID
    );

#endif // _DISKPERF_
