// GroupBitsPPG.cpp : implementation file
//

#include "stdafx.h"
#include "GroupBitsPPG.h"
#include "PropPageVarSeed.h"
#include "TrackFlagsPPG.h"
#include "SegmentIO.h"
#include <dmusici.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGroupBitsPropPageMgr property page

short CGroupBitsPropPageMgr::sm_nActiveTab = 0;
short *CGroupBitsPPG::sm_pnActiveTab = NULL;

CGroupBitsPropPageMgr::CGroupBitsPropPageMgr()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	m_pGroupBitsPPG = NULL;
	m_pTrackFlagsPPG = NULL;
	m_pVarSeedPPG = NULL;
	CStaticPropPageManager::CStaticPropPageManager();
}

CGroupBitsPropPageMgr::~CGroupBitsPropPageMgr()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
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
	if( m_pVarSeedPPG )
	{
		delete m_pVarSeedPPG;
		m_pVarSeedPPG = NULL;
	}
	CStaticPropPageManager::~CStaticPropPageManager();
}

HRESULT CGroupBitsPropPageMgr::QueryInterface( REFIID riid, LPVOID *ppv )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	return CStaticPropPageManager::QueryInterface( riid, ppv );
}

HRESULT CGroupBitsPropPageMgr::GetPropertySheetTitle( BSTR* pbstrTitle, 
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

HRESULT CGroupBitsPropPageMgr::GetPropertySheetPages( IDMUSProdPropSheet* pIPropSheet, 
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

	// Add group bits tab
	HPROPSHEETPAGE hPage;
	short nNbrPages = 0;
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
		CGroupBitsPPG::sm_pnActiveTab = &CGroupBitsPropPageMgr::sm_nActiveTab;
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
		CTrackFlagsPPG::sm_pnActiveTab = &CGroupBitsPropPageMgr::sm_nActiveTab;
	}

	// Add variation seed tab
	if( NULL == m_pVarSeedPPG )
	{
		m_pVarSeedPPG = new CPropPageVarSeed();
	}
	if( m_pVarSeedPPG )
	{
		hPage = ::CreatePropertySheetPage( (LPCPROPSHEETPAGE)&m_pVarSeedPPG->m_psp );
		if( hPage )
		{
			hPropSheetPage[nNbrPages] = (LONG *)hPage;
			nNbrPages++;
		}			

		// Weak reference
		m_pVarSeedPPG->m_pIPropSheet = m_pIPropSheet;

		// Tell the property page to update the active tab setting
		CPropPageVarSeed::sm_pnActiveTab = &CGroupBitsPropPageMgr::sm_nActiveTab;
	}

	// Set number of pages
	*pnNbrPages = nNbrPages;
	return S_OK;
}

HRESULT CGroupBitsPropPageMgr::RefreshData()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	if( m_pGroupBitsPPG )
	{
		m_pGroupBitsPPG->RefreshData();
	}
	if( m_pTrackFlagsPPG )
	{
		m_pTrackFlagsPPG->RefreshData();
	}
	if( m_pVarSeedPPG )
	{
		m_pVarSeedPPG->RefreshData();
	}
	return S_OK;
}

HRESULT CGroupBitsPropPageMgr::SetObject( IDMUSProdPropPageObject* pINewPropPageObject )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	if( m_pGroupBitsPPG )
	{
		m_pGroupBitsPPG->SetObject( pINewPropPageObject );
	}
	if( m_pTrackFlagsPPG )
	{
		m_pTrackFlagsPPG->SetObject( pINewPropPageObject );
	}
	if( m_pVarSeedPPG )
	{
		m_pVarSeedPPG->SetObject( pINewPropPageObject );
	}
	return CBasePropPageManager::SetObject( pINewPropPageObject );
}

/////////////////////////////////////////////////////////////////////////////
// CGroupBitsPPG property page

IMPLEMENT_DYNCREATE(CGroupBitsPPG, CPropertyPage)

CGroupBitsPPG::CGroupBitsPPG() : CPropertyPage(CGroupBitsPPG::IDD)
{
	//{{AFX_DATA_INIT(CGroupBitsPPG)
	//}}AFX_DATA_INIT
	m_pPPO = NULL;
	m_PPGTrackParams.dwPageIndex = 0;
	m_PPGTrackParams.dwGroupBits = 0;
	m_fNeedToDetach = FALSE;
}

CGroupBitsPPG::~CGroupBitsPPG()
{
	if( m_pPPO )
	{
		m_pPPO->Release();
		m_pPPO = NULL;
	}
}

void CGroupBitsPPG::DoDataExchange(CDataExchange* pDX)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGroupBitsPPG)
	DDX_Control(pDX, IDC_COMBO_GROUP_LIST, m_GroupDropDownList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGroupBitsPPG, CPropertyPage)
	//{{AFX_MSG_MAP(CGroupBitsPPG)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_CBN_SELCHANGE(IDC_COMBO_GROUP_LIST, OnSelchangeComboGroupList)
	ON_MESSAGE( WM_APP, OnApp )
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// private functions
void CGroupBitsPPG::SetObject( IDMUSProdPropPageObject* pPPO )
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

void CGroupBitsPPG::EnableControls( BOOL fEnable ) 
{
	if( ::IsWindow(m_hWnd) == FALSE )
	{
		return;
	}

	if(::IsWindow(m_GroupDropDownList.m_hWnd))
	{
		m_GroupDropDownList.EnableWindow(fEnable);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CGroupBitsPPG message handlers

int CGroupBitsPPG::OnCreate(LPCREATESTRUCT lpCreateStruct) 
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

void CGroupBitsPPG::OnDestroy() 
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

void CGroupBitsPPG::RefreshData( void )
{
	int iIndex;
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_pPPO == NULL )
	{
		EnableControls( FALSE );
		return;
	}

	ioGroupBitsPPGData *pioGroupBitsPPGData = &m_PPGTrackParams;
	if( FAILED( m_pPPO->GetData( (void**)&pioGroupBitsPPGData ) ) )
	{
		EnableControls( FALSE );
		return;
	}

	// Make sure controls have been created
	if( ::IsWindow(m_hWnd) == FALSE )
	{
		return;
	}

	// Prevent control notifications from being dispatched during UpdateData
	_AFX_THREAD_STATE* pThreadState = AfxGetThreadState();
	HWND hWndOldLockout = pThreadState->m_hLockoutNotifyWindow;
	ASSERT(hWndOldLockout != m_hWnd);   // must not recurse
	pThreadState->m_hLockoutNotifyWindow = m_hWnd;

	EnableControls( TRUE );

	// Group Bits combo box
	if (!m_PPGTrackParams.dwGroupBits)
	{
		m_PPGTrackParams.dwGroupBits = 1;
	}
	for (iIndex = 0; iIndex < 32; iIndex++)
	{
		if (m_PPGTrackParams.dwGroupBits & (1 << iIndex) )
			break;
	}
	m_GroupDropDownList.SetCurSel(iIndex);

	pThreadState->m_hLockoutNotifyWindow = hWndOldLockout;

}

BOOL CGroupBitsPPG::OnSetActive() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	RefreshData();

	// Store active tab
	if( sm_pnActiveTab && m_pIPropSheet )
	{
		m_pIPropSheet->GetActivePage( sm_pnActiveTab );
	}

	return CPropertyPage::OnSetActive();
}

void CGroupBitsPPG::UpdatePPO()
{
	if( m_pPPO )
	{
		VERIFY( SUCCEEDED( m_pPPO->SetData(&m_PPGTrackParams) ) );
	}
}

void CGroupBitsPPG::OnSelchangeComboGroupList() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	PostMessage( WM_APP, 0, 0 );
}

LRESULT CGroupBitsPPG::OnApp(WPARAM, LPARAM)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// Custom handler for OnSelChanged to work-around bug in user32.dll
	int iIndex = m_GroupDropDownList.GetCurSel();

	if (iIndex >= 0 && iIndex < 32)
	{
		m_PPGTrackParams.dwGroupBits = (1 << iIndex);
	}
	else
	{
		ASSERT(FALSE);
		return 0;
	}

	UpdatePPO();
	return 0;
}
