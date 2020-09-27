// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:     DlgProcs.cpp
// Contents: Contains Dialog procedures for various dialogs 
//
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ INCLUDE FILES +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// "stdafx.h"       -- Precompiled header file
#include "stdafx.h"


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ FUNCTIONS +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void PromoteWindow(HWND hwnd)
{
    SetForegroundWindow(hwnd);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  AboutDlgProc
// Purpose:   Message handler for About dialog. This function is automatically called by Windows
//            whenever a message needs to be sent to the dialog box (ie 'paint', 'move', etc).
// Arguments: hwnd          -- Handle to the Dialog box's window
//            message       -- The message sent to the dialog box
//            wParam        -- A message parameter.
//            lParam        -- Another message parameter
// Return:    nonzero if we processed the message
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
LRESULT CALLBACK AboutDlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	case WM_INITDIALOG:
        g_progress.SetError();
		return TRUE;
        
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDOK:
            // User clicked the 'OK' button; shut down the dialog box.
			EndDialog(hwnd, TRUE);
            g_progress.ClearError();
			return TRUE;
        }
	}
	return FALSE;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  ConfirmDeleteFolderDlg
// Purpose:   Message handler for the 'Confirm folder delete' dialog. This function is
//            automatically called by Windows whenever a message needs to be sent to the dialog box
//            (ie 'paint', 'move', etc).
// Arguments: hwnd          -- Handle to the Dialog box's window
//            message       -- The message sent to the dialog box
//            wParam        -- A message parameter.
//            lParam        -- Another message parameter
// Return:    nonzero if we processed the message
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
LRESULT CALLBACK ConfirmDeleteFolderDlg(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    TCHAR szBuf[MAX_PATH];
    TCHAR szBuf2[MAX_PATH];

	switch(message)
	{
	case WM_INITDIALOG:
        g_progress.SetError();
        PromoteWindow(hwnd);
        LoadString(hInst, IDS_CONFIRM_RO_FOLDER, szBuf, MAX_PATH);
        wsprintf(szBuf2, szBuf, g_szDelFile);
        SetDlgItemText(hwnd, IDC_FOL_TEXT, szBuf2);
		return TRUE;
        
	case WM_COMMAND:
		EndDialog(hwnd, LOWORD(wParam));
        g_progress.ClearError();
		return TRUE;
	}
	return FALSE;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  ConfirmDeleteFileDlg
// Purpose:   Message handler for the 'Confirm file delete' dialog. This function is automatically
//            called by Windows whenever a message needs to be sent to the dialog box (ie 'paint',
//            'move', etc).
// Arguments: hwnd          -- Handle to the Dialog box's window
//            message       -- The message sent to the dialog box
//            wParam        -- A message parameter.
//            lParam        -- Another message parameter
// Return:    nonzero if we processed the message
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
LRESULT CALLBACK ConfirmDeleteFileDlg(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    TCHAR szBuf[MAX_PATH];
    TCHAR szBuf2[MAX_PATH];

	switch(message)
	{
	case WM_INITDIALOG:
        g_progress.SetError();
        PromoteWindow(hwnd);
        LoadString(hInst, IDS_CONFIRM_RO_FILE, szBuf, MAX_PATH);
        wsprintf(szBuf2, szBuf, g_szDelFile);
        SetDlgItemText(hwnd, IDC_FILE_TEXT, szBuf2);
		return TRUE;
        
	case WM_COMMAND:
        g_progress.ClearError();
		EndDialog(hwnd, LOWORD(wParam));
		return TRUE;
	}
	return FALSE;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  ConfirmFileOverwriteDlg
// Purpose:   Message handler for the 'Confirm file overwrite' dialog. This function is automatically
//            called by Windows whenever a message needs to be sent to the dialog box (ie 'paint',
//            'move', etc).
// Arguments: hwnd          -- Handle to the Dialog box's window
//            message       -- The message sent to the dialog box
//            wParam        -- A message parameter.
//            lParam        -- Another message parameter
// Return:    nonzero if we processed the message
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
LRESULT CALLBACK ConfirmFileOverwriteDlg(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    TCHAR szBuf[MAX_PATH];
    TCHAR szBuf2[MAX_PATH];

	switch(message)
	{
	case WM_INITDIALOG:
        g_progress.SetError();
        PromoteWindow(hwnd);
        LoadString(hInst, IDS_CONFIRM_FILE_OVERWRITE, szBuf, MAX_PATH);
        wsprintf(szBuf2, szBuf, g_szDelFile);
        SetDlgItemText(hwnd, IDC_OVERWRITE_TEXT, szBuf2);
		return TRUE;
        
	case WM_COMMAND:
        g_progress.ClearError();
		EndDialog(hwnd, LOWORD(wParam));
		return TRUE;
	}
	return FALSE;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  ConfirmFolderOverwriteDlg
// Purpose:   Message handler for the 'Confirm folder overwrite' dialog. This function is automatically
//            called by Windows whenever a message needs to be sent to the dialog box (ie 'paint',
//            'move', etc).
// Arguments: hwnd          -- Handle to the Dialog box's window
//            message       -- The message sent to the dialog box
//            wParam        -- A message parameter.
//            lParam        -- Another message parameter
// Return:    nonzero if we processed the message
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
LRESULT CALLBACK ConfirmFolderOverwriteDlg(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    TCHAR szBuf[MAX_PATH];
    TCHAR szBuf2[MAX_PATH];

	switch(message)
	{
	case WM_INITDIALOG:
        g_progress.SetError();
        PromoteWindow(hwnd);
        LoadString(hInst, IDS_CONFIRM_FOLDER_OVERWRITE, szBuf, MAX_PATH);
        wsprintf(szBuf2, szBuf, g_szDelFile);
        SetDlgItemText(hwnd, IDC_OVERWRITE_TEXT, szBuf2);
		return TRUE;
        
	case WM_COMMAND:
        g_progress.ClearError();
		EndDialog(hwnd, LOWORD(wParam));
		return TRUE;
	}
	return FALSE;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  ConfirmMoveROFileDlg
// Purpose:   Message handler for the 'Confirm file move' dialog. This function is automatically
//            called by Windows whenever a message needs to be sent to the dialog box (ie 'paint',
//            'move', etc).
// Arguments: hwnd          -- Handle to the Dialog box's window
//            message       -- The message sent to the dialog box
//            wParam        -- A message parameter.
//            lParam        -- Another message parameter
// Return:    nonzero if we processed the message
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
LRESULT CALLBACK ConfirmMoveROFileDlg(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    TCHAR szBuf[MAX_PATH];
    TCHAR szBuf2[MAX_PATH];

	switch(message)
	{
	case WM_INITDIALOG:
        g_progress.SetError();
        PromoteWindow(hwnd);
        LoadString(hInst, IDS_CONFIRM_RO_FILE_MOVE, szBuf, MAX_PATH);
        wsprintf(szBuf2, szBuf, g_szDelFile);
        SetDlgItemText(hwnd, IDC_OVERWRITE_TEXT, szBuf2);
		return TRUE;
        
	case WM_COMMAND:
        g_progress.ClearError();
		EndDialog(hwnd, LOWORD(wParam));
		return TRUE;
	}
	return FALSE;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  ConfirmMoveROFolderDlg
// Purpose:   Message handler for the 'Confirm folder move' dialog. This function is automatically
//            called by Windows whenever a message needs to be sent to the dialog box (ie 'paint',
//            'move', etc).
// Arguments: hwnd          -- Handle to the Dialog box's window
//            message       -- The message sent to the dialog box
//            wParam        -- A message parameter.
//            lParam        -- Another message parameter
// Return:    nonzero if we processed the message
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
LRESULT CALLBACK ConfirmMoveROFolderDlg(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    TCHAR szBuf[MAX_PATH];
    TCHAR szBuf2[MAX_PATH];

	switch(message)
	{
	case WM_INITDIALOG:
        g_progress.SetError();
        PromoteWindow(hwnd);
        LoadString(hInst, IDS_CONFIRM_RO_FOLDER_MOVE, szBuf, MAX_PATH);
        wsprintf(szBuf2, szBuf, g_szDelFile);
        SetDlgItemText(hwnd, IDC_OVERWRITE_TEXT, szBuf2);
		return TRUE;
        
	case WM_COMMAND:
        g_progress.ClearError();
		EndDialog(hwnd, LOWORD(wParam));
		return TRUE;
	}
	return FALSE;
}
