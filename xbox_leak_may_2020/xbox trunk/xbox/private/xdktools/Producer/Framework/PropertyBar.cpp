// PropertyBar.cpp : implementation file
//

#include "stdafx.h"
#include "JazzApp.h"
#include "MainFrm.h"
#include "ChildFrm.h"
#include "ComponentCntrItem.h"
#include "PropertyBar.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CPropertyBar construction/destruction

CPropertyBar::CPropertyBar()
{
	m_pPropertySheet = NULL;
}


CPropertyBar::~CPropertyBar()
{
	if( m_pPropertySheet )
	{
		delete m_pPropertySheet;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CPropertyBar message handlers

BEGIN_MESSAGE_MAP(CPropertyBar, CDialogBar)
	//{{AFX_MSG_MAP(CPropertyBar)
	ON_WM_CREATE()
	ON_WM_WINDOWPOSCHANGED()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CPropertyBar::OnCreate

int CPropertyBar::OnCreate( LPCREATESTRUCT lpCreateStruct) 
{
	if( CDialogBar::OnCreate(lpCreateStruct) == -1 )
	{
		return -1;
	}
	
	// Display the context help button on the title bar.
	ModifyStyleEx( 0, WS_EX_CONTEXTHELP | WS_SYSMENU );
	
	return 0;
}


/////////////////////////////////////////////////////////////////////////////
// CPropertyBar::OnCommand

BOOL CPropertyBar::OnCommand( WPARAM wParam, LPARAM lParam ) 
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	if( HIWORD(wParam) == BN_CLICKED )
	{
		HWND hWndParent = ::GetParent( (HWND)lParam );
		if( hWndParent )
		{
			::SendMessage( hWndParent, WM_COMMAND, wParam, lParam );
			return TRUE;
		}
	}

	return CDialogBar::OnCommand(wParam, lParam);
}


/////////////////////////////////////////////////////////////////////////////
// CPropertyBar::OnWindowPosChanged

void CPropertyBar::OnWindowPosChanged( WINDOWPOS FAR* lpwndpos ) 
{
	CDialogBar::OnWindowPosChanged( lpwndpos );

	if( lpwndpos->flags & SWP_HIDEWINDOW )
	{
		CMainFrame* pMainFrame = (CMainFrame *)theApp.m_pMainWnd;
		ASSERT( pMainFrame != NULL );

		// Get the active MDI child window.
		CChildFrame *pMDIChild = (CChildFrame *)pMainFrame->GetActiveFrame();
		if( pMDIChild )
		{
			pMDIChild->SetFocusToEditor();
		}
	}
}
