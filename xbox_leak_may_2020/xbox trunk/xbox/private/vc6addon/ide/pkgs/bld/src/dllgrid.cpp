// varswnd.cpp : implementation file
//

#include "stdafx.h"
#pragma hdrstop
#include "dllgrid.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif


// Helper functions
int DrawPathText(CDC *pDC, LPCSTR sz, CRect rect, UINT nFormat);

// Global vars
CBitmap c_checkBitmap;
CSize c_checkSize;

#define cxCheckboxMargin	3

/////////////////////////////////////////////////////////////////////////////
// CDLLGridRow
#if 0 // moved to header
CDLLGridRow::CDLLGridRow(CDLLGridWnd *pGridWnd,
						 CDLLGridRow *pParent /*= NULL */)
	: CGridControlRow(pGridWnd)
{
	m_fPreload = TRUE;
	m_fDelete = FALSE;
}	
	
CDLLGridRow::~CDLLGridRow( )
{
	// We don't allocate any data dynamically.
}
#endif

void CDLLGridRow::Update( )
{
	// Sub-classes should provide interesting behavior if need be.
}

void CDLLGridRow::GetColumnText(int nCol, CString& rStr)
{
	switch (nCol)
	{
		case COL_LOCALNAME:
			rStr = m_strLocalName;
			rStr.MakeLower();
			if (!rStr.IsEmpty())
			{
				// If no extension specified, add .dll.
				TCHAR szExt[_MAX_EXT];
				_tsplitpath(rStr, NULL, NULL, NULL, szExt);
				if (*szExt == _T('\0'))
				{
					CString strDLLExt;
					VERIFY(strDLLExt.LoadString(IDS_DLL_EXT));
					rStr += strDLLExt;
				}
			}
			break;

		case COL_REMOTENAME:
			rStr = m_strRemoteName;
			break;

		default:
			ASSERT(FALSE);
			break;
	}		
}

void CDLLGridRow::AdjustForMargins(CRect& rect, int nColumn)
{
	if (nColumn == COL_PRELOAD)
	{
		rect.right -= cxCheckboxMargin;
		rect.left += cxCheckboxMargin;
		rect.InflateRect(0, -cyDefaultGridCellMargin);
	}
	else
	{
		CGridControlRow::AdjustForMargins(rect, nColumn);
	}
}

void CDLLGridRow::OnActivate(BOOL bActivate, int nColumn)
{
	if ( bActivate )
	{
		switch (nColumn)
		{
			case COL_PRELOAD:
				GetDLLGrid()->m_nControlColumn = COL_PRELOAD;
				break;

			case COL_LOCALNAME:
				GetDLLGrid()->m_bEnableEllipse = TRUE;
				CreateControl(edit, nColumn, ES_AUTOHSCROLL);
				break;

			case COL_REMOTENAME:
				GetDLLGrid()->m_bEnableEllipse = FALSE;
				CreateControl(edit, nColumn, ES_AUTOHSCROLL);
				break;

			case -1:
				// don't hilite any column
				break;

			default:
				ASSERT(FALSE);
				break;
		}
	}
	else
	{
		CreateControl(none, -1);
		// CreateControl would normally do this when deleting the previous
		// control, but it may be a checkbox, which the generic grid doesn't use
		GetDLLGrid()->m_nControlColumn = -1;
	}
}

BOOL CDLLGridRow::OnAccept(CWnd *pControlWnd)
{
	CString str;
	CDLLGridWnd *pGrid = GetDLLGrid();
	int nCol = pGrid->m_nControlColumn;

	if (pControlWnd != NULL)
	{
		switch ( pGrid->m_nControlColumn ) 
		{
			case COL_PRELOAD:
				GetDLLGrid()->SetFocus();
				break;

			case COL_LOCALNAME:
				pControlWnd->GetWindowText(str);
				m_strLocalName = str;
				break;

			case COL_REMOTENAME:
				pControlWnd->GetWindowText(str);
				m_strRemoteName = str;
				break;

			default:
				ASSERT(FALSE);
				break;
		}
	}

	// If the user has deleted both the local and remote name, remove the row
	// (but don't ever delete the last row - it's the "new" row)
	if (this != pGrid->GetLastRow() &&
		(nCol == COL_LOCALNAME || nCol == COL_REMOTENAME) &&
		(m_strRemoteName.IsEmpty() && m_strLocalName.IsEmpty()) 
	) {

		// only the current row will be selected
		GetDLLGrid()->DoDelete();
		return TRUE;
	}

	if (this == pGrid->GetLastRow()
			&& (nCol == COL_LOCALNAME || nCol == COL_REMOTENAME)
			&& !str.IsEmpty() ) {

		pGrid->AddRow(new CDLLGridRow(pGrid));
		// Make sure the new row is added at the tail
		ASSERT( pGrid->GetLastRow() != this );
	}

	return TRUE;
}																		 

int CDLLGridRow::GetNextControlColumn(int nColumn)
{
	int nColumnCount = GetDLLGrid()->GetColumnCount();

	if (!GetDLLGrid()->m_fShowRemoteCol)
		nColumnCount--;

	return nColumn == nColumnCount - 1 ? 0 : nColumn + 1;
}

int CDLLGridRow::GetPrevControlColumn(int nColumn)
{
	int nColumnCount = GetDLLGrid()->GetColumnCount();

	if (!GetDLLGrid()->m_fShowRemoteCol)
		nColumnCount--;

	return nColumn <= 0 ? nColumnCount - 1 : nColumn - 1;
}

BOOL CDLLGridRow::OnLButtonDown(UINT nFlags, CPoint point)
{
	// First let normal grid behavior happen
	CGridControlRow::OnLButtonDown(nFlags, point);

	CRect rect;
	GetColumnRect(COL_PRELOAD, rect);
	AdjustForMargins(rect, COL_PRELOAD);
	rect.bottom = rect.top + c_checkSize.cy;
	// Is the click on a checkbox?
	if (rect.PtInRect(point))
	{
		if (GetDLLGrid()->LooksSelected(this))
		{
			GetDLLGrid()->DoToggleCheck();
		}
		else
		{
			SetCheck(m_fPreload ? 0 : 1);
		}
		return TRUE;
	}

	return FALSE;
}

// DrawPathText - helper function that draws the path line by line, since
// DrawText won't split the string after backslashes like we want it to.
int DrawPathText(CDC *pDC, LPCSTR sz, CRect rect, UINT nFormat)
{
	LPSTR szDup = _ftcsdup(sz);
	LPSTR pchNextCharToDraw = szDup;
	int cbRemaining;
	int nMaxWidth = rect.Width();
	CRect rectRemaining = rect;
	int cy = 0;
	TEXTMETRIC tm;

	// Don't use this flag, we'll split the string our way (between slashes)
	ASSERT((nFormat & DT_WORDBREAK) == 0);

	pDC->GetTextMetrics(&tm);
	int cySingleLine = tm.tmHeight + tm.tmExternalLeading;

	while (*pchNextCharToDraw)
	{
		cbRemaining = _ftcslen(pchNextCharToDraw);

		while (TRUE)
		{
			//
			// We'll exit this loop when we know what to draw on the next line
			//
			LPCSTR p;
			int chTmp;
			CRect rectCurrent;

			pDC->DrawText(pchNextCharToDraw, cbRemaining, rectCurrent,
				DT_CALCRECT | DT_NOPREFIX);

			ASSERT(rectCurrent.Height() <= cySingleLine);

			// It fits!  Draw it.			
			if (rectCurrent.Width() <= nMaxWidth) {
				break;
			}

			// Temporarily end the string prematurely so _ftcsrchr finds the 
			// next backslash
			chTmp = pchNextCharToDraw[cbRemaining-1];
			pchNextCharToDraw[cbRemaining-1] = _T('\0');
			p = _ftcsrchr(pchNextCharToDraw, _T('\\'));
			pchNextCharToDraw[cbRemaining-1] = chTmp;

			if (p == NULL) {
				// can't shrink this string anymore, just print it
				break;
			}

			// Chop of the last dir and see if the remaining string fits
			cbRemaining = p - pchNextCharToDraw + 1;
		}

		if (nFormat & DT_CALCRECT) {
			cy += cySingleLine;
		} else {
			cy = pDC->DrawText(pchNextCharToDraw, cbRemaining, rectRemaining, DT_NOPREFIX);
			ASSERT(cy == cySingleLine);
			rectRemaining.top += cy;
		}

		pchNextCharToDraw += cbRemaining;
	}

	free(szDup);
	return cy;
}

// Need to override DrawCell because CGridControlRow will always call
// GetColumnText, which of course doesn't make sense for a checkbox.
// To move this functionality into grid.cpp we need need a virtual func
// GetColumnGridType that, given a column, returns the grid type.  That
// way DrawCell could find out what type the column is and call the correct
// GetColumn* (Text | Check | ?).
void CDLLGridRow::DrawCell(CDC *pDC, const CRect& cellRect, int nColumn)
{
	switch (nColumn)
	{
		case COL_PRELOAD:
		{
			// clear the background
			pDC->ExtTextOut(0,0, ETO_OPAQUE, cellRect, NULL, 0, NULL);

			// Hide the checkbox in the last row, unless the last row is being changed
			if (!(this == GetDLLGrid()->GetLastRow() &&
				(!GetDLLGrid()->LooksSelected(this) || !GetDLLGrid()->m_pActiveElement)))
			{
				CRect rect;
				GetColumnRect(COL_PRELOAD, rect);
				AdjustForMargins(rect, COL_PRELOAD);
				rect.bottom = rect.top + c_checkSize.cy;

				CDC bitmapDC;
				if (bitmapDC.CreateCompatibleDC(pDC))
				{
					int nCheck = GetCheck();
					CBitmap* pOldBitmap = bitmapDC.SelectObject(&c_checkBitmap);
					
					int xSrc, ySrc, xDest, yDest;
					if (theApp.m_bWin4)
					{
						xSrc = c_checkSize.cx * nCheck;
						ySrc = 0;
						xDest = 1;
						yDest = 0;
					}
					else
					{
						xSrc = c_checkSize.cx * nCheck;
						ySrc = 0;
						xDest = 1;
						yDest = 1;
					}
					
					pDC->BitBlt(rect.left + xDest, 
						rect.top + yDest /*+ max(0, (m_cyItem - c_checkSize.cy) / 2)*/, 
						c_checkSize.cx, c_checkSize.cy, 
						&bitmapDC, xSrc, ySrc, SRCCOPY);
					bitmapDC.SelectObject(pOldBitmap);
				}
			}
			break;
		}

		case COL_LOCALNAME:
		case COL_REMOTENAME:
		{
			CRect textRect = cellRect;
			CString str;

			GetColumnText(nColumn, str);
			AdjustForMargins(textRect, nColumn);

			// clear the background
			pDC->ExtTextOut(0,0, ETO_OPAQUE, cellRect, NULL, 0, NULL);

			// The value column can be multi-row if need be, but we need to 
			// split the rows manually after backslashes (since there
			// probably aren't any spaces).
			DrawPathText(pDC, str, textRect, 0);

			// Draw a focus rect in the local column of the last row
			if (this == GetDLLGrid()->GetLastRow() && nColumn == COL_LOCALNAME)
			{
				pDC->DrawFocusRect(textRect);
			}

			break;
		}

		default:
			CGridControlRow::DrawCell(pDC, cellRect, nColumn);
			break;
	}
}

void CDLLGridRow::ResetSize(CDC *pDC)
{
	//CWindowDC dc(NULL);	
	TEXTMETRIC tm;
	int nWidth;
	int nHeight;
	CRect rect;
	CString str;
	
	pDC->GetTextMetrics(&tm);
	m_cyHeight = tm.tmHeight + tm.tmExternalLeading + 3;

	// First do the Local DLL column
	nWidth = GetDLLGrid()->GetColumnWidth(COL_LOCALNAME) - 2 * cxDefaultGridCellMargin;
	rect.SetRect(0, 0, nWidth, 0);
	GetColumnText(COL_LOCALNAME, str);
	nHeight = 2 * cyDefaultGridCellMargin + DrawPathText(pDC, str, rect, DT_CALCRECT);
	m_cyHeight = max(m_cyHeight, nHeight);

	// Next do the Remote DLL column
	nWidth = GetDLLGrid()->GetColumnWidth(COL_REMOTENAME) - 2 * cxDefaultGridCellMargin;
	rect.SetRect(0, 0, nWidth, 0);
	GetColumnText(COL_REMOTENAME, str);
	nHeight = 2 * cyDefaultGridCellMargin + DrawPathText(pDC, str, rect, DT_CALCRECT);
	m_cyHeight = max(m_cyHeight, nHeight);
}

CSize CDLLGridRow::GetCellSize(CDC *, int) const
{
	CSize size;
	
	size.cy = m_cyHeight;
	// This row is flexible so we will not make any size demands.
	size.cx = 0;		

	return size;
}
					 	
void CDLLGridRow::SetCheck(int nCheck)
{
	m_fPreload = nCheck ? TRUE : FALSE;
	Invalidate();
}

/////////////////////////////////////////////////////////////////////////////
// CDLLGridWnd

CDLLGridWnd::CDLLGridWnd(int nColumns, BOOL bDisplayCaption)
	: CGridControlWnd(nColumns, bDisplayCaption)
{
	// DLL-like grids should not be sorted by other criterion.
	EnableSort(FALSE);
	EnableAutoSort(FALSE); 
	m_bNewRow = FALSE;
	m_bEnableEllipse = TRUE;

	m_bShowContextPopup = TRUE;

	// Get the size of the Preload checkbox
	if (c_checkBitmap.m_hObject == NULL)
	{
		BITMAP bitmap;

		VERIFY(c_checkBitmap.LoadOEMBitmap(OBM_CHECKBOXES));
		c_checkBitmap.GetObject(sizeof (BITMAP), &bitmap);
		c_checkSize.cx = bitmap.bmWidth / 4;
		c_checkSize.cy = bitmap.bmHeight / 3;
	}
	//AddRow(new CDLLGridRow(this));
}

CDLLGridWnd::~CDLLGridWnd()
{
	// The CGridWnd destructor deletes the rows so we don't
	// need to do the destruction.
}


BEGIN_MESSAGE_MAP(CDLLGridWnd, CDLLGridWnd::CBaseClass)
	//{{AFX_MSG_MAP(CDLLGridWnd)
	ON_WM_PAINT()
	ON_WM_DESTROY()
	ON_WM_GETDLGCODE()
	ON_WM_CREATE()
	ON_BN_CLICKED(ID_GRD_NEW, OnGridNew)
	ON_BN_CLICKED(ID_GRD_DELETE, OnGridDelete)

	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDLLGridWnd Attributes


/////////////////////////////////////////////////////////////////////////////
// CDLLGridWnd Operations

int CDLLGridWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CGridControlWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect Rect;
	CString strHeader;
	int cxPreload, cx;

	// Set the column captions...
#if 0
	VERIFY(strHeader.LoadString(IDS_DLL_COL_PRELOAD));
	SetColumnCaption(COL_PRELOAD, strHeader);
	cxPreload = (int)GetMinColumnWidth(strHeader);
#else
	SetColumnCaption(COL_PRELOAD, "");

	// Make this column just big enough for the box and margins
	cxPreload = c_checkSize.cx + 2*cxCheckboxMargin;
#endif

	VERIFY(strHeader.LoadString(IDS_DLL_COL_LOCALNAME));
	SetColumnCaption(COL_LOCALNAME, strHeader);

	VERIFY(strHeader.LoadString(IDS_DLL_COL_REMOTENAME));
	SetColumnCaption(COL_REMOTENAME, strHeader);

	// Set the column widths (use the minimum for preload and split the
	// rest of the space between the local and remote columns)

	SetColumnWidth(COL_PRELOAD, cxPreload);
	GetCaptionRow()->SetColumnFixedWidth(COL_PRELOAD);

	GetCaptionRow()->GetRect(Rect);
	cx = Rect.Width() - cxPreload;
	SetColumnWidth(COL_LOCALNAME, cx/2);
	cx -= cx/2;
	SetColumnWidth(COL_REMOTENAME, cx);

	return 0;
}

BOOL CDLLGridWnd::SetCurSel(int nSelect)
{
	CDLLGridRow* pRow = (CDLLGridRow *)GetRow(nSelect);
	if (pRow != NULL)
	{
		Select(pRow);
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

/****************************************************************************

	FUNCTION:	GetMinColumnWidth()

	PURPOSE:	Calculates the smallest possible column width based
				on the size of of the fixed font, so that they line up
				nicely with the text in the listbox.

****************************************************************************/
long CDLLGridWnd::GetMinColumnWidth(CString strCaption)
{
	HFONT hFont = (HFONT)GetCaptionFont()->m_hObject;
	HDC hdc = CreateIC("DISPLAY", NULL, NULL, NULL);
	SIZE size;

	ASSERT(hdc != NULL);
	SelectObject(hdc, (HGDIOBJ) hFont);

	GetTextExtentPoint32(hdc, strCaption, strCaption.GetLength(), &size);
	VERIFY(DeleteDC(hdc));
	return (size.cx + 6); // add back for some calls to InsetRect
}

CDLLGridRow * CDLLGridWnd::GetCurSel( )
{
	POSITION pos = GetHeadSelPosition();

	// We should have exactly one item selected.
	if ( pos != NULL &&  (GetNextSel(pos), pos == NULL))
		return GetRowAt(m_posCurRow);
	else
		return NULL;
}

						
CDLLGridRow * CDLLGridWnd::GetRow(int nIndex /* = -1*/ )
{
	if ( nIndex == -1 )
		return GetCurSel();
	else
		return (CDLLGridRow *)CGridControlWnd::GetRow(nIndex); 
}

void CDLLGridWnd::DoDelete()
{
	if (m_selection.IsEmpty())  // don't even try with empty grid
		return;

	POSITION posNewSel = NULL;
	while (!m_selection.IsEmpty())
	{
		POSITION posPrimary = GetHeadSelPosition();
		CDLLGridRow* pSelRow = (CDLLGridRow*) GetNextSel(posPrimary);

		if (pSelRow != GetLastRow())
		{
			// BLOCK: Pick the correct row to select
			{
				if (posPrimary != NULL)
				{
					CDLLGridRow* pSetSel = (CDLLGridRow*) m_selection.GetAt(posPrimary);
					posNewSel = m_rows.Find(pSetSel);
				}
				else
				{
					posNewSel = m_rows.Find(pSelRow);
					m_rows.GetNext(posNewSel);
				}
			}

			// Inform of the delete
			//int nIndex = GetRowIndex(pSelRow);
			//if (OnDeleteString(nIndex))
				RemoveRow(pSelRow); // Remove the primary selection
				delete pSelRow;
		}
		else
		{
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
	CDLLGridRow* pSelRow = (CDLLGridRow*) m_rows.GetAt(posNewSel);
	ASSERT((CGridRow*)pSelRow != (CGridRow*)GetCaptionRow());

	SetActiveElement(pSelRow);
	Select(pSelRow);
}

void CDLLGridWnd::DoToggleCheck()
{
	POSITION pos = GetHeadSelPosition();
	CDLLGridRow* pSelRow;
	BOOL fSomeChecked = FALSE;

	SetFocus();

	// Pass1: are any boxes checked?
	while (pos != NULL)
	{
		pSelRow = (CDLLGridRow*) GetNextSel(pos);
		if (pSelRow != GetLastRow())
		{
			// Toggle the checkbox
			fSomeChecked |= pSelRow->m_fPreload;
		}
	}

	// Pass2: set all selected checkboxes
	pos = GetHeadSelPosition();
	while (pos != NULL)
	{
		pSelRow = (CDLLGridRow*) GetNextSel(pos);
		if (pSelRow != GetLastRow())
		{
			pSelRow->SetCheck(fSomeChecked ? 0 : 1);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CDLLGridWnd diagnostics

#ifdef _DEBUG
void CDLLGridWnd::AssertValid() const
{
	// Sorted DLL control doesn't make sense.
	// ASSERT(m_nSortColumn == -1);
	CGridControlWnd::AssertValid();
}

void CDLLGridWnd::Dump(CDumpContext& dc) const
{
	CGridControlWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CDLLGridWnd message handlers


void CDLLGridWnd::OnPaint() 
{
	if (m_fEnabled) {
		CGridControlWnd::OnPaint();	// pass on to our base instance
	} else {
		
		CPaintDC dc(this);
		CGridRow* pRow;
		CRect rowRect;
		GetClientRect(rowRect);

		// Draw the captions first
		pRow = GetCaptionRow();
		rowRect.bottom = rowRect.top + pRow->GetRowHeight();
		if (dc.m_ps.rcPaint.top < rowRect.bottom)
		{
			CFont* pOldFont = dc.SelectObject(GetCaptionFont( ));
			pRow->Draw(&dc, rowRect);
			dc.SelectObject(pOldFont);
		}
	
		// Setup clip rect around the actual grid (client less captions)
		rowRect.top = rowRect.bottom;
		rowRect.bottom = dc.m_ps.rcPaint.bottom;

		dc.SetBkColor(GetSysColor(COLOR_BTNFACE));
		dc.ExtTextOut(0, 0, ETO_OPAQUE, rowRect, NULL, 0, NULL);
		
	}
}


void CDLLGridWnd::OnDestroy() 
{
	CGridControlWnd::OnDestroy();
}


BOOL CDLLGridWnd::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{

	// HACK begets HACK. CGridControlWnd bypasses command handlers
	// in the grid window if no control is enabled. We need to handle
	// commands even when no control is available. So directly
	// call CGridWnd's OnCmdMsg.	
	return CGridWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

afx_msg UINT CDLLGridWnd::OnGetDlgCode() 
{
	return CGridControlWnd::OnGetDlgCode() | DLGC_WANTARROWS | DLGC_WANTCHARS;
}

afx_msg void CDLLGridWnd::OnGridNew() 
{
	// activate the new row
	CDLLGridRow* pSelRow = GetLastRow();
	if (pSelRow != NULL)
	{
		Select(pSelRow);
		// Auto-activate the edit box in the Local Name column
		ActivateNextControl();
		ActivateNextControl();

	}	

	CGridControlWnd::OnGridNew();
}

afx_msg void CDLLGridWnd::OnGridDelete()
{
	if (m_pActiveElement == NULL)
	{
		DoDelete();
	}
	CGridControlWnd::OnGridDelete();
}


BOOL CDLLGridWnd::ProcessKeyboard(MSG* pMsg, BOOL bPreTrans /*= FALSE*/)
{
	switch (pMsg->message)
	{
	case WM_KEYDOWN:
		switch (pMsg->wParam)
		{
		case VK_DELETE:
			if (m_pActiveElement == NULL)
			{
				DoDelete();
				return TRUE;
			}
			break;
		case VK_F2:
			if (!m_pActiveElement && m_selection.GetCount())
			{
				// Auto-activate the edit box in the Local Name column
				ActivateNextControl();
				ActivateNextControl();

				// The Local Name cell should now be active
				ASSERT( m_pControlWnd->IsKindOf( RUNTIME_CLASS( CEdit ) ) );
				ASSERT(m_nControlColumn == COL_LOCALNAME);
				((CEdit *)m_pControlWnd)->SetSel( 0, -1 );

				// Don't use CGridControlWnd - it'll try to auto-activate
				return TRUE;
			}
			break;
		case VK_RETURN:
		case VK_ESCAPE:
			if (m_pActiveElement == NULL)
			{
				CWnd* pParent = GetParent();
				if (pParent != NULL)
				{
					ASSERT(bPreTrans); // If this ever fails, we need to re-send the message to hwndChild
					pMsg->hwnd = pParent->m_hWnd;
				}
				return CGridWnd::ProcessKeyboard(pMsg, bPreTrans); // must skip CGridControlWnd
			}
			break;

		case VK_TAB:
			// control-tab is mdi window switching
			if (GetKeyState(VK_CONTROL) < 0)
				break;

			// in a dialog tab pops us out of grid. Pretrans ignores, non pretrans cluses and posts
			if (m_pActiveElement == NULL && m_bParentIsDialog)
				break;

			if (!AcceptControl(FALSE))
				return TRUE;

			// BLOCK: Tab between elements, or dlg controls
			{
				CGridElement* pOldActive = m_pActiveElement;
				int nOldColumn = m_nControlColumn;

				BOOL bDoTabInDialog = m_bParentIsDialog;

				if (!bDoTabInDialog)
				{
					if (GetKeyState(VK_SHIFT) < 0)
						ActivatePrevControl();
					else
						ActivateNextControl();
				}

				if (bDoTabInDialog || (pOldActive == m_pActiveElement && nOldColumn == m_nControlColumn))
				{
					// We must be at the last (or first) element in the grid
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

		case VK_UP:
		case VK_DOWN:
		case VK_RIGHT:
		case VK_LEFT:
			// Don't let the user "arrow" out of a checkbox
			if (m_nControlColumn == COL_PRELOAD)
			{
				ASSERT(m_pActiveElement);
				ASSERT(m_pControlWnd == NULL);
				return TRUE;
			}
		}
		break;

	case WM_CHAR:
		// We handle WM_CHAR to auto-activate the Local Name column
		// (default behavior would activate the Preload column).

		BOOL bEscape = (LOBYTE(pMsg->wParam) == VK_ESCAPE);
		BOOL bReturn = (LOBYTE(VkKeyScan(pMsg->wParam)) == VK_RETURN);
		BOOL bSpace =  (LOBYTE(pMsg->wParam) == VK_SPACE);

		// Auto-toggle the checkbox(es)
		if (bSpace && (!m_pActiveElement || m_nControlColumn == COL_PRELOAD))
		{
			DoToggleCheck();
			return TRUE;
		}

		// Auto activation
		if (!m_pActiveElement && m_selection.GetCount() == 1 && !bEscape && !bReturn)
		{
			// Auto-activate the edit box in the Local Name column
			ActivateNextControl();
			ActivateNextControl();

			// The Local Name cell should now be active
			ASSERT( m_pControlWnd->IsKindOf( RUNTIME_CLASS( CEdit ) ) );
			ASSERT(m_nControlColumn == COL_LOCALNAME);
			((CEdit *)m_pControlWnd)->SetSel( 0, -1 );
	
			HWND hwndChild = m_pControlWnd->m_hWnd;
			ASSERT(hwndChild);
			if (bPreTrans)
			{
				pMsg->hwnd = hwndChild;
			}
			else
			{
				::PostMessage(hwndChild, pMsg->message, pMsg->wParam, pMsg->lParam);
			}
			// Don't use CGridControlWnd - it'll try to auto-activate
			return CGridWnd::ProcessKeyboard(pMsg, bPreTrans);
		}
		break;
	}

	return CGridControlWnd::ProcessKeyboard(pMsg, bPreTrans);
}

