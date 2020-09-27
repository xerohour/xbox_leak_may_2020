// comppage.cpp : implementation file
//

#include "stdafx.h"
#include "bldtools.h"
#include "execpage.h"

#define new DEBUG_NEW

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CX86ExecutablePage property page

IMPLEMENT_DYNCREATE(CX86ExecutablePage, CPropertyPage)

CX86ExecutablePage::CX86ExecutablePage(CSettings* pSettings /*= NULL*/) : CPropertyPage(CX86ExecutablePage::IDD)
{
	//{{AFX_DATA_INIT(CX86ExecutablePage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CX86ExecutablePage::~CX86ExecutablePage()
{
}

void CX86ExecutablePage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CX86ExecutablePage)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CX86ExecutablePage, CPropertyPage)
	//{{AFX_MSG_MAP(CX86ExecutablePage)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CX86ExecutablePage message handlers
