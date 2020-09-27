// DirectoryNode.cpp: implementation of the CDirectoryNode class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "JazzApp.h"
#include "MainFrm.h"
#include "direct.h"
#include "Timeline.h"
#include "StripMgr.h"
#include "DLSDesigner.h"
#include "SegmentDesigner.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


// {3B119360-CBAE-11d0-89AE-00A0C9054129}
const GUID GUID_DirectoryNode = 
{ 0x3b119360, 0xcbae, 0x11d0, { 0x89, 0xae, 0x0, 0xa0, 0xc9, 0x5, 0x41, 0x29 } };


//////////////////////////////////////////////////////////////////////
// CDirectoryNode Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDirectoryNode::CDirectoryNode()
{
    m_dwRef = 0;

	m_pIParentNode = NULL;
	m_hItem = NULL;
}

CDirectoryNode::~CDirectoryNode()
{
	IDMUSProdNode* pINode;

	while( !m_lstNodes.IsEmpty() )
	{
		pINode = static_cast<IDMUSProdNode*>( m_lstNodes.RemoveHead() );
		if( pINode )
		{
			DeleteChildNode( pINode, FALSE );
			pINode->Release();
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDirectoryNode IUnknown implementation

HRESULT CDirectoryNode::QueryInterface( REFIID riid, LPVOID FAR* ppvObj )
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

    if( ::IsEqualIID(riid, IID_IDMUSProdNode)
	||  ::IsEqualIID(riid, IID_IUnknown) )
    {
        AddRef();
        *ppvObj = (IDMUSProdNode *)this;
        return S_OK;
    }

    if( ::IsEqualIID(riid, IID_IDMUSProdPropPageObject) )
    {
		IDMUSProdProject* pIProject; 

		if( SUCCEEDED ( theApp.m_pFramework->FindProject( this, &pIProject ) ) )
		{
			HRESULT hr = pIProject->QueryInterface( riid, ppvObj );
			pIProject->Release();
			return hr;
		}

		*ppvObj = NULL;
		return E_NOINTERFACE;
    }

    *ppvObj = NULL;
    return E_NOINTERFACE;
}

ULONG CDirectoryNode::AddRef()
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

    return ++m_dwRef;
}

ULONG CDirectoryNode::Release()
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

    ASSERT( m_dwRef != 0 );

    --m_dwRef;

    if( m_dwRef == 0 )
    {
        delete this;
        return 0;
    }

    return m_dwRef;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectoryNode IDMUSProdNode implementation

/////////////////////////////////////////////////////////////////////////////
// CDirectoryNode IDMUSProdNode::GetNodeImageIndex

HRESULT CDirectoryNode::GetNodeImageIndex( short* pnFirstImage )
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	// Store directory path
	CString strPath;
	ConstructPath( strPath );
	
	// See if this directory contains a Producer Project
	WIN32_FIND_DATA	fd;
	TCHAR achFileMask[_MAX_PATH + 1];

	wsprintf( achFileMask, "%s%s.pro", strPath, m_strName );

	HANDLE hFind = FindFirstFile( achFileMask, &fd );
	if( hFind == INVALID_HANDLE_VALUE )
	{
		// Directory does not contain a Producer Project
		*pnFirstImage = theApp.m_nFirstImage + FIRST_DIRECTORY_IMAGE;
	}
	else
	{
		// Directory contains a Producer Project
		*pnFirstImage = theApp.m_nFirstImage + FIRST_PROJECT_IMAGE;
		FindClose( hFind );
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectoryNode IDMUSProdNode::GetFirstChild

HRESULT CDirectoryNode::GetFirstChild( IDMUSProdNode** ppIFirstChildNode )
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	if( ppIFirstChildNode == NULL )
	{
		return E_POINTER;
	}
	
	*ppIFirstChildNode = NULL;

	if( !m_lstNodes.IsEmpty() )
	{
		IDMUSProdNode* pINode = static_cast<IDMUSProdNode*>( m_lstNodes.GetHead() );
		
		if( pINode )
		{
			pINode->AddRef();
			*ppIFirstChildNode = pINode;
		}
	}

    return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectoryNode IDMUSProdNode::GetNextChild

HRESULT CDirectoryNode::GetNextChild( IDMUSProdNode* pIChildNode, IDMUSProdNode** ppINextChildNode )
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	if( ppINextChildNode == NULL )
	{
		return E_POINTER;
	}
	
	*ppINextChildNode = NULL;

	if( pIChildNode == NULL )
	{
		return E_INVALIDARG;
	}

	IDMUSProdNode* pINode;

    POSITION pos = m_lstNodes.GetHeadPosition();

    while( pos )
    {
        pINode = m_lstNodes.GetNext( pos );
		if( pINode == pIChildNode )
		{
			if( pos )
			{
				pINode = m_lstNodes.GetNext( pos );

				pINode->AddRef();
				*ppINextChildNode = pINode;
			}
			break;
		}
    }

    return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectoryNode IDMUSProdNode::GetComponent

HRESULT CDirectoryNode::GetComponent( IDMUSProdComponent** ppIComponent )
{
//	AFX_MANAGE_STATE( AfxGetAppModuleState() );

    return E_NOTIMPL;	// DirectoryNode node is not associated with a Component
}


/////////////////////////////////////////////////////////////////////////////
// CDirectoryNode IDMUSProdNode::GetDocRootNode

HRESULT CDirectoryNode::GetDocRootNode( IDMUSProdNode** ppIDocRootNode )
{
//	AFX_MANAGE_STATE( AfxGetAppModuleState() );

    return E_NOTIMPL;	// DirectoryNode node is not associated with a Document
}


/////////////////////////////////////////////////////////////////////////////
// CDirectoryNode IDMUSProdNode::SetDocRootNode

HRESULT CDirectoryNode::SetDocRootNode( IDMUSProdNode* pIDocRootNode )
{
//	AFX_MANAGE_STATE( AfxGetAppModuleState() );

    return E_NOTIMPL;	// DirectoryNode node is not associated with a Document
}


/////////////////////////////////////////////////////////////////////////////
// CDirectoryNode IDMUSProdNode::GetParentNode

HRESULT CDirectoryNode::GetParentNode( IDMUSProdNode** ppIParentNode )
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	ASSERT( m_pIParentNode != NULL );

	if( ppIParentNode == NULL )
	{
		return E_POINTER;
	}

	m_pIParentNode->AddRef();
	*ppIParentNode = m_pIParentNode;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectoryNode IDMUSProdNode::SetParentNode

HRESULT CDirectoryNode::SetParentNode( IDMUSProdNode* pIParentNode )
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	ASSERT( pIParentNode != NULL );

	if( pIParentNode == NULL )
	{
		return E_INVALIDARG;
	}

	m_pIParentNode = pIParentNode;
//	m_pIParentNode->AddRef();		intentionally missing

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectoryNode IDMUSProdNode::GetNodeId

HRESULT CDirectoryNode::GetNodeId( GUID* pguid )
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	if( pguid == NULL )
	{
		return E_POINTER;
	}

	*pguid = GUID_DirectoryNode;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectoryNode IDMUSProdNode::GetNodeName

HRESULT CDirectoryNode::GetNodeName( BSTR* pbstrName )
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	if( pbstrName == NULL )
	{
		return E_POINTER;
	}

    *pbstrName = m_strName.AllocSysString();

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectoryNode IDMUSProdNode::GetNodeNameMaxLength

HRESULT CDirectoryNode::GetNodeNameMaxLength( short* pnMaxLength )
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	if( pnMaxLength == NULL )
	{
		return E_POINTER;
	}

	*pnMaxLength = MAX_LENGTH_DIR_NAME;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectoryNode IDMUSProdNode::ValidateNodeName

HRESULT CDirectoryNode::ValidateNodeName( BSTR bstrName )
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	CString strName = bstrName;
	::SysFreeString( bstrName );

	int iBad = strName.FindOneOf( _T("\\/:*?\"<>;|#%") );
	if( iBad != -1 )
	{
		AfxMessageBox( IDS_ERR_FOLDER_CHAR );
		return S_FALSE;
	}

	// Determine node's old filename
	CString strOldPathName;
	ConstructPath( strOldPathName );

	// Store original name
	CString strOrigName = m_strName;
	m_strName = strName;

	// Determine node's new filename
	CString strNewPathName;
	ConstructPath( strNewPathName );

	// restore original name
	m_strName = strOrigName;

	if( strNewPathName.GetLength() >= _MAX_DIR )
	{
		AfxMessageBox( IDS_ERR_FILENAME_LENGTH );
		return S_FALSE;
	}

	// Make sure we aren't trying to rename the current directory
	TCHAR achCurrentDir[_MAX_PATH + 1];
	DWORD dwNbrBytes = ::GetCurrentDirectory( _MAX_PATH, achCurrentDir );
	if( (dwNbrBytes > 0) 
	&&  (dwNbrBytes < _MAX_PATH) )
	{
		CString strCurrentDir = achCurrentDir;
		strCurrentDir +=  + (TCHAR)'\\';

		if( _tcsicmp( strCurrentDir, strOldPathName ) == 0 )
		{
			int nFindPos = strCurrentDir.ReverseFind( (TCHAR)'\\' );
			if( nFindPos != -1 )
			{
				strCurrentDir = strCurrentDir.Left( nFindPos );

				nFindPos = strCurrentDir.ReverseFind( (TCHAR)'\\' );
				if( nFindPos != -1 )
				{
					strCurrentDir = strCurrentDir.Left( nFindPos );
					::SetCurrentDirectory( strCurrentDir );
				}
			}
		}
	}

	// Notify connected nodes filenames are about to change
	if(	theApp.m_fSendFileNameChangeNotification )
	{
		NotifyAllFiles( FRAMEWORK_BeforeFileNameChange );
	}

	// Rename node directory to the new path
	if( theApp.RenameThePath(strOldPathName, strNewPathName, strOldPathName) == FALSE )
	{
		// Notify connected nodes filename change was aborted
		if(	theApp.m_fSendFileNameChangeNotification )
		{
			NotifyAllFiles( FRAMEWORK_AbortFileNameChange );
		}
		return S_FALSE;
	}

	// Adjust Project Folders
	IDMUSProdProject* pIProject;

	if( SUCCEEDED ( theApp.m_pFramework->FindProject( this, &pIProject ) ) )
	{
		CProject* pProject = (CProject *)pIProject;

		// Update path to affected Project folders
		pProject->AdjustProjectFolders( strOldPathName, strNewPathName );

		pIProject->Release();
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectoryNode IDMUSProdNode::SetNodeName

HRESULT CDirectoryNode::SetNodeName( BSTR bstrName )
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	CString strName = bstrName;
	::SysFreeString( bstrName );

	// Change node name
	m_strName = strName;

	// Rename path to all open documents
	IDMUSProdProject* pIProject;

	if( SUCCEEDED ( theApp.m_pFramework->FindProject( this, &pIProject ) ) )
	{
		CProject* pProject = (CProject *)pIProject;

		// Update path to all open documents
		pProject->RenameOpenDocuments();

		pIProject->Release();
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectoryNode IDMUSProdNode::GetNodeListInfo

HRESULT CDirectoryNode::GetNodeListInfo( DMUSProdListInfo* pListInfo )
{
//	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	return E_NOTIMPL;
}

	
/////////////////////////////////////////////////////////////////////////////
// CDirectoryNode IDMUSProdNode::GetEditorClsId

HRESULT CDirectoryNode::GetEditorClsId( CLSID* pClsId )
{
//	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	return E_NOTIMPL;	// DirectoryNode is not associated with an editor
}


/////////////////////////////////////////////////////////////////////////////
// CDirectoryNode IDMUSProdNode::GetEditorTitle

HRESULT CDirectoryNode::GetEditorTitle( BSTR* pbstrTitle )
{
//	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	return E_NOTIMPL;	// DirectoryNode is not associated with an editor
}


/////////////////////////////////////////////////////////////////////////////
// CDirectoryNode IDMUSProdNode::GetEditorWindow

HRESULT CDirectoryNode::GetEditorWindow( HWND* hWndEditor )
{
//	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	return E_NOTIMPL;	// DirectoryNode is not associated with an editor
}


/////////////////////////////////////////////////////////////////////////////
// CDirectoryNode IDMUSProdNode::SetEditorWindow

HRESULT CDirectoryNode::SetEditorWindow( HWND hWndEditor )
{
//	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	return E_NOTIMPL;	// DirectoryNode is not associated with an editor
}


/////////////////////////////////////////////////////////////////////////////
// CDirectoryNode IDMUSProdNode::UseOpenCloseImages

HRESULT CDirectoryNode::UseOpenCloseImages( BOOL* pfUseOpenCloseImages )
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	if( pfUseOpenCloseImages == NULL )
	{
		return E_POINTER;
	}

	*pfUseOpenCloseImages = TRUE;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectoryNode IDMUSProdNode::GetRightClickMenuId

HRESULT CDirectoryNode::GetRightClickMenuId( HINSTANCE* phInstance, UINT* pnMenuId )
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	*phInstance = AfxGetResourceHandle();
	*pnMenuId   = IDM_DIRECTORY_NODE_RMENU;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectoryNode IDMUSProdNode::OnRightClickMenuInit

HRESULT CDirectoryNode::OnRightClickMenuInit( HMENU hMenu )
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	CMainFrame* pMainFrame = (CMainFrame *)theApp.m_pMainWnd;
	ASSERT( pMainFrame != NULL );

	CMenu menu;

	if( menu.Attach(hMenu) )
	{
		// IDM_IMPORT_MID, IDM_IMPORT_WAV, IDM_IMPORT_WAV_VAR and IDM_IMPORT_SEC
		IDMUSProdComponent* pIComponent;
		if( SUCCEEDED ( theApp.m_pFramework->FindComponent( CLSID_SegmentComponent, &pIComponent ) ) )
		{
			menu.EnableMenuItem( 5, (MF_ENABLED | MF_BYPOSITION) );
			pIComponent->Release();

			if( SUCCEEDED ( theApp.m_pFramework->FindComponent( CLSID_DLSComponent, &pIComponent ) ) )
			{
				menu.EnableMenuItem( IDM_IMPORT_WAV, (MF_ENABLED | MF_BYCOMMAND) );
				menu.EnableMenuItem( IDM_IMPORT_WAV_VAR, (MF_ENABLED | MF_BYCOMMAND) );
				pIComponent->Release();
			}
			else
			{
				menu.EnableMenuItem( IDM_IMPORT_WAV, (MF_GRAYED | MF_BYCOMMAND) );
				menu.EnableMenuItem( IDM_IMPORT_WAV_VAR, (MF_GRAYED | MF_BYCOMMAND) );
			}
		}
		else
		{
			menu.EnableMenuItem( 5, (MF_GRAYED | MF_BYPOSITION) );
		}
		
		// IDM_PASTE
		{
			IDataObject* pIDataObject;
			BOOL fWillSetReference;

			menu.EnableMenuItem( IDM_PASTE, (MF_GRAYED | MF_BYCOMMAND) );

			if( SUCCEEDED ( ::OleGetClipboard( &pIDataObject ) ) )
			{
				if( CanPasteFromData( pIDataObject, &fWillSetReference ) == S_OK )
				{
					menu.EnableMenuItem( IDM_PASTE, (MF_ENABLED | MF_BYCOMMAND) );
				}
				
				pIDataObject->Release();
			}
		}

		// IDM_SORT_FILENAME, IDM_SORT_FILETYPE and IDM_SORT_FILESIZE
		int nTreeSortType = pMainFrame->m_wndTreeBar.GetSortType();
		menu.CheckMenuItem( IDM_SORT_FILENAME, (nTreeSortType == TREE_SORTBY_NAME) ? MF_CHECKED : MF_UNCHECKED );
		menu.CheckMenuItem( IDM_SORT_FILETYPE, (nTreeSortType == TREE_SORTBY_TYPE) ? MF_CHECKED : MF_UNCHECKED );
		menu.CheckMenuItem( IDM_SORT_FILESIZE, (nTreeSortType == TREE_SORTBY_SIZE) ? MF_CHECKED : MF_UNCHECKED );

		menu.Detach();
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectoryNode IDMUSProdNode::OnRightClickMenuSelect

HRESULT CDirectoryNode::OnRightClickMenuSelect( long lCommandId )
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	CMainFrame* pMainFrame = (CMainFrame *)theApp.m_pMainWnd;
	ASSERT( pMainFrame != NULL );

	HRESULT hr = E_FAIL;

	switch( lCommandId )
	{
		case IDM_NEW:
			theApp.OnFileNew();
			hr = S_OK;
			break;

		case IDM_INSERT_FILE:
			theApp.OnFileOpen();
			hr = S_OK;
			break;

		case IDM_NEWFOLDER:
			NewFolder();
			hr = S_OK;
			break;

		case IDM_IMPORT_MID:
			theApp.OnImportMid();
			hr = S_OK;
			break;

		case IDM_IMPORT_WAV:
			theApp.OnImportWav();
			hr = S_OK;
			break;

		case IDM_IMPORT_WAV_VAR:
			theApp.OnImportWavVariations();
			hr = S_OK;
			break;

		case IDM_IMPORT_SEC:
			theApp.OnImportSec();
			hr = S_OK;
			break;

		case IDM_PASTE:
			pMainFrame->m_wndTreeBar.OnEditPaste();
			hr = S_OK;
			break;

		case IDM_RENAME:
			if( SUCCEEDED ( theApp.m_pFramework->EditNodeLabel((IDMUSProdNode *)this) ) )
			{
				hr = S_OK;
			}
			break;

		case IDM_DELETE:
			if( pMainFrame->m_wndTreeBar.DeleteTreeNode( this, DTN_PROMPT_USER ) )
			{
				hr = S_OK;
			}
			break;

		case IDM_SORT_FILENAME:
			pMainFrame->m_wndTreeBar.SetSortType( TREE_SORTBY_NAME );
			hr = S_OK;
			break;

		case IDM_SORT_FILETYPE:
			pMainFrame->m_wndTreeBar.SetSortType( TREE_SORTBY_TYPE );
			hr = S_OK;
			break;

		case IDM_SORT_FILESIZE:
			pMainFrame->m_wndTreeBar.SetSortType( TREE_SORTBY_SIZE );
			hr = S_OK;
			break;

		case IDM_PROPERTIES:
		{
			IDMUSProdProject* pIProject;

			if( SUCCEEDED ( theApp.m_pFramework->FindProject( this, &pIProject ) ) )
			{
				CProject* pProject = (CProject *)pIProject;

				pProject->OnShowProperties();

				pIProject->Release();
				hr = S_OK;
			}
			break;
		}
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectoryNode IDMUSProdNode::DeleteChildNode

HRESULT CDirectoryNode::DeleteChildNode( IDMUSProdNode* pIChildNode, BOOL fPromptUser )
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	ASSERT( pIChildNode != NULL );

	if( pIChildNode == NULL )
	{
		return E_INVALIDARG;
	}

	GUID guidNodeId;

	if( FAILED ( pIChildNode->GetNodeId( &guidNodeId ) ) )
	{
		return E_FAIL;
	}

	if( !IsEqualGUID (guidNodeId, GUID_DirectoryNode)
	&&  !IsEqualGUID (guidNodeId, GUID_FileNode) )
	{
		return E_INVALIDARG;
	}

	if( IsEqualGUID ( guidNodeId, GUID_FileNode ) )
	{
		CFileNode* pFileNode = (CFileNode *)pIChildNode;

		if( pFileNode->m_pIChildNode )
		{
			GUID guidUpdateType;

			if( theApp.m_fDeleteFromTree )
			{
				guidUpdateType = FRAMEWORK_FileDeleted;
			}
			else
			{
				guidUpdateType = FRAMEWORK_FileClosed;
			}

			CJzNotifyNode* pJzNotifyNode;
			IDMUSProdNotifySink* pINotifySink;

			// Notify connected nodes that file is being removed from Project Tree
			POSITION pos = pFileNode->m_lstNotifyNodes.GetHeadPosition();
			while( pos != NULL )
			{
				pJzNotifyNode = static_cast<CJzNotifyNode*>( pFileNode->m_lstNotifyNodes.GetNext(pos) );
				if( pJzNotifyNode->m_pINotifyThisNode )
				{
					if( SUCCEEDED ( pJzNotifyNode->m_pINotifyThisNode->QueryInterface( IID_IDMUSProdNotifySink, (void **)&pINotifySink ) ) )
					{
						pINotifySink->OnUpdate( pFileNode->m_pIChildNode, guidUpdateType, NULL );
						pINotifySink->Release();
					}
				}
			}
		}

		// Cleanup m_lstNotifyNodes
		pFileNode->EmptyNotifyNodesList();

		if( pFileNode->m_pProject )
		{
			// Remove from CProject file list
			pFileNode->m_pProject->RemoveFile( pFileNode );
		}

		pFileNode->ReleaseDocument();
	}

	// Remove node from Project Tree
	theApp.m_pFramework->RemoveNode( pIChildNode, fPromptUser );

	// Remove from CDirectoryNode Node list
	POSITION pos = m_lstNodes.Find( pIChildNode );
	if( pos )
	{
		m_lstNodes.RemoveAt( pos );
		pIChildNode->Release();
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectoryNode IDMUSProdNode::InsertChildNode

HRESULT CDirectoryNode::InsertChildNode( IDMUSProdNode* pIChildNode )
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	ASSERT( pIChildNode != NULL );

	if( pIChildNode == NULL )
	{
		return E_INVALIDARG;
	}

	GUID guidNodeId;

	if( FAILED ( pIChildNode->GetNodeId( &guidNodeId ) ) )
	{
		return E_FAIL;
	}

	if( !IsEqualGUID (guidNodeId, GUID_DirectoryNode)
	&&  !IsEqualGUID (guidNodeId, GUID_FileNode) )
	{
		return E_INVALIDARG;
	}

	pIChildNode->AddRef();

	// Add to CDirectoryNode Node list
	m_lstNodes.AddTail( pIChildNode );

	// Set parent node
	pIChildNode->SetParentNode( (IDMUSProdNode *)this );

	if( IsEqualGUID ( guidNodeId, GUID_FileNode ) )
	{
		IDMUSProdProject* pIProject;

		if( SUCCEEDED ( theApp.m_pFramework->FindProject( this, &pIProject ) ) )
		{
			CProject* pProject = (CProject *)pIProject;
			CFileNode* pFileNode = (CFileNode *)pIChildNode;

			// Add to CProject file list
			pProject->AddFile( pFileNode );

			pIProject->Release();
		}
		else
		{
			return E_FAIL;
		}
	}

	// Add node to Project Tree
	if( FAILED ( theApp.m_pFramework->AddNode(pIChildNode, (IDMUSProdNode *)this) ) )
	{
		DeleteChildNode( pIChildNode, FALSE );
		return E_FAIL;
	}

	if( IsEqualGUID ( guidNodeId, GUID_DirectoryNode ) )
	{
		// Create directory on hard drive
		CDirectoryNode* pDirNode = (CDirectoryNode *)pIChildNode;
		CString strPath;
		pDirNode->ConstructPath( strPath );
		theApp.CreateTheDirectory( strPath );
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectoryNode IDMUSProdNode::DeleteNode

HRESULT CDirectoryNode::DeleteNode( BOOL fPromptUser )
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	IDMUSProdNode* pIParentNode;

	if( SUCCEEDED ( GetParentNode(&pIParentNode) ) )
	{
		if( pIParentNode )
		{
			HRESULT hr = pIParentNode->DeleteChildNode( this, fPromptUser );
			pIParentNode->Release();
			return hr;
		}
	}

	return E_FAIL;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectoryNode IDMUSProdNode::OnNodeSelChanged

HRESULT CDirectoryNode::OnNodeSelChanged( BOOL fSelected )
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectoryNode IDMUSProdNode::GetObject

HRESULT CDirectoryNode::GetObject( REFCLSID rclsid, REFIID riid, void** ppvObject )
{
//	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectoryNode IDMUSProdNode::CreateDataObject

HRESULT CDirectoryNode::CreateDataObject( IDataObject** ppIDataObject )
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	if( ppIDataObject == NULL )
	{
		return E_POINTER;
	}

	*ppIDataObject = NULL;

	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectoryNode IDMUSProdNode::CanCut

HRESULT CDirectoryNode::CanCut( void )
{
//	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	return S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectoryNode IDMUSProdNode::CanCopy

HRESULT CDirectoryNode::CanCopy( void )
{
//	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	return S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectoryNode IDMUSProdNode::CanDelete

HRESULT CDirectoryNode::CanDelete( void )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectoryNode IDMUSProdNode::CanDeleteChildNode

HRESULT CDirectoryNode::CanDeleteChildNode( IDMUSProdNode* pIChildNode )
{
//	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	// Should not get here!
	ASSERT( 0 );

	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectoryNode IDMUSProdNode::CanPasteFromData

HRESULT CDirectoryNode::CanPasteFromData( IDataObject* pIDataObject, BOOL* pfWillSetReference )
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );
	
	if( pfWillSetReference == NULL )
	{
		return E_POINTER;
	}

	*pfWillSetReference = FALSE;

	if( pIDataObject == NULL )
	{
		return E_INVALIDARG;
	}

	// If data object has one of the registered file clipboard formats
	if( theApp.m_pFramework->FindFileClipFormat( pIDataObject ) )
	{
		// It must also have a CF_DMUSPROD_FILE format
		CAppJazzDataObject* pDataObject = new CAppJazzDataObject();
		if( pDataObject )
		{
			HRESULT hr = pDataObject->IsClipFormatAvailable( pIDataObject, theApp.m_pFramework->m_cfProducerFile );
			pDataObject->Release();
			
			if( SUCCEEDED ( hr ) )
			{
				return S_OK;
			}
		}

		return S_FALSE;
	}

	// Directory node can also paste a CF_DMUSPROD_FILE
	// without one of the registered file clipboard formats
	CAppJazzDataObject* pDataObject = new CAppJazzDataObject();
	if( pDataObject )
	{
		HRESULT hr = pDataObject->IsClipFormatAvailable( pIDataObject, theApp.m_pFramework->m_cfProducerFile );
		pDataObject->Release();
		
		if( SUCCEEDED ( hr ) )
		{
			return S_OK;
		}
	}

	return S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectoryNode IDMUSProdNode::GetPasteFileName

HRESULT CDirectoryNode::GetPasteFileName( CFProducerFile* pcfProducerFile, CString& strFileName )
{
	if( pcfProducerFile == NULL )
	{
		return E_INVALIDARG;
	}

	// Determine path of File
	CString strPath;

	theApp.m_nFileTarget = TGT_SELECTEDNODE;
	IDMUSProdNode* pIDirNode = theApp.m_pFramework->DetermineParentNode( NULL );
	theApp.m_nFileTarget = TGT_PROJECT;

	if( pIDirNode )
	{
		GUID guidNodeId;
		if( SUCCEEDED ( pIDirNode->GetNodeId( &guidNodeId ) ) )
		{
			if( IsEqualGUID ( guidNodeId, GUID_ProjectFolderNode )
			||  IsEqualGUID ( guidNodeId, GUID_DirectoryNode ) )
			{
				CDirectoryNode* pDirNode = (CDirectoryNode *)pIDirNode;
				pDirNode->ConstructPath( strPath );
			}
		}
	}

	if( strPath.IsEmpty() )
	{
		return E_FAIL;
	}

	int nFindPos = pcfProducerFile->strFileName.ReverseFind( (TCHAR)'.' );
	if( nFindPos == -1 )
	{
		return E_FAIL;
	}

	// Extract the filename's name
	CString strName = pcfProducerFile->strFileName.Left( nFindPos );

	// Extract the filename's extension
	CString strExt = pcfProducerFile->strFileName.Right( pcfProducerFile->strFileName.GetLength() - nFindPos );

	// Determine proposed filename
	CString strProposedFileName = strPath + strName + strExt;
	
	// Make sure we have a unique filename
	CString strFName;

	theApp.GetUniqueFileName( strProposedFileName, strFName );
	strFileName = strPath + strFName;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectoryNode IDMUSProdNode::PasteOpenFile

HRESULT CDirectoryNode::PasteOpenFile( CFProducerFile* pcfProducerFile, IStream* pIMemStream, IDMUSProdDocType* pIDocType )
{
	HRESULT hr;

	if( pcfProducerFile == NULL
	||  pIMemStream == NULL
	||  pIDocType == NULL )
	{
		return E_INVALIDARG;
	}

	// Get file's name
	CString strFileName;

	if( FAILED ( GetPasteFileName( pcfProducerFile, strFileName ) ) )
	{
		return E_FAIL;
	}

	// Get the original file's name
	CString strOrigFileName;

	CFileNode* pFileNode = theApp.GetFileByGUID( pcfProducerFile->guidFile );
	if( pFileNode )
	{
		pFileNode->ConstructFileName( strOrigFileName );
		pFileNode->Release();
		pFileNode = NULL;
	}

	// Paste the file
	STATSTG			statstg;
	ULARGE_INTEGER	uliSizeOut, uliSizeRead, uliSizeWritten;

	// Get pIMemStream's size
	hr = pIMemStream->Stat( &statstg, STATFLAG_NONAME );
	if( FAILED( hr ) )
	{
		return E_FAIL;
	}
	uliSizeOut = statstg.cbSize;

	// Get an IStream pointer to the file we are creating for the paste
	IStream* pIStream;

	BSTR bstrFileName = strFileName.AllocSysString();

	if( SUCCEEDED ( theApp.m_pFramework->AllocFileStream(bstrFileName, GENERIC_WRITE, FT_DESIGN,
					GUID_CurrentVersion, NULL, &pIStream) ) )
	{
		// Copy pIMemStream into pIStream
		hr = pIMemStream->CopyTo( pIStream, uliSizeOut, &uliSizeRead, &uliSizeWritten );
		pIStream->Release();

		if( FAILED( hr )
		||  uliSizeRead.QuadPart != uliSizeOut.QuadPart
		||  uliSizeWritten.QuadPart != uliSizeOut.QuadPart )
		{

			// If the storage ran out of space, return STG_E_MEDIUMFULL, otherwise just return E_FAIL
			if( hr == STG_E_MEDIUMFULL )
			{
				return hr;
			}
			return E_FAIL;
		}

		// Now open the file to create a document and place it in the Project Tree
		CComponentDoc* pComponentDoc = (CComponentDoc *)theApp.OpenTheFile( strFileName, TGT_SELECTEDNODE );

		// If filename has been altered, sync the object name
		if( pComponentDoc
		&&  pComponentDoc->m_pIDocRootNode )
		{
			// Extract the filename's name
			CString strNewName = strFileName;
			int nFindPos = strNewName.ReverseFind( (TCHAR)'.' );
			if( nFindPos != -1 )
			{
				strNewName = strNewName.Left( nFindPos );
				nFindPos = strNewName.ReverseFind( (TCHAR)'\\' );
				if( nFindPos != -1 )
				{
					strNewName = strNewName.Right( strNewName.GetLength() - nFindPos - 1 );
				}
			}

			// Extract the original filename's name
			CString strOrigName = strOrigFileName;
			nFindPos = strOrigName.ReverseFind( (TCHAR)'.' );
			if( nFindPos != -1 )
			{
				strOrigName = strOrigName.Left( nFindPos );
				nFindPos = strOrigName.ReverseFind( (TCHAR)'\\' );
				if( nFindPos != -1 )
				{
					strOrigName = strOrigName.Right( strOrigName.GetLength() - nFindPos - 1 );
				}
			}

			// Has filename been altered to make it unique?
			if( strNewName.CompareNoCase( strOrigName ) != 0 )
			{
				// Does current name equal original filename?
				BSTR bstrName;
				if( SUCCEEDED ( pComponentDoc->m_pIDocRootNode->GetNodeName( &bstrName ) ) )
				{
					CString strName = bstrName;
					::SysFreeString( bstrName );

					if( strName.CompareNoCase( strOrigName ) == 0 )
					{
						// Set object's name to the new filename
						bstrName = strNewName.AllocSysString();
						pComponentDoc->m_pIDocRootNode->SetNodeName( bstrName );
						theApp.m_pFramework->RefreshNode( pComponentDoc->m_pIDocRootNode );
					}
				}
			}
		}
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectoryNode IDMUSProdNode::PasteShutFile

HRESULT CDirectoryNode::PasteShutFile( CFProducerFile* pcfProducerFile )
{
	if( pcfProducerFile == NULL )
	{
		return E_INVALIDARG;
	}

	// Get the new file's name
	CString strFileName;

	if( FAILED ( GetPasteFileName( pcfProducerFile, strFileName ) ) )
	{
		return E_FAIL;
	}

	// Get the original file's name
	CString strOrigFileName;

	CFileNode* pFileNode = theApp.GetFileByGUID( pcfProducerFile->guidFile );
	if( pFileNode )
	{
		pFileNode->ConstructFileName( strOrigFileName );
		pFileNode->Release();
		pFileNode = NULL;
	}
	if( strOrigFileName.IsEmpty() )
	{
		return E_FAIL;
	}

	// Copy the file
	if( theApp.CopyTheFile( strOrigFileName, strFileName, TRUE ) )
	{
		// Create the new FileNode
		CFileNode* pNewFileNode = new CFileNode;
		
		if( pNewFileNode )
		{
			int nFindPos = strFileName.ReverseFind( (TCHAR)'\\' );
			if( nFindPos != -1 )
			{
				strFileName = strFileName.Right( strFileName.GetLength() - nFindPos - 1 );
			}

			// Add file to Project Tree
			pNewFileNode->m_strName = strFileName;
			InsertChildNode( pNewFileNode );
			pNewFileNode->m_strName.Empty();

			BSTR bstrFileName = strFileName.AllocSysString();
			pNewFileNode->SetNodeName( bstrFileName );

			// Sync List info
			pNewFileNode->SyncListInfo();

			return S_OK;
		}
	}

	return E_FAIL;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectoryNode IDMUSProdNode::PasteFromData

HRESULT CDirectoryNode::PasteFromData( IDataObject* pIDataObject )
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	if( pIDataObject == NULL )
	{
		return E_INVALIDARG;
	}

	IStream* pIStream = NULL;

	// Read CF_DMUSPROD_FILE information
	BOOL fCF_DMUSPROD_FILE = FALSE;
	CFProducerFile cfProducerFile;

	if( SUCCEEDED ( theApp.m_pFramework->LoadClipFormat( pIDataObject, theApp.m_pFramework->m_cfProducerFile, &pIStream ) ) )
	{
		if( SUCCEEDED ( theApp.m_pFramework->LoadCF_DMUSPROD_FILE( pIStream, &cfProducerFile ) ) )
		{
			fCF_DMUSPROD_FILE = TRUE;
		}

		pIStream->Release();
		pIStream = NULL;
	}

	if( fCF_DMUSPROD_FILE == FALSE )
	{
		return E_FAIL;
	}

	// Get stream of file data (if present)
	BOOL fFileStream = FALSE;
	IDMUSProdDocType* pIDocType = NULL;

	UINT uClipFormat = theApp.m_pFramework->FindFileClipFormat( pIDataObject );
	if( uClipFormat )
	{
		if( SUCCEEDED ( theApp.m_pFramework->LoadClipFormat( pIDataObject, uClipFormat, &pIStream ) ) )
		{
			if( SUCCEEDED ( theApp.m_pFramework->FindDocTypeByClipFormat( uClipFormat, &pIDocType ) ) )
			{
				fFileStream = TRUE;
			}
		}
	}

	HRESULT hr = E_FAIL;

	// Paste the file
	if( fFileStream )
	{
		// Adjust filename
		CString strExt;

		if( SUCCEEDED ( theApp.m_pFramework->FindExtensionByClipFormat( uClipFormat, strExt ) ) )
		{
			int nFindPos = cfProducerFile.strFileName.ReverseFind( (TCHAR)'.' );
			if( nFindPos != -1 )
			{
				cfProducerFile.strFileName = cfProducerFile.strFileName.Left( nFindPos );
			}
			cfProducerFile.strFileName += strExt;
		}

		// Do the paste
		hr = PasteOpenFile( &cfProducerFile, pIStream, pIDocType );
		pIStream->Release();
		pIDocType->Release();
	}
	else
	{
		hr = PasteShutFile( &cfProducerFile );
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectoryNode IDMUSProdNode::CanChildPasteFromData

HRESULT CDirectoryNode::CanChildPasteFromData( IDataObject* pIDataObject, IDMUSProdNode* pIChildNode,
											   BOOL* fWillSetReference )
{
//	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	// Should not get here!
	ASSERT( 0 );

	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectoryNode IDMUSProdNode::ChildPasteFromData

HRESULT CDirectoryNode::ChildPasteFromData( IDataObject* pIDataObject, IDMUSProdNode* pIChildNode )
{
//	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	// Should not get here!
	ASSERT( 0 );

	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// Additional methods

/////////////////////////////////////////////////////////////////////////////
// CDirectoryNode DeleteDirectoryToRecycleBin

BOOL CDirectoryNode::DeleteDirectoryToRecycleBin( void )
{
	IDMUSProdNode* pINode;
	GUID guidNodeId;

	theApp.DoWaitCursor( TRUE );

	CMainFrame* pMainFrame = (CMainFrame *)theApp.m_pMainWnd;
	ASSERT( pMainFrame != NULL );

	BOOL fAllFilesDeleted = TRUE;

	// Store directory path
	CString strDir;
	ConstructPath( strDir );
	
	// Delete each file in the directory
	POSITION pos = m_lstNodes.GetHeadPosition();
	while( pos != NULL )
	{
		pINode = static_cast<IDMUSProdNode*>( m_lstNodes.GetNext(pos) );
		pINode->AddRef();

		pINode->GetNodeId( &guidNodeId );

		// Delete sub-directory
		if( ::IsEqualGUID ( guidNodeId, GUID_DirectoryNode ) )
		{
			CDirectoryNode* pDirNode = (CDirectoryNode *)pINode;

			fAllFilesDeleted = pDirNode->DeleteDirectoryToRecycleBin();
		}

		// Delete file
		else if( ::IsEqualGUID ( guidNodeId, GUID_FileNode ) )
		{
			CFileNode* pFileNode = (CFileNode *)pINode;

			CJzNotifyNode* pJzNotifyNode;
			CFileNode* pNotifyFileNode;
			CString strFileName;
			WORD wFlags = 0;

			// Set wFlags to DTN_PROMPT_REFERENCES if pFileNode referenced by a file in another directory
			POSITION pos = pFileNode->m_lstNotifyNodes.GetHeadPosition();
			while( pos )
			{
				pJzNotifyNode = static_cast<CJzNotifyNode*>( pFileNode->m_lstNotifyNodes.GetNext(pos) );
				
				pNotifyFileNode = theApp.GetFileByGUID( pJzNotifyNode->m_guidFile );
				if( pNotifyFileNode )
				{
					pNotifyFileNode->ConstructFileName( strFileName );

					if( strFileName.Find( strDir ) != 0 )
					{
						wFlags = DTN_PROMPT_REFERENCES;
					}

					pNotifyFileNode->Release();
				}
			}
			
			if( pMainFrame->m_wndTreeBar.DeleteTreeNode( pFileNode, wFlags ) == FALSE )
			{
				fAllFilesDeleted = FALSE;
			}

			theApp.DoWaitCursor( TRUE );
		}

		pINode->Release();
	}

	// Delete this Directory
	if( fAllFilesDeleted )
	{
		// Delete directory node
		theApp.m_fDeleteFromTree = TRUE;
		if( SUCCEEDED ( DeleteNode( FALSE ) ) )
		{
			// Delete directory from hard drive
			theApp.DeleteEmptyDirToRecycleBin( strDir );
		}
		theApp.m_fDeleteFromTree = FALSE;
	}

	theApp.DoWaitCursor( FALSE );

	return fAllFilesDeleted;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectoryNode ConstructPath

void CDirectoryNode::ConstructPath( CString& strPath )
{
	strPath.Empty();

	CString strName;
	BSTR bstrName;

	GUID guidNodeId;
	IDMUSProdNode* pINode = this;

	while( pINode )
	{
		strName.Empty();

		if( SUCCEEDED ( pINode->GetNodeId( &guidNodeId ) ) )
		{
			if( IsEqualGUID ( guidNodeId, GUID_ProjectFolderNode ) )
			{
				CProject* pProject = (CProject *)pINode;
				strName = pProject->m_strProjectDir;
			}
			else
			{
				if( SUCCEEDED ( pINode->GetNodeName( &bstrName ) ) )
				{
					strName = bstrName;
					::SysFreeString( bstrName );
				}
			}
		}

		if( !strName.IsEmpty() )
		{
			if( strName.Right(1) != _T("\\") )
			{
				strName += _T("\\");
			}

			strPath = strName + strPath;
		}

		if( pINode != this )
		{
			pINode->Release();
		}

		if( FAILED ( pINode->GetParentNode( &pINode ) ) )
		{
			pINode = NULL;
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDirectoryNode NewFolder

void CDirectoryNode::NewFolder( void )
{
	CString strNewDir;
	CString strNewSubDir;
	CString strNewFolderText;

	// Derive a name for the new folder
	CString strPath;
	ConstructPath( strPath );

	strNewFolderText.LoadString( IDS_NEW_FOLDER );

	BOOL fContinue = TRUE;
	int i = 0;

	while( fContinue )
	{
		if( i > 0 )
		{
			strNewSubDir.Format( "%s %s%d%s", strNewFolderText, _T("("), i, _T(")") );
		}
		else
		{
			strNewSubDir = strNewFolderText;
		}

		strNewDir = strPath + strNewSubDir;

		DWORD dwAttributes = GetFileAttributes( strNewDir );

		if( (dwAttributes == 0xFFFFFFFF)
		|| !(dwAttributes & FILE_ATTRIBUTE_DIRECTORY) )
		{
			// directory does not exist
			fContinue = FALSE;
		}

		i++;
	}

	// Create the new directory node and insert it into the Project Tree
	CDirectoryNode* pDirNode = new CDirectoryNode;

	if( pDirNode )
	{
		pDirNode->m_strName = strNewSubDir;
		InsertChildNode( pDirNode );

		theApp.m_pFramework->EditNodeLabel( pDirNode );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDirectoryNode GetSubDirectoryByName

CDirectoryNode* CDirectoryNode::GetSubDirectoryByName( LPCTSTR szName )
{
	CDirectoryNode* pTheDirNode = NULL;
	CDirectoryNode* pDirNode;
	IDMUSProdNode* pINode;
	GUID guidNodeId;

    POSITION pos = m_lstNodes.GetHeadPosition();

    while( pos )
    {
        pINode = m_lstNodes.GetNext( pos );

		if( SUCCEEDED ( pINode->GetNodeId( &guidNodeId )  ) )
		{
			if( ::IsEqualGUID( guidNodeId, GUID_DirectoryNode ) )
			{
				pDirNode = (CDirectoryNode* )pINode;

				if( pDirNode->m_strName.CompareNoCase( szName ) == 0 )
				{
					pTheDirNode = pDirNode;
					break;
				}
			}
		}
	}

	return pTheDirNode;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectoryNode FindDirNode

CDirectoryNode* CDirectoryNode::FindDirNode( LPCTSTR szPathName )
{
	CDirectoryNode* pTheDirNode = NULL;
	CDirectoryNode* pDirNode;
	IDMUSProdNode* pINode;
	CString strPath;
	GUID guidNodeId;

    POSITION pos = m_lstNodes.GetHeadPosition();
    while( pos )
    {
        pINode = m_lstNodes.GetNext( pos );

		if( SUCCEEDED ( pINode->GetNodeId( &guidNodeId )  ) )
		{
			if( ::IsEqualGUID( guidNodeId, GUID_DirectoryNode ) )
			{
				pDirNode = (CDirectoryNode* )pINode;

				pDirNode->ConstructPath( strPath );

				if( strPath.CompareNoCase( szPathName ) == 0 )
				{
					pTheDirNode = pDirNode;
				}
				else
				{
					pTheDirNode = pDirNode->FindDirNode( szPathName );
				}
			}
		}

		if( pTheDirNode )
		{
			break;
		}
	}

	return pTheDirNode;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectoryNode NotifyAllFiles

void CDirectoryNode::NotifyAllFiles( GUID guidNotification )
{
	GUID guidNodeId;

	POSITION pos = m_lstNodes.GetHeadPosition();
	while( pos != NULL )
	{
		IDMUSProdNode* pINode = static_cast<IDMUSProdNode*>( m_lstNodes.GetNext(pos) );

		pINode->GetNodeId( &guidNodeId );

		// Handle sub-directories
		if( ::IsEqualGUID ( guidNodeId, GUID_DirectoryNode ) )
		{
			CDirectoryNode* pDirNode = (CDirectoryNode *)pINode;

			pDirNode->NotifyAllFiles( guidNotification );
		}

		// Handle files
		else if( ::IsEqualGUID ( guidNodeId, GUID_FileNode ) )
		{
			CFileNode* pFileNode = (CFileNode *)pINode;

			if( pFileNode->m_pIChildNode )
			{
				// Notify connected nodes that filename is about to change
				theApp.m_pFramework->NotifyNodes( pFileNode->m_pIChildNode, guidNotification, NULL );

				// Notify m_pChildNode that filename is about to change
				IDMUSProdNotifySink* pINotifySink;
				if( SUCCEEDED ( pFileNode->m_pIChildNode->QueryInterface( IID_IDMUSProdNotifySink, (void **)&pINotifySink ) ) )
				{
					pINotifySink->OnUpdate( pFileNode->m_pIChildNode, guidNotification, NULL );
					pINotifySink->Release();
				}
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDirectoryNode::SortTree

void CDirectoryNode::SortTree( void )
{
	CMainFrame* pMainFrame = (CMainFrame *)theApp.m_pMainWnd;
	ASSERT( pMainFrame != NULL );

    // Sort all sub-Directory nodes
	POSITION pos = m_lstNodes.GetHeadPosition();
    while( pos )
    {
        IDMUSProdNode* pIDirNode = m_lstNodes.GetNext( pos );

		GUID guidNodeId;
		if( SUCCEEDED ( pIDirNode->GetNodeId( &guidNodeId ) ) )
		{
			if( IsEqualGUID ( guidNodeId, GUID_DirectoryNode ) )
			{
				CDirectoryNode* pDirNode = (CDirectoryNode *)pIDirNode;

				pDirNode->SortTree();
			}
		}
	}

	// Sort this Directory node
	HTREEITEM hItem = theApp.m_pFramework->FindTreeItem( this );
	if( hItem )
	{
		pMainFrame->m_wndTreeBar.SortChildren( hItem );
	}
}
