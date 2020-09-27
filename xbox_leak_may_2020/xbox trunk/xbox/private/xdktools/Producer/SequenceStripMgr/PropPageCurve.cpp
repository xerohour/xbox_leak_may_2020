// PropPageCurve.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "SequenceStripMgrApp.h"
#include "SequenceMgr.h"
#include "CurvePropPageMgr.h"
#include "CurveStrip.h"
#include "PropCurve.h"
#include "PropPageCurve.h"
#include "PropPageCurveReset.h"
#include "SharedPianoRoll.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define SMALL_BUFFER	32
#define DIALOG_LEN		20

static const TCHAR achUnknown[11] = "----------";

short CCurvePropPageMgr::sm_nActiveTab = 0;


//////////////////////////////////////////////////////////////////////
// CCurvePropPageMgr Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCurvePropPageMgr::CCurvePropPageMgr( REFGUID rGUIDManager )
{
	m_pTabCurve = NULL;
	m_pTabCurveReset = NULL;
	m_GUIDManager = rGUIDManager;
}

CCurvePropPageMgr::~CCurvePropPageMgr()
{
	if( m_pTabCurve )
	{
		delete m_pTabCurve;
	}
	if (m_pTabCurveReset) {
		
		delete m_pTabCurveReset;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CCurvePropPageMgr::RemoveCurrentObject

void CCurvePropPageMgr::RemoveCurrentObject( void )
{
	if( m_pIPropPageObject == NULL )
	{
		return;
	}

	m_pIPropPageObject->OnRemoveFromPageManager();
	m_pIPropPageObject = NULL;
}


/////////////////////////////////////////////////////////////////////////////
// CCurvePropPageMgr IDMUSProdPropPageManager::GetPropertySheetTitle

HRESULT CCurvePropPageMgr::GetPropertySheetTitle( BSTR* pbstrTitle, BOOL* pfAddPropertiesText )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( (pbstrTitle == NULL)
	||  (pfAddPropertiesText == NULL) )
	{
		return E_POINTER;
	}

	// Get "Curve" text
	CString strCurve;
	strCurve.LoadString( IDS_CURVE_TEXT );

	// Format title
	CString strTitle = strCurve;
	
	if( m_pIPropPageObject )
	{
		CCurveStrip* pCurveStrip = (CCurveStrip *)m_pIPropPageObject;

		if( pCurveStrip->m_pSequenceStrip )
		{
			CString strCCType;

			if( pCurveStrip->m_bCCType != CCTYPE_MINIMIZED_CURVE_STRIP )
			{
				// Check if this is a known RPN curve strip
				if( (pCurveStrip->m_bCCType == CCTYPE_RPN_CURVE_STRIP)
				&&	(pCurveStrip->m_wRPNType < 5) )
				{
					// Yes - retrieve the RPN name
					CString strTemp;
					strTemp.LoadString( pCurveStrip->m_wRPNType + IDS_RPN_PITCHBEND );
					AfxExtractSubString( strCCType, strTemp, 0, '\n' );
				}
				// Check if this an NRPN or unknown RPN curve strip.
				else if( (pCurveStrip->m_bCCType == CCTYPE_RPN_CURVE_STRIP)
					 ||  (pCurveStrip->m_bCCType == CCTYPE_NRPN_CURVE_STRIP) )
				{
					// Yes - format the RPN/NRPN name

					// Load the text to display for this CC type
					CString strTemp;
					strTemp.LoadString( pCurveStrip->m_bCCType + IDS_ControlChange0 );
					AfxExtractSubString( strCCType, strTemp, 1, '\n' );

					// Add the number to the text to display
					strTemp = strCCType;
					strCCType.Format( strTemp, int(pCurveStrip->m_wRPNType) );
				}
				else
				{
					// No - just use the CC name
					CString str1;
					CString str2;

					str1.LoadString( IDS_ControlChange0 + pCurveStrip->m_bCCType );
					AfxExtractSubString( str2, str1, 0, '\n' );
					strCCType = str2;
				}
			}

			// Put together the title
			strTitle = strCCType +
					   _T(" ") +
					   strCurve;
		}
	}

	*pbstrTitle = strTitle.AllocSysString();
	*pfAddPropertiesText = TRUE;
		
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CCurvePropPageMgr IDMUSProdPropPageManager::GetPropertySheetPages

HRESULT CCurvePropPageMgr::GetPropertySheetPages( IDMUSProdPropSheet* pIPropSheet, LONG* hPropSheetPage[], short* pnNbrPages )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( (hPropSheetPage == NULL)
	||  (pnNbrPages == NULL) )
	{
		return E_POINTER;
	}

	if( pIPropSheet == NULL )
	{
		return E_INVALIDARG;
	}

	m_pIPropSheet = pIPropSheet;
	m_pIPropSheet->AddRef();

	hPropSheetPage[0] = NULL;
	*pnNbrPages = 0;

	HPROPSHEETPAGE hPage;
	short nNbrPages = 0;

	// Add Curve tab
	m_pTabCurve = new CPropPageCurve( this );
	if( m_pTabCurve )
	{
		hPage = ::CreatePropertySheetPage( (LPCPROPSHEETPAGE)&m_pTabCurve->m_psp );
		if( hPage )
		{
			hPropSheetPage[nNbrPages] = (LONG *)hPage;
			nNbrPages++;
		}			
	}

	// Add Reset tab
	m_pTabCurveReset = new CPropPageCurveReset(this);
	if (m_pTabCurveReset) {
		hPage = ::CreatePropertySheetPage( (LPCPROPSHEETPAGE)&m_pTabCurveReset->m_psp );
		if( hPage )
		{
			hPropSheetPage[nNbrPages] = (LONG *)hPage;
			nNbrPages++;
		}			
	}


	// Set number of pages
	*pnNbrPages = nNbrPages;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CCurvePropPageMgr IDMUSProdPropPageManager::OnRemoveFromPropertySheet

HRESULT CCurvePropPageMgr::OnRemoveFromPropertySheet()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_pIPropSheet )
	{
		m_pIPropSheet->GetActivePage( &CCurvePropPageMgr::sm_nActiveTab );
	}

	CStaticPropPageManager::OnRemoveFromPropertySheet();

	theApp.m_pIPageManager = NULL;

	Release();	// delete myself

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CCurvePropPageMgr IDMUSProdPropPageManager::RefreshData

HRESULT CCurvePropPageMgr::RefreshData( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	CPropCurve propcurve;
	CPropCurve* pPropCurve = &propcurve;

	if( ( m_pIPropPageObject == NULL )
	||  ( m_pIPropPageObject->GetData( (void **)&pPropCurve ) != S_OK ) )
	{
		pPropCurve = NULL;
	}

	// Make sure changes to current Curve are processed in OnKillFocus
	// messages before setting the new Curve(s)
	CWnd* pWndHadFocus = CWnd::GetFocus();
	CWnd* pWnd = pWndHadFocus;
	CWnd* pWndParent = m_pTabCurve->GetParent();

	while( pWnd )
	{
		if( pWnd == pWndParent )
		{
			::SetFocus( NULL );
			break;
		}
		pWnd = pWnd->GetParent();
	}

	m_PropCurve.Copy( pPropCurve );

	// Set the curve type
	if( m_pIPropPageObject )
	{
		CCurveStrip* pCurveStrip = (CCurveStrip *)m_pIPropPageObject;
		m_PropCurve.m_bEventType = StripCCTypeToCurveType( pCurveStrip->m_bCCType );
		m_PropCurve.m_bCCData = pCurveStrip->m_bCCType;
		m_PropCurve.m_wParamType = pCurveStrip->m_wRPNType;
	}
	else
	{
		m_PropCurve.m_bEventType = 0;
		m_PropCurve.m_bCCData = 0;
		m_PropCurve.m_wParamType = 0;
	}

	// Set Property tabs to display the new Curve(s)
	m_pTabCurve->UpdateControls( pPropCurve );
	m_pTabCurveReset->UpdateControls( pPropCurve );

	// Restore focus
	if( pWndHadFocus
	&&  pWndHadFocus != CWnd::GetFocus() )
	{
		pWndHadFocus->SetFocus();
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CCurvePropPageMgr IDMUSProdPropPageManager::SetObject

HRESULT CCurvePropPageMgr::SetObject( IDMUSProdPropPageObject* pINewPropPageObject )
{
	if( pINewPropPageObject == NULL )
	{
		return E_INVALIDARG;
	}

	if( IsEqualObject( pINewPropPageObject ) != S_OK )
	{
		RemoveCurrentObject();

		m_pIPropPageObject = pINewPropPageObject;
//		m_pIPropPageObject->AddRef();		intentionally missing

		if( m_pIPropSheet )
		{
			m_pIPropSheet->RefreshTitle();
		}
	}

	RefreshData();

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
// CPropPageCurve::SetControlRanges

void CPropPageCurve::SetControlRanges( CPropCurve* pPropCurve )
{
	SetStartDurRanges( pPropCurve );
	SetEndRanges( pPropCurve );
	SetChordRanges( pPropCurve );

	// 'Start' and 'End' value controls
	if( pPropCurve->m_bEventType == DMUS_CURVET_PBCURVE )
	{
		m_editStartValue.LimitText( 5 );
		m_spinStartValue.SetRange( MIN_PB_VALUE, MAX_PB_VALUE );

		m_editEndValue.LimitText( 5 );
		m_spinEndValue.SetRange( MIN_PB_VALUE, MAX_PB_VALUE );
	}
	else if( pPropCurve->m_bEventType == DMUS_CURVET_RPNCURVE
		 &&  pPropCurve->m_wParamType == 0 )
	{
		m_editStartValue.LimitText( 3 );
		m_spinStartValue.SetRange( MIN_CC_VALUE, MAX_CC_VALUE );
		m_editPBRStartCents.LimitText( 3 );
		m_spinPBRStartCents.SetRange( MIN_CC_VALUE, MAX_CC_VALUE );

		m_editEndValue.LimitText( 3 );
		m_spinEndValue.SetRange( MIN_CC_VALUE, MAX_CC_VALUE );
		m_editPBREndCents.LimitText( 3 );
		m_spinPBREndCents.SetRange( MIN_CC_VALUE, MAX_CC_VALUE );
	}
	else if( (pPropCurve->m_bEventType == DMUS_CURVET_RPNCURVE)
		 ||  (pPropCurve->m_bEventType == DMUS_CURVET_NRPNCURVE) )
	{
		m_editStartValue.LimitText( 5 );
		m_spinStartValue.SetRange( MIN_RPN_VALUE, MAX_RPN_VALUE );

		m_editEndValue.LimitText( 5 );
		m_spinEndValue.SetRange( MIN_RPN_VALUE, MAX_RPN_VALUE );
	}
	else
	{
		m_editStartValue.LimitText( 3 );
		m_spinStartValue.SetRange( MIN_CC_VALUE, MAX_CC_VALUE );

		m_editEndValue.LimitText( 3 );
		m_spinEndValue.SetRange( MIN_CC_VALUE, MAX_CC_VALUE );
	}

	// 'Merge index' control
	if( pPropCurve->m_bEventType == DMUS_CURVET_PBCURVE
	|| (pPropCurve->m_bEventType == DMUS_CURVET_CCCURVE
		&& (pPropCurve->m_bCCData == 7 // Volume
			|| pPropCurve->m_bCCData == 11 // Expression
			|| pPropCurve->m_bCCData == 1 // Mod wheel
			|| pPropCurve->m_bCCData == 91 // Reverb send
			|| pPropCurve->m_bCCData == 93)) ) // Chorus send
	{
		m_editMergeIndex.LimitText( 3 );
		m_spinMergeIndex.SetRange( MIN_MERGE_INDEX, MAX_MERGE_INDEX );
	}
}


void CPropPageCurve::DoDataExchange(CDataExchange* pDX)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPropPageCurve)
	DDX_Control(pDX, IDC_STATIC_PBR_SEMITONES, m_staticPBRSemitones);
	DDX_Control(pDX, IDC_STATIC_PBR_CENTS, m_staticPBRCents);
	DDX_Control(pDX, IDC_SPIN_PBR_START_CENTS, m_spinPBRStartCents);
	DDX_Control(pDX, IDC_EDIT_PBR_START_CENTS, m_editPBRStartCents);
	DDX_Control(pDX, IDC_SPIN_PBR_END_CENTS, m_spinPBREndCents);
	DDX_Control(pDX, IDC_EDIT_PBR_END_CENTS, m_editPBREndCents);
	DDX_Control(pDX, IDC_STATIC_MERGE_INDEX, m_staticMergeIndex);
	DDX_Control(pDX, IDC_SPIN_MERGE_INDEX, m_spinMergeIndex);
	DDX_Control(pDX, IDC_EDIT_MERGE_INDEX, m_editMergeIndex);
	DDX_Control(pDX, IDC_CHECK_START_CURRENT, m_checkStartCurrent);
	DDX_Control(pDX, IDC_SPIN_STARTVALUE, m_spinStartValue);
	DDX_Control(pDX, IDC_SPIN_STARTTICK, m_spinStartTick);
	DDX_Control(pDX, IDC_SPIN_STARTGRID, m_spinStartGrid);
	DDX_Control(pDX, IDC_SPIN_STARTBEAT, m_spinStartBeat);
	DDX_Control(pDX, IDC_SPIN_STARTBAR, m_spinStartBar);
	DDX_Control(pDX, IDC_SPIN_ENDVALUE, m_spinEndValue);
	DDX_Control(pDX, IDC_SPIN_ENDTICK, m_spinEndTick);
	DDX_Control(pDX, IDC_SPIN_ENDGRID, m_spinEndGrid);
	DDX_Control(pDX, IDC_SPIN_ENDBEAT, m_spinEndBeat);
	DDX_Control(pDX, IDC_SPIN_ENDBAR, m_spinEndBar);
	DDX_Control(pDX, IDC_SPIN_DURTICK, m_spinDurTick);
	DDX_Control(pDX, IDC_SPIN_DURGRID, m_spinDurGrid);
	DDX_Control(pDX, IDC_SPIN_DURBEAT, m_spinDurBeat);
	DDX_Control(pDX, IDC_SPIN_DURBAR, m_spinDurBar);
	DDX_Control(pDX, IDC_EDIT_STARTVALUE, m_editStartValue);
	DDX_Control(pDX, IDC_EDIT_STARTTICK, m_editStartTick);
	DDX_Control(pDX, IDC_EDIT_STARTGRID, m_editStartGrid);
	DDX_Control(pDX, IDC_EDIT_STARTBEAT, m_editStartBeat);
	DDX_Control(pDX, IDC_EDIT_STARTBAR, m_editStartBar);
	DDX_Control(pDX, IDC_EDIT_ENDVALUE, m_editEndValue);
	DDX_Control(pDX, IDC_EDIT_ENDTICK, m_editEndTick);
	DDX_Control(pDX, IDC_EDIT_ENDGRID, m_editEndGrid);
	DDX_Control(pDX, IDC_EDIT_ENDBEAT, m_editEndBeat);
	DDX_Control(pDX, IDC_EDIT_ENDBAR, m_editEndBar);
	DDX_Control(pDX, IDC_EDIT_DURTICK, m_editDurTick);
	DDX_Control(pDX, IDC_EDIT_DURGRID, m_editDurGrid);
	DDX_Control(pDX, IDC_EDIT_DURBEAT, m_editDurBeat);
	DDX_Control(pDX, IDC_EDIT_DURBAR, m_editDurBar);
	DDX_Control(pDX, IDC_COMBO_SHAPE, m_comboShape);
	DDX_Control(pDX, IDC_BTN_FLIPVERT, m_btnFlipVert);
	DDX_Control(pDX, IDC_BTN_FLIPHORZ, m_btnFlipHorz);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPropPageCurve, CPropertyPage)
	//{{AFX_MSG_MAP(CPropPageCurve)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_CBN_SELCHANGE(IDC_COMBO_SHAPE, OnSelChangeComboShape)
	ON_BN_CLICKED(IDC_BTN_FLIPVERT, OnBtnFlipVert)
	ON_BN_DOUBLECLICKED(IDC_BTN_FLIPVERT, OnDoubleClickedBtnFlipVert)
	ON_BN_CLICKED(IDC_BTN_FLIPHORZ, OnBtnFlipHorz)
	ON_BN_DOUBLECLICKED(IDC_BTN_FLIPHORZ, OnDoubleClickedBtnFlipHorz)
	ON_EN_KILLFOCUS(IDC_EDIT_STARTVALUE, OnKillFocusStartValue)
	ON_EN_KILLFOCUS(IDC_EDIT_ENDVALUE, OnKillFocusEndValue)
	ON_EN_CHANGE(IDC_EDIT_STARTBAR, OnChangeStartBar)
	ON_EN_KILLFOCUS(IDC_EDIT_STARTBAR, OnKillFocusStartBar)
	ON_EN_CHANGE(IDC_EDIT_STARTBEAT, OnChangeStartBeat)
	ON_EN_KILLFOCUS(IDC_EDIT_STARTBEAT, OnKillFocusStartBeat)
	ON_EN_CHANGE(IDC_EDIT_STARTGRID, OnChangeStartGrid)
	ON_EN_KILLFOCUS(IDC_EDIT_STARTGRID, OnKillFocusStartGrid)
	ON_EN_CHANGE(IDC_EDIT_STARTTICK, OnChangeStartTick)
	ON_EN_KILLFOCUS(IDC_EDIT_STARTTICK, OnKillFocusStartTick)
	ON_EN_CHANGE(IDC_EDIT_ENDBAR, OnChangeEndBar)
	ON_EN_KILLFOCUS(IDC_EDIT_ENDBAR, OnKillFocusEndBar)
	ON_EN_CHANGE(IDC_EDIT_ENDBEAT, OnChangeEndBeat)
	ON_EN_KILLFOCUS(IDC_EDIT_ENDBEAT, OnKillFocusEndBeat)
	ON_EN_CHANGE(IDC_EDIT_ENDGRID, OnChangeEndGrid)
	ON_EN_KILLFOCUS(IDC_EDIT_ENDGRID, OnKillFocusEndGrid)
	ON_EN_CHANGE(IDC_EDIT_ENDTICK, OnChangeEndTick)
	ON_EN_KILLFOCUS(IDC_EDIT_ENDTICK, OnKillFocusEndTick)
	ON_EN_CHANGE(IDC_EDIT_DURBAR, OnChangeDurBar)
	ON_EN_KILLFOCUS(IDC_EDIT_DURBAR, OnKillFocusDurBar)
	ON_EN_CHANGE(IDC_EDIT_DURBEAT, OnChangeDurBeat)
	ON_EN_KILLFOCUS(IDC_EDIT_DURBEAT, OnKillFocusDurBeat)
	ON_EN_CHANGE(IDC_EDIT_DURGRID, OnChangeDurGrid)
	ON_EN_KILLFOCUS(IDC_EDIT_DURGRID, OnKillFocusDurGrid)
	ON_EN_CHANGE(IDC_EDIT_DURTICK, OnChangeDurTick)
	ON_EN_KILLFOCUS(IDC_EDIT_DURTICK, OnKillFocusDurTick)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_ENDVALUE, OnDeltaPosSpinEndValue)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_STARTVALUE, OnDeltaPosSpinStartValue)
	ON_EN_CHANGE(IDC_EDIT_ENDVALUE, OnChangeEditEndValue)
	ON_EN_CHANGE(IDC_EDIT_STARTVALUE, OnChangeEditStartValue)
	ON_EN_CHANGE(IDC_EDIT_MERGE_INDEX, OnChangeEditMergeIndex)
	ON_EN_KILLFOCUS(IDC_EDIT_MERGE_INDEX, OnKillfocusEditMergeIndex)
	ON_BN_CLICKED(IDC_CHECK_START_CURRENT, OnCheckStartCurrent)
	ON_EN_CHANGE(IDC_EDIT_PBR_START_CENTS, OnChangeEditPbrStartCents)
	ON_EN_KILLFOCUS(IDC_EDIT_PBR_START_CENTS, OnKillfocusEditPbrStartCents)
	ON_EN_CHANGE(IDC_EDIT_PBR_END_CENTS, OnChangeEditPbrEndCents)
	ON_EN_KILLFOCUS(IDC_EDIT_PBR_END_CENTS, OnKillfocusEditPbrEndCents)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_PBR_END_CENTS, OnDeltaposSpinPbrEndCents)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_PBR_START_CENTS, OnDeltaposSpinPbrStartCents)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropPageCurve message handlers

void CPropPageCurve::SetStartDurRanges( CPropCurve* pPropCurve )
{
	// Compute Start time max/min values
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
	m_spinStartTick.SetRange( lMinStartClocks - 1, lMaxStartClocks + 1 );
	SetItemWidth(IDC_NOTE_EDIT_STARTTICK, lTemp);
	m_spinDurTick.SetRange( lMinStartClocks - 1, lMaxStartClocks + 1 );
	SetItemWidth(IDC_NOTE_EDIT_DURTICK, lTemp);

	/// GRIDS
	cstrTemp.Format("%d",lMaxStartGrids);
	lTemp = cstrTemp.GetLength();
	m_spinStartGrid.SetRange(0, lMaxStartGrids+1);
	SetItemWidth(IDC_NOTE_EDIT_STARTGRID, lTemp);

	cstrTemp.Format("%d",lMaxStartGrids - 1);
	lTemp = cstrTemp.GetLength();
	m_spinDurGrid.SetRange(-1, lMaxStartGrids);
	SetItemWidth(IDC_NOTE_EDIT_DURGRID, lTemp);

	/// BEATS
	cstrTemp.Format("%d",lMaxStartBeats);
	lTemp = cstrTemp.GetLength();
	m_spinStartBeat.SetRange(0, lMaxStartBeats+1);
	SetItemWidth(IDC_NOTE_EDIT_STARTBEAT, lTemp);

	cstrTemp.Format("%d",lMaxStartBeats - 1);
	lTemp = cstrTemp.GetLength();
	m_spinDurBeat.SetRange(-1, lMaxStartBeats);
	SetItemWidth(IDC_NOTE_EDIT_DURBEAT, lTemp);

	/// BARS
	cstrTemp.Format("%d",999);
	lTemp = cstrTemp.GetLength();
	m_spinStartBar.SetRange(0, 999);	// We can have pick-up notes
	SetItemWidth(IDC_NOTE_EDIT_STARTBAR, lTemp);
	m_spinDurBar.SetRange(0, 999);
	SetItemWidth(IDC_NOTE_EDIT_DURBAR, lTemp);
}

void CPropPageCurve::SetEndRanges( CPropCurve* pPropCurve )
{
	// Compute End time max/min values
	DMUS_TIMESIGNATURE ts;
	pPropCurve->GetTimeSig( pPropCurve->AbsTime() + pPropCurve->m_mtDuration, &ts );

	long lGridClocks = ((DMUS_PPQ * 4) / ts.bBeat) / ts.wGridsPerBeat;
	long lMaxEndBeats, lMaxEndGrids, lMinEndClocks, lMaxEndClocks;
	lMaxEndBeats = ts.bBeatsPerMeasure;
	lMaxEndGrids = ts.wGridsPerBeat;
	lMinEndClocks = -(lGridClocks / 2);
	lMaxEndClocks = lGridClocks + lMinEndClocks - 1;

	/// TICKS
	long	lTemp;
	CString	cstrTemp;
	cstrTemp.Format("%d",lMinEndClocks);
	lTemp = cstrTemp.GetLength();
	m_spinEndTick.SetRange( lMinEndClocks - 1, lMaxEndClocks + 1 );
	SetItemWidth(IDC_NOTE_EDIT_ENDTICK, lTemp);

	/// GRIDS
	cstrTemp.Format("%d",lMaxEndGrids);
	lTemp = cstrTemp.GetLength();
	m_spinEndGrid.SetRange(0, lMaxEndGrids+1);
	SetItemWidth(IDC_NOTE_EDIT_ENDGRID, lTemp);

	/// BEATS
	cstrTemp.Format("%d",lMaxEndBeats);
	lTemp = cstrTemp.GetLength();
	m_spinEndBeat.SetRange(0, lMaxEndBeats+1);
	SetItemWidth(IDC_NOTE_EDIT_ENDBEAT, lTemp);

	/// BARS
	cstrTemp.Format("%d",999);
	lTemp = cstrTemp.GetLength();
	m_spinEndBar.SetRange(0, 999 + 1);	// We can have pick-up notes
	SetItemWidth(IDC_NOTE_EDIT_ENDBAR, lTemp);
}

void CPropPageCurve::SetChordRanges( CPropCurve* pPropCurve )
{
	/*
	// Compute Chord time max values
	DMUS_TIMESIGNATURE ts;
	pPropCurve->GetTimeSig( pPropCurve->MusicTime(), &ts );

	long lMaxStartBeats = ts.bBeatsPerMeasure;

	/// BEATS
	long	lTemp;
	CString	cstrTemp;
	cstrTemp.Format("%d",lMaxStartBeats);
	lTemp = cstrTemp.GetLength();
	m_spinChordBeat.SetRange(0, lMaxStartBeats);
	SetItemWidth(IDC_NOTE_EDIT_CHORDBEAT, lTemp);

	/// BARS
	cstrTemp.Format("%d",999);
	lTemp = cstrTemp.GetLength();
	m_spinChordBar.SetRange(1, 999);
	SetItemWidth(IDC_NOTE_EDIT_CHORDBAR, lTemp);
	*/
}

void CPropPageCurve::SetItemWidth( int nItem, int nWidth)
{
	CEdit* pEdit;
	pEdit = (CEdit *) GetDlgItem(nItem);
	if ((pEdit != NULL) && (nWidth >=0))
	{
		pEdit->LimitText( nWidth );
	}
}
