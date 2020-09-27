// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:     HandleAssert.cpp
// Contents: Contains code to handle the assert dialog box
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

// gs_szAssert       -- Assertion string passed from the Xbox.
static char *gs_szAssert;

// gs_szFile         -- String prefixing the file of the assertion in the assertion string
static char *gs_szFile = "File: ";

// gs_szLine         -- String prefixing the line of the assertion in the assertion string
static char *gs_szLine = "Line: ";

// gs_szExp         -- String prefixing the exception of the assertion in the assertion string
static char *gs_szExp  = "Expression: ";

// rgszAssert       -- Contains the text output to the assert dialog box.  We allocate a large
//                     enough per-line buffer to hold the text below and a filename.
static char rgszAssert[8][MAX_PATH + 60] = 
{
    "Assertion failed!",
    "",
    "Program: ",
    "File: ",
    "Line: ",
    "",
    "Expression: ",
    "",
};

// gs_dwThreadId     -- Id of the thread in which the assert occurred.
static DWORD gs_dwThreadId;


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ FUNCTIONS +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  AssertDlgProc
// Purpose:   Message handler for the Assert dialog. This function is automatically called by Windows
//            whenever a message needs to be sent to the dialog box (ie 'paint', 'move', etc).
// Arguments: hDlg          -- Handle to the Dialog box's window
//            message       -- The message sent to the dialog box
//            wParam        -- A message parameter.
//            lParam        -- Another message parameter
// Return:    nonzero if we processed the message
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
LRESULT CALLBACK AssertDlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    // lf, hfont, hfontPrev -- Used in generating the font for the text in the Assert dialog.
    LOGFONT lf;
    HFONT hfont, hfontPrev;

    int i;
    char szBuf[MAX_PATH], szTitle[MAX_PATH];

    // ps                   -- Standard windows practice.  Used in WM_PAINT.
	PAINTSTRUCT ps;
    HDC hdc;

    // szFile, szLine, szExp -- Point at the file in which the assertion occurred.
    char *szFile, szFile2[MAX_PATH];
    char *szLine, szLine2[256];
    char *szExp,  szExp2[256];

    // dmxbe, cr            -- Used to communicate with the Xbox
    DM_XBE dmxbe;
    CONTEXT cr;

    // rgszAssertThis       -- Local copy of the assertion text.
    static char rgszAssertThis[8][MAX_PATH + 60];

    // Handle the specified message
	switch (message)
	{
    case WM_INITDIALOG:
        MessageBeep(MB_ICONEXCLAMATION);
        SetWindowPos(hwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
        
        // set titlebar contents
        DmGetXbeInfo(NULL, &dmxbe);
        strcpy(szTitle, strrchr(dmxbe.LaunchPath, '\\') + 1);
        sprintf(szBuf, "Xbox Assertion Failed - %s [%s]", szTitle, g_szTargetXbox);
        SetWindowText(hwnd, szBuf);

        // Return that we've handled the message
        return TRUE;

    case WM_PAINT:
        // Standard windows WM_PAINT handling.
		hdc = BeginPaint(hwnd, &ps);

        // Don't draw any background in the ExtTextOut calls below.
        SetBkMode(hdc, TRANSPARENT);

        // Get the name of the asserting program from the Xbox
        DmGetXbeInfo(NULL, &dmxbe);

        // Use a local copy of the assertion text so that multiple assertions are displayed properly.
        memcpy(rgszAssertThis, rgszAssert, sizeof(rgszAssert));

        // Parse the information out of the assert string
        if (strstr(gs_szAssert, gs_szFile) == NULL)
        {
            // Not a regular assert string - must be abort
            strcpy(rgszAssertThis[0], "Debug Error!");
            strcat(rgszAssertThis[2], dmxbe.LaunchPath);
            strcpy(rgszAssertThis[3], "");
            strcpy(rgszAssertThis[4], "This application has requested the Runtime to terminate it");
            strcpy(rgszAssertThis[5], "in an unusual way.  Please contact the application's support");
            strcpy(rgszAssertThis[6], "team for more information.");
        }
        else
        {
            szFile = strstr(gs_szAssert, gs_szFile) + strlen(gs_szFile);
            szLine = strstr(gs_szAssert, gs_szLine) + strlen(gs_szLine);
            szExp  = strstr(gs_szAssert, gs_szExp) + strlen(gs_szExp);
            strncpy(szFile2, szFile, strchr(szFile, '\n') - szFile);
            strncpy(szLine2, szLine, strchr(szLine, '\n') - szLine);
            strncpy(szExp2,  szExp,  strchr(szExp,  '\n')  - szExp);
            szFile2[strchr(szFile, '\n') - szFile] = '\0';
            szLine2[strchr(szLine, '\n') - szLine] = '\0';
            szExp2[strchr(szExp, '\n')   - szExp]  = '\0';

            // Concatenate the assertion-specific information onto the assertion text.
            strcat(rgszAssertThis[2], dmxbe.LaunchPath);
            strcat(rgszAssertThis[3], szFile2);
            strcat(rgszAssertThis[4], szLine2);
            strcat(rgszAssertThis[6], szExp2);
        }
        // Create the GDI font object
        memset(&lf, 0, sizeof(lf));
        lf.lfHeight = -11;
        strcpy(lf.lfFaceName, "Arial");
        hfont = CreateFontIndirect(&lf);

        // Select the newly created font into the assertion dialog's HDC
        hfontPrev = (HFONT)SelectObject(hdc, hfont);

        // Output each line of assertion text to the assertion dialog.
        for (i = 0; i < 8; i++)
            ExtTextOut(hdc, 60, 10 + i * 13, 0, NULL, rgszAssertThis[i], strlen(rgszAssertThis[i]), NULL);

        // Select the old font back into the assertion dialog's HDC (Windows standard practice).
        SelectObject(hdc, hfontPrev);

        // We're done with our font so delete it.
        DeleteObject(hfont);

        // Notify windows that we're done painting.
        EndPaint(hwnd, &ps);

        // Return that we've handled the message
        return TRUE;

    case WM_COMMAND:
        // The user pressed either 'Reboot', 'Break', or 'Continue'

        // We communicate the choice to the Xbox through the DmSetThreadContext function, and specify
        // the desired action in the context's eax register.

        // Get the existing thread context (so that we don't write bogus ebx, ecx, etc, values.
		cr.ContextFlags = CONTEXT_INTEGER;
		DmGetThreadContext(gs_dwThreadId, &cr);

        switch(LOWORD(wParam))
        {
        case ID_REBOOT:
            // halt the offending thread
            // cr.Eax = 't';

            // Reboot the Xbox
            DmReboot(DMBOOT_WARM);
            EndDialog(hwnd, LOWORD(wParam));
            return TRUE;

        case ID_BREAK:
    		cr.Eax = 'b';
            break;

        case ID_CONTINUE:
    		cr.Eax = 'i';
            break;
        }

        // Tell the Xbox what the user chose.
        DmSetThreadContext(gs_dwThreadId, &cr);

        if (LOWORD(wParam) == ID_CONTINUE)
        {
            // Tell the Xbox to start running again
		    DmContinueThread(gs_dwThreadId, FALSE);
		    DmGo();
        }

        // We're done, so exit the dialog.
        EndDialog(hwnd, LOWORD(wParam));

        // Return that we've handled the message
        return TRUE;
    }

    // Return that we haven't handled the message
    return FALSE;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  HandleAssert
// Purpose:   Called when we've been notified of an Xbox assertion and we need to handle it.  Pops
//            up the Assert dialog box and lets it take control.
// Arguments: szAssert          -- The assert string passed from the Xbox.  Contains file, line, and
//                                 exception information.
//            dwThreadId        -- Id of the thread in which the assert occurred.
// Return:    None
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void HandleAssert(char *szAssert, DWORD dwThreadId)
{
    // Track the assert string and thread that the assert occurred in.  Note: Can't avoid global
    // variables with a dialog box...
    gs_szAssert   = szAssert;
    gs_dwThreadId = dwThreadId;

    // Open the assert dialog box - this will handle abort, retry, and fail for us.
    DialogBox(g_hInstance, (LPCTSTR)IDD_ASSERT, g_hwnd, (DLGPROC)AssertDlgProc);    
}
