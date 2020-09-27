/*++

Copyright (c) 2001  Microsoft Corporation

Module Name:

    rwlock.c

Abstract:

    This module implements routines to support executive reader/writer locks.

--*/

#include "exp.h"

VOID
ExInitializeReadWriteLock(
    IN PERWLOCK ReadWriteLock
    )
/*++

Routine Description:

    This routine is called by the I/O manager to handle IRP_MJ_CLOSE requests.

Arguments:

    DeviceObject - Specifies the device object that the I/O request is for.

    Irp - Specifies the packet that describes the I/O request.

Return Value:

    Status of operation.

--*/
{
    ReadWriteLock->LockCount = -1;
    ReadWriteLock->WritersWaitingCount = 0;
    ReadWriteLock->ReadersWaitingCount = 0;
    ReadWriteLock->ReadersEntryCount = 0;

    KeInitializeEvent(&ReadWriteLock->WriterEvent, SynchronizationEvent, FALSE);
    KeInitializeSemaphore(&ReadWriteLock->ReaderSemaphore, 0, MAXLONG);
}
