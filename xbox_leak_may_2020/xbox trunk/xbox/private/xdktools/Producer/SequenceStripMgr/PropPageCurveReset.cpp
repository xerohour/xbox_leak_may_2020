// PropPageCurveReset.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "Sequencestripmgr.h"
#include "CurvePropPageMgr.h"
#include "PropCurve.h"
#include "PropPageCurveReset.h"
#include "CurveStrip.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define DIALOG_LEN				20

static const TCHAR achUnknown[11] = "----------";

/////////////////////////////////////////////////////////////////////////////
// CPropPageCurveReset property page

//IMPLEMENT_DYNCREATE(CPropPageCurveReset, CPropertyPage)

CPropPageCurveReset::CPropPageCurveReset(CCurvePropPageMgr* pCurvePropPageMgr) : CPropertyPage(CPropPageCurveReset::IDD)
{
	//{{AFX_DATA_INIT(CPropPageCurveReset)
	//}}AFX_DATA_INIT

	ASSERT( pCurvePropPageMgr != NULL );

	m_pPageManager = pCurvePropPageMgr;
	m_fInUpdateControls = FALSE;
	m_fNeedToDetach = FALSE;
	m_fInOnKillFocusResetValue = FALSE;
}

CPropPageCurveReset::~CPropPageCurveReset()
{
}

/////////////////////////////////////////////////////////////////////////////
// CPropPageCurveReset::EnableItem

void CPropPageCurveReset::EnableItem( int nID, BOOL fEnable )
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
// CPropPageCurveReset::EnableControls

void CPropPageCurveReset::EnableControls( BOOL fEnable ) 
{
	m_checkEnableReset.EnableWindow(fEnable);

	if (m_checkEnableReset.GetCheck() == 0) {

		// the rest of the items are only enabled if
		// enable reset is checked / undeterminate.
		fEnable = FALSE;
	}
	else if (fEnable == FALSE) {
		// if controls are being disabled, make sure the checkbox
		// is unchecked.
		m_checkEnableReset.SetCheck(0);
	}
		
	m_spinResetValue.EnableWindow(fEnable);
	EnableItem(IDC_EDIT_RESETVALUE, fEnable);

	EnableItem( IDC_EDIT_DURATIONBAR, fEnable );
	m_spinDurationBar.EnableWindow( fEnable );
	EnableItem( IDC_EDIT_DURATIONBEAT, fEnable );
	m_spinDurationBeat.EnableWindow( fEnable );
	EnableItem( IDC_EDIT_DURATIONGRID, fEnable );
	m_spinDurationGrid.EnableWindow( fEnable );
	EnableItem( IDC_EDIT_DURATIONTICK, fEnable );
	m_spinDurationTick.EnableWindow( fEnable );

	if( m_pPageManager
	&&	m_pPageManager->m_PropCurve.m_bEventType == DMUS_CURVET_RPNCURVE
	&&	m_pPageManager->m_PropCurve.m_wParamType == 0 )
	{
		m_staticPBRSemitones.ShowWindow( SW_NORMAL );
		m_staticPBRCents.ShowWindow( SW_NORMAL );
		m_spinPBRResetCents.ShowWindow( SW_NORMAL );
		m_editPBRResetCents.ShowWindow( SW_NORMAL );


		EnableItem( IDC_EDIT_PBR_RESET_CENTS, fEnable );
		m_spinPBRResetCents.EnableWindow( fEnable );
	}
	else
	{
		m_staticPBRSemitones.ShowWindow( SW_HIDE );
		m_staticPBRCents.ShowWindow( SW_HIDE );
		m_spinPBRResetCents.ShowWindow( SW_HIDE );
		m_editPBRResetCents.ShowWindow( SW_HIDE );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CPropPageCurveReset::SetSpinResetValuePos

void CPropPageCurveReset::SetSpinResetValuePos( int nResetValue )
{
	if( m_pPageManager->m_PropCurve.m_bEventType == DMUS_CURVET_PBCURVE )
	{
		m_spinResetValue.SetPos( nResetValue + PB_DISP_OFFSET );
	}
	else if( m_pPageManager->m_PropCurve.m_bEventType == DMUS_CURVET_RPNCURVE
		&&	 m_pPageManager->m_PropCurve.m_wParamType == 0 )
	{
		m_spinResetValue.SetPos( nResetValue / 128 );
	}
	else
	{
		m_spinResetValue.SetPos( nResetValue );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CPropPageCurveReset::SetSpinResetCentsValuePos

void CPropPageCurveReset::SetSpinResetCentsValuePos( int nResetValue )
{
	m_spinPBRResetCents.SetPos( nResetValue % 128 );
}


/////////////////////////////////////////////////////////////////////////////
// CPropPageCurveReset::GetSpinResetValuePos

int CPropPageCurveReset::GetSpinResetValuePos( void )
{
	int nResetValue;

	if( m_pPageManager->m_PropCurve.m_bEventType == DMUS_CURVET_PBCURVE )
	{
		nResetValue = LOWORD(m_spinResetValue.GetPos()) - PB_DISP_OFFSET;
	}
	else
	{
		nResetValue = LOWORD(m_spinResetValue.GetPos());
	}

	return nResetValue;
}


/////////////////////////////////////////////////////////////////////////////
// CPropPageCurveReset::SetControlRanges

void CPropPageCurveReset::SetControlRanges( CPropCurve* pPropCurve )
{
	// Compute Reset duration time max/min values
	DMUS_TIMESIGNATURE ts;
	pPropCurve->GetTimeSig( pPropCurve->AbsTime(), &ts );

	long lGridClocks = ((DMUS_PPQ * 4) / ts.bBeat) / ts.wGridsPerBeat;
	long lMaxStartBeats, lMaxStartGrids, lMinStartClocks, lMaxStartClocks;
	lMaxStartBeats = ts.bBeatsPerMeasure;
	lMaxStartGrids = ts.wGridsPerBeat;
	lMinStartClocks = -(lGridClocks / 2);
	lMaxStartClocks = lGridClocks + lMinStartClocks - 1;

	/// TICKS
	long	lTemp;
	CString	cstrTemp;
	cstrTemp.Format("%d",lMinStartClocks);
	lTemp = cstrTemp.GetLength();
	m_spinDurationTick.SetRange( lMinStartClocks, lMaxStartClocks );
	m_editDurationTick.SetLimitText( lTemp );

	/// GRIDS
	cstrTemp.Format("%d",lMaxStartGrids - 1);
	lTemp = cstrTemp.GetLength();
	m_spinDurationGrid.SetRange(-1, lMaxStartGrids);
	m_editDurationGrid.SetLimitText( lTemp );

	/// BEATS
	cstrTemp.Format("%d",lMaxStartBeats - 1);
	lTemp = cstrTemp.GetLength();
	m_spinDurationBeat.SetRange(-1, lMaxStartBeats);
	m_editDurationBeat.SetLimitText( lTemp );

	/// BARS
	cstrTemp.Format("%d",999);
	lTemp = cstrTemp.GetLength();
	m_spinDurationBar.SetRange(0, 999);
	m_editDurationBar.SetLimitText( lTemp );

	// Reset value control
	if( pPropCurve->m_bEventType == DMUS_CURVET_PBCURVE )
	{
		m_editResetValue.LimitText( 5 );
		m_spinResetValue.SetRange( MIN_PB_VALUE, MAX_PB_VALUE );
	}
	else if( pPropCurve->m_bEventType == DMUS_CURVET_RPNCURVE
		 &&  pPropCurve->m_wParamType == 0 )
	{
		m_editResetValue.LimitText( 3 );
		m_spinResetValue.SetRange( MIN_CC_VALUE, MAX_CC_VALUE );
		m_editPBRResetCents.LimitText( 3 );
		m_spinPBRResetCents.SetRange( MIN_CC_VALUE, MAX_CC_VALUE );
	}
	else if( (pPropCurve->m_bEventType == DMUS_CURVET_RPNCURVE)
		 ||  (pPropCurve->m_bEventType == DMUS_CURVET_NRPNCURVE) )
	{
		m_editResetValue.LimitText( 5 );
		m_spinResetValue.SetRange( MIN_RPN_VALUE, MAX_RPN_VALUE );
	}
	else
	{
		m_editResetValue.LimitText( 3 );
		m_spinResetValue.SetRange( MIN_CC_VALUE, MAX_CC_VALUE );
	}
}

/////////////////////////////////////////////////////////////////////////////
// CPropPageCurveReset::UpdateEdit

void CPropPageCurveReset::UpdateEdit(CPropCurve* pPropCurve,
									 CSpinButtonCtrl& spin,
									 long lValue,
									 DWORD dwUndt)
{
	if( pPropCurve->m_dwUndetermined & dwUndt )
	{
		CWnd* pEditCtrl = spin.GetBuddy();
		ASSERT(pEditCtrl != NULL);
		pEditCtrl->SetWindowText( achUnknown );
	}
	else
	{
		int nPos = spin.GetPos();
		if( HIWORD(nPos) != 0
		||  LOWORD(nPos) != lValue )
		{
			spin.SetPos( lValue );
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CPropPageCurveReset::UpdateControls

void CPropPageCurveReset::UpdateControls( CPropCurve* pPropCurve )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Make sure controls have been created
	if( ::IsWindow(m_checkEnableReset.m_hWnd) == FALSE )
	{
		return;
	}

	m_fInUpdateControls = TRUE;
	
	// Update controls
	if( pPropCurve == NULL )
	{
		EnableControls( FALSE );
		
		m_fInUpdateControls = FALSE;
		return;
	}

	SetControlRanges( pPropCurve );

	// enable reset check box
	if (pPropCurve->m_dwUndetermined & UNDT_RESETENABLE) {
		// undeterminate state
		m_checkEnableReset.SetCheck(2);
	}
	else {
		m_checkEnableReset.SetCheck( 
			(pPropCurve->m_bFlags & DMUS_CURVE_RESET) ? 1 : 0);
	}
	
	UpdateEdit(pPropCurve, m_spinDurationBar, pPropCurve->m_lResetBar, UNDT_RESETBAR);
	UpdateEdit(pPropCurve, m_spinDurationBeat, pPropCurve->m_lResetBeat, UNDT_RESETBEAT);
	UpdateEdit(pPropCurve, m_spinDurationGrid, pPropCurve->m_lResetGrid, UNDT_RESETGRID);
	UpdateEdit(pPropCurve, m_spinDurationTick, pPropCurve->m_lResetTick, UNDT_RESETTICK);

	if( pPropCurve->m_dwUndetermined & UNDT_RESETVAL )
	{
		m_editResetValue.SetWindowText( achUnknown );
	}
	else
	{
		CString strValue;
		ResetValueToString( pPropCurve->m_nResetValue, strValue );

		CString strCurrentText;
		m_editResetValue.GetWindowText( strCurrentText );
		if( strValue != strCurrentText )
		{
			m_editResetValue.SetWindowText( strValue );
			// SetSel's needed to get caret to end of string
			m_editResetValue.SetSel( 0, -1 );
			m_editResetValue.SetSel( -1, -1 );

			SetSpinResetValuePos( pPropCurve->m_nResetValue ); 
		}
	}

	if( m_pPageManager
	&&	m_pPageManager->m_PropCurve.m_bEventType == DMUS_CURVET_RPNCURVE
	&&	m_pPageManager->m_PropCurve.m_wParamType == 0 )
	{
		// Reset value
		if( pPropCurve->m_dwUndetermined & UNDT_RESETVAL )
		{
			m_editPBRResetCents.SetWindowText( achUnknown );
		}
		else
		{
			CString strValue;
			ResetValueToCentsString( pPropCurve->m_nResetValue, strValue );

			CString strCurrentText;
			m_editPBRResetCents.GetWindowText( strCurrentText );
			if( strValue != strCurrentText )
			{
				m_editPBRResetCents.SetWindowText( strValue );
				// SetSel's needed to get caret to end of string
				m_editPBRResetCents.SetSel( 0, -1 );
				m_editPBRResetCents.SetSel( -1, -1 );

				SetSpinResetCentsValuePos( pPropCurve->m_nResetValue ); 
			}
		}
	}

	EnableControls(TRUE);

	m_fInUpdateControls = FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// CPropPageCurveReset::HandleEditChange
//
// Generic handler for edit box value changes
void CPropPageCurveReset::HandleEditChange(CSpinButtonCtrl& spin,
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
// CPropPageCurveReset::HandleKillFocus
//
// Generic handler for edit box kill focus events.
void CPropPageCurveReset::HandleKillFocus(CSpinButtonCtrl& spin,
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

void CPropPageCurveReset::DoDataExchange(CDataExchange* pDX)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPropPageCurveReset)
	DDX_Control(pDX, IDC_SPIN_PBR_RESET_CENTS, m_spinPBRResetCents);
	DDX_Control(pDX, IDC_EDIT_PBR_RESET_CENTS, m_editPBRResetCents);
	DDX_Control(pDX, IDC_STATIC_PBR_SEMITONES, m_staticPBRSemitones);
	DDX_Control(pDX, IDC_STATIC_PBR_CENTS, m_staticPBRCents);
	DDX_Control(pDX, IDC_EDIT_RESETVALUE, m_editResetValue);
	DDX_Control(pDX, IDC_EDIT_DURATIONTICK, m_editDurationTick);
	DDX_Control(pDX, IDC_EDIT_DURATIONGRID, m_editDurationGrid);
	DDX_Control(pDX, IDC_EDIT_DURATIONBEAT, m_editDurationBeat);
	DDX_Control(pDX, IDC_EDIT_DURATIONBAR, m_editDurationBar);
	DDX_Control(pDX, IDC_ENABLE_RESET, m_checkEnableReset);
	DDX_Control(pDX, IDC_SPIN_DURATIONTICK, m_spinDurationTick);
	DDX_Control(pDX, IDC_SPIN_DURATIONGRID, m_spinDurationGrid);
	DDX_Control(pDX, IDC_SPIN_DURATIONBEAT, m_spinDurationBeat);
	DDX_Control(pDX, IDC_SPIN_DURATIONBAR, m_spinDurationBar);
	DDX_Control(pDX, IDC_SPIN_RESETVALUE, m_spinResetValue);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPropPageCurveReset, CPropertyPage)
	//{{AFX_MSG_MAP(CPropPageCurveReset)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_EN_CHANGE(IDC_EDIT_DURATIONBAR, OnChangeDurationbar)
	ON_EN_KILLFOCUS(IDC_EDIT_DURATIONBAR, OnKillfocusDurationbar)
	ON_EN_CHANGE(IDC_EDIT_DURATIONBEAT, OnChangeDurationbeat)
	ON_EN_KILLFOCUS(IDC_EDIT_DURATIONBEAT, OnKillfocusDurationbeat)
	ON_EN_CHANGE(IDC_EDIT_DURATIONGRID, OnChangeDurationgrid)
	ON_EN_KILLFOCUS(IDC_EDIT_DURATIONGRID, OnKillfocusDurationgrid)
	ON_EN_CHANGE(IDC_EDIT_DURATIONTICK, OnChangeDurationtick)
	ON_EN_KILLFOCUS(IDC_EDIT_DURATIONTICK, OnKillfocusDurationtick)
	ON_EN_CHANGE(IDC_EDIT_RESETVALUE, OnChangeResetValue)
	ON_EN_KILLFOCUS(IDC_EDIT_RESETVALUE, OnKillfocusResetValue)
	ON_BN_CLICKED(IDC_ENABLE_RESET, OnEnableResetClicked)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_PBR_RESET_CENTS, OnDeltaPosSpinPbrResetCents)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_RESETVALUE, OnDeltaPosSpinResetValue)
	ON_EN_CHANGE(IDC_EDIT_PBR_RESET_CENTS, OnChangeEditPbrResetCents)
	ON_EN_KILLFOCUS(IDC_EDIT_PBR_RESET_CENTS, OnKillfocusEditPbrResetCents)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropPageCurveReset message handlers


int CPropPageCurveReset::OnCreate(LPCREATESTRUCT lpCreateStruct) 
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
	
	if (CPropertyPage::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	return 0;
}

void CPropPageCurveReset::OnDestroy() 
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

BOOL CPropPageCurveReset::OnSetActive() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Update controls
	m_pPageManager->RefreshData();

	// Store active tab
	m_pPageManager->m_pIPropSheet->GetActivePage( &CCurvePropPageMgr::sm_nActiveTab );
	
	return CPropertyPage::OnSetActive();
}


void CPropPageCurveReset::OnChangeDurationbar() 
{
	HandleEditChange(m_spinDurationBar, CHGD_RESETBAR,
		m_pPageManager->m_PropCurve.m_lResetBar);
}

void CPropPageCurveReset::OnKillfocusDurationbar() 
{
	HandleKillFocus(m_spinDurationBar, CHGD_RESETBAR,
		m_pPageManager->m_PropCurve.m_lResetBar);
}

void CPropPageCurveReset::OnChangeDurationbeat() 
{
	HandleEditChange(m_spinDurationBeat, CHGD_RESETBEAT,
		m_pPageManager->m_PropCurve.m_lResetBeat);
}

void CPropPageCurveReset::OnKillfocusDurationbeat() 
{
	HandleKillFocus(m_spinDurationBeat, CHGD_RESETBEAT,
		m_pPageManager->m_PropCurve.m_lResetBeat);
}

void CPropPageCurveReset::OnChangeDurationgrid() 
{
	HandleEditChange(m_spinDurationGrid, CHGD_RESETGRID,
		m_pPageManager->m_PropCurve.m_lResetGrid);
}

void CPropPageCurveReset::OnKillfocusDurationgrid() 
{
	HandleKillFocus(m_spinDurationGrid, CHGD_RESETGRID,
		m_pPageManager->m_PropCurve.m_lResetGrid);
}

void CPropPageCurveReset::OnChangeDurationtick() 
{
	HandleEditChange(m_spinDurationTick, CHGD_RESETTICK,
		m_pPageManager->m_PropCurve.m_lResetTick);
}

void CPropPageCurveReset::OnKillfocusDurationtick() 
{
	HandleKillFocus(m_spinDurationTick, CHGD_RESETTICK,
		m_pPageManager->m_PropCurve.m_lResetTick);
}

void CPropPageCurveReset::OnChangeResetValue() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_pPageManager->m_pIPropPageObject == NULL
	||	m_editResetValue.GetSafeHwnd() == NULL
	||  m_fInUpdateControls == TRUE )
	{
		return;
	}

	// Get text from edit control
	CString strNewResetValue;
	m_editResetValue.GetWindowText( strNewResetValue );
	strNewResetValue.TrimRight();
	strNewResetValue.TrimLeft();

	// Return if string is empty
	if( strNewResetValue.IsEmpty() )
	{
		return;
	}

	// Return if string equals minus sign
	CString strMinus;
	strMinus.LoadString( IDS_MINUS_TEXT );
	if( strMinus.CompareNoCase(strNewResetValue) == 0 )
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

			if( (strLeft.CompareNoCase(strNewResetValue) == 0)
			||  (strRight.CompareNoCase(strNewResetValue) == 0) )
			{
				return;
			}
		}
	}

	OnKillfocusResetValue();
}


/////////////////////////////////////////////////////////////////////////////
// CPropPageCurveReset::ResetValueToString

void CPropPageCurveReset::ResetValueToString( int nValue, CString& strValue ) 
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
// CPropPageCurveReset::StringToResetValue

int CPropPageCurveReset::StringToResetValue( LPCTSTR pszNewValue )
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
// CPropPageCurveReset::ResetValueToCentsString

void CPropPageCurveReset::ResetValueToCentsString( int nValue, CString& strValue ) 
{
	strValue.Format( "%d", nValue % 128 );
}


/////////////////////////////////////////////////////////////////////////////
// CPropPageCurveReset::OnDeltaPosSpinResetValue

void CPropPageCurveReset::OnDeltaPosSpinResetValue(NMHDR* pNMHDR, LRESULT* pResult) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_pPageManager->m_pIPropPageObject == NULL
	||  m_fInUpdateControls == TRUE )
	{
		return;
	}

	// Just in case user clicked on spin control immediately after typing text
	OnKillfocusResetValue();

	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

	int nNewResetValue = GetSpinResetValuePos() + pNMUpDown->iDelta;

	// Get ResetValue range
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
	if( nNewResetValue < nMin )
	{
		nNewResetValue = nMin;
	}
	if( nNewResetValue > nMax )
	{
		nNewResetValue = nMax;
	}

	// If a PB Range value, multiply by 128
	if( m_pPageManager->m_PropCurve.m_bEventType == DMUS_CURVET_RPNCURVE
	&&	m_pPageManager->m_PropCurve.m_wParamType == 0 )
	{
		nNewResetValue *= 128;
	}

	CString strValue;

	ResetValueToString( nNewResetValue, strValue );
	m_editResetValue.SetWindowText( strValue );
	SetSpinResetValuePos( nNewResetValue );

	m_pPageManager->m_PropCurve.m_dwChanged = CHGD_RESETVAL;
	if( m_pPageManager->m_PropCurve.m_bEventType == DMUS_CURVET_RPNCURVE
	&&	m_pPageManager->m_PropCurve.m_wParamType == 0 )
	{
		m_pPageManager->m_PropCurve.m_nResetValue &= 0x007F;
		m_pPageManager->m_PropCurve.m_nResetValue |= (short)nNewResetValue;
	}
	else
	{
		m_pPageManager->m_PropCurve.m_nResetValue = (short)nNewResetValue;
	}
	m_pPageManager->m_pIPropPageObject->SetData( &m_pPageManager->m_PropCurve );
	
	*pResult = 1;
}

void CPropPageCurveReset::OnKillfocusResetValue() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_pPageManager->m_pIPropPageObject == NULL
	||  m_fInUpdateControls == TRUE 
	||  m_fInOnKillFocusResetValue == TRUE )
	{
		return;
	}

	m_fInOnKillFocusResetValue = TRUE;

	if( m_pPageManager->m_PropCurve.m_bEventType == DMUS_CURVET_RPNCURVE
	&&	m_pPageManager->m_PropCurve.m_wParamType == 0 )
	{
		HandleKillFocusResetPBRange();
		m_fInOnKillFocusResetValue = FALSE;
		return;
	}

	// Get current reset value
	int nCurResetValue = GetSpinResetValuePos();

	// Get text from edit control
	CString strNewResetValue;
	m_editResetValue.GetWindowText( strNewResetValue );
	strNewResetValue.TrimRight();
	strNewResetValue.TrimLeft();

	int nNewResetValue;

	// Handle unknown text
	if( _tcsncmp( strNewResetValue, achUnknown, 10 ) == 0 )
	{
		return;
	}
	// Handle empty text
	else if( strNewResetValue.IsEmpty() )
	{
		nNewResetValue = 0; // Set to 0
	}
	else
	{
		nNewResetValue = StringToResetValue( strNewResetValue );
		if( nNewResetValue == INVALID_CC_VALUE )
		{
			nNewResetValue = nCurResetValue;	// No change
		}
		else
		{
			// Get ResetValue range
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
			if( nNewResetValue < nMin )
			{
				nNewResetValue = nMin;
			}
			if( nNewResetValue > nMax )
			{
				nNewResetValue = nMax;
			}
		}
	}

	// Text in edit control will by synced in response to the SetData() call, if necessary

	if( nCurResetValue != nNewResetValue )
	{
		SetSpinResetValuePos( nNewResetValue );

		m_pPageManager->m_PropCurve.m_dwChanged = UNDT_RESETVAL;
		m_pPageManager->m_PropCurve.m_nResetValue = (short)nNewResetValue;
		m_pPageManager->m_pIPropPageObject->SetData( &m_pPageManager->m_PropCurve );
	}
	else
	{
		// Check if edit control's text needs to be updated
		CString strValue;
		CString strCurValue;
		ResetValueToString( nNewResetValue, strValue );
		m_editResetValue.GetWindowText( strCurValue );
		if( strCurValue != strValue )
		{
			m_editResetValue.SetWindowText( strValue );
			// SetSel's needed to get caret to end of string
			m_editResetValue.SetSel( 0, -1 );
			m_editResetValue.SetSel( -1, -1 );
		}
	}

	m_fInOnKillFocusResetValue = FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CPropPageCurveReset::HandleKillFocusResetPBRange

void CPropPageCurveReset::HandleKillFocusResetPBRange() 
{
	// Get text from edit control
	CString strNewResetSemiTones, strNewResetCents;
	m_editResetValue.GetWindowText( strNewResetSemiTones );
	strNewResetSemiTones.TrimRight();
	strNewResetSemiTones.TrimLeft();
	m_editPBRResetCents.GetWindowText( strNewResetCents );
	strNewResetCents.TrimRight();
	strNewResetCents.TrimLeft();

	// Handle unknown text
	if( _tcsncmp( strNewResetSemiTones, achUnknown, 10 ) == 0
	&&	_tcsncmp( strNewResetCents, achUnknown, 10 ) == 0 )
	{
		return;
	}

	BOOL fSucceeded = FALSE;
	int nSemiTones = GetDlgItemInt( IDC_EDIT_RESETVALUE, &fSucceeded, FALSE );
	if( !fSucceeded )
	{
		nSemiTones = 0;
	}

	int nCents = GetDlgItemInt( IDC_EDIT_PBR_RESET_CENTS, &fSucceeded, FALSE );
	if( !fSucceeded )
	{
		nCents = 0;
	}

	nCents = min( 127, max( nCents, 0 ) );
	nSemiTones = min( 127, max( nSemiTones, 0 ) );

	int nNewResetValue = nSemiTones * 128 + nCents;

	if( m_pPageManager->m_PropCurve.m_nResetValue != nNewResetValue )
	{
		SetSpinResetValuePos( nNewResetValue );
		SetSpinResetCentsValuePos( nNewResetValue );

		// Text in edit control will by synced in response to the SetData() call, if necessary
		m_pPageManager->m_PropCurve.m_dwChanged = UNDT_RESETVAL;
		m_pPageManager->m_PropCurve.m_nResetValue = (short)nNewResetValue;
		m_pPageManager->m_pIPropPageObject->SetData( &m_pPageManager->m_PropCurve );
	}
	else
	{
		// Check if edit control's text needs to be updated
		CString strValue;
		CString strCurValue;
		ResetValueToString( nNewResetValue, strValue );
		m_editResetValue.GetWindowText( strCurValue );
		if( strCurValue != strValue )
		{
			m_editResetValue.SetWindowText( strValue );
			// SetSel's needed to get caret to end of string
			m_editResetValue.SetSel( 0, -1 );
			m_editResetValue.SetSel( -1, -1 );
		}

		ResetValueToCentsString( nNewResetValue, strValue );
		m_editPBRResetCents.GetWindowText( strCurValue );
		if( strCurValue != strValue )
		{
			m_editPBRResetCents.SetWindowText( strValue );
			// SetSel's needed to get caret to end of string
			m_editPBRResetCents.SetSel( 0, -1 );
			m_editPBRResetCents.SetSel( -1, -1 );
		}
	}
}

void CPropPageCurveReset::OnEnableResetClicked() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_pPageManager->m_pIPropPageObject == NULL
	||  m_fInUpdateControls == TRUE )
	{
		return;
	}

	int nCheck = m_checkEnableReset.GetCheck();

	m_pPageManager->m_PropCurve.m_dwChanged = CHGD_RESETENABLE;
	if( nCheck == 1 )
	{
		m_pPageManager->m_PropCurve.m_bFlags |= DMUS_CURVE_RESET;
	}
	else
	{
		m_pPageManager->m_PropCurve.m_bFlags &= ~DMUS_CURVE_RESET;
	}
	m_pPageManager->m_pIPropPageObject->SetData( &m_pPageManager->m_PropCurve );
}

void CPropPageCurveReset::OnChangeEditPbrResetCents() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_pPageManager->m_pIPropPageObject == NULL
	||  m_fInUpdateControls == TRUE )
	{
		return;
	}

	// Get text from edit control
	CString strNewResetValue;
	m_editPBRResetCents.GetWindowText( strNewResetValue );
	strNewResetValue.TrimRight();
	strNewResetValue.TrimLeft();

	// Return if string is empty
	if( strNewResetValue.IsEmpty() )
	{
		return;
	}

	OnKillfocusEditPbrResetCents();
}

void CPropPageCurveReset::OnKillfocusEditPbrResetCents() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_pPageManager->m_pIPropPageObject == NULL
	||  m_fInUpdateControls == TRUE 
	||  m_fInOnKillFocusResetValue == TRUE )
	{
		return;
	}

	m_fInOnKillFocusResetValue = TRUE;
	HandleKillFocusResetPBRange();
	m_fInOnKillFocusResetValue = FALSE;
}

void CPropPageCurveReset::OnDeltaPosSpinPbrResetCents(NMHDR* pNMHDR, LRESULT* pResult) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_pPageManager->m_pIPropPageObject == NULL
	||  m_fInUpdateControls == TRUE )
	{
		return;
	}

	// Just in case user clicked on spin control immediately after typing text
	OnKillfocusEditPbrResetCents();

	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

	int nNewResetValue = LOWORD(m_spinPBRResetCents.GetPos()) + pNMUpDown->iDelta;

	// Make sure value is within range
	if( nNewResetValue < MIN_CC_VALUE )
	{
		nNewResetValue = MIN_CC_VALUE;
	}
	if( nNewResetValue > MAX_CC_VALUE )
	{
		nNewResetValue = MAX_CC_VALUE;
	}

	CString strValue;

	// It's ok to use nNewResetValue without the high-order value, since the value
	// is % 128 inside the conversion functions.
	ResetValueToCentsString( nNewResetValue, strValue );
	m_editPBRResetCents.SetWindowText( strValue );
	SetSpinResetCentsValuePos( nNewResetValue );

	m_pPageManager->m_PropCurve.m_dwChanged = CHGD_RESETVAL;
	m_pPageManager->m_PropCurve.m_nResetValue &= 0xFF80;
	m_pPageManager->m_PropCurve.m_nResetValue |= (short)nNewResetValue;
	m_pPageManager->m_pIPropPageObject->SetData( &m_pPageManager->m_PropCurve );
	
	*pResult = 1;
}
