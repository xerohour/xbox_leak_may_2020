// PersonalityAbout.cpp : implementation file
//

#include "stdafx.h"
#pragma warning(disable:4201)
#include "personalitydesigner.h"
#include "PersonalityAbout.h"
#pragma warning(default:4201)

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPersonalityAbout dialog


CPersonalityAbout::CPersonalityAbout(CWnd* pParent /*=NULL*/)
	: CDialog(CPersonalityAbout::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPersonalityAbout)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CPersonalityAbout::DoDataExchange(CDataExchange* pDX)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPersonalityAbout)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPersonalityAbout, CDialog)
	//{{AFX_MSG_MAP(CPersonalityAbout)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPersonalityAbout message handlers

BOOL CPersonalityAbout::OnInitDialog() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CDialog::OnInitDialog();

	// Get version information
	TCHAR achJazzExeName[MAX_PATH + 1];
	TCHAR achFileVersion[MAX_PATH];

	if( GetModuleFileName ( theApp.m_hInstance, achJazzExeName, MAX_PATH ) )
	{
		if( theApp.GetFileVersion( achJazzExeName, achFileVersion, MAX_PATH ) )
		{
			CString strFileVersion;

			AfxFormatString1( strFileVersion, IDS_PERSONALITY_VERSION_TEXT, achFileVersion );
			SetDlgItemText( IDC_FILE_VERSION, strFileVersion );
		}
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
