/*++

Copyright (c) 2000-2001  Microsoft Corporation

Module Name:

    allocsup.c

Abstract:

    This module implements routines which provide support for managing volume
    cluster allocations.

--*/

#include "fatx.h"

NTSTATUS
FatxInitializeDirectoryCluster(
    IN PFAT_VOLUME_EXTENSION VolumeExtension,
    IN PIRP Irp,
    IN ULONG ClusterNumber
    )
/*++

Routine Description:

    This routine initializes the contents of the supplied cluster for use as a
    directory entry cluster.

Arguments:

    VolumeExtension - Specifies the extension that the I/O request is for.

    Irp - Specifies the packet that describes the I/O request.

    ClusterNumber - Specifies the cluster number to initialize.

Return Value:

    Status of operation.

--*/
{
    ULONGLONG PhysicalByteOffset;

    ASSERT(FatxIsValidCluster(VolumeExtension, ClusterNumber));
    ASSERT(VolumeExtension->VolumeMutexExclusiveOwner == KeGetCurrentThread());

    //
    // Compute the physical byte offset of the starting cluster.  Clusters
    // always start on a page boundary.
    //

    PhysicalByteOffset = FatxClusterToPhysicalByteOffset(VolumeExtension,
        ClusterNumber);

    ASSERT(BYTE_OFFSET(PhysicalByteOffset) == 0);

    //
    // Use the file system cache to quickly fill the cluster with 0xFFs
    // (FAT_DIRENT_NEVER_USED2) using a single cache buffer.  This pattern is
    // used in order to minimize signal noise when writing to a memory unit (MU)
    // device.
    //

    return FscWriteFFs(&VolumeExtension->CacheExtension, Irp, PhysicalByteOffset,
        VolumeExtension->BytesPerCluster);
}

NTSTATUS
FatxLinkClusterChains(
    IN PFAT_VOLUME_EXTENSION VolumeExtension,
    IN PIRP Irp,
    IN ULONG LinkClusterNumber,
    IN ULONG StartingClusterNumber
    )
/*++

Routine Description:

    This routine links the starting cluster number to the link cluster number.
    The link cluster number should currently have the value FAT_CLUSTER_LAST.

Arguments:

    VolumeExtension - Specifies the extension that the I/O request is for.

    Irp - Specifies the packet that describes the I/O request.

    LinkClusterNumber - Specifies the cluster number to link in the starting
        cluster number to.  This cluster should be the end of an existing file
        allocation chain.

    StartingClusterNumber - Specifies the starting cluster number of an
        allocation chain which is added to the above cluster chain.

Return Value:

    Status of operation.

--*/
{
    NTSTATUS status;
    ULONGLONG PhysicalFatByteOffset;
    PVOID CacheBuffer;
    PVOID FatEntry;

    ASSERT(FatxIsValidCluster(VolumeExtension, LinkClusterNumber));
    ASSERT(FatxIsValidCluster(VolumeExtension, StartingClusterNumber));
    ASSERT(VolumeExtension->VolumeMutexExclusiveOwner == KeGetCurrentThread());

    //
    // Map in the cache buffer that contains the desired file allocation table
    // entry.
    //

    PhysicalFatByteOffset = VolumeExtension->FatByteOffset +
        FatxClusterToFatByteOffset(VolumeExtension, LinkClusterNumber);

    status = FscMapBuffer(&VolumeExtension->CacheExtension, Irp,
        PhysicalFatByteOffset, TRUE, &CacheBuffer);

    if (NT_SUCCESS(status)) {

        //
        // Change the file allocation table entry from FAT_CLUSTER_LAST to the
        // starting cluster number of the allocation extension.
        //

        FatEntry = CacheBuffer;
        ASSERT(FatxReadFatEntry(VolumeExtension, FatEntry) == FAT_CLUSTER_LAST);
        FatxWriteFatEntry(VolumeExtension, FatEntry, StartingClusterNumber);

        //
        // Write the change back out the disk and unmap the cache buffer.
        //

        status = FscWriteBuffer(&VolumeExtension->CacheExtension,
            Irp, PhysicalFatByteOffset, 1 << VolumeExtension->FatEntryShift,
            CacheBuffer);
    }

    //
    // If we failed to complete the above operation, then delete the allocation
    // extension on behalf of the caller.
    //

    if (!NT_SUCCESS(status)) {
        FatxFreeClusters(VolumeExtension, Irp, StartingClusterNumber, FALSE);
    }

    return status;
}

NTSTATUS
FatxAllocateClusters(
    IN PFAT_VOLUME_EXTENSION VolumeExtension,
    IN PIRP Irp,
    IN ULONG EndingCluster,
    IN ULONG ClustersNeeded,
    OUT FAT_CLUSTER_RUN ClusterRuns[FAT_MAXIMUM_CLUSTER_RUNS],
    OUT PULONG NumberOfClusterRuns,
    OUT PULONG ReturnedEndingCluster
    )
/*++

Routine Description:

    This routine allocates a chain of clusters.  The cluster chain is optionally
    linked to the supplied ending cluster.

Arguments:

    VolumeExtension - Specifies the extension that the I/O request is for.

    Irp - Specifies the packet that describes the I/O request.

    EndingCluster - Specifies the last cluster of an existing file allocation
        chain.  The new file allocation chain is linked to this cluster.

    ClustersNeeded - Specifies the number of clusters to allocate.

    ClusterRuns - Specifies an array of that will receive the starting physical
        cluster and run length for the first FAT_MAXIMUM_CLUSTER_RUNS cluster
        fragments.

    NumberOfClusterRuns - Specifies the buffer to receive the number of cluster
        runs that were actually filled in.

    ReturnedEndingCluster - Specifies the buffer to receive the last cluster of
        the allocation chain.

Return Value:

    Status of operation.

--*/
{
    NTSTATUS status;
    ULONG SectorSize;
    ULONG SectorMask;
    PFAT_CLUSTER_RUN CurrentClusterRun;
    PFAT_CLUSTER_RUN EndingClusterRun;
    ULONG EndingClusterFatByteOffset;
    ULONG ClusterNumber;
    BOOLEAN UsedFirstAvailableClusterHint;
    ULONG InitialCluster;
    BOOLEAN SkipLinkClusterChains;
    ULONG ClusterRunInitialCluster;
    ULONG ClusterRunLength;
    PVOID LastFatEntry;
    ULONG LastSectorLastAllocatedCluster;
    ULONG FirstAllocatedCluster;
    ULONG LastAllocatedCluster;
    BOOLEAN CacheBufferDirty;
    PVOID CacheBuffer;
    ULONG CacheBufferFatByteOffset;
    ULONG CacheBufferClustersAllocated;
    ULONG FatByteOffset;
    ULONG SectorFatByteOffset;
    PVOID FatEntry;
    ULONG FatEntryClusterNumber;

    ASSERT(VolumeExtension->VolumeMutexExclusiveOwner == KeGetCurrentThread());

    SectorSize = VolumeExtension->SectorSize;
    SectorMask = SectorSize - 1;

    //
    // The below code assumes that relative byte offsets into the file
    // allocation table have the same page alignment as their corresponding
    // physical byte offsets.
    //

    ASSERT(BYTE_OFFSET(VolumeExtension->FatByteOffset) == 0);

    //
    // Check if the volume has enough clusters available to satisfy the request.
    //

    if (ClustersNeeded > VolumeExtension->NumberOfClustersAvailable) {
        return STATUS_DISK_FULL;
    }

    //
    // Keep track of the current and maximum cluster run pointers.
    //

#if DBG
    RtlFillMemory(ClusterRuns, sizeof(FAT_CLUSTER_RUN) * FAT_MAXIMUM_CLUSTER_RUNS, 0xcc);
#endif

    CurrentClusterRun = &ClusterRuns[0];
    EndingClusterRun = &ClusterRuns[FAT_MAXIMUM_CLUSTER_RUNS];

    //
    // Determine from what cluster to start scanning for free clusters.
    //

    if (EndingCluster != FAT_CLUSTER_NULL) {

        EndingClusterFatByteOffset = FatxClusterToFatByteOffset(VolumeExtension,
            EndingCluster);
        ClusterNumber = EndingCluster + 1;

        if (ClusterNumber == VolumeExtension->NumberOfClusters +
            FAT_RESERVED_FAT_ENTRIES) {
            ClusterNumber = VolumeExtension->FirstAvailableClusterHint;
        }

    } else {
        EndingClusterFatByteOffset = MAXULONG;
        ClusterNumber = VolumeExtension->FirstAvailableClusterHint;
    }

    //
    // Walk the file allocation table allocating the cluster chain.
    //

    UsedFirstAvailableClusterHint =
        (BOOLEAN)(ClusterNumber == VolumeExtension->FirstAvailableClusterHint);
    InitialCluster = FAT_CLUSTER_NULL;
    SkipLinkClusterChains = FALSE;
    ClusterRunInitialCluster = FAT_CLUSTER_NULL;
    ClusterRunLength = 0;
    LastFatEntry = NULL;
    LastSectorLastAllocatedCluster = EndingCluster;
    FirstAllocatedCluster = FAT_CLUSTER_NULL;
    LastAllocatedCluster = FAT_CLUSTER_NULL;
    CacheBufferDirty = FALSE;
    CacheBuffer = NULL;
    CacheBufferFatByteOffset = 0;
    CacheBufferClustersAllocated = 0;

    for (;;) {

        ASSERT(FatxIsValidCluster(VolumeExtension, ClusterNumber));

        //
        // Compute the relative offset to the file allocation table entry for
        // the next cluster to check and its sector aligned byte offset.
        //

        FatByteOffset = FatxClusterToFatByteOffset(VolumeExtension, ClusterNumber);
        SectorFatByteOffset = FatByteOffset & ~SectorMask;

        //
        // If we haven't mapped in a cache buffer yet or if we're switching
        // sectors, then we need to commit and switch cache buffers.
        //

        if ((CacheBuffer == NULL) ||
            (CacheBufferFatByteOffset != SectorFatByteOffset)) {

            //
            // If another cache buffer has already been mapped, then commit the
            // changes on this buffer if necessary.
            //

            if (CacheBuffer != NULL) {

                if (CacheBufferDirty) {

                    status = FscWriteBuffer(&VolumeExtension->CacheExtension,
                        Irp, VolumeExtension->FatByteOffset +
                        CacheBufferFatByteOffset, SectorSize, CacheBuffer);

                    if (!NT_SUCCESS(status)) {
                        break;
                    }

                    //
                    // Decrease the number of clusters available on the volume
                    // now that we've commited the file allocation table sector.
                    //

                    VolumeExtension->NumberOfClustersAvailable -=
                        CacheBufferClustersAllocated;

                    //
                    // If we have an existing allocation from a previous sector,
                    // then link up the first cluster from this sector to the
                    // last cluster from the previous sector.
                    //
                    // Note that FatxLinkClusterChains calls FatxFreeClusters
                    // for the starting cluster number when the operation fails.
                    //

                    if ((LastSectorLastAllocatedCluster != FAT_CLUSTER_NULL) &&
                        !SkipLinkClusterChains) {

                        status = FatxLinkClusterChains(VolumeExtension, Irp,
                            LastSectorLastAllocatedCluster,
                            FirstAllocatedCluster);

                        if (!NT_SUCCESS(status)) {
                            break;
                        }
                    }

                    LastSectorLastAllocatedCluster = LastAllocatedCluster;

                } else {
                    FscUnmapBuffer(CacheBuffer);
                }

                CacheBufferDirty = FALSE;
                CacheBufferClustersAllocated = 0;
            }

            //
            // Map the next file allocation table page into memory.
            //

            status = FscMapBuffer(&VolumeExtension->CacheExtension, Irp,
                VolumeExtension->FatByteOffset + SectorFatByteOffset, TRUE,
                &CacheBuffer);

            if (!NT_SUCCESS(status)) {
                break;
            }

            CacheBufferFatByteOffset = SectorFatByteOffset;

            //
            // If we're searching for clusters on the same sector as the initial
            // ending cluster number, then set LastFatEntry to point at that
            // cluster's file allocation table entry.  That causes the below
            // code to link the initial ending cluster to any clusters we find
            // on the same sector with a single write operation.
            //

            if ((LastSectorLastAllocatedCluster != FAT_CLUSTER_NULL) &&
                (LastSectorLastAllocatedCluster == EndingCluster) &&
                ((EndingClusterFatByteOffset & ~SectorMask) == SectorFatByteOffset)) {

                LastFatEntry = (PUCHAR)CacheBuffer +
                    (EndingClusterFatByteOffset & SectorMask);
                SkipLinkClusterChains = TRUE;

                ASSERT(FatxReadFatEntry(VolumeExtension, LastFatEntry) ==
                    FAT_CLUSTER_LAST);

            } else {
                LastFatEntry = NULL;
                SkipLinkClusterChains = FALSE;
            }
        }

        //
        // Compute the pointer to the desired file allocation table entry and
        // grab its allocation status.
        //

        FatEntry = (PUCHAR)CacheBuffer + (FatByteOffset & SectorMask);
        FatEntryClusterNumber = FatxReadFatEntry(VolumeExtension, FatEntry);

        //
        // Check if the cluster is unallocated.
        //

        if (FatEntryClusterNumber == FAT_CLUSTER_AVAILABLE) {

            //
            // Convert the unallocated cluster to an allocated cluster.
            //

            if (FatxIsFlagClear(VolumeExtension->Flags, FAT_VOLUME_FAT16X)) {
                *(PULONG)FatEntry = FAT_CLUSTER_LAST;
            } else {
                *(PUSHORT)FatEntry = FAT_CLUSTER16_LAST;
            }

            CacheBufferDirty = TRUE;
            CacheBufferClustersAllocated++;

            //
            // Check how to link this newly allocated cluster to the file's
            // cluster chain.
            //

            if (LastFatEntry != NULL) {

                //
                // The previously allocated cluster resides in the same sector
                // as this newly allocated cluster, so link the clusters up now.
                //

                FatxWriteFatEntry(VolumeExtension, LastFatEntry, ClusterNumber);

            } else {

                //
                // This is the first cluster allocation for this sector.  After
                // we write out this sector, we'll go back and fix up the
                // cluster chain to point at this cluster number.
                //

                FirstAllocatedCluster = ClusterNumber;
            }

            //
            // If this is the first cluster that we're allocating, remember its
            // number.  This is also the first cluster for the first cluster
            // run.
            //
            // Otherwise, if this newly allocated cluster isn't physically
            // contiguous with the current cluster run, then store the current
            // cluster run into the caller's buffer if space allows and then
            // start a new cluster run.
            //

            if (InitialCluster == FAT_CLUSTER_NULL) {

                InitialCluster = ClusterNumber;

                ClusterRunInitialCluster = ClusterNumber;
                ASSERT(ClusterRunLength == 0);

            } else if ((ClusterRunInitialCluster + ClusterRunLength) !=
                ClusterNumber) {

                if (CurrentClusterRun < EndingClusterRun) {
                    CurrentClusterRun->PhysicalClusterNumber = ClusterRunInitialCluster;
                    CurrentClusterRun->ClusterRunLength = ClusterRunLength;
                    CurrentClusterRun++;
                }

                ClusterRunInitialCluster = ClusterNumber;
                ClusterRunLength = 0;
            }

            ClusterRunLength++;

            //
            // If we continue to allocate clusters in this sector, then we can
            // use the current file allocation table entry pointer to link up
            // those clusters.
            //

            LastFatEntry = FatEntry;
            LastAllocatedCluster = ClusterNumber;

            //
            // Decrement the number of clusters needed.
            //

            if (--ClustersNeeded == 0) {

                //
                // We've allocated all of the clusters we need.  Commit the
                // changes on this buffer.
                //

                status = FscWriteBuffer(&VolumeExtension->CacheExtension,
                    Irp, VolumeExtension->FatByteOffset +
                    CacheBufferFatByteOffset, SectorSize, CacheBuffer);

                if (!NT_SUCCESS(status)) {
                    break;
                }

                //
                // If we starting scanning for free clusters from the first
                // available cluster hint, then we need to update that hint to
                // point past the last cluster that we allocated.
                //

                if (UsedFirstAvailableClusterHint) {
                    VolumeExtension->FirstAvailableClusterHint =
                        LastAllocatedCluster + 1;
                }

                //
                // Decrease the number of clusters available on the volume now
                // that we've commited the file allocation table sector.
                //

                VolumeExtension->NumberOfClustersAvailable -=
                    CacheBufferClustersAllocated;

                //
                // If we have an existing allocation from a previous sector,
                // then link up the first cluster from this sector to the last
                // cluster from the previous sector.
                //
                // Note that FatxLinkClusterChains calls FatxFreeClusters for
                // the starting cluster number when the operation fails.
                //

                if ((LastSectorLastAllocatedCluster != FAT_CLUSTER_NULL) &&
                    !SkipLinkClusterChains) {

                    status = FatxLinkClusterChains(VolumeExtension, Irp,
                        LastSectorLastAllocatedCluster,
                        FirstAllocatedCluster);

                    if (!NT_SUCCESS(status)) {
                        break;
                    }
                }

                //
                // Store this last cluster run if space allows.
                //

                if (CurrentClusterRun < EndingClusterRun) {
                    CurrentClusterRun->PhysicalClusterNumber = ClusterRunInitialCluster;
                    CurrentClusterRun->ClusterRunLength = ClusterRunLength;
                    CurrentClusterRun++;
                }

                //
                // Return the number of cluster runs that were actually used.
                //

                *NumberOfClusterRuns = CurrentClusterRun - ClusterRuns;

                //
                // Return the last cluster that we allocated to the client.  If
                // we overflowed the cluster runs array, then this is the only
                // easy way to get the value.
                //

                *ReturnedEndingCluster = LastAllocatedCluster;

                return STATUS_SUCCESS;
            }
        }

        //
        // Advance to the next cluster.  If we reach the end of the disk, jump
        // back to the first available cluster hint.  If we've already used the
        // first available cluster hint, then either the in memory state is
        // corrupt or the on disk structures are corrupt, so bail out.
        //

        ClusterNumber++;

        if (ClusterNumber == VolumeExtension->NumberOfClusters +
            FAT_RESERVED_FAT_ENTRIES) {

            if (UsedFirstAvailableClusterHint) {
                FatxDbgPrint(("FATX: FAT table is corrupt.\n"));
                FscDiscardBuffer(CacheBuffer);
                status = STATUS_DISK_CORRUPT_ERROR;
                break;
            }

            ClusterNumber = VolumeExtension->FirstAvailableClusterHint;
            UsedFirstAvailableClusterHint = TRUE;
        }
    }

    //
    // We failed for some reason.  Attempt to free any clusters that we've
    // already allocated.  Nothing has been written to the disk yet if
    // LastSectorLastAllocatedCluster is still initialized to EndingCluster.
    //

    ASSERT(!NT_SUCCESS(status));

    if (LastSectorLastAllocatedCluster != EndingCluster) {

        if (EndingCluster != FAT_CLUSTER_NULL) {
            FatxFreeClusters(VolumeExtension, Irp, EndingCluster, TRUE);
        } else if (InitialCluster != FAT_CLUSTER_NULL) {
            FatxFreeClusters(VolumeExtension, Irp, InitialCluster, FALSE);
        }
    }

    return status;
}

BOOLEAN
FatxFreeClusters(
    IN PFAT_VOLUME_EXTENSION VolumeExtension,
    IN PIRP Irp,
    IN ULONG ClusterNumber,
    IN BOOLEAN MarkFirstAsLast
    )
/*++

Routine Description:

    This routine converts an allocated chain of clusters to free clusters.

    This routine can optionally mark the initial cluster as FAT_CLUSTER_LAST in
    order to truncate an existing file allocation.

Arguments:

    VolumeExtension - Specifies the extension that the I/O request is for.

    Irp - Specifies the packet that describes the I/O request.

    ClusterNumber - Specifies the first cluster to mark as free or to truncate.

    MarkFirstAsLast - Specifies whether the first cluster should be marked as
        FAT_CLUSTER_LAST or FAT_CLUSTER_AVAILABLE.  This can be used to truncate
        an existing file allocation.

Return Value:

    Status of operation.

--*/
{
    BOOLEAN Result;
    ULONG SectorSize;
    ULONG SectorMask;
    ULONG FirstAvailableClusterHint;
    ULONG FatEntryFreeValue;
    BOOLEAN FlushedFirstDirtySector;
    PVOID CacheBuffer;
    ULONG CacheBufferFatByteOffset;
    ULONG CacheBufferClustersFreed;
    ULONG FatByteOffset;
    ULONG SectorFatByteOffset;
    NTSTATUS status;
    PVOID FatEntry;

    ASSERT(VolumeExtension->VolumeMutexExclusiveOwner == KeGetCurrentThread());

    SectorSize = VolumeExtension->SectorSize;
    SectorMask = SectorSize - 1;
    FirstAvailableClusterHint = VolumeExtension->FirstAvailableClusterHint;

    //
    // The below code assumes that relative byte offsets into the file
    // allocation table have the same page alignment as their corresponding
    // physical byte offsets.
    //

    ASSERT(BYTE_OFFSET(VolumeExtension->FatByteOffset) == 0);

    //
    // Check if the starting cluster number is valid to reduce the amount of
    // code at the call sites.  We may be passed a cluster number from a
    // directory entry that hasn't been validated yet.
    //

    if (!FatxIsValidCluster(VolumeExtension, ClusterNumber)) {
        return FALSE;
    }

    //
    // Compute the value that we want to be writing into the first file
    // allocation table entry.
    //

    FatEntryFreeValue = MarkFirstAsLast ? FAT_CLUSTER_LAST : FAT_CLUSTER_AVAILABLE;

    //
    // If we're truncating an existing file allocation, then the flush of the
    // first dirty sector must succeed or else we'll end up leaving a dangling
    // allocation.  If we're deleting an entire file allocation, then it's okay
    // to fail on the first write since there aren't any other pointers to the
    // allocation chain.
    //

    FlushedFirstDirtySector = (BOOLEAN)!MarkFirstAsLast;

    //
    // Walk the file allocation table freeing the cluster chain.
    //

    Result = FALSE;
    CacheBuffer = NULL;
    CacheBufferFatByteOffset = 0;
    CacheBufferClustersFreed = 0;

    for (;;) {

        //
        // Compute the relative offset to the file allocation table entry for
        // the next cluster to check and its sector aligned byte offset.
        //

        FatByteOffset = FatxClusterToFatByteOffset(VolumeExtension, ClusterNumber);
        SectorFatByteOffset = FatByteOffset & ~SectorMask;

        //
        // If we haven't mapped in a cache buffer yet or if we're switching
        // sectors, then we need to commit and switch cache buffers.
        //

        if ((CacheBuffer == NULL) ||
            (CacheBufferFatByteOffset != SectorFatByteOffset)) {

            //
            // If another cache buffer has already been mapped, then commit the
            // changes on this buffer.
            //

            if (CacheBuffer != NULL) {

                status = FscWriteBuffer(&VolumeExtension->CacheExtension,
                    Irp, VolumeExtension->FatByteOffset +
                    CacheBufferFatByteOffset, SectorSize, CacheBuffer);

                if (NT_SUCCESS(status)) {

                    //
                    // Store the first available cluster in case it changed for
                    // this cache buffer.
                    //

                    VolumeExtension->FirstAvailableClusterHint =
                        FirstAvailableClusterHint;

                    //
                    // Increase the number of clusters available on the volume
                    // now that we've committed the file allocation table
                    // sector.
                    //

                    VolumeExtension->NumberOfClustersAvailable +=
                        CacheBufferClustersFreed;

                    Result = TRUE;

                } else {

                    //
                    // If we failed to flush the first sector, then bail out
                    // now.  We can't free any additional clusters without
                    // leaving behind dangling clusters.
                    //

                    if (!FlushedFirstDirtySector) {
                        return FALSE;
                    }

                    //
                    // We failed to write out the last cache buffer, but
                    // we're going to keep trying to free the rest of the
                    // cluster chain.  Restore the first available cluster
                    // number back to a clean value.
                    //

                    FirstAvailableClusterHint =
                        VolumeExtension->FirstAvailableClusterHint;
                }

                FlushedFirstDirtySector = TRUE;
                CacheBufferClustersFreed = 0;
            }

            //
            // Map the next file allocation table page into memory.
            //

            status = FscMapBuffer(&VolumeExtension->CacheExtension, Irp,
                VolumeExtension->FatByteOffset + SectorFatByteOffset, TRUE,
                &CacheBuffer);

            if (!NT_SUCCESS(status)) {
                return Result;
            }

            CacheBufferFatByteOffset = SectorFatByteOffset;
        }

        //
        // Mark the file allocation table entry as the last allocated unit or as
        // an available entry.  We'll only mark the first cluster that we
        // process as the end of file.
        //

        FatEntry = (PUCHAR)CacheBuffer + (FatByteOffset & SectorMask);

        if ((ClusterNumber < FirstAvailableClusterHint) &&
            (FatEntryFreeValue == FAT_CLUSTER_AVAILABLE)) {
            FirstAvailableClusterHint = ClusterNumber;
        }

        ClusterNumber = FatxReadFatEntry(VolumeExtension, FatEntry);
        FatxWriteFatEntry(VolumeExtension, FatEntry, FatEntryFreeValue);

        if (FatEntryFreeValue == FAT_CLUSTER_AVAILABLE) {
            CacheBufferClustersFreed++;
        }

        //
        // All future file allocation table entries are marked as unallocated.
        //

        FatEntryFreeValue = FAT_CLUSTER_AVAILABLE;

        //
        // Verify that the next cluster entry to free is valid.  Note that this
        // check also handles the normal FAT_CLUSTER_LAST case.
        //

        if (!FatxIsValidCluster(VolumeExtension, ClusterNumber)) {
            break;
        }
    }

    //
    // At this point, we only break out of the above loop when we find an
    // invalid cluster entry.  The cluster entry is most likely the end of file
    // marker.
    //

    if (ClusterNumber != FAT_CLUSTER_LAST) {
        FatxDbgPrint(("FATX: corrupt FAT chain found while freeing clusters\n"));
    }

    //
    // We're still holding on to a dirty cache buffer, so commit the changes.
    //

    ASSERT(CacheBuffer != NULL);

    status = FscWriteBuffer(&VolumeExtension->CacheExtension, Irp,
        VolumeExtension->FatByteOffset + CacheBufferFatByteOffset, SectorSize,
        CacheBuffer);

    if (NT_SUCCESS(status)) {

        //
        // Store the first available cluster in case it changed for this cache
        // buffer.
        //

        VolumeExtension->FirstAvailableClusterHint = FirstAvailableClusterHint;

        //
        // Increase the number of clusters available on the volume now that
        // we've committed the file allocation table page.
        //

        VolumeExtension->NumberOfClustersAvailable += CacheBufferClustersFreed;

        Result = TRUE;
    }

    //
    // The root directory should always consume at least one cluster, so we
    // don't check for equality here.
    //

    ASSERT(VolumeExtension->NumberOfClustersAvailable <
        VolumeExtension->NumberOfClusters);

    return Result;
}

NTSTATUS
FatxInitializeAllocationSupport(
    IN PFAT_VOLUME_EXTENSION VolumeExtension,
    IN PIRP Irp
    )
/*++

Routine Description:

    This routine initializes the allocation support for the supplied volume.
    This includes doing things such as determining the number of free clusters
    on the volume and finding the initial cluster to use for new file
    allocations.

Arguments:

    VolumeExtension - Specifies the extension that the I/O request is for.

    Irp - Specifies the packet that describes the I/O request.

Return Value:

    Status of operation.

--*/
{
    NTSTATUS status;
    BOOLEAN FirstPage;
    ULONG NumberOfClustersAvailable;
    ULONG FirstAvailableClusterHint;
    ULONG TotalClustersRemaining;
    ULONG FatByteOffset;
    PVOID CacheBuffer;
    ULONG PageClustersRemaining;
    PUSHORT FatEntry16;
    PUSHORT EndingFatEntry16;
    PULONG FatEntry32;
    PULONG EndingFatEntry32;

    ASSERT(BYTE_OFFSET(VolumeExtension->FatByteOffset) == 0);

    //
    // Walk over the file allocation table to determine the number of available
    // clusters and other information used for cluster allocation.
    //

    status = STATUS_SUCCESS;
    FirstPage = TRUE;
    NumberOfClustersAvailable = 0;
    FirstAvailableClusterHint = MAXULONG;
    TotalClustersRemaining = VolumeExtension->NumberOfClusters +
        FAT_RESERVED_FAT_ENTRIES;
    FatByteOffset = 0;

    while (TotalClustersRemaining > 0) {

        //
        // Map the file allocation table page into memory.
        //

        status = FscMapBuffer(&VolumeExtension->CacheExtension, Irp,
            VolumeExtension->FatByteOffset + FatByteOffset, FALSE, &CacheBuffer);

        if (!NT_SUCCESS(status)) {
            break;
        }

        if (FatxIsFlagSet(VolumeExtension->Flags, FAT_VOLUME_FAT16X)) {

            PageClustersRemaining = PAGE_SIZE / sizeof(USHORT);

            if (PageClustersRemaining > TotalClustersRemaining) {
                PageClustersRemaining = TotalClustersRemaining;
            }

            FatEntry16 = (PUSHORT)CacheBuffer;
            EndingFatEntry16 = FatEntry16 + PageClustersRemaining;

            if (FirstPage) {
                FirstPage = FALSE;
                FatEntry16 += FAT_RESERVED_FAT_ENTRIES;
            }

            for (NOTHING; FatEntry16 < EndingFatEntry16; FatEntry16++) {

                if (*FatEntry16 == FAT_CLUSTER16_AVAILABLE) {

                    //
                    // If this is the first available cluster that we've seen,
                    // then save off the number.  We use this as a hint to avoid
                    // scanning the entire volume for a free cluster.
                    //

                    if (FirstAvailableClusterHint == MAXULONG) {
                        FirstAvailableClusterHint =
                            (FatByteOffset / sizeof(USHORT)) +
                            (FatEntry16 - (PUSHORT)CacheBuffer);
                    }

                    //
                    // Increment the number of available clusters for the volume.
                    //

                    NumberOfClustersAvailable++;
                }
            }

        } else {

            PageClustersRemaining = PAGE_SIZE / sizeof(ULONG);

            if (PageClustersRemaining > TotalClustersRemaining) {
                PageClustersRemaining = TotalClustersRemaining;
            }

            FatEntry32 = (PULONG)CacheBuffer;
            EndingFatEntry32 = FatEntry32 + PageClustersRemaining;

            if (FirstPage) {
                FirstPage = FALSE;
                FatEntry32 += FAT_RESERVED_FAT_ENTRIES;
            }

            for (NOTHING; FatEntry32 < EndingFatEntry32; FatEntry32++) {

                if (*FatEntry32 == FAT_CLUSTER_AVAILABLE) {

                    //
                    // If this is the first available cluster that we've seen,
                    // then save off the number.  We use this as a hint to avoid
                    // scanning the entire volume for a free cluster.
                    //

                    if (FirstAvailableClusterHint == MAXULONG) {
                        FirstAvailableClusterHint =
                            (FatByteOffset / sizeof(ULONG)) +
                            (FatEntry32 - (PULONG)CacheBuffer);
                    }

                    //
                    // Increment the number of available clusters for the volume.
                    //

                    NumberOfClustersAvailable++;
                }
            }
        }

        //
        // Unmap this cache buffer.
        //

        FscUnmapBuffer(CacheBuffer);

        //
        // Advance to the next file allocation table page.
        //

        FatByteOffset += PAGE_SIZE;
        TotalClustersRemaining -= PageClustersRemaining;
    }

    //
    // Store the results in the volume extension.
    //

    VolumeExtension->NumberOfClustersAvailable = NumberOfClustersAvailable;
    VolumeExtension->FirstAvailableClusterHint = FirstAvailableClusterHint;

    return status;
}
