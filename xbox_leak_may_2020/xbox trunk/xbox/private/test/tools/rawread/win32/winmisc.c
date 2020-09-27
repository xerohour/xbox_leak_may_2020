#include <windows.h>
#include <commctrl.h>
#include <shellapi.h>
#include "..\bootsec.h"
#include "debug.h"
#include "..\diskutil.h"
#include "..\mem.h"
#include "winmisc.h"
#include "resource.h"
#include "..\text.h"


typedef struct tagFILENAMESTRUCT
{
    LPSTR                   lpszLabel;
    LPSTR                   lpszFile;
} FILENAMESTRUCT, *LPFILENAMESTRUCT;


HCURSOR                     g_hPrevCursor = NULL;
HWND                        g_hWndParent = NULL;
WNDPROC                     g_lpfnOldWndProc = NULL;


BOOL CALLBACK ProgressDlgProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK GetFileNameDlgProc(HWND, UINT, WPARAM, LPARAM);
LPARAM CALLBACK DragDropCallback(HWND, UINT, WPARAM, LPARAM);


int ParseCommandLine(LPCSTR lpszCmdLine, LPSTR **lpszArg)
{
    LPSTR                   lpszParsed, lpsz;
    BOOL                    fOpenQuote = FALSE;
    int                     nArgCount = 0, i, nLen;;

    *lpszArg = NULL;
    
    // Make a copy of the command line
    if(!(lpszParsed = (LPSTR)MemAlloc(lstrlen(lpszCmdLine) + 2)))
    {
        DPF("Out of memory");
        return 0;
    }

    lpsz = lpszParsed;

    // Skip any leading spaces
    while(*lpszCmdLine == ' ')
    {
        lpszCmdLine++;
    }

    // Parse the command line
    while(*lpszCmdLine)
    {
        switch(*lpszCmdLine)
        {
            case '\"':
                fOpenQuote = !fOpenQuote;
                lpszCmdLine++;
                
                break;

            case ' ':
                if(!fOpenQuote)
                {
                    while(*lpszCmdLine && *lpszCmdLine == ' ')
                    {
                        lpszCmdLine++;
                    }
                    
                    *lpsz++ = 0;

                    break;
                }

                // Fall through

            default:
                *lpsz++ = *lpszCmdLine++;

                break;
        }
    }

    // double NULL-terminate
    *(LPWORD)lpsz = 0;

    // Count the number of arguments
    lpsz = lpszParsed;

    while(*lpsz)
    {
        while(*lpsz++);

        nArgCount++;
        lpsz++;
    }

    if(!nArgCount)
    {
        return 0;
    }

    // Return the number of arguments
    if(!(*lpszArg = (LPSTR *)MemAlloc(sizeof(LPSTR) * nArgCount)))
    {
        DPF("Out of memory");
        MemFree(lpszParsed);

        return 0;
    }

    lpsz = lpszParsed;
    
    for(i = 0; i < nArgCount; i++)
    {
        nLen = lstrlen(lpsz);
        (*lpszArg)[i] = (LPSTR)MemAlloc(nLen + 1);
        lstrcpy((*lpszArg)[i], lpsz);
        lpsz += lstrlen(lpsz) + 1;

        DPF("Argument %u:  %s", i, (*lpszArg)[i]);
    }

    return nArgCount;
}


void FreeCommandLine(int argc, LPSTR **argv)
{
    int                     i;
    
    if(argc)
    {
        for(i = 0; i < argc; i++)
        {
            MemFree((*argv)[i]);
        }

        MemFree(*argv);
    }

    *argv = NULL;
}    


void InitDriveList(HWND hWndLV)
{
    HIMAGELIST              hImageList;
    LV_COLUMN               lvc;
    LV_ITEM                 lvi;
    int                     nItem;
    BYTE                    bDrive;
    char                    szRoot[4], szLabel[0xFF];
    UINT                    uDriveType;
    SHFILEINFO              shfi;
    DWORD                   dwSectorsPerCluster, dwBytesPerSector, dwFreeClusters, dwTotalClusters;

    // Hourglass
    SetWaitCursor(hWndLV);
    
    // Reset the list
    ListView_DeleteAllItems(hWndLV);
    while(ListView_DeleteColumn(hWndLV, 0));

    if(hImageList = ListView_GetImageList(hWndLV, LVSIL_SMALL))
    {
        ImageList_Destroy(hImageList);
    }

    hImageList = ImageList_Create(16, 16, ILC_COLOR8, 16, 16);

    ImageList_SetBkColor(hImageList, GetSysColor(COLOR_WINDOW));
    ListView_SetImageList(hWndLV, hImageList, LVSIL_SMALL);    

    // Add the columns
    MemSet(&lvc, 0, sizeof(lvc));

    lvc.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
    lvc.fmt = LVCFMT_LEFT;
    lvc.cx = 125;
    lvc.pszText = "Name";
    lvc.cchTextMax = lstrlen(lvc.pszText);

    ListView_InsertColumn(hWndLV, lvc.iSubItem, &lvc);

    lvc.fmt = LVCFMT_CENTER;
    lvc.cx = 75;
    lvc.pszText = "File System";
    lvc.cchTextMax = lstrlen(lvc.pszText);

    ListView_InsertColumn(hWndLV, ++(lvc.iSubItem), &lvc);

    lvc.pszText = "Total Size";
    lvc.cchTextMax = lstrlen(lvc.pszText);

    ListView_InsertColumn(hWndLV, ++(lvc.iSubItem), &lvc);

    lvc.pszText = "Free Space";
    lvc.cchTextMax = lstrlen(lvc.pszText);

    ListView_InsertColumn(hWndLV, ++(lvc.iSubItem), &lvc);

    // Add each local drive to the list
    szRoot[1] = ':';
    szRoot[2] = '\\';
    szRoot[3] = 0;

    nItem = 0;
    
    for(bDrive = 0; bDrive < 26; bDrive++)
    {
        szRoot[0] = bDrive + 'A';
        uDriveType = GetDriveType(szRoot);

        DPF("Drive %u (%c:) is type %u", bDrive, szRoot[0], uDriveType);

        if(uDriveType == DRIVE_REMOVABLE || uDriveType == DRIVE_FIXED || uDriveType == DRIVE_RAMDISK)
        {
            if(!GetDiskFreeSpace(szRoot, &dwSectorsPerCluster, &dwBytesPerSector, &dwFreeClusters, &dwTotalClusters))
            {
                DPF("Can't get free space on drive %u", bDrive);
                continue;
            }

            if(GetDriveMapping(bDrive, NULL, NULL))
            {
                DPF("Drive %u is compressed", bDrive);
                continue;
            }

            if(!GetFileSystemType(bDrive, szLabel))
            {
                DPF("Can't get file system type for drive %u", bDrive);
                continue;
            }

            MemSet(&shfi, 0, sizeof(shfi));
            SHGetFileInfo(szRoot, 0, &shfi, sizeof(shfi), SHGFI_DISPLAYNAME | SHGFI_ICON | SHGFI_SMALLICON);
            
            MemSet(&lvi, 0, sizeof(lvi));

            lvi.mask = LVIF_IMAGE | LVIF_PARAM | LVIF_TEXT;
            lvi.iItem = nItem;
            lvi.pszText = shfi.szDisplayName;
            lvi.cchTextMax = lstrlen(lvi.pszText);
            lvi.iImage = ImageList_AddIcon(hImageList, shfi.hIcon);
            lvi.lParam = bDrive;

            ListView_InsertItem(hWndLV, &lvi);

            lvi.mask &= ~(LVIF_IMAGE | LVIF_PARAM);
            lvi.pszText = szLabel;
            lvi.cchTextMax = lstrlen(lvi.pszText);
            lvi.iSubItem++;

            ListView_SetItem(hWndLV, &lvi);

            BytesToString(dwSectorsPerCluster * dwBytesPerSector * dwTotalClusters, szLabel);
            
            lvi.pszText = szLabel;
            lvi.cchTextMax = lstrlen(lvi.pszText);
            lvi.iSubItem++;

            ListView_SetItem(hWndLV, &lvi);

            BytesToString(dwSectorsPerCluster * dwBytesPerSector * dwFreeClusters, szLabel);
                        
            lvi.pszText = szLabel;
            lvi.cchTextMax = lstrlen(lvi.pszText);
            lvi.iSubItem++;

            ListView_SetItem(hWndLV, &lvi);

            nItem++;
        }
    }

    // Un-hourglass
    ReleaseWaitCursor();
}


void BytesToString(DWORDLONG dwBytes, LPSTR lpszString)
{
    DWORDLONG               dwDiv, dwMod;
    LPCSTR                  lpszLabel;
    
    if(dwBytes >= 0x40000000)
    {
        dwDiv = 0x40000000;
        lpszLabel = "GB";
    }
    else if(dwBytes >= 0x100000)
    {
        dwDiv = 0x100000;
        lpszLabel = "MB";
    }
    else
    {
        dwDiv = 1;
        lpszLabel = "Bytes";
    }

    wsprintf(lpszString, "%lu", (DWORD)(dwBytes / dwDiv));
    
    if(dwMod = dwBytes % dwDiv)
    {
        wsprintf(lpszString + lstrlen(lpszString), ".%-2.2lu", (DWORD)((dwMod * 100) / dwDiv));
    }

    lstrcat(lpszString, " ");
    lstrcat(lpszString, lpszLabel);
}


void SetWaitCursor(HWND hWnd)
{
    SetCapture(hWnd);
    g_hPrevCursor = SetCursor(LoadCursor(NULL, IDC_WAIT));
}


void ReleaseWaitCursor(void)
{
    SetCursor(g_hPrevCursor);
    ReleaseCapture();
}


BOOL BrowseForImageFile(HWND hWndParent, LPSTR lpszFileName, BOOL fAllowCreate)
{
    OPENFILENAME            ofn;
    static char             szCurDir[MAX_PATH];

    MemSet(&ofn, 0, sizeof(ofn));

    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hWndParent;
    ofn.lpstrFilter = "Image Files (*.img)\0*.img\0All Files (*.*)\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFile = lpszFileName;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrInitialDir = szCurDir;
    ofn.lpstrTitle = "Open Image File";
    ofn.Flags = OFN_EXPLORER | OFN_HIDEREADONLY | OFN_LONGNAMES | OFN_PATHMUSTEXIST;
    ofn.lpstrDefExt = "img";

    if(!fAllowCreate)
    {
        ofn.Flags |= OFN_FILEMUSTEXIST;
    }

    return GetOpenFileName(&ofn);
}


void CenterWindow(HWND hWnd, HWND hWndParent)
{
    RECT                    rc, rcParent;

    GetWindowRect(hWnd, &rc);

    if(hWndParent)
    {
        GetWindowRect(hWndParent, &rcParent);
    }
    else
    {
        SystemParametersInfo(SPI_GETWORKAREA, 0, &rcParent, 0);
    }

    SetWindowPos(hWnd, NULL, rcParent.left + (((rcParent.right - rcParent.left) - (rc.right - rc.left)) / 2), rcParent.top + (((rcParent.bottom - rcParent.top) - (rc.bottom - rc.top)) / 2), 0, 0, SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOSIZE | SWP_NOZORDER);
}


HWND CreateProgressDialog(HWND hWndParent)
{
    g_hWndParent = hWndParent;
    
    return CreateDialog(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_PROGRESS), hWndParent, ProgressDlgProc);
}


HWND CloseProgressDialog(HWND hWnd)
{
    // Close the dialog
    if(IsWindow(hWnd))
    {
        SendMessage(hWnd, WM_CLOSE, 0, 0);
        while(IsWindow(hWnd));
    }

    // Return the parent window handle
    return g_hWndParent;
}


BOOL CALLBACK ProgressDlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    HWND                    hWndParent;
    
    switch(uMsg)
    {
        case WM_INITDIALOG:
            hWndParent = GetParent(hWnd);
            
            // Initialize the progress bar
            SendDlgItemMessage(hWnd, IDC_PROGRESS, PBM_SETRANGE, 0, MAKELPARAM(0, 100));
            SendDlgItemMessage(hWnd, IDC_PROGRESS, PBM_SETPOS, 0, 0);

            // Center the window
            CenterWindow(hWnd, hWndParent);

            // Become visible
            ShowWindow(hWnd, SW_RESTORE);
            UpdateWindow(hWnd);

            // Disable the parent window
            EnableWindow(hWndParent, FALSE);

            return FALSE;

        case WM_CLOSE:
            hWndParent = GetParent(hWnd);
            
            // Enable the parent window
            EnableWindow(hWndParent, TRUE);
            
            // Destroy the dialog
            ShowWindow(hWnd, SW_HIDE);
            DestroyWindow(hWnd);

            break;

        case WM_COMMAND:
            switch(LOWORD(wParam))
            {
                case IDCANCEL:
                    SendMessage(hWnd, WM_CLOSE, 0, 0);

                    break;
            }

            break;
    }

    return FALSE;
}


void SetProgressLabel(HWND hWnd, LPSTR lpszFormat, ...)
{
    va_list                 va;
    char                    szString[0xFF];

    va_start(va, lpszFormat);
    wvsprintf(szString, lpszFormat, va);
    va_end(va);

    SetDlgItemText(hWnd, IDC_LABEL, szString);
    
    UpdateWindow(hWnd);
}


void SetProgressPos(HWND hWnd, DWORD dwCurrentCluster, DWORD dwTotalClusters)
{
    char                    szCluster[13];

    wsprintf(szCluster, "%lu", dwCurrentCluster);
    
    SendDlgItemMessage(hWnd, IDC_PROGRESS, PBM_SETPOS, (WORD)(((DWORDLONG)dwCurrentCluster * 100) / (DWORDLONG)dwTotalClusters), 0);
    SetDlgItemText(hWnd, IDC_CLUSTER, szCluster);

    UpdateWindow(hWnd);
}


BOOL GetFileName(HWND hWndParent, LPSTR lpszLabel, LPSTR lpszFile)
{
    FILENAMESTRUCT          fns;
    
    fns.lpszLabel = lpszLabel;
    fns.lpszFile = lpszFile;

    return DialogBoxParam(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_FRAGNAME), hWndParent, GetFileNameDlgProc, (LPARAM)&fns);
}


BOOL GetFragmentName(HWND hWndParent, UINT uFrag, LPSTR lpszFile)
{
    char                    szLabel[512];

    wsprintf(szLabel, "Please enter the filename for fragment %u", uFrag);

    return GetFileName(hWndParent, szLabel, lpszFile);
}


BOOL CALLBACK GetFileNameDlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    static LPFILENAMESTRUCT lpfns;
    char                    szFile[MAX_PATH];

    switch(uMsg)
    {
        case WM_INITDIALOG:
            lpfns = (LPFILENAMESTRUCT)lParam;

            // Set the text label
            SetDlgItemText(hWnd, IDC_LABEL, lpfns->lpszLabel);

            // Initialize the filename
            SetDlgItemText(hWnd, IDC_FRAGNAME, lpfns->lpszFile);

            return TRUE;

        case WM_CLOSE:
            EndDialog(hWnd, FALSE);

            break;

        case WM_COMMAND:
            switch(LOWORD(wParam))
            {
                case IDOK:
                    GetDlgItemText(hWnd, IDC_FRAGNAME, szFile, MAX_PATH);

                    // Check the image filename.  Names > 8.3 are not supported
                    if(ValidateFileName(szFile))
                    {
                        ImageMessageBox(hWnd, MB_ICONEXCLAMATION, "Long filenames are not supported for image fragments.");
                        break;
                    }
    
                    GetFullPath(lpfns->lpszFile, szFile, MAX_PATH);
                    EndDialog(hWnd, TRUE);

                    break;

                case IDCANCEL:
                    SendMessage(hWnd, WM_CLOSE, 0, 0);

                    break;

                case IDC_FRAGNAME_BROWSE:
                    GetDlgItemText(hWnd, IDC_FRAGNAME, szFile, sizeof(szFile));
                    
                    if(BrowseForImageFile(hWnd, szFile, TRUE))
                    {
                        SetDlgItemText(hWnd, IDC_FRAGNAME, szFile);
                    }
                    
                    break;
            }

            break;
    }

    return FALSE;
}


void ProcessMessages(HWND hWnd)
{
    MSG                         msg;

    while(PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
    {
        GetMessage(&msg, NULL, 0, 0);

        if(!IsDialogMessage(hWnd, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
}
    

void GetCheckFlag(HWND hWnd, UINT uId, WORD wBit, LPWORD lpwFlags)
{
    if(IsDlgButtonChecked(hWnd, uId))
    {
        *lpwFlags |= wBit;
    }
    else
    {
        *lpwFlags &= ~wBit;
    }
}

void SetCheckFlag(HWND hWnd, UINT uId, WORD wBit, WORD wFlags)
{
    CheckDlgButton(hWnd, uId, (wFlags & wBit) ? BST_CHECKED : BST_UNCHECKED);
}


void EnableDragDrop(HWND hWnd, BOOL fEnable)
{
    DragAcceptFiles(hWnd, fEnable);

    if(fEnable && !g_lpfnOldWndProc)
    {
        g_lpfnOldWndProc = (WNDPROC)SetWindowLong(hWnd, GWL_WNDPROC, (LONG)DragDropCallback);
    }
}


LPARAM CALLBACK DragDropCallback(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    char                        szFile[MAX_PATH];

    switch(uMsg)
    {
        case WM_DROPFILES:
            DragQueryFile((HDROP)wParam, 0, szFile, sizeof(szFile));
            DragFinish((HDROP)wParam);
            SetWindowText(hWnd, szFile);

            return TRUE;
    }

    return g_lpfnOldWndProc ? CallWindowProc(g_lpfnOldWndProc, hWnd, uMsg, wParam, lParam) : FALSE;
}


