// PropPageMarker.cpp : implementation file
//

#include "stdafx.h"
#include "MarkerItem.h"
#include "PropPageMgr.h"
#include "PropPageMarker.h"
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

/////////////////////////////////////////////////////////////////////////////
// CMarkerPropPageMgr constructor/destructor

CMarkerPropPageMgr::CMarkerPropPageMgr()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Initialize our pointer to NULL
	m_pPropPageMarker = NULL;

	// call the base class contstructor
	CStaticPropPageManager::CStaticPropPageManager();
}

CMarkerPropPageMgr::~CMarkerPropPageMgr()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// If the Marker property page exists, delete it
	if( m_pPropPageMarker )
	{
		delete m_pPropPageMarker;
		m_pPropPageMarker = NULL;
	}

	// Call the base class destructor
	CStaticPropPageManager::~CStaticPropPageManager();
}


/////////////////////////////////////////////////////////////////////////////
// CMarkerPropPageMgr IDMUSProdPropPageManager implementation

/////////////////////////////////////////////////////////////////////////////
// CMarkerPropPageMgr::GetPropertySheetTitle

HRESULT STDMETHODCALLTYPE CMarkerPropPageMgr::GetPropertySheetTitle( BSTR* pbstrTitle, 
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
	if( comBSTR.LoadString( IDS_PROPPAGE_MARKER ) )
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
// CMarkerPropPageMgr::GetPropertySheetPages

HRESULT STDMETHODCALLTYPE CMarkerPropPageMgr::GetPropertySheetPages( IDMUSProdPropSheet* pIPropSheet, 
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
	if( m_pPropPageMarker == NULL )
	{
		m_pPropPageMarker = new PropPageMarker();
	}

	// If the property page now exists, add the property page
	if( m_pPropPageMarker )
	{
		// Copy the PROPSHEETPAGE structure
		PROPSHEETPAGE psp;
		memcpy( &psp, &m_pPropPageMarker->m_psp, sizeof(PROPSHEETPAGE) );

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
		m_pPropPageMarker->m_pPropPageMgr = this;
	}

	// Set number of pages
	*pnNbrPages = nNbrPages;
	return S_OK;
};


/////////////////////////////////////////////////////////////////////////////
// CMarkerPropPageMgr::RefreshData

HRESULT STDMETHODCALLTYPE CMarkerPropPageMgr::RefreshData()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Get a pointer to a CMarkerItem
	CMarkerItem* pMarker;

	if( m_pIPropPageObject == NULL )
	{
		// No property page object - set pointer to NULL
		pMarker = NULL;
	}
	// Have a property page object - try and get a pointer from it
	else if( FAILED ( m_pIPropPageObject->GetData( (void **)&pMarker ) ) )
	{
		return E_FAIL;
	}

	// Update the property page, if it exists
	if (m_pPropPageMarker )
	{
		// pMarker may be NULL, meaning nothing is selected
		m_pPropPageMarker->SetMarker( pMarker );
		return S_OK;
	}
	else
	{
		return E_FAIL;
	}
};


/////////////////////////////////////////////////////////////////////////////
// PropPageMarker property page

IMPLEMENT_DYNCREATE(PropPageMarker, CPropertyPage)

/////////////////////////////////////////////////////////////////////////////
// PropPageMarker constructor/destructor

PropPageMarker::PropPageMarker(): CPropertyPage(PropPageMarker::IDD)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

//	CPropertyPage(IDD_MARKER_PROPPAGE);
	//{{AFX_DATA_INIT(PropPageMarker)
	//}}AFX_DATA_INIT

	// Initialize our pointers to NULL
	m_pPropPageMgr = NULL;
	m_pTimeline = NULL;

	// Initialize our class members to FALSE
	m_fValidMarker = FALSE;
	m_fNeedToDetach = FALSE;
}

PropPageMarker::~PropPageMarker()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
}


/////////////////////////////////////////////////////////////////////////////
// PropPageMarker::DoDataExchange

void PropPageMarker::DoDataExchange(CDataExchange* pDX)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	CPropertyPage::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(PropPageMarker)
	DDX_Control(pDX, IDC_EDIT_GRID, m_editGrid);
	DDX_Control(pDX, IDC_SPIN_GRID, m_spinGrid);
	DDX_Control(pDX, IDC_SPIN_TICK, m_spinTick);
	DDX_Control(pDX, IDC_EDIT_TICK, m_editTick);
	DDX_Control(pDX, IDC_EDIT_BEAT, m_editBeat);
	DDX_Control(pDX, IDC_EDIT_MARKER, m_editMarker);
	DDX_Control(pDX, IDC_EDIT_MEASURE, m_editMeasure);
	DDX_Control(pDX, IDC_SPIN_MEASURE, m_spinMeasure);
	DDX_Control(pDX, IDC_SPIN_BEAT, m_spinBeat);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(PropPageMarker, CPropertyPage)
	//{{AFX_MSG_MAP(PropPageMarker)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_BEAT, OnDeltaposSpinBeat)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_MEASURE, OnDeltaposSpinMeasure)
	ON_EN_KILLFOCUS(IDC_EDIT_BEAT, OnKillfocusEditBeat)
	ON_EN_KILLFOCUS(IDC_EDIT_MEASURE, OnKillfocusEditMeasure)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_TICK, OnDeltaposSpinTick)
	ON_EN_KILLFOCUS(IDC_EDIT_TICK, OnKillfocusEditTick)
	ON_WM_KILLFOCUS()
	ON_EN_KILLFOCUS(IDC_EDIT_GRID, OnKillfocusEditGrid)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_GRID, OnDeltaposSpinGrid)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// PropPageMarker custom functions

/////////////////////////////////////////////////////////////////////////////
// PropPageMarker::SetMarker

void PropPageMarker::SetMarker( const CMarkerItem* pMarker )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// If the pointer is null, there are no Markers selected, so disable the property page
	if ( pMarker == NULL )
	{
		m_fValidMarker = FALSE;
		m_Marker.Clear();
		EnableControls( FALSE );
		return;
	}

	// Update our timeline pointer

	// Initialize the timeline pointer to NULL
	m_pTimeline = NULL;

	// Initialize our groupbits to all 32 groups
	DWORD dwGroupBits = 0xFFFFFFFF;

	// Verify we have a valid pinter to our Property Page Object
	if (m_pPropPageMgr && m_pPropPageMgr->m_pIPropPageObject)
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

	// Flag that we have a valid Marker
	m_fValidMarker = TRUE;

		// Copy the information from the new Marker
	m_Marker.Copy( pMarker );

	long lMaxMeasure, lMaxBeat, lMaxGrid; // Maximum measure, beat, and grid values

	// Validate the timeline pointer
	if( m_pTimeline == NULL )
	{
		// No timeline pointer - use default values
		lMaxMeasure = 32767;
		lMaxBeat = 255;
		lMaxGrid = 255;
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

		// If the Marker is not in the last measure
		if( m_Marker.m_lMeasure < lMaxMeasure )
		{
			// Convert from a Measure value to a clock value
			long lClockForMeasure;
			m_pTimeline->MeasureBeatToClocks( dwGroupBits, 0, m_Marker.m_lMeasure, 0, &lClockForMeasure );

			// Get the TimeSig for this measure
			DMUS_TIMESIGNATURE TimeSig;
			if( SUCCEEDED( m_pTimeline->GetParam( GUID_TimeSignature, dwGroupBits, 0, lClockForMeasure, NULL, &TimeSig ) ) )
			{
				lMaxBeat = TimeSig.bBeatsPerMeasure - 1;
			}
		}
		// Else the Marker is in the last measure and lMaxBeat is already set

		// Now convert the measure and beat from 0-based to 1-based
		lMaxMeasure++;
		lMaxBeat++;

		// BUGBUG:
		lMaxGrid = lMaxBeat;
	}

	// Check if the edit control has a valid window handle.
	if( m_editBeat.GetSafeHwnd() == NULL )
	{
		// It doesn't have a valid window handle - the property page may have been destroyed (or not yet created).
		// Just exit early
		return;
	}

	// Don't send OnKill/OnUpdate notifications when updating the display
	CLockoutNotification LockoutNotifications( m_hWnd );

	// Check if multiple items are selected
	if( m_Marker.m_dwBitsUI & UD_MULTIPLESELECT )
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

	// The text to display what type of marker is selected
	CString strMarkerText;
	switch( m_Marker.m_typeMarker )
	{
	case MARKER_SWITCH:
		strMarkerText.LoadString( IDS_SWITCH_MARKER );
		break;
	case MARKER_CUE:
		strMarkerText.LoadString( IDS_CUE_MARKER );
		break;
	default:
		ASSERT(FALSE);
		break;
	}
	m_editMarker.SetWindowText( strMarkerText );

	// If the item was previously invalid, update the measure and beat display
	if( !m_fValidMarker )
	{
		SetDlgItemInt( m_editMeasure.GetDlgCtrlID(), m_Marker.m_lMeasure + 1 );
		SetDlgItemInt( m_editBeat.GetDlgCtrlID(), m_Marker.m_lBeat + 1 );
		SetDlgItemInt( m_editGrid.GetDlgCtrlID(), m_Marker.m_lGrid + 1 );
		SetDlgItemInt( m_editTick.GetDlgCtrlID(), m_Marker.m_lTick );
	}
	// Otherwise, check if the values displayed don't match the new ones
	else
	{
		// Get text from measure edit control
		TCHAR tcstrTmp[DIALOG_EDIT_LEN];
		m_editMeasure.GetWindowText( tcstrTmp, DIALOG_EDIT_LEN );

		// Convert from text to an integer
		BOOL fTransSuccess;
		long lCurValue = GetDlgItemInt( m_editMeasure.GetDlgCtrlID(), &fTransSuccess, TRUE );

		// Check if the conversion failed, the text is empty, or if the values are different
		if( !fTransSuccess || (tcstrTmp[0] == NULL) || (lCurValue != m_Marker.m_lMeasure + 1) )
		{
			// Update the displayed measure number
			SetDlgItemInt( m_editMeasure.GetDlgCtrlID(), m_Marker.m_lMeasure + 1 );
		}


		// Get text from beat edit control
		m_editBeat.GetWindowText( tcstrTmp, DIALOG_EDIT_LEN );

		// Convert from text to an integer
		lCurValue = GetDlgItemInt( m_editBeat.GetDlgCtrlID(), &fTransSuccess, TRUE );

		// Check if the conversion failed, the text is empty, or if the values are different
		if( !fTransSuccess || (tcstrTmp[0] == NULL) || (lCurValue != m_Marker.m_lBeat + 1) )
		{
			// Update the displayed beat number
			SetDlgItemInt( m_editBeat.GetDlgCtrlID(), m_Marker.m_lBeat + 1 );
		}


		// Get text from grid edit control
		m_editGrid.GetWindowText( tcstrTmp, DIALOG_EDIT_LEN );

		// Convert from text to an integer
		lCurValue = GetDlgItemInt( m_editGrid.GetDlgCtrlID(), &fTransSuccess, TRUE );

		// Check if the conversion failed, the text is empty, or if the values are different
		if( !fTransSuccess || (tcstrTmp[0] == NULL) || (lCurValue != m_Marker.m_lGrid + 1) )
		{
			// Update the displayed grid number
			SetDlgItemInt( m_editGrid.GetDlgCtrlID(), m_Marker.m_lGrid + 1 );
		}


		// Get text from tick edit control
		m_editTick.GetWindowText( tcstrTmp, DIALOG_EDIT_LEN );

		// Convert from text to an integer
		lCurValue = GetDlgItemInt( m_editTick.GetDlgCtrlID(), &fTransSuccess, TRUE );

		// Check if the conversion failed, the text is empty, or if the values are different
		if( !fTransSuccess || (tcstrTmp[0] == NULL) || (lCurValue != m_Marker.m_lTick) )
		{
			// Update the displayed tick
			SetDlgItemInt( m_editTick.GetDlgCtrlID(), m_Marker.m_lTick );
		}
	}

	// Update the range for the measure spin control
	m_spinMeasure.SetRange( 1, lMaxMeasure );

	// Update the range for the beat spin control
	m_spinBeat.SetRange( 1, lMaxBeat );

	// Update the range for the grid spin control
	m_spinGrid.SetRange( 1, lMaxGrid );

	// Update the range for the tick spin control
	m_spinTick.SetRange( 0, MAX_TICK );
}


/////////////////////////////////////////////////////////////////////////////
// PropPageMarker message handlers

/////////////////////////////////////////////////////////////////////////////
// PropPageMarker::OnCreate

int PropPageMarker::OnCreate(LPCREATESTRUCT lpCreateStruct) 
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
// PropPageMarker::OnDestroy

void PropPageMarker::OnDestroy() 
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
// PropPageMarker::OnInitDialog

BOOL PropPageMarker::OnInitDialog() 
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

	// Limit the grid number to 1-256
	m_spinGrid.SetRange( 1, 256 );
	m_editGrid.LimitText( 3 );

	// Limit the tick number to MAX_TICK
	m_spinTick.SetRange( -MAX_TICK, MAX_TICK );
	m_editTick.LimitText( 5 );

	// If we have valid information
	if(	m_fValidMarker )
	{
		// Force update of the controls
		m_fValidMarker = FALSE;

		// Update the dialog
		SetMarker( &m_Marker );
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	               // EXCEPTION: OCX Property Pages should return FALSE
}


/////////////////////////////////////////////////////////////////////////////
// PropPageMarker::OnDeltaposSpinBeat

void PropPageMarker::OnDeltaposSpinBeat(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// Need to do this in case the user clicked the spin control immediately after
	// typing in a value
	OnKillfocusEditBeat();

	HandleDeltaChange( pNMHDR, pResult, m_Marker.m_lBeat );
}


/////////////////////////////////////////////////////////////////////////////
// PropPageMarker::OnDeltaposSpinMeasure

void PropPageMarker::OnDeltaposSpinMeasure(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// Need to do this in case the user clicked the spin control immediately after
	// typing in a value
	OnKillfocusEditMeasure();

	HandleDeltaChange( pNMHDR, pResult, m_Marker.m_lMeasure );
}


/////////////////////////////////////////////////////////////////////////////
// PropPageMarker::HandleDeltaChange
//
// Generic handler for deltapos changes
void PropPageMarker::HandleDeltaChange( NMHDR* pNMHDR,
									   LRESULT* pResult,
									   long& lUpdateVal )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_pPropPageMgr->m_pIPropPageObject == NULL )
	{
		return;
	}

	// If the value changed - update the selected Marker
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
// PropPageMarker::OnKillfocusEditBeat

void PropPageMarker::OnKillfocusEditBeat() 
{
	HandleKillFocus( m_spinBeat, m_Marker.m_lBeat );
}


/////////////////////////////////////////////////////////////////////////////
// PropPageMarker::OnKillfocusEditMeasure

void PropPageMarker::OnKillfocusEditMeasure() 
{
	HandleKillFocus( m_spinMeasure, m_Marker.m_lMeasure );
}


/////////////////////////////////////////////////////////////////////////////
// PropPageMarker::HandleKillFocus
//
// Generic handler for KillFocus changes
void PropPageMarker::HandleKillFocus( CSpinButtonCtrl& spin, long& lUpdateVal )
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
// PropPageMarker::UpdateObject

void PropPageMarker::UpdateObject( void )
{
	// Check for a valid Property Page Object pointer
	if (m_pPropPageMgr && m_pPropPageMgr->m_pIPropPageObject)
	{
		// Update the Property Page Object with the new Marker
		m_pPropPageMgr->m_pIPropPageObject->SetData((void *) &m_Marker);
	}
}


/////////////////////////////////////////////////////////////////////////////
// PropPageMarker::EnableControls

void PropPageMarker::EnableControls( BOOL fEnable )
{
	// Check if the Beat edit control exists
	if( m_editBeat.GetSafeHwnd() == NULL )
	{
		return;
	}

	// Update the beat, measure, and text controls with the new window state
	m_spinTick.EnableWindow( fEnable );
	m_spinGrid.EnableWindow( fEnable );
	m_spinBeat.EnableWindow( fEnable );
	m_spinMeasure.EnableWindow( fEnable );
	m_editTick.EnableWindow( fEnable );
	m_editGrid.EnableWindow( fEnable );
	m_editBeat.EnableWindow( fEnable );
	m_editMeasure.EnableWindow( fEnable );
	m_editMarker.EnableWindow( fEnable );

	// Check if we're disabling
	if( !fEnable )
	{
		// Check if multiple items are selected
		if( m_Marker.m_dwBitsUI & UD_MULTIPLESELECT )
		{
			// Load the 'multiple select' text
			CString strText;
			if( strText.LoadString( IDS_MULTIPLE_SELECT ) )
			{
				// Display the 'multiple select' text
				m_editMarker.SetWindowText( strText );
				return;
			}
		}

		// No items are selected, so clear the edit controls
		m_editMarker.SetWindowText( NULL );
		m_editTick.SetWindowText( NULL );
		m_editGrid.SetWindowText( NULL );
		m_editBeat.SetWindowText( NULL );
		m_editMeasure.SetWindowText( NULL );
	}
}


/////////////////////////////////////////////////////////////////////////////
// PropPageMarker::OnOK

// Windows translated presses of the 'Enter' key into presses of the button 'OK'.
// Here we create a hidden 'OK' button and translate presses of it into tabs to the next
// control in the property page.
void PropPageMarker::OnOK() 
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

void PropPageMarker::OnDeltaposSpinTick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// Need to do this in case the user clicked the spin control immediately after
	// typing in a value
	OnKillfocusEditTick();

	HandleDeltaChange( pNMHDR, pResult, m_Marker.m_lTick );
}

void PropPageMarker::OnKillfocusEditTick() 
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
		if( m_Marker.m_lTick != 0 )
		{
			// Update the item with the new value
			m_Marker.m_lTick = 0;

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
			SetDlgItemInt( IDC_EDIT_TICK, m_Marker.m_lTick );
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
			if( m_Marker.m_lTick != lNewValue )
			{
				// Update the item with the new value
				m_Marker.m_lTick = lNewValue;

				// Now, update the object with the new value
				UpdateObject();
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// PropPageMarker::OnKillfocusEditGrid

void PropPageMarker::OnKillfocusEditGrid() 
{
	HandleKillFocus( m_spinGrid, m_Marker.m_lGrid );
}


/////////////////////////////////////////////////////////////////////////////
// PropPageMarker::OnDeltaposSpinGrid

void PropPageMarker::OnDeltaposSpinGrid(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// Need to do this in case the user clicked the spin control immediately after
	// typing in a value
	OnKillfocusEditGrid();

	HandleDeltaChange( pNMHDR, pResult, m_Marker.m_lGrid );
}
