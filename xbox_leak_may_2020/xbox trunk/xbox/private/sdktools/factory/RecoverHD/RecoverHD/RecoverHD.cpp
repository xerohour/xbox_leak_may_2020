// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:      RecoverHD.cpp
//
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ INCLUDE FILES ++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#include "recovpch.h"
#include "xboxvideo.h"
#include <init.h>
#include <xconfig.h>
#include <xboxp.h>


// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ DEFINES ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// MAX_IMAGE_FILE_SIZE   -- The maximum size of a recovery image file.
//                          This MUST match the value in MakeRecImg.
#define MAX_IMAGE_FILE_SIZE          0x3000000     // 48 MB

// COPY_BUFFER_SIZE      -- Size of the buffer that holds the copied data
#define COPY_BUFFER_SIZE             32768

// Threshold for analog buttons
#define XINPUT_ANALOGBUTTONTHRESHOLD 32


// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ STRUCTURES +++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// sImageHeader          -- Contains information about the image files.
//                          Written to the start of the first image file.
typedef struct
{
    USHORT usVerMajor, usVerMinor, usVerBuild, usVerQfe;
    DWORD cImageFiles;
    DWORD dwChecksum;
	ULARGE_INTEGER uliSize;
} sImageHeader;

// sFileHeader           -- Contains information about a particular file in
//                          an image file.  'szRelPathName' is the relative
//                          path and file name.
typedef struct
{
    char szRelPathName[MAX_PATH];
    DWORD dwSize;
} sFileHeader;


// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ GLOBAL OBJECTS +++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// g_pVideo             -- Interface to the xbox video display
CXBoxVideo* g_pVideo;

// g_pBackgroundBitmap  -- The bitmap to display behind the text
BitmapFile* g_pBackgroundBitmap;

// g_szResult           -- Used to store result string displayed to user.
TCHAR g_szResult[1000];
TCHAR g_szResult2[1000];

// g_cbyRead            -- Number of bytes read from the current section
DWORD g_cbyRead;

// g_cbyTotalRead       -- Number of bytes read from all sections
int g_cbyTotalRead;

// g_ih                 -- Image header
sImageHeader g_ih;

// g_pbySection         -- Location in the current section
BYTE *g_pbySection;

// g_iCurSection        -- Current section number
int g_iCurSection;

// g_xsum               -- Ongoing checksum of files in the image.
CCheckSum g_xsum;

// g_tszQuickMBtoWC     -- Used by QuickMBtoWC for quick multibyte-to-wide
//                         char conversions.
TCHAR g_tszQuickMBtoWC[2000];

extern COBJECT_STRING HdPartition1;
extern COBJECT_STRING HdPartition2;
extern COBJECT_STRING HdPartition3;
extern COBJECT_STRING CDrive;
extern COBJECT_STRING YDrive;
extern COBJECT_STRING ZDrive;


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

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  ResetUserEEPROMSettings
// Purpose:   Resets the user data section of the EEPROM
// Arguments: None
// Return:    'true' if successful, 'false' otherwise
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
BOOL ResetUserEEPROMSettings(void)
{
    EEPROM_LAYOUT Data;
    XBOX_USER_SETTINGS *UserData;
    NTSTATUS st;
    ULONG Type, Size;

    st = ExQueryNonVolatileSetting(XC_MAX_ALL, &Type, &Data, sizeof Data,
        &Size);
    if(!NT_SUCCESS(st))
        return FALSE;

    UserData = (XBOX_USER_SETTINGS *)Data.UserConfigSection;
    memset(UserData, 0, sizeof *UserData);

    if (XGetGameRegion() & XC_GAME_REGION_JAPAN) {
        UserData->Language = XC_LANGUAGE_JAPANESE;
        UserData->TimeZoneBias = -540;
        strcpy(UserData->TimeZoneStdName, "TST");
        strcpy(UserData->TimeZoneDltName, "TST");
    }

    UserData->Checksum = ~XConfigChecksum(UserData, sizeof *UserData);

    st = ExSaveNonVolatileSetting(XC_MAX_ALL, REG_BINARY, &Data, sizeof Data);
    return NT_SUCCESS(st);
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  QuickMBtoWC
// Purpose:   Supplies easy inlined multibyte-to-wide char conversions.
//            Must not call this function twice in the same function call.
// Arguments: sz            -- String to convert
// Return:    Wide-char version of passed-in string.
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
TCHAR *QuickMBtoWC(char *sz)
{
    memset(g_tszQuickMBtoWC, 0, sizeof g_tszQuickMBtoWC);
    mbstowcs(g_tszQuickMBtoWC, sz, strlen(sz));
    return g_tszQuickMBtoWC;
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  DrawMessage
// Purpose:   Outputs a one or two line message to the screen
// Arguments: pcszTextLine1 -- First line to output
//            pcszTextLine2 -- Second line to output
// Return:    None
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void DrawMessage(LPCWSTR pcszTextLine1, LPCWSTR pcszTextLine2,
                 LPCWSTR pcszTextLine3 = NULL)
{
    // If the video display or background bitmap failed to load, then we
    // should never have gotten here.
    assert(g_pVideo && g_pBackgroundBitmap);

    // Set the screen back to it's original state (just the background
    // image)
    g_pVideo->ClearScreen(COLOR_BLACK);
    g_pBackgroundBitmap->render(g_pVideo->GetD3DDevicePtr());


    if (NULL != pcszTextLine1)
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

    if (NULL != pcszTextLine3)
        g_pVideo->DrawText(SCREEN_X_CENTER,
                           MENUBOX_TEXTAREA_Y1 + FONT_DEFAULT_HEIGHT * 8,
                           SCREEN_DEFAULT_TEXT_FG_COLOR,
                           SCREEN_DEFAULT_TEXT_BG_COLOR,
                           pcszTextLine3);
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  UpdateProgress
// Purpose:   Updates the onscreen progress display.  This includes the
//            progress bar and 'time remaining' displays.
// Arguments: None
// Return:    None
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void UpdateProgress()
{
    // Keep track of the last progress output so that we can avoid
    // unnecessary screen renders.
    static DWORD s_dwProgPercent = -1;

    // Calculate the percentage of the disc that's been handled.
    DWORD dwProgPercent = (DWORD)((__int64)g_cbyTotalRead * 100 /
                                           g_ih.uliSize.QuadPart);

    // We only need to update if the onscreen display may change
    if (dwProgPercent != s_dwProgPercent) {
        DrawMessage(TEXT("Restoring HD"), TEXT("Please wait"));

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
                          PROGRESS_X1 + ((PROGRESS_WIDTH / 100) *
                                         dwProgPercent),
                          PROGRESS_Y2,
                          SCREEN_DEFAULT_TEXT_FG_COLOR);

        g_pVideo->ShowScreen();

        s_dwProgPercent = dwProgPercent;
    }
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

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  NextSection
// Purpose:   Closes the current section and moves on to the next one.
// Arguments: None
// Return:    'true' if successful, 'false' otherwise
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
BOOL NextSection()
{
    char szSection[20];

    if (g_iCurSection != -1)
    {
        sprintf(szSection, ".RCVR%d", g_iCurSection);
        DebugOutput(TEXT("Freeing section '%s'.\n"), QuickMBtoWC(szSection));
        if (!XFreeSection(szSection))
        {
            wsprintf(g_szResult, TEXT("Failed to Release section '%s'"),
                                 QuickMBtoWC(szSection));
            return FALSE;
        }
    }

    g_iCurSection++;

    sprintf(szSection, ".RCVR%d", g_iCurSection);
    DebugOutput(TEXT("Loading section '%s'.\n"), QuickMBtoWC(szSection));
    g_pbySection = (BYTE*)XLoadSection(szSection);
    if (g_pbySection == NULL)
    {
        wsprintf(g_szResult, TEXT("Section '%s' expected but not found"),
                             QuickMBtoWC(szSection));
        return FALSE;
    }

    // Reset the number of bytes read in the current section
    g_cbyRead = 0;

    return TRUE;
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  ReadBuffer
// Purpose:   Reads the specified number of bytes from the image file.
//            Automatically handles switching to the next section as
//            necessary
// Arguments: pvBuffer          -- Buffer to hold the read data.
//            dwBytes           -- Number of bytes to read.
//            fChecksum         -- Whether to add bytes to checksum
// Return:    'TRUE' if successful, 'FALSE' otherwise
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
BOOL ReadBuffer(void *pvBuffer, DWORD dwBytes, bool fChecksum)
{
    DWORD dwBytesToRead;

    assert(dwBytes < MAX_IMAGE_FILE_SIZE);

ReadBytes:

    dwBytesToRead = min(dwBytes, MAX_IMAGE_FILE_SIZE - g_cbyRead);
    if (dwBytesToRead)
    {
//      Contemplate: pvBuffer = g_pbySection + g_cbyRead;
        memcpy(pvBuffer, g_pbySection + g_cbyRead, dwBytesToRead);
        dwBytes -= dwBytesToRead;
        g_cbyRead += dwBytesToRead;
        g_cbyTotalRead += dwBytesToRead;

        if (fChecksum)
            g_xsum.SumBytes((BYTE*)pvBuffer, dwBytesToRead);
    }

    if (dwBytes)
    {
        // Need to move to the next section
        if (!NextSection())
            return FALSE;

        pvBuffer = (BYTE*)pvBuffer + dwBytesToRead;
        goto ReadBytes;
    }

    UpdateProgress();

    return TRUE;
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  SetupDirectoryTree
// Purpose:
// Arguments:
// Return:    'true' if successful, 'false' otherwise
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
BOOL SetupDirectoryTree(char *szFile)
{
    char szPath[MAX_PATH];
    char szNew[MAX_PATH];

    // Separate the path alone (ie ignore the filename for now) since that's
    // what we want to create.
    strcpy(szPath, szFile);
    char *pszSlash = strrchr(szPath, '\\');
    if (!pszSlash)
    {
        // Root file; no directory to create
        return TRUE;
    }

    *pszSlash = '\0';

    // szNew will contain the full path to the current depth, while szCur
    // will contain the name of just the current directory; it will get
    // repeatedly appended onto szNew to iteratively create the entire
    // directory chain.
    char *szCur = strtok(szPath, "\\/");

    // Skip the drive designation
    strcpy(szNew, szCur);
    szCur = strtok(NULL, "\\/");

    while (szCur)
    {
        // Append the current directory onto the full-path string.
        strcat(szNew, "\\");
        strcat(szNew, szCur);

        // Try to create the full path to the current depth.
        CreateDirectory(szNew, NULL);

        // Get the name of the next-depth directory.
        szCur = strtok(NULL, "\\/");
    }

    return TRUE;
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  StrToUpper
// Purpose:   Convert an arbitrary-cased string to upper case.
// Arguments: szSource      -- String to convert.
//            szDest        -- Buffer to hold upper case version of szSource
// Return:    None
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void StrToUpper(char *szSource, char *szDest)
{
    while (*szSource)
        *szDest++ = (char)toupper(*szSource++);
    *szDest = *szSource;
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  ExtractNextFile
// Purpose:   Extracts the contents of the next file in the image, and
//            writes it to disk.  Generates the necessary directories.
// Arguments: None
// Return:    'TRUE' if successful, 'FALSE' otherwise
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
BOOL ExtractNextFile()
{
    sFileHeader filehdr;
    char        szFilePathName[MAX_PATH];
    char        szFullPathUpper[MAX_PATH];

    // Read the file header
    if (!ReadBuffer(&filehdr, sizeof filehdr, false))
    {
        wsprintf(g_szResult, TEXT("Failed to read file header"));
        return FALSE;
    }

    DebugOutput(TEXT("Extracting file '%s' (%d bytes)\n"),
                QuickMBtoWC(filehdr.szRelPathName), filehdr.dwSize);

    // Files MUST start with 'XDASH'.
    StrToUpper(filehdr.szRelPathName, szFullPathUpper);
    if (memcmp(szFullPathUpper, "XDASH", 5))
    {
        wsprintf(g_szResult, TEXT("Invalid File '%s'"),
                 QuickMBtoWC(szFilePathName));
        return FALSE;
    }

    // Extract files to temporary partition.
    sprintf(szFilePathName, "Z:\\%s", filehdr.szRelPathName + 6);

    // Generate necessary directory tree
    if (!SetupDirectoryTree(szFilePathName))
        return FALSE;

    // Create the new file on the xbox
    HANDLE hfile = CreateFile(szFilePathName, GENERIC_WRITE, 0, NULL,
                              CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hfile == INVALID_HANDLE_VALUE)
    {
        wsprintf(g_szResult, TEXT("Failed to create '%s' (%08X)"),
                 QuickMBtoWC(szFilePathName), GetLastError());
        return FALSE;
    }

    // Extract the contents of the file from the image
    char rgbyCopyBuffer[32768];
    DWORD dwBytesToCopy = filehdr.dwSize;

    while (dwBytesToCopy)
    {
        DWORD dw, dwBytes = min(32768, dwBytesToCopy);
        if (!ReadBuffer(&rgbyCopyBuffer, dwBytes, true))
        {
            wsprintf(g_szResult, TEXT("Failed to read from image"));
            CloseHandle(hfile);
            return FALSE;
        }

        // Write the bytes
        if (!WriteFile(hfile, rgbyCopyBuffer, dwBytes, &dw, NULL) ||
            dw != dwBytes)
        {
            wsprintf(g_szResult, TEXT("Failed to write to '%s'"),
                     QuickMBtoWC(filehdr.szRelPathName));
            CloseHandle(hfile);
            return FALSE;
        }

        dwBytesToCopy -= dwBytes;
    }

    // Close the file
    CloseHandle(hfile);

    return TRUE;
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  XCopyFiles
// Purpose:   Recursively copies a directory.
// Arguments: szSourceDir       -- Source directory to copy
//            szDestDir         -- Destination to copy files to.
// Return:    'true' if successful, 'false' otherwise
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
BOOL XCopyFiles(char *szSourceDir, char *szDestDir)
{
    WIN32_FIND_DATA wfd;
    char szSearchString[MAX_PATH];

    // Find all files in 'szSourceDir' and copy them to 'szDestDir'.  Recurse
    // into directories.
    sprintf(szSearchString, "%s\\*", szSourceDir);
    HANDLE hfile = FindFirstFile(szSearchString, &wfd);
    if (hfile == INVALID_HANDLE_VALUE)
        return FALSE;

    do
    {
        char szFullSourcePath[MAX_PATH];
        char szFullDestPath[MAX_PATH];

        // Ignore '.' and '..'
        if (!strcmp(wfd.cFileName, ".") || !strcmp(wfd.cFileName, ".."))
            continue;

        // Generate the full pathname to the current file.
        sprintf(szFullSourcePath, "%s\\%s", szSourceDir, wfd.cFileName);
        sprintf(szFullDestPath,   "%s\\%s", szDestDir,   wfd.cFileName);

        DebugOutput(TEXT("Copying %s "), QuickMBtoWC(szFullSourcePath));

        if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            // Recurse into the directory
            if (!CreateDirectory(szFullDestPath, NULL))
                return FALSE;

            DebugOutput(TEXT("to %s (recurse)\n"), QuickMBtoWC(szFullDestPath));
            if (!XCopyFiles(szFullSourcePath, szFullDestPath))
                return FALSE;
        }
        else
        {
            // Copy the file
            DebugOutput(TEXT("to %s\n"), QuickMBtoWC(szFullDestPath));
            if (!CopyFile(szFullSourcePath, szFullDestPath, TRUE))
                return FALSE;
        }

    } while (FindNextFile(hfile, &wfd));

    // We've enumerated all files in 'szSourceDir'
    FindClose(hfile);

    return TRUE;
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  VerifyVersion
// Purpose:   Verifies that the xbox and this app's versions match.
// Arguments:
// Return:    'true' if successful, 'false' otherwise
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
BOOL VerifyVersion(XBOX_KRNL_VERSION *pverXbox,
                   XBOX_KRNL_VERSION *pverRecoverHD)
{
    int nMajor, nMinor, nBuild, nQFE;

    // Store the Xbox version
    memcpy(pverXbox, XboxKrnlVersion, sizeof XBOX_KRNL_VERSION);

    // Mask out the debug bit
    pverXbox->Qfe &= 0x7FFF;

    // Store the RecoverHD version
    sscanf(VER_PRODUCTVERSION_STR, "%d.%d.%d.%d", &nMajor, &nMinor, &nBuild,
                                                  &nQFE);
    *(WORD*)(&pverRecoverHD->Major) = (WORD)nMajor;
    *(WORD*)(&pverRecoverHD->Minor) = (WORD)nMinor;
    *(WORD*)(&pverRecoverHD->Build) = (WORD)nBuild;
    *(WORD*)(&pverRecoverHD->Qfe)   = (WORD)nQFE;

    return (pverXbox->Major == pverRecoverHD->Major &&
            pverXbox->Minor == pverRecoverHD->Minor &&
            pverXbox->Build == pverRecoverHD->Build &&
            pverXbox->Qfe   == pverRecoverHD->Qfe);
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  main
// Purpose:   Main entry point to application
// Arguments: None
// Return:    None
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void __cdecl main()
{
    XBOX_KRNL_VERSION verXbox, verRecoverHD;
    TCHAR szVersion[20];

    wsprintf(g_szResult, TEXT("Generic Failure"));

    g_pbySection  = NULL;
    g_iCurSection = -1;

    // Initialize core peripheral port support
    XInitDevices(0, NULL);

    // Create the video subsytem and background bitmap objects
    g_pVideo = new CXBoxVideo;
    g_pBackgroundBitmap = new BitmapFile;
    if ((NULL == g_pVideo) || (NULL == g_pBackgroundBitmap))
    {
        OutputDebugString(TEXT("Couldn't allocate video objects"));
        Sleep(INFINITE);
    }

    // Initialize the screen and read in the background bitmap
    g_pBackgroundBitmap->read(0, 0,
                              FILE_DATA_IMAGE_DIRECTORY_A "\\backgrnd.bmp");
    g_pVideo->Initialize( SCREEN_WIDTH, SCREEN_HEIGHT );

    // Load the first section (so that we can get the image header).
    if (!NextSection())
        goto done;

    // Store the image header (since g_pbySection will go away as soon as
    // we unload the section).
    memcpy(&g_ih, g_pbySection, sizeof g_ih);
    wsprintf(szVersion, TEXT("%d.%d.%d.%d"), g_ih.usVerMajor, g_ih.usVerMinor,
                                             g_ih.usVerBuild, g_ih.usVerQfe);
    DrawMessage(TEXT("Press any button on the controller to restore"),
                TEXT("the Xbox HD to its original state."), szVersion);

    g_pVideo->ShowScreen();

    // Wait for them to press a button.
    WaitForAnyButton();

    g_cbyTotalRead = 0;

    if (g_ih.uliSize.QuadPart == 0)
    {
        g_ih.uliSize.QuadPart = 1;
    }

    UpdateProgress();

    // Load the first section containing data
    if (!NextSection())
        goto done;

    // Verify that the kernel version matches

    // 0x7FFF to mask out the debug bit
    if (XboxKrnlVersion->Major != 1 ||
        XboxKrnlVersion->Minor != 0 ||
        (XboxKrnlVersion->Build != 3944 && XboxKrnlVersion->Build != 4034) ||
        (XboxKrnlVersion->Qfe & 0x7FFF) != 1)
    {
        wsprintf(g_szResult, TEXT("Incorrect RecoverHD version"));
        wsprintf(g_szResult2, TEXT("Xbox: %d.%d.%d.%d, RecoverHD: %d.%d.%d.%d"),
                 XboxKrnlVersion->Major, XboxKrnlVersion->Minor,
                 XboxKrnlVersion->Build, XboxKrnlVersion->Qfe & 0x7FFF,
                 g_ih.usVerMajor, g_ih.usVerMinor, g_ih.usVerBuild,
                 g_ih.usVerQfe);

        goto done;
    }

    // Format the HD to remove anything that was previously there.
    // NOTE: Does NOT touch the Y partition (partition 2) -- we don't want
    // to format that until we're sure the data was successfully extracted
    // from the xbe.
    if (!FormatHD())
        goto done;

    DebugOutput(TEXT("%d files to extract, %d bytes, Checksum=0x%08X.\n"),
                g_ih.cImageFiles, g_ih.uliSize.LowPart, g_ih.dwChecksum);

    if (!NT_SUCCESS(IoCreateSymbolicLink((POBJECT_STRING) &CDrive,
                                         (POBJECT_STRING) &HdPartition1)))
    {
        wsprintf(g_szResult, TEXT("Failed to link C to Partition1"));
        goto done;
    }

    if (!NT_SUCCESS(IoCreateSymbolicLink((POBJECT_STRING) &YDrive,
                                         (POBJECT_STRING) &HdPartition2)))
    {
        wsprintf(g_szResult, TEXT("Failed to link Y to Partition2"));
        goto done;
    }

    // Mount the utility drive so that we can use it for temporary space
    if (!XMountUtilityDrive(TRUE))
    {
        wsprintf(g_szResult, TEXT("Failed to mount Z:"));
        goto done;
    }

    // Add the empty TDATA and UDATA directories.  Note that this is not
    // necessary if we are doing more than a retail recovery.
    if (!CreateDirectory("C:\\TDATA", NULL))
    {
        wsprintf(g_szResult, TEXT("Failed to create C:\\TDATA"));
        goto done;
    }

    if (!CreateDirectory("C:\\UDATA", NULL))
    {
        wsprintf(g_szResult, TEXT("Failed to create C:\\UDATA"));
        goto done;
    }

    // We write everything to a temp partition first in case there's invalid
    // data in one of the sections.  After all data is successfully written,
    // we copy it to Parititon2 (Y:).  By writing to the temp partition
    // first, if we fail out part-way due to invalid data, then we know we
    // haven't FUBAR'ed the dash partition.

    // Step over all of the image file sections; create files as we go
    for (DWORD i = 0; i < g_ih.cImageFiles; i++)
    {
        if (!ExtractNextFile())
            goto done;
    }

    // Do the checksums match?
    if (g_ih.dwChecksum != g_xsum.DwFinalSum())
    {
        wsprintf(g_szResult, TEXT("Checksums do not match: 0x%08X, 0x%08X"),
                 g_ih.dwChecksum, g_xsum.DwFinalSum());
        goto done;
    }

    // At this point, we're confident that the data on the disc is valid, so
    // we can start mucking about with the Y partition and EEPROM
    if (!ResetUserEEPROMSettings())
    {
        wsprintf(g_szResult, TEXT("Failed to reset EEPROM"));
        goto done;
    }

    if (!FormatYPartition())
    {
        wsprintf(g_szResult, TEXT("Failed to format the Y partition"));
        goto done;
    }

    DrawMessage(TEXT("Moving Files..."), TEXT(""));

    g_pVideo->ShowScreen();

    // Copy the data from the utility drive to the Y Drive
    if (!XCopyFiles("Z:", "Y:"))
    {
        wsprintf(g_szResult, TEXT("Failed to copy files from Z to Y"));
        goto done;
    }

    // Wipe partition 3 (by clearing out the cache database)
    if (!WipeCacheDB())
    {
        wsprintf(g_szResult, TEXT("Failed to clear the Cache DB"));
        goto done;
    }

    // If here, then we were successful
    wsprintf(g_szResult, TEXT("Xbox HD successfully restored.  Remove the"));
	wsprintf(g_szResult2, TEXT("DVD and power off or reboot the Xbox now."));

done:

    DebugOutput(TEXT("\ng_szResult: '%s'.\n\n\n"), g_szResult);

    // We're done; report the results to the user
	DrawMessage(g_szResult, g_szResult2);
    g_pVideo->ShowScreen();

    Sleep(INFINITE);
}
