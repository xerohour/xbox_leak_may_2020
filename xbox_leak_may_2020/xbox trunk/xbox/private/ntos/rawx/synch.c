/*++

Copyright (c) 2000-2001  Microsoft Corporation

Module Name:

    synch.c

Abstract:

    This module implements routines which provide synchronization to the file
    system.

--*/

#include "rawx.h"

//
// All file system access is guarded by this single global lock.
//
INITIALIZED_CRITICAL_SECTION(RawxGlobalMutex);

VOID
RawxAcquireGlobalMutexExclusive(
    VOID
    )
/*++

Routine Description:

    This routine acquires the file system's global lock.

Arguments:

    None.

Return Value:

    None.

--*/
{
    KeEnterCriticalRegion();
    RtlEnterCriticalSection(&RawxGlobalMutex);
}

VOID
RawxReleaseGlobalMutex(
    VOID
    )
/*++

Routine Description:

    This routine releases the file system's global lock.

Arguments:

    None.

Return Value:

    None.

--*/
{
    RtlLeaveCriticalSection(&RawxGlobalMutex);
    KeLeaveCriticalRegion();
}

VOID
RawxAcquireVolumeMutexExclusive(
    IN PRAW_VOLUME_EXTENSION VolumeExtension
    )
/*++

Routine Description:

    This routine acquires the volume's lock for exclusive access.

Arguments:

    VolumeExtension - Specifies the volume to lock.

Return Value:

    None.

--*/
{
    KeEnterCriticalRegion();
    ExAcquireReadWriteLockExclusive(&VolumeExtension->VolumeMutex);
}

VOID
RawxAcquireVolumeMutexShared(
    IN PRAW_VOLUME_EXTENSION VolumeExtension
    )
/*++

Routine Description:

    This routine acquires the volume's lock for shared access.

Arguments:

    VolumeExtension - Specifies the volume to lock.

Return Value:

    None.

--*/
{
    KeEnterCriticalRegion();
    ExAcquireReadWriteLockShared(&VolumeExtension->VolumeMutex);
}

VOID
RawxReleaseVolumeMutex(
    IN PRAW_VOLUME_EXTENSION VolumeExtension
    )
/*++

Routine Description:

    This routine releases the volume's lock.

Arguments:

    VolumeExtension - Specifies the volume to unlock.

Return Value:

    None.

--*/
{
    ExReleaseReadWriteLock(&VolumeExtension->VolumeMutex);
    KeLeaveCriticalRegion();
}
