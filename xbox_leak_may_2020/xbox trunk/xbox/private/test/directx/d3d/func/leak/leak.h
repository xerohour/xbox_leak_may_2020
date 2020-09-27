/***************************************************************************
 *
 *  Copyright (C) 2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       leak.h
 *  Content:    leak utility interface
 *  History:
 *   Date       By       Reason
 *   ====       ==       ======
 *   8/28/01    jgould Created to test Xbox leak
 *
 ****************************************************************************/

#pragma once

#include <xtl.h>
#include <xdbg.h>
#include <xtestlib.h>
#include <xlog.h>
#include <macros.h>

/*
The following is ripped from mm.h, which can't be included without including 99% of the other nt headers, which makes it not possible to import xtl.h without a lot of difficulty, etc. You'd think that they could make those headers compatible with eachother, but they didn't. Wow, you actually took the effort to read this comment that is now 350 characters long. I'm impressed.               *** GET TO WORK! ***                                 (sheesh!)

*/

typedef struct _MM_STATISTICS {
    ULONG Length;
    ULONG TotalPhysicalPages;
    ULONG AvailablePages;
    ULONG VirtualMemoryBytesCommitted;
    ULONG VirtualMemoryBytesReserved;
    ULONG CachePagesCommitted;
    ULONG PoolPagesCommitted;
    ULONG StackPagesCommitted;
    ULONG ImagePagesCommitted;
} MM_STATISTICS, *PMM_STATISTICS;

#define NTKERNELAPI DECLSPEC_IMPORT         // wdm
typedef LONG NTSTATUS;

EXTERN_C
NTKERNELAPI
NTSTATUS
MmQueryStatistics(
    IN OUT PMM_STATISTICS MemoryStatistics
    );

extern "C" void DebugPrint (const char *, ...);