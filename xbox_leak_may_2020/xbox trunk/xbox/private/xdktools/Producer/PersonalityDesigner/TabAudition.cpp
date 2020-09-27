// TabAudition.cpp : implementation file
//

#include "stdafx.h"
#pragma warning(disable:4201)
#include "DMUSProd.h"
#include "StyleDesigner.h"
#include "PersonalityDesigner.h"
#include "TabAudition.h"
#include "Chord.h"
#pragma warning(default:4201)

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTabAudition property page

CTabAudition::CTabAudition( CPersonalityPageManager* pPageManager, IDMUSProdFramework* pIFramework ) : CPropertyPage(CTabAudition::IDD)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	//{{AFX_DATA_INIT(CTabAudition)
	//}}AFX_DATA_INIT

	ASSERT( pPageManager != NULL );
	ASSERT( pIFramework != NULL );

	m_pIFramework = pIFramework;
	m_pIFramework->AddRef();

	m_pPageManager = pPageManager;
//	m_pPageManager->AddRef();		intentionally missing

	m_fHaveData = FALSE;
	m_fInOnSetActive = FALSE;
	m_fNeedToDetach = FALSE;

	m_pIStyleDocRootNode = NULL;
	m_dwGroove = 1;
	memset( &m_guidProject, 0, sizeof( m_guidProject ) );
}

CTabAudition::~CTabAudition()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

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

	if( m_pIStyleDocRootNode )
	{
		m_pIStyleDocRootNode->Release();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabAudition::CopyDataToTab

void CTabAudition::CopyDataToTab( tabPersonality* pTabData )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pIStyleDocRootNode )
	{
		m_pIStyleDocRootNode->Release();
		m_pIStyleDocRootNode = NULL;
	}

	if( pTabData )
	{
		m_fHaveData = TRUE;
		m_dwGroove = pTabData->dwGroove;
		memcpy( &m_guidProject, &pTabData->guidProject, sizeof( m_guidProject ) );

		m_pIStyleDocRootNode = pTabData->pIStyleDocRootNode;
		if( m_pIStyleDocRootNode )
		{
			m_pIStyleDocRootNode->AddRef();
		}

		m_pPersonality = pTabData->pPersonality;
	}
	else
	{
		m_fHaveData = FALSE;
		m_dwGroove = 0;
		memset( &m_guidProject, 0, sizeof( m_guidProject ) );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabAudition::GetDataFromTab

void CTabAudition::GetDataFromTab( tabPersonality* pTabData )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	pTabData->pIStyleDocRootNode = m_pIStyleDocRootNode;
	pTabData->dwGroove = m_dwGroove;
	memcpy( &pTabData->guidProject, &m_guidProject, sizeof( pTabData->guidProject ) );
}


/////////////////////////////////////////////////////////////////////////////
// CTabAudition::EnableControls

void CTabAudition::EnableControls( BOOL fEnable ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	m_comboStyle.EnableWindow( fEnable );
	m_spinGroove.EnableWindow(fEnable);
	m_editGroove.EnableWindow(fEnable);
}


/////////////////////////////////////////////////////////////////////////////
// CTabAudition::SetGroove

void CTabAudition::SetGroove( void ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CString strGroove;
	strGroove.Format( "%d", m_dwGroove );
	m_editGroove.SetWindowText( strGroove );
}


/////////////////////////////////////////////////////////////////////////////
// CTabAudition::SetStyle

void CTabAudition::SetStyle( void ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pIFramework != NULL );

	IDMUSProdProject* pIProject = NULL;
	CString strText;
	int nMatch;
	int nCount;
	int nCurPos;
	int nPos = -1;

	if( m_pIStyleDocRootNode == NULL )
	{
		SetStyleDocRootNode();
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
	m_pIFramework->GetNodeFileGUID(	m_pIStyleDocRootNode, &pStyleListInfo->guidFile );

	if( m_pIStyleDocRootNode != NULL && SUCCEEDED ( m_pIStyleDocRootNode->GetNodeListInfo ( &ListInfo ) ) )
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

		if( FAILED ( m_pIFramework->FindProject( m_pIStyleDocRootNode, &pIProject ) ) )
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

		if( !IsEqualGUID( guidProject, m_guidProject ) )
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

	// List is only built in OnDropDownComboStyle
	// If list is empty and we have a Style,
	//	just put the one item in the list
	if( m_lstStyleListInfo.IsEmpty() )
	{
		nPos = 0;
		m_lstStyleListInfo.AddTail( pStyleListInfo );
		InsertStyleInfoListInComboBox();
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

	delete pStyleListInfo;
	pStyleListInfo = NULL;

	if( nMatch != CB_ERR )
	{
		nPos = nMatch;
	}


LEAVE:
	m_comboStyle.SetCurSel( nPos );

	if( pIProject )
	{
		pIProject->Release();
	}
}


void CTabAudition::DoDataExchange(CDataExchange* pDX)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTabAudition)
	DDX_Control(pDX, IDC_SPIN_GROOVE, m_spinGroove);
	DDX_Control(pDX, IDC_EDIT_GROOVE, m_editGroove);
	DDX_Control(pDX, IDC_COMBO_STYLE, m_comboStyle);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTabAudition, CPropertyPage)
	//{{AFX_MSG_MAP(CTabAudition)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_CBN_SELCHANGE(IDC_COMBO_STYLE, OnSelChangeComboStyle)
	ON_CBN_DROPDOWN(IDC_COMBO_STYLE, OnDropDownComboStyle)
	ON_EN_UPDATE(IDC_EDIT_GROOVE, OnUpdateEditGroove)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_GROOVE, OnDeltaposSpinGroove)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CTabAudition message handlers

/////////////////////////////////////////////////////////////////////////////
// CTabAudition:OnSetActive

BOOL CTabAudition::OnSetActive( void ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Set enable state of controls
	BOOL fEnable;

	if( m_fHaveData )
	{
		fEnable = TRUE;
	}
	else
	{
		fEnable = FALSE;
	}
	EnableControls( fEnable );

	// Place values in controls
	m_fInOnSetActive = TRUE;
	SetStyle();
	SetGroove();
	m_fInOnSetActive = FALSE;
	
	return CPropertyPage::OnSetActive();
}


/////////////////////////////////////////////////////////////////////////////
// CTabAudition::OnInitDialog

BOOL CTabAudition::OnInitDialog( void ) 
{
	// Need this AFX_MANAGE_STATE so that resource defined
	// combo box strings can be found
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CPropertyPage::OnInitDialog();


	m_editGroove.LimitText(3);
	m_spinGroove.SetRange( 1,100 );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


/////////////////////////////////////////////////////////////////////////////
// CTabAudition::OnCreate

int CTabAudition::OnCreate( LPCREATESTRUCT lpCreateStruct ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

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
// CTabAudition::OnDestroy

void CTabAudition::OnDestroy( void ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

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
// CTabAudition::OnSelChangeComboStyle

void CTabAudition::OnSelChangeComboStyle( void ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pIFramework != NULL );

	if( m_fInOnSetActive )
	{
		return;
	}

	int nItem = m_comboStyle.GetCurSel();
	if( nItem != CB_ERR )
	{
		StyleListInfo* pStyleListInfo = (StyleListInfo *)m_comboStyle.GetItemDataPtr( nItem );
		if( pStyleListInfo != (StyleListInfo *)-1 )
		{
			IDMUSProdNode* pIStyleDocRootNode;
			if(SUCCEEDED(m_pIFramework->FindDocRootNodeByFileGUID(pStyleListInfo->guidFile, &pIStyleDocRootNode)))
			{
				if( m_pIStyleDocRootNode )
				{
					m_pIStyleDocRootNode->Release();
					m_pIStyleDocRootNode = NULL;
				}
				m_pIStyleDocRootNode = pIStyleDocRootNode;
				m_pPageManager->UpdateObjectWithTabData();
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabAudition::GetComboBoxText

void CTabAudition::GetComboBoxText( StyleListInfo* pStyleListInfo, CString& strText )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pStyleListInfo->strProjectName.IsEmpty() )
	{
		strText.Format( "%s %s", pStyleListInfo->strName, pStyleListInfo->strDescriptor );
	}
	else
	{
		strText.Format( "%s: %s %s", pStyleListInfo->strProjectName, pStyleListInfo->strName, pStyleListInfo->strDescriptor );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CTabAudition::InsertStyleInfo

void CTabAudition::InsertStyleInfo( StyleListInfo* pStyleListInfo )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

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


/////////////////////////////////////////////////////////////////////////////
// CTabAudition::BuildStyleInfoList

void CTabAudition::BuildStyleInfoList( void ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

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

				m_pIFramework->GetNodeFileGUID( pIFileNode, &pStyleListInfo->guidFile );

				if( SUCCEEDED ( pIProject->GetGUID( &guidProject ) ) )
				{
					if( !IsEqualGUID( guidProject, m_guidProject ) )
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


/////////////////////////////////////////////////////////////////////////////
// CTabAudition::InsertStyleInfoListInComboBox

void CTabAudition::InsertStyleInfoListInComboBox( void ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

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


/////////////////////////////////////////////////////////////////////////////
// CTabAudition::OnDropDownComboStyle

void CTabAudition::OnDropDownComboStyle( void ) 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

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
	SetStyle();
}

void CTabAudition::SetStyleDocRootNode()
//
// Similar to BuildStyleInfoList, but extracts first style and sets doc root node.
//
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT(m_pIFramework);

	IDMUSProdDocType* pIDocType;
	IDMUSProdProject* pIProject;
	IDMUSProdNode* pIFileNode;

	if( FAILED ( m_pIFramework->FindDocTypeByNodeId( GUID_StyleNode, &pIDocType ) ) )
	{
		return;
	}

	HRESULT hr = m_pIFramework->FindProject(m_pPersonality, &pIProject);

	hr = pIProject->GetFirstFileByDocType( pIDocType, &pIFileNode );

	if(SUCCEEDED(hr))
	{
		if(m_pIStyleDocRootNode)
		{
			m_pIStyleDocRootNode->Release();
		}
		m_pIStyleDocRootNode = pIFileNode;
		pIProject->Release();
	}

	pIDocType->Release();

}

void CTabAudition::OnUpdateEditGroove() 
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);


	if( m_fInOnSetActive )
	{
		return;
	}

	CString strGroove;

	if( ::IsWindow(m_editGroove.m_hWnd) == FALSE )
	{
		return;
	}

	m_editGroove.GetWindowText( strGroove );

	DWORD nGroove = (short)atoi( strGroove );
	if(nGroove < 1)
	{
		m_dwGroove = 100;
	}
	else if(nGroove >100)
	{
		m_dwGroove = 1;
	}
	else
	{
		m_dwGroove = nGroove;
	}

	if(nGroove != m_dwGroove)
	{
		m_fInOnSetActive = true;
		m_editGroove.SetWindowText( strGroove );
		m_fInOnSetActive = false;
	}

	m_pPageManager->UpdateObjectWithTabData();
	
}

void CTabAudition::OnDeltaposSpinGroove(NMHDR* pNMHDR, LRESULT* pResult) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	UNREFERENCED_PARAMETER(pNMHDR);

/*  Removed: causes control to be set twice, first time (from here) in the wrong direction (down instead of up and vice versa)
	if( m_fInOnSetActive )
	{
		return;
	}

	CString strGroove;

	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

	DWORD dwGroove = m_dwGroove - pNMUpDown->iDelta;

	if(dwGroove < 1)
	{
		m_dwGroove = 100;
	}
	else if(dwGroove >100)
	{
		m_dwGroove = 1;
	}
	else
	{
		m_dwGroove = dwGroove;
	}

	SetGroove();
*/
	*pResult = 0;
}
