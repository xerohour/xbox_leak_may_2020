// BandRef.cpp : implementation file
//

#include "stdafx.h"
#include "BandEditorDLL.h"

#include "Band.h"
#include "BandRef.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CBandRef constructor/destructor

CBandRef::CBandRef( CBandComponent* pComponent )
{
	ASSERT( pComponent != NULL );

	m_pComponent = pComponent;
	m_pComponent->AddRef();

    m_dwRef = 0;
	AddRef();

	m_pIDocRootNode = NULL;
	m_pIParentNode = NULL;
	m_pBand = NULL;
}

CBandRef::~CBandRef()
{
	RELEASE( m_pBand );
	RELEASE( m_pComponent );
}


/////////////////////////////////////////////////////////////////////////////
// CBandRef IUnknown implementation

HRESULT CBandRef::QueryInterface( REFIID riid, LPVOID FAR* ppvObj )
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

ULONG CBandRef::AddRef()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	AfxOleLockApp(); 
    return ++m_dwRef;
}

ULONG CBandRef::Release()
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
// CBandRef IDMUSProdNode implementation

/////////////////////////////////////////////////////////////////////////////
// CBandRef IDMUSProdNode::GetNodeImageIndex

HRESULT CBandRef::GetNodeImageIndex( short* pnFirstImage )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pComponent != NULL );

	return( m_pComponent->GetBandRefImageIndex(pnFirstImage) );
}


/////////////////////////////////////////////////////////////////////////////
// CBandRef IDMUSProdNode::GetFirstChild

HRESULT CBandRef::GetFirstChild( IDMUSProdNode** ppIFirstChildNode )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(ppIFirstChildNode);

    return E_NOTIMPL;	// BandRef node does not have children
}


/////////////////////////////////////////////////////////////////////////////
// CBandRef IDMUSProdNode::GetNextChild

HRESULT CBandRef::GetNextChild( IDMUSProdNode* pIChildNode, IDMUSProdNode** ppINextChildNode )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(pIChildNode);
	UNREFERENCED_PARAMETER(ppINextChildNode);

    return E_NOTIMPL;	// BandRef node does not have children
}


/////////////////////////////////////////////////////////////////////////////
// CBandRef IDMUSProdNode::GetComponent

HRESULT CBandRef::GetComponent( IDMUSProdComponent** ppIComponent )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pComponent != NULL );

	return m_pComponent->QueryInterface( IID_IDMUSProdComponent, (void**)ppIComponent );
}


/////////////////////////////////////////////////////////////////////////////
// CBandRef IDMUSProdNode::GetDocRootNode

HRESULT CBandRef::GetDocRootNode( IDMUSProdNode** ppIDocRootNode )
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
// CBandRef IDMUSProdNode::SetDocRootNode

HRESULT CBandRef::SetDocRootNode( IDMUSProdNode* pIDocRootNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( pIDocRootNode != NULL );

	m_pIDocRootNode = pIDocRootNode;
//	m_pIDocRootNode->AddRef();		intentionally missing

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CBandRef IDMUSProdNode::GetParentNode

HRESULT CBandRef::GetParentNode( IDMUSProdNode** ppIParentNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pIParentNode != NULL );

	m_pIParentNode->AddRef();
	*ppIParentNode = m_pIParentNode;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CBandRef IDMUSProdNode::SetParentNode

HRESULT CBandRef::SetParentNode( IDMUSProdNode* pIParentNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( pIParentNode != NULL );

	m_pIParentNode = pIParentNode;
//	m_pIParentNode->AddRef();		intentionally missing

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CBandRef IDMUSProdNode::GetNodeId

HRESULT CBandRef::GetNodeId( GUID* pguid )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pguid == NULL )
	{
		return E_POINTER;
	}

	*pguid = GUID_BandRefNode;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CBandRef IDMUSProdNode::GetNodeName

HRESULT CBandRef::GetNodeName( BSTR* pbstrName )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pBand == NULL )
	{
		TCHAR achNoBand[MID_BUFFER];

		::LoadString( theApp.m_hInstance, IDS_NO_BAND, achNoBand, MID_BUFFER );
		CString strNoBand = achNoBand;
		*pbstrName = strNoBand.AllocSysString();
		return S_OK;
	}

    return m_pBand->GetNodeName( pbstrName );
}


/////////////////////////////////////////////////////////////////////////////
// CBandRef IDMUSProdNode::GetNodeNameMaxLength

HRESULT CBandRef::GetNodeNameMaxLength( short* pnMaxLength )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	*pnMaxLength = -1;	// Can't rename a BandRef node

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CBandRef IDMUSProdNode::ValidateNodeName

HRESULT CBandRef::ValidateNodeName( BSTR bstrName )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(bstrName);

	return E_NOTIMPL;	// Can't rename a BandRef node
}


/////////////////////////////////////////////////////////////////////////////
// CBandRef IDMUSProdNode::SetNodeName

HRESULT CBandRef::SetNodeName( BSTR bstrName )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(bstrName);

	return E_NOTIMPL;	// Can't rename a BandRef node
}


/////////////////////////////////////////////////////////////////////////////
// CBandRef IDMUSProdNode::GetNodeListInfo

HRESULT CBandRef::GetNodeListInfo( DMUSProdListInfo* pListInfo )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pBand == NULL )
	{
		return E_FAIL;
	}

    return m_pBand->GetNodeListInfo( pListInfo );
}


/////////////////////////////////////////////////////////////////////////////
// CBandRef IDMUSProdNode::GetEditorClsId

HRESULT CBandRef::GetEditorClsId( CLSID* pClsId )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pBand == NULL )
	{
		return E_FAIL;
	}

    *pClsId = CLSID_BandEditor;
	
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CBandRef IDMUSProdNode::GetEditorTitle

HRESULT CBandRef::GetEditorTitle( BSTR* pbstrTitle )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pBand == NULL )
	{
		return E_FAIL;
	}

    return m_pBand->GetEditorTitle( pbstrTitle );
}


/////////////////////////////////////////////////////////////////////////////
// CBandRef IDMUSProdNode::GetEditorWindow

HRESULT CBandRef::GetEditorWindow( HWND* hWndEditor )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pBand == NULL )
	{
		return E_FAIL;
	}

    return m_pBand->GetEditorWindow( hWndEditor );
}


/////////////////////////////////////////////////////////////////////////////
// CBandRef IDMUSProdNode::SetEditorWindow

HRESULT CBandRef::SetEditorWindow( HWND hWndEditor )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pBand == NULL )
	{
		return E_FAIL;
	}

    return m_pBand->SetEditorWindow( hWndEditor );
}


/////////////////////////////////////////////////////////////////////////////
// CBandRef IDMUSProdNode::UseOpenCloseImages

HRESULT CBandRef::UseOpenCloseImages( BOOL* pfUseOpenCloseImages )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	*pfUseOpenCloseImages = FALSE;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CBandRef IDMUSProdNode::GetRightClickMenuId

HRESULT CBandRef::GetRightClickMenuId( HINSTANCE* phInstance, UINT* pnMenuId )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	*phInstance = theApp.m_hInstance;
	*pnMenuId   = IDM_BANDREF_NODE_RMENU;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CBandRef IDMUSProdNode::OnRightClickMenuInit

HRESULT CBandRef::OnRightClickMenuInit( HMENU hMenu )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(hMenu);

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CBandRef IDMUSProdNode::OnRightClickMenuSelect

HRESULT CBandRef::OnRightClickMenuSelect( long lCommandId )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	switch( lCommandId )
	{
		case IDM_PROPERTIES:
			if( m_pBand )
			{
				return m_pBand->OnRightClickMenuSelect( lCommandId );
			}
			break;

		default:
			return E_FAIL;
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CBandRef IDMUSProdNode::DeleteChildNode

HRESULT CBandRef::DeleteChildNode( IDMUSProdNode* pIChildNode, BOOL fPromptUser )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(pIChildNode);
	UNREFERENCED_PARAMETER(fPromptUser);

	return E_NOTIMPL;	// BandRef nodes do not have children
}


/////////////////////////////////////////////////////////////////////////////
// CBandRef IDMUSProdNode::InsertChildNode

HRESULT CBandRef::InsertChildNode( IDMUSProdNode* pIChildNode )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(pIChildNode);

	return E_NOTIMPL;	// BandRef nodes do not have children
}


/////////////////////////////////////////////////////////////////////////////
// CBandRef IDMUSProdNode::DeleteNode

HRESULT CBandRef::DeleteNode( BOOL fPromptUser )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(fPromptUser);

	return E_FAIL;		// Cannot delete BandRef node?????
}


/////////////////////////////////////////////////////////////////////////////
// CBandRef IDMUSProdNode::OnNodeSelChanged

HRESULT CBandRef::OnNodeSelChanged( BOOL fSelected )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(fSelected);

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CBandRef IDMUSProdNode::CreateDataObject

HRESULT CBandRef::CreateDataObject( IDataObject** ppIDataObject )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( ppIDataObject == NULL )
	{
		return E_POINTER;
	}

	*ppIDataObject = NULL;

	if( m_pBand )
	{
		// Let Band create data object
		return m_pBand->CreateDataObject( ppIDataObject );
	}

	return E_FAIL;
}


/////////////////////////////////////////////////////////////////////////////
// CBandRef IDMUSProdNode::CanCut

HRESULT CBandRef::CanCut( void )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
 
	return CanDelete();
}


/////////////////////////////////////////////////////////////////////////////
// CBandRef IDMUSProdNode::CanCopy

HRESULT CBandRef::CanCopy( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
 
	if( m_pBand )
	{
		return S_OK;	// Will copy the Band
	}

	return S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CBandRef IDMUSProdNode::CanDelete

HRESULT CBandRef::CanDelete( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pIParentNode != NULL );

	return m_pIParentNode->CanDeleteChildNode( this );
}


/////////////////////////////////////////////////////////////////////////////
// CBandRef IDMUSProdNode::CanDeleteChildNode

HRESULT CBandRef::CanDeleteChildNode( IDMUSProdNode* pIChildNode )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(pIChildNode);

	ASSERT( 0 );

	return E_NOTIMPL;	// Band Reference nodes do not have children
}


/////////////////////////////////////////////////////////////////////////////
// CBandRef IDMUSProdNode::CanPasteFromData

HRESULT CBandRef::CanPasteFromData( IDataObject* pIDataObject, BOOL* pfWillSetReference )
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

	// Create a new CDllJazzDataObject and see if it contains a Band
	CDllJazzDataObject* pDataObject = new CDllJazzDataObject();
	if( pDataObject == NULL )
	{
		return E_OUTOFMEMORY;
	}
	
	HRESULT hr = pDataObject->IsClipFormatAvailable( pIDataObject, m_pComponent->m_cfBand );

	RELEASE( pDataObject );

	if( SUCCEEDED ( hr ) )
	{
		ASSERT( m_pIParentNode != NULL );

		// Let parent decide whether it wants to paste a Band
		return m_pIParentNode->CanChildPasteFromData( pIDataObject, this, pfWillSetReference );
	}

	if( m_pBand )
	{
		// Let Band decide what can be dropped
		return m_pBand->CanPasteFromData( pIDataObject, pfWillSetReference );
	}

	return S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CBandRef IDMUSProdNode::PasteFromData

HRESULT CBandRef::PasteFromData( IDataObject* pIDataObject )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pIDataObject == NULL )
	{
		return E_INVALIDARG;
	}

	// Create a new CDllJazzDataObject and see if it contains a Band
	CDllJazzDataObject* pDataObject = new CDllJazzDataObject();
	if( pDataObject == NULL )
	{
		return E_OUTOFMEMORY;
	}
	
	HRESULT hr = pDataObject->IsClipFormatAvailable( pIDataObject, m_pComponent->m_cfBand );

	RELEASE( pDataObject );

	if( SUCCEEDED ( hr ) )
	{
		ASSERT( m_pIParentNode != NULL );

		// Let parent handle pasting of Band
		return m_pIParentNode->ChildPasteFromData( pIDataObject, this );
	}

	if( m_pBand )
	{
		// Let Band handle paste
		return m_pBand->PasteFromData( pIDataObject );
	}

	return E_FAIL;
}


/////////////////////////////////////////////////////////////////////////////
// CBandRef IDMUSProdNode::CanChildPasteFromData

HRESULT CBandRef::CanChildPasteFromData( IDataObject* pIDataObject, IDMUSProdNode* pIChildNode,
										  BOOL* pfWillSetReference )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(pIDataObject);
	UNREFERENCED_PARAMETER(pIChildNode);
	UNREFERENCED_PARAMETER(pfWillSetReference);

	return E_NOTIMPL;			// Band Reference nodes do not have children
}


/////////////////////////////////////////////////////////////////////////////
// CBandRef IDMUSProdNode::ChildPasteFromData

HRESULT CBandRef::ChildPasteFromData( IDataObject* pIDataObject, IDMUSProdNode* pIChildNode )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(pIDataObject);
	UNREFERENCED_PARAMETER(pIChildNode);

	return E_NOTIMPL;			// Band Reference nodes do not have children
}


/////////////////////////////////////////////////////////////////////////////
// CBandRef IDMUSProdNode::GetObject

HRESULT CBandRef::GetObject( REFCLSID rclsid, REFIID riid, void** ppvObject )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(rclsid);
	UNREFERENCED_PARAMETER(riid);
	UNREFERENCED_PARAMETER(ppvObject);

	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CBandRef IDMUSProdReferenceNode implementation

/////////////////////////////////////////////////////////////////////////////
// CBandRef IDMUSProdReferenceNode::GetReferencedFile

HRESULT CBandRef::GetReferencedFile( IDMUSProdNode** ppIDocRootNode )
{
 	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( ppIDocRootNode == NULL )
	{
		return E_POINTER;
	}

	*ppIDocRootNode = NULL;

	if( m_pBand )
	{
		m_pBand->AddRef();
		*ppIDocRootNode = m_pBand;
		return S_OK;
	}

    return E_FAIL;
}


/////////////////////////////////////////////////////////////////////////////
// CBandRef::IDMUSProdReferenceNode::SetReferencedFile

HRESULT CBandRef::SetReferencedFile( IDMUSProdNode* pIDocRootNode )
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

		// Make sure method was passed a Band Node
		GUID guidNodeId;
		if( SUCCEEDED ( pIDocRootNode->GetNodeId ( &guidNodeId ) ) )
		{
			if( !( IsEqualGUID ( guidNodeId, GUID_BandNode ) ) )
			{
				return E_INVALIDARG;
			}
		}
	}

	// Release previous DocRoot Node
	RELEASE( m_pBand );

	// Link to new DocRoot Node
	if( pIDocRootNode )
	{
		m_pBand = (CBand *)pIDocRootNode;
		m_pBand->AddRef();
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CBandRef Additional functions
