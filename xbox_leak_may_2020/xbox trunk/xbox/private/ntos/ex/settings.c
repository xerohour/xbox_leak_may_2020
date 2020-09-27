/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    settings.c

Abstract:

    Functions for querying or saving non-volatile settings

--*/

#include "exp.h"

#include <windef.h>
#include <winbase.h>
#include <xbox.h>
#include "xboxp.h"
#include "xconfig.h"

//
// Guards against multiple threads accessing settings at the same time
//
INITIALIZED_CRITICAL_SECTION(ExpNonVolatileSettingsLock);

//
// SMBus slave addresses for reading/writing EEPROM
//
#define EEPROM_SMBUS_WRITE  0xA8
#define EEPROM_SMBUS_READ   0xA9
#define EEPROM_RW_BLOCK     2

//
// EEPROM shadow in RAM
//
DECLSPEC_STICKY UCHAR EEPROMShadow[EEPROM_TOTAL_MEMORY_SIZE];
DECLSPEC_STICKY ULONG EEPROMShadowIsValid;


NTSTATUS
ExpReadEEPROMIntoCache()

/*++

Routine Description:

    Read the entire content of the EEPROM into the RAM buffer

Arguments:

    NONE

Return Value:

    Status code

--*/

{
    USHORT* p = (USHORT*) EEPROMShadow;
    ULONG Offset;
    NTSTATUS Status;

    ASSERT(EEPROM_RW_BLOCK == sizeof(*p));

    // Read data out of the EEPROM, 2 bytes at a time
    for (Offset=0; Offset < EEPROM_TOTAL_MEMORY_SIZE; Offset += EEPROM_RW_BLOCK) {
        ULONG val;
        Status = HalReadSMBusWord(EEPROM_SMBUS_READ, (UCHAR) Offset, &val);
        if (!NT_SUCCESS(Status)) {
            KdPrint(("EX: failed to read EEPROM offset %02x, status=%08x\n", Offset, Status));
            return Status;
        }
        *p++ = (USHORT) val;
    }

    // Check to see if the EEPROM is empty (i.e. all 1's)
    p = (USHORT*) EEPROMShadow;
    for (Offset=0; Offset < EEPROM_TOTAL_MEMORY_SIZE/EEPROM_RW_BLOCK; Offset++) {
        if (p[Offset] != 0xffff)
            return STATUS_SUCCESS;
    }

    KdPrint(("Uninitialized EEPROM data!!!\n"));
    return STATUS_INVALID_DEVICE_STATE;
}


NTSTATUS
ExpReadEEPROMSection(
    ULONG Offset,
    ULONG Count,
    VOID* Buffer,
    BOOLEAN DoChecksum
    )

/*++

Routine Description:

    Read data from the specified section of the EEPROM

Arguments:

    Offset - Starting byte offset of the specified EEPROM section
    Count - Number of bytes in the section
    Buffer - Buffer for returning the data

Return Value:

    Status code

--*/

{
    // Make sure we have a cached copy of the EEPROM content in RAM
    if (!EEPROMShadowIsValid) {
        NTSTATUS Status = ExpReadEEPROMIntoCache();
        if (!NT_SUCCESS(Status)) return Status;

        EEPROMShadowIsValid = TRUE;
    }

    memcpy(Buffer, &EEPROMShadow[Offset], Count);

    // Verify the section checksum if requested
    if (DoChecksum && XConfigChecksum(Buffer, Count) != 0xffffffff) {
        KdPrint(("Corrupted EEPROM data!!!\n"));
        return STATUS_DEVICE_DATA_ERROR;
    }

    return STATUS_SUCCESS;
}


NTSTATUS
ExpWriteEEPROM(
    ULONG Offset,
    ULONG Count,
    const VOID* Buffer
    )

/*++

Routine Description:

    Write data to EEPROM

Arguments:

    Offset - EEPROM byte offset to start writing at
    Count - Number of bytes to write
    Buffer - Points to the data to be written to the EEPROM

Return Value:

    Status code

--*/

{
    const USHORT* p = (const USHORT*) Buffer;
    NTSTATUS Status;

    ASSERT(EEPROM_RW_BLOCK == sizeof(*p));
    ASSERT(Count % EEPROM_RW_BLOCK == 0);

    // Update the cached content as well
    if (EEPROMShadowIsValid) {
        memcpy(&EEPROMShadow[Offset], Buffer, Count);
    }

    // Write data into the EEPROM, 2 bytes at a time
    for (Count += Offset; Offset < Count; Offset += EEPROM_RW_BLOCK) {
        ULONG val = *p++;
        Status = HalWriteSMBusWord(EEPROM_SMBUS_WRITE, (UCHAR) Offset, val);
        if (!NT_SUCCESS(Status)) {
            KdPrint(("EX: failed to write EEPROM offset %02x, status=%08x\n", Offset, Status));
            // If the write fails, marked the cached data as invalid
            EEPROMShadowIsValid = FALSE;
            return Status;
        }
    }

    return STATUS_SUCCESS;
}


//
// Information about a non-volatile setting
//
typedef struct _SETTING_ENTRY_INFO {
    UCHAR Type;
    UCHAR Index;
    UCHAR Size;
    UCHAR Offset;
} SETTING_ENTRY_INFO;

//
// Values in the factory settings section of the EEPROM
//
#define FACTORY_SETTING_ENTRY(type, index, field) { \
            type, \
            index - XC_FACTORY_START_INDEX, \
            FIELD_SIZE(XBOX_FACTORY_SETTINGS, field), \
            FIELD_OFFSET(XBOX_FACTORY_SETTINGS, field) \
        }

const SETTING_ENTRY_INFO FactorySettingsInfo[] = {
    FACTORY_SETTING_ENTRY(REG_BINARY, XC_FACTORY_SERIAL_NUMBER, SerialNumber),
    FACTORY_SETTING_ENTRY(REG_BINARY, XC_FACTORY_ETHERNET_ADDR, EthernetAddr),
    FACTORY_SETTING_ENTRY(REG_BINARY, XC_FACTORY_ONLINE_KEY, OnlineKey),
    FACTORY_SETTING_ENTRY(REG_DWORD, XC_FACTORY_AV_REGION, AVRegion),

    { REG_BINARY, XC_MAX_FACTORY-XC_FACTORY_START_INDEX, EEPROM_FACTORY_SECTION_SIZE, 0 }
};

//
// Values in the user settings section of the EEPROM
//
#define USER_SETTING_ENTRY(type, index, field) { \
            type, \
            index, \
            FIELD_SIZE(XBOX_USER_SETTINGS, field), \
            FIELD_OFFSET(XBOX_USER_SETTINGS, field) \
        }

const SETTING_ENTRY_INFO UserSettingsInfo[] = {
    USER_SETTING_ENTRY(REG_DWORD, XC_TIMEZONE_BIAS, TimeZoneBias),
    USER_SETTING_ENTRY(REG_BINARY, XC_TZ_STD_NAME, TimeZoneStdName),
    USER_SETTING_ENTRY(REG_BINARY, XC_TZ_STD_DATE, TimeZoneStdDate),
    USER_SETTING_ENTRY(REG_DWORD, XC_TZ_STD_BIAS, TimeZoneStdBias),
    USER_SETTING_ENTRY(REG_BINARY, XC_TZ_DLT_NAME, TimeZoneDltName),
    USER_SETTING_ENTRY(REG_BINARY, XC_TZ_DLT_DATE, TimeZoneDltDate),
    USER_SETTING_ENTRY(REG_DWORD, XC_TZ_DLT_BIAS, TimeZoneDltBias),
    USER_SETTING_ENTRY(REG_DWORD, XC_LANGUAGE, Language),
    USER_SETTING_ENTRY(REG_DWORD, XC_VIDEO_FLAGS, VideoFlags),
    USER_SETTING_ENTRY(REG_DWORD, XC_AUDIO_FLAGS, AudioFlags),
    USER_SETTING_ENTRY(REG_DWORD, XC_PARENTAL_CONTROL_GAMES, ParentalControlGames),
    USER_SETTING_ENTRY(REG_DWORD, XC_PARENTAL_CONTROL_PASSWORD, ParentalControlPassword),
    USER_SETTING_ENTRY(REG_DWORD, XC_PARENTAL_CONTROL_MOVIES, ParentalControlMovies),
    USER_SETTING_ENTRY(REG_DWORD, XC_ONLINE_IP_ADDRESS, OnlineIpAddress),
    USER_SETTING_ENTRY(REG_DWORD, XC_ONLINE_DNS_ADDRESS, OnlineDnsAddress),
    USER_SETTING_ENTRY(REG_DWORD, XC_ONLINE_DEFAULT_GATEWAY_ADDRESS, OnlineDefaultGatewayAddress),
    USER_SETTING_ENTRY(REG_DWORD, XC_ONLINE_SUBNET_ADDRESS, OnlineSubnetMask),
    USER_SETTING_ENTRY(REG_DWORD, XC_MISC_FLAGS, MiscFlags),
    USER_SETTING_ENTRY(REG_DWORD, XC_DVD_REGION, DvdRegion),

    { REG_BINARY, XC_MAX_OS, EEPROM_USER_SECTION_SIZE, 0 }
};


const SETTING_ENTRY_INFO*
ExpFindSettingInfo(
    const SETTING_ENTRY_INFO* Table,
    ULONG Count,
    ULONG Index
    )

/*++

Routine Description:

    Find information about a specified non-volatile setting

Arguments:

    Table - Points to a table that contains information about
        all non-volatile settings in one section
    Count - Number of entries in the table
    Index - Specifies which setting is interested interested in

Return Value:

    Returns a pointer to the information about the specified setting
    NULL if the specified setting is not supported

--*/

{
    const SETTING_ENTRY_INFO* EntryInfo;
    
    for (EntryInfo=Table; Count--; EntryInfo++) 
        if (EntryInfo->Index == Index) return EntryInfo;

    return NULL;
}


NTSTATUS
ExQueryNonVolatileSetting(
    IN ULONG ValueIndex,
    OUT ULONG* Type,
    OUT VOID* Value,
    IN ULONG ValueLength,
    OUT ULONG* ResultLength
    )

/*++

Routine Description:

    Query a non-volatile factory or user setting

Arguments:

    ValueIndex - Specifies which non-volatile setting to query
    Type - Return the value type
    Value - Output buffer for storing the result
    ValueLength - Specifies the output buffer size
    ResultLength - Returns the actually amount of data returned to the caller

Return Value:

    Status code

--*/

{
    NTSTATUS Status;
    ULONG SectionOffset, SectionSize;
    const SETTING_ENTRY_INFO* EntryInfo;
    UCHAR buf[EEPROM_TOTAL_MEMORY_SIZE];

    //
    // Sanity checks for the EEPROM layout
    //
    ASSERT(EEPROM_ENCRYPTED_SECTION_SIZE % EEPROM_PAGE_ALIGNMENT == 0 &&
           EEPROM_FACTORY_SECTION_SIZE % EEPROM_PAGE_ALIGNMENT == 0 &&
           EEPROM_ENCRYPTED_SECTION_SIZE +
           EEPROM_FACTORY_SECTION_SIZE +
           EEPROM_USER_SECTION_SIZE <= EEPROM_TOTAL_MEMORY_SIZE);

    ASSERT(Type != NULL && Value != NULL);

    //
    // Special case for game region value
    //
    if (ValueIndex == XC_FACTORY_GAME_REGION) {
        if (ResultLength)
            *ResultLength = sizeof(ULONG);
        if (ValueLength < sizeof(ULONG))
            return STATUS_BUFFER_TOO_SMALL;

        *Type = REG_DWORD;
        *((ULONG*) Value) = XboxGameRegion;
        return STATUS_SUCCESS;
    }

    //
    // Another special case to read the entire EEPROM content at once
    // or to read the encrypted data section
    //
    if (ValueIndex == XC_MAX_ALL || ValueIndex == XC_ENCRYPTED_SECTION) {
        SectionSize = (ValueIndex == XC_MAX_ALL) ?
                        EEPROM_TOTAL_MEMORY_SIZE :
                        EEPROM_ENCRYPTED_SECTION_SIZE;

        if (ResultLength)
            *ResultLength = SectionSize;
        if (ValueLength < SectionSize)
            return STATUS_BUFFER_TOO_SMALL;

        *Type = REG_BINARY;

        RtlEnterCriticalSectionAndRegion(&ExpNonVolatileSettingsLock);
        Status = ExpReadEEPROMSection(0, SectionSize, Value, FALSE);
        RtlLeaveCriticalSectionAndRegion(&ExpNonVolatileSettingsLock);
        return Status;
    }
    
    //
    // Figure out which setting is being queried
    //
    if (ValueIndex <= XC_MAX_OS) {
        //
        // User settings section
        //
        SectionOffset = EEPROM_USER_SECTION_START;
        SectionSize = EEPROM_USER_SECTION_SIZE;
        EntryInfo = ExpFindSettingInfo(
                        UserSettingsInfo,
                        sizeof(UserSettingsInfo) / sizeof(UserSettingsInfo[0]),
                        ValueIndex);
    } else if (ValueIndex <= XC_MAX_FACTORY) {
        //
        // Factory settings section
        //
        SectionOffset = EEPROM_FACTORY_SECTION_START;
        SectionSize = EEPROM_FACTORY_SECTION_SIZE;
        EntryInfo = ExpFindSettingInfo(
                        FactorySettingsInfo,
                        sizeof(FactorySettingsInfo) / sizeof(FactorySettingsInfo[0]),
                        ValueIndex - XC_FACTORY_START_INDEX);
    } else {
        EntryInfo = NULL;
    }

    //
    // The requested value doesn't exist
    //
    if (EntryInfo == NULL)
        return STATUS_OBJECT_NAME_NOT_FOUND;

    //
    // Now read the relevant section from the EEPROM
    //
    RtlEnterCriticalSectionAndRegion(&ExpNonVolatileSettingsLock);
    Status = ExpReadEEPROMSection(SectionOffset, SectionSize, buf, TRUE);
    RtlLeaveCriticalSectionAndRegion(&ExpNonVolatileSettingsLock);

    if (!NT_SUCCESS(Status))
        return Status;

    //
    // Is the supplied output buffer large enough
    //
    if (ValueLength < EntryInfo->Size)
        return STATUS_BUFFER_TOO_SMALL;

    //
    // Now copy the value to the output buffer and return success.
    // If the output buffer is larger than necessary, unused portion is zero-filled.
    //
    *Type = EntryInfo->Type;
    if (ResultLength)
        *ResultLength = EntryInfo->Size;

    memset(Value, 0, ValueLength);
    memcpy(Value, &buf[EntryInfo->Offset], EntryInfo->Size);
    return STATUS_SUCCESS;
}


NTSTATUS
ExSaveNonVolatileSetting(
    IN ULONG ValueIndex,
    IN ULONG Type,
    IN const VOID* Value,
    IN ULONG ValueLength
    )

/*++

Routine Description:

    Save a non-volatile user-setting

Arguments:

    ValueIndex - Specifies which non-volatile setting to save
    Type - Specifies the value type
    Value - Points to the value to be saved
    ValueLength - Size of the data

Return Value:

    Status code

--*/

{
    NTSTATUS Status;
    ULONG SectionOffset, SectionSize;
    const SETTING_ENTRY_INFO* EntryInfo;
    UCHAR OldSettings[EEPROM_TOTAL_MEMORY_SIZE];
    UCHAR NewSettings[EEPROM_TOTAL_MEMORY_SIZE];
    BOOLEAN SmartWrite = TRUE;
    UCHAR* buf;

    ASSERT(Value != NULL);

    if (ValueIndex == XC_MAX_ALL) {
        //
        // Special case to write the entire EEPROM content at once
        //
        if (ValueLength != EEPROM_TOTAL_MEMORY_SIZE)
            return STATUS_INVALID_PARAMETER;

        RtlEnterCriticalSectionAndRegion(&ExpNonVolatileSettingsLock);

        //
        // We try to be smart about the write.  If we can fetch all of the old
        // settings, we'll use the smart write code.  Otherwise, we'll
        // just force the complete write.
        //
        SectionOffset = 0;
        SectionSize = EEPROM_TOTAL_MEMORY_SIZE;
        memcpy(NewSettings, Value, SectionSize);
        Status = ExpReadEEPROMSection(SectionOffset, SectionSize, OldSettings,
            FALSE);
        if(!NT_SUCCESS(Status))
            SmartWrite = FALSE;
        goto DoSmartWrite;
    } else if (ValueIndex <= XC_MAX_OS) {
        //
        // User settings section
        //
        SectionOffset = EEPROM_USER_SECTION_START;
        SectionSize = EEPROM_USER_SECTION_SIZE;
        EntryInfo = ExpFindSettingInfo(
                        UserSettingsInfo,
                        sizeof(UserSettingsInfo) / sizeof(UserSettingsInfo[0]),
                        ValueIndex);
#ifdef DEVKIT
    } else if (ValueIndex == XC_FACTORY_GAME_REGION) {
        //
        // Special case to update the game region
        //
        if (ValueLength != FIELD_SIZE(XBOX_ENCRYPTED_SETTINGS, GameRegion))
            return STATUS_INVALID_PARAMETER;

        RtlEnterCriticalSectionAndRegion(&ExpNonVolatileSettingsLock);
        
        Status = ExpWriteEEPROM(
                    EEPROM_ENCRYPTED_SECTION_START +
                        FIELD_OFFSET(XBOX_ENCRYPTED_SETTINGS, GameRegion),
                    ValueLength,
                    Value);

        if (NT_SUCCESS(Status))
            XboxGameRegion = *((ULONG*) Value);

        RtlEnterCriticalSectionAndRegion(&ExpNonVolatileSettingsLock);
        return Status;
    } else if (ValueIndex <= XC_MAX_FACTORY) {
        //
        // Factory settings section
        //
        SectionOffset = EEPROM_FACTORY_SECTION_START;
        SectionSize = EEPROM_FACTORY_SECTION_SIZE;
        EntryInfo = ExpFindSettingInfo(
                        FactorySettingsInfo,
                        sizeof(FactorySettingsInfo) / sizeof(FactorySettingsInfo[0]),
                        ValueIndex - XC_FACTORY_START_INDEX);
#endif // DEVKIT
    } else {
        EntryInfo = NULL;
    }

    if (EntryInfo == NULL)
        return STATUS_OBJECT_NAME_NOT_FOUND;

    // NOTE: If necessary in the future, we could potentially store
    // a REG_DWORD value in a smaller field (e.g. UCHAR or USHORT),
    // as long as the top bits of the value to be stored are all 0's.
    if (ValueLength > EntryInfo->Size)
        return STATUS_INVALID_PARAMETER;

    RtlEnterCriticalSectionAndRegion(&ExpNonVolatileSettingsLock);

    //
    // Read the entire user configuration section of the EEPROM
    //
    Status = ExpReadEEPROMSection(SectionOffset, SectionSize, OldSettings, TRUE);

    if (!NT_SUCCESS(Status)) {
#ifndef DEVKIT
        //
        // If we're not just changing an entry in the user section, and the
        // EEPROM is corrupt, we don't want to make things worse by changing
        // it some more, so we'll just fail out.  If this is an entry from the
        // user section, we'll just restore the user portion of the EEPROM to
        // default (ie., zero) settings.
        //
        if(SectionOffset != EEPROM_USER_SECTION_START) {
            RtlLeaveCriticalSectionAndRegion(&ExpNonVolatileSettingsLock);
            return Status;
        }
#endif

        // Reset the configuration section with default values.
        memset(NewSettings, 0, SectionSize);
        SmartWrite = FALSE;
    } else {
        memcpy(NewSettings, OldSettings, SectionSize);
    }

    //
    // Update the specified field
    //
    buf = &(NewSettings[EntryInfo->Offset]);
    memset(buf, 0, EntryInfo->Size);
    memcpy(buf, Value, ValueLength);

    //
    // Compute the new checksum
    //  We use the 32-bit version of the TCP/IP checksum algorithm, i.e.
    //  1's complement of the 1's complement sum of the whole data structure.
    //  The checksum field itself is 0 when the checksum is computed.
    //

    //
    // NOTE: Checksum MUST be stored as the first ULONG of the settings structure
    //
    
    *((PULONG) NewSettings) = 0;
    *((PULONG) NewSettings) = ~XConfigChecksum(NewSettings, SectionSize);

    //
    // Write out the data to EEPROM
    //
DoSmartWrite:
    if (SmartWrite) {
        UCHAR* oldbuf = OldSettings;
        UCHAR* newbuf = NewSettings;
        ULONG Offset = 0;

        Status = STATUS_SUCCESS;
        while (Offset < SectionSize) {
            if (memcmp(oldbuf+Offset, newbuf+Offset, EEPROM_RW_BLOCK) != 0) {
                Status = ExpWriteEEPROM(SectionOffset+Offset, EEPROM_RW_BLOCK, newbuf+Offset);
                if (!NT_SUCCESS(Status)) break;
            }
            Offset += EEPROM_RW_BLOCK;
        }

    } else {
        Status = ExpWriteEEPROM(SectionOffset, SectionSize, NewSettings);
    }

    RtlLeaveCriticalSectionAndRegion(&ExpNonVolatileSettingsLock);

    return Status;
}


NTSTATUS
ExReadWriteRefurbInfo(
    OUT XBOX_REFURB_INFO* RefurbInfo,
    IN ULONG ValueLength,
    IN BOOLEAN DoWrite
    )

/*++

Routine Description:

    Read or write refurb information

Arguments:

    RefurbInfo - Pointer to the data buffer
    ValueLength - Size of the data buffer
    DoWrite - TRUE for writing and FALSE for reading

Return Value:

    Status code

--*/

{
    OBJECT_STRING objname;
    OBJECT_ATTRIBUTES oa;
    IO_STATUS_BLOCK iosb;
    NTSTATUS status;
    HANDLE disk;

    ASSERT(RefurbInfo != NULL);
    if (ValueLength != sizeof(XBOX_REFURB_INFO))
        return STATUS_INVALID_PARAMETER;

    //
    // Open disk partition 0 for raw r/w access
    //
    RtlInitObjectString(&objname, OTEXT("\\Device\\Harddisk0\\partition0"));
    InitializeObjectAttributes(&oa, &objname, OBJ_CASE_INSENSITIVE, NULL, NULL);
    status = NtOpenFile(
                &disk,
                GENERIC_READ | (DoWrite ? GENERIC_WRITE : 0) | SYNCHRONIZE,
                &oa,
                &iosb,
                FILE_SHARE_READ|FILE_SHARE_WRITE,
                FILE_SYNCHRONOUS_IO_ALERT);

    if (NT_SUCCESS(status)) {
        LARGE_INTEGER offset;
        CHAR buf[XBOX_HD_SECTOR_SIZE];
        XBOX_REFURB_INFO* info = (XBOX_REFURB_INFO*) buf;

        //
        // Refurb information is stored in sector 3
        // 
        ASSERT(sizeof(XBOX_REFURB_INFO) <= XBOX_HD_SECTOR_SIZE);
        memset(buf, 0, sizeof(buf));
        offset.QuadPart = XBOX_REFURB_INFO_SECTOR_INDEX * XBOX_HD_SECTOR_SIZE;

        if (DoWrite) {
            memcpy(info, RefurbInfo, sizeof(XBOX_REFURB_INFO));
            info->Signature = XBOX_REFURB_INFO_SIGNATURE;
            status = NtWriteFile(disk, 0, NULL, NULL, &iosb, info, XBOX_HD_SECTOR_SIZE, &offset);
        } else {
            status = NtReadFile(disk, 0, NULL, NULL, &iosb, info, XBOX_HD_SECTOR_SIZE, &offset);
            if (NT_SUCCESS(status)) {
                if (info->Signature != XBOX_REFURB_INFO_SIGNATURE) {
                    //
                    // Invalid signature - assume everything is uninitialized
                    //
                    memset(RefurbInfo, 0, sizeof(XBOX_REFURB_INFO));
                } else {
                    memcpy(RefurbInfo, info, sizeof(XBOX_REFURB_INFO));
                }
            }
        }

        NtClose(disk);
    }

    return status;
}

