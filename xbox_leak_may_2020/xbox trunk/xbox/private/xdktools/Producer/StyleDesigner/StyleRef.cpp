// StyleRef.cpp : implementation file
//

#include "stdafx.h"
#include "StyleDesignerDLL.h"

#include "Style.h"
#include "StyleRef.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CStyleRef constructor/destructor

CStyleRef::CStyleRef()
{
    m_dwRef = 0;
	AddRef();

	m_pIDocRootNode = NULL;
	m_pIParentNode = NULL;
	m_pStyle = NULL;
}

CStyleRef::~CStyleRef()
{
	RELEASE( m_pStyle );
}


/////////////////////////////////////////////////////////////////////////////
// CStyleRef IUnknown implementation

HRESULT CStyleRef::QueryInterface( REFIID riid, LPVOID FAR* ppvObj )
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

ULONG CStyleRef::AddRef()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	AfxOleLockApp(); 
    return ++m_dwRef;
}

ULONG CStyleRef::Release()
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
// CStyleRef IDMUSProdNode implementation

/////////////////////////////////////////////////////////////////////////////
// CStyleRef IDMUSProdNode::GetNodeImageIndex

HRESULT CStyleRef::GetNodeImageIndex( short* pnFirstImage )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( theApp.m_pStyleComponent != NULL );

	return( theApp.m_pStyleComponent->GetStyleRefImageIndex(pnFirstImage) );
}


/////////////////////////////////////////////////////////////////////////////
// CStyleRef IDMUSProdNode::GetFirstChild

HRESULT CStyleRef::GetFirstChild( IDMUSProdNode** ppIFirstChildNode )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(ppIFirstChildNode);

    return E_NOTIMPL;	// StyleRef node does not have children
}


/////////////////////////////////////////////////////////////////////////////
// CStyleRef IDMUSProdNode::GetNextChild

HRESULT CStyleRef::GetNextChild( IDMUSProdNode* pIChildNode, IDMUSProdNode** ppINextChildNode )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(pIChildNode);
	UNREFERENCED_PARAMETER(ppINextChildNode);

    return E_NOTIMPL;	// StyleRef node does not have children
}


/////////////////////////////////////////////////////////////////////////////
// CStyleRef IDMUSProdNode::GetComponent

HRESULT CStyleRef::GetComponent( IDMUSProdComponent** ppIComponent )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( theApp.m_pStyleComponent != NULL );

	return theApp.m_pStyleComponent->QueryInterface( IID_IDMUSProdComponent, (void**)ppIComponent );
}


/////////////////////////////////////////////////////////////////////////////
// CStyleRef IDMUSProdNode::GetDocRootNode

HRESULT CStyleRef::GetDocRootNode( IDMUSProdNode** ppIDocRootNode )
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
// CStyleRef IDMUSProdNode::SetDocRootNode

HRESULT CStyleRef::SetDocRootNode( IDMUSProdNode* pIDocRootNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( pIDocRootNode != NULL );

	m_pIDocRootNode = pIDocRootNode;
//	m_pIDocRootNode->AddRef();		intentionally missing

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleRef IDMUSProdNode::GetParentNode

HRESULT CStyleRef::GetParentNode( IDMUSProdNode** ppIParentNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pIParentNode != NULL );

	m_pIParentNode->AddRef();
	*ppIParentNode = m_pIParentNode;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleRef IDMUSProdNode::SetParentNode

HRESULT CStyleRef::SetParentNode( IDMUSProdNode* pIParentNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( pIParentNode != NULL );

	m_pIParentNode = pIParentNode;
//	m_pIParentNode->AddRef();		intentionally missing

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleRef IDMUSProdNode::GetNodeId

HRESULT CStyleRef::GetNodeId( GUID* pguid )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pguid == NULL )
	{
		return E_POINTER;
	}

	*pguid = GUID_StyleRefNode;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleRef IDMUSProdNode::GetNodeName

HRESULT CStyleRef::GetNodeName( BSTR* pbstrName )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pStyle == NULL )
	{
		TCHAR achNoStyle[MID_BUFFER];

		::LoadString( theApp.m_hInstance, IDS_NO_STYLE, achNoStyle, MID_BUFFER );
		CString strNoStyle = achNoStyle;
		*pbstrName = strNoStyle.AllocSysString();
		return S_OK;
	}

    return m_pStyle->GetNodeName( pbstrName );
}


/////////////////////////////////////////////////////////////////////////////
// CStyleRef IDMUSProdNode::GetNodeNameMaxLength

HRESULT CStyleRef::GetNodeNameMaxLength( short* pnMaxLength )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	*pnMaxLength = -1;	// Can't rename a StyleRef node

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleRef IDMUSProdNode::ValidateNodeName

HRESULT CStyleRef::ValidateNodeName( BSTR bstrName )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(bstrName);

	return E_NOTIMPL;	// Can't rename a StyleRef node
}


/////////////////////////////////////////////////////////////////////////////
// CStyleRef IDMUSProdNode::SetNodeName

HRESULT CStyleRef::SetNodeName( BSTR bstrName )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(bstrName);

	return E_NOTIMPL;	// Can't rename a StyleRef node
}


/////////////////////////////////////////////////////////////////////////////
// CStyleRef IDMUSProdNode::GetNodeListInfo

HRESULT CStyleRef::GetNodeListInfo( DMUSProdListInfo* pListInfo )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pStyle == NULL )
	{
		return E_FAIL;
	}

    return m_pStyle->GetNodeListInfo( pListInfo );
}


/////////////////////////////////////////////////////////////////////////////
// CStyleRef IDMUSProdNode::GetEditorClsId

HRESULT CStyleRef::GetEditorClsId( CLSID* pClsId )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pStyle == NULL )
	{
		return E_FAIL;
	}

    *pClsId = CLSID_StyleEditor;
	
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleRef IDMUSProdNode::GetEditorTitle

HRESULT CStyleRef::GetEditorTitle( BSTR* pbstrTitle )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pStyle == NULL )
	{
		return E_FAIL;
	}

    return m_pStyle->GetEditorTitle( pbstrTitle );
}


/////////////////////////////////////////////////////////////////////////////
// CStyleRef IDMUSProdNode::GetEditorWindow

HRESULT CStyleRef::GetEditorWindow( HWND* hWndEditor )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pStyle == NULL )
	{
		return E_FAIL;
	}

    return m_pStyle->GetEditorWindow( hWndEditor );
}


/////////////////////////////////////////////////////////////////////////////
// CStyleRef IDMUSProdNode::SetEditorWindow

HRESULT CStyleRef::SetEditorWindow( HWND hWndEditor )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pStyle == NULL )
	{
		return E_FAIL;
	}

    return m_pStyle->SetEditorWindow( hWndEditor );
}


/////////////////////////////////////////////////////////////////////////////
// CStyleRef IDMUSProdNode::UseOpenCloseImages

HRESULT CStyleRef::UseOpenCloseImages( BOOL* pfUseOpenCloseImages )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	*pfUseOpenCloseImages = FALSE;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleRef IDMUSProdNode::GetRightClickMenuId

HRESULT CStyleRef::GetRightClickMenuId( HINSTANCE* phInstance, UINT* pnMenuId )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	*phInstance = theApp.m_hInstance;
	*pnMenuId   = IDM_STYLEREF_NODE_RMENU;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleRef IDMUSProdNode::OnRightClickMenuInit

HRESULT CStyleRef::OnRightClickMenuInit( HMENU hMenu )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(hMenu);

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleRef IDMUSProdNode::OnRightClickMenuSelect

HRESULT CStyleRef::OnRightClickMenuSelect( long lCommandId )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	switch( lCommandId )
	{
		case IDM_PROPERTIES:
			if( m_pStyle )
			{
				return m_pStyle->OnRightClickMenuSelect( lCommandId );
			}
			break;

		default:
			return E_FAIL;
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleRef IDMUSProdNode::DeleteChildNode

HRESULT CStyleRef::DeleteChildNode( IDMUSProdNode* pIChildNode, BOOL fPromptUser )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(pIChildNode);
	UNREFERENCED_PARAMETER(fPromptUser);

	return E_NOTIMPL;	// StyleRef nodes do not have children
}


/////////////////////////////////////////////////////////////////////////////
// CStyleRef IDMUSProdNode::InsertChildNode

HRESULT CStyleRef::InsertChildNode( IDMUSProdNode* pIChildNode )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(pIChildNode);

	return E_NOTIMPL;	// StyleRef nodes do not have children
}


/////////////////////////////////////////////////////////////////////////////
// CStyleRef IDMUSProdNode::DeleteNode

HRESULT CStyleRef::DeleteNode( BOOL fPromptUser )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(fPromptUser);

	return E_FAIL;		// Cannot delete StyleRef node?????
}


/////////////////////////////////////////////////////////////////////////////
// CStyleRef IDMUSProdNode::OnNodeSelChanged

HRESULT CStyleRef::OnNodeSelChanged( BOOL fSelected )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(fSelected);

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleRef IDMUSProdNode::CreateDataObject

HRESULT CStyleRef::CreateDataObject( IDataObject** ppIDataObject )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( ppIDataObject == NULL )
	{
		return E_POINTER;
	}

	*ppIDataObject = NULL;

	if( m_pStyle )
	{
		// Let Style create data object
		return m_pStyle->CreateDataObject( ppIDataObject );
	}

	return E_FAIL;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleRef IDMUSProdNode::CanCut

HRESULT CStyleRef::CanCut( void )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
 
	return CanDelete();
}


/////////////////////////////////////////////////////////////////////////////
// CStyleRef IDMUSProdNode::CanCopy

HRESULT CStyleRef::CanCopy( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
 
	if( m_pStyle )
	{
		return S_OK;	// Will copy the Style
	}

	return S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleRef IDMUSProdNode::CanDelete

HRESULT CStyleRef::CanDelete( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pIParentNode != NULL );

	return m_pIParentNode->CanDeleteChildNode( this );
}


/////////////////////////////////////////////////////////////////////////////
// CStyleRef IDMUSProdNode::CanDeleteChildNode

HRESULT CStyleRef::CanDeleteChildNode( IDMUSProdNode* pIChildNode )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(pIChildNode);

	ASSERT( 0 );

	return E_NOTIMPL;	// Style Reference nodes do not have children
}


/////////////////////////////////////////////////////////////////////////////
// CStyleRef IDMUSProdNode::CanPasteFromData

HRESULT CStyleRef::CanPasteFromData( IDataObject* pIDataObject, BOOL* pfWillSetReference )
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

	// Create a new CDllJazzDataObject and see if it contains a Style
	CDllJazzDataObject* pDataObject = new CDllJazzDataObject();
	if( pDataObject == NULL )
	{
		return E_OUTOFMEMORY;
	}
	
	HRESULT hr = pDataObject->IsClipFormatAvailable( pIDataObject, theApp.m_pStyleComponent->m_cfStyle );

	RELEASE( pDataObject );

	if( SUCCEEDED ( hr ) )
	{
		ASSERT( m_pIParentNode != NULL );

		// Let parent decide whether it wants to paste a Style
		return m_pIParentNode->CanChildPasteFromData( pIDataObject, this, pfWillSetReference );
	}

	if( m_pStyle )
	{
		// Let Style decide what can be dropped
		return m_pStyle->CanPasteFromData( pIDataObject, pfWillSetReference );
	}

	return S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleRef IDMUSProdNode::PasteFromData

HRESULT CStyleRef::PasteFromData( IDataObject* pIDataObject )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pIDataObject == NULL )
	{
		return E_INVALIDARG;
	}

	// Create a new CDllJazzDataObject and see if it contains a Style
	CDllJazzDataObject* pDataObject = new CDllJazzDataObject();
	if( pDataObject == NULL )
	{
		return E_OUTOFMEMORY;
	}
	
	HRESULT hr = pDataObject->IsClipFormatAvailable( pIDataObject, theApp.m_pStyleComponent->m_cfStyle );

	RELEASE( pDataObject );

	if( SUCCEEDED ( hr ) )
	{
		ASSERT( m_pIParentNode != NULL );

		// Let parent handle pasting of Style
		return m_pIParentNode->ChildPasteFromData( pIDataObject, this );
	}

	if( m_pStyle )
	{
		// Let Style handle paste
		return m_pStyle->PasteFromData( pIDataObject );
	}

	return E_FAIL;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleRef IDMUSProdNode::CanChildPasteFromData

HRESULT CStyleRef::CanChildPasteFromData( IDataObject* pIDataObject, IDMUSProdNode* pIChildNode,
										  BOOL* pfWillSetReference )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(pIDataObject);
	UNREFERENCED_PARAMETER(pIChildNode);
	UNREFERENCED_PARAMETER(pfWillSetReference);

	return E_NOTIMPL;			// Style Reference nodes do not have children
}


/////////////////////////////////////////////////////////////////////////////
// CStyleRef IDMUSProdNode::ChildPasteFromData

HRESULT CStyleRef::ChildPasteFromData( IDataObject* pIDataObject, IDMUSProdNode* pIChildNode )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(pIDataObject);
	UNREFERENCED_PARAMETER(pIChildNode);

	return E_NOTIMPL;			// Style Reference nodes do not have children
}


/////////////////////////////////////////////////////////////////////////////
// CStyleRef IDMUSProdNode::GetObject

HRESULT CStyleRef::GetObject( REFCLSID rclsid, REFIID riid, void** ppvObject )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(rclsid);
	UNREFERENCED_PARAMETER(riid);
	UNREFERENCED_PARAMETER(ppvObject);

	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleRef IDMUSProdReferenceNode implementation

/////////////////////////////////////////////////////////////////////////////
// CStyleRef IDMUSProdReferenceNode::GetReferencedFile

HRESULT CStyleRef::GetReferencedFile( IDMUSProdNode** ppIDocRootNode )
{
 	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( ppIDocRootNode == NULL )
	{
		return E_POINTER;
	}

	*ppIDocRootNode = NULL;

	if( m_pStyle )
	{
		m_pStyle->AddRef();
		*ppIDocRootNode = m_pStyle;
		return S_OK;
	}

    return E_FAIL;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleRef::IDMUSProdReferenceNode::SetReferencedFile

HRESULT CStyleRef::SetReferencedFile( IDMUSProdNode* pIDocRootNode )
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

		// Make sure method was passed a Style Node
		GUID guidNodeId;
		if( SUCCEEDED ( pIDocRootNode->GetNodeId ( &guidNodeId ) ) )
		{
			if( !( IsEqualGUID ( guidNodeId, GUID_StyleNode ) ) )
			{
				return E_INVALIDARG;
			}
		}
	}

	// Release previous DocRoot Node
	RELEASE( m_pStyle );

	// Link to new DocRoot Node
	if( pIDocRootNode )
	{
		m_pStyle = (CDirectMusicStyle *)pIDocRootNode;
		m_pStyle->AddRef();
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CStyleRef Additional functions
