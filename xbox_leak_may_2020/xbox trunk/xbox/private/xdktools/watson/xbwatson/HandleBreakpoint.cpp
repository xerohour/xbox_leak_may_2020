// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:     HandleBreakpoint.cpp
// Contents: Contains code to handle the Breakpoint dialog box
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

// gs_dwThreadId     -- Id of the thread in which the Breakpoint occurred.
static DWORD gs_dwThreadId;

// gs_pvAddress      -- Address of the exception.
static void *gs_pvAddress;


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ FUNCTIONS +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  BreakpointDlgProc
// Purpose:   Message handler for the Breakpoint dialog. This function is automatically called by
//            Windows whenever a message needs to be sent to the dialog box (ie 'paint', 'move', etc).
// Arguments: hDlg          -- Handle to the Dialog box's window
//            message       -- The message sent to the dialog box
//            wParam        -- A message parameter.
//            lParam        -- Another message parameter
// Return:    nonzero if we processed the message
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
LRESULT CALLBACK BreakpointDlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    // ps                   -- Standard windows practice.  Used in WM_PAINT.
	PAINTSTRUCT ps;
    HDC hdc;

    // hfont, hfontPrev, lf -- Variables necessary for outputing text
    HFONT   hfont, hfontPrev;
    LOGFONT lf;
    char szBuf[1024];

    // Handle the specified message
	switch (message)
	{
    case WM_INITDIALOG:
        MessageBeep(MB_ICONEXCLAMATION);
        SetWindowPos(hwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);

        // Set titlebar contents
        sprintf(szBuf, "An exception has occurred [%s]", g_szTargetXbox);
        SetWindowText(hwnd, szBuf);

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
        sprintf(szBuf, "A breakpoint was hit at address 0x%08X", (DWORD)gs_pvAddress);

        // Output the exception text
        ExtTextOut(hdc, 52, 10, 0, NULL, szBuf, strlen(szBuf), NULL);

        // Select the old font back into the assertion dialog's HDC (Windows standard practice).
        SelectObject(hdc, hfontPrev);

        // We're done with our font so delete it.
        DeleteObject(hfont);

        // Notify windows that we're done painting.
        EndPaint(hwnd, &ps);

        // Return that we've handled the message
        return TRUE;

    case WM_COMMAND:
        // The user pressed a button
        switch(LOWORD(wParam))
        {
        case ID_REBOOT:
            // Reboot the Xbox
            DmReboot(DMBOOT_WARM);
            break;

        case ID_CONTINUE:
            // Ignore the Breakpoint.  Tell the Xbox to continue running
		    DmContinueThread(gs_dwThreadId, FALSE);
		    DmGo();
            break;

        case ID_DONOTHING:
            // User doesn't want us to do anything.  They will probably hook up a debugger...
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
// Function:  HandleBreakpoint
// Purpose:   Called when we've been notified of an Xbox Breakpoint and we need to handle it.  Pops
//            up the Breakpoint dialog box and lets it take control.
// Arguments: dwThreadId        -- Id of the thread in which the Breakpoint occurred.
//            pvAddress         -- Address at which the exception occurred.
// Return:    None
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void HandleBreakpoint(DWORD dwThreadId, void *pvAddress)
{
    // Track various event-related variable.  Note: Can't avoid global variables with a dialog box...
    gs_dwThreadId = dwThreadId;
    gs_pvAddress  = pvAddress;

    // Open the Breakpoint dialog box - this will handle Ignore, Dump, and Reboot for us.
    DialogBox(g_hInstance, (LPCTSTR)IDD_BREAKPOINT, g_hwnd, (DLGPROC)BreakpointDlgProc);    
}
