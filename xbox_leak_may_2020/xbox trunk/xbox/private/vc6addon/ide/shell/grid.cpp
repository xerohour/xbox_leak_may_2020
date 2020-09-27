// grid.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "imeutil.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

#define SORT_COLUMNS	// undefine to prevent column sorting
//#define DYNAMIC_SIZING // undefine if column sizing should happen on button up

#define cxSelectMargin 8

extern CFont* GetStdFont(const int);

///////////////////////////////////////////////////////////////////////////////
//	CGridToolBar

CGridToolBar::CGridToolBar()
	: CToolBar()
{
}

void CGridToolBar::SetSizes()
{
	CToolBar::SetSizes(CSize(sizeButtonX, sizeButtonY),
	                   CSize(sizeImageX, sizeImageY));

	m_cxDefaultGap = 0;
	m_cyTopBorder = m_cyBottomBorder = 0;
	m_cxLeftBorder = 0;
	m_dwStyle = m_dwStyle & ~0x0F00;

}
///////////////////////////////////////////////////////////////////////////////
//	CStaticVCenter

void CStaticVCenter::OnPaint()
{
	CPaintDC dc(this);
	CRect rc;
	CString str;

	GetClientRect(rc);

	dc.SetBkMode( TRANSPARENT );

	if (IsWindowEnabled())
		dc.SetTextColor(::GetSysColor(COLOR_WINDOWTEXT));
	else
		dc.SetTextColor(::GetSysColor(COLOR_GRAYTEXT));

	COLORREF bkColor = GetSysColor(COLOR_BTNFACE);
	if (theApp.m_bWin4 && GetRValue(bkColor) == 0 && GetGValue(bkColor) == 0 && GetBValue(bkColor) == 0)
		bkColor = GetSysColor(COLOR_WINDOW);

	// this all to avoid problems with disabled control text drawing. this sets the bkcolor in the dc too
	dc.FillSolidRect(rc, bkColor);

	CBrush brWindow(bkColor);
	CBrush * pbrushOld = dc.SelectObject(&brWindow);

	CFont* pfontOld = dc.SelectObject(GetStdFont(font_Normal));

	rc.left += 2;  // seems to be the norm... bobz
	GetWindowText(str);
	dc.DrawText(str, str.GetLength(), rc, DT_LEFT | DT_VCENTER | DT_SINGLELINE );

	dc.SelectObject(pfontOld);
	dc.SelectObject(pbrushOld);

}

void CStaticVCenter::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpParams)
{
	// NT won't draw sunken border
	if (!UseWin4Look())
	{
		LPRECT pRect = &(lpParams->rgrc[0]);
		::InflateRect(pRect, -1, -1); // make room for borders
		return;
	}
        
	CStatic::OnNcCalcSize(bCalcValidRects, lpParams);
}

void CStaticVCenter::OnNcPaint()
{
	// NT won't draw sunken border
	if (!UseWin4Look())
	{
		// Draw sunken border
	    CWindowDC dc(this);

		CRect rect;
	    GetWindowRect(rect);
		rect.OffsetRect(-rect.left, -rect.top); // essentially convert to client coords for draw

		dc.Draw3dRect(rect, GetSysColor(COLOR_BTNSHADOW), GetSysColor(COLOR_BTNHIGHLIGHT));
		return;
	}

	CStatic::OnNcPaint();
}

BEGIN_MESSAGE_MAP(CStaticVCenter, CStatic)
	//{{AFX_MSG_MAP(CStaticVCenter)
	ON_WM_PAINT()
	ON_WM_NCCALCSIZE()
	ON_WM_NCPAINT()

	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CGridElement

CGridElement::~CGridElement()
{
}

void CGridElement::SetCapture()
{
	CGridWnd* pGrid = GetGrid();
	pGrid->m_pCaptureElement = this;
	pGrid->SetCapture();
}

void CGridElement::ReleaseCapture()
{
	CGridWnd* pGrid = GetGrid();
	if (pGrid->m_pCaptureElement == this)
	{
		pGrid->m_pCaptureElement = NULL;
		::ReleaseCapture();
	}
}

void CGridElement::Invalidate(BOOL bErase, BOOL bInflate)
{
	CGridWnd* pGrid = GetGrid();
	if (pGrid->m_hWnd == NULL)
		return;
	
	CRect rect;
	GetRect(rect);
	if (bInflate)
		rect.InflateRect(0, 3);

	pGrid->InvalidateRect(rect, bErase);
}

void CGridElement::OnSelect(BOOL bSelected)
{
}

void CGridElement::ResetSize(CDC* pDC)
{
}

CGridElement* CGridElement::ElementFromPoint(const CPoint& point)
{
	return this;
}

BOOL CGridElement::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_bWasSelected = GetGrid()->IsSelected(this);

	return FALSE;
}

BOOL CGridElement::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	GetGrid()->SetActiveElement(this, GetGrid()->ColumnFromX(point.x));
	return TRUE;
}

BOOL CGridElement::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_bWasSelected)
	{
		GetGrid()->SetActiveElement(this, GetGrid()->ColumnFromX(point.x));
	}
	else {
		GetGrid()->Select(this, nFlags & (MK_SHIFT | MK_CONTROL));
	}

	return TRUE;
}

BOOL CGridElement::OnMouseMove(UINT nFlags, CPoint point)
{
	return FALSE;
}

void CGridElement::OnActivate(BOOL bActivate, int nColumn)
{
}

void CGridElement::OnControlNotify(UINT nControlID, UINT nNotification)
{
}

CGridRow* CGridElement::GetRow() const
{
	ASSERT(this != NULL);
	ASSERT(!m_bIsCell);

	return (CGridRow*)this;
}

/////////////////////////////////////////////////////////////////////////////
// CGridCell

CGridCell::CGridCell()
{
	m_size.cx = m_size.cy = 0;
	m_pRow = NULL;
	m_nColumn = 0;
	m_dt = DT_WORDBREAK;
	m_bIsCell = TRUE;
}

void CGridCell::GetRect(CRect& cellRect)
{
	CGridWnd* pGrid = GetGrid();
	CGridRow* pRow = GetRow();
	pRow->GetRect(cellRect);
	for (int nColumn = 0; nColumn < m_nColumn; nColumn += 1)
		cellRect.left += pGrid->GetColumnWidth(nColumn);
	cellRect.right = cellRect.left + pGrid->GetColumnWidth(m_nColumn);
}

void CGridCell::Invalidate(BOOL bErase)
{
	CGridWnd* pGrid = GetGrid();
	if (pGrid->m_hWnd != NULL)
	{
		CRect cellRect;
		GetRect(cellRect);
		pGrid->InvalidateRect(cellRect, bErase);
	}
}

void CGridCell::SetText(const char* szText)
{
	if (m_str.Compare(szText) != 0)
	{
		m_str = szText;
		Invalidate();
	}
}

void CGridCell::Draw(CDC* pDC, const CRect& cellRect)
{
	CGridWnd* pGrid = GetGrid();
	BOOL bCurCell = pGrid->LooksSelected(this);
	COLORREF oldTextColor = pDC->SetTextColor(bCurCell ? pGrid->GetHighlightTextColor():pGrid->GetWindowTextColor());
	COLORREF oldBkColor = pDC->SetBkColor(bCurCell ? pGrid->GetHighlightColor() : pGrid->GetWindowColor());
	
	CRect rect = cellRect;
	rect.right -= 1;
	rect.bottom -= 1;
	
	pDC->ExtTextOut(0, 0, ETO_OPAQUE, &rect, "", 0, NULL);

	rect.InflateRect(-1, -1);
	pDC->DrawText(m_str, m_str.GetLength(), rect, DT_NOPREFIX | m_dt);
	rect.InflateRect(1, 1);
	
	CBrush brush;
	brush.CreateSolidBrush(GetSysColor(COLOR_BTNFACE));
	CBrush* pOldBrush = pDC->SelectObject(&brush);
	pDC->PatBlt(cellRect.left, cellRect.bottom - 1, cellRect.Width(), 1, PATCOPY);
	pDC->PatBlt(cellRect.right - 1, cellRect.top, 1, cellRect.Height() - 1, PATCOPY);
	pDC->SelectObject(pOldBrush);
		
	CWnd* pFocus = CWnd::GetFocus();
	if ((pFocus == pGrid /*|| pGrid->IsChild(pFocus)*/) && pGrid->IsPrimary(this))
		pDC->DrawFocusRect(rect);
	
	pDC->SetTextColor(oldTextColor);
	pDC->SetBkColor(oldBkColor);
}

void CGridCell::ResetSize(CDC* pDC)
{
	CRect rect(0, 0, GetGrid()->GetColumnWidth(GetColumnIndex()), 0);
	if (rect.Width() <= 0)
	{
		m_size.cx = m_size.cy = 0;
	}
	else
	{
		pDC->DrawText(m_str, m_str.GetLength(), rect, DT_CALCRECT | DT_NOPREFIX | m_dt);
		m_size = rect.Size();
		m_size.cx += 3;
		m_size.cy += 3;
	}
}

CGridRow* CGridCell::GetRow() const
{
	ASSERT(this != NULL);
	ASSERT(m_bIsCell);

	return m_pRow;
}


/////////////////////////////////////////////////////////////////////////////
// CGridRow

CGridRow::CGridRow(CGridWnd* pGrid)
{
	m_pGrid = pGrid;
	m_cyHeight = 0;
	m_bIsCell = FALSE;
}

CGridRow::~CGridRow()
{
	if (m_pGrid != NULL)
		m_pGrid->RemoveRow(this);
}

CSize CGridRow::GetCellSize(CDC* pDC, int nColumn) const
{
	return GetGrid()->GetCaptionRow()->GetCellSize(nColumn);
}

void CGridRow::GetRect(CRect& rowRect)
{
	CGridWnd* pGrid = GetGrid();
	pGrid->GetClientRect(rowRect);
	
	int yTopVisible, yThis;
	int y = 0;
	BOOL bFoundThis = FALSE;
	BOOL bFoundTop = FALSE;
	POSITION pos = pGrid->GetHeadRowPosition();
	if (pGrid->m_posTopVisibleRow == NULL)
		pGrid->m_posTopVisibleRow = pos;

	while (pos != NULL && !(bFoundThis && bFoundTop))
	{
		if (pos == pGrid->m_posTopVisibleRow)
		{
			bFoundTop = TRUE;
			yTopVisible = y;
		}
		
		CGridRow* pRow = pGrid->GetNextRow(pos);
		if (pRow == this)
		{
			bFoundThis = TRUE;
			yThis = y;
		}
		
		y += pRow->GetRowHeight();
	}
	
	ASSERT(bFoundThis && bFoundTop);
	
	rowRect.top = yThis - yTopVisible - pGrid->m_cyScroll +
		pGrid->GetCaptionRow()->GetRowHeight();
	rowRect.bottom = rowRect.top + GetRowHeight();
}

void CGridRow::Draw(CDC* pDC, const CRect& rowRect)
{
	CGridWnd* pGrid = GetGrid();
	
	COLORREF oldTextColor;
	COLORREF oldBkColor;

	if (!pGrid->m_bHasCells)
	{
		BOOL bCurRow = pGrid->LooksSelected(this);
		oldTextColor = pDC->SetTextColor(bCurRow ? pGrid->GetHighlightTextColor():pGrid->GetWindowTextColor());
		oldBkColor = pDC->SetBkColor(bCurRow ? pGrid->GetHighlightColor() : pGrid->GetWindowColor());
	}
	
	CRect cellRect = rowRect;
	int nColumnCount = pGrid->GetColumnCount();
	for (int nColumn = 0; nColumn < nColumnCount; nColumn += 1)
	{
		cellRect.right = cellRect.left + pGrid->GetColumnWidth(nColumn);
		if (cellRect.right > cellRect.left)
			DrawCell(pDC, cellRect, nColumn);
		cellRect.left = cellRect.right;
	}
	
	if (!pGrid->m_bHasCells)
	{
		oldTextColor = pDC->SetTextColor(oldTextColor);
		oldBkColor = pDC->SetBkColor(oldBkColor);
		
		CWnd* pFocus = CWnd::GetFocus();
		if ((pFocus == pGrid /*|| pGrid->IsChild(pFocus)*/) && pGrid->IsPrimary(this))
			pDC->DrawFocusRect(rowRect);
	}
}

int CGridRow::Compare(const CGridRow* pCompareRow) const
{
	return 0; // don't know, so claim they're equal!
}

/////////////////////////////////////////////////////////////////////////////
// CGridControlRow

CGridControlRow::CGridControlRow(CGridWnd* pGrid) : CGridRow(pGrid)
{
}

CGridControlRow::~CGridControlRow()
{
}

void CGridControlRow::GetColumnRect(int nColumn, CRect& rect)
{
	CGridWnd* pGrid = GetGrid();
	int x = 0;
	for (int n = 0; n < nColumn; n += 1)
		x += pGrid->GetColumnWidth(n);
	GetRect(rect);
	rect.left = x;
	rect.right = x + pGrid->GetColumnWidth(nColumn);
}

#define cxDefaultGridCellMargin 8
#define cyDefaultGridCellMargin 2

void CGridControlRow::AdjustForMargins(CRect& rect, int nColumn)
{
	if (nColumn == 0)
	{
		int cx = max(cxDefaultGridCellMargin, GetGrid()->m_cxSelectMargin);

		rect.right -= cxDefaultGridCellMargin;
		rect.left += cx;
		rect.InflateRect(0, -cyDefaultGridCellMargin);
	}
	else
	{
		rect.InflateRect(-cxDefaultGridCellMargin, -cyDefaultGridCellMargin);
	}
}

void CGridControlRow::DrawCell(CDC* pDC, const CRect& cellRect, int nColumn)
{
	CRect textRect = cellRect;
	AdjustForMargins(textRect, nColumn);
	CString str;
	GetColumnText(nColumn, str);
	
	int x;
	switch (pDC->GetTextAlign() & (TA_LEFT | TA_RIGHT | TA_CENTER))
	{
	default:
		ASSERT(FALSE);
		
	case TA_LEFT:
		x = textRect.left;
		break;
		
	case TA_CENTER:
		x = textRect.left + textRect.Width() / 2;
		break;
		
	case TA_RIGHT:
		x = textRect.right;
		break;
	}
	
	pDC->ExtTextOut(x, textRect.top, ETO_CLIPPED | ETO_OPAQUE,
		cellRect, str, str.GetLength(), NULL);
}

CEdit* CGridControlRow::NewEdit( )
{
	return( new CEdit ) ;
}

CComboBox* CGridControlRow::NewCombo( )
{
	return( new CComboBox );
}

// Called when the grid is about to create a control in the cell.
// The CRect argument has the rectangle corr to the cell
// and is adjusted to give the dimensions of the control.

void CGridControlRow::AdjustControlRect( GRID_CONTROL_TYPE controlType, int nColumn, CRect& rect  )
{
	switch ( controlType )
	{
		default:
			ASSERT(FALSE);
			break;
		case none:
			break;
		case edit:
			// FUTURE: hacky numbers won't work for all sys fonts
			rect.left -= 4;
			rect.top -= 4;
			rect.right += 4;
			rect.bottom += 4;
			break;
		case editCombo:
			// FUTURE: hacky numbers won't work for all sys fonts
			rect.left -= 3;
			rect.top -= 3;
			rect.right += 3;
			rect.bottom = rect.top + 150;
			break;
		case listCombo:
			// FUTURE: hacky numbers won't work for all sys fonts
			rect.left -= 3;
			rect.top -= 3;
			rect.right += 3;
			rect.bottom = rect.top + 150;
			break;						
	}
}

int GetEllipseWidth()
{
	static int cxWidth;
	if (cxWidth == 0)
	{
		CWindowDC dc(NULL);
		cxWidth = dc.GetTextExtent("...W", 4).cx;
	}

	return cxWidth;
}

BOOL CGridControlRow::CreateControl(GRID_CONTROL_TYPE controlType, int nColumn, DWORD dwStyle)
{
	CGridControlWnd* pGrid = (CGridControlWnd*)GetGrid();
	BOOL bDestroyedControl = FALSE;
	
	if (pGrid->m_pControlWnd != NULL && controlType != pGrid->m_controlType)
	{
		delete pGrid->m_pControlWnd;
		pGrid->m_pControlWnd = NULL;
		pGrid->m_controlType = none;
		pGrid->m_nControlColumn = -1;
		bDestroyedControl = TRUE;

		if (pGrid->m_Ellipse.m_hWnd != NULL)
			pGrid->m_Ellipse.DestroyWindow();
	}
	
	ASSERT(nColumn >= 0 || controlType == none);
	CRect rect;
	if (nColumn >= 0)
	{
		GetColumnRect(nColumn, rect);
		AdjustForMargins(rect, nColumn);
	}
	
	if (pGrid->m_pControlWnd == NULL)
	{
		CRect rcEllipse;
		switch (controlType)
		{
		default:
			ASSERT(FALSE);
			
		case none:
			break;
		
		case edit:
			AdjustControlRect(controlType, nColumn, rect);
			if (pGrid->m_bEnableEllipse)
			{
				rcEllipse = rect;
				rect.right -= GetEllipseWidth();
				rcEllipse.left = rect.right;
			}

			pGrid->m_pControlWnd = NewEdit();
			if ((dwStyle & ES_MULTILINE) != 0)
			{
				dwStyle |= ES_AUTOVSCROLL;
				rect.bottom++;		// multiline edit's draw text 1 pixel lower
			}
			else
				dwStyle |= ES_AUTOHSCROLL;
			if (!((CEdit*)pGrid->m_pControlWnd)->Create(WS_BORDER | dwStyle, rect, pGrid, 1))
			{
				delete pGrid->m_pControlWnd;
				pGrid->m_pControlWnd = NULL;
				return FALSE;
			}

			if (pGrid->m_bEnableEllipse)
				pGrid->m_Ellipse.Create("...", BS_PUSHBUTTON, rcEllipse, pGrid, 2);
	
			ASSERT(pGrid->m_pControlWnd->GetDlgCtrlID() == 1);
			break;
			
		case editCombo:
			AdjustControlRect( controlType, nColumn, rect);			
			pGrid->m_pControlWnd = NewCombo() ;

			if (!((CComboBox*)pGrid->m_pControlWnd)->Create(
				CBS_DROPDOWN | CBS_AUTOHSCROLL | WS_VSCROLL | dwStyle, rect, pGrid, 1))
			{
				delete pGrid->m_pControlWnd;
				pGrid->m_pControlWnd = NULL;
				return FALSE;
			}
			break;

		case listCombo:
			AdjustControlRect( controlType, nColumn, rect);			
			pGrid->m_pControlWnd = NewCombo() ;

			if (!((CComboBox*)pGrid->m_pControlWnd)->Create(
				CBS_DROPDOWNLIST | CBS_AUTOHSCROLL | WS_VSCROLL | dwStyle, rect, pGrid, 1))
			{
				delete pGrid->m_pControlWnd;
				pGrid->m_pControlWnd = NULL;
				return FALSE;
			}
			break;
		}
	}
	else
	{
		CRect rcEllipse;
		pGrid->m_pControlWnd->ShowWindow(SW_HIDE);

		AdjustControlRect(controlType, nColumn, rect);
		if (pGrid->m_bEnableEllipse)
		{
			rcEllipse = rect;
			rect.right -= GetEllipseWidth();
			rcEllipse.left = rect.right;
		}

		pGrid->m_pControlWnd->MoveWindow(rect);

		if (pGrid->m_bEnableEllipse)
		{
			ASSERT(pGrid->m_Ellipse.m_hWnd != NULL);
			pGrid->m_Ellipse.ShowWindow(SW_HIDE);
			pGrid->m_Ellipse.MoveWindow(rcEllipse);
		}
	}
	
	if (pGrid->m_pControlWnd != NULL)
	{
		pGrid->m_pControlWnd->SetFont(pGrid->GetFont( ));
		pGrid->m_pControlWnd->SetFocus();
					
		CString str;
		GetColumnText(nColumn, str);
		
		switch (controlType)
		{
		case edit:
			pGrid->m_pControlWnd->SetWindowText(str);
			((CEdit*)pGrid->m_pControlWnd)->SetSel(0, -1);
			break;
			
		case editCombo:
		case listCombo:
			AddListItems(nColumn, (CComboBox*) pGrid->m_pControlWnd);
#ifdef _WIN32			
			pGrid->m_pControlWnd->SetWindowText(str);
#else			
			// Win16 dropdown lists don't respond to WM_GETWINDOWTEXT
			((CComboBox*)pGrid->m_pControlWnd)->SelectString(-1, str);
#endif			
			((CComboBox*)pGrid->m_pControlWnd)->SetEditSel(32767, 32767);
			break;
		}

		pGrid->m_pControlWnd->ShowWindow(SW_SHOWNA);
		if (pGrid->m_bEnableEllipse && pGrid->m_Ellipse.m_hWnd != NULL)
			pGrid->m_Ellipse.ShowWindow(SW_SHOWNA);
	}
	else
	{
		// If we destroyed a control, set the focus back to the grid.
		if ( bDestroyedControl && (GetFocus() == NULL))
			pGrid->SetFocus();
	}
	
	pGrid->m_controlType = controlType;
	if (nColumn != -1)
		pGrid->m_nControlColumn = nColumn;
	
	return TRUE;
}

void CGridControlRow::OnControlNotify(UINT nControlID, UINT nNotification)
{
	CGridControlWnd* pGrid = (CGridControlWnd*)GetGrid();
		CWnd* pParent = pGrid->GetParent();
		ASSERT_VALID(pParent);
	
	if (nControlID == 2)
	{
		if (nNotification == BN_CLICKED)
			pParent->PostMessage(WM_COMMAND, MAKEWPARAM(pGrid->GetDlgCtrlID(), GLN_ELLIPSE), (LPARAM)pGrid->m_hWnd);

		return; 
	}

	ASSERT(nControlID == 1);
	switch (pGrid->m_controlType)
	{
		case edit:
			if (nNotification == EN_KILLFOCUS || nNotification == EN_SETFOCUS)
				pParent->PostMessage(WM_COMMAND,
									 MAKEWPARAM(pGrid->GetDlgCtrlID(), nNotification == EN_KILLFOCUS ? GLN_KILLFOCUS : GLN_SETFOCUS),
									 (LPARAM)pGrid->m_hWnd);
				
			if (nNotification != EN_CHANGE)
				return;
			break;

		case editCombo:
		case listCombo:
			if (nNotification == CBN_KILLFOCUS || nNotification == CBN_SETFOCUS)
				pParent->PostMessage(WM_COMMAND,
									 MAKEWPARAM(pGrid->GetDlgCtrlID(), nNotification == CBN_KILLFOCUS ? GLN_KILLFOCUS : GLN_SETFOCUS),
									 (LPARAM)pGrid->m_hWnd);

			if (nNotification != CBN_SELENDCANCEL)
				return;
			break;

		default:
			return;
	}

	CString strOld;
	GetColumnText(pGrid->m_nControlColumn, strOld);

	CString strNew;
#ifdef _WIN32	
	pGrid->m_pControlWnd->GetWindowText(strNew);
#else
 	// Stupid, annoying, innane Win16 DROPDOWNLISTs don't support WM_GETTEXTLENGTH so
 	// we have to do something else instead
 	if (pGrid->m_controlType == listCombo)
 	{
 		CComboBox* pcombo = (CComboBox*) pGrid->m_pControlWnd;
 		pcombo->GetLBText(pcombo->GetCurSel(), strNew);
 	}
 	else
 		pGrid->m_pControlWnd->GetWindowText(strNew);
#endif	
	

	if (strNew.Compare(strOld) == 0)
		return;

	OnChangeColumnText(pGrid->m_nControlColumn, strNew);

	switch (pGrid->m_controlType)
	{
	case edit:
		{
			int cyOldHeight = m_cyHeight;
			
			CClientDC dc(GetGrid());
			CFont* pOldFont = dc.SelectObject(pGrid->GetFont( ));
			ResetSize(&dc);
			dc.SelectObject(pOldFont);
			
			if (m_cyHeight != cyOldHeight)
			{
				GetGrid()->Invalidate(FALSE);
				
				CRect rect;
				GetColumnRect(pGrid->m_nControlColumn, rect);
				AdjustForMargins(rect, pGrid->m_nControlColumn);
				// FUTURE: hacky numbers won't work for all sys fonts
				rect.left -= 4;
				rect.top -= 4;
				rect.right += 4;
				rect.bottom += 4;
				pGrid->m_pControlWnd->MoveWindow(rect);
			
				// Hack alert!  Since we have to have ES_AUTOSCROLL set to prevent the
				// edit from just beeping when the cursor is at the bottom-right and
				// the user types, the edit has auto-scrolled when we have to resize the
				// window.  Here we scroll it back...
				((CEdit*)pGrid->m_pControlWnd)->LineScroll(-10, 0);
			}
		}
		break;
		
	case editCombo:
	case listCombo:
		// FUTURE: NYI
		break;
	}
}

BOOL CGridControlRow::OnAccept(CWnd* pControlWnd)
{
	return TRUE;
}

int CGridControlRow::GetNextControlColumn(int nColumn)
{
	int nColumnCount = GetGrid()->GetColumnCount();
	return nColumn == nColumnCount - 1 ? 0 : nColumn + 1;
}

int CGridControlRow::GetPrevControlColumn(int nColumn)
{
	int nColumnCount = GetGrid()->GetColumnCount();
	return nColumn <= 0 ? nColumnCount - 1 : nColumn - 1;
}

void CGridControlRow::OnChangeColumnText(int nColumn, const char* szText)
{
}

void CGridControlRow::AddListItems(int nColumn, CComboBox* pComboBox)
{

}

/////////////////////////////////////////////////////////////////////////////
// CGridCellRow

CGridCellRow::CGridCellRow(CGridWnd* pGrid) : CGridRow(pGrid)
{
}

CGridCellRow::~CGridCellRow()
{
	int nColumnCount = GetGrid()->GetColumnCount();
	for (int nColumn = 0; nColumn < nColumnCount; nColumn += 1)
		delete GetCell(nColumn);
}

void CGridCellRow::DrawCell(CDC* pDC, const CRect& cellRect, int nColumn)
{
	CGridCell* pCell = GetCell(nColumn);
	pCell->Draw(pDC, cellRect);
}

void CGridCellRow::ResetSize(CDC* pDC)
{
	int nColumnCount = GetGrid()->GetColumnCount();
	int cyHeight = 4; // FUTURE: minimum row height...
	for (int nColumn = 0; nColumn < nColumnCount; nColumn += 1)
	{
		CGridCell* pCell = GetCell(nColumn);
		pCell->ResetSize(pDC);
		int cyCell = pCell->GetSize().cy;
		if (cyCell > cyHeight)
			cyHeight = cyCell;
	}
	
	m_cyHeight = cyHeight;
}

int CGridCellRow::Compare(const CGridRow* pCompareRow) const
{
	int nSortColumn = GetGrid()->m_nSortColumn;
	return GetCell(nSortColumn)->m_str.Compare(((CGridCellRow*)pCompareRow)->GetCell(nSortColumn)->m_str);
}

BOOL CGridCellRow::CreateCells()
{
	int nColumnCount = GetGrid()->GetColumnCount();
	for (int nColumn = 0; nColumn < nColumnCount; nColumn += 1)
	{
		CGridCell* pCell = new CGridCell;
		pCell->m_pRow = this;
		pCell->m_nColumn = nColumn;
		m_cells.Add(pCell);
	}
	
	return TRUE;
}

CGridElement* CGridCellRow::ElementFromPoint(const CPoint& point)
{
	int x = 0;
	for (int nColumn = 0; nColumn < GetGrid()->GetColumnCount(); nColumn += 1)
	{
		x += GetGrid()->GetColumnWidth(nColumn);
		if (point.x < x)
			break;
	}
	
	if ( nColumn == GetGrid()->GetColumnCount() )
		return NULL;
	else
		return GetCell(nColumn);
}


/////////////////////////////////////////////////////////////////////////////
// CGridCaptionCell

CGridCaptionCell::CGridCaptionCell(BOOL bDisplay)
{
	m_bPressed = FALSE;
	m_bFixedWidth = FALSE;
	m_trackZone = none;
	m_dt &= ~DT_WORDBREAK;
}

CGridCaptionCell::HitZone CGridCaptionCell::GetHitZone(const CPoint& point)
{
	CRect cellRect;
	GetRect(cellRect);
	
	if (GetColumnIndex() > 0 && point.x < cellRect.left + 2 )
	{
		// Exclude fixed width columns.
		if (!((CGridCaption *)GetRow())->IsColumnFixedWidth(GetColumnIndex() - 1))
			return resizeLeft;
	}
	
	if (GetColumnIndex() < GetGrid()->GetColumnCount() - 1 &&
		point.x > cellRect.right - 3)
	{
		if ( !m_bFixedWidth )
			return resizeRight;
	}
	
	return select;
}

void CGridCaptionCell::InvertResizeTracker(CDC* pDC)
{
#ifndef DYNAMIC_SIZING
	BOOL bReleaseDC = FALSE;
	if (pDC == NULL)
	{
		bReleaseDC = TRUE;
		pDC = GetGrid()->GetDC();
	}
		
	// NYI: select a 50% halftone brush
	
	pDC->PatBlt(m_xSizeTracker, 0, 2, 32767, PATINVERT);
	
	if (bReleaseDC)
		GetGrid()->ReleaseDC(pDC);
#endif
}

BOOL CGridCaptionCell::OnLButtonDown(UINT nFlags, CPoint point)
{
	SetCapture();
	m_trackZone = GetHitZone(point);
	switch (m_trackZone)
	{
	case resizeLeft:
	case resizeRight:
		m_xSizeTracker = point.x;
		GetGrid()->UpdateWindow();
		InvertResizeTracker();
		break;
		
	case select:
#ifdef SORT_COLUMNS
		if (GetGrid()->m_bSort)
		{	
			m_bPressed = TRUE;
			Invalidate();
		}
#endif		
		break;
	}
	
	return TRUE;
}

BOOL CGridCaptionCell::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	int nColumn = GetColumnIndex();
	
	switch (GetHitZone(point))
	{
	case resizeLeft:
		if (nColumn > 0)
			GetGrid()->SizeColumnToFit(nColumn - 1);
		break;
		
	case resizeRight:
		GetGrid()->SizeColumnToFit(nColumn);
		break;
	}
	
	return TRUE;
}

void CGridCaptionCell::SizeFromTracker()
{
	InvertResizeTracker();

	int nColumn = GetColumnIndex();
	if (m_trackZone == resizeLeft)
	{
		ASSERT(nColumn > 0);
		nColumn -= 1;
	}
			
	CGridWnd* pGrid = GetGrid();
	CGridCell* pLeftCell = pGrid->GetCaptionRow()->GetCell(nColumn);
	CGridCell* pRightCell = pGrid->GetCaptionRow()->GetCell(nColumn + 1); // may be NULL
			
	CRect cellRect;
	pLeftCell->GetRect(cellRect);
	int dx = m_xSizeTracker - cellRect.right;

	pLeftCell->m_size.cx += dx;
			
	if (pRightCell != NULL)
		pRightCell->m_size.cx -= dx;
			
	pGrid->ResetSize();
	pGrid->Invalidate(FALSE);
	pGrid->ResetScrollBars();
}

BOOL CGridCaptionCell::OnLButtonUp(UINT nFlags, CPoint point)
{
	ReleaseCapture();
		
	switch (m_trackZone)
	{
	case resizeRight:
	case resizeLeft:
#ifndef DYNAMIC_SIZING
		SizeFromTracker();
#endif
		break;
		
	case select:
		if (m_bPressed)
		{
#ifdef SORT_COLUMNS	
			if (GetGrid()->m_bSort)
			{	
				m_bPressed = FALSE;
				Invalidate();
				GetGrid()->SortColumn(GetColumnIndex(), (nFlags & MK_SHIFT) != 0);
			}
#endif			
		}
		break;
	}
	
	m_trackZone = none;
	
	return TRUE;
}

BOOL CGridCaptionCell::OnMouseMove(UINT nFlags, CPoint point)
{
	switch (m_trackZone == none ? GetHitZone(point) : m_trackZone)
	{
	case resizeLeft:
	case resizeRight:
		SetCursor(AfxGetApp()->LoadCursor(IDC_SIZE_BAR_HORZ));
		break;
				
	default:
		SetCursor(LoadCursor(NULL, IDC_ARROW));
		break;
	}
	
	switch (m_trackZone)
	{
	case none:
		// Just set the cursor (already done)...
		break;

	case resizeLeft:
	case resizeRight:
		// The user is resizing a column...
		{
			CGridWnd* pGrid = GetGrid();
			CRect rect;
			GetRect(rect);
			if (m_trackZone == resizeLeft)
				rect.left -= pGrid->GetColumnWidth(GetColumnIndex() - 1);
			else
				rect.right += pGrid->GetColumnWidth(GetColumnIndex() + 1);
			
			InvertResizeTracker();
			
			m_xSizeTracker = point.x;
			if (m_xSizeTracker < rect.left)
				m_xSizeTracker = rect.left;
			else if (m_xSizeTracker > rect.right)
				m_xSizeTracker = rect.right;

			InvertResizeTracker();

#ifdef DYNAMIC_SIZING
			SizeFromTracker();
			pGrid->UpdateWindow();
#endif
		}
		break;
		
	case select:
		{
			// The user clicked in the "button" so we track the mouse
			// to see if they let go in there too..
#ifdef SORT_COLUMNS		
			if ( GetGrid()->m_bSort )		
			{
				CRect cellRect;
				GetRect(cellRect);
				if (cellRect.PtInRect(point))
				{
					if (!m_bPressed)
					{
						m_bPressed = TRUE;
						Invalidate();
					}
				}
				else
				{
					if (m_bPressed)
					{
						m_bPressed = FALSE;
						Invalidate();
					}
				}
			}
#endif					
		}
		break;
	}
	
	return TRUE;
}

void CGridCaptionCell::Draw(CDC* pDC, const CRect& cellRect)
{
	CBrush whiteBrush;
	if (!whiteBrush.CreateSolidBrush(GetSysColor(COLOR_BTNHIGHLIGHT)))
		return;

	CBrush blackBrush;
	if (!blackBrush.CreateSolidBrush(GetSysColor(COLOR_BTNTEXT)))
		return;

	CBrush* pOldBrush = pDC->SelectObject(m_bPressed ? &blackBrush : &whiteBrush);
	pDC->PatBlt(cellRect.left, cellRect.top, cellRect.Width() - 1, 1, PATCOPY);
	pDC->PatBlt(cellRect.left, cellRect.top + 1, 1, cellRect.Height() - 2, PATCOPY);
	
	pDC->SelectObject(m_bPressed ? &whiteBrush : &blackBrush);
	pDC->PatBlt(cellRect.right - 1, cellRect.top, 1, cellRect.Height(), PATCOPY);
	pDC->PatBlt(cellRect.left, cellRect.bottom - 1, cellRect.Width(), 1, PATCOPY);
	
	pDC->SelectObject(pOldBrush);
	
	COLORREF oldBkColor = pDC->SetBkColor(GetSysColor(COLOR_BTNFACE));
	COLORREF oldTextColor = pDC->SetTextColor(GetSysColor(COLOR_BTNTEXT));
	CSize textSize = pDC->GetTextExtent(m_str, m_str.GetLength());
	
	CRect rect = cellRect;
	rect.InflateRect(-1, -1); // border
	
	int xText = rect.left + 1;
	int cxText = pDC->GetTextExtent(m_str, m_str.GetLength()).cx;
	if ((m_dt & DT_CENTER) != 0)
	{
		xText += (rect.Width() - cxText) / 2;
	}
	else if ((m_dt & DT_RIGHT) != 0)
	{
		xText = rect.right - 1 - cxText;
	}
	pDC->ExtTextOut(xText + m_bPressed, rect.top + 1 + m_bPressed,
		ETO_OPAQUE | ETO_CLIPPED, &rect, m_str, m_str.GetLength(), NULL);
	pDC->SetBkColor(oldBkColor);
	pDC->SetTextColor(oldTextColor);
}

void CGridCaptionCell::ResetSize(CDC* pDC)
{
	CRect rect(0, 0, GetGrid()->GetColumnWidth(GetColumnIndex()), 0);
	if (rect.Width() <= 0)
	{
		m_size.cx = m_size.cy = 0;
	}
	else
	{
		m_size = rect.Size();
		pDC->DrawText(m_str, m_str.GetLength(), rect, DT_CALCRECT | DT_NOPREFIX | m_dt);
		m_size.cy = rect.Height() + 3;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CGridCaption

CGridCaption::CGridCaption(CGridWnd* pGrid, BOOL bDisplay)
	: CGridCellRow(pGrid)
{
	int nColumnCount = pGrid->GetColumnCount();
	for (int nColumn = 0; nColumn < nColumnCount; nColumn += 1)
	{
		CGridCaptionCell* pCell = new CGridCaptionCell(bDisplay);
		pCell->m_pRow = this;
		pCell->m_nColumn = nColumn;
		m_cells.Add(pCell);
	}

	m_bDisplay = bDisplay;
}

void CGridCaption::GetRect(CRect& rowRect)
{
	GetGrid()->GetClientRect(rowRect);
	rowRect.bottom = rowRect.top + m_cyHeight;
}

void CGridCaption::ResetSize(CDC* pDC)
{
	if (!m_bDisplay)
	{
		m_cyHeight = 0;
	}
	else
	{
		int nColumnCount = GetGrid()->GetColumnCount();
		int cyHeight = 4; // FUTURE: minimum row height...
		for (int nColumn = 0; nColumn < nColumnCount; nColumn += 1)
		{
			CGridCell* pCell = GetCell(nColumn);
			pCell->ResetSize(pDC);
			int cyCell = pCell->GetSize().cy;
			if (cyCell > cyHeight)
				cyHeight = cyCell;
		}
	
		m_cyHeight = cyHeight;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CGridWnd


CGridWnd::CGridWnd(int nColumns, BOOL bDisplayCaption )
{
	m_nColumnCount = nColumns;
	m_cyScroll = 0;
	m_posTopVisibleRow = NULL;
	m_pCaptureElement = NULL;
	
	m_posCurRow = NULL;
	m_nCurColumn = 0;
	m_bShowSelection = TRUE;

	m_nSortColumn = -1; // not sorted
	m_bReverseSort = FALSE;
	
	m_bHasCells = FALSE;
	
	m_bDragSelecting = FALSE;
	m_bDragRow = FALSE;
	m_pDropRow = NULL;
	m_pFirstSelect = NULL;

	m_pFont = NULL ;
	m_pCaptionFont = NULL ;

	m_bSysDefaultColors = TRUE;
		
	m_pActiveElement = NULL;

	m_fMultiSelect = TRUE;
	m_bSort = TRUE;
	m_bAutoSort = TRUE;
	m_bShowContextPopup = FALSE;
	m_bShowProperties = FALSE;
	m_bDestruct = FALSE;
	m_bEnableDragRow = FALSE;
	m_bEnableEllipse = FALSE;

	m_bBtnDownValid = FALSE;  // only used in LBUTTONDOWN processing

	m_cxSelectMargin = cxSelectMargin;
	
	m_rows.AddHead(new CGridCaption(this, bDisplayCaption));
	m_pToolBar = NULL;

}

CGridWnd::~CGridWnd()
{
	if (m_pToolBar != NULL)
	{
		delete m_pToolBar;
		m_pToolBar = NULL;
	}

	// Clearing these speeds up row deletion
	m_selection.RemoveAll();
	m_posCurRow = NULL;
	m_posTopVisibleRow = NULL;
	m_cyScroll = 0;

	m_bDestruct = TRUE;
	while (!m_rows.IsEmpty())
		delete (CGridRow*)m_rows.GetHead();
}

BOOL CGridWnd::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	return CreateEx(0, dwStyle, rect, pParentWnd, nID);
}

BOOL CGridWnd::CreateEx(DWORD dwExStyle, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	return CWnd::CreateEx(dwExStyle, AfxRegisterWndClass(CS_DBLCLKS), NULL, dwStyle,
	                      rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top,
	                      pParentWnd->GetSafeHwnd(), (HMENU)nID, NULL);
}

UINT rgidBtnGrid [] =
{
	ID_GRD_NEW,
	ID_GRD_DELETE,
	ID_GRD_MOVEUP,
	ID_GRD_MOVEDOWN,
};

BOOL CGridWnd::ReplaceControl(CWnd* pWnd, UINT nIDTemp, UINT nIDGrid,
	DWORD dwStyle /*= WS_VISIBLE | WS_CHILD | WS_VSCROLL | WS_TABSTOP*/, 
	DWORD dwExStyle /*= WS_EX_CLIENTEDGE */,
	DWORD dwTBStyle /* = GRIDWND_TB_NOTOOLBAR */,
	BOOL bRemovePlaceholder /* = TRUE */)
{
	// Get the place holder window
	CWnd* pReplace = pWnd->GetDlgItem(nIDTemp);
	if (pReplace == NULL)
		return FALSE;
	ASSERT_VALID(pReplace);
	
	// Get the correct location and size
	CRect rcGrid;
	CRect rcStatic;
	pReplace->GetWindowRect(rcGrid);
	pWnd->ScreenToClient(rcGrid);
	rcStatic = rcGrid;

	CString strLabel;
	pReplace->GetWindowText(strLabel);

	// we layout the text at least if present
	BOOL fNeedStatic =  (dwTBStyle != GRIDWND_TB_NOTOOLBAR || !strLabel.IsEmpty());

	if (fNeedStatic)
	{    		
		CRect rcToolBar;
		if (dwTBStyle != GRIDWND_TB_NOTOOLBAR)
		{
			CWnd *pwndParent = pWnd;

			int cBtnToolBar = 0;
			int iRgidBtnGrid = 0;
			BOOL fDelOnly = (dwTBStyle == GRIDWND_TB_DELETE);
			UINT nIDBitmap = (fDelOnly ? IDR_GRD_TB_DELETE : IDR_GRD_TOOLBAR);
				
			if (dwTBStyle & GRIDWND_TB_NEW)
				cBtnToolBar++;
			if (dwTBStyle & GRIDWND_TB_DELETE)
			{
				cBtnToolBar++;
				if (!(dwTBStyle & GRIDWND_TB_NEW))
					iRgidBtnGrid = 1;
			}
			
			if (dwTBStyle & GRIDWND_TB_MOVE)
			{
				cBtnToolBar += 2;
				if (!(dwTBStyle & (GRIDWND_TB_NEW | GRIDWND_TB_DELETE)))
					iRgidBtnGrid = 2;
			}
			
			if( !pwndParent->IsKindOf(RUNTIME_CLASS(CDialog)))
			{
				ASSERT(FALSE); // only grid controls in dialogs should be using toolbars. We will ignore flag otherwise bobz
				return (FALSE);
			}

			if (m_pToolBar == NULL)
				m_pToolBar = new CGridToolBar;

			// create as child of dialog. Will set owner to grid later
			DWORD dwStyleTB = WS_CHILD | WS_VISIBLE | CBRS_ALIGN_TOP | CBRS_TOOLTIPS | CBRS_FLYBY;
			if (m_pToolBar == NULL)
				{
				TRACE("Failed to Create/Init ToolBar\n");
				return FALSE;
				}

			if	(m_pToolBar->m_hWnd == NULL)
				if (m_pToolBar->Create(pwndParent, dwStyleTB) == 0)
				{
					TRACE("Failed to Create/Init ToolBar\n");
					return FALSE;
				}

			if (!m_pToolBar->LoadBitmap(nIDBitmap) ||
				!m_pToolBar->SetButtons(&rgidBtnGrid[iRgidBtnGrid], cBtnToolBar))
			{
				TRACE("Failed to Create/Init ToolBar\n");
				return FALSE;
			}
		
			m_pToolBar->SetSizes();
			// force tooltips
			m_pToolBar->EnableToolTips(TRUE);

			// reposition toolbar outside of parent window. at corner of grid
			// at create time toolbar size is set to 0. Use the rect of the first and last button to
			// determine toolbar size

			CRect rcFirst;
			CRect rcLast;
			CSize csizeTB;
			CSize csizeText;
			int cyStatic;

			rcFirst.SetRectEmpty();
			rcLast.SetRectEmpty();

			m_pToolBar->GetItemRect(0, rcFirst);
			ASSERT(cBtnToolBar > 0);
			m_pToolBar->GetItemRect(cBtnToolBar - 1, rcLast);
			// rcLast.right is end of last btn from start of toolbar (includes left border). 
			// Add in rcFirst.left to get end border width
			csizeTB.cx = rcLast.right + rcFirst.left;
			csizeTB.cy = rcFirst.Height() + 2 * rcFirst.top;   // top offset is border height. 2 for top and bottom

			CWindowDC dc(pReplace);
			CFont* pfontOld = dc.SelectObject(GetStdFont(font_Normal));
			csizeText = dc.GetTextExtent(strLabel, 1); // get font height
			dc.SelectObject(pfontOld);
			
			// height of window enclosing toolbar based on max of toolbar and font
			cyStatic = max(csizeText.cy, csizeTB.cy);
			int cyOffset = (cyStatic - csizeTB.cy) / 2; // so we center in window
			
			// NOTE: bobz these offsets will only work if the static text has a sunken border
#define cyBorderTop (1)
#define cxBorderRight (1)
			
			// center toolbar vertically in static window
			rcToolBar.top = rcGrid.top + cyOffset + cyBorderTop;
			rcToolBar.right = rcGrid.right - cxBorderRight;
			rcToolBar.left = rcToolBar.right - csizeTB.cx;
			rcToolBar.bottom = rcToolBar.top + csizeTB.cy;

			// adjust rcGrid to exclude the toolbar. 2 for the borders.
			rcGrid.top += cyStatic + (cyBorderTop * 2);

			// position the toolbar 
			m_pToolBar->SetWindowPos(pReplace, rcToolBar.left, rcToolBar.top,
				rcToolBar.Width(), rcToolBar.Height(), SWP_NOACTIVATE);
		}
		else  // no toolbar, but leave room for text. Use height of caption row
		{
			rcGrid.top += GetCaptionRow()->GetRowHeight();
		}

		// create a CStaticVCenter window from the placeholder, with same text and border styles
		// size will be based on what we have now
		// make it a child of the dialog, so it can be outside the grid
		
		rcStatic.bottom = rcGrid.top;
		
		if (!m_cstatVCenter.Create(strLabel, (pReplace->GetStyle() | WS_CLIPSIBLINGS) , rcStatic, pWnd))
		{
			return FALSE;
		}

		m_cstatVCenter.SetFont(GetStdFont(font_Normal));
	}

	// Create the grid

		// NT won't draw clientedge border, so force in normal border
	if (!UseWin4Look())
	{
		if (dwExStyle & WS_EX_CLIENTEDGE)
			dwStyle |= WS_BORDER;
	}

	if (!CreateEx(dwExStyle, dwStyle, rcGrid, pWnd, nIDGrid))
		return FALSE;
	
	// set the control help ID on the grid to be the ID that the placeholder had.
	DWORD dwHelpID = pReplace->GetWindowContextHelpId();
	if (dwHelpID)
		SetWindowContextHelpId(dwHelpID);

	// tab order of controls: toolbar, static, grid. Position in reverse order, all
	// with respect to pReplace

	// Fix the grid control's Z-Order
	SetWindowPos(pReplace, 0, 0, 0, 0,
		SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOREDRAW | SWP_NOSIZE);
	
	if (fNeedStatic)
	{
				// Fix the static control's Z-Order
		m_cstatVCenter.SetWindowPos(pReplace, 0, 0, 0, 0,
			SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOREDRAW | SWP_NOSIZE);
	}

	// establish toolbar chain of command
	if (m_pToolBar != NULL)
	{
		m_pToolBar->SetOwner(this);
					// Fix the toolbar control's Z-Order
		m_pToolBar->SetWindowPos(pReplace, 0, 0, 0, 0,
			SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOREDRAW | SWP_NOSIZE);
	}


	if (bRemovePlaceholder)
			pReplace->DestroyWindow();

	return TRUE;
}



IMPLEMENT_DYNAMIC(CGridWnd, CWnd)

BEGIN_MESSAGE_MAP(CGridWnd, CWnd)
	//{{AFX_MSG_MAP(CGridWnd)
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONUP()
	ON_WM_KEYDOWN()
	ON_WM_SYSKEYDOWN()
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_WM_SIZE()
	ON_WM_VSCROLL()
	ON_WM_TIMER()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTA, 0x0000, 0xffff, OnToolTipText)
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTW, 0x0000, 0xffff, OnToolTipText)
	ON_WM_SHOWWINDOW()
	ON_WM_ENABLE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGridWnd message handlers

int CGridWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if ( m_pFont == NULL )
	{	
		m_pFont = GetStdFont(FALSE);
	}
	if ( m_pCaptionFont == NULL )
	{
		m_pCaptionFont = GetStdFont(TRUE);
	}

	CWnd* pParent = GetParent();
	if (pParent != NULL)
	{
		if (pParent->IsKindOf(RUNTIME_CLASS(CDialog)))
			m_bParentIsDialog = TRUE;
		else
			m_bParentIsDialog = FALSE;
	}
	else
	{
		ASSERT (FALSE);
		m_bParentIsDialog = FALSE;
	}


	ResetSize();
	
	return 0;
}

// NOTE: Only works if point is on or below a visible row!
CGridElement* CGridWnd::ElementFromPoint(const CPoint& point)
{
	CGridRow* pRow = GetCaptionRow();
	if (point.y < pRow->GetRowHeight())
		return pRow->ElementFromPoint(point);
	
	int y = pRow->GetRowHeight() - m_cyScroll;
	for (POSITION posRow = m_posTopVisibleRow; posRow != NULL; )
	{
		pRow = GetNextRow(posRow);
		y += pRow->GetRowHeight();
		if (point.y < y)
			return pRow->ElementFromPoint(point);
	}
	
	return NULL;
}

void CGridWnd::SetFont( CFont *pNewFont,  BOOL bRedraw /* TRUE */  )
{
	m_pFont = pNewFont ;
	
	if ( m_hWnd != NULL &&  bRedraw )
 	{
		ResetSize( );
		Invalidate( );
		ResetScrollBars( );
	}
}

void CGridWnd::SetCaptionFont( CFont *pNewFont,  BOOL bRedraw /* TRUE */  )
{

	m_pCaptionFont = pNewFont ;

	if ( m_hWnd != NULL && bRedraw )
 	{
		ResetSize( );
		Invalidate( );																				
		ResetScrollBars( );
	}
}

void CGridWnd::SetColor(COLORREF colorText, COLORREF colorBk, COLORREF colorHighlight,
					COLORREF colorHighlightText)
{
	m_bSysDefaultColors = FALSE;
	m_colorWindowText = colorText;
	m_colorWindow = colorBk;
	m_colorHighlightText = colorHighlightText;
	m_colorHighlight = colorHighlight;

	if (m_hWnd != NULL)
		Invalidate();
}

void CGridWnd::EnableMultiSelect( BOOL fEnable /* = TRUE */ )
{
	m_fMultiSelect = fEnable;
}

void CGridWnd::EnableAutoSort( BOOL bEnable /* = TRUE */ )
{
	m_bAutoSort = bEnable;
}

void CGridWnd::EnableSort( BOOL bEnable /* = TRUE */ )
{
	m_bSort = bEnable;
}
	
BOOL CGridWnd::BeginDrag(UINT nFlags, const CPoint& point)
{
	// Override this function to provide drag'n'drop functionality...
	// This will be called from OnLButtonDown with the same flags and
	// mouse point.  Return TRUE if your grid is going to initiate a
	// drag'n'drop, or FALSE to use the default (extend selection).

	if (m_bEnableDragRow)
	{
		// Begin a row drag
		m_bDragSelecting = FALSE;
		m_bDragRow = TRUE;
		m_pDropRow = NULL;
		SetCapture();

		DWORD dwTimeout; // NT Defines this lparam as LPDWORD
		SystemParametersInfo(SPI_GETKEYBOARDSPEED, 0, &dwTimeout, FALSE);
		SetTimer(0, dwTimeout, NULL);

		return TRUE;
	}
	return FALSE;
}

void CGridWnd::DoDragDrop(CPoint point)
{
	// NOP
}

BOOL CGridWnd::ProcessKeyboard(MSG* pMsg, BOOL bPreTrans /*= FALSE*/)
{
	if (pMsg->message == WM_KEYDOWN && GetHeadRowPosition() != NULL)
	{
		UINT nChar = (UINT)pMsg->wParam;
		UINT nRepCnt = (UINT)LOWORD(pMsg->lParam);
		UINT nFlags = (UINT)HIWORD(pMsg->lParam);
		BOOL bShift = GetKeyState(VK_SHIFT) < 0;
		BOOL bControl = GetKeyState(VK_CONTROL) < 0;

		switch (nChar)
		{

		case VK_ESCAPE:
			ClearSelection();
			break;

		case VK_UP:
			{
				if (m_bHasCells)
				{
					if (m_posCurRow == NULL)
					{
						m_posCurRow = GetHeadRowPosition();
						Select(GetCellRowAt(m_posCurRow)->GetCell(m_nCurColumn));
					}
					else if (m_posCurRow != GetHeadRowPosition())
					{
						GetPrevRow(m_posCurRow);
						Select(GetCellRowAt(m_posCurRow)->GetCell(m_nCurColumn),
							bShift || bControl);
					}
				}
				else
				{
					if (m_posCurRow == NULL)
					{
						m_posCurRow = GetHeadRowPosition();
						Select(GetRowAt(m_posCurRow));
					}
					else if (m_posCurRow != GetHeadRowPosition())
					{
						GetPrevRow(m_posCurRow);
						Select(GetRowAt(m_posCurRow), bShift || bControl);
					}
				}
			}
			return TRUE;

		case VK_DOWN:
			{
				if (m_bHasCells)
				{
					if (m_posCurRow == NULL)
					{
						m_posCurRow = GetHeadRowPosition();
						Select(GetCellRowAt(m_posCurRow)->GetCell(m_nCurColumn));
					}
					else if (m_posCurRow != m_rows.GetTailPosition())
					{
						GetNextRow(m_posCurRow);
						Select(GetCellRowAt(m_posCurRow)->GetCell(m_nCurColumn),
							bShift || bControl);
					}
				}
				else
				{
					if (m_posCurRow == NULL)
					{
						m_posCurRow = GetHeadRowPosition();
						Select(GetRowAt(m_posCurRow));
					}
					else if (m_posCurRow != m_rows.GetTailPosition())
					{
						GetNextRow(m_posCurRow);
						Select(GetRowAt(m_posCurRow), bShift || bControl);
					}
				}
			}
			return TRUE;
	
		case VK_LEFT:
			if (m_bHasCells)
			{
				if  (m_nCurColumn > 0)
				{
					Select(GetCellRowAt(m_posCurRow)->GetCell(m_nCurColumn - 1),
						bShift || bControl);
				}
				return TRUE;
			}
			break;
	
		case VK_RIGHT:
			if (m_bHasCells)
			{
				if ( m_nCurColumn < GetColumnCount() - 1 )
				{
					Select(GetCellRowAt(m_posCurRow)->GetCell(m_nCurColumn + 1),
						bShift || bControl);
				}
				return TRUE;
			}
			break;

		case VK_HOME:
		case VK_END:
			{
				UINT nColumn = (nChar == VK_HOME)? 0 : GetColumnCount();
				 
				if (bControl || !m_bHasCells)
				{
					m_posCurRow = (nChar == VK_HOME) ? 
									GetHeadRowPosition() : m_rows.GetTailPosition();

					CGridElement * pElementTo;

					if (m_bHasCells)
						pElementTo = GetCellRowAt(m_posCurRow)->GetCell(nColumn);
					else
						pElementTo = GetRowAt(m_posCurRow);

					if (bShift && GetHeadSelPosition() != NULL)
					{
						CGridElement * pElementFrom = (CGridElement *)m_selection.GetHead();
						SelectBetween(pElementFrom, pElementTo); 
					}

					Select(pElementTo, bShift);	 // Will ensure this is the primary selection.
				}
				else if (m_bHasCells)
				{
					Select(GetCellRowAt(m_posCurRow)->GetCell(nColumn), bShift);
				}			
				return TRUE;
			}

		case VK_NEXT:
		case VK_PRIOR:
			CRect rect;
			POSITION pos;
			int yCurPos, y;
			
			if (bControl)
				break;

			// Get the y coord (ignoring caption rows) of the selection
			if (!m_posCurRow)
				m_posCurRow = m_posTopVisibleRow;
			ASSERT( m_posCurRow );
			ScrollIntoView( GetRowAt(m_posCurRow) );

			pos = m_posTopVisibleRow;
			yCurPos = 0;
			while (pos != m_posCurRow)
			{
				yCurPos += GetRowAt(pos)->GetRowHeight();
				GetNextRow(pos);
			}

			// Scroll by a screenfull
			GetClientRect( &rect );
			VScroll( (rect.bottom - GetCaptionRow()->GetRowHeight()) * ((nChar == VK_NEXT) ? 1 : -1));

			// Set selection
			int yScrollPos = GetScrollPos(SB_VERT);
			int yScrollMin, yScrollLim;
			GetScrollRange(SB_VERT, &yScrollMin, &yScrollLim);

			if ((yScrollPos == yScrollMin) && (nChar == VK_PRIOR))
			{
				m_posCurRow = GetHeadRowPosition();
			}
			else if ((yScrollPos == yScrollLim) && (nChar == VK_NEXT))
			{
				m_posCurRow = m_rows.GetTailPosition();
			}
			else
			{
				pos = m_posTopVisibleRow;
				y = 0;
				while (pos && (y < yCurPos))
				{
					y += GetRowAt(pos)->GetRowHeight();
					GetNextRow(pos);
				}
				m_posCurRow = pos;
				if (!m_posCurRow)
					m_posCurRow = m_posTopVisibleRow;
			}

			ASSERT( m_posCurRow );
			if (m_bHasCells)
				Select(GetCellRowAt(m_posCurRow)->GetCell(m_nCurColumn), bShift || bControl);
			else
				Select(GetRowAt(m_posCurRow), bShift || bControl);
			return TRUE;
		}
	} 

	if (bPreTrans)
		return CWnd::PreTranslateMessage(pMsg);
	else
		return FALSE;
}

void CGridWnd::AdjustDropRow(CGridRow* &rpDropRow)
{
}

void CGridWnd::InvalidateSelection(CGridElement* pElement)
{
	if (pElement != NULL)
	{
		// We have a new primary element...
		
		pElement->Invalidate();
		
		if (m_selection.GetCount() > 1)
		{
			// Invalidate the old primary as well
			POSITION pos = m_selection.GetHeadPosition();
			m_selection.GetNext(pos);
			((CGridElement*)m_selection.GetAt(pos))->Invalidate();
		}
	}
	else
	{
		// Invalidate the whole selection...
		
		POSITION pos = m_selection.GetHeadPosition();
		while (pos != NULL)
			((CGridElement*)m_selection.GetNext(pos))->Invalidate(FALSE, TRUE);
	}
}

void CGridWnd::ScrollIntoView(CGridRow* pRow)
{
	CRect rowRect;
	pRow->GetRect(rowRect);
	
	CRect view;
	GetClientRect(view);
	view.top += GetCaptionRow()->GetRowHeight();
	
	if (rowRect.top < view.top)
	{
		VScroll(rowRect.top - view.top);
	}
	else if (rowRect.bottom >= view.bottom)
	{
		VScroll(rowRect.bottom - view.bottom);
	}
}

void CGridWnd::ClearSelection()
{
	SetActiveElement(NULL);
	InvalidateSelection();

	POSITION pos = m_selection.GetHeadPosition();
	while (pos != NULL)
		((CGridElement*)m_selection.GetNext(pos))->OnSelect(FALSE);

	m_selection.RemoveAll();
}

void CGridWnd::ShowSelection(BOOL bShow /* =TRUE */)
{ 
	if (m_bShowSelection == bShow)
		return;

	m_bShowSelection = bShow;
	InvalidateSelection();
}

void CGridWnd::Deselect(CGridElement* pElement)
{
	POSITION pos = m_selection.Find(pElement);
	if (pos != NULL)
	{
		if (pElement == m_pActiveElement)
			SetActiveElement(NULL);
		
		pElement->Invalidate();
		pElement->OnSelect(FALSE);
		m_selection.RemoveAt(pos);
		InvalidateSelection(NULL);
	}
}

void CGridWnd::Select(CGridElement* pElement, BOOL bAdd)
{
	CloseActiveElement();
	if (!bAdd || !m_fMultiSelect)	 // single select
		ClearSelection();
	
	POSITION pos = m_selection.Find(pElement);
	if (pos != NULL)
		m_selection.RemoveAt(pos);
	
	m_selection.AddHead(pElement);
	pElement->OnSelect(TRUE);
	
	CGridRow* pRow = pElement->GetRow();
	if (pElement->m_bIsCell)
	{
		CGridCell* pCell = (CGridCell*)pElement;
		m_nCurColumn = pCell->GetColumnIndex();
	}

	// Update the keyboard position
	if (m_posCurRow == NULL || GetRowAt(m_posCurRow) != pRow)
		m_posCurRow = m_rows.Find(pRow);

	ScrollIntoView(pRow);
	InvalidateSelection(pElement);
}

void CGridWnd::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	ProcessKeyboard((MSG*)GetCurrentMessage());
}

void CGridWnd::OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	MSG *pMsg = (MSG *)GetCurrentMessage();
	if(!ProcessKeyboard(pMsg))
		DefWindowProc(pMsg->message, pMsg->wParam, pMsg->lParam);
}

BOOL CGridWnd::SubclassDlgItem(UINT nID, CWnd* pParent)
{
	if (!CWnd::SubclassDlgItem(nID, pParent))
		return FALSE;
	
	return TRUE;
}


void CGridWnd::SetColumnCaption(int nColumn, const char* szCaption)
{
	GetCaptionRow()->GetCell(nColumn)->SetText(szCaption);
}

void CGridWnd::SetColumnWidth(int nColumn, int cxWidth)
{
	GetCaptionRow()->GetCell(nColumn)->m_size.cx = cxWidth;
	ResetSize();
	
	if (m_hWnd)
		Invalidate(); // FUTURE: Invalidate selectively
}

POSITION CGridWnd::GetHeadRowPosition() const
{
	POSITION pos = m_rows.GetHeadPosition();
	m_rows.GetNext(pos); // first one is caption row
	return pos;
}

void CGridWnd::OnPaint()
{
	CPaintDC dc(this);
	
	CGridRow* pRow;
	CRect rowRect;
	
	GetClientRect(rowRect);

	// Draw the captions first
	pRow = GetCaptionRow();
	rowRect.bottom = rowRect.top + pRow->m_cyHeight;
	if (dc.m_ps.rcPaint.top < rowRect.bottom)
	{
		CFont* pOldFont = dc.SelectObject(GetCaptionFont( ));
		pRow->Draw(&dc, rowRect);
		dc.SelectObject(pOldFont);
	}
	
	CFont* pOldFont = dc.SelectObject(GetFont( ));
	
	// Setup clip rect around the actual grid (client less captions)
	rowRect.top = rowRect.bottom;
	rowRect.bottom = dc.m_ps.rcPaint.bottom;
	dc.IntersectClipRect(rowRect);

	// Figure out where to start drawing
	POSITION pos = m_posTopVisibleRow;
	if (pos == NULL)
	{
		// Need to kickstart things
		pos = m_posTopVisibleRow = GetHeadRowPosition();
		m_cyScroll = 0;
	}
	
	CRect activeRowRect(0, 0, 0, 0);
	
	// Draw each row in turn
	rowRect.top -= m_cyScroll;
	while (pos != NULL)
	{
		CGridRow* pRow = GetNextRow(pos);
		rowRect.bottom = rowRect.top + pRow->m_cyHeight;
		if (rowRect.bottom > dc.m_ps.rcPaint.top)
			pRow->Draw(&dc, rowRect);
		
		if ((pRow == m_pActiveElement) && LooksSelected(pRow))
			activeRowRect = rowRect;
		
		rowRect.top = rowRect.bottom;
		
		if (rowRect.top >= dc.m_ps.rcPaint.bottom)
			break; // we're done!
	}
	
	// Erase the bottom of the window
	if (rowRect.top < dc.m_ps.rcPaint.bottom)
	{
		rowRect.bottom = dc.m_ps.rcPaint.bottom;
		dc.SetBkColor(GetWindowColor());
		dc.ExtTextOut(0, 0, ETO_OPAQUE, rowRect, NULL, 0, NULL);
	}
	
	if (!activeRowRect.IsRectEmpty())
	{
		CRect rect;
		dc.SetBkColor(GetHighlightColor());
		rect.left = activeRowRect.left;
		rect.top = activeRowRect.top - 3;
		rect.right = activeRowRect.right;
		rect.bottom = activeRowRect.top;
		dc.ExtTextOut(0, 0, ETO_OPAQUE, rect, NULL, 0, NULL);
		rect.top = activeRowRect.bottom;
		rect.bottom = activeRowRect.bottom + 3;
		dc.ExtTextOut(0, 0, ETO_OPAQUE, rect, NULL, 0, NULL);
	}
	
	dc.SelectObject(pOldFont);

	// Draw the drop marker
	if (m_pDropRow != NULL && m_pDropRow != GetCaptionRow())
	{
		BOOL bTop = TRUE;
		POSITION posSel = GetHeadSelPosition();
		if (posSel != NULL)
		{
			CGridElement* pHeadSelElem = GetNextSel(posSel);
			CGridRow* pHeadSel = pHeadSelElem->GetRow();

			POSITION pos = GetHeadRowPosition();
			while (pos != NULL)
			{
				CGridRow* pRow = GetNextRow(pos);
				ASSERT(pRow != NULL);

				if (pRow == pHeadSel)
				{
					bTop = FALSE;
					break;
				}
				else if (pRow == m_pDropRow)
				{
					break;
				}
			}
		}

		// BLOCK: Draw the drop marker
		{
			CBrush* pHalftone = dc.GetHalftoneBrush();
			CRect rcRow, rc;
			m_pDropRow->GetRect(rcRow);
			rcRow.left += m_cxSelectMargin / 2;
			rcRow.right -= m_cxSelectMargin / 2;

			int y = bTop ? rcRow.top - 1: rcRow.bottom - 1;
			m_rcDropInvalidate.SetRect(rcRow.left, y - 5,
			                           rcRow.right + 1, y + 8);

			// Draw the main line
			rc.SetRect(rcRow.left, y,
			           rcRow.right + 1, y + 3);
			dc.FillRect(rc, pHalftone);

			// Draw the left vertical line
			rc.SetRect(rc.left, y - 5,
			           rc.left + 3, y + 8);
			dc.FillRect(rc, pHalftone);

			// Draw the right vertical line
			rc.SetRect(rcRow.right - 2, y - 5,
			           rcRow.right + 1, y + 8);
			dc.FillRect(rc, pHalftone);
		}
	}
}

void CGridWnd::InvalidateColumn(int nColumn, BOOL bErase)
{
	CRect rect;
	GetCaptionRow()->GetCell(nColumn)->GetRect(rect);
	rect.bottom = 32767;
	InvalidateRect(rect, bErase);
}

void CGridWnd::SizeColumnToFit(int nColumn)
{
	CClientDC dc(this);
	CFont* pOldFont = dc.SelectObject(GetFont( ));
	
	int cxWidth = 0;
	POSITION pos = GetHeadRowPosition();
	while (pos != NULL)
	{
		int cxCell = GetNextRow(pos)->GetCellSize(&dc, nColumn).cx;
		if (cxCell > cxWidth)
			cxWidth = cxCell;
	}
	dc.SelectObject(pOldFont);
	
	CRect client;
	GetClientRect(client);
	CRect column;
	GetCaptionRow()->GetCell(nColumn)->GetRect(column);
	if (column.left + cxWidth > client.right)
		cxWidth = client.right - column.left;
	
	SetColumnWidth(nColumn, cxWidth);
	
	OnSize(SIZE_RESTORED, client.Width(), client.Height()); // adjust last column
}

#include <search.h>

static int CompareRows(const void* p1, const void* p2)
{
	const CGridRow* pRow1 = *(const CGridRow**)p1;
	const CGridRow* pRow2 = *(const CGridRow**)p2;
	return pRow1->Compare(pRow2);
}

void CGridWnd::SortColumn(int nColumn, BOOL bReverse)
{
	if (nColumn == m_nSortColumn && !bReverse == !m_bReverseSort && m_bAutoSort)
		return;
	
	CPtrArray rows;
	rows.SetSize(GetRowCount());
	POSITION pos = GetHeadRowPosition();
	int nRow = 0;
	while (pos != NULL)
		rows[nRow++] = GetNextRow(pos);
	
	m_bReverseSort = bReverse;

	if (m_nSortColumn != nColumn)
	{
		m_nSortColumn = nColumn;
		if ( rows.GetSize( ) > 0 )
			qsort(&rows[0], rows.GetSize(), sizeof (void*), CompareRows);
	}
	else
	{
		// Just reversing previous sort...
		bReverse = TRUE;
	}
	
	CGridRow* pCaptionRow = GetCaptionRow();
	CGridRow* pCurRow = m_posCurRow == NULL ? NULL : GetRowAt(m_posCurRow);

	m_rows.RemoveAll();
	for (nRow = 0; nRow < rows.GetSize(); nRow += 1)
	{
		if (bReverse)
			m_rows.AddHead(rows[nRow]);
		else
			m_rows.AddTail(rows[nRow]);
	}
	
	m_rows.AddHead(pCaptionRow);
	
	// Reset the position's that are cached...
	m_posTopVisibleRow = GetHeadRowPosition();//m_rows.Find(pTopRow);
	m_cyScroll = 0;
	SetScrollPos(SB_VERT, 0);
	
	if (pCurRow != NULL)
	{
		m_posCurRow = m_rows.Find(pCurRow);
		ScrollIntoView(pCurRow);
	}
	
	Invalidate(FALSE);
}

void CGridWnd::ReSort()
{
	// force grid to re-sort all entries; may be needed when items
	// have been added, deleted, or modified.
	int column = m_nSortColumn;
	m_nSortColumn = -1;
	SortColumn(column);
}

void CGridWnd::SelectBetween(CGridElement* pElement1, CGridElement* pElement2)
{
	int nIndex1 = 0;
	int nIndex2 = 0;

	if (!m_fMultiSelect)	// single select
	{
		Select(pElement2);
		return;
	}
			
	POSITION pos = GetHeadRowPosition();
	int nIndex = 1;
	while (pos != NULL)
	{
		CGridRow* pRow = GetRowAt(pos);
				
		if (pRow == pElement1)
			nIndex1 = nIndex;
				
		if (pRow == pElement2)
			nIndex2 = nIndex;
				
		if (nIndex1 != 0 && nIndex2 != 0)
			break;
				
		GetNextRow(pos);
		nIndex += 1;
	}
			
	ASSERT(nIndex1 != 0 && nIndex2 != 0);
			
	int nFirstSel = min(nIndex1, nIndex2);
	int nLastSel = max(nIndex1, nIndex2);
	pos = m_rows.FindIndex(nFirstSel);
	for (int n = nFirstSel; n <= nLastSel; n += 1)
	{
		CGridRow* pRow = GetNextRow(pos);
		if (!IsSelected(pRow))
			Select(pRow, TRUE);
	}
}

void CGridWnd::OnRButtonDown(UINT nFlags, CPoint point)
{
	CGridElement * pElement = ElementFromPoint(point);

	if (pElement != NULL && pElement->GetRow() != GetCaptionRow() 
	    && (point.x < m_cxSelectMargin || !IsSelected(pElement)) )
	{
		Select(pElement, FALSE);
		UpdateWindow(); // So we don't blow save bits
	}
}

void CGridWnd::OnRButtonUp(UINT nFlags, CPoint point)
{
	CSlobWnd* pWnd = GetActiveSlobWnd();
	if (!(m_bShowContextPopup && pWnd && pWnd->IsChild(this)))
		return;

	ClientToScreen(&point);
	pWnd->ShowContextPopupMenu(point);
}

CPoint CGridWnd::c_LastClick;
void CGridWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
	// NOTE: m_bBtnDownValid is used to determine if we have lost focus during
	// the keydown processing. We currently set it true only in CGridCOntrolWnd::LButtonDown
	// and turn it off during KillFocus. In case we are called from another place, we
	// are careful to always reset it to FALSE, so if you return out of here,
	// please be sure to set it. (do a goto LRet rather than a return! bobz)

	c_LastClick = point;

	SetFocus();
	
	CGridElement* pElement = ElementFromPoint(point);
	if (pElement != NULL)
	{
		if (pElement->OnLButtonDown(nFlags, point))
			goto LRet;
		
		// Make a selection, unless the element is already selected
		// since we don't want to collapse the selection in the case
		// of a drag'n'drop...
		BOOL bSelected = IsSelected(pElement);
		if ((nFlags & MK_SHIFT) != 0 && !m_selection.IsEmpty())
		{
			bSelected = TRUE;
			SelectBetween((CGridElement*)m_selection.GetHead(), pElement);
			Select(pElement, TRUE); // this will ensure this is the primary selection
		}
		else if (point.x < m_cxSelectMargin || !bSelected)
		{
			bSelected = TRUE;
			Select(pElement, (nFlags & (MK_CONTROL | MK_SHIFT)));
		}
		else if (bSelected)
		{
			if (nFlags & MK_CONTROL)
			{
				// Ctl-click a selected row to deselect it
				if (m_fMultiSelect)
				{
					bSelected = FALSE;
					Deselect(pElement);
				}
			}
			else
			{
				Select(pElement, FALSE); // Ensure this is the primary selection
			}
		}
		ASSERT(bSelected == IsSelected(pElement));

		// make sure row is visible
		CGridRow* pRow = pElement->GetRow();
		ScrollIntoView(pRow);

		// only set up extended select or drag/drop if we did not lose focus while
		// key down was processed
		if (m_bBtnDownValid && (point.x < m_cxSelectMargin || !BeginDrag(nFlags, point)) && bSelected)
		{
			TRACE("Begin drag-select\n");
			
			m_bDragSelecting = TRUE;
			m_bDragRow = FALSE;
			m_pDropRow = NULL;
			SetCapture();
			DWORD dwTimeout; // NT Defines this lparam as LPDWORD
			SystemParametersInfo(SPI_GETKEYBOARDSPEED, 0, &dwTimeout, FALSE);
			SetTimer(0, dwTimeout, NULL);
		}
	}
	
	m_pFirstSelect = m_pLastSelect = pElement;

LRet:
	m_bBtnDownValid = FALSE;  // return to known state
}

void CGridWnd::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	CGridElement* pElement = ElementFromPoint(point);

	if (pElement != NULL)	
	{
		// CHICAGO: Move property handling to rbutton.
		if (point.y > GetCaptionRow()->GetRowHeight() &&
				m_bShowProperties && (GetKeyState(VK_MENU) & ~1) != 0)
			ActivatePropertyBrowser();
		else
			pElement->OnLButtonDblClk(nFlags, point);
	}
}

int CGridWnd::ColumnFromX(int x)
{
	for (int nColumn = 0; nColumn < GetColumnCount(); nColumn += 1)
	{
		x -= GetColumnWidth(nColumn);
		if (x < 0)
			return nColumn;
	}
	
	return -1;
}

static CGridElement *g_pLastActive = NULL;
void CGridWnd::OnLButtonUp(UINT nFlags, CPoint point)
{
	int cSelItems = m_selection.GetCount();
	if (m_pCaptureElement != NULL)
	{
		m_pCaptureElement->OnLButtonUp(nFlags, point);
	}
	else if (cSelItems > 0)
	{
		CGridElement* pSel = (CGridElement*)m_selection.GetHead();
		if (g_pLastActive != pSel)
		{
			if (m_bDragRow && c_LastClick != point)
			{
				DoDragDrop(point);
			}
			else
			{
				if (cSelItems == 1 && c_LastClick == point &&
				    ElementFromPoint(point) == pSel)
				{
					pSel->OnLButtonUp(nFlags, point);
				}
			}
		}
		else
		{
			CloseActiveElement();
		}

		g_pLastActive = NULL;
	}

	m_bDragSelecting = FALSE;
	m_bDragRow = FALSE;
	if (m_pDropRow != NULL)
		InvalidateRect(m_rcDropInvalidate);
	m_pDropRow = NULL;
	m_pFirstSelect = NULL;
	ReleaseCapture();
	KillTimer(0);

	// if you clicked a row, set m_posCurRow
	if (point.y > GetCaptionRow()->GetRowHeight())
	{
		int y = GetCaptionRow()->GetRowHeight() - m_cyScroll;
		POSITION pos = m_posTopVisibleRow;
		while (pos && ((y += GetRowAt(pos)->GetRowHeight()) < point.y))
			GetNextRow(pos);
		if (pos)
			m_posCurRow = pos;
	}
}

void CGridWnd::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_bDragSelecting || m_bDragRow)
	{
		CRect gridRect;
		GetClientRect(gridRect);
		gridRect.top += GetCaptionRow()->GetRowHeight();
		
		BOOL bNoDrop = FALSE;
		if (point.y < gridRect.top)
		{
			point.y = gridRect.top;
			bNoDrop = TRUE;
		}
		else if (point.y > gridRect.bottom - 1)
		{
			point.y = gridRect.bottom - 1;
			bNoDrop = TRUE;
		}
		
		CGridElement* pElement = ElementFromPoint(point);
		CGridRow* pNewRow = GetRowAt(m_rows.GetTailPosition());
		if (pElement == GetCaptionRow())
		{
			bNoDrop = TRUE;
			POSITION pos = m_posTopVisibleRow;
			ASSERT(pos != NULL);
			pElement = GetPrevRow(pos);
			if (pos != NULL)
				pElement = GetRowAt(pos);
			if (pElement == GetCaptionRow())
				pElement = GetRowAt(m_posTopVisibleRow);
		}
		
		if (pElement != NULL && pElement != m_pLastSelect)
		{
			int nDownIndex = 0;
			int nLastIndex = 0;
			int nThisIndex = 0;
			
			POSITION pos = GetHeadRowPosition();
			int nIndex = 1;
			while (pos != NULL)
			{
				CGridRow* pRow = GetRowAt(pos);
				
				if (pRow == m_pFirstSelect)
					nDownIndex = nIndex;
				
				if (pRow == m_pLastSelect)
					nLastIndex = nIndex;
				
				if (pRow == pElement)
					nThisIndex = nIndex;
				
				if (nDownIndex != 0 && nLastIndex != 0 && nThisIndex != 0)
					break;
				
				GetNextRow(pos);
				nIndex += 1;
			}
			
			ASSERT(nDownIndex != 0 && nLastIndex != 0 && nThisIndex != 0);
			
			int nFirstSel = min(nDownIndex, nThisIndex);
			int nFirst = min(nLastIndex, nFirstSel);
			int nLastSel = max(nDownIndex, nThisIndex);
			int nLast = max(nLastIndex, nLastSel);
			pos = m_rows.FindIndex(nFirst);
			for (int n = nFirst; n <= nLast; n += 1)
			{
				CGridRow* pRow = GetNextRow(pos);
				if (n >= nFirstSel && n <= nLastSel)
				{
					if (!IsSelected(pRow) && !m_bDragRow)
						Select(pRow, TRUE);
				}
				else
				{
					if (!m_bDragRow)
						Deselect(pRow);
				}
			}
			
			m_pLastSelect = pElement;
			if (!m_bDragRow)
				Select(pElement, TRUE); // this will ensure this is the primary selection
		}

		if (m_bEnableDragRow && m_bDragRow)
		{
			// Set the drop row
			CGridRow* pOldDrop = m_pDropRow;
			if (bNoDrop)
			{
				m_pDropRow = NULL;
			}
			else
			{
				if (pElement == NULL)
					m_pDropRow = (CGridRow*)m_rows.GetTail();
				else
					m_pDropRow = pElement->GetRow();

				ASSERT(m_pDropRow != NULL);
				AdjustDropRow(m_pDropRow);

				if (pOldDrop != m_pDropRow)
				{
					CRect rcRow;
					m_pDropRow->GetRect(rcRow);

					rcRow.top -= 8;
					rcRow.bottom += 16;
					InvalidateRect(rcRow, FALSE);
				}
			}

			if (pOldDrop != NULL && pOldDrop != m_pDropRow)
				InvalidateRect(m_rcDropInvalidate, FALSE);
		}
		return; // don't change mouse cursor...
	}
	else if (m_pCaptureElement != NULL)
	{
		if (m_pCaptureElement->OnMouseMove(nFlags, point))
			return;
	}
	else
	{
		CGridElement* pElement = ElementFromPoint(point);
		if (pElement != NULL && pElement->OnMouseMove(nFlags, point))
			return;
			
		if (point.x < m_cxSelectMargin && m_fMultiSelect)
		{
			SetCursor(AfxGetApp()->LoadCursor(IDC_RIGHT_ARROW));
			return;
		}
	}
	
	SetCursor(LoadCursor(NULL, IDC_ARROW));
}

void CGridWnd::AdjustTopVisible()
{
	if (m_posTopVisibleRow)
	{
		CRect rect;
		GetWindowRect(rect);
		int cy = rect.Height();

		POSITION pos = m_rows.GetTailPosition();
		ASSERT( pos );

		GetRowAt(pos)->GetRect( rect );
		if ( (rect.bottom < cy) &&
				 (m_posTopVisibleRow != m_rows.GetHeadPosition() ? TRUE : !m_cyScroll) )
		{
			POSITION posPrev = NULL;
			int y = GetCaptionRow()->GetRowHeight();
			int yThis;
			while (pos)
			{
				yThis = GetRowAt(pos)->GetRowHeight();
				if ((yThis + y) > cy)
				{
					pos = posPrev;
					break;
				}
				y += yThis;
				posPrev = pos;
				GetPrevRow( pos );
			}

			// adjust m_cyScroll
			m_cyScroll = 0;

			if (pos && pos != GetHeadRowPosition())
			{
				if ((cy - y) > 0)
				{
					GetPrevRow(pos);
					ASSERT(pos);
					m_cyScroll = (y + GetRowAt(pos)->GetRowHeight()) - cy;
				}
			}

			if (pos)
				m_posTopVisibleRow = pos;
			else
				m_posTopVisibleRow = GetHeadRowPosition();
		}
	}
}

void CGridWnd::OnSize(UINT nType, int cx, int cy)
{
	// Deactivate the element first
	SetActiveElement(NULL);
	
	// Reset the width of the last column...
	int nLastColumn = GetColumnCount() - 1;
	for (int nColumn = 0; nColumn < nLastColumn; nColumn += 1)
		cx -= GetColumnWidth(nColumn);
	SetColumnWidth(nLastColumn, cx);

	// Adjust the topvisible row if needed (window is made taller, and the last row is already visible)	
	AdjustTopVisible();
	ResetScrollBars();

	// UNDONE: Warning! Do not reactivate the element at this time, or it could cause crashes when
	// the Active element tries to get the focus. [fabriced] 25jul96 (DevStudio #5847)
}

void CGridWnd::RemoveRow(CGridRow* pRow, BOOL bRedraw /* = TRUE */ )
{
	POSITION pos = m_rows.Find(pRow);
	if (pos == NULL)
		return;

	if (m_pActiveElement == pRow)
	{
		SetActiveElement(NULL);
	}

	POSITION posSel = m_selection.Find(pRow);
	if (posSel != NULL)
	{
		// Make sure the row is properly deselected before removing it
		Deselect(pRow);
	}
	
	if (m_posTopVisibleRow == pos)
	{
		GetNextRow(m_posTopVisibleRow);
		if (m_posTopVisibleRow == NULL)
		{
			m_posTopVisibleRow = pos;
			GetPrevRow(m_posTopVisibleRow);
		}
	}
	
	if (m_posCurRow == pos)
	{
		GetNextRow(m_posCurRow);
		if (m_posCurRow == NULL)
		{
			m_posCurRow = pos;
			GetPrevRow(m_posCurRow);
		}
	}
	
	m_rows.RemoveAt(pos);
	
	if (m_hWnd != NULL && bRedraw )
	{
		Invalidate(FALSE); // INEFFICIENT
		ResetScrollBars();
	}
}

void CGridWnd::AddRow(CGridRow* pRow, BOOL bRedraw /* = TRUE */ )
{
	if (m_nSortColumn == -1)
	{
		m_rows.AddTail(pRow);
	}
	else
	{
		POSITION pos = GetHeadRowPosition();
		while (pos != NULL)
		{
			int nCompare = pRow->Compare(GetRowAt(pos));
			if (m_bReverseSort)
				nCompare = -nCompare;
			if (nCompare < 0)
				break;
			GetNextRow(pos);
		}
		
		if (pos == NULL)
		{
			m_rows.AddTail(pRow);
		}
		else
		{
			m_rows.InsertBefore(pos, pRow);
		}
	}
	
	if ((m_hWnd != NULL) && bRedraw  )
	{
		ResetSize(); // INEFFICIENT
		Invalidate(FALSE); // INEFFICIENT
		ResetScrollBars();
	}
}

CGridCellRow* CGridWnd::AddCellRow()
{
	CGridCellRow* pRow = new CGridCellRow(this);
	if (!pRow->CreateCells())
	{
		delete pRow;
		return NULL;
	}
	m_rows.AddTail(pRow);
	m_bHasCells = TRUE;
	
	if (m_hWnd != NULL)
	{
		ResetSize(); // INEFFICIENT
		Invalidate(FALSE); // INEFFICIENT
		ResetScrollBars();
	}

	return pRow;
}

void CGridWnd::ResetScrollBars()
{
	if (m_bDestruct)
		return;

	if (m_hWnd == NULL)
		return;
	
	int cyRange = 0;
	int yScroll = 0;
	POSITION pos = GetHeadRowPosition();
	while (pos != NULL)
	{
		if (pos == m_posTopVisibleRow)
			yScroll = cyRange + m_cyScroll;
		cyRange += GetNextRow(pos)->GetRowHeight();
	}
	
	CRect client;
	GetClientRect(client);
	cyRange -= client.Height() - GetCaptionRow()->GetRowHeight();
	if (cyRange < 0)
	{
		cyRange = 0;
		yScroll = 0;
		m_cyScroll = 0;
		m_posTopVisibleRow = GetHeadRowPosition();
		Invalidate(FALSE);
	}

	if (theApp.m_bOnChicago)
	{
		SCROLLINFO si;
		si.cbSize = sizeof (si);
		si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;
		si.nMin = 0;
		si.nPage = client.Height() - GetCaptionRow()->GetRowHeight();
		si.nMax = cyRange + si.nPage;
		si.nPos = yScroll;
		SetScrollInfo(SB_VERT, &si, TRUE);
	}
	else
	{
		SetScrollRange(SB_VERT, 0, cyRange, FALSE);
		SetScrollPos(SB_VERT, yScroll);
	}

	if (yScroll > cyRange)
		VScroll(cyRange - yScroll);
}

void CGridWnd::VScroll(int cyScroll)
{
	CRect client;
	GetClientRect(client);

	int yCurPos = GetScrollPos(SB_VERT);
	int yMin, yLim;
	GetScrollRange(SB_VERT, &yMin, &yLim);
	if (theApp.m_bOnChicago)
		yLim -= client.Height() - GetCaptionRow()->GetRowHeight();
	
	if (yCurPos + cyScroll < 0)
		cyScroll = -yCurPos;
	else if (yCurPos + cyScroll > yLim)
		cyScroll = yLim - yCurPos;
	
	client.top += GetCaptionRow()->GetRowHeight();
	UpdateWindow(); // since we're not scrolling whole client
	SetActiveElement(NULL);
	ScrollWindow(0, -cyScroll, client, client);
	if (m_pDropRow != NULL)
		m_rcDropInvalidate.OffsetRect(0, -cyScroll);
	
	SetScrollPos(SB_VERT, yCurPos + cyScroll);
	
	if (cyScroll < 0)
	{
		// scroll towards top
		
		cyScroll = -cyScroll;
		if (cyScroll <= m_cyScroll)
		{
			m_cyScroll -= cyScroll;
		}
		else
		{
			cyScroll -= m_cyScroll;
			while (cyScroll > 0)
			{
				POSITION pos = m_posTopVisibleRow;
				if (pos == GetHeadRowPosition())
					break;

				GetPrevRow(pos);
				m_posTopVisibleRow = pos;
				cyScroll -= GetRowAt(pos)->GetRowHeight();
			}
			m_cyScroll = -cyScroll;
		}
	}
	else if (cyScroll > 0)
	{
		// scroll towards bottom
		
		m_cyScroll += cyScroll;
		POSITION pos = m_posTopVisibleRow;
		while (pos != NULL && m_cyScroll > GetRowAt(pos)->GetRowHeight())
		{
			m_cyScroll -= GetRowAt(pos)->GetRowHeight();
			GetNextRow(pos);
			if (pos != NULL)
				m_posTopVisibleRow = pos;
		}
	}
}

void CGridWnd::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	CRect page;
	GetClientRect(page);
	page.top += GetCaptionRow()->GetRowHeight();
	if (page.Height() > 20)
		page.top += 10;

	// FUTURE: This isn't the perfect solution...
	int cyLine = m_posCurRow != NULL ? GetRowAt(m_posCurRow)->GetRowHeight() : 15;
	if (cyLine > page.Height())
		cyLine = page.Height() - 1;

	if (cyLine <= 0)
		return;
	
	switch (nSBCode)
	{
	case SB_LINEUP:
		VScroll(-cyLine);
		break;
		
	case SB_LINEDOWN:
		VScroll(cyLine);
		break;
	
	case SB_PAGEUP:
		VScroll(-page.Height());
		break;
	
	case SB_PAGEDOWN:
		VScroll(page.Height());
		break;
		
	case SB_THUMBTRACK:
		VScroll(nPos - GetScrollPos(SB_VERT));
		break;
	}
}

void CGridWnd::ResetSize()
{
	if (m_hWnd != NULL)
	{
		CClientDC dc(this);

		if (GetCaptionRow() != NULL)
		{
			ASSERT( GetCaptionFont() != NULL );
			CFont *pOldFont = dc.SelectObject( GetCaptionFont() );
		
			GetCaptionRow()->ResetSize( &dc );
			dc.SelectObject( pOldFont );
		}
		
		CFont* pOldFont = dc.SelectObject(GetFont( ));
		POSITION pos = GetHeadRowPosition();
		while (pos != NULL)
		{
			CGridRow* pRow = GetNextRow(pos);
			pRow->ResetSize(&dc);
		}
		dc.SelectObject(pOldFont);
	}
}

void CGridWnd::OnTimer(UINT nIDEvent)
{
	ASSERT(nIDEvent == 0);
	
	CPoint point;
	GetCursorPos(&point);
	ScreenToClient(&point);

	// Auto-scroll
	CRect gridRect;
	GetClientRect(gridRect);
	gridRect.top += GetCaptionRow()->GetRowHeight();
	if (point.y <= gridRect.top)
	{
		VScroll(-15); // FUTURE: random number!
		OnMouseMove(0, CPoint(gridRect.left, gridRect.top - 1));
	}
	else if (point.y >= gridRect.bottom - 1)
	{
		VScroll(15); // FUTURE: random number!
		OnMouseMove(0, CPoint(gridRect.left, gridRect.bottom - 1));
	}
}

void CGridWnd::SetActiveElement(CGridElement* pElement, int nColumn)
{
	BOOL bSetFocus = FALSE;
	
	if (m_pActiveElement != NULL)
	{
		if (!m_bDestruct)
			m_pActiveElement->Invalidate(FALSE, TRUE);
		
		if (pElement == NULL && m_hWnd != NULL && IsChild(GetFocus()))
			bSetFocus = TRUE;
		
		m_pActiveElement->OnActivate(FALSE, -1);

		// Inform the owner of the grid.
		GetParent()->PostMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), GLN_ACCEPT_CTL), (LPARAM)m_hWnd);
	}
	
	m_pActiveElement = pElement;
	
	if (m_pActiveElement != NULL)
	{
		if (!m_bDestruct)
			m_pActiveElement->Invalidate(FALSE, TRUE);
		m_pActiveElement->OnActivate(TRUE, nColumn);

		// Inform the owner of the grid.
		GetParent()->PostMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), GLN_ACTIVATE_CTL), (LPARAM)m_hWnd);
	}
	
	if (bSetFocus)
		SetFocus();
}

BOOL CGridWnd::CloseActiveElement()
{
	SetActiveElement( NULL );
	return TRUE;
}

void CGridWnd::OnSetFocus(CWnd* pOldWnd)
{
	InvalidateSelection();
  
	// only if the old focus was not child window
	if (!IsChild(pOldWnd))
		GetParent()->PostMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), GLN_SETFOCUS), (LPARAM)m_hWnd);
}

void CGridWnd::OnKillFocus(CWnd* pNewWnd)
{
	InvalidateSelection();
	m_bBtnDownValid = FALSE; // so we don't try to handle drag in a buttondown if focus is lost

	// only if the new focus is not a child window
	if (!IsChild(pNewWnd))
	{
		GetParent()->PostMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), GLN_KILLFOCUS), (LPARAM)m_hWnd);
	}
}

// Tool tip handler for grid toolbar buttons
BOOL CGridWnd::OnToolTipText(UINT nID, NMHDR* pNMHDR, LRESULT* pResult)
{
	// only do this code if tip is for a grid toolbar

	if (!m_bParentIsDialog || m_pToolBar == NULL)
	{
		return FALSE;
	}

	ASSERT(pNMHDR->code == TTN_NEEDTEXTA || pNMHDR->code == TTN_NEEDTEXTW);

	// these should never use window handles instead of ids
	ASSERT (!(pNMHDR->code == TTN_NEEDTEXTA && (((TOOLTIPTEXTA*)pNMHDR)->uFlags & TTF_IDISHWND)));
	ASSERT (!(pNMHDR->code == TTN_NEEDTEXTW && (((TOOLTIPTEXTW*)pNMHDR)->uFlags & TTF_IDISHWND)));

	if (!theApp.m_bToolTips)
	{
		CTheApp::SetEmptyToolTip(pNMHDR);
	
		*pResult = 0;
		return TRUE;
	}

	UINT nIDFrom = pNMHDR->idFrom;

	ASSERT (nIDFrom == ID_GRD_NEW || nIDFrom == ID_GRD_DELETE || nIDFrom == ID_GRD_MOVEUP || nIDFrom == ID_GRD_MOVEDOWN);
	if (nIDFrom < ID_GRD_NEW || nIDFrom > ID_GRD_MOVEDOWN)
		return FALSE;

	TCHAR szFullText[256];
	CString strTipText;
	if (nIDFrom != 0) // will be zero on a separator
	{
		AfxLoadString(nIDFrom, szFullText);
			// this is the otherwise unused status line text, which contains the key names
		AfxExtractSubString(strTipText, szFullText, 0, '\n');
	}
	
	LPCTSTR szKey;
	if (strTipText.IsEmpty())
		szKey = "";
	else
		szKey = strTipText;

	return CTheApp::HandleToolTipText(nID, pNMHDR, pResult, szKey);
}


void CGridWnd::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CWnd::OnShowWindow(bShow, nStatus);
	
	// tie toolbar and static window visibility to grid
	if (m_pToolBar != NULL)
		m_pToolBar->ShowWindow(bShow);

	if (m_cstatVCenter.m_hWnd != NULL)
		m_cstatVCenter.ShowWindow(bShow);
}

void CGridWnd::OnEnable(BOOL bEnable) 
{
	CWnd::OnEnable(bEnable);
	
	// tie toolbar and static window enable state to grid
	if (m_pToolBar != NULL)
		m_pToolBar->EnableWindow(bEnable);

	if (m_cstatVCenter.m_hWnd != NULL)
		m_cstatVCenter.EnableWindow(bEnable);
}

////////////////////////////////////////////////////////////////////////////
// CGridControlWnd

CGridControlWnd::CGridControlWnd(int nColumns, BOOL bDisplayCaption)
	: CGridWnd(nColumns, bDisplayCaption )
{
	m_controlType = none;
	m_pControlWnd = NULL;
	m_nControlColumn = -1;
}

CGridControlWnd::~CGridControlWnd()
{
	if (m_pControlWnd != NULL)
	{
		delete m_pControlWnd;
		m_pControlWnd = NULL;
		m_controlType = none;
		m_nControlColumn = -1;
	}
}

afx_msg int CGridControlWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CGridWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}



BOOL CGridControlWnd::PreTranslateMessage(MSG* pMsg)
{
	return ProcessKeyboard(pMsg, TRUE);
}

BOOL CGridControlWnd::ProcessKeyboard(MSG* pMsg, BOOL bPreTrans /*= FALSE*/)
{
	// FUTURE: Clean up
	switch (pMsg->message)
	{
	case WM_KEYDOWN:
		switch (pMsg->wParam)
		{
		case VK_TAB:
			// control-tab is mdi window switching
			if (GetKeyState(VK_CONTROL) < 0)
				break;

			// in a dialog tab pops us out of grid. Pretrans ignores, non pretrans cluses and posts
			if (bPreTrans && m_bParentIsDialog)
				break;

			if (!AcceptControl(FALSE))
				return TRUE;

			{ // BLOCK
			BOOL bDoTabInDialog = (!bPreTrans && m_bParentIsDialog);

			if (!bDoTabInDialog)
			{
				if (GetKeyState(VK_SHIFT) < 0)
					ActivatePrevControl();
				else
					ActivateNextControl();
			}
			else // (bDoTabInDialog)
			{
				CloseActiveElement();

				CWnd* pParent = GetParent();
				
				if (pParent != NULL)
				{
					pParent->PostMessage(pMsg->message, pMsg->wParam, pMsg->lParam);
				}
			}
			} // End Block

			return TRUE;
	
		case VK_ESCAPE:
			if (m_bDragRow) // Cancel the drag
			{
				CancelDrag();
			}
			else if (m_pActiveElement != NULL)
			{
				CancelControl();
			}
			else
			{
				ClearSelection();
				return CGridWnd::ProcessKeyboard(pMsg, bPreTrans);
			}
			return TRUE;

		case VK_RETURN:
			if (m_pActiveElement != NULL && (GetKeyState(VK_CONTROL) >= 0))
			{
				AcceptControl();
				return TRUE;
			}
			if (GetKeyState(VK_MENU) < 0)
			{
				// if there's no active control, let the event be
				// handled by the ResView (which brings up prop page).
				return CGridWnd::ProcessKeyboard(pMsg, bPreTrans);
			}
			if (GetKeyState(VK_CONTROL) < 0)
				return CGridWnd::ProcessKeyboard(pMsg, bPreTrans);

		case VK_F2:
//		case VK_F3: (UNDONE) 05-15-96 [fabriced]: this is conflicting with
//					FindNext in the Version and String editors (in devres.pkg)
			if (m_selection.GetCount() != 1)
				MessageBeep(0);
			else
				SetActiveElement((CGridElement*)m_selection.GetHead(), 0);
			return TRUE;
		}
		if (bPreTrans)
		{
			switch (pMsg->wParam)
			{
//			case 'Z':
			case 'X':
			case 'C':
			case 'V':
				if (GetKeyState(VK_CONTROL) >= 0)
					break;
				// otherwise, fall through to translate/dispatch

			case VK_DELETE:
				// The key mapping code will steal these keys away, unless
				// we dispatch them here.
				if (!m_pActiveElement || !m_pControlWnd)
					break;
				TranslateMessage(pMsg);
				DispatchMessage(pMsg);
				return TRUE;
			}
		}
		break;

	case WM_KEYUP:	// if using the IME, I won't get a wm_char
	{
		if (!theIME.IsOpen() || !theIME.IsEnabled())
			break;
		// convert the VK to ascii, to weed out chars like VK_DOWN
		// and note if Ctrl or Alt is down (an additional check for printable char)
		//
		WORD wTransKey = 0;
		UINT oemScan = LOBYTE(HIWORD(pMsg->lParam));

		BYTE kbdState[256] = {0};
		GetKeyboardState(kbdState);

		int fAscii = ToAscii(pMsg->wParam, oemScan, kbdState, &wTransKey, 0);
		ASSERT(!(WORD)fAscii == !wTransKey);		// they should both be 0, or both non-0

		// If iconic, unprintable character, or in drag op, return.
		//
		if (IsIconic() || (GetCapture() != NULL) || (fAscii == 0) || (wTransKey <= ' '))
			break;

		// if there was a WM_MENUSELECT recently, ignore this
		int nKeyDelay = 251;
// FUTURE:		VERIFY( SystemParametersInfo( SPI_GETKEYBOARDDELAY, 0, &nKeyDelay, 0 ) );
		if ( theApp.m_dwMenuSelectTime + nKeyDelay > pMsg->time )
			break;

		// if it's a valid character, then fall through to the auto activation code
	}
	case WM_CHAR:
		// NOTE: removed VkKeyScan here because Chicago
		// returns back garbage for escape.	
		BOOL bEscape = pMsg->message == WM_CHAR ? 
					(LOBYTE(pMsg->wParam) == VK_ESCAPE) :
					(pMsg->wParam	== VK_ESCAPE);

		// Auto activation
		if (!m_pActiveElement && !bEscape)
		{
			if (m_selection.GetCount() != 1)
			{
				MessageBeep(0);
			}
			else
			{
				CGridElement *pElement = (CGridElement*)m_selection.GetHead();

				// make sure row is visible
				CGridRow* pRow = pElement->GetRow();
				ScrollIntoView( pRow );

				SetActiveElement(pElement, 0);
				BOOL bReturn = pMsg->message == WM_CHAR ?
					(LOBYTE(VkKeyScan((CHAR)pMsg->wParam)) == VK_RETURN) :
					(pMsg->wParam	== VK_RETURN);

				if (m_pControlWnd && !bReturn)
				{
					switch (m_controlType)
					{
					case edit:
						ASSERT( m_pControlWnd->IsKindOf( RUNTIME_CLASS( CEdit ) ) );
						((CEdit *)m_pControlWnd)->SetSel( 0, -1 );
						break;

					case editCombo:
						ASSERT( m_pControlWnd->IsKindOf( RUNTIME_CLASS( CComboBox ) ) );
						((CComboBox *)m_pControlWnd)->SetEditSel( 0, -1 );
						break;
				
					case listCombo:
						ASSERT( m_pControlWnd->IsKindOf( RUNTIME_CLASS( CComboBox ) ) );
						break;	// nothing to do

					default:
						ASSERT(FALSE);
					}
				}
				if (!bReturn)
				{
					HWND hwndChild = GetWindow(GW_CHILD)->GetSafeHwnd();
					if (hwndChild != NULL)
					{
						if (bPreTrans)
						{
							pMsg->hwnd = hwndChild;
						}
						else
						{
							::PostMessage(hwndChild, pMsg->message, pMsg->wParam, pMsg->lParam);
						}
					}
				}	
			}
		}

		break;
	}

	if (m_pControlWnd == NULL)
		return CGridWnd::ProcessKeyboard(pMsg, bPreTrans);
	else
		return FALSE;
}

void CGridControlWnd::CancelDrag(void)
{
	m_bDragSelecting = FALSE;
	m_bDragRow = FALSE;
	if (m_pDropRow != NULL)
		InvalidateRect(m_rcDropInvalidate);
	m_pDropRow = NULL;
	m_pFirstSelect = NULL;
	ReleaseCapture();
	KillTimer(0);
}
 
LRESULT CGridControlWnd::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	UINT nControlID;
	UINT nNotification;

	if (message == WM_COMMAND && m_pActiveElement != NULL)
	{
#ifdef _WIN32
		nControlID = LOWORD(wParam);
		nNotification =  HIWORD(wParam);
#else
		nControlID = (UINT) wParam;
		nNotification = (UINT) HIWORD(lParam); 
#endif

		if (!FGridToolBarCmd(nControlID, nNotification))
			m_pActiveElement->OnControlNotify(nControlID, nNotification);

		return 0;
	}

	return CGridWnd::WindowProc(message, wParam, lParam);
}

IMPLEMENT_DYNAMIC(CGridControlWnd, CGridWnd)

BEGIN_MESSAGE_MAP(CGridControlWnd, CGridWnd)
	//{{AFX_MSG_MAP(CGridControlWnd)
	ON_WM_SETFOCUS()
	ON_WM_CTLCOLOR()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_WM_CHAR()
	ON_WM_LBUTTONDOWN()
	ON_WM_NCLBUTTONDOWN()
	ON_WM_CREATE()
	ON_COMMAND(ID_EDIT_CUT, OnCut)
	ON_COMMAND(ID_EDIT_COPY, OnCopy)
	ON_COMMAND(ID_EDIT_PASTE, OnPaste)
	ON_COMMAND(ID_EDIT_CLEAR, OnClear)
	ON_COMMAND(ID_EDIT_UNDO, OnUndo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, OnUpdateCut)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateCopy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdatePaste)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CLEAR, OnUpdateClear)
	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, OnUpdateUndo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_REDO, OnUpdateRedo)
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_WM_VSCROLL()
	ON_BN_CLICKED(ID_GRD_NEW, OnGridNew)
	ON_BN_CLICKED(ID_GRD_DELETE, OnGridDelete)
	ON_BN_CLICKED(ID_GRD_MOVEUP, OnGridMoveUp)
	ON_BN_CLICKED(ID_GRD_MOVEDOWN, OnGridMoveDown)


	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

afx_msg void CGridControlWnd::OnGridNew()  
{
}

afx_msg void CGridControlWnd::OnGridDelete()
{
}

afx_msg void CGridControlWnd::OnGridMoveUp()
{
}

afx_msg void CGridControlWnd::OnGridMoveDown()
{
}


void CGridControlWnd::OnDestroy()
{
	if (m_pActiveElement != NULL)
		CancelControl(); // close any open element without validation
}

void CGridControlWnd::OnSetFocus(CWnd* pOldWnd)
{
	if (m_pControlWnd != NULL)
	{
		m_pControlWnd->SetFocus();
		return;
	}
	CGridWnd::OnSetFocus(pOldWnd);
}

HBRUSH CGridControlWnd::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	static CBrush brush;
	static COLORREF color;
	COLORREF colorText = GetWindowTextColor();
	COLORREF colorBk = GetWindowColor();

	if (color != colorBk || brush.m_hObject == NULL)
	{
		color = colorBk;
		brush.DeleteObject();
		brush.CreateSolidBrush(colorBk);
	}

	if (nCtlColor == CTLCOLOR_LISTBOX && pWnd->GetWindow(GW_CHILD) != NULL)
	{
		return (HBRUSH)brush.m_hObject;
	}

	if (nCtlColor == CTLCOLOR_BTN)
	{
		static CBrush btnBrush;
		if (btnBrush.m_hObject == NULL)
			btnBrush.CreateSolidBrush(GetSysColor(COLOR_BTNFACE));

		return (HBRUSH)btnBrush.m_hObject;
	}

	// FUTURE: This doesn't make any sense to me (the msgbox bit) but 
	// this is what the documentation says needs to be done.
	// Doc Error?
	if (nCtlColor == CTLCOLOR_EDIT || nCtlColor == CTLCOLOR_MSGBOX)
	{
		pDC->SetTextColor(colorText);
		if ( nCtlColor == CTLCOLOR_EDIT )
			pDC->SetBkColor(colorBk);

		return (HBRUSH)brush.m_hObject;
	}
		
	return CGridWnd::OnCtlColor(pDC, pWnd, nCtlColor);
}

void CGridControlWnd::ActivatePrevControl()
{
	POSITION pos;
	
	if (GetRowCount() == 0)
		return;

	int nPrevControlColumn = ((CGridControlRow*)GetRow(0))->GetPrevControlColumn(m_nControlColumn);
	
	if (m_pActiveElement == NULL)
	{
		if (m_selection.IsEmpty())
		{
			// FUTURE: This really should look for the last visible row!
			if (m_posTopVisibleRow == NULL)
			{
				MessageBeep(0);
				return;
			}
			
			pos = m_posTopVisibleRow;
		}
		else
		{
			pos = m_rows.Find(m_selection.GetHead());
		}
	}
	else if (m_nControlColumn > nPrevControlColumn)
	{
		SetActiveElement(m_pActiveElement, nPrevControlColumn);
		return;
	}
	else
	{
		pos = m_rows.Find(m_pActiveElement);
		m_rows.GetPrev(pos);
	}
	
	if (pos != NULL && pos != m_rows.GetHeadPosition())
	{
		CGridElement* pElement = (CGridElement*)m_rows.GetAt(pos);
		Select(pElement);
		SetActiveElement(pElement, nPrevControlColumn);
		m_posCurRow = pos;
	}
}

void CGridControlWnd::ActivateNextControl()
{
	POSITION pos;
	
	if (GetRowCount() == 0)
		return;

	int nNextControlColumn;
	if (m_pActiveElement != NULL)
		nNextControlColumn = ((CGridControlRow*)m_pActiveElement)->GetNextControlColumn(m_nControlColumn);
	else
		nNextControlColumn = ((CGridControlRow*)GetRow(0))->GetNextControlColumn(m_nControlColumn);
	
	if (m_pActiveElement == NULL)
	{
		if (m_selection.IsEmpty())
		{
			if (m_posTopVisibleRow == NULL)
			{
				MessageBeep(0);
				return;
			}
			
			pos = m_posTopVisibleRow;
		}
		else
		{
			pos = m_rows.Find(m_selection.GetHead());
		}
	}
	else if ((m_nControlColumn >= nNextControlColumn) || (nNextControlColumn == -1))
	{
		pos = m_rows.Find(m_pActiveElement);
		m_rows.GetNext(pos);
	}
	else
	{
		SetActiveElement(m_pActiveElement, nNextControlColumn);
		return;
	}
	
	if (pos != NULL)
	{
		CGridElement* pElement = (CGridElement*)m_rows.GetAt(pos);
		Select(pElement);
		SetActiveElement(pElement, nNextControlColumn);
		m_posCurRow = pos;
	}
}

void CGridControlWnd::CancelControl()
{
	SetActiveElement(NULL);
}

BOOL CGridControlWnd::AcceptControl(BOOL bDeactivate /*=TRUE*/)
{
	if (m_pActiveElement != NULL && (m_pActiveElement->GetRow() != GetCaptionRow()))
	{
		if (!((CGridControlRow*)m_pActiveElement)->OnAccept(m_pControlWnd))
			return FALSE;
	}

	// Dynamic row sizing
	// If the current control is an edit box, and the new text makes us wrap lines,
	// increase the size of the control.
	CGridControlRow *pCurRow = (CGridControlRow *) (m_posCurRow ? GetRowAt(m_posCurRow) : NULL);
	if ( pCurRow && m_controlType == edit )
	{
		int cyOldHeight = pCurRow->GetRowHeight();
		
		CClientDC dc(this);
		CFont* pOldFont = dc.SelectObject(GetFont());
		pCurRow->ResetSize(&dc);
		dc.SelectObject(pOldFont);
		
		if (pCurRow->GetRowHeight() != cyOldHeight)
		{
			Invalidate(FALSE);
			
			CRect rect;
			pCurRow->GetRect(rect);
			pCurRow->AdjustForMargins(rect, m_nControlColumn);
			// FUTURE: hacky numbers won't work for all sys fonts
			rect.left -= 4; 
			rect.top -= 4;
			rect.right += 4;
			rect.bottom += 4;
			m_pControlWnd->MoveWindow(rect);
		
			// Hack alert!  Since we have to have ES_AUTOSCROLL set to prevent the
			// edit from just beeping when the cursor is at the bottom-right and
			// the user types, the edit has auto-scrolled when we have to resize the
			// window.  Here we scroll it back...
			((CEdit*)m_pControlWnd)->LineScroll(-10, 0);

			// Adjust the topvisible row if needed (edit box shrinks, and the last
			// row is already visible)	
			if (m_posTopVisibleRow && (pCurRow->GetRowHeight() < cyOldHeight))
			{
				POSITION pos = m_rows.GetTailPosition();
				CRect rect;
				GetWindowRect( rect );
				int cy = rect.Width();
				ASSERT( pos );

				GetRowAt(pos)->GetRect( rect );
				if ( (rect.bottom < cy) &&
						 (m_posTopVisibleRow != m_rows.GetHeadPosition() ? TRUE : !m_cyScroll) )
				{
					POSITION posPrev = NULL;
					int y = GetCaptionRow()->GetRowHeight();
					int yThis;
					while (pos)
					{
						yThis = GetRowAt(pos)->GetRowHeight();
						if ((yThis + y) > cy)
						{
							pos = posPrev;
							break;
						}
						y += yThis;
						posPrev = pos;
						GetPrevRow( pos );
					}
					// adjust m_cyScroll
					if (pos)
					{
						if ((cy - y) > 0)
						{
							GetPrevRow(pos);
							if (pos)
								m_cyScroll = (y + GetRowAt(pos)->GetRowHeight()) - cy;
							else
								m_cyScroll = 0;
				 		}
						else
						{
							m_cyScroll = 0;
						}
					}
					if (pos)
						m_posTopVisibleRow = pos;
					else
						m_posTopVisibleRow = GetHeadRowPosition();
				}
			}
		
			// recalc the scroll bars
			ResetScrollBars();
		}
	}

	if (bDeactivate)
		SetActiveElement(NULL);

	// if the row just accepted is out of order, perform an insertion sort
	if (m_bAutoSort && m_posCurRow)
	{
		// Check if we need to sort (cur > next || cur < prev) (check m_bReverseSort)
		// Set fResort to -1 if we need to move up, 1 to move down, 0 do nothing
		int fResort = 0;
		POSITION pos;
		CGridRow *pCurRow = GetRowAt(m_posCurRow);

		// compare with prev
		if (m_posCurRow != GetHeadRowPosition())
		{
			pos = m_posCurRow;
			GetPrevRow(pos);
			ASSERT( pos );
			fResort = (pCurRow->Compare(GetRowAt(pos)) == (m_bReverseSort ? 1 : -1)) ? -1 : 0;
		}
		// compare with next
		if ((m_posCurRow != m_rows.GetTailPosition()) && (fResort == 0))
		{
			pos = m_posCurRow;
			GetNextRow(pos);
			ASSERT(pos);
			fResort = (pCurRow->Compare(GetRowAt(pos)) == (m_bReverseSort ? -1 : 1)) ? 1 : 0;
		}

		// sort if needed
		if (fResort != 0)
		{
			void *pRow = m_rows.GetAt(m_posCurRow);
			POSITION posCaption = m_rows.GetHeadPosition();
			pos = m_posCurRow;
			int fTest;

			if (fResort == -1) // move up
			{
				GetPrevRow(pos);
				m_rows.RemoveAt(m_posCurRow);
				fTest = (m_bReverseSort ? 1 : -1);
				while (pos != NULL && pos != posCaption && pCurRow->Compare(GetRowAt(pos)) == fTest)
					GetPrevRow(pos);
				if (pos)
					m_posCurRow = m_rows.InsertAfter(pos, pRow);
				else
					m_posCurRow = m_rows.AddHead( pRow );
			} else if (fResort == 1)	// move down
			{
				GetNextRow(pos);
				m_rows.RemoveAt(m_posCurRow);
				fTest = (m_bReverseSort ? -1 : 1);
				while (pos != NULL && pCurRow->Compare(GetRowAt(pos)) == fTest)
					GetNextRow(pos);
				if (pos)
					m_posCurRow = m_rows.InsertBefore(pos, pRow);
				else
					m_posCurRow = m_rows.AddTail( pRow );
			}

			// make sure row is visible
			ScrollIntoView( pCurRow );
			Invalidate();
		}
	}

	return TRUE;
}

BOOL CGridControlWnd::CloseActiveElement()
{
	return AcceptControl();
}

BOOL CGridControlWnd::FGridToolBarCmd(UINT nID, int nCode)
{
	if (nID == ID_GRD_NEW || nID == ID_GRD_DELETE || nID == ID_GRD_MOVEUP || nID == ID_GRD_MOVEDOWN)
		return TRUE;
	// let toolbar tooltip notifications through
	if (nCode == MAKELONG(TTN_NEEDTEXTA, WM_NOTIFY) || nCode == MAKELONG(TTN_NEEDTEXTW, WM_NOTIFY))
		return TRUE;
	
	return FALSE;
}


BOOL CGridControlWnd::OnCmdMsg( UINT nID, int nCode, void *pExtra, AFX_CMDHANDLERINFO *pHandlerInfo )
{
	// FUTURE:  HACK!!! This will only work if each cmd in the message map
	// should only be called if there is an active control
	// bobz added extra hack for toolbars
	if (m_pControlWnd->GetSafeHwnd() || FGridToolBarCmd(nID, nCode))
		return CGridWnd::OnCmdMsg( nID, nCode, pExtra, pHandlerInfo );
	return FALSE;
}

void CGridControlWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
	g_pLastActive = m_pActiveElement;

	// turn this off if we lose focus before we are done in this routine. This
	// prevents us from starting a drag when we have lose the up click by a dialog.
	// the killfocus turn off and use of this flag are in CGridWnd
	// bobz  11/8/96
	m_bBtnDownValid = TRUE;

	if (! AcceptControl())
	{
		m_bBtnDownValid = FALSE;  // return to known state
		return;
	}

	CGridWnd::OnLButtonDown(nFlags, point);
	m_bBtnDownValid = FALSE;  // return to known state
}

afx_msg void CGridControlWnd::OnNcLButtonDown(UINT nHitTest, CPoint point) 
{
	if (AcceptControl())
		CGridWnd::OnNcLButtonDown(nHitTest, point);
}

afx_msg void CGridControlWnd::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	ProcessKeyboard((MSG*)GetCurrentMessage());
}

afx_msg void CGridControlWnd::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	ProcessKeyboard((MSG*)GetCurrentMessage());
}

afx_msg void CGridControlWnd::OnCut()
{
	if (m_pControlWnd->GetSafeHwnd())
		m_pControlWnd->SendMessage( WM_CUT );
}

afx_msg void CGridControlWnd::OnCopy()
{
	if (m_pControlWnd->GetSafeHwnd())
		m_pControlWnd->SendMessage( WM_COPY );
}

afx_msg void CGridControlWnd::OnPaste()
{
	if (m_pControlWnd->GetSafeHwnd())
		m_pControlWnd->SendMessage( WM_PASTE );
}

afx_msg void CGridControlWnd::OnClear()
{
	if (m_pControlWnd->GetSafeHwnd())
		m_pControlWnd->SendMessage( WM_CLEAR );
}

afx_msg void CGridControlWnd::OnUndo()
{
	if (m_pControlWnd->GetSafeHwnd())
		CancelControl();
}

void CGridControlWnd::OnUpdateCut( CCmdUI *pCmdUI )
{
	pCmdUI->Enable( m_pControlWnd->GetSafeHwnd() != NULL );
}

void CGridControlWnd::OnUpdateCopy( CCmdUI *pCmdUI )
{
	pCmdUI->Enable( m_pControlWnd->GetSafeHwnd() != NULL );
}

void CGridControlWnd::OnUpdatePaste( CCmdUI *pCmdUI )
{
	pCmdUI->Enable( m_pControlWnd->GetSafeHwnd() != NULL );
}

void CGridControlWnd::OnUpdateClear( CCmdUI *pCmdUI )
{
	pCmdUI->Enable( m_pControlWnd->GetSafeHwnd() != NULL );
}

void CGridControlWnd::OnUpdateUndo( CCmdUI *pCmdUI )
{
	pCmdUI->Enable( (m_pControlWnd->GetSafeHwnd() ?
		m_pControlWnd->SendMessage( EM_CANUNDO ) : FALSE) );
}

void CGridControlWnd::OnUpdateRedo( CCmdUI *pCmdUI )
{
	pCmdUI->Enable( FALSE );
}

afx_msg void CGridControlWnd::OnSize(UINT nType, int cx, int cy)
{
	if (AcceptControl(FALSE))
		CGridWnd::OnSize(nType, cx, cy);
}

afx_msg void CGridControlWnd::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if (AcceptControl())
		CGridWnd::OnVScroll(nSBCode, nPos, pScrollBar);
}


////////////////////////////////////////////////////////////////////////////
// Ctl3d helpers for the grid control
typedef BOOL (WINAPI *CTL3DUNSUBCLASSCTLPROC)(HWND);
class CGridCtl3dInterface
{
public:
	CGridCtl3dInterface();
	void Load();
	void Free();
	void UnsubclassCtl(HWND hWnd);

protected:
	HINSTANCE m_Ctl3d;
	CTL3DUNSUBCLASSCTLPROC m_lpfnUnsubclassCtl;
};
CGridCtl3dInterface g_GridCtl3dInterface;

CGridCtl3dInterface::CGridCtl3dInterface()
{
	m_Ctl3d = NULL;
	m_lpfnUnsubclassCtl = NULL;
}

void CGridCtl3dInterface::Load()
{
	if (m_lpfnUnsubclassCtl == NULL)
	{
		m_Ctl3d = ::LoadLibrary("ctl3d32");
		if (m_Ctl3d != NULL)
			m_lpfnUnsubclassCtl = (CTL3DUNSUBCLASSCTLPROC)::GetProcAddress(m_Ctl3d, "Ctl3dUnsubclassCtl");
	}
}

void CGridCtl3dInterface::Free()
{
	if (m_Ctl3d != NULL)
	{
		::FreeLibrary(m_Ctl3d);

		m_Ctl3d = NULL;
		m_lpfnUnsubclassCtl = NULL;
	}
}

void CGridCtl3dInterface::UnsubclassCtl(HWND hWnd)
{
	if (m_lpfnUnsubclassCtl != NULL)
		(*m_lpfnUnsubclassCtl)(hWnd);
}


////////////////////////////////////////////////////////////////////////////
// CStringListEdit
class CStringListEdit : public CEdit
{
// Construction
public:
	CStringListEdit();

// Attributes
public:
	CStringListGridWnd* m_pGrid;

// Implementation
public:
	virtual ~CStringListEdit();

protected:
	afx_msg UINT OnGetDlgCode();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()
};

CStringListEdit::CStringListEdit() : CEdit()
{
}

CStringListEdit::~CStringListEdit()
{
}

BEGIN_MESSAGE_MAP(CStringListEdit, CEdit)
	ON_WM_GETDLGCODE()
	ON_WM_KEYDOWN()
	ON_WM_CREATE()
END_MESSAGE_MAP()

UINT CStringListEdit::OnGetDlgCode()
{
	return CEdit::OnGetDlgCode() | DLGC_WANTTAB | DLGC_WANTARROWS | DLGC_WANTALLKEYS;
}

void CStringListEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if ( nChar == VK_TAB || nChar == VK_RETURN || nChar == VK_ESCAPE || nChar == VK_F2)
		m_pGrid->ProcessKeyboard((MSG*)GetCurrentMessage());
	else
		CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
}

int CStringListEdit::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CEdit::OnCreate(lpCreateStruct) == -1)
		return -1;

	// Un3D the edit control - note failure is ok
	g_GridCtl3dInterface.UnsubclassCtl(this->m_hWnd);
	return 0;
}

////////////////////////////////////////////////////////////////////////////
// CStringListGridRow
BOOL AFX_EXT_DATADEF CStringListGridRow::c_bIsMove;

CStringListGridRow::CStringListGridRow(CGridWnd* pGrid)
	: CGridControlRow(pGrid), m_dwUser(0), m_nGlyph(-1)
{
}

CStringListGridRow::~CStringListGridRow()
{
	// NOTE: This must be done here instead of in CGridRow::~CGridRow
	// so that CStringListGridRow::OnActivate() is called from RemoveRow,
	// instead of CGridElement::OnActivate().
	if (m_pGrid != NULL)
		m_pGrid->RemoveRow(this);
	m_pGrid = NULL;
}

void CStringListGridRow::GetColumnText(int nColumn, CString& rStr)
{
	ASSERT(nColumn == 0);

	if (IsNewRow())
		rStr.Empty();
	else
		rStr = m_str;
}

void CStringListGridRow::OnActivate(BOOL bActivate, int nColumn)
{
	CStringListGridWnd* pGrid = (CStringListGridWnd*) GetGrid();
	if (bActivate)
	{
		if (IsNewRow())
		{
			if (!pGrid->IsFlag(GRIDLIST_NOADD))
			{
				CreateControl(edit, 0);
			}
		}
		else if (!pGrid->IsFlag(GRIDLIST_NOCHANGE))
		{
			CreateControl(edit, 0);
		}
	}
	else
	{
		if (pGrid->m_pControlWnd != NULL)
			CreateControl(none, -1);
	}
}

BOOL CStringListGridRow::OnAccept(CWnd* pControlWnd)
{
	CStringListGridWnd* pGrid = (CStringListGridWnd*) GetGrid();
	if (pControlWnd != NULL)
	{
		CString strNew;
		pControlWnd->GetWindowText(strNew);

		if (IsNewRow())
		{
			CString strOld = m_str;
			m_str = strNew;

			ASSERT(!pGrid->IsFlag(GRIDLIST_NOADD));
			BOOL bRC = TRUE;
			if (m_str.IsEmpty() || !(bRC = pGrid->OnAddString(pGrid->GetRowIndex(this))))
				m_str = strOld;
			else
			{
				pGrid->AddNewRow(!c_bIsMove);
				CWnd* pParent = pGrid->GetParent();
				ASSERT_VALID(pParent);
				pParent->PostMessage(WM_COMMAND, MAKEWPARAM(pGrid->GetDlgCtrlID(), GLN_ADDSTRING), (LPARAM)pGrid->m_hWnd);
			}	

			Invalidate();
			return bRC;
		}
		else if (strNew.Compare(m_str))
		{
			CString strOld = m_str;
			m_str = strNew;

			ASSERT(!pGrid->IsFlag(GRIDLIST_NOCHANGE));
			BOOL bRC = pGrid->OnChange(pGrid->GetRowIndex(this));
			if (!bRC)
				m_str = strOld;

			return bRC;
		}
	}

	return TRUE;
}

void CStringListGridRow::OnSelect(BOOL bSelected)
{
	CGridControlRow::OnSelect(bSelected);

	CStringListGridWnd* pGrid = (CStringListGridWnd*) GetGrid();
	CWnd* pParent = pGrid->GetParent();
	ASSERT_VALID(pParent);

	// Notify owner of sel change
	if (!CStringListGridWnd::c_bSelChangeFromCode)
		pParent->PostMessage(WM_COMMAND,
			MAKEWPARAM(pGrid->GetDlgCtrlID(), GLN_SELCHANGE),
			(LPARAM)pGrid->m_hWnd);
}

void CStringListGridRow::DrawCell(CDC* pDC, const CRect& cellRect, int nColumn)
{
	if (IsNewRow())
	{
		CRect textRect = cellRect;
		AdjustForMargins(textRect, nColumn);

		pDC->ExtTextOut(textRect.left, textRect.top, ETO_CLIPPED | ETO_OPAQUE,
			cellRect, "", 0, NULL);

		textRect.right = textRect.left + textRect.Width() / 3;
		pDC->DrawFocusRect(textRect);
	}
	else
	{
		CGridControlRow::DrawCell(pDC, cellRect, nColumn);
	}

	// Now paint our glyph
	if (m_nGlyph != -1)
	{
		CPoint ptGlyph;
		CStringListGridWnd* pGrid = (CStringListGridWnd*)GetGrid();
		ptGlyph.x = (pGrid->m_cxSelectMargin / 2) - (pGrid->m_sizeGlyph.cx / 2);
		ptGlyph.y = cellRect.top + (cellRect.Height() / 2) - (pGrid->m_sizeGlyph.cy / 2);

		pGrid->DrawGlyph(pDC, m_nGlyph, ptGlyph, 0);
	}
}

void CStringListGridRow::ResetSize(CDC* pDC)
{
	CWindowDC dc(NULL);
	TEXTMETRIC tm;

	dc.GetTextMetrics(&tm);
	m_cyHeight = tm.tmHeight + tm.tmExternalLeading + 1;
}

CEdit* CStringListGridRow::NewEdit()
{
	CStringListEdit* pEdit = new CStringListEdit;
	pEdit->m_pGrid = (CStringListGridWnd *)m_pGrid;
	return pEdit;
}


////////////////////////////////////////////////////////////////////////////
// CStringListGridWnd
BOOL CStringListGridWnd::c_bSelChangeFromCode;
CStringListGridWnd::CStringListGridWnd(UINT nModeFlags /*= 0*/)
	: CGridControlWnd(1, FALSE), m_nFlags(nModeFlags), m_pNewRow(NULL), m_bRedraw(TRUE)
{
	// Configure the grid
	EnableAutoSort(FALSE);
	if (!IsFlag(GRIDLIST_MULTISEL))
		EnableMultiSelect(FALSE);

	m_bEnableDragRow = !IsFlag(GRIDLIST_NOORDER);
	m_bEnableEllipse = IsFlag(GRIDLIST_ELLIPSE);

	CStringListGridRow::c_bIsMove = FALSE;

	m_sizeGlyph.cx = m_sizeGlyph.cy = 0;

	g_GridCtl3dInterface.Load();
	c_bSelChangeFromCode = FALSE;

}

CStringListGridWnd::~CStringListGridWnd()
{
	g_GridCtl3dInterface.Free();
}

BEGIN_MESSAGE_MAP(CStringListGridWnd, CGridControlWnd)
	//{{AFX_MSG_MAP(CStringListGridWnd)
	ON_WM_CREATE()
	ON_WM_GETDLGCODE()
	ON_BN_CLICKED(ID_GRD_NEW, OnGridNew)
	ON_BN_CLICKED(ID_GRD_DELETE, OnGridDelete)
	ON_BN_CLICKED(ID_GRD_MOVEUP, OnGridMoveUp)
	ON_BN_CLICKED(ID_GRD_MOVEDOWN, OnGridMoveDown)

	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


afx_msg void CStringListGridWnd::OnGridNew() 
{
	if (m_pNewRow != NULL)
	{
		Select(m_pNewRow);
		SetActiveElement(m_pNewRow);
	}

	CGridControlWnd::OnGridNew();
}

afx_msg void CStringListGridWnd::OnGridDelete()
{
	if (m_pControlWnd == NULL && !IsFlag(GRIDLIST_NODELETE))
	{
		DoDelete();
	}
	CGridControlWnd::OnGridDelete();
}

afx_msg void CStringListGridWnd::OnGridMoveUp()
{
	POSITION posSel = GetHeadSelPosition();
	if (posSel != NULL)
	{
		CStringListGridRow* pSelRow = (CStringListGridRow*) GetNextSel(posSel);
		if (!pSelRow->IsNewRow())
			MoveUpDown(TRUE /* fMoveUp */);
	}
	CGridControlWnd::OnGridMoveUp();
}

afx_msg void CStringListGridWnd::OnGridMoveDown()
{
	POSITION posSel = GetHeadSelPosition();
	if (posSel != NULL)
	{
		CStringListGridRow* pSelRow = (CStringListGridRow*) GetNextSel(posSel);
		if (!pSelRow->IsNewRow())
			MoveUpDown(FALSE /* fMoveUp */);
	}

	CGridControlWnd::OnGridMoveDown();
}

int CStringListGridWnd::GetCount() const
{
	if (IsFlag(GRIDLIST_NOADD))
		return GetRowCount();
	else
		return GetRowCount() - 1;
}

int CStringListGridWnd::GetCurSel() const
{
	ASSERT(!IsFlag(GRIDLIST_MULTISEL));

	POSITION pos = GetHeadSelPosition();
	if (pos == NULL)
		return -1;

	CGridRow* pRow = (CGridRow*) GetNextSel(pos);
	return GetRowIndex(pRow);
}

BOOL CStringListGridWnd::SetCurSel(int nSelect)
{
	ASSERT(!IsFlag(GRIDLIST_MULTISEL));

	CGridRow* pRow = GetRow(nSelect);
	if (pRow != NULL)
	{
		c_bSelChangeFromCode = TRUE;
		Select(pRow);
		c_bSelChangeFromCode = FALSE;

		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

/* FUTURE: NYI
int CStringListGridWnd::GetCaretIndex() const
{
}

BOOL CStringListGridWnd::SetCaretIndex(int nIndex, BOOL bScroll = TRUE)
{
}
*/

BOOL CStringListGridWnd::SetSel(int nIndex, BOOL bSelect /*= TRUE*/)
{
	ASSERT(IsFlag(GRIDLIST_MULTISEL));

	CStringListGridRow* pRow = GetRow(nIndex);
	if (pRow == NULL)
		return FALSE;

	c_bSelChangeFromCode = TRUE;
	if (bSelect)
	{
		Select(pRow, TRUE);
	}
	else
	{
		Deselect(pRow);
	}
	c_bSelChangeFromCode = FALSE;

	return TRUE;
}

int CStringListGridWnd::GetSelCount() const
{
	ASSERT(IsFlag(GRIDLIST_MULTISEL));

	return m_selection.GetCount();
}

static int CompareInts(const void* p1, const void* p2)
{
	return (*(int*)p1) - (*(int*)p2);
}

int CStringListGridWnd::GetSelItems(int nMaxItems, LPINT rgIndex) const
{
	ASSERT(IsFlag(GRIDLIST_MULTISEL));

	POSITION posSel = GetHeadSelPosition();
	int cSel = GetSelCount();
	if (cSel == 0)
		return 0;

	LPINT rgTemp = new INT[cSel];
	int nSel;
	for (nSel = 0; nSel < cSel; nSel++)
	{
		ASSERT(posSel != NULL);

		CGridRow* pRow = (CGridRow*) GetNextSel(posSel);
		rgTemp[nSel] = GetRowIndex(pRow);
	}

	// Sort based on position in the grid
	qsort(rgTemp, cSel, sizeof(int), CompareInts);

	// Put them into the passed in buffer
	int nCopy = min(nMaxItems, cSel);
	memcpy(rgIndex, rgTemp, sizeof(int) * nCopy);

	delete[] rgTemp;

	return nCopy;
}

int CStringListGridWnd::AddString(LPCTSTR lpszItem)
{
	return InsertString(-1, lpszItem);
}

int CStringListGridWnd::DeleteString(int nIndex)
{
	CGridRow* pRow = GetRow(nIndex);

	if (pRow != NULL)
		RemoveRow(pRow);

	return GetCount();
}

int CStringListGridWnd::InsertString(int nIndex, LPCTSTR lpszItem)
{
	ASSERT(m_nSortColumn == -1);

	// Allocate a new row
	CStringListGridRow* pRow = NULL;
	NewRow(pRow);
	ASSERT(pRow != NULL);

	// Set it's text
	if (lpszItem != NULL)
		pRow->m_str = lpszItem;

	// Insert it in the correct location
	POSITION posBefore = NULL;
	if (nIndex != -1)
		posBefore = m_rows.FindIndex(nIndex + 1);

	if (posBefore != NULL)
	{
		m_rows.InsertBefore(posBefore, pRow);
	}
	else
	{
		if (m_pNewRow != NULL)
		{
			POSITION pos = m_rows.Find(m_pNewRow);
			ASSERT(pos != NULL);

			m_rows.InsertBefore(pos, pRow);
		}
		else
		{
			m_rows.AddTail(pRow);
		}
	}

	if (m_bRedraw)
	{
		AdjustTopVisible();
		ResetSize(); // INEFFICIENT
		Invalidate(FALSE); // INEFFICIENT
		ResetScrollBars();
	}

	return GetRowIndex(pRow);
}

void CStringListGridWnd::ResetContent()
{
	// can't reset midway thru a drag and expect the drag to continue.
	if(m_bDragRow)
	{
		CancelDrag();
	}

	// Clearing these speeds up row deletion
	m_selection.RemoveAll();
	m_posCurRow = NULL;
	m_posTopVisibleRow = NULL;
	m_cyScroll = 0;

	ASSERT(!m_bDestruct);
	m_bDestruct = TRUE;

	// Temporarily remove the caption
	CGridRow* pCaption = GetCaptionRow();
	while (!m_rows.IsEmpty())
	{
		CGridRow* pRow = (CGridRow*) m_rows.GetTail();
		if (pRow == pCaption)
			break;

		delete pRow;
	}

	m_bDestruct = FALSE;
	AddNewRow();
}

BOOL CStringListGridWnd::GetText(int nIndex, CString& rString) const
{
	const CStringListGridRow* pRow = GetRow(nIndex);
	if (pRow == NULL)
		return FALSE;

	rString = pRow->m_str;
	return TRUE;
}

BOOL CStringListGridWnd::SetText(int nIndex, LPCTSTR lpszNew)
{
	CStringListGridRow* pRow = GetRow(nIndex);
	if (pRow == NULL)
		return FALSE;

	pRow->m_str = lpszNew;
	return TRUE;
}

DWORD CStringListGridWnd::GetItemData(int nIndex) const
{
	const CStringListGridRow* pRow = GetRow(nIndex);
	if (pRow == NULL)
		return (DWORD)-1;

	return pRow->m_dwUser;
}

BOOL CStringListGridWnd::SetItemData(int nIndex, DWORD dwItemData)
{
	CStringListGridRow* pRow = GetRow(nIndex);
	if (pRow == NULL)
		return FALSE;

	pRow->m_dwUser = dwItemData;
	return TRUE;
}

void* CStringListGridWnd::GetItemDataPtr(int nIndex) const
{
	return (void*) GetItemData(nIndex);
}

BOOL CStringListGridWnd::SetItemDataPtr(int nIndex, void* pData)
{
	return SetItemData(nIndex, (DWORD)pData);
}

int CStringListGridWnd::GetItemGlyph(int nIndex) const
{
	const CStringListGridRow* pRow = GetRow(nIndex);
	if (pRow == NULL)
		return -1;

	return pRow->m_nGlyph;
}

BOOL CStringListGridWnd::SetItemGlyph(int nIndex, int nGlyph)
{
	CStringListGridRow* pRow = GetRow(nIndex);
	if (pRow == NULL)
		return FALSE;

	pRow->m_nGlyph = nGlyph;
	return TRUE;
}

void CStringListGridWnd::SetGlyphSize(LONG cx, LONG cy)
{
	m_sizeGlyph.cx = cx;
	m_sizeGlyph.cy = cy;

	m_cxSelectMargin = max(cxSelectMargin, cx + 4);
}

void CStringListGridWnd::SetRedraw(BOOL bRedraw)
{
	// If turning on redraw, recalc stuff.
	if (!m_bRedraw && bRedraw)
	{
		AdjustTopVisible();
		ResetSize();
		Invalidate(FALSE);
		ResetScrollBars();
	}

	m_bRedraw = bRedraw;
}

BOOL CStringListGridWnd::OnChange(int nIndex)
{
	ASSERT(!IsFlag(GRIDLIST_NOCHANGE));

	return TRUE;
}

BOOL CStringListGridWnd::OnAddString(int nIndex)
{
	ASSERT(!IsFlag(GRIDLIST_NOADD));

	return TRUE;
}

BOOL CStringListGridWnd::OnDeleteString(int nIndex)
{
	ASSERT(!IsFlag(GRIDLIST_NODELETE));

	return TRUE;
}

BOOL CStringListGridWnd::OnMove(int nSrcIndex, int nDestIndex)
{
	ASSERT(!IsFlag(GRIDLIST_NOORDER));

	return TRUE;
}

void CStringListGridWnd::DrawGlyph(CDC *pDC, int nGlyph, CPoint ptGlyph, DWORD dwRop)
{
}

void CStringListGridWnd::NewRow(CStringListGridRow*& rpRow)
{
	rpRow = new CStringListGridRow(this);
}

int CStringListGridWnd::GetRowIndex(const CGridRow* pRow) const
{
	POSITION pos = GetHeadRowPosition();
	int nIndex = 0;
	while (pos != NULL)
	{
		if (GetNextRow(pos) == pRow)
			return nIndex;
		nIndex++;
	}

	return -1;
}

CStringListGridRow* CStringListGridWnd::GetRow(int nIndex)
{
	POSITION pos = nIndex == -1 ? NULL : m_rows.FindIndex(nIndex + 1);
	if (pos != NULL)
		return (CStringListGridRow*)m_rows.GetAt(pos);
	else
		return NULL;
}

const CStringListGridRow* CStringListGridWnd::GetRow(int nIndex) const
{
	POSITION pos = nIndex == -1 ? NULL : m_rows.FindIndex(nIndex + 1);
	if (pos != NULL)
		return (const CStringListGridRow*)m_rows.GetAt(pos);
	else
		return NULL;
}

void CStringListGridWnd::AddNewRow(BOOL bSelect /*= TRUE*/)
{
	if (!IsFlag(GRIDLIST_NOADD))
	{
		NewRow(m_pNewRow);
		AddRow(m_pNewRow);

		if (bSelect && !IsFlag(GRIDLIST_NONEWSELONADD))
		{
			c_bSelChangeFromCode = TRUE;
			Select(m_pNewRow);
			c_bSelChangeFromCode = FALSE;
		}
	}
}

void CStringListGridWnd::DoDelete()
{
	if (m_selection.IsEmpty())  // don't even try with empty grid
		return;

	POSITION posNewSel = NULL;
	while (!m_selection.IsEmpty())
	{
		POSITION posPrimary = GetHeadSelPosition();
		CStringListGridRow* pSelRow = (CStringListGridRow*) GetNextSel(posPrimary);

		// Don't try to delete the caption row!
		if ((CGridRow*)pSelRow == (CGridRow*)GetCaptionRow())
		{
			Deselect(pSelRow);
			continue;
		}

		if (pSelRow != m_pNewRow)
		{
			// BLOCK: Pick the correct row to select
			{
				if (posPrimary != NULL)
				{
					CStringListGridRow* pSetSel = (CStringListGridRow*) m_selection.GetAt(posPrimary);
					posNewSel = m_rows.Find(pSetSel);
				}
				else
				{
					posNewSel = m_rows.Find(pSelRow);
					m_rows.GetNext(posNewSel);
				}
			}

			// Inform of the delete
			int nIndex = GetRowIndex(pSelRow);
			if (OnDeleteString(nIndex))
			{
				RemoveRow(pSelRow); // Remove the primary selection
				delete pSelRow;

				CStringListGridWnd* pGrid = this;
				CWnd* pParent = pGrid->GetParent();
				ASSERT_VALID(pParent);
				pParent->PostMessage(WM_COMMAND, MAKEWPARAM(pGrid->GetDlgCtrlID(), GLN_DELETESTRING), (LPARAM)pGrid->m_hWnd);

			}
			else
			{
				posNewSel = m_rows.Find(pSelRow);
				Deselect(pSelRow);
			}
		}
		else
		{
			posNewSel = m_rows.Find(pSelRow);
			Deselect(pSelRow);
		}
	}

	// Redraw
	Invalidate(FALSE);
	ResetScrollBars();

	// Default to the last row
	if (posNewSel == NULL)
		posNewSel = m_rows.GetTailPosition();

	// Set the selection
	CStringListGridRow* pSelRow = (CStringListGridRow*) m_rows.GetAt(posNewSel);

	if ((CGridRow*)pSelRow == (CGridRow*)GetCaptionRow())
		{
			Deselect(pSelRow);
		}
	else
		Select(pSelRow);
}

int CompareRowIDs(const void* p1, const void* p2)
{
	return ((SORTABLEROW*)p1)->m_nRow - ((SORTABLEROW*)p2)->m_nRow;
}

void CStringListGridWnd::DoMove(int nToIndex)
{
	ASSERT(!IsFlag(GRIDLIST_NOORDER));

	if (nToIndex >= GetCount())
		nToIndex = GetCount() - 1;

	if (nToIndex < 0)
		return;

	// Notify the grid owner of these moves
	SORTABLEROW* pSelRows;
	pSelRows = new SORTABLEROW[m_selection.GetCount()];
	int cMove = 0;

	POSITION posSel = GetHeadSelPosition();
	int nDest = nToIndex;
	while (posSel != NULL)
	{
		CStringListGridRow* pSelRow = (CStringListGridRow*) GetNextSel(posSel);
		if (pSelRow->IsNewRow())
		{
			Deselect(pSelRow);
		}
		else
		{
			int nSrc = GetRowIndex(pSelRow);

			if (!OnMove(nSrc, nDest++))
			{
				Deselect(pSelRow);
				nDest--;
			}
			else
			{
				pSelRows[cMove].m_pRow = pSelRow;
				pSelRows[cMove++].m_nRow = nSrc;
			}
		}
	}

	// Remove the select rows
	posSel = GetHeadSelPosition();
	while (posSel != NULL)
	{
		CStringListGridRow* pSelRow = (CStringListGridRow*) GetNextSel(posSel);

		// Remove the row from the grid
		POSITION pos = m_rows.Find(pSelRow);
		ASSERT(pos != NULL);

		// Update m_posTopVisibleRow if needed
		if (m_posTopVisibleRow == pos)
		{
			GetPrevRow(m_posTopVisibleRow);
			if (m_posTopVisibleRow != NULL && (GetRowAt(m_posTopVisibleRow) == GetCaptionRow()))
			{
				m_posTopVisibleRow = NULL;
				m_cyScroll = 0;
			}
		}
		m_rows.RemoveAt(pos);
	}

	// Sort the row list
	qsort(pSelRows, cMove, sizeof(SORTABLEROW), CompareRowIDs);

	// Now move each row from our temp list into the grid's list of rows
	POSITION posTarget = NULL;
	for (int iCur = 0; iCur < cMove; iCur++)
	{
		// Where should we insert the row?
		if (posTarget == NULL)
		{
			if (nToIndex >= GetCount())
			{
				if (m_pNewRow != NULL)
					posTarget = m_rows.InsertBefore(m_rows.Find(m_pNewRow), pSelRows[iCur].m_pRow);
				else
					posTarget = m_rows.AddTail(pSelRows[iCur].m_pRow);
			}
			else if (nToIndex <= 0)
			{
				posTarget = m_rows.InsertAfter(m_rows.GetHeadPosition(), pSelRows[iCur].m_pRow);
			}
			else
			{
				posTarget = m_rows.InsertAfter(m_rows.FindIndex(nToIndex), pSelRows[iCur].m_pRow);
			}
		}
		else
		{
			posTarget = m_rows.InsertAfter(posTarget, pSelRows[iCur].m_pRow);
		}
	}
	delete [] pSelRows;

	AdjustTopVisible();
	ResetSize(); // INEFFICIENT
	Invalidate(FALSE); // INEFFICIENT
	ResetScrollBars();

	// Make sure the primary selection is still visible
	if (!m_selection.IsEmpty())
	{
		CGridElement* pSelElement = (CGridElement*)m_selection.GetHead();
		if (pSelElement != NULL)
		{
			CGridRow* pRow = pSelElement->GetRow();
			ASSERT(pRow != NULL);

			ScrollIntoView(pRow);
		}
	}
}



void CStringListGridWnd::MoveUpDown(BOOL fMoveUp)
{
	if (IsFlag(GRIDLIST_NOORDER))
		return;

	CStringListGridRow::c_bIsMove = TRUE;
	BOOL bAccepted = AcceptControl();
	CStringListGridRow::c_bIsMove = FALSE;

	if (bAccepted)
	{
		int iCurSel = -1;
		if (IsFlag(GRIDLIST_MULTISEL))
			GetSelItems(1, &iCurSel);
		else
			iCurSel = GetCurSel();

		DoMove(iCurSel + (fMoveUp ? -1 : 1));
	}

}


BOOL CStringListGridWnd::ProcessKeyboard(MSG* pMsg, BOOL bPreTrans /*= FALSE*/)
{
	switch (pMsg->message)
	{
	case WM_SYSKEYDOWN:
	case WM_KEYDOWN:
		switch (pMsg->wParam)
		{
		case VK_DELETE:
			if (m_pControlWnd == NULL && !IsFlag(GRIDLIST_NODELETE))
			{
				DoDelete();
				return TRUE;
			}
			break;

		case VK_UP:
		case VK_DOWN:
			if (GetKeyState(VK_MENU) < 0) 
			{
				MoveUpDown(pMsg->wParam == VK_UP);
				return TRUE;
			}

			break;

		case VK_F2:
			if (m_Ellipse.m_hWnd != NULL)
			{
				CWnd* pParent = GetParent();
				ASSERT_VALID(pParent);

				pParent->PostMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), GLN_ELLIPSE), (LPARAM)m_hWnd);
				return TRUE;
			}
			break;

		case VK_RETURN:
		case VK_ESCAPE:
			if (m_pControlWnd == NULL && !m_bDragRow)
			{
				CWnd* pParent = GetParent();
				if (pParent != NULL)
				{
					if (bPreTrans)
						pMsg->hwnd = pParent->m_hWnd;
					else
						pParent->PostMessage(pMsg->message, pMsg->wParam, pMsg->lParam);
				}

				// Need to skip CGridControlWnd, because it will reactivate the row
				return CGridWnd::ProcessKeyboard(pMsg, bPreTrans);
			}
			break;

		case VK_TAB:

			// control-tab is mdi window switching
			if (GetKeyState(VK_CONTROL) < 0)
				break;
			
			// in a dialog tab pops us out of grid. Pretrans ignores, non pretrans cluses and posts
			if (bPreTrans && m_bParentIsDialog)
				break;

			if (!AcceptControl(FALSE))
				return TRUE;

			// BLOCK: Tab between elements, or dlg controls
			{
				CGridElement* pOldActive = m_pActiveElement;

				BOOL bDoTabInDialog = (!bPreTrans && m_bParentIsDialog);

				if (!bDoTabInDialog)
				{
					if (GetKeyState(VK_SHIFT) < 0)
						ActivatePrevControl();
					else
						ActivateNextControl();
				}

				if (bDoTabInDialog || pOldActive == m_pActiveElement)
				{
					// We must be at the last (or first) element in the grid or tabbing out in a dialog
					CloseActiveElement();

					CWnd* pParent = GetParent();
					if (pParent != NULL)
					{
						if (bPreTrans)
						{
							pMsg->hwnd = pParent->m_hWnd;
						}
						else
						{
							if (m_bParentIsDialog)
							{
								ASSERT(pParent->IsKindOf(RUNTIME_CLASS(CDialog)));
								if (GetKeyState(VK_SHIFT) < 0)
									((CDialog *)pParent)->PrevDlgCtrl();	
								else
									((CDialog *)pParent)->NextDlgCtrl();
							}
							else
								pParent->PostMessage(pMsg->message, pMsg->wParam, pMsg->lParam);
						}
					}
					return CGridWnd::ProcessKeyboard(pMsg, bPreTrans); // must skip CGridControlWnd
				}

				return TRUE;
			}
		}
	}

	return CGridControlWnd::ProcessKeyboard(pMsg, bPreTrans);
}

void CStringListGridWnd::AdjustDropRow(CGridRow* &rpDropRow)
{
	ASSERT(rpDropRow != NULL);
	if (((CStringListGridRow*)rpDropRow)->IsNewRow())
	{
		POSITION pos = m_rows.GetTailPosition();
		if (pos != NULL)
			GetPrevRow(pos);

		if (pos == NULL)
			rpDropRow = NULL;
		else
			rpDropRow = GetPrevRow(pos);
	}
}

BOOL CStringListGridWnd::AcceptControl(BOOL bDeactivate /*= TRUE*/)
{
	// Call CGridControlRow::OnAccept()
	if (m_pActiveElement != NULL)
	{
		if (!((CGridControlRow*)m_pActiveElement)->OnAccept(m_pControlWnd))
			return FALSE;
	}

	// HACK!!! Set m_pActiveElement to NULL so that CGridControl::AcceptControl()
	// doesn't call CGridControlRow::OnAccept() again.
	CGridElement* pElement = m_pActiveElement;
	m_pActiveElement = NULL;

	// For dymanic row sizing:
	CGridControlWnd::AcceptControl(bDeactivate);

	// If the row is now empty, delete it
	m_pActiveElement = pElement;
	if (m_pActiveElement != NULL)
	{
		CStringListGridRow* pRow = (CStringListGridRow*) m_pActiveElement;
		if (pRow->m_str.IsEmpty() && !pRow->IsNewRow())
		{
			// Remove the row
			DoDelete();
		}
	}

	if (bDeactivate)
		SetActiveElement(NULL);

	return TRUE;
}

void CStringListGridWnd::DoDragDrop(CPoint point)
{
	CGridElement* pElement = ElementFromPoint(point);
	if (pElement == NULL)
	{
		CGridRow* pRow = (CGridRow*)m_rows.GetTail();
		if (pRow == NULL)
			return;

		pElement = pRow->ElementFromPoint(point);
	}

	ASSERT(pElement != NULL);
	CGridRow* pRow = pElement->GetRow();
	if (pRow == GetCaptionRow())
		return;

	POSITION posSelHead = GetHeadSelPosition();
	CGridRow* pSelRow = (posSelHead == NULL) ? NULL : (CGridRow*) GetNextSel(posSelHead);

	ASSERT(pRow != NULL);
	// don't try to drag the new row
	if (pRow != pSelRow )
	{
		DoMove(GetRowIndex(pRow));
	}
}

afx_msg int CStringListGridWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CGridControlWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	ResetContent();
	return 0;
}

afx_msg UINT CStringListGridWnd::OnGetDlgCode() 
{
	return CGridControlWnd::OnGetDlgCode() | DLGC_WANTARROWS | DLGC_WANTCHARS;
}

 
/////////////////////////////////////////////////////////////////////////////
// CStringListGlyphGridWnd
CStringListGlyphGridWnd::CStringListGlyphGridWnd(LONG cxGlyph, LONG cyGlyph, UINT nModeFlags /*= 0*/)
	: CStringListGridWnd(nModeFlags), m_pGlyphWell(NULL)
{
	SetGlyphSize(cxGlyph, cyGlyph);
}

CStringListGlyphGridWnd::~CStringListGlyphGridWnd()
{
	if (m_pGlyphWell != NULL)
		delete m_pGlyphWell;
}

BOOL CStringListGlyphGridWnd::LoadGlyphWell(UINT nIDWell)
{
	if (m_pGlyphWell == NULL)
		m_pGlyphWell = new CImageWell;

	if (!m_pGlyphWell->Load(nIDWell, m_sizeGlyph))
		return FALSE;
	return m_pGlyphWell->Open();
}

BOOL CStringListGlyphGridWnd::AttachGlyphWell(HBITMAP hWell)
{
	if (m_pGlyphWell == NULL)
		m_pGlyphWell = new CImageWell;

	if (!m_pGlyphWell->Attach(hWell, m_sizeGlyph))
		return FALSE;
	return m_pGlyphWell->Open();
}

void CStringListGlyphGridWnd::DrawGlyph(CDC *pDC, int nGlyph, CPoint ptGlyph, DWORD dwRop)
{
	if (nGlyph != -1)
		m_pGlyphWell->DrawImage(pDC, ptGlyph, nGlyph, dwRop);
}

