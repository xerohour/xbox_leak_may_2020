/*++

Copyright (c) 2000-2002  Microsoft Corporation

Module Name:

    format.c

Abstract:

    This module implements routines to format a FAT volume.

--*/

#include "basedll.h"
#include "..\..\fatx\fat.h"

BOOL
WINAPI
XapiFormatFATVolume(
    IN POBJECT_STRING VolumePath
    )
{
    return XapiFormatFATVolumeEx(VolumePath, 16384);
}

BOOL
WINAPI
XapiFormatFATVolumeEx(
    IN POBJECT_STRING VolumePath,
    IN ULONG BytesPerCluster
    )
{
    NTSTATUS status;
    OBJECT_ATTRIBUTES ObjectAttributes;
    IO_STATUS_BLOCK IoStatusBlock;
    HANDLE VolumeHandle;
    DISK_GEOMETRY DiskGeometry;
    ULONG SectorShift;
    PARTITION_INFORMATION PartitionInformation;
    ULONG StructureAlignment;
    ULONG NumberOfReservedBytes;
    ULONG NumberOfClustersLimit;
    ULONG NumberOfBytesPerFat;
    BOOLEAN Fat16X;
    ULONGLONG NumberOfUnreservedBytes;
    ULONGLONG NumberOfFileAreaBytes;
    PUCHAR Buffer;
    PFAT_VOLUME_METADATA VolumeMetadata;
    LARGE_INTEGER SystemTime;
    LARGE_INTEGER ByteOffset;
    ULONG BytesRemaining;

    //
    // Open the volume for read/write access.
    //

    InitializeObjectAttributes(&ObjectAttributes, VolumePath,
        OBJ_CASE_INSENSITIVE, NULL, NULL);

    status = NtOpenFile(&VolumeHandle, SYNCHRONIZE | FILE_READ_DATA |
        FILE_WRITE_DATA, &ObjectAttributes, &IoStatusBlock, 0,
        FILE_SYNCHRONOUS_IO_ALERT | FILE_NO_INTERMEDIATE_BUFFERING);

    if (!NT_SUCCESS(status)) {
        XapiSetLastNTError(status);
        return FALSE;
    }

    //
    // Obtain the drive geometry for the target device.
    //

    status = NtDeviceIoControlFile(VolumeHandle, NULL, NULL, NULL, &IoStatusBlock,
        IOCTL_DISK_GET_DRIVE_GEOMETRY, NULL, 0, &DiskGeometry,
        sizeof(DiskGeometry));

    if (!NT_SUCCESS(status)) {
        NtClose(VolumeHandle);
        XapiSetLastNTError(status);
        return FALSE;
    }

    SectorShift = RtlFindFirstSetRightMember(DiskGeometry.BytesPerSector);

    //
    // Obtain the size of the partition for the target device.
    //

    status = NtDeviceIoControlFile(VolumeHandle, NULL, NULL, NULL, &IoStatusBlock,
        IOCTL_DISK_GET_PARTITION_INFO, NULL, 0, &PartitionInformation,
        sizeof(PartitionInformation));

    if (!NT_SUCCESS(status)) {
        NtClose(VolumeHandle);
        XapiSetLastNTError(status);
        return FALSE;
    }

    //
    // Compute the alignment between the various file system structures.
    // Everything should be page aligned in order to maximum file system cache
    // efficiency.
    //

    StructureAlignment = ROUND_TO_PAGES(DiskGeometry.BytesPerSector);

    if (BytesPerCluster < StructureAlignment) {
        BytesPerCluster = StructureAlignment;
    }

    //
    // Compute the number of reserved bytes for the volume.
    //
    // We only need to reserve enough space for the volume metadata block.
    //

    NumberOfReservedBytes = PAGE_SIZE;

    if (NumberOfReservedBytes < StructureAlignment) {
        NumberOfReservedBytes = StructureAlignment;
    }

    //
    // Verify that the partition has enough space to contain the reserved bytes.
    //

    if ((ULONGLONG)NumberOfReservedBytes >=
        (ULONGLONG)PartitionInformation.PartitionLength.QuadPart) {
        NtClose(VolumeHandle);
        SetLastError(ERROR_DISK_FULL);
        return FALSE;
    }

    //
    // Compute the limit of the number of clusters given the length of the
    // partition (bump up the count by two to account for the two reserved
    // entries in the FAT).  With this limit, decide whether or not we'll
    // use 16-bit or 32-bit FAT entries.
    //
    // Near the limit of the number of bytes per cluster multiplied by
    // FAT_CLUSTER16_AVAILABLE, we could end up using a bigger FAT than we
    // really need, but the math is very simple to understand.
    //

    NumberOfClustersLimit =
        (ULONG)(PartitionInformation.PartitionLength.QuadPart /
        BytesPerCluster) + FAT_RESERVED_FAT_ENTRIES;

    if (NumberOfClustersLimit < FAT_CLUSTER16_RESERVED) {
        NumberOfBytesPerFat = NumberOfClustersLimit * sizeof(USHORT);
        Fat16X = TRUE;
    } else {
        NumberOfBytesPerFat = NumberOfClustersLimit * sizeof(ULONG);
        Fat16X = FALSE;
    }

    NumberOfBytesPerFat = (NumberOfBytesPerFat + (StructureAlignment - 1)) &
        ~(StructureAlignment - 1);

    //
    // Compute the number of bytes that haven't been reserved above.
    //

    NumberOfUnreservedBytes =
        (ULONGLONG)PartitionInformation.PartitionLength.QuadPart -
        NumberOfReservedBytes;

    //
    // Verify that the partition has enough space to contain the FAT.
    //

    if ((ULONGLONG)NumberOfBytesPerFat > NumberOfUnreservedBytes) {
        NtClose(VolumeHandle);
        SetLastError(ERROR_DISK_FULL);
        return FALSE;
    }

    //
    // Verify that the partition has enough space for a single cluster to hold
    // the root directory.
    //

    NumberOfFileAreaBytes = NumberOfUnreservedBytes - NumberOfBytesPerFat;

    if (BytesPerCluster > NumberOfFileAreaBytes) {
        NtClose(VolumeHandle);
        SetLastError(ERROR_DISK_FULL);
        return FALSE;
    }

    //
    // Allocate a buffer to hold the sectors we're writing to the disk.
    //

    Buffer = (PUCHAR) LocalAlloc(LMEM_FIXED, StructureAlignment);

    if (Buffer == NULL) {
        status = STATUS_INSUFFICIENT_RESOURCES;
        NtClose(VolumeHandle);
        XapiSetLastNTError(status);
        return FALSE;
    }

    //
    // Prepare the reserved sectors.
    //

    RtlFillMemory(Buffer, StructureAlignment, 0xFF);

    VolumeMetadata = (PFAT_VOLUME_METADATA)Buffer;

    VolumeMetadata->Signature = FAT_VOLUME_SIGNATURE;
    VolumeMetadata->SectorsPerCluster = (UCHAR)(BytesPerCluster >> SectorShift);
    VolumeMetadata->RootDirFirstCluster = 1;
    VolumeMetadata->VolumeName[0] = L'\0';

    //
    // Use the system time for the serial number.
    //

    KeQuerySystemTime(&SystemTime);
    VolumeMetadata->SerialNumber = SystemTime.LowPart;

    //
    // After this point, any failures leaves the volume in an intermediate
    // state.  Jump to DismountAndExit to force a dismount of the volume so that
    // we don't leave a file system device object in a random state.
    //

    //
    // Write out the reserved sectors.
    //

    ByteOffset.QuadPart = 0;
    BytesRemaining = NumberOfReservedBytes;

    do {

        status = NtWriteFile(VolumeHandle, NULL, NULL, NULL, &IoStatusBlock,
            Buffer, StructureAlignment, &ByteOffset);

        if (!NT_SUCCESS(status)) {
            LocalFree(Buffer);
            goto DismountAndExit;
        }

        //
        // Wipe out the reserved bytes for the second and greater passes of this
        // loop.
        //

        RtlZeroMemory(Buffer, StructureAlignment);

        //
        // Advance to the next reserved bytes page.
        //

        ByteOffset.QuadPart += StructureAlignment;
        BytesRemaining -= StructureAlignment;

    } while (BytesRemaining > 0);

    //
    // Prepare and write out the initial FAT sectors.  For the first page of FAT
    // sectors, mark the root directory's cluster as allocated and the end of
    // the file allocation table.
    //

    RtlZeroMemory(Buffer, StructureAlignment);

    ByteOffset.QuadPart = NumberOfReservedBytes;
    BytesRemaining = NumberOfBytesPerFat;

    if (Fat16X) {
        ((PUSHORT)Buffer)[0] = FAT_CLUSTER16_MEDIA;
        ((PUSHORT)Buffer)[1] = FAT_CLUSTER16_LAST;
    } else {
        ((PULONG)Buffer)[0] = FAT_CLUSTER_MEDIA;
        ((PULONG)Buffer)[1] = FAT_CLUSTER_LAST;
    }

    do {

        status = NtWriteFile(VolumeHandle, NULL, NULL, NULL, &IoStatusBlock,
            Buffer, StructureAlignment, &ByteOffset);

        if (!NT_SUCCESS(status)) {
            LocalFree(Buffer);
            goto DismountAndExit;
        }

        //
        // Wipe out the cluster values that we filled out above.
        //

        RtlZeroMemory(Buffer, sizeof(ULONG) * (FAT_RESERVED_FAT_ENTRIES + 1));

        //
        // Advance to the next FAT page.
        //

        ByteOffset.QuadPart += StructureAlignment;
        BytesRemaining -= StructureAlignment;

    } while (BytesRemaining > 0);

    //
    // Write out an empty cluster for the directory.  The byte offset is already
    // set to point at the first allocatable cluster.
    //

    RtlFillMemory(Buffer, StructureAlignment, FAT_DIRENT_NEVER_USED2);

    BytesRemaining = BytesPerCluster;

    do {

        status = NtWriteFile(VolumeHandle, NULL, NULL, NULL, &IoStatusBlock,
            Buffer, StructureAlignment, &ByteOffset);

        if (!NT_SUCCESS(status)) {
            LocalFree(Buffer);
            goto DismountAndExit;
        }

        ByteOffset.QuadPart += StructureAlignment;
        BytesRemaining -= StructureAlignment;

    } while (BytesRemaining > 0);

    LocalFree(Buffer);

    //
    // Dismount the file system so that we get a new file system the next time
    // somebody touches this volume.
    //

DismountAndExit:
    NtFsControlFile(VolumeHandle, NULL, NULL, NULL, &IoStatusBlock,
        FSCTL_DISMOUNT_VOLUME, NULL, 0, NULL, 0);

    NtClose(VolumeHandle);

    if (NT_SUCCESS(status)) {
        return TRUE;
    } else {
        XapiSetLastNTError(status);
        return FALSE;
    }
}
