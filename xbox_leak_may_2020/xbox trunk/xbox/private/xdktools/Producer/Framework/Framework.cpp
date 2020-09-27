// Framework.cpp : implementation file
//

/*-----------
@doc DMUSPROD
-----------*/

#include "stdafx.h"
#include "JazzApp.h"
#include "MainFrm.h"
#include "Framework.h"
#include "ioJazzDoc.h"
#include "DeleteFileDlg.h"
#include "WhichProjectDlg.h"
#include "ClientToolBar.h"
#include <ContainerDesigner.h>
#include <PrivateUnpackingFiles.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CJzComponent constructor/destructor 

CJzComponent::CJzComponent( CLSID clsidComponent, IDMUSProdComponent* pIComponent )
{
	ASSERT( pIComponent != NULL );

    m_clsidComponent = clsidComponent;
	m_pIComponent = pIComponent;
	m_fInitialized = FALSE;
}


CJzComponent::~CJzComponent()
{
	if( m_pIComponent )
	{
		m_pIComponent->CleanUp();
		m_pIComponent->Release();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CJzMenu constructor/destructor 

CJzMenu::CJzMenu( IDMUSProdMenu* pIMenu, UINT nCommandID )
{
	ASSERT( pIMenu != NULL );
	ASSERT( nCommandID >= FIRST_ADDINS_MENU_ID );

	m_pIMenu = pIMenu;
	m_pIMenu->AddRef();

    m_nCommandID = nCommandID;
}


CJzMenu::~CJzMenu()
{
	if( m_pIMenu )
	{
		m_pIMenu->Release();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CJzToolBar constructor/destructor 

CJzToolBar::CJzToolBar()
{
    m_pClientToolBar = NULL;
    m_nControlID = -1;
}


CJzToolBar::~CJzToolBar()
{
	if( m_pClientToolBar )
	{
		if( m_pClientToolBar->GetSafeHwnd() )
		{
			m_pClientToolBar->DestroyWindow();
		}
		delete m_pClientToolBar;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CJzNode constructor/destructor 

CJzNode::CJzNode( IDMUSProdNode* pINode )
{
	ASSERT( pINode != NULL );

	m_pINode = pINode;
	m_pINode->AddRef();

	ZeroMemory( &m_wp, sizeof(m_wp) );
}


CJzNode::~CJzNode()
{
	if( m_pINode )
	{
		m_pINode->Release();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CJzNotifyNode constructor/destructor 

CJzNotifyNode::CJzNotifyNode()
{
    m_pINotifyThisNode = NULL;
	m_nUseCount = 0;

	memset( &m_guidFile, 0, sizeof(m_guidFile) );
}


CJzNotifyNode::~CJzNotifyNode()
{
	if( m_pINotifyThisNode )
	{
		m_pINotifyThisNode->Release();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CJzClipFormat

CJzClipFormat::CJzClipFormat( UINT uClipFormat, LPCTSTR szExt )
{
	ASSERT( uClipFormat != 0 );
	ASSERT( szExt != NULL );

    m_uClipFormat = uClipFormat;
	m_strExt = szExt;
}


CJzClipFormat::~CJzClipFormat()
{
}


/////////////////////////////////////////////////////////////////////////////
// CJzSharedObject

CJzSharedObject::CJzSharedObject( REFCLSID clsid, IUnknown* pIUnknown )
{
	ASSERT( pIUnknown != NULL );

    m_clsid = clsid;

	m_pIUnknown = pIUnknown;
	m_pIUnknown->AddRef();
}


CJzSharedObject::~CJzSharedObject()
{
	if( m_pIUnknown )
	{
		m_pIUnknown->Release();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CFramework constructor/destructor 

CFramework::CFramework()
{
    m_dwRef = 1;
	m_nNextMenuID = FIRST_ADDINS_MENU_ID;
	m_nNextToolBarID = FIRST_TOOLBAR_ID;
}

CFramework::~CFramework()
{
	CleanUp();
	theApp.m_pFramework = NULL;
}

void CFramework::CleanUp()
{
	FreeDocTypes();
	FreeComponents();
	FreeClipFormats();
	FreeSharedObjects();
}


/////////////////////////////////////////////////////////////////////////////
// CFramework implementation

/////////////////////////////////////////////////////////////////////////////
// CFramework::LoadComponents

BOOL CFramework::LoadComponents( void )
{
	HKEY	  hKeyOpen;
	HKEY	  hKeyOpenClsId;
	DWORD	  dwIndex;
	LONG	  lResult;
	BOOL	  fRegMsg = FALSE;
	BOOL	  fLoadClsId;
	DWORD	  dwType;
	DWORD	  dwCbData;
	FILETIME  ftFileTime;
	DWORD     dwSkipValue;
	IDMUSProdComponent* pIComponent;
	_TCHAR	  achClsId[MID_BUFFER];
	_TCHAR	  achName[MID_BUFFER];

	lResult = ::RegOpenKeyEx( HKEY_LOCAL_MACHINE,
						  	  _T("Software\\Microsoft\\DMUSProducer\\Components"),
							  0, KEY_READ, &hKeyOpen );
	if( lResult != ERROR_SUCCESS )
	{
		AfxMessageBox( IDS_ERR_NO_COMPONENTS );
		return FALSE;
	}

	dwIndex = 0;
	for( ; ; )
	{
		dwCbData = MID_BUFFER;
		lResult  = ::RegEnumKeyEx( hKeyOpen, dwIndex++, achClsId, &dwCbData,
								   NULL, NULL, NULL, &ftFileTime );
		if( lResult == ERROR_NO_MORE_ITEMS )
		{
			break;
		}
		if( lResult != ERROR_SUCCESS )
		{
			fRegMsg  = TRUE;
			AfxMessageBox( IDS_ERR_REG_COMPONENT );
			break;
		}

		_tcscpy( achName, _T("") );
		fLoadClsId = TRUE;
		lResult    = ::RegOpenKeyEx( hKeyOpen, achClsId,
								     0, KEY_QUERY_VALUE, &hKeyOpenClsId );
		if( lResult == ERROR_SUCCESS )
		{
			dwCbData = MID_BUFFER;
			lResult  = ::RegQueryValueEx( hKeyOpenClsId, _T(""), NULL,
										  &dwType, (LPBYTE)&achName, &dwCbData );
			if( (lResult != ERROR_SUCCESS)
			||  (dwType != REG_SZ) )
			{
				_tcscpy( achName, _T("") );
			}

			dwCbData = sizeof(DWORD);
			lResult  = ::RegQueryValueEx( hKeyOpenClsId, _T("Skip"), NULL,
										  &dwType, (LPBYTE)&dwSkipValue, &dwCbData );
			if( (lResult == ERROR_SUCCESS)
			&&  (dwType == REG_DWORD)
			&&  (dwSkipValue == 1) )
			{
				fLoadClsId = FALSE;
			}
		
			::RegCloseKey( hKeyOpenClsId );
		}

		if( fLoadClsId )
		{
			CLSID clsid;
			wchar_t awchClsId[80];

			if( MultiByteToWideChar( CP_ACP, 0, achClsId, -1, awchClsId, sizeof(awchClsId) / sizeof(wchar_t) ) != 0 )
			{
				IIDFromString( awchClsId, &clsid );
			}
			else
			{
				memset( &clsid, 0, sizeof(clsid) );
			}

			if( SUCCEEDED(::CoCreateInstance(clsid, NULL, CLSCTX_INPROC_SERVER,
						  IID_IDMUSProdComponent, (void**)&pIComponent)) )
			{
				CJzComponent* pJzComponent = new CJzComponent( clsid, pIComponent );
				if( pJzComponent )
				{
					m_lstComponents.AddTail( pJzComponent );
				}
				else
				{
					AfxMessageBox( IDS_ERR_MEMORY );
					pIComponent->Release();
				}
			}
			else
			{
				CString strMsg;

				AfxFormatString2( strMsg, IDS_ERR_LOAD_COMPONENT, achName, achClsId );
				AfxMessageBox( strMsg );
			}
		}
	}

	::RegCloseKey( hKeyOpen );

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CFramework::InitComponents

BOOL CFramework::InitComponents( void )
{
	HRESULT hr;
	IDMUSProdComponent* pIComponent;
	IDMUSProdComponent* pINextComponent;

	BSTR bstrErrMsg = NULL;

    hr = GetFirstComponent( &pINextComponent );

	while( SUCCEEDED( hr )  &&  pINextComponent )
    {
		pIComponent = pINextComponent;

		CJzComponent* pJzComponent = GetJzComponent( pIComponent );
		ASSERT( pJzComponent != NULL );		// Should not happen!

		if( pJzComponent->m_fInitialized  )
		{
			hr = S_OK;
		}
		else
		{
			hr = pIComponent->Initialize( (IDMUSProdFramework *)this, &bstrErrMsg );
		}
		if( SUCCEEDED( hr ) )
		{
			pJzComponent->m_fInitialized = TRUE;

			hr = GetNextComponent( pIComponent, &pINextComponent );
		}
		else
		{
			BSTR	 bstrName;
			CString  strName;
			CString  strMsg;
			CString  strErrMsg;
			TCHAR    achClsId[MID_BUFFER];
			LPOLESTR pszClsId;

			if( hr == E_POINTER
			||  hr == E_INVALIDARG )
			{
				strErrMsg.LoadString( IDS_ERR_INVALIDARG );
			}
			else if( hr == E_OUTOFMEMORY )
			{
				strErrMsg.LoadString( IDS_ERR_NO_MEMORY );
			}
			else
			{
				if( bstrErrMsg )
				{
					strErrMsg = bstrErrMsg;
				}
			}
			if( bstrErrMsg )
			{
				::SysFreeString( bstrErrMsg );
			}

			if( SUCCEEDED ( pIComponent->GetName( &bstrName ) ) )
			{
				strName = bstrName;
				::SysFreeString( bstrName );
			}
			else
			{
				strName.Empty();
			}

			if( pJzComponent
			&&  SUCCEEDED( StringFromIID( pJzComponent->m_clsidComponent, &pszClsId ) ) )
			{
				WideCharToMultiByte( CP_ACP, 0, pszClsId, -1, achClsId, sizeof( achClsId ), NULL, NULL );
				CoTaskMemFree( pszClsId );
			}
			else
			{
				_tcscpy( achClsId, _T("") );
			}

		    hr = GetNextComponent( pIComponent, &pINextComponent );

			if( pJzComponent )
			{
				POSITION pos = m_lstComponents.Find( pJzComponent );
				if( pos )
				{
					m_lstComponents.RemoveAt( pos );
				}
				delete pJzComponent;
			}

			AfxFormatString2( strMsg, IDS_ERR_INIT_COMPONENT, strName, achClsId );
			strMsg += _T("\n\n");
			strMsg += strErrMsg;
			AfxMessageBox( strMsg );
		}

		pIComponent->Release();
    }

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CFramework::FreeComponents

void CFramework::FreeComponents( void )
{
	CJzComponent* pJzComponent;

	while( !m_lstComponents.IsEmpty() )
	{
		pJzComponent = static_cast<CJzComponent*>( m_lstComponents.RemoveHead() );
		delete pJzComponent;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CFramework::FreeDocTypes

void CFramework::FreeDocTypes( void )
{
	IDMUSProdDocType* pIDocType;

	while( !m_lstDocTypes.IsEmpty() )
	{
		pIDocType = static_cast<IDMUSProdDocType*>( m_lstDocTypes.RemoveHead() );
		pIDocType->Release();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CFramework::FreeClipFormats

void CFramework::FreeClipFormats( void )
{
	CJzClipFormat* pJzClipFormat;

	while( !m_lstClipFormats.IsEmpty() )
	{
		pJzClipFormat = static_cast<CJzClipFormat*>( m_lstClipFormats.RemoveHead() );
		delete pJzClipFormat;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CFramework::FreeSharedObjects

void CFramework::FreeSharedObjects( void )
{
	CJzSharedObject* pJzSharedObject;

	while( !m_lstSharedObjects.IsEmpty() )
	{
		pJzSharedObject = static_cast<CJzSharedObject*>( m_lstSharedObjects.RemoveHead() );
		delete pJzSharedObject;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CFramework::OnActivateApp

void CFramework::OnActivateApp( BOOL fActive ) 
{
	CJzComponent* pJzComponent;

    POSITION pos = m_lstComponents.GetHeadPosition();
    while( pos )
    {
        pJzComponent = m_lstComponents.GetNext( pos );

		pJzComponent->m_pIComponent->OnActivateApp( fActive );
    }
}


/////////////////////////////////////////////////////////////////////////////
// CFramework::RegisterClipboardFormats

BOOL CFramework::RegisterClipboardFormats( void )
{
	m_cfProducerFile = ::RegisterClipboardFormat( CF_DMUSPROD_FILE );

	if( m_cfProducerFile == 0 )
	{
		return FALSE;
	}

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CFramework::SetNodeName

BOOL CFramework::SetNodeName( IDMUSProdNode* pINode, LPCTSTR szNewName )
{
	if( pINode )
	{
		CString strNewName = szNewName;
		BSTR bstrNewName = strNewName.AllocSysString();

		pINode->SetNodeName( bstrNewName );

		HTREEITEM hItem = FindTreeItem( pINode );
		if( hItem )
		{
			CTreeCtrl* pTreeCtrl = theApp.GetProjectTreeCtrl();
			if( pTreeCtrl )
			{
				pTreeCtrl->SetItemText( hItem, strNewName );
				return TRUE;
			}
		}
	}

	return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CFramework::DetermineParentNode

IDMUSProdNode* CFramework::DetermineParentNode( LPCTSTR szFileName )
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	CTreeCtrl* pTreeCtrl = theApp.GetProjectTreeCtrl();

	if( pTreeCtrl == NULL )
	{
		return NULL;
	}

	IDMUSProdNode* pIParentNode = NULL;

	switch( theApp.m_nFileTarget )
	{
		case TGT_FILENODE:
		{
			ASSERT( szFileName != NULL );

			CFileNode* pFileNode = theApp.FindFileNode( szFileName );
			ASSERT( pFileNode != NULL );

			if( pFileNode )
			{
				pIParentNode = pFileNode->m_pIParentNode;
				ASSERT( pIParentNode != NULL );
			}
			break;
		}

		case TGT_FILENODE_SAVEAS:
		{
			ASSERT( szFileName != NULL );

			CProject* pProject = theApp.GetProjectByFileName( szFileName );
			if( pProject )
			{
				CFileNode* pFileNode = pProject->FindFileNode( szFileName );

				if( pFileNode == NULL )
				{
					// Create a FileNode
					pFileNode = pProject->CreateFileNode( szFileName );
				}

				if( pFileNode )
				{
					pIParentNode = pFileNode->m_pIParentNode;
					ASSERT( pIParentNode != NULL );
				}

				pProject->Release();
			}
			break;
		}

		case TGT_SELECTEDNODE:
		{
			IDMUSProdProject* pIProject;
			GUID guidNodeId;

			// Make sure there is a Project
			theApp.GetFirstProject( &pIProject );
			if( pIProject )
			{
				pIProject->Release();
			}
			else
			{
				theApp.CreateNewProject();
			}

			// First directory node will be the parent
			HTREEITEM hItem = pTreeCtrl->GetSelectedItem();
			while( hItem )
			{
				CJzNode* pJzNode = (CJzNode *)pTreeCtrl->GetItemData( hItem );
				if( pJzNode )
				{
					ASSERT( pJzNode->m_pINode != NULL );

					if( pJzNode->m_pINode )
					{
						if( SUCCEEDED ( pJzNode->m_pINode->GetNodeId( &guidNodeId ) ) )
						{
							if( IsEqualGUID( guidNodeId, GUID_ProjectFolderNode )
							||  IsEqualGUID( guidNodeId, GUID_DirectoryNode ) )
							{
								pIParentNode = pJzNode->m_pINode;
								ASSERT( pIParentNode != NULL );
								break;
							}
						}
					}
				}

				hItem = pTreeCtrl->GetNextItem( hItem, TVGN_PARENT );
			}
			break;
		}
		
		case TGT_PROJECT:
		{
			CString strProjectFileName;

			theApp.FindProjectFileName( szFileName, strProjectFileName );
			if( strProjectFileName.IsEmpty() )
			{
				pIParentNode = (IDMUSProdNode *)theApp.CreateNewProject();
			}
			else
			{
				HINSTANCE hInstance = AfxGetResourceHandle();
				AfxSetResourceHandle( theApp.m_hInstance );

				CWhichProjectDlg wpDlg;
				wpDlg.m_strFileName = szFileName;
				int nResult = wpDlg.DoModal();

				AfxSetResourceHandle( hInstance );
			
				switch( nResult )
				{
					case IDC_NEW_PROJECT:
						pIParentNode = (IDMUSProdNode *)theApp.CreateNewProject();
						break;

					case IDC_USE_PROJECT:
						if( theApp.OpenDocumentFile( strProjectFileName ) )
						{
							CFileNode* pFileNode = theApp.FindFileNode( szFileName );
							if( pFileNode )
							{
								pIParentNode = pFileNode->m_pIParentNode;
								ASSERT( pIParentNode != NULL );
							}
						}
						break;
				}
			}
			break;
		}
	}

	return pIParentNode;
}


/////////////////////////////////////////////////////////////////////////////
// CFramework::SearchTree

HTREEITEM CFramework::SearchTree( CTreeCtrl* pTreeCtrl, IDMUSProdNode* pINode, HTREEITEM hItem )
{
	HTREEITEM hNodeItem = NULL;
	HTREEITEM hChildItem;
	IDMUSProdNode* pITreeNode;

	ASSERT( pTreeCtrl != NULL );
	ASSERT( pINode != NULL );
	ASSERT( hItem != NULL );

	while( hItem )
	{
		CJzNode* pJzNode = (CJzNode *)pTreeCtrl->GetItemData( hItem );
		if( pJzNode )
		{
			pITreeNode = pJzNode->m_pINode;
			ASSERT( pITreeNode != NULL );

			if( pITreeNode == pINode )
			{
				hNodeItem = hItem;
			}
			else
			{
				hChildItem = hItem;
				while( hChildItem = pTreeCtrl->GetNextItem(hChildItem, TVGN_CHILD) )
				{
					hNodeItem = SearchTree( pTreeCtrl, pINode, hChildItem );
					if( hNodeItem )
					{
						break;
					}
				}
			}
		}

		if( hNodeItem )
		{
			break;
		}

		hItem = pTreeCtrl->GetNextItem( hItem, TVGN_NEXT );
	}

	return hNodeItem;
}


/////////////////////////////////////////////////////////////////////////////
// CFramework::FindTreeItem

HTREEITEM CFramework::FindTreeItem( IDMUSProdNode* pINode )
{
	HTREEITEM hNodeItem = NULL;

	ASSERT( pINode != NULL );

	CTreeCtrl* pTreeCtrl = theApp.GetProjectTreeCtrl();
	if( pTreeCtrl == NULL )
	{
		return NULL;
	}

	HTREEITEM hItem = pTreeCtrl->GetNextItem( NULL, TVGN_CHILD );		// first child in tree

	if( hItem )
	{
		GUID guidNodeId;
		BOOL fSearchFinished = FALSE;

		if( SUCCEEDED ( pINode->GetNodeId( &guidNodeId ) ) )
		{
			if( ::IsEqualGUID( guidNodeId, GUID_ProjectFolderNode )
			||  ::IsEqualGUID( guidNodeId, GUID_DirectoryNode ) )
			{
				CDirectoryNode* pDirNode = (CDirectoryNode *)pINode;

				hNodeItem = pDirNode->m_hItem;
				fSearchFinished = TRUE;
			}
			else if( IsEqualGUID( guidNodeId, GUID_FileNode ) )
			{
				CFileNode* pFileNode = (CFileNode *)pINode;

				hNodeItem = pFileNode->m_hItem;
				fSearchFinished = TRUE;
			}
		}

		if( fSearchFinished == FALSE )
		{
			IDMUSProdNode* pIDocRootNode;

			if( SUCCEEDED ( pINode->GetDocRootNode( &pIDocRootNode ) ) )
			{
				if( pIDocRootNode )
				{
					CFileNode* pFileNode = theApp.GetFileByDocRootNode( pIDocRootNode );
					if( pFileNode )
					{
						HTREEITEM hDocRootItem = pFileNode->m_hChildItem;
						if( hDocRootItem )
						{
							if( pINode == pIDocRootNode )
							{
								hNodeItem = hDocRootItem;
							}
							else
							{
								// Search all nodes under DocRootNode
								hNodeItem = SearchTree( pTreeCtrl, pINode, hDocRootItem );
							}
						}

						pFileNode->Release();
					}

					pIDocRootNode->Release();
				}
			}
		}
	}

	return hNodeItem;
}


/////////////////////////////////////////////////////////////////////////////
// CFramework::FindTreeProjectByGUID

HTREEITEM CFramework::FindTreeProjectByGUID( GUID guidProject )
{
	HTREEITEM hProjectNodeItem = NULL;

	CTreeCtrl* pTreeCtrl = theApp.GetProjectTreeCtrl();
	if( pTreeCtrl == NULL )
	{
		return NULL;
	}

	HTREEITEM hItem = pTreeCtrl->GetNextItem( NULL, TVGN_CHILD );		// first child in tree
	
	IDMUSProdNode* pITreeNode;
	IDMUSProdProject* pITreeProject;
	GUID guidTreeProject;

	while( hItem )
	{
		CJzNode* pJzNode = (CJzNode *)pTreeCtrl->GetItemData( hItem );
		if( pJzNode )
		{
			pITreeNode = pJzNode->m_pINode;
			ASSERT( pITreeNode != NULL );

			if( SUCCEEDED ( pITreeNode->QueryInterface( IID_IDMUSProdProject, (void **)&pITreeProject ) ) )
			{
				if( SUCCEEDED ( pITreeProject->GetGUID ( &guidTreeProject ) ) )
				{
					if( IsEqualGUID( guidProject, guidTreeProject ) )
					{
						hProjectNodeItem = hItem;
					}
				}

				pITreeProject->Release();

				if( hProjectNodeItem )
				{
					break;
				}
			}
		}

		hItem = pTreeCtrl->GetNextItem( hItem, TVGN_NEXT );
	}

	return hProjectNodeItem;
}


/////////////////////////////////////////////////////////////////////////////
// CFramework::SearchTreeForWP

HTREEITEM CFramework::SearchTreeForWP( CTreeCtrl* pTreeCtrl, wpWindowPlacement* pWP, long lTreePos, HTREEITEM hItem )
{
	HTREEITEM hNodeItem = NULL;
	HTREEITEM hNodeItemSecondChoice = NULL;
	HTREEITEM hChildItem;
	IDMUSProdNode* pITreeNode;

	ASSERT( pTreeCtrl != NULL );
	ASSERT( pWP != NULL );
	ASSERT( hItem != NULL );

	while( hItem )
	{
		CJzNode* pJzNode = (CJzNode *)pTreeCtrl->GetItemData( hItem );
		if( pJzNode )
		{
			pITreeNode = pJzNode->m_pINode;
			ASSERT( pITreeNode != NULL );

			GUID guidNodeId;
			if( SUCCEEDED ( pITreeNode->GetNodeId( &guidNodeId ) ) )
			{
				// Is the Node ID equal?
				if( IsEqualGUID ( guidNodeId, pWP->guidNodeId ) )
				{
					BSTR bstrNodeName;
					if( SUCCEEDED ( pITreeNode->GetNodeName ( &bstrNodeName ) ) )
					{
						CString strNodeName = bstrNodeName;
						::SysFreeString( bstrNodeName );

						// Is node name equal?
						if( strNodeName == pWP->strNodeName )
						{
							// Is the tree position equal?
							if( lTreePos == pWP->lTreePos )
							{
								// This is a match
								hNodeItem = hItem;
								break;
							}
							else
							{
								// This is second choice
								hNodeItemSecondChoice = hItem;
							}
						}
					}
				}
			}
		}

		if( hNodeItem == NULL )
		{
			hChildItem = hItem;
			while( hChildItem = pTreeCtrl->GetNextItem(hChildItem, TVGN_CHILD) )
			{
				hNodeItem = SearchTreeForWP( pTreeCtrl, pWP, ++lTreePos, hChildItem );
				if( hNodeItem )
				{
					break;
				}
			}
		}

		if( hNodeItem )
		{
			break;
		}

		hItem = pTreeCtrl->GetNextItem( hItem, TVGN_NEXT );
		lTreePos++;
	}

	if( hNodeItem == NULL )
	{
		hNodeItem = hNodeItemSecondChoice;
	}

	return hNodeItem;
}


/////////////////////////////////////////////////////////////////////////////
// CFramework FindTreeItemByWP

HTREEITEM CFramework::FindTreeItemByWP( wpWindowPlacement* pWP )
{
	ASSERT( pWP != NULL );

	HTREEITEM hItemWP = NULL;

	// Get the FileNode
	CFileNode* pFileNode = theApp.GetFileByGUID( pWP->guidFile );
	if( pFileNode )
	{
		if( pFileNode->m_pIChildNode )
		{
			// Get the File's DocRoot node
			HTREEITEM hItem = theApp.m_pFramework->FindTreeItem( pFileNode->m_pIChildNode );
			if( hItem )
			{
				CTreeCtrl* pTreeCtrl = theApp.GetProjectTreeCtrl();
				if( pTreeCtrl )
				{
					// Get the
					hItemWP = SearchTreeForWP( pTreeCtrl, pWP, 1, hItem );
				}
			}
		}

		pFileNode->Release();
	}

	return hItemWP;
}


/////////////////////////////////////////////////////////////////////////////
// CFramework::CountWPTreePos

long CFramework::CountWPTreePos( CTreeCtrl* pTreeCtrl, IDMUSProdNode* pINode, long lTreePos, HTREEITEM hItem )
{
	long lPos = 0;
	HTREEITEM hChildItem;
	IDMUSProdNode* pITreeNode;

	ASSERT( pTreeCtrl != NULL );
	ASSERT( pINode != NULL );
	ASSERT( hItem != NULL );

	while( hItem )
	{
		CJzNode* pJzNode = (CJzNode *)pTreeCtrl->GetItemData( hItem );
		if( pJzNode )
		{
			pITreeNode = pJzNode->m_pINode;
			ASSERT( pITreeNode != NULL );

			// Is the node equal?
			if( pITreeNode == pINode )
			{
				lPos = lTreePos;
				break;
			}
		}

		if( lPos == 0 )
		{
			hChildItem = hItem;
			while( hChildItem = pTreeCtrl->GetNextItem(hChildItem, TVGN_CHILD) )
			{
				lPos = CountWPTreePos( pTreeCtrl, pINode, ++lTreePos, hChildItem );
				if( lPos )
				{
					break;
				}
			}
		}

		if( lPos )
		{
			break;
		}

		hItem = pTreeCtrl->GetNextItem( hItem, TVGN_NEXT );
		lTreePos++;
	}

	return lPos;
}


/////////////////////////////////////////////////////////////////////////////
// CFramework GetWPTreePos

long CFramework::GetWPTreePos( IDMUSProdNode* pINode )
{
	ASSERT( pINode != NULL );

	HTREEITEM hItem = NULL;
	CTreeCtrl* pTreeCtrl = theApp.GetProjectTreeCtrl();
	if( pTreeCtrl )
	{
		hItem = pTreeCtrl->GetNextItem( NULL, TVGN_CHILD );		// first child in tree
	}
	if( hItem == NULL )
	{
		return 0;
	}

	long lTreePos = 0;

	IDMUSProdNode* pIDocRootNode;

	if( SUCCEEDED ( pINode->GetDocRootNode( &pIDocRootNode ) ) )
	{
		if( pIDocRootNode )
		{
			// Find the DocRootNode
			CFileNode* pFileNode = theApp.GetFileByDocRootNode( pIDocRootNode );
			if( pFileNode )
			{
				HTREEITEM hDocRootItem = pFileNode->m_hChildItem;
				if( hDocRootItem )
				{
					// Return tree position of pINode
					lTreePos = CountWPTreePos( pTreeCtrl, pINode, 1, hDocRootItem );
				}

				pFileNode->Release();
			}

			pIDocRootNode->Release();
		}
	}

	return lTreePos;
}


/////////////////////////////////////////////////////////////////////////////
// CFramework ApplyWPSettings

void CFramework::ApplyWPSettings( HTREEITEM hItem, wpWindowPlacement* pWP )
{
	ASSERT( hItem != NULL );
	ASSERT( pWP != NULL );

	CTreeCtrl* pTreeCtrl = theApp.GetProjectTreeCtrl();
	if( pTreeCtrl )
	{
		CJzNode* pJzNode = (CJzNode *)pTreeCtrl->GetItemData( hItem );
		ASSERT( pJzNode != NULL );

		pJzNode->m_wp.length = sizeof(pJzNode->m_wp);
		pJzNode->m_wp.flags = pWP->wp.flags;
		pJzNode->m_wp.showCmd = pWP->wp.showCmd;
		pJzNode->m_wp.ptMinPosition = pWP->wp.ptMinPosition;
		pJzNode->m_wp.ptMaxPosition = pWP->wp.ptMaxPosition;
		pJzNode->m_wp.rcNormalPosition = pWP->wp.rcNormalPosition;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CFramework::ShowTreeNode

BOOL CFramework::ShowTreeNode( IDMUSProdNode* pINode )
{
	if( theApp.m_nShowNodeInTree < 0 )
	{
		ASSERT( 0 );	// Should not happen
		theApp.m_nShowNodeInTree = SHOW_NODE_IN_TREE;
	}

	if( theApp.m_nShowNodeInTree == SHOW_NODE_IN_TREE )
	{
		if( pINode )
		{
			HTREEITEM hItem = FindTreeItem( pINode );

			if( hItem )
			{
				CTreeCtrl* pTreeCtrl = theApp.GetProjectTreeCtrl();
				if( pTreeCtrl )
				{
					CWnd* pWndHadFocus = CWnd::GetFocus();

					// BUG 52522: Let's disable 'auto-scrolling' in the project tree.  Files in use should continue to be highlighted, just not scrolled to.
					//pTreeCtrl->EnsureVisible( hItem );
					pTreeCtrl->LockWindowUpdate();
					HTREEITEM hItemFirstVis = pTreeCtrl->GetFirstVisibleItem();
					pTreeCtrl->SelectItem( hItem );
					if( hItemFirstVis )
					{
						pTreeCtrl->Select( hItemFirstVis, TVGN_FIRSTVISIBLE );
					}
					pTreeCtrl->UnlockWindowUpdate();

					if( pWndHadFocus == pTreeCtrl
					&&  pWndHadFocus != CWnd::GetFocus() )
					{
						pWndHadFocus->SetFocus();
					}

					return TRUE;
				}
			}
		}
	}

	return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CFramework::RemoveNodes

BOOL CFramework::RemoveNodes( IDMUSProdNode* pINode )
{
	ASSERT( pINode != NULL );

	HTREEITEM hItem = FindTreeItem( pINode );

	if( hItem )
	{
		CTreeCtrl* pTreeCtrl = theApp.GetProjectTreeCtrl();
		if( pTreeCtrl )
		{
			HTREEITEM hParentItem = pTreeCtrl->GetParentItem( hItem );

			if( pTreeCtrl->DeleteItem(hItem) )
			{
				if( hParentItem )
				{
					if( pTreeCtrl->GetChildItem(hParentItem) == NULL )	// no children
					{
						CJzNode* pJzNode = (CJzNode *)pTreeCtrl->GetItemData( hParentItem );
						if( pJzNode )
						{
							IDMUSProdNode* pIParentNode = pJzNode->m_pINode;
							ASSERT( pIParentNode != NULL );
							
							if( pIParentNode )
							{
								BOOL fUseOpenCloseImages;

								pIParentNode->UseOpenCloseImages( &fUseOpenCloseImages );

								if( fUseOpenCloseImages == TRUE )
								{
									short nImage;

									UINT nState = pTreeCtrl->GetItemState( hParentItem, TVIS_EXPANDED );
									pTreeCtrl->SetItemState( hParentItem, nState & ~TVIS_EXPANDED, TVIS_EXPANDED );
									pIParentNode->GetNodeImageIndex( &nImage );
									pTreeCtrl->SetItemImage( hParentItem, nImage, nImage );
								}
							}
						}
					}
				}

				theApp.DrawProjectTree();
				return TRUE;
			}
		}
	}

	return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CFramework::AddNodes

BOOL CFramework::AddNodes( IDMUSProdNode* pINode, HTREEITEM hParent )
{
	HRESULT hr;
    IDMUSProdNode* pIChild;
    IDMUSProdNode* pINextChild;
	CTreeCtrl* pTreeCtrl;
	HTREEITEM hItem;
	BSTR bstrNodeName;
	BOOL fUseOpenCloseImages;
    short nImage;

	ASSERT( pINode != NULL );
	if( pINode == NULL )
	{
		return FALSE;
	}

	pTreeCtrl = theApp.GetProjectTreeCtrl();
	if( pTreeCtrl == NULL )
	{
		return FALSE;
	}
	
	CJzNode* pJzNode = new CJzNode( pINode );
	if( pJzNode == NULL )
	{
		return FALSE;
	}

    pINode->GetNodeName( &bstrNodeName );
    pINode->GetNodeImageIndex( &nImage );
    
	hItem = pTreeCtrl->InsertItem( CString( bstrNodeName ), hParent );

	if( hItem )
	{
		GUID guidNodeId;

		pINode->GetNodeId( &guidNodeId );

		if( ::IsEqualGUID( guidNodeId, GUID_ProjectFolderNode )
		||  ::IsEqualGUID( guidNodeId, GUID_DirectoryNode ) )
		{
			CDirectoryNode* pDirNode = (CDirectoryNode *)pINode;

			pDirNode->m_hItem = hItem;
		}
		else if( ::IsEqualGUID( guidNodeId, GUID_FileNode ) )
		{
			CFileNode* pFileNode = (CFileNode *)pINode;

			pFileNode->m_hItem = hItem;
		}
		else
		{
			IDMUSProdNode* pIDocRootNode;

			if( SUCCEEDED ( pINode->GetDocRootNode( &pIDocRootNode ) ) )
			{
				if( pIDocRootNode )
				{
					if( pINode == pIDocRootNode )
					{
						CJzNode* pJzNode = (CJzNode *)pTreeCtrl->GetItemData( hParent );
						if( pJzNode )
						{
							ASSERT( pJzNode->m_pINode != NULL );

							if( pJzNode->m_pINode )
							{
								CFileNode* pFileNode = (CFileNode *)pJzNode->m_pINode;

								ASSERT( pFileNode->m_hChildItem == NULL ); 
								pFileNode->m_hChildItem = hItem;
							}
						}
					}

					pIDocRootNode->Release();
				}
			}
		}

		pINode->UseOpenCloseImages( &fUseOpenCloseImages );
		if( fUseOpenCloseImages == TRUE )
		{
			pTreeCtrl->SetItemImage( hItem, nImage, nImage );
		}
		else
		{
			pTreeCtrl->SetItemImage( hItem, nImage, nImage + 1 );
		}
		pTreeCtrl->SetItemData( hItem, reinterpret_cast<DWORD>( pJzNode ) );

		hr = pINode->GetFirstChild( &pINextChild );

		while( SUCCEEDED( hr )  &&  pINextChild )
		{
			pIChild = pINextChild;

			AddNodes( pIChild, hItem );

		    hr = pINode->GetNextChild( pIChild, &pINextChild );
			pIChild->Release();
		}
	}

	if( pTreeCtrl->GetNextItem( hItem, TVGN_CHILD ) )
	{
		CMainFrame* pMainFrame = (CMainFrame *)theApp.m_pMainWnd;
		ASSERT( pMainFrame != NULL );
		pMainFrame->m_wndTreeBar.SortChildren( hItem );
	}

	::SysFreeString( bstrNodeName );

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CFramework::OnInitAddInsMenu

void CFramework::OnInitAddInsMenu( CMenu* pAddInsMenu )
{
	CJzMenu* pJzMenu;
	
	POSITION pos = m_lstMenus.GetHeadPosition();

    while( pos != NULL )
    {
        pJzMenu = static_cast<CJzMenu*>( m_lstMenus.GetNext(pos) );
		if( pJzMenu )
		{
			pJzMenu->m_pIMenu->OnMenuInit( pAddInsMenu->GetSafeHmenu(), pJzMenu->m_nCommandID );
		}
    }
}


/////////////////////////////////////////////////////////////////////////////
// CFramework::OnSelectAddInsMenu

BOOL CFramework::OnSelectAddInsMenu( UINT nCommandID )
{
	CJzMenu* pJzMenu;
	
	POSITION pos = m_lstMenus.GetHeadPosition();

    while( pos != NULL )
    {
        pJzMenu = static_cast<CJzMenu*>( m_lstMenus.GetNext(pos) );
		if( pJzMenu )
		{
			if( pJzMenu->m_nCommandID == nCommandID )
			{
				pJzMenu->m_pIMenu->OnMenuSelect();
				return TRUE;
			}
		}
    }

	return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CFramework::GetAddInsMenuHelpText

void CFramework::GetAddInsMenuHelpText( UINT nCommandID, CString& rMessage )
{
	CJzMenu* pJzMenu;
	
	POSITION pos = m_lstMenus.GetHeadPosition();

    while( pos != NULL )
    {
        pJzMenu = static_cast<CJzMenu*>( m_lstMenus.GetNext(pos) );
		if( pJzMenu )
		{
			if( pJzMenu->m_nCommandID == nCommandID )
			{
				BSTR bstrMenuHelpText;

				pJzMenu->m_pIMenu->GetMenuHelpText( &bstrMenuHelpText );
				rMessage = bstrMenuHelpText;
				::SysFreeString( bstrMenuHelpText );
				break;
			}
		}
    }
}


/////////////////////////////////////////////////////////////////////////////
// CFramework::FixAddInsMenu

void CFramework::FixAddInsMenu( void )
{
	CMenu* pAddInsMenu = theApp.FindMenuByName( IDS_ADDINS_MENU_TEXT );
	if( pAddInsMenu == NULL )
	{
		return;
	}
	
	// remove crusty IDMUSProdMenu menu items
	UINT nMenuID = pAddInsMenu->GetMenuItemID( 0 );
	while( (nMenuID > FIRST_ADDINS_MENU_ID) && (nMenuID <= m_nNextMenuID) )
	{
		pAddInsMenu->RemoveMenu( nMenuID, MF_BYCOMMAND );
		nMenuID = pAddInsMenu->GetMenuItemID( 0 );
	}
	
	// insert separator
//	UINT nMenuState = pAddInsMenu->GetMenuState( 0, MF_BYPOSITION );
//	if( !(nMenuState & MF_SEPARATOR) )
//	{
//		pAddInsMenu->InsertMenu( 0, MF_BYPOSITION | MF_SEPARATOR ); 
//	}
	
	// insert current list of IDMUSProdMenu items
	CJzMenu* pJzMenu;

	POSITION pos = m_lstMenus.GetHeadPosition();

    while( pos != NULL )
    {
        pJzMenu = static_cast<CJzMenu*>( m_lstMenus.GetNext(pos) );
		if( pJzMenu )
		{
			CString strMenuText;
			BSTR bstrMenuText;

			pJzMenu->m_pIMenu->GetMenuText( &bstrMenuText );
			strMenuText = bstrMenuText;
			::SysFreeString( bstrMenuText );

			pAddInsMenu->InsertMenu( 0, MF_BYPOSITION | MF_STRING, pJzMenu->m_nCommandID, strMenuText ); 
		}
    }
}


/////////////////////////////////////////////////////////////////////////////
// CFramework::RedrawClientToolbars

void CFramework::RedrawClientToolbars()
{
	CJzToolBar* pJzToolBar;

	POSITION pos = m_lstToolBars.GetHeadPosition();
	while( pos )
	{
		pJzToolBar = static_cast<CJzToolBar*>( m_lstToolBars.GetNext(pos) );

		pJzToolBar->m_pClientToolBar->RedrawWindow( NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ALLCHILDREN );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CFramework::InsertViewMenuItem

void CFramework::InsertViewMenuItem( CJzToolBar* pJzToolBar )
{
	ASSERT( pJzToolBar != NULL );
	ASSERT( pJzToolBar->m_pClientToolBar != NULL );
	ASSERT( pJzToolBar->m_pClientToolBar->m_pIToolBar != NULL );

	CMenu* pViewMenu = theApp.FindMenuByName( IDS_VIEW_MENU_TEXT );

	if( pViewMenu )
	{
		UINT nPosition = (pJzToolBar->m_nControlID - FIRST_TOOLBAR_ID) + FIRST_TOOLBAR_ITEM;

		BSTR bstrMenuText;
		pJzToolBar->m_pClientToolBar->m_pIToolBar->GetMenuText( &bstrMenuText );
		CString strMenuText = bstrMenuText;
		::SysFreeString( bstrMenuText );

		pViewMenu->InsertMenu( nPosition, MF_BYPOSITION | MF_STRING, pJzToolBar->m_nControlID, strMenuText ); 
	}
}


/////////////////////////////////////////////////////////////////////////////
// CFramework::RemoveViewMenuItem

void CFramework::RemoveViewMenuItem( CJzToolBar* pJzToolBar )
{
	ASSERT( pJzToolBar != NULL );

	CMenu* pViewMenu = theApp.FindMenuByName( IDS_VIEW_MENU_TEXT );

	if( pViewMenu )
	{
		pViewMenu->RemoveMenu( pJzToolBar->m_nControlID, MF_BYCOMMAND );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CFramework::OnInitViewMenu

void CFramework::OnInitViewMenu( CMenu* pViewMenu )
{
	CMainFrame* pMainFrame = (CMainFrame *)theApp.m_pMainWnd;
	ASSERT( pMainFrame != NULL );

	CJzToolBar* pJzToolBar;
	CControlBar* pBar;
	UINT nChecked;
	UINT nEnabled;
	
	POSITION pos = m_lstToolBars.GetHeadPosition();

    while( pos != NULL )
    {
        pJzToolBar = static_cast<CJzToolBar*>( m_lstToolBars.GetNext(pos) );
		if( pJzToolBar )
		{
			nChecked = MF_UNCHECKED;
			nEnabled = MF_GRAYED;

			pBar = pMainFrame->GetControlBar( pJzToolBar->m_nControlID );
			if( pBar )
			{
				nEnabled = MF_ENABLED;
				if( (pBar->GetStyle() & WS_VISIBLE) )
				{
					nChecked = MF_CHECKED;
				}
			}

			::EnableMenuItem( pViewMenu->GetSafeHmenu(),
							  pJzToolBar->m_nControlID,
							  (nEnabled | MF_BYCOMMAND) );
			::CheckMenuItem(  pViewMenu->GetSafeHmenu(),
							  pJzToolBar->m_nControlID,
							  (nChecked | MF_BYCOMMAND) );
		}
    }
}


/////////////////////////////////////////////////////////////////////////////
// CFramework::OnSelectViewMenu

BOOL CFramework::OnSelectViewMenu( UINT nCommandID )
{
	if( (nCommandID >= FIRST_TOOLBAR_ID) && (nCommandID < m_nNextToolBarID) )
	{
		CMainFrame* pMainFrame = (CMainFrame *)theApp.m_pMainWnd;
		ASSERT( pMainFrame != NULL );

		CControlBar* pBar = pMainFrame->GetControlBar( nCommandID );
		if( pBar )
		{
			pMainFrame->OnBarCheck( nCommandID );
			return TRUE;
		}
	}

	return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CFramework::FixViewMenu

void CFramework::FixViewMenu( void )
{
	CMenu* pViewMenu = theApp.FindMenuByName( IDS_VIEW_MENU_TEXT );
	if( pViewMenu == NULL )
	{
		return;
	}
	
	// remove crusty IDMUSProdToolBar menu items
	UINT nMenuID;

	nMenuID = pViewMenu->GetMenuItemID( FIRST_TOOLBAR_ITEM );
	while( (nMenuID >= FIRST_TOOLBAR_ID) && (nMenuID < m_nNextToolBarID) )
	{
		pViewMenu->RemoveMenu( nMenuID, MF_BYCOMMAND );
		nMenuID = pViewMenu->GetMenuItemID( FIRST_TOOLBAR_ITEM );
	}
	
	// insert current list of IDMUSProdToolBar items
	CJzToolBar* pJzToolBar;

	POSITION pos = m_lstToolBars.GetHeadPosition();

    while( pos != NULL )
    {
        pJzToolBar = static_cast<CJzToolBar*>( m_lstToolBars.GetNext(pos) );
		if( pJzToolBar )
		{
			InsertViewMenuItem( pJzToolBar ); 
		}
    }
}

	
/////////////////////////////////////////////////////////////////////////////
// CFramework::GetViewMenuHelpText

void CFramework::GetViewMenuHelpText( UINT nCommandID, CString& rMessage )
{
	CJzToolBar* pJzToolBar;
	
	POSITION pos = m_lstToolBars.GetHeadPosition();

    while( pos != NULL )
    {
        pJzToolBar = static_cast<CJzToolBar*>( m_lstToolBars.GetNext(pos) );
		if( pJzToolBar )
		{
			if( pJzToolBar->m_nControlID == nCommandID )
			{
				BSTR bstrMenuHelpText;

				pJzToolBar->m_pClientToolBar->m_pIToolBar->GetMenuHelpText( &bstrMenuHelpText );
				rMessage = bstrMenuHelpText;
				::SysFreeString( bstrMenuHelpText );
				break;
			}
		}
    }
}


/////////////////////////////////////////////////////////////////////////////
// CFramework::GetJzComponent

CJzComponent* CFramework::GetJzComponent( IDMUSProdComponent* pIComponent )
{
	CJzComponent* pTheJzComponent = NULL;

    POSITION pos = m_lstComponents.GetHeadPosition();

    while( pos )
    {
        CJzComponent* pJzComponent = m_lstComponents.GetNext( pos );
		if( pJzComponent->m_pIComponent == pIComponent )
		{
			pTheJzComponent = pJzComponent;
			break;
		}
    }

	return pTheJzComponent;
}


/////////////////////////////////////////////////////////////////////////////
// CFramework IDMUSProdFramework implementation

/////////////////////////////////////////////////////////////////////////////
// CFramework::QueryInterface

HRESULT CFramework::QueryInterface( REFIID iid, void FAR* FAR* ppvObj )
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	if( ::IsEqualIID(iid, IID_IDMUSProdPropSheet) )
	{
		CMainFrame* pMainFrame = (CMainFrame *)theApp.m_pMainWnd;
		if( pMainFrame
		&&  pMainFrame->m_wndProperties.m_pPropertySheet  )
		{
			return pMainFrame->m_wndProperties.m_pPropertySheet->QueryInterface( iid, ppvObj );
		}
		else
		{
		    *ppvObj = NULL;
		    return E_NOINTERFACE;
		}
	}

    if( ::IsEqualIID(iid, IID_IDMUSProdFramework)
    ||  ::IsEqualIID(iid, IID_IDMUSProdFramework8)
	||  ::IsEqualIID(iid, IID_IUnknown) )
    {
        AddRef();
        *ppvObj = (IDMUSProdFramework8 *)this;
        return S_OK;
    }

    if( ::IsEqualIID(iid, IID_IDMUSProdFileRefChunk) )
    {
        AddRef();
        *ppvObj = (IDMUSProdFileRefChunk *)this;
        return S_OK;
    }

    if( ::IsEqualIID(iid, IID_IDMUSProdLoaderRefChunk) )
    {
        AddRef();
        *ppvObj = (IDMUSProdLoaderRefChunk *)this;
        return S_OK;
    }

    *ppvObj = NULL;
    return E_NOINTERFACE;
}


/////////////////////////////////////////////////////////////////////////////
// CFramework::AddRef

ULONG CFramework::AddRef()
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

    return ++m_dwRef;
}


/////////////////////////////////////////////////////////////////////////////
// CFramework::Release

ULONG CFramework::Release()
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


/*======================================================================================
METHOD:  IDMUSPRODFRAMEWORK::FINDCOMPONENT
========================================================================================
@method HRESULT | IDMUSProdFramework | FindComponent | Returns a pointer to the specified
		Component's <i IDMUSProdComponent> interface.
 
@comm
	The Framework calls CoCreateInstance to obtain an <i IDMUSProdComponent> interface for each
	of the Component CLSID's registered under:

	[HKEY_LOCAL_MACHINE\Software\Microsoft\DMUSProducer\Components].

	<om IDMUSProdFramework.FindComponent> locates the <i IDMUSProdComponent> whose CLSID matches
	<p rclsid> and ensures the Component's <om IDMUSProdComponent.Initialize> method has been
	called before returning its pointer in <p ppIComponent>.

	A Component that uses other DirectMusic Producer Components must call
	<om IDMUSProdFramework.FindComponent> to obtain a pointer to each of the required Component's
	<i IDMUSProdComponent> interface.

@rvalue S_OK | The specified Component's <i IDMUSProdComponent> was returned in <p ppIComponent>.
@rvalue E_POINTER | The address in <p ppIComponent> is not valid.  For example, it may be NULL.

@ex The following excerpt from an <om IDMUSProdComponent.Initialize> method obtains interface
	pointers to the Conductor Component and the Band Editor Component: |

	// Get IDMUSProdConductor interface pointers 
	if( FAILED ( pIFramework->FindComponent( CLSID_CConductor,  &pIComponent ) )
	||  FAILED ( pIComponent->QueryInterface( IID_IDMUSProdConductor, (void**)&m_pIConductor ) ) )
	{
		CleanUp();
		if( pIComponent )
		{
			pIComponent->Release();
		}
		::LoadString( theApp.m_hInstance, IDS_ERR_MISSING_CONDUCTOR, achErrMsg, MID_BUFFER );
		strErrMsg = achErrMsg;
		*pbstrErrMsg = strErrMsg.AllocSysString();
		return E_FAIL;
	}
	pIComponent->Release();

	// Get the IDMUSProdComponent interface for the Band Editor Component 
	if( FAILED ( pIFramework->FindComponent( CLSID_BandComponent,  &m_pIBandComponent ) ) )
	{
		CleanUp();
		::LoadString( theApp.m_hInstance, IDS_ERR_MISSING_BAND, achErrMsg, MID_BUFFER );
		strErrMsg = achErrMsg;
		*pbstrErrMsg = strErrMsg.AllocSysString();
		return E_FAIL;
	}
	
@xref <i IDMUSProdComponent>, <i IDMUSProdFramework>, <om IDMUSProdFramework.GetFirstComponent>, <om IDMUSProdFramework.GetNextComponent>
--------------------------------------------------------------------------------------*/

/////////////////////////////////////////////////////////////////////////////
// CFramework::FindComponent

HRESULT CFramework::FindComponent(
	REFCLSID rclsid,				// @parm [in] CLSID of the Component to be found.
	IDMUSProdComponent** ppIComponent	// @parm [out,retval] Address of a variable to receive the  
									//		requested <i IDMUSProdComponent> interface.  If an error
									//		occurs, the implementation sets <p ppIComponent>
									//		to NULL.  On success, the caller is responsible
									//		for calling <om IDMUSProdComponent.Release>.
)
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	if( ppIComponent == NULL )
	{
		return E_POINTER;
	}

    *ppIComponent = NULL;

    POSITION pos = m_lstComponents.GetHeadPosition();

    while( pos )
    {
        CJzComponent* pJzComponent = m_lstComponents.GetNext( pos );

		if( IsEqualCLSID( pJzComponent->m_clsidComponent, rclsid ) )
		{
			if( pJzComponent->m_fInitialized == FALSE )
			{
				BSTR bstrErrMsg = NULL;

				if( SUCCEEDED( pJzComponent->m_pIComponent->Initialize(this, &bstrErrMsg) ) )
				{
					pJzComponent->m_fInitialized = TRUE;
				}
				if( bstrErrMsg )
				{
					::SysFreeString( bstrErrMsg );
				}
			}
			if( pJzComponent->m_fInitialized )
			{
				pJzComponent->m_pIComponent->AddRef();
				*ppIComponent = pJzComponent->m_pIComponent;
			}
			break;
		}
    }

	if( *ppIComponent )
	{
		return S_OK;
	}

	return E_FAIL;
}


/*======================================================================================
METHOD:  IDMUSPRODFRAMEWORK::GETFIRSTCOMPONENT
========================================================================================
@method HRESULT | IDMUSProdFramework | GetFirstComponent | Returns an <i IDMUSProdComponent>
		interface pointer for the first <o Component> in the <o Framework>'s list of
		registered Components.

@comm
	<p ppIFirstComponent> is set to NULL if the list is empty.

@rvalue S_OK | The first Component was returned in <p ppIFirstComponent>. 
@rvalue E_POINTER | The address in <p ppIFirstComponent> is not valid.  For example, it
		may be NULL.
@rvalue E_FAIL | An error occurred, and the first Component could not be returned.

@ex The following example calls <om IDMUSProdComponent::Initialize> for all Components
		registered with the Framework: |

BOOL CFramework::InitComponents( void )
{
	HRESULT hr;
	IDMUSProdComponent* pIComponent;
	IDMUSProdComponent* pINextComponent;
	BOOL fSuccess = FALSE;

	BSTR bstrErrMsg = NULL;

    hr = GetFirstComponent( &pINextComponent );

	while( SUCCEEDED( hr )  &&  pINextComponent )
    {
		pIComponent = pINextComponent;

		hr = pIComponent->Initialize( (IDMUSProdFramework *)this, &bstrErrMsg );
		if( SUCCEEDED( hr ) )
		{
			fSuccess = TRUE;
		}
		else
		{
			...		// Display error message
			...		// Free resources
		}

	    hr = GetNextComponent( pIComponent, &pINextComponent );
		pIComponent->Release();
    }

	return fSuccess;
}

@xref <i IDMUSProdComponent>, <i IDMUSProdFramework>, <om IDMUSProdFramework.FindComponent>, <om IDMUSProdFramework.GetNextComponent>
--------------------------------------------------------------------------------------*/

/////////////////////////////////////////////////////////////////////////////
// CFramework::GetFirstComponent

HRESULT CFramework::GetFirstComponent(
	IDMUSProdComponent** ppIFirstComponent	// @parm [out,retval] Address of a variable to receive the requested
										//		<i IDMUSProdComponent> interface.  If an error occurs, the 
										//		implementation sets <p ppIFirstComponent> to NULL.  On success, the
										//		caller is responsible for calling
										//		<om IDMUSProdComponent.Release>.
)
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	if( ppIFirstComponent == NULL )
	{
		return E_POINTER;
	}
	
	*ppIFirstComponent = NULL;

	if( !m_lstComponents.IsEmpty() )
	{
		CJzComponent* pJzComponent = static_cast<CJzComponent*>( m_lstComponents.GetHead() );
		
		if( pJzComponent )
		{
			pJzComponent->m_pIComponent->AddRef();
			*ppIFirstComponent = pJzComponent->m_pIComponent;
		}
	}

	return S_OK;
}


/*======================================================================================
METHOD:  IDMUSPRODFRAMEWORK::GETNEXTCOMPONENT
========================================================================================
@method HRESULT | IDMUSProdFramework | GetNextComponent | Returns an <i IDMUSProdComponent>
		interface pointer for the next <o Component> in the <o Framework>'s list of
		registered Components.

@comm
	Returns the Component located after <p pIComponent> in the Framework's list of
	registered Components.

	<p ppINextComponent> is set to NULL when the end of the list has been reached.

@rvalue S_OK | The next Component was returned in <p ppINextComponent>. 
@rvalue E_POINTER | The address in <p ppINextComponent> is not valid.  For example, it
		may be NULL.
@rvalue E_INVALIDARG | <p pIComponent> is not valid.  For example, it may be NULL.
@rvalue E_FAIL | An error occurred, and the next Component could not be returned.

@ex The following example calls <om IDMUSProdComponent::Initialize> for all Components
		registered with the Framework: |

BOOL CFramework::InitComponents( void )
{
	HRESULT hr;
	IDMUSProdComponent* pIComponent;
	IDMUSProdComponent* pINextComponent;
	BOOL fSuccess = FALSE;

	BSTR bstrErrMsg = NULL;

    hr = GetFirstComponent( &pINextComponent );

	while( SUCCEEDED( hr )  &&  pINextComponent )
    {
		pIComponent = pINextComponent;

		hr = pIComponent->Initialize( (IDMUSProdFramework *)this, &bstrErrMsg );
		if( SUCCEEDED( hr ) )
		{
			fSuccess = TRUE;
		}
		else
		{
			...		// Display error message
			...		// Free resources
		}

	    hr = GetNextComponent( pIComponent, &pINextComponent );
		pIComponent->Release();
    }

	return fSuccess;
}

@xref <i IDMUSProdComponent>, <i IDMUSProdFramework>, <om IDMUSProdFramework.GetFirstComponent>, <om IDMUSProdFramework.FindComponent>
--------------------------------------------------------------------------------------*/

/////////////////////////////////////////////////////////////////////////////
// CFramework::GetNextComponent

HRESULT CFramework::GetNextComponent(
	IDMUSProdComponent* pIComponent,		// @parm [in] A pointer to the previous Component in
											//		the Framework's list of registered Components.
	IDMUSProdComponent** ppINextComponent	// @parm [out,retval] Address of a variable to receive the
											//		requested <i IDMUSProdComponent>
											//		interface.  If an error occurs, the implementation
											//		sets <p ppINextComponent> to NULL.  On success, the
											//		caller is responsible for calling
											//		<om IDMUSProdComponent.Release>.
)
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	if( ppINextComponent == NULL )
	{
		return E_POINTER;
	}
	
	*ppINextComponent = NULL;

	if( pIComponent == NULL )
	{
		return E_INVALIDARG;
	}

	CJzComponent* pJzComponent;

    POSITION pos = m_lstComponents.GetHeadPosition();

    while( pos )
    {
        pJzComponent = m_lstComponents.GetNext( pos );
		if( pJzComponent->m_pIComponent == pIComponent )
		{
			if( pos )
			{
				pJzComponent = m_lstComponents.GetNext( pos );

				pJzComponent->m_pIComponent->AddRef();
				*ppINextComponent = pJzComponent->m_pIComponent;
			}
			break;
		}
    }

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CFramework::AddComponentDocTemplates

BOOL CFramework::AddComponentDocTemplates( void )
{
	IDMUSProdDocType* pIDocType;
	HINSTANCE hInstance;
	UINT nResourceId;

	POSITION pos = m_lstDocTypes.GetHeadPosition();

    while( pos )
    {
        pIDocType = m_lstDocTypes.GetNext( pos );

		if( FAILED ( pIDocType->GetResourceId(&hInstance, &nResourceId) )
		||  !(theApp.CreateDocTemplate( pIDocType, hInstance, nResourceId )) )
		{
			return FALSE;
		}
    }

	return TRUE;
}


/*======================================================================================
METHOD:  IDMUSPRODFRAMEWORK::ADDDOCTYPE
========================================================================================
@method HRESULT | IDMUSProdFramework | AddDocType | Registers a <o DocType> object with the
	<o Framework>.
 
@comm
	A Component's <om IDMUSProdComponent.Initialize> method can call <om IDMUSProdFramework.AddDocType>
	to include additional document types in the Framework's File New, File Open and
	File Save dialogs.

	<om IDMUSProdFramework.AddDocType> must receive a successful return from
	<om IDMUSProdDocType.GetResourceId> in order to complete successfully.

@rvalue S_OK | <p pIDocType> was registered successfully. 
@rvalue E_INVALIDARG | <p pIDocType> is not valid.  For example, it may be NULL.
@rvalue E_FAIL | <p pIDocType> could not be registered. 

@ex The following excerpt from an <om IDMUSProdComponent.Initialize> method registers
	an additional document type with the Framework: |

	IDMUSProdDocType* pIDocType;
	IDMUSProdFramework* pIFramework;

	pIDocType = new CStyleDocType( this );
    if( pIDocType == NULL )
    {
		CleanUp();
		::LoadString( theApp.m_hInstance, IDS_ERR_MEMORY, achErrMsg, MID_BUFFER );
		strErrMsg = achErrMsg;
		*pbstrErrMsg = strErrMsg.AllocSysString();
        return E_OUTOFMEMORY;
    }

	pIDocType->AddRef();

	if( FAILED ( pIFramework->AddDocType(pIDocType) ) )
	{
		CleanUp();
		::LoadString( theApp.m_hInstance, IDS_ERR_ADD_DOCTYPE, achErrMsg, MID_BUFFER );
		strErrMsg = achErrMsg;
		*pbstrErrMsg = strErrMsg.AllocSysString();
		return E_FAIL;
	}
	
@xref <o DocType> Object, <i IDMUSProdDocType>, <i IDMUSProdFramework>, <om IDMUSProdFramework.FindDocTypeByNodeId>, <om IDMUSProdFramework.FindDocTypeByExtension>, <om IDMUSProdFramework.GetFirstDocType>, <om IDMUSProdFramework.GetNextDocType>
--------------------------------------------------------------------------------------*/

/////////////////////////////////////////////////////////////////////////////
// CFramework::AddDocType

HRESULT CFramework::AddDocType(
	IDMUSProdDocType* pIDocType		// @parm [in] Pointer to the <i IDMUSProdDocType> interface
								//		being registered.
)
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	CString strFilter;
	CString strListFilter;
	IDMUSProdDocType* pIListDocType;
	HINSTANCE hInstance;
	UINT nResourceId;
	POSITION posLast;
	TCHAR achFullText[MAX_BUFFER];

	if( pIDocType == NULL )
	{
		return E_INVALIDARG;
	}

	// Get the HINSTANCE and resource ID for the new DocType
	HRESULT hr = pIDocType->GetResourceId( &hInstance, &nResourceId );
	if( FAILED ( hr ) )
	{
		return hr;
	}

	// Get the filter string for the new DocType
	::LoadString( hInstance, nResourceId, achFullText, MAX_BUFFER );
	AfxExtractSubString( strFilter, achFullText, CDocTemplate::filterName, '\n' );

	// Place the new DocType in list - sort by filter string
	POSITION pos = m_lstDocTypes.GetHeadPosition();
    while( pos )
    {
		posLast = pos;
        pIListDocType = m_lstDocTypes.GetNext( pos );

		if( SUCCEEDED ( pIListDocType->GetResourceId( &hInstance, &nResourceId ) ) )
		{
			::LoadString( hInstance, nResourceId, achFullText, MAX_BUFFER );
			AfxExtractSubString( strListFilter, achFullText, CDocTemplate::filterName, '\n' );

			if( strFilter.CompareNoCase( strListFilter ) < 0 )
			{
				pIDocType->AddRef();
				m_lstDocTypes.InsertBefore( posLast, pIDocType );
				return S_OK;
			}
		}
    }

	pIDocType->AddRef();
	m_lstDocTypes.AddTail( pIDocType );
	return S_OK;
}


/*======================================================================================
METHOD:  IDMUSPRODFRAMEWORK::FINDDOCTYPEBYEXTENSION
========================================================================================
@method HRESULT | IDMUSProdFramework | FindDocTypeByExtension | Returns a pointer to the
		<i IDMUSProdDocType> interface associated with file extension <p bstrExt>. 

@comm
	A Component's <om IDMUSProdComponent.Initialize> method can call <om IDMUSProdFramework.AddDocType>
	to include additional document types in the Framework's File New, File Open and
	File Save dialogs.

	<om IDMUSProdFramework.FindDocTypeByExtension> enumerates the <i IDMUSProdDocType> interfaces
	registered through <om IDMUSProdFramework.AddDocType> and calls
	<om IDMUSProdDocType.DoesExtensionMatch> to find the <i IDMUSProdDocType> interface whose
	file extension matches <p bstrExt>.

@rvalue S_OK | The specified file extension's <i IDMUSProdDocType> was returned in <p ppIDocType>.
@rvalue E_POINTER | The address in <p ppIDocType> is not valid.  For example, it may be NULL.
@rvalue E_FAIL | A matching <i IDMUSProdDocType> interface was not found.

@ex The following example searches for the <i IDMUSProdDocType> interface that manages
	Jazz Style files (*.sty): |

	IDMUSProdFramework* pIFramework;
	IDMUSProdDocType* pIDocType;
	CString strExt;
	BSTR bstrExt;

	strExt = ".sty";
	bstrExt = strExt.AllocSysString();
	if( SUCCEEDED ( pIFramework->FindDocTypeByExtension( bstrExt, &pIDocType ) ) )
	{
		pIDocType->Release();
	}
	
@xref <i IDMUSProdDocType>, <om IDMUSProdDocType.DoesExtensionMatch><i IDMUSProdFramework>, <om IDMUSProdFramework.AddDocType>, <om IDMUSProdFramework.FindDocTypeByNodeId>, <om IDMUSProdFramework.GetFirstDocType>, <om IDMUSProdFramework.GetNextDocType>
--------------------------------------------------------------------------------------*/

/////////////////////////////////////////////////////////////////////////////
// CFramework::FindDocTypeByExtension

HRESULT CFramework::FindDocTypeByExtension(
	BSTR bstrExt,					// @parm [in] File extension.  This method frees
									//		<p bstrExt> with SysFreeString when it is no
									//		longer needed.
	IDMUSProdDocType** ppIDocType	// 	@parm [out,retval] Address of a variable to receive the requested  
									//		<i IDMUSProdDocType> interface.  If an error occurs,
									//		the implementation sets <p ppIDocType> to NULL.
									//		On success, the caller is responsible for calling
									//		<om IDMUSProdDocType.Release>.
)
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	HRESULT hr;
	IDMUSProdDocType* pIDocType;
	IDMUSProdDocType* pINextDocType;
	BSTR bstrTheExt;

	CString strExt = bstrExt;
	::SysFreeString( bstrExt );

	if( ppIDocType == NULL )
	{
		return E_POINTER;
	}

	*ppIDocType = NULL;

    hr = GetFirstDocType( &pINextDocType );

	while( SUCCEEDED( hr )  &&  pINextDocType )
    {
		pIDocType = pINextDocType;

		bstrTheExt = strExt.AllocSysString();
		if( pIDocType->DoesExtensionMatch( bstrTheExt ) == S_OK )
		{
			*ppIDocType = pIDocType;
			break;
		}

	    hr = GetNextDocType( pIDocType, &pINextDocType );
		pIDocType->Release();
	}

	if( *ppIDocType )
	{
		return S_OK;
	}
	
	return E_FAIL;
}


/*======================================================================================
METHOD:  IDMUSPRODFRAMEWORK::FINDDOCTYPEBYNODEID
========================================================================================
@method HRESULT | IDMUSProdFramework | FindDocTypeByNodeId | Returns a pointer to the
		<i IDMUSProdDocType> interface associated with node ID <p rguid>. 

@comm
	A GUID identifies a type of Node.  For example, all Style Nodes have the same
	GUID, or node ID.  

	A Component's <om IDMUSProdComponent.Initialize> method can call <om IDMUSProdFramework.AddDocType>
	to include additional document types in the Framework's File New, File Open and
	File Save dialogs.

	<om IDMUSProdFramework.FindDocTypeByNodeId> enumerates the <i IDMUSProdDocType> interfaces
	registered through <om IDMUSProdFramework.AddDocType> and calls
	<om IDMUSProdDocType.DoesIdMatch> to find the <i IDMUSProdDocType> interface associated with
	the node ID specified in <p rguid>.

@rvalue S_OK | The specified id's <i IDMUSProdDocType> was returned in <p ppIDocType>.
@rvalue E_POINTER | The address in <p ppIDocType> is not valid.  For example, it may be NULL.
@rvalue E_FAIL | A matching <i IDMUSProdDocType> interface was not found.

@ex See <om IDMUSProdDocType.DoesIdMatch> for sample code. |
	
@xref <i IDMUSProdDocType>, <om IDMUSProdDocType.DoesIdMatch>, <i IDMUSProdFramework>, <om IDMUSProdFramework.AddDocType>, <om IDMUSProdFramework.FindDocTypeByExtension>, <om IDMUSProdFramework.GetFirstDocType>, <om IDMUSProdFramework.GetNextDocType>
--------------------------------------------------------------------------------------*/

/////////////////////////////////////////////////////////////////////////////
// CFramework::FindDocTypeByNodeId

HRESULT CFramework::FindDocTypeByNodeId(
	REFGUID rguid,					// @parm [in] Node ID.
	IDMUSProdDocType** ppIDocType	// @parm [out,retval] Address of a variable to receive the requested 
									//		<i IDMUSProdDocType> interface.  If an error occurs, the implementation
									//		sets <p ppIDocType> to NULL.  On success, the caller
									//		is responsible for calling <om IDMUSProdDocType.Release>.
)
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	HRESULT hr;
	IDMUSProdDocType* pIDocType;
	IDMUSProdDocType* pINextDocType;

	if( ppIDocType == NULL )
	{
		return E_POINTER;
	}

	*ppIDocType = NULL;

    hr = GetFirstDocType( &pINextDocType );

	while( SUCCEEDED( hr )  &&  pINextDocType )
    {
		pIDocType = pINextDocType;

		if( pIDocType->DoesIdMatch( rguid ) == S_OK )
		{
			*ppIDocType = pIDocType;
			break;
		}

	    hr = GetNextDocType( pIDocType, &pINextDocType );
		pIDocType->Release();
	}

	if( *ppIDocType )
	{
		return S_OK;
	}
	
	return E_FAIL;
}


/*======================================================================================
METHOD:  IDMUSPRODFRAMEWORK::GETFIRSTDOCTYPE
========================================================================================
@method HRESULT | IDMUSProdFramework | GetFirstDocType | Returns an <i IDMUSProdDocType>
		interface pointer for the first <o DocType> in the <o Framework>'s list of
		registered DocTypes.

@comm
	<p ppIFirstDocType> is set to NULL if the list is empty.

@rvalue S_OK | The first DocType was returned in <p ppIFirstDocType>. 
@rvalue E_POINTER | The address in <p ppIFirstDocType> is not valid.  For example, it may
		be NULL.
@rvalue E_FAIL | An error occurred, and the first DocType could not be returned.

@ex The following example returns a pointer to the <i IDMUSProdDocType> interface associated
	with file extension <p bstrExt>: |

HRESULT CFramework::FindDocTypeByExtension( BSTR bstrExt, IDMUSProdDocType** ppIDocType	)
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	HRESULT hr;
	IDMUSProdDocType* pIDocType;
	IDMUSProdDocType* pINextDocType;

	if( ppIDocType == NULL )
	{
		return E_POINTER;
	}

	*ppIDocType = NULL;

    hr = GetFirstDocType( &pINextDocType );

	while( SUCCEEDED( hr )  &&  pINextDocType )
    {
		pIDocType = pINextDocType;

		if( pIDocType->DoesExtensionMatch( bstrExt ) == S_OK )
		{
			*ppIDocType = pIDocType;
			break;
		}

	    hr = GetNextDocType( pIDocType, &pINextDocType );
		pIDocType->Release();
	}

	if( *ppIDocType )
	{
		return S_OK;
	}
	
	return E_FAIL;
}

@xref <i IDMUSProdDocType>, <om IDMUSProdDocType.DoesIdMatch>, <i IDMUSProdFramework>, <om IDMUSProdFramework.AddDocType>, <om IDMUSProdFramework.FindDocTypeByExtension>, <om IDMUSProdFramework.GetNextDocType>
--------------------------------------------------------------------------------------*/

/////////////////////////////////////////////////////////////////////////////
// CFramework::GetFirstDocType

HRESULT CFramework::GetFirstDocType(
	IDMUSProdDocType** ppIFirstDocType	// @parm [out,retval] Address of a variable to receive the requested 
									//		<i IDMUSProdDocType> interface.  If an error occurs, the implementation
									//		sets <p ppIFirstDocType> to NULL.  On success, the
									//		caller is responsible for calling
									//		<om IDMUSProdDocType.Release>.
)
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	if( ppIFirstDocType == NULL )
	{
		return E_POINTER;
	}
	
	*ppIFirstDocType = NULL;

	if( !m_lstDocTypes.IsEmpty() )
	{
		IDMUSProdDocType* pIDocType = static_cast<IDMUSProdDocType*>( m_lstDocTypes.GetHead() );
		
		if( pIDocType )
		{
			pIDocType->AddRef();
			*ppIFirstDocType = pIDocType;
		}
	}

	return S_OK;
}


/*======================================================================================
METHOD:  IDMUSPRODFRAMEWORK::GETNEXTDOCTYPE
========================================================================================
@method HRESULT | IDMUSProdFramework | GetNextDocType | Returns an <i IDMUSProdDocType>
		interface pointer for the next <o DocType> in the <o Framework>'s list of
		registered DocTypes.

@comm
	Returns the DocType located after <p pIDocType> in the Framework's list of
	registered DocTypes.

	<p ppINextDocType> is set to NULL when the end of the list has been reached.

@rvalue S_OK | The next DocType was returned in <p ppINextDocType>. 
@rvalue E_POINTER | The address in <p ppINextDocType> is not valid.  For example, it may
		be NULL.
@rvalue E_INVALIDARG | <p pIDocType> is not valid.  For example, it may be NULL.
@rvalue E_FAIL | An error occurred, and the next DocType could not be returned.

@ex The following example returns a pointer to the <i IDMUSProdDocType> interface associated
	with file extension <p bstrExt>: |

HRESULT CFramework::FindDocTypeByExtension( BSTR bstrExt, IDMUSProdDocType** ppIDocType	)
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	HRESULT hr;
	IDMUSProdDocType* pIDocType;
	IDMUSProdDocType* pINextDocType;

	if( ppIDocType == NULL )
	{
		return E_POINTER;
	}

	*ppIDocType = NULL;

    hr = GetFirstDocType( &pINextDocType );

	while( SUCCEEDED( hr )  &&  pINextDocType )
    {
		pIDocType = pINextDocType;

		if( pIDocType->DoesExtensionMatch( bstrExt ) == S_OK )
		{
			*ppIDocType = pIDocType;
			break;
		}

	    hr = GetNextDocType( pIDocType, &pINextDocType );
		pIDocType->Release();
	}

	if( *ppIDocType )
	{
		return S_OK;
	}
	
	return E_FAIL;
}

@xref <i IDMUSProdDocType>, <om IDMUSProdDocType.DoesIdMatch>, <i IDMUSProdFramework>, <om IDMUSProdFramework.AddDocType>, <om IDMUSProdFramework.FindDocTypeByExtension>, <om IDMUSProdFramework.GetFirstDocType>
--------------------------------------------------------------------------------------*/

/////////////////////////////////////////////////////////////////////////////
// CFramework::GetNextDocType

HRESULT CFramework::GetNextDocType(
	IDMUSProdDocType* pIDocType,		// @parm [in] A pointer to the previous DocType in
										//		the Framework's list of registered DocTypes.
	IDMUSProdDocType** ppINextDocType	// @parm [out,retval] Address of a variable to receive the requested  
										//		<i IDMUSProdDocType> interface.  If an error occurs, the implementation
										//		sets <p ppINextDocType> to NULL.  On success, the
										//		caller is responsible for calling
										//		<om IDMUSProdDocType.Release>.
)
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	if( ppINextDocType == NULL )
	{
		return E_POINTER;
	}
	
	*ppINextDocType = NULL;

	if( pIDocType == NULL )
	{
		return E_INVALIDARG;
	}

	IDMUSProdDocType* pIListDocType;

    POSITION pos = m_lstDocTypes.GetHeadPosition();

    while( pos )
    {
        pIListDocType = m_lstDocTypes.GetNext( pos );
		if( pIListDocType == pIDocType )
		{
			if( pos )
			{
				pIListDocType = m_lstDocTypes.GetNext( pos );

				pIListDocType->AddRef();
				*ppINextDocType = pIListDocType;
			}
			break;
		}
    }

	return S_OK;
}


/*======================================================================================
METHOD:  IDMUSPRODFRAMEWORK::CREATENEWFILE
========================================================================================
@method HRESULT | IDMUSProdFramework | CreateNewFile | Invokes the <o Framework>'s File New
	command and returns a new file of the type described through <p pIDocType>.

@comm
	This method provides the means for a Component to initiate a File New command.  

	Part of the Framework's File New processing uses <p pIDocType> to call
	<om IDMUSProdDocType.OnFileNew>.

@rvalue S_OK | The newly created file's top <i IDMUSProdNode> was returned in <p ppIDocRootNode>. 
@rvalue E_POINTER | The address in <p ppIDocRootNode> is not valid.  For example, it may
		be NULL.
@rvalue E_INVALIDARG | Either <p pIDocType> or <p pITreePositionNode> is not valid.
		For example, they may be NULL.
@rvalue E_FAIL | A new file could not be created. 

@ex The following example creates a new Style file and places it in the Project Tree in the
	same directory as <p pISegmentDocRootNode>: |

	IDMUSProdDocType* pIDocType;
	IDMUSProdNode* pIDocRootNode;

	if( SUCCEEDED ( m_pIFramework->FindDocTypeByNodeId( GUID_StyleNode, &pIDocType ) ) )
	{
		if( SUCCEEDED ( m_pIFramework->CreateNewFile( pIDocType, pISegmentDocRootNode, &pIDocRootNode ) ) )
		{
			pIDocRootNode->Release();
		}

		pIDocType->Release();
	}
	
@xref <i IDMUSProdDocType>, <i IDMUSProdFramework>, <om IDMUSProdFramework.OpenFile>
--------------------------------------------------------------------------------------*/

/////////////////////////////////////////////////////////////////////////////
// CFramework::CreateNewFile

HRESULT CFramework::CreateNewFile(
	IDMUSProdDocType* pIDocType,		// @parm [in] Pointer to the <i IDMUSProdDocType> interface which
										//		is used to determine the kind of file to create.
	IDMUSProdNode* pITreePositionNode,	// @parm [in] Pointer to the <o Node> used to determine where
										//		the newly created file should be placed in the Project
										//		Tree.
	IDMUSProdNode** ppIDocRootNode		// @parm [out,retval] Address of a variable to receive the requested  
										//		<i IDMUSProdNode> interface.  If an error occurs, the implementation
										//		sets <p ppIDocRootNode> to NULL.  On success, the caller
										//		is responsible for calling <om IDMUSProdNode.Release> when
										//		this pointer is no longer needed.
)
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	if( ppIDocRootNode == NULL )
	{
		return E_POINTER;
	}

	*ppIDocRootNode = NULL;

	if( pIDocType == NULL
	||  pITreePositionNode == NULL )
	{
		return E_INVALIDARG;
	}

	// Find the DocType's CJazzDocTemplate*
	CJazzDocTemplate* pDocTemplate = theApp.FindDocTemplate( pIDocType );
	if( pDocTemplate )
	{
		// Set Project Tree position
		theApp.SetProjectTreePosition( pITreePositionNode );

		// Create the file
		theApp.m_nFileTarget = TGT_SELECTEDNODE;
		CComponentDoc* pComponentDoc = (CComponentDoc *)pDocTemplate->OpenDocumentFile( NULL );
		theApp.m_nFileTarget = TGT_PROJECT;

		if( pComponentDoc )
		{
			if( pComponentDoc->m_pIDocRootNode )
			{
				pComponentDoc->m_pIDocRootNode->AddRef();
				*ppIDocRootNode = pComponentDoc->m_pIDocRootNode;
				return S_OK;
			}
		}
	}

	return E_FAIL;
}


/*======================================================================================
METHOD:  IDMUSPRODFRAMEWORK::OPENFILE
========================================================================================
@method HRESULT | IDMUSProdFramework | OpenFile | Invokes the <o Framework>'s File Open
	command and returns a file of the type described through <p pIDocType>.

@comm
	This method provides the means for a Component to initiate a File Open command.  

	The title of the File Open dialog is set to 'Open' when <p bstrTitle> is NULL.

	Part of the Framework's File Open processing uses <p pIDocType> to call
	<om IDMUSProdDocType.OnFileOpen>.

@rvalue S_OK | The file's top <i IDMUSProdNode> was returned in <p ppIDocRootNode>. 
@rvalue E_POINTER | The address in <p ppIDocRootNode> is not valid.  For example, it may
		be NULL.
@rvalue E_INVALIDARG | Either <p pIDocType> or <p pITreePositionNode> is not valid.
		For example, they may be NULL.
@rvalue E_FAIL | The file could not be opened. 

@ex The following example loads a Style file and places it in the Project Tree in the
	same directory as <p pISegmentDocRootNode>: |

	IDMUSProdDocType* pIDocType;
	IDMUSProdNode* pIDocRootNode;

	if( SUCCEEDED ( m_pIFramework->FindDocTypeByNodeId( GUID_StyleNode, &pIDocType ) ) )
	{
		if( SUCCEEDED ( m_pIFramework->OpenFile( pIDocType, NULL, pISegmentDocRootNode, &pIDocRootNode ) ) )
		{
			pIDocRootNode->Release();
		}

		pIDocType->Release();
	}
	
@xref <i IDMUSProdDocType>, <i IDMUSProdFramework>, <om IDMUSProdFramework.CreateNewFile>
--------------------------------------------------------------------------------------*/

/////////////////////////////////////////////////////////////////////////////
// CFramework::OpenFile

HRESULT CFramework::OpenFile(
	IDMUSProdDocType* pIDocType,	// @parm [in] Pointer to the <i IDMUSProdDocType> interface which
									//		is used to determine the kind of file to open.
	BSTR bstrTitle,					// @parm [in] Title used for the File Open dialog.  This method
									//		frees <p bstrTitle> with SysFreeString when it is no
									//		longer needed.
	IDMUSProdNode* pITreePositionNode,	// @parm [in] Pointer to the <o Node> used to determine where
									//		the newly created file should be placed in the Project
									//		Tree.
	IDMUSProdNode** ppIDocRootNode	// @parm [out,retval] Address of a variable to receive the requested  
									//		<i IDMUSProdNode> interface.  If an error occurs, the implementation
									//		sets <p ppIDocRootNode> to NULL.  On success, the caller
									//		is responsible for calling <om IDMUSProdNode.Release> when
									//		this pointer is no longer needed.
)
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	if( ppIDocRootNode == NULL )
	{
		return E_POINTER;
	}

	*ppIDocRootNode = NULL;

	if( pIDocType == NULL
	||  pITreePositionNode == NULL )
	{
		return E_INVALIDARG;
	}

	// Find the DocType's CJazzDocTemplate*
	CJazzDocTemplate* pTemplate = theApp.FindDocTemplate( pIDocType );
	if( pTemplate )
	{
		CString strFileName;

		// Prompt user for filename
		BOOL fHaveFileName = FALSE;
		while( fHaveFileName == FALSE )
		{
			CString strTitle;
			LPCTSTR szTitle = NULL;

			if( bstrTitle )
			{
				strTitle = bstrTitle;
				::SysFreeString( bstrTitle );
				szTitle = strTitle;
			}

			strFileName.Empty();
			if( theApp.DoPromptFileOpenSingle(strFileName, pTemplate, szTitle) )
			{
				// Make sure the user is not trying to open a Project file
				if( theApp.IsValidProjectFile( strFileName, FALSE ) == TRUE )
				{
					CString strMsg;

					HINSTANCE hInstance = AfxGetResourceHandle();
					AfxSetResourceHandle( theApp.m_hInstance );

					AfxFormatString1( strMsg, IDS_ERR_IS_PROJECT_FILE, strFileName );
					AfxMessageBox( strMsg );

					AfxSetResourceHandle( hInstance );
				}
				else
				{
					fHaveFileName = TRUE;
				}
			}
			else
			{
				return S_FALSE;
			}
		}

		// Set Project Tree position
		theApp.SetProjectTreePosition( pITreePositionNode );

		// Open the file
		CComponentDoc* pComponentDoc = (CComponentDoc *)theApp.OpenTheFile( strFileName, TGT_SELECTEDNODE );
		if( pComponentDoc )
		{
			if( pComponentDoc->m_pIDocRootNode )
			{
				// Make sure user opened the type of file that was expected
				if( pComponentDoc->m_pIDocType == pIDocType )
				{
					pComponentDoc->m_pIDocRootNode->AddRef();
					*ppIDocRootNode = pComponentDoc->m_pIDocRootNode;
					return S_OK;
				}
				else
				{
					CString strMsg;
					CString strFilterName;

					HINSTANCE hInstance = AfxGetResourceHandle();
					AfxSetResourceHandle( theApp.m_hInstance );

					pTemplate->GetDocString( strFilterName, CDocTemplate::docName );
					AfxFormatString2( strMsg, IDS_ERR_WRONG_FILE, strFileName, strFilterName );
					AfxMessageBox( strMsg );
	
					AfxSetResourceHandle( hInstance );
					return S_FALSE;
				}
			}
		}
	}

	return E_FAIL;
}


/*======================================================================================
METHOD:  IDMUSPRODFRAMEWORK::COPYFILE
========================================================================================
@method HRESULT | IDMUSProdFramework | CopyFile | Returns a copy of the file specified in
	<p pIDocRootNode>. 

@comm
	<p pIDocRootNode> must point to a DocRoot <o Node>.  A DocRoot node is the top node of
	a document, or file.  For example, the Style node of a Style file would be considered
	the DocRoot node of the Style.  The Band node of a Band file would be considered the
	DocRoot node of the Band file.

	This method saves the file specified in <p pIDocRootNode> using a name built from
	appending the filename of <p pIDocRootNode> to the path of <p pITreePositionNode>.  
	After the file has been successfully saved, <om IDMUSProdDocType.OnFileOpen> loads the
	new file and places its nodes into the Project Tree.  The DocRoot node of the newly
	created file is returned in <p ppINewDocRootNode>.

	if <p pIDocRootNode> does not already exist in the Project Tree, the filename of
	<p ppINewDocRootNode> will be derived by appending the the node name of <p pIDocRootNode>
	to the path of <p pITreePositionNode>.

@rvalue S_OK | A copy of the file specified in <p pIDocRootNode> was returned in
		<p ppINewDocRootNode>.
@rvalue E_POINTER | The address in <p ppINewDocRootNode> is not valid.  For example, it
		may be NULL.
@rvalue E_INVALIDARG | The value of either <p pIDocRootNode> or <p pITreePositionNode> is
		not valid.  For example, it may be NULL.
@rvalue E_FAIL | An error occurred, and the file could not be copied.

@ex The following example makes sure that a referenced file exists in the same Project as the
	file it references.  m_pIStyleRefNode contains an IDMUSProdNode interface pointer to the node
	used to display a referenced Style file:|

	IDMUSProdReferenceNode* pIReferenceNode;

	if( SUCCEEDED ( m_pIStyleRefNode->QueryInterface( IID_IDMUSProdReferenceNode, (void **)&pIReferenceNode ) ) )
	{
		IDMUSProdNode* pIStyleDocRootNode;

		if( SUCCEEDED ( pIReferenceNode->GetReferencedFile( &pIStyleDocRootNode ) ) )
		{
			if( pIFramework->IsProjectEqual( this, pIStyleDocRootNode ) == S_FALSE )
			{
				IDMUSProdNode* pINewDocRootNode;

				if( SUCCEEDED ( pIFramework->CopyFile( pIStyleDocRootNode, this, &pINewDocRootNode ) ) )
				{
					pIReferenceNode->SetReferencedFile( pINewDocRootNode );
					pINewDocRootNode->Release();
				}
			}

			pIStyleDocRootNode->Release();
		}

		pIReferenceNode->Release();
	}

@xref <i IDMUSProdFramework>, <om IDMUSProdFramework.IsProjectEqual>
--------------------------------------------------------------------------------------*/

/////////////////////////////////////////////////////////////////////////////
// CFramework::CopyFile

HRESULT CFramework::CopyFile(
	IDMUSProdNode* pIDocRootNode,	// @parm [in] Pointer to an <i IDMUSProdNode> interface which
									//		specifies the file to be copied.
	IDMUSProdNode* pITreePositionNode,	// @parm [in] Pointer to the Node used to determine where
									//		the newly created file should be placed in the Project
									//		Tree.
	IDMUSProdNode** ppINewDocRootNode	// @parm [out,retval] Address of a variable to receive the requested  
									//		IDMUSProdNode interface.  If an error occurs, the implementation
									//		sets <p ppINewDocRootNode> to NULL.  On success, the
									//		caller is responsible for calling <om IDMUSProdNode.Release>
									//		when this pointer is no longer needed.
)
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	if( ppINewDocRootNode == NULL )
	{
		return E_POINTER;
	}

	*ppINewDocRootNode = NULL;

	if( pITreePositionNode == NULL
	||  pIDocRootNode == NULL )
	{
		return E_INVALIDARG;
	}

	// Make sure pIDocRootNode really is a DocRootNode
	IDMUSProdNode* pINode;

	if( FAILED ( pIDocRootNode->GetDocRootNode( &pINode ) ) )
	{
		return E_FAIL;
	}
	ASSERT( pIDocRootNode == pINode );
	if( pINode == NULL )
	{
		return E_FAIL;
	}
	if( pIDocRootNode != pINode )
	{
		pINode->Release();
		return E_INVALIDARG;
	}
	pINode->Release();

	CString strNewFileName;

	// Get the pComponentDoc associated with pIDocRootNode
	CComponentDoc* pComponentDoc = theApp.FindComponentDocument( pIDocRootNode );
	if( pComponentDoc )
	{
		// Get the filename of pComponentDoc
		CString strFileName = pComponentDoc->GetPathName();

		// Determine the new filename
		BSTR bstrNewFileName;
		int nFindPos;

		if( FAILED ( GetNodeFileName( pITreePositionNode, &bstrNewFileName ) ) )
		{
			return E_FAIL;
		}
		strNewFileName = bstrNewFileName;
		::SysFreeString( bstrNewFileName );
		nFindPos = strNewFileName.ReverseFind( (TCHAR)'\\' );
		if( nFindPos == -1 )
		{
			return E_FAIL;
		}
		strNewFileName = strNewFileName.Left( nFindPos + 1 );
		nFindPos = strFileName.ReverseFind( (TCHAR)'\\' );
		if( nFindPos == -1 )
		{
			return E_FAIL;
		}
		strNewFileName += strFileName.Right( strFileName.GetLength() - nFindPos - 1 );

		// Save the new file
		if( pComponentDoc->DoSave( strNewFileName, FALSE ) == FALSE )
		{
			return E_FAIL;
		}
	}
	else
	{
		// Get path associated with pITreePositionNode
		GUID guidNodeId;
		CString strTargetPath;

		if( SUCCEEDED ( pITreePositionNode->GetNodeId( &guidNodeId ) ) )
		{
			if( ::IsEqualGUID(guidNodeId, GUID_ProjectFolderNode) == FALSE
			&&  ::IsEqualGUID(guidNodeId, GUID_DirectoryNode) == FALSE )
			{
				return E_FAIL;
			}

			CDirectoryNode* pDirNode = (CDirectoryNode *)pITreePositionNode;		
			pDirNode->ConstructPath( strTargetPath );
		}

		// Get guidNodeId of pIDocRootNode
		if( FAILED ( pIDocRootNode->GetNodeId( &guidNodeId ) ) )
		{
			return E_FAIL;
		}

		// Determine the new filename
		IDMUSProdDocType* pIDocType;
		if( SUCCEEDED ( FindDocTypeByNodeId( guidNodeId, &pIDocType ) ) )
		{
			IDMUSProdDocType8* pIDocType8;
			if( SUCCEEDED ( pIDocType->QueryInterface( IID_IDMUSProdDocType8, (void**)&pIDocType8 ) ) )
			{
				BSTR bstrExt;
				if( SUCCEEDED ( pIDocType8->GetObjectExt( guidNodeId, FT_DESIGN, &bstrExt ) ) )
				{
					CString strExt = bstrExt;
					::SysFreeString( bstrExt );

					BSTR bstrName;
					if( SUCCEEDED ( pIDocRootNode->GetNodeName( &bstrName ) ) )
					{
						CString strName = bstrName;
						::SysFreeString( bstrName );

						// Determine proposed filename
						CString strProposedFileName = strTargetPath + strName + strExt;
						
						// Make sure we have a unique filename
						CString strFName;

						theApp.GetUniqueFileName( strProposedFileName, strFName );
						strNewFileName = strTargetPath + strFName;
					}
				}

				pIDocType8->Release();
			}

			pIDocType->Release();
		}

		HRESULT hr = E_FAIL;

		// Save the file
		IStream* pIStream;
	    if( SUCCEEDED ( ::AllocFileStream(strNewFileName, GENERIC_WRITE, FT_DESIGN, GUID_CurrentVersion, pITreePositionNode, &pIStream) ) )
		{
			IPersistStream* pIPersistStream;

			if( SUCCEEDED ( pIDocRootNode->QueryInterface( IID_IPersistStream, (void**)&pIPersistStream ) ) )
			{
				if( SUCCEEDED ( pIPersistStream->Save( pIStream, FALSE ) ) )
				{
					hr = S_OK;
				}

				pIPersistStream->Release();
			}

			pIStream->Release();
		}

		if( FAILED ( hr ) )
		{
			return hr;
		}
	}

	// Set Project Tree position
	theApp.SetProjectTreePosition( pITreePositionNode );

	// Open the newly created file
	CComponentDoc* pNewComponentDoc = (CComponentDoc *)theApp.OpenTheFile( strNewFileName, TGT_SELECTEDNODE );
	if( pNewComponentDoc )
	{
		// Ensure the file node has a runtime file name
		if( pNewComponentDoc->m_pFileNode )
		{
			// Force SetNodeName to handle setting of FileNode node name
			BSTR bstrName = pNewComponentDoc->m_pFileNode->m_strName.AllocSysString();
			pNewComponentDoc->m_pFileNode->m_strName.Empty();
			pNewComponentDoc->m_pFileNode->SetNodeName( bstrName );
		}

		if( pNewComponentDoc->m_pIDocRootNode )
		{
			pNewComponentDoc->m_pIDocRootNode->AddRef();
			*ppINewDocRootNode = pNewComponentDoc->m_pIDocRootNode;
			return S_OK;
		}
	}

	return E_FAIL;
}


/*======================================================================================
METHOD:  IDMUSPRODFRAMEWORK::SHOWFILE
========================================================================================
@method HRESULT | IDMUSProdFramework | ShowFile | Displays a newly created file in the
	Project Tree.

@comm
	There may be times when a <o Component> creates a new file.  For example, a MIDI export
	feature would generate a new MIDI file.  <om IDMUSProdFramework::ShowFile> provides the
	means for a Component to display a newly created file in an open <o Project> in the
	Project Tree.

	<p bstrFileName> must point to the complete path/filename of an existing file.

	This method neither creates nor opens Projects.  It returns E_FAIL if the Project Tree
	does not already contain a Project folder in which to place <p bstrFileName>.  Sub-folders,
	however, will be created when necessary.

@rvalue S_OK | The file specified in <p bstrFileName> was added to the Project Tree.
@rvalue E_INVALIDARG | The value of <p bstrFileName> is not valid.  For example, it may be NULL.
@rvalue E_FAIL | An error occurred, and the file could not be displayed in the Project Tree.

@xref <i IDMUSProdFramework>
--------------------------------------------------------------------------------------*/

/////////////////////////////////////////////////////////////////////////////
// CFramework::ShowFile

HRESULT CFramework::ShowFile(
	BSTR bstrFileName				// @parm [in] Complete path and filename of the file to 
									//		be added to the Project Tree.  This method frees
									//		<p bstrFileName> with SysFreeString when it is
									//		no longer needed.
)
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	if( bstrFileName == NULL )
	{
		return E_INVALIDARG;
	}

	CString strFileName = bstrFileName;
	::SysFreeString( bstrFileName );

	// See if file already exists in Project tree
	CFileNode* pFileNode = theApp.FindFileNode( strFileName );
	if( pFileNode )
	{
		// Already exists in Project Tree, nothing to do so just exit
		return S_OK;
	}

	// Make sure file exists on hard drive
	DWORD dwAttributes = ::GetFileAttributes( strFileName );
	if( dwAttributes == 0xFFFFFFFF )
	{
		// File does not exist
		return E_FAIL;
	}

	// Make sure strFileName is not a directory
	if( dwAttributes & FILE_ATTRIBUTE_DIRECTORY )
	{
		// strFileName is a directory
		return E_FAIL;
	}

	HRESULT hr = E_FAIL;

	// See if the file resides in any of the open Projects
	CProject* pProject = theApp.GetProjectByFileName( strFileName );
	if( pProject )
	{
		// Find the parent node in the Project Tree
		theApp.m_nFileTarget = TGT_FILENODE_SAVEAS;
		IDMUSProdNode* pIParentNode = theApp.m_pFramework->DetermineParentNode( strFileName );
		theApp.m_nFileTarget = TGT_PROJECT;
		
		if( pIParentNode )
		{
			CDirectoryNode* pParentDirNode = (CDirectoryNode *)pIParentNode;

			// CFileNode should have been created in theApp.m_pFramework->DetermineParentNode()
			pFileNode = theApp.FindFileNode( strFileName );
			if( pFileNode )
			{
				CTreeCtrl* pTreeCtrl = theApp.GetProjectTreeCtrl();

				if( pParentDirNode->m_hItem )
				{
					CMainFrame* pMainFrame = (CMainFrame *)theApp.m_pMainWnd;
					ASSERT( pMainFrame != NULL );
					pMainFrame->m_wndTreeBar.SortChildren( pParentDirNode->m_hItem );
				}

				theApp.m_pFramework->ShowTreeNode( pFileNode );
				hr = S_OK;
			}
		}

		pProject->Release();
	}

	return hr;
}


/*======================================================================================
METHOD:  IDMUSPRODFRAMEWORK::REGISTERCLIPFORMATFORFILE
========================================================================================
@method HRESULT | IDMUSProdFramework | RegisterClipFormatForFile | Registers a file's clipboard
	format with the <o Framework>.
 
@comm
	A <o Component>'s <om IDMUSProdComponent.Initialize> method can call
	<om IDMUSProdFramework.RegisterClipFormatForFile> to make the Framework aware of clipboard
	formats that can be used to create files in the Project Tree.  

	Data objects which have the potential of being pasted as DirectMusic files must contain at
	least two clipboard formats:  CF_DMUSPROD_FILE, and their custom format for file content.  The
	custom format must be registered with the Framework via a call to
	<om IDMUSProdFramework.RegisterClipFormatForFile>.  CF_DMUSPROD_FILE clipboard formats are created
	by calling <om IDMUSProdFramework.SaveClipFormat>.
	
@rvalue S_OK | <p uClipFormat> was registered successfully. 
@rvalue E_INVALIDARG | Either <p uClipFormat> or <p bstrExt> is not valid.  For example,
	<p uClipFormat> may be zero.
@rvalue E_OUTOFMEMORY | Out of memory. 
@rvalue E_FAIL | <p uClipFormat> could not be registered. 

@ex The following excerpt from an <om IDMUSProdComponent.Initialize> method registers
	a Band file's clipboard format with the Framework: |

	#define CF_BAND "DMUSProd v.1 Band"
	#define CF_BANDLIST "DMUSProd v.1 Band List"
  
	// Register clipboard formats
	m_cfBand = ::RegisterClipboardFormat( CF_BAND );
	m_cfBandList = ::RegisterClipboardFormat( CF_BANDLIST );
	if( m_cfBand == 0
	||  m_cfBandList == 0 )
	{
		CleanUp();
		::LoadString( theApp.m_hInstance, IDS_ERR_REGISTER_CF, achErrMsg, MID_BUFFER );
		strErrMsg = achErrMsg;
		*pbstrErrMsg = strErrMsg.AllocSysString();
		return E_FAIL;
	}

	// Tell Framework that the CF_BAND format can be used to create a .bnj file 
	CString strExt = _T(".bnj");
	BSTR bstrExt = strExt.AllocSysString();
	if( !SUCCEEDED ( pIFramework->RegisterClipFormatForFile(m_cfBand, bstrExt) ) )
	{
		CleanUp();
		::LoadString( theApp.m_hInstance, IDS_ERR_ADD_CLIPFORMAT, achErrMsg, MID_BUFFER );
		strErrMsg = achErrMsg;
		*pbstrErrMsg = strErrMsg.AllocSysString();
		return E_FAIL;
	}

	// Register applicable doc types with the Framework 
	m_pIDocType = new CBandDocType( this );
    if( m_pIDocType == NULL )
    {
		CleanUp();
		::LoadString( theApp.m_hInstance, IDS_ERR_MEMORY, achErrMsg, MID_BUFFER );
		strErrMsg = achErrMsg;
		*pbstrErrMsg = strErrMsg.AllocSysString();
        return E_OUTOFMEMORY;
    }

	m_pIDocType->AddRef();

	if( !SUCCEEDED ( pIFramework->AddDocType(m_pIDocType) ) )
	{
		CleanUp();
		::LoadString( theApp.m_hInstance, IDS_ERR_ADD_DOCTYPE, achErrMsg, MID_BUFFER );
		strErrMsg = achErrMsg;
		*pbstrErrMsg = strErrMsg.AllocSysString();
		return E_FAIL;
	}

@xref <i IDMUSProdFramework>, <om IDMUSProdFramework.SaveClipFormat>, <om IDMUSProdFramework.GetDocRootNodeFromData>
--------------------------------------------------------------------------------------*/

/////////////////////////////////////////////////////////////////////////////
// CFramework::RegisterClipFormatForFile

HRESULT CFramework::RegisterClipFormatForFile(
	UINT uClipFormat,		// @parm [in] Clipboard format used to create files
							//		with an extension equal to <p bstrExt>.
	BSTR bstrExt			// @parm [in] File extension.  This method frees
							//		<p bstrExt> with SysFreeString when it is no
							//		longer needed.
)
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	if( bstrExt == NULL )
	{
		return E_INVALIDARG;
	}

	CString strExt = bstrExt;
	::SysFreeString( bstrExt );

	if( uClipFormat == 0 )
	{
		return E_INVALIDARG;
	}

	// Place the new ClipFormat in list
	CJzClipFormat* pJzClipFormat = new CJzClipFormat( uClipFormat, strExt ); 
	if( pJzClipFormat == NULL )
	{
		return E_OUTOFMEMORY;
	}

	m_lstClipFormats.AddTail( pJzClipFormat );
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CFramework::LoadCF_DMUSPROD_FILE

HRESULT CFramework::LoadCF_DMUSPROD_FILE( IStream* pIStream, CFProducerFile* pProducerFile )
{
	HRESULT hr = E_FAIL;
    IDMUSProdRIFFStream* pIRiffStream = NULL;
	CString strFileName;
    MMCKINFO ck;
	DWORD dwSize;
    WORD wStructSize;
	ioCFProducerFile iCFProducerFile;
	
	if( pIStream == NULL
	||  pProducerFile == NULL )
	{
		return E_INVALIDARG;
	}

    // Create RIFF stream
	hr = AllocRIFFStream( pIStream, &pIRiffStream );
	if( FAILED( hr ) )
	{
		goto ON_ERROR;
	}

	// Find the FOURCC_CF_DMUSPROD_FILE chunk header
	ck.ckid = FOURCC_CF_DMUSPROD_FILE;
    if( pIRiffStream->Descend( &ck, NULL, MMIO_FINDCHUNK ) != 0 )
    {
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Read size of ioCFProducerFile structure
    dwSize = ck.cksize;
    pIStream->Read( &wStructSize, sizeof( wStructSize ), NULL );
    dwSize -= sizeof( wStructSize );

	// Initialize ioCFProducerFile structure
	memset( &iCFProducerFile, 0, sizeof(ioCFProducerFile) );

	// Read ioCFProducerFile structure
    if( wStructSize > sizeof(ioCFProducerFile) )
    {
        pIStream->Read( &iCFProducerFile, sizeof(ioCFProducerFile), NULL );
        StreamSeek( pIStream, wStructSize - sizeof(ioCFProducerFile), STREAM_SEEK_CUR );
    }
    else
    {
        pIStream->Read( &iCFProducerFile, wStructSize, NULL );
    }
    dwSize -= wStructSize;

	// Read filename (unicode format)
	if( dwSize > 0 )
	{
		ReadMBSfromWCS( pIStream, dwSize, &strFileName );
	}

	pProducerFile->guidFile = iCFProducerFile.guidFile;
	pProducerFile->strFileName = strFileName;
	hr = S_OK;

ON_ERROR:
	if( pIRiffStream )
	{
		pIRiffStream->Release();
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CFramework::LoadClipFormat

HRESULT CFramework::LoadClipFormat( IDataObject* pIDataObject, UINT uClipFormat, IStream** ppIStream )
{
	if( ppIStream == NULL )
	{
		return E_POINTER;
	}

	*ppIStream = NULL;

	if( pIDataObject == NULL
	||  uClipFormat == 0 )
	{
		return E_INVALIDARG;
	}

	// Create a new CDllJazzDataObject and see if it can read the data object's format.
	CAppJazzDataObject* pDataObject = new CAppJazzDataObject();
	if( pDataObject == NULL )
	{
		return E_OUTOFMEMORY;
	}

	HRESULT hr = E_FAIL;

	// Check to see if specified format is available
	if( SUCCEEDED (	pDataObject->IsClipFormatAvailable( pIDataObject, uClipFormat ) ) )
	{
		IStream* pIStream;

		// Get stream containing specified format
		if( SUCCEEDED (	pDataObject->AttemptRead( pIDataObject, uClipFormat, &pIStream ) ) )
		{
			LARGE_INTEGER liTemp;

			// Seek to beginning of stream
			liTemp.QuadPart = 0;
			pIStream->Seek( liTemp, STREAM_SEEK_SET, NULL );

			*ppIStream = pIStream;
			hr = S_OK;
		}
	}

	pDataObject->Release();

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CFramework::SaveCF_DMUSPROD_FILE

HRESULT CFramework::SaveCF_DMUSPROD_FILE( IDMUSProdNode* pINode, IStream** ppIStream )
{
	HRESULT hr;
	CString strName;
    MMCKINFO ck;
	DWORD dwBytesWritten;
    WORD wStructSize;
	ioCFProducerFile oCFProducerFile;
	
	if( ppIStream == NULL )
	{
		return E_POINTER;
	}

	*ppIStream = NULL;

	if( pINode == NULL )
	{
		return E_INVALIDARG;
	}

	// Get CFileNode for pINode (when applicable)
	CFileNode* pFileNode = NULL;

	GUID guidNodeId;
	if( SUCCEEDED ( pINode->GetNodeId( &guidNodeId ) ) )
	{
		if( IsEqualGUID( guidNodeId, GUID_FileNode ) )
		{
			pFileNode = (CFileNode *)pINode;
		}
	}
	if( pFileNode == NULL )
	{
		if( IsDocRootNode( pINode ) )
		{
			CComponentDoc* pComponentDoc = theApp.FindComponentDocument( pINode );
			if( pComponentDoc )
			{
				pFileNode = pComponentDoc->m_pFileNode;
			}

			ASSERT( pFileNode != NULL );
		}
	}

    IStream* pIMemStream = NULL;
    IDMUSProdRIFFStream* pIRiffStream = NULL;

	// Create an IStream
	hr = AllocMemoryStream( FT_DESIGN, GUID_CurrentVersion, &pIMemStream );
	if( FAILED ( hr ) )
	{
		goto ON_ERROR;
	}

	// Create a RIFFStream
    hr = AllocRIFFStream( pIMemStream, &pIRiffStream );
	if( FAILED ( hr ) )
	{
		goto ON_ERROR;
	}

	// Write CF_DMUSPROD_FILE chunk header
	ck.ckid = FOURCC_CF_DMUSPROD_FILE;
	if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Save size of ioCFProducerFile structure
	wStructSize = sizeof(ioCFProducerFile);
	hr = pIMemStream->Write( &wStructSize, sizeof(wStructSize), &dwBytesWritten );
	if( FAILED( hr )
	||  dwBytesWritten != sizeof(wStructSize) )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Prepare ioCFProducerFile structure
	memset( &oCFProducerFile, 0, sizeof(ioCFProducerFile) );
	if( pFileNode )
	{
		memcpy( &oCFProducerFile.guidFile, &pFileNode->m_guid, sizeof( oCFProducerFile.guidFile ) );
	}
	else
	{
		oCFProducerFile.guidFile = GUID_AllZeros;
	}

	// Write ioCFProducerFile structure
	hr = pIMemStream->Write( &oCFProducerFile, sizeof(ioCFProducerFile), &dwBytesWritten);
	if( FAILED( hr )
	||  dwBytesWritten != sizeof(ioCFProducerFile) )
	{
        hr = E_FAIL;
        goto ON_ERROR;
	}

	// Determine name
	if( pFileNode )
	{
		strName = pFileNode->m_strName;
	}
	else
	{
		// Node is not a file so just save the node's name
		BSTR bstrNodeName;

		if( SUCCEEDED ( pINode->GetNodeName( &bstrNodeName ) ) )
		{
			strName = bstrNodeName;
			::SysFreeString( bstrNodeName );
		}
	}

	// Write name in unicode format
	hr = SaveMBStoWCS( pIMemStream, &strName );
	if( FAILED( hr ) )
	{
        hr = E_FAIL;
        goto ON_ERROR;
	}

	if( pIRiffStream->Ascend( &ck, 0 ) != 0 )
	{
 		hr = E_FAIL;
		goto ON_ERROR;
	}

	pIMemStream->AddRef();
	*ppIStream = pIMemStream;
	hr = S_OK;

ON_ERROR:
	if( pIRiffStream )
	{
		pIRiffStream->Release();
	}

	if( pIMemStream )
	{
		pIMemStream->Release();
	}

	return hr;
}


/*======================================================================================
METHOD:  IDMUSPRODFRAMEWORK::SAVECLIPFORMAT
========================================================================================
@method HRESULT | IDMUSProdFramework | SaveClipFormat | Returns a stream containing the
		specified clipboard format for <p pINode>.
 
@comm
	A <o Component>'s <om IDMUSProdComponent.Initialize> method can call
	<om IDMUSProdFramework.RegisterClipFormatForFile> to make the Framework aware of clipboard
	formats that can be used to create files in the Project Tree.  

	Data objects which have the potential of being pasted as DirectMusic files must contain at
	least two clipboard formats:  CF_DMUSPROD_FILE, and their custom format for file content.  The
	custom format must be registered with the Framework via a call to
	<om IDMUSProdFramework.RegisterClipFormatForFile>.  CF_DMUSPROD_FILE clipboard formats are created
	by calling <om IDMUSProdFramework.SaveClipFormat>.
	
    The following list contains valid values for <p uClipFormat>:

	CF_DMUSPROD_FILE<tab><tab>The Framework uses CF_DMUSPROD_FILE format when pasting streams into files.
	
 
@rvalue S_OK | <p uClipFormat> was created for < p pINode> and was returned in <p ppIStream>.
@rvalue E_POINTER | The address in <p ppIStream> is not valid.  For example, it may be NULL.
@rvalue E_INVALIDARG | Either <p uClipFormat> or <p pINode> is not valid.  For example,
	<p uClipFormat> may be zero.
@rvalue E_OUTOFMEMORY | Out of memory. 
@rvalue E_FAIL | An error occurred, and the stream could not be created. 

@ex The following example fills a data object with the clipboard formats needed to paste a Band
	file into the Project Tree:|

HRESULT CBand::CreateDataObject( IDataObject** ppIDataObject )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pComponent != NULL );

	if( ppIDataObject == NULL )
	{
		return E_POINTER;
	}

	*ppIDataObject = NULL;

	// Create the CDllJazzDataObject 
	CDllJazzDataObject* pDataObject = new CDllJazzDataObject();
	if( pDataObject == NULL )
	{
		return E_OUTOFMEMORY;
	}

	IStream* pIStream;

	// Save Band into stream
	HRESULT hr = E_FAIL;

	if( SUCCEEDED ( theApp.m_pIFramework->AllocMemoryStream(FT_DESIGN, GUID_CurrentVersion, &pIStream) ) )
	{
		if( SUCCEEDED ( Save( pIStream, FALSE ) ) )
		{
			// Place CF_BAND into CDllJazzDataObject
			if( SUCCEEDED ( pDataObject->AddClipFormat( m_pComponent->m_cfBand, pIStream ) ) )
			{
				hr = S_OK;
			}
		}

		pIStream->Release();
	}

	if( SUCCEEDED ( hr ) )
	{
		hr = E_FAIL;

		// Create a stream in CF_DMUSPROD_FILE format
		if( SUCCEEDED ( theApp.m_pIFramework->SaveClipFormat( m_pComponent->m_cfProducerFile, this, &pIStream ) ) )
		{
			// There is the potential for a Band node to become a file
			// so we must also place CF_DMUSPROD_FILE into CDllJazzDataObject
			if( SUCCEEDED ( pDataObject->AddClipFormat( m_pComponent->m_cfProducerFile, pIStream ) ) )
			{
				hr = S_OK;
			}

			pIStream->Release();
		}
	}

	if( SUCCEEDED ( hr ) )
	{
		*ppIDataObject = pDataObject;	// already AddRef'd
	}
	else
	{
		pDataObject->Release();
	}

	return hr;
}


@xref <i IDMUSProdFramework>, <om IDMUSProdFramework.RegisterClipFormatForFile>, <om IDMUSProdFramework.GetDocRootNodeFromData>
--------------------------------------------------------------------------------------*/

/////////////////////////////////////////////////////////////////////////////
// CFramework::SaveClipFormat

HRESULT CFramework::SaveClipFormat(
	UINT uClipFormat,		// @parm [in] Clipboard format. 
	IDMUSProdNode* pINode,	// @parm [in] A pointer to the <o Node> whose clipboard
							//		format be returned in <p ppIStream>.
	IStream **ppIStream		// @parm [out,retval] Address of a variable to receive the requested
							//		<i IStream> interface.  If an error occurs, the implementation
							//		sets <p ppIStream> to NULL.  On success, the caller is responsible
							//		for calling <om IStream.Release>.
)
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	if( ppIStream == NULL )
	{
		return E_POINTER;
	}

	*ppIStream = NULL;

	if( uClipFormat == 0
	||  pINode == NULL )
	{
		return E_INVALIDARG;
	}

	if( uClipFormat == m_cfProducerFile )
	{
		// Create the CF_DMUSPROD_FILE stream
		return SaveCF_DMUSPROD_FILE( pINode, ppIStream );
	}
	
	return E_FAIL;
}


/*======================================================================================
METHOD:  IDMUSPRODFRAMEWORK::GETDOCROOTNODEFROMDATA
========================================================================================
@method HRESULT | IDMUSProdFramework | GetDocRootNodeFromData | Returns an <i IDMUSProdNode>
		interface pointer for the DocRoot <o Node> in <p pIDataObject>.
 
@comm
	A DocRoot Node is the top Node of a file.  For more information refer to <o Node>
	Object.
		
@rvalue S_OK | The matching DocRoot Node was returned in <p ppIDocRootNode>. 
@rvalue E_POINTER | The address in <p ppIDocRootNode> is not valid.  For example, it may be NULL.
@rvalue E_INVALIDARG | <p pIDataObject> is not valid.  For example, it may be NULL.
@rvalue E_OUTOFMEMORY | Out of memory. 
@rvalue E_FAIL | An error occurred, and a DocRoot Node could not be returned.

@ex The following example: |


@xref <i IDMUSProdFramework>, <om IDMUSProdFramework.SaveClipFormat>, <om IDMUSProdFramework.RegisterClipFormatForFile>, 
--------------------------------------------------------------------------------------*/

/////////////////////////////////////////////////////////////////////////////
// CFramework::GetDocRootNodeFromData

HRESULT CFramework::GetDocRootNodeFromData(
	IDataObject* pIDataObject,	// @parm [in] Pointer to an IDataObject interface.
	IDMUSProdNode** ppIDocRootNode	// @parm [out,retval] Address of a variable to receive
								//		the requested IDMUSProdNode interface.
								//		If an error occurs, the implementation
								//		sets <p ppIDocRootNode> to NULL.  On success, the caller
								//		is responsible for calling <om IDMUSProdNode.Release>.
)
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	if( ppIDocRootNode == NULL )
	{
		return E_POINTER;
	}

	*ppIDocRootNode = NULL;

	if( pIDataObject == NULL )
	{
		return E_INVALIDARG;
	}

	IStream* pIStream;

	HRESULT hr = E_FAIL;

	if( SUCCEEDED ( LoadClipFormat( pIDataObject, m_cfProducerFile, &pIStream ) ) )
	{
		CFProducerFile cfProducerFile;

		if( SUCCEEDED ( LoadCF_DMUSPROD_FILE( pIStream, &cfProducerFile ) ) )
		{
			if( !( IsEqualGUID( cfProducerFile.guidFile, GUID_AllZeros ) ) )
			{
				CFileNode* pFileNode = theApp.GetFileByGUID( cfProducerFile.guidFile );
				if( pFileNode )
				{
					if( pFileNode->m_pIChildNode )
					{
						pFileNode->m_pIChildNode->AddRef();
						*ppIDocRootNode = pFileNode->m_pIChildNode;
						hr = S_OK;
					}

					pFileNode->Release();
				}
			}
		}

		pIStream->Release();
	}

	return hr;
}


/*======================================================================================
METHOD:  IDMUSPRODFRAMEWORK::GETFIRSTPROJECT
========================================================================================
@method HRESULT | IDMUSProdFramework | GetFirstProject | Returns an <i IDMUSProdProject> interface
		pointer for the first <o Project> in the application's list of Projects.

@comm
	<p ppIFirstProject> is set to NULL if the list is empty.

@rvalue S_OK | The first Project was returned in <p ppIFirstProject>. 
@rvalue E_POINTER | The address in <p ppIFirstProject> is not valid.  For example, it may
		be NULL.
@rvalue E_FAIL | An error occurred, and the first Project could not be returned.

@ex The following example: |

@xref <i IDMUSProdFramework>, <om IDMUSProdFramework.GetNextProject>, <i IDMUSProdProject>
--------------------------------------------------------------------------------------*/

/////////////////////////////////////////////////////////////////////////////
// CFramework::GetFirstProject

HRESULT CFramework::GetFirstProject(
	IDMUSProdProject** ppIFirstProject	// @parm [out,retval] Address of a variable to receive the requested  
									//		<i IDMUSProdProject> interface.  If an error occurs, the implementation
									//		sets <p ppIFirstProject> to NULL.  On success, the
									//		caller is responsible for calling
									//		<om IDMUSProdProject.Release>.
)
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	return theApp.GetFirstProject( ppIFirstProject );
}


/*======================================================================================
METHOD:  IDMUSPRODFRAMEWORK::GETNEXTPROJECT
========================================================================================
@method HRESULT | IDMUSProdFramework | GetNextProject | Returns an <i IDMUSProdProject> interface
		pointer for the next <o Project> in the application's list of Projects.

@comm
	Returns the Project located after <p pIProject> in the application's list of Projects.

	<p ppINextProject> is set to NULL when the end of the list has been reached.

@rvalue S_OK | The next Project was returned in <p ppINextProject>. 
@rvalue E_POINTER | The address in <p ppINextProject> is not valid.  For example, it may
		be NULL.
@rvalue E_INVALIDARG | <p pIProject> is not valid.  For example, it may be NULL.
@rvalue E_FAIL | An error occurred, and the next Project could not be returned.

@ex The following example: |

@xref <i IDMUSProdFramework>, <om IDMUSProdFramework.GetFirstProject>, <i IDMUSProdProject>
--------------------------------------------------------------------------------------*/

/////////////////////////////////////////////////////////////////////////////
// CFramework::GetNextProject

HRESULT CFramework::GetNextProject(
	IDMUSProdProject* pIProject,		// @parm [in] A pointer to the previous Project in
										//		the application's list of Projects.
	IDMUSProdProject** ppINextProject	// @parm [out,retval] Address of a variable to receive the requested  
										//		<i IDMUSProdProject> interface.  If an error occurs, the implementation
										//		sets <p ppINextProject> to NULL.  On success, the
										//		caller is responsible for calling
										//		<om IDMUSProdProject.Release>.
)
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	return theApp.GetNextProject( pIProject, ppINextProject );
}


/*======================================================================================
METHOD:  IDMUSPRODFRAMEWORK::FINDPROJECT
========================================================================================
@method HRESULT | IDMUSProdFramework | FindProject | Returns an <i IDMUSProdProject> interface
		pointer for the <o Project> containing <p pINode>.

@comm

@rvalue S_OK | <p pINode>'s Project was returned in <p ppIProject>. 
@rvalue E_POINTER | The address in <p ppIProject> is not valid.  For example, it may
		be NULL.
@rvalue E_FAIL | An error occurred, and <p pINode>'s Project could not be returned.

@ex The following example: |

@xref <i IDMUSProdFramework>, <i IDMUSProdProject>
--------------------------------------------------------------------------------------*/

/////////////////////////////////////////////////////////////////////////////
// CFramework::FindProject

HRESULT CFramework::FindProject(
	IDMUSProdNode* pINode,			// @parm [in] A pointer to the node whose Project
									//		will be returned in <p ppIProject>.
	IDMUSProdProject** ppIProject	// @parm [out,retval] Address of a variable to receive the requested  
									//		<i IDMUSProdProject> interface.  If an error occurs, the implementation
									//		sets <p ppIProject> to NULL.  On success, the
									//		caller is responsible for calling
									//		<om IDMUSProdProject.Release>.
)
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	if( ppIProject == NULL )
	{
		return E_POINTER;
	}

	*ppIProject = NULL;

	if( pINode == NULL )
	{
		return E_INVALIDARG;
	}

	CTreeCtrl* pTreeCtrl = theApp.GetProjectTreeCtrl();
	if( pTreeCtrl == NULL )
	{
		return E_FAIL;
	}

	IDMUSProdNode* pITreeNode;
	GUID guidTreeNodeId;

	HTREEITEM hItem = FindTreeItem( pINode );

	while( hItem )
	{
		CJzNode* pJzNode = (CJzNode *)pTreeCtrl->GetItemData( hItem );
		if( pJzNode )
		{
			pITreeNode = pJzNode->m_pINode;
			ASSERT( pITreeNode != NULL );

			if( pITreeNode )
			{
				if( SUCCEEDED ( pITreeNode->GetNodeId( &guidTreeNodeId ) ) )
				{
					if( IsEqualGUID( guidTreeNodeId, GUID_ProjectFolderNode ) )
					{
						if( FAILED ( pITreeNode->QueryInterface( IID_IDMUSProdProject, (void **)ppIProject ) ) )
						{
							*ppIProject = NULL;
						}
						break;
					}
				}
			}
		}

		hItem = pTreeCtrl->GetNextItem( hItem, TVGN_PARENT );
	}

	if( *ppIProject )
	{
		return S_OK;
	}

	return E_FAIL;
}


/*======================================================================================
METHOD:  IDMUSPRODFRAMEWORK::ISPROJECTEQUAL
========================================================================================
@method HRESULT | IDMUSProdFramework | IsProjectEqual | Determines whether <p pIDocRootNode1>
		and <p pIDocRootNode2> are in the same <o Project>. 

@comm
	<p pIDocRootNode1> and <p pIDocRootNode2> must point to DocRoot <o Node>s.

	A DocRoot node is the top node of a document, or file.  For example, the Style node of
	a Style file would be considered the DocRoot node of the Style.  The Band node of a Band
	file would be considered the DocRoot node of the Band file.

@rvalue S_OK | <p pIDocRootNode1> and <p pIDocRootNode2> are in the same Project.
@rvalue S_FALSE | <p pIDocRootNode1> and <p pIDocRootNode2> are not in the same Project.
@rvalue E_INVALIDARG | The value of either <p pIDocRootNode1> or <p pIDocRootNode2> is
		not valid.  For example, it may not point to a DocRoot node.
@rvalue E_FAIL | An error occurred.  Cannot determine whether the specified DocRoot nodes
		are in the same Project.

@ex The following example makes sure that a referenced file exists in the same Project as the
	file it references.  m_pIStyleRefNode contains an IDMUSProdNode interface pointer to the node
	used to display a referenced Style file:|

	IDMUSProdReferenceNode* pIReferenceNode;

	if( SUCCEEDED ( m_pIStyleRefNode->QueryInterface( IID_IDMUSProdReferenceNode, (void **)&pIReferenceNode ) ) )
	{
		IDMUSProdNode* pIStyleDocRootNode;

		if( SUCCEEDED ( pIReferenceNode->GetReferencedFile( &pIStyleDocRootNode ) ) )
		{
			if( pIFramework->IsProjectEqual( this, pIStyleDocRootNode ) == S_FALSE )
			{
				IDMUSProdNode* pINewDocRootNode;

				if( SUCCEEDED ( pIFramework->CopyFile( pIStyleDocRootNode, this, &pINewDocRootNode ) ) )
				{
					pIReferenceNode->SetReferencedFile( pINewDocRootNode );
					pINewDocRootNode->Release();
				}
			}

			pIStyleDocRootNode->Release();
		}

		pIReferenceNode->Release();
	}

@xref <i IDMUSProdFramework>, <om IDMUSProdFramework.CopyFile>
--------------------------------------------------------------------------------------*/

/////////////////////////////////////////////////////////////////////////////
// CFramework::IsProjectEqual

HRESULT CFramework::IsProjectEqual(
	IDMUSProdNode* pIDocRootNode1,		// @parm [in] Pointer to the <i IDMUSProdNode> interface of
									//		a DocRoot node.
	IDMUSProdNode* pIDocRootNode2		// @parm [in] Pointer to the <i IDMUSProdNode> interface of
									//		a DocRoot node.
)
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	if( pIDocRootNode1 == NULL
	||  pIDocRootNode2 == NULL )
	{
		return E_INVALIDARG;
	}

	// Make sure pIDocRootNode1 really is a DocRootNode
	IDMUSProdNode* pINode;

	if( FAILED ( pIDocRootNode1->GetDocRootNode( &pINode ) ) )
	{
		return E_FAIL;
	}
	ASSERT( pIDocRootNode1 == pINode );
	if( pINode == NULL )
	{
		return E_FAIL;
	}
	if( pIDocRootNode1 != pINode )
	{
		pINode->Release();
		return E_INVALIDARG;
	}
	pINode->Release();

	// Make sure pIDocRootNode2 really is a DocRootNode
	if( FAILED ( pIDocRootNode2->GetDocRootNode( &pINode ) ) )
	{
		return E_FAIL;
	}
	ASSERT( pIDocRootNode2 == pINode );
	if( pINode == NULL )
	{
		return E_FAIL;
	}
	if( pIDocRootNode2 != pINode )
	{
		pINode->Release();
		return E_INVALIDARG;
	}
	pINode->Release();

	HRESULT hr = E_FAIL;
	
	// See if DocRootNodes are in the same Project
	IDMUSProdProject* pIProject1;
	IDMUSProdProject* pIProject2;

	if( SUCCEEDED ( FindProject( pIDocRootNode1, &pIProject1 ) ) )
	{
		if( SUCCEEDED ( FindProject( pIDocRootNode2, &pIProject2 ) ) )
		{
			if( pIProject1 == pIProject2 )
			{
				hr = S_OK;
			}
			else
			{
				hr = S_FALSE;
			}

			pIProject2->Release();
		}

		pIProject1->Release();
	}

	return hr;
}


/*======================================================================================
METHOD:  IDMUSPRODFRAMEWORK::ADDNODEIMAGELIST
========================================================================================
@method HRESULT | IDMUSProdFramework | AddNodeImageList | Adds images to the Project Tree's
		image list. 

@comm
	If a Component inserts nodes into the Project Tree, its <om IDMUSProdComponent.Initialize>
	method must call <om IDMUSProdFramework.AddNodeImageList> so that the Component can add
	its images to the Project Tree's image list.

	Project Tree images must be 16 X 16 and contain 256 colors.

	The Project Tree maintains a list of images used when drawing nodes.  As nodes are
	inserted into the Project Tree, the Framework calls <om IDMUSProdNode.GetNodeImageIndex>
	so that it can associate a pair of consecutive images in its list with the node
	being inserted.  The index returned by <om IDMUSProdNode.GetNodeImageIndex> is based
	<p pnNbrfirstImage>.  Images must always be added in pairs.  The
	<om IDMUSProdNode.UseOpenCloseImages> method determines whether the consecutive images
	pertain to non-selected/selected states or closed/opened states (folders).  

@rvalue S_OK | The image list was added successfully and the index to the first image in
		the list was returned in <p pnNbrFirstImage>. 
@rvalue E_POINTER | The address in <p pnNbrFirstImage> is not valid.  For example, it may
		be NULL.
@rvalue E_FAIL | The images were not added to the Project Tree's image list.

@ex The following example adds images to the Project Tree's image list: |

HRESULT CMotifComponent::AddNodeImageLists( void )
{
	CImageList lstImages;
	HICON hIcon;

	lstImages.Create( 16, 16, ILC_COLOR16, 2, 0 );
	lstImages.SetBkColor( GetSysColor(COLOR_WINDOW) );

	hIcon = ::LoadIcon( theApp.m_hInstance, MAKEINTRESOURCE(IDR_MOTIF_DOCTYPE) );
	lstImages.Add( hIcon );
	::DestroyIcon( hIcon );
	hIcon = ::LoadIcon( theApp.m_hInstance, MAKEINTRESOURCE(IDI_MOTIF_DOCTYPE_SEL) );
	lstImages.Add( hIcon );
	::DestroyIcon( hIcon );

	if( FAILED (m_pIFramework->AddNodeImageList( lstImages.Detach(), &m_nFirstMotifImage ) ) )
	{
		return E_FAIL;
	}
	
	lstImages.Create( 16, 16, ILC_COLOR16, 2, 0 );
	lstImages.SetBkColor( GetSysColor(COLOR_WINDOW) );

	hIcon = ::LoadIcon( theApp.m_hInstance, MAKEINTRESOURCE(IDI_FOLDER) );
	lstImages.Add( hIcon );
	::DestroyIcon( hIcon );
	hIcon = ::LoadIcon( theApp.m_hInstance, MAKEINTRESOURCE(IDI_FOLDER_SEL) );
	lstImages.Add( hIcon );
	::DestroyIcon( hIcon );

	if( FAILED (m_pIFramework->AddNodeImageList( lstImages.Detach(), &m_nFirstFolderImage ) ) )
	{
		return E_FAIL;
	}
	
	return S_OK;
}
	
@xref <om IDMUSProdComponent.Initialize>, <i IDMUSProdFramework>, <i IDMUSProdNode>, <om IDMUSProdNode.GetNodeImageIndex>
--------------------------------------------------------------------------------------*/

/////////////////////////////////////////////////////////////////////////////
// CFramework::AddNodeImageList

HRESULT CFramework::AddNodeImageList(
	HANDLE hNodeImageList,		// @parm [in] Handle to image list to be added.
	short* pnNbrFirstImage		// @parm [out,retval] Pointer to corresponding index into
								//		Project Tree's image list.
								//		<om IDMUSProdNode.GetNodeImageIndex> returns an index
								//		based on <p pnNbrFirstImage>.
)
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	CTreeCtrl* pTreeCtrl;
    CImageList lstNodeImages;
    CImageList* plstTreeImages;
    int nNbrNewImages;
    int nNbrTreeImages;
	int i;

	ASSERT( hNodeImageList != NULL );

	if( pnNbrFirstImage == NULL )
	{
		return E_POINTER;
	}

	pTreeCtrl = theApp.GetProjectTreeCtrl();
	if( pTreeCtrl == NULL )
	{
		return E_FAIL;
	}

	lstNodeImages.Attach( (HIMAGELIST)hNodeImageList );
    nNbrNewImages = lstNodeImages.GetImageCount();

    plstTreeImages = pTreeCtrl->GetImageList( TVSIL_NORMAL );

    if( plstTreeImages == NULL )
    {
        pTreeCtrl->SetImageList( &lstNodeImages, TVSIL_NORMAL );
		*pnNbrFirstImage = 0;
    }
    else
    {
		CImageList lstNewTreeImages;

	    nNbrTreeImages = plstTreeImages->GetImageCount();
		*pnNbrFirstImage = nNbrTreeImages;

		lstNewTreeImages.Create( 16, 16, ILC_COLOR16, nNbrTreeImages + nNbrNewImages, 0 );

		for( i = 0 ; i < nNbrTreeImages ; ++i )
		{
			HICON hIcon = plstTreeImages->ExtractIcon( i );
			lstNewTreeImages.Add( hIcon );
			::DestroyIcon( hIcon );
		}

		for( i = 0 ; i < nNbrNewImages ; ++i )
		{
			HICON hIcon = lstNodeImages.ExtractIcon( i );
			lstNewTreeImages.Add( hIcon );
			::DestroyIcon( hIcon );
		}


		CImageList* pImageList = pTreeCtrl->SetImageList( &lstNewTreeImages, TVSIL_NORMAL );
		if( pImageList )
		{
			pImageList->DeleteImageList();
		}

		plstTreeImages->Detach();
	    lstNewTreeImages.Detach();
    }

    lstNodeImages.Detach();

	return S_OK;
}


/*======================================================================================
METHOD:  IDMUSPRODFRAMEWORK::GETNODEFILENAME
========================================================================================
@method HRESULT | IDMUSProdFramework | GetNodeFileName | Returns the path/filename of the
		file associated with <p pINode>. 

@rvalue S_OK | The filename was returned in <p pbstrFileName>.
@rvalue E_POINTER | The address in <p pbstrFileName> is not valid.  For example, it may be NULL.
@rvalue E_INVALIDARG | The address in <p pINode> is not valid.  For example, it may be NULL.
@rvalue E_FAIL | An error occurred and the filename was not returned.

@ex The following example retrieves the filename of the file which contains <p pINode>: |

	IDMUSProdFramework pIFramework;
	CString strFileName;
	BSTR bstrFileName;

	if( SUCCEEDED ( pIFramework->GetNodeFileName((IDMUSProdNode *)this, &bstrFileName) ) )
	{
		strFileName = bstrFileName;
		::SysFreeString( bstrFileName );
	}
	
@xref <i IDMUSProdFramework>, <om IDMUSProdFramework.SaveNode>, <om IDMUSProdFramework.SaveNodeAsNewFile>, <i IDMUSProdNode>
--------------------------------------------------------------------------------------*/

/////////////////////////////////////////////////////////////////////////////
// CFramework::GetNodeFileName

HRESULT CFramework::GetNodeFileName(
	IDMUSProdNode* pINode,		// @parm [in] Pointer to any <i IDMUSProdNode> in the file.
	BSTR* pbstrFileName		// @parm [out,retval] Pointer to the caller-allocated variable
							// that receives the copy of the filename.  The caller must
							// free <p pbstrFileName> with SysFreeString when it is no longer
							// needed.
)
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	if( pbstrFileName == NULL )
	{
		return E_POINTER;
	}

	*pbstrFileName = NULL;

	if( pINode == NULL )
	{
		return E_INVALIDARG;
	}

	CString strFileName;

	GUID guidNodeId;
	if( SUCCEEDED ( pINode->GetNodeId ( &guidNodeId ) ) )
	{
		// Handle Project Folder Nodes
		if( IsEqualGUID( guidNodeId, GUID_ProjectFolderNode ) )
		{
			CProject* pProject = (CProject *)pINode;

			// Get file extension used for Projects
			CString strFilterExt;

			ASSERT( pProject->m_pProjectDoc != NULL );
			CJazzDocTemplate* pDocTemplate = (CJazzDocTemplate *)pProject->m_pProjectDoc->GetDocTemplate();
			ASSERT( pDocTemplate != NULL );

			pDocTemplate->GetDocString( strFilterExt, CDocTemplate::filterExt );
			strFileName = pProject->m_strProjectDir + pProject->m_strName + strFilterExt; 
			*pbstrFileName = strFileName.AllocSysString();
			return S_OK;
		}

		// Handle FileNodes
		else if( IsEqualGUID( guidNodeId, GUID_FileNode ) )
		{
			CFileNode* pFileNode = (CFileNode *)pINode;

			pFileNode->ConstructFileName( strFileName );
			*pbstrFileName = strFileName.AllocSysString();
			return S_OK;
		}
	}

	// Handle other Nodes
	CComponentDoc* pComponentDoc = theApp.FindComponentDocument( pINode );
	if( pComponentDoc )
	{
		strFileName = pComponentDoc->GetPathName();
		*pbstrFileName = strFileName.AllocSysString();
		return S_OK;
	}

	return E_FAIL;
}


/*======================================================================================
METHOD:  IDMUSPRODFRAMEWORK::GETNODEFILEGUID
========================================================================================
@method HRESULT | IDMUSProdFramework | GetNodeFileGuid | Returns the Producer generated
		GUID assigned to the file associated with <p pINode>. 

@comm	Producer assigns a GUID to every file in the Project Tree.  This GUID can be obtained
		via the <om IDMUSProdFramework::GetNodeFileGuid> method.  <p pguidFile> refers to this 
		GUID. 

		The <i IDMUSProdProject> interface provides methods to enumerate all files in a given
		Project.  The <i IDMUSProdNode> interface pointer returned by the 
		<om IDMUSProdProject::GetFirstFileByDocType> and  <om IDMUSProdProject::GetNextFileByDocType>
		methods can be passed to <om IDMUSProdFramework::GetNodeFileGuid> and used in
		conjunction with <om IDMUSProdFramework::FindDocRootNodeByFileGUID> to retrieve
		the DocRoot node for a specific file.

@rvalue S_OK | The file's GUID was placed in <p pguidFile>.
@rvalue E_POINTER | The address in <p pguidFile> is not valid.  For example, it may be NULL.
@rvalue E_INVALIDARG | The address in <p pINode> is not valid.  For example, it may be NULL.
@rvalue E_FAIL | An error occurred and the file's GUID was not returned.

@ex The following example retrieves the Producer generated GUID identifying the file which
	contains <p pINode>: |

	IDMUSProdFramework pIFramework;
	IDMUSProdNode* pIThisDocRootNode;
	IDMUSProdNode* pIDocRootNode;
	GUID guidFile;

	if( SUCCEEDED ( pIFramework->GetNodeFileGUID(pIThisDocRootNode, &guidFile) ) )
	{
		if( SUCCEEDED ( pIFramework->FindDocRootNodeByFileGUID(guidFile, &pIDocRootNode) ) )
		{
			ASSERT( pIDocRootNode == pIThisDocRootNode );	
		}
    }
	
@xref <i IDMUSProdFramework>, <i IDMUSProdProject>, <i IDMUSProdNode>, <om IDMUSProdFramework.FindDocRootNodeByFileGUID>
--------------------------------------------------------------------------------------*/

/////////////////////////////////////////////////////////////////////////////
// CFramework::GetNodeFileGUID

HRESULT CFramework::GetNodeFileGUID(
	IDMUSProdNode* pINode,	// @parm [in] Pointer to any <i IDMUSProdNode> in the file.
	GUID* pguidFile			// @parm [out,retval] Pointer to the caller-allocated variable
							//		that receives a copy of the file's GUID.
)
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	if( pguidFile == NULL )
	{
		return E_POINTER;
	}

	memset( pguidFile, 0, sizeof( GUID ) );

	if( pINode == NULL )
	{
		return E_INVALIDARG;
	}

	GUID guidNodeId;
	if( SUCCEEDED ( pINode->GetNodeId ( &guidNodeId ) ) )
	{
		// Handle Project Folder Nodes
		if( IsEqualGUID( guidNodeId, GUID_ProjectFolderNode ) )
		{
			return E_FAIL;
		}

		// Handle Directory Nodes
		if( IsEqualGUID( guidNodeId, GUID_DirectoryNode ) )
		{
			return E_FAIL;
		}

		// Handle FileNodes
		else if( IsEqualGUID( guidNodeId, GUID_FileNode ) )
		{
			CFileNode* pFileNode = (CFileNode *)pINode;

			*pguidFile = pFileNode->m_guid;
			return S_OK;
		}
	}

	// Handle other Nodes
	CComponentDoc* pComponentDoc = theApp.FindComponentDocument( pINode );
	if( pComponentDoc )
	{
		if( pComponentDoc->m_pFileNode )
		{
			*pguidFile = pComponentDoc->m_pFileNode->m_guid;
			return S_OK;
		}
	}

	return E_FAIL;
}


/*======================================================================================
METHOD:  IDMUSPRODFRAMEWORK::REVERTFILETOSAVED
========================================================================================
@method HRESULT | IDMUSProdFramework | RevertFileToSaved | Reverts the document associated with
	<p pINode> to its last saved state.

@comm
	This method provides the means for a Component to undo changes by reloading a file.  For
	example, the right-click context menu associated with a node can contain a 'Revert to Saved'
	menu item.

    The Framework obtains the DocRoot node for the document by using <p pINode> to call
	<om IDMUSProdNode.GetDocRootNode>.

@rvalue S_OK | The file associated with <p pINode> was successfully reverted to it last saved
		state.
@rvalue E_INVALIDARG | The address in <p pINode> is not valid.  For example, it may be NULL.
@rvalue E_FAIL | An error occurred and the file was not reverted.

@ex The following example reverts the document containing <p pINode> to its last saved state: |

HRESULT CMotif::OnRightClickMenuSelect( long lCommandId )
{
	ASSERT( m_pComponent != NULL );
	ASSERT( m_pComponent->m_pIFramework != NULL );

	HRESULT hr = E_FAIL;

	switch( lCommandId )
	{
		case IDM_REVERT:
			if( SUCCEEDED ( m_pComponent->m_pIFramework->RevertFileToSaved((IDMUSProdNode *)this) ) )
			{
				hr = S_OK;
			}
			break;
	}

	return hr;
}
	
@xref <i IDMUSProdFramework>, <om IDMUSProdFramework.GetNodeFileName>, <om IDMUSProdFramework.SaveNodeAsNewFile>, <i IDMUSProdNode>, <om IDMUSProdNode.GetDocRootNode>
--------------------------------------------------------------------------------------*/

/////////////////////////////////////////////////////////////////////////////
// CFramework::RevertFileToSaved

HRESULT CFramework::RevertFileToSaved(
	IDMUSProdNode* pINode		// @parm [in] Pointer to any <i IDMUSProdNode> in the file.
)
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	if( pINode == NULL )
	{
		return E_INVALIDARG;
	}

	CComponentDoc* pComponentDoc = theApp.FindComponentDocument( pINode );
	if( pComponentDoc )
	{
		pComponentDoc->m_bAction = DOC_ACTION_REVERT;
		if( pComponentDoc->RevertDocument() )
		{
			return S_OK;
		}
		pComponentDoc->m_bAction = DOC_ACTION_NONE;
	}

	return E_FAIL;
}


/*======================================================================================
METHOD:  IDMUSPRODFRAMEWORK::SAVENODE
========================================================================================
@method HRESULT | IDMUSProdFramework | SaveNode | Invokes the Framework's File Save command
		for the document associated with the DocRoot node of <p pINode>.

@comm
	This method provides the means for a Component to initiate a File Save command.  For
	example, it allows the right-click context menu associated with a file's DocRoot node
	to contain a menu item for saving itself.

    The Framework obtains the DocRoot node for the document by using <p pINode> to call
	<om IDMUSProdNode.GetDocRootNode>.

@rvalue S_OK | The file associated with <p pINode> was saved successfully.
@rvalue E_INVALIDARG | The address in <p pINode> is not valid.  For example, it may be NULL.
@rvalue E_FAIL | An error occurred and the file was not saved.

@ex The following example saves the document which contains <p pINode>: |

HRESULT CMotif::OnRightClickMenuSelect( long lCommandId )
{
	ASSERT( m_pComponent != NULL );
	ASSERT( m_pComponent->m_pIFramework != NULL );

	HRESULT hr = E_FAIL;

	IDMUSProdNode* pIDocRootNode;

	switch( lCommandId )
	{
		case IDM_SAVE:
			if( SUCCEEDED ( GetDocRootNode(&pIDocRootNode) ) )
			{
				if( SUCCEEDED ( m_pComponent->m_pIFramework->SaveNode(pIDocRootNode) ) )
				{
					hr = S_OK;
				}
				pIDocRootNode->Release();
			}
			break;
	}

	return hr;
}
	
@xref <i IDMUSProdFramework>, <om IDMUSProdFramework.GetNodeFileName>, <om IDMUSProdFramework.SaveNodeAsNewFile>, <i IDMUSProdNode>, <om IDMUSProdNode.GetDocRootNode>
--------------------------------------------------------------------------------------*/

/////////////////////////////////////////////////////////////////////////////
// CFramework::SaveNode

HRESULT CFramework::SaveNode(
	IDMUSProdNode* pINode		// @parm [in] Pointer to any <i IDMUSProdNode> in the file.
)
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	if( pINode == NULL )
	{
		return E_INVALIDARG;
	}

	CComponentDoc* pComponentDoc = theApp.FindComponentDocument( pINode );
	if( pComponentDoc )
	{
		if( pComponentDoc->DoFileSave() )
		{
			return S_OK;
		}
	}

	return E_FAIL;
}


/*======================================================================================
METHOD:  IDMUSPRODFRAMEWORK::SAVENODEASNEWFILE
========================================================================================
@method HRESULT | IDMUSProdFramework | SaveNodeAsNewFile | Invokes the <o Framework>'s File
	Save As command to create a new file for the <o Node> specified in <p pINode>. 

@comm
	This method provides the means for a Component to initiate a File Save As command.
	For example, it allows the right-click context menu associated with a Project Tree
	node to contain a menu item for saving itself as a new file.

	???? Need to explain how to get a Style's Band node into a Band file node.

@rvalue S_OK | <p pINode> was saved successfully as a new file.
@rvalue E_INVALIDARG | The address in <p pINode> is not valid.  For example, it may be NULL.
@rvalue E_FAIL | An error occurred and a new file was not saved.

@ex The following example saves the document containing <p pINode> as a new file: |

HRESULT CMotif::OnRightClickMenuSelect( long lCommandId )
{
	ASSERT( m_pComponent != NULL );
	ASSERT( m_pComponent->m_pIFramework != NULL );

	HRESULT hr = E_FAIL;

	IDMUSProdNode* pIDocRootNode;

	switch( lCommandId )
	{
		case IDM_SAVEAS:
			if( SUCCEEDED ( GetDocRootNode(&pIDocRootNode) ) )
			{
				if( SUCCEEDED ( m_pComponent->m_pIFramework->SaveNodeAsNewFile(pIDocRootNode) ) )
				{
					hr = S_OK;
				}
				pIDocRootNode->Release();
			}
			break;
	}

	return hr;
}
	
@xref <i IDMUSProdFramework>, <om IDMUSProdFramework.GetNodeFileName>, <om IDMUSProdFramework.SaveNode>, <i IDMUSProdNode>, <om IDMUSProdNode.GetDocRootNode>
--------------------------------------------------------------------------------------*/

/////////////////////////////////////////////////////////////////////////////
// CFramework::SaveNodeAsNewFile

HRESULT CFramework::SaveNodeAsNewFile(
	IDMUSProdNode* pINode		// @parm [in] Pointer to any <i IDMUSProdNode> in the file.
)
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	if( pINode == NULL )
	{
		return E_INVALIDARG;
	}

	// Handle DocRoot nodes
	if( IsDocRootNode( pINode ) )
	{
		CComponentDoc* pComponentDoc = theApp.FindComponentDocument( pINode );

		if( pComponentDoc )
		{
			if( pComponentDoc->DoSave(NULL) )
			{
				return S_OK;
			}
		}

		return E_FAIL;
	}

	HRESULT hr = E_FAIL;

	// Handle other node (i.e. Style Band nodes)
	IDataObject* pIDataObject;

	if( SUCCEEDED ( pINode->CreateDataObject( &pIDataObject ) ) )
	{
		hr = CreateFileFromDataObject( pIDataObject );

		pIDataObject->Release();
	}

	return hr;
}


/*======================================================================================
METHOD:  IDMUSPRODFRAMEWORK::ADDNODE
========================================================================================
@method HRESULT | IDMUSProdFramework | AddNode | Inserts <p pITopNode>, along with its
		children, underneath <p pIParentNode> in the Framework's Project Tree.

@comm
	If <p pIParentNode> is NULL, <p pITopNode> is inserted into the selected folder of
	the Project Tree.

	The Framework uses <om IDMUSProdNode.GetFirstChild> and <om IDMUSProdNode.GetNextChild>
	when inserting child nodes. 

@rvalue S_OK | <p pITopNode>, along with its children, was inserted successfully. 
@rvalue E_INVALIDARG | The address in <p pITopNode> is not valid.  For example, it may be NULL.
@rvalue E_FAIL | An error occurred and <p pITopNode> was not inserted into the Project Tree.

@ex The following example inserts a Style file into the active folder of the Project Tree: |

HRESULT CStyleDocType::InsertChildNode( IDMUSProdNode* pIChildNode )
{
	ASSERT( pIChildNode != NULL );
	ASSERT( m_pComponent != NULL );
	ASSERT( m_pComponent->m_pIFramework != NULL );

	// Add Style to Style component list 
	pIChildNode->AddRef();
	m_pComponent->m_lstStyles.AddTail( (CStyle *)pIChildNode );

	// Set DocRoot and parent node of ALL children
	theApp.SetNodePointers( pIChildNode, pIChildNode, (IDMUSProdNode *)this );

	// Add node to Project Tree
	if( FAILED ( m_pComponent->m_pIFramework->AddNode(pIChildNode, NULL) ) )
	{
		DeleteChildNode( pIChildNode );
		return E_FAIL;
	}

	return S_OK;
}
	
@xref <i IDMUSProdFramework>, <om IDMUSProdFramework.RemoveNode>, <i IDMUSProdNode>
--------------------------------------------------------------------------------------*/

/////////////////////////////////////////////////////////////////////////////
// CFramework::AddNode

HRESULT CFramework::AddNode(
	IDMUSProdNode* pITopNode,		// @parm [in] Pointer to the <i IDMUSProdNode> interface to be inserted.
	IDMUSProdNode* pIParentNode		// @parm [in] Pointer to the parent <i IDMUSProdNode> interface.
)
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	if( pITopNode == NULL )
	{
		return E_INVALIDARG;
	}

	HTREEITEM hTheParent = NULL;

	if( pIParentNode == NULL )
	{
		GUID guidNodeId;

		if( SUCCEEDED ( pITopNode->GetNodeId( &guidNodeId ) ) )
		{
			if( IsEqualGUID( guidNodeId, GUID_ProjectFolderNode ) )
			{
				hTheParent = TVI_ROOT;
			}
		}
	}
	else
	{
		hTheParent = FindTreeItem( pIParentNode );
	}

	// Don't worry about DocRoot nodes
	// DocRoot nodes are inserted during OnNewDocument() or OnOpenDocument()
	if( hTheParent == NULL )
	{
		IDMUSProdNode* pIDocRootNode;

		if( FAILED ( pITopNode->GetDocRootNode( &pIDocRootNode ) ) )
		{
			return E_FAIL;
		}
		if( pIDocRootNode )
		{
			if( pITopNode == pIDocRootNode )
			{
				pIDocRootNode->Release();
				return S_OK;
			}
			pIDocRootNode->Release();
		}
	}

	if( hTheParent )
	{
		if( AddNodes(pITopNode, hTheParent) )
		{
			CMainFrame* pMainFrame = (CMainFrame *)theApp.m_pMainWnd;
			ASSERT( pMainFrame != NULL );
			pMainFrame->m_wndTreeBar.SortChildren( hTheParent );

			if( theApp.m_fInPasteFromData )
			{
				ShowTreeNode( pITopNode );
				return S_OK;
			}

			if( pIParentNode )
			{
				GUID guidNodeId;

				if( SUCCEEDED ( pIParentNode->GetNodeId( &guidNodeId ) ) )
				{
					if( !(IsEqualGUID( guidNodeId, GUID_ProjectFolderNode ))
					&&  !(IsEqualGUID( guidNodeId, GUID_DirectoryNode ))
					&&  !(IsEqualGUID( guidNodeId, GUID_FileNode )) )
					{
						ShowTreeNode( pITopNode );
						return S_OK;
					}
				}
			}

			theApp.DrawProjectTree();
			return S_OK;
		}
	}

	return E_FAIL;
}


/*======================================================================================
METHOD:  IDMUSPRODFRAMEWORK::REMOVENODE
========================================================================================
@method HRESULT | IDMUSProdFramework | RemoveNode | Removes <p pITopNode>, along with its 
		children, from the Project Tree.

@rvalue S_OK | <p pITopNode>, along with its children, was removed successfully. 
@rvalue S_FALSE | User cancelled the delete.
@rvalue E_INVALIDARG | The address in <p pITopNode> is not valid.  For example, it may be NULL.
@rvalue E_FAIL | An error occurred and <p pITopNode> was not removed from the Project Tree.

@ex The following example removes a Style file from the Project Tree: |

HRESULT CStyleDocType::DeleteChildNode( IDMUSProdNode* pIChildNode )
{
	ASSERT( pIChildNode != NULL );
	ASSERT( m_pComponent != NULL );
	ASSERT( m_pComponent->m_pIFramework != NULL );

	// Remove from Project Tree
	if( m_pComponent->m_pIFramework->RemoveNode( pIChildNode, FALSE ) == S_FALSE )
	{
		return E_FAIL;
	}

	// Remove from Component Style list
	POSITION pos = m_pComponent->m_lstStyles.Find( (CStyle *)pIChildNode );
	if( pos )
	{
		m_pComponent->m_lstStyles.RemoveAt( pos );
		pIChildNode->Release();
	}

	return S_OK;
}
	
@xref <i IDMUSProdFramework>, <om IDMUSProdFramework.AddNode>, <i IDMUSProdNode>
--------------------------------------------------------------------------------------*/

/////////////////////////////////////////////////////////////////////////////
// CFramework::RemoveNode

HRESULT CFramework::RemoveNode(
	IDMUSProdNode* pITopNode,		// @parm [in] Pointer to the <i IDMUSProdNode> interface to be removed.
	BOOL fPromptUser				// @parm [in] Determines whether delete prompt is displayed.
)
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	if( pITopNode == NULL )
	{
		return E_INVALIDARG;
	}

	CTreeCtrl* pTreeCtrl = theApp.GetProjectTreeCtrl();
	HTREEITEM hTopItem = FindTreeItem( pITopNode );

	// If item is not in the Project Tree return S_OK
	if( hTopItem == NULL )
	{
		return S_OK;
	}

	// If node represents document, we want to delete its FileNode
	CComponentDoc* pComponentDoc = theApp.FindComponentDocument( pITopNode );
	if( pComponentDoc )
	{
		if( pComponentDoc->m_pIDocRootNode == pITopNode )
		{
			if( pComponentDoc->m_pFileNode )
			{
				if( fPromptUser )
				{
					if( AreYouSure( pComponentDoc->m_pFileNode ) == FALSE ) 
					{
						return S_FALSE;
					}
				}

				HRESULT hr = E_FAIL;

				// Store filename
				CString strFileName;
				pComponentDoc->m_pFileNode->ConstructFileName( strFileName );

				// Delete node
				theApp.m_fInDocRootDelete = TRUE;
				theApp.m_fDeleteFromTree = TRUE;
				if( SUCCEEDED ( pComponentDoc->m_pFileNode->DeleteNode( FALSE ) ) )
				{
					// Delete file from hard drive
					hr = theApp.DeleteFileToRecycleBin( strFileName );
					if( FAILED ( hr ) )
					{
						// Put file back in the Project Tree
						BSTR bstrFileName = strFileName.AllocSysString();
						ShowFile( bstrFileName );
					}
				}
				theApp.m_fInDocRootDelete = FALSE;
				theApp.m_fDeleteFromTree = FALSE;
				return hr;
			}
		}
	}

	if( fPromptUser )
	{
		if( AreYouSure( pITopNode ) == FALSE ) 
		{
			return S_FALSE;
		}
	}

	if( pTreeCtrl )
	{
		// See if the node we are deleting was selected
		if( pTreeCtrl->GetSelectedItem() == hTopItem )
		{
			// Highlight a sibling if at all possible
			HTREEITEM hNextItem = pTreeCtrl->GetNextItem( hTopItem, TVGN_NEXT );
			if( hNextItem == NULL )
			{
				HTREEITEM hParentItem = pTreeCtrl->GetParentItem( hTopItem );
				if( hParentItem )
				{
					hNextItem = pTreeCtrl->GetNextItem( hParentItem, TVGN_CHILD );
					if( hNextItem == hTopItem )
					{
						hNextItem = NULL;
					}
				}
			}
			if( hNextItem )
			{
				pTreeCtrl->SelectItem( hNextItem );
			}
		}
	}

	// Remove the node
	if( RemoveNodes(pITopNode) )
	{
		return S_OK;
	}

	return E_FAIL;
}


/*======================================================================================
METHOD:  IDMUSPRODFRAMEWORK::FINDDOCROOTNODE
========================================================================================
@method HRESULT | IDMUSProdFramework | FindDocRootNode | Returns an <i IDMUSProdNode> interface
		pointer for the matching DocRoot <o Node>.

@comm
	A DocRoot Node is the top Node of a file.  For more information refer to <o Node>
	Object.

	The search is confined to the <o Project> specified in <p guidProject>.

	A DocRoot Node is considered to be a match if its <o DocType> matches <p pIDocType>, 
	and its <om IDMUSProdNode::GetNodeListInfo> method returns a name and descriptor equal
	to <p bstrNodeName> and <p bstrNodeDescriptor>.

    The DocRoot Node for first file matching the criteria will be returned in <p ppIDocRootNode>.
	<om IDMUSProdFramework::GetNodeFileGuid> should be used in conjunction with
	<om IDMUSProdFramework::FindDocRootNodeByFileGUID> when it is necessary to obtain the
	DocRoot Node of a specific file.

    <p bstrNodeName> is required.  <p bstrNodeDescriptor> may be NULL.
		
@rvalue S_OK | The matching DocRoot Node was returned in <p ppIDocRootNode>. 
@rvalue E_POINTER | The address in <p ppIDocRootNode> is not valid.  For example, it may be NULL.
@rvalue E_INVALIDARG | Either <p pIDocType> or <p bstrNodeName> is not valid.  For example, they may be NULL.
@rvalue E_FAIL | An error occurred, and a DocRoot Node could not be returned.

@ex The following example: |

@xref <i IDMUSProdFramework>, <om IDMUSProdFramework.GetBestGuessDocRootNode>, <om IDMUSProdFramework::FindDocRootNodeByFileGUID>, <i IDMUSProdNode>
--------------------------------------------------------------------------------------*/

/////////////////////////////////////////////////////////////////////////////
// CFramework::FindDocRootNode

HRESULT CFramework::FindDocRootNode(
	GUID guidProject,			// @parm [in] Project GUID.
	IDMUSProdDocType* pIDocType,	// @parm [in] <o DocType> object.
	BSTR bstrNodeName,			// @parm [in] Node name.  This method frees
								//		<p bstrNodeName> with SysFreeString when it is no
								//		longer needed.
	BSTR bstrNodeDescriptor,	// @parm [in] Node descriptor.  This method frees
								//		<p bstrNodeDescriptor> with SysFreeString when it is
								//		no longer needed.
	IDMUSProdNode** ppIDocRootNode	// @parm [out,retval] Address of a variable to receive the requested  
								//		IDMUSProdNode interface.  If an error occurs, the implementation
								//		sets <p ppIDocRootNode> to NULL.  On success, the caller
								//		is responsible for calling <om IDMUSProdNode.Release>.
)
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );
	
	HRESULT hr = E_FAIL;

	CString strNodeName;
	CString strNodeDescriptor;

	if( ppIDocRootNode == NULL )
	{
		return E_POINTER;
	}

	*ppIDocRootNode = NULL;

	if( pIDocType == NULL )
	{
		return E_INVALIDARG;
	}

	// bstrNodeDescriptor is optional
	if( bstrNodeDescriptor )
	{
		strNodeDescriptor = bstrNodeDescriptor;
		::SysFreeString( bstrNodeDescriptor );
	}

	// bstrNodeName is required
	if( bstrNodeName )
	{
		strNodeName = bstrNodeName;
		::SysFreeString( bstrNodeName );
	}
	else
	{
		return E_INVALIDARG;
	}

	CFileNode* pFileNode = NULL;

	// Get Project node
	CProject* pProject = theApp.GetProjectByGUID( guidProject );
	if( pProject )
	{
		pFileNode = pProject->GetFileByText( pIDocType, strNodeName, strNodeDescriptor );

		pProject->Release();
	}

	if( pFileNode )
	{
		if( pFileNode->m_pIChildNode == NULL )
		{
			CString strFileName;
			pFileNode->ConstructFileName( strFileName );
			
			// Open the file
			theApp.m_nShowNodeInTree++;
			theApp.OpenTheFile( strFileName, TGT_FILENODE );
			theApp.m_nShowNodeInTree--;
		}

		if( pFileNode->m_pIChildNode )
		{
			pFileNode->m_pIChildNode->AddRef();
			*ppIDocRootNode = pFileNode->m_pIChildNode;
			hr = S_OK;
		}

		pFileNode->Release();
	}

	return hr;
}


/*======================================================================================
METHOD:  IDMUSPRODFRAMEWORK::FINDDOCROOTNODEBYFILEGUID
========================================================================================
@method HRESULT | IDMUSProdFramework | FindDocRootNodeByFileGUID | Returns an <i IDMUSProdNode> interface
		pointer for the matching DocRoot <o Node>.

@comm
	A DocRoot Node is the top Node of a file.  For more information refer to <o Node>
	Object.

	Producer assigns a GUID to every file in the Project Tree.  This GUID can be obtained
	via the <om IDMUSProdFramework::GetNodeFileGuid> method.  <p guidFile> refers to this 
	GUID.

	The <i IDMUSProdProject> interface provides methods to enumerate all files in a given
	Project.  The <i IDMUSProdNode> interface pointer returned by the 
	<om IDMUSProdProject::GetFirstFileByDocType> and  <om IDMUSProdProject::GetNextFileByDocType>
	methods can be passed to <om IDMUSProdFramework::GetNodeFileGuid> and used in
	conjunction with <om IDMUSProdFramework::FindDocRootNodeByFileGUID> to retrieve
	the DocRoot node for a specific file.
		
@rvalue S_OK | The matching DocRoot Node was returned in <p ppIDocRootNode>. 
@rvalue E_POINTER | The address in <p ppIDocRootNode> is not valid.  For example, it may be NULL.
@rvalue E_FAIL | An error occurred, and a DocRoot Node could not be returned.

@ex The following example retrieves the DocRoot node for the GUID returned by
	<om IDMUSProdFramework::GetNodeFileGuid>: |

	IDMUSProdFramework pIFramework;
	IDMUSProdNode* pIThisDocRootNode;
	IDMUSProdNode* pIDocRootNode;
	GUID guidFile;

	if( SUCCEEDED ( pIFramework->GetNodeFileGUID(pIThisDocRootNode, &guidFile) ) )
	{
		if( SUCCEEDED ( pIFramework->FindDocRootNodeByFileGUID(guidFile, &pIDocRootNode) ) )
		{
			ASSERT( pIDocRootNode == pIThisDocRootNode );	
		}
    }
	
@xref <i IDMUSProdFramework>, <i IDMUSProdNode>, <om IDMUSProdFramework.GetNodeFileGUID>
--------------------------------------------------------------------------------------*/

/////////////////////////////////////////////////////////////////////////////
// CFramework::FindDocRootNodeByFileGUID

HRESULT CFramework::FindDocRootNodeByFileGUID(
	GUID guidFile,				// @parm [in] File's GUID.
	IDMUSProdNode** ppIDocRootNode	// @parm [out,retval] Address of a variable to receive the requested  
								//		IDMUSProdNode interface.  If an error occurs, the implementation
								//		sets <p ppIDocRootNode> to NULL.  On success, the caller
								//		is responsible for calling <om IDMUSProdNode.Release>.
)
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );
	
	HRESULT hr = E_FAIL;

	if( ppIDocRootNode == NULL )
	{
		return E_POINTER;
	}

	*ppIDocRootNode = NULL;

	CFileNode* pFileNode = theApp.GetFileByGUID( guidFile ) ;

	if( pFileNode )
	{
		if( pFileNode->m_pIChildNode == NULL )
		{
			CString strFileName;
			pFileNode->ConstructFileName( strFileName );
			
			// Open the file
			theApp.m_nShowNodeInTree++;
			theApp.OpenTheFile( strFileName, TGT_FILENODE );
			theApp.m_nShowNodeInTree--;
		}

		if( pFileNode->m_pIChildNode )
		{
			pFileNode->m_pIChildNode->AddRef();
			*ppIDocRootNode = pFileNode->m_pIChildNode;
			hr = S_OK;
		}

		pFileNode->Release();
	}

	return hr;
}


/*======================================================================================
METHOD:  IDMUSPRODFRAMEWORK::GETBESTGUESSDOCROOTNODE
========================================================================================
@method HRESULT | IDMUSProdFramework | GetBestGuessDocRootNode | Returns an <i IDMUSProdNode>
	interface pointer for the closest matching DocRoot <o Node>.

@comm
	A DocRoot Node is the top Node of a file.  For more information refer to <o Node>
	Object.

	The search is confined to the <o Project> containing <p pITreePositionNode>.

	A DocRoot Node is considered to be a candidate for match if its <o DocType> matches
	<p pIDocType>, and its <om IDMUSProdNode::GetNodeListInfo> method returns a name equal
	to <p bstrNodeName>.

	If a matching DocRoot node cannot be located in the same directory as <p pITreePositionNode>,
	<om IDMUSProdFramework.GetBestGuessDocRootNode> searches through the entire <o Project> 
	containing <p pITreePositionNode>.  Other Projects are not searched.

	This method sets <p ppIDocRootNode> to NULL and returns S_OK when the Project Tree
	does not contain a matching node.

@rvalue S_OK | A matching DocRoot node was returned in <p ppIDocRootNode>.
@rvalue E_POINTER | The address in <p ppIDocRootNode> is not valid.  For example, it may be NULL.
@rvalue E_INVALIDARG | The address in either <p pIDocType>, <p bstrNodeName> or
		<p pITreePositionNode> is not valid.  For example, it may be NULL.
@rvalue E_PENDING | A matching DocRoot node is in the process of loading and could not be returned.
@rvalue E_FAIL | An error occurred, and a matching node could not be returned.

@ex The following example:|

@xref <i IDMUSProdFramework>, <om IDMUSProdFramework.FindDocRootNode>, <i IDMUSProdNode>
--------------------------------------------------------------------------------------*/

/////////////////////////////////////////////////////////////////////////////
// CFramework::GetBestGuessDocRootNode

HRESULT CFramework::GetBestGuessDocRootNode(
	IDMUSProdDocType* pIDocType,	// @parm [in] Pointer to the <i IDMUSProdDocType> interface
									//		associated with the desired type of DocRoot node.
									//		Used by search algorithm.
	BSTR bstrNodeName,				// @parm [in] Name of desired DocRoot node.  Used by search
									//		algorithm.  This method frees <p bstrNodeName> with
									//		SysFreeString when it is no longer needed.
	IDMUSProdNode* pITreePositionNode,	// @parm [in] Specifies position in Project Tree.  Used by search
									//		algorithm.
	IDMUSProdNode** ppIDocRootNode	// @parm [out,retval] Address of a variable to receive the requested  
									//		<i IDMUSProdNode> interface.  If an error occurs, the implementation
									//		sets <p ppIDocRootNode> to NULL.  On success, the caller
									//		is responsible for calling <om IDMUSProdNode.Release> when
									//		this pointer is no longer needed.
)
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	CString strNodeName;

	// Validate ppIDocRootNode
	ASSERT( ppIDocRootNode != NULL );
	if( ppIDocRootNode == NULL )
	{
		return E_POINTER;
	}

	*ppIDocRootNode = NULL;

	// Validate bstrNodeName
	if( bstrNodeName == NULL )
	{
		return E_INVALIDARG;
	}
	else
	{
		strNodeName = bstrNodeName;
		::SysFreeString( bstrNodeName );
	}

	// Validate pIDocType and pITreePositionNode
	if( pIDocType == NULL
	||  pITreePositionNode == NULL )
	{
		return E_INVALIDARG;
	}

	HRESULT hr = E_FAIL;

	// Does DocRoot exist in a container that is just now being unpacked?
	IDMUSProdComponent* pIComponent;
	if( SUCCEEDED ( FindComponent( CLSID_ContainerComponent,  &pIComponent ) ) )
	{
		IDMUSProdUnpackingFiles* pIUnpackingFiles;
		if( SUCCEEDED ( pIComponent->QueryInterface( IID_IDMUSProdUnpackingFiles, (void **)&pIUnpackingFiles ) ) )
		{
			BSTR bstrName = strNodeName.AllocSysString();

			IUnknown* pIUnknown;
			hr = pIUnpackingFiles->GetDocRootOfEmbeddedFile( pIDocType, bstrName, &pIUnknown );
			if( SUCCEEDED ( hr ) )
			{
				if( FAILED ( pIUnknown->QueryInterface( IID_IDMUSProdNode, (void **)ppIDocRootNode ) ) )
				{
					hr = E_FAIL;
				}
				
				pIUnknown->Release();
			}

			pIUnpackingFiles->Release();
		}

		pIComponent->Release();
	}
	if( SUCCEEDED(hr)
	||  hr == E_PENDING )
	{
		// The file is in a Container that is being unpacked, exit now
		return hr;
	}

	// Does DocRoot exist in a Song that is just now being unpacked?
	/*
	if( SUCCEEDED ( FindComponent( CLSID_SongComponent,  &pIComponent ) ) )
	{
		IDMUSProdUnpackingFiles* pIUnpackingFiles;
		if( SUCCEEDED ( pIComponent->QueryInterface( IID_IDMUSProdUnpackingFiles, (void **)&pIUnpackingFiles ) ) )
		{
			BSTR bstrName = strNodeName.AllocSysString();

			IUnknown* pIUnknown;
			hr = pIUnpackingFiles->GetDocRootOfEmbeddedFile( pIDocType, bstrName, &pIUnknown );
			if( SUCCEEDED ( hr ) )
			{
				if( FAILED ( pIUnknown->QueryInterface( IID_IDMUSProdNode, (void **)ppIDocRootNode ) ) )
				{
					hr = E_FAIL;
				}
				
				pIUnknown->Release();
			}

			pIUnpackingFiles->Release();
		}

		pIComponent->Release();
	}
	if( SUCCEEDED(hr)
	||  hr == E_PENDING )
	{
		// The file is in a Song that is being unpacked, exit now
		return hr;
	}
	*/

	// Does DocRoot exist in list if files waiting to be opened?
	POSITION pos = theApp.m_lstFileNamesToOpen.GetHeadPosition();
	while( pos )
	{
		CJzFileName* pJzFileName = theApp.m_lstFileNamesToOpen.GetNext( pos );

		if( pJzFileName->m_fBeingLoaded == true )
		{
			// Must handle this case further down (via it's existing CFileNode)
			continue;
		}

		IDMUSProdDocType* pIDocTypeList = theApp.GetDocType( pJzFileName->m_strFileName );
		if( pIDocTypeList )
		{
			if( pIDocTypeList == pIDocType )
			{
				if( strNodeName.Compare(pJzFileName->m_strObjectName) == 0 )
				{
					// Open the file
					theApp.m_nShowNodeInTree++;
					pJzFileName->m_fBeingLoaded = true;
					theApp.OpenDocumentFile( pJzFileName->m_strFileName );
					theApp.m_nShowNodeInTree--;

					// Find the DocRoot
					CFileNode* pFileNode = theApp.FindFileNode( pJzFileName->m_strFileName );
					if( pFileNode )
					{
						if( pFileNode->m_pIChildNode )
						{
							pFileNode->m_pIChildNode->AddRef();
							*ppIDocRootNode = pFileNode->m_pIChildNode;
							hr = S_OK;
						}

						pFileNode->Release();
					}

					pIDocTypeList->Release();
					break;
				}
			}

			pIDocTypeList->Release();
		}
	}
	if( SUCCEEDED(hr) )
	{
		// The file is in the list of files waiting to be opened
		return hr;
	}

	// The file is not in a Container, a Song or the list of file waiting to be opened
	hr = S_OK;

	// Get the Project
	IDMUSProdProject* pIProject;
	if( SUCCEEDED ( theApp.m_pFramework->FindProject( pITreePositionNode, &pIProject ) ) )
	{
		CProject* pProject = (CProject *)pIProject;

		CFileNode* pFileNode = NULL;

		// Look for an appropriate FileNode
		pFileNode = pProject->GetBestGuessDocRootNode( pIDocType, strNodeName, pITreePositionNode );

		if( pFileNode )
		{
			// Make sure the FileNode's file is loaded
			if( pFileNode->m_pIChildNode == NULL )
			{
				if( pFileNode->m_fInOnOpenDocument )
				{
					hr = E_PENDING;
				}
				else
				{
					CString strFileName;
					pFileNode->ConstructFileName( strFileName );
					
					// Open the file
					theApp.m_nShowNodeInTree++;
					theApp.OpenTheFile( strFileName, TGT_FILENODE );
					theApp.m_nShowNodeInTree--;
				}
			}

			if( pFileNode->m_pIChildNode )
			{
				pFileNode->m_pIChildNode->AddRef();
				*ppIDocRootNode = pFileNode->m_pIChildNode;
			}

			pFileNode->Release();
		}

		pIProject->Release();
	}

	return hr;
}


/*======================================================================================
METHOD:  IDMUSPRODFRAMEWORK::GETSELECTEDNODE
========================================================================================
@method HRESULT | IDMUSProdFramework | GetSelectedNode | Returns the Project Tree's currently
	selected <o Node>.

@comm
	<om IDMUSProdFramework.GetSelectedNode> sets <p ppINode> to NULL and returns S_OK when 
	either the Project Tree is empty or none of its nodes are selected.

@rvalue S_OK | The Project Tree's selected node was returned in <p ppINode>.
@rvalue E_POINTER | The address in <p ppINode> is not valid.  For example, it may be NULL.
@rvalue E_FAIL | An error occurred, and the selected node could not be returned.

@ex The following example obtains the active node of the Project Tree:|

	IDMUSProdNode* pINode;

	if( SUCCEEDED ( pIFramework->GetSelectedNode( &pINode ) ) )
	{
		if( pINode )
		{
			...
			...
			...
			pINode->Release();
		}
	}

@xref <i IDMUSProdFramework>
--------------------------------------------------------------------------------------*/

/////////////////////////////////////////////////////////////////////////////
// CFramework::GetSelectedNode

HRESULT CFramework::GetSelectedNode(
	IDMUSProdNode** ppINode	// @parm [out,retval] Address of a variable to receive the requested  
							//		<i IDMUSProdNode> interface.  If an error occurs, the implementation
							//		sets <p ppINode> to NULL.  On success, the caller
							//		is responsible for calling <om IDMUSProdNode.Release>
							//		when this pointer is no longer needed.
)
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	if( ppINode == NULL )
	{
		return E_POINTER;
	}

	*ppINode = NULL;

	CTreeCtrl* pTreeCtrl = theApp.GetProjectTreeCtrl();
	if( pTreeCtrl )
	{
		HTREEITEM hItem = pTreeCtrl->GetSelectedItem();
		if( hItem )
		{
			CJzNode* pJzNode = (CJzNode *)pTreeCtrl->GetItemData( hItem );
			if( pJzNode )
			{
				pJzNode->m_pINode->AddRef();
				*ppINode = pJzNode->m_pINode;
			}
		}

		return S_OK;
	}

	return E_FAIL;
}


/////////////////////////////////////////////////////////////////////////////
// CFramework::SyncEditorTitles

void CFramework::SyncEditorTitles( IDMUSProdNode* pINode )
{
	HWND hWndEditor;
	CWnd* pWndEditor;

	ASSERT( pINode != NULL );

	CComponentDoc * pComponentDoc = (CComponentDoc *)theApp.FindDocument( pINode );
	if( pComponentDoc )
	{
		ASSERT_VALID( pComponentDoc );

		if( pComponentDoc->IsKindOf( RUNTIME_CLASS(CComponentDoc) ) )
		{
			POSITION pos = pComponentDoc->GetFirstViewPosition();
			while( pos )
			{
				CComponentView * pComponentView = (CComponentView *)pComponentDoc->GetNextView( pos );
				ASSERT_VALID( pComponentView );

				if( pComponentView->m_pINode )
				{
					if( SUCCEEDED ( pComponentView->m_pINode->GetEditorWindow(&hWndEditor) ) )
					{
						if( hWndEditor )
						{
							pWndEditor = CWnd::FromHandlePermanent( hWndEditor );
							if( pWndEditor )
							{
								BSTR bstrTitle;

								pComponentView->m_pINode->GetEditorTitle( &bstrTitle );
								pWndEditor->SetWindowText(  CString(bstrTitle) );
								::SysFreeString( bstrTitle );
							}
						}
					}
				}
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CFramework::SyncNodeName

HRESULT CFramework::SyncNodeName( IDMUSProdNode* pINode, BOOL fRedraw )
{
	ASSERT( pINode != NULL );
	if( pINode == NULL )
	{
		return E_INVALIDARG;
	}

	BSTR bstrNodeName;
	CString strNodeName;

	if( SUCCEEDED ( pINode->GetNodeName(&bstrNodeName) ) )
	{
		strNodeName = bstrNodeName;
		::SysFreeString( bstrNodeName );
	}

	// Update Project Tree
	HTREEITEM hItem = FindTreeItem( pINode );
	if( hItem )
	{
		CTreeCtrl* pTreeCtrl = theApp.GetProjectTreeCtrl();
		if( pTreeCtrl )
		{
			// Update label in the Project Tree
			pTreeCtrl->SetItemText( hItem, strNodeName );
			if( fRedraw )
			{
				pTreeCtrl->UpdateWindow();
			}
		}
	}

	// Set Document Title
	CCommonDoc* pCommonDoc = theApp.FindDocument( pINode );
	if( pCommonDoc
	&&  pCommonDoc->m_pIDocRootNode == pINode )
	{
		CString strPathName = pCommonDoc->GetPathName();
		if( strPathName.IsEmpty() )
		{
			pCommonDoc->SetTitle( strNodeName );
		}
	}

	// Set Window Titles
	SyncEditorTitles( pINode );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CFramework::SyncNodeIcon

HRESULT CFramework::SyncNodeIcon( IDMUSProdNode* pINode, BOOL fRedraw )
{
	ASSERT( pINode != NULL );
	if( pINode == NULL )
	{
		return E_INVALIDARG;
	}

	short nImage;
    if( SUCCEEDED ( pINode->GetNodeImageIndex( &nImage ) ) )
	{
		// Find node in Project Tree
		HTREEITEM hItem = FindTreeItem( pINode );
		if( hItem )
		{
			CTreeCtrl* pTreeCtrl = theApp.GetProjectTreeCtrl();
			if( pTreeCtrl )
			{
				BOOL fUseOpenCloseImages = FALSE;

				// Update icon in the Project Tree
				pINode->UseOpenCloseImages( &fUseOpenCloseImages );
				if( fUseOpenCloseImages == TRUE )
				{
					pTreeCtrl->SetItemImage( hItem, nImage, nImage );
				}
				else
				{
					pTreeCtrl->SetItemImage( hItem, nImage, nImage + 1 );
				}

				if( fRedraw )
				{
					pTreeCtrl->UpdateWindow();
				}
			}
		}
	}

	return S_OK;
}


/*======================================================================================
METHOD:  IDMUSPRODFRAMEWORK::SETSELECTEDNODE
========================================================================================
@method HRESULT | IDMUSProdFramework | SetSelectedNode | Highlights the Project Tree node
		associated with <p pINode>.

@comm
	The Project Tree does not scroll to make sure that the selected item is visible.

	<om IDMUSProdNode.OnNodeSelChanged> gets invoked after calling this method.

@rvalue S_OK | <p pINode> was highlighted successfully. 
@rvalue E_INVALIDARG | The address in <p pINode> is not valid.  For example, it may be NULL.
@rvalue E_FAIL | An error occurred and <p pINode> could not be highlighted.

@xref <i IDMUSProdFramework>, <i IDMUSProdNode>
--------------------------------------------------------------------------------------*/

/////////////////////////////////////////////////////////////////////////////
// CFramework::SetSelectedNode

HRESULT CFramework::SetSelectedNode(
	IDMUSProdNode* pINode		// @parm [in] Pointer to an <i IDMUSProdNode> interface.
)
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	if( pINode == NULL )
	{
		return E_INVALIDARG;
	}

	if( ShowTreeNode( pINode ) )
	{
		return S_OK;
	}

	return E_FAIL;
}


/*======================================================================================
METHOD:  IDMUSPRODFRAMEWORK::REFRESHNODE
========================================================================================
@method HRESULT | IDMUSProdFramework | RefreshNode | Refreshes and redraws the Project Tree
	node associated with <p pINode>.

@comm
	This method calls <om IDMUSProdNode.GetNodeName> before updating the Project Tree with
	new label text for <p pINode>.

	This method calls <om IDMUSProdNode.GetNodeImageIndex> before updating the Project Tree
	with a new icon for <p pINode>.

@rvalue S_OK | <p pINode> was refreshed successfully. 
@rvalue E_INVALIDARG | The address in <p pINode> is not valid.  For example, it may be NULL.
@rvalue E_FAIL | An error occurred and <p pINode> could not be refreshed.

@xref <i IDMUSProdFramework>, <i IDMUSProdNode>
--------------------------------------------------------------------------------------*/

/////////////////////////////////////////////////////////////////////////////
// CFramework::RefreshNode

HRESULT CFramework::RefreshNode(
	IDMUSProdNode* pINode		// @parm [in] Pointer to an <i IDMUSProdNode> interface.
)
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	if( pINode == NULL )
	{
		return E_INVALIDARG;
	}

	SyncNodeName( pINode, FALSE );
	SyncNodeIcon( pINode, TRUE );

	return S_OK;
}


/*======================================================================================
METHOD:  IDMUSPRODFRAMEWORK::SORTCHILDNODES
========================================================================================
@method HRESULT | IDMUSProdFramework | SortChildNodes | Sorts the Project Tree child
		<o Node>s associated with <p pINode>.


@rvalue S_OK | The child nodes of <p pINode> were sorted successfully. 
@rvalue E_INVALIDARG | The address in <p pINode> is not valid.  For example, it may be NULL.
@rvalue E_FAIL | An error occurred and the Project Tree could not be sorted.

@xref <i IDMUSProdFramework>, <i IDMUSProdNode>
--------------------------------------------------------------------------------------*/

/////////////////////////////////////////////////////////////////////////////
// CFramework::SortChildNodes

HRESULT CFramework::SortChildNodes(
	IDMUSProdNode* pINode		// @parm [in] Pointer to an <i IDMUSProdNode> interface.
)
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	if( pINode == NULL )
	{
		return E_INVALIDARG;
	}

	HTREEITEM hItem = FindTreeItem( pINode );
	if( hItem )
	{
		CMainFrame* pMainFrame = (CMainFrame *)theApp.m_pMainWnd;
		ASSERT( pMainFrame != NULL );
		pMainFrame->m_wndTreeBar.SortChildren( hItem );
		return S_OK;
	}

	return E_FAIL;
}


/*======================================================================================
METHOD:  IDMUSPRODFRAMEWORK::EDITNODELABEL
========================================================================================
@method HRESULT | IDMUSProdFramework | EditNodeLabel | Begins in-place editing of the label
		associated with <p pINode>.

@comm
	This method provides the means for a Component to initiate in-place editing of the
	node names in the Project Tree.  For example, it allows the right-click context menu
	associated with a node to contain a menu item for renaming itself.

@rvalue S_OK | In-place editing of the node's label began successfully. 
@rvalue E_INVALIDARG | The address in <p pINode> is not valid.  For example, it may be NULL.
@rvalue E_FAIL | In-place editing of the node's label could not begin.

@ex The following example initiates in-place editing of a node's label: |

HRESULT CMotif::OnRightClickMenuSelect( long lCommandId )
{
	ASSERT( m_pComponent != NULL );
	ASSERT( m_pComponent->m_pIFramework != NULL );

	HRESULT hr = E_FAIL;

	switch( lCommandId )
	{
		case IDM_RENAME:
			if( SUCCEEDED ( m_pComponent->m_pIFramework->EditNodeLabel((IDMUSProdNode *)this) ) )
			{
				hr = S_OK;
			}
			break;
	}

	return hr;
}
	
@xref <i IDMUSProdFramework>, <i IDMUSProdNode>, <om IDMUSProdNode.GetNodeNameMaxLength>, <om IDMUSProdNode.ValidateNodeName>, <om IDMUSProdNode.SetNodeName>
--------------------------------------------------------------------------------------*/

/////////////////////////////////////////////////////////////////////////////
// CFramework::EditNodeLabel

HRESULT CFramework::EditNodeLabel(
	IDMUSProdNode* pINode		// @parm [in] Pointer to the <i IDMUSProdNode> interface.
)
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	if( pINode == NULL )
	{
		return E_INVALIDARG;
	}

	HTREEITEM hItem = FindTreeItem( pINode );

	if( hItem )
	{
		CTreeCtrl* pTreeCtrl = theApp.GetProjectTreeCtrl();
		if( pTreeCtrl )
		{
			pTreeCtrl->EnsureVisible( hItem );
			pTreeCtrl->SelectItem( hItem );
			if( pTreeCtrl->EditLabel(hItem) )
			{
				return S_OK;
			}
		}
	}

	return E_FAIL;
}


/*======================================================================================
METHOD:  IDMUSPRODFRAMEWORK::OPENEDITOR
========================================================================================
@method HRESULT | IDMUSProdFramework | OpenEditor | Opens an editor for the node specified
		in <p pINode>.

@comm
	This method provides the means for a Component to initiate in-place editing for a
	node in the Project Tree.  

@rvalue S_OK | An editor was successfully opened for <p pINode>. 
@rvalue E_INVALIDARG | The address in <p pINode> is not valid.  For example, it may be NULL.
@rvalue E_FAIL | An error occurred, and the editor could not be opened.

@ex The following example opens an editor for <p pINode>: |

@xref <o Editor> Object, <i IDMUSProdFramework>, <om IDMUSProdFramework.CloseEditor>, <i IDMUSProdNode>, <i IDMUSProdEditor>
--------------------------------------------------------------------------------------*/

/////////////////////////////////////////////////////////////////////////////
// CFramework::OpenEditor

HRESULT CFramework::OpenEditor(
	IDMUSProdNode* pINode		// @parm [in] Pointer to the <i IDMUSProdNode> interface.
)
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	if( pINode == NULL )
	{
		return E_INVALIDARG;
	}

	HRESULT hr = E_FAIL;

	// If reference node, use document of referenced file
	IDMUSProdReferenceNode* pIReferenceNode;
	if( SUCCEEDED ( pINode->QueryInterface( IID_IDMUSProdReferenceNode, (void **)&pIReferenceNode ) ) )
	{
		IDMUSProdNode* pIDocRootNode;
		if( SUCCEEDED ( pIReferenceNode->GetReferencedFile( &pIDocRootNode ) ) )
		{
			if( pIDocRootNode )
			{
				CComponentDoc* pComponentDoc = theApp.FindComponentDocument( pIDocRootNode );
				if( pComponentDoc )
				{
					pComponentDoc->OpenEditorWindow( pIDocRootNode );
					hr = S_OK;
				}

				pIDocRootNode->Release();
			}
		}

		pIReferenceNode->Release();
	}
	else
	{
		CComponentDoc* pComponentDoc = theApp.FindComponentDocument( pINode );
		if( pComponentDoc )
		{
			pComponentDoc->OpenEditorWindow( pINode );
			hr = S_OK;
		}
	}

	return hr;
}


/*======================================================================================
METHOD:  IDMUSPRODFRAMEWORK::CLOSEEDITOR
========================================================================================
@method HRESULT | IDMUSProdFramework | CloseEditor | Closes the editor for the node
		specified in <p pINode>.

@comm
	This method provides the means for a Component to close an editor opened by 
	the <om IDMUSProdFramework.OpenEditor> method.

@rvalue S_OK | <p pINode>'s editor was closed successfully. 
@rvalue E_INVALIDARG | The address in <p pINode> is not valid.  For example, it may be NULL.
@rvalue E_FAIL | An error occurred, and the editor could not be closed.

@ex The following example closes the editor for <p pINode>: |

@xref <o Editor> Object, <i IDMUSProdFramework>, <om IDMUSProdFramework.OpenEditor>, <i IDMUSProdNode>, <i IDMUSProdEditor>
--------------------------------------------------------------------------------------*/

/////////////////////////////////////////////////////////////////////////////
// CFramework::CloseEditor

HRESULT CFramework::CloseEditor(
	IDMUSProdNode* pINode		// @parm [in] Pointer to the <i IDMUSProdNode> interface.
)
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	if( pINode == NULL )
	{
		return E_INVALIDARG;
	}

	CDocTemplate* pTemplate;
	CComponentDoc* pDocument;

	// no doc manager - no templates
	if( theApp.m_pDocManager )
	{
		// walk all templates in the application
		POSITION pos = theApp.m_pDocManager->GetFirstDocTemplatePosition();
		while( pos )
		{
			pTemplate = theApp.m_pDocManager->GetNextDocTemplate( pos );
			ASSERT_VALID( pTemplate );
			ASSERT_KINDOF( CDocTemplate, pTemplate );

			// walk all documents in the template
			POSITION pos2 = pTemplate->GetFirstDocPosition();
			while( pos2 )
			{
				pDocument = (CComponentDoc *)pTemplate->GetNextDoc( pos2 );
				ASSERT_VALID( pDocument );

				if( pDocument->IsKindOf( RUNTIME_CLASS(CComponentDoc) ) )
				{
					if( pDocument->CloseViewsByNode( pINode ) )
					{
						pos = NULL;
						pos2 = NULL;
					}
				}
			}
		}
	}

	return S_OK;
}


/*======================================================================================
METHOD:  IDMUSPRODFRAMEWORK::ADDMENUITEM
========================================================================================
@method HRESULT | IDMUSProdFramework | AddMenuItem | Adds the <o Menu> object defined by
		<p pIMenu> to the Framework's 'Add-Ins' menu. 

@comm
	<om IDMUSProdFramework.AddMenuItem> provides a way for a Component, that otherwise
	has no UI, to hook into the Framework.  For example, a Component may want to place a
	menu item in the Framework's 'Add-Ins' menu to provide a dialog that can be used to setup
	various options.

	A Component's <om IDMUSProdComponent.Initialize> method should call
	<om IDMUSProdFramework.AddMenuItem> if it wants to add menu item(s) to the Framework's
	'Add-Ins' menu.  <om IDMUSProdFramework.RemoveMenuItem> should be called from within
	<om IDMUSProdComponent.CleanUp>.

@rvalue S_OK | <p pIMenu> was successfully added to the Framework's 'Add-Ins' menu. 
@rvalue E_INVALIDARG | <p pIMenu> is not valid.  For example, it may be NULL.
@rvalue E_FAIL | <p pIMenu> was not added to the 'Add-Ins' menu. 
@rvalue E_OUTOFMEMORY | Out of memory. 

@ex The following excerpt from an <om IDMUSProdComponent.Initialize> method adds an
	item to the Framework's 'Add-Ins' menu: |

	if( FAILED ( m_pIFramework->AddMenuItem( (IDMUSProdMenu *)this ) ) )
	{
		CleanUp();
		::LoadString( theApp.m_hInstance, IDS_ERR_ADD_MENUITEM, achErrMsg, MID_BUFFER );
		strErrMsg = achErrMsg;
		*pbstrErrMsg = strErrMsg.AllocSysString();
		return E_FAIL;
	}
	else
	{
		m_fMenuWasAdded = TRUE;
	}
	
@xref <o Menu> Object, <i IDMUSProdFramework>, <om IDMUSProdFramework.RemoveMenuItem>, <i IDMUSProdMenu>
--------------------------------------------------------------------------------------*/

/////////////////////////////////////////////////////////////////////////////
// CFramework::AddMenuItem

HRESULT CFramework::AddMenuItem(
	IDMUSProdMenu* pIMenu		// @parm [in] Pointer to the <i IDMUSProdMenu> interface to be added.
)
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	if( pIMenu == NULL )
	{
		return E_INVALIDARG;
	}

	CMenu* pAddInsMenu = theApp.FindMenuByName( IDS_ADDINS_MENU_TEXT );
	if( pAddInsMenu == NULL )
	{
		return E_FAIL;
	}
	
	CJzMenu* pJzMenu = new CJzMenu( pIMenu, ++m_nNextMenuID ); 
	if( pJzMenu == NULL )
	{
		return E_OUTOFMEMORY;
	}

	CString strMenuText;
	BSTR bstrMenuText;

	pJzMenu->m_pIMenu->GetMenuText( &bstrMenuText );
	strMenuText = bstrMenuText;
	::SysFreeString( bstrMenuText );

	pAddInsMenu->InsertMenu( 0, MF_BYPOSITION | MF_STRING, pJzMenu->m_nCommandID, strMenuText ); 

	m_lstMenus.AddTail( pJzMenu );

	return S_OK;
}


/*======================================================================================
METHOD:  IDMUSPRODFRAMEWORK::REMOVEMENUITEM
========================================================================================
@method HRESULT | IDMUSProdFramework | RemoveMenuItem | Removes the menu item defined
		by <p pIMenu> from the Framework's 'Add-Ins' menu. 

@comm
	A Component's <om IDMUSProdComponent.Initialize> method should call
	<om IDMUSProdFramework.AddMenuItem> if it wants to add menu item(s) to the Framework's
	'Add-Ins' menu.  <om IDMUSProdFramework.RemoveMenuItem> should be called from within
	<om IDMUSProdComponent.CleanUp>.

@rvalue S_OK | <p pIMenu> was successfully removed from the Framework's 'Add-Ins' menu. 
@rvalue E_INVALIDARG | <p pIMenu> is not valid.  For example, it may be NULL.
@rvalue E_FAIL | <p pIMenu> was not removed from the 'Add-Ins' menu. 

@ex The following excerpt from an <om IDMUSProdComponent.Cleanup> method removes an
	item from the Framework's 'Add-Ins' menu: |

	if( m_fMenuWasAdded )
	{
		m_pIFramework->RemoveMenuItem( (IDMUSProdMenu *)this ); 
		m_fMenuWasAdded = FALSE;
	}
	
@xref <i IDMUSProdFramework>, <om IDMUSProdFramework.AddMenuItem>, <i IDMUSProdMenu>
--------------------------------------------------------------------------------------*/

/////////////////////////////////////////////////////////////////////////////
// CFramework::RemoveMenuItem

HRESULT CFramework::RemoveMenuItem(
	IDMUSProdMenu* pIMenu		// @parm [in] Pointer to the <i IDMUSProdMenu> interface to be removed.

)
{
	CJzMenu* pJzMenu;

	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	if( pIMenu == NULL )
	{
		return E_INVALIDARG;
	}

	POSITION pos = m_lstMenus.GetHeadPosition();

    while( pos != NULL )
    {
        pJzMenu = static_cast<CJzMenu*>( m_lstMenus.GetNext(pos) );
		if( pJzMenu )
		{
			if( pJzMenu->m_pIMenu == pIMenu )
			{
				pos = m_lstMenus.Find( pJzMenu );
				if( pos )
				{
					m_lstMenus.RemoveAt( pos );
				}

				CMenu* pAddInsMenu = theApp.FindMenuByName( IDS_ADDINS_MENU_TEXT );
				if( pAddInsMenu )
				{
					pAddInsMenu->RemoveMenu( pJzMenu->m_nCommandID, MF_BYCOMMAND );
				}

				delete pJzMenu;
				return S_OK;
			}
		}
    }

	return E_FAIL;
}


/*======================================================================================
METHOD:  IDMUSPRODFRAMEWORK::ADDTOOLBAR
========================================================================================
@method HRESULT | IDMUSProdFramework | AddToolBar| Adds the <o ToolBar> object defined by
		<p pIToolBar> to the Framework.

@comm
	<om IDMUSProdFramework.AddToolBar> provides a way for a <o Component> to place a toolbar in
	the Framework so that it can be accessed by other Components.  For example, a Component
	may want to provide transport controls for other DirectMusic Producer Components. 

	A Component's <om IDMUSProdComponent.Initialize> method should call
	<om IDMUSProdFramework.AddToolBar> if it wants to add toolbar(s) to the Framework.
	<om IDMUSProdFramework.RemoveToolBar> should be called from within
	<om IDMUSProdComponent.CleanUp>.

@rvalue S_OK | <p pIToolBar> was successfully added to the Framework. 
@rvalue E_INVALIDARG | <p pIToolBar> is not valid.  For example, it may be NULL.
@rvalue E_FAIL | <p pIToolBar> was not added to the Framework. 
@rvalue E_OUTOFMEMORY | Out of memory. 

@ex The following excerpt from an <om IDMUSProdComponent.Initialize> method adds a toolbar
	to the Framework: |

	CWnd* pWndParent = new CWnd;
	if( pWndParent )
	{
		if( pWndParent->CreateEx(0, AfxRegisterWndClass(0), NULL, WS_OVERLAPPED, 0, 0, 0, 0, NULL, NULL) )
		{
			CToolbarMsgHandler* pBarMsgHandler = new CToolbarMsgHandler;
			if( pBarMsgHandler )
			{
				CRect rect( 0, 0, 0, 0 );
				if( (pBarMsgHandler->Create(AfxRegisterWndClass(0), NULL, WS_CHILD, rect, pWndParent, -1))
				&&  (SUCCEEDED(pIFramework->AddToolBar(pBarMsgHandler))) )
				{
					m_pBarMsgHandler = pBarMsgHandler;
				}
				else
				{
					if( pBarMsgHandler->GetSafeHwnd() )
					{
						pBarMsgHandler->DestroyWindow();
					}
					delete pBarMsgHandler;
				}
			}
		}

		//  IDMUSProdFramework::AddToolBar reassigns parent so it is ok to destroy pWndParent
		if( pWndParent->GetSafeHwnd() )
		{
			pWndParent->DestroyWindow();
			delete pWndParent;
		}
	}
	
@xref <o ToolBar> Object, <i IDMUSProdFramework>, <om IDMUSProdFramework.RemoveToolBar>, <i IDMUSProdToolBar>
--------------------------------------------------------------------------------------*/

/////////////////////////////////////////////////////////////////////////////
// CFramework::AddToolBar

HRESULT CFramework::AddToolBar(
	IDMUSProdToolBar* pIToolBar		// @parm [in] Pointer to the <i IDMUSProdToolBar> interface to be added.
)
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	if( pIToolBar == NULL )
	{
		return E_INVALIDARG;
	}

	CJzToolBar* pJzToolBar = new CJzToolBar; 
	if( pJzToolBar == NULL )
	{
		return E_OUTOFMEMORY;
	}

	pJzToolBar->m_pClientToolBar = new CClientToolBar( pIToolBar ); 
	if( pJzToolBar->m_pClientToolBar == NULL )
	{
		delete pJzToolBar;
		return E_OUTOFMEMORY;
	}

	HRESULT hr = E_FAIL;

	HWND hWndOwner;
	HINSTANCE hInstance;
	UINT nResourceId = -1;
	BSTR bstrTitle = NULL;
	CString strTitle;
	
	if( SUCCEEDED ( pIToolBar->GetInfo( &hWndOwner, &hInstance, &nResourceId, &bstrTitle ) ) )
	{
		ASSERT( hInstance != NULL );
		ASSERT( hWndOwner != NULL );
		ASSERT( GetWindowLong(hWndOwner, GWL_STYLE) & WS_CHILD );

		if( (hInstance != NULL)
		&&  (hWndOwner != NULL)
		&&  (GetWindowLong(hWndOwner, GWL_STYLE) & WS_CHILD) )
		{
			HINSTANCE hInstanceOld; 

			CMainFrame* pMainFrame = (CMainFrame *)theApp.m_pMainWnd;
			ASSERT( pMainFrame != NULL );

			if( bstrTitle )
			{
				strTitle = bstrTitle;
				::SysFreeString( bstrTitle );
			}

			pJzToolBar->m_pClientToolBar->m_hInstance = hInstance;
			pJzToolBar->m_nControlID = m_nNextToolBarID++;

			hInstanceOld = AfxGetResourceHandle();
			AfxSetResourceHandle( pJzToolBar->m_pClientToolBar->m_hInstance );

			if( pJzToolBar->m_pClientToolBar->Create(pMainFrame, WS_CHILD | WS_VISIBLE | CBRS_TOP, pJzToolBar->m_nControlID) )
			{
				if( nResourceId != -1 )
				{
					pJzToolBar->m_pClientToolBar->LoadToolBar( nResourceId );
				}
				pJzToolBar->m_pClientToolBar->ModifyStyle( 0, (WS_CLIPCHILDREN | TBSTYLE_FLAT), 0 );
				pJzToolBar->m_pClientToolBar->SetBarStyle( pJzToolBar->m_pClientToolBar->GetBarStyle() |
														   CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC );
//														   CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_FIXED );
				pJzToolBar->m_pClientToolBar->SetWindowText( strTitle );

				pIToolBar->Initialize( pJzToolBar->m_pClientToolBar->GetSafeHwnd() );

				::SetParent( hWndOwner, pMainFrame->GetSafeHwnd() );

				pJzToolBar->m_pClientToolBar->SendMessage( TB_SETPARENT, (WPARAM)hWndOwner, 0 );
				pJzToolBar->m_pClientToolBar->m_hWndOwner = hWndOwner;

				pJzToolBar->m_pClientToolBar->EnableDocking( CBRS_ALIGN_TOP | CBRS_ALIGN_BOTTOM );
				pMainFrame->DockControlBar( pJzToolBar->m_pClientToolBar );

				InsertViewMenuItem( pJzToolBar ); 
				m_lstToolBars.AddTail( pJzToolBar );

				hr = S_OK;
			}

			AfxSetResourceHandle( hInstanceOld );
		}
	}

	if( FAILED(hr) )
	{
		delete pJzToolBar;
	}

	return hr;
}


/*======================================================================================
METHOD:  IDMUSPRODFRAMEWORK::REMOVETOOLBAR
========================================================================================
@method HRESULT | IDMUSProdFramework | RemoveToolBar | Removes the <o ToolBar> defined by
		<p pIToolBar> from the Framework.

@comm
	A Component's <om IDMUSProdComponent.Initialize> method should call
	<om IDMUSProdFramework.AddToolBar> if it wants to add toolbar(s) to the Framework.
	<om IDMUSProdFramework.RemoveToolBar> should be called from within
	<om IDMUSProdComponent.CleanUp>.

@rvalue S_OK | <p pIToolBar> was successfully removed from the Framework.
@rvalue E_INVALIDARG | <p pIToolBar> is not valid.  For example, it may be NULL.
@rvalue E_FAIL | <p pIToolBar> was not removed from the Framework. 

@ex The following excerpt from an <om IDMUSProdComponent.Cleanup> method removes a toolbar
	from the Framework: |

	if( m_pBarMsgHandler )
	{
		m_pIFramework->RemoveToolBar( m_pBarMsgHandler );
		if( m_pBarMsgHandler->GetSafeHwnd() )
		{
			m_pBarMsgHandler->DestroyWindow();
		}
		delete m_pBarMsgHandler;
		m_pBarMsgHandler = NULL;
	}
	
@xref <i IDMUSProdFramework>, <om IDMUSProdFramework.AddToolBar>, <i IDMUSProdToolBar>
--------------------------------------------------------------------------------------*/

/////////////////////////////////////////////////////////////////////////////
// CFramework::RemoveToolBar

HRESULT CFramework::RemoveToolBar(
	IDMUSProdToolBar* pIToolBar		// @parm [in] Pointer to the <i IDMUSProdToolBar> interface to be removed.

)
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	CJzToolBar* pJzToolBar;

	if( pIToolBar == NULL )
	{
		return E_INVALIDARG;
	}

	POSITION pos = m_lstToolBars.GetHeadPosition();

    while( pos != NULL )
    {
        pJzToolBar = static_cast<CJzToolBar*>( m_lstToolBars.GetNext(pos) );
		if( pJzToolBar )
		{
			if( pJzToolBar->m_pClientToolBar->m_pIToolBar == pIToolBar )
			{
				pos = m_lstToolBars.Find( pJzToolBar );
				if( pos )
				{
					m_lstToolBars.RemoveAt( pos );
				}

				// Remove toolbar from Framework
				RemoveViewMenuItem( pJzToolBar ); 
				delete pJzToolBar;

				return S_OK;
			}
		}
    }

	return E_FAIL;
}


/*======================================================================================
METHOD:  IDMUSPRODFRAMEWORK::SETNBRSTATUSBARPANES
========================================================================================
@method HRESULT | IDMUSProdFramework | SetNbrStatusBarPanes | Creates panes in the
		<o Framework>'s status bar. 

@comm
	Creates <p nCount> panes on the right-hand side of the Framework's status bar.  The
	Framework continues to use the left side of the status bar for its progress bar and
	to display help text.

    Panes created by other calls to <om IDMUSProdFramework.SetNbrStatusBarPanes> may
	exist simultaneously in the status bar.  <p nLifeSpan> helps determine the position
	of newly created panes in relationship to the other panes currently displaying in the
	status bar.  Panes with higher <p nLifeSpan> values are displayed to the left of panes
	with lower <p nLifeSpan> values.  In other words, the more "permanent" panes are
	positioned to the right-hand side of the application window.
	
    The following values are valid for use in <p nLifeSpan>:

	SBLS_APPLICATION: <tab>The status bar panes will exist the entire time the application is running.
	
	SBLS_EDITOR: <tab><tab>The status bar panes will only exist when a specific editor is active.    

	SBLS_CONTROL: <tab><tab>The status bar panes will only exist when a specific control has the focus.

	SBLS_MOUSEDOWNUP: <tab>The status bar panes will only exist while the mouse button is down.

	For even more control over positioning, <p nLifeSpan> may equal one of the defined values
	plus or minus an offset; i.e. SBLS_CONTROL + 1. 
	  
	The caller of <om IDMUSProdFramework.SetNbrStatusBarPanes> is responsible for removing the panes
	at the appropriate time.  For example, when <p nLifeSpan> is set to SBLS_EDITOR it is expected
	that the status bar panes will exist only when their associated editor is active.  In this case,
	<om IDMUSProdFramework::SetNbrStatusBarPanes> should be called during in-place activation when
	the editor's in-place toolbar is being displayed and <om IDMUSProdFramework::RestoreStatusBar>
	should be called the same time the in-place toolbar is removed.

@rvalue S_OK | <p nCount> status bar panes were created successfully. 
@rvalue E_INVALIDARG | Either <p nCount> or <p phKey> is not valid.  <p nCount> must be greater than zero
		and less than 16. <p phKey> must not be NULL.
@rvalue E_ACCESSDENIED | There is no more free space in the status bar.
@rvalue E_FAIL | An error occurred and the panes were not created. 

@ex The following example creates and initializes several panes in the Framework's status
	bar: |

	IDMUSProdFramework* pIFramework;
	HANDLE hKey;

	pIFramework->SetNbrStatusBarPanes( 2, SBLS_EDITOR, &hKey );

    BSTR bstrName = m_pStyle->m_strCategoryName.AllocSysString();
	pIFramework->SetStatusBarPaneInfo( hKey, 0, SBS_NOBORDERS, 16 );
	pIFramework->SetStatusBarPaneText( hKey, 0, bstrName, TRUE );

    bstrName = m_pStyle->m_strName.AllocSysString();
	pIFramework->SetStatusBarPaneInfo( hKey, 1, SBS_SUNKEN, 16 );
	pIFramework->SetStatusBarPaneText( hKey, 1, bstrName, TRUE );
	
@xref <i IDMUSProdFramework>, <om IDMUSProdFramework.SetStatusBarPaneInfo>, <om IDMUSProdFramework.SetStatusBarPaneText>, <om IDMUSProdFramework.RestoreStatusBar>
--------------------------------------------------------------------------------------*/


/////////////////////////////////////////////////////////////////////////////
// CFramework::SetNbrStatusBarPanes

HRESULT CFramework::SetNbrStatusBarPanes(
	int nCount,		// @parm [in] Number of panes to be created.  Must be greater than zero
					//		and less than 16.
	short nLifeSpan,// @parm [in] Length of time the status bar panes are needed.
	HANDLE* phKey	// @parm [out,retval] Pointer to the caller-allocated variable
					//		that receives the HANDLE which allows access to the
					//		Framework's status bar. 
)
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	if( phKey == NULL )
	{
		return E_INVALIDARG;
	}
	*phKey = NULL;

	CMainFrame* pMainFrame = (CMainFrame *)theApp.m_pMainWnd;
	ASSERT( pMainFrame != NULL );
	
	int nTotalPanes = pMainFrame->m_lstStatusBarPanes.GetCount() + nCount;
	if( (nCount <= 0)
	||  (nTotalPanes > MAX_PANES) )
	{
		return E_INVALIDARG;
	}

	// Generate hKey
	HANDLE hKey = (HANDLE)rand();

	HRESULT hr = S_OK;
	
	// Create the pane list
	for( int i = 0 ;  i < nCount ;  i++ )
	{
		// Create item for m_lstStatusBarPanes 
		CJzStatusBarPane* pJzStatusBarPane = new CJzStatusBarPane( hKey, nLifeSpan, i );
		if( pJzStatusBarPane == NULL )
		{
			hr = E_OUTOFMEMORY;
			break;
		}

		// Insert item into m_lstStatusBarPanes 
		pMainFrame->InsertStatusBarPane( pJzStatusBarPane );
	}

	// Sync the UI
	if( SUCCEEDED ( hr ) )
	{
		hr = pMainFrame->SyncStatusBarPanes();
		if( SUCCEEDED ( hr ) )
		{
			*phKey = hKey;
		}
	}
	else
	{
		RestoreStatusBar( hKey );
	}

	CString strText;
	strText.LoadString( AFX_IDS_IDLEMESSAGE );
	pMainFrame->m_wndStatusBar.SetWindowText( strText );

	return hr;
}


/*======================================================================================
METHOD:  IDMUSPRODFRAMEWORK::SETSTATUSBARPANEINFO
========================================================================================
@method HRESULT | IDMUSProdFramework | SetStatusBarPaneInfo | Sets the specified status bar
		pane to a new style and width. 

@comm
	<om IDMUSProdFramework::SetNbrStatusBarPanes> creates one or more panes on the right-hand
	side of the Framework's status bar.  The Framework continues to use the left side of
	the status bar for its progress bar and to display help text.

	The <om IDMUSProdFramework::SetStatusBarPaneInfo>
	and <om IDMUSProdFramework::SetStatusBarPaneText> methods set the style, width and text of each
	pane.

	The following styles are valid for use in <p sbStyle>:
	
	SBS_NOBORDERS		No 3-D border around the pane.

	SBS_RAISED		Reverse border so that text "pops out".

	SBS_SUNKEN		Border is sunken.
	
	<p nMaxChars> is used to set the width of the pane.  The Framework sets the width of the
	pane equal to <p nMaxChars> * tm.tmAveCharWidth, where tm.tmAveCharWidth is the average
	character width of the status bar font.

@rvalue S_OK | Status bar pane <p nIndex> was updated successfully. 
@rvalue E_INVALIDARG | Either <p hKey>, <p nIndex>, <p sbStyle>, or <p nMaxChars> is not
		valid.  For example, <p hKey> may be NULL. 
@rvalue E_ACCESSDENIED | <p hKey> does not match a HANDLE returned by a previous call
		to <om IDMUSProdFramework.SetNbrStatusBarPanes>. 
@rvalue E_FAIL | An error occurred and the pane was not modified. 

@ex The following example creates and initializes several panes in the Framework's status
	bar: |

	IDMUSProdFramework* pIFramework;
	HANDLE hKey;

	pIFramework->SetNbrStatusBarPanes( 2, SBLS_EDITOR, &hKey );

    BSTR bstrName = m_pStyle->m_strCategoryName.AllocSysString();
	pIFramework->SetStatusBarPaneInfo( hKey, 0, SBS_NOBORDERS, 16 );
	pIFramework->SetStatusBarPaneText( hKey, 0, bstrName, TRUE );

    bstrName = m_pStyle->m_strName.AllocSysString();
	pIFramework->SetStatusBarPaneInfo( hKey, 1, SBS_SUNKEN, 16 );
	pIFramework->SetStatusBarPaneText( hKey, 1, bstrName, TRUE );
	
@xref <i IDMUSProdFramework>, <om IDMUSProdFramework.SetNbrStatusBarPanes>, <om IDMUSProdFramework.SetStatusBarPaneText>, <om IDMUSProdFramework.RestoreStatusBar>
--------------------------------------------------------------------------------------*/

#define MAX_PANE_CHARS	48

/////////////////////////////////////////////////////////////////////////////
// CFramework::SetStatusBarPaneInfo

HRESULT CFramework::SetStatusBarPaneInfo(
	HANDLE hKey,			// @parm [in] HANDLE returned by <om IDMUSProdFramework.SetNbrStatusBarPanes>.
	int nIndex,				// @parm [in] Zero based index of the pane whose
							//		information is to be set.  <p nIndex> must be
							//		greater than or equal to zero, and less than
							//		the number of panes created by
							//		<om IDMUSProdFramework::SetNbrStatusBarPanes>.
	StatusBarStyle sbStyle,	// @parm [in] New style for the pane.  <p sbStyle>
							//		must be SBS_NOBORDERS, SBS_RAISED, or SBS_SUNKEN.
	int nMaxChars			// @parm [in] Maximum number of characters in pane text.
							//		<p nMaxChars> must be greater than zero, and less
							//		than or equal to 48.
)
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	CMainFrame* pMainFrame = (CMainFrame *)theApp.m_pMainWnd;
	ASSERT( pMainFrame != NULL );

	CJzStatusBarPane* pJzStatusBarPane;
		
	HRESULT hr = pMainFrame->GetStatusBarPane( hKey, nIndex, &pJzStatusBarPane );
	if( FAILED ( hr ) )
	{
		return hr;
	}

	switch( sbStyle )
	{
		case SBS_NOBORDERS:
			pJzStatusBarPane->m_nStyle = SBPS_NOBORDERS;
			break;

		case SBS_RAISED:
			pJzStatusBarPane->m_nStyle = SBPS_POPOUT;
			break;

		case SBS_SUNKEN:
			pJzStatusBarPane->m_nStyle = SBPS_NORMAL;
			break;

		default:
			return E_INVALIDARG;
	}

	CDC* pDC = pMainFrame->m_wndStatusBar.GetDC();
	if( pDC )
	{
		TEXTMETRIC tm;

		pDC->GetTextMetrics( &tm );
		pJzStatusBarPane->m_nWidth = (short)(nMaxChars * tm.tmAveCharWidth);
		pMainFrame->m_wndStatusBar.ReleaseDC( pDC );
	}
	else
	{
		return E_FAIL;
	}

	pMainFrame->m_wndStatusBar.SetPaneInfo( pJzStatusBarPane->m_nStatusBarIndex + NBR_FRAMEWORK_PANES,
											ID_SEPARATOR,
											pJzStatusBarPane->m_nStyle,
											pJzStatusBarPane->m_nWidth );

	return S_OK;
}


/*======================================================================================
METHOD:  IDMUSPRODFRAMEWORK::SETSTATUSBARPANETEXT
========================================================================================
@method HRESULT | IDMUSProdFramework | SetStatusBarPaneText | Sets the text of the specified
		status bar pane. 

@comm
	<om IDMUSProdFramework::SetNbrStatusBarPanes> creates one or more panes on the right-hand
	side of the Framework's status bar.  The Framework continues to use the left side of
	the status bar for its progress bar and to display help text.

	The <om IDMUSProdFramework::SetStatusBarPaneInfo> and
	<om IDMUSProdFramework::SetStatusBarPaneText> methods set the style, width and text of each
	pane.

@rvalue S_OK | Status bar pane text was updated successfully. 
@rvalue E_INVALIDARG | Either <p hKey> or <p nIndex> is not valid. For example,
		<p hKey> may be NULL.
@rvalue E_ACCESSDENIED | <p hKey> does not match a HANDLE returned by a previous call
		to <om IDMUSProdFramework.SetNbrStatusBarPanes>. 
@rvalue E_FAIL | An error occurred and the pane text was not modified. 

@ex The following example creates and initializes several panes in the Framework's status
	bar: |

	IDMUSProdFramework* pIFramework;
	HANDLE hKey;

	pIFramework->SetNbrStatusBarPanes( 2, SBLS_EDITOR, &hKey );

    BSTR bstrName = m_pStyle->m_strCategoryName.AllocSysString();
	pIFramework->SetStatusBarPaneInfo( hKey, 0, SBS_NOBORDERS, 16 );
	pIFramework->SetStatusBarPaneText( hKey, 0, bstrName, TRUE );

    bstrName = m_pStyle->m_strName.AllocSysString();
	pIFramework->SetStatusBarPaneInfo( hKey, 1, SBS_SUNKEN, 16 );
	pIFramework->SetStatusBarPaneText( hKey, 1, bstrName, TRUE );
	
@xref <i IDMUSProdFramework>, <om IDMUSProdFramework.SetNbrStatusBarPanes>, <om IDMUSProdFramework.SetStatusBarPaneInfo>, <om IDMUSProdFramework.RestoreStatusBar>
--------------------------------------------------------------------------------------*/

/////////////////////////////////////////////////////////////////////////////
// CFramework::SetStatusBarPaneText

HRESULT CFramework::SetStatusBarPaneText(
	HANDLE hKey,	// @parm [in] HANDLE returned by <om IDMUSProdFramework.SetNbrStatusBarPanes>.
	int nIndex,		// @parm [in] Zero based index of the pane whose
					//		information is to be set.  <p nIndex> must be
					//		greater than or equal to zero, and less than
					//		the number of panes created by
					//		<om IDMUSProdFramework::SetNbrStatusBarPanes>.
	BSTR bstrText,	// @parm [in] New text.  This method must free <p bstrText> with
					//		SysFreeString when it is no longer needed.
	BOOL bUpdate	// @parm [in] If TRUE, the pane is invalidated after the text update.
)
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	if( bstrText == NULL )
	{
		return E_INVALIDARG;
	}

	CMainFrame* pMainFrame = (CMainFrame *)theApp.m_pMainWnd;
	ASSERT( pMainFrame != NULL );

	CJzStatusBarPane* pJzStatusBarPane;
		
	HRESULT hr = pMainFrame->GetStatusBarPane( hKey, nIndex, &pJzStatusBarPane );
	if( FAILED ( hr ) )
	{
		::SysFreeString( bstrText );
		return hr;
	}

	pJzStatusBarPane->m_strText = bstrText;
	::SysFreeString( bstrText );

	pMainFrame->m_wndStatusBar.SetPaneText( pJzStatusBarPane->m_nStatusBarIndex + NBR_FRAMEWORK_PANES,
											pJzStatusBarPane->m_strText,
											bUpdate );

	return S_OK;
}


/*======================================================================================
METHOD:  IDMUSPRODFRAMEWORK::RESTORESTATUSBAR
========================================================================================
@method HRESULT | IDMUSProdFramework | RestoreStatusBar | Removes panes from the
		<o Framework>'s status bar. 

@comm
	<om IDMUSProdFramework::SetNbrStatusBarPanes> creates one or more panes on the right-hand
	side of the Framework's status bar.  The Framework continues to use the left side of
	the status bar for its progress bar and to display help text.

@rvalue S_OK | The status bar panes were removed successfully. 
@rvalue E_INVALIDARG | The value of <p hKey> is not valid.  For example, it may be NULL.
@rvalue E_ACCESSDENIED | <p hKey> does not match a HANDLE returned by a previous call
		to <om IDMUSProdFramework.SetNbrStatusBarPanes>. 
	
@xref <i IDMUSProdFramework>, <om IDMUSProdFramework.SetNbrStatusBarPanes>, <om IDMUSProdFramework.SetStatusBarPaneInfo>, <om IDMUSProdFramework.SetStatusBarPaneText>
--------------------------------------------------------------------------------------*/

/////////////////////////////////////////////////////////////////////////////
// CFramework::RestoreStatusBar

HRESULT CFramework::RestoreStatusBar(
	HANDLE hKey		// @parm [in] HANDLE returned by <om IDMUSProdFramework.SetNbrStatusBarPanes>.
)
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	CMainFrame* pMainFrame = (CMainFrame *)theApp.m_pMainWnd;
	ASSERT( pMainFrame != NULL );

	if( hKey == NULL )
	{
		return E_INVALIDARG;
	}

	HRESULT hr = E_ACCESSDENIED;

	CJzStatusBarPane* pJzStatusBarPane;
	POSITION pos2;

    POSITION pos = pMainFrame->m_lstStatusBarPanes.GetHeadPosition();
    while( pos )
    {
        pos2 = pos;
		pJzStatusBarPane = pMainFrame->m_lstStatusBarPanes.GetNext( pos );

		if( pJzStatusBarPane->m_hKey == hKey )
		{
			// Key is valid
			hr = S_OK;

			pMainFrame->m_lstStatusBarPanes.RemoveAt( pos2 );
			delete pJzStatusBarPane; 
		}
    }

	// Sync the UI
	if( SUCCEEDED ( hr ) )
	{
		hr = pMainFrame->SyncStatusBarPanes();
	}

	CString strText;
	strText.LoadString( AFX_IDS_IDLEMESSAGE );
	pMainFrame->m_wndStatusBar.SetWindowText( strText );

	return hr;
}



/*======================================================================================
METHOD:  IDMUSPRODFRAMEWORK::STARTPROGRESSBAR
========================================================================================
@method HRESULT | IDMUSProdFramework | StartProgressBar | Creates a progress bar control
		in the <o Framework>'s status bar. 

@comm The <o Framework>'s "progress bar" can be used by DirectMusic Producer <o Component>s
	to indicate the progress of a lengthy operation.  The progress bar control has a range
	and a current position.  The range represents the length of the entire operation, and
	the current position represents the progress the application has made toward completing
	the operation.

@rvalue S_OK | The progress bar was created successfully. 
@rvalue E_OUTOFMEMORY | Out of memory.
@rvalue E_INVALIDARG | The value of <p phKey> is not valid.  For example, it may be NULL.
@rvalue E_ACCESSDENIED | The progress bar is already being used to track another operation.
@rvalue E_FAIL | An error occurred and the progress bar was not created.

@ex The following example creates a progress bar control in the Framework's status bar: |

	IDMUSProdFramework* pIFramework;
	HANDLE hKey;
	  
	CString strPrompt = _T( "Preparing information" );
	BSTR bstrPrompt = strPrompt.AllocSysString();

	pIFramework->StartProgressBar( 0, 50, bstrPrompt, &hKey );
	for( int i = 0 ;  i < 50 ;  i++ )
	{
		Sleep( 50 );
		pIFramework->SetProgressBarPos( hKey, i );
	}
	pIFramework->EndProgressBar( hKey );
	
@xref <i IDMUSProdFramework>, <om IDMUSProdFramework.SetProgressBarPos>, <om IDMUSProdFramework.SetProgressBarStep>, <om IDMUSProdFramework.StepProgressBar>, <om IDMUSProdFramework.EndProgressBar>
--------------------------------------------------------------------------------------*/

#define PROGRESS_BAR_WIDTH	146

/////////////////////////////////////////////////////////////////////////////
// CFramework::StartProgressBar

HRESULT CFramework::StartProgressBar(
	int nLower,			// @parm [in] Specifies lower limit of the progress bar range.
	int nUpper,			// @parm [in] Specifies upper limit of the progress bar range.
	BSTR bstrPrompt,	// @parm [in] Text for the progress bar prompt.  This
						//		method must free <p bstrPrompt> with SysFreeString
						//		when it is no longer needed.
	HANDLE* phKey		// @parm [out,retval] Pointer to the caller-allocated variable
						//		that receives the HANDLE which allows access to the
						//		Framework's progress bar. 
)
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	if( bstrPrompt == NULL )
	{
		return E_INVALIDARG;
	}
	CString strPrompt = bstrPrompt;
	::SysFreeString( bstrPrompt );

	if( phKey == NULL )
	{
		return E_INVALIDARG;
	}
	*phKey = NULL;

	CMainFrame* pMainFrame = (CMainFrame *)theApp.m_pMainWnd;
	ASSERT( pMainFrame != NULL );

	if( pMainFrame->m_pProgressCtrl )
	{
		return E_ACCESSDENIED;
	}

	pMainFrame->m_pProgressCtrl = new CProgressCtrl;
	if( pMainFrame->m_pProgressCtrl == NULL )
	{
		return E_OUTOFMEMORY;
	}

	pMainFrame->m_wndStatusBar.SetWindowText( strPrompt );
	pMainFrame->m_wndStatusBar.UpdateWindow();

	RECT rect;
	CSize sizeText;
	CDC* pDC;

	pDC = pMainFrame->m_wndStatusBar.GetDC();
	sizeText = pDC->GetTextExtent( strPrompt );
	pMainFrame->m_wndStatusBar.ReleaseDC( pDC );

	pMainFrame->m_wndStatusBar.GetItemRect( 0, &rect );
	rect.left += sizeText.cx;
	if( rect.left > (rect.right - PROGRESS_BAR_WIDTH) )
	{
		rect.left = (rect.right - PROGRESS_BAR_WIDTH);
	}
	rect.right = rect.left + PROGRESS_BAR_WIDTH;

	VERIFY( pMainFrame->m_pProgressCtrl->Create((WS_CHILD | WS_VISIBLE),
			rect, &pMainFrame->m_wndStatusBar, 1) );
	pMainFrame->m_pProgressCtrl->SetRange( nLower, nUpper );

	pMainFrame->m_hProgressCtrl = (HANDLE)rand();
	if( pMainFrame->m_hProgressCtrl == NULL )
	{
		pMainFrame->m_hProgressCtrl = (HANDLE)0x71954;
	}
	*phKey = pMainFrame->m_hProgressCtrl;

	return S_OK;
}


/*======================================================================================
METHOD:  IDMUSPRODFRAMEWORK::SETPROGRESSBARPOS
========================================================================================
@method HRESULT | IDMUSProdFramework | SetProgressBarPos | Sets the current position of the
		<o Framework>'s progress bar control. 

@comm The <o Framework>'s "progress bar" can be used by DirectMusic Producer <o Component>s
	to indicate the progress of a lengthy operation.  The progress bar control has a range
	and a current position.  The range represents the length of the entire operation, and
	the current position represents the progress the application has made toward completing
	the operation.

@rvalue S_OK | The position of the progress bar control was set successfully. 
@rvalue E_INVALIDARG | The value of <p hKey> is not valid.  For example, it may be NULL.
@rvalue E_ACCESSDENIED | <p hKey> does not match the HANDLE returned by the previous call
		to <om IDMUSProdFramework.StartProgressBar>. 

@ex The following example creates a progress bar control in the Framework's status bar: |

	IDMUSProdFramework* pIFramework;
	HANDLE hKey;
	  
	CString strPrompt = _T( "Preparing information" );
	BSTR bstrPrompt = strPrompt.AllocSysString();

	pIFramework->StartProgressBar( 0, 50, bstrPrompt, &hKey );
	for( int i = 0 ;  i < 50 ;  i++ )
	{
		Sleep( 50 );
		pIFramework->SetProgressBarPos( hKey, i );
	}
	pIFramework->EndProgressBar( hKey );
	
@xref <i IDMUSProdFramework>, <om IDMUSProdFramework.StartProgressBar>, <om IDMUSProdFramework.SetProgressBarStep>, <om IDMUSProdFramework.StepProgressBar>, <om IDMUSProdFramework.EndProgressBar>
--------------------------------------------------------------------------------------*/

/////////////////////////////////////////////////////////////////////////////
// CFramework::SetProgressBarPos

HRESULT CFramework::SetProgressBarPos(
	HANDLE hKey,	// @parm [in] HANDLE returned by <om IDMUSProdFramework.StartProgressBar>.
	int nPos		// @parm [in] New position of the progress bar control.
)
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	if( hKey == NULL )
	{
		return E_INVALIDARG;
	}

	CMainFrame* pMainFrame = (CMainFrame *)theApp.m_pMainWnd;
	ASSERT( pMainFrame != NULL );

	if( hKey !=  pMainFrame->m_hProgressCtrl )
	{
		return E_ACCESSDENIED;
	}

	if( pMainFrame->m_pProgressCtrl )
	{
		pMainFrame->m_pProgressCtrl->SetPos( nPos );
	}

	return S_OK;
}


/*======================================================================================
METHOD:  IDMUSPRODFRAMEWORK::SETPROGRESSBARSTEP
========================================================================================
@method HRESULT | IDMUSProdFramework | SetProgressBarStep | Specifies the step increment for
		the <o Framework>'s progress bar control. 

@comm The <o Framework>'s "progress bar" can be used by DirectMusic Producer <o Component>s
	to indicate the progress of a lengthy operation.  The progress bar control has a range
	and a current position.  The range represents the length of the entire operation, and
	the current position represents the progress the application has made toward completing
	the operation.

	The step increment is the amount by which a call to <om IDMUSProdFramework.StepProgressBar>
	increases the progress bar's current position.

@rvalue S_OK | The step increment of the progress bar control was set successfully. 
@rvalue E_INVALIDARG | The value of <p hKey> is not valid.  For example, it may be NULL.
@rvalue E_ACCESSDENIED | <p hKey> does not match the HANDLE returned by the previous call
		to <om IDMUSProdFramework.StartProgressBar>. 


@ex The following example creates a progress bar control in the Framework's status bar: |

	IDMUSProdFramework* pIFramework;
	HANDLE hKey;
	  
	CString strPrompt = _T( "Preparing information" );
	BSTR bstrPrompt = strPrompt.AllocSysString();

	pIFramework->StartProgressBar( 0, 50, bstrPrompt, &hKey );
	pIFramework->SetProgressBarPos( hKey, 0 );
	pIFramework->SetProgressBarStep( hKey, 5 );

	for( int i = 0 ;  i < 10 ;  i++ )
	{
		Sleep( 50 );
		pIFramework->StepProgressBar( hKey );
	}

	pIFramework->EndProgressBar( hKey );
	
@xref <i IDMUSProdFramework>, <om IDMUSProdFramework.StartProgressBar>, <om IDMUSProdFramework.SetProgressBarPos>, <om IDMUSProdFramework.StepProgressBar>, <om IDMUSProdFramework.EndProgressBar>
--------------------------------------------------------------------------------------*/

/////////////////////////////////////////////////////////////////////////////
// CFramework::SetProgressBarStep

HRESULT CFramework::SetProgressBarStep(
	HANDLE hKey,	// @parm [in] HANDLE returned by <om IDMUSProdFramework.StartProgressBar>.
	UINT nValue		// @parm [in] New step increment for the progress bar control.
)
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	if( hKey == NULL )
	{
		return E_INVALIDARG;
	}

	CMainFrame* pMainFrame = (CMainFrame *)theApp.m_pMainWnd;
	ASSERT( pMainFrame != NULL );

	if( hKey !=  pMainFrame->m_hProgressCtrl )
	{
		return E_ACCESSDENIED;
	}

	if( pMainFrame->m_pProgressCtrl )
	{
		pMainFrame->m_pProgressCtrl->SetStep( nValue );
	}

	return S_OK;
}


/*======================================================================================
METHOD:  IDMUSPRODFRAMEWORK::STEPPROGRESSBAR
========================================================================================
@method HRESULT | IDMUSProdFramework | StepProgressBar | Advances current position of the
	<o Framework>'s progress bar control by the step increment. 

@comm The <o Framework>'s "progress bar" can be used by DirectMusic Producer <o Component>s
	to indicate the progress of a lengthy operation.  The progress bar control has a range
	and a current position.  The range represents the length of the entire operation, and
	the current position represents the progress the application has made toward completing
	the operation.

	<om IDMUSProdFramework.SetProgressBarStep> sets the step increment for the progress bar control.

@rvalue S_OK | The current position of the progress bar control was advanced successfully. 
@rvalue E_INVALIDARG | The value of <p hKey> is not valid.  For example, it may be NULL.
@rvalue E_ACCESSDENIED | <p hKey> does not match the HANDLE returned by the previous call
		to <om IDMUSProdFramework.StartProgressBar>. 

@ex The following example creates a progress bar control in the Framework's status bar: |

	IDMUSProdFramework* pIFramework;
	HANDLE hKey;
	  
	CString strPrompt = _T( "Preparing information" );
	BSTR bstrPrompt = strPrompt.AllocSysString();

	pIFramework->StartProgressBar( 0, 50, bstrPrompt, &hKey );
	pIFramework->SetProgressBarPos( hKey, 0 );
	pIFramework->SetProgressBarStep( hKey, 5 );

	for( int i = 0 ;  i < 10 ;  i++ )
	{
		Sleep( 50 );
		pIFramework->StepProgressBar( hKey );
	}

	pIFramework->EndProgressBar( hKey );
	
@xref <i IDMUSProdFramework>, <om IDMUSProdFramework.StartProgressBar>, <om IDMUSProdFramework.SetProgressBarPos>, <om IDMUSProdFramework.SetProgressBarStep>, <om IDMUSProdFramework.EndProgressBar>
--------------------------------------------------------------------------------------*/

/////////////////////////////////////////////////////////////////////////////
// CFramework::StepProgressBar

HRESULT CFramework::StepProgressBar(
	HANDLE hKey		// @parm [in] HANDLE returned by <om IDMUSProdFramework.StartProgressBar>.
)
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	if( hKey == NULL )
	{
		return E_INVALIDARG;
	}

	CMainFrame* pMainFrame = (CMainFrame *)theApp.m_pMainWnd;
	ASSERT( pMainFrame != NULL );

	if( hKey !=  pMainFrame->m_hProgressCtrl )
	{
		return E_ACCESSDENIED;
	}

	if( pMainFrame->m_pProgressCtrl )
	{
		pMainFrame->m_pProgressCtrl->StepIt();
	}

	return S_OK;
}


/*======================================================================================
METHOD:  IDMUSPRODFRAMEWORK::ENDPROGRESSBAR
========================================================================================
@method HRESULT | IDMUSProdFramework | EndProgressBar | Removes the progress bar control
		from the <o Framework>'s status bar. 

@comm The <o Framework>'s "progress bar" can be used by DirectMusic Producer <o Component>s
	to indicate the progress of a lengthy operation.  The progress bar control has a range
	and a current position.  The range represents the length of the entire operation, and
	the current position represents the progress the application has made toward completing
	the operation.

@rvalue S_OK | The progress bar control was removed successfully. 
@rvalue E_INVALIDARG | The value of <p hKey> is not valid.  For example, it may be NULL.
@rvalue E_ACCESSDENIED | <p hKey> does not match the HANDLE returned by the previous call
		to <om IDMUSProdFramework.StartProgressBar>. 

@ex The following example creates a progress bar control in the Framework's status bar: |

	IDMUSProdFramework* pIFramework;
	HANDLE hKey;
	  
	CString strPrompt = _T( "Preparing information" );
	BSTR bstrPrompt = strPrompt.AllocSysString();

	pIFramework->StartProgressBar( 0, 50, bstrPrompt, &hKey );
	for( int i = 0 ;  i < 50 ;  i++ )
	{
		Sleep( 50 );
		pIFramework->SetProgressBarPos( hKey, i );
	}
	pIFramework->EndProgressBar( hKey );
	
@xref <i IDMUSProdFramework>, <om IDMUSProdFramework.StartProgressBar>, <om IDMUSProdFramework.SetProgressBarPos>, <om IDMUSProdFramework.SetProgressBarStep>, <om IDMUSProdFramework.StepProgressBar>
--------------------------------------------------------------------------------------*/

/////////////////////////////////////////////////////////////////////////////
// CFramework::EndProgressBar

HRESULT CFramework::EndProgressBar(
	HANDLE hKey		// @parm [in] HANDLE returned by <om IDMUSProdFramework.StartProgressBar>.
)
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	if( hKey == NULL )
	{
		return E_INVALIDARG;
	}

	CMainFrame* pMainFrame = (CMainFrame *)theApp.m_pMainWnd;
	ASSERT( pMainFrame != NULL );

	if( hKey !=  pMainFrame->m_hProgressCtrl )
	{
		return E_ACCESSDENIED;
	}

	if( pMainFrame->m_pProgressCtrl )
	{
		delete pMainFrame->m_pProgressCtrl;
		pMainFrame->m_pProgressCtrl = NULL;
		pMainFrame->m_hProgressCtrl = NULL;
	}

	CString strText;
	
	strText.LoadString( AFX_IDS_IDLEMESSAGE );
	pMainFrame->m_wndStatusBar.SetWindowText( strText );

	return S_OK;
}


/*======================================================================================
METHOD:  IDMUSPRODFRAMEWORK::GETSHAREDOBJECT
========================================================================================
@method HRESULT | IDMUSProdFramework | GetSharedObject | Allows one or more DirectMusic
	Producer <o Component>s to share the same COM object.

@comm
	This method calls CoCreateInstance to create objects of type <p rclsid>.  The
	<o Framework> holds a reference to the created object so that subsequent calls to
	<om IDMUSProdFramework.GetSharedObject> can simply return an additional reference
	to a previously created object whose CLSID matches <p rclsid>.

@rvalue S_OK | The interface specified in <p riid> was returned successfully.
@rvalue E_POINTER | The address in <p ppvObject> is not valid.  For example, it may be NULL.
@rvalue E_FAIL | An error occurred, and the interface specified in <p riid> could not be returned.

@ex The following example obtains an interface to a shared DirectMusic Composer object: |

IDirectMusicComposer* pIComposer;
HRESULT hr;

hr = pIFramework->GetSharedObject( CLSID_DMCompos, IID_IDirectMusicComposer, (void**)&pIComposer )

if( SUCCEEDED ( hr ) )  
{
		.....
		.....

		pIComposer->Release();
}

@xref <i IDMUSProdFramework>
--------------------------------------------------------------------------------------*/

HRESULT CFramework::GetSharedObject(
	REFCLSID rclsid,				// @parm [in] CLSID of the requested object.
	REFIID riid,					// @parm [in] Identifier of the requested interface.
	void** ppvObject				// @parm [out,retval] Address of a variable to receive
									//		the requested object. On success, the caller is
									//		responsible for calling IUnknown::Release.
)
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	if( ppvObject == NULL )
	{
		return E_POINTER;
	}

	*ppvObject = NULL;

	// See if the object was created by a previous call to GetSharedObject
	CJzSharedObject* pJzSharedObject;
	
	POSITION pos = m_lstSharedObjects.GetHeadPosition();
    while( pos )
    {
        pJzSharedObject = static_cast<CJzSharedObject*>( m_lstSharedObjects.GetNext(pos) );

		if( ::IsEqualCLSID( pJzSharedObject->m_clsid, rclsid ) )
		{
			return pJzSharedObject->m_pIUnknown->QueryInterface( riid, ppvObject );
		}
    }

	// Attempt to create the object
	IUnknown* pIUnknown;

	if( FAILED ( ::CoCreateInstance( rclsid, NULL, CLSCTX_INPROC, IID_IUnknown, (void **)&pIUnknown ) ) )
	{
		return E_FAIL;
	}

	// Store IUnknown* in our SharedObjects list
	pJzSharedObject = new CJzSharedObject( rclsid, pIUnknown );
	if( pJzSharedObject == NULL )
	{
		pIUnknown->Release();
		return E_OUTOFMEMORY;
	}

	m_lstSharedObjects.AddTail( pJzSharedObject );
	pIUnknown->Release();

	// Return the requested interface
	return pJzSharedObject->m_pIUnknown->QueryInterface( riid, ppvObject );
}


/*======================================================================================
METHOD:  IDMUSPRODFRAMEWORK::ADDTONOTIFYLIST
========================================================================================
@method HRESULT | IDMUSProdFramework | AddToNotifyList | Adds <p pINotifyThisNode> to the list
		of <o Node>s notified when the state of <p pIDocRootNode> changes. 

@comm 
	<p pIDocRootNode> must point to a DocRoot <o Node>.  A DocRoot node is the top node of
	a document, or file.  For example, the Style node of a Style file would be considered
	the DocRoot node of the Style.  The Band node of a Band file would be considered the
	DocRoot node of the Band file.

	<p pINotifyThisNode> must also implement <i IDMUSProdNotifySink>.  The IDMUSProdNotifySink
	interface is used to notify <p pINotifyThisNode> when the state of <p pIDocRootNode>
	changes.

@rvalue S_OK | <p pINotifyThisNode> was added to the list of nodes to be notified when
		the state of <p pIDocRootNode> changes.
@rvalue E_INVALIDARG | Either <p pIDocRootNode> or <p pINotifyThisNode> is not valid.
		For example, they may be NULL.
@rvalue E_FAIL | An error occurred, and <p pINotifyThisNode> could not be added to the
		notify list attached to <p pIDocRootNode>.

@ex The following example displays a File Open dialog for the purpose of selecting a Segment's Style: |

	IDMUSProdReferenceNode* pIReferenceNode;
	IDMUSProdDocType* pIDocType;

    if( SUCCEEDED( pSegment->m_pIStyleRefNode->QueryInterface( IID_IDMUSProdReferenceNode, (void**)&pIReferenceNode ) ) )
	{
		if( SUCCEEDED ( pIFramework->FindDocTypeByNodeId( GUID_StyleNode, &pIDocType ) ) ) 
		{
			if( pIFramework->OpenFile( pIDocType, NULL, pSegment, &pIStyleDocRootNode ) == S_OK ) 
			{
				if( SUCCEEDED( pIReferenceNode->SetReferencedFile( pIStyleDocRootNode ) ) )
				{
					pIFramework->AddToNotifyList( pIStyleDocRootNode, pSegment );
				}
				pIStyleDocRootNode->Release();
			}
			pIDocType->Release();
		}
		pIReferenceNode->Release();
	}

@xref <i IDMUSProdFramework>, <om IDMUSProdFramework.RemoveFromNotifyList>, <om IDMUSProdFramework.NotifyNodes>, <i IDMUSProdNotifySink>
--------------------------------------------------------------------------------------*/

/////////////////////////////////////////////////////////////////////////////
// CFramework::AddToNotifyList

HRESULT CFramework::AddToNotifyList(
	IDMUSProdNode* pIDocRootNode,		// @parm [in] Pointer to an <i IDMUSProdNode> interface.
	IDMUSProdNode* pINotifyThisNode		// @parm [in] Pointer to the <i IDMUSProdNode> requesting a
									//		callback when <p pIDocRootNode> changes.
)
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	if( pIDocRootNode == NULL
	||  pINotifyThisNode == NULL )
	{
		return E_INVALIDARG;
	}

	// Make sure pIDocRootNode is really a DocRoot node already in the Project Tree
	if( IsDocRootNode( pIDocRootNode ) == FALSE )
	{
		return E_INVALIDARG;
	}

	// Make sure pINotifyThisNode has implemented IDMUSProdNotifySink
	IDMUSProdNotifySink* pINotifySink;

	if( FAILED ( pINotifyThisNode->QueryInterface( IID_IDMUSProdNotifySink, (void **)&pINotifySink ) ) )
	{
		return E_INVALIDARG;
	}
	pINotifySink->Release();

	// Get the CFileNode associated with pIDocRootNode
	CFileNode* pFileNode = NULL;
	
	CComponentDoc* pComponentDoc = theApp.FindComponentDocument( pIDocRootNode );
	if( pComponentDoc )
	{
		pFileNode = pComponentDoc->m_pFileNode;
	}
	if( pFileNode == NULL )
	{
		return E_FAIL;
	}

	// Insert pINotifyThisNode into CFileNode's notify list
	pFileNode->AddToNotifyList( pINotifyThisNode );

	return S_OK;
}


/*======================================================================================
METHOD:  IDMUSPRODFRAMEWORK::REMOVEFROMNOTIFYLIST
========================================================================================
@method HRESULT | IDMUSProdFramework | RemoveFromNotifyList | Removes <p pINotifyThisNode> from
		the list of <o Node>s notified when the state of <p pIDocRootNode> changes. 

@comm 
	<p pIDocRootNode> must point to a DocRoot <o Node>.  A DocRoot node is the top node of
	a document, or file.  For example, the Style node of a Style file would be considered
	the DocRoot node of the Style.  The Band node of a Band file would be considered the
	DocRoot node of the Band file.

    <om IDMUSProdFramework.RemoveFromNotifyList> advises the Framework that <p pINotifyThisNode>
	no longer references <p pIDocRootNode>.  This method should not be called as part of normal
	shutdown procedure.

@rvalue S_OK | <p pINotifyThisNode> was removed from the list of nodes to be notified when
		the state of <p pIDocRootNode> changes.
@rvalue E_INVALIDARG | Either <p pIDocRootNode> or <p pINotifyThisNode> is not valid.
		For example, they may be NULL.
@rvalue E_FAIL | An error occurred, and <p pINotifyThisNode> could not be removed from
		the notify list attached to <p pIDocRootNode>.
	
@xref <i IDMUSProdFramework>, <om IDMUSProdFramework.AddToNotifyList>, <om IDMUSProdFramework.NotifyNodes>, <i IDMUSProdNotifySink>
--------------------------------------------------------------------------------------*/

/////////////////////////////////////////////////////////////////////////////
// CFramework::RemoveFromNotifyList

HRESULT CFramework::RemoveFromNotifyList(
	IDMUSProdNode* pIDocRootNode,		// @parm [in] Pointer to an <i IDMUSProdNode> interface.
	IDMUSProdNode* pINotifyThisNode		// @parm [in] Pointer to the <i IDMUSProdNode> requesting a
									//		callback when <p pIDocRootNode> changes.
)
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	if( pIDocRootNode == NULL
	||  pINotifyThisNode == NULL )
	{
		return E_INVALIDARG;
	}

	// Get the CFileNode associated with pIDocRootNode
	CFileNode* pFileNode = NULL;
	
	CComponentDoc* pComponentDoc = theApp.FindComponentDocument( pIDocRootNode );
	if( pComponentDoc )
	{
		pFileNode = pComponentDoc->m_pFileNode;
	}
	if( pFileNode == NULL )
	{
		return E_FAIL;
	}

	// Remove matching pINotifyThisNode from CFileNode's notify list
	pFileNode->RemoveFromNotifyList( pINotifyThisNode );

	return S_OK;
}


/*======================================================================================
METHOD:  IDMUSPRODFRAMEWORK::NOTIFYNODES
========================================================================================
@method HRESULT | IDMUSProdFramework | NotifyNodes | Notifies all <o Node>s linked to
		<p pIChangedDocRootNode> via calls to <om IDMUSProdFramework.AddToNotifyList> that
		the state of <p pIChangedDocRootNode> has changed. 

@comm 
	<p pIChangedDocRootNode> must point to a DocRoot <o Node>.  A DocRoot node is the top
	node of a document, or file.  For example, the Style node of a Style file would be
	considered the DocRoot node of the Style.  The Band node of a Band file would be
	considered the DocRoot node of the Band file.

	<p pData> may be NULL.
	
	Valid <p guidUpdateType> and <p pData> values must be defined and documented in the .h
	and .lib files distributed by the Component supporting nodes of type <p pIDocRootNode>.
	For example, the Style Component's StyleDesigner.h and StyleDesignerGuid.lib files
	must contain and document the various <p guidUpdateType> and <p pData> values it
	uses to communicate Style file changes. 

	The following list contains notifications provided by the Framework:

	<tab><p guidUpdateType> <tab><tab><tab><p pData> <tab><tab><p Description>

	<tab>FRAMEWORK_FileDeleted <tab><tab>NULL <tab><tab> File deleted by user and is no longer in the Project Tree.

	<tab>FRAMEWORK_FileReplaced <tab><tab>NULL <tab><tab> Linked file being replaced by another version of the file.

	<tab>FRAMEWORK_FileClosed <tab><tab>NULL <tab><tab> File closed normally and is no longer in the Project Tree.

	<tab>FRAMEWORK_FileNameChange <tab><tab>NULL <tab><tab> File renamed by user.

@rvalue S_OK | Nodes linked to <p pIDocRootNode> were notified successfully.
@rvalue E_INVALIDARG | <p pIChangedDocRootNode> is not valid.  For example, it may be NULL.
@rvalue E_FAIL | An error occurred, and the nodes linked to <p pIDocRootNode> could not be
		notified.

@ex The following example notifies interested nodes that the name of a Style has changed: |

HRESULT CStyle::SetNodeName( BSTR bstrName )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pComponent != NULL );
	ASSERT( m_pComponent->m_pIFramework != NULL );

	CString strName = bstrName;
	::SysFreeString( bstrName );

	m_strName = strName;

	IDMUSProdPropSheet* pIPropSheet;
	if( SUCCEEDED ( m_pComponent->m_pIFramework->QueryInterface( IID_IDMUSProdPropSheet, (void**)&pIPropSheet ) ) )
	{
		pIPropSheet->RefreshTitleByObject( this );
		pIPropSheet->Release();
	}
	
	SetModified( TRUE );

	// Notify connected nodes that the Style name has changed
	m_pComponent->m_pIFramework->NotifyNodes( this, STYLE_NameChange, NULL );

	return S_OK;
}
	
@xref <i IDMUSProdFramework>, <om IDMUSProdFramework.AddToNotifyList>, <om IDMUSProdFramework.RemoveFromNotifyList>, <i IDMUSProdNotifySink>
--------------------------------------------------------------------------------------*/

/////////////////////////////////////////////////////////////////////////////
// CFramework::NotifyNodes

HRESULT CFramework::NotifyNodes(
	IDMUSProdNode* pIChangedDocRootNode, // @parm [in] Pointer to the changed DocRoot's <i IDMUSProdNode>
									 //		interface.
	GUID guidUpdateType,			 // @parm [in] Identifies type of change.
	void* pData						 // @parm [in] Pointer to additional data associated
									 //		with the change.
)
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	if( pIChangedDocRootNode == NULL )
	{
		return E_INVALIDARG;
	}

	// Get the CFileNode associated with pIChangedDocRootNode
	CFileNode* pFileNode = theApp.GetFileByDocRootNode( pIChangedDocRootNode );
	if( pFileNode == NULL )
	{
		return E_FAIL;
	}

	CJzNotifyNode* pJzNotifyNode;
	IDMUSProdNotifySink* pINotifySink;

	// Send notification to all nodes in the CFileNode's notify list
	POSITION pos = pFileNode->m_lstNotifyNodes.GetHeadPosition();
    while( pos != NULL )
    {
        pJzNotifyNode = static_cast<CJzNotifyNode*>( pFileNode->m_lstNotifyNodes.GetNext(pos) );
		if( pJzNotifyNode->m_pINotifyThisNode )
		{
			if( SUCCEEDED ( pJzNotifyNode->m_pINotifyThisNode->QueryInterface( IID_IDMUSProdNotifySink, (void **)&pINotifySink ) ) )
			{
				pINotifySink->OnUpdate( pIChangedDocRootNode, guidUpdateType, pData );
				pINotifySink->Release();
			}
		}
    }

	pFileNode->Release();

	return S_OK;
}


/*======================================================================================
METHOD:  IDMUSPRODFRAMEWORK::ALLOCFILESTREAM
========================================================================================
@method HRESULT | IDMUSProdFramework | AllocFileStream | Returns an IStream interface pointer
		for the file specified in <p bstrFileName>. 

@comm 
	Streams created through either <om IDMUSProdFramework.AllocFileStream> or
	<om IDMUSProdFramework.AllocMemoryStream> implement <i IDMUSProdPersistInfo> for the purpose
	of making available additional information about the stream.  In particular, these methods
	associate a file type and data format with every stream they create.  In addition,
	<om IDMUSProdFramework.AllocFileStream> associates a target directory node with each stream
	having a <p dwDesiredAccess> of GENERIC_READ.
	
	An object can obtain a pointer to a stream's <i IDMUSProdPersistInfo> interface via a call to
	<om IStream::QueryInterface>.  <om IDMUSProdPersistInfo.GetStreamInfo> returns the <p ftFileType>,
	<p guidDataFormat>, and <p pITargetDirectoryNode> associated with a stream.  <p pITargetDirectory>
	will always be NULL for streams created via <om IDMUSProdFramework.AllocMemoryStream>.

	The following file types are valid for use in <p ftFileType>:

	FT_DESIGN: <tab>Design-time saves may include chunks of UI related information only used
	during editing.
	
	FT_RUNTIME: <tab>Runtime saves are invoked to create files for distribution.  Data should
	be saved in its most compact form.    

	FT_UNKNOWN: <tab>FT_UNKNOWN is only valid when <om IDMUSProdFramework::AllocFileStream>
	is called to open a file.
	
	<p guidDataFormat> indicates the specific format to be used when writing data into the
	stream.  Objects may create additional GUIDs to communicate specific data formats they need
	when persisting data.  The <o Framework> supplies the following GUIDs for general use:
	
	GUID_CurrentVersion: <tab><tab>Current version of the file format.

	GUID_CurrentVersion_OnlyUI: <tab>Current version of the file format (UI state information only).

	GUID_DirectMusicObject: <tab><tab>Stream being prepared to persist into a DirectMusic object.

	GUID_Bookmark: <tab><tab><tab>Framework uses this GUID when creating streams to include in bookmarks.

	GUID_AllZeros: <tab><tab><tab>GUID_AllZeros is only valid when <om IDMUSProdFramework::AllocFileStream>
	is called to open a file.

@rvalue S_OK | The IStream interface pointer was returned in <p ppIStream>.
@rvalue E_INVALIDARG | Either <p dwDesiredAccess> or <p ftFileType> is not valid.
		For example, <p dwDesiredAccess> must be GENERIC_READ or GENERIC_WRITE.
@rvalue E_OUTOFMEMORY | Out of memory.
@rvalue E_FAIL | An error occurred, and the file could not be opened.
	
@xref <i IDMUSProdFramework>, <om IDMUSProdFramework.AllocMemoryStream>, <i IDMUSProdPersistInfo>
--------------------------------------------------------------------------------------*/

//////////////////////////////////////////////////////////////////////////////
// CFramework::AllocFileStream

HRESULT CFramework::AllocFileStream(
	BSTR bstrFileName,		// @parm [in] Name of file.  This method frees <p bstrFileName>
							//		with SysFreeString when it is no longer needed.
	DWORD dwDesiredAccess,	// @parm [in] GENERIC_READ or GENERIC_WRITE.
	FileType ftFileType,	// @parm [in] Type of file. FT_DESIGN, FT_RUNTIME, or FT_UNKNOWN.
	GUID guidDataFormat,	// @parm [in] GUID identifying data format of stream.
	IDMUSProdNode* pITargetDirectoryNode,	// @parm [in] <i IDMUSProdNode> interface pointer
							//		to the Project Tree Directory <o Node> that will contain
							//		this file. 
	IStream **ppIStream		// @parm [out,retval] Address of a variable to receive the requested
							//		<i IStream> interface.
							//		If an error occurs, the implementation sets <p ppIStream>
							//		to NULL.  On success, the caller is responsible for calling
							//		<om IStream.Release>.
)
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	CString strFileName = bstrFileName;
	::SysFreeString( bstrFileName );

    return ::AllocFileStream( strFileName, dwDesiredAccess,
							  ftFileType, guidDataFormat, pITargetDirectoryNode, ppIStream );
}


/*======================================================================================
METHOD:  IDMUSPRODFRAMEWORK::ALLOCMEMORYSTREAM
========================================================================================
@method HRESULT | IDMUSProdFramework | AllocMemoryStream | Returns an IStream interface pointer
		for a newly created memory stream. 

@comm 
	Streams created through either <om IDMUSProdFramework.AllocFileStream> or
	<om IDMUSProdFramework.AllocMemoryStream> implement <i IDMUSProdPersistInfo> for the purpose
	of making available additional information about the stream.  In particular, these methods
	associate a file type and data format with every stream they create.  In addition,
	<om IDMUSProdFramework.AllocFileStream> associates a target directory node with each stream
	having a <p dwDesiredAccess> of GENERIC_READ.
	
	An object can obtain a pointer to a stream's <i IDMUSProdPersistInfo> interface via a call to
	<om IStream::QueryInterface>.  <om IDMUSProdPersistInfo.GetStreamInfo> returns the <p ftFileType>,
	<p guidDataFormat>, and <p pITargetDirectoryNode> associated with a stream.  <p pITargetDirectory>
	will always be NULL for streams created via <om IDMUSProdFramework.AllocMemoryStream>.

	The following file types are valid for use in <p ftFileType>:

	FT_DESIGN: <tab>Design-time saves may include chunks of UI related information only used
	during editing.
	
	FT_RUNTIME: <tab>Runtime saves are invoked to create files for distribution.  Data should
	be saved in its most compact form.    
	
	<p guidDataFormat> indicates the specific format to be used when writing data into the
	memory stream.  Objects may create additional GUIDs to communicate specific data formats
	they need when persisting data.  The <o Framework> supplies the following GUIDs for general use:
	
	GUID_CurrentVersion: <tab><tab>Current version of the file format.

	GUID_CurrentVersion_OnlyUI: <tab>Current version of the file format (UI state information only).

	GUID_DirectMusicObject: <tab><tab>Stream being prepared to persist into a DirectMusic object.

	GUID_Bookmark: <tab><tab><tab>Framework uses this GUID when creating streams to include in bookmarks.

@rvalue S_OK | The IStream interface pointer was returned in <p ppIStream>.
@rvalue E_INVALIDARG | <p ftFileType> is not valid.  For example, <p ftFileType> must be
		FT_DESIGN or FT_RUNTIME.
@rvalue E_OUTOFMEMORY | Out of memory.
@rvalue E_FAIL | An error occurred, and the stream could not be created.
	
@xref <i IDMUSProdFramework>, <om IDMUSProdFramework.AllocFileStream>, <i IDMUSProdPersistInfo>
--------------------------------------------------------------------------------------*/

//////////////////////////////////////////////////////////////////////////////
// CFramework::AllocMemoryStream

HRESULT CFramework::AllocMemoryStream(
	FileType ftFileType,	// @parm [in] Type of file. FT_DESIGN or FT_RUNTIME.
	GUID guidDataFormat,	// @parm [in] GUID identifying data format of stream.
	IStream **ppIStream		// @parm [out,retval] Address of a variable to receive the requested
							//		<i IStream> interface.
							//		If an error occurs, the implementation sets <p ppIStream>
							//		to NULL.  On success, the caller is responsible for calling
							//		<om IStream.Release>.
)
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

    return ::AllocMemoryStream( ftFileType, guidDataFormat, ppIStream );
}


/*======================================================================================
METHOD:  IDMUSPRODFRAMEWORK8::GETNODERUNTIMEFILENAME
========================================================================================
*/
/////////////////////////////////////////////////////////////////////////////
// CFramework::GetNodeRuntimeFileName

HRESULT CFramework::GetNodeRuntimeFileName(
	IDMUSProdNode* pINode,	
	BSTR* pbstrRuntimeFileName		
)
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	if( pbstrRuntimeFileName == NULL )
	{
		return E_POINTER;
	}

	*pbstrRuntimeFileName = NULL;

	if( pINode == NULL )
	{
		return E_INVALIDARG;
	}

	GUID guidNodeId;
	if( SUCCEEDED ( pINode->GetNodeId ( &guidNodeId ) ) )
	{
		// Handle Project Folder and Directory Nodes
		if( IsEqualGUID( guidNodeId, GUID_ProjectFolderNode ) 
		||  IsEqualGUID( guidNodeId, GUID_DirectoryNode ) )
		{
			return E_UNEXPECTED;
		}

		// Handle FileNodes
		else if( IsEqualGUID( guidNodeId, GUID_FileNode ) )
		{
			CFileNode* pFileNode = (CFileNode *)pINode;

			*pbstrRuntimeFileName = pFileNode->m_strRuntimeFile.AllocSysString();
			return S_OK;
		}
	}

	// Handle other Nodes
	CComponentDoc* pComponentDoc = theApp.FindComponentDocument( pINode );
	if( pComponentDoc
	&&  pComponentDoc->m_pFileNode )
	{
		*pbstrRuntimeFileName = pComponentDoc->m_pFileNode->m_strRuntimeFile.AllocSysString();
		return S_OK;
	}

	return E_FAIL;
}


/*======================================================================================
METHOD:  IDMUSPRODFRAMEWORK::RESOLVEBESTGUESSWHENLOADFINISHED
========================================================================================
@method HRESULT | IDMUSProdFramework | ResolveBestGuessWhenLoadFinished | Adds <p pINotifySink>
	to a list which will receive FRAMEWORK_FileLoadFinished notifications, and returns a
	Producer generated GUID assigned to the referenced file.

@comm
	The search is confined to the <o Project> containing <p pITreePositionNode>.

    Call this method when <om IDMUSProdFramework.GetBestGuessDocRootNode> returns E_PENDING and
	you wish to be notified when the matching file has finished loading.

	<p pIDocType>, <p bstrNodeName>, and <p pITreePositionNode>  must equal the arguments
	passed to <om IDMUSProdFramework.GetBestGuessDocRootNode> when E_PENDING was returned.

@rvalue S_OK | A Producer generated GUID was returned in <p pguidFile>.
@rvalue E_POINTER | The address in <p pguidFile> is not valid.  For example, it may be NULL.
@rvalue E_INVALIDARG | The address in either <p pINotifySink>, <p bstrNodeName>, <p pIDocType>,
	or <p pITreePositionNode> is not valid.  For example, it may be NULL.
@rvalue E_FAIL | An error occurred, and the GUID could not be returned.

@ex The following example:|

@xref <i IDMUSProdFramework>, <om IDMUSProdFramework.GetBestGuessDocRootNode>, <i IDMUSProdNode>
--------------------------------------------------------------------------------------*/

/////////////////////////////////////////////////////////////////////////////
// CFramework::ResolveBestGuessWhenLoadFinished

HRESULT CFramework::ResolveBestGuessWhenLoadFinished(
	IDMUSProdDocType* pIDocType,		// @parm [in] Pointer to the <i IDMUSProdDocType> interface
										//		associated with the desired type of DocRoot node.
	BSTR bstrNodeName,					// @parm [in] Name of desired DocRoot node.  This method
										//		frees <p bstrNodeName> with SysFreeString when it
										//		is no longer needed.
	IDMUSProdNode* pITreePositionNode,	// @parm [in] Specifies position in Project Tree.  Used by search
										//		algorithm.
	IDMUSProdNotifySink* pINotifySink,	// @parm [in] Pointer to IDMUSProdNotifySink interface
										//		which will be sent a FRAMEWORK_FileLoadFinished notification.
	GUID* pguidFile						// @parm [out,retval] Address of a variable to receive the requested  
										//		GUID.  If an error occurs, the implementation sets
										//		<p pguidFile> to NULL. 
)
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	if( pguidFile )
	{
		*pguidFile = GUID_AllZeros;
	}

	// Validate bstrNodeName
	CString strNodeName;
	if( bstrNodeName == NULL )
	{
		return E_INVALIDARG;
	}
	else
	{
		strNodeName = bstrNodeName;
		::SysFreeString( bstrNodeName );
	}

	// Validate pguidFile
	if( pguidFile == NULL )
	{
		return E_POINTER;
	}

	// Validate pITreePositionNode, pINotifySink, and pIDocType
	if( pIDocType == NULL 
	||  pITreePositionNode == NULL 
	||  pINotifySink == NULL )
	{
		return E_INVALIDARG;
	}

	HRESULT hr = E_FAIL;

	// Does DocRoot exist in a Container that is just now being unpacked?
	IDMUSProdComponent* pIComponent;
	if( SUCCEEDED ( FindComponent( CLSID_ContainerComponent,  &pIComponent ) ) )
	{
		IDMUSProdUnpackingFiles* pIUnpackingFiles;
		if( SUCCEEDED ( pIComponent->QueryInterface( IID_IDMUSProdUnpackingFiles, (void **)&pIUnpackingFiles ) ) )
		{
			IUnknown* pIUnknown;
			BSTR bstrName = strNodeName.AllocSysString();
			hr = pIUnpackingFiles->GetDocRootOfEmbeddedFile( pIDocType, bstrName, &pIUnknown );
			if( SUCCEEDED ( hr ) )
			{
				pIUnknown->Release();
			}
			else if( hr == E_PENDING )
			{
				// File is being loaded so add to pINotifySink to file's notification list
				BSTR bstrName = strNodeName.AllocSysString();
				pIUnpackingFiles->AddToNotifyWhenLoadFinished( pIDocType, bstrName, pINotifySink, pguidFile );
			}

			pIUnpackingFiles->Release();
		}

		pIComponent->Release();
	}
	if( SUCCEEDED(hr) )
	{
		// File is already loaded
		return E_FAIL;
	}
	else if( hr == E_PENDING )
	{
		// File is in process of being loaded
		return S_OK;
	}

	// Does DocRoot exist in a Sibg that is just now being unpacked?
	/*
	if( SUCCEEDED ( FindComponent( CLSID_SongComponent,  &pIComponent ) ) )
	{
		IDMUSProdUnpackingFiles* pIUnpackingFiles;
		if( SUCCEEDED ( pIComponent->QueryInterface( IID_IDMUSProdUnpackingFiles, (void **)&pIUnpackingFiles ) ) )
		{
			IUnknown* pIUnknown;
			BSTR bstrName = strNodeName.AllocSysString();
			hr = pIUnpackingFiles->GetDocRootOfEmbeddedFile( pIDocType, bstrName, &pIUnknown );
			if( SUCCEEDED ( hr ) )
			{
				pIUnknown->Release();
			}
			else if( hr == E_PENDING )
			{
				// File is being loaded so add to pINotifySink to file's notification list
				BSTR bstrName = strNodeName.AllocSysString();
				pIUnpackingFiles->AddToNotifyWhenLoadFinished( pIDocType, bstrName, pINotifySink, pguidFile );
			}

			pIUnpackingFiles->Release();
		}

		pIComponent->Release();
	}
	if( SUCCEEDED(hr) )
	{
		// File is already loaded
		return E_FAIL;
	}
	else if( hr == E_PENDING )
	{
		// File is in process of being loaded
		return S_OK;
	}
	*/

	// The file is not in a Container or Song that is being unpacked
	hr = E_FAIL;

	// Get the Project
	IDMUSProdProject* pIProject;
	if( SUCCEEDED ( theApp.m_pFramework->FindProject( pITreePositionNode, &pIProject ) ) )
	{
		CProject* pProject = (CProject *)pIProject;

		// Look for an appropriate FileNode
		CFileNode* pFileNode = pProject->GetBestGuessDocRootNode( pIDocType, strNodeName, pITreePositionNode );
		if( pFileNode )
		{
			// Make sure file is in process of being loaded
			if( pFileNode->m_pIChildNode == NULL 
			&&  pFileNode->m_fInOnOpenDocument )
			{
				// File is being loaded so add to pINotifySink to file's notification list
				pFileNode->AddToNotifyWhenLoadFinished( pINotifySink );
				*pguidFile = pFileNode->m_guid;
				hr = S_OK;
			}

			pFileNode->Release();
		}

		pIProject->Release();
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CFramework::StartWaitCursor

HRESULT CFramework::StartWaitCursor( void )
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	CMainFrame* pMainFrame = (CMainFrame *)theApp.m_pMainWnd;
	ASSERT( pMainFrame != NULL );

	if( pMainFrame->m_pWaitCursor == NULL )
	{
		pMainFrame->m_pWaitCursor = new CJzWaitCursor( pMainFrame, &pMainFrame->m_wndStatusBar );
		if( pMainFrame->m_pWaitCursor == NULL )
		{
			return E_OUTOFMEMORY;
		}
	}

	pMainFrame->m_pWaitCursor->StartWait();

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CFramework::EndWaitCursor

HRESULT CFramework::EndWaitCursor( void )
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	CMainFrame* pMainFrame = (CMainFrame *)theApp.m_pMainWnd;
	ASSERT( pMainFrame != NULL );

	if( pMainFrame->m_pWaitCursor )
	{
		if( pMainFrame->m_pWaitCursor->EndWait() == 0 )	// CJzWaitCursor was deleted
		{
			pMainFrame->m_pWaitCursor = NULL;
		}
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CFramework::ReadTheRefChunk

HRESULT CFramework::ReadTheRefChunk( IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain, IDMUSProdNode** ppIDocRootNode )
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

    IStream* pIStream;
    HRESULT hr;
	DWORD dwByteCount;
	DWORD dwSize;
	CFileNode* pFileNode;
	ioFileRef iFileRef;

    pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	if( ppIDocRootNode == NULL )
	{
		hr = E_POINTER;
		goto ON_ERROR;
	}

	*ppIDocRootNode = NULL;

	if( pIRiffStream == NULL
	||  pckMain == NULL )
	{
		hr = E_INVALIDARG;
		goto ON_ERROR;
	}

	// Initialize ioFileRef structure
	memset( &iFileRef, 0, sizeof(ioFileRef) );

    dwSize = min( pckMain->cksize, sizeof( ioFileRef ) );

	// Read the FileRef chunk data
    hr = pIStream->Read( &iFileRef, dwSize, &dwByteCount );
    if( FAILED( hr )
	||  dwByteCount != dwSize )
	{
		goto ON_ERROR;
	}

	hr = E_FAIL;
	pFileNode = NULL;

	// Get the referenced file's FileNode
	pFileNode = theApp.GetFileByGUID( iFileRef.guidFile );

	// We found the referenced file's FileNode
	if( pFileNode )
	{
		// Make sure the FileNode's file is loaded
		if( pFileNode->m_pIChildNode == NULL )
		{
			if( pFileNode->m_fInOnOpenDocument )
			{
				hr = E_PENDING;
			}
			else
			{
				CString strFileName;
				pFileNode->ConstructFileName( strFileName );
				
				// Open the file
				theApp.m_nShowNodeInTree++;
				theApp.OpenTheFile( strFileName, TGT_FILENODE );
				theApp.m_nShowNodeInTree--;
			}
		}

		if( pFileNode->m_pIChildNode )
		{
			pFileNode->m_pIChildNode->AddRef();
			*ppIDocRootNode = pFileNode->m_pIChildNode;
			hr = S_OK;
		}

		pFileNode->Release();
	}

ON_ERROR:
    pIStream->Release();

    return hr;
}


/*======================================================================================
METHOD:  IDMUSPRODFILEREFCHUNK::LOADREFCHUNK
========================================================================================
@method HRESULT | IDMUSProdFileRefChunk | LoadRefChunk | Reads a DirectMusic Producer file
		reference RIFF chunk, loads the referenced file, and returns a pointer to the
		referenced file's DocRoot node in <p ppIDocRootNode>. 

@comm
	A DirectMusic Producer <o Component> can call <om IDMUSProdFramework::QueryInterface>
	to obtain a pointer to the <o Framework>'s <i IDMUSProdFileRefChunk> interface.

	Call <om IDMUSProdFileRefChunk.SaveRefChunk> to create the DirectMusic Producer file
	reference RIFF chunk loaded by <om IDMUSProdFileRefChunk.LoadRefChunk>.

@rvalue S_OK | The top node of the referenced file was returned in <p ppIDocRootNode>.
@rvalue E_POINTER | The address in <p ppIDocRootNode> is not valid.  For example, it may be NULL.
@rvalue E_INVALIDARG | The value of <p pIStream> is not valid.  For example, it may be NULL.
@rvalue E_PENDING | The referenced file is in the process of loading and its top node could not
	be returned.
@rvalue E_FAIL | An error occurred, and the top node of the refrenced file could not be returned.

@ex The following example reads a DirectMusic Producer file reference chunk and creates a
	reference <o Node>: |

	const GUID GUID_StyleRefNode = {0x408FBB21,0xB009,0x11D0,{0x89,0xAD,0x00,0xA0,0xC9,0x05,0x41,0x29}}; 
	const GUID GUID_StyleNode = {0xFAE21E45,0xA51A,0x11D0,{0x89,0xAD,0x00,0xA0,0xC9,0x05,0x41,0x29}}; 
	const CLSID CLSID_StyleComponent = {0x44207721,0x487B,0x11d0,{0x89,0xAC,0x00,0xA0,0xC9,0x05,0x41,0x29}}; 

	IDMUSProdNode* m_pIStyleRefNode;	// member variable storing pointer to reference Node

    IDMUSProdFramework* pIFramework;
	IDMUSProdFileRefChunk* pIFileRefChunk;
	IStream* pIStream;
	IDMUSProdNode* pINode;

	IDMUSProdNode* pIDocRootNode = NULL;

	// Get an IDMUSProdNode* pointer to the top node of the referenced file
    if( SUCCEEDED ( pIFramework->QueryInterface( IID_IDMUSProdFileRefChunk, (void **)&pIFileRefChunk ) ) )
	{
		if( FAILED ( pIFileRefChunk->LoadRefChunk( pIStream, &pIDocRootNode ) ) )
		{
			if( AfxMessageBox( IDS_BROWSE_FOR_FILE, MB_YESNO ) == IDYES )
			{
				IDMUSProdDocType* pIDocType;

				if( SUCCEEDED ( pIFramework->FindDocTypeByNodeId( GUID_StyleNode, &pIDocType ) ) )
				{
					pIFramework->OpenFile( pIDocType, NULL, pINode, &pIDocRootNode );
					pIDocType->Release();
				}
			}
		}

		pIFileRefChunk->Release();
	}

	// Create a reference node for the referenced file
    if( pIDocRootNode )
	{
		IDMUSProdComponent* pIComponent;

		if( SUCCEEDED ( pIFramework->FindComponent( CLSID_StyleComponent, &pIComponent ) ) )
		{
			if( SUCCEEDED ( pIComponent->AllocReferenceNode( GUID_StyleRefNode, &m_pIStyleRefNode ) ) )
			{
				IDMUSProdReferenceNode* pIReferenceNode;

				if( SUCCEEDED ( m_pIStyleRefNode->QueryInterface( IID_IDMUSProdReferenceNode, (void **)&pIReferenceNode ) ) )
				{
					pIReferenceNode->SetReferencedFile( pIDocRootNode );
					pIReferenceNode->Release();
				}
			}
			pIComponent->Release();
		}
	}

  
@xref <o Node> Object, <i IDMUSProdFileRefChunk>, <om IDMUSProdFileRefChunk.SaveRefChunk>, <om IDMUSProdFileRefChunk.GetChunkSize>, <om IDMUSProdFramework.OpenFile>, <i IDMUSProdReferenceNode>
--------------------------------------------------------------------------------------*/

/////////////////////////////////////////////////////////////////////////////
// CFramework::LoadRefChunk

HRESULT CFramework::LoadRefChunk(
	IStream* pIStream,				// @parm [in] Pointer to IStream interface.
	IDMUSProdNode** ppIDocRootNode	// @parm [out,retval] Address of a variable to receive the requested  
									//		<i IDMUSProdNode> interface.  If an error occurs, the implementation
									//		sets <p ppIDocRootNode> to NULL.  On success, the caller
									//		is responsible for calling <om IDMUSProdNode.Release> when
									//		this pointer is no longer needed.
)
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

    IDMUSProdRIFFStream* pIRiffStream;
    MMCKINFO ckMain;

	if( ppIDocRootNode == NULL )
	{
		return E_POINTER;
	}

	*ppIDocRootNode = NULL;

	if( pIStream == NULL )
	{
		return E_INVALIDARG;
	}

	HRESULT hr = E_FAIL;

    if( SUCCEEDED( AllocRIFFStream( pIStream, &pIRiffStream ) ) )
    {
        ckMain.ckid = FOURCC_DMUSPROD_FILEREF;

        if( pIRiffStream->Descend( &ckMain, NULL, MMIO_FINDCHUNK ) == 0 )
        {
            hr = ReadTheRefChunk( pIRiffStream, &ckMain, ppIDocRootNode );
        }

        pIRiffStream->Release();
    }

    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CFramework::WriteTheRefChunk

HRESULT CFramework::WriteTheRefChunk( IDMUSProdRIFFStream* pIRiffStream, CFileNode* pFileNode )
{
    IStream* pIStream;
    HRESULT hr;
    MMCKINFO ck;
	DWORD dwBytesWritten;
	CString strRelativePath;
	ioFileRef oFileRef;

    pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	if( pIRiffStream == NULL
	||  pFileNode == NULL )
	{
		hr = E_INVALIDARG;
		goto ON_ERROR;
	}

	// Get the referenced file's relative path
	if( pFileNode->ConstructRelativePath( strRelativePath ) )
	{
		strRelativePath = _T("..\\") + strRelativePath;
	}
	if( strRelativePath.IsEmpty() )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Append the referenced file's name to the relative path
	BSTR bstrFileName;

	hr = pFileNode->GetNodeName( &bstrFileName );
	if( FAILED ( hr ) )
	{
		goto ON_ERROR;
	}
	if( strRelativePath.Right(1) != _T("\\") )
	{
		strRelativePath += _T("\\");
	}
	strRelativePath += CString(bstrFileName);
	::SysFreeString( bstrFileName );

	// Make sure the FileNode has a DocRoot Node
	if( pFileNode->m_pIChildNode == NULL )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Get the DocRoot's Node ID
	GUID guidDocRootNodeId;

	if( FAILED ( pFileNode->m_pIChildNode->GetNodeId ( &guidDocRootNodeId ) ) )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Write FileRef chunk header
	ck.ckid = FOURCC_DMUSPROD_FILEREF;
	if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Prepare ioFileRef structure
	memset( &oFileRef, 0, sizeof(ioFileRef) );
	memcpy( &oFileRef.guidFile, &pFileNode->m_guid, sizeof( oFileRef.guidFile ) );
	memcpy( &oFileRef.guidDocRootNodeId, &guidDocRootNodeId, sizeof( oFileRef.guidDocRootNodeId ) );

	// Use file GUID from new Project if in the middle of 'Duplicate Project'
	if( theApp.m_fInDuplicateProject )
	{
		GUID guidNewFile;

		if( theApp.GetNewGUIDForDuplicateFile( pFileNode->m_guid, &guidNewFile ) )
		{
			memcpy( &oFileRef.guidFile, &guidNewFile, sizeof( oFileRef.guidFile ) );
		}
	}

	// Write FileRef chunk data 
	hr = pIStream->Write( &oFileRef, sizeof(ioFileRef), &dwBytesWritten);
	if( FAILED( hr )
	||  dwBytesWritten != sizeof(ioFileRef) )
	{
        hr = E_FAIL;
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


/*======================================================================================
METHOD:  IDMUSPRODFILEREFCHUNK::SAVEREFCHUNK
========================================================================================
@method HRESULT | IDMUSProdFileRefChunk| SaveRefChunk | Saves a DirectMusic Producer file
		reference RIFF chunk. 

@comm
	A DirectMusic Producer <o Component> can call <om IDMUSProdFramework::QueryInterface>
	to obtain a pointer to the <o Framework>'s <i IDMUSProdFileRefChunk> interface.

	Call <om IDMUSProdFileRefChunk.LoadRefChunk> to load the DirectMusic Producer file
	reference RIFF chunk created by <om IDMUSProdFileRefChunk.SaveRefChunk>.

@rvalue S_OK | The DirectMusic Producer file reference RIFF chunk was saved successfully.
@rvalue E_INVALIDARG | Either <p pIStream> or <p pIDocRootNode> is not valid.  For example,
		they may be NULL.
@rvalue E_FAIL | An error occurred, and the file reference chunk could not be saved.

@ex The following code excerpt handles creation of file reference chunks correctly.  Reference
	chunks are saved differently when persisting directly into a DirectMusic object because Producer's
	implementation of IDirectMusicLoader will be called upon to resolve the reference.  When saving to
	a file, the file type (FT_RUNTIME, FT_DESIGN) returned by <om IDMUSProdPersistInfo.GetStreamInfo> 
	determines whether or not it is necessary to save the Producer specific file reference chunk: |


	// Write Reference chunk
	if( pIStyleDocRootNode )
	{
		if( ::IsEqualGUID( guidDataFormat, GUID_DirectMusicObject ) )
		{
			SaveDMRef( pIRiffStream, pIStyleDocRootNode, WL_PRODUCER );
		}
		else if( ::IsEqualGUID( guidDataFormat, GUID_CurrentVersion ) )
		{
			SaveDMRef( pIRiffStream, pIStyleDocRootNode, WL_DIRECTMUSIC );
			if( ftFileType == FT_DESIGN )
			{
				SaveProducerRef( pIRiffStream, pIStyleDocRootNode );
			}
		}
	}


HRESULT CStyleRefMgr::SaveProducerRef( IDMUSProdRIFFStream* pIRiffStream, IDMUSProdNode* pIDocRootNode )
{
	IDMUSProdFileRefChunk* pIFileRefChunk;

	ASSERT( m_pIFramework != NULL );
	if( m_pIFramework == NULL )
	{
		return E_FAIL;
	}

    IStream* pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	if ( SUCCEEDED ( m_pIFramework->QueryInterface( IID_IDMUSProdFileRefChunk, (void**)&pIFileRefChunk ) ) )
	{
		pIFileRefChunk->SaveRefChunk( pIStream, pIDocRootNode );
		pIFileRefChunk->Release();
	}

	pIStream->Release();
	return S_OK;
}


HRESULT CStyleRefMgr::SaveDMRef( IDMUSProdRIFFStream* pIRiffStream,
								 IDMUSProdNode* pIDocRootNode, WhichLoader whichLoader )
{
	IDMUSProdLoaderRefChunk* pIRefChunkLoader;

	ASSERT( m_pIFramework != NULL );
	if( m_pIFramework == NULL )
	{
		return E_FAIL;
	}

	HRESULT hr = E_FAIL;

    IStream* pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	if( SUCCEEDED( m_pIFramework->QueryInterface( IID_IDMUSProdLoaderRefChunk, (void**)&pIRefChunkLoader ) ) )
	{
		if( pIRefChunkLoader )
		{
			switch( whichLoader )
			{
				case WL_DIRECTMUSIC:
				case WL_PRODUCER:
					hr = pIRefChunkLoader->SaveRefChunkForLoader( pIStream,
																  pIDocRootNode,
																  CLSID_DirectMusicStyle,
																  NULL,
																  whichLoader );
					break;
			}

			pIRefChunkLoader->Release();
		}
	}

	pIStream->Release();
	return hr;
}
	
@xref <i IDMUSProdLoaderRefChunk>, <i IDMUSProdFileRefChunk>, <om IDMUSProdFileRefChunk.LoadRefChunk>, <om IDMUSProdFileRefChunk.GetChunkSize>, <om IDMUSProdFramework.OpenFile>, <i IDMUSProdReferenceNode>
--------------------------------------------------------------------------------------*/

/////////////////////////////////////////////////////////////////////////////
// CFramework::SaveRefChunk

HRESULT CFramework::SaveRefChunk(
	IStream* pIStream,				// @parm [in] Pointer to IStream interface.
	IDMUSProdNode* pIDocRootNode	// @parm [in] Pointer to an <i IDMUSProdNode> interface for the DocRoot
									//		<o Node> of the referenced file.
)
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	if( pIStream == NULL
	||  pIDocRootNode == NULL )
	{
		return E_INVALIDARG;
	}

	HRESULT hr;

	CFileNode* pFileNode = NULL;
    IDMUSProdRIFFStream* pIRiffStream = NULL;
	CComponentDoc* pComponentDoc;

	// Get the DocRoot's FileNode
	pComponentDoc = theApp.FindComponentDocument( pIDocRootNode );
	if( pComponentDoc )
	{
		if( pComponentDoc->m_pIDocRootNode == pIDocRootNode )
		{
			pComponentDoc->m_pFileNode->AddRef();
			pFileNode = pComponentDoc->m_pFileNode;
		}
	}
	if( pFileNode == NULL )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}
	
	// Create a RIFFStream
    hr = AllocRIFFStream( pIStream, &pIRiffStream );
	if( FAILED ( hr ) )
	{
		goto ON_ERROR;
	}

	// Save the DirectMusic Producer Reference Chunk
    hr = WriteTheRefChunk( pIRiffStream, pFileNode );

ON_ERROR:
	if( pIRiffStream )
	{
		pIRiffStream->Release();
	}

	if( pFileNode )
	{
		pFileNode->Release();
	}

	return hr;
}


/*======================================================================================
METHOD:  IDMUSPRODFILEREFCHUNK::GETCHUNKSIZE
========================================================================================
@method HRESULT | IDMUSProdFileRefChunk| GetChunkSize | Returns the size of a DirectMusic
	Producer file reference RIFF chunk.

@rvalue S_OK | The chunk size was returned in <p pdwSize>.
@rvalue E_POINTER | <p pdwSize> is not valid.  For example, it may be NULL.
	
@xref <i IDMUSProdFileRefChunk>, <i IDMUSProdReferenceNode>
--------------------------------------------------------------------------------------*/

/////////////////////////////////////////////////////////////////////////////
// CFramework::GetChunkSize

HRESULT CFramework::GetChunkSize(
	DWORD* pdwSize		// @parm [out,retval] Pointer to size of stream needed to save the
						//		chunk.
)
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	if( pdwSize == NULL )
	{
		return E_POINTER;
	}

	*pdwSize = 8 + sizeof( ioFileRef );

	return S_OK;
}


/*======================================================================================
METHOD:  IDMUSPRODFILEREFCHUNK::RESOLVEWHENLOADFINISHED
========================================================================================
@method HRESULT | IDMUSProdFileRefChunk | ResolveWhenLoadFinished | Adds <p pINotifySink>
	to a list which will receive FRAMEWORK_FileLoadFinished notifications, reads a DirectMusic
	Producer file reference RIFF chunk, and returns the Producer generated GUID assigned
	to the referenced file. 

@comm
	A DirectMusic Producer <o Component> can call <om IDMUSProdFramework::QueryInterface>
	to obtain a pointer to the <o Framework>'s <i IDMUSProdFileRefChunk> interface.

	Call <om IDMUSProdFileRefChunk.SaveRefChunk> to create the DirectMusic Producer file
	reference RIFF chunk loaded by <om IDMUSProdFileRefChunk.LoadRefChunk>.

@rvalue S_OK | The Producer generated GUID was returned in <p pguidFile>.
@rvalue E_POINTER | The address in <p pguidFile> is not valid.  For example, it may be NULL.
@rvalue E_INVALIDARG | The value of <p pIStream> is not valid.  For example, it may be NULL.
@rvalue E_FAIL | An error occurred, and the GUID could not be returned.
  
@xref <o Node> Object, <i IDMUSProdFileRefChunk>, <om IDMUSProdFileRefChunk.SaveRefChunk>, <om IDMUSProdFileRefChunk.GetChunkSize>, <om IDMUSProdFramework.OpenFile>, <i IDMUSProdReferenceNode>
--------------------------------------------------------------------------------------*/

/////////////////////////////////////////////////////////////////////////////
// CFramework::ResolveWhenLoadFinished

HRESULT CFramework::ResolveWhenLoadFinished(
	IStream* pIStream,					// @parm [in] Pointer to IStream interface.
	IDMUSProdNotifySink* pINotifySink,	// @parm [in] Pointer to IDMUSProdNotifySink interface
										//		which will be sent a FRAMEWORK_FileLoadFinished notification.
	GUID* pguidFile						// @parm [out,retval] Address of a variable to receive the requested  
										//		GUID.  If an error occurs, the implementation sets
										//		<p pguidFile> to NULL. 
)
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

    IDMUSProdRIFFStream* pIRiffStream;
    MMCKINFO ckMain;

	if( pguidFile == NULL )
	{
		return E_POINTER;
	}

	*pguidFile = GUID_AllZeros;

	if( pIStream == NULL 
	||  pINotifySink == NULL )
	{
		return E_INVALIDARG;
	}

	HRESULT hr = E_FAIL;

    if( SUCCEEDED( AllocRIFFStream( pIStream, &pIRiffStream ) ) )
    {
        ckMain.ckid = FOURCC_DMUSPROD_FILEREF;

        if( pIRiffStream->Descend( &ckMain, NULL, MMIO_FINDCHUNK ) == 0 )
        {
			DWORD dwByteCount;
			ioFileRef iFileRef;

			DWORD dwSize = min( ckMain.cksize, sizeof( ioFileRef ) );

			// Read the FileRef chunk data
			if( SUCCEEDED ( pIStream->Read( &iFileRef, dwSize, &dwByteCount ) ) )
			{
				if( dwByteCount == dwSize )
				{
					// Get the referenced file's FileNode
					CFileNode* pFileNode = theApp.GetFileByGUID( iFileRef.guidFile );
					if( pFileNode )
					{
						pFileNode->AddToNotifyWhenLoadFinished( pINotifySink );
						*pguidFile = iFileRef.guidFile;
						hr = S_OK;

						pFileNode->Release();
					}
				}
			}
        }

        pIRiffStream->Release();
    }

    return hr;
}


/*======================================================================================
METHOD:  IDMUSPRODFILEREFCHUNK::GETFILEREFINFO
========================================================================================
@method HRESULT | IDMUSProdFileRefChunk | GetFileRefInfo | Reads a DirectMusic Producer
	file reference RIFF chunk, and fills a <t DMUSProdFileRefInfo> structure with information
	about the reference. 

@comm
	A DirectMusic Producer <o Component> can call <om IDMUSProdFramework::QueryInterface>
	to obtain a pointer to the <o Framework>'s <i IDMUSProdFileRefChunk> interface.

	Call <om IDMUSProdFileRefChunk.SaveRefChunk> to create the DirectMusic Producer file
	reference RIFF chunk loaded by <om IDMUSProdFileRefChunk.LoadRefChunk>.

@rvalue S_OK | Information about the reference was returned in the <t DMUSProdFileRefInfo> structure.
@rvalue E_POINTER | The address in <p pFileRefInfo> is not valid.  For example, it may be NULL.
@rvalue E_INVALIDARG | The value of <p pIStream> is not valid.  For example, it may be NULL.
@rvalue E_FAIL | An error occurred, and the <t DMUSProdFileRefInfo> structure could not be populated.
  
@xref <o Node> Object, <i IDMUSProdFileRefChunk>, <om IDMUSProdFileRefChunk.SaveRefChunk>, <om IDMUSProdFileRefChunk.GetChunkSize>, <om IDMUSProdFramework.OpenFile>, <i IDMUSProdReferenceNode>
--------------------------------------------------------------------------------------*/

/////////////////////////////////////////////////////////////////////////////
// CFramework::GetFileRefInfo

HRESULT CFramework::GetFileRefInfo(
	IStream* pIStream,					// @parm [in] Pointer to IStream interface.
	DMUSProdFileRefInfo* pFileRefInfo	// @parm [in] [in] Pointer to a DMUSProdFileRefInfo
										//		structure.
)
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

    IDMUSProdRIFFStream* pIRiffStream;
    MMCKINFO ckMain;

	if( pFileRefInfo == NULL )
	{
		return E_POINTER;
	}

	if( pIStream == NULL
	||  pFileRefInfo->wSize == 0 )
	{
		return E_INVALIDARG;
	}

	pFileRefInfo->guidFile = GUID_AllZeros;
	pFileRefInfo->guidDocRootNodeId = GUID_AllZeros;

	HRESULT hr = E_FAIL;

    if( SUCCEEDED( AllocRIFFStream( pIStream, &pIRiffStream ) ) )
    {
        ckMain.ckid = FOURCC_DMUSPROD_FILEREF;

        if( pIRiffStream->Descend( &ckMain, NULL, MMIO_FINDCHUNK ) == 0 )
        {
			DWORD dwByteCount;
			ioFileRef iFileRef;

			DWORD dwSize = min( ckMain.cksize, sizeof( ioFileRef ) );

			// Read the FileRef chunk data
			if( SUCCEEDED ( pIStream->Read( &iFileRef, dwSize, &dwByteCount ) ) )
			{
				if( dwByteCount == dwSize )
				{
					pFileRefInfo->guidFile = iFileRef.guidFile;
					pFileRefInfo->guidDocRootNodeId = iFileRef.guidDocRootNodeId;
					hr = S_OK;
				}
			}
        }

        pIRiffStream->Release();
    }

    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CFramework::WriteTheRefChunkForLoader

HRESULT CFramework::WriteTheRefChunkForLoader( IDMUSProdRIFFStream* pIRiffStream, LPDMUS_OBJECTDESC pObjectDesc )
{
    IStream* pIStream;
    HRESULT hr;
    MMCKINFO ckMain;
    MMCKINFO ck;
	DWORD dwBytesWritten;
	DWORD dwLength;

    pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	if( pIRiffStream == NULL
	||  pObjectDesc == NULL )
	{
		hr = E_INVALIDARG;
		goto ON_ERROR;
	}

	// Write REF LIST header
	ckMain.fccType = DMUS_FOURCC_REF_LIST;
	if( pIRiffStream->CreateChunk(&ckMain, MMIO_CREATELIST) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Write REF chunk
	{
		DMUS_IO_REFERENCE dmusReference;

		ck.ckid = DMUS_FOURCC_REF_CHUNK;
		if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}

		// Prepare DMUS_IO_REFERENCE structure
		memset( &dmusReference, 0, sizeof(DMUS_IO_REFERENCE) );
		memcpy( &dmusReference.guidClassID, &pObjectDesc->guidClass, sizeof(GUID) );
		dmusReference.dwValidData = pObjectDesc->dwValidData;

		// Write REF chunk data 
		hr = pIStream->Write( &dmusReference, sizeof(DMUS_IO_REFERENCE), &dwBytesWritten);
		if( FAILED( hr )
		||  dwBytesWritten != sizeof(DMUS_IO_REFERENCE) )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}

		if( pIRiffStream->Ascend(&ck, 0) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}
	}

	// Write GUID chunk
	if( pObjectDesc->dwValidData & DMUS_OBJ_OBJECT )
	{
		ck.ckid = DMUS_FOURCC_GUID_CHUNK;
		if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}

		hr = pIStream->Write( &pObjectDesc->guidObject, sizeof(GUID), &dwBytesWritten);
		if( FAILED( hr )
		||  dwBytesWritten != sizeof(GUID) )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}

		if( pIRiffStream->Ascend(&ck, 0) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}
	}

	// Write Date chunk
	if( pObjectDesc->dwValidData & DMUS_OBJ_DATE )
	{
		ck.ckid = DMUS_FOURCC_DATE_CHUNK;
		if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}

		hr = pIStream->Write( &pObjectDesc->ftDate, sizeof(FILETIME), &dwBytesWritten);
		if( FAILED( hr )
		||  dwBytesWritten != sizeof(FILETIME) )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}

		if( pIRiffStream->Ascend(&ck, 0) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}
	}

	// Write Name chunk
	if( pObjectDesc->dwValidData & DMUS_OBJ_NAME )
	{
		ck.ckid = DMUS_FOURCC_NAME_CHUNK;
		if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}

		dwLength = wcslen( pObjectDesc->wszName ) + 1; 
		dwLength *= sizeof( wchar_t );
		hr = pIStream->Write( &pObjectDesc->wszName, dwLength, &dwBytesWritten);
		if( FAILED( hr )
		||  dwBytesWritten != dwLength )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}

		if( pIRiffStream->Ascend(&ck, 0) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}
	}

	// Write File chunk
	if( pObjectDesc->dwValidData & DMUS_OBJ_FILENAME )
	{
		ck.ckid = DMUS_FOURCC_FILE_CHUNK;
		if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}

		dwLength = wcslen( pObjectDesc->wszFileName ) + 1; 
		dwLength *= sizeof( wchar_t );
		hr = pIStream->Write( &pObjectDesc->wszFileName, dwLength, &dwBytesWritten);
		if( FAILED( hr )
		||  dwBytesWritten != dwLength )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}

		if( pIRiffStream->Ascend(&ck, 0) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}
	}

	// Write Category chunk
	if( pObjectDesc->dwValidData & DMUS_OBJ_CATEGORY )
	{
		ck.ckid = DMUS_FOURCC_CATEGORY_CHUNK;
		if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}

		dwLength = wcslen( pObjectDesc->wszCategory ) + 1;
		dwLength *= sizeof( wchar_t );
		hr = pIStream->Write( &pObjectDesc->wszCategory, dwLength, &dwBytesWritten);
		if( FAILED( hr )
		||  dwBytesWritten != dwLength )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}

		if( pIRiffStream->Ascend(&ck, 0) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}
	}

	// Write Version chunk
	if( pObjectDesc->dwValidData & DMUS_OBJ_VERSION )
	{
		DMUS_IO_VERSION dmusVersion;

		ck.ckid = DMUS_FOURCC_VERSION_CHUNK;
		if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}

		// Prepare DMUS_IO_VERSION structure
		memset( &dmusVersion, 0, sizeof(DMUS_IO_VERSION) );

		dmusVersion.dwVersionMS = pObjectDesc->vVersion.dwVersionMS ;
		dmusVersion.dwVersionLS = pObjectDesc->vVersion.dwVersionLS;

		// Write Version chunk data
		hr = pIStream->Write( &dmusVersion, sizeof(DMUS_IO_VERSION), &dwBytesWritten);
		if( FAILED( hr )
		||  dwBytesWritten != sizeof(DMUS_IO_VERSION) )
		{
			hr = E_FAIL;
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


/*======================================================================================
METHOD:  IDMUSPRODLOADERREFCHUNK::SAVEREFCHUNKFORLOADER
========================================================================================
@method HRESULT | IDMUSProdLoaderRefChunk| SaveRefChunkForLoader | Embeds a RIFF chunk
		into <p pIStream> that enables an implementation of IDirectMusicLoader to
		resolve file references.

@comm
	A DirectMusic Producer Component can call <om IDMUSProdFramework::QueryInterface> to
	obtain a pointer to the <o Framework>'s <i IDMUSProdLoaderRefChunk> interface.	

	<p pObjectDesc> is required when <p pIDocRootNode's> IDMUSProdNode::GetObject method 
	fails to return an IDirectMusicObject interface pointer.  
	
	The following values are valid for use in <p wlWhichLoader>:

	WL_DIRECTMUSIC: <tab>The Framework creates a file reference chunk with information
	that the DirectMusic Loader object can use to resolve the file reference.  Use this
	value when saving runtime files to disk.

	WL_PRODUCER: <tab>The Framework creates a file reference chunk with information
	that the Producer implementation of IDirectMusicLoader can use to resolve the file
	reference.  Use this value when persisting into DirectMusic objects during the course
	of a DirectMusic Producer work session.

@rvalue S_OK | The file reference chunk was saved successfully.
@rvalue E_INVALIDARG | Either <p pIStream> or <p pIDocRootNode> is not valid.  For example,
		they may be NULL.
@rvalue E_FAIL | An error occurred, and the file reference chunk could not be saved.

@ex The following code excerpt handles creation of file reference chunks correctly.  Reference
	chunks are saved differently when persisting directly into a DirectMusic object because Producer's
	implementation of IDirectMusicLoader will be called upon to resolve the reference.  When saving to
	a file, the file type (FT_RUNTIME, FT_DESIGN) returned by <om IDMUSProdPersistInfo.GetStreamInfo> 
	determines whether or not it is necessary to save the Producer specific file reference chunk: |


	// Write Reference chunk
	if( pIStyleDocRootNode )
	{
		if( ::IsEqualGUID( guidDataFormat, GUID_DirectMusicObject ) )
		{
			SaveDMRef( pIRiffStream, pIStyleDocRootNode, WL_PRODUCER );
		}
		else if( ::IsEqualGUID( guidDataFormat, GUID_CurrentVersion ) )
		{
			SaveDMRef( pIRiffStream, pIStyleDocRootNode, WL_DIRECTMUSIC );
			if( ftFileType == FT_DESIGN )
			{
				SaveProducerRef( pIRiffStream, pIStyleDocRootNode );
			}
		}
	}


HRESULT CStyleRefMgr::SaveProducerRef( IDMUSProdRIFFStream* pIRiffStream, IDMUSProdNode* pIDocRootNode )
{
	IDMUSProdFileRefChunk* pIFileRefChunk;

	ASSERT( m_pIFramework != NULL );
	if( m_pIFramework == NULL )
	{
		return E_FAIL;
	}

    IStream* pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	if ( SUCCEEDED ( m_pIFramework->QueryInterface( IID_IDMUSProdFileRefChunk, (void**)&pIFileRefChunk ) ) )
	{
		pIFileRefChunk->SaveRefChunk( pIStream, pIDocRootNode );
		pIFileRefChunk->Release();
	}

	pIStream->Release();
	return S_OK;
}


HRESULT CStyleRefMgr::SaveDMRef( IDMUSProdRIFFStream* pIRiffStream,
								 IDMUSProdNode* pIDocRootNode, WhichLoader whichLoader )
{
	IDMUSProdLoaderRefChunk* pIRefChunkLoader;

	ASSERT( m_pIFramework != NULL );
	if( m_pIFramework == NULL )
	{
		return E_FAIL;
	}

	HRESULT hr = E_FAIL;

    IStream* pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	if( SUCCEEDED( m_pIFramework->QueryInterface( IID_IDMUSProdLoaderRefChunk, (void**)&pIRefChunkLoader ) ) )
	{
		if( pIRefChunkLoader )
		{
			switch( whichLoader )
			{
				case WL_DIRECTMUSIC:
				case WL_PRODUCER:
					hr = pIRefChunkLoader->SaveRefChunkForLoader( pIStream,
																  pIDocRootNode,
																  CLSID_DirectMusicStyle,
																  NULL,
																  whichLoader );
					break;
			}

			pIRefChunkLoader->Release();
		}
	}

	pIStream->Release();
	return hr;
}
	
@xref <i IDMUSProdFileRefChunk>, <i IDMUSProdLoaderRefChunk>
--------------------------------------------------------------------------------------*/

/////////////////////////////////////////////////////////////////////////////
// CFramework::SaveRefChunkForLoader

HRESULT CFramework::SaveRefChunkForLoader(
	IStream* pIStream,				// @parm [in] Pointer to an IStream interface.
	IDMUSProdNode* pIDocRootNode,	// @parm [in] Pointer to an <i IDMUSProdNode> interface for the DocRoot
									//		<o Node> of the referenced file.
	REFCLSID rclsid,				// @parm [in] CLSID of the DirectMusic object. 
	void*  pObjectDesc,				// @parm [in] Pointer to a DirectMusic DMUS_OBJECTDESC structure. May
									//		be NULL when <p pIDocRootNode's> IDMUSProdNode::GetObject method 
									//		returns an IDirectMusicObject interface pointer.  
	WhichLoader wlWhichLoader		// @parm [in] Specifies the target IDirectMusicLoader implementation
)
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	if( pIStream == NULL
	||  pIDocRootNode == NULL )
	{
		return E_INVALIDARG;
	}

	HRESULT hr;

	DMUSProdStreamInfo StreamInfo;
	StreamInfo.ftFileType = FT_RUNTIME;
	StreamInfo.guidDataFormat = GUID_CurrentVersion;
	StreamInfo.pITargetDirectoryNode = NULL;

	// Get additional stream information
	IDMUSProdPersistInfo* pPersistInfo;
	if( SUCCEEDED ( pIStream->QueryInterface( IID_IDMUSProdPersistInfo, (void **)&pPersistInfo ) ) )
	{
		pPersistInfo->GetStreamInfo( &StreamInfo );
		pPersistInfo->Release();
		pPersistInfo = NULL;
	}

	CFileNode* pFileNode = NULL;
    IDMUSProdRIFFStream* pIRiffStream = NULL;
	CComponentDoc* pComponentDoc;

	// Get the DocRoot's FileNode
	pComponentDoc = theApp.FindComponentDocument( pIDocRootNode );
	if( pComponentDoc )
	{
		if( pComponentDoc->m_pIDocRootNode == pIDocRootNode )
		{
			pComponentDoc->m_pFileNode->AddRef();
			pFileNode = pComponentDoc->m_pFileNode;
		}
	}
	if( pFileNode == NULL )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Prepare the DMUS_OBJECTDESC structure
	DMUS_OBJECTDESC dmusObjectDesc;

	memset( &dmusObjectDesc, 0, sizeof(DMUS_OBJECTDESC) );
	dmusObjectDesc.dwSize = sizeof(DMUS_OBJECTDESC);

	switch( wlWhichLoader )
	{
		case WL_DIRECTMUSIC:
		{
			IDirectMusicObject* pIDMObject;

			if( pObjectDesc )
			{
				// Use the supplied object descriptor
				LPDMUS_OBJECTDESC pDMObjectDesc = (DMUS_OBJECTDESC *)pObjectDesc;

				DWORD dwSize = min( dmusObjectDesc.dwSize, pDMObjectDesc->dwSize );  
				memcpy( &dmusObjectDesc, pDMObjectDesc, dwSize );
			}
			else
			{
				hr = pIDocRootNode->GetObject( rclsid, IID_IDirectMusicObject, (void**)&pIDMObject );
				if( SUCCEEDED ( hr ) )
				{
					pIDMObject->GetDescriptor( &dmusObjectDesc );
					pIDMObject->Release();
					pIDMObject = NULL;

					dmusObjectDesc.dwValidData &= 0x000000FF;
				}
				else
				{
					// Should have supplied pObjectDesc!!
					ASSERT( 0 );
					hr = E_INVALIDARG;
					goto ON_ERROR;
				}
			}

			// Add the filename
			CString strFileName;
			if( StreamInfo.ftFileType == FT_RUNTIME )
			{
				if( pFileNode->m_strRuntimeFile.IsEmpty() == FALSE )
				{
					// Make sure file exists on hard drive
					DWORD dwAttributes = ::GetFileAttributes( pFileNode->m_strRuntimeFile );
					if( dwAttributes != 0xFFFFFFFF )
					{
						// File exists
						theApp.SplitOutFileName( pFileNode->m_strRuntimeFile, TRUE, strFileName );
					}
				}
				if( strFileName.IsEmpty() )
				{
					if( pFileNode->m_strRuntimeFileName.IsEmpty() == FALSE )
					{
						theApp.SplitOutFileName( pFileNode->m_strRuntimeFileName, TRUE, strFileName );
					}
					else
					{
						theApp.AdjustFileName( FT_RUNTIME, pFileNode->m_strName, strFileName );
					}
				}
			}
			else
			{
				strFileName = pFileNode->m_strName;
			}
			dmusObjectDesc.dwValidData |= DMUS_OBJ_FILENAME;
			MultiByteToWideChar( CP_ACP, 0, strFileName, -1, &dmusObjectDesc.wszFileName[0], DMUS_MAX_FILENAME );

			// Add the file modified date
			if( StreamInfo.ftFileType != FT_RUNTIME )
			{
				// Skipping FT_RUNTIME because I need to figure out a way to get the full
				// path of the runtime file
				pFileNode->ConstructFileName( strFileName );
				HANDLE hFile = ::CreateFile( strFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
	 										 FILE_ATTRIBUTE_NORMAL, NULL );
				if( hFile != INVALID_HANDLE_VALUE )
				{
					FILETIME ftModified;

					if( ::GetFileTime(hFile, NULL, NULL, &ftModified) )
					{
						dmusObjectDesc.dwValidData |= DMUS_OBJ_DATE;
						dmusObjectDesc.ftDate = ftModified;
					}

					CloseHandle( hFile );
				}
			}
			break;
		}

		case WL_PRODUCER:
			dmusObjectDesc.dwValidData = (DMUS_OBJ_OBJECT | DMUS_OBJ_CLASS); 
			memcpy( &dmusObjectDesc.guidObject, &pFileNode->m_guid, sizeof( dmusObjectDesc.guidObject ) );
			memcpy( &dmusObjectDesc.guidClass, &rclsid, sizeof( dmusObjectDesc.guidClass ) );
			break;
	}
	
	// Create a RIFFStream
    hr = AllocRIFFStream( pIStream, &pIRiffStream );
	if( FAILED ( hr ) )
	{
		goto ON_ERROR;
	}

	// Save the DirectMusic file Reference Chunk
    hr = WriteTheRefChunkForLoader( pIRiffStream, &dmusObjectDesc );

ON_ERROR:
	if( pIRiffStream )
	{
		pIRiffStream->Release();
	}

	if( pFileNode )
	{
		pFileNode->Release();
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CFramework::IsDocRootNode

BOOL CFramework::IsDocRootNode( IDMUSProdNode* pIDocRootNode )
{
	ASSERT( pIDocRootNode != NULL );

	BOOL fDocRootNode = FALSE;

	CFileNode* pFileNode = theApp.GetFileByDocRootNode( pIDocRootNode );
	if( pFileNode )
	{
		fDocRootNode = TRUE;

		pFileNode->Release();
	}

	return fDocRootNode;
}


/////////////////////////////////////////////////////////////////////////////
// CFramework::FindFileClipFormat

UINT CFramework::FindFileClipFormat( IDataObject* pIDataObject )
{
	UINT uClipFormat = 0;

	if( pIDataObject == NULL )
	{
		return 0;
	}

	// Create a new CDllJazzDataObject and see if it can read the data object's format.
	CAppJazzDataObject* pDataObject = new CAppJazzDataObject();
	if( pDataObject == NULL )
	{
		return 0;
	}

    POSITION pos = m_lstClipFormats.GetHeadPosition();
    while( pos )
    {
        CJzClipFormat* pJzClipFormat = m_lstClipFormats.GetNext( pos );

		if( SUCCEEDED (	pDataObject->IsClipFormatAvailable( pIDataObject, pJzClipFormat->m_uClipFormat ) ) )
		{
			uClipFormat = pJzClipFormat->m_uClipFormat;
			break;
		}
	}

	pDataObject->Release();

	return uClipFormat;
}


/////////////////////////////////////////////////////////////////////////////
// CFramework::FindDocTypeByClipFormat

HRESULT CFramework::FindDocTypeByClipFormat( UINT uClipFormat, IDMUSProdDocType** ppIDocType )
{
	if( ppIDocType == NULL )
	{
		return E_POINTER;
	}

	*ppIDocType = NULL;

	if( uClipFormat == 0 )
	{
		return E_INVALIDARG;
	}

    POSITION pos = m_lstClipFormats.GetHeadPosition();
    while( pos )
    {
        CJzClipFormat* pJzClipFormat = m_lstClipFormats.GetNext( pos );

		if( pJzClipFormat->m_uClipFormat == uClipFormat )
		{
			IDMUSProdDocType* pIDocType;

			BSTR bstrExt = pJzClipFormat->m_strExt.AllocSysString();

			if( SUCCEEDED ( FindDocTypeByExtension( bstrExt, &pIDocType ) ) )
			{
				*ppIDocType = pIDocType;
				return S_OK;
			}
			break;
		}
	}

	return E_FAIL;
}


/////////////////////////////////////////////////////////////////////////////
// CFramework::FindExtensionByClipFormat

HRESULT CFramework::FindExtensionByClipFormat( UINT uClipFormat, CString& strExt )
{
    POSITION pos = m_lstClipFormats.GetHeadPosition();
    while( pos )
    {
        CJzClipFormat* pJzClipFormat = m_lstClipFormats.GetNext( pos );

		if( pJzClipFormat->m_uClipFormat == uClipFormat )
		{
			strExt = pJzClipFormat->m_strExt;
			return S_OK;
		}
	}

	return E_FAIL;
}


/////////////////////////////////////////////////////////////////////////////
// CFramework WriteTheFileFromDataObject

HRESULT CFramework::WriteTheFileFromDataObject( CFProducerFile* pcfProducerFile, IStream* pIMemStream, IDMUSProdDocType* pIDocType )
{
	HRESULT hr;

	if( pcfProducerFile == NULL
	||  pIMemStream == NULL
	||  pIDocType == NULL )
	{
		return E_INVALIDARG;
	}

	// Get the DocTemplate
	CJazzDocTemplate* pDocTemplate = theApp.FindDocTemplate( pIDocType ); 
	if( pDocTemplate == NULL )
	{
		return E_FAIL;
	}

	// Get file's name
	CString strFileName = pcfProducerFile->strFileName;
	
	if( theApp.DoPromptFileSave( pDocTemplate, strFileName ) == FALSE )
	{
		return E_FAIL;
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
	}
	
	// See if the file is in any of the opened Projects
	CProject* pProject = theApp.GetProjectByFileName( strFileName );
	if( pProject )
	{
		// Display the newly created file in the Project Tree
		theApp.OpenTheFile( strFileName, TGT_FILENODE_SAVEAS );
		pProject->Release();
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CFramework CreateFileFromDataObject

HRESULT CFramework::CreateFileFromDataObject( IDataObject* pIDataObject )
{
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

	HRESULT hr = E_FAIL;

	// Get stream of file data
	UINT uClipFormat = theApp.m_pFramework->FindFileClipFormat( pIDataObject );
	if( uClipFormat )
	{
		if( SUCCEEDED ( theApp.m_pFramework->LoadClipFormat( pIDataObject, uClipFormat, &pIStream ) ) )
		{
			IDMUSProdDocType* pIDocType;

			if( SUCCEEDED ( theApp.m_pFramework->FindDocTypeByClipFormat( uClipFormat, &pIDocType ) ) )
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

				// Write the file
				hr = WriteTheFileFromDataObject( &cfProducerFile, pIStream, pIDocType );

				pIDocType->Release();
			}

			pIStream->Release();
		}
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CFramework::AreYouSure

BOOL CFramework::AreYouSure( IDMUSProdNode* pINode )
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );
	ASSERT( pINode != NULL );

	CTreeCtrl* pTreeCtrl = theApp.GetProjectTreeCtrl();
	ASSERT( pTreeCtrl != NULL );

	// If node represents document, we want to delete its FileNode
	CComponentDoc* pComponentDoc = theApp.FindComponentDocument( pINode );
	if( pComponentDoc )
	{
		if( pComponentDoc->m_pIDocRootNode == pINode )
		{
			pINode = pComponentDoc->m_pFileNode;
			ASSERT( pINode != NULL );
		}
	}

	if( pINode->CanDelete() != S_OK )
	{
		return FALSE;
	}

	GUID guidNodeId;
	if( FAILED ( pINode->GetNodeId( &guidNodeId ) ) )
	{
		return FALSE;
	}

	// Projects and  Directories handled elsewhere
	if( ::IsEqualGUID ( guidNodeId, GUID_ProjectFolderNode )
	||  ::IsEqualGUID ( guidNodeId, GUID_DirectoryNode ) )
	{
		// Should not get here!
		ASSERT( 0 );
		return TRUE;
	}

	// Handle File nodes
	else if( ::IsEqualGUID ( guidNodeId, GUID_FileNode ) )
	{
		CFileNode* pFileNode = (CFileNode *)pINode;
		int nResult;

		// Are you sure?
		CJzNotifyNode* pJzNotifyNode;
		CFileNode* pTheFileNode;
		BOOL fReferenced = FALSE;

		// Set fReferenced flag
		POSITION pos = pFileNode->m_lstNotifyNodes.GetHeadPosition();
		while( pos )
		{
			pJzNotifyNode = static_cast<CJzNotifyNode*>( pFileNode->m_lstNotifyNodes.GetNext(pos) );
			
			pTheFileNode = theApp.GetFileByGUID( pJzNotifyNode->m_guidFile );
			if( pTheFileNode )
			{
				fReferenced = TRUE;
				pTheFileNode->Release();
				break;
			}
		}

		if( fReferenced )
		{
			if( pFileNode->m_hItem )
			{
				if( pTreeCtrl )
				{
					pTreeCtrl->EnsureVisible( pFileNode->m_hItem );
					pTreeCtrl->SelectItem( pFileNode->m_hItem );
				}
			}

			CDeleteFileDlg deleteDlg;
			deleteDlg.m_pFileNode = pFileNode;
			nResult = deleteDlg.DoModal();
		}
		else
		{
			CString strMsg;
			AfxFormatString1( strMsg, IDS_DELETE_FILE, pFileNode->m_strName );
			nResult = AfxMessageBox( strMsg, MB_OKCANCEL );
		}

		if( nResult == IDOK
		||  nResult == IDYES )
		{
			return TRUE;
		}
	}

	// Handle all other nodes
	else
	{
		CString strMsg;
		CString strName;
		BSTR bstrName;

		pINode->GetNodeName( &bstrName );
		strName = bstrName;
		::SysFreeString( bstrName );

		AfxFormatString1( strMsg, IDS_DELETE_NODE, strName );

		if( AfxMessageBox( strMsg, MB_OKCANCEL ) == IDOK )
		{
			return TRUE;
		}
	}

	return FALSE;
}
