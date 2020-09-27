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

    /*
    return NTE_BAD_LEN;
    return NTE_BAD_DATA;
    return E_FAIL;
    return NTE_NO_KEY;        
    return E_FAIL;

    hr = VerifyEEPROMData();
    if (FAILED(hr))
    {
        return hr;
    }
    */
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

    //
    // Use the EEPROM-KEY to decrypt the EEPROM and verify the checksum
    //
    if ((XboxHardwareInfo->Flags & XBOX_HW_FLAG_DEVKIT_KERNEL) == 0)
    {
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

        bResult = rc4HmacDecrypt(
            (LPBYTE)(*XboxEEPROMKey), XBOX_KEY_LENGTH,
            pEncryptedEEPROMData->HardDriveKey, sizeof(pEncryptedEEPROMData->HardDriveKey) + sizeof(pEncryptedEEPROMData->GameRegion),
            (LPBYTE)&(pEncryptedEEPROMData->EncHeader) );
        if ( !bResult )
        {
            return NTE_BAD_DATA;
        }
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


HRESULT VerifyPerBoxData()
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

    //
    // Use the EEPROM-KEY to decrypt the EEPROM
    //
    if ((XboxHardwareInfo->Flags & XBOX_HW_FLAG_DEVKIT_KERNEL) == 0)
    {
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

        bResult = rc4HmacDecrypt(
            (LPBYTE)(*XboxEEPROMKey), XBOX_KEY_LENGTH,
            pEncryptedEEPROMData->HardDriveKey, sizeof(pEncryptedEEPROMData->HardDriveKey) + sizeof(pEncryptedEEPROMData->GameRegion),
            (LPBYTE)&(pEncryptedEEPROMData->EncHeader) );
        if ( !bResult )
        {
            return NTE_BAD_DATA;
        }
    }

    //
    // Clear the manufacturing region flag
    //
    pEncryptedEEPROMData->GameRegion &= ~XC_GAME_REGION_MANUFACTURING;

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
    if ((XboxHardwareInfo->Flags & XBOX_HW_FLAG_DEVKIT_KERNEL) == 0)
    {
        //
        // Only Encrypt if it is not a dev kit
        //
        rc4HmacEncrypt(
            (LPBYTE)(&(pEncryptedEEPROMData->EncHeader) + XC_SERVICE_DIGEST_SIZE), // Use original confounder
            (LPBYTE)(*XboxEEPROMKey), XBOX_KEY_LENGTH,
            pEncryptedEEPROMData->HardDriveKey, sizeof(pEncryptedEEPROMData->HardDriveKey) + sizeof(pEncryptedEEPROMData->GameRegion),
            (LPBYTE)&(pEncryptedEEPROMData->EncHeader) );
    }

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
        ASSERT( RtlEqualMemory( ZERO_KEY, (LPBYTE)(*XboxHDKey), XBOX_KEY_LENGTH ) );
        RtlZeroMemory( (LPBYTE)(*XboxHDKey), XBOX_KEY_LENGTH );
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

