// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:      CStep_DestSource.cpp
// Contents:  In this step, the user enters the source and destination paths for the desired action
// Revisions: 31-May-2001: Created (jeffsim)
//
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ INCLUDE FILES +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// "stdafx.h"       -- Precompiled header file
#include "stdafx.h"


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ GLOBAL VARIABLES ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

extern char g_szDefaultFLD[MAX_PATH], g_szDefaultPKG[MAX_PATH], g_szDefaultOutputFile[MAX_PATH];
extern int g_iDefaultTapeDrive;

extern bool VerifyPackageFile(char *szPKGFile);

// Structures and defines for timestamp verification
typedef struct
{
    WORD wMajor;
    WORD wMinor;
    WORD wBuild;
    WORD wQFE;
} sVersion;

#define FLD_SIG "XBOXFLDFILE"
#define FLD_SIG_SIZE sizeof(FLD_SIG)

typedef struct
{
    BYTE  szSig[FLD_SIG_SIZE];
    time_t timeSaved;
    sVersion version;
} sFLDHeader;

// Taken from AMC's file "MediaBase.h"
struct TFileHeader
{
    char m_szFileType[32]; // FST? Error Map?
    char m_szMediaType[32]; // DVD, CD, etc
};

struct TMediaHeader
{
    DWORD uNumSectorsLayer0;
    DWORD uNumSectorsLayer1;
    BYTE m_ImpUseArea[120];       // total = 128 bytes
};


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ FUNCTIONS +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CStep_DestSource::CStep_DestSource
// Purpose:   CStep_DestSource constructor.
// Arguments: pdlg          -- The dialog in which the step will appear.
//            action        -- The action to perform (write, verity, etc)
// Return:    None
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
CStep_DestSource::CStep_DestSource(CDlg *pdlg) : CStep(pdlg)
{
    // When run internally, we allow an option which sources off of a premaster
    // "package" file.  This is allowed if a particular regkey is present
    HKEY hkey;
    if ((RegOpenKeyEx(HKEY_LOCAL_MACHINE,
                      TEXT("SOFTWARE\\Microsoft\\XboxSDK\\xbPremaster"), 0,
                      KEY_READ, &hkey) == ERROR_SUCCESS))
    {
        m_fAllowReadFromPackage = RegQueryValueEx(hkey, "ReadFromPackage",
                                      NULL, NULL, NULL, NULL) == ERROR_SUCCESS;
        m_fAllowToDisk          = RegQueryValueEx(hkey, "RenderToDisk",
                                      NULL, NULL, NULL, NULL) == ERROR_SUCCESS;
        RegCloseKey(hkey);
    }
    else
    {
        m_fAllowReadFromPackage = false;
        m_fAllowToDisk          = false;
    }

    m_pdlg->DisplayControl(IDC_DESTSOURCE_RBTN_TAPE);
    m_pdlg->DisplayControl(IDC_DESTSOURCE_TEXT_SETDEST);
    m_pdlg->DisplayControl(IDC_DESTSOURCE_RBTN_ELEC);
    m_pdlg->DisplayControl(IDC_DESTSOURCE_TEXT_TITLE);
    m_pdlg->DisplayControl(IDC_DESTSOURCE_STATIC);
    m_pdlg->DisplayControl(IDC_DESTSOURCE_STATIC2);
    m_pdlg->DisplayControl(IDC_DESTSOURCE_LINE);
    m_pdlg->DisplayControl(IDC_STATIC3);
    m_pdlg->DisplayControl(IDC_STATIC4);

    if (m_fAllowReadFromPackage)
    {
        m_pdlg->DisplayControl(IDC_DESTSOURCE_TEXT_SETFLDPATH2);
        m_pdlg->DisplayControl(IDC_DESTSOURCE_EDIT_SETFLDPATH2);
        m_pdlg->DisplayControl(IDC_DESTSOURCE_BTN_FLDPATH2);
        m_pdlg->DisplayControl(IDC_DESTSOURCE_RBTN_PACKAGE);
        m_pdlg->DisplayControl(IDC_DESTSOURCE_RBTN_FLD);
        
        if (lstrcmpi(g_szDefaultPKG, ""))
            SetSource(SOURCE_PACKAGE);
        else
            SetSource(SOURCE_FLD);

        m_pdlg->SetControlText(IDC_DESTSOURCE_EDIT_SETFLDPATH2, g_szDefaultPKG);        

        // Also allow render to disk for testing purposes
        if (m_fAllowToDisk)
            m_pdlg->DisplayControl(IDC_DESTSOURCE_RBTN_DISK);
    }
    else
        m_pdlg->DisplayControl(IDC_DESTSOURCE_TEXT_SETFLDPATH);

    m_pdlg->DisplayControl(IDC_DESTSOURCE_BTN_FILEPATH);
    m_pdlg->DisplayControl(IDC_DESTSOURCE_EDIT_SETDISKPATH);
    m_pdlg->DisplayControl(IDC_DESTSOURCE_EDIT_SETFLDPATH);
    m_pdlg->DisplayControl(IDC_DESTSOURCE_BTN_FLDPATH);
    m_pdlg->DisplayControl(IDC_DESTSOURCE_COMBO_TAPEDRIVES);
    m_pdlg->DisplayControl(IDC_BTN_PREV);
    m_pdlg->DisplayControl(IDC_BTN_NEXT);
    m_pdlg->DisplayControl(IDC_BTN_EXIT);
    m_pdlg->SetControlEnable(IDC_BTN_PREV, true);

    // By default the user starts with 'tape' as the media, so set the appropriate media controls.
    SetMedia(MEDIA_TAPE);
    
    // Overload the background colors of some controls to white
    m_pdlg->OverloadBkColor(IDC_DESTSOURCE_TEXT_TITLE);

    m_pdlg->SetControlFont(IDC_DESTSOURCE_TEXT_TITLE, -11, "Arial", FW_BOLD);

    // Set the FLD path text control as the default control
    m_pdlg->SetCurControl(IDC_DESTSOURCE_EDIT_SETFLDPATH);

    m_pdlg->SetControlText(IDC_DESTSOURCE_EDIT_SETFLDPATH, g_szDefaultFLD);
    m_pdlg->SetControlText(IDC_DESTSOURCE_EDIT_SETDISKPATH, "");

#ifdef _DEBUG1
    m_pdlg->SetControlText(IDC_DESTSOURCE_EDIT_SETDISKPATH, "C:\\jeff.pkg");
#endif
    
    // Update the list of available tape drives (in case the media is disk)
    UpdateAvailableTapeDrives();
}

bool CStep_DestSource::DoModal()
{
    // If the user specified the necessary information on the command line,
    // then go straight into the progress step
    if (g_iDefaultTapeDrive != -1)
    {
        HandleButtonPress(IDC_BTN_NEXT);
        return true;
    }
            
    return false;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CStep_DestSource::SetSource
// Purpose:   
// Arguments: source        -- The new source medium
// Return:    None
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CStep_DestSource::SetSource(eSource source)
{
    m_source = source;
    switch(source)
    {
    case SOURCE_FLD:
        m_pdlg->SetControlState(IDC_DESTSOURCE_RBTN_FLD, true);
        m_pdlg->SetControlState(IDC_DESTSOURCE_RBTN_PACKAGE, false);
        m_pdlg->SetControlEnable(IDC_DESTSOURCE_EDIT_SETFLDPATH, true);
        m_pdlg->SetControlEnable(IDC_DESTSOURCE_EDIT_SETFLDPATH2, false);
        m_pdlg->SetControlEnable(IDC_DESTSOURCE_BTN_FLDPATH, true);
        m_pdlg->SetControlEnable(IDC_DESTSOURCE_BTN_FLDPATH2, false);
        m_pdlg->SetControlEnable(IDC_DESTSOURCE_RBTN_ELEC, true);
        break;

    case SOURCE_PACKAGE:
        m_pdlg->SetControlState(IDC_DESTSOURCE_RBTN_FLD, false);
        m_pdlg->SetControlState(IDC_DESTSOURCE_RBTN_PACKAGE, true);
        m_pdlg->SetControlEnable(IDC_DESTSOURCE_EDIT_SETFLDPATH, false);
        m_pdlg->SetControlEnable(IDC_DESTSOURCE_EDIT_SETFLDPATH2, true);
        m_pdlg->SetControlEnable(IDC_DESTSOURCE_BTN_FLDPATH, false);
        m_pdlg->SetControlEnable(IDC_DESTSOURCE_BTN_FLDPATH2, true);
        m_pdlg->SetControlEnable(IDC_DESTSOURCE_RBTN_ELEC, false);
        if (m_media == MEDIA_ELECSUB)
            SetMedia(MEDIA_TAPE);
        break;
    }
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CStep_DestSource::SetMedia
// Purpose:   Modifies the UI to indicate what selected media type is currently selected.
// Arguments: media         -- The media (disk, tape, ...) to use.
// Return:    None
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CStep_DestSource::SetMedia(eMedia media)
{
    // Track what the currently selected media is
    m_media = media;
    
    // Update the controls accordingly
    switch(media)
    {
    case MEDIA_ELECSUB:
        m_pdlg->SetControlEnable(IDC_DESTSOURCE_EDIT_SETDISKPATH, true);
        m_pdlg->SetControlEnable(IDC_DESTSOURCE_BTN_FILEPATH, true);
        m_pdlg->SetControlEnable(IDC_DESTSOURCE_COMBO_TAPEDRIVES, false);
        m_pdlg->SetControlState(IDC_DESTSOURCE_RBTN_ELEC, true);
        m_pdlg->SetControlState(IDC_DESTSOURCE_RBTN_DISK, false);
        m_pdlg->SetControlState(IDC_DESTSOURCE_RBTN_TAPE, false);
        m_pdlg->SetControlEnable(IDC_BTN_NEXT, true);
        break;

    case MEDIA_TAPE:
        m_pdlg->SetControlEnable(IDC_DESTSOURCE_BTN_FILEPATH, false);
        m_pdlg->SetControlEnable(IDC_DESTSOURCE_COMBO_TAPEDRIVES, true);
        m_pdlg->SetControlEnable(IDC_DESTSOURCE_EDIT_SETDISKPATH, false);
        m_pdlg->SetControlState(IDC_DESTSOURCE_RBTN_ELEC, false);
        m_pdlg->SetControlState(IDC_DESTSOURCE_RBTN_DISK, false);
        m_pdlg->SetControlState(IDC_DESTSOURCE_RBTN_TAPE, true);

        // Update the list of available tape drives
        UpdateAvailableTapeDrives();

        // Don't allow the user to click 'next' if there aren't any tape drives
        m_pdlg->SetControlEnable(IDC_BTN_NEXT, (m_cTapeDrives > 0) ? true : false);

        break;

    case MEDIA_DISK:
        m_pdlg->SetControlEnable(IDC_DESTSOURCE_EDIT_SETDISKPATH, false);
        m_pdlg->SetControlEnable(IDC_DESTSOURCE_BTN_FILEPATH, false);
        m_pdlg->SetControlEnable(IDC_DESTSOURCE_COMBO_TAPEDRIVES, false);
        m_pdlg->SetControlState(IDC_DESTSOURCE_RBTN_ELEC, false);
        m_pdlg->SetControlState(IDC_DESTSOURCE_RBTN_DISK, true);
        m_pdlg->SetControlState(IDC_DESTSOURCE_RBTN_TAPE, false);
        m_pdlg->SetControlEnable(IDC_BTN_NEXT, true);
        break;
    }
}

#define MAX_TAPE_DRIVES 100

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CStep_DestSource::UpdateAvailableTapeDrives
// Purpose:   Enumerates the list of available scsi tape drives and adds them to the combobox.
// Arguments: None
// Return:    None
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CStep_DestSource::UpdateAvailableTapeDrives()
{
    // Remove the previous list of tape drives
    m_pdlg->ClearComboBox(IDC_DESTSOURCE_COMBO_TAPEDRIVES);

    // Enumerate the list of scsi tape drives, adding each of them to the tape drive dropdown box
    for (int i = 0; i < MAX_TAPE_DRIVES; i++)
    {
        char szTapeDrive[20];
        sprintf(szTapeDrive, "\\\\.\\TAPE%d", i);

        // Try to open the 'ith' tape drive for querying.
        HANDLE hDrive = CreateFile(szTapeDrive, 0, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
        if (hDrive == INVALID_HANDLE_VALUE)
        {
            // The tape drive doesn't exist - we've reached the end of the installed drives
            break;
        }

        // The tape drive exists!  Add it to the combo box
        char sz[100];
        sprintf(sz, "Tape Drive %d", i);
        m_pdlg->AddToComboBox(IDC_DESTSOURCE_COMBO_TAPEDRIVES, sz);
    }

    // Did the user specify (on the command-line) a tape drive which isn't
    // present?
    if (g_iDefaultTapeDrive != -1 && g_iDefaultTapeDrive > i - 1)
    {
        MessageBox(m_pdlg->GetHwnd(), "Non-present tape drive specified on "
                                      "command line.", "Invalid tape drive",
                                      MB_ICONEXCLAMATION | MB_OK);
        g_iDefaultTapeDrive = -1;
    }

    if (i == 0)
    {
        // There are no tape drives currently accessible
        m_pdlg->AddToComboBox(IDC_DESTSOURCE_COMBO_TAPEDRIVES, "No Tape Drives Found");
    }

    // Keep track of how many tape drives there are
    m_cTapeDrives = i;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CStep_DestSource::HandleButtonPress
// Purpose:   This function is called when the user presses a button.
// Arguments: nButtonId         -- the resource identifier of the button pressed.
// Return:    None
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CStep_DestSource::HandleButtonPress(int nButtonId)
{
    // pstepNext        -- The next step to perform
    CStep *pstepNext;

    // szSourceFile        -- Path\filename of the source (FLD, Package) file the user chose.
    char szSourceFile[MAX_PATH];

    // szDestPath       -- Pathname of the folder in which generated files should be placed.
    char szDestPath[MAX_PATH];

    switch (nButtonId)
    {
    case IDC_BTN_EXIT:
    case ID_ESCAPE_KEY:
        // User wants to exit the application.  Inform the dialog that there is no 'next' step.
        m_pdlg->SetNextStep(NULL);
        break;

    case IDC_BTN_NEXT:
        // If the source is 'package' then we need to verify it's a valid file
        if (m_source == SOURCE_PACKAGE)
        {
            // Verify the file exists
            m_pdlg->GetControlText(IDC_DESTSOURCE_EDIT_SETFLDPATH2, szSourceFile, MAX_PATH);
            if ((GetFileAttributes(szSourceFile) == 0xffffffff))
            {
                // nonexistant PKG source file specified.  Warn the user and stay in this step
                MessageBox(m_pdlg->GetHwnd(), "Please specify a valid PKG source file",
                           "Nonexistant PKG source file specified", MB_ICONEXCLAMATION | MB_OK | MB_APPLMODAL);
                break;
            }

            // Is the specified file a valid PKG file?
            if (!VerifyPackageFile(szSourceFile))
            {
                // nonexistant PKG source file specified.  Warn the user and stay in this step
                MessageBox(m_pdlg->GetHwnd(), "The specified source file is not a valid PKG file",
                           "Invalid PKG source file specified", MB_ICONEXCLAMATION | MB_OK | MB_APPLMODAL);
                break;
            }
        }
        else
        {
            // Verify that a FLD file, like-named FST file, and gdfs.bin all exist in the specified
            // location
            if (!ValidateFiles())
            {
                // User did not enter a valid path.  Remain in this step.
                m_pdlg->SetCurControl(IDC_DESTSOURCE_EDIT_SETFLDPATH);
                break;
            }

            // Grab the FLD path from the appropriate text control
            m_pdlg->GetControlText(IDC_DESTSOURCE_EDIT_SETFLDPATH, szSourceFile, MAX_PATH);
        }

        // If the media is 'Electronic Submission', then we need to verify its
        // a valid path.
        if (m_media == MEDIA_ELECSUB)
        {
            if (!ValidatePath())
            {
                // User did not enter a valid path (and didn't select create or
                // we couldn't create it).  Break out and continue running in
                // this step.
                m_pdlg->SetCurControl(IDC_DESTSOURCE_EDIT_SETDISKPATH);
                break;
            }

            // Grab the path from the appropriate control
            m_pdlg->GetControlText(IDC_DESTSOURCE_EDIT_SETDISKPATH, szDestPath, MAX_PATH);
        }
        else if (m_media == MEDIA_DISK)
        {
            // This code is *only* for testing purposes, so it can point at an arbitrary
            // fixed path (and not check for creation privileges or sufficent
            // disk space.
            HKEY hkey;
            sprintf(szDestPath, "C:\\test");

            // Allow override via regkey
            if ((RegOpenKeyEx(HKEY_LOCAL_MACHINE,
                              TEXT("SOFTWARE\\Microsoft\\XboxSDK\\xbPremaster"), 0,
                              KEY_READ, &hkey) == ERROR_SUCCESS))
            {
                DWORD dwSize = MAX_PATH;
                RegQueryValueEx(hkey, "RenderDest", NULL, NULL,
                                    (BYTE*)szDestPath, &dwSize);
                RegCloseKey(hkey);
            }

            CreateDirectory(szDestPath, NULL);
        }
        else // MEDIA_TAPE
        {
            // Grab the name of the tape drive from the combo box
            if (g_iDefaultTapeDrive == -1)
            {
                char szDrive[100];
                m_pdlg->GetComboSel(IDC_DESTSOURCE_COMBO_TAPEDRIVES, szDrive);
                sprintf(szDestPath, "\\\\.\\Tape%s", szDrive + 11);
            }
            else
                sprintf(szDestPath, "\\\\.\\Tape%d", g_iDefaultTapeDrive);
        }

        // Move on to the progress step.  szDestPath is ignored if the media is MEDIA_TAPE.
        pstepNext = new CStep_Progress(m_pdlg, m_source, m_media, szDestPath, szSourceFile);
        if (pstepNext == NULL)
            ReportError(ERROR_OUTOFMEM);

        m_pdlg->SetNextStep(pstepNext);
        break;

    case IDC_DESTSOURCE_BTN_FILEPATH:
        // User wants to open the common directory dialog to obtain the path.
        if (m_pdlg->GetFileNameCommon(TEXT("Select Filename"), "*.pkg", "PKG Files\0*.PKG\0", szDestPath, false) == true)
        {
            // user selected a path.  Copy it into our text control
            m_pdlg->SetControlText(IDC_DESTSOURCE_EDIT_SETDISKPATH, szDestPath);
        }
        break;

    case IDC_DESTSOURCE_RBTN_FLD:
        SetSource(SOURCE_FLD);
        break;

    case IDC_DESTSOURCE_RBTN_PACKAGE:
        SetSource(SOURCE_PACKAGE);
        break;

    case IDC_DESTSOURCE_BTN_FLDPATH:
        // User wants to open the common file dialog to obtain the file/pathname.
        if (m_pdlg->GetFileNameCommon(TEXT("Select FLD file"), "*.fld", "FLD Files\0*.FLD\0", szSourceFile, true) == true)
        {
            // user selected a file.  Copy it into our text control
            m_pdlg->SetControlText(IDC_DESTSOURCE_EDIT_SETFLDPATH, szSourceFile);
        }
        break;

    case IDC_DESTSOURCE_BTN_FLDPATH2:
        // User wants to open the common file dialog to obtain the file/pathname for the package
        if (m_pdlg->GetFileNameCommon(TEXT("Select Package file"), "*.pkg", "PKG Files\0*.PKG\0", szSourceFile, true) == true)
        {
            // user selected a file.  Copy it into our text control
            m_pdlg->SetControlText(IDC_DESTSOURCE_EDIT_SETFLDPATH2, szSourceFile);
        }
        break;

    case IDC_BTN_PREV:
        // user wants to go to previous step
        pstepNext = new CStep_UserAction(m_pdlg);
        if (pstepNext == NULL)
            ReportError(ERROR_OUTOFMEM);

        m_pdlg->SetNextStep(pstepNext);
        break;
        
    case IDC_DESTSOURCE_RBTN_TAPE:
        // User has selected the tape as the media.  Update the appropriate controls
        SetMedia(MEDIA_TAPE);
        break;

    case IDC_DESTSOURCE_RBTN_ELEC:
        // User has selected the electronic submission type.  Update the appropriate controls
        SetMedia(MEDIA_ELECSUB);
        break;

    case IDC_DESTSOURCE_RBTN_DISK:
        // User has selected the tape type.  Update the appropriate controls
        SetMedia(MEDIA_DISK);
        break;
    }
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  GetFSTandBINFromFLD
// Purpose:   Given a valid (existing) FLD file/pathname, generates the associated FST file/path
//            and gdfs.bin path strings.
// Arguments: 
// Return:    'true' if the necessary files exist alongside the FLD file.
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool GetFSTandBINFromFLD(CDlg *pdlg, CStep *pstep, char *szFLDFilePathName, char *szFSTName, char *szBINName)
{
    char szProjName[MAX_PATH];
    char *pszSlash;
    DWORD dwStrLen;
    DWORD nRead;
    sFLDHeader fldhdr;
    
    // Open the file for READ-ONLY access.  The file must exist or we fail.
    HANDLE hFile = CreateFile(szFLDFilePathName, GENERIC_READ, 0, NULL,
                              OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    if (hFile == INVALID_HANDLE_VALUE)
        return false;

    // Is it an old or new FLD file format?  We fail on old formats...
    if (!ReadFile(hFile, &fldhdr, sizeof fldhdr, &nRead, NULL) ||
        nRead != sizeof fldhdr)
    {
        CloseHandle(hFile);
        return false;
    }

    if (memcmp(fldhdr.szSig, FLD_SIG, FLD_SIG_SIZE))
    {
        // Not a new version FLD file.
        CloseHandle(hFile);
        return false;
    }

    if (!ReadFile(hFile, &dwStrLen, 4, &nRead, NULL) || nRead != 4)
    {
        CloseHandle(hFile);
        return false;
    }

    if (!ReadFile(hFile, szFSTName, dwStrLen, &nRead, NULL) || dwStrLen != nRead)
    {
        CloseHandle(hFile);
        return false;
    }
    CloseHandle(hFile);


    szFSTName[dwStrLen] = '\0';
   
    pszSlash = strrchr(szFSTName, '/');
    if (!pszSlash)
    {
        // Try forward slash
        pszSlash = strrchr(szFSTName, '\\');
        if (!pszSlash)
            return false;
    }
    strcpy(szProjName, pszSlash);
    strcat(szFSTName, "\\_amc");
    strcpy(szBINName, szFSTName);
    strcat(szFSTName, szProjName);
    strcat(szFSTName, ".fst");
    strcat(szBINName, "\\gdfs.bin");
    return true;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CStep_DestSource::ValidateFiles
// Purpose:   Validates the current path in the FLD text control is a valid path, and that the
//            necessary FLD file exists in the directory.  Also verifies that a like-named FST file
//            and gdfs.bin exist in the same path.  No validation is done on the actual files
//            (they are validated in the next CStep).
// Arguments: None
// Return:    'true' if the path is valid and the required files exist in the path.
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool CStep_DestSource::ValidateFiles()
{
    char szFLDName[MAX_PATH];
    char szFSTName[MAX_PATH];
    char szBINName[MAX_PATH];
    char szTemp[MAX_PATH + 100];
    bool fMissingFiles;
    
    // Get the path string from the FLD text control.
    m_pdlg->GetControlText(IDC_DESTSOURCE_EDIT_SETFLDPATH, szFLDName, MAX_PATH);

    // Verify the user specified a path
    if (!strcmp(szFLDName, ""))
    {
        MessageBox(m_pdlg->GetHwnd(), "Please specify a path for the source files",
                   "Path required", MB_ICONEXCLAMATION | MB_OK | MB_APPLMODAL);
        return false;
    }

    // Verify it's not a local path
    strcpy(szTemp, szFLDName);
    char *pszSlash = strtok(szTemp, "\\/");
    if (!pszSlash)
    {
        sprintf(szTemp, "Please specify the full pathname to the FLD file");
        MessageBox(m_pdlg->GetHwnd(), szTemp, "Local path not allowed", MB_ICONEXCLAMATION |
                   MB_OK | MB_APPLMODAL);
        return false;
    }

    // Verify the name of the file was also included (ie not just the dir)
    pszSlash = szFLDName + strlen(szFLDName) - 4;
    if (lstrcmpi(szFLDName + strlen(szFLDName) - 4, ".fld"))
    {
        sprintf(szTemp, "Please specify the filename of the FLD file, not just the path");
        MessageBox(m_pdlg->GetHwnd(), szTemp, "Filename not specified", MB_ICONEXCLAMATION |
                   MB_OK | MB_APPLMODAL);
        return false;
    }

    // Verify the path exists
    bool fExists = (GetFileAttributes(szFLDName) != 0xffffffff);

    // If the file doesn't exists then return failure
    if (!fExists)
    {
        sprintf(szTemp, "The FLD file '%s' does not exist.  Please specify an existing file.", szFLDName);
        MessageBox(m_pdlg->GetHwnd(), szTemp, "FLD file not found", MB_ICONEXCLAMATION | MB_OK |
                   MB_APPLMODAL);
        return false;
    }

    // If here, then the path exists and there's an FLD file in it.  Check if there's a like-named
    // FST file and 'gdfs.bin' there as well.
    if (!GetFSTandBINFromFLD(m_pdlg, this, szFLDName, szFSTName, szBINName))
        fMissingFiles = true;
    else
    {
        // The filenames were successfully generated - do the files actually exist?
        fMissingFiles = (GetFileAttributes(szFSTName) == 0xffffffff) ||
                        (GetFileAttributes(szBINName) == 0xffffffff);
    }

    if (fMissingFiles)
    {
        pszSlash = strrchr(szFSTName, '/');
        if (!pszSlash)
            pszSlash = strrchr(szFSTName, '\\');
        sprintf(szTemp, "'%s' is out of date.  Please rerun xbLayout and click 'Premaster' to update this file.",
                pszSlash + 1);
        MessageBox(m_pdlg->GetHwnd(), szTemp, "File out of date", MB_ICONEXCLAMATION |
                   MB_OK| MB_APPLMODAL);
        return false;
    }

    // Verify that the FLD and FST files are in sync
    if (!FLDAndFSTInSync(szFLDName, szFSTName))
    {
        MessageBox(m_pdlg->GetHwnd(), "The FLD is newer than the concordant"
                   " FST file.  Please run xbLayout and click 'Premaster' "
                   "to generate an new FST file.", "FST file not updated",
                   MB_ICONEXCLAMATION | MB_OK | MB_APPLMODAL);
        return false;
    }

    return true;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CStep_DestSource::FLDAndFSTInSync
// Purpose:   Return 'true' if the FST and BIN files are in sync.
// Arguments: None
// Return:    'true' if the FST and BIN are in sync
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool CStep_DestSource::FLDAndFSTInSync(char *szFLDName, char *szFSTName)
{
    sFLDHeader fldhdr;
    bool   fInSync = false;
    time_t timeFST;
    DWORD  dwTimeStampLoc, dwRead;
    HANDLE hfileFST = NULL, hfileFLD = NULL;

    // Compare the 'FLD last saved' timestamps stored in the FLD and FST.
    // 1. If FLD is old-version FLD, then force rebuild
    // 2. If timestamps are different, then force rebuild
    // 3. If either file can't be found, then force rebuild
   
    // Load the two files.
    hfileFLD = CreateFile(szFLDName, GENERIC_READ, 0, NULL, OPEN_EXISTING,
                 FILE_ATTRIBUTE_NORMAL, 0);
    hfileFST = CreateFile(szFSTName, GENERIC_READ, 0, NULL, OPEN_EXISTING,
                          FILE_ATTRIBUTE_NORMAL, 0);
    if (hfileFLD == INVALID_HANDLE_VALUE)
        goto done;
    if (hfileFST == INVALID_HANDLE_VALUE)
        goto done;

    // Get the FLD signature
    if ((!ReadFile(hfileFLD, &fldhdr, sizeof fldhdr, &dwRead, 0) ||
        dwRead != sizeof fldhdr))
        goto done;
    
    // Verify it's a newer FLD file; if it's an older one, then force rebuild
    if (memcmp(fldhdr.szSig, FLD_SIG, FLD_SIG_SIZE))
        goto done;
    
    // Get the FST timestamp
    dwTimeStampLoc = sizeof(TFileHeader) + sizeof(TMediaHeader) + 2;
    SetFilePointer(hfileFST, dwTimeStampLoc, NULL, FILE_BEGIN);
    if ((!ReadFile(hfileFST, &timeFST, sizeof timeFST, &dwRead, 0) ||
        dwRead != sizeof timeFST))
        goto done;

    // If times are different, we're not in sync
    fInSync = (fldhdr.timeSaved == timeFST);

done:

    if (hfileFLD)
        CloseHandle(hfileFLD);
    if (hfileFST)
        CloseHandle(hfileFST);
    
    return fInSync;        
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CStep_DestSource::ValidatePath
// Purpose:   Validates the current path in the path text control.  If the path doesn't exist, then
//            queries the user if they would like for it to be created automatically.
// Arguments: None
// Return:    'true' if the path is valid after processing.  If the user selected to not create the
//            path, or an error occurred during path creation, then 'false'.
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool CStep_DestSource::ValidatePath()
{
    char szPath[MAX_PATH], szFilePath[MAX_PATH];
    char szNew[MAX_PATH];
    char szTemp[MAX_PATH + 100];

    // Get the path string from the text control.
    if (m_pdlg->GetControlText(IDC_DESTSOURCE_EDIT_SETDISKPATH, szFilePath, MAX_PATH) != S_OK)
    {
        // An unexpected error has occurred.  Report fatal error and exit.
        ReportError(ERROR_UNEXPECTED);
    }
    
    // Separate the path alone (ie ignore the filename for now) since that's
    // what we want to verify/create if necessary
    strcpy(szPath, szFilePath);
    char *pszSlash = strrchr(szPath, '/');
    if (!pszSlash)
    {
        pszSlash = strrchr(szPath, '\\');
        if (!pszSlash)
        {
            // Totally invalid filename
            MessageBox(m_pdlg->GetHwnd(), "You need to specify a full destination path (including drive letter).  Please check "\
                       "the path and try again.", "Invalid destination path specified",
                       MB_ICONEXCLAMATION | MB_OK | MB_APPLMODAL);
            return false;
        }
    }
    if (pszSlash == szPath + 2)
        *(pszSlash + 1) = '\0';
    else
        *pszSlash = '\0';

    // First, check if the string is a properly formed directory (regardless of if the directory
    // doesn't exist).  A properly formed dir must begin with '*:\"
    if (szPath[1] != ':' || szPath[2] != '\\')
    {
        // User forgot the drive letter (ie "\foo\bar") or something
        // completely random.  Don't allow it.
        MessageBox(m_pdlg->GetHwnd(), "You need to specify a full destination path (including drive letter).  Please check "\
                   "the path and try again.", "Invalid destination path specified",
                   MB_ICONEXCLAMATION | MB_OK | MB_APPLMODAL);
        return false;
    }

    // Did the user specify a PKG file extension?
    if (lstrcmpi(szFilePath + strlen(szFilePath) - 4, ".pkg"))
    {
        MessageBox(m_pdlg->GetHwnd(), "You need to specify a filename ending in '.pkg'.  Please check "\
                   "the path/file and try again.", "Invalid package filename specified",
                   MB_ICONEXCLAMATION | MB_OK | MB_APPLMODAL);
        return false;
    }

    // Does the path exist?
    DWORD dwAttr = GetFileAttributes(szPath);
    bool fExists = (dwAttr != 0xffffffff);

    // If the path exists, then the great - get out of here!
    if (fExists)
        return true;
    
    // If here, then the directory doesn't exist. Query the user if they would
    // like the path to be created.
    sprintf(szTemp, "The directory '%s' does not exist.  Would you like for it to be created?", szPath);
    if (MessageBox(m_pdlg->GetHwnd(), szTemp, "Create Directory?", MB_ICONQUESTION | MB_YESNO | MB_APPLMODAL) == IDNO)
        return false;

    // The user wants the directory to be created.  We may need to create parent directories as
    // well.  Walk up the string, creating the directories as we go.

    // szNew will contain the full path to the current depth, while szCur will contain the name of
    // just the current directory; it will get repeatedly appended onto szNew to iteratively create
    // the entire directory chain.

    char *szCur = strtok(szPath, "\\/");

    // Start out with just the drive name (ie "C:") in szNew.
    strcpy(szNew, szCur);

    // Skip once to get past the drive letter -- we don't need to create "C:\" for instance.
    szCur = strtok(NULL, "\\/");

    // Loop until we've reached the end of the directory string.
    while (szCur)
    {
        // Append the current directory onto the full-path string.
        strcat(szNew, "\\");
        strcat(szNew, szCur);
        
        // Try to create the full path to the current depth.
        if (CreateDirectory(szNew, NULL) == false)
        {
            switch (GetLastError())
            {
            case ERROR_ALREADY_EXISTS:
                // Ignore - just means that the current path already exists.
                break;
            
            case ERROR_ACCESS_DENIED:
                // We can't write to the specified destination (ie user pointed at read-only media)
                // fall through...

            case ERROR_PATH_NOT_FOUND:
                // User specified a drive that isn't mapped
                MessageBox(m_pdlg->GetHwnd(), "The specified directory cannot be created.  Please"\
                           " specify a different path", "Cannot create directory",
                           MB_ICONEXCLAMATION | MB_OK | MB_APPLMODAL);
                return false;
            }
        }
        
        // Get the name of the next-depth directory.
        szCur = strtok(NULL, "\\/");
    }
    
    return true;
}
