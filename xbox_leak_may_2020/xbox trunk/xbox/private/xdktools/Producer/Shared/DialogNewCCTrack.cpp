// DialogNewCCTrack.cpp : implementation file
//

#include "stdafx.h"
#include "DialogNewCCTrack.h"
#include "CurveIO.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define NBR_EXTRA_CCS			5	// PB, AT(M), AT(P), RPN, NRPN (CC's other than 0-127)

/////////////////////////////////////////////////////////////////////////////
// CDialogNewCCTrack dialog


CDialogNewCCTrack::CDialogNewCCTrack(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogNewCCTrack::IDD, pParent)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	//{{AFX_DATA_INIT(CDialogNewCCTrack)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_pfCurveStripExists = NULL;
	m_pVoid = NULL;
	m_bCCType = 0xFF;
	m_wRPNType = 0xFFFF;
}


void CDialogNewCCTrack::DoDataExchange(CDataExchange* pDX)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogNewCCTrack)
	DDX_Control(pDX, IDC_COMBO_CC, m_comboCC);
	DDX_Control(pDX, IDC_STATIC_RPN_DESC, m_staticRPNDesc);
	DDX_Control(pDX, IDC_STATIC_RPN_FRIENDLYTEXT, m_staticRPNFriendlyName);
	DDX_Control(pDX, IDC_STATIC_CCTRACK_EXIST, m_staticRPNExists);
	DDX_Control(pDX, IDC_SPIN_RPN_TYPE, m_spinRPNType);
	DDX_Control(pDX, IDC_EDIT_RPN_TYPE, m_editRPNType);
	DDX_Control(pDX, IDOK, m_btnOK);
	//}}AFX_DATA_MAP
}


void CDialogNewCCTrack::FillComboCC( void )
{
	ASSERT( m_pfCurveStripExists != NULL );

	// Don't redraw until we are finished building the list
	m_comboCC.SetRedraw( FALSE );
	
	// Remove all items
	m_comboCC.ResetContent();

	// Load "create" string
	CString cstrCreate; 
	cstrCreate.LoadString( IDS_CREATE_TEXT );

	CString cstrCCText;
	TCHAR tcstrText[100];
	CString strTemp;

	// #define NBR_EXTRA_CCS = number of CC's added by this for loop
	// Build Control Change list
	for( int i = IDS_PitchBend ;  i <= IDS_NRPN ;  i++ )
	{
		strTemp.LoadString( i );
		AfxExtractSubString( cstrCCText, strTemp, 0, '\n' );

		// Add the 'CC' text
		_tcsncpy( tcstrText,cstrCCText, 99 );
		if( !m_pfCurveStripExists( m_pVoid, BYTE(i - IDS_ControlChange0), 0xFFFF ) )
		{
			// Add the ' - Create' text
			_tcscat( tcstrText, cstrCreate );
		}

		m_comboCC.AddString( tcstrText );
	}

	for( i = IDS_ControlChange0 ;  i <= IDS_ControlChange127 ;  i++ )
	{
		strTemp.LoadString( i );
		AfxExtractSubString( cstrCCText, strTemp, 0, '\n' );

		// Add the 'CC' text
		_tcsncpy( tcstrText,cstrCCText, 99 );

		if( !m_pfCurveStripExists( m_pVoid, BYTE(i - IDS_ControlChange0), 0xFFFF ) )
		{
			// Add the ' - Create' text
			_tcscat( tcstrText, cstrCreate );
		}

		m_comboCC.AddString( tcstrText );
	}

	// Redraw the new list
	m_comboCC.SetRedraw( TRUE );
	m_comboCC.SetCurSel( 0 );
}


BEGIN_MESSAGE_MAP(CDialogNewCCTrack, CDialog)
	//{{AFX_MSG_MAP(CDialogNewCCTrack)
	ON_EN_CHANGE(IDC_EDIT_RPN_TYPE, OnChangeEditRPNType)
	ON_CBN_SELCHANGE(IDC_COMBO_CC, OnSelChangeComboCC)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogNewCCTrack message handlers

BOOL CDialogNewCCTrack::OnInitDialog() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	CDialog::OnInitDialog();

	FillComboCC();

	m_spinRPNType.SetRange( MIN_RPN_TYPE_NUMBER, MAX_RPN_TYPE_NUMBER );
	m_editRPNType.SetLimitText( 5 );
	m_staticRPNDesc.ShowWindow( SW_HIDE );
	m_staticRPNFriendlyName.ShowWindow( SW_HIDE );
	m_staticRPNExists.ShowWindow( SW_HIDE );
	m_spinRPNType.ShowWindow( SW_HIDE );
	m_editRPNType.ShowWindow( SW_HIDE );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDialogNewCCTrack::OnOK() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	int nCurSel = m_comboCC.GetCurSel();

	if( nCurSel == CB_ERR )
	{
		HINSTANCE hInstance = AfxGetResourceHandle();
		AfxSetResourceHandle( AfxGetInstanceHandle() );
		AfxMessageBox( IDS_ERR_CC_TRACK );
		AfxSetResourceHandle( hInstance );

		m_comboCC.SetFocus();
	}
	else
	{
		// Ensure RPN and NRPN number is valid
		if( (nCurSel == 3) || (nCurSel == 4) )
		{
			// Get text from edit control
			TCHAR tcstrTmp[6];
			m_editRPNType.GetWindowText( tcstrTmp, 6 );

			// If empty, exit early
			if( tcstrTmp[0] == NULL )
			{
				HINSTANCE hInstance = AfxGetResourceHandle();
				AfxSetResourceHandle( AfxGetInstanceHandle() );
				AfxMessageBox( IDS_ERR_CC_TRACK );
				AfxSetResourceHandle( hInstance );

				m_editRPNType.SetFocus();
				return;
			}

			// Convert from text to an integer
			long lNewValue = _ttoi( tcstrTmp );

			// Assign the RPN/NRPN type, with a max of MAX_RPN_TYPE_NUMBER and min of MIN_RPN_TYPE_NUMBER
			m_wRPNType = WORD(max(MIN_RPN_TYPE_NUMBER, min(MAX_RPN_TYPE_NUMBER, lNewValue)));
		}

		// PitchBend and Aftertouch (Mono & Polyphonic) appear
		// first in combo list but appear after other CC's based
		// on bCCType value.
		if( nCurSel >= NBR_EXTRA_CCS )
		{
			nCurSel -= NBR_EXTRA_CCS; 
		}
		else
		{
			nCurSel += 128;
		}

		m_bCCType = (BYTE)nCurSel;

		CDialog::OnOK();
	}
}

void CDialogNewCCTrack::OnChangeEditRPNType() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if (!::IsWindow(m_spinRPNType.m_hWnd))
	{
		return;
	}

	// Get text from edit control
	TCHAR tcstrTmp[6];
	m_editRPNType.GetWindowText( tcstrTmp, 6 );

	// Get current CC selection
	int nCurSel = m_comboCC.GetCurSel();

	// If empty, exit early
	if( tcstrTmp[0] == NULL )
	{
		m_staticRPNFriendlyName.ShowWindow( SW_HIDE );
		m_staticRPNExists.ShowWindow( SW_HIDE );

		if( (nCurSel == 3) || (nCurSel == 4) )
		{
			m_btnOK.EnableWindow( FALSE );
		}
		else
		{
			m_btnOK.EnableWindow( TRUE );
		}
		return;
	}

	m_btnOK.EnableWindow( TRUE );

	// Get range
	int nMin;
	int nMax;
	m_spinRPNType.GetRange( nMin, nMax );

	// Convert from text to an integer
	long lNewValue = _ttoi( tcstrTmp );

	// Ensure the value stays within bounds
	if ( lNewValue < nMin )
	{
		lNewValue = nMin;
		m_spinRPNType.SetPos( nMin );
	}
	else if (lNewValue > nMax)
	{
		lNewValue = nMax;
		m_spinRPNType.SetPos( nMax );
	}

	if( (nCurSel == 3) && (lNewValue < 5) )
	{
		CString strTemp;
		if( strTemp.LoadString( lNewValue + IDS_RPN_PITCHBEND ) )
		{
			CString strRPNName;
			AfxExtractSubString( strRPNName, strTemp, 0, '\n' );
			m_staticRPNFriendlyName.SetWindowText( strRPNName );
			m_staticRPNFriendlyName.ShowWindow( SW_NORMAL );
		}
		else
		{
			m_staticRPNFriendlyName.ShowWindow( SW_HIDE );
		}
	}
	else
	{
		m_staticRPNFriendlyName.ShowWindow( SW_HIDE );
	}

	if( (nCurSel == 3) || (nCurSel == 4) )
	{
		// Display whether RPN/NRPN strip is already in use
		if( m_pfCurveStripExists( m_pVoid, BYTE(nCurSel + 0x80), WORD(lNewValue) ) )
		{
			m_staticRPNExists.ShowWindow( SW_NORMAL );
		}
		else
		{
			m_staticRPNExists.ShowWindow( SW_HIDE );
		}
	}
	else
	{
		m_staticRPNExists.ShowWindow( SW_HIDE );
	}
}

void CDialogNewCCTrack::OnSelChangeComboCC() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	int nCurSel = m_comboCC.GetCurSel();

	if( nCurSel != CB_ERR )
	{
		switch( nCurSel )
		{
		case 3: // RPN
		case 4: // NRPN
			m_staticRPNDesc.ShowWindow( SW_NORMAL );
			m_spinRPNType.ShowWindow( SW_NORMAL );
			m_editRPNType.ShowWindow( SW_NORMAL );

			// Update the friendly RPN text
			OnChangeEditRPNType();
			break;
		default:
			m_staticRPNDesc.ShowWindow( SW_HIDE );
			m_staticRPNExists.ShowWindow( SW_HIDE );
			m_staticRPNFriendlyName.ShowWindow( SW_HIDE );
			m_spinRPNType.ShowWindow( SW_HIDE );
			m_editRPNType.ShowWindow( SW_HIDE );
			m_btnOK.EnableWindow( TRUE );
			break;
		}
	}
}
