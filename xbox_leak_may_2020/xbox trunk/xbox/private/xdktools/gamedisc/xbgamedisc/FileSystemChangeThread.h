// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:      FileSystemChangeThread.h
// Contents:  
// Revisions: 27-Nov-2001: Created (jeffsim)
//
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class CFileObject;
class CFileObjectList;
class CFileHierarchy;

class CEventNode
{
public:
    CEventNode(eEvent event, CFileObject *pfo) {m_event = event; m_pfo = pfo; m_penNext = NULL;}
    CEventNode *m_penNext;
    eEvent m_event;
    CFileObject *m_pfo;
};

class CChangeQueue
{
public:
    CChangeQueue();
    ~CChangeQueue();

    BOOL Init(char *szRoot);
    BOOL Start(CFileHierarchy *pfh);
    void Stop();

    void Enqueue(eEvent event, CFileObject *pfo);
    void Dequeue(eEvent *pevent, CFileObject **ppfo);

    BOOL HasEvent() {return m_penHead != NULL; }
    CFileObject *RecursePopulate(CFileObject *pfoParent, CFileObject *pfoMainThread);

    DWORD FileSystemChangeThread();

    void CheckExistingFiles(CFileObject *pfoCur);
    void CheckDeletedFiles(CFileObject *pfoFolder);

    void ValidateFileSystem();

    void ForceUpdate();

    int m_cChangeQueueReady;
private:
    CFileObject *m_pfoRoot;
    char m_szRoot[MAX_PATH];
    CFileHierarchy *m_pfhInit;

    // debugging:
    BOOL VerifyNoNewFiles(CFileObject *pfoFolder);
    void VerifyFilesExist(CFileObject *pfoFolder);

    void HandleChangeNotification();

    void RecurseRemoveChildren(CFileObject *pfoFolder);

    CEventNode *m_penHead, *m_penTail;

    HANDLE m_hthreadFileSystemChange;
    HANDLE m_hevtStop;
    HANDLE m_hevtForceUpdate;
    HANDLE m_hevtForceUpdateComplete;
    HANDLE m_hevtDoneUpdate;
    HANDLE m_hevtFinishedCQInit;
    HANDLE m_hevtStartQueue;

    BOOL   m_fStarted;
    BOOL   m_fFailedTimeCompare;
    BOOL   m_fEventAdded;
    CRITICAL_SECTION m_csQueue;
    BOOL   m_fValidating;
};

extern HANDLE g_hthreadFileSystemChange;
