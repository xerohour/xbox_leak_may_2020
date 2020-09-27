// CTabVariations.cpp : implementation file
//

#include "stdafx.h"
#include <RiffStrm.h>
#include "TrackMgr.h"
#include "TrackItem.h"
#include "PropTrackItem.h"
#include "PropPageMgr_Item.h"
#include "TabVariations.h"
#include "LockoutNotification.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


static const TCHAR g_pstrUnknown[11] = _T("----------");


/////////////////////////////////////////////////////////////////////////////
// CTabVariations property page

CTabVariations::CTabVariations() : CPropertyPage(CTabVariations::IDD)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	//{{AFX_DATA_INIT(CTabVariations)
	//}}AFX_DATA_INIT

	m_pPropPageMgr = NULL;
	m_fValidTrackItem = FALSE;
	m_fNeedToDetach = FALSE;
	m_fAllDisabled = TRUE;
}

CTabVariations::~CTabVariations()
{
}

void CTabVariations::DoDataExchange(CDataExchange* pDX)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTabVariations)
	//}}AFX_DATA_MAP
}


/////////////////////////////////////////////////////////////////////////////
// CTabVariations::EnableControls

void CTabVariations::EnableControls( BOOL fEnable )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Make sure controls have been created
	if( ::IsWindow(m_hWnd) == FALSE )
	{
		return;
	}

	CWnd* pWnd;
	
	for( int i = IDC_CHECK_VAR1 ;  i <= IDC_CHECK_VAR32 ;  i++ )
	{
		pWnd = GetDlgItem( i );
		if( pWnd )
		{
			if( fEnable == FALSE )
			{
				CheckDlgButton( i, 0 );
			}

			pWnd->EnableWindow( fEnable );
		}
	}

	pWnd = GetDlgItem( IDC_WAVE_NAME );
	if( pWnd )
	{
		pWnd->EnableWindow( fEnable );
	}

	pWnd = GetDlgItem( IDC_PROMPT );
	if( pWnd )
	{
		pWnd->EnableWindow( fEnable );
	}

	if( fEnable == FALSE )
	{
		pWnd = GetDlgItem( IDC_WAVE_NAME );
		if( pWnd )
		{
			pWnd->SetWindowText( NULL );
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabVariations::SetPropTrackItem

void CTabVariations::SetPropTrackItem( const CPropTrackItem* pPropItem )
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

	// Flag that we have a valid item
	m_fValidTrackItem = TRUE;

	// Copy the information from the new item
	m_PropItem.Copy( pPropItem );
	m_PropItem.m_nPropertyTab = TAB_VARIATIONS;
	m_PropItem.m_dwChanged = 0;

	// Make sure controls have been created
	if( ::IsWindow(m_hWnd) == FALSE )
	{
		return;
	}

	// Don't send OnKill/OnUpdate notifications when updating the display
	CLockoutNotification LockoutNotifications( m_hWnd );

	// Update the property page based on the new data.
	// It is usually profitable to check to see if the data actually changed
	// before updating the controls in the property page.

	// Enable all the controls
	EnableControls( TRUE );

	// Set prompt
	CWnd* pWnd = GetDlgItem( IDC_WAVE_NAME );
	if( pWnd )
	{
		CString strWaveName;
		if( m_PropItem.m_dwBits & PROPF_MULTIPLESELECT )
		{
			strWaveName = g_pstrUnknown;
		}
		else
		{
			m_PropItem.m_Item.FormatUIText( strWaveName );
		}
		pWnd->SetWindowText( strWaveName );
	}

	// Set variation check boxes
	int nState;
	int j;

	for( int i = IDC_CHECK_VAR1 ;  i <= IDC_CHECK_VAR32 ;  i++ )
	{
		pWnd = GetDlgItem( i );
		if( pWnd )
		{
			j = i - IDC_CHECK_VAR1;

			if( m_PropItem.m_dwUndetermined_TabVariations & (1 << j) )
			{
				nState = 2;
			}
			else if( m_PropItem.m_Item.m_dwVariations & (1 << j) )
			{
				nState = 1;
			}
			else
			{
				nState = 0;
			}

			CheckDlgButton( i, nState );
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabVariations::UpdateObject

void CTabVariations::UpdateObject( void )
{
	// Check for a valid Property Page Object pointer
	if( m_pPropPageMgr
	&&  m_pPropPageMgr->m_pIPropPageObject )
	{
		// Update the Property Page Object with the new item
		m_pPropPageMgr->m_pIPropPageObject->SetData( (void *)&m_PropItem );
	}
}


BEGIN_MESSAGE_MAP(CTabVariations, CPropertyPage)
	//{{AFX_MSG_MAP(CTabVariations)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTabVariations message handlers

BOOL CTabVariations::OnSetActive() 
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
// CTabVariations::OnCreate

int CTabVariations::OnCreate(LPCREATESTRUCT lpCreateStruct) 
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


/////////////////////////////////////////////////////////////////////////////
// CTabVariations::OnDestroy

void CTabVariations::OnDestroy() 
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
// CTabVariations::OnCommand

BOOL CTabVariations::OnCommand( WPARAM wParam, LPARAM lParam ) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	int nControlID = LOWORD( wParam );
	int nCommandID = HIWORD( wParam );

	if( nCommandID == BN_CLICKED
	||  nCommandID == BN_DOUBLECLICKED )
	{
		if( nControlID >= IDC_CHECK_VAR1
		&&  nControlID <= IDC_CHECK_VAR32 )
		{
			CWnd* pWnd = GetDlgItem( nControlID );
			if( pWnd )
			{
				int nVariation = nControlID - IDC_CHECK_VAR1;

				switch( IsDlgButtonChecked( nControlID ) )
				{
					case 0:		// Currently unchecked
					case 2:		// Currently undetermined
						CheckDlgButton( nControlID, 1 );
						m_PropItem.m_Item.m_dwVariations |= (1 << nVariation);
						break;

					case 1:		// Currently checked
						CheckDlgButton( nControlID, 0 );
						m_PropItem.m_Item.m_dwVariations &= ~(1 << nVariation);
						break;
				}

				m_PropItem.m_dwChanged = CHGD_VARIATIONS;
				m_PropItem.m_dwChangedVar_TabVariations = (1 << nVariation);
				UpdateData(FALSE);
				UpdateObject();
			}

			return TRUE;
		}
	}
	
	return CPropertyPage::OnCommand( wParam, lParam );
}
