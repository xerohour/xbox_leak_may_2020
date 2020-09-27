// filtrdlg.cpp : implementation file
//

#include "stdafx.h"
#pragma hdrstop

#include "msglog.h"
#include "msgview.h"
#include "msgdoc.h"
#include "findtool.h"
#include "filtrdlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFiltersTabbedDialog

CFiltersTabbedDialog::~CFiltersTabbedDialog()
{
	CMsgDoc::m_iInitialTab = m_nTabCur;
}


BEGIN_MESSAGE_MAP(CFiltersTabbedDialog, CTabbedDialog)
	//{{AFX_MSG_MAP(CFiltersTabbedDialog)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CFiltersTabbedDialog message handlers

//////////////////////////////////////////////////////////////////////////////
