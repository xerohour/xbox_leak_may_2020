#include "stdafx.h"

#include "util.h"
#include "imeutil.h"
#include "resource.h"
#include "ipcslob.h"

#include <prjapi.h>
#include <prjguid.h>

#include "ipcits.h"		// for IInternalTrackSelection

extern "C"
{
#include "string.h"
}

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

extern void PASCAL AfxCancelModes(HWND hWndRcvr);

extern UINT WM_SETAPPVALIDATE;
extern UINT WM_ISERROR;

/////////////////////////////////////////////////////////////////////////////
//	CSheetSlob

IMPLEMENT_DYNAMIC(CSheetSlob, CSlob)

CSheetSlob::CSheetSlob() : CSlob()
{
	m_pSheetWnd = NULL;

	theApp.m_theAppSlob.AddDependant(this);
}

CSheetSlob::~CSheetSlob()
{
	theApp.m_theAppSlob.RemoveDependant(this);
	g_PropSheet = NULL;

	NukeInforms();
}

void CSheetSlob::OnInform(CSlob* pChangedSlob, UINT idChange, DWORD dwHint)
{
	if (idChange == SN_DESTROY)
	{
		CSlob::OnInform(pChangedSlob, idChange, dwHint);
		return;
	}

	CInformRecord* pIR = new CInformRecord(pChangedSlob, idChange);
	m_informList.AddTail(pIR);
}

void CSheetSlob::FlushInforms()
{
	ASSERT(m_pSheetWnd != NULL);

	while (!m_informList.IsEmpty())
	{
		CInformRecord* pIR = (CInformRecord*) m_informList.RemoveHead();

		if (m_pSheetWnd->m_pCurPage != NULL &&
			m_pSheetWnd->m_pCurPage->IsKindOf(RUNTIME_CLASS(CSlobPage)))
		{
			((CSlobPage*) m_pSheetWnd->m_pCurPage)->
				OnPropChange(pIR->m_idChange);
		}

		CSlob::OnInform(pIR->m_pChangedSlob, pIR->m_idChange, 0);
		delete pIR;
	}
}

void CSheetSlob::NukeInforms()
{
	ASSERT(m_pSheetWnd != NULL);

	while (!m_informList.IsEmpty())
		delete (CInformRecord*) m_informList.RemoveHead();
}

CInformRecord::CInformRecord(CSlob* pSlob, UINT idChange)
{
	ASSERT(idChange != SN_DESTROY);

	m_pChangedSlob = pSlob;
	m_idChange = idChange;
}

static UINT BASED_CODE propPageButtons [] = 
{
	ID_PROP_PUSHPIN,
	ID_HELP,
};

///////////////////////////////////////////////////////////////////////////////
//	CPropFrameToolBar

CPropFrameToolBar::CPropFrameToolBar()
	: CToolBar()
{
}

void CPropFrameToolBar::SetSizes()
{
	CToolBar::SetSizes(CSize(sizeButtonX, sizeButtonY),
	                   CSize(sizeImageX, sizeImageY));

	m_cxDefaultGap = 0;
	m_cyTopBorder = m_cyBottomBorder = 0;
	m_cxLeftBorder = 0;
	m_dwStyle = m_dwStyle & ~0x0F00;

	UINT nID, nStyle;
	int iImage;

	GetButtonInfo(0, nID, nStyle, iImage);
	SetButtonInfo(0, nID, TBBS_CHECKBOX, iImage);
}

void CPropFrameToolBar::SetPushPin(BOOL bPushed)
{
	if (bPushed == IsPinned())
		return;

	UINT nID, style;
	int iImage;
	GetButtonInfo(0, nID, style, iImage);

	if (bPushed)
	{
		theApp.m_theAppSlob.SetIntProp(P_PoppingProperties, FALSE);
		LoadBitmap(IDB_PPBAR_PINNED);
		style |= TBBS_CHECKED;
	}
	else
	{
		theApp.m_theAppSlob.SetIntProp(P_PoppingProperties, TRUE);
		LoadBitmap(IDB_PPBAR_UNPINNED);
		style &= ~TBBS_CHECKED;
	}

	ASSERT(bPushed == IsPinned());
	SetButtonInfo(0, nID, style, iImage);
	Invalidate(FALSE);
}

BOOL CPropFrameToolBar::IsPinned()
{
	BOOL bPopProp;
	theApp.m_theAppSlob.GetIntProp(P_PoppingProperties, bPopProp);

	return !bPopProp;
}

#ifdef _DEBUG
void CPropFrameToolBar::AssertValid() const
{
	// deliberate skipping of base class, because we changed the assumptions... 
	CWnd::AssertValid();

	ASSERT(m_nCount == 0 || m_pData != NULL);
}
#endif

/////////////////////////////////////////////////////////////////////////////
//	CSheetWnd

CObList CSheetWnd::s_listLoadedPages;
int CSheetWnd::s_cyBreakLine;

// CSheetWnd message map

BEGIN_MESSAGE_MAP(CSheetWnd, CMiniFrameWnd)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_WM_SYSCOMMAND()
	ON_WM_ACTIVATE()
	ON_WM_SIZE()
	ON_WM_ENABLE()
	ON_WM_NCLBUTTONDOWN()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_TIMER()
	ON_WM_MOUSEACTIVATE()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()

	ON_WM_QUERYNEWPALETTE()
	ON_WM_PALETTECHANGED()
		
	ON_COMMAND(IDMX_OKPAGE, CmdOkPage)
	ON_COMMAND(IDMX_CANCELPAGE, CmdCancelPage)
	ON_COMMAND(IDMX_NEXTPAGE, CmdNextPage)
	ON_COMMAND(IDMX_PREVPAGE, CmdPrevPage)

	ON_COMMAND(ID_PROP_PUSHPIN, OnCmdPushPin)
	ON_COMMAND(ID_HELP, OnCmdHelp)

	ON_CBN_SELCHANGE(IDC_COMBOBOX_LEVEL, OnSelChangeLevel)

	ON_MESSAGE(WM_COMMANDHELP, OnCommandHelp)
	ON_MESSAGE(WM_SETMESSAGESTRING, OnSetMessageString)
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTA, 0x0000, 0xffff, OnToolTipText)
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTW, 0x0000, 0xffff, OnToolTipText)
	ON_MESSAGE(WM_USER_VALIDATEREQ, OnValidateReq)
	ON_REGISTERED_MESSAGE(WM_ISERROR, OnIsError)
END_MESSAGE_MAP()

CSheetWnd::CSheetWnd() : CMiniFrameWnd(),
	m_noPage(), m_nullPage(), m_multiPage(),
	m_tabRow(), m_strDefaultPage(),
	m_sheetSlob(), m_toolBar()
{
	m_pCurPage = &m_noPage;
	m_iFocusSave = 0;
	m_bSheetChanged = FALSE;
	m_bValidating = FALSE;
	m_bValidateEnabled = TRUE;
	m_bInCancel = FALSE;
	m_pCurSlob = NULL;
	m_pNewSlob = NULL;
	m_selState = blank;
	m_newSelState = blank;
	m_nCurPage = 0;
	m_nNewPage = -1;

	m_sheetSlob.m_pSheetWnd = this;
	m_tabRow.SetParent(this);

	m_refInhibitExtraPages = 0;
	m_nFirstPageSet = 0;
	
	m_cyRowHeight = INITIAL_LEVELCOMBO_HEIGHT;
}

CSheetWnd::~CSheetWnd()
{
	// free up the OLE property page cache
	COlePage::Cleanup();
}

//	CSheetWnd::Create

BOOL CSheetWnd::Create()
{
	CPoint pos(xDefaultPos, yDefaultPos);
	CSize size(cxSize, cySize);
	pos.y -= 15;				// ACK!

	VERIFY(LoadAccelTable(MAKEINTRESOURCE(IDR_ACCEL_PROP)));

	DWORD dwStyle = WS_CAPTION | WS_SYSMENU | WS_POPUP | WS_DISABLED;

	if (!CMiniFrameWnd::Create(AfxRegisterWndClass(CS_VREDRAW | CS_HREDRAW,
			LoadCursor(NULL, IDC_ARROW),
			(HBRUSH)(COLOR_BTNFACE + 1)),
		NULL,
		dwStyle,
		CRect(pos.x, pos.y, pos.x + size.cx, pos.y + size.cy),
		theApp.m_pMainWnd,
		NULL))
	{
		return FALSE;
	}								

	EnableWindow(TRUE);

	// Hide the ID and ID caption fields on this prop page -- they are
	// here only because they must go _somewhere_ and this spot is as
	// good as any other.
	//
	CWnd* pWnd = m_nullPage.GetDlgItem(IDC_ID);
	ASSERT(pWnd != NULL);
	pWnd->ShowWindow(SW_HIDE);

	pWnd = m_nullPage.GetDlgItem(IDC_ID_CAP);
	ASSERT(pWnd != NULL);
	pWnd->ShowWindow(SW_HIDE);

	// Do this so the sheet wnd will be set to the right size so the
	// ini file position stuff can calculate a rectangle...
	VERIFY(m_nullPage.SetupPage(this, NULL));

	// Chicago may want to show a system menu for this window, so let's
	// make sure it looks consistent with the rest of our app.

	CMenu* pMenu = GetSystemMenu(FALSE);
	if (pMenu != NULL)
	{
		pMenu->DeleteMenu(SC_MAXIMIZE, MF_BYCOMMAND);
		pMenu->DeleteMenu(SC_MINIMIZE, MF_BYCOMMAND);
		pMenu->DeleteMenu(SC_RESTORE, MF_BYCOMMAND);
	}

	return TRUE;
}

int CSheetWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	// Create the toolbar for the pushpin and help button
	if (!m_toolBar.Create(this) ||
	    !m_toolBar.SetButtons(propPageButtons, sizeof(propPageButtons) / sizeof(UINT)))
	{
		TRACE("Failed to create toolbar\n");
		return -1;
	}

	m_toolBar.SetSizes();
	m_toolBar.m_dwStyle |= CBRS_TOOLTIPS | CBRS_FLYBY;
	m_toolBar.EnableToolTips(TRUE);
	m_toolBar.MoveWindow(cxWndMargin, 2, CPropFrameToolBar::sizeButtonX * 2, CPropFrameToolBar::sizeButtonY);

	// Flip popping properties so that setting the push pin will work.
	theApp.m_theAppSlob.SetIntProp(P_PoppingProperties, m_toolBar.IsPinned());
	m_toolBar.SetPushPin(! m_toolBar.IsPinned());

	// Figure out where to put the pages.
	CDC* pDC = GetDC();
	int cyRowHeight = m_tabRow.DesiredHeight(pDC);
	ReleaseDC(pDC);

	int cyButtonTop = 1;
	if(cyRowHeight <= 22)
		s_cyBreakLine = 22 + 3;
	else
	{
		cyButtonTop = 1 + (cyRowHeight - 22) / 2;
		s_cyBreakLine = cyRowHeight + 3;
	}

	// Preload the pages that do not have their own class...
	VERIFY(m_multiPage.Create(IDDP_MUL, this));
	VERIFY(m_nullPage.Create(IDDP_NUL, this));
 	VERIFY(m_noPage.Create(IDDP_NOPAGE, this));

	CPoint ptPos(cxWndMargin + cxPageMargin - GetSystemMetrics(SM_CYBORDER),
		s_cyBreakLine + cyPageMargin - GetSystemMetrics(SM_CYBORDER));
	m_multiPage.MovePage(CRect(ptPos, m_multiPage.GetPageSize()));
	m_nullPage.MovePage(CRect(ptPos, m_nullPage.GetPageSize()));
	m_noPage.MovePage(CRect(ptPos, m_noPage.GetPageSize()));

	BOOL	bRet;
	
	bRet = m_cbLevel.Create(WS_CHILD | WS_VISIBLE | WS_TABSTOP | CBS_DROPDOWNLIST,// | WS_DISABLED,
		CRect(0, 0, 0, 0), this, IDC_COMBOBOX_LEVEL);

	ASSERT(bRet);
	m_cbLevel.SetFont(GetStdFont(font_Normal));

	return CMiniFrameWnd::OnCreate(lpCreateStruct);
}

void CSheetWnd::InitialUpdate(CPoint ptInitial)
{
	CRect rectWnd;
	GetWindowRect(rectWnd);

	int cxScreen = GetSystemMetrics(SM_CXSCREEN);
	int cyScreen = GetSystemMetrics(SM_CYSCREEN);

	// Calculate the top-left coordinates from the initial anchor point.
	if (ptInitial.x >= cxScreen - ptInitial.x)
		ptInitial.x -= rectWnd.Width();
	if (ptInitial.y >= cyScreen - ptInitial.y)
		ptInitial.y -= rectWnd.Height();

	rectWnd.SetRect(ptInitial.x, ptInitial.y,
		ptInitial.x + rectWnd.Width(), ptInitial.y + rectWnd.Height());
	ForceRectOnScreen(rectWnd);

	SetWindowPos(NULL, rectWnd.left, rectWnd.top, 0, 0,
	             SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);

	g_PropSheetVisible = m_toolBar.IsPinned();
}

BOOL CSheetWnd::PreTranslateMessage(MSG* pMsg)
{
	switch (pMsg->message)
	{
		case WM_KEYDOWN:
			FlushInforms();
			switch (pMsg->wParam)
			{
			case VK_LEFT:
				if (GetFocus() == this)
				{
					CmdPrevPage();
					return TRUE;
				}
				break;

			case VK_RIGHT:
				if (GetFocus() == this)
				{
					CmdNextPage();
					return TRUE;
				}
				break;

			case VK_ESCAPE:
				if (GetKeyState(VK_SHIFT) < 0)
				{
					SendMessage(WM_CLOSE);
					return TRUE;
				}
				break;

			case VK_F1:
				OnCmdHelp();
				return TRUE;
			}
			break;

		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_MBUTTONDOWN:
			FlushInforms();
			break;

		case WM_SYSCOMMAND:
			// OLE property pages might forward on of these to us.
			if(pMsg->wParam == SC_KEYMENU)
			{
				SendMessage(WM_SYSCOMMAND, pMsg->wParam, pMsg->lParam);
				return TRUE;
			}
			break;
	}
	
	// If the sheet has the focus, or the current page doesn't have a window
	// then the page has not yet had a chance to PreTranslate this message.

	if (m_pCurPage != NULL &&
		(GetFocus() == this || m_pCurPage->m_hWnd == NULL) &&
		m_pCurPage->PreTranslateMessage(pMsg))
	{
		return TRUE;
	}

	if (CMiniFrameWnd::PreTranslateMessage(pMsg))
		return TRUE;

	return FALSE;
}

//	CSheetWnd::OnPaint

void CSheetWnd::OnPaint()
{
	CPaintDC dc(this);
	if (dc.m_hDC == NULL)
	{
		// PPMOVE : [matthewt]
		// VSHELL currently does not support this
		// SetPaintEmergency();
		return;
	}

	// Draw tabs.
	dc.SetBkMode(TRANSPARENT);
	m_tabRow.Draw(&dc);

	CRect rectPage;
	GetClientRect(rectPage);
	rectPage.InflateRect(-cxWndMargin, -cyWndMargin);
	rectPage.top = s_cyBreakLine;

	if ((NULL != m_pNewSlob) &&
		m_pNewSlob->IsKindOf(RUNTIME_CLASS(CIPCompSlob)) &&
		((CIPCompSlob*)m_pNewSlob)->ShowLevelCombo())
		rectPage.bottom -= GetLevelComboHeight();

	m_tabRow.DrawPageRect(&dc, rectPage);
}

void CSheetWnd::OnPaletteChanged(CWnd* pFocusWnd)
{
	if (IsWindowVisible() && m_pCurPage->GetSafeHwnd() != NULL &&
		m_pCurPage->IsWindowVisible())
	{
		m_pCurPage->SendMessage(WM_PALETTECHANGED, 
			(WPARAM)pFocusWnd->GetSafeHwnd());
	}
}

BOOL CSheetWnd::OnQueryNewPalette()
{
	BOOL bRet = FALSE;
	
	if (IsWindowVisible())
	{
		if (m_pCurPage->GetSafeHwnd() != NULL && m_pCurPage->IsWindowVisible())
			bRet = m_pCurPage->SendMessage(WM_QUERYNEWPALETTE);

		OnPaletteChanged(m_pCurPage);
	}
	
	return bRet;
}

void CSheetWnd::OnSize(UINT nType, int cx, int cy)
{
	CDC* pDC = GetDC();

	m_cyRowHeight = m_tabRow.DesiredHeight(pDC);

	ReleaseDC(pDC);

	CRect	rect;
	CRect	rectCB;

	m_toolBar.GetWindowRect(rect);

	ScreenToClient(rect);

	rect.SetRect(rect.right + 4, s_cyBreakLine - m_cyRowHeight - 1,
		cx - cxWndMargin, s_cyBreakLine - 1);

	m_tabRow.SetPos(rect);

	if ((NULL != m_pNewSlob) &&
		m_pNewSlob->IsKindOf(RUNTIME_CLASS(CIPCompSlob)) &&
		((CIPCompSlob*)m_pNewSlob)->ShowLevelCombo())
	{
		rectCB.top = cy - (cyWndMargin + 2 + m_cyRowHeight);
		rectCB.left = cxWndMargin;
		rectCB.bottom = rectCB.top + (m_cyRowHeight * 5);
		rectCB.right = cx - cxWndMargin;

		m_cbLevel.SetWindowPos(NULL, rectCB.left, rectCB.top,
			rectCB.Width(), rectCB.Height(),
			SWP_NOZORDER | SWP_NOACTIVATE);
		m_cbLevel.ShowWindow(SW_SHOW);
	}
	else
	{
		m_cbLevel.ShowWindow(SW_HIDE);
	}

	CMiniFrameWnd::OnSize(nType, cx, cy);
}

int CSheetWnd::GetLevelComboHeight()
{
	int		iHeight = 0;

	// if the slob is a IPCompSlob and if it says that we can display
	// the combo box, return the height to accomodate the combo box
	// otherwise, return 0
	if ((NULL != m_pNewSlob) &&
		m_pNewSlob->IsKindOf(RUNTIME_CLASS(CIPCompSlob)) &&
		((CIPCompSlob*)m_pNewSlob)->ShowLevelCombo())
		iHeight = (cyWndMargin + 2 + m_cyRowHeight + cyWndMargin + 2);

	return iHeight;
}

void CSheetWnd::RecalcLayout(BOOL bNotify)
{
	// Keep MFC from changing the layout.
}

void CSheetWnd::CmdOkPage()
{
	if (Validate())
	{
		SetActive(FALSE);
		theApp.ReactivateView();

		if (! m_toolBar.IsPinned())
		{
			ShowWindow(SW_HIDE);
			g_PropSheetVisible = FALSE;
		}
	}
}

void CSheetWnd::CancelPage()
{
	m_bInCancel = TRUE;
	
	UndoPendingValidate();
	theApp.ReactivateView();

	if (! m_toolBar.IsPinned())
	{
		ShowWindow(SW_HIDE);
		g_PropSheetVisible = FALSE;
	}

	m_bInCancel = FALSE;
}

void CSheetWnd::CmdCancelPage()
{
	CancelPage();
}

void CSheetWnd::CmdPrevPage()
{
	IncrementPage(-1);
}

void CSheetWnd::CmdNextPage()
{
	IncrementPage(1);
}

void CSheetWnd::IncrementPage(int nInc)
{
	int cItems = m_tabRow.MaxTab() + 1;
	if (cItems == 0 || !Validate())
		return;

	int iActiveTab = m_tabRow.GetActiveTab();
	m_tabRow.SetActiveTab((iActiveTab + nInc + cItems) % cItems);
	OnSelPage();
	if (!m_tabRow.HasFocus())
		RestoreFocus();
}

void CSheetWnd::OnCmdPushPin()
{
	m_toolBar.SetPushPin(! m_toolBar.IsPinned());
}

void CSheetWnd::OnCmdHelp()
{
	// If there is a current popup, give it the first chance to handle
	// this help request.
	CWnd* pWnd = GetTopLevelParent();
	HWND hWnd = ::GetLastActivePopup(pWnd->GetSafeHwnd());

	// IStudio bug# 4031 & 2437 (fixed by scotg)
	// if we are the last active popup window, there is no need
	// to send the WM_COMMANDHELP message to ourself, since we
	// are already processing a WM_COMMANDHELP message.  this
	// is how we got here in the first place.
	if(hWnd != this->GetSafeHwnd())
	{
		if (::SendMessage(hWnd, WM_COMMANDHELP, 0, 0))
			return;
	}

	// Otherwise, let the current property page process the help request.
	OnCommandHelp(0, 0);
}

void CSheetWnd::OnSelChangeLevel()
{
	// if the slob is of type IPCompSlob, get its internal track selection
	// interface pointer and inform that of the internal select change
	if ((m_pNewSlob != NULL) &&
		(m_pNewSlob->IsKindOf(RUNTIME_CLASS(CIPCompSlob))))
	{
		int		iIndex;
		DWORD	dwCookie;

		iIndex = m_cbLevel.GetCurSel();
		ASSERT(iIndex != CB_ERR);

		dwCookie = m_cbLevel.GetItemData(iIndex);

		HRESULT								hr;
		COleRef<IInternalTrackSelection>	srpITS;

		hr = ((CIPCompSlob*)m_pNewSlob)->GetInternalTrackSelection(&srpITS);
		if (NOERROR == hr)
			srpITS->OnInternalSelectChange(dwCookie);
	}
}

LRESULT CSheetWnd::OnCommandHelp(WPARAM wParam, LPARAM lParam)
{
	IdleUpdate();	// Make sure the slobs up to date.

	if (m_pCurPage != NULL)
		return (S_OK == m_pCurPage->OnPageHelp(wParam, lParam));

	return FALSE;
}

LRESULT CSheetWnd::OnSetMessageString(WPARAM wParam, LPARAM lParam)
{
	LRESULT lr = AfxGetMainWnd()->SendMessage(WM_SETMESSAGESTRING, wParam, lParam);

	UINT nIDLast = m_nIDLastMessage;
	m_nIDLastMessage = (UINT)wParam;    // new ID (or 0)
	m_nIDTracking = (UINT)wParam;       // so F1 on toolbar buttons work

	return lr;
}

BOOL CSheetWnd::OnToolTipText(UINT nID, NMHDR* pNMHDR, LRESULT* pResult)
{
	ASSERT(pNMHDR->code == TTN_NEEDTEXTA || pNMHDR->code == TTN_NEEDTEXTW);

	if (!theApp.m_bToolTips)
	{
		CTheApp::SetEmptyToolTip(pNMHDR);
	
		*pResult = 0;
		return TRUE;
	}

	return CMiniFrameWnd::OnToolTipText(nID, pNMHDR, pResult);
}

BOOL CSheetWnd::UndoPendingValidate()
{
	if (m_pCurPage == NULL)
		return FALSE;
	
	return m_pCurPage->UndoPendingValidate();
}

BOOL CSheetWnd::FlipToPage(int iStringID)
{
	// iStringID == -1 means the first page no matter what its name is.
	//
	if (iStringID == -1)
	{
		m_tabRow.SetActiveTab(0);
		OnSelPage();
		RestoreFocus();
		return TRUE;
	}
	else
	{
		CString str;
		str.LoadString(iStringID);

		return FlipToPage(str);
	}

	return FALSE;
}

BOOL CSheetWnd::FlipToPage(const CString& str)
{
	if (m_tabRow.SetActiveTab(str) != -1)
	{
		OnSelPage();
		RestoreFocus();
		return TRUE;
	}

	return FALSE;
}

void CSheetWnd::OnSelPage()
{
	// scotg ASSERT(Validate());
	Validate();

	SaveFocus();
	SelectionChange(m_selState, m_pCurSlob, m_tabRow.GetActiveTab());
	IdleUpdate(); // force immediate selection change

	// Make this new one the default page since the user explicitly
	// selected it...
	SetDefaultPage();
}

// Set the default page to the one currently selected.
void CSheetWnd::SetDefaultPage()
{
	m_strDefaultPage = m_tabRow.GetTabString(m_tabRow.GetActiveTab());
}

void CSheetWnd::OnLButtonDown(UINT nHitTest, CPoint point)
{
	AfxCancelModes(m_hWnd);

	int nTab = m_tabRow.TabFromPoint(point);
	switch(nTab)
	{
		default:
			if (Validate())
			{
				ASSERT(nTab >= 0);
				
				m_iFocusSave = 0;
				m_tabRow.SetActiveTab(nTab);

				SetFocus();
				OnSelPage();
			}
			break;

		case CTabRow::SCROLL_LEFT:
		case CTabRow::SCROLL_RIGHT:

			m_tabRow.Scroll(nTab);
			m_tabRow.Capture(nTab);
			break;

		case -1:
			break;
	}

	CMiniFrameWnd::OnLButtonDown(nHitTest, point);
}

void CSheetWnd::OnTimer(UINT nTimerID)
{
	if (m_tabRow.HasCapture())
	{
		ASSERT(nTimerID == CTabRow::TIMER_ID);
		m_tabRow.OnTimer();
	}
	else
		CMiniFrameWnd::OnTimer(nTimerID);
}

void CSheetWnd::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_tabRow.HasCapture())
		m_tabRow.OnMouseMove(point);
	else
		CMiniFrameWnd::OnMouseMove(nFlags, point);
}

void CSheetWnd::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_tabRow.HasCapture())
		m_tabRow.OnLButtonUp(point);
	else
		CMiniFrameWnd::OnLButtonUp(nFlags, point);
}

void CSheetWnd::OnNcLButtonDown(UINT nHitTest, CPoint point)
{
	AfxCancelModes(m_hWnd);

	switch( nHitTest )
	{
		case HTCAPTION:
		case HTLEFT:
		case HTTOP:
		case HTRIGHT:
		case HTBOTTOM:
		case HTTOPLEFT:
		case HTTOPRIGHT:
		case HTBOTTOMLEFT:
		case HTBOTTOMRIGHT:
		{
			CSlobWnd* pView = (CSlobWnd*)((CMainFrame*)theApp.m_pMainWnd)->GetActiveFrame()->
				GetActiveView();
			if( (pView != NULL) && pView->IsKindOf(RUNTIME_CLASS(CSlobWnd)) )
				pView->ObscureSprites();
			break;
		}
	}

	CMiniFrameWnd::OnNcLButtonDown(nHitTest, point);
}

int CSheetWnd::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message)
{
	IdleUpdate();	// Make sure the slobs up to date.

	return CMiniFrameWnd::OnMouseActivate(pDesktopWnd, nHitTest, message);
}

//	CSheetWnd::OnClose
//		A sheet is usally created by the parent, and will be destroyed
//		specifically by the parent upon leaving the app.  When the user closes
//		the sheet, it is simply hidden.  The parent can then reshow it without
//		recreating it.

void CSheetWnd::OnClose()
{
	if (!Validate())
		return;
		 
 	g_PropSheetVisible = FALSE;
	ShowWindow(SW_HIDE);

	m_toolBar.SetPushPin(FALSE);
}

void CSheetWnd::ClearPage()
{
	// if we're displaying a page, cause commits & then close the
	// page.

	if (m_pCurPage != NULL)
	{
		m_pCurPage->TermPage();
		m_pCurPage->ShowPage(SW_HIDE);
		m_pCurPage = NULL;

		m_pCurSlob = NULL;
	}
}

//	CSheetWnd::OnSysCommand
//		Implements Alt+F6 and Shift+Alt+F6 for  the SheetWnd to
//		be same as pressing Enter.

void CSheetWnd::OnSysCommand(UINT nID, LONG lParam)
{
	switch (nID & 0xfff0)
	{
		case SC_NEXTWINDOW:
		case SC_PREVWINDOW:
			if (LOWORD(lParam) == VK_F6)
			{
				if (m_pCurPage != NULL)
					CmdOkPage();
				else
					GetParent()->SetFocus();
				return;
			}
			break;

		case SC_KEYMENU:
			if (LOWORD(lParam) == VK_RETURN)
			{
				CmdOkPage();
				return;
			}
			// Check lParam.  If it is 0L, then the user may have done
			// an Alt+Tab, so just ignore it.  This breaks the ability to
			// just press the Alt-key and have the first menu selected,
			// but this is minor compared to what happens in the Alt+Tab
			// case.
			else if (lParam == 0L || !Validate())
			{
				return;
			}
		
		//FALL THROUGH
		case SC_CLOSE:
			if (((nID & 0xfff0) == SC_CLOSE && lParam == 0L) ||
				((nID & 0xfff0) == SC_KEYMENU && lParam != 0L))
			{
				CWnd* pParent = GetParent();
				ASSERT(pParent != NULL);

				ASSERT(!bQuitting);
				pParent->SetActiveWindow();

				if (! m_toolBar.IsPinned())
				{
					ShowWindow(SW_HIDE);
					g_PropSheetVisible = FALSE;
				}
				pParent->SendMessage(WM_SYSCOMMAND, nID, lParam);
				if (nID & 0xfff0 != SC_CLOSE)
				{
					ASSERT(IsWindow(m_hWnd));
					SetActiveWindow();
				}

				return;
			}
			break;
		case SC_MOVE:
		case SC_SIZE:
		{
			CSlobWnd* pView = (CSlobWnd*)((CMainFrame*)theApp.m_pMainWnd)->
				GetActiveFrame()->GetActiveView();
			if( (pView != NULL) && pView->IsKindOf(RUNTIME_CLASS(CSlobWnd)) )
				pView->ObscureSprites();
			break;
		}
	}

	CMiniFrameWnd::OnSysCommand(nID, lParam);
}

void CSheetWnd::OnEnable(BOOL bEnable)
{
	// Deal with the frame too so dialog boxes using this as the owner
	// are truly modal...
	if (!theApp.m_pMainWnd->IsWindowEnabled() != !bEnable)
		theApp.m_pMainWnd->EnableWindow(bEnable);

	CMiniFrameWnd::OnEnable(bEnable);

	// We used to disable the page here for VB to get page notification.
	// This caused DOLPHIN Bug 2574.  If you think you need this kind
	// of notification, beware.
}

void CSheetWnd::DrawFocusRect()
{
	if (!m_tabRow.HasFocus() || m_tabRow.MaxTab() < 0)
		return;		

	CRect rect;
	if (GetUpdateRect(rect) && rect.IntersectRect(rect, m_tabRow.GetRect()))
	{
		m_tabRow.InvalidateTab(m_tabRow.GetActiveTab());
		return;
	}

	CDC* pDC = GetDC();
	m_tabRow.DrawFocusRect(pDC);
	ReleaseDC(pDC);
}

void CSheetWnd::OnSetFocus(CWnd* pOldWnd)
{
	ASSERT(!bQuitting);
	m_tabRow.PutFocus(TRUE);
	DrawFocusRect();
}

void CSheetWnd::OnKillFocus(CWnd* pNewWnd)
{
	if (IsWindowVisible() && m_tabRow.HasFocus())
		DrawFocusRect();

	m_tabRow.PutFocus(FALSE);
}

//	CSheetWnd::OnActivate
//		Performs Save and Restore on focus.  Also sets the "modal window"
// 		flag, which turns on the validation checking if the user clicks
// 		away from this window.  We do an OnIdle hide/show of the property
// 		sheet here just to make sure nothing can throw a PoppingProp sheet
// 		out of synch.

void CSheetWnd::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
//	CMiniFrameWnd::OnActivate(nState, pWndOther, bMinimized);

	static BOOL bDisableIME;
	if (nState == WA_INACTIVE)
	{
		// hide the property sheet only if we're losing activation to
		// one of our app's own windows -- if we lose activation to
		// a window of another app, we don't want to hide, because
		// doing so will cause us to not appear in the alt-tab list.

		// NOTE:  pWndOther is NULL under NT if you are losing focus
		// to another application!

		// Loop on GetParent to support activation loss to other popups.
		while (pWndOther != NULL && pWndOther != AfxGetMainWnd())
			pWndOther = pWndOther->GetParent();

		if (pWndOther == AfxGetMainWnd())
		{
			if (! m_toolBar.IsPinned())
				g_PropSheetVisible = FALSE;

			theApp.m_pMainWnd->SendMessage(WM_SETAPPVALIDATE, (UINT) NULL);
	
			// inform the current slob page of this activation change
			if (m_pCurPage != NULL)
				m_pCurPage->Activate(nState, m_pCurSlob);
		}

		SaveFocus();

		if (bDisableIME)
		{
			theIME.EnableIME(FALSE);
			bDisableIME = FALSE;
		}
	}
	else
	{
		// inform the current slob page of this activation change
		if (m_pCurPage != NULL)
			m_pCurPage->Activate(nState, m_pCurSlob);

		g_PropSheetVisible = TRUE;
		RestoreFocus();

		theApp.m_pMainWnd->SendMessage(WM_SETAPPVALIDATE, (UINT) m_hWnd);

		SetActive(TRUE);

		if (theIME.IsEnabled())
		{
			bDisableIME = FALSE;
		}
		else
		{
			bDisableIME = TRUE;
			theIME.EnableIME(TRUE);
		}
	}
}

void CSheetWnd::SetActive(BOOL bActive)
{
TRACE1("CSheetWnd::Activate(%d)\n", bActive);

	m_bActive = bActive;

	if (m_pCurPage != NULL)
	{
		if (bActive)
			m_pCurPage->OnActivate();
		else
			m_pCurPage->OnDeactivate();
	}
}

void CSheetWnd::CurrentSlobDestroyed()
{
	m_pCurSlob = (CSlob *)NULL;	// destroyed!
	SetPropertyBrowserSelection(NULL);	// update view
}

//	CSheetWnd::SelectionChange
// 		Called when the property page needs to change to reflect a newly
//		selected object.  This is generally called by the editor or the
//		selection.	This is a "deferred change", i.e. the property page
//		maintains it's current state until IdleUpdate() is called, at which
//		time the property page changes to the new selection.  This avoids
//		nastyness in the switching of the property sheet.

void CSheetWnd::SelectionChange(CSheetWnd::SELSTYLE selState,
	CSlob *pEditor, int nPage)
{
    if(selState == editorupdate)
    {
        // force an update eneven though the slob is the same
        m_selState = selState;              // old different from new
        selState = CSheetWnd::editor;       // new is editor
    }

	m_pNewSlob = pEditor;
	m_newSelState = selState;
	m_nNewPage = nPage;
}

//	CSheetWnd::IdleUpdate
//		Called when the we hit the idle loop, or when the property sheet should
//		really be updated -- this must be called at a "safe" time, (i.e. not
//		in a focus change handler).	This actually updates the property page.

void CSheetWnd::IdleUpdate()
{
	// PPMOVE : [matthewt]
	// VSHELL currently does not support this
	//if (InEmergencyState())
	//	return;

	// if validation of the property page has already happened, it will
	// likely have disabled further validation until the idle loop, so
	// re-enable validation.

	m_bValidateEnabled = TRUE;

	// if selection didn't change, then don't update the property
	// page at all,
	if (m_pNewSlob == m_pCurSlob &&
		m_newSelState == m_selState &&
		m_nNewPage == m_nCurPage)
	{
		FlushInforms(); // make sure controls are up to date
		return;
	}

	// We just changed the selection, so we won't need to handle
	// any of these pending updates...
	while (!m_sheetSlob.m_informList.IsEmpty())
		delete m_sheetSlob.m_informList.RemoveHead();

    // otherwise go through the work.

	if (GetActiveWindow() == this)
		SaveFocus();

	if (m_pNewSlob == NULL)
	{
		m_newSelState = CSheetWnd::blank;
	}

	// before setting up pages, see if we can resolve a mutliple selection
	// to a single selection
	if( (m_pNewSlob != NULL) && m_pNewSlob->IsKindOf(RUNTIME_CLASS(CMultiSlob))
		&& (m_pNewSlob->GetContentList()->GetCount() == 1) )
	{
		POSITION pos = m_pNewSlob->GetHeadPosition();
		CSlob* pSlob = m_pNewSlob->GetNext(pos);
		ASSERT( (pSlob != NULL) && (pos == NULL) );
		m_pNewSlob = pSlob;
		m_newSelState = CSheetWnd::editor;
	}

	// Ask the m_pNewSlob to add any property pages it wants to
	// This should also set the caption
	SetupPropertyPages();

	// make sure we come out of a multi-selection ok
	if( (m_pNewSlob != NULL) && m_pNewSlob->IsKindOf(RUNTIME_CLASS(CMultiSlob)) )
	{
		POSITION pos = m_pNewSlob->GetHeadPosition();
		if( m_pNewSlob->GetContentList()->GetCount() == 0 )
		{
			ASSERT( m_listNewPages.IsEmpty() );
			m_pNewSlob = NULL;
			m_newSelState = CSheetWnd::none;
		}
		else
		{
			// this would have been caught above
			ASSERT( m_pNewSlob->GetContentList()->GetCount() > 1 );

			if( m_listNewPages.IsEmpty() )
			{
				// we have a multiple selection with no common pages
				m_pNewSlob = NULL;
				m_newSelState = CSheetWnd::multiple;
			}
		}
	}

	// Fill in the list of pages.
	// NOTE: I used to try to optimize this by only filling the list when
	// the class of the slob changed, but unfortunatly even the style of
	// the parent of a slob can affect the list of pages...

	// IStudio bug# 964  it is possible for inplace components to use the same
	// slob but through the use of ITrackSelection change the objects that are
	// to be displayed in the property frame.  Thus the additional check for an
	// CIPCompSlob is necessary to allow ipcslobs to display new property pages
	// from the same slob object.
//	if( m_pNewSlob != m_pCurSlob
//		|| m_pNewSlob != NULL && m_pNewSlob->IsKindOf(RUNTIME_CLASS(CMultiSlob))
//		|| m_pNewSlob != NULL && m_pNewSlob->IsKindOf(RUNTIME_CLASS(CIPCompSlob)) )
	{
		m_tabRow.ResetContent();

		if( m_listNewPages.GetCount() > 0 )
		{
			C3dPropertyPage* pPage;
			CDC* pDC = GetDC();
			CString str;

			POSITION pos = m_listNewPages.GetHeadPosition();
			while( pos != NULL )
			{
				pPage = (C3dPropertyPage*)m_listNewPages.GetNext(pos);
				pPage->GetPageName(str);
				m_tabRow.AddTab(str, pDC, 0);
			}

			ReleaseDC(pDC);
		}
	}

	if( m_pNewSlob != NULL )
	{
		m_nNewPage = m_nNewPage != -1 ?
			m_tabRow.SetActiveTab(m_nNewPage) :
			m_tabRow.SetActiveTab(m_strDefaultPage);

		if( m_nNewPage == -1 )
		{
			m_nNewPage = 0;
			m_tabRow.SetActiveTab(0);
		}
	}

	m_pCurSlob = m_pNewSlob;
	m_selState = m_newSelState;
	m_nCurPage = m_nNewPage;

	// move all new pages to the CurPages list
	m_listCurPages.RemoveAll();
	m_listCurPages.AddTail(&m_listNewPages);
	m_listNewPages.RemoveAll();

	if( (m_pCurSlob == NULL) && (m_selState == CSheetWnd::editor) )
		m_selState = CSheetWnd::none;

	C3dPropertyPage* pNewPage = NULL;

	switch (m_selState)
	{
		case CSheetWnd::none:
			pNewPage = &m_nullPage;
			break;

		case CSheetWnd::blank:
			ClearPage();
			pNewPage = &m_noPage;
			break;

		case CSheetWnd::multiple:
			if( m_pNewSlob == NULL )
			{
				ASSERT( m_listCurPages.IsEmpty() );
				pNewPage = &m_multiPage;
				break;
			}
			// otherwise fall through

		case CSheetWnd::editor:
		{
			if( m_listCurPages.IsEmpty() )
			{	// some perfectly valid selections have no property pages
				pNewPage = &m_noPage;
				break;
			}

			if( (m_nCurPage < 0) || (m_nCurPage >= m_listCurPages.GetCount()) )
				m_nCurPage = 0;
			POSITION posPage = m_listCurPages.FindIndex(m_nCurPage);
			if( posPage != NULL )
				pNewPage = (C3dPropertyPage*)m_listCurPages.GetAt(posPage);
			// We can't deal with having a NULL page on a usable SheetWnd
			if( pNewPage == NULL )
				pNewPage = &m_noPage;
			break;
		}
	}

	// ok, now pNewPage is the property page we want to display, so
	// hide the old one and show the new one in one operation, optimize
	// for the case where they're the same page.
	//
	if (pNewPage != NULL)
	{
		if (!pNewPage->SetupPage(this, m_pNewSlob))
		{
			pNewPage = NULL;
		}
		else
		{
			pNewPage->InitializePage();
			pNewPage->ShowPage(SW_SHOW);

			if (pNewPage->m_hWnd != NULL)
				pNewPage->SendMessage(WM_QUERYNEWPALETTE);
		}
	}

	// if the new and old pages aren't the same, then hide the old one
	C3dPropertyPage* pOldPage = m_pCurPage;
	m_pCurPage = pNewPage;

	if (pOldPage != NULL && pOldPage != pNewPage)
		pOldPage->ShowPage(SW_HIDE);

	// Need to restore focus before getting rid of the old page because the
	// old one will get a kill focus...  Also need to update m_pCurPage, since
	// RestoreFocus depends on it.
	//

	if (GetActiveWindow() == this)
		RestoreFocus();

	// if the new and old pages aren't the same, then hide the old one
	if (pOldPage != NULL && pOldPage != pNewPage)
		pOldPage->TermPage();

	m_bSheetChanged = TRUE;
}

LRESULT CSheetWnd::OnIsError(WPARAM wIfEnabled, LPARAM)
{
	if (wIfEnabled && !m_bValidateEnabled)
		return TRUE;

	return !Validate();
}

// CSheetWnd::Validate
//
// This validates the information in the currently displayed property
// sheet (if any), and saves away the information -- if any information
// is not valid, this will show a dialog notifying the user of the error,
// setfocus to the appropriate field, and return FALSE.	It will
// return TRUE if the property sheet was validated and it's information
// stored in the viewed object.
//
// If we're already in a validation routine, this will return TRUE to
// allow operations to proceed without piling up.

BOOL CSheetWnd::Validate()
{
	BOOL bResult = TRUE;

	if (m_pCurPage && !m_bValidating)
	{
		m_bValidating = TRUE;
		bResult = m_pCurPage->Validate();
		m_bValidating = FALSE;

		// If the validation failed, the validate enable flag is cleared.
		// Certain validation code uses this flag to avoid validating twice
		// on a single user action, and hence showing two error message
		// boxes.  The flag is reset in IdleUpdate.

		if (!bResult)
			m_bValidateEnabled = FALSE;
	}

	return bResult;
}

// OnValidateReq
//
// This responds to a "special" message which is posted when an edit
// loses focus, and validation of that edit's contents need to be
// done.  It may change the focus back to the edit.   This should
// !!not!! do validation on focus changes outside of the property sheet,
// as they are handled elsewhere!
static const GUID CLSID_ADODC_General = 
{0xaa2073e6, 0x7b9c, 0x11d0, {0xb1, 0x43, 0x00, 0xa0, 0xc9, 0x22, 0xe8, 0x20}};

static const GUID CLSID_ADODC_Authenticate  = 
{0x27f8ffb1, 0x7406, 0x11d1, {0xb1, 0x8c, 0x00, 0xa0, 0xc9, 0x22, 0xe8, 0x20}};

static const GUID CLSID_ADODC_RecordSource = 
{0x27f8ffb2, 0x7406, 0x11d1, {0xb1, 0x8c, 0x00, 0xa0, 0xc9, 0x22, 0xe8, 0x20}};

LRESULT CSheetWnd::OnValidateReq(WPARAM, LPARAM)
{
	// Don't handle focus changes if the window the focus changed to is
	// not the property window or one of its descendants.

	CWnd* pWndFocus = GetFocus();
	if (this == pWndFocus || IsChild(pWndFocus))
	{
		// HACK HACK HACK:
		// This is to disable validation when we type a character.
		// Instead we only want to validate when we switch page
		// This is done only on the ADODC control
		BOOL bValidate = TRUE;
		if (m_pCurPage->IsKindOf (RUNTIME_CLASS (COlePage)))
		{
			CLSID clsid = CLSID_NULL ;
			((COlePage *)m_pCurPage)->GetCLSID(&clsid); 
			bValidate = !(::IsEqualCLSID (clsid, CLSID_ADODC_Authenticate) ||
						  ::IsEqualCLSID (clsid, CLSID_ADODC_RecordSource) ||	
						  ::IsEqualCLSID (clsid, CLSID_ADODC_General));
		}
		if (bValidate)
			Validate();
	}
	return TRUE;
}

BOOL CSheetWnd::ValidateRequest()
{
	PostMessage(WM_USER_VALIDATEREQ);
	
	return TRUE;
}

// 	CSheetWnd::SetEditFocus
//		Sets the focus to an edit field of the general property page,
//		if present -- if no edit property with ID is present, or there is no
//		valid current page, the focus is not changed.
//
//		This also selects the entire edit text, so that any keystrokes will
//		replace the current caption.
//
//		The edit window (if any) is returned, in case a message should be
//		posted to it.

CWnd* CSheetWnd::SetEditFocus(UINT nProp)
{
	ASSERT(!bQuitting);
	
	IdleUpdate();	// Make sure the slobs up to date.

	if (m_pCurSlob == NULL || !Validate())
		return NULL;

	// Look out for pending slob changes.
	//
	POSITION pos = m_listCurPages.GetHeadPosition();
	for( int nPage = 0; pos != NULL; nPage++ )
	{
		CSlobPage* pPage = (CSlobPage*)m_listCurPages.GetNext(pos);
		ASSERT( pPage != NULL );

		ASSERT(pPage->IsKindOf(RUNTIME_CLASS(CSlobPage)));
		CControlMap* pCtlMap = pPage->FindProp(nProp);
		if (pCtlMap == NULL)
			continue;
		CWnd* pControl = (CWnd*) pPage->GetDlgItem(pCtlMap->m_nCtlID);
		if (pControl == NULL)
			continue;
		if (!pControl->IsWindowEnabled())
			return NULL;

		if (nPage != m_tabRow.GetActiveTab())
		{
			m_tabRow.SetActiveTab(nPage);
			OnSelPage();
		}

		CString str;
		::GetClassName(pControl->m_hWnd, str.GetBuffer(256), 256);
		str.ReleaseBuffer();

		pControl->SetFocus();
		if (str.CompareNoCase("Edit") == 0)
			((CEdit*)pControl)->SetSel(0,32727);
		else
		{
			// We only support combo's and edits.
			ASSERT(str.CompareNoCase("ComboBox") == 0);
			((CComboBox*)pControl)->SetEditSel(0,32727);
		}

		return pControl;
	}

	return NULL;
}

CWnd* CSheetWnd::GetControl(UINT nControlID)
{
	IdleUpdate();	// Make sure the slobs up to date.

	if( (m_pCurSlob != NULL) && (m_listCurPages.GetCount() > 0) )
	{
		// Look out for pending slob changes.
		//
		C3dPropertyPage* pPage = (C3dPropertyPage*)m_listCurPages.GetHead();
		if (pPage->GetSafeHwnd() == NULL)
			return NULL;

		CWnd* pControl = (CWnd*) pPage->GetDlgItem(nControlID);
		return pControl;
	}
	return NULL;
}

//	CSheetWnd::SaveFocus
//		Saves the current page's focus in the form of the control's
//		ID, so that we can restore focus even if the page (and hence the
//		controls' hWnds) have changed.

#define FOCUS_TABS -1

BOOL CSheetWnd::SaveFocus()
{
	CWnd* pWnd = GetFocus();
	if (m_pCurPage == NULL || (pWnd == this && m_tabRow.HasFocus()))
	{
		m_iFocusSave = FOCUS_TABS;
		return TRUE;
	}
	else if(pWnd != NULL)
	{
		if (m_pCurPage->GetSafeHwnd() == NULL)
		{
			// No page window, so just save the hwnd with focus.

			m_hwndFocusSave = pWnd->GetSafeHwnd();
			m_iFocusSave = NULL;
			m_bSheetChanged = FALSE;
			return TRUE;
		}
		else if(m_pCurPage->IsChild(pWnd))
		{
			// Search up for first child, so we don't get part of a
			// combo-box.

			while(pWnd->GetParent() != m_pCurPage)
				pWnd = pWnd->GetParent();

			m_iFocusSave = pWnd->GetDlgCtrlID();
			m_hwndFocusSave = NULL;
			m_bSheetChanged = FALSE;
			return TRUE;
		}
	}

	m_iFocusSave = NULL;
	return FALSE;
}

//	CSheetWnd::RestoreFocus
//		Restores the focus to the SheetWnd using the saved ID.

BOOL CSheetWnd::RestoreFocus()
{
	ASSERT(!bQuitting);
	
	BOOL bRestored = FALSE;

    if (m_pCurPage != NULL)
	{
	 	// Don't restore the focus if we are disabled, give the tab focus.
		if (m_iFocusSave == FOCUS_TABS || m_pCurPage->IsPageDisabled())
		{
			SetFocus();
			return TRUE;
		}
		else if (m_iFocusSave == NULL || m_pCurPage->GetSafeHwnd() == NULL)
		{
			if (::IsWindow(m_hwndFocusSave)	&&
					::IsChild(m_hWnd, m_hwndFocusSave) &&
					::IsWindowVisible(m_hwndFocusSave) &&
					::IsWindowEnabled(m_hwndFocusSave))
				::SetFocus(m_hwndFocusSave);
			else
				m_pCurPage->ShowPage(SW_SHOWNORMAL);

			return TRUE;
		}

		CWnd* pWnd = m_pCurPage->GetDlgItem(m_iFocusSave);

		// If no window with the ID is in this dialog, or the window is
		// disabled, or it just isn't a tabstop, set focus to the dialog
		// itself, which will set focus to it's first tabstop.
		// because this will seem like a focus change, be sure to save
		// the old focussed id so that we can restore there when we can!
		// ggh 08-Sep-92

		if (pWnd == NULL || !pWnd->IsWindowEnabled() ||
			 !(pWnd->GetStyle() & WS_TABSTOP))
		{
			UINT oldFocus = m_iFocusSave;
		 	bRestored = (m_pCurPage->SetFocus() != NULL);
		 	m_iFocusSave = oldFocus;
		}
		else		// we can restore focus where it was saved!
		{
			char szClass[5];
			::GetClassName(pWnd->m_hWnd, (LPSTR) szClass, 5);

			// If we are not restoring to an edit, or we are restoring
			// to the unchanged edit from which we saved, then just SetFocus.

			if (lstrcmp(szClass, "Edit") || !m_bSheetChanged)
			{
				bRestored = (pWnd->SetFocus() != NULL);
			}

			// This is a new/changed editbox, so select its contents.

			else
			{
				bRestored = (pWnd->SetFocus() != NULL);
				((CEdit*) pWnd)->SetSel(0,0x7FFF);
			}
		}
	}

	return bRestored;
}

void CSheetWnd::FlushUnusedPages()
{
	TRACE("Flushing unused property pages...\n");
	POSITION pos = s_listLoadedPages.GetHeadPosition();
	while (pos != NULL)
	{
		POSITION posThis = pos;
		C3dPropertyPage* pPage = (C3dPropertyPage*)s_listLoadedPages.GetNext(pos);

		if (pPage != &m_nullPage && pPage != &m_multiPage &&
			pPage != &m_noPage && pPage != m_pCurPage)
		{
			TRACE1("Destroying page 0x%08lx\n", pPage);

			CWnd* pWnd;
			while ((pWnd = pPage->GetDlgItem(IDC_ID)) != NULL)
				pWnd->SetParent(&m_nullPage);
			while ((pWnd = pPage->GetDlgItem(IDC_ID_CAP)) != NULL)
				pWnd->SetParent(&m_nullPage);

			pPage->DestroyWindow();
			s_listLoadedPages.RemoveAt(posThis);
		}
	}
}

void CSheetWnd::RemovePageFromUse(C3dPropertyPage* pPage)
{
	POSITION pos = CSheetWnd::s_listLoadedPages.Find(pPage);
	if (pos != NULL)
		CSheetWnd::s_listLoadedPages.RemoveAt(pos);
}

/* SetupPropertyPages
 *	Set up property pages on the newly selected slob
 */
int CSheetWnd::SetupPropertyPages()
{
	// In case no one sets the caption...
	SetCaption(NULL);	// set it to "Properties"

	// initially, extra pages are allowed whenever the slob likes
	m_refInhibitExtraPages = 0;

	// let the slob add as many pages as they like
	if( m_pNewSlob != NULL )
	{
		m_pNewSlob->SetupPropertyPages(m_pNewSlob, TRUE);

		// if the slob is an CIPCompSlob, get the internal track selection
		// and find out if an external change happened.  If so, repopulate
		// the combo box
		if (m_pNewSlob->IsKindOf(RUNTIME_CLASS(CIPCompSlob)))
		{
			HRESULT								hr;
			COleRef<IInternalTrackSelection>	srpITS;

			hr = ((CIPCompSlob*)m_pNewSlob)->GetInternalTrackSelection(&srpITS);
			if (NOERROR == hr)
			{
				VARIANT_BOOL	bRet = VARIANT_FALSE;

				hr = srpITS->IsExternalSelectChange(&bRet);
				if ((NOERROR == hr) && (bRet == VARIANT_TRUE))
				{
					CALPOLESTR	caStringsOut;
					CADWORD		caCookiesOut;

					// delete all the items from the combo box
					m_cbLevel.ResetContent();

					hr = srpITS->GetInternalSelectList(&caStringsOut, &caCookiesOut);
					if (NOERROR == hr)
					{
						// release the interfaces of the old items from the combo box
						int			iIndex;

						USES_CONVERSION;

						// now add new items to the combo box
						ASSERT(caStringsOut.cElems == caCookiesOut.cElems);
						for (int i = 0; i < (int) caStringsOut.cElems; ++i)
						{
							iIndex = m_cbLevel.AddString(OLE2T(caStringsOut.pElems[i]));
							ASSERT(iIndex != CB_ERR);
							ASSERT(iIndex != CB_ERRSPACE);
							ASSERT(NULL != caCookiesOut.pElems[i]);
							m_cbLevel.SetItemData(iIndex, (DWORD)caCookiesOut.pElems[i]);
							AfxFreeTaskMem(caStringsOut.pElems[i]);
						}

						AfxFreeTaskMem(caStringsOut.pElems);
						AfxFreeTaskMem(caCookiesOut.pElems);

						m_cbLevel.SetCurSel(0);
					}
				}
			}
		}

		// show/hide the combo box depending upon whether
		// we need have HTML Elements to edit
		if (GetLevelComboHeight() > 0)
			m_cbLevel.ShowWindow(SW_SHOW);
		else
			m_cbLevel.ShowWindow(SW_HIDE);
	}

	// Try to append extra property pages from other packages.
	// If this has already been done, or is otherwise inhibited, this is harmless.
	AppendExtraPages();

	return m_listNewPages.GetCount();
}

/* AddPropertyPage
 *	Add a property page to the list.  
 *	Also, specifies what slob contains the data that this page reflects.
 */
int CSheetWnd::AddPropertyPage(C3dPropertyPage* pPage, CSlob* pSlob)
{
	ASSERT( pPage != NULL );
	// Set page's m_pSlob to be the slob passed to us.
	// The provider can pass NULL to use the m_pNewSlob of the whole sheet.
	// Finally, if m_pNewSlob is actually a multiple selection, and pSlob is 
	// actually a member of the selection, then point to the multiple selection.
	if( (pSlob == NULL) || (m_pNewSlob->IsKindOf(RUNTIME_CLASS(CMultiSlob)) &&
		m_pNewSlob->GetContentList()->Find(pSlob)) )
	{
		pPage->m_pSlob = m_pNewSlob;
	}
	else
		pPage->m_pSlob = pSlob;

	m_listNewPages.AddTail(pPage);
	return m_listNewPages.GetCount() - 1;	// return index of page in list
}

/* SetCaption
 *	Allows the owner of a property page to set the noun part of our caption 
 *	(e.g. "Foo" Properties).  The word " Properties" is appended automatically.
 */
void CSheetWnd::SetCaption(LPCTSTR sz)
{
	CString strCaption, strProperties;
	VERIFY( strProperties.LoadString(IDS_PROPERTIES) );

	strCaption.Format((LPCTSTR)strProperties, (sz == NULL) ? _T("") : sz);
	SendMessage(WM_SETTEXT, 0, (LPARAM)(LPCTSTR)strCaption);
}

/* AppendExtraPages
 *	Calls a virtual function in CPackage that allows them to add on extra 
 *	property pages of their own, depending on the selection.
 */
BOOL CSheetWnd::AppendExtraPages(void)
{
	if( m_refInhibitExtraPages > 0 )
		return FALSE;

	CPartView* pView = (CPartView*)CWnd::FromHandle(theApp.GetActiveView());
	if( pView == NULL )
		return FALSE;
	ASSERT( pView->IsKindOf(RUNTIME_CLASS(CPartView)) );

	// go get the GUID of the current view, and the selection interface
	GUID guidView;
	IUnknown* piUnkSel;

	if( !pView->GetSelectionInterface(&guidView, &piUnkSel) )
		return FALSE;	// lacking a guid and interface, there's no point

	POSITION pos = theApp.m_packages.GetHeadPosition();
	while( pos != NULL )
	{
		CPackage* pPackage = (CPackage*)theApp.m_packages.GetNext(pos);
		ASSERT( (pPackage != NULL) && pPackage->IsKindOf(RUNTIME_CLASS(CPackage)) );
		pPackage->AppendExtraPropertyPages(guidView, piUnkSel);
	}

	// once we've been called, we cannot be called again
	m_refInhibitExtraPages++;

	piUnkSel->Release();

	return TRUE;	// we at least tried
}

/* InhibitExtraPages
 *	Dissalow AppendExtraPages.  Ref-count to be sure.
 *	This can be used by owners of pages who don't want to allow any extra pages
 *	at all, by calling InhibitExtraPages(), and then not releasing it.
 *	Return whether or not extra pages are now inhibited.
 */
BOOL CSheetWnd::InhibitExtraPages(BOOL bInhibit /*=TRUE*/)
{
	if( bInhibit )
	{
		m_refInhibitExtraPages++;
		return TRUE;
	}

	m_refInhibitExtraPages = max(m_refInhibitExtraPages - 1, 0);

	// return whether or not inhibition is now off
	return m_refInhibitExtraPages > 0;
}

/* StartNewPageSet
 *	Marks all of the existing pages as belonging to a single set,
 *	and all subsequent pages as belonging to a new set.
 *	This marker is then used by MergePageSets() to tell the sets apart.
 *	Returns size of first page set.
 */
int CSheetWnd::StartNewPageSet(void)
{
	m_nFirstPageSet = m_listNewPages.GetCount();	// index of next page to be added
	return m_nFirstPageSet;
}

/* MergePageSets
 *	Merges two sets of pages together.  Afterwards, the only pages that will be 
 *	left will be those that occureded in both sets.
 *	Thus, to set up the pages for a 2-item multiple selection, add the pages of
 *	the first object (using AddPropertyPage()), then call StartNewPageSet(), 
 *	then add the pages of the second object, and call MergePageSets().
 *	To merge pages for more than 2 objects in a multiple selection, simply call
 *	StartNewPageSet(), add the pages, and call MergePageSets() for each object.
 *	Return the number of pages in common.
 */
int CSheetWnd::MergePageSets(void)
{
	if( (m_nFirstPageSet == 0) ||
		(m_nFirstPageSet >= m_listNewPages.GetCount()) )
	{
		// Either the first or second set is empty,
		// so there are no mutual pages.

		m_listNewPages.RemoveAll();
		m_nFirstPageSet = 0;
		return 0;	// no pages
	}

	POSITION posFirst = m_listNewPages.GetHeadPosition();
	POSITION posSecond = m_listNewPages.FindIndex(m_nFirstPageSet);
	ASSERT( (posFirst != NULL) && (posSecond != NULL) );

	// Loop through all of the pages in the first set.
	// If the page also exists in the second set, then remove it from that set
	// to shorten the loops.
	// If the page does not exist in the second set, remove it from the first 
	// set.
	// Finally, remove everything left from the second set.

	POSITION pos1 = posFirst;
	while( pos1 != posSecond )
	{
		POSITION pos1cur = pos1;
		C3dPropertyPage* pPage1 = (C3dPropertyPage*)m_listNewPages.GetNext(pos1);
		UINT idPage1 = (UINT)-1;
		if( pPage1->IsKindOf(RUNTIME_CLASS(CSlobPage)) )
			idPage1 = ((CSlobPage*)pPage1)->GetControlMap()->m_nCtlID;

		BOOL bMatch = FALSE;
		POSITION pos2 = posSecond;
		while( pos2 != NULL )
		{
			POSITION pos2cur = pos2;
			C3dPropertyPage* pPage2 = (C3dPropertyPage*)m_listNewPages.GetNext(pos2);

			// REVIEW(davidga): how do I really tell if pPage1 and pPage2 are 
			// the same dailog?  I'd like to use the IDD_, but those are only
			// in CSlobPages.
			UINT idPage2 = (UINT)-1;
			if( pPage2->IsKindOf(RUNTIME_CLASS(CSlobPage)) )
				idPage2 = ((CSlobPage*)pPage2)->GetControlMap()->m_nCtlID;

			if( (idPage1 != -1) && (idPage1 == idPage2) )
			{
				// These are both CSlobPages, and use the same dialog template.
				if( pos2cur != posSecond )
					m_listNewPages.RemoveAt(pos2cur);		// shorten list for speed
				bMatch = TRUE;
				break;
			}
			else if( (idPage1 == (UINT)-1) && (idPage2 == (UINT)-1) )
			{
				// both pages are COlePages.
				ASSERT( pPage1->IsKindOf(RUNTIME_CLASS(COlePage)) );
				ASSERT( pPage2->IsKindOf(RUNTIME_CLASS(COlePage)) );

				// In a multiple selection, all OLE pages will be removed.
				// This is accomplished by not setting bMatch to TRUE.

				break;
			}
		}

		if( !bMatch )
		{
			m_listNewPages.RemoveAt(pos1cur);
		}
	}

	// Remove everything from the second set
	while( posSecond != NULL )
	{
		POSITION pos2 = posSecond;
		m_listNewPages.GetNext(posSecond);
		m_listNewPages.RemoveAt(pos2);
	}

	m_nFirstPageSet = 0;
	return m_listNewPages.GetCount();	// return number of mutual pages
}

CSize CSheetWnd::CalcMaxPageSize(BOOL bNewPages /*=FALSE*/)
{
	// Calculate desired SheetWnd client area size.
	// Also store away the maximum page size, so the pages can expand during
	// InitializePage() if they want to.

	CSize sizeRet = m_noPage.GetPageSize();

	BOOL bFirst = TRUE;
	CObList* pList = bNewPages ? &m_listNewPages : &m_listCurPages;
	POSITION pos = pList->GetHeadPosition();
//	ASSERT( pos != NULL );
	while( pos != NULL )
	{
		C3dPropertyPage* pPage = (C3dPropertyPage*)pList->GetNext(pos);
		if( bFirst )
		{
			sizeRet = pPage->GetPageSize();
			bFirst = FALSE;
		}
		else
		{
			CSize size = pPage->GetPageSize();
			if( size.cx > sizeRet.cx )
				sizeRet.cx = size.cx;
			if( size.cy > sizeRet.cy )
				sizeRet.cy = size.cy;
		}
	}

	// if we're calculating the current set, update this member I found lying around
	if( !bNewPages )
		m_sizePageMax = sizeRet;

	return sizeRet;
}

void CSheetWnd::SetPushPin(BOOL fPushed /* = TRUE */)
{
	if(fPushed != m_toolBar.IsPinned())
		m_toolBar.SetPushPin(fPushed);		
}

BOOL CSheetWnd::IsPinned()
{
	return m_toolBar.IsPinned();
}

#if 0
//
// Globals for MLE sublassing in the property page
//

FARPROC glpfnOldSheetEditProc = NULL;
FARPROC glpfnNewSheetEditProc = NULL;

//
// SheetMleProc (C windowproc function)
//
// This overrides the default behaviors for WM_KEYDOWN and WM_CHAR to map the
// carriage return to a newline (which goes to the next line in the window,
// rather than pushing the default dialog button.
//

int FAR PASCAL EXPORT SheetMleProc(HWND hMLE, UINT wMessage, WPARAM wParam, LPARAM lParam)
{
    switch(wMessage)
    {
    case WM_KEYDOWN:	// make return work in MLE, not push the def button
    case WM_CHAR:
		if(wParam == 0x0d)
	    	wParam = 0x0A;
		break;
    }

    ASSERT(glpfnOldSheetEditProc != NULL);

    return (int) CallWindowProc((WNDPROC)glpfnOldSheetEditProc,
    	hMLE, wMessage, wParam, lParam);
}
#endif	// 0

//
// CSheet
//
