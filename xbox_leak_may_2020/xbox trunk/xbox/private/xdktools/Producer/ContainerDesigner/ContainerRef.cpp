// ContainerRef.cpp : implementation file
//

#include "stdafx.h"
#include "ContainerDesignerDLL.h"

#include "Container.h"
#include "ContainerRef.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CContainerRef constructor/destructor

CContainerRef::CContainerRef()
{
    m_dwRef = 0;
	AddRef();

	m_pIDocRootNode = NULL;
	m_pIParentNode = NULL;
	m_pContainer = NULL;
}

CContainerRef::~CContainerRef()
{
	RELEASE( m_pContainer );
}


/////////////////////////////////////////////////////////////////////////////
// CContainerRef IUnknown implementation

HRESULT CContainerRef::QueryInterface( REFIID riid, LPVOID FAR* ppvObj )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    if( ::IsEqualIID(riid, IID_IDMUSProdNode)
	||  ::IsEqualIID(riid, IID_IUnknown) )
    {
        AddRef();
        *ppvObj = (IDMUSProdNode *)this;
        return S_OK;
    }

    if( ::IsEqualIID(riid, IID_IDMUSProdReferenceNode) )
    {
        AddRef();
        *ppvObj = (IDMUSProdReferenceNode *)this;
        return S_OK;
    }

    *ppvObj = NULL;
    return E_NOINTERFACE;
}

ULONG CContainerRef::AddRef()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	AfxOleLockApp(); 
    return ++m_dwRef;
}

ULONG CContainerRef::Release()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    ASSERT( m_dwRef != 0 );

	AfxOleUnlockApp(); 
    --m_dwRef;

    if( m_dwRef == 0 )
    {
        delete this;
        return 0;
    }

    return m_dwRef;
}


/////////////////////////////////////////////////////////////////////////////
// CContainerRef IDMUSProdNode implementation

/////////////////////////////////////////////////////////////////////////////
// CContainerRef IDMUSProdNode::GetNodeImageIndex

HRESULT CContainerRef::GetNodeImageIndex( short* pnFirstImage )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( theApp.m_pContainerComponent != NULL );

	return( theApp.m_pContainerComponent->GetContainerRefImageIndex(pnFirstImage) );
}


/////////////////////////////////////////////////////////////////////////////
// CContainerRef IDMUSProdNode::GetFirstChild

HRESULT CContainerRef::GetFirstChild( IDMUSProdNode** ppIFirstChildNode )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(ppIFirstChildNode);

    return E_NOTIMPL;	// ContainerRef node does not have children
}


/////////////////////////////////////////////////////////////////////////////
// CContainerRef IDMUSProdNode::GetNextChild

HRESULT CContainerRef::GetNextChild( IDMUSProdNode* pIChildNode, IDMUSProdNode** ppINextChildNode )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(pIChildNode);
	UNREFERENCED_PARAMETER(ppINextChildNode);

    return E_NOTIMPL;	// ContainerRef node does not have children
}


/////////////////////////////////////////////////////////////////////////////
// CContainerRef IDMUSProdNode::GetComponent

HRESULT CContainerRef::GetComponent( IDMUSProdComponent** ppIComponent )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( theApp.m_pContainerComponent != NULL );

	return theApp.m_pContainerComponent->QueryInterface( IID_IDMUSProdComponent, (void**)ppIComponent );
}


/////////////////////////////////////////////////////////////////////////////
// CContainerRef IDMUSProdNode::GetDocRootNode

HRESULT CContainerRef::GetDocRootNode( IDMUSProdNode** ppIDocRootNode )
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
// CContainerRef IDMUSProdNode::SetDocRootNode

HRESULT CContainerRef::SetDocRootNode( IDMUSProdNode* pIDocRootNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( pIDocRootNode != NULL );

	m_pIDocRootNode = pIDocRootNode;
//	m_pIDocRootNode->AddRef();		intentionally missing

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CContainerRef IDMUSProdNode::GetParentNode

HRESULT CContainerRef::GetParentNode( IDMUSProdNode** ppIParentNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pIParentNode != NULL );

	m_pIParentNode->AddRef();
	*ppIParentNode = m_pIParentNode;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CContainerRef IDMUSProdNode::SetParentNode

HRESULT CContainerRef::SetParentNode( IDMUSProdNode* pIParentNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( pIParentNode != NULL );

	m_pIParentNode = pIParentNode;
//	m_pIParentNode->AddRef();		intentionally missing

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CContainerRef IDMUSProdNode::GetNodeId

HRESULT CContainerRef::GetNodeId( GUID* pguid )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pguid == NULL )
	{
		return E_POINTER;
	}

	*pguid = GUID_ContainerRefNode;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CContainerRef IDMUSProdNode::GetNodeName

HRESULT CContainerRef::GetNodeName( BSTR* pbstrName )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pContainer == NULL )
	{
		TCHAR achNoContainer[MID_BUFFER];

		::LoadString( theApp.m_hInstance, IDS_NO_CONTAINER, achNoContainer, MID_BUFFER );
		CString strNoContainer = achNoContainer;
		*pbstrName = strNoContainer.AllocSysString();
		return S_OK;
	}

    return m_pContainer->GetNodeName( pbstrName );
}


/////////////////////////////////////////////////////////////////////////////
// CContainerRef IDMUSProdNode::GetNodeNameMaxLength

HRESULT CContainerRef::GetNodeNameMaxLength( short* pnMaxLength )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	*pnMaxLength = -1;	// Can't rename a ContainerRef node

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CContainerRef IDMUSProdNode::ValidateNodeName

HRESULT CContainerRef::ValidateNodeName( BSTR bstrName )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(bstrName);

	return E_NOTIMPL;	// Can't rename a ContainerRef node
}


/////////////////////////////////////////////////////////////////////////////
// CContainerRef IDMUSProdNode::SetNodeName

HRESULT CContainerRef::SetNodeName( BSTR bstrName )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(bstrName);

	return E_NOTIMPL;	// Can't rename a ContainerRef node
}


/////////////////////////////////////////////////////////////////////////////
// CContainerRef IDMUSProdNode::GetNodeListInfo

HRESULT CContainerRef::GetNodeListInfo( DMUSProdListInfo* pListInfo )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pContainer == NULL )
	{
		return E_FAIL;
	}

    return m_pContainer->GetNodeListInfo( pListInfo );
}


/////////////////////////////////////////////////////////////////////////////
// CContainerRef IDMUSProdNode::GetEditorClsId

HRESULT CContainerRef::GetEditorClsId( CLSID* pClsId )
{
// AMC delete??

//	AFX_MANAGE_STATE(_afxModuleAddrThis);
//
//	if( m_pContainer == NULL )
//	{
//		return E_FAIL;
//	}
//
//  *pClsId = CLSID_ContainerEditor;
//	
//	return S_OK;
	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CContainerRef IDMUSProdNode::GetEditorTitle

HRESULT CContainerRef::GetEditorTitle( BSTR* pbstrTitle )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pContainer == NULL )
	{
		return E_FAIL;
	}

    return m_pContainer->GetEditorTitle( pbstrTitle );
}


/////////////////////////////////////////////////////////////////////////////
// CContainerRef IDMUSProdNode::GetEditorWindow

HRESULT CContainerRef::GetEditorWindow( HWND* hWndEditor )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pContainer == NULL )
	{
		return E_FAIL;
	}

    return m_pContainer->GetEditorWindow( hWndEditor );
}


/////////////////////////////////////////////////////////////////////////////
// CContainerRef IDMUSProdNode::SetEditorWindow

HRESULT CContainerRef::SetEditorWindow( HWND hWndEditor )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pContainer == NULL )
	{
		return E_FAIL;
	}

    return m_pContainer->SetEditorWindow( hWndEditor );
}


/////////////////////////////////////////////////////////////////////////////
// CContainerRef IDMUSProdNode::UseOpenCloseImages

HRESULT CContainerRef::UseOpenCloseImages( BOOL* pfUseOpenCloseImages )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	*pfUseOpenCloseImages = FALSE;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CContainerRef IDMUSProdNode::GetRightClickMenuId

HRESULT CContainerRef::GetRightClickMenuId( HINSTANCE* phInstance, UINT* pnMenuId )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	*phInstance = theApp.m_hInstance;
	*pnMenuId   = IDM_CONTAINERREF_NODE_RMENU;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CContainerRef IDMUSProdNode::OnRightClickMenuInit

HRESULT CContainerRef::OnRightClickMenuInit( HMENU hMenu )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(hMenu);

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CContainerRef IDMUSProdNode::OnRightClickMenuSelect

HRESULT CContainerRef::OnRightClickMenuSelect( long lCommandId )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	switch( lCommandId )
	{
		case IDM_PROPERTIES:
			if( m_pContainer )
			{
				return m_pContainer->OnRightClickMenuSelect( lCommandId );
			}
			break;

		default:
			return E_FAIL;
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CContainerRef IDMUSProdNode::DeleteChildNode

HRESULT CContainerRef::DeleteChildNode( IDMUSProdNode* pIChildNode, BOOL fPromptUser )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(pIChildNode);
	UNREFERENCED_PARAMETER(fPromptUser);

	return E_NOTIMPL;	// ContainerRef nodes do not have children
}


/////////////////////////////////////////////////////////////////////////////
// CContainerRef IDMUSProdNode::InsertChildNode

HRESULT CContainerRef::InsertChildNode( IDMUSProdNode* pIChildNode )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(pIChildNode);

	return E_NOTIMPL;	// ContainerRef nodes do not have children
}


/////////////////////////////////////////////////////////////////////////////
// CContainerRef IDMUSProdNode::DeleteNode

HRESULT CContainerRef::DeleteNode( BOOL fPromptUser )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(fPromptUser);

	return E_FAIL;		// Cannot delete ContainerRef node?????
}


/////////////////////////////////////////////////////////////////////////////
// CContainerRef IDMUSProdNode::OnNodeSelChanged

HRESULT CContainerRef::OnNodeSelChanged( BOOL fSelected )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(fSelected);

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CContainerRef IDMUSProdNode::CreateDataObject

HRESULT CContainerRef::CreateDataObject( IDataObject** ppIDataObject )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( ppIDataObject == NULL )
	{
		return E_POINTER;
	}

	*ppIDataObject = NULL;

	if( m_pContainer )
	{
		// Let Container create data object
		return m_pContainer->CreateDataObject( ppIDataObject );
	}

	return E_FAIL;
}


/////////////////////////////////////////////////////////////////////////////
// CContainerRef IDMUSProdNode::CanCut

HRESULT CContainerRef::CanCut( void )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
 
	return CanDelete();
}


/////////////////////////////////////////////////////////////////////////////
// CContainerRef IDMUSProdNode::CanCopy

HRESULT CContainerRef::CanCopy( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
 
	if( m_pContainer )
	{
		return S_OK;	// Will copy the Container
	}

	return S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CContainerRef IDMUSProdNode::CanDelete

HRESULT CContainerRef::CanDelete( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pIParentNode != NULL );

	return m_pIParentNode->CanDeleteChildNode( this );
}


/////////////////////////////////////////////////////////////////////////////
// CContainerRef IDMUSProdNode::CanDeleteChildNode

HRESULT CContainerRef::CanDeleteChildNode( IDMUSProdNode* pIChildNode )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(pIChildNode);

	ASSERT( 0 );

	return E_NOTIMPL;	// Container Reference nodes do not have children
}


/////////////////////////////////////////////////////////////////////////////
// CContainerRef IDMUSProdNode::CanPasteFromData

HRESULT CContainerRef::CanPasteFromData( IDataObject* pIDataObject, BOOL* pfWillSetReference )
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

	// Create a new CDllJazzDataObject and see if it contains a Container
	CDllJazzDataObject* pDataObject = new CDllJazzDataObject();
	if( pDataObject == NULL )
	{
		return E_OUTOFMEMORY;
	}
	
	HRESULT hr = pDataObject->IsClipFormatAvailable( pIDataObject, theApp.m_pContainerComponent->m_cfContainer );

	RELEASE( pDataObject );

	if( SUCCEEDED ( hr ) )
	{
		ASSERT( m_pIParentNode != NULL );

		// Let parent decide whether it wants to paste a Container
		return m_pIParentNode->CanChildPasteFromData( pIDataObject, this, pfWillSetReference );
	}

	if( m_pContainer )
	{
		// Let Container decide what can be dropped
		return m_pContainer->CanPasteFromData( pIDataObject, pfWillSetReference );
	}

	return S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CContainerRef IDMUSProdNode::PasteFromData

HRESULT CContainerRef::PasteFromData( IDataObject* pIDataObject )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pIDataObject == NULL )
	{
		return E_INVALIDARG;
	}

	// Create a new CDllJazzDataObject and see if it contains a Container
	CDllJazzDataObject* pDataObject = new CDllJazzDataObject();
	if( pDataObject == NULL )
	{
		return E_OUTOFMEMORY;
	}
	
	HRESULT hr = pDataObject->IsClipFormatAvailable( pIDataObject, theApp.m_pContainerComponent->m_cfContainer );

	RELEASE( pDataObject );

	if( SUCCEEDED ( hr ) )
	{
		ASSERT( m_pIParentNode != NULL );

		// Let parent handle pasting of Container
		return m_pIParentNode->ChildPasteFromData( pIDataObject, this );
	}

	if( m_pContainer )
	{
		// Let Container handle paste
		return m_pContainer->PasteFromData( pIDataObject );
	}

	return E_FAIL;
}


/////////////////////////////////////////////////////////////////////////////
// CContainerRef IDMUSProdNode::CanChildPasteFromData

HRESULT CContainerRef::CanChildPasteFromData( IDataObject* pIDataObject, IDMUSProdNode* pIChildNode,
										  BOOL* pfWillSetReference )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(pIDataObject);
	UNREFERENCED_PARAMETER(pIChildNode);
	UNREFERENCED_PARAMETER(pfWillSetReference);

	return E_NOTIMPL;			// Container Reference nodes do not have children
}


/////////////////////////////////////////////////////////////////////////////
// CContainerRef IDMUSProdNode::ChildPasteFromData

HRESULT CContainerRef::ChildPasteFromData( IDataObject* pIDataObject, IDMUSProdNode* pIChildNode )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(pIDataObject);
	UNREFERENCED_PARAMETER(pIChildNode);

	return E_NOTIMPL;			// Container Reference nodes do not have children
}


/////////////////////////////////////////////////////////////////////////////
// CContainerRef IDMUSProdNode::GetObject

HRESULT CContainerRef::GetObject( REFCLSID rclsid, REFIID riid, void** ppvObject )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(rclsid);
	UNREFERENCED_PARAMETER(riid);
	UNREFERENCED_PARAMETER(ppvObject);

	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CContainerRef IDMUSProdReferenceNode implementation

/////////////////////////////////////////////////////////////////////////////
// CContainerRef IDMUSProdReferenceNode::GetReferencedFile

HRESULT CContainerRef::GetReferencedFile( IDMUSProdNode** ppIDocRootNode )
{
 	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( ppIDocRootNode == NULL )
	{
		return E_POINTER;
	}

	*ppIDocRootNode = NULL;

	if( m_pContainer )
	{
		m_pContainer->AddRef();
		*ppIDocRootNode = m_pContainer;
		return S_OK;
	}

    return E_FAIL;
}


/////////////////////////////////////////////////////////////////////////////
// CContainerRef::IDMUSProdReferenceNode::SetReferencedFile

HRESULT CContainerRef::SetReferencedFile( IDMUSProdNode* pIDocRootNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pIDocRootNode )
	{
		// Make sure method was passed a DocRootNode
		IDMUSProdNode* pINode;
		if( FAILED ( pIDocRootNode->GetDocRootNode ( &pINode ) ) )
		{
			pINode = NULL;
		}
		if( pIDocRootNode != pINode )
		{
			RELEASE( pINode );
			return E_INVALIDARG;
		}
		RELEASE( pINode );

		// Make sure method was passed a Container Node
		GUID guidNodeId;
		if( SUCCEEDED ( pIDocRootNode->GetNodeId ( &guidNodeId ) ) )
		{
			if( !( IsEqualGUID ( guidNodeId, GUID_ContainerNode ) ) )
			{
				return E_INVALIDARG;
			}
		}
	}

	// Release previous DocRoot Node
	RELEASE( m_pContainer );

	// Link to new DocRoot Node
	if( pIDocRootNode )
	{
		m_pContainer = (CDirectMusicContainer *)pIDocRootNode;
		m_pContainer->AddRef();
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CContainerRef Additional functions
