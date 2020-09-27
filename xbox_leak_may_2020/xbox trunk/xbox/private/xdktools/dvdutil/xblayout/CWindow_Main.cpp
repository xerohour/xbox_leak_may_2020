// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:      CWindow_Main.cpp
// Contents:  
// Revisions: 13-Jun-2001: Created (jeffsim)
//
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ INCLUDE FILES +++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// "stdafx.h"       -- Precompiled header file
#include "stdafx.h"

// <xboxverp.h>     -- Contains Xbox build-environment versioning information
#include <xboxverp.h>
#include <direct.h>

extern DWORD BlockSize(ULONGLONG ullSize);


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ DEFINES  ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// COLBAR_WIDTH -- Width of the left and right ColorBar objects
#define COLBAR_WIDTH 15


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ GLOBAL VARIABLES ++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// AboutDlgProc -- Dialog procedure for the about box.
extern LRESULT CALLBACK AboutDlgProc(HWND hDlg, UINT message, WPARAM wParam,
                                     LPARAM lParam);

HINSTANCE g_hinst;


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ FUNCTIONS++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CWindow_Main::CWindow_Main
// Purpose:   CWindow_Main constructor.  Doesn't do anything except call the
//            base CWindow constructor.
// Arguments: dwWindowCreationFlags -- window creation flags
//            hinst                 -- Handle to the application instance.
// Return:    None
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
CWindow_Main::CWindow_Main(DWORD dwWindowCreationFlags, HINSTANCE hinst) :
                            CWindow("Main", dwWindowCreationFlags, hinst)
{
    g_hinst = hinst;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CWindow_Main::Init
// Purpose:   Initializes the window object.
// Arguments: None
// Return:    'true' if successful, 'false' otherwise
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool CWindow_Main::Init()
{
    srand(time(NULL));

    // Set our menu name (necessary for base class initialization)
    strcpy(m_szMenuName, "XBLAYOUT");

    // Track which directory we ran from (necessary for help functionality);
    _getcwd(m_szWorkingDir, MAX_PATH);

    // Do base class initialization
    if (!CWindow::Init(NULL))
        return false;

    m_hwndTip = CreateWindowEx(NULL, TOOLTIPS_CLASS, NULL,
                            WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP,
                            CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                            CW_USEDEFAULT, m_hwnd, NULL, g_hinst, NULL);


    // The window has been created.  Add the various UI elements
    m_rgplv[LV_LAYER0]= new CListView();
    if (!m_rgplv[LV_LAYER0])
        FatalError(E_OUTOFMEMORY);
    if (!m_rgplv[LV_LAYER0]->Init(this, false, true))
        return false;

    m_rgplv[LV_LAYER1] = new CListView();
    if (!m_rgplv[LV_LAYER1])
        FatalError(E_OUTOFMEMORY);
    if (!m_rgplv[LV_LAYER1]->Init(this, true, true))
        return false;

    m_pstatusbar = new CStatusBar();
    if (!m_pstatusbar)
        FatalError(E_OUTOFMEMORY);
    if (!m_pstatusbar->Init(this))
        return false;
    
    m_ptoolbar   = new CToolBar();
    if (!m_ptoolbar)
        FatalError(E_OUTOFMEMORY);
    if (!m_ptoolbar->Init(this))
        return false;

    m_pscratch   = new CWindow_Scratch(FLAG_WINDOW_POS_LASTKNOWN, this);
    if (!m_pscratch)
        FatalError(E_OUTOFMEMORY);
    if (!m_pscratch->Init())
        return false;

    m_pcolbarLeft = new CColorBar();
    if (!m_pcolbarLeft)
        FatalError(E_OUTOFMEMORY);
    if (!m_pcolbarLeft->Init(this, RGB(255, 0, 0), RGB(0, 255, 0)))
        return false;

    m_pcolbarRight = new CColorBar();
    if (!m_pcolbarRight)
        FatalError(E_OUTOFMEMORY);
    if (!m_pcolbarRight->Init(this, RGB(0, 255, 0), RGB(255, 0, 0)))
        return false;

    m_pcolbarMid = new CColorBar();
    if (!m_pcolbarMid)
        FatalError(E_OUTOFMEMORY);
    if (!m_pcolbarMid->Init(this, RGB(212, 208, 200), RGB(212, 208, 200)))
        return false;

    // Keep a local pointer to the scratch window's listview control so that we
    // have a contiguous array of listviews for easy reference
    m_rgplv[LV_SCRATCH] = m_pscratch->m_plistview;

    m_rgplv[LV_LAYER0]->m_iLayer  = LV_LAYER0;
    m_rgplv[LV_LAYER1]->m_iLayer  = LV_LAYER1;
    m_rgplv[LV_SCRATCH]->m_iLayer = LV_SCRATCH;

    // At the start, grouping and ungrouping aren't possible (since nothing's
    // selected).
    SetGroupable(false);
    SetUngroupable(false);


    // Create our workspace.  It will automatically populate our controls
    m_pws = new CWorkspace(this);
    if (!m_pws)
        FatalError(E_OUTOFMEMORY);

    if (!m_pws->Init())
        return false;

	// Start out looking at the layer 0 listview
	SetCurLayer(0);

    // Set the caption of the whole window.
    char szCaption[1024];
    LoadString(m_hinst, IDS_WINDOW_CAPTION, szCaption, 1024);
    SetCaption(szCaption);


    // Set the window's initial size and positioned if it was previously known
    if (m_dwWindowCreationFlags & FLAG_WINDOW_POS_LASTKNOWN)
        SetLastKnownPos();
    
    // Finally make the window visible
    ShowWindow(m_hwnd, SW_SHOW);

    // Until something is loaded, we can't do certain tasks
    HMENU hmenu = GetMenu(m_hwnd);
    m_ptoolbar->EnableButton(ID_TOOLS_PREMASTERTAPE,     false);
    m_ptoolbar->EnableButton(ID_TOOLS_EMULATE,           false);
    m_ptoolbar->EnableButton(ID_TOOLS_BUILDFOREMULATION, false);
    m_ptoolbar->EnableButton(ID_FILE_SAVE, false);
    m_ptoolbar->EnableButton(ID_FILE_SAVEAS, false);
    m_ptoolbar->EnableButton(ID_VIEW_RESCANLAYOUT, false);
    EnableMenuItem(hmenu, ID_TOOLS_PREMASTERTAPE,     MF_GRAYED);
    EnableMenuItem(hmenu, ID_TOOLS_EMULATE,           MF_GRAYED);
    EnableMenuItem(hmenu, ID_TOOLS_BUILDFOREMULATION, MF_GRAYED);
    EnableMenuItem(hmenu, ID_FILE_SAVE,     MF_GRAYED);
    EnableMenuItem(hmenu, ID_FILE_SAVEAS,     MF_GRAYED);
    EnableMenuItem(hmenu, ID_VIEW_RESCANLAYOUT, MF_GRAYED);
    CheckMenuItem(hmenu, ID_VIEW_SCRATCHAREA, MF_UNCHECKED);

#ifdef _DEBUG
    // Force loading a particular root loc for debugging purposes UNDONE: Remove
    m_pws->SetDVDRoot("C:\\DemoUS", true);
    //m_pws->SetDVDRoot("C:\\d", true);
//    ListView_SetItemState(m_rgplv[LV_LAYER0]->GetHwnd(), 20, LVNI_SELECTED, LVNI_SELECTED);
#endif

    return true;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CWindow_Main::Uninit
// Purpose:   CWindow_Main uninitialization function.  Frees up all used memory
//            and prepares the CWindow_Main object for deallocation.
// Arguments: None
// Return:    None
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CWindow_Main::Uninit()
{
    // Delete all allocated objects
    if (m_rgplv[LV_LAYER0]) delete m_rgplv[LV_LAYER0];
    if (m_rgplv[LV_LAYER1]) delete m_rgplv[LV_LAYER1];
    if (m_pstatusbar)    delete m_pstatusbar;
    if (m_ptoolbar)      delete m_ptoolbar;
    if (m_pcolbarLeft)   delete m_pcolbarLeft;
    if (m_pcolbarRight)  delete m_pcolbarRight;
    if (m_pcolbarMid)    delete m_pcolbarMid;
    if (m_pws)           delete m_pws;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CWindow_Main::DerivedWndProc
// Purpose:   Handle messages that are specific to this derived window.  Most
//            common messages are already automatically sent to the appropriate
//            "OnFOO" functions.
// Arguments: uMsg              -- The message being reported.
//            wparam            -- Parameter 1 (message-specific)
//            lparam            -- Parameter 2 (message-specific)
// Return:    Result of the message processing - depends on the message sent.
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
BOOL CWindow_Main::DerivedWndProc(UINT uMsg, WPARAM wparam, LPARAM lparam) 
{
    switch (uMsg) 
    {
    case WM_GETMINMAXINFO:
        // The window is being resized. Don't let its size drop below threshold
        ((MINMAXINFO*)lparam)->ptMinTrackSize.x = 240;
        ((MINMAXINFO*)lparam)->ptMinTrackSize.y = 95;
        return 0;

    case WM_DRAWITEM:
        // An item in one of the owner-draw listviews needs to be updated.
        return m_pws->HandleDrawItem(wparam, lparam);
    
    case WM_CLOSE:
        // User wants to close the application.  Verify they don't want to lose
        // modified data.
        if (!m_pws->Close())
        {
            // Return that we didn't handle the message, and that closing should
            // not actually happen.
            return 1;
        }

        // Store the last position of the window
        StorePosition();
        m_pscratch->StorePosition();

        // Prepare the window for destructions
        Uninit();

        // Tell the OS to destroy this window
        PostQuitMessage(0);

        // Return that we handled the message
        return 0;
    }

    // Let windows handle the rest of the messages
    return DefWindowProc(m_hwnd, uMsg, wparam, lparam);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CWindow_Main::ButtonPressed
// Purpose:   This function is called when a button is pressed or a menu item
//            is selected.  Pass the message on to the appropriate function(s).
// Arguments: idButton      -- The resource identifier of the button pressed.
// Return:    'true' if button was handled, 'false' otherwise
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
BOOL CWindow_Main::ButtonPressed(int idButton)
{
	PROCESS_INFORMATION pInfo;
	STARTUPINFO         sInfo;
    char sz[MAX_PATH + 30], sz2[MAX_PATH + 30];
    HMENU hmenu = GetMenu(m_hwnd);
    bool fRet;
    
    switch(idButton)
    {
    case ID_FILE_NEW:
        // Try to close the current workspace -- user given option of not
        // closing if modified.
        SendMessage(m_rgplv[LV_LAYER0]->GetHwnd(),   WM_SETREDRAW, 0, 0);
        SendMessage(m_rgplv[LV_LAYER1]->GetHwnd(),   WM_SETREDRAW, 0, 0);
        SendMessage(m_rgplv[LV_SCRATCH]->GetHwnd(),  WM_SETREDRAW, 0, 0);
        if (m_pws->Close())
            m_pws->New();
        SendMessage(m_rgplv[LV_LAYER0]->GetHwnd(),   WM_SETREDRAW, 1, 0);
        SendMessage(m_rgplv[LV_LAYER1]->GetHwnd(),   WM_SETREDRAW, 1, 0);
        SendMessage(m_rgplv[LV_SCRATCH]->GetHwnd(),  WM_SETREDRAW, 1, 0);
        m_ptoolbar->EnableButton(ID_TOOLS_PREMASTERTAPE,     true);
        m_ptoolbar->EnableButton(ID_TOOLS_EMULATE,           true);
        m_ptoolbar->EnableButton(ID_TOOLS_BUILDFOREMULATION, true);
        m_ptoolbar->EnableButton(ID_FILE_SAVE, true);
        m_ptoolbar->EnableButton(ID_FILE_SAVEAS, true);
        m_ptoolbar->EnableButton(ID_VIEW_RESCANLAYOUT, true);
        EnableMenuItem(hmenu, ID_TOOLS_PREMASTERTAPE,     MF_ENABLED);
        EnableMenuItem(hmenu, ID_TOOLS_EMULATE,           MF_ENABLED);
        EnableMenuItem(hmenu, ID_TOOLS_BUILDFOREMULATION, MF_ENABLED);
        EnableMenuItem(hmenu, ID_FILE_SAVE,     MF_ENABLED);
        EnableMenuItem(hmenu, ID_FILE_SAVEAS,     MF_ENABLED);
        EnableMenuItem(hmenu, ID_VIEW_RESCANLAYOUT,     MF_ENABLED);
        return TRUE;

    case ID_FILE_OPEN:
        // Try to close the current workspace -- user given option of not
        // closing if modified.
        SendMessage(m_rgplv[LV_LAYER0]->GetHwnd(),   WM_SETREDRAW, 0, 0);
        SendMessage(m_rgplv[LV_LAYER1]->GetHwnd(),   WM_SETREDRAW, 0, 0);
        SendMessage(m_rgplv[LV_SCRATCH]->GetHwnd(),  WM_SETREDRAW, 0, 0);
        if (m_pws->Close())
        {
            fRet = m_pws->Open();
            if (!fRet)
            {
                // Failed to open
                m_pws->Reset();
            }
        }
        else
            fRet = true;
        SendMessage(m_rgplv[LV_LAYER0]->GetHwnd(),   WM_SETREDRAW, 1, 0);
        SendMessage(m_rgplv[LV_LAYER1]->GetHwnd(),   WM_SETREDRAW, 1, 0);
        SendMessage(m_rgplv[LV_SCRATCH]->GetHwnd(),  WM_SETREDRAW, 1, 0);
        if (fRet)
        {
            m_ptoolbar->EnableButton(ID_TOOLS_PREMASTERTAPE,     true);
            m_ptoolbar->EnableButton(ID_TOOLS_EMULATE,           true);
            m_ptoolbar->EnableButton(ID_TOOLS_BUILDFOREMULATION, true);
            m_ptoolbar->EnableButton(ID_FILE_SAVE, true);
            m_ptoolbar->EnableButton(ID_FILE_SAVEAS, true);
            m_ptoolbar->EnableButton(ID_VIEW_RESCANLAYOUT, true);
            EnableMenuItem(hmenu, ID_TOOLS_PREMASTERTAPE,     MF_ENABLED);
            EnableMenuItem(hmenu, ID_TOOLS_EMULATE,           MF_ENABLED);
            EnableMenuItem(hmenu, ID_TOOLS_BUILDFOREMULATION, MF_ENABLED);
            EnableMenuItem(hmenu, ID_FILE_SAVE,     MF_ENABLED);
            EnableMenuItem(hmenu, ID_FILE_SAVEAS,     MF_ENABLED);
            EnableMenuItem(hmenu, ID_VIEW_RESCANLAYOUT,     MF_ENABLED);
        }
        else
        {
            m_ptoolbar->EnableButton(ID_TOOLS_PREMASTERTAPE,     false);
            m_ptoolbar->EnableButton(ID_TOOLS_EMULATE,           false);
            m_ptoolbar->EnableButton(ID_TOOLS_BUILDFOREMULATION, false);
            m_ptoolbar->EnableButton(ID_FILE_SAVE, false);
            m_ptoolbar->EnableButton(ID_FILE_SAVEAS, false);
            m_ptoolbar->EnableButton(ID_VIEW_RESCANLAYOUT, false);
            EnableMenuItem(hmenu, ID_TOOLS_PREMASTERTAPE,     MF_GRAYED);
            EnableMenuItem(hmenu, ID_TOOLS_EMULATE,           MF_GRAYED);
            EnableMenuItem(hmenu, ID_TOOLS_BUILDFOREMULATION, MF_GRAYED);
            EnableMenuItem(hmenu, ID_FILE_SAVE,     MF_GRAYED);
            EnableMenuItem(hmenu, ID_FILE_SAVEAS,     MF_GRAYED);
            EnableMenuItem(hmenu, ID_VIEW_RESCANLAYOUT,     MF_GRAYED);
        }
        return TRUE;

    case ID_FILE_SAVE:
        // Save the current workspace
        m_pws->Save();
        return TRUE;

    case ID_FILE_SAVEAS:
        // Query for a new name and save the current workspace
        m_pws->SaveAs();
        return TRUE;

    case ID_FILE_EXIT:
        // Exit the current workspace
        SendMessage(m_hwnd, WM_CLOSE, 0, 0);
        return TRUE;

    case ID_EDIT_SELECTALL:
        // Select all items in the current layer's listview
        m_plvCur->SelectAll();
        return TRUE;

    case ID_EDIT_GROUP:
        // Perform group functionality
        OnGroup();
        return TRUE;

    case ID_EDIT_UNGROUP:
        // Perform ungroup functionality
        OnUngroup();
        return TRUE;

	case ID_VIEW_LAYER0:
        // User wants to see layer 0
		SetCurLayer(0);
		return TRUE;

	case ID_VIEW_LAYER1:
        // User wants to see layer 1
		SetCurLayer(1);
		return TRUE;

	case ID_VIEW_BOTHLAYERS:
        // User wants to see both layers
		SetCurLayer(2);
		return TRUE;

    case ID_VIEW_SCRATCHAREA:
        // User toggled the 'view scratch area' button.
        m_pscratch->ToggleView();
        
        // Update the status of the 'view scratch area' toggle button.
        m_ptoolbar->CheckButton(ID_VIEW_SCRATCHAREA, m_pscratch->m_fVisible);
        CheckMenuItem(GetMenu(m_hwnd), ID_VIEW_SCRATCHAREA, m_pscratch->m_fVisible ? MF_CHECKED : MF_UNCHECKED);
        break;

    case ID_VIEW_RESCANLAYOUT:
        if (!CheckModifiedFiles())
            MessageBox(m_hwnd, "Files rescanned successfully.", "Rescan successful", MB_ICONINFORMATION | MB_OK);

        break;

    case ID_TOOLS_EMULATE:
        // User wants to spawn the dvd emulation tool

        // Force the user through save if they haven't yet saved.
        if (!lstrcmpi(m_pws->m_szName, ""))
            if (!m_pws->SaveAs())
            {
                MessageBoxResource(m_hwnd, IDS_ERR_MUSTSAVE_TEXT,
                                   IDS_ERR_MUSTSAVE_CAPTION,
                                   MB_ICONWARNING | MB_APPLMODAL | MB_OK);
                return TRUE;
            }
        if (m_pws->m_fModified)
            m_pws->Save();

        // Check if any of the files in the layout have changed or been removed,
        // or if any files have been added.  In either case, we need to update
        // the layout.
        if (CheckModifiedFiles())
            return TRUE;

        if (CheckNeedBuildForEmulation())
            if (!BuildForEmulation())
            {
                // Failed to build for emulation
                return TRUE;
            }

        // Generate the string used to spawn the emulator
        sprintf(sz, "dvdemuctrlgui.exe \"%s\\_amc\\%s.fst\"", g_szRootFolderName, strrchr(g_szRootFolderName, '\\')+1);

        ZeroMemory(&sInfo, sizeof(STARTUPINFO));
	    sInfo.cb = sizeof(STARTUPINFO);

        // Spawn the process
	    if (!CreateProcess(NULL, sz, NULL, NULL, TRUE,
				           NULL, NULL, NULL, &sInfo, &pInfo))
	    {
            MessageBoxResource(m_hwnd, IDS_ERR_LAUNCHDVDEMU_TEXT,
                               IDS_ERR_LAUNCHDVDEMU_CAPTION,
                               MB_ICONWARNING | MB_APPLMODAL | MB_OK);
	    }
        return TRUE;
        
    case ID_TOOLS_PREMASTERTAPE:
        // User wants to spawn the premastering tool.

        // Force the user through save if they haven't yet saved.
        if (!lstrcmpi(m_pws->m_szName, ""))
            if (!m_pws->SaveAs())
            {
                MessageBoxResource(m_hwnd, IDS_ERR_MUSTSAVE_TEXT,
                                   IDS_ERR_MUSTSAVE_CAPTION,
                                   MB_ICONWARNING | MB_APPLMODAL | MB_OK);
                return TRUE;
            }
        if (m_pws->m_fModified)
            m_pws->Save();

        // Check if any of the files in the layout have changed or been removed,
        // or if any files have been added.  In either case, we need to update
        // the layout.
        if (CheckModifiedFiles())
            return TRUE;
        
        if (CheckNeedBuildForEmulation())
            if (!BuildForEmulation())
            {
                // Failed to build for emulation
                return TRUE;
            }


	    ZeroMemory(&sInfo, sizeof(STARTUPINFO));
	    sInfo.cb = sizeof(STARTUPINFO);

        // Create the full command line to spawn.  This will tell xbPremaster
        // the name and location of the FLD file to source off of.
        sprintf(sz, "xbPremaster.exe \"%s\"", m_pws->m_szName);

        // Spawn the process
        if (!CreateProcess(NULL, sz, NULL, NULL, TRUE, NULL, NULL, NULL,
                           &sInfo, &pInfo))
	    {
            MessageBoxResource(m_hwnd, IDS_ERR_LAUNCHPREM_TEXT,
                               IDS_ERR_LAUNCHPREM_CAPTION,
                               MB_ICONWARNING | MB_APPLMODAL | MB_OK);
	    }
        return TRUE;

    case ID_HELP_CONTENTS:
        LoadString(m_hinst, IDS_HELP_FILE_NAME, sz, 256);
    
        // Generate the exe spawn name\parms.
        sprintf(sz2, "winhlp32.exe %s\\%s", m_szWorkingDir, sz);

        ZeroMemory(&sInfo, sizeof(STARTUPINFO));
        sInfo.cb = sizeof(STARTUPINFO);
        if (!CreateProcess(NULL, sz2, NULL, NULL, TRUE, NULL, NULL, NULL,
                            &sInfo, &pInfo))
        {
            MessageBoxResource(m_hwnd, IDS_ERR_LAUNCHHELP_TEXT,
                               IDS_ERR_LAUNCHHELP_CAPTION,
                               MB_ICONERROR | MB_APPLMODAL | MB_OK);
        }
        return TRUE;

    case ID_HELP_ABOUTXBOXDVDLAYOUTTOOL:
        // User wants to see the 'About' box.
        DialogBox(m_hinst, (LPCTSTR)IDD_ABOUTBOX, m_hwnd, (DLGPROC)AboutDlgProc);
        return TRUE;
    }
    
    // If here, then we didn't handle the message
    return FALSE;
}

bool CWindow_Main::CMF_HandleFiles()
{
    // Recursively scan the contents of every directory -
    // A. for each file or folder found, find it in the layout
    //   a. If the file does not exist in the layout, create a new object
    //      and place it in the unplaced files window
    //   b. If the file exists in the layout, mark it as 'present'
    //   c. If the file exists, but its size has changed, then remove it
    //      from the layout and place it in the unplaced files window

    WIN32_FIND_DATA wfd;
    char szSearch[MAX_PATH], szCurDir[MAX_PATH];
    CObject *poNew;

    GetCurrentDirectory(MAX_PATH, szCurDir);

    // Create the search string
    sprintf(szSearch, "%s\\*.*", szCurDir);

    HANDLE hff = FindFirstFile(szSearch, &wfd);
    if (hff == INVALID_HANDLE_VALUE)
        return false;
    do {
        if (!lstrcmpi(wfd.cFileName, ".") || !lstrcmpi(wfd.cFileName, "..") || !lstrcmpi(wfd.cFileName, "_amc"))
            continue;

        if (!lstrcmpi(wfd.cFileName, "default.xbe"))
            m_fDefaultXbePresent = true;

        sprintf(szSearch, "%s\\%s", szCurDir, wfd.cFileName);

        // Search for the object in the layout
        bool fScratch = false;
        CObject *po = m_pws->m_pdvd->m_rgpolLayer[0]->FindObjectByFullFileName(szSearch);
        if (!po) po = m_pws->m_pdvd->m_rgpolLayer[1]->FindObjectByFullFileName(szSearch);
        if (!po) {
            po = m_pscratch->FindObjectByFullFileName(szSearch);
            if (po) fScratch = true;
        }

        if (!po) {
            // Object doesn't exist in the layout.
            // Create a new object and add it to the unplaced files window
            if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                poNew = new CObj_Folder(szCurDir, wfd.cFileName);
            else
                poNew = new CObj_File(szCurDir, wfd.cFileName,
                                      wfd.nFileSizeHigh, wfd.nFileSizeLow);
            m_pscratch->AddObject(poNew);
            m_fCMF_FilesInUnplaced = true;
        }
        else
        {
            // Object exists.  Is it the same size?
            // Folders are handled by the call to refreshdirectoryentries at the end.
            if (!(wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
                if (!fScratch && (wfd.nFileSizeHigh != po->m_uliSize.HighPart ||
                                  wfd.nFileSizeLow  != po->m_uliSize.LowPart)) {

                    // File size changed.
                    po->m_uliSize.HighPart = wfd.nFileSizeHigh;
                    po->m_uliSize.LowPart  = wfd.nFileSizeLow;
                    DWORD dwNewBlockSize = BlockSize(po->m_uliSize.QuadPart);
                    if (dwNewBlockSize != po->m_dwBlockSize) {
                        po->m_dwBlockSize = dwNewBlockSize;
                        po->m_fCMF_Resized  = true;
                        m_fCMF_FilesResized = true;
                    }
                }
            }
            po->m_fCMF_Present = true;
        }

        if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            SetCurrentDirectory(szSearch);
            if (!CMF_HandleFiles())
                return false;
            SetCurrentDirectory("..");
        }
    } while (FindNextFile(hff, &wfd));
    FindClose(hff);

    return true;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CWindow_Main::CheckModifiedFiles
// Purpose:   
// Arguments: None
// Return:    None
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool CWindow_Main::CheckModifiedFiles()
{
    // Check if any of the files or folders in the layout have been modified.
    // we need to do a relayout if any sizes have changed.

    // 1. Mark all objects in layout as 'not present'
    // 2. Recursively scan the contents of every directory -
    //    A. for each file or folder found, find it in the layout
    //      a. If the file does not exist in the layout, create a new object
    //         and place it in the unplaced files window
    //      b. If the file exists in the layout, mark it as 'present'
    //      c. If the file exists, but its size has changed, then remove it
    //         from the layout and place it in the unplaced files window
    // 3. Walk the layout -- if any files are marked as 'not present' then
    //    remove them from the layout
    // 4. The layout now contains only unmodified files that are still present,
    //    and holes where modified/removed files were. Compact each layer now.
    // 5. Refresh directory entries if any files were added or removed.  This
    //    may force files into the unplaced files window
    // 6. If any files were placed in the unplaced files window, then notify
    //    the user that the directory structure or files changed, and modified
    //    files have been placed in the unplaced files window

    bool fCMF_FileDeleted = false;
    m_fCMF_FilesInUnplaced = false;
    m_fCMF_FilesResized = false;
    m_fDefaultXbePresent = false;

    // 1. Mark all objects in layout as 'not present'
    for (int i = 0; i < 2; i++) {
        CObject *poCur = m_pws->m_pdvd->m_rgpolLayer[i]->GetInside();
        while (poCur) {
            if (poCur->GetType() == OBJ_FILE || poCur->GetType() == OBJ_FOLDER) {
                poCur->m_fCMF_Present = false;
                poCur->m_fCMF_Resized = false;
            }
            poCur = m_pws->m_pdvd->m_rgpolLayer[i]->GetNextOuter();
        }
    }
    int iItem = -1;
    while ((iItem = ListView_GetNextItem(m_pscratch->m_plistview->m_hwnd, iItem, 0)) != -1)
    {
        CObject *poCur = m_pscratch->m_plistview->GetObjectFromIndex(iItem);
        poCur->m_fCMF_Present = false;
    }

    // 2. Recursively scan the contents of every directory -
    //    A. for each file or folder found, find it in the layout
    //      a. If the file does not exist in the layout, create a new object
    //         and place it in the unplaced files window
    //      b. If the file exists in the layout, mark it as 'present'
    //      c. If the file exists, but its size has changed, then remove it
    //         from the layout and place it in the unplaced files window
    SetCurrentDirectory(g_szRootFolderName);
    CMF_HandleFiles();

    // 3. Walk the layout -- if any files are marked as 'not present' then
    //    remove them from the layout
    for (i = 0; i < 2; i++) {
        CObject *poCur = m_pws->m_pdvd->m_rgpolLayer[i]->GetInside();
        while (poCur) {
            CObject *poNext = m_pws->m_pdvd->m_rgpolLayer[i]->GetNextOuter();
            if (!poCur->m_fCMF_Present && lstrcmpi(g_szRootFolderName, poCur->m_szFullFileName)) {
                poCur->RemoveFromList();
                if (poCur->m_pog)
                    poCur->m_pog->RemoveGroupedObject(poCur);
                delete poCur;
                fCMF_FileDeleted = true;
            }

            poCur = poNext;
        }
    }

    // 3b. walk the scratch window as well
    iItem = -1;
    while ((iItem = ListView_GetNextItem(m_pscratch->m_plistview->m_hwnd, iItem, 0)) != -1)
    {
        CObject *poCur = m_pscratch->m_plistview->GetObjectFromIndex(iItem);
        if (!poCur->m_fCMF_Present && lstrcmpi(g_szRootFolderName, poCur->m_szFullFileName)) {
            poCur->RemoveFromList();
            if (poCur->m_pog)
                poCur->m_pog->RemoveGroupedObject(poCur);
            m_pscratch->m_plistview->RemoveObject(poCur);
            delete poCur;
            iItem = -1;
        }
    }

    if (m_fCMF_FilesResized) {
        // At least one file was resized.  Find the farthest out file on each
        // layer (if present) and remove\reinsert it
        for (i = 0; i < 2; i++) {
            CObject *poCur = m_pws->m_pdvd->m_rgpolLayer[i]->GetOutside();
            while (poCur) {
                if (poCur->m_fCMF_Resized) {
                    // Found it - poCur is the farthest out resized object on
                    // this layer.  Remove it and reinsert it - this will force
                    // reinsertion of all objects above it, and potentially
                    // move objects off of the layer.

                    // Track which object is immediately outside of the resized one.
                    CObject *poOuter = m_pws->m_pdvd->m_rgpolLayer[i]->GetNextOuter();
                    m_pws->QueueSnapspot();

                    // Remove the object
                    poCur->RemoveFromList();
                    if (poCur->m_pog)
                        poCur->m_pog->RemoveGroupedObject(poCur);

                    // We need the name of the object in case we need to dequeue
                    // the snapshot, which renders poCur invalid.
                    char szName[MAX_PATH];
                    strcpy(szName, poCur->m_szFullFileName);

                    // Reinsert that object 'inside of' the next outermost object
                    if (!m_pws->m_pdvd->Insert(m_pws->m_pdvd->m_rgpolLayer[i], poCur, poOuter))
                    {
                        // Failed to insert the object; restore the previous state
                        // and add it to the unplaced files window
                        m_pws->DequeueSnapspot();

                        // Refind the object (since the dequeue made poCur invalid)
                        poCur = m_pws->m_pdvd->m_rgpolLayer[i]->FindObjectByFullFileName(szName);
                        assert(poCur);
                        
                        poCur->RemoveFromList();
                        if (poCur->m_pog)
                            poCur->m_pog->RemoveGroupedObject(poCur);

                        m_pscratch->AddObject(poCur);
                        m_fCMF_FilesInUnplaced = true;

                        // Start again on the outside of the disc (since the
                        // dequeue made previous pointers invalid).
                        poCur = m_pws->m_pdvd->m_rgpolLayer[i]->GetOutside();
                        continue;
                    }

                    m_pws->CompactLayer(i);

                    // We're done with this layer, so break out of the loop.
                    break;
                }

                poCur = m_pws->m_pdvd->m_rgpolLayer[i]->GetNextInner();
            }
        }
    }

    // 4. Compact each layer now.
    m_pws->CompactLayer(0);
    m_pws->CompactLayer(1);

    // Reset the 'warned about too many files' flag so that, if there are *now*
    // too many directory entries, we warn the user (again, if appropriate).
    m_pws->m_pdvd->m_fWarnedTooManyFiles = false;

    // 5. Refresh directory entries if any files were added or removed.  This
    //    may force files into the unplaced files window
    m_pws->m_pdvd->RefreshPlaceholders();
    m_pws->m_pdvd->RefreshDirectoryEntries(m_pws, false);
    
    // Update the views that display the workspace's information
    m_pws->UpdateViews();

    if (!m_fDefaultXbePresent)
    {
        // No default.xbe present! warn user
        MessageBox(m_hwnd, "The specified directory does not contain " \
                   "default.xbe.\nDefault.xbe MUST be present or the application " \
                   "will fail to execute.", "Default.xbe not present",
                   MB_ICONWARNING | MB_OK);
    }

    // 6. If any files were placed in the unplaced files window, then notify
    //    the user that the directory structure or files changed, and modified
    //    files have been placed in the unplaced files window
    if (m_fCMF_FilesInUnplaced) {
        m_pws->SetModified();
        m_pws->UpdateWindowCaption();
        m_pws->m_pdvd->RefreshRelationships();
        if (!m_pscratch->m_fVisible)
            SendMessage(m_hwnd, WM_COMMAND, ID_VIEW_SCRATCHAREA, 0);

        MessageBox(m_hwnd, "Files and/or folders in the layout have been" \
                           " modified.  They have been added to the unplaced" \
                           " files window.  Please examine it and determine " \
                           " their final placement.", "Files Moved to Unplaced",
                           MB_ICONWARNING | MB_OK);
        return true;
    }
    else if (fCMF_FileDeleted) {
        m_pws->SetModified();
        m_pws->UpdateWindowCaption();
        m_pws->m_pdvd->RefreshRelationships();
        MessageBox(m_hwnd, "Files and/or folders in the layout have been"
                           " removed from the source directories.  The layout"
                           " has been updated to reflect this - please examine"
                           " it for correctness.", "Files Removed",
                           MB_ICONWARNING | MB_OK);
        return true;
    }
    else if (m_fCMF_FilesResized) {
        m_pws->SetModified();
        m_pws->UpdateWindowCaption();
        m_pws->m_pdvd->RefreshRelationships();
        MessageBox(m_hwnd, "Files and/or folders in the layout have changed"
                           " size.  The layout has been updated to reflect"
                           " this - please examine it for correctness.",
                           "Files Removed", MB_ICONWARNING | MB_OK);
        return true;
    }


    // Return that no files were placed in unplaced window, removed, or resized
    return false;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CWindow_Main::CheckNeedBuildForEmulation
// Purpose:   Return 'true' if the FST and BIN files need to be regenerated for 
//            emulation.  We know this is the case if the FLD file's timestamp
//            is newer than the FST's timestamp.
// Arguments: None
// Return:    'true' if the FST and BIN need to be rebuilt
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool CWindow_Main::CheckNeedBuildForEmulation()
{
    sFLDHeader fldhdr;
    char   szFST[MAX_PATH];
    bool   fRebuild = true;
    time_t timeFST;
    DWORD  dwTimeStampLoc;

    // Compare the 'FLD last saved' timestamps stored in the FLD and FST.
    // 1. If FLD is old-version FLD, then force rebuild
    // 2. If timestamps are different, then force rebuild
    // 3. If either file can't be found, then force rebuild

    sprintf(szFST, "%s\\_amc\\%s.fst", g_szRootFolderName,
                                       strrchr(g_szRootFolderName, '\\')+1);
    
    // Load the two files.
    CFile fileFST(szFST, FILE_READ | FILE_DISK);
    CFile fileFLD(m_pws->m_szName, FILE_READ | FILE_DISK);

    // Verify both files were successfully loaded.
    if (!fileFST.IsInited())
        goto done;
    if (!fileFLD.IsInited())
        goto done;

    // Get the FLD signature
    if (!fileFLD.ReadData(&fldhdr, sizeof fldhdr))
        goto done;
    
    // Verify it's a newer FLD file; if it's an older one, then force rebuild
    if (memcmp(fldhdr.szSig, FLD_SIG, FLD_SIG_SIZE))
        goto done;
    
    // Get the FST timestamp
    dwTimeStampLoc = sizeof(TFileHeader) + sizeof(TMediaHeader) + 2;
    if (!fileFST.SetPos(dwTimeStampLoc))
        goto done;
    if (!fileFST.ReadData(&timeFST, sizeof timeFST))
        goto done;

    // If times are different, force rebuild
    fRebuild = (fldhdr.timeSaved != timeFST);

done:

    return fRebuild;        
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CWindow_Main::OnNotify
// Purpose:   Certain windows events are sent via 'notifications'.  We handle
//            those notifications here.
// Arguments: wparam            -- First param passed to WM_NOTIFY
//            lparam            -- Second param passed to WM_NOTIFY
// Return:    Ignored unless a particular notification specifies.  See MSDN
//            for more information.
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool CWindow_Main::OnNotify(WPARAM wparam, LPARAM lparam)
{
    NMLISTVIEW   *pnmlv = (NMLISTVIEW*)lparam;
    NMHDR        *pnmhdr = (NMHDR*)lparam;
    NMTTDISPINFO *pnmttdi = (LPNMTTDISPINFO)lparam;
    char         szTooltip[100];
    static bool  fChanging = false;
    CListView    *plvTemp;
    NMLVKEYDOWN  *pnmlvkd = (NMLVKEYDOWN*)lparam;
    bool         fRet;

    switch(pnmlv->hdr.code)
    {
    case LVN_BEGINDRAG:
        // User began a dragging session.
        m_plvCur->BeginDrag();
        break;

    case TTN_GETDISPINFO:
        // The OS is asking for tooltip text (the user paused over a toolbar
        // button).

        // Load the string resource for the moused-over tool
        LoadString(m_hinst, g_rgdwTooltip[pnmttdi->hdr.idFrom - 40001],
                   szTooltip, 100);

        // Store the appropriate text in the structure
        strcpy(pnmttdi->szText, szTooltip);

        // Don't ask for this again (since it never changes)
        pnmttdi->uFlags = TTF_DI_SETITEM;
        break;

    case LVN_ITEMCHANGING:
        // An item is changing focus or selection state
        if ((pnmlv->uNewState & LVIS_SELECTED) != (pnmlv->uOldState & LVIS_SELECTED))
        {
            // Annoyingly, Windows sends the itemchanging notification BEFORE it
            // sends the setfocus notification (sigh).  So check which listview
            // this is intended for via the hwnd passed in...
            plvTemp = GetLVFromHwnd(pnmhdr->hwndFrom);
#ifdef _DEBUG1
            {
                CObject *pobj = plvTemp->GetObjectFromIndex(pnmlv->iItem);
                DebugOutput("ItemChanging: %d (%s, %s) %s->%s", pnmlv->iItem, pobj->m_szName, pobj->m_fIgnoreNextUnselect ? "ignore next": "do next",
                    (pnmlv->uOldState & LVIS_SELECTED) ? "selected" : "unselected",
                    (pnmlv->uNewState & LVIS_SELECTED) ? "selected" : "unselected");
            }
#endif
            fRet = !plvTemp->CanItemChange(pnmlv->iItem, pnmlv->uOldState, pnmlv->uNewState);
#ifdef _DEBUG1
            DebugOutput("  (%s)\r\n", fRet ? "Denied" : "Accepted");
#endif
            return fRet;
        }
        return false;

    case LVN_ITEMCHANGED:
        // An item changed focus or selection state
        if (pnmlv->uChanged & LVIF_STATE)
        {
            // If the selection state changed, then notify the listview.
            if ((pnmlv->uNewState & LVIS_SELECTED) != (pnmlv->uOldState & LVIS_SELECTED))
            {
                // User clicked on one of our list views.  A different window may
                // have current focus (since LVN_ITEMCHANGED is sent before the
                // NM_SETFOCUS message).
                plvTemp = GetLVFromHwnd(pnmhdr->hwndFrom);

#ifdef _DEBUG1
                CObject *pobj = plvTemp->GetObjectFromIndex(pnmlv->iItem);
                DebugOutput("ItemChanged: %d (%s, %s) %s->%s\r\n", pnmlv->iItem, pobj->m_szName, pobj->m_fIgnoreNextUnselect ? "ignore next": "do next",
                    (pnmlv->uOldState & LVIS_SELECTED) ? "selected" : "unselected",
                    (pnmlv->uNewState & LVIS_SELECTED) ? "selected" : "unselected");
#endif
                plvTemp->ItemSelected(pnmlv->iItem, pnmlv->uNewState & LVIS_SELECTED ? true : false);

                // Update the status bar (which shows selected-item information).
                UpdateStatusBar();
            }
        }
        break;

    case NM_SETFOCUS:
        // User clicked on one of our list views.

        // Did they click on the one that already has the focus?
        if (pnmhdr->hwndFrom == m_plvCur->GetHwnd())
            break;

        // Remove focus from the 'old' current listview
        m_plvCur->SetFocus(false);
        
        // Determine which listview the use clicked on.
        m_plvCur = GetLVFromHwnd(pnmhdr->hwndFrom);

        // Set focus to the 'new' current listview
        m_plvCur->SetFocus(true);
        break;

    case LVN_KEYDOWN:
        // User pressed a key.  Check to see if the user can move to the
        // requested location in the current listview
        if (m_plvCur->MoveSelection(pnmlvkd->wVKey))
            break;

        // Otherwise, fall through and let the normal wndproc handle it

    default:
        return DerivedWndProc(WM_NOTIFY, wparam, lparam) ? TRUE: FALSE;

    }
    return TRUE;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CWindow_Main::GetLVFromHwnd
// Purpose:   Determines which listview cooresponds to the specified hwnd
// Arguments: 
// Return:    Listview with the specified hwnd
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
CListView *CWindow_Main::GetLVFromHwnd(HWND hwnd)
{
    if (hwnd == m_rgplv[LV_LAYER0]->m_hwnd)
        return m_rgplv[LV_LAYER0];
    else if (hwnd == m_rgplv[LV_LAYER1]->m_hwnd)
        return m_rgplv[LV_LAYER1];
    else
        return m_rgplv[LV_SCRATCH];
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CWindow_Main::SetCurLayer
// Purpose:   Called when the user wants to see layer 0, 1, or both.
// Arguments: nLayer        -- The layer to show
// Return:    None
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CWindow_Main::SetCurLayer(int nLayer)
{
	if (nLayer == LV_LAYER0 || nLayer == LV_LAYER1)
	{
        // User wants to see either layer 0 or layer 1
        m_plvCur = m_rgplv[nLayer];
        m_plvCur->SetVisible(true);
        m_ptoolbar->CheckButton(nLayer == LV_LAYER0 ? ID_VIEW_LAYER0 :
                                                      ID_VIEW_LAYER1, true);
	}
    else
    {
        // User wants to see both layer 0 and layer 1.  Leave the current view
        // focus on which view already had it.
        m_rgplv[LV_LAYER0]->SetVisible(true);
        m_rgplv[LV_LAYER1]->SetVisible(true);
        m_ptoolbar->CheckButton(ID_VIEW_BOTHLAYERS, true);
    }

    // Track which layer (or both) is currently visible
    m_nCurLayerView = nLayer;

    // Force a repaint since we changed the visible listviews
    OnSize();

    m_pws->UpdateWindowCaption();

    // Update the status bar (which shows layer-specific information).
    UpdateStatusBar();
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CWindow_Main::GetFocusLayer
// Purpose:   Returns the number of the layer that currently has the focus
// Arguments: None
// Return:    The layer that currently has the focus
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
int CWindow_Main::GetFocusLayer()
{
    return (m_plvCur == m_rgplv[LV_LAYER0]) ? 0 : 1;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CWindow_Main::UpdateStatusBar
// Purpose:   Updates the text in the status bar.
// Arguments: None
// Return:    None
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CWindow_Main::UpdateStatusBar()
{
    char szText[256], szSelText[256], szNum[256];
    int nSelectedFiles, nSelectedSize;
    char szFormat[1024], szFormat2[1024];

    LoadString(m_hinst, IDS_STATUS, szFormat, 1024);
    LoadString(m_hinst, IDS_STATUS2, szFormat2, 1024);
        
    if (m_nCurLayerView == 2)
    {
        // Both layers are visible, so break the status bar into two parts
        m_pstatusbar->SetNumParts(2);

        // Generate the text for the first half of the status bar (layer 0)
        FormatByteString(m_pws->GetNumUsedSectors(0)*2048, szNum);
        sprintf(szText, szFormat, 0, m_pws->GetNumFiles(0), szNum);

        // Get information about the selected files on layer 0
        m_rgplv[LV_LAYER0]->GetSelectedFileCountAndSize(&nSelectedFiles, &nSelectedSize);
        if (nSelectedFiles)
        {
            FormatByteString(nSelectedSize, szNum);
            sprintf(szSelText, szFormat2, nSelectedFiles, szNum);
            strcat(szText, szSelText);
        }
        m_pstatusbar->SetText(0, szText);

        // Generate the text for the second half of the status bar (layer 1)
        FormatByteString(m_pws->GetNumUsedSectors(1)*2048, szNum);
        sprintf(szText, szFormat, 1, m_pws->GetNumFiles(1), szNum);

        // Get information about the selected files on layer 1
        m_rgplv[LV_LAYER1]->GetSelectedFileCountAndSize(&nSelectedFiles, &nSelectedSize);
        if (nSelectedFiles)
        {
            FormatByteString(nSelectedSize, szNum);
            sprintf(szSelText, szFormat2, nSelectedFiles, szNum);
            strcat(szText, szSelText);
        }
        m_pstatusbar->SetText(1, szText);
    }
    else
    {
        // Only one layer is currently visible, so we only need one part in the
        // status bar.
        m_pstatusbar->SetNumParts(1);

        // Generate the text that will get drawn into the status bar.
        FormatByteString(m_pws->GetNumUsedSectors(GetFocusLayer())*2048, szNum);
        sprintf(szText, szFormat, GetFocusLayer() == 0 ? 0 : 1,
                m_pws->GetNumFiles(GetFocusLayer()), szNum);

        // Get the selected-file information from the current layer.
        (GetFocusLayer() == 0 ? m_rgplv[LV_LAYER0] : m_rgplv[LV_LAYER1])->GetSelectedFileCountAndSize(&nSelectedFiles, &nSelectedSize);

        // If there are any selected files, then add their information to the
        // status bar text
        if (nSelectedFiles)
        {
            FormatByteString(nSelectedSize, szNum);
            sprintf(szSelText, szFormat2, nSelectedFiles, szNum);
            strcat(szText, szSelText);
        }

        // Set the text inside the status bar
        m_pstatusbar->SetText(0, szText);
    }
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CWindow_Main::OnSize
// Purpose:   Called when the user resizes the window.  We resize the controls to fill the new
//            window size
// Arguments: None
// Return:    None
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CWindow_Main::OnSize()
{
    RECT rcToolbar;
    RECT rcStatusbar;

    // Get the new dimensions of the window and various controls
    GetClientRect(m_hwnd, &m_rc);
    GetClientRect(m_ptoolbar->m_hwnd, &rcToolbar);
    GetClientRect(m_pstatusbar->m_hwnd, &rcStatusbar);

    // Calculate the height of the listview controls
    int nListHeight = (m_rc.bottom - m_rc.top) - rcToolbar.bottom -
                      (rcStatusbar.bottom - rcStatusbar.top) - 2;

    switch(m_nCurLayerView)
    {
    case 0:
        // Set the visibility of the different controls
        m_pcolbarLeft->SetVisible(true);
        m_pcolbarMid->SetVisible(false);
        m_pcolbarRight->SetVisible(false);
        m_rgplv[LV_LAYER0]->SetVisible(true);
        m_rgplv[LV_LAYER1]->SetVisible(false);

        // Move the layer 0 list control
        m_rgplv[LV_LAYER0]->MoveTo(COLBAR_WIDTH, rcToolbar.bottom + 2,
                                   (m_rc.right - m_rc.left) - COLBAR_WIDTH,
                                   nListHeight);

        // Make sure the left-side colorbar is in the right place
        m_pcolbarLeft->MoveTo(0, rcToolbar.bottom + 3, COLBAR_WIDTH, nListHeight-1);
        InvalidateRect(m_rgplv[LV_LAYER0]->m_hwnd, NULL, FALSE);
        break;

    case 1:
        // Set the visibility of the different controls
        m_pcolbarLeft->SetVisible(false);
        m_pcolbarMid->SetVisible(false);
        m_pcolbarRight->SetVisible(true);
        m_rgplv[LV_LAYER0]->SetVisible(false);
        m_rgplv[LV_LAYER1]->SetVisible(true);

        // Move the layer 1 list control
        m_rgplv[LV_LAYER1]->MoveTo(COLBAR_WIDTH, rcToolbar.bottom + 2,
                                   (m_rc.right - m_rc.left) - COLBAR_WIDTH,
                                   nListHeight);

        // Make sure the right-side colorbar is in the right place
        m_pcolbarRight->MoveTo(0, rcToolbar.bottom + 3, COLBAR_WIDTH, nListHeight-1);
        InvalidateRect(m_rgplv[LV_LAYER1]->m_hwnd, NULL, FALSE);
        break;

    case 2:
        // Set the visibility of the different controls
        m_pcolbarLeft->SetVisible(true);
        m_pcolbarMid->SetVisible(true);
        m_pcolbarRight->SetVisible(true);
        m_rgplv[LV_LAYER0]->SetVisible(true);
        m_rgplv[LV_LAYER1]->SetVisible(true);

        m_pcolbarLeft->MoveTo(0, rcToolbar.bottom + 3, COLBAR_WIDTH, nListHeight-1);
        m_rgplv[LV_LAYER0]->MoveTo(COLBAR_WIDTH, rcToolbar.bottom + 2,
                                   (m_rc.right - m_rc.left)/2 - COLBAR_WIDTH - 3, nListHeight);
        m_pcolbarMid->MoveTo((m_rc.right - m_rc.left)/2 - 3, rcToolbar.bottom + 3, 6, nListHeight-1);
        m_rgplv[LV_LAYER1]->MoveTo((m_rc.right - m_rc.left)/2 + 3, rcToolbar.bottom + 2,
                                   ((m_rc.right - m_rc.left) - COLBAR_WIDTH) - (rcToolbar.bottom + 2,
                                   (m_rc.right - m_rc.left)/2 + 3), nListHeight);
        m_pcolbarRight->MoveTo((m_rc.right - m_rc.left) - COLBAR_WIDTH,
                               rcToolbar.bottom + 3, COLBAR_WIDTH, nListHeight-1);
        InvalidateRect(m_rgplv[LV_LAYER0]->m_hwnd, NULL, FALSE);
        InvalidateRect(m_rgplv[LV_LAYER1]->m_hwnd, NULL, FALSE);
        break;
    }
    // Move the status bar into place.
    m_pstatusbar->MoveTo(0, (m_rc.bottom - m_rc.top) - rcToolbar.bottom,
                         (m_rc.right - m_rc.left), (m_rc.bottom - m_rc.top));
    m_ptoolbar->MoveTo(rcToolbar.left, rcToolbar.top, (m_rc.right - m_rc.left), rcToolbar.bottom - rcToolbar.top);

    // Update our tooltip's position
    TOOLINFO ti;
    ti.cbSize = sizeof(TOOLINFO);
    ti.uFlags = TTF_SUBCLASS;
    ti.hwnd = m_hwnd;
    ti.hinst = g_hinst;
    ti.uId = 0;
    ti.lpszText = MAKEINTRESOURCE(IDS_COLORBAR_TOOLTIP);
    ti.rect = m_rc;

    SendMessage(m_hwndTip, TTM_SETMAXTIPWIDTH, 0, 80);

 
    // Delete Previous tool (if any)
    SendMessage(m_hwndTip, TTM_DELTOOL, 0, (LPARAM) (LPTOOLINFO) &ti);

    // Send an ADDTOOL message to the tooltip control window
    SendMessage(m_hwndTip, TTM_ADDTOOL, 0, (LPARAM) (LPTOOLINFO) &ti);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  AboutDlgProc
// Purpose:   Message handler for About dialog. This function is automatically called by Windows
//            whenever a message needs to be sent to the dialog box (ie 'paint', 'move', etc).
// Arguments: hDlg          -- Handle to the Dialog box's window
//            message       -- The message sent to the dialog box
//            wParam        -- A message parameter.
//            lParam        -- Another message parameter
// Return:    nonzero if we processed the message
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
LRESULT CALLBACK AboutDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    char szBuf[1024], szFormat[1024];

	switch (message)
	{
	case WM_INITDIALOG:
        // Get the build number from the resource file
        LoadString(g_hinst, IDS_ABOUT, szFormat, 1024);
        sprintf(szBuf, szFormat, VER_PRODUCTBUILD);
        SetDlgItemText(hDlg, IDC_ABOUTTEXT, szBuf);
		return true;

	case WM_COMMAND:
		EndDialog(hDlg, 0);
		return false;
	}
    return false;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CWindow_Main::SetGroupable
// Purpose:   Set whether or not the group function is currently enabled.
// Arguments: fGroupable        -- Whether or not to enable 'group'.
// Return:    None
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CWindow_Main::SetGroupable(bool fGroupable)
{
    HMENU hmenu = GetMenu(m_hwnd);
    EnableMenuItem(hmenu, ID_EDIT_GROUP, fGroupable ? MF_ENABLED : MF_GRAYED);
    m_ptoolbar->EnableButton(ID_EDIT_GROUP, fGroupable);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CWindow_Main::SetUngroupable
// Purpose:   Set whether or not the ungroup function is currently enabled.
// Arguments: fUngroupable      -- Whether or not to enable 'ungroup'.
// Return:    None
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CWindow_Main::SetUngroupable(bool fUngroupable)
{
    HMENU hmenu = GetMenu(m_hwnd);
    EnableMenuItem(hmenu, ID_EDIT_UNGROUP, fUngroupable ? MF_ENABLED : MF_GRAYED);
    m_ptoolbar->EnableButton(ID_EDIT_UNGROUP, fUngroupable);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CWindow_Main::OnGroup
// Purpose:   Called when the user wants to group the currently selected items
//            We know if we got here that the items are groupable.
// Arguments: None
// Return:    None
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CWindow_Main::OnGroup()
{
    CObject *poInsertAt;
    int     iAddAt;

    // Create the new group object that the files will go into.
    CObj_Group *pog = new CObj_Group;
    if (!pog)
        FatalError(E_OUTOFMEMORY);

    // Determine the object immediately before the first selected object.  That
    // is the point at which the group of objects will be reinserted
    if (m_plvCur->m_fInsideOut)
        poInsertAt = m_plvCur->GetLastSelectedObject();
    else
        poInsertAt = m_plvCur->GetFirstSelectedObject();

    if (m_plvCur == m_rgplv[LV_SCRATCH])
    {
        iAddAt = m_pscratch->m_plistview->GetIndexFromObject(poInsertAt);
    }
    else
    {
        // poInsertAt points at the first item we'll be adding to the group; step up
        // one so that we're pointing at the first item above the group...  After all
        // of the to-be-grouped items are removed from the list, we step the the next
        // farther out object, and that's our actual insertion point (since insertion
        // occurs 'before' the specified insertion point).
        poInsertAt = poInsertAt->m_poInner;
    }
    
    // Get the list of all selected objects.  This will also remove them from
    // their current layer (if they are in one) and the listview.
    CObjList olToMove;
    m_plvCur->GetListOfSelectedObjects(&olToMove);

    // Now that the to-be-grouped items are out of the layer, step out one to
    // get the actual insertion point.
    if (m_plvCur != m_rgplv[LV_SCRATCH])
        poInsertAt = poInsertAt->m_poOuter;

    // Add all of the selected objects to the new group
    CObject *poCur = olToMove.GetInside();
    while (poCur)
    {
        poCur->SetGroup(pog);
        poCur = olToMove.GetNextOuter();
    }

    // Add the group to the list of objects
    olToMove.AddToHead(pog);

    // And then add the whole list back into the layer.  We go through all of
    // this hassle in case the user grouped a collection of files that straddled
    // a placeholder -- we need to remove and reinsert the grouped files in order
    // to atomically place the group on either side of the placeholder.
    if (m_plvCur == m_rgplv[LV_LAYER0])
    {
        m_pws->m_pdvd->Insert(m_pws->m_pdvd->m_rgpolLayer[0], &olToMove, poInsertAt);
        m_pws->m_pdvd->CompactLayer(0);
    }
    else if (m_plvCur == m_rgplv[LV_LAYER1])
    {
        m_pws->m_pdvd->Insert(m_pws->m_pdvd->m_rgpolLayer[1], &olToMove, poInsertAt);
        m_pws->m_pdvd->CompactLayer(1);
    }
    else
    {
        // Grouping in the scratch window.  Just add the files directly into the list
        CObject *poCur = olToMove.GetOutside();
        while (poCur)
        {
            if (poCur->GetType() != OBJ_GROUP)
                m_pscratch->m_plistview->AddObjectAtIndex(poCur, iAddAt);
            poCur = olToMove.GetNextInner();
        }
    }

    // We've modified the layers in the CompactLayer calls without updating the
    // listviews accordingly.  Update the listviews now.
    m_pws->UpdateViews();
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CWindow_Main::OnUngroup
// Purpose:   Called when the user wants to ungroup the currently selected items
//            We know if we got here that the items are ungroupable.
// Arguments: None
// Return:    None
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CWindow_Main::OnUngroup()
{
    // Tell the current listview to ungroup its currently selected items.
    m_plvCur->UngroupSelectedItems(m_pws, (m_plvCur == m_rgplv[LV_LAYER0]) ? 0 : 1);

    m_pws->CompactLayer((m_plvCur == m_rgplv[LV_LAYER0]) ? 0 : 1);

    // Update the onscreen display of the listviews
    m_pws->UpdateViews();
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CWindow_Main::BuildForEmulation
// Purpose:   Creates the .bin and .fst files necessary for emulation.
// Arguments: None
// Return:    None
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool CWindow_Main::BuildForEmulation()
{
    SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_WAIT)));

    // Create the .bin file
    bool fSuccess = m_pws->CreateBIN();
    if (!fSuccess)
        goto done;

    // Create the .fst file
    fSuccess = m_pws->CreateFST();    

done:

    SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW)));

    char szFormat[1024], szCaption[1024];
    LoadString(m_hinst, IDS_BFE_CAPTION, szCaption, 1024);
    if (!fSuccess)
    {
        char sz[1024];
        LoadString(m_hinst, IDS_BFE_SUCCESS, szFormat, 1024);
        sprintf(sz, szFormat, g_szRootFolderName,
                strrchr(g_szRootFolderName, '\\')+1);
        MessageBox(m_hwnd, sz, szCaption, MB_ICONINFORMATION | MB_APPLMODAL | MB_OK);
    }

    return fSuccess;
}
