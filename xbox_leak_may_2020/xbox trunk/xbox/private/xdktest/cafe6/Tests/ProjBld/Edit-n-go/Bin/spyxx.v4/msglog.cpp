// msglog.cpp : implementation file
//

#include "stdafx.h"
#pragma hdrstop

#include "msglog.h"
#include "msgview.h"
#include "msgdoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMsgLog

#define FIRST()			(m_iFirst)
#define TOP()			((m_iFirst + m_iTop) % m_cLinesMax)
#define LAST()			((m_iFirst + m_cLines) % m_cLinesMax)
#define SELECTED()		((m_iFirst + m_iSelected) % m_cLinesMax)
#define INC(x)			((x) = ++(x) % m_cLinesMax)
#define OFFSETX			(m_Tdx/2)

#define BOUND(x,mn,mx)	((x) < (mn) ? (mn) : ((x) > (mx) ? (mx) : (x)))

#define FTwixtI3(l,x,h)	((x)>=(l) && (x<=h))

CMsgLog::CMsgLog()
{
	m_aLines = NULL;
	m_hfont = NULL;
	m_iSelected = -1;
	m_cchMax = 200;
	m_pmdPopup = NULL;

	//
	// Set to non-zero values until SetFont is called, or the
	// create might end up causing a divide by zero error.
	//
	m_Tdx = 1;
	m_Tdy = 1;
}

CMsgLog::~CMsgLog()
{
}

BEGIN_MESSAGE_MAP(CMsgLog, CWnd)
	//{{AFX_MSG_MAP(CMsgLog)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_WM_PAINT()
	ON_WM_KEYDOWN()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONDOWN()
	ON_WM_KILLFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMsgLog message handlers

int CMsgLog::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}

void CMsgLog::OnDestroy()
{
	CWnd::OnDestroy();

	int i;
	int iQueue;

	iQueue = FIRST();
	for (i = 0; i < m_cLines; i++, INC(iQueue))
	{
		ASSERT(m_aLines[iQueue].pszText != NULL);
		delete [] m_aLines[iQueue].pszText;
		delete [] m_aLines[iQueue].msd2.pbExtraData;
	}

	delete [] m_aLines;
}

void CMsgLog::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	CalcLineSizes();
	UpdateScrollBars(TRUE);
}

void CMsgLog::CalcLineSizes()
{
	//
	// Determine the number of full lines that can be drawn in
	// the current client area.
	//
	CRect Rect;
	GetClientRect(&Rect);
	ASSERT(m_Tdy);
	m_cMaxVisibleLines = Rect.Height() / m_Tdy;
}

void CMsgLog::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	switch (nSBCode)
	{
		case SB_LINEDOWN:
			VerticalScroll(1);
			break;

		case SB_LINEUP:
			VerticalScroll(-1);
			break;

		case SB_PAGEUP:
			VerticalScroll(-m_cMaxVisibleLines);
			break;

		case SB_PAGEDOWN:
			VerticalScroll(m_cMaxVisibleLines);
			break;

		case SB_THUMBTRACK:
		case SB_THUMBPOSITION:
			VerticalScroll(nPos - m_iTop);
			break;

		case SB_ENDSCROLL:
			break;
	}
}

void CMsgLog::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	switch (nSBCode)
	{
		case SB_LINEDOWN:
			HorizontalScroll(1);
			break;

		case SB_LINEUP:
			HorizontalScroll(-1);
			break;

		case SB_PAGEUP:
			HorizontalScroll(-GetVisibleChars());
			break;

		case SB_PAGEDOWN:
			HorizontalScroll(GetVisibleChars());
			break;

		case SB_THUMBTRACK:
		case SB_THUMBPOSITION:
			HorizontalScroll(nPos - m_iLeft);
			break;

		case SB_ENDSCROLL:
			break;
	}
}

void CMsgLog::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	int iQueue;
	int xco;
	int yco;
	int i;
	int cDrawLines;
	HBRUSH hb;
	COLORREF c;
	RECT rcT;

	dc.SetTextColor(GetSysColor(COLOR_WINDOWTEXT));
	c = GetSysColor(COLOR_WINDOW);
	dc.SetBkColor(c);
	hb = CreateSolidBrush(c);

	//
	// SetFont must be called before painting.
	//
	ASSERT(m_hfont);

	dc.SelectObject(m_hfont);

	iQueue = TOP();
	cDrawLines = m_cLines - m_iTop;

	xco = OFFSETX - m_iLeft * m_Tdx;
	yco = 0;

	rcT.left = dc.m_ps.rcPaint.left;
	rcT.right = dc.m_ps.rcPaint.right;

	for (i = 0; i < cDrawLines; i++)
	{
		if (FTwixtI3((int)dc.m_ps.rcPaint.top, yco,(int)dc.m_ps.rcPaint.bottom)
			|| FTwixtI3((int)dc.m_ps.rcPaint.top, yco + (int)m_Tdy, (int)dc.m_ps.rcPaint.bottom))
		{
			rcT.top = yco;
			rcT.bottom = yco + m_Tdy;

			if (m_aLines[iQueue].pszText == NULL || *(m_aLines[iQueue].pszText) == 0)
			{
				FillRect(dc.m_ps.hdc, &rcT, hb);
			}
			else
			{
				CWnd *pFocus = GetFocus();

				if (m_iSelected != -1 && iQueue == SELECTED())
				{
					if (pFocus == this || pFocus == GetParent())
					{
						dc.SetTextColor(GetSysColor(COLOR_HIGHLIGHTTEXT));
						dc.SetBkColor(GetSysColor(COLOR_HIGHLIGHT));
						dc.ExtTextOut(xco, yco, ETO_OPAQUE, &rcT, m_aLines[iQueue].pszText, m_aLines[iQueue].cchText, NULL);
						dc.SetTextColor(GetSysColor(COLOR_WINDOWTEXT));
						dc.SetBkColor(GetSysColor(COLOR_WINDOW));

						GetClientRect(&m_rcFocus);
						m_rcFocus.top = rcT.top;
						m_rcFocus.bottom = rcT.bottom;
						dc.DrawFocusRect(&m_rcFocus);
					}
					else
					{
						dc.ExtTextOut(xco, yco, ETO_OPAQUE, &rcT, m_aLines[iQueue].pszText, m_aLines[iQueue].cchText, NULL);

						CBrush brush;
						if (brush.CreateSolidBrush(GetSysColor(COLOR_HIGHLIGHT)))
						{
							GetClientRect(&m_rcFocus);
							m_rcFocus.top = rcT.top;
							m_rcFocus.bottom = rcT.bottom;
							dc.FrameRect(&m_rcFocus, &brush);
						}
					}
				}
				else
				{
					dc.ExtTextOut(xco, yco, ETO_OPAQUE, &rcT, m_aLines[iQueue].pszText, m_aLines[iQueue].cchText, NULL);
				}
			}
		}

		yco += m_Tdy;
		INC(iQueue);
	}

	//
	// Erase the rest of the client area below the lines we just
	// painted above.
	//
	if (yco < dc.m_ps.rcPaint.bottom)
	{
		rcT.top = yco;
		rcT.bottom = dc.m_ps.rcPaint.bottom;
		FillRect(dc.m_ps.hdc, &rcT, hb);
	}

	DeleteObject((HANDLE)hb);
}

void CMsgLog::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	switch (nChar)
	{
		int iCurSel;
		case VK_UP:
			if (GetCurSel() > 0)
			{
				// the ordering of get selection, set selection, scroll is important here.
				// this keeps the selected item at the top of the list during scrolling...
				iCurSel = GetCurSel();
				SelectLine(iCurSel - 1);
				if (iCurSel == m_iTop)
					VerticalScroll(-1);
			}
			break;

		case VK_DOWN:
			if (GetCurSel() < (m_cLines - 1))
			{
				// the ordering of get selection, set selection, scroll is important here.
				// this keeps the selected item at the top of the list during scrolling...
				iCurSel = GetCurSel();
				SelectLine(iCurSel + 1);
				if (iCurSel == m_iTop + m_cMaxVisibleLines - 1)
					VerticalScroll(1);
			}
			break;

		case VK_PRIOR:
			if (m_iTop < m_cMaxVisibleLines)
			{
				SelectLine(0);
				VerticalScroll(-m_iTop);
			}
			else
			{
				SelectLine(GetCurSel() - m_cMaxVisibleLines);
				VerticalScroll(-m_cMaxVisibleLines);
			}
			break;

		case VK_NEXT:
			if (m_iTop + 2 * m_cMaxVisibleLines + 1 > m_cLines)
			{
				SelectLine(m_cLines - 1);
				VerticalScroll(m_cLines - m_iTop - m_cMaxVisibleLines);
			}
			else
			{
				SelectLine(GetCurSel() + m_cMaxVisibleLines);
				VerticalScroll(m_cMaxVisibleLines);
			}
			break;

		case VK_LEFT:
			HorizontalScroll(-1);
			break;

		case VK_RIGHT:
			HorizontalScroll(1);
			break;

		case VK_HOME:
			if (m_cLines)
			{
				SelectLine(0);
				VerticalScroll(-m_iTop);
			}
			break;

		case VK_END:
			if (m_cLines)
			{
				SelectLine(m_cLines - 1);
				VerticalScroll(m_cLines - m_iTop - m_cMaxVisibleLines);
			}
			break;
	}
}

void CMsgLog::ClearAll()
{
	int i;
	int iQueue;

	iQueue = FIRST();
	for (i = 0; i < m_cLines; i++, INC(iQueue))
	{
		ASSERT(m_aLines[iQueue].pszText != NULL);
		delete [] m_aLines[iQueue].pszText;
		m_aLines[iQueue].pszText = NULL;
		delete [] m_aLines[iQueue].msd2.pbExtraData;
		m_aLines[iQueue].msd2.pbExtraData = NULL;
	}

	m_cLines = 0;
	m_iFirst = 0;
	m_iTop = 0;
	m_iLeft = 0;

	SelectLine(-1);

	UpdateScrollBars(FALSE);
	InvalidateRect(NULL, TRUE);
	UpdateWindow();
}

void CMsgLog::VerticalScroll(int n)
{
	int iMinPos;
	int iMaxPos;

	GetScrollRange(SB_VERT, &iMinPos, &iMaxPos);

	n = BOUND(m_iTop + n, iMinPos, iMaxPos) - m_iTop;
	if (n)
	{
		m_iTop += n;
		ScrollWindow(0, -n * m_Tdy, NULL, NULL);
		SetScrollPos(SB_VERT, m_iTop, TRUE);
	}
	UpdateWindow();
}

void CMsgLog::HorizontalScroll(int n)
{
	RECT rc;
	int iMinPos;
	int iMaxPos;

	GetScrollRange(SB_HORZ, &iMinPos, &iMaxPos);
	GetClientRect(&rc);

	n = BOUND(m_iLeft + n, iMinPos, iMaxPos) - m_iLeft;
	if (n)
	{
		CDC* pDC = CDC::FromHandle(::GetDC(m_hWnd));

		m_iLeft += n;

		if (m_iSelected != -1)
			pDC->DrawFocusRect(&m_rcFocus);	// to eliminate focus rect before scrolling
		ScrollWindow(-n * m_Tdx, 0, &rc, &rc);
		if (m_iSelected != -1)
			pDC->DrawFocusRect(&m_rcFocus);	// to restore focus rect after scrolling

		SetScrollPos(SB_HORZ, m_iLeft, TRUE);

		::ReleaseDC(m_hWnd, pDC->m_hDC);
	}
	UpdateWindow();
}

int CMsgLog::GetVisibleChars()
{
	RECT rcClient;

	ASSERT(m_Tdx);

	GetClientRect(&rcClient);

	return (rcClient.right - rcClient.left - OFFSETX) / m_Tdx;
}

void CMsgLog::AddLine(LPSTR psz, PMSGSTREAMDATA pmsd)
{
	int iLine;   // lines of output to show
	int cScroll;
	int cch = _tcslen(psz);
	int iNew;

	//
	// Number of lines actually displayed in the current window
	//
	iLine = m_cLines - m_iTop;

	if (m_cLines == m_cLinesMax)
	{
		iNew = m_iFirst;
		delete [] m_aLines[iNew].pszText;
		delete [] m_aLines[iNew].msd2.pbExtraData;
		m_aLines[iNew].msd2.pbExtraData = NULL;

		INC(m_iFirst);

		if (m_iTop > 0)
		{
			m_iTop--;
		}

		//
		// Do we have a selection?
		//
		if (m_iSelected != -1)
		{
			//
			// Is the selection at the top line?
			//
			if (m_iSelected == 0)
			{
				//
				// The selected line just rolled off the top of the
				// log.  Clear the selection.
				//
				SelectLine(-1);
			}
			else
			{
				//
				// Decrement the index to the selection.  Note that
				// since we are not changing the selection (just
				// adjusting the index to it to account for the line
				// that was just deleted) we do not need to call
				// SelectLine().
				//
				m_iSelected--;
			}
		}
	}
	else
	{
		iNew = LAST();
		m_cLines++;
	}

	LINE* pLineNew = &m_aLines[iNew];

	if ((pLineNew->pszText = new CHAR[cch + 1]) == NULL)
	{
		return;
	}

	_tcscpy(pLineNew->pszText, psz);
	pLineNew->cchText = cch;

	pLineNew->msd2.fPostType	= pmsd->fPostType;
	pLineNew->msd2.nLevel	   = pmsd->nLevel;
	pLineNew->msd2.hwnd		 = pmsd->hwnd;
	pLineNew->msd2.msg		  = pmsd->msg;
	pLineNew->msd2.wParam	   = pmsd->wParam;
	pLineNew->msd2.lParam	   = pmsd->lParam;
	pLineNew->msd2.lResult	  = pmsd->lResult;
	pLineNew->msd2.mtMsgType = GetMsgType(pmsd->msg, pmsd->hwnd);

	//
	// Determine if there is additional data and copy it if necessary.
	// Note that pmsd->cb is the whole size of the packet, including
	// any additional data, which will be found at the first byte
	// past the MSGSTREAMDATA structure.  Therefore, if the size in
	// pmsd->cb is greater than the size of a PMSGSTREAMDATA structure,
	// there must be additional data.
	//
	if (pLineNew->msd2.cbExtraData = pmsd->cb - sizeof(MSGSTREAMDATA))
	{
		pLineNew->msd2.pbExtraData = new BYTE[pLineNew->msd2.cbExtraData];
		memcpy(pLineNew->msd2.pbExtraData, pmsd + 1, pLineNew->msd2.cbExtraData);
	}
	else
	{
		pLineNew->msd2.pbExtraData = NULL;
	}

	//
	// Invalidate the area where the new line will be so that
	// it gets painted.  This is actually done by invalidating
	// all of the client area below the line just before the
	// one that was just added.
	//
	RECT rcClient;
	GetClientRect(&rcClient);

	//
	// However, we must handle the special case where the max lines
	// allowed can all be visible at once and we have reached the
	// max lines.  If this is NOT the case, we move the rectangle
	// to invalidate down to only invalidate the new line.  Otherwise,
	// we must invalidate the entire area or the existing lines
	// that just got logically scrolled up will not get painted
	// properly.
	//
	if (m_cLinesMax > m_cMaxVisibleLines || m_cLines != m_cLinesMax)
	{
		rcClient.top += iLine * m_Tdy;
	}

	InvalidateRect(&rcClient, TRUE);

	//
	// If we have more lines than we can display, scroll the window
	// such that the last line printed is now at the bottom.
	//
	if (iLine + 1 > m_cMaxVisibleLines)
	{
		cScroll = iLine + 1 - m_cMaxVisibleLines;
		UpdateScrollBars(FALSE);
		VerticalScroll(cScroll);
	}
}

void CMsgLog::UpdateScrollBars(BOOL bRedraw)
{
	int iRange;

	/* Update the scroll bars */
	iRange = m_cLines - m_cMaxVisibleLines;

	if (iRange < 0)
	{
		iRange = 0;
		VerticalScroll(-m_iTop);
	}

	SetScrollRange(SB_VERT, 0, iRange, FALSE);
	SetScrollPos(SB_VERT, m_iTop, bRedraw);

	iRange = m_cchMax - GetVisibleChars() + 1;
	if (iRange < 0)
	{
		iRange = 0;
		HorizontalScroll(-m_iLeft);
	}

	SetScrollRange(SB_HORZ, 0, iRange, FALSE);
	SetScrollPos(SB_HORZ, m_iLeft, bRedraw);
}

void CMsgLog::ShowSelection()
{
	if ((m_iSelected >= m_iTop) && (m_iSelected < (m_iTop + m_cMaxVisibleLines)))
		return;
	else if (m_iSelected < m_iTop)
		VerticalScroll(-(m_iTop - m_iSelected));
	else if (m_iSelected >= (m_iTop + m_cMaxVisibleLines))
		VerticalScroll(m_iSelected - (m_iTop + m_cMaxVisibleLines) + 1);
}

BOOL CMsgLog::IsEmpty()
{
	//
	// It is empty if the line count is zero.
	//
	return (m_cLines == 0) ? TRUE : FALSE;
}

void CMsgLog::SetFont(CFont* pFont, BOOL bRedraw)
{
	TEXTMETRIC tm;

	m_hfont = (HFONT)pFont->m_hObject;

	/* Find out the size of a Char in the font */
	CDC dc;
	CFont* pFontOld;
	dc.Attach(::GetDC(NULL));
	pFontOld = dc.SelectObject(pFont);
	dc.GetTextMetrics(&tm);
	dc.SelectObject(pFontOld);
	dc.Detach();

	m_Tdy = (int)tm.tmHeight;
	m_Tdx = (int)tm.tmAveCharWidth;

	//
	// Force the size calculations to be done because the height
	// of the new font may have changed the number of visible lines.
	//
	CalcLineSizes();
	UpdateScrollBars(FALSE);

	if (bRedraw)
	{
		InvalidateRect(NULL, TRUE);
		UpdateWindow();
	}
}

void CMsgLog::SetMaxLines(int cLinesMax)
{
	LINE* aLinesNew;
	int i;
	int iQueue;

	//
	// Zero is not valid
	//
	ASSERT(cLinesMax != 0);

	//
	// Allocate a new array with the requested number of lines.
	// Note that we do a memset to quickly zero out the memory
	// instead of looping through each one.  This is only ok
	// because we initially want each line structure set to
	// all zeros.
	//
	aLinesNew = new LINE[cLinesMax];
	memset(aLinesNew, 0, sizeof(LINE) * cLinesMax);

	if (m_aLines)
	{
		//
		// Are we making the maximum lines be less than the current
		// lines the log has?  If so, we need to delete the extra
		// lines.
		//
		if (cLinesMax < m_cLines)
		{
			int cDelete = m_cLines - cLinesMax;

			//
			// Is the selected line one of the lines that is
			// being surplused?
			//
			if (m_iSelected != -1 && m_iSelected < cDelete)
			{
				//
				// Yes, clear the selection.
				//
				SelectLine(-1);
			}
			else
			{
				//
				// No, we can still have a selection, but we must
				// adjust the index down by the number of lines
				// that are being deleted.  Note that since we
				// are not actually changing the line that was
				// selected (just its index), we do not need to
				// call SelectLine().
				//
				m_iSelected -= cDelete;
			}

			iQueue = FIRST();
			while (cDelete--)
			{
				delete [] m_aLines[iQueue].pszText;
				delete [] m_aLines[iQueue].msd2.pbExtraData;

				INC(iQueue);
			}

			m_iFirst = iQueue;
			m_cLines = cLinesMax;
		}

		i = 0;
		iQueue = FIRST();
		while (i < m_cLines)
		{
			//
			// Do a memberwise copy of the old line structure to
			// the new one.  Note that this will copy the pointers
			// as well, including pszText and msd2.pbExtraData.
			// This is what we want, because we don't want to
			// have to allocate new blocks for these things
			// for speed.  If the line structures were a class,
			// then we would have to orphan off the old memory
			// by setting the pointers to NULL (or the destructor
			// would free the memory block out from under us!)
			// but since m_aLines is just an array of structures
			// we don't need to worry about it.
			//
			aLinesNew[i] = m_aLines[iQueue];

			i++;
			INC(iQueue);
		}

		//
		// Poof the old array now.
		//
		delete [] m_aLines;
	}
	else
	{
		m_cLines = 0;
	}

	m_iFirst = 0;
	m_iTop = 0;
	m_iLeft = 0;
	m_cLinesMax = cLinesMax;
	m_aLines = aLinesNew;

	UpdateScrollBars(TRUE);
	InvalidateRect(NULL, TRUE);
	UpdateWindow();
}

void CMsgLog::SelectLine(int iSelect)
{
	if (iSelect > (m_cLines - 1) || (!m_fFocusLostBetween && (iSelect == m_iSelected)))
	{
		return;
	}

	m_fFocusLostBetween = FALSE;
	InvalidateLine(m_iSelected);
	InvalidateLine(iSelect);

	m_iSelected = iSelect;
	if (iSelect != -1)
		SetLastSelectedObject((DWORD)&m_aLines[SELECTED()].msd2, OT_MESSAGE);
	else
		SetLastSelectedObject((DWORD)0, OT_NONE);
}

void CMsgLog::InvalidateLine(int iLine)
{
	if (iLine >= m_iTop && iLine <= m_iTop + m_cMaxVisibleLines)
	{
		RECT rc;

		GetClientRect(&rc);
		rc.top = (iLine - m_iTop) * m_Tdy;
		rc.bottom = rc.top + m_Tdy;

		InvalidateRect(&rc, TRUE);
	}
}

void CMsgLog::OnLButtonDown(UINT nFlags, CPoint point)
{
	CWnd::OnLButtonDown(nFlags, point);

	SelectLine(m_iTop + (point.y / m_Tdy));
}

void CMsgLog::ShowSelItemProperties()
{
	//
	// We must check for a valid selection here.  The following
	// scenario is possible:
	// 1. The user selects an item with the right mouse popup menu.
	// 2. More messages come in and scroll the selected line off the
	//	top and out of the circular buffer, thus removing any selection.
	// 3. The user then selects the "Properties" menu item, which is
	//	still enabled.
	//
	if (m_iSelected != -1)
	{
		CPropertyInspector::ShowObjectProperties((DWORD)&m_aLines[SELECTED()].msd2, OT_MESSAGE);
		InvalidateLine(GetCurSel());
		UpdateWindow();
	}
}

void CMsgLog::ShowViewedItemProperties(CMsgView *pView)
{
//	CPropertyInspector::ShowObjectProperties((DWORD)&m_aLines[SELECTED()].msd2, OT_MESSAGE);
	CMsgDoc *pDoc = pView->GetDocument();

	switch (pDoc->GetObjectTypeSpyingOn())
	{
		case OT_WINDOW:
			CPropertyInspector::ShowObjectProperties((DWORD)pDoc->GetWindowSpyingOn(), OT_WINDOW);
			break;

		case OT_THREAD:
			CPropertyInspector::ShowObjectProperties((DWORD)pDoc->GetThreadSpyingOn(), OT_THREAD);
			break;

		case OT_PROCESS:
			CPropertyInspector::ShowObjectProperties((DWORD)pDoc->GetProcessSpyingOn(), OT_PROCESS);
			break;

		case OT_NONE:
		default:
			break;
	}

	InvalidateLine(GetCurSel());
	UpdateWindow();
}

void CMsgLog::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	//
	// Be sure that the double-click hit a valid line
	//
	if (m_iTop + (point.y / m_Tdy) < m_cLines)
	{
		ShowSelItemProperties();
	}
}

void CMsgLog::OnRButtonDown(UINT nFlags, CPoint point)
{
	CWnd::OnRButtonDown(nFlags, point);

	//
	// Be sure that the click hit a valid line
	//
	if (m_iTop + (point.y / m_Tdy) < m_cLines)
	{
		SelectLine(m_iTop + (point.y / m_Tdy));

		PMSGSTREAMDATA2 pmsd2 = &m_aLines[SELECTED()].msd2;

		CString str;
		CMenu MenuPopup;
		if (!MenuPopup.CreatePopupMenu())
		{
			return;
		}

		str.LoadString(IDS_HELPON);

		switch (pmsd2->mtMsgType)
		{
			case MT_NORMAL:
				m_pmdPopup = CMsgDoc::m_apmdLT[pmsd2->msg];
				str += m_pmdPopup->pszMsg;
				break;

			case MT_DIALOG:
				m_pmdPopup = CMsgDoc::m_apmdDlgLT[pmsd2->msg - WM_USER];
				str += m_pmdPopup->pszMsg;
				break;

#ifndef DISABLE_WIN95_MESSAGES
			case MT_ANIMATE:
				m_pmdPopup = CMsgDoc::m_apmdAniLT[pmsd2->msg - (WM_USER + 100)];
				str += m_pmdPopup->pszMsg;
				break;

			case MT_HEADER:
				m_pmdPopup = CMsgDoc::m_apmdHdrLT[pmsd2->msg - HDM_FIRST];
				str += m_pmdPopup->pszMsg;
				break;

			case MT_HOTKEY:
				m_pmdPopup = CMsgDoc::m_apmdHKLT[pmsd2->msg - (WM_USER + 1)];
				str += m_pmdPopup->pszMsg;
				break;

			case MT_LISTVIEW:
				m_pmdPopup = CMsgDoc::m_apmdLVLT[pmsd2->msg - LVM_FIRST];
				str += m_pmdPopup->pszMsg;
				break;

			case MT_PROGRESS:
				m_pmdPopup = CMsgDoc::m_apmdProgLT[pmsd2->msg - (WM_USER + 1)];
				str += m_pmdPopup->pszMsg;
				break;

			case MT_STATUSBAR:
				m_pmdPopup = CMsgDoc::m_apmdStatLT[pmsd2->msg - (WM_USER + 1)];
				str += m_pmdPopup->pszMsg;
				break;

			case MT_TOOLBAR:
				m_pmdPopup = CMsgDoc::m_apmdTBLT[pmsd2->msg - (WM_USER + 1)];
				str += m_pmdPopup->pszMsg;
				break;

			case MT_TRACKBAR:
				m_pmdPopup = CMsgDoc::m_apmdTrkLT[pmsd2->msg - WM_USER];
				str += m_pmdPopup->pszMsg;
				break;

			case MT_TABCTRL:
				m_pmdPopup = CMsgDoc::m_apmdTabLT[pmsd2->msg - TCM_FIRST];
				str += m_pmdPopup->pszMsg;
				break;

			case MT_TOOLTIP:
				m_pmdPopup = CMsgDoc::m_apmdTTLT[pmsd2->msg - (WM_USER + 1)];
				str += m_pmdPopup->pszMsg;
				break;

			case MT_TREEVIEW:
				m_pmdPopup = CMsgDoc::m_apmdTVLT[pmsd2->msg - TV_FIRST];
				str += m_pmdPopup->pszMsg;
				break;

			case MT_UPDOWN:
				m_pmdPopup = CMsgDoc::m_apmdUpDnLT[pmsd2->msg - (WM_USER + 101)];
				str += m_pmdPopup->pszMsg;
				break;
#endif	// DISABLE_WIN95_MESSAGES
			default:
				m_pmdPopup = NULL;
				CString str2;
				str2.LoadString(IDS_MESSAGE);
				str += str2;
				break;
		}

		MenuPopup.AppendMenu(MF_STRING, ID_VIEW_HELPONMESSAGE, str);

		str.LoadString(IDS_PROPERTIES);
		MenuPopup.AppendMenu(MF_STRING, ID_VIEW_PROPERTIES, str);

		ClientToScreen(&point);
		MenuPopup.TrackPopupMenu(TPM_RIGHTBUTTON, point.x, point.y, theApp.m_pMainWnd);
	}
}

void CMsgLog::OnKillFocus(CWnd* pNewWnd)
{
	CWnd::OnKillFocus(pNewWnd);
	
	SetLastSelectedObject(0, OT_NONE);

	InvalidateLine(GetCurSel());
	UpdateWindow();
	m_fFocusLostBetween = TRUE;
}

void CMsgLog::OnSetFocus(CWnd* pOldWnd)
{
	CWnd::OnSetFocus(pOldWnd);
	
	InvalidateLine(GetCurSel());
	UpdateWindow();
}
