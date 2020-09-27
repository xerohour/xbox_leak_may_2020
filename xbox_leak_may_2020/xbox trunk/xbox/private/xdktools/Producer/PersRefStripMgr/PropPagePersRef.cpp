// PropPagePersRef.cpp : implementation file
//

#include "stdafx.h"
#include <DMUSProd.h>
#include <Conductor.h>
#include "PropPersRef.h"
#include "PropPageMgr.h"
#include "PropPagePersRef.h"
#include <initguid.h>
#include <ChordMapDesigner.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CPersRefPropPageMgr constructor/destructor

CPersRefPropPageMgr::CPersRefPropPageMgr(IDMUSProdFramework* pIFramework) : CStaticPropPageManager()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	m_pIFramework = pIFramework;
	m_pIFramework->AddRef();
	m_pPropPagePersRef = NULL;
//	CStaticPropPageManager::CStaticPropPageManager();
}

CPersRefPropPageMgr::~CPersRefPropPageMgr()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	if( m_pIFramework )
	{
		m_pIFramework->Release();
	}
	if( m_pPropPagePersRef )
	{
		delete m_pPropPagePersRef;
		m_pPropPagePersRef = NULL;
	}
//	CStaticPropPageManager::~CStaticPropPageManager();
}


/////////////////////////////////////////////////////////////////////////////
// CPersRefPropPageMgr IUnknown implementation

/////////////////////////////////////////////////////////////////////////////
// CPersRefPropPageMgr::QueryInterface

HRESULT STDMETHODCALLTYPE CPersRefPropPageMgr::QueryInterface( REFIID riid, LPVOID *ppv )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Just call the base class implementation
	//return CStaticPropPageManager::QueryInterface( riid, ppv );
	return CStaticPropPageManager::QueryInterface( riid, ppv );
};


/////////////////////////////////////////////////////////////////////////////
// CPersRefPropPageMgr IDMUSProdPropPageManager implementation

/////////////////////////////////////////////////////////////////////////////
// CPersRefPropPageMgr::GetPropertySheetTitle

HRESULT STDMETHODCALLTYPE CPersRefPropPageMgr::GetPropertySheetTitle( BSTR* pbstrTitle, 
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

	CString strTitle;

	// Get and store the title.
	strTitle.LoadString( IDS_PROPPAGE_PERS_REF );
	*pbstrTitle = strTitle.AllocSysString();

	return S_OK;
};


/////////////////////////////////////////////////////////////////////////////
// CPersRefPropPageMgr::GetPropertySheetPages

HRESULT STDMETHODCALLTYPE CPersRefPropPageMgr::GetPropertySheetPages( IDMUSProdPropSheet* pIPropSheet, 
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
		return E_INVALIDARG;
	}

	m_pIPropSheet = pIPropSheet;
	m_pIPropSheet->AddRef();

	hPropSheetPage[0] = NULL;
	*pnNbrPages = 0;

	// Add PersRef tab
	HPROPSHEETPAGE hPage;
	short nNbrPages = 0;

	if(!m_pPropPagePersRef)
	{
		m_pPropPagePersRef = new PropPagePersRef;
		m_pPropPagePersRef->m_pIFramework = m_pIFramework;
		m_pPropPagePersRef->m_pIFramework->AddRef();

		m_pPropPagePersRef->m_pPropPageMgr = this;
	}


	if( m_pPropPagePersRef )
	{
		PROPSHEETPAGE psp;
		memcpy( &psp, &m_pPropPagePersRef->m_psp, sizeof(PROPSHEETPAGE) );

		hPage = ::CreatePropertySheetPage( (LPCPROPSHEETPAGE)&psp );
		if( hPage )
		{
			hPropSheetPage[nNbrPages] = (LONG *)hPage;
			nNbrPages++;
		}
		m_pPropPagePersRef->m_pPropPageMgr = this;
	}

	// Set number of pages
	*pnNbrPages = nNbrPages;
	return S_OK;
};


/////////////////////////////////////////////////////////////////////////////
// CPersRefPropPageMgr::RefreshData

HRESULT STDMETHODCALLTYPE CPersRefPropPageMgr::RefreshData()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	CPropPersRef* pPersRef = NULL;
	HRESULT hr = S_OK;

	if( m_pIPropPageObject == NULL )
	{
		pPersRef = NULL;
	}
	else if( FAILED ( m_pIPropPageObject->GetData( (void **)&pPersRef ) ) )
	{
		pPersRef = NULL;
		hr = E_FAIL;
	}

	if( m_pPropPagePersRef )
	{
		m_pPropPagePersRef->m_fHaveData = FALSE;

		if( pPersRef )
		{
			m_pPropPagePersRef->m_fHaveData = TRUE;
			m_pPropPagePersRef->CopyDataToPersRef( pPersRef );
		}
		else
		{
			CPropPersRef PersRef;
			m_pPropPagePersRef->CopyDataToPersRef( &PersRef );
		}

		m_pPropPagePersRef->UpdateControls();
	}

	if( pPersRef )
	{
		delete pPersRef;
	}

	return hr;
};

void CPersRefPropPageMgr::UpdateObjectWithPersRefData( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	ASSERT( m_pIPropPageObject != NULL );

	CPropPersRef PersRef;

	// Populate the PersRef structure
	m_pPropPagePersRef->GetDataFromPersRef( &PersRef );
	
	// Send the new data to the PropPageObject
	m_pIPropPageObject->SetData( (void *)&PersRef );
}


/////////////////////////////////////////////////////////////////////////////
// PropPagePersRef property page

IMPLEMENT_DYNCREATE(PropPagePersRef, CPropertyPage)

/////////////////////////////////////////////////////////////////////////////
// PropPagePersRef constructor/destructor

PropPagePersRef::PropPagePersRef() : CPropertyPage(PropPagePersRef::IDD)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

//	CPropertyPage(IDD_PERS_REF_PROPPAGE);
	//{{AFX_DATA_INIT(PropPagePersRef)
	//}}AFX_DATA_INIT
	m_pIFramework = NULL;

	m_fIgnoreSelChange = FALSE;
	m_fNeedToDetach = FALSE;
	m_fHaveData = FALSE;

	m_pPersRef = new CPropPersRef;
}

PropPagePersRef::~PropPagePersRef()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	PersListInfo* pPersListInfo;

	while( !m_lstPersListInfo.IsEmpty() )
	{
		pPersListInfo = static_cast<PersListInfo*>( m_lstPersListInfo.RemoveHead() );
		delete pPersListInfo;
	}

	if( m_pIFramework )
	{
		m_pIFramework->Release();
	}

	if( m_pPersRef )
	{
		delete m_pPersRef;
	}
}


/////////////////////////////////////////////////////////////////////////////
// PropPagePersRef::DoDataExchange

void PropPagePersRef::DoDataExchange(CDataExchange* pDX)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	CPropertyPage::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(PropPagePersRef)
	DDX_Control(pDX, IDC_COMBO_PERSS, m_comboPers);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(PropPagePersRef, CPropertyPage)
	//{{AFX_MSG_MAP(PropPagePersRef)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_KILLFOCUS()
	ON_CBN_SELCHANGE(IDC_COMBO_PERSS, OnSelchangeComboPerss)
	ON_CBN_DROPDOWN(IDC_COMBO_PERSS, OnDropDownComboPers)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// PropPagePersRef custom functions

/////////////////////////////////////////////////////////////////////////////
// PropPagePersRef::UpdateControls

void PropPagePersRef::UpdateControls( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Controls not created yet
	if( IsWindow( m_hWnd ) == 0 )
	{
		return;
	}

	ASSERT( m_pPersRef );
	if( m_pPersRef == NULL )
	{
		EnableControls( FALSE );
		return;
	}

	// Update the property page based on the new data.

	// Set enable state of controls
	EnableControls( m_fHaveData );

	// Personality combo box
	m_fIgnoreSelChange = TRUE;
	SetPersComboBoxSelection();
	m_fIgnoreSelChange = FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// PropPagePersRef message handlers

/////////////////////////////////////////////////////////////////////////////
// PropPagePersRef::OnCreate

int PropPagePersRef::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Attach the window to the property page structure.
	// This has been done once already in the main application
	// since the main application owns the property sheet.
	// It needs to be done here so that the window handle can
	// be found in the DLLs handle map.
	// JHD 8/26/98: Only need to do this if the window handle isn't in the
	// permament map.
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


/////////////////////////////////////////////////////////////////////////////
// PropPagePersRef::OnDestroy

void PropPagePersRef::OnDestroy() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Detach the window from the property page structure.
	// This will be done again by the main application since
	// it owns the property sheet.  It needs o be done here
	// so that the window handle can be removed from the
	// DLLs handle map.

	// JHD 8/26/98: Only need to do this if we called 'Attach' in OnCreate
	if( m_fNeedToDetach && m_hWnd )
	{
		HWND hWnd = m_hWnd;
		Detach();
		m_hWnd = hWnd;
		m_fNeedToDetach = FALSE;
	}

	CPropertyPage::OnDestroy();	
}


/////////////////////////////////////////////////////////////////////////////
// PropPagePersRef::OnInitDialog

BOOL PropPagePersRef::OnInitDialog() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	CPropertyPage::OnInitDialog();
	
	// Update the dialog
	UpdateControls();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	               // EXCEPTION: OCX Property Pages should return FALSE
}

BEGIN_EVENTSINK_MAP(PropPagePersRef, CPropertyPage)
    //{{AFX_EVENTSINK_MAP(PropPagePersRef)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

void PropPagePersRef::OnSelchangeComboPerss() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	ASSERT( m_pIFramework != NULL );
	ASSERT( m_pPersRef != NULL );

	if( m_fIgnoreSelChange )
	{
		return;
	}

	int nItem = m_comboPers.GetCurSel();
	if( nItem != CB_ERR )
	{
		PersListInfo* pPersListInfo = (PersListInfo *)m_comboPers.GetItemDataPtr( nItem );
		if( pPersListInfo != (PersListInfo *)-1 )
		{
			// fill in appropriate fields
			m_pPersRef->m_PersListInfo = *pPersListInfo;

			IDMUSProdNode* pIPersDocRootNode = NULL;
					
			// NOTE: personalities must currently be selected to avoid killing
			// the property page while making this call
			if( SUCCEEDED ( m_pIFramework->FindDocRootNodeByFileGUID( pPersListInfo->guidFile, &pIPersDocRootNode ) ) )
			{
				if( m_pPersRef->m_pIPersDocRootNode )
				{
					m_pPersRef->m_pIPersDocRootNode->Release();
					m_pPersRef->m_pIPersDocRootNode = NULL;
				}

				m_pPersRef->m_pIPersDocRootNode = pIPersDocRootNode;
				if( m_pPersRef->m_pIPersDocRootNode )
				{
					m_pPersRef->m_pIPersDocRootNode->AddRef();
				}

				m_pPropPageMgr->UpdateObjectWithPersRefData();

				pIPersDocRootNode->Release();
			}
		}
	}
	
}


BOOL PropPagePersRef::OnSetActive( void ) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_pPropPageMgr )
	{
		m_pPropPageMgr->RefreshData();
	}
	
	return CPropertyPage::OnSetActive();
}

void PropPagePersRef::EnableControls( BOOL fEnable ) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_comboPers.GetSafeHwnd() == NULL )
	{
		return;
	}

	m_comboPers.EnableWindow( fEnable );

	if( !fEnable )
	{
		m_comboPers.SetCurSel( -1 );
	}
}


void PropPagePersRef::SetPersComboBoxSelection( void ) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	ASSERT( m_pIFramework != NULL );

	IDMUSProdProject* pIProject = NULL;
	CString strText;
	int nMatch;
	int nCount;
	int nCurPos;
	int nPos = -1;

	// Nothing to select
	if( m_pPersRef->m_pIPersDocRootNode == NULL )
	{
		goto LEAVE;
	}

	// Create a PersListInfo struct for the current Pers
	DMUSProdListInfo ListInfo;
	PersListInfo* pPersListInfo;

	ZeroMemory( &ListInfo, sizeof(ListInfo) );
	ListInfo.wSize = sizeof(ListInfo);

	pPersListInfo = new PersListInfo;
	if( pPersListInfo == NULL )
	{
		goto LEAVE;
	}

	// Update DocRoot file GUID
	m_pIFramework->GetNodeFileGUID ( m_pPersRef->m_pIPersDocRootNode, &pPersListInfo->guidFile );

	if( SUCCEEDED ( m_pPersRef->m_pIPersDocRootNode->GetNodeListInfo ( &ListInfo ) ) )
	{
		if( ListInfo.bstrName )
		{
			pPersListInfo->strName = ListInfo.bstrName;
			::SysFreeString( ListInfo.bstrName );
		}
		
		if( ListInfo.bstrDescriptor )
		{
			pPersListInfo->strDescriptor = ListInfo.bstrDescriptor;
			::SysFreeString( ListInfo.bstrDescriptor );
		}

		if( FAILED ( m_pIFramework->FindProject( m_pPersRef->m_pIPersDocRootNode, &pIProject ) ) )
		{
			delete pPersListInfo;
			goto LEAVE;
		}

		pPersListInfo->pIProject = pIProject;
//		pPersListInfo->pIProject->AddRef();	intentionally missing

		GUID guidProject;
		if( FAILED ( pIProject->GetGUID( &guidProject ) ) )
		{
			delete pPersListInfo;
			goto LEAVE;
		}

//		if( !IsEqualGUID( guidProject, m_guidProject ) )
		{
			BSTR bstrProjectName;

			if( FAILED ( pIProject->GetName( &bstrProjectName ) ) )
			{
				delete pPersListInfo;
				goto LEAVE;
			}

			pPersListInfo->strProjectName = bstrProjectName;
			::SysFreeString( bstrProjectName );
		}
	}
	else
	{
		delete pPersListInfo;
		goto LEAVE;
	}

	// Select the Personality in the combo box list
	nMatch = CB_ERR;
	nCount = m_comboPers.GetCount();
	for( nCurPos = 0 ;  nCurPos < nCount ;  nCurPos++ )
	{
		PersListInfo* pCurPersListInfo = (PersListInfo *)m_comboPers.GetItemDataPtr( nCurPos );
		if( pCurPersListInfo )
		{
			if( pCurPersListInfo != (PersListInfo *)-1 )
			{
				// See if GUIDs are equal
				if( ::IsEqualGUID( pCurPersListInfo->guidFile, pPersListInfo->guidFile ) )
				{
					nMatch = nCurPos;
					break;
				}
			}
		}
	}
	
	if( nMatch == CB_ERR )
	{
		PersListInfo* pPLI;

		// Delete old PersInfo list
		while( !m_lstPersListInfo.IsEmpty() )
		{
			pPLI = static_cast<PersListInfo*>( m_lstPersListInfo.RemoveHead() );
			delete pPLI;
		}

		// Remove old list from combo box
		m_comboPers.ResetContent();

		// Add this Personality to the combo box list
		nPos = 0;
		m_lstPersListInfo.AddTail( pPersListInfo );
		InsertPersInfoListInComboBox();
	}
	else
	{
		nPos = nMatch;
		delete pPersListInfo;
		pPersListInfo = NULL;
	}

LEAVE:
	m_comboPers.SetCurSel( nPos );

	if( pIProject )
	{
		pIProject->Release();
	}
}


void PropPagePersRef::GetComboBoxText( PersListInfo* pPersListInfo, CString& strText )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( pPersListInfo->strProjectName.IsEmpty() )
	{
		strText.Format( "%s %s", pPersListInfo->strName, pPersListInfo->strDescriptor );
	}
	else
	{
		strText.Format( "%s: %s %s", pPersListInfo->strProjectName, pPersListInfo->strName, pPersListInfo->strDescriptor );
	}
}

void PropPagePersRef::InsertPersInfoListInComboBox( void ) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	CString strText;
	PersListInfo* pPersListInfo;
	int nPos;

	POSITION pos = m_lstPersListInfo.GetHeadPosition();
	while( pos != NULL )
	{
		pPersListInfo = static_cast<PersListInfo*>( m_lstPersListInfo.GetNext(pos) );
		if( pPersListInfo )
		{
			GetComboBoxText( pPersListInfo, strText );
			nPos = m_comboPers.AddString( strText );
			if( nPos >= 0 )
			{
				m_comboPers.SetItemDataPtr( nPos, pPersListInfo );
			}
		}
	}
}

void PropPagePersRef::InsertPersInfo( PersListInfo* pPersListInfo )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	PersListInfo* pListInfo;
	CString strListText;
	CString strPersText;
	POSITION posList;

	GetComboBoxText( pPersListInfo, strPersText );

	POSITION pos = m_lstPersListInfo.GetHeadPosition();
	while( pos != NULL )
	{
		posList = pos;

		pListInfo = static_cast<PersListInfo*>( m_lstPersListInfo.GetNext(pos) );
		if( pListInfo )
		{
			if( !pPersListInfo->strProjectName.IsEmpty()
			&&  pListInfo->strProjectName.IsEmpty() )
			{
				continue;
			}

			if( pPersListInfo->strProjectName.IsEmpty()
			&&  !pListInfo->strProjectName.IsEmpty() )
			{
				if( pos )
				{
					m_lstPersListInfo.InsertBefore( pos, pPersListInfo );
				}
				else
				{
					m_lstPersListInfo.AddTail( pPersListInfo );
				}
				return;
			}

			GetComboBoxText( pListInfo, strListText );

			if( strListText.CompareNoCase( strPersText ) > 0 )
			{
				m_lstPersListInfo.InsertBefore( posList, pPersListInfo );
				return;
			}
		}
	}

	m_lstPersListInfo.AddTail( pPersListInfo );
}


void PropPagePersRef::BuildPersInfoList( void ) 
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
	PersListInfo* pPersListInfo;

	if( FAILED ( m_pIFramework->FindDocTypeByNodeId( GUID_PersonalityNode, &pIDocType ) ) )
	{
		return;
	}

	HRESULT hr = m_pIFramework->GetFirstProject( &pINextProject );

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
				pPersListInfo = new PersListInfo;

				pPersListInfo->pIProject = pIProject;
//				pPersListInfo->pIProject->AddRef();	intentionally missing

				if( SUCCEEDED ( pIProject->GetGUID( &guidProject ) ) )
				{
//					if( !IsEqualGUID( guidProject, m_guidProject ) )
					{
						if( SUCCEEDED ( pIProject->GetName( &bstrProjectName ) ) )
						{
							pPersListInfo->strProjectName = bstrProjectName;
							::SysFreeString( bstrProjectName );
						}
					}

				}

				if( ListInfo.bstrName )
				{
					pPersListInfo->strName = ListInfo.bstrName;
					::SysFreeString( ListInfo.bstrName );
				}
				
				if( ListInfo.bstrDescriptor )
				{
					pPersListInfo->strDescriptor = ListInfo.bstrDescriptor;
					::SysFreeString( ListInfo.bstrDescriptor );
				}
				// Update DocRoot file GUID
				m_pIFramework->GetNodeFileGUID ( pIFileNode, &pPersListInfo->guidFile );

				InsertPersInfo( pPersListInfo );

				hr = pIProject->GetNextFileByDocType( pIFileNode, &pINextFileNode );
				pIFileNode->Release();
			}
			else
			{
				hr = E_FAIL;
			}
		}
	
	    hr = m_pIFramework->GetNextProject( pIProject, &pINextProject );
		pIProject->Release();
	}

	pIDocType->Release();
}

void PropPagePersRef::CopyDataToPersRef( CPropPersRef* pPersRef )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	ASSERT( pPersRef != NULL );

	m_pPersRef->m_mtTime		= pPersRef->m_mtTime;
	m_pPersRef->m_dwMeasure		= pPersRef->m_dwMeasure;
	m_pPersRef->m_bBeat			= pPersRef->m_bBeat;
	m_pPersRef->m_dwBits		= pPersRef->m_dwBits;
	m_pPersRef->m_PersListInfo  = pPersRef->m_PersListInfo;

	if( m_pPersRef->m_pIPersDocRootNode )
	{
		m_pPersRef->m_pIPersDocRootNode->Release();
		m_pPersRef->m_pIPersDocRootNode = NULL;
	}
	m_pPersRef->m_pIPersDocRootNode = pPersRef->m_pIPersDocRootNode;
	if( m_pPersRef->m_pIPersDocRootNode )
	{
		m_pPersRef->m_pIPersDocRootNode->AddRef();
	}

	memcpy( &m_pPersRef->m_guidProject, &pPersRef->m_guidProject, sizeof( m_pPersRef->m_guidProject ) );
}


void PropPagePersRef::GetDataFromPersRef( CPropPersRef* pPersRef )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	ASSERT( pPersRef != NULL );

	pPersRef->m_mtTime			= m_pPersRef->m_mtTime;
	pPersRef->m_dwMeasure		= m_pPersRef->m_dwMeasure;
	pPersRef->m_bBeat			= m_pPersRef->m_bBeat;
	pPersRef->m_dwBits			= m_pPersRef->m_dwBits;
	pPersRef->m_PersListInfo	= m_pPersRef->m_PersListInfo;

	if( pPersRef->m_pIPersDocRootNode )
	{
		pPersRef->m_pIPersDocRootNode->Release();
		pPersRef->m_pIPersDocRootNode = NULL;
	}
	pPersRef->m_pIPersDocRootNode = m_pPersRef->m_pIPersDocRootNode;
	if( pPersRef->m_pIPersDocRootNode )
	{
		pPersRef->m_pIPersDocRootNode->AddRef();
	}

	memcpy( &pPersRef->m_guidProject, &m_pPersRef->m_guidProject, sizeof( pPersRef->m_guidProject ) );
}

void PropPagePersRef::OnDropDownComboPers( void ) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	PersListInfo* pPersListInfo;

	// Delete old PersInfo list
	while( !m_lstPersListInfo.IsEmpty() )
	{
		pPersListInfo = static_cast<PersListInfo*>( m_lstPersListInfo.RemoveHead() );
		delete pPersListInfo;
	}

	// Remove old list from combo box
	m_comboPers.ResetContent();

	// Rebuild the PersInfo list
	BuildPersInfoList();
	InsertPersInfoListInComboBox();

	// Select the current Pers
	SetPersComboBoxSelection();
}

BOOL PropPagePersRef::PreTranslateMessage(MSG* pMsg) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( !m_fHaveData )
	{
		return CPropertyPage::PreTranslateMessage( pMsg );
	}

	switch( pMsg->message )
	{
		case WM_KEYDOWN:
			if( pMsg->lParam & 0x40000000 )
			{
				break;
			}

			switch( pMsg->wParam )
			{
				case VK_RETURN:
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
					return TRUE;
				}
			}
			break;
		default:
			break;

	}
	
	return CPropertyPage::PreTranslateMessage( pMsg );
}
