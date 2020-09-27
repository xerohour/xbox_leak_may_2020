// SongRef.cpp : implementation file
//

#include "stdafx.h"
#include "SongDesignerDLL.h"

#include "Song.h"
#include "SongRef.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CSongRef constructor/destructor

CSongRef::CSongRef()
{
    m_dwRef = 0;
	AddRef();

	m_pIDocRootNode = NULL;
	m_pIParentNode = NULL;
	m_pSong = NULL;
}

CSongRef::~CSongRef()
{
	RELEASE( m_pSong );
}


/////////////////////////////////////////////////////////////////////////////
// CSongRef IUnknown implementation

HRESULT CSongRef::QueryInterface( REFIID riid, LPVOID FAR* ppvObj )
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

ULONG CSongRef::AddRef()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	AfxOleLockApp(); 
    return ++m_dwRef;
}

ULONG CSongRef::Release()
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
// CSongRef IDMUSProdNode implementation

/////////////////////////////////////////////////////////////////////////////
// CSongRef IDMUSProdNode::GetNodeImageIndex

HRESULT CSongRef::GetNodeImageIndex( short* pnFirstImage )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( theApp.m_pSongComponent != NULL );

	return( theApp.m_pSongComponent->GetSongRefImageIndex(pnFirstImage) );
}


/////////////////////////////////////////////////////////////////////////////
// CSongRef IDMUSProdNode::GetFirstChild

HRESULT CSongRef::GetFirstChild( IDMUSProdNode** ppIFirstChildNode )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(ppIFirstChildNode);

    return E_NOTIMPL;	// SongRef node does not have children
}


/////////////////////////////////////////////////////////////////////////////
// CSongRef IDMUSProdNode::GetNextChild

HRESULT CSongRef::GetNextChild( IDMUSProdNode* pIChildNode, IDMUSProdNode** ppINextChildNode )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(pIChildNode);
	UNREFERENCED_PARAMETER(ppINextChildNode);

    return E_NOTIMPL;	// SongRef node does not have children
}


/////////////////////////////////////////////////////////////////////////////
// CSongRef IDMUSProdNode::GetComponent

HRESULT CSongRef::GetComponent( IDMUSProdComponent** ppIComponent )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( theApp.m_pSongComponent != NULL );

	return theApp.m_pSongComponent->QueryInterface( IID_IDMUSProdComponent, (void**)ppIComponent );
}


/////////////////////////////////////////////////////////////////////////////
// CSongRef IDMUSProdNode::GetDocRootNode

HRESULT CSongRef::GetDocRootNode( IDMUSProdNode** ppIDocRootNode )
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
// CSongRef IDMUSProdNode::SetDocRootNode

HRESULT CSongRef::SetDocRootNode( IDMUSProdNode* pIDocRootNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( pIDocRootNode != NULL );

	m_pIDocRootNode = pIDocRootNode;
//	m_pIDocRootNode->AddRef();		intentionally missing

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSongRef IDMUSProdNode::GetParentNode

HRESULT CSongRef::GetParentNode( IDMUSProdNode** ppIParentNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pIParentNode != NULL );

	m_pIParentNode->AddRef();
	*ppIParentNode = m_pIParentNode;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSongRef IDMUSProdNode::SetParentNode

HRESULT CSongRef::SetParentNode( IDMUSProdNode* pIParentNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( pIParentNode != NULL );

	m_pIParentNode = pIParentNode;
//	m_pIParentNode->AddRef();		intentionally missing

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSongRef IDMUSProdNode::GetNodeId

HRESULT CSongRef::GetNodeId( GUID* pguid )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pguid == NULL )
	{
		return E_POINTER;
	}

	*pguid = GUID_SongRefNode;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSongRef IDMUSProdNode::GetNodeName

HRESULT CSongRef::GetNodeName( BSTR* pbstrName )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pSong == NULL )
	{
		TCHAR achNoSong[MID_BUFFER];

		::LoadString( theApp.m_hInstance, IDS_NO_SONG, achNoSong, MID_BUFFER );
		CString strNoSong = achNoSong;
		*pbstrName = strNoSong.AllocSysString();
		return S_OK;
	}

    return m_pSong->GetNodeName( pbstrName );
}


/////////////////////////////////////////////////////////////////////////////
// CSongRef IDMUSProdNode::GetNodeNameMaxLength

HRESULT CSongRef::GetNodeNameMaxLength( short* pnMaxLength )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	*pnMaxLength = -1;	// Can't rename a SongRef node

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSongRef IDMUSProdNode::ValidateNodeName

HRESULT CSongRef::ValidateNodeName( BSTR bstrName )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(bstrName);

	return E_NOTIMPL;	// Can't rename a SongRef node
}


/////////////////////////////////////////////////////////////////////////////
// CSongRef IDMUSProdNode::SetNodeName

HRESULT CSongRef::SetNodeName( BSTR bstrName )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(bstrName);

	return E_NOTIMPL;	// Can't rename a SongRef node
}


/////////////////////////////////////////////////////////////////////////////
// CSongRef IDMUSProdNode::GetNodeListInfo

HRESULT CSongRef::GetNodeListInfo( DMUSProdListInfo* pListInfo )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pSong == NULL )
	{
		return E_FAIL;
	}

    return m_pSong->GetNodeListInfo( pListInfo );
}


/////////////////////////////////////////////////////////////////////////////
// CSongRef IDMUSProdNode::GetEditorClsId

HRESULT CSongRef::GetEditorClsId( CLSID* pClsId )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pSong == NULL )
	{
		return E_FAIL;
	}

    *pClsId = CLSID_SongEditor;
	
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSongRef IDMUSProdNode::GetEditorTitle

HRESULT CSongRef::GetEditorTitle( BSTR* pbstrTitle )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pSong == NULL )
	{
		return E_FAIL;
	}

    return m_pSong->GetEditorTitle( pbstrTitle );
}


/////////////////////////////////////////////////////////////////////////////
// CSongRef IDMUSProdNode::GetEditorWindow

HRESULT CSongRef::GetEditorWindow( HWND* hWndEditor )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pSong == NULL )
	{
		return E_FAIL;
	}

    return m_pSong->GetEditorWindow( hWndEditor );
}


/////////////////////////////////////////////////////////////////////////////
// CSongRef IDMUSProdNode::SetEditorWindow

HRESULT CSongRef::SetEditorWindow( HWND hWndEditor )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pSong == NULL )
	{
		return E_FAIL;
	}

    return m_pSong->SetEditorWindow( hWndEditor );
}


/////////////////////////////////////////////////////////////////////////////
// CSongRef IDMUSProdNode::UseOpenCloseImages

HRESULT CSongRef::UseOpenCloseImages( BOOL* pfUseOpenCloseImages )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	*pfUseOpenCloseImages = FALSE;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSongRef IDMUSProdNode::GetRightClickMenuId

HRESULT CSongRef::GetRightClickMenuId( HINSTANCE* phInstance, UINT* pnMenuId )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	*phInstance = theApp.m_hInstance;
	*pnMenuId   = IDM_SONGREF_NODE_RMENU;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSongRef IDMUSProdNode::OnRightClickMenuInit

HRESULT CSongRef::OnRightClickMenuInit( HMENU hMenu )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(hMenu);

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSongRef IDMUSProdNode::OnRightClickMenuSelect

HRESULT CSongRef::OnRightClickMenuSelect( long lCommandId )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	switch( lCommandId )
	{
		case IDM_PROPERTIES:
			if( m_pSong )
			{
				return m_pSong->OnRightClickMenuSelect( lCommandId );
			}
			break;

		default:
			return E_FAIL;
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSongRef IDMUSProdNode::DeleteChildNode

HRESULT CSongRef::DeleteChildNode( IDMUSProdNode* pIChildNode, BOOL fPromptUser )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(pIChildNode);
	UNREFERENCED_PARAMETER(fPromptUser);

	return E_NOTIMPL;	// SongRef nodes do not have children
}


/////////////////////////////////////////////////////////////////////////////
// CSongRef IDMUSProdNode::InsertChildNode

HRESULT CSongRef::InsertChildNode( IDMUSProdNode* pIChildNode )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(pIChildNode);

	return E_NOTIMPL;	// SongRef nodes do not have children
}


/////////////////////////////////////////////////////////////////////////////
// CSongRef IDMUSProdNode::DeleteNode

HRESULT CSongRef::DeleteNode( BOOL fPromptUser )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(fPromptUser);

	return E_FAIL;		// Cannot delete SongRef node?????
}


/////////////////////////////////////////////////////////////////////////////
// CSongRef IDMUSProdNode::OnNodeSelChanged

HRESULT CSongRef::OnNodeSelChanged( BOOL fSelected )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(fSelected);

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSongRef IDMUSProdNode::CreateDataObject

HRESULT CSongRef::CreateDataObject( IDataObject** ppIDataObject )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( ppIDataObject == NULL )
	{
		return E_POINTER;
	}

	*ppIDataObject = NULL;

	if( m_pSong )
	{
		// Let Song create data object
		return m_pSong->CreateDataObject( ppIDataObject );
	}

	return E_FAIL;
}


/////////////////////////////////////////////////////////////////////////////
// CSongRef IDMUSProdNode::CanCut

HRESULT CSongRef::CanCut( void )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
 
	return CanDelete();
}


/////////////////////////////////////////////////////////////////////////////
// CSongRef IDMUSProdNode::CanCopy

HRESULT CSongRef::CanCopy( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
 
	if( m_pSong )
	{
		return S_OK;	// Will copy the Song
	}

	return S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CSongRef IDMUSProdNode::CanDelete

HRESULT CSongRef::CanDelete( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pIParentNode != NULL );

	return m_pIParentNode->CanDeleteChildNode( this );
}


/////////////////////////////////////////////////////////////////////////////
// CSongRef IDMUSProdNode::CanDeleteChildNode

HRESULT CSongRef::CanDeleteChildNode( IDMUSProdNode* pIChildNode )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(pIChildNode);

	ASSERT( 0 );

	return E_NOTIMPL;	// Song Reference nodes do not have children
}


/////////////////////////////////////////////////////////////////////////////
// CSongRef IDMUSProdNode::CanPasteFromData

HRESULT CSongRef::CanPasteFromData( IDataObject* pIDataObject, BOOL* pfWillSetReference )
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

	// Create a new CDllJazzDataObject and see if it contains a Song
	CDllJazzDataObject* pDataObject = new CDllJazzDataObject();
	if( pDataObject == NULL )
	{
		return E_OUTOFMEMORY;
	}
	
	HRESULT hr = pDataObject->IsClipFormatAvailable( pIDataObject, theApp.m_pSongComponent->m_cfSong );

	RELEASE( pDataObject );

	if( SUCCEEDED ( hr ) )
	{
		ASSERT( m_pIParentNode != NULL );

		// Let parent decide whether it wants to paste a Song
		return m_pIParentNode->CanChildPasteFromData( pIDataObject, this, pfWillSetReference );
	}

	if( m_pSong )
	{
		// Let Song decide what can be dropped
		return m_pSong->CanPasteFromData( pIDataObject, pfWillSetReference );
	}

	return S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CSongRef IDMUSProdNode::PasteFromData

HRESULT CSongRef::PasteFromData( IDataObject* pIDataObject )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pIDataObject == NULL )
	{
		return E_INVALIDARG;
	}

	// Create a new CDllJazzDataObject and see if it contains a Song
	CDllJazzDataObject* pDataObject = new CDllJazzDataObject();
	if( pDataObject == NULL )
	{
		return E_OUTOFMEMORY;
	}
	
	HRESULT hr = pDataObject->IsClipFormatAvailable( pIDataObject, theApp.m_pSongComponent->m_cfSong );

	RELEASE( pDataObject );

	if( SUCCEEDED ( hr ) )
	{
		ASSERT( m_pIParentNode != NULL );

		// Let parent handle pasting of Song
		return m_pIParentNode->ChildPasteFromData( pIDataObject, this );
	}

	if( m_pSong )
	{
		// Let Song handle paste
		return m_pSong->PasteFromData( pIDataObject );
	}

	return E_FAIL;
}


/////////////////////////////////////////////////////////////////////////////
// CSongRef IDMUSProdNode::CanChildPasteFromData

HRESULT CSongRef::CanChildPasteFromData( IDataObject* pIDataObject, IDMUSProdNode* pIChildNode,
										  BOOL* pfWillSetReference )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(pIDataObject);
	UNREFERENCED_PARAMETER(pIChildNode);
	UNREFERENCED_PARAMETER(pfWillSetReference);

	return E_NOTIMPL;			// Song Reference nodes do not have children
}


/////////////////////////////////////////////////////////////////////////////
// CSongRef IDMUSProdNode::ChildPasteFromData

HRESULT CSongRef::ChildPasteFromData( IDataObject* pIDataObject, IDMUSProdNode* pIChildNode )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(pIDataObject);
	UNREFERENCED_PARAMETER(pIChildNode);

	return E_NOTIMPL;			// Song Reference nodes do not have children
}


/////////////////////////////////////////////////////////////////////////////
// CSongRef IDMUSProdNode::GetObject

HRESULT CSongRef::GetObject( REFCLSID rclsid, REFIID riid, void** ppvObject )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(rclsid);
	UNREFERENCED_PARAMETER(riid);
	UNREFERENCED_PARAMETER(ppvObject);

	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CSongRef IDMUSProdReferenceNode implementation

/////////////////////////////////////////////////////////////////////////////
// CSongRef IDMUSProdReferenceNode::GetReferencedFile

HRESULT CSongRef::GetReferencedFile( IDMUSProdNode** ppIDocRootNode )
{
 	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( ppIDocRootNode == NULL )
	{
		return E_POINTER;
	}

	*ppIDocRootNode = NULL;

	if( m_pSong )
	{
		m_pSong->AddRef();
		*ppIDocRootNode = m_pSong;
		return S_OK;
	}

    return E_FAIL;
}


/////////////////////////////////////////////////////////////////////////////
// CSongRef::IDMUSProdReferenceNode::SetReferencedFile

HRESULT CSongRef::SetReferencedFile( IDMUSProdNode* pIDocRootNode )
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

		// Make sure method was passed a Song Node
		GUID guidNodeId;
		if( SUCCEEDED ( pIDocRootNode->GetNodeId ( &guidNodeId ) ) )
		{
			if( !( IsEqualGUID ( guidNodeId, GUID_SongNode ) ) )
			{
				return E_INVALIDARG;
			}
		}
	}

	// Release previous DocRoot Node
	RELEASE( m_pSong );

	// Link to new DocRoot Node
	if( pIDocRootNode )
	{
		m_pSong = (CDirectMusicSong *)pIDocRootNode;
		m_pSong->AddRef();
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CSongRef Additional functions
