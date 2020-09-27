// ResampleDlg.cpp : implementation file
//

#include "stdafx.h"
#include "dlsdesigner.h"
#include "ResampleDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CResampleDlg dialog


CResampleDlg::CResampleDlg(DWORD dwSampleRate, CWnd* pParent /*=NULL*/)
	: CDialog(CResampleDlg::IDD, pParent)
{
	m_dwCurrentSampleRate = dwSampleRate;
	m_dwNewSampleRate = dwSampleRate;
	//{{AFX_DATA_INIT(CResampleDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CResampleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CResampleDlg)
	DDX_Control(pDX, IDC_EDIT_NEW_SAMPLERATE, m_editNewSamplerate);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CResampleDlg, CDialog)
	//{{AFX_MSG_MAP(CResampleDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CResampleDlg message handlers

BOOL CResampleDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	SetDlgItemInt( IDC_EDIT_CURRENT_SAMPLERATE, m_dwCurrentSampleRate, FALSE );

	m_editNewSamplerate.LimitText( 5 );
	SetDlgItemInt( IDC_EDIT_NEW_SAMPLERATE, m_dwCurrentSampleRate, FALSE );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CResampleDlg::OnOK() 
{
	BOOL fSucceeded = FALSE;
	DWORD dwNewSampleRate = GetDlgItemInt( IDC_EDIT_NEW_SAMPLERATE, &fSucceeded, FALSE );
	if( !fSucceeded || dwNewSampleRate < 3072 || dwNewSampleRate > 81920 )
	{
		AfxMessageBox(IDS_INVALID_NEW_SAMPLERATE, MB_ICONEXCLAMATION);
		return;
	}

	m_dwNewSampleRate = dwNewSampleRate;

	CDialog::OnOK();
}
