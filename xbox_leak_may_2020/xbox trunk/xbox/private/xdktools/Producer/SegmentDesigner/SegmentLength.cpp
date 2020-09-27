// SegmentLength.cpp : implementation file
//

#include "stdafx.h"
#include "SegmentDesignerDll.h"
#include "SegmentLength.h"
#include "SegmentPPGMgr.h"
#include "DMUSProd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define MS_TO_REFTIME (10000i64)
#define SEC_TO_REFTIME (MS_TO_REFTIME * 1000i64)
#define MIN_TO_REFTIME (SEC_TO_REFTIME * 60i64)
#define MAX_REFTIME (SEGMENT_MAX_MEASURES * MIN_TO_REFTIME + 59i64 * SEC_TO_REFTIME + 999i64 * MS_TO_REFTIME)

/////////////////////////////////////////////////////////////////////////////
// CSegmentLength dialog


CSegmentLength::CSegmentLength(CWnd* pParent /*=NULL*/)
	: CDialog(CSegmentLength::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSegmentLength)
	//}}AFX_DATA_INIT
	m_dwLength = 0;
	m_rtLength = 0;
	m_fClockTime = FALSE;
	m_pTimeline = NULL;
	m_dwGroupBits = 0;
	m_dwNbrExtraBars = 0;
	m_fPickupBar = FALSE;
}


void CSegmentLength::DoDataExchange(CDataExchange* pDX)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSegmentLength)
	DDX_Control(pDX, IDC_SPIN_SECOND, m_spinSecond);
	DDX_Control(pDX, IDC_SPIN_MINUTE, m_spinMinute);
	DDX_Control(pDX, IDC_SPIN_MILLISECOND, m_spinMillisecond);
	DDX_Control(pDX, IDC_EDIT_SECOND, m_editSecond);
	DDX_Control(pDX, IDC_EDIT_MINUTE, m_editMinute);
	DDX_Control(pDX, IDC_EDIT_MILLISECOND, m_editMillisecond);
	DDX_Control(pDX, IDC_CHECK_PICKUP, m_checkPickup);
	DDX_Control(pDX, IDC_EXT_LENGTH_SPIN, m_spinExtLength);
	DDX_Control(pDX, IDC_EXT_LENGTH, m_editExtLength);
	DDX_Control(pDX, IDC_EDITLENGTH, m_editLength);
	DDX_Control(pDX, IDC_SPINLENGTH, m_spinLength);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSegmentLength, CDialog)
	//{{AFX_MSG_MAP(CSegmentLength)
	ON_EN_KILLFOCUS(IDC_EDITLENGTH, OnKillfocusEditlength)
	ON_EN_KILLFOCUS(IDC_EXT_LENGTH, OnKillfocusExtLength)
	ON_EN_KILLFOCUS(IDC_EDIT_MILLISECOND, OnKillfocusEditMillisecond)
	ON_EN_KILLFOCUS(IDC_EDIT_MINUTE, OnKillfocusEditMinute)
	ON_EN_KILLFOCUS(IDC_EDIT_SECOND, OnKillfocusEditSecond)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_MILLISECOND, OnDeltaposSpinMillisecond)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_MINUTE, OnDeltaposSpinMinute)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_SECOND, OnDeltaposSpinSecond)
	ON_BN_CLICKED(IDC_RADIO_CLOCKTIME, OnRadioClocktime)
	ON_BN_CLICKED(IDC_RADIO_MEASURES, OnRadioMeasures)
	ON_EN_CHANGE(IDC_EDIT_MILLISECOND, OnChangeEditMillisecond)
	ON_EN_CHANGE(IDC_EDIT_MINUTE, OnChangeEditMinute)
	ON_EN_CHANGE(IDC_EDIT_SECOND, OnChangeEditSecond)
	ON_EN_CHANGE(IDC_EDITLENGTH, OnChangeEditlength)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSegmentLength message handlers

BOOL CSegmentLength::OnInitDialog() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	CDialog::OnInitDialog();

	// Set # of chars in edit control based on largest possible value
	TCHAR tcstrTemp[20];
	_itot( SEGMENT_MAX_MEASURES, tcstrTemp, 10 );
	m_editLength.LimitText(_tcslen(tcstrTemp));
	m_spinLength.SetRange( 1, SEGMENT_MAX_MEASURES );
	m_spinLength.SetPos( m_dwLength );

	// Use the max # of measures as the max # of minutes
	m_editMinute.LimitText(_tcslen(tcstrTemp));
	m_spinMinute.SetRange( 0, SEGMENT_MAX_MEASURES );
	m_editSecond.LimitText( 2 );
	m_spinSecond.SetRange( 0, 59 );
	m_editMillisecond.LimitText( 3 );
	m_spinMillisecond.SetRange( 0, 999 );

	// Update the reference time controls
	UpdateReflengthControls();

	// Set the measures/clocktime radio buttons
	CheckRadioButton( IDC_RADIO_MEASURES, IDC_RADIO_CLOCKTIME, m_fClockTime ? IDC_RADIO_CLOCKTIME : IDC_RADIO_MEASURES );

	// Enable/disable the length controls appropriately
	EnableLengthControls();

	// Set extension length
	_itot( MAX_EXTRA_BARS, tcstrTemp, 10 );
	m_editExtLength.LimitText(_tcslen(tcstrTemp));
	m_spinExtLength.SetRange( MIN_EXTRA_BARS, MAX_EXTRA_BARS );
	if( m_dwNbrExtraBars == 0xFFFFFFFF )
	{
		m_spinExtLength.SetPos( 0 );
		m_editExtLength.SetWindowText( _T("---") );
	}
	else
	{
		m_spinExtLength.SetPos( m_dwNbrExtraBars );
	}

	// Set pick-up checkbox
	m_checkPickup.SetCheck( m_fPickupBar );

	UpdateMeasureConvenienceText();
	UpdateRefTimeConvenienceText();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSegmentLength::OnOK() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	OnKillfocusEditlength();
	OnKillfocusExtLength();
	OnKillfocusEditMillisecond();
	OnKillfocusEditSecond();
	OnKillfocusEditMinute();

	BOOL fTransSucceeded = FALSE;
	DWORD dwNewLength = GetDlgItemInt( IDC_EXT_LENGTH, &fTransSucceeded, FALSE );
	if( fTransSucceeded )
	{
		ASSERT( (dwNewLength >= MIN_EXTRA_BARS) && (dwNewLength <= MAX_EXTRA_BARS) );

		m_dwNbrExtraBars = dwNewLength;
	}

	m_fPickupBar = m_checkPickup.GetCheck();

	m_fClockTime = (GetCheckedRadioButton( IDC_RADIO_MEASURES, IDC_RADIO_CLOCKTIME ) == IDC_RADIO_CLOCKTIME);

	CDialog::OnOK();
}

void CSegmentLength::OnKillfocusEditlength() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	BOOL fTransSucceeded;
	DWORD dwNewLength = GetDlgItemInt( IDC_EDITLENGTH, &fTransSucceeded, FALSE );
	if( !fTransSucceeded )
	{
		dwNewLength = m_dwLength;
	}
	else
	{
		if( dwNewLength < 1)
		{
			dwNewLength = 1;
		}
		else if( dwNewLength > SEGMENT_MAX_MEASURES )
		{
			dwNewLength = SEGMENT_MAX_MEASURES;
		}
	}

	m_spinLength.SetPos( dwNewLength );
	SetDlgItemInt( IDC_EDITLENGTH, dwNewLength, FALSE );

	OnChangeEditlength();
}

void CSegmentLength::OnKillfocusExtLength() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	BOOL fTransSucceeded;
	DWORD dwNewExtLength = GetDlgItemInt( IDC_EXT_LENGTH, &fTransSucceeded, FALSE );
	if( !fTransSucceeded )
	{
		dwNewExtLength = m_dwNbrExtraBars;
	}
	else
	{
		if( dwNewExtLength < MIN_EXTRA_BARS)
		{
			dwNewExtLength = MIN_EXTRA_BARS;
		}
		else if( dwNewExtLength > MAX_EXTRA_BARS )
		{
			dwNewExtLength = MAX_EXTRA_BARS;
		}
	}

	if( dwNewExtLength == 0xFFFFFFFF )
	{
		m_spinExtLength.SetPos( 0 );
		m_editExtLength.SetWindowText( _T("---") );
	}
	else
	{
		m_spinExtLength.SetPos( dwNewExtLength );
		SetDlgItemInt( IDC_EXT_LENGTH, dwNewExtLength, FALSE );
	}
}

BOOL CSegmentLength::OnCommand( WPARAM wParam, LPARAM lParam ) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	const int nCommandID = HIWORD( wParam );

	if( nCommandID == BN_CLICKED
	||  nCommandID == BN_DOUBLECLICKED )
	{
		const int nControlID = LOWORD( wParam );
		if( nControlID == IDC_CHECK_PICKUP )
		{
			if( IsDlgButtonChecked( IDC_CHECK_PICKUP ) )
			{
				CheckDlgButton( IDC_CHECK_PICKUP, 0 );
			}
			else
			{
				CheckDlgButton( IDC_CHECK_PICKUP, 1 );
			}

			return TRUE;
		}
	}
	
	return CDialog::OnCommand( wParam, lParam );
}

void CSegmentLength::OnKillfocusEditMillisecond() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Convert from a number of 100ns to milliseconds
	REFERENCE_TIME rtMillisecondLength = (m_rtLength + MS_TO_REFTIME / 2) / MS_TO_REFTIME;

	BOOL fTransSucceeded;
	DWORD dwNewMillisecond = GetDlgItemInt( IDC_EDIT_MILLISECOND, &fTransSucceeded, FALSE );
	if( !fTransSucceeded )
	{
		// Set to the existing number of milliseconds
		dwNewMillisecond = DWORD(rtMillisecondLength % 1000);
	}

	// Set the edit box to the new value
	SetDlgItemInt( IDC_EDIT_MILLISECOND, dwNewMillisecond, FALSE );

	UpdateReflengthValueAndConvenienceText();
	UpdateReflengthControls();
}

void CSegmentLength::OnKillfocusEditMinute() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Convert from a number of 100ns to minutes
	REFERENCE_TIME rtMinuteLength = (m_rtLength + MS_TO_REFTIME / 2) / MIN_TO_REFTIME;

	BOOL fTransSucceeded;
	DWORD dwNewMinute = GetDlgItemInt( IDC_EDIT_MINUTE, &fTransSucceeded, FALSE );
	if( !fTransSucceeded )
	{
		// Set dwNewMinute to the existing number of minutes
		dwNewMinute = DWORD(rtMinuteLength);
	}
	else
	{
		if( dwNewMinute > SEGMENT_MAX_MEASURES )
		{
			dwNewMinute = SEGMENT_MAX_MEASURES;
		}
	}

	// Set the edit box to the new value
	SetDlgItemInt( IDC_EDIT_MINUTE, dwNewMinute, FALSE );

	UpdateReflengthValueAndConvenienceText();
	UpdateReflengthControls();
}

void CSegmentLength::OnKillfocusEditSecond() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Convert from a number of 100ns to seconds
	REFERENCE_TIME rtSecondLength = (m_rtLength + MS_TO_REFTIME / 2) / SEC_TO_REFTIME;

	BOOL fTransSucceeded;
	DWORD dwNewSecond = GetDlgItemInt( IDC_EDIT_SECOND, &fTransSucceeded, FALSE );
	if( !fTransSucceeded )
	{
		// Set to the existing number of seconds
		dwNewSecond = DWORD(rtSecondLength);
	}

	// Set the edit box to the new value
	SetDlgItemInt( IDC_EDIT_SECOND, dwNewSecond, FALSE );

	UpdateReflengthValueAndConvenienceText();
	UpdateReflengthControls();
}

void CSegmentLength::OnDeltaposSpinMillisecond(NMHDR* pNMHDR, LRESULT* pResult) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

	m_rtLength = max( 0, m_rtLength + pNMUpDown->iDelta * MS_TO_REFTIME);
	m_rtLength = min( MAX_REFTIME, m_rtLength );

	UpdateReflengthControls();
	UpdateRefTimeConvenienceText();

	*pResult = 1;
}

void CSegmentLength::OnDeltaposSpinMinute(NMHDR* pNMHDR, LRESULT* pResult) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

	m_rtLength = max( 0, m_rtLength + pNMUpDown->iDelta * MIN_TO_REFTIME);
	m_rtLength = min( MAX_REFTIME, m_rtLength );

	UpdateReflengthControls();
	UpdateRefTimeConvenienceText();

	*pResult = 1;
}

void CSegmentLength::OnDeltaposSpinSecond(NMHDR* pNMHDR, LRESULT* pResult) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

	m_rtLength = max( 0, m_rtLength + pNMUpDown->iDelta * SEC_TO_REFTIME);
	m_rtLength = min( MAX_REFTIME, m_rtLength );

	UpdateReflengthControls();
	UpdateRefTimeConvenienceText();

	*pResult = 1;
}

void CSegmentLength::UpdateRefTimeConvenienceText()
{
	long lMeasures;
	REFERENCE_TIME rtTemp;
	if( m_pTimeline
	&&	SUCCEEDED( m_pTimeline->RefTimeToMeasureBeat( m_dwGroupBits, 0, m_rtLength, &lMeasures, NULL ) )
	&&	SUCCEEDED( m_pTimeline->MeasureBeatToRefTime( m_dwGroupBits, 0, lMeasures, 0, &rtTemp ) ) )
	{
		if( rtTemp < m_rtLength )
		{
			lMeasures++;
		}

		CString strDialog;
		if( lMeasures > 1 )
		{
			CString strFormat;
			strFormat.LoadString( IDS_MEASURES_TEXT );
			strDialog.Format( strFormat, lMeasures );
		}
		else
		{
			strDialog.LoadString( IDS_MEASURE_TEXT );
		}

		SetDlgItemText( IDC_STATIC_CLOCKTIME_CONVENIENCE, strDialog );
	}
	else
	{
		SetDlgItemText( IDC_STATIC_CLOCKTIME_CONVENIENCE, _T("") );
	}
}

void CSegmentLength::UpdateMeasureConvenienceText()
{
	REFERENCE_TIME rtLength;
	CString strFormat;
	if( m_pTimeline
	&&	SUCCEEDED( m_pTimeline->MeasureBeatToRefTime( m_dwGroupBits, 0, m_dwLength, 0, &rtLength ) )
	&&	strFormat.LoadString( IDS_CLOCKTIME_TEXT ) )
	{
		rtLength = (rtLength + MS_TO_REFTIME / 2) / MS_TO_REFTIME;
		CString strDialog;
		strDialog.Format( strFormat, DWORD(rtLength / 60000), DWORD((rtLength / 1000) % 60), DWORD(rtLength % 1000) );
		SetDlgItemText( IDC_STATIC_MUSICTIME_CONVENIENCE, strDialog );
	}
	else
	{
		SetDlgItemText( IDC_STATIC_MUSICTIME_CONVENIENCE, _T("") );
	}
}

void CSegmentLength::EnableLengthControls()
{
	m_editLength.EnableWindow( !m_fClockTime );
	m_spinLength.EnableWindow( !m_fClockTime );

	m_editMinute.EnableWindow( m_fClockTime );
	m_spinMinute.EnableWindow( m_fClockTime );
	m_editSecond.EnableWindow( m_fClockTime );
	m_spinSecond.EnableWindow( m_fClockTime );
	m_editMillisecond.EnableWindow( m_fClockTime );
	m_spinMillisecond.EnableWindow( m_fClockTime );
}

void CSegmentLength::OnRadioClocktime() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	m_fClockTime = true;
	EnableLengthControls();
}

void CSegmentLength::OnRadioMeasures() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	m_fClockTime = false;
	EnableLengthControls();
}

void CSegmentLength::UpdateReflengthControls()
{
	// Convert from a number of 100ns to milliseconds
	REFERENCE_TIME rtMillisecondLength = (m_rtLength + MS_TO_REFTIME / 2) / MS_TO_REFTIME;

	SetDlgItemInt( IDC_EDIT_MINUTE, DWORD(rtMillisecondLength / 60000), FALSE );
	SetDlgItemInt( IDC_EDIT_SECOND, DWORD((rtMillisecondLength / 1000) % 60), FALSE );
	SetDlgItemInt( IDC_EDIT_MILLISECOND, DWORD(rtMillisecondLength % 1000), FALSE );
}

void CSegmentLength::UpdateReflengthValueAndConvenienceText()
{
	BOOL fTransSucceeded;
	DWORD dwMinute = GetDlgItemInt( IDC_EDIT_MINUTE, &fTransSucceeded, FALSE );
	if( fTransSucceeded )
	{
		DWORD dwSecond = GetDlgItemInt( IDC_EDIT_SECOND, &fTransSucceeded, FALSE );
		if( fTransSucceeded )
		{
			DWORD dwMillisecond = GetDlgItemInt( IDC_EDIT_MILLISECOND, &fTransSucceeded, FALSE );
			if( fTransSucceeded )
			{
				m_rtLength = dwMinute * MIN_TO_REFTIME + dwSecond * SEC_TO_REFTIME + dwMillisecond * MS_TO_REFTIME;
				m_rtLength = min( MAX_REFTIME, m_rtLength );

				UpdateRefTimeConvenienceText();
			}
		}
	}
}

void CSegmentLength::OnChangeEditMillisecond() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	BOOL fTransSucceeded;
	GetDlgItemInt( IDC_EDIT_MILLISECOND, &fTransSucceeded, FALSE );
	if( fTransSucceeded )
	{
		UpdateReflengthValueAndConvenienceText();
	}
}

void CSegmentLength::OnChangeEditMinute() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	BOOL fTransSucceeded;
	GetDlgItemInt( IDC_EDIT_MINUTE, &fTransSucceeded, FALSE );
	if( fTransSucceeded )
	{
		UpdateReflengthValueAndConvenienceText();
	}
}

void CSegmentLength::OnChangeEditSecond() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	BOOL fTransSucceeded;
	GetDlgItemInt( IDC_EDIT_SECOND, &fTransSucceeded, FALSE );
	if( fTransSucceeded )
	{
		UpdateReflengthValueAndConvenienceText();
	}
}

void CSegmentLength::OnChangeEditlength() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	BOOL fTransSucceeded;
	DWORD dwNewLength = GetDlgItemInt( IDC_EDITLENGTH, &fTransSucceeded, FALSE );
	if( fTransSucceeded
	&&	(dwNewLength > 0)
	&&	(dwNewLength <= SEGMENT_MAX_MEASURES) )
	{
		m_dwLength = dwNewLength;

		UpdateMeasureConvenienceText();
	}
}
