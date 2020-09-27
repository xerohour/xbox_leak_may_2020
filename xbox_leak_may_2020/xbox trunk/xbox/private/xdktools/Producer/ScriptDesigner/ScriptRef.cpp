// ScriptRef.cpp : implementation file
//

#include "stdafx.h"
#include "ScriptDesignerDLL.h"

#include "Script.h"
#include "ScriptRef.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CScriptRef constructor/destructor

CScriptRef::CScriptRef()
{
    m_dwRef = 0;
	AddRef();

	m_pIDocRootNode = NULL;
	m_pIParentNode = NULL;
	m_pScript = NULL;
}

CScriptRef::~CScriptRef()
{
	RELEASE( m_pScript );
}


/////////////////////////////////////////////////////////////////////////////
// CScriptRef IUnknown implementation

HRESULT CScriptRef::QueryInterface( REFIID riid, LPVOID FAR* ppvObj )
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

ULONG CScriptRef::AddRef()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	AfxOleLockApp(); 
    return ++m_dwRef;
}

ULONG CScriptRef::Release()
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
// CScriptRef IDMUSProdNode implementation

/////////////////////////////////////////////////////////////////////////////
// CScriptRef IDMUSProdNode::GetNodeImageIndex

HRESULT CScriptRef::GetNodeImageIndex( short* pnFirstImage )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( theApp.m_pScriptComponent != NULL );

	return( theApp.m_pScriptComponent->GetScriptRefImageIndex(pnFirstImage) );
}


/////////////////////////////////////////////////////////////////////////////
// CScriptRef IDMUSProdNode::GetFirstChild

HRESULT CScriptRef::GetFirstChild( IDMUSProdNode** ppIFirstChildNode )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(ppIFirstChildNode);

    return E_NOTIMPL;	// ScriptRef node does not have children
}


/////////////////////////////////////////////////////////////////////////////
// CScriptRef IDMUSProdNode::GetNextChild

HRESULT CScriptRef::GetNextChild( IDMUSProdNode* pIChildNode, IDMUSProdNode** ppINextChildNode )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(pIChildNode);
	UNREFERENCED_PARAMETER(ppINextChildNode);

    return E_NOTIMPL;	// ScriptRef node does not have children
}


/////////////////////////////////////////////////////////////////////////////
// CScriptRef IDMUSProdNode::GetComponent

HRESULT CScriptRef::GetComponent( IDMUSProdComponent** ppIComponent )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( theApp.m_pScriptComponent != NULL );

	return theApp.m_pScriptComponent->QueryInterface( IID_IDMUSProdComponent, (void**)ppIComponent );
}


/////////////////////////////////////////////////////////////////////////////
// CScriptRef IDMUSProdNode::GetDocRootNode

HRESULT CScriptRef::GetDocRootNode( IDMUSProdNode** ppIDocRootNode )
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
// CScriptRef IDMUSProdNode::SetDocRootNode

HRESULT CScriptRef::SetDocRootNode( IDMUSProdNode* pIDocRootNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( pIDocRootNode != NULL );

	m_pIDocRootNode = pIDocRootNode;
//	m_pIDocRootNode->AddRef();		intentionally missing

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CScriptRef IDMUSProdNode::GetParentNode

HRESULT CScriptRef::GetParentNode( IDMUSProdNode** ppIParentNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pIParentNode != NULL );

	m_pIParentNode->AddRef();
	*ppIParentNode = m_pIParentNode;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CScriptRef IDMUSProdNode::SetParentNode

HRESULT CScriptRef::SetParentNode( IDMUSProdNode* pIParentNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( pIParentNode != NULL );

	m_pIParentNode = pIParentNode;
//	m_pIParentNode->AddRef();		intentionally missing

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CScriptRef IDMUSProdNode::GetNodeId

HRESULT CScriptRef::GetNodeId( GUID* pguid )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pguid == NULL )
	{
		return E_POINTER;
	}

	*pguid = GUID_ScriptRefNode;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CScriptRef IDMUSProdNode::GetNodeName

HRESULT CScriptRef::GetNodeName( BSTR* pbstrName )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pScript == NULL )
	{
		TCHAR achNoScript[MID_BUFFER];

		::LoadString( theApp.m_hInstance, IDS_NO_SCRIPT, achNoScript, MID_BUFFER );
		CString strNoScript = achNoScript;
		*pbstrName = strNoScript.AllocSysString();
		return S_OK;
	}

    return m_pScript->GetNodeName( pbstrName );
}


/////////////////////////////////////////////////////////////////////////////
// CScriptRef IDMUSProdNode::GetNodeNameMaxLength

HRESULT CScriptRef::GetNodeNameMaxLength( short* pnMaxLength )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	*pnMaxLength = -1;	// Can't rename a ScriptRef node

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CScriptRef IDMUSProdNode::ValidateNodeName

HRESULT CScriptRef::ValidateNodeName( BSTR bstrName )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(bstrName);

	return E_NOTIMPL;	// Can't rename a ScriptRef node
}


/////////////////////////////////////////////////////////////////////////////
// CScriptRef IDMUSProdNode::SetNodeName

HRESULT CScriptRef::SetNodeName( BSTR bstrName )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(bstrName);

	return E_NOTIMPL;	// Can't rename a ScriptRef node
}


/////////////////////////////////////////////////////////////////////////////
// CScriptRef IDMUSProdNode::GetNodeListInfo

HRESULT CScriptRef::GetNodeListInfo( DMUSProdListInfo* pListInfo )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pScript == NULL )
	{
		return E_FAIL;
	}

    return m_pScript->GetNodeListInfo( pListInfo );
}


/////////////////////////////////////////////////////////////////////////////
// CScriptRef IDMUSProdNode::GetEditorClsId

HRESULT CScriptRef::GetEditorClsId( CLSID* pClsId )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pScript == NULL )
	{
		return E_FAIL;
	}

    *pClsId = CLSID_ScriptEditor;
	
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CScriptRef IDMUSProdNode::GetEditorTitle

HRESULT CScriptRef::GetEditorTitle( BSTR* pbstrTitle )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pScript == NULL )
	{
		return E_FAIL;
	}

    return m_pScript->GetEditorTitle( pbstrTitle );
}


/////////////////////////////////////////////////////////////////////////////
// CScriptRef IDMUSProdNode::GetEditorWindow

HRESULT CScriptRef::GetEditorWindow( HWND* hWndEditor )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pScript == NULL )
	{
		return E_FAIL;
	}

    return m_pScript->GetEditorWindow( hWndEditor );
}


/////////////////////////////////////////////////////////////////////////////
// CScriptRef IDMUSProdNode::SetEditorWindow

HRESULT CScriptRef::SetEditorWindow( HWND hWndEditor )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pScript == NULL )
	{
		return E_FAIL;
	}

    return m_pScript->SetEditorWindow( hWndEditor );
}


/////////////////////////////////////////////////////////////////////////////
// CScriptRef IDMUSProdNode::UseOpenCloseImages

HRESULT CScriptRef::UseOpenCloseImages( BOOL* pfUseOpenCloseImages )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	*pfUseOpenCloseImages = FALSE;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CScriptRef IDMUSProdNode::GetRightClickMenuId

HRESULT CScriptRef::GetRightClickMenuId( HINSTANCE* phInstance, UINT* pnMenuId )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	*phInstance = theApp.m_hInstance;
	*pnMenuId   = IDM_SCRIPTREF_NODE_RMENU;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CScriptRef IDMUSProdNode::OnRightClickMenuInit

HRESULT CScriptRef::OnRightClickMenuInit( HMENU hMenu )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(hMenu);

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CScriptRef IDMUSProdNode::OnRightClickMenuSelect

HRESULT CScriptRef::OnRightClickMenuSelect( long lCommandId )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	switch( lCommandId )
	{
		case IDM_PROPERTIES:
			if( m_pScript )
			{
				return m_pScript->OnRightClickMenuSelect( lCommandId );
			}
			break;

		default:
			return E_FAIL;
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CScriptRef IDMUSProdNode::DeleteChildNode

HRESULT CScriptRef::DeleteChildNode( IDMUSProdNode* pIChildNode, BOOL fPromptUser )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(pIChildNode);
	UNREFERENCED_PARAMETER(fPromptUser);

	return E_NOTIMPL;	// ScriptRef nodes do not have children
}


/////////////////////////////////////////////////////////////////////////////
// CScriptRef IDMUSProdNode::InsertChildNode

HRESULT CScriptRef::InsertChildNode( IDMUSProdNode* pIChildNode )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(pIChildNode);

	return E_NOTIMPL;	// ScriptRef nodes do not have children
}


/////////////////////////////////////////////////////////////////////////////
// CScriptRef IDMUSProdNode::DeleteNode

HRESULT CScriptRef::DeleteNode( BOOL fPromptUser )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(fPromptUser);

	return E_FAIL;		// Cannot delete ScriptRef node?????
}


/////////////////////////////////////////////////////////////////////////////
// CScriptRef IDMUSProdNode::OnNodeSelChanged

HRESULT CScriptRef::OnNodeSelChanged( BOOL fSelected )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(fSelected);

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CScriptRef IDMUSProdNode::CreateDataObject

HRESULT CScriptRef::CreateDataObject( IDataObject** ppIDataObject )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( ppIDataObject == NULL )
	{
		return E_POINTER;
	}

	*ppIDataObject = NULL;

	if( m_pScript )
	{
		// Let Script create data object
		return m_pScript->CreateDataObject( ppIDataObject );
	}

	return E_FAIL;
}


/////////////////////////////////////////////////////////////////////////////
// CScriptRef IDMUSProdNode::CanCut

HRESULT CScriptRef::CanCut( void )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
 
	return CanDelete();
}


/////////////////////////////////////////////////////////////////////////////
// CScriptRef IDMUSProdNode::CanCopy

HRESULT CScriptRef::CanCopy( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
 
	if( m_pScript )
	{
		return S_OK;	// Will copy the Script
	}

	return S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CScriptRef IDMUSProdNode::CanDelete

HRESULT CScriptRef::CanDelete( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pIParentNode != NULL );

	return m_pIParentNode->CanDeleteChildNode( this );
}


/////////////////////////////////////////////////////////////////////////////
// CScriptRef IDMUSProdNode::CanDeleteChildNode

HRESULT CScriptRef::CanDeleteChildNode( IDMUSProdNode* pIChildNode )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(pIChildNode);

	ASSERT( 0 );

	return E_NOTIMPL;	// Script Reference nodes do not have children
}


/////////////////////////////////////////////////////////////////////////////
// CScriptRef IDMUSProdNode::CanPasteFromData

HRESULT CScriptRef::CanPasteFromData( IDataObject* pIDataObject, BOOL* pfWillSetReference )
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

	// Create a new CDllJazzDataObject and see if it contains a Script
	CDllJazzDataObject* pDataObject = new CDllJazzDataObject();
	if( pDataObject == NULL )
	{
		return E_OUTOFMEMORY;
	}
	
	HRESULT hr = pDataObject->IsClipFormatAvailable( pIDataObject, theApp.m_pScriptComponent->m_cfScript );

	RELEASE( pDataObject );

	if( SUCCEEDED ( hr ) )
	{
		ASSERT( m_pIParentNode != NULL );

		// Let parent decide whether it wants to paste a Script
		return m_pIParentNode->CanChildPasteFromData( pIDataObject, this, pfWillSetReference );
	}

	if( m_pScript )
	{
		// Let Script decide what can be dropped
		return m_pScript->CanPasteFromData( pIDataObject, pfWillSetReference );
	}

	return S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CScriptRef IDMUSProdNode::PasteFromData

HRESULT CScriptRef::PasteFromData( IDataObject* pIDataObject )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pIDataObject == NULL )
	{
		return E_INVALIDARG;
	}

	// Create a new CDllJazzDataObject and see if it contains a Script
	CDllJazzDataObject* pDataObject = new CDllJazzDataObject();
	if( pDataObject == NULL )
	{
		return E_OUTOFMEMORY;
	}
	
	HRESULT hr = pDataObject->IsClipFormatAvailable( pIDataObject, theApp.m_pScriptComponent->m_cfScript );

	RELEASE( pDataObject );

	if( SUCCEEDED ( hr ) )
	{
		ASSERT( m_pIParentNode != NULL );

		// Let parent handle pasting of Script
		return m_pIParentNode->ChildPasteFromData( pIDataObject, this );
	}

	if( m_pScript )
	{
		// Let Script handle paste
		return m_pScript->PasteFromData( pIDataObject );
	}

	return E_FAIL;
}


/////////////////////////////////////////////////////////////////////////////
// CScriptRef IDMUSProdNode::CanChildPasteFromData

HRESULT CScriptRef::CanChildPasteFromData( IDataObject* pIDataObject, IDMUSProdNode* pIChildNode,
										  BOOL* pfWillSetReference )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(pIDataObject);
	UNREFERENCED_PARAMETER(pIChildNode);
	UNREFERENCED_PARAMETER(pfWillSetReference);

	return E_NOTIMPL;			// Script Reference nodes do not have children
}


/////////////////////////////////////////////////////////////////////////////
// CScriptRef IDMUSProdNode::ChildPasteFromData

HRESULT CScriptRef::ChildPasteFromData( IDataObject* pIDataObject, IDMUSProdNode* pIChildNode )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(pIDataObject);
	UNREFERENCED_PARAMETER(pIChildNode);

	return E_NOTIMPL;			// Script Reference nodes do not have children
}


/////////////////////////////////////////////////////////////////////////////
// CScriptRef IDMUSProdNode::GetObject

HRESULT CScriptRef::GetObject( REFCLSID rclsid, REFIID riid, void** ppvObject )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(rclsid);
	UNREFERENCED_PARAMETER(riid);
	UNREFERENCED_PARAMETER(ppvObject);

	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CScriptRef IDMUSProdReferenceNode implementation

/////////////////////////////////////////////////////////////////////////////
// CScriptRef IDMUSProdReferenceNode::GetReferencedFile

HRESULT CScriptRef::GetReferencedFile( IDMUSProdNode** ppIDocRootNode )
{
 	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( ppIDocRootNode == NULL )
	{
		return E_POINTER;
	}

	*ppIDocRootNode = NULL;

	if( m_pScript )
	{
		m_pScript->AddRef();
		*ppIDocRootNode = m_pScript;
		return S_OK;
	}

    return E_FAIL;
}


/////////////////////////////////////////////////////////////////////////////
// CScriptRef::IDMUSProdReferenceNode::SetReferencedFile

HRESULT CScriptRef::SetReferencedFile( IDMUSProdNode* pIDocRootNode )
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

		// Make sure method was passed a Script Node
		GUID guidNodeId;
		if( SUCCEEDED ( pIDocRootNode->GetNodeId ( &guidNodeId ) ) )
		{
			if( !( IsEqualGUID ( guidNodeId, GUID_ScriptNode ) ) )
			{
				return E_INVALIDARG;
			}
		}
	}

	// Release previous DocRoot Node
	RELEASE( m_pScript );

	// Link to new DocRoot Node
	if( pIDocRootNode )
	{
		m_pScript = (CDirectMusicScript *)pIDocRootNode;
		m_pScript->AddRef();
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CScriptRef Additional functions
