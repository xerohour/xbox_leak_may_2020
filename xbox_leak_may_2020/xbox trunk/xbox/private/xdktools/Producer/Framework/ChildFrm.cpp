// ChildFrm.cpp : implementation of the CChildFrame class
//

#include "stdafx.h"
#include "JazzApp.h"
#include "MainFrm.h"
#include "ComponentCntrItem.h"
#include "ChildFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CChildFrame

IMPLEMENT_DYNCREATE(CChildFrame, CMDIChildWnd)

BEGIN_MESSAGE_MAP(CChildFrame, CMDIChildWnd)
	//{{AFX_MSG_MAP(CChildFrame)
	ON_WM_GETMINMAXINFO()
	ON_WM_MDIACTIVATE()
	ON_WM_NCRBUTTONDOWN()
	ON_WM_MOUSEACTIVATE()
	ON_WM_NCLBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChildFrame construction/destruction

CChildFrame::CChildFrame()
{
	// TODO: add member initialization code here
	
}

CChildFrame::~CChildFrame()
{
}
	
/////////////////////////////////////////////////////////////////////////////
// CChildFrame SetFocusToEditor

void CChildFrame::SetFocusToEditor( void )
{
	// Get the MDI child window's active view
	CComponentView *pView = (CComponentView *)GetActiveView();

	if( pView
	&&  pView->IsKindOf( RUNTIME_CLASS(CComponentView) ) )
	{
		CWnd* pWnd = pView->GetNextWindow( GW_CHILD );
		if( pWnd )
		{
			pWnd = pWnd->GetNextWindow( GW_CHILD );
			if( pWnd )
			{
				CWnd* pWndCtl = pWnd->GetNextDlgTabItem( pWnd );
				if( pWndCtl )
				{
					pWndCtl->SetFocus();
				}
				else
				{
					pWnd->SetFocus();
				}
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CChildFrame PreCreateWindow

BOOL CChildFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CMDIChildWnd::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CChildFrame diagnostics

#ifdef _DEBUG
void CChildFrame::AssertValid() const
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );
	CMDIChildWnd::AssertValid();
}

void CChildFrame::Dump(CDumpContext& dc) const
{
	CMDIChildWnd::Dump(dc);
}

#endif //_DEBUG


/////////////////////////////////////////////////////////////////////////////
// CChildFrame message handlers

/////////////////////////////////////////////////////////////////////////////
// CChildFrame::OnGetMinMaxInfo

void CChildFrame::OnGetMinMaxInfo( MINMAXINFO FAR* lpMMI ) 
{
    lpMMI->ptMinTrackSize.x = 80;
    lpMMI->ptMinTrackSize.y = 40;
	
	CComponentView* pView = (CComponentView *)GetActiveView();

	if( pView
	&&  pView->m_pINode
	&&  pView->m_pEditorCtrl )
	{
		CWnd* pWnd = pView->m_pEditorCtrl->GetInPlaceWindow(); 
		if( pWnd )
		{
			CWnd* pWndChild = pWnd->GetWindow( GW_CHILD );
			if( pWndChild )
			{
				pWndChild->SendMessage( WM_GETMINMAXINFO, 0, (LPARAM)lpMMI );
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CChildFrame::OnMouseActivate

int CChildFrame::OnMouseActivate( CWnd* pDesktopWnd, UINT nHitTest, UINT message ) 
{
	if( nHitTest == HTCLIENT )
	{
		CComponentView* pView = (CComponentView *)GetActiveView();

		if( pView
		&&  pView->m_pINode )
		{
			if( pView->m_pIProject )
			{
				// Set the active Project to the Project containing this View
				theApp.SetActiveProject( (CProject *)pView->m_pIProject );
			}
		}
	}
	
	return CMDIChildWnd::OnMouseActivate( pDesktopWnd, nHitTest, message );
}


/////////////////////////////////////////////////////////////////////////////
// CChildFrame::OnUpdateFrameMenu

void CChildFrame::OnUpdateFrameMenu( BOOL bActivate, CWnd* pActivateWnd, HMENU hMenuAlt )
{
	CMDIFrameWnd* pFrame = GetMDIFrame();

	if (hMenuAlt == NULL && bActivate)
	{
		// attempt to get default menu from document
		CDocument* pDoc = GetActiveDocument();
		if (pDoc != NULL)
			hMenuAlt = pDoc->GetDefaultMenu();
	}

	// use default menu stored in frame if none from document
	if (hMenuAlt == NULL)
		hMenuAlt = m_hMenuShared;

	if (hMenuAlt != NULL && bActivate)
	{
		ASSERT(pActivateWnd == this);

		// activating child, set parent menu
		::SendMessage(pFrame->m_hWndMDIClient, WM_MDISETMENU,
			(WPARAM)hMenuAlt, (LPARAM)pFrame->GetWindowMenuPopup(hMenuAlt));
	}
	else if (hMenuAlt != NULL && !bActivate && pActivateWnd == NULL)
	{
		// destroying last child
		HMENU hMenuLast = NULL;
		::SendMessage(pFrame->m_hWndMDIClient, WM_MDISETMENU,
			(WPARAM)pFrame->m_hMenuDefault, (LPARAM)hMenuLast);
	}
//	else
//	{
//		// refresh MDI Window menu (even if non-shared menu)
//		::SendMessage(pFrame->m_hWndMDIClient, WM_MDIREFRESHMENU, 0, 0);
//	}
}


/////////////////////////////////////////////////////////////////////////////
// CChildFrame::OnMDIActivate

void CChildFrame::OnMDIActivate( BOOL bActivate, CWnd* pActivateWnd, CWnd* pDeactivateWnd ) 
{
	if( bActivate == TRUE )
	{
		CComponentView* pView = (CComponentView *)GetActiveView();

		if( pView )
		{
			if( pView->m_pINode )
			{
				// Highlight the corresponding node in the Project Tree
				theApp.m_pFramework->ShowTreeNode( pView->m_pINode );
			}

			if( pView->m_pIProject )
			{
				// Set the active Project to the Project containing this View
				theApp.SetActiveProject( (CProject *)pView->m_pIProject );
			}
		}
	}

	CMDIChildWnd::OnMDIActivate( bActivate, pActivateWnd, pDeactivateWnd );
	
	if( bActivate == FALSE )
	{
		if( pDeactivateWnd->IsZoomed() )
		{
			CMainFrame* pMainFrame = (CMainFrame *)theApp.m_pMainWnd;
			if( pMainFrame )
			{
				pMainFrame->RecalcLayout( TRUE );
			}
		}
	}

	if( bActivate )
	{
		if( pActivateWnd->IsZoomed() )
		{
			ShowWindow( SW_SHOWNOACTIVATE );
			ShowWindow( SW_SHOWMAXIMIZED );
		}

		SetFocusToEditor();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CChildFrame::OnNcRButtonDown

void CChildFrame::OnNcRButtonDown( UINT nHitTest, CPoint point ) 
{
	CMDIChildWnd::OnNcRButtonDown( nHitTest, point );

	if( (nHitTest == HTCAPTION)
	&&  (GetKeyState(VK_RBUTTON) < 0) )
	{
		CMainFrame* pMainFrame = (CMainFrame *)theApp.m_pMainWnd;
		ASSERT( pMainFrame != NULL );
		CComponentView* pView = (CComponentView *)GetActiveView();

		if( pView
		&&  pView->m_pINode )
		{
			SetFocusToEditor();
			pMainFrame->m_wndTreeBar.RightClickMenu( pView->m_pINode, point );
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CChildFrame::OnNcLButtonDown

void CChildFrame::OnNcLButtonDown( UINT nHitTest, CPoint point ) 
{
	CMDIChildWnd::OnNcLButtonDown( nHitTest, point );

	if( nHitTest == HTCAPTION )
	{
		// User clicked in title bar so display properties for associated node
		CMainFrame* pMainFrame = (CMainFrame *)theApp.m_pMainWnd;
		ASSERT( pMainFrame != NULL );

		CComponentView* pView = (CComponentView *)GetActiveView();

		if( pView
		&&  pView->m_pINode )
		{
			CTreeCtrl* pTreeCtrl = theApp.GetProjectTreeCtrl();
			if( pTreeCtrl )
			{
				IDMUSProdPropSheet* pIPropSheet = theApp.GetPropertySheet();
				if( pIPropSheet )
				{
					if( pIPropSheet->IsShowing() == S_OK )
					{
						BOOL fHaveProperties = FALSE;

						// Get properties for node
						IDMUSProdPropPageObject* pIPageObject;
						if( SUCCEEDED ( pView->m_pINode->QueryInterface( IID_IDMUSProdPropPageObject, (void **)&pIPageObject ) ) )
						{
							fHaveProperties = TRUE;
							if( pIPropSheet->IsEqualPageManagerObject(pIPageObject) != S_OK )
							{
								pIPageObject->OnShowProperties();
							}
							pIPageObject->Release();
						}

						if( fHaveProperties == FALSE )
						{
							// Display 'No Properties'
							if( pMainFrame->m_wndProperties.m_pPropertySheet )
							{
								pMainFrame->m_wndProperties.m_pPropertySheet->RemoveCurrentPageManager();
							}
						}
					}

					pIPropSheet->Release();
				}
			}

			SetFocusToEditor();
		}
	}
}
