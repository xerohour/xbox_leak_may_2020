// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:      StartupDlg.cpp
// Contents:  
// Revisions: 1-Feb-2002: Created (jeffsim)
//
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ INCLUDE FILES ++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// "stdafx.h"       -- stdafxiled header file
#include "stdafx.h"

static BOOL g_fDoOpen;

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ FUNCTIONS ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

LRESULT CALLBACK StartupDialog(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	case WM_INITDIALOG:
        CheckRadioButton(hwnd, IDC_NEW, IDC_OPEN, g_fDoOpen ? IDC_OPEN : IDC_NEW);
        SetDlgControlFont(hwnd, IDC_NEW, 14, "Arial", FW_BOLD);
        SetDlgControlFont(hwnd, IDC_OPEN, 14, "Arial", FW_BOLD);
        SetDlgControlFont(hwnd, IDC_TITLE, 14, "Arial", FW_BOLD);
        CheckDlgButton(hwnd, IDC_DONTSHOW, !g_fDoStartupDlg);
		return 1;

    case WM_CTLCOLORSTATIC:
        if ((HWND)lParam == GetDlgItem(hwnd, IDC_TITLE))
        {
            SetBkColor((HDC)wParam, RGB(255,255,255));
            return (BOOL)GetStockObject(WHITE_BRUSH);
        }
        return FALSE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK)
        {
            g_fDoStartupDlg = !IsDlgButtonChecked(hwnd, IDC_DONTSHOW);
            g_fDoOpen = IsDlgButtonChecked(hwnd, IDC_OPEN);
		    EndDialog(hwnd, LOWORD(wParam));
        }
		return TRUE;
	}
	return FALSE;
}


void StartupDlg()
{
    g_fDoOpen = false;
    while (true)
    {
        DialogBox(g_hInst, MAKEINTRESOURCE(IDD_STARTUP), g_hwndMain, (DLGPROC) ::StartupDialog);
        if (g_fDoOpen)
        {
            if (HandleOpen())
                break;
        }
        else
        {
            if (HandleNew())
                break;
        }
    }
}

