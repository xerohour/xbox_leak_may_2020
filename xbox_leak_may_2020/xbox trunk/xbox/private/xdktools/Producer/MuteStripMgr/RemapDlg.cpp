// RemapDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MuteStripMgr.h"
#include "RemapDlg.h"
#include "MuteMgr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRemapDlg dialog


CRemapDlg::CRemapDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRemapDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRemapDlg)
	//}}AFX_DATA_INIT
	m_pIPChannelName = NULL;
}

CRemapDlg::~CRemapDlg()
{
	if( m_pIPChannelName )
	{
		m_pIPChannelName->Release();
		m_pIPChannelName = NULL;
	}
}


void CRemapDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRemapDlg)
	DDX_Control(pDX, IDC_PCHANNEL_NAME, m_ChannelNameEdit);
	DDX_Control(pDX, IDC_PCHANNEL_EDIT, m_ChannelEdit);
	DDX_Control(pDX, IDC_PCHANNEL_SPIN, m_ChannelSpin);
	//}}AFX_DATA_MAP
}


void CRemapDlg::UpdatePChannelName( void )
{
	if( ::IsWindow(m_ChannelSpin.m_hWnd) == FALSE )
	{
		return;
	}

	long lPChannel = m_ChannelSpin.GetPos() - 1;

	// Fill the PChannel name edit box with the PChannel's name
	if( m_pIPChannelName )
	{
		WCHAR wstrName[MAX_PATH];
		if( SUCCEEDED( m_pIPChannelName->GetPChannelName( lPChannel, wstrName) ) )
		{
			m_ChannelNameEdit.SetWindowText( CString( wstrName ) );
		}
	}
}


BEGIN_MESSAGE_MAP(CRemapDlg, CDialog)
	//{{AFX_MSG_MAP(CRemapDlg)
	ON_NOTIFY(UDN_DELTAPOS, IDC_PCHANNEL_SPIN, OnDeltaposPchannelSpin)
	ON_EN_CHANGE(IDC_PCHANNEL_EDIT, OnChangePchannelEdit)
	ON_EN_KILLFOCUS(IDC_PCHANNEL_EDIT, OnKillfocusPchannelEdit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRemapDlg message handlers

BOOL CRemapDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// initialize the dlg strings
	CString str, strFormat;
	if( m_pIPChannelName )
	{
		WCHAR wstrName[MAX_PATH];
		if( SUCCEEDED( m_pIPChannelName->GetPChannelName(m_dwPChannel, wstrName) ) )
		{
			str = wstrName;
		}
		AfxFormatString1(strFormat, IDS_REMAP_PROMPT, str);
	}
	else
	{
		GetDlgItemText(IDC_REMAPTXT, strFormat);
	}
	str.Format(strFormat, m_dwPChannel+1);
	SetDlgItemText(IDC_REMAPTXT, str);

	GetDlgItemText(IDC_MEASUREBEATTXT, strFormat);
	str.Format(strFormat, m_lMeasure+1, m_lBeat+1);
	SetDlgItemText(IDC_MEASUREBEATTXT, str);

	int nMin = (m_dwPChannel == 0) ? 2: 1;
	m_ChannelSpin.SetRange(nMin, MAX_PCHANNEL);
	m_ChannelSpin.SetPos(nMin);

	m_ChannelEdit.SetLimitText( 3 );

	UpdatePChannelName();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CRemapDlg::OnOK() 
{
	m_dwPChannelMap = m_ChannelSpin.GetPos() - 1;

	CDialog::OnOK();
}

void CRemapDlg::OnDeltaposPchannelSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

	//Wrap
	DWORD dwNewPos = pNMUpDown->iPos + pNMUpDown->iDelta;
	if( dwNewPos > MAX_PCHANNEL )
	{
		dwNewPos = 1;
	}
	if( dwNewPos < 1 )
	{
		dwNewPos = MAX_PCHANNEL;
	}

	m_ChannelSpin.SetPos( dwNewPos );
	UpdatePChannelName();

	*pResult = 1;
}

void CRemapDlg::OnChangePchannelEdit() 
{
	// disable OK button if pchannel is same.
	if (m_ChannelSpin.GetSafeHwnd())
	{
		CString str;
		m_ChannelEdit.GetWindowText(str);
		if (str.IsEmpty()) {
			int lower, higher;
			m_ChannelSpin.GetRange(lower, higher);
			str.Format("%d", lower);
			m_ChannelEdit.SetWindowText(str);
			m_ChannelEdit.SetSel(0, 1);
		}
	}

	UpdatePChannelName();
}		

void CRemapDlg::OnKillfocusPchannelEdit() 
{
	if( m_ChannelSpin.GetSafeHwnd() != NULL )
	{
		long l = m_ChannelSpin.GetPos();

		if( (LOWORD(l) < 1) || (HIWORD(l) != 0) )
		{
			m_ChannelSpin.SetPos(1);
		}
	}

	UpdatePChannelName();
}
