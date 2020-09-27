// DialogVelocity.cpp : implementation file
//

#include "stdafx.h"
#include "DialogVelocity.h"
#include "Windowsx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// Custom edit control that only accepts numbers, editing keys, and '-'
IMPLEMENT_DYNCREATE( CVelocityEdit, CEdit )

CVelocityEdit::CVelocityEdit() : CEdit()
{
}

LRESULT CVelocityEdit::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch( message )
	{
	case WM_CHAR:
		switch( wParam )
		{
		case 8:  // Backspace
		case '-':
		case '+':
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
		case 71: // Home
		case 75: // Left
		case 77: // Right
		case 79: // End
		case 82: // Ins
		case 83: // Del
			break;
		default:
			return TRUE;
		break;
		}
	}
	return CEdit::WindowProc( message, wParam, lParam );
}

/////////////////////////////////////////////////////////////////////////////
// CDialogVelocity dialog


CDialogVelocity::CDialogVelocity(CWnd* pParent /*=NULL*/)
	: CDialog(CDialogVelocity::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialogVelocity)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_fEnableSelected = false;
	m_fEndTracksStart = true;

	m_vtTarget = VELOCITY_TARGET_PART;
	m_fAbsolute = true;
	m_fPercent = true;
	m_lAbsoluteChangeStart = 0;
	m_lAbsoluteChangeEnd = 0;
	m_bCompressMin = 1;
	m_bCompressMax = 127;
}


void CDialogVelocity::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogVelocity)
	DDX_Control(pDX, IDC_SPIN_STRENGTH_END, m_spinStrengthEnd);
	DDX_Control(pDX, IDC_EDIT_STRENGTH_END, m_editStrengthEnd);
	DDX_Control(pDX, IDC_SPIN_STRENGTH, m_spinStrength);
	DDX_Control(pDX, IDC_SPIN_MIN, m_spinMin);
	DDX_Control(pDX, IDC_SPIN_MAX, m_spinMax);
	DDX_Control(pDX, IDC_EDIT_STRENGTH, m_editStrength);
	DDX_Control(pDX, IDC_EDIT_MIN, m_editMin);
	DDX_Control(pDX, IDC_EDIT_MAX, m_editMax);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogVelocity, CDialog)
	//{{AFX_MSG_MAP(CDialogVelocity)
	ON_BN_CLICKED(IDC_RADIO_ABSOLUTE, OnRadioAbsolute)
	ON_BN_CLICKED(IDC_RADIO_COMPRESS, OnRadioCompress)
	ON_BN_CLICKED(IDC_RADIO_LINEAR, OnRadioLinear)
	ON_BN_CLICKED(IDC_RADIO_PERCENT, OnRadioPercent)
	ON_EN_KILLFOCUS(IDC_EDIT_MAX, OnKillfocusEditMax)
	ON_EN_KILLFOCUS(IDC_EDIT_MIN, OnKillfocusEditMin)
	ON_EN_KILLFOCUS(IDC_EDIT_STRENGTH, OnKillfocusEditStrength)
	ON_EN_KILLFOCUS(IDC_EDIT_STRENGTH_END, OnKillfocusEditStrengthEnd)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogVelocity message handlers

void SetAbsEditText( CVelocityEdit &editCtrl, CSpinButtonCtrl &spinCtrl, int nValue )
{
	// Set the spin control
	spinCtrl.SetPos( nValue );

	// Get the text string for the number (including the +/- sign)
	TCHAR tcstrText[20];
	_stprintf( tcstrText, _T("%+d"), nValue );

	// Set the edit control's text
	editCtrl.SetWindowText( tcstrText );
}

BOOL CDialogVelocity::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// Enable/Disable the Selected Notes radio button
	EnableDlgItem( IDC_RADIO_SELECTED_NOTES, m_fEnableSelected );

	// Set the Velocity Target radio button
	switch( m_vtTarget )
	{
	case VELOCITY_TARGET_SELECTED:
		ASSERT( m_fEnableSelected );
		CheckRadioButton( IDC_RADIO_SELECTED_NOTES, IDC_RADIO_PATTERN, IDC_RADIO_SELECTED_NOTES );
		break;
	case VELOCITY_TARGET_PART:
		CheckRadioButton( IDC_RADIO_SELECTED_NOTES, IDC_RADIO_PATTERN, IDC_RADIO_PART );
		break;
	case VELOCITY_TARGET_PATTERN:
		CheckRadioButton( IDC_RADIO_SELECTED_NOTES, IDC_RADIO_PATTERN, IDC_RADIO_PATTERN );
		break;
	default:
		break;
	}

	// Initialize min/max edit boxes
	m_editMin.SetLimitText( 3 );
	m_editMax.SetLimitText( 3 );
	m_spinMin.SetRange( 1, 127 );
	m_spinMax.SetRange( 1, 127 );
	m_spinMin.SetPos( m_bCompressMin );
	m_spinMax.SetPos( m_bCompressMax );

	// Initialize Strength edit box
	if( m_fPercent )
	{
		m_editStrength.SetLimitText( 4 ); // +200
		m_editStrengthEnd.SetLimitText( 4 );
		m_spinStrength.SetRange32( -99, 200 );
		m_spinStrengthEnd.SetRange32( -99, 200 );
	}
	else
	{
		m_editStrength.SetLimitText( 4 ); // +127
		m_editStrengthEnd.SetLimitText( 4 );
		m_spinStrength.SetRange32( -126, 127 );
		m_spinStrengthEnd.SetRange32( -126, 127 );
	}
	SetAbsEditText( m_editStrength, m_spinStrength, m_lAbsoluteChangeStart );
	SetAbsEditText( m_editStrengthEnd, m_spinStrengthEnd, m_lAbsoluteChangeEnd );
	CheckRadioButton( IDC_RADIO_PERCENT, IDC_RADIO_LINEAR, m_fPercent ? IDC_RADIO_PERCENT : IDC_RADIO_LINEAR );

	// Initialize whether the end value tracks the start value
	m_fEndTracksStart = (m_lAbsoluteChangeStart == m_lAbsoluteChangeEnd);

	// Initialize the absolute/compress radio buttons
	CheckRadioButton( IDC_RADIO_ABSOLUTE, IDC_RADIO_COMPRESS, m_fAbsolute ? IDC_RADIO_ABSOLUTE : IDC_RADIO_COMPRESS );

	// Enable/Disable the controls appropriately
	m_editMin.EnableWindow( m_fAbsolute ? FALSE : TRUE );
	m_editMax.EnableWindow( m_fAbsolute ? FALSE : TRUE );
	m_spinMin.EnableWindow( m_fAbsolute ? FALSE : TRUE );
	m_spinMax.EnableWindow( m_fAbsolute ? FALSE : TRUE );
	EnableDlgItem( IDC_STATIC_MIN, m_fAbsolute ? FALSE : TRUE );
	EnableDlgItem( IDC_STATIC_MAX, m_fAbsolute ? FALSE : TRUE );

	m_editStrength.EnableWindow( m_fAbsolute ? TRUE : FALSE );
	m_spinStrength.EnableWindow( m_fAbsolute ? TRUE : FALSE );
	m_editStrengthEnd.EnableWindow( m_fAbsolute ? TRUE : FALSE );
	m_spinStrengthEnd.EnableWindow( m_fAbsolute ? TRUE : FALSE );
	EnableDlgItem( IDC_RADIO_PERCENT, m_fAbsolute ? TRUE : FALSE );
	EnableDlgItem( IDC_RADIO_LINEAR, m_fAbsolute ? TRUE : FALSE );
	EnableDlgItem( IDC_STATIC_START, m_fAbsolute ? TRUE : FALSE );
	EnableDlgItem( IDC_STATIC_END, m_fAbsolute ? TRUE : FALSE );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDialogVelocity::OnRadioAbsolute() 
{
	// Enable/Disable the controls appropriately
	m_editMin.EnableWindow( FALSE );
	m_editMax.EnableWindow( FALSE );
	m_spinMin.EnableWindow( FALSE );
	m_spinMax.EnableWindow( FALSE );
	EnableDlgItem( IDC_STATIC_MIN, FALSE );
	EnableDlgItem( IDC_STATIC_MAX, FALSE );

	m_editStrength.EnableWindow( TRUE );
	m_spinStrength.EnableWindow( TRUE );
	m_editStrengthEnd.EnableWindow( TRUE );
	m_spinStrengthEnd.EnableWindow( TRUE );
	EnableDlgItem( IDC_RADIO_PERCENT, TRUE );
	EnableDlgItem( IDC_RADIO_LINEAR, TRUE );
	EnableDlgItem( IDC_STATIC_START, TRUE );
	EnableDlgItem( IDC_STATIC_END, TRUE );
}

void CDialogVelocity::OnRadioCompress() 
{
	// Enable/Disable the controls appropriately
	m_editMin.EnableWindow( TRUE );
	m_editMax.EnableWindow( TRUE );
	m_spinMin.EnableWindow( TRUE );
	m_spinMax.EnableWindow( TRUE );
	EnableDlgItem( IDC_STATIC_MIN, TRUE );
	EnableDlgItem( IDC_STATIC_MAX, TRUE );

	m_editStrength.EnableWindow( FALSE );
	m_spinStrength.EnableWindow( FALSE );
	m_editStrengthEnd.EnableWindow( FALSE );
	m_spinStrengthEnd.EnableWindow( FALSE );
	EnableDlgItem( IDC_RADIO_PERCENT, FALSE );
	EnableDlgItem( IDC_RADIO_LINEAR, FALSE );
	EnableDlgItem( IDC_STATIC_START, FALSE );
	EnableDlgItem( IDC_STATIC_END, FALSE );
}

void CDialogVelocity::OnRadioLinear() 
{
	// Initialize Strength edit box
	m_editStrength.SetLimitText( 4 ); // +127
	m_spinStrength.SetRange32( -126, 127 );
	m_editStrengthEnd.SetLimitText( 4 );
	m_spinStrengthEnd.SetRange32( -126, 127 );

	OnKillfocusEditStrength();
	OnKillfocusEditStrengthEnd();
}

void CDialogVelocity::OnRadioPercent() 
{
	// Initialize Strength edit box
	m_editStrength.SetLimitText( 4 ); // +200
	m_spinStrength.SetRange32( -99, 200 );
	m_editStrengthEnd.SetLimitText( 4 );
	m_spinStrengthEnd.SetRange32( -99, 200 );

	OnKillfocusEditStrength();
	OnKillfocusEditStrengthEnd();
}

void CDialogVelocity::OnKillfocusEditMax() 
{
	BOOL fTransSucceeded;
	int nValue = GetDlgItemInt( IDC_EDIT_MAX, &fTransSucceeded, FALSE );
	if ( !fTransSucceeded || (nValue > 127) )
	{
		m_spinMax.SetPos( 127 );
		nValue = 127;
	}
	else if( nValue < 1 )
	{
		m_spinMax.SetPos( 1 );
		m_spinMin.SetPos( 1 );
	}
	else if ( nValue < m_spinMin.GetPos() )
	{
		m_spinMin.SetPos( nValue );
	}
}

void CDialogVelocity::OnKillfocusEditMin() 
{
	BOOL fTransSucceeded;
	int nValue = GetDlgItemInt( IDC_EDIT_MIN, &fTransSucceeded, FALSE );
	if ( !fTransSucceeded )
	{
		m_spinMin.SetPos( 1 );
	}
	else if( nValue > 127 )
	{
		m_spinMin.SetPos( 127 );
		m_spinMax.SetPos( 127 );
	}
	else if( nValue < 1 )
	{
		m_spinMin.SetPos( 1 );
	}
	else if( nValue > m_spinMax.GetPos() )
	{
		m_spinMax.SetPos( nValue );
	}
}

void CDialogVelocity::OnKillfocusEditStrength() 
{
	CString strText;
	m_editStrength.GetWindowText( strText );
	strText.TrimLeft();
	strText.TrimRight();

	// Strip off the leading '+'
	if( strText.GetAt(0) == _T('+') )
	{
		strText = strText.Right( strText.GetLength() - 1 );
	}

	TCHAR tcstrExtra[256];
	tcstrExtra[0] = 0;
	int nValue = 0;
	if( !strText.IsEmpty() )
	{
		_stscanf( strText, _T("%d%s"), &nValue, tcstrExtra );
	}

	if( strText.IsEmpty()
	||	(_tcslen(tcstrExtra) > 0) )
	{
		nValue = 0;
	}
	else if( IDC_RADIO_PERCENT == GetCheckedRadioButton( IDC_RADIO_PERCENT, IDC_RADIO_LINEAR ) )
	{
		// Percent
		if( nValue > 200 )
		{
			nValue = 200;
		}
		else if( nValue < -99 )
		{
			nValue = -99;
		}
	}
	else
	{
		// Absolute
		if( nValue > 127 )
		{
			nValue = 127;
		}
		else if( nValue < -126 )
		{
			nValue = -126;
		}
	}

	if( m_fEndTracksStart )
	{
		SetAbsEditText( m_editStrengthEnd, m_spinStrengthEnd, (int)(short)LOWORD(m_spinStrength.GetPos()) );
	}

	SetAbsEditText( m_editStrength, m_spinStrength, nValue );
}

void CDialogVelocity::OnKillfocusEditStrengthEnd()
{
	CString strText;
	m_editStrengthEnd.GetWindowText( strText );
	strText.TrimLeft();
	strText.TrimRight();

	// Strip off the leading '+'
	if( strText.GetAt(0) == _T('+') )
	{
		strText = strText.Right( strText.GetLength() - 1 );
	}

	TCHAR tcstrExtra[256];
	tcstrExtra[0] = 0;
	int nValue = 0;
	if( !strText.IsEmpty() )
	{
		_stscanf( strText, _T("%d%s"), &nValue, tcstrExtra );
	}

	if( strText.IsEmpty()
	||	(_tcslen(tcstrExtra) > 0) )
	{
		nValue = 0;
	}
	else if( IDC_RADIO_PERCENT == GetCheckedRadioButton( IDC_RADIO_PERCENT, IDC_RADIO_LINEAR ) )
	{
		// Percent
		if( nValue > 200 )
		{
			nValue = 200;
		}
		else if( nValue < -99 )
		{
			nValue = -99;
		}
	}
	else
	{
		// Absolute
		if( nValue > 127 )
		{
			nValue = 127;
		}
		else if( nValue < -126 )
		{
			nValue = -126;
		}
	}

	if( (int)(short)LOWORD(m_spinStrength.GetPos()) != nValue )
	{
		m_fEndTracksStart = false;
	}

	SetAbsEditText( m_editStrengthEnd, m_spinStrengthEnd, nValue );
}

void CDialogVelocity::OnOK() 
{
	// Set the Edit Velocity Target
	switch( GetCheckedRadioButton( IDC_RADIO_SELECTED_NOTES, IDC_RADIO_PATTERN ) )
	{
	case IDC_RADIO_SELECTED_NOTES:
		m_vtTarget = VELOCITY_TARGET_SELECTED;
		break;
	case IDC_RADIO_PART:
		m_vtTarget = VELOCITY_TARGET_PART;
		break;
	case IDC_RADIO_PATTERN:
		m_vtTarget = VELOCITY_TARGET_PATTERN;
		break;
	default:
		break;
	}

	m_bCompressMin = BYTE(m_spinMin.GetPos());
	m_bCompressMax = BYTE(m_spinMax.GetPos());
	m_lAbsoluteChangeStart = (int)(short)LOWORD(m_spinStrength.GetPos());
	m_lAbsoluteChangeEnd = (int)(short)LOWORD(m_spinStrengthEnd.GetPos());

	// Set the linear/percent flag
	m_fPercent = (IDC_RADIO_PERCENT == GetCheckedRadioButton( IDC_RADIO_PERCENT, IDC_RADIO_LINEAR ));

	// Set the absolute/compres flag
	m_fAbsolute = (IDC_RADIO_ABSOLUTE == GetCheckedRadioButton( IDC_RADIO_ABSOLUTE, IDC_RADIO_COMPRESS ));

	CDialog::OnOK();
}

void CDialogVelocity::EnableDlgItem( int nItemID, BOOL fEnable )
{
	CWnd* pWnd = GetDlgItem( nItemID );
	if (pWnd)
	{
		pWnd->EnableWindow( fEnable );
	}
}
