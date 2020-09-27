/*
State1: System Integration	
EEPROM	            No settings other than fixed mac addr & game region
HDD	                Unlocked, formatted, no dash.
VIDEO REGION	    None.
GAME REGION	        Manufacturing.
CLOCK/TZ/LANGUAGE	Not Set.
Behavior	
 Boot, No disk (or non game disc) in drive:	Box hangs at boot. 
 Boot, game disc in drive, non-mfg region	Box hangs at boot. 
 Boot, game disc in drive, mfg region.	Box runs disc.


State 2: System Test	
EEPROM	            All factory settings valid
HDD	                Unlocked, formatted, dash present
VIDEO REGION	    Correct per SKU.
GAME REGION	        Manufacturing + SKU Region
CLOCK/TZ/LANGUAGE	Not Set.
Behavior	
 Boot, No disk (or non game disc) in drive:	Displays Universal Error Message
 Boot, game disc in drive, non-mfg region	Displays Universal Error Message (even if region matches SKU region)
 Boot, game disc in drive, mfg region.	Box runs disc.


State 3: OOBE/Out Of Box Audit	
EEPROM	            All factory settings valid
HDD	                locked, formatted, dash present
VIDEO REGION	    Correct per SKU.
GAME REGION	        SKU region
CLOCK/TZ/LANGUAGE	Not Set.
Behavior	
 Boot, No disk (or non game disc) in drive:	Prompts for TZ/Clock/Language
 Boot, game disc in drive, non-mfg region	Prompts for TZ/Clock/Language
 Boot, game disc in drive, mfg region.	Prompts for TZ/Clock/Language


State 4: Post OOBE	
EEPROM	            All factory settings valid
HDD	                locked, formatted, dash present
VIDEO REGION	    Correct per SKU.
GAME REGION	        SKU region
CLOCK/TZ/LANGUAGE	Set.
Behavior	
 Boot, No disk (or non game disc) in drive:	Launch Xbox Dashboard
 Boot, game disc in drive, non-mfg region	Launch game (if region matches) or Dashboard w/error.
 Boot, game disc in drive, mfg region.	Launch Xbox Dashboard w/error
*/
//
// "winerror.h"
//
#include <ntos.h>
#include <init.h>
#include <xtl.h>
#include <xboxp.h>
#include <xconfig.h>
#include <av.h>
#include <stdio.h>
#include <xdbg.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include "PerBoxData.h"
#include "cryptkeys.h"
#include "xboxverp.h"

extern void DebugPrint(char* format, ...);

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

HRESULT SetRegion(DWORD regionFlags)
{
    BYTE buf[EEPROM_TOTAL_MEMORY_SIZE];
    EEPROM_LAYOUT* eeprom;
    PXBOX_ENCRYPTED_EEPROM_DATA pEncryptedEEPROMData;
    NTSTATUS status;
    DWORD type, size;
    BOOL bResult;
    HRESULT hr;
    BYTE FinalHardDriveKey[XC_SERVICE_DIGEST_SIZE];

    eeprom = (EEPROM_LAYOUT*) buf;
    pEncryptedEEPROMData = (PXBOX_ENCRYPTED_EEPROM_DATA) &eeprom->EncryptedSection;

    //
    // read EEPROM Data
    //
    status = ExQueryNonVolatileSetting(XC_MAX_ALL, &type, buf, sizeof(buf), &size);
    if (!NT_SUCCESS(status))
    {
        DebugPrint("   Error: %u\n", RtlNtStatusToDosError(status));
        return E_FAIL;
    }

    ASSERT( type == REG_BINARY );
    ASSERT( size == EEPROM_TOTAL_MEMORY_SIZE );

    //
    // Use the EEPROM-KEY to decrypt the EEPROM
    //
    if ((XboxHardwareInfo->Flags & XBOX_HW_FLAG_DEVKIT_KERNEL) == 0)
    {
        bResult = rc4HmacDecrypt(
            (LPBYTE)(*XboxEEPROMKey), XBOX_KEY_LENGTH,
            pEncryptedEEPROMData->HardDriveKey, sizeof(pEncryptedEEPROMData->HardDriveKey) + sizeof(pEncryptedEEPROMData->GameRegion),
            (LPBYTE)&(pEncryptedEEPROMData->EncHeader) );
        if ( !bResult )
        {
            DebugPrint("   Error: rc4HmacDecrypt\n");
            return E_FAIL;
        }
    }

    //
    // Set the manufacturing region flag
    //
    pEncryptedEEPROMData->GameRegion = regionFlags;

    //
    // Compute Key to use to lock hard drive
    // This must be done before we encrypt the buffer back to the encrypted state
    //
    XcHMAC( pEncryptedEEPROMData->HardDriveKey, HARD_DRIVE_KEY_LEN,
            (BYTE*)HalDiskModelNumber->Buffer, HalDiskModelNumber->Length,
            (BYTE*)HalDiskSerialNumber->Buffer, HalDiskSerialNumber->Length,
            FinalHardDriveKey );

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
        DebugPrint("   Error: %u\n", RtlNtStatusToDosError(status));
        return E_FAIL;
    }


    //
    // Zero out the memory
    //
    RtlZeroMemory( FinalHardDriveKey, sizeof(FinalHardDriveKey) );
    RtlZeroMemory( buf, sizeof(buf) );

    return S_OK;
}


