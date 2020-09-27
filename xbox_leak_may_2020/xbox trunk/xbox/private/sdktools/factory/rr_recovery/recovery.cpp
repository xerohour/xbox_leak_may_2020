#include "recovpch.h"
#include "recovstr.h"
#include "xboxverp.h"
#include "xconfig.h"
#include "xboxp.h"
#include "xdisk.h"
#include "smcdef.h"
#include "xboxvideo.h"
#include "imagefile.h"
#include "xcrypt.h"

#define RECOVERYNOFLASH 1

#ifndef RECOVERYNOFLASH

#define ROM_SIZE                    (1024 * 1024)
#define ROM_VERSION_OFFSET          30 // In DWORDs (0x78 is the absolute offset)
#define ROM_VERSION_BYTE_OFFSET     0x78
#define ROM_FLASH_RETRIES           25

//
// Lowest possible base address of flash ROM and region size
//

#define FLASH_BASE_ADDRESS      0xFFF00000
#define FLASH_REGION_SIZE       (0xFFFFFFFF-FLASH_BASE_ADDRESS-1)

#define FlashReadByte(a) \
    (*(PBYTE)((ULONG_PTR)RomBase+(ULONG_PTR)(a)-FLASH_BASE_ADDRESS))

#endif // ! RECOVERYNOFLASH

#define ZERO_KEY "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"


#define QUICK_FORMAT TRUE


extern "C"
{
UCHAR
FASTCALL
RtlFindFirstSetRightMember(
    IN ULONG Set
    );

BOOL
WINAPI
XapiFormatFATVolume(
    POBJECT_STRING pcVolume
    );


BYTE g_InitTableVer = 0;
BOOL g_fInternalCD;

extern DWORD D3D__AvInfo;



}


ULONG g_FilesCopied = 0;

#define APPROX_FILE_COUNT   350

const POSTR g_RootDirExceptions[] =
{
    OTEXT("default.xbe"),
    OTEXT("RETAIL"),
    OTEXT("RECMEDIA"),
    OTEXT("XDASH")
};

const POSTR g_RetailDashExceptions[] =
{
    OTEXT("3dinfo"),
    OTEXT("data"),
    OTEXT("media"),
    OTEXT("menus"),
    OTEXT("sounds"),
    OTEXT("xboxdash.xbe"),
    OTEXT("xshell.xbe"),
    OTEXT("dvdkeyd.bin")
};

CXBoxVideo* g_pVideo;
BitmapFile* g_pBackgroundBitmap;


COBJECT_STRING DVDDevice =         CONSTANT_OBJECT_STRING( OTEXT("\\Device\\Cdrom0") );
COBJECT_STRING PrimaryHarddisk =   CONSTANT_OBJECT_STRING( OTEXT("\\Device\\Harddisk0\\partition0") );
COBJECT_STRING HdPartition1 =      CONSTANT_OBJECT_STRING( OTEXT("\\Device\\Harddisk0\\partition1") );
COBJECT_STRING HdPartition2 =      CONSTANT_OBJECT_STRING( OTEXT("\\Device\\Harddisk0\\partition2") );
COBJECT_STRING CDrive =            CONSTANT_OBJECT_STRING( OTEXT("\\??\\C:") );
COBJECT_STRING YDrive =            CONSTANT_OBJECT_STRING( OTEXT("\\??\\Y:") );

#define DASHBOARD_PARTITION_NUMBER 2

static const CHAR HdPartitionX[] = "\\Device\\Harddisk0\\partition%d";


//
// Functions for dealing with the encrypted section of the EEPROM
//

//
// The EEPROM encrypted section is unencrypted if the hard drive key field is all 0's
//
inline BOOL IsEEPROMEncryptedSectionUnencrypted(const XBOX_ENCRYPTED_SETTINGS* encryptedSection)
{
    const UCHAR* hdkey = encryptedSection->HDKey;
    for (INT i=0; i < XBOX_KEY_LENGTH; i++)
        if (hdkey[i] != 0) return FALSE;
    return TRUE;
}

//
// Decrypted the encrypted section of the EEPROM
//
static DWORD DecryptEEPROMEncryptedSection(XBOX_ENCRYPTED_SETTINGS* encryptedSection)
{
    if (IsEEPROMEncryptedSectionUnencrypted(encryptedSection))
        return ERROR_SUCCESS;

    #define RC4_CONFOUNDER_LEN FIELD_SIZE(XBOX_ENCRYPTED_SETTINGS, Confounder)

    UCHAR Checksum[XC_SERVICE_DIGEST_SIZE];
    UCHAR Confounder[RC4_CONFOUNDER_LEN];
    UCHAR LocalKey[XC_SERVICE_DIGEST_SIZE];
    UCHAR Rc4KeyStruct[XC_SERVICE_RC4_KEYSTRUCT_SIZE];
    UCHAR* Data;
    ULONG DataSize;
    XBOX_ENCRYPTED_SETTINGS EncryptedSettings;
    UCHAR* EepromKey = (UCHAR*) *XboxEEPROMKey;

    EncryptedSettings = *encryptedSection;
    Data = EncryptedSettings.HDKey;
    DataSize = EEPROM_ENCRYPTED_SECTION_SIZE - FIELD_OFFSET(XBOX_ENCRYPTED_SETTINGS, HDKey);
    memcpy(Confounder, EncryptedSettings.Confounder, RC4_CONFOUNDER_LEN);

    //
    // HMAC the checksum into the key
    //
    XcHMAC(
        EepromKey,
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
        EepromKey,
        XBOX_KEY_LENGTH,
        Confounder,
        RC4_CONFOUNDER_LEN,
        Data,
        DataSize,
        Checksum);

    //
    // Decrypt is successful only if checksum matches
    //
    if (memcmp(Checksum, EncryptedSettings.Checksum, XC_SERVICE_DIGEST_SIZE) != 0)
        return ERROR_INVALID_PASSWORD;

    memset(encryptedSection, 0, sizeof(*encryptedSection));
    memcpy(encryptedSection->HDKey, EncryptedSettings.HDKey, XBOX_KEY_LENGTH);
    encryptedSection->GameRegion = EncryptedSettings.GameRegion;
    return ERROR_SUCCESS;
}

//
// Encrypt the encrypted section of the EEPROM
//
static DWORD EncryptEEPROMEncryptedSection(XBOX_ENCRYPTED_SETTINGS* encryptedSection)
{
    if (!IsEEPROMEncryptedSectionUnencrypted(encryptedSection))
        return ERROR_SUCCESS;

    UCHAR Confounder[RC4_CONFOUNDER_LEN] = { 0x4c,0x70,0x33,0xcb,0x5b,0xb5,0x97,0xd2 };
    UCHAR LocalKey[XC_SERVICE_DIGEST_SIZE];
    UCHAR Rc4KeyStruct[XC_SERVICE_RC4_KEYSTRUCT_SIZE];
    UCHAR* Data;
    ULONG DataSize;
    UCHAR* EepromKey = (UCHAR*) *XboxEEPROMKey;

    Data = encryptedSection->HDKey;
    DataSize = EEPROM_ENCRYPTED_SECTION_SIZE - FIELD_OFFSET(XBOX_ENCRYPTED_SETTINGS, HDKey);

    //
    // Create the header - the confounder & checksum
    //
    memset(encryptedSection->Checksum, 0, sizeof(encryptedSection->Checksum));
    memcpy(encryptedSection->Confounder, Confounder, sizeof(encryptedSection->Confounder));

    //
    // Checksum everything but the checksum
    //
    XcHMAC(EepromKey,
           XBOX_KEY_LENGTH,
           encryptedSection->Confounder,
           RC4_CONFOUNDER_LEN,
           Data,
           DataSize,
           encryptedSection->Checksum);

    //
    // HMAC the checksum into the key
    //
    XcHMAC(EepromKey,
           XBOX_KEY_LENGTH,
           encryptedSection->Checksum,
           XC_SERVICE_DIGEST_SIZE,
           NULL,
           0,
           LocalKey);

    //
    // Use the generated key as the RC4 encryption key
    //
    XcRC4Key(Rc4KeyStruct, XC_SERVICE_DIGEST_SIZE, LocalKey);

    //
    // Encrypt everything but the checksum
    //
    XcRC4Crypt(Rc4KeyStruct, RC4_CONFOUNDER_LEN, encryptedSection->Confounder);
    XcRC4Crypt(Rc4KeyStruct, DataSize, Data);

    return ERROR_SUCCESS;
}

//
// Make sure the encrypted section of the EEPROM is encrypted and that there is
// a region set
//
static DWORD PrepareEEPROM(BOOL fRetail)
{
    EEPROM_LAYOUT eepromData;
    ULONG type, length;
    DWORD err;
    XBOX_ENCRYPTED_SETTINGS* encryptedSection =
        (XBOX_ENCRYPTED_SETTINGS*) eepromData.EncryptedSection;

    //
    // Read the existing content of the encrypted section
    //
    err = XQueryValue(XC_MAX_ALL, &type, &eepromData, sizeof(eepromData), &length);

    if (ERROR_SUCCESS != err)
    {
        return err;
    }

    if (!IsEEPROMEncryptedSectionUnencrypted(encryptedSection))
    {
        if (ERROR_SUCCESS != DecryptEEPROMEncryptedSection(encryptedSection))
        {
            memset(encryptedSection, 0, sizeof(*encryptedSection));
        }
    }

    if (0 == (encryptedSection->GameRegion & ~XC_GAME_REGION_MANUFACTURING))
    {
        encryptedSection->GameRegion = XC_GAME_REGION_NA;
    }
    else
    {
        encryptedSection->GameRegion &= ~XC_GAME_REGION_MANUFACTURING;
    }

    //
    // Only encrypt the EEPROM header if this is going to be a retail box
    //
    if (fRetail)
    {
        err = EncryptEEPROMEncryptedSection(encryptedSection);

        if (ERROR_SUCCESS != err)
        {
            return err;
        }
    }

    return XSetValue(XC_MAX_ALL, REG_BINARY, &eepromData, sizeof(eepromData));
}


VOID
DrawMessage(LPCWSTR pcszTextLine1, LPCWSTR pcszTextLine2, int iProgPercent)
{
    ASSERT(g_pVideo && g_pBackgroundBitmap && pcszTextLine1);

    g_pVideo->ClearScreen(COLOR_BLACK);
    g_pBackgroundBitmap->render(g_pVideo->GetD3DDevicePtr());

    g_pVideo->DrawText(SCREEN_X_CENTER,
                       MENUBOX_TEXTAREA_Y1,
                       SCREEN_DEFAULT_TEXT_FG_COLOR,
                       SCREEN_DEFAULT_TEXT_BG_COLOR,
                       pcszTextLine1);

    if (NULL != pcszTextLine2)
    {
        g_pVideo->DrawText(SCREEN_X_CENTER,
                           MENUBOX_TEXTAREA_Y1 + FONT_DEFAULT_HEIGHT + ITEM_VERT_SPACING,
                           SCREEN_DEFAULT_TEXT_FG_COLOR,
                           SCREEN_DEFAULT_TEXT_BG_COLOR,
                           pcszTextLine2);
    }

    if (-1 != iProgPercent)
    {
        //
        // Draw progress bar background
        //

        g_pVideo->DrawBox(PROGRESS_X1 - PROGRESS_BORDER_OUTER_OFFSET,
                          PROGRESS_Y1 - PROGRESS_BORDER_OUTER_OFFSET,
                          PROGRESS_X2 + PROGRESS_BORDER_OUTER_OFFSET,
                          PROGRESS_Y2 + PROGRESS_BORDER_OUTER_OFFSET,
                          COLOR_BRIGHT_GREEN);

        //
        // Draw progress bar bkgnd
        //

        g_pVideo->DrawBox(PROGRESS_X1 - PROGRESS_BORDER_GAP,
                          PROGRESS_Y1 - PROGRESS_BORDER_GAP,
                          PROGRESS_X2 + PROGRESS_BORDER_GAP,
                          PROGRESS_Y2 + PROGRESS_BORDER_GAP,
                          COLOR_BLACK);

        //
        // Draw progress bar
        //

        g_pVideo->DrawBox(PROGRESS_X1,
                          PROGRESS_Y1,
                          PROGRESS_X1 + ((PROGRESS_WIDTH / 100) * min(iProgPercent, 100)),
                          PROGRESS_Y2,
                          SCREEN_DEFAULT_TEXT_FG_COLOR);
    }

    WCHAR szBuildString[64];

    if (g_fInternalCD)
    {
        wsprintfW(szBuildString,
                  RStrings[RS_BUILD_NUMBER_FORMAT_INTERNAL],
                  VER_PRODUCTVERSION_STR,
                  g_InitTableVer);
    }
    else
    {
        wsprintfW(szBuildString,
                  RStrings[RS_BUILD_NUMBER_FORMAT],
                  VER_PRODUCTVERSION_STR);
    }

    g_pVideo->DrawText(SCREEN_X_CENTER,
                       MENUBOX_BUILDNUM_Y1,
                       SCREEN_DEFAULT_TEXT_FG_COLOR,
                       SCREEN_DEFAULT_TEXT_BG_COLOR,
                       szBuildString);

    g_pVideo->ShowScreen();
}


VOID
EjectDVD()
{
    //
    // This requires matching SMC firmware and DVD firmware (DVT3 and onward)..
    //

    HalWriteSMBusByte(SMC_SLAVE_ADDRESS, SMC_COMMAND_DVD_TRAY_OPERATION,
        SMC_DVD_TRAY_OPERATION_OPEN);
}


DWORD ReadSMC(unsigned char addr)
{
    DWORD value = 0xCCCCCCCC;
    DWORD status = HalReadSMBusValue(0x21, addr, FALSE, &value);
    return value;
}


VOID GetSMCVersion(PSTR pszSMCVersion, UINT cchSMCVersion)
{
    ASSERT(cchSMCVersion > 3);

    HalWriteSMBusValue(0x21, 0x01, FALSE, 0);

    pszSMCVersion[0] = (char)ReadSMC(0x01);
    pszSMCVersion[1] = (char)ReadSMC(0x01);
    pszSMCVersion[2] = (char)ReadSMC(0x01);
    pszSMCVersion[3] = '\0';
}

//
// Copies files from a source path to a destination path.  The source path
// is a filter that can be passed to FindFirstFile() and is currently required
// to end in "\\*".  The destination path should end with a trailing backslash ('\')
//

BOOL
CopyFiles(
    IN PCOSTR pcszSrcPath,
    IN PCOSTR pcszDestPath,
    IN BOOL fRootDir,
    IN const POSTR * ppszExceptionList,
    IN UINT cExceptionListItems
    )
{
    WIN32_FIND_DATA wfd;
    BOOL bRet = FALSE;
    int nSrcLen = ocslen(pcszSrcPath);
    int nDestLen = ocslen(pcszDestPath);
    HANDLE hFind = FindFirstFile(pcszSrcPath, &wfd);

    if (NULL == ppszExceptionList)
    {
        ppszExceptionList = g_RootDirExceptions;
        cExceptionListItems = ARRAYSIZE(g_RootDirExceptions);
    }

    if (INVALID_HANDLE_VALUE != hFind)
    {
        OCHAR szSrcFile[MAX_PATH], szDestFile[MAX_PATH];
        ocscpy(szSrcFile, pcszSrcPath);
        ocscpy(szDestFile, pcszDestPath);
        ASSERT((szSrcFile[nSrcLen - 1] == OTEXT('*')) &&
               (szSrcFile[nSrcLen - 2] == OTEXT('\\')));
        do
        {
            BOOL fCopyThis = TRUE;

            if (fRootDir)
            {
                int i;
                for (i = 0; i < (int) cExceptionListItems; i++)
                {
                    if (0 == _ocsicmp(wfd.cFileName, ppszExceptionList[i]))
                    {
                        fCopyThis = FALSE;
                        break;
                    }
                }
            }

            if (!fCopyThis)
            {
                continue;
            }

            lstrcpynO(&(szSrcFile[nSrcLen - 1]),
                      wfd.cFileName,
                      ARRAYSIZE(szSrcFile) - (nSrcLen - 1));
            lstrcpynO(&(szDestFile[nDestLen]),
                      wfd.cFileName,
                      ARRAYSIZE(szDestFile) - nDestLen);

            if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                int nFileLen = ocslen(wfd.cFileName);

                bRet = CreateDirectory(szDestFile, NULL);

                if (!bRet)
                {
                    break;
                }

                lstrcpynO(&(szSrcFile[nSrcLen + nFileLen - 1]),
                          OTEXT("\\*"),
                          ARRAYSIZE(szSrcFile) - (nSrcLen + nFileLen - 1));

                lstrcpynO(&(szDestFile[nDestLen + nFileLen]),
                          OTEXT("\\"),
                          ARRAYSIZE(szDestFile) - (nDestLen + nFileLen));

                bRet = CopyFiles(szSrcFile, szDestFile, FALSE, NULL, 0);

                if (!bRet)
                {
                    break;
                }
            }
            else
            {
                bRet = CopyFile(szSrcFile, szDestFile, FALSE);

                //
                // Files coming off of CD/DVD end up read-only, so clear
                // all of the attributes.. (otherwise dosnet won't work)
                //

                SetFileAttributes(szDestFile, FILE_ATTRIBUTE_NORMAL);

                if (!bRet)
                {
                    break;
                }

                DrawMessage(RStrings[RS_COPYING],
                            NULL,
                            5 + min((++g_FilesCopied * 90) / APPROX_FILE_COUNT, 90));
            }

        } while (FindNextFile(hFind, &wfd));

        FindClose(hFind);
    }
    else if (ERROR_FILE_NOT_FOUND == GetLastError())
    {
        bRet = TRUE;
    }

    return bRet;
}


NTSTATUS
WriteFirstSectors(
    IN HANDLE hDisk
    )
{
    LARGE_INTEGER WriteOffset;
    IO_STATUS_BLOCK ioStatus;
    NTSTATUS status;
    BYTE rgbSectorBytes[XBOX_HD_SECTOR_SIZE];
	BYTE RefurbSectorBytes[XBOX_HD_SECTOR_SIZE];
	XBOX_REFURB_INFO RefurbInfo;
    PXBOX_CONFIG_SECTOR ConfigSector = (PXBOX_CONFIG_SECTOR) rgbSectorBytes;
    int i;

    //
    // Zero out sectors 0-2 and 4-7 (including cache db sector):
    //

    //
    // Sector 3 (XBOX_REFURB_INFO_SECTOR_INDEX) contains the refurb
    // information - leave it alone
    //

    RtlZeroMemory(rgbSectorBytes, sizeof(rgbSectorBytes));

    for (i = XBOX_BOOT_SECTOR_INDEX;
         i < XBOX_REFURB_INFO_SECTOR_INDEX;
         i++)
    {
        WriteOffset.QuadPart = i * XBOX_HD_SECTOR_SIZE;

        NtWriteFile(hDisk,
                    NULL,
                    NULL,
                    NULL,
                    &ioStatus,
                    rgbSectorBytes,
                    XBOX_HD_SECTOR_SIZE,
                    &WriteOffset);
    }

	// reset refurb info

	RefurbInfo.Signature = XBOX_REFURB_INFO_SIGNATURE;
	RefurbInfo.PowerCycleCount = 0;
	RefurbInfo.FirstSetTime.QuadPart = 0;
	// setup sector data with xbox refurb info
	memcpy(rgbSectorBytes,&RefurbInfo,sizeof(XBOX_REFURB_INFO));
	// using the dropout value of i for the calculation
	WriteOffset.QuadPart = i * XBOX_HD_SECTOR_SIZE;
	NtWriteFile(hDisk,
            NULL,
            NULL,
            NULL,
            &ioStatus,
            rgbSectorBytes,
            XBOX_HD_SECTOR_SIZE,
            &WriteOffset);

	// clear rgbsectorbytes again
    RtlZeroMemory(rgbSectorBytes, sizeof(rgbSectorBytes));

    for (i = XBOX_CACHE_DB_SECTOR_INDEX;
         i < XBOX_CONFIG_SECTOR_INDEX;
         i++)
    {
        WriteOffset.QuadPart = i * XBOX_HD_SECTOR_SIZE;

        NtWriteFile(hDisk,
                    NULL,
                    NULL,
                    NULL,
                    &ioStatus,
                    rgbSectorBytes,
                    XBOX_HD_SECTOR_SIZE,
                    &WriteOffset);
    }

    //
    // Preserve config sectors sectors 8-15 (if valid), otherwise zero them:
    //

    for (i = XBOX_CONFIG_SECTOR_INDEX;
         i < (XBOX_CONFIG_SECTOR_INDEX + XBOX_NUM_CONFIG_SECTORS);
         i++)
    {
        WriteOffset.QuadPart = i * XBOX_HD_SECTOR_SIZE;

        status = NtReadFile(hDisk,
                            NULL,
                            NULL,
                            NULL,
                            &ioStatus,
                            ConfigSector,
                            XBOX_HD_SECTOR_SIZE,
                            &WriteOffset);

        if (NT_SUCCESS(status) &&
            (ConfigSector->SectorBeginSignature == XBOX_CONFIG_SECTOR_BEGIN_SIGNATURE) &&
            (ConfigSector->SectorEndSignature == XBOX_CONFIG_SECTOR_END_SIGNATURE) &&
            (XConfigChecksum(ConfigSector, sizeof(*ConfigSector)) == 0xffffffff))
        {
            XDBGWRN("RECOVERY", "Preserving config sector %d.", i);
        }
        else
        {
            XDBGWRN("RECOVERY", "Invalid config sector %d - not preserving.", i);
            RtlZeroMemory(ConfigSector, XBOX_HD_SECTOR_SIZE);

            NtWriteFile(hDisk,
                        NULL,
                        NULL,
                        NULL,
                        &ioStatus,
                        ConfigSector,
                        XBOX_HD_SECTOR_SIZE,
                        &WriteOffset);
        }
    }

    return status;
}


//
// Reset user settings in EEPROM to their defaults (all zeros)
//

NTSTATUS ResetEEPROMUserSettings()
{
    BYTE buf[EEPROM_TOTAL_MEMORY_SIZE];
    EEPROM_LAYOUT* eeprom;
    XBOX_USER_SETTINGS* userSettings;
    NTSTATUS status;
    DWORD type, size;

    eeprom = (EEPROM_LAYOUT*) buf;
    userSettings = (XBOX_USER_SETTINGS*) &eeprom->UserConfigSection;
    status = ExQueryNonVolatileSetting(XC_MAX_ALL, &type, buf, sizeof(buf), &size);

    if (NT_SUCCESS(status))
    {
        RtlZeroMemory(userSettings, sizeof(*userSettings));
        userSettings->Checksum = ~XConfigChecksum(userSettings, sizeof(*userSettings));
        status = ExSaveNonVolatileSetting(XC_MAX_ALL, REG_BINARY, buf, sizeof(buf));
    }

    return status;
}




//
// Generate the default EEPROM image
//  We assume the output buffer has already been
//  zero-initialized by the caller.
//
VOID DefaultNonVolatileSettings(EEPROM_LAYOUT* eeprom)
{
    XBOX_FACTORY_SETTINGS* factorySettings;
    XBOX_USER_SETTINGS* userSettings;
    LARGE_INTEGER systime;
    UCHAR* enetaddr;
    ULONG tick0;

    // Pick a random Ethernet address in the 1M range
    // of the 00-50-f2 address block. Only even addresses
    // are used here.
    __asm {
        rdtsc
        mov tick0, eax
    }
    KeQuerySystemTime(&systime);
    tick0 = (tick0 ^ systime.LowPart) & 0xffffe;

    factorySettings = (XBOX_FACTORY_SETTINGS*) &eeprom->FactorySection;
    userSettings = (XBOX_USER_SETTINGS*) &eeprom->UserConfigSection;

    enetaddr = factorySettings->EthernetAddr;
    enetaddr[0] = 0x00;
    enetaddr[1] = 0x50;
    enetaddr[2] = 0xf2;
    enetaddr[3] = (UCHAR) (tick0 >> 16);
    enetaddr[4] = (UCHAR) (tick0 >>  8);
    enetaddr[5] = (UCHAR) (tick0      );

    // The first 32 addresses are reserved.
    enetaddr[5] |= 0x20;

    factorySettings->AVRegion = AV_STANDARD_NTSC_M | AV_FLAGS_60Hz;
    factorySettings->Checksum = ~XConfigChecksum(factorySettings, sizeof(*factorySettings));

    userSettings->Checksum = ~XConfigChecksum(userSettings, sizeof(*userSettings));
}

//
// Verify the non-volatile settings are valid
// and initialize them to default values if necessary.
//
NTSTATUS CheckNonVolatileSettings()
{
    BYTE buf[EEPROM_TOTAL_MEMORY_SIZE];
    EEPROM_LAYOUT* eeprom;
    XBOX_FACTORY_SETTINGS* factorySettings;
    XBOX_USER_SETTINGS* userSettings;
    NTSTATUS status;
    DWORD type, size;

    eeprom = (EEPROM_LAYOUT*) buf;
    factorySettings = (XBOX_FACTORY_SETTINGS*) &eeprom->FactorySection;
    userSettings = (XBOX_USER_SETTINGS*) &eeprom->UserConfigSection;
    status = ExQueryNonVolatileSetting(XC_MAX_ALL, &type, buf, sizeof(buf), &size);

    if (NT_SUCCESS(status) &&
        XConfigChecksum(factorySettings, sizeof(*factorySettings)) == 0xffffffff &&
        XConfigChecksum(userSettings, sizeof(*userSettings)) == 0xffffffff) {
        XDBGWRN("RECOVERY", "Preserving EEPROM settings.");
    } else if (XboxHardwareInfo->GpuRevision == 0xa1) {
        XDBGWRN("RECOVERY", "Initializing EEPROM settings (DVT2) ...");
        memset(buf, 0, sizeof(buf));

        DefaultNonVolatileSettings(eeprom);
        status = ExSaveNonVolatileSetting(XC_MAX_ALL, REG_BINARY, buf, sizeof(buf));

        if (!NT_SUCCESS(status)) {
            XDBGWRN("RECOVERY", "Failed to initialized EEPROM settings: %x.", status);
        }
    } else {
        XDBGWRN("RECOVERY", "!!!!!!!!!! Corrupted EEPROM on DVT3 box !!!!!!!!!!");
    }

    return status;
}


// Threshold for analog buttons
#define XINPUT_ANALOGBUTTONTHRESHOLD 32

//
// This function returns with *pdwNewVideoMode set to 0xFFFFFFFF if a regular
// controller button is pressed.
//
// It returns with *pdwNewVideoMode set to a valid video mode if a video mode
// change button sequence has been pressed:
// (both triggers + right thumbstick + some other button)
//

VOID
WaitForAnyButton(PDWORD pdwNewVideoMode, PBOOL pfYButton)
{
    BOOL fButtonPressed = FALSE;
    static DWORD dwPads = 0;
    DWORD dwInsertions, dwRemovals;
    static HANDLE hPads[XGetPortCount()] = { 0 };
    int i;

    ASSERT(pdwNewVideoMode);
    *pdwNewVideoMode = 0xFFFFFFFF;

    while (!fButtonPressed)
    {
        if (XGetDeviceChanges(XDEVICE_TYPE_GAMEPAD, &dwInsertions, &dwRemovals))
        {
            dwPads |= dwInsertions;
            dwPads &= ~dwRemovals;

            for (i = 0; i < XGetPortCount(); i++)
            {
                if ((1 << i) & dwRemovals)
                {
                    if (NULL != hPads[i])
                    {
                        XInputClose(hPads[i]);
                        hPads[i] = NULL;
                    }
                }

                if ((1 << i) & dwInsertions)
                {
                    if (NULL == hPads[i])
                    {
                        hPads[i] = XInputOpen(XDEVICE_TYPE_GAMEPAD, XDEVICE_PORT0 + i, XDEVICE_NO_SLOT, NULL);;
                    }
                }
            }
        }

        for (i = 0; i < XGetPortCount(); i++)
        {
            if ((1 << i) & dwPads)
            {
                if (NULL != hPads[i])
                {
                    XINPUT_STATE State;

                    if (ERROR_SUCCESS == XInputGetState(hPads[i], &State))
                    {
                        if ((State.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER] > XINPUT_ANALOGBUTTONTHRESHOLD) &&
                            (State.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_RIGHT_TRIGGER] > XINPUT_ANALOGBUTTONTHRESHOLD))
                        {
                            if (State.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP)
                            {
                                *pdwNewVideoMode = AV_STANDARD_PAL_I | AV_FLAGS_50Hz;
                            }
                            else if (State.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT)
                            {
                                *pdwNewVideoMode = AV_STANDARD_NTSC_M | AV_FLAGS_60Hz;
                            }
                            else if (State.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT)
                            {
                                *pdwNewVideoMode = AV_STANDARD_NTSC_J | AV_FLAGS_60Hz;
                            }
                        }
                        else if ((0 != ((XINPUT_GAMEPAD_START | XINPUT_GAMEPAD_BACK) & State.Gamepad.wButtons)) ||
                                 (State.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_A] > XINPUT_ANALOGBUTTONTHRESHOLD) ||
                                 (State.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_B] > XINPUT_ANALOGBUTTONTHRESHOLD) ||
                                 (State.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_X] > XINPUT_ANALOGBUTTONTHRESHOLD) ||
                                 (State.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_Y] > XINPUT_ANALOGBUTTONTHRESHOLD) ||
                                 (State.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_BLACK] > XINPUT_ANALOGBUTTONTHRESHOLD) ||
                                 (State.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_WHITE] > XINPUT_ANALOGBUTTONTHRESHOLD))
                        {
                            fButtonPressed = TRUE;
                            
                            if (pfYButton)
                            {
                                *pfYButton = (State.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_Y] >
                                                XINPUT_ANALOGBUTTONTHRESHOLD);
                            }
                        }
                    }
                }
            }

            if (fButtonPressed || (*pdwNewVideoMode != 0xFFFFFFFF))
            {
                fButtonPressed = TRUE;
                break;
            }
        }
    }
}



#define FAIL_RECOVERY(code)                                    \
{                                                              \
    WCHAR szMsg[128];                                          \
    wsprintfW(szMsg, RStrings[RS_FAILURE_LINE2_FORMAT], code); \
    DrawMessage(RStrings[RS_FAILURE_LINE1], szMsg, -1);        \
    XDBGWRN("RECOVERY", "Recovery failed (err 0x%08x)", code); \
    IDirect3DDevice8* pDevice = g_pVideo->GetD3DDevicePtr();   \
    ASSERT(pDevice);                                           \
    pDevice->BlockUntilIdle();                                 \
    Sleep(INFINITE);                                           \
    return;                                                    \
}




#define MAX_SECS_PER_VERIFY 256

typedef
VOID
(CALLBACK *PFORMAT_PROGRESS_ROUTINE)(
    UINT uPercent
    );

BOOL
WINAPI
RecFormatFATVolume(
    POBJECT_STRING VolumePath,
    BOOL fQuickFormat,
    PFORMAT_PROGRESS_ROUTINE pFPR
    )
{
    NTSTATUS status;
    OBJECT_ATTRIBUTES ObjectAttributes;
    IO_STATUS_BLOCK IoStatusBlock;
    HANDLE VolumeHandle;
    DISK_GEOMETRY DiskGeometry;
    ULONG SectorShift;
    ULONG StructureAlignment;
    PARTITION_INFORMATION PartitionInformation;
    LARGE_INTEGER ByteOffset;

    if (pFPR)
    {
        pFPR(0);
    }

    InitializeObjectAttributes(&ObjectAttributes, VolumePath,
        OBJ_CASE_INSENSITIVE, NULL, NULL);

    if (!XapiFormatFATVolume(VolumePath))
    {
        return FALSE;
    }

    //
    // Open the volume for read/write access.
    //

    status = NtOpenFile(&VolumeHandle, SYNCHRONIZE | FILE_READ_DATA |
        FILE_WRITE_DATA, &ObjectAttributes, &IoStatusBlock, 0,
        FILE_SYNCHRONOUS_IO_ALERT | FILE_NO_INTERMEDIATE_BUFFERING);

    if (!NT_SUCCESS(status)) {
        // XapiSetLastNTError(status);
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
        // XapiSetLastNTError(status);
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
        // XapiSetLastNTError(status);
        return FALSE;
    }

    //
    // Compute the alignment between the various file system structures.
    // Everything should be page aligned in order to maximum file system cache
    // efficiency.
    //

    StructureAlignment = ROUND_TO_PAGES(DiskGeometry.BytesPerSector);

    //
    // Now do the verify...
    //

    {
        DWORD dwAllSectors = (DWORD) PartitionInformation.PartitionLength.QuadPart >> SectorShift;
        DWORD dwSecsPerVerify = min((dwAllSectors / 20) + 1, MAX_SECS_PER_VERIFY);
        DWORD dwPercent = 0;

        // Do a full verify of the disk

        for (DWORD i = 0; i < dwAllSectors; i += dwSecsPerVerify)
        {
            VERIFY_INFORMATION vi;

            if ((i * 100) / dwAllSectors > dwPercent)
            {
                dwPercent = ((i * 100) / dwAllSectors);
                XDBGTRC("RECOVERY", "Format is %ld%% complete", dwPercent);
                if (pFPR)
                {
                    pFPR(dwPercent);
                }
            }

            vi.StartingOffset.QuadPart = i << SectorShift;
            vi.Length = min(dwSecsPerVerify, dwAllSectors - i) << SectorShift;

            ASSERT(dwSecsPerVerify < (0xFFFFFFFF >> SectorShift));

            status = NtDeviceIoControlFile(
                        VolumeHandle,
                        NULL,
                        NULL,
                        NULL,
                        &IoStatusBlock,
                        IOCTL_DISK_VERIFY,
                        &vi,
                        sizeof(vi),
                        NULL,
                        0
                        );

            if (!NT_SUCCESS(status))
            {
                XDBGERR("RECOVERY",
                        "NtDeviceIoControlFile() IOCTL_DISK_VERIFY failed - st=0x%08X",
                        status);
                break;
            }

            if ((0 == i) && fQuickFormat)
            {
                //
                // NOTE: modifying loop counter, ugly, I know.  But this is a
                // convenient way to make sure that quickformat verifies the
                // beginning and the end of the disk..
                //
                i = max(i + dwSecsPerVerify, dwAllSectors - dwSecsPerVerify);
            }
        }

        NtClose(VolumeHandle);

        if (!NT_SUCCESS(status))
        {
            return FALSE;
        }
    }

    if (pFPR)
    {
        pFPR(100);
    }

    return TRUE;
}

//
// Index of the partition that is currently being formatted (1-based)
//

UINT g_CurrentFormatPartition;


VOID
CALLBACK
RecFormatProgress(
    UINT uPercent
    )
{
    UINT uPartitions = (XDISK_FIRST_CACHE_PARTITION - 1) + *HalDiskCachePartitionCount;
    DrawMessage(RStrings[RS_PREPARING],
                NULL,
                ((100 / uPartitions) * (g_CurrentFormatPartition - 1)) +
                    (uPercent / uPartitions));
}

BOOL HDSetPassword( BYTE* password, DWORD dwLen )
{
    NTSTATUS status;
    HANDLE h;
    OBJECT_ATTRIBUTES obja;
    IO_STATUS_BLOCK iosb;
    OBJECT_STRING name;
    ULONG AtaPassThroughSize;
    ATA_PASS_THROUGH AtaPassThrough;
    IDE_SECURITY_DATA SecurityData;
    BOOL fReturn;
    DWORD cbBytes;

    RtlInitObjectString(&name, "\\Device\\Harddisk0\\Partition0");
    InitializeObjectAttributes(&obja, &name, 0, NULL, 0);

    status = NtOpenFile(&h, GENERIC_READ | GENERIC_WRITE | SYNCHRONIZE, &obja, &iosb,
        FILE_SHARE_WRITE | FILE_SHARE_READ, FILE_SYNCHRONOUS_IO_NONALERT);

    if (NT_SUCCESS(status)) {

        RtlZeroMemory(&AtaPassThrough, sizeof(ATA_PASS_THROUGH));
        RtlZeroMemory(&SecurityData, sizeof(IDE_SECURITY_DATA));

        AtaPassThrough.IdeReg.bHostSendsData = TRUE;
        AtaPassThrough.IdeReg.bCommandReg = IDE_COMMAND_SECURITY_SET_PASSWORD;
        AtaPassThrough.DataBufferSize = sizeof(IDE_SECURITY_DATA);
        AtaPassThrough.DataBuffer = &SecurityData;
        SecurityData.Maximum = TRUE;
        SecurityData.Master = FALSE;

        RtlCopyMemory(SecurityData.Password, password, dwLen);

        fReturn = DeviceIoControl(h, IOCTL_IDE_PASS_THROUGH,
            &AtaPassThrough, sizeof(ATA_PASS_THROUGH),
            &AtaPassThrough, sizeof(ATA_PASS_THROUGH),
            &cbBytes, NULL);

        if (!fReturn) {
            XDBGTRC("HDSetPassword", "API error!");
            return FALSE;
        } else if (AtaPassThrough.IdeReg.bCommandReg & 1) {
            XDBGTRC("HDSetPassword", "IDE error!");
            return FALSE;
        }

        NtClose(h);
    }

    return TRUE;
}



void __cdecl main()
{
    IO_STATUS_BLOCK statusBlock;
    DISK_GEOMETRY diskGeometry;
    OBJECT_ATTRIBUTES oa;
    HANDLE hDisk;
    NTSTATUS Status;
    int i;
    

    WCHAR szText[128];
    DWORD dwVideoFlags;
    BOOL fRetail = FALSE;
    UINT WaitStep = 0;
    BYTE FinalHardDriveKey[XC_SERVICE_DIGEST_SIZE];
    BOOL fDiskHasRetail = (0xFFFFFFFF != GetFileAttributes("D:\\RETAIL"));
    DWORD dwNV2ARevision = *((DWORD*)0xFD000000);
    CHAR szSMCVersion[4];

    GetSMCVersion(szSMCVersion, ARRAYSIZE(szSMCVersion));
    
    g_fInternalCD = (0xFFFFFFFF != GetFileAttributes("D:\\DEVKIT\\DXT\\CYDRIVE.DXT"));

    //
    // Initialize core peripheral port support
    //
    XInitDevices(0,NULL);

    CheckNonVolatileSettings();


    g_pVideo = new CXBoxVideo;
    g_pBackgroundBitmap = new BitmapFile;

    if ((NULL == g_pVideo) || (NULL == g_pBackgroundBitmap))
    {
        XDBGERR("RECOVERY", "Couldn't allocate video objects");
        Sleep(INFINITE);
    }

    g_pBackgroundBitmap->read(0, 0, FILE_DATA_IMAGE_DIRECTORY_A "\\rrbkgrnd.bmp");

    g_pVideo->Initialize( SCREEN_WIDTH, SCREEN_HEIGHT );

    //
    // Init table version < 0x40: Old DVT 3 Boxes with NV2A A02 old metal 7 (NVCLK 155, CPU 665)
    // Init table version = 0x40: New DVT 3 Boxes with NV2A A02 new metal 7 (Faster)
    // Init table version >= 0x46: DVT4
    //

    //
    // SMC Versions starting with 'B' are no longer supported (DVT3=B2A, DVT4=B3B, etc.)
    //
    // DVT5/XDK2 and greater start with 'D' or 'P' (i.e. DVT5=DXB)
    //
/* allow DVT 4 boxes for now
    if ((g_InitTableVer < 0x46) ||
        ((dwNV2ARevision & 0xFF) == 0xA1) ||
        ((dwNV2ARevision & 0xFF) == 0xA2) ||
        (szSMCVersion[0] == 'B'))

     if ((g_InitTableVer < 0x46) ||
        ((dwNV2ARevision & 0xFF) == 0xA1) ||
        ((dwNV2ARevision & 0xFF) == 0xA2))
    {
        DrawMessage(RStrings[RS_FAILURE_LINE1], RStrings[RS_FAILURE_LINE2_OLD_BOX], -1);
        XDBGWRN("RECOVERY", "Recovery failed because this Xbox is no longer supported");

        XDBGWRN("RECOVERY", "Versions: 0x%02x, 0x%02x, %s",
                g_InitTableVer, (dwNV2ARevision & 0xFF), szSMCVersion);

        IDirect3DDevice8* pDevice = g_pVideo->GetD3DDevicePtr();
        ASSERT(pDevice);
        pDevice->BlockUntilIdle();

        Sleep(INFINITE);
        return;
    }
*/
    DrawMessage(RStrings[RS_GAMEPAD_BEGIN_LINE1], RStrings[RS_GAMEPAD_BEGIN_LINE2], -1);

    while (WaitStep < 2)
    {
        WaitForAnyButton(&dwVideoFlags, &fRetail);

        if (0xFFFFFFFF != dwVideoFlags)
        {
            //
            // Reset video flags and re-init
            //

            ExSaveNonVolatileSetting(XC_FACTORY_AV_REGION, REG_DWORD, &dwVideoFlags, sizeof(dwVideoFlags));

            D3D__AvInfo = 0;

            g_pVideo->DeInitialize();

            g_pVideo->Initialize( SCREEN_WIDTH, SCREEN_HEIGHT );

            g_pVideo->ClearScreen(COLOR_BLACK);
            g_pBackgroundBitmap->render(g_pVideo->GetD3DDevicePtr());
            g_pVideo->ShowScreen();
        }
        else
        {
            WaitStep++;
            if (!fDiskHasRetail)
            {
                fRetail = FALSE;
                break;
            }
        }

        //
        // Wait for the button to go back up before asking for another button press
        //
        
        if ((WaitStep > 0) && (WaitStep < 2))
        {
            Sleep(200);
        }
        
        DrawMessage(RStrings[(0 == WaitStep) ? RS_GAMEPAD_BEGIN_LINE1 : RS_RETAIL_LINE1],
                    RStrings[(0 == WaitStep) ? RS_GAMEPAD_BEGIN_LINE2 : RS_RETAIL_LINE2],
                    -1);

    }

    if (ERROR_SUCCESS != PrepareEEPROM(fRetail))
    {
        XDBGWRN("RECOVERY", "Recovery failed because it could not prepare the EEPROM");
        FAIL_RECOVERY(25);
    }



    DrawMessage(RStrings[RS_PREPARING], NULL, 0);



    Status = ResetEEPROMUserSettings();

    if (!NT_SUCCESS(Status))
    {
        XDBGERR("RECOVERY", "Dash recovery failed because ResetEEPROMUserSettings() failed");
        HalReturnToFirmware(HalFatalErrorRebootRoutine);
    }


    //
    // Open a handle to the entire hard disk (aka Partition0)
    //

    InitializeObjectAttributes(&oa,
                               (POBJECT_STRING) &PrimaryHarddisk,
                               OBJ_CASE_INSENSITIVE,
                               NULL,
                               NULL);

    Status = NtOpenFile(&hDisk,
                        SYNCHRONIZE | GENERIC_ALL,
                        &oa,
                        &statusBlock,
                        0,
                        FILE_SYNCHRONOUS_IO_ALERT);

    if (!NT_SUCCESS(Status))
    {
        XDBGERR("RECOVERY", "Could not open hard disk %Z (0x%08x).", &PrimaryHarddisk, Status);
        FAIL_RECOVERY(13);
    }

    Status = WriteFirstSectors(hDisk);

    if (!NT_SUCCESS(Status))
    {
        XDBGERR("RECOVERY",
                "WriteFirstSectors() failed (0x%08x) for hard disk %Z.",
                Status,
                &PrimaryHarddisk);
        FAIL_RECOVERY(14);
    }

    NtClose(hDisk);

    XDBGTRC("RECOVERY", "Formatting hard disk partitions...");

    for (g_CurrentFormatPartition = 1;
         g_CurrentFormatPartition < XDISK_FIRST_CACHE_PARTITION + *HalDiskCachePartitionCount;
         g_CurrentFormatPartition++)
    {
        OCHAR szPartition[MAX_PATH];
        OBJECT_STRING PartitionString;


        _snoprintf(szPartition,
                   ARRAYSIZE(szPartition),
                   HdPartitionX,
                   g_CurrentFormatPartition);

        RtlInitObjectString(&PartitionString, szPartition);

        if (!RecFormatFATVolume(
                &PartitionString,
                QUICK_FORMAT,
                RecFormatProgress))
        {
            XDBGERR("RECOVERY", "Could not format hard disk partition %d.", g_CurrentFormatPartition);
            FAIL_RECOVERY(100 + g_CurrentFormatPartition);
        }
    }



    DrawMessage(RStrings[RS_COPYING], NULL, 5);

    //
    // Give the main partition a drive letter
    //

    if (!NT_SUCCESS(IoCreateSymbolicLink((POBJECT_STRING) &CDrive, (POBJECT_STRING) &HdPartition1)))
    {
        XDBGERR("RECOVERY", "Could not assign %Z to %Z", &CDrive, &HdPartition1);
        FAIL_RECOVERY(15);
    }

    if (fRetail)
    {
        if (!CreateDirectory("C:\\TDATA", NULL))
        {
            FAIL_RECOVERY(22);
        }
        
        if (!CreateDirectory("C:\\UDATA", NULL))
        {
            FAIL_RECOVERY(23);
        }
    }
    else
    {
        if (!CopyFiles(OTEXT("D:\\*"), OTEXT("C:\\"), TRUE, NULL, 0))
        {
            XDBGERR("RECOVERY", "Could not copy files to the hard disk (main partition)");
            FAIL_RECOVERY(16);
        }
    }

    //
    // Give the dash partition a drive letter
    //

    if (!NT_SUCCESS(IoCreateSymbolicLink((POBJECT_STRING) &YDrive, (POBJECT_STRING) &HdPartition2)))
    {
        XDBGERR("RECOVERY", "Could not assign %Z to %Z", &YDrive, &HdPartition2);
        FAIL_RECOVERY(17);
    }

    if (!CopyFiles(OTEXT("D:\\XDASH\\*"),
                   OTEXT("Y:\\"),
                   fRetail,
                   fRetail ? g_RetailDashExceptions : NULL,
                   fRetail ? ARRAYSIZE(g_RetailDashExceptions) : 0))
    {
        XDBGERR("RECOVERY", "Could not copy files to the hard disk (dash partition)");
        FAIL_RECOVERY(18);
    }

    if (fRetail)
    {
        //
        // Copy over the retail dash
        //

        if (!CopyFile("D:\\RETAIL\\xboxdash.xbe", "Y:\\xboxdash.xbe", FALSE))
        {
            FAIL_RECOVERY(24);
        }
    }

    if ((XboxHardwareInfo->Flags & XBOX_HW_FLAG_DEVKIT_KERNEL) != 0)
    {
        ASSERT( RtlEqualMemory( ZERO_KEY, (LPBYTE)(*XboxHDKey), XBOX_KEY_LENGTH ) );
        RtlZeroMemory( (LPBYTE)(*XboxHDKey), XBOX_KEY_LENGTH );
    }

    //
    // Compute Key to use to lock hard drive
    //
    XcHMAC( (LPBYTE)(*XboxHDKey), XBOX_KEY_LENGTH,
            (LPBYTE)HalDiskModelNumber->Buffer, HalDiskModelNumber->Length,
            (LPBYTE)HalDiskSerialNumber->Buffer, HalDiskSerialNumber->Length,
            FinalHardDriveKey );

    HDSetPassword( FinalHardDriveKey, XC_SERVICE_DIGEST_SIZE );

    DrawMessage(RStrings[RS_COPYING], NULL, 100);

    if ((XboxHardwareInfo->Flags & XBOX_HW_FLAG_DEVKIT_KERNEL) != 0)
    {
        XDBGTRC("RECOVERY", "\n\nPlease remove the recovery CD and then press any gamepad button to reboot...\n");

        DrawMessage(RStrings[RS_GAMEPAD_END_LINE1], RStrings[RS_GAMEPAD_END_LINE2], -1);

        EjectDVD();
    }
    else
    {
        //
        // We can't eject the CD if we're running on the retail kernel, so we have to ask the
        // user to eject instead...
        //
        
        XDBGTRC("RECOVERY", "\n\nPlease eject and remove the recovery CD and then press any gamepad button to reboot...\n");

        DrawMessage(RStrings[RS_GAMEPAD_END_RETAIL], NULL, -1);
    }

    WaitForAnyButton(&dwVideoFlags, NULL);

    if (0xFFFFFFFF != dwVideoFlags)
    {
        //
        // Reset video flags before rebooting
        //

        ExSaveNonVolatileSetting(XC_FACTORY_AV_REGION, REG_DWORD, &dwVideoFlags, sizeof(dwVideoFlags));
    }

    XDBGWRN("RECOVERY", "Recovery completed successfully.");

    HalReturnToFirmware( HalRebootRoutine );
}


