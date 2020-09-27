// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:      CStep_Progress.cpp
// Contents:  The 'meat' of xbPremaster.  Displays a constantly updating progress bar as it
//            performs the requested action.
// Revisions: 31-May-2001: Created (jeffsim)
//
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ INCLUDE FILES +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// "stdafx.h"       -- Precompiled header file
#include "stdafx.h"
#include <time.h>


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ FUNCTIONS +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CStep_Progress::CStep_Progress
// Purpose:   CStep_Progress constructor.
// Arguments: pdlg          -- The dialog in which the step will appear.
//            source        -- The source from which the files originate.
//            media         -- The media the action was performed on (tape, disk)
//            szPath        -- The location in which files were created.  Ignored if media == tape.
//            szSourceFile     -- The location of the source file.
// Return:    None
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
CStep_Progress::CStep_Progress(CDlg *pdlg, eSource source, eMedia media, 
                               char szDestPath[MAX_PATH], char szSourceFile[MAX_PATH]) : CStep(pdlg)
{
    // Track the media and action that the user had previously selected.
    m_media  = media;
    m_source = source;

    // Start out in 'noncancelled' state.
    m_fCancelled = false;

    // Track the destination path that the action is being performed in.
    if (m_media == MEDIA_ELECSUB || m_media == MEDIA_DISK)
        strcpy(m_szDestPath, szDestPath);

    // Track the Source filepath and name
    strcpy(m_szSourceFile, szSourceFile);

    // Make all of our controls visible
    m_pdlg->DisplayControl(IDC_PROGRESS_STATIC);
    m_pdlg->DisplayControl(IDC_PROGRESS_BADXBE);
    m_pdlg->SetControlFont(IDC_PROGRESS_BADXBE, -18, "Arial", 0);
    m_pdlg->SetControlText(IDC_PROGRESS_BADXBE, "");

    m_pdlg->DisplayControl(IDC_PROGRESS_TEXT_TITLE);
    m_pdlg->DisplayControl(IDC_PROGRESS_STATIC2);
    m_pdlg->DisplayControl(IDC_PROGRESS_PROGRESSBAR);
    m_pdlg->DisplayControl(IDC_PROGRESS_TEXT_PROGRESS);
    m_pdlg->DisplayControl(IDC_BTN_PREV);
    m_pdlg->DisplayControl(IDC_BTN_NEXT);
    m_pdlg->DisplayControl(IDC_BTN_EXIT);
    m_pdlg->DisplayControl(IDC_PROGRESS_STATIC3);
    m_pdlg->DisplayControl(IDC_PROGRESS_STATIC4);
    m_pdlg->SetControlEnable(IDC_BTN_PREV, false);
    m_pdlg->SetControlEnable(IDC_BTN_NEXT, false);

    // Set the cancel button as the default control
    m_pdlg->SetCurControl(IDC_BTN_EXIT);

    // Overload the background colors of some controls to white
    m_pdlg->OverloadBkColor(IDC_PROGRESS_TEXT_TITLE);

    if (m_media != MEDIA_ELECSUB)
    {
        // Set the default range and position of the progress bar (this will
        // be set elsewhere for the 'elecsub' case)
        m_pdlg->SetProgressRange(IDC_PROGRESS_PROGRESSBAR, 0, NUM_XDATA_SECTORS * 2);
        m_pdlg->SetProgressPos(IDC_PROGRESS_PROGRESSBAR, 0);
    }

    if (m_media == MEDIA_TAPE)
    {
        // Mount the tape drive for writing
mount:
        SetCursor(LoadCursor(NULL, IDC_WAIT));
        if (!m_td.FMount(szDestPath, true))
        {
            // Failed to mount the volume.  Either there is no tape in the drive,
            // or there is a bad tape.  Warn the user and let them insert a new
            // tape.
            SetCursor(LoadCursor(NULL, IDC_ARROW));
            if (MessageBox(m_pdlg->GetHwnd(), "There does not appear to be a " \
                           "valid tape in the drive.\nPlease insert a tape and " \
                           "press 'Ok' to continue.", "Invalid or Missing Tape",
                           MB_ICONQUESTION | MB_OKCANCEL| MB_APPLMODAL) == IDOK)
            {
                // User wants to try again.
                m_td.FUnmount();
                goto mount;
            }
            else
            {
                // User wants to cancel
                m_fCancelled = true;
                return;
            }
        }
        
        /* Make sure the XML file will get padded with whitespace when we
         * write it */
        m_td.SetWritePadByte('\n');
        SetCursor(LoadCursor(NULL, IDC_ARROW));
    }

    // Set the random number generator
    srand(time(NULL));
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CStep_Progress::CheckCancelled
// Purpose:   Handle any pending windows messages.  If the user presses the 'cancel' button, then
//            it will get handled elsewhere; by the time we exit the PeekMessage loop, the
//            'm_fCancelled' variable will be set appropraite.
// Arguments: None
// Return:    'true' if the step should exit; 'false' if the step should continue
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool CStep_Progress::CheckCancelled()
{
    MSG msg;
    
    // Check if there are any windows message waiting that need to be handled
    while (PeekMessage(&msg, NULL, 0,0, PM_REMOVE))
    {
        if (msg.message == WM_QUIT)
            return true;

        if (!IsDialogMessage(m_pdlg->GetHwnd(), &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    // If the user clicked 'cancel', then the HandleButtonPress function already set m_fCancelled
    // appropriately.
    return m_fCancelled;
}

// Recursively delete a directory and all of it's subdirectories
void RecursiveRemoveDirectory(char *szFolder)
{
    if (SetCurrentDirectory(szFolder))
    {
        WIN32_FIND_DATA wfd;
        HANDLE hfile = FindFirstFile("*.*", &wfd);
        while (hfile != INVALID_HANDLE_VALUE)
        {
            char *szFile = wfd.cFileName;
            if (strcmp(szFile, ".") && strcmp(szFile, ".."))
            {
                // It's neither '.' nor '..' - remove it!
                // if it's a file, delete it; if it's a directory, then recurse down into it
                if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                    RecursiveRemoveDirectory(szFile);
                else
                    DeleteFile(szFile);
            }

            if (!FindNextFile(hfile, &wfd))
                break;
        }
        FindClose(hfile);

        // Set our current directory back to the original directory so that we can remove 'this' directory
        SetCurrentDirectory("..");

        // Finally, remove the directory itself
        RemoveDirectory(szFolder);
    }
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CStep_Progress::DoModal
// Purpose:   CSteps that want to perform a block of contiguous functionality should create a modal
//            step that handles message loops (to allow cancellation, etc).  To do this, they
//            override the DoModal function.  In this case, the function calls directly into the
//            appropriate function for handling of data (writing, verifying, etc).
// Arguments: None.
// Return:    'true' to specify that the step has overridden the 'DoModal' function.
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool CStep_Progress::DoModal()
{
    // If we're reading from a package file, then extract the files first
    if (m_source == SOURCE_PACKAGE)
    {
        m_pdlg->SetControlText(IDC_PROGRESS_TEXT_PROGRESS, "Progress (extraction):");
        ExtractPackagedFiles(m_szSourceFile);
        
        // m_szSourceFile now contains the path to the source FLD file.

        if (!m_fCancelled)
        {
            m_pdlg->SetControlText(IDC_PROGRESS_TEXT_PROGRESS, "Progress (media creation):");

            // Get the DVD root from the source FLD file; we'll need it to
            // convert the addresses in the FST file to our temporary directory
            CFLD fld(m_pdlg, this, m_szSourceFile);
            strcpy(m_szPackageDVDRoot, fld.m_szDVDRoot);
        }
    }

    if (!m_fCancelled)
    {
        // After extracting the packaged files, we need to update the progress range
        // for actual creation of the data.
        m_pdlg->SetProgressRange(IDC_PROGRESS_PROGRESSBAR, 0, NUM_XDATA_SECTORS * 2);

        // Do the actual work of writing the files out.  Note: by the time WriteFiles returns, the next
        // step has already been created...
        WriteFiles();
    }
    
    if (m_source == SOURCE_PACKAGE)
    {
        // Only delete the temporary directory if the 'rendertodisk' key is not present
        HKEY hkey;
        if ((RegOpenKeyEx(HKEY_LOCAL_MACHINE,
                          TEXT("SOFTWARE\\Microsoft\\XboxSDK\\xbPremaster"), 0,
                          KEY_READ, &hkey) == ERROR_SUCCESS))
        {
            if (RegQueryValueEx(hkey, "RenderToDisk", NULL, NULL, NULL, NULL)
                            != ERROR_SUCCESS)
            {
                // Delete the temporary directory that we created
                if (m_szPackageExtractFolder[0] != '\0')
                    RecursiveRemoveDirectory(m_szPackageExtractFolder);
            }
            RegCloseKey(hkey);
        }
    }

    // At this point, this step has completed.  We could do cleanup here, but there is none needed.
    return true;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CStep_Progress::HandleButtonPress
// Purpose:   This function is called when the user presses a button.
// Arguments: nButtonId         -- the resource identifier of the button pressed.
// Return:    None
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CStep_Progress::HandleButtonPress(int nButtonId)
{
    switch (nButtonId)
    {
    case IDC_BTN_EXIT:
    case ID_ESCAPE_KEY:
        // User wants to cancel the current operation.  Pop up the verification dialog to ensure
        // that the user actually wants to cancel.
        if (!m_fCancelled)
        {
            if (MessageBox(m_pdlg->GetHwnd(), "Are you sure you want to cancel the operation?",
                           "Cancel operation?", MB_ICONQUESTION | MB_YESNO | MB_APPLMODAL) == IDYES)
            {
                // Yep, user wants to cancel the operation.  Set m_fCancelled = true so that the
                // DoModal function can tell that it should exit.
                m_fCancelled = true;
            }
        }
        break;
    }
}
