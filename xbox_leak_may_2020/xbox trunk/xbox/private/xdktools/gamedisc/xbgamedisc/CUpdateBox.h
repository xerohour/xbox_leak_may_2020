// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:      CUpdateBox.h
// Contents:  
// Revisions: 15-Jan-2002: Created (jeffsim)
//
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class CUpdateBox
{
public:
    CUpdateBox();
    ~CUpdateBox();
    void Start(char *szTitle);
    void Stop();

    void SetStatus(char *szStatus);
    void SetFileCount(int nFiles);
    void SetFolderCount(int nFolders);

    DWORD UpdateBoxThread();
    LRESULT CALLBACK UpdateBoxDialog(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
    HWND m_hwnd;

private:
    HANDLE m_hthread;
    HANDLE m_hevtStopUpdate;
    CRITICAL_SECTION m_csStatusText;
    bool m_fNewStatus;
    bool m_fNewObjCount;
    char m_szStatusText[255];
    char m_szFiles[20];
    char m_szFolders[20];
    char m_szTitle[1024];
    int m_nPeriods;
};

extern CUpdateBox *g_pupdateboxCur;
