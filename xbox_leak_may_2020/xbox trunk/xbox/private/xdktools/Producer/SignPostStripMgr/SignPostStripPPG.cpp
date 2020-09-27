// SignPostStripPPG.cpp : implementation file
//

#include "stdafx.h"
#include "SignPostStripPPG.h"
#include "GroupBitsPPG.h"
#include "TrackFlagsPPG.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

short CSignPostStripPropPageMgr::sm_nActiveTab = 0;


/////////////////////////////////////////////////////////////////////////////
// CSignPostStripPropPageMgr property page

CSignPostStripPropPageMgr::CSignPostStripPropPageMgr()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	m_pSignPostStripPPG = NULL;
	m_pGroupBitsPPG = NULL;
	m_pTrackFlagsPPG = NULL;
	CStaticPropPageManager::CStaticPropPageManager();
}

CSignPostStripPropPageMgr::~CSignPostStripPropPageMgr()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	if( m_pSignPostStripPPG )
	{
		delete m_pSignPostStripPPG;
		m_pSignPostStripPPG = NULL;
	}
	if( m_pGroupBitsPPG )
	{
		delete m_pGroupBitsPPG;
		m_pGroupBitsPPG = NULL;
	}
	if( m_pTrackFlagsPPG )
	{
		delete m_pTrackFlagsPPG;
		m_pTrackFlagsPPG = NULL;
	}
	CStaticPropPageManager::~CStaticPropPageManager();
}

HRESULT CSignPostStripPropPageMgr::QueryInterface( REFIID riid, LPVOID *ppv )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	return CStaticPropPageManager::QueryInterface( riid, ppv );
}

HRESULT CSignPostStripPropPageMgr::GetPropertySheetTitle( BSTR* pbstrTitle, 
	BOOL* pfAddPropertiesText )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	if( (pbstrTitle == NULL)
	||  (pfAddPropertiesText == NULL) )
	{
		return E_POINTER;
	}

	*pfAddPropertiesText = TRUE;

	CString cstrPart;
	cstrPart.LoadString( IDS_PROPPAGE_GROUP_BITS );
	*pbstrTitle = cstrPart.AllocSysString();

	return S_OK;
}

HRESULT CSignPostStripPropPageMgr::GetPropertySheetPages( IDMUSProdPropSheet* pIPropSheet, 
	LONG* hPropSheetPage[], short* pnNbrPages )
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
	// Add Group Bits tab
	if( NULL == m_pGroupBitsPPG )
	{
		m_pGroupBitsPPG = new CGroupBitsPPG();
	}
	if( m_pGroupBitsPPG )
	{
		hPage = ::CreatePropertySheetPage( (LPCPROPSHEETPAGE)&m_pGroupBitsPPG->m_psp );
		if( hPage )
		{
			hPropSheetPage[nNbrPages] = (LONG *)hPage;
			nNbrPages++;
		}

		// Weak reference
		m_pGroupBitsPPG->m_pIPropSheet = m_pIPropSheet;

		// Tell the property page to update the active tab setting
		CGroupBitsPPG::sm_pnActiveTab = &CSignPostStripPropPageMgr::sm_nActiveTab;
	}

	// Add track flags tab
	if( NULL == m_pTrackFlagsPPG )
	{
		m_pTrackFlagsPPG = new CTrackFlagsPPG();
	}
	if( m_pTrackFlagsPPG )
	{
		hPage = ::CreatePropertySheetPage( (LPCPROPSHEETPAGE)&m_pTrackFlagsPPG->m_psp );
		if( hPage )
		{
			hPropSheetPage[nNbrPages] = (LONG *)hPage;
			nNbrPages++;
		}			

		// Weak reference
		m_pTrackFlagsPPG->m_pIPropSheet = m_pIPropSheet;

		// Tell the property page to update the active tab setting
		CTrackFlagsPPG::sm_pnActiveTab = &CSignPostStripPropPageMgr::sm_nActiveTab;
	}

	// Add audition tab
	if( NULL == m_pSignPostStripPPG )
	{
		m_pSignPostStripPPG = new CSignPostStripPPG();
	}
	if( m_pSignPostStripPPG )
	{
		hPage = ::CreatePropertySheetPage( (LPCPROPSHEETPAGE)&m_pSignPostStripPPG->m_psp );
		if( hPage )
		{
			hPropSheetPage[nNbrPages] = (LONG *)hPage;
			nNbrPages++;
		}
		
		// Weak reference
		m_pSignPostStripPPG->m_pPropPageMgr = this;
	}

	// Set number of pages
	*pnNbrPages = nNbrPages;
	return S_OK;
}

HRESULT CSignPostStripPropPageMgr::RefreshData()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	if( m_pSignPostStripPPG )
	{
		m_pSignPostStripPPG->RefreshData();
	}
	if( m_pGroupBitsPPG )
	{
		m_pGroupBitsPPG->RefreshData();
	}
	if( m_pTrackFlagsPPG )
	{
		m_pTrackFlagsPPG->RefreshData();
	}
	return S_OK;
}

HRESULT CSignPostStripPropPageMgr::SetObject( IDMUSProdPropPageObject* pINewPropPageObject )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	if( m_pSignPostStripPPG )
	{
		m_pSignPostStripPPG->SetObject( pINewPropPageObject );
	}
	if( m_pGroupBitsPPG )
	{
		m_pGroupBitsPPG->SetObject( pINewPropPageObject );
	}
	if( m_pTrackFlagsPPG )
	{
		m_pTrackFlagsPPG->SetObject( pINewPropPageObject );
	}
	return CBasePropPageManager::SetObject( pINewPropPageObject );
}

/////////////////////////////////////////////////////////////////////////////
// CSignPostStripPPG property page

IMPLEMENT_DYNCREATE(CSignPostStripPPG, CPropertyPage)

CSignPostStripPPG::CSignPostStripPPG() : CPropertyPage(CSignPostStripPPG::IDD)
{
	//{{AFX_DATA_INIT(CSignPostStripPPG)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pPPO = NULL;
	m_pPropPageMgr = NULL;
	m_fNeedToDetach = FALSE;
	m_dwActivityLevel = 5;
}

CSignPostStripPPG::~CSignPostStripPPG()
{
	if( m_pPPO )
	{
		m_pPPO->Release();
	}
}

void CSignPostStripPPG::DoDataExchange(CDataExchange* pDX)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSignPostStripPPG)
	DDX_Control(pDX, IDC_ACTIVITYLEVEL, m_comboActivityLevel);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSignPostStripPPG, CPropertyPage)
	//{{AFX_MSG_MAP(CSignPostStripPPG)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_CBN_SELCHANGE(IDC_ACTIVITYLEVEL, OnSelchangeActivitylevel)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// private functions
void CSignPostStripPPG::SetObject( IDMUSProdPropPageObject* pPPO )
{
	if( m_pPPO )
	{
		m_pPPO->Release();
	}
	m_pPPO = pPPO;
	if( m_pPPO )
	{
		m_pPPO->AddRef();
	}
}

void CSignPostStripPPG::EnableControls( BOOL fEnable ) 
{
	if( ::IsWindow(m_hWnd) == FALSE )
	{
		return;
	}

	if( ::IsWindow(m_comboActivityLevel.m_hWnd))
	{
		m_comboActivityLevel.EnableWindow( fEnable );
	}
}

/////////////////////////////////////////////////////////////////////////////
// CSignPostStripPPG message handlers

int CSignPostStripPPG::OnCreate(LPCREATESTRUCT lpCreateStruct) 
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

void CSignPostStripPPG::OnDestroy() 
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

void CSignPostStripPPG::RefreshData( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_pPPO == NULL )
	{
		EnableControls( FALSE );
		return;
	}

	ioSignPostStripPPG iSignPostStripPPG, *pioSignPostStripPPG;
	iSignPostStripPPG.dwPageIndex = 2;
	pioSignPostStripPPG = &iSignPostStripPPG;
	if( FAILED( m_pPPO->GetData( (void**)&pioSignPostStripPPG ) ) )
	{
		EnableControls( FALSE );
		return;
	}

	// Make sure controls have been created
	if( ::IsWindow(m_hWnd) == FALSE )
	{
		return;
	}

	m_dwActivityLevel = iSignPostStripPPG.dwActivityLevel;

	if( m_dwActivityLevel < 4 )
	{
		m_comboActivityLevel.SetCurSel( m_dwActivityLevel );
	}
	else
	{
		m_comboActivityLevel.SetCurSel( 4 );
	}

	EnableControls( TRUE );
}

BOOL CSignPostStripPPG::OnSetActive() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	RefreshData();

	// Store active tab
	if( m_pPropPageMgr && m_pPropPageMgr->m_pIPropSheet )
	{
		m_pPropPageMgr->m_pIPropSheet->GetActivePage( &CSignPostStripPropPageMgr::sm_nActiveTab );
	}

	return CPropertyPage::OnSetActive();
}

void CSignPostStripPPG::UpdatePPO()
{
	if( m_pPPO )
	{
		ioSignPostStripPPG oSignPostStripPPG;
		oSignPostStripPPG.dwPageIndex = 2;
		oSignPostStripPPG.dwActivityLevel = m_dwActivityLevel;

		m_pPPO->SetData( (void *)&oSignPostStripPPG );
	}
}

void CSignPostStripPPG::OnSelchangeActivitylevel() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	int nChoice = m_comboActivityLevel.GetCurSel();
	if ( nChoice != (signed)m_dwActivityLevel )
	{
		m_dwActivityLevel = nChoice;

		UpdatePPO();
	}
}
