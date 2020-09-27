// ScriptEdit.cpp : implementation file
//

#include "stdafx.h"
#include "dlsdesigner.h"
#include "ConditionEditor.h"
#include "ScriptEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CScriptEdit

CScriptEdit::CScriptEdit()
{
}

CScriptEdit::~CScriptEdit()
{
}


BEGIN_MESSAGE_MAP(CScriptEdit, CEdit)
	//{{AFX_MSG_MAP(CScriptEdit)
	ON_WM_CONTEXTMENU()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CScriptEdit message handlers

void CScriptEdit::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	ASSERT(m_pConditionEditor);
	if(m_pConditionEditor)
	{
		m_pConditionEditor->ShowPopupMenu(pWnd, point);
	}
	return;	
}

void CScriptEdit::SetEditor(CConditionEditor* pEditor)
{
	ASSERT(pEditor);
	if(pEditor == NULL) 
		return;

	m_pConditionEditor = pEditor;
}