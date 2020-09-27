// SegmentRef.cpp : implementation file
//

#include "stdafx.h"
#include "SegmentDesignerDLL.h"

#include "Segment.h"
#include "SegmentRef.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CSegmentRef constructor/destructor

CSegmentRef::CSegmentRef( CSegmentComponent* pComponent )
{
	ASSERT( pComponent != NULL );
	m_pComponent = pComponent;
	m_pComponent->AddRef();

    m_dwRef = 0;
	AddRef();

	m_pIDocRootNode = NULL;
	m_pIParentNode = NULL;
	m_pSegment = NULL;
}

CSegmentRef::~CSegmentRef()
{
	RELEASE( m_pSegment );
	RELEASE( m_pComponent );
}


/////////////////////////////////////////////////////////////////////////////
// CSegmentRef IUnknown implementation

HRESULT CSegmentRef::QueryInterface( REFIID riid, LPVOID FAR* ppvObj )
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

ULONG CSegmentRef::AddRef()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	AfxOleLockApp(); 
    return ++m_dwRef;
}

ULONG CSegmentRef::Release()
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
// CSegmentRef IDMUSProdNode implementation

/////////////////////////////////////////////////////////////////////////////
// CSegmentRef IDMUSProdNode::GetNodeImageIndex

HRESULT CSegmentRef::GetNodeImageIndex( short* pnFirstImage )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	ASSERT( m_pComponent != NULL );

	return( m_pComponent->GetSegmentRefImageIndex(pnFirstImage) );
}


/////////////////////////////////////////////////////////////////////////////
// CSegmentRef IDMUSProdNode::GetFirstChild

HRESULT CSegmentRef::GetFirstChild( IDMUSProdNode** ppIFirstChildNode )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(ppIFirstChildNode);

    return E_NOTIMPL;	// SegmentRef node does not have children
}


/////////////////////////////////////////////////////////////////////////////
// CSegmentRef IDMUSProdNode::GetNextChild

HRESULT CSegmentRef::GetNextChild( IDMUSProdNode* pIChildNode, IDMUSProdNode** ppINextChildNode )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(pIChildNode);
	UNREFERENCED_PARAMETER(ppINextChildNode);

    return E_NOTIMPL;	// SegmentRef node does not have children
}


/////////////////////////////////////////////////////////////////////////////
// CSegmentRef IDMUSProdNode::GetComponent

HRESULT CSegmentRef::GetComponent( IDMUSProdComponent** ppIComponent )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pComponent != NULL );

	return m_pComponent->QueryInterface( IID_IDMUSProdComponent, (void**)ppIComponent );
}


/////////////////////////////////////////////////////////////////////////////
// CSegmentRef IDMUSProdNode::GetDocRootNode

HRESULT CSegmentRef::GetDocRootNode( IDMUSProdNode** ppIDocRootNode )
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
// CSegmentRef IDMUSProdNode::SetDocRootNode

HRESULT CSegmentRef::SetDocRootNode( IDMUSProdNode* pIDocRootNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( pIDocRootNode != NULL );

	m_pIDocRootNode = pIDocRootNode;
//	m_pIDocRootNode->AddRef();		intentionally missing

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSegmentRef IDMUSProdNode::GetParentNode

HRESULT CSegmentRef::GetParentNode( IDMUSProdNode** ppIParentNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pIParentNode != NULL );

	m_pIParentNode->AddRef();
	*ppIParentNode = m_pIParentNode;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSegmentRef IDMUSProdNode::SetParentNode

HRESULT CSegmentRef::SetParentNode( IDMUSProdNode* pIParentNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( pIParentNode != NULL );

	m_pIParentNode = pIParentNode;
//	m_pIParentNode->AddRef();		intentionally missing

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSegmentRef IDMUSProdNode::GetNodeId

HRESULT CSegmentRef::GetNodeId( GUID* pguid )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pguid == NULL )
	{
		return E_POINTER;
	}

	*pguid = GUID_SegmentRefNode;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSegmentRef IDMUSProdNode::GetNodeName

HRESULT CSegmentRef::GetNodeName( BSTR* pbstrName )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pSegment == NULL )
	{
		TCHAR achNoSegment[MID_BUFFER];

		::LoadString( theApp.m_hInstance, IDS_NO_SEGMENT, achNoSegment, MID_BUFFER );
		CString strNoSegment = achNoSegment;
		*pbstrName = strNoSegment.AllocSysString();
		return S_OK;
	}

    return m_pSegment->GetNodeName( pbstrName );
}


/////////////////////////////////////////////////////////////////////////////
// CSegmentRef IDMUSProdNode::GetNodeNameMaxLength

HRESULT CSegmentRef::GetNodeNameMaxLength( short* pnMaxLength )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	*pnMaxLength = -1;	// Can't rename a SegmentRef node

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSegmentRef IDMUSProdNode::ValidateNodeName

HRESULT CSegmentRef::ValidateNodeName( BSTR bstrName )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(bstrName);

	return E_NOTIMPL;	// Can't rename a SegmentRef node
}


/////////////////////////////////////////////////////////////////////////////
// CSegmentRef IDMUSProdNode::SetNodeName

HRESULT CSegmentRef::SetNodeName( BSTR bstrName )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(bstrName);

	return E_NOTIMPL;	// Can't rename a SegmentRef node
}


/////////////////////////////////////////////////////////////////////////////
// CSegmentRef IDMUSProdNode::GetNodeListInfo

HRESULT CSegmentRef::GetNodeListInfo( DMUSProdListInfo* pListInfo )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pSegment == NULL )
	{
		return E_FAIL;
	}

    return m_pSegment->GetNodeListInfo( pListInfo );
}


/////////////////////////////////////////////////////////////////////////////
// CSegmentRef IDMUSProdNode::GetEditorClsId

HRESULT CSegmentRef::GetEditorClsId( CLSID* pClsId )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pSegment == NULL )
	{
		return E_FAIL;
	}

    *pClsId = CLSID_SegmentDesigner;
	
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSegmentRef IDMUSProdNode::GetEditorTitle

HRESULT CSegmentRef::GetEditorTitle( BSTR* pbstrTitle )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pSegment == NULL )
	{
		return E_FAIL;
	}

    return m_pSegment->GetEditorTitle( pbstrTitle );
}


/////////////////////////////////////////////////////////////////////////////
// CSegmentRef IDMUSProdNode::GetEditorWindow

HRESULT CSegmentRef::GetEditorWindow( HWND* hWndEditor )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pSegment == NULL )
	{
		return E_FAIL;
	}

    return m_pSegment->GetEditorWindow( hWndEditor );
}


/////////////////////////////////////////////////////////////////////////////
// CSegmentRef IDMUSProdNode::SetEditorWindow

HRESULT CSegmentRef::SetEditorWindow( HWND hWndEditor )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pSegment == NULL )
	{
		return E_FAIL;
	}

    return m_pSegment->SetEditorWindow( hWndEditor );
}


/////////////////////////////////////////////////////////////////////////////
// CSegmentRef IDMUSProdNode::UseOpenCloseImages

HRESULT CSegmentRef::UseOpenCloseImages( BOOL* pfUseOpenCloseImages )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	*pfUseOpenCloseImages = FALSE;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSegmentRef IDMUSProdNode::GetRightClickMenuId

HRESULT CSegmentRef::GetRightClickMenuId( HINSTANCE* phInstance, UINT* pnMenuId )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	*phInstance = theApp.m_hInstance;
	*pnMenuId   = IDM_SEGMENTREF_NODE_RMENU;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSegmentRef IDMUSProdNode::OnRightClickMenuInit

HRESULT CSegmentRef::OnRightClickMenuInit( HMENU hMenu )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(hMenu);

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSegmentRef IDMUSProdNode::OnRightClickMenuSelect

HRESULT CSegmentRef::OnRightClickMenuSelect( long lCommandId )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	switch( lCommandId )
	{
		case IDM_PROPERTIES:
			if( m_pSegment )
			{
				return m_pSegment->OnRightClickMenuSelect( lCommandId );
			}
			break;

		default:
			return E_FAIL;
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSegmentRef IDMUSProdNode::DeleteChildNode

HRESULT CSegmentRef::DeleteChildNode( IDMUSProdNode* pIChildNode, BOOL fPromptUser )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(pIChildNode);
	UNREFERENCED_PARAMETER(fPromptUser);

	return E_NOTIMPL;	// SegmentRef nodes do not have children
}


/////////////////////////////////////////////////////////////////////////////
// CSegmentRef IDMUSProdNode::InsertChildNode

HRESULT CSegmentRef::InsertChildNode( IDMUSProdNode* pIChildNode )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(pIChildNode);

	return E_NOTIMPL;	// SegmentRef nodes do not have children
}


/////////////////////////////////////////////////////////////////////////////
// CSegmentRef IDMUSProdNode::DeleteNode

HRESULT CSegmentRef::DeleteNode( BOOL fPromptUser )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(fPromptUser);

	return E_FAIL;		// Cannot delete SegmentRef node?????
}


/////////////////////////////////////////////////////////////////////////////
// CSegmentRef IDMUSProdNode::OnNodeSelChanged

HRESULT CSegmentRef::OnNodeSelChanged( BOOL fSelected )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(fSelected);

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSegmentRef IDMUSProdNode::CreateDataObject

HRESULT CSegmentRef::CreateDataObject( IDataObject** ppIDataObject )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( ppIDataObject == NULL )
	{
		return E_POINTER;
	}

	*ppIDataObject = NULL;

	if( m_pSegment )
	{
		// Let Segment create data object
		return m_pSegment->CreateDataObject( ppIDataObject );
	}

	return E_FAIL;
}


/////////////////////////////////////////////////////////////////////////////
// CSegmentRef IDMUSProdNode::CanCut

HRESULT CSegmentRef::CanCut( void )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
 
	return CanDelete();
}


/////////////////////////////////////////////////////////////////////////////
// CSegmentRef IDMUSProdNode::CanCopy

HRESULT CSegmentRef::CanCopy( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
 
	if( m_pSegment )
	{
		return S_OK;	// Will copy the Segment
	}

	return S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CSegmentRef IDMUSProdNode::CanDelete

HRESULT CSegmentRef::CanDelete( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pIParentNode != NULL );

	return m_pIParentNode->CanDeleteChildNode( this );
}


/////////////////////////////////////////////////////////////////////////////
// CSegmentRef IDMUSProdNode::CanDeleteChildNode

HRESULT CSegmentRef::CanDeleteChildNode( IDMUSProdNode* pIChildNode )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(pIChildNode);

	ASSERT( 0 );

	return E_NOTIMPL;	// Segment Reference nodes do not have children
}


/////////////////////////////////////////////////////////////////////////////
// CSegmentRef IDMUSProdNode::CanPasteFromData

HRESULT CSegmentRef::CanPasteFromData( IDataObject* pIDataObject, BOOL* pfWillSetReference )
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

	// Create a new CDllJazzDataObject and see if it contains a Segment
	CDllJazzDataObject* pDataObject = new CDllJazzDataObject();
	if( pDataObject == NULL )
	{
		return E_OUTOFMEMORY;
	}
	
	HRESULT hr = pDataObject->IsClipFormatAvailable( pIDataObject, m_pComponent->m_cfSegment );

	RELEASE( pDataObject );

	if( SUCCEEDED ( hr ) )
	{
		ASSERT( m_pIParentNode != NULL );

		// Let parent decide whether it wants to paste a Segment
		return m_pIParentNode->CanChildPasteFromData( pIDataObject, this, pfWillSetReference );
	}

	if( m_pSegment )
	{
		// Let Segment decide what can be dropped
		return m_pSegment->CanPasteFromData( pIDataObject, pfWillSetReference );
	}

	return S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CSegmentRef IDMUSProdNode::PasteFromData

HRESULT CSegmentRef::PasteFromData( IDataObject* pIDataObject )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pIDataObject == NULL )
	{
		return E_INVALIDARG;
	}

	// Create a new CDllJazzDataObject and see if it contains a Segment
	CDllJazzDataObject* pDataObject = new CDllJazzDataObject();
	if( pDataObject == NULL )
	{
		return E_OUTOFMEMORY;
	}
	
	HRESULT hr = pDataObject->IsClipFormatAvailable( pIDataObject, m_pComponent->m_cfSegment );

	RELEASE( pDataObject );

	if( SUCCEEDED ( hr ) )
	{
		ASSERT( m_pIParentNode != NULL );

		// Let parent handle pasting of Segment
		return m_pIParentNode->ChildPasteFromData( pIDataObject, this );
	}

	if( m_pSegment )
	{
		// Let Segment handle paste
		return m_pSegment->PasteFromData( pIDataObject );
	}

	return E_FAIL;
}


/////////////////////////////////////////////////////////////////////////////
// CSegmentRef IDMUSProdNode::CanChildPasteFromData

HRESULT CSegmentRef::CanChildPasteFromData( IDataObject* pIDataObject, IDMUSProdNode* pIChildNode,
										  BOOL* pfWillSetReference )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(pIDataObject);
	UNREFERENCED_PARAMETER(pIChildNode);
	UNREFERENCED_PARAMETER(pfWillSetReference);

	return E_NOTIMPL;			// Segment Reference nodes do not have children
}


/////////////////////////////////////////////////////////////////////////////
// CSegmentRef IDMUSProdNode::ChildPasteFromData

HRESULT CSegmentRef::ChildPasteFromData( IDataObject* pIDataObject, IDMUSProdNode* pIChildNode )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(pIDataObject);
	UNREFERENCED_PARAMETER(pIChildNode);

	return E_NOTIMPL;			// Segment Reference nodes do not have children
}


/////////////////////////////////////////////////////////////////////////////
// CSegmentRef IDMUSProdNode::GetObject

HRESULT CSegmentRef::GetObject( REFCLSID rclsid, REFIID riid, void** ppvObject )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(rclsid);
	UNREFERENCED_PARAMETER(riid);
	UNREFERENCED_PARAMETER(ppvObject);

	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CSegmentRef IDMUSProdReferenceNode implementation

/////////////////////////////////////////////////////////////////////////////
// CSegmentRef IDMUSProdReferenceNode::GetReferencedFile

HRESULT CSegmentRef::GetReferencedFile( IDMUSProdNode** ppIDocRootNode )
{
 	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( ppIDocRootNode == NULL )
	{
		return E_POINTER;
	}

	*ppIDocRootNode = NULL;

	if( m_pSegment )
	{
		m_pSegment->AddRef();
		*ppIDocRootNode = m_pSegment;
		return S_OK;
	}

    return E_FAIL;
}


/////////////////////////////////////////////////////////////////////////////
// CSegmentRef::IDMUSProdReferenceNode::SetReferencedFile

HRESULT CSegmentRef::SetReferencedFile( IDMUSProdNode* pIDocRootNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

// AMC TEST THIS CODE

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

		// Make sure method was passed a Segment Node
		GUID guidNodeId;
		if( SUCCEEDED ( pIDocRootNode->GetNodeId ( &guidNodeId ) ) )
		{
			if( !( IsEqualGUID ( guidNodeId, GUID_SegmentNode ) ) )
			{
				return E_INVALIDARG;
			}
		}
	}

	// Release previous DocRoot Node
	RELEASE( m_pSegment );

	// Link to new DocRoot Node
	if( pIDocRootNode )
	{
		m_pSegment = (CSegment *)pIDocRootNode;
		m_pSegment->AddRef();
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSegmentRef Additional functions
