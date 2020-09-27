// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:     cprogress.cpp
// Contents: UNDONE-WARN: Add file description
//
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ INCLUDE FILES +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// "stdafx.h"       -- Precompiled header file
#include "stdafx.h"


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ GLOBAL VARIABLES ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// g_progress       -- The global progress bar object.
CProgress g_progress;

static char gs_szFileName[MAX_PATH];

static ULARGE_INTEGER gs_uliFileSize;

static HANDLE g_hWakeup;
static HANDLE g_hError;

static bool g_fNewFileInfo;

HBITMAP gs_hbmpCopy[4];

int gs_iCurFileBmp;
DWORD gs_rgFileBmpDelay[] = {500, 500, 500, 1000};


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ FUNCTIONS +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void UpdateFileBmp(HWND hwndControl, int iBmp)
{
  //  char szBmp[20];
//    sprintf(szBmp, "FILECOPY%d", iBmp);
//    SetDlgItemText(hwndControl, IDC_ANIM, szBmp);
//    SetWindowText(hwndControl, szBmp);
SendMessage( hwndControl, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM) gs_hbmpCopy[iBmp] );
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CopyInProgressDlg
// Purpose:   Message handler for the 'File Copy in Progress' dialog. This function is automatically
//            called by Windows whenever a message needs to be sent to the dialog box (ie 'paint',
//            'move', etc).
// Arguments: hwnd          -- Handle to the Dialog box's window
//            message       -- The message sent to the dialog box
//            wParam        -- A message parameter.
//            lParam        -- Another message parameter
// Return:    nonzero if we processed the message
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
LRESULT CALLBACK CopyInProgressDlg(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static int s_iTimer;
    static int gs_iCurBmpFile;
    static DWORD s_dwLastAnimTime;
    DWORD dwTime;

	switch(message)
	{
	case WM_INITDIALOG:
        s_iTimer = SetTimer(hwnd, 1, 50, NULL);
        CenterDialog(hwnd);

        // Force an animation update immediately
        gs_iCurFileBmp = 0;
        s_dwLastAnimTime = GetTickCount();
        UpdateFileBmp(GetDlgItem(hwnd, IDC_ANIM), gs_iCurBmpFile);
        SetForegroundWindow(hwnd);
		return TRUE;
        
    case WM_TIMER:
        // Done?
        if (g_progress.InThread() == false)
        {
            // we're done!
            KillTimer(hwnd, s_iTimer);
            EndDialog(hwnd, 0);
            return TRUE;
        }

        // New file?
        if (g_fNewFileInfo)
        {
            char szBuf[MAX_PATH];
            ULONGLONG ull = gs_uliFileSize.QuadPart;
            if (ull == 0)
                sprintf(szBuf, "%s", gs_szFileName);
            else
                sprintf(szBuf, "%s (%s bytes)", gs_szFileName, FormatBigNumber(ull));
            SetDlgItemText(hwnd, IDC_FILENAME, szBuf);
            g_fNewFileInfo = false;
        }

        // Time to update our animation bitmap?
        dwTime = GetTickCount();

        if (dwTime > s_dwLastAnimTime + gs_rgFileBmpDelay[gs_iCurFileBmp])
        {
            gs_iCurFileBmp = (gs_iCurFileBmp + 1) % 4;
            UpdateFileBmp(GetDlgItem(hwnd, IDC_ANIM), gs_iCurBmpFile);            
        }
        break;
	}
	return FALSE;
}


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  ProgressThread
// Purpose:   Thread that handles displaying and updating the progress bar
// Arguments: pvUnused          -- Ignore.
// Returns:   '0' if successfully connected; '-1' otherwise.
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
DWORD WINAPI ProgressThread(LPVOID pvArg)
{
    DWORD dwCurOperation;

    while (true)
    {
        // Kick back and wait for something to tell us to wake up (ie a file operation beginning).
        WaitForSingleObject(g_hWakeup, INFINITE);

        // Hey, a file operation began! Wait for a bit to see if the operation is still going.
        dwCurOperation = g_progress.GetCurOperation();
        Sleep(1000);

check:
        // If the file operation is still going on, then we need to pop up the file copy dialog
        // We animate the file copy dialog until we're told to stop.
        if (g_progress.InThread() && g_progress.GetCurOperation() == dwCurOperation)
        {
            // The progress bar is still copying files in the operation that started us above.
            // Display the dialog box
            if (g_progress.ErrorSet())
            {
                // Something has specified that it's displaying it's own error dialog.  Let's
                // kick back until it's no longer set, and then we'll recheck everything.
                // UNDONE-ERR: Can do something like WaitForSingleObject(g_hError, INFINITE), but
                //             I'm a bit too tired and unsure of semaphores to try it right now.
                //             Rethink and do it right for V2.
                while (true)
                {
                    Sleep(50);
                    if (!g_progress.ErrorSet())
                        goto check;
                }
            }
            DialogBox(hInst, "COPY_IN_PROGRESS", NULL, (DLGPROC) CopyInProgressDlg);
        }
        else
        {
            // The progress bar is still copying files, BUT it's in another operation that began
            // while we were sleeping (the previous operation ended).  Just fall through and loop
            // back up at the top.  The worst case is that the user waits 2 seconds instead of
            // one to see the progress dialog.
        }

        // At this point, we're done with the current file operation, so loop back around and wait
        // for another one to begin.
    }

    return 0;
}


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CProgress::CProgress
// Purpose:   CProgress constructor
// Arguments: None
// Return:    None
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
CProgress::CProgress()
{
    m_dwCurOperation = -1;
    m_fInThread = false;

    // load our bitmaps
    gs_hbmpCopy[0] = LoadBitmap(hInst, MAKEINTRESOURCE("FILECOPY1"));
    gs_hbmpCopy[1] = LoadBitmap(hInst, TEXT("FILECOPY2"));
    gs_hbmpCopy[2] = LoadBitmap(hInst, "FILECOPY3");
    gs_hbmpCopy[3] = LoadBitmap(hInst, "FILECOPY4");

    g_hWakeup = CreateEvent(NULL, false, false, NULL);
    //g_hError  = CreateEvent(NULL, false, false, NULL);

    ClearError();

    // Create the progress bar thread
    m_hthread = CreateThread(NULL, 0, ProgressThread, this, 0, NULL);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CProgress::~CProgress
// Purpose:   CProgress destructor
// Arguments: None
// Return:    None
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
CProgress::~CProgress()
{
    for (int i = 0; i <4; i++)
        DeleteObject(gs_hbmpCopy[i]);

    CloseHandle(g_hWakeup);
    CloseHandle(g_hError);

    // Delete the progress bar thread
    CloseHandle(m_hthread);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CProgress::StartThread
// Purpose:   Starts up the progress bar thread
// Arguments: None
// Return:    Non
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CProgress::StartThread()
{
    m_dwCurOperation++;

    // Tell the progress bar thread to start waiting two seconds.  After a moment, it'll
    // wake up and look around to see if we're still copying.  If we are, then it displays the
    // progress bar dialog and starts updating it appropriately.
    SetEvent(g_hWakeup);
    ResetEvent(g_hError);
    m_fInThread = true;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CProgress::EndThread
// Purpose:   Stops the progress bar thread
// Arguments: None
// Return:    None
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CProgress::EndThread()
{
    m_fInThread = false;
    g_fNewFileInfo = false;
}

void CProgress::SetError()
{
    m_fError = true;
//    SetEvent(g_hError);
}

void CProgress::ClearError()
{
    m_fError = false;
//    ResetEvent(g_hError);
}

bool CProgress::ErrorSet()
{
    return m_fError;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CProgress::SetFileInfo
// Purpose:   Tracks the newest file info
// Arguments: 
// Return:    None
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CProgress::SetFileInfo(char *szFileName, ULARGE_INTEGER uliFileSize)
{
    // Track the new file info.  The progress bar thread will update itself automatically
    strcpy(gs_szFileName, szFileName);
    gs_uliFileSize.QuadPart = uliFileSize.QuadPart;
    g_fNewFileInfo = true;
}
