// widgetsb.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "utilctrl.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// defines

#define cxBitmap 8
#define cxTabOverhead 8
#define cxButton 17
#define cxBorder 1
#define cyBorder 1
#define cxSplitter 5
#define msRepeatDelay 250
#define msRepeatRate 100
#define cxMinScrollBarWidth 48

#define DIVIDER_INACTIVE         (0)
#define DIVIDER_INACTIVE_LEFT1   (2)
#define DIVIDER_INACTIVE_RIGHT1  (1)
#define DIVIDER_ACTIVE_LEFT      (6)
#define DIVIDER_ACTIVE_RIGHT		 (4)
#define DIVIDER_ACTIVE_LEFT1		 (5)
#define DIVIDER_ACTIVE_RIGHT1		 (3)

/////////////////////////////////////////////////////////////////////////////
// CWidgetScrollBar static members

CBitmap CWidgetScrollBar::c_bmpArrows;
CBitmap CWidgetScrollBar::c_bmpDividers;
int CWidgetScrollBar::c_cyBitmap = 0;
CFont /*CWidgetScrollBar::*/c_widgetFontNormal;
CFont /*CWidgetScrollBar::*/c_widgetFontBold;
int /*CWidgetScrollBar::*/c_cyHeightPrev = 0;

/////////////////////////////////////////////////////////////////////////////
// CWidgetScrollBar

CWidgetScrollBar::CWidgetScrollBar(BOOL bAlwaysShowScrollBar /*=FALSE*/)
{
	m_bShowScrollBar = TRUE;
	m_bAlwaysShowScrollBar = bAlwaysShowScrollBar;
	m_nSplitterPref = 0; // reset to half of window width on first resize
	m_nWidgetWidth = 2 * cxButton - 2 * cxBorder;
	m_captureZone = none;
	m_bButtonIn = TRUE;
	m_xScrollPos = 0;
	m_cxScrollExtent = cxBitmap;
	m_nActiveTab = -1;	// so first tab added can be activated properly
}

CWidgetScrollBar::~CWidgetScrollBar()
{
	ResetTabs();
}

void CWidgetScrollBar::ResetTabs()
{
	m_cxScrollExtent = cxBitmap;
	m_xScrollPos = 0;
	m_nActiveTab = -1;
	while (!m_tabs.IsEmpty())
		delete (CWidgetTab*) m_tabs.RemoveHead();

	if (m_hWnd != NULL)
		InvalidateRect(CRect(2 * cxButton - 2 * cxBorder, 0, m_nWidgetWidth, 100), FALSE);
}

BOOL CWidgetScrollBar::PreCreateWindow(CREATESTRUCT& cs)
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	if (c_bmpArrows.m_hObject == NULL)
		c_bmpArrows.LoadBitmap(IDB_WIDGETBAR_ARROWS);
//	if (c_bmpDividers.m_hObject == NULL)
//		OnSysColorChange();		// will load dividers using sys colors

	cs.lpszClass = AfxRegisterWndClass(0);
	return TRUE;
}

void CWidgetScrollBar::AddTab(const char* szCaption, DWORD dwUser, BOOL bAdjustSplitter)
{
	CClientDC dc(this); // okay if 'this' doesn't have an hWnd yet...
	CFont* pOldFont = dc.SelectObject(&c_widgetFontBold);
	CWidgetTab* pTab = new CWidgetTab;
	pTab->m_str = szCaption;
	pTab->m_dwUser = dwUser;
	pTab->m_cxWidth = cxTabOverhead + (c_cyBitmap + 1) / 2 + dc.GetTextExtent(pTab->m_str, pTab->m_str.GetLength()).cx;
	m_tabs.AddTail(pTab);
	dc.SelectObject(pOldFont);
	m_cxScrollExtent += pTab->m_cxWidth;

	if (bAdjustSplitter)
	{
		// add cxSplitter here because it's immediately subtracted in SetSplitterPos()
		CRect rect;
		GetClientRect(rect);
		SetSplitterPos(rect.right - cxBorder - 4 * GetSystemMetrics(SM_CXVSCROLL) + cxSplitter);
		m_nSplitterPref = m_nWidgetWidth;
	}
}

void CWidgetScrollBar::AddTabAt(int nPosition, const char* szCaption, DWORD dwUser, BOOL bAdjustSplitter)
{
	CClientDC dc(this); // okay if 'this' doesn't have an hWnd yet...
	CFont* pOldFont = dc.SelectObject(&c_widgetFontBold);
	CWidgetTab* pTab = new CWidgetTab;
	pTab->m_str = szCaption;
	pTab->m_dwUser = dwUser;
	pTab->m_cxWidth = cxTabOverhead + (c_cyBitmap + 1) / 2 + dc.GetTextExtent(pTab->m_str, pTab->m_str.GetLength()).cx;

	// ADD TAB AT SPECIFIED POSITION!
	POSITION pos = m_tabs.FindIndex(nPosition);
	if (pos == NULL)
		m_tabs.AddTail(pTab);
	else
		m_tabs.InsertBefore(pos, pTab);

	dc.SelectObject(pOldFont);
	m_cxScrollExtent += pTab->m_cxWidth;

	if (bAdjustSplitter)
	{
		// add cxSplitter here because it's immediately subtracted in SetSplitterPos()
		CRect rect;
		GetClientRect(rect);
		SetSplitterPos(rect.right - cxBorder - 4 * GetSystemMetrics(SM_CXVSCROLL) + cxSplitter);
		m_nSplitterPref = m_nWidgetWidth;
	}
}

void CWidgetScrollBar::RemoveTab(int nTab)
{
	POSITION pos = m_tabs.FindIndex(nTab);
	ASSERT(pos != NULL);
	CWidgetTab* pTab = (CWidgetTab*)m_tabs.GetAt(pos);
	ASSERT(pTab != NULL);

	m_tabs.RemoveAt(pos);
	m_cxScrollExtent -= pTab->m_cxWidth;
	delete pTab;

	if (m_tabs.IsEmpty())
	{
		m_nActiveTab = -1;
	}
	else if (m_nActiveTab > nTab)
	{
		int nCurTab = m_nActiveTab - 1;
		m_nActiveTab = -1;
		ActivateTab(nCurTab);
	}
	else if (m_nActiveTab == nTab)
	{
		int nLastTab = m_tabs.GetCount() - 1;
		m_nActiveTab = -1;
		ActivateTab(nTab <= nLastTab ? nTab : nLastTab);
	}

	Invalidate(FALSE);	// force repaint of widget bar
}

void CWidgetScrollBar::ChangeTabCaption(int nTab, const char* szNewCaption)
{
	POSITION pos = m_tabs.FindIndex(nTab);
	ASSERT(pos != NULL);
	CWidgetTab* pTab = (CWidgetTab*)m_tabs.GetAt(pos);
	ASSERT(pTab != NULL);

	CClientDC dc(this); // okay if 'this' doesn't have an hWnd yet...
	CFont* pOldFont = dc.SelectObject(&c_widgetFontBold);

	int cxOldWidth = pTab->m_cxWidth;

	pTab->m_str = szNewCaption;
	pTab->m_cxWidth = cxTabOverhead + (c_cyBitmap + 1) / 2 + dc.GetTextExtent(pTab->m_str, pTab->m_str.GetLength()).cx;
	dc.SelectObject(pOldFont);
	m_cxScrollExtent += pTab->m_cxWidth - cxOldWidth;

	Invalidate(FALSE);	// force repaint of widget bar
}


BEGIN_MESSAGE_MAP(CWidgetScrollBar, CWnd)
	//{{AFX_MSG_MAP(CWidgetScrollBar)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_WM_CREATE()
	ON_WM_SYSCOLORCHANGE()
	ON_WM_WININICHANGE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWidgetScrollBar message handlers

CWidgetScrollBar::HIT_ZONE CWidgetScrollBar::HitTest(CPoint point)
{
	if (point.x < cxButton)
		return left;

	if (point.x < cxButton + cxButton)
		return right;

	if (m_bShowScrollBar && point.x > m_nWidgetWidth)
		return splitter;

	int x = point.x - 2 * cxButton - 2 * cxBorder + m_xScrollPos;
	POSITION pos = m_tabs.GetHeadPosition();
	int nTab = 0;
	while (pos != NULL)
	{
		CWidgetTab* pTab = (CWidgetTab*)m_tabs.GetNext(pos);
		if (x < pTab->m_cxWidth)
			return (HIT_ZONE)(firstTab + nTab);
		x -= pTab->m_cxWidth;
		nTab += 1;
	}

	return none;
}


void CWidgetScrollBar::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_captureZone = HitTest(point);

	switch (m_captureZone)
	{
	case none:
		return;

	case left:
	case right:
		Scroll(m_captureZone == left ? -cxButton : cxButton);
		SetTimer(0, msRepeatDelay, NULL);
		m_bButtonIn = TRUE;
		InvalidateRect(CRect(0, 0, cxButton * 2 - cxBorder, 100), FALSE); // redraw buttons
		break;

	case splitter:
		break;

	default:
		// must be in a tab...
		ActivateTab(m_captureZone - firstTab);
		break;
	}

	SetCapture();
}


UINT WSB_CHANGETAB = RegisterMessage("WSB_CHANGETAB");

void CWidgetScrollBar::ActivateTab(int nTab)
{
	if (nTab >= m_tabs.GetCount())
		nTab = m_tabs.GetCount() - 1;

	if (nTab != m_nActiveTab)
	{
		CRect rect;

		if (m_nActiveTab != -1)
		{
			GetTabRect(m_nActiveTab, rect);
			InvalidateRect(rect, FALSE);
		}

		m_nActiveTab = nTab;

		GetTabRect(m_nActiveTab, rect);
		InvalidateRect(rect, FALSE);

		if (rect.left < 2 * cxButton - cxBorder)
			Scroll(rect.left - (2 * cxButton - cxBorder));
		else if (rect.right > m_nWidgetWidth)
			Scroll(rect.right - m_nWidgetWidth);

		UpdateWindow();

		POSITION pos  = m_tabs.GetHeadPosition();
		CWidgetTab* pTab = NULL;
		for (int i = 0; i < nTab + 1; i += 1)
		{
			pTab = (CWidgetTab*)m_tabs.GetNext(pos);
			if (i == nTab)
			{
				// set window text of widget scroll bar so QA
				// can check if correct tab is activated...
				SetWindowText(pTab->m_str);
			}
		}
		ASSERT(pTab != NULL);

		GetParent()->SendMessage(WSB_CHANGETAB, nTab, pTab->m_dwUser);
	}
}

void CWidgetScrollBar::GetTabRect(int nTab, CRect& rect)
{
	ASSERT(nTab >= 0 && nTab < m_tabs.GetCount());

	GetClientRect(rect);
	rect.InflateRect(-cxBorder, -cyBorder);

	rect.left += 2 * cxButton - 2 * cxBorder;
	POSITION pos  = m_tabs.GetHeadPosition();
	for (int i = 0; i < nTab; i += 1)
	{
		CWidgetTab* pTab = (CWidgetTab*)m_tabs.GetNext(pos);
		rect.left += pTab->m_cxWidth;
	}

	ASSERT(pos != NULL);
	CWidgetTab* pTab = (CWidgetTab*)m_tabs.GetNext(pos);
	rect.right = rect.left + pTab->m_cxWidth + cxTabOverhead + (c_cyBitmap + 1) / 2;
	rect.OffsetRect(-m_xScrollPos, 0);
	rect.InflateRect(cxBorder, cyBorder);
}

void CWidgetScrollBar::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (GetCapture() != this)
		return;

	ReleaseCapture();
	KillTimer(0);
	m_captureZone = none;
	m_bButtonIn = FALSE;
	InvalidateRect(CRect(0, 0, cxButton * 2 - cxBorder, 100), FALSE); // redraw buttons
}

void CWidgetScrollBar::OnMouseMove(UINT nFlags, CPoint point)
{
	if (GetCapture() == NULL)
	{
		m_captureZone = none;

		HCURSOR hCursor;
		switch (HitTest(point))
		{
		default:
			hCursor = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
			break;

		case splitter:
			hCursor = AfxLoadCursor(AFX_IDC_HSPLITBAR);
			break;
		}

		SetCursor(hCursor);
	}
	else
	{
		switch (m_captureZone)
		{
		default:
			break;

		case left:
		case right:
			{
				CRect rect;
				GetClientRect(rect);
				rect.InflateRect(-cxBorder, -cyBorder);

				if (m_captureZone == left)
				{
					rect.left = rect.left - cxBorder;
					rect.right = rect.left + cxButton;
				}
				else
				{
					rect.left = rect.left + cxButton - 2 * cxBorder;
					rect.right = rect.left + cxButton;
				}

				if (rect.PtInRect(point))
				{
					if (!m_bButtonIn)
					{
						// Moved back into the button area
						m_bButtonIn = TRUE;
						InvalidateRect(rect, FALSE);
						SetTimer(0, msRepeatRate, NULL);
					}
				}
				else
				{
					if (m_bButtonIn)
					{
						// Moved out of the button area
						m_bButtonIn = FALSE;
						KillTimer(0);
						InvalidateRect(rect, FALSE);
					}
				}
			}
			break;

		case splitter:
			SetSplitterPos(point.x);
			m_nSplitterPref = m_nWidgetWidth;
			break;
		}
	}
}

void CWidgetScrollBar::SetSplitterPos(int xPos)
{
	xPos -= cxSplitter;

	CRect rect;
	GetClientRect(rect);
	rect.InflateRect(-cxBorder, -cyBorder);

	if (xPos < 2 * (cxButton - cxBorder))
		xPos = 2 * (cxButton - cxBorder);
	if (m_bShowScrollBar && xPos > rect.right - cxSplitter - cxMinScrollBarWidth)
		xPos = rect.right - cxSplitter - cxMinScrollBarWidth;

	if (xPos > m_nWidgetWidth)
		InvalidateRect(CRect(m_nWidgetWidth, 0, xPos + cxSplitter, rect.bottom));
	else
		InvalidateRect(CRect(xPos, 0, xPos + cxSplitter, rect.bottom));
	
	if (m_nWidgetWidth == xPos)
		return;

	m_nWidgetWidth = xPos;

	if (m_xScrollPos > 0 && xPos > 2 * (cxButton - cxBorder) + m_cxScrollExtent - m_xScrollPos)
	{
		// If the user enlarges the tab area and the tabs are scrolled, scroll
		// back so more of the tabs are visible
		Scroll(xPos - (2 * (cxButton - cxBorder) + (m_cxScrollExtent - m_xScrollPos)));
	}

	RecalcLayout();
	UpdateWindow();
}

void CWidgetScrollBar::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
#ifdef _WIN32
	GetParent()->SendMessage(WM_HSCROLL, MAKEWPARAM(nSBCode, nPos), (LPARAM)pScrollBar->m_hWnd);
#else
	GetParent()->SendMessage(WM_HSCROLL, nSBCode, MAKELPARAM(nPos, pScrollBar->m_hWnd));
#endif
	
	CWnd::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CWidgetScrollBar::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	CWnd::OnVScroll(nSBCode, nPos, pScrollBar);
}

void DrawButton(CDC* pDC, CDC* pButtonDC, int nButton, const CRect& rect, BOOL bIn, BOOL bDisabled)
{
	if (bDisabled)
		bIn = FALSE;

	CBrush* pbrBorder = GetSysBrush(COLOR_BTNTEXT);
	CBrush* pbrHighlight = GetSysBrush(COLOR_BTNHIGHLIGHT);
	CBrush* pbrShadow = GetSysBrush(COLOR_BTNSHADOW);
	CBrush* pbrFace = GetSysBrush(COLOR_BTNFACE);

	CBrush* pOldBrush = pDC->SelectObject(pbrBorder);
	pDC->PatBlt(rect.left, rect.top, rect.Width(), cyBorder, PATCOPY);
	pDC->PatBlt(rect.left, rect.top + cyBorder, cxBorder, rect.Height() - 2 * cyBorder, PATCOPY);
	pDC->PatBlt(rect.right - cxBorder, rect.top + cyBorder, cxBorder, rect.Height() - 2 * cyBorder, PATCOPY);
	pDC->PatBlt(rect.left, rect.bottom - cyBorder, rect.Width(), cyBorder, PATCOPY);

	pDC->SelectObject(bIn ? pbrShadow : pbrHighlight);
	pDC->PatBlt(rect.left + cxBorder, rect.top + cyBorder, rect.Width() - (bIn ? 2 : 3) * cxBorder, cyBorder, PATCOPY);
	pDC->PatBlt(rect.left + cxBorder, rect.top + cyBorder * 2, cxBorder, rect.Height() - (bIn ? 3 : 4) * cyBorder, PATCOPY);

	if (!bIn)
	{
		pDC->SelectObject(pbrShadow);
		pDC->PatBlt(rect.right - 2 * cxBorder, rect.top + cyBorder, cxBorder, rect.Height() - cyBorder * 4, PATCOPY);
		pDC->PatBlt(rect.right - 3 * cxBorder, rect.top + 2 * cyBorder, cxBorder, rect.Height() - cyBorder * 5, PATCOPY);
		pDC->PatBlt(rect.left + 2 * cxBorder, rect.bottom - 3 * cyBorder, rect.Width() - 3 * cxBorder, cyBorder, PATCOPY);
		pDC->PatBlt(rect.left + cxBorder, rect.bottom - 2 * cyBorder, rect.Width() - 2 * cxBorder, cyBorder, PATCOPY);
	}
	
	pDC->SelectObject(pbrFace);
	pDC->PatBlt(rect.left + 2 * cxBorder, rect.top + 2 * cyBorder, 
		rect.Width() - ((bIn ? 3 : 5) * cxBorder), rect.Height() - ((bIn ? 3 : 5) * cyBorder), PATCOPY);

	pDC->SelectObject(pOldBrush);

	// determine size of glyph
	int yGlyph = 4 + bIn;
	int xGlyph = 6 + bIn;
	int cyGlyph = max(min(9, rect.Height() - 8), 3);
	if( (yGlyph + cyGlyph) >= rect.Height() )
		yGlyph = max(0, (rect.Height() - cyGlyph) / 2 + 1);
	int cxGlyph = (cyGlyph + 1) / 2;	// 5x9, 4x7or8, 3x5or6, or 2x3or4
	cyGlyph = cxGlyph * 2 - 1;			// 5x9, 4x7, 3x5, or 2x3

	// Draw the glyph...
	pDC->SetTextColor(bDisabled ? GetSysColor(COLOR_BTNSHADOW) : 
		GetSysColor(COLOR_BTNTEXT));
	pDC->SetBkColor(GetSysColor(COLOR_BTNFACE));
	pDC->BitBlt(rect.left + xGlyph, rect.top + yGlyph, cxGlyph, cyGlyph, 
		pButtonDC, (nButton == 1) ? (10 - cxGlyph) : 0, (9 - cyGlyph) / 2, SRCCOPY);
}

/* DrawDivider
 *	Draws one of the 7 divider glyphs
 */
enum
{
	DIVIDER_X_I,	// left-side of first tab on left, which is inacitve
	DIVIDER_I_X,	// right-side of last tab on right, which is inacitve
	DIVIDER_X_A,	// left-side of first tab on left, which is acitve
	DIVIDER_A_X,	// right-side of last tab on right, which is acitve
	DIVIDER_I_A,	// divider between active tab on right, and inactive on left
	DIVIDER_A_I,	// divider between active tab on left, and inactive on right
	DIVIDER_I_I,	// divider between two inactive tabs
};

#define DIVIDER_IMG_WIDTH		17
#define DIVIDER_IMG_HEIGHT		23

void CWidgetScrollBar::DrawDivider(CDC* pDC, int nImage, const CPoint& pt)
{
	CDC dcMem;
	dcMem.CreateCompatibleDC(pDC);
	CBitmap* pOldBitmap = dcMem.SelectObject(&c_bmpDividers);
	int cxDivider = (c_cyBitmap + 1) / 2;
	pDC->BitBlt(pt.x, pt.y, cxDivider, c_cyBitmap,
		&dcMem, cxDivider * nImage, 0, SRCCOPY);
	dcMem.SelectObject(pOldBitmap);
}

void CWidgetScrollBar::OnPaint()
{
	CRect client;
	GetClientRect(client);

	if (!m_bShowScrollBar != !m_scrollBar.IsWindowVisible())
	{
		Invalidate(FALSE);

		if (!m_bAlwaysShowScrollBar)
			m_bShowScrollBar = !m_bShowScrollBar;
		else
			m_bShowScrollBar = TRUE;
		if (m_bShowScrollBar)
			m_nWidgetWidth = m_nSplitterPref;
		RecalcLayout();
		if (m_bAlwaysShowScrollBar)
			m_scrollBar.ShowWindow(m_bShowScrollBar);
	}

	CPaintDC dc(this);
	
	CRect rect;

	CBrush* pbrWindowFrame = GetSysBrush(COLOR_WINDOWFRAME);
	CBrush* pbrWindow = GetSysBrush(COLOR_WINDOW);
	CBrush* pbrBtnText = GetSysBrush(COLOR_BTNTEXT);
	CBrush* pbrBtnHighlight = GetSysBrush(COLOR_BTNHIGHLIGHT);
	CBrush* pbrBtnShadow = GetSysBrush(COLOR_BTNSHADOW);
	CBrush* pbrBtnFace = GetSysBrush(COLOR_BTNFACE);

	// Draw tab scroll buttons...
	CDC memDC;
	memDC.CreateCompatibleDC(&dc);
	CBitmap* pOldBitmap = memDC.SelectObject(&c_bmpArrows);
	rect.SetRect(0, 0, cxButton, client.bottom);
	DrawButton(&dc, &memDC, 0, rect, m_captureZone == left && m_bButtonIn, m_xScrollPos == 0);
	rect.OffsetRect(cxButton - cxBorder, 0);
	DrawButton(&dc, &memDC, 1, rect, m_captureZone == right && m_bButtonIn, m_xScrollPos >= m_cxScrollExtent - (m_nWidgetWidth - 2 * cxButton - 2 * cxBorder));
	rect.OffsetRect(cxButton, 0);
	memDC.SelectObject(pOldBitmap);

	CBrush* pOldBrush = dc.SelectObject(pbrWindowFrame);
	dc.PatBlt(m_nWidgetWidth, client.top + cyBorder, cxBorder, client.Height() - 2 * cyBorder, PATCOPY);

	// Draw the border at the bottom.
	dc.PatBlt(rect.left, client.bottom - cyBorder, m_nWidgetWidth - rect.left + cxBorder, cyBorder, PATCOPY); 

	if (m_tabs.IsEmpty())
	{
		// if there are no tabs in the tab bar then paint the remainder and return
		dc.PatBlt(rect.left, client.top, m_nWidgetWidth - rect.left + cxBorder, cyBorder, PATCOPY); 
		dc.SelectObject(pbrBtnFace);
		dc.PatBlt(rect.left, client.top + cyBorder, m_nWidgetWidth, client.Height() - 2 * cyBorder, PATCOPY);
		dc.SelectObject(pOldBrush);
		return;
	}

	if (m_bShowScrollBar)
	{
		// Draw splitter box...
		dc.PatBlt(m_nWidgetWidth + cxSplitter - cxBorder, client.top + cyBorder, cxBorder, client.Height() - 2 * cyBorder, PATCOPY);
		dc.PatBlt(m_nWidgetWidth, client.top, cxSplitter, cyBorder, PATCOPY);
		dc.PatBlt(client.left, client.bottom - cyBorder, client.Width(), cyBorder, PATCOPY);
		dc.SelectObject(pbrBtnHighlight);
		dc.PatBlt(m_nWidgetWidth + cxBorder, client.top + cyBorder, cxBorder, client.Height() - 2 * cyBorder, PATCOPY);
		dc.SelectObject(pbrBtnShadow);
		dc.PatBlt(m_nWidgetWidth + cxSplitter - 2 * cxBorder, client.top + cyBorder, cxBorder, client.Height() - 2 * cyBorder, PATCOPY);
		dc.SelectObject(pbrBtnFace);
		dc.PatBlt(m_nWidgetWidth + cxBorder * 2, client.top + cyBorder, cxSplitter - 4 * cxBorder, client.Height() - 2 * cyBorder, PATCOPY);
	}
	

	// Draw tabs...
	CRect tabArea(rect.left, client.top, m_nWidgetWidth, client.bottom);
	if (dc.IntersectClipRect(tabArea) == NULLREGION)
	{
		dc.SelectObject(pOldBrush);
		return;
	}

	UINT nDivider = DIVIDER_X_I;	// first left-side divider

	// minimum width is 7, at which the height is between 7 and 13
	// at heights > 13, the width needs to grow by the same amount
	// maximum height is 23
	int cyDivider = client.Height() - 2;
	int cxDivider = (cyDivider + 1) / 2;
//	int cxDivider = 7;
//	if( cyDivider > 13 )
//		cxDivider += ((cyDivider - 13 + 1) / 2) * 2;	// will keep cx an odd number

	int x = tabArea.left - m_xScrollPos;
	int y = tabArea.top + cyBorder;
	int yText = y;
	CFont* pBoldFont = &c_widgetFontBold;
	CFont* pNormalFont = &c_widgetFontNormal;
	CFont* pOldFont = dc.SelectObject(pNormalFont);

	dc.SetBkColor(GetSysColor(COLOR_BTNFACE));
	dc.SetTextColor(GetSysColor(COLOR_BTNTEXT));
	dc.SetBkMode(OPAQUE);

	dc.SelectObject(pbrBtnShadow);

	POSITION pos = m_tabs.GetHeadPosition();
	int nTab = 0;
	CWidgetTab* pActiveTab = NULL;
	int xActive = 0;
	while (pos != NULL)
	{
		CWidgetTab* pTab = (CWidgetTab*)m_tabs.GetNext(pos);

		// recalc width here, based on current font
//		pTab->m_cxWidth = cxTabOverhead + dc.GetTextExtent(pTab->m_str, pTab->m_str.GetLength()).cx;

		if (nTab == m_nActiveTab)
		{
			dc.SelectObject(pBoldFont);
			dc.SetTextColor(GetSysColor(COLOR_WINDOWTEXT));
			dc.SetBkColor(GetSysColor(COLOR_WINDOW));
			nDivider = (nTab == 0) ? DIVIDER_X_A : DIVIDER_I_A;

			pActiveTab = pTab;
			xActive = x;
		}

		DrawDivider(&dc, nDivider, CPoint(x, y));
//			c_imgDividers.DrawImage(&dc, CPoint(x,y), nDivider, SRCCOPY);
	
		rect.SetRect(x + cxDivider, tabArea.top + cyBorder, 
			x + pTab->m_cxWidth, tabArea.bottom - 2 * cyBorder);
		int cxTextWidth = dc.GetTextExtent(pTab->m_str, pTab->m_str.GetLength()).cx;
		dc.ExtTextOut(x + cxDivider + (rect.Width() - cxTextWidth) / 2,
			 yText, ETO_OPAQUE, rect, pTab->m_str, pTab->m_str.GetLength(), NULL);
		dc.PatBlt(x + cxDivider, tabArea.bottom - 2 * cyBorder, pTab->m_cxWidth - cxDivider, cyBorder, PATCOPY);

		x += pTab->m_cxWidth;

		if (nTab == m_nActiveTab)
		{
			nDivider = DIVIDER_A_I;		// divider between active and inactive tabs
			dc.SelectObject(pNormalFont);
			dc.SetTextColor(GetSysColor(COLOR_BTNTEXT));
			dc.SetBkColor(GetSysColor(COLOR_BTNFACE));
		}
		else
			nDivider = DIVIDER_I_I;		// divider between inactive tabs

		nTab += 1;
	}
	
	if( !m_tabs.IsEmpty() )
	{
		CPoint pt(x, y);
		if( nTab != m_nActiveTab + 1 )
			DrawDivider(&dc, DIVIDER_I_X, pt);
		else
			DrawDivider(&dc, DIVIDER_A_X, pt);

		// draw top border to left and right of active tab.
		ASSERT( pActiveTab != NULL );
		dc.SelectObject(pbrBtnText);	// You'd think it should be WindowFrame, but Excel uses BtnText
		dc.PatBlt(client.left, client.top, xActive - client.left, cyBorder, PATCOPY);
		dc.PatBlt(xActive + pActiveTab->m_cxWidth + cxDivider, client.top, 
			client.right - xActive + pActiveTab->m_cxWidth + cxDivider, cyBorder, 
			PATCOPY);
		dc.SelectObject(pbrWindow);
		dc.PatBlt(xActive, client.top, pActiveTab->m_cxWidth + cxDivider, cyBorder, 
			PATCOPY);
		dc.SelectObject(pbrBtnShadow);

	}

	dc.SelectObject(pbrBtnFace);
	dc.PatBlt(x + cxDivider, tabArea.top + cyBorder, tabArea.Width(), tabArea.Height() - 2 * cyBorder, PATCOPY);

	dc.SelectObject(pOldFont);
	dc.SelectObject(pOldBrush);
}

void CWidgetScrollBar::OnSysColorChange()
{
	CWnd::OnSysColorChange();

	UpdateDividerBitmap(c_cyBitmap, TRUE);

	Invalidate();
}

void CWidgetScrollBar::OnWinIniChange(LPCSTR lpszSection)
{
	CWnd::OnWinIniChange(lpszSection);

	RecalcLayout();

	Invalidate();
}

void CWidgetScrollBar::OnTimer(UINT nIDEvent)
{
	ASSERT(m_captureZone == left || m_captureZone == right);
	
	Scroll(m_captureZone == left ? -cxButton : cxButton);
	SetTimer(nIDEvent, msRepeatRate, NULL);
}

int CWidgetScrollBar::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (m_scrollBar.Create(WS_HSCROLL | WS_VISIBLE | WS_CHILD, CRect(0, 0, 0, 0), 
		this, AFX_IDW_HSCROLL_FIRST) == -1)
	{
		return -1;
	}
	
	return 0;
}

void CWidgetScrollBar::RecalcLayout()
{
	CRect client;
	GetClientRect(client);
	if (!m_bAlwaysShowScrollBar && !m_bShowScrollBar)
		m_nWidgetWidth = client.right - cxBorder;

	int xScrollBar = m_nWidgetWidth + cxSplitter - cxBorder;
	m_scrollBar.MoveWindow(xScrollBar, 0, client.Width() - xScrollBar, client.Height());
}

void CWidgetScrollBar::OnSize(UINT nType, int cx, int cy)
{
	if (c_cyHeightPrev != cy)
	{
		c_cyHeightPrev = cy;
		
		LOGFONT logfont = *GetStdLogfont(font_Normal);
		logfont.lfHeight = (cy - 3 * cyBorder);
		logfont.lfPitchAndFamily = DEFAULT_PITCH | FF_SWISS;
		// [olympus 10955 - chauv] I decided to not zero out the lfFaceName below because
		// our standard package gives the appropriate facename for some platforms we support
		// specifically US and Japanese. BradCh mentioned that by zero out the facename, we
		// are relying for the OS to supply the appropriate FF_SWISS family font and therefore
		// we will get smoother font as scrollbar height is changed. However we both agreed
		// that we are going to use our standardfont package supplies by GetStdLogFont()
		// in order for the widget control to display readable text (instead of pixel blocks).

		// [chauv - 7/11/95] Oh well, MS Sans Serif is not a True Type font so for US and other
		// FE other than Japanese, we just clear the face name and let the OS give us the default.
		// For Japanese, we use "MS P Gothic" and it's a TrueType so we do want to keep the
		// facename from GetStdLogfont().
		if (_getmbcp() != 932)	// check for Japanese codepage 932 here
			logfont.lfFaceName[0] = 0;
		c_widgetFontNormal.DeleteObject();
		c_widgetFontNormal.CreateFontIndirect(&logfont);
		
		logfont = *GetStdLogfont(font_Bold);
		logfont.lfHeight = (cy - 3 * cyBorder);
		logfont.lfPitchAndFamily = DEFAULT_PITCH | FF_SWISS;
		// [olympus 10955 - chauv] see comment above...
		if (_getmbcp() != 932)	// check for Japanese codepage 932 here
			logfont.lfFaceName[0] = 0;
		c_widgetFontBold.DeleteObject();
		c_widgetFontBold.CreateFontIndirect(&logfont);

		UpdateDividerBitmap(cy - 2);

		CClientDC dc(this);
		CFont* pOldFont = dc.SelectObject(&c_widgetFontBold);
		
		m_cxScrollExtent = 0;
		POSITION pos = m_tabs.GetHeadPosition();
		while (pos != NULL)
		{
			CWidgetTab* pTab = (CWidgetTab*)m_tabs.GetNext(pos);

			// recalc width here, based on current font and divider size
			pTab->m_cxWidth = cxTabOverhead + (c_cyBitmap + 1) / 2 + dc.GetTextExtent(pTab->m_str, pTab->m_str.GetLength()).cx;
			m_cxScrollExtent += pTab->m_cxWidth;

		}
		
		dc.SelectObject(pOldFont);
	}
	
	if (m_nSplitterPref == 0)
		m_nSplitterPref = cx / 2;

	SetSplitterPos(m_nSplitterPref);

	if (m_nWidgetWidth > cx - cxSplitter - cxMinScrollBarWidth)
		m_nWidgetWidth = cx - cxSplitter - cxMinScrollBarWidth;

	RecalcLayout();
}

// The dividers need to have a specific look to them, going down 2, over 1
void DrawDiagonal(CDC* pDC, int x, int y, int dx, int cyBottom)
{
	for( ; y < cyBottom; y += 2, x += dx )
	{
		pDC->MoveTo(x, y);
		pDC->LineTo(x, y + 2);
	}
}

void CWidgetScrollBar::UpdateDividerBitmap(int cy, BOOL bForce)
{
	if( ((cy == c_cyBitmap) && !bForce) || (cy == -1 ) )
		return;

	c_cyBitmap = cy;
	int cx = (cy + 1) / 2;
	int cxBmp = cx * 7;

	if( c_bmpDividers.m_hObject != NULL )
		c_bmpDividers.DeleteObject();

	CWindowDC dcWindow(this);
	CDC dcMem;
	VERIFY( dcMem.CreateCompatibleDC(&dcWindow) );
	c_bmpDividers.CreateCompatibleBitmap(&dcWindow, cxBmp, c_cyBitmap);
	CBitmap* pOldBitmap = dcMem.SelectObject(&c_bmpDividers);

	// fill bitmap with gray
	CBrush* pOldBrush = dcMem.SelectObject(GetSysBrush(COLOR_BTNFACE));
	dcMem.PatBlt(0, 0, cxBmp, c_cyBitmap, PATCOPY);

	// paint active area white
	dcMem.SelectObject(GetSysBrush(COLOR_WINDOW));
	for( int y = 0; y < c_cyBitmap; y++ )
	{
		dcMem.PatBlt(cx * 2 + (y / 2) + 1, y, cx * 2 - ((y / 2) * 2) - 2, 1, 
			PATCOPY);
		dcMem.PatBlt(cx * 4 + (y / 2) + 1, y, cx * 2 - ((y / 2) * 2) - 2, 1, 
			PATCOPY);
	}

	int cyHalf = (cy + 1) / 2;
	
	// draw shadows first, because they might overwrite lines if drawn later
	CPen* pOldPen = dcMem.SelectObject(GetSysPen(COLOR_BTNSHADOW));
	DrawDiagonal(&dcMem, 2 * cx - 2, 0, -1, cy);
	DrawDiagonal(&dcMem, 9 * cx / 2 - 1, cyHalf, -1, cy);
	DrawDiagonal(&dcMem, 13 * cx / 2 - 1, cyHalf, -1, cy);
	dcMem.SetPixel(cx * 6 - 1, cy - 1, GetSysColor(COLOR_BTNSHADOW));

	// draw divider borders
	dcMem.SelectObject(GetSysPen(COLOR_BTNTEXT));
	DrawDiagonal(&dcMem, 0, 0, 1, cy);
	DrawDiagonal(&dcMem, 2 * cx - 1, 0, -1, cy);
	DrawDiagonal(&dcMem, 2 * cx, 0, 1, cy);
	DrawDiagonal(&dcMem, 4 * cx - 1, 0, -1, cy);
	DrawDiagonal(&dcMem, 4 * cx, 0, 1, cy);
	DrawDiagonal(&dcMem, 9 * cx / 2, cyHalf, -1, cy);
	DrawDiagonal(&dcMem, 6 * cx - 1, 0, -1, cy);
	DrawDiagonal(&dcMem, 6 * cx - 1 - (cx / 2), cyHalf, 1, cy);
	DrawDiagonal(&dcMem, 6 * cx, 0, 1, cy);
	DrawDiagonal(&dcMem, 13 * cx / 2, cyHalf, -1, cy);

	dcMem.SelectObject(pOldPen);
	dcMem.SelectObject(pOldBrush);
	dcMem.SelectObject(pOldBitmap);
}

void CWidgetScrollBar::Scroll(int cxScroll)
{
	// Adjust cxScroll to something within acceptable limits...
	if (m_xScrollPos + cxScroll > m_cxScrollExtent - (m_nWidgetWidth - 2 * cxButton - 2 * cxBorder))
		cxScroll = m_cxScrollExtent - (m_nWidgetWidth - 2 * cxButton - 2 * cxBorder) - m_xScrollPos;
	if (m_xScrollPos + cxScroll < 0)
		cxScroll = -m_xScrollPos;

	if (cxScroll == 0)
		return;

	CRect rect;
	GetClientRect(rect);

	rect.left += 2 * cxButton - cxBorder;
	rect.right = m_nWidgetWidth;
	rect.bottom -= cyBorder;

	UpdateWindow();
	ScrollWindow(-cxScroll, 0, rect, rect);

	m_xScrollPos += cxScroll;
}

int CWidgetScrollBar::FindTab(DWORD dwUser)
{
	POSITION pos = m_tabs.GetHeadPosition();
	int nTab = 0;
	while (pos != NULL)
	{
		CWidgetTab* pTab = (CWidgetTab*)m_tabs.GetNext(pos);
		if (pTab->m_dwUser == dwUser)
			return nTab;
		nTab += 1;
	}

	return -1;
}

DWORD CWidgetScrollBar::GetTabData(int nTab)
{
	POSITION pos = m_tabs.FindIndex(nTab);
	ASSERT(pos != NULL);
	CWidgetTab* pTab = (CWidgetTab*)m_tabs.GetAt(pos);
	ASSERT(pTab != NULL);

	return pTab->m_dwUser;
}
