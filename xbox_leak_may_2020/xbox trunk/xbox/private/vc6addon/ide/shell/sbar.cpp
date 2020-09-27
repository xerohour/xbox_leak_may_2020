// sbar.cpp
//
// This file contains the Sushi status bar.
//
// Copyright (c) 1993 Microsoft Corporation, all rights reserved.

#include "stdafx.h"

#include "shell.h"
#include "sbar.h"
#include "barglob.h"
#include "resource.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// arrays of IDs used to initialize status bars

static UINT BASED_CODE indicators[] =
{
	ID_SEPARATOR,
};

#define PERCENT_NIL -1

/////////////////////////////////////////////////////////////////////////////

static BOOL g_bEnableClock = FALSE;

void EnableClock(BOOL bEnableClock)
{
	g_bEnableClock = bEnableClock;
	if (theApp.m_pMainWnd != NULL)
		((CMainFrame*)theApp.m_pMainWnd)->m_statusBar.ResetIndicators();
}

BOOL IsClockEnabled()
{
	return g_bEnableClock;
}

CSushiBarInfo::CSushiBarInfo(const UINT FAR* pIndicators, int cIndicators,
	              STATINFO FAR* pBitmaps, int cBitmaps)
{
	m_pIndicators = pIndicators;
	m_cIndicators = cIndicators;

	m_pBitmaps = pBitmaps;
	m_cBitmaps = cBitmaps;
}

IMPLEMENT_DYNAMIC(CSushiBar, CStatusBar)

BEGIN_MESSAGE_MAP(CSushiBar, CStatusBar)
	//{{AFX_MSG_MAP(CSushiBar)
	ON_MESSAGE(WM_SETTEXT, OnSetText)
	ON_WM_CREATE()
	ON_WM_SYSCOLORCHANGE()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CString CSushiBar::m_strAfxIdleMessage;

CSushiBar::CSushiBar()
{
	if (GetSystemMetrics(SM_DBCSENABLED))
	{
		// olympus 14527: Make sure we use a reasonable font for DBCS environments
		ASSERT(globalData.hStatusFont != NULL);

		// Use something other than the system font
		DeleteObject(globalData.hStatusFont);
		globalData.hStatusFont = HFONT(*GetStdFont(font_Normal));
	}

	for (int i = 0 ; i < MAXPANES; i++)
		m_hbitmapArray[i] = NULL;

	// Optimization -- load this static string only once.
	if (m_strAfxIdleMessage.IsEmpty())
	{
		VERIFY( m_strAfxIdleMessage.LoadString(AFX_IDS_IDLEMESSAGE) );
		m_strIdlePrompt = m_strAfxIdleMessage;
	}

	m_pCurrentIndicators = NULL;
	m_bLockedUntilIdle = FALSE;
	m_bScrollingPrompt = FALSE;
	Reset();
}

CSushiBar::~CSushiBar()
{
	for (int i = 0 ; i < MAXPANES; i++)
		if (m_hbitmapArray[i] != NULL)
			::DeleteObject(m_hbitmapArray[i]);
}

int CSushiBar::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	return CStatusBar::OnCreate(lpCreateStruct);
}

void CSushiBar::DrawIndicatorNow(UINT nIDIndicator, BOOL bBitmapToo)
{
	int nItem = CommandToIndex(nIDIndicator);
	if (nItem != -1)
	{
		CString str, strOld;
		GetText(nIDIndicator, str);
		GetPaneText(nItem, strOld);
		if (str == strOld)
			return;

		SetPaneText(nItem, str, FALSE);
		CRect rect;
		GetItemRect(nItem, rect);

		// no need to redraw borders or bitmap; only invalidate text region
		rect.InflateRect(-1, -1);
	
		if (!bBitmapToo && m_hbitmapArray[nItem] != NULL)
		{
			BITMAP bmap;
			::GetObject(m_hbitmapArray[nItem], sizeof(BITMAP), &bmap);
			rect.left += bmap.bmWidth + CX_BORDER * 2;
		}
		
		InvalidateRect(rect, TRUE);
		UpdateWindow();
	}
}

void CSushiBar::OnTimer(UINT nIDEvent)
{
	ASSERT(nIDEvent == 0);
	DrawIndicatorNow(ID_INDICATOR_CLOCK);
}

void CSushiBar::SetPrompt(const char* szMessage, BOOL bScrolling)
{
	if (m_bLockedUntilIdle)
		return;

	m_bScrollingPrompt = bScrolling;

	// Note that szMessage can be NULL, meaning the status bar should be blank.
	CString strCurrentText;
	GetPaneText(0, strCurrentText);

	if ((szMessage == NULL) && strCurrentText.IsEmpty())
		return;

	if (szMessage && !strCurrentText.Compare(szMessage))
		return;

	SetPaneText(0, szMessage, FALSE);
	Invalidate(FALSE);
}

LRESULT CSushiBar::OnSetText(WPARAM, LPARAM lParam)
{
	if (!m_bLockedUntilIdle)
	{
		SetPromptColor(0, 0);

		if (lParam && !m_strAfxIdleMessage.Compare((LPCSTR)lParam))
			SetPrompt(m_strIdlePrompt);
		else
			SetPrompt((LPCSTR)lParam);
	}

	return 1;
}

void CSushiBar::SetIdlePrompt(const char* szMessage)
{
	m_strIdlePrompt = szMessage;
}

void CSushiBar::SetPromptUntilIdle(const char* szMessage)
{
	SetPrompt(szMessage);
	UpdateWindow();
	m_bLockedUntilIdle = TRUE;
}

void CSushiBar::SetPromptColor(COLORREF textColor, COLORREF bkColor)
{
	if (m_bLockedUntilIdle)
		return;

	BOOL bZeros = ((textColor == 0) && (bkColor == 0));

	if (m_bStandardColors && bZeros)
		return;

	if ((m_textColor = textColor) == 0)
		m_textColor = ::GetSysColor(COLOR_BTNTEXT);
	if ((m_bkColor = bkColor) == 0)
		m_bkColor = ::GetSysColor(COLOR_BTNFACE);

	m_bStandardColors = bZeros;
}

void CSushiBar::OnIdle()
{
	m_bLockedUntilIdle = FALSE;
}

void CSushiBar::BeginPercentDone(const char* szPrompt)
{
	ASSERT(szPrompt != NULL);

	m_iPercentDone++;

	if (m_iPercentDone == 1)
	{
		SetPrompt(" ");
		m_strPercentDonePrompt = szPrompt;
	}
	else
	{
		if (m_iPercentDone >= MAXLEVELS)
		{
			ASSERT(FALSE);
			return;
		}

		SetPrompt(szPrompt);
	}

	Invalidate(TRUE);
	m_rectCounter.SetRectEmpty();

	m_rgPercentDone[m_iPercentDone] = PERCENT_NIL;
	PercentDone(0);
}

void CSushiBar::EndPercentDone()
{
	if (m_iPercentDone < 1)
		return;

	m_iPercentDone--;
	::SetPrompt();
	Invalidate();
}

void CSushiBar::PercentDone(int percent)
{
	if (m_iPercentDone < 1)
		return;

	// No going back, and staying the same is a no-op.
	if (percent <= m_rgPercentDone[m_iPercentDone])
		return;

	// Do not allow percentages less than 0 or greater than 100.
	m_rgPercentDone[m_iPercentDone] = min(max(percent, 0), 100);

	// Unless we've got two counters running, we can simply invalidate the
	// rect in which the percent-done blocks appear.
	if (m_iPercentDone == 1)
		InvalidateRect(m_rectCounter, FALSE);
	else
		Invalidate(FALSE);
	UpdateWindow();
}

BOOL CSushiBar::SetIndicators(const CSushiBarInfo* pSushiBarInfo)
{
	if (pSushiBarInfo == NULL)
		return SetIndicators();

	return SetIndicators(pSushiBarInfo->m_pIndicators, pSushiBarInfo->m_cIndicators,
	                     pSushiBarInfo->m_pBitmaps, pSushiBarInfo->m_cBitmaps);
}

void CSushiBar::ResetIndicators()
{
	m_pCurrentIndicators = NULL;
	CPacket* pPacket = theApp.m_pActivePacket;
	SetIndicators(pPacket == NULL ? NULL : pPacket->m_pSushiBarInfo);
}

BOOL CSushiBar::SetIndicators(const UINT FAR* pIDArray, int nIDCount,
                              const STATINFO FAR* pStatInfo, int nCount)
{
	if (pIDArray == NULL)
	{
		ASSERT(nIDCount == 0);
		pIDArray = indicators;
		nIDCount = sizeof(indicators) / sizeof(UINT);
	}

	if (pIDArray == m_pCurrentIndicators)
		return TRUE;

	// If there is status text, preserve it across the switch
	CString str;
	if (m_nCount > 0)
		GetPaneText(0, str);

	// BLOCK: Add default indicators (e.g. the clock)
	{
		UINT* pRealIDArray = (UINT*)pIDArray;

		if (g_bEnableClock)
		{
			pRealIDArray = new UINT [nIDCount + 1];
			memcpy(pRealIDArray, pIDArray, sizeof (UINT) * nIDCount);
			pRealIDArray[nIDCount++] = ID_INDICATOR_CLOCK;
		}

		BOOL bOkay = CStatusBar::SetIndicators(pRealIDArray, nIDCount);

		if (pRealIDArray != pIDArray)
			delete [] pRealIDArray;

		if (!bOkay)
			return FALSE;
	}

	if (g_bEnableClock)
		SetTimer(0, 10000, NULL);
	else
		KillTimer(0);

	ASSERT(nIDCount < MAXPANES);
	for (int i = 0 ; i < MAXPANES; i++)
		if (m_hbitmapArray[i] != NULL)
		{
			::DeleteObject(m_hbitmapArray[i]);
			m_hbitmapArray[i] = NULL;
		}

	UINT nID, nStyle;
	int cxWidth, index;

	for (i = 0 ; i < nCount ; i++, pStatInfo++)
	{
		ASSERT(pStatInfo);

		index = CommandToIndex(pStatInfo->nID);
		if (index != -1)
		{
			m_hbitmapArray[index] = LoadBitmap(pStatInfo->nGroup, pStatInfo->nBitmap);
			ASSERT(m_hbitmapArray[index] != NULL);	// we could not find the
			                                      	// specified bitmap.
			BITMAP bmap;
			::GetObject(m_hbitmapArray[index], sizeof(BITMAP), &bmap);

			GetPaneInfo(index, nID, nStyle, cxWidth);
			ASSERT(nID == pStatInfo->nID);

			SetPaneInfo(index, nID, nStyle, cxWidth + bmap.bmWidth + CX_BORDER * 4);
		}
	}

	m_pCurrentIndicators = pIDArray;

	// Load the text into the newly-created status pane, if necessary
	if (m_bLockedUntilIdle && !str.IsEmpty())
		SetPaneText(0, str);
	else
		::SetPrompt();

	Invalidate();
	OnIdleUpdateCmdUI((WPARAM)TRUE, (LPARAM)0);
	return TRUE;
}

HBITMAP CSushiBar::LoadBitmap(int nPackage, int nBitmap)
{
	ASSERT(theApp.m_pMainWnd != NULL);
	ASSERT(theApp.m_pMainWnd->IsKindOf( RUNTIME_CLASS(CMainFrame) ));
	
	// locate the package which offers this bitmap
	CPack* pPack = theApp.GetPackage(nPackage);
	ASSERT(pPack != NULL);

	HINSTANCE hInstance = pPack->HInstance();
	if (hInstance == NULL)
		hInstance = GetResourceHandle();

	return ::LoadBitmap(hInstance, MAKEINTRESOURCE(nBitmap));
}

void CSushiBar::OnSysColorChange()
{
	CStatusBar::OnSysColorChange();
	m_bStandardColors = FALSE;
	::SetPrompt();
}

UINT
LnColMinWidth(CDC* pDC, UINT nLines, UINT nCols)
{
	CString strPrmpt;
	char strTmp[30];

	VERIFY( strPrmpt.LoadString(IDS_LNCOLPRMPT) );
	strPrmpt += 'W';		// Add some padding
	sprintf(strTmp, strPrmpt, nLines, nCols);
	return pDC->GetTextExtent(strTmp, _tcslen(strTmp)).cx;
}


void CSushiBar::DoPaint(CDC* pDC)
{
	ASSERT_VALID(this);
	ASSERT_VALID(pDC);

	CControlBar::DoPaint(pDC);      // draw border

	CRect rect;
	GetClientRect(rect);

	int xMax = rect.right - m_cxSizeBox;

	CalcInsideRect(rect, TRUE);

	UINT nID, nStyle, extraSpace = 0;
	int i, cxWidth;

	ASSERT(m_hFont != NULL);        // must have a font !
	HGDIOBJ hOldFont = pDC->SelectObject(m_hFont);

	// When we're in a progress loop, only the progress indicator shows
	if (m_iPercentDone > 0)
	{
		CString str;
		GetPaneText(0, str);
		if (m_iPercentDone == 1)
		{
			// Never put "ready" next to a progress bar
			if (!str.Compare(m_strAfxIdleMessage))
				str.Empty();
			DrawStatusPercentDone(pDC->m_hDC, rect, m_strPercentDonePrompt, m_rgPercentDone[1], str);
		}
		else
		{
			char buf [255];
			CString strFormat;
			VERIFY( strFormat.LoadString(ID_SBAR_PROGRESS) );

			sprintf(buf, (const char*)strFormat, (const char*)str, m_rgPercentDone[2]);
			DrawStatusPercentDone(pDC->m_hDC, rect, m_strPercentDonePrompt, m_rgPercentDone[1], buf);
		}
		pDC->SelectObject(hOldFont);
		return;
	}

	// If we are in a progress loop, only show the prompt -- not other panes
	int nCount = (m_iPercentDone > 0) ? 1 : m_nCount;
	static UINT cxLnColMin = 0;
	
	if (!cxLnColMin)
		cxLnColMin = LnColMinWidth(pDC, 100, 10);

	for (i = 0; i < nCount; i++)
	{
		CString str;
		BOOL	bRightAlign = FALSE;
		GetPaneInfo(i, nID, nStyle, cxWidth);
		GetPaneText(i, str);
		ASSERT(cxWidth >= 0);

		GetItemRect(i, &rect);

		if (::RectVisible(pDC->m_hDC, &rect))
		{
			if (m_hbitmapArray[i] != NULL)
				DrawStatusBitmap(pDC->m_hDC, rect, m_hbitmapArray[i], str, nStyle);
			else
			{
				// To improve the appearance when the text does not completely
				// fill the field (such as the LNCOL indicator) move the text
				// 1/3 towards the center of the available space.
				UINT textWidth = pDC->GetTextExtent(str, str.GetLength()).cx;
				if (nID != ID_SEPARATOR)
				{
					// Resize the line/col indicator if we're overflowing the available space
					if (nID == ID_INDICATOR_LNCOL)
					{
						CString strTmp = str + 'W';
						UINT	cxWidthRequired = max(cxLnColMin, (UINT)(pDC->GetTextExtent(strTmp, strTmp.GetLength()).cx));

						if ((UINT)cxWidth != cxWidthRequired)
							SetPaneInfo(i, nID, nStyle, cxWidth = cxWidthRequired);
					}
					extraSpace = ((UINT)cxWidth > textWidth) ? (cxWidth - textWidth) / 3 : 0;
				}
				else
				{
					extraSpace = 0;
				}

				DrawStatusText(pDC->m_hDC, rect, str, nStyle, textWidth, extraSpace);
			}
		}
	}

	pDC->SelectObject(hOldFont);

	// draw the size box in the bottom right corner
	if (!m_bHideSizeBox)
	{
		int cxMax = min(m_cxSizeBox, rect.Height()+m_cyTopBorder);
		rect.left = xMax + (m_cxSizeBox - cxMax) + CX_BORDER;
		rect.bottom -= CX_BORDER;
		HPEN hPenOld = (HPEN)pDC->SelectObject(globalData.hpenBtnHilite);
		for (i = 0; i < cxMax; i += 4)
		{
			pDC->MoveTo(rect.left+i, rect.bottom);
			pDC->LineTo(rect.left+cxMax, rect.bottom-cxMax+i);
		}
		pDC->SelectObject(globalData.hpenBtnShadow);
		for (i = 1; i < cxMax; i += 4)
		{
			pDC->MoveTo(rect.left+i, rect.bottom);
			pDC->LineTo(rect.left+cxMax, rect.bottom-cxMax+i);
		}
		for (i = 2; i < cxMax; i += 4)
		{
			pDC->MoveTo(rect.left+i, rect.bottom);
			pDC->LineTo(rect.left+cxMax, rect.bottom-cxMax+i);
		}
		pDC->SelectObject(hPenOld);
	}
}

void CSushiBar::DrawStatusBitmap(HDC hdcDest, CRect rect, HBITMAP hBitmap,
                                 LPCSTR szText, UINT nStyle)
{
	DrawStatusHelper(hdcDest, rect, nStyle);
	
	if (! (nStyle & SBPS_DISABLED))
	{
		CRect rectDraw(rect);
		rectDraw.InflateRect(-2*CX_BORDER, -CY_BORDER);

		HDC hdcSrc = ::CreateCompatibleDC(hdcDest);
		if (hdcSrc != NULL)
		{
			BITMAP bmap;
			::GetObject(hBitmap, sizeof(BITMAP), &bmap);
			HBITMAP hOldBitmap = (HBITMAP) ::SelectObject(hdcSrc, hBitmap);

			BitBlt(hdcDest, rectDraw.left, rectDraw.top, bmap.bmWidth,
			    bmap.bmHeight, hdcSrc, 0, 0, SRCCOPY);

			::SelectObject(hdcSrc, hOldBitmap);
			::DeleteDC(hdcSrc);

			rectDraw.left += bmap.bmWidth + CX_BORDER * 2;
		}

		if (szText != NULL)
		{
			SIZE size;
			if (GetTextExtentPoint32(hdcDest, szText, lstrlen(szText), &size))
			{
				if (size.cx < rectDraw.Width())
					rectDraw.InflateRect((size.cx - rectDraw.Width()) / 3, 0);
			}

			// align on bottom (since descent is more important than ascent)
			SetTextAlign(hdcDest, TA_LEFT | TA_BOTTOM);
			ExtTextOut(hdcDest, rectDraw.left, rectDraw.bottom,
				ETO_CLIPPED, &rectDraw, szText, lstrlen(szText), NULL);
		}
	}
}

void CSushiBar::DrawStatusText(HDC hdcDest, CRect rect, LPCSTR lpszText, UINT nStyle, UINT textWidth, UINT extraSpace /* =0 */)
{
	DrawStatusHelper(hdcDest, rect, nStyle);

	// just support left justified text
	if (lpszText != NULL)
	{
		COLORREF oldColor;
		if (nStyle & SBPS_DISABLED)
			oldColor = SetTextColor(hdcDest, GetSysColor(COLOR_BTNSHADOW));

		CRect rectText(rect);
		rectText.InflateRect(-2*CX_BORDER, -CY_BORDER);

		// align on bottom (since descent is more important than ascent)
		if (m_bScrollingPrompt && rectText.Width() < (INT)textWidth)
		{
			SetTextAlign(hdcDest, TA_RIGHT | TA_BOTTOM);
			ExtTextOut(hdcDest, rectText.right - extraSpace, rectText.bottom,
				ETO_CLIPPED | ETO_OPAQUE, &rectText, lpszText, lstrlen(lpszText), NULL);
		}
		else
		{
			SetTextAlign(hdcDest, TA_LEFT | TA_BOTTOM);
			ExtTextOut(hdcDest, rectText.left + extraSpace, rectText.bottom,
				ETO_CLIPPED | ETO_OPAQUE, &rectText, lpszText, lstrlen(lpszText), NULL);
		}

		if (nStyle & SBPS_DISABLED)
			SetTextColor(hdcDest, oldColor);
	}
}

void CSushiBar::DrawStatusHelper(HDC hdcDest, CRect rect, UINT nStyle)
{
	ASSERT(hdcDest != NULL);
	ASSERT(! (nStyle & SBPS_POPOUT));	// do bitmap indicators need this?

	HBRUSH hbrHilite = NULL;
	HBRUSH hbrShadow = NULL;
	if (!(nStyle & SBPS_NOBORDERS))
	{
		// normal colors
		hbrHilite = ::CreateSolidBrush(::GetSysColor(COLOR_BTNHIGHLIGHT));
		hbrShadow = ::CreateSolidBrush(::GetSysColor(COLOR_BTNSHADOW));
	}

	// background is already grey
	if (nStyle & SBPS_STRETCH)
	{
		SetBkMode(hdcDest, OPAQUE);
		SetTextColor(hdcDest, m_textColor);
		SetBkColor(hdcDest, m_bkColor);
	}
	else
	{
		SetBkMode(hdcDest, TRANSPARENT);
		SetTextColor(hdcDest, ::GetSysColor(COLOR_BTNTEXT));
		SetBkColor(hdcDest, ::GetSysColor(COLOR_BTNFACE));
	}

	// Draw the hilites
	if (hbrHilite != NULL)
	{
		HGDIOBJ hOldBrush = SelectObject(hdcDest, hbrHilite);
		if (hOldBrush)
		{
			PatBlt(hdcDest, rect.right, rect.bottom,
				-(rect.Width() - CX_BORDER),
				-CY_BORDER, PATCOPY);
			PatBlt(hdcDest, rect.right, rect.bottom,
				-CX_BORDER,
				-(rect.Height() - CY_BORDER), PATCOPY);
			SelectObject(hdcDest, hOldBrush);
		}
	}

	if (hbrShadow != NULL)
	{
		HGDIOBJ hOldBrush = SelectObject(hdcDest, hbrShadow);
		if (hOldBrush)
		{
			PatBlt(hdcDest, rect.left, rect.top,
				rect.Width(), CY_BORDER, PATCOPY);
			PatBlt(hdcDest, rect.left, rect.top,
				CX_BORDER, rect.Height(), PATCOPY);
			SelectObject(hdcDest, hOldBrush);
		}
	}

	if (hbrHilite != NULL)
		DeleteObject(hbrHilite);

	if (hbrShadow != NULL)
		DeleteObject(hbrShadow);
}

void CSushiBar::DrawStatusPercentDone(HDC hDC, CRect rect, LPCSTR lpszText,
                                      int percent, LPCSTR lpszExtraText)
{
	ASSERT(lpszText != NULL);

	CSize size;
	// FUTURE: Remove once chicago support GetTextExtentPoint32
	if (!theApp.m_bOnChicago)
		VERIFY( GetTextExtentPoint32(hDC, lpszText, lstrlen(lpszText), &size) );
	else
		// NOTE: GetTextExtentPoint is known to have an off by 1 bug on NT 
		VERIFY( GetTextExtentPoint(hDC, lpszText, lstrlen(lpszText), &size) );

	m_rectCounter = rect;
	m_rectCounter.left += size.cx + m_cxDefaultGap + (4 * CX_BORDER);
	m_rectCounter.right = m_rectCounter.left + (BLOCKWIDTH * 20) + (BLOCKMARGIN * 22);

	DrawStatusHelper(hDC, m_rectCounter, SBPS_NORMAL);

	// Draw leading prompt, left justified
	CRect rectText (rect);
	rectText.right = m_rectCounter.left - m_cxDefaultGap;
	rectText.InflateRect(-2*CX_BORDER, -CY_BORDER);

	// align on bottom (since descent is more important than ascent)
	SetTextAlign(hDC, TA_LEFT | TA_BOTTOM);
	ExtTextOut(hDC, rectText.left, rectText.bottom,
		ETO_CLIPPED | ETO_OPAQUE, &rectText, lpszText, lstrlen(lpszText), NULL);

	// If there's a trailing prompt, draw that too
	if (lpszExtraText != NULL)
	{
		CRect rectText (rect);
		rectText.left = m_rectCounter.right + m_cxDefaultGap;
		rectText.InflateRect(-2*CX_BORDER, -CY_BORDER);

		// align on bottom (since descent is more important than ascent)
		SetTextAlign(hDC, TA_LEFT | TA_BOTTOM);
		ExtTextOut(hDC, rectText.left, rectText.bottom,
			ETO_CLIPPED | ETO_OPAQUE, &rectText, lpszExtraText, lstrlen(lpszExtraText), NULL);
	}

	// Draw a number of blocks appropriate to the current percent done.
	ASSERT(percent <= 100);
	CRect rectBlock (m_rectCounter);
	rectBlock.InflateRect(-BLOCKMARGIN, -4);
	HBRUSH hbrBlue = CreateSolidBrush( RGB(0, 0, 128) );
	for (int i = 0 ; i < percent ; i += 5)
	{
		rectBlock.right = rectBlock.left + BLOCKWIDTH;
		FillRect(hDC, &rectBlock, hbrBlue);
		rectBlock.left = rectBlock.right + BLOCKMARGIN;
	}

	DeleteObject(hbrBlue);
}

void CSushiBar::Reset()
{
	m_position = CPoint(0, 0);
	m_size = CSize(0, 0);
	m_zoom = 100;
	m_line = 0;
	m_column = 0;
	m_bOverStrike = FALSE;
	m_bLockedUntilIdle = FALSE;
	m_bStandardColors = FALSE;

	// Because we can be in nested levels of percent-dones at a time, we
	// use an array to store all the percentages.  Note that this array
	// is indexed by the levels of nesting depth, meaning that position 0
	// is never used.
	for (int i = 0 ; i <= MAXLEVELS ; i++)
		m_rgPercentDone[i] = PERCENT_NIL;
	m_iPercentDone = 0;

	ASSERT(!m_strAfxIdleMessage.IsEmpty());

	// Reset to the current idle prompt [marklam]
	SetIdlePrompt(m_strIdlePrompt);
}

void CSushiBar::SetPosition(const CPoint& pos)
{
	m_position = pos;
	DrawIndicatorNow(ID_INDICATOR_POSITION);
}

void CSushiBar::SetSize(const CSize& size)
{
	m_size = size;
	DrawIndicatorNow(ID_INDICATOR_SIZE);
}

void CSushiBar::SetZoom(int zoom)
{
	m_zoom = zoom;
}

void CSushiBar::SetLineColumn(int line, int col, BOOL fForce /* = FALSE */)
{
	// Optimization: only paint on every third change during scrolling.
	// It will update to the actual correct value when we hit idle.
	BOOL bUpdate = fForce ||
				   ((line != m_line) && (line % 3 == 0)) ||
	               ((col != m_column) && (col % 3 == 0));

	m_line = line;
	m_column = col;

	if (bUpdate)
		DrawIndicatorNow(ID_INDICATOR_LNCOL);
}

void CSushiBar::SetOffsetExtent(long lOffset, long lExtent)
{
	m_offset = lOffset;
	m_extent = lExtent;
}

void CSushiBar::SetOverStrike(BOOL bOverStrike)
{
	m_bOverStrike = bOverStrike;
}

void CSushiBar::GetText(UINT nID, CString& str)
{
	char* pch = str.GetBufferSetLength(30);
	switch (nID)
	{
		case ID_INDICATOR_CLOCK:
			str = m_clock.Format();
			break;
		case ID_INDICATOR_ZOOM:
			sprintf(pch, "%d:1", m_zoom);
			break;
		case ID_INDICATOR_SIZE:
			if (m_size.cx == -32768 && m_size.cy == -32768)
				*pch = 0;
			else
				sprintf(pch, "%d x %d", m_size.cx, m_size.cy);
			break;
		case ID_INDICATOR_POSITION:
			if (m_position.x == -32768 && m_position.y == -32768)
				*pch = 0;
			else
				sprintf(pch, "%d, %d", m_position.x, m_position.y);
			break;
		case ID_INDICATOR_LNCOL:
			{
				CString strPrmpt;
				VERIFY( strPrmpt.LoadString(IDS_LNCOLPRMPT) );
				sprintf(pch, strPrmpt, m_line, m_column);
				break;
			}
		case ID_INDICATOR_OFFSET:
		case ID_INDICATOR_EXTENT:
			{
				// Load the string we used for sizing.
				CString strPrmpt;
				VERIFY(strPrmpt.LoadString(nID));

				// Strip sizing text after the space, and replace with zeros.
				int nSpace = strPrmpt.Find(_TEXT(" "));
				strPrmpt = strPrmpt.Left(nSpace + 1) + _TEXT("000000");

				// Get hex string for actual value.
				_ltoa((nID == ID_INDICATOR_OFFSET) ? m_offset : m_extent, pch, 16);
				str.ReleaseBuffer();
				str.MakeUpper();

				str = strPrmpt.Left(strPrmpt.GetLength() - str.GetLength()) + str;
				return;		// Buffer alread released.
			}
		default:
			ASSERT(FALSE);
			*pch = '\0';
			break;
	}

	str.ReleaseBuffer();
}

BOOL CSushiBar::IsIndicator(UINT nID)
{
	for (int i = 0; i < m_nCount; i++)
	{
		if (nID == GetItemID(i))
			return TRUE;
	}

	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////////
//
// Global Status Bar API
//
// These global functions are used by all project components to access the
// status bar.
//

#define TheStatusBar ((CMainFrame*)(theApp.m_pMainWnd))->m_statusBar

void ShowStatusBar(BOOL bShow /* = TRUE */)
{
	if (theApp.m_pMainWnd)
	{
		TheStatusBar.ShowWindow(bShow ? SW_SHOWNOACTIVATE : SW_HIDE);
		TheStatusBar.OnIdleUpdateCmdUI((WPARAM)TRUE, (LPARAM)0);
	}
}

BOOL IsStatusBarVisible()
{
	if (theApp.m_pMainWnd)
		return (TheStatusBar.GetStyle() & WS_VISIBLE) != 0;
	else
		return FALSE;
}

BOOL IsStatusBarIndicator(UINT nID)
{
	return TheStatusBar.IsIndicator(nID);
}

int GetStatusBarHeight()
{
	CRect rect;
	if (theApp.m_pMainWnd)
		TheStatusBar.GetWindowRect(rect);
	else
		rect.SetRectEmpty();
	return rect.Height();
}

void InvalidateStatusBar(BOOL bErase /* = FALSE */)
{
	if (theApp.m_pMainWnd)
		TheStatusBar.Invalidate(bErase);
}

void ClearStatusBarSize()
{
	if (theApp.m_pMainWnd)
		TheStatusBar.SetSize(CSize(-32768, -32768));
//	if (theApp.m_pMainWnd)
//		g_pMainWnd->m_statBar.ClearSize();
}

void ClearStatusBarPosition()
{
	if (theApp.m_pMainWnd)
		TheStatusBar.SetPosition(CPoint(-32768, -32768));
//	if (theApp.m_pMainWnd)
//		g_pMainWnd->m_statBar.ClearPosition();
}

void ClearStatusBarPositionAndSize()
{
	ClearStatusBarSize();
	ClearStatusBarPosition();
}

void ResetStatusBar()
{
	if (theApp.m_pMainWnd)
		TheStatusBar.Reset();
}

void SetStatusBarPosition(const CPoint& pos)
{
	if (theApp.m_pMainWnd)
		TheStatusBar.SetPosition(pos);
}

void SetStatusBarSize(const CSize& size)
{
	if (theApp.m_pMainWnd)
		TheStatusBar.SetSize(size);
}

void SetStatusBarPositionAndSize(const CRect& rect)
{
	SetStatusBarPosition(((CRect&)rect).TopLeft());
	SetStatusBarSize(rect.Size());
}

void SetStatusBarZoom(int zoom)
{
	if (theApp.m_pMainWnd)
		TheStatusBar.SetZoom(zoom);
}

void SetStatusBarLineColumn(int line, int col, BOOL fForce /* = FALSE */)
{
	if (theApp.m_pMainWnd)
		TheStatusBar.SetLineColumn(line, col, fForce);
}

void SetStatusBarOffsetExtent(long lOffset, long lExtent)
{
	if (theApp.m_pMainWnd)
		TheStatusBar.SetOffsetExtent(lOffset, lExtent);
}

void SetStatusBarOverStrike(BOOL bOverStrike)
{
	if (theApp.m_pMainWnd)
		TheStatusBar.SetOverStrike(bOverStrike);
}

void SetPrompt(UINT nStringID, BOOL bRedrawNow,
               COLORREF textColor, COLORREF bkColor)
{
	// Optimize for the common (AFX_IDS_IDLEMESSAGE) case.
	if (nStringID == AFX_IDS_IDLEMESSAGE)
	{
		ASSERT(textColor == 0);	ASSERT(bkColor == 0);
		SetPrompt(CSushiBar::m_strAfxIdleMessage, bRedrawNow, 0, 0);
	}
	else
	{
	    CString str;
	    VERIFY( str.LoadString(nStringID) );
		SetPrompt(str, bRedrawNow, textColor, bkColor);
	}
}

void SetPrompt(const char* szPrompt, BOOL bRedrawNow,
               COLORREF textColor, COLORREF bkColor)
{
	if (theApp.m_pMainWnd)
	{
		TheStatusBar.SetPrompt(szPrompt);
		TheStatusBar.SetPromptColor(textColor, bkColor);

		if (bRedrawNow)
			TheStatusBar.UpdateWindow();
	}
}

void SetScrollingPrompt(const char* szPrompt, BOOL bRedrawNow,
						COLORREF textColor, COLORREF bkColor)
{
	if (theApp.m_pMainWnd)
	{
		TheStatusBar.SetPrompt(szPrompt, TRUE);
		TheStatusBar.SetPromptColor(textColor, bkColor);

		if (bRedrawNow)
			TheStatusBar.UpdateWindow();
	}
}

void SetIdlePrompt(UINT nStringID, BOOL bRedrawNow)
{
    CString str;
    VERIFY( str.LoadString(nStringID) );
	SetIdlePrompt(str, bRedrawNow);
}

void SetIdlePrompt(const char* szPrompt, BOOL bRedrawNow)
{
	if (theApp.m_pMainWnd)
	{
		TheStatusBar.SetIdlePrompt(szPrompt);

		if (bRedrawNow)
			TheStatusBar.UpdateWindow();
	}
}

void SetPromptUntilIdle(const char* szMessage, COLORREF textColor, COLORREF bkColor)
{
	if (theApp.m_pMainWnd)
	{
		TheStatusBar.SetPromptColor(textColor, bkColor);
		TheStatusBar.SetPromptUntilIdle(szMessage);
	}
}

BOOL IsPromptSetUntilIdle()
{
	return TheStatusBar.IsSetUntilIdle();
}

void StatusBeginPercentDone(const char* szPrompt)
{
	if (theApp.m_pMainWnd)
		TheStatusBar.BeginPercentDone(szPrompt);
}

void StatusEndPercentDone()
{
	if (theApp.m_pMainWnd)
		TheStatusBar.EndPercentDone();
}

void StatusPercentDone(int percentDone)
{
	if (theApp.m_pMainWnd)
		TheStatusBar.PercentDone(percentDone);
}

void StatusOnIdle()
{
	if (theApp.m_pMainWnd)
		TheStatusBar.OnIdle();
}

CString CStatusClock::Format()
{
	CTime now(CTime::GetCurrentTime());

	return theApp.m_CPLReg.Format(now, TIME_NO_SECONDS);
}
