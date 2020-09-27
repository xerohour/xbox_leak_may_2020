// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:      About.cpp
// Contents:  
// Revisions: 4-Feb-2002: Created (jeffsim)
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

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  AboutDlgProc
// Purpose:   Message handler for About dialog. This function is automatically called by Windows
//            whenever a message needs to be sent to the dialog box (ie 'paint', 'move', etc).
// Arguments: hDlg          -- Handle to the Dialog box's window
//            message       -- The message sent to the dialog box
//            wParam        -- A message parameter.
//            lParam        -- Another message parameter
// Return:    nonzero if we processed the message
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
LRESULT CALLBACK AboutDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    char szBuf[1024], szFormat[1024];

	switch (message)
	{
	case WM_INITDIALOG:
        // Get the build number from the resource file
        LoadString(g_hInst, IDS_ABOUT, szFormat, 1024);
        sprintf(szBuf, szFormat, VER_PRODUCTBUILD);
        SetDlgItemText(hDlg, IDC_ABOUTTEXT, szBuf);
		return true;

	case WM_COMMAND:
		EndDialog(hDlg, 0);
		return false;
	}
    return false;
}

void DoAbout(HWND hwndParent)
{
    DialogBox(g_hInst, (LPCTSTR)IDD_ABOUTBOX, hwndParent, (DLGPROC)AboutDlgProc);
}