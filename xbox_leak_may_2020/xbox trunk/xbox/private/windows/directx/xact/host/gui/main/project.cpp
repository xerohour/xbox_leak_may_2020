/***************************************************************************
 *
 *  Copyright (C) 2/13/2002 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       project.cpp
 *  Content:    Project object (parser, object manager).
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  2/13/2002   dereks  Created.
 *
 ****************************************************************************/

#include "xactapp.h"
#include <commdlg.h>


/****************************************************************************
 *
 *  CProject
 *
 *  Description:
 *      Object constructor.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CProject::CProject"

CProject::CProject
(
    void
)
{
    m_hBuildEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    m_fUpdateAppTitle = TRUE;
}


/****************************************************************************
 *
 *  ~CProject
 *
 *  Description:
 *      Object destructor.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CProject::~CProject"

CProject::~CProject
(
    void
)
{
    //
    // Free owned objects
    //
    
    Reset();

    //
    // Free resources
    //

    CLOSE_HANDLE(m_hBuildEvent);
}


/****************************************************************************
 *
 *  New
 *
 *  Description:
 *      Creates a new project.
 *
 *  Arguments:
 *      BOOL [in]: TRUE to reset the file path.
 *      BOOL [in]: TRUE to prompt the user to save first.
 *
 *  Returns:  
 *      BOOL: TRUE on success.
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CProject::New"

BOOL
CProject::New
(
    BOOL                    fResetFilePath,
    BOOL                    fConfirm
)
{
    BOOL                    fSuccess;

    //
    // Prompt to save
    //

    if(fConfirm)
    {
        if(!CheckDirty())
        {
            return FALSE;
        }
    }

    //
    // Free any current project data
    //
    
    Reset();

    //
    // Reset the file path
    //

    if(fResetFilePath)
    {
        m_szProjectFile[0] = 0;
    }

    //
    // Create child objects
    //

    fSuccess = MAKEBOOL(m_pWaveBankProject = NEW(CGuiWaveBankProject()));

    //
    // Clear the dirty flag
    //

    MakeDirty(FALSE);

    //
    // Disable the "reload" command
    //

    g_pApplication->m_MainFrame.EnableCommand(ID_RELOAD_PROJECT, FALSE);

    return fSuccess;
}


/****************************************************************************
 *
 *  Open
 *
 *  Description:
 *      Opens a new project.
 *
 *  Arguments:
 *      LPCTSTR [in]: project file path, or NULL to prompt the user for one.
 *      BOOL [in]: TRUE to prompt the user to save first.
 *
 *  Returns:  
 *      BOOL: TRUE on success.
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CProject::Open"

BOOL
CProject::Open
(
    LPCTSTR                 pszProjectFile,
    BOOL                    fConfirm
)
{
    static const LPCTSTR    pszFilter       = MAKE_COMMDLG_FILTER(TEXT("Wave Bundler Project"), TEXT("*.") WBWRITER_FILEEXTENSION_PROJECT) \
                                              MAKE_COMMDLG_FILTER(TEXT("All"), TEXT("*.*"));

    static const LPCTSTR    pszDefExt       = WBWRITER_FILEEXTENSION_PROJECT;
    static const DWORD      dwOfnFlags      = OFN_ENABLESIZING | OFN_EXPLORER | OFN_HIDEREADONLY | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_ALLOWMULTISELECT;
    OPENFILENAME            ofn             = { 0 };

    //
    // Don't reopen the same project.  If you want to reload it, call Reopen.
    //

    if(pszProjectFile)
    {
        if(!_tcsicmp(pszProjectFile, m_szProjectFile))
        {
            return TRUE;
        }
    }

    //
    // Prompt the user to save first
    //

    if(fConfirm)
    {
        if(!CheckDirty())
        {
            return FALSE;
        }
    }

    //
    // Get the new project file path
    //

    if(pszProjectFile)
    {
        _tcscpy(m_szProjectFile, pszProjectFile);
    }
    else
    {
        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = g_pApplication->m_MainFrame;
        ofn.lpstrFilter = pszFilter;
        ofn.lpstrFile = m_szProjectFile;
        ofn.nMaxFile = NUMELMS(m_szProjectFile);
        ofn.lpstrTitle = FormatStringResourceStatic(IDS_OPEN_PROJECT);
        ofn.Flags = dwOfnFlags;
        ofn.lpstrDefExt = pszDefExt;

        if(!GetOpenFileName(&ofn))
        {
            return FALSE;
        }
    }

    //
    // Now that the file name's set, just call the Reopen method to
    // do all the work
    //

    return Reopen(FALSE);
}


/****************************************************************************
 *
 *  Reopen
 *
 *  Description:
 *      Reopens the current project.
 *
 *  Arguments:
 *      BOOL [in]: TRUE to prompt the user to save first.
 *
 *  Returns:  
 *      BOOL: TRUE on success.
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CProject::Reopen"

BOOL
CProject::Reopen
(
    BOOL                    fConfirm
)
{
    CWaveBankProjectFile    WaveBankLoader;
    BOOL                    fAppTitle;
    BOOL                    fSuccess;

    //
    // If we don't have a valid project file, bail
    //

    if(!m_szProjectFile[0])
    {
        return FALSE;
    }

    //
    // Get confirmation that they want to reload
    //

    if(fConfirm)
    {
        if(IsDirty())
        {
            if(IDYES != MsgBoxResource(g_pApplication->m_MainFrame, MB_YESNOCANCEL | MB_ICONQUESTION, IDS_CONFIRM_RELOAD))
            {
                return FALSE;
            }
        }
    }

    //
    // Toggle app title updates
    //

    fAppTitle = EnableAppTitleUpdates(FALSE);

    //
    // Reinitialize the project
    //

    if(fSuccess = New(FALSE, FALSE))
    {
        //
        // Change the cursor to provide user feedback
        //

        BeginWaitCursor();

        //
        // Open Wave Bundler stuff
        //

        fSuccess = SUCCEEDED(WaveBankLoader.Load(m_pWaveBankProject, m_szProjectFile));

        //
        // Update the application's MRU
        //

        if(fSuccess)
        {
            g_pApplication->m_MRU.AddItem(m_szProjectFile);
        }

        //
        // Clear the dirty flag.  It gets reset by New, but loading the
        // project could indirectly set it back to dirty.
        //

        MakeDirty(FALSE);

        //
        // Enable the "reload" command
        //

        g_pApplication->m_MainFrame.EnableCommand(ID_RELOAD_PROJECT, TRUE);

        //
        // Restore the cursor
        //

        EndWaitCursor();
    }

    //
    // Update the app title
    //

    EnableAppTitleUpdates(fAppTitle);

    return fSuccess;
}


/****************************************************************************
 *
 *  Save
 *
 *  Description:
 *      Saves the project.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      BOOL: TRUE on success.
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CProject::Save"

BOOL
CProject::Save
(
    void
)
{
    BOOL                    fSuccess        = TRUE;
    CWaveBankProjectFile    WaveBankLoader;

    //
    // If we don't have a valid file path, hand off to the SaveAs method
    //

    if(!m_szProjectFile[0])
    {
        return SaveAs();
    }
    
    //
    // Save Wave Bundler stuff
    //

    fSuccess = SUCCEEDED(WaveBankLoader.Save(m_pWaveBankProject, m_szProjectFile));

    //
    // Update the application's MRU
    //

    if(fSuccess)
    {
        g_pApplication->m_MRU.AddItem(m_szProjectFile);
    }

    //
    // Clear the dirty flag
    //

    if(fSuccess)
    {
        MakeDirty(FALSE);
    }

    return fSuccess;
}


/****************************************************************************
 *
 *  SaveAs
 *
 *  Description:
 *      Saves the project.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      BOOL: TRUE on success.
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "SaveAs::SaveAs"

BOOL
CProject::SaveAs
(
    void
)
{
    static const LPCTSTR    pszFilter   = MAKE_COMMDLG_FILTER(TEXT("Wave Bundler Project"), TEXT("*.") WBWRITER_FILEEXTENSION_PROJECT) \
                                          MAKE_COMMDLG_FILTER(TEXT("All"), TEXT("*.*"));

    static const LPCTSTR    pszDefExt   = WBWRITER_FILEEXTENSION_PROJECT;
    static const DWORD      dwOfnFlags  = OFN_ENABLESIZING | OFN_EXPLORER | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | OFN_ALLOWMULTISELECT;
    OPENFILENAME            ofn         = { 0 };

    //
    // Get the new project file path
    //

    ZeroMemory(&ofn, sizeof(ofn));

    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = g_pApplication->m_MainFrame;
    ofn.lpstrFilter = pszFilter;
    ofn.lpstrFile = m_szProjectFile;
    ofn.nMaxFile = NUMELMS(m_szProjectFile);
    ofn.lpstrTitle = FormatStringResourceStatic(IDS_SAVE_PROJECT);
    ofn.Flags = dwOfnFlags;
    ofn.lpstrDefExt = pszDefExt;

    if(!GetSaveFileName(&ofn))
    {
        return FALSE;
    }

    //
    // Enable the "reload" command
    //

    g_pApplication->m_MainFrame.EnableCommand(ID_RELOAD_PROJECT, TRUE);

    //
    // Save the project
    //
    
    return Save();
}


/****************************************************************************
 *
 *  Reset
 *
 *  Description:
 *      Frees all project data.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CProject::Reset"

void
CProject::Reset
(
    void
)
{
    //
    // Free objects
    //

    DELETE(m_pWaveBankProject);
}


/****************************************************************************
 *
 *  AddWaveBank
 *
 *  Description:
 *      Adds a new bank to the project.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      BOOL: TRUE on success.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CProject::AddWaveBank"

BOOL
CProject::AddWaveBank
(
    void
)
{
    CGuiWaveBank *          pBank                   = NULL;
    TCHAR                   szBankPath[MAX_PATH];
    TCHAR                   szHeaderPath[MAX_PATH];
    TCHAR                   szDrive[MAX_PATH];
    TCHAR                   szDir[MAX_PATH];
    TCHAR                   szFile[MAX_PATH];
    TCHAR                   szExt[MAX_PATH];

    while(TRUE)
    {
        //
        // Get the wave bank file path
        //

        szBankPath[0] = 0;
        
        if(!CGuiWaveBankProject::BrowseBankFile(g_pApplication->m_MainFrame, szBankPath, NUMELMS(szBankPath)))
        {
            return FALSE;
        }
    
        //
        // Get the header file path
        //
    
        _splitpath(szBankPath, szDrive, szDir, szFile, szExt);
        _makepath(szHeaderPath, szDrive, szDir, szFile, TEXT(".") WBWRITER_FILEEXTENSION_HEADER);

        if(!CGuiWaveBankProject::BrowseHeaderFile(g_pApplication->m_MainFrame, szHeaderPath, NUMELMS(szHeaderPath)))
        {
            return FALSE;
        }

        //
        // Create the bank
        //

        if(SUCCEEDED(m_pWaveBankProject->AddBank(szFile, szBankPath, szHeaderPath, (CWaveBank **)&pBank)))
        {
            break;
        }
    }

    //
    // Open the new bank
    //

    pBank->OpenList();

    //
    // Show the bank properties dialog in case the user wants to change 
    // anything else
    //

    pBank->OnCmdProperties();

    //
    // Mark the project as dirty
    //

    MakeDirty();

    return TRUE;
}


/****************************************************************************
 *
 *  CheckDirty
 *
 *  Description:
 *      Prompts the user to save if the project has changed.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      BOOL: TRUE to continue processing; FALSE to cancel.
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CProject::CheckDirty"

BOOL
CProject::CheckDirty
(
    void
)
{
    TCHAR                   szProjectFile[MAX_PATH];
    
    if(!m_fDirty)
    {
        return TRUE;
    }

    if(m_szProjectFile[0])
    {
        _tcsncpy(szProjectFile, m_szProjectFile, NUMELMS(szProjectFile));
    }
    else
    {
        FormatStringResource(szProjectFile, NUMELMS(szProjectFile), IDS_UNNAMED_PROJECT);
    }

    switch(MsgBoxResource(g_pApplication->m_MainFrame, MB_ICONQUESTION | MB_YESNOCANCEL, IDS_SAVE_CHANGES, szProjectFile))
    {
        case IDYES:
            return Save();

        case IDNO:
            return TRUE;
    }

    return FALSE;
}


/****************************************************************************
 *
 *  UpdateAppTitle
 *
 *  Description:
 *      Updates the main frame's application title.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CProject::UpdateAppTitle"

void
CProject::UpdateAppTitle
(
    void
)
{
    TCHAR                   szTitle[MAX_PATH + 0x100];

    if(!m_fUpdateAppTitle)
    {
        return;
    }

    if(m_szProjectFile[0])
    {
        _tsplitpath(m_szProjectFile, NULL, NULL, szTitle, NULL);
    }
    else
    {
        FormatStringResource(szTitle, NUMELMS(szTitle), IDS_UNNAMED_PROJECT);
    }

    if(m_fDirty)
    {
        _tcscat(szTitle, TEXT(" *"));
    }

    _tcscat(szTitle, TEXT(" - "));
    _tcscat(szTitle, g_szAppTitle);

    g_pApplication->m_MainFrame.SetWindowText(szTitle);
}


/****************************************************************************
 *
 *  EnableAppTitleUpdates
 *
 *  Description:
 *      Toggles whether the project updates the application title.
 *
 *  Arguments:
 *      BOOL [in]: TRUE to enable.
 *
 *  Returns:  
 *      BOOL: previous value.
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CProject::EnableAppTitleUpdates"

BOOL
CProject::EnableAppTitleUpdates
(
    BOOL                    fEnable
)
{
    const BOOL              fPrevious   = m_fUpdateAppTitle;

    m_fUpdateAppTitle = fEnable;

    if(fEnable && !fPrevious)
    {
        UpdateAppTitle();
    }

    return fPrevious;
}


/****************************************************************************
 *
 *  Build
 *
 *  Description:
 *      Builds the project.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      BOOL: TRUE on success.
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CProject::Build"

BOOL
CProject::Build
(
    void
)
{
    BOOL                    fSuccess    = TRUE;
    DWORD                   dwThreadId;

    ASSERT(!IS_VALID_HANDLE_VALUE(m_hThread));

    //
    // Reset the build event
    //

    ResetEvent(m_hBuildEvent);

    //
    // Create the build thread
    //

    m_hThread = CreateThread(NULL, 0, BuildThreadProc, this, 0, &dwThreadId);
    fSuccess = IS_VALID_HANDLE_VALUE(m_hThread);

    //
    // Pop the build dialog
    //
    
    if(fSuccess)
    {
        fSuccess = m_BuildDialog.DoModal(&g_pApplication->m_MainFrame);
    }

    //
    // Clean up
    //

    CLOSE_HANDLE(m_hThread);

    return fSuccess;
}


/****************************************************************************
 *
 *  BuildThread
 *
 *  Description:
 *      Build worker thread procedure.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      DWORD: thread exit code.
 *
 ****************************************************************************/
 
#undef DPF_FNAME
#define DPF_FNAME "CProject::BuildThread"

DWORD
CProject::BuildThread
(
    void
)
{
    HRESULT                 hr;
    
    //
    // Wait for the go-ahead
    //

    WaitForSingleObject(m_hBuildEvent, INFINITE);

    //
    // Build wave banks
    //

    hr = m_pWaveBankProject->Generate(&m_BuildDialog.m_WaveBundler, TRUE);

    //
    // Post a message to the build dialog to tell it that we're done
    //

    m_BuildDialog.PostMessage(WM_PROJECT_BUILDDONE, 0, hr);

    return hr;
}


/****************************************************************************
 *
 *  CProjectBuildDialog
 *
 *  Description:
 *      Object constructor.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CProjectBuildDialog::CProjectBuildDialog"

CProjectBuildDialog::CProjectBuildDialog
(
    void
)
:   CDialog(IDD)
{
}


/****************************************************************************
 *
 *  ~CProjectBuildDialog
 *
 *  Description:
 *      Object destructor.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CProjectBuildDialog::~CProjectBuildDialog"

CProjectBuildDialog::~CProjectBuildDialog
(
    void
)
{
}


/****************************************************************************
 *
 *  HandleMessage
 *
 *  Description:
 *      Dispatches a window message to the appropriate handler.
 *
 *  Arguments:
 *      UINT [in]: message identifier.
 *      WPARAM [in]: message parameter 1.
 *      LPARAM [in]: message parameter 2.
 *      LRESULT * [out]: message result.
 *
 *  Returns:  
 *      BOOL: FALSE to allow the message to be passed to the default handler.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CProjectBuildDialog::HandleMessage"

BOOL 
CProjectBuildDialog::HandleMessage
(
    UINT                    uMsg, 
    WPARAM                  wParam, 
    LPARAM                  lParam, 
    LRESULT *               plResult
)
{
    BOOL                    fHandled;
    
    switch(uMsg)
    {
        case WM_PROJECT_BUILDDONE:
            fHandled = OnBuildDone((HRESULT)lParam, plResult);
            break;

        default:
            fHandled = CDialog::HandleMessage(uMsg, wParam, lParam, plResult);
            break;
    }

    return fHandled;
}


/****************************************************************************
 *
 *  OnCreate
 *
 *  Description:
 *      Handles WM_CREATE messages.
 *
 *  Arguments:
 *      LPVOID [in]: creation context.
 *      LRESULT * [out]: message result.
 *
 *  Returns:  
 *      BOOL: FALSE to allow the message to be passed to the default handler.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CProjectBuildDialog::OnCreate"

BOOL
CProjectBuildDialog::OnCreate
(
    LPVOID                  pvContext,
    LRESULT *               plResult
)
{
    HWND                    hWndChildFrame  = GetDlgItem(IDC_PLACEHOLDER);
    UINT                    nProgressSteps;
    RECT                    rcPlaceholder;
    
    if(CDialog::OnCreate(pvContext, plResult))
    {
        return TRUE;
    }

    //
    // Determine the total number of steps in building the project
    //

    nProgressSteps = g_pApplication->m_Project.m_pWaveBankProject->m_dwEntryCount;

    //
    // Initialize controls
    //

    SendDlgItemMessage(IDC_OVERALL_PROGRESS, PBM_SETRANGE32, 0, nProgressSteps);

    //
    // Load child dialogs
    //

    ::GetWindowRect(GetDlgItem(IDC_PLACEHOLDER), &rcPlaceholder);
    ScreenToClient(&rcPlaceholder);

    m_WaveBundler.Create(this);
    m_WaveBundler.MoveWindow(&rcPlaceholder);

    //
    // Start building
    //

    SetEvent(g_pApplication->m_Project.m_hBuildEvent);

    //
    // Return TRUE to set the keyboard focus
    //

    return TRUE;
}


/****************************************************************************
 *
 *  OnBuildDone
 *
 *  Description:
 *      Handles WM_PROJECT_BUILDDONE messages.
 *
 *  Arguments:
 *      HRESULT [in]: creation context.
 *      LRESULT * [out]: message result.
 *
 *  Returns:  
 *      BOOL: FALSE to allow the message to be passed to the default handler.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CProjectBuildDialog::OnBuildDone"

BOOL
CProjectBuildDialog::OnBuildDone
(
    HRESULT                 hr,
    LRESULT *               plResult
)
{
    UINT                    nStringId;
    
    //
    // Update the UI
    //

    ShowWindow(m_WaveBundler, SW_HIDE);

    switch(hr)
    {
        case S_OK:
            nStringId = IDS_BUILD_SUCCEEDED;
            break;

        case E_ABORT:
            nStringId = IDS_BUILD_ABORTED;
            break;

        default:
            nStringId = IDS_BUILD_FAILED;
            break;
    }
    
    SetDlgItemText(IDC_PLACEHOLDER, FormatStringResourceStatic(nStringId));
    SetDlgItemText(IDCANCEL, FormatStringResourceStatic(IDS_CLOSE));

    EnableWindow(GetDlgItem(IDCANCEL), TRUE);

    //
    // Alert the user
    //
    
    MessageBeep(MB_ICONEXCLAMATION);

    *plResult = 0;
    return TRUE;
}


/****************************************************************************
 *
 *  OnCmdCancel
 *
 *  Description:
 *      Handles the Cancel command.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CProjectBuildDialog::OnCmdCancel"

void
CProjectBuildDialog::OnCmdCancel
(
    void
)
{
    DWORD                   dwExitCode  = ~STILL_ACTIVE;
    
    //
    // If the thread is still running, we can't just close the dialog
    //

    GetExitCodeThread(g_pApplication->m_Project.m_hThread, &dwExitCode);

    if(STILL_ACTIVE == dwExitCode)
    {
        g_pApplication->m_Project.CancelBuild();
        
        SetDlgItemText(IDCANCEL, FormatStringResourceStatic(IDS_ABORTING));
        EnableWindow(GetDlgItem(IDCANCEL), FALSE);
    }
    else
    {
        CDialog::OnCmdCancel();
    }
}


/****************************************************************************
 *
 *  CWaveBundlerBuildDialog
 *
 *  Description:
 *      Object constructor.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CWaveBundlerBuildDialog::CWaveBundlerBuildDialog"

CWaveBundlerBuildDialog::CWaveBundlerBuildDialog
(
    void
)
:   CDialog(IDD)
{
}


/****************************************************************************
 *
 *  ~CWaveBundlerBuildDialog
 *
 *  Description:
 *      Object destructor.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CWaveBundlerBuildDialog::~CWaveBundlerBuildDialog"

CWaveBundlerBuildDialog::~CWaveBundlerBuildDialog
(
    void
)
{
}


/****************************************************************************
 *
 *  BeginProject
 *
 *  Description:
 *      Status callback.
 *
 *  Arguments:
 *      CWaveBankProject * [in]: project.
 *
 *  Returns:  
 *      BOOL: TRUE to continue.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CWaveBundlerBuildDialog::BeginProject"

BOOL
CWaveBundlerBuildDialog::BeginProject
(
    CWaveBankProject *      pProject
)
{
    //
    // Make the dialog visible
    //

    ShowWindow(m_hWnd, SW_SHOW);

    //
    // Check for a cancellation
    //

    return !g_pApplication->m_Project.CheckBuildCancellation();
}


/****************************************************************************
 *
 *  EndProject
 *
 *  Description:
 *      Status callback.
 *
 *  Arguments:
 *      CWaveBankProject * [in]: project.
 *      HRESULT [in]: result code.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CWaveBundlerBuildDialog::EndProject"

void
CWaveBundlerBuildDialog::EndProject
(
    CWaveBankProject *      pProject,
    HRESULT                 hr
)
{
    //
    // Hide the dialog
    //

    ShowWindow(m_hWnd, SW_HIDE);
}


/****************************************************************************
 *
 *  OpenBank
 *
 *  Description:
 *      Status callback.
 *
 *  Arguments:
 *      CWaveBank * [in]: bank.
 *
 *  Returns:  
 *      BOOL: TRUE to continue.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CWaveBundlerBuildDialog::OpenBank"

BOOL
CWaveBundlerBuildDialog::OpenBank
(
    CWaveBank *             pBank
)
{
    //
    // Update the UI
    //
    
    SetDlgItemText(IDC_CURRENT_BANK, pBank->m_szBankName);

    //
    // Set up progress indicators
    //

    SendDlgItemMessage(IDC_BANK_PROGRESS, PBM_SETPOS, 0, 0);
    SendDlgItemMessage(IDC_BANK_PROGRESS, PBM_SETRANGE32, 0, pBank->m_dwEntryCount);

    //
    // Check for a cancellation
    //

    return !g_pApplication->m_Project.CheckBuildCancellation();
}


/****************************************************************************
 *
 *  BeginEntry
 *
 *  Description:
 *      Status callback.
 *
 *  Arguments:
 *      CWaveBankEntry * [in]: entry.
 *
 *  Returns:  
 *      BOOL: TRUE to continue.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CWaveBundlerBuildDialog::BeginEntry"

BOOL
CWaveBundlerBuildDialog::BeginEntry
(
    CWaveBankEntry *        pEntry
)
{
    //
    // Update the UI
    //

    SetDlgItemText(IDC_CURRENT_ENTRY, pEntry->m_szEntryName);

#ifdef IDC_ENTRY_PROGRESS

    //
    // Set up progress indicators
    //

    SendDlgItemMessage(IDC_ENTRY_PROGRESS, PBM_SETPOS, 0, 0);
    SendDlgItemMessage(IDC_ENTRY_PROGRESS, PBM_SETRANGE32, 0, pEntry->m_PlayRegion.dwLength);

#endif // IDC_ENTRY_PROGRESS

    //
    // Check for a cancellation
    //

    return !g_pApplication->m_Project.CheckBuildCancellation();
}


/****************************************************************************
 *
 *  ProcessEntry
 *
 *  Description:
 *      Status callback.
 *
 *  Arguments:
 *      CWaveBankEntry * [in]: entry.
 *      DWORD [in]: number of bytes that have been read up to this point.
 *
 *  Returns:  
 *      BOOL: TRUE to continue.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CWaveBundlerBuildDialog::ProcessEntry"

BOOL
CWaveBundlerBuildDialog::ProcessEntry
(
    CWaveBankEntry *        pEntry,
    DWORD                   dwProcessed
)
{

#ifdef IDC_ENTRY_PROGRESS

    //
    // Update the UI
    //

    SendDlgItemMessage(IDC_ENTRY_PROGRESS, PBM_SETPOS, dwProcessed, 0);

#endif // IDC_ENTRY_PROGRESS

    //
    // Check for a cancellation
    //

    return !g_pApplication->m_Project.CheckBuildCancellation();
}


/****************************************************************************
 *
 *  EndEntry
 *
 *  Description:
 *      Status callback.
 *
 *  Arguments:
 *      CWaveBankEntry * [in]: entry.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CWaveBundlerBuildDialog::EndEntry"

void
CWaveBundlerBuildDialog::EndEntry
(
    CWaveBankEntry *        pEntry
)
{
    //
    // Update the UI
    //

    SendDlgItemMessage(IDC_BANK_PROGRESS, PBM_DELTAPOS, 1, 0);
    m_pParent->SendDlgItemMessage(IDC_OVERALL_PROGRESS, PBM_DELTAPOS, 1, 0);
}


