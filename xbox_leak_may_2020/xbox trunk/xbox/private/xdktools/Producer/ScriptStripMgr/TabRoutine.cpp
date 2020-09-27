// TabRoutine.cpp : implementation file
//

#include "stdafx.h"
#include <RiffStrm.h>
#include "TrackMgr.h"
#include "TrackItem.h"
#include "PropPageMgr_Item.h"
#include "TabRoutine.h"
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

// {D66917E0-7758-11d3-B45F-00105A2796DE}
static const GUID GUID_ScriptItemPPGMgr = 
{ 0xd66917e0, 0x7758, 0x11d3, { 0xb4, 0x5f, 0x0, 0x10, 0x5a, 0x27, 0x96, 0xde } };

/////////////////////////////////////////////////////////////////////////////
// CPropPageMgrItem constructor/destructor

CPropPageMgrItem::CPropPageMgrItem( IDMUSProdFramework* pIFramework, CTrackMgr* pTrackMgr )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Store Framework pointer
	ASSERT( pIFramework != NULL ); 
	m_pIFramework = pIFramework;
	m_pIFramework->AddRef();

	ASSERT( pTrackMgr != NULL ); 
	m_pTrackMgr = pTrackMgr;

	// Store GUID
	m_GUIDManager = GUID_ScriptItemPPGMgr;

	// Initialize our pointer to NULL
	m_pTabRoutine = NULL;

	// call the base class contstructor
	CStaticPropPageManager::CStaticPropPageManager();
}

CPropPageMgrItem::~CPropPageMgrItem()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// If the Routine property page exists, delete it
	if( m_pTabRoutine )
	{
		delete m_pTabRoutine;
		m_pTabRoutine = NULL;
	}

	if( m_pIFramework )
	{
		m_pIFramework->Release();
		m_pIFramework = NULL;
	}

	// Call the base class destructor
	CStaticPropPageManager::~CStaticPropPageManager();
}


/////////////////////////////////////////////////////////////////////////////
// CPropPageMgrItem IDMUSProdPropPageManager implementation

/////////////////////////////////////////////////////////////////////////////
// CPropPageMgrItem::GetPropertySheetTitle

HRESULT STDMETHODCALLTYPE CPropPageMgrItem::GetPropertySheetTitle( BSTR* pbstrTitle, 
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
	if( comBSTR.LoadString( IDS_PROPPAGE_ROUTINE ) )
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
// CPropPageMgrItem::GetPropertySheetPages

HRESULT STDMETHODCALLTYPE CPropPageMgrItem::GetPropertySheetPages( IDMUSProdPropSheet* pIPropSheet, 
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

	// If it doesn't exist yet, create the property page
	if( m_pTabRoutine == NULL )
	{
		m_pTabRoutine = new CTabRoutine();
	}

	// If the property page now exists, add the property page
	if( m_pTabRoutine )
	{
		// Copy the PROPSHEETPAGE structure
		PROPSHEETPAGE psp;
		memcpy( &psp, &m_pTabRoutine->m_psp, sizeof(PROPSHEETPAGE) );

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

		// Point the property page back to this property page manager
		m_pTabRoutine->m_pPropPageMgr = this;
	}

	// Set number of pages
	*pnNbrPages = nNbrPages;
	return S_OK;
};


/////////////////////////////////////////////////////////////////////////////
// CPropPageMgrItem::RefreshData

HRESULT STDMETHODCALLTYPE CPropPageMgrItem::RefreshData()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Get a pointer to a CTrackItem
	CTrackItem* pItem;

	if( m_pIPropPageObject == NULL )
	{
		// No property page object - set pointer to NULL
		pItem = NULL;
	}
	// Have a property page object - try and get a pointer from it
	else if( FAILED ( m_pIPropPageObject->GetData( (void **)&pItem ) ) )
	{
		return E_FAIL;
	}

	// Update the property page, if it exists
	if( m_pTabRoutine )
	{
		// pItem may be NULL, meaning nothing is selected
		m_pTabRoutine->SetItem( pItem );
		return S_OK;
	}
	else
	{
		return E_FAIL;
	}
};


/////////////////////////////////////////////////////////////////////////////
// CTabRoutine property page

IMPLEMENT_DYNCREATE(CTabRoutine, CPropertyPage)

/////////////////////////////////////////////////////////////////////////////
// CTabRoutine constructor/destructor

CTabRoutine::CTabRoutine(): CPropertyPage(CTabRoutine::IDD)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

//	CPropertyPage(IDD_ROUTINE_PROPPAGE);
	//{{AFX_DATA_INIT(CTabRoutine)
	//}}AFX_DATA_INIT

	// Initialize our pointers to NULL
	m_pPropPageMgr = NULL;
	m_pTimeline = NULL;

	// Initialize our class members to FALSE
	m_fValidRoutine = FALSE;
	m_fNeedToDetach = FALSE;
	m_fIgnoreSelChange = FALSE;
}

CTabRoutine::~CTabRoutine()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	FileListInfo* pFileListInfo;
	while( !m_lstFileListInfo.IsEmpty() )
	{
		pFileListInfo = static_cast<FileListInfo*>( m_lstFileListInfo.RemoveHead() );
		delete pFileListInfo;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabRoutine::DoDataExchange

void CTabRoutine::DoDataExchange(CDataExchange* pDX)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	CPropertyPage::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(CTabRoutine)
	DDX_Control(pDX, IDC_SPIN_BELONGS_MEASURE, m_spinLogicalMeasure);
	DDX_Control(pDX, IDC_SPIN_BELONGS_BEAT, m_spinLogicalBeat);
	DDX_Control(pDX, IDC_EDIT_BELONGS_MEASURE, m_editLogicalMeasure);
	DDX_Control(pDX, IDC_EDIT_BELONGS_BEAT, m_editLogicalBeat);
	DDX_Control(pDX, IDC_STATIC_PROMPT_TICK, m_staticPromptTick);
	DDX_Control(pDX, IDC_STATIC_PROMPT_MEASURE, m_staticPromptMeasure);
	DDX_Control(pDX, IDC_STATIC_PROMPT_BEAT, m_staticPromptBeat);
	DDX_Control(pDX, IDC_COMBO_SCRIPT, m_comboScript);
	DDX_Control(pDX, IDC_COMBO_ROUTINE, m_comboRoutine);
	DDX_Control(pDX, IDC_TIMING_QUICK, m_radioTimingQuick);
	DDX_Control(pDX, IDC_TIMING_BEFORE_TIME, m_radioTimingBeforeTime);
	DDX_Control(pDX, IDC_TIMING_AT_TIME, m_radioTimingAtTime);
	DDX_Control(pDX, IDC_SPIN_TICK, m_spinTick);
	DDX_Control(pDX, IDC_EDIT_TICK, m_editTick);
	DDX_Control(pDX, IDC_EDIT_BEAT, m_editBeat);
	DDX_Control(pDX, IDC_EDIT_MEASURE, m_editMeasure);
	DDX_Control(pDX, IDC_SPIN_MEASURE, m_spinMeasure);
	DDX_Control(pDX, IDC_SPIN_BEAT, m_spinBeat);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTabRoutine, CPropertyPage)
	//{{AFX_MSG_MAP(CTabRoutine)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_TICK, OnDeltaposSpinTick)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_BEAT, OnDeltaposSpinBeat)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_MEASURE, OnDeltaposSpinMeasure)
	ON_EN_KILLFOCUS(IDC_EDIT_TICK, OnKillfocusEditTick)
	ON_EN_KILLFOCUS(IDC_EDIT_BEAT, OnKillfocusEditBeat)
	ON_EN_KILLFOCUS(IDC_EDIT_MEASURE, OnKillfocusEditMeasure)
	ON_CBN_DROPDOWN(IDC_COMBO_SCRIPT, OnDropDownComboScript)
	ON_CBN_SELCHANGE(IDC_COMBO_SCRIPT, OnSelChangeComboScript)
	ON_CBN_DROPDOWN(IDC_COMBO_ROUTINE, OnDropDownComboRoutine)
	ON_CBN_SELCHANGE(IDC_COMBO_ROUTINE, OnSelChangeComboRoutine)
	ON_BN_CLICKED(IDC_TIMING_QUICK, OnTimingQuick)
	ON_BN_DOUBLECLICKED(IDC_TIMING_QUICK, OnDoubleClickedTimingQuick)
	ON_BN_CLICKED(IDC_TIMING_BEFORE_TIME, OnTimingBeforeTime)
	ON_BN_DOUBLECLICKED(IDC_TIMING_BEFORE_TIME, OnDoubleClickedTimingBeforeTime)
	ON_BN_CLICKED(IDC_TIMING_AT_TIME, OnTimingAtTime)
	ON_BN_DOUBLECLICKED(IDC_TIMING_AT_TIME, OnDoubleClickedTimingAtTime)
	ON_CBN_KILLFOCUS(IDC_COMBO_ROUTINE, OnKillFocusComboRoutine)
	ON_WM_KILLFOCUS()
	ON_EN_KILLFOCUS(IDC_EDIT_BELONGS_BEAT, OnKillFocusEditLogicalBeat)
	ON_EN_KILLFOCUS(IDC_EDIT_BELONGS_MEASURE, OnKillFocusEditLogicalMeasure)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_BELONGS_BEAT, OnDeltaPosSpinLogicalBeat)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_BELONGS_MEASURE, OnDeltaPosSpinLogicalMeasure)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTabRoutine custom functions

/////////////////////////////////////////////////////////////////////////////
// CTabRoutine::SetItem

void CTabRoutine::SetItem( const CTrackItem* pItem )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// If the pointer is null, there are no items selected, so disable the property page
	if ( pItem == NULL )
	{
		m_fValidRoutine = FALSE;
		m_TrackItem.Clear();
		EnableControls( FALSE );
		return;
	}

	// Flag that we have a valid Routine
	m_fValidRoutine = TRUE;

	// Copy the information from the new Routine
	m_TrackItem.Copy( pItem );

	// Check if the edit control has a valid window handle.
	if( m_editBeat.GetSafeHwnd() == NULL )
	{
		// It doesn't have a valid window handle - the property page may have been destroyed (or not yet created).
		// Just exit early
		return;
	}

	// Clear out the file combo box
	// this is necessary!
	{
		FileListInfo* pSLI;

		// Delete old FileInfo list
		while( !m_lstFileListInfo.IsEmpty() )
		{
			pSLI = static_cast<FileListInfo*>( m_lstFileListInfo.RemoveHead() );
			delete pSLI;
		}

		// Remove old list from combo box
		m_comboScript.ResetContent();
	}

	// Don't send OnKill/OnUpdate notifications when updating the display
	CLockoutNotification LockoutNotifications( m_hWnd );

	// Check if multiple items are selected
	if( m_TrackItem.m_dwBitsUI & UD_MULTIPLESELECT )
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

	// Set Script combo box selection
	m_fIgnoreSelChange = TRUE;
	SetScriptComboBoxSelection();
	m_fIgnoreSelChange = FALSE;

	// Set Routine combo box selection
	m_fIgnoreSelChange = TRUE;
	SetRoutineComboBoxSelection();
	m_fIgnoreSelChange = FALSE;

	// Set nMeasureBeatOffset
	int nMeasureBeatOffset;
	if( m_pPropPageMgr->m_pTrackMgr->IsRefTimeTrack() )
	{
		nMeasureBeatOffset = 0;
	}
	else
	{
		nMeasureBeatOffset = 1;
	}

	// Update START controls
	SetEditControl( m_editMeasure, m_TrackItem.m_lMeasure, nMeasureBeatOffset );
	SetEditControl( m_editBeat, m_TrackItem.m_lBeat, nMeasureBeatOffset );
	SetEditControl( m_editTick, m_TrackItem.m_lTick, 0 );

	// Update BELONGS TO controls
	if( m_pPropPageMgr->m_pTrackMgr->IsRefTimeTrack() == false )
	{
		SetEditControl( m_editLogicalMeasure, m_TrackItem.m_lLogicalMeasure, 1 );
		SetEditControl( m_editLogicalBeat, m_TrackItem.m_lLogicalBeat, 1 );
	}

	// Set 'Timing' radio buttons
	if( m_TrackItem.m_dwFlagsDM & DMUS_IO_SCRIPTTRACKF_PREPARE )
	{
		if( m_radioTimingQuick.GetCheck() == 0 )
		{
			m_radioTimingQuick.SetCheck( 1 );
			m_radioTimingBeforeTime.SetCheck( 0 );
			m_radioTimingAtTime.SetCheck( 0 );
		}
	}
	if( m_TrackItem.m_dwFlagsDM & DMUS_IO_SCRIPTTRACKF_QUEUE )
	{
		if( m_radioTimingBeforeTime.GetCheck() == 0 )
		{
			m_radioTimingQuick.SetCheck( 0 );
			m_radioTimingBeforeTime.SetCheck( 1 );
			m_radioTimingAtTime.SetCheck( 0 );
		}
	}
	if( m_TrackItem.m_dwFlagsDM & DMUS_IO_SCRIPTTRACKF_ATTIME )
	{
		if( m_radioTimingAtTime.GetCheck() == 0 )
		{
			m_radioTimingQuick.SetCheck( 0 );
			m_radioTimingBeforeTime.SetCheck( 0 );
			m_radioTimingAtTime.SetCheck( 1 );
		}
	}

	// Set ranges of spin controls
	SetControlRanges();
}
	

/////////////////////////////////////////////////////////////////////////////
// CTabRoutine::SetEditControl

void CTabRoutine::SetEditControl( CEdit& edit, long lValue, int nOffset )
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
// CTabRoutine::SetControlRanges

void CTabRoutine::SetControlRanges( void )
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
	if( m_pPropPageMgr->m_pTrackMgr->IsRefTimeTrack() )
	{
		SetRefTimeRanges();
	}
	else
	{
		SetMusicTimeRanges( dwGroupBits );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabRoutine::SetRefTimeRanges

void CTabRoutine::SetRefTimeRanges( void )
{
	// Limit the minute number to positive numbers
	m_spinMeasure.SetRange( 1, 32767 );
	m_editMeasure.LimitText( 5 );

	// Limit the second number to 1-60
	m_spinBeat.SetRange( 1, 60 );
	m_editBeat.LimitText( 2 );

	// Limit the millisecond number to 1-1000
	m_spinTick.SetRange( 1, 1000 );
	m_editTick.LimitText( 4 );

	// Limit the "Belongs To" measure number to 1-32767
	m_spinLogicalMeasure.SetRange( 1, 32767 );
	m_editLogicalMeasure.LimitText( 5 );

	// Limit the "Belongs To" beat number to 1-256
	m_spinLogicalBeat.SetRange( 1, 256 );
	m_editLogicalBeat.LimitText( 3 );
}


/////////////////////////////////////////////////////////////////////////////
// CTabRoutine::SetMusicTimeRanges

void CTabRoutine::SetMusicTimeRanges( DWORD dwGroupBits )
{
	long lMaxMeasure, lMaxBeat;

	// Get the maximum measure value
	if( m_pPropPageMgr->m_pTrackMgr->m_pTimeline == NULL )
	{
		// No timeline pointer - use default values
		lMaxMeasure = 32767;
		lMaxBeat = 256;
	}
	else
	{
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

		long lItemMeasure, lItemBeat, lItemTick;
		m_pPropPageMgr->m_pTrackMgr->UnknownTimeToMeasureBeatTick( m_TrackItem.m_rtTimePhysical,
																   &lItemMeasure, &lItemBeat, &lItemTick );

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
	m_spinMeasure.SetRange( 1, lMaxMeasure );
	m_editMeasure.LimitText( 5 );

	// Update the range for the beat spin control
	m_spinBeat.SetRange( 1, lMaxBeat );
	m_editBeat.LimitText( 3 );

	// Update the range for the tick spin control
	m_spinTick.SetRange( -MAX_TICK, MAX_TICK );
	m_editTick.LimitText( 5 );

	// Update the range for the "Belongs To" measure spin control
	m_spinLogicalMeasure.SetRange( 1, lMaxMeasure );
	m_editLogicalMeasure.LimitText( 5 );

	// Update the range for the "Belongs To" beat spin control
	m_spinLogicalBeat.SetRange( 1, lMaxBeat );
	m_editLogicalBeat.LimitText( 3 );
}


/////////////////////////////////////////////////////////////////////////////
// CTabRoutine::SetStart

void CTabRoutine::SetStart( REFERENCE_TIME rtNewStart ) 
{
	REFERENCE_TIME rtSegmentLength = m_pPropPageMgr->m_pTrackMgr->GetSegmentLength();
	REFERENCE_TIME rtMinTime = MinTimeToUnknownTime();

	REFERENCE_TIME rtMinStart;
	m_pPropPageMgr->m_pTrackMgr->ClocksToUnknownTime( -MAX_TICK, &rtMinStart );

	rtNewStart = max( rtNewStart, rtMinStart );
	rtNewStart = min( rtNewStart, rtSegmentLength - rtMinTime );

	if( m_TrackItem.m_rtTimePhysical != rtNewStart )
	{
		m_TrackItem.m_rtTimePhysical = rtNewStart;

		// Now, update the object with the new value(s)
		UpdateObject();
	}
	else
	{
		// Display original values
		SetItem( &m_TrackItem );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabRoutine::StartToUnknownTime

REFERENCE_TIME CTabRoutine::StartToUnknownTime( void )
{
	REFERENCE_TIME rtNewStart;

	if( m_pPropPageMgr->m_pTrackMgr->IsRefTimeTrack() )
	{
		REFERENCE_TIME rtTime;
		m_pPropPageMgr->m_pTrackMgr->MinSecMsToRefTime( m_TrackItem.m_lMeasure, m_TrackItem.m_lBeat, m_TrackItem.m_lTick,
														&rtTime );
		m_pPropPageMgr->m_pTrackMgr->RefTimeToUnknownTime( rtTime, &rtNewStart );
	}
	else
	{
		MUSIC_TIME mtTime;
		m_pPropPageMgr->m_pTrackMgr->MeasureBeatTickToClocks( m_TrackItem.m_lMeasure, m_TrackItem.m_lBeat, m_TrackItem.m_lTick,
															  &mtTime );
		m_pPropPageMgr->m_pTrackMgr->ClocksToUnknownTime( mtTime, &rtNewStart );
	}

	return rtNewStart;
}


/////////////////////////////////////////////////////////////////////////////
// CTabRoutine::MinTimeToUnknownTime

REFERENCE_TIME CTabRoutine::MinTimeToUnknownTime( void )
{
	REFERENCE_TIME rtMinTime;

	if( m_pPropPageMgr->m_pTrackMgr->IsRefTimeTrack() )
	{
		m_pPropPageMgr->m_pTrackMgr->MinSecMsToRefTime( 0, 0, 1, &rtMinTime );
	}
	else
	{
		MUSIC_TIME mtMinTime;
		m_pPropPageMgr->m_pTrackMgr->MeasureBeatTickToClocks( 0, 0, 1, &mtMinTime );
		rtMinTime = mtMinTime;
	}

	return rtMinTime;
}


/////////////////////////////////////////////////////////////////////////////
// CTabRoutine::SetScriptComboBoxSelection

void CTabRoutine::SetScriptComboBoxSelection( void ) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	ASSERT( m_pPropPageMgr->m_pIFramework != NULL );

	IDMUSProdProject* pIProject = NULL;
	CString strText;
	int nMatch;
	int nCount;
	int nCurPos;
	int nPos = -1;

	// Make sure multiple Scripts were not selected
	ASSERT( !(m_TrackItem.m_dwBitsUI & UD_MULTIPLESELECT) );
	if( m_TrackItem.m_dwBitsUI & UD_MULTIPLESELECT )
	{
		goto LEAVE;
	}

	// Nothing to select
	if( m_TrackItem.m_FileRef.pIDocRootNode == NULL )
	{
		goto LEAVE;
	}

	// Create a FileListInfo struct for the current Script
	DMUSProdListInfo ListInfo;
	FileListInfo* pFileListInfo;

	ZeroMemory( &ListInfo, sizeof(ListInfo) );
	ListInfo.wSize = sizeof(ListInfo);

	pFileListInfo = new FileListInfo;
	if( pFileListInfo == NULL )
	{
		goto LEAVE;
	}

	// Update DocRoot file GUID
	m_pPropPageMgr->m_pIFramework->GetNodeFileGUID ( m_TrackItem.m_FileRef.pIDocRootNode, &pFileListInfo->guidFile );

	if( SUCCEEDED ( m_TrackItem.m_FileRef.pIDocRootNode->GetNodeListInfo ( &ListInfo ) ) )
	{
		if( ListInfo.bstrName )
		{
			pFileListInfo->strName = ListInfo.bstrName;
			::SysFreeString( ListInfo.bstrName );
		}
		
		if( ListInfo.bstrDescriptor )
		{
			pFileListInfo->strDescriptor = ListInfo.bstrDescriptor;
			::SysFreeString( ListInfo.bstrDescriptor );
		}

		if( FAILED ( m_pPropPageMgr->m_pIFramework->FindProject( m_TrackItem.m_FileRef.pIDocRootNode, &pIProject ) ) )
		{
			delete pFileListInfo;
			goto LEAVE;
		}

		pFileListInfo->pIProject = pIProject;
//		pFileListInfo->pIProject->AddRef();	intentionally missing

		BSTR bstrProjectName;
		if( FAILED ( pIProject->GetName( &bstrProjectName ) ) )
		{
			delete pFileListInfo;
			goto LEAVE;
		}

		pFileListInfo->strProjectName = bstrProjectName;
		::SysFreeString( bstrProjectName );
	}
	else
	{
		delete pFileListInfo;
		goto LEAVE;
	}

	// Select the Script in the combo box list
	nMatch = CB_ERR;
	nCount = m_comboScript.GetCount();
	for( nCurPos = 0 ;  nCurPos < nCount ;  nCurPos++ )
	{
		FileListInfo* pCurFileListInfo = (FileListInfo *)m_comboScript.GetItemDataPtr( nCurPos );
		if( pCurFileListInfo )
		{
			if( pCurFileListInfo != (FileListInfo *)-1 )
			{
				// See if GUIDs are equal
				if( ::IsEqualGUID( pCurFileListInfo->guidFile, pFileListInfo->guidFile ) )
				{
					nMatch = nCurPos;
					break;
				}
			}
		}
	}

	if( nMatch == CB_ERR )
	{
		FileListInfo* pSLI;

		// Delete old ScriptInfo list
		while( !m_lstFileListInfo.IsEmpty() )
		{
			pSLI = static_cast<FileListInfo*>( m_lstFileListInfo.RemoveHead() );
			delete pSLI;
		}

		// Remove old list from combo box
		m_comboScript.ResetContent();

		// Add this Script to the combo box list
		nPos = 0;
		m_lstFileListInfo.AddTail( pFileListInfo );
		InsertScriptInfoListInComboBox();
	}
	else
	{
		nPos = nMatch;
		delete pFileListInfo;
		pFileListInfo = NULL;
	}

LEAVE:
	m_comboScript.SetCurSel( nPos );

	if( pIProject )
	{
		pIProject->Release();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabRoutine::GetScriptComboBoxText

void CTabRoutine::GetScriptComboBoxText( const FileListInfo* pFileListInfo, CString& strText )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( pFileListInfo->strProjectName.IsEmpty() )
	{
		strText.Format( "%s %s", pFileListInfo->strName, pFileListInfo->strDescriptor );
	}
	else
	{
		strText.Format( "%s: %s %s", pFileListInfo->strProjectName, pFileListInfo->strName, pFileListInfo->strDescriptor );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabRoutine::InsertScriptInfoListInComboBox

void CTabRoutine::InsertScriptInfoListInComboBox( void ) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	CString strText;
	FileListInfo* pFileListInfo;
	int nPos;

	POSITION pos = m_lstFileListInfo.GetHeadPosition();
	while( pos != NULL )
	{
		pFileListInfo = static_cast<FileListInfo*>( m_lstFileListInfo.GetNext(pos) );
		if( pFileListInfo )
		{
			GetScriptComboBoxText( pFileListInfo, strText );
			nPos = m_comboScript.AddString( strText );
			if( nPos >= 0 )
			{
				m_comboScript.SetItemDataPtr( nPos, pFileListInfo );
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabRoutine::InsertScriptInfo

void CTabRoutine::InsertScriptInfo( FileListInfo* pFileListInfo )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	FileListInfo* pListInfo;
	CString strListText;
	CString strScriptText;
	POSITION posList;

	GetScriptComboBoxText( pFileListInfo, strScriptText );

	POSITION pos = m_lstFileListInfo.GetHeadPosition();
	while( pos != NULL )
	{
		posList = pos;

		pListInfo = static_cast<FileListInfo*>( m_lstFileListInfo.GetNext(pos) );
		if( pListInfo )
		{
			if( !pFileListInfo->strProjectName.IsEmpty()
			&&  pListInfo->strProjectName.IsEmpty() )
			{
				continue;
			}

			if( pFileListInfo->strProjectName.IsEmpty()
			&&  !pListInfo->strProjectName.IsEmpty() )
			{
				if( pos )
				{
					m_lstFileListInfo.InsertBefore( pos, pFileListInfo );
				}
				else
				{
					m_lstFileListInfo.AddTail( pFileListInfo );
				}
				return;
			}

			GetScriptComboBoxText( pListInfo, strListText );

			if( strListText.CompareNoCase( strScriptText ) > 0 )
			{
				m_lstFileListInfo.InsertBefore( posList, pFileListInfo );
				return;
			}
		}
	}

	m_lstFileListInfo.AddTail( pFileListInfo );
}


/////////////////////////////////////////////////////////////////////////////
// CTabRoutine::BuildScriptInfoList

void CTabRoutine::BuildScriptInfoList( void ) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	IDMUSProdDocType* pIDocType;
	IDMUSProdProject* pIProject;
	IDMUSProdProject* pINextProject;
	IDMUSProdNode* pIFileNode;
	IDMUSProdNode* pINextFileNode;
	BSTR bstrProjectName;
	GUID guidProject;
	DMUSProdListInfo ListInfo;
	FileListInfo* pFileListInfo;

	if( FAILED ( m_pPropPageMgr->m_pIFramework->FindDocTypeByNodeId( GUID_ScriptNode, &pIDocType ) ) )
	{
		return;
	}

	HRESULT hr = m_pPropPageMgr->m_pIFramework->GetFirstProject( &pINextProject );

	while( SUCCEEDED( hr )  &&  pINextProject )
    {
		pIProject = pINextProject;

		HRESULT hr = pIProject->GetFirstFileByDocType( pIDocType, &pINextFileNode );

		while( hr == S_OK )
		{
			pIFileNode = pINextFileNode;

			ZeroMemory( &ListInfo, sizeof(ListInfo) );
			ListInfo.wSize = sizeof(ListInfo);

			if( SUCCEEDED ( pIFileNode->GetNodeListInfo ( &ListInfo ) ) )
			{
				pFileListInfo = new FileListInfo;

				pFileListInfo->pIProject = pIProject;
//				pFileListInfo->pIProject->AddRef();	intentionally missing

				if( SUCCEEDED ( pIProject->GetGUID( &guidProject ) ) )
				{
//					if( !IsEqualGUID( guidProject, m_guidProject ) )
					{
						if( SUCCEEDED ( pIProject->GetName( &bstrProjectName ) ) )
						{
							pFileListInfo->strProjectName = bstrProjectName;
							::SysFreeString( bstrProjectName );
						}
					}

				}

				if( ListInfo.bstrName )
				{
					pFileListInfo->strName = ListInfo.bstrName;
					::SysFreeString( ListInfo.bstrName );
				}
				
				if( ListInfo.bstrDescriptor )
				{
					pFileListInfo->strDescriptor = ListInfo.bstrDescriptor;
					::SysFreeString( ListInfo.bstrDescriptor );
				}

				// Update DocRoot file GUID
				m_pPropPageMgr->m_pIFramework->GetNodeFileGUID ( pIFileNode, &pFileListInfo->guidFile );

				InsertScriptInfo( pFileListInfo );

				hr = pIProject->GetNextFileByDocType( pIFileNode, &pINextFileNode );
				pIFileNode->Release();
			}
			else
			{
				hr = E_FAIL;
			}
		}
	
	    hr = m_pPropPageMgr->m_pIFramework->GetNextProject( pIProject, &pINextProject );
		pIProject->Release();
	}

	pIDocType->Release();
}


/////////////////////////////////////////////////////////////////////////////
// CTabRoutine::FillRoutineComboBox

void CTabRoutine::FillRoutineComboBox( void ) 
{
	// Remove old list from combo box
	m_comboRoutine.ResetContent();

	// Make sure multiple Scripts were not selected
	ASSERT( !(m_TrackItem.m_dwBitsUI & UD_MULTIPLESELECT) );
	if( m_TrackItem.m_dwBitsUI & UD_MULTIPLESELECT )
	{
		return;
	}

	// Make sure there is a script from which we can extract names
	if( m_TrackItem.m_FileRef.pIDocRootNode == NULL )
	{
		return;
	}

	IDirectMusicScript* pIDMScript;
	CString strRoutineName;
	WCHAR awchRoutineName[MAX_PATH];

	if( SUCCEEDED ( m_TrackItem.m_FileRef.pIDocRootNode->GetObject( CLSID_DirectMusicScript, IID_IDirectMusicScript, (void**)&pIDMScript ) ) )
	{
		int i = 0;
		while( pIDMScript->EnumRoutine( i++, awchRoutineName ) == S_OK )
		{
			strRoutineName = awchRoutineName;
			m_comboRoutine.AddString( strRoutineName );
		}

		pIDMScript->Release();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabRoutine::SetRoutineComboBoxSelection

void CTabRoutine::SetRoutineComboBoxSelection( void ) 
{
	// Make sure multiple Scripts were not selected
	ASSERT( !(m_TrackItem.m_dwBitsUI & UD_MULTIPLESELECT) );
	if( m_TrackItem.m_dwBitsUI & UD_MULTIPLESELECT )
	{
		m_comboRoutine.SetCurSel( -1 );
		return;
	}

	// Rebuild the Routine list
	FillRoutineComboBox();

	// Select the Routine in the combo box list
	int nPos = m_comboRoutine.FindStringExact( -1, m_TrackItem.m_strRoutine );
	if( nPos != CB_ERR )
	{
		m_comboRoutine.SetCurSel( nPos );
	}
	else
	{
		m_comboRoutine.SetWindowText( m_TrackItem.m_strRoutine );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabRoutine message handlers

/////////////////////////////////////////////////////////////////////////////
// CTabRoutine::OnCreate

int CTabRoutine::OnCreate(LPCREATESTRUCT lpCreateStruct) 
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
// CTabRoutine::OnDestroy

void CTabRoutine::OnDestroy() 
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
// CTabRoutine::OnInitDialog

BOOL CTabRoutine::OnInitDialog() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Call the base class's OnInitDialog method
	CPropertyPage::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
	               // EXCEPTION: OCX Property Pages should return FALSE
}


/////////////////////////////////////////////////////////////////////////////
// CTabRoutine::OnDeltaposSpinTick

void CTabRoutine::OnDeltaposSpinTick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// Need to do this in case the user clicked the spin control immediately after
	// typing in a value
	OnKillfocusEditTick();

	if( HandleDeltaChange( pNMHDR, pResult, m_TrackItem.m_lTick ) )
	{
		SetStart( StartToUnknownTime() );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabRoutine::OnDeltaposSpinBeat

void CTabRoutine::OnDeltaposSpinBeat(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// Need to do this in case the user clicked the spin control immediately after
	// typing in a value
	OnKillfocusEditBeat();

	if( HandleDeltaChange( pNMHDR, pResult, m_TrackItem.m_lBeat ) )
	{
		SetStart( StartToUnknownTime() );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabRoutine::OnDeltaposSpinMeasure

void CTabRoutine::OnDeltaposSpinMeasure(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// Need to do this in case the user clicked the spin control immediately after
	// typing in a value
	OnKillfocusEditMeasure();

	if( HandleDeltaChange( pNMHDR, pResult, m_TrackItem.m_lMeasure ) )
	{
		SetStart( StartToUnknownTime() );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabRoutine::OnDeltaPosSpinLogicalBeat

void CTabRoutine::OnDeltaPosSpinLogicalBeat(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// Need to do this in case the user clicked the spin control immediately after
	// typing in a value
	OnKillFocusEditLogicalBeat();

	if( HandleDeltaChange( pNMHDR, pResult, m_TrackItem.m_lLogicalBeat ) )
	{
		UpdateObject();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabRoutine::OnDeltaPosSpinLogicalMeasure

void CTabRoutine::OnDeltaPosSpinLogicalMeasure(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// Need to do this in case the user clicked the spin control immediately after
	// typing in a value
	OnKillFocusEditLogicalMeasure();

	if( HandleDeltaChange( pNMHDR, pResult, m_TrackItem.m_lLogicalMeasure ) )
	{
		UpdateObject();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabRoutine::HandleDeltaChange
//
// Generic handler for deltapos changes
bool CTabRoutine::HandleDeltaChange( NMHDR* pNMHDR, LRESULT* pResult, long& lUpdateVal )
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
// CTabRoutine::OnKillfocusEditTick

void CTabRoutine::OnKillfocusEditTick() 
{
	if( HandleKillFocus( m_spinTick, m_TrackItem.m_lTick, 0 ) )
	{
		SetStart( StartToUnknownTime() );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabRoutine::OnKillfocusEditBeat

void CTabRoutine::OnKillfocusEditBeat() 
{
	int nOffset;
	if( m_pPropPageMgr->m_pTrackMgr->IsRefTimeTrack() )
	{
		nOffset = 0;
	}
	else
	{
		nOffset = 1;
	}

	if( HandleKillFocus( m_spinBeat, m_TrackItem.m_lBeat, nOffset ) )
	{
		SetStart( StartToUnknownTime() );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabRoutine::OnKillfocusEditMeasure

void CTabRoutine::OnKillfocusEditMeasure() 
{
	int nOffset;
	if( m_pPropPageMgr->m_pTrackMgr->IsRefTimeTrack() )
	{
		nOffset = 0;
	}
	else
	{
		nOffset = 1;
	}

	if( HandleKillFocus( m_spinMeasure, m_TrackItem.m_lMeasure, nOffset ) )
	{
		SetStart( StartToUnknownTime() );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabRoutine::OnKillFocusEditLogicalBeat

void CTabRoutine::OnKillFocusEditLogicalBeat() 
{
	if( HandleKillFocus( m_spinLogicalBeat, m_TrackItem.m_lLogicalBeat, 1 ) )
	{
		UpdateObject();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabRoutine::OnKillFocusEditLogicalMeasure

void CTabRoutine::OnKillFocusEditLogicalMeasure() 
{
	if( HandleKillFocus( m_spinLogicalMeasure, m_TrackItem.m_lLogicalMeasure, 1 ) )
	{
		UpdateObject();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabRoutine::HandleKillFocus
//
// Generic handler for KillFocus changes
bool CTabRoutine::HandleKillFocus( CSpinButtonCtrl& spin, long& lUpdateVal, int nOffset )
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


/////////////////////////////////////////////////////////////////////////////
// CTabRoutine::UpdateObject

void CTabRoutine::UpdateObject( void )
{
	// Check for a valid Property Page Object pointer
	if( m_pPropPageMgr
	&&  m_pPropPageMgr->m_pIPropPageObject )
	{
		// Update the Property Page Object with the new Routine
		m_pPropPageMgr->m_pIPropPageObject->SetData( (void *)&m_TrackItem );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabRoutine::EnableControls

void CTabRoutine::EnableControls( BOOL fEnable )
{
	// Check if the Beat edit control exists
	if( m_editBeat.GetSafeHwnd() == NULL )
	{
		return;
	}

	// Reference time or music time?
	CString strPrompt;
	if( m_pPropPageMgr->m_pTrackMgr->IsRefTimeTrack() )
	{
		strPrompt.LoadString( IDS_MINUTE_PROMPT );
		m_staticPromptMeasure.SetWindowText( strPrompt );
		strPrompt.LoadString( IDS_SECOND_PROMPT );
		m_staticPromptBeat.SetWindowText( strPrompt );
		strPrompt.LoadString( IDS_MS_PROMPT );
		m_staticPromptTick.SetWindowText( strPrompt );
	}
	else
	{
		strPrompt.LoadString( IDS_MEASURE_PROMPT );
		m_staticPromptMeasure.SetWindowText( strPrompt );
		strPrompt.LoadString( IDS_BEAT_PROMPT );
		m_staticPromptBeat.SetWindowText( strPrompt );
		strPrompt.LoadString( IDS_TICK_PROMPT );
		m_staticPromptTick.SetWindowText( strPrompt );
	}

	// Update the tick, beat, measure, and text controls with the new window state
	m_spinTick.EnableWindow( fEnable );
	m_spinBeat.EnableWindow( fEnable );
	m_spinMeasure.EnableWindow( fEnable );
	m_editTick.EnableWindow( fEnable );
	m_editBeat.EnableWindow( fEnable );
	m_editMeasure.EnableWindow( fEnable );
	m_comboScript.EnableWindow( fEnable );
	m_comboRoutine.EnableWindow( fEnable );
	m_radioTimingQuick.EnableWindow( fEnable );
	m_radioTimingBeforeTime.EnableWindow( fEnable );
	m_radioTimingAtTime.EnableWindow( fEnable );

	BOOL fEnableBelongsTo = FALSE;
	if( m_pPropPageMgr->m_pTrackMgr->IsRefTimeTrack() == false )
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
		if( m_TrackItem.m_dwBitsUI & UD_MULTIPLESELECT )
		{
			// Load the 'multiple select' text
			CString strText;
			if( strText.LoadString( IDS_MULTIPLE_SELECT ) )
			{
				// Display the 'multiple select' text
				int nPos = m_comboScript.AddString( strText );
				m_comboScript.SetCurSel( nPos );
//				return;
			}
		}
		else
		{
			m_comboScript.SetCurSel( -1 );
		}

		// No items are selected, so clear the edit controls
		m_comboRoutine.SetCurSel( -1 );
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
// CTabRoutine::OnOK

// Windows translated presses of the 'Enter' key into presses of the button 'OK'.
// Here we create a hidden 'OK' button and translate presses of it into tabs to the next
// control in the property page.
void CTabRoutine::OnOK() 
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


/////////////////////////////////////////////////////////////////////////////
// CTabRoutine::OnDropDownComboScript

void CTabRoutine::OnDropDownComboScript() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Delete old ScriptInfo list
	FileListInfo* pFileListInfo;
	while( !m_lstFileListInfo.IsEmpty() )
	{
		pFileListInfo = static_cast<FileListInfo*>( m_lstFileListInfo.RemoveHead() );
		delete pFileListInfo;
	}

	// Remove old list from combo box
	m_comboScript.ResetContent();

	// Rebuild the ScriptInfo list
	BuildScriptInfoList();
	InsertScriptInfoListInComboBox();

	// Select the current Script
	SetScriptComboBoxSelection();
}


/////////////////////////////////////////////////////////////////////////////
// CTabRoutine::OnSelChangeComboScript

void CTabRoutine::OnSelChangeComboScript() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	ASSERT( m_pPropPageMgr->m_pIFramework != NULL );

	if( m_fIgnoreSelChange )
	{
		return;
	}

	int nItem = m_comboScript.GetCurSel();
	if( nItem != CB_ERR )
	{
		FileListInfo* pFileListInfo = (FileListInfo *)m_comboScript.GetItemDataPtr( nItem );
		if( pFileListInfo != (FileListInfo *)-1 )
		{
			// fill in appropriate fields
			m_TrackItem.m_FileRef.li = *pFileListInfo;

			IDMUSProdNode* pIDocRootNode = NULL;
			
			// NOTE: Scripts must currently be selected to avoid killing
			// the property page while making this call
			if( SUCCEEDED ( m_pPropPageMgr->m_pIFramework->FindDocRootNodeByFileGUID( pFileListInfo->guidFile, &pIDocRootNode ) ) )
			{
				if( m_TrackItem.m_FileRef.pIDocRootNode )
				{
					m_TrackItem.m_FileRef.pIDocRootNode->Release();
					m_TrackItem.m_FileRef.pIDocRootNode = NULL;
				}
				
				m_TrackItem.m_FileRef.pIDocRootNode = pIDocRootNode;
				if( m_TrackItem.m_FileRef.pIDocRootNode )
				{
					m_TrackItem.m_FileRef.pIDocRootNode->AddRef();
				}

				UpdateObject();
//				m_pPropPageMgr->UpdateObjectWithScriptRefData();

				pIDocRootNode->Release();
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabRoutine::OnDropDownComboRoutine

void CTabRoutine::OnDropDownComboRoutine() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Select the current Routine
	SetRoutineComboBoxSelection();
}


/////////////////////////////////////////////////////////////////////////////
// CTabRoutine::OnSelChangeComboRoutine

void CTabRoutine::OnSelChangeComboRoutine() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_fIgnoreSelChange )
	{
		return;
	}

	int nItem = m_comboRoutine.GetCurSel();
	if( nItem != CB_ERR )
	{
		TCHAR achText[MAX_PATH];

		if( m_comboRoutine.GetLBText(nItem, achText) != CB_ERR )
		{
			m_TrackItem.m_strRoutine = achText;
			UpdateObject();
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabRoutine::OnKillFocusComboRoutine

void CTabRoutine::OnKillFocusComboRoutine() 
{
	// Make sure multiple Scripts were not selected
	ASSERT( !(m_TrackItem.m_dwBitsUI & UD_MULTIPLESELECT) );
	if( m_TrackItem.m_dwBitsUI & UD_MULTIPLESELECT )
	{
		return;
	}

	// Get text from the control
	CString strRoutineComboBoxText;
	m_comboRoutine.GetWindowText( strRoutineComboBoxText );
	strRoutineComboBoxText.TrimLeft();
	strRoutineComboBoxText.TrimRight();

	if( strRoutineComboBoxText.IsEmpty() )
	{
		strRoutineComboBoxText.LoadString( IDS_NONE_TEXT );
		m_comboRoutine.SetWindowText( strRoutineComboBoxText );
	}

	// See if the text has changed
	if( strRoutineComboBoxText != m_TrackItem.m_strRoutine )
	{
		// Update the track item
		m_TrackItem.m_strRoutine = strRoutineComboBoxText;
		UpdateObject();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabRoutine::OnTimingQuick

void CTabRoutine::OnTimingQuick() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_pPropPageMgr->m_pIPropPageObject == NULL )
	{
		return;
	}

	if( m_radioTimingQuick.GetCheck() )
	{
		m_TrackItem.m_dwFlagsDM &= ~(DMUS_IO_SCRIPTTRACKF_PREPARE | DMUS_IO_SCRIPTTRACKF_QUEUE | DMUS_IO_SCRIPTTRACKF_ATTIME);
		m_TrackItem.m_dwFlagsDM |= DMUS_IO_SCRIPTTRACKF_PREPARE;

		// Now, update the object with the new value
		UpdateObject();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabRoutine::OnDoubleClickedTimingQuick

void CTabRoutine::OnDoubleClickedTimingQuick() 
{
	OnTimingQuick();
}


/////////////////////////////////////////////////////////////////////////////
// CTabRoutine::OnTimingBeforeTime

void CTabRoutine::OnTimingBeforeTime() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_pPropPageMgr->m_pIPropPageObject == NULL )
	{
		return;
	}

	if( m_radioTimingBeforeTime.GetCheck() )
	{
		m_TrackItem.m_dwFlagsDM &= ~(DMUS_IO_SCRIPTTRACKF_PREPARE | DMUS_IO_SCRIPTTRACKF_QUEUE | DMUS_IO_SCRIPTTRACKF_ATTIME);
		m_TrackItem.m_dwFlagsDM |= DMUS_IO_SCRIPTTRACKF_QUEUE;

		// Now, update the object with the new value
		UpdateObject();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabRoutine::OnDoubleClickedTimingBeforeTime

void CTabRoutine::OnDoubleClickedTimingBeforeTime() 
{
	OnTimingBeforeTime();
}


/////////////////////////////////////////////////////////////////////////////
// CTabRoutine::OnTimingAtTime

void CTabRoutine::OnTimingAtTime() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_pPropPageMgr->m_pIPropPageObject == NULL )
	{
		return;
	}

	if( m_radioTimingAtTime.GetCheck() )
	{
		m_TrackItem.m_dwFlagsDM &= ~(DMUS_IO_SCRIPTTRACKF_PREPARE | DMUS_IO_SCRIPTTRACKF_QUEUE | DMUS_IO_SCRIPTTRACKF_ATTIME);
		m_TrackItem.m_dwFlagsDM |= DMUS_IO_SCRIPTTRACKF_ATTIME;

		// Now, update the object with the new value
		UpdateObject();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabRoutine::OnDoubleClickedTimingAtTime

void CTabRoutine::OnDoubleClickedTimingAtTime() 
{
	OnTimingAtTime();
}
