/*++

Copyright (c) 1989-2002  Microsoft Corporation

Module Name:

    init.c

Abstract:

    Main source file the NTOS system initialization subcomponent.

--*/

#include "ntos.h"
#include <ntimage.h>
#include <ntddcdrm.h>
#include <ntdddisk.h>
#include <ntverp.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ldr.h>
#include <xbeimage.h>
#include <dm.h>
#include <xlaunch.h>
#include <ani.h>
#include <xprofp.h>
#include <bldr.h>
#include <rc4.h>
#include <xcrypt.h>
#include <smcdef.h>

#include <wtypes.h>
#include <xconfig.h>
#include <xbox.h>
#include <xboxp.h>

VOID
IdexChannelCreate(
    VOID
    );

#ifdef DEVKIT

VOID
ExpTryToBootMediaROM(
    VOID
    );

#endif

VOID
ExpDecryptEEPROM(
    VOID
    );

ULONG
ExpDetectSettingsError(
    VOID
    );

VOID
ExpCopyLastUEMError(
    VOID
    );

#ifdef ALLOC_PRAGMA
#pragma alloc_text(INIT, ExpDecryptEEPROM)
#pragma alloc_text(INIT, ExpCopyLastUEMError)
#pragma alloc_text(INIT, ExpDetectSettingsError)
#ifdef DEVKIT
#pragma alloc_text(INIT, ExpTryToBootMediaROM)
#endif
#endif

//
// Define global static data used during initialization.
//

#if DBG
const XBOX_KRNL_VERSION XboxKrnlVersion = { VER_PRODUCTVERSION | 0x8000 };
#else
const XBOX_KRNL_VERSION XboxKrnlVersion = { VER_PRODUCTVERSION };
#endif

XBOX_HARDWARE_INFO XboxHardwareInfo;
DECLSPEC_STICKY PLAUNCH_DATA_PAGE LaunchDataPage = NULL;
DECLSPEC_STICKY ULONG XboxBootFlags;

#ifdef DEVKIT
PVOID KiDbgClockInt;
PVOID KiDbgProfInt;
PSWAP_CONTEXT_NOTIFY_ROUTINE KiDbgCtxSwapNotify;
PVOID KiDpcDispatchNotify;
DECLSPEC_STICKY BOOLEAN ExpDisableDebugMonitor;
XProfpGlobals ExpCallAttributedProfileData;

INITIALIZED_OBJECT_STRING_RDATA(ExpDVDXbdmDLL, "\\Device\\CdRom0\\xbdm.dll");
INITIALIZED_OBJECT_STRING_RDATA(ExpHDXbdmDLL, "\\Device\\Harddisk0\\Partition1\\xbdm.dll");
INITIALIZED_OBJECT_STRING_RDATA(ExpCdRomBootROMString, "\\Device\\CdRom0\\XBOXROM.BIN");
INITIALIZED_OBJECT_STRING_RDATA(ExpHardDiskBootROMString, "\\Device\\Harddisk0\\Partition1\\XBOXROM.BIN");
#endif // DEVKIT

//
// Various encryption keys used by Xbox:
//
//  EEPROM key -
//      This key is stored in the boot loader (encrypted by the master key in MCPX).
//      The boot loader passes it to the kernel on a cold reboot.
//      The kernel uses it to decrypt the encryption section of the EEPROM
//      and then throws it away (except that we need to let the manufacturing
//      program access it during the final system integration stage).
//
//  Hard disk key -
//      This key is stored in the EEPROM (encrypted by the EEPROM key above).
//
//  LAN key -
//      This key's raw data is stored in the first XBE that is run from a cold
//      boot and modified by the CERT key.
//
//  Signature key -
//      This key's raw data is stored in the first XBE that is run from a cold
//      boot and modified by the CERT key.
//
//  CERT key -
//      This key is stored in the boot loader (encrypted by the master key in MCPX).
//      The boot loader passes it to the kernel on a cold reboot.
//      It's used to decrypt keys in the title CERT header.
//
DECLSPEC_STICKY XBOX_KEY_DATA XboxEEPROMKey;
DECLSPEC_STICKY XBOX_KEY_DATA XboxHDKey;
DECLSPEC_STICKY XBOX_KEY_DATA XboxLANKey;
DECLSPEC_STICKY XBOX_KEY_DATA XboxSignatureKey;
DECLSPEC_STICKY XBOX_KEY_DATA XboxAlternateSignatureKeys[XBEIMAGE_ALTERNATE_TITLE_ID_COUNT];
DECLSPEC_STICKY XBOX_KEY_DATA XboxCERTKey;

//
// Game region setting is stored in the encrypted section
// of the EEPROM and is read into a global variable during the boot process.
//
DECLSPEC_STICKY ULONG XboxGameRegion = XC_GAME_REGION_MANUFACTURING;

VOID
ExpInitializeExecutive(
    VOID
    )
/*++

Routine Description:

    This routine is called from the kernel initialization routine during
    bootstrap to initialize the executive and all of its subcomponents.
    Each subcomponent is potentially called twice to perform phase 0, and
    then phase 1 initialization. During phase 0 initialization, the only
    activity that may be performed is the initialization of subcomponent
    specific data. Phase 0 initilaization is performed in the context of
    the kernel start up routine with initerrupts disabled. During phase 1
    initialization, the system is fully operational and subcomponents may
    do any initialization that is necessary.

Arguments:

    None.

Return Value:

    None.

--*/
{
    //
    // Initialize the hardware information flags.
    //

#ifdef DEVKIT
    XboxHardwareInfo.Flags |= XBOX_HW_FLAG_DEVKIT_KERNEL;
#endif

#ifdef FOCUS
    XboxHardwareInfo.Flags |= XBOX_480P_MACROVISION_ENABLED;
#endif

#ifdef ARCADE
    XboxHardwareInfo.Flags |= XBOX_HW_FLAG_ARCADE;
#endif

    //
    // Initialize the Hardware Architecture Layer (HAL).
    //

    if (KeHasQuickBooted) {
        HalInitSystemPhase0Quick();
    } else {
        HalInitSystemPhase0();
    }

    //
    // Initialize the memory manager.
    //

    HalPulseHardwareMonitorPin();
    MmInitSystem();

    //
    // Initialize the object manager.
    //

    if (!ObInitSystem()) {
        KeBugCheck(OBJECT_INITIALIZATION_FAILED);
    }

    if (!PsInitSystem()) {
        KeBugCheck(PROCESS_INITIALIZATION_FAILED);
    }
}

VOID
ExpDecryptEEPROM(
    VOID
    )
{
    NTSTATUS Status;
    ULONG Type, Length;
    XBOX_ENCRYPTED_SETTINGS EncryptedSettings;
    ULONG Temp;

    ASSERT(sizeof(EncryptedSettings) == EEPROM_ENCRYPTED_SECTION_SIZE);
    ASSERT(sizeof(EncryptedSettings.Checksum) == XC_SERVICE_DIGEST_SIZE);

    //
    // Read the encrypted section of the EEPROM
    //
    Status = ExQueryNonVolatileSetting(
                XC_ENCRYPTED_SECTION,
                &Type,
                &EncryptedSettings,
                sizeof(EncryptedSettings),
                &Length);

    if (!NT_SUCCESS(Status)) {
        goto DecryptEEPROMError;
    }
#ifdef DEVKIT
    //
    // On DEVKIT box, the data can be either encrypted or unencrypted.
    // On retail box, it's always encrypted.
    //
    // NOTE: We treat the EEPROM as unencrypted if the confounder
    // field is all 0's.
    //
    if (((PULONG) EncryptedSettings.Confounder)[0] | ((PULONG) EncryptedSettings.Confounder)[1])
#endif
    {
        #define RC4_CONFOUNDER_LEN FIELD_SIZE(XBOX_ENCRYPTED_SETTINGS, Confounder)

        UCHAR Checksum[XC_SERVICE_DIGEST_SIZE];
        UCHAR Confounder[RC4_CONFOUNDER_LEN];
        UCHAR LocalKey[XC_SERVICE_DIGEST_SIZE];
        UCHAR Rc4KeyStruct[XC_SERVICE_RC4_KEYSTRUCT_SIZE];
        UCHAR* Data;
        ULONG DataSize;

        Data = EncryptedSettings.HDKey;
        DataSize = EEPROM_ENCRYPTED_SECTION_SIZE - FIELD_OFFSET(XBOX_ENCRYPTED_SETTINGS, HDKey);
        memcpy(Confounder, EncryptedSettings.Confounder, RC4_CONFOUNDER_LEN);

        //
        // HMAC the checksum into the key
        //
        XcHMAC(
            XboxEEPROMKey,
            XBOX_KEY_LENGTH,
            EncryptedSettings.Checksum,
            XC_SERVICE_DIGEST_SIZE,
            NULL,
            0,
            LocalKey);

        //
        // Use the generated key as the RC4 encryption key
        //
        XcRC4Key(Rc4KeyStruct, XC_SERVICE_DIGEST_SIZE, LocalKey);

        //
        // Decrypt confounder and data
        //
        XcRC4Crypt(Rc4KeyStruct, RC4_CONFOUNDER_LEN, Confounder);
        XcRC4Crypt(Rc4KeyStruct, DataSize, Data);

        //
        // Now verify the checksum.
        //
        XcHMAC(
            XboxEEPROMKey,
            XBOX_KEY_LENGTH,
            Confounder,
            RC4_CONFOUNDER_LEN,
            Data,
            DataSize,
            Checksum);

        //
        // Decrypt is successful only if checksum matches
        //
        if (memcmp(Checksum, EncryptedSettings.Checksum, XC_SERVICE_DIGEST_SIZE) != 0) {
            Status = STATUS_IMAGE_CHECKSUM_MISMATCH;
            goto DecryptEEPROMError;
        }
    }

    memcpy(XboxHDKey, EncryptedSettings.HDKey, XBOX_KEY_LENGTH);
    XboxGameRegion = EncryptedSettings.GameRegion;

#ifdef DEVKIT
    //
    // For now, force a valid game region
    //
    if (XboxGameRegion == 0) {
        XboxGameRegion = XC_GAME_REGION_MANUFACTURING;
    }

#endif

    //
    // Verify that only one game region bit is set, ignoring the manufacturing
    // region.
    //

    Temp = XboxGameRegion & (~XC_GAME_REGION_MANUFACTURING);
    if ((Temp & (Temp - 1)) != 0) {
        Status = STATUS_IMAGE_GAME_REGION_VIOLATION;
        goto DecryptEEPROMError;
    }

    return;

DecryptEEPROMError:
    KdPrint(("INIT: failed to decrypt EEPROM settings (status=%08x).\n", Status));

#ifndef DEVKIT
    //
    // Write LED state to indicate that there was a problem with the EEPROM
    //

    HalWriteSMCLEDStates(SMC_LED_STATES_RED_STATE1 | SMC_LED_STATES_RED_STATE3);

    PsTerminateSystemThread(STATUS_SUCCESS);
#endif
}

#ifdef DEVKIT

DECLSPEC_INITDATA UCHAR ExpBldrEncryptionKey[] = {
    0x57,0x42,0x29,0x0C,0x30,0x1E,0xD3,0x01,
    0xB3,0xE5,0x5D,0x28,0x50,0x31,0xE1,0xCE
};

VOID
ExpTryToBootMediaROM(
    VOID
    )
{
    NTSTATUS status;
    OBJECT_ATTRIBUTES ObjectAttributes;
    HANDLE FileHandle;
    IO_STATUS_BLOCK IoStatusBlock;
    PVOID RelocatedBootLoader;
    PVOID ROMShadow;
    struct RC4_KEYSTRUCT RC4KeyStruct;
    BOOLEAN LoadedFromCdRom;
    PUCHAR BootOptions;
    PBLDR_LOAD_MEDIA_ROM_ROUTINE LoadMediaROMRoutine;

    //
    // Attempt to open a ROM from the CD-ROM.
    //

    LoadedFromCdRom = TRUE;

    InitializeObjectAttributes(&ObjectAttributes, &ExpCdRomBootROMString,
        OBJ_CASE_INSENSITIVE, NULL, NULL);

    status = NtOpenFile(&FileHandle, GENERIC_READ, &ObjectAttributes,
        &IoStatusBlock, 0, FILE_SYNCHRONOUS_IO_NONALERT |
        FILE_NON_DIRECTORY_FILE | FILE_NO_INTERMEDIATE_BUFFERING);

    if (!NT_SUCCESS(status)) {

        //
        // Attempt to open a ROM from the hard drive.
        //

        LoadedFromCdRom = FALSE;

        ObjectAttributes.ObjectName = &ExpHardDiskBootROMString;

        status = NtOpenFile(&FileHandle, GENERIC_READ, &ObjectAttributes,
            &IoStatusBlock, 0, FILE_SYNCHRONOUS_IO_NONALERT |
            FILE_NON_DIRECTORY_FILE | FILE_NO_INTERMEDIATE_BUFFERING);
    }

    //
    // If no ROM file was found, then use the current ROM to boot.
    //

    if (!NT_SUCCESS(status)) {
        return;
    }

    //
    // Allocate the boot loader memory at the boot loader's relocated base
    // physical address.
    //

    RelocatedBootLoader = MmAllocateContiguousMemoryEx(BLDR_BLOCK_SIZE,
        BLDR_RELOCATED_ORIGIN, BLDR_RELOCATED_ORIGIN + BLDR_BLOCK_SIZE - 1, 0,
        PAGE_READWRITE);

    if (RelocatedBootLoader == NULL) {
        DbgPrint("INIT: Failed to allocate boot loader memory.\n");
        NtClose(FileHandle);
        return;
    }

    //
    // Allocate the media ROM memory.  If we cannot allocate a block of this
    // size at this early phase of initialization, then something's wrong, but
    // we'll attempt to continue initializing the system.  We limit the top
    // address of the allocation so that we don't run into the bottom of the
    // PFN database where we may try to place a copy of the kernel's initialized
    // data section.
    //

    ROMShadow = MmAllocateContiguousMemoryEx(ROM_SHADOW_SIZE, 0, 48 * 1024 * 1024,
        0, PAGE_READWRITE);

    if (ROMShadow == NULL) {
        DbgPrint("INIT: Failed to allocate shadow ROM memory.\n");
        NtClose(FileHandle);
        return;
    }

    //
    // Read the media ROM into memory.  If this fails, we'll take our chances
    // and continue booting with the current ROM.
    //

    status = NtReadFile(FileHandle, NULL, NULL, NULL, &IoStatusBlock,
        ROMShadow, ROM_SHADOW_SIZE, NULL);

    NtClose(FileHandle);

    if (!NT_SUCCESS(status) || (IoStatusBlock.Information != ROM_SHADOW_SIZE)) {
        DbgPrint("INIT: Failed to read ROM from media.\n");
        MmFreeContiguousMemory(ROMShadow);
        MmFreeContiguousMemory(RelocatedBootLoader);
        return;
    }

    //
    // This is an ugly hack to prevent downgrading to the July release - we refuse
    // to load the xboxrom.bin on its CD based on the absence of a number in the
    // "trim values version" part of the init table (at DWORD 0x1B == offset 0x6C)
    //

    if (LoadedFromCdRom &&
        (((PULONG)ROMShadow)[0x1B] == 0x00000000)) {
        DbgPrint("INIT: Failed to load ROM from media because it is too old.\n");
        MmFreeContiguousMemory(ROMShadow);
        MmFreeContiguousMemory(RelocatedBootLoader);
        return;
    }

    //
    // Decrypt the boot loader in the media ROM image.
    //

    rc4_key(&RC4KeyStruct, sizeof(ExpBldrEncryptionKey), ExpBldrEncryptionKey);
    rc4(&RC4KeyStruct, BLDR_BLOCK_SIZE, (PUCHAR)ROMShadow + ROM_SHADOW_SIZE -
        ROM_DEC_SIZE - BLDR_BLOCK_SIZE);

    //
    // Copy the boot loader to its relocated base physical address.
    //

    RtlCopyMemory(RelocatedBootLoader, (PUCHAR)ROMShadow + ROM_SHADOW_SIZE -
        ROM_DEC_SIZE - BLDR_BLOCK_SIZE, BLDR_BLOCK_SIZE);

    //
    // Modify the load options to indicate where the media ROM was loaded from.
    //

    BootOptions = (PUCHAR)(KSEG0_BASE + BLDR_RELOCATED_ORIGIN + sizeof(ULONG));

    if (LoadedFromCdRom) {
        strcat(BootOptions, " /SHADOW /CDBOOT");
        DbgPrint("INIT: Loaded kernel image from CD-ROM.\n");
    } else {
        strcat(BootOptions, " /SHADOW /HDBOOT");
        DbgPrint("INIT: Loaded kernel image from hard disk.\n");
    }

    if ((XboxBootFlags & XBOX_BOOTFLAG_DASHBOARDBOOT) != 0) {
        strcat(BootOptions, " /DBBOOT");
    }

    //
    // Compute the address of the load media ROM routine.  The pointer to the
    // routine is located immediately before the start address of the boot
    // loader.
    //

    LoadMediaROMRoutine =
        *((PBLDR_LOAD_MEDIA_ROM_ROUTINE*)((PUCHAR)RelocatedBootLoader +
        (*((PULONG_PTR)RelocatedBootLoader) - BLDR_BOOT_ORIGIN -
        sizeof(ULONG_PTR))));
    LoadMediaROMRoutine = (PBLDR_LOAD_MEDIA_ROM_ROUTINE)
        (MM_SYSTEM_PHYSICAL_MAP + (ULONG_PTR)LoadMediaROMRoutine);

    //
    // Stop the boot animation.
    //

    AniSetLogo(NULL, 0);
    AniTerminateAnimation();

    //
    // Notify the debugger that we're "rebooting".
    //

    DbgUnLoadImageSymbols(NULL, (PVOID)-1, 0);

    //
    // Load the kernel from the media ROM.
    //

    LoadMediaROMRoutine(MmGetPhysicalAddress(ROMShadow));
}

NTSTATUS
ExpStartDebugMonitor(
    VOID
    )
/*++

Routine Description:

    This routine starts the debug monitor on a development kit system.

Arguments:

    None.

Return Value:

    Status of operation.

--*/
{
    PVOID pvBase;
    NTSTATUS st;
    DMINIT dminit;
    ULONG (__stdcall *EntryPoint)(PVOID, PVOID, ULONG);
    extern PKDEBUG_ROUTINE KiDebugRoutine;
    HANDLE h;
    OBJECT_ATTRIBUTES oa;
    FILE_NETWORK_OPEN_INFORMATION fni;
    IO_STATUS_BLOCK iosb;
    ULONG cPages;
    BOOLEAN fLoadCanFail;
    POBJECT_STRING postLoadFrom;

    memset(&dminit, 0, sizeof dminit);
    /* We insist on loading from the same location as the kernel.  If the
     * kernel loaded from ROM, we'll try to load from the HD, but we'll
     * allow the load to fail */
    if(XboxBootFlags & XBOX_BOOTFLAG_HDBOOT) {
        fLoadCanFail = FALSE;
        postLoadFrom = &ExpHDXbdmDLL;
    } else if(XboxBootFlags & XBOX_BOOTFLAG_CDBOOT) {
        dminit.Flags |= DMIFLAG_CDBOOT;
        fLoadCanFail = FALSE;
        postLoadFrom = &ExpDVDXbdmDLL;
    } else {
        fLoadCanFail = TRUE;
        postLoadFrom = &ExpHDXbdmDLL;
    }

    // Load the dm dll
    InitializeObjectAttributes(&oa, postLoadFrom, OBJ_CASE_INSENSITIVE, NULL,
        NULL);
    st = NtOpenFile(&h, FILE_EXECUTE, &oa, &iosb, 0,
        FILE_SYNCHRONOUS_IO_NONALERT);
    if(!NT_SUCCESS(st)) {
        goto ErrRet;
    }

    st = NtQueryInformationFile(h, &iosb, &fni, sizeof fni,
        FileNetworkOpenInformation);
    if(!NT_SUCCESS(st))
        goto ErrClose;
    pvBase = MmDbgAllocateMemory(fni.EndOfFile.LowPart, PAGE_READWRITE);
    if(pvBase == NULL) {
        st = STATUS_NO_MEMORY;
        goto ErrClose;
    }
    st = NtReadFile(h, NULL, NULL, NULL, &iosb, pvBase, fni.EndOfFile.LowPart,
        NULL);
    if(!NT_SUCCESS(st))
        goto ErrClose;
    try {
        st = LdrRelocateImage(pvBase, "xbdm", STATUS_SUCCESS,
            STATUS_CONFLICTING_ADDRESSES, STATUS_INVALID_IMAGE_FORMAT);
    } except(EXCEPTION_EXECUTE_HANDLER) {
        st = GetExceptionCode();
    }
    if(!NT_SUCCESS(st)) {
        MmDbgFreeMemory(pvBase, fni.EndOfFile.LowPart);
ErrClose:
        NtClose(h);
ErrRet:
        if(fLoadCanFail) {
            DbgPrint("Failed to load any xbdm.dll!\n");
            return STATUS_SUCCESS;
        }
        return st;
    }
    NtClose(h);

    // The whole thing is finally loaded.  Set up the init parameters
    dminit.DebugRoutine = &KiDebugRoutine;
    dminit.LoadedModuleList = &KdLoadedModuleList;
    dminit.CtxSwapNotifyRoutine = (VOID**) &KiDbgCtxSwapNotify;
    dminit.DpcDispatchNotifyRoutine = (VOID**)&KiDpcDispatchNotify;
    dminit.XProfpDataPtr = &ExpCallAttributedProfileData;
    dminit.ClockIntRoutine = &KiDbgClockInt;
    dminit.ProfIntRoutine = &KiDbgProfInt;
    dminit.HalStartProfileRoutine = HalStartProfileInterrupt;
    dminit.HalStopProfileRoutine = HalStopProfileInterrupt;
    dminit.HalProfileIntervalRoutine = HalSetProfileInterval;
    dminit.DisallowXbdm = &ExpDisableDebugMonitor;

    EntryPoint = (PVOID)((ULONG_PTR) pvBase +
        RtlImageNtHeader(pvBase)->OptionalHeader.AddressOfEntryPoint);

    return (EntryPoint)(pvBase, &dminit, 0);
}

#endif // DEVKIT

ULONG
ExpDetectSettingsError(
    VOID
    )
/*++

Routine Description:

    Detect if clock/timezone/language settings are invalid

Arguments:

    None.

Return Value:

    0 if everything is ok
    Otherwise, a bit flag indicating which settings are bad

--*/
{
    ULONG errors = 0;
    NTSTATUS status;
    ULONG type, length;
    XBOX_USER_SETTINGS userSettings;

    //
    // Check if the CMOS data is valid.
    //
    if (!HalIsCmosValid()) {
        errors |= XLD_SETTINGS_CLOCK;
    } else {
        //
        // NOTE: Workaround for RTC hardware problem
        // Sometimes the clock would lose power but the flag bit isn't set.
        // We query the RTC time here and do a sanity check.
        // If the time is before 1/1/2001 or after 1/1/2101,
        // we'll assume the clock has gone bad.
        //
        LARGE_INTEGER time;
        KeQuerySystemTime(&time);
        if (time.QuadPart < 0x01c07385c89dc000i64 ||
            time.QuadPart > 0x02309034f02ac000i64) {
            KdPrint(("Bad real-time clock value: %08x%08x\n", time.HighPart, time.LowPart));
            errors |= XLD_SETTINGS_CLOCK;
        }
    }

    //
    // Read the user settings inside the EEPROM
    //
    status = ExQueryNonVolatileSetting(
                XC_MAX_OS,
                &type,
                &userSettings,
                sizeof(userSettings),
                &length);

    if (NT_SUCCESS(status)) {
        //
        // Check the language setting
        //
        if (userSettings.Language == XC_LANGUAGE_UNKNOWN)
            errors |= XLD_SETTINGS_LANGUAGE;

        //
        // Check the time zone setting
        //  NOTE: we assume the time zone isn't set if the name is empty.
        //
        if (XBOX_INVALID_TIMEZONE_SETTING(userSettings))
            errors |= XLD_SETTINGS_TIMEZONE;
    } else {
        //
        // Assume both language/timezone are invalid
        // if there is an EEPROM read error.
        //
        errors |= XLD_SETTINGS_TIMEZONE|XLD_SETTINGS_LANGUAGE;
    }

#if DBG
    if (errors) {
        DbgPrint("INIT: invalid clock/timezone/language settings - %x\n", errors);
    }
#endif

    return errors;
}


VOID
ExpCopyLastUEMError(
    VOID
    )
/*++

Routine Description:

    This rotutine copies the Last UEM error code from the EEPROM to the
    SMC error register.  This routine is called only when we are in the
    Manufacturing region.

Arguments:

    None.

Return Value:

    None

--*/
{
    EEPROM_LAYOUT EEPROMData;
    NTSTATUS Status;
    ULONG SettingType;
    ULONG SettingLength;
    XBOX_UEM_INFO* UEMInfo;

    UEMInfo = (XBOX_UEM_INFO*)&(EEPROMData.UEMInfo[0]);

    //
    // Read the EEPROM
    //

    Status = ExQueryNonVolatileSetting(XC_MAX_ALL, &SettingType, &EEPROMData,
        sizeof(EEPROMData), &SettingLength);

    if (NT_SUCCESS(Status)) {

        //
        // If the last code is non-zero, write it to the SMC and then clear
        // it in the EEPROM
        //

        if (UEMInfo->LastCode != FATAL_ERROR_NONE) {

            HalWriteSMBusByte(SMC_SLAVE_ADDRESS, SMC_COMMAND_WRITE_ERROR_CODE, UEMInfo->LastCode);

            UEMInfo->LastCode = 0;

            ExSaveNonVolatileSetting(XC_MAX_ALL, SettingType, &EEPROMData, SettingLength);
        }
    }
}



VOID
Phase1Initialization(
    IN PVOID Context
    )
{
    NTSTATUS status;
    LARGE_INTEGER UniversalTime;
    LARGE_INTEGER CmosTime;
    LARGE_INTEGER OldTime;
    TIME_FIELDS TimeFields;
    ULONG SettingsError;

    //
    // Initialize the Hardware Architecture Layer (HAL).
    //

    HalInitSystemPhase1();

    //
    // Initialize the system time and set the time the system was booted.
    //
    // N.B. This cannot be done until after the phase one initialization
    //      of the HAL Layer.
    //

    if (HalQueryRealTimeClock(&TimeFields) != FALSE) {

        if(TimeFields.Year < 2000 || TimeFields.Year > 2100 ||
            !RtlTimeFieldsToTime(&TimeFields, &CmosTime))
        {
            // use a bogus time value to indicate bogus clock
            CmosTime.HighPart = 0x20000001;
            CmosTime.LowPart = 0;
        }
        UniversalTime = CmosTime;
        KeSetSystemTime(&UniversalTime, &OldTime);

        KeBootTime = UniversalTime;
        KeBootTimeBias = 0;
    }

    //
    // Read the MCPX revision number.
    //

    HalReadPCISpace(
        0,
        XPCI_SLOT_NUMBER(LPCBRIDGE),
        8,
        &XboxHardwareInfo.McpRevision,
        sizeof(XboxHardwareInfo.McpRevision));

    //
    // NOTE: NV2A revision number is not found in the PCI config space.
    // Rather, it's in the LSB of the very first NV2A register.
    //

    XboxHardwareInfo.GpuRevision = (UCHAR) *((ULONG*) XPCICFG_GPU_MEMORY_REGISTER_BASE_0);

    //
    // Set the USB flag based on the MCP revision.  If the MCP is B03 (rev=b3) or
    // earlier, we are using a USB daughterboard.  For later revs, we don't use it.
    //

    if (XboxHardwareInfo.McpRevision <= 0xB3) {
        XboxHardwareInfo.Flags |= XBOX_HW_FLAG_INTERNAL_USB_HUB;
    }

    //
    // If the kernel is cold-booting, then do some additional work before
    // initializing the rest of the kernel.
    //

    if (!KeHasQuickBooted) {

        //
        // Decrypt the encrypted section of the EEPROM and save the hard disk
        // key and the game region information into global variables.
        //

        HalPulseHardwareMonitorPin();
        ExpDecryptEEPROM();

        //
        // Block if no AV pack is attached to the system.
        //

        HalPulseHardwareMonitorPin();
        HalBlockIfNoAVPack();

        //
        // Display the fatal error message if we've been rebooted for that
        // purpose.
        //

        if ((XboxBootFlags & XBOX_BOOTFLAG_DISPLAYFATALERROR) != 0) {

            HalWriteSMCLEDStates(SMC_LED_STATES_GREEN_STATE0 | SMC_LED_STATES_RED_STATE1 |
                SMC_LED_STATES_RED_STATE2 | SMC_LED_STATES_RED_STATE3);

            ExDisplayFatalError(FATAL_ERROR_REBOOT_ROUTINE);
        }

        //
        // Start the boot animation if this isn't a shadow boot.
        //

        if ((XboxBootFlags & XBOX_BOOTFLAG_SHADOW) == 0) {
            HalPulseHardwareMonitorPin();
            AniStartAnimation((XboxBootFlags & XBOX_BOOTFLAG_SHORTANIMATION) != 0);
        }
    }

    //
    // Initialize the IDE driver.
    //

    IdexChannelCreate();

#ifdef DEVKIT
    //
    // Try to load a ROM from the media.  Only do this if we haven't already
    // tried in an earlier boot from the system's real ROM or if we haven't
    // already been loaded from a shadow ROM.
    //

    if (!KeHasQuickBooted && !(XboxBootFlags & XBOX_BOOTFLAG_SHADOW)) {
        ExpTryToBootMediaROM();
    }

    //
    // Start the debug monitor.
    //

    if (!ExpDisableDebugMonitor) {

        status = ExpStartDebugMonitor();

        if (!NT_SUCCESS(status)) {
            KdPrint(("INIT: Debug monitor failed to start (status=%08x).\n", status));
            KeBugCheckEx(PHASE1_INITIALIZATION_FAILED, status, 0, 0, 0);
        }
    }
#endif

    //
    // If the kernel is cold-booting, and we're not booting from a kernel loaded
    // from the CD-ROM and the console isn't configured for the manufacturing
    // content, then check if we should force a boot into the dashboard.  If we
    // detect a settinngs error, we'll try to bring up a title anyway, because
    // titles signed in the manufacturing region are allowed to run with
    // invalid settings; we'll fall back to the dash if we don't find such a
    // title.
    //

    if (!KeHasQuickBooted &&
        ((XboxBootFlags & XBOX_BOOTFLAG_CDBOOT) == 0) &&
        ((XboxGameRegion & XC_GAME_REGION_MANUFACTURING) == 0)) {

        HalPulseHardwareMonitorPin();

        if ((SettingsError = ExpDetectSettingsError()) != 0) {
            XeLoadTitleImage(SettingsError);
        } else if ((XboxBootFlags & XBOX_BOOTFLAG_DASHBOARDBOOT) != 0) {
            XeLoadDashboardImageWithReason(XLD_LAUNCH_DASHBOARD_BOOT, 0);
        } else if ((XboxBootFlags & XBOX_BOOTFLAG_TRAYEJECT) != 0) {
            XeLoadImageAfterTrayEjectBoot();
        } else if ((XboxBootFlags & XBOX_BOOTFLAG_NONSECUREMODE) != 0) {
            XeLoadDashboardImage();
        } else {
            XeLoadTitleImage(0);
        }

    } else {
        XeLoadTitleImage(0);
    }

    //
    // If the kernel is cold-booting, then do some additional work before
    // handing control over to the XBE image.
    //

    if (!KeHasQuickBooted) {

        HalPulseHardwareMonitorPin();

        //
        // Notify the SMC that we've finished initializing the system.  The SMC
        // uses this as a signal to know whether or not we need to be notified
        // when the user powers down the system.  After this point, we do need
        // to be notified because we may have I/O buffers that need to be
        // flushed.
        //

        HalWriteSMBusByte(SMC_SLAVE_ADDRESS, SMC_COMMAND_OS_READY,
            SMC_OS_READY_READY);

        //
        // Stop the boot animation.
        //

        AniTerminateAnimation();

        //
        // If we are in manufacturing mode and have not quickbooted, handle the
        // copying the last UEM error code from EEPROM to SMC error code register.
        //

        if (XboxGameRegion & XC_GAME_REGION_MANUFACTURING) {
            ExpCopyLastUEMError();
        }

        //
        // Discard the kernel's initialization code section.  All of the code in
        // the INIT section applies to a cold boot only.
        //

        MmDiscardInitSection();
    }

    //
    // Relocate persistent contiguous memory allocations in order to present
    // roughly the same physical memory layout to an XBE image.
    //

    MmRelocatePersistentMemory();

    //
    // Invoke the entry point of the XBE image.
    //

    XeImageHeader()->AddressOfEntryPoint();

    //
    // The XBE startup code runs the main title thread on a new thread, so
    // terminate this initialization thread.
    //

    PsTerminateSystemThread(STATUS_SUCCESS);
}
