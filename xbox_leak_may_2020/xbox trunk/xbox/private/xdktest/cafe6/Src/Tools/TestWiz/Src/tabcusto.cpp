// tabcusto.cpp : implementation file
//

#include "stdafx.h"
#include "testwiz.h"
#include "tabcusto.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// TabCustomize property page

IMPLEMENT_DYNCREATE(TabCustomize, CPropertyPage)

TabCustomize::TabCustomize() : CPropertyPage(TabCustomize::IDD)
{
	//{{AFX_DATA_INIT(TabCustomize)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

TabCustomize::~TabCustomize()
{
}

void TabCustomize::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(TabCustomize)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(TabCustomize, CPropertyPage)
	//{{AFX_MSG_MAP(TabCustomize)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// TabCustomize message handlers
