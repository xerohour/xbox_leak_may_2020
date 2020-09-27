// dbggrid.cpp : implementation file
//

#include "stdafx.h"
#pragma hdrstop

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CDbgGridWnd


CDbgGridWnd::CDbgGridWnd(CString wndName, CBaseVarsView * pView, BOOL bEditable /* = FALSE */)
	: CBaseClass(WATCH_NUM_COLUMNS), m_strTabName(wndName)
{
	m_bEditable = bEditable;
	m_bAlive = FALSE;
	m_bShowContextPopup = TRUE;
	m_bDragging = FALSE;
	m_bDockable = TRUE;
	m_pView = pView;
						
	if ( bEditable )
	{
		EnableNewRow();
	}
	
	CString strHeader;
	VERIFY(strHeader.LoadString(IDS_COL_NAME));
	SetColumnCaption(COL_NAME, strHeader);

	VERIFY(strHeader.LoadString(IDS_COL_VALUE));
	SetColumnCaption(COL_VALUE, strHeader);

	SetColumnWidth(COL_NAME, GetDefaultNameWidth());
}

CDbgGridWnd::~CDbgGridWnd()
{
}


IMPLEMENT_DYNAMIC(CDbgGridWnd, CTreeGridWnd);

BEGIN_MESSAGE_MAP(CDbgGridWnd, CDbgGridWnd::CBaseClass)
	//{{AFX_MSG_MAP(CDbgGridWnd)
 	ON_COMMAND(ID_EDIT_CLEAR, OnEditClear)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CLEAR, OnUpdateEditClear)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, OnUpdateEditCut)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdateEditPaste)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WU_UPDATEDEBUG, OnUpdateDebug)
	ON_MESSAGE(WU_CLEARDEBUG, OnClearDebug)
END_MESSAGE_MAP()



/////////////////////////////////////////////////////////////////////////////
// CDbgGridWnd diagnostics

#ifdef _DEBUG
void CDbgGridWnd::AssertValid() const
{
	CBaseClass::AssertValid();
}

void CDbgGridWnd::Dump(CDumpContext& dc) const
{
	CBaseClass::Dump(dc);
}
#endif //_DEBUG


int  CDbgGridWnd::GetNameColWidth( ) const
{
	return GetColumnWidth(COL_NAME);
}

int  CDbgGridWnd::GetDefaultNameWidth()
{
	int cxFull = ::GetSystemMetrics(SM_CXFULLSCREEN);
	int nColumnWidth = 90;
	
	if ( cxFull > 1000 )
		nColumnWidth += 70;
	else if (cxFull >= 800)
		nColumnWidth += 40;
	
	return nColumnWidth;	
}

void CDbgGridWnd::RestoreSettings(CPersistWatchTab& rTab, int nColumnWidth)
{
	ASSERT(m_bEditable);
	// This should happen before the window is shown.
	// If this needs to be more general, you might
	// need to add a call to OnSize ( see ::ShowTypeColumn)
	ASSERT(m_hWnd == NULL);

	if ( nColumnWidth != 0 )
		SetColumnWidth(COL_NAME, nColumnWidth);
		
	for ( int i = 0 ; i < rTab.GetWatchCount(); i++ )
	{
		CTM *pTM = new CRootTM(rTab.GetWatchName(i));
		CWatchRow * pRow = new CWatchRow(this, pTM);
		
		VERIFY(InsertTopLevelRow(pRow, FALSE));
	}

}

void CDbgGridWnd::RestoreSettings(CPersistVarsTab& rTab, int nColumnWidth)
{
	if ( nColumnWidth != 0 )
		SetColumnWidth(COL_NAME, nColumnWidth);

	ASSERT(!m_bEditable);
}

void CDbgGridWnd::RememberSettings(CPersistWatchTab& rTab)
{
	ASSERT(m_bEditable);

	// Iterate through the rows and for every top-level row
	// remember the name.
	POSITION pos = GetHeadRowPosition();

	while ( pos != NULL )
	{
		CTreeGridRow *pRow = (CTreeGridRow *)GetNextRow(pos);		

		if (pRow->m_nLevels == 0 && !pRow->IsNewRow())
		{
			CString str;
			pRow->GetColumnText(COL_NAME, str);

			rTab.AddWatchName(str);
		}
	}
}
			
void CDbgGridWnd::RememberSettings(CPersistVarsTab& rTab)
{
	ASSERT(!m_bEditable);
}
	
void CDbgGridWnd::Expand(CTreeGridRow *pRow, BOOL bRedraw /* = TRUE */)
{
	if ( !(DebuggeeAlive() && DebuggeeRunning()))
	{
		CBaseClass::Expand(pRow, bRedraw);
	}
	else
	{
		::MessageBeep(0);
	}
}			

void CDbgGridWnd::Collapse(CTreeGridRow *pRow, BOOL bRedraw /* = TRUE */)
{
	CBaseClass::Collapse(pRow, bRedraw);
}
			
BOOL CDbgGridWnd::InsertTopLevelRow(CTreeGridRow *pRow, BOOL bRedraw /* = TRUE */)
{
	return CBaseClass::InsertTopLevelRow (pRow, bRedraw);
}
			
BOOL CDbgGridWnd::DeleteTopLevelRow(CTreeGridRow *pRow, BOOL bRedraw /* = TRUE */)
{
	return CBaseClass::DeleteTopLevelRow(pRow, bRedraw);
}

			
void CDbgGridWnd::MakeWindowAlive(BOOL bAlive, int iNameWidth)
{
	m_bAlive = bAlive;

	if ( bAlive )	// It is time to wake up.
	{
		if ( GetNameColWidth() != iNameWidth)
		{
			SetColumnWidth(COL_NAME,iNameWidth);
			if (GetSafeHwnd() != NULL)
			{
				CRect client;
				GetClientRect(&client);
				OnSize(SIZE_RESTORED, client.Width(), client.Height());
			}
		}

		OnUpdateDebug(0,0);	// Refresh the contents of the window.
	}
}	

void CDbgGridWnd::DirtyRow(CTreeGridRow * pRow, int tf)
{
	CBaseClass::DirtyRow(pRow, tf);
	// Inform the property sheet that values might have changed.
	if ( GetCurSel( ) == pRow && GetView() != NULL )
		GetView()->OnGridRowSelectionChange();
}
					
CTGNewRow * CDbgGridWnd::CreateNewRow( )
{
	if ( m_bEditable )
	{
		return (new CNewWatchRow(this));
	}
	else {
		ASSERT(FALSE);
		return NULL;
	}
}

void CDbgGridWnd::OnEnterNew(CString str)
{
	if (!AddNewWatch(str))	
	{
		// Couldn't add the row for some reason.
		::MessageBeep(0);
	}
}		

BOOL CDbgGridWnd::AddNewWatch(CString str)
{
	CWatchRow * pRow ;

	return AddNewWatch(str, pRow);
}

BOOL CDbgGridWnd::AddNewWatch(CString str, CWatchRow *& rRow)
{
	ASSERT(m_bEditable);
	ASSERT(!DebuggeeRunning());
	CRootTM *pTM;
/*
	DS #3834 [CFlaat]: we were incorrectly bringing up a wait cursor here, so I replaced
		mismatched DoWaitCursor calls with a CWaitCursor object
*/
	{ // this scope exists to limit the effects of this CWaitCursor object
		CWaitCursor wc;

		pTM = new CRootTM(str);
	}

	if ( pTM )
	{
		CWatchRow *pRow = new CWatchRow(this, pTM);

		if ( pRow && InsertTopLevelRow(pRow))
		{
			rRow = pRow;

			if ( pTM->HadSideEffects() )
				pRow->OnModify();

			return TRUE;
		}
		else
		{
			delete pRow;
		}

	}

	return FALSE;
}

// Due to some context change all the current rows are invalid.
// Use the sledge hammer and clear them all.

void CDbgGridWnd::ClearCurEntries( )
{
 	POSITION pos = GetHeadRowPosition( );

	if ( pos == NULL ) return; // Quick exit.

	// clearing these speeds up the deletion
	CloseActiveElement();
	m_selection.RemoveAll();
	m_posCurRow = NULL;
	m_posTopVisibleRow = NULL;


	// Just blow all the rows out.
	while ( pos != NULL )
	{
		CTreeGridRow *pRow = (CTreeGridRow *)GetNextRow(pos);
		RemoveRow(pRow, FALSE);
		delete pRow;
	}

	InvalidateGrid( );
}

BOOL CDbgGridWnd::IsStrInArray (CStrArray& rgStr, const CString& str)
{
	int nIndex;
	for ( nIndex = 0 ; nIndex < rgStr.GetSize() ; nIndex++ )
	{
		if ( rgStr[nIndex] == str )
			return TRUE;
	}
	return FALSE;
}


void AFXAPI ConstructElements(CWRInfo *pWRInfo, int nCount)
{
	for ( int i = 0; i < nCount ; i++, pWRInfo++ )
	{
		pWRInfo->CWRInfo::CWRInfo();
	}
}

void AFXAPI DestructElements(CWRInfo *pWRInfo, int nCount)
{
	for ( int i = 0; i < nCount ; i++, pWRInfo++ )
	{
		pWRInfo->CWRInfo::~CWRInfo();
	}
}

//
void CDbgGridWnd::RememberPrevState(CWRInfoArray& rgWRInfo)
{
	// Iterate over the list of elements in the thing currently and then
	//
	POSITION pos = GetHeadRowPosition();
	CWRInfo wrInfo;

	while ( pos != NULL )
	{
		CTreeGridRow * pRow = (CTreeGridRow *)GetNextRow(pos);

		pRow->GetColumnText(COL_NAME, wrInfo.strName);
		pRow->GetColumnText(COL_VALUE, wrInfo.strValue);

		if ( pRow->m_nLevels == 0 && !pRow->IsNewRow() && IsExpanded(pRow))
			wrInfo.bExpanded = TRUE;
		else
			wrInfo.bExpanded = FALSE;

		rgWRInfo.Add(wrInfo);
	}
}

// Given a list of names expands the entries corresponding to that name.

void CDbgGridWnd::RefreshUsingPrevState (CWRInfoArray& rgWRInfo)
{
	POSITION pos;
	int nIndex = 1;
	CString strName;

	while ((pos = m_rows.FindIndex(nIndex)) != NULL)
	{
		CWatchRow * pRow = (CWatchRow *)GetRowAt(pos);
		ASSERT( !pRow->IsNewRow());
		pRow->GetColumnText(COL_NAME, strName);
		// Look and see if the name exists in the array.
		int indexWR;
		for ( indexWR = 0; indexWR < rgWRInfo.GetSize() ; indexWR++)
		{
			if (rgWRInfo[indexWR].strName == strName)
			{
				break;
			}
		}

		if ( indexWR != rgWRInfo.GetSize() )
		{
			CWRInfo wrInfo = rgWRInfo[indexWR];
			if ( pRow->m_nLevels == 0 && pRow->IsExpandable() && wrInfo.bExpanded)
				Expand(pRow, FALSE);
			
			CString strValue;
			pRow->GetColumnText(COL_VALUE, strValue);
			if ( wrInfo.strValue != strValue)
				pRow->SetValueChanged();			

		}
		nIndex++;
	}
}
			
CTM *  CDbgGridWnd::GetSelectedTM()
{
	CTreeGridRow * pRow = (CTreeGridRow *)GetCurSel();
	if ( pRow != NULL && !pRow->IsNewRow())
		return ((CWatchRow *)pRow)->GetTM();
	else
		return NULL;
}

void CDbgGridWnd::GetDragSlopRect( const CPoint& pt, CRect & rc )
{
	CGridElement * pElement = ElementFromPoint(pt);
	if (NULL == pElement)
	{
		rc.TopLeft() = pt;
		rc.BottomRight() = pt;
		rc.InflateRect(2*g_dwDragMinRadius, 2*g_dwDragMinRadius);
	}
	else
	{
		pElement->GetRect(rc);
		//ClientToScreen(rc);
		rc.left = rc.right = pt.x;
		rc.left -= rc.Height()/2;
		rc.right += rc.Height()/2;
		rc.InflateRect(g_dwDragMinRadius,g_dwDragMinRadius);
	}
}

BOOL CDbgGridWnd::BeginDrag(UINT nFlags, const CPoint& pt)
{
	CBaseVarsView *pView = GetView();
	if (pView)
	{
		if (!pView->DoDrag(pt))
		{
			// Either canceled the drag or we had a click, but we can't
			// tell which from the DoDrag return (it can't either). 
			//
			// Here's how we figure it out:
			//
			// If the current mouse point is outside a slop rect around the 
			// initial mouse point, then we guess you canceled a drag. 
			// If the mouse is within the rect, then we guess you clicked and 
			// released the right mouse button.
			//
			CPoint ptMouse;
			if (::GetCursorPos(&ptMouse))
			{
				CRect rect;
				GetDragSlopRect( pt, rect );
				ScreenToClient(&ptMouse);

				if (rect.PtInRect(ptMouse))
				{
					// Simulate the ButtonUp that was eaten by MFC/OLE to cancel
					// or not start the drag.
					if (nFlags & MK_LBUTTON)
						OnLButtonUp(nFlags, ptMouse);
					else if (nFlags & MK_RBUTTON)
						OnRButtonUp(nFlags, ptMouse);
				}
			}
		}
		return TRUE;
	}
	else
		return FALSE;
}

BOOL CDbgGridWnd::ImportWatchFromBuffer(LPCSTR lpcstr, DWORD dwSize)
{
	// We will only consider the first token.
	CString strBuf(lpcstr);
	
	ASSERT(dwSize == (DWORD)strBuf.GetLength());

	// remove leading white space.
	const TCHAR rgch[] = {' ', '\t', '\r', '\n', '\0' };
	strBuf = strBuf.Right(dwSize - strBuf.SpanIncluding(rgch).GetLength());
	
	// line terminators
	const TCHAR rgchT[] = {'\r', '\n', '\0'};

	// Get tokens till first line terminator.				
	int index = strBuf.FindOneOf(rgchT);
	if ( index != -1 )
		strBuf = strBuf.Left(index);
	
	if ( strBuf.IsEmpty())
		return FALSE;

	CWatchRow * pRow;
	if ( AddNewWatch(strBuf, pRow) )
	{
		Select(pRow); 	
		ScrollIntoView(pRow);
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}
				
					
// returns a handle to a memory-block handle which has the appropriate
// CF_TEXT data.
HANDLE CDbgGridWnd::CFTextHandle()
{
	CString str;

	POSITION pos = GetHeadRowPosition();

	while (pos != NULL)
	{
		CTreeGridRow * pRow = (CTreeGridRow *)GetNextRow(pos);
		
		if ( IsSelected(pRow) && !pRow->IsNewRow() )
		{
			CString strCol;

			if ( pRow->IsExpandable())
			{
				if (IsExpanded(pRow))
					strCol = "-\t";
				else
					strCol = "+\t";
			}
			else
			{
				strCol = "\t";
			}

			str += strCol;

  			for ( int i = 0; i < GetColumnCount( ); i++ )
  			{
  				pRow->GetColumnText(i, strCol);
				if ( i == (GetColumnCount() - 1) )
					strCol += "\r\n" ;
				else
					strCol += '\t' ;

				str += strCol;
			}
			//
		}
	}
	HGLOBAL hMem = HandleFromString(str);
	return hMem;
}


/////////////////////////////////////////////////////////////////////////////
// CDbgGridWnd message handlers

LRESULT CDbgGridWnd::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	LRESULT lResult;

	if (m_bDockable && !IsPopupMessage(message) &&
		 DkPreHandleMessage(GetSafeHwnd(), message, wParam, lParam, &lResult))
	{
		return lResult;
	}
			 	
	return CBaseClass::WindowProc(message, wParam, lParam);
}

void CDbgGridWnd::ShowContextPopupMenu(POPDESC* ppop, CPoint pt)
{
    TRACE2("CBShow: CDbgGridWnd(%s, %d)\r\n", __FILE__, __LINE__);
    ::CBShowContextPopupMenu(ppop, pt, this);
}


BOOL  CDbgGridWnd::ProcessKeyboard(MSG *pMsg, BOOL bPreTrans /* = FALSE*/)
{
	return CBaseClass::ProcessKeyboard(pMsg, bPreTrans);
}

void CDbgGridWnd::OnEditClear()
{
	// If a control is active give it precedence.
	if (m_pControlWnd->GetSafeHwnd())
	{
		CBaseClass::OnClear( );
		return;
	}

	if ( !DeleteSelectedRows( ) )
		::MessageBeep(0);
}

// NOTE: Do not call this if you are handling clipboard in a derived
// class.
void CDbgGridWnd::OnEditCopy()
{
	if (m_pControlWnd->GetSafeHwnd())
	{
		CBaseClass::OnCopy();
		return;
	}

	if (OpenClipboard())
	{
		HANDLE handle = CFTextHandle();

		if ( handle )
		{
			::EmptyClipboard();
			::SetClipboardData(CF_TEXT, handle);
		}

		CloseClipboard();
	}
}		

void CDbgGridWnd::OnEditCut()
{
	if (m_pControlWnd->GetSafeHwnd())
	{
		CBaseClass::OnCut();
		return;
	}

	HANDLE handle = CFTextHandle();

	if ( !DeleteSelectedRows())
	{
		::GlobalFree(handle);
		::MessageBeep(0);
	}
	else
	{
		if ( handle != NULL  && OpenClipboard())
		{
			::EmptyClipboard();
			::SetClipboardData(CF_TEXT, handle);
			CloseClipboard();
		}
	}
}

void CDbgGridWnd::OnEditPaste()
{
	if (m_pControlWnd->GetSafeHwnd())
	{
		CBaseClass::OnPaste();
		return;
	}

	if (!OpenClipboard())
		return;

	HANDLE hData = GetClipboardData(CF_TEXT);

	if ( hData != NULL )
	{
		CString strText((LPCSTR)RealGlobalLock(hData));

		ImportWatchFromBuffer(strText, strText.GetLength());
		RealGlobalUnlock(hData);
	}

	CloseClipboard();
}
	


void CDbgGridWnd::OnUpdateEditClear(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_bEditable && HasASelection());
}

void CDbgGridWnd::OnUpdateEditCopy(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(HasASelection());
}

void CDbgGridWnd::OnUpdateEditCut(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_bEditable && HasASelection());
}

void CDbgGridWnd::OnUpdateEditPaste(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_bEditable && !DebuggeeRunning() &&
		IsClipboardFormatAvailable(CF_TEXT));
}

LRESULT CDbgGridWnd::OnUpdateDebug(WPARAM, LPARAM)
{

	// Iterate through our windows and call the Update method on each
	// of them.
	POSITION pos = GetHeadRowPosition();

	while (pos != NULL)
	{
		CTreeGridRow * pRow = (CTreeGridRow *)GetNextRow(pos);		
		pRow->Update();
	}

	// Note: it is important that this refresh occurs here.
	// we might expand some items while refreshing and this cannot
	// happen at paint time, because the debuggee might be running by then.
	RefreshAllRoots();


	return 0;
}

LRESULT CDbgGridWnd::OnClearDebug(WPARAM wParam, LPARAM lParam )
{
	CloseActiveElement();		 // If we are in the midst of an edit dismiss the
								 // edit control.

	// If the platform supports quick restart, the EE is not unloaded
	// and the TMs are still valid.

	// Use IsQuickRestart instead of SupportsQuickRestart here
	// so that we do the right thing if the image has been edited
	// by Edit & Continue [georgiop 10/15/97]
	if ( pDebugCurr->IsQuickRestart())
	{
		return OnUpdateDebug(wParam, lParam);
	}
	else
	{
		POSITION pos = GetHeadRowPosition();

		while (pos != NULL)
		{
			CWatchRow * pRow = (CWatchRow *)GetNextRow(pos);		
			pRow->Restart();
		}

		RefreshAllRoots();


		return 0;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CLocalsWnd

CLocalsWnd::CLocalsWnd(CBaseVarsView *pView)
	: CBaseClass("", pView)
{
	m_strTabName.LoadString(SYS_LocalsWin_Title);
	m_bContextValid = FALSE;
}

CLocalsWnd::~CLocalsWnd()
{
}

				
void CLocalsWnd::UpdateEntries( )
{
	CXF curCxf( ::CxfWatch( ));
	
	if ( ! ::DebuggeeAlive( )  ||
		 ! (  curCxf.cxt.hMod && curCxf.cxt.hProc ) )
	{
		// No locals in this context.
		ClearCurEntries( );
	}
	else
	{
	 	if ( m_bContextValid )
		{
			// Check and see if the new context matches the
			// one seen last.
			if (	( m_lastCxf.cxt.hMod  == curCxf.cxt.hMod  )  // same context
					&&
					( m_lastCxf.cxt.hGrp  == curCxf.cxt.hGrp  )
					&&
					( m_lastCxf.cxt.hProc  == curCxf.cxt.hProc  )
					&&
					( m_lastCxf.cxt.hBlk == curCxf.cxt.hBlk  )
	                &&
					( 	GetFrameBPOff( * SHpFrameFrompCXF( &m_lastCxf)) ==
						 GetFrameBPOff( * SHpFrameFrompCXF( &curCxf))
					 ) 														 // same bp.offset
					 &&
					( 	GetFrameBPSeg(*SHpFrameFrompCXF(&m_lastCxf)) ==
						GetFrameBPSeg(*SHpFrameFrompCXF(&curCxf))
					 )														// same bp.seg.
					 &&
					 // didn't cross the prolog boundary.
					 ( !m_bProlog == !SHIsInProlog(SHpCXTFrompCXF(&curCxf))) 								
				)
			{ // We are in the same context , just call the base classes
			  // update debug and let it do the normal refreshing.
			  CBaseClass::OnUpdateDebug(0,0);
			  return;	
			}
		}
		// Update the context information.
		m_bContextValid = TRUE ;
		m_lastCxf = curCxf ;
		m_bProlog = SHIsInProlog(SHpCXTFrompCXF(&curCxf));

		CWRInfoArray rgWRInfo;
		RememberPrevState(rgWRInfo);
		ClearCurEntries(  );
	
		HDEP  hSymL = 0  ;
						
		if ( EEGetHSYMList(	&hSymL,SHpCXTFrompCXF( &curCxf),
					 (HSYMR_function | HSYMR_lexical), NULL, FALSE ) == EENOERROR )
		{
			ASSERT( hSymL != (HDEP)NULL) ;
			PHSL_HEAD lphsymhead = (PHSL_HEAD) BMLock(  hSymL ) ;
			PHSL_LIST lphsyml = (PHSL_LIST)( lphsymhead + 1);

			 for ( int i = 0 ; i != lphsymhead->blockcnt ; i++ )
			 {
			 	for ( int j = 0 ; j != lphsyml->symbolcnt ; j++ )
				{
					if (  SHCanDisplay( lphsyml->hSym[j] ) )
					{
						CRootTM * pTM = new CRootTM((HIND)lphsyml->hSym[j], IS_HSYM);
						CWatchRow * pRow = new CWatchRow(this, pTM);
						VERIFY(InsertTopLevelRow(pRow, FALSE));
					}
				}
				lphsyml  = (PHSL_LIST) &(lphsyml->hSym[j]) ;
			}

			BMUnlock (hSymL);
			EEFreeHSYMList(&hSymL);
		}

		RefreshUsingPrevState(rgWRInfo);
	}
	// If at this stage we still don't have any valid rows in the
	// grid, make sure we try again the next time.

	if ( GetRowCount() == 0 )
		m_bContextValid = FALSE ;
	else
		Select(GetRow(0));

	InvalidateGrid();
}
		
BEGIN_MESSAGE_MAP(CLocalsWnd, CLocalsWnd::CBaseClass)
	//{{AFX_MSG_MAP(CLocalsWnd)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WU_UPDATEDEBUG, OnUpdateDebug)
	ON_MESSAGE(WU_CLEARDEBUG, OnClearDebug)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CLocalsWnd message handlers


LRESULT CLocalsWnd::OnUpdateDebug(WPARAM, LPARAM)
{
	UpdateEntries( );
	return 0;		
}

LRESULT CLocalsWnd::OnClearDebug(WPARAM wParam, LPARAM lParam)
{
	m_bContextValid = FALSE;
	return OnUpdateDebug(wParam, lParam);
}

/////////////////////////////////////////////////////////////////////////////
// CThisWnd

CThisWnd::CThisWnd(CBaseVarsView *pView)
	: CBaseClass( "", pView)
{
	m_strTabName.LoadString(SYS_ThisWin_Title);
}

CThisWnd::~CThisWnd()
{
}

// recursively expand the base classes.
void CThisWnd::ExpandBaseClasses(CWatchRow * pRow)
{
	HBCIA hBCIA;

	if (!pRow->IsExpandable())
		return;

	Expand(pRow, FALSE);

	// This seems to slow down the stepping quite a bit to the point
	// where people think the IDE is hung. We have to speed up
	// EE expansions before we can enable this code back. 
#if 0
	if ( IsExpanded(pRow) && pRow->GetTM()->GetBCIA(&hBCIA) )
	{
		PHINDEX_ARRAY pIndexArray = (PHINDEX_ARRAY)BMLock(hBCIA);
		
		for (int i = 0 ; i < pIndexArray->count ; i++ )
		{
			CWatchRow * pBCRow = (CWatchRow *)GetChildRow(pRow, pIndexArray->rgIndex[i]);
			ASSERT(pBCRow != NULL);
			ExpandBaseClasses(pBCRow);
		}
		BMUnlock(hBCIA);
		pRow->GetTM()->FreeBCIA(&hBCIA);
	}
#endif
}

				
		
		
	
void CThisWnd::UpdateThis( )
{
	ClearCurEntries();

	CRootTM * pTM = new CRootTM("this");

	CWatchRow * pRow = new CWatchRow(this, pTM);
	
	VERIFY(InsertTopLevelRow(pRow, FALSE));

	ExpandBaseClasses(pRow);
	Select(pRow);	// Select the first row.
	InvalidateGrid();
}

		
BEGIN_MESSAGE_MAP(CThisWnd, CThisWnd::CBaseClass)
	//{{AFX_MSG_MAP(CThisWnd)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WU_UPDATEDEBUG, OnUpdateDebug)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CThisWnd message handlers


LRESULT CThisWnd::OnUpdateDebug(WPARAM wParam, LPARAM lParam)
{
	if ( GetRowCount() != 0 )
	{
		CRootTM tmthis("this");
		CWatchRow * pThisRow = (CWatchRow *)GetRow(0);

		if ( tmthis.IsTypeIdentical(pThisRow->GetTM()) )
		{
			return CBaseClass::OnUpdateDebug(wParam, lParam);
		}
	}

	UpdateThis();
			
	return 0;		
}




