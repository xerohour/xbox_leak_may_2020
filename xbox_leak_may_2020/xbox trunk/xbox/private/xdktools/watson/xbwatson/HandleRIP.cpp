// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:     HandleRIP.cpp
// Contents: Contains code to handle the RIP dialog box
//
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ INCLUDE FILES +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// "stdafx.h"       -- Precompiled header file
#include "stdafx.h"


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ LOCAL GLOBAL VARIABLES ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// gs_dwThreadId     -- Id of the thread in which the RIP occurred.
static DWORD gs_dwThreadId;


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ GLOBAL VARIABLES ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// g_pszRIP          -- Pointer to string passed back from Xbox in the RIP. This is global because
//                      GetBreakInfo needs to access it.
char *g_pszRIP;


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ FUNCTIONS +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  RIPDlgProc
// Purpose:   Message handler for the RIP dialog. This function is automatically called by
//            Windows whenever a message needs to be sent to the dialog box (ie 'paint', 'move', etc).
// Arguments: hDlg          -- Handle to the Dialog box's window
//            message       -- The message sent to the dialog box
//            wParam        -- A message parameter.
//            lParam        -- Another message parameter
// Return:    nonzero if we processed the message
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
LRESULT CALLBACK RIPDlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static char szLogSaved[] = "(A crash dump has been saved)";
    char szBuf[1024];
    
    // s_fSaved             -- Tracks whether or not we've saved a file already.  Used in WM_PAINT.
    static bool s_fSaved = false;

    // ps                   -- Standard windows practice.  Used in WM_PAINT.
	PAINTSTRUCT ps;
    HDC hdc;

    // hfont, hfontPrev, lf -- Variables necessary for outputing text
    HFONT   hfont, hfontPrev;
    LOGFONT lf;

    CONTEXT cr;

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

        // Create the RIP string
        sprintf(szBuf, "A RIP error has occurred on the Xbox");

        // Output the RIP text
        ExtTextOut(hdc, 52, 10, 0, NULL, szBuf, strlen(szBuf), NULL);

        // Create the RIP string
        sprintf(szBuf, "The error description was: %s", g_pszRIP);

        // Output the RIP text
        ExtTextOut(hdc, 52, 25, 0, NULL, szBuf, strlen(szBuf), NULL);

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
        switch(LOWORD(wParam))
        {
        case ID_REBOOT:
            // Reboot the Xbox
            DmReboot(DMBOOT_WARM);
            break;

        case ID_DUMP:
            // Save the RIP log to disk
            if (DumpLog(hwnd, IDD_RIP, gs_dwThreadId))
            {
                // Track that we've saved a file so that WM_PAINT also displays the fact...
                s_fSaved = true;

                // Update the dialog box
                InvalidateRect(hwnd, NULL, false);
            }

            // After saving the log, we remain in the dialog box so that the user can choose to
            // reboot the machine or ignore the RIP (or, if so desired, to save the log again)
            return true;

        case ID_BREAK:
            // User wants to hit an INT 3.
            DmGetThreadContext(gs_dwThreadId, &cr);
            DmSetBreakpoint((void*)cr.Eip);
//            DmDoBreak();
            break;

        case ID_CONTINUE:
            // Ignore the RIP.  Tell the Xbox to continue running
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
// Function:  HandleRIP
// Purpose:   Called when we've been notified of an Xbox RIP and we need to handle it.  Pops
//            up the RIP dialog box and lets it take control.
// Arguments: dwThreadId        -- Id of the thread in which the RIP occurred.
//            pszRIP            -- The string passed back from the Xbox in the RIP.
// Return:    None
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void HandleRIP(DWORD dwThreadId, char *pszRIP)
{
    // Track the  thread that the RIP occurred in.  Note: Can't avoid global variables with a
    // dialog box...
    gs_dwThreadId = dwThreadId;
    g_pszRIP     = pszRIP;

    // Open the RIP dialog box - this will handle Ignore, Dump, and Reboot for us.
    DialogBox(g_hInstance, (LPCTSTR)IDD_RIP, g_hwnd, (DLGPROC)RIPDlgProc);    
}
