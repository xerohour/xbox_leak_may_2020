// InsertSilenceDlg.cpp : implementation file
//

#include "stdafx.h"
#include "dlsdesigner.h"
#include "InsertSilenceDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CInsertSilenceDlg dialog


CInsertSilenceDlg::CInsertSilenceDlg(DWORD dwSampleRate, CWnd* pParent /*=NULL*/)
	: CDialog(CInsertSilenceDlg::IDD, pParent)
{
	m_dwSampleRate = dwSampleRate;
	m_dwSampleLength = dwSampleRate / 2;
	m_dwSecondLength = 0;
	m_dwMillisecondLength = 500;

	//{{AFX_DATA_INIT(CInsertSilenceDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CInsertSilenceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInsertSilenceDlg)
	DDX_Control(pDX, IDC_EDIT_SECONDS, m_editSeconds);
	DDX_Control(pDX, IDC_EDIT_SAMPLES, m_editSamples);
	DDX_Control(pDX, IDC_EDIT_MILLISECONDS, m_editMilliseconds);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CInsertSilenceDlg, CDialog)
	//{{AFX_MSG_MAP(CInsertSilenceDlg)
	ON_EN_KILLFOCUS(IDC_EDIT_SAMPLES, OnKillfocusEditSamples)
	ON_EN_KILLFOCUS(IDC_EDIT_MILLISECONDS, OnKillfocusEditMilliseconds)
	ON_EN_KILLFOCUS(IDC_EDIT_SECONDS, OnKillfocusEditSeconds)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInsertSilenceDlg message handlers

BOOL CInsertSilenceDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_editMilliseconds.LimitText(3);

	SetDlgItemInt( IDC_EDIT_SAMPLES, m_dwSampleLength, FALSE );
	SetDlgItemInt( IDC_EDIT_MILLISECONDS, m_dwMillisecondLength, FALSE );
	SetDlgItemInt( IDC_EDIT_SECONDS, m_dwSecondLength, FALSE );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CInsertSilenceDlg::OnKillfocusEditSamples() 
{
	BOOL fSucceeded = FALSE;
	DWORD dwSamples = GetDlgItemInt( IDC_EDIT_SAMPLES, &fSucceeded, FALSE );
	if( !fSucceeded || dwSamples == m_dwSampleLength )
	{
		SetDlgItemInt( IDC_EDIT_SAMPLES, m_dwSampleLength, FALSE );
	}
	else
	{
		// Ensure we're at least one sample long
		dwSamples = max( dwSamples, 1 );

		m_dwSampleLength = dwSamples;
		m_dwSecondLength = dwSamples / m_dwSampleRate;
		m_dwMillisecondLength = ((dwSamples - m_dwSecondLength * m_dwSampleRate) * 1000 + m_dwSampleRate / 2 ) / m_dwSampleRate;

		SetDlgItemInt( IDC_EDIT_MILLISECONDS, m_dwMillisecondLength, FALSE );
		SetDlgItemInt( IDC_EDIT_SECONDS, m_dwSecondLength, FALSE );
		SetDlgItemInt( IDC_EDIT_SAMPLES, m_dwSampleLength, FALSE );
	}
}

void CInsertSilenceDlg::OnKillfocusEditMilliseconds() 
{
	BOOL fSucceeded = FALSE;
	DWORD dwMilliseconds = GetDlgItemInt( IDC_EDIT_MILLISECONDS, &fSucceeded, FALSE );
	if( !fSucceeded || dwMilliseconds == m_dwMillisecondLength )
	{
		SetDlgItemInt( IDC_EDIT_MILLISECONDS, m_dwMillisecondLength, FALSE );
	}
	else
	{
		// Ensure we're at least one millisecond long
		if( m_dwSecondLength == 0 )
		{
			dwMilliseconds = max( dwMilliseconds, 1 );
		}

		m_dwMillisecondLength = dwMilliseconds;
		m_dwSampleLength = ((m_dwMillisecondLength + m_dwSecondLength * 1000) * m_dwSampleRate + 500) / 1000;

		SetDlgItemInt( IDC_EDIT_MILLISECONDS, m_dwMillisecondLength, FALSE );
		SetDlgItemInt( IDC_EDIT_SECONDS, m_dwSecondLength, FALSE );
		SetDlgItemInt( IDC_EDIT_SAMPLES, m_dwSampleLength, FALSE );
	}
}

void CInsertSilenceDlg::OnKillfocusEditSeconds() 
{
	BOOL fSucceeded = FALSE;
	DWORD dwSeconds = GetDlgItemInt( IDC_EDIT_SECONDS, &fSucceeded, FALSE );
	if( !fSucceeded || dwSeconds == m_dwSecondLength )
	{
		SetDlgItemInt( IDC_EDIT_SECONDS, m_dwSecondLength, FALSE );
	}
	else
	{
		// Ensure we're at least one millisecond long
		if( dwSeconds == 0 && m_dwMillisecondLength == 0 )
		{
			m_dwMillisecondLength = 1;
		}

		m_dwSecondLength = dwSeconds;
		m_dwSampleLength = ((m_dwMillisecondLength + m_dwSecondLength * 1000) * m_dwSampleRate + 500) / 1000;

		SetDlgItemInt( IDC_EDIT_MILLISECONDS, m_dwMillisecondLength, FALSE );
		SetDlgItemInt( IDC_EDIT_SECONDS, m_dwSecondLength, FALSE );
		SetDlgItemInt( IDC_EDIT_SAMPLES, m_dwSampleLength, FALSE );
	}
}
