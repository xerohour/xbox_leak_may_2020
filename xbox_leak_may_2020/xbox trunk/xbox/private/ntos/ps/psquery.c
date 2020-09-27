/*++

Copyright (c) 1989  Microsoft Corporation

Module Name:

    psquery.c

Abstract:

    This module implements the set and query functions for
    process and thread objects.

--*/

#include "psp.h"

extern OBJECT_HANDLE_TABLE ObpObjectHandleTable;

NTSTATUS
PsQueryStatistics(
    IN OUT PPS_STATISTICS ProcessStatistics
    )
/*++

Routine Description:

    This routine returns various statistics about the state of the process
    subsystem.

Arguments:

    ProcessStatistics - Supplies the buffer to fill with the process manager's
        statistics.

Return Value:

    Status of operation.

--*/
{
    NTSTATUS status;
    PKPROCESS Process;

    if (ProcessStatistics->Length == sizeof(PS_STATISTICS)) {

        Process = PsGetCurrentProcess();

        ProcessStatistics->ThreadCount = Process->StackCount;
        ProcessStatistics->HandleCount = ObpObjectHandleTable.HandleCount;

        status = STATUS_SUCCESS;

    } else {
        status = STATUS_INVALID_PARAMETER;
    }

    return status;
}
