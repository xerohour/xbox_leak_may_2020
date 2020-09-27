// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:      CFileHierarchy.cpp
// Contents:  
// Revisions: 14-Dec-2001: Created (jeffsim)
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

BOOL CFileHierarchy::SetRoot(char *szFolder)
{
    WIN32_FIND_DATA wfd;
    m_cFiles = 0;
    m_cFolders = 0;
    
    // Caller must have reset the file hierarchy first
    assert(m_pfiRoot == NULL);

    HANDLE hff = FindFirstFile(szFolder, &wfd);
    if (hff == INVALID_HANDLE_VALUE)
    {
        // The directory that we want to listen to doesn't actually exist!
        // undone: Caller must handle error!
        return FALSE;
    }
    FindClose(hff);
    
    m_pfiRoot = RecursePopulate(szFolder, &wfd, 1);
    assert(m_pfiRoot); // undone: handle this error
    sprintf(m_szRootDir, szFolder);

    return TRUE;
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

BOOL CFileHierarchy::PersistTo(CFile *pfileDest)
{
    // Persist the contents of the file hierarchy to the specified file
    assert(pfileDest);
    assert(m_pfiRoot);
    return m_pfiRoot->PersistTo(pfileDest);
}

BOOL CFileHierarchy::CreateFrom(CFile *pfileDest, char *szRoot)
{
    assert(pfileDest);
    m_pfiRoot = new CFileObject(pfileDest);
    sprintf(m_szRootDir, szRoot);
    return m_pfiRoot && m_pfiRoot->m_fInited;
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

CFileObject *CFileHierarchy::RecursePopulate(char *szFolder,
                                             WIN32_FIND_DATA *pwfd,
                                             int nDepth)
{
    // Recursively populate the hierarchy with the specified folder
    char sz[MAX_PATH];
    WIN32_FIND_DATA wfd;

    CheckMessages();
    if (pwfd->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        m_cFolders++;
    else
        m_cFiles++;
    if (((m_cFolders + m_cFiles) % 27) == 0) // pick an off-# to make it look like it's updating more often
    {
        g_updatebox.SetFileCount(m_cFiles);
        g_updatebox.SetFolderCount(m_cFolders);
    }

    // Create the file object that will hold information about the object
    // called 'szPath' in the physical file system.
    CFileObject *pfi = new CFileObject(szFolder, pwfd);
    pfi->m_nDepth = nDepth;
    
    // By default, all files are included
    pfi->m_checkstate = CHECKSTATE_CHECKED;

    // If pfo is a dir, then recurse into it
    if (pwfd->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
    {
        // Find all files in the dir
        sprintf(sz, "%s\\*.*", szFolder);

        HANDLE hff = FindFirstFile(sz, &wfd);
        if (hff == INVALID_HANDLE_VALUE)
        {
            // Folder must have been deleted between the time RecursePopulate
            // was called and FindFirstFile was called.  We simply return
            // since there's nothing more to add!  The ForceUpdate call after
            // CFileHierarchy::SetRoot will ensure everything's uptodate.
            delete pfi;
            return NULL;
        }
        do
        {
            if (_stricmp(wfd.cFileName, ".") && _stricmp(wfd.cFileName, ".."))
            {
                // Found an object in the dir.  Recurse into it, and then
                // add the object to pfo's list of child objects
                sprintf(sz, "%s\\%s", szFolder, wfd.cFileName);
                pfi->AddChild(RecursePopulate(sz, &wfd, nDepth + 1));
            }        
        } while (FindNextFile(hff, &wfd));
        FindClose(hff);
    }

    // Return the file object that represents 'szName'.
    return pfi;
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

BOOL CFileHierarchy::Add(CFileObject *pfoChangeThread)
{
    // At this point, pfo is a ChangeThread fileObject; we create a new
    // FileObject for the MainThread (see the xbGameDisc technical specification
    // for a detailed description of why we do this - in short, reentrancy
    // issues and code complexity are not worth the trade off of memory usage).
    CFileObject *pfoNew = new CFileObject(pfoChangeThread);

    // Track the concordant changethread object so that we don't have to
    // search for pfoNew when file attribute changes occur
    pfoNew->m_pfoChangeThread = pfoChangeThread;
    pfoChangeThread->m_pfoMainThread = pfoNew;

    // The changethread object must have a parent object that was already
    // added to the MainThread hierarchy - thus we can simply obtain the 
    // MainThread parent object of the newly created object.
    CFileObject *pfoParent;
    pfoParent = pfoChangeThread->m_pfoOrigParent->m_pfoMainThread;

    assert(pfoParent);
    pfoParent->AddChild(pfoNew);

    return TRUE;
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

BOOL CFileHierarchy::Remove(CFileObject *pfoChangeThread)
{
    CFileObject *pfoToRemove = pfoChangeThread->m_pfoMainThread;

    // Remove pfoToRemove from this hierarchy; the change thread will handle
    // removing pfoChangeThread from its hierarchy.
    CFileObject *pfoParent = pfoToRemove->m_pfoParent;
    assert(pfoParent);
    pfoParent->RemoveChild(pfoToRemove);
    delete pfoToRemove;
    
    return TRUE;
}

void CFileHierarchy::RemoveAll()
{
    if (m_pfiRoot)
    {
        // Delete all objects in the file hierarchy
        RemoveAllChildren(m_pfiRoot);
        delete m_pfiRoot;
        m_pfiRoot = NULL;
    }
}

void CFileHierarchy::RemoveAllChildren(CFileObject *pfoFolder)
{
    CFileObject *pfoChild;
    while ((pfoChild = pfoFolder->m_lpfoChildren.GetFirst()) != NULL)
    {
        RemoveAllChildren(pfoChild);
        pfoFolder->RemoveChild(pfoChild);
        delete pfoChild;
    }
}