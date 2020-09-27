// DlgAltTempo.cpp : implementation file
//

#include "stdafx.h"
#include "cconduct.h"
#include "DlgAltTempo.h"
#include "math.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// DlgAltTempo dialog


DlgAltTempo::DlgAltTempo(CWnd* pParent /*=NULL*/)
	: CDialog(DlgAltTempo::IDD, pParent)
{
	//{{AFX_DATA_INIT(DlgAltTempo)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void DlgAltTempo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(DlgAltTempo)
	DDX_Control(pDX, IDC_SPIN_RATIO, m_spinRatio);
	DDX_Control(pDX, IDC_RATIO_EDIT, m_editRatio);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(DlgAltTempo, CDialog)
	//{{AFX_MSG_MAP(DlgAltTempo)
	ON_EN_KILLFOCUS(IDC_RATIO_EDIT, OnKillfocusRatioEdit)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_RATIO, OnDeltaposSpinRatio)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// DlgAltTempo message handlers

void DlgAltTempo::OnOK() 
{
	// TODO: Add extra validation here
	
	CDialog::OnOK();
}

BOOL DlgAltTempo::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_nRatio = g_pconductor->m_nRatio;

	if (m_editRatio.GetSafeHwnd() != NULL)
	{
		m_editRatio.EnableWindow(TRUE);
		m_editRatio.SetLimitText( 5 );
	}
	if (m_spinRatio.GetSafeHwnd() != NULL)
	{
		m_spinRatio.EnableWindow(TRUE);
		m_spinRatio.SetRange( int(ceil(DMUS_MASTERTEMPO_MIN * 100.0f)), int(DMUS_MASTERTEMPO_MAX * 100) );
		m_spinRatio.SetPos( m_nRatio );
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void DlgAltTempo::OnKillfocusRatioEdit() 
{
	CString strNewRatio;

	m_editRatio.GetWindowText( strNewRatio );

	// Strip leading and trailing spaces
	strNewRatio.TrimRight();
	strNewRatio.TrimLeft();

	if( strNewRatio.IsEmpty() )
	{
		m_spinRatio.SetPos( m_nRatio );
	}
	else
	{
		int iNewRatio = _ttoi( strNewRatio );
		if( iNewRatio > int(DMUS_MASTERTEMPO_MAX * 100) )
		{
			iNewRatio = int(DMUS_MASTERTEMPO_MAX * 100);
			m_spinRatio.SetPos( iNewRatio );
		}
		else if( float(iNewRatio) / 100 < DMUS_MASTERTEMPO_MIN )
		{
			iNewRatio = int(ceil(DMUS_MASTERTEMPO_MIN * 100.0f));
			m_spinRatio.SetPos( iNewRatio );
		}
		if( iNewRatio != m_nRatio )
		{
			m_nRatio = iNewRatio;
		}
	}
}

void DlgAltTempo::OnDeltaposSpinRatio(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

	int iNewRatio = m_spinRatio.GetPos() + pNMUpDown->iDelta;
	if( iNewRatio < int(ceil(DMUS_MASTERTEMPO_MIN * 100.0f)) )
	{
		iNewRatio = int(ceil(DMUS_MASTERTEMPO_MIN * 100.0f));
	}
	else if( iNewRatio > int(DMUS_MASTERTEMPO_MAX * 100) )
	{
		iNewRatio = int(DMUS_MASTERTEMPO_MAX * 100);
	}

	if( iNewRatio != m_nRatio )
	{
		m_nRatio = iNewRatio;
	}
	
	*pResult = 0;
}
