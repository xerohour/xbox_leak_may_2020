//////////////////////////////////////////////////////////////////////////////
//      DLGBASE.CPP
//              Implementation for Sushi dialog base classes.
//

#include "stdafx.h"

#include "shell.h"
#include "dlgs.h"
#include "about.h"
#include "resource.h"

#include "shell.hid" // 26 Jun 96 - The combobox and the listbox need help ids for the
						// Chooser need help ids.

#include <prjapi.h>
#include <prjguid.h>
#include "toolexpt.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

extern HWND PASCAL _SushiGetSafeOwner(CWnd* pParent);
extern UINT DSM_TESTMENU; 

///////////////////////////////////////////////////////////////////////////////
//      C3dDialog
//
C3dDialog::C3dDialog(LPCSTR lpszTemplateName, CWnd* pParentWnd)
    : CDialog(lpszTemplateName, FromHandle(_SushiGetSafeOwner(pParentWnd)))
{
}

C3dDialog::C3dDialog(UINT nTemplateID, CWnd* pParentWnd)
    : CDialog(nTemplateID, FromHandle(_SushiGetSafeOwner(pParentWnd)))
{
}

void C3dDialog::OnContextMenu(CWnd* pWnd, CPoint pt) 
{
	DoHelpContextMenu(this, m_nIDHelp, pt);
}

BOOL C3dDialog::OnHelpInfo(HELPINFO* pInfo) 
{
	return DoHelpInfo(this, m_nIDHelp, pInfo);
}

LRESULT C3dDialog::OnTestMenu(WPARAM wParam, LPARAM lParam)
{
	CMainFrame *pFrame=(CMainFrame*)AfxGetMainWnd();
	
	return pFrame->SendMessage(DSM_TESTMENU, wParam,lParam);
}

int WideStrLen(WCHAR* pwch)
{
	int nLen = 0;
	while (*pwch != 0)
	{
		pwch += 1;
		nLen += 1;
	}
	
	return nLen;
}


//++PATCH_CONTEXT_HELP
//  Borrow DLGTEMPLATEEX from AFXIMPL.H
#pragma pack(push, 1)
typedef struct
{
	WORD dlgVer;
	WORD signature;
	DWORD helpID;
	DWORD exStyle;
	DWORD style;
	WORD cDlgItems;
	short x;
	short y;
	short cx;
	short cy;
} DLGTEMPLATEEX;
#pragma pack(pop)

inline static BOOL IsDialogEx(const DLGTEMPLATE* pTemplate)
{
	return ((DLGTEMPLATEEX*)pTemplate)->signature == 0xFFFF;
}

// BUGBUG:
// Res editor applies WS_EX_CONTEXTHELP for Context Help
// Ok for Win95, but NT 3,51 wants DS_CONTEXTHELP
//
static void PatchContextHelpStyle(HGLOBAL hTemplate)
{
	DLGTEMPLATE* pTemplate = (DLGTEMPLATE*)GlobalLock(hTemplate);
	if (IsDialogEx(pTemplate))
	{
		if (((DLGTEMPLATEEX*)pTemplate)->exStyle & WS_EX_CONTEXTHELP)
			((DLGTEMPLATEEX*)pTemplate)->style |= DS_CONTEXTHELP;
	}
	else
	{
		if (pTemplate->dwExtendedStyle & WS_EX_CONTEXTHELP)
			pTemplate->style |= DS_CONTEXTHELP;
	}
	GlobalUnlock(hTemplate);
}
//--PATCH_CONTEXT_HELP

void SetStdFont(C3dDialogTemplate & dt)
{
	extern void GetStdFontInfo(int iType, CString& strFace, int& ptSize);

	CString strFace;
	int ptSize;
	GetStdFontInfo(font_Bold, strFace, ptSize);

	dt.SetFont(strFace, (WORD)ptSize);
}

int C3dDialog::DoModal()
{
	C3dDialogTemplate dt;
	LPCTSTR lpOldDialogTemplate = m_lpszTemplateName;
	if (dt.Load(m_lpszTemplateName))
	{
		SetStdFont(dt);
		m_lpszTemplateName = NULL;
		m_hDialogTemplate = NULL;
		PatchContextHelpStyle(dt.GetTemplate()); // REVIEW: (see note on function def. above)
		InitModalIndirect(dt.GetTemplate());
	}

	PreModalWindow();
	int nRet = CDialog::DoModal();
	PostModalWindow();
	
	m_lpszTemplateName = lpOldDialogTemplate;
	return nRet;
}

BOOL C3dDialog::Create(LPCTSTR lpszTemplateName, CWnd* pParentWnd)
{
	C3dDialogTemplate dt;
	if (dt.Load(lpszTemplateName))
	{
		PatchContextHelpStyle(dt.GetTemplate()); // REVIEW: (see note on function def. above)
		SetStdFont(dt);
	}
	
	return CDialog::CreateIndirect(dt.GetTemplate(), pParentWnd);
}


BEGIN_MESSAGE_MAP(C3dDialog, CDialog)
    ON_COMMAND(IDOK, OnRobustOK)
	ON_WM_CONTEXTMENU()
	ON_WM_HELPINFO()
END_MESSAGE_MAP()

void C3dDialog::OnRobustOK()
{
#if 0   // FUTURE: Handle low memory.
    if (thePackage.CheckForEmergency())
    {
	SendMessage(WM_COMMAND, IDCANCEL);
	return;
    }
#endif

    OnOK(); // ok to call "real" OnOK
}

///////////////////////////////////////////////////////////////////////////////
//      C3dDialogBar
//

IMPLEMENT_DYNAMIC(C3dDialogBar, CDialogBar)

C3dDialogBar::C3dDialogBar()
{
	m_nDockableID = 0;
}

BOOL C3dDialogBar::Create(CWnd* pParentWnd, LPCTSTR lpszTemplateName, UINT nStyle, UINT nID)
{
	C3dDialogTemplate dt;
	if (dt.Load(lpszTemplateName))
		SetStdFont(dt);

	nStyle |= (CBRS_TOOLTIPS | CBRS_FLYBY);
	
	if (!CreateIndirect(dt.GetTemplate(), pParentWnd, nStyle, nID))
		return FALSE;

	return ExecuteDlgInit(lpszTemplateName);
}

BOOL C3dDialogBar::CreateIndirect(const void *lpDialogTemplate, CWnd* pParentWnd, UINT nStyle, UINT nID)
{
	ASSERT(pParentWnd != NULL);
	ASSERT(lpDialogTemplate != NULL);

	// allow chance to modify styles
	m_dwStyle = nStyle;
	CREATESTRUCT cs;        // This is only used for a call to PreCreateWindow().
	memset(&cs, 0, sizeof(cs));
	cs.lpszClass = NULL;
	cs.style = (DWORD)nStyle | WS_CHILD;
	cs.hMenu = (HMENU)nID;
	cs.hInstance = AfxGetInstanceHandle();
	cs.hwndParent = pParentWnd->GetSafeHwnd();
	if (!PreCreateWindow(cs))
		return FALSE;

	// create a modeless dialog
	AfxHookWindowCreate(this);
	HWND hWnd = ::CreateDialogIndirect(AfxGetInstanceHandle(),
		(LPCDLGTEMPLATE)lpDialogTemplate, pParentWnd->GetSafeHwnd(), NULL);
	if (!AfxUnhookWindowCreate())
		PostNcDestroy();        // cleanup if Create fails too soon

	if (hWnd == NULL)
		return FALSE;
	ASSERT(hWnd == m_hWnd);

	// dialog template MUST specify that the dialog
	//      is an invisible child window
	SetDlgCtrlID(nID);
	CRect rect;
	GetWindowRect(&rect);
	m_sizeDefault = rect.Size();    // set fixed size

	// force WS_CLIPSIBLINGS
	ModifyStyle(0, WS_CLIPSIBLINGS);

	// Make sure we get parent notifications from our children.
	for (CWnd* pWnd = GetTopWindow(); pWnd != NULL;
			pWnd = pWnd->GetWindow(GW_HWNDNEXT))
		pWnd->ModifyStyleEx(WS_EX_NOPARENTNOTIFY, 0);

	// allow CTL3D32.DLL to subclass the dialog and its controls
	// REVEIW_MFC: we shouldn't have to explicitly enable 3d controls!
//      if (!theApp.m_bWin4)
//              SubclassDlg3d();
		
	// force the size to zero - resizing bar will occur later
	SetWindowPos(NULL, 0, 0, 0, 0, SWP_NOZORDER|SWP_NOACTIVATE|SWP_SHOWWINDOW);

	return TRUE;
}

void C3dDialogBar::RecalcBarLayout(DOCKPOS dp, BOOL bForceCenter /*=FALSE*/)
{
	ASSERT(m_nDockableID);

	DWORD dwStyleOld = GetBarStyle();

	m_sizeDefault.cy = CY_TOOLBAR;

	if (theApp.m_bWin4)
	{
		if (dp != dpMDI)
			SetBarStyle(dwStyleOld & ~CBRS_BORDER_TOP);
		else
		{
			m_sizeDefault.cy += CY_BORDER * 2;
			SetBarStyle(dwStyleOld | CBRS_BORDER_TOP);
		}
	}

	if (bForceCenter || dwStyleOld != GetBarStyle())
		VertCenterControls();
}

void C3dDialogBar::VertCenterControls(int cySize /*=0*/)
{
	if (cySize == 0)
	{
		cySize = m_sizeDefault.cy;

		DWORD dwStyle = GetBarStyle();

		int cyBorder = CY_BORDER;
		if (dwStyle & CBRS_BORDER_3D)
			cyBorder = CY_BORDER * 2;

		if (dwStyle & CBRS_BORDER_TOP)
			cySize += cyBorder;
		if (dwStyle & CBRS_BORDER_BOTTOM)
			cySize -= cyBorder;
	}
	
	// Count the controls.
	int nControls = 0;
	for (HWND hwnd = ::GetTopWindow(m_hWnd); hwnd != NULL;
			hwnd = ::GetWindow(hwnd, GW_HWNDNEXT))
		nControls++;

	CRect rect;

	HDWP hdwp = ::BeginDeferWindowPos(nControls);
	for (hwnd = ::GetTopWindow(m_hWnd); hwnd != NULL;
			hwnd = ::GetWindow(hwnd, GW_HWNDNEXT))
	{
		::GetWindowRect(hwnd, rect);
		ScreenToClient(rect);

		hdwp = ::DeferWindowPos(hdwp, hwnd, NULL,
			rect.left, (cySize - rect.Height()) / 2, 0, 0,
			SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOSIZE);
	}
	::EndDeferWindowPos(hdwp);
}

BEGIN_MESSAGE_MAP(C3dDialogBar, CDialogBar)
    ON_WM_NCHITTEST()
END_MESSAGE_MAP()

BOOL C3dDialogBar::PreTranslateSysKey(MSG* pMsg)
{
	ASSERT(pMsg->message == WM_SYSKEYDOWN);

	if ((GetStyle() & WS_VISIBLE) == 0 || IsChild(GetFocus()))
		return FALSE;

	for (CWnd* pWnd = GetTopWindow(); pWnd != NULL;
		pWnd = pWnd->GetWindow(GW_HWNDNEXT))
	{
		WORD nCode = (WORD)(DWORD)pWnd->SendMessage(WM_GETDLGCODE);

		if ((nCode & DLGC_STATIC) || (nCode & DLGC_BUTTON))
		{
			CString strText;
			pWnd->GetWindowText(strText);
			if (FindAccel(strText, pMsg))
			{
				if (nCode & DLGC_STATIC)
				{
					CWnd* pWndNext = pWnd->GetWindow(GW_HWNDNEXT);
					if (pWndNext != NULL && pWndNext->IsWindowEnabled())
					{
						pWndNext->SetFocus();
						if (pWndNext->SendMessage(WM_GETDLGCODE) & DLGC_HASSETSEL)
							pWndNext->SendMessage(EM_SETSEL, 0, -1);
					}
				}
				else if (pWnd->IsWindowEnabled())
				{
					pWnd->SendMessage(WM_LBUTTONDOWN);      // Click on button.
					pWnd->SendMessage(WM_LBUTTONUP);
				}
				return TRUE;
			}
		}
	}

	return FALSE;
}

LRESULT C3dDialogBar::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// Filter window docking messages
	LRESULT lResult;
	if (m_nDockableID != 0 &&
			DkPreHandleMessage(m_hWnd, message, wParam, lParam, &lResult))
		return lResult;

	return CDialogBar::WindowProc(message, wParam, lParam);
}

//      C3dDialogBar::OnNcHitTest
//      All non-control area is HTCAPTION for dock moving.

UINT C3dDialogBar::OnNcHitTest(CPoint pt)
{
	if (m_nDockableID != 0 && DkWGetDock(m_nDockableID) != dpMDI)
	{
		for (CWnd* pWnd = GetTopWindow(); pWnd != NULL;
			pWnd = pWnd->GetWindow(GW_HWNDNEXT))
		{
			if (pWnd->IsWindowVisible() &&
				pWnd->SendMessage(WM_NCHITTEST, 0,
					MAKELPARAM(pt.x, pt.y)) != HTNOWHERE &&
				((WORD)(DWORD)pWnd->SendMessage(WM_GETDLGCODE) &
					DLGC_STATIC) == 0)
			{
				return HTCLIENT;
			}
		}

		return HTCAPTION;
	}

    return CDialogBar::OnNcHitTest(pt);
}

BOOL FindAccel(LPCTSTR lpstr, MSG *pMsg)
{
	ASSERT(pMsg->message == WM_SYSKEYDOWN);

	UINT nKey = MapVirtualKey(pMsg->wParam, 2);
	if ((int) nKey <= 0)    // Check for Zero or high bit.
		return FALSE;

	CString strAccel = _T('&');
	strAccel += (TCHAR) LOWORD(nKey);
	strAccel.MakeUpper();

	CString strCtl = lpstr;
	strCtl.MakeUpper();
	return (strCtl.Find(strAccel) != -1);
}

///////////////////////////////////////////////////////////////////////////////
//      C3dBitmapButton
//              Derrived class of CBitmapButton for our dialog bars.

C3dBitmapButton::C3dBitmapButton()
{
	m_bToggle = FALSE;
}

// Autoload will load the bitmap resources based on the text of
//  the button
// Using suffices "U", "D", "F" and "X" for up/down/focus/disabled
// and extra suffix to support Chicago.
BOOL C3dBitmapButton::AutoLoad(UINT nID, CWnd* pParent,
	BOOL bToggle /*=FALSE*/, LPCRECT lpRect /*=NULL*/)
{
	// first attach the CBitmapButton to the dialog control
	if (!SubclassDlgItem(nID, pParent))
		return FALSE;

	m_bToggle = bToggle;

	DrawBitmaps(bToggle, lpRect);

	// we need at least the primary
	if (m_bitmap.m_hObject == NULL)
		return FALSE;

	// size to content
	SizeToContent();
	return TRUE;
}

BOOL C3dBitmapButton::DrawBitmaps(BOOL bToggle, LPCRECT lpRect)
{
	// delete old bitmaps (if present)
	m_bitmap.DeleteObject();
	m_bitmapSel.DeleteObject();
	m_bitmapFocus.DeleteObject();
	m_bitmapDisabled.DeleteObject();

	CString strText;
	GetWindowText(strText);
	ASSERT(!strText.IsEmpty());             // must provide a title

	HBITMAP hbm, hbmButton;
	UINT nStyle = TBBS_BUTTON;
	CBitmap* pbmGlyph = NULL;
	LPCTSTR lpszText = NULL;

	if (strText.Find(_T('&')) != -1)        // Text button?
		lpszText = strText;
	else
	{
		LoadBitmaps(strText + _T("U"), strText + _T("D"),
		  NULL, strText + _T("X"));
		
		pbmGlyph = &m_bitmap;
	}

	hbmButton = CreateButtonBitmap(nStyle, lpRect, pbmGlyph, lpszText);
	if (hbmButton == NULL)
	{
		TRACE0("Failed to create bitmap for normal image.\n");
		goto failed;   // need this one image
	}

	nStyle |= (bToggle ? TBBS_CHECKED : TBBS_PRESSED);
	if (m_bitmapSel.m_hObject != NULL)
		pbmGlyph = &m_bitmapSel;
	
	hbm = CreateButtonBitmap(nStyle, lpRect, pbmGlyph, lpszText);
	if (hbm != NULL)
	{
		m_bitmapSel.DeleteObject();
		m_bitmapSel.Attach(hbm);
	}

	m_bitmap.DeleteObject();        // delete the glyph.
	m_bitmap.Attach(hbmButton);

	if (m_bitmapDisabled.m_hObject != NULL)
	{
		pbmGlyph = &m_bitmapDisabled;

		hbm = CreateButtonBitmap(TBBS_BUTTON, lpRect, pbmGlyph);
		if (hbm != NULL)
		{
			m_bitmapDisabled.DeleteObject();
			m_bitmapDisabled.Attach(hbm);
		}
	}

	return TRUE;

failed:
	// delete bitmaps (if present)
	m_bitmap.DeleteObject();
	m_bitmapSel.DeleteObject();
	m_bitmapFocus.DeleteObject();
	m_bitmapDisabled.DeleteObject();

	return FALSE;   
}

HBITMAP C3dBitmapButton::CreateButtonBitmap(UINT nStyle,  LPCRECT lpRect,
		CBitmap* pbmGlyph /*= NULL*/, LPCTSTR lpszText /*= NULL*/)
{
	return ::CreateButtonBitmap(this, nStyle, lpRect, pbmGlyph, lpszText);
}


void C3dBitmapButton::DrawItem(LPDRAWITEMSTRUCT lpDIS)
{
	CBitmapButton::DrawItem(lpDIS);

	if (GetState() & 4)
	{
		UINT nID = lpDIS->CtlID;
		CString strRes, strPrompt;
		if (nID != 0 && (!strRes.LoadString(nID) ||
				!AfxExtractSubString(strPrompt, strRes, 0)))
			TRACE1("No String for ID = 0x%x.\n", nID);

		SetPrompt(strPrompt);
	}
}

BEGIN_MESSAGE_MAP(C3dBitmapButton, CBitmapButton)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONUP()
	ON_WM_SYSCOLORCHANGE()
END_MESSAGE_MAP()

void C3dBitmapButton::OnLButtonDblClk(UINT nFlags, CPoint pt)
{
	// we're getting bogus double click messages for the button class
	// that don't have special meaning (this is because we are subclassing
	// the windows BUTTON class).  We turn these into regular WM_LBUTTONDOWN
	//
	SendMessage(WM_LBUTTONDOWN, (WPARAM)nFlags, (LPARAM)MAKELONG(pt.x, pt.y));
}

void C3dBitmapButton::OnLButtonUp(UINT n1, CPoint pt)
{
	// do default processing for this message...
	CBitmapButton::OnLButtonUp(n1, pt);

	// the above call could destroy this window
	// put focus back in parent at some level
	if (CWnd::FromHandlePermanent(m_hWnd) == this && ::GetFocus() == m_hWnd)
	{
		CFrameWnd* pFrame = GetParentFrame();
		ASSERT(pFrame != NULL);

		HWND hwndFocus = NULL, hwndView = theApp.GetActiveView();
		CWnd* pWndParent = this;
		if (hwndView != NULL)
		{
			do
			{
				pWndParent = pWndParent->GetParent();
				ASSERT(pWndParent != NULL);

				if (::IsChild(pWndParent->m_hWnd, hwndView))
				{
					hwndFocus = hwndView;
					break;
				}
				else if (pWndParent->IsKindOf(RUNTIME_CLASS(CDialog))) // REVIEW: bobz - expensive?
				{
					hwndFocus = pWndParent->m_hWnd;
					break;
				}
			}
			while (pWndParent != pFrame);
		}
		else // hwndView NULL, but in a dialog
		{
			pWndParent = pWndParent->GetParent();
			ASSERT(pWndParent != NULL);

			if (pWndParent->IsKindOf(RUNTIME_CLASS(CDialog)))
			{
				hwndFocus = pWndParent->m_hWnd;
			}

		}

		if (hwndFocus == NULL)
			hwndFocus = pFrame->m_hWnd;
		if (hwndFocus != NULL)
			::SetFocus(hwndFocus);
	}

	SetPrompt();    // Clear the prompt.
}

void C3dBitmapButton::OnSysColorChange()
{
	CRect rect;
	GetWindowRect(rect);
	DrawBitmaps(m_bToggle, rect);

	Invalidate(TRUE);
}

///////////////////////////////////////////////////////////////////////////////
//      C3dFileDialog
//
DWORD MungeFlags( DWORD dwFlags )
{
	if( dwFlags & OFN_ENABLETEMPLATE )
		dwFlags = (dwFlags & ~OFN_ENABLETEMPLATE) | OFN_ENABLETEMPLATEHANDLE;

	return dwFlags;
}

C3dFileDialog::C3dFileDialog(BOOL bOpenFileDialog, LPCSTR lpszDefExt,
    LPCSTR lpszFileName, DWORD dwFlags, LPCSTR lpszFilter, CWnd* pParentWnd,
    UINT nHelpID, UINT nExplorerDlg)
: CFileDialog(bOpenFileDialog, lpszDefExt, lpszFileName, MungeFlags( dwFlags ),
    lpszFilter, FromHandle(_SushiGetSafeOwner(pParentWnd)))
{
	m_dirCurBefore.CreateFromCurrent();
	if( bOpenFileDialog )
		m_ofn.Flags |= OFN_PATHMUSTEXIST;

	if (nHelpID)
		SetHelpID(nHelpID);

	m_iddWinNT = nHelpID;
	m_iddWin95 = nExplorerDlg;      // -1 means no explorer dialog
	m_bMultiSelectOnNT = FALSE;
	m_bSccEnabled = FALSE;

	m_psaFileNames = NULL;
}

C3dFileDialog::~C3dFileDialog()
{
	if (m_dirCurBefore != m_dirCurAfter)
	{
		// If the dialog was closed with Cancel, the current directory
		// was restored to "Before".  Here we make sure that "After"
		// is current.
		m_dirCurAfter.MakeCurrent();

		// Notify everyone that the current directory was changed.
		((CMainFrame*) theApp.m_pMainWnd)->OnDirChange();
	}
}

int C3dFileDialog::DoModal()
{
	if( theApp.m_bWin4 && (m_iddWin95 != (UINT)-1) )
	{
		if ((m_bSccEnabled) && (m_iddWin95==0))
		{
			m_iddWin95 = IDD_CHICAGO_WRKSPCOPEN;
		}

		if( m_iddWin95 != 0x0 )
		{
			m_ofn.lpTemplateName = MAKEINTRESOURCE(m_iddWin95);
			m_ofn.Flags |= OFN_ENABLETEMPLATEHANDLE;
		}
		else
		{
			m_ofn.lpTemplateName = NULL;
			m_ofn.Flags &= ~OFN_ENABLETEMPLATEHANDLE;
		}

		m_ofn.Flags |= OFN_EXPLORER;
		m_bMultiSelectOnNT = FALSE;
	}
	else
	{
		if( m_iddWinNT != 0x0 )
		{
			m_ofn.lpTemplateName = MAKEINTRESOURCE(m_iddWinNT);
			m_ofn.Flags |= OFN_ENABLETEMPLATEHANDLE;
		}
		else
		{
			m_ofn.lpTemplateName = NULL;
			m_ofn.Flags &= ~OFN_ENABLETEMPLATEHANDLE;
		}

		m_ofn.Flags &= ~OFN_EXPLORER;
		if( m_ofn.Flags & OFN_ALLOWMULTISELECT )
		{
			m_bMultiSelectOnNT = TRUE;
			m_ofn.Flags &= ~OFN_ALLOWMULTISELECT;
		}
		else
			m_bMultiSelectOnNT = FALSE;
	}

	m_ofn.Flags &= ~OFN_SHOWHELP;

	C3dDialogTemplate dt;
	PreModalWindow();
	
	// Munge the dialogs font
	if (m_ofn.Flags & OFN_ENABLETEMPLATEHANDLE)
	{
		VERIFY( dt.Load( m_ofn.lpTemplateName ) );
		SetStdFont(dt);
		m_ofn.hInstance = (HINSTANCE) dt.GetTemplate();
	}

	int nRet = CFileDialog::DoModal();

	// Note: We used to try to do our own case sensitive default extension
	// handling for saving, but since COMMDLG strips off any period typed by
	// the user, we ended up adding default extension even when not wanted
	// (Bug 12492). If the user wants a case specific extension then they
	// will just have to type it.  Also doing it here allows saving to an
	// open file, since we check for that case in OnFileNameOK().
	
	PostModalWindow();

	if( (nRet == -1) && (m_ofn.Flags & OFN_EXPLORER)
		&& (CommDlgExtendedError() == 0x2) )            // initialization failure
	{
		ASSERT( m_iddWin95 != (UINT)-1 );
		m_iddWin95 = (UINT)-1;          // explorer dialog doesn't work on this operating system

		return DoModal();                       // try again (recursive call will now use NT dialog)
	}
	else if( m_bMultiSelectOnNT )
	{               // return things to the way we found them
		m_ofn.Flags |= OFN_ALLOWMULTISELECT;
	}

	return nRet;
}

BOOL C3dFileDialog::OnInitDialog()
{
	ModifyStyle(0, DS_CONTEXTHELP);
	ModifyStyleEx(0, WS_EX_CONTEXTHELP);

	CWnd* pScc = GetDlgItem(IDC_SCC);
	if (pScc != NULL)
	{
		// Conditionally provide Source Control button
		LPSOURCECONTROLSTATUS pInterface;
		if (SUCCEEDED(theApp.FindInterface(IID_ISourceControlStatus, (LPVOID FAR*)&pInterface)))
		{
			LPSOURCECONTROL pSccManager;
			VERIFY(SUCCEEDED(theApp.FindInterface(IID_ISourceControl, (LPVOID FAR*)&pSccManager)));
			BOOL bScc = ((m_bSccEnabled) && (pInterface->IsSccInstalled() == S_OK) && (pSccManager->CanBrowse() == S_OK) && (pSccManager->CanSync() == S_OK));
			pInterface->Release();
			pSccManager->Release();
			ASSERT( pScc != NULL );
			if (IsExplorer())
			{
				CWnd * pSep = GetDlgItem(IDC_SCC_SEPARATOR);
				ASSERT( pSep != NULL );
				CWnd * pTxt = GetDlgItem(IDC_SCC_HELPERTEXT);
				ASSERT( pTxt != NULL );

				if (bScc)
				{
					// position button directly under OK button -- right-justified
					CRect rcOK, rcScc, rcSep, rcTxt, rcLB;
					pScc->GetWindowRect(&rcScc);
		
					CWnd * pOK = GetParent()->GetDlgItem(IDOK);
					ASSERT(pOK != NULL);
					pOK->GetWindowRect(&rcOK);

					pSep->GetWindowRect(&rcSep);
					pTxt->GetWindowRect(&rcTxt);

					CWnd * pLB = GetParent()->GetDlgItem(lst1);
					ASSERT(pLB != NULL);
					pLB->GetWindowRect(&rcLB);

					int offset = rcOK.right - rcScc.right;
					rcScc.left += offset;
					rcScc.right += offset;

					rcSep.right = rcOK.right;
					rcSep.left = rcLB.left;

					rcTxt.right = rcScc.left - 8; // leave a slight gap
					rcTxt.left = rcLB.left;

					ScreenToClient(&rcScc);
					pScc->MoveWindow(&rcScc);

					ScreenToClient(&rcSep);
					pSep->MoveWindow(&rcSep);

					ScreenToClient(&rcTxt);
					pTxt->MoveWindow(&rcTxt);
				}

				pSep->EnableWindow(bScc);
				pSep->ShowWindow(bScc ? SW_SHOWNA : SW_HIDE);

				pTxt->EnableWindow(bScc);
				pTxt->ShowWindow(bScc ? SW_SHOWNA : SW_HIDE);
			}
			pScc->EnableWindow(bScc);
			pScc->ShowWindow(bScc ? SW_SHOWNA : SW_HIDE);
		}
		else
		{
			pScc->EnableWindow(FALSE);
			pScc->ShowWindow(SW_HIDE);
		}
	}

	return CFileDialog::OnInitDialog();
}

void C3dFileDialog::OnScc()
{
	// ASSERT(m_bSccEnabled);
	COleRef<ISourceControlStatus> pInterface;
	VERIFY(SUCCEEDED(theApp.FindInterface(IID_ISourceControlStatus, (LPVOID FAR*)&pInterface)));
	ASSERT(pInterface->IsSccInstalled() == S_OK);

	CWnd* pDirName = IsExplorer() ? GetParent()->GetDlgItem(stc1)
		: GetDlgItem(stc1);
	ASSERT( pDirName != NULL );
	CString strDirName;
	CString strProjName;
	CString strProjPath;
	pDirName->GetWindowText(strDirName);
	CWnd* pFileName = IsExplorer() ? GetParent()->GetDlgItem(edt1) : GetDlgItem(edt1);
	ASSERT(pFileName != NULL);
	LPSOURCECONTROL pSccManager;
	VERIFY(SUCCEEDED(theApp.FindInterface(IID_ISourceControl, (LPVOID FAR*)&pSccManager)));
	if (SUCCEEDED(pSccManager->GetProject(strProjName, strDirName, strProjPath, FALSE)))
	{

	//	CString strFileName;
	//	pFileName->GetWindowText(strFileName);

		pFileName->SetRedraw(FALSE);
		// set directory where the dialog should now be pointing
		pFileName->SetWindowText(strDirName);

		// force an update of file list box contents (press OK)
		CWnd *pOK = (IsExplorer() ? GetParent()->GetDlgItem(IDOK) : GetDlgItem(IDOK));
		ASSERT(pOK != NULL);
		GotoDlgCtrl(pOK);
		pOK->SendMessage(WM_LBUTTONDOWN, 0, 0);
		pOK->SendMessage(WM_LBUTTONUP, 0, 0);

		CPath pathProj;
		pathProj.Create(strProjPath);

		// REVIEW [patbr]: we should set the filter to that which contains
		// the extension of the file that was passed back in strProjPath...

		if (IsExplorer())
		{
			// get the listview and select the project name
			CWnd *pListViewParent = GetParent()->GetDlgItem(lst2);
			CListCtrl *pListCtrl = (CListCtrl *)pListViewParent->GetDescendantWindow(0x01);

			LV_FINDINFO lvfi;
			lvfi.flags = LVFI_STRING;
			lvfi.psz = pathProj.GetFileName();
			lvfi.lParam = NULL;
			int nIndex = pListCtrl->FindItem(&lvfi);
			if (nIndex != -1)
			{
				LV_ITEM lvItem;
				lvItem.iItem = nIndex;
				lvItem.iSubItem = 0;
				lvItem.mask = LVIF_STATE;
				lvItem.stateMask = LVIS_SELECTED | LVIS_FOCUSED;
				lvItem.state = LVIS_SELECTED | LVIS_FOCUSED;
				pListCtrl->SetItem(&lvItem);
				pListCtrl->SetFocus();
			}
		}

		// set content of file name edit field to project name
		pFileName->SetWindowText(pathProj.GetFileName());
		pFileName->SetRedraw(TRUE);
	}

	if (pInterface->IsSccInstalled() != S_OK) {
// if there are problems upon pushing the scc button (such as sourcesafe4 installed),
// then subsequent pushes shouldn't work. kind of freaky ui..will probably need to improve.
		CWnd* pScc = GetDlgItem(IDC_SCC);
		ASSERT( pScc != NULL );
		pScc->EnableWindow(FALSE);
		pScc->ShowWindow(SW_SHOWNA);
	
		CWnd * pSep = GetDlgItem(IDC_SCC_SEPARATOR);
		ASSERT( pSep != NULL );
		pSep->EnableWindow(FALSE);
		pSep->ShowWindow(SW_SHOWNA);

		CWnd * pTxt = GetDlgItem(IDC_SCC_HELPERTEXT);
		ASSERT( pTxt != NULL );
		pTxt->EnableWindow(FALSE);
		pTxt->ShowWindow(SW_SHOWNA);
	}
	pSccManager->Release();
}

LPCTSTR C3dFileDialog::GetFilterExtension(int n)
{
	LPCTSTR pch = m_ofn.lpstrFilter;

	if( pch == NULL )
		return NULL;

	for( int i = 0; *pch ; i++ )
	{
		while( *pch++ )         // skip listbox string
			;
		if( i == n )
			return pch;
		while( *pch++ )         // skip extension string
			;
	}

	return NULL;
}

// Call this function to 
void C3dFileDialog::ApplyDefaultExtension(void)
{
	CWnd* pName = GetDlgItem(edt1);
	CString strName;
	pName->GetWindowText(strName);

	char acDrive[_MAX_DRIVE];
	char acDir[_MAX_DIR];
	char acName[_MAX_FNAME];
	char acExt[_MAX_EXT];
	_splitpath(strName,acDrive, acDir, acName, acExt);

	if( (*acExt == '\0') &&
		(m_ofn.lpstrDefExt != NULL) )
	{
		strName = acDrive;
		strName += acDir;
		strName += acName;
		ASSERT( _tcschr(m_ofn.lpstrDefExt, '.') == NULL );
		strName += '.';
		strName += m_ofn.lpstrDefExt;
		pName->SetWindowText(strName);
	}
}

BEGIN_MESSAGE_MAP(C3dFileDialog, CFileDialog)
	ON_EN_KILLFOCUS(edt1, OnNameKillFocus)
	ON_LBN_SELCHANGE(lst1, OnFileListSelChange)
    ON_COMMAND(IDOK, OnRobustOK)
	ON_BN_CLICKED(IDC_SCC, OnScc)
	ON_WM_CONTEXTMENU()
	ON_WM_HELPINFO()
END_MESSAGE_MAP()

void C3dFileDialog::OnContextMenu(CWnd* pWnd, CPoint pt) 
{
	DoHelpContextMenu(this, m_nIDHelp, pt);
}

BOOL C3dFileDialog::OnHelpInfo(HELPINFO* pInfo) 
{
	return DoHelpInfo(this, m_nIDHelp, pInfo);
}

void C3dFileDialog::UpdateType(LPCTSTR lpszName /*=NULL*/)
{
	CComboBox* pTypeCombo = (CComboBox*)(IsExplorer() ? 
		GetParent()->GetDlgItem(cmb1) : GetDlgItem(cmb1));
	int nCurSel = pTypeCombo->GetCurSel();

	CString strExt, strFilter, strTmp;
	
	if (lpszName != NULL)
		strExt = lpszName;
	else
	{
		CEdit* pNameEdit = (CEdit*)(IsExplorer() ? 
			GetParent()->GetDlgItem(edt1) : GetDlgItem(edt1));
		pNameEdit->GetWindowText(strExt);
	}

	BOOL bExpr = (strExt.FindOneOf(_TEXT("*?")) != -1);

	strExt.MakeLower();
	if (strExt.Find(_T(';')) == -1)
		strExt = GetExtension(strExt);

	// Just return if no extension. (Empty or ".")
	if (strExt.GetLength() < 2)
		return;

	if (strExt.Find(_T(';')) == -1)
	{
		strTmp = _TEXT("*");
		strTmp += strExt;
		strExt = strTmp;
	}

	// Bracket the search string with ";" to insure valid find results.
	strTmp = _TEXT(";");
	strTmp += strExt;
	strTmp += _TEXT(";");
	strExt = strTmp;

	LPCTSTR pch = m_ofn.lpstrFilter;
	int nIndex = 0, nSel = LB_ERR;
	int nLen = INT_MAX;

	for (;;)
	{
		// Skip listbox string
		while (*pch++ != _T('\0'))
			;

		if (*pch == _T('\0'))
			break;

		// Bracket the filter string with ";" to insure valid find results.
		strFilter = _TEXT(";");
		strFilter += pch;
		strFilter += _TEXT(";");
		if (strFilter.Find(strExt) != -1)
		{
			if (!bExpr && nIndex == nCurSel)
			{
				// Not expression, and extension in current type selection.
				nSel = nIndex;
				break;
			}

			int nNewLen = _tcslen(pch);
			if (nNewLen < nLen)
			{
				// Select shortest extension list containing this string.
				nLen = nNewLen;
				nSel = nIndex;
			}
		}

		// Skip the extension list
		while (*pch++ != _T('\0'))
			;

		nIndex++;
	}

	if (nSel != LB_ERR)
	{
		pTypeCombo->SetCurSel(nSel);
		m_ofn.nFilterIndex = nSel + 1;
	}
}

void C3dFileDialog::SetOkButtonText(UINT ids)
{
	CString strOK;
	strOK.LoadString(ids);
	if( IsExplorer() )
	{
		ASSERT( GetParent() != NULL );
		GetParent()->SendMessage(CDM_SETCONTROLTEXT, IDOK, (LPARAM)(const char*)strOK);
	}
//      else
//              SetDlgItemText(IDOK, strOK);    // this isn't really NT standard -- we could just not do anything here
}

void C3dFileDialog::OnNameKillFocus()
{
	if (m_ofn.lpstrFilter != NULL)
		UpdateType();
	Default();
}

void C3dFileDialog::OnRobustOK()
{
#if 0
    if (thePackage.CheckForEmergency())
    {
	SendMessage(WM_COMMAND, IDCANCEL);
	return;
    }
#endif
	
    OnOK(); // ok to call "real" OnOK
}

/* OnFileListSelChange
 *      In order for the the multi-selection on NT hack to work, the contents of
 *      the edit control must reflect the selection in the list box, or else
 *      the list box selection is ignored.  Unfortunately, when an item is
 *      deselected, it still shows up in the edit control, which causes the exact
 *      opposite of the desired effect.
 */
void C3dFileDialog::OnFileListSelChange()
{
	Default();              // first let CommDlg do its thing

	if( !m_bMultiSelectOnNT )
		return;

	CListBox* pList = (CListBox*)GetDlgItem(lst1);
	ASSERT( pList != NULL );

	int nCurSel = pList->GetCurSel();

	CString strEdit;
	if( pList->GetSel(nCurSel) > 0 )        // user selected new item
		pList->GetText(nCurSel, strEdit);
	else
	{
		// user DE-selected an item, so don't let CommDlg put that filename
		// in the edit field
		int nCount = pList->GetCount();
		while( --nCount >= 0 )
		{
			if( pList->GetSel(nCount) > 0 )
				break;
		}
		if( nCount < 0 )
			strEdit.Empty();
		else
			pList->GetText(nCount, strEdit);
	}

	SetDlgItemText(edt1, strEdit);
}

void C3dFileDialog::OnOK()
{
	m_dirCurAfter.CreateFromCurrent();

    CFileDialog::OnOK();   // Do this first, or we get saved index problem.
}

// COMMDLG saves the current directory when the dialog starts up.  If the user
// exits with Cancel, that directory is restored.  Since we want the user's
// directory changes to be kept, we save the CWD here, let COMMDLG do its thing,
// then restore the user's choice in the destructor.
// 
void C3dFileDialog::OnCancel()
{
	m_dirCurAfter.CreateFromCurrent();

    CFileDialog::OnCancel(); 
}

#if 0
void C3dFileDialog::OnLBSelChangedNotify (UINT nIDBox, UINT iCurSel, UINT nCode)
{
	if (nIDBox == lst1)
	{
	}
	CFileDialog::OnLBSelChangedNotify (nIDBox, iCurSel, nCode);
}
#endif

// COMMDLG calls back here after OK is pressed but before the dialog
// is closed.  This gives you a chance to reject the user's input.
//
//      In addition to checking the filename(s), we also add a file extension
//      if the user did not specify one and one is called for.
//
//      Since the user may have selected more than one file, we reject the input 
//      if any of the files does not exist.
BOOL C3dFileDialog::OnFileNameOK()
{
	if (!m_bOpenFileDialog)
	{
		// because OnKillFocus may never get called do it here
		if (m_ofn.lpstrFilter != NULL)
			UpdateType();
		return CFileDialog::OnFileNameOK();
	}

	// work-around for win bug [fabriced]
	CButton *pChk = (CButton *)(IsExplorer() ? 
						GetParent()->GetDlgItem(chx1) :	GetDlgItem(chx1));
	if(pChk && pChk->GetCheck())
		m_ofn.Flags |= OFN_READONLY;

	int nLength = 0;
	CString strText = GetFileName();
	nLength = strText.GetLength();

	CDir dir;
	VERIFY(dir.CreateFromCurrent());
	nLength += dir.GetLength() + _MAX_EXT + 3;	// add room for extension, backslash, trailing double NULL

	LPSTR lpstrFileT = m_ofn.lpstrFile;
	int nMaxFileT = m_ofn.nMaxFile;

	LPSTR lpstrFile = (LPSTR)malloc(nLength);
	memset(lpstrFile, 0, nLength);
	m_ofn.lpstrFile = lpstrFile;
	m_ofn.nMaxFile = (DWORD)nLength;

	if (strText.Find("\" \"") >= 0)
	{
		// we have multiple filenames selected or entered by the user
		_tcscpy(m_ofn.lpstrFile, (const TCHAR *)dir);
		int nOffset = dir.GetLength() + 1;
		m_ofn.lpstrFileTitle = m_ofn.lpstrFile + nOffset;
		m_ofn.nFileOffset = (WORD)nOffset;
		int nStart, nEnd;
		while ((nStart = strText.Find("\"")) != -1)
		{
			strText = strText.Right(strText.GetLength() - nStart - 1);
			nEnd = strText.Find("\"");
			CString strName = strText.Left(nEnd);
			_tcscpy(m_ofn.lpstrFile + nOffset, strName);
			strText = strText.Right(strText.GetLength() - nEnd - 1);
			nOffset += nEnd + 1;
		}
	}
	else
	{
		// single filename selected or entered--commdlg has
		// the right path and we don't have to do anything to it
		_tcscpy(m_ofn.lpstrFile, lpstrFileT);
		m_ofn.nMaxFile = nMaxFileT;
	}

	// fill in m_psaFileNames with the multiple filenames
	if( m_psaFileNames != NULL )
	{
		m_psaFileNames->RemoveAll();
		if( !m_bMultiSelectOnNT )
		{
			POSITION pos = GetStartPosition();
			while( pos != NULL )
				m_psaFileNames->Add(GetNextPathName(pos));
		}
		else
			UpdateMultiSelectOnNT();
	}
	else
	{
		ASSERT((m_ofn.Flags & OFN_ALLOWMULTISELECT) == 0);      // we need the StringArray to handle multiselection issues
	}

	// Do better than commdlg 'default extension'
	CString strFileName;
	int nFiles = (m_psaFileNames == NULL) ? 1 : m_psaFileNames->GetSize();
	for( int iFile = 0; iFile < nFiles; iFile++ )
	{
		if(m_psaFileNames == NULL)
			strFileName = GetPathName();
		else
			strFileName = m_psaFileNames->GetAt(iFile);

		CString strFileTemp = strFileName;
		// don't do MakeUpper on the original file name--this mangles MBCS characters
		strFileTemp.MakeUpper();
		int nIndex;
		// check for indications that this is a UNC or URL path name and adjust if necessary
		if ((nIndex = strFileTemp.Find("\\\\\\")) != -1)
		{
			strFileName = strFileName.Right(strFileName.GetLength() - nIndex - 1);
		}
		else if ((nIndex = strFileTemp.Find("FILE:")) != -1)
		{
			strFileName = strFileName.Right(strFileName.GetLength() - nIndex);
		}
		else if ((nIndex = strFileTemp.Find("HTTP:")) != -1)
		{
			strFileName = strFileName.Right(strFileName.GetLength() - nIndex);
		}

		if (FileExists(strFileName))
			continue;

		CString strExt = GetExtension(strFileName);
		if (strExt.IsEmpty())
		{
			CComboBox* pComboBox = (CComboBox*)GetDlgItem(cmb1);
			if( IsExplorer() )
			{
				ASSERT( pComboBox == NULL );
				pComboBox = (CComboBox*)GetParent()->GetDlgItem(cmb1);
			}
			ASSERT( pComboBox != NULL );
			int nSel = pComboBox->GetCurSel();
			if (nSel >= 0)
			{
				LPCSTR lpsz = m_ofn.lpstrFilter;
				while (lpsz[0] != 0 && nSel--)
				{
					lpsz += lstrlen(lpsz)+1;    // skip past display text
					ASSERT(lpsz[0] != 0);
					lpsz += lstrlen(lpsz)+1;    // skip past *.ext info
				}
				if (lpsz[0])
				{
					lpsz += lstrlen(lpsz)+1;    // skip past display text
					ASSERT(lpsz[0] == '*');
					ASSERT(lpsz[1] == '.');
					ASSERT(lpsz[2] != '\0');
					do
					{
						CString strTestName = strFileName;
						_TCHAR ext[_MAX_EXT];
						int i = 0;

						while (*++lpsz != '\0' && *lpsz != ';')
							ext[i++] = *lpsz;
						ext[i] = '\0';

						strTestName += ext;

						// Check for existence.
						if (FileExists(strTestName))
						{
							// Update the file name
							// Note that FOR::CE() assumes no dot.
							if( m_psaFileNames == NULL )
								strncpy(lpstrFileT, strTestName, m_ofn.nMaxFile);
							else
								m_psaFileNames->SetAt(iFile, strTestName);
							goto NextFile;
						}

					} while (*lpsz++ != '\0');
				}
			}
		}

		// "File does not exist"
		ErrorBox(ERR_File_NoExist, m_ofn.lpstrFile);
		free(lpstrFile);
		m_ofn.lpstrFile = lpstrFileT;
		m_ofn.nMaxFile = nMaxFileT;
		return TRUE;    // don't allow dialog to close

	NextFile: ;
	}

	free(lpstrFile);
	m_ofn.lpstrFile = lpstrFileT;
	m_ofn.nMaxFile = nMaxFileT;
	return FALSE;
}

void C3dFileDialog::UpdateMultiSelectOnNT(void)
{
	ASSERT( m_psaFileNames != NULL );
	ASSERT( m_bMultiSelectOnNT );
	
	// Because NT is a pain in the butt about long file names and 
	// multiple selection circumvent the CommDlg behavior, and do it 
	// ourselves by querying the listbox.  Note that the CommDlg 
	// behavior of only having the first file show up in the edit 
	// control will remain.  We take advantage of that to get a full path.
	POSITION pos = GetStartPosition();
	CString strFile = GetNextPathName(pos);
	char acDrive[_MAX_DRIVE];
	char acPath[_MAX_DIR];
	_splitpath(strFile, acDrive, acPath, NULL, NULL);
	CString strPath = acDrive;
	strPath += acPath;

	CString strEdit;
	GetDlgItemText(edt1, strEdit);
	BOOL bEditInList = strEdit.IsEmpty();   // if empty, then we're fine
	
	CListBox* pList = (CListBox*)GetDlgItem(lst1);
	ASSERT( pList != NULL );
	int nCount = pList->GetCount();
	while( --nCount >= 0 )
	{
		if( pList->GetSel(nCount) > 0 )
		{
			pList->GetText(nCount, strFile);
			if( strFile.CompareNoCase(strEdit) == 0 )
				bEditInList = TRUE;
			m_psaFileNames->Add(strPath + strFile);
		}
	}

	// if the filename in the edit field doesn't match any of the
	// ones selected in the listbox, then the edit field overrules.
	if( !bEditInList )
	{
		m_psaFileNames->RemoveAll();
		POSITION pos = GetStartPosition();
		while( pos != NULL )
			m_psaFileNames->Add(GetNextPathName(pos));
	}
}

BOOL C3dFileDialog::OnCommand(WPARAM wParam, LPARAM lParam)
{
	HWND hWndCtrl = (HWND)lParam;

	if (hWndCtrl != NULL)
	{
		// control notification
		if (!::IsWindow(hWndCtrl)) // work around COMDLG32 bug
			return FALSE;
	}

	return CFileDialog::OnCommand(wParam, lParam);
}

POSITION C3dFileDialog::GetFullFileStartPosition()
{
	return NULL;
}

CString C3dFileDialog::GetNextFullFileName(POSITION pos)
{
	static CString strPath;
	return strPath;
}

//      AppendFilterSuffix
//              Utility function for building file dialog filters.

void AppendFilterSuffix(CString& filter, OPENFILENAME& ofn,
	const CString& strFilterName, BOOL bSetDefExt)
{
	if (!strFilterName.IsEmpty())
	{
		// add to filter
		filter += strFilterName;
		ASSERT(!filter.IsEmpty());  // must have a file type name
		filter += (TCHAR)_T('\0');       // next string please

		// the filter name should be of the form "<Files string> (<Filter>)"
		// e.g. - "Source Files (*.c;*.cpp;*.cxx)"

		int nOpenParen = strFilterName.Find('(');
		int nCloseParen = strFilterName.Find(')');
		if( nOpenParen == -1 || nCloseParen == -1 )
			filter += _T("*.*");    // Bogus filter name.
		else
		{
			CString str = strFilterName.Mid(nOpenParen + 1,
				nCloseParen - nOpenParen - 1);

			// prepend * to each extension, if it isn't already there
			CString strStar;
			for( int n = 0; n < str.GetLength(); n++ )
			{
				if( str[n] == '.' )
					if( (n == 0) || (str[n - 1] != '*') )
						strStar += '*';
				strStar += str[n];
			}

			filter += strStar;
			if( bSetDefExt )
			{
				int nSemi = strStar.Find(';');
				if( nSemi >= 0 )
					strStar = strStar.Left(nSemi);
				static char acExt[_MAX_EXT];
				_splitpath(strStar, NULL, NULL, NULL, acExt);
				if( *acExt == '\0' || _tcschr(acExt, '*') != NULL || _tcschr(acExt, '?') != NULL )
					ofn.lpstrDefExt = NULL;
				else
				{
					ASSERT( *acExt == '.' );
					ofn.lpstrDefExt = acExt + 1;
				}
			}
		}

		filter += (TCHAR)_T('\0');       // next string please
		ofn.nMaxCustFilter++;
	}
}

void AppendFilterSuffix(CString& filter, OPENFILENAME& ofn,
	UINT idFilter, BOOL bSetDefExt /*= FALSE*/)
{
	CString strFilter;
	VERIFY(strFilter.LoadString(idFilter));

	AppendFilterSuffix(filter, ofn, strFilter, bSetDefExt);
}

/////////////////////////////////////////////////////////////////////////////

//Slightly friendlier names for the common dialog items.
#define DIRLIST         lst2
#define EDITFILE        edt1
#define DIRPATH         stc1
#define DRIVECOMBO      cmb2

CDirChooser* CDirChooser::pCurrentChooser = NULL;

extern HWND PASCAL _SushiGetSafeOwner(CWnd* pParent);

CDirChooser::CDirChooser(LPCTSTR lpszOpenDir, LPCTSTR lpszTitle, LPCTSTR lpszPrompt, UINT nDlgID)
{
	if (nDlgID == 0)
		nDlgID = IDD_OPENDIR;

	m_dwHelpContext = IDD_DIR_PICKER;

	if (lpszTitle == NULL || *lpszTitle == _T('\0'))
		m_strTitle.LoadString(IDS_DEFAULTCHOOSERTITLE);
	else
		m_strTitle = lpszTitle;

	if (lpszPrompt == NULL || *lpszPrompt == _T('\0'))
		m_strPrompt.LoadString(IDS_DEFAULTCHOOSERPROMPT);
	else
		m_strPrompt = lpszPrompt;

	*m_szFileName = '\0';

	if (lpszOpenDir != NULL)
	{
		ASSERT(*lpszOpenDir != _T('\0'));
		::SetCurrentDirectory(lpszOpenDir);
		m_strCurDir = lpszOpenDir;
	}
	else
	{
		::GetCurrentDirectory(_MAX_PATH, m_strCurDir.GetBuffer(_MAX_PATH));
		m_strCurDir.ReleaseBuffer();
	}

	m_ofn.lStructSize               = sizeof(OPENFILENAME);
	m_ofn.hwndOwner                 = _SushiGetSafeOwner(NULL);
	m_ofn.hInstance                 = GetResourceHandle();
	m_ofn.lpstrFilter               = NULL;
	m_ofn.lpstrCustomFilter = NULL;
	m_ofn.nMaxCustFilter    = 0L;
	m_ofn.nFilterIndex              = 0;
	m_ofn.lpstrFile                 = m_szFileName;
	m_ofn.nMaxFile                  = sizeof(m_szFileName);
	m_ofn.lpstrFileTitle    = NULL;
	m_ofn.nMaxFileTitle             = 0;
	m_ofn.lpstrInitialDir   = NULL;
	m_ofn.lpstrTitle                = NULL;
	m_ofn.Flags                             = OFN_ENABLEHOOK | OFN_ENABLETEMPLATEHANDLE;
	m_ofn.nFileOffset               = 0;
	m_ofn.nFileExtension    = 0;
	m_ofn.lpstrDefExt               = NULL;
	m_ofn.lCustData                 = 0;
	m_ofn.lpfnHook                  = DirOpenHookProc;
	m_ofn.lpTemplateName    = MAKEINTRESOURCE(nDlgID);

	m_bMustExist = TRUE;
}

int CDirChooser::DoModal()
{
	// Only one chooser active at a time.
	ASSERT(pCurrentChooser == NULL);
	if (pCurrentChooser != NULL)
		return 0;

	{
		int fRet;

		pCurrentChooser = this;

		PreModalWindow();
		// allow OLE servers to disable themselves, usually called in CDialog::PreModal
		AfxGetApp()->EnableModeless(FALSE);
		
		C3dDialogTemplate dt;

		VERIFY( dt.Load( m_ofn.lpTemplateName ) );
		SetStdFont(dt);
		m_ofn.hInstance = (HINSTANCE) dt.GetTemplate();

		fRet = GetOpenFileName(&m_ofn);

	#ifdef DEBUGGING
		if (fRet == 0)
		{
			CommDlgExtendedError();
		}
	#endif

		AfxGetApp()->EnableModeless(TRUE);
		// Reset the dockmgr state since the dialog is now gone
		PostModalWindow();

		// Tell sushi the directory has changed, so title bars can be updated
		((CMainFrame*) theApp.m_pMainWnd)->OnDirChange();

		pCurrentChooser = NULL;

		return fRet;
	}
}

void CDirChooser::SetMustExist(BOOL bMustExist)
{
	m_bMustExist = bMustExist;
}

CString CDirChooser::GetPathName() const
{
	return m_strCurDir;
}

// Default semantics for dialog dismissal
// Can display text in hStatus
BOOL CDirChooser::fDismissOpenDir(const LPCSTR szNew, HWND)
{
	BOOL fReturn;
	fReturn = (m_strCurDir.Collate( szNew) == 0);   // Can close if selected twice
	m_strCurDir = szNew;
	return fReturn;
}

// FUTURE: rewrite all of this to use message maps instead of this hook stuff.
#define WM_DIRCHOOSERDIRCHNG (WM_USER + 1941)

// Hook for commdlg Open dialog box.
UINT APIENTRY CDirChooser::DirOpenHookProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static DWORD     dwMsgSelCh;
	char            szEditPath[_MAX_PATH];

	switch (msg)
	{
		// 26 Jun 96 - Set the context help for lst2 and cmb2
		// WM_HELP and WM_CONTEXTMENU handle the context help for the lst2 and cmb2 dialog items.
		case WM_HELP:
			{
				// F10 and Context help.

				// Handle the context help ourselves.
				// Create a CWnd for the dialog box.
				CWnd DlgWnd;
				DlgWnd.Attach(hDlg);

				// Call the context help helper functions in help.cpp.
				BOOL bResult = ::DoHelpInfo(&DlgWnd, pCurrentChooser->m_dwHelpContext, reinterpret_cast<HELPINFO*>(lParam));                              

				// Detach the window and return .
				DlgWnd.Detach() ;
				return bResult ;
			}
			break ;
		case WM_CONTEXTMENU:
			{
				// Right mouse button help.

				// Handle the context help ourselves.
				// Create a CWnd for the dialog box.
				CWnd DlgWnd;
				DlgWnd.Attach(hDlg) ;

				// Get Mouse coordinates
				CPoint pt(LOWORD(lParam),HIWORD(lParam)); 

				// Call the context help helper functions in help.cpp.
				::DoHelpContextMenu(&DlgWnd, pCurrentChooser->m_dwHelpContext, pt);
					
				// Detach the window and return .
				DlgWnd.Detach() ;
				return 1;
			}
			break ;
		case WM_INITDIALOG:
			dwMsgSelCh = ::RegisterMessage(LBSELCHSTRING);
			
			// Set the edit box prompt.             
			::SetWindowText(::GetDlgItem(hDlg, IDC_DIRPROMPT), pCurrentChooser->m_strPrompt);

			// Set the directory string from the current directory
			::SetWindowText(::GetDlgItem(hDlg, IDC_DIRNAME), pCurrentChooser->m_strCurDir);

			// Set the window title, the m_ofn entry for this doesn't seem to work
			::SetWindowText(hDlg, pCurrentChooser->m_strTitle);

			// During restart this dialog may be brought up, and NOT in the foreground.
			// Fix for: Dolphin #3390
			SetForegroundWindow( hDlg );

			// 26 Jun 96 - Set the context help for lst2 and cmb2
			::SetWindowContextHelpId(::GetDlgItem(hDlg, DIRLIST), HIDC_OPENDIR_DIRECTORIES) ;
			::SetWindowContextHelpId(::GetDlgItem(hDlg, DRIVECOMBO), HIDC_OPENDIR_DRIVES) ;

			if (theApp.m_bWin95 && pCurrentChooser->m_strCurDir.Find("\\\\") == 0)
			{
				CWaitCursor wc;

				CString strReset = pCurrentChooser->m_strCurDir;
				// on Win95, the tree will not be initialized for a UNC directory. we
				// force a couple of directory changes to make it initialize (gross!).
				HWND hwndEdit = ::GetDlgItem(hDlg, IDC_DIRNAME);

				::SetFocus(hwndEdit);
				// we want to change to a valid directory and back to force the update.
				if (GetDriveType("C:\\") == DRIVE_FIXED)
				{
					// most machines have C: as a fixed disk, so use that.
					::SetWindowText(hwndEdit, "C:\\");
				}
				else
				{
					// machines without C: as a fixed disk have A: fixed (NEC machines).
					::SetWindowText(hwndEdit, "A:\\");
				}
				pCurrentChooser->m_strCurDir.Empty();
				::SendMessage(hDlg, WM_COMMAND, MAKEWPARAM(IDOK, IDC_DIRNAME), (LPARAM)hwndEdit);

				::SetFocus(hwndEdit);
				::SetWindowText(hwndEdit, strReset);
				pCurrentChooser->m_strCurDir.Empty();
				::SendMessage(hDlg, WM_COMMAND, MAKEWPARAM(IDOK, IDC_DIRNAME), (LPARAM)hwndEdit);
			}

			return TRUE;

		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
				case ID_HELP:
					theApp.HelpOnApplication(pCurrentChooser->m_dwHelpContext);
					break;
				case IDOK:
				{
					HWND hwndFocus = ::GetFocus();
					HWND hwndEdit = ::GetDlgItem(hDlg, IDC_DIRNAME);
					HWND hwndOK = ::GetDlgItem(hDlg, IDOK);
					HWND hwndList = ::GetDlgItem(hDlg, DIRLIST);

					if (hwndFocus == hwndList)
					{
						if (theApp.m_bWin95)
						{
							// on Win95, directory changes from a UNC root are not
							// reflected in the directory editbox unless we do this.
							::PostMessage(hDlg, WM_DIRCHOOSERDIRCHNG, 0, 0);
						}
						return 0;       // call the default handler
					}

					if( (hwndFocus != hwndEdit) && (hwndFocus != hwndOK) )
						return 0;       // call the default handler

					CWaitCursor wc;

					::GetCurrentDirectory(sizeof(szEditPath), szEditPath);
					CString strCurDir = szEditPath;

					// Find the path entered by the user
					::GetWindowText(hwndEdit, szEditPath, sizeof(szEditPath));

					// set the current directory to whatever the user typed
					// if they typed a non-existent directory, back up until we 
					// find one that exists
					CString strEdit = szEditPath;
					BOOL bDirExists = TRUE;
					int nBack = -1;
					while( !strEdit.IsEmpty() && !::SetCurrentDirectory(strEdit) )
					{
						bDirExists = FALSE;
						nBack = strEdit.ReverseFind('\\');
						if( nBack < 0 )
							break;
						strEdit = strEdit.Left(nBack);
						if( (nBack == 2) && isalpha(strEdit[0]) && (strEdit[1] == ':') )
						{
							strEdit += '\\';        // change "c:" to "c:\"
							nBack = 3;
							if( !::SetCurrentDirectory(strEdit) )
								nBack = 0;
							break;          // if we don't break here, we're libel to get into an endless loop
						}
					}
					if( bDirExists )
					{
						// directory does exist, and is now the current one
						::GetCurrentDirectory(sizeof(szEditPath), szEditPath);
						strEdit = szEditPath;
					}
					else if( nBack > 0 ) 
					{
						// directory doesn't exist, but a parent directory does
						// generate full name based off of confirmed parent directory
						strEdit = szEditPath + nBack;
						::GetCurrentDirectory(sizeof(szEditPath), szEditPath);
						strEdit = szEditPath + strEdit;

						// The attempt to find a partial directory has changed
						// the current directory.  Change it back.
						::SetCurrentDirectory(strCurDir);
					}
					else
					{
						// directory doesn't exist at all, so just complain about it
						strEdit = szEditPath;
					}

					// The virtual fDismissOpenDir function is called here to check if we can
					// dismiss from this directory.
					if (pCurrentChooser->fDismissOpenDir(strEdit, ::GetDlgItem (hDlg, IDC_OPENDIR_STATUS)))
					{
						::PostMessage(hDlg, WM_COMMAND, IDABORT, IDOK);
					}
					else
					{
						::SetWindowText(::GetDlgItem(hDlg, EDITFILE), strEdit);
						::SetWindowText(::GetDlgItem(hDlg, IDC_DIRNAME), strEdit);
						::PostMessage(::GetDlgItem(hDlg, IDC_DIRNAME), EM_SETSEL, 0,-1);

						if( !bDirExists )
						{
							if( pCurrentChooser->m_bMustExist )
							{
								CString str;
								::MsgBox(Error, ::MsgText(str, ERR_Change_Directory, strEdit));
								pCurrentChooser->m_strCurDir = "";
							}

							return 1; // Don't call the default handler.
						}
					}
					break;
				}
				case DIRLIST:
					if (HIWORD(wParam) == LBN_DBLCLK)
					{
						if (theApp.m_bWin95)
						{
							// on Win95, directory changes from a UNC root are not
							// reflected in the directory editbox unless we do this.
							::PostMessage(hDlg, WM_DIRCHOOSERDIRCHNG, 0, 0);
						}
						return 0;       // call the default handler
					}
					break;
			}
			break;

		case WM_COMMANDHELP:
			theApp.HelpOnApplication(pCurrentChooser->m_dwHelpContext);
			::SetWindowLong(hDlg, DWL_MSGRESULT, TRUE);
			return TRUE;

		case WM_DIRCHOOSERDIRCHNG:
		{
		//	TRACE0("CDirChooserDlg received WM_DIRCHOOSERDIRCHNG message.\n");
			CString strActualDir;
			strActualDir.Empty();
			int nIndex = ::SendMessage(::GetDlgItem(hDlg, DIRLIST), LB_GETCURSEL, 0, 0);
			for (int i = 0; i <= nIndex; i++)
			{
				TCHAR szDirName[_MAX_PATH];
				::SendMessage(::GetDlgItem(hDlg, DIRLIST), LB_GETTEXT, i, (LPARAM)(LPCTSTR)szDirName);
				if (!strActualDir.IsEmpty())
					strActualDir += "\\";
				strActualDir += szDirName;
			}

			if (strActualDir.Find("\\\\") == 0)
			{
				// changing the editbox is only necessary if the path is UNC.
				::SetWindowText(::GetDlgItem(hDlg, IDC_DIRNAME), strActualDir);
				::SendMessage(::GetDlgItem(hDlg, IDC_DIRNAME), EM_SETSEL, 0,-1);

				if (pCurrentChooser->fDismissOpenDir(strActualDir, ::GetDlgItem(hDlg, IDC_OPENDIR_STATUS)))
					::PostMessage(hDlg, WM_COMMAND, IDABORT, IDOK);
			}
			return TRUE;
		}

		default:
			if ( (msg = dwMsgSelCh) && 
				 ( (LOWORD(wParam) == DIRLIST) || (LOWORD(wParam) == DRIVECOMBO) ) && 
				 (HIWORD(lParam) == CD_LBSELCHANGE) )
			// Check that selection change is from Drive or Directory list to avoid
			// Calls to the fDismissOpenDir due to the (hidden) file list box.
			{
				// LBSELCH message  to the Directory list or drive combo
				::GetCurrentDirectory(sizeof(szEditPath), szEditPath);
				::SetWindowText(::GetDlgItem(hDlg, IDC_DIRNAME), szEditPath);
				::SetWindowText(::GetDlgItem(hDlg, EDITFILE), szEditPath);
				::SendMessage(::GetDlgItem(hDlg, IDC_DIRNAME), EM_SETSEL, 0,-1);
				
				// There is a bogusness in CommDlg, that when a user hits 
				// enter twice in a row, the second CD_LBSELCHANGE message 
				// claims to come from the DRIVECOMBO (which is also the
				// everywhere-else bin).  This works around that.
				BOOL bDirList = (LOWORD(wParam) == DIRLIST) ||
					(::GetFocus() == ::GetDlgItem(hDlg, DIRLIST));

				// The virtual fDismissOpenDir function is called here to check if we can dismiss
				// only call if the change came from the directory list
				if ( bDirList && pCurrentChooser->fDismissOpenDir(szEditPath, ::GetDlgItem (hDlg, IDC_OPENDIR_STATUS) ))
				{
					::PostMessage(hDlg, WM_COMMAND, IDABORT, IDOK);
				}
			}
			break;
	}

	return 0;
}


/////////////////////////////////////////////////////////////////////////////

CFileFindDlg::CFileFindDlg(LPCTSTR lpszFileToFind, LPCTSTR lpszOpenDir,
		LPCTSTR lpszTitle, LPCTSTR lpszPrompt)
	: CDirChooser(lpszOpenDir, lpszTitle, lpszPrompt)
{
	m_strFileToFind = lpszFileToFind;
	m_dwHelpContext = IDD_DIR_FINDFILE;
	m_pfnValidator = NULL;
}

void CFileFindDlg::SetValidator(LPFNFILEFINDVALIDATOR pfnValidator, LONG lparam) {
	
	m_pfnValidator = pfnValidator;
	m_lValidatorParam = lparam;
}

// fDismissOpenDir returns TRUE if the dialog can be dismissed:
//              szNew is the path when OK is hit or the directory list box selection
//              is chosen.
// Sometimes posts a message in the hStatus.
// This function *must* update m_strCurDir to be a copy of szNew.
BOOL CFileFindDlg::fDismissOpenDir(const LPCSTR szNew, HWND hStatus)
{
// There's been some amount of disagreement over the best UI here.
// Some people think we should check each directory we land in, to see if the
// file we want is there, and then return immediately.
// Others think that we should act like CommDlg, and if the user changes
// directories, they may just be navigating somewhere, so make them enter twice.
// Removing the #if 0 code here would do the latter.
#if 0
	// the user must always select a directory twice, to say they mean it.
	if( !CDirChooser::fDismissOpenDir(szNew) )
		return FALSE;

	// only then do me verify that the file is really there.
#endif

	CDir    dir;
	CPath   path;
	BOOL	 fFound;

	m_strCurDir = szNew;
	
	if ((fFound = dir.CreateFromString(szNew) &&
		path.CreateFromDirAndFilename(dir, m_strFileToFind) &&
		path.ExistsOnDisk())) {

		m_strStatusMessage.Empty();

	} else {

		// Not found, so we let the user know.
		m_strStatusMessage.LoadString (IDS_OPENDIR_STATUS_NOFILE);
	}

	if (fFound && m_pfnValidator) {
		fFound = (*m_pfnValidator) (szNew, m_strFileToFind, m_lValidatorParam);
		if (!fFound) {
			m_strStatusMessage.LoadString (IDS_OPENDIR_STATUS_WRONGFILE);
		}
	}
	
	if (!::SetWindowText (hStatus, m_strStatusMessage)) {
		DWORD dw = GetLastError();
	}
	return fFound;
}



///////////////////////////////////////////////////////////////////////////////
//      Modal window utility functions

BOOL fProjectWindowLocked = FALSE;
LPPROJECTWORKSPACEWINDOW pProject = NULL;
int nLevels = 0;

void PreModalWindow()
{
	// Destroy the splash screen before showing modal window.
	ShowSplashScreen(FALSE);

    // Hide sprites so they paint correctly when dialog goes away.
    CSlobWnd* pWnd = GetActiveSlobWnd();
    if (pWnd != NULL)
	pWnd->ObscureSprites(TRUE);

	CDockWorker::LockWorker();

	if (nLevels == 0)
	{
		theApp.FindInterface(IID_IProjectWorkspaceWindow, (LPVOID FAR *)&pProject);

		// DevStudio 96 BUG #5645
		// The output window change close while the printing dialog is up
		// when we switch from Debug to non-Debug causing an access violation.
		// Lock the dock state so the windows don't change.
		CMainFrame* pFrame = (CMainFrame*)theApp.m_pMainWnd ;
		ASSERT(pFrame != NULL && pFrame->IsKindOf(RUNTIME_CLASS(CMainFrame)));
		pFrame->LockDockState(TRUE);
	}


	nLevels++;

	if (pProject != NULL)
	{
		if (pProject->IsWindowLocked() == S_OK)
		{
			pProject->PreModalWindow();
			fProjectWindowLocked = TRUE;
		}
	}
}

void PostModalWindow()
{
	// DevStudio 96 BUG #5645
	// The output window change close while the printing dialog is up
	// when we switch from Debug to non-Debug causing an access violation.
	// Lock the dock state so the windows don't change.
	if (nLevels == 1) 
	{
		CMainFrame* pFrame = (CMainFrame*)theApp.m_pMainWnd ;
		ASSERT(pFrame != NULL && pFrame->IsKindOf(RUNTIME_CLASS(CMainFrame)));
		pFrame->LockDockState(FALSE);
	}

	// If we came from a floating palette, then we need to restore focus.
	CDockWorker* pDocker = CDockWorker::s_pActiveDocker;
	if (pDocker != NULL && pDocker->GetDock() == dpNil)
		pDocker->m_pWnd->SetFocus();

	nLevels--;

	if (pProject != NULL)
	{
		if (fProjectWindowLocked == TRUE)
		{
			pProject->PostModalWindow();
			fProjectWindowLocked = FALSE;
		}

		if (nLevels == 0)
		{
			pProject->Release();
			pProject = NULL;
		}
	}
}
