// TabBoundary.cpp : implementation file
//

#include "stdafx.h"
#include <RiffStrm.h>
#include "TrackMgr.h"
#include "TrackItem.h"
#include "PropPageMgr_Item.h"
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


/////////////////////////////////////////////////////////////////////////////
// CTabBoundary property page

IMPLEMENT_DYNCREATE(CTabBoundary, CPropertyPage)

/////////////////////////////////////////////////////////////////////////////
// CTabBoundary constructor/destructor

CTabBoundary::CTabBoundary(): CPropertyPage(CTabBoundary::IDD)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

//	CPropertyPage(IDD_PROPPAGE_FILEREF);
	//{{AFX_DATA_INIT(CTabBoundary)
	//}}AFX_DATA_INIT

	// Initialize our pointers to NULL
	m_pPropPageMgr = NULL;
	m_pTimeline = NULL;

	// Initialize our class members to FALSE
	m_fValidTrackItem = FALSE;
	m_fNeedToDetach = FALSE;
}

CTabBoundary::~CTabBoundary()
{
}


/////////////////////////////////////////////////////////////////////////////
// CTabBoundary::DoDataExchange

void CTabBoundary::DoDataExchange(CDataExchange* pDX)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	CPropertyPage::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(CTabBoundary)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTabBoundary, CPropertyPage)
	//{{AFX_MSG_MAP(CTabBoundary)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTabBoundary custom functions

/////////////////////////////////////////////////////////////////////////////
// CTabBoundary::SetTrackItem

void CTabBoundary::SetTrackItem( const CTrackItem* pItem )
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

	// Check to see if the controls have been created
//	if( m_editAttenuation.GetSafeHwnd() == NULL )
//	{
		// It doesn't have a valid window handle - the property page may have been destroyed (or not yet created).
		// Just exit early
		return;
//	}

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

	// Set xxxx control
}


/////////////////////////////////////////////////////////////////////////////
// CTabBoundary message handlers

/////////////////////////////////////////////////////////////////////////////
// CTabBoundary::OnCreate

int CTabBoundary::OnCreate(LPCREATESTRUCT lpCreateStruct) 
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
// CTabBoundary::OnDestroy

void CTabBoundary::OnDestroy() 
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
// CTabBoundary::OnInitDialog

BOOL CTabBoundary::OnInitDialog() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Call the base class's OnInitDialog method
	CPropertyPage::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
	               // EXCEPTION: OCX Property Pages should return FALSE
}


/////////////////////////////////////////////////////////////////////////////
// CTabBoundary::UpdateObject

void CTabBoundary::UpdateObject( void )
{
	// Check for a valid Property Page Object pointer
	if( m_pPropPageMgr && m_pPropPageMgr->m_pIPropPageObject )
	{
		// Update the Property Page Object with the new item
		m_pPropPageMgr->m_pIPropPageObject->SetData( (void *) &m_TrackItem );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabBoundary::EnableControls

void CTabBoundary::EnableControls( BOOL fEnable )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Make sure controls have been created
//	if( ::IsWindow(m_editAttenuation.m_hWnd) == FALSE )
//	{
//		return;
//	}

//	m_sliderAttenuation.EnableWindow(fEnable);
}


/////////////////////////////////////////////////////////////////////////////
// CTabBoundary::OnOK

// Windows translated presses of the 'Enter' key into presses of the button 'OK'.
// Here we create a hidden 'OK' button and translate presses of it into tabs to the next
// control in the property page.
void CTabBoundary::OnOK() 
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
// CTabBoundary::OnSetActive

BOOL CTabBoundary::OnSetActive() 
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
		m_pPropPageMgr->m_pIPropSheet->GetActivePage( &CPropPageMgrItem::sm_nActiveTab );
	}
	
	return CPropertyPage::OnSetActive();
}
