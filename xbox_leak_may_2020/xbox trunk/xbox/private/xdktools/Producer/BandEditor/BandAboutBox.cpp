// BandAboutBox.cpp : implementation file
//

#include "stdafx.h"
#include "BandEditorDLL.h"
#include "BandAboutBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBandAboutBox dialog


CBandAboutBox::CBandAboutBox(CWnd* pParent /*=NULL*/)
	: CDialog(CBandAboutBox::IDD, pParent)
{
	//{{AFX_DATA_INIT(CBandAboutBox)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CBandAboutBox::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBandAboutBox)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBandAboutBox, CDialog)
	//{{AFX_MSG_MAP(CBandAboutBox)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBandAboutBox message handlers

BOOL CBandAboutBox::OnInitDialog() 
{
	CDialog::OnInitDialog();

	// Get version information
	TCHAR achJazzExeName[MAX_PATH + 1];
	TCHAR achFileVersion[MAX_PATH];

	if( GetModuleFileName ( theApp.m_hInstance, achJazzExeName, MAX_PATH ) )
	{
		if( theApp.GetFileVersion( achJazzExeName, achFileVersion, MAX_PATH ) )
		{
			CString strFileVersion;

			AfxFormatString1( strFileVersion, IDS_BAND_VERSION_TEXT, achFileVersion );
			SetDlgItemText( IDC_FILE_VERSION, strFileVersion );
		}
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
