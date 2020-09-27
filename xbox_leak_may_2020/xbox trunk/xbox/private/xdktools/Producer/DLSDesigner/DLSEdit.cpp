// DLSEdit.cpp : implementation file
//

#include "stdafx.h"
#include "dlsdesigner.h"
#include "DLSEdit.h"
#include "InstrumentCtl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDLSEdit

CDLSEdit::CDLSEdit(CInstrumentCtrl* pParent)
{
	m_ParentCtrl = pParent;
}

CDLSEdit::~CDLSEdit()
{
}


BEGIN_MESSAGE_MAP(CDLSEdit, CEdit)
	//{{AFX_MSG_MAP(CDLSEdit)
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_WM_CHAR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDLSEdit message handlers

void CDLSEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	if (nChar == 32) // Space bar
    {
		m_ParentCtrl->OnKeyDown(nChar, nRepCnt, nFlags);
	}	
	else
	{
		CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
	}
}

void CDLSEdit::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	if (nChar == 32) // Space bar
    {
		m_ParentCtrl->OnKeyUp(nChar, nRepCnt, nFlags);
	}	
	else
	{
		CEdit::OnKeyUp(nChar, nRepCnt, nFlags);
	}
}

void CDLSEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if (nChar != 32)
	{
		CEdit::OnChar(nChar, nRepCnt, nFlags);
	}
}
