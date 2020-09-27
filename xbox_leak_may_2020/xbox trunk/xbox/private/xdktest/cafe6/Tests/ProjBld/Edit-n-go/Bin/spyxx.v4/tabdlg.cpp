// tabdlg.cpp : implementation file
//

//[byrond] Use the Spy++ headers
#include "stdafx.h"
#pragma hdrstop

//[byrond] Don't use 3D (there are some external dependencies below)
#define NOCTL3D

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif


CLastTabMap CTabbedDialog::m_DialogMap;

CLastTabMap::~CLastTabMap()
{
	POSITION pos = GetStartPosition();
	while (pos != (POSITION)NULL)
	{
		WORD wKey; CObject * pStr;
		GetNextAssoc(pos, wKey, pStr);
		delete (CString *)pStr;
	}	
}

////////////////////////////////////////////////////////////////////////////
// CDlgTab -- one page of a tabbed dialog

BEGIN_MESSAGE_MAP(CDlgTab, CDialog)
	//{{AFX_MSG_MAP(CDlgTab)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

IMPLEMENT_DYNAMIC(CDlgTab, CDialog)

CDlgTab::CDlgTab()
{
	m_nOrder = 0;
}

CDlgTab::CDlgTab(UINT nIDTemplate, UINT nIDCaption)
{
	m_strCaption.LoadString(nIDCaption);
#if _MFC_VER < 0x0400
	m_lpDialogTemplate = MAKEINTRESOURCE(nIDTemplate);
#else
	m_lpszTemplateName = MAKEINTRESOURCE(nIDTemplate);
#endif
	m_nIDHelp = LOWORD(nIDTemplate);

	m_nOrder = 0;
}

CDlgTab::CDlgTab(LPCSTR lpszTemplateName, UINT nIDCaption)
{
	m_strCaption.LoadString(nIDCaption);
#if _MFC_VER < 0x0400
	m_lpDialogTemplate = lpszTemplateName;
#else
	m_lpszTemplateName = lpszTemplateName;
#endif

	m_nOrder = 0;
}

void CDlgTab::OnOK()
{
	// Overide for OK processing, but do NOT call EndDialog!
	
	// NB. don't use SendMessage here
	m_pParentWnd->PostMessage(WM_COMMAND, IDOK);
}

void CDlgTab::OnCancel()
{
	// Overide for Cancel processing, but do NOT call EndDialog!
	
	// NB. don't use SendMessage here
	m_pParentWnd->PostMessage(WM_COMMAND, IDCANCEL);
}

void CDlgTab::CancelTab()
{
}

BOOL CDlgTab::ValidateTab()
{
	return TRUE;
}

void CDlgTab::CommitTab()
{
	ASSERT(GetSafeHwnd() != NULL);

	VERIFY( UpdateData(TRUE) );
}

BOOL CDlgTab::PreTranslateMessage(MSG* pMsg)
{
	if (((CTabbedDialog*)m_pParentWnd)->MessageStolenByTabbedDialog(pMsg))
		return TRUE;

	return CDialog::PreTranslateMessage(pMsg);
}

BOOL CDlgTab::Activate(CTabbedDialog* pParentWnd, CPoint position)
{
	m_pParentWnd = pParentWnd;
	
	if (m_hWnd == NULL)
	{
#ifdef MUNGE_DLG_FONTS
		CDialogTemplate dt;
#if _MFC_VER < 0x0400
		if (dt.Load(m_lpDialogTemplate))
#else
		if (dt.Load(m_lpszTemplateName))
#endif
			dt.SetStdFont();
		if (!CreateIndirect(dt.GetTemplate(), pParentWnd))
			return FALSE; // Create() failed...
#else
#if _MFC_VER < 0x0400
		if (!Create(m_lpDialogTemplate, pParentWnd))
#else
		if (!Create(m_lpszTemplateName, pParentWnd))
#endif
			return FALSE; // Create() failed...
#endif
		// Must be a child for obvious reasons, and must be disabled to prevent
		// it from taking the focus away from the tab area during initialization...	
		ASSERT((GetStyle() & (WS_DISABLED | WS_CHILD)) == (WS_DISABLED | WS_CHILD));
	}

	// DEBUG - Setting the window text allows our sniff tests to determine
	//         which DlgTab is currently active with a simple WM_GETTEXT
	//         message.  This line of code is otherwise unnecessary.
	SetWindowText(m_strCaption);

	SetWindowPos(&wndTop, position.x, position.y, 0, 0,
		SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOCOPYBITS);
	ShowWindow(SW_SHOWNOACTIVATE);
	EnableWindow(TRUE);

	return TRUE;
}

void CDlgTab::Deactivate(CTabbedDialog* pParentWnd)
{
	ASSERT(pParentWnd == m_pParentWnd);
}

#ifdef _DEBUG
void CDlgTab::EndDialog(int nID)
{
	// Do NOT call EndDialog for a tab!  Coordinate with the parent
	// for termination (you can post WM_COMMAND with IDOK or IDCANCEL
	// do handle those cases).
	
	ASSERT(FALSE);
}
#endif

BOOL IsTabNextFocus(CDialog* pDlg, UINT nCmd)
{
	if ((pDlg->SendMessage(WM_GETDLGCODE) &
		(DLGC_WANTALLKEYS | DLGC_WANTMESSAGE | DLGC_WANTTAB)) == 0)
	{
		CWnd* pCtl = CWnd::GetFocus();
		if (pDlg->IsChild(pCtl))
		{
			// Get top level child for controls with children, like combo.
			while (pCtl->GetParent() != pDlg)
			{
				pCtl = pCtl->GetParent();
				ASSERT_VALID(pCtl);
			}

			do
			{
				if ((pCtl = pCtl->GetWindow(nCmd)) == NULL)
					return TRUE;
			}
			while ((pCtl->GetStyle() & (WS_DISABLED | WS_TABSTOP)) != WS_TABSTOP);
		}
	}

	return FALSE;
}

// CTabRecord: an object representing one graphical tab
//
CTabRecord::CTabRecord(const TCHAR* szCaption, int nWidth, int nOrder, DWORD dw)
{
	m_strCaption = szCaption;
	m_nWidth = nWidth;
	m_nOrder = nOrder;
	m_dw = dw;

	m_rect.SetRectEmpty();
}


////////////////////////////////////////////////////////////////////////////
// CTabs -- implementation for a generic row of tabs along the top of dialog

CTabRow::CTabRow()
{
	m_rowRect.SetRectEmpty();
	m_scrollRect.SetRectEmpty();

	m_curTab = m_firstTab = 0;
	m_bHasFocus = FALSE;
	m_nScrollState = SCROLL_NULL;

	m_pWndOwner = NULL;
}

CTabRow::~CTabRow()
{
	for (int i = 0 ; i <= MaxTab() ; i++)
		delete m_tabs[i];
}

void CTabRow::ResetContent()
{
	for (int i = 0 ; i <= MaxTab() ; i++)
		delete m_tabs[i];

	m_curTab = 0;
	m_firstTab = 0;
	m_nScrollState = SCROLL_NULL;

	m_tabs.SetSize(0);
	m_pWndOwner->InvalidateRect(&m_rowRect);
}

void CTabRow::AddTab(CString& strCaption, CDC* pDC, int nOrder, DWORD dw)
{
	ASSERT(pDC);

	// Calculate the required width of the tab based on the text
	CFont* pOldFont = pDC->SelectObject(GetStdFont(font_Normal));

	CSize text = pDC->GetTextExtent(strCaption, strCaption.GetLength());
	int nWidth = text.cx + cxTabTextMargin * 2;

	pDC->SelectObject(pOldFont);

	CTabRecord* pTabRecord = new CTabRecord(strCaption, nWidth, nOrder, dw);
	for (int i = 0; ; i += 1)
	{
		if (i == m_tabs.GetSize())
		{
			m_tabs.Add(pTabRecord);
			break;
		}
		
		if (nOrder < ((CTabRecord*)m_tabs[i])->m_nOrder)
		{
			m_tabs.InsertAt(i, pTabRecord);
			break;
		}
		
		ASSERT(i < m_tabs.GetSize());
	}
			
	SetFirstTab(0);

	if (!m_rowRect.IsRectEmpty())
		m_pWndOwner->InvalidateRect(&m_rowRect);
}

void CTabRow::SetPos(CRect rect)
{
	// rect is in client coordinates of the owning window, of course
	m_rowRect = rect;
	m_scrollRect.SetRect(m_rowRect.right - cxScrollSize, m_rowRect.bottom - 1 - cyScrollSize,
	                     m_rowRect.right, m_rowRect.bottom - 1);
	SetFirstTab(m_firstTab);	// recalc all tab positions
	ScrollIntoView(m_curTab);	// make sure current selection still in view
}

int CTabRow::DesiredHeight(CDC* pDC)
{
	CFont* pOldFont = pDC->SelectObject(GetStdFont(font_NormalBold));

	// Our desired height is the height of a tall character plus the
	// required margin space, plus the amount by which selection
	// increases the height of a tab.
	int height = pDC->GetTextExtent("M", 1).cy + cyTabTextMargin * 2 + cySelectedTab * 2;

	pDC->SelectObject(pOldFont);
	return height;
}

int CTabRow::SetActiveTab(int nTab)
{
	if (nTab < 0 || nTab > MaxTab())
		return -1;
	if (nTab == m_curTab)
		return nTab;

	InvalidateTab(m_curTab);
	InvalidateTab(nTab);

	m_curTab = nTab;

	ScrollIntoView(nTab);

	return nTab;
}

int CTabRow::SetActiveTab(const CString& str)
{
	for (int i = 0 ; i <= MaxTab() ; i++)
	{
		if (str == GetTabString(i))
			return SetActiveTab(i);
	}

	return -1;
}

void CTabRow::PutFocus(BOOL bTabsHaveFocus)
{
	if (m_bHasFocus == bTabsHaveFocus)
		return;

	m_bHasFocus = bTabsHaveFocus;
	if ((m_curTab != -1) && (m_curTab <= MaxTab()))
		InvalidateTab(m_curTab);
}

void CTabRow::DrawTab(CDC* pDC, int nTab, BOOL bCurTab)
{
	CRect rc = GetTab(nTab)->m_rect;
	int  nWidth = GetTab(nTab)->m_nWidth;
	BOOL bClipped = GetTab(nTab)->m_bClipped;

	if (bCurTab)
		rc.InflateRect(cxSelectedTab, cySelectedTab);

	CPen pen;

	// Draw white line along left and top of tab
	COLORREF cr = GetSysColor(COLOR_BTNHIGHLIGHT);
	if (!pen.CreatePen(PS_SOLID, 1, cr))
		return;

	CPen* pOldPen = pDC->SelectObject(&pen);

	pDC->MoveTo(rc.left, rc.bottom - 1);
	pDC->LineTo(rc.left, rc.top + 2);
	pDC->LineTo(rc.left + 2, rc.top);
	pDC->LineTo(rc.right - 1, rc.top);

	pDC->SelectObject(pOldPen);
	pen.DeleteObject();

	// if the tab following this one is the selected tab, or if it is
	// clipped by the right edge, don't draw the right border lines.
	if ((m_curTab != nTab + 1) || !IsTabVisible(m_curTab))
	{
		cr = GetSysColor(COLOR_BTNSHADOW);
		if (!pen.CreatePen(PS_SOLID, 1, cr))
			return;

		pOldPen = pDC->SelectObject(&pen);

		// Depending on whether this tab is clipped or not, we draw either
		// the "torn" dark gray edge, or else the standard gray+black line
		// on the right-side border.
		if (!bClipped)
		{
		//	pDC->MoveTo(rc.right - 1, rc.top);		already there
			pDC->LineTo(rc.right - 1, rc.bottom);

			pDC->SelectObject(pOldPen);
			pen.DeleteObject();

			// Now do the second (black) line down right side
			cr = GetSysColor(COLOR_BTNTEXT);
			if (!pen.CreatePen(PS_SOLID, 1, cr))
				return;

			pOldPen = pDC->SelectObject(&pen);

			pDC->MoveTo(rc.right, rc.top + 2);
			pDC->LineTo(rc.right, rc.bottom);
		}
		else
		{
			// draw dark gray "torn" edge for a clipped tab
			for (int i = rc.top ; i < rc.bottom ; i += 3)
			{
				// This nifty (but obscure-looking) equation will draw
				// a jagged-edged line.
				int j = ((6 - (i - rc.top) % 12) / 3) % 2;
				pDC->MoveTo(rc.right + j, i);
				pDC->LineTo(rc.right + j, min(i + 3, rc.bottom));
			}
		}

		pDC->SelectObject(pOldPen);
		pen.DeleteObject();
	}

	// Finally, draw the tab's text
	CFont* pOldFont = pDC->SelectObject(GetStdFont(font_Normal));
	CString strCaption = GetTabString(nTab);

	CSize text = pDC->GetTextExtent(strCaption,
		strCaption.GetLength());
	pDC->ExtTextOut(rc.left + nWidth / 2 - text.cx / 2,
		rc.top + rc.Height() / 2 - text.cy / 2, ETO_CLIPPED, &rc,
		strCaption, strCaption.GetLength(), NULL);

	pDC->SelectObject(pOldFont);
}

void CTabRow::Draw(CDC* pDC)
{
	// Draw all the tabs that are currently within view
	for (int i = MaxTab() ; i >= 0 ; i--)
		if (IsTabVisible(i))
			DrawTab(pDC, i, i == m_curTab);

	// Draw the line underneath all the tabs
	CPen pen;
	COLORREF cr = GetSysColor(COLOR_BTNHIGHLIGHT);
	if (pen.CreatePen(PS_SOLID, 1, cr))
	{
		CRect rc(0, 0, 0, 0);
		if (m_curTab <= MaxTab())
			rc = GetTab(m_curTab)->m_rect;
		CPen* pOldPen = pDC->SelectObject(&pen);

		pDC->MoveTo(m_rowRect.left, m_rowRect.bottom - 1);
		if (!rc.IsRectNull())
		{
			// this leaves a gap in the line if the currently selected
			// tab is within view.
			pDC->LineTo(rc.left - cxSelectedTab, m_rowRect.bottom - 1);
			pDC->MoveTo(rc.right + cxSelectedTab + 1, m_rowRect.bottom - 1);
		}
		pDC->LineTo(m_rowRect.right, m_rowRect.bottom - 1);

		pDC->SelectObject(pOldPen);
		pen.DeleteObject();
	}

	if (CanScroll())
		DrawScrollers(pDC);

	if (m_bHasFocus && (m_pWndOwner->GetFocus() == m_pWndOwner))
		DrawFocusRect(pDC);
}

void CTabRow::DrawFocusRect(CDC* pDC)
{
	if (MaxTab() < 0)
		return;

	CRect rc;
	rc = GetTab(m_curTab)->m_rect;

	pDC->DrawFocusRect(&rc);
}

void CTabRow::DrawScrollers(CDC* pDC)
{
	int idb;

	// Choose bitmap depending on scroll state
	if (IsTabVisible(0, FALSE))
		idb = IDB_SCROLL_LD;
	else if (IsTabVisible(MaxTab(), FALSE))
		idb = IDB_SCROLL_RD;
	else
		idb = IDB_SCROLL;

	if (!m_bScrollPause)
	{
		if (m_nScrollState == SCROLL_LEFT)
			idb = IDB_SCROLL_L;
		else if (m_nScrollState == SCROLL_RIGHT)
			idb = IDB_SCROLL_R;
	}

	// now load and draw it
	CBitmap bm;
	bm.LoadBitmap(idb);

	CDC dcTemp;
	dcTemp.CreateCompatibleDC(pDC);
	CBitmap* pOldBitmap = dcTemp.SelectObject(&bm);

	pDC->BitBlt(m_scrollRect.left, m_scrollRect.top, m_scrollRect.Width(),
	            m_scrollRect.Height(), &dcTemp, 0, 0, SRCCOPY);

	dcTemp.SelectObject(pOldBitmap);
}

void CTabRow::DrawPageRect(CDC* pDC, const CRect& rectPage)
{
	CRect rect;
	rect.left = min(m_rowRect.left - 1, rectPage.left);
	rect.top = m_rowRect.bottom - 2;
	rect.right = max(m_rowRect.right, rectPage.right);
	rect.bottom = rectPage.bottom;

	// black line along right and bottom side
	COLORREF cr = GetSysColor(COLOR_WINDOWFRAME);
	pDC->FillSolidRect(rect.right - 1, rect.top, 1, rect.Height(), cr);
	pDC->FillSolidRect(rect.left, rect.bottom - 1, rect.Width() - 1, 1, cr);

	rect.InflateRect(-1, -1);
	// draw white line along left of page, and top where not covered by
	// the tabs.
	cr = GetSysColor(COLOR_BTNHIGHLIGHT);

	if (rect.left < m_rowRect.left)
	{
		pDC->FillSolidRect(rect.left, rect.top,
			m_rowRect.left - rect.left, 1, cr);
	}

	if (rect.right > m_rowRect.right)
	{
		pDC->FillSolidRect(m_rowRect.right, rect.top,
			rect.right - m_rowRect.right - 1, 1, cr);
	}

	pDC->FillSolidRect(rect.left, rect.top + 1,	1, rect.Height() - 2, cr);

	// dark gray line along right and bottom side
	cr = GetSysColor(COLOR_BTNSHADOW);
	pDC->FillSolidRect(rect.right - 1, rect.top, 1, rect.Height(), cr);
	pDC->FillSolidRect(rect.left, rect.bottom - 1, rect.Width() - 1, 1, cr);

}

void CTabRow::SetFirstTab(int nTab)
{
	// This function recalcs the positions of all the tabs, assuming the
	// specified tab is the first (leftmost) visible tab.

	if (nTab < 0 || MaxTab() < nTab)
		return;

	int x = m_rowRect.left + cxSelectedTab;

	// everything before the first tab is not visible
	for (int i = 0 ; i < nTab ; i++)
		GetTab(i)->m_rect.SetRectEmpty();

	// calculate locations for all other tabs
	for (i = nTab ; i <= MaxTab() ; i++)
	{
		GetTab(i)->m_rect.SetRect(x, m_rowRect.top + cySelectedTab,
		                          x + GetTab(i)->m_nWidth, m_rowRect.bottom - cySelectedTab);
		x += GetTab(i)->m_nWidth + cxSelectedTab;
		GetTab(i)->m_bClipped = FALSE;
	}

	// do they all fit?
	x = m_rowRect.right - (nTab == 0 ? 0 : (cxScrollMargin + cxScrollSize));
	i = MaxTab();
	while ((i >= 0) && (GetTab(i)->m_rect.right > x))
	{
		x = m_rowRect.right - (cxScrollMargin + cxScrollSize);
		if (GetTab(i)->m_rect.left < x)
		{
			GetTab(i)->m_bClipped = TRUE;
			GetTab(i)->m_rect.right = x;
		}
		else
			GetTab(i--)->m_rect.SetRectEmpty();
	}

	m_firstTab = nTab;
}

BOOL CTabRow::IsTabVisible(int nTab, BOOL bPartials)
{
	if (GetTab(nTab)->m_rect.IsRectNull())
		return FALSE;

	return bPartials || !GetTab(nTab)->m_bClipped;
}

void CTabRow::Scroll(int nDirection)
{
	ASSERT(nDirection == SCROLL_LEFT || nDirection == SCROLL_RIGHT);
	ASSERT(CanScroll());

	BOOL bRepaintScrollers = ((nDirection == SCROLL_LEFT) && IsTabVisible(MaxTab(), FALSE))
			|| ((nDirection == SCROLL_RIGHT) && IsTabVisible(0, FALSE));

	switch(nDirection)
	{
		case SCROLL_LEFT:
			if (m_firstTab == 0)
				return;

			SetFirstTab(m_firstTab - 1);
			break;

		case SCROLL_RIGHT:
			if (IsTabVisible(MaxTab(), FALSE))
				return;

			SetFirstTab(m_firstTab + 1);
			break;
	}

	// repaint all tabs, and (if their enable state has changed) the scroll
	// buttons as well.
	CRect rc = m_rowRect;
	if (!bRepaintScrollers)
		rc.right = m_scrollRect.left - 1;
	m_pWndOwner->InvalidateRect(&rc);
}

void CTabRow::ScrollIntoView(int nTab)
{
	if (nTab < 0 || nTab > MaxTab() || IsTabVisible(nTab, FALSE) ||
			m_rowRect.IsRectEmpty())
		return;

	// do we need to scroll left, or right?
	int increment = (nTab > m_firstTab) ? 1 : -1;

	while (!IsTabVisible(nTab, FALSE))
		SetFirstTab(m_firstTab + increment);

	m_pWndOwner->InvalidateRect(&m_rowRect);
}

BOOL CTabRow::CanScroll()
{
	// if either the first or the last tab is not visible, it's scrollable
	return MaxTab() >= 0 && (!IsTabVisible(0, FALSE) || !IsTabVisible(MaxTab(), FALSE));
}

void CTabRow::OnMouseMove(CPoint pt)
{
	ASSERT((m_nScrollState == SCROLL_LEFT) || (m_nScrollState == SCROLL_RIGHT));

	int nNewState = TabFromPoint(pt);
	BOOL bPause = !(nNewState == m_nScrollState);

	if (bPause == m_bScrollPause)
		return;

	if (bPause)
		m_pWndOwner->KillTimer(TIMER_ID);
	else
	{
		VERIFY( m_pWndOwner->SetTimer(TIMER_ID, TIMER_DELAY, NULL) == TIMER_ID );
		Scroll(m_nScrollState);
	}

	m_bScrollPause = bPause;
	InvalidateTab(m_nScrollState);
}

void CTabRow::OnLButtonUp(CPoint pt)
{
	Capture(SCROLL_NULL);
}

void CTabRow::OnTimer()
{
	ASSERT((m_nScrollState == SCROLL_LEFT) || (m_nScrollState == SCROLL_RIGHT));
	ASSERT(!m_bScrollPause);

	Scroll(m_nScrollState);
}

void CTabRow::Capture(int nDirection)
{
	ASSERT((m_nScrollState == SCROLL_LEFT) || (m_nScrollState == SCROLL_RIGHT) ||
	       (m_nScrollState == SCROLL_NULL));

	switch(nDirection)
	{
		case SCROLL_LEFT:
		case SCROLL_RIGHT:
			m_pWndOwner->SetCapture();
			VERIFY( m_pWndOwner->SetTimer(TIMER_ID, TIMER_DELAY, NULL) == TIMER_ID );
			InvalidateTab(nDirection);
			break;

		case SCROLL_NULL:
			::ReleaseCapture();
			m_pWndOwner->KillTimer(TIMER_ID);
			InvalidateTab(m_nScrollState);
			break;
	}

	m_nScrollState = nDirection;
	m_bScrollPause = FALSE;
}

void CTabRow::InvalidateTab(int nTab, BOOL bInflate)
{
	CRect rc;
	
	switch (nTab)
	{
		case SCROLL_LEFT:
			rc = m_scrollRect;
			rc.right -= (rc.Width() / 2 - 1);
			break;

		case SCROLL_RIGHT:
			rc = m_scrollRect;
			rc.left += (rc.Width() / 2 - 1);
			break;

		default:
			rc = GetTab(nTab)->m_rect;
			if (bInflate)
			{
				rc.left -= cxSelectedTab;
				rc.right += cxSelectedTab + 1;
				rc.top = m_rowRect.top;
				rc.bottom = m_rowRect.bottom;
			}
			if (GetTab(nTab)->m_bClipped)
				rc.right += 1;
	}

	ASSERT(m_pWndOwner);
	m_pWndOwner->InvalidateRect(&rc, nTab >= 0);
}

int CTabRow::TabFromPoint(CPoint pt)
{
	// are we on the scroll buttons?
	if (CanScroll() && m_scrollRect.PtInRect(pt))
	{
		if (pt.x < m_scrollRect.left + (m_scrollRect.Width() / 2))
			return IsTabVisible(0, FALSE) ? -1 : SCROLL_LEFT;
		else
			return IsTabVisible(MaxTab(), FALSE) ? -1 : SCROLL_RIGHT;
	}

	// are we on a tab?
	for (int i = 0 ; i <= MaxTab() ; i++)
		if (GetTab(i)->m_rect.PtInRect(pt))
			return i;

	return -1;
}


/////////////////////////////////////////////////////////////////////////////
// CTabbedDialog -- a tabbed "dialog" (really a popup-window)

IMPLEMENT_DYNAMIC(CTabbedDialog, CWnd)

CTabbedDialog::CTabbedDialog(UINT nIDCaption, CWnd* pParent /*=NULL*/,
                             UINT iSelectTab /*=-1*/, COMMIT_MODEL commitModel /*=commitOnOk*/)
{
	m_pParentWnd = pParent == NULL ? AfxGetApp()->m_pMainWnd : pParent;

	nCaptionID = nIDCaption;
	m_nTabCur = (iSelectTab == -1) ? 0 : iSelectTab;

	m_tabRow.SetParent(this);
	m_tabRow.PutFocus(TRUE);
	m_hFocusWnd = 0;
	m_strCaption.LoadString( nIDCaption );
	m_commitModel = commitModel;
	m_iDefBtnIndex = 0;		// i.e., the first one (usually 'OK')
	m_buttonSize = CSize(0, 0);
}

BOOL CTabbedDialog::PreTranslateMessage(MSG* pMsg)
{
	CWnd* pWnd;

	// If the standard tabbed-dialog pretranslate code knows how to handle
	// this message, let it.
	if (MessageStolenByTabbedDialog(pMsg))
		return TRUE;

	switch (pMsg->message)
	{
	case WM_KEYDOWN:

		switch ((char) pMsg->wParam)
		{
			case VK_ESCAPE:
				PostMessage(WM_COMMAND, IDCANCEL);
				return TRUE;

			case VK_RETURN:
				pWnd = GetFocus();
				if ((pWnd == NULL) || !IsButton(pWnd->GetSafeHwnd()))
					pWnd = GetButtonFromIndex(0);
				PostMessage(WM_COMMAND, pWnd->GetDlgCtrlID());
				return TRUE;

			default:
				::TranslateMessage( pMsg );
				::DispatchMessage( pMsg );
				return TRUE;
		}
	}

	return CWnd::PreTranslateMessage(pMsg);
}

void CTabbedDialog::BuildTabItemList(CObList& list, CMapWordToOb& map, CWnd* pWndCur)
{
	int i;
	CString str;
	char cAccel = 0;

	if (pWndCur == NULL)
	{
		ASSERT(list.IsEmpty());
		ASSERT(map.IsEmpty());
		pWndCur = GetWindow(GW_CHILD);
	}

	while (pWndCur != NULL)
	{
		if (pWndCur->IsKindOf( RUNTIME_CLASS(CDialog) ))
		{
			if (pWndCur->IsWindowVisible())
			{
				// if it's a tabbed dialog, include an entry for the tab row
				if (pWndCur->IsKindOf( RUNTIME_CLASS(CDlgTab) ))
					list.AddTail(this);

				// recursively process all dialogs to get a list of tab stops
				CWnd* pWndChild = pWndCur->GetWindow(GW_CHILD);
				ASSERT(pWndChild);
				BuildTabItemList(list, map, pWndChild);
			}
		}
		else
		{
			DWORD dwStyle = pWndCur->GetStyle();
			if ((dwStyle & (WS_DISABLED | WS_VISIBLE)) == WS_VISIBLE)
			{
				pWndCur->GetWindowText(str);

DoubleAmpersand:
				if ((i = str.Find('&')) != -1)
				{
					ASSERT(str.GetLength() > i+1);	// & can't be last char
					if (str[i + 1] == '&')
					{
						str = str.Right(str.GetLength() - (i + 2));
						goto DoubleAmpersand;
					}
					cAccel = str[i + 1];
				}

				if (dwStyle & WS_TABSTOP)
				{
					list.AddTail(pWndCur);
					if (cAccel != 0)
					{
						map.SetAt(toupper(cAccel), pWndCur);
						cAccel = 0;
					}
				}
			}
		}

		pWndCur = pWndCur->GetWindow(GW_HWNDNEXT);
	}
}

CWnd* CTabbedDialog::FindControl(CWnd* pWndFocus, CObList& list, FC fc)
{
	POSITION pos = list.Find(pWndFocus);
	while (pos == NULL)
	{
		// possibly the current focus window is a child of some window
		// in the list?
		pWndFocus = pWndFocus->GetParent();
		if (pWndFocus == NULL)
		{
			ASSERT(fc == FC_THIS);	// Never happens otherwise?
			return NULL;
		}

		pos = list.Find(pWndFocus);
	}

	// Handle wrapping off the top or bottom of the dialog.
	switch (fc)
	{
		case FC_NEXT:
			list.GetNext(pos);
			if (pos == NULL)
				return (CWnd*) list.GetHead();
			break;

		case FC_PREVIOUS:
			list.GetPrev(pos);
			if (pos == NULL)
				return (CWnd*) list.GetTail();
			break;

		case FC_THIS:
			if (pos == NULL)
				return NULL;
			break;

#ifdef _DEBUG
		default:
			ASSERT(FALSE);
			break;
#endif
	}

	ASSERT(pos != NULL);
	return (CWnd*) list.GetAt(pos);
}

void CTabbedDialog::SetFocusToControl(CWnd* pControl, CObList* pList, BOOL bSetSel)
{
	ASSERT(pControl);
	CWnd* pOldDefault = NULL;
	CObList list;
	CMapWordToOb map;

	if (pList == NULL)
	{
		BuildTabItemList(list, map);
		pList = &list;
	}

	// If it's not in the list, it can't take the focus.  This catches user
	// clicks on subdialogs.
	CWnd* pListItem = FindControl(pControl, *pList, FC_THIS);
	if ((pListItem == NULL) || ((pListItem != pControl) && (pListItem == this)))
		return;

	// Figure out who currently has the default button style
	POSITION pos = pList->GetHeadPosition();
	while (pos != NULL)
	{
		pOldDefault = (CWnd*) pList->GetNext(pos);
		if (pOldDefault->SendMessage(WM_GETDLGCODE) & DLGC_DEFPUSHBUTTON)
			break;
	}

	// Figure out who ought to have the default button style
	CWnd* pNewDefault = (CWnd *)NULL;
	if (m_iDefBtnIndex != -1)
	{
		pNewDefault = GetButtonFromIndex(m_iDefBtnIndex);
		if (pControl->SendMessage(WM_GETDLGCODE) & (DLGC_DEFPUSHBUTTON | DLGC_UNDEFPUSHBUTTON))
			pNewDefault = pControl;
		ASSERT(pNewDefault);
		if (pNewDefault->m_hWnd == NULL)
			return;
	}

	// Now set the focus...
	m_tabRow.PutFocus(pControl == this);
	pControl->SetFocus();

	// if this control handles SETSEL messages, select the entire contents
	if (bSetSel && pControl->SendMessage(WM_GETDLGCODE) & DLGC_HASSETSEL)
		pControl->SendMessage(EM_SETSEL, 0, -1);

	// no new default button if new equals old
	if (pNewDefault == pOldDefault)
		return;

	// ...and, if the default button needs to change, change it.
	if (pOldDefault != NULL)
	{
		::SetWindowLong(pOldDefault->GetSafeHwnd(), GWL_STYLE,
			GetWindowLong(pOldDefault->GetSafeHwnd(), GWL_STYLE) & ~BS_DEFPUSHBUTTON);
		pOldDefault->Invalidate();
	}

	// ...set a new default button?
	if (pNewDefault != NULL)
	{
		::SetWindowLong(pNewDefault->GetSafeHwnd(), GWL_STYLE,
			GetWindowLong(pNewDefault->GetSafeHwnd(), GWL_STYLE) | BS_DEFPUSHBUTTON);
		pNewDefault->Invalidate();
	}
}

void CTabbedDialog::HandleTab(BOOL bShift, BOOL bControl)
{
	if (bControl)
	{
		SetFocusToControl(this);
		SendMessage(WM_KEYDOWN, bShift ? VK_LEFT : VK_RIGHT);
	}
	else
	{
		CObList list;
		CMapWordToOb map;

		BuildTabItemList(list, map);

		CWnd* pWndNext = FindControl(GetFocus(), list, bShift ? FC_PREVIOUS : FC_NEXT);
		ASSERT(pWndNext != NULL);

		if (pWndNext != NULL)
			SetFocusToControl(pWndNext, &list);
	}
}

BOOL CTabbedDialog::HandleAcceleratorKey(char ch)
{
	CObList list;
	CMapWordToOb map;

	BuildTabItemList(list, map);

	CObject* pObject;
	if (!map.Lookup(ch, pObject))
		return FALSE;

	CWnd* pWnd = (CWnd*)pObject;
	ASSERT(pWnd->IsKindOf( RUNTIME_CLASS(CWnd) ));

	SetFocusToControl(pWnd, &list);

	// Automatically push buttons and twiddle checkboxes
	if (pWnd->SendMessage(WM_GETDLGCODE) & DLGC_BUTTON)
	{
		CButton* pButton = (CButton*)pWnd;
		DWORD dwStyle = pButton->GetStyle();

		// [marklam] Dolphin 9492,9488 need to set check and then send button clicked message.
		// Random.
		if ((dwStyle & BS_AUTOCHECKBOX) == BS_AUTOCHECKBOX)
		{
			pButton->SetCheck(1 - pButton->GetCheck());
			pButton->GetParent()->SendMessage(WM_COMMAND, (WPARAM)(pWnd->GetDlgCtrlID() | (BN_CLICKED<<16)), (LPARAM)pWnd->GetSafeHwnd());
		}
		else if ((dwStyle & BS_AUTO3STATE) == BS_AUTO3STATE)
		{
			pButton->SetCheck((pButton->GetCheck() + 1) % 3);
			pButton->GetParent()->SendMessage(WM_COMMAND, (WPARAM)(pWnd->GetDlgCtrlID() | (BN_CLICKED<<16)), (LPARAM)pWnd->GetSafeHwnd());
		}
		else
		{									
			pButton->SendMessage(WM_LBUTTONDOWN, (WPARAM)0, (LPARAM)0);
			pButton->SendMessage(WM_LBUTTONUP, (WPARAM)0, (LPARAM)0);
		}
	}

	return TRUE;
}

BOOL CTabbedDialog::HandleTabRowKey(char ch)
{
	// Starting from current position + 1, cycle through all the dlgtabs
	// looking for one whose caption starts with the requested character.
	CDlgTab* pTab;
	int i = m_nTabCur + 1;

	while (i != m_nTabCur)
	{
		if ((pTab = GetTab(i++)) == NULL)
		{
			i = 0;
			continue;
		}

		if (pTab->m_strCaption.GetLength() < 1)
		{
			ASSERT(FALSE);	// shouldn't have captionless dlgtabs
			continue;
		}

		if (pTab->m_strCaption[0] == ch)
		{
			SelectTab(i - 1);
			return TRUE;
		}
	}

	return FALSE;
}


// MessageStolenByTabbedDialog
//
// This function should be called during PreTranslateMessage by the DlgTab
// or by any subdialog contained within the DlgTab.
//
// If the function returns TRUE, the tabbed dialog has processed the
// message and no further action is required by the DlgTab.
//
BOOL CTabbedDialog::MessageStolenByTabbedDialog(MSG* pMsg)
{
	LRESULT dlgCode;

	switch (pMsg->message)
	{
		case WM_SYSKEYDOWN:

			if ((char) pMsg->wParam == VK_MENU)
				break;

			// Unless there is a current focus item that wants all its own
			// keys, process accelerators here.
			dlgCode = (GetFocus() == NULL) ? (LRESULT)0 : GetFocus()->SendMessage(WM_GETDLGCODE);

			if (! (dlgCode & DLGC_WANTALLKEYS) &&
			    HandleAcceleratorKey((char) pMsg->wParam))
			{
				return TRUE;
			}

			break;

		case WM_KEYDOWN:

			dlgCode = (GetFocus() == NULL) ? (LRESULT)0 : GetFocus()->SendMessage(WM_GETDLGCODE);

			switch(pMsg->wParam)
			{
				case VK_TAB:
					// Note that we do tab handling here even if the control
					// with the current focus has DLGC_WANTALLKEYS set.
					HandleTab(GetKeyState(VK_SHIFT) < 0, GetKeyState(VK_CONTROL) < 0);
					return TRUE;

				case VK_F1:
					if (dlgCode & DLGC_WANTALLKEYS)
						break;

					OnHelp();
					return TRUE;

				default:
					// Hitting keys with the focus on the tab row should go
					// through normal key processing.
					if (m_tabRow.HasFocus())
						break;

					// If the current focus window doesn't respond to character
					// input, then hitting a key should be handled as an
					// accelerator in accordance with Windows standards.
					if (! (dlgCode & (DLGC_WANTCHARS | DLGC_WANTALLKEYS))
						&& HandleAcceleratorKey((char) pMsg->wParam))
					{
						return TRUE;
					}

					break;
			}
			break;
	}

	return FALSE;
}

int CTabbedDialog::DoModal()
{
	int nResult = IDABORT;

	// cannot call DoModal on a dialog already constructed as modeless
	ASSERT(m_hWnd == NULL);

	//[patbr] Remove external dependencies!
	//PreModalWindow();

	// allow OLE servers to disable themselves
	AfxGetApp()->EnableModeless(FALSE);

	// disable parent before creating or MFC will disable us.
	m_pParentWnd->EnableWindow(FALSE);

	if (Create())
	{	
		ASSERT(IsWindowEnabled());	// should not be disabled to start!
		// EnableWindow();
		SetActiveWindow();

		m_nID = -1;
		while (m_hWnd != NULL && AfxGetApp()->PumpMessage() && m_nID == -1)
			;
	
		nResult = m_nID;
	}

	// allow OLE servers to enable themselves
	AfxGetApp()->EnableModeless(TRUE);

	// enable top level parent window again
	m_pParentWnd->EnableWindow(TRUE);

	//[patbr] Remove external dependencies!
	//PostModalWindow();

	return nResult;
}

BOOL CTabbedDialog::Create()
{
	if (!CreateEx(WS_EX_DLGMODALFRAME,
		AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW, LoadCursor(NULL, IDC_ARROW), (HBRUSH)(COLOR_BTNFACE + 1)),
		m_strCaption, WS_SYSMENU | WS_POPUP | WS_CAPTION | WS_VISIBLE | WS_CAPTION | DS_MODALFRAME,
		50, 50, 400, 200, m_pParentWnd->m_hWnd, NULL))
	{
		return FALSE;
	}

	SetActiveWindow();
	return TRUE;
}

////////////////////////////////////////////////////////////////////////////

BOOL CTabbedDialog::ClearAllTabs()
{
	if (m_tabs.GetSize() != 0)
	{
		// de-select the current
		SelectTab(-1);

		// tab still active?
		if (m_nTabCur != -1)
			return FALSE;	// clear failed

		// delete our existing tabs
		int nTabs = m_tabs.GetSize();
		for (int i = 0; i < nTabs; i++)
			delete m_tabs[i];
		m_tabs.SetSize(0);

		// reset the tab row to reflect this clear
		m_tabRow.ResetContent();
	}

	return TRUE;	// success
}

////////////////////////////////////////////////////////////////////////////

void CTabbedDialog::AddTab(CDlgTab* pDlgTab)
{
	// FUTURE this uses the screen DC rather than the tabbed dialog DC
	// in the normal case, because the tabbed dialog's DC will often be
	// NULL at this point.  This was an oversight in the code, not
	// discovered until shortly before ship -- we should probably calc
	// tab sizes when the dialog is created, like MFC does.
	CDC* pDC = CDC::FromHandle(::GetDC(m_hWnd));

	for (int i = 0; ; i += 1)
	{
		if (i == m_tabs.GetSize())
		{
			m_tabs.Add(pDlgTab);
			break;
		}
		
		if (pDlgTab->m_nOrder < ((CDlgTab*)m_tabs[i])->m_nOrder)
		{
			m_tabs.InsertAt(i, pDlgTab);
			break;
		}
		
		ASSERT(i < m_tabs.GetSize());
	}
	
	m_tabRow.AddTab(pDlgTab->m_strCaption, pDC, pDlgTab->m_nOrder);

	::ReleaseDC(m_hWnd, pDC->m_hDC);
}

BEGIN_MESSAGE_MAP(CTabbedDialog, CWnd)
	//{{AFX_MSG_MAP(CTabbedDialog)
	ON_WM_PAINT()
	ON_WM_NCCREATE()
	ON_WM_CREATE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_TIMER()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_ACTIVATE()
	ON_WM_KEYDOWN()
	ON_WM_CLOSE()
	ON_WM_SIZE()
	ON_WM_PARENTNOTIFY()
	ON_WM_KEYDOWN()
	ON_WM_CTLCOLOR()
	ON_WM_SYSCOLORCHANGE()
	//}}AFX_MSG_MAP
	ON_COMMAND(IDOK, OnOK)
	ON_COMMAND(IDCANCEL, OnCancel)
	ON_COMMAND(ID_HELP, OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTabbedDialog message handlers

BOOL CTabbedDialog::OnNcCreate(LPCREATESTRUCT lpcs)
{
	if (theApp.IsChicago())
		ModifyStyle(0, 0x00000004L);	// DS_3DLOOK

	if (!CWnd::OnNcCreate(lpcs))
		return FALSE;

	SubclassDlg3d();	// add CTL3D effects, if loaded
	return TRUE;
}

int CTabbedDialog::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// Fix-up the system menu so this looks like a dialog box
	CMenu* pSysMenu = GetSystemMenu(FALSE);
	ASSERT(pSysMenu != NULL);
	pSysMenu->DeleteMenu(7, MF_BYPOSITION);
	pSysMenu->DeleteMenu(5, MF_BYPOSITION);
	pSysMenu->DeleteMenu(SC_SIZE, MF_BYCOMMAND);
	pSysMenu->DeleteMenu(SC_TASKLIST, MF_BYCOMMAND);
	pSysMenu->DeleteMenu(SC_MINIMIZE, MF_BYCOMMAND);
	pSysMenu->DeleteMenu(SC_MAXIMIZE, MF_BYCOMMAND);
	pSysMenu->DeleteMenu(SC_RESTORE, MF_BYCOMMAND);
		
	// do we have a tab ready to select?
	if (m_nTabCur == -1)
	{
		CString strTabCaption;
		if (m_DialogMap.LookupTab( nCaptionID, strTabCaption))
			for (int i = 0 ; i <= m_tabRow.MaxTab() ; i++)
				if (m_tabRow.GetTabString(i) == strTabCaption)
				{
					m_nTabCur = i; break;
				}

		// did we find a remembered tab?
		if (m_nTabCur == -1)
			m_nTabCur = 0;
	}

	if (ActivateTab(m_nTabCur))
		m_tabRow.SetActiveTab(m_nTabCur);
	else
		m_nTabCur = -1;	// no tab selected

	// send notification of this initial tab
	OnSelectTab(m_nTabCur);

	return 0;
}

void CTabbedDialog::OnPaint()
{
	CPaintDC dc(this);
	dc.SetBkMode(TRANSPARENT);

	// the tabs know how to draw themselves
	m_tabRow.Draw(&dc);

	CRect rectPage, margins;
	GetClientRect(rectPage);
	GetMargins(margins);

	rectPage.left += margins.left;
	rectPage.right -= margins.right;
	rectPage.bottom -= margins.bottom;

	m_tabRow.DrawPageRect(&dc, rectPage);
}

void CTabbedDialog::SelectTab(int nTab)
{
	CDlgTab *pTab;

	ASSERT(nTab == -1 || (nTab >= 0 && nTab < m_tabs.GetSize()));

	if (nTab == m_nTabCur)
		return;

	int nPrevTab = m_nTabCur;
	if (m_nTabCur != -1)
	{
		pTab = GetTab(m_nTabCur);
		if (!pTab->ValidateTab())
		{
			TRACE("Tab %s refused deactivation!\n",
				(const char*)GetTab(m_nTabCur)->m_strCaption);
			return;
		}
		pTab->Deactivate(this);
	}

	if (nTab != -1 && !ActivateTab(nTab))
	{
		TRACE("Could not activate tab %s!\n",
			(const char*)GetTab(nTab)->m_strCaption);
		return;
	}

	if (nPrevTab != -1)
		pTab->ShowWindow(SW_HIDE);

	m_nTabCur = nTab;

	if (nTab != -1)
	{
		m_tabRow.SetActiveTab(nTab);

		// send notification of this tab change
		OnSelectTab(nTab);
	}
}

void CTabbedDialog::GetMargins(CRect& rect)
{
	if (m_buttonSize.cx == 0)
	{
		// Calculate the proper size of the buttons, based on average character
		// width and height of the font.  If it's a variable pitch font,
		// calculate the average pitch (code copied from editor.cpp)
		//
		LONG cxChar;
		CDC* pDC = GetDC();
		CFont* pOldFont;
		if (theApp.IsChicago())
		 	pOldFont = pDC->SelectObject(GetStdFont(font_Normal));
		else
		 	pOldFont = pDC->SelectObject(GetStdFont(font_NormalBold));

	    TEXTMETRIC tm;
	    pDC->GetTextMetrics(&tm);

	    if (tm.tmPitchAndFamily & 0x01)
	    {
	        CHAR szAveCharWidth[52];        // array to hold A-Z and a-z

	        for (int i = 0; i < 26; i++)
	        {
	            szAveCharWidth[i] = (CHAR)(i + 'a');
	            szAveCharWidth[i + 26] = (CHAR)(i + 'A');
	        }

			CSize sizeExtent = pDC->GetTextExtent(szAveCharWidth, 52);
			cxChar = ((sizeExtent.cx / 26) + 1) / 2;
	    }
	    else
	    {
	        cxChar = tm.tmAveCharWidth;
	    }

		pDC->SelectObject(pOldFont);
		ReleaseDC(pDC);

		int duWidth = duButtonWidth;
		if (_getmbcp() != 0)
			duWidth += 10;	// Need a little extra space for japanese.

		m_buttonSize.cx = cxChar * duWidth / 4;
		m_buttonSize.cy = tm.tmHeight * duButtonHeight / 8;
		m_cxBetweenButtons = cxChar * duBetweenButtons / 4;
	}

	// Override to specify left, right, top and bottom margins around the tabs
	rect.SetRect(cxTabMargin, cyTabMargin, cxTabMargin, cyTabMargin);

	// If we have any buttons, leave space for them at the bottom
	if (GetButtonFromIndex(0) != NULL)
		rect.bottom += cyBottomMargin + m_buttonSize.cy;
}

void CTabbedDialog::OnLButtonDown(UINT nFlags, CPoint point)
{
	int nTab = m_tabRow.TabFromPoint(point);
	switch(nTab)
	{
		default:
			ASSERT(nTab >= 0);
			SetFocusToControl(this);
			SelectTab(nTab);
			break;

		case CTabRow::SCROLL_LEFT:
		case CTabRow::SCROLL_RIGHT:
			m_tabRow.Scroll(nTab);
			m_tabRow.Capture(nTab);
			break;

		case -1:
			break;
	}
}

void CTabbedDialog::OnTimer(UINT nTimerID)
{
	if (m_tabRow.HasCapture())
	{
		ASSERT(nTimerID == CTabRow::TIMER_ID);
		m_tabRow.OnTimer();
	}
	else
		CWnd::OnTimer(nTimerID);
}

void CTabbedDialog::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_tabRow.HasCapture())
		m_tabRow.OnMouseMove(point);
	else
		CWnd::OnMouseMove(nFlags, point);
}

void CTabbedDialog::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_tabRow.HasCapture())
		m_tabRow.OnLButtonUp(point);
	else
		CWnd::OnLButtonUp(nFlags, point);
}

void CTabbedDialog::DrawFocusRect()
{
	if (!m_tabRow.HasFocus())
		return;		

	CDC* pDC = GetDC();
	m_tabRow.DrawFocusRect(pDC);
	ReleaseDC(pDC);
}

void CTabbedDialog::OnSetFocus(CWnd* pOldWnd)
{
	if (m_tabRow.HasFocus())
		DrawFocusRect();
	else
		(void) ActivateTab(m_nTabCur);
}

void CTabbedDialog::OnKillFocus(CWnd* pNewWnd)
{
	if (IsWindowVisible() && m_tabRow.HasFocus())
		DrawFocusRect();
}

void CTabbedDialog::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	// Changed this routine to use m_hFocusWnd instead of m_pFocusWnd
	if (nState == WA_INACTIVE)
	{
		m_hFocusWnd = ::GetFocus();
		m_tabRow.PutFocus(m_hFocusWnd == GetSafeHwnd());
	}
	else
	{
		CWnd* pWnd = CWnd::FromHandle(m_hFocusWnd);

		// Focus is either in the CTabRow or belongs to a control.
		if (m_nTabCur != -1)
		{
			if ((pWnd != NULL) && !m_tabRow.HasFocus())
			{
				SetFocusToControl(pWnd);
			}
			else
			{
				m_tabRow.InvalidateTab(m_nTabCur);
				SetFocusToControl(this);
			}
		}

		return;
	}
	
	CWnd::OnActivate(nState, pWndOther, bMinimized);
}

void CTabbedDialog::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	int nTab = m_nTabCur;

	switch (nChar)
	{
		case VK_ESCAPE:
			PostMessage(WM_COMMAND, IDCANCEL);
			break;

		case VK_LEFT:
			nTab -= 1;
			break;

		case VK_RIGHT:
			nTab += 1;
			break;

#ifdef _DEBUG
		case VK_RETURN:
		case VK_TAB:
			ASSERT(FALSE);
			break;
#endif // _DEBUG

		default:
			HandleTabRowKey(nChar);
			return;
	}

	if (nTab != m_nTabCur)
	{
		// if we have other arrow keys pending, coalesce
		MSG msg;
		while (::PeekMessage(&msg, GetSafeHwnd(), WM_KEYDOWN, WM_KEYDOWN, PM_NOREMOVE)
		       && (msg.wParam == VK_LEFT || msg.wParam == VK_RIGHT))
		{
			// remove the message and increment tab as necessary
			::PeekMessage(&msg, GetSafeHwnd(), WM_KEYDOWN, WM_KEYDOWN, PM_REMOVE);
			nTab += (msg.wParam == VK_RIGHT ? 1 : -1);
		}

		// make sure new tab is in legal range
		while (nTab < 0)
			nTab += max(m_tabs.GetSize(), 1);
		nTab %= m_tabs.GetSize();

		SelectTab(nTab);
	}
}

void CTabbedDialog::EndDialog(int nEndID)
{
	if (m_nTabCur != -1)
	{
		CString strCaption = m_tabRow.GetTabString(m_nTabCur);
		m_DialogMap.RememberTab(nCaptionID, strCaption);
	}

	m_nID = nEndID;
	m_pParentWnd->EnableWindow(TRUE);
	SetWindowPos(NULL, 0, 0, 0, 0,
  		SWP_HIDEWINDOW | SWP_NOACTIVATE | SWP_NOMOVE |
  		SWP_NOSIZE | SWP_NOZORDER);

	// Destroy all the tabs. We do it here, so that the toolbar
	// customiz dialog can reenable the MDIClient before we reset the
	// active window.
	int nTabs = m_tabs.GetSize();
	for (int i = 0; i < nTabs; i++)
	{
		delete m_tabs[i];	// We have delete responsibility.
		m_tabs[i] = NULL;
	}

	m_pParentWnd->SetActiveWindow();	// SetFocus messes up dock workers.
	DestroyWindow();
}

void CTabbedDialog::OnOK()
{
	if (m_nTabCur != -1)
	{
		CDlgTab* pTab = GetTab(m_nTabCur);
		if (!pTab->ValidateTab())
		{
			TRACE("Tab %s refused deactivation!\n",
				(const char*)GetTab(m_nTabCur)->m_strCaption);
			return;
		}

		pTab->Deactivate(this);
	}

	// At this point, all tabs must be valid.  We commit all changes.
	for (int nTab = 0 ; nTab < m_tabs.GetSize() ; nTab++)
		if (GetTab(nTab)->GetSafeHwnd() != NULL)
			GetTab(nTab)->CommitTab();

	EndDialog(IDOK);
}

void CTabbedDialog::OnCancel()
{
	if (m_nTabCur != -1)
	{
		CDlgTab* pTab = GetTab(m_nTabCur);
		pTab->Deactivate(this);
	}

	// Cancel changes on all tabs.
	for (int nTab = 0 ; nTab < m_tabs.GetSize() ; nTab++)
		if (GetTab(nTab)->GetSafeHwnd() != NULL)
			GetTab(nTab)->CancelTab();

	EndDialog(IDCANCEL);
}

void CTabbedDialog::OnHelp()
{
	// If there is a current popup, give it the first chance to handle
	// this help request.
	CWnd* pWnd = GetTopLevelParent();
	HWND hWnd = ::GetLastActivePopup(pWnd->GetSafeHwnd());

	if (::SendMessage(hWnd, WM_COMMANDHELP, 0, 0))
		return;

	// Otherwise, let the current DlgTab process the help request.
	if (m_nTabCur != -1)
		GetTab(m_nTabCur)->OnCommandHelp(0, 0);
}

void CTabbedDialog::OnClose()
{
	// just like cancel
	OnCancel();
}

void CTabbedDialog::OnSize(UINT nType, int cx, int cy)
{
	// We adjust the size a bit in here, so we must block recursion
	static BOOL bInHere;
	if (bInHere)
		return;

	CRect subject; // "page" dialog size
	CRect margins;  // "margin" sizes
	GetTabSize(subject);
	GetMargins(margins);

	CClientDC dc(this);
	int nTabHeight = m_tabRow.DesiredHeight(&dc);
	int nWidth = subject.Width() + margins.left + margins.right + 3;
	int nHeight = subject.Height() + margins.top + margins.bottom + nTabHeight + 3;

	// calculate our required width and height to accom. this tab size
	CRect rectClient(0, 0, nWidth, nHeight);
	CRect dialog = rectClient;
	AdjustWindowRectEx(dialog, GetStyle(), FALSE, GetExStyle());

	bInHere = TRUE;
	SetWindowPos(NULL, 0, 0, dialog.Width(), dialog.Height(),
		SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
	bInHere = FALSE;

	// center the tabbed dialog in the main window
    CenterWindow(AfxGetApp()->m_pMainWnd);

	// set up the size/position of the tab row
	CPoint pt = GetDlgTabPos();
	m_tabRow.SetPos(CRect(pt.x - 1, pt.y - nTabHeight, pt.x + subject.Width() + 2, pt.y));

	// create our buttons and move them into place
	CreateButtons();
	int nBottomBorder = dialog.bottom - rectClient.bottom;
	MoveButtons(margins.left,
		dialog.bottom - (margins.bottom + nBottomBorder) / 2);
}

void CTabbedDialog::GetTabSize(CRect & rect)
{
	// *must* have a tab to do the initial sizing!
	CDlgTab * pTab = GetTab(m_nTabCur);
	ASSERT(pTab != (CDlgTab *)NULL);
	pTab->GetWindowRect(rect);
}

void CTabbedDialog::CreateButtons()
{
	// Create the OK, Cancel, and Help buttons (or Close and Help, if the
	// dialog prefers that commit model).
	//
	// This function also creates the buttons if necessary.  While it might
	// seem preferable to create the buttons in CTabbedDialog::Create, that
	// actually doesn't work, since this function will be called in the
	// middle of the tabbed dialog's creation.

	if (m_btnOk.GetSafeHwnd() != NULL)
		return;

	CString str;
	CRect rcEmpty;
	rcEmpty.SetRectEmpty();

	str.LoadString((m_commitModel == commitOnOk) ? IDS_TABDLG_OK : IDS_TABDLG_CLOSE);
	m_btnOk.Create(str, WS_CHILD | WS_TABSTOP | BS_DEFPUSHBUTTON, rcEmpty, this, IDOK);

	str.LoadString(IDS_TABDLG_CANCEL);
	m_btnCancel.Create(str, WS_CHILD | WS_TABSTOP, rcEmpty, this, IDCANCEL);

	str.LoadString(IDS_TABDLG_HELP);
	m_btnHelp.Create(str, WS_CHILD | WS_TABSTOP, rcEmpty, this, ID_HELP);
}

void CTabbedDialog::MoveButtons(int nLeft, int nCenter)
{
	// Position the buttons along the bottom of the dialog
	// centered in the bottom margin.
	int y = nCenter - m_buttonSize.cy / 2;
	int x = nLeft;
	int index = 0;

	CButton* pButton;
	CButton* pButtonPrev = NULL;

	// Cycle through all the buttons, positioning them along the bottom
	for (pButton = GetButtonFromIndex(index) ; pButton != NULL ;
	     pButton = GetButtonFromIndex(++index) )
	{
		if (theApp.IsChicago())
			pButton->SetFont(GetStdFont(font_Normal));
		else
			pButton->SetFont(GetStdFont(font_NormalBold));

		pButton->SetWindowPos(pButtonPrev == NULL ? &wndBottom : pButtonPrev,
		                      x, y, m_buttonSize.cx, m_buttonSize.cy, SWP_NOACTIVATE);
		pButton->ShowWindow(SW_SHOW);

		x += m_buttonSize.cx + m_cxBetweenButtons;
		pButtonPrev = pButton;
	}
}

void CTabbedDialog::SetDefButtonIndex(int index)
{
	CButton* pbtnDef;
	
	if (m_iDefBtnIndex != -1)
	{
		pbtnDef = GetButtonFromIndex(m_iDefBtnIndex);
		// remove the default style bit
		::SetWindowLong(pbtnDef->GetSafeHwnd(), GWL_STYLE,
						::GetWindowLong(pbtnDef->GetSafeHwnd(), GWL_STYLE) & ~BS_DEFPUSHBUTTON);
		pbtnDef->Invalidate();
	}

	if (index != -1)
	{
		pbtnDef = GetButtonFromIndex(index);
		// add the default style bit
		::SetWindowLong(pbtnDef->GetSafeHwnd(), GWL_STYLE,
						::GetWindowLong(pbtnDef->GetSafeHwnd(), GWL_STYLE) | BS_DEFPUSHBUTTON);
		pbtnDef->Invalidate();
	}
	
	// remember this
	m_iDefBtnIndex = index;
}

BOOL CTabbedDialog::IsButton(HWND hWnd)
{
	int i = 0;
	CButton* pButton;

	while ((pButton = GetButtonFromIndex(i++)) != NULL)
		if (pButton->GetSafeHwnd() == hWnd)
			return TRUE;

	return FALSE;
}

CButton* CTabbedDialog::GetButtonFromIndex(int index)
{
	// Default is either OK/Cancel/Help or Close/Help, depending on
	// commit model.  Derived classes may override this, of course.
	if (m_commitModel != commitOnOk && (index > 0))
		index++;

	switch (index)
	{
		case 0:
			return &m_btnOk;

		case 1:
			return &m_btnCancel;

		case 2:
			return &m_btnHelp;

		default:
			return NULL;
	}
}

BOOL CTabbedDialog::ActivateTab(int nTab)
{
	CDlgTab *pTab;

	// activating no tab (or a tab that doesn't exist) is a no-op
	if (nTab == -1 || (pTab = GetTab(nTab)) == (CDlgTab *)NULL)
		return FALSE;

	if (pTab->Activate(this, GetDlgTabPos()))
	{
		SetFocusToControl(this);
		return TRUE;
	}

	return FALSE;
}

CPoint CTabbedDialog::GetDlgTabPos()
{
	CRect margins;
	GetMargins(margins);

	int nTabHeight;
	CRect rect = m_tabRow.GetRect();
	if (rect.IsRectEmpty())
	{
		CClientDC dc(this);
		nTabHeight = m_tabRow.DesiredHeight(&dc);
	}
	else
	{
		nTabHeight = rect.Height();
	}

	return CPoint(margins.left + 1, margins.top + 1 + nTabHeight);
}

void CTabbedDialog::OnParentNotify(UINT message, LPARAM lParam)
{
	CWnd::OnParentNotify(message, lParam);

	// We have to move the focus and default button state to the
	// proper button whenever focus changes in the dialog.
	if (message == WM_LBUTTONDOWN)
	{
		CPoint pt = CPoint(LOWORD(lParam), HIWORD(lParam));
		ClientToScreen(&pt);
		CWnd* pControl = WindowFromPoint(pt);
		if (pControl != NULL)
			SetFocusToControl(pControl, NULL, FALSE);
	}
}
