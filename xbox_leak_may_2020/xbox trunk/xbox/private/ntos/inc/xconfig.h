#ifndef _XCONFIG_H_
#define _XCONFIG_H_

#ifdef __cplusplus
extern "C" {
#endif

//
//  Define the FAT32 X-Box config sector
//
typedef struct _XBOX_CONFIG_SECTOR {
    ULONG SectorBeginSignature;                     // offset = 0x000   0
    ULONG Version;                                  // offset = 0x004   4
    ULONG SectorCount;                              // offset = 0x008   8
    UCHAR Data[492];                                // offset = 0x00c  12
    ULONG Checksum;                                 // offset = 0x1f8 504
    ULONG SectorEndSignature;                       // offset = 0x1fc 508
} XBOX_CONFIG_SECTOR, *PXBOX_CONFIG_SECTOR;

#define XBOX_CONFIG_SECTOR_BEGIN_SIGNATURE   0x79132568
#define XBOX_CONFIG_SECTOR_END_SIGNATURE     0xAA550000
#define XBOX_CONFIG_VERSION                  0x00000001
#define XBOX_CONFIG_SECTOR_COUNT             0x00000001

#define XBOX_CONFIG_DATA_SIZE      (sizeof(((PXBOX_CONFIG_SECTOR) 0)->Data))
#define XBOX_HD_SECTOR_SIZE        512

#define XBOX_BOOT_SECTOR_INDEX     0

#define XBOX_CACHE_DB_SECTOR_INDEX 4
#define XBOX_CONFIG_SECTOR_INDEX   8
#define XBOX_NUM_CONFIG_SECTORS    8

//
// Compute 32-bit 1's complement sum
//  NOTE: It'd be nice to make this a __fastcall.
//  But it doesn't work because of some compiler problems.
//
#pragma warning(push)
#pragma warning(disable:4035)
__inline ULONG XConfigChecksum(const VOID* data, ULONG count) {
    __asm {
        mov     ecx, data
        mov     edx, count
        xor     eax, eax
        xor     ebx, ebx
        shr     edx, 2      // count /= sizeof(ULONG)
        test    edx, edx
        jz      L2
    L1: add     eax, [ecx]  // eax += *data++
        adc     ebx, 0      // ebx += carry
        add     ecx, 4
        dec     edx
        jnz     L1          // while (--count)
    L2: add     eax, ebx    // take care of accumulated carries
        adc     eax, 0
    }
}
#pragma warning(pop)

//
// Refurb information sector
//
#define XBOX_REFURB_INFO_SECTOR_INDEX 3
#define XBOX_REFURB_INFO_SIGNATURE 'RFRB'

typedef struct _XBOX_REFURB_INFO {
    ULONG Signature;
    ULONG PowerCycleCount;
    LARGE_INTEGER FirstSetTime;
} XBOX_REFURB_INFO;

//
// Data layout of the non-volatile memory (EEPROM)
//
//  encrypted factory settings section:
//      programmed at factory and never changes
//      data is encrypted
//  factory settings section:
//      programmed at factory and never changes
//  user settings section:
//      can be modified by the user
//
// !!! NOTE:
//  Each section should start at 8-byte aligned EEPROM page boundary.
//  When you change the structures below in the future,
//  make sure you don't disturb the existing fields.
//
#include <PshPack1.h>

#define FIELD_SIZE(type, field) (sizeof(((type *) 0)->field))

typedef struct _XBOX_FACTORY_SETTINGS {
    ULONG Checksum;                         //  0 +  4
    UCHAR SerialNumber[12];                 //  4 + 12
    UCHAR EthernetAddr[6];                  // 16 +  6
    UCHAR Reserved1[2];                     // 22 +  2
    UCHAR OnlineKey[16];                    // 24 + 16
    ULONG AVRegion;                         // 40 +  4
    ULONG Reserved2;                        // 44 +  4
} XBOX_FACTORY_SETTINGS;                    // 48

// Compressed timezone date representation
// (4 bytes instead of 16 bytes for SYSTEMTIME)
typedef struct _XBOX_TIMEZONE_DATE {
    UCHAR Month;            // 0 = unspecified time zone info
    UCHAR Day;
    UCHAR DayOfWeek;
    UCHAR Hour;
} XBOX_TIMEZONE_DATE;

#define XC_TZNAMELEN 4
typedef struct _XBOX_USER_SETTINGS {
    ULONG Checksum;                         //  0 +  4
    LONG TimeZoneBias;                      //  4 +  4
    CHAR TimeZoneStdName[XC_TZNAMELEN];     //  8 +  4
    CHAR TimeZoneDltName[XC_TZNAMELEN];     // 12 +  4
    ULONG Reserved1[2];                     // 16 +  8
    XBOX_TIMEZONE_DATE TimeZoneStdDate;     // 24 +  4
    XBOX_TIMEZONE_DATE TimeZoneDltDate;     // 28 +  4
    ULONG Reserved2[2];                     // 32 +  8
    LONG TimeZoneStdBias;                   // 40 +  4
    LONG TimeZoneDltBias;                   // 44 +  4
    ULONG Language;                         // 48 +  4
    ULONG VideoFlags;                       // 52 +  4
    ULONG AudioFlags;                       // 56 +  4
    ULONG ParentalControlGames;             // 60 +  4
    ULONG ParentalControlPassword;          // 64 +  4
    ULONG ParentalControlMovies;            // 68 +  4
    ULONG OnlineIpAddress;                  // 72 +  4
    ULONG OnlineDnsAddress;                 // 76 +  4
    ULONG OnlineDefaultGatewayAddress;      // 80 +  4
    ULONG OnlineSubnetMask;                 // 84 +  4
    ULONG MiscFlags;                        // 88 +  4
    ULONG DvdRegion;                        // 92 +  4
} XBOX_USER_SETTINGS;                       // 96


typedef struct _XBOX_UEM_INFO {
    UCHAR  LastCode;
    UCHAR  Reserved1;
    USHORT History;
} XBOX_UEM_INFO;


#define EEPROM_ENCRYPTED_SECTION_START      FIELD_OFFSET(EEPROM_LAYOUT, EncryptedSection)
#define EEPROM_ENCRYPTED_SECTION_SIZE       48
#define EEPROM_FACTORY_SECTION_START        FIELD_OFFSET(EEPROM_LAYOUT, FactorySection)
#define EEPROM_FACTORY_SECTION_SIZE         sizeof(XBOX_FACTORY_SETTINGS)
#define EEPROM_USER_SECTION_START           FIELD_OFFSET(EEPROM_LAYOUT, UserConfigSection)
#define EEPROM_USER_SECTION_SIZE            sizeof(XBOX_USER_SETTINGS)
#define EEPROM_TOTAL_MEMORY_SIZE            256
#define EEPROM_PAGE_ALIGNMENT               8

typedef struct _EEPROM_LAYOUT {
    UCHAR EncryptedSection[EEPROM_ENCRYPTED_SECTION_SIZE];
    UCHAR FactorySection[EEPROM_FACTORY_SECTION_SIZE];
    UCHAR UserConfigSection[EEPROM_USER_SECTION_SIZE];
    UCHAR Unused[58];
    UCHAR UEMInfo[4];
    UCHAR Reserved1[2];   // Reserved for manufacturing
} EEPROM_LAYOUT;

// Check if the time zone information is invalid
#define XBOX_INVALID_TIMEZONE_SETTING(userSettings) \
        ((userSettings).TimeZoneStdName[0] == '\0')

// Convert the timezone name stored in the EEPROM to/from Unicode string
__inline VOID XboxTimeZoneNameToWstr(const CHAR* tzname, WCHAR* wstr)
{
    ULONG count = XC_TZNAMELEN;
    while (count--)
        *wstr++ = (WCHAR) ((UCHAR) *tzname++);
    *wstr = L'\0';
}

__inline VOID WstrToXboxTimeZoneName(const WCHAR* wstr, CHAR* tzname)
{
    ULONG count = XC_TZNAMELEN;
    while (count--)
        *tzname++ = (CHAR) *wstr++;
}

//
// Encrypted section of the EEPROM
//
typedef struct _XBOX_ENCRYPTED_SETTINGS {
    UCHAR Checksum[20];                     //  0 + 20
    UCHAR Confounder[8];                    // 20 +  8
    UCHAR HDKey[XBOX_KEY_LENGTH];           // 28 + 16
    ULONG GameRegion;                       // 44 +  4
} XBOX_ENCRYPTED_SETTINGS;                  // 48

//
// Duplicated constants from xboxp.h so that the kernel proper doesn't have to
// include XTL headers.
//

#define XC_GAME_REGION_NA             0x00000001
#define XC_GAME_REGION_JAPAN          0x00000002
#define XC_GAME_REGION_RESTOFWORLD    0x00000004
#define XC_GAME_REGION_INTERNAL_TEST  0x40000000
#define XC_GAME_REGION_MANUFACTURING  0x80000000

#include <PopPack.h>

//
// Private kernel functions for querying or saving non-volatile settings
//
NTSTATUS
ExQueryNonVolatileSetting(
    IN ULONG ValueIndex,
    OUT ULONG* Type,
    OUT VOID* Value,
    IN ULONG ValueLength,
    OUT ULONG* ResultLength
    );

NTSTATUS
ExSaveNonVolatileSetting(
    IN ULONG ValueIndex,
    IN ULONG Type,
    IN const VOID* Value,
    IN ULONG ValueLength
    );

//
// Private function for reading/writing refurb information
//
NTSTATUS
ExReadWriteRefurbInfo(
    OUT XBOX_REFURB_INFO* RefurbInfo,
    IN ULONG ValueLength,
    BOOLEAN DoWrite
    );

#ifdef __cplusplus
}
#endif

#endif // ! _XCONFIG_H_

