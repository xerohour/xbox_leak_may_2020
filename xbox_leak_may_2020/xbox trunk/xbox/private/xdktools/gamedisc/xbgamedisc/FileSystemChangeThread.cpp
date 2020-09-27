// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:      FileSystemChangeThread.cpp
// Contents:  
// Revisions: 27-Nov-2001: Created (jeffsim)
//
// UNDONE: things to test:
//  1. Delete the root dir - can't; FindFirstChangeNot locks it
//  2. Replace the root dir with a same-named file - can't; FindFirstChangeNot locks it
// A. Delete a file
// B. Delete a folder (what happens to the files?)
// C. Rename a file (should show up as a delete followed by create)
// D. Rename a folder ("   ")
// E. Save layout, exit, make changes, load layout.  NOTE: Could persist
//    "LKG local filesystem" and go through same code below
// f. Replace a dir with a same-named file (and vice versa)
// g. exit xbGameDisc immediately after changes to the filesystem (ie, before changes have a chance to
//    enqueue.  ALSO test when they'be been enqueued but not dequeued).
// h. Test across a network (ie rootdir == "\\jeffsim5\c$\files"
//
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ INCLUDE FILES ++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// "stdafx.h"       -- stdafxiled header file
#include "stdafx.h"
#include <time.h>

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ FUNCTIONS ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  FileSystemChangeThread
// Purpose:   Non-instance-specific version of the thread.  Immediately
//            calls into the correct instance.
// Arguments: pvArg     -- The changequeue that this thread 'belongs to'
// Return:    '0' for success, '-1' for failure
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
DWORD WINAPI FileSystemChangeThread(LPVOID pvArg)
{
    return ((CChangeQueue*)pvArg)->FileSystemChangeThread();
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CChangeQueue::CChangeQueue
// Purpose:   CChangeQueue constructor.  Creates the Thread.
// Arguments: None
// Return:    None
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
CChangeQueue::CChangeQueue()
{
    m_cChangeQueueReady = 0;
    m_penHead = NULL;
    m_penTail = NULL;
    m_fFailedTimeCompare = FALSE;
    m_fValidating = FALSE;
    m_fStarted = FALSE;

    m_hevtStop                = CreateEvent(NULL, FALSE, FALSE, "Stop");
    m_hevtForceUpdate         = CreateEvent(NULL, FALSE, FALSE, "ForceUpdate");
    m_hevtForceUpdateComplete = CreateEvent(NULL, FALSE, FALSE, "ForceUpdateComplete");
    m_hevtDoneUpdate          = CreateEvent(NULL, FALSE, FALSE, "m_hevtDoneUpdate");
    m_hevtFinishedCQInit      = CreateEvent(NULL, FALSE, FALSE, "g_hevtFinishedCQInit");
    m_hevtStartQueue          = CreateEvent(NULL, FALSE, FALSE, "g_hevtStartQueue");

    // undone - leaking events?
    InitializeCriticalSection(&m_csQueue);
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CChangeQueue::~CChangeQueue
// Purpose:   CChangeQueue destructor.
// Arguments: None
// Return:    None
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
CChangeQueue::~CChangeQueue()
{
    Stop();
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CChangeQueue::Init
// Purpose:   Creates (and implicitly starts) the thread that track changes
//            to the physical file system.  However, the thread itself doesn't
//            do anything other than initialize the filechange notifiers and
//            wait for 'Start'.
// Arguments: szRoot - the root dir we'll monitor
// Return:    'TRUE' if successful, 'FALSE' otherwise
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
BOOL CChangeQueue::Init(char *szRoot)
{
    strcpy(m_szRoot, szRoot);

    // Create the thread that tracks file system changes; this will initialize
    // the filechange notifiers, but won't actually do anything else...
    m_hthreadFileSystemChange = CreateThread(NULL, 0,
                                   ::FileSystemChangeThread, this, 0, NULL);
    return TRUE;
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CChangeQueue::Start
// Purpose:   Start up the Change thread that was created in 'Init()'
// Arguments: pfh - The file hierarchy we'll monitor
// Return:    'TRUE' if successful, 'FALSE' otherwise
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
BOOL CChangeQueue::Start(CFileHierarchy *pfh)
{
    // note: pfh is gauranteed not to change by the Main Thread until the
    // Change Thread is done populating its own hierarchy
    m_pfhInit = pfh;

    // Tell the filechange thread to populate itself from pfh
    SetEvent(m_hevtStartQueue);

    // Wait until the filechange thread tells us that it's down populating.
    WaitForSingleObject(m_hevtFinishedCQInit, INFINITE);
    
    m_fStarted = true;

    return TRUE;
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CChangeQueue::Stop
// Purpose:   Stops and deletes the thread that tracks changes to the
//            physical file system.
// Arguments: None
// Return:    None
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CChangeQueue::Stop()
{
    if (m_fStarted == false)
        return;

    // The user isn't allowed to call stop until all of the events are handled.
    assert(m_penHead == NULL && m_penTail == NULL);

    // If already deleted or never even created, then bail ship.
    if (m_hthreadFileSystemChange != NULL)
        return;

    // Tell the FileSystemChangeThread that we're stopped
    SetEvent(m_hevtStop);

    // undone: msgloop here?
    
    // wait for FileSystemChangeThread to exit, so that we know it won't be
    // doing any more object-specific actions.
    WaitForSingleObject(m_hthreadFileSystemChange, INFINITE);

    // At this point, the FileSystemChange thread has exited.
    DeleteObject(m_hthreadFileSystemChange);
    m_hthreadFileSystemChange = NULL;
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CChangeQueue::ForceUpdate
// Purpose:   
// Arguments: None
// Return:    None
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CChangeQueue::ForceUpdate()
{
    MSG msg;

    // Tell the FileSystemChangeThread to force an update
    SetEvent(m_hevtForceUpdate);

    // Wait until the FileSystemChangeThread has signalled that it's finished
    // We sit in the windows message loop here to avoid blocking the UI
    while (WaitForSingleObject(m_hevtForceUpdateComplete, 0) == WAIT_TIMEOUT)
    {
        if (PeekMessage(&msg, NULL,  0, 0, PM_REMOVE)) 
        {
            if (!TranslateAccelerator(g_hwndMain, g_haccel, &msg))
            {
                if (msg.message == WM_QUIT)
                {
                    // User is trying to quit the application.
                    // UNDONE: Need to wait until update is done?
                    break;
                }
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
    }    

    // Tell the thread to continue
    SetEvent(m_hevtDoneUpdate);
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CChangeQueue::RecursePopulate
// Purpose:   Recursively populates the local filesystem with the physical
//            file system object found at 'szPath'.
// Return:    
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
CFileObject *CChangeQueue::RecursePopulate(CFileObject *pfoParent, CFileObject *pfoMainThread)
{
    // Create the file object that will hold information about the object
    // called 'szPath' in the physical file system
    CFileObject *pfoNew = new CFileObject(pfoMainThread);
    pfoNew->m_pfoMainThread = pfoMainThread;
    pfoNew->m_pcq = this;
    pfoMainThread->m_pfoChangeThread = pfoNew;

    if (pfoParent)
    {
        pfoParent->m_pcq = NULL;
        pfoParent->AddChild(pfoNew);
        pfoParent->m_pcq = this;
    }
    
    CFileObject *pfoMainThreadChild = pfoMainThread->m_lpfoChildren.GetFirst();
    while (pfoMainThreadChild)
    {
        RecursePopulate(pfoNew, pfoMainThreadChild);
        pfoMainThreadChild = pfoMainThread->m_lpfoChildren.GetNext();
    }

    return pfoNew;
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  FileSystemChangeThread
// Purpose:   Thread that handles tracking changes to the underlying file
//            system.
// Arguments: pvUnused          -- Ignore.
// Returns:   '0' if successfully connected; '-1' otherwise.
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
DWORD CChangeQueue::FileSystemChangeThread()
{
    HANDLE rghListen[3];
    int nRet;

    // Listen for file change notifications and "Stop" notifications
    rghListen[0] = m_hevtStop;
    rghListen[1] = m_hevtForceUpdate;
    rghListen[2] = FindFirstChangeNotification(m_szRoot, TRUE,
                                            FILE_NOTIFY_CHANGE_FILE_NAME |
                                            FILE_NOTIFY_CHANGE_DIR_NAME |
                                            FILE_NOTIFY_CHANGE_ATTRIBUTES |
                                            FILE_NOTIFY_CHANGE_SIZE |
                                            FILE_NOTIFY_CHANGE_LAST_WRITE);

    // Now that the change notifiers are going, wait until we're *really*
    // started.  This allows us to catch changes that occur while the file
    // hierarchy is being initially populated in the main thread.
    WaitForSingleObject(m_hevtStartQueue, INFINITE);

    // Create a local copy of the current filesystem.  This will be compared
    // against the physical filesystem when changes are notified.
    // Note: If a change occurs while we are constructing this local copy,
    // the wait will fire immediately and report the changes.
    m_pfoRoot = RecursePopulate(NULL, m_pfhInit->m_pfiRoot);

    // Tell the main thread that we're through with the file hierarchy and
    // that it can continue
    SetEvent(m_hevtFinishedCQInit);

    while (true)
    {
        // Kick back and wait for a file to change or for a Stop to occur
        DWORD dw = WaitForMultipleObjects(3, rghListen, FALSE, INFINITE); 
        if (dw == WAIT_OBJECT_0)
        {
            // 'Stop()' was called on us.
            nRet = 0;
            goto done;
        }

        BOOL fForceUpdate = FALSE;
        if (dw == WAIT_OBJECT_0 + 1)
            fForceUpdate = TRUE;

        // Reset the notification so that other changes are tracked next time
        // around the loop
        if (!FindNextChangeNotification(rghListen[2]))
        {
            // We should only get here if the user deleted the root tree that
            // we're watching.  Post a notification at the front of the change
            // queue so that the main thread can handle it as soon as possible.
            // We can ignore all other messages in the queue since they're
            // obviously now meaningless.  It is up to the main thread to
            // delete the actual queue messages (we don't touch them here in case
            // the main thread is currently accessing them).
            // Meanwhile, there's nothing left for us to track here, so we're
            // outta here!
            Enqueue(EVENT_ROOTDELETED, NULL);
            m_cChangeQueueReady = 1;
            nRet = 0;
            goto done;
        }

        // It's often the case that the user is doing a number of 
        // actions to the physical filesystem in the background.  To
        // batch these actions together as much as possible, we
        // wait for a period of time after the first notification
        // to see if any other actions occur; we keep waiting until
        // either the filesystem has been stable for that period of
        // time, or a larger "force update" period of time has elapsed.
        DWORD dwPauseTime = 500; // Wait for filesystem to be stable for .5s
        time_t timeEnd = time(NULL) + 10; // After 10s, force update
        while (time(NULL) < timeEnd || fForceUpdate)
        {
            dw = WaitForMultipleObjects(3, rghListen, FALSE, dwPauseTime); 

            // Need to reset change notification after every 'wait'
            // UNDONE: It's possible that FNCN shouldn't be called if the
            //         signalled event is 'force update'.  MSDN is unclear
            //         about this.  Same with FNCN call above.
            if (!FindNextChangeNotification(rghListen[2])) 
            {
                Enqueue(EVENT_ROOTDELETED, NULL);
                m_cChangeQueueReady = 1;
                nRet = 0;
                goto done;
            }

            // Check if things were stable for the duration of the pause
            if (dw == WAIT_TIMEOUT)
                break;

            if (dw == WAIT_OBJECT_0)
            {
                // 'Stop()' was called on us.
                nRet = 0;
                goto done;
            }
            if (dw == WAIT_OBJECT_0 + 1)
                fForceUpdate = TRUE;
        }

        // If here, then a file change occurred in the physical file
        // system. Determine which file(s) changed, and add the
        // appropriate events to the change queue.
		HandleChangeNotification();

        if (fForceUpdate)
        {
            // Debugging msg 'force update' was sent to us.  signal that we're updated
            SetEvent(m_hevtForceUpdateComplete);
            
            // Don't rescan the filesystem until the main thread has finished validating the fs
            WaitForSingleObject(m_hevtDoneUpdate, INFINITE);
        }
    }

done:

    FindCloseChangeNotification(rghListen[2]);

    return nRet;
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CChangeQueue::CheckExistingFiles
// Purpose:   Compares the local file system against the physical one to
//            determine if any files have been added to the physical fs or
//            modified (size, date). Recursive.
// Arguments: pfoCur        -- Folder to examine.
// Return:    'TRUE' if an event was added, 'FALSE' otherwise
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CChangeQueue::CheckExistingFiles(CFileObject *pfoCur)
{
    /*
    Algorithm:
    For (every child file in the current filesystem dir represented by pfoCur)
		If the child file doesn't exist in the local filesystem copy
			Add 'FileAdded' event to ChangeQueue
			Add child file to local filesystem
        else if the file exists in both, but the type has changed (file<->folder)
            Remove and readd the object
		Else if the child file exists in both, but size has changed
			Add 'FileSizeChanged' event to ChangeQueue
			Update local filesystem
        if the child file object is a folder, then recurse into it
    */

    BY_HANDLE_FILE_INFORMATION bhfi;
    WIN32_FIND_DATA wfdChild;
    char sz[MAX_PATH], szPathChild[MAX_PATH];
    FILETIME ftModified;
    DWORD nFileSizeLow, nFileSizeHigh;

    // Enumerate all files in current folder and ensure they all exist in
    // pfoCur's list of children.  If any do not, then add them (and recurse
    // into them if folders)
    sprintf(sz, "%s\\*.*", pfoCur->m_szPath);
    HANDLE hff = FindFirstFile(sz, &wfdChild);
    if (hff == INVALID_HANDLE_VALUE)
    {
        // Failed to find the folder.  It must have been deleted; let
        // CheckDeletedFiles handle it
        return;
    }
    do
    {
        if (!_stricmp(wfdChild.cFileName, ".") || !_stricmp(wfdChild.cFileName, ".."))
            continue;

        sprintf(szPathChild, "%s\\%s", pfoCur->m_szPath, wfdChild.cFileName);

        HANDLE hfileChild;
        int cTriedOpen = 0;
OpenFile:
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

#if 0
#pragma message("*******************************************************************************************************************************")
#pragma message("************************  WARNING: FOLLOWING LINE ADDED FOR DEBUGGING PURPOSES ONLY AND MUST BE REMOVED ***********************")
#pragma message("*******************************************************************************************************************************")
m_fValidating = true;
#endif
        if (hfileChild == INVALID_HANDLE_VALUE)
        {
            // Couldn't open the file for reading.  Is it in use?
            DWORD dwErr = GetLastError();
            if (dwErr == ERROR_SHARING_VIOLATION)
            {
                if (cTriedOpen >= 5)
                {
                    // Already tried opening it several times and failed.
                    if (m_fValidating)
                    {
                        // Don't block UI when we're testing.
                        DebugOutput("***** FILE '%s' WAS IN USE *****\n", szPathChild);
                    }
                    else
                    {
                        // Pop a message box to the user to let them know
                        // the file is in use and we're not going anywhere
                        // until it's available.
                        sprintf(sz, "The file '%s' is in use and needs to be"
                                    " unlocked so that it can\n be added to"
                                    " the layout.  Please close all"
                                    " applications accessing the file and"
                                    " click 'OK'.", szPathChild);

                        MessageBox(NULL, sz, "File In Use",
                                 MB_ICONEXCLAMATION | MB_OK | MB_APPLMODAL);
                    }
                    goto OpenFile;
                }
                // File's in use by another process.  Try sleeping for a
                // little bit and see if the file is released
                Sleep(100);

                cTriedOpen++;
                goto OpenFile;
            }
            else
            {
                // File has been deleted.  Ignore it here; will be caught
                // by CheckDeletedFiles
                continue;
            }
        }

        // FindFirstFile and FindNextFile do not always return valid information.
        // (See MSDN) - Unfortunately MSDN is unclear on what causes it, thus
        // rendering the functions completely useless! Yay!  Call GFIBH on it
        // to ensure correct information.
        if (!GetFileInformationByHandle(hfileChild, &bhfi))
        {
            // Failed to get information about the object.  Go back and
            // try to reopen it.
            CloseHandle(hfileChild);
            goto OpenFile;
        }
        
        CloseHandle(hfileChild);
        ftModified    = bhfi.ftLastWriteTime;
        nFileSizeLow  = bhfi.nFileSizeLow;
        nFileSizeHigh = bhfi.nFileSizeHigh;
        
        // If the file isn't a child of the current object, then add it
        CFileObject *pfoChild = pfoCur->GetChildByName(wfdChild.cFileName);
        if (pfoChild == NULL)
        {
            // The file doesn't exist in the local file system, so the user
            // must have added it.  Add the object to the local file system
            pfoChild = new CFileObject(szPathChild, &wfdChild, this);
            pfoCur->AddChild(pfoChild);
        }
        else
        {
            // The child exists.
            if (wfdChild.dwFileAttributes != pfoChild->m_dwa)
            {
                // File type changed (ie file->folder or vice versa)
                // Delete the old one and readd the new one.
                RecurseRemoveChildren(pfoChild);
                pfoCur->RemoveChild(pfoChild);

                // Add the object back in (nonrecursively -- the
                // CheckExistingFiles call below will recurse into it for us).
                char sz[MAX_PATH];
                sprintf(sz, "%s\\%s", pfoCur->m_szPath, wfdChild.cFileName);
                pfoChild = new CFileObject(sz, &wfdChild, this);
                pfoCur->AddChild(pfoChild);
            }
            else if (pfoChild->m_uliSize.LowPart  != nFileSizeLow ||
                     pfoChild->m_uliSize.HighPart != nFileSizeHigh)
            {
                // File size changed
                pfoChild->m_uliSize.LowPart  = nFileSizeLow;
                pfoChild->m_uliSize.HighPart = nFileSizeHigh;
                Enqueue(EVENT_SIZECHANGED, pfoChild);
            }
            else if (CompareFileTime(&ftModified, &pfoChild->m_ftModified) != 0)
            {
                // 'Last modified' time changed
                pfoChild->m_ftModified = ftModified;
                Enqueue(EVENT_TIMECHANGED, pfoChild);
            }
        }

        // If the child is a directory, then recurse into it
        if (pfoChild->m_fIsDir)
            CheckExistingFiles(pfoChild);

    } while (FindNextFile(hff, &wfdChild));

    FindClose(hff);
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void CChangeQueue::RecurseRemoveChildren(CFileObject *pfoFolder)
{
    // Recursively remove all of the objects in pfoFolder from the local
    // file system.
    CFileObject *pfoChild;
    while ((pfoChild = pfoFolder->GetFirstChild()) != NULL)
    {
        // Recursively remove the child object's contents
        if (pfoChild->m_fIsDir)
            RecurseRemoveChildren(pfoChild);
        pfoFolder->RemoveChild(pfoChild);
    }
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void CChangeQueue::CheckDeletedFiles(CFileObject *pfoFolder)
{
    /*
	For (every file in the internal filesystem)	
		If the file doesn't exist in the current filesystem
			Add 'FileDeleted' event to ChangeQueue
			Remove file from local filesystem
    */
    CFileObject *pfoChild = pfoFolder->GetFirstChild();
    while (pfoChild)
    {
        // Does pfoChild exist in 'szPath'?
        if (GetFileAttributes(pfoChild->m_szPath) == INVALID_FILE_ATTRIBUTES)
        {
            // File doesn't exist; it's been removed from the physical fs!
            // Remove it from here as well
            if (pfoChild->m_fIsDir)
                RecurseRemoveChildren(pfoChild);
            pfoFolder->RemoveChild(pfoChild);
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

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void CChangeQueue::HandleChangeNotification()
{
    m_fEventAdded = FALSE;

    // Recursively check if any files were added to the physical fs or resized
    CheckExistingFiles(m_pfoRoot);

    // Recursively check if any files were removed from the physical fs
    CheckDeletedFiles(m_pfoRoot);
 
    if (m_fEventAdded)
    {
        m_cChangeQueueReady++;
        m_fEventAdded = FALSE;
    }
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void CChangeQueue::Enqueue(eEvent event, CFileObject *pfo)
{
    CEventNode *pen = new CEventNode(event, pfo);
    EnterCriticalSection(&m_csQueue);

    if (event == EVENT_ROOTDELETED)
    {
        // Place this message at the head of the queue so that it gets
        // handled first
        pen->m_penNext = m_penHead;
        m_penHead = pen;
        if (m_penTail == NULL)
            m_penTail = m_penHead;
    }
    else
    {
        if (m_penHead == NULL)
        {
            m_penHead = m_penTail = pen;
        }
        else
        {
            m_penTail->m_penNext = pen;
            m_penTail = pen;
        }
    }
    LeaveCriticalSection(&m_csQueue);
    
    m_fEventAdded = TRUE;
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void CChangeQueue::Dequeue(eEvent *pevent, CFileObject **ppfo)
{
    assert(m_penHead && m_penTail);
    
    EnterCriticalSection(&m_csQueue);
    *pevent = m_penHead->m_event;
    *ppfo = m_penHead->m_pfo;
    CEventNode *pen = m_penHead;
    m_penHead = m_penHead->m_penNext;
    if (m_penHead == NULL)
        m_penTail = NULL;
    
    // We delete the EventNode, but it's up to the dequeue'er to delete the
    // fileobject IFF the event is a remove.
    delete pen;

    LeaveCriticalSection(&m_csQueue);
}
