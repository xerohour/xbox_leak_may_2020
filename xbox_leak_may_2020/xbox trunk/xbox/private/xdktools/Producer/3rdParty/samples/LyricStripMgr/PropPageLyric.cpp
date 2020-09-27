/************************************************************************
*                                                                       *
*   Copyright (c) 1998-1999 Microsoft Corp. All rights reserved.        *
*                                                                       *
************************************************************************/

// PropPageLyric.cpp : implementation file
//

#include "stdafx.h"
#include <RiffStrm.h>
#include "LyricMgr.h"
#include "PropPageMgr.h"
#include "PropPageLyric.h"
#include <dmusici.h>
#include <dmusicf.h>
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

// {694073F0-9948-11d3-B474-00105A2796DE}
static const GUID GUID_LyricPPGMgr = 
{ 0x694073f0, 0x9948, 0x11d3, { 0xb4, 0x74, 0x0, 0x10, 0x5a, 0x27, 0x96, 0xde } };


/////////////////////////////////////////////////////////////////////////////
// CLyricPropPageMgr constructor/destructor

CLyricPropPageMgr::CLyricPropPageMgr( CLyricMgr* pLyricMgr )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	ASSERT( pLyricMgr != NULL );
	m_pLyricMgr = pLyricMgr;

	// Initialize our pointer to NULL
	m_pPropPageLyric = NULL;

	// Store GUID
	m_GUIDManager = GUID_LyricPPGMgr;

	// call the base class contstructor
	CStaticPropPageManager::CStaticPropPageManager();
}

CLyricPropPageMgr::~CLyricPropPageMgr()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// If the Lyric property page exists, delete it
	if( m_pPropPageLyric )
	{
		delete m_pPropPageLyric;
		m_pPropPageLyric = NULL;
	}

	// Call the base class destructor
	CStaticPropPageManager::~CStaticPropPageManager();
}


/////////////////////////////////////////////////////////////////////////////
// CLyricPropPageMgr IDMUSProdPropPageManager implementation

/////////////////////////////////////////////////////////////////////////////
// CLyricPropPageMgr::GetPropertySheetTitle

HRESULT STDMETHODCALLTYPE CLyricPropPageMgr::GetPropertySheetTitle( BSTR* pbstrTitle, 
	BOOL* pfAddPropertiesText )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Validate parameters
	if( (pbstrTitle == NULL)
	||  (pfAddPropertiesText == NULL) )
	{
		return E_POINTER;
	}

	*pfAddPropertiesText = TRUE;

	// Try and load the title
	CComBSTR comBSTR;
	if( comBSTR.LoadString( IDS_PROPPAGE_LYRIC ) )
	{
		// Succeeded - return the BSTR
		*pbstrTitle = comBSTR.Detach();
		return S_OK;
	}

	// Failed - return NULL
	*pbstrTitle = NULL;

	return E_FAIL;
};


/////////////////////////////////////////////////////////////////////////////
// CLyricPropPageMgr::GetPropertySheetPages

HRESULT STDMETHODCALLTYPE CLyricPropPageMgr::GetPropertySheetPages( IDMUSProdPropSheet* pIPropSheet, 
	LONG* hPropSheetPage[], short* pnNbrPages )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Validate parameters
	if( (hPropSheetPage == NULL)
	||  (pnNbrPages == NULL) )
	{
		return E_POINTER;
	}

	if( pIPropSheet == NULL )
	{
		return E_POINTER;
	}

	// Save a copy of the IDMUSProdPropSheet pointer
	m_pIPropSheet = pIPropSheet;
	m_pIPropSheet->AddRef();

	// Initialize the array and number of property sheets
	hPropSheetPage[0] = NULL;
	*pnNbrPages = 0;

	// Initialize the number of pages we've added
	short nNbrPages = 0;

	// If it doesn't exist, create the lyric property page
	if( m_pPropPageLyric == NULL )
	{
		m_pPropPageLyric = new PropPageLyric();
	}

	// If it exists, add the lyric property page
	if( m_pPropPageLyric )
	{
		// Copy the PROPSHEETPAGE structure
		PROPSHEETPAGE psp;
		memcpy( &psp, &m_pPropPageLyric->m_psp, sizeof(PROPSHEETPAGE) );

		// Crate a property sheet page from the PROPSHEETPAGE structure
		HPROPSHEETPAGE hPage;
		hPage = ::CreatePropertySheetPage( (LPCPROPSHEETPAGE)&psp );
		if( hPage )
		{
			// If the page creation succeeded, add it to the array
			hPropSheetPage[nNbrPages] = (LONG *)hPage;

			// And increment the number of pages in the array
			nNbrPages++;
		}

		// Point the lyric property page back to this property page manager
		m_pPropPageLyric->m_pPropPageMgr = this;
	}

	// Set number of pages
	*pnNbrPages = nNbrPages;
	return S_OK;
};


/////////////////////////////////////////////////////////////////////////////
// CLyricPropPageMgr::RefreshData

HRESULT STDMETHODCALLTYPE CLyricPropPageMgr::RefreshData()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Get a pointer to a CLyricItem
	CLyricItem* pLyric;

	if( m_pIPropPageObject == NULL )
	{
		// No property page object - set pointer to NULL
		pLyric = NULL;
	}
	// Have a property page object - try and get a pointer from it
	else if( FAILED ( m_pIPropPageObject->GetData( (void **)&pLyric ) ) )
	{
		return E_FAIL;
	}

	// Update the lyric property page, if it exists
	if (m_pPropPageLyric )
	{
		// pLyric may be NULL, meaning nothing is selected
		m_pPropPageLyric->SetLyric( pLyric );
		return S_OK;
	}
	else
	{
		return E_FAIL;
	}
};


/////////////////////////////////////////////////////////////////////////////
// PropPageLyric property page

IMPLEMENT_DYNCREATE(PropPageLyric, CPropertyPage)

/////////////////////////////////////////////////////////////////////////////
// PropPageLyric constructor/destructor

PropPageLyric::PropPageLyric(): CPropertyPage(PropPageLyric::IDD)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

//	CPropertyPage(IDD_LYRIC_PROPPAGE);
	//{{AFX_DATA_INIT(PropPageLyric)
	//}}AFX_DATA_INIT

	// Initialize our pointers to NULL
	m_pPropPageMgr = NULL;
	m_pTimeline = NULL;

	// Initialize our class members to FALSE
	m_fValidLyric = FALSE;
	m_fNeedToDetach = FALSE;
}

PropPageLyric::~PropPageLyric()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
}


/////////////////////////////////////////////////////////////////////////////
// PropPageLyric::DoDataExchange

void PropPageLyric::DoDataExchange(CDataExchange* pDX)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	CPropertyPage::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(PropPageLyric)
	DDX_Control(pDX, IDC_SPIN_BELONGS_MEASURE, m_spinLogicalMeasure);
	DDX_Control(pDX, IDC_SPIN_BELONGS_BEAT, m_spinLogicalBeat);
	DDX_Control(pDX, IDC_EDIT_BELONGS_MEASURE, m_editLogicalMeasure);
	DDX_Control(pDX, IDC_EDIT_BELONGS_BEAT, m_editLogicalBeat);
	DDX_Control(pDX, IDC_TIMING_QUICK, m_radioTimingQuick);
	DDX_Control(pDX, IDC_TIMING_BEFORE_TIME, m_radioTimingBeforeTime);
	DDX_Control(pDX, IDC_TIMING_AT_TIME, m_radioTimingAtTime);
	DDX_Control(pDX, IDC_SPIN_TICK, m_spinTick);
	DDX_Control(pDX, IDC_EDIT_TICK, m_editTick);
	DDX_Control(pDX, IDC_EDIT_BEAT, m_editBeat);
	DDX_Control(pDX, IDC_EDIT_LYRIC, m_editLyric);
	DDX_Control(pDX, IDC_EDIT_MEASURE, m_editMeasure);
	DDX_Control(pDX, IDC_SPIN_MEASURE, m_spinMeasure);
	DDX_Control(pDX, IDC_SPIN_BEAT, m_spinBeat);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(PropPageLyric, CPropertyPage)
	//{{AFX_MSG_MAP(PropPageLyric)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_BEAT, OnDeltaposSpinBeat)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_MEASURE, OnDeltaposSpinMeasure)
	ON_EN_KILLFOCUS(IDC_EDIT_BEAT, OnKillfocusEditBeat)
	ON_EN_KILLFOCUS(IDC_EDIT_LYRIC, OnKillfocusEditLyric)
	ON_EN_KILLFOCUS(IDC_EDIT_MEASURE, OnKillfocusEditMeasure)
	ON_EN_KILLFOCUS(IDC_EDIT_TICK, OnKillfocusEditTick)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_TICK, OnDeltaposSpinTick)
	ON_BN_CLICKED(IDC_TIMING_QUICK, OnTimingQuick)
	ON_BN_DOUBLECLICKED(IDC_TIMING_QUICK, OnDoubleClickedTimingQuick)
	ON_BN_CLICKED(IDC_TIMING_BEFORE_TIME, OnTimingBeforeTime)
	ON_BN_DOUBLECLICKED(IDC_TIMING_BEFORE_TIME, OnDoubleClickedTimingBeforeTime)
	ON_BN_CLICKED(IDC_TIMING_AT_TIME, OnTimingAtTime)
	ON_BN_DOUBLECLICKED(IDC_TIMING_AT_TIME, OnDoubleClickedTimingAtTime)
	ON_WM_KILLFOCUS()
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_BELONGS_BEAT, OnDeltaPosSpinLogicalBeat)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_BELONGS_MEASURE, OnDeltaPosSpinLogicalMeasure)
	ON_EN_KILLFOCUS(IDC_EDIT_BELONGS_BEAT, OnKillFocusEditLogicalBeat)
	ON_EN_KILLFOCUS(IDC_EDIT_BELONGS_MEASURE, OnKillFocusEditLogicalMeasure)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// PropPageLyric custom functions

/////////////////////////////////////////////////////////////////////////////
// PropPageLyric::SetLyric

void PropPageLyric::SetLyric( const CLyricItem* pLyric )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// If the lyric pointer is null, there are no Lyrics selected, so disable the property page
	if ( pLyric == NULL )
	{
		m_fValidLyric = FALSE;
		m_Lyric.Clear();
		EnableControls( FALSE );
		return;
	}

	// Update our timeline pointer

	// Initialize the timeline pointer to NULL
	m_pTimeline = NULL;

	// Initialize our groupbits to all 32 groups
	DWORD dwGroupBits = 0xFFFFFFFF;

	// Verify we have a valid pointer to our Property Page Object
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

	// Flag that we have a valid lyric
	m_fValidLyric = TRUE;

		// Copy the information from the new lyric
	m_Lyric.Copy( pLyric );

	long lMaxMeasure, lMaxBeat; // Maximum measure and beat values
	// Get the maximum measure value
	if( m_pTimeline == NULL )
	{
		// No timeline pointer - use default values
		lMaxMeasure = 32767;
		lMaxBeat = 256;
	}
	else
	{
		// Get the length of the timeline, in clocks
		VARIANT var;
		m_pTimeline->GetTimelineProperty( TP_CLOCKLENGTH, &var );

		// Convert from clocks to a measure and beat value
		m_pTimeline->ClocksToMeasureBeat( dwGroupBits, 0,
										  V_I4( &var ), &lMaxMeasure, &lMaxBeat );

		// If the beat value is zero, check if the segment is exactly lMaxMeasure in length
		if( lMaxBeat == 0 )
		{
			// Convert from lMaxMeasure to a clock value
			long lClockLength;
			m_pTimeline->MeasureBeatToClocks( dwGroupBits, 0, lMaxMeasure, 0, &lClockLength );

			// Check if this clock value is equal to the length
			if( lClockLength == V_I4( &var ) )
			{
				// Exactly lMaxMeasures long.  Get the measure and beat value for the previous tick
				m_pTimeline->ClocksToMeasureBeat( dwGroupBits, 0,
												  V_I4( &var ) - 1, &lMaxMeasure, &lMaxBeat );
			}
		}

		// If the lyric is not in the last measure
		if( m_Lyric.m_lMeasure < lMaxMeasure )
		{
			// Convert from a Measure value to a clock value
			long lClockForMeasure;
			m_pTimeline->MeasureBeatToClocks( dwGroupBits, 0, m_Lyric.m_lMeasure, 0, &lClockForMeasure );

			// Get the TimeSig for this measure
			DMUS_TIMESIGNATURE TimeSig;
			if( SUCCEEDED( m_pTimeline->GetParam( GUID_TimeSignature, dwGroupBits, 0, lClockForMeasure, NULL, &TimeSig ) ) )
			{
				lMaxBeat = TimeSig.bBeatsPerMeasure - 1;
			}
		}
		// Else the lyric is in the last measure and lMaxBeat is already set

		// Now convert the measure and beat from 0-based to 1-based
		lMaxMeasure++;
		lMaxBeat++;
	}

	// Check if the edit control has a valid window handle.
	if( m_editBeat.GetSafeHwnd() == NULL )
	{
		// It doesn't have a valid window handle - the property page may have been destroyed (or not yet created).
		// Just exit early
		return;
	}

	// Don't send OnKill/OnUpdate notifications when updating the lyric text
	CLockoutNotification LockoutNotifications( m_hWnd );

	// Check if multiple items are selected
	if( m_Lyric.m_dwBitsUI & UD_MULTIPLESELECT )
	{
		// Yes - disable the property page and return
		EnableControls( FALSE );
		return;
	}

	// Update the property page based on the new data.
	// It is usually profitable to check to see if the data actually changed
	// before updating the controls in the property page.

	// Enable all the edit controls
	EnableControls( TRUE );

	// Get the currently displayed lyric text
	CString strTemp;
	m_editLyric.GetWindowText( strTemp );

	// If it's different from what we were passed or the lyric was previously invalid, update the display
	if ( !m_fValidLyric || (pLyric->m_strText != strTemp) )
	{
		m_editLyric.SetWindowText( pLyric->m_strText );
	}

	// If the lyric was previously invalid, update the measure and beat display
	if( !m_fValidLyric )
	{
		SetDlgItemInt( m_editMeasure.GetDlgCtrlID(), m_Lyric.m_lMeasure + 1 );
		SetDlgItemInt( m_editBeat.GetDlgCtrlID(), m_Lyric.m_lBeat + 1 );
		SetDlgItemInt( m_editTick.GetDlgCtrlID(), m_Lyric.m_lTick );
		m_radioTimingQuick.SetCheck( 0 );
		m_radioTimingBeforeTime.SetCheck( 0 );
		m_radioTimingAtTime.SetCheck( 0 );

		if( m_pPropPageMgr
		&&  m_pPropPageMgr->m_pLyricMgr
		&&  m_pPropPageMgr->m_pLyricMgr->IsRefTimeTrack() == false )
		{
			SetDlgItemInt( m_editLogicalMeasure.GetDlgCtrlID(), m_Lyric.m_lLogicalMeasure + 1 );
			SetDlgItemInt( m_editLogicalBeat.GetDlgCtrlID(), m_Lyric.m_lLogicalBeat + 1 );
		}
		else
		{
			m_editLogicalMeasure.SetWindowText( NULL );
			m_editLogicalBeat.SetWindowText( NULL );
		}
	}
	// Otherwise, check if the values displayed don't match the new ones
	else
	{
		TCHAR tcstrTmp[DIALOG_EDIT_LEN];
		long lCurValue;
		BOOL fTransSuccess;

		// MEASURE
		{
			m_editMeasure.GetWindowText( tcstrTmp, DIALOG_EDIT_LEN );

			// Convert from text to an integer
			lCurValue = GetDlgItemInt( m_editMeasure.GetDlgCtrlID(), &fTransSuccess, TRUE );

			// Check if the conversion failed, the text is empty, or if the values are different
			if( !fTransSuccess
			||  (tcstrTmp[0] == NULL)
			||  (lCurValue != m_Lyric.m_lMeasure + 1) )
			{
				// Update the displayed measure number
				SetDlgItemInt( m_editMeasure.GetDlgCtrlID(), m_Lyric.m_lMeasure + 1 );
			}
		}

		// BEAT
		{
			m_editBeat.GetWindowText( tcstrTmp, DIALOG_EDIT_LEN );

			// Convert from text to an integer
			lCurValue = GetDlgItemInt( m_editBeat.GetDlgCtrlID(), &fTransSuccess, TRUE );

			// Check if the conversion failed, the text is empty, or if the values are different
			if( !fTransSuccess
			||  (tcstrTmp[0] == NULL)
			||  (lCurValue != m_Lyric.m_lBeat + 1) )
			{
				// Update the displayed beat number
				SetDlgItemInt( m_editBeat.GetDlgCtrlID(), m_Lyric.m_lBeat + 1 );
			}
		}

		// TICK
		{
			m_editTick.GetWindowText( tcstrTmp, DIALOG_EDIT_LEN );

			// Convert from text to an integer
			lCurValue = GetDlgItemInt( m_editTick.GetDlgCtrlID(), &fTransSuccess, TRUE );

			// Check if the conversion failed, the text is empty, or if the values are different
			if( !fTransSuccess
			|| (tcstrTmp[0] == NULL)
			|| (lCurValue != m_Lyric.m_lTick) )
			{
				// Update the displayed tick number
				SetDlgItemInt( m_editTick.GetDlgCtrlID(), m_Lyric.m_lTick );
			}
		}

		if( m_pPropPageMgr
		&&  m_pPropPageMgr->m_pLyricMgr
		&&  m_pPropPageMgr->m_pLyricMgr->IsRefTimeTrack() == false )
		{
			// LOGICAL MEASURE
			{
				m_editLogicalMeasure.GetWindowText( tcstrTmp, DIALOG_EDIT_LEN );

				// Convert from text to an integer
				long lCurValue = GetDlgItemInt( m_editLogicalMeasure.GetDlgCtrlID(), &fTransSuccess, TRUE );

				// Check if the conversion failed, the text is empty, or if the values are different
				if( !fTransSuccess
				||  (tcstrTmp[0] == NULL)
				||  (lCurValue != m_Lyric.m_lLogicalMeasure + 1) )
				{
					// Update the displayed logical measure number
					SetDlgItemInt( m_editLogicalMeasure.GetDlgCtrlID(), m_Lyric.m_lLogicalMeasure + 1 );
				}
			}

			// LOGICAL BEAT
			{
				m_editLogicalBeat.GetWindowText( tcstrTmp, DIALOG_EDIT_LEN );

				// Convert from text to an integer
				lCurValue = GetDlgItemInt( m_editLogicalBeat.GetDlgCtrlID(), &fTransSuccess, TRUE );

				// Check if the conversion failed, the text is empty, or if the values are different
				if( !fTransSuccess
				||  (tcstrTmp[0] == NULL)
				||  (lCurValue != m_Lyric.m_lLogicalBeat + 1) )
				{
					// Update the displayed logical beat number
					SetDlgItemInt( m_editLogicalBeat.GetDlgCtrlID(), m_Lyric.m_lLogicalBeat + 1 );
				}
			}
		}

		// Set 'Timing' radio buttons
		if( m_Lyric.m_dwTimingFlagsDM & DMUS_PMSGF_TOOL_IMMEDIATE )
		{
			if( m_radioTimingQuick.GetCheck() == 0 )
			{
				m_radioTimingQuick.SetCheck( 1 );
				m_radioTimingBeforeTime.SetCheck( 0 );
				m_radioTimingAtTime.SetCheck( 0 );
			}
		}
		if( m_Lyric.m_dwTimingFlagsDM & DMUS_PMSGF_TOOL_QUEUE )
		{
			if( m_radioTimingBeforeTime.GetCheck() == 0 )
			{
				m_radioTimingQuick.SetCheck( 0 );
				m_radioTimingBeforeTime.SetCheck( 1 );
				m_radioTimingAtTime.SetCheck( 0 );
			}
		}
		if( m_Lyric.m_dwTimingFlagsDM & DMUS_PMSGF_TOOL_ATTIME )
		{
			if( m_radioTimingAtTime.GetCheck() == 0 )
			{
				m_radioTimingQuick.SetCheck( 0 );
				m_radioTimingBeforeTime.SetCheck( 0 );
				m_radioTimingAtTime.SetCheck( 1 );
			}
		}
	}

	// Update the control ranges
	m_spinMeasure.SetRange( 1, lMaxMeasure );
	m_spinBeat.SetRange( 1, lMaxBeat );
	m_spinTick.SetRange( -MAX_TICK, MAX_TICK );
	m_spinLogicalMeasure.SetRange( 1, lMaxMeasure );
	m_spinLogicalBeat.SetRange( 1, lMaxBeat );
}


/////////////////////////////////////////////////////////////////////////////
// PropPageLyric message handlers

/////////////////////////////////////////////////////////////////////////////
// PropPageLyric::OnCreate

int PropPageLyric::OnCreate(LPCREATESTRUCT lpCreateStruct) 
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
// PropPageLyric::OnDestroy

void PropPageLyric::OnDestroy() 
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

	// Call the CPropertyPage's OnDestroy method to actually destroy all our child controls.
	CPropertyPage::OnDestroy();	
}


/////////////////////////////////////////////////////////////////////////////
// PropPageLyric::OnInitDialog

BOOL PropPageLyric::OnInitDialog() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Call the base class's OnInitDialog method
	CPropertyPage::OnInitDialog();

	// Limit the measure number to positive numbers
	m_spinMeasure.SetRange( 1, 32767 );
	m_editMeasure.LimitText( 5 );

	// Limit the beat number to 1-256
	m_spinBeat.SetRange( 1, 256 );
	m_editBeat.LimitText( 3 );

	// Limit the tick number to MAX_TICK
	m_spinTick.SetRange( -MAX_TICK, MAX_TICK );
	m_editTick.LimitText( 5 );

	// Limit the logical measure number to positive numbers
	m_spinLogicalMeasure.SetRange( 1, 32767 );
	m_editLogicalMeasure.LimitText( 5 );

	// Limit the logical beat number to 1-256
	m_spinLogicalBeat.SetRange( 1, 256 );
	m_editLogicalBeat.LimitText( 3 );

	// If we have valid lyric information
	if(	m_fValidLyric )
	{
		// Force update of the controls
		m_fValidLyric = FALSE;

		// Update the dialog
		SetLyric( &m_Lyric );
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	               // EXCEPTION: OCX Property Pages should return FALSE
}


/////////////////////////////////////////////////////////////////////////////
// PropPageLyric::OnDeltaposSpinTick

void PropPageLyric::OnDeltaposSpinTick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// Need to do this in case the user clicked the spin control immediately after
	// typing in a value
	OnKillfocusEditTick();

	HandleDeltaChange( pNMHDR, pResult, m_Lyric.m_lTick );
}


/////////////////////////////////////////////////////////////////////////////
// PropPageLyric::OnDeltaposSpinBeat

void PropPageLyric::OnDeltaposSpinBeat(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// Need to do this in case the user clicked the spin control immediately after
	// typing in a value
	OnKillfocusEditBeat();

	HandleDeltaChange( pNMHDR, pResult, m_Lyric.m_lBeat );
}


/////////////////////////////////////////////////////////////////////////////
// PropPageLyric::OnDeltaposSpinMeasure

void PropPageLyric::OnDeltaposSpinMeasure(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// Need to do this in case the user clicked the spin control immediately after
	// typing in a value
	OnKillfocusEditMeasure();

	HandleDeltaChange( pNMHDR, pResult, m_Lyric.m_lMeasure );
}


/////////////////////////////////////////////////////////////////////////////
// PropPageLyric::OnDeltaPosSpinLogicalBeat

void PropPageLyric::OnDeltaPosSpinLogicalBeat(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// Need to do this in case the user clicked the spin control immediately after
	// typing in a value
	OnKillFocusEditLogicalBeat();

	HandleDeltaChange( pNMHDR, pResult, m_Lyric.m_lLogicalBeat );
}


/////////////////////////////////////////////////////////////////////////////
// PropPageLyric::OnDeltaPosSpinLogicalMeasure

void PropPageLyric::OnDeltaPosSpinLogicalMeasure(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// Need to do this in case the user clicked the spin control immediately after
	// typing in a value
	OnKillFocusEditLogicalMeasure();

	HandleDeltaChange( pNMHDR, pResult, m_Lyric.m_lLogicalMeasure );
}


/////////////////////////////////////////////////////////////////////////////
// PropPageLyric::HandleDeltaChange
//
// Generic handler for deltapos changes
void PropPageLyric::HandleDeltaChange( NMHDR* pNMHDR,
									   LRESULT* pResult,
									   long& lUpdateVal )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_pPropPageMgr->m_pIPropPageObject == NULL )
	{
		return;
	}

	// If the value changed - update the selected Lyric
	if( ((NM_UPDOWN* )pNMHDR)->iDelta != 0 ) 
	{
		// Update the value
		lUpdateVal += ((NM_UPDOWN* )pNMHDR)->iDelta;

		// Now, update the object with the new value
		UpdateObject();
	}

	// Set the result to 1 to show that we handled this message
	*pResult = 1;
}


/////////////////////////////////////////////////////////////////////////////
// PropPageLyric::OnKillfocusEditTick

void PropPageLyric::OnKillfocusEditTick() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Check if we have a valid DataObject pointer
	if( m_pPropPageMgr->m_pIPropPageObject == NULL )
	{
		return;
	}

	// Check if the spin control is a valid window
	if (!::IsWindow(m_spinTick.m_hWnd))
	{
		return;
	}

	// Get text from edit control
	TCHAR tcstrTmp[DIALOG_EDIT_LEN];
	m_editTick.GetWindowText( tcstrTmp, DIALOG_EDIT_LEN );

	// Handle empty text
	if( tcstrTmp[0] == NULL )
	{
		// Set it back to the minimum value
		m_spinTick.SetPos( 0 );

		// Check if the value changed
		if( m_Lyric.m_lTick != 0 )
		{
			// Update the item with the new value
			m_Lyric.m_lTick = 0;

			// Now, update the object with the new value
			UpdateObject();
		}
	}
	else
	{
		// Convert from text to an integer
		BOOL fTransSuccess;
		long lNewValue = GetDlgItemInt( IDC_EDIT_TICK, &fTransSuccess, TRUE );

		// If unable to translate (if dialog contains invalid data, such as '-'), exit early
		if( !fTransSuccess )
		{
			// If conversion failed, update dialog item text
			SetDlgItemInt( IDC_EDIT_TICK, m_Lyric.m_lTick );
		}
		// If conversion succeeded, ensure the value stays in range
		else
		{
			// Get the valid range
			int iLower, iUpper;
			m_spinTick.GetRange( iLower, iUpper );

			// Check if the value is out of range
			if( lNewValue > iUpper )
			{
				// Value out of range, update dialog item text
				SetDlgItemInt( IDC_EDIT_TICK, iUpper );

				// Update the value to the top of the range
				lNewValue = iUpper;
			}

			// Check if the value changed
			if( m_Lyric.m_lTick != lNewValue )
			{
				// Update the item with the new value
				m_Lyric.m_lTick = lNewValue;

				// Now, update the object with the new value
				UpdateObject();
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// PropPageLyric::OnKillfocusEditBeat

void PropPageLyric::OnKillfocusEditBeat() 
{
	HandleKillFocus( m_spinBeat, m_Lyric.m_lBeat );
}


/////////////////////////////////////////////////////////////////////////////
// PropPageLyric::OnKillfocusEditMeasure

void PropPageLyric::OnKillfocusEditMeasure() 
{
	HandleKillFocus( m_spinMeasure, m_Lyric.m_lMeasure );
}


/////////////////////////////////////////////////////////////////////////////
// PropPageLyric::OnKillFocusEditLogicalBeat

void PropPageLyric::OnKillFocusEditLogicalBeat() 
{
	HandleKillFocus( m_spinLogicalBeat, m_Lyric.m_lLogicalBeat );
}


/////////////////////////////////////////////////////////////////////////////
// PropPageLyric::OnKillFocusEditLogicalMeasure

void PropPageLyric::OnKillFocusEditLogicalMeasure() 
{
	HandleKillFocus( m_spinLogicalMeasure, m_Lyric.m_lLogicalMeasure );
}


/////////////////////////////////////////////////////////////////////////////
// PropPageLyric::HandleKillFocus
//
// Generic handler for KillFocus changes
void PropPageLyric::HandleKillFocus( CSpinButtonCtrl& spin, long& lUpdateVal )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Check if we have a valid DataObject pointer
	if( m_pPropPageMgr->m_pIPropPageObject == NULL )
	{
		return;
	}

	// Check if the spin control is a valid window
	if (!::IsWindow(spin.m_hWnd))
	{
		return;
	}

	// Get text from edit control
	TCHAR tcstrTmp[DIALOG_EDIT_LEN];
	CWnd* pEditCtrl = spin.GetBuddy();
	ASSERT(pEditCtrl != NULL);
	if( pEditCtrl == NULL )
	{
		return;
	}
	pEditCtrl->GetWindowText( tcstrTmp, DIALOG_EDIT_LEN );

	// Handle empty text
	if( tcstrTmp[0] == NULL )
	{
		// Set it back to the minimum value
		spin.SetPos( 1 );

		// Check if the value changed
		if( 0 != lUpdateVal )
		{
			// Update the lyric with the new value
			lUpdateVal = 0;

			// Now, update the object with the new value
			UpdateObject();
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
			SetDlgItemInt( pEditCtrl->GetDlgCtrlID(), lUpdateVal + 1 );
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
			if( lNewValue - 1 != lUpdateVal )
			{
				// Update the lyric with the new value
				lUpdateVal = lNewValue - 1;

				// Now, update the object with the new value
				UpdateObject();
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// PropPageLyric::OnKillfocusEditLyric

void PropPageLyric::OnKillfocusEditLyric() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Get the new lyric text
	CString strText;
	m_editLyric.GetWindowText( strText );

	// Check if the text is empty
	if( strText.IsEmpty() )
	{
		// Check if the existing text is empty
		if( m_Lyric.m_strText != _T("") )
		{
			// Nope - update the Lyric strip manager with the new text
			m_Lyric.m_strText = "";
			UpdateObject();
		}
	}
	else
	{
		// Check if the text is different from the existing text.
		if( strText != m_Lyric.m_strText )
		{
			// Yep - update the Lyric strip manager with the new text
			m_Lyric.m_strText = strText;
			UpdateObject();
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// PropPageLyric::UpdateObject

void PropPageLyric::UpdateObject( void )
{
	// Check for a valid Property Page Object pointer
	if (m_pPropPageMgr && m_pPropPageMgr->m_pIPropPageObject)
	{
		// Update the Property Page Object with the new Lyric
		m_pPropPageMgr->m_pIPropPageObject->SetData((void *) &m_Lyric);
	}
}


/////////////////////////////////////////////////////////////////////////////
// PropPageLyric::EnableControls

void PropPageLyric::EnableControls( BOOL fEnable )
{
	// Check if the Beat edit control exists
	if( m_editBeat.GetSafeHwnd() == NULL )
	{
		return;
	}

	// Update the beat, measure, and lyric controls with the new window state
	m_spinTick.EnableWindow( fEnable );
	m_spinBeat.EnableWindow( fEnable );
	m_spinMeasure.EnableWindow( fEnable );
	m_editTick.EnableWindow( fEnable );
	m_editBeat.EnableWindow( fEnable );
	m_editMeasure.EnableWindow( fEnable );
	m_editLyric.EnableWindow( fEnable );
	m_radioTimingQuick.EnableWindow( fEnable );
	m_radioTimingBeforeTime.EnableWindow( fEnable );
	m_radioTimingAtTime.EnableWindow( fEnable );
	
	BOOL fEnableBelongsTo = FALSE;
	if( m_pPropPageMgr
	&&  m_pPropPageMgr->m_pLyricMgr
	&&  m_pPropPageMgr->m_pLyricMgr->IsRefTimeTrack() == false )
	{
		fEnableBelongsTo = fEnable;
	}

	m_spinLogicalBeat.EnableWindow( fEnableBelongsTo );
	m_spinLogicalMeasure.EnableWindow( fEnableBelongsTo );
	m_editLogicalBeat.EnableWindow( fEnableBelongsTo );
	m_editLogicalMeasure.EnableWindow( fEnableBelongsTo );

	// Check if we're disabling
	if( !fEnable )
	{
		// Check if multiple items are selected
		if( m_Lyric.m_dwBitsUI & UD_MULTIPLESELECT )
		{
			// Load the 'multiple select' text
			CString strText;
			if( strText.LoadString( IDS_MULTIPLE_SELECT ) )
			{
				// Display the 'multiple select' text
				m_editLyric.SetWindowText( strText );
				return;
			}
		}

		// No items are selected, so clear the edit controls
		m_editLyric.SetWindowText( NULL );
		m_editTick.SetWindowText( NULL );
		m_editBeat.SetWindowText( NULL );
		m_editMeasure.SetWindowText( NULL );
		m_radioTimingQuick.SetCheck( 0 );
		m_radioTimingBeforeTime.SetCheck( 0 );
		m_radioTimingAtTime.SetCheck( 0 );
	}

	if( fEnableBelongsTo == FALSE )
	{
		m_editLogicalMeasure.SetWindowText( NULL );
		m_editLogicalBeat.SetWindowText( NULL );
	}
}


/////////////////////////////////////////////////////////////////////////////
// PropPageLyric::OnTimingQuick

void PropPageLyric::OnTimingQuick() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_pPropPageMgr->m_pIPropPageObject == NULL )
	{
		return;
	}

	if( m_radioTimingQuick.GetCheck() )
	{
		m_Lyric.m_dwTimingFlagsDM &= ~(DMUS_PMSGF_TOOL_IMMEDIATE | DMUS_PMSGF_TOOL_QUEUE | DMUS_PMSGF_TOOL_ATTIME);
		m_Lyric.m_dwTimingFlagsDM |= DMUS_PMSGF_TOOL_IMMEDIATE;

		// Now, update the object with the new value
		UpdateObject();
	}
}


/////////////////////////////////////////////////////////////////////////////
// PropPageLyric::OnDoubleClickedTimingQuick

void PropPageLyric::OnDoubleClickedTimingQuick() 
{
	OnTimingQuick();
}


/////////////////////////////////////////////////////////////////////////////
// PropPageLyric::OnTimingBeforeTime

void PropPageLyric::OnTimingBeforeTime() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_pPropPageMgr->m_pIPropPageObject == NULL )
	{
		return;
	}

	if( m_radioTimingBeforeTime.GetCheck() )
	{
		m_Lyric.m_dwTimingFlagsDM &= ~(DMUS_PMSGF_TOOL_IMMEDIATE | DMUS_PMSGF_TOOL_QUEUE | DMUS_PMSGF_TOOL_ATTIME);
		m_Lyric.m_dwTimingFlagsDM |= DMUS_PMSGF_TOOL_QUEUE;

		// Now, update the object with the new value
		UpdateObject();
	}
}


/////////////////////////////////////////////////////////////////////////////
// PropPageLyric::OnDoubleClickedTimingBeforeTime

void PropPageLyric::OnDoubleClickedTimingBeforeTime() 
{
	OnTimingBeforeTime();
}


/////////////////////////////////////////////////////////////////////////////
// PropPageLyric::OnTimingAtTime

void PropPageLyric::OnTimingAtTime() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_pPropPageMgr->m_pIPropPageObject == NULL )
	{
		return;
	}

	if( m_radioTimingAtTime.GetCheck() )
	{
		m_Lyric.m_dwTimingFlagsDM &= ~(DMUS_PMSGF_TOOL_IMMEDIATE | DMUS_PMSGF_TOOL_QUEUE | DMUS_PMSGF_TOOL_ATTIME);
		m_Lyric.m_dwTimingFlagsDM |= DMUS_PMSGF_TOOL_ATTIME;

		// Now, update the object with the new value
		UpdateObject();
	}
}


/////////////////////////////////////////////////////////////////////////////
// PropPageLyric::OnDoubleClickedTimingAtTime

void PropPageLyric::OnDoubleClickedTimingAtTime() 
{
	OnTimingAtTime();
}


/////////////////////////////////////////////////////////////////////////////
// PropPageLyric::OnOK

// Windows translated presses of the 'Enter' key into presses of the button 'OK'.
// Here we create a hidden 'OK' button and translate presses of it into tabs to the next
// control in the property page.
void PropPageLyric::OnOK() 
{
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
