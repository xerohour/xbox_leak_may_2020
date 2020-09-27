/*++

Copyright (c) 2000-2001  Microsoft Corporation

Module Name:

    mountvol.c

Abstract:

    This module implements routines related to the mounting of a FAT volume.

--*/

#include "fatx.h"

NTSTATUS
FatxCreateVolumeDevice(
    IN PDEVICE_OBJECT TargetDeviceObject,
    IN PDISK_GEOMETRY DiskGeometry,
    IN PPARTITION_INFORMATION PartitionInformation,
    OUT PDEVICE_OBJECT *ReturnedVolumeDeviceObject
    )
/*++

Routine Description:

    This routine creates an instance of a volume device object.

Arguments:

    TargetDeviceObject - Specifies the device object that the volume object
        sends I/O requests to.

    DiskGeometry - Specifies the disk geometry of the target device.

    PartitionInformation - Specifies the partition information for the target
        device.

    ReturnedVolumeDeviceObject - Specifies the buffer to receive the created
        device object.

Return Value:

    Status of operation.

--*/
{
    NTSTATUS status;
    PDEVICE_OBJECT VolumeDeviceObject;
    PFAT_VOLUME_EXTENSION VolumeExtension;

    //
    // Create the volume's device object.
    //

    status = IoCreateDevice(&FatxDriverObject, sizeof(FAT_VOLUME_EXTENSION),
        NULL, FILE_DEVICE_DISK_FILE_SYSTEM, FALSE, &VolumeDeviceObject);

    if (!NT_SUCCESS(status)) {
        return status;
    }

    //
    // Increase the volume's IRP stack size for requests that we pass down to
    // the target device object.
    //

    VolumeDeviceObject->StackSize = (UCHAR)(VolumeDeviceObject->StackSize +
        TargetDeviceObject->StackSize);

    //
    // Ensure that the alignment for this device matches the requirements of the
    // target device.
    //

    if (VolumeDeviceObject->AlignmentRequirement < TargetDeviceObject->AlignmentRequirement) {
        VolumeDeviceObject->AlignmentRequirement = TargetDeviceObject->AlignmentRequirement;
    }

    //
    // If the target device object supports scatter gather I/O, then indicate
    // that we can as well.
    //

    if (FatxIsFlagSet(TargetDeviceObject->Flags, DO_SCATTER_GATHER_IO)) {
        VolumeDeviceObject->Flags |= DO_SCATTER_GATHER_IO;
    }

    //
    // Set the size of a sector for this device.
    //

    VolumeDeviceObject->SectorSize = (USHORT)DiskGeometry->BytesPerSector;

    //
    // Increment the reference count on the target device object since we'll be
    // holding a pointer to it.
    //

    ObReferenceObject(TargetDeviceObject);

    //
    // Initialize the volume's device extension data.
    //

    VolumeExtension = (PFAT_VOLUME_EXTENSION)VolumeDeviceObject->DeviceExtension;

    VolumeExtension->TargetDeviceObject = TargetDeviceObject;
    VolumeExtension->PartitionLength = PartitionInformation->PartitionLength;
    VolumeExtension->SectorSize = DiskGeometry->BytesPerSector;
    VolumeExtension->SectorShift = RtlFindFirstSetRightMember(DiskGeometry->BytesPerSector);

    //
    // Initialize the volume's instance lock.
    //

    ExInitializeReadWriteLock(&VolumeExtension->VolumeMutex);

    //
    // Initialize the volume's cluster cache lock.
    //

    RtlInitializeCriticalSection(&VolumeExtension->ClusterCacheMutex);

    //
    // Initialize the file control block for the physical volume.
    //

    VolumeExtension->VolumeFcb.Flags = FAT_FCB_VOLUME;
    VolumeExtension->VolumeFcb.ReferenceCount = 1;

    //
    // The device has finished initializing and is ready to accept requests.
    //

    VolumeDeviceObject->Flags &= ~DO_DEVICE_INITIALIZING;

    //
    // Return the device object to the caller.
    //

    *ReturnedVolumeDeviceObject = VolumeDeviceObject;

    return STATUS_SUCCESS;
}

VOID
FatxDeleteVolumeDevice(
    IN PDEVICE_OBJECT VolumeDeviceObject
    )
/*++

Routine Description:

    This routine deletes an instance of a volume device object.

Arguments:

    VolumeDeviceObject - Specifies the volume device object to delete.

Return Value:

    Status of operation.

--*/
{
    PFAT_VOLUME_EXTENSION VolumeExtension;

    VolumeExtension = (PFAT_VOLUME_EXTENSION)VolumeDeviceObject->DeviceExtension;

    //
    // There shouldn't be any open files for this volume.
    //

    ASSERT(VolumeExtension->FileObjectCount == 0);

    //
    // Delete the volume's cluster cache lock.
    //

    RtlDeleteCriticalSection(&VolumeExtension->ClusterCacheMutex);

    //
    // Delete any file control blocks created by the volume.
    //

    if (VolumeExtension->RootDirectoryFcb != NULL) {
        FatxDereferenceFcb(VolumeExtension->RootDirectoryFcb);
    }

    //
    // Invalidate any file system cache buffers for this device.
    //

    FscInvalidateDevice(&VolumeExtension->CacheExtension);

    //
    // Release the reference on the target device object.
    //

    if (VolumeExtension->TargetDeviceObject != NULL) {
        ObDereferenceObject(VolumeExtension->TargetDeviceObject);
    }

    //
    // Delete the device object.
    //

    IoDeleteDevice(VolumeDeviceObject);
}

NTSTATUS
FatxProcessBootSector(
    IN PDEVICE_OBJECT VolumeDeviceObject,
    IN PIRP Irp,
    IN PLARGE_INTEGER PartitionLength
    )
/*++

Routine Description:

    This routine reads the boot sector for the supplied volume device object,
    validates its structure, and caches any interesting information in the
    device extension.

Arguments:

    VolumeDeviceObject - Specifies the volume device object to mount.

    Irp - Specifies the packet that describes the I/O request.

    PartitionLength - Specifies the length of the partition in bytes.

Return Value:

    Status of operation.

--*/
{
    NTSTATUS status;
    PFAT_VOLUME_EXTENSION VolumeExtension;
    PVOID CacheBuffer;
    ULONG StructureAlignment;
    ULONG NumberOfReservedBytes;
    PFAT_VOLUME_METADATA VolumeMetadata;
    ULONG RootDirectoryFirstCluster;
    ULONG NumberOfClustersLimit;
    ULONG NumberOfBytesPerFat;
    ULONGLONG NumberOfUnreservedBytes;
    ULONGLONG NumberOfFileAreaBytes;

    VolumeExtension = (PFAT_VOLUME_EXTENSION)VolumeDeviceObject->DeviceExtension;
    CacheBuffer = NULL;

    //
    // Compute the alignment between the various file system structures.
    // Everything should be page aligned in order to maximum file system cache
    // efficiency.
    //

    StructureAlignment = ROUND_TO_PAGES(VolumeExtension->SectorSize);

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

    if ((ULONGLONG)NumberOfReservedBytes >= (ULONGLONG)PartitionLength->QuadPart) {
        status = STATUS_UNRECOGNIZED_VOLUME;
        goto CleanupAndExit;
    }

    //
    // Map the volume metadata block into the cache.
    //

    status = FscMapBuffer(&VolumeExtension->CacheExtension, Irp, 0, FALSE,
        &CacheBuffer);

    if (!NT_SUCCESS(status)) {
        status = STATUS_UNRECOGNIZED_VOLUME;
        goto CleanupAndExit;
    }

    VolumeMetadata = (PFAT_VOLUME_METADATA)CacheBuffer;

    //
    // Verify that the signature is valid.
    //

    if (VolumeMetadata->Signature != FAT_VOLUME_SIGNATURE) {
        FatxDbgPrint(("FATX: found unrecognized signature\n"));
        status = STATUS_UNRECOGNIZED_VOLUME;
        goto CleanupAndExit;
    }

    //
    // Verify that the sectors per cluster looks reasonable.
    //

    switch (VolumeMetadata->SectorsPerCluster) {

        case 1:
        case 2:
        case 4:
        case 8:
        case 16:
        case 32:
        case 64:
        case 128:
            break;

        default:
            FatxDbgPrint(("FATX: found invalid sectors per cluster\n"));
            status = STATUS_UNRECOGNIZED_VOLUME;
            goto CleanupAndExit;
    }

    VolumeExtension->BytesPerCluster = VolumeMetadata->SectorsPerCluster <<
        VolumeExtension->SectorShift;
    VolumeExtension->ClusterShift =
        RtlFindFirstSetRightMember(VolumeExtension->BytesPerCluster);

    //
    // Copy the serial number from to the volume extension.
    //

    VolumeExtension->SerialNumber = VolumeMetadata->SerialNumber;

    //
    // Remember the root directory cluster and unmap the volume metadata block.
    //

    RootDirectoryFirstCluster = VolumeMetadata->RootDirFirstCluster;

    FscUnmapBuffer(CacheBuffer);
    CacheBuffer = NULL;

    //
    // Verify that the number of bytes per cluster is greater than or equal to
    // the structure alignment requirement.
    //

    if (VolumeExtension->BytesPerCluster < StructureAlignment) {
        FatxDbgPrint(("FATX: found too small of cluster size\n"));
        status = STATUS_UNRECOGNIZED_VOLUME;
        goto CleanupAndExit;
    }

    //
    // For volumes with more than 64K of clusters, we use 32-bit file allocation
    // table entries.  Otherwise, we use 16-bit file allocation table entries.
    // This is not FAT16 as defined by DOS; we still have a FAT32 style BIOS
    // parameter block, but the FAT is in a "compressed" format.
    //
    // Note that we base the test on the limit of the number of clusters for the
    // entire partition.  The same test is performed in the FAT format code.
    //
    // The below number isn't exactly 64K, because we need to avoid the magic
    // cluster numbers that mark free or end of file clusters.
    //

    NumberOfClustersLimit = (ULONG)((ULONGLONG)PartitionLength->QuadPart >>
        VolumeExtension->ClusterShift) + FAT_RESERVED_FAT_ENTRIES;

    if (NumberOfClustersLimit < FAT_CLUSTER16_RESERVED) {
        VolumeExtension->Flags |= FAT_VOLUME_FAT16X;
        VolumeExtension->FatEntryShift = 1;
        NumberOfBytesPerFat = NumberOfClustersLimit * sizeof(USHORT);
    } else {
        VolumeExtension->FatEntryShift = 2;
        NumberOfBytesPerFat = NumberOfClustersLimit * sizeof(ULONG);
    }

    NumberOfBytesPerFat = (NumberOfBytesPerFat + (StructureAlignment - 1)) &
        ~(StructureAlignment - 1);

    //
    // Compute the number of bytes that haven't been reserved above.
    //

    NumberOfUnreservedBytes = (ULONGLONG)PartitionLength->QuadPart -
        NumberOfReservedBytes;

    //
    // Verify that the partition has enough space to contain the FAT.
    //

    if ((ULONGLONG)NumberOfBytesPerFat >= NumberOfUnreservedBytes) {
        FatxDbgPrint(("FATX: volume too small to hold the FAT\n"));
        status = STATUS_UNRECOGNIZED_VOLUME;
        goto CleanupAndExit;
    }

    //
    // Compute the number of clusters available on the volume.
    //

    NumberOfFileAreaBytes = NumberOfUnreservedBytes - NumberOfBytesPerFat;

    VolumeExtension->NumberOfClusters = (ULONG)(NumberOfFileAreaBytes >>
        VolumeExtension->ClusterShift);

    //
    // Compute the physical byte offsets of the FAT and the first cluster.
    //

    VolumeExtension->FatByteOffset = NumberOfReservedBytes;
    VolumeExtension->FileAreaByteOffset = VolumeExtension->FatByteOffset +
        NumberOfBytesPerFat;

    //
    // When dealing with the file system cache, other parts of the file system
    // assume that the file allocation table or file area start on a page
    // aligned byte offset.  Also, the number of bytes per cluster must be a
    // multiple of the page size.
    //

    ASSERT(BYTE_OFFSET(VolumeExtension->FatByteOffset) == 0);
    ASSERT(BYTE_OFFSET(VolumeExtension->FileAreaByteOffset) == 0);
    ASSERT(BYTE_OFFSET(VolumeExtension->BytesPerCluster) == 0);

    //
    // Create the file control block for the root directory.
    //

    status = FatxCreateFcb(NULL, RootDirectoryFirstCluster, NULL, 0,
        &VolumeExtension->RootDirectoryFcb);

    if (!NT_SUCCESS(status)) {
        goto CleanupAndExit;
    }

    status = STATUS_SUCCESS;

CleanupAndExit:
    if (CacheBuffer != NULL) {
        FscUnmapBuffer(CacheBuffer);
    }

    return status;
}

NTSTATUS
FatxMountVolume(
    IN PDEVICE_OBJECT TargetDeviceObject
    )
/*++

Routine Description:

    This routine is called by the I/O manager to attempt to mount this file
    system.

Arguments:

    TargetDeviceObject - Specifies the device object to attempt to mount.

Return Value:

    Status of operation.

--*/
{
    NTSTATUS status;
    PIRP Irp;
    PDEVICE_OBJECT VolumeDeviceObject;
    PFAT_VOLUME_EXTENSION VolumeExtension;
    DISK_GEOMETRY DiskGeometry;
    PARTITION_INFORMATION PartitionInformation;

    Irp = NULL;
    VolumeDeviceObject = NULL;

    FatxAcquireGlobalMutexExclusive();

    //
    // Get the drive geometry from the device.
    //

    status = IoSynchronousDeviceIoControlRequest(IOCTL_DISK_GET_DRIVE_GEOMETRY,
        TargetDeviceObject, NULL, 0, &DiskGeometry, sizeof(DISK_GEOMETRY), NULL,
        FALSE);

    if (!NT_SUCCESS(status)) {
        goto CleanupAndExit;
    }

    //
    // Get the partition information from the device.
    //

    status = IoSynchronousDeviceIoControlRequest(IOCTL_DISK_GET_PARTITION_INFO,
        TargetDeviceObject, NULL, 0, &PartitionInformation,
        sizeof(PARTITION_INFORMATION), NULL, FALSE);

    if (!NT_SUCCESS(status)) {
        goto CleanupAndExit;
    }

    //
    // Create an I/O request packet in order to submit requests to the target
    // device.
    //

    Irp = IoAllocateIrp(TargetDeviceObject->StackSize);

    if (Irp == NULL) {
        status = STATUS_INSUFFICIENT_RESOURCES;
        goto CleanupAndExit;
    }

    //
    // Create the volume's device object.
    //

    status = FatxCreateVolumeDevice(TargetDeviceObject, &DiskGeometry,
        &PartitionInformation, &VolumeDeviceObject);

    if (!NT_SUCCESS(status)) {
        goto CleanupAndExit;
    }

    VolumeExtension = (PFAT_VOLUME_EXTENSION)VolumeDeviceObject->DeviceExtension;

    //
    // Process the boot sector for the volume.
    //

    status = FatxProcessBootSector(VolumeDeviceObject, Irp,
        &PartitionInformation.PartitionLength);

    if (!NT_SUCCESS(status)) {
        goto CleanupAndExit;
    }

    //
    // Initialize the allocation support for the volume.
    //

    status = FatxInitializeAllocationSupport(VolumeExtension, Irp);

    if (!NT_SUCCESS(status)) {
        goto CleanupAndExit;
    }

    //
    // Connect the volume device object to the target device object and return
    // success.
    //

    TargetDeviceObject->MountedOrSelfDevice = VolumeDeviceObject;
    VolumeDeviceObject = NULL;

    status = STATUS_SUCCESS;

CleanupAndExit:
    if (VolumeDeviceObject != NULL) {
        FatxDeleteVolumeDevice(VolumeDeviceObject);
    }

    if (Irp != NULL) {
        IoFreeIrp(Irp);
    }

    FatxReleaseGlobalMutex();

    return status;
}
