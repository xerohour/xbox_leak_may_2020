// PropPageMarker.cpp : implementation file
//

#include "stdafx.h"
#include "midistripmgr.h"
#include "PropPageMgr.h"
#include "PropPageMarker.h"
#include "MIDIStripMgrApp.h"
#include "VarSwitchStrip.h"
#include "MIDIMgr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


static const TCHAR achUnknown[11] = "----------";

short CMarkerPropPageMgr::sm_nActiveTab = 0;


//////////////////////////////////////////////////////////////////////
// CMarkerPropPageMgr Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMarkerPropPageMgr::CMarkerPropPageMgr( REFGUID rGUIDManager )
{
	m_pTabMarker = NULL;
	m_GUIDManager = rGUIDManager;
}

CMarkerPropPageMgr::~CMarkerPropPageMgr()
{
	if( m_pTabMarker )
	{
		delete m_pTabMarker;
		m_pTabMarker = NULL;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CMarkerPropPageMgr::RemoveCurrentObject

void CMarkerPropPageMgr::RemoveCurrentObject( void )
{
	if( m_pIPropPageObject == NULL )
	{
		return;
	}

	m_pIPropPageObject->OnRemoveFromPageManager();
	m_pIPropPageObject = NULL;
}


/////////////////////////////////////////////////////////////////////////////
// CMarkerPropPageMgr IDMUSProdPropPageManager::GetPropertySheetTitle

HRESULT CMarkerPropPageMgr::GetPropertySheetTitle( BSTR* pbstrTitle, BOOL* pfAddPropertiesText )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( (pbstrTitle == NULL)
	||  (pfAddPropertiesText == NULL) )
	{
		return E_POINTER;
	}

	// Get "Var Switch Point" text
	CString strVarSwitch;
	strVarSwitch.LoadString( IDS_VARIATION_SWITCH_TITLE );

	// Format title
	CString strTitle = strVarSwitch;
	
	if( m_pIPropPageObject )
	{
		CVarSwitchStrip* pVarSwitchStrip = (CVarSwitchStrip *)m_pIPropPageObject;

		if( pVarSwitchStrip->m_pMIDIMgr
		&&	!pVarSwitchStrip->m_pMIDIMgr->m_pIDMTrack
		&&  pVarSwitchStrip->m_pPianoRollStrip
		&&  pVarSwitchStrip->m_pPianoRollStrip->m_pPartRef
		&&  pVarSwitchStrip->m_pPianoRollStrip->m_pPartRef->m_pPattern )
		{
			// Get Style Name
			CString strStyleName;
			BSTR bstrStyleName;
			if( pVarSwitchStrip->m_pMIDIMgr->m_pIStyleNode )
			{
				if( SUCCEEDED ( pVarSwitchStrip->m_pMIDIMgr->m_pIStyleNode->GetNodeName( &bstrStyleName ) ) )
				{
					strStyleName = bstrStyleName;
					::SysFreeString( bstrStyleName );
				}
			}

			// Put together the title
			if( strStyleName.IsEmpty() == FALSE )
			{
				strTitle = strStyleName +
						   _T(" - " );
			}
			strTitle += pVarSwitchStrip->m_pPianoRollStrip->m_pPartRef->m_pPattern->m_strName +
					    _T(" - ") +
					    pVarSwitchStrip->m_pPianoRollStrip->m_pPartRef->m_strName +
					    _T(" - ") +
					    strVarSwitch;
		}
	}

	*pbstrTitle = strTitle.AllocSysString();
	*pfAddPropertiesText = TRUE;
		
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CMarkerPropPageMgr IDMUSProdPropPageManager::GetPropertySheetPages

HRESULT CMarkerPropPageMgr::GetPropertySheetPages( IDMUSProdPropSheet* pIPropSheet, LONG* hPropSheetPage[], short* pnNbrPages )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( (hPropSheetPage == NULL)
	||  (pnNbrPages == NULL) )
	{
		return E_POINTER;
	}

	if( pIPropSheet == NULL )
	{
		return E_INVALIDARG;
	}

	m_pIPropSheet = pIPropSheet;
	m_pIPropSheet->AddRef();

	hPropSheetPage[0] = NULL;
	*pnNbrPages = 0;

	HPROPSHEETPAGE hPage;
	short nNbrPages = 0;

	// Add Marker tab
	m_pTabMarker = new CPropPageMarker( this );
	if( m_pTabMarker )
	{
		hPage = ::CreatePropertySheetPage( (LPCPROPSHEETPAGE)&m_pTabMarker->m_psp );
		if( hPage )
		{
			hPropSheetPage[nNbrPages] = (LONG *)hPage;
			nNbrPages++;
		}			
	}

	// Set number of pages
	*pnNbrPages = nNbrPages;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CMarkerPropPageMgr IDMUSProdPropPageManager::OnRemoveFromPropertySheet

HRESULT CMarkerPropPageMgr::OnRemoveFromPropertySheet()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_pIPropSheet )
	{
		m_pIPropSheet->GetActivePage( &CMarkerPropPageMgr::sm_nActiveTab );
	}

	CStaticPropPageManager::OnRemoveFromPropertySheet();

	theApp.m_pIPageManager = NULL;

	Release();	// delete myself

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CMarkerPropPageMgr IDMUSProdPropPageManager::RefreshData

HRESULT CMarkerPropPageMgr::RefreshData( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	CPropSwitchPoint propSwitchPoint;
	CPropSwitchPoint* pPropSwitchPoint = &propSwitchPoint;

	if( ( m_pIPropPageObject == NULL )
	||  ( m_pIPropPageObject->GetData( (void **)&pPropSwitchPoint ) != S_OK ) )
	{
		pPropSwitchPoint = NULL;
	}

	// Save a pointer to the window that has focus
	CWnd* pWndHadFocus = CWnd::GetFocus();

	m_PropSwitchPoint.Copy( pPropSwitchPoint );

	// Set Property tabs to display the new Marker(s)
	m_pTabMarker->UpdateControls( pPropSwitchPoint );

	// Restore focus
	if( pWndHadFocus
	&&  pWndHadFocus != CWnd::GetFocus() )
	{
		pWndHadFocus->SetFocus();
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CMarkerPropPageMgr IDMUSProdPropPageManager::SetObject

HRESULT CMarkerPropPageMgr::SetObject( IDMUSProdPropPageObject* pINewPropPageObject )
{
	if( pINewPropPageObject == NULL )
	{
		return E_INVALIDARG;
	}

	if( IsEqualObject( pINewPropPageObject ) != S_OK )
	{
		RemoveCurrentObject();

		m_pIPropPageObject = pINewPropPageObject;
//		m_pIPropPageObject->AddRef();		intentionally missing

		if( m_pIPropSheet )
		{
			m_pIPropSheet->RefreshTitle();
		}
	}

	RefreshData();

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CPropPageMarker property page

CPropPageMarker::CPropPageMarker( CMarkerPropPageMgr *pMarkerPropPageMgr ) : CPropertyPage(CPropPageMarker::IDD)
{
	//{{AFX_DATA_INIT(CPropPageMarker)
	//}}AFX_DATA_INIT
	
	ASSERT( pMarkerPropPageMgr != NULL );

	m_pPageManager = pMarkerPropPageMgr;
	m_fInUpdateControls = FALSE;
	m_fNeedToDetach = FALSE;
}

CPropPageMarker::~CPropPageMarker()
{
}

void CPropPageMarker::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPropPageMarker)
	DDX_Control(pDX, IDC_CHECK_ALIGN_START, m_btnEnterChord);
	DDX_Control(pDX, IDC_CHECK_STOP, m_btnExit);
	DDX_Control(pDX, IDC_CHECK_START, m_btnEnter);
	DDX_Control(pDX, IDC_CHECK_ALIGN_STOP, m_btnExitChord);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPropPageMarker, CPropertyPage)
	//{{AFX_MSG_MAP(CPropPageMarker)
	ON_WM_DESTROY()
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropPageMarker message handlers

int CPropPageMarker::OnCreate(LPCREATESTRUCT lpCreateStruct) 
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

	if( CPropertyPage::OnCreate(lpCreateStruct) == -1 )
	{
		return -1;
	}
	
	return 0;
}

void CPropPageMarker::OnDestroy() 
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

BOOL CPropPageMarker::OnSetActive() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Update controls
	m_pPageManager->RefreshData();

	// Store active tab
	m_pPageManager->m_pIPropSheet->GetActivePage( &CMarkerPropPageMgr::sm_nActiveTab );
	
	return CPropertyPage::OnSetActive();
}

void CPropPageMarker::UpdateControls( CPropSwitchPoint* pPropSwitchPoint )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	ASSERT( m_pPageManager != NULL );
	
	// Make sure controls have been created
	if( ::IsWindow(m_btnEnterChord.m_hWnd) == FALSE )
	{
		return;
	}

	m_fInUpdateControls = TRUE;

	// Update controls
	if( pPropSwitchPoint == NULL )
	{
		EnableControls( FALSE );

		m_btnEnter.SetCheck( 0 );
		m_btnEnterChord.SetCheck( 0 );
		m_btnExit.SetCheck( 0 );
		m_btnExitChord.SetCheck( 0 );

		for( int i = IDC_CHECK_VAR1 ;  i <= IDC_CHECK_VAR32 ;  i++ )
		{
			CWnd *pWnd = GetDlgItem( i );
			if( pWnd )
			{
				CheckDlgButton( i, 0 );
			}
		}

		m_fInUpdateControls = FALSE;
		return;
	}

	EnableControls( TRUE );

	m_btnEnter.SetCheck( (pPropSwitchPoint->m_dwUndetermined & UNDT_ENTER) ?
		2 : ((pPropSwitchPoint->m_wEnterMarkerFlags & DMUS_MARKERF_START) ? 1 : 0) );

	m_btnEnterChord.SetCheck( (pPropSwitchPoint->m_dwUndetermined & UNDT_ENTERCHORD) ?
		2 : ((pPropSwitchPoint->m_wEnterMarkerFlags & DMUS_MARKERF_CHORD_ALIGN) ? 1 : 0) );

	m_btnExit.SetCheck( (pPropSwitchPoint->m_dwUndetermined & UNDT_EXIT) ?
		2 : ((pPropSwitchPoint->m_wExitMarkerFlags & DMUS_MARKERF_STOP) ? 1 : 0) );

	m_btnExitChord.SetCheck( (pPropSwitchPoint->m_dwUndetermined & UNDT_EXITCHORD) ?
		2 : ((pPropSwitchPoint->m_wExitMarkerFlags & DMUS_MARKERF_CHORD_ALIGN) ? 1 : 0) );

	for( int i = IDC_CHECK_VAR1 ;  i <= IDC_CHECK_VAR32 ;  i++ )
	{
		CWnd *pWnd = GetDlgItem( i );
		if( pWnd )
		{
			CheckDlgButton( i, (pPropSwitchPoint->m_dwVarUndetermined & (1 << (i - IDC_CHECK_VAR1))) ?
				2 : ((pPropSwitchPoint->m_dwVariation & (1 << (i - IDC_CHECK_VAR1))) ? 1 : 0) );
		}
	}

	m_fInUpdateControls = FALSE;
}

void CPropPageMarker::EnableControls( BOOL fEnable )
{
	m_btnEnter.EnableWindow( fEnable );
	m_btnEnterChord.EnableWindow( fEnable && (m_pPageManager->m_PropSwitchPoint.m_wEnterMarkerFlags & DMUS_MARKERF_START) );
	m_btnExit.EnableWindow( fEnable );
	m_btnExitChord.EnableWindow( fEnable && (m_pPageManager->m_PropSwitchPoint.m_wExitMarkerFlags & DMUS_MARKERF_STOP) );

	for( int i = IDC_CHECK_VAR1 ;  i <= IDC_CHECK_VAR32 ;  i++ )
	{
		CWnd *pWnd = GetDlgItem( i );
		if( pWnd )
		{
			pWnd->EnableWindow( fEnable );
		}
	}
}

BOOL CPropPageMarker::OnCommand(WPARAM wParam, LPARAM lParam) 
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
			// Verify that the checkbox actually exists
			if( GetDlgItem( nControlID ) )
			{
				int nVariation = nControlID - IDC_CHECK_VAR1;

				switch( IsDlgButtonChecked( nControlID ) )
				{
					case 0:		// Currently unchecked
					case 2:		// Currently undetermined
						CheckDlgButton( nControlID, 1 );
						m_pPageManager->m_PropSwitchPoint.m_dwVariation = (1 << nVariation);
						m_pPageManager->m_PropSwitchPoint.m_dwChanged = CHGD_SELVARIATION;
						UpdatePPO();
						break;

					case 1:		// Currently checked
						CheckDlgButton( nControlID, 0 );
						m_pPageManager->m_PropSwitchPoint.m_dwVariation = (1 << nVariation);
						m_pPageManager->m_PropSwitchPoint.m_dwChanged = CHGD_UNSELVARIATION;
						UpdatePPO();
						break;
				}
			}

			return TRUE;
		}

		// Start checkbox
		if( nControlID == IDC_CHECK_START )
		{
			// Verify that the checkbox actually exists
			if( GetDlgItem( IDC_CHECK_START ) )
			{
				switch( IsDlgButtonChecked( IDC_CHECK_START ) )
				{
					case 0:		// Currently unchecked
					case 2:		// Currently undetermined
						CheckDlgButton( IDC_CHECK_START, 1 );
						m_pPageManager->m_PropSwitchPoint.m_wEnterMarkerFlags |= DMUS_MARKERF_START;
						m_pPageManager->m_PropSwitchPoint.m_dwUndetermined = DWORD(~UNDT_ENTER);
						m_pPageManager->m_PropSwitchPoint.m_dwChanged = CHGD_ENTER;
						UpdatePPO();
						break;

					case 1:		// Currently checked
						CheckDlgButton( IDC_CHECK_START, 0 );
						m_pPageManager->m_PropSwitchPoint.m_wEnterMarkerFlags &= ~DMUS_MARKERF_START;
						m_pPageManager->m_PropSwitchPoint.m_dwUndetermined = DWORD(~UNDT_ENTER);
						m_pPageManager->m_PropSwitchPoint.m_dwChanged = CHGD_ENTER;
						UpdatePPO();
						break;
				}
			}
			return TRUE;
		}

		// Align start checkbox
		if( nControlID == IDC_CHECK_ALIGN_START )
		{
			// Verify that the checkbox actually exists
			if( GetDlgItem( IDC_CHECK_ALIGN_START ) )
			{
				switch( IsDlgButtonChecked( IDC_CHECK_ALIGN_START ) )
				{
					case 0:		// Currently unchecked
					case 2:		// Currently undetermined
						CheckDlgButton( IDC_CHECK_ALIGN_START, 1 );
						m_pPageManager->m_PropSwitchPoint.m_wEnterMarkerFlags |= DMUS_MARKERF_CHORD_ALIGN;
						m_pPageManager->m_PropSwitchPoint.m_dwUndetermined = DWORD(~UNDT_ENTERCHORD);
						m_pPageManager->m_PropSwitchPoint.m_dwChanged = CHGD_ENTERCHORD;
						UpdatePPO();
						break;

					case 1:		// Currently checked
						CheckDlgButton( IDC_CHECK_ALIGN_START, 0 );
						m_pPageManager->m_PropSwitchPoint.m_wEnterMarkerFlags &= ~DMUS_MARKERF_CHORD_ALIGN;
						m_pPageManager->m_PropSwitchPoint.m_dwUndetermined = DWORD(~UNDT_ENTERCHORD);
						m_pPageManager->m_PropSwitchPoint.m_dwChanged = CHGD_ENTERCHORD;
						UpdatePPO();
						break;
				}
			}
			return TRUE;
		}

		// Stop checkbox
		if( nControlID == IDC_CHECK_STOP )
		{
			// Verify that the checkbox actually exists
			if( GetDlgItem( IDC_CHECK_STOP ) )
			{
				switch( IsDlgButtonChecked( IDC_CHECK_STOP ) )
				{
					case 0:		// Currently unchecked
					case 2:		// Currently undetermined
						CheckDlgButton( IDC_CHECK_STOP, 1 );
						m_pPageManager->m_PropSwitchPoint.m_wExitMarkerFlags |= DMUS_MARKERF_STOP;
						m_pPageManager->m_PropSwitchPoint.m_dwUndetermined = DWORD(~UNDT_EXIT);
						m_pPageManager->m_PropSwitchPoint.m_dwChanged = CHGD_EXIT;
						UpdatePPO();
						break;

					case 1:		// Currently checked
						CheckDlgButton( IDC_CHECK_STOP, 0 );
						m_pPageManager->m_PropSwitchPoint.m_wExitMarkerFlags &= ~DMUS_MARKERF_STOP;
						m_pPageManager->m_PropSwitchPoint.m_dwUndetermined = DWORD(~UNDT_EXIT);
						m_pPageManager->m_PropSwitchPoint.m_dwChanged = CHGD_EXIT;
						UpdatePPO();
						break;
				}
			}
			return TRUE;
		}

		// Align stop checkbox
		if( nControlID == IDC_CHECK_ALIGN_STOP )
		{
			// Verify that the checkbox actually exists
			if( GetDlgItem( IDC_CHECK_ALIGN_STOP ) )
			{
				switch( IsDlgButtonChecked( IDC_CHECK_ALIGN_STOP ) )
				{
					case 0:		// Currently unchecked
					case 2:		// Currently undetermined
						CheckDlgButton( IDC_CHECK_ALIGN_STOP, 1 );
						m_pPageManager->m_PropSwitchPoint.m_wExitMarkerFlags |= DMUS_MARKERF_CHORD_ALIGN;
						m_pPageManager->m_PropSwitchPoint.m_dwUndetermined = DWORD(~UNDT_EXITCHORD);
						m_pPageManager->m_PropSwitchPoint.m_dwChanged = CHGD_EXITCHORD;
						UpdatePPO();
						break;

					case 1:		// Currently checked
						CheckDlgButton( IDC_CHECK_ALIGN_STOP, 0 );
						m_pPageManager->m_PropSwitchPoint.m_wExitMarkerFlags &= ~DMUS_MARKERF_CHORD_ALIGN;
						m_pPageManager->m_PropSwitchPoint.m_dwUndetermined = DWORD(~UNDT_EXITCHORD);
						m_pPageManager->m_PropSwitchPoint.m_dwChanged = CHGD_EXITCHORD;
						UpdatePPO();
						break;
				}
			}
			return TRUE;
		}
	}
	
	return CPropertyPage::OnCommand( wParam, lParam );
}

void CPropPageMarker::UpdatePPO( void )
{
	if( m_pPageManager->m_pIPropPageObject )
	{
		m_pPageManager->m_pIPropPageObject->SetData( &(m_pPageManager->m_PropSwitchPoint) );
	}
}
