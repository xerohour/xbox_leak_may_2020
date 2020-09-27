// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:     HandleException.cpp
// Contents: Contains code to handle the exception dialog box
//
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ INCLUDE FILES +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// "stdafx.h"        -- Precompiled header file
#include "stdafx.h"


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ LOCAL GLOBAL VARIABLES ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// gs_dwThreadId     -- Id of the thread in which the exception occurred.
static DWORD gs_dwThreadId;

// gs_pvAddress      -- Address of the exception.
void *gs_pvAddress;


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ GLOBAL VARIABLES ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// g_dwExceptionCode -- Exception code of the exception which occurred.
DWORD g_dwExceptionCode;


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ FUNCTIONS +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  ExceptionDlgProc
// Purpose:   Message handler for the Exception dialog. This function is automatically called by
//            Windows whenever a message needs to be sent to the dialog box (ie 'paint', 'move', etc).
// Arguments: hDlg          -- Handle to the Dialog box's window
//            message       -- The message sent to the dialog box
//            wParam        -- A message parameter.
//            lParam        -- Another message parameter
// Return:    nonzero if we processed the message
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
LRESULT CALLBACK ExceptionDlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static char szLogSaved[] = "(A crash dump has been saved)";
    char szBuf[MAX_PATH + 30];
    char szBuf1[256];
    char szBuf2[256];

    // s_fSaved             -- Tracks whether or not we've saved a file already.  Used in WM_PAINT.
    static bool s_fSaved = false;

    // ps                   -- Standard windows practice.  Used in WM_PAINT.
	PAINTSTRUCT ps;
    HDC hdc;

    // hfont, hfontPrev, lf -- Variables necessary for outputing text
    HFONT   hfont, hfontPrev;
    LOGFONT lf;

    // Handle the specified message
	switch (message)
	{
    case WM_INITDIALOG:
        MessageBeep(MB_ICONEXCLAMATION);
        SetWindowPos(hwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);

        // Set titlebar contents
        sprintf(szBuf, "An exception has occurred [%s]", g_szTargetXbox);
        SetWindowText(hwnd, szBuf);

        // Mark that we haven't yet saved a file
        s_fSaved = false;

        // Return that we've handled the message
        return true;

    case WM_PAINT:
        // Standard windows WM_PAINT handling.
		hdc = BeginPaint(hwnd, &ps);

        // Don't draw any background in the ExtTextOut calls below.
        SetBkMode(hdc, TRANSPARENT);

        // Create the GDI font object
        memset(&lf, 0, sizeof(lf));
        lf.lfHeight = -11;
        strcpy(lf.lfFaceName, "Arial");
        hfont = CreateFontIndirect(&lf);

        // Select the newly created font into the assertion dialog's HDC
        hfontPrev = (HFONT)SelectObject(hdc, hfont);

        // Create the exception string
        switch(g_dwExceptionCode)
        {
        case 0x80000003:
            // Breakpoint
            sprintf(szBuf1, "A breakpoint exception (0x%08X) has been reached in", g_dwExceptionCode);
            sprintf(szBuf2, "the application at location 0x%08X.", (DWORD)gs_pvAddress);
            break;

        case 0xC0000005:
            // Access Violation
            sprintf(szBuf1, "The instruction at address 0x%08X referenced memory", (DWORD)gs_pvAddress);
            if (g_fWriteException)
                sprintf(szBuf2, "at address 0x%08X.  The memory could not be written.", g_pvAVAddress);
            else
                sprintf(szBuf2, "at address 0x%08X.  The memory could not be read.", g_pvAVAddress);
            break;

        default:
            // Other
            sprintf(szBuf1, "An exception (0x%08X) occurred in the application", g_dwExceptionCode);
            sprintf(szBuf2, "at location 0x%08X", (DWORD)gs_pvAddress);
            break;
        }

        // Output the exception text
        ExtTextOut(hdc, 60, 10, 0, NULL, szBuf1, strlen(szBuf1), NULL);
        ExtTextOut(hdc, 60, 24, 0, NULL, szBuf2, strlen(szBuf2), NULL);

        // If we've already saved a file, then also output that text to the dialog
        if (s_fSaved)
            ExtTextOut(hdc, 103, 50, 0, NULL, szLogSaved, strlen(szLogSaved), NULL);

        // Select the old font back into the assertion dialog's HDC (Windows standard practice).
        SelectObject(hdc, hfontPrev);

        // We're done with our font so delete it.
        DeleteObject(hfont);

        // Notify windows that we're done painting.
        EndPaint(hwnd, &ps);

        // Return that we've handled the message
        return TRUE;

    case WM_COMMAND:
        // The user pressed a button.
        switch(LOWORD(wParam))
        {
        case ID_REBOOT:
            // Reboot the Xbox
            DmReboot(DMBOOT_WARM);
            break;

        case ID_DUMP:
            // Save the exception log to disk
            if (DumpLog(hwnd, IDD_EXCEPTION, gs_dwThreadId))
            {
                // Track that we've saved a file so that WM_PAINT also displays the fact...
                s_fSaved = true;

                // Update the dialog box
                InvalidateRect(hwnd, NULL, false);
            }

            // After saving the log, we remain in the dialog box so that the user can choose to
            // reboot the machine or ignore the exception (or, if so desired, to save the log again)
            return true;

        case ID_CONTINUE:
            // User watns to continue.  Tell the Xbox to start running again
		    DmContinueThread(gs_dwThreadId, FALSE);
		    DmGo();
            break;
        }

        // We're done, so exit the dialog.
        EndDialog(hwnd, LOWORD(wParam));

        // Return that we've handled the message
        return true;
    }

    // Return that we haven't handled the message
    return false;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  HandleException
// Purpose:   Called when we've been notified of an Xbox exception and we need to handle it.  Pops
//            up the Exception dialog box and lets it take control.
// Arguments: dwThreadId        -- Id of the thread in which the exception occurred.
//            dwCode            -- Exception code.
//            pvAddress         -- Address at which the exception occurred.
// Return:    None
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void HandleException(DWORD dwThreadId, DWORD dwCode, void *pvAddress)
{
    // Track the  thread that the exception occurred in.  Note: Can't avoid global variables with a
    // dialog box...
    gs_dwThreadId     = dwThreadId;
    gs_pvAddress      = pvAddress;

    // The exception code is file-global since GetBreakInfo needs to get it.
    g_dwExceptionCode = dwCode;

    // Open the exception dialog box - this will handle Ignore, Dump, and Reboot for us.
    DialogBox(g_hInstance, (LPCTSTR)IDD_EXCEPTION, g_hwnd, (DLGPROC)ExceptionDlgProc);    
}
