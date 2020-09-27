#include <ntos.h>
#include <init.h>
#include <xtl.h>
#include <xboxp.h>
#include <xconfig.h>
#include <av.h>
#include <stdio.h>
#include <xdbg.h>
#include <malloc.h>
#define XFONT_TRUETYPE
#include <xfont.h>
#include "cryptkeys.h"
#include "xboxvideo.h"
#include "xboxverp.h"

#if DBG
extern "C" ULONG XDebugOutLevel;
#endif



#define RC4_CONFOUNDER_LEN 8

BOOL HDDisablePassword( BYTE* password, DWORD dwLen );

HRESULT UnlockHardDrive(void)
{
    BYTE FinalHardDriveKey[XC_SERVICE_DIGEST_SIZE];
    HRESULT hr;

    //
    // Compute Key to use and unlock hard drive
    //
    XcHMAC( (PUCHAR)*XboxHDKey, XBOX_KEY_LENGTH,
            (LPBYTE)HalDiskModelNumber->Buffer, HalDiskModelNumber->Length,
            (LPBYTE)HalDiskSerialNumber->Buffer, HalDiskSerialNumber->Length,
            FinalHardDriveKey );

    hr = HDDisablePassword( FinalHardDriveKey, XC_SERVICE_DIGEST_SIZE ) ?
        S_OK : E_FAIL;

    //
    // Zero out the memory
    //
    RtlZeroMemory( FinalHardDriveKey, sizeof(FinalHardDriveKey) );

    return hr;
}

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

HRESULT SetManufacturingRegion (VOID)
{
    BYTE buf[EEPROM_TOTAL_MEMORY_SIZE];
    EEPROM_LAYOUT* eeprom;
    PXBOX_ENCRYPTED_EEPROM_DATA pEncryptedEEPROMData;
    NTSTATUS status;
    DWORD type, size;
    BOOL bResult;
    HRESULT hr;
    BOOL fWasEncrypted;

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

    if ((XboxHardwareInfo->Flags & XBOX_HW_FLAG_DEVKIT_KERNEL) == 0)
        fWasEncrypted = TRUE;
    else
        fWasEncrypted = !RtlEqualMemory(pEncryptedEEPROMData->HardDriveKey - 8,
            ZERO_KEY, 8);

    //
    // Use the EEPROM-KEY to decrypt the EEPROM
    //
    if(fWasEncrypted) {
        bResult = rc4HmacDecrypt(
            (LPBYTE)(*XboxEEPROMKey), XBOX_KEY_LENGTH,
            pEncryptedEEPROMData->HardDriveKey, sizeof(pEncryptedEEPROMData->HardDriveKey) + sizeof(pEncryptedEEPROMData->GameRegion),
            (LPBYTE)&(pEncryptedEEPROMData->EncHeader) );
        if ( !bResult )
        {
            return E_FAIL;
        }
    }

    //
    // Set the manufacturing region flag
    //
    pEncryptedEEPROMData->GameRegion |= XC_GAME_REGION_MANUFACTURING;


    //
    // Encrypt the encrypted part back again
    //
    if(fWasEncrypted) {
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
    // Zero out the memory
    //
    RtlZeroMemory( buf, sizeof(buf) );

    return S_OK;
}




CXBoxVideo* g_pVideo;


VOID
DrawMessage(LPCWSTR pcszTextLine1, LPCWSTR pcszTextLine2, DWORD dwColor)
{
    ASSERT(g_pVideo && pcszTextLine1);

    g_pVideo->ClearScreen(dwColor);

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


    WCHAR szBuildString[64];
    wsprintfW(szBuildString, L"XTL Version %hs", VER_PRODUCTVERSION_STR);
    g_pVideo->DrawText(SCREEN_X_CENTER,
                       MENUBOX_BUILDNUM_Y1,
                       SCREEN_DEFAULT_TEXT_FG_COLOR,
                       SCREEN_DEFAULT_TEXT_BG_COLOR,
                       szBuildString);

    g_pVideo->ShowScreen();

}



#define MAX_RETRIES 3

//======================== The main function
void __cdecl main()
{
    HRESULT hr = E_FAIL;
    int tries = 0;

#if DBG
    XDebugOutLevel = XDBG_EXIT;
#endif

    //
    // Initialize our graphics.
    //
    if ((g_pVideo = new CXBoxVideo) == NULL) {

        XDBGWRN("factory", "Couldn't allocate video objects");
        Sleep(INFINITE);
    }

    g_pVideo->Initialize( SCREEN_WIDTH, SCREEN_HEIGHT );
    g_pVideo->ClearScreen(COLOR_BLACK);
    g_pVideo->ShowScreen();



    //
    // Unlock the hard drive.
    //
    if (SUCCEEDED(hr)) {

        tries = 0;
        hr = E_FAIL;

        while (tries < MAX_RETRIES && FAILED (hr)) {
            tries++;
            hr = UnlockHardDrive ();
        }
    }



    if (SUCCEEDED(hr))
    {
        //
        // Reset the manufacturing bit.
        // Per manufacturing request: We will retry MAX_RETRIES times before
        // giving up as failed.
        //
        tries = 0;
        hr = E_FAIL;
        while (tries < MAX_RETRIES && FAILED (hr)) {
            tries++;
            hr = SetManufacturingRegion ();
        }
    }

    //
    // Give status. We don't assume the user reads english, or understands the terminology
    // Although there is text, the background color is the distinguishing feature.
    // RED == Unable to update EEPROM data.
    // GREEN == EEPROM update succeeded.
    //
    if ( FAILED( hr ) )
    {
        XDBGTRC("factory", "UnlockXboxHardDrive or DecryptDevKitEEPROM failed (returned %x)",hr);
        DrawMessage (L"Drive Reset", L"Failed to reset hard disk security.", COLOR_TV_RED);
    }
    else
    {
        DrawMessage (L"Drive Reset", L"Xbox hard disk successfully reset.", COLOR_DARK_GREEN);
    }

    Sleep (INFINITE);

    return;
}
