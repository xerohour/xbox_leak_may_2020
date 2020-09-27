// watchrow.cpp : implementation file
//

#include "stdafx.h"
#pragma	  hdrstop	

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#define new	DEBUG_NEW
#endif


CImageWell CWatchRow::s_imgPlusMinus;
int CWatchRow::s_cUsage;


/////////////////////////////////////////////////////////////////////////////
// CWatchRow


CWatchRow::CWatchRow ( CTreeGridWnd * pGrid, CTM *pTM, CTreeGridRow *pParent)
	: CBaseClass ( pGrid, pParent )
{
	m_pTM = pTM;	
	m_bValueBeingEdited  = FALSE ;  // TREAD carefully when changing anything about this variable.
	m_bHasSynthChild	= FALSE;
	m_bValueChanged		= FALSE;

	if (!s_cUsage++)
	{
		s_imgPlusMinus.LoadSysColor(IDB_VARSEXPAND, CSize(cxBitmap, cyBitmap));
		s_imgPlusMinus.Open();
		pGrid->m_cxSelectMargin = cxBeforeBmap ;
	}		
}


CWatchRow::~CWatchRow ( )
{
	delete m_pTM;
	if (!--s_cUsage)
	{
		s_imgPlusMinus.Close();
		s_imgPlusMinus.Unload();
	}		
}

CWatchRow * CWatchRow::NewWatchRow(CTreeGridWnd * pGrid, CTM *pTM, CTreeGridRow *pParent)
{
	return new CWatchRow(pGrid, pTM, pParent);
}

int CWatchRow::GetNextControlColumn ( int nColumn )
{
	ASSERT(COL_NAME < COL_VALUE);

	CDbgGridWnd *pGrid = (CDbgGridWnd *)GetTreeGrid();

	if ( nColumn < COL_NAME )
	{
		if (GetTM()->IsNameEditable() && pGrid->IsEditable())
			return COL_NAME;
		else if (GetTM()->IsValueEditable())
			return COL_VALUE;
		else
			return -1;
	}			
	else if ( nColumn == COL_NAME )
	{
		if (GetTM()->IsValueEditable())
			return COL_VALUE;
		else
			return -1;	
	}
	else
	{
		return -1;
	}
}

int CWatchRow::GetPrevControlColumn ( int nColumn )
{
	CDbgGridWnd *pGrid = (CDbgGridWnd *)GetTreeGrid();

	if ( nColumn == -1 )
	{
		if (GetTM()->IsValueEditable())
			return COL_VALUE;
		else if (GetTM()->IsNameEditable() && pGrid->IsEditable())
			return COL_NAME;
		else
			return -1;
	}
	else if (nColumn == COL_VALUE)
	{
		if (GetTM()->IsNameEditable() && pGrid->IsEditable())
		 	return COL_NAME;
		else
			return -1;
	}
	else
	{
		return -1;
	}		
}					
	

// This is called to supply text for default column drawing as
// well as to fill the edit-box

void CWatchRow::GetColumnText( int nColumn, CString& str )
{
	CTM *pTM = GetTM( );
	
	switch ( nColumn )
	{
		case COL_NAME:
			str = pTM->GetName( );
			break;

		case COL_VALUE:
			if ( m_bValueBeingEdited  )
			{
				str = pTM->GetEditableValue( );
			}
			else
			{
				str = pTM->GetValue( );
			}
			break;
		default:
			ASSERT(FALSE);
	}
}		

void CWatchRow::GetValueStr( CString & str)
{
	str = GetTM()->GetValue();
}

int CWatchRow::Compare(const CGridRow *pCompareRow) const
{
	CString strRow, strCompareRow;

	// Currently we have no way to deal with child rows.
	ASSERT(m_nLevels == 0);

	// We should never be sorting two rows which are at different levels.
	ASSERT(m_nLevels == ((CTreeGridRow *)pCompareRow)->m_nLevels);

	strRow = ((CWatchRow *)this)->GetTM()->GetName();
	strCompareRow = ((CWatchRow *)pCompareRow)->GetTM()->GetName();

	int nIndex;

	// remove leading *'s and &'s from the first string.
	for (nIndex = 0; nIndex < strRow.GetLength() ; nIndex++)
	{
		TCHAR tch = strRow[nIndex];

		if (tch != '*' && tch != '&')
			break;
	}

	ASSERT(nIndex < strRow.GetLength());

	strRow = strRow.Mid(nIndex);

	// trim the second string as well.
	for (nIndex = 0; nIndex < strCompareRow.GetLength() ; nIndex++)
	{
		TCHAR tch = strCompareRow[nIndex];

		if (tch != '*' && tch != '&')
			break;
	}

	ASSERT(nIndex < strCompareRow.GetLength());
	strCompareRow = strCompareRow.Mid(nIndex);

	return strRow.CompareNoCase(strCompareRow);
}

// This is called when the user types in a new value for a variable.

BOOL  CWatchRow::OnAccept ( CWnd* pControlWnd  )
{
	if ( pControlWnd != NULL )
	{
		CString strNew ;
		CTM * pTM = GetTM( );
		pControlWnd->GetWindowText(strNew);

		// Which column are we editing.

		switch ( GetTreeGrid()->m_nControlColumn )
		{
			case COL_NAME:
				// Don't do anything if the string didn't change.
				if (pTM->GetName( ) == strNew )
					return TRUE ;
				
				if ( strNew.IsEmpty( ) )
				{
					GetTreeGrid()->DirtyRow(this, TGF_DIRTY_DELETE);
					return TRUE;
				}
				// Ok new name, we need to replace our TM.
				delete m_pTM;
				m_pTM = new CRootTM(strNew);
	
				{
				int tgf = TGF_DIRTY_ITEM;
				if ( GetTreeGrid()->IsExpanded(this) )
					tgf |= TGF_DIRTY_CONTENT;

				GetTreeGrid( )->DirtyRow(this, tgf);
				}
					
				if ( m_pTM->HadSideEffects() )
				{
					OnModify();
				}

				break;
			case COL_VALUE:
				m_bValueBeingEdited = FALSE;
				// If the string hasn't changed don't update the value
				if ( pTM->GetEditableValue( ) == strNew )
					return TRUE ;

				// Tell the CWtchPane to update the expression..
				if ( ! GetTM()->ModifyValue(strNew) )
				{
					// not succesful in updating the value.
					// display message box indicating there is a problem.
					// FUTURE: use MessageBeep instead of MessageBox,
					// till setting up msg boxes from docking windows
					// works correctly.
					::MessageBeep(0);
					return GetTreeGrid()->m_bForceAccept;
				}
				else
				{
					OnModify();
				}
				break ;
			default:
					ASSERT(FALSE);
		}
	}
	return ( TRUE );
}

void CWatchRow::OnModify()
{
	// Default action is to update all the debugger windows, whenever a value is modified.
	::UpdateDebuggerState(UPDATE_ALLDBGWIN);
}

void CWatchRow::OnControlNotify( UINT nControl, UINT nNotification)
{
	CTreeGridWnd *pGrid = GetTreeGrid( );

	switch ( pGrid->m_controlType)
	{
		case edit:
			if (nNotification == EN_KILLFOCUS)
			{
				CWnd *pNewWnd = CWnd::GetFocus( );

				// This code causes the auto-commit of the active control
				// if the user clicks in some other window.
				// pNewWnd != NULL => don't commit if focus is lost to another app.
				//
				if (  pNewWnd != NULL && (pNewWnd != pGrid) &&
					!pGrid->IsChild(pNewWnd) && !pNewWnd->IsChild(pGrid))
				{
					pGrid->PostMessage(WM_DISMISS_CONTROL);
					return;
				}
			}
			break;
	}
	CBaseClass::OnControlNotify(nControl, nNotification);
}
			
		
void CWatchRow::OnActivate(BOOL bActivate, int nColumn )
{
	if ( bActivate )
	{
		ASSERT(GetTreeGrid()->IsKindOf(RUNTIME_CLASS(CDbgGridWnd)));

		CDbgGridWnd *pGrid = (CDbgGridWnd *)GetTreeGrid();

		switch ( nColumn )
		{
			case COL_NAME:
				if ( DebuggeeAlive() && !DebuggeeRunning() &&
					 GetTM( )->IsNameEditable( ) &&
					 pGrid->IsEditable() )
				{
					CreateControl(edit, COL_NAME, ES_AUTOHSCROLL);
				}
				break ;
			case COL_VALUE:
				{
					CTM * pTM  = GetTM( );

					// If the value field is editable, create the edit control
					if ( DebuggeeAlive() && !DebuggeeRunning() &&
							pTM->IsValueEditable( ) )
					{	
						// FUTURE: refer to code in GetColumnRect which uses
						// m_bValueBeingEdited. The value should be assigned
						// before calling CreateControl. BAD HACK
						// this depends on the understanding of how the grid
						// control itself works.
						m_bValueBeingEdited = TRUE ;	
						CreateControl(edit, COL_VALUE, ES_MULTILINE);
					}
					break;
				}
			default:
				ASSERT(nColumn == -1);
				break;
		}
		// If we didn't create a control we don't want to be the
		// active element. This is neccessary because otherwise
		// clicking on the row header causes a visible state change
		// as the currently active element gets a wider highlight
		// then ordinary selected elements.
		if ( pGrid->m_pControlWnd == NULL )
			pGrid->SetActiveElement(NULL);
	}
	else
	{
		m_bValueBeingEdited = FALSE;
		CreateControl(none, -1);
	}
}

void CWatchRow::ResetSize ( CDC *pDC )
{
	
	CRect rect(0,0, GetGrid()->GetColumnWidth(COL_VALUE) - 2 * cxMargin, 0);
	CString str;
	GetValueStr(str);

	UINT dt = DT_CALCRECT|DT_NOPREFIX;

	// Strings in error are only displayed in single-line. This is so
	// useless error messages don't take up too much space.
	if ( GetTM()->IsValueValid() )
		dt |= DT_WORDBREAK;
	else
		dt |= DT_SINGLELINE;

	// Black Magic that I don't understand. If the row's height is less 
	// than the number we get from the calculation below, the edit control
	// that comes up in the grid doesn't have anything visible on certain
	// settings ( NT 3.5 1024*768 Large Fonts). Similar logic is found in
	// the version editor and the use of CWindowDC dc(NULL) is common in the
	// grid control code as well. Need to figure out what the real deal here is.
	// [sanjays]

	TEXTMETRIC tm;
	CWindowDC dc(NULL);
	dc.GetTextMetrics(&tm);
	int cyMinHeight = tm.tmAscent + tm.tmInternalLeading + 1;

	if ( cyMinHeight < cyMinPlusColHeight )
		cyMinHeight = cyMinPlusColHeight;

	m_cyHeight = 2 * cyMargin + pDC->DrawText(str, str.GetLength(), rect, dt);

	if ( m_cyHeight < cyMinHeight)
		m_cyHeight = cyMinHeight;

	// m_sizeM = pDC->GetTextExtent("M", 1);
}

void CWatchRow::AdjustForMargins( CRect& rect, int nColumn )
{
	// The default margins used by the CGridWnd class are too big
	// for us, so we use our own.
	rect.InflateRect(-cxMargin, -cyMargin);
}
	
CSize  CWatchRow::GetCellSize(CDC* pDC, int nColumn)	 const
{
	// Calculate size to display the text .
	CSize size ;

	if ( nColumn == COL_NAME )
	{
		CString str;
		// Cast away the constness so we can call this function
		// to get the text for this column.
		((CWatchRow *)this)->GetColumnText(nColumn, str);				
		size = 	pDC->GetTextExtent(str, str.GetLength( ));
	 	size.cx += 2 * cxMargin ;
		// Now add the space for the +/- portion.
		size.cx += GetPlusColWidth() ;
		// size.cx += m_nLevels * m_sizeM.cx;
	}
	else if (nColumn  == COL_VALUE)
	{
		size.cx = GetGrid()->GetColumnWidth(COL_VALUE);
	}
	
	// Should be the same as the height we already indicated.
	size.cy = m_cyHeight;
		
	return size;
}

CRect CWatchRow::ComputeGlyphRect(CDC *pDC, const CRect& cellRect)
{
	TEXTMETRIC tm;
	pDC->GetTextMetrics(&tm);

	CRect glyphRect(cellRect);

	AdjustForMargins(glyphRect, COL_NAME);

	int height = tm.tmHeight + tm.tmExternalLeading;

	// We want to center the glyph assuming that we have single-line row,
	// so assume that the row height is just the char height.
	glyphRect.bottom = glyphRect.top + height;

	// Factor in the fact that we might have increased the size beyond the size reqd by the
	// font to make the glyph look good.
	if ( height + 2 * cyMargin < cyMinPlusColHeight )
		glyphRect.bottom += cyMinPlusColHeight - (height + 2 * cyMargin);

	
	glyphRect.left = cxBeforeBmap + m_nLevels * cxBitmap ; // x-placement is static.
	glyphRect.top += (glyphRect.Height() - cyBitmap)/2;

	glyphRect.right = glyphRect.left + cxBitmap;
	glyphRect.bottom = glyphRect.top + cyBitmap;

	// Rememeber the displacement of the glyph rectangle from the top-left border of the cell.
	m_dispGlyph.cx = glyphRect.left - cellRect.left;
	m_dispGlyph.cy = glyphRect.top - cellRect.top;

	return glyphRect;
}


CRect CWatchRow::GetGlyphRect(const CRect& cellRect) const
{
	CRect glyphRect;

	glyphRect.left = cellRect.left + m_dispGlyph.cx;
	glyphRect.top = cellRect.top + m_dispGlyph.cy;
	glyphRect.right = glyphRect.left + cxBitmap;
	glyphRect.bottom = glyphRect.top + cyBitmap;

	return glyphRect;
}

void CWatchRow::DrawCell( CDC *pDC , const CRect& cellRect, int nColumn )
{

	COLORREF rgbForeground = pDC->SetTextColor(GetSysColor(COLOR_BTNSHADOW));
	CBrush* pOldBrush = pDC->SelectObject(CDC::GetHalftoneBrush());

	// Cell Rect excluding border regions if any.
	CRect cellRectSansBorder(cellRect);

	// Draw the border, each cell takes responsibility for painting
	// their bottom and right edges.

	pDC->PatBlt(cellRect.left, cellRect.bottom -1,  cellRect.Width(), 1, PATCOPY);

	// Exclude the border region.
	cellRectSansBorder.bottom -= 1;

	// The last column doesn't need the vertical border
	//  as the frame window has a border.	
	if ( (nColumn + 1) !=  GetGrid()->GetColumnCount() )
	{
		pDC->PatBlt(cellRect.right -1, cellRect.top, 1, cellRect.Height(), PATCOPY);
		cellRectSansBorder.right -= 1 ;
	}
	
	pDC->SelectObject(pOldBrush);
	pDC->SetTextColor(rgbForeground);

	// Get the string we are supposed to draw.
	CString str ;
	GetColumnText(nColumn, str);
		
	// Adjust for the margins on the top and bottom.
	CRect textRect(cellRect);
	AdjustForMargins(textRect, nColumn);
	

	if  ( nColumn == COL_NAME )
	{	
		// clear the background
		pDC->ExtTextOut(0,0, ETO_OPAQUE, cellRectSansBorder, NULL, 0, NULL);

		CRect glyphRect = ComputeGlyphRect(pDC, cellRect);

		// Select gray brush for drawing the actual tree.
		pOldBrush = (CBrush *) pDC->SelectStockObject(GRAY_BRUSH);


		if ( IsExpandable() || IsRetVal())
		{
			BOOL bExpanded = GetTreeGrid()->IsExpanded(this);

			UINT nImage;
			if (IsRetVal() )
			{
				ASSERT(!IsExpandable());
				nImage = IMG_RETVAL;
			}
			else
				nImage = bExpanded ? IMG_MINUS : IMG_PLUS;

			CPoint destPoint = glyphRect.TopLeft();

			s_imgPlusMinus.DrawImage(pDC, destPoint, nImage, SRCCOPY);

			// If we are expanded draw the line from the center of the glyph to the bottom.
			if ( bExpanded )
			{
				CPoint point;
				point.x = glyphRect.left + glyphRect.Width()/2;
				point.y = glyphRect.bottom;
				pDC->PatBlt(point.x, point.y, 1, (cellRect.bottom - glyphRect.bottom), PATCOPY);
			}	
		}
		
		if ( m_pParent != NULL )
		{
			int x = cxBeforeBmap + ((m_nLevels - 1) * cxBitmap) + cxBitmap/2;
			int y = glyphRect.top + glyphRect.Height() / 2;

			// Draw the horizontal line to the item
			int cxWidth;
			
			if (IsExpandable())
				cxWidth = (cxBitmap/2);
			else if (m_nFlags & TGF_LASTLEVELENTRY)
				cxWidth = (cxBitmap + cxBitmap/2); // Arms for the last row are slightly longer for demarcation.
			else
				cxWidth = cxBitmap;

			pDC->PatBlt(x, y, cxWidth, 1, PATCOPY);
			
			// Draw the vertical line from the parent.
			if ( m_nFlags & TGF_LASTLEVELENTRY )
				pDC->PatBlt(x, cellRect.top, 1, (y - cellRect.top), PATCOPY);
			else
				pDC->PatBlt(x, cellRect.top, 1, m_cyHeight, PATCOPY);

			// Draw the verticals in the left connecting other nodes.
			int nLevel = m_nLevels;
			for (CTreeGridRow * pRowT = m_pParent ; pRowT != NULL ; pRowT = pRowT->m_pParent)
			{
				nLevel -= 1;
				x -= cxBitmap;

				if (!(pRowT->m_nFlags & TGF_LASTLEVELENTRY) && (pRowT->m_pParent != NULL))
					pDC->PatBlt(x, cellRect.top, 1, m_cyHeight, PATCOPY);
			}		
		}
		
		pDC->SelectObject(pOldBrush);

		int cxBeginName = glyphRect.left + glyphRect.Width() + cxAfterBmap;

		// Finally draw the text after the glyph has been drawn.
		pDC->ExtTextOut(cxBeginName, textRect.top,  ETO_CLIPPED,
						cellRectSansBorder , str, str.GetLength( ), NULL);
				
	}
	else if (nColumn == COL_VALUE)
	{
		// clear the background
		pDC->ExtTextOut(0,0, ETO_OPAQUE, cellRectSansBorder, NULL, 0, NULL);

		UINT dt;
		if ( GetTM()->IsValueValid( ))
			dt = DT_NOPREFIX | DT_WORDBREAK ;
		else
			dt = DT_NOPREFIX | DT_SINGLELINE ;
		
		CDbgGridWnd *pGrid = (CDbgGridWnd *)GetTreeGrid();
		// Only permanent windows i.e watch and variables which have a
		// view attached to it, show color highlighting.
		BOOL bHiliteChanges = IsValueChanged() &&
				(pGrid->GetView() != NULL) && GetTM()->IsValueValid();

		COLORREF crOldText, crText, crBk;

		if (bHiliteChanges)
		{	
			pGrid->GetChangeHighlightColors(crText, crBk);
			crOldText = pDC->SetTextColor(crText);
		}
				
		// The value column can be m ulti-row if need be.
		pDC->DrawText(str, str.GetLength(), textRect, dt);

		if (bHiliteChanges)
		{
			pDC->SetTextColor(crOldText);
		}
	}
	else			
	{
		ASSERT(FALSE);
	}

}

UINT CWatchRow::HitTest(CPoint pt)
{
	CTreeGridWnd *pGrid = GetTreeGrid();
	CRect rcCell;
	GetRect(rcCell); // top, bottom and left are correct here

	int cxName = pGrid->GetColumnWidth(COL_NAME);
	int cxValue = pGrid->GetColumnWidth(COL_VALUE);
	rcCell.right = rcCell.left + cxName + cxValue;

	if (!rcCell.PtInRect(pt))
		return WRH_NONE;
	
	if (rcCell.left + cxName <= pt.x)
		return WRH_VALUE;

	if (rcCell.left + GetPlusColWidth() <= pt.x)
		return WRH_NAME;
	
	if (IsExpandable())
	{
		CRect rcGlyph( GetGlyphRect(rcCell) );
		if ( rcGlyph.PtInRect(pt) )
			return WRH_GLYPH;
	}

	return WRH_SELMARGIN;
}

BOOL CWatchRow::OnLButtonDown(UINT nFlags, CPoint pt)
{
	if ( !(nFlags & MK_CONTROL || nFlags & MK_SHIFT) )
	{
		if ( WRH_GLYPH == HitTest(pt) )
		{
			GetTreeGrid()->ToggleExpansion(this);
			// We still want the default action of selection to happen.
		}
	}
		
	return CBaseClass::OnLButtonDown(nFlags, pt) ;
}				

BOOL CWatchRow::OnLButtonUp( UINT nFlags, CPoint pt)
{
	// We don't want to activate the name field if the
	// hit-test put us in the tree part of the column.
	if ( m_bWasSelected &&  pt.x <= GetPlusColWidth())
		return FALSE;
	else
		return CBaseClass::OnLButtonUp(nFlags, pt);
}

BOOL CWatchRow::OnLButtonDblClk( UINT nFlags, CPoint pt)
{
	if (pt.x <= GetPlusColWidth())
		return FALSE;
	else
		return CBaseClass::OnLButtonDblClk(nFlags, pt);
}

BOOL CWatchRow::OnMouseMove(UINT nFlags, CPoint pt)
{
	if (WRH_GLYPH == HitTest(pt))
	{
		SetCursor(LoadCursor(NULL,IDC_ARROW));
		return TRUE;
	}
	return CBaseClass::OnMouseMove(nFlags, pt);
}

// Since the watch window is dockable  we need to create
// special child controls so things like right-mouse popup
// still work when the control is active.
																											
CEdit*  CWatchRow::NewEdit(  )
{
	return ( new CDockEdit );
}

CComboBox* CWatchRow::NewCombo( )
{
	return ( new CDockCombo ) ;
}


void CWatchRow::AdjustControlRect ( GRID_CONTROL_TYPE controlType, int nColumn,
										 CRect& rect )
{
	if ( nColumn == COL_NAME )
	{
		// We don't want the edit control to go over the tree control parts.
		rect.left += GetPlusColWidth();
	}

	CBaseClass::AdjustControlRect(controlType, nColumn, rect);
}


BOOL CWatchRow::IsExpandable( ) const
{
	return GetTM()->IsExpandable( );
}

DWORD CWatchRow::GetChildCount()
{
	return m_pTM->GetChildCount();
}

BOOL CWatchRow::GetChildren(CTreeGridRowArray& rgTGRow )
{
	CTMArray rgTM;

	if  (!m_pTM->GetTMChildren(rgTM))
		return FALSE;

	m_bHasSynthChild = m_pTM->HasSynthChild();
	
	// Currently we should never call this function with entries
	// already filled in.
	ASSERT(rgTGRow.GetSize() == 0);

	for ( int i = 0; i < rgTM.GetSize(); i++ )
	{
		CWatchRow * pRow = NewWatchRow(GetTreeGrid( ), rgTM[i], this);
		rgTGRow.Add(pRow);
	}

	rgTM.RemoveAll();
	return TRUE;
}

				
void CWatchRow::Update( )
{
	// Make sure that the ValueChanged bit is not set before Update
	this->SetValueChanged(FALSE);

	// First check if our parent is Dirty, if so don't even bother
	// refreshing anything here.
	if ( m_pParent && (m_pParent->m_nFlags & TGF_DIRTY_CONTENT))
	{
		// Okay then we are dirty as well.
		GetTreeGrid()->DirtyRow(this, TGF_DIRTY_CONTENT|TGF_DIRTY_ITEM);
	}
	else
	{
		// This logic is significantly complicated by the presence of synthesized
		// child TM's. There  are 3 cases possible.
		// 1) We have a new synthesized child which didn't exist before.
		// 2) Our parent still has a synthesized child but its type might have
		//	  changed. The current TM is not valid in this case.
		// 3) We are a synthesized child but the parent no longer has one.
		
		if ( m_pTM->IsSynthChildNode( ))
		{
			ASSERT(m_pParent != NULL);

			// If our parent is not supposed to have a synthesized
			// child anymore mark ourself for deletion.	We require that
			// the parent's Update be called before the childs.
			CWatchRow * pParent = (CWatchRow *)m_pParent;

			if ( !pParent->GetTM()->HasSynthChild() )
			{
				GetTreeGrid()->DirtyRow(this, TGF_DIRTY_DELETE);
				pParent->m_bHasSynthChild = FALSE;
			}
			else {
				CTM *pTM = pParent->GetTM()->GetSynthChildTM();

				// If the type of the synthesized child didn't
				// change we can just leave it as is.
				if ( GetTreeGrid()->IsExpanded(this) && !pTM->IsTypeIdentical(m_pTM) )
				{
					GetTreeGrid()->DirtyRow(this, TGF_DIRTY_CONTENT|TGF_DIRTY_ITEM);
				}
				else {
					GetTreeGrid()->DirtyRow(this, TGF_DIRTY_ITEM);
				}
				delete ( m_pTM );
				m_pTM = pTM;	// replace the TM with the new one.
			}				
		}	
		else if ( m_pTM->Refresh( ) == CTM::TYPE_CHANGED )
		{
			ASSERT(m_pParent == NULL);
			int tgf = TGF_DIRTY_ITEM;
			if ( GetTreeGrid()->IsExpanded(this) )
				tgf |= TGF_DIRTY_CONTENT;

			GetTreeGrid()->DirtyRow(this, tgf);
		}
		else
		{
			if ( GetTreeGrid( )->IsExpanded(this) && !m_bHasSynthChild
				  && m_pTM->HasSynthChild( ))
			{
				// We have a new synthesized child, even though our
				// own type didn't change. The synthesized child is
				// always at position 0.
				CTM *pTM = m_pTM->GetSynthChildTM();
					
				if ( pTM )
				{
					CWatchRow *pRow = NewWatchRow(GetTreeGrid(), pTM, this);

					if ( pRow != NULL )
					{
						GetTreeGrid()->InsertRowAfter(pRow, this);
						m_bHasSynthChild = TRUE;
					}
					else
						delete pTM;					
				}	
			}
			GetTreeGrid()->DirtyRow(this, TGF_DIRTY_ITEM);
		}
	}
}


void CWatchRow::Restart( )
{
	// For a root level node, call
	if (m_pParent == NULL)
	{
		m_pTM->Restart();
	}
	
	GetTreeGrid()->DirtyRow(this, TGF_DIRTY_ITEM|TGF_DIRTY_CONTENT);
}


void CWatchRow::OnSelect(BOOL bSelect)
{
	CBaseClass::OnSelect(bSelect);
	
	// If we are connected to a view inform the view that the selection has changed.
	CDbgGridWnd *pGrid = (CDbgGridWnd *)GetTreeGrid();
	if ( pGrid->GetView() != NULL )
		pGrid->GetView()->OnGridRowSelectionChange();
}
	
				
/////////////////////////////////////////////////////////////////////////
// CNewWatchRow

CNewWatchRow::CNewWatchRow(CDbgGridWnd *pGrid)
	: CBaseClass(pGrid, COL_NAME)
{
}

CNewWatchRow::~CNewWatchRow()
{
}

void CNewWatchRow::ResetSize(CDC *pDC)
{
	TEXTMETRIC tm;
	
	pDC->GetTextMetrics(&tm);
	m_cyHeight = tm.tmHeight + tm.tmExternalLeading + 2 * CWatchRow::cyMargin;

	// This is so the row height is the same as the CWatchRow's in the 
	// grid. See comments in CWatchRow::ResetSize.
	CWindowDC dc(NULL);
	dc.GetTextMetrics(&tm);
	int cyMinHeight = tm.tmAscent + tm.tmInternalLeading + 1;

	if ( m_cyHeight < cyMinHeight )
		m_cyHeight = cyMinHeight;

	if ( m_cyHeight < CWatchRow::cyMinPlusColHeight )
		m_cyHeight = CWatchRow::cyMinPlusColHeight;
}

CSize CNewWatchRow::GetCellSize(CDC *pDC, int nColumn) const
{
	if ( nColumn != COL_NAME )
		return CBaseClass::GetCellSize(pDC, nColumn);
	else
	{
		CSize size;
        size.cx = CWatchRow::cxMinPlusColWidth;
		size.cy = m_cyHeight;
		return size;
	}
}
								
void CNewWatchRow::DrawCell(CDC *pDC, const CRect& cellRect, int nColumn)
{

	// FUTURE: Figure out a better way to erase the background.	
	pDC->ExtTextOut(cellRect.left, cellRect.top, ETO_CLIPPED| ETO_OPAQUE,
			 cellRect,"", 0, NULL);

	COLORREF rgbForeground = pDC->SetTextColor(GetSysColor(COLOR_BTNSHADOW));
	CBrush* pOldBrush = pDC->SelectObject(CDC::GetHalftoneBrush());

	// Cell Rect excluding border regions if any.
	CRect cellRectSansBorder(cellRect);

	// Draw the border, each cell takes responsibility for painting
	// their bottom and right edges.

	pDC->PatBlt(cellRect.left, cellRect.bottom -1,  cellRect.Width(), 1, PATCOPY);

	// Exclude the border region.
	cellRectSansBorder.bottom -= 1;

	// The last column doesn't need the vertical border
	//  as the frame window has a border.	
	if ( (nColumn + 1) !=  GetGrid()->GetColumnCount() )
	{
		pDC->PatBlt(cellRect.right -1, cellRect.top, 1, cellRect.Height(), PATCOPY);
		cellRectSansBorder.right -= 1 ;
	}
	
	pDC->SelectObject(pOldBrush);
	pDC->SetTextColor(rgbForeground);

	switch ( nColumn )
	{
		case COL_NAME:
		{
			CRect textRect(cellRect);

			AdjustForMargins(textRect, nColumn);
			pDC->DrawFocusRect(textRect);
			break;
		}
		default:
			break;
	}
}							

int CNewWatchRow::Compare(const CGridRow *) const
{
	ASSERT(FALSE);

	return 1;		// We are bigger than everyone else.
}

void CNewWatchRow::OnActivate(BOOL bActivate, int nColumn)
{
	if ( bActivate && DebuggeeRunning())
	{
		return;
	}
	
	CBaseClass::OnActivate(bActivate, nColumn);
}						

void CNewWatchRow::OnControlNotify( UINT nControl, UINT nNotification)
{
	CGridControlWnd *pGrid = (CGridControlWnd *)GetGrid( );

	switch ( pGrid->m_controlType)
	{
		case edit:
			if (nNotification == EN_KILLFOCUS)
			{
				CWnd *pNewWnd = CWnd::GetFocus( );

				if (  pNewWnd != NULL && (pNewWnd != pGrid) &&
					!pGrid->IsChild(pNewWnd) && !pNewWnd->IsChild(pGrid))
				{
					pGrid->PostMessage(WM_DISMISS_CONTROL);
				}
			}
			break;
	}
	CBaseClass::OnControlNotify(nControl, nNotification);
}

CEdit * CNewWatchRow::NewEdit( )
{
	return (new CDockEdit);
}

CComboBox * CNewWatchRow::NewCombo( )
{
	return (new CDockCombo);
}

void CNewWatchRow::AdjustForMargins(CRect &rect, int nColumn)
{
	rect.InflateRect(-CWatchRow::cxMargin, -CWatchRow::cyMargin);
}

BOOL CNewWatchRow::OnMouseMove(UINT, CPoint)
{
	// The new watch row always puts up the edit box when the user clicks on it.
	// So we never show just the selection only cursor.
	SetCursor(LoadCursor(NULL, IDC_ARROW));
	return TRUE;
}

void CNewWatchRow::OnSelect(BOOL bSelect)
{
	CBaseClass::OnSelect(bSelect);
	
	// If we are connected to a view inform the view that the selection has changed.
	CDbgGridWnd *pGrid = (CDbgGridWnd *)GetTreeGrid();
	if ( pGrid->GetView() != NULL )
		pGrid->GetView()->OnGridRowSelectionChange();
}
