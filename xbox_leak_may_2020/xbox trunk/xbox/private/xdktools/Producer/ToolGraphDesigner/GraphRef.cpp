// GraphRef.cpp : implementation file
//

#include "stdafx.h"
#include "ToolGraphDesignerDLL.h"
#include "Graph.h"
#include "GraphRef.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CGraphRef constructor/destructor

CGraphRef::CGraphRef()
{
    m_dwRef = 0;
	AddRef();

	m_pIDocRootNode = NULL;
	m_pIParentNode = NULL;
	m_pGraph = NULL;
}

CGraphRef::~CGraphRef()
{
	RELEASE( m_pGraph );
}


/////////////////////////////////////////////////////////////////////////////
// CGraphRef IUnknown implementation

HRESULT CGraphRef::QueryInterface( REFIID riid, LPVOID FAR* ppvObj )
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

ULONG CGraphRef::AddRef()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	AfxOleLockApp(); 
    return ++m_dwRef;
}

ULONG CGraphRef::Release()
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
// CGraphRef IDMUSProdNode implementation

/////////////////////////////////////////////////////////////////////////////
// CGraphRef IDMUSProdNode::GetNodeImageIndex

HRESULT CGraphRef::GetNodeImageIndex( short* pnFirstImage )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( theApp.m_pGraphComponent != NULL );

	return( theApp.m_pGraphComponent->GetGraphRefImageIndex(pnFirstImage) );
}


/////////////////////////////////////////////////////////////////////////////
// CGraphRef IDMUSProdNode::GetFirstChild

HRESULT CGraphRef::GetFirstChild( IDMUSProdNode** ppIFirstChildNode )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(ppIFirstChildNode);

    return E_NOTIMPL;	// GraphRef node does not have children
}


/////////////////////////////////////////////////////////////////////////////
// CGraphRef IDMUSProdNode::GetNextChild

HRESULT CGraphRef::GetNextChild( IDMUSProdNode* pIChildNode, IDMUSProdNode** ppINextChildNode )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(pIChildNode);
	UNREFERENCED_PARAMETER(ppINextChildNode);

    return E_NOTIMPL;	// GraphRef node does not have children
}


/////////////////////////////////////////////////////////////////////////////
// CGraphRef IDMUSProdNode::GetComponent

HRESULT CGraphRef::GetComponent( IDMUSProdComponent** ppIComponent )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( theApp.m_pGraphComponent != NULL );

	return theApp.m_pGraphComponent->QueryInterface( IID_IDMUSProdComponent, (void**)ppIComponent );
}


/////////////////////////////////////////////////////////////////////////////
// CGraphRef IDMUSProdNode::GetDocRootNode

HRESULT CGraphRef::GetDocRootNode( IDMUSProdNode** ppIDocRootNode )
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
// CGraphRef IDMUSProdNode::SetDocRootNode

HRESULT CGraphRef::SetDocRootNode( IDMUSProdNode* pIDocRootNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( pIDocRootNode != NULL );

	m_pIDocRootNode = pIDocRootNode;
//	m_pIDocRootNode->AddRef();		intentionally missing

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CGraphRef IDMUSProdNode::GetParentNode

HRESULT CGraphRef::GetParentNode( IDMUSProdNode** ppIParentNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pIParentNode != NULL );

	m_pIParentNode->AddRef();
	*ppIParentNode = m_pIParentNode;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CGraphRef IDMUSProdNode::SetParentNode

HRESULT CGraphRef::SetParentNode( IDMUSProdNode* pIParentNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( pIParentNode != NULL );

	m_pIParentNode = pIParentNode;
//	m_pIParentNode->AddRef();		intentionally missing

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CGraphRef IDMUSProdNode::GetNodeId

HRESULT CGraphRef::GetNodeId( GUID* pguid )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pguid == NULL )
	{
		return E_POINTER;
	}

	*pguid = GUID_ToolGraphRefNode;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CGraphRef IDMUSProdNode::GetNodeName

HRESULT CGraphRef::GetNodeName( BSTR* pbstrName )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pGraph == NULL )
	{
		TCHAR achNoGraph[MID_BUFFER];

		::LoadString( theApp.m_hInstance, IDS_NO_GRAPH, achNoGraph, MID_BUFFER );
		CString strNoGraph = achNoGraph;
		*pbstrName = strNoGraph.AllocSysString();
		return S_OK;
	}

    return m_pGraph->GetNodeName( pbstrName );
}


/////////////////////////////////////////////////////////////////////////////
// CGraphRef IDMUSProdNode::GetNodeNameMaxLength

HRESULT CGraphRef::GetNodeNameMaxLength( short* pnMaxLength )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	*pnMaxLength = -1;	// Can't rename a GraphRef node

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CGraphRef IDMUSProdNode::ValidateNodeName

HRESULT CGraphRef::ValidateNodeName( BSTR bstrName )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(bstrName);

	return E_NOTIMPL;	// Can't rename a GraphRef node
}


/////////////////////////////////////////////////////////////////////////////
// CGraphRef IDMUSProdNode::SetNodeName

HRESULT CGraphRef::SetNodeName( BSTR bstrName )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(bstrName);

	return E_NOTIMPL;	// Can't rename a GraphRef node
}


/////////////////////////////////////////////////////////////////////////////
// CGraphRef IDMUSProdNode::GetNodeListInfo

HRESULT CGraphRef::GetNodeListInfo( DMUSProdListInfo* pListInfo )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pGraph == NULL )
	{
		return E_FAIL;
	}

    return m_pGraph->GetNodeListInfo( pListInfo );
}


/////////////////////////////////////////////////////////////////////////////
// CGraphRef IDMUSProdNode::GetEditorClsId

HRESULT CGraphRef::GetEditorClsId( CLSID* pClsId )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pGraph == NULL )
	{
		return E_FAIL;
	}

    *pClsId = CLSID_GraphEditor;
	
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CGraphRef IDMUSProdNode::GetEditorTitle

HRESULT CGraphRef::GetEditorTitle( BSTR* pbstrTitle )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pGraph == NULL )
	{
		return E_FAIL;
	}

    return m_pGraph->GetEditorTitle( pbstrTitle );
}


/////////////////////////////////////////////////////////////////////////////
// CGraphRef IDMUSProdNode::GetEditorWindow

HRESULT CGraphRef::GetEditorWindow( HWND* hWndEditor )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pGraph == NULL )
	{
		return E_FAIL;
	}

    return m_pGraph->GetEditorWindow( hWndEditor );
}


/////////////////////////////////////////////////////////////////////////////
// CGraphRef IDMUSProdNode::SetEditorWindow

HRESULT CGraphRef::SetEditorWindow( HWND hWndEditor )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pGraph == NULL )
	{
		return E_FAIL;
	}

    return m_pGraph->SetEditorWindow( hWndEditor );
}


/////////////////////////////////////////////////////////////////////////////
// CGraphRef IDMUSProdNode::UseOpenCloseImages

HRESULT CGraphRef::UseOpenCloseImages( BOOL* pfUseOpenCloseImages )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	*pfUseOpenCloseImages = FALSE;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CGraphRef IDMUSProdNode::GetRightClickMenuId

HRESULT CGraphRef::GetRightClickMenuId( HINSTANCE* phInstance, UINT* pnMenuId )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	*phInstance = theApp.m_hInstance;
	*pnMenuId   = IDM_GRAPHREF_NODE_RMENU;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CGraphRef IDMUSProdNode::OnRightClickMenuInit

HRESULT CGraphRef::OnRightClickMenuInit( HMENU hMenu )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(hMenu);

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CGraphRef IDMUSProdNode::OnRightClickMenuSelect

HRESULT CGraphRef::OnRightClickMenuSelect( long lCommandId )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	switch( lCommandId )
	{
		case IDM_PROPERTIES:
			if( m_pGraph )
			{
				return m_pGraph->OnRightClickMenuSelect( lCommandId );
			}
			break;

		default:
			return E_FAIL;
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CGraphRef IDMUSProdNode::DeleteChildNode

HRESULT CGraphRef::DeleteChildNode( IDMUSProdNode* pIChildNode, BOOL fPromptUser )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(pIChildNode);
	UNREFERENCED_PARAMETER(fPromptUser);

	return E_NOTIMPL;	// GraphRef nodes do not have children
}


/////////////////////////////////////////////////////////////////////////////
// CGraphRef IDMUSProdNode::InsertChildNode

HRESULT CGraphRef::InsertChildNode( IDMUSProdNode* pIChildNode )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(pIChildNode);

	return E_NOTIMPL;	// GraphRef nodes do not have children
}


/////////////////////////////////////////////////////////////////////////////
// CGraphRef IDMUSProdNode::DeleteNode

HRESULT CGraphRef::DeleteNode( BOOL fPromptUser )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(fPromptUser);

	return E_FAIL;		// Cannot delete GraphRef node?????
}


/////////////////////////////////////////////////////////////////////////////
// CGraphRef IDMUSProdNode::OnNodeSelChanged

HRESULT CGraphRef::OnNodeSelChanged( BOOL fSelected )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(fSelected);

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CGraphRef IDMUSProdNode::CreateDataObject

HRESULT CGraphRef::CreateDataObject( IDataObject** ppIDataObject )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( ppIDataObject == NULL )
	{
		return E_POINTER;
	}

	*ppIDataObject = NULL;

	if( m_pGraph )
	{
		// Let Graph create data object
		return m_pGraph->CreateDataObject( ppIDataObject );
	}

	return E_FAIL;
}


/////////////////////////////////////////////////////////////////////////////
// CGraphRef IDMUSProdNode::CanCut

HRESULT CGraphRef::CanCut( void )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
 
	return CanDelete();
}


/////////////////////////////////////////////////////////////////////////////
// CGraphRef IDMUSProdNode::CanCopy

HRESULT CGraphRef::CanCopy( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
 
	if( m_pGraph )
	{
		return S_OK;	// Will copy the Graph
	}

	return S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CGraphRef IDMUSProdNode::CanDelete

HRESULT CGraphRef::CanDelete( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pIParentNode != NULL );

	return m_pIParentNode->CanDeleteChildNode( this );
}


/////////////////////////////////////////////////////////////////////////////
// CGraphRef IDMUSProdNode::CanDeleteChildNode

HRESULT CGraphRef::CanDeleteChildNode( IDMUSProdNode* pIChildNode )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(pIChildNode);

	ASSERT( 0 );

	return E_NOTIMPL;	// Graph Reference nodes do not have children
}


/////////////////////////////////////////////////////////////////////////////
// CGraphRef IDMUSProdNode::CanPasteFromData

HRESULT CGraphRef::CanPasteFromData( IDataObject* pIDataObject, BOOL* pfWillSetReference )
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

	// Create a new CDllJazzDataObject and see if it contains a Graph
	CDllJazzDataObject* pDataObject = new CDllJazzDataObject();
	if( pDataObject == NULL )
	{
		return E_OUTOFMEMORY;
	}
	
	HRESULT hr = pDataObject->IsClipFormatAvailable( pIDataObject, theApp.m_pGraphComponent->m_cfGraph );

	RELEASE( pDataObject );

	if( SUCCEEDED ( hr ) )
	{
		ASSERT( m_pIParentNode != NULL );

		// Let parent decide whether it wants to paste a Graph
		return m_pIParentNode->CanChildPasteFromData( pIDataObject, this, pfWillSetReference );
	}

	if( m_pGraph )
	{
		// Let Graph decide what can be dropped
		return m_pGraph->CanPasteFromData( pIDataObject, pfWillSetReference );
	}

	return S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CGraphRef IDMUSProdNode::PasteFromData

HRESULT CGraphRef::PasteFromData( IDataObject* pIDataObject )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pIDataObject == NULL )
	{
		return E_INVALIDARG;
	}

	// Create a new CDllJazzDataObject and see if it contains a Graph
	CDllJazzDataObject* pDataObject = new CDllJazzDataObject();
	if( pDataObject == NULL )
	{
		return E_OUTOFMEMORY;
	}
	
	HRESULT hr = pDataObject->IsClipFormatAvailable( pIDataObject, theApp.m_pGraphComponent->m_cfGraph );

	RELEASE( pDataObject );

	if( SUCCEEDED ( hr ) )
	{
		ASSERT( m_pIParentNode != NULL );

		// Let parent handle pasting of Graph
		return m_pIParentNode->ChildPasteFromData( pIDataObject, this );
	}

	if( m_pGraph )
	{
		// Let Graph handle paste
		return m_pGraph->PasteFromData( pIDataObject );
	}

	return E_FAIL;
}


/////////////////////////////////////////////////////////////////////////////
// CGraphRef IDMUSProdNode::CanChildPasteFromData

HRESULT CGraphRef::CanChildPasteFromData( IDataObject* pIDataObject, IDMUSProdNode* pIChildNode,
										  BOOL* pfWillSetReference )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(pIDataObject);
	UNREFERENCED_PARAMETER(pIChildNode);
	UNREFERENCED_PARAMETER(pfWillSetReference);

	return E_NOTIMPL;			// Graph Reference nodes do not have children
}


/////////////////////////////////////////////////////////////////////////////
// CGraphRef IDMUSProdNode::ChildPasteFromData

HRESULT CGraphRef::ChildPasteFromData( IDataObject* pIDataObject, IDMUSProdNode* pIChildNode )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(pIDataObject);
	UNREFERENCED_PARAMETER(pIChildNode);

	return E_NOTIMPL;			// Graph Reference nodes do not have children
}


/////////////////////////////////////////////////////////////////////////////
// CGraphRef IDMUSProdNode::GetObject

HRESULT CGraphRef::GetObject( REFCLSID rclsid, REFIID riid, void** ppvObject )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(rclsid);
	UNREFERENCED_PARAMETER(riid);
	UNREFERENCED_PARAMETER(ppvObject);

	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CGraphRef IDMUSProdReferenceNode implementation

/////////////////////////////////////////////////////////////////////////////
// CGraphRef IDMUSProdReferenceNode::GetReferencedFile

HRESULT CGraphRef::GetReferencedFile( IDMUSProdNode** ppIDocRootNode )
{
 	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( ppIDocRootNode == NULL )
	{
		return E_POINTER;
	}

	*ppIDocRootNode = NULL;

	if( m_pGraph )
	{
		m_pGraph->AddRef();
		*ppIDocRootNode = m_pGraph;
		return S_OK;
	}

    return E_FAIL;
}


/////////////////////////////////////////////////////////////////////////////
// CGraphRef::IDMUSProdReferenceNode::SetReferencedFile

HRESULT CGraphRef::SetReferencedFile( IDMUSProdNode* pIDocRootNode )
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

		// Make sure method was passed a Graph Node
		GUID guidNodeId;
		if( SUCCEEDED ( pIDocRootNode->GetNodeId ( &guidNodeId ) ) )
		{
			if( !( IsEqualGUID ( guidNodeId, GUID_ToolGraphNode ) ) )
			{
				return E_INVALIDARG;
			}
		}
	}

	// Release previous DocRoot Node
	RELEASE( m_pGraph );

	// Link to new DocRoot Node
	if( pIDocRootNode )
	{
		m_pGraph = (CDirectMusicGraph *)pIDocRootNode;
		m_pGraph->AddRef();
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CGraphRef Additional functions
