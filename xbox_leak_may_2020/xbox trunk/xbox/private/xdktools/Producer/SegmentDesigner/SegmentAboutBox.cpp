// SegmentAboutBox.cpp : implementation file
//

#include "stdafx.h"
#include "SegmentDesignerDLL.h"
#include "SegmentAboutBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSegmentAboutBox dialog


CSegmentAboutBox::CSegmentAboutBox(CWnd* pParent /*=NULL*/)
	: CDialog(CSegmentAboutBox::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSegmentAboutBox)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CSegmentAboutBox::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSegmentAboutBox)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSegmentAboutBox, CDialog)
	//{{AFX_MSG_MAP(CSegmentAboutBox)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSegmentAboutBox message handlers

BOOL CSegmentAboutBox::OnInitDialog() 
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

			AfxFormatString1( strFileVersion, IDS_SEGMENT_VERSION_TEXT, achFileVersion );
			SetDlgItemText( IDC_FILE_VERSION, strFileVersion );
		}
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
