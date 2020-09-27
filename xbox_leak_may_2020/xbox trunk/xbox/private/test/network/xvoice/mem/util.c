/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) Microsoft Corporation

Module Name:

  util.c

Abstract:

  This module contains the utility functions for xvoice

Author:

  Steven Kehrli (steveke) 2-Jan-2002

Environment:

  XBox

Revision History:

------------------------------------------------------------------------------*/

#include "precomp.h"



VOID
GetStats(
    IN OUT  MM_STATISTICS  *mmStats,
    IN OUT  PS_STATISTICS  *psStats
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Gets the current statistics

Arguments:

  mmStats - Pointer to the memory statistics
  psStats - Pointer to the processor statistics

Return Value:

  None

------------------------------------------------------------------------------*/
{
    // Initialize the mm stats
    ZeroMemory(mmStats, sizeof(*mmStats));
    mmStats->Length = sizeof(*mmStats);

    // Initialize the ps stats
    ZeroMemory(psStats, sizeof(*psStats));
    psStats->Length = sizeof(*psStats);

    // Query the mm stats
    MmQueryStatistics(mmStats);

    // Query the ps stats
    PsQueryStatistics(psStats);
}



VOID
CheckStats(
    IN  HANDLE         hLog,
    IN  MM_STATISTICS  *mmStatsInitial,
    IN  MM_STATISTICS  *mmStatsFinal,
    IN  PS_STATISTICS  *psStatsInitial,
    IN  PS_STATISTICS  *psStatsFinal
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Gets the current statistics

Arguments:

  hLog - Handle to the log file
  mmStatsInitial - Pointer to the memory statistics initial
  mmStatsFinal - Pointer to the memory statistics final
  psStatsInitial - Pointer to the processor statistics initial
  psStatsFinal - Pointer to the processor statistics final

Return Value:

  None

------------------------------------------------------------------------------*/
{
    if (0 != memcmp(mmStatsFinal, mmStatsInitial, sizeof(*mmStatsFinal))) {
        if (mmStatsFinal->TotalPhysicalPages != mmStatsInitial->TotalPhysicalPages) {
            Log_va(hLog, TRUE, "  Total Memory\r\n");
            Log_va(hLog, TRUE, "    Initial: %u\r\n", mmStatsInitial->TotalPhysicalPages);
            Log_va(hLog, TRUE, "    Final:   %u\r\n", mmStatsFinal->TotalPhysicalPages);
            Log_va(hLog, TRUE, "    Diff:    %u\r\n\r\n", abs(mmStatsFinal->TotalPhysicalPages - mmStatsInitial->TotalPhysicalPages));
        }

        if (mmStatsFinal->AvailablePages != mmStatsInitial->AvailablePages) {
            Log_va(hLog, TRUE, "  Available Memory\r\n");
            Log_va(hLog, TRUE, "    Initial: %u\r\n", mmStatsInitial->AvailablePages);
            Log_va(hLog, TRUE, "    Final:   %u\r\n", mmStatsFinal->AvailablePages);
            Log_va(hLog, TRUE, "    Diff:    %u\r\n\r\n", abs(mmStatsFinal->AvailablePages - mmStatsInitial->AvailablePages));
        }

        if (mmStatsFinal->CachePagesCommitted != mmStatsInitial->CachePagesCommitted) {
            Log_va(hLog, TRUE, "  Cache\r\n");
            Log_va(hLog, TRUE, "    Initial: %u\r\n", mmStatsInitial->CachePagesCommitted);
            Log_va(hLog, TRUE, "    Final:   %u\r\n", mmStatsFinal->CachePagesCommitted);
            Log_va(hLog, TRUE, "    Diff:    %u\r\n\r\n", abs(mmStatsFinal->CachePagesCommitted - mmStatsInitial->CachePagesCommitted));
        }

        if (mmStatsFinal->PoolPagesCommitted != mmStatsInitial->PoolPagesCommitted) {
            Log_va(hLog, TRUE, "  Pool\r\n");
            Log_va(hLog, TRUE, "    Initial: %u\r\n", mmStatsInitial->PoolPagesCommitted);
            Log_va(hLog, TRUE, "    Final:   %u\r\n", mmStatsFinal->PoolPagesCommitted);
            Log_va(hLog, TRUE, "    Diff:    %u\r\n\r\n", abs(mmStatsFinal->PoolPagesCommitted - mmStatsInitial->PoolPagesCommitted));
        }

        if (mmStatsFinal->StackPagesCommitted != mmStatsInitial->StackPagesCommitted) {
            Log_va(hLog, TRUE, "  Stack\r\n");
            Log_va(hLog, TRUE, "    Initial: %u\r\n", mmStatsInitial->StackPagesCommitted);
            Log_va(hLog, TRUE, "    Final:   %u\r\n", mmStatsFinal->StackPagesCommitted);
            Log_va(hLog, TRUE, "    Diff:    %u\r\n\r\n", abs(mmStatsFinal->StackPagesCommitted - mmStatsInitial->StackPagesCommitted));
        }

        if (mmStatsFinal->ImagePagesCommitted != mmStatsInitial->ImagePagesCommitted) {
            Log_va(hLog, TRUE, "  Image\r\n");
            Log_va(hLog, TRUE, "    Initial: %u\r\n", mmStatsInitial->ImagePagesCommitted);
            Log_va(hLog, TRUE, "    Final:   %u\r\n", mmStatsFinal->ImagePagesCommitted);
            Log_va(hLog, TRUE, "    Diff:    %u\r\n\r\n", abs(mmStatsFinal->ImagePagesCommitted - mmStatsInitial->ImagePagesCommitted));
        }
    }

    if (0 != memcmp(psStatsFinal, psStatsInitial, sizeof(*psStatsFinal))) {
        if (psStatsFinal->HandleCount != psStatsInitial->HandleCount) {
            Log_va(hLog, TRUE, "  Handle Count\r\n");
            Log_va(hLog, TRUE, "    Initial: %u\r\n", psStatsInitial->HandleCount);
            Log_va(hLog, TRUE, "    Final:   %u\r\n", psStatsFinal->HandleCount);
            Log_va(hLog, TRUE, "    Diff:    %u\r\n\r\n", abs(psStatsFinal->HandleCount - psStatsInitial->HandleCount));
        }
    }

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
