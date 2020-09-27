/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) Microsoft Corporation

Module Name:

  util.c

Abstract:

  This module contains the utility functions for xnet

Author:

  Steven Kehrli (steveke) 17-Nov-2001

Environment:

  XBox

Revision History:

------------------------------------------------------------------------------*/

#include "precomp.h"



VOID
GetCpuTimes(
    ULONG  *KernelTime,
    ULONG  *InterruptTime,
    ULONG  *DpcTime,
    ULONG  *IdleTime
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Gets the kernel and idle time

Arguments:

  KernelTime - Kernel time
  InterruptTime - Interrupt time
  DpcTime - Dpc time
  IdleTime - Idle time

Return Value:

  None

------------------------------------------------------------------------------*/
{
    // pkprcb is a pointer to the processor control block
    PKPRCB  pkprcb = NULL;



    // Update the system times
    __asm {
        pushfd
        cli
    };

    pkprcb = KeGetCurrentPrcb();

    *KernelTime = pkprcb->CurrentThread->KernelTime;
    *InterruptTime = pkprcb->InterruptTime;
    *DpcTime = pkprcb->DpcTime;
    *IdleTime = pkprcb->IdleThread->KernelTime;

    __asm popfd;
}



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
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Calculates the difference in each stat

Arguments:

  KernelTimeInitial - Initial kernel time
  KernelTimeFinal - Final kernel time
  InterruptTimeInitial - Initial interrupt time
  InterruptIdleFinal - Final interrupt time
  DpcTimeInitial - Initial dpc time
  DpcTimeFinal - Final dpc time
  IdleTimeInitial - Initial idle time
  IdleTimeFinal - Final idle time
  KernelTimeDiff - Diff kernel time
  InterruptTimeDiff - Diff interrupt time
  DpcTimeDiff - Diff dpc time
  IdleTimeDiff - Diff idle time

Return Value:

  None

------------------------------------------------------------------------------*/
{
    // Calculate the diff
    *KernelTimeDiff = KernelTimeFinal - KernelTimeInitial;
    *InterruptTimeDiff = InterruptTimeFinal - InterruptTimeInitial;
    *DpcTimeDiff = DpcTimeFinal - DpcTimeInitial;
    *IdleTimeDiff = IdleTimeFinal - IdleTimeInitial;
}



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
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Calculates the difference in each stat and adds it to the difference

Arguments:

  KernelTimeInitial - Initial kernel time
  KernelTimeFinal - Final kernel time
  InterruptTimeInitial - Initial interrupt time
  InterruptTimeFinal - Final interrupt time
  DpcTimeInitial - Initial dpc time
  DpcTimeFinal - Final dpc time
  IdleTimeInitial - Initial idle time
  IdleTimeFinal - Final idle time
  KernelTimeDiff - Diff kernel time
  InterruptTimeDiff - Diff interrupt time
  DpcTimeDiff - Diff dpc time
  IdleTimeDiff - Diff idle time

Return Value:

  None

------------------------------------------------------------------------------*/
{
    // Calculate the diff
    *KernelTimeDiff += (KernelTimeFinal - KernelTimeInitial);
    *InterruptTimeDiff += (InterruptTimeFinal - InterruptTimeInitial);
    *DpcTimeDiff += (DpcTimeFinal - DpcTimeInitial);
    *IdleTimeDiff += (IdleTimeFinal - IdleTimeInitial);
}



VOID
Log_va(
    IN  HANDLE  hLog,
    IN  BOOL    bEcho,
    IN  LPSTR   lpszFormatString,
    IN  ...
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Writes the string to the log file

Arguments:

  hLog - Handle to the log object
  bEcho - Specifies if string should be echoed to the debugger
  lpszFormatString - Pointer to the format string

Return Value:

  None

------------------------------------------------------------------------------*/
{
    // varg_ptr is a pointer to the variable argument list
    va_list  varg_ptr = NULL;
    // szLogString is the string in the line of log output
    CHAR     szLogString[1024 + 1] = {'\0'};
    // dwBytesWritten is the number of bytes written to the file
    DWORD    dwBytesWritten = 0;



    // Get the variable argument list
    va_start(varg_ptr, lpszFormatString);

    // Format the variable argument list
    _vsnprintf(szLogString, 1024, lpszFormatString, varg_ptr);

    // Write the string to the log file
    WriteFile(hLog, szLogString, strlen(szLogString), &dwBytesWritten, NULL);

    if (TRUE == bEcho) {
        // Echo the string to the debugger
        OutputDebugStringA(szLogString);
    }
}



VOID
LogCsvHeader(
    IN  HANDLE  hLog
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Logs the csv column header banner to the log file

Arguments:

  hLog - Handle to the log object

Return Value:

  None

------------------------------------------------------------------------------*/
{
    // Write the banner to the log file
    Log_va(hLog, FALSE, "Socket Type,Payload (Bytes),Total Payload (Bytes),Overhead (Bytes),Total Ethernet (Bytes),Kernel Time (ms),Interrupt Time (ms),DPC Time (ms),Idle Time (ms),Total Time (ms),CPU Usage (%%)\r\n");
}



VOID
LogStats(
    IN  HANDLE  hLog,
    IN  LPSTR   lpszBanner,
    IN  DWORD   dwTestIteration,
    IN  INT     nPayload,
    IN  INT     nPayloadTotal,
    IN  INT     nEthernetOverhead,
    IN  INT     nEthernetTotal,
    IN  ULONG   KernelTimeInitial,
    IN  ULONG   KernelTimeFinal,
    IN  ULONG   InterruptTimeInitial,
    IN  ULONG   InterruptTimeFinal,
    IN  ULONG   DpcTimeInitial,
    IN  ULONG   DpcTimeFinal,
    IN  ULONG   IdleTimeInitial,
    IN  ULONG   IdleTimeFinal
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Logs the stats to the log file

Arguments:

  hLog - Handle to the log object
  lpszBanner - Banner string
  dwTestIteration - Test iteration number
  nPayload - Payload
  nPayloadTotal - Total payload
  nEthernetOverhead - Ethernet overhead
  nEthernetTotal - Total ethernet
  KernelTimeInitial - Initial kernel time
  KernelTimeFinal - Final kernel time
  InterruptTimeInitial - Initial interrupt time
  InterruptTimeFinal - Final interrupt time
  DpcTimeInitial - Initial dpc time
  DpcTimeFinal - Final dpc time
  IdleTimeInitial - Initial idle time
  IdleTimeFinal - Final idle time

Return Value:

  None

------------------------------------------------------------------------------*/
{
    // KernelTimeDiff is the diff in kernel time
    ULONG   KernelTimeDiff;
    // InterruptTimeDiff is the diff in interrupt time
    ULONG   InterruptTimeDiff;
    // DpcTimeDiff is the diff in dpc time
    ULONG   DpcTimeDiff;
    // IdleTimeDiff is the diff in idle time
    ULONG   IdleTimeDiff;
    // TotalTime is the total time
    ULONG   TotalTime;
    // CPUUsage is the CPU Usage (percentage)
    DOUBLE  CPUUsage;



    // Calculate the diff
    CalculateStatsDiff(KernelTimeInitial, KernelTimeFinal, InterruptTimeInitial, InterruptTimeFinal, DpcTimeInitial, DpcTimeFinal, IdleTimeInitial, IdleTimeFinal, &KernelTimeDiff, &InterruptTimeDiff, &DpcTimeDiff, &IdleTimeDiff);

    // Write the banner to the log file
    Log_va(hLog, TRUE, "**********\r\n%s - %d\r\n**********\r\n", lpszBanner, dwTestIteration);

    // Log the stats
    Log_va(hLog, TRUE, "Data (Bytes)\r\n");
    Log_va(hLog, TRUE, "    Payload:           %14u\r\n", nPayload);
    Log_va(hLog, TRUE, "    Total Payload:     %14u\r\n", nPayloadTotal);
    Log_va(hLog, TRUE, "    Ethernet Overhead: %14u\r\n", nEthernetOverhead);
    Log_va(hLog, TRUE, "    Total Ethernet:    %14u\r\n", nEthernetTotal);

    Log_va(hLog, TRUE, "Kernel Time (ms)\r\n");
    Log_va(hLog, TRUE, "    Final:             %14u\r\n", KernelTimeFinal);
    Log_va(hLog, TRUE, "    Initial:           %14u\r\n", KernelTimeInitial);
    Log_va(hLog, TRUE, "    Diff:              %14u\r\n", KernelTimeDiff);

    Log_va(hLog, TRUE, "Interrupt Time (ms)\r\n");
    Log_va(hLog, TRUE, "    Final:             %14u\r\n", InterruptTimeFinal);
    Log_va(hLog, TRUE, "    Initial:           %14u\r\n", InterruptTimeInitial);
    Log_va(hLog, TRUE, "    Diff:              %14u\r\n", InterruptTimeDiff);

    Log_va(hLog, TRUE, "DPC Time (ms)\r\n");
    Log_va(hLog, TRUE, "    Final:             %14u\r\n", DpcTimeFinal);
    Log_va(hLog, TRUE, "    Initial:           %14u\r\n", DpcTimeInitial);
    Log_va(hLog, TRUE, "    Diff:              %14u\r\n", DpcTimeDiff);

    Log_va(hLog, TRUE, "Idle Time (ms)\r\n");
    Log_va(hLog, TRUE, "    Final:             %14u\r\n", IdleTimeFinal);
    Log_va(hLog, TRUE, "    Initial:           %14u\r\n", IdleTimeInitial);
    Log_va(hLog, TRUE, "    Diff:              %14u\r\n", IdleTimeDiff);

    TotalTime = KernelTimeDiff + InterruptTimeDiff + DpcTimeDiff + IdleTimeDiff;

    if (0 == TotalTime) {
        CPUUsage = 0;
    }
    else {
        CPUUsage = 100 - (100 * ((DOUBLE) IdleTimeDiff / (DOUBLE) TotalTime));
    }
    Log_va(hLog, TRUE, "CPU Usage\r\n");
    Log_va(hLog, TRUE, "    Total Time (ms):   %14u\r\n", TotalTime);
    Log_va(hLog, TRUE, "    CPU Usage (%%):     %14.3f\r\n", CPUUsage);

    // End the section
    Log_va(hLog, TRUE, "**********\r\n\r\n");
}



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
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Logs the stats to the log file

Arguments:

  hLog - Handle to the log object
  lpszBanner - Banner string
  nPayload - Payload
  nPayloadTotal - Total payload
  nEthernetOverhead - Ethernet overhead
  nEthernetTotal - Total ethernet
  KernelTime - Kernel time
  InterruptTime - Interrupt time
  DpcTime - Dpc time
  IdleTime - Idle time
  dwAverage - Number to average stats

Return Value:

  None

------------------------------------------------------------------------------*/
{
    // TotalTime is the total time
    ULONG   TotalTime;
    // CPUUsage is the CPU Usage (percentage)
    DOUBLE  CPUUsage;



    if (0 == dwAverage) {
        dwAverage = 1;
    }

    // Write the banner to the log file
    Log_va(hLog, TRUE, "**********\r\n%s\r\n**********\r\n", lpszBanner);

    // Log the stats
    Log_va(hLog, TRUE, "Payload Data (Bytes):           %14u\r\n", nPayload);
    Log_va(hLog, TRUE, "Total Payload Data (Bytes):     %14u\r\n", nPayloadTotal);
    Log_va(hLog, TRUE, "Ethernet Overhead Data (Bytes): %14u\r\n", nEthernetOverhead);
    Log_va(hLog, TRUE, "Total Ethernet Data (Bytes):    %14u\r\n", nEthernetTotal);
    Log_va(hLog, TRUE, "Kernel Time (ms):               %14.3f\r\n", ((DOUBLE) KernelTime / (DOUBLE) dwAverage));
    Log_va(hLog, TRUE, "Interrupt Time (ms):            %14.3f\r\n", ((DOUBLE) InterruptTime / (DOUBLE) dwAverage));
    Log_va(hLog, TRUE, "DPC Time (ms):                  %14.3f\r\n", ((DOUBLE) DpcTime / (DOUBLE) dwAverage));
    Log_va(hLog, TRUE, "Idle Time (ms):                 %14.3f\r\n", ((DOUBLE) IdleTime / (DOUBLE) dwAverage));

    TotalTime = KernelTime + InterruptTime + DpcTime + IdleTime;
    Log_va(hLog, TRUE, "Total Time (ms):                %14.3f\r\n", ((DOUBLE) TotalTime / (DOUBLE) dwAverage));

    if (0 == TotalTime) {
        CPUUsage = 0;
    }
    else {
        CPUUsage = 100 - (100 * ((DOUBLE) IdleTime / (DOUBLE) TotalTime));
    }
    Log_va(hLog, TRUE, "CPU Usage (%%):                  %14.3f\r\n", CPUUsage);

    // End the section
    Log_va(hLog, TRUE, "**********\r\n\r\n");
}



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
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Logs the stats to the csv log file

Arguments:

  hLog - Handle to the log object
  lpszBanner - Banner string
  nPayload - Payload
  nPayloadTotal - Total payload
  nEthernetOverhead - Ethernet overhead
  nEthernetTotal - Total ethernet
  KernelTime - Kernel time
  InterruptTime - Interrupt time
  DpcTime - Dpc time
  IdleTime - Idle time
  dwAverage - Number to average stats

Return Value:

  None

------------------------------------------------------------------------------*/
{
    // TotalTime is the total time
    ULONG   TotalTime;
    // CPUUsage is the CPU Usage (percentage)
    DOUBLE  CPUUsage;



    if (0 == dwAverage) {
        dwAverage = 1;
    }

    // Write the stats to the csv log file
    Log_va(hLog, FALSE, "%s,", lpszBanner);
    Log_va(hLog, FALSE, "%u,", nPayload);
    Log_va(hLog, FALSE, "%u,", nPayloadTotal);
    Log_va(hLog, FALSE, "%u,", nEthernetOverhead);
    Log_va(hLog, FALSE, "%u,", nEthernetTotal);
    Log_va(hLog, FALSE, "%.3f,", ((DOUBLE) KernelTime / (DOUBLE) dwAverage));
    Log_va(hLog, FALSE, "%.3f,", ((DOUBLE) InterruptTime / (DOUBLE) dwAverage));
    Log_va(hLog, FALSE, "%.3f,", ((DOUBLE) DpcTime / (DOUBLE) dwAverage));
    Log_va(hLog, FALSE, "%.3f,", ((DOUBLE) IdleTime / (DOUBLE) dwAverage));

    TotalTime = KernelTime + InterruptTime + DpcTime + IdleTime;
    Log_va(hLog, FALSE, "%.3f,", ((DOUBLE) TotalTime / (DOUBLE) dwAverage));

    if (0 == TotalTime) {
        CPUUsage = 0;
    }
    else {
        CPUUsage = 100 - (100 * ((DOUBLE) IdleTime / (DOUBLE) TotalTime));
    }
    Log_va(hLog, FALSE, "%.3f\r\n", CPUUsage);
}
