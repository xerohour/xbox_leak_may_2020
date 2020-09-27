/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) Microsoft Corporation

Module Name:

  util.h

Abstract:

  This module contains the definitions for util.c

Author:

  Steven Kehrli (steveke) 23-Oct-2001

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
LogCodecCreateBanner(
    IN  HANDLE  hLog
);

VOID
LogDeviceCreateBanner(
    IN  HANDLE  hLog
);

VOID
LogProcessBanner(
    IN  HANDLE  hLog
);

VOID
LogStats(
    IN  HANDLE  hLog,
    IN  LPSTR   lpszBanner,
    IN  DWORD   dwTestIteration,
    IN  ULONG   KernelTimeInitial,
    IN  ULONG   KernelTimeFinal,
    IN  ULONG   InterruptTimeInitial,
    IN  ULONG   InterruptTimeFinal,
    IN  ULONG   DpcTimeInitial,
    IN  ULONG   DpcTimeFinal,
    IN  ULONG   IdleTimeInitial,
    IN  ULONG   IdleTimeFinal,
    IN  DWORD   *dwNumMicrophoneFailures,
    IN  DWORD   *dwNumHeadphoneFailures,
    IN  DWORD   *dwNumEncoderFailures,
    IN  DWORD   *dwNumDecoderFailures,
    IN  DWORD   *dwNumQueueFailures
);

VOID
LogStatsSummary(
    IN  HANDLE  hLog,
    IN  LPSTR   lpszBanner,
    IN  ULONG   KernelTime,
    IN  ULONG   InterruptTime,
    IN  ULONG   DpcTime,
    IN  ULONG   IdleTime,
    IN  DWORD   *dwNumMicrophoneFailures,
    IN  DWORD   *dwNumHeadphoneFailures,
    IN  DWORD   *dwNumEncoderFailures,
    IN  DWORD   *dwNumDecoderFailures,
    IN  DWORD   *dwNumQueueFailures,
    IN  DWORD   dwAverage
);

VOID
LogStatsCsv(
    IN  HANDLE  hLog,
    IN  LPSTR   lpszBanner,
    IN  ULONG   KernelTime,
    IN  ULONG   InterruptTime,
    IN  ULONG   DpcTime,
    IN  ULONG   IdleTime,
    IN  DWORD   *dwNumMicrophoneFailures,
    IN  DWORD   *dwNumHeadphoneFailures,
    IN  DWORD   *dwNumEncoderFailures,
    IN  DWORD   *dwNumDecoderFailures,
    IN  DWORD   *dwNumQueueFailures,
    IN  DWORD   dwAverage
);



VOID
LogStatsFailures(
    IN  HANDLE  hLog,
    IN  DWORD   *dwNumMicrophoneFailures,
    IN  DWORD   *dwNumHeadphoneFailures,
    IN  DWORD   *dwNumEncoderFailures,
    IN  DWORD   *dwNumDecoderFailures,
    IN  DWORD   *dwNumQueueFailures,
    IN  DWORD   dwAverage
);

#define LogStatsBanner(hLog) Log_va(hLog, FALSE, "Kernel Time (ms), Interrupt Time (ms), DPC Time (ms), Idle Time (ms), Total Time (ms), CPU Usage, Microphone Total Failures, Headphone Total Failures, Encoder Total Failures, Decoder Total Failures, Queue Total Failures, Microphone Average Failures, Headphone Average Failures, Encoder Average Failures, Decoder Average Failures, Queue Average Failures\r\n")

#endif
