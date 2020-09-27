// ComponentDoc.cpp : implementation file
//

#include "stdafx.h"
#include "JazzApp.h"
#include "MainFrm.h"
#include "ComponentCntrItem.h"
#include "SavePrompt.h"
#include "NotifyList.h"
#include <DLSDesigner.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CComponentDoc

IMPLEMENT_DYNCREATE(CComponentDoc, CCommonDoc)

CComponentDoc::CComponentDoc()
{
	m_pFileNode = NULL;
	m_pIDocType = NULL;
	m_bAction = DOC_ACTION_NONE;
}


/////////////////////////////////////////////////////////////////////////////
// CComponentDoc::OnNewDocument

BOOL CComponentDoc::OnNewDocument()
{
	CJazzDocTemplate* pDocTemplate;
	IDMUSProdProject* pIProject;

	// Find IDMUSProdDocType interface
	ASSERT( m_pIDocType == NULL );
	pDocTemplate = (CJazzDocTemplate *)GetDocTemplate();
	ASSERT( pDocTemplate != NULL );
	m_pIDocType = pDocTemplate->m_pIDocType;
	m_pIDocType->AddRef();
	ASSERT( m_pIDocType != NULL );

	// MFC new document processing
	if (!COleDocument::OnNewDocument())
	{
		return FALSE;
	}

	// Create FileNode
	m_pFileNode = new CFileNode;
	if( m_pFileNode == NULL )
	{
		return FALSE;
	}
	m_pFileNode->AddRef();
	m_pFileNode->m_pComponentDoc = this;
//	m_pFileNode->m_pComponentDoc->AddRef();		intentionally missing

	// Determine path of FileNode
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

	// Get IDMUSProdProject for this file
	if( FAILED ( theApp.m_pFramework->FindProject( pIDirNode, &pIProject ) ) )
	{
		return FALSE;
	}
	CProject* pProject = (CProject *)pIProject;

	ASSERT( m_pIDocRootNode == NULL );
	if(	theApp.m_strImportFileName.IsEmpty() )
	{
		// Create a new file
		theApp.DoWaitCursor( TRUE );
		HRESULT hr = m_pIDocType->OnFileNew( pIProject, pIDirNode, &m_pIDocRootNode );
		theApp.DoWaitCursor( FALSE );

		// Override the default name with name from CFileNewDlg
		if( SUCCEEDED ( hr )
		&&  m_pIDocRootNode )
		{
			if( theApp.m_strNewFileName.IsEmpty() == FALSE )
			{
				BSTR bstrName = theApp.m_strNewFileName.AllocSysString();
				m_pIDocRootNode->SetNodeName( bstrName );
			}
		}
		
		if( FAILED( hr ) )
		{
			pIProject->Release();
			return FALSE;
		}
	}
	else
	{
		// Open the file and have the Component load (import) it
		IStream* pIStream;
		BSTR bstrPathName = theApp.m_strImportFileName.AllocSysString();

		if( FAILED ( theApp.m_pFramework->AllocFileStream(bstrPathName, GENERIC_READ, FT_UNKNOWN,
														  GUID_AllZeros, pIDirNode, &pIStream) ) )
		{
			pIProject->Release();
			return FALSE;
		}

		// Start tracking File Open
		if( theApp.m_pJzTrackFileOpen == NULL )
		{
			theApp.m_pJzTrackFileOpen = new CJzTrackFileOpen;
		}
		if( theApp.m_pJzTrackFileOpen )
		{
			theApp.m_pJzTrackFileOpen->AddRef();
		}

		theApp.DoWaitCursor( TRUE );
		HRESULT hr = m_pIDocType->OnFileOpen( pIStream, pIProject, pIDirNode, &m_pIDocRootNode );
		theApp.DoWaitCursor( FALSE );

		// Stop tracking File Open
		if( theApp.m_pJzTrackFileOpen )
		{
			theApp.m_pJzTrackFileOpen->Release();
		}
		
		if( FAILED( hr )
		||  m_pIDocRootNode == NULL )
		{
			HINSTANCE hInstance = AfxGetResourceHandle();
			AfxSetResourceHandle( theApp.m_hInstance );

			CString strMsg;
			AfxFormatString1( strMsg, IDS_ERR_CANNOT_IMPORT_FILE, theApp.m_strImportFileName );
			AfxMessageBox( strMsg );

			AfxSetResourceHandle( hInstance );

			pIProject->Release();
			pIStream->Release();
			return FALSE;
		}
		pIStream->Release();
	}
	ASSERT( m_pIDocRootNode != NULL );
	pIProject->Release();
	pIProject = NULL;

	// Set the FileNode's child node
	m_pFileNode->InsertChildNode( m_pIDocRootNode );

	// Set the FileNode's parent node
	m_pFileNode->SetParentNode( pIDirNode );

	// Get name of DocRoot
	CString strDocRootName;
	BSTR bstrDocRootName;
	if(	theApp.m_strImportFileName.IsEmpty() )
	{
		if( SUCCEEDED ( m_pIDocRootNode->GetNodeName( &bstrDocRootName ) ) )
		{
			strDocRootName = bstrDocRootName;
			::SysFreeString( bstrDocRootName );
		}
		if( strDocRootName.IsEmpty() )
		{
			pDocTemplate->GetDocString( strDocRootName, CDocTemplate::docName );
			if( strDocRootName.IsEmpty() )
			{
				// Use generic 'untitled'
				VERIFY( strDocRootName.LoadString( AFX_IDS_UNTITLED ) );
			}

			// Append "1" to docName 
			CString strNbr;
			strNbr.Format( "%d", 1 );
			strDocRootName += strNbr;
		}
	}
	else
	{
		TCHAR achName[_MAX_FNAME];
		_tsplitpath( theApp.m_strImportFileName, NULL, NULL, achName, NULL );
		strDocRootName = achName;
	}
	ASSERT( strDocRootName.IsEmpty() == FALSE );

	// Determine filename (may have to adjust document title)
	CString strProposedFileName;
	SetTitle( strDocRootName );
	DetermineDefaultFileName( m_pFileNode->m_strName );
	m_pFileNode->ConstructFileName( strProposedFileName );
	theApp.GetUniqueFileName( strProposedFileName, m_pFileNode->m_strName );
	SetTitle( m_pFileNode->m_strName );

	// Add file to CProject file list
	pProject->AddFile( m_pFileNode );

	// Limit length of DocRoot text
	short nMaxLength;

	theApp.SplitOutFileName( m_pFileNode->m_strName, FALSE, strDocRootName );
	if( SUCCEEDED ( m_pIDocRootNode->GetNodeNameMaxLength(&nMaxLength) ) )
	{
		if( nMaxLength > 0 )
		{
			if( strDocRootName.GetLength() > nMaxLength )
			{
				strDocRootName = strDocRootName.Left( nMaxLength );
			}
		}
	}

	// Set DocRoot node name
	bstrDocRootName = strDocRootName.AllocSysString();
	m_pIDocRootNode->SetNodeName( bstrDocRootName );

	// Force SetNodeName to handle setting of FileNode node name
	bstrDocRootName = m_pFileNode->m_strName.AllocSysString();
	m_pFileNode->m_strName.Empty();
	m_pFileNode->SetNodeName( bstrDocRootName );

	// Save file
	CString strFileName;
	m_pFileNode->ConstructFileName( strFileName );
	if( DoSave ( strFileName ) )
	{
		// Place document nodes in Project Tree
		if( SUCCEEDED ( pIDirNode->InsertChildNode( m_pFileNode ) )
		&&  SUCCEEDED ( theApp.m_pFramework->AddNode( m_pIDocRootNode, m_pFileNode ) ) )
		{
			CTreeCtrl* pTreeCtrl = theApp.GetProjectTreeCtrl();

			CDirectoryNode* pDirNode = (CDirectoryNode *)pIDirNode;
			if( pDirNode->m_hItem )
			{
				CMainFrame* pMainFrame = (CMainFrame *)theApp.m_pMainWnd;
				ASSERT( pMainFrame != NULL );
				pMainFrame->m_wndTreeBar.SortChildren( pDirNode->m_hItem );
			}

			theApp.CleanUpNotifyLists( m_pFileNode, FALSE );

			if( theApp.m_nShowNodeInTree == SHOW_NODE_IN_TREE )
			{
				if( pTreeCtrl )
				{
					pTreeCtrl->SetFocus();
				}
			}
			theApp.m_pFramework->ShowTreeNode( m_pFileNode );

			if( m_pFileNode->m_hChildItem )
			{
				if( pTreeCtrl )
				{
					if( theApp.m_nShowNodeInTree == SHOW_NODE_IN_TREE )
					{
						pTreeCtrl->EnsureVisible( m_pFileNode->m_hChildItem );
					}
					if( pTreeCtrl->ItemHasChildren(m_pFileNode->m_hChildItem) )
					{
						pTreeCtrl->Expand( m_pFileNode->m_hChildItem, TVE_EXPAND );
					}
				}
			}

			if( m_pFileNode->m_pIChildNode )
			{
				if( theApp.m_fOpenEditorWindow )
				{
					OpenEditorWindow( m_pFileNode->m_pIChildNode );
				}
			}
			return TRUE;
		}
	}

	// Remove file from CProject file list
	pProject->RemoveFile( m_pFileNode );

	return FALSE;
}

CComponentDoc::~CComponentDoc()
{
	if( m_pFileNode )
	{
		theApp.CleanUpNotifyLists( m_pFileNode, TRUE );

		m_pFileNode->m_pComponentDoc = NULL;
		m_pFileNode->Release();
	}

	if( m_pIDocType )
	{
		m_pIDocType->Release();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CComponentDoc::CreateNewWindow

CFrameWnd* CComponentDoc::CreateNewWindow( IDMUSProdNode* pINode ) 
{
	HINSTANCE hInstance;

	CJazzDocTemplate* pTemplate = (CJazzDocTemplate *)GetDocTemplate();
	ASSERT_VALID(pTemplate);

	hInstance = AfxGetResourceHandle();
	AfxSetResourceHandle( pTemplate->m_hInstance );

	CFrameWnd* pFrame = pTemplate->CreateNewFrame( this, NULL );

	AfxSetResourceHandle( hInstance );

	if( pFrame == NULL )
	{
		TRACE0("Warning: failed to create new frame.\n");
		return NULL;     // command failed
	}

	CJzNode* pJzNode = NULL;

	// Set Editor WindowPlacement
	CTreeCtrl* pTreeCtrl = theApp.GetProjectTreeCtrl();
	if( pTreeCtrl )
	{
		HTREEITEM hItem = theApp.m_pFramework->FindTreeItem( pINode );
		if( hItem )
		{
			pJzNode = (CJzNode *)pTreeCtrl->GetItemData( hItem );
			if( pJzNode )
			{
				if( pJzNode->m_wp.length != 0 )
				{
					pJzNode->m_wp.length = sizeof(pJzNode->m_wp);

					int nOrigShowCmd = pJzNode->m_wp.showCmd;
					pJzNode->m_wp.showCmd = SW_HIDE;
					if( pJzNode->m_wp.rcNormalPosition.left < 0 )
					{
						pJzNode->m_wp.rcNormalPosition.right -= pJzNode->m_wp.rcNormalPosition.left;
						pJzNode->m_wp.rcNormalPosition.left = 0;
					}
					if( pJzNode->m_wp.rcNormalPosition.top < 0 )
					{
						pJzNode->m_wp.rcNormalPosition.bottom -= pJzNode->m_wp.rcNormalPosition.top;
						pJzNode->m_wp.rcNormalPosition.top = 0;
					}
					pFrame->SetWindowPlacement( &pJzNode->m_wp );
					pJzNode->m_wp.showCmd = nOrigShowCmd;
				}
			}
		}
	}

	pTemplate->InitialUpdateFrame( pFrame, this );

	// Get Editor WindowPlacement
	if( pJzNode )
	{
		if( pJzNode->m_wp.length == 0 )
		{
			pJzNode->m_wp.length = sizeof(pJzNode->m_wp);
			if( pFrame->GetWindowPlacement( &pJzNode->m_wp ) == 0 )
			{
				ZeroMemory( &pJzNode->m_wp, sizeof(pJzNode->m_wp) );
			}
		}
	}

	return pFrame;
}


/////////////////////////////////////////////////////////////////////////////
// CComponentDoc::OpenEditorWindow

void CComponentDoc::OpenEditorWindow( IDMUSProdNode* pINode ) 
{
	CComponentCntrItem* pCntrItem = NULL;
	CComponentView* pView = NULL;
	CMainFrame* pMainFrame;
	CFrameWnd* pFrame;
	CLSID clsidEditor;
	IDMUSProdEditor* pIEditor;
	HWND  hWndEditor;
	CWnd* pWndEditor;
	CRect rect;

	ASSERT( pINode != NULL );

	pMainFrame = (CMainFrame *)theApp.m_pMainWnd;
	ASSERT( pMainFrame != NULL );
	
	CWnd* pWndActive = pMainFrame->MDIGetActive();

	// If editor already open just bring window to front
	if( SUCCEEDED ( pINode->GetEditorWindow(&hWndEditor) ) )
	{
		if( hWndEditor )
		{
			pWndEditor = CWnd::FromHandlePermanent( hWndEditor );
			if( pWndEditor )
			{
				if( pWndEditor->IsIconic() )
				{
					pMainFrame->MDIRestore( pWndEditor );
				}
				pMainFrame->MDIActivate( pWndEditor );
				return;
			}
		}
	}

	TRY
	{
		if( SUCCEEDED( pINode->GetEditorClsId(&clsidEditor) ) )
		{
			pFrame = CreateNewWindow( pINode );
			if( pFrame == NULL )
			{
				AfxThrowMemoryException();		// any exception will do
			}
			ASSERT_VALID( pFrame );

			pFrame->ModifyStyle( FWS_ADDTOTITLE, 0, 0 );
			pView = (CComponentView *)pFrame->GetActiveView();
			ASSERT_VALID( pView );
			
			pView->m_pINode = pINode;
			pView->m_pINode->AddRef();
			pView->m_pINode->SetEditorWindow( pFrame->GetSafeHwnd() );

			IDMUSProdProject* pIProject;
			if( SUCCEEDED ( theApp.m_pFramework->FindProject( pView->m_pINode, &pIProject ) ) )
			{
				pView->m_pIProject = pIProject;
			}

			// Set Window Title
			{
				BSTR bstrTitle;
				CString strTitle;

				pView->m_pINode->GetEditorTitle( &bstrTitle );
				strTitle = bstrTitle;
				::SysFreeString( bstrTitle );
				pFrame->SetWindowText( strTitle );
			}

			pCntrItem = new CComponentCntrItem( this );
			ASSERT_VALID( pCntrItem );

			if( !pCntrItem->CreateNewItem( clsidEditor ) )
			{
				AfxThrowMemoryException();		// any exception will do
			}
			
			// Attach object to the control 
			ASSERT( pCntrItem->m_lpObject != NULL );
			if( SUCCEEDED ( pCntrItem->m_lpObject->QueryInterface( IID_IDMUSProdEditor, (void**)&pIEditor ) ) )
			{
				pView->m_pIEditor = pIEditor;
				pIEditor->AttachObjects( pINode );
			}
			else
			{
				AfxThrowMemoryException();		// any exception will do
			}

			// Force WS_CLIPCHILDREN style
			pFrame->ModifyStyle( 0, WS_CLIPCHILDREN, 0 );

			ASSERT_VALID( pCntrItem );
			pCntrItem->DoVerb( OLEIVERB_SHOW, static_cast<CComponentView*>( pView ) );
			ASSERT_VALID( pCntrItem );

			// Force a WM_GETMINMAXINFO 
			CWnd* pWndParent = pFrame->GetParent();
			if( pWndParent )
			{
				CRect rect;
				pFrame->GetWindowRect( &rect );
				pWndParent->ScreenToClient( &rect );
				pFrame->MoveWindow( &rect, TRUE );
			}

			pView->m_pEditorCtrl = pCntrItem;			
			pView->GetWindowRect( &rect );
			pView->SendMessage( WM_SIZE, SIZE_RESTORED, MAKELPARAM(rect.Width(), rect.Height()) );
			
			// If no other editors are open maximize the window 
			if( pWndActive == NULL )
			{
				pFrame->ShowWindow( SW_SHOWMAXIMIZED );
			}

			// Highlight the corresponding node in the Project Tree
			theApp.m_pFramework->ShowTreeNode( pView->m_pINode );
		}
	}

	CATCH( CException, e )
	{
		AfxMessageBox( IDP_FAILED_TO_CREATE );
		if( pCntrItem != NULL )
		{
			ASSERT_VALID( pCntrItem );
			pCntrItem->Delete();
		}
		if( pFrame != NULL )
		{
			ASSERT_VALID( pFrame );
			PreCloseFrame( pFrame );
			pFrame->DestroyWindow();
		}
	}
	END_CATCH
}


/////////////////////////////////////////////////////////////////////////////
// CComponentDoc::CloseAllViews

void CComponentDoc::CloseAllViews( BOOL fAutoDelete )
{
	CComponentView* pView;
	CFrameWnd* pFrame;
	BOOL fKeepGoing;

	// destroy all frames viewing this document
	// but keep the seed view so document won't get destroyed
	BOOL bAutoDelete = m_bAutoDelete;
	m_bAutoDelete = fAutoDelete;

	fKeepGoing = TRUE;
	while( !m_viewList.IsEmpty()  &&  (fKeepGoing == TRUE) )
	{
		fKeepGoing = FALSE;

		POSITION pos = GetFirstViewPosition();
		if( pos )
		{
			pView = (CComponentView *)GetNextView( pos );
			ASSERT_VALID( pView );

			while( pView  &&  (pView->m_fSeed == TRUE) )		// skip over seed view
			{
				pView = (CComponentView *)GetNextView( pos );
			}

			// keep the seed view so that the document
			// won't get destroyed for having zero views
			if( pView  &&  (pView->m_fSeed == FALSE) )
			{
				// get the frame associated with the view and close it
				// this will destroy the view as well
				ASSERT_VALID( pView );
				pFrame = pView->GetParentFrame();
				ASSERT_VALID( pFrame );
				PreCloseFrame( pFrame );
				pFrame->DestroyWindow();

				fKeepGoing = TRUE;
			}
		}

	}

	m_bAutoDelete = bAutoDelete;
}


/////////////////////////////////////////////////////////////////////////////
// CComponentDoc::CloseViewsByNode

BOOL CComponentDoc::CloseViewsByNode( IDMUSProdNode* pINode )
{
	CComponentView* pView;
	CFrameWnd* pFrame;
	BOOL fKeepGoing;
	BOOL fClosedEditor = FALSE;

	// destroy all frames viewing this document
	// but keep the seed view so document won't get destroyed
	BOOL bAutoDelete = m_bAutoDelete;
	m_bAutoDelete = FALSE;

	fKeepGoing = TRUE;
	POSITION pos = GetFirstViewPosition();

	while( pos  &&  (fKeepGoing == TRUE) )
	{
		fKeepGoing = FALSE;

		pView = (CComponentView *)GetNextView( pos );
		ASSERT_VALID( pView );

		while( pView  &&  (pView->m_fSeed == TRUE) )		// skip over seed view
		{
			pView = (CComponentView *)GetNextView( pos );
		}

		// keep the seed view so that the document
		// won't get destroyed for having zero views
		if( pView
		&&  pView->m_fSeed == FALSE )
		{
			if( pView->m_pINode == pINode )
			{
				// get the frame associated with the view and close it
				// this will destroy the view as well
				ASSERT_VALID( pView );
				pFrame = pView->GetParentFrame();
				pFrame->ActivateFrame();
				ASSERT_VALID( pFrame );
				PreCloseFrame( pFrame );
				pFrame->SendMessage( WM_CLOSE );

				fClosedEditor = TRUE;
			}

			fKeepGoing = TRUE;
		}
	}

	m_bAutoDelete = bAutoDelete;

	return fClosedEditor;
}


/////////////////////////////////////////////////////////////////////////////
// CComponentDoc::RevertDocument

BOOL CComponentDoc::RevertDocument( void ) 
{
	BOOL fSuccess = FALSE;

	CString strFileName = GetPathName();

	ASSERT( m_pFileNode != NULL );
	ASSERT( AfxIsValidString(strFileName) );

	if( !strFileName.IsEmpty() )	// Have a filename
	{
		DWORD dwAttributes = GetFileAttributes( strFileName );
		if( dwAttributes != 0xFFFFFFFF )	// File exists
		{
			// Set Project Tree position
			theApp.SetProjectTreePosition( m_pFileNode );

			// Remove file's notify list
			CNotifyList nl;
			nl.Detach( m_pFileNode );

			// Revert the file
			if( SUCCEEDED ( m_pFileNode->DeleteNode( FALSE ) ) )
			{
				if( theApp.OpenTheFile( strFileName, TGT_FILENODE_SAVEAS ) )
				{
					fSuccess = TRUE;
				}
			}

			// Reattach file's notify list
			CFileNode* pFileNode = theApp.FindFileNode( strFileName );
			if( pFileNode )
			{
				nl.Attach( pFileNode );
			}
		}
	}

	return fSuccess;
}


/////////////////////////////////////////////////////////////////////////////
// CComponentDoc::IsModified

BOOL CComponentDoc::IsModified( void )
{
	BOOL fModified = FALSE;

	if( m_pIDocRootNode )
	{
		IPersistStream *pIPS;

		m_pIDocRootNode->QueryInterface( IID_IPersistStream, (void **)&pIPS );
		if( pIPS )
		{
			if( pIPS->IsDirty() == S_OK)
			{
				fModified = TRUE;
			}

			pIPS->Release();
		}
	}

	return fModified;
}


/////////////////////////////////////////////////////////////////////////////
// CComponentDoc::SaveModified

// Same as COleDocument::SaveModified with following exception: 
// Call to UpdateModifiedFlag() was removed.  We do not want to force
// modified flag to signify a dirty file.

BOOL CComponentDoc::SaveModified() 
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	CString strName;
	CString strPrompt;
	int nSavePromptAction;

	// Following "if" statement from COleDocument::SaveModified
	if (::InSendMessage())
	{
		POSITION pos = GetStartPosition();
		COleClientItem* pItem;
		while( (pItem = GetNextClientItem(pos)) != NULL )
		{
			ASSERT( pItem->m_lpObject != NULL );
			SCODE sc = pItem->m_lpObject->IsUpToDate();
			if( sc != OLE_E_NOTRUNNING && FAILED(sc) )
			{
				// inside inter-app SendMessage limits the user's choices
				strName = m_strPathName;
				if( strName.IsEmpty() )
				{
					VERIFY(strName.LoadString(AFX_IDS_UNTITLED));
				}

				AfxFormatString1( strPrompt, AFX_IDP_ASK_TO_DISCARD, strName );

				return AfxMessageBox( strPrompt, MB_OKCANCEL|MB_DEFBUTTON2,
										AFX_IDP_ASK_TO_DISCARD ) == IDOK;
			}
		}
	}

	// Remainder of code based on CDocument::SaveModified
	// CDocument::SaveModified was changed to customize
	// the buttons on the save prompt message box.

	if( !IsModified() )
	{
		return TRUE;        // ok to continue
	}

	BOOL fNewFile = FALSE;
	
	// Get name/title of document
	if( m_strPathName.IsEmpty() )
	{
		fNewFile = TRUE;

		// get name based on caption
		strName = m_strTitle;
		if( strName.IsEmpty() )
		{
			VERIFY( strName.LoadString(AFX_IDS_UNTITLED) );
		}
	}
	else
	{
		// Get name based on file title of path name
		theApp.SplitOutFileName( m_strPathName, TRUE, strName );
	}

	CString strFilterName;

	CJazzDocTemplate* pDocTemplate = (CJazzDocTemplate *)GetDocTemplate();
	ASSERT( pDocTemplate != NULL );
	if( pDocTemplate )
	{
		pDocTemplate->GetDocString( strFilterName, CDocTemplate::docName );
	}

	if( fNewFile )
	{
		AfxFormatString2( strPrompt, IDS_SAVEPROMPT_SAVEAS, strFilterName, strName );
	}
	else
	{
		AfxFormatString2( strPrompt, IDS_SAVEPROMPT_SAVE, strFilterName, strName );
	}

	if( theApp.m_nSavePromptAction == 0 )
	{
		CSavePromptDlg spDlg( &strPrompt );
		::MessageBeep( MB_ICONEXCLAMATION );
		nSavePromptAction = spDlg.DoModal();
	}
	else
	{
		nSavePromptAction = theApp.m_nSavePromptAction;
	}

	switch( nSavePromptAction )
	{
		case IDCANCEL:
			return FALSE;       // don't continue

		case IDYES:
			if( !DoFileSave() )
				return FALSE;	// don't continue
			break;

		case ID_YESALL:
			theApp.m_nSavePromptAction = nSavePromptAction;
			if( !DoFileSave() )
				return FALSE;	// don't continue
			break;

		case IDNO:
			break;

		case ID_NOALL:
			theApp.m_nSavePromptAction = nSavePromptAction;
			break;

		default:
			ASSERT( FALSE );
			break;
	}

	return TRUE;    // keep going
}


/////////////////////////////////////////////////////////////////////////////
// CComponentDoc::SaveAndCloseDoc

BOOL CComponentDoc::SaveAndCloseDoc( void )
{
	BOOL fContinue = TRUE;

	if( m_bAction == DOC_ACTION_NONE )	// Don't prompt again if DOC_ACTION in progress
	{
		fContinue = SaveModified();
	}

	if( fContinue )
	{
		OnCloseDocument();
		return TRUE;
	}

	return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CComponentDoc::SaveComponentFileIfModified

BOOL CComponentDoc::SaveComponentFileIfModified( void )	// Customizes buttons on save prompt
{
	return SaveModified();
}


BEGIN_MESSAGE_MAP(CComponentDoc, CCommonDoc)
	//{{AFX_MSG_MAP(CComponentDoc)
	ON_COMMAND(ID_FILE_RUNTIME_SAVEAS, OnFileRuntimeSaveAs)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CComponentDoc diagnostics

#ifdef _DEBUG
void CComponentDoc::AssertValid() const
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );
	CCommonDoc::AssertValid();
}

void CComponentDoc::Dump(CDumpContext& dc) const
{
	CCommonDoc::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CComponentDoc serialization

void CComponentDoc::Serialize(CArchive& ar)
{
}


/////////////////////////////////////////////////////////////////////////////
// CComponentDoc commands

/////////////////////////////////////////////////////////////////////////////
// CComponentDoc::OnOpenDocument

BOOL CComponentDoc::OnOpenDocument( LPCTSTR szPathName ) 
{
	CJazzDocTemplate* pDocTemplate;
	CDirectoryNode* pParentDirNode;
	IDMUSProdProject* pIProject;
	CNotifyList nl;		// Used to transfer new file pointer when overwriting existing file
	
	ASSERT( szPathName != NULL );
	ASSERT( AfxIsValidString(szPathName) );

	// Find the parent node in the Project Tree
	IDMUSProdNode* pIParentNode = theApp.m_pFramework->DetermineParentNode( szPathName );
	if( pIParentNode == NULL )
	{
		return FALSE;
	}

	pParentDirNode = (CDirectoryNode *)pIParentNode;

	// Find IDMUSProdDocType interface
	ASSERT( m_pIDocType == NULL );
	pDocTemplate = (CJazzDocTemplate *)GetDocTemplate();
	ASSERT( pDocTemplate != NULL );
	m_pIDocType = pDocTemplate->m_pIDocType;
	m_pIDocType->AddRef();
	ASSERT( m_pIDocType != NULL );

	BOOL fInsertFileNode = FALSE;
	ASSERT( m_pFileNode == NULL );

	// Get FileNode based on target location in the Project
	{
		CString strOrigFileName;
		CString strNewFileName;
		CString strNewPathName;
		CString strRuntimeFileName;
		CString strRuntimePathName;

		// Split out name of file
		theApp.SplitOutFileName( szPathName, TRUE, strOrigFileName );

		// Get new directory
		pParentDirNode->ConstructPath( strNewPathName );

		// Append "runtime" name of file to the new directory
		theApp.AdjustFileName( FT_RUNTIME, strOrigFileName, strRuntimeFileName );
		strRuntimePathName = strNewPathName + strRuntimeFileName;

		// Append "design" name of file to the new directory
		theApp.AdjustFileName( FT_DESIGN, strOrigFileName, strNewFileName );
		strNewPathName += strNewFileName;
	
		// See if the FileNode exists
		if( strRuntimePathName.CompareNoCase( szPathName ) == 0 )
		{
			m_pFileNode = theApp.FindFileNode( strRuntimePathName );
		}
		else
		{
			m_pFileNode = theApp.FindFileNode( strNewPathName );
			if( m_pFileNode )
			{
				if( strNewPathName.CompareNoCase( szPathName ) != 0 )
				{
					// A different file wants to reside on this FileNode
					// Make sure user wants to override existing file
					CString strRelativePath;
					CString strMsg;
					BOOL fReturn;

					// A different file with the same name already exists in the Project
					// See if the user wants to overwrite it
					if( m_pFileNode->ConstructRelativePath( strRelativePath ) )
					{
						strRelativePath = _T("..\\") + strRelativePath;
						if( strRelativePath.Right(1) != _T("\\") )
						{
							strRelativePath += _T("\\");
						}
						strRelativePath += strNewFileName;
					}

					HINSTANCE hInstance = AfxGetResourceHandle();
					AfxSetResourceHandle( theApp.m_hInstance );

					AfxFormatString2( strMsg, IDS_OVERWRITE_EXISTING, m_pFileNode->m_pProject->m_strName, strRelativePath );
					if( strOrigFileName.CompareNoCase(strRuntimeFileName) == 0 )
					{
						IDMUSProdDocType* pIWaveDocType;
						if( FAILED ( theApp.m_pFramework->FindDocTypeByNodeId( GUID_WaveNode, &pIWaveDocType ) ) )
						{
							pIWaveDocType = NULL;
						}

						IDMUSProdDocType* pIDLSDocType;
						if( FAILED ( theApp.m_pFramework->FindDocTypeByNodeId( GUID_CollectionNode, &pIDLSDocType ) ) )
						{
							pIDLSDocType = NULL;
						}

						CString strMsg2;
						if( m_pIDocType == pIWaveDocType
						||  m_pIDocType == pIDLSDocType )
						{
							strMsg2.LoadString( IDS_OVERWRITE_EXISTING_WAVE_WARNING );
						}
						else
						{
							strMsg2.LoadString( IDS_OVERWRITE_EXISTING_RUNTIME_WARNING );
						}
						strMsg += _T( "\n\n" );
						strMsg += strMsg2;

						if( pIWaveDocType )
						{
							pIWaveDocType->Release();
						}
						if( pIDLSDocType )
						{
							pIDLSDocType->Release();
						}
					}
					fReturn = AfxMessageBox( strMsg, MB_YESNO|MB_DEFBUTTON2 );
					
					AfxSetResourceHandle( hInstance );
					
					if( fReturn == IDNO )
					{
						m_pFileNode = NULL;
						return FALSE;
					}

					// Close the old file so that we can overwrite it
					if( m_pFileNode->m_pComponentDoc )
					{
						ASSERT( m_pFileNode->m_pIChildNode != NULL );

						// Remove file's notify list
						nl.Detach( m_pFileNode );

						// Notify attached nodes that this file is being replaced
//						theApp.m_pFramework->NotifyNodes( m_pFileNode->m_pIChildNode, FRAMEWORK_FileDeleted, NULL );
						
						// Remove old notify list
//						m_pFileNode->EmptyNotifyNodesList();

						// Remove association with DocRoot before DeleteNode() 
						ASSERT( m_pFileNode->m_pComponentDoc->m_pIDocRootNode != NULL );
						m_pFileNode->m_pComponentDoc->m_pIDocRootNode->Release();
						m_pFileNode->m_pComponentDoc->m_pIDocRootNode = NULL;

						// Remove DocRoot node from Project Tree
						m_pFileNode->m_pIChildNode->DeleteNode( FALSE );
						m_pFileNode->m_pIChildNode->Release();
						m_pFileNode->m_pIChildNode = NULL;
						m_pFileNode->m_hChildItem = NULL;

						// Close the document
						m_pFileNode->m_pComponentDoc->OnCloseDocument();
					}
				}
			}
		}
	}

	// Create a new FileNode
	if( m_pFileNode == NULL )
	{
		fInsertFileNode = TRUE;

		m_pFileNode = new CFileNode;
		if( m_pFileNode == NULL )
		{
			return FALSE;
		}
	}

	if( m_pFileNode->m_pComponentDoc )
	{
		// Nothing to do because file is already open
		// This scenario happens when user dblclks on file
		// in Explorer and restoring Project state opens 
		// the file from within DetermineParentNode called
		// above.
		ASSERT( m_pFileNode->m_pIChildNode != NULL );
		ASSERT( m_pFileNode->m_pComponentDoc != this );
		m_pFileNode = NULL;
		// Sets m_fShellCommandOK flag to TRUE so that app won't shut
		// itself down thinking that the open failed
		theApp.m_fShellCommandOK = TRUE;
		return FALSE;
	}

	m_pFileNode->AddRef();
	m_pFileNode->m_pComponentDoc = this;
//	m_pFileNode->m_pComponentDoc->AddRef();		intentionally missing

	WCHAR awszPathName[MAX_PATH];
	MultiByteToWideChar( CP_ACP, 0, szPathName, -1, awszPathName, (size_t)MAX_PATH );

	// Needed so others can query m_pFileNode->m_fInOnOpenDocument status
	if( fInsertFileNode )
	{
		// Get ListInfo
		CString strFileName = szPathName;
		BSTR bstrFileName = strFileName.AllocSysString();

		IStream* pIStream;
		if( SUCCEEDED ( theApp.m_pFramework->AllocFileStream ( bstrFileName, GENERIC_READ, FT_UNKNOWN,
															   GUID_AllZeros, pIParentNode, &pIStream ) ) )
		{
			DMUSProdListInfo ListInfo;
			ZeroMemory( &ListInfo, sizeof(ListInfo) );
			ListInfo.wSize = sizeof(ListInfo);

			if( SUCCEEDED ( m_pIDocType->GetListInfo ( pIStream, &ListInfo ) ) )
			{
				if( ListInfo.bstrName )
				{
					m_pFileNode->m_strListInfoName = ListInfo.bstrName;
					::SysFreeString( ListInfo.bstrName );
				}
				if( ListInfo.bstrDescriptor )
				{
					m_pFileNode->m_strListInfoDescriptor = ListInfo.bstrDescriptor;
					::SysFreeString( ListInfo.bstrDescriptor );
				}
				memcpy( &m_pFileNode->m_guidListInfoObject, &ListInfo.guidObject, sizeof(GUID) );
			}

			pIStream->Release();
		}
	}

	// Open the file
	IStream* pIStream;
	CString strPathName = szPathName;
	BSTR bstrPathName = strPathName.AllocSysString();

	if( FAILED ( theApp.m_pFramework->AllocFileStream(bstrPathName, GENERIC_READ, FT_UNKNOWN,
													  GUID_AllZeros, pParentDirNode, &pIStream) ) )
	{
		if( fInsertFileNode )
		{
			m_pFileNode->Release();
			m_pFileNode = NULL;
		}
		return FALSE;
	}

	// Get IDMUSProdProject for this file
	if( FAILED ( theApp.m_pFramework->FindProject( pParentDirNode, &pIProject ) ) )
	{
		if( fInsertFileNode )
		{
			m_pFileNode->Release();
			m_pFileNode = NULL;
		}
		pIStream->Release();
		return FALSE;
	}

	// Start tracking File Open
	if( theApp.m_pJzTrackFileOpen == NULL )
	{
		theApp.m_pJzTrackFileOpen = new CJzTrackFileOpen;
	}
	if( theApp.m_pJzTrackFileOpen )
	{
		theApp.m_pJzTrackFileOpen->AddRef();
	}

	CProject* pProject = (CProject *)pIProject;

	// Set state so others can query m_pFileNode->m_fInOnOpenDocument status
	if( fInsertFileNode )
	{
		CString strFileName;
		theApp.SplitOutFileName( szPathName, TRUE, strFileName );
		m_pFileNode->m_strName = strFileName;
		m_pFileNode->SetParentNode( pIParentNode );
		pProject->AddFile( m_pFileNode );
	}
	
	m_pFileNode->m_fInOnOpenDocument = TRUE;
	HRESULT hr = m_pIDocType->OnFileOpen( pIStream, pIProject, pParentDirNode, &m_pIDocRootNode );
	m_pFileNode->m_fInOnOpenDocument = FALSE;

	// File is done loading so reset state
	if( fInsertFileNode )
	{
		pProject->RemoveFile( m_pFileNode );	
	}	

	// Stop tracking File Open
	if( theApp.m_pJzTrackFileOpen )
	{
		theApp.m_pJzTrackFileOpen->Release();
	}

	pIStream->Release();
	pIStream = NULL;
	pIProject->Release();
	pIProject = NULL;

	if( FAILED( hr )
	||  m_pIDocRootNode == NULL )
	{
		if( hr != E_OUTOFMEMORY )
		{
			HINSTANCE hInstance = AfxGetResourceHandle();
			AfxSetResourceHandle( theApp.m_hInstance );

			CString strMsg;
			AfxFormatString1( strMsg, IDS_ERR_NOT_COMPONENT_FILE, szPathName );
			AfxMessageBox( strMsg );

			AfxSetResourceHandle( hInstance );
		}

		if( fInsertFileNode )
		{
			m_pFileNode->Release();
			m_pFileNode = NULL;
		}
		return FALSE;
	}

	// Set the FileNode's child node
	m_pFileNode->InsertChildNode( m_pIDocRootNode );

	// Insert FileNode when necessary
	if( fInsertFileNode )
	{
		if( FAILED ( pIParentNode->InsertChildNode( m_pFileNode ) ) )
		{
			m_pFileNode->DeleteChildNode( m_pIDocRootNode, FALSE );
			if( fInsertFileNode )
			{
				m_pFileNode->Release();
				m_pFileNode = NULL;
			}
			return FALSE;
		}
	}

	// Set the FileNode's name
	CString strFileName;
	theApp.SplitOutFileName( szPathName, TRUE, strFileName );
	BSTR bstrFileName = strFileName.AllocSysString();
	m_pFileNode->SetNodeName( bstrFileName );

	// Add nodes to the Project Tree
	if( SUCCEEDED ( theApp.m_pFramework->AddNode( m_pIDocRootNode, m_pFileNode ) ) )
	{
		CTreeCtrl* pTreeCtrl = theApp.GetProjectTreeCtrl();

		CDirectoryNode* pParentNode = (CDirectoryNode *)pIParentNode;
		if( pParentNode->m_hItem )
		{
			CMainFrame* pMainFrame = (CMainFrame *)theApp.m_pMainWnd;
			ASSERT( pMainFrame != NULL );
			pMainFrame->m_wndTreeBar.SortChildren( pParentNode->m_hItem );
		}

		m_pFileNode->MoveWPListToNodes();
		theApp.CleanUpNotifyLists( m_pFileNode, FALSE );

		if( theApp.m_nShowNodeInTree == SHOW_NODE_IN_TREE )
		{
			if( pTreeCtrl )
			{
				pTreeCtrl->SetFocus();
			}
		}
		theApp.m_pFramework->ShowTreeNode( m_pFileNode );

		if( m_pFileNode->m_hChildItem )
		{
			if( pTreeCtrl )
			{
				if( theApp.m_nShowNodeInTree == SHOW_NODE_IN_TREE )
				{
					pTreeCtrl->EnsureVisible( m_pFileNode->m_hChildItem );
					pTreeCtrl->SelectItem( m_pFileNode->m_hChildItem );
				}
				if( pTreeCtrl->ItemHasChildren(m_pFileNode->m_hChildItem) )
				{
					pTreeCtrl->Expand( m_pFileNode->m_hChildItem, TVE_EXPAND );
				}
			}
		}

		// Attach file's notify list
		// 'nl' list will be empty unless overwriting the file
		// This keeps file references intact when overwriting files
		// linked to other files
		nl.Attach( m_pFileNode );

		// Notify connected nodes that we have successfully loaded
		m_pFileNode->SendLoadFinishedNotifications();

		// FRAMEWORK_AfterFileOpen
		IDMUSProdNotifySink* pINotifySink;
		if( SUCCEEDED ( m_pIDocRootNode->QueryInterface( IID_IDMUSProdNotifySink, (void **)&pINotifySink ) ) )
		{
			pINotifySink->OnUpdate( m_pIDocRootNode, FRAMEWORK_AfterFileOpen, awszPathName );
			pINotifySink->Release();
		}

		return TRUE;
	}

	// Remove the FileNode's child node
	m_pFileNode->DeleteChildNode( m_pIDocRootNode, FALSE );

	// Remove FileNode when necessary
	if( fInsertFileNode )
	{
		pIParentNode->DeleteChildNode( m_pFileNode, FALSE );
		m_pFileNode->Release();
		m_pFileNode = NULL;
	}
	
	return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CComponentDoc::OnSaveDocument

BOOL CComponentDoc::OnSaveDocument( LPCTSTR szPathName ) 
{
	ASSERT( szPathName != NULL );
	ASSERT( AfxIsValidString(szPathName) );
	ASSERT( m_pIDocRootNode != NULL );
	ASSERT( m_pFileNode != NULL );

	IStream* pIStream;
	FileType ftFileType;
	GUID guidVersion;
	BOOL fSuccess = FALSE;

	if( theApp.m_nRuntimeSave == DOC_RUNTIME_SAVE
	||  theApp.m_nRuntimeSave == DOC_RUNTIME_SAVE_ALL )
	{
		ftFileType = FT_RUNTIME;
		guidVersion = GUID_CurrentVersion;
	}
	else
	{
		ftFileType = FT_DESIGN;
		guidVersion = GUID_CurrentVersion;
	}

	CString strPathName = szPathName;
	BSTR bstrPathName = strPathName.AllocSysString();

	WCHAR awszPathName[MAX_PATH];
	MultiByteToWideChar( CP_ACP, 0, szPathName, -1, awszPathName, (size_t)MAX_PATH );

	// FRAMEWORK_BeforeFileSave
	IDMUSProdNotifySink* pINotifySink;
	if( SUCCEEDED ( m_pIDocRootNode->QueryInterface( IID_IDMUSProdNotifySink, (void **)&pINotifySink ) ) )
	{
		pINotifySink->OnUpdate( m_pIDocRootNode, FRAMEWORK_BeforeFileSave, awszPathName );
		pINotifySink->Release();
	}

	if( SUCCEEDED ( theApp.m_pFramework->AllocFileStream(bstrPathName, GENERIC_WRITE, ftFileType,
														 guidVersion, NULL, &pIStream) ) )
	{
		CJazzDocTemplate* pDocTemplate = (CJazzDocTemplate *)GetDocTemplate();
		ASSERT( pDocTemplate != NULL );

		IDMUSProdDocType* pIDocType = pDocTemplate->m_pIDocType;
		ASSERT( pIDocType != NULL );

		HRESULT hr = pIDocType->OnFileSave( pIStream, m_pIDocRootNode );

		if( SUCCEEDED ( hr ) )
		{
			fSuccess = TRUE;
		}
		else if( hr == E_NOTIMPL )
		{
			IPersistStream *pIPS;

			m_pIDocRootNode->QueryInterface( IID_IPersistStream, (void **)&pIPS );
			if( pIPS )
			{
				BOOL fClearDirty;

				if( theApp.m_nRuntimeSave == DOC_RUNTIME_SAVE
				||  theApp.m_nRuntimeSave == DOC_RUNTIME_SAVE_ALL )
				{
					fClearDirty = FALSE;
				}
				else
				{
					fClearDirty = TRUE;
				}

				if( SUCCEEDED ( pIPS->Save(pIStream, fClearDirty) ) )
				{
					fSuccess = TRUE;
				}

				pIPS->Release();
			}
		}

		pIStream->Release();
	}

	if( fSuccess )
	{
		// Store the Runtime filename
		if( ftFileType == FT_RUNTIME )
		{
			CString strDefaultRuntimeFile;

			m_pFileNode->ConstructRuntimePath( strDefaultRuntimeFile );
			strDefaultRuntimeFile += m_pFileNode->m_strRuntimeFileName;
			
			// Only store if equal to the default runtime filename
			if( strDefaultRuntimeFile.CompareNoCase( szPathName ) == 0 )
			{
				m_pFileNode->m_strRuntimeFile = szPathName;
			}
		}

		// Refresh Property sheet
		IDMUSProdPropSheet* pIPropSheet = theApp.GetPropertySheet();
		if( pIPropSheet )
		{
			pIPropSheet->RefreshTitleByObject( m_pFileNode );
			pIPropSheet->RefreshActivePageByObject( m_pFileNode );
			pIPropSheet->Release();
		}
	}

	// FRAMEWORK_AfterFileSave
	if( SUCCEEDED ( m_pIDocRootNode->QueryInterface( IID_IDMUSProdNotifySink, (void **)&pINotifySink ) ) )
	{
		pINotifySink->OnUpdate( m_pIDocRootNode, FRAMEWORK_AfterFileSave, awszPathName );
		pINotifySink->Release();
	}

	return fSuccess;
}


/////////////////////////////////////////////////////////////////////////////
// CComponentDoc::CanCloseFrame

BOOL CComponentDoc::CanCloseFrame( CFrameWnd* pFrame ) 
{
	return TRUE;	// Always ok to close a Component Frame
					// CJazzDoc::SaveModified() takes care of SaveModified()
					// for all of the Component documents.

//	return CCommonDoc::CanCloseFrame(pFrame);
}


/////////////////////////////////////////////////////////////////////////////
// CComponentDoc::AppendFilterSuffix

void CComponentDoc::AppendFilterSuffix( CString& filter, OPENFILENAME_NT4& ofn, CString* pstrDefaultExt )
{
	//Get the document's DocTemplate
	CJazzDocTemplate* pDocTemplate = (CJazzDocTemplate *)GetDocTemplate();
	ASSERT_VALID( pDocTemplate );
	ASSERT_KINDOF( CJazzDocTemplate, pDocTemplate );
	ASSERT( pDocTemplate->m_pIDocType != NULL );

	FileType ftFileType = FT_DESIGN;

	if( theApp.m_nRuntimeSave == DOC_RUNTIME_SAVE
	||  theApp.m_nRuntimeSave == DOC_RUNTIME_SAVE_ALL )
	{
		ftFileType = FT_RUNTIME;
	}

	CString strFilterExt, strFilterName;

	if( pDocTemplate->GetDocString( strFilterExt, CDocTemplate::filterExt )
	&&  !strFilterExt.IsEmpty()
	&&  pDocTemplate->GetDocString( strFilterName, CDocTemplate::filterName )
	&&  !strFilterName.IsEmpty() )
	{
		// a file based document template - add to filter list
		ASSERT( strFilterExt[0] == '.' );

		BOOL fContinue = TRUE;
		CString strMyFilter;
		CString strMyExt;
		CString strExt;
		BSTR bstrExt;
		int nFindPos;

		nFindPos = strFilterExt.Find( _T("*") );
		while( fContinue )
		{
			if( nFindPos == -1 )
			{
				fContinue = FALSE;

				nFindPos = strFilterExt.Find( _T(".") );
				if( nFindPos != 0 )
				{
					break;
				}
				strExt = strFilterExt;
			}
			else
			{
				strExt = strFilterExt.Left( nFindPos - 1 );
				strFilterExt = strFilterExt.Right( strFilterExt.GetLength() - (nFindPos + 1) ); 
			}
			bstrExt = strExt.AllocSysString();

			if( pDocTemplate->m_pIDocType->IsFileTypeExtension(ftFileType, bstrExt) == S_OK )
			{
				if( ofn.nFilterIndex == 0 )
				{
					nFindPos = strFilterName.Find( _T("*") );
					if( nFindPos != -1 )
					{
						strMyFilter = strFilterName.Left( nFindPos );
						strMyFilter += (TCHAR)'*';
						strMyFilter += strExt;

						strMyExt += (TCHAR)'*';
						strMyExt += strExt;

						if( pstrDefaultExt != NULL )
						{
							// set the default extension
							*pstrDefaultExt = ((LPCTSTR)strExt) + 1;  // skip the '.'
							ofn.lpstrDefExt = (LPTSTR)(LPCTSTR)(*pstrDefaultExt);
							ofn.nFilterIndex = ofn.nMaxCustFilter + 1;
						}
					}
					ASSERT( strMyFilter.IsEmpty() == FALSE );
					ASSERT( strMyExt.IsEmpty() == FALSE );
				}
				else
				{
					strMyFilter += (TCHAR)';';
					strMyFilter += (TCHAR)'*';
					strMyFilter += strExt;

					strMyExt += (TCHAR)';';
					strMyExt += (TCHAR)'*';
					strMyExt += strExt;
				}

			}

			nFindPos = strFilterExt.Find( _T("*") );
		}

		// add to filter
		filter = strMyFilter;
		ASSERT(!filter.IsEmpty());  // must have a file type name
		filter += (TCHAR)')';		
		filter += (TCHAR)'\0';		// next string please
		filter += strMyExt;
		filter += (TCHAR)'\0';		// next string please
		ofn.nMaxCustFilter++;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CComponentDoc::DoPromptFileSave

BOOL CComponentDoc::DoPromptFileSave( CJazzDocTemplate* pDocTemplate, UINT nResourceID, CString& fileName )
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	ASSERT( m_pFileNode != NULL );
	ASSERT( pDocTemplate != NULL );
	ASSERT_VALID( pDocTemplate );
	
	CFileDialog dlgFile( FALSE );

	CString strTitle;
	VERIFY( strTitle.LoadString( nResourceID ) );

	dlgFile.m_ofn.Flags |= ( OFN_HIDEREADONLY | OFN_PATHMUSTEXIST );

	CString strFilter;
	CString strDefault;

	AppendFilterSuffix( strFilter, dlgFile.m_ofn, &strDefault );

	// Append the "*.*" all files filter
	CString allFilter;
	VERIFY( allFilter.LoadString( AFX_IDS_ALLFILTER ) );
	strFilter += allFilter;
	strFilter += (TCHAR)'\0';   // next string please
	strFilter += _T("*.*");
	strFilter += (TCHAR)'\0';   // last string
	dlgFile.m_ofn.nMaxCustFilter++;

	dlgFile.m_ofn.lpstrFilter = strFilter;
	dlgFile.m_ofn.lpstrTitle = strTitle;
	dlgFile.m_ofn.lpstrFile = fileName.GetBuffer( _MAX_PATH );

	// Get the default directory for this template
	CString strDefaultDir;

	if( theApp.m_nRuntimeSave == DOC_RUNTIME_SAVE
	||  theApp.m_nRuntimeSave == DOC_RUNTIME_SAVE_ALL )
	{
		// Determine default runtime filename
		m_pFileNode->ConstructRuntimePath( strDefaultDir );
	}
	else
	{
		// Split path from filename
		m_pFileNode->ConstructFileName( strDefaultDir );
		int nFindPos = strDefaultDir.ReverseFind( (TCHAR)'\\' );
		if( nFindPos != -1 )
		{
			strDefaultDir = strDefaultDir.Left( nFindPos );
		}
//		theApp.GetDefaultDirForFileSave( dlgFile.m_ofn.lpstrFile, strDefaultDir );
	}
	if( !strDefaultDir.IsEmpty() )
	{
		dlgFile.m_ofn.lpstrInitialDir = strDefaultDir;
	}

	BOOL bResult = dlgFile.DoModal() == IDOK ? TRUE : FALSE;
	fileName.ReleaseBuffer();

	// Store the default directory for the next File Open/Save dialog
	if( bResult )
	{
		if( theApp.m_nRuntimeSave != DOC_RUNTIME_SAVE
		&&  theApp.m_nRuntimeSave != DOC_RUNTIME_SAVE_ALL )
		{
			theApp.SetDefaultDirForFileSave( dlgFile.m_ofn.lpstrFile, NULL );
		}
	}

	return bResult;
}


/////////////////////////////////////////////////////////////////////////////
// CComponentDoc::DetermineDefaultFileName

void CComponentDoc::DetermineDefaultFileName( CString& strName )
{
	int nFindPos;

	strName.Empty();
	
	//Get the document's DocTemplate
	CJazzDocTemplate* pDocTemplate = (CJazzDocTemplate *)GetDocTemplate();
	ASSERT( pDocTemplate != NULL );

	// Determine whether or not a Runtime save
	FileType ftFileType = FT_DESIGN;

	if( theApp.m_nRuntimeSave == DOC_RUNTIME_SAVE
	||  theApp.m_nRuntimeSave == DOC_RUNTIME_SAVE_ALL )
	{
		ftFileType = FT_RUNTIME;
	}

	// Figure out the "proposed" filename
	CString strFileName = GetPathName();
	ASSERT( AfxIsValidString(strFileName) );

	if( (strFileName.IsEmpty())
	||  (ftFileType == FT_RUNTIME) )
	{
		if( strFileName.IsEmpty() )
		{
			strName = GetTitle();

			// check for dubious filename
			int iBad = strName.FindOneOf( _T("%;/\\") );
			if( iBad != -1 )
			{
				strName.ReleaseBuffer( iBad );
			}
		}
		else
		{
			// Get name based on file title of path name
			theApp.SplitOutFileName( strFileName, TRUE, strName );

			// Remove extension
			nFindPos = strName.ReverseFind( (TCHAR)'.' );
			if( nFindPos != -1 )
			{
				strName = strName.Left( nFindPos );
			}
		}

		// append default extension
		if( pDocTemplate->m_pIDocType )
		{
			CString strFilterExt;

			if( pDocTemplate->GetDocString(strFilterExt, CDocTemplate::filterExt)
			&&  !strFilterExt.IsEmpty() )
			{
				ASSERT( strFilterExt[0] == '.' );

				BOOL fContinue = TRUE;
				CString strExt;
				BSTR bstrExt;

				nFindPos = strFilterExt.Find( _T("*") );
				while( fContinue )
				{
					if( nFindPos == -1 )
					{
						fContinue = FALSE;

						nFindPos = strFilterExt.Find( _T(".") );
						if( nFindPos != 0 )
						{
							break;
						}
						strExt = strFilterExt;
					}
					else
					{
						strExt = strFilterExt.Left( nFindPos - 1 );
						strFilterExt = strFilterExt.Right( strFilterExt.GetLength() - (nFindPos + 1) ); 
					}
					bstrExt = strExt.AllocSysString();

					if( pDocTemplate->m_pIDocType->IsFileTypeExtension(ftFileType, bstrExt) == S_OK )
					{
						strName += strExt;
						break;
					}

					nFindPos = strFilterExt.Find( _T("*") );
				}
			}
		}
	}
	else
	{
		// Get name based on file title of path name
		theApp.SplitOutFileName( strFileName, TRUE, strName );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CComponentDoc::DoSave

BOOL CComponentDoc::DoSave( LPCTSTR szPathName, BOOL bReplace )
{
	CString strName;
	CString strPath;
	CString strFileName;
	BOOL fSuccess = FALSE;
	BOOL fContinue = TRUE;
	BOOL fReplace = bReplace;
	BOOL fSaveAs = FALSE;
	int nFindPos;

	ASSERT( m_pFileNode != NULL );
	ASSERT( m_pIDocRootNode != NULL );

	// Get the document's DocTemplate
	CJazzDocTemplate* pDocTemplate = (CJazzDocTemplate *)GetDocTemplate();
	ASSERT( pDocTemplate != NULL );

	if( theApp.m_nRuntimeSave == DOC_RUNTIME_SAVE
	||  theApp.m_nRuntimeSave == DOC_RUNTIME_SAVE_ALL )
	{
		CString strDefaultRuntimeFileName;

		// Runtime files are separate entities that exist outside
		// of the Project.
		// Do not replace document filename or title with "Runtime" filename.
		fReplace = FALSE;

		// Determine default runtime filename
		m_pFileNode->ConstructRuntimePath( strPath );
		strName = m_pFileNode->m_strRuntimeFileName;
		strFileName = strPath + strName;
		strDefaultRuntimeFileName = strFileName;

		// Make sure the path exists
		fContinue = theApp.CreateTheDirectory( strPath );
		if( fContinue )
		{
			// Display 'Save As' dialog for the "Runtime" file
			if( szPathName == NULL
			||  theApp.m_nRuntimeSave == DOC_RUNTIME_SAVE )
			{
				fContinue = DoPromptFileSave( pDocTemplate, IDS_RUNTIME_SAVEAS, strName );
				strFileName = strName;

				// Determine the new "Runtime" default directory
				if( fContinue )
				{
					if( theApp.m_nRuntimeSave == DOC_RUNTIME_SAVE
					||  theApp.m_nRuntimeSave == DOC_RUNTIME_SAVE_ALL )
					{
						if( strFileName.CompareNoCase( strDefaultRuntimeFileName ) != 0 )
						{
							CString strTitle;
							CString strMsg;
							CString strTemp;
							CString strRelativePath;

							strTitle.LoadString( IDS_RUNTIME_DEFAULT_TITLE );

							m_pFileNode->ConstructRelativePath( strRelativePath );
							if( !strRelativePath.IsEmpty() )
							{
								if( strRelativePath.Right(1) != _T("\\") )
								{
									strRelativePath += _T("\\");
								}
							}
							strRelativePath += m_pFileNode->m_strName;

							AfxFormatString1( strMsg, IDS_RUNTIME_DEFAULT_MSG1, strRelativePath );
							AfxFormatString1( strTemp, IDS_RUNTIME_DEFAULT_MSG2, strDefaultRuntimeFileName );
							strMsg += strTemp;
							AfxFormatString1( strTemp, IDS_RUNTIME_DEFAULT_MSG3, strFileName );
							strMsg += strTemp;

							CMainFrame* pMainFrame = (CMainFrame *)theApp.m_pMainWnd;
							ASSERT( pMainFrame != NULL );

							switch( pMainFrame->MessageBox( strMsg, strTitle, MB_YESNOCANCEL ) )
							{
								case IDYES:
									m_pFileNode->LinkToRuntimeFile( strFileName );
									break;

								case IDCANCEL:
									fContinue = FALSE;
									break;
							}
						}
					}
				}
			}
		}
	}
	else
	{
		if( szPathName == NULL )
		{
			// Do not replace design document filename or title with "Save As" filename.
			fReplace = FALSE;
			fSaveAs = TRUE;

			strName = m_pFileNode->m_strName;
			fContinue = DoPromptFileSave( pDocTemplate, AFX_IDS_SAVEFILE, strName );
			strFileName = strName;
		}
		else
		{
			strFileName = szPathName;

			// Make sure the path exists
			nFindPos = strFileName.ReverseFind( (TCHAR)'\\' );
			if( nFindPos != -1 )
			{
				strPath = strFileName.Left( nFindPos + 1 );
				fContinue = theApp.CreateTheDirectory( strPath );
			}
		}
	}

	if( fContinue )
	{
		fSuccess = CCommonDoc::DoSave( strFileName, fReplace );

		if( fSuccess )
		{
			if( fSaveAs )
			{
				// See if the newly created file is in any of the opened Projects
				CProject* pProject = theApp.GetProjectByFileName( strFileName );
				if( pProject )
				{
					// Display the newly created file in the Project Tree
					theApp.OpenTheFile( strFileName, TGT_FILENODE_SAVEAS );
					pProject->Release();
				}
			}

			// Figure out new name for FileNode 
			strFileName = GetPathName();
			ASSERT( AfxIsValidString(strFileName) );

			nFindPos = strFileName.ReverseFind( (TCHAR)'\\' );
			if( nFindPos != -1 )
			{
				strFileName = strFileName.Right( strFileName.GetLength() - nFindPos - 1 );
				if( !strFileName.IsEmpty() )
				{
					theApp.m_pFramework->SetNodeName( m_pFileNode, strFileName );
				}
			}
		}
	}

	return fSuccess;
}


/////////////////////////////////////////////////////////////////////////////
// CComponentDoc::OnFileRuntimeSaveAs

void CComponentDoc::OnFileRuntimeSaveAs() 
{
	theApp.m_nRuntimeSave = DOC_RUNTIME_SAVE;

	DoSave( NULL );

	theApp.m_nRuntimeSave = DOC_NOT_RUNTIME_SAVE;
}


/////////////////////////////////////////////////////////////////////////////
// CComponentDoc::SetPathName

void CComponentDoc::SetPathName( LPCTSTR lpszPathName, BOOL bAddToMRU )
{
	CString strOrigPathName = m_strPathName;

	CCommonDoc::SetPathName( lpszPathName, FALSE );

	if( strOrigPathName.IsEmpty() )
	{
		return;
	}

	if(	theApp.m_fSendFileNameChangeNotification )
	{
		if( m_pFileNode
		&&  m_pFileNode->m_pIChildNode )
		{
			// Notify connected nodes that file name has changed
			theApp.m_pFramework->NotifyNodes( m_pFileNode->m_pIChildNode, FRAMEWORK_FileNameChange, NULL );

			// Notify m_pChildNode that filename has changed
			IDMUSProdNotifySink* pINotifySink;
			if( SUCCEEDED ( m_pFileNode->m_pIChildNode->QueryInterface( IID_IDMUSProdNotifySink, (void **)&pINotifySink ) ) )
			{
				pINotifySink->OnUpdate( m_pFileNode->m_pIChildNode, FRAMEWORK_FileNameChange, NULL );
				pINotifySink->Release();
			}
		}
	}
}
