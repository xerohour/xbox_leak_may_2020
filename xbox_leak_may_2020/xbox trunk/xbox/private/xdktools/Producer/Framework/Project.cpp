// Project.cpp: implementation of the CProject class.
//
//////////////////////////////////////////////////////////////////////

/*-----------
@doc DMUSPROD
-----------*/

#include "stdafx.h"
#include "JazzApp.h"
#include "MainFrm.h"
#include "ioJazzDoc.h"
#include "RuntimeDupeDlg.h"
#include <mmreg.h>
#include "ProjectPropTabGeneral.h"
#include "ProjectPropTabFolders.h"
#include "Timeline.h"
#include "StripMgr.h"
#include "DLSDesigner.h"
#include "SegmentDesigner.h"
#include "CloseProjectDlg.h"

#include <io.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


// {2FD243C0-A4FB-11d0-89AD-00A0C9054129}
const GUID GUID_ProjectFolderNode = 
{ 0x2fd243c0, 0xa4fb, 0x11d0, { 0x89, 0xAD, 0x00, 0xA0, 0xC9, 0x05, 0x41, 0x29 } };

// {ECC6BFC0-C21E-11d0-89AE-00A0C9054129}
static const GUID GUID_ProjectPropPageManager = 
{ 0xECC6BFC0, 0xC21E, 0x11d0, { 0x89, 0xAE, 0x00, 0xA0, 0xC9, 0x05, 0x41, 0x29 } };


short CProjectPropPageManager::sm_nActiveTab = 0;

// BUGBUG: Need to remove these when the API is updated to support
// broadcast PChannels in parts.
#define PRIVATE_PART_BROADCAST_PERFORMANCE	0xFFFF
#define PRIVATE_PART_BROADCAST_SEGMENT		0xFFFE


/////////////////////////////////////////////////////////////////////////////
// CJzRuntimeFolder constructor/destructor 

CJzRuntimeFolder::CJzRuntimeFolder()
{
	m_pDocTemplate = NULL;
}


CJzRuntimeFolder::~CJzRuntimeFolder()
{
}


//////////////////////////////////////////////////////////////////////
// CProjectPropPageManager Construction/Destruction
//////////////////////////////////////////////////////////////////////

CProjectPropPageManager::CProjectPropPageManager()
{
    m_dwRef = 1;
	m_pIPropPageObject = NULL;
	m_pIPropSheet = NULL;

	m_pTabGeneral = NULL;
	m_pTabFolders = NULL;
}

CProjectPropPageManager::~CProjectPropPageManager()
{
	if( m_pIPropSheet )
	{
		m_pIPropSheet->Release();
	}

	if( m_pTabGeneral )
	{
		delete m_pTabGeneral;
	}

	if( m_pTabFolders )
	{
		delete m_pTabFolders;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CProjectPropPageManager::RemoveCurrentObject

void CProjectPropPageManager::RemoveCurrentObject( void )
{
	if( m_pIPropPageObject == NULL )
	{
		return;
	}

	m_pIPropPageObject->OnRemoveFromPageManager();
	m_pIPropPageObject = NULL;
}


/////////////////////////////////////////////////////////////////////////////
// CProjectPropPageManager IUnknown implementation

HRESULT CProjectPropPageManager::QueryInterface( REFIID riid, LPVOID FAR* ppvObj )
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

    if( ::IsEqualIID(riid, IID_IDMUSProdPropPageManager)
	||  ::IsEqualIID(riid, IID_IUnknown) )
    {
        AddRef();
        *ppvObj = (IDMUSProdPropPageManager *)this;
        return S_OK;
    }

    *ppvObj = NULL;
    return E_NOINTERFACE;
}

ULONG CProjectPropPageManager::AddRef()
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

    return ++m_dwRef;
}

ULONG CProjectPropPageManager::Release()
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
// CProjectPropPageManager IDMUSProdPropPageManager implementation

/////////////////////////////////////////////////////////////////////////////
// CProjectPropPageManager IDMUSProdPropPageManager::GetPropertySheetTitle

HRESULT CProjectPropPageManager::GetPropertySheetTitle( BSTR* pbstrTitle, BOOL* pfAddPropertiesText )
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	if( (pbstrTitle == NULL)
	||  (pfAddPropertiesText == NULL) )
	{
		return E_POINTER;
	}

	*pfAddPropertiesText = TRUE;

	CString strTitle;
	strTitle.LoadString( IDS_PROJECT_TEXT );

	CProject *pProject;

	if( m_pIPropPageObject
	&& (SUCCEEDED (m_pIPropPageObject->GetData((void **)&pProject))) )
	{
		strTitle = pProject->m_strName + _T(" ") + strTitle;
	}

	*pbstrTitle = strTitle.AllocSysString();

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CProjectPropPageManager IDMUSProdPropPageManager::GetPropertySheetPages

HRESULT CProjectPropPageManager::GetPropertySheetPages( IDMUSProdPropSheet* pIPropSheet, LONG* hPropSheetPage[], short* pnNbrPages )
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	if( (hPropSheetPage == NULL)
	||  (pnNbrPages == NULL) )
	{
		return E_POINTER;
	}

	if( pIPropSheet == NULL )
	{
		return E_INVALIDARG;
	}

	m_pIPropSheet = pIPropSheet;
	m_pIPropSheet->AddRef();

	hPropSheetPage[0] = NULL;
	*pnNbrPages = 0;

	// Add General tab
	HPROPSHEETPAGE hPage;
	short nNbrPages = 0;

	m_pTabGeneral = new CProjectPropTabGeneral( this );
	if( m_pTabGeneral )
	{
		hPage = ::CreatePropertySheetPage( (LPCPROPSHEETPAGE)&m_pTabGeneral->m_psp );
		if( hPage )
		{
			hPropSheetPage[nNbrPages] = (LONG *)hPage;
			nNbrPages++;
		}			
	}

	// Add Folders tab
	m_pTabFolders = new CProjectPropTabFolders( this );
	if( m_pTabFolders )
	{
		hPage = ::CreatePropertySheetPage( (LPCPROPSHEETPAGE)&m_pTabFolders->m_psp );
		if( hPage )
		{
			hPropSheetPage[nNbrPages] = (LONG *)hPage;
			nNbrPages++;
		}			
	}

	// Set number of pages
	*pnNbrPages = nNbrPages;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CProjectPropPageManager IDMUSProdPropPageManager::OnRemoveFromPropertySheet

HRESULT CProjectPropPageManager::OnRemoveFromPropertySheet()
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	ASSERT( m_pIPropSheet != NULL );
	m_pIPropSheet->GetActivePage( &CProjectPropPageManager::sm_nActiveTab );

	RemoveCurrentObject();
	theApp.m_pIPageManager = NULL;

	Release();	// delete myself

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CProjectPropPageManager IDMUSProdPropPageManager::SetObject

HRESULT CProjectPropPageManager::SetObject( IDMUSProdPropPageObject* pINewPropPageObject )
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	if( pINewPropPageObject == NULL )
	{
		return E_INVALIDARG;
	}

	if( m_pIPropPageObject == pINewPropPageObject )
	{
		return S_OK;
	}

	RemoveCurrentObject();

	m_pIPropPageObject = pINewPropPageObject;
//	m_pIPropPageObject->AddRef();		intentionally missing

	RefreshData();

	if( m_pIPropSheet )
	{
		m_pIPropSheet->RefreshTitle();
		m_pIPropSheet->RefreshActivePage();
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CProjectPropPageManager IDMUSProdPropPageManager::RemoveObject

HRESULT CProjectPropPageManager::RemoveObject( IDMUSProdPropPageObject* pIPropPageObject )
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	if( (pIPropPageObject == NULL)
	||  (pIPropPageObject != m_pIPropPageObject) )
	{
		return E_INVALIDARG;
	}

	RemoveCurrentObject();
	RefreshData();

	if( m_pIPropSheet )
	{
		m_pIPropSheet->RefreshTitle();
		m_pIPropSheet->RefreshActivePage();
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CProjectPropPageManager  IDMUSProdPropPageManager::IsEqualObject

HRESULT FAR EXPORT CProjectPropPageManager::IsEqualObject( IDMUSProdPropPageObject* pIPropPageObject )
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	if( pIPropPageObject == NULL )
	{
		return E_INVALIDARG;
	}

	if( pIPropPageObject == m_pIPropPageObject )
	{
		return S_OK;
	}

	return S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CProjectPropPageManager IDMUSProdPropPageManager::RefreshData

HRESULT CProjectPropPageManager::RefreshData( void )
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	CProject* pProject;
	
	if( m_pIPropPageObject == NULL )
	{
		pProject = NULL;
	}
	else if( FAILED ( m_pIPropPageObject->GetData( (void **)&pProject ) ) )
	{
		return E_FAIL;
	}

	m_pTabGeneral->SetProject( pProject );
	m_pTabFolders->SetProject( pProject );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CProjectPropPageManager  IDMUSProdPropPageManager::IsEqualPageManagerGUID

HRESULT FAR EXPORT CProjectPropPageManager::IsEqualPageManagerGUID( REFGUID rguidPageManager )
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	if( ::IsEqualGUID(rguidPageManager, GUID_ProjectPropPageManager) )
	{
		return S_OK;
	}

	return S_FALSE;
}


//////////////////////////////////////////////////////////////////////
// CProject Construction/Destruction
//////////////////////////////////////////////////////////////////////

CProject::CProject()
{
	m_pProjectDoc = NULL;
	m_pBookmark = NULL;
	CoCreateGuid( &m_guid ); 
}

CProject::~CProject()
{
	IDMUSProdPropSheet* pIPropSheet = theApp.GetPropertySheet();

	if( pIPropSheet )
	{
		pIPropSheet->RemovePageManagerByObject( this );
		pIPropSheet->Release();
	}
	
	// Delete all items from m_lstPChannelNames
	while( !m_lstPChannelNames.IsEmpty() )
	{
		PChannelName *pPChannelName;
		pPChannelName = m_lstPChannelNames.RemoveHead();
		delete pPChannelName;
	}

	CFileNode* pFileNode;

	while( !m_lstFiles.IsEmpty() )
	{
		pFileNode = static_cast<CFileNode*>( m_lstFiles.GetHead() );
		RemoveFile( pFileNode );
	}
	
	CJzRuntimeFolder* pJzRuntimeFolder;

	while( !m_lstRuntimeFolders.IsEmpty() )
	{
		pJzRuntimeFolder = static_cast<CJzRuntimeFolder*>( m_lstRuntimeFolders.RemoveHead() );
		delete pJzRuntimeFolder;
	}

	if( m_pBookmark )
	{
		delete m_pBookmark;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CProject IUnknown overrides

HRESULT CProject::QueryInterface( REFIID riid, LPVOID FAR* ppvObj )
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

    if( ::IsEqualIID(riid, IID_IDMUSProdNode)
	||  ::IsEqualIID(riid, IID_IUnknown) )
    {
        AddRef();
        *ppvObj = (IDMUSProdNode *)this;
        return S_OK;
    }

    if( ::IsEqualIID(riid, IID_IDMUSProdProject) )
    {
        AddRef();
        *ppvObj = (IDMUSProdProject *)this;
        return S_OK;
    }

    if( ::IsEqualIID(riid, IID_IDMUSProdPropPageObject) )
    {
        AddRef();
        *ppvObj = (IDMUSProdPropPageObject *)this;
        return S_OK;
    }

    if( ::IsEqualIID(riid, IID_IDMUSProdPChannelName) )
	{
        AddRef();
        *ppvObj = (IDMUSProdPChannelName *)this;
        return S_OK;
	}

    *ppvObj = NULL;
    return E_NOINTERFACE;
}


ULONG CProject::AddRef()
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

    return ++m_dwRef;
}

ULONG CProject::Release()
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
// CProject IDMUSProdNode overrides

/////////////////////////////////////////////////////////////////////////////
// CProject IDMUSProdNode::GetNodeImageIndex

HRESULT CProject::GetNodeImageIndex( short* pnFirstImage )
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	*pnFirstImage = theApp.m_nFirstImage + FIRST_PROJECT_IMAGE;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CProject IDMUSProdNode::GetDocRootNode

HRESULT CProject::GetDocRootNode( IDMUSProdNode** ppIDocRootNode )
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	AddRef();
	*ppIDocRootNode = (IDMUSProdNode *)this;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CProject IDMUSProdNode::SetDocRootNode

HRESULT CProject::SetDocRootNode( IDMUSProdNode* pIDocRootNode )
{
//	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	return E_NOTIMPL;	// Root node is always 'this'
}


/////////////////////////////////////////////////////////////////////////////
// CProject IDMUSProdNode::GetParentNode

HRESULT CProject::GetParentNode( IDMUSProdNode** ppIParentNode )
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	*ppIParentNode = NULL;	// Project node is always the top node

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CProject IDMUSProdNode::SetParentNode

HRESULT CProject::SetParentNode( IDMUSProdNode* pIParentNode )
{
//	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	return E_NOTIMPL;	// Project node is always the top node
}


/////////////////////////////////////////////////////////////////////////////
// CProject IDMUSProdNode::GetNodeId

HRESULT CProject::GetNodeId( GUID* pguid )
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	if( pguid == NULL )
	{
		return E_POINTER;
	}

	*pguid = GUID_ProjectFolderNode;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CProject IDMUSProdNode::GetNodeNameMaxLength

HRESULT CProject::GetNodeNameMaxLength( short* pnMaxLength )
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	if( pnMaxLength == NULL )
	{
		return E_POINTER;
	}

	*pnMaxLength = MAX_LENGTH_PROJECT_NAME;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CProject IDMUSProdNode::ValidateNodeName

HRESULT CProject::ValidateNodeName( BSTR bstrName )
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	CString strName = bstrName;
	::SysFreeString( bstrName );

	int iBad = strName.FindOneOf( _T("\\/:*?\"<>;|#%.") );
	if( iBad != -1 )
	{
		AfxMessageBox( IDS_ERR_PROJECT_NAME );
		return S_FALSE;
	}

	// Get Project DocTemplate
	CJazzDocTemplate* pDocTemplate = (CJazzDocTemplate *)m_pProjectDoc->GetDocTemplate();
	ASSERT( pDocTemplate != NULL );
	if( pDocTemplate == NULL )
	{
		return S_FALSE;
	}

	// Get file extension used for Projects
	CString strFilterExt;
	pDocTemplate->GetDocString( strFilterExt, CDocTemplate::filterExt );

	// Add Project extension to name
	CString strNameAndExt = strName + strFilterExt;

	// Make sure name plus extension does not exceed max chars
	if( strNameAndExt.GetLength() > MAX_LENGTH_PROJECT_NAME )
	{
		AfxMessageBox( IDS_ERR_NAME_LENGTH );
		return S_FALSE;
	}

	// Determine Project's new directory
	CString strNewDirectory;
	ConstructPath( strNewDirectory );

	int nFindPos = strNewDirectory.ReverseFind( (TCHAR)'\\' );
	if( nFindPos == -1 )
	{
		return S_FALSE;
	}
	strNewDirectory = strNewDirectory.Left( nFindPos );

	nFindPos = strNewDirectory.ReverseFind( (TCHAR)'\\' );
	if( nFindPos == -1 )
	{
		return S_FALSE;
	}
	strNewDirectory = strNewDirectory.Left( nFindPos + 1 );

	strNewDirectory = strNewDirectory + strName;
	strNewDirectory += _T("\\");

	// Determine Project's new filename
	CString strNewFileName = strNewDirectory + strNameAndExt;

	// Make sure path does not exceed max chars
	if( strNewFileName.GetLength() >= _MAX_DIR )
	{
		AfxMessageBox( IDS_ERR_FILENAME_LENGTH );
		return S_FALSE;
	}

	// Determine Project's old directory
	CString strOldDirectory;
	ConstructPath( strOldDirectory );

	// Make sure we aren't trying to rename the current directory
	TCHAR achCurrentDir[_MAX_PATH + 1];
	DWORD dwNbrBytes = ::GetCurrentDirectory( _MAX_PATH, achCurrentDir );
	if( (dwNbrBytes > 0) 
	&&  (dwNbrBytes < _MAX_PATH) )
	{
		CString strCurrentDir = achCurrentDir;
		strCurrentDir +=  + (TCHAR)'\\';

		if( _tcsicmp( strCurrentDir, strOldDirectory ) == 0 )
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

	// Change Project directory to new directory name
	if( theApp.RenameThePath(strOldDirectory, strNewDirectory, strOldDirectory) == FALSE )
	{
		// Notify connected nodes filename change was aborted
		if(	theApp.m_fSendFileNameChangeNotification )
		{
			NotifyAllFiles( FRAMEWORK_AbortFileNameChange );
		}
		return S_FALSE;
	}

	// Determine Project's old filename
	CString strOldFileName = strOldDirectory + m_strName + strFilterExt;

	// Determine Project's current filename
	CString strCurFileName = strNewDirectory + m_strName + strFilterExt;

	// Change Project filename to new filename
	if( theApp.RenameThePath(strCurFileName, strNewFileName, strOldFileName) == FALSE )
	{
		theApp.RenameThePath( strNewDirectory, strOldDirectory, strNewDirectory );
		return S_FALSE;
	}

	// Set the "Project" directory
	TCHAR achPath[FILENAME_MAX];
	TCHAR achDrive[_MAX_DRIVE];
	TCHAR achDir[_MAX_DIR];

	CString strOrigProjectDir = m_strProjectDir;

	_tsplitpath( strNewFileName, achDrive, achDir, NULL, NULL );
	_tmakepath( achPath, achDrive, achDir, NULL, NULL );
	m_strProjectDir = achPath;
	if( m_strProjectDir.Right(1) != _T("\\") )
	{
		m_strProjectDir += _T("\\");
	}

	// Adjust Project Folders
	AdjustProjectFolders( strOrigProjectDir, m_strProjectDir );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CProject IDMUSProdNode::SetNodeName

HRESULT CProject::SetNodeName( BSTR bstrName )
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	ASSERT( m_pProjectDoc != NULL );
	ASSERT( m_pProjectDoc->m_pIDocRootNode != NULL );

	// Change Project name
	m_strName = bstrName;
	::SysFreeString( bstrName );

	// Change text in Project Tree
	HTREEITEM hItem = theApp.m_pFramework->FindTreeItem( m_pProjectDoc->m_pIDocRootNode );
	if( hItem )
	{
		CTreeCtrl* pTreeCtrl = theApp.GetProjectTreeCtrl();
		if( pTreeCtrl )
		{
			// Update label in the Project Tree
			pTreeCtrl->SetItemText( hItem, m_strName );
		}
	}

	// Update path to all open documents
	RenameOpenDocuments();

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CProject IDMUSProdNode::GetRightClickMenuId

HRESULT CProject::GetRightClickMenuId( HINSTANCE* phInstance, UINT* pnMenuId )
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	*phInstance = AfxGetResourceHandle();
	*pnMenuId   = IDM_PROJECT_NODE_RMENU;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CProject IDMUSProdNode::OnRightClickMenuInit

HRESULT CProject::OnRightClickMenuInit( HMENU hMenu )
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	ASSERT( m_pProjectDoc != NULL );

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

		// IDM_SAVE, IDM_DUPLICATE and IDM_CLOSE
		if( m_strName.IsEmpty() )
		{
			menu.EnableMenuItem( IDM_SAVE, (MF_GRAYED | MF_BYCOMMAND) );
			menu.EnableMenuItem( IDM_DUPLICATE, (MF_GRAYED | MF_BYCOMMAND) );
			menu.EnableMenuItem( IDM_CLOSE, (MF_GRAYED | MF_BYCOMMAND) );
		}
		else
		{
			CString strMenuText;

			menu.GetMenuString( IDM_SAVE, strMenuText, MF_BYCOMMAND );
			strMenuText += _T(" ");
			strMenuText += m_strName;
			menu.ModifyMenu( IDM_SAVE, (MF_STRING | MF_BYCOMMAND), IDM_SAVE, strMenuText );
			menu.EnableMenuItem( IDM_SAVE, (MF_ENABLED | MF_BYCOMMAND) );

			AfxFormatString1( strMenuText, IDS_DUPLICATE_PROJECT_RMENU_TEXT, m_strName );
			menu.ModifyMenu( IDM_DUPLICATE, (MF_STRING | MF_BYCOMMAND), IDM_DUPLICATE, strMenuText );
			menu.EnableMenuItem( IDM_DUPLICATE, (MF_ENABLED | MF_BYCOMMAND) );

			menu.GetMenuString( IDM_CLOSE, strMenuText, MF_BYCOMMAND );
			strMenuText += _T(" ");
			strMenuText += m_strName;
			menu.ModifyMenu( IDM_CLOSE, (MF_STRING | MF_BYCOMMAND), IDM_CLOSE, strMenuText );
			menu.EnableMenuItem( IDM_CLOSE, (MF_ENABLED | MF_BYCOMMAND) );
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
// CProject IDMUSProdNode::OnRightClickMenuSelect

HRESULT CProject::OnRightClickMenuSelect( long lCommandId )
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	ASSERT( m_pProjectDoc != NULL );

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

		case IDM_SAVE:
			if( m_pProjectDoc )
			{
				if( m_pProjectDoc->DoFileSave() )
				{
					theApp.SaveAppState();
				}
				hr = S_OK;
			}
			break;

		case IDM_DUPLICATE:
			theApp.SetActiveProject( this );
			theApp.OnFileDuplicateProject();
			hr = S_OK;
			break;

		case IDM_CLOSE:
			if( m_pProjectDoc )
			{
				theApp.m_fCloseProject = TRUE;
				if( CanClose() )
				{
					if( m_pProjectDoc->SaveAndCloseDoc() )
					{
						theApp.SaveAppState();
					}
				}
				theApp.m_fCloseProject = FALSE;
				theApp.CleanUpBookmarks();
				hr = S_OK;
			}
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
			OnShowProperties();
			hr = S_OK;
			break;
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CProject IDMUSProdNode::DeleteNode

HRESULT CProject::DeleteNode( BOOL fPromptUser )
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	// Remove node from Project Tree
	ASSERT( fPromptUser == FALSE );
	theApp.m_pFramework->RemoveNode( this, FALSE );

	if( m_pProjectDoc )
	{
		m_pProjectDoc->OnCloseDocument();		// No Save - Unconditional close!!!!!!!

		CMainFrame* pMainFrame = (CMainFrame *)theApp.m_pMainWnd;
		if( pMainFrame )
		{
			pMainFrame->RecalcLayout( TRUE );
		}
	}

	theApp.CleanUpBookmarks();
	theApp.SaveAppState();

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CProject IDMUSProdNode::CreateDataObject

HRESULT CProject::CreateDataObject( IDataObject** ppIDataObject )
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
// CProject IDMUSProdNode::CanCut

HRESULT CProject::CanCut( void )
{
//	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	return S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CProject IDMUSProdNode::CanCopy

HRESULT CProject::CanCopy( void )
{
//	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	return S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CProject IDMUSProdNode::CanDelete

HRESULT CProject::CanDelete( void )
{
//	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CProject IDMUSProdNode::CanDeleteChildNode

HRESULT CProject::CanDeleteChildNode( IDMUSProdNode* pIChildNode )
{
//	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	// Should not get here!
	ASSERT( 0 );

	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CProject IDMUSProdProject implementation

/*======================================================================================
METHOD:  IDMUSPRODPROJECT::GETNAME
========================================================================================
@method HRESULT | IDMUSProdProject | GetName | Returns the name of a DirectMusic Producer
		<o Project>.

@rvalue S_OK | The name was returned successfully.
@rvalue E_POINTER | The address in <p pbstrName> is not valid.  For example, it may
		be NULL.
@rvalue E_FAIL | An error occurred and the name was not returned.

@ex The following example retrieves the name of a DirectMusic Producer Project: |

	IDMUSProdProject* pIProject;
	CString strName;
	BSTR bstrName;
	
	pIProject->GetName( &bstrName );
	strName = bstrName;
	::SysFreeString( bstrName );

@xref <i IDMUSProdProject>, <i IDMUSProdFramework>, <om IDMUSProdFramework.FindProject>, <om IDMUSProdFramework.GetFirstProject>, <om IDMUSProdFramework.GetNextProject>
--------------------------------------------------------------------------------------*/

/////////////////////////////////////////////////////////////////////////////
// CProject IDMUSProdProject::GetName

HRESULT CProject::GetName(
	BSTR* pbstrName		// @parm [out,retval] Pointer to the caller-allocated variable
						//		that receives a copy of the name.  The caller must free
						//		<p pbstrName> with SysFreeString when it is no longer
						//		needed.
)
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	return GetNodeName( pbstrName );
}


/*======================================================================================
METHOD:  IDMUSPRODPROJECT::GETGUID
========================================================================================
@method HRESULT | IDMUSProdProject | GetGUID | Returns the GUID of a DirectMusic Producer
		<o Project>.

@rvalue S_OK | The GUID was returned successfully.
@rvalue E_POINTER | The address in <p pguid> is not valid.  For example, it may
		be NULL.
@rvalue E_FAIL | An error occurred and the GUID was not returned.

@ex The following example retrieves the GUID of a DirectMusic Producer Project: |

	IDMUSProdProject* pIProject;
	GUID guidProject;
	
	pIProject->GetGUID( &guidProject );

@xref <i IDMUSProdProject>, <i IDMUSProdFramework>, <om IDMUSProdFramework.FindProject>, <om IDMUSProdFramework.GetFirstProject>, <om IDMUSProdFramework.GetNextProject>
--------------------------------------------------------------------------------------*/

/////////////////////////////////////////////////////////////////////////////
// CProject IDMUSProdProject::GetGUID

HRESULT CProject::GetGUID(
	GUID* pguid		// @parm [out,retval] Pointer to the caller-allocated variable
					//		that receives a copy of the Project's GUID.
)
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	if( pguid == NULL )
	{
		return E_POINTER;
	}

	*pguid = m_guid;

	return S_OK;
}


/*======================================================================================
METHOD:  IDMUSPRODPROJECT::GETFIRSTFILEBYDOCTYPE
========================================================================================
@method HRESULT | IDMUSProdProject | GetFirstFileByDocType | Returns an <i IDMUSProdNode> interface
		pointer for the first File <o Node> in the <o Project> Tree whose corresponding
		<o DocType> matches <p pIDocType>. 

@comm
	<p ppIFirstFileNode> is set to NULL if a match is not found.

@rvalue S_OK | The first File Node was returned in <p ppIFirstFileNode>. 
@rvalue S_FALSE | A match was not found. 
@rvalue E_POINTER | The address in <p ppIFirstFileNode> is not valid.  For example, it may
		be NULL.
@rvalue E_FAIL | An error occurred, and the first File Node could not be returned.

@ex The following example : |


@xref <i IDMUSProdProject>, <om IDMUSProdProject.GetNextFileByDocType>, <i IDMUSProdFramework>, <om IDMUSProdFramework.FindProject>, <om IDMUSProdFramework.GetFirstProject>, <om IDMUSProdFramework.GetNextProject>
--------------------------------------------------------------------------------------*/

/////////////////////////////////////////////////////////////////////////////
// CProject IDMUSProdProject::GetFirstFileByDocType

HRESULT CProject::GetFirstFileByDocType(
	IDMUSProdDocType* pIDocType,	// @parm [in] <o DocType> object.
	IDMUSProdNode** ppIFirstFileNode// @parm [out,retval] Address of a variable to receive the 
									//		requested <i IDMUSProdNode> interface.  If an error occurs,
									//		the implementation sets <p ppIFirstFileNode>
									//		to NULL.  On success, the caller is responsible
									//		for calling <om IDMUSProdNode.Release>.
)
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	if( ppIFirstFileNode == NULL )
	{
		return E_POINTER;
	}

	*ppIFirstFileNode = NULL;

	if( pIDocType == NULL )
	{
		return E_INVALIDARG;
	}

	CString strFileName;	
	BSTR bstrFileName;
	IDMUSProdDocType* pIFileDocType;
	CFileNode* pFileNode;
	BOOL fMatchingDocType;

    POSITION pos = m_lstFiles.GetHeadPosition();

    while( pos )
    {
        pFileNode = m_lstFiles.GetNext( pos );

		if( SUCCEEDED ( pFileNode->GetNodeName ( &bstrFileName ) ) )
		{
			strFileName = bstrFileName;
			::SysFreeString( bstrFileName );

			pIFileDocType = theApp.GetDocType( strFileName );

			fMatchingDocType = FALSE;
			if( pIFileDocType )
			{
				if( pIDocType == pIFileDocType )
				{
					fMatchingDocType = TRUE;
				}

				pIFileDocType->Release();
			}

			if( fMatchingDocType )
			{
				pFileNode->AddRef();
				*ppIFirstFileNode = pFileNode;
				break;
			}
		}
    }

	if( *ppIFirstFileNode )
	{
		return S_OK;
	}

    return S_FALSE;
}


/*======================================================================================
METHOD:  IDMUSPRODPROJECT::GETNEXTFILEBYDOCTYPE
========================================================================================
@method HRESULT | IDMUSProdProject | GetNextFileByDocType | Returns an <i IDMUSProdNode> interface
		pointer for the next File <o Node> in the <o Project> Tree whose corresponding
		<o DocType> matches that of <p pIFileNode>.

@comm
	Returns the next File Node located after <p pIFileNode> whose corresponding DocType
	matches that of <p pIFileNode>.

	<p ppINextFileNode> is set to NULL when another match cannot be found.

@rvalue S_OK | The next File Node was returned in <p ppINextFileNode>. 
@rvalue S_FALSE | The end of the list was reached. 
@rvalue E_POINTER | The address in <p ppINextFileNode> is not valid.  For example, it may
		be NULL.
@rvalue E_INVALIDARG | <p pIFileNode> is not valid.  For example, it may be NULL.
@rvalue E_FAIL | An error occurred, and the next File Node could not be returned.

@ex The following example: |


@xref <i IDMUSProdProject>, <om IDMUSProdProject.GetFirstFileByDocType>, <i IDMUSProdFramework>, <om IDMUSProdFramework.FindProject>, <om IDMUSProdFramework.GetFirstProject>, <om IDMUSProdFramework.GetNextProject>
--------------------------------------------------------------------------------------*/

/////////////////////////////////////////////////////////////////////////////
// CProject IDMUSProdProject::GetNextFileByDocType

HRESULT CProject::GetNextFileByDocType(
	IDMUSProdNode* pIFileNode,		// @parm [in] A pointer to the previous File Node.
	IDMUSProdNode** ppINextFileNode	// @parm [out,retval] Address of a variable to receive the 
									//		requested <i IDMUSProdNode> interface.  If an error occurs,
									//		the implementation sets <p ppINextFileNode>
									//		to NULL.  On success, the caller is responsible
									//		for calling <om IDMUSProdNode.Release>.
)
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	if( ppINextFileNode == NULL )
	{
		return E_POINTER;
	}

	*ppINextFileNode = NULL;

	if( pIFileNode == NULL )
	{
		return E_INVALIDARG;
	}

	CString strFileName;	
	BSTR bstrFileName;
	IDMUSProdDocType* pIFileDocType;
	CFileNode* pFileNode;
	BOOL fMatchingDocType;

	// Get IDMUSProdDocType*
	if( FAILED ( pIFileNode->GetNodeName ( &bstrFileName ) ) )
	{
		return E_FAIL;
	}

	strFileName = bstrFileName;
	::SysFreeString( bstrFileName );

	IDMUSProdDocType* pIDocType = theApp.GetDocType( strFileName );
	if( pIDocType == NULL )
	{
		return E_FAIL;
	}

	POSITION pos = m_lstFiles.Find( (CFileNode *)pIFileNode );
	if( pos )
	{
		pFileNode = m_lstFiles.GetNext( pos );
	}

    while( pos )
    {
        pFileNode = m_lstFiles.GetNext( pos );

		if( SUCCEEDED ( pFileNode->GetNodeName ( &bstrFileName ) ) )
		{
			strFileName = bstrFileName;
			::SysFreeString( bstrFileName );

			pIFileDocType = theApp.GetDocType( strFileName );

			fMatchingDocType = FALSE;
			if( pIFileDocType )
			{
				if( pIDocType == pIFileDocType )
				{
					fMatchingDocType = TRUE;
				}

				pIFileDocType->Release();
			}

			if( fMatchingDocType )
			{
				pFileNode->AddRef();
				*ppINextFileNode = pFileNode;
				break;
			}
		}
    }

	pIDocType->Release();

	if( *ppINextFileNode )
	{
		return S_OK;
	}

    return S_FALSE;
}


/*======================================================================================
METHOD:  IDMUSPRODPROJECT::GETFIRSTFILE
========================================================================================
@method HRESULT | IDMUSProdProject | GetFirstFile| Returns an <i IDMUSProdNode> interface
		pointer for the first File <o Node> in the <o Project> Tree.

@comm
	<p ppIFirstFileNode> is set to NULL if the Project does not contain any files.

@rvalue S_OK | The first File Node was returned in <p ppIFirstFileNode>. 
@rvalue S_FALSE | The Project is empty. 
@rvalue E_POINTER | The address in <p ppIFirstFileNode> is not valid.  For example, it may
		be NULL.
@rvalue E_FAIL | An error occurred, and the first File Node could not be returned.

@ex The following example : |


@xref <i IDMUSProdProject>, <om IDMUSProdProject.GetNextFile>, <i IDMUSProdFramework>, <om IDMUSProdFramework.FindProject>, <om IDMUSProdFramework.GetFirstProject>, <om IDMUSProdFramework.GetNextProject>
--------------------------------------------------------------------------------------*/

/////////////////////////////////////////////////////////////////////////////
// CProject IDMUSProdProject::GetFirstFile

HRESULT CProject::GetFirstFile(
	IDMUSProdNode** ppIFirstFileNode// @parm [out,retval] Address of a variable to receive the 
									//		requested <i IDMUSProdNode> interface.  If an error occurs,
									//		the implementation sets <p ppIFirstFileNode>
									//		to NULL.  On success, the caller is responsible
									//		for calling <om IDMUSProdNode.Release>.
)
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	if( ppIFirstFileNode == NULL )
	{
		return E_POINTER;
	}

	*ppIFirstFileNode = NULL;

    POSITION pos = m_lstFiles.GetHeadPosition();
    if( pos )
    {
        CFileNode* pFileNode = m_lstFiles.GetNext( pos );

		pFileNode->AddRef();
		*ppIFirstFileNode = pFileNode;
    }

	if( *ppIFirstFileNode )
	{
		return S_OK;
	}

    return S_FALSE;
}


/*======================================================================================
METHOD:  IDMUSPRODPROJECT::GETNEXTFILE
========================================================================================
@method HRESULT | IDMUSProdProject | GetNextFile| Returns an <i IDMUSProdNode> interface
		pointer for the next File <o Node> in the <o Project> Tree.

@comm
	Returns the next File Node located after <p pIFileNode>.

	<p ppINextFileNode> is set to NULL when <p pIFileNode> is the last file in the Project.

@rvalue S_OK | The next File Node was returned in <p ppINextFileNode>. 
@rvalue S_FALSE | The end of the list was reached. 
@rvalue E_POINTER | The address in <p ppINextFileNode> is not valid.  For example, it may
		be NULL.
@rvalue E_INVALIDARG | <p pIFileNode> is not valid.  For example, it may be NULL.
@rvalue E_FAIL | An error occurred, and the next File Node could not be returned.

@ex The following example: |


@xref <i IDMUSProdProject>, <om IDMUSProdProject.GetFirstFile>, <i IDMUSProdFramework>, <om IDMUSProdFramework.FindProject>, <om IDMUSProdFramework.GetFirstProject>, <om IDMUSProdFramework.GetNextProject>
--------------------------------------------------------------------------------------*/

/////////////////////////////////////////////////////////////////////////////
// CProject IDMUSProdProject::GetNextFile

HRESULT CProject::GetNextFile(
	IDMUSProdNode* pIFileNode,		// @parm [in] A pointer to the previous File Node.
	IDMUSProdNode** ppINextFileNode	// @parm [out,retval] Address of a variable to receive the 
									//		requested <i IDMUSProdNode> interface.  If an error occurs,
									//		the implementation sets <p ppINextFileNode>
									//		to NULL.  On success, the caller is responsible
									//		for calling <om IDMUSProdNode.Release>.
)
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	if( ppINextFileNode == NULL )
	{
		return E_POINTER;
	}

	*ppINextFileNode = NULL;

	if( pIFileNode == NULL )
	{
		return E_INVALIDARG;
	}

	POSITION pos = m_lstFiles.Find( (CFileNode *)pIFileNode );
	if( pos )
	{
		CFileNode* pFileNode = m_lstFiles.GetNext( pos );

		if( pos )
		{
			pFileNode = m_lstFiles.GetNext( pos );

			pFileNode->AddRef();
			*ppINextFileNode = pFileNode;
		}
	}

	if( *ppINextFileNode )
	{
		return S_OK;
	}

    return S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CProject IDMUSProdPropPageObject implementation

/////////////////////////////////////////////////////////////////////////////
// CProject IDMUSProdPropPageObject::GetData

HRESULT CProject::GetData( void** ppData )
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	if( ppData == NULL )
	{
		return E_POINTER;
	}

	*ppData = this;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CProject IDMUSProdPropPageObject::SetData

HRESULT CProject::SetData( void* pData )
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CProject IDMUSProdPropPageObject::OnRemoveFromPageManager

HRESULT CProject::OnRemoveFromPageManager( void )
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CProject IDMUSProdPropPageObject::OnShowProperties

HRESULT CProject::OnShowProperties( void )
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	// Get the Project page manager
	CProjectPropPageManager* pPageManager;

	if( theApp.m_pIPageManager
	&&  theApp.m_pIPageManager->IsEqualPageManagerGUID( GUID_ProjectPropPageManager ) == S_OK )
	{
		pPageManager = (CProjectPropPageManager *)theApp.m_pIPageManager;
	}
	else
	{
		pPageManager = new CProjectPropPageManager();
	}

	if( pPageManager == NULL )
	{
		return E_FAIL;
	}

	// Show the Project properties
	IDMUSProdPropSheet* pIPropSheet = theApp.GetPropertySheet();
	if( pIPropSheet )
	{
		short nActiveTab = CProjectPropPageManager::sm_nActiveTab;

		if( SUCCEEDED ( pIPropSheet->SetPageManager(pPageManager) ) )
		{
			theApp.m_pIPageManager = pPageManager;
			pPageManager->SetObject( this );
			pIPropSheet->SetActivePage( nActiveTab ); 
		}

		pIPropSheet->Show( TRUE );
		pIPropSheet->Release();
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CProject IDMUSProdPChannelName implementation

/////////////////////////////////////////////////////////////////////////////
// CProject IDMUSProdPChannelName::GetPChannelName

HRESULT CProject::GetPChannelName( DWORD dwPChannel, WCHAR* pwszName )
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	if( pwszName == NULL )
	{
		return E_POINTER;
	}

	// Handle the performance broadcast PChannel
	if( (dwPChannel == DMUS_PCHANNEL_BROADCAST_PERFORMANCE)
	||	(dwPChannel == PRIVATE_PART_BROADCAST_PERFORMANCE) )
	{
		CString strName;
		if( strName.LoadString( IDS_PCHANNEL_PERFBROADCAST ) )
		{
			if( MultiByteToWideChar( CP_ACP, 0, strName, -1, pwszName, (size_t)DMUS_MAX_NAME ) )
			{
				return S_OK;
			}
		}
		return E_FAIL;
	}
	// Handle the segment broadcast PChannel
	else if( (dwPChannel == DMUS_PCHANNEL_BROADCAST_SEGMENT)
	||	(dwPChannel == PRIVATE_PART_BROADCAST_SEGMENT) )
	{
		CString strName;
		if( strName.LoadString( IDS_PCHANNEL_SEGBROADCAST ) )
		{
			if( MultiByteToWideChar( CP_ACP, 0, strName, -1, pwszName, (size_t)DMUS_MAX_NAME ) )
			{
				return S_OK;
			}
		}
		return E_FAIL;
	}
	// Handle the group broadcast PChannel
	else if( dwPChannel == DMUS_PCHANNEL_BROADCAST_GROUPS )
	{
		CString strName;
		if( strName.LoadString( IDS_PCHANNEL_GROUPBROADCAST ) )
		{
			if( MultiByteToWideChar( CP_ACP, 0, strName, -1, pwszName, (size_t)DMUS_MAX_NAME ) )
			{
				return S_OK;
			}
		}
		return E_FAIL;
	}
	// Handle the AudioPath broadcast PChannel
	else if( dwPChannel == DMUS_PCHANNEL_BROADCAST_AUDIOPATH )
	{
		CString strName;
		if( strName.LoadString( IDS_PCHANNEL_APATHBROADCAST ) )
		{
			if( MultiByteToWideChar( CP_ACP, 0, strName, -1, pwszName, (size_t)DMUS_MAX_NAME ) )
			{
				return S_OK;
			}
		}
		return E_FAIL;
	}

	if( dwPChannel > 31 )
	{
		PChannelName *pPChannelName;

		// Iterate through the list of names
		POSITION pos;
		pos = m_lstPChannelNames.GetHeadPosition();
		while( pos != NULL )
		{
			pPChannelName = m_lstPChannelNames.GetNext( pos );

			// If we found a match, copy it to pwstrName
			if( pPChannelName->m_dwPChannel == dwPChannel )
			{
				if( MultiByteToWideChar( CP_ACP, 0, pPChannelName->m_strName, -1, pwszName, (size_t)DMUS_MAX_NAME ) )
				{
					return S_OK;
				}
				return E_FAIL;
			}
		}
	}
	else
	{
		if( !m_aPChannelNames[dwPChannel].IsEmpty() )
		{
			if( MultiByteToWideChar( CP_ACP, 0, m_aPChannelNames[dwPChannel], -1, pwszName, (size_t)DMUS_MAX_NAME ) )
			{
				return S_OK;
			}
			return E_FAIL;
		}
	}

	CString strEmpty;
	strEmpty.Format("%d", dwPChannel + 1);
	if( MultiByteToWideChar( CP_ACP, 0, strEmpty, -1, pwszName, (size_t)DMUS_MAX_NAME ) )
	{
		return S_FALSE;
	}

	return E_FAIL;
}


/////////////////////////////////////////////////////////////////////////////
// CProject IDMUSProdPChannelName::SetPChannelName

HRESULT CProject::SetPChannelName( DWORD dwPChannel, WCHAR* pwszName )
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	if( pwszName == NULL )
	{
		return E_POINTER;
	}

	if( (dwPChannel >= DMUS_PCHANNEL_BROADCAST_GROUPS)
	||	(dwPChannel == PRIVATE_PART_BROADCAST_PERFORMANCE)
	||	(dwPChannel == PRIVATE_PART_BROADCAST_SEGMENT) )
	{
		return E_INVALIDARG;
	}

	CString strNewName = pwszName;
	if( strNewName.GetLength() > DMUS_MAX_NAME - 1 )
	{
		strNewName = strNewName.Left( DMUS_MAX_NAME - 1 );
	}

	BOOL fRedrawWindows = FALSE;

	if( dwPChannel > 31 )
	{
		PChannelName* pPChannelName;
		BOOL fDone = FALSE;

		// Iterate through the list of names
		POSITION pos, pos2;
		pos = m_lstPChannelNames.GetHeadPosition();
		while( pos )
		{
			pos2 = pos;
			pPChannelName = m_lstPChannelNames.GetNext( pos );

			if( pPChannelName->m_dwPChannel == dwPChannel )
			{
				// Found a matching PChannel, so update the name if it has changed
				if( pPChannelName->m_strName.Compare( strNewName ) != 0 )
				{
					pPChannelName->m_strName = strNewName;
					fRedrawWindows = TRUE;
				}
				fDone = TRUE;
				break;
			}
			else if( pPChannelName->m_dwPChannel > dwPChannel )
			{
				// This is a new PChannel that needs inserted in the middle of the list
				pPChannelName = new PChannelName;
				if( pPChannelName )
				{
					pPChannelName->m_dwPChannel = dwPChannel;
					pPChannelName->m_strName = strNewName;
					fRedrawWindows = TRUE;

					m_lstPChannelNames.InsertBefore( pos2, pPChannelName );
				}
				fDone = TRUE;
				break;
			}
		}

		if( fDone == FALSE )
		{
			pPChannelName = new PChannelName;
			if( pPChannelName )
			{
				pPChannelName->m_dwPChannel = dwPChannel;
				pPChannelName->m_strName = strNewName;
				fRedrawWindows = TRUE;

				m_lstPChannelNames.AddTail( pPChannelName );
			}
		}
	}
	else
	{
		// Update the name if it has changed
		if( m_aPChannelNames[dwPChannel].Compare( strNewName ) != 0 )
		{
			m_aPChannelNames[dwPChannel] = strNewName;
			fRedrawWindows = TRUE;
		}
	}

	// Update all views and toolbars
	if( fRedrawWindows )
	{
		CDocTemplate* pTemplate;
		CComponentDoc* pDocument;

		// No doc manager - no templates
		if( theApp.m_pDocManager )
		{
			// Walk all templates in the application
			POSITION pos = theApp.m_pDocManager->GetFirstDocTemplatePosition();
			while( pos )
			{
				pTemplate = theApp.m_pDocManager->GetNextDocTemplate( pos );
				ASSERT_VALID( pTemplate );
				ASSERT_KINDOF( CDocTemplate, pTemplate );

				// Walk all documents in the template
				POSITION pos2 = pTemplate->GetFirstDocPosition();
				while( pos2 )
				{
					pDocument = (CComponentDoc *)pTemplate->GetNextDoc( pos2 );
					ASSERT_VALID( pDocument );

					if( pDocument->IsKindOf( RUNTIME_CLASS(CComponentDoc) ) )
					{
						pDocument->UpdateAllViews( NULL, 1 );
					}
				}
			}
		}

		theApp.m_pFramework->RedrawClientToolbars();
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CProject Duplicate

BOOL CProject::Duplicate( LPCTSTR pszTargetDir )
{
	CFileNode* pFileNode;
	CString strFileName;
	CString strNewFileName;
	CString strRelativePath;
	BOOL fCloseFile; 
    
	POSITION pos = m_lstFiles.GetHeadPosition();
    while( pos )
    {
        pFileNode = m_lstFiles.GetNext( pos );

		// Determine the original filename
		pFileNode->ConstructFileName( strFileName );

		// Determine the new filename
		pFileNode->ConstructRelativePath( strRelativePath );
		if( !strRelativePath.IsEmpty() )
		{
			if( strRelativePath.Right(1) != _T("\\") )
			{
				strRelativePath += _T("\\");
			}
		}
		strNewFileName = pszTargetDir + strRelativePath + pFileNode->m_strName;

		// Make sure the path exists
		if( theApp.CreateTheDirectory( pszTargetDir ) )
		{
			if( pFileNode->IsProducerFile() )	
			{
				// Save Producer file
				fCloseFile = FALSE;
				
				if( pFileNode->m_pComponentDoc == NULL )
				{
					// Open the file
					theApp.m_nShowNodeInTree++;
					theApp.OpenTheFile( strFileName, TGT_FILENODE );
					theApp.m_nShowNodeInTree--;

					// Will need to close file after  save
					fCloseFile = TRUE;
				}

				if( pFileNode->m_pComponentDoc )
				{
					// Do the save, but don't alter the document's original filename
					pFileNode->m_pComponentDoc->DoSave( strNewFileName, FALSE );

					// close the file
					if( fCloseFile )
					{
						// Remove association with DocRoot before DeleteNode() 
						ASSERT( pFileNode->m_pComponentDoc->m_pIDocRootNode != NULL );
						pFileNode->m_pComponentDoc->m_pIDocRootNode->Release();
						pFileNode->m_pComponentDoc->m_pIDocRootNode = NULL;

						// Remove DocRoot node from Project Tree
						ASSERT( pFileNode->m_pIChildNode != NULL );
						pFileNode->m_pIChildNode->DeleteNode( FALSE );
						pFileNode->m_pIChildNode->Release();
						pFileNode->m_pIChildNode = NULL;
						pFileNode->m_hChildItem = NULL;

						// Close the document
						pFileNode->m_pComponentDoc->OnCloseDocument();
					}
				}
			}
			else
			{
				// Copy non-Producer file
				theApp.CopyTheFile( strFileName, strNewFileName, FALSE );
			}
		}

		theApp.m_pFramework->StepProgressBar( theApp.m_hKeyProgressBar );
	}

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CProject DetermineDefaultRuntimeFolders

void CProject::DetermineDefaultRuntimeFolders( void )
{
	IDMUSProdDocType* pIDocType;
	IDMUSProdDocType* pINextDocType;
	CJazzDocTemplate* pDocTemplate;
	CJzRuntimeFolder* pJzRuntimeFolder;
	POSITION pos;
	CString strFilterExt;
	BOOL fExists;

    HRESULT hr = theApp.m_pFramework->GetFirstDocType( &pINextDocType );

	while( SUCCEEDED( hr )  &&  pINextDocType )
    {
		pIDocType = pINextDocType;

		pDocTemplate = theApp.FindDocTemplate( pIDocType );
		if( pDocTemplate )
		{
			if( pDocTemplate->GetDocString( strFilterExt, CDocTemplate::filterExt )
			&& !strFilterExt.IsEmpty() )
			{
				ASSERT( strFilterExt[0] == '.' );

				// Make sure there is not already an entry for this template
				fExists = FALSE;

				pos = m_lstRuntimeFolders.GetHeadPosition();
				while( pos )
				{
					pJzRuntimeFolder = m_lstRuntimeFolders.GetNext(pos);
					if( pJzRuntimeFolder->m_strFilterExt.CompareNoCase( strFilterExt ) == 0 )
					{
						fExists = TRUE;
						break;
					}
				}

				// Create a new item for the list of default runtime folders
				if( fExists == FALSE )
				{
					pJzRuntimeFolder = new CJzRuntimeFolder;
					if( pJzRuntimeFolder )
					{
						pJzRuntimeFolder->m_strFilterExt = strFilterExt;
						pJzRuntimeFolder->m_strRuntimeFolder = m_strRuntimeDir;
						pJzRuntimeFolder->m_pDocTemplate = pDocTemplate;

						m_lstRuntimeFolders.AddTail( pJzRuntimeFolder );
					}
				}
			}
		}

	    hr = theApp.m_pFramework->GetNextDocType( pIDocType, &pINextDocType );
		pIDocType->Release();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CProject SyncFilesWithDisk

void CProject::SyncFilesWithDisk( IDMUSProdNode* pIParentNode, CString& strPath )
{
	CString strFileName;
	WIN32_FIND_DATA	FindData;
	TCHAR achFileMask[_MAX_PATH + 1];

	if( strPath.Right(1) != _T('\\') )
	{
		wsprintf( achFileMask, "%s\\*.*", strPath );
	}
	else
	{
		wsprintf( achFileMask, "%s*.*", strPath );
	}

	HANDLE hFind = FindFirstFile( achFileMask, &FindData );
	if( hFind != INVALID_HANDLE_VALUE )
	{
		do
		{
			if( !_tcsicmp(FindData.cFileName, _T("."))
			||  !_tcsicmp(FindData.cFileName, _T("..")) )
			{
				continue;
			}

			if( (FindData.dwFileAttributes & FILE_ATTRIBUTE_TEMPORARY)
			||  (FindData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN)
			||  (FindData.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM) )
			{
				continue;
			}

			if( FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
			{
				// Bypass Runtime files directory
				CString strRuntimeDirText;
				strRuntimeDirText.LoadString( IDS_RUNTIME_DIR );
				if( _tcsicmp(FindData.cFileName, strRuntimeDirText) == 0 )
				{
					continue;
				}

				// Make sure CDirectoryNode exists
				CDirectoryNode* pDirNode = new CDirectoryNode;

				// Handle proper initialization of CDirectoryNode
				if( pDirNode )
				{
					// Set node name
					pDirNode->m_strName = FindData.cFileName;

					// Set parent node
					pDirNode->SetParentNode( pIParentNode );

					// Add directory to CDirectoryNode node list
					CDirectoryNode* pParentDirNode = (CDirectoryNode *)pIParentNode;
					pDirNode->AddRef();
					pParentDirNode->m_lstNodes.AddTail( pDirNode );

					// Process contents of directory
					CString strPath;
					pDirNode->ConstructPath( strPath );
					ASSERT( !strPath.IsEmpty() );
					SyncFilesWithDisk( pDirNode, strPath );
				}
			}
			else
			{
				// Bypass project file
				CJazzDocTemplate* pTemplate = theApp.FindProjectDocTemplate();
				if( pTemplate )
				{
					CDocument* pDocument;
					if( pTemplate->MatchDocType(FindData.cFileName, pDocument) == CDocTemplate::yesAttemptNative )
					{
						continue;
					}
				}

				// Determine full path filename
				strFileName = strPath;
				if( strFileName.Right(1) != _T('\\') )
				{
					strFileName += _T('\\');
				}
				strFileName += FindData.cFileName;

				// Make sure CFileNode exists
				CFileNode* pFileNode = FindFileNode( strFileName );
				if( pFileNode == NULL )
				{
					pFileNode = new CFileNode;
					if( pFileNode )
					{
						// Set flags
						pFileNode->m_wFlags |= FSF_DOSYNC;

						// Set parent node
						pFileNode->SetParentNode( pIParentNode );

						// Add file to CProject file list
						AddFile( pFileNode );
					}
				}

				// Handle proper initialization of FileNode
				if( pFileNode )
				{
					// Set node name
					strFileName = FindData.cFileName;
					BSTR bstrFileName = strFileName.AllocSysString();
					pFileNode->SetNodeName( bstrFileName );

					// Set parent node (pre-existing CFileNodes may change their parent)
					pFileNode->SetParentNode( pIParentNode );

					// Set flags
					pFileNode->m_wFlags |= FSF_EXISTS;

					// Add file to CDirectoryNode node list
					CDirectoryNode* pParentDirNode = (CDirectoryNode *)pIParentNode;
					pFileNode->AddRef();
					pParentDirNode->m_lstNodes.AddTail( pFileNode );

					// Use file GUID from new Project if in the middle of 'Duplicate Project'
					if( theApp.m_fInDuplicateProject )
					{
						GUID guidNewFile;
						CString strRelativePathName;

						pFileNode->ConstructRelativePath( strRelativePathName );
						if( !strRelativePathName.IsEmpty() )
						{
							if( strRelativePathName.Right(1) != _T("\\") )
							{
								strRelativePathName += _T("\\");
							}
						}
						strRelativePathName += pFileNode->m_strName;

						if( theApp.GetNewGUIDForDuplicateFile( strRelativePathName, &guidNewFile ) )
						{
							memcpy( &pFileNode->m_guid, &guidNewFile, sizeof( pFileNode->m_guid ) );
						}
					}
				}
			}
		}
		while( FindNextFile( hFind, &FindData ) );

		FindClose( hFind );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CProject FindNestedProjects

void CProject::FindNestedProjects( CString& strPath, CString& strNestedProjects )
{
	WIN32_FIND_DATA	FindData;
	TCHAR achFileMask[_MAX_PATH + 1];

	if( strPath.Right(1) != _T('\\') )
	{
		wsprintf( achFileMask, "%s\\*.*", strPath );
	}
	else
	{
		wsprintf( achFileMask, "%s*.*", strPath );
	}

	HANDLE hFind = FindFirstFile( achFileMask, &FindData );
	if( hFind != INVALID_HANDLE_VALUE )
	{
		do
		{
			if( !_tcsicmp(FindData.cFileName, _T("."))
			||  !_tcsicmp(FindData.cFileName, _T("..")) )
			{
				continue;
			}

			if( (FindData.dwFileAttributes & FILE_ATTRIBUTE_TEMPORARY)
			||  (FindData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN)
			||  (FindData.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM) )
			{
				continue;
			}

			if( FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
			{
				// Bypass Runtime files directory
				CString strRuntimeDirText;
				strRuntimeDirText.LoadString( IDS_RUNTIME_DIR );
				if( _tcsicmp(FindData.cFileName, strRuntimeDirText) == 0 )
				{
					continue;
				}

				// See if this directory contains a Producer Project
				WIN32_FIND_DATA	FindData_ProjectFile;
				TCHAR achProjectFile[_MAX_PATH + 1];

				wsprintf( achProjectFile, "%s%s\\%s.pro", strPath, FindData.cFileName, FindData.cFileName );

				HANDLE hFind_ProjectFile = FindFirstFile( achProjectFile, &FindData_ProjectFile );
				if( hFind_ProjectFile != INVALID_HANDLE_VALUE )
				{
					// Directory contains a Producer Project
					if( strNestedProjects.IsEmpty() == FALSE )
					{
						strNestedProjects += _T(", "); 
					}
					strNestedProjects += _T("'");
					strNestedProjects += FindData.cFileName;
					strNestedProjects += _T("'");

					FindClose( hFind_ProjectFile );
				}

				// Process contents of this subdirectory
				CString strPathSubDir;
				strPathSubDir = strPath;
				strPathSubDir += FindData.cFileName;
				strPathSubDir += _T("\\");
				FindNestedProjects( strPathSubDir, strNestedProjects );
			}
		}
		while( FindNextFile( hFind, &FindData ) );

		FindClose( hFind );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CProject DeleteNestedProjectFiles

void CProject::DeleteNestedProjectFiles( CString& strPath )
{
	WIN32_FIND_DATA	FindData;
	TCHAR achFileMask[_MAX_PATH + 1];

	if( strPath.Right(1) != _T('\\') )
	{
		wsprintf( achFileMask, "%s\\*.*", strPath );
	}
	else
	{
		wsprintf( achFileMask, "%s*.*", strPath );
	}

	HANDLE hFind = FindFirstFile( achFileMask, &FindData );
	if( hFind != INVALID_HANDLE_VALUE )
	{
		do
		{
			if( !_tcsicmp(FindData.cFileName, _T("."))
			||  !_tcsicmp(FindData.cFileName, _T("..")) )
			{
				continue;
			}

			if( (FindData.dwFileAttributes & FILE_ATTRIBUTE_TEMPORARY)
			||  (FindData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN)
			||  (FindData.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM) )
			{
				continue;
			}

			if( FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
			{
				// Bypass Runtime files directory
				CString strRuntimeDirText;
				strRuntimeDirText.LoadString( IDS_RUNTIME_DIR );
				if( _tcsicmp(FindData.cFileName, strRuntimeDirText) == 0 )
				{
					continue;
				}

				// See if this directory contains a Producer Project
				WIN32_FIND_DATA	FindData_ProjectFile;
				TCHAR achProjectFile[_MAX_PATH + 1];

				wsprintf( achProjectFile, "%s%s\\%s.pro", strPath, FindData.cFileName, FindData.cFileName );

				HANDLE hFind_ProjectFile = FindFirstFile( achProjectFile, &FindData_ProjectFile );
				if( hFind_ProjectFile != INVALID_HANDLE_VALUE )
				{
					// Delete the *.pro file
					theApp.DeleteFileToRecycleBin( achProjectFile );

					FindClose( hFind_ProjectFile );
				}

				// Process contents of this subdirectory
				CString strPathSubDir;
				strPathSubDir = strPath;
				strPathSubDir += FindData.cFileName;
				strPathSubDir += _T("\\");
				DeleteNestedProjectFiles( strPathSubDir );
			}
		}
		while( FindNextFile( hFind, &FindData ) );

		FindClose( hFind );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CProject DeleteNonExistentFiles

void CProject::DeleteNonExistentFiles( void )
{
	// Remove files that were deleted through Explorer
	// and are no longer on disk
	// Remove runtime files that are already linked to
	// a design file

	CFileNode* pFileNode;
	
	POSITION pos = m_lstFiles.GetHeadPosition();

	while( pos != NULL )
	{
		pFileNode = static_cast<CFileNode*>( m_lstFiles.GetNext(pos) );

		if( !(pFileNode->m_wFlags & FSF_EXISTS) )
		{
			RemoveFile( pFileNode );
		}
		else
		{
			// Bypass runtime files already linked to a design file
			IDMUSProdDocType* pIDocType = theApp.GetDocType( pFileNode->m_strName );
			if( pIDocType )
			{
				// This is a Producer file
				pIDocType->Release();
				pIDocType = NULL;

				CString strRuntimeFileName;
				CString strPathName;
				
				theApp.AdjustFileName( FT_RUNTIME, pFileNode->m_strName, strRuntimeFileName );
				if( strRuntimeFileName.CompareNoCase( pFileNode->m_strName ) == 0 )
				{
					BOOL fLinked = FALSE;

					// This is a runtime file
					pFileNode->ConstructFileName( strPathName );
					CFileNode* pDesignFileNode = GetFileByRuntimeFileName( strPathName, NULL, FALSE );
					if( pDesignFileNode )
					{
						if( pDesignFileNode->m_wFlags & FSF_EXISTS )
						{
							// This runtime file is already linked to a design file
							fLinked = TRUE;
						}

						pDesignFileNode->Release();
					}

					if( fLinked == FALSE )
					{
						CString strDesignFileName;

						theApp.AdjustFileName( FT_DESIGN, strPathName, strDesignFileName );
						pDesignFileNode = theApp.FindFileNode( strDesignFileName );
						if( pDesignFileNode )
						{
							if( pDesignFileNode->m_strRuntimeFile.IsEmpty() )
							{
								pDesignFileNode->LinkToRuntimeFile( strPathName );
								fLinked = TRUE;
							}
						}
					}

					if( fLinked )
					{
						// A design file is linked to this runtime file
						pFileNode->m_pIParentNode->DeleteChildNode( pFileNode, FALSE );
					}
				}
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CProject SyncListInfo

void CProject::SyncListInfo( void )
{
	// Sync the DMUSProdListInfo of all files in the Project
	CFileNode* pFileNode;
	
	POSITION pos = m_lstFiles.GetHeadPosition();

	while( pos != NULL )
	{
		pFileNode = static_cast<CFileNode*>( m_lstFiles.GetNext(pos) );
		pFileNode->SyncListInfo();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CProject  DeleteAllFilesToRecycleBin

BOOL CProject::DeleteAllFilesToRecycleBin( short nWhichFiles, BOOL fRuntimePrompt )
{
	CFileNode* pFileNode;
	CString strFileName;
	CString strDir;
	CString strMsg;
	int nFindPos;
	int nAnswer;

	theApp.DoWaitCursor( TRUE );

	CMainFrame* pMainFrame = (CMainFrame *)theApp.m_pMainWnd;
	ASSERT( pMainFrame != NULL );

	// Start progress bar
	int nNbrFiles = GetNbrFiles( GNF_NOFLAGS );
	strMsg.LoadString( IDS_DELETING_PROJECT );
	BSTR bstrMsg = strMsg.AllocSysString();
	HANDLE hKey;
	if( SUCCEEDED ( theApp.m_pFramework->StartProgressBar( 0, nNbrFiles+1, bstrMsg, &hKey ) ) )
	{
		theApp.m_hKeyProgressBar = hKey;
	}
	theApp.m_pFramework->SetProgressBarPos( theApp.m_hKeyProgressBar, 0 );
	theApp.m_pFramework->SetProgressBarStep( theApp.m_hKeyProgressBar, 1 );

	BOOL fAllFilesDeleted = TRUE;
	
	// Delete each file in the project
	POSITION pos = m_lstFiles.GetHeadPosition();
	while( pos != NULL )
	{
		pFileNode = static_cast<CFileNode*>( m_lstFiles.GetNext(pos) );
		pFileNode->AddRef();

		// Delete "Design" file
		pFileNode->ConstructFileName( strFileName );
		if( strFileName.CompareNoCase( pFileNode->m_strRuntimeFile ) != 0 )
		{
			CJzNotifyNode* pJzNotifyNode;
			CFileNode* pNotifyFileNode;
			WORD wFlags = 0;

			// Set wFlags to DTN_PROMPT_REFERENCES if pFileNode referenced by a file in another Project
			POSITION pos = pFileNode->m_lstNotifyNodes.GetHeadPosition();
			while( pos )
			{
				pJzNotifyNode = static_cast<CJzNotifyNode*>( pFileNode->m_lstNotifyNodes.GetNext(pos) );
				
				pNotifyFileNode = theApp.GetFileByGUID( pJzNotifyNode->m_guidFile );
				if( pNotifyFileNode )
				{
					if( pNotifyFileNode->m_pProject != pFileNode->m_pProject )
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

		// Delete "Runtime" file
		if( nWhichFiles == DPF_ALL_FILES )
		{
			if( !pFileNode->m_strRuntimeFile.IsEmpty() )
			{
				nAnswer = IDOK;

				if( fRuntimePrompt )
				{
					AfxFormatString1( strMsg, IDS_DELETE_FILE, pFileNode->m_strRuntimeFile );
					nAnswer = AfxMessageBox( strMsg, MB_OKCANCEL );
					theApp.DoWaitCursor( TRUE );
				}

				if( nAnswer == IDOK )
				{
					theApp.DeleteFileToRecycleBin( pFileNode->m_strRuntimeFile );

					// See if "Runtime" directory can be deleted
					nFindPos = pFileNode->m_strRuntimeFile.ReverseFind( (TCHAR)'\\' );
					if( nFindPos != -1 )
					{
						strDir = pFileNode->m_strRuntimeFile.Left( nFindPos + 1 );
						theApp.DeleteEmptyDirToRecycleBin( strDir );
					}
				}
			}
		}

		pFileNode->Release();
		theApp.m_pFramework->StepProgressBar( theApp.m_hKeyProgressBar );
	}

	// Delete "Project" file
	if( fAllFilesDeleted )
	{
		CString strFilterExt;

		CJazzDocTemplate* pTemplate = (CJazzDocTemplate *)m_pProjectDoc->GetDocTemplate();
		ASSERT( pTemplate != NULL );
		pTemplate->GetDocString( strFilterExt, CDocTemplate::filterExt );

		theApp.m_fDeleteFromTree = TRUE;
		if( SUCCEEDED ( DeleteNode( FALSE ) ) )
		{
			strFileName = m_strProjectDir + m_strName + strFilterExt;
			theApp.DeleteFileToRecycleBin( strFileName );
		}
		theApp.m_fDeleteFromTree = FALSE;

		// See if "Project" directory can be deleted
		nFindPos = strFileName.ReverseFind( (TCHAR)'\\' );
		if( nFindPos != -1 )
		{
			strDir = strFileName.Left( nFindPos + 1 );
			theApp.DeleteEmptyDirToRecycleBin( strDir );
		}
	}

	// Remove progress bar
	theApp.m_pFramework->SetProgressBarPos( theApp.m_hKeyProgressBar, nNbrFiles+1 );
	Sleep( 10 );
	theApp.m_pFramework->EndProgressBar( theApp.m_hKeyProgressBar );

	theApp.DoWaitCursor( FALSE );

	return fAllFilesDeleted;
}


/////////////////////////////////////////////////////////////////////////////
// CProject AddFile

void CProject::AddFile( CFileNode* pFileNode )
{
	// Must have parent node set prior to placing
	// CFileNode in the Project's list of files
	IDMUSProdNode* pIParentNode;

	if( SUCCEEDED ( pFileNode->GetParentNode( &pIParentNode ) ) )
	{
		pIParentNode->Release();
	}

	if( pFileNode->m_pProject == NULL )
	{
		pFileNode->AddRef();
		pFileNode->m_pProject = this;
		m_lstFiles.AddTail( pFileNode );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CProject RenameOpenDocuments

void CProject::RenameOpenDocuments( void )
{
	CFileNode* pFileNode;
	CString strFileName;
	CString strFilterExt;

	ASSERT( m_pProjectDoc != NULL );

	// Make sure the Project directory has been populated
	if( m_strProjectDir.IsEmpty() )
	{
		return;
	}
	
	// Change Project document name
	CJazzDocTemplate* pTemplate = (CJazzDocTemplate *)m_pProjectDoc->GetDocTemplate();
	ASSERT( pTemplate != NULL );

	pTemplate->GetDocString( strFilterExt, CDocTemplate::filterExt );
	strFileName = m_strProjectDir + m_strName + strFilterExt;
	m_pProjectDoc->SetPathName( strFileName, TRUE );

	// Change name of all open Component documents
    POSITION pos = m_lstFiles.GetHeadPosition();
    while( pos )
    {
        pFileNode = m_lstFiles.GetNext( pos );
			
		if( pFileNode->m_pComponentDoc )
		{
			// Change path of open document
			pFileNode->ConstructFileName( strFileName );
			pFileNode->m_pComponentDoc->SetPathName( strFileName, FALSE );
		}
    }

	// Refresh Property sheet
	IDMUSProdPropSheet* pIPropSheet = theApp.GetPropertySheet();
	if( pIPropSheet )
	{
		pIPropSheet->RefreshTitleByObject( this );
		pIPropSheet->RefreshActivePageByObject( this );
		pIPropSheet->Release();
	}
}

/////////////////////////////////////////////////////////////////////////////
// CProject RemoveFile

void CProject::RemoveFile( CFileNode* pFileNode )
{
	ASSERT( pFileNode != NULL );

	POSITION pos = m_lstFiles.Find( pFileNode );

	if( pos )
	{
		m_lstFiles.RemoveAt( pos );
		pFileNode->m_pProject = NULL;
		pFileNode->Release();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CProject::GetNbrFiles

int CProject::GetNbrFiles( DWORD dwFlags )
{
	int nNbrFiles = 0;

	if( dwFlags & GNF_DIRTY )
	{
		CFileNode* pFileNode;

		POSITION pos = m_lstFiles.GetHeadPosition();

		while( pos )
		{
			pFileNode = m_lstFiles.GetNext( pos );
			if( pFileNode->m_pComponentDoc )
			{
				if( pFileNode->m_pComponentDoc->IsModified() )
				{
					nNbrFiles++;
				}
			}
		}
	}
	else
	{
		nNbrFiles = m_lstFiles.GetCount();
	}

	return nNbrFiles;
}


/////////////////////////////////////////////////////////////////////////////
// CProject::CloseAllFiles

void CProject::CloseAllFiles( void )
{
	CFileNode* pFileNode;

	POSITION pos = m_lstFiles.GetHeadPosition();

	while( pos )
	{
		pFileNode = m_lstFiles.GetNext( pos );
		if( pFileNode->m_pComponentDoc )
		{
			pFileNode->m_pComponentDoc->OnCloseDocument();	// Unconditional close!!!
		}													// See SaveAllFiles
	}
}


/////////////////////////////////////////////////////////////////////////////
// CProject::SaveAllFiles

BOOL CProject::SaveAllFiles( void )
{
	BOOL fContinue = TRUE;
	CFileNode* pFileNode;

	if( theApp.m_fShutDown == FALSE )
	{
		theApp.m_nSavePromptAction = 0;
	}

	POSITION pos = m_lstFiles.GetHeadPosition();
	while( (pos != NULL)  &&  (fContinue == TRUE) )
	{
		pFileNode = m_lstFiles.GetNext( pos );
		if( pFileNode->m_pComponentDoc )
		{
			fContinue = pFileNode->m_pComponentDoc->SaveComponentFileIfModified();
			theApp.m_pFramework->StepProgressBar( theApp.m_hKeyProgressBar );
		}									
	}

	if( theApp.m_fShutDown == FALSE )
	{
		theApp.m_nSavePromptAction = 0;
	}

	return fContinue;
}


/////////////////////////////////////////////////////////////////////////////
// CProject::RuntimeSaveAllFiles

void CProject::RuntimeSaveAllFiles( void ) 
{
	CFileNode* pFileNode;
	CString strFileName;
	CString strRuntimeFileName;
	BOOL fCloseFile; 
    
	POSITION pos = m_lstFiles.GetHeadPosition();

    while( pos )
    {
        pFileNode = m_lstFiles.GetNext( pos );

		// Determine the "Design" filename
		pFileNode->ConstructFileName( strFileName );

		// Determine the "Runtime" filename
		pFileNode->ConstructRuntimePath( strRuntimeFileName );
		strRuntimeFileName += pFileNode->m_strRuntimeFileName;

		if( pFileNode->m_nRuntimeSaveAction == RSA_SKIP)
		{
			// This "Runtime" file will be associated with a different design file
			if( strRuntimeFileName.CompareNoCase( pFileNode->m_strRuntimeFile ) == 0 )
			{
				pFileNode->m_strRuntimeFile.Empty();
			}
		}
		else if( pFileNode->m_nRuntimeSaveAction == RSA_SAVE
			 ||  pFileNode->m_nRuntimeSaveAction == RSA_NOACTION )
		{
			if( pFileNode->IsProducerFile() )	
			{
				// Save a "Runtime" version of all Producer files
				fCloseFile = FALSE;
				
				if( pFileNode->m_pComponentDoc == NULL )
				{
					// Open the file
					theApp.m_nShowNodeInTree++;
					theApp.OpenTheFile( strFileName, TGT_FILENODE );
					theApp.m_nShowNodeInTree--;

					// Will need to close file after "Runtime" save
					fCloseFile = TRUE;
				}

				if( pFileNode->m_pComponentDoc )
				{
					// Do the "Runtime" save
					pFileNode->m_pComponentDoc->DoSave( strRuntimeFileName );

					// close the file
					if( fCloseFile )
					{
						// Remove association with DocRoot before DeleteNode() 
						ASSERT( pFileNode->m_pComponentDoc->m_pIDocRootNode != NULL );
						pFileNode->m_pComponentDoc->m_pIDocRootNode->Release();
						pFileNode->m_pComponentDoc->m_pIDocRootNode = NULL;

						// Remove DocRoot node from Project Tree
						ASSERT( pFileNode->m_pIChildNode != NULL );
						pFileNode->m_pIChildNode->DeleteNode( FALSE );
						pFileNode->m_pIChildNode->Release();
						pFileNode->m_pIChildNode = NULL;
						pFileNode->m_hChildItem = NULL;

						// Close the document
						pFileNode->m_pComponentDoc->OnCloseDocument();
					}
				}
			}
			else
			{
				// Copy all non-Producer files
				CString strRuntimePath;

				// Make sure the path exists
				pFileNode->ConstructRuntimePath( strRuntimePath );
				if( theApp.CreateTheDirectory( strRuntimePath ) )
				{
					// Copy the file
					if( theApp.CopyTheFile( strFileName, strRuntimeFileName, FALSE ) )
					{
						pFileNode->m_strRuntimeFile = strRuntimeFileName;
					}
				}
			}
		}

		pFileNode->m_nRuntimeSaveAction = RSA_NOACTION;
		theApp.m_pFramework->StepProgressBar( theApp.m_hKeyProgressBar );
	}

	// Refresh Property sheet
	IDMUSProdPropSheet* pIPropSheet = theApp.GetPropertySheet();
	if( pIPropSheet )
	{
		pIPropSheet->RefreshActivePage();
		pIPropSheet->Release();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CProject CleanUpNotifyLists

void CProject::CleanUpNotifyLists( CFileNode* pTheFileNode, BOOL fFileClosed  )
{
	CFileNode* pFileNode;
	CJzNotifyNode* pJzNotifyNode;
	IDMUSProdNode* pIDocRootNode;
	POSITION pos1;
	POSITION pos2;
	POSITION pos3;

    pos1 = m_lstFiles.GetHeadPosition();
    while( pos1 )
    {
        pFileNode = m_lstFiles.GetNext( pos1 );

		// Clean up CFileNode's notify list
		pos2 = pFileNode->m_lstNotifyNodes.GetHeadPosition();
		while( pos2 )
		{
			pJzNotifyNode = static_cast<CJzNotifyNode*>( pFileNode->m_lstNotifyNodes.GetNext(pos2) );

			// If all values are equal to zero, remove CJzNotifyNode from notify list.
			if( IsEqualGUID ( pJzNotifyNode->m_guidFile, GUID_AllZeros )
			&&  pJzNotifyNode->m_pINotifyThisNode == NULL
			&&  pJzNotifyNode->m_nUseCount == 0 )
			{
				pos3 = pFileNode->m_lstNotifyNodes.Find( pJzNotifyNode );
				if( pos3 )
				{
					pFileNode->m_lstNotifyNodes.RemoveAt( pos3 );
					delete pJzNotifyNode;
				}
			}
			// If GUIDs are equal, remove CJzNotifyNode from notify list.
			// Need to remove it from list because either File Open process
			// will have inserted another CJzNotifyNode with the IDMUSProdNode*
			// or File Close process will have caused need to remove CJzNotifyNodes 
			// with m_pINotifyThisNode equal to the file being closed.
			else if( IsEqualGUID ( pJzNotifyNode->m_guidFile, pTheFileNode->m_guid ) )
			{
				pos3 = pFileNode->m_lstNotifyNodes.Find( pJzNotifyNode );
				if( pos3 )
				{
					if( fFileClosed )
					{
						if( pJzNotifyNode->m_pINotifyThisNode )
						{
							IDMUSProdNode* pINotifyThisNode = pJzNotifyNode->m_pINotifyThisNode;
							pJzNotifyNode->m_pINotifyThisNode = NULL;
							pINotifyThisNode->Release();
						}
					}
					else
					{
						pFileNode->m_lstNotifyNodes.RemoveAt( pos3 );
						delete pJzNotifyNode;
					}
				}
			}
			else
			{
				// The File Open and File New processes insert CJzFileNodes with a GUID 
				// equal to zero because the File guid is not known at the time
				// IDMUSProdFramework::AddToNotifyList is called.  If we run across a matching
				// CJZNotifyNode we have to fill in its GUID.
				if( pJzNotifyNode->m_pINotifyThisNode )
				{
					// We can only fill in the GUID after the node has been added to the Project Tree
					if( SUCCEEDED ( pJzNotifyNode->m_pINotifyThisNode->GetDocRootNode( &pIDocRootNode ) ) )
					{
						if( pIDocRootNode )
						{
							if( theApp.m_pFramework->IsDocRootNode( pIDocRootNode ) )
							{
								// Node is in Project Tree so fill in the GUID
								if( pIDocRootNode == pTheFileNode->m_pIChildNode )
								{
									memcpy( &pJzNotifyNode->m_guidFile, &pTheFileNode->m_guid, sizeof(pTheFileNode->m_guid) );
								}
							}

							pIDocRootNode->Release();
						}
					}
				}
			}
		}
    }
}


/////////////////////////////////////////////////////////////////////////////
// CProject::LoadDefaultRuntimeFolders

HRESULT CProject::LoadDefaultRuntimeFolders( IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain ) 
{
    IStream* pIStream;
	MMCKINFO ck;
	MMCKINFO ckList;
    HRESULT hr = S_OK;
	CJzRuntimeFolder* pJzRuntimeFolder;
	CString strRuntimeFolder;
	CString strRelative = _T("..\\");
	DWORD dwAttributes;

    pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	// Read through the chunks in the Project list
	while( pIRiffStream->Descend( &ck, pckMain, 0 ) == 0 )
	{
		switch( ck.ckid )
		{
			case FOURCC_LIST:
				switch( ck.fccType )
				{
					case FOURCC_RUNTIME_FOLDER_LIST:
					{
						pJzRuntimeFolder = NULL;

						while( pIRiffStream->Descend( &ckList, &ck, 0 ) == 0 )
						{
							switch( ckList.ckid )
							{
								case FOURCC_PATH_CHUNK:
									ReadMBSfromWCS( pIStream, ckList.cksize, &strRuntimeFolder );
									if( strRuntimeFolder.Find( strRelative ) == 0 )
									{
										strRuntimeFolder = strRuntimeFolder.Right( strRuntimeFolder.GetLength() - strRelative.GetLength() );
										strRuntimeFolder = m_strProjectDir + strRuntimeFolder;
									}
									else
									{
										// Make sure the new directory exists
										dwAttributes = GetFileAttributes( strRuntimeFolder );

										if( (dwAttributes == 0xFFFFFFFF)
										|| !(dwAttributes & FILE_ATTRIBUTE_DIRECTORY) )
										{
											// Directory does not exist
											strRuntimeFolder.Empty();
										}
									}
									if( !strRuntimeFolder.IsEmpty() )
									{
										pJzRuntimeFolder = new CJzRuntimeFolder;
										if( pJzRuntimeFolder )
										{
											pJzRuntimeFolder->m_strRuntimeFolder = strRuntimeFolder;

											m_lstRuntimeFolders.AddTail( pJzRuntimeFolder );
										}
									}
									break;

								case FOURCC_FILTER_CHUNK:
									if( pJzRuntimeFolder )
									{
										ReadMBSfromWCS( pIStream, ckList.cksize, &pJzRuntimeFolder->m_strFilterExt );
										pJzRuntimeFolder->m_pDocTemplate = theApp.FindDocTemplateByFilter( pJzRuntimeFolder->m_strFilterExt );
									}
									break;
							}

							pIRiffStream->Ascend( &ckList, 0 );
						}
						break;
					}
				}
				break;
		}

		pIRiffStream->Ascend( &ck, 0 );
	}

    pIStream->Release();
    return hr;
}

/////////////////////////////////////////////////////////////////////////////
// CProject::LoadTheProject

HRESULT CProject::LoadTheProject( IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain ) 
{
    IStream*      pIStream;
	MMCKINFO	  ck;
	MMCKINFO	  ckList;
	DWORD		  dwByteCount;
	DWORD		  dwSize;
    WORD		  wStructSize;
	HANDLE		  hFile;
    CString       strFileName;
    CString       strDir;
	BSTR		  bstrName;
    HRESULT       hr = E_FAIL;

    pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );
	
	CString strRelative = _T("..\\");
	CString strRuntimeDir;

	// Get the filename
	IDMUSProdPersistInfo* pPersistInfo;

	if( SUCCEEDED ( pIStream->QueryInterface( IID_IDMUSProdPersistInfo, (void **)&pPersistInfo ) ) )
	{
		BSTR bstrFileName;

		pPersistInfo->GetFileName( &bstrFileName );
		strFileName = bstrFileName;
		::SysFreeString( bstrFileName );

		pPersistInfo->Release();
	}

	// Determine the Project directory
	TCHAR achProjectPath[FILENAME_MAX];
	TCHAR achDrive[_MAX_DRIVE];
	TCHAR achDir[_MAX_DIR];
	TCHAR achName[_MAX_FNAME];
	TCHAR achExt[_MAX_EXT];

	_tsplitpath( strFileName, achDrive, achDir, achName, achExt );
	_tmakepath( achProjectPath, achDrive, achDir, NULL, NULL );

	// Set the Project name
	m_strName = achName;
	bstrName = m_strName.AllocSysString();
	SetNodeName( bstrName );

	// Set the "Project" directory
	m_strProjectDir = achProjectPath;
	if( m_strProjectDir.Right(1) != _T("\\") )
	{
		m_strProjectDir += _T("\\");
	}
	::SetCurrentDirectory( m_strProjectDir );

	// Set the "Runtime" directory
	strDir.LoadString( IDS_RUNTIME_DIR );
	m_strRuntimeDir = m_strProjectDir + strDir;
	if( m_strRuntimeDir.Right(1) != _T("\\") )
	{
		m_strRuntimeDir += _T("\\");
	}

	// Set the Project Last Modified time
	m_strLastModified.Empty();
    hFile = ::CreateFile( strFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
	 					  FILE_ATTRIBUTE_NORMAL, NULL );
	if( hFile != INVALID_HANDLE_VALUE )
	{
		FILETIME ftModified;

		if( ::GetFileTime(hFile, NULL, NULL, &ftModified) )
		{
			CTime timeFile( ftModified );
			m_strLastModified = timeFile.Format( "%A, %B %d, %Y %I:%M:%S %p" );
		}
		CloseHandle( hFile );
	}

	// Read through the chunks in the Project list
	while( pIRiffStream->Descend( &ck, pckMain, 0 ) == 0 )
	{
		switch( ck.ckid )
		{
			case FOURCC_PROJECT_CHUNK:
			{
				ioJzProject iJzProject;

				// Read size of ioJzProject structure
				dwSize = ck.cksize;
				pIStream->Read( &wStructSize, sizeof( wStructSize ), NULL );
				dwSize -= sizeof( wStructSize );

				// Read ioJzProject structure
				if( wStructSize > sizeof(iJzProject) )
				{
					hr = pIStream->Read( &iJzProject, sizeof(ioJzProject), &dwByteCount );
					if( FAILED( hr )
					||  dwByteCount != sizeof(ioJzProject) )
					{
						goto ON_ERROR;
					}
					StreamSeek( pIStream, wStructSize - sizeof(ioJzProject), STREAM_SEEK_CUR );
				}
				else
				{
					hr = pIStream->Read( &iJzProject, wStructSize, &dwByteCount );
					if( FAILED( hr )
					||  dwByteCount != wStructSize )
					{
						goto ON_ERROR;
					}
				}
				dwSize -= wStructSize;

				memcpy( &m_guid, &iJzProject.m_guidProject, sizeof(GUID) );

				// Read ModifiedBy name (unicode format)
				if( dwSize > 0 )
				{
					ReadMBSfromWCS( pIStream, dwSize, &m_strLastModifiedBy );
				}
				break;
			}

			case FOURCC_PROJECT_PCHANNEL_NAMES:
			{
				DWORD dwStringCount;
				DWORD dwPChannel;
				CString strPChName;

				dwSize = ck.cksize;

				while( dwSize > 0 )
				{
					// Read PChannel #
					hr = pIStream->Read( &dwPChannel, sizeof(DWORD), &dwByteCount );
					if( FAILED( hr )
					||  dwByteCount != sizeof(DWORD) )
					{
						goto ON_ERROR;
					}
					dwSize -= sizeof(DWORD);

					// Read character count
					hr = pIStream->Read( &dwStringCount, sizeof(DWORD), &dwByteCount );
					if( FAILED( hr )
					||  dwByteCount != sizeof(DWORD) )
					{
						goto ON_ERROR;
					}
					dwSize -= sizeof(DWORD);

					if( dwSize < sizeof(WCHAR) * dwStringCount )
					{
						goto ON_ERROR;
					}

					// Read the PChannel's name
					ReadMBSfromWCS( pIStream, sizeof(WCHAR) * dwStringCount, &strPChName );
					dwSize -= sizeof(WCHAR) * dwStringCount;

					if( dwPChannel < 32 )
					{
						m_aPChannelNames[dwPChannel] = strPChName;
					}
					else
					{
						// Assume saved in sorted order
						PChannelName* pPChannelName;
						
						// Create and fill out a new PChannelName structure
						pPChannelName = new PChannelName;
						pPChannelName->m_strName = strPChName;
						pPChannelName->m_dwPChannel = dwPChannel;

						m_lstPChannelNames.AddTail( pPChannelName );
					}
				}
				break;
			}

			case FOURCC_LIST:
				switch( ck.fccType )
				{
					case FOURCC_INFO_LIST:
					case FOURCC_UNFO_LIST:
						while( pIRiffStream->Descend( &ckList, &ck, 0 ) == 0 )
						{
							switch( ckList.ckid )
							{
								case RIFFINFO_ICMT:
								case FOURCC_UNFO_COMMENT:
									ReadMBSfromWCS( pIStream, ckList.cksize, &m_strDescription );
									break;

								case FOURCC_UNFO_RUNTIME_FOLDER:
									ReadMBSfromWCS( pIStream, ckList.cksize, &strRuntimeDir );
									if( strRuntimeDir.Find( strRelative ) == 0 )
									{
										strRuntimeDir = strRuntimeDir.Right( strRuntimeDir.GetLength() - strRelative.GetLength() );
										m_strRuntimeDir = m_strProjectDir + strRuntimeDir;
									}
									else
									{
										// Make sure the new directory exists
										DWORD dwAttributes = GetFileAttributes( strRuntimeDir );

										if( (dwAttributes != 0xFFFFFFFF)
										&&  (dwAttributes & FILE_ATTRIBUTE_DIRECTORY) )
										{
											// Directory exists
											m_strRuntimeDir = strRuntimeDir;
										}
									}
									break;
							}
							pIRiffStream->Ascend( &ckList, 0 );
						}
						break;

					case FOURCC_RUNTIME_FOLDERS_LIST:
						hr = LoadDefaultRuntimeFolders( pIRiffStream, &ck );
						if( FAILED ( hr ) )
						{
							goto ON_ERROR;
						}
						break;

					case FOURCC_OPEN_EDITORS_LIST:
					{
						CBookmark* pBookmark = new CBookmark;
						if( pBookmark )
						{
							if( SUCCEEDED ( pBookmark->LoadBookmark( pIRiffStream, &ck ) ) )
							{
								m_pBookmark = pBookmark;
							}
							else
							{
								delete pBookmark;
							}
						}
						break;
					}

					case FOURCC_BOOKMARK_LIST:
					{
						CMainFrame* pMainFrame = (CMainFrame *)theApp.m_pMainWnd;
						ASSERT( pMainFrame != NULL );

						CBookmark* pBookmark = new CBookmark;
						if( pBookmark )
						{
							if( (FAILED ( pBookmark->LoadBookmark( pIRiffStream, &ck ) ) )
							||  (pMainFrame->m_wndBookmarkToolBar.AddBookmark( pBookmark, FALSE ) == FALSE) )
							{
								delete pBookmark;
								break;
							}
						}
						break;
					}
				}
				break;
		}

		pIRiffStream->Ascend( &ck, 0 );
	}

ON_ERROR:
    pIStream->Release();
    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CProject::SavePChannelNames

HRESULT CProject::SavePChannelNames( IDMUSProdRIFFStream* pIRiffStream )
{
	IStream* pIStream;
	HRESULT hr = S_OK;
	MMCKINFO ck;
	DWORD dwBytesWritten;
	DWORD dwStringCount;
	DWORD dwPChannel;
	WCHAR awszName[MAX_PATH];

    pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	// Write Project chunk header
	ck.ckid = FOURCC_PROJECT_PCHANNEL_NAMES;
	if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Iterate through first 32 PChannels
	for( dwPChannel = 0; dwPChannel < 32; dwPChannel++ )
	{
		if( !m_aPChannelNames[dwPChannel].IsEmpty() )
		{
			dwStringCount = MultiByteToWideChar( CP_ACP, 0, m_aPChannelNames[dwPChannel], -1, awszName, (size_t)MAX_PATH );
			if( dwStringCount > 0 )
			{
				// Write PChannel #
				hr = pIStream->Write( &dwPChannel, sizeof(DWORD), &dwBytesWritten );
				if( FAILED( hr )
				||  dwBytesWritten != sizeof(DWORD) )
				{
					hr = E_FAIL;
					goto ON_ERROR;
				}

				// Write # of characters
				hr = pIStream->Write( &dwStringCount, sizeof(DWORD), &dwBytesWritten );
				if( FAILED( hr )
				||  dwBytesWritten != sizeof(DWORD) )
				{
					hr = E_FAIL;
					goto ON_ERROR;
				}

				// Write string
				hr = pIStream->Write( awszName, sizeof(WCHAR) * dwStringCount, &dwBytesWritten );
				if( FAILED( hr )
				||  dwBytesWritten != sizeof(WCHAR) * dwStringCount )
				{
					hr = E_FAIL;
					goto ON_ERROR;
				}
			}
		}
	}

	// Now, iterate through all other PChannels
	POSITION pos;
	PChannelName* pPChannelName;
	pos = m_lstPChannelNames.GetHeadPosition();
	while( pos != NULL )
	{
		// Get the next PChannelName structure
		pPChannelName = m_lstPChannelNames.GetNext( pos );

		// Convert the name to WCHARs
		dwStringCount = MultiByteToWideChar( CP_ACP, 0, pPChannelName->m_strName, -1, awszName, MAX_PATH );
		if( dwStringCount > 0 )
		{
			// Write PChannel #
			hr = pIStream->Write( &(pPChannelName->m_dwPChannel), sizeof(DWORD), &dwBytesWritten );
			if( FAILED( hr )
			||  dwBytesWritten != sizeof(DWORD) )
			{
				hr = E_FAIL;
				goto ON_ERROR;
			}

			// Write # of characters
			hr = pIStream->Write( &dwStringCount, sizeof(DWORD), &dwBytesWritten );
			if( FAILED( hr )
			||  dwBytesWritten != sizeof(DWORD) )
			{
				hr = E_FAIL;
				goto ON_ERROR;
			}

			// Write string
			hr = pIStream->Write( awszName, sizeof(WCHAR) * dwStringCount, &dwBytesWritten );
			if( FAILED( hr )
			||  dwBytesWritten != sizeof(WCHAR) * dwStringCount )
			{
				hr = E_FAIL;
				goto ON_ERROR;
			}
		}
	}

	if( pIRiffStream->Ascend( &ck, 0 ) != 0 )
	{
 		hr = E_FAIL;
		goto ON_ERROR;
	}

ON_ERROR:
    pIStream->Release();
    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CProject::SaveProjectChunk

HRESULT CProject::SaveProjectChunk( IDMUSProdRIFFStream* pIRiffStream ) 
{
	IStream* pIStream;
	HRESULT hr;
	MMCKINFO ck;
	DWORD dwBytesWritten;
	DWORD dwBufferSize;
    WORD wStructSize;
	TCHAR achText[MID_BUFFER];
	ioJzProject oJzProject;

    pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	// Write Project chunk header
	ck.ckid = FOURCC_PROJECT_CHUNK;
	if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Save size of ioJzProject structure
	wStructSize = sizeof(ioJzProject);
	hr = pIStream->Write( &wStructSize, sizeof(wStructSize), &dwBytesWritten );
	if( FAILED( hr )
	||  dwBytesWritten != sizeof(wStructSize) )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Prepare ioJzProject structure
	memset( &oJzProject, 0, sizeof(ioJzProject) );
	memcpy( &oJzProject.m_guidProject, &m_guid, sizeof(GUID) );

	// Write Project chunk data
	hr = pIStream->Write( &oJzProject, sizeof(ioJzProject), &dwBytesWritten);
	if( FAILED( hr )
	||  dwBytesWritten != sizeof(ioJzProject) )
	{
        hr = E_FAIL;
        goto ON_ERROR;
	}

	// Determine Project Last Modified By
	dwBufferSize = MID_BUFFER;
	if( GetUserName( achText, &dwBufferSize ) == 0 )
	{
		_tcscpy( achText, _T("") );
	}
	m_strLastModifiedBy = achText;

	// Write Project Last Modified By
	hr = SaveMBStoWCS( pIStream, &m_strLastModifiedBy );
	if( FAILED( hr ) )
	{
		goto ON_ERROR;
	}
	
	if( pIRiffStream->Ascend( &ck, 0 ) != 0 )
	{
 		hr = E_FAIL;
		goto ON_ERROR;
	}

ON_ERROR:
    pIStream->Release();
    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CProject::SaveProjectInfo

HRESULT CProject::SaveProjectInfo( IDMUSProdRIFFStream* pIRiffStream ) 
{
	IStream* pIStream;
	HRESULT hr;
    MMCKINFO ckMain;
    MMCKINFO ck;

	if( m_strDescription.IsEmpty()
	&&  m_strRuntimeDir.IsEmpty() )
	{
		return S_OK;
	}

    pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	// Write INFO LIST header
	ckMain.fccType = FOURCC_UNFO_LIST;
	if( pIRiffStream->CreateChunk(&ckMain, MMIO_CREATELIST) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Write Project default "Runtime" directory
	if( !m_strRuntimeDir.IsEmpty() )
	{
		ck.ckid = FOURCC_UNFO_RUNTIME_FOLDER;
		if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}

		CString strRuntimeDir = m_strRuntimeDir;
		
		// Save relative path if we can
		if( strRuntimeDir.Find( m_strProjectDir ) == 0 )
		{
			// Strip Project directory from path
			strRuntimeDir = strRuntimeDir.Right( strRuntimeDir.GetLength() - m_strProjectDir.GetLength() );

			// Prefix with "..\"
			strRuntimeDir = _T("..\\") + strRuntimeDir;
		}

		hr = SaveMBStoWCS( pIStream, &strRuntimeDir );
		if( FAILED( hr ) )
		{
			goto ON_ERROR;
		}

		if( pIRiffStream->Ascend(&ck, 0) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}
	}

	// Write Project Description
	if( !m_strDescription.IsEmpty() )
	{
		ck.ckid = FOURCC_UNFO_COMMENT;
		if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}

		hr = SaveMBStoWCS( pIStream, &m_strDescription );
		if( FAILED( hr ) )
		{
			goto ON_ERROR;
		}

		if( pIRiffStream->Ascend(&ck, 0) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}
	}

	if( pIRiffStream->Ascend(&ckMain, 0) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

ON_ERROR:
    pIStream->Release();
    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CProject::SaveDefaultRuntimeFolders

HRESULT CProject::SaveDefaultRuntimeFolders( IDMUSProdRIFFStream* pIRiffStream ) 
{
	IStream* pIStream;
	HRESULT hr;
    MMCKINFO ckFolders;
    MMCKINFO ckFolder;
    MMCKINFO ck;
	POSITION pos;
	CJzRuntimeFolder* pJzRuntimeFolder;
	CString strRuntimeFolder;

    pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	// Write "Runtime" folders LIST header
	ckFolders.fccType = FOURCC_RUNTIME_FOLDERS_LIST;
	if( pIRiffStream->CreateChunk(&ckFolders, MMIO_CREATELIST) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Save folders
	pos = m_lstRuntimeFolders.GetHeadPosition();
	while( pos )
	{
		pJzRuntimeFolder = m_lstRuntimeFolders.GetNext(pos);

		// Write "Runtime" folder LIST header
		ckFolder.fccType = FOURCC_RUNTIME_FOLDER_LIST;
		if( pIRiffStream->CreateChunk(&ckFolder, MMIO_CREATELIST) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}

		// Write "Runtime" folder path
		{
			ck.ckid = FOURCC_PATH_CHUNK;
			if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
			{
				hr = E_FAIL;
				goto ON_ERROR;
			}

			strRuntimeFolder = pJzRuntimeFolder->m_strRuntimeFolder;
			if( strRuntimeFolder.Find( m_strProjectDir ) == 0 )
			{
				strRuntimeFolder = strRuntimeFolder.Right( strRuntimeFolder.GetLength() - m_strProjectDir.GetLength() );
				strRuntimeFolder = _T("..\\") + strRuntimeFolder;
			}

			hr = SaveMBStoWCS( pIStream, &strRuntimeFolder );
			if( FAILED( hr ) )
			{
				goto ON_ERROR;
			}

			if( pIRiffStream->Ascend(&ck, 0) != 0 )
			{
				hr = E_FAIL;
				goto ON_ERROR;
			}
		}

		// Write file filter associated with "Runtime" folder 
		{
			ck.ckid = FOURCC_FILTER_CHUNK;
			if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
			{
				hr = E_FAIL;
				goto ON_ERROR;
			}

			hr = SaveMBStoWCS( pIStream, &pJzRuntimeFolder->m_strFilterExt );
			if( FAILED( hr ) )
			{
				goto ON_ERROR;
			}

			if( pIRiffStream->Ascend(&ck, 0) != 0 )
			{
				hr = E_FAIL;
				goto ON_ERROR;
			}
		}

		if( pIRiffStream->Ascend(&ckFolder, 0) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}
	}

	if( pIRiffStream->Ascend(&ckFolders, 0) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

ON_ERROR:
    pIStream->Release();
    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CProject::SaveTheProject

HRESULT CProject::SaveTheProject( IDMUSProdRIFFStream* pIRiffStream ) 
{
	HRESULT hr = S_OK;
	CBookmark* pBookmark;
	IDMUSProdPropSheet* pIPropSheet;
	IStream* pIStream;
	MMCKINFO ck;
	MMCKINFO ckBookmark;
	POSITION pos;
	HANDLE hFile;
	CString strFileName;

    pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

// Write Project list header
	ck.fccType = FOURCC_PROJECT_LIST;
	if( pIRiffStream->CreateChunk(&ck, MMIO_CREATELIST) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

// Save Project chunk
	hr = SaveProjectChunk( pIRiffStream );
	if( FAILED ( hr ) )
	{
		goto ON_ERROR;
	}

// Save Project info
	hr = SaveProjectInfo( pIRiffStream );
	if( FAILED ( hr ) )
	{
		goto ON_ERROR;
	}

// Save PChannel names
	hr = SavePChannelNames( pIRiffStream );
	if( FAILED ( hr ) )
	{
		goto ON_ERROR;
	}

// Save Project default "Runtime" folders
	hr = SaveDefaultRuntimeFolders( pIRiffStream );
	if( FAILED ( hr ) )
	{
		goto ON_ERROR;
	}

// Save open editors
	pBookmark = new CBookmark;
	if( pBookmark )
	{
		pBookmark->Create();

		// Save Component states even if there are no open editors
		ckBookmark.fccType = FOURCC_OPEN_EDITORS_LIST;

		if( pIRiffStream->CreateChunk( &ckBookmark, MMIO_CREATELIST) == 0
		&&  SUCCEEDED( pBookmark->SaveBookmark( this, pIRiffStream ) )
		&&  pIRiffStream->Ascend( &ckBookmark, 0 ) == 0 )
		{
			hr = S_OK;
		}
		else
		{
			hr = E_FAIL;
		}

		delete pBookmark;
		if( FAILED ( hr ) )
		{
			goto ON_ERROR;
		}
	}

// Save Bookmarks
    pos = theApp.m_lstBookmarks.GetHeadPosition();
    while( pos )
    {
        pBookmark = theApp.m_lstBookmarks.GetNext( pos );

		if( pBookmark->IsForThisProject( this ) )
		{
			ckBookmark.fccType = FOURCC_BOOKMARK_LIST;

			if( pIRiffStream->CreateChunk( &ckBookmark, MMIO_CREATELIST) == 0
			&&  SUCCEEDED( pBookmark->SaveBookmark( this, pIRiffStream ) )
			&&  pIRiffStream->Ascend( &ckBookmark, 0 ) == 0 )
			{
				hr = S_OK;
			}
			else
			{
				hr = E_FAIL;
				goto ON_ERROR;
			}
		}
    }
	
	if( pIRiffStream->Ascend( &ck, 0 ) != 0 )
	{
 		hr = E_FAIL;
		goto ON_ERROR;
	}

// Save Files
	CFileNode* pFileNode;

    pos = m_lstFiles.GetHeadPosition();
    while( pos )
    {
        pFileNode = m_lstFiles.GetNext( pos );
		hr = pFileNode->SaveTheFile( pIRiffStream );
		if( FAILED ( hr ) )
		{
			goto ON_ERROR;
		}
    }

// Change Property sheet modified information
	pIPropSheet = theApp.GetPropertySheet();
	if( pIPropSheet )
	{
		pIPropSheet->RefreshActivePageByObject( this );
		pIPropSheet->Release();
	}

	// Get the filename
	IDMUSProdPersistInfo* pPersistInfo;

	if( SUCCEEDED ( pIStream->QueryInterface( IID_IDMUSProdPersistInfo, (void **)&pPersistInfo ) ) )
	{
		BSTR bstrFileName;

		pPersistInfo->GetFileName( &bstrFileName );
		strFileName = bstrFileName;
		::SysFreeString( bstrFileName );

		pPersistInfo->Release();
	}

	// Reset Project Last Modified
	m_strLastModified.Empty();
    hFile = ::CreateFile( strFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
		 				  FILE_ATTRIBUTE_NORMAL, NULL );
	if( hFile != INVALID_HANDLE_VALUE )
	{
		FILETIME ftModified;

		if( ::GetFileTime(hFile, NULL, NULL, &ftModified) )
		{
			CTime timeFile( ftModified );
			m_strLastModified = timeFile.Format( "%A, %B %d, %Y %I:%M:%S %p" );
		}
		CloseHandle( hFile );
	}

ON_ERROR:
    pIStream->Release();
    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CProject CreateFileNode

CFileNode* CProject::CreateFileNode( LPCTSTR szFileName )
{
	CFileNode* pFileNode = NULL;
	CString strPath = szFileName;
	CString strProjectDir = m_strProjectDir;
	CString strName;
	CString strSubDir;

	strPath.MakeLower();
	strProjectDir.MakeLower();

	// Remove Project directory from path
	if( strPath.Find( strProjectDir ) != 0 )
	{
		return NULL;
	}
	strPath = strPath.Right( strPath.GetLength() - m_strProjectDir.GetLength() );

	// Split path from filename
	int nFindPos = strPath.ReverseFind( (TCHAR)'\\' );
	if( nFindPos == -1 )
	{
		strName = strPath;
		strPath.Empty();
	}
	else
	{
		strName = strPath.Right( strPath.GetLength() - nFindPos - 1 );
		strPath = strPath.Left( nFindPos + 1 );
	}

	CDirectoryNode* pParentNode = (CDirectoryNode *)this;
	CDirectoryNode* pSubDirNode;
	
	// Create Directory nodes
	while( !strPath.IsEmpty() )
	{
		nFindPos = strPath.Find( (TCHAR)'\\' );
		if( nFindPos == -1 )
		{
			break;
		}
		
		strSubDir = strPath.Left( nFindPos );
		strPath = strPath.Right( strPath.GetLength() - nFindPos - 1 );

		pSubDirNode = pParentNode->GetSubDirectoryByName( strSubDir );

		if( pSubDirNode )
		{
			pParentNode = pSubDirNode;
		}
		else
		{
			// Create a new directory node and insert it into the Project Tree
			pSubDirNode = new CDirectoryNode;

			if( pSubDirNode == NULL )
			{
				return NULL;
			}

			pSubDirNode->m_strName = strSubDir;
			pParentNode->InsertChildNode( pSubDirNode );
			pParentNode = pSubDirNode;
		}
	}

	// Create a new file node and insert it into the Project Tree
	pFileNode = new CFileNode;

	if( pFileNode )
	{
		pFileNode->m_strName = strName;
		pParentNode->InsertChildNode( pFileNode );
		pFileNode->m_strName.Empty();

		BSTR bstrName = strName.AllocSysString();
		pFileNode->SetNodeName( bstrName );

		// Sync List info
		pFileNode->SyncListInfo();
	}

	return pFileNode;
}


/////////////////////////////////////////////////////////////////////////////
// CProject FindFileNode

CFileNode* CProject::FindFileNode( LPCTSTR szPathName )
{
	CFileNode* pFileNode;
	CString strFileName;

    POSITION pos = m_lstFiles.GetHeadPosition();

    while( pos )
    {
        pFileNode = m_lstFiles.GetNext( pos );

		pFileNode->ConstructFileName( strFileName );
		if( strFileName.CompareNoCase( szPathName )  == 0 )
		{
			return pFileNode;
		}
    }

	return NULL;
}


/////////////////////////////////////////////////////////////////////////////
// CProject GetFileByGUID

CFileNode* CProject::GetFileByGUID( GUID guidFile )
{
	CFileNode* pFileNode;

    POSITION pos = m_lstFiles.GetHeadPosition();

    while( pos )
    {
        pFileNode = m_lstFiles.GetNext( pos );
		if( IsEqualGUID ( pFileNode->m_guid, guidFile ) )
		{
			pFileNode->AddRef();
			return pFileNode;
		}
    }

	return NULL;
}


/////////////////////////////////////////////////////////////////////////////
// CProject GetFileByDocRootNode

CFileNode* CProject::GetFileByDocRootNode( IDMUSProdNode* pIDocRootNode )
{
	CFileNode* pFileNode;

    POSITION pos = m_lstFiles.GetHeadPosition();

    while( pos )
    {
        pFileNode = m_lstFiles.GetNext( pos );
		if( pFileNode->m_pIChildNode == pIDocRootNode )
		{
			pFileNode->AddRef();
			return pFileNode;
		}
    }

	return NULL;
}


/////////////////////////////////////////////////////////////////////////////
// CProject GetFileByText

CFileNode* CProject::GetFileByText( IDMUSProdDocType* pIDocType, LPCTSTR szNodeName, LPCTSTR szNodeDescriptor )
{
	CFileNode* pListFileNode;
	IDMUSProdDocType* pIListDocType;
	CString strListName;
	CString strListDescriptor;
	DMUSProdListInfo ListInfo;

    POSITION pos = m_lstFiles.GetHeadPosition();

    while( pos )
    {
        pListFileNode = m_lstFiles.GetNext( pos );

		// Get FileNode's DocType
		pIListDocType = theApp.GetDocType( pListFileNode->m_strName );

		// Determine whether pFileNode is correct type of file
		BOOL fMatchingDocType = FALSE;
		if( pIListDocType )
		{
			if( pIDocType == pIListDocType )
			{
				fMatchingDocType = TRUE;
			}
			pIListDocType->Release();
		}

		if( fMatchingDocType )
		{
			ZeroMemory( &ListInfo, sizeof(DMUSProdListInfo) );
			ListInfo.wSize = sizeof(DMUSProdListInfo);

			if( SUCCEEDED ( pListFileNode->GetNodeListInfo ( &ListInfo ) ) )
			{	
				strListName.Empty();
				if( ListInfo.bstrName )
				{
					strListName = ListInfo.bstrName;
					::SysFreeString( ListInfo.bstrName );
				}

				strListDescriptor.Empty();
				if( ListInfo.bstrDescriptor )
				{
					if( _tcslen( szNodeDescriptor ) )
					{
						strListDescriptor = ListInfo.bstrDescriptor;
					}
					::SysFreeString( ListInfo.bstrDescriptor );
				}

				if( (_tcscmp( szNodeName, strListName ) == 0)
				&&  (_tcscmp( szNodeDescriptor, strListDescriptor ) == 0) )
				{
					// This is the File Node we want
					pListFileNode->AddRef();
					return pListFileNode;
				}
			}
		}
	}

	return NULL;
}


/////////////////////////////////////////////////////////////////////////////
// CProject GetFileByRuntimeFileName

CFileNode* CProject::GetFileByRuntimeFileName( LPCTSTR szRuntimeFileName,
											   CFileNode* pBypassFileNode, BOOL fUseDefaultName )
{
	CFileNode* pFileNode;
	CString strRuntimeFileName;

    POSITION pos = m_lstFiles.GetHeadPosition();

    while( pos )
    {
        pFileNode = m_lstFiles.GetNext( pos );

		if( pFileNode != pBypassFileNode )
		{
			if( fUseDefaultName )
			{
				pFileNode->ConstructRuntimePath( strRuntimeFileName );
				strRuntimeFileName += pFileNode->m_strRuntimeFileName;
			}
			else
			{
				strRuntimeFileName = pFileNode->m_strRuntimeFile;
			}

			if( strRuntimeFileName.CompareNoCase( szRuntimeFileName ) == 0 )
			{
				pFileNode->AddRef();
				return pFileNode;
			}
		}
    }

	return NULL;
}


/////////////////////////////////////////////////////////////////////////////
// CProject GetBestGuessDocRootNode

CFileNode* CProject::GetBestGuessDocRootNode( IDMUSProdDocType* pIDocType, LPCTSTR szNodeName,
											  IDMUSProdNode* pITreePositionNode )
{
	CFileNode* pTheFileNode = NULL;
	CFileNode* pListFileNode;
	IDMUSProdDocType* pIListDocType;
	CString strListFileName;
	BSTR bstrListFileName;
	CString strListName;
	DMUSProdListInfo ListInfo;

	ASSERT( pIDocType != NULL );
	ASSERT( szNodeName != NULL );
	ASSERT( pITreePositionNode != NULL );

	// Find the path of the file associated with pITreePositionNode
	CString strTreePath;
	BSTR bstrTreePath;
	GUID guidNodeId;

	if( SUCCEEDED ( pITreePositionNode->GetNodeId( &guidNodeId ) ) )
	{
		if( IsEqualGUID ( guidNodeId, GUID_ProjectFolderNode )
		||  IsEqualGUID ( guidNodeId, GUID_DirectoryNode ) )
		{
			CDirectoryNode* pDirNode = (CDirectoryNode *)pITreePositionNode;

			pDirNode->ConstructPath( strTreePath );
		}
		else
		{
			if( SUCCEEDED ( theApp.m_pFramework->GetNodeFileName( pITreePositionNode, &bstrTreePath ) ) )
			{
				strTreePath = bstrTreePath;
				::SysFreeString( bstrTreePath );

				int nFindPos = strTreePath.ReverseFind( (TCHAR)'\\' );
				if( nFindPos != -1 )
				{
					strTreePath = strTreePath.Left( nFindPos + 1 );
				}
			}
		}
	}

	// Search list of files
    POSITION pos = m_lstFiles.GetHeadPosition();

    while( pos )
    {
        pListFileNode = m_lstFiles.GetNext( pos );

		// Get FileNode's DocType
		pIListDocType = theApp.GetDocType( pListFileNode->m_strName );

		// Determine whether pFileNode is correct type of file
		BOOL fMatchingDocType = FALSE;
		if( pIListDocType )
		{
			if( pIDocType == pIListDocType )
			{
				fMatchingDocType = TRUE;
			}
			pIListDocType->Release();
		}

		if( fMatchingDocType )
		{
			ZeroMemory( &ListInfo, sizeof(DMUSProdListInfo) );
			ListInfo.wSize = sizeof(DMUSProdListInfo);

			if( SUCCEEDED ( pListFileNode->GetNodeListInfo ( &ListInfo ) ) )
			{	
				strListName.Empty();
				if( ListInfo.bstrName )
				{
					strListName = ListInfo.bstrName;
					::SysFreeString( ListInfo.bstrName );
				}

				if( ListInfo.bstrDescriptor )
				{
					::SysFreeString( ListInfo.bstrDescriptor );
				}

				if( _tcscmp( szNodeName, strListName ) == 0 )
				{
					// Return this FileNode if it is in the same directory as pITreePositionNode
					if( SUCCEEDED ( theApp.m_pFramework->GetNodeFileName( pListFileNode, &bstrListFileName ) ) )
					{
						strListFileName = bstrListFileName;
						::SysFreeString( bstrListFileName );

						if( strListFileName.Find( strTreePath ) == 0 )
						{
							pTheFileNode = pListFileNode;
							break;
						}
					}
					if( pTheFileNode == NULL )
					{
						pTheFileNode = pListFileNode;
					}
				}
			}
		}
	}

	if( pTheFileNode )
	{
		pTheFileNode->AddRef();
	}

	return pTheFileNode;
}


/////////////////////////////////////////////////////////////////////////////
// CProject::GetDefaultRuntimeFolderByExt

void CProject::GetDefaultRuntimeFolderByExt( LPCTSTR szExt, CString& strDefaultRuntimeFolder )
{
	strDefaultRuntimeFolder.Empty();
	
	CString strExt = szExt;
	BSTR bstrExt = strExt.AllocSysString();

	IDMUSProdDocType* pIDocType;

	if( SUCCEEDED ( theApp.m_pFramework->FindDocTypeByExtension( bstrExt, &pIDocType ) ) )
	{
		CJazzDocTemplate* pDocTemplate = theApp.FindDocTemplate( pIDocType );
		pIDocType->Release();
		pIDocType = NULL;

		if( pDocTemplate )
		{
			CJzRuntimeFolder* pJzRuntimeFolder;

			POSITION pos = m_lstRuntimeFolders.GetHeadPosition();
			while( pos )
			{
				pJzRuntimeFolder = m_lstRuntimeFolders.GetNext( pos );

				if( pJzRuntimeFolder->m_pDocTemplate == pDocTemplate )
				{
					strDefaultRuntimeFolder = pJzRuntimeFolder->m_strRuntimeFolder;
					break;
				}
			}
		}
	}

	if( strDefaultRuntimeFolder.IsEmpty() )
	{
		strDefaultRuntimeFolder = m_strRuntimeDir;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CProject::GetUniqueFileName

void CProject::GetUniqueFileName( CString& strFileName )
{
	CString	strOrigPath;
	CString	strOrigName;
	CString	strOrigExt;
	CString	strNbr;
	int	i;

	int nFindPos = strFileName.ReverseFind( (TCHAR)'.' );
	if( nFindPos != -1 )
	{
		strOrigName = strFileName.Left( nFindPos );
		strOrigExt = strFileName.Right( strFileName.GetLength() - nFindPos );

		nFindPos = strOrigName.ReverseFind( (TCHAR)'\\' );
		if( nFindPos != -1 )
		{
			strOrigPath = strOrigName.Left( nFindPos + 1 );
			strOrigName = strOrigName.Right( strOrigName.GetLength() - nFindPos - 1 );
		}
	}

	while( !strOrigName.IsEmpty()  &&  _istdigit(strOrigName[strOrigName.GetLength() - 1]) )
	{
		strNbr = strOrigName.Right(1) + strNbr;
		strOrigName = strOrigName.Left( strOrigName.GetLength() - 1 );
	}

	i = _ttoi( strNbr );

	while( FindFileNode( strFileName ) )
	{
		strNbr.Format( "%d", ++i ); 
		strFileName = strOrigPath + strOrigName + strNbr + strOrigExt;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CProject::::HandleRuntimeDuplicates

BOOL CProject::HandleRuntimeDuplicates( void )
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	CFileNode* pDupeFileNode;
	CFileNode* pFileNode;
	CString strRuntimePath;
    POSITION pos;
	
	// Initialize all "Runtime" Save Action flags to RSA_NOACTION
	pos = m_lstFiles.GetHeadPosition();
    while( pos )
    {
        pFileNode = m_lstFiles.GetNext( pos );
		pFileNode->m_nRuntimeSaveAction = RSA_NOACTION;
    }
	
	// Determine if any files have duplicate "Runtime" target files
	pos = m_lstFiles.GetHeadPosition();
    while( pos )
    {
        pFileNode = m_lstFiles.GetNext( pos );

		if( pFileNode->m_nRuntimeSaveAction == RSA_NOACTION )
		{
			pFileNode->ConstructRuntimePath( strRuntimePath );
			strRuntimePath += pFileNode->m_strRuntimeFileName;

			pDupeFileNode = GetFileByRuntimeFileName( strRuntimePath, pFileNode, TRUE );
			if( pDupeFileNode )
			{
				pDupeFileNode->Release();
				pDupeFileNode = NULL;

				CRuntimeDupeDlg RuntimeDupeDlg;
				RuntimeDupeDlg.m_pFileNode = pFileNode;

				if( RuntimeDupeDlg.DoModal() == IDCANCEL )
				{
					return FALSE;
				}
			}
			else
			{
				pFileNode->m_nRuntimeSaveAction = RSA_SAVE;
			}
		}
    }

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CProject::AdjustProjectFolders

void CProject::AdjustProjectFolders( LPCTSTR szOrigDir, LPCTSTR szNewDir )
{
	CString strOrigDir = szOrigDir;
	CString strNewDir = szNewDir;
	CString strFolder;

	// Adjust Project files
	CFileNode* pFileNode;

	POSITION pos = m_lstFiles.GetHeadPosition();
	while( pos )
	{
		pFileNode = m_lstFiles.GetNext( pos );

		if( pFileNode->m_strRuntimeFile.Find( strOrigDir ) == 0 )
		{
			strFolder = pFileNode->m_strRuntimeFile.Right( pFileNode->m_strRuntimeFile.GetLength() - strOrigDir.GetLength() ); 
			pFileNode->m_strRuntimeFile = strNewDir + strFolder; 
		}

		if( pFileNode->m_strRuntimeFolder.Find( strOrigDir ) == 0 )
		{
			strFolder = pFileNode->m_strRuntimeFolder.Right( pFileNode->m_strRuntimeFolder.GetLength() - strOrigDir.GetLength() ); 
			pFileNode->m_strRuntimeFolder = strNewDir + strFolder; 
		}
	}

	// Adjust default "Runtime" folder for each file extension
	CJzRuntimeFolder* pJzRuntimeFolder;

	pos = m_lstRuntimeFolders.GetHeadPosition();
	while( pos )
	{
		pJzRuntimeFolder = m_lstRuntimeFolders.GetNext( pos );
		if( pJzRuntimeFolder->m_strRuntimeFolder.Find( strOrigDir ) == 0 )
		{
			strFolder = pJzRuntimeFolder->m_strRuntimeFolder.Right( pJzRuntimeFolder->m_strRuntimeFolder.GetLength() - strOrigDir.GetLength() ); 
			pJzRuntimeFolder->m_strRuntimeFolder = strNewDir + strFolder; 
		}
	}

	// Adjust Project default "Runtime" folder
	if( m_strRuntimeDir.Find( strOrigDir ) == 0 )
	{
		strFolder = m_strRuntimeDir.Right( m_strRuntimeDir.GetLength() - strOrigDir.GetLength() ); 
		m_strRuntimeDir = strNewDir + strFolder; 
	}
}


/////////////////////////////////////////////////////////////////////////////
// CProject::::GenerateNewGUIDs

void CProject::GenerateNewGUIDs( void )
{
	CFileNode* pFileNode;

	// New GUID for Project
	CoCreateGuid( &m_guid ); 

    // New GUIDs for all files in Project
	POSITION pos = m_lstFiles.GetHeadPosition();
    while( pos )
    {
        pFileNode = m_lstFiles.GetNext( pos );
		CoCreateGuid( &pFileNode->m_guid ); 
    }
}


/////////////////////////////////////////////////////////////////////////////
// CProject::::CanClose

BOOL CProject::CanClose( void )
{
	CFileNode* pFileNode;
	CFileNode* pTheFileNode;
	CJzNotifyNode* pJzNotifyNode;
	POSITION posNotifyList;

    // If this Project contains files referenced by other Projects
	// we must warn the user that links will be broken
	BOOL fWarnUser = FALSE;

    // Does this Project contain files being used by other Projects?
	POSITION pos = m_lstFiles.GetHeadPosition();
    while( pos  &&  (fWarnUser == FALSE) )
    {
        pFileNode = m_lstFiles.GetNext( pos );

		posNotifyList = pFileNode->m_lstNotifyNodes.GetHeadPosition();
		while( posNotifyList  &&  (fWarnUser == FALSE) )
		{
			pJzNotifyNode = static_cast<CJzNotifyNode*>( pFileNode->m_lstNotifyNodes.GetNext(posNotifyList) );
			
			pTheFileNode = theApp.GetFileByGUID( pJzNotifyNode->m_guidFile );
			if( pTheFileNode )
			{
				if( pTheFileNode->m_pProject != this )
				{
					fWarnUser = TRUE;
				}

				pTheFileNode->Release();
			}
		}
	}

	if( fWarnUser )
	{
		CCloseProjectDlg dlgCloseProject;

		// Initialize dlgCloseProject
		dlgCloseProject.m_pProject = this;

		// Display dialog
		int nAnswer = dlgCloseProject.DoModal();

		if( nAnswer == IDNO
		||  nAnswer == IDCANCEL )
		{
			return FALSE;
		}
	}

	return TRUE;
}
