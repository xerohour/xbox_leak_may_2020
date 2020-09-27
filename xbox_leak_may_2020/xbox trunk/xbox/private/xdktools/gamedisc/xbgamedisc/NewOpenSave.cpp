// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:      NewOpenSave.cpp
// Contents:  
// Revisions: 28-Jan-2002: Created (jeffsim)
//
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ INCLUDE FILES ++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// "stdafx.h"       -- precompiled header file
#include "stdafx.h"
#include <commdlg.h>


// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ FUNCTIONS ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

CUpdateBox g_updatebox;

void HandleEvents();


void SetRoot(char *szPath)
{
    SetCursor(g_hcurWait);
    g_hcur = g_hcurWait;

    g_cq.Init(szPath);
    
    g_updatebox.Start("Creating Layout...");

    ClearModified();

    g_updatebox.SetStatus("Examining Root Directory contents");
    
    g_fh.SetRoot(szPath);

    g_updatebox.SetStatus("Populating Views");

    g_vmExplorer.m_treeview.Populate(g_fh.m_pfiRoot);
    g_vmExplorer.m_treeview.SetCurFolder(g_fh.m_pfiRoot);

    // "Start" the filechange queue object.  This object will be used to
    // notify us about changes to the underlying filesystem.  
    g_cq.Start(&g_fh);

    g_updatebox.SetStatus("Finalizing changes");
    
    // Don't let the UI go until we've handled all events
    while (g_cq.m_cChangeQueueReady > 0)
    {
        HandleEvents();
        g_cq.m_cChangeQueueReady--;
    }
    if (g_pvmCur)
        g_pvmCur->UpdateIfDirty();

    g_updatebox.Stop();

    SetCursor(g_hcurArrow);
    g_hcur = g_hcurArrow;
}

BOOL VerifyCloseWorkspace()
{
    assert (g_fModified);

    // Verify with the user that they want to exit even though the
    // workspace has been modified
    switch(MessageBoxResource(g_hwndMain, IDS_WS_MODIFIED_TEXT,
                           IDS_WS_MODIFIED_CAPTION,
                           MB_YESNOCANCEL | MB_ICONWARNING | MB_APPLMODAL))
    {
    case IDYES:
        // Save
        HandleSave();
        return TRUE;

    case IDNO:
        // Don't save, close
        return TRUE;

    case IDCANCEL:
        // Don't save, don't close
        return FALSE;
    }
    assert(FALSE);
    return FALSE;
}

void CloseWorkspace()
{
    eEvent event;
    CFileObject *pfo;

    // When here, user has said they want to close the current workspace; ergo,
    // we can shut everything down "messily".
    
    // 1. Empty out the queue so that the queue nodes are deleted
    g_cq.Stop();
    while (g_cq.HasEvent())
    {
        g_cq.Dequeue(&event, &pfo);
        if (event == EVENT_REMOVED)
            delete pfo;
    }

    // 2. Clean out the file hierarchy
    g_fh.RemoveAll();

    // 3. Clean out the UI elements
    g_vmExplorer.Clear();
    g_vmLayer.Clear();
    ClearModified();

    // 4. Disable save and save as...
    SetMenuCmds(MF_GRAYED);
}


typedef struct
{
    WORD wMajor;
    WORD wMinor;
    WORD wBuild;
    WORD wQFE;
} sVersion;

#define XLO_SIG "XBGDXLOFILE"
#define XLO_SIG_SIZE sizeof(XLO_SIG)
#include <time.h>
typedef struct
{
    BYTE  szSig[XLO_SIG_SIZE];
    char szRoot[MAX_PATH];
    time_t timeSaved;
    sVersion version;
} sXLOHeader;

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  PersistTo
// Purpose:   
// Arguments: 
// Return:    
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool PersistTo(char *szFLDName)
{
    sXLOHeader xlohdr;
    int nMajor, nMinor, nBuild, nQFE;

    CFile *pfile = new CFile(szFLDName, FILE_WRITE);
    if (!pfile)
        FatalError(E_OUTOFMEMORY);
    if (!pfile->IsInited())
    {
        MessageBoxResource(g_hwndMain, IDS_ERR_SAVE_TEXT,
                           IDS_ERR_SAVE_CAPTION, MB_APPLMODAL | MB_OK);
        if (pfile)
            delete pfile;
        return false;
    }

    SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_WAIT)));

    // Write out the header information
    memcpy(xlohdr.szSig, XLO_SIG, XLO_SIG_SIZE);
    xlohdr.timeSaved = time(NULL);
    sscanf(VER_PRODUCTVERSION_STR, "%d.%d.%d.%d", &nMajor, &nMinor, &nBuild,
                                                  &nQFE);
    xlohdr.version.wMajor = (WORD)nMajor;
    xlohdr.version.wMinor = (WORD)nMinor;
    xlohdr.version.wBuild = (WORD)nBuild;
    xlohdr.version.wQFE   = (WORD)nQFE;
    strcpy(xlohdr.szRoot, g_fh.m_szRootDir);

    pfile->WriteData(&xlohdr, sizeof xlohdr);

    g_fh.PersistTo(pfile);
    delete pfile;

    ClearModified();

    SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW)));

    return true;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  SaveAs
// Purpose:   
// Arguments: 
// Return:    
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool SaveAs()
{
    // Get the name of the XLO file to save to.
    OPENFILENAME ofn;
    char szFile[MAX_PATH];
    char szFilter[1024], szTitle[1024];

    LoadString(g_hInst, IDS_OPEN_FILTER, szFilter, 1024);
    char *szExt = szFilter + strlen(szFilter) + 1;
    strcpy(szExt, "*.XLO");
    szExt[6] = '\0';
    LoadString(g_hInst, IDS_SAVEAS_TITLE, szTitle, 1024);

    strcpy(szFile, "");

    // 1. Get the name of the FLD file to open.

    // Initialize the openfilename structure so that the GetOpenFileName function knows what
    // kind of files we want to allow.
    memset(&ofn, 0, sizeof(ofn));
    ofn.lStructSize       = sizeof(ofn);
    ofn.hwndOwner         = g_hwndMain;
    ofn.hInstance         = g_hInst;
    ofn.lpstrFilter       = szFilter;
    ofn.lpstrCustomFilter = NULL;
    ofn.nFilterIndex      = 1;
    ofn.lpstrFile         = szFile;
    ofn.nMaxFile          = MAX_PATH - 1;
    ofn.lpstrFileTitle    = ofn.lpstrFile;
    ofn.nMaxFileTitle     = ofn.nMaxFile;
    ofn.lpstrInitialDir   = "C:\\";
    ofn.lpstrDefExt       = "*.xlo";
    ofn.lpstrTitle        = szTitle;
    ofn.Flags             = OFN_CREATEPROMPT | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;

    // Pop up the common file open dialog
    if (!GetSaveFileName(&ofn))
    {
        // User clicked cancel or closed the dialog (or an error occurred).
        return false;
    }

    if (!PersistTo(szFile))
        return false;

    strcpy(g_szName, szFile);

    return true;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  HandleSave
// Purpose:   
// Arguments: 
// Return:    
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool HandleSave()
{
    // If we haven't been named yet, then force saveas to get name
    if (!lstrcmpi(g_szName, ""))
        return SaveAs();

    // Create the FLD file
    return PersistTo(g_szName);
}

bool HandleNew()
{
    char       szDisplayName[MAX_PATH];
    char       szPath[MAX_PATH];
    char       szTitle[1024];
    BROWSEINFO bi;
    IMalloc    *pmalloc;

    // Close out current workspace if appropriate.  This may
    // require shutting down and restarting g_cq...
    if (g_fModified)
    {
        if (!VerifyCloseWorkspace())
            return FALSE;
    }

    LoadString(g_hInst, IDS_NEW_TITLE, szTitle, 1024);

    // Get a pointer to the shell's allocator.  We'll need it to free up the pidl that's created.
    SHGetMalloc(&pmalloc);

getname:
    // Fill in the browse info structure.
    bi.pszDisplayName = szDisplayName;          // Buffer to hold display name
    bi.hwndOwner      = g_hwndMain;             // Owner window for the dialog
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
        return FALSE;
    }
    
    // If here, then the user selected a path.  Extract it from the pidl and store it in the
    // buffer the caller specified.
    if (::SHGetPathFromIDList(pidl, szPath) == false)
    {
        // Malformed pidl, or other failure.  In any case, don't set the path
        return FALSE;
    }

    // We don't allow root drives to be dvd roots.
    if (szPath[3] == '\0')
    {
        MessageBox(g_hwndMain, "A drive root cannot be selected as the root DVD directory.",
                   "Invalid directory", MB_APPLMODAL | MB_ICONWARNING | MB_OK);
        goto getname;
    }


    // At this point, szPath contains the path the user chose.
    if (szPath[0] == '\0')
    {
        // SHGetPathFromIDList failed, or SHBrowseForFolder failed.
        return FALSE;
    }

    CloseWorkspace();        
    strcpy(g_szRootDir, szPath);
    SetRoot(g_szRootDir);
    
    SetModified();

    if (g_pvmCur == &g_vmLayer)
        SetViewMode(&g_vmExplorer);
    // Enable save and save as...
    SetMenuCmds(MF_ENABLED);

    return TRUE;
}


char g_szDlgText[MAX_PATH + 400];
char g_szDlgRoot[MAX_PATH];

LRESULT CALLBACK GetTextItemDlg(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	case WM_INITDIALOG:
        SetDlgItemText(hwnd, IDC_TEXT, g_szDlgText);
		return TRUE;
        
	case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            GetDlgItemText(hwnd, IDC_ROOT, g_szDlgRoot, MAX_PATH);
		    EndDialog(hwnd, LOWORD(wParam));
        }
		return TRUE;
	}
	return FALSE;
}
bool g_fNewRoot;

BOOL GetRootDir(char *szRootDir, sXLOHeader *pxlohdr)
{
    BOOL fTried = false;

    strcpy(szRootDir, pxlohdr->szRoot);

openroot:
    // Is the root directory accessible?
    DWORD dw = GetFileAttributes(szRootDir);
    if (dw != INVALID_FILE_ATTRIBUTES && (dw & FILE_ATTRIBUTE_DIRECTORY))
        return TRUE;

    // Root dir isn't there.  Prompt user for new root directory.
    if (fTried) 
        sprintf(g_szDlgText, "The specified root directory, '%s', is not accessible; "
                    "the drive may not be visible to this machine.  You may "
                    "enter a different location for the root directory "
                    "below.  The new directory will be stored in the XLO "
                    "file the next time you save it.", szRootDir);
    else
        sprintf(g_szDlgText, "The root directory specified in the XLO file, '%s', is "
                    "not accessible; the drive may not be visible to this "
                    "machine.  You may enter a different location for the "
                    "root directory below.  The new directory will be "
                    "stored in the XLO file the next time you save it.",
                    szRootDir);
    fTried = true;

 	if (DialogBox(g_hInst, MAKEINTRESOURCE(IDD_GETROOTDIR), g_hwndMain,
                  (DLGPROC) GetTextItemDlg) == IDOK)
    {
        // User entered a new dir and clicked 'ok'
        strcpy(szRootDir, g_szDlgRoot);
        g_fNewRoot = true;
        goto openroot;
    }

    // user clicked 'cancel'
    return FALSE;
}

bool HandleOpen()
{
    OPENFILENAME ofn;
    char szFile[MAX_PATH];
    char szFilter[1024], szTitle[1024];

    // Close out current workspace if appropriate.  This may
    // require shutting down and restarting g_cq...
    if (g_fModified)
    {
        if (!VerifyCloseWorkspace())
            return false;
    }

    LoadString(g_hInst, IDS_OPEN_FILTER, szFilter, 1024);
    char *szExt = szFilter + strlen(szFilter) + 1;
    strcpy(szExt, "*.XLO");
    szExt[6] = '\0';
    LoadString(g_hInst, IDS_OPEN_TITLE, szTitle, 1024);

    strcpy(szFile, "");

    // 1. Get the name of the FLD file to open.

    // Initialize the openfilename structure so that the GetOpenFileName function knows what
    // kind of files we want to allow.
    memset(&ofn, 0, sizeof(ofn));
    ofn.lStructSize       = sizeof(ofn);
    ofn.hwndOwner         = g_hwndMain;
    ofn.hInstance         = g_hInst;
    ofn.lpstrFilter       = szFilter;
    ofn.lpstrCustomFilter = NULL;
    ofn.nMaxCustFilter    = 0;
    ofn.nFilterIndex      = 1;
    ofn.lpstrFile         = szFile;
    ofn.nMaxFile          = MAX_PATH - 1;
    ofn.lpstrFileTitle    = ofn.lpstrFile;
    ofn.nMaxFileTitle     = ofn.nMaxFile;
    ofn.lpstrInitialDir   = "C:\\";
    ofn.nFileOffset       = 0;
    ofn.nFileExtension    = 0;
    ofn.lpstrDefExt       = "*.xlo";
    ofn.lCustData         = 0;
    ofn.lpstrTitle        = szTitle;
    ofn.Flags             = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;

    // Pop up the common file open dialog
    if (!GetOpenFileName(&ofn))
    {
        // User clicked cancel or closed the dialog (or an error occurred).
        return false;
    }

    // If here, then a file name was chosen.
    CloseWorkspace();        
    g_cq.Stop();

    // Create the XLO file
    CFile *pfile = new CFile(szFile, FILE_READ);
    if (!pfile)
        FatalError(E_OUTOFMEMORY);
    if (!pfile->IsInited())
    {
        MessageBoxResource(g_hwndMain, IDS_ERR_OPEN_TEXT,
                           IDS_ERR_OPEN_CAPTION, MB_APPLMODAL | MB_OK);
        if (pfile)
            delete pfile;
        return false;
    }

    sXLOHeader xlohdr;

    // Read the header information.
    pfile->ReadData(&xlohdr, sizeof xlohdr);

    if (memcmp(xlohdr.szSig, XLO_SIG, XLO_SIG_SIZE))
    {
        // No idea what this is...
        MessageBoxResource(g_hwndMain, IDS_ERR_INVALID_FILE_TEXT,
                           IDS_ERR_INVALID_FILE_CAPTION, MB_APPLMODAL | MB_OK);
        delete pfile;
        return false;
    }

    // Does root directory specified in the XLO file exist?  If not, allow
    // user to specify a new root directory
    g_fNewRoot = false;
    if (!GetRootDir(g_szRootDir, &xlohdr))
    {
        delete pfile;
        return false;
    }

    g_updatebox.Start("Loading Layout...");
    g_updatebox.SetStatus("Loading Files");

    if (!g_fh.CreateFrom(pfile, g_szRootDir))
    {
        delete pfile;
        return false;
    }

    delete pfile;

    
    g_updatebox.SetStatus("Populating UI elements");

    g_vmExplorer.m_treeview.Populate(g_fh.m_pfiRoot);
    g_vmExplorer.m_treeview.SetCurFolder(g_fh.m_pfiRoot);

    // When creating a new layout, we initialize the changequeue *before*
    // populating the filehierarchy; this allows us to catch changes to files
    // that occur during said populating.  Here however, we're forced to
    // do a ForceUpdate anyways since files could have changed since the
    // layout was last saved; therefore, we don't bother listening for file
    // system changes while populating the filehierarchy.
    g_cq.Init(g_szRootDir);

    // "Start" the filechange queue object.  This object will be used to
    // notify us about changes to the underlying filesystem.  
    g_cq.Start(&g_fh);

    // Force an update on the filesystem in case files changed since the
    // the layout was last saved.
    g_updatebox.SetStatus("Scanning for changes since layout was saved.");
    g_cq.ForceUpdate();

    g_updatebox.Stop();

    strcpy(g_szName, szFile);
    if (g_fNewRoot)
        SetModified();
    else
        ClearModified();
    g_fJustOpened = true;

    if (g_pvmCur == &g_vmLayer)
        SetViewMode(&g_vmExplorer);

    // Enable save and save as...
    SetMenuCmds(MF_ENABLED);
    
    return true;
}

