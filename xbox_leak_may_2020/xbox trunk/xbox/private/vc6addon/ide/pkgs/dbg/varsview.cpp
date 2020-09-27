// varsview.cpp : implementation file
//

#include "stdafx.h"
#pragma hdrstop
#include "autownd.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

CGridWatchView * g_pWatchView;
CVarsView * g_pVarsView;
CPersistWatch g_persistWatch;
CPersistVariables g_persistVars;
UINT CGridWatchView::g_cRef;
CFontInfo * CGridWatchView::g_pFontInfo;
UINT CVarsView::g_cRef;
CFontInfo * CVarsView::g_pFontInfo;

UINT WSB_CHANGETAB = RegisterWindowMessage("WSB_CHANGETAB");

/////////////////////////////////////////////////////////////////////////////
// CBaseVarsView

IMPLEMENT_DYNAMIC(CBaseVarsView, CDockablePartView)

CBaseVarsView::CBaseVarsView(UINT nIDWnd, UINT nIDPacket)
{
	m_slob.m_pView = this;
	m_nIDWnd = nIDWnd;
	m_nIDPacket = nIDPacket;
	m_bHasVScroll = TRUE;
	m_bFirstNonZeroSize = TRUE;
	m_curIndex = -1;	// No child is active initially.
	c_nSplitterPref = 0;
	m_iNameColWidth = 0;	// Zero means unspecified.
	m_pDragInfo = 0;
}

CBaseVarsView::~CBaseVarsView()
{
	// Delete the children.
	for (int i = 0; i < m_arrChildWnd.GetSize(); i++)
		delete m_arrChildWnd[i];
}

BOOL CBaseVarsView::EnsureWindow( )
{
	BOOL bRet = TRUE;

	// No window created yet.
	if ( GetSafeHwnd( ) == NULL )
	{
		DWORD dwStyle = WS_CHILD | WS_CLIPCHILDREN | DWS_STRETCHY ;

		bRet = Create(NULL, NULL, dwStyle, CRect(0,0,0,0),
						 theApp.m_pMainWnd,
						 m_nIDWnd);
		m_pPacket = theApp.GetPacket(PACKAGE_DEBUG, m_nIDPacket);
	}

	return bRet;
}
			
		
BEGIN_MESSAGE_MAP(CBaseVarsView, CBaseVarsView::CBaseClass)
	//{{AFX_MSG_MAP(CBaseVarsView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_SETFOCUS()
	//}}AFX_MSG_MAP
	ON_REGISTERED_MESSAGE(WSB_CHANGETAB, OnChangeTab)
	ON_MESSAGE(WU_UPDATEDEBUG, OnUpdateDebug)
	ON_MESSAGE(WU_CLEARDEBUG, OnClearDebug)
END_MESSAGE_MAP()



/////////////////////////////////////////////////////////////////////////////
// CBaseVarsView diagnostics

#ifdef _DEBUG
void CBaseVarsView::AssertValid() const
{
	ASSERT(m_curIndex < m_arrChildWnd.GetSize( ));
	CBaseClass::AssertValid();
}

void CBaseVarsView::Dump(CDumpContext& dc) const
{
	CBaseClass::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CBaseVarsView overrides

LRESULT CBaseVarsView::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	LRESULT lResult;

	if ( DkPreHandleMessage(GetSafeHwnd(), message, wParam, lParam, &lResult) )
	{
		return lResult;
	}	
		
	return CBaseClass::WindowProc(message, wParam, lParam);
}


void CBaseVarsView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView)
{
	ShowSelection(bActivate);
	CBaseClass::OnActivateView(bActivate, pActivateView, pDeactiveView);
}

/////////////////////////////////////////////////////////////////////////////
// CBaseVarsView message handlers

BOOL CBaseVarsView::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// First let the active child window get a shot at handling the command.
	if (m_curIndex != -1 &&
		m_arrChildWnd[m_curIndex]->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo)
	   )
	{
		return TRUE;
	}
			
	return CBaseClass::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

int CBaseVarsView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CBaseClass::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (::CreateWindow(_T("SCROLLBAR"), NULL,
		SBS_SIZEBOX | WS_DISABLED | WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS,
		0, 0, 1, 1, m_hWnd, (HMENU)AFX_IDW_SIZE_BOX,
		AfxGetInstanceHandle(), NULL) == NULL)
	{
		return -1;
	}

	if (::CreateWindow(_T("SCROLLBAR"), NULL,
		SBS_VERT | WS_CHILD,
		0, 0, 1, 1, m_hWnd, (HMENU)AFX_IDW_VSCROLL_FIRST,
		AfxGetInstanceHandle(), NULL) == NULL)
	{
		return -1;
	}
	
	m_widgetScroller.SetSplitterPref(c_nSplitterPref);

	if (!m_widgetScroller.Create(NULL, NULL,
			WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS,
			CRect(0, 0, 1, 1), this, AFX_IDW_HSCROLL_FIRST))
	{
		return -1;
	}

	// We don't use our scrollbars.
	GetScrollBarCtrl(SB_HORZ)->ShowWindow(SW_HIDE);
	GetScrollBarCtrl(SB_VERT)->ShowWindow(SW_HIDE);

	m_DropTarget.Register(this);

	return 0;
}

void CBaseVarsView::PostNcDestroy()
{
	// re-fetch the final column width so we will restore it correctly
	// the next time we open this window.
	if ( m_curIndex != -1 )
		m_iNameColWidth = m_arrChildWnd[m_curIndex]->GetNameColWidth();

	m_font.Detach();	
	delete this ; // Dockable windows delete themselves.
}

BOOL CBaseVarsView::PreTranslateMessage(MSG* pMsg)
{
	int nTab;

	switch (pMsg->message)
	{
	case WM_KEYDOWN:
		switch (pMsg->wParam)
		{
		case VK_NEXT:
		case VK_PRIOR:
			if ( GetKeyState(VK_CONTROL) >= 0)
				break;

			if ( m_curIndex != -1 )
			{
				if ( pMsg->wParam == VK_NEXT )
				{
					if ( m_curIndex == (m_arrChildWnd.GetSize() - 1))
						nTab = 0;
					else
						nTab = m_curIndex + 1;
				}
				else
				{
					if ( m_curIndex == 0 )
						nTab = m_arrChildWnd.GetSize( ) - 1;
					else
						nTab = m_curIndex - 1;
				}
				m_widgetScroller.ActivateTab(nTab);
				return TRUE;
			}
		}
	}
								
	return CBaseClass::PreTranslateMessage(pMsg);
}


CScrollBar* CBaseVarsView::GetScrollBarCtrl(int nBar) const
{
	ASSERT ( nBar == SB_HORZ || nBar == SB_VERT );

	return ((CScrollBar*) (GetDescendantWindow(nBar==SB_HORZ?
				AFX_IDW_HSCROLL_FIRST : AFX_IDW_VSCROLL_FIRST)));		
}

void CBaseVarsView::UpdateTabs( )
{
	m_widgetScroller.ResetTabs( );

	// We should have atleast one child.
	ASSERT ( m_arrChildWnd.GetSize( ) != 0);
	
	for ( int i = 0; i < m_arrChildWnd.GetSize( ) ; i++ )
	{
		m_widgetScroller.AddTab(m_arrChildWnd[i]->GetTabName(), 0);
	}

	if ( m_curIndex == -1)
		m_curIndex = 0;
	
	ASSERT(m_curIndex < m_arrChildWnd.GetSize());
		
	m_widgetScroller.ActivateTab(m_curIndex);
}

			

LRESULT CBaseVarsView::OnChangeTab(WPARAM wParam, LPARAM)
{
	m_iNameColWidth = m_arrChildWnd[m_curIndex]->GetNameColWidth();

	// First tell the previous window it is time to sleep
	m_arrChildWnd[m_curIndex]->MakeWindowAlive(FALSE, m_iNameColWidth);
	m_curIndex = wParam;

	// the second option with be to maintain all the windows
	// at the correct size and just BringWindowToTop when the tabs
	// change.
	GetActiveGrid()->SetWindowPos(&wndTop, 0, 0, 0, 0,
					SWP_NOMOVE | SWP_NOREDRAW | SWP_NOSIZE );
	// Position the window correctly.
	RecalcLayout();

	// Force a repaint on the grid. This strange order is neccessary
	// so we don't see the border of the window being painted, which
	// causes a flicker on the edge of the currently selected tab.
	// FUTURE : Could we do this a better way?
	GetActiveGrid()->Invalidate();
	GetActiveGrid()->SetWindowPos(&wndTop, 0, 0, 0, 0,
					SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_DRAWFRAME);
	
   	m_arrChildWnd[m_curIndex]->MakeWindowAlive(TRUE, m_iNameColWidth);

	// If we or our progeny had the Focus
	// then set it to the child we just activated.
	CWnd * pWnd = CWnd::GetFocus();

	if (this == pWnd || (pWnd && IsChild(pWnd)))
		m_arrChildWnd[m_curIndex]->SetFocus();

	// Finally inform ourself that the selection within the grid's has changed.
	OnGridRowSelectionChange();
	return 0;
}

void CBaseVarsView::OnSize(UINT nType, int cx, int cy)
{
	if ( nType != SIZE_MINIMIZED && cx > 0 && cy > 0 )
	{
		RecalcLayout( );
		if ( m_bFirstNonZeroSize )
		{
			// HACK the hack below is probably not neccessary if we
			// get rid of the zeroeth column altogether.
			// The grid will truncate the first column to zero-width if we do the
			// UpdateTabs right after Creating the grid, since it is still 0-width
			// We depend on the first-column sizing itself
			m_bFirstNonZeroSize = FALSE;
			UpdateTabs( );
		}
	}

	CBaseClass::OnSize(nType, cx, cy);
}

void CBaseVarsView::RecalcLayout( )
{
	int cxVScroll = GetSystemMetrics(SM_CXVSCROLL);
	int cyHScroll = GetSystemMetrics(SM_CYHSCROLL);
	int cxBorder = GetSystemMetrics(SM_CXBORDER);
	int cyBorder = GetSystemMetrics(SM_CYBORDER);
	
	CRect view;
	GetClientRect(view);

	// We assume
	// a) The view itself does not have a border.
	// b) The child window has a border.

	ASSERT((GetStyle() & WS_BORDER) == 0);
	ASSERT( (m_curIndex == -1) ||
		    ((m_arrChildWnd[m_curIndex]->GetStyle() & WS_BORDER) != 0) );

	BOOL bHasVScroll = m_bHasVScroll;
	if (bHasVScroll)
	{
		CWnd* pScrollBar = GetDlgItem(AFX_IDW_VSCROLL_FIRST);
		ASSERT(pScrollBar != NULL);
		if ((pScrollBar->GetStyle() & WS_VISIBLE) == 0)
			bHasVScroll = FALSE;
	}

	if (bHasVScroll)
	{
		view.right -= cxVScroll;
	 	// NOTE: This code has never been tried with the vertical scrollbars
		// visible. Some tweaking might be required to get the
		// vertical toolbar and size box to line up.
	 	ASSERT(FALSE);
	}

	view.bottom -= cyHScroll;

	HDWP hDWP = BeginDeferWindowPos(4);

	CWnd* pScrollBar;
	pScrollBar = GetDlgItem(AFX_IDW_HSCROLL_FIRST);
	ASSERT(pScrollBar != NULL);


	int nOverlapSizeBox;
	if (!gpISrc->GetSrcEnvironParam(ENV_HSCROLL) && bHasVScroll)
		nOverlapSizeBox = cxVScroll;
	else
		nOverlapSizeBox = 0;

	DeferWindowPos(hDWP, pScrollBar->m_hWnd, HWND_TOP,
		0 , view.bottom,
		view.Width() + nOverlapSizeBox, cyHScroll,
		SWP_NOACTIVATE);

	CWnd* pSizeBox;
	pSizeBox = GetDlgItem(AFX_IDW_SIZE_BOX);
	if (pSizeBox != NULL)
	{
		DeferWindowPos(hDWP, pSizeBox->m_hWnd, HWND_TOP,
			view.right + cxBorder, view.bottom + cyBorder,
			cxVScroll - 2 * cxBorder, cyHScroll - 2 * cyBorder,
			SWP_NOACTIVATE);
	}

	pScrollBar = GetDlgItem(AFX_IDW_VSCROLL_FIRST);
	if (pScrollBar != NULL)
	{
		DeferWindowPos(hDWP, pScrollBar->m_hWnd, NULL,
			view.right, 0,
			cxVScroll, view.Height() ,
			SWP_NOZORDER | SWP_NOACTIVATE);
	}

	if (m_curIndex != -1)
	{
		// The bottom border is provided by the widget bar.
		DeferWindowPos(hDWP, m_arrChildWnd[m_curIndex]->GetSafeHwnd(),
			NULL, 0, 0,
			view.Width(), view.Height() + cyBorder,
			SWP_NOZORDER | SWP_NOACTIVATE);
	}

	EndDeferWindowPos(hDWP);
}

CSlob * CBaseVarsView::GetSelection( )
{
	return &m_multiSlob;
}

CSlob * CBaseVarsView::GetEditSlob( )
{
	return GetSelection();
}

void	CBaseVarsView::OnGridRowSelectionChange( )
{
	CDbgGridWnd *pGrid = GetActiveGrid();
	
	m_multiSlob.Clear();

	if (pGrid == NULL)
		return;

	POSITION pos = pGrid->GetHeadSelPosition();
	if ( pos == NULL)
		return;				

	CGridRow * pRow = (CGridRow *)(pGrid->GetNextSel(pos)->GetRow());

	// We have only one item selected and that is the caption row.
	if ( pos == NULL && pRow == pGrid->GetCaptionRow() )
		return;

	m_slob.InformDependants(P_VarExpr);
	m_slob.InformDependants(P_VarType);
	m_slob.InformDependants(P_VarValue);

	if (pGrid->GetCurSel() != NULL)
	{
		m_multiSlob.Add(&m_slob);
	}
	else
	{
		m_multiSlob.Add(&m_slob);
		m_multiSlob.Add(&m_fakeSlob);
	}
}	
	
			
		
void CBaseVarsView::OnSetFocus(CWnd* pOldWnd)
{
	// Just pass on the focus to our child.	
	if (m_curIndex != -1)
	{
		CWnd *pWnd = m_arrChildWnd[m_curIndex];

		ASSERT(pWnd->GetSafeHwnd() != NULL);
		pWnd->SetFocus( );
	}	
}

void CBaseVarsView::ResetWidgetScrollBar( )
{
	m_widgetScroller.Invalidate(FALSE);
}

LRESULT CBaseVarsView::OnUpdateDebug(WPARAM wParam, LPARAM lParam)
{
	CTreeGridWnd *pGrid = GetActiveGrid( );
	
	if ( pGrid )
		return pGrid->SendMessage(WU_UPDATEDEBUG, wParam, lParam);
	else
		return Default( );
}


LRESULT CBaseVarsView::OnClearDebug(WPARAM wParam, LPARAM lParam)
{
	// On Clear debug we have to iterate over the windows as the
	// underlying data structures might no longer be valid between
	// debug sessions. All the grids need a chance to deal with this. 	
	for ( int i = 0; i < m_arrChildWnd.GetSize() ; i++ )
	{
		m_arrChildWnd[i]->SendMessage(WU_CLEARDEBUG, wParam, lParam);
	}

	return 0;
}


void CBaseVarsView::OnFontColorChange( )
{
	ASSERT(m_pFontInfo != NULL);
	if (m_font.GetSafeHandle())
		m_font.Detach();

	m_font.Attach(m_pFontInfo->m_hFont);

	// Get the new colors.
	FMT_ELEMENT *rgfmtel = pfmtelForVarsWnd(m_nIDWnd);
	COLORREF colText = rgfmtel[FMTEL_TEXT].rgbText;
	COLORREF colBkground = rgfmtel[FMTEL_TEXT].rgbBackground;
	COLORREF colSelc = rgfmtel[FMTEL_SELC].rgbBackground;
	COLORREF colSelcText = rgfmtel[FMTEL_SELC].rgbText;
	COLORREF colChangeText = rgfmtel[FMTEL_VCHI].rgbText;
	COLORREF colChangeBk = rgfmtel[FMTEL_VCHI].rgbBackground;
			
	for ( int i = 0 ; i < m_arrChildWnd.GetSize(); i++ )
	{
		m_arrChildWnd[i]->SetColor(colText, colBkground, colSelc, colSelcText);
		m_arrChildWnd[i]->SetChangeHighlightColors(colChangeText, colChangeBk);
		m_arrChildWnd[i]->SetFont(&m_font);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CVarsSlob + helpers to do the property page for the variables window.

BEGIN_IDE_CONTROL_MAP(CVarsPropPage, IDDP_VARS_GEN, IDS_GENERAL)
	MAP_THIN_TEXT(IDC_VAR_TYPE, P_VarType)
	MAP_THIN_TEXT(IDC_VAR_EXPR, P_VarExpr)
	MAP_THIN_TEXT(IDC_VAR_VALUE, P_VarValue)
END_IDE_CONTROL_MAP()

IMPLEMENT_DYNCREATE(CVarsSlob, CSlob)

CString CVarsSlob::s_strPropCaption;
CVarsPropPage CVarsSlob::s_varsPropPage;

CVarsSlob::CVarsSlob()
{
}

CVarsSlob::~CVarsSlob()
{
}

BOOL CVarsSlob::SetupPropertyPages(CSlob* pNewSel, BOOL bSetCaption)
{
	AddPropertyPage(&s_varsPropPage, this);

	CSlob::SetupPropertyPages(pNewSel, FALSE);

	if( bSetCaption )
	{
		if (s_strPropCaption.IsEmpty())
			s_strPropCaption.LoadString(IDS_VARS_PROP_TITLE);
		SetPropertyCaption(s_strPropCaption);
	}

	return TRUE;
}

GPT CVarsSlob::GetStrProp(UINT nPropID, CString& val)
{
	ASSERT(m_pView != NULL);
	CDbgGridWnd *pGridWnd = m_pView->GetActiveGrid();

	if (pGridWnd == NULL)
		return invalid;
	
	CTreeGridRow *pRow = pGridWnd->GetCurSel();

	if (pRow == NULL || pRow->IsNewRow())
		return invalid;

	CTM * pTM = ((CWatchRow *)pRow)->GetTM();
	if (pTM == NULL)
		return invalid;

	switch (nPropID)
	{
	default:
		return CSlob::GetStrProp(nPropID, val);
	
	case P_VarExpr:
		val = pTM->GetExpr();
		// If the watch is in error, the EE could return an empty
		// string for the expression.
		if (val.IsEmpty())
			val = pTM->GetName();

		break;
		
	case P_VarType:
		val = pTM->GetType();
		break;
		
	case P_VarValue:
		val = pTM->GetValue();;
		break;
	}

	return valid;
}
					


 	
/////////////////////////////////////////////////////////////////////////////
// CGridWatchView

IMPLEMENT_DYNCREATE(CGridWatchView, CBaseVarsView)

CGridWatchView::CGridWatchView():CBaseClass(IDDW_VCPP_WATCH_WIN, PACKET_NIL)
{
	m_iNameColWidth = runDebugParams.iWatchNameColWidth;

	if (g_cRef++ == 0)
		g_pFontInfo = new CFontInfo(*(pfmtcatForVarsWnd(m_nIDWnd)->pLogFont));

	m_pFontInfo = g_pFontInfo;
}

CGridWatchView::~CGridWatchView()
{
	runDebugParams.iWatchNameColWidth = m_iNameColWidth;

	if (--g_cRef == 0)
	{
		delete g_pFontInfo;
		g_pFontInfo = NULL;
	}
}

// Static function to make sure the watch window is available.
BOOL CGridWatchView::EnsureWatchWindow()
{
	if ( g_pWatchView == NULL )
	{
		DkWShowWindow(MAKEDOCKID(PACKAGE_DEBUG, IDDW_VCPP_WATCH_WIN), TRUE);
	}
	
	return (g_pWatchView != NULL);
}		 				
	
BEGIN_MESSAGE_MAP(CGridWatchView, CGridWatchView::CBaseClass)
	//{{AFX_MSG_MAP(CGridWatchView)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CGridWatchView overrides.


BEGIN_POPUP_MENU(GridWatch)
	MENU_ITEM(ID_EDIT_PASTE)
	MENU_SEPARATOR()
	MENU_ITEM(IDM_DEBUG_RADIX)
	MENU_SEPARATOR()
	MENU_ITEM(IDM_TOGGLE_MDI)
	MENU_ITEM(ID_WINDOW_POPUP_HIDE)
	MENU_SEPARATOR()
	MENU_ITEM(IDM_WINDOW_SHOWPROPERTIES)
END_POPUP_MENU()

void CGridWatchView::ShowContextPopupMenu(CPoint pt)
{
    #ifndef NO_CMDBARS
        TRACE2("GridWatch (%s, %d)\r\n", __FILE__, __LINE__);
        ::CBShowContextPopupMenu(MENU_CONTEXT_POPUP(GridWatch), pt);
    #else
    	::ShowContextPopupMenu(MENU_CONTEXT_POPUP(GridWatch), pt);
    #endif
}

BOOL CGridWatchView::AddNewWatch(CString str)
{
	return(GetActiveGrid()->AddNewWatch(str));
}
			
/////////////////////////////////////////////////////////////////////////////
// CGridWatchView diagnostics

#ifdef _DEBUG
void CGridWatchView::AssertValid() const
{
	CBaseClass::AssertValid();
}

void CGridWatchView::Dump(CDumpContext& dc) const
{
	CBaseClass::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CGridWatchView message handlers


int CGridWatchView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CBaseClass::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// Add the child windows to the view.
	DWORD dwStyle = WS_BORDER | WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE;
	CRect rectNull(0,0,0,0);

	// Go through adding the various child windows
	ASSERT(g_persistWatch.GetTabCount() != 0);

	for ( int i = 0; i < g_persistWatch.GetTabCount(); i++ )
	{
		CPersistWatchTab& rPersistTab = g_persistWatch.GetWatchTab(i);
		CDbgGridWnd * pGrid = new CDbgGridWnd(rPersistTab.GetTabName(), this, TRUE);
										
		pGrid->RestoreSettings(rPersistTab, m_iNameColWidth);
				
		if (pGrid->Create(dwStyle, rectNull, this, (i + 1)))
		{
			m_arrChildWnd.Add(pGrid);
		}
		
	}
	
	m_widgetScroller.SetSplitterPref(g_persistWatch.m_nSplitterPref);
							
	// Add our child windows to the tab.
	m_curIndex = g_persistWatch.GetActiveTabIndex();

	ShowSelection(FALSE);	// don't show selection till we get the ActivateView.
	OnFontColorChange();

	return 0;
}

void CGridWatchView::OnDestroy()
{
	CBaseClass::OnDestroy();

	g_persistWatch.ClearAll();
	g_persistWatch.m_nSplitterPref = m_widgetScroller.GetSplitterPref();

	// Remember the watches so we get them the next time over.
	for ( int i = 0; i < m_arrChildWnd.GetSize() ; i++ )
	{
		CDbgGridWnd *pGrid = m_arrChildWnd[i];
		g_persistWatch.AddWatchTab(pGrid->GetTabName());
		pGrid->RememberSettings(g_persistWatch.GetWatchTab(i));
	}
		
	g_persistWatch.SetActiveTab(m_curIndex);	
	
	g_pWatchView = NULL;
}


/////////////////////////////////////////////////////////////////////////////
// CVarsCombo

CVarsCombo::CVarsCombo()
{
}

CVarsCombo::~CVarsCombo()
{
}


BEGIN_MESSAGE_MAP(CVarsCombo, CComboBox)
	//{{AFX_MSG_MAP(CVarsCombo)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CVarsCombo message handlers

BOOL CVarsCombo::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message != WM_KEYDOWN)
		return CComboBox::PreTranslateMessage(pMsg);

	switch (pMsg->wParam)
	{
	default:
		return CComboBox::PreTranslateMessage(pMsg);

	case VK_ESCAPE:
		if ( !( GetKeyState( VK_SHIFT ) & ~1 ) &&
		!( GetKeyState( VK_CONTROL ) & ~1 ))
		{
			m_pToolBar->m_fDoUpdate = FALSE;

			ShowDropDown(FALSE);
			return TRUE;
		}
		break;

	case VK_RETURN:
		if (GetDroppedState())
		{
			ShowDropDown(FALSE);
			return TRUE;
		}
		break;

	case VK_UP:
	case VK_DOWN:
	case VK_NEXT:
	case VK_PRIOR:
		break;
	}

	// Getting this far means we should short-circuit the normal
	// accelerator processing and dispatch immediately...
	TranslateMessage(pMsg);
	DispatchMessage(pMsg);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CVarsDialogBar

BOOL CVarsDialogBar::OnInitDialog()
{
	m_list.m_pToolBar = this;


	// dialog box is coming up, initialize items
	if (!m_list.SubclassDlgItem(IDM_CTL_VARS_FUNCS, this))
		return FALSE;

	m_list.SetExtendedUI();
	m_list.SetFont(GetStdFont(font_Normal));

	RecalcBarLayout(DkWGetDock(MAKEDOCKID(PACKAGE_DEBUG, IDDW_VCPP_VARIABLES_WIN)), TRUE);

	return TRUE;
}

void CVarsDialogBar::OnUpdateCmdUI(CFrameWnd *,  BOOL)
{
	m_list.EnableWindow(DebuggeeAlive() && !DebuggeeRunning());
}

/////////////////////////////////////////////////////////////////////////////
// CVarsView

IMPLEMENT_DYNCREATE(CVarsView, CBaseVarsView)

CVarsView::CVarsView()
	:CBaseClass(IDDW_VCPP_VARIABLES_WIN, PACKET_NIL)
{
	m_iNameColWidth = runDebugParams.iVarsNameColWidth;

	if (g_cRef++ == 0)
		g_pFontInfo = new CFontInfo(*(pfmtcatForVarsWnd(m_nIDWnd)->pLogFont));

	m_pFontInfo = g_pFontInfo;
}

CVarsView::~CVarsView()
{
	runDebugParams.iVarsNameColWidth = m_iNameColWidth;

	if (--g_cRef == 0)
	{
		delete g_pFontInfo;
		g_pFontInfo = NULL;
	}
}

BOOL CVarsView::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_SYSKEYDOWN && m_toolbar.PreTranslateSysKey(pMsg))
	{
		if (&m_toolbar.m_list == GetFocus( ))
			m_toolbar.m_list.ShowDropDown( );

		return TRUE;
	}
				
	return CBaseClass::PreTranslateMessage(pMsg);
}

BOOL CVarsView::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT *pResult)
{
	// Route notifies to main window, for tool tip text handling.
	*pResult = AfxGetMainWnd()->SendMessage(WM_NOTIFY, wParam, lParam);

	return TRUE;
}

// It is unfortuante that this code is almost the same as
// the base classes RecalcLayout but duplicated here.
// We want to however do the multi-window layout only once.

void CVarsView::RecalcLayout( )
{
	int cxVScroll = GetSystemMetrics(SM_CXVSCROLL);
	int cyHScroll = GetSystemMetrics(SM_CYHSCROLL);
	int cxBorder = GetSystemMetrics(SM_CXBORDER);
	int cyBorder = GetSystemMetrics(SM_CYBORDER);
	
	CRect view;
	GetClientRect(view);

	// We assume
	// a) The view itself does not have a border.
	// b) The child window has a border.
	ASSERT((GetStyle() & WS_BORDER) == 0);
	ASSERT( (m_curIndex == -1) ||
		    ((m_arrChildWnd[m_curIndex]->GetStyle() &  WS_BORDER) != 0) );

	BOOL bHasToolBar = FALSE;
	if ((m_toolbar.GetStyle( ) & WS_VISIBLE) != 0)
	{
		view.top += m_toolbar.m_sizeDefault.cy;
		bHasToolBar = TRUE;
	}

	if ( bHasToolBar && UseWin4Look() )
		m_toolbar.RecalcBarLayout(DkWGetDock(MAKEDOCKID(PACKAGE_DEBUG, IDDW_VCPP_WATCH_WIN)));		

	BOOL bHasVScroll = m_bHasVScroll;
	if (bHasVScroll)
	{
		CWnd* pScrollBar = GetDlgItem(AFX_IDW_VSCROLL_FIRST);
		ASSERT(pScrollBar != NULL);
		if ((pScrollBar->GetStyle() & WS_VISIBLE) == 0)
			bHasVScroll = FALSE;
	}

	if (bHasVScroll)
	{
		view.right -= cxVScroll;
	 	// NOTE: This code has never been tried with the vertical scrollbars
		// visible. Some tweaking might be required to get the
		// vertical toolbar and size box to line up.
	 	ASSERT(FALSE);
	}

	view.bottom -= cyHScroll;

	HDWP hDWP = BeginDeferWindowPos(5);

	CWnd* pScrollBar;
	pScrollBar = GetDlgItem(AFX_IDW_HSCROLL_FIRST);
	ASSERT(pScrollBar != NULL);

	int nOverlapSizeBox;
	if (!gpISrc->GetSrcEnvironParam(ENV_HSCROLL) && bHasVScroll)
		nOverlapSizeBox = cxVScroll;
	else
		nOverlapSizeBox = 0;

	DeferWindowPos(hDWP, pScrollBar->m_hWnd, HWND_TOP,
		0, view.bottom,
		view.Width() + nOverlapSizeBox, cyHScroll,
		SWP_NOACTIVATE);

	if ( bHasToolBar )
	{
		CRect rectClient;
		GetClientRect(&rectClient);

		DeferWindowPos(hDWP, m_toolbar.m_hWnd, HWND_TOP,
			rectClient.left, rectClient.top,
			rectClient.Width(), m_toolbar.m_sizeDefault.cy,
			SWP_NOACTIVATE);
	}

	CWnd* pSizeBox;
	pSizeBox = GetDlgItem(AFX_IDW_SIZE_BOX);
	if (pSizeBox != NULL)
	{
		DeferWindowPos(hDWP, pSizeBox->m_hWnd, HWND_TOP,
			view.right + cxBorder, view.bottom + cyBorder,
			cxVScroll - 2 * cxBorder, cyHScroll - 2 * cyBorder,
			SWP_NOACTIVATE);
	}

	pScrollBar = GetDlgItem(AFX_IDW_VSCROLL_FIRST);
	if (pScrollBar != NULL)
	{
		DeferWindowPos(hDWP, pScrollBar->m_hWnd, NULL,
			view.right, 0,
			cxVScroll, view.Height(),
			SWP_NOZORDER | SWP_NOACTIVATE);
	}

	if (m_curIndex != -1)
	{

		DeferWindowPos(hDWP, m_arrChildWnd[m_curIndex]->GetSafeHwnd(),
			NULL, 0, view.top,
			view.Width(), view.Height() + cyBorder,
			SWP_NOZORDER | SWP_NOACTIVATE);
	}

	EndDeferWindowPos(hDWP);

	// now do the computation to move the combo box
	// appropriately within the toolbar.

	CRect rcBar, rcList;
	m_toolbar.GetWindowRect(&rcBar);
	m_toolbar.m_list.GetDroppedControlRect(&rcList);

	int wAvail = rcBar.right - rcBar.left;	 // width of window
	int iOffs  = rcList.left - rcBar.left;	 // offset of listbox

	wAvail -= iOffs + 6;	 				 // remove used space + padding

	// we now know how wide we can make the listbox but lets not
	// make it too small... we'd rather let some fall off the edge
	// if its going to be too small

//	if (wAvail < 150) wAvail = 150;
	if (wAvail < 24) wAvail = 24;
	// Even if the width of the window hasn't changed we do the SetWindowPos.
	// This causes the drop-down to get dismissed. If this is not done the drop
	// down detaches from the combo-box ( see OlYMPUS:1375)
	// if (wAvail == rcList.Width()) return;

	m_toolbar.m_list.ShowWindow(SW_HIDE);	// 3-d fix - keep window invalid.
	m_toolbar.m_list.SetWindowPos(NULL,0,0, wAvail, rcList.Height(),
			SWP_NOACTIVATE|SWP_NOMOVE|SWP_NOZORDER);
	m_toolbar.m_list.ShowWindow(SW_SHOWNOACTIVATE);
}

	
BEGIN_MESSAGE_MAP(CVarsView, CVarsView::CBaseClass)
	//{{AFX_MSG_MAP(CVarsView)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
	ON_COMMAND(IDM_DBGSHOW_VARS_BAR, OnToggleVarsBar)
	ON_UPDATE_COMMAND_UI(IDM_DBGSHOW_VARS_BAR, OnUpdateToggleVarsBar)
	ON_CBN_DROPDOWN(IDM_CTL_VARS_FUNCS, OnDropDown)
	ON_CBN_CLOSEUP(IDM_CTL_VARS_FUNCS, OnCloseUp)
	ON_MESSAGE(WU_UPDATEDEBUG, OnUpdateDebug)
	ON_MESSAGE(WU_FREEZEDATA, OnFreezeData)
END_MESSAGE_MAP()


BEGIN_POPUP_MENU(Variables)
	MENU_ITEM(IDM_DBGSHOW_VARS_BAR)
	MENU_SEPARATOR()
	MENU_ITEM(IDM_DEBUG_RADIX)
	MENU_SEPARATOR()
	MENU_ITEM(IDM_TOGGLE_MDI)
	MENU_ITEM(ID_WINDOW_POPUP_HIDE)
	MENU_SEPARATOR()
	MENU_ITEM(IDM_WINDOW_SHOWPROPERTIES)
END_POPUP_MENU()

void CVarsView::ShowContextPopupMenu(CPoint pt)
{
    #ifndef NO_CMDBARS
        TRACE2("Variables (%s, %d)\r\n", __FILE__, __LINE__);
        ::CBShowContextPopupMenu(MENU_CONTEXT_POPUP(Variables), pt);
    #else
    	::ShowContextPopupMenu(MENU_CONTEXT_POPUP(Variables), pt);
    #endif
}


void CVarsView::ResetCallStack( )
{
	m_toolbar.m_fUpdateListbox = TRUE;
}

void CVarsView::UpdateCallStack( )
{
	// not safe to update if debuggee is running...
	if (!DebuggeeAlive() || DebuggeeRunning())
		return;
	
	int iCall = (iCallsCxf < 0 ) ? 0 : iCallsCxf;

	char buf[256];
	CLGetWalkbackStack(hpidCurr, htidCurr, iCall + 1);
	CLGetProcName(iCall, buf, sizeof(buf), FALSE);

	CString str;
	m_toolbar.m_list.GetWindowText(str);
	if (str == buf)
		return;

	m_toolbar.m_list.ResetContent();
	m_toolbar.m_fUpdateListbox = TRUE;
	m_toolbar.m_list.AddString(buf);
	m_toolbar.m_list.SetCurSel(0);
}

			
/////////////////////////////////////////////////////////////////////////////
// CVarsView diagnostics

#ifdef _DEBUG
void CVarsView::AssertValid() const
{
	CBaseClass::AssertValid();
}

void CVarsView::Dump(CDumpContext& dc) const
{
	CBaseClass::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CVarsView message handlers.

int CVarsView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CBaseClass::OnCreate(lpCreateStruct) == -1)
		return -1;

	// First create the toolbar window.
	m_toolbar.SetDockableID(MAKEDOCKID(PACKAGE_DEBUG, IDDW_VCPP_WATCH_WIN));	

	if (!m_toolbar.Create(this, IDD_VARSVIEW_DIALOGBAR,
		CBRS_ALIGN_TOP | CBRS_BORDER_3D, AFX_IDW_TOOLBAR))
	{		
		TRACE("Failed to Create/Init Toolbar\n");
		return -1;
	}

	// must manually initialize the dialog bar because there will be no
	// WM_INIT message as there is no dialog per se.  We are just a dialog
	// template...

	if (!m_toolbar.OnInitDialog())
	{
		TRACE("Failed to Init Toolbar\n");
		return -1;
	}

	if (!runDebugParams.fVarsToolbar)
	{
		m_toolbar.ShowWindow(SW_HIDE);
	}


	// Add the child windows to the view.
	DWORD dwStyle = WS_BORDER | WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE;
	CRect rectNull(0,0,0,0);

	BOOL fCreateCPPWindows =
		runDebugParams.fWatchExt &&
		(IsPackageLoaded(PACKAGE_LANGCPP) ||
		 !IsPackageLoaded(PACKAGE_LANGFOR));

	if (fCreateCPPWindows)
	{
		CAutoWnd   * pAutoWnd = new CAutoWnd(this);
		pAutoWnd->RestoreSettings(g_persistVars.m_auto, m_iNameColWidth);

		if (pAutoWnd->Create(dwStyle, rectNull, this, 1))
		{
			m_arrChildWnd.Add(pAutoWnd);
		}
	}

	CLocalsWnd * pLocalsWnd = new CLocalsWnd(this);
	pLocalsWnd->RestoreSettings(g_persistVars.m_locals, m_iNameColWidth);
													
	if (pLocalsWnd->Create(dwStyle, rectNull, this, 1))
	{
		m_arrChildWnd.Add(pLocalsWnd);
	}

	if (fCreateCPPWindows)
	{
		CThisWnd * pThisWnd = new CThisWnd(this);
		pThisWnd->RestoreSettings(g_persistVars.m_this, m_iNameColWidth);
		
		if (pThisWnd->Create(dwStyle, rectNull, this, 1))
		{
			m_arrChildWnd.Add(pThisWnd);
		}
	}

	m_widgetScroller.SetSplitterPref(g_persistVars.m_nSplitterPref);
	m_curIndex = g_persistVars.GetActiveTabIndex();

	ShowSelection(FALSE); // don't show selections till we get activation.
	OnFontColorChange();
		
	return 0;
}

void CVarsView::OnDestroy()
{
	CBaseClass::OnDestroy();
	
	g_persistVars.m_nSplitterPref = m_widgetScroller.GetSplitterPref();

	m_arrChildWnd[indexLocals]->RememberSettings(g_persistVars.m_locals);

	ASSERT(m_curIndex >= 0);
	g_persistVars.SetActiveTab(m_curIndex);
	g_pVarsView = NULL;
}


void CVarsView::OnToggleVarsBar( )
{
	BOOL fShow = runDebugParams.fVarsToolbar = !runDebugParams.fVarsToolbar;
	
	m_toolbar.ShowWindow(fShow ? SW_SHOWNA : SW_HIDE);
	RecalcLayout();

	// Let the dock frame adjust to the new toolbar state
	// if it needs to.
	DkRecalcBorders(this);		

	if ( m_toolbar.IsChild(CWnd::GetFocus()) )
		SetFocus();
}

void CVarsView::OnUpdateToggleVarsBar ( CCmdUI *pCmdUI )
{
	pCmdUI->SetCheck(runDebugParams.fVarsToolbar);
}


void CVarsView::OnDropDown()
{
	if ( m_toolbar.m_fUpdateListbox )
	{
		m_toolbar.m_list.ResetContent();

		if (!DebuggeeAlive() || DebuggeeRunning())
		{
			MessageBeep(0);
			return;
		}

		CLGetWalkbackStack(hpidCurr, htidCurr, 0);

		int cfme = CLChfmeOnStack();

		for (int ifme = 0; ifme < cfme; ifme++)
		{
			char buf[256];
			CLGetProcName(ifme, buf, sizeof(buf), FALSE);
			m_toolbar.m_list.AddString(buf);
		}
	
		int iCall = (iCallsCxf < 0 ) ? 0 : iCallsCxf;

		m_toolbar.m_list.SetCurSel(iCall);

		m_toolbar.m_fUpdateListbox = FALSE;
	}
	m_toolbar.m_fDoUpdate = TRUE;
}

// a new frame has been selected, change the locals & source being displayed

void CVarsView::OnCloseUp()
{

	int isel = m_toolbar.m_list.GetCurSel();

	if (isel != LB_ERR && m_toolbar.m_fDoUpdate)
	{
		int iCall = (iCallsCxf < 0) ? 0 : iCallsCxf;

		if ( isel != iCallsCxf )
			CLDispCallInfo(isel);
	}
	else
	{
		// if we don't update the callstack at CloseUp make sure the current
		// selection in the combo box is set correctly.
		int iCall = (iCallsCxf < 0)? 0 : iCallsCxf;
		m_toolbar.m_list.SetCurSel(iCall);
	}
}



LRESULT CVarsView::OnUpdateDebug(WPARAM wParam, LPARAM lParam)
{
	// Besides refreshing the grid we also need to update the callstack
	// entries.
	
	// FUTURE: This is the comment from the old locals window.
	// I do not understand this part completely -- sgs --

	// If wParam is zero, then we have a passed in CXT,
	// invalidate the entire client area to force a repaint
	if  (wParam ==wPCXF)  	
	{
		// lParam is zero, then we are updating because memory has been
		// updated, reset the callstack so it will be reset and updated
		// when dropped.
		if (!lParam)
		{
			// Want to make sure that when the listbox is
			// dropped, that it gets reset and updated
			ResetCallStack();
		}
		lParam = NULL;
	}

	UpdateCallStack( );

	return CBaseClass::OnUpdateDebug(wParam, lParam);
}


LRESULT CVarsView::OnFreezeData(WPARAM wParam, LPARAM lParam)
{
	ResetCallStack();

	return 0;
}

			
//////////////////////////////////////////////////////////////////////////////
// Persistence related classes.

IMPLEMENT_SERIAL(CPersistWatchTab, CObject, 1)

CPersistWatchTab::CPersistWatchTab( )
{
	InitDefault();
}

CPersistWatchTab::CPersistWatchTab(const CString& str)
	: m_strTabName(str)
{
	InitDefault();
}

void CPersistWatchTab::InitDefault()
{
	// Place-holder if we need to do any common initializations.
}
		
const CPersistWatchTab& CPersistWatchTab::operator=(const CPersistWatchTab& p)
{
	m_strTabName = p.m_strTabName;

	m_arrWatchNames.RemoveAll();

	for ( int i = 0; i < p.m_arrWatchNames.GetSize() ; i++ )
		m_arrWatchNames[i] = p.m_arrWatchNames[i];

	return *this;
}				

void CPersistWatchTab::ChangeTabName(const CString& str)
{
	m_strTabName = str;
}

void CPersistWatchTab::AddWatchName(CString& str)
{
	m_arrWatchNames.Add(str);
}

void CPersistWatchTab::ClearAllWatches( )
{
	// Will the memory for the strings be reclaimed correctly.
	m_arrWatchNames.RemoveAll( );
}

void CPersistWatchTab::Serialize(CArchive& ar)
{
	CObject::Serialize(ar);
	m_arrWatchNames.Serialize(ar);

	if ( ar.IsStoring( ))
	{
		ar << m_strTabName;
	}
	else
	{
		ar >> m_strTabName;
	}
}

void AFXAPI ConstructElements(CPersistWatchTab *pTab, int nCount)
{
	for ( int i = 0; i < nCount ; i++, pTab++ )
	{
		pTab->CPersistWatchTab::CPersistWatchTab();
	}
}

void AFXAPI DestructElements(CPersistWatchTab *pTab, int nCount)
{
	for ( int i = 0; i < nCount ; i++, pTab++ )
	{
		pTab->CPersistWatchTab::~CPersistWatchTab();
	}
}

void AFXAPI SerializeElements(CArchive& ar, CPersistWatchTab* pTab, int nCount)
{
	for ( int i = 0; i < nCount ; i++, pTab++ )
	{
		pTab->Serialize(ar);
	}
}

IMPLEMENT_SERIAL(CPersistWatch, CObject, 1)

CPersistWatch::CPersistWatch( )
{
	
}

void CPersistWatch::InitDefault()
{
	CString str;

	VERIFY(str.LoadString(IDS_WATCH1));
	CPersistWatchTab watch1(str);
	VERIFY(str.LoadString(IDS_WATCH2));
	CPersistWatchTab watch2(str);
	VERIFY(str.LoadString(IDS_WATCH3));
	CPersistWatchTab watch3(str);
	VERIFY(str.LoadString(IDS_WATCH4));
	CPersistWatchTab watch4(str);

	ClearAll();

	m_arrWatchTab.Add(watch1);
	m_arrWatchTab.Add(watch2);
	m_arrWatchTab.Add(watch3);
	m_arrWatchTab.Add(watch4);
	m_curIndex = 0;
	m_nSplitterPref = 0;
}

void CPersistWatch::AddWatchTab(const CString& str)
{
	CPersistWatchTab watchTab(str);

	m_arrWatchTab.Add(watchTab);
}

void CPersistWatch::AddWatchTab(CPersistWatchTab& rTab)
{
	m_arrWatchTab.Add(rTab);
}

CPersistWatchTab& CPersistWatch::GetWatchTab(int index)
{
	return m_arrWatchTab[index];
}

int CPersistWatch::GetTabCount( )
{
	return m_arrWatchTab.GetSize();
}

void CPersistWatch::ClearAll( )
{
	for ( int i = 0 ; i < m_arrWatchTab.GetSize() ; i++ )
		m_arrWatchTab[i].ClearAllWatches();

	m_arrWatchTab.RemoveAll( );
	m_curIndex = 0;	
}
		
void CPersistWatch::Serialize(CArchive& ar)
{
	CObject::Serialize(ar);

	m_arrWatchTab.Serialize(ar);

	if ( ar.IsStoring() )
	{
		ar << (WORD)m_curIndex;
		ar << (WORD)m_nSplitterPref;
	}
	else
	{
		WORD w;
		ar >> w;
		m_curIndex = w;
		// We saved a 32 bit int as a 16 bit unsigned.
		// This is fine, except that the value "-1" is used
		// as a special flag.  So convert 65535 to "-1" if
		// we hit it.  This preserves backward compat with
		// saved files.
		if (m_curIndex == 0xffff) m_curIndex = -1;
		ar >> w;
		m_nSplitterPref = w;
	}
}


// variables window persistence information.
IMPLEMENT_SERIAL(CPersistVarsTab, CObject, 1)

CPersistVarsTab::CPersistVarsTab( )
{
	InitDefault();
}

void CPersistVarsTab::InitDefault()
{
	// Place-holder for common initializations.
}

void CPersistVarsTab::Serialize(CArchive& ar)
{
	CObject::Serialize(ar);

	if ( ar.IsStoring( ))
	{
	}
	else
	{
	}
}

	
IMPLEMENT_SERIAL(CPersistVariables, CObject, 1)

CPersistVariables::CPersistVariables( )
{
}

void CPersistVariables::InitDefault( )
{
	m_curIndex = 0;
	m_nSplitterPref = 0;
}		


void CPersistVariables::Serialize(CArchive& ar)
{
	CObject::Serialize(ar);

	if ( ar.IsStoring() )
	{
		ar << (WORD)m_curIndex;
		ar << (WORD)m_nSplitterPref;
	}
	else
	{
		WORD w;
		ar >> w;
		m_curIndex = w;
		// We saved a 32 bit int as a 16 bit unsigned.
		// This is fine, except that the value "-1" is used
		// as a special flag.  So convert 65535 to "-1" if
		// we hit it.  This preserves backward compat with
		// saved files.
		if (m_curIndex == 0xffff) m_curIndex = -1;
		ar >> w;
		m_nSplitterPref = w;
	}
}
			
