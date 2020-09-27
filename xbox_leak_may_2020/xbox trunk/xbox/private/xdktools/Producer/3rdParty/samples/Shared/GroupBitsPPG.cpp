/************************************************************************
*                                                                       *
*   Copyright (c) 1998-1999 Microsoft Corp. All rights reserved.        *
*                                                                       *
************************************************************************/

// GroupBitsPPG.cpp : implementation file
//

#include "stdafx.h"
#include "GroupBitsPPG.h"
#include "TrackFlagsPPG.h"
#pragma warning( push )
#pragma warning( disable : 4201 )
#include <dmusici.h>
#pragma warning( pop )
#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// {18E70D80-EA88-4224-8EA2-F336F24C053B}
const GUID GUID_GroupBitsPPGMgr = 
{ 0x18e70d80, 0xea88, 0x4224, { 0x8e, 0xa2, 0xf3, 0x36, 0xf2, 0x4c, 0x5, 0x3b } };

short CGroupBitsPropPageMgr::sm_nActiveTab = 0;
short* CGroupBitsPPG::sm_pnActiveTab = NULL;

/////////////////////////////////////////////////////////////////////////////
// CGroupBitsPropPageMgr property page

CGroupBitsPropPageMgr::CGroupBitsPropPageMgr()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	m_pGroupBitsPPG = NULL;
	m_pTrackFlagsPPG = NULL;
	m_GUIDManager = GUID_GroupBitsPPGMgr;
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

	// Add Group bits tab
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
	m_pIPropSheet = NULL;
	m_fNeedToDetach = FALSE;
	m_PPGTrackParams.dwPageIndex = GROUPBITSPPG_INDEX;
	m_PPGTrackParams.dwGroupBits = 0;
	m_PPGTrackParams.dwPropPageFlags = 0;
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
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGroupBitsPPG, CPropertyPage)
	//{{AFX_MSG_MAP(CGroupBitsPPG)
	ON_WM_CREATE()
	ON_WM_DESTROY()
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

	// Copy the fEnable flag to a private flag for setting the group bits checkboxes
	BOOL fGroupBitsEnable = fEnable;

	// If the read-only flag is set, set fGroupBitsEnable to FALSE
	if( m_PPGTrackParams.dwPropPageFlags & GROUPBITSPPG_GROUPBITS_RO )
	{
		fGroupBitsEnable = FALSE;
	}

	// Now, enable or disable the group bits checkboxes
	for( int i = IDC_CHECK_GROUP1 ;  i <= IDC_CHECK_GROUP32 ;  i++ )
	{
		EnableItem( i, fGroupBitsEnable );
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

BOOL CGroupBitsPPG::OnInitDialog() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	CPropertyPage::OnInitDialog();
	
	return FALSE;  // return TRUE unless you set the focus to a control
	               // EXCEPTION: OCX Property Pages should return FALSE
}

void CGroupBitsPPG::RefreshData( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_pPPO == NULL )
	{
		EnableControls( FALSE );
		return;
	}

	PPGTrackParams *pPPGTrackParams = &m_PPGTrackParams;
	if( FAILED( m_pPPO->GetData( (void**)&pPPGTrackParams ) ) )
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

	CWnd* pWnd;

	// Group Bits check boxes
	int nState;
	int j;

	for( int i = IDC_CHECK_GROUP1 ;  i <= IDC_CHECK_GROUP32 ;  i++ )
	{
		pWnd = GetDlgItem( i );
		if( pWnd )
		{
			j = i - IDC_CHECK_GROUP1;

			if( m_PPGTrackParams.dwGroupBits & (1 << j) )
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

BOOL CGroupBitsPPG::OnCommand( WPARAM wParam, LPARAM lParam ) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	int nControlID = LOWORD( wParam );
	int nCommandID = HIWORD( wParam );

	if( nCommandID == BN_CLICKED
	||  nCommandID == BN_DOUBLECLICKED )
	{
		if( nControlID >= IDC_CHECK_GROUP1
		&&  nControlID <= IDC_CHECK_GROUP32 )
		{
			CWnd* pWnd = GetDlgItem( nControlID );
			if( pWnd )
			{
				int nVariation = nControlID - IDC_CHECK_GROUP1;

				switch( IsDlgButtonChecked( nControlID ) )
				{
					case 0:		// Currently unchecked
						CheckDlgButton( nControlID, 1 );
						m_PPGTrackParams.dwGroupBits |= (1 << nVariation);
						UpdatePPO();
						break;

					case 1:		// Currently checked
						{
							// Don't allow ourself to be removed from all groups
							DWORD dwNewGroupBits = m_PPGTrackParams.dwGroupBits & ~(1 << nVariation);
							if( dwNewGroupBits == 0 )
							{
								CheckDlgButton( nControlID, 1 );
							}
							else
							{
								CheckDlgButton( nControlID, 0 );
								m_PPGTrackParams.dwGroupBits = dwNewGroupBits;
								UpdatePPO();
							}
						}
						break;
				}
			}

			return TRUE;
		}
	}
	
	return CPropertyPage::OnCommand( wParam, lParam );
}

void CGroupBitsPPG::UpdatePPO()
{
	if( m_pPPO )
	{
		m_pPPO->SetData( (void *)&m_PPGTrackParams );
	}
}

void CGroupBitsPPG::EnableItem(int nItem, BOOL fEnable)
{
	CWnd* pWnd;
	pWnd = GetDlgItem(nItem);
	if (pWnd)
	{
		pWnd->EnableWindow(fEnable);
	}
}
