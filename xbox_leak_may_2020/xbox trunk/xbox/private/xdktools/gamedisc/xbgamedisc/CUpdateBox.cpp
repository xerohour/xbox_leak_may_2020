// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:      CUpdateBox.cpp
// Contents:  
// Revisions: 15-Jan-2002: Created (jeffsim)
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
// Function:  UpdateBoxThread
// Purpose:   Non-instance-specific version of the thread.  Immediately
//            calls into the correct instance.
// Arguments: pvArg     -- The CUpdateBox that this thread 'belongs to'
// Return:    '0' for success, '-1' for failure
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
DWORD WINAPI UpdateBoxThread(LPVOID pvArg)
{
    return ((CUpdateBox*)pvArg)->UpdateBoxThread();
}

CUpdateBox::CUpdateBox()
{
    m_fNewStatus = false;
    m_fNewObjCount = false;
    InitializeCriticalSection(&m_csStatusText);

    strcpy(m_szTitle, "");
    strcpy(m_szStatusText, "");
    SetFileCount(-1);
    SetFolderCount(-1);
    
    m_hthread  = NULL;
    m_hevtStopUpdate = CreateEvent(NULL, FALSE, FALSE, "StopUpdate");
}

CUpdateBox::~CUpdateBox()
{
    if (m_hthread)
        Stop();
    DeleteObject(m_hevtStopUpdate);
}

CUpdateBox *g_pupdateboxCur = NULL;

void CUpdateBox::Start(char *szTitle)
{
    strcpy(m_szTitle, szTitle);
    m_nPeriods = 0;
    m_fNewStatus = false;
    strcpy(m_szStatusText, "");
    SetFileCount(-1);
    SetFolderCount(-1);

    // WARNING: This does not allow multiple UpdateBoxes to appear
    // simultaneously.  I do not need that functionality here...
    assert(g_pupdateboxCur == NULL);
    g_pupdateboxCur = this;

    // Spawn the thread that displays the dialog box
    m_hthread = CreateThread(NULL, 0, ::UpdateBoxThread, this, 0, NULL);

}
extern BOOL g_fQuit;

void CUpdateBox::Stop()
{
    // Tell the dialog box thread to stop and go away
    SetEvent(m_hevtStopUpdate);

    // Wait for the dialog box thread to die
    MSG msg;
    while (WaitForSingleObject(m_hthread, 0) == WAIT_TIMEOUT)
    {
        if (PeekMessage(&msg, NULL,  0, 0, PM_REMOVE)) 
        {

            if (!TranslateAccelerator(g_hwndMain, g_haccel, &msg))
            {
                if (msg.message == WM_QUIT)
                {
                    // User is trying to quit the application.
                    g_fQuit = true;
                    break;
                }
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
    }    

    // At this point, the FileSystemChange thread has exited.  undone: need to delete?
    DeleteObject(m_hthread);
    m_hthread = NULL;
    
    g_pupdateboxCur = NULL;
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  UpdateBoxDialog
// Return:    nonzero if we processed the message
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
LRESULT CALLBACK UpdateBoxDialog(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    return g_pupdateboxCur->UpdateBoxDialog(hwnd, message, wParam, lParam);
}

LRESULT CALLBACK CUpdateBox::UpdateBoxDialog(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static int s_iTimer;
    static DWORD s_dwLastPeriod;
    DWORD dwTime;

	switch(message)
	{
	case WM_INITDIALOG:
        m_hwnd = hwnd;

        SetWindowText(hwnd, m_szTitle);
        s_iTimer = SetTimer(hwnd, 1, 50, NULL);
        SetDlgItemText(hwnd, IDC_STATUS, "");
        SetDlgItemText(hwnd, IDC_PROGRESS, "");
        CenterDialog(hwnd);
        SetForegroundWindow(hwnd);
		return 1;

    case WM_TIMER:
        if (WaitForSingleObject(m_hevtStopUpdate, 0) == WAIT_OBJECT_0)
        {
            // we're done!
            KillTimer(hwnd, s_iTimer);
            EndDialog(hwnd, 0);
            return 1;
        }

        // New text?
        if (m_fNewStatus)
        {
            EnterCriticalSection(&m_csStatusText);
            SetDlgItemText(hwnd, IDC_STATUS, m_szStatusText);
            LeaveCriticalSection(&m_csStatusText);
            s_dwLastPeriod = GetTickCount();
            m_fNewStatus = false;
        }

        if (m_fNewObjCount)
        {
            EnterCriticalSection(&m_csStatusText);
            SetDlgItemText(hwnd, IDC_FILES, m_szFiles);
            SetDlgItemText(hwnd, IDC_FOLDERS, m_szFolders);
            LeaveCriticalSection(&m_csStatusText);
            m_fNewObjCount = false;
        }

        // Time to add a period?
        dwTime = GetTickCount();

        if (dwTime > s_dwLastPeriod + 1000)
        {
            char szText[100];
            if (m_nPeriods > 10)
            {
                strcpy(szText, m_szStatusText);
                m_nPeriods = 0;
            }
            else
            {
                GetDlgItemText(hwnd, IDC_STATUS, szText, 100);
                strcat(szText, ". ");
                m_nPeriods++;
            }
            SetDlgItemText(hwnd, IDC_STATUS, szText);
            s_dwLastPeriod = dwTime;
        }
        break;
	}
	return FALSE;
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  UpdateBoxThread
// Purpose:   Display a modal dialog box with update contents
// Arguments: None
// Returns:   '0' if successfully connected; '-1' otherwise.
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
DWORD CUpdateBox::UpdateBoxThread()
{
    DialogBox(g_hInst, MAKEINTRESOURCE(IDD_UPDATEBOXDLG), g_hwndMain, (DLGPROC) ::UpdateBoxDialog);
    return 0;
}

void CUpdateBox::SetStatus(char *szStatus)
{
    EnterCriticalSection(&m_csStatusText);
    sprintf (m_szStatusText, szStatus);
    LeaveCriticalSection(&m_csStatusText);

    sprintf(m_szFiles, "");
    sprintf(m_szFolders, "");
    m_fNewStatus = true;
}

void CUpdateBox::SetFileCount(int nFiles)
{
    EnterCriticalSection(&m_csStatusText);
    if (nFiles == -1)
        sprintf(m_szFiles, "Files: ");
    else
        sprintf(m_szFiles, "Files: %d", nFiles);
    LeaveCriticalSection(&m_csStatusText);
    m_fNewObjCount = true;
}

void CUpdateBox::SetFolderCount(int nFolders)
{
    EnterCriticalSection(&m_csStatusText);
    if (nFolders == -1)
        sprintf(m_szFolders, "Folders: ");
    else
        sprintf(m_szFolders, "Folders: %d", nFolders);
    LeaveCriticalSection(&m_csStatusText);
    m_fNewObjCount = true;
}
