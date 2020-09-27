// Dlg3DParam.cpp : implementation file
//

#include "stdafx.h"
#include "Dlg3DParam.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define POSITION_FORMAT _T("%.4f")

/*
// The amount to multiply the slider position by to get the actual position value
#define POSITION_FACTOR (1000000000 / 10000)

#define VELOCITY_FORMAT POSITION_FORMAT
#define VELOCITY_FACTOR POSITION_FACTOR

#define CONE_FORMAT POSITION_FORMAT
#define CONE_FACTOR POSITION_FACTOR
*/

#define DISTANCE_FORMAT POSITION_FORMAT

/////////////////////////////////////////////////////////////////////////////
// C3DEdit class


IMPLEMENT_DYNCREATE( C3DEdit, CEdit )

C3DEdit::C3DEdit() : CEdit()
{
	m_fMinusValid = true;
	m_fDotValid = true;
}

LRESULT C3DEdit::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch( message )
	{
	case WM_CHAR:
		switch( wParam )
		{
		case 8:  // Backspace
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
			break;
		case '.':
			if( !m_fDotValid )
			{
				return TRUE;
			}
			break;
		case '-':
			if( m_fMinusValid )
			{
				break;
			}
			// else return TRUE
		default:
			return TRUE;
		break;
		}
	}
	return CEdit::WindowProc( message, wParam, lParam );
}


/////////////////////////////////////////////////////////////////////////////
// CDlg3DParam dialog


CDlg3DParam::CDlg3DParam(CWnd* pParent /*=NULL*/)
	: CDialog(CDlg3DParam::IDD, pParent)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	//{{AFX_DATA_INIT(CDlg3DParam)
	//}}AFX_DATA_INIT
}


void CDlg3DParam::DoDataExchange(CDataExchange* pDX)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlg3DParam)
	DDX_Control(pDX, IDC_EDIT_CONE_OUTSIDE, m_editConeOutside);
	DDX_Control(pDX, IDC_EDIT_CONE_INSIDE, m_editConeInside);
	DDX_Control(pDX, IDC_EDIT_CONE_VOLUME, m_editConeVolume);
	DDX_Control(pDX, IDC_EDIT_MAX_DISTANCE, m_editMaxDistance);
	DDX_Control(pDX, IDC_EDIT_MIN_DISTANCE, m_editMinDistance);
	DDX_Control(pDX, IDC_EDIT_ZVEL, m_editZVel);
	DDX_Control(pDX, IDC_EDIT_YVEL, m_editYVel);
	DDX_Control(pDX, IDC_EDIT_XVEL, m_editXVel);
	DDX_Control(pDX, IDC_EDIT_ZPOS, m_editZPos);
	DDX_Control(pDX, IDC_EDIT_YPOS, m_editYPos);
	DDX_Control(pDX, IDC_EDIT_XPOS, m_editXPos);
	DDX_Control(pDX, IDC_EDIT_ZCONE, m_editZCone);
	DDX_Control(pDX, IDC_EDIT_YCONE, m_editYCone);
	DDX_Control(pDX, IDC_EDIT_XCONE, m_editXCone);
	DDX_Control(pDX, IDC_COMBO_MODE, m_comboMode);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlg3DParam, CDialog)
	//{{AFX_MSG_MAP(CDlg3DParam)
	ON_EN_KILLFOCUS(IDC_EDIT_CONE_INSIDE, OnKillfocusEditConeInside)
	ON_EN_KILLFOCUS(IDC_EDIT_CONE_OUTSIDE, OnKillfocusEditConeOutside)
	ON_EN_KILLFOCUS(IDC_EDIT_CONE_VOLUME, OnKillfocusEditConeVolume)
	ON_EN_KILLFOCUS(IDC_EDIT_MAX_DISTANCE, OnKillfocusEditMaxDistance)
	ON_EN_KILLFOCUS(IDC_EDIT_MIN_DISTANCE, OnKillfocusEditMinDistance)
	ON_EN_KILLFOCUS(IDC_EDIT_XCONE, OnKillfocusEditXcone)
	ON_EN_KILLFOCUS(IDC_EDIT_XPOS, OnKillfocusEditXpos)
	ON_EN_KILLFOCUS(IDC_EDIT_XVEL, OnKillfocusEditXvel)
	ON_EN_KILLFOCUS(IDC_EDIT_YCONE, OnKillfocusEditYcone)
	ON_EN_KILLFOCUS(IDC_EDIT_YPOS, OnKillfocusEditYpos)
	ON_EN_KILLFOCUS(IDC_EDIT_YVEL, OnKillfocusEditYvel)
	ON_EN_KILLFOCUS(IDC_EDIT_ZCONE, OnKillfocusEditZcone)
	ON_EN_KILLFOCUS(IDC_EDIT_ZPOS, OnKillfocusEditZpos)
	ON_EN_KILLFOCUS(IDC_EDIT_ZVEL, OnKillfocusEditZvel)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlg3DParam message handlers

// Slider control helper function
/*
void OnHSCrollHelper( UINT nSBCode, UINT nPos, CSliderCtrl &sliderCtrl, C3DEdit &editCtrl )
{
	UNREFERENCED_PARAMETER(nPos);
	UNREFERENCED_PARAMETER(nSBCode);
	CString strText;
	strText.Format( POSITION_FORMAT, float(sliderCtrl.GetPos()) );
	editCtrl.SetWindowText( strText );
}

void CDlg3DParam::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pScrollBar )
	{
		switch( pScrollBar->GetDlgCtrlID() )
		{
		case IDC_SLIDER_XPOS:
			OnHSCrollHelper( nSBCode, nPos, m_sliderXPos, m_editXPos );
			break;
		case IDC_SLIDER_YPOS:
			OnHSCrollHelper( nSBCode, nPos, m_sliderYPos, m_editYPos );
			break;
		case IDC_SLIDER_ZPOS:
			OnHSCrollHelper( nSBCode, nPos, m_sliderZPos, m_editZPos );
			break;

		case IDC_SLIDER_XVEL:
			OnHSCrollHelper( nSBCode, nPos, m_sliderXVel, m_editXVel );
			break;
		case IDC_SLIDER_YVEL:
			OnHSCrollHelper( nSBCode, nPos, m_sliderYVel, m_editYVel );
			break;
		case IDC_SLIDER_ZVEL:
			OnHSCrollHelper( nSBCode, nPos, m_sliderZVel, m_editZVel );
			break;

		case IDC_SLIDER_XCONE:
			OnHSCrollHelper( nSBCode, nPos, m_sliderXCone, m_editXCone );
			break;
		case IDC_SLIDER_YCONE:
			OnHSCrollHelper( nSBCode, nPos, m_sliderYCone, m_editYCone );
			break;
		case IDC_SLIDER_ZCONE:
			OnHSCrollHelper( nSBCode, nPos, m_sliderZCone, m_editZCone );
			break;
		}
	}

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}
*/

void CDlg3DParam::OnOK() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	/* Not necessary - updated on EN_KILLFOCUS notifications
	// Get Position
	if( !GetFloat( IDC_EDIT_XPOS, &m_ds3DBuffer.vPosition.x ) )
	{
		AfxMessageBox("Bad X Position.");
		return;
	}
	if( !GetFloat( IDC_EDIT_YPOS, &m_ds3DBuffer.vPosition.y ) )
	{
		AfxMessageBox("Bad Y Position.");
		return;
	}
	if( !GetFloat( IDC_EDIT_ZPOS, &m_ds3DBuffer.vPosition.z ) )
	{
		AfxMessageBox("Bad Z Position.");
		return;
	}

	// Get Velocity
	if( !GetFloat( IDC_EDIT_XVEL, &m_ds3DBuffer.vVelocity.x ) )
	{
		AfxMessageBox("Bad X Velocity.");
		return;
	}
	if( !GetFloat( IDC_EDIT_YVEL, &m_ds3DBuffer.vVelocity.y ) )
	{
		AfxMessageBox("Bad Y Velocity.");
		return;
	}
	if( !GetFloat( IDC_EDIT_ZVEL, &m_ds3DBuffer.vVelocity.z ) )
	{
		AfxMessageBox("Bad Z Velocity.");
		return;
	}

	// Get Cone Orientation
	if( !GetFloat( IDC_EDIT_XCONE, &m_ds3DBuffer.vConeOrientation.x ) )
	{
		AfxMessageBox("Bad X Cone Orientation.");
		return;
	}
	if( !GetFloat( IDC_EDIT_YCONE, &m_ds3DBuffer.vConeOrientation.y ) )
	{
		AfxMessageBox("Bad Y Cone Orientation.");
		return;
	}
	if( !GetFloat( IDC_EDIT_ZCONE, &m_ds3DBuffer.vConeOrientation.z ) )
	{
		AfxMessageBox("Bad Z Cone Orientation.");
		return;
	}

	// Get Cone angles
	BOOL fSuccess = FALSE;
	long lTemp;
	lTemp = GetDlgItemInt( IDC_EDIT_CONE_INSIDE, &fSuccess, FALSE );
	if( !fSuccess )
	{
		AfxMessageBox("Bad Cone Inside Angle.");
		return;
	}
	m_ds3DBuffer.dwInsideConeAngle = lTemp;
	lTemp = GetDlgItemInt( IDC_EDIT_CONE_OUTSIDE, &fSuccess, FALSE );
	if( !fSuccess )
	{
		AfxMessageBox("Bad Cone Outisde Angle.");
		return;
	}
	m_ds3DBuffer.dwOutsideConeAngle = lTemp;


	// Get Cone outside volume
	lTemp = GetDlgItemInt( IDC_EDIT_CONE_VOLUME, &fSuccess, TRUE );
	if( !fSuccess )
	{
		AfxMessageBox("Bad Cone Outside Volume.");
		return;
	}
	m_ds3DBuffer.lConeOutsideVolume = lTemp;


	// Get distance
	if( !GetFloat( IDC_EDIT_MIN_DISTANCE, &m_ds3DBuffer.flMinDistance ) )
	{
		AfxMessageBox("Bad Min Distance.");
		return;
	}
	if( !GetFloat( IDC_EDIT_MAX_DISTANCE, &m_ds3DBuffer.flMaxDistance ) )
	{
		AfxMessageBox("Bad Max Distance.");
		return;
	}
	*/


	// Get mode
	long lTemp = m_comboMode.GetCurSel();
	if( lTemp == CB_ERR )
	{
		AfxMessageBox("Bad Mode.");
		return;
	}
	m_ds3DBuffer.dwMode = lTemp;
	
	CDialog::OnOK();
}

void SliderInitHelper( /*CSliderCtrl &sliderCtrl,*/ C3DEdit &editCtrl, float fValue, int nLength )
{
	/*
	sliderCtrl.SetRangeMin( int(-DS3D_DEFAULTMAXDISTANCE), FALSE );
	sliderCtrl.SetRangeMax( int(DS3D_DEFAULTMAXDISTANCE), TRUE );
	sliderCtrl.SetLineSize( int(DS3D_DEFAULTMAXDISTANCE / 100) );
	sliderCtrl.SetPageSize( int(DS3D_DEFAULTMAXDISTANCE / 10) );
	sliderCtrl.SetPos( int(fValue) );
	*/
	CString strText;
	strText.Format( POSITION_FORMAT, fValue );
	editCtrl.SetWindowText( strText );
	editCtrl.SetLimitText( nLength );
}

BOOL CDlg3DParam::OnInitDialog() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CDialog::OnInitDialog();

	CString strText;
	int nLength;

	// Compute the maximum number of characters for the postition, velocity, and cone orientation
	strText.Format( _T("%f"), -DS3D_DEFAULTMAXDISTANCE );
	nLength = strText.GetLength();

	// Set Position
	SliderInitHelper( /*m_sliderXPos,*/ m_editXPos, m_ds3DBuffer.vPosition.x, nLength );
	SliderInitHelper( /*m_sliderYPos,*/ m_editYPos, m_ds3DBuffer.vPosition.y, nLength );
	SliderInitHelper( /*m_sliderZPos,*/ m_editZPos, m_ds3DBuffer.vPosition.z, nLength );


	// Set Velocity
	SliderInitHelper( /*m_sliderXVel,*/ m_editXVel, m_ds3DBuffer.vVelocity.x, nLength );
	SliderInitHelper( /*m_sliderYVel,*/ m_editYVel, m_ds3DBuffer.vVelocity.y, nLength );
	SliderInitHelper( /*m_sliderZVel,*/ m_editZVel, m_ds3DBuffer.vVelocity.z, nLength );


	// Set Cone Orientation
	SliderInitHelper( /*m_sliderXCone,*/ m_editXCone, m_ds3DBuffer.vConeOrientation.x, nLength );
	SliderInitHelper( /*m_sliderYCone,*/ m_editYCone, m_ds3DBuffer.vConeOrientation.y, nLength );
	SliderInitHelper( /*m_sliderZCone,*/ m_editZCone, m_ds3DBuffer.vConeOrientation.z, nLength );


	// Set Cone angles
	SetDlgItemInt( IDC_EDIT_CONE_INSIDE, m_ds3DBuffer.dwInsideConeAngle, FALSE );
	SetDlgItemInt( IDC_EDIT_CONE_OUTSIDE, m_ds3DBuffer.dwOutsideConeAngle, FALSE );
	strText.Format( _T("%d"), DS3D_MAXCONEANGLE );
	m_editConeInside.SetLimitText( strText.GetLength() );
	m_editConeOutside.SetLimitText( strText.GetLength() );


	// Set Cone outside volume
	m_editConeVolume.m_fDotValid = false;
	SetDlgItemInt( IDC_EDIT_CONE_VOLUME, m_ds3DBuffer.lConeOutsideVolume, TRUE );
	strText.Format( _T("%d"), DSBVOLUME_MIN );
	m_editConeVolume.SetLimitText( strText.GetLength() );


	// Set distance
	m_editMaxDistance.m_fMinusValid = false;
	m_editMinDistance.m_fMinusValid = false;
	strText.Format( DISTANCE_FORMAT, m_ds3DBuffer.flMinDistance );
	SetDlgItemText( IDC_EDIT_MIN_DISTANCE, strText );
	strText.Format( DISTANCE_FORMAT, m_ds3DBuffer.flMaxDistance );
	SetDlgItemText( IDC_EDIT_MAX_DISTANCE, strText );


	// Set mode
	m_comboMode.SetCurSel( m_ds3DBuffer.dwMode );


	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

bool CDlg3DParam::GetFloat( int nControlID, float *pfValue )
{
	ASSERT( pfValue );

	CString strText;
	if( GetDlgItemText( nControlID, strText ) == 0 )
	{
		// If no text was copied, return false
		return false;
	}

	// Convert from text to float
	bool fResult = false;
	TCHAR *tcstrTmp = new TCHAR[strText.GetLength() + 1];
	tcstrTmp[0] = 0;
	if( _stscanf( strText, "%f%s", pfValue, tcstrTmp )
	&&	(tcstrTmp[0] == 0) )
	{
		fResult = true;
	}

	delete tcstrTmp;

	return fResult;
}

void CDlg3DParam::OnKillfocusEditConeInside() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	BOOL fSuccess = FALSE;
	DWORD dwTemp = GetDlgItemInt( IDC_EDIT_CONE_INSIDE, &fSuccess, FALSE );
	if( fSuccess )
	{
		if( dwTemp > DS3D_MAXCONEANGLE )
		{
			dwTemp = DS3D_MAXCONEANGLE;
		}

		if( dwTemp > m_ds3DBuffer.dwOutsideConeAngle )
		{
			m_ds3DBuffer.dwOutsideConeAngle = dwTemp;
			SetDlgItemInt( IDC_EDIT_CONE_OUTSIDE, dwTemp, FALSE );
		}

		m_ds3DBuffer.dwInsideConeAngle = dwTemp;
	}

	SetDlgItemInt( IDC_EDIT_CONE_INSIDE, m_ds3DBuffer.dwInsideConeAngle, FALSE );
}

void CDlg3DParam::OnKillfocusEditConeOutside() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	BOOL fSuccess = FALSE;
	DWORD dwTemp = GetDlgItemInt( IDC_EDIT_CONE_OUTSIDE, &fSuccess, FALSE );
	if( fSuccess )
	{
		if( dwTemp > DS3D_MAXCONEANGLE )
		{
			dwTemp = DS3D_MAXCONEANGLE;
		}

		if( dwTemp < m_ds3DBuffer.dwInsideConeAngle )
		{
			m_ds3DBuffer.dwInsideConeAngle = dwTemp;
			SetDlgItemInt( IDC_EDIT_CONE_INSIDE, dwTemp, FALSE );
		}

		m_ds3DBuffer.dwOutsideConeAngle = dwTemp;
	}

	SetDlgItemInt( IDC_EDIT_CONE_OUTSIDE, m_ds3DBuffer.dwOutsideConeAngle, FALSE );
}

void CDlg3DParam::OnKillfocusEditConeVolume() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	BOOL fSuccess = FALSE;
	long lTemp = GetDlgItemInt( IDC_EDIT_CONE_VOLUME, &fSuccess, TRUE );
	if( fSuccess )
	{
		if( lTemp < DSBVOLUME_MIN )
		{
			lTemp = DSBVOLUME_MIN;
		}
		else if( lTemp > DSBVOLUME_MAX )
		{
			lTemp = DSBVOLUME_MAX;
		}

		m_ds3DBuffer.lConeOutsideVolume = lTemp;
	}

	SetDlgItemInt( IDC_EDIT_CONE_VOLUME, m_ds3DBuffer.lConeOutsideVolume, TRUE );
}

void CDlg3DParam::OnKillfocusEditMaxDistance() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CString strText;
	float fTemp;
	if( !GetFloat( IDC_EDIT_MAX_DISTANCE, &fTemp ) )
	{
		strText.Format( DISTANCE_FORMAT, m_ds3DBuffer.flMaxDistance );
		SetDlgItemText( IDC_EDIT_MAX_DISTANCE, strText );
		return;
	}

	if( fTemp <= 0.0 )
	{
		fTemp = 0.01f;
	}

	if( fTemp < m_ds3DBuffer.flMinDistance )
	{
		m_ds3DBuffer.flMinDistance = fTemp;
		strText.Format( DISTANCE_FORMAT, fTemp );
		SetDlgItemText( IDC_EDIT_MIN_DISTANCE, strText );
	}

	m_ds3DBuffer.flMaxDistance = fTemp;
	strText.Format( DISTANCE_FORMAT, fTemp );
	SetDlgItemText( IDC_EDIT_MAX_DISTANCE, strText );
}

void CDlg3DParam::OnKillfocusEditMinDistance() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CString strText;
	float fTemp;
	if( !GetFloat( IDC_EDIT_MIN_DISTANCE, &fTemp ) )
	{
		strText.Format( DISTANCE_FORMAT, m_ds3DBuffer.flMinDistance );
		SetDlgItemText( IDC_EDIT_MIN_DISTANCE, strText );
		return;
	}

	if( fTemp <= 0.0 )
	{
		fTemp = 0.01f;
	}

	if( fTemp > m_ds3DBuffer.flMaxDistance )
	{
		m_ds3DBuffer.flMaxDistance = fTemp;
		strText.Format( DISTANCE_FORMAT, fTemp );
		SetDlgItemText( IDC_EDIT_MAX_DISTANCE, strText );
	}

	m_ds3DBuffer.flMinDistance = fTemp;
	strText.Format( DISTANCE_FORMAT, fTemp );
	SetDlgItemText( IDC_EDIT_MIN_DISTANCE, strText );
}

void CDlg3DParam::OnKillfocusEditXcone() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CString strText;
	float fTemp;
	if( GetFloat( IDC_EDIT_XCONE, &fTemp ) )
	{
		if( (fTemp == 0.0)
		&&	(m_ds3DBuffer.vConeOrientation.y == 0.0)
		&&	(m_ds3DBuffer.vConeOrientation.z == 0.0) )
		{
			fTemp = 0.01f;
		}

		m_ds3DBuffer.vConeOrientation.x = fTemp;
	}

	strText.Format( DISTANCE_FORMAT, m_ds3DBuffer.vConeOrientation.x );
	SetDlgItemText( IDC_EDIT_XCONE, strText );
}

void CDlg3DParam::OnKillfocusEditYcone() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CString strText;
	float fTemp;
	if( GetFloat( IDC_EDIT_YCONE, &fTemp ) )
	{
		if( (fTemp == 0.0)
		&&	(m_ds3DBuffer.vConeOrientation.x == 0.0)
		&&	(m_ds3DBuffer.vConeOrientation.z == 0.0) )
		{
			fTemp = 0.01f;
		}

		m_ds3DBuffer.vConeOrientation.y = fTemp;
	}

	strText.Format( DISTANCE_FORMAT, m_ds3DBuffer.vConeOrientation.y );
	SetDlgItemText( IDC_EDIT_YCONE, strText );
}

void CDlg3DParam::OnKillfocusEditZcone() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CString strText;
	float fTemp;
	if( GetFloat( IDC_EDIT_ZCONE, &fTemp ) )
	{
		if( (fTemp == 0.0)
		&&	(m_ds3DBuffer.vConeOrientation.x == 0.0)
		&&	(m_ds3DBuffer.vConeOrientation.y == 0.0) )
		{
			fTemp = 0.01f;
		}

		m_ds3DBuffer.vConeOrientation.z = fTemp;
	}

	strText.Format( DISTANCE_FORMAT, m_ds3DBuffer.vConeOrientation.z );
	SetDlgItemText( IDC_EDIT_ZCONE, strText );
}

void CDlg3DParam::OnKillfocusEditXpos() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CString strText;
	float fTemp;
	if( GetFloat( IDC_EDIT_XPOS, &fTemp ) )
	{
		m_ds3DBuffer.vPosition.x = fTemp;
	}

	strText.Format( DISTANCE_FORMAT, m_ds3DBuffer.vPosition.x );
	SetDlgItemText( IDC_EDIT_XPOS, strText );
}

void CDlg3DParam::OnKillfocusEditYpos() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CString strText;
	float fTemp;
	if( GetFloat( IDC_EDIT_YPOS, &fTemp ) )
	{
		m_ds3DBuffer.vPosition.y = fTemp;
	}

	strText.Format( DISTANCE_FORMAT, m_ds3DBuffer.vPosition.y );
	SetDlgItemText( IDC_EDIT_YPOS, strText );
}

void CDlg3DParam::OnKillfocusEditZpos() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CString strText;
	float fTemp;
	if( GetFloat( IDC_EDIT_ZPOS, &fTemp ) )
	{
		m_ds3DBuffer.vPosition.z = fTemp;
	}

	strText.Format( DISTANCE_FORMAT, m_ds3DBuffer.vPosition.z );
	SetDlgItemText( IDC_EDIT_ZPOS, strText );
}

void CDlg3DParam::OnKillfocusEditXvel() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CString strText;
	float fTemp;
	if( GetFloat( IDC_EDIT_XVEL, &fTemp ) )
	{
		m_ds3DBuffer.vVelocity.x = fTemp;
	}

	strText.Format( DISTANCE_FORMAT, m_ds3DBuffer.vVelocity.x );
	SetDlgItemText( IDC_EDIT_XVEL, strText );
}

void CDlg3DParam::OnKillfocusEditYvel() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CString strText;
	float fTemp;
	if( GetFloat( IDC_EDIT_YVEL, &fTemp ) )
	{
		m_ds3DBuffer.vVelocity.y = fTemp;
	}

	strText.Format( DISTANCE_FORMAT, m_ds3DBuffer.vVelocity.y );
	SetDlgItemText( IDC_EDIT_YVEL, strText );
}

void CDlg3DParam::OnKillfocusEditZvel() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CString strText;
	float fTemp;
	if( GetFloat( IDC_EDIT_ZVEL, &fTemp ) )
	{
		m_ds3DBuffer.vVelocity.z = fTemp;
	}

	strText.Format( DISTANCE_FORMAT, m_ds3DBuffer.vVelocity.z );
	SetDlgItemText( IDC_EDIT_ZVEL, strText );
}
