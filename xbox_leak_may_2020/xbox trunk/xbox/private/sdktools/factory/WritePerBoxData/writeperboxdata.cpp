//===================================================================
//
// Copyright Microsoft Corporation. All Right Reserved.
//
//===================================================================
#include <ntos.h>
#include <ntdddisk.h>
#include <ntddscsi.h>
#include <init.h>
#include <xtl.h>
#include <xboxp.h>
#include <xconfig.h>
#include <av.h>
#include <stdio.h>
#include <xdbg.h>
#include <malloc.h>
#include "PerBoxData.h"
#include "cryptkeys.h"
#include <smcdef.h>
#include <wpbdver.h>

// #define WRITE_EEPROM_IMAGE_TO_FILE 1

// #define DOUBLE_CHECK_EEPROM_KEY 1

// #define TEMP_CODE_FOR_TESTING 1

#define EEPROM_TAIL_PRESERVE_BYTES 2

#define RC4_CONFOUNDER_LEN 8

//
// Header before encrypted data
//
typedef struct _RC4_SHA1_HEADER {
    UCHAR Checksum[XC_SERVICE_DIGEST_SIZE];
    UCHAR Confounder[RC4_CONFOUNDER_LEN];
} RC4_SHA1_HEADER, *PRC4_SHA1_HEADER;

static void rc4HmacEncrypt(
    IN PUCHAR confounder, // RC4_CONFOUNDER_LEN bytes
    IN PUCHAR pbKey,
    IN ULONG cbKey,
    IN PUCHAR pbInput,
    IN ULONG cbInput,
    OUT PUCHAR pbHeader // XC_SERVICE_DIGEST_SIZE + RC4_CONFOUNDER_LEN bytes
    )
{
    PRC4_SHA1_HEADER CryptHeader = (PRC4_SHA1_HEADER) pbHeader;
    BYTE LocalKey[XC_SERVICE_DIGEST_SIZE];
    BYTE Rc4KeyStruct[XC_SERVICE_RC4_KEYSTRUCT_SIZE];

    //
    // Create the header - the confounder & checksum
    //
    RtlZeroMemory( CryptHeader->Checksum, XC_SERVICE_DIGEST_SIZE );
    RtlCopyMemory( CryptHeader->Confounder, confounder, RC4_CONFOUNDER_LEN );

    //
    // Checksum everything but the checksum
    //
    XcHMAC( pbKey, cbKey,
            CryptHeader->Confounder, RC4_CONFOUNDER_LEN,
            pbInput, cbInput,
            CryptHeader->Checksum );

    //
    // HMAC the checksum into the key
    //
    XcHMAC( pbKey, cbKey,
            CryptHeader->Checksum, XC_SERVICE_DIGEST_SIZE,
            NULL, 0,
            LocalKey );

    //
    // Use the generated key as the RC4 encryption key
    //
    XcRC4Key( Rc4KeyStruct, XC_SERVICE_DIGEST_SIZE, LocalKey );

    //
    // Encrypt everything but the checksum
    //
    XcRC4Crypt( Rc4KeyStruct, RC4_CONFOUNDER_LEN, CryptHeader->Confounder );
    XcRC4Crypt( Rc4KeyStruct, cbInput, pbInput );
}


static BOOL rc4HmacDecrypt(
    IN PUCHAR pbKey,
    IN ULONG cbKey,
    IN PUCHAR pbInput,
    IN ULONG cbInput,
    IN PUCHAR pbHeader // XC_SERVICE_DIGEST_SIZE + RC4_CONFOUNDER_LEN bytes
    )
{
    PRC4_SHA1_HEADER CryptHeader = (PRC4_SHA1_HEADER) pbHeader;
    RC4_SHA1_HEADER TempHeader;
    BYTE Confounder[RC4_CONFOUNDER_LEN];
    BYTE LocalKey[XC_SERVICE_DIGEST_SIZE];
    BYTE Rc4KeyStruct[XC_SERVICE_RC4_KEYSTRUCT_SIZE];

    RtlCopyMemory( TempHeader.Confounder, CryptHeader->Confounder, RC4_CONFOUNDER_LEN );

    //
    // HMAC the checksum into the key
    //
    XcHMAC( pbKey, cbKey,
            CryptHeader->Checksum, XC_SERVICE_DIGEST_SIZE,
            NULL, 0,
            LocalKey );

    //
    // Use the generated key as the RC4 encryption key
    //
    XcRC4Key( Rc4KeyStruct, XC_SERVICE_DIGEST_SIZE, LocalKey );

    //
    // Decrypt confounder and data
    //
    XcRC4Crypt( Rc4KeyStruct, RC4_CONFOUNDER_LEN, TempHeader.Confounder );
    XcRC4Crypt( Rc4KeyStruct, cbInput, pbInput );

    //
    // Now verify the checksum.
    //
    XcHMAC( pbKey, cbKey,
            TempHeader.Confounder, RC4_CONFOUNDER_LEN,
            pbInput, cbInput,
            TempHeader.Checksum );

    //
    // Decrypt is successful only if checksum matches
    //
    return ( RtlEqualMemory(
                TempHeader.Checksum,
                CryptHeader->Checksum,
                XC_SERVICE_DIGEST_SIZE) );
}


VOID WriteFileFromMemory(
    LPCSTR lpFileName,
    LPVOID pvFileContents,
    DWORD dwFileSize
    )
{
    HANDLE hFile;
    DWORD dwBytesWrote;

    hFile = CreateFile(lpFileName, GENERIC_WRITE, FILE_SHARE_READ, NULL,
        CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    if (hFile == INVALID_HANDLE_VALUE) {
        fprintf(stderr, "WritePerBoxData: unable to open %s\n", lpFileName);
        return;
    }

    if (!WriteFile(hFile, pvFileContents, dwFileSize, &dwBytesWrote, NULL) ||
        (dwBytesWrote != dwFileSize)) {
        fprintf(stderr, "WritePerBoxData: cannot write to %s\n", lpFileName);
        return;
    }

    CloseHandle(hFile);
}


VOID ReadFileFromMemory(
    LPCSTR lpFileName,
    LPVOID pvFileContents,
    DWORD* pdwBytesRead
    )
{
    HANDLE hFile;

    hFile = CreateFile(lpFileName, GENERIC_READ, FILE_SHARE_READ, NULL,
        OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if (hFile == INVALID_HANDLE_VALUE) {
        fprintf(stderr, "WritePerBoxData: unable to open %s\n", lpFileName);
        return;
    }

    if (!ReadFile(hFile, pvFileContents, *pdwBytesRead, pdwBytesRead, NULL))
    {
        fprintf(stderr, "WritePerBoxData: cannot read to %s\n", lpFileName);
        return;
    }

    CloseHandle(hFile);
}

HRESULT VerifyEEPROMData();

HRESULT WritePerBoxData(
    IN LPBYTE pbInputBuffer,
    IN DWORD dwInputBufferSize
    )
{
    BOOL bResult;
    HRESULT hr;
    PXBOX_ENCRYPTED_PER_BOX_DATA pData;
    char RecoveryKey[8];
    BYTE inputBuffer[sizeof(XBOX_ENCRYPTED_PER_BOX_DATA) + sizeof(RC4_SHA1_HEADER)];

    BYTE buf[EEPROM_TOTAL_MEMORY_SIZE];
    BYTE buf2[XBOX_KEY_LENGTH];
    EEPROM_LAYOUT* eeprom;
    XBOX_FACTORY_SETTINGS* factorySettings;
    XBOX_USER_SETTINGS* userSettings;
    PXBOX_ENCRYPTED_EEPROM_DATA pEncryptedEEPROMData;
    NTSTATUS status;
    DWORD type, size;
    int i;

    if ( dwInputBufferSize != sizeof(inputBuffer) )
    {
        return NTE_BAD_LEN;
    }

    //
    // Copy input data into new buffer
    //
    RtlCopyMemory( inputBuffer, pbInputBuffer, sizeof(inputBuffer) );

    bResult = rc4HmacDecrypt(
                    (LPBYTE)FACTORY_RAND_KEY, FACTORY_RAND_KEY_LEN,
                    inputBuffer + sizeof(RC4_SHA1_HEADER), sizeof(XBOX_ENCRYPTED_PER_BOX_DATA),
                    inputBuffer );
    if ( !bResult )
    {
        return NTE_BAD_DATA;
    }

    //
    // Setup pointer to decrypted data
    //
    pData = (PXBOX_ENCRYPTED_PER_BOX_DATA)(inputBuffer + sizeof(RC4_SHA1_HEADER));

#if DBG
    XDBGTRC("factory", "OnlineKey: %02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",
        pData->OnlineKey[0], pData->OnlineKey[1],
        pData->OnlineKey[2], pData->OnlineKey[3],
        pData->OnlineKey[4], pData->OnlineKey[5],
        pData->OnlineKey[6], pData->OnlineKey[7],
        pData->OnlineKey[8], pData->OnlineKey[9],
        pData->OnlineKey[10], pData->OnlineKey[11],
        pData->OnlineKey[12], pData->OnlineKey[13],
        pData->OnlineKey[14], pData->OnlineKey[15]
        );

    XDBGTRC("factory", "HardDriveKey: %02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",
        pData->HardDriveKey[0], pData->HardDriveKey[1],
        pData->HardDriveKey[2], pData->HardDriveKey[3],
        pData->HardDriveKey[4], pData->HardDriveKey[5],
        pData->HardDriveKey[6], pData->HardDriveKey[7],
        pData->HardDriveKey[8], pData->HardDriveKey[9],
        pData->HardDriveKey[10], pData->HardDriveKey[11],
        pData->HardDriveKey[12], pData->HardDriveKey[13],
        pData->HardDriveKey[14], pData->HardDriveKey[15]
        );

    ComputeRecoveryKey( pData->HardDriveKey, RecoveryKey );
    XDBGTRC("factory", "RecoveryKey: %.8s", RecoveryKey);

    XDBGTRC("factory", "MACAddress: %.6s", pData->MACAddress);

    XDBGTRC("factory", "XboxSerialNumber: %.12s", pData->XboxSerialNumber);

    XDBGTRC("factory", "GameRegion: %X", pData->GameRegion);

    XDBGTRC("factory", "AVRegion: %X", pData->AVRegion);
#endif

    //
    // All devkits have 16 bytes of zero for HD key
    //
    if ((XboxHardwareInfo->Flags & XBOX_HW_FLAG_DEVKIT_KERNEL) != 0)
    {
        RtlZeroMemory( pData->HardDriveKey, sizeof(pData->HardDriveKey) );
    }

    //
    // Check some basic assumptions
    //
    ASSERT( sizeof(RC4_SHA1_HEADER) == sizeof(pEncryptedEEPROMData->EncHeader) );
    ASSERT( EEPROM_ENCRYPTED_SECTION_SIZE == sizeof(*pEncryptedEEPROMData) );

    //
    // read EEPROM Data, some tail bytes need to be preserved.
    //
    status = ExQueryNonVolatileSetting(XC_MAX_ALL, &type, buf, sizeof(buf), &size);
    if (!NT_SUCCESS(status))
    {
        return E_FAIL;
    }

    ASSERT( type == REG_BINARY );
    ASSERT( size == EEPROM_TOTAL_MEMORY_SIZE );

    //
    // Zero out the EEPROM buffer
    //
    RtlZeroMemory( buf, EEPROM_TOTAL_MEMORY_SIZE - EEPROM_TAIL_PRESERVE_BYTES );

    //
    // Get some pointers ready
    //
    eeprom = (EEPROM_LAYOUT*) buf;
    factorySettings = (XBOX_FACTORY_SETTINGS*) &eeprom->FactorySection;
    userSettings = (XBOX_USER_SETTINGS*) &eeprom->UserConfigSection;
    pEncryptedEEPROMData = (PXBOX_ENCRYPTED_EEPROM_DATA) &eeprom->EncryptedSection;

    //
    // Get encrypted EEPROM contents ready
    //
    pEncryptedEEPROMData->GameRegion = pData->GameRegion;
    pEncryptedEEPROMData->GameRegion |= XC_GAME_REGION_MANUFACTURING;
    RtlCopyMemory( pEncryptedEEPROMData->HardDriveKey, pData->HardDriveKey, sizeof(pEncryptedEEPROMData->HardDriveKey) );

    //
    // Place in ROM slot for future use
    //
    ASSERT( XBOX_KEY_LENGTH == sizeof(pEncryptedEEPROMData->HardDriveKey) );
    RtlCopyMemory( (LPBYTE)(*XboxHDKey), pData->HardDriveKey, sizeof(pEncryptedEEPROMData->HardDriveKey) );

#ifdef DOUBLE_CHECK_EEPROM_KEY
    //
    // Double check that a random EEPROM key has really been generated
    //
    ASSERT( XBOX_KEY_LENGTH == TEMP_RAND_KEY_LEN );
    if (RtlEqualMemory( ZERO_KEY, (LPBYTE)(*XboxEEPROMKey), XBOX_KEY_LENGTH ) ||
        RtlEqualMemory( TEMP_RAND_KEY, (LPBYTE)(*XboxEEPROMKey), XBOX_KEY_LENGTH ))
    {
        ASSERT(!"The EEPROM Key has not been randomly generated");
        return NTE_NO_KEY;
    }
#endif

    //
    // Encrypt the encrypted part and copy it into the EEPROM buffer
    //
    rc4HmacEncrypt(
        inputBuffer, (LPBYTE)(*XboxEEPROMKey), XBOX_KEY_LENGTH,
        pEncryptedEEPROMData->HardDriveKey, sizeof(pEncryptedEEPROMData->HardDriveKey) + sizeof(pEncryptedEEPROMData->GameRegion),
        (LPBYTE)&(pEncryptedEEPROMData->EncHeader) );

#ifdef TEMP_CODE_FOR_TESTING

    rc4HmacEncrypt(
        inputBuffer, (LPBYTE)(*XboxEEPROMKey), XBOX_KEY_LENGTH,
        pEncryptedEEPROMData->HardDriveKey, sizeof(pEncryptedEEPROMData->HardDriveKey) + sizeof(pEncryptedEEPROMData->GameRegion),
        (LPBYTE)&(pEncryptedEEPROMData->EncHeader) );

    for (i=0; i<sizeof(*pEncryptedEEPROMData) / 16; ++i)
    {
        BYTE* p = (BYTE*)pEncryptedEEPROMData;
        p += i * 16;
        XDBGWRN("factory", ",0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x",
            p[0], p[1],
            p[2], p[3],
            p[4], p[5],
            p[6], p[7],
            p[8], p[9],
            p[10], p[11],
            p[12], p[13],
            p[14], p[15]
            );
    }

    bResult = rc4HmacDecrypt(
        (LPBYTE)(*XboxEEPROMKey), XBOX_KEY_LENGTH,
        pEncryptedEEPROMData->HardDriveKey, sizeof(pEncryptedEEPROMData->HardDriveKey) + sizeof(pEncryptedEEPROMData->GameRegion),
        (LPBYTE)&(pEncryptedEEPROMData->EncHeader) );

    ASSERT(bResult);
#endif

    //
    // Get factory settings ready
    //
    RtlCopyMemory( factorySettings->SerialNumber, pData->XboxSerialNumber, sizeof(factorySettings->SerialNumber) );
    ASSERT( sizeof(factorySettings->SerialNumber) == sizeof(pData->XboxSerialNumber) );
    RtlCopyMemory( factorySettings->EthernetAddr, pData->MACAddress, sizeof(factorySettings->EthernetAddr) );
    ASSERT( sizeof(factorySettings->EthernetAddr) == sizeof(pData->MACAddress) );
    RtlCopyMemory( factorySettings->OnlineKey, pData->OnlineKey, sizeof(factorySettings->OnlineKey) );
    ASSERT( sizeof(factorySettings->OnlineKey) == sizeof(pData->OnlineKey) );

#if DBG
    ASSERT( XBOX_KEY_LENGTH == sizeof(factorySettings->OnlineKey) );
    RtlCopyMemory( buf2, factorySettings->OnlineKey, XBOX_KEY_LENGTH );
    MorphKeyByHardDriveKey( factorySettings->OnlineKey, sizeof(factorySettings->OnlineKey) );
    MorphKeyByHardDriveKey( factorySettings->OnlineKey, sizeof(factorySettings->OnlineKey) );
    ASSERT(RtlEqualMemory( buf2, factorySettings->OnlineKey, XBOX_KEY_LENGTH ));
    RtlZeroMemory( buf2, XBOX_KEY_LENGTH );
#endif

    MorphKeyByHardDriveKey( factorySettings->OnlineKey, sizeof(factorySettings->OnlineKey) );
    factorySettings->AVRegion = pData->AVRegion;
    factorySettings->Checksum = ~XConfigChecksum(factorySettings, sizeof(*factorySettings));

    //
    // Get user Settings ready
    //
    if ( pData->GameRegion & XC_GAME_REGION_JAPAN )
    {
        userSettings->Language = XC_LANGUAGE_JAPANESE;
        userSettings->TimeZoneBias = -540;
        strcpy(userSettings->TimeZoneStdName, "TST");
        strcpy(userSettings->TimeZoneDltName, "TST");
    }
    userSettings->Checksum = ~XConfigChecksum(userSettings, sizeof(*userSettings));

    //
    // Copy EEPROM Data into the EEPROM
    //
    status = ExSaveNonVolatileSetting(XC_MAX_ALL, REG_BINARY, buf, EEPROM_TOTAL_MEMORY_SIZE);
    if (!NT_SUCCESS(status))
    {
        return E_FAIL;
    }

    //
    // Write a copy of the EEPROM to file for debugging
    //
#ifdef WRITE_EEPROM_IMAGE_TO_FILE
    WriteFileFromMemory( "T:\\EEPROMImage.dat", buf, EEPROM_TOTAL_MEMORY_SIZE );
#endif

    //
    // Zero out the memory
    //
    RtlZeroMemory( inputBuffer, sizeof(inputBuffer) );
    RtlZeroMemory( buf, sizeof(buf) );

    //
    // Verify the written data to double check
    //
    hr = VerifyEEPROMData();
    if (FAILED(hr))
    {
        return hr;
    }

    return S_OK;
}


HRESULT VerifyEEPROMData()
{
    BYTE buf[EEPROM_TOTAL_MEMORY_SIZE];
    EEPROM_LAYOUT* eeprom;
    XBOX_FACTORY_SETTINGS* factorySettings;
    XBOX_USER_SETTINGS* userSettings;
    PXBOX_ENCRYPTED_EEPROM_DATA pEncryptedEEPROMData;
    NTSTATUS status;
    DWORD type, size;
    BOOL bResult;

    eeprom = (EEPROM_LAYOUT*) buf;
    factorySettings = (XBOX_FACTORY_SETTINGS*) &eeprom->FactorySection;
    userSettings = (XBOX_USER_SETTINGS*) &eeprom->UserConfigSection;
    pEncryptedEEPROMData = (PXBOX_ENCRYPTED_EEPROM_DATA) &eeprom->EncryptedSection;

    //
    // read EEPROM Data
    //
    status = ExQueryNonVolatileSetting(XC_MAX_ALL, &type, buf, sizeof(buf), &size);
    if (!NT_SUCCESS(status))
    {
        return E_FAIL;
    }

    ASSERT( type == REG_BINARY );
    ASSERT( size == EEPROM_TOTAL_MEMORY_SIZE );

    //
    // check checksums
    //
    if (XConfigChecksum(factorySettings, sizeof(*factorySettings)) != 0xffffffff ||
        XConfigChecksum(userSettings, sizeof(*userSettings)) != 0xffffffff)
    {
        return NTE_BAD_DATA;
    }

#ifdef DOUBLE_CHECK_EEPROM_KEY
    //
    // Double check that a random EEPROM key has really been generated
    //
    ASSERT( XBOX_KEY_LENGTH == TEMP_RAND_KEY_LEN );
    if (RtlEqualMemory( ZERO_KEY, (LPBYTE)(*XboxEEPROMKey), XBOX_KEY_LENGTH ) ||
        RtlEqualMemory( TEMP_RAND_KEY, (LPBYTE)(*XboxEEPROMKey), XBOX_KEY_LENGTH ))
    {
        ASSERT(!"The EEPROM Key has not been randomly generated");
        return NTE_NO_KEY;
    }
#endif

    //
    // Use the EEPROM-KEY to decrypt the EEPROM and verify the checksum
    //
    bResult = rc4HmacDecrypt(
        (LPBYTE)(*XboxEEPROMKey), XBOX_KEY_LENGTH,
        pEncryptedEEPROMData->HardDriveKey, sizeof(pEncryptedEEPROMData->HardDriveKey) + sizeof(pEncryptedEEPROMData->GameRegion),
        (LPBYTE)&(pEncryptedEEPROMData->EncHeader) );
    if ( !bResult )
    {
        return NTE_BAD_DATA;
    }

    //
    // Zero out the memory
    //
    RtlZeroMemory( buf, sizeof(buf) );

    return S_OK;
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


HRESULT VerifyPerBoxData(void)
{
    return VerifyPerBoxDataEx(TRUE);
}

HRESULT VerifyPerBoxDataEx(BOOL fResetRegionBits)
{
    BYTE buf[EEPROM_TOTAL_MEMORY_SIZE];
    EEPROM_LAYOUT* eeprom;
    PXBOX_ENCRYPTED_EEPROM_DATA pEncryptedEEPROMData;
    NTSTATUS status;
    DWORD type, size;
    BOOL bResult;
    HRESULT hr;
    BYTE FinalHardDriveKey[XC_SERVICE_DIGEST_SIZE];

    //
    // Verify the EEPROM data first
    //
    hr = VerifyEEPROMData();
    if (FAILED(hr))
    {
        return hr;
    }

    eeprom = (EEPROM_LAYOUT*) buf;
    pEncryptedEEPROMData = (PXBOX_ENCRYPTED_EEPROM_DATA) &eeprom->EncryptedSection;

    //
    // read EEPROM Data
    //
    status = ExQueryNonVolatileSetting(XC_MAX_ALL, &type, buf, sizeof(buf), &size);
    if (!NT_SUCCESS(status))
    {
        return E_FAIL;
    }

    ASSERT( type == REG_BINARY );
    ASSERT( size == EEPROM_TOTAL_MEMORY_SIZE );

#ifdef DOUBLE_CHECK_EEPROM_KEY
    //
    // Double check that a random EEPROM key has really been generated
    //
    ASSERT( XBOX_KEY_LENGTH == TEMP_RAND_KEY_LEN );
    if (RtlEqualMemory( ZERO_KEY, (LPBYTE)(*XboxEEPROMKey), XBOX_KEY_LENGTH ) ||
        RtlEqualMemory( TEMP_RAND_KEY, (LPBYTE)(*XboxEEPROMKey), XBOX_KEY_LENGTH ))
    {
        ASSERT(!"The EEPROM Key has not been randomly generated");
        return NTE_NO_KEY;
    }
#endif

    //
    // Use the EEPROM-KEY to decrypt the EEPROM
    //
    bResult = rc4HmacDecrypt(
        (LPBYTE)(*XboxEEPROMKey), XBOX_KEY_LENGTH,
        pEncryptedEEPROMData->HardDriveKey, sizeof(pEncryptedEEPROMData->HardDriveKey) + sizeof(pEncryptedEEPROMData->GameRegion),
        (LPBYTE)&(pEncryptedEEPROMData->EncHeader) );
    if ( !bResult )
    {
        return NTE_BAD_DATA;
    }

    if(fResetRegionBits) {
        //
        // Clear the manufacturing region flag
        //
        pEncryptedEEPROMData->GameRegion &= ~XC_GAME_REGION_MANUFACTURING;
    }

    //
    // Double check the hard drive key
    //
    ASSERT( HARD_DRIVE_KEY_LEN == XBOX_KEY_LENGTH );
    if (!RtlEqualMemory( pEncryptedEEPROMData->HardDriveKey, (LPBYTE)(*XboxHDKey), XBOX_KEY_LENGTH ))
    {
        ASSERT(!"HardDriveKey does not match");
        return NTE_BAD_KEY;
    }

    //
    // Encrypt the encrypted part back again
    //
    rc4HmacEncrypt(
        (LPBYTE)(&(pEncryptedEEPROMData->EncHeader) + XC_SERVICE_DIGEST_SIZE), // Use original confounder
        (LPBYTE)(*XboxEEPROMKey), XBOX_KEY_LENGTH,
        pEncryptedEEPROMData->HardDriveKey, sizeof(pEncryptedEEPROMData->HardDriveKey) + sizeof(pEncryptedEEPROMData->GameRegion),
        (LPBYTE)&(pEncryptedEEPROMData->EncHeader) );

    //
    // Save EEPROM Data into the EEPROM
    //
    status = ExSaveNonVolatileSetting(XC_MAX_ALL, REG_BINARY, buf, EEPROM_TOTAL_MEMORY_SIZE);
    if (!NT_SUCCESS(status))
    {
        return E_FAIL;
    }

    //
    // Check that devkits have zero key
    //
    if ((XboxHardwareInfo->Flags & XBOX_HW_FLAG_DEVKIT_KERNEL) != 0)
    {
        if(!RtlEqualMemory( ZERO_KEY, (LPBYTE)(*XboxHDKey), XBOX_KEY_LENGTH ) )
			return E_FAIL;
    }

    //
    // Compute Key to use to lock hard drive and lock it
    //
    XcHMAC( (LPBYTE)(*XboxHDKey), XBOX_KEY_LENGTH,
            (LPBYTE)HalDiskModelNumber->Buffer, HalDiskModelNumber->Length,
            (LPBYTE)HalDiskSerialNumber->Buffer, HalDiskSerialNumber->Length,
            FinalHardDriveKey );

    HDSetPassword( FinalHardDriveKey, XC_SERVICE_DIGEST_SIZE );

    //
    // Zero out the memory
    //
    RtlZeroMemory( FinalHardDriveKey, sizeof(FinalHardDriveKey) );
    RtlZeroMemory( buf, sizeof(buf) );

    return S_OK;
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

HRESULT UnlockXboxHardDrive (
    IN LPBYTE pbHardDriveKey,
    IN DWORD dwHardDriveKeySize
    )
{
    BYTE FinalHardDriveKey[XC_SERVICE_DIGEST_SIZE];

    if ( pbHardDriveKey == NULL )
    {
        //
        // Use the key the ROM read from the EEPROM
        //
        pbHardDriveKey = (LPBYTE)(*XboxHDKey);
        dwHardDriveKeySize = XBOX_KEY_LENGTH;
    }
    else
    {
        ASSERT( XBOX_KEY_LENGTH == dwHardDriveKeySize );
        if ( dwHardDriveKeySize != XBOX_KEY_LENGTH )
        {
            return NTE_BAD_LEN;
        }
    }

    //
    // Compute Key to use and unlock hard drive
    //
    XcHMAC( pbHardDriveKey, dwHardDriveKeySize,
            (LPBYTE)HalDiskModelNumber->Buffer, HalDiskModelNumber->Length,
            (LPBYTE)HalDiskSerialNumber->Buffer, HalDiskSerialNumber->Length,
            FinalHardDriveKey );

    HDDisablePassword( FinalHardDriveKey, XC_SERVICE_DIGEST_SIZE );

    //
    // Zero out the memory
    //
    RtlZeroMemory( FinalHardDriveKey, sizeof(FinalHardDriveKey) );

    return S_OK;
}

#if 0

//
// DecryptDevKitEEPROM should never be used anymore -- a devkit functions just
// like a retail box
//

HRESULT DecryptDevKitEEPROM()
{
    BYTE buf[EEPROM_TOTAL_MEMORY_SIZE];
    EEPROM_LAYOUT* eeprom;
    PXBOX_ENCRYPTED_EEPROM_DATA pEncryptedEEPROMData;
    NTSTATUS status;
    DWORD type, size;
    BOOL bResult;
    HRESULT hr;
    BYTE FinalHardDriveKey[XC_SERVICE_DIGEST_SIZE];

    if ((XboxHardwareInfo->Flags & XBOX_HW_FLAG_DEVKIT_KERNEL) == 0)
    {
        return S_OK;
    }

    eeprom = (EEPROM_LAYOUT*) buf;
    pEncryptedEEPROMData = (PXBOX_ENCRYPTED_EEPROM_DATA) &eeprom->EncryptedSection;

    //
    // read EEPROM Data
    //
    status = ExQueryNonVolatileSetting(XC_MAX_ALL, &type, buf, sizeof(buf), &size);
    if (!NT_SUCCESS(status))
    {
        return E_FAIL;
    }

    ASSERT( type == REG_BINARY );
    ASSERT( size == EEPROM_TOTAL_MEMORY_SIZE );

    //
    // if encrypted then decrypt the EEPROM
    //
    if ( !RtlEqualMemory( ZERO_KEY, pEncryptedEEPROMData->HardDriveKey, XBOX_KEY_LENGTH ) )
    {
        bResult = rc4HmacDecrypt(
            (LPBYTE)(*XboxEEPROMKey), XBOX_KEY_LENGTH,
            pEncryptedEEPROMData->HardDriveKey, sizeof(pEncryptedEEPROMData->HardDriveKey) + sizeof(pEncryptedEEPROMData->GameRegion),
            (LPBYTE)&(pEncryptedEEPROMData->EncHeader) );
        if ( !bResult )
        {
            return NTE_BAD_DATA;
        }

        //
        // Check that Hard drive key is the zero key
        //
        ASSERT( RtlEqualMemory( ZERO_KEY, pEncryptedEEPROMData->HardDriveKey, XBOX_KEY_LENGTH ) );
        ASSERT( RtlEqualMemory( ZERO_KEY, (LPBYTE)(*XboxHDKey), XBOX_KEY_LENGTH ) );
        RtlZeroMemory( pEncryptedEEPROMData->HardDriveKey, XBOX_KEY_LENGTH );
        RtlZeroMemory( (LPBYTE)(*XboxHDKey), XBOX_KEY_LENGTH );

        //
        // Save EEPROM Data into the EEPROM
        //
        status = ExSaveNonVolatileSetting(XC_MAX_ALL, REG_BINARY, buf, EEPROM_TOTAL_MEMORY_SIZE);
        if (!NT_SUCCESS(status))
        {
            return E_FAIL;
        }
    }

    //
    // Zero out the memory
    //
    RtlZeroMemory( buf, sizeof(buf) );

    return S_OK;
}

#endif // 0

HRESULT QueryFatalErrorLastCode(
    OUT LPDWORD pdwFatalErrorCode
    )
{
    NTSTATUS Status;

    //
    // Read the last error code from the SMC error register
    //

    Status = HalReadSMBusByte(SMC_SLAVE_ADDRESS, SMC_COMMAND_READ_ERROR_CODE, 
        pdwFatalErrorCode);

    if (!NT_SUCCESS(Status)) {
        *pdwFatalErrorCode = 0;
        return E_FAIL;
    }

    return S_OK;
}


HRESULT QueryFatalErrorHistory(
    OUT LPDWORD pdwFatalErrorHistory
    )
{
    EEPROM_LAYOUT EepromData;
    ULONG Size;
    ULONG Type;
    NTSTATUS Status;
    XBOX_UEM_INFO* UEMInfo = (XBOX_UEM_INFO*)&(EepromData.UEMInfo[0]);
    ULONG History = 0;

    //
    // Read all of the eeprom
    //

    Status = ExQueryNonVolatileSetting(XC_MAX_ALL, &Type, &EepromData, sizeof(EepromData), &Size);
    
    if (!NT_SUCCESS(Status)) {
        *pdwFatalErrorHistory = 0;
        return E_FAIL;
    }

    History = (ULONG)UEMInfo->History;

    //
    // Shift the value so that bits match up with the error codes (starting at 1).  Kernel 
    // does not store the codes 0 through 5 in the history field in EEPROM
    //

    History <<= 5;
    
    *pdwFatalErrorHistory = History;
    
    return S_OK;
}



HRESULT ClearFatalErrorHistory(
    VOID
    )
{
    EEPROM_LAYOUT EepromData;
    ULONG Size;
    ULONG Type;
    NTSTATUS Status;
    XBOX_UEM_INFO* UEMInfo = (XBOX_UEM_INFO*)&(EepromData.UEMInfo[0]);
    ULONG History = 0;

    //
    // Read all of the eeprom
    //

    Status = ExQueryNonVolatileSetting(XC_MAX_ALL, &Type, &EepromData, sizeof(EepromData), &Size);
    
    if (!NT_SUCCESS(Status)) {
        return E_FAIL;
    }

    //
    // Clear the history
    //

    UEMInfo->History = 0;

    //
    // Write the EEPROM back out
    //

    Status = ExSaveNonVolatileSetting(XC_MAX_ALL, Type, &EepromData, Size);
    
    if (!NT_SUCCESS(Status)) {
        return E_FAIL;
    }
    
    return S_OK;
}



HRESULT WINAPI SetFatalErrorRetryFlag(
    IN BOOL Retry
    )
{
    EEPROM_LAYOUT EepromData;
    ULONG Size;
    ULONG Type;
    NTSTATUS Status;
    XBOX_UEM_INFO* UEMInfo = (XBOX_UEM_INFO*)&(EepromData.UEMInfo[0]);
    ULONG History = 0;

    //
    // Read all of the eeprom
    //

    Status = ExQueryNonVolatileSetting(XC_MAX_ALL, &Type, &EepromData, sizeof(EepromData), &Size);
    
    if (!NT_SUCCESS(Status)) {
        return E_FAIL;
    }

    //
    // Set the last error code which will determine if we need to retry or not
    //

    if (Retry) {
        UEMInfo->LastCode = FATAL_ERROR_NONE;
    } else {
        UEMInfo->LastCode = FATAL_ERROR_RESERVED;
    }

    //
    // Write the EEPROM back out
    //

    Status = ExSaveNonVolatileSetting(XC_MAX_ALL, Type, &EepromData, Size);
    
    if (!NT_SUCCESS(Status)) {
        return E_FAIL;
    }
    
    return S_OK;
}

