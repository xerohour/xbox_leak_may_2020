/*++

Copyright (c) 2000-2001  Microsoft Corporation

Module Name:

    synch.c

Abstract:

    This module implements routines which provide synchronization to the file
    system.

--*/

#include "fatx.h"

//
// Non volume specific file system access is guarded by this global lock.
//
INITIALIZED_CRITICAL_SECTION(FatxGlobalMutex);

VOID
FatxAcquireGlobalMutexExclusive(
    VOID
    )
/*++

Routine Description:

    This routine acquires the file system's global lock for exclusive access.

Arguments:

    None.

Return Value:

    None.

--*/
{
    KeEnterCriticalRegion();
    RtlEnterCriticalSection(&FatxGlobalMutex);
}

VOID
FatxReleaseGlobalMutex(
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
    RtlLeaveCriticalSection(&FatxGlobalMutex);
    KeLeaveCriticalRegion();
}

VOID
FatxAcquireVolumeMutexExclusive(
    IN PFAT_VOLUME_EXTENSION VolumeExtension
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
    ASSERT(KeGetCurrentIrql() < DISPATCH_LEVEL);

#if DBG
    //
    // Assert that we're not recursively entering a write lock.
    //

    if (ExDbgIsReadWriteLockExclusive(&VolumeExtension->VolumeMutex)) {
        ASSERT(VolumeExtension->VolumeMutexExclusiveOwner != KeGetCurrentThread());
    }
#endif

    KeEnterCriticalRegion();
    ExAcquireReadWriteLockExclusive(&VolumeExtension->VolumeMutex);

#if DBG
    VolumeExtension->VolumeMutexExclusiveOwner = KeGetCurrentThread();
#endif
}

VOID
FatxAcquireVolumeMutexShared(
    IN PFAT_VOLUME_EXTENSION VolumeExtension
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
    ASSERT(KeGetCurrentIrql() < DISPATCH_LEVEL);

#if DBG
    //
    // Assert that we're not entering a read lock while owning the write lock.
    //

    if (ExDbgIsReadWriteLockExclusive(&VolumeExtension->VolumeMutex)) {
        ASSERT(VolumeExtension->VolumeMutexExclusiveOwner != KeGetCurrentThread());
    }
#endif

    KeEnterCriticalRegion();
    ExAcquireReadWriteLockShared(&VolumeExtension->VolumeMutex);
}

VOID
FatxReleaseVolumeMutex(
    IN PFAT_VOLUME_EXTENSION VolumeExtension
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
#if DBG
    //
    // Clear out the exclusive owner of the lock.
    //

    if (ExDbgIsReadWriteLockExclusive(&VolumeExtension->VolumeMutex)) {
        VolumeExtension->VolumeMutexExclusiveOwner = NULL;
    }
#endif

    ExReleaseReadWriteLock(&VolumeExtension->VolumeMutex);
    KeLeaveCriticalRegion();
}

#if DBG

VOID
FatxDpcReleaseVolumeMutex(
    IN PFAT_VOLUME_EXTENSION VolumeExtension
    )
/*++

Routine Description:

    This routine releases the volume's lock from an I/O completion routine.

Arguments:

    VolumeExtension - Specifies the volume to unlock.

Return Value:

    None.

--*/
{
#if DBG
    //
    // Clear out the exclusive owner of the lock.
    //

    if (ExDbgIsReadWriteLockExclusive(&VolumeExtension->VolumeMutex)) {
        VolumeExtension->VolumeMutexExclusiveOwner = NULL;
    }
#endif

    ExReleaseReadWriteLock(&VolumeExtension->VolumeMutex);
}

#endif

VOID
FatxAcquireFileMutexExclusive(
    IN PFAT_FCB FileFcb
    )
/*++

Routine Description:

    This routine acquires the file's lock for exclusive access.

Arguments:

    FileFcb - Specifies the file control block to unlock.

Return Value:

    None.

--*/
{
    ASSERT(KeGetCurrentIrql() < DISPATCH_LEVEL);
    ASSERT(FatxIsFlagClear(FileFcb->Flags, FAT_FCB_VOLUME | FAT_FCB_DIRECTORY));

    KeEnterCriticalRegion();
    ExAcquireReadWriteLockExclusive(&FileFcb->File.FileMutex);

#if DBG
    FileFcb->File.FileMutexExclusiveOwner = KeGetCurrentThread();
#endif
}

VOID
FatxAcquireFileMutexShared(
    IN PFAT_FCB FileFcb
    )
/*++

Routine Description:

    This routine acquires the file's lock for shared access.

Arguments:

    FileFcb - Specifies the file control block to unlock.

Return Value:

    None.

--*/
{
    ASSERT(KeGetCurrentIrql() < DISPATCH_LEVEL);
    ASSERT(FatxIsFlagClear(FileFcb->Flags, FAT_FCB_VOLUME | FAT_FCB_DIRECTORY));

    KeEnterCriticalRegion();
    ExAcquireReadWriteLockShared(&FileFcb->File.FileMutex);
}

VOID
FatxReleaseFileMutex(
    IN PFAT_FCB FileFcb
    )
/*++

Routine Description:

    This routine releases the file's lock.

Arguments:

    FileFcb - Specifies the file control block to unlock.

Return Value:

    None.

--*/
{
    ASSERT(FatxIsFlagClear(FileFcb->Flags, FAT_FCB_VOLUME | FAT_FCB_DIRECTORY));

#if DBG
    //
    // Clear out the exclusive owner of the lock.
    //

    if (ExDbgIsReadWriteLockExclusive(&FileFcb->File.FileMutex)) {
        FileFcb->File.FileMutexExclusiveOwner = NULL;
    }
#endif

    ExReleaseReadWriteLock(&FileFcb->File.FileMutex);
    KeLeaveCriticalRegion();
}

#if DBG

VOID
FatxDpcReleaseFileMutex(
    IN PFAT_FCB FileFcb
    )
/*++

Routine Description:

    This routine releases the file's lock from an I/O completion routine.

Arguments:

    FileFcb - Specifies the file control block to unlock.

Return Value:

    None.

--*/
{
    ASSERT(FatxIsFlagClear(FileFcb->Flags, FAT_FCB_VOLUME | FAT_FCB_DIRECTORY));

#if DBG
    //
    // Clear out the exclusive owner of the lock.
    //

    if (ExDbgIsReadWriteLockExclusive(&FileFcb->File.FileMutex)) {
        FileFcb->File.FileMutexExclusiveOwner = NULL;
    }
#endif

    ExReleaseReadWriteLock(&FileFcb->File.FileMutex);
}

#endif
