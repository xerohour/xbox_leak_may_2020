// JazzDoc.cpp : implementation of the CJazzDoc class
//

#include "stdafx.h"
#include "JazzApp.h"
#include "MainFrm.h"

#include "CntrItem.h"
#include "NewProjectDlg.h"
#include "ioJazzDoc.h"
#include <mmreg.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CJazzDoc

IMPLEMENT_DYNCREATE(CJazzDoc, CCommonDoc)

BEGIN_MESSAGE_MAP(CJazzDoc, CCommonDoc)
	//{{AFX_MSG_MAP(CJazzDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Enable default OLE container implementation
	ON_UPDATE_COMMAND_UI(ID_OLE_EDIT_CONVERT, CCommonDoc::OnUpdateObjectVerbMenu)
	ON_COMMAND(ID_OLE_EDIT_CONVERT, CCommonDoc::OnEditConvert)
	ON_UPDATE_COMMAND_UI(ID_OLE_EDIT_LINKS, CCommonDoc::OnUpdateEditLinksMenu)
	ON_COMMAND(ID_OLE_EDIT_LINKS, CCommonDoc::OnEditLinks)
	ON_UPDATE_COMMAND_UI(ID_OLE_VERB_FIRST, CCommonDoc::OnUpdateObjectVerbMenu)
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CJazzDoc, CCommonDoc)
	//{{AFX_DISPATCH_MAP(CJazzDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//      DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_IDMUSProd to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

// {B6AED004-3BDF-11D0-89AC-00A0C9054129}
static const IID IID_IDMUSProd =
{ 0xb6aed004, 0x3bdf, 0x11d0, { 0x89, 0xac, 0x0, 0xa0, 0xc9, 0x5, 0x41, 0x29 } };

BEGIN_INTERFACE_MAP(CJazzDoc, CCommonDoc)
	INTERFACE_PART(CJazzDoc, IID_IDMUSProd, Dispatch)
END_INTERFACE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CJazzDoc construction/destruction

/////////////////////////////////////////////////////////////////////////////
// CJazzDoc::CJazzDoc

CJazzDoc::CJazzDoc()
{
	// Use OLE compound files
//	EnableCompoundFile();

	m_pProject = NULL;

//	EnableAutomation();
}


/////////////////////////////////////////////////////////////////////////////
// CJazzDoc::~CJazzDoc

CJazzDoc::~CJazzDoc()
{
	if( m_pProject )
	{
		theApp.RemoveProject( m_pProject );
		m_pProject->Release();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CJazzDoc::OnNewDocument

BOOL CJazzDoc::OnNewDocument()
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );
	ASSERT( theApp.m_pFramework != NULL );

	CString strProjectDir;
	CNewProjectDlg dlgNewProject;

	if( theApp.m_fInDuplicateProject )
	{
		// Get the active Project
		CProject* pActiveProject = theApp.GetActiveProject();
		if( pActiveProject == NULL )
		{
			return FALSE;
		}
		if( pActiveProject->m_pProjectDoc == NULL )
		{
			pActiveProject->Release();
			return FALSE;
		}
	
		// Put up 'Duplicate Project XXXX...' dialog
		AfxFormatString1( dlgNewProject.m_strTitle, IDS_DUPLICATE_PROJECT_TITLE_TEXT, pActiveProject->m_strName );
		if( dlgNewProject.DoModal() == IDCANCEL )
		{	
			pActiveProject->Release();
			return FALSE;
		}

		// Create Project directory
		if( !theApp.MakeTheDirectory(strProjectDir, dlgNewProject.m_strProjectDir, 0) )
		{
			pActiveProject->Release();
			return FALSE;
		}

		// Get number of files that are going to be saved
		int nNbrFiles = pActiveProject->GetNbrFiles( GNF_NOFLAGS );

		// Start progress bar
		CString strPrompt;
		strPrompt.LoadString( IDS_SAVING_PROJECT_AS );
		BSTR bstrPrompt = strPrompt.AllocSysString();
		HANDLE hKey;
		if( SUCCEEDED ( theApp.m_pFramework->StartProgressBar( 0, nNbrFiles, bstrPrompt, &hKey ) ) )
		{
			theApp.m_hKeyProgressBar = hKey;
		}
		theApp.m_pFramework->SetProgressBarPos( theApp.m_hKeyProgressBar, 0 );
		theApp.m_pFramework->SetProgressBarStep( theApp.m_hKeyProgressBar, 1 );

		// Do all other processing necessary for 'Duplicate Project'
		pActiveProject->Duplicate( strProjectDir );

		// Remove progress bar
		theApp.m_pFramework->SetProgressBarPos( theApp.m_hKeyProgressBar, 100 );
		Sleep( 10 );
		theApp.m_pFramework->EndProgressBar( theApp.m_hKeyProgressBar );

		// Free pActiveProject
		pActiveProject->Release();
	}
	else
	{
		// Put up 'New Project' dialog
		dlgNewProject.m_strTitle.LoadString( IDS_NEW_PROJECT_TEXT );
		if( dlgNewProject.DoModal() == IDCANCEL )
		{	
			return FALSE;
		}

		// Create Project directory
		if( !theApp.MakeTheDirectory(strProjectDir, dlgNewProject.m_strProjectDir, 0) )
		{
			return FALSE;
		}
	}

	// MFC new document processing
	if( !COleDocument::OnNewDocument() )
		return FALSE;

	// Create CProject
	m_pProject = new CProject;
	if( !m_pProject )
	{
		return FALSE;
	}

	m_pProject->AddRef();
	m_pProject->m_pProjectDoc = this;
	m_pProject->m_strName = dlgNewProject.m_strName;

	// Add to application Project list
	theApp.AddProject( m_pProject );

	// Add extension to Project name
	CString strFilterExt;

	CJazzDocTemplate* pTemplate = (CJazzDocTemplate *)GetDocTemplate();
	ASSERT( pTemplate != NULL );

	pTemplate->GetDocString( strFilterExt, CDocTemplate::filterExt );
	dlgNewProject.m_strName = dlgNewProject.m_strName + strFilterExt;
		
	// Set document title
	SetTitle( dlgNewProject.m_strName );

	// Set Project directory
	m_pProject->m_strProjectDir = strProjectDir;

	// Set Runtime directory
	CString strDir;
	strDir.LoadString( IDS_RUNTIME_DIR );
	m_pProject->m_strRuntimeDir = m_pProject->m_strProjectDir + strDir;
	if( m_pProject->m_strRuntimeDir.Right(1) != _T("\\") )
	{
		m_pProject->m_strRuntimeDir += _T("\\");
	}

	if( theApp.m_fInDuplicateProject )
	{
		// Get the active Project
		CProject* pActiveProject = theApp.GetActiveProject();
		if( pActiveProject )
		{
			// Copy PChannel name array to the new Project
			for( int i = 0 ;  i < 32 ;  i++ )
			{
				m_pProject->m_aPChannelNames[i] = pActiveProject->m_aPChannelNames[i];
			}
			
			// Copy PChannel name list to the new Project
			PChannelName* pPChannelName;
			PChannelName* pNewPChannelName;
			POSITION pos = pActiveProject->m_lstPChannelNames.GetHeadPosition();
			while( pos )
			{
				pPChannelName = pActiveProject->m_lstPChannelNames.GetNext( pos );

				pNewPChannelName = new PChannelName;
				if( pNewPChannelName )
				{
					pNewPChannelName->m_strName = pPChannelName->m_strName;
					pNewPChannelName->m_dwPChannel = pPChannelName->m_dwPChannel;
					m_pProject->m_lstPChannelNames.AddTail( pNewPChannelName );
				}
			}

			// Copy other things to the new Project
			m_pProject->m_strDescription = pActiveProject->m_strDescription;

			// Deal with bookmarks
			theApp.DuplicateBookmarks( pActiveProject, m_pProject );

			// Free pActiveProject
			pActiveProject->Release();
		}
	}

	// Create the .pro file
	CString strFileName = m_pProject->m_strProjectDir + dlgNewProject.m_strName; 
	if( DoSave(strFileName) == FALSE )
	{
		m_pProject->Release();
		m_pProject = NULL;
		return FALSE;
	}

	m_pIDocRootNode = (IDMUSProdNode *)m_pProject;
	m_pIDocRootNode->AddRef();

	// Save the parent directory of the Project file for File Open
	theApp.SetProjectDirForFileOpen( strFileName );

	// Sync Project information with files on hard drive
	CString strPath;
	m_pProject->ConstructPath( strPath );
	ASSERT( !strPath.IsEmpty() );

	m_pProject->SyncFilesWithDisk( m_pProject, strPath );
	m_pProject->DeleteNonExistentFiles();
	m_pProject->SyncListInfo();

	// Make sure default "Runtime" folders exist for all extensions
	m_pProject->DetermineDefaultRuntimeFolders();

	// Add node to Project Tree
	if( FAILED ( theApp.m_pFramework->AddNode((IDMUSProdNode *)m_pProject, NULL) ) )
	{
		m_pProject->Release();
		m_pProject = NULL;
		return FALSE;
	}

	CTreeCtrl* pTreeCtrl = theApp.GetProjectTreeCtrl();
	if( pTreeCtrl )
	{
		pTreeCtrl->SetFocus();
	}
	theApp.m_pFramework->ShowTreeNode( m_pIDocRootNode );

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CJazzDoc serialization

void CJazzDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}

	// Calling the base class CCommonDoc enables serialization
	//  of the container document's COleClientItem objects.
	CCommonDoc::Serialize(ar);
}


/////////////////////////////////////////////////////////////////////////////
// CJazzDoc diagnostics

#ifdef _DEBUG
void CJazzDoc::AssertValid() const
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );
	CCommonDoc::AssertValid();
}

void CJazzDoc::Dump(CDumpContext& dc) const
{
	CCommonDoc::Dump(dc);
}
#endif //_DEBUG


/////////////////////////////////////////////////////////////////////////////
// CJazzDoc commands

/////////////////////////////////////////////////////////////////////////////
// CJazzDoc::OnOpenDocument

BOOL CJazzDoc::OnOpenDocument( LPCTSTR szPathName ) 
{
	ASSERT( theApp.m_pFramework != NULL );

	// Make sure this is a valid Project File
	if( theApp.IsValidProjectFile( szPathName, TRUE ) == FALSE )
	{
		return FALSE;	
	}

	// Load the Project
	if( !LoadProject(szPathName) )
	{
		return FALSE;
	}
	ASSERT( m_pProject != NULL );

	// Sync Project information with files on hard drive
	CString strPath;
	m_pProject->ConstructPath( strPath );
	ASSERT( !strPath.IsEmpty() );

	m_pProject->SyncFilesWithDisk( m_pProject, strPath );
	m_pProject->DeleteNonExistentFiles();
	m_pProject->SyncListInfo();

	// Make sure default "Runtime" folders exist for all extensions
	m_pProject->DetermineDefaultRuntimeFolders();

	// Add node to Project Tree
	if( FAILED ( theApp.m_pFramework->AddNode((IDMUSProdNode *)m_pProject, NULL) ) )
	{
		m_pProject->Release();
		m_pProject = NULL;
		return FALSE;
	}

	// Set the document path name
	SetPathName( szPathName, TRUE );

	CTreeCtrl* pTreeCtrl = theApp.GetProjectTreeCtrl();
	if( pTreeCtrl )
	{
		pTreeCtrl->SetFocus();
	}
	theApp.m_pFramework->ShowTreeNode( m_pIDocRootNode );

	/* Per bug 54043 (CC: DMP: New feature: Better Project File Handling (repeal last-opened policy)),
	   No longer do this.

	// Open the editors that were open when the Project was saved
	if( m_pProject->m_pBookmark )
	{ 
		// If m_guid is zero, then we GPF'd last time we tried to apply the Bookmark
		if( ::IsEqualGUID(m_pProject->m_pBookmark->m_guid, GUID_AllZeros) == FALSE )
		{
			if( pTreeCtrl )
			{
				pTreeCtrl->SetRedraw( FALSE );
			}

			SetOpenEditorGUIDInFile( szPathName, GUID_AllZeros );
			m_pProject->m_pBookmark->Apply( FALSE );
			SetOpenEditorGUIDInFile( szPathName, m_pProject->m_pBookmark->m_guid );

			if( pTreeCtrl )
			{
				pTreeCtrl->SetRedraw( TRUE );
			}

			delete m_pProject->m_pBookmark;
			m_pProject->m_pBookmark = NULL;
		}
	}
	*/
	
	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CJazzDoc::SetOpenEditorGUIDInFile

void CJazzDoc::SetOpenEditorGUIDInFile( LPCTSTR szPathName, GUID guid )
{
	IStream* pIStream;
    IDMUSProdRIFFStream* pIRiffStream;
    MMCKINFO ckMain;
    MMCKINFO ckList;
    MMCKINFO ckOpen;
	MMCKINFO ck;

	// Open the file
	if( SUCCEEDED ( AllocFileReadWriteStream(szPathName, &pIStream) ) )
	{
		if( SUCCEEDED( AllocRIFFStream( pIStream, &pIRiffStream ) ) )
		{
			// Find the Project Form header
			ckMain.fccType = FOURCC_PROJECT_FORM;
			if( pIRiffStream->Descend( &ckMain, NULL, MMIO_FINDRIFF ) == 0 )
			{
				// Find the Project List header
				ckList.fccType = FOURCC_PROJECT_LIST;
				if( pIRiffStream->Descend( &ckList, &ckMain, MMIO_FINDLIST ) == 0 )
				{
					// Find the OpenEditor List header
					ckOpen.fccType = FOURCC_OPEN_EDITORS_LIST;
					if( pIRiffStream->Descend( &ckOpen, &ckList, MMIO_FINDLIST ) == 0 )
					{
						// Find the GUID
						ck.ckid = DMUS_FOURCC_GUID_CHUNK;
						if( pIRiffStream->Descend( &ck, &ckOpen, MMIO_FINDCHUNK ) == 0 )
						{
							// Replace the GUID
							DWORD dwBytesWritten;
							DWORD dwSize = min( ck.cksize, sizeof( GUID ) );
							HRESULT	hr = pIStream->Write( &guid, dwSize, &dwBytesWritten);
						}
					}
				}
			}

			pIRiffStream->Release();
		}

		pIStream->Release();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CJazzDoc::OnCloseDocument

void CJazzDoc::OnCloseDocument() 
{
	CProject* pThisProject = m_pProject;
	
	if( pThisProject )
	{
		pThisProject->CloseAllFiles();
	}

	CCommonDoc::OnCloseDocument();

	if( pThisProject )
	{
		CProject* pProject = theApp.GetActiveProject();
		if( pProject )
		{
			if( pProject == pThisProject )
			{
				theApp.SetActiveProject( NULL );
			}
			pProject->Release();
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CJazzDoc::LoadProject

BOOL CJazzDoc::LoadProject( LPCTSTR szPathName ) 
{
	IStream* pIStream = NULL;
    IDMUSProdRIFFStream* pIRiffStream = NULL;
	BOOL fGenerateNewGUIDs;
	BOOL fSuccess = TRUE;
    MMCKINFO ckMain;
	MMCKINFO ck;

	ASSERT( szPathName != NULL );
	ASSERT( AfxIsValidString(szPathName) );

	// Open the file
	CString strPathName = szPathName;
	BSTR bstrPathName = strPathName.AllocSysString();

	if( FAILED ( theApp.m_pFramework->AllocFileStream(bstrPathName, GENERIC_READ, FT_UNKNOWN,
													  GUID_AllZeros, NULL, &pIStream) ) )
	{
		fSuccess = FALSE;
		goto ON_ERROR;
	}

	// Allocate a RIFF Stream
	if( FAILED( AllocRIFFStream( pIStream, &pIRiffStream ) ) )
	{
		fSuccess = FALSE;
		goto ON_ERROR;
	}

	// Find the Project Form ID header
	ckMain.fccType = FOURCC_PROJECT_FORM;
	if( pIRiffStream->Descend( &ckMain, NULL, MMIO_FINDRIFF ) != 0 )
	{
		fSuccess = FALSE;
		goto ON_ERROR;
	}

	fGenerateNewGUIDs = FALSE;

	// Load the chunks of data in the Project file
	while( pIRiffStream->Descend( &ck, &ckMain, 0 ) == 0 )
	{
		switch( ck.ckid )
		{
			case FOURCC_LIST:
				switch( ck.fccType )
				{
					case FOURCC_PROJECT_LIST:
					{
						ASSERT( m_pProject == NULL );
						m_pProject = new CProject;
						if( m_pProject == NULL )
						{
							fSuccess = FALSE;
							goto ON_ERROR;
						}

						m_pProject->AddRef();
						m_pProject->m_pProjectDoc = this;
						m_pIDocRootNode = (IDMUSProdNode *)m_pProject;
						m_pIDocRootNode->AddRef();
						
						if( FAILED ( m_pProject->LoadTheProject( pIRiffStream, &ck ) ) )
						{
							fSuccess = FALSE;
							goto ON_ERROR;
						}

						// See if a Project already exists with this GUID.
						// Could happen if user copied Project via Explorer.
						CProject* pProject = theApp.GetProjectByGUID( m_pProject->m_guid );
						if( pProject )
						{
							fGenerateNewGUIDs = TRUE;
							pProject->Release();
						}

						// Add to application's Project list
						theApp.AddProject( m_pProject );
						break;
					}

					case FOURCC_FILE_LIST:
					{
						ASSERT( m_pProject != NULL );
						
						CFileNode* pFileNode = new CFileNode;
						if( pFileNode == NULL )
						{
							fSuccess = FALSE;
							goto ON_ERROR;
						}
					
						// Set parent node
						pFileNode->SetParentNode( m_pProject );

						// Add to Project's file list
						m_pProject->AddFile( pFileNode );

						// Read RIFF chunks associates with the file
						if( FAILED ( pFileNode->LoadTheFile( pIRiffStream, &ck ) ) )
						{
							m_pProject->RemoveFile( pFileNode );
							fSuccess = FALSE;
							goto ON_ERROR;
						}
						break;
					}
				}
				break;
		}

		pIRiffStream->Ascend( &ck, 0 );
	}

	if( fGenerateNewGUIDs )
	{
		m_pProject->GenerateNewGUIDs();
	}

ON_ERROR:
	if( pIRiffStream )
	{
		pIRiffStream->Release();
	}

	if( pIStream )
	{
		pIStream->Release();
	}

	return fSuccess;
}


/////////////////////////////////////////////////////////////////////////////
// CJazzDoc::SaveProject

BOOL CJazzDoc::SaveProject( LPCTSTR szPathName ) 
{
	IStream* pIStream;
    IDMUSProdRIFFStream* pIRiffStream;
    MMCKINFO ckMain;

	ASSERT( m_pProject != NULL );

	BOOL fSuccess = FALSE;

	// Open the file
	CString strPathName = szPathName;
	BSTR bstrPathName = strPathName.AllocSysString();

	if( SUCCEEDED ( theApp.m_pFramework->AllocFileStream(bstrPathName, GENERIC_WRITE, FT_RUNTIME,
														 GUID_CurrentVersion, NULL, &pIStream) ) )
	{
		if( SUCCEEDED( AllocRIFFStream( pIStream, &pIRiffStream ) ) )
		{
			ckMain.fccType = FOURCC_PROJECT_FORM;

			if( pIRiffStream->CreateChunk( &ckMain, MMIO_CREATERIFF ) == 0
			&&  SUCCEEDED( m_pProject->SaveTheProject( pIRiffStream ) )
			&&  pIRiffStream->Ascend( &ckMain, 0 ) == 0 )
			{
				fSuccess = TRUE;
			}

			pIRiffStream->Release();
		}

		pIStream->Release();
	}

	return fSuccess;
}


/////////////////////////////////////////////////////////////////////////////
// CJazzDoc::SaveModified

BOOL CJazzDoc::SaveModified() 
{
	BOOL fContinue;
	CString strPrompt;

	ASSERT( theApp.m_pFramework != NULL );
	ASSERT( m_pProject != NULL );
	
	CMainFrame* pMainFrame = (CMainFrame *)theApp.m_pMainWnd;
	ASSERT( pMainFrame != NULL );

	// Start progress bar
	int nNbrFiles = m_pProject->GetNbrFiles( GNF_NOFLAGS );
	strPrompt.LoadString( IDS_SAVING_PROJECT );
	BSTR bstrPrompt = strPrompt.AllocSysString();
	HANDLE hKey;
	if( SUCCEEDED ( theApp.m_pFramework->StartProgressBar( 0, nNbrFiles+1, bstrPrompt, &hKey ) ) )
	{
		theApp.m_hKeyProgressBar = hKey;
	}
	theApp.m_pFramework->SetProgressBarPos( theApp.m_hKeyProgressBar, 0 );
	theApp.m_pFramework->SetProgressBarStep( theApp.m_hKeyProgressBar, 1 );

	// Make sure all Component files have been saved
	fContinue = m_pProject->SaveAllFiles();

	// Save the Project file
	theApp.m_fInJazzDocSaveModified = TRUE;
	if( fContinue )
	{
		fContinue = DoFileSave();
	}
	theApp.m_fInJazzDocSaveModified = FALSE;

	// Remove progress bar
	theApp.m_pFramework->SetProgressBarPos( theApp.m_hKeyProgressBar, 100 );
	Sleep( 10 );
	theApp.m_pFramework->EndProgressBar( theApp.m_hKeyProgressBar );

	return fContinue;
}


/////////////////////////////////////////////////////////////////////////////
// CJazzDoc::OnSaveDocument

BOOL CJazzDoc::OnSaveDocument( LPCTSTR szPathName ) 
{
	BOOL fContinue;
	CString strPrompt;

	ASSERT( theApp.m_pFramework != NULL );
	ASSERT( m_pProject != NULL );
	
	CMainFrame* pMainFrame = (CMainFrame *)theApp.m_pMainWnd;
	ASSERT( pMainFrame != NULL );

	// Start progress bar
	int nNbrFiles = m_pProject->GetNbrFiles( GNF_NOFLAGS );
	strPrompt.LoadString( IDS_SAVING_PROJECT );
	BSTR bstrPrompt = strPrompt.AllocSysString();
	HANDLE hKey;
	if( SUCCEEDED ( theApp.m_pFramework->StartProgressBar( 0, nNbrFiles+1, bstrPrompt, &hKey ) ) )
	{
		theApp.m_hKeyProgressBar = hKey;
	}
	theApp.m_pFramework->SetProgressBarPos( theApp.m_hKeyProgressBar, 0 );
	theApp.m_pFramework->SetProgressBarStep( theApp.m_hKeyProgressBar, 1 );

	// Make sure we didn't just call CProject::SaveAllFiles
	if( theApp.m_fInJazzDocSaveModified == FALSE )
	{
		// Make sure all Component files have been saved
		fContinue = m_pProject->SaveAllFiles();
	}

	// Save the Project file
	if( fContinue )
	{
		fContinue = SaveProject( szPathName );
	}

	// Remove progress bar
	theApp.m_pFramework->SetProgressBarPos( theApp.m_hKeyProgressBar, 100 );
	Sleep( 10 );
	theApp.m_pFramework->EndProgressBar( theApp.m_hKeyProgressBar );

	// Refresh Property sheet
	IDMUSProdPropSheet* pIPropSheet = theApp.GetPropertySheet();
	if( pIPropSheet )
	{
		pIPropSheet->RefreshTitleByObject( m_pProject );
		pIPropSheet->RefreshActivePageByObject( m_pProject );
		pIPropSheet->Release();
	}

	return fContinue;
}


/////////////////////////////////////////////////////////////////////////////
// CJazzDoc::DoFileSave

BOOL CJazzDoc::DoFileSave()
{
	BOOL fSuccess = FALSE;

	// Comment out until we implement Project "Save As..." 

//	DWORD dwAttrib = GetFileAttributes( m_strPathName );
//	if( dwAttrib & FILE_ATTRIBUTE_READONLY )
//	{
//		// we do not have read-write access or the file does not (now) exist
//		fSuccess = DoSave( NULL );
//		if( fSuccess == FALSE )
//		{
//			TRACE0( "Warning: File save with new name failed.\n" );
//		}
//	}
//	else
	{
		fSuccess = DoSave( m_strPathName );
		if( fSuccess == FALSE )
		{
			TRACE0( "Warning: File save failed.\n" );
		}
	}

	if( fSuccess == FALSE )
	{
		CString strMsg;
		CString strProjectName;

		if( m_pProject )
		{
			strProjectName = m_pProject->m_strName;
		}

		if( theApp.m_fShutDown )
		{
			AfxFormatString1( strMsg, IDS_CONTINUE_SHUTDOWN, strProjectName );
			if( AfxMessageBox( strMsg, MB_OKCANCEL ) == IDOK )
			{
				fSuccess = TRUE;
			}
		}
		else if( theApp.m_fCloseProject )
		{
			AfxFormatString1( strMsg, IDS_CONTINUE_CLOSE, strProjectName );
			if( AfxMessageBox( strMsg, MB_OKCANCEL ) == IDOK )
			{
				fSuccess = TRUE;
			}
		}
	}

	return fSuccess;
}


/////////////////////////////////////////////////////////////////////////////
// CJazzDoc::DoSave

BOOL CJazzDoc::DoSave( LPCTSTR szPathName, BOOL bReplace )
{
	BOOL fSuccess = FALSE;
	CString strName;
	
	ASSERT( m_pProject != NULL );

	//Get the document's DocTemplate
	CJazzDocTemplate* pDocTemplate = (CJazzDocTemplate *)GetDocTemplate();
	ASSERT( pDocTemplate != NULL );

	// Figure out the "proposed" filename
	CString strFileName = GetPathName();
	ASSERT( AfxIsValidString(strFileName) );

	if( strFileName.IsEmpty() )
	{
		strName = GetTitle();

		// check for dubious filename
		int iBad = strName.FindOneOf( _T("%;/\\") );
		if( iBad != -1 )
		{
			strName.ReleaseBuffer( iBad );
		}

		// append the default suffix if there is one
		CString strExt;
		if( pDocTemplate->GetDocString(strExt, CDocTemplate::filterExt)
		&&  !strExt.IsEmpty() )
		{
			ASSERT( strExt[0] == '.' );
			strName += strExt;
		}
	}
	else
	{
		// Get name based on file title of path name
		theApp.SplitOutFileName( strFileName, TRUE, strName );
	}

	BOOL fContinue = TRUE;

	if( szPathName == NULL )
	{
		strFileName = m_pProject->m_strProjectDir + strName;
		fContinue = theApp.DoPromptFileName( strFileName, AFX_IDS_SAVEFILE,
											 OFN_HIDEREADONLY | OFN_PATHMUSTEXIST,
											 FALSE, pDocTemplate );
	}
	else
	{
		strFileName = szPathName;
	}

	if( fContinue )
	{
		fSuccess = CCommonDoc::DoSave( strFileName );
	}

	return fSuccess;
}
