// custabdl.cpp : implementation file
//

#include "stdafx.h"
#include "apiwiz.h"
#include "custpage.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCustomizePage property page

IMPLEMENT_DYNCREATE(CCustomizePage, CPropertyPage)

CCustomizePage::CCustomizePage() : CPropertyPage(CCustomizePage::IDD)
{
	//{{AFX_DATA_INIT(CCustomizePage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CCustomizePage::~CCustomizePage()
{
}

void CCustomizePage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCustomizePage)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCustomizePage, CPropertyPage)
	//{{AFX_MSG_MAP(CCustomizePage)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CCustomizePage message handlers
