/*++

Copyright (c) 2000-2001  Microsoft Corporation

Module Name:

    udfsup.c

Abstract:

    This module implements UDF file system utility routines.

--*/

#include "udfx.h"

BOOLEAN
UdfxVerifyDescriptor(
    IN PDESTAG Descriptor,
    IN ULONG DescriptorSize,
    IN USHORT Ident,
    IN ULONG LogicalSectorNumber
    )
/*++

Routine Description:

    This routine checks if the supplied descriptor tag is structurally correct.

    Refer to ISO 13346 section 3/7.2.

Arguments:

    Descriptor - Specifies the descriptor tag to verify.

    DescriptorSize - Specifies the size in bytes of the descriptor.

    Ident - Specifies the expected identifier of the tag.

    LogicalSectorNumber - Specifies the expected sector number of the tag.

Return Value:

    Returns TRUE if the descriptor is structurally correct, else FALSE.

--*/
{
    if ((Descriptor->Version == DESTAG_VER_CURRENT) &&
        (Descriptor->Ident == Ident) &&
        (Descriptor->Lbn == LogicalSectorNumber)) {
        return TRUE;
    } else {
        return FALSE;
    }
}

BOOLEAN
UdfxEqualOSTACS0StringAndObjectString(
    IN PUCHAR OSTACS0String,
    IN SIZE_T OSTACS0StringLength,
    IN POBJECT_STRING ObjectString
    )
/*++

Routine Description:

    This routine does a case insensitive comparision of a OSTA CS0 string to an
    object manager string.

Arguments:

    OSTACS0String - Specifies the OSTA CS0 string to compare.

    OSTACS0StringLength - Specifies the length of the OSTA CS0 string.

    ObjectString - Specifies the object manager string to compare.

Return Value:

    Returns TRUE if the strings are equal, else FALSE.

--*/
{
    POSTR ObjectBuffer;
    OCHAR c1;
    OCHAR c2;

    //
    // The first byte of an OSTA Compressed Unicode string is the compression
    // format, which is either going to be 8-bits per character or 16-bits per
    // character.  All other values are reserved and we'll treat as a failed
    // compare.
    //

    if (*OSTACS0String != 8) {
        return FALSE;
    }

    //
    // Advance past the compression code.
    //

    OSTACS0String++;
    OSTACS0StringLength--;

    //
    // Check if the lengths of the strings match.
    //

    if (ObjectString->Length != OSTACS0StringLength * sizeof(OCHAR)) {
        return FALSE;
    }

    ObjectBuffer = ObjectString->Buffer;

    while (OSTACS0StringLength) {

        c1 = *ObjectBuffer++;
        c2 = (OCHAR)*OSTACS0String++;

        if ((c1 != c2) && (RtlUpperChar(c1) != RtlUpperChar(c2))) {
            return FALSE;
        }

        OSTACS0StringLength -= sizeof(CHAR);
    }

    return TRUE;
}

NTSTATUS
UdfxOSTACS0StringToObjectString(
    IN PUCHAR OSTACS0String,
    IN SIZE_T OSTACS0StringLength,
    OUT POBJECT_STRING ObjectString
    )
/*++

Routine Description:

    This routine converts a OSTA CS0 string to an object manager string.

Arguments:

    OSTACS0String - Specifies the source string to convert.

    OSTACS0StringLength - Specifies the length of the source string.

    ObjectString - Specifies the buffer to receive the converted object manager
        string.

Return Value:

    Status of operation.

--*/
{
    POSTR ObjectBuffer;

    ObjectString->Buffer = NULL;

    if (OSTACS0StringLength != 0) {

        //
        // The first byte of an OSTA Compressed Unicode string is the compression
        // format, which is either going to be 8-bits per character or 16-bits
        // per character.
        //

        if (*OSTACS0String == 8) {

            //
            // Advance past the compression code.
            //

            OSTACS0String++;
            OSTACS0StringLength--;

            //
            // Allocate a buffer to receive the object manager string.
            //

            ObjectBuffer = ExAllocatePoolWithTag(OSTACS0StringLength *
                sizeof(OCHAR), 'gSxU');

            if (ObjectBuffer == NULL) {
                return STATUS_INSUFFICIENT_RESOURCES;
            }

            ObjectString->Buffer = ObjectBuffer;
            ObjectString->Length = (USHORT)(OSTACS0StringLength * sizeof(OCHAR));
            ObjectString->MaximumLength = ObjectString->Length;

            //
            // Fill in the output buffer.
            //

            RtlCopyMemory(ObjectBuffer, OSTACS0String, OSTACS0StringLength);

            return STATUS_SUCCESS;
        }
    }

    return STATUS_INVALID_PARAMETER;
}

LARGE_INTEGER
UdfxUdfTimestampToTime(
    IN PTIMESTAMP UdfTimestamp
    )
/*++

Routine Description:

    This routine converts a UDF timestamp to a NT system time.

Arguments:

    UdfTimestamp - Specifies the timestamp to convert to NT system time.

Return Value:

    Conversion to NT system time.

--*/
{
    LARGE_INTEGER SystemTime;
    TIME_FIELDS TimeField;

    TimeField.Year = UdfTimestamp->Year;
    TimeField.Month = UdfTimestamp->Month;
    TimeField.Day = UdfTimestamp->Day;
    TimeField.Hour = UdfTimestamp->Hour;
    TimeField.Minute = UdfTimestamp->Minute;
    TimeField.Second = UdfTimestamp->Second;
    TimeField.Milliseconds = 0;

    //
    // Validate the timestamp is valid and convert it to a system time.
    //

    if ((UdfTimestamp->Type <= TIMESTAMP_T_LOCAL) &&
        ((UdfTimestamp->Zone >= TIMESTAMP_Z_MIN && UdfTimestamp->Zone <= TIMESTAMP_Z_MAX) ||
            (UdfTimestamp->Zone == TIMESTAMP_Z_NONE)) &&
        RtlTimeFieldsToTime(&TimeField, &SystemTime)) {

        //
        // If necessary, convert to coordinated universal time.
        //

        if ((UdfTimestamp->Type == TIMESTAMP_T_LOCAL) &&
            (UdfTimestamp->Zone != TIMESTAMP_Z_NONE)) {
            SystemTime.QuadPart -= (ULONGLONG)UdfTimestamp->Zone * 60 * 10 * 1000 * 1000;
        }

    } else {

        SystemTime.QuadPart = 0;
    }

    return SystemTime;
}

NTSTATUS
UdfxMapLogicalSector(
    IN PUDF_VOLUME_EXTENSION VolumeExtension,
    IN PIRP Irp,
    IN ULONG LogicalSectorNumber,
    OUT PVOID *CacheBuffer
    )
/*++

Routine Description:

    This routine maps the supplied logical sector number into the file system
    cache.

Arguments:

    VolumeExtension - Specifies the extension that the I/O request is for.

    Irp - Specifies the packet that describes the I/O request.

    LogicalSectorNumber - Specifies the number of the logical sector to map into
        the cache.

    CacheBuffer - Specifies the buffer to receive the address of the mapped
        cache buffer.

Return Value:

    Status of operation.

--*/
{
    ULONGLONG ByteOffset;

    //
    // Verify that the logical sector resides inside the physical partition.
    //

    if (LogicalSectorNumber >= VolumeExtension->PartitionSectorCount) {
        return STATUS_DISK_CORRUPT_ERROR;
    }

    //
    // Convert the logical sector to the disk offset.
    //

    ByteOffset = ((ULONGLONG)VolumeExtension->PartitionSectorStart +
        LogicalSectorNumber) << UDF_CD_SECTOR_SHIFT;

    //
    // Map the directory sector into the cache.
    //

    return FscMapBuffer(&VolumeExtension->CacheExtension, Irp, ByteOffset,
        FALSE, CacheBuffer);
}
