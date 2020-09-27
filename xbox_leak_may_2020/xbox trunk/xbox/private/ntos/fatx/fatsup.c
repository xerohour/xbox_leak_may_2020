/*++

Copyright (c) 2000-2001  Microsoft Corporation

Module Name:

    fatsup.c

Abstract:

    This module implements FAT file system utility routines.

--*/

#include "fatx.h"

//
// A condensed table of legal fat character values
//
const ULONG FatxFatIllegalTable[] = {
    0xffffffff,
    0xfc009c04,
    0x10000000,
    0x10000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
};

BOOLEAN
FatxIsValidFatFileName(
    IN POBJECT_STRING FileName
    )
/*++

Routine Description:

    This routine checks if the supplied file name is a legal FAT file name

Arguments:

    FileName - Specifies the buffer to check for validity.

Return Value:

    Returns TRUE if the string is valid, else FALSE.

--*/
{
    POSTR Buffer;
    POSTR EndingBuffer;
    OCHAR Char;

    //
    // If the file name is zero or too long, then the name is not valid.
    //

    if ((FileName->Length == 0) || (FileName->Length > FAT_FILE_NAME_LENGTH)) {
        return FALSE;
    }

    //
    // Perform some preliminary checks if the name begins with a period.
    //

    Buffer = FileName->Buffer;
    EndingBuffer = Buffer + FileName->Length / sizeof(OCHAR);

    if (*Buffer == '.') {

        //
        // If the name is "." or "..", then the name is not valid.
        //

        if ((FileName->Length == sizeof(OCHAR)) ||
            (FileName->Length == sizeof(OCHAR) * 2) && (*(Buffer + 1) == '.')) {
            return FALSE;
        }
    }

    //
    // Process the rest of the file name.
    //

    while (Buffer < EndingBuffer) {

        Char = *Buffer++;

        //
        // Check for illegal FAT characters.
        //

        if (FatxFatIllegalTable[Char >> 5] & (1 << (Char & 31))) {
            return FALSE;
        }
    }

    return TRUE;
}

LARGE_INTEGER
FatxFatTimestampToTime(
    IN PFAT_TIME_STAMP FatTimestamp
    )
/*++

Routine Description:

    This routine converts a FAT timestamp to a NT system time.

Arguments:

    FatTimestamp - Specifies the timestamp to convert to NT system time.

Return Value:

    Conversion to NT system time.

--*/
{
    LARGE_INTEGER SystemTime;
    TIME_FIELDS TimeField;

    TimeField.Year = (USHORT)(FatTimestamp->Year + 2000);
    TimeField.Month = FatTimestamp->Month;
    TimeField.Day = FatTimestamp->Day;
    TimeField.Hour = FatTimestamp->Hour;
    TimeField.Minute = FatTimestamp->Minute;
    TimeField.Second = (USHORT)(FatTimestamp->DoubleSeconds * 2);
    TimeField.Milliseconds = 0;

    if (!RtlTimeFieldsToTime(&TimeField, &SystemTime)) {
        SystemTime.QuadPart = 0;
    }

    return SystemTime;
}

LOGICAL
FatxTimeToFatTimestamp(
    IN PLARGE_INTEGER Time,
    OUT PFAT_TIME_STAMP FatTimestamp
    )
/*++

Routine Description:

    This routine converts a NT system time to a FAT timestamp.

Arguments:

    Time - Specifies the NT system time.

    FatTimestamp - Specifies the buffer to receive the FAT timestamp.

Return Value:

    Returns TRUE if the time could be converted to a valid FAT time, else FALSE.

--*/
{

    LARGE_INTEGER LocalTime;
    TIME_FIELDS TimeField;

    //
    // Round the time up to the nearest double second.
    //

    LocalTime.QuadPart = Time->QuadPart + (2 * 1000 * 1000 * 10 - 1);

    RtlTimeToTimeFields(&LocalTime, &TimeField);

    FatTimestamp->Year = (USHORT)(TimeField.Year - 2000);
    FatTimestamp->Month = TimeField.Month;
    FatTimestamp->Day = TimeField.Day;
    FatTimestamp->Hour = TimeField.Hour;
    FatTimestamp->Minute = TimeField.Minute;
    FatTimestamp->DoubleSeconds = (USHORT)(TimeField.Second >> 1);

    return (TimeField.Year >= 2000) && (TimeField.Year <= 2000 + 127);
}

LARGE_INTEGER
FatxRoundToFatTime(
    IN PLARGE_INTEGER Time
    )
/*++

Routine Description:

    This routine rounds a NT system time to FAT granularity (two seconds).

Arguments:

    Time - Specifies the NT system time.

Return Value:

    Rounded NT system time.

--*/
{
    FAT_TIME_STAMP FatTimestamp;

    //
    // Convert the time to a FAT timestamp and back.
    //

    if ((Time->QuadPart != 0) && FatxTimeToFatTimestamp(Time, &FatTimestamp)) {
        return FatxFatTimestampToTime(&FatTimestamp);
    } else {
        return *Time;
    }
}
