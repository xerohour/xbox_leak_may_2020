// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:     xbfu.cpp
// Contents: Internal implementation of 'XBFU' functionality.  Talks directly to the Xbox.d
//
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ INCLUDE FILES +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#include "stdafx.h"

// UNDONE-ERR: What about multiple Xboxs? (Affects almost all functions)

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ FUNCTIONS +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  XBFU::XBFU
// Purpose:   XBFU Constructor
// Arguments: UNDONE
// Return:    None
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
XBFU::XBFU()
{
    m_cOpenedHandles = 0;
    m_fCannotConnect = false;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  XBFU::~XBFU
// Purpose:   XBFU Destructor
// Arguments: UNDONE
// Return:    None
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
XBFU::~XBFU()
{
    // UNDONE-WARN:Check if there are any open handles left
    m_mappdmwdFindFile.clear();
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  XBFU::GetTarget
// Purpose:   UNDONE-WARN: Fill In
// Arguments: UNDONE
// Return:    'S_OK' if successful, Error code otherwise
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
HRESULT XBFU::GetTarget(LPSTR sz, UINT ui)
{
    return DmGetXboxName(sz, (LPDWORD)&ui);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  XBFU::SetTarget
// Purpose:   UNDONE-WARN: Fill In
// Arguments: UNDONE
// Return:    'S_OK' if successful, Error code otherwise
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
HRESULT XBFU::SetTarget(LPCSTR sz)
{
    return DmSetXboxName(sz);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  XBFU::IsDrive
// Purpose:   Determine if the specified drive is accessible to the user.
// Arguments: chDriveName       -- Name of the drive to check (ie 'C').
// Return:    'true' if accessible, 'false' otherwise.
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool XBFU::IsDrive(char ch)
{
    bool fRet = false;
    HRESULT hr;
    g_pxboxconn->Before();

    char rgchDrives[26];
    DWORD cDrives = 26;

    // Get the drive list from the Xbox
    hr = DmGetDriveList(rgchDrives, &cDrives);
    if (hr != XBDM_NOERR)
    {
        if (hr == XBDM_CANNOTCONNECT)
        {
            // Mark global variable so that we know that we can't currently 
            // access the named xbox.  If this is happening during initialization
            // then the caller will handle informing the user that the xbox
            // can't be found.  Ideally we would enable this across all XBFU
            // functions and all references to the functions (to allow catching
            // if an xbox goes offline post-initialization); however, the effort
            // is not worth it considering the timeframe (after lockdown) and
            // the imminent deprecation of this app in favor of shellext.
            m_fCannotConnect = true;
        }
        goto done;
    }

    // Check if the specified drive is available
    fRet = strchr(rgchDrives, ch) ? true : false;

done:

    g_pxboxconn->After();
    return fRet;
}

bool XBFU::GetDriveList(LPSTR sz, LPDWORD pcch)
{
    HRESULT hr;
    g_pxboxconn->Before();
    hr = DmGetDriveList(sz, pcch);
    g_pxboxconn->After();
    return SUCCEEDED(hr);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  XBFU::FindFirstFile
// Purpose:   Finds the first file on the Xbox with the specified file attributes.  The caller should
//            pass the returned 'Find Handle' to subsequent calls to FindNextFile and FindClose.
// Arguments: szDir             -- The directory to search
//            pdmfa             -- File attributes of the first file.
//            fTargetXbox       -- 'true' if we're searching an Xbox, 'false' if it's a PC.
//            szTargetXbox      -- Name of the target xbox to search.
// Return:    'Find Handle' if successful, INVALID_HANDLE_VALUE otherwise
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
HANDLE XBFU::FindFirstFile(LPCSTR szDir, PDM_FILE_ATTRIBUTES pdmfa, bool fTargetXbox, 
                           LPCSTR szTargetXbox)
{
    HANDLE hr = INVALID_HANDLE_VALUE;
    HRESULT hr2;
    g_pxboxconn->Before();

    if (fTargetXbox)
    {
        // Searching an Xbox...
        PDM_WALK_DIR pdmwd = NULL;

        // UNDONE-ERR: Not taking Xbox name into account.
        if ((hr2 = DmWalkDir(&pdmwd, szDir, pdmfa)) != XBDM_NOERR)
        {
            if (hr2 == XBDM_CANNOTCONNECT)
                m_fCannotConnect = true;

            // Didn't find any files.
            memset(pdmfa, 0, sizeof(DM_FILE_ATTRIBUTES));
            goto done;
        }

        m_cOpenedHandles++;
        m_mappdmwdFindFile.insert(MAPFINDFILE::value_type((HANDLE)m_cOpenedHandles, pdmwd));

	    // Return the handle to the file that we found
        hr = (HANDLE)m_cOpenedHandles;
    }
    else
    {
        // Searching the PC
        WIN32_FIND_DATA wfd;
        SetCurrentDirectory(szDir);
        HANDLE hfile = ::FindFirstFile("*.*", &wfd);
        if (hfile == INVALID_HANDLE_VALUE)
            goto done;

        if (!MyStrcmp(wfd.cFileName, ".") || !MyStrcmp(wfd.cFileName, ".."))
        {
            if (!FindNextFile(hfile, pdmfa, fTargetXbox))
            {
                FindClose(hfile, fTargetXbox);
                goto done;
            }
            hr = hfile;
            goto done;
        }
        strcpy(pdmfa->Name, wfd.cFileName);
        pdmfa->CreationTime = wfd.ftCreationTime;
        pdmfa->ChangeTime = wfd.ftLastAccessTime;
        pdmfa->SizeHigh = wfd.nFileSizeHigh;
        pdmfa->SizeLow = wfd.nFileSizeLow;
        pdmfa->Attributes = wfd.dwFileAttributes;
        hr = hfile;
    }

done:

    g_pxboxconn->After();
    return hr;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  XBFU::FindNextFile
// Purpose:   Finds the next file using the filter and directory stored in the specified Find Handle
// Arguments: hfind             -- The 'Find Handle' to search with.
//            pdmfa             -- Attributes of the next file found.
//            fTargetXbox       -- 'true' if we're searching an Xbox, 'false' if it's a PC.
// Return:    'true' if another file was found, 'false' otherwise.
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool XBFU::FindNextFile(HANDLE hfind, PDM_FILE_ATTRIBUTES pdmfa, bool fTargetXbox)
{
    bool fRet = false;
    g_pxboxconn->Before();

    if (fTargetXbox)
    {
        // Searching an Xbox...

        // Find the File handle
        MAPFINDFILE::iterator iterFile = m_mappdmwdFindFile.find(hfind);

        // Verify that the specified find handle exists
        if (iterFile == m_mappdmwdFindFile.end())    
            goto done;

        // Find the next file
        if (DmWalkDir(&(*iterFile).second, NULL, pdmfa) != XBDM_NOERR)
        {
            // Didn't find a 'next' files.
            memset(pdmfa, 0, sizeof(DM_FILE_ATTRIBUTES));
            goto done;
        }

        fRet = true;
    }
    else
    {
        // Searching the PC

        WIN32_FIND_DATA wfd;
        if (::FindNextFile(hfind, &wfd) == false)
            goto done;
        if (!MyStrcmp(wfd.cFileName, ".") || !MyStrcmp(wfd.cFileName, ".."))
        {
            fRet = FindNextFile(hfind, pdmfa, fTargetXbox);
            goto done;
        }

        strcpy(pdmfa->Name, wfd.cFileName);
        pdmfa->CreationTime = wfd.ftCreationTime;
        pdmfa->ChangeTime = wfd.ftLastAccessTime;
        pdmfa->SizeHigh = wfd.nFileSizeHigh;
        pdmfa->SizeLow = wfd.nFileSizeLow;
        pdmfa->Attributes = wfd.dwFileAttributes;

        fRet = true;
    }
done:

    g_pxboxconn->After();
    return fRet;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  XBFU::FindClose
// Purpose:   Closes the specified Find Handle
// Arguments: hfind             -- The 'Find Handle' to close.
//            fTargetXbox       -- 'true' if we're searching an Xbox, 'false' if it's a PC.
// Return:    'true' if successful, 'false' otherwise.
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool XBFU::FindClose(HANDLE hfind, bool fTargetXbox)
{
    bool fRet;
    g_pxboxconn->Before();

    if (fTargetXbox)
    {
        // Find the File handle
        MAPFINDFILE::iterator iterFile = m_mappdmwdFindFile.find(hfind);

        // Verify that the specified find handle exists
        if (iterFile == m_mappdmwdFindFile.end())    
        {
            fRet = false;
            goto done;
        }

        DmCloseDir((*iterFile).second);
    
        // Remove the handle from the list
        m_mappdmwdFindFile.erase(iterFile);

	    // Return that everything went fine.
	    fRet = true;
    }
    else
    {
        fRet = ::FindClose(hfind) ? true : false;
        SetCurrentDirectory("..");
    }

done:

    g_pxboxconn->After();
    return fRet;
}

HRESULT XBFU::GetDriveFreeSize(char *szDrive, ULARGE_INTEGER *puliSpace)
{
    HRESULT hr = E_FAIL;
    g_pxboxconn->Before();
    
    if (DmGetDiskFreeSpace(szDrive, puliSpace, NULL, NULL) != XBDM_NOERR)
        goto done;

    hr = S_OK;

done:
    g_pxboxconn->After();
    return hr;
}

char g_szDelFile[MAX_PATH];

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  XBFU::Del
// Purpose:   Deletes the specified file
// Arguments: szPathFileName    -- Complete file path and name of the file to delete
//            fIsDir            -- 'true' if the specified file is a directory.
// Return:    'true' if successful, 'false' otherwise.
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool XBFU::Del(LPCSTR szPathFileName, bool *pfQuery, HWND hWnd)
{
    HRESULT hr = E_FAIL;

    int nChoice;
	bool fIsDir;
    DM_FILE_ATTRIBUTES dmfa;

    g_pxboxconn->Before();
    
    GetFileAttributes((char*)szPathFileName, &dmfa);
	fIsDir = dmfa.Attributes & FILE_ATTRIBUTE_DIRECTORY ? true : false;
   
    if (dmfa.Attributes & FILE_ATTRIBUTE_READONLY)
    {
        // Query the user if appropriate
        if (*pfQuery)
        {
            strcpy(g_szDelFile, strrchr(szPathFileName, '\\') + 1);
            if (fIsDir)
                nChoice = DialogBox(hInst, "DELFOL", hWnd, (DLGPROC) ConfirmDeleteFolderDlg);
            else
                nChoice = DialogBox(hInst, "DELFILE", hWnd, (DLGPROC) ConfirmDeleteFileDlg);

            switch (nChoice)
            {
            case IDC_YES:
                // Yep, delete it
                break;
            case IDC_YESALL:
                // Yep delete it, and stop asking
                *pfQuery = false;
                break;

            case IDC_NOPE:
                // User doesn't want to delete this file, but wants to delete others
                hr = XBDM_NOERR;
                goto done;

            case IDC_CANCEL:
                // User wants to stop deleting
                *pfQuery = false;
                goto done;
            }
        }

        // If here, then we want to delete it.  Remove the 'readonly' attrib
        dmfa.Attributes &= ~FILE_ATTRIBUTE_READONLY;
        SetFileAttributes((char*)szPathFileName, &dmfa);
    }

    if (fIsDir)
    {
        // Recursively remove the contents of dir
        DM_FILE_ATTRIBUTES dmfa;

        // Create the search string that will return us the list of all files and directories in
        // the current directory (on the source side).
        HANDLE hfind = FindFirstFile(szPathFileName, &dmfa, true, g_TargetXbox);
        if (hfind != INVALID_HANDLE_VALUE)
        {
            do
            {
                char szFile[MAX_PATH];

                wsprintf(szFile, "%s\\%s", szPathFileName, dmfa.Name);
                if (!Del(szFile, pfQuery, hWnd))
                    goto done;
            }
            while (FindNextFile(hfind, &dmfa, true));
        }

        // Now that everything is removed from the directory, delete the directory itself below.
    }

    // When here, it's either a file or an EMPTY directory.
    hr = DmDeleteFile(szPathFileName, dmfa.Attributes & FILE_ATTRIBUTE_DIRECTORY);

done:
    
    g_pxboxconn->After();

    return hr == XBDM_NOERR;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  XBFU::Md
// Purpose:   Creates a new directory
// Arguments: szDirName         -- Name of the directory to create.
// Return:    'true' if successful, 'false' otherwise.
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool XBFU::Md(LPCSTR szDirName)
{
    g_pxboxconn->Before();
    HRESULT hr = DmMkdir(szDirName);
    
    g_pxboxconn->After();
    
    return hr == XBDM_NOERR;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  XBFU::Ren
// Purpose:   Renames the specified file.
// Arguments: szOrigName        -- File to rename (must be complete path).
//            szNewName         -- Name to rename the file to.
// Return:    'true' if successful, 'false' otherwise.
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool XBFU::Ren(LPCSTR szOrigName, LPCSTR szNewName)
{
    g_pxboxconn->Before();
    HRESULT hr = DmRenameFile(szOrigName, szNewName);
    
    g_pxboxconn->After();
    
    return hr == XBDM_NOERR;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  XBFU::Reboot
// Purpose:   Reboots the Xbox.
// Arguments: UNDONE-WARN: Unknown.  Not used anywhere.
// Return:    'S_OK' if successful, Error code otherwise
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
HRESULT XBFU::Reboot(bool f, LPCSTR sz)
{
    g_pxboxconn->Before();

    // Reboot the Xbox.
    DmReboot(DMBOOT_WARM);

    g_pxboxconn->After();

    // Return that everything went fine.
	return S_OK;
}


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  XBFU::SetFileAttributes
// Purpose:   Set file attributes for specified file
// Arguments: 
// Return:    Always return success (Raid #5430)
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
HRESULT XBFU::SetFileAttributes(char *szFileName, DM_FILE_ATTRIBUTES *pdmfa, bool fXbox)
{
    g_pxboxconn->Before();

    if (!pdmfa->Attributes)
        pdmfa->Attributes = FILE_ATTRIBUTE_NORMAL;

    if (fXbox)
    {
        if (DmSetFileAttributes(szFileName, pdmfa) != XBDM_NOERR)
            goto done;
    }
    else
    {
        ::SetFileAttributes(szFileName, pdmfa->Attributes);
    }
        

done:

    g_pxboxconn->After();
    return S_OK;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  XBFU::GetNumFiles
// Purpose:   
// Arguments: 
// Return:    
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
int XBFU::GetNumFiles(char *szRoot)
{
    g_pxboxconn->Before();

    DM_FILE_ATTRIBUTES dmfa;
    int nFiles = 1; // ACcount for 'this' file
    
    // if this is a dir, then add the subfiles
    GetFileAttributes(szRoot, &dmfa);
    if (dmfa.Attributes & FILE_ATTRIBUTE_DIRECTORY)
    {
        HANDLE hfile = FindFirstFile(szRoot, &dmfa, true);
        if (hfile != INVALID_HANDLE_VALUE)
        {
            do
            {
                if (dmfa.Attributes & FILE_ATTRIBUTE_DIRECTORY)
                {
                    char szSub[MAX_PATH];
                    wsprintf(szSub, "%s\\%s", szRoot, dmfa.Name);
                    nFiles += GetNumFiles(szSub);
                }
                else
                    nFiles += 1;
            } while (FindNextFile(hfile, &dmfa, true));
        }
    }
    g_pxboxconn->After();
    return nFiles;
}

// If the dest file is a readonly file, then remove the readonly attribute now
bool XBFU::RemoveAttribute(const char *szDestPathFile, DWORD dwAttr, bool fXbox)
{
    DM_FILE_ATTRIBUTES dmfa;
    bool fRet = false;
    g_pxboxconn->Before();
    
    if (GetFileAttributes((char*)szDestPathFile, &dmfa, fXbox) != S_OK)
        goto done;

    dmfa.Attributes &= ~dwAttr;
    if (SetFileAttributes((char*)szDestPathFile, &dmfa, fXbox) != S_OK)
        goto done;

    fRet = true;
done:
    g_pxboxconn->After();
    return fRet;
}

// If the dest file is a readonly file, then remove the readonly attribute now
bool XBFU::SetAttribute(const char *szDestPathFile, DWORD dwAttr, bool fXbox)
{
    DM_FILE_ATTRIBUTES dmfa;
    bool fRet = false;
    g_pxboxconn->Before();
    
    if (GetFileAttributes((char*)szDestPathFile, &dmfa, fXbox) != S_OK)
        goto done;

    dmfa.Attributes |= dwAttr;
    if (SetFileAttributes((char*)szDestPathFile, &dmfa, fXbox) != S_OK)
        goto done;

    fRet = true;
done:
    g_pxboxconn->After();
    return fRet;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  XBFU::GetFileAttributes
// Purpose:   Get file attributes for specified file
// Arguments: 
// Return:    
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
HRESULT XBFU::GetFileAttributes(char *szFileName, DM_FILE_ATTRIBUTES *pdmfa, bool fXbox)
{
    HRESULT hr;
    g_pxboxconn->Before();

    if (fXbox)
    {
        hr = DmGetFileAttributes(szFileName, pdmfa);
        if (hr != XBDM_NOERR)
        {
            hr = E_FAIL;
            goto done;
        }
    }
    else
    {
        WIN32_FILE_ATTRIBUTE_DATA wfad;

        GetFileAttributesEx(szFileName, GetFileExInfoStandard, &wfad);
        pdmfa->Attributes = wfad.dwFileAttributes;
        pdmfa->SizeLow = wfad.nFileSizeLow;
        pdmfa->SizeHigh = wfad.nFileSizeHigh;
        pdmfa->ChangeTime = wfad.ftLastAccessTime;
        pdmfa->CreationTime = wfad.ftCreationTime;
    }

    // NOTE: DmGetFileAttributes does not set Name...
    strcpy(pdmfa->Name, strrchr(szFileName, '\\') + 1);

    hr = S_OK;

done:

    g_pxboxconn->After();
    return hr;
}

void XBFU::DoFailMoveCopyMsg(bool fMove)
{
    char szText[1024], szCaption[200];

    g_progress.SetError();
    LoadString(hInst, IDS_FAILED_COPYMOVE_TEXT, szText, 1024);
    LoadString(hInst, IDS_FAILED_COPYMOVE_CAPTION, szCaption, 200);
    MessageBox(g_hwnd, szText, szCaption, MB_ICONEXCLAMATION | MB_OK);
}

#include <assert.h>

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  XBFU::CopyMove
// Purpose:   Copies or moves the specified file to the specified directory.
// Arguments: szSrcPathFile     -- Full path\name of the source file to copy.
//            szDestPathFile    -- Full path\name of the destination file.
//            pfQuery           -- 'true' if we should query the user on overwrite or read-only
//                                 file operations.  We set this to 'false' if the user clicks
//                                 'Yes to all' in the query dialog box.
//            fDestXbox         -- 'true' if the destination is on the Xbox.
//            fSrcXbox          -- 'false' if the destination is on the Xbox.
//            fRecursive        -- 'true' if we should recurse down into subdirectories.
// Notes:     - This function will only be called in the Xbox->Xbox and PC->XBox cases.  The
//              Xbox->PC case is handled separately through IStreams.
//            - This function is broken into 7 main parts:
//              1. Verify we CAN copy/move
//                  a. Verify sufficient harddisk space
//                  b. Handle the same-directory same-name copy/move case.
//                      - move == error dialog.  Copy == rename dest to "Copy of <source name>"
//                  c. Verify valid file names
//                      - length <= 42
//                  d. Verify the destination object is not in a child folder of the source object
//                      also, dropping folder a *onto* a.
//              2. Verify the user WANTS to do the copy/move (if *pfQuery == true)
//                  a. If the source object is read-only and this is a move, then ask if user wants
//                     to move the read-only object.
//                  b. If destination object already exists then ask if user wants to overwrite it
//                  -- If user clicks 'yes to all' in either of those, then set *pfQuery = true;
//              3. Remove destination object if it exists and is a file
//              4. Copy the object if it is a file
//              5. Copy Attributes from the source object to the dest object (readonly, etc)
//              6. Recurse into the object if it is a folder
//              7. Delete the source object if this is a move.
//
// Return:    'S_OK' if successful, Error code otherwise
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
HRESULT XBFU::CopyMoveToXbox(LPCSTR szSrcPathFile, LPCSTR szDestPathFile, bool *pfQuery, 
                             bool fDestXbox, bool fSrcXbox, bool fRecursive, bool fMove, bool *pfSkipped)
{
    DM_FILE_ATTRIBUTES dmfaDestFile, dmfaSrcFile;
    ULARGE_INTEGER     uliSourceSize, uliDestFree;
    char               szBuf[MAX_PATH], szText[1024], szCaption[200];
    char               szDestPathFinal[MAX_PATH];
    bool               fDestExists;
    HRESULT            hr;
    int                nChoice;
    
    // Initialize the shared Xbox connection and cursor state.
    g_pxboxconn->Before();
    g_progress.ClearError();

    // Verify we're not getting called in the Xbox->PC case
    assert(!(fSrcXbox == true && fDestXbox == false));

    // Verify we were passed valid parameters
    if (!szSrcPathFile || !szDestPathFile || !pfQuery)
    {
        hr = E_INVALIDARG;
        goto done;
    }

	if (pfSkipped)
		*pfSkipped = false;

    // Get the attributes of the source file.  It must exist.
    hr = GetFileAttributes((char*)szSrcPathFile, &dmfaSrcFile, fSrcXbox);
    if (hr != S_OK)
    {
        // Failed to get the source file's attributes.
        DoFailMoveCopyMsg(fMove);
        hr = E_FAIL;
        goto done;
    }

    // Get the attributes of the destination file if it exists
    if (GetFileAttributes((char*)szDestPathFile, &dmfaDestFile) == S_OK)
        fDestExists = true;
    else
        fDestExists = false;

    // Store the name of the destination file separately.  We do this since we may rename it below.
    strcpy(szDestPathFinal, szDestPathFile);


    // ============== VERIFY WE CAN COPY/MOVE =====================================================

    // Verify sufficient harddisk space
    sprintf(szBuf, "%c:\\", szDestPathFinal[1]);
    if (GetDriveFreeSize(szBuf, &uliDestFree) != S_OK)
    {
        // Failed to get the destination drive's free space.
        DoFailMoveCopyMsg(fMove);
        hr = E_FAIL;
        goto done;
    }


    if (fDestExists)
    {
        // We'll be overwriting the destination file, so we should add it's size back in to
        // the destination free space to determine the final space usage.
        ULARGE_INTEGER uliDestFileSize;
        uliDestFileSize.LowPart = dmfaDestFile.SizeLow;
        uliDestFileSize.HighPart = dmfaDestFile.SizeHigh;
        uliDestFree.QuadPart += uliDestFileSize.QuadPart;
    }

    uliSourceSize.LowPart = dmfaSrcFile.SizeLow;
    uliSourceSize.HighPart = dmfaSrcFile.SizeHigh;

    // Need to special-case the folder case since their size shows up as '0' bytes,
    if (uliSourceSize.QuadPart == 0)
        uliSourceSize.QuadPart = 1;
    
    if (uliDestFree.QuadPart < uliSourceSize.QuadPart)
    {
        // Not enough disk space!
        LoadString(hInst, IDS_NOSPACE_TEXT, szText, 1024);
        LoadString(hInst, IDS_NOSPACE_CAPTION, szCaption, 200);
        g_progress.SetError();
        MessageBox(g_hwnd, szText, szCaption, MB_ICONEXCLAMATION | MB_OK);
        hr = ERROR_DISK_FULL;
        goto done;
    }

    // Take this opportunity to update the file progress bar now that we have the size of the file
    g_progress.SetFileInfo(strrchr(szSrcPathFile, '\\') + 1, uliSourceSize);

    // This code should only come into play when a file overwrites the exact same file.  At all
	// other times, the issue should be caught by the 'overwrite' code farther below.
    if (fDestExists && !MyStrcmp(szSrcPathFile, szDestPathFile))
    {
        if (fMove)
        {
            // User is not allowed to move the same file to 'itself'.
            LoadString(hInst, IDS_CANT_MOVE_SAME_NAME_TEXT, szText, 1024);
            LoadString(hInst, IDS_CANT_MOVE_SAME_NAME_CAPTION, szCaption, 200);
            sprintf(szBuf, szText, strrchr(szSrcPathFile, '\\') + 1);
            g_progress.SetError();
            MessageBox(g_hwnd, szBuf, szCaption, MB_ICONEXCLAMATION | MB_OK);
            hr = E_FAIL;
            goto done;
        }
        else
        {
            // Copying - rename the destination object to "copy (#) of <source name>".  We loop
            // through this until we find a dest filename that doesn't exist.
            int iTry = 0;

            // UNDONE-ERR: Because of the way that listview::paste works, modifying the name of
            // a directory introduces difficulties (in short: the paste dataobject contains a complete
            // list of all files to copy; therefore if we modify the name of a file, we need to somehow
            // track that fact so that subsequent copies into the 'expected' folder actually get
            // sent to the new folder 'copy of foo'.  Once that's resolved, remove following 
            // block and everything should be fine.
            if (dmfaSrcFile.Attributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                // User is not allowed to copy the same folder to 'itself'.
                LoadString(hInst, IDS_CANT_COPY_SAME_NAME_TEXT, szText, 1024);
                LoadString(hInst, IDS_CANT_COPY_SAME_NAME_CAPTION, szCaption, 200);
                sprintf(szBuf, szText, strrchr(szSrcPathFile, '\\') + 1);
                g_progress.SetError();
                MessageBox(g_hwnd, szBuf, szCaption, MB_ICONEXCLAMATION | MB_OK);
                hr = E_FAIL;
                goto done;
            }
            
            while (true)
            {
                DM_FILE_ATTRIBUTES dmfa;

                char *pszDestFile = strrchr(szDestPathFinal, '\\') + 1;
                if (iTry == 0)
                    sprintf(pszDestFile, "Copy of %s", strrchr(szSrcPathFile, '\\') + 1);
                else
                    sprintf(pszDestFile, "Copy (%d) of %s", iTry + 1, strrchr(szSrcPathFile, '\\') + 1);

                // Does new dest file exist?
                if (GetFileAttributes((char*)szDestPathFinal, &dmfa) == E_FAIL)
                    break;

                iTry++;
            }
            fDestExists = false;
        }
    }
    
    // Verify that the destination file name is of an acceptable length <= 42 chararacters)
    if (MyStrlen(strrchr(szDestPathFinal, '\\') + 1) > 42)
    {
        LoadString(hInst, IDS_FILENAME_TOO_LONG_TEXT, szText, 1024);
        LoadString(hInst, IDS_FILENAME_TOO_LONG_CAPTION, szCaption, 200);
        sprintf(szBuf, szText, strrchr(szDestPathFinal, '\\') + 1);
        g_progress.SetError();
        MessageBox(g_hwnd, szBuf, szCaption, MB_ICONEXCLAMATION | MB_OK);
        hr = E_FAIL;
        goto done;
    }
    
    // Verify that the destination object is not in a child folder of the source object
    if (memcmp(szSrcPathFile, szDestPathFinal, MyStrlen(szSrcPathFile)) == 0)
    {
        // Destination is in a subfolder of the source!
        LoadString(hInst, IDS_CANT_MOVE_SUBFOLDER_TEXT, szText, 1024);
        LoadString(hInst, IDS_CANT_MOVE_SUBFOLDER_CAPTION, szCaption, 200);
        sprintf(szBuf, szText, strrchr(szSrcPathFile, '\\') + 1);
        g_progress.SetError();
        MessageBox(g_hwnd, szBuf, szCaption, MB_ICONEXCLAMATION | MB_OK);
        hr = E_FAIL;
        goto done;
    }


    // ==== VERIFY THE USER WANTS TO DO THE COPY/MOVE =============================================

    // If the source object is read-only and this is a move, then ask if the user wants to move
    // the read-only object
    if (*pfQuery && fMove && dmfaSrcFile.Attributes & FILE_ATTRIBUTE_READONLY)
    {
        g_progress.SetError();
        strcpy(g_szDelFile, strrchr(szSrcPathFile, '\\') + 1); 
        if (dmfaSrcFile.Attributes & FILE_ATTRIBUTE_DIRECTORY)
            nChoice = DialogBox(hInst, "CONFIRM_RO_MOVE", g_hwnd, (DLGPROC) ConfirmMoveROFolderDlg);
        else
            nChoice = DialogBox(hInst, "CONFIRM_RO_MOVE", g_hwnd, (DLGPROC) ConfirmMoveROFileDlg);

        switch (nChoice)
        {
        case IDC_YES:
            // Yep, move it
            break;
        case IDC_YESALL:
            // Yep move it, and stop asking
            *pfQuery = false;
            break;

        case IDC_NOPE:
            // User doesn't want to move this file, but wants to move others
            hr = S_OK;
            goto done;

        case IDC_CANCEL:
            // User wants to stop moving.
            *pfQuery = false;
            hr = E_FAIL;
            goto done;
        }
    }
    
    // If the destination object already exists then ask if the user wants to overwrite it
    if (fDestExists && *pfQuery)
    {
        // Ask the user if they want to overwrite it.  Use a global var for the filename
        // string since we can't pass data into a dialog.
        g_progress.SetError();
        strcpy(g_szDelFile, strrchr(szDestPathFile, '\\') + 1);
        if (dmfaDestFile.Attributes & FILE_ATTRIBUTE_DIRECTORY)
            nChoice = DialogBox(hInst, "OVERWRITE", g_hwnd, (DLGPROC) ConfirmFolderOverwriteDlg);
        else
            nChoice = DialogBox(hInst, "OVERWRITE", g_hwnd, (DLGPROC) ConfirmFileOverwriteDlg);

        switch (nChoice)
        {
        case IDC_YES:
            // Yep, move it
            break;

        case IDC_YESALL:
            // Yep move it, and stop asking
            *pfQuery = false;
            break;

        case IDC_NOPE:
            // User doesn't want to move this file, but wants to move others
            hr = S_OK;
			if (pfSkipped)
				*pfSkipped = true;
            goto done;

        case IDC_CANCEL:
            // User wants to stop moving.
            *pfQuery = false;
            hr = E_FAIL;
            goto done;
        }
    }


    // ==== REMOVE DESTINATION OBJECT =============================================================
    
    // Remove the destination object if it exists and is a file
    if (fDestExists && !(dmfaDestFile.Attributes & FILE_ATTRIBUTE_DIRECTORY))
    {
        // Remove the Read-only attribute from the file if it's set
        RemoveAttribute(szDestPathFinal, FILE_ATTRIBUTE_READONLY, fDestXbox);

        // Delete the file.
        if (DmDeleteFile(szDestPathFinal, false) != XBDM_NOERR)
        {
            // Failed to delete the file.
            DoFailMoveCopyMsg(fMove);
            hr = E_FAIL;
            goto done;
        }
    }
    

    // ==== COPY OBJECT (IF FILE), CREATE DIR (IF FOLDER) =========================================
    
    if (dmfaSrcFile.Attributes & FILE_ATTRIBUTE_DIRECTORY)
    {
        // It's a folder.  We only need to create the directory if it doesn't already exist
        if (fDestExists)
        {
            // Remove the 'hidden' attribute in case it's set
            if (!RemoveAttribute(szDestPathFinal, FILE_ATTRIBUTE_HIDDEN, fDestXbox))
            {
                DoFailMoveCopyMsg(fMove);
                hr = E_FAIL;
                goto done;
            }
        }
        else
        {
            // Create the directory
            if (!Md(szDestPathFinal))
            {
                DoFailMoveCopyMsg(fMove);
                hr = E_FAIL;
                goto done;
            }
        }
    }
    else
    {
        // It's a file.  Copy from the source

        // Clear the error since the file copy could take a looooong time
        g_progress.ClearError();

        if (fSrcXbox)
        {
            // Xbox->Xbox copy
            // Note: I can't see a way to do a localized copy on the XBox, so we instead
            //       copy the file locally to the PC, and then copy it back out to the
            //       xbox under the new name.  Could add support to XboxDbg, s'pose...

            char szTempPath[MAX_PATH];
            char szTempPathFileName[MAX_PATH];

            // Create a temporary file name for the new file
            if (GetTempPath(MAX_PATH, szTempPath) == 0)
            {
                hr = E_FAIL;
                DoFailMoveCopyMsg(fMove);
                goto done;
            }

            if (GetTempFileName(szTempPath, "xbx", 0, szTempPathFileName) == 0)
            {
                hr = E_FAIL;
                DoFailMoveCopyMsg(fMove);
                goto done;
            }

            // Now that we have a valid name for our temporary file, copy the file from the Xbox
            // to the temporary file, and then copy it back out to the Xbox under the new name
            if (DmReceiveFile(szTempPathFileName, szSrcPathFile) != XBDM_NOERR)
            {
                hr = E_FAIL;
                DoFailMoveCopyMsg(fMove);
                goto done;
            }

            if (DmSendFile(szTempPathFileName, szDestPathFinal) != XBDM_NOERR)
            {
                hr = E_FAIL;
                DoFailMoveCopyMsg(fMove);
                goto done;
            }

            // Finally, delete our temporary file
            if (!DeleteFile(szTempPathFileName))
            {
                hr = E_FAIL;
                DoFailMoveCopyMsg(fMove);
                goto done;
            }
        }
        else
        {
            // PC->Xbox copy
            if (DmSendFile(szSrcPathFile, szDestPathFinal) != XBDM_NOERR)
            {
                hr = E_FAIL;
                DoFailMoveCopyMsg(fMove);
                goto done;
            }
        }
    }

    // Copy attributes to the new object
    if (SetFileAttributes((char*)szDestPathFinal, &dmfaSrcFile) != S_OK)
    {
        hr = E_FAIL;
        DoFailMoveCopyMsg(fMove);
        goto done;
    }


    // ==== RECURSE INTO OBJECT (IF FOLDER) =======================================================
    
    if (dmfaSrcFile.Attributes & FILE_ATTRIBUTE_DIRECTORY && fRecursive)
    {
        DM_FILE_ATTRIBUTES dmfa;
        
        // Create the search string that will return us the list of all files and directories in
        // the current directory (on the source side).
        HANDLE hfind = FindFirstFile(szSrcPathFile, &dmfa, fSrcXbox);
        if (hfind != INVALID_HANDLE_VALUE)
        {
            do
            {
                char szSourceSubDir[MAX_PATH];
                char szDestSubDir[MAX_PATH];

                wsprintf(szSourceSubDir, "%s\\%s", szSrcPathFile, dmfa.Name);
                wsprintf(szDestSubDir, "%s\\%s", szDestPathFile, dmfa.Name);
                HRESULT hr = CopyMoveToXbox(szSourceSubDir, szDestSubDir, pfQuery, fDestXbox,
                                            fSrcXbox, fRecursive, fMove);
                if (hr != S_OK)
                {
                    FindClose(hfind, fSrcXbox);
                    goto done;
                }
            }
            while (FindNextFile(hfind, &dmfa, fSrcXbox));
            FindClose(hfind, fSrcXbox);
        }
    }

    
    // ==== DELETE SOURCE OBJECT IF (MOVE) ========================================================

    // UNDONE-ERR: This should actually *completely* disappear once I'm handling 'paste succeeded'
    // in the data object handling on my end (in the Xbox->Xbox and Xbox->PC cases).
    if (fMove && fRecursive)
    {
        // Remove the Read-only attribute from the file if it's set
        if (!RemoveAttribute(szSrcPathFile, FILE_ATTRIBUTE_READONLY, fSrcXbox))
        {
            DoFailMoveCopyMsg(fMove);
            hr = E_FAIL;
            goto done;
        }

        // Delete the file.
        if (fSrcXbox)
        {
            // Xbox->Xbox move.  Delete the original file from the Xbox
            if (DmDeleteFile(szSrcPathFile, dmfaSrcFile.Attributes & FILE_ATTRIBUTE_DIRECTORY) != XBDM_NOERR)
            {
                // Failed to delete the file.
                DoFailMoveCopyMsg(fMove);
                hr = E_FAIL;
                goto done;
            }
        }
        else
        {
            // PC->Xbox move.  Delete the original file from the PC
            if (dmfaSrcFile.Attributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                if (!RemoveDirectory(szSrcPathFile))
                {
                    hr = E_FAIL;
                    goto done;
                }
            }
            else
            {
                if (!DeleteFile(szSrcPathFile))
                {
                    hr = E_FAIL;
                    goto done;
                }
            }
        }
    }

    // If here, then the file was successfully moved/copied.
    hr = S_OK;

done:

    g_pxboxconn->After();
    return hr;
}
