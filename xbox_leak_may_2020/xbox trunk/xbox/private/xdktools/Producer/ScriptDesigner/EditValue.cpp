// EditValue.cpp : implementation file
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
// CEditValue

CEditValue::CEditValue()
{
	m_pScriptDlg = NULL;
}

CEditValue::~CEditValue()
{
}


BEGIN_MESSAGE_MAP(CEditValue, CEdit)
	//{{AFX_MSG_MAP(CEditValue)
	ON_CONTROL_REFLECT(EN_KILLFOCUS, OnKillFocus)
	ON_WM_KEYDOWN()
	ON_WM_CHAR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditValue message handlers

/////////////////////////////////////////////////////////////////////////////
// CEditValue::OnKillFocus

void CEditValue::OnKillFocus() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pScriptDlg != NULL );
	if( m_pScriptDlg )
	{
		m_pScriptDlg->PostMessage( WM_END_EDIT_VALUE, (WPARAM)GetSafeHwnd() );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CEditValue::OnKeyDown

void CEditValue::OnKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pScriptDlg != NULL );

	switch( nChar )
	{
		case VK_RETURN :
			if( nRepCnt == 1 )
			{
				if( m_pScriptDlg )
				{
					m_pScriptDlg->PostMessage( WM_END_EDIT_VALUE, (WPARAM)GetSafeHwnd() );
				}
			}
			return;

		case VK_ESCAPE :
			if( nRepCnt == 1 )
			{
				if( m_pScriptDlg )
				{
					m_pScriptDlg->PostMessage( WM_CANCEL_EDIT_VALUE, (WPARAM)GetSafeHwnd() );
				}
			}
			return;
	}
	
	CEdit::OnKeyDown( nChar, nRepCnt, nFlags );
}


/////////////////////////////////////////////////////////////////////////////
// CEditValue::OnChar

void CEditValue::OnChar( UINT nChar, UINT nRepCnt, UINT nFlags ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	switch( nChar )
	{
		case VK_RETURN:
		case VK_ESCAPE:
			return;
	}
	
	CEdit::OnChar( nChar, nRepCnt, nFlags );
}
