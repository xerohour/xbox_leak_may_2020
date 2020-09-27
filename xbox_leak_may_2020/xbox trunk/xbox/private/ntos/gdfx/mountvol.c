/*++

Copyright (c) 2000-2001  Microsoft Corporation

Module Name:

    mountvol.c

Abstract:

    This module implements routines related to the mounting of a GDF volume.

--*/

#include "gdfx.h"

//
// GDF volume descriptor signature.
//
const UCHAR GdfxVolumeDescriptorSignature[] = GDF_VOLUME_DESCRIPTOR_SIGNATURE;

NTSTATUS
GdfxCreateVolumeDevice(
    IN PDEVICE_OBJECT TargetDeviceObject,
    IN PDISK_GEOMETRY DiskGeometry,
    OUT PDEVICE_OBJECT *ReturnedVolumeDeviceObject
    )
/*++

Routine Description:

    This routine creates an instance of a volume device object.

Arguments:

    TargetDeviceObject - Specifies the device object that the volume object
        sends I/O requests to.

    DiskGeometry - Specifies the disk geometry of the target device.

    ReturnedVolumeDeviceObject - Specifies the buffer to receive the created
        device object.

Return Value:

    Status of operation.

--*/
{
    NTSTATUS status;
    PDEVICE_OBJECT VolumeDeviceObject;
    PGDF_VOLUME_EXTENSION VolumeExtension;
    PGDF_FCB VolumeFcb;
    PGDF_FCB RootDirectoryFcb;

    //
    // Create the volume's device object.
    //

    status = IoCreateDevice(&GdfxDriverObject, sizeof(GDF_VOLUME_EXTENSION),
        NULL, FILE_DEVICE_CD_ROM_FILE_SYSTEM, FALSE, &VolumeDeviceObject);

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

    if (GdfxIsFlagSet(TargetDeviceObject->Flags, DO_SCATTER_GATHER_IO)) {
        VolumeDeviceObject->Flags |= DO_SCATTER_GATHER_IO;
    }

    //
    // Set the size of a sector for this device.
    //

    VolumeDeviceObject->SectorSize = GDF_CD_SECTOR_SIZE;

    //
    // Increment the reference count on the target device object since we'll be
    // holding a pointer to it.
    //

    ObReferenceObject(TargetDeviceObject);

    //
    // Initialize the volume's device extension data.
    //

    VolumeExtension = (PGDF_VOLUME_EXTENSION)VolumeDeviceObject->DeviceExtension;

    VolumeExtension->TargetDeviceObject = TargetDeviceObject;
    VolumeExtension->SectorSize = GDF_CD_SECTOR_SIZE;
    VolumeExtension->PartitionSectorCount = DiskGeometry->Cylinders.LowPart;
    VolumeExtension->PartitionLength.QuadPart =
        (ULONGLONG)DiskGeometry->Cylinders.LowPart << GDF_CD_SECTOR_SHIFT;

    //
    // Allocate the file control block for the physical volume.
    //

    VolumeFcb = ExAllocatePoolWithTag(sizeof(GDF_FCB), 'cVxG');

    if (VolumeFcb == NULL) {
        GdfxDeleteVolumeDevice(VolumeDeviceObject);
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    VolumeExtension->VolumeFcb = VolumeFcb;

    //
    // Initialize the file control block for the physical volume.
    //

    RtlZeroMemory(VolumeFcb, sizeof(GDF_FCB));

    VolumeFcb->Flags = GDF_FCB_VOLUME;
    VolumeFcb->ReferenceCount = 1;

    InitializeListHead(&VolumeFcb->SiblingFcbLink);

    //
    // Allocate the file control block for the physical volume.
    //

    RootDirectoryFcb = ExAllocatePoolWithTag(sizeof(GDF_FCB), 'cRxG');

    if (RootDirectoryFcb == NULL) {
        GdfxDeleteVolumeDevice(VolumeDeviceObject);
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    VolumeExtension->RootDirectoryFcb = RootDirectoryFcb;

    //
    // Initialize the file control block for the root directory;
    //

    RtlZeroMemory(RootDirectoryFcb, sizeof(GDF_FCB));

    RootDirectoryFcb->Flags = GDF_FCB_DIRECTORY | GDF_FCB_ROOT_DIRECTORY;
    RootDirectoryFcb->ReferenceCount = 1;

    InitializeListHead(&RootDirectoryFcb->SiblingFcbLink);
    InitializeListHead(&RootDirectoryFcb->ChildFcbList);

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
GdfxDeleteVolumeDevice(
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
    PGDF_VOLUME_EXTENSION VolumeExtension;

    VolumeExtension = (PGDF_VOLUME_EXTENSION)VolumeDeviceObject->DeviceExtension;

    //
    // There shouldn't be any open files for this volume.
    //

    ASSERT(VolumeExtension->FileObjectCount == 0);

    //
    // Delete any file control blocks created by the volume.
    //

    if (VolumeExtension->VolumeFcb != NULL) {
        GdfxDereferenceFcb(VolumeExtension->VolumeFcb);
    }

    if (VolumeExtension->RootDirectoryFcb != NULL) {
        GdfxDereferenceFcb(VolumeExtension->RootDirectoryFcb);
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
GdfxProcessVolumeDescriptor(
    IN PDEVICE_OBJECT VolumeDeviceObject,
    IN PIRP Irp
    )
/*++

Routine Description:

    This routine reads the volume descriptor for the supplied volume device
    object, validates its structure, and caches any interesting information in
    the device extension.

Arguments:

    VolumeDeviceObject - Specifies the volume device object to mount.

    Irp - Specifies the packet that describes the I/O request.

Return Value:

    Status of operation.

--*/
{
    NTSTATUS status;
    PGDF_VOLUME_EXTENSION VolumeExtension;
    PVOID CacheBuffer;
    PGDF_VOLUME_DESCRIPTOR VolumeDescriptor;

    VolumeExtension = (PGDF_VOLUME_EXTENSION)VolumeDeviceObject->DeviceExtension;

    //
    // If the volume is too small to hold a volume descriptor sector, then bail
    // out now.
    //

    if (VolumeExtension->PartitionSectorCount <= GDF_VOLUME_DESCRIPTOR_SECTOR) {
        return STATUS_UNRECOGNIZED_VOLUME;
    }

    //
    // Map the volume descriptor into the cache.
    //

    status = FscMapBuffer(&VolumeExtension->CacheExtension, Irp,
        GdfxSectorToPhysicalByteOffset(GDF_VOLUME_DESCRIPTOR_SECTOR), FALSE,
        &CacheBuffer);

    if (!NT_SUCCESS(status)) {
        return status;
    }

    VolumeDescriptor = (PGDF_VOLUME_DESCRIPTOR)CacheBuffer;

    //
    // Validate the signature of the sector.
    //

    if (RtlEqualMemory(VolumeDescriptor->HeadSignature,
            GdfxVolumeDescriptorSignature, GDF_VOLUME_DESCRIPTOR_SIGNATURE_LENGTH) &&
        RtlEqualMemory(VolumeDescriptor->TailSignature,
            GdfxVolumeDescriptorSignature, GDF_VOLUME_DESCRIPTOR_SIGNATURE_LENGTH)) {

        //
        // Validate that the root directory is contained within the volume.
        //

        if ((VolumeDescriptor->RootDirectoryFirstSector < VolumeExtension->PartitionSectorCount) &&
            ((VolumeExtension->PartitionSectorCount - VolumeDescriptor->RootDirectoryFirstSector) >=
                (GDF_CD_SECTOR_ALIGN_UP(VolumeDescriptor->RootDirectoryFileSize) >> GDF_CD_SECTOR_SHIFT))) {

            //
            // Save the starting sector and size of the root directory in the
            // root directory file control block.
            //

            VolumeExtension->RootDirectoryFcb->FirstSector =
                VolumeDescriptor->RootDirectoryFirstSector;
            VolumeExtension->RootDirectoryFcb->FileSize =
                VolumeDescriptor->RootDirectoryFileSize;

            //
            // Save the global time stamp for the volume's files.
            //

            VolumeExtension->TimeStamp = VolumeDescriptor->TimeStamp;

            status = STATUS_SUCCESS;

        } else {
            status = STATUS_DISK_CORRUPT_ERROR;
        }

    } else {
        status = STATUS_UNRECOGNIZED_VOLUME;
    }

    //
    // Unmap the cache buffer.
    //

    FscUnmapBuffer(CacheBuffer);

    return status;
}

NTSTATUS
GdfxMountVolume(
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
    DISK_GEOMETRY DiskGeometry;

    Irp = NULL;
    VolumeDeviceObject = NULL;

    GdfxAcquireGlobalMutexExclusive();

    //
    // Attempt to perform the DVD-X2 authentication sequence in order to switch
    // to the Xbox partition.  If this fails, continue using the standard
    // partition.
    //

    IoSynchronousDeviceIoControlRequest(IOCTL_CDROM_AUTHENTICATION_SEQUENCE,
        TargetDeviceObject, NULL, 0, NULL, 0, NULL, FALSE);

    //
    // Get the drive geometry from the device.
    //

    status = IoSynchronousDeviceIoControlRequest(IOCTL_CDROM_GET_DRIVE_GEOMETRY,
        TargetDeviceObject, NULL, 0, &DiskGeometry, sizeof(DISK_GEOMETRY), NULL,
        FALSE);

    if (!NT_SUCCESS(status)) {
        goto CleanupAndExit;
    }

    ASSERT(DiskGeometry.Cylinders.HighPart == 0);
    ASSERT(DiskGeometry.TracksPerCylinder == 1);
    ASSERT(DiskGeometry.SectorsPerTrack == 1);
    ASSERT(DiskGeometry.BytesPerSector == GDF_CD_SECTOR_SIZE);

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

    status = GdfxCreateVolumeDevice(TargetDeviceObject, &DiskGeometry,
        &VolumeDeviceObject);

    if (!NT_SUCCESS(status)) {
        goto CleanupAndExit;
    }

    //
    // Process the volume descriptor for the volume.
    //

    status = GdfxProcessVolumeDescriptor(VolumeDeviceObject, Irp);

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
        GdfxDeleteVolumeDevice(VolumeDeviceObject);
    }

    if (Irp != NULL) {
        IoFreeIrp(Irp);
    }

    GdfxReleaseGlobalMutex();

    return status;
}
