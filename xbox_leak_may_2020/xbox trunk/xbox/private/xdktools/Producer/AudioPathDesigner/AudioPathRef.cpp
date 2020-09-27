// AudioPathRef.cpp : implementation file
//

#include "stdafx.h"
#include "AudioPathDesignerDLL.h"

#include "AudioPath.h"
#include "AudioPathRef.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CAudioPathRef constructor/destructor

CAudioPathRef::CAudioPathRef()
{
    m_dwRef = 0;
	AddRef();

	m_pIDocRootNode = NULL;
	m_pIParentNode = NULL;
	m_pAudioPath = NULL;
}

CAudioPathRef::~CAudioPathRef()
{
	RELEASE( m_pAudioPath );
}


/////////////////////////////////////////////////////////////////////////////
// CAudioPathRef IUnknown implementation

HRESULT CAudioPathRef::QueryInterface( REFIID riid, LPVOID FAR* ppvObj )
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

ULONG CAudioPathRef::AddRef()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	AfxOleLockApp(); 
    return ++m_dwRef;
}

ULONG CAudioPathRef::Release()
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
// CAudioPathRef IDMUSProdNode implementation

/////////////////////////////////////////////////////////////////////////////
// CAudioPathRef IDMUSProdNode::GetNodeImageIndex

HRESULT CAudioPathRef::GetNodeImageIndex( short* pnFirstImage )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( theApp.m_pAudioPathComponent != NULL );

	return( theApp.m_pAudioPathComponent->GetAudioPathRefImageIndex(pnFirstImage) );
}


/////////////////////////////////////////////////////////////////////////////
// CAudioPathRef IDMUSProdNode::GetFirstChild

HRESULT CAudioPathRef::GetFirstChild( IDMUSProdNode** ppIFirstChildNode )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(ppIFirstChildNode);

    return E_NOTIMPL;	// AudioPathRef node does not have children
}


/////////////////////////////////////////////////////////////////////////////
// CAudioPathRef IDMUSProdNode::GetNextChild

HRESULT CAudioPathRef::GetNextChild( IDMUSProdNode* pIChildNode, IDMUSProdNode** ppINextChildNode )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(pIChildNode);
	UNREFERENCED_PARAMETER(ppINextChildNode);

    return E_NOTIMPL;	// AudioPathRef node does not have children
}


/////////////////////////////////////////////////////////////////////////////
// CAudioPathRef IDMUSProdNode::GetComponent

HRESULT CAudioPathRef::GetComponent( IDMUSProdComponent** ppIComponent )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( theApp.m_pAudioPathComponent != NULL );

	return theApp.m_pAudioPathComponent->QueryInterface( IID_IDMUSProdComponent, (void**)ppIComponent );
}


/////////////////////////////////////////////////////////////////////////////
// CAudioPathRef IDMUSProdNode::GetDocRootNode

HRESULT CAudioPathRef::GetDocRootNode( IDMUSProdNode** ppIDocRootNode )
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
// CAudioPathRef IDMUSProdNode::SetDocRootNode

HRESULT CAudioPathRef::SetDocRootNode( IDMUSProdNode* pIDocRootNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( pIDocRootNode != NULL );

	m_pIDocRootNode = pIDocRootNode;
//	m_pIDocRootNode->AddRef();		intentionally missing

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CAudioPathRef IDMUSProdNode::GetParentNode

HRESULT CAudioPathRef::GetParentNode( IDMUSProdNode** ppIParentNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pIParentNode != NULL );

	m_pIParentNode->AddRef();
	*ppIParentNode = m_pIParentNode;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CAudioPathRef IDMUSProdNode::SetParentNode

HRESULT CAudioPathRef::SetParentNode( IDMUSProdNode* pIParentNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( pIParentNode != NULL );

	m_pIParentNode = pIParentNode;
//	m_pIParentNode->AddRef();		intentionally missing

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CAudioPathRef IDMUSProdNode::GetNodeId

HRESULT CAudioPathRef::GetNodeId( GUID* pguid )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pguid == NULL )
	{
		return E_POINTER;
	}

	*pguid = GUID_AudioPathRefNode;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CAudioPathRef IDMUSProdNode::GetNodeName

HRESULT CAudioPathRef::GetNodeName( BSTR* pbstrName )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pAudioPath == NULL )
	{
		TCHAR achNoAudioPath[MID_BUFFER];

		::LoadString( theApp.m_hInstance, IDS_NO_AUDIOPATH, achNoAudioPath, MID_BUFFER );
		CString strNoAudioPath = achNoAudioPath;
		*pbstrName = strNoAudioPath.AllocSysString();
		return S_OK;
	}

    return m_pAudioPath->GetNodeName( pbstrName );
}


/////////////////////////////////////////////////////////////////////////////
// CAudioPathRef IDMUSProdNode::GetNodeNameMaxLength

HRESULT CAudioPathRef::GetNodeNameMaxLength( short* pnMaxLength )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	*pnMaxLength = -1;	// Can't rename a AudioPathRef node

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CAudioPathRef IDMUSProdNode::ValidateNodeName

HRESULT CAudioPathRef::ValidateNodeName( BSTR bstrName )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(bstrName);

	return E_NOTIMPL;	// Can't rename a AudioPathRef node
}


/////////////////////////////////////////////////////////////////////////////
// CAudioPathRef IDMUSProdNode::SetNodeName

HRESULT CAudioPathRef::SetNodeName( BSTR bstrName )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(bstrName);

	return E_NOTIMPL;	// Can't rename a AudioPathRef node
}


/////////////////////////////////////////////////////////////////////////////
// CAudioPathRef IDMUSProdNode::GetNodeListInfo

HRESULT CAudioPathRef::GetNodeListInfo( DMUSProdListInfo* pListInfo )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pAudioPath == NULL )
	{
		return E_FAIL;
	}

    return m_pAudioPath->GetNodeListInfo( pListInfo );
}


/////////////////////////////////////////////////////////////////////////////
// CAudioPathRef IDMUSProdNode::GetEditorClsId

HRESULT CAudioPathRef::GetEditorClsId( CLSID* pClsId )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pAudioPath == NULL )
	{
		return E_FAIL;
	}

    *pClsId = CLSID_AudioPathEditor;
	
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CAudioPathRef IDMUSProdNode::GetEditorTitle

HRESULT CAudioPathRef::GetEditorTitle( BSTR* pbstrTitle )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pAudioPath == NULL )
	{
		return E_FAIL;
	}

    return m_pAudioPath->GetEditorTitle( pbstrTitle );
}


/////////////////////////////////////////////////////////////////////////////
// CAudioPathRef IDMUSProdNode::GetEditorWindow

HRESULT CAudioPathRef::GetEditorWindow( HWND* hWndEditor )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pAudioPath == NULL )
	{
		return E_FAIL;
	}

    return m_pAudioPath->GetEditorWindow( hWndEditor );
}


/////////////////////////////////////////////////////////////////////////////
// CAudioPathRef IDMUSProdNode::SetEditorWindow

HRESULT CAudioPathRef::SetEditorWindow( HWND hWndEditor )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pAudioPath == NULL )
	{
		return E_FAIL;
	}

    return m_pAudioPath->SetEditorWindow( hWndEditor );
}


/////////////////////////////////////////////////////////////////////////////
// CAudioPathRef IDMUSProdNode::UseOpenCloseImages

HRESULT CAudioPathRef::UseOpenCloseImages( BOOL* pfUseOpenCloseImages )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	*pfUseOpenCloseImages = FALSE;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CAudioPathRef IDMUSProdNode::GetRightClickMenuId

HRESULT CAudioPathRef::GetRightClickMenuId( HINSTANCE* phInstance, UINT* pnMenuId )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	*phInstance = theApp.m_hInstance;
	*pnMenuId   = IDM_AUDIOPATHREF_NODE_RMENU;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CAudioPathRef IDMUSProdNode::OnRightClickMenuInit

HRESULT CAudioPathRef::OnRightClickMenuInit( HMENU hMenu )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(hMenu);

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CAudioPathRef IDMUSProdNode::OnRightClickMenuSelect

HRESULT CAudioPathRef::OnRightClickMenuSelect( long lCommandId )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	switch( lCommandId )
	{
		case IDM_PROPERTIES:
			if( m_pAudioPath )
			{
				return m_pAudioPath->OnRightClickMenuSelect( lCommandId );
			}
			break;

		default:
			return E_FAIL;
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CAudioPathRef IDMUSProdNode::DeleteChildNode

HRESULT CAudioPathRef::DeleteChildNode( IDMUSProdNode* pIChildNode, BOOL fPromptUser )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(pIChildNode);
	UNREFERENCED_PARAMETER(fPromptUser);

	return E_NOTIMPL;	// AudioPathRef nodes do not have children
}


/////////////////////////////////////////////////////////////////////////////
// CAudioPathRef IDMUSProdNode::InsertChildNode

HRESULT CAudioPathRef::InsertChildNode( IDMUSProdNode* pIChildNode )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(pIChildNode);

	return E_NOTIMPL;	// AudioPathRef nodes do not have children
}


/////////////////////////////////////////////////////////////////////////////
// CAudioPathRef IDMUSProdNode::DeleteNode

HRESULT CAudioPathRef::DeleteNode( BOOL fPromptUser )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(fPromptUser);

	return E_FAIL;		// Cannot delete AudioPathRef node?????
}


/////////////////////////////////////////////////////////////////////////////
// CAudioPathRef IDMUSProdNode::OnNodeSelChanged

HRESULT CAudioPathRef::OnNodeSelChanged( BOOL fSelected )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(fSelected);

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CAudioPathRef IDMUSProdNode::CreateDataObject

HRESULT CAudioPathRef::CreateDataObject( IDataObject** ppIDataObject )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( ppIDataObject == NULL )
	{
		return E_POINTER;
	}

	*ppIDataObject = NULL;

	if( m_pAudioPath )
	{
		// Let AudioPath create data object
		return m_pAudioPath->CreateDataObject( ppIDataObject );
	}

	return E_FAIL;
}


/////////////////////////////////////////////////////////////////////////////
// CAudioPathRef IDMUSProdNode::CanCut

HRESULT CAudioPathRef::CanCut( void )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
 
	return CanDelete();
}


/////////////////////////////////////////////////////////////////////////////
// CAudioPathRef IDMUSProdNode::CanCopy

HRESULT CAudioPathRef::CanCopy( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
 
	if( m_pAudioPath )
	{
		return S_OK;	// Will copy the AudioPath
	}

	return S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CAudioPathRef IDMUSProdNode::CanDelete

HRESULT CAudioPathRef::CanDelete( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pIParentNode != NULL );

	return m_pIParentNode->CanDeleteChildNode( this );
}


/////////////////////////////////////////////////////////////////////////////
// CAudioPathRef IDMUSProdNode::CanDeleteChildNode

HRESULT CAudioPathRef::CanDeleteChildNode( IDMUSProdNode* pIChildNode )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(pIChildNode);

	ASSERT( 0 );

	return E_NOTIMPL;	// AudioPath Reference nodes do not have children
}


/////////////////////////////////////////////////////////////////////////////
// CAudioPathRef IDMUSProdNode::CanPasteFromData

HRESULT CAudioPathRef::CanPasteFromData( IDataObject* pIDataObject, BOOL* pfWillSetReference )
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

	// Create a new CDllJazzDataObject and see if it contains a AudioPath
	CDllJazzDataObject* pDataObject = new CDllJazzDataObject();
	if( pDataObject == NULL )
	{
		return E_OUTOFMEMORY;
	}
	
	HRESULT hr = pDataObject->IsClipFormatAvailable( pIDataObject, theApp.m_pAudioPathComponent->m_cfAudioPath );

	RELEASE( pDataObject );

	if( SUCCEEDED ( hr ) )
	{
		ASSERT( m_pIParentNode != NULL );

		// Let parent decide whether it wants to paste a AudioPath
		return m_pIParentNode->CanChildPasteFromData( pIDataObject, this, pfWillSetReference );
	}

	if( m_pAudioPath )
	{
		// Let AudioPath decide what can be dropped
		return m_pAudioPath->CanPasteFromData( pIDataObject, pfWillSetReference );
	}

	return S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CAudioPathRef IDMUSProdNode::PasteFromData

HRESULT CAudioPathRef::PasteFromData( IDataObject* pIDataObject )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pIDataObject == NULL )
	{
		return E_INVALIDARG;
	}

	// Create a new CDllJazzDataObject and see if it contains a AudioPath
	CDllJazzDataObject* pDataObject = new CDllJazzDataObject();
	if( pDataObject == NULL )
	{
		return E_OUTOFMEMORY;
	}
	
	HRESULT hr = pDataObject->IsClipFormatAvailable( pIDataObject, theApp.m_pAudioPathComponent->m_cfAudioPath );

	RELEASE( pDataObject );

	if( SUCCEEDED ( hr ) )
	{
		ASSERT( m_pIParentNode != NULL );

		// Let parent handle pasting of AudioPath
		return m_pIParentNode->ChildPasteFromData( pIDataObject, this );
	}

	if( m_pAudioPath )
	{
		// Let AudioPath handle paste
		return m_pAudioPath->PasteFromData( pIDataObject );
	}

	return E_FAIL;
}


/////////////////////////////////////////////////////////////////////////////
// CAudioPathRef IDMUSProdNode::CanChildPasteFromData

HRESULT CAudioPathRef::CanChildPasteFromData( IDataObject* pIDataObject, IDMUSProdNode* pIChildNode,
										  BOOL* pfWillSetReference )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(pIDataObject);
	UNREFERENCED_PARAMETER(pIChildNode);
	UNREFERENCED_PARAMETER(pfWillSetReference);

	return E_NOTIMPL;			// AudioPath Reference nodes do not have children
}


/////////////////////////////////////////////////////////////////////////////
// CAudioPathRef IDMUSProdNode::ChildPasteFromData

HRESULT CAudioPathRef::ChildPasteFromData( IDataObject* pIDataObject, IDMUSProdNode* pIChildNode )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(pIDataObject);
	UNREFERENCED_PARAMETER(pIChildNode);

	return E_NOTIMPL;			// AudioPath Reference nodes do not have children
}


/////////////////////////////////////////////////////////////////////////////
// CAudioPathRef IDMUSProdNode::GetObject

HRESULT CAudioPathRef::GetObject( REFCLSID rclsid, REFIID riid, void** ppvObject )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(rclsid);
	UNREFERENCED_PARAMETER(riid);
	UNREFERENCED_PARAMETER(ppvObject);

	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CAudioPathRef IDMUSProdReferenceNode implementation

/////////////////////////////////////////////////////////////////////////////
// CAudioPathRef IDMUSProdReferenceNode::GetReferencedFile

HRESULT CAudioPathRef::GetReferencedFile( IDMUSProdNode** ppIDocRootNode )
{
 	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( ppIDocRootNode == NULL )
	{
		return E_POINTER;
	}

	*ppIDocRootNode = NULL;

	if( m_pAudioPath )
	{
		m_pAudioPath->AddRef();
		*ppIDocRootNode = m_pAudioPath;
		return S_OK;
	}

    return E_FAIL;
}


/////////////////////////////////////////////////////////////////////////////
// CAudioPathRef::IDMUSProdReferenceNode::SetReferencedFile

HRESULT CAudioPathRef::SetReferencedFile( IDMUSProdNode* pIDocRootNode )
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

		// Make sure method was passed a AudioPath Node
		GUID guidNodeId;
		if( SUCCEEDED ( pIDocRootNode->GetNodeId ( &guidNodeId ) ) )
		{
			if( !( IsEqualGUID ( guidNodeId, GUID_AudioPathNode ) ) )
			{
				return E_INVALIDARG;
			}
		}
	}

	// Release previous DocRoot Node
	RELEASE( m_pAudioPath );

	// Link to new DocRoot Node
	if( pIDocRootNode )
	{
		m_pAudioPath = (CDirectMusicAudioPath *)pIDocRootNode;
		m_pAudioPath->AddRef();
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CAudioPathRef Additional functions
