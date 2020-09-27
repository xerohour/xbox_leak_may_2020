// PropPage$$Safe_root$$.cpp : implementation file
//

#include "stdafx.h"
#include "$$Safe_root$$Item.h"
#include "PropPageMgr.h"
#include "PropPage$$Safe_root$$.h"
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
// C$$Safe_root$$PropPageMgr constructor/destructor

C$$Safe_root$$PropPageMgr::C$$Safe_root$$PropPageMgr()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Initialize our pointer to NULL
	m_pPropPage$$Safe_root$$ = NULL;

	// call the base class contstructor
	CStaticPropPageManager::CStaticPropPageManager();
}

C$$Safe_root$$PropPageMgr::~C$$Safe_root$$PropPageMgr()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// If the $$Safe_root$$ property page exists, delete it
	if( m_pPropPage$$Safe_root$$ )
	{
		delete m_pPropPage$$Safe_root$$;
		m_pPropPage$$Safe_root$$ = NULL;
	}

	// Call the base class destructor
	CStaticPropPageManager::~CStaticPropPageManager();
}


/////////////////////////////////////////////////////////////////////////////
// C$$Safe_root$$PropPageMgr IDMUSProdPropPageManager implementation

/////////////////////////////////////////////////////////////////////////////
// C$$Safe_root$$PropPageMgr::GetPropertySheetTitle

HRESULT STDMETHODCALLTYPE C$$Safe_root$$PropPageMgr::GetPropertySheetTitle( BSTR* pbstrTitle, 
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
	if( comBSTR.LoadString( IDS_PROPPAGE_$$SAFE_ROOT$$ ) )
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
// C$$Safe_root$$PropPageMgr::GetPropertySheetPages

HRESULT STDMETHODCALLTYPE C$$Safe_root$$PropPageMgr::GetPropertySheetPages( IDMUSProdPropSheet* pIPropSheet, 
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
	if( m_pPropPage$$Safe_root$$ == NULL )
	{
		m_pPropPage$$Safe_root$$ = new PropPage$$Safe_root$$();
	}

	// If the property page now exists, add the property page
	if( m_pPropPage$$Safe_root$$ )
	{
		// Copy the PROPSHEETPAGE structure
		PROPSHEETPAGE psp;
		memcpy( &psp, &m_pPropPage$$Safe_root$$->m_psp, sizeof(PROPSHEETPAGE) );

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
		m_pPropPage$$Safe_root$$->m_pPropPageMgr = this;
	}

	// Set number of pages
	*pnNbrPages = nNbrPages;
	return S_OK;
};


/////////////////////////////////////////////////////////////////////////////
// C$$Safe_root$$PropPageMgr::RefreshData

HRESULT STDMETHODCALLTYPE C$$Safe_root$$PropPageMgr::RefreshData()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Get a pointer to a C$$Safe_root$$Item
	C$$Safe_root$$Item* p$$Safe_root$$;

	if( m_pIPropPageObject == NULL )
	{
		// No property page object - set pointer to NULL
		p$$Safe_root$$ = NULL;
	}
	// Have a property page object - try and get a pointer from it
	else if( FAILED ( m_pIPropPageObject->GetData( (void **)&p$$Safe_root$$ ) ) )
	{
		return E_FAIL;
	}

	// Update the property page, if it exists
	if (m_pPropPage$$Safe_root$$ )
	{
		// p$$Safe_root$$ may be NULL, meaning nothing is selected
		m_pPropPage$$Safe_root$$->Set$$Safe_root$$( p$$Safe_root$$ );
		return S_OK;
	}
	else
	{
		return E_FAIL;
	}
};


/////////////////////////////////////////////////////////////////////////////
// PropPage$$Safe_root$$ property page

IMPLEMENT_DYNCREATE(PropPage$$Safe_root$$, CPropertyPage)

/////////////////////////////////////////////////////////////////////////////
// PropPage$$Safe_root$$ constructor/destructor

PropPage$$Safe_root$$::PropPage$$Safe_root$$(): CPropertyPage(PropPage$$Safe_root$$::IDD)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

//	CPropertyPage(IDD_$$SAFE_ROOT$$_PROPPAGE);
	//{{AFX_DATA_INIT(PropPage$$Safe_root$$)
	//}}AFX_DATA_INIT

	// Initialize our pointers to NULL
	m_pPropPageMgr = NULL;
	m_pTimeline = NULL;

	// Initialize our class members to FALSE
	m_fValid$$Safe_root$$ = FALSE;
	m_fNeedToDetach = FALSE;
}

PropPage$$Safe_root$$::~PropPage$$Safe_root$$()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
}


/////////////////////////////////////////////////////////////////////////////
// PropPage$$Safe_root$$::DoDataExchange

void PropPage$$Safe_root$$::DoDataExchange(CDataExchange* pDX)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	CPropertyPage::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(PropPage$$Safe_root$$)
	DDX_Control(pDX, IDC_EDIT_BEAT, m_editBeat);
	DDX_Control(pDX, IDC_EDIT_$$SAFE_ROOT$$, m_edit$$Safe_root$$);
	DDX_Control(pDX, IDC_EDIT_MEASURE, m_editMeasure);
	DDX_Control(pDX, IDC_SPIN_MEASURE, m_spinMeasure);
	DDX_Control(pDX, IDC_SPIN_BEAT, m_spinBeat);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(PropPage$$Safe_root$$, CPropertyPage)
	//{{AFX_MSG_MAP(PropPage$$Safe_root$$)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_BEAT, OnDeltaposSpinBeat)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_MEASURE, OnDeltaposSpinMeasure)
	ON_EN_KILLFOCUS(IDC_EDIT_BEAT, OnKillfocusEditBeat)
	ON_EN_KILLFOCUS(IDC_EDIT_$$SAFE_ROOT$$, OnKillfocusEdit$$Safe_root$$)
	ON_EN_KILLFOCUS(IDC_EDIT_MEASURE, OnKillfocusEditMeasure)
	ON_WM_KILLFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// PropPage$$Safe_root$$ custom functions

/////////////////////////////////////////////////////////////////////////////
// PropPage$$Safe_root$$::Set$$Safe_root$$

void PropPage$$Safe_root$$::Set$$Safe_root$$( const C$$Safe_root$$Item* p$$Safe_root$$ )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// If the pointer is null, there are no $$Safe_root$$s selected, so disable the property page
	if ( p$$Safe_root$$ == NULL )
	{
		m_fValid$$Safe_root$$ = FALSE;
		m_$$Safe_root$$.Clear();
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

	// Flag that we have a valid $$Safe_root$$
	m_fValid$$Safe_root$$ = TRUE;

		// Copy the information from the new $$Safe_root$$
	m_$$Safe_root$$.Copy( p$$Safe_root$$ );

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

		// If the $$Safe_root$$ is not in the last measure
		if( m_$$Safe_root$$.m_lMeasure < lMaxMeasure )
		{
			// Convert from a Measure value to a clock value
			long lClockForMeasure;
			m_pTimeline->MeasureBeatToClocks( dwGroupBits, 0, m_$$Safe_root$$.m_lMeasure, 0, &lClockForMeasure );

			// Get the TimeSig for this measure
			DMUS_TIMESIGNATURE TimeSig;
			if( SUCCEEDED( m_pTimeline->GetParam( GUID_TimeSignature, dwGroupBits, 0, lClockForMeasure, NULL, &TimeSig ) ) )
			{
				lMaxBeat = TimeSig.bBeatsPerMeasure - 1;
			}
		}
		// Else the $$Safe_root$$ is in the last measure and lMaxBeat is already set

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

	// Don't send OnKill/OnUpdate notifications when updating the display
	CLockoutNotification LockoutNotifications( m_hWnd );

	// Check if multiple items are selected
	if( m_$$Safe_root$$.m_dwBits & UD_MULTIPLESELECT )
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

	// Get the currently displayed text
	CString strTemp;
	m_edit$$Safe_root$$.GetWindowText( strTemp );

	// If it's different from what we were passed or the item was previously invalid, update the display
	if ( !m_fValid$$Safe_root$$ || (p$$Safe_root$$->m_strText != strTemp) )
	{
		m_edit$$Safe_root$$.SetWindowText( p$$Safe_root$$->m_strText );
	}

	// If the item was previously invalid, update the measure and beat display
	if( !m_fValid$$Safe_root$$ )
	{
		SetDlgItemInt( m_editMeasure.GetDlgCtrlID(), m_$$Safe_root$$.m_lMeasure + 1 );
		SetDlgItemInt( m_editBeat.GetDlgCtrlID(), m_$$Safe_root$$.m_lBeat + 1 );
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
		if( !fTransSuccess || (tcstrTmp[0] == NULL) || (lCurValue != m_$$Safe_root$$.m_lMeasure + 1) )
		{
			// Update the displayed measure number
			SetDlgItemInt( m_editMeasure.GetDlgCtrlID(), m_$$Safe_root$$.m_lMeasure + 1 );
		}


		// Get text from beat edit control
		m_editBeat.GetWindowText( tcstrTmp, DIALOG_EDIT_LEN );

		// Convert from text to an integer
		lCurValue = GetDlgItemInt( m_editBeat.GetDlgCtrlID(), &fTransSuccess, TRUE );

		// Check if the conversion failed, the text is empty, or if the values are different
		if( !fTransSuccess || (tcstrTmp[0] == NULL) || (lCurValue != m_$$Safe_root$$.m_lBeat + 1) )
		{
			// Update the displayed beat number
			SetDlgItemInt( m_editBeat.GetDlgCtrlID(), m_$$Safe_root$$.m_lBeat + 1 );
		}
	}

	// Update the range for the measure spin control
	m_spinMeasure.SetRange( 1, lMaxMeasure );

	// Update the range for the beat spin control
	m_spinBeat.SetRange( 1, lMaxBeat );
}


/////////////////////////////////////////////////////////////////////////////
// PropPage$$Safe_root$$ message handlers

/////////////////////////////////////////////////////////////////////////////
// PropPage$$Safe_root$$::OnCreate

int PropPage$$Safe_root$$::OnCreate(LPCREATESTRUCT lpCreateStruct) 
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
// PropPage$$Safe_root$$::OnDestroy

void PropPage$$Safe_root$$::OnDestroy() 
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
// PropPage$$Safe_root$$::OnInitDialog

BOOL PropPage$$Safe_root$$::OnInitDialog() 
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

	// If we have valid information
	if(	m_fValid$$Safe_root$$ )
	{
		// Force update of the controls
		m_fValid$$Safe_root$$ = FALSE;

		// Update the dialog
		Set$$Safe_root$$( &m_$$Safe_root$$ );
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	               // EXCEPTION: OCX Property Pages should return FALSE
}


/////////////////////////////////////////////////////////////////////////////
// PropPage$$Safe_root$$::OnDeltaposSpinBeat

void PropPage$$Safe_root$$::OnDeltaposSpinBeat(NMHDR* pNMHDR, LRESULT* pResult) 
{
	HandleDeltaChange( pNMHDR, pResult, m_$$Safe_root$$.m_lBeat );
}


/////////////////////////////////////////////////////////////////////////////
// PropPage$$Safe_root$$::OnDeltaposSpinMeasure

void PropPage$$Safe_root$$::OnDeltaposSpinMeasure(NMHDR* pNMHDR, LRESULT* pResult) 
{
	HandleDeltaChange( pNMHDR, pResult, m_$$Safe_root$$.m_lMeasure );
}


/////////////////////////////////////////////////////////////////////////////
// PropPage$$Safe_root$$::HandleDeltaChange
//
// Generic handler for deltapos changes
void PropPage$$Safe_root$$::HandleDeltaChange( NMHDR* pNMHDR,
									   LRESULT* pResult,
									   long& lUpdateVal )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_pPropPageMgr->m_pIPropPageObject == NULL )
	{
		return;
	}

	// If the value changed - update the selected $$Safe_root$$
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
// PropPage$$Safe_root$$::OnKillfocusEditBeat

void PropPage$$Safe_root$$::OnKillfocusEditBeat() 
{
	HandleKillFocus( m_spinBeat, m_$$Safe_root$$.m_lBeat );
}


/////////////////////////////////////////////////////////////////////////////
// PropPage$$Safe_root$$::OnKillfocusEditMeasure

void PropPage$$Safe_root$$::OnKillfocusEditMeasure() 
{
	HandleKillFocus( m_spinMeasure, m_$$Safe_root$$.m_lMeasure );
}


/////////////////////////////////////////////////////////////////////////////
// PropPage$$Safe_root$$::HandleKillFocus
//
// Generic handler for KillFocus changes
void PropPage$$Safe_root$$::HandleKillFocus( CSpinButtonCtrl& spin, long& lUpdateVal )
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
// PropPage$$Safe_root$$::OnKillfocusEdit$$Safe_root$$

void PropPage$$Safe_root$$::OnKillfocusEdit$$Safe_root$$() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Get the new text
	CString strText;
	m_edit$$Safe_root$$.GetWindowText( strText );

	// Check if the text is empty
	if( strText.IsEmpty() )
	{
		// Check if the existing text is empty
		if( m_$$Safe_root$$.m_strText != _T("") )
		{
			// Nope - update the $$Safe_root$$ strip manager with the new text
			m_$$Safe_root$$.m_strText = "";
			UpdateObject();
		}
	}
	else
	{
		// Check if the text is different from the existing text.
		if( strText != m_$$Safe_root$$.m_strText )
		{
			// Yep - update the $$Safe_root$$ strip manager with the new text
			m_$$Safe_root$$.m_strText = strText;
			UpdateObject();
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// PropPage$$Safe_root$$::UpdateObject

void PropPage$$Safe_root$$::UpdateObject( void )
{
	// Check for a valid Property Page Object pointer
	if (m_pPropPageMgr && m_pPropPageMgr->m_pIPropPageObject)
	{
		// Update the Property Page Object with the new $$Safe_root$$
		m_pPropPageMgr->m_pIPropPageObject->SetData((void *) &m_$$Safe_root$$);
	}
}


/////////////////////////////////////////////////////////////////////////////
// PropPage$$Safe_root$$::EnableControls

void PropPage$$Safe_root$$::EnableControls( BOOL fEnable )
{
	// Check if the Beat edit control exists
	if( m_editBeat.GetSafeHwnd() == NULL )
	{
		return;
	}

	// Update the beat, measure, and text controls with the new window state
	m_spinBeat.EnableWindow( fEnable );
	m_spinMeasure.EnableWindow( fEnable );
	m_editBeat.EnableWindow( fEnable );
	m_editMeasure.EnableWindow( fEnable );
	m_edit$$Safe_root$$.EnableWindow( fEnable );

	// Check if we're disabling
	if( !fEnable )
	{
		// Check if multiple items are selected
		if( m_$$Safe_root$$.m_dwBits & UD_MULTIPLESELECT )
		{
			// Load the 'multiple select' text
			CString strText;
			if( strText.LoadString( IDS_MULTIPLE_SELECT ) )
			{
				// Display the 'multiple select' text
				m_edit$$Safe_root$$.SetWindowText( strText );
				return;
			}
		}

		// No items are selected, so clear the edit controls
		m_edit$$Safe_root$$.SetWindowText( NULL );
		m_editBeat.SetWindowText( NULL );
		m_editMeasure.SetWindowText( NULL );
	}
}


/////////////////////////////////////////////////////////////////////////////
// PropPage$$Safe_root$$::OnOK

// Windows translated presses of the 'Enter' key into presses of the button 'OK'.
// Here we create a hidden 'OK' button and translate presses of it into tabs to the next
// control in the property page.
void PropPage$$Safe_root$$::OnOK() 
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
