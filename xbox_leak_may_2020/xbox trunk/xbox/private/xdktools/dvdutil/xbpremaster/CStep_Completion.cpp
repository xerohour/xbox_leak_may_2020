// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:      CStep_Completion.cpp
// Contents:  Final step of the xbPremaster wizard.  This step displays the results of the action
//            to the user.
// Revisions: 31-May-2001: Created (jeffsim)
//
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ INCLUDE FILES +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// "stdafx.h"       -- Precompiled header file
#include "stdafx.h"
#include <commdlg.h>
extern CCheckSum g_xsum;
extern int g_nMinAppLevel;

extern DWORD g_dwTapeChecksum;
extern DWORD g_dwPackageChecksum;
extern char g_szDefaultOutputFile[MAX_PATH];

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ FUNCTIONS +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void CStep_Completion::SaveLogFile(char *szText, char *szFile)
{
    OPENFILENAME ofn;
    static TCHAR szTitleName[MAX_PATH], szFileName[MAX_PATH];
    static TCHAR szFilter[] = TEXT("Txt Files (*.TXT)\0*.txt\0") \
                              TEXT("All Files (*.*)\0*.*\0\0");
    char *szName;

    // The user could have specified the log filename on the command line
    if (lstrcmpi(g_szDefaultOutputFile, ""))
        szName = g_szDefaultOutputFile;
    else
    {
Save:
        // Initialize the common save-file dialog box structure
        memset(&ofn, 0, sizeof(ofn));
        ofn.lStructSize    = sizeof(OPENFILENAME);
        ofn.lpstrFilter    = szFilter;
        ofn.nMaxFile       = MAX_PATH;
        ofn.nMaxFileTitle  = MAX_PATH;
        ofn.lpstrDefExt    = TEXT("txt");
        ofn.hwndOwner      = m_pdlg->GetHwnd();
        ofn.lpstrFile      = szFileName;
        ofn.lpstrFileTitle = szTitleName;
        ofn.lpstrTitle     = "Save Premaster Log File (required action)";
        ofn.Flags          = OFN_OVERWRITEPROMPT;

        // Pop up the common save-file dialog box.
        if (!GetSaveFileName(&ofn))
        {
            // Nope, you're GOING to save it
            MessageBox(m_pdlg->GetHwnd(), "The xbpremaster log file contains important CRC information that "\
                             "cannot\nbe regenerated.  As a result, it is required that the file "\
                             "be saved.", "File must be saved.", MB_ICONEXCLAMATION | MB_OK);
            goto Save;
        }
        szName = ofn.lpstrFile;
    }

    HANDLE hfile = CreateFile(szName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hfile == INVALID_HANDLE_VALUE)
    {
        // Figure out why we died
        DWORD dwErr = GetLastError();
        switch(dwErr)
        {
        case ERROR_ALREADY_EXISTS:
            // Already prompted (automatically) for this in the GetSaveFileName dialog.  Fall
            // out of error handling
            break;

        case ERROR_DISK_FULL:
            MessageBox(m_pdlg->GetHwnd(), "The specified drive is full.  Please free some space on it or select another drive, and try again.",
                       "Insufficient disk space", MB_ICONWARNING | MB_OK);
            goto Save;

        case ERROR_ACCESS_DENIED:
            MessageBox(m_pdlg->GetHwnd(), "The specified file cannot be overwritten.  Please ensure the file isn't in use and try again.",
                       "Cannot overwrite file", MB_ICONWARNING | MB_OK);
            goto Save;

        default:
            MessageBox(m_pdlg->GetHwnd(), "xbPremaster cannot save the log to the specified location.  Please check the device and try again.",
                       "Cannot save file", MB_ICONWARNING | MB_OK);
            goto Save;
        }
    }

    // Write the log file
    DWORD dwToWrite = strlen(szText), dwWritten;

    if (!WriteFile(hfile, szText, dwToWrite, &dwWritten, NULL) || dwWritten != dwToWrite)
    {
        MessageBox(m_pdlg->GetHwnd(), "Failed to write log file to specified location.  Please try again.",
            "Invalid location", MB_ICONWARNING | MB_OK);
        goto Save;
    }

    // File successfully written
    CloseHandle(hfile);

    strcpy(szFile, szName);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CStep_Completion::CStep_Completion
// Purpose:   CStep_Completion constructor.
// Arguments: pdlg          -- The dialog in which the step will appear.
//            action        -- The action performed (write, verity, etc)
//            media         -- The media the action was performed on (tape, disk)
//            szPath        -- The location in which files were created.  Ignored if media == tape.
//            szFLDFile     -- The location of the source FLD file.  Used if the user goes straight
//                             to 'verify' from here.
//            szResult      -- The result of the action.  Determined by the previous CStep
// Return:    None
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
CStep_Completion::CStep_Completion(CDlg *pdlg, eSource source, eMedia media, 
                                   char szPath[MAX_PATH], char szFLDFile[MAX_PATH],
                                   char szResult[MAX_PATH]) : CStep(pdlg)
{
    bool fSuccessful;
    char szFile[MAX_PATH];

    // Track the action and media that the user selected
    m_source = source;
    m_media  = media;
        
    if (!lstrcmpi(szResult, "The operation completed successfully"))
        fSuccessful = true;
    else
        fSuccessful = false;

    // Set the result text
    char szOutput[4000];
    char szTemp[MAX_PATH];

    // Add the result string
    sprintf(szOutput, "Source FLD File: %s\r\n\r\nResult: %s", szFLDFile, szResult);

    // Add the list of invalid xbes (if any)
    if (g_cInvalidXbes)
    {
        strcat(szOutput, "\r\n\r\nThe following XBE files are linked to " \
                        "unapproved libraries, and may not be approved:");
        for (int i = 0; i < g_cInvalidXbes; i++)
        {
            strcat(szOutput, "\r\n    ");
            strcat(szOutput, g_rgstrInvalidXbeName[i].sz);
        }
    }

    // Add the checksum (if successful)
    if (fSuccessful && (media == MEDIA_TAPE))
    {
        sprintf(szTemp, "\r\n\r\nSubmission CRC: 0x%08X\r\n\r\nMedia CRC: 0x%08X",
                 g_dwTapeChecksum, g_xsum.DwFinalSum());
        strcat(szOutput, szTemp);
        if (source == SOURCE_PACKAGE)
        {
            sprintf(szTemp, "\r\n\r\nPackage CRC: 0x%08X", g_dwPackageChecksum);
            strcat(szOutput, szTemp);
        }
    }
    if (fSuccessful && (media == MEDIA_ELECSUB))
    {
        sprintf(szTemp, "\r\n\r\nSubmission CRC: 0x%08X", g_dwPackageChecksum);
        strcat(szOutput, szTemp);
        sprintf(szTemp, "\r\n\r\nPackage file saved as '%s'.", szPath);
        strcat(szOutput, szTemp);
    }

    if (g_nMinAppLevel == 0)
    {
        // Total failure! We will refuse to accept the app.  Inform the user
        MessageBox(pdlg->GetHwnd(), "Your image contains one or more " \
                   "libraries which are unapproved.\nThe image will not be " \
                   "accepted for certification", "Invalid image", MB_ICONEXCLAMATION | MB_OK);
    }
    else if (g_nMinAppLevel == 1)
    {
        // Potential failure.  We may refuse to accept the app.
        MessageBox(pdlg->GetHwnd(), "Your image contains one or more " \
                   "libraries which may not be approved.\nThe image may or " \
                   "may not be accepted for certification", "Invalid image",
                   MB_ICONEXCLAMATION | MB_OK);
    }

    if (fSuccessful && (media == MEDIA_TAPE))
    {
        // **FORCE** the user to save the log file.
        SaveLogFile(szOutput, szFile);
        sprintf(szTemp, "\r\n\r\nResult log saved to file '%s'.", szFile);
        strcat(szOutput, szTemp);
    }

    if (fSuccessful && (media == MEDIA_ELECSUB))
    {
        // **FORCE** the user to save the log file.
        SaveLogFile(szOutput, szFile);
        sprintf(szTemp, "\r\n\r\nResult log saved to file '%s'.", szFile);
        strcat(szOutput, szTemp);
    }

    // Make all of our controls visible
    m_pdlg->DisplayControl(IDC_COMPLETION_STATIC3);
    m_pdlg->DisplayControl(IDC_COMPLETION_TEXT_TITLE);
    m_pdlg->DisplayControl(IDC_COMPLETION_TEXT_BODY);
    m_pdlg->DisplayControl(IDC_COMPLETION_BTN_FINISH);
    m_pdlg->DisplayControl(IDC_BTN_PREV);
    m_pdlg->DisplayControl(IDC_BTN_EXIT);
    m_pdlg->DisplayControl(IDC_COMPLETION_STATIC);
    m_pdlg->DisplayControl(IDC_COMPLETION_STATIC2);
    m_pdlg->DisplayControl(IDC_COMPLETION_TEXT_TOCLOSE);
    m_pdlg->SetControlEnable(IDC_BTN_PREV, false);
    m_pdlg->SetControlEnable(IDC_BTN_EXIT, false);

    // Overload the background colors of some controls to white
    m_pdlg->OverloadBkColor(IDC_COMPLETION_TEXT_TITLE);
    m_pdlg->OverloadBkColor(IDC_COMPLETION_TEXT_BODY);
    m_pdlg->OverloadBkColor(IDC_COMPLETION_TEXT_TOCLOSE);

    if (m_media == MEDIA_TAPE)
        m_pdlg->DisplayControl(IDC_COMPLETION_BTN_EJECT);

    m_pdlg->SetControlText(IDC_COMPLETION_TEXT_BODY, szOutput);
    m_pdlg->SetControlFont(IDC_COMPLETION_TEXT_TITLE, -18, "Arial", 0);

    // Set the exit button as the default control
    m_pdlg->SetCurControl(IDC_BTN_EXIT);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CStep_Completion::HandleButtonPress
// Purpose:   This function is called when the user presses a button.
// Arguments: nButtonId         -- the resource identifier of the button pressed.
// Return:    None
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CStep_Completion::HandleButtonPress(int nButtonId)
{
    switch (nButtonId)
    {
    case IDC_COMPLETION_BTN_FINISH:
    case ID_ESCAPE_KEY:
        // User wants to exit the application.  Inform the dialog that there is no 'next' step.
        m_pdlg->SetNextStep(NULL);
        break;
    }
}
