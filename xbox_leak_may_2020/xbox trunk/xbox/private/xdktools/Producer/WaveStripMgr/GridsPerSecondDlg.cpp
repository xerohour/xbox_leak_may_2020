// GridsPerSecondDlg.cpp : implementation file
//

#include "stdafx.h"
#include <RiffStrm.h>
#include "TrackMgr.h"
#include "GridsPerSecondDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGridsPerSecondDlg dialog


CGridsPerSecondDlg::CGridsPerSecondDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGridsPerSecondDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CGridsPerSecondDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_nSnapToRefTime_GridsPerSecond = 0;
	m_fAllParts = FALSE;
}


void CGridsPerSecondDlg::DoDataExchange(CDataExchange* pDX)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGridsPerSecondDlg)
	DDX_Control(pDX, IDC_NBR_MS, m_staticNbrMS);
	DDX_Control(pDX, IDOK, m_btnOK);
	DDX_Control(pDX, IDC_SPIN_NBR_GRIDS, m_spinNbrGrids);
	DDX_Control(pDX, IDC_EDIT_NBR_GRIDS, m_editNbrGrids);
	DDX_Control(pDX, IDC_RADIO_ACTIVE_PART, m_radioActivePart);
	DDX_Control(pDX, IDC_RADIO_ALL_PARTS, m_radioAllParts);
	//}}AFX_DATA_MAP
}


/////////////////////////////////////////////////////////////////////////////
// CGridsPerSecondDlg::SetNbrMsPerGridText

void CGridsPerSecondDlg::SetNbrMsPerGridText( void ) 
{
	int nGridsPerSecond = m_spinNbrGrids.GetPos();
	ASSERT( nGridsPerSecond >= MIN_NBR_GRIDS_PER_SECOND );
	ASSERT( nGridsPerSecond <= MAX_NBR_GRIDS_PER_SECOND );

	CString strNbr;
	if( 1000 % nGridsPerSecond )
	{
		strNbr.Format( "%.2f", ((float)1000 / nGridsPerSecond) );
	}
	else
	{
		strNbr.Format( "%d", (1000 / nGridsPerSecond) );
	}

	CString strNbrMS;
	AfxFormatString1( strNbrMS, IDS_NBR_MS_PER_GRID_TEXT, strNbr );

	m_staticNbrMS.SetWindowText( strNbrMS );
}


BEGIN_MESSAGE_MAP(CGridsPerSecondDlg, CDialog)
	//{{AFX_MSG_MAP(CGridsPerSecondDlg)
	ON_EN_KILLFOCUS(IDC_EDIT_NBR_GRIDS, OnKillFocusEditNbrGrids)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_NBR_GRIDS, OnDeltaPosSpinNbrGrids)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGridsPerSecondDlg message handlers

/////////////////////////////////////////////////////////////////////////////
// CGridsPerSecondDlg::OnInitDialog

BOOL CGridsPerSecondDlg::OnInitDialog() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	CDialog::OnInitDialog();
	
	// Limit the number of grids
	m_spinNbrGrids.SetRange( MIN_NBR_GRIDS_PER_SECOND, MAX_NBR_GRIDS_PER_SECOND );
	m_editNbrGrids.LimitText( 3 );

	// Set number of grids
	ASSERT( m_nSnapToRefTime_GridsPerSecond != 0 );
	m_spinNbrGrids.SetPos( m_nSnapToRefTime_GridsPerSecond );
	SetNbrMsPerGridText();

	// Set radio buttons
	m_radioActivePart.SetCheck( 1 );
	m_radioAllParts.SetCheck( 0 );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


/////////////////////////////////////////////////////////////////////////////
// CGridsPerSecondDlg::OnKillFocusEditNbrGrids

void CGridsPerSecondDlg::OnKillFocusEditNbrGrids( void ) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	CString strNewNbrGrids;

	m_editNbrGrids.GetWindowText( strNewNbrGrids );

	// Strip leading and trailing spaces
	strNewNbrGrids.TrimRight();
	strNewNbrGrids.TrimLeft();

	if( strNewNbrGrids.IsEmpty() )
	{
		m_spinNbrGrids.SetPos( m_nSnapToRefTime_GridsPerSecond );
	}
	else
	{
		short nNewNbrGrids = (short)_ttoi( strNewNbrGrids );

		// Check bounds
		if( nNewNbrGrids > MAX_NBR_GRIDS_PER_SECOND 
		||  nNewNbrGrids < MIN_NBR_GRIDS_PER_SECOND )
		{
			AfxMessageBox( IDS_ERR_NBR_GRIDS_PER_SECOND );
			m_spinNbrGrids.SetPos( m_nSnapToRefTime_GridsPerSecond );
			m_editNbrGrids.SetFocus();
			m_editNbrGrids.SetSel( 0, -1 );
		}
		else
		{
			m_spinNbrGrids.SetPos( nNewNbrGrids );
		}
	}

	SetNbrMsPerGridText();
}


/////////////////////////////////////////////////////////////////////////////
// CGridsPerSecondDlg::OnDeltaPosSpinNbrGrids

void CGridsPerSecondDlg::OnDeltaPosSpinNbrGrids( NMHDR* pNMHDR, LRESULT* pResult ) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Need to do this in case the user clicked the spin control immediately after
	// typing in a value
	OnKillFocusEditNbrGrids();

	// If the value changed - update the selected item
	if( ((NM_UPDOWN* )pNMHDR)->iDelta != 0 ) 
	{
		// Get the valid range
		int iLower, iUpper;
		m_spinNbrGrids.GetRange( iLower, iUpper );

		// Get current posiion
		int iPos = m_spinNbrGrids.GetPos();

		// Check ranges
		iPos += ((NM_UPDOWN* )pNMHDR)->iDelta;
		if( iPos > iUpper )
		{
			iPos = iUpper;
		}
		if( iPos < iLower )
		{
			iPos = iLower;
		}

		m_spinNbrGrids.SetPos( iPos );
	}

	SetNbrMsPerGridText();

	// Set the result to 1 to show that we handled this message
	*pResult = 1;
}


/////////////////////////////////////////////////////////////////////////////
// CGridsPerSecondDlg::OnOK

void CGridsPerSecondDlg::OnOK( void ) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if(	m_radioAllParts.GetCheck() )
	{
		m_fAllParts = TRUE;
	}

	m_nSnapToRefTime_GridsPerSecond = (short)m_spinNbrGrids.GetPos();
	ASSERT( m_nSnapToRefTime_GridsPerSecond >= MIN_NBR_GRIDS_PER_SECOND );
	ASSERT( m_nSnapToRefTime_GridsPerSecond <= MAX_NBR_GRIDS_PER_SECOND );

	CDialog::OnOK();
}
