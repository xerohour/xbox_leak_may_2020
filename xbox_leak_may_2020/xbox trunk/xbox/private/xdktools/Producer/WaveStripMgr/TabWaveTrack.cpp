// TabWaveTrack.cpp : implementation file
//

#include "stdafx.h"
#include <RiffStrm.h>
#include "TrackMgr.h"
#include "TabWaveTrack.h"
#include "GroupBitsPPG.h"
#include "Timeline.h"
#include <dmusicf.h>
#include <dmusici.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CTabWaveTrack property page

CTabWaveTrack::CTabWaveTrack() :
	CPropertyPage(CTabWaveTrack::IDD),
	CSliderCollection(1),
	m_pmsAttenuation(NULL)
{
	//{{AFX_DATA_INIT(CTabWaveTrack)
	//}}AFX_DATA_INIT

	m_pPPO = NULL;
	m_pPropPageMgr = NULL;
	m_pIPropSheet = NULL;

	m_PPGTrackParams.dwPageIndex = 2;
	m_PPGTrackParams.lVolume = 0;
	m_PPGTrackParams.dwTrackFlagsDM = 0;

	CSliderCollection::Init(this);
	m_lAttenuation = 0;

	m_fNeedToDetach = FALSE;
}

CTabWaveTrack::~CTabWaveTrack()
{
	if( m_pPPO )
	{
		m_pPPO->Release();
	}
}

void CTabWaveTrack::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTabWaveTrack)
	DDX_Control(pDX, IDC_PERSIST, m_checkPersistVariationControl);
	DDX_Control(pDX, IDC_SYNC, m_checkSyncVariations);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTabWaveTrack, CPropertyPage)
	//{{AFX_MSG_MAP(CTabWaveTrack)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_SYNC, OnSync)
	ON_BN_DOUBLECLICKED(IDC_SYNC, OnDoubleClickedSync)
	ON_BN_CLICKED(IDC_PERSIST, OnPersist)
	ON_BN_DOUBLECLICKED(IDC_PERSIST, OnDoubleClickedPersist)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// private functions
void CTabWaveTrack::SetObject( IDMUSProdPropPageObject* pPPO )
{
	if( m_pPPO )
	{
		m_pPPO->Release();
	}
	m_pPPO = pPPO;
	if( m_pPPO )
	{
		m_pPPO->AddRef();
	}
}

void CTabWaveTrack::EnableControls( BOOL fEnable )
{
	if (m_pmsAttenuation == NULL)
		return;

	m_pmsAttenuation->EnableControl(this, fEnable != 0);
	m_checkSyncVariations.EnableWindow( fEnable );
}

/////////////////////////////////////////////////////////////////////////////
// CTabWaveTrack message handlers

int CTabWaveTrack::OnCreate(LPCREATESTRUCT lpCreateStruct) 
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

void CTabWaveTrack::OnDestroy() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// free the sliders
	CSliderCollection::Free();
	m_pmsAttenuation = NULL;

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

BOOL CTabWaveTrack::OnInitDialog() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	CPropertyPage::OnInitDialog();

	ASSERT(m_pmsAttenuation == NULL);
	m_pmsAttenuation = Insert(
		IDC_ATTENUATION,
		IDC_DB_ATTENUATION,	
		IDC_DB_ATTENUATION_SPIN,
		MYSLIDER_VOLUME,
		0,
		0, // no undo
		&m_lAttenuation );


	return FALSE;  // return TRUE unless you set the focus to a control
	               // EXCEPTION: OCX Property Pages should return FALSE
}

void CTabWaveTrack::RefreshData( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_pPPO == NULL )
	{
		EnableControls( FALSE );
		return;
	}

	ioWaveTrackPPG* pioWaveTrackPPG = &m_PPGTrackParams;
	if( FAILED( m_pPPO->GetData( (void**)&pioWaveTrackPPG ) ) )
	{
		EnableControls( FALSE );
		return;
	}

	// Make sure controls have been created
	if( ::IsWindow(m_hWnd) == FALSE )
	{
		return;
	}

	// Prevent control notifications from being dispatched during UpdateData
	_AFX_THREAD_STATE* pThreadState = AfxGetThreadState();
	HWND hWndOldLockout = pThreadState->m_hLockoutNotifyWindow;
	ASSERT(hWndOldLockout != m_hWnd);   // must not recurse
	pThreadState->m_hLockoutNotifyWindow = m_hWnd;

	EnableControls( TRUE );

	// Set volume control
	m_lAttenuation = (m_PPGTrackParams.lVolume << 16) / 10;
	m_pmsAttenuation->SetValue(this, m_lAttenuation);

	// Set flag controls
	m_checkSyncVariations.SetCheck( (m_PPGTrackParams.dwTrackFlagsDM & DMUS_WAVETRACKF_SYNC_VAR) ? 1 : 0 );
	m_checkPersistVariationControl.SetCheck( (m_PPGTrackParams.dwTrackFlagsDM & DMUS_WAVETRACKF_PERSIST_CONTROL) ? 0 : 1 );

	pThreadState->m_hLockoutNotifyWindow = hWndOldLockout;
}

BOOL CTabWaveTrack::OnSetActive() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	RefreshData();

	// Store active tab
	m_pIPropSheet->GetActivePage( &CGroupBitsPropPageMgr::sm_nActiveTab );

	return CPropertyPage::OnSetActive();
}

void CTabWaveTrack::UpdatePPO()
{
	if( m_pPPO )
	{
		VERIFY( SUCCEEDED( m_pPPO->SetData(&m_PPGTrackParams) ) );
	}
}

void CTabWaveTrack::OnHScroll( UINT nSBCode, UINT nPos, CScrollBar* pScrollBar ) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	CSliderCollection::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CTabWaveTrack::OnSync() 
{
	if( m_checkSyncVariations.GetCheck() )
	{
		m_PPGTrackParams.dwTrackFlagsDM |= DMUS_WAVETRACKF_SYNC_VAR;
	}
	else
	{
		m_PPGTrackParams.dwTrackFlagsDM &= ~DMUS_WAVETRACKF_SYNC_VAR;
	}

	UpdatePPO();
}

void CTabWaveTrack::OnDoubleClickedSync() 
{
	OnSync();
}

void CTabWaveTrack::OnPersist() 
{
	if( m_checkPersistVariationControl.GetCheck() )
	{
		m_PPGTrackParams.dwTrackFlagsDM &= ~DMUS_WAVETRACKF_PERSIST_CONTROL;
	}
	else
	{
		m_PPGTrackParams.dwTrackFlagsDM |= DMUS_WAVETRACKF_PERSIST_CONTROL;
	}

	UpdatePPO();
}

void CTabWaveTrack::OnDoubleClickedPersist() 
{
	OnPersist();
}

BOOL CTabWaveTrack::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	LRESULT lResult;
	if (CSliderCollection::OnCommand(wParam, lParam, &lResult))
		return lResult;

	return CPropertyPage::OnCommand(wParam, lParam);
}

BOOL CTabWaveTrack::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	LRESULT lResult;
	if (CSliderCollection::OnNotify(wParam, lParam, &lResult))
		return lResult;
	
	return CPropertyPage::OnNotify(wParam, lParam, pResult);
}

// CSliderCollection override
bool CTabWaveTrack::OnSliderUpdate(MySlider *pms, DWORD dwmscupdf)
{
	switch (dwmscupdf)
		{
		case dwmscupdfStart:
			return true; // save state is done on End of updates

		case dwmscupdfEnd:
			ASSERT(pms == m_pmsAttenuation); // slider not handled
			m_PPGTrackParams.lVolume = (m_lAttenuation >> 16) * 10;
			UpdatePPO();
			return true;

		default:
			ASSERT(FALSE);
			return false;
		}
}

