// FolderToolGraphs.cpp : implementation file
//

#include "stdafx.h"

#include "SongDesignerDLL.h"
#include "Song.h"
#include <ToolGraphDesigner.h>
#include <dmusicf.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CFolderToolGraphs constructor/destructor

CFolderToolGraphs::CFolderToolGraphs()
{
	m_pSong = NULL;
	m_pIDocRootNode = NULL;
	m_pIParentNode = NULL;
	m_fModified = FALSE;
}

CFolderToolGraphs::~CFolderToolGraphs()
{
	EmptyToolGraphList();
}


/////////////////////////////////////////////////////////////////////////////
// CFolderToolGraphs IUnknown implementation

HRESULT CFolderToolGraphs::QueryInterface( REFIID riid, LPVOID FAR* ppvObj )
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
		if( m_pSong )
		{
	        return m_pSong->QueryInterface( riid, ppvObj );
		}

		*ppvObj = NULL;
		return E_NOINTERFACE;
    }

    *ppvObj = NULL;
    return E_NOINTERFACE;
}

ULONG CFolderToolGraphs::AddRef()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pSong != NULL );

	AfxOleLockApp(); 

	return m_pSong->AddRef();
}

ULONG CFolderToolGraphs::Release()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pSong != NULL );

	AfxOleUnlockApp(); 

	return m_pSong->Release();
}


/////////////////////////////////////////////////////////////////////////////
// CFolderToolGraphs IDMUSProdNode implementation

/////////////////////////////////////////////////////////////////////////////
// CFolderToolGraphs IDMUSProdNode::GetNodeImageIndex

HRESULT CFolderToolGraphs::GetNodeImageIndex( short* pnFirstImage )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( theApp.m_pSongComponent != NULL );

	return( theApp.m_pSongComponent->GetFolderImageIndex(pnFirstImage) );
}


/////////////////////////////////////////////////////////////////////////////
// CFolderToolGraphs IDMUSProdNode::GetFirstChild

HRESULT CFolderToolGraphs::GetFirstChild( IDMUSProdNode** ppIFirstChildNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( ppIFirstChildNode == NULL )
	{
		return E_POINTER;
	}
	
	*ppIFirstChildNode = NULL;

	if( !m_lstToolGraphs.IsEmpty() )
	{
		IDMUSProdNode* pIToolGraph = m_lstToolGraphs.GetHead();
		
		if( pIToolGraph )
		{
			pIToolGraph->AddRef();
			*ppIFirstChildNode = pIToolGraph;
		}
	}

    return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CFolderToolGraphs IDMUSProdNode::GetNextChild

HRESULT CFolderToolGraphs::GetNextChild( IDMUSProdNode* pIChildNode, IDMUSProdNode** ppINextChildNode )
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

    POSITION pos = m_lstToolGraphs.GetHeadPosition();
    while( pos )
    {
        IDMUSProdNode* pIToolGraph = m_lstToolGraphs.GetNext( pos );
		if( pIToolGraph == pIChildNode )
		{
			if( pos )
			{
				pIToolGraph = m_lstToolGraphs.GetNext( pos );

				pIToolGraph->AddRef();
				*ppINextChildNode = pIToolGraph;
			}
			break;
		}
    }

    return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CFolderToolGraphs IDMUSProdNode::GetComponent

HRESULT CFolderToolGraphs::GetComponent( IDMUSProdComponent** ppIComponent )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( theApp.m_pSongComponent != NULL );

	return theApp.m_pSongComponent->QueryInterface( IID_IDMUSProdComponent, (void**)ppIComponent );
}


/////////////////////////////////////////////////////////////////////////////
// CFolderToolGraphs IDMUSProdNode::GetDocRootNode

HRESULT CFolderToolGraphs::GetDocRootNode( IDMUSProdNode** ppIDocRootNode )
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
// CFolderToolGraphs IDMUSProdNode::SetDocRootNode

HRESULT CFolderToolGraphs::SetDocRootNode( IDMUSProdNode* pIDocRootNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( pIDocRootNode != NULL );

	m_pIDocRootNode = pIDocRootNode;
//	m_pIDocRootNode->AddRef();		intentionally missing

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CFolderToolGraphs IDMUSProdNode::GetParentNode

HRESULT CFolderToolGraphs::GetParentNode( IDMUSProdNode** ppIParentNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pIParentNode != NULL );

	m_pIParentNode->AddRef();
	*ppIParentNode = m_pIParentNode;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CFolderToolGraphs IDMUSProdNode::SetParentNode

HRESULT CFolderToolGraphs::SetParentNode( IDMUSProdNode* pIParentNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( pIParentNode != NULL );

	m_pIParentNode = pIParentNode;
//	m_pIParentNode->AddRef();		intentionally missing

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CFolderToolGraphs IDMUSProdNode::GetNodeId

HRESULT CFolderToolGraphs::GetNodeId( GUID* pguid )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pguid == NULL )
	{
		return E_POINTER;
	}

	*pguid = GUID_Song_ToolGraphsFolderNode;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CFolderToolGraphs IDMUSProdNode::GetNodeName

HRESULT CFolderToolGraphs::GetNodeName( BSTR* pbstrName )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CString strName;
	TCHAR achBuffer[MID_BUFFER];

	if( ::LoadString(theApp.m_hInstance, IDS_TOOLGRAPHS_FOLDER_NAME, achBuffer, MID_BUFFER) )
	{
		strName = achBuffer;
	}

    *pbstrName = strName.AllocSysString();

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CFolderToolGraphs IDMUSProdNode::GetNodeNameMaxLength

HRESULT CFolderToolGraphs::GetNodeNameMaxLength( short* pnMaxLength )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	*pnMaxLength = -1;	// Can't rename a 'Source ToolGraphs' folder

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CFolderToolGraphs IDMUSProdNode::ValidateNodeName

HRESULT CFolderToolGraphs::ValidateNodeName( BSTR bstrName )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return E_NOTIMPL;	// Can't rename a 'Source ToolGraphs' folder
}


/////////////////////////////////////////////////////////////////////////////
// CFolderToolGraphs IDMUSProdNode::SetNodeName

HRESULT CFolderToolGraphs::SetNodeName( BSTR bstrName )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return E_NOTIMPL;	// Can't rename a 'Source ToolGraphs' folder
}


/////////////////////////////////////////////////////////////////////////////
// CFolderToolGraphs IDMUSProdNode::GetNodeListInfo

HRESULT CFolderToolGraphs::GetNodeListInfo( DMUSProdListInfo* pListInfo )
{
//	AFX_MANAGE_STATE( _afxModuleAddrThis );

	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CFolderToolGraphs IDMUSProdNode::GetEditorClsId

HRESULT CFolderToolGraphs::GetEditorClsId( CLSID* pClsId )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return E_NOTIMPL;	// Can't edit a 'Source ToolGraphs' folder
}


/////////////////////////////////////////////////////////////////////////////
// CFolderToolGraphs IDMUSProdNode::GetEditorTitle

HRESULT CFolderToolGraphs::GetEditorTitle( BSTR* pbstrTitle )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return E_NOTIMPL;	// Can't edit a 'Source ToolGraphs' folder
}


/////////////////////////////////////////////////////////////////////////////
// CFolderToolGraphs IDMUSProdNode::GetEditorWindow

HRESULT CFolderToolGraphs::GetEditorWindow( HWND* hWndEditor )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return E_NOTIMPL;	// Can't edit a 'Source ToolGraphs' folder
}


/////////////////////////////////////////////////////////////////////////////
// CFolderToolGraphs IDMUSProdNode::SetEditorWindow

HRESULT CFolderToolGraphs::SetEditorWindow( HWND hWndEditor )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return E_NOTIMPL;	// Can't edit a 'Source ToolGraphs' folder
}


/////////////////////////////////////////////////////////////////////////////
// CFolderToolGraphs IDMUSProdNode::UseOpenCloseImages

HRESULT CFolderToolGraphs::UseOpenCloseImages( BOOL* pfUseOpenCloseImages )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	*pfUseOpenCloseImages = TRUE;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CFolderToolGraphs IDMUSProdNode::GetRightClickMenuId

HRESULT CFolderToolGraphs::GetRightClickMenuId( HINSTANCE* phInstance, UINT* pnMenuId )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	*phInstance = theApp.m_hInstance;
	*pnMenuId   = IDM_FOLDER_TOOLGRAPHS_NODE_RMENU;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CFolderToolGraphs IDMUSProdNode::OnRightClickMenuInit

HRESULT CFolderToolGraphs::OnRightClickMenuInit( HMENU hMenu )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CMenu menu;

	if( menu.Attach(hMenu) )
	{
		// IDM_PASTE
		{
			IDataObject* pIDataObject;
			BOOL fWillSetReference;

			menu.EnableMenuItem( ID_EDIT_PASTE, (MF_GRAYED | MF_BYCOMMAND) );

			if( SUCCEEDED ( ::OleGetClipboard( &pIDataObject ) ) )
			{
				if( CanPasteFromData( pIDataObject, &fWillSetReference ) == S_OK )
				{
					menu.EnableMenuItem( ID_EDIT_PASTE, (MF_ENABLED | MF_BYCOMMAND) );
				}
				
				pIDataObject->Release();
			}
		}

		// IDM_PROPERTIES
		IDMUSProdPropPageObject* pIPageObject;
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
// CFolderToolGraphs IDMUSProdNode::OnRightClickMenuSelect

HRESULT CFolderToolGraphs::OnRightClickMenuSelect( long lCommandId )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pSong != NULL );

	HRESULT hr = E_FAIL;

	switch( lCommandId )
	{
		case IDM_NEW_SOURCE_TOOLGRAPH:
			hr = InsertChildNode( NULL );
			break;

		case ID_EDIT_PASTE:
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
// CFolderToolGraphs IDMUSProdNode::DeleteChildNode

HRESULT CFolderToolGraphs::DeleteChildNode( IDMUSProdNode* pIChildNode, BOOL fPromptUser )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( pIChildNode != NULL );
	ASSERT( m_pSong != NULL );
	ASSERT( theApp.m_pSongComponent != NULL );
	ASSERT( theApp.m_pSongComponent->m_pIFramework8 != NULL );

	CWaitCursor wait;

	if( pIChildNode == NULL )
	{
		return E_INVALIDARG;
	}

	// Remove node from Project Tree
	if( theApp.m_pSongComponent->m_pIFramework8->RemoveNode( pIChildNode, fPromptUser ) == S_FALSE )
	{
		return E_FAIL;
	}

	// Save undo state
//	m_pSong->m_pINodeBeingDeleted = pIChildNode;
//	m_pSong->m_pUndoMgr->SaveState( m_pSong, theApp.m_hInstance, IDS_UNDO_DELETE_PATTERN );
//	m_pSong->m_pINodeBeingDeleted = NULL;

	// Remove from ToolGraph list
	RemoveToolGraph( (IDMUSProdNode *)pIChildNode );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CFolderToolGraphs IDMUSProdNode::InsertChildNode

HRESULT CFolderToolGraphs::InsertChildNode( IDMUSProdNode* pIChildNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pSong != NULL );
	ASSERT( theApp.m_pSongComponent != NULL );
	ASSERT( theApp.m_pSongComponent->m_pIFramework8 != NULL );

	IDMUSProdNode* pIToolGraph = (IDMUSProdNode *)pIChildNode;

	bool fOpenEditor = false;

	if( pIToolGraph )
	{
		// Make sure we received a ToolGraph node
		GUID guidNodeId;
		HRESULT hr = pIChildNode->GetNodeId( &guidNodeId );
		if( SUCCEEDED ( hr ) )
		{
			if( ::IsEqualGUID( guidNodeId, GUID_ToolGraphNode ) == FALSE )
			{
				hr = E_FAIL;
			}
		}
		if( FAILED( hr ) )
		{
			return hr;
		}

		// Add to ToolGraph list
		InsertToolGraph( pIToolGraph );
	}
	else
	{
		// Create the ToolGraph
		IDMUSProdDocType* pIDocType;
		HRESULT hr = theApp.m_pSongComponent->m_pIFramework8->FindDocTypeByNodeId( GUID_ToolGraphNode, &pIDocType );
		if( SUCCEEDED ( hr ) ) 
		{
			hr = pIDocType->AllocNode( GUID_ToolGraphNode, &pIToolGraph );
			if( SUCCEEDED ( hr ) )
			{
				// Add to ToolGraph list
				fOpenEditor = true;
				InsertToolGraph( pIToolGraph );
				pIToolGraph->Release();
			}

			RELEASE( pIDocType );
		}
	}

	// Set root and parent node of ALL children
	ASSERT( m_pIDocRootNode != NULL );
	theApp.SetNodePointers( pIToolGraph, m_pIDocRootNode, (IDMUSProdNode *)this );

	// Add node to Project Tree
	HRESULT hr = theApp.m_pSongComponent->m_pIFramework8->AddNode( pIToolGraph, (IDMUSProdNode *)this );
	if( SUCCEEDED ( hr ) )
	{
		if( fOpenEditor )
		{
			theApp.m_pSongComponent->m_pIFramework8->OpenEditor( pIToolGraph );
		}
	}
	else
	{
		DeleteChildNode( pIToolGraph, FALSE );
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CFolderToolGraphs IDMUSProdNode::DeleteNode

HRESULT CFolderToolGraphs::DeleteNode( BOOL fPromptUser )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return E_NOTIMPL;	// Can't delete a 'Source ToolGraphs' folder
}


/////////////////////////////////////////////////////////////////////////////
// CFolderToolGraphs IDMUSProdNode::OnNodeSelChanged

HRESULT CFolderToolGraphs::OnNodeSelChanged( BOOL fSelected )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CFolderToolGraphs IDMUSProdNode::CreateDataObject

HRESULT CFolderToolGraphs::CreateDataObject( IDataObject** ppIDataObject )
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
// CFolderToolGraphs IDMUSProdNode::CanCut

HRESULT CFolderToolGraphs::CanCut( void )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
 
	return S_FALSE;		// Can't remove 'Source ToolGraphs' folder from Song
}


/////////////////////////////////////////////////////////////////////////////
// CFolderToolGraphs IDMUSProdNode::CanCopy

HRESULT CFolderToolGraphs::CanCopy( void )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
 
	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CFolderToolGraphs IDMUSProdNode::CanDelete

HRESULT CFolderToolGraphs::CanDelete( void )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return S_FALSE;		// Can't delete 'Source ToolGraphs' folder from Song
}


/////////////////////////////////////////////////////////////////////////////
// CFolderToolGraphs IDMUSProdNode::CanDeleteChildNode

HRESULT CFolderToolGraphs::CanDeleteChildNode( IDMUSProdNode* pIChildNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Make sure node is in ToolGraph list
	POSITION pos = m_lstToolGraphs.Find( pIChildNode );
	if( pos == NULL )
	{
		return E_FAIL;
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CFolderToolGraphs IDMUSProdNode::CanPasteFromData

HRESULT CFolderToolGraphs::CanPasteFromData( IDataObject* pIDataObject, BOOL* pfWillSetReference )
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
	
	if( SUCCEEDED (	pDataObject->IsClipFormatAvailable( pIDataObject, theApp.m_pSongComponent->m_cfGraph ) ) )
	{
		hr = S_OK;
	}

	RELEASE( pDataObject );
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CFolderToolGraphs IDMUSProdNode::PasteFromData

HRESULT CFolderToolGraphs::PasteFromData( IDataObject* pIDataObject )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pSong != NULL );

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

	HRESULT hr = E_FAIL;
	
	if( SUCCEEDED ( pDataObject->IsClipFormatAvailable( pIDataObject, theApp.m_pSongComponent->m_cfGraph ) ) )
	{
		hr = PasteCF_GRAPH( pDataObject, pIDataObject );
	}

	RELEASE( pDataObject );
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CFolderToolGraphs IDMUSProdNode::CanChildPasteFromData

HRESULT CFolderToolGraphs::CanChildPasteFromData( IDataObject* pIDataObject, IDMUSProdNode* pIChildNode,
											   BOOL* pfWillSetReference )
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
	
	if( SUCCEEDED (	pDataObject->IsClipFormatAvailable( pIDataObject, theApp.m_pSongComponent->m_cfGraph ) ) )
	{
		hr = S_OK;
	}

	RELEASE( pDataObject );
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CFolderToolGraphs IDMUSProdNode::ChildPasteFromData

HRESULT CFolderToolGraphs::ChildPasteFromData( IDataObject* pIDataObject, IDMUSProdNode* pIChildNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pSong != NULL );

	if( pIDataObject == NULL 
	||  pIChildNode == NULL )
	{
		return E_INVALIDARG;
	}

	// Create a new CDllJazzDataObject and see if it can read the data object's format.
	CDllJazzDataObject* pDataObject = new CDllJazzDataObject();
	if( pDataObject == NULL )
	{
		return E_OUTOFMEMORY;
	}

	HRESULT hr = E_FAIL;
	
	if( SUCCEEDED ( pDataObject->IsClipFormatAvailable( pIDataObject, theApp.m_pSongComponent->m_cfGraph ) ) )
	{
		hr = PasteCF_GRAPH( pDataObject, pIDataObject );
	}

	RELEASE( pDataObject );
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CFolderToolGraphs IDMUSProdNode::GetObject

HRESULT CFolderToolGraphs::GetObject( REFCLSID rclsid, REFIID riid, void** ppvObject )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CFolderToolGraphs::PasteCF_GRAPH

HRESULT CFolderToolGraphs::PasteCF_GRAPH( CDllJazzDataObject* pDataObject, IDataObject* pIDataObject )
{
	IStream* pIStream;
	HRESULT hr = E_FAIL;

	if( SUCCEEDED (	pDataObject->AttemptRead( pIDataObject, theApp.m_pSongComponent->m_cfGraph, &pIStream ) ) )
	{
		LARGE_INTEGER liTemp;

		// Seek to beginning of stream
		liTemp.QuadPart = 0;
		pIStream->Seek( liTemp, STREAM_SEEK_SET, NULL );

		// Create and load a new ToolGraph
		IDMUSProdRIFFExt* pIRIFFExt;
		IDMUSProdNode* pINode;

		if( SUCCEEDED ( theApp.m_pSongComponent->m_pIToolGraphComponent->QueryInterface( IID_IDMUSProdRIFFExt, (void**)&pIRIFFExt ) ) )
		{
			hr = pIRIFFExt->LoadRIFFChunk( pIStream, &pINode );
			if( SUCCEEDED ( hr ) )
			{
				InsertChildNode( pINode );
				RELEASE( pINode );
			}

			RELEASE( pIRIFFExt );
		}

		RELEASE( pIStream );
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CFolderToolGraphs::InsertToolGraph

HRESULT CFolderToolGraphs::InsertToolGraph( IDMUSProdNode* pIToolGraphNodeToInsert )
{
	if( pIToolGraphNodeToInsert == NULL )
	{
		ASSERT( 0 );
		return E_INVALIDARG;
	}

	pIToolGraphNodeToInsert->AddRef();

	// Add to ToolGraph list
	m_lstToolGraphs.AddTail( pIToolGraphNodeToInsert );

	// Sync changes
	m_pSong->SetModified( TRUE );
	m_pSong->SyncChanges( SC_EDITOR | SC_DIRECTMUSIC,
						  SSE_TOOLGRAPH_LIST,
						  NULL );
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CFolderToolGraphs::RemoveToolGraph

HRESULT CFolderToolGraphs::RemoveToolGraph( IDMUSProdNode* pIToolGraphNode )
{
	if( pIToolGraphNode == NULL )
	{
		ASSERT( 0 );
		return E_INVALIDARG;
	}

	// Make sure other VirtualSegments do not point to pIToolGraphNode
	m_pSong->RemoveToolGraphReferences( pIToolGraphNode );

	// Remove from ToolGraph list
	POSITION pos = m_lstToolGraphs.Find( pIToolGraphNode );
	if( pos )
	{
		m_lstToolGraphs.RemoveAt( pos );

		// Sync changes
		m_pSong->SetModified( TRUE );
		m_pSong->SyncChanges( SC_EDITOR | SC_DIRECTMUSIC,
							  SSE_TOOLGRAPH_LIST,
							  NULL );
		
		RELEASE( pIToolGraphNode );
		return S_OK;
	}

	return E_FAIL;
}


/////////////////////////////////////////////////////////////////////////////
// CFolderToolGraphs SaveToolGraphs
    
HRESULT CFolderToolGraphs::SaveToolGraphs( IDMUSProdRIFFStream* pIRiffStream, BOOL fClearDirty )
{
	IStream* pIStream;
	HRESULT hr = S_OK;
    MMCKINFO ckMain;
	POSITION pos;

	if( m_lstToolGraphs.IsEmpty() )
	{
		return S_OK;
	}

    pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	// Write DMUS_FOURCC_TOOLGRAPHS_LIST header
	ckMain.fccType = DMUS_FOURCC_TOOLGRAPHS_LIST;
	if( pIRiffStream->CreateChunk(&ckMain, MMIO_CREATELIST) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Save each ToolGraph
	pos = m_lstToolGraphs.GetHeadPosition();
	while( pos )
	{
		IDMUSProdNode* pIToolGraphNode = m_lstToolGraphs.GetNext( pos );

		IPersistStream* pIPersistStream;
		hr = pIToolGraphNode->QueryInterface( IID_IPersistStream, (void**)&pIPersistStream );
		if( FAILED( hr ) )
		{
			goto ON_ERROR;
		}
		hr = pIPersistStream->Save( pIStream, fClearDirty );
		RELEASE( pIPersistStream );
		if( FAILED( hr ) )
		{
			goto ON_ERROR;
		}
	}

	if( pIRiffStream->Ascend(&ckMain, 0) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

ON_ERROR:
    RELEASE( pIStream );
    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CFolderToolGraphs LoadToolGraphs
    
HRESULT CFolderToolGraphs::LoadToolGraphs( IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain )
{
	IStream* pIStream;
	HRESULT hr = S_OK;
    MMCKINFO ck;
	DWORD dwPos;

    pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

    dwPos = StreamTell( pIStream );

	while( pIRiffStream->Descend( &ck, pckMain, 0 ) == 0 )
	{
		switch( ck.ckid )
		{
			case FOURCC_RIFF:
				switch( ck.fccType )
				{
					case DMUS_FOURCC_TOOLGRAPH_FORM: 
					{
						IDMUSProdRIFFExt* pIRIFFExt;
						IDMUSProdNode* pIToolGraphNode;

						hr = theApp.m_pSongComponent->m_pIToolGraphComponent->QueryInterface( IID_IDMUSProdRIFFExt, (void**)&pIRIFFExt );
						if( FAILED( hr ) )
						{
			                goto ON_ERROR;
						}
					    StreamSeek( pIStream, dwPos, STREAM_SEEK_SET );
						hr = pIRIFFExt->LoadRIFFChunk( pIStream, &pIToolGraphNode );
						RELEASE( pIRIFFExt );
						if( FAILED( hr ) )
						{
			                goto ON_ERROR;
						}
						m_lstToolGraphs.AddTail( pIToolGraphNode );
						break;
					}
				}
		}

		pIRiffStream->Ascend( &ck, 0 );
	    dwPos = StreamTell( pIStream );
	}

ON_ERROR:
	RELEASE( pIStream );
	return hr;
}


///////////////////////////////////////////////////////////////////////////
// CFolderToolGraphs::EmptyToolGraphList

void CFolderToolGraphs::EmptyToolGraphList( void )
{
	IDMUSProdNode* pIToolGraphNode;

	while( !m_lstToolGraphs.IsEmpty() )
	{
		pIToolGraphNode = m_lstToolGraphs.RemoveHead();
		RELEASE( pIToolGraphNode );
	}
}


///////////////////////////////////////////////////////////////////////////
// CFolderToolGraphs::OnUpdate	(handles Song's IDMUSProdNotifSink notifications)

HRESULT CFolderToolGraphs::OnUpdate( IDMUSProdNode* pIDocRootNode, GUID guidUpdateType, VOID* pData )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	IDMUSProdNode* pIToolGraphNode;

	POSITION pos = m_lstToolGraphs.GetHeadPosition();
	while( pos )
	{
		pIToolGraphNode = m_lstToolGraphs.GetNext( pos );

		if( pIToolGraphNode == pIDocRootNode )
		{
			// TOOLGRAPH_NameChange
			if( ::IsEqualGUID(guidUpdateType, TOOLGRAPH_NameChange ) )
			{
				m_pSong->SetModified( TRUE );
				m_pSong->SyncChanges( SC_EDITOR,
									  SSE_TOOLGRAPH_LIST,
									  NULL );
				return S_OK;
			}

			// All other notifications
			else
			{
				m_pSong->SetModified( TRUE );
				return S_OK;
			}
		}
	}

	return E_INVALIDARG;
}
