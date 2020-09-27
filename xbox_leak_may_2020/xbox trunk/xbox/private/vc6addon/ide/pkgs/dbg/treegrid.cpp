/// varswnd.cpp : implementation file
//

#include "stdafx.h"
#pragma hdrstop

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTreeGridRow

CTreeGridRow::CTreeGridRow(CTreeGridWnd *pGridWnd,
						 CTreeGridRow *pParent /*= NULL */)
	: CGridControlRow(pGridWnd)
{
	m_pParent = pParent;

	if ( m_pParent )
		m_nLevels = m_pParent->m_nLevels + 1;
	else
		m_nLevels = 0;

	m_nFlags = TGF_NONE;
}	
	
CTreeGridRow::~CTreeGridRow( )
{
	// We don't allocate any data dynamically.
}

BOOL CTreeGridRow::IsNewRow() const
{
	return FALSE;
}

void CTreeGridRow::Update( )
{
	// Sub-classes should provide interesting behavior if need be.
}

void CTreeGridRow::Restart( )
{
	// Sub-classes should provide interesting behavior if need be.
}

/////////////////////////////////////////////////////////////////////////////
// CTGNewRow

CTGNewRow::CTGNewRow(CTreeGridWnd *pGrid, int editColumn)
	: CBaseClass(pGrid)
{
	m_editColumn = editColumn;
}	

CTGNewRow::~CTGNewRow()
{
}

BOOL CTGNewRow::IsExpandable() const
{
	return FALSE;
}

BOOL CTGNewRow::GetChildren(CTreeGridRowArray&)
{
	ASSERT(FALSE);
	return FALSE;
}

DWORD CTGNewRow::GetChildCount() 
{
	return 0;
}

BOOL CTGNewRow::IsNewRow() const
{
	return TRUE;
}

void CTGNewRow::GetColumnText(int, CString& rStr)
{
	rStr.Empty();
}
	
void CTGNewRow::OnActivate(BOOL bActivate, int nColumn)
{
	if ( bActivate )
	{
		CreateControl(edit, m_editColumn, ES_AUTOHSCROLL);
	}
	else
	{
		CreateControl(none, -1);
	}
}

BOOL CTGNewRow::OnAccept(CWnd *pControlWnd)
{	
	static bInAccept = FALSE;
	if (pControlWnd != NULL)
	{
		// HACK, Adding a new row can cause us to introduce
		// a scrollbar and get an OnSize message. The default
		// OnSize handling calls AcceptControl again. So we end up adding
		// two rows. We just prevent the re-entrancy here.
		if (!bInAccept)
		{
			bInAccept = TRUE;	
			CString str;
		
			pControlWnd->GetWindowText(str);

			if ( !str.IsEmpty())
			{
				// Inform the treegrid about the new value.
				((CTreeGridWnd *)GetGrid())->OnEnterNew(str);
			}
			bInAccept = FALSE;
		}
	}

	return TRUE;
}																		

void CTGNewRow::DrawCell(CDC *pDC, const CRect& cellRect, int nColumn)
{
	if (nColumn == m_editColumn)
	{
		CRect textRect = cellRect;
		AdjustForMargins(textRect, nColumn);

		pDC->ExtTextOut(textRect.left, textRect.top, ETO_CLIPPED | ETO_OPAQUE,
			cellRect, "", 0, NULL);

		pDC->DrawFocusRect(textRect);
	}
	else
	{
		CBaseClass::DrawCell(pDC, cellRect, nColumn);
	}
}
							
void CTGNewRow::ResetSize(CDC *pDC)
{
	// FUTURE: what is the right thing to do here.
	CWindowDC dc(NULL);	
	TEXTMETRIC tm;
	
	dc.GetTextMetrics(&tm);
	m_cyHeight = tm.tmHeight + tm.tmExternalLeading;
}

CSize CTGNewRow::GetCellSize(CDC *, int) const
{
	CSize size;
	
	size.cy = m_cyHeight;
	// This row is flexible so we will not make any size demands.
	size.cx = 0;		

	return size;
}
					 	
int CTGNewRow::GetNextControlColumn(int nColumn)
{
	if ( nColumn < m_editColumn )
		return m_editColumn;
	else
		return -1;
}

int CTGNewRow::GetPrevControlColumn(int nColumn)
{
	if ( nColumn > m_editColumn )
		return m_editColumn;
	else
		return -1;
}



/////////////////////////////////////////////////////////////////////////////
// CTreeGridWnd

CTreeGridWnd::CTreeGridWnd(int nColumns, BOOL bDisplayCaption)
	: CGridControlWnd(nColumns, bDisplayCaption)
{
	// Tree-like grids should not be sorted by other criterion.
	EnableSort(FALSE);
	EnableAutoSort(FALSE);
	m_bNewRow = FALSE;
	m_bForceAccept = FALSE;
}

CTreeGridWnd::~CTreeGridWnd()
{
	// The CGridWnd destructor deletes the rows so we don't
	// need to do the destruction.
}


IMPLEMENT_DYNAMIC(CTreeGridWnd, CGridControlWnd)

BEGIN_MESSAGE_MAP(CTreeGridWnd, CTreeGridWnd::CBaseClass)
	//{{AFX_MSG_MAP(CTreeGridWnd)
	ON_WM_PAINT()
	ON_WM_DESTROY()
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_DISMISS_CONTROL, OnDismissControl)
	ON_MESSAGE(WM_WARN_AND_EXPAND, OnWarnAndExpand)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTreeGridWnd Attributes

BOOL CTreeGridWnd::IsExpanded(CTreeGridRow *pRow)
{
	if ( pRow != NULL )
		return ((pRow->m_nFlags & TGF_EXPANDED) != 0);
	else
		return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CTreeGridWnd Operations

CTreeGridRow * CTreeGridWnd::GetCurSel( ) const
{
	POSITION pos = GetHeadSelPosition();

	// We should have exactly one item selected.
	if ( pos != NULL)
	{
		CGridElement *pElement = GetNextSel(pos);
	
		ASSERT ( pElement != NULL );
		CGridRow *pRow = pElement->GetRow();
				
		// We should have exactly one row selected
		// the row shouldn'be the caption row.
		if (pos == NULL && pRow != GetCaptionRow())
			return (CTreeGridRow *)pRow;
	}
	return NULL;
}

CTreeGridRow * CTreeGridWnd::GetRow(int nIndex /* = -1*/ )
{
	if ( nIndex == -1 )
		return GetCurSel();
	else
		return (CTreeGridRow *)CBaseClass::GetRow(nIndex);
}

CTreeGridRow * CTreeGridWnd::GetChildRow(CTreeGridRow * pParentRow, int nIndex)
{
	ASSERT(nIndex >= 0);
	ASSERT(pParentRow != NULL);

	
	CTreeGridRow * pChildRow = NULL;
	POSITION pos = m_rows.Find(pParentRow);
	// Skip over the parent row.
	(void)GetNextRow(pos);

	int count = 0;
	while ( pos != NULL )
	{
		CTreeGridRow *pRow = (CTreeGridRow *)GetNextRow(pos);

		if (pRow->m_nLevels <= pParentRow->m_nLevels)
			break;

		if (pRow->m_nLevels == (pParentRow->m_nLevels + 1))
		{
			if (count == nIndex)
			{
				pChildRow = pRow;
				break;
			}
			else
			{
				count++;
			}
		}
	}							
	
	return pChildRow;
}
			
void CTreeGridWnd::Expand(CTreeGridRow *pParent, BOOL bRedraw /* = TRUE */)
{
	ExpandHelper(pParent, bRedraw, TRUE);
}
						  
void CTreeGridWnd::ExpandHelper(CTreeGridRow *pParent, BOOL bRedraw, BOOL bWarn)
{
	ASSERT(pParent != NULL);
	ASSERT(!pParent->IsNewRow());

	// Update the rows expandable/unexpandable bit. Then see if we
	// can really expand this node.

	if (!((pParent->m_nFlags & TGF_EXPANDABLE) ||
		   (pParent->m_nFlags & TGF_NOT_EXPANDABLE)))
	{
		pParent->m_nFlags |= pParent->IsExpandable()
								? TGF_EXPANDABLE : TGF_NOT_EXPANDABLE;

		if (pParent->m_nFlags & TGF_NOT_EXPANDABLE)						
		{
			pParent->m_nFlags &= ~TGF_EXPANDED;
			return;						
		}
	}

	if ( pParent->m_nFlags & TGF_EXPANDED )
		return;		// we are already expanded.

	if (bWarn && pParent->GetChildCount() > GetExpansionWarnCount())
	{
		// Putting up a message box here confuses the drag-drop code 
		// Hence we post a message to warn about the expansion after
		// the current message handler has unwound.
		PostMessage(WM_WARN_AND_EXPAND, (WPARAM)pParent);
		return;
	}
	
	CWaitCursor waitCursor;

   	InsertLevel(pParent, bRedraw);
}

BOOL CTreeGridWnd::InsertLevel(CTreeGridRow *pParent, BOOL bRedraw /* = TRUE*/)
{
	ASSERT(!pParent->IsNewRow());
	ASSERT(pParent->IsExpandable());
	CTreeGridRowArray rgRowArray;

	if ( !pParent->GetChildren(rgRowArray) )
		return FALSE;

	pParent->m_nFlags |= TGF_EXPANDED | TGF_EXPANDABLE;
	pParent->m_nFlags &= ~TGF_NOT_EXPANDABLE;
	if ( rgRowArray.GetSize( ) != 0 )
	{
		CTreeGridRow * pLastRow = rgRowArray[rgRowArray.GetSize() - 1];
		pLastRow->m_nFlags |= TGF_LASTLEVELENTRY;
	}

	InsertMultipleRows(&rgRowArray, pParent, bRedraw);

	// Free up the memory for the array.
	rgRowArray.RemoveAll();

	return TRUE;
}

BOOL CTreeGridWnd::InsertTopLevelRow(CTreeGridRow *pRow, BOOL bRedraw /* = TRUE*/)
{
	ASSERT(pRow->m_pParent == NULL);
	ASSERT(pRow->m_nLevels == 0);

	if ( m_bNewRow )
	{
		POSITION pos = m_rows.GetTailPosition();
		ASSERT(((CTreeGridRow *)GetRowAt(pos))->IsNewRow());
		m_rows.InsertBefore(pos, pRow);
	}
	else
	{
		AddRow(pRow, FALSE);
	}

	if (bRedraw)
		InvalidateGrid();

	return TRUE;
}

BOOL CTreeGridWnd::DeleteTopLevelRow(CTreeGridRow *pRow, BOOL bRedraw /* = TRUE*/)
{
	ASSERT(pRow->m_pParent == NULL);
	ASSERT(pRow->m_nLevels == 0);
	ASSERT(!pRow->IsNewRow());	// Use EnableNewRow for this.

	Collapse(pRow, FALSE);
	RemoveRow(pRow, FALSE);
	delete (pRow);
	
    if (bRedraw)
    {
        AdjustTopVisible();
        InvalidateGrid();
    }

	return TRUE;
}				
	
void CTreeGridWnd::Collapse(CTreeGridRow *pParentRow, BOOL bRedraw /* = TRUE*/)
{
	ASSERT(!pParentRow->IsNewRow());

	int cDeleted = 0;

	if ( !(pParentRow->m_nFlags & TGF_EXPANDED))
		return;		// nothing to do.

	POSITION pos = m_rows.Find(pParentRow);
	// Skip over the parent row.
	(void)GetNextRow(pos);

	// This is kind of tricky, we delete exactly one row
	// at a time, pos is already at the next entry so
	// it is okay to modify the list and still assume
	// that pos is valid.
	while (pos != NULL)
	{
		CTreeGridRow *pRow = (CTreeGridRow *)GetNextRow(pos);
		ASSERT(pRow != NULL);
					
		if ( pRow->m_nLevels <= pParentRow->m_nLevels )
			break;
		
		RemoveRow(pRow, FALSE);
		delete pRow;
		cDeleted++;
	}

	if (bRedraw)
	{
		AdjustTopVisible();
		InvalidateGrid();
	}

	pParentRow->m_nFlags &= ~TGF_EXPANDED;
}	

BOOL CTreeGridWnd::ToggleExpansion(CTreeGridRow *pRow)
{
	BOOL bRet = TRUE;

	if (IsExpanded(pRow))
	{
		Collapse(pRow);
	}
	else {
		if (pRow->IsExpandable( ))
			Expand(pRow);
		else
			bRet = FALSE;
	}
	
	return bRet;
}

void CTreeGridWnd::ActivatePrevControl()
{
	POSITION pos;

	if (GetRowCount() == 0)
		return;

	if ( m_pActiveElement != NULL )
	{
		pos = m_rows.Find(m_pActiveElement->GetRow());
	}
	else if ( !m_selection.IsEmpty() )
	{
		pos = m_rows.Find(m_selection.GetHead());
	}
	else if ( m_posTopVisibleRow != NULL )
	{
		pos = m_posTopVisibleRow;
	}
	else
	{
		// FUTURE: This really should look for the last visible row!
		MessageBeep(0);
		return;
	}

	int nPrevControlColumn = m_nControlColumn;	
	while ( pos != m_rows.GetHeadPosition() )
	{
		nPrevControlColumn = ((CGridControlRow *)GetRowAt(pos))->GetPrevControlColumn(nPrevControlColumn);

		if ( nPrevControlColumn != -1 )
		{
			break;
		}
		else
		{
			m_rows.GetPrev(pos);
		}
	}
			 		
	
	if (pos != NULL && pos != m_rows.GetHeadPosition())
	{
		CGridElement* pElement = (CGridElement*)m_rows.GetAt(pos);
		if (pElement != m_pActiveElement)
			Select(pElement);

		SetActiveElement(pElement, nPrevControlColumn);
		m_posCurRow = pos;
	}
}

void CTreeGridWnd::ActivateNextControl()
{
	POSITION pos;
	
	if (GetRowCount() == 0)
		return;

	if ( m_pActiveElement != NULL )
	{
		pos = m_rows.Find(m_pActiveElement->GetRow());
	}
	else if ( !m_selection.IsEmpty() )
	{
		pos = m_rows.Find(m_selection.GetHead());
	}
	else if ( m_posTopVisibleRow != NULL )
	{
		pos = m_posTopVisibleRow;
	}
	else
	{
		// FUTURE: This really should look for the last visible row!
		MessageBeep(0);
		return;
	}
	
	int nNextControlColumn = m_nControlColumn;
	while ( pos != NULL )
	{
		nNextControlColumn = ((CGridControlRow *)GetRowAt(pos))->GetNextControlColumn(nNextControlColumn);

		if ( nNextControlColumn != -1 )
		{
			break;
		}
		else
		{
			m_rows.GetNext(pos);
		}
	}
	
	if (pos != NULL)
	{
		CGridElement* pElement = (CGridElement*)m_rows.GetAt(pos);
		if (pElement != m_pActiveElement)
			Select(pElement);
		SetActiveElement(pElement, nNextControlColumn);
		m_posCurRow = pos;
	}
}

BOOL CTreeGridWnd::ProcessKeyboard( MSG *pMsg, BOOL bPreTrans /* = FALSE */)
{
	switch(pMsg->message)
	{
	default:
		break;
	case WM_KEYDOWN:
		{
			BOOL bControl = GetKeyState(VK_CONTROL) < 0;
			BOOL bMenu = GetKeyState(VK_MENU) < 0;
			// Is an edit control currently active.
			BOOL bEditCtrl = m_pControlWnd != NULL;
			CTreeGridRow * pRow = GetCurSel();

			switch(pMsg->wParam)
			{

			case VK_TAB:
				// If we are currently editing in the new row, just treat Tab to mean
				// add a new value. This is important as the interaction of default
				// tab behavior with the fact that a new row gets inserted produces
				// strange behavior.
				if ( pRow != NULL && m_pControlWnd != NULL && pRow->IsNewRow( ))
				{
					// control-tab is mdi window switching
					if (bControl)
						break;

					AcceptControl();
					return TRUE;
				}
				break;
			case VK_ADD:
			case VK_RIGHT:
				if ( !bEditCtrl && pRow != NULL && pRow->IsExpandable() )
				{
					if (!bControl)
						Expand(pRow);

					return TRUE;
				}
				break;

			case VK_SUBTRACT:
				if (!bEditCtrl && pRow != NULL && pRow->IsExpandable())
				{
					if (!bControl && IsExpanded(pRow) )
					{
						Collapse(pRow);
					}
					return TRUE;
				}
				break;

			case VK_LEFT:
				if (!bEditCtrl && pRow != NULL)
				{
					if ( !bControl && IsExpanded(pRow) )
					{
						Collapse(pRow);
						return TRUE;
					}	
				}
				// Fall through.
											
			case VK_BACK:
				// In a tree-control backspace is
				// "go to parent".
				if ( !bEditCtrl && pRow != NULL)
				{
					if ( !bControl && pRow->m_pParent != NULL )
					{	// Go to the parent row.
						Select(pRow->m_pParent);
					}
					return TRUE;
				}
				break;

			case VK_RETURN:
				if ( m_pControlWnd == NULL && !bMenu)
				{
					if ( pRow != NULL && ToggleExpansion(pRow))
					{	
						return TRUE;
					}
				}
				break;

			default:
				break;
			} /* end switch */

		}
		
	case WM_CHAR:
		{
			CTreeGridRow * pRow = GetCurSel();

			if (pRow != NULL && m_pControlWnd == NULL)
			{
				switch(pMsg->wParam)
				{
					case '+':
						if (pRow->IsExpandable())
						{
							Expand(pRow);
							return TRUE;
						}
						break;
					case '-':
						if (pRow->IsExpandable())
						{
							if (IsExpanded(pRow))
								Collapse(pRow);

							return TRUE;
						}
						break;

					default:
						break;
				}
			}
		}
					
	}

	return CBaseClass::ProcessKeyboard(pMsg, bPreTrans);
}

BOOL CTreeGridWnd::Refresh(CTreeGridRow *pRow, int& nIndex)
{
	BOOL bRetVal = FALSE;

	if (pRow->m_nFlags & TGF_DIRTY_DELETE)	
	{
		// FUTURE: This will cause us to miss a row in RefreshAllRoots
		// if a top level row is marked TGF_DIRTY_DELETE.

		// This row and any of its children need to be deleted.
		if (pRow->m_nFlags & TGF_EXPANDED )
			Collapse(pRow, FALSE);
		
		RemoveRow(pRow, FALSE);
		delete pRow;
		nIndex--;	// the current index should be refreshed again.
		bRetVal = TRUE;
	}					
	else if (pRow->m_nFlags & TGF_DIRTY_CONTENT)
	{
		BOOL bWasExpanded = FALSE;

		// reset the dirty node.
		pRow->m_nFlags &= ~TGF_DIRTY_CONTENT;

		// If it is expanded, collapse it.
		if (pRow->m_nFlags & TGF_EXPANDED)
		{
			bWasExpanded = TRUE;
			Collapse(pRow, FALSE);
		}

		pRow->m_nFlags &= ~(TGF_EXPANDABLE|TGF_NOT_EXPANDABLE);
		pRow->m_nFlags |= pRow->IsExpandable() ? TGF_EXPANDABLE : TGF_NOT_EXPANDABLE;

		if ( bWasExpanded )
		{
			if ( pRow->m_nFlags & TGF_EXPANDABLE)
				Expand(pRow, FALSE);

			bRetVal = TRUE;
		}				
	}
	// If we have children and they are expanded,
	// perform this function on them first.	
	else if  ( pRow->m_nFlags & TGF_EXPANDED )						
	{
		int nIndexT =  nIndex + 1;
		POSITION pos;

		while  ((pos = m_rows.FindIndex(nIndexT)) != NULL )
		{
			CTreeGridRow * pRowChild;

			if ((pRowChild = (CTreeGridRow *)GetRowAt(pos)) == NULL)
			{
				ASSERT(FALSE);
				break;
			}

			if (pRowChild->m_nLevels <= pRow->m_nLevels )
				break;

			// Just refresh our children.
			if (pRowChild->m_nLevels == (pRow->m_nLevels + 1))
			{
				if ( Refresh(pRowChild, nIndexT) )
					bRetVal = TRUE;
			}

			nIndexT++;
		}
	}

	if (pRow->m_nFlags & TGF_DIRTY_ITEM)
	{
		bRetVal = TRUE;
		pRow->m_nFlags &= ~TGF_DIRTY_ITEM;
	}

	return bRetVal;
}

void CTreeGridWnd::RefreshAllRoots( )
{
	POSITION pos;
	int nIndex = 1;
	BOOL bInvalidate = FALSE;

	// Note: we use the index for iterating instead of
	// using GetNextRow on the pos, because the pos could
	// become invalid if refresh causes some items to be
	// add/deleted from the list.

	while ((pos = m_rows.FindIndex(nIndex)) != NULL)
	{
		CTreeGridRow * pRow = (CTreeGridRow *)GetRowAt(pos);
		if ( pRow->m_nLevels == 0 && !pRow->IsNewRow())
		{
			if (Refresh(pRow, nIndex))
				bInvalidate = TRUE;
		}					
		nIndex++;
	}

	// If any of the refreshing requires us to invalidate do it now.
	if (bInvalidate)
		InvalidateGrid( );
}


CTGNewRow * CTreeGridWnd::CreateNewRow( )
{
	// You need to provide an override in a sub-class
	// if you want this capability.
	ASSERT(FALSE);
	return NULL;
}	

void CTreeGridWnd::OnEnterNew(CString str)
{
	// Sub-class should override this to deal with the user
	// entering a new row.
	ASSERT(FALSE);
	return;
}

void CTreeGridWnd::EnableNewRow(BOOL bEnable /* = TRUE */)
{
	if ( m_bNewRow == bEnable )
		return;
		
	m_bNewRow = bEnable;
	
	if ( m_bNewRow )
	{
		CTGNewRow * pNewRow = CreateNewRow();
		if ( pNewRow )
			AddRow(pNewRow);
		else
		{
			m_bNewRow = FALSE;
			return;
		}		
	}
	else {									
		int cRows = GetRowCount();
		CTreeGridRow *pRow = GetRow(cRows - 1);

		ASSERT(pRow != NULL);
		if ( pRow->IsNewRow( ))
		{
			RemoveRow(pRow);
			delete pRow;
		}
		else
		{
			ASSERT(FALSE);
		}
	}
	
	return;
}			
		
			
						

void CTreeGridWnd::DirtyRow(CTreeGridRow *pRow, int tf)
{
	ASSERT(pRow != NULL);
	ASSERT(!pRow->IsNewRow());

	pRow->m_nFlags |= tf;
	Invalidate( );
}

BOOL CTreeGridWnd::DeleteSelectedRows( )
{
	POSITION pos = GetHeadRowPosition( );
	CTreeGridRow *pRowFirstSelected = NULL;
	CTreeGridRow *pRowSelectLater = NULL;

	while ( pos != NULL )
	{
		CTreeGridRow *pRow = (CTreeGridRow *)GetNextRow(pos);
		// Either we are a top-level row or atleast
		// somebody up on the chain should also be marked for
		// deletion.
		if ( IsSelected(pRow))
		{
			// Remember the first selected row. This will be used
			// to set the selection after the deletion is done.
			if (pRowFirstSelected == NULL)
				pRowFirstSelected = pRow;

			if ( pRow->m_nLevels != 0 )
			{
				CTreeGridRow * pRowParent = pRow->m_pParent;
				while ( pRowParent )
				{
					if (IsSelected(pRowParent))
					{
						break;
					}
					else {
						pRowParent = pRowParent->m_pParent;
					}
				}

				if (pRowParent == NULL)
				{	
					// None of this rows parents are selected.
					// it is illegal to delete this row.
					return FALSE;
				}
			}
		}
		else
		{
			if ( pRowFirstSelected != NULL && pRowSelectLater == NULL)
			{
				// Figure out which row gets the selection after the deletion is done.
				// The rules are
				// a) Pick the first non-selected row after the first selected row.
				//    In case of the treegrid this row has to be at level 0 as all children
				//    of the selected row will get deleted automatically.
				// b) If there is no such row pick the row before the first selected row.
				if ( pRow->m_nLevels == 0 )
					pRowSelectLater = pRow;
			}
		}
	}

	
	if (pRowSelectLater == NULL)
	{
		POSITION pos = m_rows.Find(pRowFirstSelected);
		(void)GetPrevRow(pos);		// Skip over the First selected row.
		if (pos != NULL)
			pRowSelectLater = GetRowAt(pos);		
	}

	// We can now proceed with the actual deletion.
	int nIndex = 1;

	while ( (pos = m_rows.FindIndex(nIndex++)) != NULL)
	{
		CTreeGridRow *pRow = (CTreeGridRow *)GetRowAt(pos);

		if (IsSelected(pRow) && pRow->m_nLevels == 0 && !pRow->IsNewRow())
		{
			if ( DeleteTopLevelRow(pRow, FALSE) )
				nIndex--;
		}
	}

	// it is possible that the new row is still selected.
	if ((GetHeadSelPosition() == NULL) && pRowSelectLater != NULL)
		Select(pRowSelectLater);

    AdjustTopVisible();
	InvalidateGrid();

	return TRUE;
}		

	
void CTreeGridWnd::FreeAllData( )
{
}

void CTreeGridWnd::InsertRowAfter(CTreeGridRow *pRow, CTreeGridRow *pRowAfter,
						BOOL bRedraw /* = TRUE*/ )
{
	ASSERT_VALID(this);
	ASSERT(!pRowAfter->IsNewRow());

	POSITION pos;

	if ( pRowAfter == NULL)
		pos = m_rows.GetHeadPosition();
	else
	{
		pos = m_rows.Find(pRowAfter);
	}
	
	ASSERT( pos != NULL);
	m_rows.InsertAfter(pos, pRow);

	if (bRedraw )
		InvalidateGrid();
}			

			
void CTreeGridWnd::InsertMultipleRows(CTreeGridRowArray *rgpRow,
				CTreeGridRow *pRowAfter, BOOL bRedraw /* = TRUE */)
{
	ASSERT_VALID(this);
	ASSERT(!pRowAfter->IsNewRow());

	POSITION pos;
	
	if (pRowAfter == NULL)
	{
		pos = m_rows.GetHeadPosition();
	}
	else
	{
		pos = m_rows.Find(pRowAfter);
	}

	ASSERT(pos != NULL);

	for ( int i = 0 ; i < rgpRow->GetSize( ); i++ )
	{
		pos = m_rows.InsertAfter(pos, (*rgpRow)[i]);
	}

	if (bRedraw )
		InvalidateGrid();
}
				
void CTreeGridWnd::InvalidateGrid()
{
	if (m_hWnd != NULL)
	{
		ResetSize();
		Invalidate(FALSE);
		ResetScrollBars();
	}
}

/////////////////////////////////////////////////////////////////////////////
// CTreeGridWnd diagnostics

#ifdef _DEBUG
void CTreeGridWnd::AssertValid() const
{
	// Sorted tree control doesn't make sense.
	// ASSERT(m_nSortColumn == -1);
	CBaseClass::AssertValid();
}

void CTreeGridWnd::Dump(CDumpContext& dc) const
{
	CBaseClass::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CTreeGridWnd message handlers


void CTreeGridWnd::OnPaint()
{
	// we want to search through the tree control
	// for any dirty nodes, if we find one of these
	// we want to update it's contents
	RefreshAllRoots();		// refresh from our roots down

	CBaseClass::OnPaint();	// pass on to our base instance
}

void CTreeGridWnd::OnDestroy()
{
	CBaseClass::OnDestroy();

}


BOOL CTreeGridWnd::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{

	// HACK begets HACK. CGridControlWnd bypasses command handlers
	// in the grid window if no control is enabled. We need to handle
	// commands even when no control is available. So directly
	// call CGridWnd's OnCmdMsg.	
	return CGridWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}




int CTreeGridWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CBaseClass::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (GetRowCount() != 0)
		Select(GetRow(0));
			
	return 0;
}

LRESULT CTreeGridWnd::OnDismissControl(WPARAM, LPARAM)
{
	m_bForceAccept = TRUE;
	AcceptControl();
	m_bForceAccept = FALSE;

	return 0;
}

LRESULT CTreeGridWnd::OnWarnAndExpand(WPARAM wParam, LPARAM)
{
	CTreeGridRow * pParent = (CTreeGridRow *)wParam;

	if (MsgBox(Question, IDS_EXPANSION_WARN, MB_YESNO|MB_DEFBUTTON2) == IDYES)
	{
		ExpandHelper(pParent, TRUE, FALSE);
	}

	return 0;
}

