// spy.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#pragma hdrstop

#include "msglog.h"
#include "msgview.h"
#include "msgdoc.h"

#include "hotlinkc.h"

#include "treectl.h"
#include "spytreed.h"
#include "spytreec.h"
#include "spytreev.h"

#include "wndnode.h"
#include "wndtreed.h"
#include "prctreed.h"
#include "thdtreed.h"

#include "mainfrm.h"
#include "aboutdlg.h"
#include "findtool.h"
#include "findwdlg.h"
#include "spyxxmdi.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

void idsDelete();	// forward declaration for idsDelete()

/////////////////////////////////////////////////////////////////////////////
// CSpyApp

CHAR CSpyApp::m_szSpyKey[] = "Software\\Microsoft\\Spy++ 3.0";
CHAR CSpyApp::m_szKeyDefFont[] = "DefFont";
CHAR CSpyApp::m_szKeyMainPos[] = "MainPos";
CHAR CSpyApp::m_szKeyDecodeStructs[] = "DecodeStructs";
CHAR CSpyApp::m_szMaximizeMDI[] = "MaximizeMDI";
BOOL CSpyApp::m_bIsChicago = FALSE;
BOOL CSpyApp::m_bIsDaytona = FALSE;
BOOL CSpyApp::m_bIsMDIMaximized = -1;
int  CSpyApp::m_nWinTabCur = -1;
int  CSpyApp::m_nPrcTabCur = -1;
int  CSpyApp::m_nThdTabCur = -1;
int  CSpyApp::m_nMsgTabCur = -1;

HINSTANCE		CSpyApp::hToolhelp32 = NULL;
SNAPSHOTPROC	CSpyApp::pfnCreateToolhelp32Snapshot = NULL;
HEAPLISTPROC	CSpyApp::pfnHeap32ListFirst = NULL;
HEAPLISTPROC	CSpyApp::pfnHeap32ListNext = NULL;
HEAP32FIRSTPROC	CSpyApp::pfnHeap32First = NULL;
HEAP32NEXTPROC	CSpyApp::pfnHeap32Next = NULL;
READPROCMEMPROC	CSpyApp::pfnToolhelp32ReadProcessMemory = NULL;
PROCESSLISTPROC	CSpyApp::pfnProcess32First = NULL;
PROCESSLISTPROC	CSpyApp::pfnProcess32Next = NULL;
THREADLISTPROC	CSpyApp::pfnThread32First = NULL;
THREADLISTPROC	CSpyApp::pfnThread32Next = NULL;
MODULELISTPROC	CSpyApp::pfnModule32First = NULL;
MODULELISTPROC	CSpyApp::pfnModule32Next = NULL;


BEGIN_MESSAGE_MAP(CSpyApp, CWinApp)
	//{{AFX_MSG_MAP(CSpyApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_SPY_MESSAGES, OnSpyMessages)
	ON_COMMAND(ID_SPY_MESSAGESDIRECT, OnSpyMessagesDirect)
	ON_COMMAND(ID_SPY_HIGHLIGHTWINDOW, OnSpyHighlightWindow)
	ON_COMMAND(ID_SPY_WINDOWS, OnSpyWindows)
	ON_COMMAND(ID_SPY_PROCESSES, OnSpyProcesses)
	ON_COMMAND(ID_SPY_THREADS, OnSpyThreads)
	ON_COMMAND(ID_SPY_FINDWINDOW, OnSpyFindWindow)
	ON_UPDATE_COMMAND_UI(ID_SPY_PROCESSES, OnUpdateSpyProcesses)
	ON_UPDATE_COMMAND_UI(ID_SPY_THREADS, OnUpdateSpyThreads)
	//}}AFX_MSG_MAP
	// Global help commands
	ON_COMMAND(ID_HELP_INDEX, CWinApp::OnHelpIndex)
	ON_COMMAND(ID_HELP_USING, CWinApp::OnHelpUsing)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
	ON_COMMAND(ID_DEFAULT_HELP, CWinApp::OnHelpIndex)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// utility functions

// call this function twice to turn on and off window border highlighting
void HighlightWindowBorder(HWND hwndHighlight)
{
	HDC hdc;
	RECT rc;
	HPEN hpen;
	HPEN hpenOld;
	HBRUSH hbrOld;
	int cxBorder = ::GetSystemMetrics(SM_CXBORDER);
	int cyBorder = ::GetSystemMetrics(SM_CYBORDER);
	int cxFrame  = ::GetSystemMetrics(SM_CXFRAME);
	int cyFrame  = ::GetSystemMetrics(SM_CYFRAME);
	int cxScreen = ::GetSystemMetrics(SM_CXSCREEN);
	int cyScreen = ::GetSystemMetrics(SM_CYSCREEN);

	::GetWindowRect(hwndHighlight, &rc);
	hdc = ::GetWindowDC(hwndHighlight);
	::SetROP2(hdc, R2_NOT);
	hpen = ::CreatePen(PS_INSIDEFRAME, 3 * cxBorder, RGB(0, 0, 0));

	hpenOld = (HPEN)::SelectObject(hdc, hpen);
	hbrOld = (HBRUSH)::SelectObject(hdc, ::GetStockObject(NULL_BRUSH));
	if (::IsZoomed(hwndHighlight))
		::Rectangle(hdc, cxFrame, cyFrame, cxScreen + cxFrame, cyScreen + cyFrame);
	else
		::Rectangle(hdc, 0, 0, rc.right - rc.left, rc.bottom - rc.top);
	::SelectObject(hdc, hbrOld);
	::SelectObject(hdc, hpenOld);

	::ReleaseDC(hwndHighlight, hdc);
	::DeleteObject(hpen);
}

void FlashWindowBorder(HWND hwndFlash)
{
	for (int i = 1; i <= 5; i++)
	{
		HighlightWindowBorder(hwndFlash);
		Sleep(100);
		HighlightWindowBorder(hwndFlash);
		Sleep(100);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CSpyApp construction

CSpyApp::CSpyApp()
//	:m_ProcessDatabase(&m_pdbError)
{
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CSpyApp object

CSpyApp NEAR theApp;

/////////////////////////////////////////////////////////////////////////////
// CSpyApp initialization

BOOL CSpyApp::InitInstance()
{
	HKEY hkey;
	DWORD dwType;
	DWORD cbData;
	BOOL fDecodeStructs;
	BOOL fMainPosValid;
	WINDOWPLACEMENT wndpl;
	HANDLE hmutexStartup;

	//
	// Create/Open the Spy startup mutex.  This is part of the
	// mechanism that is used to determine if another instance
	// of Spy++ is running.  This is not allowed because there
	// is only one shared memory queue that is used to pass the
	// hook information across, and with multiple copies of Spy++
	// running they would be fighting over reading the queue.
	//
	hmutexStartup = CreateMutex(NULL, FALSE, STARTUPMUTEXNAME);
	ASSERT(hmutexStartup);

	//
	// Grab the mutex without waiting.  If we fail because another
	// copy of Spy++ has the mutex (and is initializing), then exit.
	// We don't need to activate the other copy in this case because
	// it will activate itself as soon as it is done initializing.
	//
	if (WaitForSingleObject(hmutexStartup, 0) != WAIT_OBJECT_0)
	{
		CloseHandle(hmutexStartup);
		return FALSE;
	}

	//
	// Search for another instance of Spy++ and activate it if it
	// is found.  Since it did not have the startup mutex claimed
	// we can be sure that the window has been created by now and
	// will be found by enumerating top-level windows.
	//
	HWND hwndPrevInst = GetWindow(GetDesktopWindow(), GW_CHILD);
	while (hwndPrevInst)
	{
		if (GetWindowLong(hwndPrevInst, GWL_USERDATA) == SPYXX_WINDOW_SIGNATURE)
		{
			if (IsIconic(hwndPrevInst))
			{
				ShowWindow(hwndPrevInst, SW_SHOWNORMAL);
			}

			SetForegroundWindow(hwndPrevInst);
			BringWindowToTop(hwndPrevInst);
			ReleaseMutex(hmutexStartup);
			CloseHandle(hmutexStartup);
			return FALSE;
		}

		hwndPrevInst = GetWindow(hwndPrevInst, GW_HWNDNEXT);
	}

	m_hiconApp = LoadIcon(IDR_MAINFRAME);
	ASSERT(m_hiconApp);
	m_hcurFindTool = LoadCursor(IDC_FINDTOOL);
	ASSERT(m_hcurFindTool);
	m_hiconFindTool = LoadIcon(IDI_FINDTOOL);
	ASSERT(m_hiconFindTool);
	m_hiconFindTool2 = LoadIcon(IDI_FINDTOOL2);
	ASSERT(m_hiconFindTool2);

	m_BrushWindow.CreateSolidBrush(GetSysColor(COLOR_WINDOW));

	// hook up to the 3d control DLL (the new way)...
	if (_getmbcp() == 0)
		Enable3dControls();

	// set dialog background color to match system colors
	SetDialogBkColor(GetSysColor(COLOR_BTNFACE));

	LoadStdProfileSettings();  // Load standard INI file options (including MRU)

	if (RegOpenKey(HKEY_CURRENT_USER, m_szSpyKey, &hkey) != ERROR_SUCCESS)
	{
		hkey = NULL;
	}

	//
	// Get the default font from the registry
	//
	cbData = sizeof(m_DefLogFont);
	if (!hkey || RegQueryValueEx(hkey, m_szKeyDefFont, NULL, &dwType, (LPBYTE)&m_DefLogFont, &cbData) != ERROR_SUCCESS)
	{
		//
		// Cannot get the font from the registry.  Use the
		// system font as the default font.
		//
		HFONT hfont = (HFONT)GetStockObject(SYSTEM_FONT);
		::GetObject(hfont, sizeof(m_DefLogFont), &m_DefLogFont);
	}

	//
	// Get the main window's last position from the registy
	//
	fMainPosValid = FALSE;
	cbData = sizeof(wndpl);
	if (hkey && RegQueryValueEx(hkey, m_szKeyMainPos, NULL, &dwType, (LPBYTE)&wndpl, &cbData) == ERROR_SUCCESS)
	{
		if (m_nCmdShow != SW_SHOWNORMAL && m_nCmdShow != SW_SHOW)
		{
			//
			// The shell is launching the app in some special
			// state.  We should respect it no matter what
			// state the app was in when it was last closed.
			//
			wndpl.showCmd = m_nCmdShow;
		}
		else if (wndpl.showCmd == SW_SHOWMINIMIZED)
		{
			//
			// Don't start minimized if the app was last closed
			// when minimized.
			//
			wndpl.showCmd = SW_RESTORE;
		}

		wndpl.length = sizeof(wndpl);
		fMainPosValid = TRUE;
	}

	cbData = sizeof(BOOL);
	if (!hkey || RegQueryValueEx(hkey, m_szKeyDecodeStructs, NULL, &dwType, (LPBYTE)&fDecodeStructs, &cbData) != ERROR_SUCCESS)
	{
		//
		// Capture and decode structures and strings is on by default
		//
		fDecodeStructs = TRUE;
	}

	BOOL bIsMDIMaximized;
	cbData = sizeof(BOOL);
	if (!hkey || RegQueryValueEx(hkey, m_szMaximizeMDI, NULL, &dwType, (LPBYTE)&bIsMDIMaximized, &cbData) != ERROR_SUCCESS)
	{
		//
		// maximized MDI on startup is off by default
		//
		bIsMDIMaximized = FALSE;
	}

	//
	// Set the shared global that turns on/off decoding structs in the hook.
	//
	gfDecodeStructs = fDecodeStructs;

	//
	// Initialize the message document type static data.
	//
	CMsgDoc::InitStaticData(hkey);

	if (hkey)
	{
		RegCloseKey(hkey);
	}

	CPropertyInspector::InitializeTables();
	CHotLinkCtrl::InitStaticData();

	// Register the application's document templates.  Document templates
	// serve as the connection between documents, frame windows and views.
	// Use CSpyMDIChildWnd rather than CMDIChildWindow so that OnSize can
	// be trapped in order to save whether MDI state is maximized or not.

	m_pMsgDocTemplate = new CMultiDocTemplate(IDR_MSGTYPE,
			RUNTIME_CLASS(CMsgDoc), RUNTIME_CLASS(CSpyMDIChildWnd), RUNTIME_CLASS(CMsgView));
	AddDocTemplate(m_pMsgDocTemplate);

	m_pWndTreeDocTemplate = new CMultiDocTemplate(IDR_WNDTREETYPE,
			RUNTIME_CLASS(CWndTreeDoc), RUNTIME_CLASS(CSpyMDIChildWnd), RUNTIME_CLASS(CSpyTreeView));
	AddDocTemplate(m_pWndTreeDocTemplate);

	m_pPrcTreeDocTemplate = new CMultiDocTemplate(IDR_PRCTREETYPE,
			RUNTIME_CLASS(CPrcTreeDoc), RUNTIME_CLASS(CSpyMDIChildWnd), RUNTIME_CLASS(CSpyTreeView));
	AddDocTemplate(m_pPrcTreeDocTemplate);

	m_pThdTreeDocTemplate = new CMultiDocTemplate(IDR_THDTREETYPE,
			RUNTIME_CLASS(CThdTreeDoc), RUNTIME_CLASS(CSpyMDIChildWnd), RUNTIME_CLASS(CSpyTreeView));
	AddDocTemplate(m_pThdTreeDocTemplate);

	// create main MDI Frame window
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame->LoadFrame(IDR_MAINFRAME))
	{
		ReleaseMutex(hmutexStartup);
		CloseHandle(hmutexStartup);
		return FALSE;
	}

	if (fMainPosValid)
	{
		SetWindowPlacement(pMainFrame->m_hWnd, &wndpl);
	}
	else
	{
		pMainFrame->ShowWindow(m_nCmdShow);
	}

	pMainFrame->UpdateWindow();
	m_pMainWnd = pMainFrame;

	// check Windows version and set m_bIsChicago flag accordingly.
	DWORD dwWindowsVersion = GetVersion();
	// check the 'Win32s' bit
	if (dwWindowsVersion & 0x80000000)
	{
		// bit is set, we are running on either Win32s or Chicago
		if (LOBYTE(LOWORD(dwWindowsVersion)) > 3)
		{
			m_bIsChicago = TRUE;
			gfOnChicago = TRUE;

			// now set up the Chicago ToolHelp function pointers
			if ((hToolhelp32 = LoadLibrary("kernel32.dll")) != NULL)
			{
				pfnCreateToolhelp32Snapshot = (SNAPSHOTPROC)GetProcAddress(hToolhelp32, "CreateToolhelp32Snapshot");
				pfnHeap32ListFirst = (HEAPLISTPROC)GetProcAddress(hToolhelp32, "Heap32ListFirst");
				pfnHeap32ListNext = (HEAPLISTPROC)GetProcAddress(hToolhelp32, "Heap32ListNext");
				pfnHeap32First = (HEAP32FIRSTPROC)GetProcAddress(hToolhelp32, "Heap32First");
				pfnHeap32Next = (HEAP32NEXTPROC)GetProcAddress(hToolhelp32, "Heap32Next");
				pfnToolhelp32ReadProcessMemory = (READPROCMEMPROC)GetProcAddress(hToolhelp32, "Toolhelp32ReadProcessMemory");
				pfnProcess32First = (PROCESSLISTPROC)GetProcAddress(hToolhelp32, "Process32First");
				pfnProcess32Next = (PROCESSLISTPROC)GetProcAddress(hToolhelp32, "Process32Next");
				pfnThread32First = (THREADLISTPROC)GetProcAddress(hToolhelp32, "Thread32First");
				pfnThread32Next = (THREADLISTPROC)GetProcAddress(hToolhelp32, "Thread32Next");
				pfnModule32First = (MODULELISTPROC)GetProcAddress(hToolhelp32, "Module32First");
				pfnModule32Next = (MODULELISTPROC)GetProcAddress(hToolhelp32, "Module32Next");

				if (pfnCreateToolhelp32Snapshot == NULL || 
					pfnHeap32ListFirst == NULL || 
					pfnHeap32ListNext == NULL || 
					pfnHeap32First == NULL || 
					pfnHeap32Next == NULL || 
					pfnToolhelp32ReadProcessMemory == NULL || 
					pfnProcess32First == NULL || 
					pfnProcess32Next == NULL || 
					pfnThread32First == NULL || 
					pfnThread32Next == NULL || 
					pfnModule32First == NULL || 
					pfnModule32Next == NULL)
				{
					FreeLibrary(hToolhelp32);
					hToolhelp32 = NULL;
				}
			}

			if (hToolhelp32 == NULL)
			{
				// warn user that process/thread information will not be available
				SpyMessageBox(IDS_PT_NOT_AVAILABLE);
			}
		}
		else
		{
			// we are on Win32s, bail!
//			SpyMessageBox(IDS_OS_IS_WIN32S, MB_OK, MB_ICONSTOP);
			::MessageBox(m_pMainWnd->m_hWnd, ids(IDS_OS_IS_WIN32S), "Spy++", MB_OK | MB_ICONSTOP | MB_TASKMODAL);
			return(FALSE);
		}
	}
	else
	{
		// we are running on Windows NT, check if it's Daytona...
		if ((LOBYTE(LOWORD(dwWindowsVersion)) == 3) && (HIBYTE(LOWORD(dwWindowsVersion)) >= 50))
		{
			m_bIsDaytona = TRUE;
			gfOnDaytona = TRUE;
		}
	}

	// TODO: add code here to reload the last size and positions of the
	// child MDI tree views.  But don't do message stream views because the
	// hwnd they were spying on may or may not be valid anymore!  Only
	// create a default view if there are no MDI children saved from
	// last time.

	m_pWndTreeDocTemplate->OpenDocumentFile(NULL);

	m_bIsMDIMaximized = bIsMDIMaximized;
	if (m_bIsMDIMaximized)
	{
		CMDIChildWnd *pcmcwActive = (CMDIChildWnd *)(((CMDIFrameWnd *)m_pMainWnd)->MDIGetActive());
		((CMDIFrameWnd *)m_pMainWnd)->MDIMaximize(pcmcwActive);
	}

	CreateHookThread();

	ReleaseMutex(hmutexStartup);
	CloseHandle(hmutexStartup);
	OnSpyMessages();
	return TRUE;
}

int CSpyApp::ExitInstance()
{
	HKEY hkey;

	if (RegCreateKey(HKEY_CURRENT_USER, m_szSpyKey, &hkey) == ERROR_SUCCESS)
	{
		RegSetValueEx(hkey, m_szKeyDefFont, 0, REG_BINARY, (LPBYTE)&m_DefLogFont, sizeof(m_DefLogFont));

		if (m_fMainWndplValid)
		{
			RegSetValueEx(hkey, m_szKeyMainPos, 0, REG_BINARY, (LPBYTE)&m_wndplMain, sizeof(m_wndplMain));
		}

		RegSetValueEx(hkey, m_szKeyDecodeStructs, 0, REG_DWORD, (LPBYTE)&gfDecodeStructs, sizeof(BOOL));

		RegSetValueEx(hkey, m_szMaximizeMDI, 0, REG_DWORD, (LPBYTE)&m_bIsMDIMaximized, sizeof(BOOL));

		CMsgDoc::SaveSettings(hkey);

		RegCloseKey(hkey);
	}

	if (m_bDidPerfQueries)
	{
		// RegCloseKey is required so that network transports 
		// and drivers can be removed or installed (which 
		// cannot happen while they are open for monitoring.)
		RegCloseKey(HKEY_PERFORMANCE_DATA);
	}

	if (hToolhelp32)
		FreeLibrary(hToolhelp32);

	idsDelete();

	return CWinApp::ExitInstance();
}

void CSpyApp::OnAppAbout()
{
	CAboutDlg Dlg;

	Dlg.DoModal();
}

void CSpyApp::OnSpyMessages()
{
	// We only show the message log window after the user selects OK from the 
	// Messages.Options dialog.  If the user hits the Cancel button in Messages.Options,
	// we should destroy the message log window without ever showing it.

	// don't allow immediate spying when message log created--wait until after
	// message options dialog is dismissed (with OK).
	SetSpyImmediate(FALSE);

	// create the document with view hidden (until OK pressed in message options)
	// by calling OpenDocumentFile with bMakeVisible set to FALSE.
	CMsgDoc* pMsgDoc = (CMsgDoc*)m_pMsgDocTemplate->OpenDocumentFile(NULL, FALSE);
	if (pMsgDoc)
	{
		if (pMsgDoc->ShowWindowFinder() == IDCANCEL)
			pMsgDoc->OnCloseDocument();
		else
		{
			POSITION pos = pMsgDoc->GetFirstViewPosition();
			CView *pView = pMsgDoc->GetNextView(pos);
			ASSERT(pos == NULL);	// should only be one view!

			// now perform the initial update which was not previously performed
			// because we called OpenDocumentFile() with bMakeVisible set to FALSE.
			CFrameWnd *pFrame = (CFrameWnd *)pView->GetParent();

			// comment out to get V2 build to work.
			//pFrame->InitialUpdateFrame(pMsgDoc, TRUE);

			pMsgDoc->StartLogging();
		}
	}
}

void CSpyApp::OnSpyMessagesDirect()
{
	// allow immediate spying when message log created.
	SetSpyImmediate(TRUE);

	m_pMsgDocTemplate->OpenDocumentFile(NULL);
}

void CSpyApp::OnSpyHighlightWindow()
{
	FlashWindowBorder((HWND)GetLastSelectedObject());
}

void CSpyApp::OnSpyWindows()
{
	BeginWaitCursor();
	m_pWndTreeDocTemplate->OpenDocumentFile(NULL);
	EndWaitCursor();
}

void CSpyApp::OnSpyProcesses()
{
	BeginWaitCursor();
	m_pPrcTreeDocTemplate->OpenDocumentFile(NULL);
	EndWaitCursor();
}

void CSpyApp::OnUpdateSpyProcesses(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(!m_bIsChicago || hToolhelp32 != NULL);
}

void CSpyApp::OnSpyThreads()
{
	BeginWaitCursor();
	m_pThdTreeDocTemplate->OpenDocumentFile(NULL);
	EndWaitCursor();
}

void CSpyApp::OnUpdateSpyThreads(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(!m_bIsChicago || hToolhelp32 != NULL);
}

void CSpyApp::OnSpyFindWindow()
{
	CFindWindowDlg dlg(theApp.m_pMainWnd);

	if (dlg.DoModal() == IDOK)
	{
		// user pressd OK in dialog
		if (dlg.GetSelectedHwnd() != NULL)
		{
			// window handle is not NULL and is valid
			if (dlg.m_ShowProperties)
			{
				// user wants to see properties of window
				CPropertyInspector::ShowObjectProperties((DWORD)dlg.GetSelectedHwnd(), OT_WINDOW);
			}
			else if (dlg.m_ShowMessages)
			{
				// user wants to see message stream for window
				SetLastSelectedObject((DWORD)dlg.GetSelectedHwnd(), OT_WINDOW);
				OnSpyMessagesDirect();
			}
		}
	}
}

BOOL CSpyApp::OnIdle(LONG lCount)
{
	if (CWinApp::OnIdle(lCount))
	{
		return 1;
	}

	ChangeToLastSelectedObject();

	return 0;
}

//-----------------------------------------------------------------------------
// ids
//
// This function will return a string, given the string id.  If this
// is the first time that the string has been retrieved, memory will
// be allocated for it and it will be loaded.  After it is loaded once,
// it is then cached in an array and is available for later without
// having to load it again.
//
// Arguments:
//  UINT idString - String ID of the string to retrieve.
//
// Returns:
//  Pointer to the string.
//
//-----------------------------------------------------------------------------

static CHAR* apsz[MAX_STRINGIDS];	   // String resource array cache.

CHAR* ids(UINT idString)
{
	CHAR* psz;
	int index;

	//
	// Check for a string id that is too low.  They must start
	// at STRINGID_BASE.
	//
	ASSERT(idString >= STRINGID_BASE);

	//
	// Check for a string id that is too high.  The value of
	// MAX_STRINGIDS must be increased!
	//
	ASSERT(idString < STRINGID_BASE + MAX_STRINGIDS);

	index = idString - STRINGID_BASE;
	if (apsz[index])
	{
		return apsz[index];
	}

	CString Str;

	if (Str.LoadString(idString))
	{
		psz = new CHAR[Str.GetLength() + 1];
		_tcscpy(psz, Str);
		apsz[index] = psz;
		return psz;
	}
	else
	{
		//
		// Right now we return something that will keep the caller from
		// faulting.  But are we obfuscating a potential problem?
		//
		return "";
	}
}

//-----------------------------------------------------------------------------
// idsDelete
//
// This function will return clean up the apsz string array.
// If the array is not cleaned up, it's a memory leak!
//
//-----------------------------------------------------------------------------

void idsDelete()
{
	CHAR* psz;
	int index;

	for (index = STRINGID_BASE; index < STRINGID_BASE + MAX_STRINGIDS; index++)
	{
		if ((psz = apsz[index-STRINGID_BASE]) != NULL)
			delete psz;
	}
}

//-----------------------------------------------------------------------------
// GetExpandedClassName
//
//
//
// Arguments:
//
//
//
// Returns:
//
//
//-----------------------------------------------------------------------------

LPSTR GetExpandedClassName(LPSTR pszClass)
{
	if (*pszClass == '#')
	{
		if (_tcscmp(pszClass, "#32768") == 0)
		{
			return ids(IDS_POPUPMENU);
		}
		else if (_tcscmp(pszClass, "#32769") == 0)
		{
			return ids(IDS_DESKTOP);
		}
		else if (_tcscmp(pszClass, "#32770") == 0)
		{
			return ids(IDS_DIALOG);
		}
		else if (_tcscmp(pszClass, "#32771") == 0)
		{
			return ids(IDS_WINSWITCH);
		}
		else if (_tcscmp(pszClass, "#32772") == 0)
		{
			return ids(IDS_ICONTITLE);
		}
	}

	return NULL;
}

//-----------------------------------------------------------------------------
// SpyMessageBox
//
//
//
// Arguments:
//
//
//
// Returns:
//
//
//-----------------------------------------------------------------------------

int SpyMessageBox(UINT nIDPrompt, UINT nType, UINT nIcon)
{
	return AfxMessageBox(nIDPrompt, nType | nIcon | MB_TASKMODAL);
}

/////////////////////////////////////////////////////////////////////////////
// following code is copied from VSHELL\STDFONT.CPP
#define MAX_FONT_NAME 51
struct FontEntry
{
	TCHAR szName[MAX_FONT_NAME];	// REVIEW: Is this too small?
	int iSize;
	BOOL bAdjustSize;	// if true use -MulDiv(iSize, LOGPIXELSY, 72)
};

// the following table is from Visual C++ shell.
// dolphin 13830 [patbr] changed from Spy++ table
static BASED_CODE FontEntry FontNames[4][3] =
{
	{
		{"MS Sans Serif",	8,	TRUE},
		{"‚l‚r –¾’©",		12,	TRUE},
		{"System",			10,	TRUE}
	}, {
		{"Small Fonts",		-9,	FALSE},
		{"Terminal",		-9,	FALSE},
		{"Terminal",		-9,	FALSE}
	}, {
		{"Courier",			14,	FALSE},
		{"FixedSys",		14,	FALSE},
		{"FixedSys",		14,	FALSE}
	}, {
		{"MS Sans Serif",	8,	TRUE},
		{"‚l‚r –¾’©",		12,	TRUE},
		{"System",			10,	TRUE}
	}
};

static FontEntry* GetStdFontInfo(int iType, int* pnCharset = NULL)
{
	ASSERT(iType >= 0 && iType < font_count);

	int iFacename, nCharset;

	switch (_getmbcp())
	{
	case OEM_JAPAN:
		iFacename = 1;
		nCharset = SHIFTJIS_CHARSET;
		break;

	case OEM_PRC:
	case OEM_TAIWAN:
	case OEM_KOR_WANGSUNG:
	case OEM_KOR_JOHAB:
		iFacename = 2;
		nCharset = SHIFTJIS_CHARSET;		// REVIEW: What should this be?
		break;

	default:
		iFacename = 0;
		nCharset = ANSI_CHARSET;
		break;
	}
	
	if (pnCharset != NULL)
		*pnCharset = nCharset;

	static FontEntry fe;
/*
	CString strValName;
	switch (iType/2)
	{
		case 0:
			strValName = "Normal";
			break;
		case 1:
			strValName = "Small";
			break;
		case 2:
			strValName = "Fixed";
			break;
		default:
			// An invalid font type
			ASSERT( 0 );
	}
*/
	fe = FontNames[iType/2][iFacename];

	return &fe;
}

void GetStdFontInfo(int iType, CString& strFace, int& ptSize)
{
	FontEntry* pfe = GetStdFontInfo(iType);
	strFace = pfe->szName;
	ptSize = pfe->iSize;
}

// NOTE: The LOGFONT returned by this function is temporary!
const LOGFONT* GetStdLogfont( const int iType, CDC *pDC /* = NULL */ )
{
	static LOGFONT lf;
	
	int nCharset;
	FontEntry* pfe = GetStdFontInfo(iType, &nCharset);

	if (!pDC)
	{
		CWindowDC dc(NULL);
		lf.lfHeight = pfe->bAdjustSize ? -MulDiv(pfe->iSize, dc.GetDeviceCaps(LOGPIXELSY), 72) : pfe->iSize;
	}
	else
	{
		lf.lfHeight = pfe->bAdjustSize ? -MulDiv(pfe->iSize, pDC->GetDeviceCaps(LOGPIXELSY), 72) : pfe->iSize;
	}
	lf.lfWidth = 0;
	lf.lfEscapement = 0;
	lf.lfOrientation = 0;
	lf.lfWeight = ((iType & 1) && (_getmbcp() == 0)) ? FW_BOLD : FW_NORMAL;
	lf.lfItalic = FALSE;
	if (iType == font_NormalUnderline || iType == font_NormalUnderlineBold)
		lf.lfUnderline = TRUE;
	else
		lf.lfUnderline = FALSE;
	lf.lfStrikeOut = FALSE;
	lf.lfCharSet = nCharset;
	lf.lfOutPrecision = OUT_DEFAULT_PRECIS;
	lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	lf.lfQuality = DEFAULT_QUALITY;
	lf.lfPitchAndFamily = DEFAULT_PITCH | FW_DONTCARE;
	lstrcpy( lf.lfFaceName, pfe->szName );

	return &lf;
}

CFont* GetStdFont(const int iType)
{
	static CFont fonts[font_count];
	
	ASSERT(iType >= 0 && iType < font_count);
	
	if (fonts[iType].m_hObject == NULL)
		VERIFY(fonts[iType].CreateFontIndirect(GetStdLogfont(iType)));
	
	return &fonts[iType];
}
