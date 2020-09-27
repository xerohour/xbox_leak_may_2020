// DebugDlg.cpp : implementation file
//

#include "stdafx.h"
#include "scriptdesignerdll.h"
#include "ScriptComponent.h"
#include "DebugDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDebugListBox

CDebugListBox::CDebugListBox()
{
}

CDebugListBox::~CDebugListBox()
{
}

BEGIN_MESSAGE_MAP(CDebugListBox, CListBox)
	//{{AFX_MSG_MAP(CDebugListBox)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CDebugDlg

CDebugDlg::CDebugDlg()
{
	m_nNextLineNbr = 0;
}

CDebugDlg::~CDebugDlg()
{
}


/////////////////////////////////////////////////////////////////////////////
// CDebugDlg::GetNextLineNbr

int CDebugDlg::GetNextLineNbr( void ) 
{
	return ++m_nNextLineNbr;
}


BEGIN_MESSAGE_MAP(CDebugDlg, CWnd)
	//{{AFX_MSG_MAP(CDebugDlg)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_COMMAND(IDM_CLEAR, OnClear)
	//}}AFX_MSG_MAP

	ON_MESSAGE(WM_DISPLAY_MESSAGE_TEXT, OnDisplayMessageText)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CDebugDlg message handlers

/////////////////////////////////////////////////////////////////////////////
// CDebugDlg::OnCreate

int CDebugDlg::OnCreate( LPCREATESTRUCT lpCreateStruct ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( CWnd::OnCreate(lpCreateStruct) == -1 )
	{
		return -1;
	}

	// Create the listbox
	CRect rect;
	GetClientRect( rect );
	m_lstbxDebugSpew.Create( (WS_VISIBLE | WS_HSCROLL | WS_VSCROLL | LBS_HASSTRINGS | LBS_NOINTEGRALHEIGHT),
							 rect,
							 this,
							 IDC_LSTBX_DEBUG_SPEW );

	// Attach the menu 
	HMENU hMenu = ::LoadMenu( theApp.m_hInstance, MAKEINTRESOURCE(IDM_MESSAGE_WINDOW) );
	::SetMenu( GetSafeHwnd(), hMenu );

	// Start displaying MessageText
	theApp.m_pScriptComponent->m_fDisplayMessageText = TRUE;

	return 0;
}


/////////////////////////////////////////////////////////////////////////////
// CDebugDlg::OnDestroy

void CDebugDlg::OnDestroy( void ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Stop displaying MessageText
	::EnterCriticalSection( &theApp.m_pScriptComponent->m_CrSecDebugDlg );
	theApp.m_pScriptComponent->m_fDisplayMessageText = FALSE;
	::LeaveCriticalSection( &theApp.m_pScriptComponent->m_CrSecDebugDlg );

	// Need to cleanup WM_DISPLAY_MESSAGE_TEXT messages
	MSG msg;
	while( ::PeekMessage( &msg, GetSafeHwnd(), WM_DISPLAY_MESSAGE_TEXT, WM_DISPLAY_MESSAGE_TEXT, PM_REMOVE) )
	{
		OnDisplayMessageText( msg.wParam, msg.lParam );
	}

	// Save window placement
	ASSERT( theApp.m_pScriptComponent->m_pDebugDlg != NULL );
	if( theApp.m_pScriptComponent->m_pDebugDlgWP == NULL )
	{
		theApp.m_pScriptComponent->m_pDebugDlgWP = new WINDOWPLACEMENT;
	}
	if( theApp.m_pScriptComponent->m_pDebugDlgWP )
	{
		theApp.m_pScriptComponent->m_pDebugDlgWP->length = sizeof(WINDOWPLACEMENT);
		if( GetWindowPlacement( theApp.m_pScriptComponent->m_pDebugDlgWP ) == 0 )
		{
			delete theApp.m_pScriptComponent->m_pDebugDlgWP;
			theApp.m_pScriptComponent->m_pDebugDlgWP = NULL;
		}
	}

	// Destroy the menu
	HMENU hMenu = ::GetMenu( GetSafeHwnd() );
	::SetMenu( GetSafeHwnd(), NULL );
	if( hMenu )
	{
		::DestroyMenu( hMenu );
	}

	// Destroy the icon
	HICON hIcon = GetIcon( TRUE );
	SetIcon( NULL, TRUE );
	if( hIcon )
	{
		::DestroyIcon( hIcon );
	}

	CWnd::OnDestroy();
}


/////////////////////////////////////////////////////////////////////////////
// CDebugDlg::OnSize

void CDebugDlg::OnSize( UINT nType, int cx, int cy ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CWnd::OnSize( nType, cx, cy );

	// Resize listbox
	if( ::IsWindow(m_lstbxDebugSpew) )
	{
		m_lstbxDebugSpew.MoveWindow( 0, 0, cx, cy, TRUE );
		m_lstbxDebugSpew.Invalidate();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDebugDlg::PostNcDestroy

void CDebugDlg::PostNcDestroy( void ) 
{
	// Free resources
	ASSERT( theApp.m_pScriptComponent->m_pDebugDlg != NULL );
	if( theApp.m_pScriptComponent->m_pDebugDlg )
	{
		delete theApp.m_pScriptComponent->m_pDebugDlg;
		theApp.m_pScriptComponent->m_pDebugDlg = NULL;
	}
	
	CWnd::PostNcDestroy();
}


/////////////////////////////////////////////////////////////////////////////
// CDebugDlg::OnClear

void CDebugDlg::OnClear( void ) 
{
	m_lstbxDebugSpew.ResetContent();
	m_nNextLineNbr = 0;
}


/////////////////////////////////////////////////////////////////////////////
// CDebugDlg::OnDisplayMessageText

LRESULT CDebugDlg::OnDisplayMessageText( WPARAM wParam, LPARAM lParam )
{
	ASSERT( lParam != NULL );

	CSize sizeText; 

	m_lstbxDebugSpew.SetRedraw( FALSE );

	CDC* pDC = m_lstbxDebugSpew.GetDC();
	if( pDC )
	{
		CTypedPtrList<CPtrList, CString*>* plstText = (CTypedPtrList<CPtrList, CString*> *)lParam;

		while( !plstText->IsEmpty() )
		{
			CString* pstrMessageText = static_cast<CString*>( plstText->RemoveHead() );

			if( theApp.m_pScriptComponent->m_fDisplayMessageText )
			{
				// Add string to listbox
				int nPos = m_lstbxDebugSpew.AddString( *pstrMessageText );
				m_lstbxDebugSpew.SetCurSel( nPos );

				// Set horizontal extent
				sizeText = pDC->GetTextExtent( *pstrMessageText );
				if( sizeText.cx > m_lstbxDebugSpew.GetHorizontalExtent() )
				{
					m_lstbxDebugSpew.SetHorizontalExtent( sizeText.cx );
				}
			}

			delete pstrMessageText;
		}

		m_lstbxDebugSpew.ReleaseDC( pDC );
	}

	m_lstbxDebugSpew.SetRedraw( TRUE );

	return 0;
}
