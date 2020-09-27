// comppage.cpp : implementation file
//

#include "stdafx.h"
#include "bldtools.h"
#include "comppage.h"

#define new DEBUG_NEW

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCompilerDriverPage property page

IMPLEMENT_DYNCREATE(CCompilerDriverPage, CPropertyPage)

CCompilerDriverPage::CCompilerDriverPage(CSettings* pSettings /*= NULL*/) : CPropertyPage(CCompilerDriverPage::IDD)
{
	//{{AFX_DATA_INIT(CCompilerDriverPage)
	m_strCmdLine = _T("");
	m_strFilename = _T("");
	m_strWorkDir = _T("");
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CCompilerDriverPage::~CCompilerDriverPage()
{
}

void CCompilerDriverPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCompilerDriverPage)
	DDX_Text(pDX, IDE_CompilerCmdLine, m_strCmdLine);
	DDX_Text(pDX, IDE_CompilerFilename, m_strFilename);
	DDX_Text(pDX, IDE_CompilerWorkDir, m_strWorkDir);
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCompilerDriverPage, CPropertyPage)
	//{{AFX_MSG_MAP(CCompilerDriverPage)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CCompilerDriverPage message handlers
