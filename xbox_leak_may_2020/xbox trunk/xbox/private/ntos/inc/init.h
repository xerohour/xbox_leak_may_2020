/*++ BUILD Version: 0002    // Increment this if a change has global effects

Copyright (c) 1989  Microsoft Corporation

Module Name:

    init.h

Abstract:

    Header file for the INIT subcomponent of NTOS

Author:

    Steve Wood (stevewo) 31-Mar-1989

Revision History:

--*/

#ifndef _INIT_
#define _INIT_

//
// Define xbox kernel version structure.
//

typedef struct _XBOX_KRNL_VERSION {
    USHORT Major;
    USHORT Minor;
    USHORT Build;
    USHORT Qfe;
} XBOX_KRNL_VERSION, *PXBOX_KRNL_VERSION;

#if !defined(_NTSYSTEM_)
extern const PXBOX_KRNL_VERSION XboxKrnlVersion;
#else
extern const XBOX_KRNL_VERSION XboxKrnlVersion;
#endif

//
// Information about the current hardware that the system is running on
//
typedef struct _XBOX_HARDWARE_INFO {
    ULONG Flags;
    UCHAR GpuRevision;
    UCHAR McpRevision;
    UCHAR reserved[2];
} XBOX_HARDWARE_INFO;

#if !defined(_NTSYSTEM_)
extern const XBOX_HARDWARE_INFO* XboxHardwareInfo;
#else
extern XBOX_HARDWARE_INFO XboxHardwareInfo;
#endif

//
//  Define XboxHardwareFlags - a list of
//  flags indicating hardware differences
//  the title libraries must know about.
//
#define XBOX_HW_FLAG_INTERNAL_USB_HUB   0x00000001
#define XBOX_HW_FLAG_DEVKIT_KERNEL      0x00000002
#define XBOX_480P_MACROVISION_ENABLED   0x00000004
#define XBOX_HW_FLAG_ARCADE             0x00000008

//
// Boot flags
//
extern ULONG XboxBootFlags;

#define XBOX_BOOTFLAG_SHADOW            0x00000001
#define XBOX_BOOTFLAG_HDBOOT            0x00000002
#define XBOX_BOOTFLAG_CDBOOT            0x00000004
#define XBOX_BOOTFLAG_NONSECUREMODE     0x00000008
#define XBOX_BOOTFLAG_SHORTANIMATION    0x00000010
#define XBOX_BOOTFLAG_DISPLAYFATALERROR 0x00000020
#define XBOX_BOOTFLAG_TRAYEJECT         0x00000040
#define XBOX_BOOTFLAG_DASHBOARDBOOT     0x00000080

//
// Define the fatal error codes.  The bitmap of UEMs stored
// in EEPROM starts at HDD_NOT_LOCKED being at bit zero.  
// CORE_DIGITAL and BAD_RAM are values used by the SMC.
//

#define FATAL_ERROR_NONE                0x00
#define FATAL_ERROR_CORE_DIGITAL        0x01
#define FATAL_ERROR_BAD_EEPROM          0x02
#define FATAL_ERROR_UNUSED1             0x03    
#define FATAL_ERROR_BAD_RAM             0x04
#define FATAL_ERROR_HDD_NOT_LOCKED      0x05
#define FATAL_ERROR_HDD_CANNOT_UNLOCK   0x06
#define FATAL_ERROR_HDD_TIMEOUT         0x07
#define FATAL_ERROR_HDD_NOT_FOUND       0x08
#define FATAL_ERROR_HDD_BAD_CONFIG      0x09
#define FATAL_ERROR_DVD_TIMEOUT         0x0A
#define FATAL_ERROR_DVD_NOT_FOUND       0x0B
#define FATAL_ERROR_DVD_BAD_CONFIG      0x0C
#define FATAL_ERROR_XBE_DASH_GENERIC    0x0D
#define FATAL_ERROR_XBE_DASH_ERROR      0x0E
#define FATAL_ERROR_UNUSED2             0x0F
#define FATAL_ERROR_XBE_DASH_SETTINGS   0x10
#define FATAL_ERROR_UNUSED3             0x11
#define FATAL_ERROR_UNUSED4             0x12
#define FATAL_ERROR_UNUSED5             0x13
#define FATAL_ERROR_XBE_DASH_X2_PASS    0x14
#define FATAL_ERROR_REBOOT_ROUTINE      0x15
#define FATAL_ERROR_RESERVED            0xFF


VOID
KiSystemStartup(
    VOID
    );

VOID
Phase1Initialization(
    IN PVOID Context
    );

DECLSPEC_NORETURN
VOID 
ExDisplayFatalError(
    IN ULONG ErrorCode
    );

//
// Various Xbox encryption keys and game region
//
#define XBOX_KEY_LENGTH 16

typedef UCHAR XBOX_KEY_DATA[XBOX_KEY_LENGTH];
extern XBOX_KEY_DATA XboxCERTKey;
extern ULONG XboxGameRegion;

#if !defined(_NTSYSTEM_)
extern const XBOX_KEY_DATA* XboxEEPROMKey;
extern const XBOX_KEY_DATA* XboxHDKey;
extern const XBOX_KEY_DATA* XboxLANKey;
extern const XBOX_KEY_DATA* XboxSignatureKey;
extern const XBOX_KEY_DATA* XboxAlternateSignatureKeys[];
#else
extern XBOX_KEY_DATA XboxEEPROMKey;
extern XBOX_KEY_DATA XboxHDKey;
extern XBOX_KEY_DATA XboxLANKey;
extern XBOX_KEY_DATA XboxSignatureKey;
extern XBOX_KEY_DATA XboxAlternateSignatureKeys[];
#endif

#endif // _INIT_

