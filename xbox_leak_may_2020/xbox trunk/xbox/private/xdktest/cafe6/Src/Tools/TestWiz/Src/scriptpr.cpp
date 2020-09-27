// scriptpr.cpp : implementation file
//

#include "stdafx.h"
#include "testwiz.h"
#include "scriptpr.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// ScriptPropSheet

IMPLEMENT_DYNAMIC(ScriptPropSheet, CPropertySheet)

ScriptPropSheet::ScriptPropSheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
}

ScriptPropSheet::ScriptPropSheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
}

ScriptPropSheet::~ScriptPropSheet()
{
}


BEGIN_MESSAGE_MAP(ScriptPropSheet, CPropertySheet)
	//{{AFX_MSG_MAP(ScriptPropSheet)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// ScriptPropSheet message handlers
