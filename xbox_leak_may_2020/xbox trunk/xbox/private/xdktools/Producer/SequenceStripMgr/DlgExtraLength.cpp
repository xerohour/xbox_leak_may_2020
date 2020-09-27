// DlgExtraLength.cpp : implementation file
//

#include "stdafx.h"
#include "DlgExtraLength.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgExtraLength dialog


CDlgExtraLength::CDlgExtraLength(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgExtraLength::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgExtraLength)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_dwNbrExtraBars = 0;
	m_fPickupBar = FALSE;
}


void CDlgExtraLength::DoDataExchange(CDataExchange* pDX)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgExtraLength)
	DDX_Control(pDX, IDC_CHECK_PICKUP, m_checkPickup);
	DDX_Control(pDX, IDC_EXT_LENGTH_SPIN, m_spinLength);
	DDX_Control(pDX, IDC_EXT_LENGTH, m_editLength);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgExtraLength, CDialog)
	//{{AFX_MSG_MAP(CDlgExtraLength)
	ON_EN_KILLFOCUS(IDC_EXT_LENGTH, OnKillfocusExtLength)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgExtraLength message handlers

void CDlgExtraLength::OnOK() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	OnKillfocusExtLength();

	// Get the new length
	BOOL fTransSucceeded;
	DWORD dwNewNbrMeasures = GetDlgItemInt( IDC_EXT_LENGTH, &fTransSucceeded, FALSE );
	ASSERT( fTransSucceeded );

	// Validate new extra length
	ASSERT( (dwNewNbrMeasures >= MIN_EXTRA_BARS) && (dwNewNbrMeasures <= MAX_EXTRA_BARS) );

	m_dwNbrExtraBars = dwNewNbrMeasures;

	m_fPickupBar = m_checkPickup.GetCheck();
	
	CDialog::OnOK();
}

BOOL CDlgExtraLength::OnInitDialog() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	CDialog::OnInitDialog();
	
	// Set length
	m_editLength.LimitText( 5 );
	m_spinLength.SetRange( MIN_EXTRA_BARS, MAX_EXTRA_BARS );
	m_spinLength.SetPos( m_dwNbrExtraBars );

	// Set pick-up checkbox
	m_checkPickup.SetCheck( m_fPickupBar );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgExtraLength::OnKillfocusExtLength() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if (m_spinLength.GetSafeHwnd() == NULL)
	{
		return;
	}

	CString strNewNbrMeasures;
	m_editLength.GetWindowText( strNewNbrMeasures );

	// Strip leading and trailing spaces
	strNewNbrMeasures.TrimRight();
	strNewNbrMeasures.TrimLeft();

	// Exit if empty
	if ( strNewNbrMeasures.IsEmpty() )
	{
		m_spinLength.SetPos( MIN_EXTRA_BARS );
		SetDlgItemInt( IDC_EXT_LENGTH, MIN_EXTRA_BARS, FALSE );
		return;
	}

	// Ensure the value stays in range
	BOOL fTransSucceeded;
	int nLength = GetDlgItemInt( IDC_EXT_LENGTH, &fTransSucceeded, FALSE );
	if ( !fTransSucceeded || (nLength < MIN_EXTRA_BARS) )
	{
		m_spinLength.SetPos( MIN_EXTRA_BARS );
		SetDlgItemInt( IDC_EXT_LENGTH, MIN_EXTRA_BARS, FALSE );
	}
	else if( nLength > MAX_EXTRA_BARS )
	{
		m_spinLength.SetPos( MAX_EXTRA_BARS );
		SetDlgItemInt( IDC_EXT_LENGTH, MAX_EXTRA_BARS, FALSE );
	}
}
