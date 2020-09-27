//
//	SCMPX MPEG/Audio Player.
//
//	Implement: Playlist
//	Author: Shinji Chiba 1997-4-13
//

#include "stdafx.h"

//bool g_fCut = false;
char g_szCopyDir[MAX_PATH];

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void DebugOutput(char *szMsg, ...)
{
    char szMsgOut[1024];
    va_list valist;
    
    // Create the final formatted string.
    va_start (valist, szMsg);
    vsprintf (szMsgOut, szMsg, valist);
    OutputDebugString(szMsgOut);
    va_end (valist);
}

LISTVIEW::LISTVIEW()
{
	hListWnd = NULL;
	fHaveTimeZone = FALSE;
	sortreverse = FALSE;
	viewmode = LVS_REPORT;
	columnSize[COLUMN_NAME] = 160;
	columnSize[COLUMN_SIZE] = 80;
	columnSize[COLUMN_TYPE] = 120;
	columnSize[COLUMN_MODIFIED] = 110;
    m_pdroptarget = NULL;
    m_nDragHighlightItem = ITEM_NONE;

    // Load the column name strings
    LoadString(hInst, IDS_COLUMN_NAME, cszColumn[0], MAX_PATH);
    LoadString(hInst, IDS_COLUMN_SIZE, cszColumn[1], MAX_PATH);
    LoadString(hInst, IDS_COLUMN_TYPE, cszColumn[2], MAX_PATH);
    LoadString(hInst, IDS_COLUMN_MODIFIED, cszColumn[3], MAX_PATH);
}

LISTVIEW::~LISTVIEW()
{
    if (m_pdroptarget)
        m_pdroptarget->Release();
}

BOOL LISTVIEW::FileTime2LocalTime(LPFILETIME lpft, LPSYSTEMTIME lpst)
{
	SYSTEMTIME stZ;

	if (! FileTimeToSystemTime(lpft, &stZ)) return FALSE;
	if (! fHaveTimeZone) fHaveTimeZone = GetTimeZoneInformation(&g_tzi) != TIME_ZONE_ID_UNKNOWN;
	if (! fHaveTimeZone || ! SystemTimeToTzSpecificLocalTime(&g_tzi, &stZ, lpst)) *lpst = stZ;
	return TRUE;
}

HWND LISTVIEW::GetHwnd()
{
	return hListWnd;
}

HWND LISTVIEW::CreateListView(HWND hWndParent, LPRECT lprc/* = NULL */)
{
	RECT rcl;
	int index;
	LV_COLUMN lvC;
	char szText[32];
	INITCOMMONCONTROLSEX icex;

	icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
	icex.dwICC = ICC_LISTVIEW_CLASSES;
	InitCommonControlsEx(&icex);

	if (lprc) CopyMemory(&rcl, lprc, sizeof(RECT));
	else GetClientRect(hWndParent, &rcl);
	hListWnd = CreateWindow(WC_LISTVIEW, "", viewmode | (WS_BORDER | WS_VISIBLE | WS_CHILD | LVS_AUTOARRANGE | LVS_EDITLABELS | LVS_SHAREIMAGELISTS | LVS_SHOWSELALWAYS), rcl.left, rcl.top, rcl.right - rcl.left, rcl.bottom - rcl.top, hWndParent, (HMENU) 1, hInst, NULL);
	if (! hListWnd) return NULL;
     
	lvC.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvC.pszText = szText;
	for (index = 0; index < N_COLUMN; index++)
	{
		lvC.cx = columnSize[index];
		if (index == COLUMN_SIZE) lvC.fmt = LVCFMT_RIGHT;
		else lvC.fmt = LVCFMT_LEFT;

		lvC.iSubItem = index;
		wsprintf(szText, cszColumn[index]);
		if (ListView_InsertColumn(hListWnd, index, &lvC) == -1) return NULL;
	}

	HIMAGELIST hil;
	SHFILEINFO sfi;
	ZeroMemory(&sfi, sizeof(sfi));
	hil = (HIMAGELIST) SHGetFileInfo(".\\", 0, &sfi, sizeof(sfi), SHGFI_SYSICONINDEX | SHGFI_USEFILEATTRIBUTES | SHGFI_ICON | SHGFI_SMALLICON | SHGFI_TYPENAME);
	ListView_SetImageList(hListWnd, hil, LVSIL_SMALL);
	//ListView_SetExtendedListViewStyle(hListWnd, LVS_EX_FULLROWSELECT);

	hil = (HIMAGELIST) SHGetFileInfo(".\\", 0, &sfi, sizeof(sfi), SHGFI_SYSICONINDEX | SHGFI_USEFILEATTRIBUTES | SHGFI_ICON | SHGFI_TYPENAME);
	ListView_SetImageList(hListWnd, hil, LVSIL_NORMAL);
    
//	if (! InsertFiles()) return NULL;

    // Register ourself as a DropTarget (ie things can be dragged/dropped onto us.
    m_pdroptarget = new CDropTarget();

    IDropTarget *pidroptarget; 
    if (FAILED(m_pdroptarget->QueryInterface(IID_IDropTarget, (LPVOID*)&pidroptarget)))
        return false;

    RegisterDragDrop(GetHwnd(), pidroptarget);

	return hListWnd;
}
int StrToN(char *sz)
{
    int n = 0;
    while (*sz)
    {
        if (*sz >= '0' && *sz <= '9')
            n = n * 10 + *sz - '0';
        sz++;
    }
    return n;
}

int CompTime(char *szDate1, char *szDate2)
{
    int nYear1, nMonth1, nDate1, nHour1, nMinute1;
    int nYear2, nMonth2, nDate2, nHour2, nMinute2;
    sscanf(szDate1, "%04d/%02d/%02d %02d:%02d", &nYear1, &nMonth1, &nDate1, &nHour1, &nMinute1);
    sscanf(szDate2, "%04d/%02d/%02d %02d:%02d", &nYear2, &nMonth2, &nDate2, &nHour2, &nMinute2);

    if (nYear1 > nYear2) return 1;
    else if (nYear2 > nYear1) return -1;
    if (nMonth1 > nMonth2) return 1;
    else if (nMonth2 > nMonth1) return -1;
    if (nDate1 > nDate2) return 1;
    else if (nDate2 > nDate1) return -1;
    if (nHour1 > nHour2) return 1;
    else if (nHour2 > nHour1) return -1;
    if (nMinute1 > nMinute2) return 1;
    else if (nMinute2 > nMinute1) return -1;
   
    return 0;
}

// ListViewCompareFunc - sorts the list view control. It is a 
//     comparison function. 
// Returns a negative value if the first item should precede the 
//     second item, a positive value if the first item should 
//     follow the second item, and zero if the items are equivalent. 
// lParam1 and lParam2 - item data for the two items (in this 
//     case, pointers to application-defined MYITEM structures). 
// lParamSort - value specified by the LVM_SORTITEMS message 
//     (in this case, the index of the column to sort). 

bool m_fInvert = false;
int CALLBACK ListViewCompareProc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
    char sz1[MAX_PATH], sz2[MAX_PATH], szTemp[MAX_PATH], szTemp2[MAX_PATH];
    bool fDir1, fDir2;

    listview.m_nLastSort = lParamSort;

    int nRet;
    switch(lParamSort)
    {
    case 0:     // Sort by name.
        // Get the file names
        ListView_GetItemText(listview.GetHwnd(), lParam1, COLUMN_NAME, sz1, MAX_PATH);
        ListView_GetItemText(listview.GetHwnd(), lParam2, COLUMN_NAME, sz2, MAX_PATH);
        ListView_GetItemText(listview.GetHwnd(), lParam1, COLUMN_SIZE, szTemp, MAX_PATH);
        fDir1 = !MyStrcmp(szTemp, CHAR_DIR);
        ListView_GetItemText(listview.GetHwnd(), lParam2, COLUMN_SIZE, szTemp, MAX_PATH);
        fDir2 = !MyStrcmp(szTemp, CHAR_DIR);

        // '..' always comes first
        if (!MyStrcmp(sz1, ".."))
        { if (m_fInvert) nRet = 1; else nRet = -1; }
        else if (!MyStrcmp(sz2, ".."))
        { if (m_fInvert) nRet = -1; else nRet = 1; }
        // Directories come before files
        else if (fDir1 && !fDir2)
            nRet = -1;
        else if (fDir2 && !fDir1)
            nRet = 1;
        else
            nRet = MyStricmp(sz1, sz2);
        break;
        
    case 1:     // Sort by size.
        ListView_GetItemText(listview.GetHwnd(), lParam1, COLUMN_NAME, sz1, MAX_PATH);
        ListView_GetItemText(listview.GetHwnd(), lParam2, COLUMN_NAME, sz2, MAX_PATH);
        ListView_GetItemText(listview.GetHwnd(), lParam1, COLUMN_SIZE, szTemp, MAX_PATH);
        fDir1 = !MyStrcmp(szTemp, CHAR_DIR);
        ListView_GetItemText(listview.GetHwnd(), lParam2, COLUMN_SIZE, szTemp2, MAX_PATH);
        fDir2 = !MyStrcmp(szTemp, CHAR_DIR);

        // Directories come before files
        // '..' always comes first
        if (!MyStrcmp(sz1, ".."))
        { if (m_fInvert) nRet = 1; else nRet = -1; }
        else if (!MyStrcmp(sz2, ".."))
        { if (m_fInvert) nRet = -1; else nRet = 1; }
        else if (fDir1 && !fDir2)
            nRet = -1;
        else if (fDir2 && !fDir1)
            nRet = 1;
        else
        // Convert size strings into numbers and compare them...
        {
            int n1 = StrToN(szTemp);
            int n2 = StrToN(szTemp2);
            nRet = n1 > n2 ? 1 : -1;
        }
        break;

    case 2:     // Sort by type.
        ListView_GetItemText(listview.GetHwnd(), lParam1, COLUMN_NAME, sz1, MAX_PATH);
        ListView_GetItemText(listview.GetHwnd(), lParam2, COLUMN_NAME, sz2, MAX_PATH);
        
        if (!MyStrcmp(sz1, ".."))
        { if (m_fInvert) nRet = 1; else nRet = -1; }
        else if (!MyStrcmp(sz2, ".."))
        { if (m_fInvert) nRet = -1; else nRet = 1; }
        else
        {
            ListView_GetItemText(listview.GetHwnd(), lParam1, COLUMN_TYPE, sz1, MAX_PATH);
            ListView_GetItemText(listview.GetHwnd(), lParam2, COLUMN_TYPE, sz2, MAX_PATH);
            nRet = MyStricmp(sz1, sz2);
        }
        break;

    case 3:     // Sort by date
        ListView_GetItemText(listview.GetHwnd(), lParam1, COLUMN_NAME, sz1, MAX_PATH);
        ListView_GetItemText(listview.GetHwnd(), lParam2, COLUMN_NAME, sz2, MAX_PATH);
        
        if (!MyStrcmp(sz1, ".."))
        { if (m_fInvert) nRet = 1; else nRet = -1; }
        else if (!MyStrcmp(sz2, ".."))
        { if (m_fInvert) nRet = -1; else nRet = 1; }
        else
        {
            ListView_GetItemText(listview.GetHwnd(), lParam1, COLUMN_MODIFIED, sz1, MAX_PATH);
            ListView_GetItemText(listview.GetHwnd(), lParam2, COLUMN_MODIFIED, sz2, MAX_PATH);
            nRet = CompTime(sz1, sz2);
        }
        break;

    default:
        nRet = 0;
        break;
    }

    if (m_fInvert)
        return -nRet;
    else
        return nRet;
}	

void SetlParam(int i)
{
    LV_ITEM lvI = {0};

    lvI.mask = LVIF_PARAM;
    lvI.iItem = i;
    lvI.iSubItem = 0;

    ListView_GetItem(listview.GetHwnd(), &lvI);
    lvI.lParam = i;
    ListView_SetItem(listview.GetHwnd(), &lvI);
}

BOOL LISTVIEW::InsertFiles(int sortcolumn /* = 0 */, BOOL updown/* = FALSE */)
{
	LV_ITEM lvI;
	int index=0;
	if (! hListWnd) return FALSE;

#if 0
    // Only add '..' if we're not at the root
    if (g_CurrentDir[4] != '\0')
    {
        LVITEM item;

		LPCTSTR pExt;
		UINT flg;
		SHFILEINFO sfi;
		ZeroMemory(&sfi, sizeof(sfi));
		pExt = "*";
		flg = SHGFI_ICON | SHGFI_SMALLICON | SHGFI_USEFILEATTRIBUTES ;
		SHGetFileInfo(pExt, FILE_ATTRIBUTE_DIRECTORY, &sfi, sizeof(sfi), flg);
		item.iImage = sfi.iIcon;
		item.iImage = 0;
        item.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_STATE | LVIF_PARAM;
		item.state = 0;
		item.stateMask = 0;
		item.iSubItem = 0;
        item.iItem = 1;
        item.lParam = 0;
		item.pszText = "..";
		index = ListView_InsertItem(hListWnd, &item);
	}
#endif
    
    char szBinary[100], szBadDate[100];
    LoadString(hInst, IDS_BINARY, szBinary, MAX_PATH);
    LoadString(hInst, IDS_BAD_DATE, szBadDate, MAX_PATH);

	DM_FILE_ATTRIBUTES fa;
    bool fXbox = (*g_CurrentDir == 'x' || *g_CurrentDir == 'X');
	HANDLE hFind = xbfu.FindFirstFile(g_CurrentDir, &fa, fXbox, g_TargetXbox);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		do {
			char isdir[8];
			ZeroMemory(isdir, 8);

            SHFILEINFO sfi;
			ZeroMemory(&sfi, sizeof(sfi));
			LPCTSTR pExt;
			UINT flg;
			if (fa.Attributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				pExt = fa.Name;
        		flg = SHGFI_ICON | SHGFI_SMALLICON | SHGFI_USEFILEATTRIBUTES | SHGFI_TYPENAME ;
//				flg = SHGFI_SYSICONINDEX | SHGFI_ICON | SHGFI_SMALLICON | SHGFI_TYPENAME;
			}
			else
			{
				pExt = fa.Name;
				flg = SHGFI_SYSICONINDEX | SHGFI_USEFILEATTRIBUTES | SHGFI_ICON | SHGFI_SMALLICON | SHGFI_TYPENAME;
			}
			SHGetFileInfo(pExt, fa.Attributes & FILE_ATTRIBUTE_DIRECTORY, &sfi, sizeof(sfi), flg);
			lvI.iImage = sfi.iIcon;

			lvI.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_STATE | LVIF_PARAM;
			lvI.state = 0;
			lvI.stateMask = 0;
			lvI.iItem = 0;
			lvI.iSubItem = 0;
			lvI.pszText = fa.Name;
			if (fa.Attributes & FILE_ATTRIBUTE_HIDDEN)
			{
				lvI.mask |= LVIF_STATE;
				lvI.stateMask |= LVIS_CUT;
				lvI.state |= LVIS_CUT;
			}

            if ((index = ListView_InsertItem(hListWnd, &lvI)) == -1) return FALSE;

			if (! (fa.Attributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				char szSize[32];
				int ichReal = 0;
				int ich = 0;
				DWORD dwSize = fa.SizeLow;
				double fSize = (double) dwSize;

				while(dwSize)
				{
					if (ich && (ich % 3) == 0) ichReal++;
					ichReal++;
					ich++;
					dwSize /= 10;
				}

				ich = 0;
				dwSize = fa.SizeLow;
				if (dwSize == 0)
				{
					szSize[0] = '0';
					szSize[1] = '\0';
				}
				else
				{
					szSize[ichReal] = '\0';
					while(ichReal-- && dwSize)
					{
						if (ich && (ich % 3) == 0) szSize[ichReal--] = ',';
						szSize[ichReal] = (char) (dwSize % 10) + '0';
						dwSize /= 10;
						++ich;
					}
					while(ichReal >= 0) szSize[ichReal--] = ' ';
				}

				lvI.mask = LVIF_TEXT;
				lvI.iItem = index;
				lvI.iSubItem = COLUMN_SIZE;
				lvI.pszText = szSize;
				if (ListView_SetItem(hListWnd, &lvI) == -1) return NULL;
			}
			else
			{
				lvI.mask = LVIF_TEXT;
				lvI.iItem = index;
				lvI.iSubItem = COLUMN_SIZE;
				lvI.pszText = CHAR_DIR;
				if (ListView_SetItem(hListWnd, &lvI) == -1) return NULL;
			}
			lvI.iSubItem = COLUMN_TYPE;
			lvI.pszText = sfi.szTypeName;

            if (sfi.szTypeName[0] == '\0') lstrcpy(sfi.szTypeName, szBinary);
			if (ListView_SetItem(hListWnd, &lvI) == -1) return NULL;
			{
				char szTime[32];
				SYSTEMTIME st;
				if (FileTime2LocalTime(&fa.ChangeTime, &st)) wsprintf(szTime, "%04d/%02d/%02d  %02d:%02d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute);
				else wsprintf(szTime, "%-18.18s", szBadDate);
				lvI.iSubItem = COLUMN_MODIFIED;
				lvI.pszText = szTime;
				if (ListView_SetItem(hListWnd, &lvI) == -1) return NULL;
			}
		} while(xbfu.FindNextFile(hFind, &fa, fXbox));
		xbfu.FindClose(hFind, fXbox);
	}
    else
    {
        if (xbfu.FailedConnect())
        {
            // Failed to connect to current xbox.  reset the failed flag
            xbfu.SetFailed();
            return FALSE;
        }
    }
	int items = ListView_GetItemCount(hListWnd);
	for (int nItem = 0; nItem < items; nItem++)
	{
        SetlParam(nItem);
	}

    return TRUE;
}

HWND LISTVIEW::RedrawListView(HWND hWndParent, LPRECT lprc/* = NULL */)
{
	RECT rcl;

	if (lprc) CopyMemory(&rcl, lprc, sizeof(RECT));
	else GetClientRect(hWndParent, &rcl);
	SetWindowPos(hListWnd, NULL, rcl.left, rcl.top, rcl.right - rcl.left, rcl.bottom - rcl.top, SWP_NOZORDER | SWP_SHOWWINDOW);
	return hListWnd;
}

char *g_rgszSelectedFiles[1000];
int m_cSelected =0;

void SaveSelectedItems()
{
	char str[512] = "";
	int nItem = -1;
    m_cSelected = 0;
	while((nItem = ListView_GetNextItem(listview.GetHwnd(), nItem, LVNI_ALL | LVNI_SELECTED)) > -1)
	{
		ListView_GetItemText(listview.GetHwnd(), nItem, COLUMN_NAME, str, 512);
        g_rgszSelectedFiles[m_cSelected] = new char[MyStrlen(str) + 1];
        strcpy(g_rgszSelectedFiles[m_cSelected], str);
        m_cSelected++;
	}
}

void SetSelected(char *szItemName)
{
    if (szItemName)
    {
        // Find the item, then set it's state to selected
        LVFINDINFO lvfi;
    
        lvfi.flags = LVFI_STRING;
        lvfi.psz = szItemName;
        int i = ListView_FindItem(listview.GetHwnd(), -1, &lvfi);
        if (i == -1)
            return;

        ListView_SetItemState(listview.GetHwnd(), i, LVIS_SELECTED , LVIS_SELECTED );
    }
    else
        ListView_SetItemState(listview.GetHwnd(), -1, 0, LVIS_SELECTED );
}
        

void RestoreSelectedItems()
{
    for (int i = 0; i < m_cSelected; i++)
    {
        SetSelected(g_rgszSelectedFiles[i]);
        
        delete g_rgszSelectedFiles[i];
    }
    m_cSelected = 0;
}

void DeferSelected(char *szItemName)
{
//    strcpy(g_rgszSelectedFiles[m_cSelected], szItemName);
  //  m_cSelected++;
}

void LISTVIEW::ClearSelection()
{
    SetSelected(NULL);
}
extern void TotalFiles();
bool LISTVIEW::SortListView(int sortcolumn, BOOL fColumnclick)
{
    char szBuf[MAX_PATH];
    wsprintf(szBuf, "xbExplorer - %s: %s", g_TargetXbox, g_CurrentDir);
    SetWindowText(g_hwnd, szBuf);

    SaveSelectedItems();

    ListView_DeleteAllItems(hListWnd);

    // if this was from a column click, and we're setting the same column, then invert the ordering
    bool updown =false;
    if (fColumnclick && sortcolumn == m_nLastSort)
        m_fInvert = !m_fInvert;
	if (!InsertFiles(sortcolumn, updown))
    {
        // Failed to insert files
        return false;
    }

    if (sortcolumn == -1)
        sortcolumn = m_nLastSort;
    
    ListView_SortItems(GetHwnd(), ListViewCompareProc, sortcolumn);

    RestoreSelectedItems();
    
	Focus();
	TotalFiles();
    return true;
}

void LISTVIEW::Clear()
{
    ListView_DeleteAllItems(hListWnd);
    SetWindowText(g_hwnd, "xbExplorer - not connected");
}

void LISTVIEW::ChangeListView(DWORD dwView)
{
	DWORD dwStyle;

	dwStyle = GetWindowLong(hListWnd, GWL_STYLE);

	if ((dwStyle & LVS_TYPEMASK) != dwView)
	{
		SetWindowLong(hListWnd, GWL_STYLE, (dwStyle & ~LVS_TYPEMASK) | dwView);
	}
}

void LISTVIEW::CheckFiles(int *totalfolders, int *totalfiles, int *totalsize)
{
	char str[SHORTFILELEN * 2] = "";
	int i, items, nItem, len;

	*totalfiles = *totalsize = *totalfolders = 0;
	items = ListView_GetItemCount(hListWnd);
	for (nItem = 0; nItem < items; nItem++)
	{
        
        ListView_GetItemText(hListWnd, nItem, COLUMN_SIZE, str, SHORTFILELEN * 2);
		len = MyStrlen(str);
		if (len && MyStrcmp(str, CHAR_DIR))
		{
			(*totalfiles)++;
			for (i = 0; i < len; i++)
			{
				if (str[i] == '\0') break;
				else if (str[i] == ',')
				{
					str[i] = '\0';
					strcat(str, &str[i] + 1);
				}
			}
			(*totalsize) += atoi(str);
		}
		else
		{
			ListView_GetItemText(hListWnd, nItem, COLUMN_NAME, str, SHORTFILELEN * 2);
			if (MyStrcmp(str, "..")) (*totalfolders)++;
		}
	}
}

BOOL LISTVIEW::CheckSelectedFiles(int *totalfolders, int *totalfiles, int *totalsize)
{
	char str[512] = "";
	BOOL parentfolder = FALSE;
	int i, nItem, len;

	nItem = -1;
    *totalfiles = *totalsize = *totalfolders = 0;
	while((nItem = ListView_GetNextItem(hListWnd, nItem, LVNI_ALL | LVNI_SELECTED)) > -1)
	{
		ListView_GetItemText(hListWnd, nItem, COLUMN_SIZE, str, 512);
		len = MyStrlen(str);
		if (len && MyStrcmp(str, CHAR_DIR))
		{
			(*totalfiles)++;
			for (i = 0; i < len; i++)
			{
				if (str[i] == '\0') break;
				else if (str[i] == ',')
				{
					str[i] = '\0';
					strcat(str, &str[i] + 1);
				}
			}
			(*totalsize) += atoi(str);
		}
		else
		{
			ListView_GetItemText(hListWnd, nItem, COLUMN_NAME, str, 512);
			if (MyStrcmp(str, "..")) (*totalfolders)++;
			else parentfolder = TRUE;
		}
	}
	return parentfolder;
}

int LISTVIEW::GetNumSelected()
{
	int nSelected, nItem;

	nSelected = 0;
    nItem = -1;
	while((nItem = ListView_GetNextItem(hListWnd, nItem, LVNI_ALL | LVNI_SELECTED)) > -1)
        nSelected++;

    return nSelected;
}
BOOL LISTVIEW::IsParentFolder(int iItem)
{
    return FALSE;
	char str[256];
	ListView_GetItemText(hListWnd, iItem, COLUMN_NAME, str, 256);
	if (! MyStrcmp(str, "..")) return TRUE;
	return FALSE;
}

BOOL LISTVIEW::IsFolder(int iItem)
{
	char str[256];
	ListView_GetItemText(hListWnd, iItem, COLUMN_SIZE, str, 256);
	if (! MyStrcmp(str, CHAR_DIR)) return TRUE;
	return FALSE;
}

int LISTVIEW::GetItemAtPoint(POINT *ppt)
{
    RECT rc;
    
	int nItem = ListView_GetItemCount(hListWnd);
    for (int i = 0; i < nItem; i++)
    {
        ListView_GetItemRect(hListWnd, i, &rc, LVIR_BOUNDS);
        if (PtInRect(&rc, *ppt))
            return i;   // Just return first one we're over.
    }

    return ITEM_NONE;
}

bool LISTVIEW::SetDragHighlight(bool fEnable, POINT *pptScreen)
{
    int nItem;
    RECT rc;

    // First, determine whether or not we want to set the highlight
    if (fEnable)
    {
        // Convert the screen coordinate into list coordinate
        ScreenToClient(g_hwnd, pptScreen);

        // Offset by the toolbar
		GetWindowRect(toolview.GethWnd(), &rc);

        pptScreen->y -= (rc.bottom - rc.top);
        
        // Check if the specified cursor point is over a directory in the list; if so, then
        // we need to hilight the directory and store it's name so that we know to copy into it

        nItem = GetItemAtPoint(pptScreen);
        if (!IsFolder(nItem))
            nItem = ITEM_NONE; 
    }
    else
        nItem = ITEM_NONE;

    // Determine if we need to remove the old highlight
    if (nItem != m_nDragHighlightItem && m_nDragHighlightItem != ITEM_NONE)
    {
        // A different item (or NO item) is being highlighted; unhighlight the previously highlighted
        // item.  Note: only unhighlight the item if it was not already selected (ie part of the 
        // drag list).
        ListView_SetItemState(hListWnd, m_nDragHighlightItem, 0, LVIS_DROPHILITED);
        m_nDragHighlightItem = ITEM_NONE;
    }

    // Finally, add a new highlight if appropriate
    if (nItem != ITEM_NONE)
    {
        ListView_SetItemState(hListWnd, nItem, LVIS_DROPHILITED, LVIS_DROPHILITED);
        m_nDragHighlightItem = nItem;

        return true;
    }

    return false;
}

//return true ONLY if current drag item == directory
bool LISTVIEW::GetDragItemName(char szName[MAX_PATH])
{
    // if no item selected, then return false
    if (m_nDragHighlightItem == ITEM_NONE)
        return false;

    if (!IsFolder(m_nDragHighlightItem))
        return false;

    // It's a folder - grab its name
    ListView_GetItemText(hListWnd, m_nDragHighlightItem, COLUMN_NAME, szName, MAX_PATH);

    return true;
}

void LISTVIEW::Focus()
{
//	ListView_SetItemState(hListWnd, 0, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
	ListView_EnsureVisible(hListWnd, 0, TRUE);
	SetFocus(hListWnd);
}

BOOL LISTVIEW::IsSelected(int nItem)
{
	return (BOOL) ListView_GetItemState(hListWnd, nItem, LVIS_SELECTED);
}

int LISTVIEW::IsExistence(char* str)
{
	char name[SHORTFILELEN] = "";
	int nItem, items;
	items = ListView_GetItemCount(hListWnd);
	for (nItem = 0; nItem < items; nItem++)
	{
		ListView_GetItemText(hListWnd, nItem, COLUMN_NAME, name, SHORTFILELEN);
		if (! MyStricmp(name, str)) return nItem;
	}
	return -1;
}

void LISTVIEW::ChangeView(DWORD dwView)
{
	DWORD dwStyle;

	dwStyle = GetWindowLong(hListWnd, GWL_STYLE);

    SetWindowLong (hListWnd, GWL_STYLE, (dwStyle & ~LVS_TYPEMASK) | dwView);
	viewmode = dwView;
}

DWORD LISTVIEW::GetViewMode()
{
	return viewmode;
}

bool LISTVIEW::PopulateWithSelected(CFileContainer *pfc)
{
    // Enumerate the list of selected items and add them to the specified file container
	char szFile[512] = "";
	int nItem;
    char szBuf[MAX_PATH];
    bool fFiles = false;
	nItem = -1;
	while((nItem = ListView_GetNextItem(hListWnd, nItem, LVNI_ALL | LVNI_SELECTED)) > -1)
	{
        ListView_GetItemText(hListWnd, nItem, COLUMN_NAME, szFile, 512);
        wsprintf(szBuf, "%s%s", g_CurrentDir, szFile);
        ListView_GetItemText(listview.GetHwnd(), nItem, COLUMN_SIZE, szFile, MAX_PATH);
        if (!MyStrcmp(szFile, CHAR_DIR))
            pfc->AddDir(szBuf);
        else
            pfc->AddFile(szBuf);
        fFiles = true;
	}

    return fFiles;
}

void LISTVIEW::Delete(HWND hwndOwner)
{
    bool fDelete = (hwndOwner == NULL);

    // Logic flow:
    //  If the user selected one file
    //      if the file is read only
    //          display the 'this file is read only' dialog.
    //      else
    //          display the regular 'delete' dialog.
    //  else
    //      display the regular 'delete' dialog
    //      for each file
    //          if the file is read only
    //              display the 'this file is read only' dialog.

	char str[MAX_PATH], szFile[MAX_PATH];
	int i, count;
    bool fReadOnly = false;
    DM_FILE_ATTRIBUTES dmfa;
    int cFiles = 0;

    // Check if any of the selected files are read-only
    count = ListView_GetItemCount(hListWnd);
	for (i = 0; i < count; i++)
	{
		if (listview.IsSelected(i))
		{
			ListView_GetItemText(hListWnd, i, COLUMN_NAME, str, MAX_PATH);

            // Check if the file is a readonly file
            wsprintf(szFile, "%s%s", g_CurrentDir, str);
            xbfu.GetFileAttributes(szFile, &dmfa);
            if (dmfa.Attributes & FILE_ATTRIBUTE_READONLY)
            {
                fReadOnly = true;
                break;
            }
		}
	}

    if (hwndOwner)
    {
        // Ask the user if they want to actually delete the files
        int totalfolders, totalfiles, totalsize;
	    listview.CheckSelectedFiles(&totalfolders, &totalfiles, &totalsize);
	    if (totalfolders || totalfiles)
	    {
            if (totalfiles + totalfolders > 1 || !fReadOnly)
            {
                // More than one file/folder specified, or none of them were read only.
                // display the 'deletion confirmation' dialog
		        char str[128];
		        strcpy(str, "Delete ");
		        if (totalfolders)
		        {
			        wsprintf(str, "%s%d folder", str, totalfolders);
			        if (totalfolders > 1) strcat(str, "s");
			        if (totalfiles) strcat(str, " and ");
		        }
		        if (totalfiles)
		        {
			        wsprintf(str, "%s%d file", str, totalfiles);
			        if (totalfiles > 1) strcat(str, "s");
		        }
		        strcat(str, "?");
		        if (MessageBox(hwndOwner, str, szAppName, MB_YESNO) == IDYES)
                    fDelete = true;
            }
            else
                fDelete = true;
        }
    }
    
    if (fDelete)
    {
		char str[MAX_PATH];
		int i, count;
        bool fQuery = true;
		count = ListView_GetItemCount(hListWnd);
		for (i = 0; i < count; i++)
		{
			if (listview.IsSelected(i))
			{
				ListView_GetItemText(hListWnd, i, COLUMN_NAME, str, MAX_PATH);
                if (MyStrcmp(str, ".."))
                {
                    sprintf(szFile, "%s%s", g_CurrentDir, str);
                    if (!xbfu.Del(szFile, &fQuery, GetHwnd()))
                        break;
                }
            }
		}
    	SortListView(-1, FALSE);
    }

    // Clear the clipboard when done
    if (m_fCut)
        OleSetClipboard(NULL);
}

bool g_fForceWaitCursor = false;

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  LISTVIEW::CutCopy
// Purpose:   
// Arguments: UNDONE-WARN: Fill in
// Return:    None
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void LISTVIEW::CutCopy(bool fCut)
{
    // Create the IDataObject that contains information about the files being copied
    CFileContainer *pfc = new CFileContainer;

    g_fForceWaitCursor = true;
    SetCursor(LoadCursor(NULL, IDC_WAIT));

    // Populate the new file container with the files that are selected in us.
    bool fFiles = PopulateWithSelected(pfc);
 
    SetCursor(LoadCursor(NULL, IDC_ARROW));

    g_fForceWaitCursor = false;

    if (!fFiles)
    {
        OleSetClipboard(NULL);
        return;
    }

    IDataObject *pidataobject; 
    if (FAILED(pfc->QueryInterface(IID_IDataObject, (LPVOID*)&pidataobject)))
        return;

    m_fCut = fCut;
    pfc->m_fCut = fCut;
    strcpy(g_szCopyDir, g_CurrentDir);
    OleSetClipboard(pidataobject);  // addrefs it.


    pidataobject->Release();
    pfc->Release();

//    __asm int 3;
    if (fCut)
    {
        LV_ITEM lvI;

        // Set the currently selected files to 'cut' icons
	    int cItems = ListView_GetItemCount(hListWnd);
	    for (int i = 0; i < cItems; i++)
	    {
		    if (listview.IsSelected(i))
		    {
	            lvI.iItem = i;
	            lvI.mask = LVIF_STATE;
	            lvI.stateMask = LVIS_CUT;
	            lvI.state = LVIS_CUT;
	            ListView_SetItem(hListWnd, &lvI);
		    }
	    }
    }
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  LISTVIEW::Paste
// Purpose:   
// Arguments: UNDONE-WARN: Fill in
// Return:    None
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void LISTVIEW::Paste()
{
    IDataObject *pidataobject; 
    STGMEDIUM stgm;
    sSourceInfo sourceinfo;
    char szBuf[512], szBuf2[100];
    bool fContinue, fSkipped;

    // UNDONE-WARN: I'm sure I could be reusing code between this and the drag/drop code.  Revisit later.
    // Get the data object from the clipboard
    OleGetClipboard(&pidataobject);
    
    // Start the thread that handles the progress dialog.  It won't actually display anything for
    // a couple of seconds to avoid displaying it for small copies...
    g_progress.StartThread();
    
    bool fQuery = true;
    GetDropSourceInfo(pidataobject, &sourceinfo);
    if (sourceinfo.fXbox)
    {
//        if (MyStrcmp(sourceinfo.szName, g_TargetXbox))
        if (g_hwnd != sourceinfo.hwnd)
        {
            // We don't allow copying from one xbExplorer to another yet
            LoadString(hInst, IDS_XTOX_COPY_NOT_ALLOWED, szBuf, 512);
            LoadString(hInst, IDS_UNSUPPORTED_FEATURE, szBuf2, 100);

            MessageBox(g_hwnd, szBuf, szBuf2, MB_ICONWARNING | MB_OK);
            goto done;
        }

        // Xbox->Xbox (same) copy.
        FORMATETC fmte= {(USHORT)g_suXboxFiles,  NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL};
        if (pidataobject->GetData(&fmte, &stgm) == S_OK)
        {
            char szDestFilePath[MAX_PATH];

            // Get the list of filenames
            DROPFILES *pdf = (DROPFILES*)GlobalLock(stgm.hGlobal);
            char *szNextSourcePathFile = (char*)(pdf) + pdf->pFiles;

            while (*szNextSourcePathFile)
            {
                char *pszSource;

                if (pdf->fWide)
                {
                    // Convert from wide to single byte
                    pszSource = WideToSingleByte((WCHAR*)szNextSourcePathFile);
                }
                else
                    pszSource = szNextSourcePathFile;

                wsprintf(szDestFilePath, "%s%s", g_CurrentDir, pszSource + MyStrlen(g_szCopyDir));

                // Perform the actual copy.
                // NOTE: This code is only called in the Xbox->Xbox case.
                fContinue = (xbfu.CopyMoveToXbox(pszSource, szDestFilePath, &fQuery, true, true,
                                                 true, m_fCut, &fSkipped) == S_OK);
                if (!fContinue)
                    break;

				if (!fSkipped && m_fCut)
				{
					bool f = false;
					xbfu.Del(pszSource, &f, g_hwnd);
				}

                DeferSelected(strrchr(szDestFilePath, '\\') + 1);

                if (pdf->fWide)
                    szNextSourcePathFile += (MyStrlen(pszSource)+1)*2;
                else
                    szNextSourcePathFile += MyStrlen(pszSource)+1;
            }

            szNextSourcePathFile = (char*)(pdf) + pdf->pFiles;

            GlobalUnlock(stgm.hGlobal);
        }
        else
        {
            // UNDONE-ERR: Handle error case
        }
    }
    else
    {
        // PC->Xbox copy.
        // Request the contents of the file.  Because it's coming from the PC, we
        // can just request it as an HDROP and use the names to copy the files using
        // XboxDbg.  There's currently no way to stream directly to the Xbox.
        bool fCut;

        // Check if the source was a 'cut' command
        FORMATETC fmte= {(USHORT)g_suPrefDropEffect,  NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL};
        if (pidataobject->GetData(&fmte, &stgm) == S_OK)
        {
            DWORD *pdwEffect = (DWORD*)GlobalLock(stgm.hGlobal);
            fCut = (*pdwEffect == DROPEFFECT_MOVE);
            GlobalUnlock(stgm.hGlobal);
        }
        else
            fCut = false;

       
        fmte.cfFormat = (USHORT)CF_HDROP;
        if (pidataobject->GetData(&fmte, &stgm) == S_OK)
        {
            char szDestFilePath[MAX_PATH];

            // Get the list of filenames
            DROPFILES *pdf = (DROPFILES*)GlobalLock(stgm.hGlobal);
            char *szNextSourcePathFile = (char*)(pdf) + pdf->pFiles;
            while (*szNextSourcePathFile)
            {
                char *pszSource;

                if (pdf->fWide)
                {
                    // Convert from wide to single byte
                    pszSource = WideToSingleByte((WCHAR*)szNextSourcePathFile);
                }
                else
                    pszSource = szNextSourcePathFile;

                wsprintf(szDestFilePath, "%s%s", g_CurrentDir, strrchr(pszSource,'\\') + 1);

                // Perform the actual copy.  The 'Copy' command will take care of directories auto-
                // matically (ie creating and recursively copying them).
                // NOTE: This code is only called in the Xbox->Xbox or PC->Xbox cases.
            	fContinue = (xbfu.CopyMoveToXbox(pszSource, szDestFilePath, &fQuery, true, sourceinfo.fXbox, true, fCut, &fSkipped) == S_OK);
                if (!fContinue)
                    break;

                DeferSelected(strrchr(szDestFilePath, '\\') + 1);
            
                if (pdf->fWide)
                    szNextSourcePathFile += (MyStrlen(pszSource)+1)*2;
                else
                    szNextSourcePathFile += MyStrlen(pszSource)+1;
            }

            GlobalUnlock(stgm.hGlobal);

            if (fCut)
            {
                // Source was a cut.  inform it that the paste was done.
                fmte.cfFormat = (USHORT)g_suPerfDropEffect;
                stgm.pUnkForRelease = 0;
                stgm.hGlobal = GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT | GMEM_SHARE, 12);
                if (!stgm.hGlobal)
                    goto done;
                stgm.tymed = TYMED_HGLOBAL;
                DWORD *pdwEffect = (DWORD*)GlobalLock(stgm.hGlobal);
                *pdwEffect = DROPEFFECT_MOVE;
                GlobalUnlock(stgm.hGlobal);
                pidataobject->SetData(&fmte, &stgm, true);

                // Inform the caller that the paste succeeded
                fmte.cfFormat = (USHORT)g_suPasteSucceeded;

                stgm.hGlobal = GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT | GMEM_SHARE, 12);
                if (!stgm.hGlobal)
                    goto done;
                stgm.tymed = TYMED_HGLOBAL;
                pdwEffect = (DWORD*)GlobalLock(stgm.hGlobal);
                *pdwEffect = fSkipped ? DROPEFFECT_NONE : DROPEFFECT_MOVE;
                GlobalUnlock(stgm.hGlobal);
                pidataobject->SetData(&fmte, &stgm, true);
            }
        }
        else
        {
            // UNDONE-ERR: Handle error case
        }
    }

done:
    // We're done with our data object, so release our reference to it.
    pidataobject->Release();

    g_progress.EndThread();
    
    // redraw
    SortListView(-1, FALSE);
}


bool LISTVIEW::BeginDrag()
{
    ULONG dwEffect = DROPEFFECT_MOVE | DROPEFFECT_COPY; 
    
    strcpy(g_szCopyDir, g_CurrentDir);

    // Create the IDataObject that contains information about the files being copied
    CFileContainer *pfc = new CFileContainer;

    // Populate the new file container with the files that are selected in us.
    PopulateWithSelected(pfc);

    // Create the drop source object
    CDropSource *pdropsource = new CDropSource;
    pdropsource->AddRef();
    
    IDropSource *pidropsource; 
    if (FAILED(pdropsource->QueryInterface(IID_IDropSource, (LPVOID*)&pidropsource)))
        return false;

    // And then perform the actual drag and drop.  This is a modal call...
    DoDragDrop((IDataObject*)pfc, pidropsource, dwEffect, &dwEffect);

    if (dwEffect == DROPEFFECT_MOVE)
        pfc->m_fCut = true;

	pidropsource->Release();
    pdropsource->Release();
    pfc->Release();
    return false;
}
