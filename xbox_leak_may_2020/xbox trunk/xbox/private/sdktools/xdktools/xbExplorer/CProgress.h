// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:     cprogress.h
// Contents: 
//
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ CLASS DECLARATIONS ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class CProgress
{
public:
    CProgress();
    ~CProgress();

    void StartThread();
    void EndThread();
    void SetFileInfo(char *szFileName, ULARGE_INTEGER uliFileSize);
    DWORD GetCurOperation() {return m_dwCurOperation; }
    bool InThread() {return m_fInThread; }
    void SetError();
    void ClearError();
    bool ErrorSet();

private:
    HANDLE m_hthread;
    bool m_fInThread;
    bool m_fError;

    DWORD m_dwCurOperation;
};


extern CProgress g_progress;