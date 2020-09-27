// EditSource.cpp : implementation file
//

#include "stdafx.h"
#include "ScriptDesignerDLL.h"
#include "Script.h"
#include "ScriptCtl.h"
#include "ScriptDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEditSource

CEditSource::CEditSource()
{
	m_pScriptDlg = NULL;
	m_dwLastSelection = 0;
	m_fInOnLButtonDown = false;
	m_fSourceChanged = false;
	m_fNeedUndoEntry = false;
}

CEditSource::~CEditSource()
{
}


BEGIN_MESSAGE_MAP(CEditSource, CEdit)
	//{{AFX_MSG_MAP(CEditSource)
	ON_CONTROL_REFLECT(EN_CHANGE, OnChange)
	ON_CONTROL_REFLECT(EN_KILLFOCUS, OnKillFocus)
	ON_WM_LBUTTONDOWN()
	ON_WM_MBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_MBUTTONUP()
	ON_WM_RBUTTONUP()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_CONTROL_REFLECT(EN_SETFOCUS, OnSetFocus)
	ON_WM_CONTEXTMENU()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CEditSource message handlers

/////////////////////////////////////////////////////////////////////////////
// CEditSource::OnChange

void CEditSource::OnChange( void ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pScriptDlg != NULL );
	ASSERT( m_pScriptDlg->m_pScript != NULL );

	m_pScriptDlg->m_pScript->SetModified( TRUE );
	m_fSourceChanged = true;
	m_fNeedUndoEntry = true;
}


/////////////////////////////////////////////////////////////////////////////
// CEditSource::OnKillFocus

void CEditSource::OnKillFocus( void ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pScriptDlg != NULL );
	ASSERT( m_pScriptDlg->m_pScript != NULL );

	// Sync source
	if( m_fSourceChanged )
	{
		m_fSourceChanged = false;
		m_fNeedUndoEntry = false;

		m_pScriptDlg->m_pScript->m_strSource = m_strSourceForUndo; 
		m_pScriptDlg->m_pScript->m_pUndoMgr->SaveState( m_pScriptDlg->m_pScript,
														theApp.m_hInstance,
														IDS_UNDO_SCRIPT_SOURCE );
		
		GetWindowText( m_pScriptDlg->m_pScript->m_strSource );
		m_strSourceForUndo = m_pScriptDlg->m_pScript->m_strSource;

		HWND hwndFocus = ::GetFocus();
		if( hwndFocus
		&&  hwndFocus == m_pScriptDlg->m_btnRefresh.GetSafeHwnd() )
		{
			// Sync will be handled by Refresh button
			return;
		}

		m_pScriptDlg->m_pScript->SyncScriptWithDirectMusic();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CEditSource::SaveUndoState

void CEditSource::SaveUndoState( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pScriptDlg != NULL );
	ASSERT( m_pScriptDlg->m_pScript != NULL );

	if(	m_fNeedUndoEntry )
	{
		m_fNeedUndoEntry = false;

		CString strCurrentSource = m_pScriptDlg->m_pScript->m_strSource;
		m_pScriptDlg->m_pScript->m_strSource = m_strSourceForUndo; 
		m_pScriptDlg->m_pScript->m_pUndoMgr->SaveState( m_pScriptDlg->m_pScript,
														theApp.m_hInstance,
														IDS_UNDO_SCRIPT_SOURCE );
		m_pScriptDlg->m_pScript->m_strSource = strCurrentSource;

		GetWindowText( m_strSourceForUndo );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CEditSource::OnSetFocus

void CEditSource::OnSetFocus() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Store source for undo
	GetWindowText( m_strSourceForUndo );

	// Set selection state if not processing WM_LBUTTONDOWN message
	if( m_fInOnLButtonDown == false )
	{
		SetSel( m_dwLastSelection, FALSE );
	}

	// Save undo state
	SaveUndoState();
}


/////////////////////////////////////////////////////////////////////////////
// CEditSource::OnKeyDown

void CEditSource::OnKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CEdit::OnKeyDown( nChar, nRepCnt, nFlags );

	m_dwLastSelection = GetSel();
	m_pScriptDlg->UpdateStatusBar();

	switch( nChar )
	{
		case VK_RETURN:
			SaveUndoState();
			break;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CEditSource::OnKeyUp

void CEditSource::OnKeyUp( UINT nChar, UINT nRepCnt, UINT nFlags ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CEdit::OnKeyUp( nChar, nRepCnt, nFlags );

	m_dwLastSelection = GetSel();
	m_pScriptDlg->UpdateStatusBar();
}


/////////////////////////////////////////////////////////////////////////////
// CEditSource::OnLButtonDown

void CEditSource::OnLButtonDown( UINT nFlags, CPoint point ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	m_fInOnLButtonDown = true;
	CEdit::OnLButtonDown( nFlags, point );
	m_fInOnLButtonDown = false;

	m_dwLastSelection = GetSel();
	m_pScriptDlg->UpdateStatusBar();
}


/////////////////////////////////////////////////////////////////////////////
// CEditSource::OnMButtonDown

void CEditSource::OnMButtonDown( UINT nFlags, CPoint point ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CEdit::OnMButtonDown( nFlags, point );

	m_dwLastSelection = GetSel();
	m_pScriptDlg->UpdateStatusBar();
}


/////////////////////////////////////////////////////////////////////////////
// CEditSource::OnRButtonDown

void CEditSource::OnRButtonDown( UINT nFlags, CPoint point ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CEdit::OnRButtonDown( nFlags, point );

	m_dwLastSelection = GetSel();
	m_pScriptDlg->UpdateStatusBar();
}


/////////////////////////////////////////////////////////////////////////////
// CEditSource::OnMouseMove

void CEditSource::OnMouseMove( UINT nFlags, CPoint point ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CEdit::OnMouseMove( nFlags, point );

	m_dwLastSelection = GetSel();
	m_pScriptDlg->UpdateStatusBar();
}


/////////////////////////////////////////////////////////////////////////////
// CEditSource::OnLButtonUp

void CEditSource::OnLButtonUp( UINT nFlags, CPoint point ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CEdit::OnLButtonUp( nFlags, point );

	m_dwLastSelection = GetSel();
	m_pScriptDlg->UpdateStatusBar();
}


/////////////////////////////////////////////////////////////////////////////
// CEditSource::OnMButtonUp

void CEditSource::OnMButtonUp( UINT nFlags, CPoint point ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CEdit::OnMButtonUp( nFlags, point );

	m_dwLastSelection = GetSel();
	m_pScriptDlg->UpdateStatusBar();
}


/////////////////////////////////////////////////////////////////////////////
// CEditSource::OnRButtonUp

void CEditSource::OnRButtonUp( UINT nFlags, CPoint point ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CEdit::OnRButtonUp( nFlags, point );

	m_dwLastSelection = GetSel();
	m_pScriptDlg->UpdateStatusBar();
}


/////////////////////////////////////////////////////////////////////////////
// CEditSource::OnContextMenu

void CEditSource::OnContextMenu( CWnd* pWnd, CPoint point ) 
{
	// Do nothing
	// Need to prevent CEdit context menu from appearing
}


/////////////////////////////////////////////////////////////////////////////
// CEditSource::WindowProc

LRESULT CEditSource::WindowProc( UINT message, WPARAM wParam, LPARAM lParam ) 
{
	switch( message )
	{
		case WM_SETTEXT:
		{
			// Set text and top line
			int nFirstLine = GetFirstVisibleLine();
			LRESULT lResult = CEdit::WindowProc( message, wParam, lParam );
			LineScroll( nFirstLine, 0 );

			// Set cursor
			int nLine = LineFromChar( LOWORD(m_dwLastSelection) );
			int nChar = LineIndex( nLine );
			m_dwLastSelection = MAKELONG( nChar, nChar );
			SetSel( m_dwLastSelection, FALSE );
			m_dwLastSelection = GetSel();
			m_pScriptDlg->UpdateStatusBar();
			return lResult;
		}
	}
	
	return CEdit::WindowProc( message, wParam, lParam );
}
