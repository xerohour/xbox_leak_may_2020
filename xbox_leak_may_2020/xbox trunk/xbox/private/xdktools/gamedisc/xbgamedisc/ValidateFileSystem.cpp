// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:      ValidateFileSystem.cpp
// Contents:  
// Revisions: 10-Dec-2001: Created (jeffsim)
//
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ INCLUDE FILES ++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// "stdafx.h"       -- stdafxiled header file
#include "stdafx.h"


// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ FUNCTIONS ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CChangeQueue::ValidateFileSystem
// Purpose:   Compares the physical filesystem against the local copy and
//            ensures that they are the same.
// Arguments: None
// Return:    None
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
int cValid = 0;
void CChangeQueue::ValidateFileSystem()
{
    MSG msg;

    m_fValidating = TRUE;
forceupdate:
    // Tell the FileSystemChangeThread to force an update
    SetEvent(m_hevtForceUpdate);

    // Wait until the FileSystemChangeThread has signalled that it's finished
    // We sit in the windows message loop here to avoid blocking the UI
    while (WaitForSingleObject(m_hevtForceUpdateComplete, 0) == WAIT_TIMEOUT)
    {
        if (PeekMessage(&msg, g_hwndMain,  0, 0, PM_REMOVE)) 
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }    

    SetCursor(g_hcurWait);

    // Recursively compare m_pfoRoot's contents against what is on the disk;
    // they must match exactly.
    VerifyFilesExist(m_pfoRoot);
    BOOL fFailTimeComp = !VerifyNoNewFiles(m_pfoRoot);

    // Tell the FileSystemChangeThread that we're done validating, and it
    // can continue updating
    SetEvent(m_hevtDoneUpdate);

    SetCursor(g_hcurArrow);

    // The file system takes it's time updating times on directories; try a
    // second time here just in case
    if (fFailTimeComp)
        goto forceupdate;

    m_fValidating = FALSE;

    DebugOutput("Passed: %d\n", ++cValid);
}

BOOL CChangeQueue::VerifyNoNewFiles(CFileObject *pfoCur)
{
    BY_HANDLE_FILE_INFORMATION bhfi;
    WIN32_FIND_DATA wfdChild;
    char sz[MAX_PATH], szPathChild[MAX_PATH];
    FILETIME ftModified;
    DWORD nFileSizeLow, nFileSizeHigh;

    // Enumerate all files in current folder and ensure they all exist in
    // pfoCur's list of children.  If any do not, then error!
    sprintf(sz, "%s\\*.*", pfoCur->m_szPath);
    HANDLE hff = FindFirstFile(sz, &wfdChild);
    if (hff == INVALID_HANDLE_VALUE)
    {
        // Failed to find the folder.
        char sz[1000];
        sprintf(sz, "The folder '%s' was unexpectedly deleted!", pfoCur->m_szPath);
        MessageBox(g_hwndMain, sz, "Error in validate", MB_ICONEXCLAMATION | MB_OK);
        exit(-1);
    }
    do
    {
        if (!_stricmp(wfdChild.cFileName, ".") || !_stricmp(wfdChild.cFileName, ".."))
            continue;

        sprintf(szPathChild, "%s\\%s", pfoCur->m_szPath, wfdChild.cFileName);

        HANDLE hfileChild;
        if (wfdChild.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            hfileChild = CreateFile(szPathChild, GENERIC_READ,
                       0, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS , NULL);
        }
        else
        {
            hfileChild = CreateFile(szPathChild, GENERIC_READ,
                       0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        }
        if (hfileChild == INVALID_HANDLE_VALUE)
        {
            char sz[1000];
            sprintf(sz, "The object '%s' couldn't be opened! (error=0x%08x)", szPathChild, GetLastError());
            MessageBox(g_hwndMain, sz, "Error in validate", MB_ICONEXCLAMATION | MB_OK);
            exit(-1);
        }

        if (!GetFileInformationByHandle(hfileChild, &bhfi))
        {
            char sz[1000];
            sprintf(sz, "The object '%s' couldn't be opened #2! (error=0x%08x)", szPathChild, GetLastError());
            MessageBox(g_hwndMain, sz, "Error in validate", MB_ICONEXCLAMATION | MB_OK);
            exit(-1);
        }
        
        CloseHandle(hfileChild);
        ftModified    = bhfi.ftLastWriteTime;
        nFileSizeLow  = bhfi.nFileSizeLow;
        nFileSizeHigh = bhfi.nFileSizeHigh;
        
        // If the file isn't a child of the current object, then add it
        CFileObject *pfoChild = pfoCur->GetChildByName(wfdChild.cFileName);
        if (pfoChild == NULL)
        {
            // The file doesn't exist in the local file system!
            char sz[1000];
            sprintf(sz, "The object '%s' was in the physical fs but NOT the local fs!  Did you add this object while validation was running?", wfdChild.cFileName);
            MessageBox(g_hwndMain, sz, "Error in validate", MB_ICONEXCLAMATION | MB_OK);
            exit(-1);
        }
        else
        {
            // The child exists.
            if (wfdChild.dwFileAttributes != pfoChild->m_dwa)
            {
                char sz[1000];
                sprintf(sz, "The object '%s' has attributes '0x%08x' in the local fs, but attributes '0x%08x' in the physical fs!", 
                        pfoChild->m_szPath, pfoChild->m_dwa, wfdChild.dwFileAttributes);
                MessageBox(g_hwndMain, sz, "Error in validate", MB_ICONEXCLAMATION | MB_OK);
                exit(-1);
            }
            else if (pfoChild->m_uliSize.LowPart  != nFileSizeLow ||
                     pfoChild->m_uliSize.HighPart != nFileSizeHigh)
            {
                char sz[1000];
                sprintf(sz, "The object '%s' has size '%d' in the local fs but size '%d' in the physical fs!", 
                        pfoChild->m_szPath, pfoChild->m_uliSize.LowPart, nFileSizeLow);
                MessageBox(g_hwndMain, sz, "Error in validate", MB_ICONEXCLAMATION | MB_OK);
                exit(-1);
            }
            else if (CompareFileTime(&ftModified, &pfoChild->m_ftModified) != 0)
            {
                // 'Last modified' time changed.  This is not critical, and often the timestamp for
                // directories is updated a few moments later (caching, perhaps?).  In any case,
                // we jump back to try again; if it fails a second time then we hard-fail
                if (m_fFailedTimeCompare)
                {
                    char sz[1000];
                    sprintf(sz, "The object '%s' file time doesn't match!", pfoChild->m_szName);
                    MessageBox(g_hwndMain, sz, "Error in validate", MB_ICONEXCLAMATION | MB_OK);
                    exit(-1);
                }

                m_fFailedTimeCompare = true;
                FindClose(hff);
                return FALSE;
            }
        }

        // If the child is a directory, then recurse into it
        if (pfoChild->m_fIsDir)
            CheckExistingFiles(pfoChild);

    } while (FindNextFile(hff, &wfdChild));

    m_fFailedTimeCompare = false;

    FindClose(hff);
    return TRUE;
}

void CChangeQueue::VerifyFilesExist(CFileObject *pfoFolder)
{
    // For (every file in the internal filesystem)	
    //     If the file doesn't exist in the physical filesystem
    //         error!
    CFileObject *pfoChild = pfoFolder->GetFirstChild();
    while (pfoChild)
    {
        // Does pfoChild exist in 'szPath'?
        if (GetFileAttributes(pfoChild->m_szPath) == INVALID_FILE_ATTRIBUTES)
        {
            // File doesn't exist - error!
            char sz[1024];
            sprintf(sz, "Error - file '%s' was in the local filesystem but NOT in the physical filesystem!", pfoChild->m_szPath);
            MessageBox(g_hwndMain, sz, "Error in filesystem", MB_ICONEXCLAMATION | MB_OK);
            exit(-1);
        }
        else
        {
            // The file exists.  Recurse into it if it's a dir
            if (pfoChild->m_fIsDir)
                CheckDeletedFiles(pfoChild);
        }

        pfoChild = pfoFolder->GetNextChild();
    }

}