#include "recovpch.h"
#include "recovstr.h"
#include "xboxverp.h"
#include "xconfig.h"
#include "xboxp.h"
#include "xdisk.h"
#include "smcdef.h"
#include <xonlinep.h>


#ifdef HDSETUPAPP
#define HD_KEY "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"

typedef struct
{
    UINT FirstSector;
    UINT LastSector;
} HD_SECTORRANGE, *PHD_SECTORRANGE;

//
// Sector ranges used by the greystone duplicator when hard drives are imaged.
// These should be zero initialized prior to preparing a hard drive image.
//

HD_SECTORRANGE HardDriveSectorRanges[] =
{
    { 0x00000000, 0x000003FF }, // (reserved sectors)
    { 0x00465400, 0x0055F3FF }, // (dash partition)
    { 0x0055F400, 0x00567D97 }, // (data partition + 16 megabytes of files)
    { 0, 0 }
};

#endif // HDSETUPAPP

#ifdef RECOVERYAPP
#include "xboxvideo.h"
#include "imagefile.h"
#include "xcrypt.h"
#endif // RECOVERYAPP

#ifdef DASHLIB
#define RECOVERYNOFLASH 1
#endif // DASHLIB

#ifndef RECOVERYNOFLASH

#define ROM_SIZE                    (1024 * 1024)
#define ROM_VERSION_OFFSET          30 // In DWORDs (0x78 is the absolute offset)
#define ROM_VERSION_BYTE_OFFSET     0x78
#define ROM_VERSION_KDDELAY_FLAG    0x80000000
#define ROM_FLASH_RETRIES           25

//
// Lowest possible base address of flash ROM and region size
//

#define FLASH_BASE_ADDRESS      0xFFF00000
#define FLASH_REGION_SIZE       (0xFFFFFFFF-FLASH_BASE_ADDRESS-1)

#define FlashReadByte(a) \
    (*(PBYTE)((ULONG_PTR)RomBase+(ULONG_PTR)(a)-FLASH_BASE_ADDRESS))
#define FlashReadUlong(a) \
    (*(PULONG)((ULONG_PTR)RomBase+(ULONG_PTR)(a)-FLASH_BASE_ADDRESS))

#endif // ! RECOVERYNOFLASH

#define ZERO_KEY "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"

#ifdef RECOVERYAPP

#define QUICK_FORMAT TRUE

#endif // RECOVERYAPP

#ifdef DASHLIB

#define QUICK_FORMAT FALSE

#endif // DASHLIB

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

#ifdef RECOVERYAPP

ULONG g_InitTableVer = 0;
BOOL g_fInternalCD;

#ifdef RECOVERY_KERNEL_VER
USHORT RecoveryVersion = RECOVERY_KERNEL_VER;
#else
USHORT RecoveryVersion = VER_PRODUCTBUILD;
#endif

extern DWORD D3D__AvInfo;

#endif // RECOVERYAPP

#ifdef DASHLIB

typedef VOID (CALLBACK *PFNRECPROGRESSCALLBACK)(UINT uPercent, ULONG_PTR Context);

ULONG WINAPI DashRecovery(PFNRECPROGRESSCALLBACK pfnCallback, ULONG_PTR Context);

#endif // DASHLIB

}

#ifdef RECOVERYAPP

ULONG g_FilesCopied = 0;

#if defined(ONLINEBETA) || defined(HDSETUPAPP)
#define APPROX_FILE_COUNT   150
#else
#define APPROX_FILE_COUNT   350
#endif

const POSTR g_RootDirExceptions[] =
{
    OTEXT("default.xbe"),
#if !defined(RECOVERYNOFLASH) || defined(HDSETUPAPP)
    OTEXT("xboxrom.bin"),
#endif // !defined(RECOVERYNOFLASH) || defined(HDSETUPAPP)
    OTEXT("xboxrom_dvt4.bin"),
    OTEXT("xboxrom_qt.bin"),
#ifdef ONLINEBETA
	OTEXT("xboxrom_beta.bin"),
	OTEXT("xbdm.dll"),
	OTEXT("beta.dat"),
#endif
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

#endif // RECOVERYAPP

COBJECT_STRING DVDDevice =         CONSTANT_OBJECT_STRING( OTEXT("\\Device\\Cdrom0") );
COBJECT_STRING PrimaryHarddisk =   CONSTANT_OBJECT_STRING( OTEXT("\\Device\\Harddisk0\\partition0") );
COBJECT_STRING HdPartition1 =      CONSTANT_OBJECT_STRING( OTEXT("\\Device\\Harddisk0\\partition1") );
COBJECT_STRING HdPartition2 =      CONSTANT_OBJECT_STRING( OTEXT("\\Device\\Harddisk0\\partition2") );
COBJECT_STRING CDrive =            CONSTANT_OBJECT_STRING( OTEXT("\\??\\C:") );
COBJECT_STRING YDrive =            CONSTANT_OBJECT_STRING( OTEXT("\\??\\Y:") );

#define DASHBOARD_PARTITION_NUMBER 2

static const CHAR HdPartitionX[] = "\\Device\\Harddisk0\\partition%d";

#if defined(RECOVERYAPP) && !defined(HDSETUPAPP)

//
// Functions for dealing with the encrypted section of the EEPROM
//

//
// The EEPROM encrypted section is unencrypted if the confounder field is all 0's
//
inline BOOL IsEEPROMEncryptedSectionUnencrypted(const XBOX_ENCRYPTED_SETTINGS* encryptedSection)
{
    const UCHAR* conf = encryptedSection->Confounder;
    for (INT i=0; i < 8; i++)
        if (conf[i] != 0) return FALSE;
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

#ifdef ONLINEBETA

static UCHAR NewEEPROMKey[16];

typedef struct _XC_ENCFILE_HEADER {
	DWORD Sig;
	UCHAR MAC[XC_SERVICE_DIGEST_SIZE];
	UCHAR Confounder[RC4_CONFOUNDER_LEN];
	DWORD MsgLen;
	UCHAR unused[XC_SERVICE_DIGEST_SIZE];
} XC_ENCFILE_HEADER, *PXC_ENCFILE_HEADER;

static BOOL FLoadNewEEPROMKey(LPSTR szFilename, LPSTR szPasswd)
{
    HANDLE hFile = INVALID_HANDLE_VALUE;
    DWORD dwBytes;
    BOOL fRet = FALSE;
    DWORD dwEncSize;
    PBYTE pEncBuffer = FALSE;
    PXC_ENCFILE_HEADER pEncHead;
    UCHAR Checksum[XC_SERVICE_DIGEST_SIZE];
    UCHAR Confounder[RC4_CONFOUNDER_LEN];
    UCHAR LocalKey[XC_SERVICE_DIGEST_SIZE];
    UCHAR Rc4KeyStruct[XC_SERVICE_RC4_KEYSTRUCT_SIZE];
	UCHAR rgbEncryptedKey[sizeof(XC_ENCFILE_HEADER) + 76];
	int cbPasswd;

    hFile = CreateFile(szFilename, GENERIC_READ, FILE_SHARE_READ, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if (hFile == INVALID_HANDLE_VALUE)
        goto CleanupAndExit;

    dwEncSize = GetFileSize(hFile, NULL);
    if (dwEncSize != sizeof rgbEncryptedKey)
        goto CleanupAndExit;
    
    pEncHead = (PXC_ENCFILE_HEADER)rgbEncryptedKey;

    if (!ReadFile(hFile, rgbEncryptedKey, dwEncSize, &dwBytes, NULL))
        goto CleanupAndExit;

    if (pEncHead->Sig != 'k2ne' || pEncHead->MsgLen != (dwEncSize -
		sizeof(XC_ENCFILE_HEADER))) {
        goto CleanupAndExit;
    }

    //
    // Decrypt it
    //

	cbPasswd = strlen(szPasswd);
	XcHMAC((PUCHAR)szPasswd, cbPasswd, pEncHead->MAC, XC_SERVICE_DIGEST_SIZE,
		NULL, 0, LocalKey);
    XcRC4Key(Rc4KeyStruct, XC_SERVICE_DIGEST_SIZE, LocalKey);
    XcRC4Crypt(Rc4KeyStruct, RC4_CONFOUNDER_LEN, pEncHead->Confounder);
    XcRC4Crypt(Rc4KeyStruct, pEncHead->MsgLen, (PUCHAR)(pEncHead + 1));
    XcHMAC((PUCHAR)szPasswd, cbPasswd, pEncHead->Confounder, RC4_CONFOUNDER_LEN,
		(PUCHAR)(pEncHead + 1),  pEncHead->MsgLen, Checksum);
    if (memcmp(Checksum, pEncHead->MAC, XC_SERVICE_DIGEST_SIZE) != 0)
        goto CleanupAndExit;

	//
	// At this point, assume the key is bytes 52-67 past the encrypted key
	// header -- this matches the xcrypt symmetric key format
	//

	memcpy(NewEEPROMKey, &rgbEncryptedKey[sizeof(XC_ENCFILE_HEADER) + 52],
		16);
	fRet = TRUE;

CleanupAndExit:
    if (hFile != INVALID_HANDLE_VALUE) {
        CloseHandle(hFile);
    }

    return fRet;
}

#endif // ONLINEBETA

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
#ifdef ONLINEBETA
    UCHAR* EepromKey = NewEEPROMKey;
#else
    UCHAR* EepromKey = (UCHAR*) *XboxEEPROMKey;
#endif

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
static DWORD PrepareEEPROM()
{
    EEPROM_LAYOUT eepromData;
    ULONG type, length;
    DWORD err;
    XBOX_ENCRYPTED_SETTINGS* encryptedSection =
        (XBOX_ENCRYPTED_SETTINGS*) eepromData.EncryptedSection;
	XBOX_USER_SETTINGS* userSettings =
		(XBOX_USER_SETTINGS*) eepromData.UserConfigSection;

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
    // Always encrypt the EEPROM header (all post-July releases support this)
    //
    err = EncryptEEPROMEncryptedSection(encryptedSection);

    if (ERROR_SUCCESS != err)
    {
        return err;
    }

#ifdef ONLINEBETA
	// We need to zero out the user EEPROM settings as well
    RtlZeroMemory(userSettings, sizeof(*userSettings));
    userSettings->Checksum = ~XConfigChecksum(userSettings, sizeof(*userSettings));
#endif // ONLINEBETA

    return XSetValue(XC_MAX_ALL, REG_BINARY, &eepromData, sizeof(eepromData));
}


#endif // defined(RECOVERYAPP) && !defined(HDSETUPAPP)


#ifdef RECOVERYAPP

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

#ifdef ONLINEBETA
    wsprintfW(szBuildString,
              RStrings[RS_BUILD_NUMBER_FORMAT_INTERNAL],
              VER_PRODUCTVERSION_STR,
              g_InitTableVer & 0xFF);
#else
    if (g_fInternalCD)
    {
        wsprintfW(szBuildString,
                  RStrings[RS_BUILD_NUMBER_FORMAT_INTERNAL],
                  VER_PRODUCTVERSION_STR,
                  g_InitTableVer & 0xFF);
    }
    else
    {
        wsprintfW(szBuildString,
                  RStrings[RS_BUILD_NUMBER_FORMAT],
                  VER_PRODUCTVERSION_STR);
    }
#endif

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




#ifdef SECTORCOPY

#define RECASSERT(cond) do { \
            if (!(cond)) { \
                DbgPrint("ASSERT on line %d: %s\n", __LINE__, #cond); \
                DrawMessage(L"ASSERT", L ## #cond, 0); \
                DebugBreak(); \
            } \
        } while (0)

#define SECTORSIZE 512
#define MAXBUFSIZE (64*1024)
ULONG SectorRanges[] = {
    0x00000000, 0x000003FF,
    0x00465400, 0x0055F3FF,
    0x0055F400, 0x00567D97,
};

VOID ReadDiskSnapshot()
{
    OBJECT_STRING objname;
    OBJECT_ATTRIBUTES oa;
    IO_STATUS_BLOCK iosb;
    NTSTATUS status;
    HANDLE disk;

    RtlInitObjectString(&objname, OTEXT("\\Device\\Harddisk0\\partition0"));
    InitializeObjectAttributes(&oa, &objname, OBJ_CASE_INSENSITIVE, NULL, NULL);

    status = NtOpenFile(
                &disk,
                GENERIC_ALL|SYNCHRONIZE,
                &oa,
                &iosb,
                0,
                FILE_SYNCHRONOUS_IO_ALERT);

    RECASSERT(NT_SUCCESS(status));

    VOID* buf = VirtualAlloc(NULL, MAXBUFSIZE, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
    RECASSERT(buf != NULL);

    HANDLE file = CreateFile(
                    "z:\\snapshot.dat",
                    GENERIC_WRITE,
                    0,
                    NULL,
                    CREATE_ALWAYS,
                    FILE_ATTRIBUTE_NORMAL|FILE_FLAG_SEQUENTIAL_SCAN,
                    NULL);

    RECASSERT(file != INVALID_HANDLE_VALUE);

    // Write out the first sector
    memset(buf, 0, MAXBUFSIZE);
    memcpy(buf, SectorRanges, sizeof(SectorRanges));

    DWORD cb;
    BOOL writeok = WriteFile(file, buf, SECTORSIZE, &cb, NULL);
    RECASSERT(writeok);

    ULONG* p = SectorRanges;
    ULONG range_count = sizeof(SectorRanges) / (sizeof(ULONG)*2);
    ULONG total = 0;

    while (range_count--) {
        ULONG sector = p[0];
        ULONG sector_count = p[1] - p[0] + 1;
        p += 2;
        DbgPrint("Copying %d sectors starting from %x\n", sector_count, sector);

        total += sector_count;
        while (sector_count) {
            ULONG n = min(sector_count, MAXBUFSIZE/SECTORSIZE);
            LARGE_INTEGER offset;

            offset.QuadPart = sector;
            offset.QuadPart *= SECTORSIZE;
            status = NtReadFile(disk, 0, NULL, NULL, &iosb, buf, n*SECTORSIZE, &offset);
            RECASSERT(NT_SUCCESS(status));

            writeok = WriteFile(file, buf, n*SECTORSIZE, &cb, NULL);
            RECASSERT(writeok);

            sector += n;
            sector_count -= n;
        }
    }

    VirtualFree(buf, 0, MEM_RELEASE);
    CloseHandle(file);
    NtClose(disk);

    DbgPrint("Total of %d sectors copied\n", total);
}

VOID WriteDiskSnapshot()
{
    OBJECT_STRING objname;
    OBJECT_ATTRIBUTES oa;
    IO_STATUS_BLOCK iosb;
    NTSTATUS status;
    HANDLE disk;

    RtlInitObjectString(&objname, OTEXT("\\Device\\Harddisk0\\partition0"));
    InitializeObjectAttributes(&oa, &objname, OBJ_CASE_INSENSITIVE, NULL, NULL);

    status = NtOpenFile(
                &disk,
                GENERIC_ALL|SYNCHRONIZE,
                &oa,
                &iosb,
                0,
                FILE_SYNCHRONOUS_IO_ALERT);

    RECASSERT(NT_SUCCESS(status));

    VOID* buf = VirtualAlloc(NULL, MAXBUFSIZE, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
    RECASSERT(buf != NULL);

    HANDLE file = CreateFile(
                    "D:\\snapshot.dat",
                    GENERIC_READ,
                    0,
                    NULL,
                    OPEN_EXISTING,
                    FILE_ATTRIBUTE_NORMAL|FILE_FLAG_SEQUENTIAL_SCAN,
                    NULL);

    RECASSERT(file != INVALID_HANDLE_VALUE);

    DWORD cb;
    BOOL readok = ReadFile(file, buf, SECTORSIZE, &cb, NULL);
    RECASSERT(readok);
    RECASSERT(memcmp(buf, SectorRanges, sizeof(SectorRanges)) == 0);

    ULONG* p = SectorRanges;
    ULONG range_count = sizeof(SectorRanges) / (sizeof(ULONG)*2);
    ULONG total = 0;

    while (range_count--) {
        ULONG sector = p[0];
        ULONG sector_count = p[1] - p[0] + 1;
        p += 2;
        DbgPrint("Copying %d sectors starting from %x\n", sector_count, sector);

        total += sector_count;
        while (sector_count) {
            ULONG n = min(sector_count, MAXBUFSIZE/SECTORSIZE);
            LARGE_INTEGER offset;
            offset.QuadPart = sector;
            offset.QuadPart *= SECTORSIZE;

            readok = ReadFile(file, buf, n*SECTORSIZE, &cb, NULL);
            RECASSERT(readok);

            status = NtWriteFile(disk, NULL, NULL, NULL, &iosb, buf, n*SECTORSIZE, &offset);
            RECASSERT(NT_SUCCESS(status));

            sector += n;
            sector_count -= n;
        }
    }

    VirtualFree(buf, 0, MEM_RELEASE);
    CloseHandle(file);
    NtClose(disk);

    DbgPrint("Total of %d sectors copied\n", total);
}

#endif // SECTORCOPY


BOOL AreOnlineSectorsOk(PBYTE pbMachineAcctSectorData)
{
    XC_ONLINE_MACHINE_ACCOUNT_STRUCT* pMachineAccount = 
            (XC_ONLINE_MACHINE_ACCOUNT_STRUCT*) pbMachineAcctSectorData;

    ASSERT(pbMachineAcctSectorData);

    if (0 != pMachineAccount->xuid.qwUserID)
    {
        return TRUE;
    }

    return FALSE;
}


#endif // RECOVERYAPP

#define XBOX_MACHINE_ACCT_SECTOR  (XBOX_CONFIG_SECTOR_INDEX + 1)
#define XBOX_USER_SECTOR1         (XBOX_CONFIG_SECTOR_INDEX + 2)
#define XBOX_USER_SECTOR2         (XBOX_CONFIG_SECTOR_INDEX + 3)


NTSTATUS
WriteFirstSectors(
    IN HANDLE hDisk
    )
{
    LARGE_INTEGER WriteOffset;
    IO_STATUS_BLOCK ioStatus;
    NTSTATUS status = STATUS_SUCCESS;
    BYTE rgbSectorBytes[XBOX_HD_SECTOR_SIZE];
    PXBOX_CONFIG_SECTOR ConfigSector = (PXBOX_CONFIG_SECTOR) rgbSectorBytes;
    int i;

    BOOL fPreserveOnlineSectors = TRUE;

    RtlZeroMemory(rgbSectorBytes, sizeof(rgbSectorBytes));

#ifdef HDSETUPAPP

    //
    // Zero out all sectors used by the greystone duplicator before writing anything
    //

    for (i = 0;
         (HardDriveSectorRanges[i].FirstSector && HardDriveSectorRanges[i].LastSector);
         i++)
    {
        UINT j;

        for (j = HardDriveSectorRanges[i].FirstSector;
             j <= HardDriveSectorRanges[i].LastSector;
             j++)
        {
            WriteOffset.QuadPart = j * XBOX_HD_SECTOR_SIZE;

            NtWriteFile(hDisk,
                        NULL,
                        NULL,
                        NULL,
                        &ioStatus,
                        rgbSectorBytes,
                        XBOX_HD_SECTOR_SIZE,
                        &WriteOffset);
        }
    }

#endif // HDSETUPAPP

    //
    // Zero out sectors 0-2 and 4-7 (including cache db sector):
    //

    //
    // Sector 3 (XBOX_REFURB_INFO_SECTOR_INDEX) contains the refurb
    // information - leave it alone
    //

    //
    // In HDSETUP, we clear out all of the sectors, refurb, config, and all
    //

    for (i = XBOX_BOOT_SECTOR_INDEX;
#if defined(HDSETUPAPP) || defined(ONLINEBETA)
         i < (XBOX_CONFIG_SECTOR_INDEX + XBOX_NUM_CONFIG_SECTORS);
#else  // HDSETUPAPP
         i < XBOX_REFURB_INFO_SECTOR_INDEX;
#endif // HDSETUPAPP
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

#if !defined(HDSETUPAPP) && !defined(ONLINEBETA)
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
            (XConfigChecksum(ConfigSector, sizeof(*ConfigSector)) == 0xffffffff)
#ifdef RECOVERYAPP
            &&
            ((XBOX_MACHINE_ACCT_SECTOR != i) || (fPreserveOnlineSectors = AreOnlineSectorsOk(ConfigSector->Data))) &&
            (((XBOX_USER_SECTOR1 != i) && (XBOX_USER_SECTOR2 != i)) || fPreserveOnlineSectors))
#else  // RECOVERYAPP
            )
#endif // RECOVERYAPP
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
#endif // !HDSETUPAPP

    return status;
}

#ifdef DASHLIB

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

#endif // DASHLIB

#ifdef RECOVERYAPP

#ifdef CHECK_EEPROM_SETTINGS

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

#endif // CHECK_EEPROM_SETTINGS

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

VOID
WaitForButtonPress(
    PBOOL YButton
    )
{
    ULONG VideoMode;

    //
    // Keep processing button presses.  If the video mode changes, then
    // deal with it.
    //

    for(;;) {
        WaitForAnyButton(&VideoMode, YButton);
        if(VideoMode == 0xFFFFFFFF)
            return;

        //
        // Reset video flags and re-init
        //

        ExSaveNonVolatileSetting(XC_FACTORY_AV_REGION, REG_DWORD, &VideoMode,
            sizeof(VideoMode));

        D3D__AvInfo = 0;

        g_pVideo->DeInitialize();

        g_pVideo->Initialize( SCREEN_WIDTH, SCREEN_HEIGHT );

        g_pVideo->ClearScreen(COLOR_BLACK);
        g_pBackgroundBitmap->render(g_pVideo->GetD3DDevicePtr());
        g_pVideo->ShowScreen();
    }
}

#endif // RECOVERYAPP

#ifdef RECOVERYAPP

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

#endif // RECOVERYAPP

#ifndef RECOVERYNOFLASH

VOID
DetectInitTableVersion(
    VOID
    )
{
    PVOID RomBase;

    RomBase = MmMapIoSpace( FLASH_BASE_ADDRESS, FLASH_REGION_SIZE,
                                  PAGE_READWRITE | PAGE_NOCACHE );
    if (NULL == RomBase) {
        XDBGERR("RECOVERY", "Could not map ROM");
        FAIL_RECOVERY(4);
    }
    g_InitTableVer = FlashReadUlong(FLASH_BASE_ADDRESS +
        ROM_VERSION_BYTE_OFFSET);
    XDBGWRN("RECOVERY", "Detected init table version 0x%08x", g_InitTableVer);
    MmUnmapIoSpace( RomBase, FLASH_REGION_SIZE );
}

VOID
UpdateROMImage(
    BOOL RetailImage
    )
{
    BOOL ExternalROM;
    BOOL UpdateNeeded;
    BOOL YButton;
    PUCHAR ROMImage;
    PUCHAR ROMData;
    ULONG SizeOfDataReturned;
    PSTR ROMFileName;
    HANDLE ROMFileHandle;

#ifdef ONLINEBETA
	UpdateNeeded = TRUE;
#else

    //
    // Detect the current ROM version and flavor to see whether we need to
    // update it.
    //

    ExternalROM = (g_InitTableVer & ROM_VERSION_KDDELAY_FLAG) != 0;
    if(RetailImage && (XboxHardwareInfo->Flags & XBOX_HW_FLAG_DEVKIT_KERNEL))
        UpdateNeeded = TRUE;
    else if(!RetailImage && !(XboxHardwareInfo->Flags & XBOX_HW_FLAG_DEVKIT_KERNEL))
        UpdateNeeded = TRUE;
    else if(ExternalROM && g_fInternalCD)
        UpdateNeeded = TRUE;
    else if(!ExternalROM && !g_fInternalCD)
        UpdateNeeded = TRUE;
    else
        UpdateNeeded = ((g_InitTableVer >> 16) & 0x7FFF) < RecoveryVersion;
#endif

    //
    // If the ROM is current and we're doing internal recovery, we offer the
    // option to update anyway.
    //

    if(!UpdateNeeded) {
        if(!g_fInternalCD)
            return;
        DrawMessage(RStrings[RS_OVERWRITEROM_LINE1],
            RStrings[RS_OVERWRITEROM_LINE2], -1);
        Sleep(200);
        WaitForButtonPress(&YButton);
        if(!YButton)
            return;
    }

    //
    // Load the ROM image
    //

    ROMImage = (PUCHAR) LocalAlloc(LMEM_FIXED, ROM_SIZE);
    if (NULL == ROMImage) {
        XDBGERR("RECOVERY", "Could not allocate ROM image memory");
        FAIL_RECOVERY(1);
    }

#ifdef ONLINEBETA
    if ((g_InitTableVer & 0xFF) == 0x46 || (g_InitTableVer & 0xFF) == 0x70) 
        ROMFileName = NULL;
    else
        ROMFileName = "D:\\xboxrom_beta.bin";
#else
    if ((g_InitTableVer & 0xFF) == 0x46)
        ROMFileName = RetailImage ? "D:\\RETAIL\\xboxrom_dvt4.bin" : "D:\\xboxrom_dvt4.bin";
    else if ((g_InitTableVer & 0xFF) == 0x70) 
        ROMFileName = RetailImage ? "D:\\RETAIL\\xboxrom_qt.bin" : "D:\\xboxrom_qt.bin";
    else
        ROMFileName = RetailImage ? "D:\\RETAIL\\xboxrom.bin" : "D:\\xboxrom.bin";
#endif

    ROMFileHandle = CreateFile(ROMFileName, GENERIC_READ, 0, NULL,
        OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    //
    // If the ROM image is missing from the drive, we need to advise the user,
    // but not fail recovery.
    //

    if (INVALID_HANDLE_VALUE == ROMFileHandle) {
        DWORD dwErr = GetLastError();
        XDBGWRN("RECOVERY", "Could not open ROM image file (%s), err=0x%08x", ROMFileName, dwErr);
#ifdef ONLINEBETA
		FAIL_RECOVERY(3);
#else
        DrawMessage(RStrings[RS_NEWERROM_LINE1],
            RStrings[RS_NEWERROM_LINE2], -1);
        WaitForButtonPress(&YButton);
        LocalFree(ROMImage);
        return;
#endif
    }

    //
    // Read in the ROM
    //

    ROMData = ROMImage;
    while (ReadFile(ROMFileHandle, ROMData, ROM_SIZE - (ROMData - ROMImage),
        &SizeOfDataReturned, NULL) && (0 != SizeOfDataReturned))
    {
        ROMData += SizeOfDataReturned;
    }
    CloseHandle(ROMFileHandle);

    if (ROM_SIZE != (ROMData - ROMImage)) {
        XDBGERR("RECOVERY", "Could not read ROM image file, read %d out of %d bytes",
            (ROMData - ROMImage), ROM_SIZE);
        FAIL_RECOVERY(3);
    }

    DrawMessage(RStrings[RS_FLASH_LINE1], RStrings[RS_FLASH_LINE2], -1);

    IDirect3DDevice8* pDevice = g_pVideo->GetD3DDevicePtr();
    ASSERT(pDevice);
    pDevice->BlockUntilIdle();

    XDBGWRN("RECOVERY", "Starting ROM Flash...");

    HRESULT hrFlash;

    for (int i = 0; i < ROM_FLASH_RETRIES; i++)
    {
        CHAR szResp[128];
        hrFlash = FlashKernelImage(ROMImage,
                                   ROM_SIZE,
                                   szResp,
                                   ARRAYSIZE(szResp));

        XDBGWRN("RECOVERY", "Flash Status (%d): hr=0x%08x (%s)", i, hrFlash, szResp);

        if (SUCCEEDED(hrFlash))
        {
            break;
        }
    }

    if (ROM_FLASH_RETRIES == i)
    {
        XDBGERR("RECOVERY", "Could not flash ROM, hr=0x%08x", hrFlash);
        FAIL_RECOVERY(hrFlash);
    }

    LocalFree(ROMImage);
}

#endif // RECOVERYNOFLASH

#ifdef DASHLIB

#define FAIL_RECOVERY(code)                                    \
{                                                              \
    XDBGWRN("RECOVERY", "Recovery failed (err 0x%08x)", code); \
    return code;                                               \
}

#endif // DASHLIB

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

#ifdef RECOVERYAPP

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

#ifndef INT2RETAIL
#ifndef HDSETUPAPP
    {
    //
    // Now that we've reset the HD password, we need to make sure the EEPROM
    // has the correct key
    //
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
        return FALSE;
    }

    //
    // By now, the EEPROM should have been encrypted
    //
    if (ERROR_SUCCESS != DecryptEEPROMEncryptedSection(encryptedSection))
    {
        return FALSE;
    }

    //
    // Copy in the HD key we used to lock the HD
    //
    memcpy(encryptedSection->HDKey, *XboxHDKey, XBOX_KEY_LENGTH);

    //
    // Reencrypt the EEPROM header
    //
    err = EncryptEEPROMEncryptedSection(encryptedSection);

    if (ERROR_SUCCESS != err)
    {
        return FALSE;
    }

    err = XSetValue(XC_MAX_ALL, REG_BINARY, &eepromData, sizeof(eepromData));

    if (ERROR_SUCCESS != err)
    {
        return FALSE;
    }
    }
#endif // HDSETUPAPP
#endif // INT2RETAIL

    return TRUE;
}



BOOL HDDisablePassword( BYTE* password, DWORD dwLen )
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
        AtaPassThrough.IdeReg.bCommandReg = IDE_COMMAND_SECURITY_DISABLE_PASSWORD;
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
            XDBGTRC("HDDisablePassword", "API error!");
            return FALSE;
        } else if (AtaPassThrough.IdeReg.bCommandReg & 1) {
            XDBGTRC("HDDisablePassword", "IDE error!");
            return FALSE;
        }

        NtClose(h);
    }

    return TRUE;
}




#ifdef INT2RETAIL
static 
void 
ProgramEEPROM()
{
    EEPROM_LAYOUT ExistingEEPROM;
    EEPROM_LAYOUT NewEEPROM;
    ULONG type;
    ULONG dwBytes;
    DWORD dwRetVal;
    HANDLE hFile;
    XBOX_FACTORY_SETTINGS* pNewFactorySettings;
    XBOX_FACTORY_SETTINGS* pExistingFactorySettings;

    DrawMessage(L"Programming EEPROM", L"Please wait", -1);

    //
    // Read the eeprom image from disk
    //
    hFile = CreateFile("D:\\eeprom.bin", GENERIC_READ, 0, NULL, OPEN_EXISTING, 
        FILE_ATTRIBUTE_NORMAL, NULL);

    if (hFile == INVALID_HANDLE_VALUE) {
        FAIL_RECOVERY(31);
    }

    if (!ReadFile(hFile, &NewEEPROM, sizeof(NewEEPROM), &dwBytes, NULL) || dwBytes != 256) {
        CloseHandle(hFile);
        FAIL_RECOVERY(32);
    }
    CloseHandle(hFile);

    //
    // Read the existing EEPROM
    //
    dwRetVal = XQueryValue(XC_MAX_ALL, &type, &ExistingEEPROM, sizeof(ExistingEEPROM), &dwBytes);
    if (dwRetVal != ERROR_SUCCESS) {
        FAIL_RECOVERY(33);
    }

    //
    // Copy the ehternet address from existing to new
    //

    pNewFactorySettings = (XBOX_FACTORY_SETTINGS*)&NewEEPROM.FactorySection;
    pExistingFactorySettings = (XBOX_FACTORY_SETTINGS*)&ExistingEEPROM.FactorySection;

    memcpy(pNewFactorySettings->EthernetAddr, pExistingFactorySettings->EthernetAddr, 6);

    //
    // Calculate the checksum
    //
    pNewFactorySettings->Checksum = 0;
    pNewFactorySettings->Checksum = ~XConfigChecksum(pNewFactorySettings, sizeof(XBOX_FACTORY_SETTINGS));

    //
    // Write out the new EEPROM
    //
    dwRetVal = XSetValue(XC_MAX_ALL, REG_BINARY, &NewEEPROM, sizeof(NewEEPROM));
    if (dwRetVal != ERROR_SUCCESS) {
        FAIL_RECOVERY(34);
    }

}

#endif  // INT2RETAIL


#endif // RECOVERYAPP

#ifdef DASHLIB

static PFNRECPROGRESSCALLBACK g_spDashCallback = NULL;
static ULONG_PTR g_DashContext = NULL;

VOID
CALLBACK
RecFormatProgress(
    UINT uPercent
    )
{
    //
    // Subtract out the dashboard partition (partition2)
    //

    UINT uPartitions = (XDISK_FIRST_CACHE_PARTITION - 1) + *HalDiskCachePartitionCount - 1;
    UINT uCurPartition = g_CurrentFormatPartition;

    if (uCurPartition >= DASHBOARD_PARTITION_NUMBER)
    {
        uCurPartition--;
    }


    UINT uOverallPercent = ((100 / uPartitions) * (uCurPartition - 1)) + (uPercent / uPartitions);

    XDBGWRN("RECOVERY",
            "Recovery format is %d%% complete",
            uOverallPercent);

    if (g_spDashCallback)
    {
        g_spDashCallback(uOverallPercent, g_DashContext);
    }
}

#endif // DASHLIB

#ifdef RECOVERYAPP

BOOL IsEEPROMKeyAllZeros()
{
    int i;
    UCHAR* EepromKey = (UCHAR*) *XboxEEPROMKey;

    //
    // Check to see if the EEPROM key is zero (which happens after warm reboots).
    //
    // If this is the case, we can't run recovery
    // (which needs to decrypt/encrypt the EEPROM)
    //

    for (i = 0; i < XBOX_KEY_LENGTH; i++)
    {
        if (0 != EepromKey[i])
        {
            return FALSE;
        }
    }

    return TRUE;
}

#endif // RECOVERYAPP

#ifdef RECOVERYAPP
void __cdecl main()
#endif // RECOVERYAPP
#ifdef DASHLIB
extern "C"
{
ULONG DashRecovery(PFNRECPROGRESSCALLBACK pfnCallback, ULONG_PTR Context)
#endif // DASHLIB
{
    IO_STATUS_BLOCK statusBlock;
    DISK_GEOMETRY diskGeometry;
    OBJECT_ATTRIBUTES oa;
    HANDLE hDisk;
    NTSTATUS Status;
    int i;
    
#ifdef DASHLIB
    g_spDashCallback = pfnCallback;
    g_DashContext = Context;
#endif // DASHLIB

#ifdef RECOVERYAPP
    WCHAR szText[128];
    DWORD dwVideoFlags;
    BOOL fRetail = FALSE;
    UINT WaitStep = 0;
    BYTE FinalHardDriveKey[XC_SERVICE_DIGEST_SIZE];
    BOOL fDiskHasRetail = (0xFFFFFFFF != GetFileAttributes("D:\\RETAIL"));
    DWORD dwNV2ARevision = *((DWORD*)0xFD000000);
    CHAR szSMCVersion[4];

    GetSMCVersion(szSMCVersion, ARRAYSIZE(szSMCVersion));
    
#ifdef ONLINEBETA
	g_fInternalCD = fDiskHasRetail = FALSE;
#else
    g_fInternalCD = (0xFFFFFFFF != GetFileAttributes("D:\\DEVKIT\\DXT\\CYDRIVE.DXT"));
#endif

    //
    // Initialize core peripheral port support
    //
    XInitDevices(0,NULL);

#ifdef CHECK_EEPROM_SETTINGS
    CheckNonVolatileSettings();
#endif

    g_pVideo = new CXBoxVideo;
    g_pBackgroundBitmap = new BitmapFile;

    if ((NULL == g_pVideo) || (NULL == g_pBackgroundBitmap))
    {
        XDBGERR("RECOVERY", "Couldn't allocate video objects");
        Sleep(INFINITE);
    }

    g_pBackgroundBitmap->read(0, 0, FILE_DATA_IMAGE_DIRECTORY_A "\\backgrnd.bmp");

    g_pVideo->Initialize( SCREEN_WIDTH, SCREEN_HEIGHT );

#ifndef HDSETUPAPP

    DetectInitTableVersion();

    //
    // Init table version < 0x40: Old DVT 3 Boxes with NV2A A02 old metal 7 (NVCLK 155, CPU 665)
    // Init table version = 0x40: New DVT 3 Boxes with NV2A A02 new metal 7 (Faster)
    // Init table version = 0x46: DVT4, DVT5, XDK2 (FAB F)
    // Init table version = 0x60: DVT6+, XDK2 (FAB G/H)
    // Init table version = 0x70: QT (MCP C03)
    //

    //
    // SMC Versions starting with 'B' are no longer supported (DVT3=B2A, DVT4=B3B, etc.)
    //
    // DVT5/XDK2 and greater start with 'D' or 'P' (i.e. DVT5=DXB)
    //
    
    if (((g_InitTableVer & 0xFF) < 0x46) ||
        ((dwNV2ARevision & 0xFF) == 0xA1) ||
        ((dwNV2ARevision & 0xFF) == 0xA2) ||
        (szSMCVersion[0] == 'B'))
    {
        DrawMessage(RStrings[RS_FAILURE_LINE1], RStrings[RS_FAILURE_LINE2_OLD_BOX], -1);
        XDBGWRN("RECOVERY", "Recovery failed because this Xbox is no longer supported");

        XDBGWRN("RECOVERY", "Versions: 0x%08x, 0x%02x, %s",
                g_InitTableVer, (dwNV2ARevision & 0xFF), szSMCVersion);

        IDirect3DDevice8* pDevice = g_pVideo->GetD3DDevicePtr();
        ASSERT(pDevice);
        pDevice->BlockUntilIdle();

        Sleep(INFINITE);
        return;
    }

    if (IsEEPROMKeyAllZeros())
    {
        DrawMessage(RStrings[RS_FAILURE_LINE1], RStrings[RS_FAILURE_LINE2_WARM_BOOT], -1);
        XDBGWRN("RECOVERY", "Recovery failed because this Xbox needs to have been cold booted");

        IDirect3DDevice8* pDevice = g_pVideo->GetD3DDevicePtr();
        ASSERT(pDevice);
        pDevice->BlockUntilIdle();

        Sleep(INFINITE);
        return;
    }

#endif // ! HDSETUPAPP

#ifdef HDSETUPAPP
    if ((XboxKrnlVersion->Build < 3944) && (XboxKrnlVersion->Major == 1) && (XboxKrnlVersion->Minor == 0))
    {
        DrawMessage(RStrings[RS_FAILURE_LINE1], RStrings[RS_FAILURE_LINE2_OLD_BOX], -1);
        XDBGWRN("RECOVERY", "Recovery failed because this Xbox is no longer supported");

        IDirect3DDevice8* pDevice = g_pVideo->GetD3DDevicePtr();
        ASSERT(pDevice);
        pDevice->BlockUntilIdle();

        Sleep(INFINITE);
        return;
    }
#endif // HDSETUPAPP

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
#ifndef HDSETUPAPP
            if (!fDiskHasRetail)
            {
                fRetail = FALSE;
                break;
            }
#endif // HDSETUPAPP
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

#ifdef ONLINEBETA
	if(!FLoadNewEEPROMKey("D:\\BETA.DAT", "Beta2002_key1set")) {
		XDBGWRN("RECOVERY", "Recovery failed because it could not load the new EEPROM key");
		FAIL_RECOVERY(25);
	}
#endif

#ifndef HDSETUPAPP
    if (ERROR_SUCCESS != PrepareEEPROM())
    {
        XDBGWRN("RECOVERY", "Recovery failed because it could not prepare the EEPROM");
        FAIL_RECOVERY(25);
    }
#endif // HDSETUPAPP

#ifndef RECOVERYNOFLASH
    UpdateROMImage(fRetail);
#endif

    DrawMessage(RStrings[RS_PREPARING], NULL, 0);

#endif // RECOVERYAPP



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

#ifdef DASHLIB
        //
        // If we're running recovery from within the dash, don't reformat the dash partition:
        //

        if (DASHBOARD_PARTITION_NUMBER == g_CurrentFormatPartition)
        {
            continue;
        }
#endif // DASHLIB

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

#ifdef DASHLIB

    //
    // We're assuming that the dash already has the C: drive mapped
    //
    
    if (!CreateDirectory("C:\\TDATA", NULL))
    {
        FAIL_RECOVERY(20);
    }
    
    if (!CreateDirectory("C:\\UDATA", NULL))
    {
        FAIL_RECOVERY(21);
    }

    Status = ResetEEPROMUserSettings();

    if (!NT_SUCCESS(Status))
    {
        XDBGERR("RECOVERY", "Dash recovery failed because ResetEEPROMUserSettings() failed");
        HalReturnToFirmware(HalFatalErrorRebootRoutine);
    }

#endif // DASHLIB

#ifdef RECOVERYAPP

    DrawMessage(RStrings[RS_COPYING], NULL, 5);

    
    //
    // Give the main partition a drive letter
    //

    if (!NT_SUCCESS(IoCreateSymbolicLink((POBJECT_STRING) &CDrive, (POBJECT_STRING) &HdPartition1)))
    {
        XDBGERR("RECOVERY", "Could not assign %Z to %Z", &CDrive, &HdPartition1);
        FAIL_RECOVERY(15);
    }

#ifdef SECTORCOPY

    WriteDiskSnapshot();

#else  // SECTORCOPY

#ifndef HDSETUPAPP
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
#endif // HDSETUPAPP
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
#ifdef HDSETUPAPP
                   FALSE, NULL, 0))
#else
                   fRetail,
                   fRetail ? g_RetailDashExceptions : NULL,
                   fRetail ? ARRAYSIZE(g_RetailDashExceptions) : 0))
#endif // HDSETUPAPP
    {
        XDBGERR("RECOVERY", "Could not copy files to the hard disk (dash partition)");
        FAIL_RECOVERY(18);
    }

#ifndef HDSETUPAPP

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

#endif // HDSETUPAPP

#endif // SECTORCOPY

#ifdef INT2RETAIL
    //
    // Unlock the hard drive
    //
    XcHMAC( (LPBYTE)(*XboxHDKey), XBOX_KEY_LENGTH,
        (LPBYTE)HalDiskModelNumber->Buffer, HalDiskModelNumber->Length,
        (LPBYTE)HalDiskSerialNumber->Buffer, HalDiskSerialNumber->Length,
        FinalHardDriveKey ) ;
    
    HDDisablePassword(FinalHardDriveKey, XC_SERVICE_DIGEST_SIZE);

    //
    // Program the EEPROM
    //
    ProgramEEPROM();

    //
    // Lock the HDD
    //

    XcHMAC( (LPBYTE)"INT2RETAILHDKEY", XBOX_KEY_LENGTH,
        (LPBYTE)HalDiskModelNumber->Buffer, HalDiskModelNumber->Length,
        (LPBYTE)HalDiskSerialNumber->Buffer, HalDiskSerialNumber->Length,
        FinalHardDriveKey ) ;
 
    if (!HDSetPassword( FinalHardDriveKey, XC_SERVICE_DIGEST_SIZE )) {
        XDBGERR("RECOVERY", "Could not lock HD");
        FAIL_RECOVERY(26);
    }


#else // INT2RETAIL


#ifdef HDSETUPAPP

    //
    // Compute Key to use to lock hard drive
    //
    if(fRetail) {
        /* We're locking, so use the HD_KEY */
        XcHMAC( (LPBYTE) HD_KEY, XBOX_KEY_LENGTH,
            (LPBYTE)HalDiskModelNumber->Buffer, HalDiskModelNumber->Length,
            (LPBYTE)HalDiskSerialNumber->Buffer, HalDiskSerialNumber->Length,
            FinalHardDriveKey );
    } else {
        /* We're unlocking, so use the EEPROM key */
        XcHMAC( (LPBYTE)(*XboxHDKey), XBOX_KEY_LENGTH,
            (LPBYTE)HalDiskModelNumber->Buffer, HalDiskModelNumber->Length,
            (LPBYTE)HalDiskSerialNumber->Buffer, HalDiskSerialNumber->Length,
            FinalHardDriveKey ) ;
        /* Now perform the unlock */
        HDDisablePassword(FinalHardDriveKey, XC_SERVICE_DIGEST_SIZE);
    }

#else  // HDSETUPAPP

#ifndef ONLINEBETA
    if ((XboxHardwareInfo->Flags & XBOX_HW_FLAG_DEVKIT_KERNEL) != 0)
    {
        RtlZeroMemory( (LPBYTE)(*XboxHDKey), XBOX_KEY_LENGTH );
    }
#endif

    //
    // Compute Key to use to lock hard drive
    //
    XcHMAC( (LPBYTE)(*XboxHDKey), XBOX_KEY_LENGTH,
            (LPBYTE)HalDiskModelNumber->Buffer, HalDiskModelNumber->Length,
            (LPBYTE)HalDiskSerialNumber->Buffer, HalDiskSerialNumber->Length,
            FinalHardDriveKey ) ;

#endif // HDSETUPAPP

    if(
#ifdef HDSETUPAPP
        fRetail &&
#endif
        !HDSetPassword( FinalHardDriveKey, XC_SERVICE_DIGEST_SIZE ))
    {
        XDBGERR("RECOVERY", "Could not lock HD");
        FAIL_RECOVERY(26);
    }

#endif  // !INT2RETAIL

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

#endif // RECOVERYAPP

#ifdef DASHLIB

    //
    // Return zero to indicate success
    //

    return 0;
}

}
#endif // DASHLIB

