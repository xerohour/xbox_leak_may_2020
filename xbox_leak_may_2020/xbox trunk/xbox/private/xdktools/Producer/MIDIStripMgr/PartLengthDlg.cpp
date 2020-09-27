// PartLengthDlg.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "MIDIStripMgr.h"
#include "Pattern.h"
#include "PartLengthDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPartLengthDlg dialog


CPartLengthDlg::CPartLengthDlg( CWnd* pParent /*=NULL*/)
	: CDialog(CPartLengthDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPartLengthDlg)
	//}}AFX_DATA_INIT
	m_wNbrMeasures = 0;
	m_dwNbrExtraBars = 0;
	m_fPickupBar = FALSE;
}


void CPartLengthDlg::DoDataExchange(CDataExchange* pDX)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPartLengthDlg)
	DDX_Control(pDX, IDC_CHECK_PICKUP, m_checkPickup);
	DDX_Control(pDX, IDC_EXT_LENGTH, m_editExtLength);
	DDX_Control(pDX, IDC_EXT_LENGTH_SPIN, m_spinExtLength);
	DDX_Control(pDX, IDC_LENGTH, m_editLength);
	DDX_Control(pDX, IDOK, m_btnOK);
	DDX_Control(pDX, IDC_LENGTH_SPIN, m_spinLength);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPartLengthDlg, CDialog)
	//{{AFX_MSG_MAP(CPartLengthDlg)
	ON_EN_KILLFOCUS(IDC_LENGTH, OnKillfocusLength)
	ON_EN_KILLFOCUS(IDC_EXT_LENGTH, OnKillfocusExtLength)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPartLengthDlg message handlers


/////////////////////////////////////////////////////////////////////////////
// CPartLengthDlg::OnInitDialog

BOOL CPartLengthDlg::OnInitDialog() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	CDialog::OnInitDialog();

	// Set length
	m_editLength.LimitText( 3 );
	m_spinLength.SetRange( MIN_PART_LENGTH, MAX_PART_LENGTH );
	m_spinLength.SetPos( m_wNbrMeasures );

	// Set length
	m_editExtLength.LimitText( 5 );
	m_spinExtLength.SetRange( MIN_EXTRA_BARS, MAX_EXTRA_BARS );
	m_spinExtLength.SetPos( m_dwNbrExtraBars );

	// Set pick-up checkbox
	m_checkPickup.SetCheck( m_fPickupBar );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


/////////////////////////////////////////////////////////////////////////////
// CPartLengthDlg::OnOK

void CPartLengthDlg::OnOK() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	OnKillfocusLength();
	OnKillfocusExtLength();

	// Get the new length
	BOOL fTransSucceeded;
	WORD wNewNbrMeasures = GetDlgItemInt( IDC_LENGTH, &fTransSucceeded, FALSE );
	ASSERT( fTransSucceeded );

	// Validate new length
	ASSERT( (wNewNbrMeasures >= MIN_PART_LENGTH) && (wNewNbrMeasures <= MAX_PART_LENGTH) );

	m_wNbrMeasures = wNewNbrMeasures;

	// Get the new length
	DWORD dwNewNbrMeasures = GetDlgItemInt( IDC_EXT_LENGTH, &fTransSucceeded, FALSE );
	ASSERT( fTransSucceeded );

	// Validate new extra length
	ASSERT( (dwNewNbrMeasures >= MIN_EXTRA_BARS) && (dwNewNbrMeasures <= MAX_EXTRA_BARS) );

	m_dwNbrExtraBars = dwNewNbrMeasures;

	m_fPickupBar = m_checkPickup.GetCheck();

	CDialog::OnOK();
}

void CPartLengthDlg::OnKillfocusLength() 
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
		m_spinLength.SetPos( MIN_PART_LENGTH );
		SetDlgItemInt( IDC_LENGTH, MIN_PART_LENGTH, FALSE );
		return;
	}

	// Ensure the value stays in range
	BOOL fTransSucceeded;
	int nLength = GetDlgItemInt( IDC_LENGTH, &fTransSucceeded, FALSE );
	if ( !fTransSucceeded || (nLength < MIN_PART_LENGTH) )
	{
		m_spinLength.SetPos( MIN_PART_LENGTH );
		SetDlgItemInt( IDC_LENGTH, MIN_PART_LENGTH, FALSE );
	}
	else if( nLength > MAX_PART_LENGTH )
	{
		m_spinLength.SetPos( MAX_PART_LENGTH );
		SetDlgItemInt( IDC_LENGTH, MAX_PART_LENGTH, FALSE );
	}
}

void CPartLengthDlg::OnKillfocusExtLength() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if (m_spinExtLength.GetSafeHwnd() == NULL)
	{
		return;
	}

	CString strNewNbrMeasures;
	m_editExtLength.GetWindowText( strNewNbrMeasures );

	// Strip leading and trailing spaces
	strNewNbrMeasures.TrimRight();
	strNewNbrMeasures.TrimLeft();

	// Exit if empty
	if ( strNewNbrMeasures.IsEmpty() )
	{
		m_spinExtLength.SetPos( MIN_EXTRA_BARS );
		SetDlgItemInt( IDC_EXT_LENGTH, MIN_EXTRA_BARS, FALSE );
		return;
	}

	// Ensure the value stays in range
	BOOL fTransSucceeded;
	int nLength = GetDlgItemInt( IDC_EXT_LENGTH, &fTransSucceeded, FALSE );
	if ( !fTransSucceeded || (nLength < MIN_EXTRA_BARS) )
	{
		m_spinExtLength.SetPos( MIN_EXTRA_BARS );
		SetDlgItemInt( IDC_EXT_LENGTH, MIN_EXTRA_BARS, FALSE );
	}
	else if( nLength > MAX_EXTRA_BARS )
	{
		m_spinExtLength.SetPos( MAX_EXTRA_BARS );
		SetDlgItemInt( IDC_EXT_LENGTH, MAX_EXTRA_BARS, FALSE );
	}
}
