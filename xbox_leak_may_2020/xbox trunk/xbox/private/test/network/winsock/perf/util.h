/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) Microsoft Corporation

Module Name:

  util.h

Abstract:

  This module contains the definitions for util.c

Author:

  Steven Kehrli (steveke) 17-Nov-2001

Environment:

  XBox

Revision History:

------------------------------------------------------------------------------*/

#ifndef _UTIL_H
#define _UTIL_H

#include "precomp.h"



// Function prototypes

// Stats

VOID
GetCpuTimes(
    ULONG  *KernelTime,
    ULONG  *InterruptTime,
    ULONG  *DpcTime,
    ULONG  *IdleTime
);

VOID
CalculateStatsDiff(
    IN     ULONG  KernelTimeInitial,
    IN     ULONG  KernelTimeFinal,
    IN     ULONG  InterruptTimeInitial,
    IN     ULONG  InterruptTimeFinal,
    IN     ULONG  DpcTimeInitial,
    IN     ULONG  DpcTimeFinal,
    IN     ULONG  IdleTimeInitial,
    IN     ULONG  IdleTimeFinal,
    IN OUT ULONG  *KernelTimeDiff,
    IN OUT ULONG  *InterruptTimeDiff,
    IN OUT ULONG  *DpcTimeDiff,
    IN OUT ULONG  *IdleTimeDiff
);

VOID
CalculateAndAddStatsDiff(
    IN     ULONG  KernelTimeInitial,
    IN     ULONG  KernelTimeFinal,
    IN     ULONG  InterruptTimeInitial,
    IN     ULONG  InterruptTimeFinal,
    IN     ULONG  DpcTimeInitial,
    IN     ULONG  DpcTimeFinal,
    IN     ULONG  IdleTimeInitial,
    IN     ULONG  IdleTimeFinal,
    IN OUT ULONG  *KernelTimeDiff,
    IN OUT ULONG  *InterruptTimeDiff,
    IN OUT ULONG  *DpcTimeDiff,
    IN OUT ULONG  *IdleTimeDiff
);



// Logging

VOID
Log_va(
    IN  HANDLE  hLog,
    IN  BOOL    bEcho,
    IN  LPSTR   lpszFormatString,
    IN  ...
);

VOID
LogCsvHeader(
    IN  HANDLE  hLog
);

VOID
LogStats(
    IN  HANDLE  hLog,
    IN  LPSTR   lpszBanner,
    IN  DWORD   dwTestIteration,
    IN  INT     nPayload,
    IN  INT     nPayloadTotal,
    IN  INT     nOverhead,
    IN  INT     nEthernetTotal,
    IN  ULONG   KernelTimeInitial,
    IN  ULONG   KernelTimeFinal,
    IN  ULONG   InterruptTimeInitial,
    IN  ULONG   InterruptTimeFinal,
    IN  ULONG   DpcTimeInitial,
    IN  ULONG   DpcTimeFinal,
    IN  ULONG   IdleTimeInitial,
    IN  ULONG   IdleTimeFinal
);

VOID
LogStatsSummary(
    IN  HANDLE  hLog,
    IN  LPSTR   lpszBanner,
    IN  INT     nPayload,
    IN  INT     nPayloadTotal,
    IN  INT     nEthernetOverhead,
    IN  INT     nEthernetTotal,
    IN  ULONG   KernelTime,
    IN  ULONG   InterruptTime,
    IN  ULONG   DpcTime,
    IN  ULONG   IdleTime,
    IN  DWORD   dwAverage
);

VOID
LogStatsCsv(
    IN  HANDLE  hLog,
    IN  LPSTR   lpszBanner,
    IN  INT     nPayload,
    IN  INT     nPayloadTotal,
    IN  INT     nEthernetOverhead,
    IN  INT     nEthernetTotal,
    IN  ULONG   KernelTime,
    IN  ULONG   InterruptTime,
    IN  ULONG   DpcTime,
    IN  ULONG   IdleTime,
    IN  DWORD   dwAverage
);

#endif
