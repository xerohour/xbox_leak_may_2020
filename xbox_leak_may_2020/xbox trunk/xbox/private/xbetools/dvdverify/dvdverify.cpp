// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:      dvdverify.cpp
//
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ INCLUDE FILES +++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#include "recovpch.h"
#include "xboxverp.h"
#include "xboxvideo.h"
#include "imagefile.h"
#include "xsum.h"
#include <idex.h>
#include <assert.h>


#define VERSION_SIG "XBOX_DVD_LAYOUT_TOOL_SIG"

struct VERSION_INFO 
{
    BYTE rgbySig[32];
    BYTE rgbyLayoutVersion[8];
    BYTE rgbyPremasterVersion[8];
    BYTE rgPadding[2048-32-8-8];    // fill with zeros
};
        
#define ONE_SECTOR 2048
#define SECTOR_BATCH 16 // batch 16 sectors at a time for optimization.

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ GLOBAL OBJECTS ++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// g_pVideo             -- Interface to the xbox video display
CXBoxVideo* g_pVideo;

// g_pBackgroundBitmap  -- The bitmap to display behind the text & progress bar
BitmapFile* g_pBackgroundBitmap;

// DVDDevice            -- The internal name of dvd drive.
COBJECT_STRING DVDDevice = CONSTANT_OBJECT_STRING(OTEXT("\\Device\\Cdrom0"));

// g_cSectors           -- Total number of sectors on the dvd (media area)
DWORD g_cSectors;

// g_dwTimeStart        -- Time that the verification started
DWORD g_dwTimeStart;

VERSION_INFO g_verinfo;
bool g_fVersionValid, g_fDone;
TCHAR g_szLayoutVersion[100], g_szPremasterVersion[100];
TCHAR g_szSignature[100], g_szAuthCheckMatch[100];


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ EXTERNAL FUNCTIONS ++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// AuthenticateDisc     -- Verifies that the disc in the drive is an
    //                         authentic DVD-X2.
extern bool AuthenticateDisc(DVDX2_AUTHENTICATION Authentication,
                             DVDX2_CONTROL_DATA ControlData,
                             PDEVICE_OBJECT DeviceObject,
                             TCHAR *szResult);


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ FUNCTIONS +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  DrawMessage
// Purpose:   Outputs a one or two line message to the screen
// Arguments: pcszTextLine1 -- First line to output
//            pcszTextLine2 -- Second line to output
// Return:    None
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
VOID DrawMessage(LPCWSTR pcszTextLine1, LPCWSTR pcszTextLine2)
{
    // If the video display or background bitmap failed to load, then we should
    // never have gotten here.
    assert(g_pVideo && g_pBackgroundBitmap);

    // Set the screen back to it's original state (just the background
    // image)
    g_pVideo->ClearScreen(COLOR_BLACK);
    g_pBackgroundBitmap->render(g_pVideo->GetD3DDevicePtr());

    if (NULL != pcszTextLine2)
        g_pVideo->DrawText(SCREEN_X_CENTER,
                           MENUBOX_TEXTAREA_Y1,
                           SCREEN_DEFAULT_TEXT_FG_COLOR,
                           SCREEN_DEFAULT_TEXT_BG_COLOR,
                           pcszTextLine1);

    if (NULL != pcszTextLine2)
        g_pVideo->DrawText(SCREEN_X_CENTER,
                           MENUBOX_TEXTAREA_Y1 + FONT_DEFAULT_HEIGHT +
                           ITEM_VERT_SPACING,
                           SCREEN_DEFAULT_TEXT_FG_COLOR,
                           SCREEN_DEFAULT_TEXT_BG_COLOR,
                           pcszTextLine2);

    if (g_fDone)
    {
        // Add version info line
        if (!g_fVersionValid)
            g_pVideo->DrawText(SCREEN_X_CENTER,
                               MENUBOX_TEXTAREA_Y2 - 30,
                               SCREEN_DEFAULT_TEXT_FG_COLOR,
                               SCREEN_DEFAULT_TEXT_BG_COLOR,
                               TEXT("No version info on disc"));
        else
        {
            g_pVideo->DrawText(SCREEN_X_CENTER,
                               MENUBOX_TEXTAREA_Y2 - 50,
                               SCREEN_DEFAULT_TEXT_FG_COLOR,
                               SCREEN_DEFAULT_TEXT_BG_COLOR,
                               g_szLayoutVersion);
            g_pVideo->DrawText(SCREEN_X_CENTER,
                               MENUBOX_TEXTAREA_Y2 - 50 + FONT_DEFAULT_HEIGHT +
                               ITEM_VERT_SPACING,
                               SCREEN_DEFAULT_TEXT_FG_COLOR,
                               SCREEN_DEFAULT_TEXT_BG_COLOR,
                               g_szPremasterVersion);
        }
        g_pVideo->DrawText(SCREEN_X_CENTER,
                           MENUBOX_TEXTAREA_Y2 - 65 - FONT_DEFAULT_HEIGHT +
                           ITEM_VERT_SPACING,
                           SCREEN_DEFAULT_TEXT_FG_COLOR,
                           SCREEN_DEFAULT_TEXT_BG_COLOR,
                           g_szSignature);

        g_pVideo->DrawText(SCREEN_X_CENTER,
                           MENUBOX_TEXTAREA_Y2 - 70 - FONT_DEFAULT_HEIGHT * 2+
                           ITEM_VERT_SPACING,
                           SCREEN_DEFAULT_TEXT_FG_COLOR,
                           SCREEN_DEFAULT_TEXT_BG_COLOR,
                           g_szAuthCheckMatch);
    }
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  UpdateProgress
// Purpose:   Updates the onscreen progress display.  This includes the
//            progress bar and 'time remaining' displays.
// Arguments: dwLSN     -- The current LSN being handled.  Used to determine
//                         percentage complete.
// Return:    None
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
VOID UpdateProgress(DWORD dwLSN)
{
    // Keep track of the last progress output so that we can avoid unnecessary
    // screen renders.
    static DWORD s_dwProgPercent = -1;

    // Calculate the percentage of the disc that's been handled.
    DWORD dwProgPercent = dwLSN * 100 / g_cSectors;

    // We only need to update if the onscreen display may change
    if (dwProgPercent !=  s_dwProgPercent) {
        DrawMessage(TEXT("Verifying disc"), TEXT("Please wait"));

        // Draw progress bar background
        g_pVideo->DrawBox(PROGRESS_X1 - PROGRESS_BORDER_OUTER_OFFSET,
                          PROGRESS_Y1 - PROGRESS_BORDER_OUTER_OFFSET,
                          PROGRESS_X2 + PROGRESS_BORDER_OUTER_OFFSET,
                          PROGRESS_Y2 + PROGRESS_BORDER_OUTER_OFFSET,
                          COLOR_BRIGHT_GREEN);

        // Draw progress bar bkgnd
        g_pVideo->DrawBox(PROGRESS_X1 - PROGRESS_BORDER_GAP,
                          PROGRESS_Y1 - PROGRESS_BORDER_GAP,
                          PROGRESS_X2 + PROGRESS_BORDER_GAP,
                          PROGRESS_Y2 + PROGRESS_BORDER_GAP,
                          COLOR_BLACK);

        // Draw progress bar
        g_pVideo->DrawBox(PROGRESS_X1,
                          PROGRESS_Y1,
                          PROGRESS_X1 + ((PROGRESS_WIDTH / 100) * dwProgPercent),
                          PROGRESS_Y2,
                          SCREEN_DEFAULT_TEXT_FG_COLOR);

        g_pVideo->ShowScreen();

        s_dwProgPercent = dwProgPercent;
    }
}


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  OpenDVD
// Purpose:   Sends the appropriate command to the DVD drive commanding it to
//            open.
// Arguments: None
// Return:    None
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void OpenDVD()
{
    HalWriteSMBusByte(SMC_SLAVE_ADDRESS, SMC_COMMAND_DVD_TRAY_OPERATION,
                      SMC_DVD_TRAY_OPERATION_OPEN);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CloseDVD
// Purpose:   Sends the appropriate command to the DVD drive commanding it to
//            close.
// Arguments: None
// Return:    None
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CloseDVD()
{
    HalWriteSMBusByte(SMC_SLAVE_ADDRESS, SMC_COMMAND_DVD_TRAY_OPERATION,
                      SMC_DVD_TRAY_OPERATION_CLOSE);
}

// Threshold for analog buttons
#define XINPUT_ANALOGBUTTONTHRESHOLD 32

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  WaitForAnyButton
// Purpose:   Waits for a button to be pressed.
// UNDONE:    This function was cut-and-paste from recovery.cpp, and as such
//            contains a lot of unnecessary code.  This will eventually be
//            pared down to the minimal required functionality.
//            As a result, this function is left as-is and is uncommented.
// Arguments: None
// Return:    None
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
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

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  MSBtoLSB
// Purpose:   Converts a big-endian value to little-endian.  Does the same
//            thing as 'ntohl()', which is part of winsock (which the XDK
//            doesn't appear to include).
// Arguments: dwMSB         -- The big-endian value to convert
// Return:    dwLSB         -- The little-endian equivalent of dwMSB
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
DWORD MSBtoLSB(DWORD dwMSB)
{
    return (((dwMSB & 0xFF000000) >> 24) |
            ((dwMSB & 0x00FF0000) >>  8) |
            ((dwMSB & 0x0000FF00) <<  8) |
            ((dwMSB & 0x000000FF) << 24));
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  main
// Purpose:   Main entry point to application
// Arguments: None 
// Return:    None
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void __cdecl main()
{
    // szResult     -- Contains result string of the verification process
    TCHAR           szResult[1000];

    // szTemp       -- Temporary string buffer
    TCHAR           szTemp[100];

    // xsum         -- Ongoing checksum of all data on disc (in media region)
    CCheckSum       xsum;

    // fFirstTime   -- true if this is the first disc handled
    bool            fFirstTime = true;
    
    SCSI_PASS_THROUGH_DIRECT PassThrough;
    DVDX2_CONTROL_DATA ControlData;
    DVDX2_AUTHENTICATION Authentication;
    PDEVICE_OBJECT DeviceObject;
    PCDB Cdb = (PCDB)&PassThrough.Cdb;
    OBJECT_ATTRIBUTES oa;
    LARGE_INTEGER liByteOffset;

    // Status vars
    NTSTATUS        Status;
    IO_STATUS_BLOCK ioStatus;
    
    // Initialize core peripheral port support
    XInitDevices(0, NULL);

    // Prepare the SCSI pass through structure.
    RtlZeroMemory(&PassThrough, sizeof(SCSI_PASS_THROUGH_DIRECT));
    PassThrough.Length = sizeof(SCSI_PASS_THROUGH_DIRECT);

    // Create the video subsytem and background bitmap objects
    g_pVideo = new CXBoxVideo;
    g_pBackgroundBitmap = new BitmapFile;
    if ((NULL == g_pVideo) || (NULL == g_pBackgroundBitmap)) {
        OutputDebugString(TEXT("Couldn't allocate video objects"));
        Sleep(INFINITE);
    }

    // Initialize the screen and read in the background bitmap
    g_pBackgroundBitmap->read(0, 0, FILE_DATA_IMAGE_DIRECTORY_A "\\backgrnd.bmp");
    g_pVideo->Initialize( SCREEN_WIDTH, SCREEN_HEIGHT );

    g_fDone = false;
    // Loop forever.
    while (true) {
        // Ask the user to insert a disc...
        if (fFirstTime) {
			DrawMessage(TEXT("Insert a DVD-X2 disc to verify"),
						TEXT("and then press any button on the controller"));
            fFirstTime = false;
        }
		else
	        DrawMessage(szResult, TEXT("Insert a new disc to verify and ")\
                                  TEXT("press any button"));
        g_pVideo->ShowScreen();

        // Open the tray...
        OpenDVD();

        // ...and wait for them to press a button.
        WaitForAnyButton();

        g_fDone = false;

        DrawMessage(TEXT("Authenticating disc"), TEXT("Please Wait"));
        g_pVideo->ShowScreen();

        // Close the tray
        CloseDVD();

        // Get a reference to the dvd object so that we can query it for info.
        Status = ObReferenceObjectByName((POBJECT_STRING)&DVDDevice, 0,
                                         IoDeviceObjectType, NULL,
                                         (PVOID*)&DeviceObject);
        if (!NT_SUCCESS(Status)) {
            wsprintf(szResult, TEXT("ERROR: Failed to open DVD-X2 reference ") \
                               TEXT("(error 0x%08X)."), Status);
            // Note: Don't "goto done" since we don't want to close the dvd ref
            continue;
        }

        // Request the authentication page from the DVD-X2 drive.
        OutputDebugString(TEXT("\r\nRequesting Authentication Data...\r\n"));
        
        RtlZeroMemory(&Authentication, sizeof(DVDX2_AUTHENTICATION));
        RtlZeroMemory(&PassThrough, sizeof(SCSI_PASS_THROUGH_DIRECT));
        PassThrough.Length = sizeof(SCSI_PASS_THROUGH_DIRECT);
        PassThrough.DataIn = SCSI_IOCTL_DATA_IN;
        PassThrough.DataBuffer = &Authentication;
        PassThrough.DataTransferLength = sizeof(DVDX2_AUTHENTICATION);
        
        RtlZeroMemory(Cdb, sizeof(CDB));
        Cdb->MODE_SENSE10.OperationCode = SCSIOP_MODE_SENSE10;
        Cdb->MODE_SENSE10.PageCode      = DVDX2_MODE_PAGE_AUTHENTICATION;
        *((PUSHORT)&Cdb->MODE_SENSE10.AllocationLength) =
              (USHORT)IdexConstantUshortByteSwap(sizeof(DVDX2_AUTHENTICATION));

        Sleep(20000);

Read1:
        Status = IoSynchronousDeviceIoControlRequest(IOCTL_SCSI_PASS_THROUGH_DIRECT,
            DeviceObject, &PassThrough, sizeof(SCSI_PASS_THROUGH_DIRECT), NULL, 0,
            NULL, FALSE);

        if (!NT_SUCCESS(Status)) {

            //
            // If the drive reported back an authentication failure at this point,
            // then change the error to STATUS_UNRECOGNIZED_MEDIA to help
            // distinguish between a disc that doesn't contain any anti-piracy
            // protection from a disc that failed the anti-piracy checks below.
            //
            switch(Status) {
            case STATUS_DEVICE_NOT_READY:
                OutputDebugString(TEXT("Device wasn't ready to read auth data - Sleeping 1/2 second...\r\n"));
                Sleep(500);
                goto Read1;

            case STATUS_TOO_MANY_SECRETS:
                wsprintf(szResult, TEXT("ERROR: Too many secrets."));
                goto done;

            case STATUS_UNRECOGNIZED_MEDIA:
                wsprintf(szResult, TEXT("ERROR: Unrecognized media."));
                goto done;
            
            case STATUS_NO_MEDIA_IN_DEVICE:
                wsprintf(szResult, TEXT("ERROR: No media in device."));
                goto done;
            
            default:
                wsprintf(szResult, TEXT("ERROR: Failed to get auth data ") \
                                   TEXT("(error 0x%08X)."), Status);
                goto done;
            }
        }

        OutputDebugString(TEXT("Obtained Authentication Data...\r\n"));


        // Check if the DVD-X2 drive thinks that this is a valid CDF header.
        if (Authentication.AuthenticationPage.CDFValid != DVDX2_CDF_VALID) {
            wsprintf(szResult, TEXT("ERROR: Unrecognized media"));
            goto done;
        }



        // Request the xcontrol region from the drive. this is necessary to
        // obtain the desired drive geometry -- we need to determine how many
        // sectors are present on the disc.
        OutputDebugString(TEXT("Requesting XControl Data...\r\n"));

        PassThrough.DataIn = SCSI_IOCTL_DATA_IN;
        PassThrough.DataBuffer = &ControlData;
        PassThrough.DataTransferLength = sizeof(DVDX2_CONTROL_DATA);

        RtlZeroMemory(Cdb, sizeof(CDB));
        Cdb->READ_DVD_STRUCTURE.OperationCode = SCSIOP_READ_DVD_STRUCTURE;
        *((PUSHORT)&Cdb->READ_DVD_STRUCTURE.AllocationLength) = (USHORT)IdexConstantUshortByteSwap(sizeof(DVDX2_CONTROL_DATA));
        *((PULONG)Cdb->READ_DVD_STRUCTURE.RMDBlockNumber)     = IdexConstantUlongByteSwap((ULONG)(~DVDX2_CONTROL_DATA_BLOCK_NUMBER));
        Cdb->READ_DVD_STRUCTURE.LayerNumber = (UCHAR)(~DVDX2_CONTROL_DATA_LAYER);
        Cdb->READ_DVD_STRUCTURE.Format      = DvdPhysicalDescriptor;
        Cdb->READ_DVD_STRUCTURE.Control     = 0xC0;
Read:
        Status = IoSynchronousDeviceIoControlRequest(IOCTL_SCSI_PASS_THROUGH_DIRECT,
                  DeviceObject, &PassThrough, sizeof(SCSI_PASS_THROUGH_DIRECT),
                  NULL, 0, NULL, FALSE);

        if (!NT_SUCCESS(Status)) {
            switch(Status) {
            case STATUS_DEVICE_NOT_READY:
                OutputDebugString(TEXT("Device wasn't ready to read xcontrol")\
                                  TEXT(" - Sleeping 1/2 second...\r\n"));
                Sleep(500);
                goto Read;

            case STATUS_UNRECOGNIZED_MEDIA:
                wsprintf(szResult, TEXT("ERROR: Unrecognized media."));
                goto done;
            
            case STATUS_NO_MEDIA_IN_DEVICE:
                wsprintf(szResult, TEXT("ERROR: No media in device."));
                goto done;
            
            default:
                wsprintf(szResult, TEXT("ERROR: Failed to get xcontrol ") \
                                   TEXT("(error 0x%08X)."), Status);
                goto done;
            }
        }

        if (!AuthenticateDisc(Authentication, ControlData, DeviceObject, szResult))
            goto done;

        // If here, then authentication succeeded and szResult contains the 
        // type of disc signature (red or green). Store it away for later display
        wcscpy(g_szSignature, szResult);
        wsprintf(szResult, TEXT("Disc signature: %s\r\n"), g_szSignature);
        OutputDebugString(szResult);

        DrawMessage(TEXT("Verifying disc"), TEXT("Please Wait"));
        g_pVideo->ShowScreen();
        OutputDebugString(TEXT("Disc authenticated!\r\n"));
        
        // Calculate the total number of bytes in the media region
        // The data returned from the dvd drive is in MSB format; we need it in
        // LSB, so convert now
        DWORD dwLayer0Start = MSBtoLSB(ControlData.LayerDescriptor.StartingDataSector);
        DWORD dwLayer0End   = MSBtoLSB(ControlData.LayerDescriptor.EndLayerZeroSector);
        DWORD dwLayer1Start = 0xFFFFFF ^ dwLayer0End;
        DWORD dwLayer1End   = MSBtoLSB(ControlData.LayerDescriptor.EndDataSector);
        
        g_cSectors = (dwLayer0End - dwLayer0Start + 1)  + (dwLayer1End - dwLayer1Start + 1); 
        wsprintf(szResult, TEXT("Sectors on dvd: %d\r\n"), g_cSectors);
        OutputDebugString(szResult);

        // Read the version info of tools (xbLayout, xbPremaster) used to
        // generate this disc
        liByteOffset.QuadPart = 33 * 2048;  // Location of version info
        Status = IoSynchronousFsdRequest(IRP_MJ_READ, DeviceObject,
                                         &g_verinfo, ONE_SECTOR,&liByteOffset);
        if (!NT_SUCCESS(Status)) {
            // Failed to read the block containing the version information
            wsprintf(szResult, TEXT("Error (0x%08X) reading version data"), 
                               Status);
            goto done;
        }
        g_fVersionValid = !memcmp(g_verinfo.rgbySig, VERSION_SIG,
                                  sizeof(VERSION_SIG));
        if (g_fVersionValid)
        {
            wsprintf(g_szLayoutVersion, TEXT("xbLayout version: %d.%d.%d.%d"),
                   *(WORD*)&g_verinfo.rgbyLayoutVersion[0],
                   *(WORD*)&g_verinfo.rgbyLayoutVersion[2],
                   *(WORD*)&g_verinfo.rgbyLayoutVersion[4],
                   *(WORD*)&g_verinfo.rgbyLayoutVersion[6]);
            wsprintf(g_szPremasterVersion, TEXT("xbPremaster version: %d.%d.%d.%d"),
                   *(WORD*)&g_verinfo.rgbyPremasterVersion[0],
                   *(WORD*)&g_verinfo.rgbyPremasterVersion[2],
                   *(WORD*)&g_verinfo.rgbyPremasterVersion[4],
                   *(WORD*)&g_verinfo.rgbyPremasterVersion[6]);
        }

        // Read all data on the disc, calculating the green checksum as we go.
        BYTE rgby[ONE_SECTOR * SECTOR_BATCH];
        DWORD dwSector = 0;
        int i = 0, cPlaceholders = 0;
        g_dwTimeStart = GetTickCount();
        while (dwSector < g_cSectors) {
            // Debugging output
            if (((i++)%1000) == 0)
            {
                wsprintf(szTemp, TEXT("Sector %d  (xsum = 0x%08X)\r\n"),
                         dwSector, xsum.DwRunningSum());
                OutputDebugString(szTemp);
            }
            
            // Read the next batch of sectors.
            DWORD dwSectorsToRead = min(SECTOR_BATCH, g_cSectors - dwSector);
            liByteOffset.QuadPart = (__int64)dwSector * ONE_SECTOR;
            Status = IoSynchronousFsdRequest(IRP_MJ_READ, DeviceObject,
                                             rgby, ONE_SECTOR*dwSectorsToRead,
                                             &liByteOffset);
            if (!NT_SUCCESS(Status)) {
                if (Status != STATUS_INVALID_DEVICE_REQUEST) {
                    wsprintf(szResult, TEXT("Error (0x%08X) reading sector %d"), 
                                       Status, dwSector);
                    goto done;
                }

                // Failed to read a batch.  Assumption is because there's a
                // placeholder somewhere in the batch (or we're near the end of
                // the disc).  We're not sure where the read failed in there,
                // so we reread one sector at a time; the first one that fails
                // is the start of the placeholder.
                wsprintf(szTemp, TEXT("Failed to read batch at %d (Error 0x%08X). Reading")\
                                 TEXT(" per-sector: "), dwSector, Status);
                OutputDebugString(szTemp);

                int iSector = 0;
                while(dwSector < g_cSectors) {
                    // Sanity check - we should never read more than
                    // SECTOR_BATCH blocks in this inner loop.
                    assert (iSector < SECTOR_BATCH);

                    OutputDebugString(TEXT("."));
                    Status = IoSynchronousFsdRequest(IRP_MJ_READ, DeviceObject,
                                                     rgby, ONE_SECTOR,
                                                     &liByteOffset);
                    if (!NT_SUCCESS(Status)) {
                        if (Status != STATUS_INVALID_DEVICE_REQUEST) {
                            wsprintf(szResult, TEXT("Error (0x%08X) reading sector %d"), 
                                     Status, dwSector);
                            goto done;
                        }

                        // Failed to read the sector - this must be the PH!

                        // Do basic verification that it's not a disc error by
                        // verifying that the data at 'dwSector + 4095' is not
                        // readable, and 'dwSector + 4096' is.
                        LARGE_INTEGER liTemp = liByteOffset;
                        liTemp.QuadPart += 4095 * 2048;
                        Status = IoSynchronousFsdRequest(IRP_MJ_READ, DeviceObject,
                                                         rgby, ONE_SECTOR,
                                                         &liTemp);
                        if (NT_SUCCESS(Status)) {
                            // Successfully read the sector 4095 sectors after
                            // the start of the presumed placeholder -- therefore
                            // it's not a placeholder!
                            wsprintf(szTemp, TEXT("\r\nDisc error found ")
                                             TEXT("at sector 0x%06X\r\n"),
                                             dwSector);
                            OutputDebugString(szTemp);
                            wsprintf(szResult, TEXT("Disc error 1 found at sector 0x%06X"), 
                                     dwSector);
                            goto done;
                        }

                        // Verify the next sector (dwSector + 4096) is readable
                        liTemp.QuadPart += 2048;
                        Status = IoSynchronousFsdRequest(IRP_MJ_READ, DeviceObject,
                                                         rgby, ONE_SECTOR,
                                                         &liTemp);
                        if (!NT_SUCCESS(Status)) {
                            // Failed to read the sector 4096 sectors after
                            // the start of the presumed placeholder -- 
                            // therefore it's not a placeholder!
                            wsprintf(szTemp, TEXT("\r\nDisc error found ")
                                             TEXT("at sector 0x%06X\r\n"),
                                             dwSector);
                            OutputDebugString(szTemp);
                            wsprintf(szResult, TEXT("Disc error 2 found at sector 0x%06X"), 
                                     dwSector);
                            goto done;
                        }

                        // increment the placeholder count -- if there're more
                        // than 16, then an error occurred somewhere
                        cPlaceholders++;
                        if (cPlaceholders > 16)
                        {
                            wsprintf(szTemp, TEXT("\r\nToo many placeholders ")
                                             TEXT("determined at sector ")
                                             TEXT("0x%06X\r\n"), dwSector);
                            OutputDebugString(szTemp);
                            wsprintf(szResult, TEXT("Disc error 3 found at sector 0x%06X"), 
                                     dwSector);
                            goto done;
                        }

                        // If here, then it's a placeholder -- skip it
                        wsprintf(szTemp, TEXT("\r\nFailed to read block %d ")\
                                         TEXT("from DVD-X2.  Skipping to ") \
                                         TEXT("%d\r\n"), dwSector,
                                         dwSector + 4096);
                        OutputDebugString(szTemp);
                        dwSector += 4096;
                        break;
                    }

                    // successfully read the sector.  Checksum the data
                    xsum.SumBytes(rgby, ONE_SECTOR);
                    liByteOffset.QuadPart += ONE_SECTOR;
                    dwSector++;
                    iSector++;
                }
            } else {
                // Checksum the data
                xsum.SumBytes(rgby, ONE_SECTOR * dwSectorsToRead);

                // Update the progress bar
                UpdateProgress(dwSector);

                // Track how many sectors we just processed
                dwSector += dwSectorsToRead;
            }
        }

        // Generate the checksum string for display
        wsprintf(szResult, TEXT("Xbox Media CRC: 0x%08X"), xsum.DwFinalSum());

        // Output diagnostic info to kd
        wsprintf(szTemp, TEXT("Finished checksum:  %d sectors read, xsum=0x%08X\r\n"),
                 dwSector, xsum.DwFinalSum());
        OutputDebugString(szTemp);

done:
        // No longer need the reference to the dvd device, so release it.
        ObDereferenceObject(DeviceObject);

        // Reset the checksum for the next disc
        xsum.Reset();
        
		// Loop back around to output the message and wait for the next disc.
        g_fDone = true;
    }

    // We should never get here.  Cleanup anyways
    if (g_pVideo)
        delete g_pVideo;
    if (g_pBackgroundBitmap)
        delete g_pBackgroundBitmap;

    ASSERT(false);
}
