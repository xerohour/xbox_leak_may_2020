// FolderSegments.cpp : implementation file
//

#include "stdafx.h"

#include "SongDesignerDLL.h"
#include "Song.h"
#include <SegmentDesigner.h>
#include <dmusicf.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CFolderSegments constructor/destructor

CFolderSegments::CFolderSegments()
{
	m_pSong = NULL;
	m_pIDocRootNode = NULL;
	m_pIParentNode = NULL;
	m_fModified = FALSE;

	TCHAR achName[MID_BUFFER];

	m_FolderEmbed.m_guidTypeNode = GUID_Song_SegmentEmbedFolderNode;
	::LoadString( theApp.m_hInstance, IDS_EMBED_FOLDER_NAME, achName, MID_BUFFER );
	m_FolderEmbed.m_strName = achName;

	m_FolderReference.m_guidTypeNode = GUID_Song_SegmentRefFolderNode;
	::LoadString( theApp.m_hInstance, IDS_REF_FOLDER_NAME, achName, MID_BUFFER );
	m_FolderReference.m_strName = achName;
}

CFolderSegments::~CFolderSegments()
{
}


/////////////////////////////////////////////////////////////////////////////
// CFolderSegments IUnknown implementation

HRESULT CFolderSegments::QueryInterface( REFIID riid, LPVOID FAR* ppvObj )
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

ULONG CFolderSegments::AddRef()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pSong != NULL );

	AfxOleLockApp(); 

	return m_pSong->AddRef();
}

ULONG CFolderSegments::Release()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pSong != NULL );

	AfxOleUnlockApp(); 

	return m_pSong->Release();
}


/////////////////////////////////////////////////////////////////////////////
// CFolderSegments IDMUSProdNode implementation

/////////////////////////////////////////////////////////////////////////////
// CFolderSegments IDMUSProdNode::GetNodeImageIndex

HRESULT CFolderSegments::GetNodeImageIndex( short* pnFirstImage )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( theApp.m_pSongComponent != NULL );

	return( theApp.m_pSongComponent->GetFolderImageIndex(pnFirstImage) );
}


/////////////////////////////////////////////////////////////////////////////
// CFolderSegments IDMUSProdNode::GetFirstChild

HRESULT CFolderSegments::GetFirstChild( IDMUSProdNode** ppIFirstChildNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( ppIFirstChildNode == NULL )
	{
		return E_POINTER;
	}
	
	m_FolderEmbed.AddRef();
	*ppIFirstChildNode = (IDMUSProdNode *)&m_FolderEmbed;

    return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CFolderSegments IDMUSProdNode::GetNextChild

HRESULT CFolderSegments::GetNextChild( IDMUSProdNode* pIChildNode, IDMUSProdNode** ppINextChildNode )
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

	if( pIChildNode == (IDMUSProdNode *)&m_FolderEmbed )
	{
		m_FolderReference.AddRef();
		*ppINextChildNode = (IDMUSProdNode *)&m_FolderReference;
	}

    return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CFolderSegments IDMUSProdNode::GetComponent

HRESULT CFolderSegments::GetComponent( IDMUSProdComponent** ppIComponent )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( theApp.m_pSongComponent != NULL );

	return theApp.m_pSongComponent->QueryInterface( IID_IDMUSProdComponent, (void**)ppIComponent );
}


/////////////////////////////////////////////////////////////////////////////
// CFolderSegments IDMUSProdNode::GetDocRootNode

HRESULT CFolderSegments::GetDocRootNode( IDMUSProdNode** ppIDocRootNode )
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
// CFolderSegments IDMUSProdNode::SetDocRootNode

HRESULT CFolderSegments::SetDocRootNode( IDMUSProdNode* pIDocRootNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( pIDocRootNode != NULL );

	m_pIDocRootNode = pIDocRootNode;
//	m_pIDocRootNode->AddRef();		intentionally missing

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CFolderSegments IDMUSProdNode::GetParentNode

HRESULT CFolderSegments::GetParentNode( IDMUSProdNode** ppIParentNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pIParentNode != NULL );

	m_pIParentNode->AddRef();
	*ppIParentNode = m_pIParentNode;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CFolderSegments IDMUSProdNode::SetParentNode

HRESULT CFolderSegments::SetParentNode( IDMUSProdNode* pIParentNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( pIParentNode != NULL );

	m_pIParentNode = pIParentNode;
//	m_pIParentNode->AddRef();		intentionally missing

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CFolderSegments IDMUSProdNode::GetNodeId

HRESULT CFolderSegments::GetNodeId( GUID* pguid )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pguid == NULL )
	{
		return E_POINTER;
	}

	*pguid = GUID_Song_SegmentsFolderNode;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CFolderSegments IDMUSProdNode::GetNodeName

HRESULT CFolderSegments::GetNodeName( BSTR* pbstrName )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CString strName;
	TCHAR achBuffer[MID_BUFFER];

	if( ::LoadString(theApp.m_hInstance, IDS_SEGMENTS_FOLDER_NAME, achBuffer, MID_BUFFER) )
	{
		strName = achBuffer;
	}

    *pbstrName = strName.AllocSysString();

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CFolderSegments IDMUSProdNode::GetNodeNameMaxLength

HRESULT CFolderSegments::GetNodeNameMaxLength( short* pnMaxLength )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	*pnMaxLength = -1;	// Can't rename a 'Source Segments' folder

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CFolderSegments IDMUSProdNode::ValidateNodeName

HRESULT CFolderSegments::ValidateNodeName( BSTR bstrName )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return E_NOTIMPL;	// Can't rename a 'Source Segments' folder
}


/////////////////////////////////////////////////////////////////////////////
// CFolderSegments IDMUSProdNode::SetNodeName

HRESULT CFolderSegments::SetNodeName( BSTR bstrName )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return E_NOTIMPL;	// Can't rename a 'Source Segments' folder
}


/////////////////////////////////////////////////////////////////////////////
// CFolderSegments IDMUSProdNode::GetNodeListInfo

HRESULT CFolderSegments::GetNodeListInfo( DMUSProdListInfo* pListInfo )
{
//	AFX_MANAGE_STATE( _afxModuleAddrThis );

	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CFolderSegments IDMUSProdNode::GetEditorClsId

HRESULT CFolderSegments::GetEditorClsId( CLSID* pClsId )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return E_NOTIMPL;	// Can't edit a 'Source Segments' folder
}


/////////////////////////////////////////////////////////////////////////////
// CFolderSegments IDMUSProdNode::GetEditorTitle

HRESULT CFolderSegments::GetEditorTitle( BSTR* pbstrTitle )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return E_NOTIMPL;	// Can't edit a 'Source Segments' folder
}


/////////////////////////////////////////////////////////////////////////////
// CFolderSegments IDMUSProdNode::GetEditorWindow

HRESULT CFolderSegments::GetEditorWindow( HWND* hWndEditor )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return E_NOTIMPL;	// Can't edit a 'Source Segments' folder
}


/////////////////////////////////////////////////////////////////////////////
// CFolderSegments IDMUSProdNode::SetEditorWindow

HRESULT CFolderSegments::SetEditorWindow( HWND hWndEditor )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return E_NOTIMPL;	// Can't edit a 'Source Segments' folder
}


/////////////////////////////////////////////////////////////////////////////
// CFolderSegments IDMUSProdNode::UseOpenCloseImages

HRESULT CFolderSegments::UseOpenCloseImages( BOOL* pfUseOpenCloseImages )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	*pfUseOpenCloseImages = TRUE;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CFolderSegments IDMUSProdNode::GetRightClickMenuId

HRESULT CFolderSegments::GetRightClickMenuId( HINSTANCE* phInstance, UINT* pnMenuId )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	*phInstance = theApp.m_hInstance;
	*pnMenuId   = IDM_FOLDER_SEGMENTS_NODE_RMENU;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CFolderSegments IDMUSProdNode::OnRightClickMenuInit

HRESULT CFolderSegments::OnRightClickMenuInit( HMENU hMenu )
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
// CFolderSegments IDMUSProdNode::OnRightClickMenuSelect

HRESULT CFolderSegments::OnRightClickMenuSelect( long lCommandId )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pSong != NULL );

	HRESULT hr = E_FAIL;

	switch( lCommandId )
	{
		case IDM_NEW_SOURCE_SEGMENT:
			hr = m_FolderReference.InsertChildNode( NULL );
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
// CFolderSegments IDMUSProdNode::DeleteChildNode

HRESULT CFolderSegments::DeleteChildNode( IDMUSProdNode* pIChildNode, BOOL fPromptUser )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(pIChildNode);
	UNREFERENCED_PARAMETER(fPromptUser);

	return E_NOTIMPL;	// Can't delete children from a 'Source Segments' folder
}


/////////////////////////////////////////////////////////////////////////////
// CFolderSegments IDMUSProdNode::InsertChildNode

HRESULT CFolderSegments::InsertChildNode( IDMUSProdNode* pIChildNode )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(pIChildNode);

	return E_NOTIMPL;	// Can't insert children into a 'Source Segments' folder
}


/////////////////////////////////////////////////////////////////////////////
// CFolderSegments IDMUSProdNode::DeleteNode

HRESULT CFolderSegments::DeleteNode( BOOL fPromptUser )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return E_NOTIMPL;	// Can't delete a 'Source Segments' folder
}


/////////////////////////////////////////////////////////////////////////////
// CFolderSegments IDMUSProdNode::OnNodeSelChanged

HRESULT CFolderSegments::OnNodeSelChanged( BOOL fSelected )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CFolderSegments IDMUSProdNode::CreateDataObject

HRESULT CFolderSegments::CreateDataObject( IDataObject** ppIDataObject )
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
// CFolderSegments IDMUSProdNode::CanCut

HRESULT CFolderSegments::CanCut( void )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
 
	return S_FALSE;		// Can't remove the 'Source Segments' folder from Song
}


/////////////////////////////////////////////////////////////////////////////
// CFolderSegments IDMUSProdNode::CanCopy

HRESULT CFolderSegments::CanCopy( void )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
 
	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CFolderSegments IDMUSProdNode::CanDelete

HRESULT CFolderSegments::CanDelete( void )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return S_FALSE;		// Can't remove the 'Source Segments' folder from Song
}


/////////////////////////////////////////////////////////////////////////////
// CFolderSegments IDMUSProdNode::CanDeleteChildNode

HRESULT CFolderSegments::CanDeleteChildNode( IDMUSProdNode* pIChildNode )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(pIChildNode);

	ASSERT( 0 );

	return E_NOTIMPL;	// Cannot delete children
}


/////////////////////////////////////////////////////////////////////////////
// CFolderSegments IDMUSProdNode::CanPasteFromData

HRESULT CFolderSegments::CanPasteFromData( IDataObject* pIDataObject, BOOL* pfWillSetReference )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Let reference folder handle CanPasteFromData()
	return m_FolderReference.CanPasteFromData( pIDataObject, pfWillSetReference );
}


/////////////////////////////////////////////////////////////////////////////
// CFolderSegments IDMUSProdNode::PasteFromData

HRESULT CFolderSegments::PasteFromData( IDataObject* pIDataObject )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Let reference folder handle PasteFromData()
	return m_FolderReference.PasteFromData( pIDataObject );
}


/////////////////////////////////////////////////////////////////////////////
// CFolderSegments IDMUSProdNode::CanChildPasteFromData

HRESULT CFolderSegments::CanChildPasteFromData( IDataObject* pIDataObject, IDMUSProdNode* pIChildNode,
											   BOOL* pfWillSetReference )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	ASSERT( 0 );	// Should not happen!
	return E_FAIL;
}


/////////////////////////////////////////////////////////////////////////////
// CFolderSegments IDMUSProdNode::ChildPasteFromData

HRESULT CFolderSegments::ChildPasteFromData( IDataObject* pIDataObject, IDMUSProdNode* pIChildNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	ASSERT( 0 );	// Should not happen!
	return E_FAIL;
}


/////////////////////////////////////////////////////////////////////////////
// CFolderSegments IDMUSProdNode::GetObject

HRESULT CFolderSegments::GetObject( REFCLSID rclsid, REFIID riid, void** ppvObject )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return E_NOTIMPL;
}


///////////////////////////////////////////////////////////////////////////
// CFolderSegments::OnUpdate

HRESULT CFolderSegments::OnUpdate( IDMUSProdNode* pIDocRootNode, GUID guidUpdateType, VOID* pData )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Check Embed Runtime folder
	m_FolderEmbed.OnUpdate( pIDocRootNode, guidUpdateType, pData );

	// Check Reference Runtime folder
	m_FolderReference.OnUpdate( pIDocRootNode, guidUpdateType, pData );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CFolderSegments SaveSegments
    
HRESULT CFolderSegments::SaveSegments( IDMUSProdRIFFStream* pIRiffStream )
{
	HRESULT hr = S_OK;
    MMCKINFO ckMain;

	// Write DMUS_FOURCC_SONGSEGMENTS_LIST header
	ckMain.fccType = DMUS_FOURCC_SONGSEGMENTS_LIST;
	if( pIRiffStream->CreateChunk(&ckMain, MMIO_CREATELIST) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Save embedded list
	hr = m_FolderEmbed.SaveSegments( pIRiffStream );
	if( FAILED( hr ) )
	{
		goto ON_ERROR;
	}

	// Save referenced list
	hr = m_FolderReference.SaveSegments( pIRiffStream );
	if( FAILED( hr ) )
	{
		goto ON_ERROR;
	}

	if( pIRiffStream->Ascend(&ckMain, 0) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

ON_ERROR:
    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CFolderSegments LoadSegments
    
HRESULT CFolderSegments::LoadSegments( IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain )
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
			case FOURCC_LIST:
				switch( ck.fccType )
				{
					case DMUS_FOURCC_SONGSEGMENT_LIST:
					{
						CSourceSegment* pSourceSegment = new CSourceSegment( m_pSong );
						if( pSourceSegment == NULL )
						{
							hr = E_OUTOFMEMORY;
							goto ON_ERROR;
						}

						BOOL fReference = FALSE;

						hr = pSourceSegment->Load( pIRiffStream, &ck, &fReference );
						if( hr == S_OK )
						{
							if( fReference  )
							{
								m_FolderReference.m_lstSegments.AddTail( pSourceSegment );
							}
							else
							{
								m_FolderEmbed.m_lstSegments.AddTail( pSourceSegment );
							}
						}
						else if( hr == S_FALSE )
						{
							// Could not resolve file reference so discard this object
							delete pSourceSegment;
							hr = S_OK;
						}
						else
						{
							delete pSourceSegment;
							goto ON_ERROR;
						}
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
