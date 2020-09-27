/*++

Copyright (c) 2000-2001  Microsoft Corporation

Module Name:

    fcbsup.c

Abstract:

    This module implements routines which provide support for file control
    blocks.

--*/

#include "fatx.h"

//
// Local support.
//

VOID
FatxDbgPrintClusterCache(
    IN PFAT_FCB Fcb
    );

NTSTATUS
FatxCreateFcb(
    IN PFAT_FCB ParentFcb OPTIONAL,
    IN ULONG FirstCluster,
    IN PDIRENT DirectoryEntry OPTIONAL,
    IN ULONG DirectoryByteOffset OPTIONAL,
    OUT PFAT_FCB *ReturnedFcb
    )
/*++

Routine Description:

    This routine constructs a file control block that represents the file at the
    supplied starting cluster number and with the optionally supplied directory
    attributes.

Arguments:

    VolumeExtension - Specifies the volume that contains the file control block.

    ParentFcb - Specifies the parent directory that contains the supplied file.

    FirstCluster - Specifies the starting cluster number for the file.

    DirectoryEntry - Optionally specifies the directory entry to obtain more
        attributes about the file.

    DirectoryByteOffset - Optionally specifies the byte offset of the directory
        entry.

    ReturnedFcb - Specifies the buffer to receive the created file control
        block.

Return Value:

    Status of operation.

--*/
{
    NTSTATUS status;
    SIZE_T FcbSize;
    ULONG FcbPoolTag;
    PFAT_FCB Fcb;
    ULONG PathNameLength;
    ULONG Index;

    //
    // Compute the size of the file control block.
    //

    if (DirectoryEntry == NULL) {
        FcbSize = FAT_DIRECTORY_FCB_SIZE;
        FcbPoolTag = 'cRtF';
    } else if (FatxIsFlagSet(DirectoryEntry->FileAttributes, FILE_ATTRIBUTE_DIRECTORY)) {
        FcbSize = FAT_DIRECTORY_FCB_SIZE + DirectoryEntry->FileNameLength;
        FcbPoolTag = 'cDtF';
    } else {
        FcbSize = FAT_FILE_FCB_SIZE + DirectoryEntry->FileNameLength;
        FcbPoolTag = 'cFtF';
    }

    //
    // Allocate the file control block.
    //

    Fcb = ExAllocatePoolWithTag(FcbSize, FcbPoolTag);

    if (Fcb != NULL) {

        //
        // Initialize the file control block.
        //

        RtlZeroMemory(Fcb, FcbSize);

        //
        // A file control block starts with the single reference for the caller.
        //

        Fcb->ReferenceCount = 1;

        //
        // Store the first cluster of the stream in the file control block.  We
        // don't validate that the cluster is valid here because we need to be
        // able to create a file control block so that we can delete a corrupt
        // directory entry.
        //

        Fcb->FirstCluster = FirstCluster;
        Fcb->EndingCluster = FAT_CLUSTER_NULL;

        //
        // We don't know how many clusters have been allocated to the file yet
        // unless the starting cluster is null.
        //

        if (Fcb->FirstCluster == FAT_CLUSTER_NULL) {
            ASSERT(Fcb->AllocationSize == 0);
        } else {
            Fcb->AllocationSize = MAXULONG;
        }

        //
        // If a directory entry was supplied, then copy the attributes out of
        // the entry into the file control block.
        //

        if (DirectoryEntry != NULL) {

            ASSERT(ParentFcb != NULL);
            ASSERT(FatxIsFlagSet(ParentFcb->Flags, FAT_FCB_DIRECTORY));

            Fcb->FileAttributes = DirectoryEntry->FileAttributes;
            Fcb->FileSize = DirectoryEntry->FileSize;

            Fcb->LastAccessTime = DirectoryEntry->LastAccessTime;
            Fcb->CreationTime = DirectoryEntry->CreationTime;
            Fcb->LastWriteTime = FatxFatTimestampToTime(&DirectoryEntry->LastWriteTime);

            //
            // If the file attributes indicate that this is a directory, then
            // mark the file control block as a directory, too.  Otherwise,
            // initialize the file mutex.
            //

            if (FatxIsFlagSet(Fcb->FileAttributes, FILE_ATTRIBUTE_DIRECTORY)) {
                Fcb->Flags |= FAT_FCB_DIRECTORY;
                Fcb->FileNameBuffer = Fcb->Directory.OriginalFileName;
            } else{
                ExInitializeReadWriteLock(&Fcb->File.FileMutex);
                Fcb->FileNameBuffer = Fcb->File.OriginalFileName;
            }

            //
            // Copy the file name to the file control block.
            //

            ASSERT(DirectoryEntry->FileNameLength != 0);
            ASSERT(DirectoryEntry->FileNameLength <= FAT_FILE_NAME_LENGTH);

            Fcb->FileNameLength = DirectoryEntry->FileNameLength;

            RtlCopyMemory(Fcb->FileNameBuffer, DirectoryEntry->FileName,
                DirectoryEntry->FileNameLength);

            //
            // Store the byte offset of the directory entry so that we don't
            // have to search for the entry again when making changes.
            //

            Fcb->DirectoryByteOffset = DirectoryByteOffset;

            //
            // Increment the reference count for the parent file control block
            // and attach it to this file control block.
            //

            ParentFcb->ReferenceCount++;
            Fcb->ParentFcb = ParentFcb;

            //
            // Compute the length of the path name.
            //

            PathNameLength = ParentFcb->PathNameLength +
                sizeof(OBJ_NAME_PATH_SEPARATOR) + Fcb->FileNameLength;
            Fcb->PathNameLength = (UCHAR)PathNameLength;

            ASSERT(PathNameLength <= FAT_PATH_NAME_LIMIT);

            //
            // Insert the file control block into the child list of the parent
            // file control block.
            //

            InsertHeadList(&ParentFcb->Directory.ChildFcbList, &Fcb->SiblingFcbLink);

        } else {

            //
            // The only time that a directory entry is not supplied is when
            // we're creating the file control block for the root directory.
            //

            Fcb->Flags |= FAT_FCB_DIRECTORY | FAT_FCB_ROOT_DIRECTORY;
            Fcb->FileAttributes = FILE_ATTRIBUTE_DIRECTORY;
            ASSERT(Fcb->PathNameLength == 0);
        }

        //
        // For directories, initialize the child file control block list as
        // empty and set the directory entry lookup hint to zero.
        //

        if (FatxIsFlagSet(Fcb->Flags, FAT_FCB_DIRECTORY)) {
            InitializeListHead(&Fcb->Directory.ChildFcbList);
            Fcb->Directory.DirectoryByteOffsetLookupHint = 0;
        }

        //
        // Initialize the cluster cache for the file control block.  The cluster
        // cache has already been zeroed above.  The ClusterRunLength fields
        // must be zero in order for the elements to appear as free.  The LRU
        // list head is also zero.
        //

        if (FatxIsFlagSet(Fcb->Flags, FAT_FCB_DIRECTORY)) {

            //
            // For directories, the initialization is trivial because the
            // cluster cache is small.
            //

            ASSERT(FAT_DIRECTORY_CLUSTER_CACHE_ENTRIES == 2);

            Fcb->Directory.ClusterCache[0].LruBlink = 1;
            Fcb->Directory.ClusterCache[0].LruFlink = 1;

        } else {

            //
            // For files, chain each entry to its neighbors and then go back and
            // fix up the first and last elements of the cache to correctly wrap
            // around.
            //

            ASSERT(FAT_FILE_CLUSTER_CACHE_ENTRIES > 2);

            for (Index = 0; Index < FAT_FILE_CLUSTER_CACHE_ENTRIES; Index++) {
                Fcb->File.ClusterCache[Index].LruBlink = Index - 1;
                Fcb->File.ClusterCache[Index].LruFlink = Index + 1;
            }

            Fcb->File.ClusterCache[0].LruBlink = FAT_FILE_CLUSTER_CACHE_ENTRIES - 1;
            Fcb->File.ClusterCache[FAT_FILE_CLUSTER_CACHE_ENTRIES - 1].LruFlink = 0;
        }

        status = STATUS_SUCCESS;

    } else {
        status = STATUS_INSUFFICIENT_RESOURCES;
    }

    *ReturnedFcb = Fcb;

    return status;
}

BOOLEAN
FatxFindOpenChildFcb(
    IN PFAT_FCB DirectoryFcb,
    IN POBJECT_STRING FileName,
    OUT PFAT_FCB *ReturnedFcb
    )
/*++

Routine Description:

    This routine searches the child file control block for the supplied
    directory for a file that has the supplied file name.

Arguments:

    DirectoryFcb - Specifies the file control block of the directory to search.

    FileName - Specifies the name to search for in the directory.

    ReturnedFcb - Specifies the buffer to receive the found file control block.

Return Value:

    Returns TRUE if the file control block was found, else FALSE.

--*/
{
    PLIST_ENTRY NextFcbLink;
    PFAT_FCB Fcb;
    OBJECT_STRING FcbFileName;

    ASSERT(FatxIsFlagSet(DirectoryFcb->Flags, FAT_FCB_DIRECTORY));

    //
    // Walk through the file control blocks actively in use by the volume and
    // find a match.
    //

    NextFcbLink = DirectoryFcb->Directory.ChildFcbList.Flink;

    while (NextFcbLink != &DirectoryFcb->Directory.ChildFcbList) {

        Fcb = CONTAINING_RECORD(NextFcbLink, FAT_FCB, SiblingFcbLink);

        FcbFileName.Length = Fcb->FileNameLength;
        FcbFileName.Buffer = Fcb->FileNameBuffer;

        if ((FcbFileName.Length == FileName->Length) &&
            RtlEqualObjectString(&FcbFileName, FileName, TRUE)) {
            *ReturnedFcb = Fcb;
            return TRUE;
        }

        NextFcbLink = Fcb->SiblingFcbLink.Flink;
    }

    *ReturnedFcb = NULL;
    return FALSE;
}

NTSTATUS
FatxUpdateDirectoryEntry(
    IN PFAT_VOLUME_EXTENSION VolumeExtension,
    IN PIRP Irp,
    IN PFAT_FCB Fcb
    )
/*++

Routine Description:

    This routine writes out the directory entry for the supplied file using the
    in-memory directory information.

Arguments:

    VolumeExtension - Specifies the extension that the I/O request is for.

    Irp - Specifies the packet that describes the I/O request.

    Fcb - Specifies the file control block that describes the directory entry to
        update.

Return Value:

    Status of operation.

--*/
{
    NTSTATUS status;
    ULONGLONG PhysicalByteOffset;
    ULONG PhysicalRunLength;
    PVOID CacheBuffer;
    PDIRENT DirectoryEntry;

    ASSERT(FatxIsFlagClear(Fcb->Flags, FAT_FCB_VOLUME | FAT_FCB_ROOT_DIRECTORY));
    ASSERT(VolumeExtension->VolumeMutexExclusiveOwner == KeGetCurrentThread());

    //
    // It's possible for the file control block to be NULL if a file failed to
    // be moved from one directory to another.  In that case,
    // FatxSetRenameInformation sets the file's parent file control block to
    // NULL.
    //

    if (Fcb->ParentFcb == NULL) {
        return STATUS_FILE_CORRUPT_ERROR;
    }

    //
    // Get the physical byte offset corresponding to the file's directory byte
    // offset.
    //

    status = FatxFileByteOffsetToPhysicalByteOffset(VolumeExtension, Irp,
        Fcb->ParentFcb, Fcb->DirectoryByteOffset, FALSE, &PhysicalByteOffset,
        &PhysicalRunLength);

    if (!NT_SUCCESS(status)) {
        return status;
    }

    //
    // Map the directory entry into the cache.
    //

    status = FscMapBuffer(&VolumeExtension->CacheExtension, Irp,
        PhysicalByteOffset, TRUE, &CacheBuffer);

    if (!NT_SUCCESS(status)) {
        return status;
    }

    //
    // Apply the information in the file control block to the directory entry.
    //

    DirectoryEntry = (PDIRENT)CacheBuffer;

    DirectoryEntry->FileNameLength = Fcb->FileNameLength;

    RtlCopyMemory(DirectoryEntry->FileName, Fcb->FileNameBuffer,
        Fcb->FileNameLength);

    DirectoryEntry->FileAttributes = Fcb->FileAttributes;
    DirectoryEntry->FirstCluster = Fcb->FirstCluster;
    DirectoryEntry->FileSize = Fcb->FileSize;
    DirectoryEntry->CreationTime = Fcb->CreationTime;
    DirectoryEntry->LastAccessTime = Fcb->LastAccessTime;

    FatxTimeToFatTimestamp(&Fcb->LastWriteTime, &DirectoryEntry->LastWriteTime);

    //
    // Write the change back out the disk and unmap the cache buffer.
    //

    status = FscWriteBuffer(&VolumeExtension->CacheExtension, Irp,
        PhysicalByteOffset, sizeof(DIRENT), CacheBuffer);

    if (NT_SUCCESS(status)) {
        Fcb->Flags &= ~FAT_FCB_UPDATE_DIRECTORY_ENTRY;
    }

    return status;
}

NTSTATUS
FatxMarkDirectoryEntryDeleted(
    IN PFAT_VOLUME_EXTENSION VolumeExtension,
    IN PIRP Irp,
    IN PFAT_FCB Fcb
    )
/*++

Routine Description:

    This routine marks a directory entry as being deleted.

Arguments:

    VolumeExtension - Specifies the extension that the I/O request is for.

    Irp - Specifies the packet that describes the I/O request.

    Fcb - Specifies the file control block that describes the directory entry to
        mark as deleted.

Return Value:

    Status of operation.

--*/
{
    NTSTATUS status;
    ULONGLONG PhysicalByteOffset;
    ULONG PhysicalRunLength;
    PVOID CacheBuffer;
    PDIRENT DirectoryEntry;

    ASSERT(FatxIsFlagClear(Fcb->Flags, FAT_FCB_VOLUME | FAT_FCB_ROOT_DIRECTORY));
    ASSERT(VolumeExtension->VolumeMutexExclusiveOwner == KeGetCurrentThread());

    //
    // It's possible for the file control block to be NULL if a file failed to
    // be moved from one directory to another.  In that case,
    // FatxSetRenameInformation sets the file's parent file control block to
    // NULL.
    //

    if (Fcb->ParentFcb == NULL) {
        return STATUS_FILE_CORRUPT_ERROR;
    }

    //
    // Get the physical byte offset corresponding to the file's directory byte
    // offset.
    //

    status = FatxFileByteOffsetToPhysicalByteOffset(VolumeExtension, Irp,
        Fcb->ParentFcb, Fcb->DirectoryByteOffset, FALSE, &PhysicalByteOffset,
        &PhysicalRunLength);

    if (!NT_SUCCESS(status)) {
        return status;
    }

    //
    // Map the directory entry into the cache.
    //

    status = FscMapBuffer(&VolumeExtension->CacheExtension, Irp,
        PhysicalByteOffset, TRUE, &CacheBuffer);

    if (!NT_SUCCESS(status)) {
        return status;
    }

    //
    // Apply the information in the file control block to the directory entry.
    //

    DirectoryEntry = (PDIRENT)CacheBuffer;

    DirectoryEntry->FileNameLength = FAT_DIRENT_DELETED;

    //
    // Write the change back out the disk and unmap the cache buffer.
    //

    status = FscWriteBuffer(&VolumeExtension->CacheExtension, Irp,
        PhysicalByteOffset, sizeof(DIRENT), CacheBuffer);

    return status;
}

VOID
FatxMoveClusterCacheEntryToTail(
    IN PFAT_FCB Fcb,
    IN PFAT_FCB_CLUSTER_CACHE_ENTRY ClusterCache,
    IN ULONG ClusterCacheIndex
    )
/*++

Routine Description:

    This routine updates the LRU links for the supplied cluster cache such that
    the supplied cluster cache index is at the end of the LRU list.

Arguments:

    Fcb - Specifies the file control block that contains the cluster cache.

    ClusterCache - Specifies the pointer to the first entry of the cluster
        cache.

    ClusterCacheIndex - Specifies the index of the element to move to the end of
        the LRU list.  This index cannot be the current head of the LRU list
        (instead just change the LRU list head to be this entry's forward link).

Return Value:

    None.

--*/
{
    PFAT_FCB_CLUSTER_CACHE_ENTRY ClusterCacheEntry;
    PFAT_FCB_CLUSTER_CACHE_ENTRY CurrentHeadCacheEntry;

    ASSERT(ClusterCacheIndex != Fcb->ClusterCacheLruHead);

    ClusterCacheEntry = &ClusterCache[ClusterCacheIndex];
    CurrentHeadCacheEntry = &ClusterCache[Fcb->ClusterCacheLruHead];

    //
    // Unlink the element that we're moving to the end of the LRU list from its
    // neighbors.
    //

    ClusterCache[ClusterCacheEntry->LruBlink].LruFlink =
        ClusterCacheEntry->LruFlink;
    ClusterCache[ClusterCacheEntry->LruFlink].LruBlink =
        ClusterCacheEntry->LruBlink;

    //
    // Link the element that we're moving to the end of the LRU list to the head
    // of the LRU list and the old tail of the LRU list.
    //

    ClusterCacheEntry->LruFlink = Fcb->ClusterCacheLruHead;
    ClusterCacheEntry->LruBlink = CurrentHeadCacheEntry->LruBlink;

    //
    // Link the old tail of the LRU list and the head of the LRU list to the
    // element that we're moving to the end of the LRU list
    //

    ClusterCache[CurrentHeadCacheEntry->LruBlink].LruFlink = ClusterCacheIndex;
    CurrentHeadCacheEntry->LruBlink = ClusterCacheIndex;
}

BOOLEAN
FatxFillEmptyClusterCacheEntry(
    IN PFAT_FCB Fcb,
    IN PFAT_FCB_CLUSTER_CACHE_ENTRY ClusterCache,
    IN ULONG PhysicalClusterNumber,
    IN ULONG FileClusterNumber,
    IN ULONG ClusterRunLength
    )
/*++

Routine Description:

    This routine fills an empty cluster cache entry with the supplied cluster
    run.

Arguments:

    Fcb - Specifies the file control block that contains the cluster cache.

    ClusterCache - Specifies the pointer to the first entry of the cluster
        cache.

    PhysicalClusterNumber - Specifies the first physical cluster of the run.

    FileClusterNumber - Specifies the file cluster that the first physical
        cluster maps to.

    ClusterRunLength - Specifies the number of clusters in the run.

Return Value:

    Returns TRUE if there are more empty slots available in the cluster cache,
    else FALSE.

--*/
{
    ULONG ClusterCacheIndex;
    PFAT_FCB_CLUSTER_CACHE_ENTRY ClusterCacheEntry;

    //
    // Search the cluster cache for the first empty cache entry.
    //

    ClusterCacheIndex = Fcb->ClusterCacheLruHead;

    do {

        ClusterCacheEntry = &ClusterCache[ClusterCacheIndex];

        //
        // If this is an empty cache entry, then fill in the cache entry with
        // the supplied cluster run and return whether or not there are more
        // empty cache entries.
        //

        if (ClusterCacheEntry->ClusterRunLength == 0) {

            ClusterCacheEntry->PhysicalClusterNumber = PhysicalClusterNumber;
            ClusterCacheEntry->FileClusterNumber = FileClusterNumber;
            ClusterCacheEntry->ClusterRunLength = ClusterRunLength;

            ClusterCacheIndex = (UCHAR)ClusterCacheEntry->LruFlink;
            ClusterCacheEntry = &ClusterCache[ClusterCacheIndex];

            return (BOOLEAN)(ClusterCacheEntry->ClusterRunLength == 0);
        }

        //
        // Advance to the next LRU index.
        //

        ClusterCacheIndex = (UCHAR)ClusterCacheEntry->LruFlink;

    } while (ClusterCacheIndex != Fcb->ClusterCacheLruHead);

    //
    // No empty entries were found.  Return FALSE so that the caller doesn't
    // keep trying to fill empty cache entries.
    //

    return FALSE;
}

NTSTATUS
FatxFileByteOffsetToCluster(
    IN PFAT_VOLUME_EXTENSION VolumeExtension,
    IN PIRP Irp,
    IN PFAT_FCB Fcb,
    IN ULONG FileByteOffset,
    OUT PULONG ReturnedClusterNumber,
    OUT PULONG ReturnedClusterRunLength OPTIONAL
    )
/*++

Routine Description:

    This routine returns the cluster number for the file byte offset into the
    supplied file.

    If this routine is given a file byte offset beyond the end of the allocated
    cluster chain, then STATUS_END_OF_FILE is returned.

Arguments:

    VolumeExtension - Specifies the extension that the I/O request is for.

    Irp - Specifies the packet that describes the I/O request.

    Fcb - Specifies the file control block to retrieve the next cluster for.

    FileByteOffset - Specifies the file byte offset to retrive the cluster
        number for.

    ReturnedClusterNumber - Specifies the buffer to receive the cluster value
        for the supplied file byte offset.

    ReturnedClusterRunLength - Specifies the optional buffer to receive the
        number of adjacent clusters that can be accessed relative to the
        starting cluster number.

Return Value:

    Status of operation.

--*/
{
    NTSTATUS status;
    PFAT_FCB_CLUSTER_CACHE_ENTRY ClusterCache;
    BOOLEAN HaveEmptyClusterCacheEntries;
    ULONG RequestedFileClusterNumber;
    UCHAR ClusterCacheIndex;
    PFAT_FCB_CLUSTER_CACHE_ENTRY NearestCacheEntry;
    PFAT_FCB_CLUSTER_CACHE_ENTRY ClusterCacheEntry;
    PFAT_FCB_CLUSTER_CACHE_ENTRY CurrentHeadCacheEntry;
    ULONG PhysicalClusterNumber;
    ULONG FileClusterNumber;
    ULONG ClusterRunLength;
    PVOID CacheBuffer;
    ULONG CacheBufferFatByteOffset;
    ULONG StartingPhysicalClusterNumber;
    ULONG StartingFileClusterNumber;
    ULONG FatByteOffset;
    ULONG LastPhysicalClusterNumber;
    PVOID FatEntry;

    //
    // The volume mutex should be acquired for either exclusive or shared access
    // in order to synchronize with FatxAllocateClusters and FatxFreeClusters.
    //

    ASSERT(ExDbgIsReadWriteLockLocked(&VolumeExtension->VolumeMutex));

    //
    // Make sure that we weren't called with the file control block for the
    // volume.  Volume file control blocks don't have a cache allocated.
    //

    ASSERT(FatxIsFlagClear(Fcb->Flags, FAT_FCB_VOLUME));

    //
    // Synchronize access to the cluster cache using a special volume mutex.  At
    // this point, the volume mutex may only be held in shared mode so to guard
    // against multiple writers in this code path, we need an additional mutex.
    //

    FatxAcquireClusterCacheMutex(VolumeExtension);

    //
    // The below code assumes that relative byte offsets into the file
    // allocation table have the same page alignment as their corresponding
    // physical byte offsets.
    //

    ASSERT(BYTE_OFFSET(VolumeExtension->FatByteOffset) == 0);

    //
    // If we're requesting a file byte offset beyond the known end of file, then
    // bail out now.
    //

    if ((Fcb->AllocationSize != MAXULONG) &&
        (FileByteOffset >= Fcb->AllocationSize)) {
        FatxReleaseClusterCacheMutex(VolumeExtension);
        return STATUS_END_OF_FILE;
    }

    //
    // Obtain the base pointer to the cluster cache for the given type of file
    // control block.
    //

    if (FatxIsFlagSet(Fcb->Flags, FAT_FCB_DIRECTORY)) {
        ClusterCache = Fcb->Directory.ClusterCache;
    } else {
        ClusterCache = Fcb->File.ClusterCache;
    }

    //
    // Convert the file byte offset into its file cluster number.
    //

    RequestedFileClusterNumber = FileByteOffset >> VolumeExtension->ClusterShift;

    //
    // Search the cluster cache for the file cluster number.
    //

    HaveEmptyClusterCacheEntries = FALSE;
    ClusterCacheIndex = Fcb->ClusterCacheLruHead;
    NearestCacheEntry = NULL;

    do {

        ClusterCacheEntry = &ClusterCache[ClusterCacheIndex];

        //
        // If this is an empty cache entry, then break out now.  Set a flag so
        // that below code knows that we have at least one empty cache entry
        // that can be populated with cluster runs that are found while
        // searching for the requested file cluster number.
        //

        if (ClusterCacheEntry->ClusterRunLength == 0) {
            HaveEmptyClusterCacheEntries = TRUE;
            break;
        }

        //
        // Check if the cache entry contains the requested file cluster number.
        //

        if (RequestedFileClusterNumber >= ClusterCacheEntry->FileClusterNumber) {

            if (RequestedFileClusterNumber < ClusterCacheEntry->FileClusterNumber +
                ClusterCacheEntry->ClusterRunLength) {

                //
                // Move the entry to the top of the LRU list, if it's not
                // already the top element.
                //

MoveAndMatchCurrentCacheEntry:
                if (ClusterCacheIndex != Fcb->ClusterCacheLruHead) {

                    CurrentHeadCacheEntry = &ClusterCache[Fcb->ClusterCacheLruHead];

                    //
                    // If the index that we're moving to the top of the LRU list
                    // is the element at the bottom of the LRU list, then we can
                    // simply change the LRU head index to point at the bottom
                    // index.  The list links are already set up.  This is the
                    // common case for sequential access through the file.
                    //

                    if (ClusterCacheIndex != CurrentHeadCacheEntry->LruBlink) {
                        FatxMoveClusterCacheEntryToTail(Fcb, ClusterCache,
                            ClusterCacheIndex);
                    }

                    Fcb->ClusterCacheLruHead = ClusterCacheIndex;
                }

                //
                // Compute the actual cluster number.
                //

                PhysicalClusterNumber = ClusterCacheEntry->PhysicalClusterNumber +
                    (RequestedFileClusterNumber -
                    ClusterCacheEntry->FileClusterNumber);

                *ReturnedClusterNumber = PhysicalClusterNumber;

                //
                // Compute the number of clusters in this run.
                //

                if (ReturnedClusterRunLength != NULL) {

                    ClusterRunLength = ClusterCacheEntry->ClusterRunLength -
                        (PhysicalClusterNumber -
                        ClusterCacheEntry->PhysicalClusterNumber);
                    ASSERT(ClusterRunLength >= 1);

                    *ReturnedClusterRunLength = ClusterRunLength;
                }

                FatxReleaseClusterCacheMutex(VolumeExtension);
                return STATUS_SUCCESS;
            }

            //
            // The cache entry doesn't contain the requested file cluster
            // number.  If the cache entry's file cluster number is the nearest
            // file cluster number we have found before the requested file
            // cluster number, then keep a pointer to it.  If we need to go back
            // to the disk to read the FAT chain, we can start at this cluster.
            //

            if ((NearestCacheEntry == NULL) ||
                (ClusterCacheEntry->FileClusterNumber > NearestCacheEntry->FileClusterNumber)) {
                NearestCacheEntry = ClusterCacheEntry;
            }
        }

        //
        // Advance to the next LRU index.
        //

        ClusterCacheIndex = (UCHAR)ClusterCacheEntry->LruFlink;

    } while (ClusterCacheIndex != Fcb->ClusterCacheLruHead);

    //
    // The file cluster number wasn't found in the cache.  We'll have to go back
    // to the file allocation table to retrieve the mapping.
    //

    if (NearestCacheEntry != NULL) {

        //
        // We found a cache entry that was before the requested cluster number.
        // We can start the linear search at this point.
        //

        PhysicalClusterNumber = NearestCacheEntry->PhysicalClusterNumber +
            NearestCacheEntry->ClusterRunLength - 1;
        FileClusterNumber = NearestCacheEntry->FileClusterNumber +
            NearestCacheEntry->ClusterRunLength - 1;

        ASSERT(FatxIsValidCluster(VolumeExtension, PhysicalClusterNumber));

    } else {

        //
        // We didn't find a cache entry before the request cluster number, so we
        // have to start from the first cluster of the file.
        //

        LastPhysicalClusterNumber = FAT_CLUSTER_NULL;
        PhysicalClusterNumber = Fcb->FirstCluster;
        FileClusterNumber = 0;

        //
        // Check that the cluster number is valid.
        //

        if (!FatxIsValidCluster(VolumeExtension, PhysicalClusterNumber)) {

            //
            // If the first cluster of the file is zero and this is not a
            // directory, then no space has been allocated to the file, so
            // change the cluster number to the end of file marker for the sake
            // of the below checks.  Directories must have one cluster allocated
            // to them, so it's invalid to see an empty cluster chain at this
            // point.
            //

            if ((PhysicalClusterNumber == FAT_CLUSTER_NULL) &&
                FatxIsFlagClear(Fcb->Flags, FAT_FCB_DIRECTORY)) {
                PhysicalClusterNumber = FAT_CLUSTER_LAST;
            }

            //
            // Return the appropriate error code depending on whether we found
            // the end of file marker or not.  Now that we've found the end of
            // file, we can fill in the number of clusters actually allocated
            // to the file.
            //

FoundInvalidClusterNumber:
            if (PhysicalClusterNumber == FAT_CLUSTER_LAST) {
                Fcb->AllocationSize = FileClusterNumber <<
                    VolumeExtension->ClusterShift;
                Fcb->EndingCluster = LastPhysicalClusterNumber;
                status = STATUS_END_OF_FILE;
            } else {
                status = STATUS_FILE_CORRUPT_ERROR;
            }

            FatxReleaseClusterCacheMutex(VolumeExtension);
            return status;
        }
    }

    //
    // Walk the file allocation table chain until we find the requested cluster
    // number.
    //

    CacheBuffer = NULL;
    CacheBufferFatByteOffset = 0;
    ClusterRunLength = 1;
    StartingPhysicalClusterNumber = PhysicalClusterNumber;
    StartingFileClusterNumber = FileClusterNumber;

    while (FileClusterNumber < RequestedFileClusterNumber) {

        //
        // Advance to the next file cluster.
        //

        FileClusterNumber++;

        //
        // Convert the cluster number to its physical file allocation table byte
        // offset.
        //

        FatByteOffset = FatxClusterToFatByteOffset(VolumeExtension,
            PhysicalClusterNumber);

        //
        // If this is the first iteration of the loop or if the next file
        // allocation table entry sits on a different page, then switch to a new
        // cache buffer page.
        //

        if ((CacheBuffer == NULL) ||
            (CacheBufferFatByteOffset != (ULONG)PAGE_ALIGN(FatByteOffset))) {

            //
            // If another cache page has already been mapped, then unmap it now.
            //

            if (CacheBuffer != NULL) {
                FscUnmapBuffer(CacheBuffer);
            }

            //
            // Compute the page aligned byte offset for the desired cache page.
            //

            CacheBufferFatByteOffset = (ULONG)PAGE_ALIGN(FatByteOffset);

            //
            // Map the file allocation table page into memory.
            //

            status = FscMapBuffer(&VolumeExtension->CacheExtension, Irp,
                VolumeExtension->FatByteOffset + CacheBufferFatByteOffset,
                FALSE, &CacheBuffer);

            if (!NT_SUCCESS(status)) {
                FatxReleaseClusterCacheMutex(VolumeExtension);
                return status;
            }
        }

        //
        // Keep track of the last physical cluster number and lookup the next
        // physical cluster number.
        //

        LastPhysicalClusterNumber = PhysicalClusterNumber;
        FatEntry = (PUCHAR)CacheBuffer + BYTE_OFFSET(FatByteOffset);
        PhysicalClusterNumber = FatxReadFatEntry(VolumeExtension, FatEntry);

        //
        // Check that the cluster number is valid.
        //

        if (!FatxIsValidCluster(VolumeExtension, PhysicalClusterNumber)) {
            FscUnmapBuffer(CacheBuffer);
            goto FoundInvalidClusterNumber;
        }

        //
        // If the last and current physical clusters are adjacent, then increase
        // the cluster run length.  Otherwise, this is a new run of at least one
        // cluster.
        //

        if (LastPhysicalClusterNumber + 1 != PhysicalClusterNumber) {

            if (NearestCacheEntry != NULL) {

                //
                // If NearestCacheEntry is non-NULL, then we must have started
                // scanning clusters from this entry and not have found any
                // intermediate cluster runs.  We can increase the length of
                // this cluster run.  NearestCacheEntry is nulled out so that on
                // subsequent iterations of this loop, we don't continue adding
                // clusters to the run and so that we we break out of the loop
                // we don't attempt to add the clusters to the run.
                //

                ASSERT(StartingPhysicalClusterNumber ==
                    (NearestCacheEntry->PhysicalClusterNumber + NearestCacheEntry->ClusterRunLength - 1));
                ASSERT(StartingFileClusterNumber ==
                    (NearestCacheEntry->FileClusterNumber + NearestCacheEntry->ClusterRunLength - 1));

                NearestCacheEntry->ClusterRunLength += (ClusterRunLength - 1);
                NearestCacheEntry = NULL;

            } else {

                //
                // If there are empty entries in the cluster cache, then go
                // ahead and fill one with the run we just found.  We may not
                // end up using it, but it's better then just losing the work
                // we've already performed.
                //

                if (HaveEmptyClusterCacheEntries) {
                    HaveEmptyClusterCacheEntries =
                        FatxFillEmptyClusterCacheEntry(Fcb, ClusterCache,
                            StartingPhysicalClusterNumber,
                            StartingFileClusterNumber, ClusterRunLength);
                }
            }

            ClusterRunLength = 1;
            StartingPhysicalClusterNumber = PhysicalClusterNumber;
            StartingFileClusterNumber = FileClusterNumber;

        } else {
            ClusterRunLength++;
        }
    }

    //
    // While we have one of the pages of the file allocation table mapped
    // into memory, then try to find as many adjacent clusters as possible.
    //

    if (CacheBuffer != NULL) {

        for (;;) {

            //
            // Convert the cluster number to its physical file allocation table
            // byte offset.
            //

            FatByteOffset = FatxClusterToFatByteOffset(VolumeExtension,
                PhysicalClusterNumber);

            //
            // Break out of the loop if this file allocation table entry is on a
            // different page from the current cache buffer page.
            //

            if (CacheBufferFatByteOffset != (ULONG)PAGE_ALIGN(FatByteOffset)) {
                break;
            }

            //
            // Keep track of the last physical cluster number and lookup the
            // next physical cluster number.
            //

            LastPhysicalClusterNumber = PhysicalClusterNumber;
            FatEntry = (PUCHAR)CacheBuffer + BYTE_OFFSET(FatByteOffset);
            PhysicalClusterNumber = FatxReadFatEntry(VolumeExtension, FatEntry);

            //
            // Increase the length of the cluster run if the last and current
            // physical clusters are adjacent, otherwise break out now.
            //

            if (LastPhysicalClusterNumber + 1 != PhysicalClusterNumber) {

                if (PhysicalClusterNumber == FAT_CLUSTER_LAST) {

                    //
                    // If we found the last cluster of the file, then go ahead and
                    // and fill in the number of clusters allocated to the file
                    // now so that we don't have to bother computing it later.
                    //

                    Fcb->AllocationSize = (StartingFileClusterNumber +
                        ClusterRunLength) << VolumeExtension->ClusterShift;
                    Fcb->EndingCluster = LastPhysicalClusterNumber;

                } else if (FatxIsValidCluster(VolumeExtension, PhysicalClusterNumber)) {

                    //
                    // If there are empty entries in the cluster cache, then go
                    // ahead and fill one with a single entry cluster run for
                    // the next file cluster.  We may not end up using it, but
                    // it avoids us having to map in this cache page again just
                    // to find the next link.
                    //
                    // The cache shouldn't already contain an entry for this
                    // starting file cluster number.  We know that the cache
                    // isn't empty, so we haven't started discarding entries
                    // yet.  If we had already visited this new starting file
                    // cluster number, then the cache would have already had an
                    // entry for the requested file cluster number and we would
                    // never have reached this code.
                    //

                    if (HaveEmptyClusterCacheEntries) {
                        HaveEmptyClusterCacheEntries =
                            FatxFillEmptyClusterCacheEntry(Fcb, ClusterCache,
                                PhysicalClusterNumber,
                                StartingFileClusterNumber + ClusterRunLength, 1);
                    }
                }

                break;

            } else {
                ClusterRunLength++;
            }
        }

        FscUnmapBuffer(CacheBuffer);
    }

    //
    // If NearestCacheEntry is non-NULL, then we must have started scanning for
    // clusters from this entry and not found any discontiguous cluster runs in
    // the above loop.  Increase the length of the existing cache entry.
    //
    // Jump back to the above code that moves the current cache entry to the top
    // of the cluster cache and returns.
    //

    if (NearestCacheEntry != NULL) {

        ASSERT(StartingPhysicalClusterNumber ==
            (NearestCacheEntry->PhysicalClusterNumber + NearestCacheEntry->ClusterRunLength - 1));
        ASSERT(StartingFileClusterNumber ==
            (NearestCacheEntry->FileClusterNumber + NearestCacheEntry->ClusterRunLength - 1));

        NearestCacheEntry->ClusterRunLength += (ClusterRunLength - 1);

        ClusterCacheIndex = (UCHAR)(NearestCacheEntry - ClusterCache);
        ClusterCacheEntry = NearestCacheEntry;

        goto MoveAndMatchCurrentCacheEntry;
    }

    //
    // Grab the tail cluster cache entry, which is either empty or is the least
    // recently used entry, and fill it in with the cluster run.
    //
    // Jump back to the above code that moves the current cache entry to the top
    // of the cluster cache and returns.
    //

    ClusterCacheIndex = (UCHAR)ClusterCache[Fcb->ClusterCacheLruHead].LruBlink;
    ClusterCacheEntry = &ClusterCache[ClusterCacheIndex];

    ClusterCacheEntry->PhysicalClusterNumber = StartingPhysicalClusterNumber;
    ClusterCacheEntry->FileClusterNumber = StartingFileClusterNumber;
    ClusterCacheEntry->ClusterRunLength = ClusterRunLength;

    goto MoveAndMatchCurrentCacheEntry;
}

NTSTATUS
FatxFileByteOffsetToPhysicalByteOffset(
    IN PFAT_VOLUME_EXTENSION VolumeExtension,
    IN PIRP Irp,
    IN PFAT_FCB Fcb,
    IN ULONG FileByteOffset,
    IN BOOLEAN AcquireVolumeMutexShared,
    OUT PULONGLONG ReturnedPhysicalByteOffset,
    OUT PULONG ReturnedPhysicalRunLength
    )
/*++

Routine Description:

    This routine returns the physical byte offset for the file byte offset into
    the supplied file.

    If this routine is given a file byte offset beyond the end of the allocated
    cluster chain, then STATUS_FILE_CORRUPT_ERROR is returned.

Arguments:

    VolumeExtension - Specifies the extension that the I/O request is for.

    Irp - Specifies the packet that describes the I/O request.

    Fcb - Specifies the file control block to retrieve the next cluster for.

    FileByteOffset - Specifies the file byte offset to retrive the cluster
        number for.

    AcquireVolumeMutexShared - Specifies whether or not to acquire the volume
        mutex for shared access.

    ReturnedPhysicalByteOffset - Specifies the buffer to receive the physical
        byte offset for the supplied file byte offset.

    ReturnedPhysicalRunLength - Specifies the optional buffer to receive the
        number of adjacent bytes that can be accessed relative to the starting
        physical byte offset.

Return Value:

    Status of operation.

--*/
{
    NTSTATUS status;
    ULONG ClusterNumber;
    ULONG ClusterRunLength;
    ULONG ByteOffsetIntoCluster;

    if (AcquireVolumeMutexShared) {
        FatxAcquireVolumeMutexShared(VolumeExtension);
    }

    status = FatxFileByteOffsetToCluster(VolumeExtension, Irp, Fcb,
        FileByteOffset, &ClusterNumber, &ClusterRunLength);

    if (AcquireVolumeMutexShared) {
        FatxReleaseVolumeMutex(VolumeExtension);
    }

    if (NT_SUCCESS(status)) {

        ByteOffsetIntoCluster = FatxByteOffsetIntoCluster(VolumeExtension,
            FileByteOffset);

        *ReturnedPhysicalByteOffset =
            FatxClusterToPhysicalByteOffset(VolumeExtension, ClusterNumber) +
            ByteOffsetIntoCluster;

        *ReturnedPhysicalRunLength =
            (ClusterRunLength << VolumeExtension->ClusterShift) -
            ByteOffsetIntoCluster;

    } else {

        //
        // For every client of this routine, the file byte offset should already
        // be known to be valid, so if we see end of file here, then convert to
        // a file corrupt error.
        //

        if (status == STATUS_END_OF_FILE) {
            status = STATUS_FILE_CORRUPT_ERROR;
        }
    }

    return status;
}

VOID
FatxAppendClusterRunsToClusterCache(
    IN PFAT_FCB Fcb,
    IN ULONG FileClusterNumber,
    IN FAT_CLUSTER_RUN ClusterRuns[FAT_MAXIMUM_CLUSTER_RUNS],
    IN ULONG NumberOfClusterRuns
    )
/*++

Routine Description:

    This routine appends a series of cluster runs to the cluster cache of the
    supplied file.

Arguments:

    Fcb - Specifies the file control block that contains the cluster cache that
        will be appended to.

    FileClusterNumber - Specifies the starting logical cluster number for the
        supplied cluster runs.

    ClusterRuns - Specifies the cluster runs that will be appended to the
        cluster cache.

    NumberOfClusterRuns - Specifies the number of cluster runs that are valid.

Return Value:

    None.

--*/
{
    PFAT_FCB_CLUSTER_CACHE_ENTRY ClusterCache;
    PFAT_FCB_CLUSTER_CACHE_ENTRY ClusterCacheEntry;
    ULONG ClusterCacheIndex;
    PFAT_CLUSTER_RUN CurrentClusterRun;

    ASSERT(FatxIsFlagClear(Fcb->Flags, FAT_FCB_VOLUME));
    ASSERT(Fcb->AllocationSize != MAXULONG);
    ASSERT(NumberOfClusterRuns >= 1);

    //
    // Note that no explicit synchronization is required here.  The cluster
    // cache is appended to only when modifying the allocation of an existing
    // file.  For files, we have exclusive access to the file's mutex.  For
    // directories, we have exclusive access to the volume's mutex.  In either
    // case, no reader can enter the file system and end up using this file's
    // cluster cache while we're modifying it.
    //

    //
    // Obtain the base pointer to the cluster cache for the given type of file
    // control block.
    //

    if (FatxIsFlagSet(Fcb->Flags, FAT_FCB_DIRECTORY)) {
        ClusterCache = Fcb->Directory.ClusterCache;
    } else {
        ClusterCache = Fcb->File.ClusterCache;
    }

    //
    // If this isn't the first allocation for the file, then check if we can
    // merge any existing cache entry with the first cluster run.
    //

    if (FileClusterNumber != 0) {

        ClusterCacheIndex = Fcb->ClusterCacheLruHead;

        do {

            ClusterCacheEntry = &ClusterCache[ClusterCacheIndex];

            //
            // If this is an empty cache entry, then break out now.  There
            // aren't any more entries to check.
            //

            if (ClusterCacheEntry->ClusterRunLength == 0) {
                break;
            }

            ASSERT(ClusterCacheEntry->FileClusterNumber < FileClusterNumber);

            //
            // Check if we've found a cache entry that has the logical clusters
            // immediately before clusters we're going to append to the cache.
            //

            if (ClusterCacheEntry->FileClusterNumber +
                ClusterCacheEntry->ClusterRunLength == FileClusterNumber) {

                //
                // Check if the cache entry is also physically contiguous with
                // the first cluster run.  If so, then combine the cache entry
                // with the first cluster run and move the cache entry to the
                // tail of the cache so that we reuse it immediately below.
                //

                if (ClusterCacheEntry->PhysicalClusterNumber +
                    ClusterCacheEntry->ClusterRunLength ==
                    ClusterRuns[0].PhysicalClusterNumber) {

                    FileClusterNumber = ClusterCacheEntry->FileClusterNumber;
                    ClusterRuns[0].PhysicalClusterNumber =
                        ClusterCacheEntry->PhysicalClusterNumber;
                    ClusterRuns[0].ClusterRunLength +=
                        ClusterCacheEntry->ClusterRunLength;

                    if (ClusterCacheIndex == Fcb->ClusterCacheLruHead) {
                        Fcb->ClusterCacheLruHead =
                            (UCHAR)ClusterCacheEntry->LruFlink;
                    } else {
                        FatxMoveClusterCacheEntryToTail(Fcb, ClusterCache,
                            ClusterCacheIndex);
                    }
                }

                break;
            }

            //
            // Advance to the next LRU index.
            //

            ClusterCacheIndex = (UCHAR)ClusterCacheEntry->LruFlink;

        } while (ClusterCacheIndex != Fcb->ClusterCacheLruHead);
    }

    //
    // In the below loop where we insert the cluster runs into the cluster
    // cache, we do it in reverse order, so we need to advance the file cluster
    // number to its ending number.  Loop through the cluster runs and advance
    // the number to its ending value.
    //

    CurrentClusterRun = ClusterRuns + NumberOfClusterRuns - 1;

    do {

        FileClusterNumber += CurrentClusterRun->ClusterRunLength;

        //
        // Advance to the next cluster run.
        //

        CurrentClusterRun--;

    } while (CurrentClusterRun >= ClusterRuns);

    //
    // Insert each cluster run from end to start into the cluster cache.
    //

    ClusterCacheEntry = &ClusterCache[Fcb->ClusterCacheLruHead];
    CurrentClusterRun = ClusterRuns + NumberOfClusterRuns - 1;

    do {

        ClusterCacheIndex = (UCHAR)ClusterCacheEntry->LruBlink;
        ClusterCacheEntry = &ClusterCache[ClusterCacheIndex];

        //
        // The file cluster number refers to the ending cluster from the end of
        // the cluster runs or the previous cluster run.  Adjust it to refer to
        // the start of this cluster run.
        //

        FileClusterNumber -= CurrentClusterRun->ClusterRunLength;

        //
        // Fill in the cache entry with the next cluster run.
        //

        ClusterCacheEntry->PhysicalClusterNumber =
            CurrentClusterRun->PhysicalClusterNumber;
        ClusterCacheEntry->FileClusterNumber = FileClusterNumber;
        ClusterCacheEntry->ClusterRunLength = CurrentClusterRun->ClusterRunLength;

        //
        // Advance to the next cluster run.
        //

        CurrentClusterRun--;

    } while (CurrentClusterRun >= ClusterRuns);

    //
    // Update the head index of the cluster cache to point at the last element
    // that we inserted.
    //

    Fcb->ClusterCacheLruHead = (UCHAR)ClusterCacheIndex;
}

VOID
FatxInvalidateClusterCache(
    IN PFAT_FCB FileFcb,
    IN ULONG FileClusterNumber
    )
/*++

Routine Description:

    This routine invalidates the entries in the cluster cache for the supplied
    file starting with the supplied cluster number.

Arguments:

    FileFcb - Specifies the file control block whose cluster cache should be
        emptied.

    FileClusterNumber - Specifies the file cluster number to start invalidating
        entries from.

Return Value:

    None.

--*/
{
    PFAT_FCB_CLUSTER_CACHE_ENTRY ClusterCache;
    ULONG StartingClusterCacheLruHead;
    ULONG NextClusterCacheIndex;
    ULONG ClusterCacheIndex;
    PFAT_FCB_CLUSTER_CACHE_ENTRY ClusterCacheEntry;

    ASSERT(FatxIsFlagClear(FileFcb->Flags, FAT_FCB_VOLUME));

    //
    // The cluster cache is invalidated only in paths when modifying the
    // allocation of an existing file.  As a result, we already have exclusive
    // access to the file's mutex.  That's enough to protect us against another
    // thread reading or writing to this file's cluster cache, so don't bother
    // acquiring the cluster cache.
    //

    ASSERT(FileFcb->File.FileMutexExclusiveOwner == KeGetCurrentThread());

    //
    // Search the cluster cache for entries after the supplied file cluster
    // number.
    //

    ClusterCache = FileFcb->File.ClusterCache;
    StartingClusterCacheLruHead = FileFcb->ClusterCacheLruHead;
    NextClusterCacheIndex = StartingClusterCacheLruHead;

    do {

        ClusterCacheIndex = NextClusterCacheIndex;
        ClusterCacheEntry = &ClusterCache[ClusterCacheIndex];
        NextClusterCacheIndex = (UCHAR)ClusterCacheEntry->LruFlink;

        //
        // If this is an empty cache entry, then break out now.  There aren't
        // any more entries to invalidate.
        //

        if (ClusterCacheEntry->ClusterRunLength == 0) {
            break;
        }

        //
        // Check if the cache entry contains the requested file cluster number.
        //

        if (FileClusterNumber <= ClusterCacheEntry->FileClusterNumber) {

            //
            // The cache entry starts at or is beyond the range to be
            // invalidated.  Invalidate this entry and push it to the end of the
            // LRU list.
            //
            // If the cache entry is at the head of the LRU list, then change
            // the LRU list head to point at the next element which will
            // automatically make this element the tail of the list.  Otherwise,
            // we'll need to move the entry to the tail of the list by updating
            // the LRU links.
            //

            ClusterCacheEntry->ClusterRunLength = 0;

            if (ClusterCacheIndex == FileFcb->ClusterCacheLruHead) {
                FileFcb->ClusterCacheLruHead = (UCHAR)ClusterCacheEntry->LruFlink;
            } else {
                FatxMoveClusterCacheEntryToTail(FileFcb, ClusterCache,
                    ClusterCacheIndex);
            }

        } else if (FileClusterNumber < ClusterCacheEntry->FileClusterNumber +
            ClusterCacheEntry->ClusterRunLength) {

            //
            // The cache entry contains part of the range to be invalidated.
            // Clip off the extra clusters from the entry.
            //

            ClusterCacheEntry->ClusterRunLength = FileClusterNumber -
                ClusterCacheEntry->FileClusterNumber;
        }

    } while (NextClusterCacheIndex != StartingClusterCacheLruHead);
}

NTSTATUS
FatxDeleteFileAllocation(
    IN PFAT_VOLUME_EXTENSION VolumeExtension,
    IN PIRP Irp,
    IN PFAT_FCB FileFcb
    )
/*++

Routine Description:

    This routine deletes the file allocation assigned to the supplied file.

Arguments:

    VolumeExtension - Specifies the extension that the I/O request is for.

    Irp - Specifies the packet that describes the I/O request.

    FileFcb - Specifies the file control block to delete the file allocation
        for.

Return Value:

    Status of operation.

--*/
{
    NTSTATUS status;
    ULONG OriginalFirstCluster;
    ULONG OriginalFileSize;

    ASSERT(FatxIsFlagClear(FileFcb->Flags, FAT_FCB_VOLUME | FAT_FCB_DIRECTORY));
    ASSERT(VolumeExtension->VolumeMutexExclusiveOwner == KeGetCurrentThread());
    ASSERT(FileFcb->File.FileMutexExclusiveOwner == KeGetCurrentThread());

    //
    // If the file doesn't have any allocation assigned to it and the file size
    // is already zero, then we can bail out immediately.
    //

    if ((FileFcb->FirstCluster == FAT_CLUSTER_NULL) &&
        (FileFcb->FileSize == 0)) {
        ASSERT(FileFcb->AllocationSize == 0);
        ASSERT(FileFcb->EndingCluster == FAT_CLUSTER_NULL);
        return STATUS_SUCCESS;
    }

    //
    // Save off the fields from the file control block that we're going to
    // change so that we can back out any changes if we fail below.
    //

    OriginalFirstCluster = FileFcb->FirstCluster;
    OriginalFileSize = FileFcb->FileSize;

    //
    // Reset the starting cluster and file size to zero.
    //

    FileFcb->FirstCluster = FAT_CLUSTER_NULL;
    FileFcb->FileSize = 0;

    //
    // Commit the directory change.  If this fails, then back out the changes to
    // the file control block so that we still mirror the on-disk structure.
    //

    status = FatxUpdateDirectoryEntry(VolumeExtension, Irp, FileFcb);

    if (!NT_SUCCESS(status)) {
        FileFcb->FirstCluster = OriginalFirstCluster;
        FileFcb->FileSize = OriginalFileSize;
        return status;
    }

    //
    // Invalidate the entire cluster cache for the file control block.
    //

    FatxInvalidateClusterCache(FileFcb, 0);

    //
    // Free the cluster chain starting with the first cluster.  FatxFreeClusters
    // will check that the cluster number is valid, so we don't have to do it
    // here.  We ignore any result from FatxFreeClusters.  If some error occurs
    // inside that routine, then we may have lost clusters on the disk, but
    // everything else on the disk and in memory is consistent.
    //

    FatxFreeClusters(VolumeExtension, Irp, OriginalFirstCluster, FALSE);

    //
    // Zero out the number of bytes allocated to this file and reset the ending
    // cluster number.
    //

    FileFcb->AllocationSize = 0;
    FileFcb->EndingCluster = FAT_CLUSTER_NULL;

    return STATUS_SUCCESS;
}

NTSTATUS
FatxTruncateFileAllocation(
    IN PFAT_VOLUME_EXTENSION VolumeExtension,
    IN PIRP Irp,
    IN PFAT_FCB FileFcb,
    IN ULONG AllocationSize
    )
/*++

Routine Description:

    This routine truncates the file allocation assigned to the supplied file to
    the supplied number of bytes.

Arguments:

    VolumeExtension - Specifies the extension that the I/O request is for.

    Irp - Specifies the packet that describes the I/O request.

    FileFcb - Specifies the file control block to truncate the file allocation
        for.

    AllocationSize - Specifies the new allocation size for the file, aligned to
        a cluster boundary.

Return Value:

    Status of operation.

--*/
{
    NTSTATUS status;
    ULONG OriginalFileSize;
    ULONG ClusterNumber;

    ASSERT(AllocationSize != 0);
    ASSERT(FatxByteOffsetIntoCluster(VolumeExtension, AllocationSize) == 0);
    ASSERT(FileFcb->AllocationSize != MAXULONG);
    ASSERT(FileFcb->AllocationSize > AllocationSize);
    ASSERT(FatxIsFlagClear(FileFcb->Flags, FAT_FCB_VOLUME | FAT_FCB_DIRECTORY));
    ASSERT(VolumeExtension->VolumeMutexExclusiveOwner == KeGetCurrentThread());
    ASSERT(FileFcb->File.FileMutexExclusiveOwner == KeGetCurrentThread());

    //
    // If the allocation size for the file is non-zero, then the starting and
    // ending cluster numbers must also be valid.
    //

    if (FileFcb->AllocationSize != 0) {
        ASSERT(FileFcb->FirstCluster != FAT_CLUSTER_NULL);
        ASSERT(FileFcb->EndingCluster != FAT_CLUSTER_NULL);
    }

    //
    // Find the cluster number of the byte before the desired allocation size.
    //

    status = FatxFileByteOffsetToCluster(VolumeExtension, Irp, FileFcb,
        AllocationSize - 1, &ClusterNumber, NULL);

    if (status == STATUS_END_OF_FILE) {
        status = STATUS_FILE_CORRUPT_ERROR;
    }

    if (!NT_SUCCESS(status)) {
        return status;
    }

    //
    // Check if the current file size is greater than the requested allocation
    // size.  If so, then we'll need to truncate the file size as well.
    //

    if (FileFcb->FileSize > AllocationSize) {

        //
        // Save off the fields from the file control block that we're going to
        // change so that we can back out any changes if we fail below.
        //

        OriginalFileSize = FileFcb->FileSize;

        //
        // Truncate the file size to the number of bytes that will be allocated
        // to the file.
        //

        FileFcb->FileSize = AllocationSize;

        //
        // Commit the directory change.  If this fails, then back out the changes
        // to the file control block so that we still mirror the on-disk
        // structure.
        //

        status = FatxUpdateDirectoryEntry(VolumeExtension, Irp, FileFcb);

        if (!NT_SUCCESS(status)) {
            FileFcb->FileSize = OriginalFileSize;
            return status;
        }
    }

    //
    // Free the cluster chain starting with the cluster number found above.
    // This first cluster number is marked as the last cluster in the file and
    // everything after this cluster is marked as free.
    //

    if (FatxFreeClusters(VolumeExtension, Irp, ClusterNumber, TRUE)) {

        //
        // FatxFreeClusterChain returns TRUE if it was able to modify the first
        // cluster in the chain.  We only modify the number of clusters
        // allocated if we were able to detach the unwanted clusters.  If
        // anything fails after the modification to the first cluster, then we
        // may have lost clusters on the disk, but everything else on disk and
        // in memory is consistent.
        //

        FileFcb->AllocationSize = AllocationSize;

        //
        // Change the ending cluster for the file to the cluster that we found
        // above.
        //

        FileFcb->EndingCluster = ClusterNumber;

        //
        // Invalidate any entries in the cluster cache for the file control
        // block after the new allocation size.
        //

        FatxInvalidateClusterCache(FileFcb, AllocationSize >>
            VolumeExtension->ClusterShift);

        status = STATUS_SUCCESS;

    } else {
        status = STATUS_UNSUCCESSFUL;
    }

    return status;
}

NTSTATUS
FatxExtendFileAllocation(
    IN PFAT_VOLUME_EXTENSION VolumeExtension,
    IN PIRP Irp,
    IN PFAT_FCB FileFcb,
    IN ULONG AllocationSize
    )
/*++

Routine Description:

    This routine extends the file allocation assigned to the supplied file to
    the supplied number of clusters.

Arguments:

    VolumeExtension - Specifies the extension that the I/O request is for.

    Irp - Specifies the packet that describes the I/O request.

    FileFcb - Specifies the file control block to extend the file allocation
        for.

    AllocationSize - Specifies the new allocation size for the file, aligned to
        a cluster boundary.

Return Value:

    Status of operation.

--*/
{
    NTSTATUS status;
    ULONG ClustersNeeded;
    FAT_CLUSTER_RUN ClusterRuns[FAT_MAXIMUM_CLUSTER_RUNS];
    ULONG NumberOfClusterRuns;
    ULONG EndingCluster;
    ULONG OldAllocationSize;

    ASSERT(AllocationSize != 0);
    ASSERT(FatxByteOffsetIntoCluster(VolumeExtension, AllocationSize) == 0);
    ASSERT(FileFcb->AllocationSize != MAXULONG);
    ASSERT(FileFcb->AllocationSize < AllocationSize);
    ASSERT(FatxIsFlagClear(FileFcb->Flags, FAT_FCB_VOLUME | FAT_FCB_DIRECTORY));
    ASSERT(VolumeExtension->VolumeMutexExclusiveOwner == KeGetCurrentThread());
    ASSERT(FileFcb->File.FileMutexExclusiveOwner == KeGetCurrentThread());

    //
    // If the allocation size for the file is non-zero, then the starting and
    // ending cluster numbers must also be valid.
    //

    if (FileFcb->AllocationSize != 0) {
        ASSERT(FileFcb->FirstCluster != FAT_CLUSTER_NULL);
        ASSERT(FileFcb->EndingCluster != FAT_CLUSTER_NULL);
    }

    //
    // Allocate the additional clusters we need to reach the desired allocation
    // size.
    //

    ClustersNeeded = (AllocationSize - FileFcb->AllocationSize) >>
        VolumeExtension->ClusterShift;

    status = FatxAllocateClusters(VolumeExtension, Irp, FileFcb->EndingCluster,
        ClustersNeeded, ClusterRuns, &NumberOfClusterRuns, &EndingCluster);

    if (NT_SUCCESS(status)) {

        //
        // The cluster allocation was successful.  If this is the first cluster
        // of the file, then update the directory entry with this initial
        // cluster.
        //

        if (FileFcb->FirstCluster == FAT_CLUSTER_NULL) {

            FileFcb->FirstCluster = ClusterRuns[0].PhysicalClusterNumber;

            status = FatxUpdateDirectoryEntry(VolumeExtension, Irp, FileFcb);

            if (!NT_SUCCESS(status)) {
                FatxFreeClusters(VolumeExtension, Irp, FileFcb->FirstCluster,
                    FALSE);
                FileFcb->FirstCluster = FAT_CLUSTER_NULL;
            }
        }

        //
        // Update the allocation size and the ending cluster number for the
        // file.  Copy the allocated cluster runs into the file's cluster cache.
        //

        if (NT_SUCCESS(status)) {

            OldAllocationSize = FileFcb->AllocationSize;

            FileFcb->AllocationSize = AllocationSize;
            FileFcb->EndingCluster = EndingCluster;

            FatxAppendClusterRunsToClusterCache(FileFcb,
                OldAllocationSize >> VolumeExtension->ClusterShift,
                ClusterRuns, NumberOfClusterRuns);
        }

    } else if (status != STATUS_DISK_FULL) {

        //
        // If FatxAllocateClusters fails due to any other error than a disk full
        // condition, then reset the file's allocation size and ending cluster
        // number, because we no longer know the correct values.  A disk write
        // error may have occurred after partially extending the file and
        // FatxAllocateClusters can't guarantee that we'll be back in a clean
        // state.
        //

        FileFcb->AllocationSize = MAXULONG;
        FileFcb->EndingCluster = FAT_CLUSTER_NULL;
    }

    return status;
}

NTSTATUS
FatxSetAllocationSize(
    IN PFAT_VOLUME_EXTENSION VolumeExtension,
    IN PIRP Irp,
    IN PFAT_FCB FileFcb,
    IN ULONG AllocationSize,
    IN BOOLEAN OverwritingFile,
    IN BOOLEAN DisableTruncation
    )
/*++

Routine Description:

    This routine changes the allocation size of the supplied file.

Arguments:

    VolumeExtension - Specifies the extension that the I/O request is for.

    Irp - Specifies the packet that describes the I/O request.

    FileFcb - Specifies the file control block to change the allocation size
        for.

    AllocationSize - Specifies the new allocation size for the file.

    OverwritingFile - Specifies whether or not we're allowed to overwrite the
        current file allocation.

    DisableTruncation - If TRUE, if the requested allocation size is smaller
        than the current allocation size, then don't truncate the file
        allocation.

Return Value:

    Status of operation.

--*/
{
    NTSTATUS status;
    ULONG ClusterNumber;

    ASSERT(FatxIsFlagClear(FileFcb->Flags, FAT_FCB_VOLUME | FAT_FCB_DIRECTORY));
    ASSERT(VolumeExtension->VolumeMutexExclusiveOwner == KeGetCurrentThread());
    ASSERT(FileFcb->File.FileMutexExclusiveOwner == KeGetCurrentThread());

    AllocationSize = FatxRoundToClusters(VolumeExtension, AllocationSize);

    //
    // If the requested allocation size is zero, then delete the entire
    // allocation assigned to the file.
    //

    if (AllocationSize == 0) {
        return FatxDeleteFileAllocation(VolumeExtension, Irp, FileFcb);
    }

    //
    // Determine the number of bytes currently allocated to the file and find
    // the ending cluster number.  If that can't be determined because the file
    // is corrupt and we're allowed to overwrite the file, then delete the
    // current file allocation and start over.
    //

    if (FileFcb->AllocationSize == MAXULONG) {

        ASSERT(FileFcb->EndingCluster == FAT_CLUSTER_NULL);

        //
        // Attempt to find the cluster corresponding to the maximum byte offset
        // which will have the side effect of filling in the number of bytes
        // allocated.
        //

        status = FatxFileByteOffsetToCluster(VolumeExtension, Irp, FileFcb,
            MAXULONG, &ClusterNumber, NULL);

        if (status == STATUS_END_OF_FILE) {
            status = STATUS_SUCCESS;
        } else if (status == STATUS_FILE_CORRUPT_ERROR && OverwritingFile) {
            status = FatxDeleteFileAllocation(VolumeExtension, Irp, FileFcb);
        }

        if (!NT_SUCCESS(status)) {
            return status;
        }
    }

    //
    // Compute the number of clusters that should be allocated to the file and
    // dispatch the request as appropriate.
    //

    if (FileFcb->AllocationSize < AllocationSize) {
        status = FatxExtendFileAllocation(VolumeExtension, Irp, FileFcb,
            AllocationSize);
    } else if (!DisableTruncation && (FileFcb->AllocationSize > AllocationSize)) {
        status = FatxTruncateFileAllocation(VolumeExtension, Irp, FileFcb,
            AllocationSize);
    } else {
        status = STATUS_SUCCESS;
    }

    return status;
}

NTSTATUS
FatxExtendDirectoryAllocation(
    IN PFAT_VOLUME_EXTENSION VolumeExtension,
    IN PIRP Irp,
    IN PFAT_FCB DirectoryFcb
    )
/*++

Routine Description:

    This routine extends the file allocation assigned to the supplied directory
    by one cluster.

Arguments:

    VolumeExtension - Specifies the extension that the I/O request is for.

    Irp - Specifies the packet that describes the I/O request.

    DirectoryFcb - Specifies the file control block to extend the file
        allocation for.

Return Value:

    Status of operation.

--*/
{
    NTSTATUS status;
    FAT_CLUSTER_RUN ClusterRuns[FAT_MAXIMUM_CLUSTER_RUNS];
    ULONG NumberOfClusterRuns;
    ULONG EndingCluster;
    ULONG OldAllocationSize;

    ASSERT(FatxIsFlagSet(DirectoryFcb->Flags, FAT_FCB_DIRECTORY));
    ASSERT(VolumeExtension->VolumeMutexExclusiveOwner == KeGetCurrentThread());

    //
    // This routine is only called for directories that already exist and only
    // after the entire directory stream has been processed, so we know that the
    // allocation size is non-zero and known.
    //

    ASSERT(DirectoryFcb->AllocationSize != 0);
    ASSERT(DirectoryFcb->AllocationSize != MAXULONG);
    ASSERT(DirectoryFcb->EndingCluster != FAT_CLUSTER_NULL);

    //
    // Don't allow a directory to exceed the directory file size restrictions.
    //

    if (DirectoryFcb->AllocationSize >= FAT_MAXIMUM_DIRECTORY_FILE_SIZE) {
        return STATUS_CANNOT_MAKE;
    }

    //
    // Allocate a single cluster.  We don't specify the ending cluster number
    // because we don't want the cluster attached to the directory's allocation
    // chain yet and it's going to be unlikely that we can get a cluster that's
    // contiguous to the ending cluster anyway.
    //

    status = FatxAllocateClusters(VolumeExtension, Irp, FAT_CLUSTER_NULL, 1,
        ClusterRuns, &NumberOfClusterRuns, &EndingCluster);

    if (NT_SUCCESS(status)) {

        ASSERT(NumberOfClusterRuns == 1);
        ASSERT(EndingCluster == ClusterRuns[0].PhysicalClusterNumber);

        //
        // Initialize the contents of the directory cluster.
        //

        status = FatxInitializeDirectoryCluster(VolumeExtension, Irp, EndingCluster);

        if (NT_SUCCESS(status)) {

            //
            // Link the new cluster to the ending cluster.
            //
            // Note that FatxLinkClusterChains calls FatxFreeClusters for the
            // starting cluster number when the operation fails.
            //

            status = FatxLinkClusterChains(VolumeExtension, Irp,
                DirectoryFcb->EndingCluster, EndingCluster);

            if (NT_SUCCESS(status)) {

                //
                // The directory is now one cluster bigger and the ending
                // cluster is advanced to the new cluster.
                //

                OldAllocationSize = DirectoryFcb->AllocationSize;

                DirectoryFcb->AllocationSize += VolumeExtension->BytesPerCluster;
                DirectoryFcb->EndingCluster = EndingCluster;

                FatxAppendClusterRunsToClusterCache(DirectoryFcb,
                    OldAllocationSize >> VolumeExtension->ClusterShift,
                    ClusterRuns, NumberOfClusterRuns);
            }

        } else {
            FatxFreeClusters(VolumeExtension, Irp, EndingCluster, FALSE);
        }
    }

    return status;
}

NTSTATUS
FatxIsDirectoryEmpty(
    IN PFAT_VOLUME_EXTENSION VolumeExtension,
    IN PIRP Irp,
    IN PFAT_FCB DirectoryFcb
    )
/*++

Routine Description:

    This routine tests if the supplied directory is empty.

Arguments:

    VolumeExtension - Specifies the extension that the I/O request is for.

    Irp - Specifies the packet that describes the I/O request.

    DirectoryFcb - Specifies the file control block of the directory to test.

Return Value:

    Returns STATUS_SUCCESS if the directory is empty, STATUS_DIRECTORY_NOT_EMPTY
    if the directory is not empty, else other error codes while attempting to
    make the test.

--*/
{
    NTSTATUS status;
    DIRENT DirectoryEntry;
    ULONG DirectoryByteOffset;
    OBJECT_STRING TemplateFileName;

    ASSERT(FatxIsFlagSet(DirectoryFcb->Flags, FAT_FCB_DIRECTORY));
    ASSERT(VolumeExtension->VolumeMutexExclusiveOwner == KeGetCurrentThread());

    //
    // Attempt to enumerate any file from the supplied directory.  If we get
    // back a file, then the directory is not empty.  If we reach the end of the
    // directory stream, then the directory is empty.  Any other status is an
    // error that we push back to the caller.
    //

    TemplateFileName.Buffer = NULL;

    status = FatxFindNextDirectoryEntry(VolumeExtension, Irp, DirectoryFcb,
        0, &TemplateFileName, &DirectoryEntry, &DirectoryByteOffset);

    if (NT_SUCCESS(status)) {
        status = STATUS_DIRECTORY_NOT_EMPTY;
    } else if (status == STATUS_END_OF_FILE) {
        status = STATUS_SUCCESS;
    }

    return status;
}

NTSTATUS
FatxDeleteFile(
    IN PFAT_VOLUME_EXTENSION VolumeExtension,
    IN PIRP Irp,
    IN PFAT_FCB DirectoryFcb,
    IN ULONG DirectoryByteOffset
    )
/*++

Routine Description:

    This routine deletes the file at the byte offset inside the supplied
    directory.

Arguments:

    VolumeExtension - Specifies the extension that the I/O request is for.

    Irp - Specifies the packet that describes the I/O request.

    DirectoryFcb - Specifies the file control block of the directory that
        contains the file to be deleted.

    DirectoryByteOffset - Specifies the byte offset into the directory for the
        file to delete.

Return Value:

    Status of operation.

--*/
{
    NTSTATUS status;
    ULONGLONG PhysicalByteOffset;
    ULONG PhysicalRunLength;
    PVOID CacheBuffer;
    PDIRENT DirectoryEntry;
    ULONG FirstCluster;

    ASSERT(FatxIsFlagSet(DirectoryFcb->Flags, FAT_FCB_DIRECTORY));
    ASSERT(VolumeExtension->VolumeMutexExclusiveOwner == KeGetCurrentThread());

    //
    // Get the physical byte offset corresponding to the file's directory byte
    // offset.
    //

    status = FatxFileByteOffsetToPhysicalByteOffset(VolumeExtension, Irp,
        DirectoryFcb, DirectoryByteOffset, FALSE, &PhysicalByteOffset,
        &PhysicalRunLength);

    if (!NT_SUCCESS(status)) {
        return status;
    }

    //
    // Map the directory entry into the cache.
    //

    status = FscMapBuffer(&VolumeExtension->CacheExtension, Irp,
        PhysicalByteOffset, TRUE, &CacheBuffer);

    if (!NT_SUCCESS(status)) {
        return status;
    }

    //
    // Grab the starting cluster number from the directory entry and mark the
    // entry as deleted.
    //

    DirectoryEntry = (PDIRENT)CacheBuffer;

    FirstCluster = DirectoryEntry->FirstCluster;

    DirectoryEntry->FileNameLength = FAT_DIRENT_DELETED;

    //
    // Write the change back out the disk and unmap the cache buffer.
    //

    status = FscWriteBuffer(&VolumeExtension->CacheExtension, Irp,
        PhysicalByteOffset, sizeof(DIRENT), CacheBuffer);

    if (!NT_SUCCESS(status)) {
        return status;
    }

    //
    // Free the clusters for the file.  Note that we don't have to check if the
    // first cluster number is valid because FatxFreeClusters does that for us.
    // We also don't care if FatxFreeClusters fails, because as far as the
    // caller is concerned, the file is deleted.
    //

    FatxFreeClusters(VolumeExtension, Irp, FirstCluster, FALSE);

    return STATUS_SUCCESS;
}

VOID
FatxDereferenceFcb(
    IN PFAT_FCB Fcb
    )
/*++

Routine Description:

    This routine decrements the reference count on the supplied file control
    block.  If the reference count reaches zero, then the file control block is
    deleted.

Arguments:

    Fcb - Specifies the file control block to dereference.

Return Value:

    None.

--*/
{
    PFAT_FCB ParentFcb;

    ASSERT(Fcb->ReferenceCount > 0);

    do {

        //
        // Decrement the reference count and bail out if there are still
        // outstanding references to the file control block.
        //

        if (--Fcb->ReferenceCount != 0) {
            return;
        }

        //
        // The volume's file control block is statically allocated as part of
        // the volume's device extension.  The block is initialized with a
        // reference count of zero at volume mount and should never transition
        // from a reference count of one to zero, so we should never get past
        // the above line for a volume file control block.
        //

        ASSERT(FatxIsFlagClear(Fcb->Flags, FAT_FCB_VOLUME));

        //
        // Verify that the child file control block list is empty if this is a
        // directory.
        //

        if (FatxIsFlagSet(Fcb->Flags, FAT_FCB_DIRECTORY)) {
            ASSERT(IsListEmpty(&Fcb->Directory.ChildFcbList));
        }

        //
        // Save off the parent file control block so that we can dereference it
        // in a bit.
        //

        ParentFcb = Fcb->ParentFcb;

        //
        // Remove this file control block from the list of siblings.
        //

        if (ParentFcb != NULL) {
            RemoveEntryList(&Fcb->SiblingFcbLink);
        }

        //
        // If the file name buffer was replaced by another pool allocated
        // buffer, then free the buffer.
        //

        if (FatxIsFlagSet(Fcb->Flags, FAT_FCB_FREE_FILE_NAME_BUFFER)) {
            ExFreePool(Fcb->FileNameBuffer);
        }

        //
        // Free the file control block.
        //

        ExFreePool(Fcb);

        //
        // Switch to the parent file control block and restart the loop to
        // dereference this object.
        //

        Fcb = ParentFcb;

    } while (Fcb != NULL);
}

#if DBG

VOID
FatxDbgPrintClusterCache(
    IN PFAT_FCB Fcb
    )
/*++

Routine Description:

    This routine dumps out the contents of the supplied file control block's
    cluster cache.

Arguments:

    Fcb - Specifies the file control block to display the cluster cache for.

Return Value:

    None.

--*/
{
    PFAT_FCB_CLUSTER_CACHE_ENTRY ClusterCache;
    UCHAR ClusterCacheIndex;
    PFAT_FCB_CLUSTER_CACHE_ENTRY ClusterCacheEntry;

    ASSERT(FatxIsFlagClear(Fcb->Flags, FAT_FCB_VOLUME));

    FatxDbgPrint(("FATX: Dumping cluster cache for %s %p:\n",
        FatxIsFlagSet(Fcb->Flags, FAT_FCB_DIRECTORY) ? "directory" : "file", Fcb));

    if (FatxIsFlagSet(Fcb->Flags, FAT_FCB_DIRECTORY)) {
        ClusterCache = Fcb->Directory.ClusterCache;
    } else {
        ClusterCache = Fcb->File.ClusterCache;
    }

    ClusterCacheIndex = Fcb->ClusterCacheLruHead;

    do {

        ClusterCacheEntry = &ClusterCache[ClusterCacheIndex];

        FatxDbgPrint(("%02d: F %02d B %02d - P %08x F %08x L %08x\n",
            ClusterCacheIndex,
            ClusterCacheEntry->LruFlink, ClusterCacheEntry->LruBlink,
            ClusterCacheEntry->PhysicalClusterNumber,
            ClusterCacheEntry->FileClusterNumber,
            ClusterCacheEntry->ClusterRunLength));

        ClusterCacheIndex = (UCHAR)ClusterCacheEntry->LruFlink;

    } while (ClusterCacheIndex != Fcb->ClusterCacheLruHead);
}

#endif
