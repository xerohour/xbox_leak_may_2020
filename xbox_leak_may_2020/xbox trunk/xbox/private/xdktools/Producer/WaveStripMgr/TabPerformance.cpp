// TabPerformance.cpp : implementation file
//

#include "stdafx.h"
#include <RiffStrm.h>
#include "TrackMgr.h"
#include "TrackItem.h"
#include "PropTrackItem.h"
#include "PropPageMgr_Item.h"
#include "TabPerformance.h"
#include <dmusici.h>
#include <dmusicf.h>
#include <math.h>
#include "LockoutNotification.h"

// This sets up information for Visual C++'s memory leak tracing
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// Length (in characters) of the string to allocate to store text entered in
// the measure and beat edit boxes
#define DIALOG_EDIT_LEN 15


static const TCHAR g_pstrUnknown[11] = _T("----------");


/////////////////////////////////////////////////////////////////////////////
// CTabPerformance property page

IMPLEMENT_DYNCREATE(CTabPerformance, CPropertyPage)

/////////////////////////////////////////////////////////////////////////////
// CTabPerformance constructor/destructor

CTabPerformance::CTabPerformance():
	CPropertyPage(CTabPerformance::IDD), CSliderCollection(4),
	m_pmsAttenuation(NULL), m_pmsFineTune(NULL)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

//	CPropertyPage(IDD_PROPPAGE_WAVE_PERFORMANCE);
	//{{AFX_DATA_INIT(CTabPerformance)
	//}}AFX_DATA_INIT

    m_pmsAttenuationRange = NULL;
    m_lAttenuationRange = 0;
    m_pmsFineTuneRange = NULL;
    m_lFineTuneRange = 0;

    // Initialize our pointers to NULL
	m_pPropPageMgr = NULL;
	m_pTimeline = NULL;

	// Initialize our class members to FALSE
	m_fValidTrackItem = FALSE;
	m_fNeedToDetach = FALSE;

	m_lAttenuation = 0;
	m_lFineTune = 0;

	m_lLogicalMeasure = 0;
	m_lLogicalBeat = 0;

	CSliderCollection::Init(this);
}

CTabPerformance::~CTabPerformance()
{
}


/////////////////////////////////////////////////////////////////////////////
// CTabPerformance::DoDataExchange

void CTabPerformance::DoDataExchange(CDataExchange* pDX)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	CPropertyPage::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(CTabPerformance)
	DDX_Control(pDX, IDC_DB_ATTENUATION_RANGE, m_dbAttenuationRange);
	DDX_Control(pDX, IDC_ATTENUATION_RANGE, m_AttenuationRange);
	DDX_Control(pDX, IDC_CHECK_IGNORE_LOOPING, m_checkIgnoreLooping);
	DDX_Control(pDX, IDC_EDIT_BELONGS_BEAT, m_editLogicalBeat);
	DDX_Control(pDX, IDC_SPIN_BELONGS_BEAT, m_spinLogicalBeat);
	DDX_Control(pDX, IDC_SPIN_BELONGS_BAR, m_spinLogicalMeasure);
	DDX_Control(pDX, IDC_EDIT_BELONGS_BAR, m_editLogicalMeasure);
	DDX_Control(pDX, IDC_CHECK_NOINVALIDATE, m_checkNoInvalidate);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTabPerformance, CPropertyPage)
	//{{AFX_MSG_MAP(CTabPerformance)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_CHECK_NOINVALIDATE, OnCheckNoInvalidate)
	ON_BN_DOUBLECLICKED(IDC_CHECK_NOINVALIDATE, OnDoubleClickedCheckNoInvalidate)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_BELONGS_BAR, OnSpinLogicalMeasure)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_BELONGS_BEAT, OnSpinLogicalBeat)
	ON_EN_KILLFOCUS(IDC_EDIT_BELONGS_BAR, OnKillFocusLogicalMeasure)
	ON_EN_KILLFOCUS(IDC_EDIT_BELONGS_BEAT, OnKillFocusLogicalBeat)
	ON_BN_CLICKED(IDC_CHECK_IGNORE_LOOPING, OnCheckIgnoreLooping)
	ON_BN_DOUBLECLICKED(IDC_CHECK_IGNORE_LOOPING, OnDoubleclickedCheckIgnoreLooping)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTabPerformance custom functions

/////////////////////////////////////////////////////////////////////////////
// CTabPerformance::SetPropTrackItem

void CTabPerformance::SetPropTrackItem( const CPropTrackItem* pPropItem )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// If the pointer is null, there are no items selected, so disable the property page
	if( pPropItem == NULL
	||  pPropItem->m_Item.m_FileRef.pIDocRootNode == NULL )
	{
		m_fValidTrackItem = FALSE;
		m_PropItem.Clear();
		EnableControls( FALSE );
		return;
	}

	// Initialize the timeline pointer to NULL
	m_pTimeline = NULL;

	// Initialize our groupbits to all 32 groups
	DWORD dwGroupBits = 0xFFFFFFFF;

	// Verify we have a valid pinter to our Property Page Object
	if( m_pPropPageMgr
	&&  m_pPropPageMgr->m_pIPropPageObject )
	{
		// Query the Property Page Object for an IDMUSProdStripMgr interface
		IDMUSProdStripMgr *pStripMgr;
		if( SUCCEEDED( m_pPropPageMgr->m_pIPropPageObject->QueryInterface( IID_IDMUSProdStripMgr, (void**)&pStripMgr ) ) )
		{
			// Ask the strip manager for an IDMUSProdTimeline pointer
			VARIANT variant;
			if( SUCCEEDED( pStripMgr->GetStripMgrProperty( SMP_ITIMELINECTL, &variant ) ) )
			{
				// Query the returned IUnknown pointer for an IDMUSProdTimeline Poiter
				if( SUCCEEDED( V_UNKNOWN( &variant )->QueryInterface( IID_IDMUSProdTimeline, (void**)&m_pTimeline ) ) )
				{
					// Successfully got an IDMUSProdTimeline pointer.
					// Now, release it since we only want a weak reference
					m_pTimeline->Release();
				}

				// Release the returned IUnknown pointer
				V_UNKNOWN( &variant )->Release();
			}

			// Ask the strip mangaer for its track header, so we can read the group bits from it
			DMUS_IO_TRACK_HEADER ioTrackHeader;
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

	// Flag that we have a valid item
	m_fValidTrackItem = TRUE;

	// Copy the information from the new item
	m_PropItem.Copy( pPropItem );
	m_PropItem.m_nPropertyTab = TAB_PERFORMANCE;
	m_PropItem.m_dwChanged = 0;

	// Compute values for controls displaying "time"
	// Determine measure, beat
	m_lLogicalMeasure = 0;
	m_lLogicalBeat = 0;
	if( m_pPropPageMgr->m_pTrackMgr->IsRefTimeTrack() == false )
	{
		long lGrid, lTick;
		m_pPropPageMgr->m_pTrackMgr->ClocksToMeasureBeatGridTick( (MUSIC_TIME)m_PropItem.m_Item.m_rtTimeLogical,
																  &m_lLogicalMeasure, &m_lLogicalBeat, &lGrid, &lTick );
	}

	// Check to see if the controls have been created
	if( m_pmsAttenuation == NULL )
	{
		// It doesn't have a valid window handle - the property page may have been destroyed (or not yet created).
		// Just exit early
		return;
	}

	// Don't send OnKill/OnUpdate notifications when updating the display
	CLockoutNotification LockoutNotifications( m_hWnd );

	// Update the property page based on the new data.
	// It is usually profitable to check to see if the data actually changed
	// before updating the controls in the property page.

	// Enable all the controls
	EnableControls( TRUE );

	// Set volume control
	if( m_PropItem.m_dwUndetermined_TabPerformance & UNDT_VOLUME )
	{
		m_lAttenuation = 0;
		m_pmsAttenuation->SetUndetermined(this);
	}
	else
	{
		m_lAttenuation = (m_PropItem.m_Item.m_lVolume << 16) / 10;
		m_pmsAttenuation->SetValue(this, m_lAttenuation);
	}

	// Set fine tune control
	if( m_PropItem.m_dwUndetermined_TabPerformance & UNDT_PITCH )
	{
		m_lFineTune = 0;
		m_pmsFineTune->SetUndetermined(this);
	}
	else
	{
		m_lFineTune = (m_PropItem.m_Item.m_lPitch << 16);
		m_pmsFineTune->SetValue(this, m_lFineTune);
	}

    // Set volume control
	if( m_PropItem.m_dwUndetermined_TabPerformance & UNDT_VOLUME_RANGE )
	{
		m_lAttenuationRange = 0;
		m_pmsAttenuationRange->SetUndetermined(this);
	}
	else
	{
		m_lAttenuationRange = (m_PropItem.m_Item.m_lVolumeRange << 16) / 10;
		m_pmsAttenuationRange->SetValue(this, m_lAttenuationRange);
	}

	// Set fine tune control
	if( m_PropItem.m_dwUndetermined_TabPerformance & UNDT_PITCH_RANGE )
	{
		m_lFineTuneRange = 0;
		m_pmsFineTuneRange->SetUndetermined(this);
	}
	else
	{
		m_lFineTuneRange = (m_PropItem.m_Item.m_lPitchRange << 16) / 10;
		m_pmsFineTuneRange->SetValue(this, m_lFineTuneRange);
	}

    // Set DMUS_WAVEF_NOINVALIDATE 
	if( m_PropItem.m_dwUndetermined_TabPerformance & UNDT_WAVEF_NOINVALIDATE )
	{
		m_checkNoInvalidate.SetCheck( 2 );
	}
	else
	{
		m_checkNoInvalidate.SetCheck( (m_PropItem.m_Item.m_dwFlagsDM & DMUS_WAVEF_NOINVALIDATE) ? 1 : 0 );
	}

	// Set DMUS_WAVEF_IGNORELOOPS 
	if( m_PropItem.m_dwUndetermined_TabPerformance & UNDT_WAVEF_IGNORELOOPS )
	{
		m_checkIgnoreLooping.SetCheck( 2 );
	}
	else
	{
		m_checkIgnoreLooping.SetCheck( (m_PropItem.m_Item.m_dwFlagsDM & DMUS_WAVEF_IGNORELOOPS) ? 1 : 0 );
	}

    // Set BELONGS TO controls
	if( m_pPropPageMgr->m_pTrackMgr->IsRefTimeTrack() == false )
	{
		if( m_PropItem.m_dwUndetermined_TabPerformance & UNDT_TIME_LOGICAL )
		{
			m_editLogicalMeasure.SetWindowText( g_pstrUnknown );
			m_editLogicalBeat.SetWindowText( g_pstrUnknown );
		}
		else
		{
			SetEditControl( m_editLogicalMeasure, m_lLogicalMeasure, 1 );
			SetEditControl( m_editLogicalBeat, m_lLogicalBeat, 1 );
		}
	}

	// Set ranges of spin controls
	SetControlRanges();
}
	

/////////////////////////////////////////////////////////////////////////////
// CTabPerformance::SetEditControl

void CTabPerformance::SetEditControl( CEdit& edit, long lValue, int nOffset )
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


/////////////////////////////////////////////////////////////////////////////
// CTabPerformance::SetControlRanges

void CTabPerformance::SetControlRanges( void )
{
	// Get track group bits
	DWORD dwGroupBits = 0xFFFFFFFF;
	if( m_pPropPageMgr
	&&  m_pPropPageMgr->m_pIPropPageObject )
	{
		// Query the Property Page Object for an IDMUSProdStripMgr interface
		IDMUSProdStripMgr *pStripMgr;
		if( SUCCEEDED( m_pPropPageMgr->m_pIPropPageObject->QueryInterface( IID_IDMUSProdStripMgr, (void**)&pStripMgr ) ) )
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
	SetMusicTimeRanges( dwGroupBits );
}


/////////////////////////////////////////////////////////////////////////////
// CTabPerformance::SetMusicTimeRanges

void CTabPerformance::SetMusicTimeRanges( DWORD dwGroupBits )
{
	long lMaxMeasure, lMaxBeat, lMaxGrid;

	// Get the maximum measure value
	if( m_pPropPageMgr->m_pTrackMgr->m_pTimeline == NULL )
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
		m_pPropPageMgr->m_pTrackMgr->m_pTimeline->GetTimelineProperty( TP_CLOCKLENGTH, &var );

		// Convert from clocks to a measure and beat value
		m_pPropPageMgr->m_pTrackMgr->m_pTimeline->ClocksToMeasureBeat( dwGroupBits, 0,
																	   V_I4( &var ), &lMaxMeasure, &lMaxBeat );

		// If the beat value is zero, check if the segment is exactly lMaxMeasure in length
		if( lMaxBeat == 0 )
		{
			// Convert from lMaxMeasure to a clock value
			long lClockLength;
			m_pPropPageMgr->m_pTrackMgr->m_pTimeline->MeasureBeatToClocks( dwGroupBits, 0, lMaxMeasure, 0, &lClockLength );

			// Check if this clock value is equal to the length
			if( lClockLength == V_I4( &var ) )
			{
				// Exactly lMaxMeasures long.  Get the measure and beat value for the previous tick
				m_pPropPageMgr->m_pTrackMgr->m_pTimeline->ClocksToMeasureBeat( dwGroupBits, 0,
																			   V_I4( &var ) - 1, &lMaxMeasure, &lMaxBeat );
			}
		}

		long lItemMeasure, lItemBeat, lItemGrid, lItemTick;
		m_pPropPageMgr->m_pTrackMgr->UnknownTimeToMeasureBeatGridTick( m_PropItem.m_Item.m_rtTimePhysical,
												&lItemMeasure, &lItemBeat, &lItemGrid, &lItemTick );

		// If the item is not in the last measure
		if( lItemMeasure < lMaxMeasure )
		{
			// Convert from a Measure value to a clock value
			long lClockForMeasure;
			m_pPropPageMgr->m_pTrackMgr->m_pTimeline->MeasureBeatToClocks( dwGroupBits, 0, lItemMeasure, 0, &lClockForMeasure );

			// Get the TimeSig for this measure
			DMUS_TIMESIGNATURE TimeSig;
			if( SUCCEEDED( m_pPropPageMgr->m_pTrackMgr->m_pTimeline->GetParam( GUID_TimeSignature, dwGroupBits, 0, lClockForMeasure, NULL, &TimeSig ) ) )
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
	m_spinLogicalMeasure.SetRange( 1, lMaxMeasure );
	m_editLogicalMeasure.LimitText( 5 );

	// Update the range for the beat spin control
	m_spinLogicalBeat.SetRange( 1, lMaxBeat );
	m_editLogicalBeat.LimitText( 3 );
}


/////////////////////////////////////////////////////////////////////////////
// CTabPerformance message handlers

/////////////////////////////////////////////////////////////////////////////
// CTabPerformance::OnCreate

int CTabPerformance::OnCreate( LPCREATESTRUCT lpCreateStruct ) 
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

	// Call the CPropertyPage's OnCreate method to actually create all our controls from our dialog resource.
	if( CPropertyPage::OnCreate(lpCreateStruct) == -1 )
	{
		return -1;
	}

	return 0;
}


/////////////////////////////////////////////////////////////////////////////
// CTabPerformance::OnDestroy

void CTabPerformance::OnDestroy( void ) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// free the sliders
	CSliderCollection::Free();
	m_pmsAttenuation = m_pmsFineTune = NULL;
    m_pmsAttenuationRange = m_pmsFineTuneRange = NULL;
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

	// Call the CPropertyPage's OnDestroy method to actually destroy all our child controls.
	CPropertyPage::OnDestroy();	
}


/////////////////////////////////////////////////////////////////////////////
// CTabPerformance::OnInitDialog

BOOL CTabPerformance::OnInitDialog( void ) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Call the base class's OnInitDialog method
	CPropertyPage::OnInitDialog();

	ASSERT(m_pmsAttenuation == NULL); // this should be first time, or after OnDestroy

	m_pmsAttenuation = Insert(
		IDC_ATTENUATION,		// slider
		IDC_DB_ATTENUATION,	// editbox
		IDC_DB_ATTENUATION_SPIN,
		MYSLIDER_VOLUME,
		0,
		0, // undo handled in UpdateObject
		&m_lAttenuation );

	m_pmsFineTune = Insert(
		IDC_FINE_TUNE,		// slider
		IDC_SEMI_TONES,		// editbox
		IDC_SEMI_TONES_SPIN,
		MYSLIDER_PITCHCENTS,
		0,
		0, // undo handled in UpdateObject
		&m_lFineTune );
	m_pmsAttenuationRange = Insert(
		IDC_ATTENUATION_RANGE,		// slider
		IDC_DB_ATTENUATION_RANGE,	// editbox
		IDC_DB_ATTENUATION_SPIN_RANGE,
		MYSLIDER_VOLUMECENTS,
		0,
		0, // undo handled in UpdateObject
		&m_lAttenuationRange );
	m_pmsFineTuneRange = Insert(
		IDC_FINE_TUNE_RANGE,		// slider
		IDC_SEMI_TONES_RANGE,		// editbox
		IDC_SEMI_TONES_SPIN_RANGE,
		MYSLIDER_VOLUMECENTS,
		0,
		0, // undo handled in UpdateObject
		&m_lFineTuneRange );
/*#ifndef DMP_XBOX
    GetDlgItem( IDC_ATTENUATION_RANGE_NAME )->ShowWindow(SW_HIDE );
    GetDlgItem( IDC_ATTENUATION_RANGE )->ShowWindow(SW_HIDE );
    GetDlgItem( IDC_DB_ATTENUATION_RANGE )->ShowWindow(SW_HIDE );
    GetDlgItem( IDC_DB_ATTENUATION_RANGE_NAME )->ShowWindow(SW_HIDE );
    GetDlgItem( IDC_DB_ATTENUATION_SPIN_RANGE )->ShowWindow(SW_HIDE );
    GetDlgItem( IDC_SEMI_TONES_SPIN_RANGE )->ShowWindow(SW_HIDE );
    GetDlgItem( IDC_STATIC_FINE_TUNE_RANGE )->ShowWindow(SW_HIDE );
    GetDlgItem( IDC_FINE_TUNE_RANGE )->ShowWindow(SW_HIDE );
    GetDlgItem( IDC_SEMI_TONES_RANGE )->ShowWindow(SW_HIDE );
    GetDlgItem( IDC_STATIC_SEMI_TONES_RANGE )->ShowWindow(SW_HIDE );

    m_checkIgnoreLooping.ShowWindow( SW_HIDE );
#endif*/

	return TRUE;  // return TRUE unless you set the focus to a control
	               // EXCEPTION: OCX Property Pages should return FALSE
}


/////////////////////////////////////////////////////////////////////////////
// CTabPerformance::UpdateObject

void CTabPerformance::UpdateObject( void )
{
	// Check for a valid Property Page Object pointer
	if( m_pPropPageMgr
	&&  m_pPropPageMgr->m_pIPropPageObject )
	{
		// Update the Property Page Object with the new item
		m_pPropPageMgr->m_pIPropPageObject->SetData( (void *)&m_PropItem );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabPerformance::EnableControls

void CTabPerformance::EnableControls( BOOL fEnable )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Make sure controls have been created
	if (m_pmsAttenuation == NULL)
	{
		return;
	}

	m_pmsAttenuation->EnableControl(this, fEnable != FALSE);
	m_pmsFineTune->EnableControl(this, fEnable != FALSE);
//#ifdef DMP_XBOX
	m_pmsAttenuationRange->EnableControl(this, fEnable != FALSE);
	m_pmsFineTuneRange->EnableControl(this, fEnable != FALSE);
//#endif
	m_checkNoInvalidate.EnableWindow( fEnable );
	m_checkIgnoreLooping.EnableWindow( fEnable );

	BOOL fEnableBelongsTo = FALSE;
	if( m_pPropPageMgr->m_pTrackMgr->IsRefTimeTrack() == false )
	{
		fEnableBelongsTo = fEnable;
	}

	m_spinLogicalBeat.EnableWindow( fEnableBelongsTo );
	m_spinLogicalMeasure.EnableWindow( fEnableBelongsTo );
	m_editLogicalBeat.EnableWindow( fEnableBelongsTo );
	m_editLogicalMeasure.EnableWindow( fEnableBelongsTo );

	if( fEnableBelongsTo == FALSE )
	{
		m_editLogicalMeasure.SetWindowText( NULL );
		m_editLogicalBeat.SetWindowText( NULL );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabPerformance::OnOK

// Windows translated presses of the 'Enter' key into presses of the button 'OK'.
// Here we create a hidden 'OK' button and translate presses of it into tabs to the next
// control in the property page.
void CTabPerformance::OnOK( void ) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	CWnd* pWnd = GetFocus();
	if( pWnd )
	{
		CWnd* pWndNext = GetNextDlgTabItem( pWnd );
		if( pWndNext )
		{
			pWndNext->SetFocus();
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabPerformance::OnSetActive

BOOL CTabPerformance::OnSetActive( void ) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Refresh the page
	if(	m_fValidTrackItem )
	{
		SetPropTrackItem( &m_PropItem );
	}
	else
	{
		SetPropTrackItem( NULL );
	}

	if( m_pPropPageMgr 
	&&  m_pPropPageMgr->m_pTrackMgr 
	&&  m_pPropPageMgr->m_pIPropSheet )
	{
		m_pPropPageMgr->m_pIPropSheet->GetActivePage( &CPropPageMgrItem::sm_nActiveTab );
	}
	
	return CPropertyPage::OnSetActive();
}


/////////////////////////////////////////////////////////////////////////////
// CTabPerformance::OnHScroll

void CTabPerformance::OnHScroll( UINT nSBCode, UINT nPos, CScrollBar* pScrollBar ) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	CSliderCollection::OnHScroll(nSBCode, nPos, pScrollBar);
	CPropertyPage::OnHScroll(nSBCode, nPos, pScrollBar);
}


/////////////////////////////////////////////////////////////////////////////
// CTabPerformance::OnCheckNoInvalidate

void CTabPerformance::OnCheckNoInvalidate( void ) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Check if we have a valid DataObject pointer
	if( m_pPropPageMgr->m_pIPropPageObject == NULL )
	{
		return;
	}

	// Make sure controls have been created
	if (m_pmsAttenuation == NULL)
	{
		return;
	}

	DWORD dwFlags = m_PropItem.m_Item.m_dwFlagsDM;

	switch( IsDlgButtonChecked( IDC_CHECK_NOINVALIDATE ) )
	{
		case 0:		// Currently unchecked
		case 2:		// Currently undetermined
			CheckDlgButton( IDC_CHECK_NOINVALIDATE, 1 );
			dwFlags |= DMUS_WAVEF_NOINVALIDATE;
			break;

		case 1:		// Currently checked
			CheckDlgButton( IDC_CHECK_NOINVALIDATE, 0 );
			dwFlags &= ~DMUS_WAVEF_NOINVALIDATE;
			break;
	}

	if( (m_PropItem.m_dwUndetermined_TabPerformance & UNDT_WAVEF_NOINVALIDATE)
	||  (m_PropItem.m_Item.m_dwFlagsDM != dwFlags) )
	{
		m_PropItem.m_Item.m_dwFlagsDM = dwFlags;
		m_PropItem.m_dwChanged = CHGD_DM_FLAGS;
		UpdateObject();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabPerformance::OnDoubleClickedCheckNoInvalidate

void CTabPerformance::OnDoubleClickedCheckNoInvalidate( void ) 
{
	OnCheckNoInvalidate();
}

 

void CTabPerformance::OnCheckIgnoreLooping() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Check if we have a valid DataObject pointer
	if( m_pPropPageMgr->m_pIPropPageObject == NULL )
	{
		return;
	}

	// Make sure controls have been created
	if (m_pmsAttenuation == NULL)
	{
		return;
	}

	DWORD dwFlags = m_PropItem.m_Item.m_dwFlagsDM;

	switch( IsDlgButtonChecked( IDC_CHECK_IGNORE_LOOPING ) )
	{
		case 0:		// Currently unchecked
		case 2:		// Currently undetermined
			CheckDlgButton( IDC_CHECK_IGNORE_LOOPING, 1 );
			dwFlags |= DMUS_WAVEF_IGNORELOOPS;
			break;

		case 1:		// Currently checked
			CheckDlgButton( IDC_CHECK_IGNORE_LOOPING, 0 );
			dwFlags &= ~DMUS_WAVEF_IGNORELOOPS;
			break;
	}

	if( (m_PropItem.m_dwUndetermined_TabPerformance & UNDT_WAVEF_IGNORELOOPS)
	||  (m_PropItem.m_Item.m_dwFlagsDM != dwFlags) )
	{
		m_PropItem.m_Item.m_dwFlagsDM = dwFlags;
		m_PropItem.m_dwChanged = CHGD_DM_FLAGS;
		UpdateObject();
	}	
}

void CTabPerformance::OnDoubleclickedCheckIgnoreLooping() 
{
	OnCheckIgnoreLooping();	
}


/////////////////////////////////////////////////////////////////////////////
// CTabPerformance::SetLogicalTime

void CTabPerformance::SetLogicalTime( void ) 
{
	MUSIC_TIME mtTimeLogical;
	long lGrid, lTick;

	m_pPropPageMgr->m_pTrackMgr->ForceBoundaries( m_lLogicalMeasure, m_lLogicalBeat, &mtTimeLogical );
	m_pPropPageMgr->m_pTrackMgr->ClocksToMeasureBeatGridTick( mtTimeLogical, &m_lLogicalMeasure, &m_lLogicalBeat, &lGrid, &lTick );
	m_pPropPageMgr->m_pTrackMgr->MeasureBeatGridTickToClocks( m_lLogicalMeasure, m_lLogicalBeat, 0, 0, &mtTimeLogical );

	if( (m_PropItem.m_dwUndetermined_TabPerformance & UNDT_TIME_LOGICAL)
	||  (m_PropItem.m_Item.m_rtTimeLogical != mtTimeLogical) )
	{
		m_PropItem.m_Item.m_rtTimeLogical = mtTimeLogical;

		// Now, update the object with the new value(s)
		m_PropItem.m_dwChanged = CHGD_TIME_LOGICAL;
		UpdateObject();
	}
	else
	{
		// Display original values
		SetPropTrackItem( &m_PropItem );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabPerformance::OnSpinLogicalMeasure

void CTabPerformance::OnSpinLogicalMeasure( NMHDR* pNMHDR, LRESULT* pResult ) 
{
	// Need to do this in case the user clicked the spin control immediately after
	// typing in a value
	OnKillFocusLogicalMeasure();

	if( HandleDeltaChange( pNMHDR, pResult, m_lLogicalMeasure ) )
	{
		SetLogicalTime();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabPerformance::OnSpinLogicalBeat

void CTabPerformance::OnSpinLogicalBeat( NMHDR* pNMHDR, LRESULT* pResult ) 
{
	// Need to do this in case the user clicked the spin control immediately after
	// typing in a value
	OnKillFocusLogicalBeat();

	if( HandleDeltaChange( pNMHDR, pResult, m_lLogicalBeat ) )
	{
		SetLogicalTime();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabPerformance::HandleDeltaChange
//
// Generic handler for deltapos changes
bool CTabPerformance::HandleDeltaChange( NMHDR* pNMHDR, LRESULT* pResult, long& lUpdateVal )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_pPropPageMgr->m_pIPropPageObject == NULL )
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
// CTabPerformance::OnKillFocusLogicalMeasure

void CTabPerformance::OnKillFocusLogicalMeasure() 
{
	if( HandleKillFocus( m_spinLogicalMeasure, m_lLogicalMeasure, 1 ) )
	{
		SetLogicalTime();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabPerformance::OnKillFocusLogicalBeat

void CTabPerformance::OnKillFocusLogicalBeat() 
{
	if( HandleKillFocus( m_spinLogicalBeat, m_lLogicalBeat, 1 ) )
	{
		SetLogicalTime();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabPerformance::HandleKillFocus
//
// Generic handler for KillFocus changes
bool CTabPerformance::HandleKillFocus( CSpinButtonCtrl& spin, long& lUpdateVal, int nOffset )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Check if we have a valid DataObject pointer
	if( m_pPropPageMgr->m_pIPropPageObject == NULL )
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

BOOL CTabPerformance::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	LRESULT lResult;
	if (CSliderCollection::OnCommand(wParam, lParam, &lResult))
		return lResult;

	return CPropertyPage::OnCommand(wParam, lParam);
}

BOOL CTabPerformance::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	LRESULT lResult;
	if (CSliderCollection::OnNotify(wParam, lParam, &lResult))
		return lResult;
	
	return CPropertyPage::OnNotify(wParam, lParam, pResult);
}

void CTabPerformance::UpdateAttenuation()
{
	if( (m_PropItem.m_dwUndetermined_TabPerformance & UNDT_VOLUME)
	||  (m_PropItem.m_Item.m_lVolume != (m_lAttenuation >> 16) * 10) )
	{
		m_PropItem.m_Item.m_lVolume = (m_lAttenuation >> 16) * 10;
		m_PropItem.m_dwChanged = CHGD_VOLUME;
		UpdateObject();
	}
}

void CTabPerformance::UpdateFineTune()
{
	if( (m_PropItem.m_dwUndetermined_TabPerformance & UNDT_PITCH)
	||  (m_PropItem.m_Item.m_lPitch != (m_lFineTune >> 16)) )
	{
		m_PropItem.m_Item.m_lPitch = (m_lFineTune >> 16);
		m_PropItem.m_dwChanged = CHGD_PITCH;
		UpdateObject();
	}
}

void CTabPerformance::UpdateAttenuationRange()
{
	if( (m_PropItem.m_dwUndetermined_TabPerformance & UNDT_VOLUME_RANGE)
	||  (m_PropItem.m_Item.m_lVolumeRange != (m_lAttenuationRange * 10) >> 16) )
	{
		m_PropItem.m_Item.m_lVolumeRange = (m_lAttenuationRange * 10) >> 16;
		m_PropItem.m_dwChanged = CHGD_VOLUME_RANGE;
		UpdateObject();
	}
}

void CTabPerformance::UpdateFineTuneRange()
{
	if( (m_PropItem.m_dwUndetermined_TabPerformance & UNDT_PITCH_RANGE)
	||  (m_PropItem.m_Item.m_lPitchRange != (m_lFineTuneRange >> 16) * 10) )
	{
		m_PropItem.m_Item.m_lPitchRange = (m_lFineTuneRange >> 16) * 10;
		m_PropItem.m_dwChanged = CHGD_PITCH_RANGE;
		UpdateObject();
	}
}

bool CTabPerformance::OnSliderUpdate(MySlider *pms, DWORD dwmscupdf)
{
	switch (dwmscupdf)
		{
		case dwmscupdfStart:
			return true; // undo done at update end

		case dwmscupdfEnd:
			if (pms == m_pmsAttenuation)
				UpdateAttenuation();
			else if (pms == m_pmsAttenuationRange)
				UpdateAttenuationRange();
			else if (pms == m_pmsFineTuneRange)
				UpdateFineTuneRange();
			else
				{
				ASSERT(pms == m_pmsFineTune); // slider not implemented
				UpdateFineTune();
				}
			return true;

		default:
			ASSERT(FALSE);
			return false;
		}
}

