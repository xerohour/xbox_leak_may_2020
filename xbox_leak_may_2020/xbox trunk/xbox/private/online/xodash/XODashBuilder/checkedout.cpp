// CheckedOut.cpp : implementation file
//

#include "stdafx.h"
#include "XODashBuilder.h"
#include "CheckedOut.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern bool CheckReadOnly(char *szFileName);

/////////////////////////////////////////////////////////////////////////////
// CCheckedOut dialog


CCheckedOut::CCheckedOut(CWnd* pParent /*=NULL*/)
	: CDialog(CCheckedOut::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCheckedOut)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CCheckedOut::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCheckedOut)
	DDX_Control(pDX, IDC_LIST1, m_FilesCheckedIn);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCheckedOut, CDialog)
	//{{AFX_MSG_MAP(CCheckedOut)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CCheckedOut::CheckFile(char *szFilePath, char *szFileName)
{
    if(CheckReadOnly(szFilePath))
        m_FilesCheckedIn.AddString(szFileName);
}

/////////////////////////////////////////////////////////////////////////////
// CCheckedOut message handlers

BOOL CCheckedOut::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
    CheckFile("..\\xodashlib\\ButtonName.h", "\\xodashlib\\ButtonName.h");
    CheckFile("..\\xodashlib\\ButtonId.h", "\\xodashlib\\ButtonId.h");
    CheckFile("..\\xodashlib\\ButtonHelp.h", "\\xodashlib\\ButtonHelp.h");
    CheckFile("..\\xodashlib\\SceneName.h", "\\xodashlib\\SceneName.h");
    CheckFile("..\\xodashlib\\SceneId.h", "\\xodashlib\\SceneId.h");
    CheckFile("..\\XODashMain\\NavigationDesc.cpp", "\\XODashMain\\NavigationDesc.cpp");
    CheckFile("..\\XODashMain\\NavigationMap.cpp", "\\XODashMain\\NavigationMap.cpp");

	return TRUE;
}

void CCheckedOut::OnOK() 
{

	CDialog::OnOK();
}
