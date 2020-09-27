// PropPageCurve.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "ParamStripMgr.h"
#include "CurvePropPageMgr.h"
#include "PropCurve.h"
#include "PropPageCurve.h"
#include "TrackMgr.h"
#include "ParamStrip.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


static const TCHAR achUnknown[11] = "----------";

// {F6220A90-31FD-4f64-A477-41589A9764EE}
static const GUID GUID_ParamCurveItemPPGMgr = 
{ 0xf6220a90, 0x31fd, 0x4f64, { 0xa4, 0x77, 0x41, 0x58, 0x9a, 0x97, 0x64, 0xee } };


short CCurvePropPageMgr::sm_nActiveTab = 0;


//////////////////////////////////////////////////////////////////////
// CCurvePropPageMgr Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCurvePropPageMgr::CCurvePropPageMgr(IDMUSProdFramework* pIFramework, CParamStrip* pParamStrip)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Store Framework pointer
	ASSERT( pIFramework != NULL ); 
	m_pIFramework = pIFramework;
	m_pIFramework->AddRef();

	ASSERT(pParamStrip);
	m_pParamStrip = pParamStrip;

	m_pTrackMgr = pParamStrip->GetTrackManager();
	ASSERT( m_pTrackMgr != NULL ); 

	// Store GUID
	m_GUIDManager = GUID_ParamCurveItemPPGMgr;

	// Initialize our pointers to NULL
	m_pTabCurve = NULL;

	// call the base class contstructor
	CStaticPropPageManager::CStaticPropPageManager();
}

CCurvePropPageMgr::~CCurvePropPageMgr()
{
	// Tell the Param strip that the property page manager is going away
	if(m_pParamStrip)
	{
		m_pParamStrip->OnCurvePropPageMgrDeleted();
	}

	if( m_pTabCurve )
	{
		delete m_pTabCurve;
	}

	if(m_pIFramework)
	{
		m_pIFramework->Release();
		m_pIFramework = NULL;
	}

	m_pTrackMgr = NULL;

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

	if( (pbstrTitle == NULL) || (pfAddPropertiesText == NULL) )
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
		CParamStrip* pParamStrip = (CParamStrip *)m_pIPropPageObject;
		ASSERT(pParamStrip);
		CTrackMgr* pTrackMgr = pParamStrip->GetTrackManager();
		ASSERT(pTrackMgr);
		CTrackObject* pTrackObject = pParamStrip->GetTrackObject();
		ASSERT(pTrackObject);
		if(pTrackObject != NULL)
		{
			CString strObjectName = pTrackObject->GetName();
			CString strParam = pParamStrip->GetName();
			
			strTitle = 	strObjectName + "-" + strParam;
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

	// Set Property tabs to display the new Curve(s)
	m_pTabCurve->UpdateControls( pPropCurve );
	//m_PropCurve.Copy( pPropCurve );

	// Restore focus
	if( pWndHadFocus && pWndHadFocus != CWnd::GetFocus() )
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

void CPropPageCurve::DoDataExchange(CDataExchange* pDX)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPropPageCurve)
	DDX_Control(pDX, IDC_COMBO_ENUMSTARTVALUE, m_comboEnumStartValue);
	DDX_Control(pDX, IDC_COMBO_ENUMENDVALUE, m_comboEnumEndValue);
	DDX_Control(pDX, IDC_CHECK_START_FROM_CURRENT, m_btnStartFromCurrent);
	DDX_Control(pDX, IDC_PROMPT_TICK, m_staticPromptTick);
	DDX_Control(pDX, IDC_PROMPT_GRID, m_staticPromptGrid);
	DDX_Control(pDX, IDC_PROMPT_BEAT, m_staticPromptBeat);
	DDX_Control(pDX, IDC_PROMPT_BAR, m_staticPromptBar);
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
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_ENDVALUE, OnDeltaPosSpinEndValue)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_STARTVALUE, OnDeltaPosSpinStartValue)
	ON_EN_CHANGE(IDC_EDIT_ENDVALUE, OnChangeEditEndValue)
	ON_EN_CHANGE(IDC_EDIT_STARTVALUE, OnChangeEditStartValue)
	ON_EN_KILLFOCUS(IDC_EDIT_STARTVALUE, OnKillFocusStartValue)
	ON_EN_KILLFOCUS(IDC_EDIT_ENDVALUE, OnKillFocusEndValue)
	ON_EN_KILLFOCUS(IDC_EDIT_STARTBAR, OnKillfocusEditStartbar)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_STARTBAR, OnDeltaposSpinStartbar)
	ON_EN_KILLFOCUS(IDC_EDIT_STARTBEAT, OnKillfocusEditStartbeat)
	ON_EN_KILLFOCUS(IDC_EDIT_STARTGRID, OnKillfocusEditStartgrid)
	ON_EN_KILLFOCUS(IDC_EDIT_STARTTICK, OnKillfocusEditStarttick)
	ON_EN_KILLFOCUS(IDC_EDIT_ENDBAR, OnKillfocusEditEndbar)
	ON_EN_KILLFOCUS(IDC_EDIT_ENDBEAT, OnKillfocusEditEndbeat)
	ON_EN_KILLFOCUS(IDC_EDIT_ENDGRID, OnKillfocusEditEndgrid)
	ON_EN_KILLFOCUS(IDC_EDIT_ENDTICK, OnKillfocusEditEndtick)
	ON_EN_KILLFOCUS(IDC_EDIT_DURBAR, OnKillfocusEditDurbar)
	ON_EN_KILLFOCUS(IDC_EDIT_DURBEAT, OnKillfocusEditDurbeat)
	ON_EN_KILLFOCUS(IDC_EDIT_DURGRID, OnKillfocusEditDurgrid)
	ON_EN_KILLFOCUS(IDC_EDIT_DURTICK, OnKillfocusEditDurtick)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_STARTBEAT, OnDeltaposSpinStartbeat)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_STARTGRID, OnDeltaposSpinStartgrid)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_STARTTICK, OnDeltaposSpinStarttick)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_ENDBAR, OnDeltaposSpinEndbar)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_ENDBEAT, OnDeltaposSpinEndbeat)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_ENDGRID, OnDeltaposSpinEndgrid)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_ENDTICK, OnDeltaposSpinEndtick)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_DURBAR, OnDeltaposSpinDurbar)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_DURBEAT, OnDeltaposSpinDurbeat)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_DURGRID, OnDeltaposSpinDurgrid)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_DURTICK, OnDeltaposSpinDurtick)
	ON_BN_CLICKED(IDC_CHECK_START_FROM_CURRENT, OnCheckStartFromCurrent)
	ON_CBN_SELCHANGE(IDC_COMBO_ENUMENDVALUE, OnSelchangeComboEnumEndValue)
	ON_CBN_SELCHANGE(IDC_COMBO_ENUMSTARTVALUE, OnSelchangeComboEnumStartValue)
	ON_CBN_KILLFOCUS(IDC_COMBO_ENUMSTARTVALUE, OnKillfocusComboEnumStartValue)
	ON_CBN_KILLFOCUS(IDC_COMBO_ENUMENDVALUE, OnKillfocusComboEnumEndValue)
	ON_CBN_KILLFOCUS(IDC_COMBO_SHAPE, OnKillfocusComboShape)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


//////////////////////////////////////////////////////////////////////
// CPropPageCurve Construction/Destruction
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CPropPageCurve property page

CPropPageCurve::CPropPageCurve( CCurvePropPageMgr* pCurvePropPageMgr ) : CPropertyPage(CPropPageCurve::IDD)
{
	//{{AFX_DATA_INIT(CPropPageCurve)
	//}}AFX_DATA_INIT
	
	ASSERT( pCurvePropPageMgr != NULL );

	m_pPageManager = pCurvePropPageMgr;
	m_fInUpdateControls = FALSE;
	m_fNeedToDetach = FALSE;
	m_fInOnKillFocusStartValue = FALSE;
	m_fInOnKillFocusEndValue = FALSE;

	m_bFloatType = false;
	m_bIntType = false;
	m_bEnumType = false;
	m_bBoolType = false;
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
// CPropPageCurve::StartToUnknownTime

REFERENCE_TIME CPropPageCurve::StartToUnknownTime( void )
{
	CTrackMgr* pTrackMgr = m_pPageManager->m_pTrackMgr;;
	ASSERT(pTrackMgr);
	if(pTrackMgr == NULL)
	{
		return 0;
	}

	REFERENCE_TIME rtNewStart = 0;
	if( pTrackMgr->IsRefTimeTrack() )
	{
		REFERENCE_TIME rtTime;
		pTrackMgr->MinSecGridMsToRefTime( m_PropCurve.m_pParamStrip, m_lStartBar, m_lStartBeat, m_lStartGrid, m_lStartTick, &rtTime );
		pTrackMgr->RefTimeToUnknownTime( rtTime, &rtNewStart );
	}
	else
	{
		MUSIC_TIME mtTime;
		pTrackMgr->MeasureBeatGridTickToClocks( m_lStartBar, m_lStartBeat, m_lStartGrid, m_lStartTick, &mtTime );
		pTrackMgr->ClocksToUnknownTime( mtTime, &rtNewStart );
	}

	return rtNewStart;
}


/////////////////////////////////////////////////////////////////////////////
// CPropPageCurve::DurationToUnknownTime

REFERENCE_TIME CPropPageCurve::DurationToUnknownTime( void )
{
	CTrackMgr* pTrackMgr = m_pPageManager->m_pTrackMgr;;
	ASSERT(pTrackMgr);
	if(pTrackMgr == NULL)
	{
		return 0;
	}

	REFERENCE_TIME rtNewLength = 0;

	if( pTrackMgr->IsRefTimeTrack() )
	{
		REFERENCE_TIME rtTime = 0;
		pTrackMgr->MinSecGridMsToRefTime( m_PropCurve.m_pParamStrip, m_lDurBar, m_lDurBeat, m_lDurGrid, m_lDurTick, &rtTime );
		pTrackMgr->RefTimeToUnknownTime( rtTime, &rtNewLength );
	}
	else
	{
		MUSIC_TIME mtTime = 0;
		pTrackMgr->MeasureBeatGridTickToClocks( m_lDurBar, m_lDurBeat, m_lDurGrid, m_lDurTick, &mtTime );
		pTrackMgr->ClocksToUnknownTime( mtTime, &rtNewLength );
	}

	return rtNewLength;
}


/////////////////////////////////////////////////////////////////////////////
// CPropPageCurve::MinTimeToUnknownTime

REFERENCE_TIME CPropPageCurve::MinTimeToUnknownTime( void )
{
	CTrackMgr* pTrackMgr = m_pPageManager->m_pTrackMgr;;
	ASSERT(pTrackMgr);
	if(pTrackMgr == NULL)
	{
		return 0;
	}

	REFERENCE_TIME rtMinTime;

	if( pTrackMgr->IsRefTimeTrack() )
	{
		pTrackMgr->MinSecGridMsToRefTime( m_PropCurve.m_pParamStrip, 0, 0, 0, 1, &rtMinTime );
	}
	else
	{
		MUSIC_TIME mtMinTime;
		pTrackMgr->MeasureBeatGridTickToClocks( 0, 0, 0, 1, &mtMinTime );
		rtMinTime = mtMinTime;
	}

	return rtMinTime;
}


/////////////////////////////////////////////////////////////////////////////
// CPropPageCurve::MinTimeToRefTime

REFERENCE_TIME CPropPageCurve::MinTimeToRefTime( void )
{
	CTrackMgr* pTrackMgr = m_pPageManager->m_pTrackMgr;;
	ASSERT(pTrackMgr);
	if(pTrackMgr == NULL)
	{
		return 0;
	}

	REFERENCE_TIME rtMinTime = 0;

	if( pTrackMgr->IsRefTimeTrack() )
	{
		pTrackMgr->MinSecGridMsToRefTime( m_PropCurve.m_pParamStrip, 0, 0, 0, 1, &rtMinTime );
	}
	else
	{
		MUSIC_TIME mtMinTime;
		pTrackMgr->MeasureBeatGridTickToClocks( 0, 0, 0, 1, &mtMinTime );
		pTrackMgr->m_pTimeline->ClocksToRefTime( mtMinTime, &rtMinTime );
	}

	return rtMinTime;
}



/////////////////////////////////////////////////////////////////////////////
// CPropPageCurve::RecomputeTimes

void CPropPageCurve::RecomputeTimes()
{
	CTrackMgr* pTrackMgr = m_pPageManager->m_pTrackMgr;;
	ASSERT(pTrackMgr);
	if(pTrackMgr == NULL)
	{
		return;
	}

	if( pTrackMgr->IsRefTimeTrack() )
	{
		ComputeMinSecGridMs();
	}
	else
	{
		ComputeBarBeatGridTick();
	}
}

/////////////////////////////////////////////////////////////////////////////
// CPropPageCurve::ComputeMinSecGridMs

void CPropPageCurve::ComputeMinSecGridMs( void )
{
	CTrackMgr* pTrackMgr = m_pPageManager->m_pTrackMgr;;
	ASSERT(pTrackMgr);
	if(pTrackMgr == NULL)
	{
		return;
	}

	// START
	pTrackMgr->RefTimeToMinSecGridMs( m_PropCurve.m_pParamStrip, m_PropCurve.m_rtStartTime,
										&m_lStartBar, &m_lStartBeat, &m_lStartGrid, &m_lStartTick );
	

	// END
	pTrackMgr->RefTimeToMinSecGridMs( m_PropCurve.m_pParamStrip, m_PropCurve.m_rtStartTime + m_PropCurve.m_rtDuration,
										&m_lEndBar, &m_lEndBeat, &m_lEndGrid, &m_lEndTick );
	

	// LENGTH
	pTrackMgr->RefTimeToMinSecGridMs( m_PropCurve.m_pParamStrip, m_PropCurve.m_rtDuration,
										&m_lDurBar, &m_lDurBeat, &m_lDurGrid, &m_lDurTick );

}


/////////////////////////////////////////////////////////////////////////////
// CPropPageCurve::ComputeBarBeatGridTick

void CPropPageCurve::ComputeBarBeatGridTick( void )
{
	MUSIC_TIME mtTime = 0;

	CTrackMgr* pTrackMgr = m_pPageManager->m_pTrackMgr;;
	ASSERT(pTrackMgr);
	if(pTrackMgr == NULL)
	{
		return;
	}

	IDMUSProdTimeline* pTimeline = pTrackMgr->GetTimeline();
	ASSERT(pTimeline);
	if(pTimeline == NULL)
	{
		return;
	}


	// START
	pTrackMgr->ClocksToMeasureBeatGridTick( (MUSIC_TIME)m_PropCurve.m_rtStartTime, &m_lStartBar, &m_lStartBeat, &m_lStartGrid, &m_lStartTick );

	// END
	pTrackMgr->ClocksToMeasureBeatGridTick( (MUSIC_TIME)m_PropCurve.m_rtStartTime + (MUSIC_TIME)m_PropCurve.m_rtDuration, &m_lEndBar, &m_lEndBeat, &m_lEndGrid, &m_lEndTick );

	// LENGTH
	pTrackMgr->ClocksToMeasureBeatGridTick( (MUSIC_TIME)m_PropCurve.m_rtDuration, &m_lDurBar, &m_lDurBeat, &m_lDurGrid, &m_lDurTick );
}


/////////////////////////////////////////////////////////////////////////////
// CPropPageCurve::SetControlRanges

void CPropPageCurve::SetControlRanges( CPropCurve* pPropCurve )
{

	// Get track group bits
	DWORD dwGroupBits = 0xFFFFFFFF;
	if( m_pPageManager
	&&  m_pPageManager->m_pIPropPageObject )
	{
		// Query the Property Page Object for an IDMUSProdStripMgr interface
		IDMUSProdStripMgr *pStripMgr;
		if( SUCCEEDED( m_pPageManager->m_pIPropPageObject->QueryInterface( IID_IDMUSProdStripMgr, (void**)&pStripMgr ) ) )
		{
			// Ask the strip manager for its track header, so we can read the group bits from it
			DMUS_IO_TRACK_HEADER ioTrackHeader;
			VARIANT variant;
			variant.vt = VT_BYREF;
			V_BYREF( &variant ) = &ioTrackHeader;
			if( SUCCEEDED( pStripMgr->GetStripMgrProperty( SMP_DMUSIOTRACKHEADER, &variant ) ) )
			{
				dwGroupBits = ioTrackHeader.dwGroup;
			}

			// Release the returned IDMUSProdStripMgr pointer
			pStripMgr->Release();
		}
	}

	// Set ranges for "time" controls
	if( m_pPageManager->m_pTrackMgr->IsRefTimeTrack() )
	{
		SetRefTimeRanges();
	}
	else
	{
		SetMusicTimeRanges( dwGroupBits, pPropCurve );
	}


	// 'Start' and 'End' value controls
	
	m_editStartValue.LimitText( 3 );
	m_editEndValue.LimitText( 3 );
	CParamStrip* pParamStrip = (CParamStrip *)m_pPageManager->m_pIPropPageObject;
	ASSERT(pParamStrip);
	if(pParamStrip != NULL)
	{
		//char szMaxValue[32];
		//itoa((int)pParamStrip->m_ParamInfo.mpdMaxValue, szMaxValue, 10);
		//char szMinValue[32];
		//itoa((int)pParamStrip->m_ParamInfo.mpdMinValue, szMinValue, 10);
		CString sMaxValue;
		sMaxValue.Format("%.03f", pParamStrip->m_ParamInfo.mpdMaxValue);
		CString sMinValue;
		sMinValue.Format("%.03f", pParamStrip->m_ParamInfo.mpdMinValue);

		int nMaxLength = sMaxValue.GetLength();
		int nMinLength = sMinValue.GetLength();
		int nLength = nMaxLength > nMinLength ? nMaxLength : nMinLength;

		m_editStartValue.LimitText( nLength );
		m_editEndValue.LimitText( nLength );

		float fMinValueRange = pParamStrip->m_ParamInfo.mpdMinValue;
		float fMaxValueRange = pParamStrip->m_ParamInfo.mpdMaxValue - pParamStrip->m_ParamInfo.mpdMinValue;

		// Figure out the range if the actual range is less than 1
		if(fMaxValueRange <= 1)
		{
			fMinValueRange = 1;
			fMaxValueRange = 1 + (fMaxValueRange / 0.001);	
		}

		m_spinStartValue.SetRange(fMinValueRange, fMaxValueRange);
		m_spinEndValue.SetRange(fMinValueRange, fMaxValueRange);
	}
}


/////////////////////////////////////////////////////////////////////////////
// CPropPageCurve::SetRefTimeRanges

void CPropPageCurve::SetRefTimeRanges( void )
{
	// Limit the minute number to positive numbers
	m_spinStartBar.SetRange( 1, 32767 );
	m_editStartBar.LimitText( 5 );
	m_spinEndBar.SetRange( 1, 32767 );
	m_editEndBar.LimitText( 5 );
	m_spinDurBar.SetRange( 0, 32767 );
	m_editDurBar.LimitText( 5 );

	// Limit the second number to 1-60
	m_spinStartBeat.SetRange( 1, 60 );
	m_editStartBeat.LimitText( 2 );
	m_spinEndBeat.SetRange( 1, 60 );
	m_editEndBeat.LimitText( 2 );
	m_spinDurBeat.SetRange( 0, 60 );
	m_editDurBeat.LimitText( 2 );

	// Limit the grid number to MIN_NBR_GRIDS_PER_SECOND - MAX_NBR_GRIDS_PER_SECOND
	m_spinStartGrid.SetRange( MIN_NBR_GRIDS_PER_SECOND, MAX_NBR_GRIDS_PER_SECOND );
	m_editStartGrid.LimitText( 3 );
	m_spinEndGrid.SetRange( MIN_NBR_GRIDS_PER_SECOND, MAX_NBR_GRIDS_PER_SECOND );
	m_editEndGrid.LimitText( 3 );
	m_spinDurGrid.SetRange( (MIN_NBR_GRIDS_PER_SECOND - 1), MAX_NBR_GRIDS_PER_SECOND );
	m_editDurGrid.LimitText( 3 );

	// Limit the millisecond number to 1-1000
	m_spinStartTick.SetRange( 1, 1000 );
	m_editStartTick.LimitText( 4 );
	m_spinEndTick.SetRange( 1, 1000 );
	m_editEndTick.LimitText( 4 );
	m_spinDurTick.SetRange( 1, 1000 );
	m_editDurTick.LimitText( 4 );
}


/////////////////////////////////////////////////////////////////////////////
// CPropPageCurve::SetMusicTimeRanges

void CPropPageCurve::SetMusicTimeRanges( DWORD dwGroupBits, CPropCurve* pPropCurve )
{

	ASSERT(pPropCurve);
	if(pPropCurve == NULL)
	{
		return;
	}

	CTrackMgr* pTrackMgr = m_pPageManager->m_pTrackMgr;
	ASSERT(pTrackMgr);
	if(pTrackMgr == NULL)
	{
		return;
	}

	IDMUSProdTimeline* pTimeline = NULL;
	pTimeline = pTrackMgr->GetTimeline();

	long lMaxMeasure = 0;
	long lMaxBeat = 0;
	long lMaxGrid = 0;

	// Get the maximum measure value
	if( pTimeline == NULL )
	{
		// No timeline pointer - use default values
		lMaxMeasure = 32767;
		lMaxBeat = 256;
		lMaxGrid = 256;
	}
	else
	{
		lMaxGrid = 256;	// Fix this!!!

		// Get the length of the timeline, in clocks
		VARIANT var;
		pTimeline->GetTimelineProperty( TP_CLOCKLENGTH, &var );

		// Convert from clocks to a measure and beat value
		pTimeline->ClocksToMeasureBeat( dwGroupBits, 0, V_I4( &var ), &lMaxMeasure, &lMaxBeat );

		// If the beat value is zero, check if the segment is exactly lMaxMeasure in length
		if( lMaxBeat == 0 )
		{
			// Convert from lMaxMeasure to a clock value
			long lClockLength;
			pTimeline->MeasureBeatToClocks( dwGroupBits, 0, lMaxMeasure, 0, &lClockLength );

			// Check if this clock value is equal to the length
			if( lClockLength == V_I4( &var ) )
			{
				// Exactly lMaxMeasures long.  Get the measure and beat value for the previous tick
				pTimeline->ClocksToMeasureBeat( dwGroupBits, 0, V_I4( &var ) - 1, &lMaxMeasure, &lMaxBeat );
			}
		}

		long lItemMeasure = 0;
		long lItemBeat = 0;
		long lItemGrid = 0;
		long lItemTick = 0;

		pTrackMgr->UnknownTimeToMeasureBeatGridTick( pPropCurve->m_rtStartTime, &lItemMeasure, &lItemBeat, &lItemGrid, &lItemTick );

		// If the item is not in the last measure
		if( lItemMeasure < lMaxMeasure )
		{
			// Convert from a Measure value to a clock value
			long lClockForMeasure;
			m_pPageManager->m_pTrackMgr->m_pTimeline->MeasureBeatToClocks( dwGroupBits, 0, lItemMeasure, 0, &lClockForMeasure );

			// Get the TimeSig for this measure
			DMUS_TIMESIGNATURE TimeSig;
			if( SUCCEEDED( m_pPageManager->m_pTrackMgr->m_pTimeline->GetParam( GUID_TimeSignature, dwGroupBits, 0, lClockForMeasure, NULL, &TimeSig ) ) )
			{
				lMaxBeat = TimeSig.bBeatsPerMeasure - 1;
			}
		}
		// Else the item is in the last measure and lMaxBeat is already set

		// Now convert the measure and beat from 0-based to 1-based
		lMaxMeasure++;
		lMaxBeat++;
	}

	// Update the range for the measure spin control
	m_spinStartBar.SetRange( 1, lMaxMeasure );
	m_editStartBar.LimitText( 5 );
	m_spinEndBar.SetRange( 1, lMaxMeasure );
	m_editEndBar.LimitText( 5 );
	m_spinDurBar.SetRange( 0, lMaxMeasure );
	m_editDurBar.LimitText( 5 );

	// Update the range for the beat spin control
	m_spinStartBeat.SetRange( 1, lMaxBeat );
	m_editStartBeat.LimitText( 3 );
	m_spinEndBeat.SetRange( 1, lMaxBeat );
	m_editEndBeat.LimitText( 3 );
	m_spinDurBeat.SetRange( 0, lMaxBeat );
	m_editDurBeat.LimitText( 3 );

	// Update the range for the grid spin control
	m_spinStartGrid.SetRange( 1, lMaxGrid );
	m_editStartGrid.LimitText( 3 );
	m_spinEndGrid.SetRange( 1, lMaxGrid );
	m_editEndGrid.LimitText( 3 );
	m_spinDurGrid.SetRange( 0, lMaxGrid );
	m_editDurGrid.LimitText( 3 );

	// Update the range for the tick spin control
	m_spinStartTick.SetRange( -MAX_TICK, MAX_TICK );
	m_editStartTick.LimitText( 5 );
	m_spinEndTick.SetRange( -MAX_TICK, MAX_TICK );
	m_editEndTick.LimitText( 5 );
	m_spinDurTick.SetRange( -MAX_TICK, MAX_TICK );
	m_editDurTick.LimitText( 5 );
}


/////////////////////////////////////////////////////////////////////////////
// CPropPageCurve::EnableControls

void CPropPageCurve::EnableControls( BOOL fEnable ) 
{
	CTrackMgr* pTrackMgr = m_pPageManager->m_pTrackMgr;
	ASSERT(pTrackMgr);
	if(pTrackMgr == NULL)
	{
		return;
	}

	// Reference time or music time?
	CString strPrompt;
	if(  pTrackMgr->IsRefTimeTrack() )
	{
		strPrompt.LoadString( IDS_MIN_TEXT );
		m_staticPromptBar.SetWindowText( strPrompt );
		strPrompt.LoadString( IDS_SEC_TEXT );
		m_staticPromptBeat.SetWindowText( strPrompt );
		strPrompt.LoadString( IDS_GRID_TEXT );
		m_staticPromptGrid.SetWindowText( strPrompt );
		strPrompt.LoadString( IDS_MS_TEXT );
		m_staticPromptTick.SetWindowText( strPrompt );
	}
	else
	{
		strPrompt.LoadString( IDS_BAR_TEXT );
		m_staticPromptBar.SetWindowText( strPrompt );
		strPrompt.LoadString( IDS_BEAT_TEXT );
		m_staticPromptBeat.SetWindowText( strPrompt );
		strPrompt.LoadString( IDS_GRID_TEXT );
		m_staticPromptGrid.SetWindowText( strPrompt );
		strPrompt.LoadString( IDS_TICK_TEXT );
		m_staticPromptTick.SetWindowText( strPrompt );
	}

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

	bool bShowCombos = m_bEnumType || m_bBoolType;
	m_comboEnumStartValue.ShowWindow(bShowCombos);
	m_comboEnumEndValue.ShowWindow(bShowCombos);

	CWnd* pWnd = GetDlgItem(IDC_EDIT_STARTVALUE);
	if(pWnd)
	{
		pWnd->ShowWindow(!bShowCombos);
	}
	
	pWnd = GetDlgItem(IDC_EDIT_ENDVALUE);
	if(pWnd)
	{
		pWnd->ShowWindow(!bShowCombos);
	}
	
	m_spinStartValue.ShowWindow(!bShowCombos);
	m_spinEndValue.ShowWindow(!bShowCombos);

	if(fEnable)
	{
		if(bShowCombos)
		{
			m_comboEnumStartValue.EnableWindow(fEnable);
			m_comboEnumEndValue.EnableWindow(fEnable);
		}
		else
		{
			// Disable and hide the controls
			EnableItem( IDC_EDIT_STARTVALUE, fEnable );
			m_spinStartValue.EnableWindow( fEnable );
			EnableItem( IDC_EDIT_ENDVALUE, fEnable );
			m_spinEndValue.EnableWindow( fEnable );
		}
	}
	else
	{
		if(bShowCombos)
		{
			EnableItem(IDC_COMBO_ENUMSTARTVALUE, fEnable);
			EnableItem(IDC_COMBO_ENUMENDVALUE, fEnable);
		}
		else
		{
			EnableItem( IDC_EDIT_STARTVALUE, fEnable );
			m_spinStartValue.EnableWindow( fEnable );
			EnableItem( IDC_EDIT_ENDVALUE, fEnable );
			m_spinEndValue.EnableWindow( fEnable );
		}
	}
	
	EnableItem(IDC_COMBO_SHAPE, fEnable);
	/*m_comboShape.EnableWindow( fEnable );*/
	m_btnFlipHorz.EnableWindow( fEnable );
	m_btnFlipVert.EnableWindow( fEnable );

	m_btnStartFromCurrent.EnableWindow( fEnable );
}


/////////////////////////////////////////////////////////////////////////////
// CPropPageCurve::SetSpinStartValuePos

void CPropPageCurve::SetSpinStartValuePos( int nStartValue )
{
	m_spinStartValue.SetPos( nStartValue );
}


/////////////////////////////////////////////////////////////////////////////
// CPropPageCurve::GetSpinStartValuePos

int CPropPageCurve::GetSpinStartValuePos( void )
{
	int nStartValue = LOWORD(m_spinStartValue.GetPos());

	return nStartValue;
}


/////////////////////////////////////////////////////////////////////////////
// CPropPageCurve::SetSpinEndValuePos

void CPropPageCurve::SetSpinEndValuePos( int nEndValue )
{
	m_spinEndValue.SetPos( nEndValue );
}


/////////////////////////////////////////////////////////////////////////////
// CPropPageCurve::GetSpinEndValuePos

int CPropPageCurve::GetSpinEndValuePos( void )
{
	int nEndValue = LOWORD(m_spinEndValue.GetPos());
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
		m_fInUpdateControls = FALSE;
		return;
	}

	m_bBoolType = false;
	m_bEnumType = false;
	CParamStrip* pParamStrip = (CParamStrip *)m_pPageManager->m_pIPropPageObject;
	if(pParamStrip != NULL)
	{
		m_bEnumType = pParamStrip->m_ParamInfo.mpType == MPT_ENUM;
		m_bBoolType = pParamStrip->m_ParamInfo.mpType == MPT_BOOL;
		m_bFloatType = pParamStrip->m_ParamInfo.mpType == MPT_FLOAT;
		m_bIntType = pParamStrip->m_ParamInfo.mpType == MPT_INT;
	}

	EnableControls( TRUE );

	if(m_bEnumType)
	{
		WCHAR* pwszEnumText = NULL;
		CTrackObject* pTrackObject = pParamStrip->GetTrackObject();
		ASSERT(pTrackObject);
		if(pTrackObject)
		{
			GUID clsidObject = pTrackObject->GetCLSID();
			DWORD dwParamIndex = pParamStrip->GetParamIndex();
			if(FAILED(CTrackObject::GetParamEnumTypeText(clsidObject, dwParamIndex, &pwszEnumText)))
			{
				return;
			}
		}

		CList<CString, CString&> lstEnumStrings;
		CString sParamName;
		CString sParamUnitLabel;
		DWORD dwCount = 0;
		if(FAILED(CTrackObject::ParseEnumText(pwszEnumText, sParamName, sParamUnitLabel, &lstEnumStrings, &dwCount)))
		{
			return; 
		}

		// Free the allocated text
		::CoTaskMemFree(pwszEnumText);

		// Fill up the combo boxes with enum types
		m_comboEnumStartValue.ResetContent();
		m_comboEnumEndValue.ResetContent();

		DWORD dwValue = 0;
		POSITION position = lstEnumStrings.GetHeadPosition();
		while(position)
		{
			CString sText = lstEnumStrings.GetNext(position);
			int nIndex = m_comboEnumStartValue.AddString(sText);
			if(nIndex == CB_ERR)
			{
				return;
			}
			m_comboEnumStartValue.SetItemData(nIndex, dwValue);

			nIndex = m_comboEnumEndValue.AddString(sText);
			if(nIndex == CB_ERR)
			{
				return;
			}
			m_comboEnumEndValue.SetItemData(nIndex, dwValue);
			dwValue++;
		}
	}

	if(m_bBoolType)
	{
		// Fill up the combo boxes with BOOL types
		m_comboEnumStartValue.ResetContent();
		m_comboEnumEndValue.ResetContent();

		CString sFalse;
		sFalse.LoadString(IDS_BOOL_FALSE);

		CString sTrue;
		sTrue.LoadString(IDS_BOOL_TRUE);

		int nIndex = m_comboEnumStartValue.AddString(sFalse);
		m_comboEnumStartValue.SetItemData(nIndex, 0);
		nIndex = m_comboEnumStartValue.AddString(sTrue);
		m_comboEnumStartValue.SetItemData(nIndex, 1);

		nIndex = m_comboEnumEndValue.AddString(sFalse);
		m_comboEnumEndValue.SetItemData(nIndex, 0);
		nIndex = m_comboEnumEndValue.AddString(sTrue);
		m_comboEnumEndValue.SetItemData(nIndex, 1);
	}

	// Copy the common properties object and set the control ranges
	m_PropCurve.Copy(pPropCurve);
	SetControlRanges( &m_PropCurve);

	// Get the bar, beat, grid and tick for the properties object
	// So we can update the values in the curve to figure out what's indeterminate
	ComputeBarBeatGridTick();
	m_PropCurve.UpdateStartBarBeatGridTick(m_lStartBar, m_lStartBeat, m_lStartGrid, m_lStartTick);
	m_PropCurve.UpdateEndBarBeatGridTick(m_lEndBar, m_lEndBeat, m_lEndGrid, m_lEndTick);
	m_PropCurve.UpdateDurationBarBeatGridTick(m_lDurBar, m_lDurBeat, m_lDurGrid, m_lDurTick);
	
	RecomputeTimes();

	// Set nMeasureBeatOffset
	int nMeasureBeatOffset;
	if( m_pPageManager->m_pTrackMgr->IsRefTimeTrack() )
	{
		nMeasureBeatOffset = 0;
	}
	else
	{
		nMeasureBeatOffset = 1;
	}


	if( pParamStrip && pParamStrip->IsOneGrid())
	{
		m_editStartGrid.SetWindowText( NULL );
		m_editStartGrid.EnableWindow( FALSE );
		m_editEndGrid.SetWindowText( NULL );
		m_editEndGrid.EnableWindow( FALSE );
		m_editDurGrid.SetWindowText( NULL );
		m_editDurGrid.EnableWindow( FALSE );
	}

	int nPos = 0;

	if( pPropCurve->m_dwUndetermined & UNDT_STARTBAR )
	{
		m_editStartBar.SetWindowText( achUnknown );
	}
	else
	{
		SetEditControl(m_editStartBar, m_lStartBar, nMeasureBeatOffset);
	}

	if( pPropCurve->m_dwUndetermined & UNDT_STARTBEAT )
	{
		m_editStartBeat.SetWindowText( achUnknown );
	}
	else
	{
		SetEditControl(m_editStartBeat, m_lStartBeat, nMeasureBeatOffset);
	}

	if( pPropCurve->m_dwUndetermined & UNDT_STARTGRID )
	{
		m_editStartGrid.SetWindowText( achUnknown );
	}
	else
	{
		SetEditControl(m_editStartGrid, m_lStartGrid, 1);
	}

	if( pPropCurve->m_dwUndetermined & UNDT_STARTTICK )
	{
		m_editStartTick.SetWindowText( achUnknown );
	}
	else
	{
		SetEditControl(m_editStartTick, m_lStartTick, 0);
	}

	if( pPropCurve->m_dwUndetermined & UNDT_ENDBAR )
	{
		m_editEndBar.SetWindowText( achUnknown );
	}
	else
	{
		SetEditControl(m_editEndBar, m_lEndBar, nMeasureBeatOffset);
	}

	if( pPropCurve->m_dwUndetermined & UNDT_ENDBEAT )
	{
		m_editEndBeat.SetWindowText( achUnknown );
	}
	else
	{
		SetEditControl(m_editEndBeat, m_lEndBeat, nMeasureBeatOffset);
	}

	if( pPropCurve->m_dwUndetermined & UNDT_ENDGRID )
	{
		m_editEndGrid.SetWindowText( achUnknown );
	}
	else
	{
		SetEditControl(m_editEndGrid, m_lEndGrid, 1);
	}

	if( pPropCurve->m_dwUndetermined & UNDT_ENDTICK )
	{
		m_editEndTick.SetWindowText( achUnknown );
	}
	else
	{
		SetEditControl(m_editEndTick, m_lEndTick, 0);
	}

	if( pPropCurve->m_dwUndetermined & UNDT_DURBAR )
	{
		m_editDurBar.SetWindowText( achUnknown );
	}
	else
	{
		SetEditControl(m_editDurBar, m_lDurBar, 0);
	}

	if( pPropCurve->m_dwUndetermined & UNDT_DURBEAT )
	{
		m_editDurBeat.SetWindowText( achUnknown );
	}
	else
	{
		SetEditControl(m_editDurBeat, m_lDurBeat, 0);
	}

	if( pPropCurve->m_dwUndetermined & UNDT_DURGRID )
	{
		m_editDurGrid.SetWindowText( achUnknown );
	}
	else
	{
		SetEditControl(m_editDurGrid, m_lDurGrid, 0);
	}

	if( pPropCurve->m_dwUndetermined & UNDT_DURTICK )
	{
		m_editDurTick.SetWindowText( achUnknown );
	}
	else
	{
		SetEditControl(m_editDurTick, m_lDurTick, 0);
	}

	if(m_bFloatType || m_bIntType)
	{
		if( pPropCurve->m_dwUndetermined & UNDT_STARTVAL )
		{
			m_editStartValue.SetWindowText( achUnknown );
		}
		else
		{
			CString strValue;
			float fStartValue = pPropCurve->m_fltStartValue;
			StartEndValueToString( fStartValue, strValue );

			CString strCurrentText;
			m_editStartValue.GetWindowText( strCurrentText );
			if( strValue != strCurrentText )
			{
				m_editStartValue.SetWindowText( strValue );
				// SetSel's needed to get caret to end of string
				m_editStartValue.SetSel( 0, -1 );
				m_editStartValue.SetSel( -1, -1 );

				float fRangeGranularity = 1;
				float fValueRange = (pParamStrip->m_ParamInfo.mpdMaxValue - pParamStrip->m_ParamInfo.mpdMinValue);
				
				if(fValueRange <= 1)
				{
					fRangeGranularity = 0.001f; 
				}

				if(fValueRange > 1)
				{
					SetSpinStartValuePos(fStartValue);
				}
				else
				{
					SetSpinStartValuePos(fStartValue / fRangeGranularity);
				}

			}
		}
	}
	else if(m_bEnumType || m_bBoolType)
	{
		if( pPropCurve->m_dwUndetermined & UNDT_STARTVAL )
		{
			m_comboEnumStartValue.SetWindowText( achUnknown );
		}
		else
		{		
			m_comboEnumStartValue.SetCurSel((int)pPropCurve->m_fltStartValue);
		}

	}

	if(m_bFloatType || m_bIntType)
	{
		if( pPropCurve->m_dwUndetermined & UNDT_ENDVAL )
		{
			m_editEndValue.SetWindowText( achUnknown );
		}
		else
		{
			CString strValue;
			float fEndValue = pPropCurve->m_fltEndValue;
			StartEndValueToString( fEndValue, strValue );

			CString strCurrentText;
			m_editEndValue.GetWindowText( strCurrentText );
			if( strValue != strCurrentText )
			{
				m_editEndValue.SetWindowText( strValue );
				// SetSel's needed to get caret to end of string
				m_editEndValue.SetSel( 0, -1 );
				m_editEndValue.SetSel( -1, -1 );

				float fRangeGranularity = 1;
				float fValueRange = (pParamStrip->m_ParamInfo.mpdMaxValue - pParamStrip->m_ParamInfo.mpdMinValue);
				
				if(fValueRange <= 1)
				{
					fRangeGranularity = 0.001f; 
				}

				if(fValueRange > 1)
				{
					SetSpinEndValuePos(fEndValue);
				}
				else
				{
					SetSpinEndValuePos(fEndValue / fRangeGranularity);
				}
			}
		}
	}
	else if(m_bEnumType || m_bBoolType)
	{
		if( pPropCurve->m_dwUndetermined & UNDT_ENDVAL )
		{
			m_comboEnumEndValue.SetWindowText( achUnknown );
		}
		else
		{
			m_comboEnumEndValue.SetCurSel((int)pPropCurve->m_fltEndValue);
		}	
	}

	InitShapeCombo(pPropCurve);

	if(pPropCurve->m_dwUndetermined & UNDT2_STARTCURRENT)
	{
		m_btnStartFromCurrent.SetCheck(2);
	}
	else
	{

		if(pPropCurve->m_fStartFromCurrent == TRUE)
		{
			m_btnStartFromCurrent.SetCheck(1);
		}
		else
		{
			m_btnStartFromCurrent.SetCheck(0);
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
	TCHAR achShape[32];

	for ( int i = IDS_CURVE_SHAPE1 ;  i <= IDS_CURVE_SHAPE5 ;  i++ )
	{
		::LoadString( theApp.m_hInstance, i, achShape, 32 );
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

	m_PropCurve.m_dwCurveShape = 0xFF; 

	switch( m_comboShape.GetCurSel() )
	{
		case 0:
			m_PropCurve.m_dwCurveShape = MP_CURVE_SINE; 
			break;

		case 1:
			m_PropCurve.m_dwCurveShape = MP_CURVE_INVSQUARE; 
			break;

		case 2:
			m_PropCurve.m_dwCurveShape = MP_CURVE_SQUARE; 
			break;

		case 3:
			m_PropCurve.m_dwCurveShape = MP_CURVE_JUMP; 
			break;

		case 4:
			m_PropCurve.m_dwCurveShape = MP_CURVE_LINEAR; 
			break;
	}

	if( m_PropCurve.m_dwCurveShape != 0xFF )
	{
		m_PropCurve.m_dwChanged = CHGD_SHAPE;
		m_pPageManager->m_pIPropPageObject->SetData( &m_PropCurve );
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

	m_PropCurve.m_dwChanged = CHGD_FLIPVERT;
	m_pPageManager->m_pIPropPageObject->SetData( &m_PropCurve );
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

	m_PropCurve.m_dwChanged = CHGD_FLIPHORZ;
	m_pPageManager->m_pIPropPageObject->SetData( &m_PropCurve );
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



	float fltMin = 0;
	float fltMax = 127;
	CParamStrip* pParamStrip = (CParamStrip *)m_pPageManager->m_pIPropPageObject;
	ASSERT(pParamStrip);
	if(pParamStrip != NULL)
	{
		fltMin = pParamStrip->m_ParamInfo.mpdMinValue;
		fltMax = pParamStrip->m_ParamInfo.mpdMaxValue;
	}

	float fRangeGranularity = 1;
	float fValueRange = (fltMax - fltMin);
	
	if(fValueRange <= 1)
	{
		fRangeGranularity = 0.001f; 
	}

	// Get current end value
	float fltCurStartValue = (float)GetSpinStartValuePos();
	if(fValueRange > 1)
	{
		fltCurStartValue = (float)GetSpinStartValuePos() + fRangeGranularity;
	}
	else
	{
		fltCurStartValue = (float)GetSpinStartValuePos() * fRangeGranularity;
	}

	if(fltCurStartValue > fltMax)
	{
		fltCurStartValue = fltCurStartValue - 65536;
	}

	// Get text from edit control
	CString strNewStartValue;
	m_editStartValue.GetWindowText( strNewStartValue );
	strNewStartValue.TrimRight();
	strNewStartValue.TrimLeft();

	float fltNewStartValue = 0;

	// Handle unknown text
	if( _tcsncmp( strNewStartValue, achUnknown, 10 ) == 0 )
	{
		return;
	}
	// Handle empty text
	else if( strNewStartValue.IsEmpty() )
	{
		fltNewStartValue = 0; // Set to 0
	}
	else
	{
		fltNewStartValue = StringToStartEndValue( strNewStartValue );
		if( fltNewStartValue == 0xFFFFFFFF)
		{
			fltNewStartValue = fltCurStartValue;	// No change
		}
		else
		{
			// Make sure value is within range
			if( fltNewStartValue < fltMin )
			{
				fltNewStartValue = fltMin;
			}
			if( fltNewStartValue > fltMax )
			{
				fltNewStartValue = fltMax;
			}
		}
	}

	// Text in edit control will by synced in response to the SetData() call, if necessary

	if( fltCurStartValue != fltNewStartValue )
	{
		if(fValueRange > 1)
		{
			SetSpinStartValuePos( (int)fltNewStartValue );
		}
		else
		{
			SetSpinStartValuePos( (int)(0.5 + (fltNewStartValue / fRangeGranularity)) );
		}

		m_PropCurve.m_dwChanged = UNDT_STARTVAL;
		m_PropCurve.m_fltStartValue = fltNewStartValue;
		m_pPageManager->m_pIPropPageObject->SetData( &m_PropCurve );
	}
	else
	{
		// Check if edit control's text needs to be updated
		CString strValue;
		CString strCurValue;
		StartEndValueToString( fltNewStartValue, strValue );
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
// CPropPageCurve::OnDeltaPosSpinStartValue

void CPropPageCurve::OnDeltaPosSpinStartValue(NMHDR* pNMHDR, LRESULT* pResult) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_pPageManager->m_pIPropPageObject == NULL
	||  m_fInUpdateControls == TRUE )
	{
		return;
	}
	
	CParamStrip* pParamStrip = (CParamStrip *)m_pPageManager->m_pIPropPageObject;
	ASSERT(pParamStrip);
	if(pParamStrip == NULL)
	{
		return;
	}

	// Just in case user clicked on spin control immediately after typing text
	OnKillFocusStartValue();

	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

	float fRangeGranularity = 1;
	float fValueRange = (pParamStrip->m_ParamInfo.mpdMaxValue - pParamStrip->m_ParamInfo.mpdMinValue);
	
	if(fValueRange <= 1)
	{
		fRangeGranularity = 0.001f; 
	}

	float fltOldValue  = (float)GetSpinStartValuePos();
	if(fValueRange < 1)
	{
		fltOldValue  = fltOldValue  * fRangeGranularity;
	}

	if(fltOldValue == pParamStrip->m_ParamInfo.mpdMaxValue && pNMUpDown->iDelta > 0)
	{
		return;
	}

	if(fltOldValue == pParamStrip->m_ParamInfo.mpdMinValue && pNMUpDown->iDelta < 0)
	{
		return;
	}

	float fltNewStartValue = 0;
	if(fValueRange > 1)
	{
		if(pNMUpDown->iDelta > 0)
		{
			fltNewStartValue = (float)GetSpinStartValuePos() + fRangeGranularity;
		}
		else if(pNMUpDown->iDelta < 0)
		{
			fltNewStartValue = (float)GetSpinStartValuePos() - fRangeGranularity;
		}
	}
	else
	{
		if(pNMUpDown->iDelta > 0)
		{
			fltNewStartValue = ((float)GetSpinStartValuePos() * fRangeGranularity) + fRangeGranularity;
		}
		else if(pNMUpDown->iDelta < 0)
		{
			fltNewStartValue = ((float)GetSpinStartValuePos() * fRangeGranularity) - fRangeGranularity;
		}
	}

	// Get StartValue range
	float fltMin = pParamStrip->m_ParamInfo.mpdMinValue;
	float fltMax = pParamStrip->m_ParamInfo.mpdMaxValue;

	if(fltNewStartValue > fltMax + 0.5)
	{
		fltNewStartValue = fltNewStartValue - 65536;
	}

	// Make sure value is within range
	if( fltNewStartValue < fltMin )
	{
		fltNewStartValue = fltMin;
	}
	if( fltNewStartValue > fltMax )
	{
		fltNewStartValue = fltMax;
	}

	CString strValue;

	StartEndValueToString( fltNewStartValue, strValue );
	m_editStartValue.SetWindowText( strValue );
	if(fValueRange > 1)
	{
		SetSpinStartValuePos( (int)fltNewStartValue );
	}
	else
	{
		SetSpinStartValuePos( (int)(0.5 + (fltNewStartValue / fRangeGranularity)) );
	}

	m_PropCurve.m_dwChanged = CHGD_STARTVAL;
	m_PropCurve.m_fltStartValue = fltNewStartValue;
	m_pPageManager->m_pIPropPageObject->SetData( &m_PropCurve );
	
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

	float fltMin = 0;
	float fltMax = 127;
	CParamStrip* pParamStrip = (CParamStrip *)m_pPageManager->m_pIPropPageObject;
	ASSERT(pParamStrip);
	if(pParamStrip != NULL)
	{
		fltMin = pParamStrip->m_ParamInfo.mpdMinValue;
		fltMax = pParamStrip->m_ParamInfo.mpdMaxValue;
	}

	float fRangeGranularity = 1;
	float fValueRange = (fltMax - fltMin);
	
	if(fValueRange <= 1)
	{
		fRangeGranularity = 0.001f; 
	}

	// Get current end value
	float fltCurEndValue = (float)GetSpinEndValuePos();
	if(fValueRange > 1)
	{
		fltCurEndValue = (float)GetSpinEndValuePos() + fRangeGranularity;
	}
	else
	{
		fltCurEndValue = (float)GetSpinEndValuePos() * fRangeGranularity;
	}

	if(fltCurEndValue > fltMax)
	{
		fltCurEndValue = fltCurEndValue - 65536;
	}

	// Get text from edit control
	CString strNewEndValue;
	m_editEndValue.GetWindowText( strNewEndValue );
	strNewEndValue.TrimRight();
	strNewEndValue.TrimLeft();

	float fltNewEndValue;

	// Handle unknown text
	if( _tcsncmp( strNewEndValue, achUnknown, 10 ) == 0 )
	{
		return;
	}
	// Handle empty text
	else if( strNewEndValue.IsEmpty() )
	{
		fltNewEndValue = 0; // Set to 0
	}
	else
	{
		fltNewEndValue = StringToStartEndValue( strNewEndValue );
		if( fltNewEndValue == 0xFFFFFFFF)
		{
			fltNewEndValue = fltCurEndValue;	// No change
		}
		else
		{
			// Make sure value is within range
			if( fltNewEndValue < fltMin )
			{
				fltNewEndValue = fltMin;
			}
			if( fltNewEndValue > fltMax )
			{
				fltNewEndValue = fltMax;
			}
		}
	}

	// Text in edit control will by synced in response to the SetData() call, if necessary

	if( fltCurEndValue != fltNewEndValue )
	{
		if(fValueRange > 1)
		{
			SetSpinEndValuePos( (int)fltNewEndValue );
		}
		else
		{
			SetSpinEndValuePos( (int)(0.5 + (fltNewEndValue / fRangeGranularity)));
		}

		m_PropCurve.m_dwChanged = CHGD_ENDVAL;
		m_PropCurve.m_fltEndValue = fltNewEndValue;
		m_pPageManager->m_pIPropPageObject->SetData( &m_PropCurve );
	}
	else
	{
		// Check if edit control's text needs to be updated
		CString strValue;
		CString strCurValue;
		StartEndValueToString( fltNewEndValue, strValue );
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
// CPropPageCurve::OnDeltaPosSpinEndValue

void CPropPageCurve::OnDeltaPosSpinEndValue(NMHDR* pNMHDR, LRESULT* pResult) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_pPageManager->m_pIPropPageObject == NULL
	||  m_fInUpdateControls == TRUE )
	{
		return;
	}

	// Get EndValue range
	float fltMin = 0;
	float fltMax = 127;

	CParamStrip* pParamStrip = (CParamStrip *)m_pPageManager->m_pIPropPageObject;
	ASSERT(pParamStrip);
	if(pParamStrip != NULL)
	{
		fltMin = pParamStrip->m_ParamInfo.mpdMinValue;
		fltMax = pParamStrip->m_ParamInfo.mpdMaxValue;
	}


	// Just in case user clicked on spin control immediately after typing text
	OnKillFocusEndValue();

	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

	float fRangeGranularity = 1;
	float fValueRange = fltMax - fltMin;
	
	if(fValueRange <= 1)
	{
		fRangeGranularity = 0.001f; 
	}

	float fltOldValue  = (float)GetSpinEndValuePos();
	
	if(fltOldValue == fltMax && pNMUpDown->iDelta > 0)
	{
		return;
	}
		
	if(fltOldValue == fltMin && pNMUpDown->iDelta < 0)
	{
		return;
	}

	float fltNewEndValue = 0;  
	if(fValueRange > 1)
	{
		if(pNMUpDown->iDelta > 0)
		{
			fltNewEndValue = (float)GetSpinEndValuePos() + fRangeGranularity;
		}
		else if(pNMUpDown->iDelta < 0)
		{
			fltNewEndValue = (float)GetSpinEndValuePos() - fRangeGranularity;
		}
	}
	else
	{
		if(pNMUpDown->iDelta > 0)
		{
			fltNewEndValue = ((float)GetSpinEndValuePos() * fRangeGranularity) + fRangeGranularity;
		}
		else if(pNMUpDown->iDelta < 0)
		{
			fltNewEndValue = ((float)GetSpinEndValuePos() * fRangeGranularity) - fRangeGranularity;
		}
	}

	
	if(fltNewEndValue > fltMax + 0.5)
	{
		fltNewEndValue = fltNewEndValue - 65536;
	}

	// Make sure value is within range
	if( fltNewEndValue < fltMin )
	{
		fltNewEndValue = fltMin;
	}
	if( fltNewEndValue > fltMax )
	{
		fltNewEndValue = fltMax;
	}

	CString strValue;

	StartEndValueToString( fltNewEndValue, strValue );
	m_editEndValue.SetWindowText( strValue );

	if(fValueRange > 1)
	{
		SetSpinEndValuePos( (int)fltNewEndValue );
	}
	else
	{
		SetSpinEndValuePos( (int)(0.5 + (fltNewEndValue / fRangeGranularity)) );
	}


	m_PropCurve.m_dwChanged = CHGD_ENDVAL;
	m_PropCurve.m_fltEndValue = fltNewEndValue;
	m_pPageManager->m_pIPropPageObject->SetData( &m_PropCurve );
	
	*pResult = 1;
}


/////////////////////////////////////////////////////////////////////////////
// CPropPageCurve::StartEndValueToString

void CPropPageCurve::StartEndValueToString( float fltValue, CString& strValue ) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	strValue.Format( "%.03f", fltValue );
}


/////////////////////////////////////////////////////////////////////////////
// CPropPageCurve::StringToStartEndValue

float CPropPageCurve::StringToStartEndValue( LPCTSTR pszNewValue )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	CString strNewValue = pszNewValue;
	int nLength = strNewValue.GetLength();

	float fltNewValue = (float)atof( strNewValue );

	return fltNewValue;
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
/*	CString strNewEndValue;
	m_editEndValue.GetWindowText( strNewEndValue );
	strNewEndValue.TrimRight();
	strNewEndValue.TrimLeft();

	// Return if string is empty
	if( strNewEndValue.IsEmpty() )
	{
		return;
	}

	float fltMin = 0;
	float fltMax = 127;
	CParamStrip* pParamStrip = (CParamStrip *)m_pPageManager->m_pIPropPageObject;
	ASSERT(pParamStrip);
	if(pParamStrip != NULL)
	{
		fltMin = pParamStrip->m_ParamInfo.mpdMinValue;
		fltMax = pParamStrip->m_ParamInfo.mpdMaxValue;
	}

	if(fltMin >= 0)
	{
		// Return if string equals minus sign
		CString strMinus;
		strMinus.LoadString( IDS_MINUS_TEXT );
		if( strMinus.CompareNoCase(strNewEndValue) == 0 )
		{
			return;
		}
	}

	OnKillFocusEndValue();*/
}

void CPropPageCurve::OnChangeEditStartValue() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	/*if( m_pPageManager->m_pIPropPageObject == NULL
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

	float fltMin = 0;
	float fltMax = 127;
	CParamStrip* pParamStrip = (CParamStrip *)m_pPageManager->m_pIPropPageObject;
	ASSERT(pParamStrip);
	if(pParamStrip != NULL)
	{
		fltMin = pParamStrip->m_ParamInfo.mpdMinValue;
		fltMax = pParamStrip->m_ParamInfo.mpdMaxValue;
	}

	if(fltMin >= 0)
	{
		CString strMinus;
		strMinus.LoadString( IDS_MINUS_TEXT );
		if( strMinus.CompareNoCase(strNewStartValue) == 0 )
		{
			return;
		}
	}

	OnKillFocusStartValue();*/
}

/////////////////////////////////////////////////////////////////////////////
// CPropPageCurve::SetStart

void CPropPageCurve::SetStart( REFERENCE_TIME rtNewStart ) 
{
	CTrackMgr* pTrackMgr = m_pPageManager->m_pTrackMgr;
	ASSERT(pTrackMgr);
	if(pTrackMgr == NULL)
	{
		return;
	}

	REFERENCE_TIME rtSegmentLength = pTrackMgr->GetSegmentLength();
	REFERENCE_TIME rtMinTime = MinTimeToUnknownTime();
	REFERENCE_TIME rtCurEnd = EndToUnknownTime();

	REFERENCE_TIME rtMinStart;
	pTrackMgr->ClocksToUnknownTime( -MAX_TICK, &rtMinStart );

	rtNewStart = max( rtNewStart, rtMinStart );
	rtNewStart = min( rtNewStart, rtSegmentLength - rtMinTime );
	
	REFERENCE_TIME rtDuration = m_PropCurve.m_rtDuration;

	rtDuration = max( rtDuration, rtMinTime );

	if( m_PropCurve.m_rtStartTime != rtNewStart || m_PropCurve.m_rtDuration != rtDuration )
	{
		m_PropCurve.m_rtStartTime = rtNewStart;
		m_PropCurve.m_rtDuration = rtDuration;

		// Now, update the object with the new value(s)
		UpdateObject();
	}
	else
	{
		// Display original values
		UpdateControls( &m_PropCurve );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CPropPageCurve::SetEnd

void CPropPageCurve::SetEnd( REFERENCE_TIME rtNewEnd ) 
{
	CTrackMgr* pTrackMgr = m_pPageManager->m_pTrackMgr;
	ASSERT(pTrackMgr);
	if(pTrackMgr == NULL)
	{
		return;
	}

	REFERENCE_TIME rtSegmentLength = pTrackMgr->GetSegmentLength();
	REFERENCE_TIME rtMinTime = MinTimeToUnknownTime();
	REFERENCE_TIME rtCurStart = StartToUnknownTime();
	REFERENCE_TIME rtCurLength = DurationToUnknownTime();

	REFERENCE_TIME rtMinStart;
	pTrackMgr->ClocksToUnknownTime( -MAX_TICK, &rtMinStart );
	
	REFERENCE_TIME rtNewStart;
	REFERENCE_TIME rtNewLength;
	rtNewLength = rtNewEnd - rtCurStart;
	rtNewStart = rtCurStart;

	rtNewLength = max( rtNewLength, rtMinTime );

	if( m_PropCurve.m_rtStartTime != rtNewStart
	||  m_PropCurve.m_rtDuration != rtNewLength )
	{
		m_PropCurve.m_rtStartTime = rtNewStart;
		m_PropCurve.m_rtDuration = rtNewLength;

		// Now, update the object with the new value(s)
		UpdateObject();
	}
	else
	{
		// Display original values
		UpdateControls( &m_PropCurve );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CPropPageCurve::SetDuration

void CPropPageCurve::SetDuration( REFERENCE_TIME rtNewLength ) 
{
	ASSERT(m_pPageManager);
	if(m_pPageManager == NULL)
	{
		return;
	}

	CTrackMgr* pTrackMgr = m_pPageManager->m_pTrackMgr;
	ASSERT(pTrackMgr);
	if(pTrackMgr == NULL)
	{
		return;
	}

	REFERENCE_TIME rtSegmentLength = pTrackMgr->GetSegmentLength();
	REFERENCE_TIME rtMinTime = MinTimeToUnknownTime();
	REFERENCE_TIME rtCurStart = StartToUnknownTime();
	REFERENCE_TIME rtCurEnd = EndToUnknownTime();

	REFERENCE_TIME rtMinStart;
	pTrackMgr->ClocksToUnknownTime( -MAX_TICK, &rtMinStart );
	
	REFERENCE_TIME rtNewStart = rtCurStart;

	rtNewLength = max( rtNewLength, rtMinTime );
	rtNewLength = min( (rtSegmentLength - rtNewStart), rtNewLength );

	if( m_PropCurve.m_rtStartTime != rtNewStart
	||  m_PropCurve.m_rtDuration != rtNewLength )
	{
		m_PropCurve.m_rtStartTime = rtNewStart;
		m_PropCurve.m_rtDuration = rtNewLength;

		// Now, update the object with the new value(s)
		UpdateObject();
	}
	else
	{
		// Display original values
		UpdateControls( &m_PropCurve );
	}
}


void CPropPageCurve::OnKillfocusEditStartbar() 
{
	CTrackMgr* pTrackMgr = m_pPageManager->m_pTrackMgr;
	ASSERT(pTrackMgr);
	if(pTrackMgr == NULL)
	{
		return;
	}

	int nOffset = 0;
	if( pTrackMgr->IsRefTimeTrack() )
	{
		nOffset = 0;
	}
	else
	{
		nOffset = 1;
	}

	if( HandleKillFocus( m_spinStartBar, m_lStartBar, nOffset ) )
	{
		m_PropCurve.m_dwChanged = CHGD_STARTBAR;
		SetStart( StartToUnknownTime() );
	}
}


void CPropPageCurve::OnDeltaposSpinStartbar(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// Need to do this in case the user clicked the spin control immediately after
	// typing in a value
	OnKillfocusEditStartbar();

	if( HandleDeltaChange( pNMHDR, pResult, m_lStartBar ) )
	{
		m_PropCurve.m_dwChanged = CHGD_STARTBAR;
		SetStart( StartToUnknownTime() );
	}
}




/////////////////////////////////////////////////////////////////////////////
// CPropPageCurve::EndToUnknownTime

REFERENCE_TIME CPropPageCurve::EndToUnknownTime( void )
{
	CTrackMgr* pTrackMgr = m_pPageManager->m_pTrackMgr;
	ASSERT(pTrackMgr);
	if(pTrackMgr == NULL)
	{
		return 0;
	}

	REFERENCE_TIME rtNewEnd = 0;

	if( pTrackMgr->IsRefTimeTrack() )
	{
		REFERENCE_TIME rtTime = 0;
		pTrackMgr->MinSecGridMsToRefTime( m_PropCurve.m_pParamStrip, m_lEndBar, m_lEndBeat, m_lEndGrid, m_lEndTick, &rtTime );
		pTrackMgr->RefTimeToUnknownTime( rtTime, &rtNewEnd );
	}
	else
	{
		MUSIC_TIME mtTime;
		pTrackMgr->MeasureBeatGridTickToClocks( m_lEndBar, m_lEndBeat, m_lEndGrid, m_lEndTick, &mtTime );
		pTrackMgr->ClocksToUnknownTime( mtTime, &rtNewEnd );
	}

	return rtNewEnd;
}




/////////////////////////////////////////////////////////////////////////////
// CPropPageCurve::HandleKillFocus
//
// Generic handler for KillFocus changes
bool CPropPageCurve::HandleKillFocus( CSpinButtonCtrl& spin, long& lUpdateVal, int nOffset )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Check if we have a valid DataObject pointer
	if( m_pPageManager->m_pIPropPageObject == NULL )
	{
		return false;
	}

	// Check if the spin control is a valid window
	if (!::IsWindow(spin.m_hWnd))
	{
		return false;
	}

	// Get text from edit control
	TCHAR tcstrTmp[DIALOG_EDIT_LEN];
	CWnd* pEditCtrl = spin.GetBuddy();
	ASSERT(pEditCtrl != NULL);
	if( pEditCtrl == NULL )
	{
		return false;
	}
	pEditCtrl->GetWindowText( tcstrTmp, DIALOG_EDIT_LEN );

	bool fChanged = false;

	// Handle empty text
	if( tcstrTmp[0] == NULL )
	{
		// Set it back to the minimum value
		spin.SetPos( nOffset );

		// Check if the value changed
		if( lUpdateVal != 0 )
		{
			// Update the item with the new value
			lUpdateVal = 0;
			fChanged = true;
		}
	}
	else if(strcmp(achUnknown, tcstrTmp) == 0)
	{
		return false;
	}
	else
	{
		// Convert from text to an integer
		BOOL fTransSuccess;
		long lNewValue = GetDlgItemInt( pEditCtrl->GetDlgCtrlID(), &fTransSuccess, TRUE );

		// If unable to translate (if dialog contains invalid data, such as '-'), exit early
		if( !fTransSuccess )
		{
			// If conversion failed, update dialog item text
			SetDlgItemInt( pEditCtrl->GetDlgCtrlID(), lUpdateVal + nOffset );
		}
		// If conversion succeeded, ensure the value stays in range
		else
		{
			// Get the valid range
			int iLower, iUpper;
			spin.GetRange( iLower, iUpper );

			// Check if the value is out of range
			if( lNewValue > iUpper )
			{
				// Value out of range, update dialog item text
				SetDlgItemInt( pEditCtrl->GetDlgCtrlID(), iUpper );

				// Update the value to the top of the range
				lNewValue = iUpper;
			}

			// Check if the value changed
			if( lUpdateVal != (lNewValue - nOffset) )
			{
				// Update the item with the new value
				lUpdateVal = (lNewValue - nOffset);
				fChanged = true;
			}
		}
	}

	return fChanged;
}

/////////////////////////////////////////////////////////////////////////////
// CPropPageCurve::HandleDeltaChange
//
// Generic handler for deltapos changes
bool CPropPageCurve::HandleDeltaChange( NMHDR* pNMHDR, LRESULT* pResult, long& lUpdateVal )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_pPageManager->m_pIPropPageObject == NULL )
	{
		return false;
	}

	bool fChanged = false;

	// If the value changed - update the selected item
	if( ((NM_UPDOWN* )pNMHDR)->iDelta != 0 ) 
	{
		// Update the value
		lUpdateVal += ((NM_UPDOWN* )pNMHDR)->iDelta;
		fChanged = true;
	}

	// Set the result to 1 to show that we handled this message
	*pResult = 1;

	return fChanged;
}


/////////////////////////////////////////////////////////////////////////////
// CPropPageCurve::UpdateObject

void CPropPageCurve::UpdateObject( void )
{
	// Check for a valid Property Page Object pointer
	if( m_pPageManager && m_pPageManager->m_pIPropPageObject )
	{
		// Update the Property Page Object with the new item
		m_pPageManager->m_pIPropPageObject->SetData( (void *) &m_PropCurve);
	}
}

void CPropPageCurve::OnKillfocusEditStartbeat() 
{
	int nOffset = 0;
	if( m_pPageManager->m_pTrackMgr->IsRefTimeTrack() )
	{
		nOffset = 0;
	}
	else
	{
		nOffset = 1;
	}

	if( HandleKillFocus( m_spinStartBeat, m_lStartBeat, nOffset ) )
	{
		m_PropCurve.m_dwChanged = CHGD_STARTBEAT;
		SetStart( StartToUnknownTime() );
	}

	
}

void CPropPageCurve::OnKillfocusEditStartgrid() 
{
	if( HandleKillFocus( m_spinStartGrid, m_lStartGrid, 1 ) )
	{
		m_PropCurve.m_dwChanged = CHGD_STARTGRID;
		SetStart( StartToUnknownTime() );
	}
}

void CPropPageCurve::OnKillfocusEditStarttick() 
{
	if( HandleKillFocus( m_spinStartTick, m_lStartTick, 0 ) )
	{
		m_PropCurve.m_dwChanged = CHGD_STARTTICK;
		SetStart( StartToUnknownTime() );
	}
}

void CPropPageCurve::OnKillfocusEditEndbar() 
{
	int nOffset = 0;
	if( m_pPageManager->m_pTrackMgr->IsRefTimeTrack() )
	{
		nOffset = 0;
	}
	else
	{
		nOffset = 1;
	}

	if( HandleKillFocus( m_spinEndBar, m_lEndBar, nOffset ) )
	{
		m_PropCurve.m_dwChanged = CHGD_ENDBAR;
		SetEnd( EndToUnknownTime() );
	}
}

void CPropPageCurve::OnKillfocusEditEndbeat() 
{
	int nOffset = 0;
	if( m_pPageManager->m_pTrackMgr->IsRefTimeTrack() )
	{
		nOffset = 0;
	}
	else
	{
		nOffset = 1;
	}

	if( HandleKillFocus( m_spinEndBeat, m_lEndBeat, nOffset ) )
	{
		m_PropCurve.m_dwChanged = CHGD_ENDBEAT;
		SetEnd( EndToUnknownTime() );
	}
}

void CPropPageCurve::OnKillfocusEditEndgrid() 
{
	if( HandleKillFocus( m_spinEndGrid, m_lEndGrid, 1 ) )
	{
		m_PropCurve.m_dwChanged = CHGD_ENDGRID;
		SetEnd( EndToUnknownTime() );
	}
}

void CPropPageCurve::OnKillfocusEditEndtick() 
{
	if( HandleKillFocus( m_spinEndTick, m_lEndTick, 0 ) )
	{
		m_PropCurve.m_dwChanged = CHGD_ENDTICK;
		SetEnd( EndToUnknownTime() );
	}
}

void CPropPageCurve::OnKillfocusEditDurbar() 
{
	if( HandleKillFocus( m_spinDurBar, m_lDurBar, 0) )
	{
		m_PropCurve.m_dwChanged = CHGD_DURBAR;
		SetDuration( DurationToUnknownTime() );
	}
	
}

void CPropPageCurve::OnKillfocusEditDurbeat() 
{
	if( HandleKillFocus( m_spinDurBeat, m_lDurBeat, 0 ) )
	{
		m_PropCurve.m_dwChanged = CHGD_DURBEAT;
		SetDuration( DurationToUnknownTime() );
	}
	
}

void CPropPageCurve::OnKillfocusEditDurgrid() 
{
	if( HandleKillFocus( m_spinDurGrid, m_lDurGrid, 0 ) )
	{
		m_PropCurve.m_dwChanged = CHGD_DURGRID;
		SetDuration( DurationToUnknownTime() );
	}
}

void CPropPageCurve::OnKillfocusEditDurtick() 
{
	if( HandleKillFocus( m_spinDurTick, m_lDurTick, 0 ) )
	{
		m_PropCurve.m_dwChanged = CHGD_DURTICK;
		SetDuration( DurationToUnknownTime() );
	}
	
}

void CPropPageCurve::OnDeltaposSpinStartbeat(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// Need to do this in case the user clicked the spin control immediately after
	// typing in a value
	OnKillfocusEditStarttick();

	if( HandleDeltaChange( pNMHDR, pResult, m_lStartBeat ) )
	{
		m_PropCurve.m_dwChanged = CHGD_STARTBEAT;
		SetStart( StartToUnknownTime() );
	}

}

void CPropPageCurve::OnDeltaposSpinStartgrid(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// Need to do this in case the user clicked the spin control immediately after
	// typing in a value
	OnKillfocusEditStarttick();

	if( HandleDeltaChange( pNMHDR, pResult, m_lStartGrid ) )
	{
		m_PropCurve.m_dwChanged = CHGD_STARTGRID;
		SetStart( StartToUnknownTime() );
	}
}

void CPropPageCurve::OnDeltaposSpinStarttick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// Need to do this in case the user clicked the spin control immediately after
	// typing in a value
	OnKillfocusEditStarttick();

	if( HandleDeltaChange( pNMHDR, pResult, m_lStartTick ) )
	{
		m_PropCurve.m_dwChanged = CHGD_STARTTICK;
		SetStart( StartToUnknownTime() );
	}
}

void CPropPageCurve::OnDeltaposSpinEndbar(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// Need to do this in case the user clicked the spin control immediately after
	// typing in a value
	OnKillfocusEditEndbar();

	if( HandleDeltaChange( pNMHDR, pResult, m_lEndBar) )
	{
		m_PropCurve.m_dwChanged = CHGD_ENDBAR;
		SetEnd( EndToUnknownTime() );
	}
}

void CPropPageCurve::OnDeltaposSpinEndbeat(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// Need to do this in case the user clicked the spin control immediately after
	// typing in a value
	OnKillfocusEditEndbeat();

	if( HandleDeltaChange( pNMHDR, pResult, m_lEndBeat ) )
	{
		m_PropCurve.m_dwChanged = CHGD_ENDBEAT;
		SetEnd( EndToUnknownTime() );
	}
}

void CPropPageCurve::OnDeltaposSpinEndgrid(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// Need to do this in case the user clicked the spin control immediately after
	// typing in a value
	OnKillfocusEditEndgrid();

	if( HandleDeltaChange( pNMHDR, pResult, m_lEndGrid ) )
	{
		m_PropCurve.m_dwChanged = CHGD_ENDGRID;
		SetEnd( EndToUnknownTime() );
	}

}

void CPropPageCurve::OnDeltaposSpinEndtick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// Need to do this in case the user clicked the spin control immediately after
	// typing in a value
	OnKillfocusEditEndtick();

	if( HandleDeltaChange( pNMHDR, pResult, m_lEndTick ) )
	{
		m_PropCurve.m_dwChanged = CHGD_ENDTICK;
		SetEnd( EndToUnknownTime() );
	}
}

void CPropPageCurve::OnDeltaposSpinDurbar(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// Need to do this in case the user clicked the spin control immediately after
	// typing in a value
	OnKillfocusEditDurbar();

	if( HandleDeltaChange( pNMHDR, pResult, m_lDurBar ) )
	{
		m_PropCurve.m_dwChanged = CHGD_DURBAR;
		SetDuration( DurationToUnknownTime() );
	}
}

void CPropPageCurve::OnDeltaposSpinDurbeat(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// Need to do this in case the user clicked the spin control immediately after
	// typing in a value
	OnKillfocusEditDurbeat();

	if( HandleDeltaChange( pNMHDR, pResult, m_lDurBeat ) )
	{
		m_PropCurve.m_dwChanged = CHGD_DURBEAT;
		SetDuration( DurationToUnknownTime() );
	}
}

void CPropPageCurve::OnDeltaposSpinDurgrid(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// Need to do this in case the user clicked the spin control immediately after
	// typing in a value
	OnKillfocusEditDurgrid();

	if( HandleDeltaChange( pNMHDR, pResult, m_lDurGrid ) )
	{
		m_PropCurve.m_dwChanged = CHGD_DURGRID;
		SetDuration( DurationToUnknownTime() );
	}
}

void CPropPageCurve::OnDeltaposSpinDurtick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// Need to do this in case the user clicked the spin control immediately after
	// typing in a value
	OnKillfocusEditDurtick();

	if( HandleDeltaChange( pNMHDR, pResult, m_lDurTick ) )
	{
		m_PropCurve.m_dwChanged = CHGD_DURTICK;
		SetDuration( DurationToUnknownTime() );
	}
}



/////////////////////////////////////////////////////////////////////////////
// CPropPageCurve::SetEditControl

void CPropPageCurve::SetEditControl( CEdit& edit, long lValue, int nOffset )
{
	TCHAR tcstrTmp[DIALOG_EDIT_LEN];
	BOOL fTransSuccess;

	edit.GetWindowText( tcstrTmp, DIALOG_EDIT_LEN );

	// Convert from text to an integer
	long lCurValue = GetDlgItemInt( edit.GetDlgCtrlID(), &fTransSuccess, TRUE );

	// Check if the conversion failed, the text is empty, or if the values are different
	if( !fTransSuccess
	|| (tcstrTmp[0] == NULL)
	|| (lCurValue != lValue + nOffset) )
	{
		// Update the displayed number
		SetDlgItemInt( edit.GetDlgCtrlID(), lValue + nOffset );
	}
}

void CPropPageCurve::OnCheckStartFromCurrent() 
{
	m_PropCurve.m_dwChanged = CHGD_STARTCURRENT;

	int nCheck = m_btnStartFromCurrent.GetCheck();
	m_PropCurve.m_fStartFromCurrent = nCheck;
	m_pPageManager->m_pIPropPageObject->SetData( &m_PropCurve );
}

void CPropPageCurve::OnSelchangeComboEnumEndValue() 
{
	int nIndex = m_comboEnumEndValue.GetCurSel();
	DWORD dwValue = m_comboEnumEndValue.GetItemData(nIndex);
	m_PropCurve.m_dwChanged = CHGD_ENDVAL;
	m_PropCurve.m_fltEndValue = dwValue;
	m_pPageManager->m_pIPropPageObject->SetData( &m_PropCurve );
}

void CPropPageCurve::OnSelchangeComboEnumStartValue() 
{
	int nIndex = m_comboEnumStartValue.GetCurSel();
	DWORD dwValue = m_comboEnumStartValue.GetItemData(nIndex);
	m_PropCurve.m_dwChanged = CHGD_STARTVAL;
	m_PropCurve.m_fltStartValue = dwValue;
	m_pPageManager->m_pIPropPageObject->SetData( &m_PropCurve );
}

void CPropPageCurve::OnKillfocusComboEnumStartValue() 
{
	CString sComboSel;
	m_comboEnumStartValue.GetWindowText(sComboSel);

	if(sComboSel.CompareNoCase(achUnknown) == 0)
	{
		return;
	}

	if(m_comboEnumStartValue.SelectString(0, sComboSel) == CB_ERR)
	{
		m_comboEnumStartValue.SetCurSel(m_PropCurve.m_fltStartValue);
		return;
	}

	OnSelchangeComboEnumStartValue();
}

void CPropPageCurve::OnKillfocusComboEnumEndValue() 
{
	CString sComboSel;
	m_comboEnumEndValue.GetWindowText(sComboSel);

	if(sComboSel.CompareNoCase(achUnknown) == 0)
	{
		return;
	}

	if(m_comboEnumEndValue.SelectString(0, sComboSel) == CB_ERR)
	{
		m_comboEnumEndValue.SetCurSel(m_PropCurve.m_fltEndValue);
		return;
	}

	OnSelchangeComboEnumEndValue();
}

void CPropPageCurve::OnKillfocusComboShape() 
{
	CString sComboSel;
	m_comboShape.GetWindowText(sComboSel);

	if(sComboSel.CompareNoCase(achUnknown) == 0)
	{
		return;
	}

	if(m_comboShape.SelectString(0, sComboSel) == CB_ERR)
	{
		InitShapeCombo(&m_PropCurve);
		return;
	}

	OnSelChangeComboShape();
}


void CPropPageCurve::InitShapeCombo(CPropCurve* pPropCurve)
{
	ASSERT(pPropCurve);
	if(pPropCurve == NULL)
	{
		return;
	}

	if(pPropCurve->m_dwUndetermined & UNDT_SHAPE)
	{
		m_comboShape.SetCurSel(-1);
		m_comboShape.SetWindowText(achUnknown);
	}
	else
	{
		if(pPropCurve->m_dwCurveShape == MP_CURVE_SINE)
		{
			m_comboShape.SetCurSel(0);
		}
		else if(pPropCurve->m_dwCurveShape ==  MP_CURVE_INVSQUARE)
		{
			m_comboShape.SetCurSel(1);
		}
		else if(pPropCurve->m_dwCurveShape == MP_CURVE_SQUARE)
		{
			m_comboShape.SetCurSel(2);
		}
		else if(pPropCurve->m_dwCurveShape == MP_CURVE_JUMP)
		{
			m_comboShape.SetCurSel(3);
		}
		else if(pPropCurve->m_dwCurveShape == MP_CURVE_LINEAR)
		{
			m_comboShape.SetCurSel(4);
		}
		else
		{
			m_comboShape.SetCurSel(-1);
		}
	}
}