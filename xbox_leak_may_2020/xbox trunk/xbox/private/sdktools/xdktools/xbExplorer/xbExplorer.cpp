// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:     xbexplorer.cpp
// Contents: UNDONE-WARN: Add file description
//
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ INCLUDE FILES +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// "stdafx.h"       -- Precompiled header file
#include "stdafx.h"


#define DEFAULTXSIZE 510
#define DEFAULTYSIZE 440
#define MAINKEY HKEY_CURRENT_USER
#define SUBKEYATGXFBU	"Software\\Microsoft\\XboxSDK\\xbExplorer"
#define ATGKEYCURDIR	"Current"
#define ATGKEYDLDIR		"Download"
#define ATGKEYX			"x"
#define ATGKEYY			"y"
#define ATGKEYCX		"cx"
#define ATGKEYCY		"cy"
#define ATGKEYSPLASH	"Splash"
#define ATGKEYVIEW		"View"
#define MUTEXOFCH3SCMPX	"_XboxFileManager"
#define STARTUPDIR		"XE:\\"
#define NEWDIRNAME		"New Folder"
#define MULTIFILES		256
#ifdef JAPANESE
#define FILTER	"メディア (*.bmp;*.jpg;*.wav;*.mpg;*.wma;*.wmv;*.dls)\0*.bmp;*.jpg;*.wav;*.mpg;*.wma;*.wmv;*.dls\0"	\
				"Xbe実行ファイル (*.xbe)\0*.xbe\0"	\
				"Xboxフォント (*.abc)\0*.abc\0"		\
				"すべてのファイル (*.*)\0*.*\0"
#define OUTPUTDIRSTR "ダウンロード先を指定してください"
#else
#define FILTER	"Media files (*.bmp;*.jpg;*.wav;*.mpg;*.wma;*.wmv;*.dls)\0*.bmp;*.jpg;*.wav;*.mpg;*.wma;*.wmv;*.dls\0"	\
				"Xbe file (*.xbe)\0*.xbe\0"		\
				"Xbox font (*.abc)\0*.abc\0"	\
				"All Types (*.*)\0*.*\0"
#define OUTPUTDIRSTR "Select a directory for download."
#endif

char szAppName[] = "xbExplorer";
char szResName[] = "xbExplorer";
char szClassName[] = MUTEXOFCH3SCMPX;
int g_iPopupFileItem;
HINSTANCE hInst;
char g_TargetXbox[256];
char g_CurrentDir[MAX_PATH];
LISTVIEW listview;
STATUSVIEW statusview;
TOOLVIEW toolview;
HISTORY history;
BOOL splashscreen;
XBFU xbfu;
HWND g_hwnd;
int g_iTimer;

UINT g_suFileGroupDesc;
UINT g_suFileContents;
UINT g_suXboxObject;
UINT g_suXboxFiles;
UINT g_suPrefDropEffect;
UINT g_suPerfDropEffect;
UINT g_suPasteSucceeded;

CXboxConnection *g_pxboxconn;

UINT g_dropTypes[20];

// Register the clipboard formats necessary to transfer files.
bool RegisterFormats()
{
    g_suFileGroupDesc  = RegisterClipboardFormat(CFSTR_FILEDESCRIPTOR);
    g_suFileContents   = RegisterClipboardFormat(CFSTR_FILECONTENTS);
    g_suXboxObject     = RegisterClipboardFormat("XBOX_CLIP_FORMAT");
    g_suXboxFiles      = RegisterClipboardFormat("XBOX_FILES_LIST");
    g_suPrefDropEffect = RegisterClipboardFormat(CFSTR_PREFERREDDROPEFFECT);
    g_suPerfDropEffect = RegisterClipboardFormat(CFSTR_LOGICALPERFORMEDDROPEFFECT);
    g_suPasteSucceeded = RegisterClipboardFormat(CFSTR_PASTESUCCEEDED);

    // UNDONE-ERR: Need to make sure MitchD's code uses the same format so that we can talk to each other...

    return true;
}

void Init(LPRECT rect)
{
	REGISTRY *registry;
	char str[MAX_PATH] = "";
	DWORD dw, viewmode;
	RECT desktopRect;
	int x, y;
	registry = new REGISTRY;

        xbfu.GetTarget(g_TargetXbox, sizeof g_TargetXbox);

	registry->Init(MAINKEY, SUBKEYATGXFBU);
	if (registry->GetString(ATGKEYCURDIR, str))
        strcpy(g_CurrentDir, str);
	else strcpy(g_CurrentDir, STARTUPDIR);

    strcpy(g_CurrentDir, "XD:\\");

    // Handle old instance case -- if g_CurrentDir doesn't have 'x' at start, then add it
    if (g_CurrentDir[0] != 'X')
    {
        wsprintf(str, "X%s", g_CurrentDir);
        strcpy(g_CurrentDir, str);
    }
    
	if (registry->GetValue(ATGKEYX, &dw)) rect->left = (int) dw;
	else rect->left = CW_USEDEFAULT;
	if (registry->GetValue(ATGKEYY, &dw)) rect->top = (int) dw;
	else rect->top = CW_USEDEFAULT;
	if (registry->GetValue(ATGKEYCX, &dw)) rect->right = (int) dw;
	else rect->right = DEFAULTXSIZE;
	if (registry->GetValue(ATGKEYCY, &dw)) rect->bottom = (int) dw;
	else rect->bottom = DEFAULTYSIZE;
	if (registry->GetValue(ATGKEYSPLASH, &dw)) splashscreen = (BOOL) dw;
	else splashscreen = TRUE;
	viewmode = LVS_REPORT;
	if (registry->GetValue(ATGKEYVIEW, &dw))
        viewmode = dw;
	listview.ChangeView(viewmode);

    delete registry;

	SystemParametersInfo(SPI_GETWORKAREA, 0, &desktopRect, 0);
	x = rect->left + rect->right;
	y = rect->top + rect->bottom;
	if (x >= desktopRect.right) rect->left -= (x - desktopRect.right);
	if (rect->left < 0) rect->left = 0;
	if (y >= desktopRect.bottom) rect->top -= (y - desktopRect.bottom);
	if (rect->top < 0) rect->top = 0;
}

void UnInit(HWND hWnd)
{
	REGISTRY *registry;
	RECT rect;
	GetWindowRect(hWnd, &rect);
	rect.right -= rect.left;
	rect.bottom -= rect.top;
	registry = new REGISTRY;
	registry->Init(MAINKEY, SUBKEYATGXFBU);
	registry->SetString(ATGKEYCURDIR, g_CurrentDir);
	registry->SetValue(ATGKEYX, rect.left);
	registry->SetValue(ATGKEYY, rect.top);
	registry->SetValue(ATGKEYCX, rect.right);
	registry->SetValue(ATGKEYCY, rect.bottom);
	registry->SetValue(ATGKEYVIEW, (DWORD) listview.GetViewMode());
	delete registry;
}

bool Reload()
{
	if (!listview.SortListView(-1, FALSE))
        return false;
    toolview.Refresh();
    return true;
}

void SetTitleName(HWND hWnd)
{
	char str[256];
	strcpy(str, szAppName);
	strcat(str, " - ");
	strcat(str, g_TargetXbox);
	SetWindowText(hWnd, str);
}

void ReturnToParentDir()
{
	char *pStr1, *pStr2;
	char str[MAX_PATH];
	strcpy(str, g_CurrentDir);
	pStr1 = strrchr(str, '\\');
	if (pStr1)
	{
		pStr1[0] = '\0';
		pStr2 = strrchr(str, '\\');
		if (pStr2)
		{
			history.SaveHistory(g_CurrentDir);
			pStr2[1] = '\0';
			strcpy(g_CurrentDir, str);
			Reload();
		}
	}
}

void GetListRect(HWND hWnd, HWND statusWnd, HWND toolWnd, LPRECT lpRect)
{
	RECT rect2;
	GetClientRect(hWnd, lpRect);
	GetWindowRect(toolWnd, &rect2);
	lpRect->top += (rect2.bottom - rect2.top);
	GetWindowRect(statusWnd, &rect2);
	lpRect->bottom -= (rect2.bottom - rect2.top);
}

void CenterDialog(HWND hDlg)
{
	int x, y;
    POINT pt = {0};
	RECT rc, parentrc;
	GetWindowRect(hDlg, &rc);
	GetClientRect(g_hwnd, &parentrc);
    ClientToScreen(g_hwnd, &pt);
	x = pt.x + (parentrc.right / 2 - (rc.right - rc.left) / 2);
	y = pt.y + (parentrc.bottom / 2 - (rc.bottom - rc.top) / 2);
	SetWindowPos(hDlg, NULL, x, y, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_SHOWWINDOW);
    BringWindowToTop(hDlg);
}

char *FormatNumber(int n)
{
    return FormatBigNumber((ULONGLONG)n);
}


void TotalFiles()
{
	char str[256] = "";
	int totalfolders, totalfiles, totalsize;
	listview.CheckFiles(&totalfolders, &totalfiles, &totalsize);
	if (totalfiles > 0 || totalfolders > 0)
	{
		wsprintf(str, "Total ");
		if (totalfolders)
		{
			wsprintf(str, "%s%s folder", str, FormatNumber(totalfolders));
			if (totalfolders > 1) wsprintf(str, "%ss", str);
		}
		if (totalfiles)
		{
			if (totalfolders) wsprintf(str, "%s and ", str);
			wsprintf(str, "%s%s bytes", str, FormatNumber(totalsize));
			wsprintf(str, "%s in %s file", str, FormatNumber(totalfiles));
			if (totalfiles > 1) wsprintf(str, "%ss", str);
		}
	}
	statusview.SetText(TOTAL_STATUS, str);
}

void AllocFileNameBuffer(char **lpstrOpenFile)
{
	int i;
	for(i = 0; i < MULTIFILES; i++)
	{
		lpstrOpenFile[i] = new char[MAX_PATH];
		ZeroMemory(lpstrOpenFile[i], MAX_PATH);
	}
}

void FreeFileNameBuffer(char **lpstrOpenFile)
{
	int i;
	for(i = 0; i < MULTIFILES; i++)
	{
		delete lpstrOpenFile[i];
	}
}

bool ClipContents()
{
    IDataObject *pidataobject;

    OleGetClipboard(&pidataobject);
    if (pidataobject)
    {
        // There's data in the clipboard.  Is it files?
        STGMEDIUM stgm;
        FORMATETC fmte= {(USHORT)g_suXboxFiles,  NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL};
        if (pidataobject->GetData(&fmte, &stgm) == S_OK)
            return true;
     
        fmte.cfFormat = (USHORT)CF_HDROP;
        if (pidataobject->GetData(&fmte, &stgm) == S_OK)
            return true;
    }

    return false;
}

void CheckViewmodeMenu(HMENU hMenu)
{
	CheckMenuItem(hMenu, IDM_VIEWLARGEICONS, MFS_UNCHECKED);
	CheckMenuItem(hMenu, IDM_VIEWSMALLICONS, MFS_UNCHECKED);
	CheckMenuItem(hMenu, IDM_VIEWLIST,       MFS_UNCHECKED);
	CheckMenuItem(hMenu, IDM_VIEWDETAIL,     MFS_UNCHECKED);
    switch(listview.GetViewMode())
	{
    case LVS_ICON:
		CheckMenuItem(hMenu, IDM_VIEWLARGEICONS, MFS_CHECKED);
        break;

    case LVS_SMALLICON:
		CheckMenuItem(hMenu, IDM_VIEWSMALLICONS, MFS_CHECKED);
        break;

	case LVS_LIST:
		CheckMenuItem(hMenu, IDM_VIEWLIST,       MFS_CHECKED);
		break;
	case LVS_REPORT:
		CheckMenuItem(hMenu, IDM_VIEWDETAIL,     MFS_CHECKED);
		break;
	}

    if (ClipContents())
		EnableMenuItem(hMenu, IDM_PASTE, MF_ENABLED);
    else
		EnableMenuItem(hMenu, IDM_PASTE, MF_GRAYED);

    if (listview.GetNumSelected() > 0)
    {
        EnableMenuItem(hMenu, IDM_DELETE, MF_ENABLED);
        EnableMenuItem(hMenu, IDM_CUT, MF_ENABLED);
        EnableMenuItem(hMenu, IDM_COPY, MF_ENABLED);
    }
    else
    {
        EnableMenuItem(hMenu, IDM_DELETE, MF_GRAYED);
        EnableMenuItem(hMenu, IDM_CUT, MF_GRAYED);
        EnableMenuItem(hMenu, IDM_COPY, MF_GRAYED);
    }
}

void Selected(HWND hWnd, HWND hListWnd)
{
	char str[512];
	int i, count;
	UINT ret;
	count = ListView_GetItemCount(hListWnd);
	for(i = 0; i < count; i++)
	{
		ret = ListView_GetItemState(hListWnd, i, LVIS_SELECTED);
		if (ret) break;
	}
        if (i >= count)
            return;
	ListView_GetItemText(hListWnd, i, COLUMN_NAME, str, 512 - 1);
	if (! MyStrcmp(str, "..")) ReturnToParentDir();
	else
	{
		char dir[32];
		ListView_GetItemText(hListWnd, i, COLUMN_SIZE, dir, 32 - 1);
		if (! MyStrcmp(dir, CHAR_DIR))
		{
			history.SaveHistory(g_CurrentDir);
			strcat(g_CurrentDir, str);
			strcat(g_CurrentDir, "\\");
			Reload();
		}
		else
		{
            // UNDONE-WARN: Remove this code.  No longer needed since we have drag/drop
	//		PostMessage(hWnd, WM_COMMAND, IDM_DOWNLOAD, 0);
		}
	}
}

LRESULT CALLBACK WndProc(HWND hWnd, unsigned uMsg, WPARAM wParam, LPARAM lParam)
{
	static LPTOOLTIPTEXT lptip;
	static HWND hListWnd;
	static HWND hStatusWnd;
	static HWND hToolWnd;
	static HWND hSplashWnd = NULL;
    static bool fHack = false;
	switch (uMsg)
	{
	case WM_STATUSTEXT:
		statusview.SetText((int) wParam, (LPCTSTR) lParam);
		break;
	case WM_COMMAND:
		switch(wParam)
		{
        case IDM_PARENTDIR:
			ReturnToParentDir();
			break;
		case IDM_DELETE:
            listview.Delete(hWnd);
			break;

		case IDM_RENAME:
			{
				int i, count;
				UINT ret;
				count = ListView_GetItemCount(hListWnd);
				for(i = 0; i < count; i++)
				{
					ret = ListView_GetItemState(hListWnd, i, LVIS_FOCUSED);
					if (ret) break;
				}
				ListView_EditLabel(hListWnd, i);
			}
			break;
		case IDM_MKDIR:
			{
				char str[MAX_PATH + SHORTFILELEN];
				char str2[SHORTFILELEN] = NEWDIRNAME;
				int i;
				for(i = 1; i < 1000; i++)
				{
					strcpy(str, g_CurrentDir);
					strcat(str, str2);
					if (listview.IsExistence(str2) == -1) break;
					wsprintf(str2, NEWDIRNAME".%03d", i);
				}
				xbfu.Md(str);
				Reload();
				i = listview.IsExistence(str2);
				ListView_EditLabel(hListWnd, i);
			}
			break;
		case IDM_BACK:
			if (history.GetRewindCount())
			{
				history.RestoreRewindHistory(g_CurrentDir);
				Reload();
			}
			break;
		case IDM_FORWARD:
			if (history.GetForwardCount())
			{
				history.RestoreForwardHistory(g_CurrentDir);
				Reload();
			}
			break;
		case IDM_ALLSELECT:
			{
				int i, j;
				j = ListView_GetItemCount(hListWnd);
				for(i = 0; i < j; i++)
				{
					ListView_SetItemState(hListWnd, i, LVIS_SELECTED, LVIS_SELECTED);
				}
			}
			break;
		case IDM_REBOOT:
			{
				char str[300];
				strcpy(str, "Reboot ");
				strcat(str, g_TargetXbox);
				strcat(str, "?");
				if (MessageBox(hWnd, str, szAppName, MB_YESNO) == IDYES) xbfu.Reboot();
			}
			break;

        case IDM_VIEWLARGEICONS:
			listview.ChangeView(LVS_ICON);
            break;

        case IDM_VIEWSMALLICONS:
			listview.ChangeView(LVS_SMALLICON);
            break;

        case IDM_ARRANGE_BYNAME:
            listview.SortListView(0, true);
            break;

        case IDM_ARRANGE_BYTYPE:
            listview.SortListView(1, true);
            break;

        case IDM_ARRANGE_BYSIZE:
            listview.SortListView(2, true);
            break;

        case IDM_ARRANGE_BYDATE:
            listview.SortListView(3, true);
            break;

        case IDM_REFRESH:
            Reload();
			break;

        case IDM_PROPERTIES:
            // Open up the appropriate property sheet.
            if (g_iPopupFileItem == -1)
            {
                // user cliked on white space = show folder prop with current folder
    			DialogBox(hInst, "FOLDERPROP", hWnd, (DLGPROC) FolderPropertiesDlg);
            }
            else
            {
                // user clicked on item - folder?
                // UNDONE-ERR: Multiple selection not supported.  Need to combine all information
                //             from all selected items
                if (listview.IsFolder(g_iPopupFileItem))
        			DialogBox(hInst, "FOLDERPROP", hWnd, (DLGPROC) FolderPropertiesDlg);
                else
        			DialogBox(hInst, "FILEPROP", hWnd, (DLGPROC) FilePropertiesDlg);
            }
            break;

        case IDM_PASTE:
            listview.Paste();
            break;

        case IDM_CUT:
            listview.CutCopy(true);
            break;

        case IDM_COPY:
            listview.CutCopy(false);
            break;


		case IDM_TARGETMACHINE:
			{
                bool fDone;
                do
                {
				    if (DialogBox(hInst, "TARGETDLG", hWnd, (DLGPROC) TargetDlg) == IDOK)
				    {
                        xbfu.SetTarget(g_TargetXbox);
                        fDone = Reload();
				    }
                    else
                        fDone = true;
                } while (!fDone);
			}
			break;
		case IDM_ABOUT:
			DialogBox(hInst, "ABOUTDLG2", hWnd, (DLGPROC) AboutDlgProc);
			break;
		case IDM_VIEWLIST:
			listview.ChangeView(LVS_LIST);
			break;
		case IDM_VIEWDETAIL:
			listview.ChangeView(LVS_REPORT);
			break;
		case IDOK:
			PostMessage(hWnd, WM_CLOSE, 0, 0);
			break;
		}
		{
			WORD wID = LOWORD(wParam);
			WORD wNotifyCode = HIWORD(wParam);
			if (wID == ID_COMBOBOX)
			{
				switch(wNotifyCode)
				{
				case CBN_SELCHANGE:
					{
						char str[MAX_PATH];
						int i;
						i = SendMessage((HWND) lParam, CB_GETCURSEL, 0, 0);
						SendMessage((HWND) lParam, CB_GETLBTEXT, i, (LPARAM) str);
						if (xbfu.IsDrive(str[1]))
						{
							history.SaveHistory(g_CurrentDir);
							strcpy(g_CurrentDir, str);
						}
						Reload();
					}
					break;
				}
			}
		}
		break;
	case WM_ERASEBKGND:
		break;
	case WM_CREATE:
		{
			RECT rect;

			SetTitleName(hWnd);
            hToolWnd = toolview.CreateToolView(hWnd);
			hStatusWnd = statusview.CreateStatusView(hWnd);
			GetClientRect(hWnd, &rect);
			GetListRect(hWnd, hStatusWnd, hToolWnd, &rect);
			hListWnd = listview.CreateListView(hWnd, &rect);
            g_iTimer = SetTimer(hWnd, 1, 250, NULL);
        }
		break;

    case WM_TIMER:
        KillTimer(hWnd, g_iTimer);
        {
	        char str[MAX_PATH] = "";
            extern bool g_fForceWaitCursor;
            
            // Handle old instance case -- if g_CurrentDir doesn't have 'x' at start, then add it
            if (g_CurrentDir[0] != 'X')
            {
                wsprintf(str, "X%s", g_CurrentDir);
                strcpy(g_CurrentDir, str);
            }

            g_fForceWaitCursor = true;
            SetCursor(LoadCursor(NULL, IDC_WAIT));

            if (!xbfu.IsDrive(g_CurrentDir[0]))
            {
                // Check if we weren't able to connect at all
                while (xbfu.FailedConnect())
                {
                    // Couldn't connect to the currently named xbox.  force the
                    // user to select a new xbox
				    if (DialogBox(hInst, "TARGETDLG2", hWnd, (DLGPROC) TargetDlg) == IDOK)
				    {
                        xbfu.SetTarget(g_TargetXbox);
                        if (Reload())
                            break;
				    }
                    else
                    {
                        // User canceled
                        exit(-1);
                    }
                }

                // drive disappeared.  Default to E:
                strcpy(g_CurrentDir, "XE:\\");
            }
            SetCursor(LoadCursor(NULL, IDC_ARROW));
        }

        listview.SortListView(0, false); // 0 = by name (default)
        toolview.InsertComboBox();
	    toolview.InsertToolBar();

        break;
        
	case WM_DESTROY:
		if (hSplashWnd) DestroyWindow(hSplashWnd);
		UnInit(hWnd);
		PostQuitMessage(0);
		break;

	case WM_SIZE:
		{
			RECT rect;
			toolview.RedrawToolView(hWnd);
			statusview.RedrawStatusView(hWnd, wParam, lParam);
			GetListRect(hWnd, hStatusWnd, hToolWnd, &rect);
			listview.RedrawListView(hWnd, &rect);
		}
		break;
	case WM_ACTIVATE:
        if ((LOWORD(wParam) == WA_ACTIVE) && hListWnd) SetFocus(hListWnd);
		break;

    case WM_NOTIFY:
		{
			LPNMLISTVIEW lpNmLV;
			NMLVDISPINFO *lpNMLVDISP;
			LPNMLVKEYDOWN pnkd;

			lpNmLV = (LPNMLISTVIEW) lParam;
			lpNMLVDISP = (NMLVDISPINFO*) lParam;
			pnkd = (LPNMLVKEYDOWN) lParam;
			switch(lpNmLV->hdr.code)
			{
			case LVN_KEYDOWN:
				switch(pnkd->wVKey)
				{
				case VK_RETURN:
					Selected(hWnd, lpNmLV->hdr.hwndFrom);
					break;
				case VK_BACK:
					PostMessage(hWnd, WM_COMMAND, IDM_BACK, 0);
					break;
				case VK_F1:
					PostMessage(hWnd, WM_COMMAND, IDM_ABOUT, 0);
					break;
				case VK_F2:
					PostMessage(hWnd, WM_COMMAND, IDM_RENAME, 0);
					break;
				case VK_F5:
					Reload();
					break;
				case VK_DELETE:
					PostMessage(hWnd, WM_COMMAND, IDM_DELETE, 0);
					break;
				}
				if (GetKeyState(VK_CONTROL) < 0)
				{
					if (GetKeyState('A') < 0) PostMessage(hWnd, WM_COMMAND, IDM_ALLSELECT, 0);
					if (GetKeyState('C') < 0) PostMessage(hWnd, WM_COMMAND, IDM_COPY, 0);
					if (GetKeyState('X') < 0) PostMessage(hWnd, WM_COMMAND, IDM_CUT, 0);
					if (GetKeyState('V') < 0) PostMessage(hWnd, WM_COMMAND, IDM_PASTE, 0);
			//		else if (GetKeyState('B') < 0) PostMessage(hWnd, WM_COMMAND, IDM_BACK, 0);
			//		else if (GetKeyState('D') < 0) PostMessage(hWnd, WM_COMMAND, IDM_DELETE, 0);
			//		else if (GetKeyState('F') < 0) PostMessage(hWnd, WM_COMMAND, IDM_FORWARD, 0);
			//		else if (GetKeyState('O') < 0) PostMessage(hWnd, WM_COMMAND, IDM_UPLOAD, 0);
			//		else if (GetKeyState('R') < 0) PostMessage(hWnd, WM_COMMAND, IDM_RENAME, 0);
			//		else if (GetKeyState('S') < 0) PostMessage(hWnd, WM_COMMAND, IDM_DOWNLOAD, 0);
				}
				break;

			case LVN_BEGINLABELEDIT:
				return listview.IsParentFolder(lpNMLVDISP->item.iItem);
                
			case LVN_ENDLABELEDIT:
				{
					if (lpNMLVDISP->item.pszText)
					{
#if 0
    					char str1[MAX_PATH + SHORTFILELEN], str2[MAX_PATH + SHORTFILELEN];
                        ListView_GetItemText(lpNmLV->hdr.hwndFrom, lpNMLVDISP->item.iItem, COLUMN_NAME, str2, SHORTFILELEN);
						strcpy(str1, g_CurrentDir);
						strcat(str1, str2);
						strcpy(str2, g_CurrentDir);
						strcat(str2, lpNMLVDISP->item.pszText);
#endif
                        
                        char szOrigFilePathName[1024], szNewFilePathName[1024], szOrigFileName[MAX_PATH], szNewFileName[MAX_PATH];
                        DM_FILE_ATTRIBUTES dmfa;
        			    ListView_GetItemText(lpNmLV->hdr.hwndFrom, lpNMLVDISP->item.iItem, COLUMN_NAME, szOrigFileName, MAX_PATH);
                        strcpy(szNewFileName, lpNMLVDISP->item.pszText);
                        sprintf(szOrigFilePathName, "%s%s", g_CurrentDir, szOrigFileName);
                        sprintf(szNewFilePathName, "%s%s", g_CurrentDir, szNewFileName);

                        xbfu.GetFileAttributes(szOrigFilePathName, &dmfa);
                        if (dmfa.Attributes & FILE_ATTRIBUTE_READONLY)
                        {
                            bool fNoRename;
                            char szText[MAX_PATH], szCaption[200];
                            char szBuf[MAX_PATH];

                            if (dmfa.Attributes & FILE_ATTRIBUTE_DIRECTORY)
                            {
                                LoadString(hInst, IDS_CONFIRM_RENAME_RO_FOLDER_QUERY, szBuf, MAX_PATH);
                                LoadString(hInst, IDS_CONFIRM_RENAME_RO_FOLDER, szCaption, 200);
                                sprintf(szText, szBuf, szOrigFileName, szNewFileName);
                            }
                            else
                            {
                                LoadString(hInst, IDS_CONFIRM_RENAME_RO_FILE_QUERY, szBuf, MAX_PATH);
                                LoadString(hInst, IDS_CONFIRM_RENAME_RO_FILE, szCaption, 200);
                                sprintf(szText, szBuf, szOrigFileName, szNewFileName);
                            }
                            fNoRename = (MessageBox(g_hwnd, szText, szCaption, MB_YESNO) == IDNO);
                            if (fNoRename)
                                return FALSE;

                            // Remove the readonly bit so that we can rename the file
                            xbfu.RemoveAttribute(szOrigFilePathName, FILE_ATTRIBUTE_READONLY);
                        }
                        if (!xbfu.Ren(szOrigFilePathName, szNewFilePathName))
                        {
                            // Failed to rename the file (might be in use?)
                            char szText[MAX_PATH], szCaption[200];
                            LoadString(hInst, IDS_FAIL_RENAME, szText, MAX_PATH);
                            LoadString(hInst, IDS_FAIL_RENAME_CAPTION, szCaption, MAX_PATH);
                            MessageBox(g_hwnd, szText, szCaption, MB_ICONWARNING | MB_OK);
                            return FALSE;
                        }
                        if (dmfa.Attributes & FILE_ATTRIBUTE_READONLY)
                        {
                            // Restore the read-only attribute
                            xbfu.SetAttribute(szNewFilePathName, FILE_ATTRIBUTE_READONLY);
                        }

                        Reload();

                        // UNDONE-ERR: going...to...kill...something... If I don't reload at the end of the
                        // lvn_itemchanged below, then the file rename is all screwed up.  while
                        // ugly, this catches the bug, and that's the mental state I'm at.  Clean it up later.
                        fHack = true;

                        return TRUE;
					}
				}
                return TRUE;

            case LVN_BEGINDRAG:
                listview.BeginDrag();
                break;
                
          case LVN_COLUMNCLICK:
             // The user clicked one of the column headings. Sort by
             // this column. This function calls an application-defined
             // comparison callback function, ListViewCompareProc.
             listview.SortListView((LPARAM)(lpNmLV->iSubItem), true);
//             ListView_SortItems(lpNmLV->hdr.hwndFrom, ListViewCompareProc, (LPARAM)(lpNmLV->iSubItem));
             listview.Focus();
             break;

			case LVN_ITEMCHANGED:
				{
					char str[256] = "";
					int totalfolders, totalfiles, totalsize;

					listview.CheckSelectedFiles(&totalfolders, &totalfiles, &totalsize);
					if (totalfiles > 0 || totalfolders > 0)
					{
						wsprintf(str, "Selected ");
						if (totalfolders)
						{
							wsprintf(str, "%s%s folder", str, FormatNumber(totalfolders));
							if (totalfolders > 1) wsprintf(str, "%ss", str);
						}
						if (totalfiles)
						{
							if (totalfolders) wsprintf(str, "%s and ", str);
							wsprintf(str, "%s%s bytes", str, FormatNumber(totalsize));
							wsprintf(str, "%s in %s file", str, FormatNumber(totalfiles));
							if (totalfiles > 1) wsprintf(str, "%ss", str);
						}
					}
                    else
                    {
                        // Display disk free space
                        ULARGE_INTEGER uliFreeSpace;
                        char szBuf[100];
                        sprintf(szBuf, "%c:\\", g_CurrentDir[1]);
                        xbfu.GetDriveFreeSize(szBuf, &uliFreeSpace);
                        FormatSize(uliFreeSpace, szBuf, false);
                        wsprintf(str, "Disk free space: %s", szBuf);
                    }

					statusview.SetText(SELECTED_STATUS, str);
                    if (fHack)
                    {
                        fHack = false;
                        Reload();
                    }
				}
				break;
                
			case NM_RCLICK:
				{
                    // User right clicked - bring up property menu; which type depends on whether
                    // or not the user clicked on an icon or white space.  Further, clicking on
                    // a directory brings up different options; however for our purposes we consider
                    // them the exact same (if we want to add dir-specific options later, just check
                    // IsFolder(iItem))...

                    NMHDR *nmhdr = (NMHDR*)lParam;
                    POINT pt;
					HMENU menu, hmenuPopup;
                    char szName[MAX_PATH];
					POINT point;

                    GetCursorPos(&pt);
                    ScreenToClient(nmhdr->hwndFrom, &pt);

                    int iItem = listview.GetItemAtPoint(&pt);
                    ListView_GetItemText(listview.GetHwnd(), iItem, COLUMN_NAME, szName, MAX_PATH);
                    if (MyStrcmp(szName, ".."))
                    {
                        if (iItem == ITEM_NONE)
                        {
                            // Bring up non-object specific pop-up
					        menu = LoadMenu(hInst, "XBEXPLORER_POPUP");
                        }
                        else
                        {
                            // Bring up file popup
					        menu = LoadMenu(hInst, "XBEXPLORER_FILEPOPUP");
                        }

                        g_iPopupFileItem = iItem;

					    if (menu)
					    {
						    hmenuPopup = GetSubMenu(menu, 0);
//						    if (! history.GetRewindCount()) EnableMenuItem(menu, IDM_BACK, MF_GRAYED);
//						    if (! history.GetForwardCount()) EnableMenuItem(menu, IDM_FORWARD, MF_GRAYED);

                            if (listview.GetNumSelected() <= 1)
                            {
                                char szBuf[100];
                                if(listview.GetNumSelected() == 1) {
                                    LoadString(hInst, IDS_RENAME, szBuf, 100);
                                    AppendMenu(hmenuPopup, MF_STRING, IDM_RENAME, szBuf);
                                }
                                LoadString(hInst, IDS_PROPERTIES, szBuf, 100);
                                AppendMenu(hmenuPopup, MF_SEPARATOR, 0, NULL);
                                AppendMenu(hmenuPopup, MF_STRING, IDM_PROPERTIES, szBuf);
                            }
					    }
                    }
					CheckViewmodeMenu(menu);
					GetCursorPos(&point);
					TrackPopupMenu(hmenuPopup, TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, 0, hWnd, NULL);
					DestroyMenu(menu);
                }
				break;

			case NM_DBLCLK:
				Selected(hWnd, lpNmLV->hdr.hwndFrom);
				break;
                
			case TTN_NEEDTEXT:
				lptip = (LPTOOLTIPTEXT) lParam;
				lptip->hinst = hInst;
				lptip->lpszText = MAKEINTRESOURCE(lptip->hdr.idFrom);
				break;

			case RBN_CHILDSIZE:
				{
					RECT rect;
					GetListRect(hWnd, hStatusWnd, hToolWnd, &rect);
					listview.RedrawListView(hWnd, &rect);
				}
				break;
			}
		}
		break;
	case WM_MENUSELECT:
		if (HIWORD(wParam) == (MF_MOUSESELECT | MF_POPUP | MF_HILITE)) 
            CheckViewmodeMenu((HMENU) lParam);
		break;
	default:
		return(DefWindowProc(hWnd, uMsg, wParam, lParam));
	}
	return 0;
}

bool HandleCmdLine(char *szCmd)
{
    if (*szCmd == NULL)
        return true;

    if ((szCmd[1] == 'x' || szCmd[1] == 'X') && szCmd[3])
    {
        if(SUCCEEDED(DmSetXboxName(szCmd + 3)))
            return true;
    }

    // If here, then bad cmd line
    char szMsg[1000];
    sprintf(szMsg, "xbExplorer.exe.\n\nAllows browsing an Xbox " \
            "in a graphical environment.\r\n\r\nusage: xbExplorer [/x xboxname]\r\n"\
            "        /x    Specify Xbox to explore.");

    MessageBox(NULL, szMsg, "Invalid Command Line Option Specified", MB_ICONINFORMATION | MB_OK);
    return false;
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmd, int nCmdShow)
{
	WNDCLASS wndclass;
	HANDLE mutexhandle;
	HWND hWnd;
	MSG msg;

	// Disable multiple instance
	mutexhandle = CreateMutex(NULL, TRUE, MUTEXOFCH3SCMPX);
	if (GetLastError()) msg.wParam = -1;
	else
	{
		RECT rect;
        
        OleInitialize(NULL); 

        // Handle command line arguments
        if (!HandleCmdLine(lpszCmd))
            return -1;
        
        // If we haven't yet registered the clipboard formats for transfering files to/from the shell,
        // then do so now.
        RegisterFormats();

        hInst = hInstance;
		wndclass.style = CS_HREDRAW | CS_VREDRAW;
		wndclass.lpfnWndProc = (WNDPROC) WndProc;
		wndclass.cbClsExtra = 0;
		wndclass.cbWndExtra = 0;
		wndclass.hInstance = hInstance;
		wndclass.hIcon = LoadIcon(hInstance, szResName);
		wndclass.hCursor = NULL;//LoadCursor(NULL, IDC_ARROW);
		wndclass.hbrBackground = (HBRUSH) (COLOR_APPWORKSPACE + 1);
		wndclass.lpszMenuName = szResName;
		wndclass.lpszClassName = szClassName;
		if (! RegisterClass(&wndclass)) return FALSE;

        // Fire up the xbox connection
        g_pxboxconn = new CXboxConnection();		

		Init(&rect);
		hWnd = CreateWindow(szClassName, szAppName, WS_OVERLAPPEDWINDOW, rect.left, rect.top,
                            rect.right, rect.bottom, NULL, NULL, hInstance, NULL);
		if (! hWnd) return FALSE;
		ShowWindow(hWnd, SW_SHOWDEFAULT);
        g_hwnd = hWnd;

		while(GetMessage(&msg, NULL, 0, 0))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

        delete g_pxboxconn;
        
        OleUninitialize();
	}


	return msg.wParam;
}

int MyStricmp(const char *sz1, const char *sz2)
{
    // not defined in xbox libs
    return lstrcmpi(sz1, sz2);
}

int MyStrcmp(const char *sz1, const char *sz2)
{
    // not defined in xbox libs
    return lstrcmp(sz1, sz2);
}

char *MyStrdup(char *sz)
{
    // not defined in xbox libs
    char *szNew = (char *)malloc(MyStrlen(sz)+1);
    strcpy(szNew, sz);
    return szNew;
}

int MyStrlen(const char *sz)
{
    return _mbstrlen(sz);
}

char gs_szUtoA[1024];
char *WideToSingleByte(WCHAR *pwIn)
{
    char *szOut = gs_szUtoA;
    while (*pwIn)
    {
        *szOut++ = (char)(*pwIn++);
    }
    *szOut = '\0';
    return gs_szUtoA;
}