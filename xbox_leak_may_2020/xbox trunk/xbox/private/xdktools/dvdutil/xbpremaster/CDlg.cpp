// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:      CDlg.cpp
// Contents:  Main dialog class.  This object contains the UI handling for the Steps in this app.
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


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ LOCAL VARIABLES +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// gs_pdlgTemp      -- Temporary pointer used to circumvent CreateDialog annoyances (see below).
static CDlg *gs_pdlgTemp = NULL;


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ FUNCTIONS +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  DlgProc
// Purpose:   This is the static callback for the CDlg dialog.  It receives all messages from the
//            OS (ie user input, callbacks, etc).  We simply take the message, determine which
//            dialog box it actually belongs to, and pass it on to the appropriate CDlg object.
// Arguments: hwndDlg           -- The handle to the dialog that the message is intended for.
//            uMsg              -- The message being reported.
//            wparam            -- Parameter 1 (message-specific)
//            lparam            -- Parameter 2 (message-specific)
// Return:    TRUE if the message was handled; FALSE otherwise
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
BOOL CALLBACK DlgProc(HWND hwndDlg, UINT uMsg, WPARAM wparam, LPARAM lparam) 
{
    CDlg *pdlg;

    // When a CDlg calls CreateDialog to create the Win32 dialog, the OS sends a number of messages
    // to DlgProc before the CreateDialog returns (WM_INITDIALOG, WM_WINDOWPOSCHANGING, etc).
    // Because we haven't returned from the CreateDialog call, we can't set which CDlg hwndDlg is
    // actually referring to.  Therefore, we need to use a temporary global variable to point at
    // the correct CDlg.  If that variable is non-NULL, then we know that we haven't returned from
    // the CreateDialog call, and that we should use it instead.  If the variable is NULL, then the
    // CDlg has had the opportunity to cram a pointer to itself into the window.
    if (gs_pdlgTemp != NULL)
    {
        pdlg = gs_pdlgTemp;
    }
    else
    {
        // Determine which CDlg this message should be sent to.  We stored a pointer to the dialog
        // in the hwnd's 'user' area.  Extract it now.
        pdlg = (CDlg*)GetWindowLong(hwndDlg, DWL_USER);
        if (!pdlg)
            return false;
    }    

    // Pass the message on to the appropriate CDlg and let it handle it.
    return pdlg->DlgProc(uMsg, wparam, lparam);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CDlg::OverloadedBkColor
// Purpose:   Determines if the specified control was overridden to display a white background.
// Arguments: hwndControl       -- The control to examine
// Return:    'true' if the control's background was overloaded
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool CDlg::OverloadedBkColor(HWND hwndControl)
{
    // Check if we overrode the specified control's background color
    for (int i = 0; i < m_cOverloadBkColor; i++)
        if (m_rghwndOverload[i] == hwndControl)
            return true;
    return false;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CDlg::OverloadBkColor
// Purpose:   Overloads a control to display it's background color as white instead of the default
//            light grey.
// Arguments: nControl          -- Resource id of the control to overload
// Return:    S_OK if successful, OLE-compatible error code otherwise.
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
HRESULT CDlg::OverloadBkColor(int nControl)
{
    // Get a handle to the control with the specified resource id.
    HWND hwndControl = GetDlgItem(m_hdlg, nControl);
    if (hwndControl == NULL)
        return E_INVALIDARG;

    m_rghwndOverload[m_cOverloadBkColor] = hwndControl;
    m_cOverloadBkColor++;

    return S_OK;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CDlg::DlgProc
// Purpose:   This is the instance-specific dialog message handler.  There are some message that
//            we handle here (in the base class), while others are ignored (and handled by the OS).
// Arguments: uMsg              -- The message being reported.
//            wparam            -- Parameter 1 (message-specific)
//            lparam            -- Parameter 2 (message-specific)
// Return:    TRUE if the message was handled; FALSE otherwise
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
BOOL CDlg::DlgProc(UINT uMsg, WPARAM wparam, LPARAM lparam) 
{
    switch(uMsg)
    {
    case WM_INITDIALOG:
        // Return that we were successfully initialized
        return TRUE;
        
    case WM_COMMAND:
        // We only need to support button presses here -- pass the button press on to the current
        // step and let it handle it.
        m_pstepCur->HandleButtonPress(LOWORD(wparam));
        return TRUE;

    case WM_CTLCOLORSTATIC:
        // If we asked that the specified control hwnd (passed-in in lparam) be filled w/
        // background white, then do so now
        if (OverloadedBkColor((HWND)lparam))
        {
            SetBkColor((HDC)wparam, RGB(255,255,255));
            return (BOOL)GetStockObject(WHITE_BRUSH);
        }
        return FALSE;
    }
    
    // If the message wasn't handled above, then return FALSE to tell the OS to handle it.
    return FALSE;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CDlg::CDlg
// Purpose:   CDlg constructor.  Creates the modeless Win32 dialog box that contains the
//            application's UI.  We use a dialog box (instead of a plain window) to enable
//            easy resources (buttons, text, progress bars, etc).
// Arguments: hinstApp          -- The instance of the application
// Notes:     * This function is not reentrant due to the use of the global gs_pdlgTemp variable.
//              Doesn't affect this app, but future users of the CDlg object should be aware of it.
// Return:    None
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
CDlg::CDlg(HINSTANCE hinstApp)
{
    // Set the global CDlg pointer to point at us so that ::DlgProc can determine which CDlg the
    // current message belongs to.  This use of a global variable is necessary due to the messages
    // that the OS sends to ::DlgProc before we can stuff a pointer to ourselves into the window
    // (see above)
    gs_pdlgTemp = this;

    // Create the modeless (ie non-blocking) Win32 dialog that will contain our UI.  We pass a
    // pointer to the static DlgProc (::DlgProc) because we can't point windows at our instance-
    // specific DlgProc. ::DlgProc will take care of determining which CDlg->DlgProc to forward to.
    m_hdlg = CreateDialog(hinstApp, MAKEINTRESOURCE(IDD_MAIN), NULL, ::DlgProc);
    if (m_hdlg == NULL)
    {
        SetInited(E_FAIL);
        return;
    }

    // Set the dialog's icon.
    HICON hicon = LoadIcon(hinstApp, "IDC_MAIN");
    SetClassLong(m_hdlg, GCL_HICON, (LONG)hicon);

    // Tell the newly created Win32 dialog box that it "belongs" to us.  This is necessary so that 
    // the static DlgProc that Windows calls (::DlgProc above) can determine that it came from us,
    // and call the DlgProc of this instance of CDlg
    SetWindowLong(m_hdlg, DWL_USER, (long)this);

    // Now that our pointer has been stuffed into the window, we can stop using the global var.
    gs_pdlgTemp = NULL;

    // Make the window visible.
    ShowWindow(m_hdlg, SW_SHOW);

    // Track the instance of the application
    m_hinst = hinstApp;

    // No visible controls at the start
    m_cVisibleControls = 0;

    // Start out without any overloaded colors
    m_cOverloadBkColor = 0;

    // Mark that we were successfully initialized
    SetInited(S_OK);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CDlg::~CDlg
// Purpose:   CDlg destructor.  Destroys the Win32 dialog box (if it was successfully created).
// Arguments: None
// Return:    None
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
CDlg::~CDlg()
{
    if (m_hdlg)
        DestroyWindow(m_hdlg);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CDlg::Run
// Purpose:   Runs the current step.  Enters into the Win32 message loop and doesn't exit until the
//            user has specified that he wants to move on to a different step.
// Arguments: pstepCur      -- The step to run.
// Return:    Pointer to the next step to perform
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
CStep *CDlg::Run(CStep *pstepCur)
{
    MSG msg;

    // Track our current step
    m_pstepCur = pstepCur;

    // Track that we don't know the next step yet.
    m_pstepNext = NULL;

    // We haven't finished the current step yet
    m_fStepEnded = false;

    // Give the step itself a chance to handle the processing; if it doesn't, then we'll do it
    // ourselves.  This allows both modal and modeless steps.
    if (!pstepCur->DoModal())
    {
        // The step is a modeless step; handle the message loop here.

        // Enter into the Win32 message loop.  We'll stay there until the Step signals that it has
        // finished processing (by setting the m_fStepEnded var).
        while (GetMessage(&msg, NULL, 0,0) && !m_fStepEnded)
        {
            if (!IsDialogMessage(m_hdlg, &msg))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
    }

    // Return to the caller a pointer to the next step to perform
    return m_pstepNext;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CDlg::SetCaption
// Purpose:   Sets the caption of the dialog box
// Arguments: szCaption     -- Caption of the dialog box.
// Return:    None
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CDlg::SetCaption(char *szCaption)
{
    SetWindowText(m_hdlg, szCaption);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CDlg::SetNextStep
// Purpose:   Sets the next step to be performed.  Setting this forces the current step to be
//            stopped at the next available time (in CDlg::Run or CStep::DoModal).
// Arguments: pstepNext             -- The next step to perform.  'NULL' to exit the app.
// Return:    None
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CDlg::SetNextStep(CStep *pstepNext)
{
    // Track which step should be performed next
    m_pstepNext = pstepNext;

    // Track that the current step has ended
    m_fStepEnded = true;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CDlg::SetControlEnable
// Purpose:   Enables or disables the specified control.
// Arguments: nControl          -- Resource Id of the control to set
//            fEnabled          -- whether or not the control is to be enabled or disabled.
// Return:    S_OK if successful, OLE-compatible error code otherwise.
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
HRESULT CDlg::SetControlEnable(int nControl, bool fEnabled)
{
    // Get a handle to the control with the specified resource id.
    HWND hwndControl = GetDlgItem(m_hdlg, nControl);
    if (hwndControl == NULL)
        return E_INVALIDARG;
    
    // Enable/Disable the specified control
    EnableWindow(hwndControl, fEnabled);

    // Return that everything went fine.
    return S_OK;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CDlg::SetControlState
// Purpose:   Sets the specified controls state.  Should only be applied to buttons, checkboxes, etc.
// Arguments: nControl          -- Resource Id of the control to set
//            fChecked          -- whether or not the control is to be set or cleared.
// Return:    S_OK if successful, OLE-compatible error code otherwise.
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
HRESULT CDlg::SetControlState(int nControl, bool fChecked)
{
    // Set the specified control's state.
    bool fSuccess = CheckDlgButton(m_hdlg, nControl, fChecked) ? true : false;

    return fSuccess ? S_OK : E_FAIL;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CDlg::SetControlText
// Purpose:   Sets the control's text to the specified string.
// Arguments: nControl          -- Resource Id of the control to set
//            szText            -- Buffer to hold the contents of the control's text
// Return:    S_OK if successful, OLE-compatible error code otherwise.
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
HRESULT CDlg::SetControlText(int nControl, char *szText)
{
    // Get a handle to the control with the specified resource id.
    HWND hwndControl = GetDlgItem(m_hdlg, nControl);
    if (hwndControl == NULL)
        return E_INVALIDARG;
    
    // Store the specified text in the control
    bool fSuccess = SetWindowText(hwndControl, szText) ? true : false;;

    return fSuccess ? S_OK : E_FAIL;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CDlg::GetControlText
// Purpose:   Gets the specified control's text
// Arguments: nControl          -- Resource Id of the control to read
//            szText            -- Buffer to hold the contents of the control's text
//            cbyBufLen         -- Maximum length of the buffer.  Only up to this many bytes will
//                                 be written.
// Return:    S_OK if successful, OLE-compatible error code otherwise.
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
HRESULT CDlg::GetControlText(int nControl, char *szText, int cbyBufLen)
{
    // Get a handle to the control with the specified resource id.
    HWND hwndControl = GetDlgItem(m_hdlg, nControl);
    if (hwndControl == NULL)
        return E_INVALIDARG;
    
    // Store the specified text in the control
    GetWindowText(hwndControl, szText, cbyBufLen);

    // Return that everything went fine.
    return S_OK;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CDlg::DisplayControl
// Purpose:   Makes the specified control visible to the user.
// Arguments: nControl          -- The control to make visible
// Return:    S_OK if successful, OLE-compatible error code otherwise.
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
HRESULT CDlg::DisplayControl(int nControl)
{
    // Add the control to the list of visible controls.  This will allow us to hide the control
    // when switching steps.
    m_rgnVisibleControl[m_cVisibleControls++] = nControl;

    // Get a handle to the control with the specified resource id.
    HWND hwndControl = GetDlgItem(m_hdlg, nControl);
    if (hwndControl == NULL)
        return E_INVALIDARG;

    // Make the control visible
    ShowWindow(hwndControl, SW_SHOW);

    return S_OK;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CDlg::HideAllControls
// Purpose:   Makes all of the controls in the dialog invisible.  This is used as an initialization
//            step for CStep objects to insure that only their controls are visible.
// Arguments: None
// Return:    S_OK if successful, OLE-compatible error code otherwise.
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
HRESULT CDlg::HideAllControls()
{
    // Make all of the currently visible controls hidden
    for (int i = 0; i < m_cVisibleControls; i++)
        ShowWindow(GetDlgItem(m_hdlg, m_rgnVisibleControl[i]), SW_HIDE);

    m_cVisibleControls = 0;
    m_cOverloadBkColor = 0;

    return S_OK;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CDlg::SetCurControl
// Purpose:   Gives keyboard focus to the control with the specified resource id.
// Arguments: nControl          -- The control to make visible
// Return:    S_OK if successful, OLE-compatible error code otherwise.
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
HRESULT CDlg::SetCurControl(int nControl)
{
    // Get a handle to the control with the specified resource id.
    HWND hwndControl = GetDlgItem(m_hdlg, nControl);
    if (hwndControl == NULL)
        return E_INVALIDARG;
    
    // Set the current input focus to the specified control
    SetFocus(hwndControl);

    // Return that everything went fine.
    return S_OK;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CDlg::SetProgressPos
// Purpose:   Sets the position of the specified progress bar
// Arguments: nControl          -- The progress bar to modify
// Return:    S_OK if successful, OLE-compatible error code otherwise.
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
HRESULT CDlg::SetProgressPos(int nControl, int nPos)
{
    // Get a handle to the control with the specified resource id.
    HWND hwndControl = GetDlgItem(m_hdlg, nControl);
    if (hwndControl == NULL)
        return E_INVALIDARG;
    
    // Set the progress pos
    SendMessage(hwndControl, PBM_SETPOS, nPos, 0);

    // Return that everything went fine.
    return S_OK;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CDlg::SetProgressRange
// Purpose:   Sets the range of the specified progress bar
// Arguments: nControl          -- The progress bar to modify
// Return:    S_OK if successful, OLE-compatible error code otherwise.
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
HRESULT CDlg::SetProgressRange(int nControl, int nStart, int nEnd)
{
    // Get a handle to the control with the specified resource id.
    HWND hwndControl = GetDlgItem(m_hdlg, nControl);
    if (hwndControl == NULL)
        return E_INVALIDARG;
    
    // Set the progress range
    SendMessage(hwndControl, PBM_SETRANGE32, nStart, nEnd);
    SendMessage(hwndControl, PBM_SETRANGE32, nStart, nEnd);

    // Return that everything went fine.
    return S_OK;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CDlg::AddToComboBox
// Purpose:   Add the specified string to the specified control.  Control must be a combo box.
// Arguments: nControl          -- The combo box to add a string to
//            szString          -- The string to add
// Return:    S_OK if successful, OLE-compatible error code otherwise.
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
HRESULT CDlg::AddToComboBox(int nControl, char *szString)
{
    // Get a handle to the control with the specified resource id.
    HWND hwndControl = GetDlgItem(m_hdlg, nControl);
    if (hwndControl == NULL)
        return E_INVALIDARG;

    SendMessage(hwndControl, CB_ADDSTRING, 0, (LPARAM)szString);
    SendMessage(hwndControl, CB_SETCURSEL, 0, 0);

    // size the combobox's dropdown
    RECT rc;
    GetWindowRect(hwndControl, &rc);
    POINT pt = {rc.left, rc.top};
    ScreenToClient(m_hdlg, &pt);
    MoveWindow(hwndControl, pt.x, pt.y, (rc.right - rc.left), 100, TRUE);
    return S_OK;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CDlg::ClearComboBox
// Purpose:   Clears the contents of the specified combo box
// Arguments: nControl          -- The combo box to clear
// Return:    S_OK if successful, OLE-compatible error code otherwise.
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
HRESULT CDlg::ClearComboBox(int nControl)
{
    // Get a handle to the control with the specified resource id.
    HWND hwndControl = GetDlgItem(m_hdlg, nControl);
    if (hwndControl == NULL)
        return E_INVALIDARG;

    SendMessage(hwndControl, CB_RESETCONTENT, 0, 0);

    return S_OK;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CDlg::GetComboSel
// Purpose:   Get the text of the currently selected item in the combo box
// Arguments: nControl          -- The combo box to get the currently selection of.
//            szBuffer          -- Buffer to hold the text contents of the combo box's current
//                                 selection
// Return:    S_OK if successful, OLE-compatible error code otherwise.
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
HRESULT CDlg::GetComboSel(int nControl, char *szBuffer)
{
    // Get a handle to the control with the specified resource id.
    HWND hwndControl = GetDlgItem(m_hdlg, nControl);
    if (hwndControl == NULL)
        return E_INVALIDARG;

    // Get the currently selected item in the combo box
    int iCurSel = SendMessage(hwndControl, CB_GETCURSEL, 0, 0);

    // Get the text of the currently selected item in the combo box
    SendMessage(hwndControl, CB_GETLBTEXT, iCurSel, (LPARAM)szBuffer);

    return S_OK;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CDlg::GetPathNameCommon
// Purpose:   Opens the common directory-selection dialog and allows the user to select a path.
// Arguments: szTitle           -- Title to display at the top of the dialog
//            szPath            -- The buffer to hold the new path (if one is selected).
// Return:    'true' if user selected a path; 'false' if they cancelled out
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool CDlg::GetPathNameCommon(char *szTitle, char *szPath)
{
    char       szDisplayName[MAX_PATH];
    BROWSEINFO bi;
    bool       fRet = false;
    IMalloc    *pmalloc;
        
    // Validate parameters
    if (szPath == NULL)
        return false;
    
    // Get a pointer to the shell's allocator.  We'll need it to free up the pidl that's created.
    SHGetMalloc(&pmalloc);

    // Fill in the browse info structure.
    bi.pszDisplayName = szDisplayName;          // Buffer to hold display name
    bi.hwndOwner      = m_hdlg;                 // Owner window for the dialog
    bi.pidlRoot       = NULL;                   // Specify NULL for 'desktop'
    bi.lpszTitle      = szTitle;                // String to display at the top of the dialog
    bi.ulFlags        = BIF_RETURNONLYFSDIRS;   // Don't display control panel, etc
    bi.lpfn           = NULL;                   // No need for the callback
    bi.lParam         = 0;                      // unused.

    // Open up the dialog and have the user select the path.  The function is modal.
    LPITEMIDLIST pidl = SHBrowseForFolder(&bi);
    if (pidl == NULL)
    {
        // User cancelled out, so don't set anything.
        goto done;
    }
    
    // If here, then the user selected a path.  Extract it from the pidl and store it in the
    // buffer the caller specified.
    if (::SHGetPathFromIDList(pidl, szPath) == false)
    {
        // Malformed pidl, or other failure.  In any case, don't set the path
        goto done;
    }

    // At this point, szPath contains the path the user chose.
    if (szPath[0] == '\0')
    {
        // SHGetPathFromIDList failed, or SHBrowseForFolder failed.
        goto done;
    }

    // if here, then we succeeded!
    fRet = true;

done:
    if (pidl)
        pmalloc->Free(pidl);
    pmalloc->Release();      

    return fRet;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CDlg::GetFileNameCommon
// Purpose:   Opens the common file-selection dialog and allows the user to select a file.
// Arguments: szTitle           -- Title to display at the top of the dialog
//            szExt             -- The default extension
//            szFilter          -- The filter string to display in the dialog
//            szFile            -- The buffer to hold the new filename (if one is selected).
//            fOpen             -- 'true' if the file is being opened, 'false' if its being saved
// Return:    'true' if user selected a file; 'false' if they cancelled out
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool CDlg::GetFileNameCommon(char *szTitle, char *szExt, char *szFilter, char *szFile, bool fOpen)
{
    OPENFILENAME ofn;

    // Validate parameters
    if (szTitle == NULL || szFile == NULL)
        return false;

    // Initialize the default filename to empty
    strcpy(szFile, "");

    // Initialize the openfilename structure so that the GetOpenFileName function knows what
    // kind of files we want to allow.
    memset(&ofn, 0, sizeof(ofn));
    ofn.lStructSize         = sizeof(ofn);
    ofn.hwndOwner           = m_hdlg;
    ofn.hInstance           = m_hinst;
    ofn.lpstrCustomFilter   = NULL;
    ofn.nMaxCustFilter      = 0;
    ofn.nFilterIndex        = 1;
    ofn.lpstrFile           = szFile;
    ofn.nMaxFile            = MAX_PATH - 1;
    ofn.lpstrFileTitle      = ofn.lpstrFile;
    ofn.nMaxFileTitle       = ofn.nMaxFile;
    ofn.lpstrInitialDir     = "C:\\";
    ofn.nFileOffset         = 0;
    ofn.nFileExtension      = 0;
    ofn.lpstrDefExt         = szExt;
    ofn.lpstrFilter         = szFilter;
    ofn.lCustData           = 0;
    ofn.lpstrTitle          = szTitle;
    if (fOpen)
    {
        ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;

        // Pop up the common file open dialog
        if (!GetOpenFileName(&ofn))
        {
            // User clicked cancel or closed the dialog (or an error occurred).
            return false;
        }
    }
    else
    {
        ofn.Flags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;

        if (!GetSaveFileName(&ofn))
        {
            // User clicked cancel or closed the dialog (or an error occurred).
            return false;
        }
    }

    // If here, then the file was stored.
    return true;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CDlg::SetControlFont
// Purpose:   Sets the font of the control
// Arguments: nControl              -- The control whose font we'll set
//            nHeight               -- Height of the font
//            szFace                -- Face of the font ("Arial", "Courier", etc)
//            nWeight               -- Normal (0) or Bold (FW_BOLD)
// Return:    S_OK if successful, OLE-compatible error code otherwise.
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
HRESULT CDlg::SetControlFont(int nControl, int nHeight, char *szFace, int nWeight)
{
    LOGFONT lf;

    // Get a handle to the control with the specified resource id.
    HWND hwndControl = GetDlgItem(m_hdlg, nControl);
    if (hwndControl == NULL)
        return E_INVALIDARG;

    // Create the GDI font object
    memset(&lf, 0, sizeof(lf));
    lf.lfHeight = nHeight;
    lf.lfWeight = nWeight;
    strcpy(lf.lfFaceName, szFace);
    HFONT hfont = CreateFontIndirect(&lf);
    
    // Set the control's current font to the newly created font
    SendMessage(hwndControl, WM_SETFONT, (WPARAM)hfont, TRUE);
    
    return S_OK;
}
