// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:      cworkspace.cpp
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
#include <commdlg.h>
#include <Shlobj.h>
#include <xboxverp.h>


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ GLOBAL VARIABLES ++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// gs_fPushedToScratch  -- 'true' if files were pushed to the scratch window
//                         during an 'AddFiles' call.
static bool gs_fPushedToScratch;
extern bool g_fResizedVolDesc;

time_t g_timeLastSaved = 0;

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ FUNCTIONS +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CWorkspace::CWorkspace
// Purpose:   
// Arguments: None
// Return:    None
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
CWorkspace::CWorkspace(CWindow_Main *pwindow)
{
    m_pwindow = pwindow;
    m_pdvd = new CDVD;
    if (!m_pdvd)
        FatalError(E_OUTOFMEMORY);

    strcpy(m_szName, "");
    m_fModified = false;
//    UpdateWindowCaption();

    for (int i = 0; i < MAX_UNDO_DEPTH; i++)
        m_rgpfileSnapshot[i] = NULL;
    
    SetInited(S_OK);
}

bool CWorkspace::Init()
{
    // Clear ourselves out.
    Reset();
    return true;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CWorkspace::~CWorkspace
// Purpose:   
// Arguments: None
// Return:    None
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
CWorkspace::~CWorkspace()
{
    delete m_pdvd;

    if (m_rgpfileSnapshot[0])
    {
        delete m_rgpfileSnapshot[0];
        m_rgpfileSnapshot[0] = NULL;
    }
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CWorkspace::Close
// Purpose:   
// Arguments: 
// Return:    
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool CWorkspace::Close()
{
    if (m_fModified)
    {
        // Verify with the user that they want to exit even though the
        // workspace has been modified
        if (MessageBoxResource(m_pwindow->m_hwnd, IDS_WS_MODIFIED_TEXT,
                               IDS_WS_MODIFIED_CAPTION,
                               MB_YESNO | MB_ICONQUESTION | MB_APPLMODAL) == IDYES)
        {
            return true;
        }
        return false;
    }

    return true;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CWorkspace::Reset
// Purpose:   
// Arguments: 
// Return:    
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CWorkspace::Reset()
{
    m_pwindow->m_rgplv[0]->Clear();
    m_pwindow->m_rgplv[1]->Clear();
    m_pwindow->m_rgplv[2]->Clear();
    // Reset the virtual dvd object.  This will empty its contents and fill it only with the
    // default security placeholders
    m_pdvd->Reset();
    
    // Clear the scratch area
    m_pwindow->m_pscratch->Reset();

    // Mark that we're in a completely clean (unmodified) state.
    m_fCleanSlate = true;

    // Update the views that display the workspace's information
    UpdateViews();
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CWorkspace::Open
// Purpose:   
// Arguments: 
// Return:    
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool CWorkspace::Open()
{
    OPENFILENAME ofn;
    char szFile[MAX_PATH];
    char szFilter[1024], szTitle[1024];

    LoadString(g_hinst, IDS_OPEN_FILTER, szFilter, 1024);
    char *szExt = szFilter + strlen(szFilter) + 1;
    strcpy(szExt, "*.FLD");
    szExt[6] = '\0';
    LoadString(g_hinst, IDS_OPEN_TITLE, szTitle, 1024);

    strcpy(szFile, "");

    // 1. Get the name of the FLD file to open.

    // Initialize the openfilename structure so that the GetOpenFileName function knows what
    // kind of files we want to allow.
    memset(&ofn, 0, sizeof(ofn));
    ofn.lStructSize       = sizeof(ofn);
    ofn.hwndOwner         = m_pwindow->m_hwnd;
    ofn.hInstance         = m_pwindow->m_hinst;
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
    ofn.lpstrDefExt       = "*.fld";
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

    // Create the FLD file
    CFile *pfile = new CFile(szFile, FILE_READ);
    if (!pfile)
        FatalError(E_OUTOFMEMORY);
    if (!pfile->IsInited())
    {
        MessageBoxResource(m_pwindow->m_hwnd, IDS_ERR_OPEN_TEXT,
                           IDS_ERR_OPEN_CAPTION, MB_APPLMODAL | MB_OK);
        if (pfile)
            delete pfile;
        return false;
    }

    DWORD dwMagicNumber;
    BYTE byMajorVersion, byMinorVersion;

    // Read the header information.
    pfile->ReadDWORD(&dwMagicNumber);

    // Is it an old FLD file?  If so, we load it like normal, but force
    // modified flag immediately (so that it will be saved out in new format
    // before emulation or premastering).
    if (dwMagicNumber == FLDFILE_MAGIC_NUMBER)
    {
        // This code will eventually deprecate away...
        // Verify we understand the version of the FLD file
        bool fVersionKnown = true;
        pfile->ReadBYTE(&byMajorVersion);
        pfile->ReadBYTE(&byMinorVersion);
        if (byMajorVersion != FLDFILE_MAJOR_VERSION || byMinorVersion != FLDFILE_MINOR_VERSION)
        {
            MessageBoxResource(m_pwindow->m_hwnd, IDS_ERR_INVALID_FILE_VER_TEXT,
                               IDS_ERR_INVALID_FILE_VER_CAPTION, MB_APPLMODAL | MB_OK);
            delete pfile;
            return false;
        }

        SetModified();
    }
    else
    {
        sFLDHeader fldhdr;
        
        // Read rest of sig
        memcpy(fldhdr.szSig, &dwMagicNumber, 4);
        pfile->ReadData(fldhdr.szSig + 4, (sizeof fldhdr) - 4);

        if (memcmp(fldhdr.szSig, FLD_SIG, FLD_SIG_SIZE))
        {
            // No idea what this is...
            MessageBoxResource(m_pwindow->m_hwnd, IDS_ERR_INVALID_FILE_TEXT,
                               IDS_ERR_INVALID_FILE_CAPTION, MB_APPLMODAL | MB_OK);
            delete pfile;
            return false;
        }
        g_timeLastSaved = fldhdr.timeSaved;

        // If here, it's a newer FLD file format.
        ClearModified();
    }

    g_fResizedVolDesc = false;

    if (!m_pdvd->CreateFrom(pfile))
    {
        delete pfile;
        return false;
    }

    m_pwindow->m_pscratch->Reset();
    m_pwindow->m_pscratch->CreateFrom(pfile);

    delete pfile;

    // Tell the DVD to refresh its list of security placeholders.  Tell it NOT
    // to validate the placeholders (we'll do it separately since we want to
    // special case it in the 'open' scenario).
    m_pdvd->RefreshPlaceholders(false);

    // Check if the placeholders are valid -- if not, then this was an older
    // FLD file with invalid placeholder generation.  We need to warn the user
    // and relayout the files now.  (pass false to specify that we don't want
    // the validate function to inform the user of the situation).
    if (!m_pdvd->ValidatePlaceholders(false))
    {
        // Yep, bogus.  Need to special case it.
        MessageBox(m_pwindow->m_hwnd, "An error in a previous version of xbLayout "
                   "caused invalid placeholder\nlocations to be generated.  As a "
                   "result, this FLD file needs to be laid\nout again.  It will now "
                   "be recreated.  We will maintain your original file\nordering, "
                   "however some files may be forced to the unplaced files window.",
                   "Invalid Placeholder Encountered", MB_ICONEXCLAMATION | MB_OK);

        try
        {
            // Create a new layout.  This will generate new placeholders
            bool fAddedToUnplaced = false;
            CDVD *pdvdNew = new CDVD;
            pdvdNew->Reset();

            // Copy the objects from m_pdvd to pdvdNew
            for (int nLayer = 0; nLayer <= 1; nLayer++)
            {
                // Start at the outside of the disc, and work inwards.
                CObject *poCur = m_pdvd->m_rgpolLayer[nLayer]->GetOutside();
                while (poCur)
                {
                    CObject *poNext = m_pdvd->m_rgpolLayer[nLayer]->GetNextInner();
                    if ((poCur->GetType() == OBJ_FOLDER || poCur->GetType() == OBJ_FILE ||
                        poCur->GetType() == OBJ_GROUP) && !poCur->m_pog)
                    {
                        poCur->RemoveFromList();
                        if (!pdvdNew->Insert(pdvdNew->m_rgpolLayer[nLayer], poCur, NULL))
                        {
                            // Couldn't fit on the layer -- add to unplaced files
                            m_pwindow->m_pscratch->AddObject(poCur);
                            if (poCur->m_pog)
                                poCur->m_pog->RemoveGroupedObject(poCur);
                            fAddedToUnplaced = true;
                        }
                    }
                    poCur = poNext;
                }
            }
            delete m_pdvd;
            m_pdvd = pdvdNew;
            m_pdvd->RefreshRelationships();

            if (fAddedToUnplaced && !m_pwindow->m_pscratch->m_fVisible)
            {
                m_pwindow->m_pscratch->ToggleView();
                MessageBox(m_pwindow->m_hwnd, "Some files could not be automatically"
                           " placed and have been moved\nto the unplaced files window."
                           "  Please move them to the appropriate positions.",
                           "Files Moved to Unplaced Files Window", MB_ICONWARNING | MB_OK);
            }
            SetModified();

            // Tell the DVD to refresh its list of security placeholders.
            m_pdvd->RefreshPlaceholders();
        }
        catch(...)
        {
            // unexpected error
            MessageBox(m_pwindow->m_hwnd, "The files could not be relaid out; please"
                       "recreate the layout from scratch.", "Cannot relayout files", MB_ICONEXCLAMATION | MB_OK);
            return false;
        }
    }
    else
    {
        // We changed the internal structure of the volume descriptor to be two
        // sectors in size (instead of one).  It's possible that the FLD that we
        // just opened was created in the older xbLayout, and that a file was sitting
        // in sector 33.  In that case we need to move the file to the scratch
        // area and warn the user.  We determine this by (a) finding the volume
        // descriptor, and (b) looking at the LSN of the file immediately following
        // it.  If it's 33, then we yank it.
        // Loop in case the user created a file called "volume descriptor".
        CObj_VolDesc *povd;
        do {
           povd = (CObj_VolDesc*)m_pdvd->m_rgpolLayer[0]->FindObjectByFullFileName(
                                                            "\\Volume Descriptor");
        } while (povd && povd->m_dwLSN != 32);
        assert(povd);

        // Get the object following the volume descriptor
        CObject *pobj = povd->m_poOuter;
        assert(pobj);
        if (pobj->m_dwLSN == 33)
        {
            // We need to yank the object
            m_pdvd->m_rgpolLayer[0]->Remove(pobj);
            m_pwindow->m_pscratch->AddObject(pobj);
            MessageBox(m_pwindow->m_hwnd, "A modification to xbLayout has caused " 
                       "the volume descriptor to grow slightly -- as a result, a "
                       "file has been pushed from layer 0 to the scratch window.  "
                       "The file was previously located at LSN 33 on layer 0 -- "
                       "you will need to determine the new location for the file.",
                       "File moved to scratch window", MB_ICONEXCLAMATION | MB_OK);
            SetModified();
        }
    
        // Track that we were modified if we resized the volume descriptor
        if (g_fResizedVolDesc)
            SetModified();
    }

    // After we've added all of the files, tell the DVD to refresh the folder
    // directory entry structures
    m_pdvd->RefreshDirectoryEntries(this, false);

    strcpy(m_szName, szFile);
    UpdateWindowCaption();

    // Check if any of the files have been modified since the layout was last
    // saved.
    m_pwindow->CheckModifiedFiles();
    
    return true;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CWorkspace::New
// Purpose:   
// Arguments: 
// Return:    
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool CWorkspace::New()
{
    char       szDisplayName[MAX_PATH];
    char       szPath[MAX_PATH];
    char       szTitle[1024];
    BROWSEINFO bi;
    bool       fRet = false;
    IMalloc    *pmalloc;

    LoadString(g_hinst, IDS_NEW_TITLE, szTitle, 1024);

    // Get a pointer to the shell's allocator.  We'll need it to free up the pidl that's created.
    SHGetMalloc(&pmalloc);

getname:
    // Fill in the browse info structure.
    bi.pszDisplayName = szDisplayName;          // Buffer to hold display name
    bi.hwndOwner      = m_pwindow->m_hwnd;      // Owner window for the dialog
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

    // We don't allow root drives to be dvd roots.
    if (szPath[3] == '\0')
    {
        MessageBox(m_pwindow->m_hwnd, "A drive root cannot be selected as the root DVD directory.",
                   "Invalid directory", MB_APPLMODAL | MB_ICONWARNING | MB_OK);
        goto getname;
    }


    // At this point, szPath contains the path the user chose.
    if (szPath[0] == '\0')
    {
        // SHGetPathFromIDList failed, or SHBrowseForFolder failed.
        goto done;
    }

    // Clear ourselves out.
    Reset();

    SetDVDRoot(szPath, true);
    
    // Tell the DVD to refresh its list of security placeholders.
    m_pdvd->RefreshPlaceholders();

    // After we've added all of the files, tell the DVD to refresh the folder
    // directory entry structures
    m_pdvd->RefreshDirectoryEntries(this, false);

    m_fModified = false;
    ClearModified();

    strcpy(m_szName, "");

    // If here, then we were successful
    fRet = true;

done:

    UpdateWindowCaption();
    return fRet;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CWorkspace::UpdateWindowCaption
// Purpose:   
// Arguments: 
// Return:    None
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CWorkspace::UpdateWindowCaption()
{
    char sz[MAX_PATH + 100];
    LoadString(g_hinst, IDS_WINDOW_CAPTION, sz, MAX_PATH);

    if (lstrcmpi(m_szName, ""))
    {
        strcat (sz, " - ");
        strcat (sz, m_szName);
    }

    if (m_pwindow->m_nCurLayerView == 0)
        strcat(sz, " [Layer 0]");
    else if (m_pwindow->m_nCurLayerView == 1)
        strcat(sz, " [Layer 1]");
    else
        strcat(sz, " [Layer 0 & 1]");
    if (m_fModified)
        strcat(sz, " *");
    m_pwindow->SetCaption(sz);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CWorkspace::Save
// Purpose:   
// Arguments: 
// Return:    
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool CWorkspace::Save()
{
    // If we haven't been named yet, then force saveas to get name
    if (!lstrcmpi(m_szName, ""))
        return SaveAs();

    // Create the FLD file
    return PersistTo(m_szName);
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CWorkspace::PersistTo
// Purpose:   
// Arguments: 
// Return:    
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool CWorkspace::PersistTo(char *szFLDName)
{
    sFLDHeader fldhdr;
    int nMajor, nMinor, nBuild, nQFE;

    CFile *pfile = new CFile(szFLDName, FILE_WRITE);
    if (!pfile)
        FatalError(E_OUTOFMEMORY);
    if (!pfile->IsInited())
    {
        MessageBoxResource(m_pwindow->m_hwnd, IDS_ERR_SAVE_TEXT,
                           IDS_ERR_SAVE_CAPTION, MB_APPLMODAL | MB_OK);
        if (pfile)
            delete pfile;
        return false;
    }

    SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_WAIT)));

    m_pdvd->ValidatePlaceholders();

    // Write out the header information

    // Keep global track of last time we saved so that we can verify the
    // FST/BIN files match the saved FLD file.
    g_timeLastSaved = time(NULL);

    memcpy(fldhdr.szSig, FLD_SIG, FLD_SIG_SIZE);
    fldhdr.timeSaved = g_timeLastSaved;
    sscanf(VER_PRODUCTVERSION_STR, "%d.%d.%d.%d", &nMajor, &nMinor, &nBuild,
                                                  &nQFE);
    fldhdr.version.wMajor = (WORD)nMajor;
    fldhdr.version.wMinor = (WORD)nMinor;
    fldhdr.version.wBuild = (WORD)nBuild;
    fldhdr.version.wQFE   = (WORD)nQFE;

    pfile->WriteData(&fldhdr, sizeof fldhdr);

    m_pdvd->PersistTo(pfile);
    m_pwindow->m_pscratch->PersistTo(pfile);
    delete pfile;

    ClearModified();
    UpdateWindowCaption();

    SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW)));

    return true;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CWorkspace::SaveAs
// Purpose:   
// Arguments: 
// Return:    
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool CWorkspace::SaveAs()
{
    // Get the name of the FLD file to save to.
    // Create the FLD file
    OPENFILENAME ofn;
    char szFile[MAX_PATH];
    char szFilter[1024], szTitle[1024];

    LoadString(g_hinst, IDS_OPEN_FILTER, szFilter, 1024);
    char *szExt = szFilter + strlen(szFilter) + 1;
    strcpy(szExt, "*.FLD");
    szExt[6] = '\0';
    LoadString(g_hinst, IDS_SAVEAS_TITLE, szTitle, 1024);

    strcpy(szFile, "");

    // 1. Get the name of the FLD file to open.

    // Initialize the openfilename structure so that the GetOpenFileName function knows what
    // kind of files we want to allow.
    memset(&ofn, 0, sizeof(ofn));
    ofn.lStructSize       = sizeof(ofn);
    ofn.hwndOwner         = m_pwindow->m_hwnd;
    ofn.hInstance         = m_pwindow->m_hinst;
    ofn.lpstrFilter       = szFilter;
    ofn.lpstrCustomFilter = NULL;
    ofn.nFilterIndex      = 1;
    ofn.lpstrFile         = szFile;
    ofn.nMaxFile          = MAX_PATH - 1;
    ofn.lpstrFileTitle    = ofn.lpstrFile;
    ofn.nMaxFileTitle     = ofn.nMaxFile;
    ofn.lpstrInitialDir   = "C:\\";
    ofn.lpstrDefExt       = "*.fld";
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

    strcpy(m_szName, szFile);

    return true;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CWorkspace::SetDVDRoot
// Purpose:   Called after the user has specified the new DVD root & the workspace has already been
//            Reset.  Populates the workspace with the files at the specified root.
// Arguments: 
// Return:    'true' if successful, 'false' otherwise
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool CWorkspace::SetDVDRoot(char *szRoot, bool fRecursive)
{
    // Must have reset before coming in here
    assert(m_fCleanSlate);

    // Save the path to the new dvd root
    strcpy(g_szRootFolderName, szRoot);
    
    SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_WAIT)));

    // Add the files in the specified root to our list of objects.
    gs_fPushedToScratch = false;
    m_fDefaultXbePresent = false;
    if (!AddFiles(szRoot, fRecursive))
    {
        SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW)));
        return false;
    }
    
    if (!m_fDefaultXbePresent)
    {
        // No default.xbe present! warn user
        MessageBox(m_pwindow->m_hwnd, "The specified directory does not contain " \
                   "default.xbe.\nDefault.xbe MUST be present or the application " \
                   "will fail to execute.", "Default.xbe not present",
                   MB_ICONWARNING | MB_OK);
    }

    if (gs_fPushedToScratch)
    {
        // Some files didn't fit, so they were pushed to the scratch window.  Warn
        // the user of this fact now.

        // Pop up the scratch window.
        if (!m_pwindow->m_pscratch->m_fVisible)
            SendMessage(m_pwindow->m_hwnd, WM_COMMAND, ID_VIEW_SCRATCHAREA, 0);
        
        MessageBoxResource(m_pwindow->m_hwnd, IDS_ERR_TOO_MANY_FILES_TEXT,
                           IDS_ERR_TOO_MANY_FILES_CAPTION, MB_APPLMODAL | MB_OK);
    }

    // setup the folder->child relationships.  We didn't do this during
    // AddFiles because of the way that folders are enumerated...
    m_pdvd->RefreshRelationships();    

    // After we've added all of the files, tell the DVD to refresh the folder
    // directory entry structures
    m_pdvd->RefreshDirectoryEntries(this, false);

    SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW)));

    m_fModified = false;

    // Update the views that display the workspace's information
    UpdateViews();

    return true;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CWorkspace::AddFiles
// Purpose:   Adds the files in the specified folder to the workspace.
// Arguments: 
// Return:    'true' if successful, 'false' otherwise
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool CWorkspace::AddFiles(char *szFolder, bool fRecursive)
{
    WIN32_FIND_DATA wfd;
    char szSearch[MAX_PATH];
    bool fRet;
    CObject *poNew;

    // Add all of the files in the folder 'szFolder'.  We don't do a straight recursion because
    // we need to add the folder itself after all of it's contents have been enumerated, but 
    // *before* any subdirectories are recursed into.  ie, Breadth-first, post insertion.  This
    // will create the default optimal object layout on the dvd.

    m_pdvd->ValidatePlaceholders();

    // Don't add the "_amc" directory. This is a support directory that we added
    // for layout purposes.
    if (!lstrcmpi(strrchr(szFolder, '\\') + 1, "_amc"))
        return true;

    // Create the search string
    sprintf(szSearch, "%s\\*.*", szFolder);

   
    // FIRST PASS -- ENUMERATE ALL CHILDREN, ADD ALL FILES, IGNORE FOLDERS


    // Find all files in the specified folder
    HANDLE hff = FindFirstFile(szSearch, &wfd);
    if (hff == INVALID_HANDLE_VALUE)
        return false;

    bool fFoundFile = false;

    do
    {
        if (!(wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
        {
            // Found a file.
            if (!lstrcmpi(wfd.cFileName, "default.xbe"))
                m_fDefaultXbePresent = true;

            poNew = new CObj_File(szFolder, wfd.cFileName, wfd.nFileSizeHigh, wfd.nFileSizeLow);
            if (!poNew)
                FatalError(E_OUTOFMEMORY);

            // Place the new object on the DVD.  Determine the new object's LSN automatically.
            if (!m_pdvd->InsertAtEnd(poNew))
            {
                gs_fPushedToScratch = true;
                m_pwindow->m_pscratch->AddObject(poNew);
            }

            // If this was the first file found, then make sure all of it's directory elements
            // are also added
            if (!fFoundFile)
            {
                fFoundFile = true;

                // Walk up the directory structure for the file, adding each element as we go
                char szCurPath[MAX_PATH];
                char szParentPath[MAX_PATH];
                strcpy(szCurPath, szFolder);
                strcpy(szParentPath, szFolder);
                *(strrchr(szParentPath, '\\')) = '\0';

                while (true)
                {
                    if (m_pdvd->FileExists(szCurPath))
                        break;

                    if (lstrcmpi(szCurPath, g_szRootFolderName) < 0)
                        break;

                    char *szName = szCurPath + strlen(szParentPath) + 1;
                    CObj_Folder *pofNew = new CObj_Folder(szParentPath, szName);
                    if (!pofNew)
                        FatalError(E_OUTOFMEMORY);

                    // Place the new folder object on the DVD.  Determine
                    // the new object's LSN automatically. (This will place
                    // it immediately inside the file we just placed).
                    if (!m_pdvd->InsertAtEnd(pofNew))
                    {
                        gs_fPushedToScratch = true;
                        m_pwindow->m_pscratch->AddObject(pofNew);
                    }

                    // Remove the deepest dir from the path
                    *(strrchr(szCurPath, '\\')) = '\0';
                    if (strrchr(szParentPath, '\\') == 0)
                        break;
                    *(strrchr(szParentPath, '\\')) = '\0';
                }
            }
        }
    } while (FindNextFile(hff, &wfd));
    FindClose(hff);

    // If no files were found in this folder, then explicitly add the empty
    // folder now.
    if (fFoundFile == false)
    {
        char szCurPath[MAX_PATH];
        char szParentPath[MAX_PATH];
        strcpy(szCurPath, szFolder);
        strcpy(szParentPath, szFolder);
        *(strrchr(szParentPath, '\\')) = '\0';
        char *szName = szCurPath + strlen(szParentPath) + 1;
        CObj_Folder *pofNew = new CObj_Folder(szParentPath, szName);
        if (!pofNew)
            FatalError(E_OUTOFMEMORY);

        // Place the new folder object on the DVD.  Determine
        // the new object's LSN automatically. (This will place
        // it immediately inside the file we just placed).
        if (!m_pdvd->InsertAtEnd(pofNew))
        {
            gs_fPushedToScratch = true;
            m_pwindow->m_pscratch->AddObject(pofNew);
        }
    }

    // SECOND PASS - ENUMERATE ALL CHILD FOLDERS AND RECURSE INTO THEM
    
    if (fRecursive)
    {
        hff = FindFirstFile(szSearch, &wfd);
        if (hff == INVALID_HANDLE_VALUE)
            return false;

        do
        {
            if (lstrcmpi(wfd.cFileName, ".") && lstrcmpi(wfd.cFileName, ".."))
            {
                if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                {
                    sprintf(szSearch, "%s\\%s", szFolder, wfd.cFileName);
                    if (!AddFiles(szSearch, fRecursive))
                    {
                        fRet = false;
                        goto done;
                    }
                }
            }
        } while (FindNextFile(hff, &wfd));
    }

    fRet = true;
done:
    
    FindClose(hff);

    return fRet;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CWorkspace::UpdateViews
// Purpose:   Update the views onto our data.
// Arguments: None
// Return:    None
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CWorkspace::UpdateViews()
{
    // Update the display of the DVD's data
    if (m_pwindow->m_rgplv[LV_LAYER0]->m_fModified || m_pdvd->m_rgpolLayer[0]->m_fModified)
        m_pdvd->PopulateToList(0, m_pwindow->m_rgplv[LV_LAYER0]);
    if (m_pwindow->m_rgplv[LV_LAYER1]->m_fModified || m_pdvd->m_rgpolLayer[1]->m_fModified)
        m_pdvd->PopulateToList(1, m_pwindow->m_rgplv[LV_LAYER1]);

    m_pwindow->m_rgplv[LV_LAYER0]->m_fModified = false;
    m_pwindow->m_rgplv[LV_LAYER1]->m_fModified = false;
    m_pdvd->m_rgpolLayer[0]->m_fModified = false;
    m_pdvd->m_rgpolLayer[1]->m_fModified = false;

    // Update the scratch area
    InvalidateRect(m_pwindow->m_pscratch->m_plistview->GetHwnd(), NULL, TRUE);
    
    m_pwindow->m_rgplv[LV_LAYER0]->Sort();
    m_pwindow->m_rgplv[LV_LAYER1]->Sort();

    m_pwindow->UpdateStatusBar();
}

int CWorkspace::GetNumFiles(int iLayer)
{
    return m_pdvd->GetNumFiles(iLayer);
}

int CWorkspace::GetNumEmptySectors(int iLayer)
{
    return m_pdvd->GetNumEmptySectors(iLayer);
}

int CWorkspace::GetNumUsedSectors(int iLayer)
{
    return m_pdvd->GetNumUsedSectors(iLayer);
}

bool CWorkspace::AddObjectsToLayer(CObjList *pol, int nLayer, CObject *pobjDropAt)
{
    return m_pdvd->InsertObjectList(pol, nLayer, pobjDropAt);
}

bool CWorkspace::RemoveObjectsFromLayer(CObjList *pol, int nLayer)
{
    return m_pdvd->RemoveObjectsFromLayer(pol, nLayer);
}

bool CWorkspace::RemoveObjectFromLayer(CObject *pobj, int nLayer)
{
    return m_pdvd->RemoveObjectFromLayer(pobj, nLayer);
}

// Remove all of the objects which belong to the folder, and add them to the
// list of objects to move.
bool CWorkspace::RemoveChildObjects(CObj_Folder* pof, CObjList *pol)
{
    return m_pdvd->RemoveChildObjects(pof, pol);
}

bool CWorkspace::CompactLayer(int nLayer)
{
    return m_pdvd->CompactLayer(nLayer);
}

bool CWorkspace::QueueSnapspot()
{
    // Create a snapshot of the workspace's current state and push it onto the
    // snapshot queue.
    // FUTURE: Not pushing onto a stack currently because we don't support undo/redo.

    m_pdvd->ValidatePlaceholders();

    // Use the same code that is used for saving to disk; however, we are instead
    // saving to a memory-mapped file.
    if (m_rgpfileSnapshot[0])
    {
        delete m_rgpfileSnapshot[0];
        m_rgpfileSnapshot[0] = NULL;
    }

    m_rgpfileSnapshot[0] = new CFile("snapshot.fld", FILE_MEMORY | FILE_WRITE);
    if (!m_rgpfileSnapshot[0])
        FatalError(E_OUTOFMEMORY);
    m_pdvd->PersistTo(m_rgpfileSnapshot[0]);
    m_pwindow->m_pscratch->PersistTo(m_rgpfileSnapshot[0]);
    return true;
}

bool CWorkspace::DequeueSnapspot()
{
    // Remove the 'latest' snapshot from the snapshot queue and set the workspace's
    // current state to match it.

    // FUTURE: Not using a queue yet since we don't support undo/redo.  Because
    //         of this, just grab the latest snapshot
    SendMessage(m_pwindow->m_rgplv[LV_LAYER0]->GetHwnd(),   WM_SETREDRAW, 0, 0);
    SendMessage(m_pwindow->m_rgplv[LV_LAYER1]->GetHwnd(),   WM_SETREDRAW, 0, 0);
    SendMessage(m_pwindow->m_rgplv[LV_SCRATCH]->GetHwnd(),  WM_SETREDRAW, 0, 0);

    Reset();
    m_rgpfileSnapshot[0]->SetPos(0);
    m_pdvd->CreateFrom(m_rgpfileSnapshot[0]);
    m_pwindow->m_pscratch->CreateFrom(m_rgpfileSnapshot[0]);
    m_pdvd->RefreshPlaceholders();
    UpdateViews();

    SendMessage(m_pwindow->m_rgplv[LV_LAYER0]->GetHwnd(),   WM_SETREDRAW, 1, 0);
    SendMessage(m_pwindow->m_rgplv[LV_LAYER1]->GetHwnd(),   WM_SETREDRAW, 1, 0);
    SendMessage(m_pwindow->m_rgplv[LV_SCRATCH]->GetHwnd(),  WM_SETREDRAW, 1, 0);
    return true;
}

bool CWorkspace::HandleDrawItem(WPARAM wparam, LPARAM lparam)
{
    CListView *plvDraw = (CListView*)wparam;
    return plvDraw->HandleDrawItem((LPDRAWITEMSTRUCT)lparam);
}

bool CWorkspace::CreateBIN()
{
    char szBINName[MAX_PATH];
    sprintf(szBINName, "%s\\_amc",g_szRootFolderName);
    CreateDirectory(szBINName, NULL);
    
    sprintf(szBINName, "%s\\_amc\\gdfs.bin", g_szRootFolderName);

    CFile *pfile = new CFile(szBINName, FILE_WRITE);
    if (!pfile)
        FatalError(E_OUTOFMEMORY);
    if (!pfile->IsInited())
    {
        MessageBoxResource(m_pwindow->m_hwnd, IDS_ERR_BINFILE, IDS_ERR_CANTCREATEFILE, MB_APPLMODAL | MB_OK);
        if (pfile)
            delete pfile;
        return false;
    }
    m_pdvd->PersistBIN(this, pfile);

    pfile->Close();
    delete pfile;

    return true;
}

bool CWorkspace::CreateFST()
{
    char szFSTName[MAX_PATH], szBINPath[MAX_PATH], szBINName[MAX_PATH];
    sprintf(szFSTName, "%s\\_amc",g_szRootFolderName);
    CreateDirectory(szFSTName, NULL);
    
    sprintf(szBINPath, "%s\\_amc",g_szRootFolderName);
    strcpy(szBINName, "gdfs.bin");

    sprintf(szFSTName, "%s\\_amc\\%s.fst", g_szRootFolderName, strrchr(g_szRootFolderName, '\\')+1);

    CFile *pfile = new CFile(szFSTName, FILE_WRITE);
    if (!pfile)
        FatalError(E_OUTOFMEMORY);
    if (!pfile->IsInited())
    {
        MessageBoxResource(m_pwindow->m_hwnd, IDS_ERR_FSTFILE, IDS_ERR_CANTCREATEFILE, MB_APPLMODAL | MB_OK);
        if (pfile)
            delete pfile;
        return false;
    }

    m_pdvd->PersistFST(szBINPath, szBINName, pfile);

    pfile->Close();
    delete pfile;

    return true;
}

void CWorkspace::SetModified()
{
    m_fModified = true;
//    HMENU hmenu= GetMenu(m_pwindow->m_hwnd);
    //EnableMenuItem(hmenu, ID_FILE_SAVE, MF_ENABLED);
}
void CWorkspace::ClearModified()
{
    m_fModified = false;
//    HMENU hmenu= GetMenu(m_pwindow->m_hwnd);
  //  EnableMenuItem(hmenu, ID_FILE_SAVE, MF_GRAYED);
}
