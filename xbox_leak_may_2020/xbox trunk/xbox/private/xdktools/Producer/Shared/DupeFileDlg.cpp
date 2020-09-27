// DupeFileDlg.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "DupeFileDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDupeFileDlg dialog


CDupeFileDlg::CDupeFileDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDupeFileDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDupeFileDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDupeFileDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDupeFileDlg)
	DDX_Control(pDX, IDC_OK_ALL, m_btnOKAll);
	DDX_Control(pDX, IDC_DUPE_PROMPT, m_staticPrompt);
	DDX_Control(pDX, IDC_USE_EXISTING, m_radioUseExisting);
	DDX_Control(pDX, IDC_USE_EMBEDDED, m_radioUseEmbedded);
	DDX_Control(pDX, IDC_KEEP_BOTH, m_radioKeepBoth);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDupeFileDlg, CDialog)
	//{{AFX_MSG_MAP(CDupeFileDlg)
	ON_BN_CLICKED(IDC_OK_ALL, OnOKAll)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CDupeFileDlg message handlers

/////////////////////////////////////////////////////////////////////////////
// CDupeFileDlg::OnInitDialog

BOOL CDupeFileDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_staticPrompt.SetWindowText( m_strPrompt );

	m_radioUseExisting.SetWindowText( m_strPromptUseExisting );
	m_radioUseEmbedded.SetWindowText( m_strPromptUseEmbedded );
	m_radioKeepBoth.SetWindowText( m_strPromptKeepBoth );

	m_radioUseExisting.SetCheck( 1 );
	m_radioUseEmbedded.SetCheck( 0 );
	m_radioKeepBoth.SetCheck( 0 );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


/////////////////////////////////////////////////////////////////////////////
// CDupeFileDlg::OnOK

void CDupeFileDlg::OnOK() 
{
	if( m_radioUseExisting.GetCheck() )
	{
		EndDialog( IDC_USE_EXISTING );
	}
	else if( m_radioUseEmbedded.GetCheck() )
	{
		EndDialog( IDC_USE_EMBEDDED );
	}
	else if( m_radioKeepBoth.GetCheck() )
	{
		EndDialog( IDC_KEEP_BOTH );
	}
	else
	{
		// Should not happen!
		ASSERT( 0 );
		CDialog::OnOK();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDupeFileDlg::OnOKAll

void CDupeFileDlg::OnOKAll() 
{
	if( m_radioUseExisting.GetCheck() )
	{
		EndDialog( IDC_USE_EXISTING_ALL_FILES );
	}
	else if( m_radioUseEmbedded.GetCheck() )
	{
		EndDialog( IDC_USE_EMBEDDED_ALL_FILES );
	}
	else if( m_radioKeepBoth.GetCheck() )
	{
		EndDialog( IDC_KEEP_BOTH_ALL_FILES );
	}
	else
	{
		// Should not happen!
		ASSERT( 0 );
		CDialog::OnOK();
	}
}
