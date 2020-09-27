// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:      EEPROMdump.cpp
//
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ INCLUDE FILES ++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


#include "recovpch.h"
#include "xboxvideo.h"


// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ DEFINES ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#if DBG
extern "C" ULONG XDebugOutLevel;
#endif

#define RC4_CONFOUNDER_LEN 8

// Threshold for analog buttons
#define XINPUT_ANALOGBUTTONTHRESHOLD 32

typedef void (*FPTR)(void);

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ STRUCTURES +++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//
// Header before encrypted data
//
typedef struct _RC4_SHA1_HEADER {
    UCHAR Checksum[XC_SERVICE_DIGEST_SIZE];
    UCHAR Confounder[RC4_CONFOUNDER_LEN];
} RC4_SHA1_HEADER, *PRC4_SHA1_HEADER;


// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ GLOBAL OBJECTS +++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// g_pVideo             -- Interface to the xbox video display
CXBoxVideo* g_pVideo;

// g_pBackgroundBitmap  -- The bitmap to display behind the text
BitmapFile* g_pBackgroundBitmap;

// g_szResult           -- Used to store result string displayed to user.
TCHAR g_szResult[1000];

EEPROM_LAYOUT* eeprom;
XBOX_FACTORY_SETTINGS* factorySettings;
XBOX_USER_SETTINGS* userSettings;
XBOX_ENCRYPTED_SETTINGS * pEncryptedEEPROMData;
BOOL fEncryptedEEPROM;
BOOL fEEPROMValid;
char RecoveryKey[8];

int DrawOffset = 0;

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ FUNCTIONS ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  DebugOutput
// Purpose:   Outputs a debug string to the console
// Arguments: tszErr        -- The format string
//            ...           -- Optional parameters
// Return:    None
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void DebugOutput(TCHAR *tszErr, ...)
{
    TCHAR tszErrOut[256];

    va_list valist;

    va_start (valist,tszErr);
    wvsprintf (tszErrOut, tszErr, valist);
    OutputDebugString (tszErrOut);

    va_end (valist);
}


void DrawInit()
{
    assert(g_pVideo && g_pBackgroundBitmap);
	g_pVideo->ClearScreen(COLOR_BLACK);
	g_pBackgroundBitmap->render(g_pVideo->GetD3DDevicePtr());
	DrawOffset = 0;
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  DrawMessage
// Purpose:   Outputs a one or two line message to the screen
// Arguments: pcszTextLine1 -- First line to output
//            pcszTextLine2 -- Second line to output
// Return:    None
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void DrawMessage(LPCWSTR pcszText)
{
    assert(g_pVideo);

    if (NULL != pcszText)
        g_pVideo->DrawText(SCREEN_X_CENTER,
                           MENUBOX_TEXTAREA_Y1 + (FONT_DEFAULT_HEIGHT * DrawOffset++),
                           SCREEN_DEFAULT_TEXT_FG_COLOR,
                           SCREEN_DEFAULT_TEXT_BG_COLOR,
                           pcszText);
}




// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  WaitForAnyButton
// Purpose:   Waits for a button to be pressed.
// UNDONE:    This function was cut-and-paste from recovery.cpp, and as such
//            contains a lot of unnecessary code.  This will eventually be
//            pared down to the minimal required functionality.
//            As a result, this function is left as-is and is uncommented.
// Arguments: None
// Return:    None
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void WaitForAnyButton()
{
    DWORD dwNewVideoMode;
    BOOL fYButton;
    PDWORD pdwNewVideoMode = &dwNewVideoMode;
    PBOOL pfYButton = &fYButton;
    BOOL fButtonPressed = FALSE;
    static DWORD dwPads = 0;
    DWORD dwInsertions, dwRemovals;
    static HANDLE hPads[XGetPortCount()] = { 0 };
    int i;

    ASSERT(pdwNewVideoMode);
    *pdwNewVideoMode = 0xFFFFFFFF;

    while (!fButtonPressed) {
        if (XGetDeviceChanges(XDEVICE_TYPE_GAMEPAD, &dwInsertions, &dwRemovals)) {
            dwPads |= dwInsertions;
            dwPads &= ~dwRemovals;

            for (i = 0; i < XGetPortCount(); i++) {
                if ((1 << i) & dwRemovals) {
                    if (NULL != hPads[i])
                        XInputClose(hPads[i]);
                        hPads[i] = NULL;
                }

                if ((1 << i) & dwInsertions) {
                    if (NULL == hPads[i])
                        hPads[i] = XInputOpen(XDEVICE_TYPE_GAMEPAD, XDEVICE_PORT0 + i, XDEVICE_NO_SLOT, NULL);;
                }
            }
        }

        for (i = 0; i < XGetPortCount(); i++) {
            if ((1 << i) & dwPads) {
                if (NULL != hPads[i]) {
                    XINPUT_STATE State;

                    if (ERROR_SUCCESS == XInputGetState(hPads[i], &State)) {
                        if ((State.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER] > XINPUT_ANALOGBUTTONTHRESHOLD) &&
                            (State.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_RIGHT_TRIGGER] > XINPUT_ANALOGBUTTONTHRESHOLD))
                        {
                            if (State.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP)
                                *pdwNewVideoMode = AV_STANDARD_PAL_I | AV_FLAGS_50Hz;
                            else if (State.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT)
                                *pdwNewVideoMode = AV_STANDARD_NTSC_M | AV_FLAGS_60Hz;
                            else if (State.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT)
                                *pdwNewVideoMode = AV_STANDARD_NTSC_J | AV_FLAGS_60Hz;
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
                                *pfYButton = (State.Gamepad.bAnalogButtons[XINPUT_GAMEPAD_Y] >
                                                XINPUT_ANALOGBUTTONTHRESHOLD);
                        }
                    }
                }
            }

            if (fButtonPressed || (*pdwNewVideoMode != 0xFFFFFFFF)) {
                fButtonPressed = TRUE;
                break;
            }
        }
    }
}

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

BOOL FReadEPROM()
{
    NTSTATUS status;
    BOOL bResult;
    HRESULT hr;
    BYTE b;
    int i;



    //
    // Use the EEPROM-KEY to decrypt the EEPROM
    //
    b = 0;
    for(i = 0; i < 8; ++i)
        b |= pEncryptedEEPROMData->Confounder[i];

    if (b)
    {
        bResult = rc4HmacDecrypt(
            (LPBYTE)(*XboxEEPROMKey), XBOX_KEY_LENGTH,
            pEncryptedEEPROMData->HDKey, sizeof(pEncryptedEEPROMData->HDKey) + sizeof(pEncryptedEEPROMData->GameRegion),
            (LPBYTE)&(pEncryptedEEPROMData->Checksum) );
        if ( !bResult )
        {
            return FALSE;
        }
        fEncryptedEEPROM = TRUE;
    } else
        fEncryptedEEPROM = FALSE;

    //
    // Double check the hard drive key
    //
    ASSERT( HARD_DRIVE_KEY_LEN == XBOX_KEY_LENGTH );
    if (!RtlEqualMemory( pEncryptedEEPROMData->HDKey, (LPBYTE)(*XboxHDKey), XBOX_KEY_LENGTH ))
    {
        ASSERT(!"HardDriveKey does not match");
        return FALSE;
    }

    MorphKeyByHardDriveKey( factorySettings->OnlineKey, sizeof(factorySettings->OnlineKey) );
    ComputeRecoveryKey( pEncryptedEEPROMData->HDKey, RecoveryKey );

    return TRUE;
}



void Screen1()
{
	DrawInit();

	wsprintf(g_szResult, TEXT("Kernel Ver : %d.%d.%d.%d"),
                 XboxKrnlVersion->Major, XboxKrnlVersion->Minor,
                 XboxKrnlVersion->Build, XboxKrnlVersion->Qfe & 0x7FFF);
    DrawMessage(g_szResult);

    if(!fEEPROMValid)
        DrawMessage(TEXT("CORRUPT EEPROM!"));
    else if(fEncryptedEEPROM)
        DrawMessage(TEXT("ENCRYPTED SECTION (DECRYPTED):"));
    else
        DrawMessage(TEXT("ENCRYPTED SECTION (UNENCRYPTED):"));
        
    DrawMessage(TEXT("Checksum:"));
    wsprintf(g_szResult, TEXT("%02x%02x%02x%02x%02x%02x%02x%02x %02x%02x%02x%02x%02x%02x%02x%02x %02x%02x%02x%02x"),
		pEncryptedEEPROMData->Checksum[0],  pEncryptedEEPROMData->Checksum[1],
		pEncryptedEEPROMData->Checksum[2],  pEncryptedEEPROMData->Checksum[3],
		pEncryptedEEPROMData->Checksum[4],  pEncryptedEEPROMData->Checksum[5],
		pEncryptedEEPROMData->Checksum[6],  pEncryptedEEPROMData->Checksum[7],
		pEncryptedEEPROMData->Checksum[8],  pEncryptedEEPROMData->Checksum[9],
		pEncryptedEEPROMData->Checksum[10], pEncryptedEEPROMData->Checksum[11],
		pEncryptedEEPROMData->Checksum[12], pEncryptedEEPROMData->Checksum[13],
		pEncryptedEEPROMData->Checksum[14], pEncryptedEEPROMData->Checksum[15],
		pEncryptedEEPROMData->Checksum[16], pEncryptedEEPROMData->Checksum[17],
		pEncryptedEEPROMData->Checksum[18], pEncryptedEEPROMData->Checksum[19]
    );
    DrawMessage(g_szResult);

    wsprintf(g_szResult, TEXT("Confounder : %02x%02x%02x%02x %02x%02x%02x%02x"),
    	pEncryptedEEPROMData->Confounder[0],  pEncryptedEEPROMData->Confounder[1],
    	pEncryptedEEPROMData->Confounder[2],  pEncryptedEEPROMData->Confounder[3],
    	pEncryptedEEPROMData->Confounder[4],  pEncryptedEEPROMData->Confounder[5],
    	pEncryptedEEPROMData->Confounder[6],  pEncryptedEEPROMData->Confounder[7]
    );
    DrawMessage(g_szResult);

    DrawMessage(TEXT("Hard Drive Key"));
    wsprintf(g_szResult, TEXT("%02x%02x%02x%02x %02x%02x%02x%02x %02x%02x%02x%02x %02x%02x%02x%02x"),
    	pEncryptedEEPROMData->HDKey[0],  pEncryptedEEPROMData->HDKey[1],
    	pEncryptedEEPROMData->HDKey[2],  pEncryptedEEPROMData->HDKey[3],
    	pEncryptedEEPROMData->HDKey[4],  pEncryptedEEPROMData->HDKey[5],
    	pEncryptedEEPROMData->HDKey[6],  pEncryptedEEPROMData->HDKey[7],
    	pEncryptedEEPROMData->HDKey[8],  pEncryptedEEPROMData->HDKey[9],
    	pEncryptedEEPROMData->HDKey[10], pEncryptedEEPROMData->HDKey[11],
    	pEncryptedEEPROMData->HDKey[12], pEncryptedEEPROMData->HDKey[13],
    	pEncryptedEEPROMData->HDKey[14], pEncryptedEEPROMData->HDKey[15]
    );
    DrawMessage(g_szResult);

    wsprintf(g_szResult, TEXT("Game Region: %08x"), pEncryptedEEPROMData->GameRegion);
    DrawMessage(g_szResult);

    g_pVideo->ShowScreen();
}

void Screen2()
{
	DrawInit();

    DrawMessage(TEXT("FACTORY SECTION:"));

    wsprintf(g_szResult, TEXT("Checksum: %08x"), factorySettings->Checksum);
    DrawMessage(g_szResult);

    DrawMessage(TEXT("Serial Number:"));
    wsprintf(g_szResult, TEXT("'%c%c%c%c%c%c%c%c%c%c%c%c'"),
    	factorySettings->SerialNumber[0], factorySettings->SerialNumber[1],
    	factorySettings->SerialNumber[2], factorySettings->SerialNumber[3],
    	factorySettings->SerialNumber[4], factorySettings->SerialNumber[5],
    	factorySettings->SerialNumber[6], factorySettings->SerialNumber[7],
    	factorySettings->SerialNumber[8], factorySettings->SerialNumber[9],
    	factorySettings->SerialNumber[10], factorySettings->SerialNumber[11]
    );
    DrawMessage(g_szResult);

    wsprintf(g_szResult, TEXT("MAC Addr: %02x%02x %02x%02x %02x%02x"),
        factorySettings->EthernetAddr[0], factorySettings->EthernetAddr[1],
        factorySettings->EthernetAddr[2], factorySettings->EthernetAddr[3],
        factorySettings->EthernetAddr[4], factorySettings->EthernetAddr[5]
    );
    DrawMessage(g_szResult);

    DrawMessage(TEXT("Online Key:"));
    wsprintf(g_szResult, TEXT("%02x%02x%02x%02x %02x%02x%02x%02x %02x%02x%02x%02x %02x%02x%02x%02x"),
        factorySettings->OnlineKey[0], factorySettings->OnlineKey[1],
        factorySettings->OnlineKey[2], factorySettings->OnlineKey[3],
        factorySettings->OnlineKey[4], factorySettings->OnlineKey[5],
        factorySettings->OnlineKey[6], factorySettings->OnlineKey[7],
        factorySettings->OnlineKey[8], factorySettings->OnlineKey[9],
        factorySettings->OnlineKey[10], factorySettings->OnlineKey[11],
        factorySettings->OnlineKey[12], factorySettings->OnlineKey[13],
        factorySettings->OnlineKey[14], factorySettings->OnlineKey[15]
	);
    DrawMessage(g_szResult);

    wsprintf(g_szResult, TEXT("AV Region  : %08x"), factorySettings->AVRegion);
    DrawMessage(g_szResult);

    g_pVideo->ShowScreen();
}

void Screen3()
{
	DrawInit();

    DrawMessage(TEXT("USER DATA SECTION (Part 1):"));

    wsprintf(g_szResult, TEXT("Checksum: %08x"), userSettings->Checksum);
    DrawMessage(g_szResult);

    wsprintf(g_szResult, TEXT("TZ Bias: %d"), userSettings->TimeZoneBias);
    DrawMessage(g_szResult);

    wsprintf(g_szResult, TEXT("TZ Name Std: %c%c%c   Dlt: %c%c%c"),
    	userSettings->TimeZoneStdName[0],
    	userSettings->TimeZoneStdName[1],
    	userSettings->TimeZoneStdName[2],
    	userSettings->TimeZoneDltName[0],
    	userSettings->TimeZoneDltName[1],
    	userSettings->TimeZoneDltName[2]
    );
    DrawMessage(g_szResult);

    wsprintf(g_szResult, TEXT("TZ Std Bias: %d   Dlt Bias: %d"),
	userSettings->TimeZoneStdBias, userSettings->TimeZoneDltBias
    );
    DrawMessage(g_szResult);

	wsprintf(g_szResult, TEXT("Date Std Event: %d/%d (%d of 7) %02d:00"),
		userSettings->TimeZoneStdDate.Month,
		userSettings->TimeZoneStdDate.Day,
		userSettings->TimeZoneStdDate.DayOfWeek,
		userSettings->TimeZoneStdDate.Hour
	);
	DrawMessage(g_szResult);


	wsprintf(g_szResult, TEXT("Date Dlt Event: %d/%d (%d of 7) %02d:00"),
		userSettings->TimeZoneDltDate.Month,
		userSettings->TimeZoneDltDate.Day,
		userSettings->TimeZoneDltDate.DayOfWeek,
		userSettings->TimeZoneDltDate.Hour
	);
	DrawMessage(g_szResult);

    wsprintf(g_szResult, TEXT("Language: %08x"), userSettings->Language);
    DrawMessage(g_szResult);

    wsprintf(g_szResult, TEXT("Audio/Video Flags: %08x / %08x"), userSettings->AudioFlags, userSettings->VideoFlags);
    DrawMessage(g_szResult);

    g_pVideo->ShowScreen();

    ULONG ParentalControlGames;             // 60 +  4
    ULONG ParentalControlPassword;          // 64 +  4
    ULONG ParentalControlMovies;            // 68 +  4
    ULONG OnlineIpAddress;                  // 72 +  4
    ULONG OnlineDnsAddress;                 // 76 +  4
    ULONG OnlineDefaultGatewayAddress;      // 80 +  4
    ULONG OnlineSubnetMask;                 // 84 +  4
    ULONG MiscFlags;                        // 88 +  4
    ULONG DvdRegion;                        // 92 +  4
}

void Screen4()
{
	DrawInit();

    DrawMessage(TEXT("USER DATA SECTION (Part 2):"));

    wsprintf(g_szResult, TEXT("ParentalControlGames: %08x"), userSettings->ParentalControlGames);
    DrawMessage(g_szResult);

    wsprintf(g_szResult, TEXT("ParentalControlPassword: %08x"), userSettings->ParentalControlPassword);
    DrawMessage(g_szResult);

    wsprintf(g_szResult, TEXT("ParentalControlMovies: %08x"), userSettings->ParentalControlMovies);
    DrawMessage(g_szResult);

    wsprintf(g_szResult, TEXT("Online IP Addr: %d.%d.%d.%d"),
    	(userSettings->OnlineIpAddress & 0xff000000) >> 24,
    	(userSettings->OnlineIpAddress & 0xff0000) >> 16,
    	(userSettings->OnlineIpAddress & 0xff00) >> 8,
    	(userSettings->OnlineIpAddress & 0xff)
   	);
    DrawMessage(g_szResult);

    wsprintf(g_szResult, TEXT("Online DNS Addr: %d.%d.%d.%d"),
    	(userSettings->OnlineDnsAddress & 0xff000000) >> 24,
    	(userSettings->OnlineDnsAddress & 0xff0000) >> 16,
    	(userSettings->OnlineDnsAddress & 0xff00) >> 8,
    	(userSettings->OnlineDnsAddress & 0xff)
   	);
    DrawMessage(g_szResult);

    wsprintf(g_szResult, TEXT("Online Gateway Addr: %d.%d.%d.%d"),
    	(userSettings->OnlineDefaultGatewayAddress & 0xff000000) >> 24,
    	(userSettings->OnlineDefaultGatewayAddress & 0xff0000) >> 16,
    	(userSettings->OnlineDefaultGatewayAddress & 0xff00) >> 8,
    	(userSettings->OnlineDefaultGatewayAddress & 0xff)
   	);
    DrawMessage(g_szResult);

    wsprintf(g_szResult, TEXT("Online Subnet Mask: %d.%d.%d.%d"),
    	(userSettings->OnlineSubnetMask & 0xff000000) >> 24,
    	(userSettings->OnlineSubnetMask & 0xff0000) >> 16,
    	(userSettings->OnlineSubnetMask & 0xff00) >> 8,
    	(userSettings->OnlineSubnetMask & 0xff)
   	);
    DrawMessage(g_szResult);

    wsprintf(g_szResult, TEXT("Misc Flags: %08x"), userSettings->MiscFlags);
    DrawMessage(g_szResult);

    wsprintf(g_szResult, TEXT("DVD Region: %08x"), userSettings->DvdRegion);
    DrawMessage(g_szResult);

    g_pVideo->ShowScreen();
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  main
// Purpose:   Main entry point to application
// Arguments: None
// Return:    None
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void __cdecl main()
{
    BYTE buf[EEPROM_TOTAL_MEMORY_SIZE];
    DWORD type, size;
	int current = 0;
	FPTR Screens[4] = {Screen1, Screen2, Screen3, Screen4};

    wsprintf(g_szResult, TEXT("Generic Failure"));

	//
    // Initialize core peripheral port support
    //
    XInitDevices(0, NULL);

	//
    // Create the video subsytem and background bitmap objects
	//
    g_pVideo = new CXBoxVideo;
    g_pBackgroundBitmap = new BitmapFile;
    if ((NULL == g_pVideo) || (NULL == g_pBackgroundBitmap))
    {
        OutputDebugString(TEXT("Couldn't allocate video objects"));
        Sleep(INFINITE);
    }

	//
    // Initialize the screen and read in the background bitmap
    //
    g_pBackgroundBitmap->read(0, 0, FILE_DATA_IMAGE_DIRECTORY_A "\\backgrnd.bmp");
    g_pVideo->Initialize( SCREEN_WIDTH, SCREEN_HEIGHT );

    //
    // read EEPROM Data
    //
    if (!NT_SUCCESS(ExQueryNonVolatileSetting(XC_MAX_ALL, &type, buf, sizeof(buf), &size)))
    {
        ASSERT(false);
    }

    ASSERT( type == REG_BINARY );
    ASSERT( size == EEPROM_TOTAL_MEMORY_SIZE );

    eeprom = (EEPROM_LAYOUT*) buf;
    factorySettings = (XBOX_FACTORY_SETTINGS*) &eeprom->FactorySection;
    userSettings = (XBOX_USER_SETTINGS*) &eeprom->UserConfigSection;
    pEncryptedEEPROMData = (XBOX_ENCRYPTED_SETTINGS *) &eeprom->EncryptedSection;

	fEEPROMValid = FReadEPROM();

	while (true)
	{
		Screens[current]();
		WaitForAnyButton();
		current = (current+1)%4;
		Sleep(150);
	}

    Sleep(INFINITE);
}
