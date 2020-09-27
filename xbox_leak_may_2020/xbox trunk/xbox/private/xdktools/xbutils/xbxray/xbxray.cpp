/*==========================================================================;
 *
 *  Copyright (C) Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       xbxray.cpp
 *  Content:    Overdraw visualization tool
 *
 ***************************************************************************/

#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <stdlib.h>
#include <xboxdbg.h>
#include "resource.h"
#include <xboxverp.h>
#include "xbfile.hxx"

// encodings for flags
#define INCREMENTSHIFT  4
#define INCREMENTMASK   (0xff << INCREMENTSHIFT)

#define FILLMODESHIFT   12
#define FILLMODEMASK    (0x3 << FILLMODESHIFT)

#define ZENABLESHIFT    14
#define ZENABLEMASK     (0x3 << ZENABLESHIFT)

#define STENCILENABLESHIFT    16
#define STENCILENABLEMASK     (0x3 << STENCILENABLESHIFT)

#define CULLMODESHIFT    18
#define CULLMODEMASK     (0x3 << CULLMODESHIFT)

#define GRAYSHIFT       20
#define GRAYMASK        (0x1 << GRAYSHIFT)

char buf[65536];

DWORD g_dwFlags = 0;
DWORD g_nPos;
DWORD g_dwMarker;
BOOL g_bEnabled = FALSE;
SCROLLINFO si;

void enableXray(HWND hwnd)
{
    HRESULT hr = S_OK;
    g_dwMarker = GetDlgItemInt(hwnd, IDC_EDIT4, NULL, FALSE);
    DWORD flags=0;
    flags = 0x08000000 | g_dwFlags;
    flags |= g_nPos << INCREMENTSHIFT;
    if(IsDlgButtonChecked(hwnd, IDC_RADIO2 ) == BST_CHECKED) flags |= (1 << FILLMODESHIFT);
    if(IsDlgButtonChecked(hwnd, IDC_RADIO3 ) == BST_CHECKED) flags |= (2 << FILLMODESHIFT);
    if(IsDlgButtonChecked(hwnd, IDC_RADIO4 ) == BST_CHECKED) flags |= (3 << FILLMODESHIFT);
    if(IsDlgButtonChecked(hwnd, IDC_RADIO6 ) == BST_CHECKED) flags |= (1 << ZENABLESHIFT);
    if(IsDlgButtonChecked(hwnd, IDC_RADIO7 ) == BST_CHECKED) flags |= (2 << ZENABLESHIFT);
    if(IsDlgButtonChecked(hwnd, IDC_RADIO8 ) == BST_CHECKED) flags |= (3 << ZENABLESHIFT);
    if(IsDlgButtonChecked(hwnd, IDC_RADIO10) == BST_CHECKED) flags |= (1 << STENCILENABLESHIFT);
    if(IsDlgButtonChecked(hwnd, IDC_RADIO11) == BST_CHECKED) flags |= (2 << STENCILENABLESHIFT);
    if(IsDlgButtonChecked(hwnd, IDC_RADIO14) == BST_CHECKED) flags |= (1 << CULLMODESHIFT);
    if(IsDlgButtonChecked(hwnd, IDC_RADIO15) == BST_CHECKED) flags |= (2 << CULLMODESHIFT);
    if(IsDlgButtonChecked(hwnd, IDC_RADIO16) == BST_CHECKED) flags |= (3 << CULLMODESHIFT);
    if(IsDlgButtonChecked(hwnd, IDC_GRAY)    == BST_CHECKED) flags |= (1 << GRAYSHIFT);
    
    hr = DmVertexShaderSnapshot(0, 1, flags, g_dwMarker, (unsigned char *)buf);
    if(FAILED(hr)) 
    {
        if(hr == XBDM_FILEERROR)
            hr = HrLastError();
        MessageBox(NULL, (const char*)buf, "xbxray Error", MB_OK | MB_TASKMODAL);
    }
    g_bEnabled = TRUE;
    SetDlgItemText(hwnd, IDC_StatusText, "Xray is active");
}

void disableXray(HWND hwnd)
{
    HRESULT hr = S_OK;
    hr = DmVertexShaderSnapshot(0, 1, 0x04000000, 0, (unsigned char *)buf);
    if(FAILED(hr)) 
    {
        if(hr == XBDM_FILEERROR)
            hr = HrLastError();
        MessageBox(NULL, (const char*)buf, "xbxray Error", MB_OK | MB_TASKMODAL);
    }
    g_bEnabled = FALSE;
    SetDlgItemText(hwnd, IDC_StatusText, "Ready");
}

void updateXray(HWND hwnd)
{
    // if enabled, send new parameters
    if(g_bEnabled) enableXray(hwnd);
}

BOOL Dlg_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
    SetDlgItemText(hwnd, IDC_StatusText, "Initializing...");
    EnableWindow(GetDlgItem(hwnd, IDC_EDIT4), FALSE);
    
    // initialize the scroll bar
    si.cbSize = sizeof(si);
    si.fMask = SIF_POS | SIF_RANGE;
    g_nPos = si.nPos = 32;
    si.nMin = 1;
    si.nMax = 255;
    SetScrollInfo(GetDlgItem(hwnd, IDC_SCROLLBAR1), SB_CTL, &si, TRUE);
    sprintf(buf, "%d Levels", 255/si.nPos + 1);
    SetDlgItemText(hwnd, IDC_LevelText, buf);

    CheckDlgButton(hwnd, IDC_RADIO1, BST_CHECKED);
    CheckDlgButton(hwnd, IDC_RADIO7, BST_CHECKED);
    CheckDlgButton(hwnd, IDC_RADIO9, BST_CHECKED);
    CheckDlgButton(hwnd, IDC_RADIO13, BST_CHECKED);

    CheckDlgButton(hwnd, IDC_GRAY, BST_CHECKED);

    SetDlgItemText(hwnd, IDC_StatusText, "Ready");
    return TRUE;
}

void Dlg_OnHScroll(HWND hwnd, HWND hwndCtl, UINT uCode, int nPos)
{
    si.fMask = SIF_POS;
    GetScrollInfo(hwndCtl, SB_CTL, &si);
    switch(uCode)
    {
    case SB_LINELEFT:
    case SB_PAGELEFT:
        si.nPos -= 1;
        break;
    case SB_LINERIGHT:
    case SB_PAGERIGHT:
        si.nPos += 1;
        break;
    case SB_THUMBTRACK:
        si.nPos = nPos;
        break;
    }
    SetScrollInfo(hwndCtl, SB_CTL, &si, TRUE);
    GetScrollInfo(hwndCtl, SB_CTL, &si);
    sprintf(buf, "%d Levels", 255/si.nPos + 1);
    SetDlgItemText(hwnd, IDC_LevelText, buf);
    g_nPos = si.nPos;
    updateXray(hwnd);
}

void Dlg_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
    switch(id)
    {
    case IDCANCEL:
        EndDialog(hwnd, id);
        break;

    case IDC_CHECK1:
        if(IsDlgButtonChecked(hwnd, IDC_CHECK1) == BST_CHECKED)
        {
            enableXray(hwnd);
        }
        else
        {
            disableXray(hwnd);
        }
        break;
    case IDC_CHECK2:
        if(IsDlgButtonChecked(hwnd, IDC_CHECK2) == BST_CHECKED)
        {
            // enable marker window
            EnableWindow(GetDlgItem(hwnd, IDC_EDIT4), TRUE);
            g_dwFlags |= 0x1;  // set marker flag
            updateXray(hwnd);
        }
        else
        {
            EnableWindow(GetDlgItem(hwnd, IDC_EDIT4), FALSE);
            g_dwFlags &= ~0x1;  // reset marker flag
            updateXray(hwnd);
        }
        break;
    
    case IDC_RADIO1:
    case IDC_RADIO2:
    case IDC_RADIO3:
    case IDC_RADIO4:
    case IDC_RADIO5:
    case IDC_RADIO6:
    case IDC_RADIO7:
    case IDC_RADIO8:
    case IDC_RADIO9:
    case IDC_RADIO10:
    case IDC_RADIO11:
    case IDC_RADIO13:
    case IDC_RADIO14:
    case IDC_RADIO15:
    case IDC_RADIO16:
    case IDC_GRAY:
    case IDC_EDIT4:
        updateXray(hwnd);
        break;
    }
}

BOOL CALLBACK Dlg_Proc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch(uMsg)
    {
    HANDLE_MSG(hDlg, WM_COMMAND, Dlg_OnCommand);
    HANDLE_MSG(hDlg, WM_INITDIALOG, Dlg_OnInitDialog);
    HANDLE_MSG(hDlg, WM_HSCROLL, Dlg_OnHScroll);
    case WM_HELP: printf("Help!\n"); break;
    default:
        return FALSE;
    }
    return TRUE;
}

int __cdecl main(int cArg, char **rgszArg)
{
    // display the UI
    DialogBox(NULL, MAKEINTRESOURCE(IDD_XRAY), NULL, Dlg_Proc);
    disableXray(NULL);
    return 0;
}



