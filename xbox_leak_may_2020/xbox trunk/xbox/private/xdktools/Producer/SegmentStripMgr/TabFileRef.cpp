// TabFileRef.cpp : implementation file
//

#include "stdafx.h"
#include <RiffStrm.h>
#include "TrackMgr.h"
#include "TrackItem.h"
#include "PropPageMgr_Item.h"
#include "TabFileRef.h"
#include "TabBoundary.h"
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

// {12ABDED0-76BC-11d3-B45F-00105A2796DE}
static const GUID GUID_SegmentTriggerItemPPGMgr = 
{ 0x12abded0, 0x76bc, 0x11d3, { 0xb4, 0x5f, 0x0, 0x10, 0x5a, 0x27, 0x96, 0xde } };

short CPropPageMgrItem::sm_nActiveTab = 0;

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
	m_GUIDManager = GUID_SegmentTriggerItemPPGMgr;

	// Initialize our pointers to NULL
	m_pTabFileRef = NULL;
	m_pTabBoundary = NULL;

	// call the base class contstructor
	CStaticPropPageManager::CStaticPropPageManager();
}

CPropPageMgrItem::~CPropPageMgrItem()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_pTabFileRef )
	{
		delete m_pTabFileRef;
		m_pTabFileRef = NULL;
	}

	if( m_pTabBoundary )
	{
		delete m_pTabBoundary;
		m_pTabBoundary = NULL;
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
	if( comBSTR.LoadString( IDS_PROPPAGE_FILEREF ) )
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
	if( m_pTabFileRef == NULL )
	{
		m_pTabFileRef = new CTabFileRef();
	}

	// If the property page now exists, add the property page
	if( m_pTabFileRef )
	{
		// Copy the PROPSHEETPAGE structure
		PROPSHEETPAGE psp;
		memcpy( &psp, &m_pTabFileRef->m_psp, sizeof(PROPSHEETPAGE) );

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
		m_pTabFileRef->m_pPropPageMgr = this;
	}

	// If it doesn't exist yet, create the property page
	if( m_pTabBoundary == NULL )
	{
		m_pTabBoundary = new CTabBoundary();
	}

	// If the property page now exists, add the property page
	if( m_pTabBoundary )
	{
		// Copy the PROPSHEETPAGE structure
		PROPSHEETPAGE psp;
		memcpy( &psp, &m_pTabBoundary->m_psp, sizeof(PROPSHEETPAGE) );

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
		m_pTabBoundary->m_pPropPageMgr = this;
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

	// Update the property pages, if they exists
	if( m_pTabFileRef )
	{
		m_pTabFileRef->SetTrackItem( pItem );
	}
	if( m_pTabBoundary )
	{
		m_pTabBoundary->SetTrackItem( pItem );
	}

	return S_OK;
};


/////////////////////////////////////////////////////////////////////////////
// CTabFileRef property page

IMPLEMENT_DYNCREATE(CTabFileRef, CPropertyPage)

/////////////////////////////////////////////////////////////////////////////
// CTabFileRef constructor/destructor

CTabFileRef::CTabFileRef(): CPropertyPage(CTabFileRef::IDD)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

//	CPropertyPage(IDD_PROPPAGE_FILEREF);
	//{{AFX_DATA_INIT(CTabFileRef)
	//}}AFX_DATA_INIT

	// Initialize our pointers to NULL
	m_pPropPageMgr = NULL;
	m_pTimeline = NULL;

	// Initialize our class members to FALSE
	m_fValidTrackItem = FALSE;
	m_fNeedToDetach = FALSE;
	m_fIgnoreSelChange = FALSE;
}

CTabFileRef::~CTabFileRef()
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
// CTabFileRef::DoDataExchange

void CTabFileRef::DoDataExchange(CDataExchange* pDX)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	CPropertyPage::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(CTabFileRef)
	DDX_Control(pDX, IDC_SPIN_BELONGS_MEASURE, m_spinLogicalMeasure);
	DDX_Control(pDX, IDC_SPIN_BELONGS_BEAT, m_spinLogicalBeat);
	DDX_Control(pDX, IDC_EDIT_BELONGS_MEASURE, m_editLogicalMeasure);
	DDX_Control(pDX, IDC_EDIT_BELONGS_BEAT, m_editLogicalBeat);
	DDX_Control(pDX, IDC_CHECK_CONTROLLING, m_checkControlling);
	DDX_Control(pDX, IDC_RADIO_TYPE_PRIMARY, m_radioPrimary);
	DDX_Control(pDX, IDC_RADIO_TYPE_SECONDARY, m_radioSecondary);
	DDX_Control(pDX, IDC_RADIO_MOTIF, m_radioMotif);
	DDX_Control(pDX, IDC_RADIO_SEGMENT, m_radioSegment);
	DDX_Control(pDX, IDC_FILE_PROMPT, m_staticFilePrompt);
	DDX_Control(pDX, IDC_MOTIF_PROMPT, m_staticMotifPrompt);
	DDX_Control(pDX, IDC_COMBO_FILE, m_comboFile);
	DDX_Control(pDX, IDC_COMBO_MOTIF, m_comboMotif);
	DDX_Control(pDX, IDC_EDIT_TICK, m_editTick);
	DDX_Control(pDX, IDC_EDIT_BEAT, m_editBeat);
	DDX_Control(pDX, IDC_EDIT_MEASURE, m_editMeasure);
	DDX_Control(pDX, IDC_SPIN_TICK, m_spinTick);
	DDX_Control(pDX, IDC_SPIN_BEAT, m_spinBeat);
	DDX_Control(pDX, IDC_SPIN_MEASURE, m_spinMeasure);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTabFileRef, CPropertyPage)
	//{{AFX_MSG_MAP(CTabFileRef)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_TICK, OnDeltaposSpinTick)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_BEAT, OnDeltaposSpinBeat)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_MEASURE, OnDeltaposSpinMeasure)
	ON_EN_KILLFOCUS(IDC_EDIT_TICK, OnKillfocusEditTick)
	ON_EN_KILLFOCUS(IDC_EDIT_BEAT, OnKillfocusEditBeat)
	ON_EN_KILLFOCUS(IDC_EDIT_MEASURE, OnKillfocusEditMeasure)
	ON_CBN_DROPDOWN(IDC_COMBO_FILE, OnDropDownComboFile)
	ON_CBN_SELCHANGE(IDC_COMBO_FILE, OnSelChangeComboFile)
	ON_CBN_DROPDOWN(IDC_COMBO_MOTIF, OnDropDownComboMotif)
	ON_CBN_SELCHANGE(IDC_COMBO_MOTIF, OnSelChangeComboMotif)
	ON_BN_CLICKED(IDC_RADIO_MOTIF, OnRadioMotif)
	ON_BN_DOUBLECLICKED(IDC_RADIO_MOTIF, OnDoubleClickedRadioMotif)
	ON_BN_CLICKED(IDC_RADIO_SEGMENT, OnRadioSegment)
	ON_BN_DOUBLECLICKED(IDC_RADIO_SEGMENT, OnDoubleClickedRadioSegment)
	ON_BN_CLICKED(IDC_CHECK_CONTROLLING, OnControlling)
	ON_BN_DOUBLECLICKED(IDC_CHECK_CONTROLLING, OnDoubleClickedControlling)
	ON_BN_CLICKED(IDC_RADIO_TYPE_PRIMARY, OnRadioTypePrimary)
	ON_BN_DOUBLECLICKED(IDC_RADIO_TYPE_PRIMARY, OnDoubleClickedRadioTypePrimary)
	ON_BN_CLICKED(IDC_RADIO_TYPE_SECONDARY, OnRadioTypeSecondary)
	ON_BN_DOUBLECLICKED(IDC_RADIO_TYPE_SECONDARY, OnDoubleClickedRadioTypeSecondary)
	ON_WM_KILLFOCUS()
	ON_EN_KILLFOCUS(IDC_EDIT_BELONGS_BEAT, OnKillFocusEditLogicalBeat)
	ON_EN_KILLFOCUS(IDC_EDIT_BELONGS_MEASURE, OnKillFocusEditLogicalMeasure)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_BELONGS_BEAT, OnDeltaPosSpinLogicalBeat)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_BELONGS_MEASURE, OnDeltaPosSpinLogicalMeasure)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTabFileRef custom functions

/////////////////////////////////////////////////////////////////////////////
// CTabFileRef::SetTrackItem

void CTabFileRef::SetTrackItem( const CTrackItem* pItem )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// If the pointer is null, there are no items selected, so disable the property page
	if( pItem == NULL )
	{
		m_fValidTrackItem = FALSE;
		m_TrackItem.Clear();
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
	m_TrackItem.Copy( pItem );

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

		// If the item is not in the last measure
		if( m_TrackItem.m_lMeasure < lMaxMeasure )
		{
			// Convert from a Measure value to a clock value
			long lClockForMeasure;
			m_pTimeline->MeasureBeatToClocks( dwGroupBits, 0, m_TrackItem.m_lMeasure, 0, &lClockForMeasure );

			// Get the TimeSig for this measure
			DMUS_TIMESIGNATURE TimeSig;
			if( SUCCEEDED( m_pTimeline->GetParam( GUID_TimeSignature, dwGroupBits, 0, lClockForMeasure, NULL, &TimeSig ) ) )
			{
				lMaxBeat = TimeSig.bBeatsPerMeasure - 1;
			}
		}
		// Else the item is in the last measure and lMaxBeat is already set

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
		m_comboFile.ResetContent();
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

	// Enable all the controls
	EnableControls( TRUE );

	CString strText;
	if( m_TrackItem.m_dwFlagsDM & DMUS_SEGMENTTRACKF_MOTIF )
	{
		// Motif
		strText.LoadString( IDS_STYLE_PROMPT_TEXT );
		m_staticFilePrompt.SetWindowText( strText );
		m_radioMotif.SetCheck( 1 );
		m_radioSegment.SetCheck( 0 );
	}
	else
	{
		// Segment
		strText.LoadString( IDS_SEGMENT_PROMPT_TEXT );
		m_staticFilePrompt.SetWindowText( strText );
		m_radioMotif.SetCheck( 0 );
		m_radioSegment.SetCheck( 1 );
	}

	// Set File combo box selection
	m_fIgnoreSelChange = TRUE;
	SetFileComboBoxSelection();
	m_fIgnoreSelChange = FALSE;

	// Set Motif combo box selection
	m_fIgnoreSelChange = TRUE;
	SetMotifComboBoxSelection();
	m_fIgnoreSelChange = FALSE;

	// Set UI for Primary, Secondary, Controlling flags
	m_radioPrimary.SetCheck( 0 );
	m_radioSecondary.SetCheck( 0 );
	m_checkControlling.SetCheck( 0 );
	if( m_TrackItem.m_dwPlayFlagsDM & DMUS_SEGF_CONTROL )
	{
		m_radioSecondary.SetCheck( 1 );
		m_checkControlling.SetCheck( 1 );
	}
	else if( m_TrackItem.m_dwPlayFlagsDM & DMUS_SEGF_SECONDARY )
	{
		m_radioSecondary.SetCheck( 1 );
	}
	else
	{
		m_radioPrimary.SetCheck( 1 );
	}

	TCHAR tcstrTmp[DIALOG_EDIT_LEN];
	long lCurValue;
	BOOL fTransSuccess;

	// MEASURE
	{
		// Get text from measure edit control
		m_editMeasure.GetWindowText( tcstrTmp, DIALOG_EDIT_LEN );

		// Convert from text to an integer
		lCurValue = GetDlgItemInt( m_editMeasure.GetDlgCtrlID(), &fTransSuccess, TRUE );

		// Check if the conversion failed, the text is empty, or if the values are different
		if( !fTransSuccess
		|| (tcstrTmp[0] == NULL)
		|| (lCurValue != m_TrackItem.m_lMeasure + 1) )
		{
			// Update the displayed measure number
			SetDlgItemInt( m_editMeasure.GetDlgCtrlID(), m_TrackItem.m_lMeasure + 1 );
		}
	}

	// BEAT
	{
		// Get text from beat edit control
		m_editBeat.GetWindowText( tcstrTmp, DIALOG_EDIT_LEN );

		// Convert from text to an integer
		lCurValue = GetDlgItemInt( m_editBeat.GetDlgCtrlID(), &fTransSuccess, TRUE );

		// Check if the conversion failed, the text is empty, or if the values are different
		if( !fTransSuccess
		|| (tcstrTmp[0] == NULL)
		|| (lCurValue != m_TrackItem.m_lBeat + 1) )
		{
			// Update the displayed beat number
			SetDlgItemInt( m_editBeat.GetDlgCtrlID(), m_TrackItem.m_lBeat + 1 );
		}
	}

	// TICK
	{
		// Get text from tick edit control
		m_editTick.GetWindowText( tcstrTmp, DIALOG_EDIT_LEN );

		// Convert from text to an integer
		lCurValue = GetDlgItemInt( m_editTick.GetDlgCtrlID(), &fTransSuccess, TRUE );

		// Check if the conversion failed, the text is empty, or if the values are different
		if( !fTransSuccess
		|| (tcstrTmp[0] == NULL)
		|| (lCurValue != m_TrackItem.m_lTick) )
		{
			// Update the displayed tick number
			SetDlgItemInt( m_editTick.GetDlgCtrlID(), m_TrackItem.m_lTick );
		}
	}

	if( m_pPropPageMgr
	&&  m_pPropPageMgr->m_pTrackMgr
	&&  m_pPropPageMgr->m_pTrackMgr->IsRefTimeTrack() == false )
	{
		// LOGICAL MEASURE
		{
			// Get text from logical measure edit control
			m_editLogicalMeasure.GetWindowText( tcstrTmp, DIALOG_EDIT_LEN );

			// Convert from text to an integer
			lCurValue = GetDlgItemInt( m_editLogicalMeasure.GetDlgCtrlID(), &fTransSuccess, TRUE );

			// Check if the conversion failed, the text is empty, or if the values are different
			if( !fTransSuccess
			|| (tcstrTmp[0] == NULL)
			|| (lCurValue != m_TrackItem.m_lLogicalMeasure + 1) )
			{
				// Update the displayed logical measure number
				SetDlgItemInt( m_editLogicalMeasure.GetDlgCtrlID(), m_TrackItem.m_lLogicalMeasure + 1 );
			}
		}

		// LOGICAL BEAT
		{
			// Get text from logical beat edit control
			m_editLogicalBeat.GetWindowText( tcstrTmp, DIALOG_EDIT_LEN );

			// Convert from text to an integer
			lCurValue = GetDlgItemInt( m_editLogicalBeat.GetDlgCtrlID(), &fTransSuccess, TRUE );

			// Check if the conversion failed, the text is empty, or if the values are different
			if( !fTransSuccess
			|| (tcstrTmp[0] == NULL)
			|| (lCurValue != m_TrackItem.m_lLogicalBeat + 1) )
			{
				// Update the displayed logical beat number
				SetDlgItemInt( m_editLogicalBeat.GetDlgCtrlID(), m_TrackItem.m_lLogicalBeat + 1 );
			}
		}
	}

	// Set control ranges
	m_spinMeasure.SetRange( 1, lMaxMeasure );
	m_spinBeat.SetRange( 1, lMaxBeat );
	m_spinTick.SetRange( -MAX_TICK, MAX_TICK );
	m_spinLogicalMeasure.SetRange( 1, lMaxMeasure );
	m_spinLogicalBeat.SetRange( 1, lMaxBeat );
}


/////////////////////////////////////////////////////////////////////////////
// CTabFileRef::SetFileComboBoxSelection

void CTabFileRef::SetFileComboBoxSelection( void ) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	ASSERT( m_pPropPageMgr->m_pIFramework != NULL );

	IDMUSProdProject* pIProject = NULL;
	CString strText;
	int nMatch;
	int nCount;
	int nCurPos;
	int nPos = -1;

	// Make sure multiple items were not selected
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

	// Create a FileListInfo struct for the current file
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

	// Select the file in the combo box list
	nMatch = CB_ERR;
	nCount = m_comboFile.GetCount();
	for( nCurPos = 0 ;  nCurPos < nCount ;  nCurPos++ )
	{
		FileListInfo* pCurFileListInfo = (FileListInfo *)m_comboFile.GetItemDataPtr( nCurPos );
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

		// Delete old FileInfo list
		while( !m_lstFileListInfo.IsEmpty() )
		{
			pSLI = static_cast<FileListInfo*>( m_lstFileListInfo.RemoveHead() );
			delete pSLI;
		}

		// Remove old list from combo box
		m_comboFile.ResetContent();

		// Add this file to the combo box list
		nPos = 0;
		m_lstFileListInfo.AddTail( pFileListInfo );
		InsertFileInfoListInComboBox();
	}
	else
	{
		nPos = nMatch;
		delete pFileListInfo;
		pFileListInfo = NULL;
	}

LEAVE:
	m_comboFile.SetCurSel( nPos );

	if( pIProject )
	{
		pIProject->Release();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabFileRef::GetFileComboBoxText

void CTabFileRef::GetFileComboBoxText( const FileListInfo* pFileListInfo, CString& strText )
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
// CTabFileRef::InsertFileInfoListInComboBox

void CTabFileRef::InsertFileInfoListInComboBox( void ) 
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
			GetFileComboBoxText( pFileListInfo, strText );
			nPos = m_comboFile.AddString( strText );
			if( nPos >= 0 )
			{
				m_comboFile.SetItemDataPtr( nPos, pFileListInfo );
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabFileRef::InsertFileInfo

void CTabFileRef::InsertFileInfo( FileListInfo* pFileListInfo )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	FileListInfo* pListInfo;
	CString strListText;
	CString strFileText;
	POSITION posList;

	GetFileComboBoxText( pFileListInfo, strFileText );

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

			GetFileComboBoxText( pListInfo, strListText );

			if( strListText.CompareNoCase( strFileText ) > 0 )
			{
				m_lstFileListInfo.InsertBefore( posList, pFileListInfo );
				return;
			}
		}
	}

	m_lstFileListInfo.AddTail( pFileListInfo );
}


/////////////////////////////////////////////////////////////////////////////
// CTabFileRef::BuildFileInfoList

void CTabFileRef::BuildFileInfoList( void ) 
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

	HRESULT hr;

	// Determine whether we are looking for styles or segments
	if( m_TrackItem.m_dwFlagsDM & DMUS_SEGMENTTRACKF_MOTIF )
	{
		hr = m_pPropPageMgr->m_pIFramework->FindDocTypeByNodeId( GUID_StyleNode, &pIDocType );
	}
	else
	{
		hr = m_pPropPageMgr->m_pIFramework->FindDocTypeByNodeId( GUID_SegmentNode, &pIDocType );
	}
	if( FAILED ( hr ) )
	{
		return;
	}

	// Build the list of files
	hr = m_pPropPageMgr->m_pIFramework->GetFirstProject( &pINextProject );

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
					if( SUCCEEDED ( pIProject->GetName( &bstrProjectName ) ) )
					{
						pFileListInfo->strProjectName = bstrProjectName;
						::SysFreeString( bstrProjectName );
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

				InsertFileInfo( pFileListInfo );

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
// CTabFileRef::FillMotifComboBox

void CTabFileRef::FillMotifComboBox( void ) 
{
	// Remove old list from combo box
	m_comboMotif.ResetContent();

	// Make sure multiple Scripts were not selected
	ASSERT( !(m_TrackItem.m_dwBitsUI & UD_MULTIPLESELECT) );
	if( m_TrackItem.m_dwBitsUI & UD_MULTIPLESELECT )
	{
		return;
	}

	// Make sure we have a DocRoot node
	if( m_TrackItem.m_FileRef.pIDocRootNode == NULL )
	{
		return;
	}

	IDirectMusicStyle* pIDMStyle;
	CString strMotifName;
	WCHAR awchMotifName[MAX_PATH];

	// If we have a style, place all of its motif names in the combo box
	if( SUCCEEDED ( m_TrackItem.m_FileRef.pIDocRootNode->GetObject( CLSID_DirectMusicStyle, IID_IDirectMusicStyle, (void**)&pIDMStyle ) ) )
	{
		int i = 0;
		while( pIDMStyle->EnumMotif( i++, awchMotifName ) == S_OK )
		{
			strMotifName = awchMotifName;
			m_comboMotif.AddString( strMotifName );
		}

		pIDMStyle->Release();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabFileRef::SetMotifComboBoxSelection

void CTabFileRef::SetMotifComboBoxSelection( void ) 
{
	int nPos = -1;

	// Make sure multiple Scripts were not selected
	ASSERT( !(m_TrackItem.m_dwBitsUI & UD_MULTIPLESELECT) );
	if( m_TrackItem.m_dwBitsUI & UD_MULTIPLESELECT )
	{
		goto LEAVE;
	}

	// Rebuild the Motif list
	FillMotifComboBox();

	// Check if we're even choosing a Motif
	if( m_TrackItem.m_dwFlagsDM & DMUS_SEGMENTTRACKF_MOTIF )
	{
		// Select the Motif in the combo box list
		nPos = m_comboMotif.FindStringExact( -1, m_TrackItem.m_strMotif );
		if( nPos == CB_ERR )
		{
			// If there is no style chosen
			if( m_TrackItem.m_FileRef.pIDocRootNode == NULL )
			{
				// Display "<Select Style Above>"
				CString strSelectStyleAbove;
				if( strSelectStyleAbove.LoadString( IDS_SELECT_STYLE_ABOVE ) )
				{
					// Add existing text to combo box list
					nPos = m_comboMotif.AddString( strSelectStyleAbove );
				}
			}
			else
			{
				// Add existing motif name to combo box list
				nPos = m_comboMotif.AddString( m_TrackItem.m_strMotif );
			}
		}
	}

LEAVE:
	m_comboMotif.SetCurSel( nPos );
}


/////////////////////////////////////////////////////////////////////////////
// CTabFileRef message handlers

/////////////////////////////////////////////////////////////////////////////
// CTabFileRef::OnCreate

int CTabFileRef::OnCreate(LPCREATESTRUCT lpCreateStruct) 
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
// CTabFileRef::OnDestroy

void CTabFileRef::OnDestroy() 
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
// CTabFileRef::OnInitDialog

BOOL CTabFileRef::OnInitDialog() 
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

	return TRUE;  // return TRUE unless you set the focus to a control
	               // EXCEPTION: OCX Property Pages should return FALSE
}


/////////////////////////////////////////////////////////////////////////////
// CTabFileRef::OnDeltaposSpinTick

void CTabFileRef::OnDeltaposSpinTick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// Need to do this in case the user clicked the spin control immediately after
	// typing in a value
	OnKillfocusEditTick();

	HandleDeltaChange( pNMHDR, pResult, m_TrackItem.m_lTick );
}


/////////////////////////////////////////////////////////////////////////////
// CTabFileRef::OnDeltaposSpinBeat

void CTabFileRef::OnDeltaposSpinBeat(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// Need to do this in case the user clicked the spin control immediately after
	// typing in a value
	OnKillfocusEditBeat();

	HandleDeltaChange( pNMHDR, pResult, m_TrackItem.m_lBeat );
}


/////////////////////////////////////////////////////////////////////////////
// CTabFileRef::OnDeltaposSpinMeasure

void CTabFileRef::OnDeltaposSpinMeasure(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// Need to do this in case the user clicked the spin control immediately after
	// typing in a value
	OnKillfocusEditMeasure();

	HandleDeltaChange( pNMHDR, pResult, m_TrackItem.m_lMeasure );
}


/////////////////////////////////////////////////////////////////////////////
// CTabFileRef::OnDeltaPosSpinLogicalBeat

void CTabFileRef::OnDeltaPosSpinLogicalBeat(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// Need to do this in case the user clicked the spin control immediately after
	// typing in a value
	OnKillFocusEditLogicalBeat();

	HandleDeltaChange( pNMHDR, pResult, m_TrackItem.m_lLogicalBeat );
}


/////////////////////////////////////////////////////////////////////////////
// CTabFileRef::OnDeltaPosSpinLogicalMeasure

void CTabFileRef::OnDeltaPosSpinLogicalMeasure(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// Need to do this in case the user clicked the spin control immediately after
	// typing in a value
	OnKillFocusEditLogicalMeasure();

	HandleDeltaChange( pNMHDR, pResult, m_TrackItem.m_lLogicalMeasure );
}


/////////////////////////////////////////////////////////////////////////////
// CTabFileRef::HandleDeltaChange
//
// Generic handler for deltapos changes
void CTabFileRef::HandleDeltaChange( NMHDR* pNMHDR,
									   LRESULT* pResult,
									   long& lUpdateVal )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_pPropPageMgr->m_pIPropPageObject == NULL )
	{
		return;
	}

	// If the value changed - update the selected item
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
// CTabFileRef::OnKillfocusEditTick

void CTabFileRef::OnKillfocusEditTick() 
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
		if( m_TrackItem.m_lTick != 0 )
		{
			// Update the item with the new value
			m_TrackItem.m_lTick = 0;

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
			SetDlgItemInt( IDC_EDIT_TICK, m_TrackItem.m_lTick );
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
			if( m_TrackItem.m_lTick != lNewValue )
			{
				// Update the item with the new value
				m_TrackItem.m_lTick = lNewValue;

				// Now, update the object with the new value
				UpdateObject();
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabFileRef::OnKillfocusEditBeat

void CTabFileRef::OnKillfocusEditBeat() 
{
	HandleKillFocus( m_spinBeat, m_TrackItem.m_lBeat );
}

/////////////////////////////////////////////////////////////////////////////
// CTabFileRef::OnKillfocusEditMeasure

void CTabFileRef::OnKillfocusEditMeasure() 
{
	HandleKillFocus( m_spinMeasure, m_TrackItem.m_lMeasure );
}


/////////////////////////////////////////////////////////////////////////////
// CTabFileRef::OnKillFocusEditLogicalBeat

void CTabFileRef::OnKillFocusEditLogicalBeat() 
{
	HandleKillFocus( m_spinLogicalBeat, m_TrackItem.m_lLogicalBeat );
}

/////////////////////////////////////////////////////////////////////////////
// CTabFileRef::OnKillFocusEditLogicalMeasure

void CTabFileRef::OnKillFocusEditLogicalMeasure() 
{
	HandleKillFocus( m_spinLogicalMeasure, m_TrackItem.m_lLogicalMeasure );
}


/////////////////////////////////////////////////////////////////////////////
// CTabFileRef::HandleKillFocus
//
// Generic handler for KillFocus changes
void CTabFileRef::HandleKillFocus( CSpinButtonCtrl& spin, long& lUpdateVal )
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
			// Update the item with the new value
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
				// Update the item with the new value
				lUpdateVal = lNewValue - 1;

				// Now, update the object with the new value
				UpdateObject();
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabFileRef::UpdateObject

void CTabFileRef::UpdateObject( void )
{
	// Check for a valid Property Page Object pointer
	if( m_pPropPageMgr && m_pPropPageMgr->m_pIPropPageObject )
	{
		// Update the Property Page Object with the new item
		m_pPropPageMgr->m_pIPropPageObject->SetData( (void *) &m_TrackItem );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabFileRef::EnableControls

void CTabFileRef::EnableControls( BOOL fEnable )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Check if the Beat edit control exists
	if( m_editBeat.GetSafeHwnd() == NULL )
	{
		return;
	}

	// Update the controls with the new window state
	m_staticFilePrompt.EnableWindow( fEnable );
	m_comboFile.EnableWindow( fEnable );
	m_radioMotif.EnableWindow( fEnable );
	m_radioSegment.EnableWindow( fEnable );
	m_spinTick.EnableWindow( fEnable );
	m_spinBeat.EnableWindow( fEnable );
	m_spinMeasure.EnableWindow( fEnable );
	m_editTick.EnableWindow( fEnable );
	m_editBeat.EnableWindow( fEnable );
	m_editMeasure.EnableWindow( fEnable );
	m_radioPrimary.EnableWindow( fEnable );
	m_radioSecondary.EnableWindow( fEnable );

	BOOL fEnableBelongsTo = FALSE;
	if( m_pPropPageMgr
	&&  m_pPropPageMgr->m_pTrackMgr
	&&  m_pPropPageMgr->m_pTrackMgr->IsRefTimeTrack() == false )
	{
		fEnableBelongsTo = fEnable;
	}

	m_spinLogicalBeat.EnableWindow( fEnableBelongsTo );
	m_spinLogicalMeasure.EnableWindow( fEnableBelongsTo );
	m_editLogicalBeat.EnableWindow( fEnableBelongsTo );
	m_editLogicalMeasure.EnableWindow( fEnableBelongsTo );

	if( !(m_TrackItem.m_dwPlayFlagsDM & DMUS_SEGF_SECONDARY)
	&&  !(m_TrackItem.m_dwPlayFlagsDM & DMUS_SEGF_CONTROL) )
	{
		m_checkControlling.EnableWindow( FALSE );
	}
	else
	{
		m_checkControlling.EnableWindow( fEnable );
	}

	BOOL fEnableMotifCombo = fEnable;
	if( !(m_TrackItem.m_dwFlagsDM & DMUS_SEGMENTTRACKF_MOTIF) )
	{
		fEnableMotifCombo = FALSE;
	}
	m_comboMotif.EnableWindow( fEnableMotifCombo );
	m_staticMotifPrompt.EnableWindow( fEnableMotifCombo );

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
				int nPos = m_comboFile.AddString( strText );
				m_comboFile.SetCurSel( nPos );
//				return;
			}
		}
		else
		{
			m_comboFile.SetCurSel( -1 );
		}

		// No items are selected, so clear the controls
		CString strText;
		strText.LoadString( IDS_SEGMENT_PROMPT_TEXT );
		m_staticFilePrompt.SetWindowText( strText );
		m_comboMotif.SetCurSel( -1 );
		m_radioMotif.SetCheck( 0 );
		m_radioSegment.SetCheck( 0 );
		m_editTick.SetWindowText( NULL );
		m_editBeat.SetWindowText( NULL );
		m_editMeasure.SetWindowText( NULL );
		m_radioPrimary.SetCheck( 0 );
		m_radioSecondary.SetCheck( 0 );
		m_checkControlling.SetCheck( 0 );
	}

	if( fEnableBelongsTo == FALSE )
	{
		m_editLogicalMeasure.SetWindowText( NULL );
		m_editLogicalBeat.SetWindowText( NULL );
	}
}

#define FLAGS_THIS_TAB	(DMUS_SEGF_SECONDARY | DMUS_SEGF_CONTROL)

/////////////////////////////////////////////////////////////////////////////
// CTabFileRef::SetPlayFlags

void CTabFileRef::SetPlayFlags() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Make sure multiple Scripts were not selected
	ASSERT( !(m_TrackItem.m_dwBitsUI & UD_MULTIPLESELECT) );
	if( m_TrackItem.m_dwBitsUI & UD_MULTIPLESELECT )
	{
		return;
	}

	m_TrackItem.m_dwPlayFlagsDM &= ~FLAGS_THIS_TAB;

	if( m_radioPrimary.GetCheck() == FALSE )
	{
		if( m_checkControlling.GetCheck() )
		{
			m_TrackItem.m_dwPlayFlagsDM |= DMUS_SEGF_SECONDARY;
			m_TrackItem.m_dwPlayFlagsDM |= DMUS_SEGF_CONTROL;
		}
		else if( m_radioSecondary.GetCheck() )
		{
			m_TrackItem.m_dwPlayFlagsDM |= DMUS_SEGF_SECONDARY;
		}
	}

	UpdateObject();
}


/////////////////////////////////////////////////////////////////////////////
// CTabFileRef::OnOK

// Windows translated presses of the 'Enter' key into presses of the button 'OK'.
// Here we create a hidden 'OK' button and translate presses of it into tabs to the next
// control in the property page.
void CTabFileRef::OnOK() 
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
// CTabFileRef::OnDropDownComboFile

void CTabFileRef::OnDropDownComboFile() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Delete old FileInfo list
	FileListInfo* pFileListInfo;
	while( !m_lstFileListInfo.IsEmpty() )
	{
		pFileListInfo = static_cast<FileListInfo*>( m_lstFileListInfo.RemoveHead() );
		delete pFileListInfo;
	}

	// Remove old list from combo box
	m_comboFile.ResetContent();

	// Rebuild the FileInfo list
	BuildFileInfoList();
	InsertFileInfoListInComboBox();

	// Select the current file
	SetFileComboBoxSelection();
}


/////////////////////////////////////////////////////////////////////////////
// CTabFileRef::OnSelChangeComboFile

void CTabFileRef::OnSelChangeComboFile() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	ASSERT( m_pPropPageMgr->m_pIFramework != NULL );

	if( m_fIgnoreSelChange )
	{
		return;
	}

	// Make sure multiple Scripts were not selected
	ASSERT( !(m_TrackItem.m_dwBitsUI & UD_MULTIPLESELECT) );
	if( m_TrackItem.m_dwBitsUI & UD_MULTIPLESELECT )
	{
		return;
	}

	int nItem = m_comboFile.GetCurSel();
	if( nItem != CB_ERR )
	{
		FileListInfo* pFileListInfo = (FileListInfo *)m_comboFile.GetItemDataPtr( nItem );
		if( pFileListInfo != (FileListInfo *)-1 )
		{
			// fill in appropriate fields
			m_TrackItem.m_FileRef.li = *pFileListInfo;

			IDMUSProdNode* pIDocRootNode = NULL;
			
			// NOTE: Items must currently be selected to avoid killing
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

				pIDocRootNode->Release();
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabFileRef::OnDropDownComboMotif

void CTabFileRef::OnDropDownComboMotif() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Set Motif combo box selection
	m_fIgnoreSelChange = TRUE;
	SetMotifComboBoxSelection();
	m_fIgnoreSelChange = FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CTabFileRef::OnSelChangeComboMotif

void CTabFileRef::OnSelChangeComboMotif() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_fIgnoreSelChange )
	{
		return;
	}

	// Make sure multiple Scripts were not selected
	ASSERT( !(m_TrackItem.m_dwBitsUI & UD_MULTIPLESELECT) );
	if( m_TrackItem.m_dwBitsUI & UD_MULTIPLESELECT )
	{
		return;
	}

	int nItem = m_comboMotif.GetCurSel();
	if( nItem != CB_ERR )
	{
		TCHAR achText[MAX_PATH];

		if( m_comboMotif.GetLBText(nItem, achText) != CB_ERR )
		{
			m_TrackItem.m_strMotif = achText;
			UpdateObject();
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabFileRef::OnRadioMotif

void CTabFileRef::OnRadioMotif() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Make sure multiple Scripts were not selected
	ASSERT( !(m_TrackItem.m_dwBitsUI & UD_MULTIPLESELECT) );
	if( m_TrackItem.m_dwBitsUI & UD_MULTIPLESELECT )
	{
		return;
	}

	if( m_radioMotif.GetCheck() )
	{
		m_TrackItem.m_dwFlagsDM |= DMUS_SEGMENTTRACKF_MOTIF;
	}
	else
	{
		m_TrackItem.m_dwFlagsDM &= ~DMUS_SEGMENTTRACKF_MOTIF;
	}

	UpdateObject();
}


/////////////////////////////////////////////////////////////////////////////
// CTabFileRef::OnDoubleClickedRadioMotif

void CTabFileRef::OnDoubleClickedRadioMotif() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	OnRadioMotif();
}


/////////////////////////////////////////////////////////////////////////////
// CTabFileRef::OnRadioSegment

void CTabFileRef::OnRadioSegment() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	OnRadioMotif();
}


/////////////////////////////////////////////////////////////////////////////
// CTabFileRef::OnDoubleClickedRadioSegment

void CTabFileRef::OnDoubleClickedRadioSegment() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	OnRadioMotif();
}


/////////////////////////////////////////////////////////////////////////////
// CTabFileRef::OnControlling

void CTabFileRef::OnControlling() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	SetPlayFlags();
}


/////////////////////////////////////////////////////////////////////////////
// CTabFileRef::OnDoubleClickedControlling

void CTabFileRef::OnDoubleClickedControlling() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	SetPlayFlags();
}


/////////////////////////////////////////////////////////////////////////////
// CTabFileRef::OnRadioTypePrimary

void CTabFileRef::OnRadioTypePrimary() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	SetPlayFlags();
}


/////////////////////////////////////////////////////////////////////////////
// CTabFileRef::OnDoubleClickedRadioTypePrimary

void CTabFileRef::OnDoubleClickedRadioTypePrimary() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	SetPlayFlags();
}


/////////////////////////////////////////////////////////////////////////////
// CTabFileRef::OnRadioTypeSecondary

void CTabFileRef::OnRadioTypeSecondary() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	SetPlayFlags();
}


/////////////////////////////////////////////////////////////////////////////
// CTabFileRef::OnDoubleClickedRadioTypeSecondary

void CTabFileRef::OnDoubleClickedRadioTypeSecondary() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	SetPlayFlags();
}


/////////////////////////////////////////////////////////////////////////////
// CTabFileRef::OnSetActive

BOOL CTabFileRef::OnSetActive() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Refresh the page
	if(	m_fValidTrackItem )
	{
		SetTrackItem( &m_TrackItem );
	}
	else
	{
		SetTrackItem( NULL );
	}

	if( m_pPropPageMgr 
	&&  m_pPropPageMgr->m_pTrackMgr 
	&&  m_pPropPageMgr->m_pIPropSheet )
	{
		if( m_pPropPageMgr->m_pTrackMgr->m_fGetActiveTab )
		{
			m_pPropPageMgr->m_pIPropSheet->GetActivePage( &CPropPageMgrItem::sm_nActiveTab );
		}
	}
	
	return CPropertyPage::OnSetActive();
}
