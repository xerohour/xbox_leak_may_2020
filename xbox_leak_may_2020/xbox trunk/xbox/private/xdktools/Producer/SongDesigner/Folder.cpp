// Folder.cpp : implementation file
//

#include "stdafx.h"

#include "SongDesignerDLL.h"
#include "Song.h"
#include "SongCtl.h"
#include <SegmentDesigner.h>
#include <SegmentGuids.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CFolder constructor/destructor

CFolder::CFolder()
{
	m_pSong = NULL;
	m_pIDocRootNode = NULL;
	m_pIParentNode = NULL;
}

CFolder::~CFolder()
{
	EmptySourceSegmentList();
}


/////////////////////////////////////////////////////////////////////////////
// CFolder IUnknown implementation

HRESULT CFolder::QueryInterface( REFIID riid, LPVOID FAR* ppvObj )
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

ULONG CFolder::AddRef()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pSong != NULL );

	AfxOleLockApp(); 

	return m_pSong->AddRef();
}

ULONG CFolder::Release()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pSong != NULL );

	AfxOleUnlockApp(); 

	return m_pSong->Release();
}


/////////////////////////////////////////////////////////////////////////////
// CFolder IDMUSProdNode implementation

/////////////////////////////////////////////////////////////////////////////
// CFolder IDMUSProdNode::GetNodeImageIndex

HRESULT CFolder::GetNodeImageIndex( short* pnFirstImage )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( theApp.m_pSongComponent != NULL );

	return( theApp.m_pSongComponent->GetFolderImageIndex(pnFirstImage) );
}


/////////////////////////////////////////////////////////////////////////////
// CFolder IDMUSProdNode::GetFirstChild

HRESULT CFolder::GetFirstChild( IDMUSProdNode** ppIFirstChildNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( ppIFirstChildNode == NULL )
	{
		return E_POINTER;
	}
	
	*ppIFirstChildNode = NULL;

	if( !m_lstSegments.IsEmpty() )
	{
		CSourceSegment* pSourceSegment = m_lstSegments.GetHead();
		
		if( pSourceSegment )
		{
			pSourceSegment->AddRef();
			*ppIFirstChildNode = (IDMUSProdNode *)pSourceSegment;
		}
	}

    return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CFolder IDMUSProdNode::GetNextChild

HRESULT CFolder::GetNextChild( IDMUSProdNode* pIChildNode, IDMUSProdNode** ppINextChildNode )
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

	CSourceSegment* pSourceSegment;

    POSITION pos = m_lstSegments.GetHeadPosition();

    while( pos )
    {
        pSourceSegment = m_lstSegments.GetNext( pos );
		if( pIChildNode == (IDMUSProdNode *)pSourceSegment )
		{
			if( pos )
			{
				pSourceSegment = m_lstSegments.GetNext( pos );

				pSourceSegment->AddRef();
				*ppINextChildNode = (IDMUSProdNode *)pSourceSegment;
			}
			break;
		}
    }

    return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CFolder IDMUSProdNode::GetComponent

HRESULT CFolder::GetComponent( IDMUSProdComponent** ppIComponent )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( theApp.m_pSongComponent != NULL );

	return theApp.m_pSongComponent->QueryInterface( IID_IDMUSProdComponent, (void**)ppIComponent );
}


/////////////////////////////////////////////////////////////////////////////
// CFolder IDMUSProdNode::GetDocRootNode

HRESULT CFolder::GetDocRootNode( IDMUSProdNode** ppIDocRootNode )
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
// CFolder IDMUSProdNode::SetDocRootNode

HRESULT CFolder::SetDocRootNode( IDMUSProdNode* pIDocRootNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( pIDocRootNode != NULL );

	m_pIDocRootNode = pIDocRootNode;
//	m_pIDocRootNode->AddRef();		intentionally missing

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CFolder IDMUSProdNode::GetParentNode

HRESULT CFolder::GetParentNode( IDMUSProdNode** ppIParentNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pIParentNode != NULL );

	m_pIParentNode->AddRef();
	*ppIParentNode = m_pIParentNode;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CFolder IDMUSProdNode::SetParentNode

HRESULT CFolder::SetParentNode( IDMUSProdNode* pIParentNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( pIParentNode != NULL );

	m_pIParentNode = pIParentNode;
//	m_pIParentNode->AddRef();		intentionally missing

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CFolder IDMUSProdNode::GetNodeId

HRESULT CFolder::GetNodeId( GUID* pguid )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pguid == NULL )
	{
		return E_POINTER;
	}

	*pguid = m_guidTypeNode;		// GUID_SegmentEmbedFolderNode
									// GUID_SegmentRefFolderNode
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CFolder IDMUSProdNode::GetNodeName

HRESULT CFolder::GetNodeName( BSTR* pbstrName )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    *pbstrName = m_strName.AllocSysString();

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CFolder IDMUSProdNode::GetNodeNameMaxLength

HRESULT CFolder::GetNodeNameMaxLength( short* pnMaxLength )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	*pnMaxLength = -1;	// Can't rename a folder

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CFolder IDMUSProdNode::ValidateNodeName

HRESULT CFolder::ValidateNodeName( BSTR bstrName )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(bstrName);

	return E_NOTIMPL;	// Can't rename a folder
}


/////////////////////////////////////////////////////////////////////////////
// CFolder IDMUSProdNode::SetNodeName

HRESULT CFolder::SetNodeName( BSTR bstrName )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(bstrName);

	return E_NOTIMPL;	// Can't rename a folder
}


/////////////////////////////////////////////////////////////////////////////
// CFolder IDMUSProdNode::GetNodeListInfo

HRESULT CFolder::GetNodeListInfo( DMUSProdListInfo* pListInfo )
{
//	AFX_MANAGE_STATE( _afxModuleAddrThis );
	UNREFERENCED_PARAMETER(pListInfo);

	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CFolder IDMUSProdNode::GetEditorClsId

HRESULT CFolder::GetEditorClsId( CLSID* pClsId )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(pClsId);

	return E_NOTIMPL;	// Can't edit a folder
}


/////////////////////////////////////////////////////////////////////////////
// CFolder IDMUSProdNode::GetEditorTitle

HRESULT CFolder::GetEditorTitle( BSTR* pbstrTitle )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(pbstrTitle);

	return E_NOTIMPL;	// Can't edit a folder
}


/////////////////////////////////////////////////////////////////////////////
// CFolder IDMUSProdNode::GetEditorWindow

HRESULT CFolder::GetEditorWindow( HWND* hWndEditor )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(hWndEditor);

	return E_NOTIMPL;	// Can't edit a folder
}


/////////////////////////////////////////////////////////////////////////////
// CFolder IDMUSProdNode::SetEditorWindow

HRESULT CFolder::SetEditorWindow( HWND hWndEditor )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(hWndEditor);

	return E_NOTIMPL;	// Can't edit a folder
}


/////////////////////////////////////////////////////////////////////////////
// CFolder IDMUSProdNode::UseOpenCloseImages

HRESULT CFolder::UseOpenCloseImages( BOOL* pfUseOpenCloseImages )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	*pfUseOpenCloseImages = TRUE;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CFolder IDMUSProdNode::GetRightClickMenuId

HRESULT CFolder::GetRightClickMenuId( HINSTANCE* phInstance, UINT* pnMenuId )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	*phInstance = theApp.m_hInstance;
	*pnMenuId   = IDM_FOLDER_NODE_RMENU;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CFolder IDMUSProdNode::OnRightClickMenuInit

HRESULT CFolder::OnRightClickMenuInit( HMENU hMenu )
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
// CFolder IDMUSProdNode::OnRightClickMenuSelect

HRESULT CFolder::OnRightClickMenuSelect( long lCommandId )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pSong != NULL );

	HRESULT hr = E_FAIL;

	switch( lCommandId )
	{
		case IDM_NEW_SOURCE_SEGMENT:
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
// CFolder IDMUSProdNode::DeleteChildNode

HRESULT CFolder::DeleteChildNode( IDMUSProdNode* pIChildNode, BOOL fPromptUser )
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

	// Make sure we received a SourceSegment node
	GUID guidNodeId;
	HRESULT hr = pIChildNode->GetNodeId( &guidNodeId );
	if( SUCCEEDED ( hr ) )
	{
		if( ::IsEqualGUID( guidNodeId, GUID_Song_SourceSegmentNode ) == FALSE )
		{
			hr = E_FAIL;
		}
	}
	if( FAILED( hr ) )
	{
		return hr;
	}
	CSourceSegment* pSourceSegment = (CSourceSegment *)pIChildNode;

	// Are you sure?
	if( fPromptUser )
	{
		CString strName;
		ASSERT( pSourceSegment->m_FileRef.pIRefNode != NULL );
		if( pSourceSegment->m_FileRef.pIRefNode == NULL )
		{
			TCHAR achNoObject[MID_BUFFER];
			::LoadString( theApp.m_hInstance, IDS_EMPTY_TEXT, achNoObject, MID_BUFFER );
			strName = achNoObject;
		}
		else
		{
			BSTR bstrName;
			if( SUCCEEDED ( pSourceSegment->m_FileRef.pIRefNode->GetNodeName( &bstrName ) ) )
			{
				strName = bstrName;
				::SysFreeString( bstrName );
			}
		}
		CString strPrompt;
		AfxFormatString1( strPrompt, IDS_DELETE_NODE_PROMPT, strName );
		if( AfxMessageBox( strPrompt, MB_OKCANCEL ) != IDOK )
		{
			return E_FAIL;
		}
	}

	// Remove node from Project Tree
	if( theApp.m_pSongComponent->m_pIFramework8->RemoveNode( pIChildNode, FALSE ) == S_FALSE )
	{
		return E_FAIL;
	}

	// Save undo state
//	m_pSong->m_pINodeBeingDeleted = pIChildNode;
//	m_pSong->m_pUndoMgr->SaveState( m_pSong, theApp.m_hInstance, IDS_UNDO_DELETE_BAND );
//	m_pSong->m_pINodeBeingDeleted = NULL;

	// Remove from Segment list
	RemoveSourceSegment( pSourceSegment );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CFolder IDMUSProdNode::InsertChildNode

HRESULT CFolder::InsertChildNode( IDMUSProdNode* pIChildNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pSong != NULL );
	ASSERT( theApp.m_pSongComponent != NULL );
	ASSERT( theApp.m_pSongComponent->m_pIFramework8 != NULL );

	HRESULT hr = E_FAIL;

	if( pIChildNode )
	{
		// Make sure we received a SourceSegment node
		GUID guidNodeId;
		hr = pIChildNode->GetNodeId( &guidNodeId );
		if( SUCCEEDED ( hr ) )
		{
			if( ::IsEqualGUID( guidNodeId, GUID_Song_SourceSegmentNode ) )
			{
				// Add to SourceSegment list
				InsertSourceSegment( (CSourceSegment *)pIChildNode );
			}
			else
			{
				hr = E_FAIL;
			}
		}
	}
	else
	{
		// Create the Segment
		IDMUSProdDocType* pIDocType;
		hr = theApp.m_pSongComponent->m_pIFramework8->FindDocTypeByNodeId( GUID_SegmentNode, &pIDocType );
		if( SUCCEEDED ( hr ) ) 
		{
			IDMUSProdNode* pIDocRootNode;
			hr = theApp.m_pSongComponent->m_pIFramework8->CreateNewFile( pIDocType, m_pSong, &pIDocRootNode );
			if( SUCCEEDED ( hr ) )
			{
				CSourceSegment* pSourceSegment = new CSourceSegment( m_pSong );
				if( pSourceSegment )
				{
					hr = pSourceSegment->SetFileReference( pIDocRootNode );
					if( SUCCEEDED ( hr ) )
					{
						// Add to SourceSegment list
						InsertSourceSegment( pSourceSegment );
						pSourceSegment->Release();

						pIChildNode = (IDMUSProdNode *)pSourceSegment;
					}
				}
				else
				{
					hr = E_OUTOFMEMORY;
				}

				RELEASE( pIDocRootNode );
			}

			RELEASE( pIDocType );
		}
	}

	if( SUCCEEDED ( hr ) )
	{
		// Set root and parent node of ALL children
		ASSERT( pIChildNode != NULL );
		ASSERT( m_pIDocRootNode != NULL );
		theApp.SetNodePointers( pIChildNode, m_pIDocRootNode, (IDMUSProdNode *)this );

		// Add node to Project Tree
		hr = theApp.m_pSongComponent->m_pIFramework8->AddNode( pIChildNode, (IDMUSProdNode *)this );
		if( FAILED ( hr ) )
		{
			DeleteChildNode( pIChildNode, FALSE );
		}
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CFolder IDMUSProdNode::DeleteNode

HRESULT CFolder::DeleteNode( BOOL fPromptUser )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(fPromptUser);

	return E_NOTIMPL;	// Can't delete a folder
}


/////////////////////////////////////////////////////////////////////////////
// CFolder IDMUSProdNode::OnNodeSelChanged

HRESULT CFolder::OnNodeSelChanged( BOOL fSelected )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(fSelected);

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CFolder IDMUSProdNode::CreateDataObject

HRESULT CFolder::CreateDataObject( IDataObject** ppIDataObject )
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
// CFolder IDMUSProdNode::CanCut

HRESULT CFolder::CanCut( void )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
 
	return S_FALSE;		// Can't remove a folder
}


/////////////////////////////////////////////////////////////////////////////
// CFolder IDMUSProdNode::CanCopy

HRESULT CFolder::CanCopy( void )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
 
	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CFolder IDMUSProdNode::CanDelete

HRESULT CFolder::CanDelete( void )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return S_FALSE;		// Can't delete a folder
}


/////////////////////////////////////////////////////////////////////////////
// CFolder IDMUSProdNode::CanDeleteChildNode

HRESULT CFolder::CanDeleteChildNode( IDMUSProdNode* pIChildNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Make sure we received a SourceSegment node
	GUID guidNodeId;
	HRESULT hr = pIChildNode->GetNodeId( &guidNodeId );
	if( SUCCEEDED ( hr ) )
	{
		if( ::IsEqualGUID( guidNodeId, GUID_Song_SourceSegmentNode ) == FALSE )
		{
			ASSERT( 0 );
			hr = E_FAIL;
		}
	}
	if( FAILED( hr ) )
	{
		return hr;
	}

	// Make sure node is in Segment list
	POSITION pos = m_lstSegments.Find( (CSourceSegment *)pIChildNode );
	if( pos == NULL )
	{
		return E_FAIL;
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CFolder IDMUSProdNode::CanPasteFromData

HRESULT CFolder::CanPasteFromData( IDataObject* pIDataObject, BOOL* pfWillSetReference )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pSong != NULL );
	ASSERT( theApp.m_pSongComponent->m_pIFramework8 != NULL );

	if( pfWillSetReference == NULL )
	{
		return E_POINTER;
	}

	*pfWillSetReference = TRUE;

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
	
	if( SUCCEEDED (	pDataObject->IsClipFormatAvailable( pIDataObject, theApp.m_pSongComponent->m_cfSegment ) ) )
	{
		IDMUSProdNode* pIDocRootNode;

		if( SUCCEEDED ( theApp.m_pSongComponent->m_pIFramework8->GetDocRootNodeFromData( pIDataObject, &pIDocRootNode ) ) )
		{
			if( IsDocRootInFolder( pIDocRootNode ) == FALSE )
			{
				hr = S_OK;
			}

			RELEASE( pIDocRootNode );
		}
	}

	RELEASE( pDataObject );
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CFolder IDMUSProdNode::PasteFromData

HRESULT CFolder::PasteFromData( IDataObject* pIDataObject )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pSong != NULL );
	ASSERT( theApp.m_pSongComponent->m_pIFramework8 != NULL );

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

	HRESULT hr;

	hr = pDataObject->IsClipFormatAvailable( pIDataObject, theApp.m_pSongComponent->m_cfSegment );
	if( SUCCEEDED ( hr ) )
	{
		IDMUSProdNode* pIDocRootNode;
		hr = theApp.m_pSongComponent->m_pIFramework8->GetDocRootNodeFromData( pIDataObject, &pIDocRootNode );
		if( SUCCEEDED ( hr ) )
		{
			CSourceSegment* pSourceSegment = NULL;

			// If DocRoot is in the other folder - remove it
			if( ::IsEqualGUID( m_guidTypeNode, GUID_Song_SegmentEmbedFolderNode ) ) 
			{
				CFolder* pFolderReference = m_pSong->GetSegmentRefFolder();
				pSourceSegment = pFolderReference->GetSourceSegmentFromDocRoot( pIDocRootNode );
				if( pSourceSegment )
				{
					pSourceSegment->AddRef();
					m_pSong->m_fChangingFolder = true;
					pFolderReference->DeleteChildNode( pSourceSegment, FALSE );
					m_pSong->m_fChangingFolder = false;
				}
			}
			else if( ::IsEqualGUID( m_guidTypeNode, GUID_Song_SegmentRefFolderNode ) ) 
			{
				CFolder* pFolderEmbed = m_pSong->GetSegmentEmbedFolder();
				pSourceSegment = pFolderEmbed->GetSourceSegmentFromDocRoot( pIDocRootNode );
				if( pSourceSegment )
				{
					pSourceSegment->AddRef();
					m_pSong->m_fChangingFolder = true;
					pFolderEmbed->DeleteChildNode( pSourceSegment, FALSE );
					m_pSong->m_fChangingFolder = false;
				}
			}
			else
			{
				ASSERT( 0 );
			}
			
			// Make sure a SourceSegment exists for the DocRoot
			if( pSourceSegment == NULL )
			{
				pSourceSegment = new CSourceSegment( m_pSong );
				if( pSourceSegment )
				{
					if( FAILED ( pSourceSegment->SetFileReference( pIDocRootNode ) ) )
					{
						RELEASE( pSourceSegment );
					}
				}
			}

			// Add the DocRoot to this folder
			if( pSourceSegment )
			{
				hr = InsertChildNode( pSourceSegment );
				if( FAILED ( hr ) )
				{
					pSourceSegment->AddRef();
					DeleteChildNode( pSourceSegment, FALSE );
				}
			}
			else
			{
				hr = E_FAIL;
			}

			RELEASE( pSourceSegment );
			RELEASE( pIDocRootNode );
		}
	}

	RELEASE( pDataObject );
	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CFolder IDMUSProdNode::CanChildPasteFromData

HRESULT CFolder::CanChildPasteFromData( IDataObject* pIDataObject, IDMUSProdNode* pIChildNode,
										BOOL *pfWillSetReference )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(pIChildNode);

	return CanPasteFromData( pIDataObject, pfWillSetReference );
}


/////////////////////////////////////////////////////////////////////////////
// CFolder IDMUSProdNode::ChildPasteFromData

HRESULT CFolder::ChildPasteFromData( IDataObject* pIDataObject, IDMUSProdNode* pIChildNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(pIChildNode);

	return PasteFromData( pIDataObject );
}


/////////////////////////////////////////////////////////////////////////////
// CFolder IDMUSProdNode::GetObject

HRESULT CFolder::GetObject( REFCLSID rclsid, REFIID riid, void** ppvObject )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(rclsid);
	UNREFERENCED_PARAMETER(riid);
	UNREFERENCED_PARAMETER(ppvObject);

	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CFolder SaveSegments
    
HRESULT CFolder::SaveSegments( IDMUSProdRIFFStream* pIRiffStream )
{
	HRESULT hr = S_OK;

	BOOL fReference = TRUE;
	if( ::IsEqualGUID( m_guidTypeNode, GUID_Song_SegmentEmbedFolderNode ) ) 
	{
		fReference = FALSE;
	}

	POSITION pos = m_lstSegments.GetHeadPosition();
	while( pos )
	{
		CSourceSegment* pSourceSegment = m_lstSegments.GetNext( pos );

		hr = pSourceSegment->Save( pIRiffStream, fReference );
		if( FAILED( hr ) )
		{
			goto ON_ERROR;
		}
		hr = S_OK;	// Change possible S_FALSE to S_OK;
	}

ON_ERROR:
    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CFolder::InsertSourceSegment

HRESULT CFolder::InsertSourceSegment( CSourceSegment* pSourceSegmentToInsert )
{
	if( pSourceSegmentToInsert == NULL )
	{
		ASSERT( 0 );
		return E_INVALIDARG;
	}

	pSourceSegmentToInsert->AddRef();

	// Add SourceSegment to list
	m_lstSegments.AddTail( pSourceSegmentToInsert );

	// Sync changes
	m_pSong->SetModified( TRUE );
	m_pSong->SyncChanges( SC_EDITOR | SC_DIRECTMUSIC,
						  SSE_SOURCE_SEGMENT_LIST | SSE_TRANSITION_LIST | SSE_TRACK_LIST | SSE_SELECTED_VIRTUAL_SEGMENT,
						  NULL );
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CFolder::RemoveSourceSegment

HRESULT CFolder::RemoveSourceSegment( CSourceSegment* pSourceSegment )
{
	if( pSourceSegment == NULL )
	{
		ASSERT( 0 );
		return E_INVALIDARG;
	}

	// If we are not dropping pSourceSegment in a different Container folder....
	if( m_pSong->m_fChangingFolder == false )
	{
		// Make sure other VirtualSegments do not point to pSourceSegment
		m_pSong->RemoveSourceSegmentReferences( pSourceSegment );
	}

	// Remove from Segment list
	POSITION pos = m_lstSegments.Find( pSourceSegment );
	if( pos )
	{
		m_lstSegments.RemoveAt( pos );

		// Sync changes
		m_pSong->SetModified( TRUE );
		m_pSong->SyncChanges( SC_EDITOR | SC_DIRECTMUSIC,
							  SSE_SOURCE_SEGMENT_LIST | SSE_TRANSITION_LIST | SSE_TRACK_LIST | SSE_SELECTED_VIRTUAL_SEGMENT,
							  NULL );

		RELEASE( pSourceSegment );
		return S_OK;
	}

	return E_FAIL;
}


///////////////////////////////////////////////////////////////////////////
// CFolder::OnUpdate	(handles Song's IDMUSProdNotifSink notifications)

HRESULT CFolder::OnUpdate( IDMUSProdNode* pIDocRootNode, GUID guidUpdateType, VOID* pData )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CSourceSegment* pSourceSegment;

	// FRAMEWORK_FileLoadFinished
	if( ::IsEqualGUID( guidUpdateType, FRAMEWORK_FileLoadFinished ) )
	{
		GUID guidFile = *(GUID *)pData;

		BOOL fChanged = FALSE;
		
		POSITION pos = m_lstSegments.GetHeadPosition();
		while( pos )
		{
			pSourceSegment = m_lstSegments.GetNext( pos );

			if( ::IsEqualGUID( pSourceSegment->m_FileRef.li.guidFile, guidFile ) )
			{ 
				if( SUCCEEDED ( pSourceSegment->SetFileReference( guidFile ) ) )
				{
					if( pSourceSegment->m_pIDocRootNode
					&&  pSourceSegment->m_pIParentNode )
					{
						// Need to refresh node name and node image index
						theApp.m_pSongComponent->m_pIFramework8->RefreshNode( pSourceSegment );
					}
				}

				fChanged = TRUE;
			}
		}

		if( fChanged )
		{
			if( m_pIDocRootNode
			&&  m_pIDocRootNode != this )
			{
				// Notify DocRoot that the Song has changed
				IDMUSProdNotifySink* pINotifySink;
				if( SUCCEEDED ( m_pIDocRootNode->QueryInterface( IID_IDMUSProdNotifySink, (void**)&pINotifySink ) ) )
				{
					pINotifySink->OnUpdate( m_pSong, SONG_FileLoadFinished, NULL );

					RELEASE( pINotifySink );
				}
			}
		}

		if( theApp.m_pSongComponent->m_pEmbeddedFileRootFile )
		{
			if( ::IsEqualGUID(theApp.m_pSongComponent->m_pEmbeddedFileRootFile->guidNotification, guidFile) )
			{ 
				delete theApp.m_pSongComponent->m_pEmbeddedFileRootFile;
				theApp.m_pSongComponent->m_pEmbeddedFileRootFile = NULL;
			}
		}
	}
	else
	{
		POSITION pos = m_lstSegments.GetHeadPosition();
		while( pos )
		{
			pSourceSegment = m_lstSegments.GetNext( pos );

			if( pSourceSegment->m_FileRef.pIDocRootNode == pIDocRootNode )
			{
				// FRAMEWORK_FileDeleted
				// FRAMEWORK_FileClosed
				if( ::IsEqualGUID( guidUpdateType, FRAMEWORK_FileDeleted )
				||  ::IsEqualGUID( guidUpdateType, FRAMEWORK_FileClosed ) )
				{
					DeleteChildNode( pSourceSegment, FALSE );
					return S_OK;
				}

				// FRAMEWORK_FileReplaced
				else if( ::IsEqualGUID( guidUpdateType, FRAMEWORK_FileReplaced ) )
				{
					pSourceSegment->SetFileReference( (IDMUSProdNode *)pData );
					theApp.m_pSongComponent->m_pIFramework8->RefreshNode( pSourceSegment );
					m_pSong->SetModified( TRUE );
					return S_OK;
				}

				// DOCROOT_GuidChange
				else if( ::IsEqualGUID( guidUpdateType, DOCROOT_GuidChange ) )
				{
					m_pSong->SetModified( TRUE );
					return S_OK;
				}
				
				// FRAMEWORK_FileNameChange
				else if( ::IsEqualGUID( guidUpdateType, FRAMEWORK_FileNameChange ) )
				{
					pSourceSegment->SyncListInfo();
					m_pSong->SetModified( TRUE );
					m_pSong->SyncChanges( SC_EDITOR,
										  SSE_REDRAW_TRACK_LIST,
										  NULL );
					return S_OK;
				}

				// GUID_Segment_Name_Change
				else if( ::IsEqualGUID(guidUpdateType, GUID_Segment_Name_Change ) )
				{
					theApp.m_pSongComponent->m_pIFramework8->RefreshNode( pSourceSegment );
					pSourceSegment->SyncListInfo();
					m_pSong->SyncChanges( SC_EDITOR,
										  SSE_SOURCE_SEGMENT_LIST | SSE_REDRAW_TRACK_LIST,
										  NULL );
					return S_OK;
				}

				// GUID_TimeSignature
				else if( ::IsEqualGUID(guidUpdateType, GUID_TimeSignature ) )
				{
					m_pSong->RecomputeVirtualSegmentLengths();
					return S_OK;
				}

				// All other notifications
				else
				{
					theApp.m_pSongComponent->m_pIFramework8->RefreshNode( pSourceSegment );
					pSourceSegment->SyncListInfo();
					return S_OK;
				}
			}
		}
	}

	return E_INVALIDARG;
}


///////////////////////////////////////////////////////////////////////////
// CFolder::IsDocRootInFolder

BOOL CFolder::IsDocRootInFolder( IDMUSProdNode* pIDocRootNode )
{
	CSourceSegment* pSourceSegment;

    POSITION pos = m_lstSegments.GetHeadPosition();
    while( pos )
    {
        pSourceSegment = m_lstSegments.GetNext( pos );

		if( pSourceSegment->m_FileRef.pIDocRootNode == pIDocRootNode )
		{
			return TRUE;
		}
	}

	return FALSE;
}


///////////////////////////////////////////////////////////////////////////
// CFolder::GetSourceSegmentFromDocRoot

CSourceSegment* CFolder::GetSourceSegmentFromDocRoot( IDMUSProdNode* pIDocRootNode )
{
	CSourceSegment* pTheSourceSegment = NULL;
	CSourceSegment* pSourceSegment;

    POSITION pos = m_lstSegments.GetHeadPosition();
    while( pos )
    {
        pSourceSegment = m_lstSegments.GetNext( pos );

		if( pSourceSegment->m_FileRef.pIDocRootNode == pIDocRootNode )
		{
			pTheSourceSegment = pSourceSegment;
			break;
		}
	}

	return pTheSourceSegment;
}


///////////////////////////////////////////////////////////////////////////
// CFolder::EmptySourceSegmentList

void CFolder::EmptySourceSegmentList( void )
{
	CSourceSegment* pSourceSegment;

	while( !m_lstSegments.IsEmpty() )
	{
		pSourceSegment = m_lstSegments.RemoveHead();

		pSourceSegment->SetFileReference( NULL );
		RELEASE( pSourceSegment );
	}
}
