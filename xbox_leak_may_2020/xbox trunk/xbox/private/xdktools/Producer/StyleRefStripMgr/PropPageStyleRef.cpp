// PropPageStyleRef.cpp : implementation file
//

#include "stdafx.h"
#include <DMUSProd.h>
#include <Conductor.h>
#include "StyleRefMgr.h"
#include "timeline.h"
#include "PropStyleRef.h"
#include "PropPageMgr.h"
#include "PropPageStyleRef.h"
#include <initguid.h>
#include <StyleDesigner.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CStyleRefPropPageMgr constructor/destructor

CStyleRefPropPageMgr::CStyleRefPropPageMgr(IDMUSProdFramework* pIFramework, CStyleRefMgr* pStyleRefMgr) : CStaticPropPageManager()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	m_pIFramework = pIFramework;
	m_pIFramework->AddRef();

	m_pStyleRefMgr = pStyleRefMgr;

	m_pPropPageStyleRef = NULL;
//	CStaticPropPageManager::CStaticPropPageManager();
}

CStyleRefPropPageMgr::~CStyleRefPropPageMgr()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	if( m_pPropPageStyleRef )
	{
		delete m_pPropPageStyleRef;
		m_pPropPageStyleRef = NULL;
	}
	if( m_pIFramework )
	{
		m_pIFramework->Release();
		m_pIFramework = NULL;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CStyleRefPropPageMgr IUnknown implementation

/////////////////////////////////////////////////////////////////////////////
// CStyleRefPropPageMgr::QueryInterface

HRESULT STDMETHODCALLTYPE CStyleRefPropPageMgr::QueryInterface( REFIID riid, LPVOID *ppv )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Just call the base class implementation
	return CStaticPropPageManager::QueryInterface( riid, ppv );
};


/////////////////////////////////////////////////////////////////////////////
// CStyleRefPropPageMgr IDMUSProdPropPageManager implementation

/////////////////////////////////////////////////////////////////////////////
// CStyleRefPropPageMgr::GetPropertySheetTitle

HRESULT STDMETHODCALLTYPE CStyleRefPropPageMgr::GetPropertySheetTitle( BSTR* pbstrTitle, 
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
	strTitle.LoadString( IDS_PROPPAGE_STYLE_REF );
	*pbstrTitle = strTitle.AllocSysString();

	return S_OK;
};


/////////////////////////////////////////////////////////////////////////////
// CStyleRefPropPageMgr::GetPropertySheetPages

HRESULT STDMETHODCALLTYPE CStyleRefPropPageMgr::GetPropertySheetPages( IDMUSProdPropSheet* pIPropSheet, 
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

	// Add StyleRef tab
	HPROPSHEETPAGE hPage;
	short nNbrPages = 0;

	if(!m_pPropPageStyleRef)
	{
		m_pPropPageStyleRef = new PropPageStyleRef;
		if( m_pPropPageStyleRef )
		{
			m_pPropPageStyleRef->m_pIFramework = m_pIFramework;
			m_pPropPageStyleRef->m_pIFramework->AddRef();

			m_pPropPageStyleRef->m_pPropPageMgr = this;
		}
	}

	if( m_pPropPageStyleRef )
	{
		PROPSHEETPAGE psp;
		memcpy( &psp, &m_pPropPageStyleRef->m_psp, sizeof(PROPSHEETPAGE) );

		hPage = ::CreatePropertySheetPage( (LPCPROPSHEETPAGE)&psp );
		if( hPage )
		{
			hPropSheetPage[nNbrPages] = (LONG *)hPage;
			nNbrPages++;
		}
		m_pPropPageStyleRef->m_pPropPageMgr = this;
	}

	// Set number of pages
	*pnNbrPages = nNbrPages;
	return S_OK;
};


/////////////////////////////////////////////////////////////////////////////
// CStyleRefPropPageMgr::RefreshData

HRESULT STDMETHODCALLTYPE CStyleRefPropPageMgr::RefreshData()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	CPropStyleRef* pStyleRef = NULL;
	HRESULT hr = S_OK;

	if( m_pIPropPageObject == NULL )
	{
		pStyleRef = NULL;
	}
	else if( FAILED ( m_pIPropPageObject->GetData( (void **)&pStyleRef ) ) )
	{
		pStyleRef = NULL;
		hr = E_FAIL;
	}

	if( m_pPropPageStyleRef )
	{
		m_pPropPageStyleRef->m_fMultipleStylesSelected = FALSE;
		m_pPropPageStyleRef->m_fHaveData = FALSE;

		if( pStyleRef )
		{
			if( pStyleRef->m_dwMeasure == 0xFFFFFFFF )
			{
				m_pPropPageStyleRef->m_fMultipleStylesSelected = TRUE;
			}
			else
			{
				m_pPropPageStyleRef->m_fHaveData = TRUE;
			}
			m_pPropPageStyleRef->CopyDataToStyleRef( pStyleRef );
		}
		else
		{
			CPropStyleRef StyleRef;
			m_pPropPageStyleRef->CopyDataToStyleRef( &StyleRef );
		}

		m_pPropPageStyleRef->UpdateControls();
	}

	if( pStyleRef )
	{
		delete pStyleRef;
	}

	return hr;
};

void CStyleRefPropPageMgr::UpdateObjectWithStyleRefData( void )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	ASSERT( m_pIPropPageObject != NULL );

	CPropStyleRef StyleRef;

	// Populate the StyleRef structure
	m_pPropPageStyleRef->GetDataFromStyleRef( &StyleRef );
	
	// Send the new data to the PropPageObject
	m_pIPropPageObject->SetData( (void *)&StyleRef );
}


/////////////////////////////////////////////////////////////////////////////
// PropPageStyleRef property page

IMPLEMENT_DYNCREATE(PropPageStyleRef, CPropertyPage)

/////////////////////////////////////////////////////////////////////////////
// PropPageStyleRef constructor/destructor

PropPageStyleRef::PropPageStyleRef() : CPropertyPage(PropPageStyleRef::IDD)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

//	CPropertyPage(IDD_STYLE_REF_PROPPAGE);
	//{{AFX_DATA_INIT(PropPageStyleRef)
	//}}AFX_DATA_INIT
	m_pIFramework = NULL;

	m_fIgnoreSelChange = FALSE;
	m_fHaveData = FALSE;
	m_fMultipleStylesSelected = FALSE;
	m_pStyleRef = new CPropStyleRef;
	m_fNeedToDetach = FALSE;
}

PropPageStyleRef::~PropPageStyleRef()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	StyleListInfo* pStyleListInfo;

	while( !m_lstStyleListInfo.IsEmpty() )
	{
		pStyleListInfo = static_cast<StyleListInfo*>( m_lstStyleListInfo.RemoveHead() );
		delete pStyleListInfo;
	}

	if( m_pIFramework )
	{
		m_pIFramework->Release();
	}
	
	if( m_pStyleRef )
	{
		delete m_pStyleRef;
	}
}


/////////////////////////////////////////////////////////////////////////////
// PropPageStyleRef::DoDataExchange

void PropPageStyleRef::DoDataExchange(CDataExchange* pDX)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	CPropertyPage::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(PropPageStyleRef)
	DDX_Control(pDX, IDC_EDIT_MEASURE, m_editMeasure);
	DDX_Control(pDX, IDC_COMBO_STYLES, m_comboStyle);
	DDX_Control(pDX, IDC_SPIN_MEASURE, m_spinMeasure);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(PropPageStyleRef, CPropertyPage)
	//{{AFX_MSG_MAP(PropPageStyleRef)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_MEASURE, OnDeltaposSpinMeasure)
	ON_WM_KILLFOCUS()
	ON_EN_KILLFOCUS(IDC_EDIT_MEASURE, OnKillfocusEditMeasure)
	ON_CBN_SELCHANGE(IDC_COMBO_STYLES, OnSelchangeComboStyles)
	ON_CBN_DROPDOWN(IDC_COMBO_STYLES, OnDropDownComboStyle)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// PropPageStyleRef custom functions

/////////////////////////////////////////////////////////////////////////////
// PropPageStyleRef::UpdateControls

void PropPageStyleRef::UpdateControls()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Controls not created yet
	if( IsWindow( m_hWnd ) == 0 )
	{
		return;
	}

	ASSERT( m_pStyleRef );
	if( m_pStyleRef == NULL )
	{
		EnableControls( FALSE );
		return;
	}

	// Update the property page based on the new data.

	// Set enable state of controls
	EnableControls( m_fHaveData );

	// Style combo box
	m_fIgnoreSelChange = TRUE;
	SetStyleComboBoxSelection();
	m_fIgnoreSelChange = FALSE;

	// Measure
	if( m_pStyleRef->m_dwMeasure != (DWORD)(m_spinMeasure.GetPos() - 1) )
	{
		m_spinMeasure.SetPos( m_pStyleRef->m_dwMeasure + 1 );
	}
}


/////////////////////////////////////////////////////////////////////////////
// PropPageStyleRef message handlers

/////////////////////////////////////////////////////////////////////////////
// PropPageStyleRef::OnCreate

int PropPageStyleRef::OnCreate(LPCREATESTRUCT lpCreateStruct) 
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


/////////////////////////////////////////////////////////////////////////////
// PropPageStyleRef::OnDestroy

void PropPageStyleRef::OnDestroy() 
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
// PropPageStyleRef::OnInitDialog

BOOL PropPageStyleRef::OnInitDialog() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	CPropertyPage::OnInitDialog();
	
	m_spinMeasure.SetRange( 1, 32767 );
	m_editMeasure.LimitText( 5 );

	// Update the dialog
	UpdateControls();

	return TRUE;  // return TRUE unless you set the focus to a control
	               // EXCEPTION: OCX Property Pages should return FALSE
}

BEGIN_EVENTSINK_MAP(PropPageStyleRef, CPropertyPage)
    //{{AFX_EVENTSINK_MAP(PropPageStyleRef)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()


void PropPageStyleRef::OnDeltaposSpinMeasure(NMHDR* pNMHDR, LRESULT* pResult) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_fHaveData )
	{
		NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

		// Need to do this in case the user clicked the spin control immediately after
		// typing in a value
		OnKillfocusEditMeasure();

		int nNewValue = m_pStyleRef->m_dwMeasure + pNMUpDown->iDelta;
		int nDelta = pNMUpDown->iDelta;
		if( nNewValue < 0 )
		{
			nNewValue = 0;
			nDelta = -(signed(m_pStyleRef->m_dwMeasure));
		}
		else if( nNewValue > 32766 )
		{
			nNewValue = 32766;
			nDelta = 32766 - m_pStyleRef->m_dwMeasure;
		}

		if( (DWORD)nNewValue != m_pStyleRef->m_dwMeasure )
		{
			nNewValue = AdjustTime( nNewValue, nDelta );

			if( (DWORD)nNewValue != m_pStyleRef->m_dwMeasure )
			{
				m_pStyleRef->m_dwMeasure = nNewValue;
				m_spinMeasure.SetPos( nNewValue + 1 );
				m_pPropPageMgr->UpdateObjectWithStyleRefData();
			}
		}
	}

	// We handled this message - don't change the numbers further
	*pResult = 1;
}


void PropPageStyleRef::OnKillfocusEditMeasure() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	// Get maximum number of measures
	long lMaxMeasure;
	VARIANT var;
	m_pPropPageMgr->m_pStyleRefMgr->m_pTimeline->GetTimelineProperty( TP_CLOCKLENGTH, &var );
	m_pPropPageMgr->m_pStyleRefMgr->m_pTimeline->ClocksToMeasureBeat( m_pPropPageMgr->m_pStyleRefMgr->m_dwGroupBits,
																	  0,
																	  V_I4( &var ),
																	  &lMaxMeasure,
																	  NULL );
	// Ensure lMaxMeasure is at least 1
	lMaxMeasure = max( 1, lMaxMeasure );

	CString strNewMeasure;

	m_editMeasure.GetWindowText( strNewMeasure );

	// Strip leading and trailing spaces
	strNewMeasure.TrimRight();
	strNewMeasure.TrimLeft();

	if( strNewMeasure.IsEmpty() )
	{
		m_spinMeasure.SetPos( m_pStyleRef->m_dwMeasure + 1 );
	}
	else
	{
		int iNewMeasure = _ttoi( strNewMeasure );
		if( iNewMeasure > lMaxMeasure )
		{
			iNewMeasure = lMaxMeasure;
		}
		else if( iNewMeasure < 1 )
		{
			iNewMeasure = 1;
		}

		m_spinMeasure.SetPos( iNewMeasure );
		
		iNewMeasure--;
		if( (DWORD)iNewMeasure != m_pStyleRef->m_dwMeasure )
		{
			m_pStyleRef->m_dwMeasure = iNewMeasure;
			m_pPropPageMgr->UpdateObjectWithStyleRefData();
		}
	}
}

void PropPageStyleRef::OnSelchangeComboStyles() 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	ASSERT( m_pIFramework != NULL );
	ASSERT( m_pStyleRef != NULL );

	if( m_fIgnoreSelChange )
	{
		return;
	}

	int nItem = m_comboStyle.GetCurSel();
	if( nItem != CB_ERR )
	{
		StyleListInfo* pStyleListInfo = (StyleListInfo *)m_comboStyle.GetItemDataPtr( nItem );
		if( pStyleListInfo != (StyleListInfo *)-1 )
		{
			// fill in appropriate fields
			m_pStyleRef->m_StyleListInfo = *pStyleListInfo;

			IDMUSProdNode* pIStyleDocRootNode = NULL;
			
			// NOTE: styles must currently be selected to avoid killing
			// the property page while making this call
			if( SUCCEEDED ( m_pIFramework->FindDocRootNodeByFileGUID( pStyleListInfo->guidFile, &pIStyleDocRootNode ) ) )
			{
				if( m_pStyleRef->m_pIStyleDocRootNode )
				{
					m_pStyleRef->m_pIStyleDocRootNode->Release();
					m_pStyleRef->m_pIStyleDocRootNode = NULL;
				}
				
				m_pStyleRef->m_pIStyleDocRootNode = pIStyleDocRootNode;
				if( m_pStyleRef->m_pIStyleDocRootNode )
				{
					m_pStyleRef->m_pIStyleDocRootNode->AddRef();
				}

				m_pPropPageMgr->UpdateObjectWithStyleRefData();

				pIStyleDocRootNode->Release();	// ?????????
			}
		}
	}
	
}


BOOL PropPageStyleRef::OnSetActive( void ) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_pPropPageMgr )
	{
		m_pPropPageMgr->RefreshData();
	}
	
	return CPropertyPage::OnSetActive();
}


void PropPageStyleRef::EnableControls( BOOL fEnable ) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_editMeasure.GetSafeHwnd() == NULL )
	{
		return;
	}

	m_spinMeasure.EnableWindow( fEnable );
	m_editMeasure.EnableWindow( fEnable );
	m_comboStyle.EnableWindow( fEnable );

	if( !fEnable )
	{
		m_comboStyle.SetCurSel( -1 );
	}
}


void PropPageStyleRef::SetStyleComboBoxSelection( void ) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	ASSERT( m_pIFramework != NULL );

	IDMUSProdProject* pIProject = NULL;
	CString strText;
	int nMatch;
	int nCount;
	int nCurPos;
	int nPos = -1;

	// Multiple Styles selected
	if( m_fMultipleStylesSelected )
	{
		CString strText;
		if( strText.LoadString( IDS_MULTIPLE_SELECT ) )
		{
			StyleListInfo* pSLI;

			// Delete old StyleInfo list
			while( !m_lstStyleListInfo.IsEmpty() )
			{
				pSLI = static_cast<StyleListInfo*>( m_lstStyleListInfo.RemoveHead() );
				delete pSLI;
			}

			// Remove old list from combo box
			m_comboStyle.ResetContent();

			// Add this string to the combo box list and select it
			m_comboStyle.AddString( strText );
			nPos = 0;
			goto LEAVE;
		}
	}

	// Nothing to select
	if( m_pStyleRef->m_pIStyleDocRootNode == NULL )
	{
		goto LEAVE;
	}

	// Create a StyleListInfo struct for the current Style
	DMUSProdListInfo ListInfo;
	StyleListInfo* pStyleListInfo;

	ZeroMemory( &ListInfo, sizeof(ListInfo) );
	ListInfo.wSize = sizeof(ListInfo);

	pStyleListInfo = new StyleListInfo;
	if( pStyleListInfo == NULL )
	{
		goto LEAVE;
	}

	// Update DocRoot file GUID
	m_pIFramework->GetNodeFileGUID ( m_pStyleRef->m_pIStyleDocRootNode, &pStyleListInfo->guidFile );

	if( SUCCEEDED ( m_pStyleRef->m_pIStyleDocRootNode->GetNodeListInfo ( &ListInfo ) ) )
	{
		if( ListInfo.bstrName )
		{
			pStyleListInfo->strName = ListInfo.bstrName;
			::SysFreeString( ListInfo.bstrName );
		}
		
		if( ListInfo.bstrDescriptor )
		{
			pStyleListInfo->strDescriptor = ListInfo.bstrDescriptor;
			::SysFreeString( ListInfo.bstrDescriptor );
		}

		if( FAILED ( m_pIFramework->FindProject( m_pStyleRef->m_pIStyleDocRootNode, &pIProject ) ) )
		{
			delete pStyleListInfo;
			goto LEAVE;
		}

		pStyleListInfo->pIProject = pIProject;
//		pStyleListInfo->pIProject->AddRef();	intentionally missing

		GUID guidProject;
		if( FAILED ( pIProject->GetGUID( &guidProject ) ) )
		{
			delete pStyleListInfo;
			goto LEAVE;
		}

//		if( !IsEqualGUID( guidProject, m_guidProject ) )
		{
			BSTR bstrProjectName;

			if( FAILED ( pIProject->GetName( &bstrProjectName ) ) )
			{
				delete pStyleListInfo;
				goto LEAVE;
			}

			pStyleListInfo->strProjectName = bstrProjectName;
			::SysFreeString( bstrProjectName );
		}
	}
	else
	{
		delete pStyleListInfo;
		goto LEAVE;
	}

	// Select the Style in the combo box list
	nMatch = CB_ERR;
	nCount = m_comboStyle.GetCount();
	for( nCurPos = 0 ;  nCurPos < nCount ;  nCurPos++ )
	{
		StyleListInfo* pCurStyleListInfo = (StyleListInfo *)m_comboStyle.GetItemDataPtr( nCurPos );
		if( pCurStyleListInfo )
		{
			if( pCurStyleListInfo != (StyleListInfo *)-1 )
			{
				// See if GUIDs are equal
				if( ::IsEqualGUID( pCurStyleListInfo->guidFile, pStyleListInfo->guidFile ) )
				{
					nMatch = nCurPos;
					break;
				}
			}
		}
	}

	if( nMatch == CB_ERR )
	{
		StyleListInfo* pSLI;

		// Delete old StyleInfo list
		while( !m_lstStyleListInfo.IsEmpty() )
		{
			pSLI = static_cast<StyleListInfo*>( m_lstStyleListInfo.RemoveHead() );
			delete pSLI;
		}

		// Remove old list from combo box
		m_comboStyle.ResetContent();

		// Add this Style to the combo box list
		nPos = 0;
		m_lstStyleListInfo.AddTail( pStyleListInfo );
		InsertStyleInfoListInComboBox();
	}
	else
	{
		nPos = nMatch;
		delete pStyleListInfo;
		pStyleListInfo = NULL;
	}

LEAVE:
	m_comboStyle.SetCurSel( nPos );

	if( pIProject )
	{
		pIProject->Release();
	}
}


void PropPageStyleRef::GetComboBoxText( const StyleListInfo* pStyleListInfo, CString& strText )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( pStyleListInfo->strProjectName.IsEmpty() )
	{
		strText.Format( "%s %s", pStyleListInfo->strName, pStyleListInfo->strDescriptor );
	}
	else
	{
		strText.Format( "%s: %s %s", pStyleListInfo->strProjectName, pStyleListInfo->strName, pStyleListInfo->strDescriptor );
	}
}

void PropPageStyleRef::InsertStyleInfoListInComboBox( void ) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	CString strText;
	StyleListInfo* pStyleListInfo;
	int nPos;

	POSITION pos = m_lstStyleListInfo.GetHeadPosition();
	while( pos != NULL )
	{
		pStyleListInfo = static_cast<StyleListInfo*>( m_lstStyleListInfo.GetNext(pos) );
		if( pStyleListInfo )
		{
			GetComboBoxText( pStyleListInfo, strText );
			nPos = m_comboStyle.AddString( strText );
			if( nPos >= 0 )
			{
				m_comboStyle.SetItemDataPtr( nPos, pStyleListInfo );
			}
		}
	}
}

void PropPageStyleRef::InsertStyleInfo( StyleListInfo* pStyleListInfo )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	StyleListInfo* pListInfo;
	CString strListText;
	CString strStyleText;
	POSITION posList;

	GetComboBoxText( pStyleListInfo, strStyleText );

	POSITION pos = m_lstStyleListInfo.GetHeadPosition();
	while( pos != NULL )
	{
		posList = pos;

		pListInfo = static_cast<StyleListInfo*>( m_lstStyleListInfo.GetNext(pos) );
		if( pListInfo )
		{
			if( !pStyleListInfo->strProjectName.IsEmpty()
			&&  pListInfo->strProjectName.IsEmpty() )
			{
				continue;
			}

			if( pStyleListInfo->strProjectName.IsEmpty()
			&&  !pListInfo->strProjectName.IsEmpty() )
			{
				if( pos )
				{
					m_lstStyleListInfo.InsertBefore( pos, pStyleListInfo );
				}
				else
				{
					m_lstStyleListInfo.AddTail( pStyleListInfo );
				}
				return;
			}

			GetComboBoxText( pListInfo, strListText );

			if( strListText.CompareNoCase( strStyleText ) > 0 )
			{
				m_lstStyleListInfo.InsertBefore( posList, pStyleListInfo );
				return;
			}
		}
	}

	m_lstStyleListInfo.AddTail( pStyleListInfo );
}


void PropPageStyleRef::BuildStyleInfoList( void ) 
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
	StyleListInfo* pStyleListInfo;

	if( FAILED ( m_pIFramework->FindDocTypeByNodeId( GUID_StyleNode, &pIDocType ) ) )
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
				pStyleListInfo = new StyleListInfo;

				pStyleListInfo->pIProject = pIProject;
//				pStyleListInfo->pIProject->AddRef();	intentionally missing

				if( SUCCEEDED ( pIProject->GetGUID( &guidProject ) ) )
				{
//					if( !IsEqualGUID( guidProject, m_guidProject ) )
					{
						if( SUCCEEDED ( pIProject->GetName( &bstrProjectName ) ) )
						{
							pStyleListInfo->strProjectName = bstrProjectName;
							::SysFreeString( bstrProjectName );
						}
					}

				}

				if( ListInfo.bstrName )
				{
					pStyleListInfo->strName = ListInfo.bstrName;
					::SysFreeString( ListInfo.bstrName );
				}
				
				if( ListInfo.bstrDescriptor )
				{
					pStyleListInfo->strDescriptor = ListInfo.bstrDescriptor;
					::SysFreeString( ListInfo.bstrDescriptor );
				}

				// Update DocRoot file GUID
				m_pIFramework->GetNodeFileGUID ( pIFileNode, &pStyleListInfo->guidFile );

				InsertStyleInfo( pStyleListInfo );

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

void PropPageStyleRef::CopyDataToStyleRef( CPropStyleRef* pStyleRef )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	ASSERT( pStyleRef != NULL );

	m_pStyleRef->m_dwMeasure	 = pStyleRef->m_dwMeasure;
	m_pStyleRef->m_dwBits		 = pStyleRef->m_dwBits;
	m_pStyleRef->m_StyleListInfo = pStyleRef->m_StyleListInfo;
	m_pStyleRef->m_TimeSignature = pStyleRef->m_TimeSignature;

	if( m_pStyleRef->m_pIStyleDocRootNode )
	{
		m_pStyleRef->m_pIStyleDocRootNode->Release();
		m_pStyleRef->m_pIStyleDocRootNode = NULL;
	}
	m_pStyleRef->m_pIStyleDocRootNode = pStyleRef->m_pIStyleDocRootNode;
	if( m_pStyleRef->m_pIStyleDocRootNode )
	{
		m_pStyleRef->m_pIStyleDocRootNode->AddRef();
	}

	memcpy( &m_pStyleRef->m_guidProject, &pStyleRef->m_guidProject, sizeof( m_pStyleRef->m_guidProject ) );
}


void PropPageStyleRef::GetDataFromStyleRef( CPropStyleRef* pStyleRef )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	ASSERT( pStyleRef != NULL );

	pStyleRef->m_dwMeasure		= m_pStyleRef->m_dwMeasure;
	pStyleRef->m_dwBits			= m_pStyleRef->m_dwBits;
	pStyleRef->m_StyleListInfo	= m_pStyleRef->m_StyleListInfo;
	pStyleRef->m_TimeSignature	= m_pStyleRef->m_TimeSignature;

	if( pStyleRef->m_pIStyleDocRootNode )
	{
		pStyleRef->m_pIStyleDocRootNode->Release();
		pStyleRef->m_pIStyleDocRootNode = NULL;
	}
	pStyleRef->m_pIStyleDocRootNode = m_pStyleRef->m_pIStyleDocRootNode;
	if( pStyleRef->m_pIStyleDocRootNode )
	{
		pStyleRef->m_pIStyleDocRootNode->AddRef();
	}

	memcpy( &pStyleRef->m_guidProject, &m_pStyleRef->m_guidProject, sizeof( pStyleRef->m_guidProject ) );
}

void PropPageStyleRef::OnDropDownComboStyle( void ) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	StyleListInfo* pStyleListInfo;

	// Delete old StyleInfo list
	while( !m_lstStyleListInfo.IsEmpty() )
	{
		pStyleListInfo = static_cast<StyleListInfo*>( m_lstStyleListInfo.RemoveHead() );
		delete pStyleListInfo;
	}

	// Remove old list from combo box
	m_comboStyle.ResetContent();

	// Rebuild the StyleInfo list
	BuildStyleInfoList();
	InsertStyleInfoListInComboBox();

	// Select the current Style
	SetStyleComboBoxSelection();
}

BOOL PropPageStyleRef::PreTranslateMessage(MSG* pMsg) 
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
				case VK_ESCAPE:
				{
					CWnd* pWnd = GetFocus();
					if( pWnd )
					{
						switch( pWnd->GetDlgCtrlID() )
						{
							case IDC_EDIT_MEASURE: 
								m_spinMeasure.SetPos( m_pStyleRef->m_dwMeasure + 1 );
								break;
						}
					}
					return TRUE;
				}

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
	}
	
	return CPropertyPage::PreTranslateMessage( pMsg );
}


/////////////////////////////////////////////////////////////////////////////
// PropPageStyleRef::AdjustTime

int PropPageStyleRef::AdjustTime( int nNewValue, int nDelta )
{
	IStyleRefMgr *pIStyleRefMgr;

	if( SUCCEEDED( m_pPropPageMgr->m_pIPropPageObject->QueryInterface( IID_IStyleRefMgr, (void**) &pIStyleRefMgr ) ) )
	{
		// Save the original value
		const int nOrigValue = nNewValue - nDelta;

		while( TRUE )
		{
			HRESULT hr = pIStyleRefMgr->IsMeasureOpen( nNewValue );
			if( FAILED(hr) )
			{
				// The measure is off the end of the segment - there are no open measures
				// after nNewValue.  Check between nOrigvalue and nNewValue
				ASSERT( nDelta > 0 );
				for( int i = nOrigValue + nDelta - 1; i > nOrigValue; i-- )
				{
					if( pIStyleRefMgr->IsMeasureOpen( i ) == S_OK )
					{
						break;
					}
				}

				// If we found an open measure, i will point to it.
				// If we didn't find an open measure, i will be nOrigValue
				nNewValue = i;
				break;
			}
			else
			{
				if( hr == S_OK )
				{
					break;
				}
				else
				{
					nNewValue += nDelta > 0 ? 1 : -1;
					if( nNewValue < 0 )
					{
						// The measure is before the start of the segment - there are no open measures
						// before nNewValue.  Check between nOrigvalue and nNewValue
						ASSERT( nDelta < 0 );
						for( int i = nOrigValue - 1; i > nOrigValue + nDelta; i-- )
						{
							if( pIStyleRefMgr->IsMeasureOpen( i ) == S_OK )
							{
								nNewValue = i;
								break;
							}
						}
						nNewValue = nOrigValue;
						break;
					}
				}
			}
		}
		pIStyleRefMgr->Release();

		return nNewValue;
	}
	else
	{
		return nNewValue;
	}
}
