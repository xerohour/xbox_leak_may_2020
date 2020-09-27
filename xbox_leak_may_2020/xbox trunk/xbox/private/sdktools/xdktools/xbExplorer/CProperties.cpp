#include "stdafx.h"


char *g_rgszMonth[] = {"January", "February", "March", "April", "May", "June",
                       "July", "August", "September", "October", "November", "December"};
DM_FILE_ATTRIBUTES g_dmfa;

void GetTimeBuf(FILETIME *pft, char *szBuf)
{
    SYSTEMTIME systime;
    listview.FileTime2LocalTime(pft, &systime);

    wsprintf(szBuf, "%s %d, %d, %d:%02d:%02d %s", g_rgszMonth[systime.wMonth-1], systime.wDay,
            systime.wYear, (systime.wHour % 12), systime.wMinute, systime.wSecond, 
            (systime.wHour) < 12 ? "AM" : "PM");
}

static char g_szBuffer[32] = {'\0'};

char *FormatBigNumber(ULONGLONG n)
{
    // Convert the number into a comma-separated string; ie "1,234,567,890"
    char *sz = (char*)(g_szBuffer + 31);
    int i = 0;

    *sz = '\0';
    do {
        *--sz = ((char)(n%10)) + '0';
        if ((++i)%3 == 0 && n/10)
            *--sz = ',';
    } while (n/=10);

    return sz;
}

void DrawFolderProperties(HWND hwnd, DM_FILE_ATTRIBUTES *pdmfa, char *szPath, char *szType,
                          HICON hicon)
{
    char szBuf[256];

    SetDlgItemText(hwnd, IDC_FOLDEREPROP_EDIT_TYPE, szType);
    SetDlgItemText(hwnd, IDC_FOLDEREPROP_EDIT_LOC,  szPath);

    ULARGE_INTEGER ulSize;
    ulSize.LowPart = pdmfa->SizeLow;
    ulSize.HighPart= pdmfa->SizeHigh;

    // UNDONE-ERR: Remove or fix these!
    SetDlgItemText(hwnd, IDC_FOLDEREPROP_EDIT_SIZE, "Size of all sub files");
    SetDlgItemText(hwnd, IDC_FOLDEREPROP_EDIT_SIZEONDISK, "File size on disk");

    SetDlgItemText(hwnd, IDC_FOLDEREPROP_EDIT_CONTAINS, "# sub files and folders");

    // File times
    GetTimeBuf(&pdmfa->CreationTime, szBuf);
    SetDlgItemText(hwnd, IDC_FOLDEREPROP_EDIT_CREATED, szBuf);

    SetDlgItemText(hwnd, IDC_FOLDEREPROP_EDIT_NAME, pdmfa->Name);

    if (pdmfa->Attributes & FILE_ATTRIBUTE_HIDDEN)
        SendMessage(GetDlgItem(hwnd, IDC_FOLDERPROP_ATTR_HIDDEN), BM_SETCHECK, BST_CHECKED, 0);
    else
        SendMessage(GetDlgItem(hwnd, IDC_FOLDERPROP_ATTR_HIDDEN), BM_SETCHECK, BST_UNCHECKED, 0);

    if (pdmfa->Attributes & FILE_ATTRIBUTE_READONLY)
        SendMessage(GetDlgItem(hwnd, IDC_FOLDERPROP_ATTR_RO), BM_SETCHECK, BST_CHECKED, 0);
    else
        SendMessage(GetDlgItem(hwnd, IDC_FOLDERPROP_ATTR_RO), BM_SETCHECK, BST_UNCHECKED, 0);

    SendMessage(GetDlgItem(hwnd, IDC_FOLDERPROP_ICON), STM_SETICON, (WPARAM)hicon, 0);

    if (!MyStrcmp(g_dmfa.Name, szPath))
    {
        // Root drive
        EnableWindow(GetDlgItem(hwnd, IDC_FOLDEREPROP_EDIT_NAME), false);
        EnableWindow(GetDlgItem(hwnd, IDC_FOLDERPROP_ATTR_HIDDEN), false);
        EnableWindow(GetDlgItem(hwnd, IDC_FOLDERPROP_ATTR_RO), false);
        ShowWindow(GetDlgItem(hwnd, IDC_FOLDEREPROP_EDIT_CREATED), false);
    }
}

void GetFolderProperties(DM_FILE_ATTRIBUTES *pdmfa, char *szPath, char *szType, HICON *phicon)
{
    char szFileName[MAX_PATH];
    bool fRoot = false, fThis = false;
    char *sz;
    char szFilePath[MAX_PATH];

    SHFILEINFO sfi;
	ZeroMemory( &sfi, sizeof(sfi) );
	DWORD flg = SHGFI_ICON | SHGFI_LARGEICON | SHGFI_USEFILEATTRIBUTES;
    // if g_iPopupFileItem == ITEM_NONE, then its 'this' dir
    if (g_iPopupFileItem == ITEM_NONE)
    {
        strcpy(szFileName, g_CurrentDir);
        strcpy(szPath, g_CurrentDir);
        if (MyStrlen(szPath) == 4)
            fRoot = true;
        else
            fRoot = false;
        fThis = true;
    }
    else
    {
        // Get file name from list view
	    ListView_GetItemText(listview.GetHwnd(), g_iPopupFileItem, COLUMN_NAME, szFileName, MAX_PATH);

        strcpy(szPath, g_CurrentDir);
        if (MyStrlen(szPath) > 4)
            szPath[MyStrlen(szPath)-1] = '\0';
    }

    if (fRoot)
    {
        // it's a root drive!
        strcpy(szType, "Xbox Drive");
    	SHGetFileInfo("C:\\", 0, &sfi, sizeof(sfi), flg );
        wsprintf(szFilePath, "%s%s", g_CurrentDir, szFileName);
    }
    else
    {
        strcpy(szType, "File Folder");
	    SHGetFileInfo(szPath, 0, &sfi, sizeof(sfi), flg );
        if (fThis)
        {   
            sz = strrchr(szFileName, '\\');
            if (sz)
                *sz = '\0';
            sz = strrchr(szPath, '\\');
            if (sz)
                *sz = '\0';

            sz = strrchr(szPath, '\\');
            if (sz == szPath + 3)
            {
                // First level folder - keep the \ on XE:\!
                *(sz+1) = '\0';
            }
            else
            {
                // Deeper folder.  Remove the slach
                *sz = '\0';
            }
            wsprintf(szFilePath, "%s", szFileName);
        }
        else
            wsprintf(szFilePath, "%s%s", g_CurrentDir, szFileName);
    }
    *phicon = sfi.hIcon;

    xbfu.GetFileAttributes(szFilePath, pdmfa);

    // Account for root drive
    if (fRoot)
        strcpy(pdmfa->Name, szFileName);
}

LRESULT CALLBACK FolderPropertiesDlg(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    static char szPath[MAX_PATH];
    char szType[256], szBuf[MAX_PATH], szOrig[MAX_PATH];
    TCITEM tie; 
    HICON hicon;
    bool fNewHidden, fNewReadOnly;
    bool fOldHidden, fOldReadOnly;

	switch(message)
	{
	case WM_INITDIALOG:
        // Set the initial values

        // Add 'general' tab
        tie.mask = TCIF_TEXT | TCIF_IMAGE; 
        tie.iImage = -1; 
        tie.pszText = "General"; 
 
        TabCtrl_InsertItem(GetDlgItem(hDlg, IDC_TAB), 0, &tie);
    
        GetFolderProperties(&g_dmfa, szPath, szType, &hicon);

        DrawFolderProperties(hDlg, &g_dmfa, szPath, szType, hicon);
//		CenterDialog(hDlg);
		return TRUE;

    case WM_COMMAND:
		switch(LOWORD(wParam))
		{
        case IDAPPLY:
		case IDOK:
            // Check to see if the user (a) changed the app name or (b) changed attribute
            // state -- if so, then make the modifications now.  
            fNewHidden   = SendMessage(GetDlgItem(hDlg, IDC_FOLDERPROP_ATTR_HIDDEN), BM_GETCHECK, 0, 0) == BST_CHECKED;
            fNewReadOnly = SendMessage(GetDlgItem(hDlg, IDC_FOLDERPROP_ATTR_RO),     BM_GETCHECK, 0, 0) == BST_CHECKED;
            fOldHidden   = g_dmfa.Attributes & FILE_ATTRIBUTE_HIDDEN   ? true : false;
            fOldReadOnly = g_dmfa.Attributes & FILE_ATTRIBUTE_READONLY ? true : false;

            GetDlgItemText(hDlg, IDC_FOLDEREPROP_EDIT_NAME, szBuf, MAX_PATH);

            // Determine the name of the file or folder being modified
            if (g_iPopupFileItem == -1)
            {
                // User clicked on white space - use current dir as the full pathname
                strcpy(szOrig, g_CurrentDir);
                
                // Remove the trailing '\'
                szOrig[strlen(szOrig) - 1] = '\0';
            }
            else
                wsprintf(szOrig, "%s%s", g_CurrentDir, g_dmfa.Name);

            if (MyStrcmp(g_dmfa.Name, szBuf))
            {
                char szNew[MAX_PATH];
                bool fRename = true;

                // Determine the new path
                if (g_iPopupFileItem == -1)
                {
                    // User clicked on white space - use current dir as the full pathname
                    strcpy(szNew, g_CurrentDir);
                
                    // Remove the trailing '\'
                    szNew[strlen(szNew) - 1] = '\0';

                    // Remove the name of the current dir from the fullpath
                    *(strrchr(szNew, '\\') + 1) = '\0';

                    // Add the *new* name of the current dir to the fullpath
                    strcat(szNew, szBuf);
                }
                else
                    wsprintf(szNew, "%s%s", g_CurrentDir, szBuf);

                // User changed the name.  Read only? 
                if (g_dmfa.Attributes & FILE_ATTRIBUTE_READONLY)
                {
                    sprintf(szType, "Are you sure you want to rename the read-only folder '%s' to '%s'?", g_dmfa.Name, szBuf);
                    if (MessageBox(hDlg, szType, "Confirm Folder Rename", MB_ICONWARNING | MB_YESNO) == IDYES)
                    {
                        // Remove the read only bit
                        xbfu.RemoveAttribute(szOrig, FILE_ATTRIBUTE_READONLY, true);
                        xbfu.Ren(szOrig, szNew);
                        xbfu.SetAttribute(szNew, FILE_ATTRIBUTE_READONLY, true);
                    }
                    else
                    {
                        SetDlgItemText(hDlg, IDC_FOLDEREPROP_EDIT_NAME, g_dmfa.Name);
                        fRename = false;
                    }
                }
                else
                    xbfu.Ren(szOrig, szNew);
                if (fRename)
                {
                    // if the user changed the name of the current directory (right-clicked on whitespace),
                    // then we need to change g_CurrentDir now
                    if (g_iPopupFileItem == -1)
                    {
                        strcpy(g_CurrentDir, szNew);
                        strcat(g_CurrentDir, "\\");
                    }

                    strcpy(g_dmfa.Name, szBuf);
                    listview.SortListView(-1, false);
                }
            }

            if ((fNewHidden && !fOldHidden) || (fOldHidden && !fNewHidden) ||
                (fNewReadOnly && !fOldReadOnly) || (fOldReadOnly && !fNewReadOnly))
            {
                // user swapped the hidden and/or readonly states
                g_dmfa.Attributes = (fNewHidden   ? FILE_ATTRIBUTE_HIDDEN   : 0) |
                                  (fNewReadOnly ? FILE_ATTRIBUTE_READONLY : 0);

                xbfu.SetFileAttributes(szOrig, &g_dmfa);
                listview.SortListView(-1, false);
            }

            if (LOWORD(wParam) == IDOK)
            {
                // We're done
			    EndDialog(hDlg, TRUE);
			    return TRUE;
            }
               
            break;

        case IDCANCEL:
			EndDialog(hDlg, TRUE);
			return TRUE;
		}
		break;
	}
	return FALSE;
}

void FormatSize(ULARGE_INTEGER ulSize, char *szBuf, bool fBytes)
{
    if (ulSize.QuadPart < 1024)
        sprintf(szBuf, "%d bytes", ulSize.LowPart);
    else if (ulSize.QuadPart / 1024 < 1024)
        sprintf(szBuf, "%.2f KB", ((float)(ulSize.QuadPart)) / 1024.0);
    else if (ulSize.QuadPart / 1024 / 1024< 1024)
        sprintf(szBuf, "%.2f MB", ((float)(ulSize.QuadPart/1024)) / 1024.0);
    else 
        sprintf(szBuf, "%.2f GB",((float)(ulSize.QuadPart/1024/1024)) / 1024.0);

    if (fBytes)
    {
        char szTemp[100];
        wsprintf(szTemp, "%s (%s bytes)", szBuf, FormatBigNumber(ulSize.QuadPart));
        wsprintf(szBuf, szTemp);
    }
}

void DrawFileProperties(HWND hwnd, DM_FILE_ATTRIBUTES *pdmfa, char *szPath, char *szType,
                        HICON hicon)
{
    char szBuf[256];

    SetDlgItemText(hwnd, IDC_FILEPROP_EDIT_TYPE, szType);
    SetDlgItemText(hwnd, IDC_FILEPROP_EDIT_LOC,  szPath);

    ULARGE_INTEGER ulSize;
    ulSize.LowPart = pdmfa->SizeLow;
    ulSize.HighPart= pdmfa->SizeHigh;

    FormatSize(ulSize, szBuf, true);
    SetDlgItemText(hwnd, IDC_FILEPROP_EDIT_SIZE, szBuf);
    
#if 0
    // UNDONE-ERR: Assuming sector size of 4096 bytes.  Verify and change if necessary!
#define SECTOR_SIZE 4096
    ulSize.QuadPart = (ulSize.QuadPart / SECTOR_SIZE + 1) * SECTOR_SIZE;

    FormatSize(ulSize, szBuf, true);
    SetDlgItemText(hwnd, IDC_FILEPROP_EDIT_SIZEONDISK, szBuf);
#endif

    // File times
    GetTimeBuf(&pdmfa->CreationTime, szBuf);
    SetDlgItemText(hwnd, IDC_FILEPROP_EDIT_CREATED, szBuf);
    GetTimeBuf(&pdmfa->ChangeTime, szBuf);
    SetDlgItemText(hwnd, IDC_FILEPROP_EDIT_MODIFIED, szBuf);
    SetDlgItemText(hwnd, IDC_FILEPROP_EDIT_ACCESSED, "file accessed");

    SetDlgItemText(hwnd, IDC_FILEPROP_EDIT_NAME, pdmfa->Name);

    if (pdmfa->Attributes & FILE_ATTRIBUTE_HIDDEN)
        SendMessage(GetDlgItem(hwnd, IDC_FILEPROP_ATTR_HIDDEN), BM_SETCHECK, BST_CHECKED, 0);
    else
        SendMessage(GetDlgItem(hwnd, IDC_FILEPROP_ATTR_HIDDEN), BM_SETCHECK, BST_UNCHECKED, 0);

    if (pdmfa->Attributes & FILE_ATTRIBUTE_READONLY)
        SendMessage(GetDlgItem(hwnd, IDC_FILEPROP_ATTR_RO), BM_SETCHECK, BST_CHECKED, 0);
    else
        SendMessage(GetDlgItem(hwnd, IDC_FILEPROP_ATTR_RO), BM_SETCHECK, BST_UNCHECKED, 0);

    SendMessage(GetDlgItem(hwnd, IDC_FILEPROP_ICON), STM_SETICON, (WPARAM)hicon, 0);
}

void GetFileProperties(DM_FILE_ATTRIBUTES *pdmfa, char *szPath, char *szType, HICON *phicon)
{
    char szFileName[MAX_PATH];
    char szFilePath[MAX_PATH];

    // Get file name from list view
	ListView_GetItemText(listview.GetHwnd(), g_iPopupFileItem, COLUMN_TYPE, szType, MAX_PATH);
	ListView_GetItemText(listview.GetHwnd(), g_iPopupFileItem, COLUMN_NAME, szFileName, MAX_PATH);

    strcpy(szPath, g_CurrentDir);
    if (MyStrlen(szPath) > 4)
        szPath[MyStrlen(szPath)-1] = '\0';
    strcpy(szFilePath, g_CurrentDir);
    strcat(szFilePath, szFileName);

	SHFILEINFO sfi;
	ZeroMemory( &sfi, sizeof(sfi) );
	DWORD flg = SHGFI_ICON | SHGFI_LARGEICON | SHGFI_USEFILEATTRIBUTES;
	SHGetFileInfo(szFilePath, 0, &sfi, sizeof(sfi), flg );
    *phicon = sfi.hIcon;

    xbfu.GetFileAttributes(szFilePath, pdmfa);
}

LRESULT CALLBACK FilePropertiesDlg(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    static char szPath[MAX_PATH];
    char szType[256], szBuf[MAX_PATH];
    TCITEM tie; 
    HICON hicon;
    bool fNewHidden, fNewReadOnly;
    bool fOldHidden, fOldReadOnly;

	switch(message)
	{
	case WM_INITDIALOG:
        // Set the initial values
        // Add 'general' tab
        tie.mask = TCIF_TEXT | TCIF_IMAGE; 
        tie.iImage = -1; 
        tie.pszText = &szBuf[0];
        LoadString(hInst, IDS_GENERAL, szBuf, MAX_PATH);

        TabCtrl_InsertItem(GetDlgItem(hDlg, IDC_TAB), 0, &tie);

        GetFileProperties(&g_dmfa, szPath, szType, &hicon);

        DrawFileProperties(hDlg, &g_dmfa, szPath, szType, hicon);
        
//		CenterDialog(hDlg);
		return TRUE;

    case WM_COMMAND:
		switch(LOWORD(wParam))
		{
        case IDAPPLY:
		case IDOK:
            // Check to see if the user (a) changed the app name or (b) changed attribute
            // state -- if so, then make the modifications now.  
            fNewHidden   = SendMessage(GetDlgItem(hDlg, IDC_FILEPROP_ATTR_HIDDEN), BM_GETCHECK, 0, 0) == BST_CHECKED;
            fNewReadOnly = SendMessage(GetDlgItem(hDlg, IDC_FILEPROP_ATTR_RO),     BM_GETCHECK, 0, 0) == BST_CHECKED;
            fOldHidden   = g_dmfa.Attributes & FILE_ATTRIBUTE_HIDDEN   ? true : false;
            fOldReadOnly = g_dmfa.Attributes & FILE_ATTRIBUTE_READONLY ? true : false;

            GetDlgItemText(hDlg, IDC_FILEPROP_EDIT_NAME, szBuf, MAX_PATH);
            if (MyStrcmp(g_dmfa.Name, szBuf))
            {
                char szOrig[MAX_PATH], szNew[MAX_PATH];
                sprintf(szOrig, "%s%s", g_CurrentDir, g_dmfa.Name);
                sprintf(szNew, "%s%s", g_CurrentDir, szBuf);
                bool fRename = true;

                // User changed the name.  Read only? 
                if (g_dmfa.Attributes & FILE_ATTRIBUTE_READONLY)
                {
                    sprintf(szType, "Are you sure you want to rename the read-only file '%s' to '%s'?", g_dmfa.Name, szBuf);
                    if (MessageBox(hDlg, szType, "Confirm File Rename", MB_ICONWARNING | MB_YESNO) == IDYES)
                    {
                        // Remove the read only bit
                        xbfu.RemoveAttribute(szOrig, FILE_ATTRIBUTE_READONLY, true);
                        xbfu.Ren(szOrig, szNew);
                        xbfu.SetAttribute(szNew, FILE_ATTRIBUTE_READONLY, true);
                    }
                    else
                    {
                        SetDlgItemText(hDlg, IDC_FILEPROP_EDIT_NAME, g_dmfa.Name);
                        fRename = false;
                    }
                }
                else
                    xbfu.Ren(szOrig, szNew);
                if (fRename)
                {
                    strcpy(g_dmfa.Name, szBuf);
                    listview.SortListView(-1, false);
                }
            }

            if ((fNewHidden && !fOldHidden) || (fOldHidden && !fNewHidden) ||
                (fNewReadOnly && !fOldReadOnly) || (fOldReadOnly && !fNewReadOnly))
            {
                // user swapped the hidden and/or readonly states
                g_dmfa.Attributes = (fNewHidden   ? FILE_ATTRIBUTE_HIDDEN   : 0) |
                                  (fNewReadOnly ? FILE_ATTRIBUTE_READONLY : 0);

                wsprintf(szBuf, "%s%s", g_CurrentDir, g_dmfa.Name);
                xbfu.SetFileAttributes(szBuf, &g_dmfa);
                listview.SortListView(-1, false);
            }

            if (LOWORD(wParam) == IDOK)
            {
                // We're done
			    EndDialog(hDlg, TRUE);
			    return TRUE;
            }
            break;

		case IDCANCEL:
			EndDialog(hDlg, TRUE);
			return TRUE;
		}
		break;
	}
	return FALSE;
}
