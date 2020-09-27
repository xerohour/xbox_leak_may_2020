// TabFileRef.cpp : implementation file
//

#include "stdafx.h"
#include <RiffStrm.h>
#include "TrackMgr.h"
#include "TrackItem.h"
#include "PropTrackItem.h"
#include "PropPageMgr_Item.h"
#include "TabFileRef.h"
#include "TabLoop.h"
#include "TabPerformance.h"
#include "TabVariations.h"
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

// {D9D1D754-B61A-4216-991C-7571D5BEEC22}
static const GUID GUID_WaveItemPPGMgr = 
{ 0xd9d1d754, 0xb61a, 0x4216, { 0x99, 0x1c, 0x75, 0x71, 0xd5, 0xbe, 0xec, 0x22 } };

short CPropPageMgrItem::sm_nActiveTab = 0;

static const TCHAR g_pstrUnknown[11] = _T("----------");

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
	m_GUIDManager = GUID_WaveItemPPGMgr;

	// Initialize our pointers to NULL
	m_pTabFileRef = NULL;
	m_pTabLoop = NULL;
	m_pTabPerformance = NULL;
	m_pTabVariations = NULL;

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

	if( m_pTabLoop )
	{
		delete m_pTabLoop;
		m_pTabLoop = NULL;
	}

	if( m_pTabPerformance )
	{
		delete m_pTabPerformance;
		m_pTabPerformance = NULL;
	}

	if( m_pTabVariations )
	{
		delete m_pTabVariations;
		m_pTabVariations = NULL;
	}

	RELEASE( m_pIFramework );

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

	// Create FileRef tab
	if( m_pTabFileRef == NULL )
	{
		m_pTabFileRef = new CTabFileRef();
	}
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

	// Create Loop tab
	if( m_pTabLoop == NULL )
	{
		m_pTabLoop = new CTabLoop();
	}
	if( m_pTabLoop )
	{
		// Copy the PROPSHEETPAGE structure
		PROPSHEETPAGE psp;
		memcpy( &psp, &m_pTabLoop->m_psp, sizeof(PROPSHEETPAGE) );

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
		m_pTabLoop->m_pPropPageMgr = this;
	}

	// Create Performance tab
	if( m_pTabPerformance == NULL )
	{
		m_pTabPerformance = new CTabPerformance();
	}
	if( m_pTabPerformance )
	{
		// Copy the PROPSHEETPAGE structure
		PROPSHEETPAGE psp;
		memcpy( &psp, &m_pTabPerformance->m_psp, sizeof(PROPSHEETPAGE) );

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
		m_pTabPerformance->m_pPropPageMgr = this;
	}

	// Create Variations tab
	if( m_pTabVariations == NULL )
	{
		m_pTabVariations = new CTabVariations();
	}
	if( m_pTabVariations )
	{
		// Copy the PROPSHEETPAGE structure
		PROPSHEETPAGE psp;
		memcpy( &psp, &m_pTabVariations->m_psp, sizeof(PROPSHEETPAGE) );

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
		m_pTabVariations->m_pPropPageMgr = this;
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
	CPropTrackItem  PropTrackItem;
	CPropTrackItem* pPropItem = &PropTrackItem;

	if( m_pIPropPageObject == NULL )
	{
		// No property page object - set pointer to NULL
		pPropItem = NULL;
	}
	// Have a property page object - try and get a pointer from it
	else if( FAILED ( m_pIPropPageObject->GetData( (void **)&pPropItem ) ) )
	{
		return E_FAIL;
	}

	// Update the property pages, if they exists
	if( m_pTabFileRef )
	{
		m_pTabFileRef->SetPropTrackItem( pPropItem );
	}
	if( m_pTabLoop )
	{
		m_pTabLoop->SetPropTrackItem( pPropItem );
	}
	if( m_pTabPerformance )
	{
		m_pTabPerformance->SetPropTrackItem( pPropItem );
	}
	if( m_pTabVariations )
	{
		m_pTabVariations->SetPropTrackItem( pPropItem );
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
	m_pIWaveTimelineDraw = NULL;
	m_pPropPageMgr = NULL;

	// Initialize our class members to FALSE
	m_fValidTrackItem = FALSE;
	m_fNeedToDetach = FALSE;
	m_fIgnoreSelChange = FALSE;

	m_lStartMeasure = 0;
	m_lStartBeat = 0;
	m_lStartGrid = 0;
	m_lStartTick = 0;

	m_lEndMeasure = 0;
	m_lEndBeat = 0;
	m_lEndGrid = 0;
	m_lEndTick = 0;
	
	m_lLengthMeasure = 0;
	m_lLengthBeat = 0;
	m_lLengthGrid = 0;
	m_lLengthTick = 0;
	m_dwLengthNbrSamples = 0;
	
	m_lOffsetMeasure = 0;
	m_lOffsetBeat = 0;
	m_lOffsetGrid = 0;
	m_lOffsetTick = 0;
	m_dwOffsetNbrSamples = 0;

	m_lSourceEndMeasure = 0;
	m_lSourceEndBeat = 0;
	m_lSourceEndGrid = 0;
	m_lSourceEndTick = 0;
	
	m_lSourceLengthMeasure = 0;
	m_lSourceLengthBeat = 0;
	m_lSourceLengthGrid = 0;
	m_lSourceLengthTick = 0;
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

	RELEASE( m_pIWaveTimelineDraw );
}


/////////////////////////////////////////////////////////////////////////////
// CTabFileRef::DoDataExchange

void CTabFileRef::DoDataExchange(CDataExchange* pDX)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	CPropertyPage::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(CTabFileRef)
	DDX_Control(pDX, IDC_LOCK_LENGTH, m_btnLockLength);
	DDX_Control(pDX, IDC_LOCK_END, m_btnLockEnd);
	DDX_Control(pDX, IDC_COMBO_FILE, m_comboFile);
	DDX_Control(pDX, IDC_PROMPT_GRID, m_staticPromptGrid);
	DDX_Control(pDX, IDC_PROMPT_TICK, m_staticPromptTick);
	DDX_Control(pDX, IDC_PROMPT_BEAT, m_staticPromptBeat);
	DDX_Control(pDX, IDC_PROMPT_BAR, m_staticPromptBar);
	DDX_Control(pDX, IDC_EDIT_START_TICK, m_editStartTick);
	DDX_Control(pDX, IDC_EDIT_START_GRID, m_editStartGrid);
	DDX_Control(pDX, IDC_EDIT_START_BEAT, m_editStartBeat);
	DDX_Control(pDX, IDC_EDIT_START_BAR, m_editStartMeasure);
	DDX_Control(pDX, IDC_EDIT_END_TICK, m_editEndTick);
	DDX_Control(pDX, IDC_EDIT_END_GRID, m_editEndGrid);
	DDX_Control(pDX, IDC_EDIT_END_BEAT, m_editEndBeat);
	DDX_Control(pDX, IDC_EDIT_END_BAR, m_editEndMeasure);
	DDX_Control(pDX, IDC_EDIT_LENGTH_TICK, m_editLengthTick);
	DDX_Control(pDX, IDC_EDIT_LENGTH_GRID, m_editLengthGrid);
	DDX_Control(pDX, IDC_EDIT_LENGTH_BEAT, m_editLengthBeat);
	DDX_Control(pDX, IDC_EDIT_LENGTH_BAR, m_editLengthMeasure);
	DDX_Control(pDX, IDC_STATIC_LENGTH_NBR_SAMPLES, m_staticLengthNbrSamples);
	DDX_Control(pDX, IDC_EDIT_OFFSET_TICK, m_editOffsetTick);
	DDX_Control(pDX, IDC_EDIT_OFFSET_GRID, m_editOffsetGrid);
	DDX_Control(pDX, IDC_EDIT_OFFSET_BEAT, m_editOffsetBeat);
	DDX_Control(pDX, IDC_EDIT_OFFSET_BAR, m_editOffsetMeasure);
	DDX_Control(pDX, IDC_STATIC_OFFSET_NBR_SAMPLES, m_staticOffsetNbrSamples);
	DDX_Control(pDX, IDC_SPIN_START_TICK, m_spinStartTick);
	DDX_Control(pDX, IDC_SPIN_START_GRID, m_spinStartGrid);
	DDX_Control(pDX, IDC_SPIN_START_BEAT, m_spinStartBeat);
	DDX_Control(pDX, IDC_SPIN_START_BAR, m_spinStartMeasure);
	DDX_Control(pDX, IDC_SPIN_END_TICK, m_spinEndTick);
	DDX_Control(pDX, IDC_SPIN_END_GRID, m_spinEndGrid);
	DDX_Control(pDX, IDC_SPIN_END_BEAT, m_spinEndBeat);
	DDX_Control(pDX, IDC_SPIN_END_BAR, m_spinEndMeasure);
	DDX_Control(pDX, IDC_SPIN_LENGTH_TICK, m_spinLengthTick);
	DDX_Control(pDX, IDC_SPIN_LENGTH_GRID, m_spinLengthGrid);
	DDX_Control(pDX, IDC_SPIN_LENGTH_BEAT, m_spinLengthBeat);
	DDX_Control(pDX, IDC_SPIN_LENGTH_BAR, m_spinLengthMeasure);
	DDX_Control(pDX, IDC_SPIN_OFFSET_TICK, m_spinOffsetTick);
	DDX_Control(pDX, IDC_SPIN_OFFSET_GRID, m_spinOffsetGrid);
	DDX_Control(pDX, IDC_SPIN_OFFSET_BEAT, m_spinOffsetBeat);
	DDX_Control(pDX, IDC_SPIN_OFFSET_BAR, m_spinOffsetMeasure);
	DDX_Control(pDX, IDC_STATIC_SOURCE_END_TICK, m_staticSourceEndTick);
	DDX_Control(pDX, IDC_STATIC_SOURCE_END_GRID, m_staticSourceEndGrid);
	DDX_Control(pDX, IDC_STATIC_SOURCE_END_BEAT, m_staticSourceEndBeat);
	DDX_Control(pDX, IDC_STATIC_SOURCE_END_BAR, m_staticSourceEndMeasure);
	DDX_Control(pDX, IDC_STATIC_SOURCE_LENGTH_TICK, m_staticSourceLengthTick);
	DDX_Control(pDX, IDC_STATIC_SOURCE_LENGTH_GRID, m_staticSourceLengthGrid);
	DDX_Control(pDX, IDC_STATIC_SOURCE_LENGTH_BEAT, m_staticSourceLengthBeat);
	DDX_Control(pDX, IDC_STATIC_SOURCE_LENGTH_BAR, m_staticSourceLengthMeasure);
	DDX_Control(pDX, IDC_STATIC_SOURCE_LENGTH_NBR_SAMPLES, m_staticSourceLengthNbrSamples);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTabFileRef, CPropertyPage)
	//{{AFX_MSG_MAP(CTabFileRef)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_START_TICK, OnSpinStartTick)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_START_GRID, OnSpinStartGrid)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_START_BEAT, OnSpinStartBeat)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_START_BAR, OnSpinStartMeasure)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_END_TICK, OnSpinEndTick)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_END_GRID, OnSpinEndGrid)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_END_BEAT, OnSpinEndBeat)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_END_BAR, OnSpinEndMeasure)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_LENGTH_TICK, OnSpinLengthTick)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_LENGTH_GRID, OnSpinLengthGrid)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_LENGTH_BEAT, OnSpinLengthBeat)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_LENGTH_BAR, OnSpinLengthMeasure)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_OFFSET_TICK, OnSpinOffsetTick)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_OFFSET_GRID, OnSpinOffsetGrid)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_OFFSET_BEAT, OnSpinOffsetBeat)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_OFFSET_BAR, OnSpinOffsetMeasure)
	ON_EN_KILLFOCUS(IDC_EDIT_START_TICK, OnKillFocusStartTick)
	ON_EN_KILLFOCUS(IDC_EDIT_START_GRID, OnKillFocusStartGrid)
	ON_EN_KILLFOCUS(IDC_EDIT_START_BEAT, OnKillFocusStartBeat)
	ON_EN_KILLFOCUS(IDC_EDIT_START_BAR, OnKillFocusStartMeasure)
	ON_EN_KILLFOCUS(IDC_EDIT_END_TICK, OnKillFocusEndTick)
	ON_EN_KILLFOCUS(IDC_EDIT_END_GRID, OnKillFocusEndGrid)
	ON_EN_KILLFOCUS(IDC_EDIT_END_BEAT, OnKillFocusEndBeat)
	ON_EN_KILLFOCUS(IDC_EDIT_END_BAR, OnKillFocusEndMeasure)
	ON_EN_KILLFOCUS(IDC_EDIT_LENGTH_TICK, OnKillFocusLengthTick)
	ON_EN_KILLFOCUS(IDC_EDIT_LENGTH_GRID, OnKillFocusLengthGrid)
	ON_EN_KILLFOCUS(IDC_EDIT_LENGTH_BEAT, OnKillFocusLengthBeat)
	ON_EN_KILLFOCUS(IDC_EDIT_LENGTH_BAR, OnKillFocusLengthMeasure)
	ON_EN_KILLFOCUS(IDC_EDIT_OFFSET_TICK, OnKillFocusOffsetTick)
	ON_EN_KILLFOCUS(IDC_EDIT_OFFSET_GRID, OnKillFocusOffsetGrid)
	ON_EN_KILLFOCUS(IDC_EDIT_OFFSET_BEAT, OnKillFocusOffsetBeat)
	ON_EN_KILLFOCUS(IDC_EDIT_OFFSET_BAR, OnKillFocusOffsetMeasure)
	ON_CBN_DROPDOWN(IDC_COMBO_FILE, OnDropDownComboFile)
	ON_CBN_SELCHANGE(IDC_COMBO_FILE, OnSelChangeComboFile)
	ON_WM_KILLFOCUS()
	ON_BN_CLICKED(IDC_LOCK_END, OnLockEnd)
	ON_BN_DOUBLECLICKED(IDC_LOCK_END, OnDoubleClickedLockEnd)
	ON_BN_CLICKED(IDC_LOCK_LENGTH, OnLockLength)
	ON_BN_DOUBLECLICKED(IDC_LOCK_LENGTH, OnDoubleClickedLockLength)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTabFileRef custom functions

/////////////////////////////////////////////////////////////////////////////
// CTabFileRef::SetPropTrackItem

void CTabFileRef::SetPropTrackItem( const CPropTrackItem* pPropItem )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// If the pointer is null, there are no items selected, so disable the property page
	if( pPropItem == NULL )
	{
		m_fValidTrackItem = FALSE;
		m_PropItem.Clear();
		EnableControls( FALSE );
		return;
	}

	// Store IDMUSProdWaveTimelineDraw interface pointer
	RELEASE( m_pIWaveTimelineDraw );
	if( pPropItem->m_Item.m_FileRef.pIDocRootNode )
	{
		if( FAILED ( pPropItem->m_Item.m_FileRef.pIDocRootNode->QueryInterface( IID_IDMUSProdWaveTimelineDraw, (void **)&m_pIWaveTimelineDraw ) ) )
		{
			// Cannot update properties without this interface
			m_fValidTrackItem = FALSE;
			m_PropItem.Clear();
			EnableControls( FALSE );
			return;
		}
	}

	// Flag that we have a valid item
	m_fValidTrackItem = TRUE;

	// Copy the information from the new item
	m_PropItem.Copy( pPropItem );
	m_PropItem.m_nPropertyTab = TAB_FILEREF;
	m_PropItem.m_dwChanged = 0;
	if( m_PropItem.m_Item.m_FileRef.pIDocRootNode )
	{
		REFERENCE_TIME rtActualEnd;
		REFERENCE_TIME rtTimePhysical;
		m_pPropPageMgr->m_pTrackMgr->UnknownTimeToRefTime( m_PropItem.m_Item.m_rtTimePhysical + m_PropItem.m_Item.m_rtDuration, &rtActualEnd );
		m_pPropPageMgr->m_pTrackMgr->UnknownTimeToRefTime( m_PropItem.m_Item.m_rtTimePhysical, &rtTimePhysical );

		REFERENCE_TIME rtDuration = rtActualEnd - rtTimePhysical;
		rtDuration -= m_PropItem.m_Item.PitchAdjustedStartOffset();

		REFERENCE_TIME rtMinTime = MinTimeToUnknownTime();
		m_pIWaveTimelineDraw->RefTimeToSample( max(rtMinTime, rtDuration), &m_dwLengthNbrSamples, m_PropItem.m_Item.m_lPitch );
	}

	// Compute values for controls displaying "time"
	// Determine measure, beat, grid, tick
	// OR
	// Determine minute, second, grid, millisecond
	RecomputeTimes();

	// Check if the edit control has a valid window handle.
	if( m_editStartBeat.GetSafeHwnd() == NULL )
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

	// Check if wave length is less than minimum loop length
	ASSERT( m_PropItem.m_Item.m_WaveInfo.dwWaveEnd >= (MIN_LOOP_LENGTH + 1) );
//	if( m_PropItem.m_Item.m_WaveInfo.dwWaveEnd < (MIN_LOOP_LENGTH + 1) )
//	{
//		// Just disable duration controls???????
//		EnableControls( FALSE );
//		return;
//	}

	// Update the property page based on the new data.
	// It is usually profitable to check to see if the data actually changed
	// before updating the controls in the property page.

	// Enable all the controls
	EnableControls( TRUE );

	// Set File combo box selection
	m_fIgnoreSelChange = TRUE;
	SetFileComboBoxSelection();
	m_fIgnoreSelChange = FALSE;

	// Check if multiple items are selected
	if( m_PropItem.m_dwBits & PROPF_MULTIPLESELECT )
	{
		// For now don't set value of other controls when multiple waves selected
		return;
	}

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

	// Set GRID controls
	CWaveStrip* pWaveStrip = m_PropItem.m_Item.GetWaveStripForPropSheet();
	if( pWaveStrip
	&&  pWaveStrip->IsOneGrid() )
	{
		m_editStartGrid.SetWindowText( NULL );
		m_editStartGrid.EnableWindow( FALSE );
		m_editEndGrid.SetWindowText( NULL );
		m_editEndGrid.EnableWindow( FALSE );
		m_editLengthGrid.SetWindowText( NULL );
		m_editLengthGrid.EnableWindow( FALSE );
		m_editOffsetGrid.SetWindowText( NULL );
		m_editOffsetGrid.EnableWindow( FALSE );
	}
	else
	{
		SetEditControl( m_editStartGrid, m_lStartGrid, 1 );
		if( m_PropItem.m_Item.m_FileRef.pIDocRootNode )
		{
			SetEditControl( m_editEndGrid, m_lEndGrid, 1 );
			SetEditControl( m_editLengthGrid, m_lLengthGrid, 0 );
			SetEditControl( m_editOffsetGrid, m_lOffsetGrid, 0 );
		}
		else
		{
			m_editEndGrid.SetWindowText( NULL );
			m_editLengthGrid.SetWindowText( NULL );
			m_editOffsetGrid.SetWindowText( NULL );
		}
	}
	
	// Update SOURCE controls
	if( m_PropItem.m_Item.m_FileRef.pIDocRootNode )
	{
		// Set GRID controls
		if( pWaveStrip
		&&  pWaveStrip->IsOneGrid() )
		{
			m_staticSourceEndGrid.SetWindowText( NULL );
			m_staticSourceLengthGrid.SetWindowText( NULL );
		}
		else
		{
			SetStaticControl( m_staticSourceEndGrid, m_lSourceEndGrid, 1 );
			SetStaticControl( m_staticSourceLengthGrid, m_lSourceLengthGrid, 0 );
		}

		// Update END controls
		SetStaticControl( m_staticSourceEndMeasure, m_lSourceEndMeasure, nMeasureBeatOffset );
		SetStaticControl( m_staticSourceEndBeat, m_lSourceEndBeat, nMeasureBeatOffset );
		SetStaticControl( m_staticSourceEndTick, m_lSourceEndTick, 0 );

		// Update LENGTH controls
		SetStaticControl( m_staticSourceLengthMeasure, m_lSourceLengthMeasure, 0 );
		SetStaticControl( m_staticSourceLengthBeat, m_lSourceLengthBeat, 0 );
		SetStaticControl( m_staticSourceLengthTick, m_lSourceLengthTick, 0 );
		SetStaticControl( m_staticSourceLengthNbrSamples, (m_PropItem.m_Item.m_WaveInfo.dwWaveEnd + 1), 0 );
	}
	else
	{
		// Update END controls
		m_staticSourceEndMeasure.SetWindowText( NULL );
		m_staticSourceEndBeat.SetWindowText( NULL );
		m_staticSourceEndGrid.SetWindowText( NULL );
		m_staticSourceEndTick.SetWindowText( NULL );

		// Update LENGTH controls
		m_staticSourceLengthMeasure.SetWindowText( NULL );
		m_staticSourceLengthBeat.SetWindowText( NULL );
		m_staticSourceLengthGrid.SetWindowText( NULL );
		m_staticSourceLengthTick.SetWindowText( NULL );
		m_staticSourceLengthNbrSamples.SetWindowText( NULL );
	}

	// Update START controls
	SetEditControl( m_editStartMeasure, m_lStartMeasure, nMeasureBeatOffset );
	SetEditControl( m_editStartBeat, m_lStartBeat, nMeasureBeatOffset );
	SetEditControl( m_editStartTick, m_lStartTick, 0 );

	if( m_PropItem.m_Item.m_FileRef.pIDocRootNode )
	{
		// Update END controls
		SetEditControl( m_editEndMeasure, m_lEndMeasure, nMeasureBeatOffset );
		SetEditControl( m_editEndBeat, m_lEndBeat, nMeasureBeatOffset );
		SetEditControl( m_editEndTick, m_lEndTick, 0 );

		// Update LENGTH controls
		SetEditControl( m_editLengthMeasure, m_lLengthMeasure, 0 );
		SetEditControl( m_editLengthBeat, m_lLengthBeat, 0 );
		SetEditControl( m_editLengthTick, m_lLengthTick, 0 );
		SetStaticControl( m_staticLengthNbrSamples, m_dwLengthNbrSamples, 0 );

		// Update OFFSET controls
		SetEditControl( m_editOffsetMeasure, m_lOffsetMeasure, 0 );
		SetEditControl( m_editOffsetBeat, m_lOffsetBeat, 0 );
		SetEditControl( m_editOffsetTick, m_lOffsetTick, 0 );
		SetStaticControl( m_staticOffsetNbrSamples, m_dwOffsetNbrSamples, 0 );
	}
	else
	{
		// Update END controls
		m_editEndTick.SetWindowText( NULL );
		m_editEndBeat.SetWindowText( NULL );
		m_editEndMeasure.SetWindowText( NULL );

		// Update LENGTH controls
		m_editLengthTick.SetWindowText( NULL );
		m_editLengthBeat.SetWindowText( NULL );
		m_editLengthMeasure.SetWindowText( NULL );
		m_staticLengthNbrSamples.SetWindowText( NULL );

		// Update OFFSET controls
		m_editOffsetTick.SetWindowText( NULL );
		m_editOffsetBeat.SetWindowText( NULL );
		m_editOffsetMeasure.SetWindowText( NULL );
		m_staticOffsetNbrSamples.SetWindowText( NULL );

	}

	// Update LOCK END controls
	if( m_pPropPageMgr
	&&  m_pPropPageMgr->m_pTrackMgr )
	{
		CBitmap* pBitmap = m_pPropPageMgr->m_pTrackMgr->GetLockBitmap( m_PropItem.m_Item.m_fLockEndUI );
		if( pBitmap )
		{
			m_btnLockEnd.SetBitmap( (HBITMAP)pBitmap->GetSafeHandle() );
		}
	}
	if( m_PropItem.m_Item.m_fLockEndUI )
	{
		m_editEndMeasure.EnableWindow( FALSE );
		m_spinEndMeasure.EnableWindow( FALSE );
		m_editEndBeat.EnableWindow( FALSE );
		m_spinEndBeat.EnableWindow( FALSE );
		m_editEndGrid.EnableWindow( FALSE );
		m_spinEndGrid.EnableWindow( FALSE );
		m_editEndTick.EnableWindow( FALSE );
		m_spinEndTick.EnableWindow( FALSE );
	}

	// Update LOCK LENGTH controls
	if( m_pPropPageMgr
	&&  m_pPropPageMgr->m_pTrackMgr )
	{
		CBitmap* pBitmap = m_pPropPageMgr->m_pTrackMgr->GetLockBitmap( m_PropItem.m_Item.m_fLockLengthUI );
		if( pBitmap )
		{
			m_btnLockLength.SetBitmap( (HBITMAP)pBitmap->GetSafeHandle() );
		}
	}
	if( m_PropItem.m_Item.m_fLockLengthUI )
	{
		m_editLengthMeasure.EnableWindow( FALSE );
		m_spinLengthMeasure.EnableWindow( FALSE );
		m_editLengthBeat.EnableWindow( FALSE );
		m_spinLengthBeat.EnableWindow( FALSE );
		m_editLengthGrid.EnableWindow( FALSE );
		m_spinLengthGrid.EnableWindow( FALSE );
		m_editLengthTick.EnableWindow( FALSE );
		m_spinLengthTick.EnableWindow( FALSE );
	}

	// Set ranges of spin controls
	SetControlRanges();
}
	

/////////////////////////////////////////////////////////////////////////////
// CTabFileRef::SetEditControl

void CTabFileRef::SetEditControl( CEdit& edit, long lValue, int nOffset )
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
// CTabFileRef::SetStaticControl

void CTabFileRef::SetStaticControl( CStatic& stat, long lValue, int nOffset )
{
	// Update the displayed number
	SetDlgItemInt( stat.GetDlgCtrlID(), lValue + nOffset );
}


/////////////////////////////////////////////////////////////////////////////
// CTabFileRef::SetStart

void CTabFileRef::SetStart( REFERENCE_TIME rtNewStart ) 
{
	REFERENCE_TIME rtSegmentLength = m_pPropPageMgr->m_pTrackMgr->GetSegmentLength();
	REFERENCE_TIME rtMinTime = MinTimeToUnknownTime();
	REFERENCE_TIME rtCurEnd = EndToUnknownTime();
	REFERENCE_TIME rtCurLength = LengthToUnknownTime();

	REFERENCE_TIME rtPitchAdjustedStartOffset; 
	m_PropItem.m_Item.StartOffsetToUnknownTime( m_pPropPageMgr->m_pTrackMgr, &rtPitchAdjustedStartOffset );
	REFERENCE_TIME rtMinLength = rtMinTime + rtPitchAdjustedStartOffset;

	REFERENCE_TIME rtActualLength;
	m_PropItem.m_Item.SourceWaveLengthToUnknownTime( m_pPropPageMgr->m_pTrackMgr, &rtActualLength );

	REFERENCE_TIME rtMinStart;
//	m_pPropPageMgr->m_pTrackMgr->ClocksToUnknownTime( -MAX_TICK, &rtMinStart );
	m_pPropPageMgr->m_pTrackMgr->ClocksToUnknownTime( 0, &rtMinStart );

	rtNewStart = max( rtNewStart, rtMinStart );
	rtNewStart = min( rtNewStart, rtSegmentLength - rtMinTime );
	
	REFERENCE_TIME rtNewLength;
	if( m_PropItem.m_Item.m_fLockLengthUI )
	{
		rtNewLength = rtCurLength;
	}
	else if( m_PropItem.m_Item.m_fLockEndUI )
	{
		rtNewLength = rtCurEnd - rtNewStart;
		if( rtNewLength < rtMinLength )
		{
			rtNewLength = rtMinLength;
			rtNewStart = rtCurEnd - rtMinLength;
		}
        // No good reason to clamp this. It only causes trouble if there are pitch bends.
/*		if( m_PropItem.m_Item.m_fLoopedUI == FALSE )
		{
			if( rtNewLength > rtActualLength )
			{
				rtNewLength = rtActualLength;
				rtNewStart = rtCurEnd - rtActualLength;
			}
		}*/
	}
	else
	{
		rtNewLength = rtCurEnd - rtNewStart;
	}

	rtNewLength = max( rtNewLength, rtMinLength );
    // No good reason to clamp this. It only causes trouble if there are pitch bends.
/*	if( m_PropItem.m_Item.m_fLoopedUI == FALSE )
	{
		rtNewLength = min( rtNewLength, rtActualLength );
	}*/

	if( (m_PropItem.m_dwUndetermined_TabFileRef & UNDT_TIME_PHYSICAL)
	||  (m_PropItem.m_dwUndetermined_TabFileRef & UNDT_DURATION)
	||  (m_PropItem.m_Item.m_rtTimePhysical != rtNewStart)
	||  (m_PropItem.m_Item.m_rtDuration != rtNewLength) )
	{
		m_PropItem.m_Item.m_rtTimePhysical = rtNewStart;
		m_PropItem.m_Item.m_rtDuration = rtNewLength;

		// Now, update the object with the new value(s)
		m_PropItem.m_dwChanged = CHGD_TIME_OR_DURATION;
		UpdateObject();
	}
	else
	{
		// Display original values
		SetPropTrackItem( &m_PropItem );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabFileRef::SetEnd

void CTabFileRef::SetEnd( REFERENCE_TIME rtNewEnd ) 
{
	ASSERT( m_PropItem.m_Item.m_fLockEndUI == FALSE );

	REFERENCE_TIME rtSegmentLength = m_pPropPageMgr->m_pTrackMgr->GetSegmentLength();
	REFERENCE_TIME rtMinTime = MinTimeToUnknownTime();
	REFERENCE_TIME rtCurStart = StartToUnknownTime();
	REFERENCE_TIME rtCurLength = LengthToUnknownTime();

	REFERENCE_TIME rtPitchAdjustedStartOffset; 
	m_PropItem.m_Item.StartOffsetToUnknownTime( m_pPropPageMgr->m_pTrackMgr, &rtPitchAdjustedStartOffset );
	REFERENCE_TIME rtMinLength = rtMinTime + rtPitchAdjustedStartOffset;

	REFERENCE_TIME rtActualLength;
	m_PropItem.m_Item.SourceWaveLengthToUnknownTime( m_pPropPageMgr->m_pTrackMgr, &rtActualLength );

	REFERENCE_TIME rtMinStart;
//	m_pPropPageMgr->m_pTrackMgr->ClocksToUnknownTime( -MAX_TICK, &rtMinStart );
	m_pPropPageMgr->m_pTrackMgr->ClocksToUnknownTime( 0, &rtMinStart );
	
	REFERENCE_TIME rtNewStart;
	REFERENCE_TIME rtNewLength;
	if( m_PropItem.m_Item.m_fLockLengthUI )
	{
		rtNewLength = rtCurLength;
		rtNewStart = rtNewEnd - rtCurLength;
		rtNewStart = max( rtNewStart, rtMinStart );
		rtNewStart = min( rtNewStart, rtSegmentLength - rtMinTime );
	}
	else
	{
		rtNewLength = rtNewEnd - rtCurStart;
		rtNewStart = rtCurStart;
	}

	rtNewLength = max( rtNewLength, rtMinLength );
    // No good reason to clamp this. It only causes trouble if there are pitch bends.
/*	if( m_PropItem.m_Item.m_fLoopedUI == FALSE )
	{
		rtNewLength = min( rtNewLength, rtActualLength );
	}*/

	if( (m_PropItem.m_dwUndetermined_TabFileRef & UNDT_TIME_PHYSICAL)
	||  (m_PropItem.m_dwUndetermined_TabFileRef & UNDT_DURATION)
	||  (m_PropItem.m_Item.m_rtTimePhysical != rtNewStart)
	||  (m_PropItem.m_Item.m_rtDuration != rtNewLength) )
	{
		m_PropItem.m_Item.m_rtTimePhysical = rtNewStart;
		m_PropItem.m_Item.m_rtDuration = rtNewLength;

		// Now, update the object with the new value(s)
		m_PropItem.m_dwChanged = CHGD_TIME_OR_DURATION;
		UpdateObject();
	}
	else
	{
		// Display original values
		SetPropTrackItem( &m_PropItem );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabFileRef::SetLength

void CTabFileRef::SetLength( REFERENCE_TIME rtNewLength ) 
{
	ASSERT( m_PropItem.m_Item.m_fLockLengthUI == FALSE );

	REFERENCE_TIME rtSegmentLength = m_pPropPageMgr->m_pTrackMgr->GetSegmentLength();
	REFERENCE_TIME rtMinTime = MinTimeToUnknownTime();
	REFERENCE_TIME rtCurStart = StartToUnknownTime();
	REFERENCE_TIME rtCurEnd = EndToUnknownTime();

	REFERENCE_TIME rtPitchAdjustedStartOffset; 
	m_PropItem.m_Item.StartOffsetToUnknownTime( m_pPropPageMgr->m_pTrackMgr, &rtPitchAdjustedStartOffset );
	REFERENCE_TIME rtMinLength = rtMinTime + rtPitchAdjustedStartOffset;

	REFERENCE_TIME rtActualLength;
	m_PropItem.m_Item.SourceWaveLengthToUnknownTime( m_pPropPageMgr->m_pTrackMgr, &rtActualLength );

	REFERENCE_TIME rtMinStart;
//	m_pPropPageMgr->m_pTrackMgr->ClocksToUnknownTime( -MAX_TICK, &rtMinStart );
	m_pPropPageMgr->m_pTrackMgr->ClocksToUnknownTime( 0, &rtMinStart );
	
	REFERENCE_TIME rtNewStart;
	if( m_PropItem.m_Item.m_fLockEndUI )
	{
		if( rtNewLength < rtMinLength )
		{
			rtNewLength = rtMinLength;
		}
        // No good reason to clamp this. It only causes trouble if there are pitch bends.
/*		if( m_PropItem.m_Item.m_fLoopedUI == FALSE )
		{
			if( rtNewLength > rtActualLength )
			{
				rtNewLength = rtActualLength;
			}
		}*/
		rtNewStart = rtCurEnd - rtNewLength;

		if( rtNewStart < rtMinStart )
		{
			rtNewLength = rtNewLength - (rtMinStart - rtNewStart);
			rtNewStart = rtMinStart;
		}
		rtNewStart = min( rtNewStart, rtSegmentLength - rtMinTime );
	}
	else
	{
		rtNewStart = rtCurStart;
	}

	rtNewLength = max( rtNewLength, rtMinLength );
    // No good reason to clamp this. It only causes trouble if there are pitch bends.
/*	if( m_PropItem.m_Item.m_fLoopedUI == FALSE )
	{
		rtNewLength = min( rtNewLength, rtActualLength );
	}*/

	if( (m_PropItem.m_dwUndetermined_TabFileRef & UNDT_TIME_PHYSICAL)
	||  (m_PropItem.m_dwUndetermined_TabFileRef & UNDT_DURATION)
	||  (m_PropItem.m_Item.m_rtTimePhysical != rtNewStart)
	||  (m_PropItem.m_Item.m_rtDuration != rtNewLength) )
	{
		m_PropItem.m_Item.m_rtTimePhysical = rtNewStart;
		m_PropItem.m_Item.m_rtDuration = rtNewLength;

		// Now, update the object with the new value(s)
		m_PropItem.m_dwChanged = CHGD_TIME_OR_DURATION;
		UpdateObject();
	}
	else
	{
		// Display original values
		SetPropTrackItem( &m_PropItem );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabFileRef::RemoveFineTune

REFERENCE_TIME CTabFileRef::RemoveFineTune( REFERENCE_TIME rtTime )
{
	if( rtTime
	&&  m_PropItem.m_Item.m_lPitch )
	{
		if( m_PropItem.m_Item.m_FileRef.pIDocRootNode )
		{
			IDMUSProdWaveTimelineDraw* pIWaveTimelineDraw;
			if( SUCCEEDED ( m_PropItem.m_Item.m_FileRef.pIDocRootNode->QueryInterface( IID_IDMUSProdWaveTimelineDraw, (void **)&pIWaveTimelineDraw ) ) )
			{
				REFERENCE_TIME rtAdjustedTime;
				DWORD dwSample;

				if( SUCCEEDED ( pIWaveTimelineDraw->RefTimeToSample( rtTime, &dwSample, m_PropItem.m_Item.m_lPitch ) )
				&&  SUCCEEDED ( pIWaveTimelineDraw->SampleToRefTime( dwSample, &rtAdjustedTime, 0 ) ) )
				{
					RELEASE( pIWaveTimelineDraw );
					return rtAdjustedTime;
				}

				RELEASE( pIWaveTimelineDraw );
			}
		}
	}

	return rtTime;
}


/////////////////////////////////////////////////////////////////////////////
// CTabFileRef::SetOffset

void CTabFileRef::SetOffset( REFERENCE_TIME rtNewOffset ) 
{
	// Determine minimum
	REFERENCE_TIME rtMinTime = MinTimeToRefTime();

	// Determine maximum - which is length of source wave without fine tune
	// (or duration without fine tune - whichever is less)
	REFERENCE_TIME rtLengthOfSourceWave;
	if( m_PropItem.m_Item.m_FileRef.pIDocRootNode )
	{
		ASSERT( m_pIWaveTimelineDraw != NULL );
		m_pIWaveTimelineDraw->SampleToRefTime( (m_PropItem.m_Item.m_WaveInfo.dwWaveEnd + 1), &rtLengthOfSourceWave, 0 );
	}
	else
	{
		ASSERT( 0 );	// Should not happen!
		rtLengthOfSourceWave = m_PropItem.m_Item.m_WaveInfo.rtWaveLength;
	}
	REFERENCE_TIME rtDuration;
	m_pPropPageMgr->m_pTrackMgr->UnknownTimeToRefTime( m_PropItem.m_Item.m_rtDuration, &rtDuration );
	REFERENCE_TIME rtMaxTime = min( rtLengthOfSourceWave, rtDuration );
	
	rtNewOffset = RemoveFineTune( rtNewOffset );
	rtNewOffset = max( rtNewOffset, 0 );
	rtNewOffset = min( rtNewOffset, rtMaxTime - rtMinTime );

	if( (m_PropItem.m_dwUndetermined_TabFileRef & UNDT_START_OFFSET)
	||  (m_PropItem.m_Item.m_rtStartOffset != rtNewOffset) )
	{
		m_PropItem.m_Item.m_rtStartOffset = rtNewOffset;

		// Now, update the object with the new value(s)
		m_PropItem.m_dwChanged = CHGD_START_OFFSET;
		UpdateObject();
	}
	else
	{
		// Display original values
		SetPropTrackItem( &m_PropItem );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabFileRef::StartToUnknownTime

REFERENCE_TIME CTabFileRef::StartToUnknownTime( void )
{
	REFERENCE_TIME rtNewStart;

	if( m_pPropPageMgr->m_pTrackMgr->IsRefTimeTrack() )
	{
		REFERENCE_TIME rtTime;
		m_pPropPageMgr->m_pTrackMgr->MinSecGridMsToRefTime( m_PropItem.m_Item.GetWaveStripForPropSheet(),
															m_lStartMeasure, m_lStartBeat, m_lStartGrid, m_lStartTick,
															&rtTime );
		m_pPropPageMgr->m_pTrackMgr->RefTimeToUnknownTime( rtTime, &rtNewStart );
	}
	else
	{
		MUSIC_TIME mtTime;
		m_pPropPageMgr->m_pTrackMgr->MeasureBeatGridTickToClocks( m_lStartMeasure, m_lStartBeat, m_lStartGrid, m_lStartTick, &mtTime );
		m_pPropPageMgr->m_pTrackMgr->ClocksToUnknownTime( mtTime, &rtNewStart );
	}

	return rtNewStart;
}


/////////////////////////////////////////////////////////////////////////////
// CTabFileRef::EndToUnknownTime

REFERENCE_TIME CTabFileRef::EndToUnknownTime( void )
{
	REFERENCE_TIME rtNewEnd;

	REFERENCE_TIME rtPitchAdjustedStartOffset; 
	m_PropItem.m_Item.StartOffsetToUnknownTime( m_pPropPageMgr->m_pTrackMgr, &rtPitchAdjustedStartOffset );

	if( m_pPropPageMgr->m_pTrackMgr->IsRefTimeTrack() )
	{
		REFERENCE_TIME rtTime;
		m_pPropPageMgr->m_pTrackMgr->MinSecGridMsToRefTime( m_PropItem.m_Item.GetWaveStripForPropSheet(),
															m_lEndMeasure, m_lEndBeat, m_lEndGrid, m_lEndTick,
															&rtTime );
		rtTime += rtPitchAdjustedStartOffset;
		m_pPropPageMgr->m_pTrackMgr->RefTimeToUnknownTime( rtTime, &rtNewEnd );
	}
	else
	{
		MUSIC_TIME mtTime;
		m_pPropPageMgr->m_pTrackMgr->MeasureBeatGridTickToClocks( m_lEndMeasure, m_lEndBeat, m_lEndGrid, m_lEndTick, &mtTime );
		mtTime += (MUSIC_TIME)rtPitchAdjustedStartOffset;
		m_pPropPageMgr->m_pTrackMgr->ClocksToUnknownTime( mtTime, &rtNewEnd );
	}

	return rtNewEnd;
}


/////////////////////////////////////////////////////////////////////////////
// CTabFileRef::LengthToUnknownTime

REFERENCE_TIME CTabFileRef::LengthToUnknownTime( void )
{
	REFERENCE_TIME rtNewLength;

	REFERENCE_TIME rtPitchAdjustedStartOffset; 
	m_PropItem.m_Item.StartOffsetToUnknownTime( m_pPropPageMgr->m_pTrackMgr, &rtPitchAdjustedStartOffset );

	if( m_pPropPageMgr->m_pTrackMgr->IsRefTimeTrack() )
	{
		REFERENCE_TIME rtTime;
		m_pPropPageMgr->m_pTrackMgr->MinSecGridMsToRefTime( m_PropItem.m_Item.GetWaveStripForPropSheet(),
															m_lLengthMeasure, m_lLengthBeat, m_lLengthGrid, m_lLengthTick,
															&rtTime );
		rtTime += rtPitchAdjustedStartOffset;
		m_pPropPageMgr->m_pTrackMgr->RefTimeToUnknownTime( rtTime, &rtNewLength );
	}
	else
	{
		MUSIC_TIME mtTime;
		m_pPropPageMgr->m_pTrackMgr->MeasureBeatGridTickToClocks( m_lLengthMeasure, m_lLengthBeat, m_lLengthGrid, m_lLengthTick, &mtTime );
		mtTime += (MUSIC_TIME)rtPitchAdjustedStartOffset;
		m_pPropPageMgr->m_pTrackMgr->ClocksToUnknownTime( mtTime, &rtNewLength );
	}

	return rtNewLength;
}


/////////////////////////////////////////////////////////////////////////////
// CTabFileRef::OffsetToRefTime

REFERENCE_TIME CTabFileRef::OffsetToRefTime( void )
{
	REFERENCE_TIME rtNewOffset;

	if( m_pPropPageMgr->m_pTrackMgr->IsRefTimeTrack() )
	{
		m_pPropPageMgr->m_pTrackMgr->MinSecGridMsToRefTime( m_PropItem.m_Item.GetWaveStripForPropSheet(),
															m_lOffsetMeasure, m_lOffsetBeat, m_lOffsetGrid, m_lOffsetTick,
															&rtNewOffset );
	}
	else
	{
		REFERENCE_TIME rtTimePhysical;
		MUSIC_TIME mtTime;
		m_pPropPageMgr->m_pTrackMgr->MeasureBeatGridTickToClocks( m_lStartMeasure + m_lOffsetMeasure,
																  m_lStartBeat + m_lOffsetBeat,
																  m_lStartGrid + m_lOffsetGrid,
																  m_lStartTick + m_lOffsetTick,
																  &mtTime );
		m_pPropPageMgr->m_pTrackMgr->m_pTimeline->ClocksToRefTime( mtTime, &rtNewOffset );
		m_pPropPageMgr->m_pTrackMgr->m_pTimeline->ClocksToRefTime( (MUSIC_TIME)m_PropItem.m_Item.m_rtTimePhysical, &rtTimePhysical );
		rtNewOffset -= rtTimePhysical;
	}

	return rtNewOffset;
}


/////////////////////////////////////////////////////////////////////////////
// CTabFileRef::MinTimeToUnknownTime

REFERENCE_TIME CTabFileRef::MinTimeToUnknownTime( void )
{
	REFERENCE_TIME rtMinTime;

	if( m_pPropPageMgr->m_pTrackMgr->IsRefTimeTrack() )
	{
		m_pPropPageMgr->m_pTrackMgr->MinSecGridMsToRefTime( m_PropItem.m_Item.GetWaveStripForPropSheet(),
															0, 0, 0, 1, &rtMinTime );
	}
	else
	{
		MUSIC_TIME mtMinTime;
		m_pPropPageMgr->m_pTrackMgr->MeasureBeatGridTickToClocks( 0, 0, 0, 1, &mtMinTime );
		rtMinTime = mtMinTime;
	}

	return rtMinTime;
}


/////////////////////////////////////////////////////////////////////////////
// CTabFileRef::MinTimeToRefTime

REFERENCE_TIME CTabFileRef::MinTimeToRefTime( void )
{
	REFERENCE_TIME rtMinTime;

	if( m_pPropPageMgr->m_pTrackMgr->IsRefTimeTrack() )
	{
		m_pPropPageMgr->m_pTrackMgr->MinSecGridMsToRefTime( m_PropItem.m_Item.GetWaveStripForPropSheet(),
															0, 0, 0, 1, &rtMinTime );
	}
	else
	{
		MUSIC_TIME mtMinTime;
		m_pPropPageMgr->m_pTrackMgr->MeasureBeatGridTickToClocks( 0, 0, 0, 1, &mtMinTime );
		m_pPropPageMgr->m_pTrackMgr->m_pTimeline->ClocksToRefTime( mtMinTime, &rtMinTime );
	}

	return rtMinTime;
}


/////////////////////////////////////////////////////////////////////////////
// CTabFileRef::RecomputeTimes

void CTabFileRef::RecomputeTimes( void )
{
	if( m_pPropPageMgr->m_pTrackMgr->IsRefTimeTrack() )
	{
		ComputeMinSecGridMs();
	}
	else
	{
		ComputeBarBeatGridTick();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabFileRef::ComputeMinSecGridMs

void CTabFileRef::ComputeMinSecGridMs( void )
{
	// START
	m_pPropPageMgr->m_pTrackMgr->RefTimeToMinSecGridMs( m_PropItem.m_Item.GetWaveStripForPropSheet(),
														m_PropItem.m_Item.m_rtTimePhysical,
														&m_lStartMeasure, &m_lStartBeat, &m_lStartGrid, &m_lStartTick );
	

	if( m_PropItem.m_Item.m_FileRef.pIDocRootNode )
	{
		ASSERT( m_pIWaveTimelineDraw != NULL );
		
		REFERENCE_TIME rtDuration;
		rtDuration = m_PropItem.m_Item.m_rtDuration - m_PropItem.m_Item.PitchAdjustedStartOffset();

		// END
		m_pPropPageMgr->m_pTrackMgr->RefTimeToMinSecGridMs( m_PropItem.m_Item.GetWaveStripForPropSheet(),
															m_PropItem.m_Item.m_rtTimePhysical + rtDuration,
															&m_lEndMeasure, &m_lEndBeat, &m_lEndGrid, &m_lEndTick );
		

		// LENGTH
		m_pPropPageMgr->m_pTrackMgr->RefTimeToMinSecGridMs( m_PropItem.m_Item.GetWaveStripForPropSheet(),
															rtDuration,
															&m_lLengthMeasure, &m_lLengthBeat, &m_lLengthGrid, &m_lLengthTick );

		// OFFSET
		m_pPropPageMgr->m_pTrackMgr->RefTimeToMinSecGridMs( m_PropItem.m_Item.GetWaveStripForPropSheet(),
															m_PropItem.m_Item.PitchAdjustedStartOffset(),
															&m_lOffsetMeasure, &m_lOffsetBeat, &m_lOffsetGrid, &m_lOffsetTick );
		// m_PropItem.m_Item.m_rtStartOffset is not affected by m_PropItem.m_Item.m_lPitch
		m_pIWaveTimelineDraw->RefTimeToSample( max(0, m_PropItem.m_Item.m_rtStartOffset), &m_dwOffsetNbrSamples, 0 );
		

		// SOURCE END
		m_pPropPageMgr->m_pTrackMgr->RefTimeToMinSecGridMs( m_PropItem.m_Item.GetWaveStripForPropSheet(),
															m_PropItem.m_Item.m_rtTimePhysical + m_PropItem.m_Item.m_WaveInfo.rtWaveLength,
															&m_lSourceEndMeasure, &m_lSourceEndBeat, &m_lSourceEndGrid, &m_lSourceEndTick );
		

		// SOURCE LENGTH
		REFERENCE_TIME rtTime;
		m_pIWaveTimelineDraw->SampleToRefTime( (m_PropItem.m_Item.m_WaveInfo.dwWaveEnd + 1), &rtTime, m_PropItem.m_Item.m_lPitch );
		m_pPropPageMgr->m_pTrackMgr->RefTimeToMinSecGridMs( m_PropItem.m_Item.GetWaveStripForPropSheet(),
															rtTime,
															&m_lSourceLengthMeasure, &m_lSourceLengthBeat, &m_lSourceLengthGrid, &m_lSourceLengthTick );
	}
	else
	{
		m_lEndMeasure = 0;
		m_lEndBeat = 0;
		m_lEndGrid = 0;
		m_lEndTick = 0;

		m_lLengthMeasure = 0;
		m_lLengthBeat = 0;
		m_lLengthGrid = 0;
		m_lLengthTick = 0;
		m_dwLengthNbrSamples = 0;

		m_lOffsetMeasure = 0;
		m_lOffsetBeat = 0;
		m_lOffsetGrid = 0;
		m_lOffsetTick = 0;
		m_dwOffsetNbrSamples = 0;

		m_lSourceEndMeasure = 0;
		m_lSourceEndBeat = 0;
		m_lSourceEndGrid = 0;
		m_lSourceEndTick = 0;

		m_lSourceLengthMeasure = 0;
		m_lSourceLengthBeat = 0;
		m_lSourceLengthGrid = 0;
		m_lSourceLengthTick = 0;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabFileRef::ComputeBarBeatGridTick

void CTabFileRef::ComputeBarBeatGridTick( void )
{
	// START
	m_pPropPageMgr->m_pTrackMgr->ClocksToMeasureBeatGridTick( (MUSIC_TIME)m_PropItem.m_Item.m_rtTimePhysical,
															 &m_lStartMeasure, &m_lStartBeat, &m_lStartGrid, &m_lStartTick );

	if( m_PropItem.m_Item.m_FileRef.pIDocRootNode )
	{
		ASSERT( m_pIWaveTimelineDraw != NULL );

		REFERENCE_TIME rtTime;
		MUSIC_TIME mtTime;

		REFERENCE_TIME rtPitchAdjustedStartOffset; 
		m_PropItem.m_Item.StartOffsetToUnknownTime( m_pPropPageMgr->m_pTrackMgr, &rtPitchAdjustedStartOffset );

		MUSIC_TIME mtDuration;
		mtDuration = (MUSIC_TIME)m_PropItem.m_Item.m_rtDuration - (MUSIC_TIME)rtPitchAdjustedStartOffset;

		// END
		m_pPropPageMgr->m_pTrackMgr->ClocksToMeasureBeatGridTick( (MUSIC_TIME)m_PropItem.m_Item.m_rtTimePhysical + mtDuration,
																  &m_lEndMeasure, &m_lEndBeat, &m_lEndGrid, &m_lEndTick );

		// LENGTH
		m_pPropPageMgr->m_pTrackMgr->ClocksToMeasureBeatGridTick( mtDuration,
																 &m_lLengthMeasure, &m_lLengthBeat, &m_lLengthGrid, &m_lLengthTick );

		// OFFSET
		{
			MUSIC_TIME mtTime;
			MUSIC_TIME mtOffsetTime = (MUSIC_TIME)m_PropItem.m_Item.m_rtTimePhysical + (MUSIC_TIME)rtPitchAdjustedStartOffset;

			m_lOffsetMeasure = 0;
			m_lOffsetBeat = 0;
			m_lOffsetGrid = 0;
			m_lOffsetTick = 0;

			do
			{
				m_lOffsetMeasure++;
				m_pPropPageMgr->m_pTrackMgr->MeasureBeatGridTickToClocks( m_lStartMeasure + m_lOffsetMeasure,
																		  m_lStartBeat,
																		  m_lStartGrid,
																		  m_lStartTick,
																		  &mtTime );
			}
			while( mtTime <= mtOffsetTime ); 
			m_lOffsetMeasure--;

			do
			{
				m_lOffsetBeat++;
				m_pPropPageMgr->m_pTrackMgr->MeasureBeatGridTickToClocks( m_lStartMeasure + m_lOffsetMeasure,
																		  m_lStartBeat + m_lOffsetBeat,
																		  m_lStartGrid,
																		  m_lStartTick,
																		  &mtTime );
			}
			while( mtTime <= mtOffsetTime ); 
			m_lOffsetBeat--;

			do
			{
				m_lOffsetGrid++;
				m_pPropPageMgr->m_pTrackMgr->MeasureBeatGridTickToClocks( m_lStartMeasure + m_lOffsetMeasure,
																		  m_lStartBeat + m_lOffsetBeat,
																		  m_lStartGrid + m_lOffsetGrid,
																		  m_lStartTick,
																		  &mtTime );
			}
			while( mtTime <= mtOffsetTime ); 
			m_lOffsetGrid--;

			m_pPropPageMgr->m_pTrackMgr->MeasureBeatGridTickToClocks( m_lStartMeasure + m_lOffsetMeasure,
																	  m_lStartBeat + m_lOffsetBeat,
																	  m_lStartGrid + m_lOffsetGrid,
																	  m_lStartTick,
																	  &mtTime );
			m_lOffsetTick = mtOffsetTime - mtTime;
		}

		// m_PropItem.m_Item.m_rtStartOffset is not affected by m_PropItem.m_Item.m_lPitch
		m_pIWaveTimelineDraw->RefTimeToSample( max(0, m_PropItem.m_Item.m_rtStartOffset), &m_dwOffsetNbrSamples, 0 );

		// SOURCE END
		REFERENCE_TIME rtActualLength;
		m_PropItem.m_Item.SourceWaveLengthToUnknownTime( m_pPropPageMgr->m_pTrackMgr, &rtActualLength );
		m_pPropPageMgr->m_pTrackMgr->ClocksToMeasureBeatGridTick( (MUSIC_TIME)m_PropItem.m_Item.m_rtTimePhysical + (MUSIC_TIME)rtActualLength, 
																  &m_lSourceEndMeasure, &m_lSourceEndBeat, &m_lSourceEndGrid, &m_lSourceEndTick );

		// SOURCE LENGTH
		REFERENCE_TIME rtTimePhysical;
		m_pPropPageMgr->m_pTrackMgr->m_pTimeline->ClocksToRefTime( (MUSIC_TIME)m_PropItem.m_Item.m_rtTimePhysical, &rtTimePhysical );
		m_pIWaveTimelineDraw->SampleToRefTime( (m_PropItem.m_Item.m_WaveInfo.dwWaveEnd + 1), &rtTime, m_PropItem.m_Item.m_lPitch );
		m_pPropPageMgr->m_pTrackMgr->m_pTimeline->RefTimeToClocks( (rtTimePhysical + rtTime), &mtTime );
		m_pPropPageMgr->m_pTrackMgr->ClocksToMeasureBeatGridTick( mtTime - (MUSIC_TIME)m_PropItem.m_Item.m_rtTimePhysical,
																  &m_lSourceLengthMeasure, &m_lSourceLengthBeat, &m_lSourceLengthGrid, &m_lSourceLengthTick );
	}
	else
	{
		m_lEndMeasure = 0;
		m_lEndBeat = 0;
		m_lEndGrid = 0;
		m_lEndTick = 0;

		m_lLengthMeasure = 0;
		m_lLengthBeat = 0;
		m_lLengthGrid = 0;
		m_lLengthTick = 0;
		m_dwLengthNbrSamples = 0;

		m_lOffsetMeasure = 0;
		m_lOffsetBeat = 0;
		m_lOffsetGrid = 0;
		m_lOffsetTick = 0;
		m_dwOffsetNbrSamples = 0;

		m_lSourceEndMeasure = 0;
		m_lSourceEndBeat = 0;
		m_lSourceEndGrid = 0;
		m_lSourceEndTick = 0;

		m_lSourceLengthMeasure = 0;
		m_lSourceLengthBeat = 0;
		m_lSourceLengthGrid = 0;
		m_lSourceLengthTick = 0;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabFileRef::SetControlRanges

void CTabFileRef::SetControlRanges( void )
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
// CTabFileRef::SetRefTimeRanges

void CTabFileRef::SetRefTimeRanges( void )
{
	// Limit the minute number to positive numbers
	m_spinStartMeasure.SetRange( 1, 32767 );
	m_editStartMeasure.LimitText( 5 );
	m_spinEndMeasure.SetRange( 1, 32767 );
	m_editEndMeasure.LimitText( 5 );
	m_spinLengthMeasure.SetRange( 0, 32767 );
	m_editLengthMeasure.LimitText( 5 );
	m_spinOffsetMeasure.SetRange( 0, 32767 );
	m_editOffsetMeasure.LimitText( 5 );

	// Limit the second number to 1-60
	m_spinStartBeat.SetRange( 1, 60 );
	m_editStartBeat.LimitText( 2 );
	m_spinEndBeat.SetRange( 1, 60 );
	m_editEndBeat.LimitText( 2 );
	m_spinLengthBeat.SetRange( 0, 60 );
	m_editLengthBeat.LimitText( 2 );
	m_spinOffsetBeat.SetRange( 0, 60 );
	m_editOffsetBeat.LimitText( 2 );

	// Limit the grid number to MIN_NBR_GRIDS_PER_SECOND - MAX_NBR_GRIDS_PER_SECOND
	m_spinStartGrid.SetRange( MIN_NBR_GRIDS_PER_SECOND, MAX_NBR_GRIDS_PER_SECOND );
	m_editStartGrid.LimitText( 3 );
	m_spinEndGrid.SetRange( MIN_NBR_GRIDS_PER_SECOND, MAX_NBR_GRIDS_PER_SECOND );
	m_editEndGrid.LimitText( 3 );
	m_spinLengthGrid.SetRange( (MIN_NBR_GRIDS_PER_SECOND - 1), MAX_NBR_GRIDS_PER_SECOND );
	m_editLengthGrid.LimitText( 3 );
	m_spinOffsetGrid.SetRange( (MIN_NBR_GRIDS_PER_SECOND - 1), MAX_NBR_GRIDS_PER_SECOND );
	m_editOffsetGrid.LimitText( 3 );

	// Limit the millisecond number to 1-1000
	m_spinStartTick.SetRange( 1, 1000 );
	m_editStartTick.LimitText( 4 );
	m_spinEndTick.SetRange( 1, 1000 );
	m_editEndTick.LimitText( 4 );
	m_spinLengthTick.SetRange( 1, 1000 );
	m_editLengthTick.LimitText( 4 );
	m_spinOffsetTick.SetRange( 1, 1000 );
	m_editOffsetTick.LimitText( 4 );
}


/////////////////////////////////////////////////////////////////////////////
// CTabFileRef::SetMusicTimeRanges

void CTabFileRef::SetMusicTimeRanges( DWORD dwGroupBits )
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
	m_spinStartMeasure.SetRange( 1, lMaxMeasure );
	m_editStartMeasure.LimitText( 5 );
	m_spinEndMeasure.SetRange( 1, 32767 );
	m_editEndMeasure.LimitText( 5 );
	m_spinLengthMeasure.SetRange( 0, 32767 );
	m_editLengthMeasure.LimitText( 5 );
	m_spinOffsetMeasure.SetRange( 0, 32767 );
	m_editOffsetMeasure.LimitText( 5 );

	// Update the range for the beat spin control
	m_spinStartBeat.SetRange( 1, lMaxBeat );
	m_editStartBeat.LimitText( 3 );
	m_spinEndBeat.SetRange( 1, lMaxBeat );
	m_editEndBeat.LimitText( 3 );
	m_spinLengthBeat.SetRange( 0, lMaxBeat );
	m_editLengthBeat.LimitText( 3 );
	m_spinOffsetBeat.SetRange( 0, lMaxBeat );
	m_editOffsetBeat.LimitText( 3 );

	// Update the range for the grid spin control
	m_spinStartGrid.SetRange( 1, lMaxGrid );
	m_editStartGrid.LimitText( 3 );
	m_spinEndGrid.SetRange( 1, lMaxGrid );
	m_editEndGrid.LimitText( 3 );
	m_spinLengthGrid.SetRange( 0, lMaxGrid );
	m_editLengthGrid.LimitText( 3 );
	m_spinOffsetGrid.SetRange( 0, lMaxGrid );
	m_editOffsetGrid.LimitText( 3 );

	// Update the range for the tick spin control
	m_spinStartTick.SetRange( -MAX_TICK, MAX_TICK );
	m_editStartTick.LimitText( 5 );
	m_spinEndTick.SetRange( -MAX_TICK, MAX_TICK );
	m_editEndTick.LimitText( 5 );
	m_spinLengthTick.SetRange( -MAX_TICK, MAX_TICK );
	m_editLengthTick.LimitText( 5 );
	m_spinOffsetTick.SetRange( -MAX_TICK, MAX_TICK );
	m_editOffsetTick.LimitText( 5 );
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

	if( m_PropItem.m_dwUndetermined_TabFileRef & UNDT_DOCROOT )
	{
		goto LEAVE;
	}

	if( m_PropItem.m_Item.m_FileRef.pIDocRootNode )
	{
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

		m_pPropPageMgr->m_pIFramework->GetNodeFileGUID ( m_PropItem.m_Item.m_FileRef.pIDocRootNode, &pFileListInfo->guidFile );

		if( SUCCEEDED ( m_PropItem.m_Item.m_FileRef.pIDocRootNode->GetNodeListInfo ( &ListInfo ) ) )
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

			if( FAILED ( m_pPropPageMgr->m_pIFramework->FindProject( m_PropItem.m_Item.m_FileRef.pIDocRootNode, &pIProject ) ) )
			{
				delete pFileListInfo;
				goto LEAVE;
			}

			pFileListInfo->pIProject = pIProject;
//			pFileListInfo->pIProject->AddRef();	intentionally missing

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
	}

LEAVE:
	m_comboFile.SetCurSel( nPos );

	if( m_PropItem.m_dwUndetermined_TabFileRef & UNDT_DOCROOT )
	{
		m_comboFile.SetWindowText( g_pstrUnknown );
	}

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

	hr = m_pPropPageMgr->m_pIFramework->FindDocTypeByNodeId( GUID_WaveNode, &pIDocType );
	if( FAILED ( hr ) )
	{
		return;
	}

	// Get GUID for the DocRoot containing this Wave track (i.e. Segment's DocRoot node)
	IDMUSProdNode* pIMyDocRootNode;
	GUID guidMyDocRoot;
	hr = m_pPropPageMgr->m_pTrackMgr->GetParam( GUID_DocRootNode, 0, 0, &pIMyDocRootNode );
	if( FAILED ( hr ) )
	{
		ASSERT( 0 );
		return;
	}
	hr = m_pPropPageMgr->m_pIFramework->GetNodeFileGUID( pIMyDocRootNode, &guidMyDocRoot );
	if( FAILED ( hr ) )
	{
		ASSERT( 0 );
		return;
	}
	RELEASE( pIMyDocRootNode );

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

				if( ::IsEqualGUID(pFileListInfo->guidFile, guidMyDocRoot) )
				{
					// Prevent circular references!
					// Do not include DocRoot containing this Wave Track in the file list
					delete pFileListInfo;
				}
				else
				{
					InsertFileInfo( pFileListInfo );
				}

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

	// Set lock button bitmaps
	if( m_pPropPageMgr
	&&  m_pPropPageMgr->m_pTrackMgr )
	{
		CBitmap* pBitmap = m_pPropPageMgr->m_pTrackMgr->GetLockBitmap( 0 );
		if( pBitmap )
		{
			m_btnLockEnd.SetBitmap( (HBITMAP)pBitmap->GetSafeHandle() );
			m_btnLockLength.SetBitmap( (HBITMAP)pBitmap->GetSafeHandle() );
		}
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	               // EXCEPTION: OCX Property Pages should return FALSE
}


/////////////////////////////////////////////////////////////////////////////
// CTabFileRef::OnSpinStartTick

void CTabFileRef::OnSpinStartTick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// Need to do this in case the user clicked the spin control immediately after
	// typing in a value
	OnKillFocusStartTick();

	if( HandleDeltaChange( pNMHDR, pResult, m_lStartTick ) )
	{
		SetStart( StartToUnknownTime() );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabFileRef::OnSpinStartGrid

void CTabFileRef::OnSpinStartGrid(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// Need to do this in case the user clicked the spin control immediately after
	// typing in a value
	OnKillFocusStartGrid();

	if( HandleDeltaChange( pNMHDR, pResult, m_lStartGrid ) )
	{
		SetStart( StartToUnknownTime() );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabFileRef::OnSpinStartBeat

void CTabFileRef::OnSpinStartBeat(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// Need to do this in case the user clicked the spin control immediately after
	// typing in a value
	OnKillFocusStartBeat();

	if( HandleDeltaChange( pNMHDR, pResult, m_lStartBeat ) )
	{
		SetStart( StartToUnknownTime() );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabFileRef::OnSpinStartMeasure

void CTabFileRef::OnSpinStartMeasure(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// Need to do this in case the user clicked the spin control immediately after
	// typing in a value
	OnKillFocusStartMeasure();

	if( HandleDeltaChange( pNMHDR, pResult, m_lStartMeasure ) )
	{
		SetStart( StartToUnknownTime() );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabFileRef::OnSpinEndTick

void CTabFileRef::OnSpinEndTick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// Need to do this in case the user clicked the spin control immediately after
	// typing in a value
	OnKillFocusEndTick();

	if( HandleDeltaChange( pNMHDR, pResult, m_lEndTick ) )
	{
		SetEnd( EndToUnknownTime() );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabFileRef::OnSpinEndGrid

void CTabFileRef::OnSpinEndGrid(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// Need to do this in case the user clicked the spin control immediately after
	// typing in a value
	OnKillFocusEndGrid();

	if( HandleDeltaChange( pNMHDR, pResult, m_lEndGrid ) )
	{
		SetEnd( EndToUnknownTime() );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabFileRef::OnSpinEndBeat

void CTabFileRef::OnSpinEndBeat(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// Need to do this in case the user clicked the spin control immediately after
	// typing in a value
	OnKillFocusEndBeat();

	if( HandleDeltaChange( pNMHDR, pResult, m_lEndBeat ) )
	{
		SetEnd( EndToUnknownTime() );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabFileRef::OnSpinEndMeasure

void CTabFileRef::OnSpinEndMeasure(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// Need to do this in case the user clicked the spin control immediately after
	// typing in a value
	OnKillFocusEndMeasure();

	if( HandleDeltaChange( pNMHDR, pResult, m_lEndMeasure ) )
	{
		SetEnd( EndToUnknownTime() );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabFileRef::OnSpinLengthTick

void CTabFileRef::OnSpinLengthTick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// Need to do this in case the user clicked the spin control immediately after
	// typing in a value
	OnKillFocusLengthTick();

	if( HandleDeltaChange( pNMHDR, pResult, m_lLengthTick ) )
	{
		SetLength( LengthToUnknownTime() );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabFileRef::OnSpinLengthGrid

void CTabFileRef::OnSpinLengthGrid(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// Need to do this in case the user clicked the spin control immediately after
	// typing in a value
	OnKillFocusLengthGrid();

	if( HandleDeltaChange( pNMHDR, pResult, m_lLengthGrid ) )
	{
		SetLength( LengthToUnknownTime() );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabFileRef::OnSpinLengthBeat

void CTabFileRef::OnSpinLengthBeat(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// Need to do this in case the user clicked the spin control immediately after
	// typing in a value
	OnKillFocusLengthBeat();

	if( HandleDeltaChange( pNMHDR, pResult, m_lLengthBeat ) )
	{
		SetLength( LengthToUnknownTime() );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabFileRef::OnSpinLengthMeasure

void CTabFileRef::OnSpinLengthMeasure(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// Need to do this in case the user clicked the spin control immediately after
	// typing in a value
	OnKillFocusLengthMeasure();

	if( HandleDeltaChange( pNMHDR, pResult, m_lLengthMeasure ) )
	{
		SetLength( LengthToUnknownTime() );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabFileRef::OnSpinOffsetTick

void CTabFileRef::OnSpinOffsetTick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// Need to do this in case the user clicked the spin control immediately after
	// typing in a value
	OnKillFocusOffsetTick();

	if( HandleDeltaChange( pNMHDR, pResult, m_lOffsetTick ) )
	{
		SetOffset( OffsetToRefTime() );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabFileRef::OnSpinOffsetGrid

void CTabFileRef::OnSpinOffsetGrid(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// Need to do this in case the user clicked the spin control immediately after
	// typing in a value
	OnKillFocusOffsetGrid();

	if( HandleDeltaChange( pNMHDR, pResult, m_lOffsetGrid ) )
	{
		SetOffset( OffsetToRefTime() );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabFileRef::OnSpinOffsetBeat

void CTabFileRef::OnSpinOffsetBeat(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// Need to do this in case the user clicked the spin control immediately after
	// typing in a value
	OnKillFocusOffsetBeat();

	if( HandleDeltaChange( pNMHDR, pResult, m_lOffsetBeat ) )
	{
		SetOffset( OffsetToRefTime() );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabFileRef::OnSpinOffsetMeasure

void CTabFileRef::OnSpinOffsetMeasure(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// Need to do this in case the user clicked the spin control immediately after
	// typing in a value
	OnKillFocusOffsetMeasure();

	if( HandleDeltaChange( pNMHDR, pResult, m_lOffsetMeasure ) )
	{
		SetOffset( OffsetToRefTime() );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabFileRef::HandleDeltaChange
//
// Generic handler for deltapos changes
bool CTabFileRef::HandleDeltaChange( NMHDR* pNMHDR, LRESULT* pResult, long& lUpdateVal )
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
// CTabFileRef::OnKillFocusStartTick

void CTabFileRef::OnKillFocusStartTick() 
{
	if( HandleKillFocus( m_spinStartTick, m_lStartTick, 0 ) )
	{
		SetStart( StartToUnknownTime() );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabFileRef::OnKillFocusStartGrid

void CTabFileRef::OnKillFocusStartGrid() 
{
	if( HandleKillFocus( m_spinStartGrid, m_lStartGrid, 1 ) )
	{
		SetStart( StartToUnknownTime() );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabFileRef::OnKillFocusStartBeat

void CTabFileRef::OnKillFocusStartBeat() 
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

	if( HandleKillFocus( m_spinStartBeat, m_lStartBeat, nOffset ) )
	{
		SetStart( StartToUnknownTime() );
	}
}

/////////////////////////////////////////////////////////////////////////////
// CTabFileRef::OnKillFocusStartMeasure

void CTabFileRef::OnKillFocusStartMeasure() 
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

	if( HandleKillFocus( m_spinStartMeasure, m_lStartMeasure, nOffset ) )
	{
		SetStart( StartToUnknownTime() );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabFileRef::OnKillFocusEndTick

void CTabFileRef::OnKillFocusEndTick() 
{
	if( HandleKillFocus( m_spinEndTick, m_lEndTick, 0 ) )
	{
		SetEnd( EndToUnknownTime() );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabFileRef::OnKillFocusEndGrid

void CTabFileRef::OnKillFocusEndGrid() 
{
	if( HandleKillFocus( m_spinEndGrid, m_lEndGrid, 1 ) )
	{
		SetEnd( EndToUnknownTime() );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabFileRef::OnKillFocusEndBeat

void CTabFileRef::OnKillFocusEndBeat() 
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

	if( HandleKillFocus( m_spinEndBeat, m_lEndBeat, nOffset ) )
	{
		SetEnd( EndToUnknownTime() );
	}
}

/////////////////////////////////////////////////////////////////////////////
// CTabFileRef::OnKillFocusEndMeasure

void CTabFileRef::OnKillFocusEndMeasure() 
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

	if( HandleKillFocus( m_spinEndMeasure, m_lEndMeasure, nOffset ) )
	{
		SetEnd( EndToUnknownTime() );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabFileRef::OnKillFocusLengthTick

void CTabFileRef::OnKillFocusLengthTick() 
{
	if( HandleKillFocus( m_spinLengthTick, m_lLengthTick, 0 ) )
	{
		SetLength( LengthToUnknownTime() );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabFileRef::OnKillFocusLengthGrid

void CTabFileRef::OnKillFocusLengthGrid() 
{
	if( HandleKillFocus( m_spinLengthGrid, m_lLengthGrid, 0 ) )
	{
		SetLength( LengthToUnknownTime() );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabFileRef::OnKillFocusLengthBeat

void CTabFileRef::OnKillFocusLengthBeat() 
{
	if( HandleKillFocus( m_spinLengthBeat, m_lLengthBeat, 0 ) )
	{
		SetLength( LengthToUnknownTime() );
	}
}

/////////////////////////////////////////////////////////////////////////////
// CTabFileRef::OnKillFocusLengthMeasure

void CTabFileRef::OnKillFocusLengthMeasure() 
{
	if( HandleKillFocus( m_spinLengthMeasure, m_lLengthMeasure, 0 ) )
	{
		SetLength( LengthToUnknownTime() );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabFileRef::OnKillFocusOffsetTick

void CTabFileRef::OnKillFocusOffsetTick() 
{
	if( HandleKillFocus( m_spinOffsetTick, m_lOffsetTick, 0 ) )
	{
		SetOffset( OffsetToRefTime() );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabFileRef::OnKillFocusOffsetGrid

void CTabFileRef::OnKillFocusOffsetGrid() 
{
	if( HandleKillFocus( m_spinOffsetGrid, m_lOffsetGrid, 0 ) )
	{
		SetOffset( OffsetToRefTime() );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabFileRef::OnKillFocusOffsetBeat

void CTabFileRef::OnKillFocusOffsetBeat() 
{
	if( HandleKillFocus( m_spinOffsetBeat, m_lOffsetBeat, 0 ) )
	{
		SetOffset( OffsetToRefTime() );
	}
}

/////////////////////////////////////////////////////////////////////////////
// CTabFileRef::OnKillFocusOffsetMeasure

void CTabFileRef::OnKillFocusOffsetMeasure() 
{
	if( HandleKillFocus( m_spinOffsetMeasure, m_lOffsetMeasure, 0 ) )
	{
		SetOffset( OffsetToRefTime() );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabFileRef::HandleKillFocus
//
// Generic handler for KillFocus changes
bool CTabFileRef::HandleKillFocus( CSpinButtonCtrl& spin, long& lUpdateVal, int nOffset )
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
// CTabFileRef::UpdateObject

void CTabFileRef::UpdateObject( void )
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
// CTabFileRef::EnableControls

void CTabFileRef::EnableControls( BOOL fEnable )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Check if the Beat edit control exists
	if( m_editStartBeat.GetSafeHwnd() == NULL )
	{
		return;
	}

	BOOL fEnableFileCombo = fEnable;
	BOOL fEnableStartTime = fEnable;

	if( fEnable )
	{
		if( m_PropItem.m_dwBits & PROPF_MULTIPLESELECT )
		{
			fEnable = FALSE;
			fEnableStartTime = FALSE;
		}
		else if( m_PropItem.m_Item.m_FileRef.pIDocRootNode == NULL )
		{
			// No DocRoot, so disable remaining controls
			fEnable = FALSE;
		}
	}

	// Reference time or music time?
	CString strPrompt;
	if( m_pPropPageMgr->m_pTrackMgr->IsRefTimeTrack() )
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

	// Update the controls with the new window state
	m_comboFile.EnableWindow( fEnableFileCombo );
	m_btnLockEnd.EnableWindow( fEnable );
	m_btnLockLength.EnableWindow( fEnable );

	m_spinStartTick.EnableWindow( fEnableStartTime );
	m_spinStartGrid.EnableWindow( fEnableStartTime );
	m_spinStartBeat.EnableWindow( fEnableStartTime );
	m_spinStartMeasure.EnableWindow( fEnableStartTime );
	m_editStartTick.EnableWindow( fEnableStartTime );
	m_editStartGrid.EnableWindow( fEnableStartTime );
	m_editStartBeat.EnableWindow( fEnableStartTime );
	m_editStartMeasure.EnableWindow( fEnableStartTime );

	m_spinEndTick.EnableWindow( fEnable );
	m_spinEndGrid.EnableWindow( fEnable );
	m_spinEndBeat.EnableWindow( fEnable );
	m_spinEndMeasure.EnableWindow( fEnable );
	m_editEndTick.EnableWindow( fEnable );
	m_editEndGrid.EnableWindow( fEnable );
	m_editEndBeat.EnableWindow( fEnable );
	m_editEndMeasure.EnableWindow( fEnable );

	m_spinLengthTick.EnableWindow( fEnable );
	m_spinLengthGrid.EnableWindow( fEnable );
	m_spinLengthBeat.EnableWindow( fEnable );
	m_spinLengthMeasure.EnableWindow( fEnable );
	m_editLengthTick.EnableWindow( fEnable );
	m_editLengthGrid.EnableWindow( fEnable );
	m_editLengthBeat.EnableWindow( fEnable );
	m_editLengthMeasure.EnableWindow( fEnable );

	m_spinOffsetTick.EnableWindow( fEnable );
	m_spinOffsetGrid.EnableWindow( fEnable );
	m_spinOffsetBeat.EnableWindow( fEnable );
	m_spinOffsetMeasure.EnableWindow( fEnable );
	m_editOffsetTick.EnableWindow( fEnable );
	m_editOffsetGrid.EnableWindow( fEnable );
	m_editOffsetBeat.EnableWindow( fEnable );
	m_editOffsetMeasure.EnableWindow( fEnable );

	if( m_PropItem.m_dwUndetermined_TabFileRef & UNDT_DOCROOT )
	{
		int nPos = m_comboFile.AddString( g_pstrUnknown );
		m_comboFile.SetCurSel( nPos );
	}

	// Clear the controls that we're disabling
	if( !fEnableStartTime )
	{
		m_editStartTick.SetWindowText( NULL );
		m_editStartGrid.SetWindowText( NULL );
		m_editStartBeat.SetWindowText( NULL );
		m_editStartMeasure.SetWindowText( NULL );
	}

	if( !fEnable )
	{
		m_comboFile.SetCurSel( -1 );

		m_editEndTick.SetWindowText( NULL );
		m_editEndGrid.SetWindowText( NULL );
		m_editEndBeat.SetWindowText( NULL );
		m_editEndMeasure.SetWindowText( NULL );

		m_editLengthTick.SetWindowText( NULL );
		m_editLengthGrid.SetWindowText( NULL );
		m_editLengthBeat.SetWindowText( NULL );
		m_editLengthMeasure.SetWindowText( NULL );
		m_staticLengthNbrSamples.SetWindowText( NULL );

		m_editOffsetTick.SetWindowText( NULL );
		m_editOffsetGrid.SetWindowText( NULL );
		m_editOffsetBeat.SetWindowText( NULL );
		m_editOffsetMeasure.SetWindowText( NULL );
		m_staticOffsetNbrSamples.SetWindowText( NULL );

		m_staticSourceEndTick.SetWindowText( NULL );
		m_staticSourceEndGrid.SetWindowText( NULL );
		m_staticSourceEndBeat.SetWindowText( NULL );
		m_staticSourceEndMeasure.SetWindowText( NULL );

		m_staticSourceLengthTick.SetWindowText( NULL );
		m_staticSourceLengthGrid.SetWindowText( NULL );
		m_staticSourceLengthBeat.SetWindowText( NULL );
		m_staticSourceLengthMeasure.SetWindowText( NULL );
		m_staticSourceLengthNbrSamples.SetWindowText( NULL );

		if( m_pPropPageMgr
		&&  m_pPropPageMgr->m_pTrackMgr )
		{
			CBitmap* pBitmap = m_pPropPageMgr->m_pTrackMgr->GetLockBitmap( 0 );
			if( pBitmap )
			{
				m_btnLockLength.SetBitmap( (HBITMAP)pBitmap->GetSafeHandle() );
			}
		}
	}
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

	int nItem = m_comboFile.GetCurSel();
	if( nItem != CB_ERR )
	{
		FileListInfo* pFileListInfo = (FileListInfo *)m_comboFile.GetItemDataPtr( nItem );
		if( pFileListInfo != (FileListInfo *)-1 )
		{
			// fill in appropriate fields
			m_PropItem.m_Item.m_FileRef.li = *pFileListInfo;

			IDMUSProdNode* pIDocRootNode = NULL;
			
			// NOTE: Items must currently be selected to avoid killing
			// the property page while making this call
			if( SUCCEEDED ( m_pPropPageMgr->m_pIFramework->FindDocRootNodeByFileGUID( pFileListInfo->guidFile, &pIDocRootNode ) ) )
			{
				if( m_PropItem.m_Item.m_FileRef.pIDocRootNode )
				{
					m_PropItem.m_Item.m_FileRef.pIDocRootNode->Release();
					m_PropItem.m_Item.m_FileRef.pIDocRootNode = NULL;
				}
				
				m_PropItem.m_Item.m_FileRef.pIDocRootNode = pIDocRootNode;
				if( m_PropItem.m_Item.m_FileRef.pIDocRootNode )
				{
					m_PropItem.m_Item.m_FileRef.pIDocRootNode->AddRef();
				}

				m_PropItem.m_dwChanged = CHGD_DOCROOT;
				UpdateObject();

				pIDocRootNode->Release();
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabFileRef::OnSetActive

BOOL CTabFileRef::OnSetActive() 
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
// CTabFileRef::OnLockEnd

void CTabFileRef::OnLockEnd() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Check if we have a valid DataObject pointer
	if( m_pPropPageMgr->m_pIPropPageObject == NULL )
	{
		return;
	}

	// Check if the Loop check box exists
	if( m_btnLockEnd.GetSafeHwnd() == NULL )
	{
		return;
	}

	// Update the item with the new value
	m_PropItem.m_Item.m_fLockEndUI = !m_PropItem.m_Item.m_fLockEndUI;

	// Now, update the object with the new value
	m_PropItem.m_dwChanged = CHGD_LOCK_END;
	UpdateObject();
}


/////////////////////////////////////////////////////////////////////////////
// CTabFileRef::OnDoubleClickedLockEnd

void CTabFileRef::OnDoubleClickedLockEnd() 
{
	OnLockEnd();
}


/////////////////////////////////////////////////////////////////////////////
// CTabFileRef::OnLockLength

void CTabFileRef::OnLockLength() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Check if we have a valid DataObject pointer
	if( m_pPropPageMgr->m_pIPropPageObject == NULL )
	{
		return;
	}

	// Check if the Loop check box exists
	if( m_btnLockLength.GetSafeHwnd() == NULL )
	{
		return;
	}

	// Update the item with the new value
	m_PropItem.m_Item.m_fLockLengthUI = !m_PropItem.m_Item.m_fLockLengthUI;

	// Now, update the object with the new value
	m_PropItem.m_dwChanged = CHGD_LOCK_LENGTH;
	UpdateObject();
}


/////////////////////////////////////////////////////////////////////////////
// CTabFileRef::OnDoubleClickedLockLength

void CTabFileRef::OnDoubleClickedLockLength() 
{
	OnLockLength();
}
