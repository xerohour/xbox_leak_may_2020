// Settings.cpp : implementation file
//

#include "stdafx.h"
#include "cafebld.h"
#include "Settings.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSettingsDlg dialog


CSettingsDlg::CSettingsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSettingsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSettingsDlg)
	m_strCAFEDIR = _T("");
	m_strIDESRCDIR = _T("");
	m_strSNIFFDIR = _T("");
	m_strMakefile = _T("");
	m_strPATH = _T("");
	m_strINCLUDE = _T("");
	m_strLIB = _T("");
	m_strParameters = _T("");
	m_strRegistryKey = _T("");
	//}}AFX_DATA_INIT
}


void CSettingsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSettingsDlg)
	DDX_Text(pDX, IDE_EnvCAFEDIR, m_strCAFEDIR);
	DDX_Text(pDX, IDE_EnvIDESRCDIR, m_strIDESRCDIR);
	DDX_Text(pDX, IDE_EnvSNIFFDIR, m_strSNIFFDIR);
	DDX_Text(pDX, IDE_Makefile, m_strMakefile);
	DDX_Text(pDX, IDE_EnvPATH, m_strPATH);
	DDX_Text(pDX, IDE_EnvINCLUDE, m_strINCLUDE);
	DDX_Text(pDX, IDE_EnvLIB, m_strLIB);
	DDX_Text(pDX, IDE_Parameters, m_strParameters);
	DDX_Text(pDX, IDE_RegistryKey, m_strRegistryKey);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSettingsDlg, CDialog)
	//{{AFX_MSG_MAP(CSettingsDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CSettingsDlg message handlers
