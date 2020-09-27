/////////////////////////////////////////////////////////////////////////////
// portview.cpp
//
// email	date		change
// briancr	10/25/94	created
//
// copyright 1994 Microsoft

// Implementation of the CViewportView class

#include "stdafx.h"
#include "portdoc.h"
#include "portview.h"
#include "caferes.h"
#include "..\support\guitools\testutil.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

/////////////////////////////////////////////////////////////////////////////
// CViewportView

IMPLEMENT_DYNCREATE(CViewportView, CView)

BEGIN_MESSAGE_MAP(CViewportView, CView)
	//{{AFX_MSG_MAP(CViewportView)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
	ON_WM_CREATE()
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_WM_SIZE()
	ON_WM_KEYDOWN()
	ON_WM_LBUTTONDOWN()
	ON_COMMAND(ID_EDIT_MARK, OnEditMark)
	ON_UPDATE_COMMAND_UI(ID_EDIT_MARK, OnUpdateEditMark)
	ON_COMMAND(ID_SELECT_ALL, OnSelectAll)
	ON_UPDATE_COMMAND_UI(ID_SELECT_ALL, OnUpdateSelectAll)
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
	ON_COMMAND_RANGE(IDM_ViewportHeaderFilter, IDM_ViewportStepFilter, OnViewportFilter)
	ON_UPDATE_COMMAND_UI_RANGE(IDM_ViewportHeaderFilter, IDM_ViewportStepFilter, OnUpdateViewportFilter)
	ON_MESSAGE(WM_UPDATE_FROM_PIPE, OnUpdateFromPipe)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CViewportView construction/destruction

CViewportView::CViewportView()
{
	m_cLinesInView = 0;
	m_cxFont = m_cyFont = 0;
	m_rSel1 = m_cSel1 = 0;		// selection starts at top-left
	m_rSel2 = m_cSel2 = 0;		// current line/col == starting line/col by default
	selecting = FALSE;

	m_cxTabInterval = 50;		// set default tab stops
	m_cxTabAdjust = 0;
	for( int iTab = 0; iTab < NUM_TABS; iTab++ )
		m_aTabs[iTab] = -1;
	m_aTabs[0] = 250;
}

CViewportView::~CViewportView()
{
}

int CViewportView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	int iFnt = 0;
	BYTE bFontType = ANSI_CHARSET;

	lpCreateStruct->style |= WS_HSCROLL | WS_VSCROLL;
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;
	// for JPN we are going to pump the font up
	if (GetSystem() & SYSTEM_JAPAN)
	{
		iFnt = 8;
		bFontType = DEFAULT_CHARSET;
	}

	m_font.CreateFont( iFnt,				// height
			0,								// width
			0,								// escapement
			0,								// orientation
			FW_NORMAL,						// weight
			FALSE,							// italic
			FALSE,							// underline
			FALSE,							// strikeout
			bFontType,						// char set
			OUT_DEFAULT_PRECIS,				// out precision
			CLIP_DEFAULT_PRECIS,			// clip precision
			DEFAULT_QUALITY,				// quality
			FIXED_PITCH | FF_DONTCARE,		// pitch and family
			"System" );						// face name
	
  

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// CViewportView drawing


void CViewportView::OnInitialUpdate()
{
	CViewportDoc* pDoc = GetDocument();
	UpdateScrollbars(pDoc->GetWidth(), 0, pDoc->GetWidth(), 0);
}

void CViewportView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
	int nVScrollPos;
	int nUpdateLine;
	CRect rectInvalid;
	CRect rectClient;

	// get a pointer to the document
	CViewportDoc* pDoc = GetDocument();

	if (lHint) {
		m_rSel2 = lHint;
	}
	if(selecting == FALSE)
	{
		m_rSel1 = m_rSel2;
	}

	// put the selected line near the bottom of the window
	nVScrollPos = m_rSel2 - m_cLinesInView + 1;
	if (nVScrollPos < 0) {
		nVScrollPos = 0;
	}
	UpdateScrollbars(pDoc->GetNumLines()-1, nVScrollPos, pDoc->GetWidth(), 0);

	// scroll the window by a line
	if (pDoc->GetNumLines() > m_cLinesInView)
		ScrollWindow(0, -m_sizeFont.cy, NULL, NULL);

	// get the size of the window
	GetClientRect(rectClient);

	// figure the rectangle to invalidate
	
	// get the line we're updating
	// it's either the line passed in or the last line in the window
	// (-1 because we've already scrolled up by a line)
	nUpdateLine = (lHint >= m_cLinesInView) ? m_cLinesInView - 1 : lHint;
	// need to include the previous line (to erase the selection)
	rectInvalid.top = (nUpdateLine-1)*m_sizeFont.cy;
	if (rectInvalid.top < 0)
		rectInvalid.top = 0;
	rectInvalid.left = 0;
	rectInvalid.bottom = (nUpdateLine+1)*m_sizeFont.cy;
	rectInvalid.right = rectClient.right;
	InvalidateRect(rectInvalid, FALSE);

//	Invalidate(FALSE);
	
	UNREFERENCED_PARAMETER(pSender);
	UNREFERENCED_PARAMETER(pHint);
}

LRESULT CViewportView::OnUpdateFromPipe(WPARAM wParam, LPARAM lParam)
{
	OnUpdate(NULL, lParam, NULL);

	return TRUE;

	UNREFERENCED_PARAMETER(wParam);
}


#define LEFT	2		// left margin

void CViewportView::OnDraw(CDC* pDC)
{
	int nTopDataLine;
	int nLeftPos;
	int nTopScreenLine;
	int nBottomScreenLine;
	CRect rectClip;
	int nLine;
	CString strLine;

	CViewportDoc* pDoc = GetDocument();

	int startsel=m_rSel1,endsel=m_rSel2;
	if(m_rSel1 > m_rSel2)
	{
		startsel=m_rSel2;endsel=m_rSel1;
	}

	// determine the range of lines currently visible (scrolled position)
	
	// get the top line into the data from the vertical scroll bar position
	nTopDataLine = GetScrollPos(SB_VERT);
	ASSERT(nTopDataLine >= 0);

	// determine the left position of the lines
	nLeftPos = -GetScrollPos(SB_HORZ);

	// determine the range of lines in the clipping rectangle (zero-based)

	// get the clipping rectangle
	pDC->GetClipBox(rectClip);

	// figure the top and bottom screen lines in the clipping rectangle
	nTopScreenLine = rectClip.top / m_sizeFont.cy;
	nBottomScreenLine = rectClip.bottom / m_sizeFont.cy + 1;

	// paint the background
	// REVIEW(chriskoz): White. Do we need customization here?
	CBrush brWhite(RGB(255, 255, 255));
	pDC->FillRect(&rectClip, &brWhite);
	// selected lines backgroud is "GetSelBkColor()" (gray by default)
	rectClip.top=m_sizeFont.cy * (startsel - nTopDataLine);
	rectClip.bottom=rectClip.top + (endsel-startsel+1)*m_sizeFont.cy;
	CBrush brGray(pDoc->GetSelBkColor());
	pDC->FillRect(&rectClip, &brGray);

	COLORREF OldColor;
	// draw each line in the clipping rectangle
	for (nLine = nTopDataLine + nTopScreenLine; nLine < pDoc->GetNumLines() && (nLine - nTopDataLine) < nBottomScreenLine; nLine++)
	{
		strLine = pDoc->GetLine(nLine);
		if(startsel<=nLine && nLine <= endsel) //inside selection
			OldColor = pDC->SetBkColor(pDoc->GetSelBkColor());
		DrawLine(pDC, nLeftPos, nLine - nTopDataLine, pDoc->GetColor(nLine), strLine);
		if(startsel<=nLine && nLine <= endsel) //inside selection
			pDC->SetBkColor(OldColor);
	}
	// invert the edges of curent line
	rectClip.top = m_sizeFont.cy * (m_rSel2 - nTopDataLine);
	pDC->BitBlt(rectClip.left, rectClip.top, rectClip.Width(), 1, pDC, 0, 0, DSTINVERT);
	pDC->BitBlt(rectClip.left, rectClip.top + m_sizeFont.cy, rectClip.Width(), 1, pDC, 0, 0, DSTINVERT);

}

BOOL CViewportView::DrawLine(CDC* pDC, int nLineX, int nLineY, COLORREF color, CString& strLine)
{
	CFont* pOldFont;
	COLORREF OldColor;
	int x;
	int y;

	// convert the line's x position into window coordinates (based on the font size)
	x = nLineX * m_sizeFont.cx;

	// conver the line's y position into window coordinates (based on the font size)
	y = nLineY * m_sizeFont.cy;

	// select the font to use and the color
	pOldFont = pDC->SelectObject(&m_font);
	OldColor = pDC->SetTextColor(color);

	// draw the text
	pDC->TabbedTextOut(x, y, strLine, strLine.GetLength(), 0, NULL, 0);
//	pDC->TextOut(x, y, strLine);

	// select the old font and color
	pDC->SelectObject(pOldFont);
	pDC->SetTextColor(OldColor);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CViewportView printing

BOOL CViewportView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CViewportView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CViewportView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}




/////////////////////////////////////////////////////////////////////////////
// CViewportView diagnostics

#ifdef _DEBUG
void CViewportView::AssertValid() const
{
	CView::AssertValid();
}

void CViewportView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CViewportDoc* CViewportView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CViewportDoc)));
	return (CViewportDoc*) m_pDocument;
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CViewportView message handlers

void CViewportView::OnEditCopy()
{
	if(m_rSel1>=0 && m_rSel1 < GetDocument()->GetNumLines() &&
		m_rSel2>=0 && m_rSel2 < GetDocument()->GetNumLines() )
	{
		int startsel=m_rSel1,endsel=m_rSel2;
		CString lineset;
		if(m_rSel1 > m_rSel2)
		{
			startsel=m_rSel2;endsel=m_rSel1;
		}
		while(startsel <= endsel)
		{
			lineset=lineset + GetDocument()->GetLine(startsel) + "\r\n";
			startsel++;
		}
		SetClipText(lineset);
		if(m_rSel1 != m_rSel2)
			InvalidateRect(NULL); //REVIEW(chriskoz): do we need to invalidate whole screen?
	}
	m_rSel1=m_rSel2;
	selecting=FALSE; //leaving the selecting mode
}

void CViewportView::OnUpdateEditCopy(CCmdUI* pCmdUI)
{
	// TODO: Add your command update UI handler code here
	
	UNREFERENCED_PARAMETER(pCmdUI);
}

/////////////////////////////////////////////////////////////////////////////
// CViewportView scrolling

void CViewportView::UpdateScrollbars(int vMax, int vPos, int hMax, int hPos)
{
	if( hMax >= 0)
		SetScrollRange(SB_HORZ, 0, hMax < 0 ? 0 : hMax);
	if( hPos >= 0)
		SetScrollPos(SB_HORZ, hPos < 0 ? 0 : hPos);
	if( vMax >= 0)
		SetScrollRange(SB_VERT, 0, vMax < 0 ? 0 : vMax);
	if( vPos >= 0)
		SetScrollPos(SB_VERT, vPos < 0 ? 0 : vPos);
}

void CViewportView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	CViewportDoc* pDoc = GetDocument();

	ASSERT( pScrollBar == NULL );
	int vPos = GetScrollPos(SB_VERT);
	int vNew = vPos;
	switch( nSBCode )
	{
		case SB_LINEDOWN:
			vNew++;
			break;
		case SB_LINEUP:
			vNew--;
			break;
		case SB_PAGEDOWN:
			vNew += m_cLinesInView / 2;
			break;
		case SB_PAGEUP:
			vNew -= m_cLinesInView / 2;
			break;
		case SB_BOTTOM:
			vNew = pDoc->GetNumLines();
			break;
		case SB_TOP:
			vNew = 0;
			break;
		case SB_THUMBPOSITION:
		case SB_THUMBTRACK:
			vNew = nPos;
			break;
		case SB_ENDSCROLL:	// REVIEW(davidga): not quite sure what to do with this
		default:
			return;
	}
	if( vNew > pDoc->GetNumLines() )
		vNew = pDoc->GetNumLines();
	else if( vNew < 0 )
		vNew = 0;

	ASSERT( m_cyFont != 0 );
	UpdateWindow();
	ScrollWindow(0, (vPos - vNew) * m_cyFont, NULL, NULL);
	SetScrollPos(SB_VERT, vNew);

	CView::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CViewportView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	CViewportDoc* pDoc = GetDocument();

	ASSERT( pScrollBar == NULL );
	int hPos = GetScrollPos(SB_HORZ);
	int hNew = hPos;
	switch( nSBCode )
	{
		case SB_LINERIGHT:
			hNew++;
			break;
		case SB_LINELEFT:
			hNew--;
			break;
		case SB_PAGERIGHT:
			hNew += m_cColsInView / 2;
			break;
		case SB_PAGELEFT:
			hNew -= m_cColsInView / 2;
			break;
		case SB_RIGHT:
			if( (hNew = pDoc->GetWidth() - m_cColsInView + 1) < 0 )
				hNew = 0;
			break;
		case SB_LEFT:
			hNew = 0;
			break;
		case SB_THUMBPOSITION:
		case SB_THUMBTRACK:
			hNew = nPos;
			break;
		case SB_ENDSCROLL:	// REVIEW(davidga): not quite sure what to do with this
		default:
			return;
	}
	if( hNew > pDoc->GetWidth() )
		hNew = pDoc->GetWidth();
	else if( hNew < 0 )
		hNew = 0;

	ASSERT( m_cxFont != 0 );
	UpdateWindow();
	ScrollWindow((hPos - hNew) * m_cxFont, 0, NULL, NULL);
	SetScrollPos(SB_HORZ, hNew);

	CView::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CViewportView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);
	
// Compute font size
	CClientDC dc(this);
	CFont* pOldFont = dc.SelectObject(&m_font);
	TEXTMETRIC tm;
	dc.GetTextMetrics(&tm);
	dc.SelectObject(pOldFont);

	m_cyFont = tm.tmHeight;		// + tm.tmInternalLeading;
	ASSERT(m_cyFont != 0);
	m_cxFont = tm.tmMaxCharWidth;

	m_sizeFont.cx = m_cxFont;
	m_sizeFont.cy = m_cyFont;

// Recompute lines in view
	m_cLinesInView = (cy /*+ m_cyFont - 1*/) / m_cyFont;
	m_cColsInView = (cx /*+ m_cxFont - 1*/) / m_cxFont;
}

void CViewportView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	BOOL bControl = GetAsyncKeyState(VK_CONTROL) & 0x8000;
	switch( nChar )
	{
		case VK_UP:
			m_rSel2--;
			SelectLine(m_rSel2);
			break;
		case VK_DOWN:
			m_rSel2++;
			SelectLine(m_rSel2);
			break;
		case VK_PRIOR:
			if( bControl )
				OnHScroll(SB_PAGELEFT, 0, NULL);
			else
				SelectLine(m_rSel2 - (m_cLinesInView - 1));
			break;
		case VK_NEXT:
			if( bControl )
				OnHScroll(SB_PAGERIGHT, 0, NULL);
			else
				SelectLine(m_rSel2 + (m_cLinesInView - 1));
			break;
		case VK_LEFT:
			OnHScroll(SB_LINELEFT, 0, NULL);
			break;
		case VK_RIGHT:
			OnHScroll(SB_LINERIGHT, 0, NULL);
			break;
		case VK_HOME:
			if( bControl )
				SelectLine(0);
			else
				OnHScroll(SB_LEFT, 0, NULL);
			break;
		case VK_END:
			if( bControl )
				SelectLine(GetDocument()->GetNumLines());
			else
				OnHScroll(SB_RIGHT, 0, NULL);
			break;
		default:
			CView::OnKeyDown(nChar, nRepCnt, nFlags);
	}
}

void CViewportView::OnLButtonDown(UINT /*nFlags*/, CPoint point)
{
	CViewportDoc* pDoc = GetDocument();
//	CModelessShapePropSheet* pModelessShapePropSheet
//		= GetModelessShapePropSheet();

	CRect rectClip;
	CClientDC dc(this);

	// get the top line into the data from the vertical scroll bar position
	int nTopDataLine = GetScrollPos(SB_VERT);
	ASSERT(nTopDataLine >= 0);

	// determine the range of lines in the clipping rectangle (zero-based)

	// get the clipping rectangle
	dc.GetClipBox(rectClip);

	int nClickedline = nTopDataLine + (point.y - rectClip.top) / m_sizeFont.cy;
	ASSERT(nClickedline>=0);
	if(pDoc->GetNumLines() < nClickedline)
		return;
	// figure the top and bottom screen lines in the clipping rectangle
//	nTopScreenLine = rectClip.top / m_sizeFont.cy;
//	nBottomScreenLine = rectClip.bottom / m_sizeFont.cy + 1;
	if(m_rSel2 != nClickedline)
	{
		m_rSel2 = nClickedline;
		if(selecting == FALSE)
			m_rSel1 = m_rSel2;
		InvalidateRect(NULL, FALSE);
	}
}


void CViewportView::SelectLine(int nNewLine)
{
	if( nNewLine < 0 )
		nNewLine = 0;
	else if( nNewLine > GetDocument()->GetNumLines() )
		nNewLine = GetDocument()->GetNumLines();

	int nOldLine = m_rSel2;
	m_rSel2 = nNewLine;
	int nScroll = GetScrollPos(SB_VERT);
	if( m_rSel2 < nScroll )
		OnVScroll(SB_THUMBPOSITION, m_rSel2, NULL);
	else if( m_rSel2 > nScroll + m_cLinesInView - 1 /*2*/ )
		OnVScroll(SB_THUMBPOSITION, m_rSel2 - m_cLinesInView + 1/*2*/, NULL);
	if(selecting==FALSE)
		m_rSel1 = m_rSel2;
//REVIEW(chriskoz): need to invalidate more than two lines, but not entire screen
	InvalidateRect(NULL);
//	nScroll = GetScrollPos(SB_VERT);
//	CRect rcLine(0, 0, m_cxFont * (m_cColsInView + 1), 0);
//	rcLine.top = m_cyFont * (m_rSel2 - nScroll);
//	rcLine.bottom = rcLine.top + m_cyFont;
//	InvalidateRect(&rcLine); 

//	rcLine.top = m_cyFont * (nOldLine - nScroll);
//	rcLine.bottom = rcLine.top + m_cyFont;
//	InvalidateRect(&rcLine);
}


void CViewportView::OnViewportFilter(UINT nId)
{
	// get a pointer to the doc
	CViewportDoc* pDoc = GetDocument();

	// toggle the filter setting
	pDoc->SetFilter(nId, !pDoc->GetFilter(nId));
	if(pDoc->GetNumLines() < m_rSel1)
		m_rSel1 = pDoc->GetNumLines();
	if(pDoc->GetNumLines() < m_rSel2)
		m_rSel2 = pDoc->GetNumLines();
	InvalidateRect(NULL); //repaint this view //REVIEW(chriskoz) what about all views
}

void CViewportView::OnUpdateViewportFilter(CCmdUI* pCmdUI)
{
	// get a pointer to the doc
	CViewportDoc* pDoc = GetDocument();

	// update the UI based on the filter setting
	pCmdUI->SetCheck(pDoc->GetFilter(pCmdUI->m_nID));
}

void CViewportView::OnEditMark() 
{
	selecting=TRUE;
}

void CViewportView::OnUpdateEditMark(CCmdUI* pCmdUI) 
{
	// update the UI based on the state of selection
	pCmdUI->Enable(selecting==FALSE);
	
}

void CViewportView::OnSelectAll() 
{
	// TODO: Add your command handler code here
	
}

void CViewportView::OnUpdateSelectAll(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	
}
