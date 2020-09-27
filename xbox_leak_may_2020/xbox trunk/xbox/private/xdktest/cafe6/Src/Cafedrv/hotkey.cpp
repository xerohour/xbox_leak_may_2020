///////////////////////////////////////////////////////////////////////////////
//	HOTKEY.CPP
//
//	Created by :			Date :
//		DavidGa					8/13/93
//
//	Description :
//		Implementation of the CHotKeyWnd class
//

#include "stdafx.h"
#include "hotkey.h"
#include "cafedrv.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

/////////////////////////////////////////////////////////////////////////////
// CHotKeyThread

IMPLEMENT_DYNCREATE(CHotKeyThread, CWinThread)

CHotKeyThread::CHotKeyThread()
{
	m_pfnThreadProc = NULL;
}

BOOL CHotKeyThread::InitInstance()
{
//	CHotKeyWnd* pWnd = new CHotKeyWnd;
	m_wndHK.Create();
	// hide the window to start
	m_wndHK.ShowWindow(SW_HIDE);

	return TRUE;
}

CHotKeyThread::~CHotKeyThread()
{
}


/////////////////////////////////////////////////////////////////////////////
// CHotKeyWnd

IMPLEMENT_DYNCREATE(CHotKeyWnd, CWnd)

BEGIN_MESSAGE_MAP(CHotKeyWnd, CWnd)
	//{{AFX_MSG_MAP(CHotKeyWnd)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CLOSE()
    ON_WM_MOVE()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_HOTKEY, OnHotKey)
	ON_MESSAGE(WM_SIZING, OnSizing)
	ON_MESSAGE(WM_REGISTERHOTKEYS, OnRegisterHotKeys)
	ON_MESSAGE(WM_UNREGISTERHOTKEYS, OnUnregisterHotKeys)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHotKeyWnd construction/destruction

CHotKeyWnd::CHotKeyWnd()
: m_psettingsWnd(NULL)
{
	g_hSingleStep = CreateEvent(NULL,		// security attributes
								TRUE,		// manual reset
								FALSE,		// initial state
								"HotKey_SingleStep_Event");
// REVIEW: we need to create the event anyway, always signaled, when not using this feature
	g_hUserAbort = CreateEvent(NULL,		// security attributes
								FALSE,		// manual reset
								FALSE,		// initial state
								"HotKey_UserAbort_Event");
}

CHotKeyWnd::~CHotKeyWnd()
{
	g_hwndHotKey = NULL;

	// delete the settings object
	delete m_psettingsWnd;
}

/////////////////////////////////////////////////////////////////////////////
// CHotKeyWnd diagnostics

#ifdef _DEBUG
void CHotKeyWnd::AssertValid() const
{
	CWnd::AssertValid();
}

void CHotKeyWnd::Dump(CDumpContext& dc) const
{
	CWnd::Dump(dc);
}

#endif //_DEBUG

void CHotKeyWnd::RegisterHotKeys(void)
{
    VERIFY( ::RegisterHotKey(m_hWnd, HOTKEY_SINGLE, 0, VK_F2) );
    VERIFY( ::RegisterHotKey(m_hWnd, HOTKEY_STEPOVER, MOD_SHIFT, VK_F2) );
    VERIFY( ::RegisterHotKey(m_hWnd, HOTKEY_RUN, MOD_CONTROL, VK_F2) );
    VERIFY( ::RegisterHotKey(m_hWnd, HOTKEY_ABORT, MOD_CONTROL | MOD_SHIFT | MOD_ALT, VK_END) );
    VERIFY( ::RegisterHotKey(m_hWnd, HOTKEY_STEPBREAK, MOD_SHIFT, VK_SCROLL) );

}

void CHotKeyWnd::UnregisterHotKeys(void)
{
    ::UnregisterHotKey(m_hWnd, HOTKEY_RUN);
    ::UnregisterHotKey(m_hWnd, HOTKEY_SINGLE);
    ::UnregisterHotKey(m_hWnd, HOTKEY_STEPOVER);
    ::UnregisterHotKey(m_hWnd, HOTKEY_ABORT);
    ::UnregisterHotKey(m_hWnd, HOTKEY_STEPBREAK);
}

/////////////////////////////////////////////////////////////////////////////
// CHotKeyWnd message handlers

BOOL CHotKeyWnd::Create(void)
{
	return CWnd::CreateEx(
		WS_EX_TOPMOST,	// extended style
		// AfxRegisterWndClass(WS_POPUP | WS_THICKFRAME | WS_VISIBLE),	 // Don't need this
		AfxRegisterWndClass(CS_GLOBALCLASS),
		"Comments about the next step will be here!",	// window name
		WS_POPUP | WS_CAPTION | WS_THICKFRAME | WS_VISIBLE,	// style
		0,			// window rect
		GetSystemMetrics(SM_CYSCREEN) - GetSystemMetrics(SM_CYCAPTION) - GetSystemMetrics(SM_CYBORDER) - 1,
		GetSystemMetrics(SM_CXSCREEN), //make it screen-wide
		GetSystemMetrics(SM_CYCAPTION),
		NULL,		// parent window,
		NULL,		// nlDorHMenu
		NULL		// lpPararm
	);
}

int CHotKeyWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if( CWnd::OnCreate(lpCreateStruct) == -1 )
		return -1;

	// get a pointer to the app
	CCAFEDriver *pApp = ((CCAFEDriver *)AfxGetApp());

	// register the break key (Scroll Lock)
    VERIFY( ::RegisterHotKey(m_hWnd, HOTKEY_BREAK, 0, VK_PAUSE) );

	// create a new window settings object
	m_psettingsWnd = new CSettings(pApp->GetSettings()->GetTextValue(settingCAFEKey)+"\\Step window");

	// set up window defaults (this window's current position)
    m_psettingsWnd->SetIntValue(settingWaitStepWndX, lpCreateStruct->x);
    m_psettingsWnd->SetIntValue(settingWaitStepWndY, lpCreateStruct->y);
    m_psettingsWnd->SetIntValue(settingWaitStepWndWidth, lpCreateStruct->cx);
    m_psettingsWnd->SetIntValue(settingWaitStepWndHeight, lpCreateStruct->cy);

	// read registry settings
	m_psettingsWnd->ReadRegistry();
	// write the settings back out to initialize registry
	m_psettingsWnd->WriteRegistry();

	#ifdef _DEBUG
		m_psettingsWnd->DumpSettings("Step window settings");
	#endif // _DEBUG

	// position the window in the correct position
    MoveWindow(m_psettingsWnd->GetIntValue(settingWaitStepWndX),
               m_psettingsWnd->GetIntValue(settingWaitStepWndY),
               m_psettingsWnd->GetIntValue(settingWaitStepWndWidth),
               m_psettingsWnd->GetIntValue(settingWaitStepWndHeight));

	g_hwndHotKey = m_hWnd;
	return 0;
}

void CHotKeyWnd::OnClose()
{
	// store the position of the hot key window
    SavePosition();

	// unregister all hotkeys
	UnregisterHotKeys();
    ::UnregisterHotKey(m_hWnd, HOTKEY_BREAK);

    CWnd::OnClose();

	// terminate the thread, too
	AfxEndThread(0);
}

void CHotKeyWnd::OnMove(int x, int y)
{
    CWnd::OnMove(x, y);

    SavePosition();
}

void CHotKeyWnd::OnSizing( UINT nSide, LPRECT lpRect )
{
    CWnd::OnSizing(nSide, lpRect);
	RECT orgRect;
	if(nSide & (WMSZ_BOTTOM|WMSZ_TOP))
	{ //don't allow to resize anything but left-right
		GetWindowRect(&orgRect);
		lpRect->top=orgRect.top;
		lpRect->bottom=orgRect.bottom;
	}
}

void CHotKeyWnd::SavePosition()
{
    CRect rect;
    GetWindowRect(rect);

	// the settings object must be valid
	ASSERT(m_psettingsWnd);

	// store the window's current position
    m_psettingsWnd->SetIntValue(settingWaitStepWndX, rect.left);
    m_psettingsWnd->SetIntValue(settingWaitStepWndY, rect.top);
    m_psettingsWnd->SetIntValue(settingWaitStepWndWidth, rect.Width());
    m_psettingsWnd->SetIntValue(settingWaitStepWndHeight, rect.Height());

	// write settings to the registry
	m_psettingsWnd->WriteRegistry();
}

LRESULT CHotKeyWnd::OnHotKey(WPARAM wParam, LPARAM lParam)
{
	switch( wParam )
	{
		case HOTKEY_SINGLE:		// step a single action
		case HOTKEY_STEPOVER:	// step over the rest of this test case
			g_hkToDo = (EHotKey)wParam;
			SetEvent(g_hSingleStep);	// release anything waiting
			break;
		case HOTKEY_RUN:		// no more waiting
		case HOTKEY_BREAK:		// breaks run mode
			if( g_hkToDo == HOTKEY_RUN )	// now this hotkey toggles between run and break
			{
				// register all hot keys
				RegisterHotKeys();
				g_hkToDo = HOTKEY_SINGLE;
				ResetEvent(g_hSingleStep);	// exit run mode
				SetWindowText("Applying brakes...");
			}
			else
			{
				// unregister all hot keys
				UnregisterHotKeys();
				g_hkToDo = HOTKEY_RUN;
				SetEvent(g_hSingleStep);	// release anything waiting
			}
			break;
		case HOTKEY_ABORT:
			// unregister all hot keys
			UnregisterHotKeys();
			SetEvent(g_hUserAbort);		// user abort
			g_hkToDo = HOTKEY_RUN;
			SetEvent(g_hSingleStep);    // Release anything waiting.
			SetWindowText("Aborting...");
			break;
		case HOTKEY_STEPBREAK:
			if( g_hkToDo != HOTKEY_STEPBREAK )	// now this hotkey brings the dialog
			{
				// unregister all hot keys
				UnregisterHotKeys();
			    ::UnregisterHotKey(m_hWnd, HOTKEY_BREAK);
				int gotglobalstep=g_stopStep;
				g_stopStep=-1; // cause the WaitStepInstructions to stop unconditionnally
				g_hkToDo = HOTKEY_SINGLE;
				ResetEvent(g_hSingleStep);	// exit run mode
				SetWindowText("Applying breakpoint...");
				HWND hwndTarget = ::GetForegroundWindow();	// Save active target window.
				::SetForegroundWindow(g_hwndHotKey);	
				CStepBreak dlg(gotglobalstep);
				int nResponse = dlg.DoModal();
				if (nResponse == IDOK)
				{
					gotglobalstep=dlg.m_iStepNumber;
					g_hkToDo = HOTKEY_STEPBREAK;
				}
				else if (nResponse == IDCANCEL)
				{
				}
				::SetForegroundWindow(hwndTarget);			// Restore focus to target.
		
				while(::GetForegroundWindow() != hwndTarget)	// Wait for full activation!
					;

				g_stopStep=gotglobalstep;
				// reregister all hotkey
				RegisterHotKeys();
				VERIFY( ::RegisterHotKey(m_hWnd, HOTKEY_BREAK, 0, VK_SCROLL) );
//				if (nResponse == IDOK)
//					SetEvent(g_hSingleStep);   // do not wait to resume
			}
			else // hotkay restarts the run 
			{
				g_hkToDo = HOTKEY_SINGLE;
				SetEvent(g_hSingleStep);    // Release anything waiting.
			}
			break;
		default:
			ASSERT(FALSE);	// there are no other hot keys
	}
	return 0;
}

LRESULT CHotKeyWnd::OnRegisterHotKeys(WPARAM wParam, LPARAM lParam)
{
	// register hot keys
	RegisterHotKeys();

	return 0;
}

LRESULT CHotKeyWnd::OnUnregisterHotKeys(WPARAM wParam, LPARAM lParam)
{
	// unregister hot keys
	UnregisterHotKeys();

	return 0;
}

