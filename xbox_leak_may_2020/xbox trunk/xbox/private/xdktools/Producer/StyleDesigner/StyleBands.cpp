// StyleBands.cpp : implementation file
//

#include "stdafx.h"

#include "StyleDesignerDLL.h"
#include "Style.h"
#include "StyleCtl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CStyleBands constructor/destructor

CStyleBands::CStyleBands()
{
	m_pStyle = NULL;
	m_pIDocRootNode = NULL;
	m_pIParentNode = NULL;
	m_fModified = FALSE;
	m_fInCleanUp = FALSE;
}

CStyleBands::~CStyleBands()
{
	IDMUSProdNode* pINode;

	while( !m_lstBands.IsEmpty() )
	{
		pINode = static_cast<IDMUSProdNode*>( m_lstBands.RemoveHead() );
		RELEASE( pINode );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CStyleBands::IsDirty

BOOL CStyleBands::IsDirty( void )
{
	// Check to see if the list of Bands was changed
	if( m_fModified )
	{
		return TRUE;
	}

	// Check to see if any of the Bands were changed
	IDMUSProdNode* pINode;
	IPersistStream* pIPS;
	HRESULT hr;

    POSITION pos = m_lstBands.GetHeadPosition();

    while( pos )
    {
        pINode = m_lstBands.GetNext( pos );

		pINode->QueryInterface( IID_IPersistStream, (void **)&pIPS );
		if( pIPS )
		{
			hr = pIPS->IsDirty();
			RELEASE( pIPS );

			if( hr == S_OK )
			{
				return TRUE;
			}
		}
    }

	return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleBands::SetModified

void CStyleBands::SetModified( BOOL fModified )
{
	// Set modified flag of Band folder
	m_fModified = fModified;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleBands::CleanUp

void CStyleBands::CleanUp( void )
{
	m_fInCleanUp = TRUE;

	IDMUSProdNode* pINode;
	while( !m_lstBands.IsEmpty() )
	{
		pINode = static_cast<IDMUSProdNode*>( m_lstBands.RemoveHead() );
		pINode->DeleteNode( FALSE );
		RELEASE( pINode );
	}

	m_fInCleanUp = FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleBands::InsertChildNodeAtPos

HRESULT CStyleBands::InsertChildNodeAtPos( IDMUSProdNode* pIChildNode, IDMUSProdNode* pIPositionNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( pIChildNode != NULL );
	ASSERT( pIPositionNode != NULL );
	ASSERT( m_pStyle != NULL );
	ASSERT( theApp.m_pStyleComponent != NULL );
	ASSERT( theApp.m_pStyleComponent->m_pIFramework != NULL );

	pIChildNode->AddRef();

	// add to CDirectMusicStyle Band list
	POSITION pos = NULL;

	if( pIPositionNode )
	{
		pos = m_lstBands.Find( pIPositionNode );
	}
	if( pos )
	{
		m_lstBands.InsertBefore( pos, pIChildNode );
	}
	else
	{
		m_lstBands.AddTail( pIChildNode );
	}

	// Set root and parent node of ALL children
	theApp.SetNodePointers( pIChildNode, (IDMUSProdNode *)m_pStyle, (IDMUSProdNode *)this );

	// Make sure Band name is unique
	m_pStyle->GetUniqueBandName( pIChildNode );

	// Add node to Project Tree
	if( !SUCCEEDED ( theApp.m_pStyleComponent->m_pIFramework->AddNode(pIChildNode, (IDMUSProdNode *)this) ) )
	{
		DeleteChildNode( pIChildNode, FALSE );
		return E_FAIL;
	}

	// Make sure "Default" flag is off
	IDMUSProdBandEdit* pIBandEdit;

	if( SUCCEEDED ( pIChildNode->QueryInterface( IID_IDMUSProdBandEdit, (void**)&pIBandEdit ) ) )
	{
		pIBandEdit->SetDefaultFlag( FALSE );
		RELEASE( pIBandEdit );
	}

	// Sync Style editor (when open)
	m_pStyle->SyncStyleEditor( SSE_BANDS );
	if( m_pStyle->m_pStyleCtrl
	&&  m_pStyle->m_pStyleCtrl->m_pStyleDlg )
	{
		m_pStyle->m_pStyleCtrl->m_pStyleDlg->SelectBand( pIChildNode );
	}

	// Update the DirectMusic Style object
	HRESULT hr = m_pStyle->SyncStyleWithDirectMusic();
	ASSERT( SUCCEEDED ( hr ) );

	SetModified( TRUE );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleBands IUnknown implementation

HRESULT CStyleBands::QueryInterface( REFIID riid, LPVOID FAR* ppvObj )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    if( ::IsEqualIID(riid, IID_IDMUSProdNode)
	||  ::IsEqualIID(riid, IID_IUnknown) )
    {
        AddRef();
        *ppvObj = this;
        return S_OK;
    }

    if( ::IsEqualIID(riid, IID_IDMUSProdPropPageObject) )
    {
		if( m_pStyle )
		{
	        return m_pStyle->QueryInterface( riid, ppvObj );
		}

		*ppvObj = NULL;
		return E_NOINTERFACE;
    }

    *ppvObj = NULL;
    return E_NOINTERFACE;
}

ULONG CStyleBands::AddRef()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pStyle != NULL );

	AfxOleLockApp(); 

	return m_pStyle->AddRef();
}

ULONG CStyleBands::Release()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pStyle != NULL );

	AfxOleUnlockApp(); 

	return m_pStyle->Release();
}


/////////////////////////////////////////////////////////////////////////////
// CStyleBands IDMUSProdNode implementation

/////////////////////////////////////////////////////////////////////////////
// CStyleBands IDMUSProdNode::GetNodeImageIndex

HRESULT CStyleBands::GetNodeImageIndex( short* pnFirstImage )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( theApp.m_pStyleComponent != NULL );

	return( theApp.m_pStyleComponent->GetFolderImageIndex(pnFirstImage) );
}


/////////////////////////////////////////////////////////////////////////////
// CStyleBands IDMUSProdNode::GetFirstChild

HRESULT CStyleBands::GetFirstChild( IDMUSProdNode** ppIFirstChildNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( ppIFirstChildNode == NULL )
	{
		return E_POINTER;
	}
	
	*ppIFirstChildNode = NULL;

	if( !m_lstBands.IsEmpty() )
	{
		IDMUSProdNode* pINode = static_cast<IDMUSProdNode*>( m_lstBands.GetHead() );
		
		if( pINode )
		{
			pINode->AddRef();
			*ppIFirstChildNode = pINode;
		}
	}

    return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleBands IDMUSProdNode::GetNextChild

HRESULT CStyleBands::GetNextChild( IDMUSProdNode* pIChildNode, IDMUSProdNode** ppINextChildNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( ppINextChildNode == NULL )
	{
		return E_POINTER;
	}
	
	*ppINextChildNode = NULL;

	if( pIChildNode == NULL )
	{
		return E_INVALIDARG;
	}

	IDMUSProdNode* pINode;

    POSITION pos = m_lstBands.GetHeadPosition();

    while( pos )
    {
        pINode = m_lstBands.GetNext( pos );
		if( pINode == pIChildNode )
		{
			if( pos )
			{
				pINode = m_lstBands.GetNext( pos );

				pINode->AddRef();
				*ppINextChildNode = pINode;
			}
			break;
		}
    }

    return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleBands IDMUSProdNode::GetComponent

HRESULT CStyleBands::GetComponent( IDMUSProdComponent** ppIComponent )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( theApp.m_pStyleComponent != NULL );

	return theApp.m_pStyleComponent->QueryInterface( IID_IDMUSProdComponent, (void**)ppIComponent );
}


/////////////////////////////////////////////////////////////////////////////
// CStyleBands IDMUSProdNode::GetDocRootNode

HRESULT CStyleBands::GetDocRootNode( IDMUSProdNode** ppIDocRootNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pIDocRootNode )
	{
		m_pIDocRootNode->AddRef();
		*ppIDocRootNode = m_pIDocRootNode;
		return S_OK;
	}

	*ppIDocRootNode = NULL;
	return E_FAIL;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleBands IDMUSProdNode::SetDocRootNode

HRESULT CStyleBands::SetDocRootNode( IDMUSProdNode* pIDocRootNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( pIDocRootNode != NULL );

	m_pIDocRootNode = pIDocRootNode;
//	m_pIDocRootNode->AddRef();		intentionally missing

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleBands IDMUSProdNode::GetParentNode

HRESULT CStyleBands::GetParentNode( IDMUSProdNode** ppIParentNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pIParentNode != NULL );

	m_pIParentNode->AddRef();
	*ppIParentNode = m_pIParentNode;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleBands IDMUSProdNode::SetParentNode

HRESULT CStyleBands::SetParentNode( IDMUSProdNode* pIParentNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( pIParentNode != NULL );

	m_pIParentNode = pIParentNode;
//	m_pIParentNode->AddRef();		intentionally missing

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleBands IDMUSProdNode::GetNodeId

HRESULT CStyleBands::GetNodeId( GUID* pguid )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pguid == NULL )
	{
		return E_POINTER;
	}

	*pguid = GUID_StyleBandFolderNode;

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CStyleBands IDMUSProdNode::GetNodeName

HRESULT CStyleBands::GetNodeName( BSTR* pbstrName )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CString strName;
	TCHAR achBuffer[MID_BUFFER];

	if( ::LoadString(theApp.m_hInstance, IDS_BAND_FOLDER_NAME, achBuffer, MID_BUFFER) )
	{
		strName = achBuffer;
	}

    *pbstrName = strName.AllocSysString();

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleBands IDMUSProdNode::GetNodeNameMaxLength

HRESULT CStyleBands::GetNodeNameMaxLength( short* pnMaxLength )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	*pnMaxLength = -1;	// Can't rename a band folder

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleBands IDMUSProdNode::ValidateNodeName

HRESULT CStyleBands::ValidateNodeName( BSTR bstrName )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(bstrName);

	return E_NOTIMPL;	// Can't rename a band folder
}


/////////////////////////////////////////////////////////////////////////////
// CStyleBands IDMUSProdNode::SetNodeName

HRESULT CStyleBands::SetNodeName( BSTR bstrName )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(bstrName);

	return E_NOTIMPL;	// Can't rename a band folder
}


/////////////////////////////////////////////////////////////////////////////
// CStyleBands IDMUSProdNode::GetNodeListInfo

HRESULT CStyleBands::GetNodeListInfo( DMUSProdListInfo* pListInfo )
{
//	AFX_MANAGE_STATE( _afxModuleAddrThis );
	UNREFERENCED_PARAMETER(pListInfo);

	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleBands IDMUSProdNode::GetEditorClsId

HRESULT CStyleBands::GetEditorClsId( CLSID* pClsId )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(pClsId);

	return E_NOTIMPL;	// Can't edit a band folder
}


/////////////////////////////////////////////////////////////////////////////
// CStyleBands IDMUSProdNode::GetEditorTitle

HRESULT CStyleBands::GetEditorTitle( BSTR* pbstrTitle )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(pbstrTitle);

	return E_NOTIMPL;	// Can't edit a band folder
}


/////////////////////////////////////////////////////////////////////////////
// CStyleBands IDMUSProdNode::GetEditorWindow

HRESULT CStyleBands::GetEditorWindow( HWND* hWndEditor )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(hWndEditor);

	return E_NOTIMPL;	// Can't edit a band folder
}


/////////////////////////////////////////////////////////////////////////////
// CStyleBands IDMUSProdNode::SetEditorWindow

HRESULT CStyleBands::SetEditorWindow( HWND hWndEditor )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(hWndEditor);

	return E_NOTIMPL;	// Can't edit a band folder
}


/////////////////////////////////////////////////////////////////////////////
// CStyleBands IDMUSProdNode::UseOpenCloseImages

HRESULT CStyleBands::UseOpenCloseImages( BOOL* pfUseOpenCloseImages )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	*pfUseOpenCloseImages = TRUE;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleBands IDMUSProdNode::GetRightClickMenuId

HRESULT CStyleBands::GetRightClickMenuId( HINSTANCE* phInstance, UINT* pnMenuId )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	*phInstance = theApp.m_hInstance;
	*pnMenuId   = IDM_STYLEBANDS_NODE_RMENU;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleBands IDMUSProdNode::OnRightClickMenuInit

HRESULT CStyleBands::OnRightClickMenuInit( HMENU hMenu )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CMenu menu;

	if( menu.Attach(hMenu) )
	{
		IDMUSProdPropPageObject* pIPageObject;

		// IDM_PASTE
		{
			IDataObject* pIDataObject;
			BOOL fWillSetReference;

			menu.EnableMenuItem( IDM_PASTE, (MF_GRAYED | MF_BYCOMMAND) );

			if( SUCCEEDED ( ::OleGetClipboard( &pIDataObject ) ) )
			{
				if( CanPasteFromData( pIDataObject, &fWillSetReference ) == S_OK )
				{
					menu.EnableMenuItem( IDM_PASTE, (MF_ENABLED | MF_BYCOMMAND) );
				}
				
				pIDataObject->Release();
			}
		}

		// IDM_PROPERTIES
		if( SUCCEEDED ( QueryInterface( IID_IDMUSProdPropPageObject, (void **)&pIPageObject ) ) )
		{
			menu.EnableMenuItem( IDM_PROPERTIES, (MF_ENABLED | MF_BYCOMMAND) );
			RELEASE( pIPageObject );
		}
		else
		{
			menu.EnableMenuItem( IDM_PROPERTIES, (MF_GRAYED | MF_BYCOMMAND) );
		}

		menu.Detach();
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleBands IDMUSProdNode::OnRightClickMenuSelect

HRESULT CStyleBands::OnRightClickMenuSelect( long lCommandId )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pStyle != NULL );

	HRESULT hr = E_FAIL;

	switch( lCommandId )
	{
		case IDM_NEW_BAND:
			hr = InsertChildNode( NULL );
			break;

		case IDM_PASTE:
		{
			IDataObject* pIDataObject;
			BOOL fWillSetReference;

			if( SUCCEEDED ( ::OleGetClipboard( &pIDataObject ) ) )
			{
				if( CanPasteFromData( pIDataObject, &fWillSetReference ) == S_OK )
				{
					PasteFromData( pIDataObject );
				}
				
				RELEASE( pIDataObject );
			}
			break;
		}

		case IDM_PROPERTIES:
		{
			IDMUSProdPropPageObject* pIPageObject;

			if( SUCCEEDED ( QueryInterface( IID_IDMUSProdPropPageObject, (void **)&pIPageObject ) ) )
			{
				pIPageObject->OnShowProperties();
				RELEASE( pIPageObject );
			}
			hr = S_OK;
			break;
		}
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleBands IDMUSProdNode::DeleteChildNode

HRESULT CStyleBands::DeleteChildNode( IDMUSProdNode* pIChildNode, BOOL fPromptUser )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( pIChildNode != NULL );
	ASSERT( m_pStyle != NULL );
	ASSERT( theApp.m_pStyleComponent != NULL );
	ASSERT( theApp.m_pStyleComponent->m_pIFramework != NULL );

	if( m_fInCleanUp )
	{
		// Nothing to do
		return S_OK;
	}

	CWaitCursor wait;

	if( pIChildNode == NULL )
	{
		return E_INVALIDARG;
	}

	// Remove node from Project Tree
	if( theApp.m_pStyleComponent->m_pIFramework->RemoveNode( pIChildNode, fPromptUser ) == S_FALSE )
	{
		return E_FAIL;
	}

	// Reset Style's active Band to NULL (when applicable)
	m_pStyle->RemoveActiveBand( pIChildNode );

	// Save undo state
//	m_pStyle->m_pINodeBeingDeleted = pIChildNode;
//	m_pStyle->m_pUndoMgr->SaveState( m_pStyle, theApp.m_hInstance, IDS_UNDO_DELETE_BAND );
//	m_pStyle->m_pINodeBeingDeleted = NULL;

	// Need to keep node until after SyncStyleEditor() 
	pIChildNode->AddRef();

	// Remove from CDirectMusicStyle Band list
	POSITION pos = m_lstBands.Find( pIChildNode );
	if( pos )
	{
		m_lstBands.RemoveAt( pos );
		pIChildNode->Release();
	}

	// Make sure Style still has a "Default" Band
	IDMUSProdBandEdit* pIBandEdit;

	if( SUCCEEDED ( pIChildNode->QueryInterface( IID_IDMUSProdBandEdit, (void**)&pIBandEdit ) ) )
	{
		BOOL fDefaultFlag;

		if( FAILED ( pIBandEdit->GetDefaultFlag( &fDefaultFlag ) ) )
		{
			fDefaultFlag = FALSE;
		}
		if( fDefaultFlag )
		{
			m_pStyle->SetDefaultBand( NULL, FALSE );
		}

		RELEASE( pIBandEdit );
	}

	// Sync Style editor (when open)
	m_pStyle->SyncStyleEditor( SSE_BANDS );
	RELEASE( pIChildNode );

	// Update the DirectMusic Style object
	HRESULT hr = m_pStyle->SyncStyleWithDirectMusic();
	ASSERT( SUCCEEDED ( hr ) );

	SetModified( TRUE );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleBands IDMUSProdNode::InsertChildNode

HRESULT CStyleBands::InsertChildNode( IDMUSProdNode* pIChildNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	IDMUSProdDocType* pIDocType;

	ASSERT( m_pStyle != NULL );
	ASSERT( theApp.m_pStyleComponent != NULL );
	ASSERT( theApp.m_pStyleComponent->m_pIFramework != NULL );

	if( pIChildNode == NULL )
	{
		HRESULT hr;

		hr = theApp.m_pStyleComponent->m_pIFramework->FindDocTypeByNodeId( GUID_BandNode, &pIDocType );
		if( SUCCEEDED ( hr ) ) 
		{
			// Create a new Band 
			hr = pIDocType->AllocNode( GUID_BandNode, &pIChildNode );
			if( !SUCCEEDED ( hr ) )
			{
				pIChildNode = NULL;
			}

			RELEASE( pIDocType );
		}

		if( pIChildNode == NULL )
		{
			return E_OUTOFMEMORY;
		}
	}
	else
	{
		pIChildNode->AddRef();
	}

	// Make sure Band name is unique
	m_pStyle->GetUniqueBandName( pIChildNode );

	// Add to CDirectMusicStyle Band list
	m_lstBands.AddTail( pIChildNode );

	// Set root and parent node of ALL children
	theApp.SetNodePointers( pIChildNode, (IDMUSProdNode *)m_pStyle, (IDMUSProdNode *)this );

	// Add node to Project Tree
	if( !SUCCEEDED ( theApp.m_pStyleComponent->m_pIFramework->AddNode(pIChildNode, (IDMUSProdNode *)this) ) )
	{
		DeleteChildNode( pIChildNode, FALSE );
		return E_FAIL;
	}

	// Make sure "Default" flag is off
	IDMUSProdBandEdit* pIBandEdit;

	if( SUCCEEDED ( pIChildNode->QueryInterface( IID_IDMUSProdBandEdit, (void**)&pIBandEdit ) ) )
	{
		pIBandEdit->SetDefaultFlag( FALSE );
		RELEASE( pIBandEdit );
	}

	// Sync Style editor (when open)
	m_pStyle->SyncStyleEditor( SSE_BANDS );
	if( m_pStyle->m_pStyleCtrl
	&&  m_pStyle->m_pStyleCtrl->m_pStyleDlg )
	{
		m_pStyle->m_pStyleCtrl->m_pStyleDlg->SelectBand( pIChildNode );
	}

	// Update the DirectMusic Style object
	HRESULT hr = m_pStyle->SyncStyleWithDirectMusic();
	ASSERT( SUCCEEDED ( hr ) );

	SetModified( TRUE );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleBands IDMUSProdNode::DeleteNode

HRESULT CStyleBands::DeleteNode( BOOL fPromptUser )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(fPromptUser);

	return E_NOTIMPL;	// Can't delete a Band folder
}


/////////////////////////////////////////////////////////////////////////////
// CStyleBands IDMUSProdNode::OnNodeSelChanged

HRESULT CStyleBands::OnNodeSelChanged( BOOL fSelected )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(fSelected);

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleBands IDMUSProdNode::CreateDataObject

HRESULT CStyleBands::CreateDataObject( IDataObject** ppIDataObject )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( ppIDataObject == NULL )
	{
		return E_POINTER;
	}

	*ppIDataObject = NULL;

	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleBands IDMUSProdNode::CanCut

HRESULT CStyleBands::CanCut( void )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
 
	return S_FALSE;		// Can't remove Band folder from Style
}


/////////////////////////////////////////////////////////////////////////////
// CStyleBands IDMUSProdNode::CanCopy

HRESULT CStyleBands::CanCopy( void )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
 
	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleBands IDMUSProdNode::CanDelete

HRESULT CStyleBands::CanDelete( void )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return S_FALSE;		// Can't delete Band folder from Style
}


/////////////////////////////////////////////////////////////////////////////
// CStyleBands IDMUSProdNode::CanDeleteChildNode

HRESULT CStyleBands::CanDeleteChildNode( IDMUSProdNode* pIChildNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Make sure node is in Band list
	POSITION pos = m_lstBands.Find( pIChildNode );
	if( pos == NULL )
	{
		return E_FAIL;
	}

	// Cannot delete the last Band from a Style
	if( m_lstBands.GetCount() < 2 )
	{
		return S_FALSE;
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleBands IDMUSProdNode::CanPasteFromData

HRESULT CStyleBands::CanPasteFromData( IDataObject* pIDataObject, BOOL* pfWillSetReference )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pfWillSetReference == NULL )
	{
		return E_POINTER;
	}

	*pfWillSetReference = FALSE;

	if( pIDataObject == NULL )
	{
		return E_INVALIDARG;
	}

	// Create a new CDllJazzDataObject and see if it can read the data object's format.
	CDllJazzDataObject* pDataObject = new CDllJazzDataObject();
	if( pDataObject == NULL )
	{
		return E_OUTOFMEMORY;
	}

	HRESULT hr = S_FALSE;
	
	if( SUCCEEDED (	pDataObject->IsClipFormatAvailable( pIDataObject, theApp.m_pStyleComponent->m_cfBand ) )
	||  SUCCEEDED (	pDataObject->IsClipFormatAvailable( pIDataObject, theApp.m_pStyleComponent->m_cfBandList ) ) 
	||  SUCCEEDED (	pDataObject->IsClipFormatAvailable( pIDataObject, theApp.m_pStyleComponent->m_cfBandTrack ) ) 
	||  SUCCEEDED ( m_pStyle->ContainsClipFormat( pDataObject, pIDataObject, theApp.m_pStyleComponent->m_cfBandTrack ) ) ) 
	{
		hr = S_OK;
	}

	RELEASE( pDataObject );
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleBands IDMUSProdNode::PasteFromData

HRESULT CStyleBands::PasteFromData( IDataObject* pIDataObject )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pStyle != NULL );

	if( pIDataObject == NULL )
	{
		return E_INVALIDARG;
	}

	// Do not continue unless CStyleBands can accept data
	BOOL fWillSetReference;
	if( CanPasteFromData( pIDataObject, &fWillSetReference ) != S_OK )
	{
		return E_FAIL;
	}

	// Let CDirectMusicStyle do the work
	return m_pStyle->PasteFromData( pIDataObject );
}


/////////////////////////////////////////////////////////////////////////////
// CStyleBands IDMUSProdNode::CanChildPasteFromData

HRESULT CStyleBands::CanChildPasteFromData( IDataObject* pIDataObject, IDMUSProdNode* pIChildNode,
										    BOOL *pfWillSetReference )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(pIChildNode);

	if( pfWillSetReference == NULL )
	{
		return E_POINTER;
	}

	*pfWillSetReference = FALSE;

	if( pIDataObject == NULL )
	{
		return E_INVALIDARG;
	}

	// Create a new CDllJazzDataObject and see if it can read the data object's format.
	CDllJazzDataObject* pDataObject = new CDllJazzDataObject();
	if( pDataObject == NULL )
	{
		return E_OUTOFMEMORY;
	}

	HRESULT hr = S_FALSE;
	
	if( SUCCEEDED (	pDataObject->IsClipFormatAvailable( pIDataObject, theApp.m_pStyleComponent->m_cfBand ) )
	||  SUCCEEDED (	pDataObject->IsClipFormatAvailable( pIDataObject, theApp.m_pStyleComponent->m_cfBandList ) ) 
	||  SUCCEEDED (	pDataObject->IsClipFormatAvailable( pIDataObject, theApp.m_pStyleComponent->m_cfBandTrack ) ) 
	||  SUCCEEDED ( m_pStyle->ContainsClipFormat( pDataObject, pIDataObject, theApp.m_pStyleComponent->m_cfBandTrack ) ) )
	{
		hr = S_OK;
	}

	RELEASE( pDataObject );
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleBands IDMUSProdNode::ChildPasteFromData

HRESULT CStyleBands::ChildPasteFromData( IDataObject* pIDataObject, IDMUSProdNode* pIChildNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pStyle != NULL );

	if( pIDataObject == NULL
	||  pIChildNode == NULL )
	{
		return E_INVALIDARG;
	}

	// Create a new CDllJazzDataObject to get the data object's stream.
	CDllJazzDataObject* pDataObject = new CDllJazzDataObject();
	if( pDataObject == NULL )
	{
		return E_OUTOFMEMORY;
	}

	HRESULT hr = E_FAIL;

	if( SUCCEEDED ( pDataObject->IsClipFormatAvailable( pIDataObject, theApp.m_pStyleComponent->m_cfBand ) ) )
	{
		// Handle CF_BAND format
		hr = m_pStyle->PasteCF_BAND( pDataObject, pIDataObject, NULL );
	}

	else if( SUCCEEDED ( pDataObject->IsClipFormatAvailable( pIDataObject, theApp.m_pStyleComponent->m_cfBandList ) ) )
	{
		// Handle CF_BANDLIST format
		hr = m_pStyle->PasteCF_BANDLIST( pDataObject, pIDataObject, NULL );
	}

	else if( SUCCEEDED ( pDataObject->IsClipFormatAvailable( pIDataObject, theApp.m_pStyleComponent->m_cfBandTrack ) ) )
	{
		// Handle CF_BANDTRACK format
		hr = m_pStyle->PasteCF_BANDTRACK( pDataObject, pIDataObject, NULL );
	}

	else if( SUCCEEDED ( pDataObject->IsClipFormatAvailable( pIDataObject, theApp.m_pStyleComponent->m_cfTimeline ) ) )
	{
		// Handle CF_TIMELINE format
		hr = m_pStyle->PasteCF_TIMELINE ( pDataObject, pIDataObject, NULL );
	}

	RELEASE( pDataObject );
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleBands IDMUSProdNode::GetObject

HRESULT CStyleBands::GetObject( REFCLSID rclsid, REFIID riid, void** ppvObject )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(rclsid);
	UNREFERENCED_PARAMETER(riid);
	UNREFERENCED_PARAMETER(ppvObject);

	return E_NOTIMPL;
}
