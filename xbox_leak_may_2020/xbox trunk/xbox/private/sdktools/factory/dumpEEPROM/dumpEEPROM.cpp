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

#if DBG
extern "C" ULONG XDebugOutLevel;
#endif

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

HRESULT ReadPerBoxData()
{
    BYTE buf[EEPROM_TOTAL_MEMORY_SIZE];
    EEPROM_LAYOUT* eeprom;
    XBOX_FACTORY_SETTINGS* factorySettings;
    XBOX_USER_SETTINGS* userSettings;
    PXBOX_ENCRYPTED_EEPROM_DATA pEncryptedEEPROMData;
    NTSTATUS status;
    DWORD type, size;
    BOOL bResult;
    HRESULT hr;
    char RecoveryKey[8];
    
    //
    // Get some pointers ready
    //
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
    // Use the EEPROM-KEY to decrypt the EEPROM
    //
    //if ((XboxHardwareInfo->Flags & XBOX_HW_FLAG_DEVKIT_KERNEL) == 0)
    {
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
    // Double check the hard drive key
    //
    ASSERT( HARD_DRIVE_KEY_LEN == XBOX_KEY_LENGTH );
    if (!RtlEqualMemory( pEncryptedEEPROMData->HardDriveKey, (LPBYTE)(*XboxHDKey), XBOX_KEY_LENGTH ))
    {
        ASSERT(!"HardDriveKey does not match");
        return NTE_BAD_KEY;        
    }

    MorphKeyByHardDriveKey( factorySettings->OnlineKey, sizeof(factorySettings->OnlineKey) );

#if DBG
    XDBGTRC("factory", "OnlineKey: %02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",
        factorySettings->OnlineKey[0], factorySettings->OnlineKey[1],
        factorySettings->OnlineKey[2], factorySettings->OnlineKey[3],
        factorySettings->OnlineKey[4], factorySettings->OnlineKey[5],
        factorySettings->OnlineKey[6], factorySettings->OnlineKey[7],
        factorySettings->OnlineKey[8], factorySettings->OnlineKey[9],
        factorySettings->OnlineKey[10], factorySettings->OnlineKey[11],
        factorySettings->OnlineKey[12], factorySettings->OnlineKey[13],
        factorySettings->OnlineKey[14], factorySettings->OnlineKey[15]
        );

    XDBGTRC("factory", "HardDriveKey: %02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",
        pEncryptedEEPROMData->HardDriveKey[0], pEncryptedEEPROMData->HardDriveKey[1],
        pEncryptedEEPROMData->HardDriveKey[2], pEncryptedEEPROMData->HardDriveKey[3],
        pEncryptedEEPROMData->HardDriveKey[4], pEncryptedEEPROMData->HardDriveKey[5],
        pEncryptedEEPROMData->HardDriveKey[6], pEncryptedEEPROMData->HardDriveKey[7],
        pEncryptedEEPROMData->HardDriveKey[8], pEncryptedEEPROMData->HardDriveKey[9],
        pEncryptedEEPROMData->HardDriveKey[10], pEncryptedEEPROMData->HardDriveKey[11],
        pEncryptedEEPROMData->HardDriveKey[12], pEncryptedEEPROMData->HardDriveKey[13],
        pEncryptedEEPROMData->HardDriveKey[14], pEncryptedEEPROMData->HardDriveKey[15]
        );

    ComputeRecoveryKey( pEncryptedEEPROMData->HardDriveKey, RecoveryKey );
    XDBGTRC("factory", "RecoveryKey: %.8s", RecoveryKey);

    XDBGTRC("factory", "MACAddress: %02x%02x%02x%02x%02x%02x",
        factorySettings->EthernetAddr[0], factorySettings->EthernetAddr[1],
        factorySettings->EthernetAddr[2], factorySettings->EthernetAddr[3],
        factorySettings->EthernetAddr[4], factorySettings->EthernetAddr[5]
        );

    XDBGTRC("factory", "XboxSerialNumber: %.12s", factorySettings->SerialNumber);
    
    XDBGTRC("factory", "GameRegion: %X", pEncryptedEEPROMData->GameRegion);

    XDBGTRC("factory", "AVRegion: %X", factorySettings->AVRegion);
#endif
    
    return S_OK;
}

//======================== The main function
void __cdecl main()
{
    HRESULT hr;
    
#if DBG
    XDebugOutLevel = XDBG_EXIT;
#endif

    hr = ReadPerBoxData();
    if ( FAILED( hr ) )
    {
        XDBGTRC("factory", "ReadPerBoxData returned %x",hr);
        goto ErrorExit;        
    }

ErrorExit:

    return;
}
