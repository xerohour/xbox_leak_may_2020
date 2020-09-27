/*++

Copyright (c) 2000-2001  Microsoft Corporation

Module Name:

    mountvol.c

Abstract:

    This module implements routines related to the mounting of a UDF volume.

--*/

#include "udfx.h"

//
// Define a lookup table to map from VSD identifier string to its corresponding
// VSD identifier enumeration.
//

#include <pshpack1.h>

typedef struct _VSD_IDENT_LOOKUP_TABLE {
    UCHAR Ident[VSD_LENGTH_IDENT];
    UCHAR IdentEnum;
} VSD_IDENT_LOOKUP_TABLE, *PVSD_IDENT_LOOKUP_TABLE;

#include <poppack.h>

// C4295: array is too small to include a terminating null character
#pragma warning(disable:4295)

const VSD_IDENT_LOOKUP_TABLE UdfxVsdIdentLookupTable[] = {
    VSD_IDENT_BEA01, VsdIdentBEA01,
    VSD_IDENT_TEA01, VsdIdentTEA01,
    VSD_IDENT_CDROM, VsdIdentCDROM,
    VSD_IDENT_CD001, VsdIdentCD001,
    VSD_IDENT_NSR01, VsdIdentNSR01,
    VSD_IDENT_NSR02, VsdIdentNSR02,
    VSD_IDENT_CDW01, VsdIdentCDW01,
    VSD_IDENT_CDW02, VsdIdentCDW02,
    VSD_IDENT_BOOT2, VsdIdentBOOT2
};

#pragma warning(default:4295)

//
// Entity identifier for a ECMA-167 standard partition.
//
const UCHAR UdfxNSR02Regid[REGID_LENGTH_IDENT] = NSR_PART_CONTID_NSR02;

//
// Entity identifier for a UDF standard volume.
//
const UCHAR UdfxOSTAUdfRegid[REGID_LENGTH_IDENT] = "*OSTA UDF Compliant";

//
// Character set specification for a UDF partition.
//
const CHARSPEC UdfxCS0Identifier = {
    CHARSPEC_T_CS0, "OSTA Compressed Unicode"
};

NTSTATUS
UdfxCreateVolumeDevice(
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
    PUDF_VOLUME_EXTENSION VolumeExtension;
    PUDF_FCB VolumeFcb;

    //
    // Create the volume's device object.
    //

    status = IoCreateDevice(&UdfxDriverObject, sizeof(UDF_VOLUME_EXTENSION),
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

    if (UdfxIsFlagSet(TargetDeviceObject->Flags, DO_SCATTER_GATHER_IO)) {
        VolumeDeviceObject->Flags |= DO_SCATTER_GATHER_IO;
    }

    //
    // Set the size of a sector for this device.
    //

    VolumeDeviceObject->SectorSize = UDF_CD_SECTOR_SIZE;

    //
    // Increment the reference count on the target device object since we'll be
    // holding a pointer to it.
    //

    ObReferenceObject(TargetDeviceObject);

    //
    // Initialize the volume's device extension data.
    //

    VolumeExtension = (PUDF_VOLUME_EXTENSION)VolumeDeviceObject->DeviceExtension;

    VolumeExtension->TargetDeviceObject = TargetDeviceObject;
    VolumeExtension->SectorSize = UDF_CD_SECTOR_SIZE;
    VolumeExtension->PhysicalSectorCount = DiskGeometry->Cylinders.LowPart;

    //
    // Allocate the file control block for the physical volume.
    //

    VolumeFcb = ExAllocatePoolWithTag(sizeof(UDF_FCB), 'cVxU');

    if (VolumeFcb == NULL) {
        UdfxDeleteVolumeDevice(VolumeDeviceObject);
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    VolumeExtension->VolumeFcb = VolumeFcb;
    VolumeExtension->RootDirectoryFcb = NULL;

    //
    // Initialize the file control block for the physical volume.
    //

    RtlZeroMemory(VolumeFcb, sizeof(UDF_FCB));

    VolumeFcb->Flags = UDF_FCB_VOLUME;
    VolumeFcb->ReferenceCount = 1;

    InitializeListHead(&VolumeFcb->SiblingFcbLink);

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
UdfxDeleteVolumeDevice(
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
    PUDF_VOLUME_EXTENSION VolumeExtension;

    VolumeExtension = (PUDF_VOLUME_EXTENSION)VolumeDeviceObject->DeviceExtension;

    //
    // There shouldn't be any open files for this volume.
    //

    ASSERT(VolumeExtension->FileObjectCount == 0);

    //
    // Delete any file control blocks created by the volume.
    //

    if (VolumeExtension->VolumeFcb != NULL) {
        UdfxDereferenceFcb(VolumeExtension->VolumeFcb);
    }

    if (VolumeExtension->RootDirectoryFcb != NULL) {
        UdfxDereferenceFcb(VolumeExtension->RootDirectoryFcb);
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
UdfxReadPhysicalSector(
    IN PUDF_VOLUME_EXTENSION VolumeExtension,
    IN ULONG PhysicalSectorNumber,
    OUT PVOID Buffer
    )
/*++

Routine Description:

    This routine reads from the supplied device object a single physical sector.

Arguments:

    VolumeExtension - Specifies the volume extension to read from.

    PhysicalSectorNumber - Specifies the physical sector number to read.

    Buffer - Specifies the buffer to read the data into.

Return Value:

    Status of operation.

--*/
{
    LARGE_INTEGER ByteOffset;

    //
    // Verify that the physical sector resides inside the disk.
    //

    if (PhysicalSectorNumber >= VolumeExtension->PhysicalSectorCount) {
        return STATUS_DISK_CORRUPT_ERROR;
    }

    //
    // Read the physical sector.
    //

    ByteOffset.QuadPart = (ULONGLONG)PhysicalSectorNumber << UDF_CD_SECTOR_SHIFT;

    return IoSynchronousFsdRequest(IRP_MJ_READ, VolumeExtension->TargetDeviceObject,
        Buffer, UDF_CD_SECTOR_SIZE, &ByteOffset);
}

NTSTATUS
UdfxRecognizeVolume(
    IN PUDF_VOLUME_EXTENSION VolumeExtension
    )
/*++

Routine Description:

    This routine checks if a UDF volume is in the specified device.

    Refer to ISO 13346 section 2/8.

Arguments:

    VolumeExtension - Specifies the extension for the volume to mount.

Return Value:

    Status of operation.

--*/
{
    NTSTATUS status;
    PVSD_GENERIC VolumeStructureDescriptor;
    ULONG PhysicalSectorNumber;
    ULONG LookupIndex;
    VSD_IDENT VsdIdent;
    BOOLEAN FoundBEA;

    //
    // Allocate a buffer to read the descriptors into.  The buffer size must be
    // aligned to the size of a CD-ROM sector.
    //

    VolumeStructureDescriptor = (PVSD_GENERIC)ExAllocatePoolWithTag(UDF_CD_SECTOR_SIZE,
        'sVxU');

    if (VolumeStructureDescriptor == NULL) {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    //
    // The volume recognition area begins at VRA_BOUNDARY_LOCATION rounded up to
    // the next sector.
    //

    PhysicalSectorNumber = UDF_CD_SECTOR_ALIGN_UP(VRA_BOUNDARY_LOCATION) >>
        UDF_CD_SECTOR_SHIFT;

    //
    // Process the volume structure descriptors.
    //

    status = STATUS_UNRECOGNIZED_VOLUME;
    FoundBEA = FALSE;

    for (;;) {

        //
        // Verify that the physical sector is in the bounds of the disk.
        //

        if (PhysicalSectorNumber >= VolumeExtension->PhysicalSectorCount) {
            status = STATUS_UNRECOGNIZED_VOLUME;
            break;
        }

        //
        // Read the next volume structure descriptor.
        //

        status = UdfxReadPhysicalSector(VolumeExtension, PhysicalSectorNumber,
            VolumeStructureDescriptor);

        if (!NT_SUCCESS(status)) {
            break;
        }

        //
        // Set up the error code for an invalid UDF volume.
        //

        status = STATUS_UNRECOGNIZED_VOLUME;

        //
        // Map the VSD identifier string to its VSD identifier enumeration.  If
        // we find an unrecognized identifier, then this can't be a UDF volume
        // that we support.
        //

        VsdIdent = VsdIdentBad;

        for (LookupIndex = 0; LookupIndex <
            ARRAY_ELEMENTS(UdfxVsdIdentLookupTable); LookupIndex++) {

            if (RtlEqualMemory(VolumeStructureDescriptor->Ident,
                UdfxVsdIdentLookupTable[LookupIndex].Ident, VSD_LENGTH_IDENT)) {
                VsdIdent = UdfxVsdIdentLookupTable[LookupIndex].IdentEnum;
                break;
            }
        }

        if (VsdIdent == VsdIdentBad) {
            UdfxDbgPrint(("UDFX: unknown VSD identifier\n"));
            break;
        }

        //
        // Process the VSD type based on the VSD identifier.
        //

        if (VolumeStructureDescriptor->Type == 0) {

            if (VsdIdent == VsdIdentBEA01) {

                //
                // Beginning Extended Area descriptor.  There should be one and
                // only one of these descriptors.
                //

                if (FoundBEA) {
                    UdfxDbgPrint(("UDFX: found multiple BEA01 descriptors\n"));
                    break;
                }

                if (VolumeStructureDescriptor->Version != 1) {
                    UdfxDbgPrint(("UDFX: found BEA01 descriptor with unknown version\n"));
                    break;
                }

                FoundBEA = TRUE;

            } else if (VsdIdent == VsdIdentNSR02) {

                //
                // NSR02 descriptor.  There should be one of these descriptors
                // following the BEA01 descriptor.
                //

                if (FoundBEA) {

                    if (VolumeStructureDescriptor->Version != 1) {
                        UdfxDbgPrint(("UDFX: found NSR02 descriptor with unknown version\n"));
                        break;
                    }

                    status = STATUS_SUCCESS;
                    break;

                } else {

                    UdfxDbgPrint(("UDFX: found NSR02 descriptor without BEA01 descriptor\n"));
                    break;
                }

            } else if (VsdIdent == VsdIdentTEA01) {

                //
                // Terminating Extended Area descriptor.  This marks the end of
                // the descriptors that could mark a UDF volume.
                //

                break;
            }

        } else if ((VolumeStructureDescriptor->Type < 3) ||
            (VolumeStructureDescriptor->Type == 255)) {

            if (FoundBEA) {
                UdfxDbgPrint(("UDFX: found unexpected descriptors after BEA01\n"));
                break;
            }

            if ((VsdIdent != VsdIdentCDROM) && (VsdIdent != VsdIdentCD001)) {
                UdfxDbgPrint(("UDFX: unknown VSD type\n"));
                break;
            }

        } else {

            UdfxDbgPrint(("UDFX: unknown VSD type\n"));
            break;
        }

        //
        // Advance to the next sector.
        //

        PhysicalSectorNumber++;
    }

    ExFreePool(VolumeStructureDescriptor);

    return status;
}

NTSTATUS
UdfxProcessAnchorVolumeDescriptor(
    IN PUDF_VOLUME_EXTENSION VolumeExtension,
    OUT PEXTENTAD MainVolumeDescriptorSequenceExtent
    )
/*++

Routine Description:

    This routine finds the anchor volume descriptor from the supplied device.

    Refer to ISO 13346 section 3/8.4.

Arguments:

    VolumeExtension - Specifies the extension for the volume to mount.

    MainVolumeDescriptorSequenceExtent - Specifies the location to receive the
        main volume descriptor sequence extent from the anchor (3/10.2.2).

Return Value:

    Status of operation.

--*/
{
    NTSTATUS status;
    PNSR_ANCHOR AnchorVolumeDescriptor;

    //
    // Allocate a buffer to read the descriptors into.  The buffer size must be
    // aligned to the size of a CD-ROM sector.
    //

    AnchorVolumeDescriptor = (PNSR_ANCHOR)ExAllocatePoolWithTag(UDF_CD_SECTOR_SIZE,
        'vAxU');

    if (AnchorVolumeDescriptor == NULL) {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    //
    // Read the anchor volume descriptor from its typical location.
    //

    status = UdfxReadPhysicalSector(VolumeExtension, ANCHOR_SECTOR,
        AnchorVolumeDescriptor);

    if (NT_SUCCESS(status)) {

        if (UdfxVerifyDescriptor(&AnchorVolumeDescriptor->Destag,
            sizeof(NSR_ANCHOR), DESTAG_ID_NSR_ANCHOR, ANCHOR_SECTOR)) {

            //
            // Return the interesting fields from the descriptor back to the
            // caller.
            //

            *MainVolumeDescriptorSequenceExtent = AnchorVolumeDescriptor->Main;

            status = STATUS_SUCCESS;

        } else {

            UdfxDbgPrint(("UDFX: anchor volume descriptor not valid\n"));
            status = STATUS_DISK_CORRUPT_ERROR;
        }
    }

    ExFreePool(AnchorVolumeDescriptor);

    return status;
}

BOOLEAN
UdfxProcessPrimaryVolumeDescriptor(
    IN PNSR_PVD PrimaryVolumeDescriptor
    )
/*++

Routine Description:

    This routine validates and pulls out any interesting information from the
    supplied primary volume descriptor.

    Refer to ISO 13346 section 3/10.1.

Arguments:

    PrimaryVolumeDescriptor - Specifies the descriptor to process.

Return Value:

    Returns TRUE if the descriptor is valid, else FALSE.

--*/
{
    //
    // Verify that the volume is a single volume set.  Note that we ignore
    // VolSetSeqMax because some DVDs are authored incorrectly (Snow White).
    //

    if (PrimaryVolumeDescriptor->VolSetSeq > 1) {
        UdfxDbgPrint(("UDFX: invalid primary volume descriptor (line %d)\n", __LINE__));
        return FALSE;
    }

    //
    // Verify that the volume is using only the CS0 character set.
    //

    if ((PrimaryVolumeDescriptor->CharSetList != (1 << CHARSPEC_T_CS0)) ||
        (PrimaryVolumeDescriptor->CharSetListMax != (1 << CHARSPEC_T_CS0))) {
        UdfxDbgPrint(("UDFX: invalid primary volume descriptor (line %d)\n", __LINE__));
        return FALSE;
    }

    //
    // Verify that the volume is using the UDF domain character set.
    //

    if (!RtlEqualMemory(&PrimaryVolumeDescriptor->CharsetDesc,
            &UdfxCS0Identifier, sizeof(CHARSPEC)) ||
        !RtlEqualMemory(&PrimaryVolumeDescriptor->CharsetExplan,
            &UdfxCS0Identifier, sizeof(CHARSPEC))) {
        UdfxDbgPrint(("UDFX: invalid primary volume descriptor (line %d)\n", __LINE__));
        return FALSE;
    }

    return TRUE;
}

BOOLEAN
UdfxProcessPartitionDescriptor(
    IN PNSR_PART PartitionDescriptor,
    IN PUDF_VOLUME_EXTENSION VolumeExtension,
    OUT PUSHORT PartitionNumber
    )
/*++

Routine Description:

    This routine validates and pulls out any interesting information from the
    supplied partition descriptor.

    Refer to ISO 13346 section 3/10.5.

Arguments:

    PartitionDescriptor - Specifies the descriptor to process.

    VolumeExtension - Specifies the extension for the volume to mount.

    PartitionNumber - Specifies the buffer to receive the expected partition
        number.

Return Value:

    Returns TRUE if the descriptor is valid, else FALSE.

--*/
{
    //
    // Verify that the partition's content identifier is valid.
    //

    if (!RtlEqualMemory(&PartitionDescriptor->ContentsID.Identifier,
        &UdfxNSR02Regid, REGID_LENGTH_IDENT)) {
        UdfxDbgPrint(("UDFX: invalid partition descriptor (line %d)\n", __LINE__));
        return FALSE;
    }

    //
    // The descriptor appears to be valid.  Return any interesting information
    // in the volume extension and to the caller.
    //

    VolumeExtension->PartitionSectorStart = PartitionDescriptor->Start;
    VolumeExtension->PartitionSectorCount = PartitionDescriptor->Length;

    VolumeExtension->PartitionLength.QuadPart =
    VolumeExtension->VolumeFcb->FileSize.QuadPart =
        ((ULONGLONG)VolumeExtension->PartitionSectorStart +
        VolumeExtension->PartitionSectorCount) << UDF_CD_SECTOR_SHIFT;

    *PartitionNumber = PartitionDescriptor->Number;

    return TRUE;
}

BOOLEAN
UdfxProcessLogicalVolumeDescriptor(
    IN PNSR_LVOL LogicalVolumeDescriptor,
    IN PLONGAD FileSetDescriptorExtent,
    IN PUSHORT PartitionNumber
    )
/*++

Routine Description:

    This routine validates and pulls out any interesting information from the
    supplied logical volume descriptor.

    Refer to ISO 13346 section 3/10.6.

Arguments:

    LogicalVolumeDescriptor - Specifies the descriptor to process.

    FileSetDescriptorExtent - Specifies the buffer to receive the extents of the
        file set descriptor for the volume.

    PartitionNumber - Specifies the buffer to receive the expected partition
        number.

Return Value:

    Returns TRUE if the descriptor is valid, else FALSE.

--*/
{
    USHORT UdfVersion;
    PPARTMAP_PHYSICAL PartitionMap;

    //
    // Verify that the descriptor's size doesn't exceed the size of a sector
    // and that the volume's block size is equal to the device's sector size.
    //

    if ((ISONsrLvolSize(LogicalVolumeDescriptor) > UDF_CD_SECTOR_SIZE) ||
        (LogicalVolumeDescriptor->BlockSize != UDF_CD_SECTOR_SIZE)) {
        UdfxDbgPrint(("UDFX: invalid logical volume descriptor (line %d)\n", __LINE__));
        return FALSE;
    }

    //
    // Verify that the volume is using the UDF domain.
    //

    if (!RtlEqualMemory(&LogicalVolumeDescriptor->DomainID.Identifier,
        &UdfxOSTAUdfRegid, REGID_LENGTH_IDENT)) {
        UdfxDbgPrint(("UDFX: invalid logical volume descriptor (line %d)\n", __LINE__));
        return FALSE;
    }

    //
    // Verify that this is a supported UDF version.
    //

    UdfVersion = *((PUSHORT)&LogicalVolumeDescriptor->DomainID.Suffix[0]);

    if ((UdfVersion < UDF_VERSION_MINIMUM) || (UdfVersion > UDF_VERSION_MAXIMUM)) {
        UdfxDbgPrint(("UDFX: unsupported UDF version %04x\n", UdfVersion));
        return FALSE;
    }

    //
    // Verify that the volume is using the UDF domain character set.
    //

    if (!RtlEqualMemory(&LogicalVolumeDescriptor->Charset, &UdfxCS0Identifier,
        sizeof(CHARSPEC))) {
        UdfxDbgPrint(("UDFX: invalid logical volume descriptor (line %d)\n", __LINE__));
        return FALSE;
    }

    //
    // Only allow one partition in the logical volume (level 1 medium
    // interchange).
    //

    if ((LogicalVolumeDescriptor->MapTableCount > 1) ||
        (LogicalVolumeDescriptor->MapTableLength < sizeof(PARTMAP_PHYSICAL))) {
        UdfxDbgPrint(("UDFX: invalid logical volume descriptor (line %d)\n", __LINE__));
        return FALSE;
    }

    //
    // Verify that this is a physical partition type of the expected size and
    // that this is a single volume set.
    //

    PartitionMap = (PPARTMAP_PHYSICAL)LogicalVolumeDescriptor->MapTable;

    if ((PartitionMap->Type != PARTMAP_TYPE_PHYSICAL) ||
        (PartitionMap->Length != sizeof(PARTMAP_PHYSICAL)) ||
        (PartitionMap->VolSetSeq > 1)) {
        UdfxDbgPrint(("UDFX: invalid partition map descriptor (line %d)\n", __LINE__));
        return FALSE;
    }

    //
    // The descriptor appears to be valid.  Return any interesting information
    // back to the caller.
    //

    *FileSetDescriptorExtent = LogicalVolumeDescriptor->FSD;
    *PartitionNumber = PartitionMap->Partition;

    return TRUE;
}

NTSTATUS
UdfxProcessVolumeDescriptorSequence(
    IN PUDF_VOLUME_EXTENSION VolumeExtension,
    IN PEXTENTAD VolumeDescriptorSequenceExtent,
    OUT PLONGAD FileSetDescriptorExtent
    )
/*++

Routine Description:

    This routine processes the volume data structures bounded by the supplied
    volume descriptor sequence extents.

Arguments:

    VolumeExtension - Specifies the extension for the volume to mount.

    VolumeDescriptorSequenceExtent - Specifies the extents of the volume
        descriptor sequence area.

    FileSetDescriptorExtent - Specifies the buffer to receive the extents of the
        file set descriptor for the volume.

Return Value:

    Status of operation.

--*/
{
    NTSTATUS status;
    PNSR_VD_GENERIC VolumeDataStructure;
    BOOLEAN SeenPrimaryVolumeDescriptor;
    ULONG PrevailingPVDVolDescSeqNum;
    BOOLEAN SeenPartitionDescriptor;
    ULONG PrevailingPartitionVolDescSeqNum;
    BOOLEAN SeenLogicalVolumeDescriptor;
    ULONG PrevailingLVOLVolDescSeqNum;
    USHORT PartitionNumber;
    USHORT ExpectedPartitionNumber;
    ULONG LogicalSectorNumber;
    ULONG EndingLogicalSectorNumber;
    PNSR_PVD PrimaryVolumeDescriptor;
    PNSR_LVOL LogicalVolumeDescriptor;
    PNSR_PART PartitionDescriptor;

    //
    // Verify that the extent is correctly formed (3/7.1).
    //

    if ((VolumeDescriptorSequenceExtent->Len == 0) ||
        !UdfxIsSectorAligned(VolumeDescriptorSequenceExtent->Len)) {
        return STATUS_DISK_CORRUPT_ERROR;
    }

    //
    // Allocate a buffer to read the descriptors into.  The buffer size must be
    // aligned to the size of a CD-ROM sector.
    //

    VolumeDataStructure = (PNSR_VD_GENERIC)ExAllocatePoolWithTag(UDF_CD_SECTOR_SIZE,
        'dVxU');

    if (VolumeDataStructure == NULL) {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    //
    // Reset the state variables before we begin processing the descriptors.
    //

    SeenPrimaryVolumeDescriptor = FALSE;
    PrevailingPVDVolDescSeqNum = 0;
    SeenPartitionDescriptor = FALSE;
    PrevailingPartitionVolDescSeqNum = 0;
    SeenLogicalVolumeDescriptor = FALSE;
    PrevailingLVOLVolDescSeqNum = 0;
    PartitionNumber = 0;
    ExpectedPartitionNumber = 0;

    //
    // The volume descriptor sequence area starts at the sector given in the
    // supplied extent structure.
    //

    status = STATUS_UNRECOGNIZED_VOLUME;
    LogicalSectorNumber = VolumeDescriptorSequenceExtent->Lsn;
    EndingLogicalSectorNumber = LogicalSectorNumber +
        (VolumeDescriptorSequenceExtent->Len >> UDF_CD_SECTOR_SHIFT);

    //
    // Process the volume data structures.
    //

    while (LogicalSectorNumber < EndingLogicalSectorNumber) {

        //
        // Read the next volume data structure.
        //

        status = UdfxReadPhysicalSector(VolumeExtension, LogicalSectorNumber,
            VolumeDataStructure);

        if (!NT_SUCCESS(status)) {
            break;
        }

        //
        // Set up the error code for an invalid UDF volume.
        //

        status = STATUS_UNRECOGNIZED_VOLUME;

        //
        // Verify that this is a valid volume data structure.
        //

        if (!UdfxVerifyDescriptor(&VolumeDataStructure->Destag,
            sizeof(NSR_VD_GENERIC), VolumeDataStructure->Destag.Ident,
            LogicalSectorNumber)) {
            UdfxDbgPrint(("UDFX: invalid descriptor (line %d)\n", __LINE__));
            break;
        }

        //
        // Process the volume data structure.
        //

        if (VolumeDataStructure->Destag.Ident == DESTAG_ID_NSR_PVD) {

            //
            // If we find a primary volume descriptor, then process it if it's
            // the prevailing descriptor.
            //

            PrimaryVolumeDescriptor = (PNSR_PVD)VolumeDataStructure;

            if (PrimaryVolumeDescriptor->VolDescSeqNum >=
                PrevailingPVDVolDescSeqNum) {

                SeenPrimaryVolumeDescriptor =
                    UdfxProcessPrimaryVolumeDescriptor(PrimaryVolumeDescriptor);

                PrevailingPVDVolDescSeqNum =
                    PrimaryVolumeDescriptor->VolDescSeqNum;
            }

        } else if (VolumeDataStructure->Destag.Ident == DESTAG_ID_NSR_PART) {

            //
            // If we find a partition descriptor, then process it if it's the
            // prevailing descriptor.
            //

            PartitionDescriptor = (PNSR_PART)VolumeDataStructure;

            if (PartitionDescriptor->VolDescSeqNum >=
                PrevailingPartitionVolDescSeqNum) {

                SeenPartitionDescriptor =
                    UdfxProcessPartitionDescriptor(PartitionDescriptor,
                    VolumeExtension, &PartitionNumber);

                PrevailingPartitionVolDescSeqNum =
                    PartitionDescriptor->VolDescSeqNum;
            }

        } else if (VolumeDataStructure->Destag.Ident == DESTAG_ID_NSR_LVOL) {

            //
            // If we find a logical volume descriptor, then process it if it's
            // the prevailing descriptor.
            //

            LogicalVolumeDescriptor = (PNSR_LVOL)VolumeDataStructure;

            if (LogicalVolumeDescriptor->VolDescSeqNum >=
                PrevailingLVOLVolDescSeqNum) {

                SeenLogicalVolumeDescriptor =
                    UdfxProcessLogicalVolumeDescriptor(LogicalVolumeDescriptor,
                    FileSetDescriptorExtent, &ExpectedPartitionNumber);

                PrevailingLVOLVolDescSeqNum =
                    LogicalVolumeDescriptor->VolDescSeqNum;
            }

        } else if (VolumeDataStructure->Destag.Ident == DESTAG_ID_NSR_TERM) {

            //
            // If we find the terminating descriptor, then we're at the end of
            // the data structures we care about.
            //
            // Refer to ISO 13346 section 3/10.9.
            //

            if (SeenPrimaryVolumeDescriptor && SeenLogicalVolumeDescriptor &&
                SeenPartitionDescriptor &&
                (PartitionNumber == ExpectedPartitionNumber)) {
                status = STATUS_SUCCESS;
            }

            break;
        }

        //
        // Advance to the next sector.
        //

        LogicalSectorNumber++;
    }

    ExFreePool(VolumeDataStructure);

    return status;
}

NTSTATUS
UdfxProcessFileSetDescriptor(
    IN PUDF_VOLUME_EXTENSION VolumeExtension,
    IN PLONGAD FileSetDescriptorExtent,
    OUT PLONGAD IcbRoot
    )
/*++

Routine Description:

    This routine processes the file set descriptor.  The search for the
    prevailing descriptor begins at the supplied extent.

    Refer to ISO 13346 section 4/14.1.

Arguments:

    VolumeDeviceObject - Specifies the volume device object that's being
        mounted.

    FileSetDescriptorExtent - Specifies the extent of the file set descriptor.

    IcbRoot - Specifies the buffer to receive the extent of the root directory
        ICB.

Return Value:

    Returns TRUE if the descriptor is valid, else FALSE.

--*/
{
    NTSTATUS status;
    PNSR_VD_GENERIC VolumeDataStructure;
    BOOLEAN SeenFileSetDescriptor;
    ULONG PrevailingFSDFileSet;
    ULONG LogicalSectorNumber;
    ULONG EndingLogicalSectorNumber;
    PNSR_FSD FileSetDescriptor;

    //
    // Verify that the extent is correctly formed (4/14.1);
    //

    if ((FileSetDescriptorExtent->Length.Length == 0) ||
        (FileSetDescriptorExtent->Length.Type != NSRLENGTH_TYPE_RECORDED) ||
        !UdfxIsSectorAligned(FileSetDescriptorExtent->Length.Length)) {
        return STATUS_DISK_CORRUPT_ERROR;
    }

    //
    // Allocate a buffer to read the descriptors into.  The buffer size must be
    // aligned to the size of a CD-ROM sector.
    //

    VolumeDataStructure = (PNSR_VD_GENERIC)ExAllocatePoolWithTag(UDF_CD_SECTOR_SIZE,
        'dVxU');

    if (VolumeDataStructure == NULL) {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    //
    // Reset the state variables before we begin processing the descriptors.
    //

    SeenFileSetDescriptor = FALSE;
    PrevailingFSDFileSet = 0;

    //
    // The first file set descriptor starts at the sector given in the
    // supplied extent structure.
    //

    status = STATUS_DISK_CORRUPT_ERROR;
    LogicalSectorNumber = FileSetDescriptorExtent->Start.Lbn;
    EndingLogicalSectorNumber = LogicalSectorNumber +
        (FileSetDescriptorExtent->Length.Length >> UDF_CD_SECTOR_SHIFT);

    //
    // Process the volume data structures.
    //

    while (LogicalSectorNumber < EndingLogicalSectorNumber) {

        //
        // Verify that the logical sector resides inside the physical partition.
        //

        if (LogicalSectorNumber >= VolumeExtension->PartitionSectorCount) {
            break;
        }

        //
        // Read the next volume data structure.
        //

        status = UdfxReadPhysicalSector(VolumeExtension,
            VolumeExtension->PartitionSectorStart + LogicalSectorNumber,
            VolumeDataStructure);

        if (!NT_SUCCESS(status)) {
            break;
        }

        //
        // Set up the error code for a corrupt UDF volume.
        //

        status = STATUS_DISK_CORRUPT_ERROR;

        //
        // Verify that this is a valid volume data structure.
        //

        if (!UdfxVerifyDescriptor(&VolumeDataStructure->Destag,
            sizeof(NSR_VD_GENERIC), VolumeDataStructure->Destag.Ident,
            LogicalSectorNumber)) {
            UdfxDbgPrint(("UDFX: invalid descriptor (line %d)\n", __LINE__));
            break;
        }

        //
        // Process the volume data structure.
        //

        if (VolumeDataStructure->Destag.Ident == DESTAG_ID_NSR_FSD) {

            //
            // If we find a file set descriptor, then process it if it's the
            // prevailing descriptor.
            //

            FileSetDescriptor = (PNSR_FSD)VolumeDataStructure;

            if (FileSetDescriptor->FileSet >= PrevailingFSDFileSet) {

                *IcbRoot = FileSetDescriptor->IcbRoot;

                PrevailingFSDFileSet = FileSetDescriptor->FileSet;
                SeenFileSetDescriptor = TRUE;

                if (FileSetDescriptor->NextExtent.Length.Length != 0) {

                    //
                    // The next extent is not zero, so verify that the extent is
                    // valid and restart the loop at the new extent.
                    //

                    if ((FileSetDescriptor->NextExtent.Length.Type != NSRLENGTH_TYPE_RECORDED) ||
                        !UdfxIsSectorAligned(FileSetDescriptor->NextExtent.Length.Length)) {
                        break;
                    }

                    LogicalSectorNumber = FileSetDescriptor->NextExtent.Start.Lbn;
                    EndingLogicalSectorNumber = LogicalSectorNumber +
                        (FileSetDescriptor->NextExtent.Length.Length >>
                        UDF_CD_SECTOR_SHIFT);

                    continue;
                }
            }

        } else if (VolumeDataStructure->Destag.Ident == DESTAG_ID_NSR_TERM) {

            //
            // If we find the terminating descriptor, then we're at the end of
            // the data structures we care about.
            //
            // Refer to ISO 13346 section 4/14.2.
            //

            if (SeenFileSetDescriptor) {
                status = STATUS_SUCCESS;
            }

            break;

        } else {

            //
            // An invalid descriptor was found.  Terminate the loop since we
            // have no idea what we're in the middle of now.
            //

            break;
        }

        //
        // Advance to the next sector.
        //

        LogicalSectorNumber++;
    }

    ExFreePool(VolumeDataStructure);

    return status;
}

NTSTATUS
UdfxMountVolume(
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
    PUDF_VOLUME_EXTENSION VolumeExtension;
    DISK_GEOMETRY DiskGeometry;
    EXTENTAD MainVolumeDescriptorSequenceExtent;
    LONGAD FileSetDescriptorExtent;
    LONGAD IcbRoot;

    Irp = NULL;
    VolumeDeviceObject = NULL;

    UdfxAcquireGlobalMutexExclusive();

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
    ASSERT(DiskGeometry.BytesPerSector == UDF_CD_SECTOR_SIZE);

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

    status = UdfxCreateVolumeDevice(TargetDeviceObject, &DiskGeometry,
        &VolumeDeviceObject);

    if (!NT_SUCCESS(status)) {
        goto CleanupAndExit;
    }

    VolumeExtension = (PUDF_VOLUME_EXTENSION)VolumeDeviceObject->DeviceExtension;

    //
    // See if we can recognize a UDF volume in the device.
    //

    status = UdfxRecognizeVolume(VolumeExtension);

    if (!NT_SUCCESS(status)) {
        goto CleanupAndExit;
    }

    //
    // Process the anchor volume descriptor for the volume.
    //

    status = UdfxProcessAnchorVolumeDescriptor(VolumeExtension,
        &MainVolumeDescriptorSequenceExtent);

    if (!NT_SUCCESS(status)) {
        goto CleanupAndExit;
    }

    //
    // Process the volume data structures for the volume.
    //

    status = UdfxProcessVolumeDescriptorSequence(VolumeExtension,
        &MainVolumeDescriptorSequenceExtent, &FileSetDescriptorExtent);

    if (!NT_SUCCESS(status)) {
        goto CleanupAndExit;
    }

    //
    // Process the file set descriptor for the volume.
    //

    status = UdfxProcessFileSetDescriptor(VolumeExtension,
        &FileSetDescriptorExtent, &IcbRoot);

    if (!NT_SUCCESS(status)) {
        goto CleanupAndExit;
    }

    //
    // Create the file control block for the root directory.
    //

    status = UdfxCreateFcbFromFileEntry(VolumeExtension, Irp, &IcbRoot, NULL,
        NULL, &VolumeExtension->RootDirectoryFcb);

    if (!NT_SUCCESS(status)) {
        goto CleanupAndExit;
    }

    VolumeExtension->RootDirectoryFcb->Flags |= UDF_FCB_ROOT_DIRECTORY;

    //
    // Connect the volume device object to the target device object and return
    // success.
    //

    TargetDeviceObject->MountedOrSelfDevice = VolumeDeviceObject;
    VolumeDeviceObject = NULL;

    status = STATUS_SUCCESS;

CleanupAndExit:
    if (VolumeDeviceObject != NULL) {
        UdfxDeleteVolumeDevice(VolumeDeviceObject);
    }

    if (Irp != NULL) {
        IoFreeIrp(Irp);
    }

    UdfxReleaseGlobalMutex();

    return status;
}
