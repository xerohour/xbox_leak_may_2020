// PropPageCurve.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "PropPageCurve.h"
#include "PropCurve.h"
#include "CurveStrip.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define SMALL_BUFFER	32
#define DIALOG_LEN		20

static const TCHAR achUnknown[11] = "----------";

//////////////////////////////////////////////////////////////////////
// CPropPageCurve Construction/Destruction
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CPropPageCurve property page

CPropPageCurve::CPropPageCurve( CCurvePropPageMgr* pCurvePropPageMgr ) : CPropertyPage(CPropPageCurve::IDD)
{
	//{{AFX_DATA_INIT(CPropPageCurve)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	
	ASSERT( pCurvePropPageMgr != NULL );

	m_pPageManager = pCurvePropPageMgr;
	m_fInUpdateControls = FALSE;
	m_fNeedToDetach = FALSE;
	m_fInOnKillFocusStartValue = FALSE;
	m_fInOnKillFocusEndValue = FALSE;
}

CPropPageCurve::~CPropPageCurve()
{
}


/////////////////////////////////////////////////////////////////////////////
// CPropPageCurve::EnableItem

void CPropPageCurve::EnableItem( int nID, BOOL fEnable )
{
	CWnd* pWnd = GetDlgItem( nID );

	if( pWnd )
	{
		if( fEnable == FALSE )
		{
			pWnd->SetWindowText( achUnknown );
		}
		pWnd->EnableWindow( fEnable );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CPropPageCurve::EnableControls

void CPropPageCurve::EnableControls( BOOL fEnable ) 
{
	EnableItem( IDC_EDIT_STARTBAR, fEnable );
	m_spinStartBar.EnableWindow( fEnable );
	EnableItem( IDC_EDIT_STARTBEAT, fEnable );
	m_spinStartBeat.EnableWindow( fEnable );
	EnableItem( IDC_EDIT_STARTGRID, fEnable );
	m_spinStartGrid.EnableWindow( fEnable );
	EnableItem( IDC_EDIT_STARTTICK, fEnable );
	m_spinStartTick.EnableWindow( fEnable );

	EnableItem( IDC_EDIT_ENDBAR, fEnable );
	m_spinEndBar.EnableWindow( fEnable );
	EnableItem( IDC_EDIT_ENDBEAT, fEnable );
	m_spinEndBeat.EnableWindow( fEnable );
	EnableItem( IDC_EDIT_ENDGRID, fEnable );
	m_spinEndGrid.EnableWindow( fEnable );
	EnableItem( IDC_EDIT_ENDTICK, fEnable );
	m_spinEndTick.EnableWindow( fEnable );

	EnableItem( IDC_EDIT_DURBAR, fEnable );
	m_spinDurBar.EnableWindow( fEnable );
	EnableItem( IDC_EDIT_DURBEAT, fEnable );
	m_spinDurBeat.EnableWindow( fEnable );
	EnableItem( IDC_EDIT_DURGRID, fEnable );
	m_spinDurGrid.EnableWindow( fEnable );
	EnableItem( IDC_EDIT_DURTICK, fEnable );
	m_spinDurTick.EnableWindow( fEnable );

	EnableItem( IDC_EDIT_STARTVALUE, fEnable );
	m_spinStartValue.EnableWindow( fEnable );
	EnableItem( IDC_EDIT_ENDVALUE, fEnable );
	m_spinEndValue.EnableWindow( fEnable );

	if( m_pPageManager
	&& (m_pPageManager->m_PropCurve.m_bEventType == DMUS_CURVET_PBCURVE
		|| (m_pPageManager->m_PropCurve.m_bEventType == DMUS_CURVET_CCCURVE
			&& (m_pPageManager->m_PropCurve.m_bCCData == 7 // Volume
				|| m_pPageManager->m_PropCurve.m_bCCData == 11 // Expression
				|| m_pPageManager->m_PropCurve.m_bCCData == 1 // Mod wheel
				|| m_pPageManager->m_PropCurve.m_bCCData == 91 // Reverb send
				|| m_pPageManager->m_PropCurve.m_bCCData == 93))) ) // Chorus send
	{
		m_spinMergeIndex.ShowWindow( SW_NORMAL );
		m_editMergeIndex.ShowWindow( SW_NORMAL );
		m_staticMergeIndex.ShowWindow( SW_NORMAL );

		m_checkStartCurrent.ShowWindow( SW_NORMAL );

		EnableItem( IDC_EDIT_MERGE_INDEX, fEnable );
		m_spinMergeIndex.EnableWindow( fEnable );

		m_checkStartCurrent.EnableWindow( fEnable );
	}
	else
	{
		m_spinMergeIndex.ShowWindow( SW_HIDE );
		m_editMergeIndex.ShowWindow( SW_HIDE );
		m_staticMergeIndex.ShowWindow( SW_HIDE );

		m_checkStartCurrent.ShowWindow( SW_HIDE );
	}

	if( m_pPageManager
	&&	m_pPageManager->m_PropCurve.m_bEventType == DMUS_CURVET_RPNCURVE
	&&	m_pPageManager->m_PropCurve.m_wParamType == 0 )
	{
		m_staticPBRSemitones.ShowWindow( SW_NORMAL );
		m_staticPBRCents.ShowWindow( SW_NORMAL );
		m_spinPBRStartCents.ShowWindow( SW_NORMAL );
		m_editPBRStartCents.ShowWindow( SW_NORMAL );
		m_spinPBREndCents.ShowWindow( SW_NORMAL );
		m_editPBREndCents.ShowWindow( SW_NORMAL );


		EnableItem( IDC_EDIT_PBR_START_CENTS, fEnable );
		EnableItem( IDC_EDIT_PBR_END_CENTS, fEnable );
		m_spinPBRStartCents.EnableWindow( fEnable );
		m_spinPBREndCents.EnableWindow( fEnable );
	}
	else
	{
		m_staticPBRSemitones.ShowWindow( SW_HIDE );
		m_staticPBRCents.ShowWindow( SW_HIDE );
		m_spinPBRStartCents.ShowWindow( SW_HIDE );
		m_editPBRStartCents.ShowWindow( SW_HIDE );
		m_spinPBREndCents.ShowWindow( SW_HIDE );
		m_editPBREndCents.ShowWindow( SW_HIDE );
	}

	m_comboShape.EnableWindow( fEnable );
	m_btnFlipHorz.EnableWindow( fEnable );
	m_btnFlipVert.EnableWindow( fEnable );
}


/////////////////////////////////////////////////////////////////////////////
// CPropPageCurve::SetSpinStartValuePos

void CPropPageCurve::SetSpinStartValuePos( int nStartValue )
{
	if( m_pPageManager->m_PropCurve.m_bEventType == DMUS_CURVET_PBCURVE )
	{
		m_spinStartValue.SetPos( nStartValue + PB_DISP_OFFSET );
	}
	else if( m_pPageManager->m_PropCurve.m_bEventType == DMUS_CURVET_RPNCURVE
		&&	 m_pPageManager->m_PropCurve.m_wParamType == 0 )
	{
		m_spinStartValue.SetPos( nStartValue / 128 );
	}
	else
	{
		m_spinStartValue.SetPos( nStartValue );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CPropPageCurve::SetSpinStartCentsValuePos

void CPropPageCurve::SetSpinStartCentsValuePos( int nStartValue )
{
	m_spinPBRStartCents.SetPos( nStartValue % 128 );
}


/////////////////////////////////////////////////////////////////////////////
// CPropPageCurve::GetSpinStartValuePos

int CPropPageCurve::GetSpinStartValuePos( void )
{
	int nStartValue;

	if( m_pPageManager->m_PropCurve.m_bEventType == DMUS_CURVET_PBCURVE )
	{
		nStartValue = LOWORD(m_spinStartValue.GetPos()) - PB_DISP_OFFSET;
	}
	else
	{
		nStartValue = LOWORD(m_spinStartValue.GetPos());
	}

	return nStartValue;
}


/////////////////////////////////////////////////////////////////////////////
// CPropPageCurve::SetSpinEndValuePos

void CPropPageCurve::SetSpinEndValuePos( int nEndValue )
{
	if( m_pPageManager->m_PropCurve.m_bEventType == DMUS_CURVET_PBCURVE )
	{
		m_spinEndValue.SetPos( nEndValue + PB_DISP_OFFSET );
	}
	else if( m_pPageManager->m_PropCurve.m_bEventType == DMUS_CURVET_RPNCURVE
		&&	 m_pPageManager->m_PropCurve.m_wParamType == 0 )
	{
		m_spinEndValue.SetPos( nEndValue / 128 );
	}
	else
	{
		m_spinEndValue.SetPos( nEndValue );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CPropPageCurve::SetSpinEndCentsValuePos

void CPropPageCurve::SetSpinEndCentsValuePos( int nEndValue )
{
	m_spinPBREndCents.SetPos( nEndValue % 128 );
}


/////////////////////////////////////////////////////////////////////////////
// CPropPageCurve::GetSpinEndValuePos

int CPropPageCurve::GetSpinEndValuePos( void )
{
	int nEndValue;

	if( m_pPageManager->m_PropCurve.m_bEventType == DMUS_CURVET_PBCURVE )
	{
		nEndValue = LOWORD(m_spinEndValue.GetPos()) - PB_DISP_OFFSET;
	}
	else
	{
		nEndValue = LOWORD(m_spinEndValue.GetPos());
	}

	return nEndValue;
}


/////////////////////////////////////////////////////////////////////////////
// CPropPageCurve::UpdateControls

void CPropPageCurve::UpdateControls( CPropCurve* pPropCurve )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	ASSERT( m_pPageManager != NULL );
	
	// Make sure controls have been created
	if( ::IsWindow(m_btnFlipVert.m_hWnd) == FALSE )
	{
		return;
	}

	m_fInUpdateControls = TRUE;

	// Update controls
	if( pPropCurve == NULL )
	{
		EnableControls( FALSE );
		m_comboShape.SetCurSel( -1 );

		m_fInUpdateControls = FALSE;
		return;
	}

	EnableControls( TRUE );
	SetControlRanges( pPropCurve );

	int nPos;

	if( pPropCurve->m_dwUndetermined & UNDT_STARTBAR )
	{
		m_editStartBar.SetWindowText( achUnknown );
	}
	else
	{
		nPos = m_spinStartBar.GetPos();
		if( HIWORD(nPos) != 0
		||  LOWORD(nPos) != pPropCurve->m_lStartBar )
		{
			m_spinStartBar.SetPos( pPropCurve->m_lStartBar );
		}
	}

	if( pPropCurve->m_dwUndetermined & UNDT_STARTBEAT )
	{
		m_editStartBeat.SetWindowText( achUnknown );
	}
	else
	{
		nPos = m_spinStartBeat.GetPos();
		if( HIWORD(nPos) != 0
		||  LOWORD(nPos) != pPropCurve->m_lStartBeat )
		{
			m_spinStartBeat.SetPos( pPropCurve->m_lStartBeat );
		}
	}

	if( pPropCurve->m_dwUndetermined & UNDT_STARTGRID )
	{
		m_editStartGrid.SetWindowText( achUnknown );
	}
	else
	{
		nPos = m_spinStartGrid.GetPos();
		if( HIWORD(nPos) != 0
		||  LOWORD(nPos) != pPropCurve->m_lStartGrid )
		{
			m_spinStartGrid.SetPos( pPropCurve->m_lStartGrid );
		}
	}

	if( pPropCurve->m_dwUndetermined & UNDT_STARTTICK )
	{
		m_editStartTick.SetWindowText( achUnknown );
	}
	else
	{
		nPos = m_spinStartTick.GetPos();
		if( HIWORD(nPos) != 0
		||  LOWORD(nPos) != pPropCurve->m_lStartTick )
		{
			m_spinStartTick.SetPos( pPropCurve->m_lStartTick );
		}
	}

	if( pPropCurve->m_dwUndetermined & UNDT_ENDBAR )
	{
		m_editEndBar.SetWindowText( achUnknown );
	}
	else
	{
		nPos = m_spinEndBar.GetPos();
		if( HIWORD(nPos) != 0
		||  LOWORD(nPos) != pPropCurve->m_lEndBar )
		{
			m_spinEndBar.SetPos( pPropCurve->m_lEndBar );
		}
	}

	if( pPropCurve->m_dwUndetermined & UNDT_ENDBEAT )
	{
		m_editEndBeat.SetWindowText( achUnknown );
	}
	else
	{
		nPos = m_spinEndBeat.GetPos();
		if( HIWORD(nPos) != 0
		||  LOWORD(nPos) != pPropCurve->m_lEndBeat )
		{
			m_spinEndBeat.SetPos( pPropCurve->m_lEndBeat );
		}
	}

	if( pPropCurve->m_dwUndetermined & UNDT_ENDGRID )
	{
		m_editEndGrid.SetWindowText( achUnknown );
	}
	else
	{
		nPos = m_spinEndGrid.GetPos();
		if( HIWORD(nPos) != 0
		||  LOWORD(nPos) != pPropCurve->m_lEndGrid )
		{
			m_spinEndGrid.SetPos( pPropCurve->m_lEndGrid );
		}
	}

	if( pPropCurve->m_dwUndetermined & UNDT_ENDTICK )
	{
		m_editEndTick.SetWindowText( achUnknown );
	}
	else
	{
		nPos = m_spinEndTick.GetPos();
		if( HIWORD(nPos) != 0
		||  LOWORD(nPos) != pPropCurve->m_lEndTick )
		{
			m_spinEndTick.SetPos( pPropCurve->m_lEndTick );
		}
	}

	if( pPropCurve->m_dwUndetermined & UNDT_DURBAR )
	{
		m_editDurBar.SetWindowText( achUnknown );
	}
	else
	{
		nPos = m_spinDurBar.GetPos();
		if( HIWORD(nPos) != 0
		||  LOWORD(nPos) != pPropCurve->m_lDurBar )
		{
			m_spinDurBar.SetPos( pPropCurve->m_lDurBar );
		}
	}

	if( pPropCurve->m_dwUndetermined & UNDT_DURBEAT )
	{
		m_editDurBeat.SetWindowText( achUnknown );
	}
	else
	{
		nPos = m_spinDurBeat.GetPos();
		if( HIWORD(nPos) != 0
		||  LOWORD(nPos) != pPropCurve->m_lDurBeat )
		{
			m_spinDurBeat.SetPos( pPropCurve->m_lDurBeat );
		}
	}

	if( pPropCurve->m_dwUndetermined & UNDT_DURGRID )
	{
		m_editDurGrid.SetWindowText( achUnknown );
	}
	else
	{
		nPos = m_spinDurGrid.GetPos();
		if( HIWORD(nPos) != 0
		||  LOWORD(nPos) != pPropCurve->m_lDurGrid )
		{
			m_spinDurGrid.SetPos( pPropCurve->m_lDurGrid );
		}
	}

	if( pPropCurve->m_dwUndetermined & UNDT_DURTICK )
	{
		m_editDurTick.SetWindowText( achUnknown );
	}
	else
	{
		nPos = m_spinDurTick.GetPos();
		if( HIWORD(nPos) != 0
		||  LOWORD(nPos) != pPropCurve->m_lDurTick )
		{
			m_spinDurTick.SetPos( pPropCurve->m_lDurTick );
		}
	}

	if( pPropCurve->m_dwUndetermined & UNDT_STARTVAL )
	{
		m_editStartValue.SetWindowText( achUnknown );
	}
	else
	{
		CString strValue;
		StartEndValueToString( pPropCurve->m_nStartValue, strValue );

		CString strCurrentText;
		m_editStartValue.GetWindowText( strCurrentText );
		if( strValue != strCurrentText )
		{
			m_editStartValue.SetWindowText( strValue );
			// SetSel's needed to get caret to end of string
			m_editStartValue.SetSel( 0, -1 );
			m_editStartValue.SetSel( -1, -1 );

			SetSpinStartValuePos( pPropCurve->m_nStartValue ); 
		}
	}

	if( pPropCurve->m_dwUndetermined & UNDT_ENDVAL )
	{
		m_editEndValue.SetWindowText( achUnknown );
	}
	else
	{
		CString strValue;
		StartEndValueToString( pPropCurve->m_nEndValue, strValue );

		CString strCurrentText;
		m_editEndValue.GetWindowText( strCurrentText );
		if( strValue != strCurrentText )
		{
			m_editEndValue.SetWindowText( strValue );
			// SetSel's needed to get caret to end of string
			m_editEndValue.SetSel( 0, -1 );
			m_editEndValue.SetSel( -1, -1 );
			SetSpinEndValuePos( pPropCurve->m_nEndValue );
		}
	}

	if( pPropCurve->m_dwUndetermined & UNDT_SHAPE )
	{
		m_comboShape.SetCurSel( -1 );
	}
	else
	{
		if( pPropCurve->m_bCurveShape == DMUS_CURVES_SINE )
		{
			m_comboShape.SetCurSel( 0 );
		}
		else if( pPropCurve->m_bCurveShape == DMUS_CURVES_LOG )
		{
			m_comboShape.SetCurSel( 1 );
		}
		else if( pPropCurve->m_bCurveShape ==  DMUS_CURVES_EXP )
		{
			m_comboShape.SetCurSel( 2 );
		}
		else if( pPropCurve->m_bCurveShape == DMUS_CURVES_INSTANT )
		{
			m_comboShape.SetCurSel( 3 );
		}
		else if( pPropCurve->m_bCurveShape == DMUS_CURVES_LINEAR )
		{
			m_comboShape.SetCurSel( 4 );
		}
		else
		{
			m_comboShape.SetCurSel( -1 );
		}
	}

	if( pPropCurve->m_bEventType == DMUS_CURVET_PBCURVE
	|| (pPropCurve->m_bEventType == DMUS_CURVET_CCCURVE
		&& (pPropCurve->m_bCCData == 7 // Volume
			|| pPropCurve->m_bCCData == 11 // Expression
			|| pPropCurve->m_bCCData == 1 // Mod wheel
			|| pPropCurve->m_bCCData == 91 // Reverb send
			|| pPropCurve->m_bCCData == 93)) ) // Chorus send
	{
		// Merge index
		if( pPropCurve->m_dwUndetermined2 & UNDT2_MERGEINDEX )
		{
			m_editMergeIndex.SetWindowText( achUnknown );
		}
		else
		{
			nPos = m_spinMergeIndex.GetPos();
			if( HIWORD(nPos) != 0
			||  LOWORD(nPos) != pPropCurve->m_wMergeIndex )
			{
				m_spinMergeIndex.SetPos( pPropCurve->m_wMergeIndex );
			}
		}

		// Start from current checkbox
		if( pPropCurve->m_dwUndetermined2 & UNDT2_STARTCURRENT )
		{
			m_checkStartCurrent.SetCheck( 2 );
		}
		else
		{
			m_checkStartCurrent.SetCheck( pPropCurve->m_bFlags & DMUS_CURVE_START_FROM_CURRENT ? 1 : 0 );
		}
	}

	if( m_pPageManager
	&&	m_pPageManager->m_PropCurve.m_bEventType == DMUS_CURVET_RPNCURVE
	&&	m_pPageManager->m_PropCurve.m_wParamType == 0 )
	{
		// Start value
		if( pPropCurve->m_dwUndetermined & UNDT_STARTVAL )
		{
			m_editPBRStartCents.SetWindowText( achUnknown );
		}
		else
		{
			CString strValue;
			StartEndValueToCentsString( pPropCurve->m_nStartValue, strValue );

			CString strCurrentText;
			m_editPBRStartCents.GetWindowText( strCurrentText );
			if( strValue != strCurrentText )
			{
				m_editPBRStartCents.SetWindowText( strValue );
				// SetSel's needed to get caret to end of string
				m_editPBRStartCents.SetSel( 0, -1 );
				m_editPBRStartCents.SetSel( -1, -1 );

				SetSpinStartCentsValuePos( pPropCurve->m_nStartValue ); 
			}
		}

		// End value
		if( pPropCurve->m_dwUndetermined & UNDT_STARTVAL )
		{
			m_editPBREndCents.SetWindowText( achUnknown );
		}
		else
		{
			CString strValue;
			StartEndValueToCentsString( pPropCurve->m_nEndValue, strValue );

			CString strCurrentText;
			m_editPBREndCents.GetWindowText( strCurrentText );
			if( strValue != strCurrentText )
			{
				m_editPBREndCents.SetWindowText( strValue );
				// SetSel's needed to get caret to end of string
				m_editPBREndCents.SetSel( 0, -1 );
				m_editPBREndCents.SetSel( -1, -1 );
				SetSpinEndCentsValuePos( pPropCurve->m_nEndValue );
			}
		}
	}

	m_fInUpdateControls = FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CPropPageCurve message handlers


/////////////////////////////////////////////////////////////////////////////
// CPropPageCurve::OnCreate

int CPropPageCurve::OnCreate( LPCREATESTRUCT lpCreateStruct ) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Attach the window to the property page structure.
	// This has been done once already in the main application
	// since the main application owns the property sheet.
	// It needs to be done here so that the window handle can
	// be found in the DLLs handle map.
	if( !FromHandlePermanent( m_hWnd ) )
	{
		HWND hWnd = m_hWnd;
		m_hWnd = NULL;
		Attach( hWnd );
		m_fNeedToDetach = TRUE;
	}

	if( CPropertyPage::OnCreate(lpCreateStruct) == -1 )
	{
		return -1;
	}
	
	return 0;
}


/////////////////////////////////////////////////////////////////////////////
// CPropPageCurve::OnDestroy

void CPropPageCurve::OnDestroy() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Detach the window from the property page structure.
	// This will be done again by the main application since
	// it owns the property sheet.  It needs o be done here
	// so that the window handle can be removed from the
	// DLLs handle map.
	if( m_fNeedToDetach && m_hWnd )
	{
		HWND hWnd = m_hWnd;
		Detach();
		m_hWnd = hWnd;
	}

	CPropertyPage::OnDestroy();
}


/////////////////////////////////////////////////////////////////////////////
// CPropPageCurve::OnSetActive

BOOL CPropPageCurve::OnSetActive() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Update controls
	m_pPageManager->RefreshData();

	// Store active tab
	m_pPageManager->m_pIPropSheet->GetActivePage( &CCurvePropPageMgr::sm_nActiveTab );
	
	return CPropertyPage::OnSetActive();
}


/////////////////////////////////////////////////////////////////////////////
// CPropPageCurve::OnInitDialog

BOOL CPropPageCurve::OnInitDialog() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	CPropertyPage::OnInitDialog();

	// Fill shape combo box
	TCHAR achShape[SMALL_BUFFER];

	for ( int i = IDS_CURVE_SHAPE1 ;  i <= IDS_CURVE_SHAPE5 ;  i++ )
	{
		::LoadString( theApp.m_hInstance, i, achShape, SMALL_BUFFER );
		m_comboShape.AddString( achShape );
	}
	
	return FALSE;  // return TRUE unless you set the focus to a control
	               // EXCEPTION: OCX Property Pages should return FALSE
}


/////////////////////////////////////////////////////////////////////////////
// CPropPageCurve::OnSelChangeComboShape

void CPropPageCurve::OnSelChangeComboShape() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_pPageManager->m_pIPropPageObject == NULL
	||  m_fInUpdateControls == TRUE )
	{
		return;
	}

	m_pPageManager->m_PropCurve.m_bCurveShape = 0xFF; 

	switch( m_comboShape.GetCurSel() )
	{
		case 0:
			m_pPageManager->m_PropCurve.m_bCurveShape = DMUS_CURVES_SINE; 
			break;

		case 1:
			m_pPageManager->m_PropCurve.m_bCurveShape = DMUS_CURVES_LOG; 
			break;

		case 2:
			m_pPageManager->m_PropCurve.m_bCurveShape = DMUS_CURVES_EXP; 
			break;

		case 3:
			m_pPageManager->m_PropCurve.m_bCurveShape = DMUS_CURVES_INSTANT; 
			break;

		case 4:
			m_pPageManager->m_PropCurve.m_bCurveShape = DMUS_CURVES_LINEAR; 
			break;
	}

	if( m_pPageManager->m_PropCurve.m_bCurveShape != 0xFF )
	{
		m_pPageManager->m_PropCurve.m_dwChanged = CHGD_SHAPE;
		m_pPageManager->m_pIPropPageObject->SetData( &m_pPageManager->m_PropCurve );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CPropPageCurve::OnBtnFlipVert

void CPropPageCurve::OnBtnFlipVert() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_pPageManager->m_pIPropPageObject == NULL
	||  m_fInUpdateControls == TRUE )
	{
		return;
	}

	m_pPageManager->m_PropCurve.m_dwChanged = CHGD_FLIPVERT;
	m_pPageManager->m_pIPropPageObject->SetData( &m_pPageManager->m_PropCurve );
}


/////////////////////////////////////////////////////////////////////////////
// CPropPageCurve::OnDoubleClickedBtnFlipVert

void CPropPageCurve::OnDoubleClickedBtnFlipVert() 
{
	OnBtnFlipVert();
}


/////////////////////////////////////////////////////////////////////////////
// CPropPageCurve::OnBtnFlipHorz

void CPropPageCurve::OnBtnFlipHorz() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_pPageManager->m_pIPropPageObject == NULL
	||  m_fInUpdateControls == TRUE )
	{
		return;
	}

	m_pPageManager->m_PropCurve.m_dwChanged = CHGD_FLIPHORZ;
	m_pPageManager->m_pIPropPageObject->SetData( &m_pPageManager->m_PropCurve );
}


/////////////////////////////////////////////////////////////////////////////
// CPropPageCurve::OnDoubleClickedBtnFlipHorz

void CPropPageCurve::OnDoubleClickedBtnFlipHorz() 
{
	OnBtnFlipHorz();
}


/////////////////////////////////////////////////////////////////////////////
// CPropPageCurve::OnKillFocusStartValue

void CPropPageCurve::OnKillFocusStartValue() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_pPageManager->m_pIPropPageObject == NULL
	||  m_fInUpdateControls == TRUE 
	||  m_fInOnKillFocusStartValue == TRUE )
	{
		return;
	}

	m_fInOnKillFocusStartValue = TRUE;

	if( m_pPageManager->m_PropCurve.m_bEventType == DMUS_CURVET_RPNCURVE
	&&	m_pPageManager->m_PropCurve.m_wParamType == 0 )
	{
		HandleKillFocusStartPBRange();
		m_fInOnKillFocusStartValue = FALSE;
		return;
	}

	// Get current start value
	int nCurStartValue = GetSpinStartValuePos();

	// Get text from edit control
	CString strNewStartValue;
	m_editStartValue.GetWindowText( strNewStartValue );
	strNewStartValue.TrimRight();
	strNewStartValue.TrimLeft();

	int nNewStartValue;

	// Handle unknown text
	if( _tcsncmp( strNewStartValue, achUnknown, 10 ) == 0 )
	{
		return;
	}
	// Handle empty text
	else if( strNewStartValue.IsEmpty() )
	{
		nNewStartValue = 0; // Set to 0
	}
	else
	{
		nNewStartValue = StringToStartEndValue( strNewStartValue );
		if( nNewStartValue == INVALID_CC_VALUE )
		{
			nNewStartValue = nCurStartValue;	// No change
		}
		else
		{
			// Get StartValue range
			int nMin;
			int nMax;
			if( m_pPageManager->m_PropCurve.m_bEventType == DMUS_CURVET_PBCURVE )
			{
				nMin = MIN_PB_DISP_VALUE;
				nMax = MAX_PB_DISP_VALUE;
			}
			else if( (m_pPageManager->m_PropCurve.m_bEventType == DMUS_CURVET_RPNCURVE)
				 ||  (m_pPageManager->m_PropCurve.m_bEventType == DMUS_CURVET_NRPNCURVE) )
			{
				nMin = MIN_RPN_VALUE;
				nMax = MAX_RPN_VALUE;
			}
			else
			{
				nMin = MIN_CC_VALUE;
				nMax = MAX_CC_VALUE;
			}

			// Make sure value is within range
			if( nNewStartValue < nMin )
			{
				nNewStartValue = nMin;
			}
			if( nNewStartValue > nMax )
			{
				nNewStartValue = nMax;
			}
		}
	}

	// Text in edit control will by synced in response to the SetData() call, if necessary

	if( nCurStartValue != nNewStartValue )
	{
		SetSpinStartValuePos( nNewStartValue );

		m_pPageManager->m_PropCurve.m_dwChanged = UNDT_STARTVAL;
		m_pPageManager->m_PropCurve.m_nStartValue = (short)nNewStartValue;
		m_pPageManager->m_pIPropPageObject->SetData( &m_pPageManager->m_PropCurve );
	}
	else
	{
		// Check if edit control's text needs to be updated
		CString strValue;
		CString strCurValue;
		StartEndValueToString( nNewStartValue, strValue );
		m_editStartValue.GetWindowText( strCurValue );
		if( strCurValue != strValue )
		{
			m_editStartValue.SetWindowText( strValue );
			// SetSel's needed to get caret to end of string
			m_editStartValue.SetSel( 0, -1 );
			m_editStartValue.SetSel( -1, -1 );
		}
	}

	m_fInOnKillFocusStartValue = FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CPropPageCurve::HandleKillFocusStartPBRange

void CPropPageCurve::HandleKillFocusStartPBRange() 
{
	// Get text from edit control
	CString strNewStartSemiTones, strNewStartCents;
	m_editStartValue.GetWindowText( strNewStartSemiTones );
	strNewStartSemiTones.TrimRight();
	strNewStartSemiTones.TrimLeft();
	m_editPBRStartCents.GetWindowText( strNewStartCents );
	strNewStartCents.TrimRight();
	strNewStartCents.TrimLeft();

	// Handle unknown text
	if( _tcsncmp( strNewStartSemiTones, achUnknown, 10 ) == 0
	&&	_tcsncmp( strNewStartCents, achUnknown, 10 ) == 0 )
	{
		return;
	}

	BOOL fSucceeded = FALSE;
	int nSemiTones = GetDlgItemInt( IDC_EDIT_STARTVALUE, &fSucceeded, FALSE );
	if( !fSucceeded )
	{
		nSemiTones = 0;
	}

	int nCents = GetDlgItemInt( IDC_EDIT_PBR_START_CENTS, &fSucceeded, FALSE );
	if( !fSucceeded )
	{
		nCents = 0;
	}

	nCents = min( 127, max( nCents, 0 ) );
	nSemiTones = min( 127, max( nSemiTones, 0 ) );

	int nNewStartValue = nSemiTones * 128 + nCents;

	if( m_pPageManager->m_PropCurve.m_nStartValue != nNewStartValue )
	{
		SetSpinStartValuePos( nNewStartValue );
		SetSpinStartCentsValuePos( nNewStartValue );

		// Text in edit control will by synced in response to the SetData() call, if necessary
		m_pPageManager->m_PropCurve.m_dwChanged = UNDT_STARTVAL;
		m_pPageManager->m_PropCurve.m_nStartValue = (short)nNewStartValue;
		m_pPageManager->m_pIPropPageObject->SetData( &m_pPageManager->m_PropCurve );
	}
	else
	{
		// Check if edit control's text needs to be updated
		CString strValue;
		CString strCurValue;
		StartEndValueToString( nNewStartValue, strValue );
		m_editStartValue.GetWindowText( strCurValue );
		if( strCurValue != strValue )
		{
			m_editStartValue.SetWindowText( strValue );
			// SetSel's needed to get caret to end of string
			m_editStartValue.SetSel( 0, -1 );
			m_editStartValue.SetSel( -1, -1 );
		}

		StartEndValueToCentsString( nNewStartValue, strValue );
		m_editPBRStartCents.GetWindowText( strCurValue );
		if( strCurValue != strValue )
		{
			m_editPBRStartCents.SetWindowText( strValue );
			// SetSel's needed to get caret to end of string
			m_editPBRStartCents.SetSel( 0, -1 );
			m_editPBRStartCents.SetSel( -1, -1 );
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CPropPageCurve::OnDeltaPosSpinStartValue

void CPropPageCurve::OnDeltaPosSpinStartValue(NMHDR* pNMHDR, LRESULT* pResult) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_pPageManager->m_pIPropPageObject == NULL
	||  m_fInUpdateControls == TRUE )
	{
		return;
	}

	// Just in case user clicked on spin control immediately after typing text
	OnKillFocusStartValue();

	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

	int nNewStartValue = GetSpinStartValuePos() + pNMUpDown->iDelta;

	// Get StartValue range
	int nMin;
	int nMax;
	if( m_pPageManager->m_PropCurve.m_bEventType == DMUS_CURVET_PBCURVE )
	{
		nMin = MIN_PB_DISP_VALUE;
		nMax = MAX_PB_DISP_VALUE;
	}
	else if( m_pPageManager->m_PropCurve.m_bEventType == DMUS_CURVET_RPNCURVE
		 &&  m_pPageManager->m_PropCurve.m_wParamType == 0 )
	{
		nMin = MIN_CC_VALUE;
		nMax = MAX_CC_VALUE;
	}
	else if( (m_pPageManager->m_PropCurve.m_bEventType == DMUS_CURVET_RPNCURVE)
		 ||  (m_pPageManager->m_PropCurve.m_bEventType == DMUS_CURVET_NRPNCURVE) )
	{
		nMin = MIN_RPN_VALUE;
		nMax = MAX_RPN_VALUE;
	}
	else
	{
		nMin = MIN_CC_VALUE;
		nMax = MAX_CC_VALUE;
	}

	// Make sure value is within range
	if( nNewStartValue < nMin )
	{
		nNewStartValue = nMin;
	}
	if( nNewStartValue > nMax )
	{
		nNewStartValue = nMax;
	}

	// If a PB Range value, multiply by 128
	if( m_pPageManager->m_PropCurve.m_bEventType == DMUS_CURVET_RPNCURVE
	&&	m_pPageManager->m_PropCurve.m_wParamType == 0 )
	{
		nNewStartValue *= 128;
	}

	CString strValue;

	StartEndValueToString( nNewStartValue, strValue );
	m_editStartValue.SetWindowText( strValue );
	SetSpinStartValuePos( nNewStartValue );

	m_pPageManager->m_PropCurve.m_dwChanged = CHGD_STARTVAL;
	if( m_pPageManager->m_PropCurve.m_bEventType == DMUS_CURVET_RPNCURVE
	&&	m_pPageManager->m_PropCurve.m_wParamType == 0 )
	{
		m_pPageManager->m_PropCurve.m_nStartValue &= 0x007F;
		m_pPageManager->m_PropCurve.m_nStartValue |= (short)nNewStartValue;
	}
	else
	{
		m_pPageManager->m_PropCurve.m_nStartValue = (short)nNewStartValue;
	}
	m_pPageManager->m_pIPropPageObject->SetData( &m_pPageManager->m_PropCurve );
	
	*pResult = 1;
}


/////////////////////////////////////////////////////////////////////////////
// CPropPageCurve::OnKillFocusEndValue

void CPropPageCurve::OnKillFocusEndValue() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_pPageManager->m_pIPropPageObject == NULL
	||  m_fInUpdateControls == TRUE 
	||  m_fInOnKillFocusEndValue == TRUE )
	{
		return;
	}

	m_fInOnKillFocusEndValue = TRUE;

	if( m_pPageManager->m_PropCurve.m_bEventType == DMUS_CURVET_RPNCURVE
	&&	m_pPageManager->m_PropCurve.m_wParamType == 0 )
	{
		HandleKillFocusEndPBRange();
		m_fInOnKillFocusEndValue = FALSE;
		return;
	}

	// Get current end value
	int nCurEndValue = GetSpinEndValuePos();

	// Get text from edit control
	CString strNewEndValue;
	m_editEndValue.GetWindowText( strNewEndValue );
	strNewEndValue.TrimRight();
	strNewEndValue.TrimLeft();

	int nNewEndValue;

	// Handle unknown text
	if( _tcsncmp( strNewEndValue, achUnknown, 10 ) == 0 )
	{
		return;
	}
	// Handle empty text
	else if( strNewEndValue.IsEmpty() )
	{
		nNewEndValue = 0; // Set to 0
	}
	else
	{
		nNewEndValue = StringToStartEndValue( strNewEndValue );
		if( nNewEndValue == INVALID_CC_VALUE )
		{
			nNewEndValue = nCurEndValue;	// No change
		}
		else
		{
			// Get EndValue range
			int nMin;
			int nMax;
			if( m_pPageManager->m_PropCurve.m_bEventType == DMUS_CURVET_PBCURVE )
			{
				nMin = MIN_PB_DISP_VALUE;
				nMax = MAX_PB_DISP_VALUE;
			}
			else if( (m_pPageManager->m_PropCurve.m_bEventType == DMUS_CURVET_RPNCURVE)
				 ||  (m_pPageManager->m_PropCurve.m_bEventType == DMUS_CURVET_NRPNCURVE) )
			{
				nMin = MIN_RPN_VALUE;
				nMax = MAX_RPN_VALUE;
			}
			else
			{
				nMin = MIN_CC_VALUE;
				nMax = MAX_CC_VALUE;
			}

			// Make sure value is within range
			if( nNewEndValue < nMin )
			{
				nNewEndValue = nMin;
			}
			if( nNewEndValue > nMax )
			{
				nNewEndValue = nMax;
			}
		}
	}

	// Text in edit control will by synced in response to the SetData() call, if necessary

	if( nCurEndValue != nNewEndValue )
	{
		SetSpinEndValuePos( nNewEndValue );

		m_pPageManager->m_PropCurve.m_dwChanged = CHGD_ENDVAL;
		m_pPageManager->m_PropCurve.m_nEndValue = (short)nNewEndValue;
		m_pPageManager->m_pIPropPageObject->SetData( &m_pPageManager->m_PropCurve );
	}
	else
	{
		// Check if edit control's text needs to be updated
		CString strValue;
		CString strCurValue;
		StartEndValueToString( nNewEndValue, strValue );
		m_editEndValue.GetWindowText( strCurValue );
		if( strCurValue != strValue )
		{
			m_editEndValue.SetWindowText( strValue );
			// SetSel's needed to get caret to end of string
			m_editEndValue.SetSel( 0, -1 );
			m_editEndValue.SetSel( -1, -1 );
		}
	}

	m_fInOnKillFocusEndValue = FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CPropPageCurve::HandleKillFocusEndPBRange

void CPropPageCurve::HandleKillFocusEndPBRange() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Get text from edit control
	CString strNewEndSemiTones, strNewEndCents;
	m_editEndValue.GetWindowText( strNewEndSemiTones );
	strNewEndSemiTones.TrimRight();
	strNewEndSemiTones.TrimLeft();
	m_editPBREndCents.GetWindowText( strNewEndCents );
	strNewEndCents.TrimRight();
	strNewEndCents.TrimLeft();

	// Handle unknown text
	if( _tcsncmp( strNewEndSemiTones, achUnknown, 10 ) == 0
	&&	_tcsncmp( strNewEndCents, achUnknown, 10 ) == 0 )
	{
		return;
	}

	BOOL fSucceeded = FALSE;
	int nSemiTones = GetDlgItemInt( IDC_EDIT_ENDVALUE, &fSucceeded, FALSE );
	if( !fSucceeded )
	{
		nSemiTones = 0;
	}

	int nCents = GetDlgItemInt( IDC_EDIT_PBR_END_CENTS, &fSucceeded, FALSE );
	if( !fSucceeded )
	{
		nCents = 0;
	}

	nCents = min( 127, max( nCents, 0 ) );
	nSemiTones = min( 127, max( nSemiTones, 0 ) );

	int nNewEndValue = nSemiTones * 128 + nCents;

	if( m_pPageManager->m_PropCurve.m_nEndValue != nNewEndValue )
	{
		SetSpinEndValuePos( nNewEndValue );
		SetSpinEndCentsValuePos( nNewEndValue );

		// Text in edit control will by synced in response to the SetData() call, if necessary
		m_pPageManager->m_PropCurve.m_dwChanged = UNDT_ENDVAL;
		m_pPageManager->m_PropCurve.m_nEndValue = (short)nNewEndValue;
		m_pPageManager->m_pIPropPageObject->SetData( &m_pPageManager->m_PropCurve );
	}
	else
	{
		// Check if edit control's text needs to be updated
		CString strValue;
		CString strCurValue;
		StartEndValueToString( nNewEndValue, strValue );
		m_editEndValue.GetWindowText( strCurValue );
		if( strCurValue != strValue )
		{
			m_editEndValue.SetWindowText( strValue );
			// SetSel's needed to get caret to end of string
			m_editEndValue.SetSel( 0, -1 );
			m_editEndValue.SetSel( -1, -1 );
		}

		StartEndValueToCentsString( nNewEndValue, strValue );
		m_editPBREndCents.GetWindowText( strCurValue );
		if( strCurValue != strValue )
		{
			m_editPBREndCents.SetWindowText( strValue );
			// SetSel's needed to get caret to end of string
			m_editPBREndCents.SetSel( 0, -1 );
			m_editPBREndCents.SetSel( -1, -1 );
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CPropPageCurve::OnDeltaPosSpinEndValue

void CPropPageCurve::OnDeltaPosSpinEndValue(NMHDR* pNMHDR, LRESULT* pResult) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_pPageManager->m_pIPropPageObject == NULL
	||  m_fInUpdateControls == TRUE )
	{
		return;
	}

	// Just in case user clicked on spin control immediately after typing text
	OnKillFocusEndValue();

	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

	int nNewEndValue = GetSpinEndValuePos() + pNMUpDown->iDelta;

	// Get EndValue range
	int nMin;
	int nMax;
	if( m_pPageManager->m_PropCurve.m_bEventType == DMUS_CURVET_PBCURVE )
	{
		nMin = MIN_PB_DISP_VALUE;
		nMax = MAX_PB_DISP_VALUE;
	}
	else if( m_pPageManager->m_PropCurve.m_bEventType == DMUS_CURVET_RPNCURVE
		 &&  m_pPageManager->m_PropCurve.m_wParamType == 0 )
	{
		nMin = MIN_CC_VALUE;
		nMax = MAX_CC_VALUE;
	}
	else if( (m_pPageManager->m_PropCurve.m_bEventType == DMUS_CURVET_RPNCURVE)
		 ||  (m_pPageManager->m_PropCurve.m_bEventType == DMUS_CURVET_NRPNCURVE) )
	{
		nMin = MIN_RPN_VALUE;
		nMax = MAX_RPN_VALUE;
	}
	else
	{
		nMin = MIN_CC_VALUE;
		nMax = MAX_CC_VALUE;
	}

	// Make sure value is within range
	if( nNewEndValue < nMin )
	{
		nNewEndValue = nMin;
	}
	if( nNewEndValue > nMax )
	{
		nNewEndValue = nMax;
	}

	// If a PB Range value, multiply by 128
	if( m_pPageManager->m_PropCurve.m_bEventType == DMUS_CURVET_RPNCURVE
	&&	m_pPageManager->m_PropCurve.m_wParamType == 0 )
	{
		nNewEndValue *= 128;
	}

	CString strValue;

	StartEndValueToString( nNewEndValue, strValue );
	m_editEndValue.SetWindowText( strValue );
	SetSpinEndValuePos( nNewEndValue );

	m_pPageManager->m_PropCurve.m_dwChanged = CHGD_ENDVAL;
	if( m_pPageManager->m_PropCurve.m_bEventType == DMUS_CURVET_RPNCURVE
	&&	m_pPageManager->m_PropCurve.m_wParamType == 0 )
	{
		m_pPageManager->m_PropCurve.m_nEndValue &= 0x007F;
		m_pPageManager->m_PropCurve.m_nEndValue |= (short)nNewEndValue;
	}
	else
	{
		m_pPageManager->m_PropCurve.m_nEndValue = (short)nNewEndValue;
	}
	m_pPageManager->m_pIPropPageObject->SetData( &m_pPageManager->m_PropCurve );
	
	*pResult = 1;
}


/////////////////////////////////////////////////////////////////////////////
// CPropPageCurve::OnChangeStartBar

void CPropPageCurve::OnChangeStartBar() 
{
	HandleEditChange(m_spinStartBar, CHGD_STARTBAR,
		m_pPageManager->m_PropCurve.m_lStartBar);
}


/////////////////////////////////////////////////////////////////////////////
// CPropPageCurve::OnKillFocusStartBar

void CPropPageCurve::OnKillFocusStartBar() 
{
	HandleKillFocus(m_spinStartBar, CHGD_STARTBAR,
		m_pPageManager->m_PropCurve.m_lStartBar);
}


/////////////////////////////////////////////////////////////////////////////
// CPropPageCurve::OnChangeStartBeat

void CPropPageCurve::OnChangeStartBeat() 
{
	HandleEditChange(m_spinStartBeat, CHGD_STARTBEAT,
		m_pPageManager->m_PropCurve.m_lStartBeat);
}


/////////////////////////////////////////////////////////////////////////////
// CPropPageCurve::OnKillFocusStartBeat

void CPropPageCurve::OnKillFocusStartBeat() 
{
	HandleKillFocus(m_spinStartBeat, CHGD_STARTBEAT,
		m_pPageManager->m_PropCurve.m_lStartBeat);
}


/////////////////////////////////////////////////////////////////////////////
// CPropPageCurve::OnChangeStartGrid

void CPropPageCurve::OnChangeStartGrid() 
{
	HandleEditChange(m_spinStartGrid, CHGD_STARTGRID,
		m_pPageManager->m_PropCurve.m_lStartGrid);
}


/////////////////////////////////////////////////////////////////////////////
// CPropPageCurve::OnKillFocusStartGrid

void CPropPageCurve::OnKillFocusStartGrid() 
{
	HandleKillFocus(m_spinStartGrid, CHGD_STARTGRID,
		m_pPageManager->m_PropCurve.m_lStartGrid);
}


/////////////////////////////////////////////////////////////////////////////
// CPropPageCurve::OnChangeStartTick

void CPropPageCurve::OnChangeStartTick() 
{
	HandleEditChange(m_spinStartTick, CHGD_STARTTICK,
		m_pPageManager->m_PropCurve.m_lStartTick);
}


/////////////////////////////////////////////////////////////////////////////
// CPropPageCurve::OnKillFocusStartTick

void CPropPageCurve::OnKillFocusStartTick() 
{
	HandleKillFocus(m_spinStartTick, CHGD_STARTTICK,
		m_pPageManager->m_PropCurve.m_lStartTick);
}


/////////////////////////////////////////////////////////////////////////////
// CPropPageCurve::OnChangeEndBar

void CPropPageCurve::OnChangeEndBar() 
{
	HandleEditChange(m_spinEndBar, CHGD_ENDBAR,
		m_pPageManager->m_PropCurve.m_lEndBar);
}


/////////////////////////////////////////////////////////////////////////////
// CPropPageCurve::OnKillFocusEndBar

void CPropPageCurve::OnKillFocusEndBar() 
{
	HandleKillFocus(m_spinEndBar, CHGD_ENDBAR,
		m_pPageManager->m_PropCurve.m_lEndBar);
}


/////////////////////////////////////////////////////////////////////////////
// CPropPageCurve::OnChangeEndBeat

void CPropPageCurve::OnChangeEndBeat() 
{
	HandleEditChange(m_spinEndBeat, CHGD_ENDBEAT,
		m_pPageManager->m_PropCurve.m_lEndBeat);
}


/////////////////////////////////////////////////////////////////////////////
// CPropPageCurve::OnKillFocusEndBeat

void CPropPageCurve::OnKillFocusEndBeat() 
{
	HandleKillFocus(m_spinEndBeat, CHGD_ENDBEAT,
		m_pPageManager->m_PropCurve.m_lEndBeat);
}


/////////////////////////////////////////////////////////////////////////////
// CPropPageCurve::OnChangeEndGrid

void CPropPageCurve::OnChangeEndGrid() 
{
	HandleEditChange(m_spinEndGrid, CHGD_ENDGRID,
		m_pPageManager->m_PropCurve.m_lEndGrid);
}


/////////////////////////////////////////////////////////////////////////////
// CPropPageCurve::OnKillFocusEndGrid

void CPropPageCurve::OnKillFocusEndGrid() 
{
	HandleKillFocus(m_spinEndGrid, CHGD_ENDGRID,
		m_pPageManager->m_PropCurve.m_lEndGrid);
}


/////////////////////////////////////////////////////////////////////////////
// CPropPageCurve::OnChangeEndTick

void CPropPageCurve::OnChangeEndTick() 
{
	HandleEditChange(m_spinEndTick, CHGD_ENDTICK,
		m_pPageManager->m_PropCurve.m_lEndTick);
}


/////////////////////////////////////////////////////////////////////////////
// CPropPageCurve::OnKillFocusEndTick

void CPropPageCurve::OnKillFocusEndTick() 
{
	HandleKillFocus(m_spinEndTick, CHGD_ENDTICK,
		m_pPageManager->m_PropCurve.m_lEndTick);
}


/////////////////////////////////////////////////////////////////////////////
// CPropPageCurve::OnChangeDurBar

void CPropPageCurve::OnChangeDurBar() 
{
	HandleEditChange(m_spinDurBar, CHGD_DURBAR,
		m_pPageManager->m_PropCurve.m_lDurBar);
}


/////////////////////////////////////////////////////////////////////////////
// CPropPageCurve::OnKillFocusDurBar

void CPropPageCurve::OnKillFocusDurBar() 
{
	HandleKillFocus(m_spinDurBar, CHGD_DURBAR,
		m_pPageManager->m_PropCurve.m_lDurBar);
}


/////////////////////////////////////////////////////////////////////////////
// CPropPageCurve::OnChangeDurBeat

void CPropPageCurve::OnChangeDurBeat() 
{
	HandleEditChange(m_spinDurBeat, CHGD_DURBEAT,
		m_pPageManager->m_PropCurve.m_lDurBeat);
}


/////////////////////////////////////////////////////////////////////////////
// CPropPageCurve::OnKillFocusDurBeat

void CPropPageCurve::OnKillFocusDurBeat() 
{
	HandleKillFocus(m_spinDurBeat, CHGD_DURBEAT,
		m_pPageManager->m_PropCurve.m_lDurBeat);
}


/////////////////////////////////////////////////////////////////////////////
// CPropPageCurve::OnChangeDurGrid

void CPropPageCurve::OnChangeDurGrid() 
{
	HandleEditChange(m_spinDurGrid, CHGD_DURGRID,
		m_pPageManager->m_PropCurve.m_lDurGrid);
}


/////////////////////////////////////////////////////////////////////////////
// CPropPageCurve::OnKillFocusDurGrid

void CPropPageCurve::OnKillFocusDurGrid() 
{
	HandleKillFocus(m_spinDurGrid, CHGD_DURGRID,
		m_pPageManager->m_PropCurve.m_lDurGrid);
}


/////////////////////////////////////////////////////////////////////////////
// CPropPageCurve::OnChangeDurTick

void CPropPageCurve::OnChangeDurTick() 
{
	HandleEditChange(m_spinDurTick, CHGD_DURTICK,
		m_pPageManager->m_PropCurve.m_lDurTick);
}


/////////////////////////////////////////////////////////////////////////////
// CPropPageCurve::OnKillFocusDurTick

void CPropPageCurve::OnKillFocusDurTick() 
{
	HandleKillFocus(m_spinDurTick, CHGD_DURTICK,
		m_pPageManager->m_PropCurve.m_lDurTick);
}


/////////////////////////////////////////////////////////////////////////////
// CPropPageCurve::StartEndValueToString

void CPropPageCurve::StartEndValueToString( int nValue, CString& strValue ) 
{
	strValue.Format( "%d", nValue );

	if( m_pPageManager
	&&  m_pPageManager->m_pIPropPageObject )
	{
		CCurveStrip* pCurveStrip = (CCurveStrip *)m_pPageManager->m_pIPropPageObject;

		if( pCurveStrip->m_bCCType == CCTYPE_PAN_CURVE_STRIP )
		{
			CString strTemp;

			if( nValue < 63 )
			{
				strTemp.LoadString( IDS_PAN_LEFT );
				strValue.Format( "%s%d", strTemp, (63 - nValue) );
			}
			else if( nValue > 63 )
			{
				strTemp.LoadString( IDS_PAN_RIGHT );
				strValue.Format( "%s%d", strTemp, (nValue - 63) );
			}
			else
			{
				strValue.LoadString( IDS_PAN_MID );
			} 
		}

		if( pCurveStrip->m_bCCType == CCTYPE_RPN_CURVE_STRIP
		&&	pCurveStrip->m_wRPNType == 0 )
		{
			strValue.Format( "%d", nValue/128 );
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CPropPageCurve::StringToStartEndValue

int CPropPageCurve::StringToStartEndValue( LPCTSTR pszNewValue )
{
	CString strNewValue = pszNewValue;
	int nLength = strNewValue.GetLength();

	int nNewValue = INVALID_CC_VALUE;

	if( m_pPageManager
	&&  m_pPageManager->m_pIPropPageObject )
	{
		CCurveStrip* pCurveStrip = (CCurveStrip *)m_pPageManager->m_pIPropPageObject;

		if( pCurveStrip->m_bCCType == CCTYPE_PAN_CURVE_STRIP )
		{
			CString strTemp;
			strTemp.LoadString( IDS_PAN_MID );

			if( strTemp.CompareNoCase( strNewValue) == 0 )
			{
				nNewValue = 63;
			}
			else
			{
				CString strMinus;
				strMinus.LoadString( IDS_MINUS_TEXT );

				CString strBalance = strNewValue.Left( 1 );
				CString strValue = strNewValue.Right( nLength - 1 );

				// 1st char of 'L' or negative number means LEFT
				strTemp.LoadString( IDS_PAN_LEFT);
				if( (strTemp.CompareNoCase( strBalance ) == 0)
				||  (strMinus.CompareNoCase( strBalance ) == 0) )
				{
					nNewValue = _ttoi( strValue );
					nNewValue = 63 - nNewValue;
				}
				else
				{
					// 1st char of 'R' or positive number means RIGHT
					strTemp.LoadString( IDS_PAN_RIGHT);
					if( strTemp.CompareNoCase( strBalance ) == 0 )
					{
						nNewValue = _ttoi( strValue );
						nNewValue = 63 + nNewValue;
					}
					else
					{
						nNewValue = _ttoi( strNewValue );
						nNewValue = 63 + nNewValue;
					}
				}
			}
		}
		else
		{
			nNewValue = _ttoi( strNewValue );
		}
	}

	return nNewValue;
}


/////////////////////////////////////////////////////////////////////////////
// CPropPageCurve::StartEndValueToCentsString

void CPropPageCurve::StartEndValueToCentsString( int nValue, CString& strValue ) 
{
	strValue.Format( "%d", nValue % 128 );
}

void CPropPageCurve::OnChangeEditEndValue() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_pPageManager->m_pIPropPageObject == NULL
	||  m_fInUpdateControls == TRUE )
	{
		return;
	}

	// Get text from edit control
	CString strNewEndValue;
	m_editEndValue.GetWindowText( strNewEndValue );
	strNewEndValue.TrimRight();
	strNewEndValue.TrimLeft();

	// Return if string is empty
	if( strNewEndValue.IsEmpty() )
	{
		return;
	}

	// Return if string equals minus sign
	CString strMinus;
	strMinus.LoadString( IDS_MINUS_TEXT );
	if( strMinus.CompareNoCase(strNewEndValue) == 0 )
	{
		return;
	}

	if( m_pPageManager
	&&  m_pPageManager->m_pIPropPageObject )
	{
		CCurveStrip* pCurveStrip = (CCurveStrip *)m_pPageManager->m_pIPropPageObject;

		if( pCurveStrip->m_bCCType == CCTYPE_PAN_CURVE_STRIP )
		{
			// Load strings
			CString strLeft;
			CString strRight;
			strLeft.LoadString( IDS_PAN_LEFT );
			strRight.LoadString( IDS_PAN_RIGHT );

			if( (strLeft.CompareNoCase(strNewEndValue) == 0)
			||  (strRight.CompareNoCase(strNewEndValue) == 0) )
			{
				return;
			}
		}
	}

	OnKillFocusEndValue();
}

void CPropPageCurve::OnChangeEditStartValue() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_pPageManager->m_pIPropPageObject == NULL
	||  m_fInUpdateControls == TRUE )
	{
		return;
	}

	// Get text from edit control
	CString strNewStartValue;
	m_editStartValue.GetWindowText( strNewStartValue );
	strNewStartValue.TrimRight();
	strNewStartValue.TrimLeft();

	// Return if string is empty
	if( strNewStartValue.IsEmpty() )
	{
		return;
	}

	// Return if string equals minus sign
	CString strMinus;
	strMinus.LoadString( IDS_MINUS_TEXT );
	if( strMinus.CompareNoCase(strNewStartValue) == 0 )
	{
		return;
	}

	if( m_pPageManager
	&&  m_pPageManager->m_pIPropPageObject )
	{
		CCurveStrip* pCurveStrip = (CCurveStrip *)m_pPageManager->m_pIPropPageObject;

		if( pCurveStrip->m_bCCType == CCTYPE_PAN_CURVE_STRIP )
		{
			// Load strings
			CString strLeft;
			CString strRight;
			strLeft.LoadString( IDS_PAN_LEFT );
			strRight.LoadString( IDS_PAN_RIGHT );

			if( (strLeft.CompareNoCase(strNewStartValue) == 0)
			||  (strRight.CompareNoCase(strNewStartValue) == 0) )
			{
				return;
			}
		}
	}

	OnKillFocusStartValue();
}


/////////////////////////////////////////////////////////////////////////////
// CPropPageCurve::HandleEditChange
//
// Generic handler for edit box value changes
void CPropPageCurve::HandleEditChange(CSpinButtonCtrl& spin,
										   DWORD dwChg,
										   long& lUpdateVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_pPageManager->m_pIPropPageObject == NULL
	||  m_fInUpdateControls == TRUE )
	{
		return;
	}

	if (!::IsWindow(spin.m_hWnd)) {
		return;
	}

	// Get text from edit control
	TCHAR tcstrTmp[DIALOG_LEN];
	CWnd* pEditCtrl = spin.GetBuddy();
	ASSERT(pEditCtrl != NULL);
	pEditCtrl->GetWindowText( tcstrTmp, DIALOG_LEN );

	// If empty, exit early
	if( tcstrTmp[0] == NULL )
	{
		return;
	}

	// Get StartBar range
	int nMin;
	int nMax;
	spin.GetRange( nMin, nMax );

	// Convert from text to an integer
	long lNewValue = _ttoi( tcstrTmp );

	// Ensure the value stays within bounds
	if ( lNewValue < nMin )
	{
		lNewValue = nMin;
		spin.SetPos( nMin );
	}
	else if (lNewValue > nMax)
	{
		lNewValue = nMax;
		spin.SetPos( nMax );
	}
	
	// If the value changed, or it was previously undetermined - update the selected PropCurves
	if( (m_pPageManager->m_PropCurve.m_dwUndetermined & dwChg) || (lNewValue != lUpdateVal) )
	{
		m_pPageManager->m_PropCurve.m_dwChanged = dwChg;
		lUpdateVal = lNewValue;
		m_pPageManager->m_pIPropPageObject->SetData( &m_pPageManager->m_PropCurve );
	}
}

/////////////////////////////////////////////////////////////////////////////
// CPropPageCurve::HandleKillFocus
//
// Generic handler for edit box kill focus events.
void CPropPageCurve::HandleKillFocus(CSpinButtonCtrl& spin,
									 DWORD dwChg,
									 long& lUpdateVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_pPageManager->m_pIPropPageObject == NULL
	||  m_fInUpdateControls == TRUE )
	{
		return;
	}

	if (!::IsWindow(spin.m_hWnd)) {
		return;
	}

	// Get StartBar range
	int nMin;
	int nMax;
	spin.GetRange( nMin, nMax );

	// Get text from edit control
	TCHAR tcstrTmp[DIALOG_LEN];
	CWnd* pEditCtrl = spin.GetBuddy();
	ASSERT(pEditCtrl != NULL);
	pEditCtrl->GetWindowText( tcstrTmp, DIALOG_LEN );

	// Handle unknown text
	if( _tcsncmp( tcstrTmp, achUnknown, 10 ) == 0 )
	{
		return;
	}
	// Handle empty text
	else if( tcstrTmp[0] == NULL )
	{
		// Set it back to the minimum value
		spin.SetPos( min( nMin, 0) );

		if( (m_pPageManager->m_PropCurve.m_dwUndetermined & dwChg) || (nMin != lUpdateVal) )
		{
			m_pPageManager->m_PropCurve.m_dwChanged = dwChg;
			lUpdateVal = min( nMin, 0);
			m_pPageManager->m_pIPropPageObject->SetData( &m_pPageManager->m_PropCurve );
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CPropPageCurve::OnChangeEditMergeIndex
//
void CPropPageCurve::OnChangeEditMergeIndex() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_pPageManager->m_pIPropPageObject == NULL
	||  m_fInUpdateControls == TRUE )
	{
		return;
	}

	if (!::IsWindow(m_spinMergeIndex.m_hWnd))
	{
		return;
	}

	// Get text from edit control
	TCHAR tcstrTmp[DIALOG_LEN];
	m_editMergeIndex.GetWindowText( tcstrTmp, DIALOG_LEN );

	// If empty, exit early
	if( tcstrTmp[0] == NULL )
	{
		return;
	}

	// Get range
	int nMin;
	int nMax;
	m_spinMergeIndex.GetRange( nMin, nMax );

	// Convert from text to an integer
	long lNewValue = _ttoi( tcstrTmp );

	// Ensure the value stays within bounds
	if ( lNewValue < nMin )
	{
		lNewValue = nMin;
		m_spinMergeIndex.SetPos( nMin );
	}
	else if (lNewValue > nMax)
	{
		lNewValue = nMax;
		m_spinMergeIndex.SetPos( nMax );
	}
	
	// If the value changed, or it was previously undetermined - update the selected PropCurves
	if( (m_pPageManager->m_PropCurve.m_dwUndetermined2 & UNDT2_MERGEINDEX) || ((unsigned)lNewValue != m_pPageManager->m_PropCurve.m_wMergeIndex) )
	{
		m_pPageManager->m_PropCurve.m_dwChanged2 = UNDT2_MERGEINDEX;
		m_pPageManager->m_PropCurve.m_wMergeIndex = WORD(lNewValue);
		m_pPageManager->m_pIPropPageObject->SetData( &m_pPageManager->m_PropCurve );
	}
}

/////////////////////////////////////////////////////////////////////////////
// CPropPageCurve::OnKillfocusEditMergeIndex
//
void CPropPageCurve::OnKillfocusEditMergeIndex() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_pPageManager->m_pIPropPageObject == NULL
	||  m_fInUpdateControls == TRUE )
	{
		return;
	}

	if (!::IsWindow(m_spinMergeIndex.m_hWnd)) {
		return;
	}

	// Get range
	int nMin;
	int nMax;
	m_spinMergeIndex.GetRange( nMin, nMax );

	// Get text from edit control
	TCHAR tcstrTmp[DIALOG_LEN];
	m_editMergeIndex.GetWindowText( tcstrTmp, DIALOG_LEN );

	// Handle unknown text
	if( _tcsncmp( tcstrTmp, achUnknown, 10 ) == 0 )
	{
		return;
	}
	// Handle empty text
	else if( tcstrTmp[0] == NULL )
	{
		// Set it back to the minimum value
		m_spinMergeIndex.SetPos( min( nMin, 0) );

		if( (m_pPageManager->m_PropCurve.m_dwUndetermined2 & UNDT2_MERGEINDEX) || ((unsigned)nMin != m_pPageManager->m_PropCurve.m_wMergeIndex) )
		{
			m_pPageManager->m_PropCurve.m_dwChanged2 = CHGD2_MERGEINDEX;
			m_pPageManager->m_PropCurve.m_wMergeIndex = (WORD)min( nMin, 0);
			m_pPageManager->m_pIPropPageObject->SetData( &m_pPageManager->m_PropCurve );
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CPropPageCurve::OnCheckStartCurrent
//
void CPropPageCurve::OnCheckStartCurrent() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_pPageManager->m_pIPropPageObject == NULL
	||  m_fInUpdateControls == TRUE )
	{
		return;
	}

	int nCurState = m_checkStartCurrent.GetCheck();

	switch( nCurState )
	{
	case 0:
		m_checkStartCurrent.SetCheck( 1 );
		m_pPageManager->m_PropCurve.m_bFlags = DMUS_CURVE_START_FROM_CURRENT;
		break;
	case 1:
	case 2:
		m_checkStartCurrent.SetCheck( 0 );
		m_pPageManager->m_PropCurve.m_bFlags = 0;
		break;
	default:
		ASSERT(FALSE);
		break;
	}

	m_pPageManager->m_PropCurve.m_dwChanged2 = CHGD2_STARTCURRENT;
	m_pPageManager->m_pIPropPageObject->SetData( &m_pPageManager->m_PropCurve );
}

void CPropPageCurve::OnChangeEditPbrStartCents() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_pPageManager->m_pIPropPageObject == NULL
	||  m_fInUpdateControls == TRUE )
	{
		return;
	}

	// Get text from edit control
	CString strNewStartValue;
	m_editPBRStartCents.GetWindowText( strNewStartValue );
	strNewStartValue.TrimRight();
	strNewStartValue.TrimLeft();

	// Return if string is empty
	if( strNewStartValue.IsEmpty() )
	{
		return;
	}

	OnKillfocusEditPbrStartCents();
}

void CPropPageCurve::OnKillfocusEditPbrStartCents() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_pPageManager->m_pIPropPageObject == NULL
	||  m_fInUpdateControls == TRUE 
	||  m_fInOnKillFocusStartValue == TRUE )
	{
		return;
	}

	m_fInOnKillFocusStartValue = TRUE;
	HandleKillFocusStartPBRange();
	m_fInOnKillFocusStartValue = FALSE;
}

void CPropPageCurve::OnChangeEditPbrEndCents() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_pPageManager->m_pIPropPageObject == NULL
	||  m_fInUpdateControls == TRUE )
	{
		return;
	}

	// Get text from edit control
	CString strNewEndValue;
	m_editPBREndCents.GetWindowText( strNewEndValue );
	strNewEndValue.TrimRight();
	strNewEndValue.TrimLeft();

	// Return if string is empty
	if( strNewEndValue.IsEmpty() )
	{
		return;
	}

	OnKillfocusEditPbrEndCents();
}

void CPropPageCurve::OnKillfocusEditPbrEndCents() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_pPageManager->m_pIPropPageObject == NULL
	||  m_fInUpdateControls == TRUE 
	||  m_fInOnKillFocusEndValue == TRUE )
	{
		return;
	}

	m_fInOnKillFocusEndValue = TRUE;
	HandleKillFocusEndPBRange();
	m_fInOnKillFocusEndValue = FALSE;
}

void CPropPageCurve::OnDeltaposSpinPbrEndCents(NMHDR* pNMHDR, LRESULT* pResult) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_pPageManager->m_pIPropPageObject == NULL
	||  m_fInUpdateControls == TRUE )
	{
		return;
	}

	// Just in case user clicked on spin control immediately after typing text
	OnKillfocusEditPbrEndCents();

	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

	int nNewEndValue = LOWORD(m_spinPBREndCents.GetPos()) + pNMUpDown->iDelta;

	// Make sure value is within range
	if( nNewEndValue < MIN_CC_VALUE )
	{
		nNewEndValue = MIN_CC_VALUE;
	}
	if( nNewEndValue > MAX_CC_VALUE )
	{
		nNewEndValue = MAX_CC_VALUE;
	}

	CString strValue;

	// It's ok to use nNewEndValue without the high-order value, since the value
	// is % 128 inside the conversion functions.
	StartEndValueToCentsString( nNewEndValue, strValue );
	m_editPBREndCents.SetWindowText( strValue );
	SetSpinEndCentsValuePos( nNewEndValue );

	m_pPageManager->m_PropCurve.m_dwChanged = CHGD_ENDVAL;
	m_pPageManager->m_PropCurve.m_nEndValue &= 0xFF80;
	m_pPageManager->m_PropCurve.m_nEndValue |= (short)nNewEndValue;
	m_pPageManager->m_pIPropPageObject->SetData( &m_pPageManager->m_PropCurve );
	
	*pResult = 1;
}

void CPropPageCurve::OnDeltaposSpinPbrStartCents(NMHDR* pNMHDR, LRESULT* pResult) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_pPageManager->m_pIPropPageObject == NULL
	||  m_fInUpdateControls == TRUE )
	{
		return;
	}

	// Just in case user clicked on spin control immediately after typing text
	OnKillfocusEditPbrStartCents();

	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

	int nNewStartValue = LOWORD(m_spinPBRStartCents.GetPos()) + pNMUpDown->iDelta;

	// Make sure value is within range
	if( nNewStartValue < MIN_CC_VALUE )
	{
		nNewStartValue = MIN_CC_VALUE;
	}
	if( nNewStartValue > MAX_CC_VALUE )
	{
		nNewStartValue = MAX_CC_VALUE;
	}

	CString strValue;

	// It's ok to use nNewStartValue without the high-order value, since the value
	// is % 128 inside the conversion functions.
	StartEndValueToCentsString( nNewStartValue, strValue );
	m_editPBRStartCents.SetWindowText( strValue );
	SetSpinStartCentsValuePos( nNewStartValue );

	m_pPageManager->m_PropCurve.m_dwChanged = CHGD_STARTVAL;
	m_pPageManager->m_PropCurve.m_nStartValue &= 0xFF80;
	m_pPageManager->m_PropCurve.m_nStartValue |= (short)nNewStartValue;
	m_pPageManager->m_pIPropPageObject->SetData( &m_pPageManager->m_PropCurve );
	
	*pResult = 1;
}
