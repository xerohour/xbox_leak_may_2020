// JazzApp.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"

#include <PrivateTransport.h>

#include <initguid.h>
#include "JazzApp.h"
#include "Timeline.h"
#include "StripMgr.h"
#include "SegmentDesigner.h"
#include "StyleDesigner.h"
#include "DLSDesigner.h"
#include "ChordMapDesigner.h"
#include "ScriptDesigner.h"
#include "ContainerDesigner.h"
#include "BandEditor.h"
#include "Loader.h"
#include "conductor.h"
#include <dmusicp.h>
#include <dsoundp.h>

#include "MainFrm.h"
#include "ChildFrm.h"
#include "JazzView.h" 
#include "Splash.h"
#include "HelpBindHost.h"
#include "NewBookmarkDlg.h"
#include "NewProjectDlg.h"
#include "ioJazzDoc.h"
#include "FileNewDlg.h"

#include <PrivateDocType.h>
#include <PrivateUnpackingFiles.h>
#include <dmusicp.h>
#include <dsoundp.h>
#include <io.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static const TCHAR szShellOpenFmt[] = _T("%s\\shell\\open\\%s");
static const TCHAR szShellNewFmt[] = _T("%s\\ShellNew");
static const TCHAR szShellNewValueName[] = _T("NullFile");
static const TCHAR szShellNewValue[] = _T("");

static const TCHAR szOpenArg[] = _T(" \"%1\"");
static const TCHAR szCommand[] = _T("command");

static const TCHAR szDDEExec[] = _T("ddeexec");
static const TCHAR szDDEOpen[] = _T("[open(\"%1\")]");

static const TCHAR szDefaultIconFmt[] = _T("%s\\DefaultIcon");
static const TCHAR szDefaultIcon[] = _T("%s,0");

LPCTSTR lpszUniqueClass = _T("JzApBR");


/////////////////////////////////////////////////////////////////////////////
// CJzFileGUIDs constructor/destructor 

CJzFileGUIDs::CJzFileGUIDs( LPCTSTR pszRelativePathName, GUID guidExistingFile )
{
	ASSERT( pszRelativePathName != NULL );

	m_strRelativePathName = pszRelativePathName;
    m_guidExistingFile = guidExistingFile;
	CoCreateGuid( &m_guidNewFile ); 
}

CJzFileGUIDs::~CJzFileGUIDs()
{
}


/////////////////////////////////////////////////////////////////////////////
// CJzFileName constructor/destructor 

CJzFileName::CJzFileName( LPCTSTR pszFileName )
{
	ASSERT( pszFileName != NULL );
	m_strFileName = pszFileName;
	m_nPriority = GetPriority();
	m_fBeingLoaded = false;
	GetListInfo();
}

CJzFileName::~CJzFileName()
{
}

void CJzFileName::GetListInfo( void )
{
	m_strObjectName.Empty();
	memset( &m_guidObject, 0, sizeof(GUID) );

	IDMUSProdDocType* pIDocType = theApp.GetDocType( m_strFileName );
	if( pIDocType )
	{
		DMUSProdListInfo ListInfo;
		ZeroMemory( &ListInfo, sizeof(ListInfo) );
		ListInfo.wSize = sizeof(ListInfo);

		BSTR bstrFileName = m_strFileName.AllocSysString();

		IStream* pIStream;
		if( SUCCEEDED ( theApp.m_pFramework->AllocFileStream ( bstrFileName, GENERIC_READ, FT_UNKNOWN,
															   GUID_AllZeros, NULL, &pIStream ) ) )
		{
			if( SUCCEEDED ( pIDocType->GetListInfo ( pIStream, &ListInfo ) ) )
			{
				if( ListInfo.bstrName )
				{
					m_strObjectName = ListInfo.bstrName;
					::SysFreeString( ListInfo.bstrName );
				}
				if( ListInfo.bstrDescriptor )
				{
					::SysFreeString( ListInfo.bstrDescriptor );
				}
				memcpy( &m_guidObject, &ListInfo.guidObject, sizeof(GUID) );
			}

			pIStream->Release();
		}

		pIDocType->Release();
	}
}

short CJzFileName::GetPriority( void )
{
	// MAX_FILE_OPEN_PRIORITY reserved for Project files
	// (MAX_FILE_OPEN_PRIORITY - 1) reserved for Container files
	// (MAX_FILE_OPEN_PRIORITY - 2) reserved for Song files
	// (MAX_FILE_OPEN_PRIORITY - 3) reserved for Segment files
	short nPriority = MAX_FILE_OPEN_PRIORITY - 4;
	
	IDMUSProdDocType* pIDocType = theApp.GetDocType( m_strFileName );
	if( pIDocType )
	{
		IDMUSProdDocType* pIPriorityDocType;

		if( SUCCEEDED ( theApp.m_pFramework->FindDocTypeByNodeId( GUID_CollectionNode, &pIPriorityDocType ) ) )
		{
			if( pIDocType == pIPriorityDocType )
			{
				nPriority = 1;
			}

			pIPriorityDocType->Release();
		}

		if( nPriority == (MAX_FILE_OPEN_PRIORITY - 4) )
		{
			if( SUCCEEDED ( theApp.m_pFramework->FindDocTypeByNodeId( GUID_StyleNode, &pIPriorityDocType ) ) )
			{
				if( pIDocType == pIPriorityDocType )
				{
					nPriority = 2;
				}

				pIPriorityDocType->Release();
			}
		}

		if( nPriority == (MAX_FILE_OPEN_PRIORITY - 4) )
		{
			if( SUCCEEDED ( theApp.m_pFramework->FindDocTypeByNodeId( GUID_PersonalityNode, &pIPriorityDocType ) ) )
			{
				if( pIDocType == pIPriorityDocType )
				{
					nPriority = 3;
				}

				pIPriorityDocType->Release();
			}
		}

		if( nPriority == (MAX_FILE_OPEN_PRIORITY - 4) )
		{
			if( SUCCEEDED ( theApp.m_pFramework->FindDocTypeByNodeId( GUID_WaveNode, &pIPriorityDocType ) ) )
			{
				if( pIDocType == pIPriorityDocType )
				{
					nPriority = 4;
				}

				pIPriorityDocType->Release();
			}
		}

		if( nPriority == (MAX_FILE_OPEN_PRIORITY - 4) )
		{
			if( SUCCEEDED ( theApp.m_pFramework->FindDocTypeByNodeId( GUID_ScriptNode, &pIPriorityDocType ) ) )
			{
				if( pIDocType == pIPriorityDocType )
				{
					nPriority = 5;
				}

				pIPriorityDocType->Release();
			}
		}

		if( nPriority == (MAX_FILE_OPEN_PRIORITY - 4) )
		{
			if( SUCCEEDED ( theApp.m_pFramework->FindDocTypeByNodeId( GUID_SegmentNode, &pIPriorityDocType ) ) )
			{
				if( pIDocType == pIPriorityDocType )
				{
					nPriority = MAX_FILE_OPEN_PRIORITY - 3;
				}

				pIPriorityDocType->Release();
			}
		}

		/*
		if( nPriority == (MAX_FILE_OPEN_PRIORITY - 4) )
		{
			if( SUCCEEDED ( theApp.m_pFramework->FindDocTypeByNodeId( GUID_SongNode, &pIPriorityDocType ) ) )
			{
				if( pIDocType == pIPriorityDocType )
				{
					nPriority = MAX_FILE_OPEN_PRIORITY - 2;
				}

				pIPriorityDocType->Release();
			}
		}
		*/

		if( nPriority == (MAX_FILE_OPEN_PRIORITY - 4) )
		{
			if( SUCCEEDED ( theApp.m_pFramework->FindDocTypeByNodeId( GUID_ContainerNode, &pIPriorityDocType ) ) )
			{
				if( pIDocType == pIPriorityDocType )
				{
					nPriority = MAX_FILE_OPEN_PRIORITY - 1;
				}

				pIPriorityDocType->Release();
			}
		}

		pIDocType->Release();
	}
	else
	{
		if( theApp.IsValidProjectFile( m_strFileName, FALSE ) )
		{
			nPriority = MAX_FILE_OPEN_PRIORITY;
		}
	}
	
	return nPriority;
}

/////////////////////////////////////////////////////////////////////////////
// CJzTrackFileOpen constructor/destructor 

CJzTrackFileOpen::CJzTrackFileOpen( void )
{
	m_dwRef = 0;

	IDMUSProdComponent* pIComponent;
	IDLSReferenceLoadNotify* pILoadNotify;

	if( SUCCEEDED ( theApp.m_pFramework->FindComponent( CLSID_DLSComponent,  &pIComponent ) ) )
	{
		if( SUCCEEDED ( pIComponent->QueryInterface( IID_IDLSReferenceLoadNotify, (void**)&pILoadNotify ) ) )
		{
			pILoadNotify->MarkLoadStart();
			pILoadNotify->Release();
		}

		pIComponent->Release();
	}

	// Eliminate flicker in project tree when opening referenced files
	CTreeCtrl* pTreeCtrl = theApp.GetProjectTreeCtrl();
	if( pTreeCtrl )
	{
		pTreeCtrl->SetRedraw( FALSE );
	}
}

	
CJzTrackFileOpen::~CJzTrackFileOpen( void )
{
	ASSERT( theApp.m_pJzTrackFileOpen == this );	// Should never happen!
	theApp.m_pJzTrackFileOpen = NULL;

	IDMUSProdComponent* pIComponent;
	IDLSReferenceLoadNotify* pILoadNotify;

	if( SUCCEEDED ( theApp.m_pFramework->FindComponent( CLSID_DLSComponent,  &pIComponent ) ) )
	{
		if( SUCCEEDED ( pIComponent->QueryInterface( IID_IDLSReferenceLoadNotify, (void**)&pILoadNotify ) ) )
		{
			pILoadNotify->MarkLoadEnd();
			pILoadNotify->Release();
		}

		pIComponent->Release();
	}

	CTreeCtrl* pTreeCtrl = theApp.GetProjectTreeCtrl();
	if( pTreeCtrl )
	{
		pTreeCtrl->SetRedraw( TRUE );
		pTreeCtrl->Invalidate();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CJzTrackFileOpen IUnknown implementation

HRESULT CJzTrackFileOpen::QueryInterface( REFIID riid, LPVOID FAR* ppvObj )
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	if( ::IsEqualIID(riid, IID_IUnknown) )
    {
        AddRef();
        *ppvObj = (IUnknown *)this;
        return S_OK;
    }

    *ppvObj = NULL;
    return E_NOINTERFACE;
}

ULONG CJzTrackFileOpen::AddRef()
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

    return ++m_dwRef;
}

ULONG CJzTrackFileOpen::Release()
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
// CJazzApp

BEGIN_MESSAGE_MAP(CJazzApp, CWinApp)
	//{{AFX_MSG_MAP(CJazzApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_WINDOW_CLOSEALL, OnWindowCloseAll)
	ON_COMMAND(ID_FILE_OPENPROJECT, OnFileOpenProject)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVEPROJECT, OnUpdateFileSaveProject)
	ON_COMMAND(ID_FILE_SAVEPROJECT, OnFileSaveProject)
	ON_UPDATE_COMMAND_UI(ID_FILE_CLOSEPROJECT, OnUpdateFileCloseProject)
	ON_COMMAND(ID_FILE_CLOSEPROJECT, OnFileCloseProject)
	ON_COMMAND(ID_FILE_RUNTIME_SAVEALL, OnFileRuntimeSaveAllFiles)
	ON_UPDATE_COMMAND_UI(ID_FILE_RUNTIME_SAVEALL, OnUpdateFileRuntimeSaveAllFiles)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_COMMAND(ID_BOOKMARK_SET, OnBookmarkCreate)
	ON_UPDATE_COMMAND_UI(ID_BOOKMARK_SET, OnUpdateBookmarkCreate)
	ON_COMMAND(ID_BOOKMARK_CLEAR, OnBookmarkRemove)
	ON_UPDATE_COMMAND_UI(ID_BOOKMARK_CLEAR, OnUpdateBookmarkRemove)
	ON_COMMAND(ID_BOOKMARK_CLEARALL, OnBookmarkRemoveAll)
	ON_UPDATE_COMMAND_UI(ID_BOOKMARK_CLEARALL, OnUpdateBookmarkRemoveAll)
	ON_UPDATE_COMMAND_UI(ID_FILE_OPEN, OnUpdateFileOpen)
	ON_COMMAND(ID_FILE_NEW, OnFileNew)
	ON_COMMAND(IDM_IMPORT_MID, OnImportMid)
	ON_UPDATE_COMMAND_UI(IDM_IMPORT_MID, OnUpdateImportMid)
	ON_COMMAND(IDM_IMPORT_SEC, OnImportSec)
	ON_UPDATE_COMMAND_UI(IDM_IMPORT_SEC, OnUpdateImportSec)
	ON_UPDATE_COMMAND_UI(ID_FILE_DUPLICATE_PROJECT, OnUpdateFileDuplicateProject)
	ON_COMMAND(ID_FILE_DUPLICATE_PROJECT, OnFileDuplicateProject)
	ON_UPDATE_COMMAND_UI(IDM_IMPORT_WAV, OnUpdateImportWav)
	ON_COMMAND(IDM_IMPORT_WAV, OnImportWav)
	ON_UPDATE_COMMAND_UI(IDM_IMPORT_WAV_VAR, OnUpdateImportWavVariations)
	ON_COMMAND(IDM_IMPORT_WAV_VAR, OnImportWavVariations)
	//}}AFX_MSG_MAP
	ON_UPDATE_COMMAND_UI(IDC_BOOKMARK_COMBO, OnUpdateBookmarkCombo)
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CJazzApp construction

CJazzApp::CJazzApp()
{
	m_pActiveProject = NULL;
	m_pFramework = NULL;
	m_pIServiceProvider = NULL;
	m_pIPageManager = NULL;
	m_pJzTrackFileOpen = NULL;
	m_pLoader = NULL;
	m_nFirstImage = 0;
	m_nRuntimeSave = DOC_NOT_RUNTIME_SAVE;
	m_nFileTarget = TGT_PROJECT;
	m_fShutDown = FALSE;
	m_fCloseProject = FALSE;
	m_nShowNodeInTree = SHOW_NODE_IN_TREE;
	m_fInPasteFromData = FALSE;
	m_fInDocRootDelete = FALSE;
	m_fInDuplicateProject = FALSE;
	m_fUserChangedNodeName = FALSE;
	m_fInJazzDocSaveModified = FALSE;
	m_fDeleteFromTree = FALSE;
	m_fShellCommandOK = FALSE;
	m_fSendFileNameChangeNotification = TRUE;
	m_fOpenEditorWindow = TRUE;
	m_nSavePromptAction = 0;
	m_hKeyProgressBar = NULL;
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CJazzApp object

CJazzApp theApp;

// This identifier was generated to be statistically unique for your app.
// You may change it if you prefer to choose a specific identifier.

// {B6AED002-3BDF-11D0-89AC-00A0C9054129}
static const CLSID clsid =
{ 0xb6aed002, 0x3bdf, 0x11d0, { 0x89, 0xac, 0x0, 0xa0, 0xc9, 0x5, 0x41, 0x29 } };


// Keep track of whether the class was registered so we can
// unregister it upon exit
static BOOL g_bClassRegistered = FALSE;

/////////////////////////////////////////////////////////////////////////////
// CJazzApp InitInstance

BOOL CJazzApp::InitInstance()
{
    // If a previous instance of the application is already running,
    // then activate it and return FALSE from InitInstance to end the
    // execution of this instance.
    if( !FirstInstance() )
	{
		return FALSE;
	}

	srand( (unsigned)time( NULL ) );

    // Register our unique class name that we wish to use
    WNDCLASS wndcls;

    memset(&wndcls, 0, sizeof(WNDCLASS));   // start with NULL defaults

	wndcls.style = CS_DBLCLKS;
    wndcls.lpfnWndProc = ::DefWindowProc;
    wndcls.hInstance = AfxGetInstanceHandle();
    wndcls.hIcon = LoadIcon(IDR_MAINFRAME); // or load a different icon
    wndcls.hCursor = ::LoadCursor(NULL, IDC_ARROW);
    wndcls.hbrBackground = NULL;
    wndcls.lpszMenuName = NULL;

    // Specify our own class name for using FindWindow later
    wndcls.lpszClassName = lpszUniqueClass;

    // Register new class and exit if it fails
    if(!AfxRegisterClass(&wndcls))
    {
		TRACE("Class Registration Failed\n");
		return FALSE;
    }
    g_bClassRegistered = TRUE;

	if( __argc > 1 )
	{
		LPCTSTR pszParam = __targv[1];

		if( pszParam[0] == '-'
		||  pszParam[0] == '/' )
		{
			++pszParam;
			if( _tcsicmp( pszParam, _T("RegServer") ) == 0 )
			{
				return FALSE;
			}
		}
	}

	// Load accelerators
	m_hAcceleratorTable = ::LoadAccelerators( m_hInstance, MAKEINTRESOURCE(IDR_APP_ACCELS) );
	if( m_hAcceleratorTable == NULL )
	{
		return FALSE;
	}

	// CG: The following block was added by the Splash Screen component.
	{
		CCommandLineInfo cmdInfo;
		ParseCommandLine( cmdInfo );

		if( cmdInfo.m_nShellCommand == cmdInfo.AppUnregister )
		{
			CSplashWnd::EnableSplashScreen( FALSE );
		}
		else
		{
			CSplashWnd::EnableSplashScreen( cmdInfo.m_bShowSplash );
		}
	}

	// Initialize OLE libraries
	if( !AfxOleInit() )
	{
		AfxMessageBox( IDP_OLE_INIT_FAILED );
		return FALSE;
	}

	AfxEnableControlContainer();
	::CoInitialize( NULL );

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

    // Store DMUSProd.exe path
	TCHAR achExePath[FILENAME_MAX];
	TCHAR achDrive[_MAX_DRIVE];
	TCHAR achDir[_MAX_DIR];

	if( GetModuleFileName(m_hInstance, achExePath, FILENAME_MAX) == 0 )
	{
		return FALSE;
	}

	_tsplitpath( achExePath, achDrive, achDir, NULL, NULL );
	_tmakepath( achExePath, achDrive, achDir, NULL, NULL );
	m_strAppDir = achExePath;
	m_strAppDir.MakeLower();
	
	// Create/get "DMUSProducer" default Project directory
	CString strProjectsDir;
	
	GetDefaultProjectDir( strProjectsDir );
	::SetCurrentDirectory( strProjectsDir );

	// Change the registry key under which our settings are stored.
	// You should modify this string to be something appropriate
	// such as the name of your company or organization.
	
//AMC Commented this out so everything is going to c:\windows\Jazz.ini
	//SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	LoadStdProfileSettings();  // Load standard INI file options (including MRU)

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.
	CJazzDocTemplate* pDocTemplate;
	pDocTemplate = new CJazzDocTemplate(
						NULL,
						m_hInstance,
						IDR_JAZZTYPE,
						RUNTIME_CLASS(CJazzDoc),
						RUNTIME_CLASS(CChildFrame), // custom MDI child frame
						RUNTIME_CLASS(CJazzView));

	pDocTemplate->SetContainerInfo(IDR_JAZZTYPE_CNTR_IP );
	AddDocTemplate( pDocTemplate );

	// Connect the COleTemplateServer to the document template.
	//  The COleTemplateServer creates new documents on behalf
	//  of requesting OLE containers by using information
	//  specified in the document template.
	m_server.ConnectTemplate( clsid, pDocTemplate, FALSE );

	// Register all OLE server factories as running.  This enables the
	//  OLE libraries to create objects from other applications.
	COleTemplateServer::RegisterAll();
		// Note: MDI applications register all server objects without regard
		//  to the /Embedding or /Automation on the command line.

	// Create Framework object
	m_pFramework = new CFramework;
	if( !m_pFramework )
	{
		return FALSE;
	}

	// create main MDI Frame window
	CMainFrame* pMainFrame = new CMainFrame;
	if( (pMainFrame == NULL)
	|| !(pMainFrame->LoadFrame(IDR_MAINFRAME)) )
	{
		return FALSE;
	}
	m_pMainWnd = pMainFrame;

	// CG: The following line was added by the Splash Screen component.
	CSplashWnd::ShowSplashScreen( m_pMainWnd );

	// Add Project node images to image list
	AddNodeImageList();

	// Load DirectMusic Producer Components
	if( !m_pFramework->LoadComponents()
	||  !m_pFramework->InitComponents() 
	||  !m_pFramework->AddComponentDocTemplates()
	||  !m_pFramework->RegisterClipboardFormats() )
	{
		delete m_pFramework;
		m_pFramework = NULL;
		return FALSE;
	}
	
	// Enable drag/drop open
	m_pMainWnd->DragAcceptFiles();

	// Enable DDE Execute open
	EnableShellOpen();
	RegisterShellFileTypes( FALSE );
	UnregisterTheFileExtensions();
	RegisterTheFileExtensions();

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine( cmdInfo );

	// Check to see if launched as OLE server
	if( cmdInfo.m_bRunEmbedded
	||  cmdInfo.m_bRunAutomated )
	{
		// Application was run with /Embedding or /Automation.  Don't show the
		//  main window in this case.
		return TRUE;
	}

	// When a server application is launched stand-alone, it is a good idea
	//  to update the system registry in case it has been damaged.
	m_server.UpdateRegistry(OAT_DISPATCH_OBJECT);
	COleObjectFactory::UpdateRegistryAll();

	// We don't want to open a new file
	if( cmdInfo.m_nShellCommand == cmdInfo.FileNew )
	{
		cmdInfo.m_nShellCommand = cmdInfo.FileNothing;
	}
	else if( cmdInfo.m_nShellCommand == cmdInfo.AppUnregister)
	{
		cmdInfo.m_bRunEmbedded = TRUE;

		UnregisterTheFileExtensions();

		// Delete Jazz.ini file
		TCHAR achFileName[_MAX_PATH + 1];

		::GetWindowsDirectory( achFileName, _MAX_PATH );
		CString strFileName = achFileName;
		if( strFileName.Right(1) != _T("\\") )
		{
			strFileName += _T("\\");
		}
		strFileName += m_pszProfileName;
		SetFileAttributes( strFileName, FILE_ATTRIBUTE_ARCHIVE );
		DeleteFile( strFileName );
	}

	// Make sure main application window is active before killing splash
	m_pMainWnd->SetActiveWindow();

	// Close the Splash Screen
	CSplashWnd::KillSplashScreen();

	// Dispatch commands specified on the command line
	if( !ProcessShellCommand(cmdInfo) )
	{
		if( m_fShellCommandOK == FALSE )
		{
			return FALSE;
		}
	}

    /* Final release now..
// Xbox team doesn't want a timebomb in their beta
#ifndef DMP_XBOX
	// TIMEBOMB - Remove for final!!!
	SYSTEMTIME  st;
	GetSystemTime( &st );
	if( (st.wYear == 2001) && (st.wMonth >= 12) )
	{
		AfxMessageBox( IDS_TIME_TO_UPDATE );
	}
	else if( st.wYear >= 2002 )
	{
		AfxMessageBox( IDS_PRODUCER_EXPIRED );
		return FALSE;
	}
#endif
    */

	// Position window and toolbars and display window.
	LoadAppState( m_nCmdShow );

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// Function: BOOL CJazzApp::FirstInstance()
//
// Purpose: Check for a previous instance of this application by 
//          searching for a window with our specific pre-registered
//          class name. If one is found, then activate it and return
//          FALSE
//
//
// Parameters: none
//
// Returns: TRUE indicating that a previous instance was NOT found, or
//          FALSE if one was found and activated.
//
// Comments:
//
/////////////////////////////////////////////////////////////////////////////
BOOL CJazzApp::FirstInstance()
{
	CWnd *pWndPrev, *pWndChild;
  
	// Determine if another window with our class name exists...
	if( pWndPrev = CWnd::FindWindow( lpszUniqueClass, NULL ) )
	{
		pWndChild = pWndPrev->GetLastActivePopup(); // if so, does it have any popups?

		if( pWndPrev->IsIconic() ) 
		{
			pWndPrev->ShowWindow( SW_RESTORE );     // If iconic, restore the main window
		}

		pWndChild->SetForegroundWindow();			// Bring the main window or it's popup to
													// the foreground
		// and we are done activating the previous one.
		return FALSE;                             
	}

	return TRUE;									// First instance. Proceed as normal.
}


/////////////////////////////////////////////////////////////////////////////
// SetRegKey

static BOOL SetRegKey( LPCTSTR lpszKey, LPCTSTR lpszValue, LPCTSTR lpszValueName = NULL )
{
	if( lpszValueName == NULL )
	{
		if( ::RegSetValue(HKEY_CLASSES_ROOT, lpszKey, REG_SZ,
						  lpszValue, lstrlen(lpszValue) * sizeof(TCHAR)) != ERROR_SUCCESS )
		{
			TRACE1( "Warning: registration database update failed for key '%s'.\n", lpszKey );
			return FALSE;
		}
		return TRUE;
	}
	else
	{
		HKEY hKey;

		if( ::RegCreateKey(HKEY_CLASSES_ROOT, lpszKey, &hKey) == ERROR_SUCCESS )
		{
			LONG lResult = ::RegSetValueEx(hKey, lpszValueName, 0, REG_SZ,
				(CONST BYTE*)lpszValue, (lstrlen(lpszValue) + 1) * sizeof(TCHAR));

			if( ::RegCloseKey(hKey) == ERROR_SUCCESS && lResult == ERROR_SUCCESS )
			{
				return TRUE;
			}
		}
		TRACE1("Warning: registration database update failed for key '%s'.\n", lpszKey);
		return FALSE;
	}
}


/////////////////////////////////////////////////////////////////////////////
// RecursiveRegDeleteKey

static LONG AFXAPI RecursiveRegDeleteKey( HKEY hParentKey, LPCTSTR szKeyName )
{
	DWORD dwIndex = 0L;
	TCHAR szSubKeyName[MAX_BUFFER];
	HKEY hCurrentKey;
	DWORD dwResult;

	dwResult = ::RegOpenKey( hParentKey, szKeyName, &hCurrentKey );
	if( dwResult == ERROR_SUCCESS )
	{
		// Remove all subkeys of the key to delete
		dwResult = ::RegEnumKey( hCurrentKey, 0, szSubKeyName, MAX_BUFFER-1 );
		while( dwResult == ERROR_SUCCESS )
		{
			dwResult = RecursiveRegDeleteKey( hCurrentKey, szSubKeyName );
			if( dwResult!= ERROR_SUCCESS )
			{
				break;
			}
		}

		// If all went well, we should now be able to delete the requested key
		if( (dwResult == ERROR_NO_MORE_ITEMS)
		||  (dwResult == ERROR_BADKEY) 
		||  (dwResult == ERROR_FILE_NOT_FOUND) )	// Key not found
		{
			dwResult = ::RegDeleteKey( hParentKey, szKeyName );
		}

		::RegCloseKey( hCurrentKey );
	}

	return dwResult;
}


/////////////////////////////////////////////////////////////////////////////
// CJazzApp RegisterTheFileExtensions

void CJazzApp::RegisterTheFileExtensions( void )
{
	CJazzDocTemplate* pTemplate;
	IDMUSProdDocType* pIDocType;
	CString strFilterExt;
	CString strFileTypeId;
	CString strFileTypeName;
	CString strAdjustedTypeId;
	CString strAdjustedTypeName;
	CString strOpenCommandLine;
	CString strTemp;
	CString strTemp2;
	CString strDesign;
	CString strRuntime;
	bool fRuntimeExt;

	// no doc manager - no templates
	if( m_pDocManager == NULL )
	{
		return;
	}

	strDesign.LoadString( IDS_REG_DESIGN );
	strRuntime.LoadString( IDS_REG_RUNTIME );

	TCHAR achLongPath[MAX_BUFFER];	// Temporary work field
	TCHAR achPath[MAX_BUFFER];
	TCHAR achModuleName[MAX_BUFFER];

	GetModuleFileName( m_hInstance, achLongPath, MAX_BUFFER ); 
	GetShortPathName( achLongPath, achPath, MAX_BUFFER);

	strOpenCommandLine = achPath;
	strOpenCommandLine += szOpenArg;
	
	// walk all templates in the application
	POSITION pos = m_pDocManager->GetFirstDocTemplatePosition();
	while( pos != NULL )
	{
		pTemplate = (CJazzDocTemplate *)m_pDocManager->GetNextDocTemplate( pos );
		ASSERT_VALID( pTemplate );
		ASSERT_KINDOF( CJazzDocTemplate, pTemplate );

		if( pTemplate->GetDocString(strFilterExt, CDocTemplate::filterExt)
		&&  !strFilterExt.IsEmpty()
		&&  pTemplate->GetDocString(strFileTypeId, CDocTemplate::regFileTypeId)
		&&  !strFileTypeId.IsEmpty() )
		{
			// Set achModuleName
			GetModuleFileName( pTemplate->GetInstance(), achLongPath, MAX_BUFFER ); 
			GetShortPathName( achLongPath, achModuleName, MAX_BUFFER);

			if( !pTemplate->GetDocString( strFileTypeName, CDocTemplate::regFileTypeName) )
			{
				strFileTypeName = strFileTypeId;    // use id name
			}

			ASSERT(strFileTypeId.Find(' ') == -1);  // no spaces allowed
			ASSERT( strFilterExt[0] == '.' );

			// Remove bad entry;	i.e. HKEY_CLASSES_ROOT\.sty;*stj
			RecursiveRegDeleteKey( HKEY_CLASSES_ROOT, strFilterExt );

			// Remove bad entry;	i.e. HKEY_CLASSES_ROOT\DirectMusic.Style
			RecursiveRegDeleteKey( HKEY_CLASSES_ROOT, strFileTypeId );

			LONG lSize = _MAX_PATH * 2;
			BOOL fContinue = TRUE;
			CString strExt;
			BSTR bstrExt;
			int nFindPos;

			// Process each extension
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

				// Adjust the strFileTypeId and strFileTypeName strings
				strAdjustedTypeId = strFileTypeId;
				strAdjustedTypeName = strFileTypeName;

				fRuntimeExt = false;

				bstrExt = strExt.AllocSysString();
				if( SUCCEEDED ( m_pFramework->FindDocTypeByExtension( bstrExt, &pIDocType ) ) )
				{
					bstrExt = strExt.AllocSysString();
					if( pIDocType->IsFileTypeExtension( FT_RUNTIME, bstrExt ) == S_OK )
					{
						fRuntimeExt = true;
						strAdjustedTypeId += _T(".");
						strAdjustedTypeId += strRuntime;
						strAdjustedTypeName += _T(" (");
						strAdjustedTypeName += strRuntime;
						strAdjustedTypeName += _T(")");
					}
					else
					{
						strAdjustedTypeId += _T(".");
						strAdjustedTypeId += strDesign;
						strAdjustedTypeName += _T(" (");
						strAdjustedTypeName += strDesign;
						strAdjustedTypeName += _T(")");
					}

					pIDocType->Release();
				}

				// Do not register runtime file extensions
				// Remove this check if we decide to once again register runtime extensions
				if( fRuntimeExt == false )
				{
					// Make sure registry is correct for this extension
					LONG lResult = ::RegQueryValue( HKEY_CLASSES_ROOT,
													strExt,
													strTemp.GetBuffer(lSize),
													&lSize );
					strTemp.ReleaseBuffer();

					if( lResult != ERROR_SUCCESS
					||  strTemp.IsEmpty()
					||  strTemp == strAdjustedTypeId )
					{
						// Create registry entries
						SetRegKey( strExt, strAdjustedTypeId );
						SetRegKey( strAdjustedTypeId, strAdjustedTypeName );

						// path\DefaultIcon\modulename,0
						strTemp.Format( szDefaultIconFmt, (LPCTSTR)strAdjustedTypeId );
						strTemp2.Format( szDefaultIcon, (LPCTSTR)achModuleName );
						SetRegKey( strTemp, strTemp2 );

						// path\shell\open\ddeexec = [open("%1")]
						strTemp.Format( szShellOpenFmt, (LPCTSTR)strAdjustedTypeId, (LPCTSTR)szDDEExec );
						SetRegKey( strTemp, szDDEOpen );

						// path\shell\open\command = path filename
						strTemp.Format( szShellOpenFmt, (LPCTSTR)strAdjustedTypeId, (LPCTSTR)szCommand);
						SetRegKey( strTemp, strOpenCommandLine );
					}
				}

				nFindPos = strFilterExt.Find( _T("*") );
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CJazzApp UnregisterTheFileExtensions

void CJazzApp::UnregisterTheFileExtensions( void )
{
	CJazzDocTemplate* pTemplate;
	IDMUSProdDocType* pIDocType;
	CString strFilterExt;
	CString strFileTypeId;
	CString strFileTypeName;
	CString strAdjustedTypeId;
	CString strAdjustedTypeName;
	CString strTemp;
	CString strDesign;
	CString strRuntime;

	// no doc manager - no templates
	if( m_pDocManager == NULL )
	{
		return;
	}

	strDesign.LoadString( IDS_REG_DESIGN );
	strRuntime.LoadString( IDS_REG_RUNTIME );
	
	// walk all templates in the application
	POSITION pos = m_pDocManager->GetFirstDocTemplatePosition();
	while( pos != NULL )
	{
		pTemplate = (CJazzDocTemplate *)m_pDocManager->GetNextDocTemplate( pos );
		ASSERT_VALID( pTemplate );
		ASSERT_KINDOF( CJazzDocTemplate, pTemplate );

		if( pTemplate->GetDocString(strFilterExt, CDocTemplate::filterExt)
		&&  !strFilterExt.IsEmpty()
		&&  pTemplate->GetDocString(strFileTypeId, CDocTemplate::regFileTypeId)
		&&  !strFileTypeId.IsEmpty() )
		{
			if( !pTemplate->GetDocString( strFileTypeName, CDocTemplate::regFileTypeName) )
			{
				strFileTypeName = strFileTypeId;    // use id name
			}

			ASSERT(strFileTypeId.Find(' ') == -1);  // no spaces allowed
			ASSERT( strFilterExt[0] == '.' );

			// Remove bad entry;	i.e. HKEY_CLASSES_ROOT\.sty;*stj
			RecursiveRegDeleteKey( HKEY_CLASSES_ROOT, strFilterExt );

			// Remove bad entry;	i.e. HKEY_CLASSES_ROOT\DirectMusic.Style
			RecursiveRegDeleteKey( HKEY_CLASSES_ROOT, strFileTypeId );

			LONG lSize = _MAX_PATH * 2;
			BOOL fContinue = TRUE;
			CString strExt;
			BSTR bstrExt;
			int nFindPos;

			// Process each extension
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

				// Adjust the strFileTypeId and strFileTypeName strings
				strAdjustedTypeId = strFileTypeId;
				strAdjustedTypeName = strFileTypeName;

				bstrExt = strExt.AllocSysString();
				if( SUCCEEDED ( m_pFramework->FindDocTypeByExtension( bstrExt, &pIDocType ) ) )
				{
					bstrExt = strExt.AllocSysString();
					if( pIDocType->IsFileTypeExtension( FT_RUNTIME, bstrExt ) == S_OK )
					{
						strAdjustedTypeId += _T(".");
						strAdjustedTypeId += strRuntime;
						strAdjustedTypeName += _T(" (");
						strAdjustedTypeName += strRuntime;
						strAdjustedTypeName += _T(")");
					}
					else
					{
						strAdjustedTypeId += _T(".");
						strAdjustedTypeId += strDesign;
						strAdjustedTypeName += _T(" (");
						strAdjustedTypeName += strDesign;
						strAdjustedTypeName += _T(")");
					}

					pIDocType->Release();
				}

				// Delete entry for this extension
				LONG lResult = ::RegQueryValue( HKEY_CLASSES_ROOT,
												strExt,
												strTemp.GetBuffer(lSize),
												&lSize );
				strTemp.ReleaseBuffer();

				if( lResult != ERROR_SUCCESS
				||  strTemp.IsEmpty()
				||  strTemp == strAdjustedTypeId)
				{
					// Remove registry entries
					strTemp.Format( szShellNewFmt, (LPCTSTR)strExt );
					RecursiveRegDeleteKey( HKEY_CLASSES_ROOT, strTemp );

					// Remove entry;	i.e. HKEY_CLASSES_ROOT\.sty;*stj
					RecursiveRegDeleteKey( HKEY_CLASSES_ROOT, strExt );

					// Remove entry;	i.e. HKEY_CLASSES_ROOT\DirectMusic.Style.Design
					RecursiveRegDeleteKey( HKEY_CLASSES_ROOT, strAdjustedTypeId );
				}

				nFindPos = strFilterExt.Find( _T("*") );
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CJazzApp::LoadAppState

void CJazzApp::LoadAppState( int nCmdShow )
{
	CMainFrame* pMainFrame = (CMainFrame *)m_pMainWnd;
	ASSERT( pMainFrame != NULL );

    CString strSection;
    CString strEntry;
	int nCount;

	// Load the "General" section
	strSection = _T("General");

	//		Window Placement
	WINDOWPLACEMENT wp;

	if( GetPrivateProfileStruct( strSection, _T("AppWP"),
								 &wp, sizeof(wp), m_pszProfileName ) )
	{
		wp.length = sizeof(wp);
		if( wp.showCmd != SW_HIDE
		&&  wp.showCmd != SW_SHOWMINIMIZED )
		{
			nCmdShow = wp.showCmd;
		}
		wp.showCmd = SW_HIDE;
		pMainFrame->SetWindowPlacement( &wp );
	}
	else
	{
		nCmdShow = SW_SHOWMAXIMIZED;
	}

	// Load "ProjectTree" section
	{
		CSize size;
	
		strSection = _T("ProjectTree");
	
		size.cx = (int)GetProfileInt( strSection, _T("DockWidth"), pMainFrame->m_wndTreeBar.m_sizeDefault.cx );
		size.cy = (int)GetProfileInt( strSection, _T("DockHeight"), pMainFrame->m_wndTreeBar.m_sizeDefault.cy );
		if( size.cx != 0
		&&  size.cy != 0 )
		{
			pMainFrame->m_wndTreeBar.m_sizeDocked.cx = size.cx;
			pMainFrame->m_wndTreeBar.m_sizeDocked.cy = size.cy;
		}

		size.cx = (int)GetProfileInt( strSection, _T("FloatWidth"), pMainFrame->m_wndTreeBar.m_sizeDefault.cx );
		size.cy = (int)GetProfileInt( strSection, _T("FloatHeight"), pMainFrame->m_wndTreeBar.m_sizeDefault.cy );
		if( size.cx != 0
		&&  size.cy != 0 )
		{
			pMainFrame->m_wndTreeBar.m_sizeFloating.cx = size.cx;
			pMainFrame->m_wndTreeBar.m_sizeFloating.cy = size.cy;
		}
	}

	// Load toolbar state information
	{
		CDockState state;
		state.LoadState( _T("BarSettings") );

		int i = 0;

		while( i < state.m_arrBarInfo.GetSize() )
		{
			CControlBarInfo* pInfo = (CControlBarInfo *)state.m_arrBarInfo[i];
			ASSERT( pInfo != NULL );

			CControlBar* pBar = pMainFrame->GetControlBar( pInfo->m_nBarID );
			if( pBar == NULL )
			{
				int j = i;
				int k = i + 1;

				while( k < state.m_arrBarInfo.GetSize() )
				{
					state.m_arrBarInfo[j++] = state.m_arrBarInfo[k++];
				}

				int nNewSize = state.m_arrBarInfo.GetSize() - 1;
				state.m_arrBarInfo.SetSize( nNewSize );
			}
			else
			{
				i++;
			}
		}

		pMainFrame->SetDockState( state );
	}

	// Force redraw of application window
	pMainFrame->ShowWindow( nCmdShow );
	pMainFrame->UpdateWindow();

	// Load the "Projects" section
	strSection = _T("Projects");

	//		Project
	CString strFileName;
	nCount = 0;

    for( ; ; )
    {
		strEntry.Format( "%s%d", _T("Project"), ++nCount );
		strFileName = GetProfileString( strSection, strEntry, _T("") );

		if( strFileName.IsEmpty() )
		{
			break;
		}

		// Project files do not use targets (TGT_XXX)
		OpenDocumentFile( strFileName );
    }

	// Force redraw of property sheet before we start opening content files
	pMainFrame->m_wndProperties.UpdateWindow();

	// Force redraw of Project Tree before we start opening content files
	pMainFrame->m_wndTreeBar.UpdateWindow();

	// Set default file filter to 'All Files (*.*)'
	WritePrivateProfileString( _T("Directories"), _T("OpenExt"), _T("*"), m_pszProfileName );
}


/////////////////////////////////////////////////////////////////////////////
// CJazzApp::SaveAppState

void CJazzApp::SaveAppState( void )
{
	CMainFrame* pMainFrame = (CMainFrame *)m_pMainWnd;
	ASSERT( pMainFrame != NULL );

    CString strSection;
    CString strEntry;
	POSITION pos;
	int nCount;

	// Delete the "General" section
	strSection = _T("General");
	WriteProfileString( strSection, NULL, NULL );

	// Rebuild the "General" section

	//		Window Placement
	WINDOWPLACEMENT wp;

	wp.length = sizeof(wp);
	if( pMainFrame->GetWindowPlacement( &wp ) )
	{
		WritePrivateProfileStruct( strSection, _T("AppWP"),
								   &wp, sizeof(wp), m_pszProfileName );
	}

	// Get file extension used for Projects
	CString strFilterExt;
	CJazzDocTemplate* pDocTemplate = FindProjectDocTemplate();
	ASSERT( pDocTemplate != NULL );
	pDocTemplate->GetDocString( strFilterExt, CDocTemplate::filterExt );

	// Delete the "Projects" section
	strSection = _T("Projects");
	WriteProfileString( strSection, NULL, NULL );

	// Rebuild the "Projects" section

	//		Project
	CProject* pProject;
	CString strFileName;
	nCount = 0;

    pos = m_lstProjects.GetHeadPosition();
    while( pos )
    {
        pProject = m_lstProjects.GetNext( pos );

		strFileName = pProject->m_strProjectDir + pProject->m_strName + strFilterExt;

		strEntry.Format( "%s%d", _T("Project"), ++nCount );
		WriteProfileString( strSection, strEntry, strFileName );
    }

	// Save toolbar state information
	pMainFrame->SaveBarState( _T("BarSettings") );

	// Delete the "ProjectTree" section
	strSection = _T("ProjectTree");
	WriteProfileString( strSection, NULL, NULL );

	// Rebuild the "ProjectTree" section

	WriteProfileInt( strSection, _T("DockWidth"), pMainFrame->m_wndTreeBar.m_sizeDocked.cx );
	WriteProfileInt( strSection, _T("DockHeight"), pMainFrame->m_wndTreeBar.m_sizeDocked.cy );
	WriteProfileInt( strSection, _T("FloatWidth"), pMainFrame->m_wndTreeBar.m_sizeFloating.cx );
	WriteProfileInt( strSection, _T("FloatHeight"), pMainFrame->m_wndTreeBar.m_sizeFloating.cy );
}


/////////////////////////////////////////////////////////////////////////////
// CJazzApp CreateDocTemplate

BOOL CJazzApp::CreateDocTemplate( IDMUSProdDocType* pIDocType, HINSTANCE hInstance, UINT nResourceId )
{
	HINSTANCE hInstanceOld;
	CJazzDocTemplate* pDocTemplate;

	ASSERT( pIDocType != NULL );
	ASSERT( hInstance != NULL );

	hInstanceOld = AfxGetResourceHandle();
	AfxSetResourceHandle( hInstance );

	pDocTemplate = new CJazzDocTemplate(
						pIDocType,
						hInstance,
						nResourceId,
						RUNTIME_CLASS(CComponentDoc),
						RUNTIME_CLASS(CChildFrame), // custom MDI child frame
						RUNTIME_CLASS(CComponentView));

	AfxSetResourceHandle( hInstanceOld );

	pDocTemplate->SetContainerInfo(IDR_JAZZTYPE_CNTR_IP );
	AddDocTemplate( pDocTemplate );
//	m_server.ConnectTemplate( clsid, pDocTemplate, FALSE );

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CJazzApp::GetPropertySheet

IDMUSProdPropSheet* CJazzApp::GetPropertySheet( void )
{
	IDMUSProdPropSheet* pIPropSheet = NULL;

	m_pFramework->QueryInterface( IID_IDMUSProdPropSheet, (void**)&pIPropSheet );

	return pIPropSheet;
}


/////////////////////////////////////////////////////////////////////////////
// CJazzApp::GetProjectTreeCtrl

CTreeCtrl* CJazzApp::GetProjectTreeCtrl( void )
{
	CTreeCtrl*  pTreeCtrl = NULL;

	CMainFrame* pMainFrame = (CMainFrame *)m_pMainWnd->GetTopLevelFrame();
	if( pMainFrame )
	{
		pTreeCtrl = pMainFrame->GetProjectTreeCtrl();
	}
	
	return pTreeCtrl;
}


/////////////////////////////////////////////////////////////////////////////
// CJazzApp::DrawProjectTree

void CJazzApp::DrawProjectTree( void )
{
	CTreeCtrl* pTreeCtrl;

	pTreeCtrl = GetProjectTreeCtrl();

	if( pTreeCtrl )
	{
		pTreeCtrl->Invalidate();
		pTreeCtrl->UpdateWindow();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CJazzApp::SetProjectTreePosition

void CJazzApp::SetProjectTreePosition( IDMUSProdNode* pINode )
{
	HTREEITEM hItem = m_pFramework->FindTreeItem( pINode );

	if( hItem )
	{
		CTreeCtrl* pTreeCtrl = GetProjectTreeCtrl();
		if( pTreeCtrl )
		{
			pTreeCtrl->EnsureVisible( hItem );
			m_nShowNodeInTree++;
			pTreeCtrl->SelectItem( hItem );
			m_nShowNodeInTree--;
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CJazzApp::GetNbrDocuments

int CJazzApp::GetNbrDocuments( void )
{
	CDocTemplate* pTemplate;
	int nNbrDocuments = 0;

	// no doc manager - no templates
	if( m_pDocManager == NULL )
	{
		return 0;
	}
	
	// walk all templates in the application
	POSITION pos = m_pDocManager->GetFirstDocTemplatePosition();
	while( pos != NULL )
	{
		pTemplate = m_pDocManager->GetNextDocTemplate( pos );
		ASSERT_VALID( pTemplate );
		ASSERT_KINDOF( CDocTemplate, pTemplate );

		// walk all documents in the template
		POSITION pos2 = pTemplate->GetFirstDocPosition();
		while( pos2 )
		{
			CCommonDoc* pDoc = (CCommonDoc *)pTemplate->GetNextDoc( pos2 );
			ASSERT_VALID( pDoc );
			nNbrDocuments++;
		}
	}

	return nNbrDocuments;
}


/////////////////////////////////////////////////////////////////////////////
// CJazzApp::FindDocument

CCommonDoc* CJazzApp::FindDocument( IDMUSProdNode* pINode )
{
	CCommonDoc* pCommonDoc = NULL;
	CDocTemplate* pTemplate;
	IDMUSProdNode* pIDocRootNode;

	ASSERT( pINode != NULL );

	// no doc manager - no templates
	if( m_pDocManager == NULL )
	{
		return NULL;
	}

	if( FAILED ( pINode->GetDocRootNode( &pIDocRootNode ) ) )
	{
		return NULL;
	}

	if( pIDocRootNode == NULL )
	{
		return NULL;
	}

	// walk all templates in the application
	POSITION pos = m_pDocManager->GetFirstDocTemplatePosition();
	while( pos != NULL )
	{
		pTemplate = m_pDocManager->GetNextDocTemplate( pos );
		ASSERT_VALID( pTemplate );
		ASSERT_KINDOF( CDocTemplate, pTemplate );

		// walk all documents in the template
		POSITION pos2 = pTemplate->GetFirstDocPosition();
		while( pos2 )
		{
			CCommonDoc* pDoc = (CCommonDoc *)pTemplate->GetNextDoc( pos2 );
			ASSERT_VALID( pDoc );

			// is this the document we want?
			if( pDoc->m_pIDocRootNode == pIDocRootNode )
			{
				pCommonDoc = pDoc;
				break;
			}
		}

		if( pCommonDoc )
		{
			break;
		}
	}

	pIDocRootNode->Release();

	return pCommonDoc;
}


/////////////////////////////////////////////////////////////////////////////
// CJazzApp::FindComponentDocument

CComponentDoc* CJazzApp::FindComponentDocument( IDMUSProdNode* pINode )
{
	CComponentDoc* pComponentDoc = NULL;
	IDMUSProdNode* pIDocRootNode;

	ASSERT( pINode != NULL );

	if( SUCCEEDED ( pINode->GetDocRootNode( &pIDocRootNode ) ) )
	{
		if( pIDocRootNode )
		{
			CFileNode* pFileNode = GetFileByDocRootNode( pIDocRootNode );
			if( pFileNode )
			{
				pComponentDoc = pFileNode->m_pComponentDoc;

				pFileNode->Release();
			}

			pIDocRootNode->Release();
		}
	}

	return pComponentDoc;
}


/////////////////////////////////////////////////////////////////////////////
// CJazzApp::IsValidProjectFile

BOOL CJazzApp::IsValidProjectFile( LPCTSTR szPathName, BOOL fDisplayErrMsg ) 
{
	TCHAR achFileName[_MAX_FNAME+_MAX_EXT+1];
	TCHAR achDir[_MAX_DIR];
	TCHAR achName[_MAX_FNAME];
	TCHAR achExt[_MAX_EXT];

	_tsplitpath( szPathName, NULL, achDir, achName, achExt );
	_tmakepath( achFileName, NULL, NULL, achName, achExt );

	// Does file extension match Project file extension?
	CJazzDocTemplate* pTemplate = FindProjectDocTemplate();
	if( pTemplate )
	{
		CString strFilterExt;

		if( pTemplate->GetDocString(strFilterExt, CDocTemplate::filterExt)
		&&  !strFilterExt.IsEmpty() )
		{
			if( _tcsicmp( strFilterExt, achExt ) != 0 )
			{
				if( fDisplayErrMsg )
				{
					CString strMsg;
					AfxFormatString1( strMsg, IDS_ERR_NOT_PRODUCER_PROJECT, achFileName );
					AfxMessageBox( strMsg );
				}
				return FALSE;
			}
		}
	}

	// Does file reside in directory of same name?
	CString strFolder = achDir;

	RemoveLastSlash( strFolder );
	int nFindPos = strFolder.ReverseFind( (TCHAR)'\\' );
	if( nFindPos != -1 )
	{
		strFolder = strFolder.Right( strFolder.GetLength() - nFindPos - 1 );
	}

	if( _tcsicmp( strFolder, achName ) != 0 )
	{
		if( fDisplayErrMsg )
		{
			CString strMsg;
			AfxFormatString2( strMsg, IDS_ERR_NOT_PRODUCER_PROJECT2, achFileName, achName );
			AfxMessageBox( strMsg );
		}
		return FALSE;
	}

	// Does file contain the proper RIFF chunks?
	IStream* pIStream;
    IDMUSProdRIFFStream* pIRiffStream;
    MMCKINFO ckMain;
	MMCKINFO ck;

	BOOL fSuccess = FALSE;

	CString strPathName = szPathName;
	BSTR bstrPathName = strPathName.AllocSysString();

	if( SUCCEEDED ( m_pFramework->AllocFileStream(bstrPathName, GENERIC_READ, FT_UNKNOWN,
														 GUID_AllZeros, NULL, &pIStream) ) )
	{
		if( SUCCEEDED( AllocRIFFStream( pIStream, &pIRiffStream ) ) )
		{
			ckMain.fccType = FOURCC_PROJECT_FORM;

			if( pIRiffStream->Descend( &ckMain, NULL, MMIO_FINDRIFF ) == 0 )
			{
				ck.fccType = FOURCC_PROJECT_LIST;

				if( pIRiffStream->Descend( &ck, &ckMain, MMIO_FINDLIST ) == 0 )
				{
					fSuccess = TRUE;
				}
			}

			pIRiffStream->Release();
		}

		pIStream->Release();
	}

	if( fSuccess == FALSE )
	{
		if( fDisplayErrMsg )
		{
			CString strMsg;
			AfxFormatString1( strMsg, IDS_ERR_NOT_PRODUCER_PROJECT, achFileName );
			AfxMessageBox( strMsg );
		}
	}

	return fSuccess;
}


/////////////////////////////////////////////////////////////////////////////
// CJazzApp::FindDocType

IDMUSProdDocType* CJazzApp::GetDocType( LPCTSTR szFileName )
{
	IDMUSProdDocType* pIDocType;
	TCHAR achExt[_MAX_EXT];

	_tsplitpath( szFileName, NULL, NULL, NULL, achExt );
	CString strExt = achExt;
	BSTR bstrExt = strExt.AllocSysString();

	if( FAILED ( m_pFramework->FindDocTypeByExtension( bstrExt, &pIDocType ) ) )
	{
		pIDocType = NULL;
	}

	return pIDocType;
}


/////////////////////////////////////////////////////////////////////////////
// CJazzApp::FindDocTemplateByFilter

CJazzDocTemplate* CJazzApp::FindDocTemplateByFilter( LPCTSTR szFilterExt )
{
	CJazzDocTemplate* pDocTemplate = NULL;
	CJazzDocTemplate* pTemplate;
	CString strFilterExt;

	// no szFilterExt or no doc manager - no template
	if( szFilterExt == NULL
	||  m_pDocManager == NULL )
	{
		return NULL;
	}

	// walk all templates in the application
	POSITION pos = m_pDocManager->GetFirstDocTemplatePosition();
	while( pos != NULL )
	{
		pTemplate = (CJazzDocTemplate *)m_pDocManager->GetNextDocTemplate( pos );
		ASSERT_VALID( pTemplate );
		ASSERT_KINDOF( CJazzDocTemplate, pTemplate );

		if( pTemplate->GetDocString( strFilterExt, CDocTemplate::filterExt )
		&& !strFilterExt.IsEmpty() )
		{
			ASSERT( strFilterExt[0] == '.' );

			if( strFilterExt.CompareNoCase( szFilterExt ) == 0 )
			{
				pDocTemplate = pTemplate;
				break;
			}
		}
	}

	return pDocTemplate;
}


/////////////////////////////////////////////////////////////////////////////
// CJazzApp::FindDocTemplate

CJazzDocTemplate* CJazzApp::FindDocTemplate( IDMUSProdDocType* pIDocType )
{
	CJazzDocTemplate* pDocTemplate = NULL;
	CJazzDocTemplate* pTemplate;

	// no pIDocType or no doc manager - no templates
	if( pIDocType == NULL
	||  m_pDocManager == NULL )
	{
		return NULL;
	}

	// walk all templates in the application
	POSITION pos = m_pDocManager->GetFirstDocTemplatePosition();
	while( pos != NULL )
	{
		pTemplate = (CJazzDocTemplate *)m_pDocManager->GetNextDocTemplate( pos );
		ASSERT_VALID( pTemplate );
		ASSERT_KINDOF( CJazzDocTemplate, pTemplate );

		if( pTemplate->IsEqualDocType(pIDocType) )
		{
			pDocTemplate = pTemplate;
			break;
		}
	}

	return pDocTemplate;
}


/////////////////////////////////////////////////////////////////////////////
// CJazzApp::FindProjectDocTemplate

CJazzDocTemplate* CJazzApp::FindProjectDocTemplate( void )
{
	CJazzDocTemplate* pDocTemplate = NULL;
	CJazzDocTemplate* pTemplate;

	// no doc manager - no templates
	if( m_pDocManager == NULL )
	{
		return NULL;
	}

	// walk all templates in the application
	POSITION pos = m_pDocManager->GetFirstDocTemplatePosition();
	while( pos != NULL )
	{
		pTemplate = (CJazzDocTemplate *)m_pDocManager->GetNextDocTemplate( pos );
		ASSERT_VALID( pTemplate );
		ASSERT_KINDOF( CJazzDocTemplate, pTemplate );

		if( pTemplate->IsProjectDocType() )
		{
			pDocTemplate = pTemplate;
			break;
		}
	}

	return pDocTemplate;
}


/////////////////////////////////////////////////////////////////////////////
// CJazzApp FindFileNode

CFileNode* CJazzApp::FindFileNode( LPCTSTR szPathName )
{
	CProject* pProject;
	CFileNode* pFileNode;

    POSITION pos = m_lstProjects.GetHeadPosition();

    while( pos )
    {
        pProject = m_lstProjects.GetNext( pos );

		pFileNode = pProject->FindFileNode( szPathName );
		if( pFileNode )
		{
			return pFileNode;
		}
    }

	return NULL;
}


/////////////////////////////////////////////////////////////////////////////
// CJazzApp::FindMenuByName

CMenu* CJazzApp::FindMenuByName( UINT nResourceId )
{
	CMenu* pMenu = NULL;

	if( m_pMainWnd )
	{
		CMenu* pMainMenu = m_pMainWnd->GetMenu();

		if( pMainMenu )
		{
			CString strMenuText;
			TCHAR achMenuText[MID_BUFFER];
			UINT nNbrMenuItems;
			UINT i;

			::LoadString( m_hInstance, nResourceId, achMenuText, MID_BUFFER );
			strMenuText = achMenuText;
			nNbrMenuItems = pMainMenu->GetMenuItemCount();

			for( i = 0 ;  i < nNbrMenuItems ;  i++ )
			{
				CString strText;

				pMainMenu->GetMenuString( i, strText, MF_BYPOSITION );
				if( strText == strMenuText )
				{
					pMenu = pMainMenu->GetSubMenu( i );
					break;
				}
			}
		}
	}
	
	return pMenu;
}


/////////////////////////////////////////////////////////////////////////////
// CJazzApp::FindProjectFileName

void CJazzApp::FindProjectFileName( LPCTSTR szFileName, CString& strProjectFileName )
{
	ASSERT( szFileName != NULL );

	strProjectFileName.Empty();

	CString strFileName = szFileName;

	WIN32_FIND_DATA	fd;
	TCHAR achFileMask[_MAX_PATH + 1];
	int nFindPos;

	do
	{
		if( strFileName.Right(1) != _T("\\") )
		{
			strFileName += _T("\\");
		}
		wsprintf( achFileMask, "%s*.pro", strFileName );
	
		HANDLE hFind = FindFirstFile( achFileMask, &fd );
		if( hFind == INVALID_HANDLE_VALUE )
		{
			nFindPos = strFileName.ReverseFind( (TCHAR)'\\' );
			if( nFindPos == -1 )
			{
				break;
			}
			strFileName = strFileName.Left( nFindPos );

			nFindPos = strFileName.ReverseFind( (TCHAR)'\\' );
			if( nFindPos == -1 )
			{
				break;
			}
			strFileName = strFileName.Left( nFindPos );
		}
		else
		{
			_tcscpy( strrchr(achFileMask, '\\')+1, fd.cFileName );
			strProjectFileName = achFileMask;
	
			FindClose( hFind );
		}
	}
	while( strProjectFileName.IsEmpty() );
}


/////////////////////////////////////////////////////////////////////////////
// CJazzApp::CreateNewProject

CProject* CJazzApp::CreateNewProject( void )
{
	CProject* pProject = NULL;

	// Create the new Project
	CJazzDocTemplate* pTemplate = FindProjectDocTemplate();
	if( pTemplate )
	{
		// Project files do not use targets (TGT_XXX)
		CJazzDoc* pProjectDoc = (CJazzDoc *)pTemplate->OpenDocumentFile( NULL );
		if( pProjectDoc )
		{
			pProject = pProjectDoc->m_pProject;
		}
	}

	return pProject;
}


/////////////////////////////////////////////////////////////////////////////
// CJazzApp::AddNodeImageList

void CJazzApp::AddNodeImageList( void )
{
	CImageList lstImages;
	HICON hIcon;

	lstImages.Create( 16, 16, ILC_COLOR16, 8, 0 );
	lstImages.SetBkColor( GetSysColor(COLOR_WINDOW) );

	hIcon = ::LoadIcon( m_hInstance, MAKEINTRESOURCE(IDI_PROJECT_CLOSED) );
	lstImages.Add( hIcon );
	::DestroyIcon( hIcon );
	hIcon = ::LoadIcon( m_hInstance, MAKEINTRESOURCE(IDI_PROJECT_OPEN) );
	lstImages.Add( hIcon );
	::DestroyIcon( hIcon );

	hIcon = ::LoadIcon( m_hInstance, MAKEINTRESOURCE(IDI_FOLDER) );
	lstImages.Add( hIcon );
	::DestroyIcon( hIcon );
	hIcon = ::LoadIcon( m_hInstance, MAKEINTRESOURCE(IDI_FOLDER_SEL) );
	lstImages.Add( hIcon );
	::DestroyIcon( hIcon );

	hIcon = ::LoadIcon( m_hInstance, MAKEINTRESOURCE(IDI_FILE) );
	lstImages.Add( hIcon );
	::DestroyIcon( hIcon );
	hIcon = ::LoadIcon( m_hInstance, MAKEINTRESOURCE(IDI_FILE_SEL) );
	lstImages.Add( hIcon );
	::DestroyIcon( hIcon );

	hIcon = ::LoadIcon( m_hInstance, MAKEINTRESOURCE(IDI_JAZZ_FILE) );
	lstImages.Add( hIcon );
	::DestroyIcon( hIcon );
	hIcon = ::LoadIcon( m_hInstance, MAKEINTRESOURCE(IDI_JAZZ_FILE_SEL) );
	lstImages.Add( hIcon );
	::DestroyIcon( hIcon );

	m_pFramework->AddNodeImageList( lstImages.Detach(), &m_nFirstImage );
}


/////////////////////////////////////////////////////////////////////////////
// CJazzApp::SetActiveProject

void CJazzApp::SetActiveProject( CProject* pActiveProject ) 
{
	if( m_pActiveProject == pActiveProject )
	{
		return;
	}

	if( m_pActiveProject )
	{
		m_pActiveProject->Release();
		m_pActiveProject = NULL;
	}

	if( m_pIServiceProvider )
	{
		m_pIServiceProvider->Release();
		m_pIServiceProvider = NULL;
	}

	m_pActiveProject = pActiveProject;

	if( m_pActiveProject )
	{
		m_pActiveProject->AddRef();

		CString strFileName = m_pActiveProject->m_strProjectDir + m_pActiveProject->m_strName;

		if( FAILED ( AllocBindHostService ( strFileName, &m_pIServiceProvider ) ) )
		{
			m_pIServiceProvider = NULL;
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CJazzApp::GetActiveProject

CProject* CJazzApp::GetActiveProject( void ) 
{
	if( m_pActiveProject )
	{
		m_pActiveProject->AddRef();
		return m_pActiveProject;
	}

	return NULL;
}


/////////////////////////////////////////////////////////////////////////////
// CJazzApp::AddProject

void CJazzApp::AddProject( CProject* pProject ) 
{
	ASSERT( pProject != NULL );

	pProject->AddRef();
	m_lstProjects.AddTail( pProject );
}


/////////////////////////////////////////////////////////////////////////////
// CJazzApp::RemoveProject

void CJazzApp::RemoveProject( CProject* pProject ) 
{
	ASSERT( pProject != NULL );

	POSITION pos = m_lstProjects.Find( pProject );

	if( pos )
	{
		m_lstProjects.RemoveAt( pos );
		pProject->Release();
	}

}


/////////////////////////////////////////////////////////////////////////////
// CJazzApp::DeleteProject

BOOL CJazzApp::DeleteProject( CProject* pTheProject, short nWhichFiles, BOOL fRuntimePrompt ) 
{
	CProject* pProject;
	CString str1;
	CString str2;

	ASSERT( pTheProject != NULL );

	// Get the Project directory
	CString strPath;
	pTheProject->ConstructPath( strPath );
	ASSERT( !strPath.IsEmpty() );

	// Warn user if nested Projects will also get deleted
	CString strNestedProjects;
	pTheProject->FindNestedProjects( strPath, strNestedProjects );
	if( strNestedProjects.IsEmpty() == FALSE )
	{
		CString strMsg;

		AfxFormatString2( strMsg, IDS_DELETE_NESTED_PROJECTS, pTheProject->m_strName, strNestedProjects );
		if( AfxMessageBox( strMsg, MB_OKCANCEL ) == IDCANCEL )
		{
			return FALSE;
		}

		// Close nested Projects to remove them from the Project Tree
		POSITION pos = m_lstProjects.GetHeadPosition();
		while( pos )
		{
			pProject = m_lstProjects.GetNext( pos );

			if( pProject != pTheProject )
			{
				str1 = pProject->m_strProjectDir;
				str1.MakeLower();

				str2 = pTheProject->m_strProjectDir;
				str2.MakeLower();

				if( str1.Find( str2 ) == 0 )
				{
					ASSERT( pProject->m_pProjectDoc != NULL );
					pProject->m_pProjectDoc->OnCloseDocument();
				}
			}
		}

		// Delete nested *.pro files from the hard drive
		pTheProject->ConstructPath( strPath );
		pTheProject->DeleteNestedProjectFiles( strPath );
	}

	// If this Project is also being displayed as a subfolder in another Project, 
	// that subfolder must also be removed from the Project Tree
	POSITION pos = m_lstProjects.GetHeadPosition();
	while( pos )
	{
		pProject = m_lstProjects.GetNext( pos );

		if( pProject != pTheProject )
		{
			str1 = pTheProject->m_strProjectDir;
			str1.MakeLower();

			str2 = pProject->m_strProjectDir;
			str2.MakeLower();

			if( str1.Find( str2 ) == 0 )
			{
				CDirectoryNode* pDirNode = pProject->FindDirNode( pTheProject->m_strProjectDir );
				if( pDirNode )
				{
					// Delete directory from Project Tree and hard drive
					pDirNode->AddRef();
					pDirNode->DeleteDirectoryToRecycleBin();
					pDirNode->Release();
				}
			}
		}
	}

	// Delete the Project
	BOOL fReturn = FALSE;

	pTheProject->AddRef();
	if( pTheProject->DeleteAllFilesToRecycleBin( nWhichFiles, fRuntimePrompt ) )
	{
		fReturn = TRUE;
	}
	pTheProject->Release();

	return fReturn;
}


/////////////////////////////////////////////////////////////////////////////
// CJazzApp::SaveAllProjects

BOOL CJazzApp::SaveAllProjects() 
{
	CProject* pProject;
	CString strFileName;

	BOOL fSuccess = TRUE;

    POSITION pos = m_lstProjects.GetHeadPosition();
    while( pos )
    {
        pProject = m_lstProjects.GetNext( pos );

		ASSERT( pProject->m_pProjectDoc != NULL );
		strFileName = pProject->m_pProjectDoc->GetPathName();
		fSuccess = pProject->m_pProjectDoc->OnSaveDocument( strFileName );

		if( fSuccess == FALSE )
		{
			break;
		}
    }

	return fSuccess;
}


/////////////////////////////////////////////////////////////////////////////
// CJazzApp::CloseAllProjects

void CJazzApp::CloseAllProjects() 
{
	CProject* pProject;

    POSITION pos = m_lstProjects.GetHeadPosition();
    while( pos )
    {
        pProject = m_lstProjects.GetNext( pos );

		ASSERT( pProject->m_pProjectDoc != NULL );
		pProject->m_pProjectDoc->OnCloseDocument();
    }
}


/////////////////////////////////////////////////////////////////////////////
// CJazzApp::GetFirstProject

HRESULT CJazzApp::GetFirstProject( IDMUSProdProject** ppIFirstProject )
{
	if( ppIFirstProject == NULL )
	{
		return E_POINTER;
	}
	
	*ppIFirstProject = NULL;

	if( !m_lstProjects.IsEmpty() )
	{
		CProject* pProject = static_cast<CProject*>( m_lstProjects.GetHead() );
		
		if( pProject )
		{
			pProject->AddRef();
			*ppIFirstProject = pProject;
		}
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CJazzApp::GetNextProject

HRESULT CJazzApp::GetNextProject( IDMUSProdProject* pIProject, IDMUSProdProject** ppINextProject )
{
	if( ppINextProject == NULL )
	{
		return E_POINTER;
	}
	
	*ppINextProject = NULL;

	if( pIProject == NULL )
	{
		return E_INVALIDARG;
	}

	CProject* pListProject;

    POSITION pos = m_lstProjects.GetHeadPosition();

    while( pos )
    {
        pListProject = m_lstProjects.GetNext( pos );
		if( pListProject == (CProject *)pIProject )
		{
			if( pos )
			{
				pListProject = m_lstProjects.GetNext( pos );

				pListProject->AddRef();
				*ppINextProject = pListProject;
			}
			break;
		}
    }

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CJazzApp GetProjectByGUID

CProject* CJazzApp::GetProjectByGUID( GUID guidProject )
{
	CProject* pProject;

    POSITION pos = m_lstProjects.GetHeadPosition();

    while( pos )
    {
        pProject = m_lstProjects.GetNext( pos );
		if( IsEqualGUID ( pProject->m_guid, guidProject ) )
		{
			pProject->AddRef();
			return pProject;
		}
    }

	return NULL;
}


/////////////////////////////////////////////////////////////////////////////
// CJazzApp GetProjectByFileName

CProject* CJazzApp::GetProjectByFileName( LPCTSTR szFileName )
{
	CProject* pProject;
	CString strProjectDir;
	CString strFileName = szFileName;

	strFileName.MakeLower();

    POSITION pos = m_lstProjects.GetHeadPosition();
    while( pos )
    {
        pProject = m_lstProjects.GetNext( pos );

		strProjectDir = pProject->m_strProjectDir;
		strProjectDir.MakeLower();

		if( strFileName.Find( strProjectDir ) == 0 )
		{
			pProject->AddRef();
			return pProject;
		}
    }

	return NULL;
}


/////////////////////////////////////////////////////////////////////////////
// CJazzApp GetProjectByProjectDir

CProject* CJazzApp::GetProjectByProjectDir( LPCTSTR szProjectDir )
{
	CProject* pProject;
	CString strProjectDir = szProjectDir;

    POSITION pos = m_lstProjects.GetHeadPosition();
    while( pos )
    {
        pProject = m_lstProjects.GetNext( pos );

		if( strProjectDir.CompareNoCase( pProject->m_strProjectDir ) == 0 )
		{
			pProject->AddRef();
			return pProject;
		}
    }

	return NULL;
}


/////////////////////////////////////////////////////////////////////////////
// CJazzApp IsFileOpenInDiffProject

BOOL CJazzApp::IsFileOpenInDiffProject( GUID guidFile, CProject* pThisProject )
{
	CProject* pProject;
	CFileNode* pFileNode;

    POSITION pos = m_lstProjects.GetHeadPosition();
    while( pos )
    {
        pProject = m_lstProjects.GetNext( pos );

		if( pProject != pThisProject )
		{
			// File is in a different Project
			pFileNode = pProject->GetFileByGUID( guidFile );
			if( pFileNode )
			{
				pFileNode->Release();

				if( pFileNode->m_pIChildNode )
				{
					// File is open
					return TRUE;
				}
			}
		}
    }

	return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CJazzApp GetFileByGUID

CFileNode* CJazzApp::GetFileByGUID( GUID guidFile )
{
	CProject* pProject;
	CFileNode* pFileNode;

    POSITION pos = m_lstProjects.GetHeadPosition();
    while( pos )
    {
        pProject = m_lstProjects.GetNext( pos );

		pFileNode = pProject->GetFileByGUID( guidFile );
		if( pFileNode )
		{
//			pFileNode->AddRef();	intentionally missing
//									AddRef()'d in CProject::GetFileByGUID() 
			return pFileNode;
		}
    }

	return NULL;
}


/////////////////////////////////////////////////////////////////////////////
// CJazzApp GetFileByDocRootNode

CFileNode* CJazzApp::GetFileByDocRootNode( IDMUSProdNode* pIDocRootNode )
{
	CProject* pProject;
	CFileNode* pFileNode;

    POSITION pos = m_lstProjects.GetHeadPosition();

    while( pos )
    {
        pProject = m_lstProjects.GetNext( pos );

		pFileNode = pProject->GetFileByDocRootNode( pIDocRootNode );
		if( pFileNode )
		{
//			pFileNode->AddRef();	intentionally missing
//									AddRef()'d in CProject::GetFileByGUID() 
			return pFileNode;
		}
    }

	return NULL;
}


/////////////////////////////////////////////////////////////////////////////
// CJazzApp CleanUpNotifyLists

void CJazzApp::CleanUpNotifyLists( CFileNode* pFileNode, BOOL fFileClosed  )
{
	CProject* pProject;

    POSITION pos = m_lstProjects.GetHeadPosition();

    while( pos )
    {
        pProject = m_lstProjects.GetNext( pos );
		pProject->CleanUpNotifyLists( pFileNode, fFileClosed  );
    }
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


static BOOL GetFileVersion( LPTSTR szExeName, LPTSTR szFileVersion, short nFileVersionSize )
{
	DWORD dwBufferSize;
	DWORD dwReserved;
	
	dwBufferSize = GetFileVersionInfoSize( szExeName, &dwReserved );
	if( dwBufferSize > 0 )
	{
		void* pBuffer;

		pBuffer = (void *)malloc( dwBufferSize );
		if( pBuffer )
		{
			CString strTheFileVersion;
			VS_FIXEDFILEINFO* pFixedInfo;
			UINT nInfoSize;

			GetFileVersionInfo( szExeName, dwReserved, dwBufferSize, pBuffer );
			VerQueryValue( pBuffer, _T("\\"), (void **)&pFixedInfo, &nInfoSize );

			WORD wVer1 = HIWORD( pFixedInfo->dwFileVersionMS );
			WORD wVer2 = LOWORD( pFixedInfo->dwFileVersionMS );
			WORD wVer3 = HIWORD( pFixedInfo->dwFileVersionLS );
			WORD wVer4 = LOWORD( pFixedInfo->dwFileVersionLS );

			strTheFileVersion.Format( _T("%u.%u.%u.%u"), wVer1, wVer2, wVer3, wVer4 );

			_tcsncpy( szFileVersion, strTheFileVersion, nFileVersionSize );

			free( pBuffer );
			return TRUE;
		}
	}

	return FALSE;
}

BOOL CAboutDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	// Get version information
	TCHAR achJazzExeName[FILENAME_MAX + 1];
	TCHAR achFileVersion[MID_BUFFER];

	if( GetModuleFileName ( theApp.m_hInstance, achJazzExeName, FILENAME_MAX ) )
	{
		if( GetFileVersion( achJazzExeName, achFileVersion, MID_BUFFER ) )
		{
			CString strFileVersion;

			AfxFormatString1( strFileVersion, IDS_PRODUCER_VERSION_TEXT, achFileVersion );
			SetDlgItemText( IDC_FILE_VERSION, strFileVersion );
		}
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

// App command to run the dialog
void CJazzApp::OnAppAbout()
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CJazzApp commands

/////////////////////////////////////////////////////////////////////////////
// CJazzApp::PreTranslateMessage

BOOL CJazzApp::PreTranslateMessage( MSG* pMsg )
{
	// CG: The following lines were added by the Splash Screen component.
	if( CSplashWnd::PreTranslateAppMessage( pMsg ) )
	{
		return TRUE;
	}

	if( m_pMainWnd )
	{
		if( pMsg->message == WM_KEYDOWN )
		{
			if( pMsg->wParam == ' ' )
			{
				TCHAR tcstrClassName[10];
				if( ::GetClassName( pMsg->hwnd, tcstrClassName, 9 ) )
				{
					if( (0 == _tcscmp( tcstrClassName, _T("Edit")) )
					||	(0 == _tcscmp( tcstrClassName, _T("Button")) ) )
					/* Don't think we need to mask out these
					||	(0 == _tcscmp( tcstrClassName, _T("ListBox")) )
					||	(0 == _tcscmp( tcstrClassName, _T("ComboBox")) ) )
					*/
					{
						return CWinApp::PreTranslateMessage( pMsg );
					}
				}
			}
			else if( ( (pMsg->wParam >= '0') && (pMsg->wParam <= '9') )
				||	 ( (pMsg->wParam >= 'A') && (pMsg->wParam <= 'Z') )
				||	 ( (pMsg->wParam >= 'a') && (pMsg->wParam <= 'z') ) )
			{
				// Check if both the CONTROL and ALT keys are down
				if( (GetKeyState( VK_CONTROL ) & 0x8000)
				&&	(GetKeyState( VK_MENU  ) & 0x8000) )
				{
					// Get IPrivateTransport interface pointer
					IDMUSProdComponent* pIComponent = NULL;
					IPrivateTransport *pITransport = NULL;
					if( SUCCEEDED ( m_pFramework->FindComponent( CLSID_CConductor,  &pIComponent ) ) )
					{
						if( SUCCEEDED ( pIComponent->QueryInterface( IID_IPrivateTransport, (void**)&pITransport ) ) )
						{
							int nIndex;
							if( (pMsg->wParam >= '1') && (pMsg->wParam <= '9') )
							{
								nIndex = pMsg->wParam - '1';
							}
							else if( pMsg->wParam == '0' )
							{
								nIndex = 9;
							}
							else if( (pMsg->wParam >= 'A') && (pMsg->wParam <= 'Z') )
							{
								nIndex = pMsg->wParam - 'A' + 10;
							}
							else
							{
								nIndex = pMsg->wParam - 'a' + 10;
							}

							pITransport->PlaySecondarySegment( nIndex );
							pITransport->Release();
						}
						pIComponent->Release();
					}
				}
			}
		}

		if( ::TranslateAccelerator(m_pMainWnd->m_hWnd, m_hAcceleratorTable, pMsg) )
		{
			return TRUE;
		}
	}

	return CWinApp::PreTranslateMessage( pMsg );
}


/////////////////////////////////////////////////////////////////////////////
// IsIdleMessage

BOOL CJazzApp::IsIdleMessage(MSG* pMsg)
{
	// Return FALSE if the message just dispatched should _not_
	// cause OnIdle to be run.  Messages which do not usually
	// affect the state of the user interface and happen very
	// often are checked for.

	// redundant WM_MOUSEMOVE and WM_NCMOUSEMOVE
	if (pMsg->message == WM_MOUSEMOVE || pMsg->message == WM_NCMOUSEMOVE)
	{
		// mouse move at same position as last mouse move?
		if (m_ptCursorLast == pMsg->pt && pMsg->message == m_nMsgLast)
			return FALSE;

		m_ptCursorLast = pMsg->pt;  // remember for next time
		m_nMsgLast = pMsg->message;
		return TRUE;
	}

	// WM_PAINT, WM_SYSTIMER (caret blink), and WM_TIMER
	return pMsg->message != WM_PAINT && pMsg->message != 0x0118 && pMsg->message != WM_TIMER;
}


/////////////////////////////////////////////////////////////////////////////
// RemoveLastSlash

void CJazzApp::RemoveLastSlash( CString& strName )
{
	int nLength = strName.GetLength();

	if( nLength > 3 )
	{
		if( strName.Right(1) == _T("\\") )
		{
			strName = strName.Left( nLength - 1 );
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// GetRegString

static void GetRegString( HKEY hKey, LPCTSTR lpSubKey, LPTSTR lpValueName, LPTSTR szString )
{
	HKEY  hKeyOpen;
	LONG  lResult;

	_tcscpy( szString, _T("") );
	
	lResult = RegOpenKeyEx( hKey, lpSubKey, 0, KEY_QUERY_VALUE, &hKeyOpen );
	
	if( lResult == ERROR_SUCCESS )
	{
		DWORD dwType;
		DWORD dwCbData = MAX_PATH;

		lResult = RegQueryValueEx( hKeyOpen, lpValueName, NULL, &dwType, (LPBYTE)szString, &dwCbData );

		if( (lResult != ERROR_SUCCESS)
		||  (dwType  != REG_SZ) )
		{
			_tcscpy( szString, _T("") );
		}

		RegCloseKey( hKeyOpen );
	}
}


/////////////////////////////////////////////////////////////////////////////
// GetDefaultProjectDir

void CJazzApp::GetDefaultProjectDir( CString& strProjectDir )
{
	TCHAR achBuffer[ MAX_PATH ];

	GetRegString( HKEY_CURRENT_USER,
				  "Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders",
				  "Personal",
				  achBuffer );

	CString strDir = achBuffer;

	if( strDir.IsEmpty() )
	{
		strDir = m_strAppDir;
	}

	MakeTheDirectory( strProjectDir, strDir, IDS_PROJECT_DIR );
}


/////////////////////////////////////////////////////////////////////////////
// SetDefaultDirForFileOpen

void CJazzApp::SetDefaultDirForFileOpen( CDocTemplate * pTemplate, LPCTSTR szFileName )
{
	ASSERT( pTemplate != NULL );
	ASSERT( szFileName != NULL );

	if( pTemplate == (CDocTemplate *)-1 )
	{
		WritePrivateProfileString( _T("Directories"), _T("OpenExt"), _T("*"), m_pszProfileName );
	}
	else
	{
		TCHAR achExt[_MAX_EXT];

		_tsplitpath( szFileName, NULL, NULL, NULL, achExt );
		WritePrivateProfileString( _T("Directories"), _T("OpenExt"), &achExt[1], m_pszProfileName );
	}
		
	CString strDir = szFileName;

	// Find the directory of the file
	int nFindPos = strDir.ReverseFind( (TCHAR)'\\' );
	if( nFindPos != -1 )
	{
		strDir = strDir.Left( nFindPos + 1 );

		// Store the file's directory
		WritePrivateProfileString( _T("Directories"), _T("OpenDir"), strDir, m_pszProfileName );
	}
}


/////////////////////////////////////////////////////////////////////////////
// GetDefaultDirForFileOpen

void CJazzApp::GetDefaultDirForFileOpen( CString& strExt, CString& strDir )
{
	BOOL fSuccess = FALSE;

	GetPrivateProfileString( _T("Directories"), _T("OpenExt"),
	 					     _T(""), strExt.GetBuffer(_MAX_EXT), _MAX_EXT, m_pszProfileName );
	strExt.ReleaseBuffer();

	if( strExt.Compare( _T("*") ) == 0 )
	{
		fSuccess = TRUE;
	}
	else
	{
		IDMUSProdDocType* pIDocType;

		strExt = _T(".") + strExt;
		BSTR bstrExt = strExt.AllocSysString();

		if( SUCCEEDED ( m_pFramework->FindDocTypeByExtension( bstrExt, &pIDocType ) ) )
		{
			fSuccess = TRUE;
			pIDocType->Release();
		}
	}

	if( fSuccess )
	{
		GetPrivateProfileString( _T("Directories"), _T("OpenDir"),
	 							 _T(""), strDir.GetBuffer(_MAX_PATH), _MAX_PATH, m_pszProfileName );
		strDir.ReleaseBuffer();

		if( !strDir.IsEmpty() )
		{
			if( strDir.Right(1) != _T("\\") )
			{
				strDir += _T("\\");
			}
		}
	}
	else
	{
		strExt.Empty();
		strDir.Empty();
	}
}


/////////////////////////////////////////////////////////////////////////////
// SetDefaultDirForFileSave

void CJazzApp::SetDefaultDirForFileSave( LPCTSTR szFileName, LPCTSTR szUseThisExt )
{
	ASSERT( szFileName != NULL );

	// Find the file's extension
	TCHAR achExt[_MAX_EXT];
	if( szUseThisExt )
	{
		_tsplitpath( szUseThisExt, NULL, NULL, NULL, achExt );
	}
	else
	{
		_tsplitpath( szFileName, NULL, NULL, NULL, achExt );
	}

	// Find the directory of the file
	CString strDir = szFileName;

	int nFindPos = strDir.ReverseFind( (TCHAR)'\\' );
	if( nFindPos != -1 )
	{
		strDir = strDir.Left( nFindPos + 1 );

		// Store the file's directory
		WritePrivateProfileString( _T("Directories"), &achExt[1], strDir, m_pszProfileName );
	}
}


/////////////////////////////////////////////////////////////////////////////
// GetDefaultDirForFileSave

void CJazzApp::GetDefaultDirForFileSave( LPCTSTR szFileName, CString& strDir )
{
	ASSERT( szFileName != NULL );

	// Find the file's extension
	TCHAR achExt[_MAX_EXT];

	_tsplitpath( szFileName, NULL, NULL, NULL, achExt );

	// Find the directory of the file
	GetPrivateProfileString( _T("Directories"), &achExt[1],
	 					     _T(""), strDir.GetBuffer(_MAX_PATH), _MAX_PATH, m_pszProfileName );
	strDir.ReleaseBuffer();

	if( !strDir.IsEmpty() )
	{
		if( strDir.Right(1) != _T("\\") )
		{
			strDir += _T("\\");
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// SetProjectDirForFileOpen

void CJazzApp::SetProjectDirForFileOpen( LPCTSTR szFileName )
{
	ASSERT( szFileName != NULL );

	CString strProjectDir = szFileName;

	// Find parent directory of the Project file
	int nFindPos = strProjectDir.ReverseFind( (TCHAR)'\\' );
	if( nFindPos != -1 )
	{
		strProjectDir = strProjectDir.Left( nFindPos );

		nFindPos = strProjectDir.ReverseFind( (TCHAR)'\\' );
		if( nFindPos != -1 )
		{
			strProjectDir = strProjectDir.Left( nFindPos + 1 );

			// Store the parent directory
			WritePrivateProfileString( _T("Directories"), _T("pro"), strProjectDir, m_pszProfileName );
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// GetProjectDirForFileOpen

void CJazzApp::GetProjectDirForFileOpen( CString& strProjectDir )
{
	GetPrivateProfileString( _T("Directories"), _T("pro"),
	 					     _T(""), strProjectDir.GetBuffer(_MAX_PATH), _MAX_PATH, m_pszProfileName );
	strProjectDir.ReleaseBuffer();

	if( strProjectDir.IsEmpty() )
	{
		GetDefaultProjectDir( strProjectDir );
	}

	if( !strProjectDir.IsEmpty() )
	{
		if( strProjectDir.Right(1) != _T("\\") )
		{
			strProjectDir += _T("\\");
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CJazzApp::AdjustFileName

void CJazzApp::AdjustFileName( FileType ftFileType, LPCTSTR szOrigFileName, CString& strTheFileName )
{
	CString strFileName;
	CString strFilterExt;
	CString strExt;
	BSTR bstrExt;

	ASSERT( szOrigFileName != NULL );
	strFileName = szOrigFileName;
	strTheFileName = szOrigFileName;

	// Remove extension
	int nFindPos = strFileName.ReverseFind( (TCHAR)'.' );
	if( nFindPos != -1 )
	{
		strExt = strFileName.Right( strFileName.GetLength() - nFindPos );
		strFileName = strFileName.Left( nFindPos );
	}

	//Get the DocTemplate
	IDMUSProdDocType* pIDocType;
	bstrExt = strExt.AllocSysString();

	if( FAILED ( m_pFramework->FindDocTypeByExtension( bstrExt, &pIDocType ) ) )
	{
		return;
	}

	CJazzDocTemplate* pDocTemplate = FindDocTemplate( pIDocType );
	ASSERT( pDocTemplate != NULL );

	// Find the appropriate file extension
	if( pDocTemplate->GetDocString(strFilterExt, CDocTemplate::filterExt)
	&&  !strFilterExt.IsEmpty() )
	{
		ASSERT( strFilterExt[0] == '.' );

		BOOL fContinue = TRUE;

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

			if( pIDocType->IsFileTypeExtension(ftFileType, bstrExt) == S_OK )
			{
				strTheFileName = strFileName + strExt;
				break;
			}

			nFindPos = strFilterExt.Find( _T("*") );
		}
	}

	pIDocType->Release();
}


/////////////////////////////////////////////////////////////////////////////
// CJazzApp::SplitOutFileName

void CJazzApp::SplitOutFileName( LPCTSTR szCompleteFileName, BOOL fIncludeExt, CString& strName )
{
	// Split filename
	TCHAR achName[FILENAME_MAX];
	TCHAR achFName[_MAX_FNAME];
	TCHAR achExt[_MAX_EXT];

	_tsplitpath( szCompleteFileName, NULL, NULL, achFName, achExt );

	// Populate strName
	if( fIncludeExt )
	{
		_tmakepath( achName, NULL, NULL, achFName, achExt );
		strName = achName;
	}
	else
	{
		strName = achFName;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CJazzApp::IsFileNameUnique

BOOL CJazzApp::IsFileNameUnique( LPCTSTR szFileName )
{
	DWORD dwAttributes = ::GetFileAttributes( szFileName );
	if( dwAttributes == 0xFFFFFFFF )
	{
		// File does not exist
		return TRUE;
	}

	return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CJazzApp::GetUniqueFileName

void CJazzApp::GetUniqueFileName( LPCTSTR szCompleteFileName, CString& strUniqueFName )
{
	CString	strOrigFName;
	CString	strNbr;
	int	nOrigNameLength;
	int	nNbrLength;
	int	i;

	// Construct complete path/filename
	TCHAR achFileName[FILENAME_MAX];

	_tcscpy( achFileName, szCompleteFileName );

	// Split filename into components
	TCHAR achDrive[_MAX_DRIVE];
	TCHAR achDir[_MAX_DIR];
	TCHAR achFName[_MAX_DIR];
	TCHAR achExt[_MAX_DIR];

	_tsplitpath( achFileName, achDrive, achDir, achFName, achExt );
	strOrigFName = achFName;

	// Strip number from right side of FName
	while( !strOrigFName.IsEmpty()  &&  _istdigit(strOrigFName[strOrigFName.GetLength() - 1]) )
	{
		strOrigFName = strOrigFName.Left( strOrigFName.GetLength() - 1 );
	}

	nOrigNameLength = strOrigFName.GetLength();
	i = 0;

	// Make sure filename is unique
	while( IsFileNameUnique( achFileName ) == FALSE )
	{
		strNbr.Format( "%d", ++i ); 
		nNbrLength = strNbr.GetLength();
		
		if( (nOrigNameLength + nNbrLength) <= MAX_LENGTH_FILE_NAME )
		{
			_tmakepath( achFileName, achDrive, achDir, strOrigFName + strNbr, achExt );
		}
		else
		{
			_tmakepath( achFileName, achDrive, achDir, strOrigFName.Left(MAX_LENGTH_FILE_NAME - nNbrLength) + strNbr, achExt );
		}
	}

	// Return unique filename 
	SplitOutFileName( achFileName, TRUE, strUniqueFName );
}


/////////////////////////////////////////////////////////////////////////////
// CJazzApp::DisplaySystemError

void CJazzApp::DisplaySystemError( UINT nResourceID, LPCTSTR szContext )
{
	CString strMsg;
	LPVOID lpMessageBuffer;
	
	ASSERT( szContext != NULL );

	FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
				   NULL, GetLastError(),
				   MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), //The user default language
				   (LPTSTR)&lpMessageBuffer, 0, NULL );

	HINSTANCE hInstanceOld = AfxGetResourceHandle();
	AfxSetResourceHandle( m_hInstance );

	AfxFormatString1( strMsg, nResourceID, szContext );
	strMsg = strMsg + _T("  ") + (LPTSTR)lpMessageBuffer;
	AfxMessageBox( strMsg );

	AfxSetResourceHandle( hInstanceOld );

	LocalFree( lpMessageBuffer );
}


/////////////////////////////////////////////////////////////////////////////
// CJazzApp::OpenTheFile

CCommonDoc* CJazzApp::OpenTheFile( LPCTSTR szFileName, short nFileTarget )
{
	m_nFileTarget = nFileTarget;

	CCommonDoc* pCommonDoc = (CCommonDoc *)OpenDocumentFile( szFileName );

	m_nFileTarget = TGT_PROJECT;

	return pCommonDoc;
}


/////////////////////////////////////////////////////////////////////////////
// CJazzApp::InsertForeignFile

CFileNode* CJazzApp::InsertForeignFile( LPCTSTR szFileName )
{
	CFileNode* pFileNode = NULL;
	CString strNewFileName;
	CString strPath;
	CString strName;

// Prepare filename
	TCHAR achPath[_MAX_PATH];
	ASSERT(lstrlen(szFileName) < _countof(achPath));

	TCHAR achTemp[_MAX_PATH];
	if( szFileName[0] == '\"' )
	{
		++szFileName;
	}

	lstrcpyn( achTemp, szFileName, _MAX_PATH );
	LPTSTR szLast = _tcsrchr( achTemp, '\"' );
	if( szLast != NULL )
	{
		*szLast = 0;
	}

	AfxFullPath( achPath, achTemp );
	TCHAR szLinkName[_MAX_PATH];
	if( AfxResolveShortcut(AfxGetMainWnd(), achPath, szLinkName, _MAX_PATH) )
	{
		lstrcpy( achPath, szLinkName );
	}

// Find the parent node
	m_nFileTarget = TGT_SELECTEDNODE;
	CDirectoryNode* pParentDirNode = (CDirectoryNode *)m_pFramework->DetermineParentNode( achPath );
	m_nFileTarget = TGT_PROJECT;

	if( pParentDirNode == NULL )
	{
		return NULL;
	}

// Determine the target filename
	SplitOutFileName( achPath, TRUE, strName );
	pParentDirNode->ConstructPath( strPath );
	strNewFileName = strPath + strName;

// Make sure the target directory exists
	if( CreateTheDirectory( strPath ) == FALSE )
	{
		return NULL;
	}

// See if a FileNode already exists
	pFileNode = FindFileNode( strNewFileName );
	if( pFileNode )
	{
		if( strNewFileName.CompareNoCase( szFileName ) != 0 )
		{
			CString strRelativePath;
			CString strMsg;

			// A different file with the same name already exists in the Project
			// See if the user wants to overwrite it
			if( pFileNode->ConstructRelativePath( strRelativePath ) )
			{
				strRelativePath = _T("..\\") + strRelativePath;
				if( strRelativePath.Right(1) != _T("\\") )
				{
					strRelativePath += _T("\\");
				}
				strRelativePath += strNewFileName;
			}

			HINSTANCE hInstance = AfxGetResourceHandle();
			AfxSetResourceHandle( m_hInstance );

			AfxFormatString2( strMsg, IDS_OVERWRITE_EXISTING, pFileNode->m_pProject->m_strName, strRelativePath );
			BOOL fReturn = AfxMessageBox( strMsg, MB_YESNO|MB_DEFBUTTON2 );
			
			AfxSetResourceHandle( hInstance );
			
			if( fReturn == IDNO )
			{
				return NULL;
			}
		}
	}

// Copy the file into the Project
	if( CopyTheFile( szFileName, strNewFileName, FALSE ) )
	{
		BOOL fInsertNode = FALSE;

		// Make sure we have a FileNode
		if( pFileNode == NULL )
		{
			pFileNode = new CFileNode;
			fInsertNode = TRUE;
		}
		
		if( pFileNode )
		{
			// Add file to Project Tree
			if( fInsertNode )
			{
				pParentDirNode->InsertChildNode( pFileNode );
			}

			BSTR bstrName = strName.AllocSysString();
			pFileNode->SetNodeName( bstrName );

			CTreeCtrl* pTreeCtrl = theApp.GetProjectTreeCtrl();
			if( pParentDirNode->m_hItem )
			{
				CMainFrame* pMainFrame = (CMainFrame *)theApp.m_pMainWnd;
				pMainFrame->m_wndTreeBar.SortChildren( pParentDirNode->m_hItem );
			}

			// Display the node
			m_pFramework->ShowTreeNode( pFileNode );
		}
	}

	return pFileNode;
}


/////////////////////////////////////////////////////////////////////////////
// CJazzApp::CopyTheFile

BOOL CJazzApp::CopyTheFile( LPCTSTR szOldFile, LPCTSTR szNewFile, BOOL fFlags )
{
	ASSERT( szOldFile != NULL );
	ASSERT( szNewFile != NULL );

	// Copy the file
	if( CopyFile( szOldFile, szNewFile, fFlags ) )
	{
		SHChangeNotify(SHCNE_CREATE, SHCNF_PATH, szNewFile, 0 );
		return TRUE;
	}

	DisplaySystemError( IDS_SYSERR_COPY, szOldFile );
	return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CJazzApp::RenameThePath

BOOL CJazzApp::RenameThePath( LPCTSTR szOldName, LPCTSTR szNewName, LPCTSTR szMRUName )
{
	ASSERT( szOldName != NULL );
	ASSERT( szNewName != NULL );

	// Set old name
	CString strOldName = szOldName;
	RemoveLastSlash( strOldName );

	// Set new name
	CString strNewName = szNewName;
	RemoveLastSlash( strNewName );

	if( _trename(strOldName, strNewName) != 0 )
	{
		int nFindPos = strOldName.ReverseFind( (TCHAR)'\\' );
		if( nFindPos != -1 )
		{
			strOldName = strOldName.Right( strOldName.GetLength() - nFindPos - 1 );
		}

		DisplaySystemError( IDS_SYSERR_RENAME, strOldName  );
		return FALSE;
	}

	RemoveFromRecentFileList( szMRUName );
	AddToRecentFileList( szNewName );

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CJazzApp::RemoveFromRecentFileList

void CJazzApp::RemoveFromRecentFileList( LPCTSTR szPathName )
{
	ASSERT( szPathName != NULL );
	ASSERT( AfxIsValidString(szPathName) );
	ASSERT( m_pRecentFileList->m_arrNames != NULL);

	// update the MRU list, if an existing MRU string matches file name
	for( int iMRU = 0;  iMRU < m_pRecentFileList->m_nSize-1;  iMRU++ )
	{
		if( AfxComparePath(m_pRecentFileList->m_arrNames[iMRU], szPathName) )
		{
			// matching file
			m_pRecentFileList->Remove( iMRU );
			break;   
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CJazzApp::DeleteFileToRecycleBin

HRESULT CJazzApp::DeleteFileToRecycleBin( LPCTSTR szFile )
{
	SHFILEOPSTRUCT FileStruct;
	TCHAR achFile[_MAX_PATH + 2];

	ASSERT( szFile != NULL );

	// Delete the file
	SetFileAttributes( szFile, FILE_ATTRIBUTE_ARCHIVE );

	ZeroMemory( &FileStruct, sizeof(FileStruct) );
	ZeroMemory( achFile, sizeof(achFile) );
	_tcscpy( achFile, szFile ); 

	FileStruct.pFrom = achFile;		// terminate with 2 NULL bytes
	FileStruct.fFlags = FOF_SILENT | FOF_ALLOWUNDO | FOF_NOCONFIRMATION;
	FileStruct.wFunc = FO_DELETE;
	
	if( SHFileOperation( &FileStruct ) )
	{
		return E_FAIL;
	}

	// Clean up recent file list
	RemoveFromRecentFileList( szFile );
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CJazzApp::DeleteEmptyDirToRecycleBin

void CJazzApp::DeleteEmptyDirToRecycleBin( LPCTSTR szDirectory )
{
	ASSERT( szDirectory != NULL );

	CString strDirectory = szDirectory;
	RemoveLastSlash( strDirectory );

	// Delete the directory tree
	DeleteEmptyDirTreeToRecycleBin( strDirectory );

	// Make sure we aren't trying to delete the current directory
	TCHAR achCurrentDir[_MAX_PATH + 1];

	DWORD dwNbrBytes = GetCurrentDirectory( _MAX_PATH, achCurrentDir );
	if( (dwNbrBytes > 0) 
	&&  (dwNbrBytes < _MAX_PATH) )
	{
		CString strCurrentDir = achCurrentDir;

		if( _tcsicmp( strCurrentDir, strDirectory ) == 0 )
		{
			int nFindPos = strCurrentDir.ReverseFind( (TCHAR)'\\' );
			if( nFindPos != -1 )
			{
				strCurrentDir = strCurrentDir.Left( nFindPos );
				SetCurrentDirectory( strCurrentDir );
			}
		}
	}

	// Remove the directory
	if( IsDirectoryEmpty( strDirectory ) )
	{
		if( RemoveDirectory( strDirectory ) )
		{
			SHChangeNotify(SHCNE_RMDIR, SHCNF_PATH, strDirectory, 0 );
		}
		else
		{
			DisplaySystemError( IDS_SYSERR_DELETE, strDirectory );
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CJazzApp::DeleteEmptyDirTreeToRecycleBin

void CJazzApp::DeleteEmptyDirTreeToRecycleBin( LPCTSTR szDirectory )
{
	ASSERT( szDirectory != NULL );

	// Delete the directory tree
	WIN32_FIND_DATA	fd;
	TCHAR achFileMask[_MAX_PATH + 1];

	if( szDirectory[ _tcslen(szDirectory)-1 ] != _T('\\') )
	{
		wsprintf( achFileMask, "%s\\*.*", szDirectory );
	}
	else
	{
		wsprintf( achFileMask, "%s*.*", szDirectory );
	}
	
	HANDLE hFind = FindFirstFile( achFileMask, &fd );
	if( hFind != INVALID_HANDLE_VALUE )
	{
		do{
			if( !_tcsicmp(fd.cFileName, _T("."))
			||  !_tcsicmp(fd.cFileName, _T("..")) )
			{
				continue;
			}

			_tcscpy( strrchr(achFileMask, '\\')+1, fd.cFileName );

			if( fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
			{
				DeleteEmptyDirToRecycleBin( achFileMask );
			}
		}
		while( FindNextFile( hFind, &fd ) );

		FindClose( hFind );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CJazzApp::IsDirectoryEmpty

BOOL CJazzApp::IsDirectoryEmpty( LPCTSTR szDirectory )
{
	BOOL fEmpty = TRUE;

	ASSERT( szDirectory != NULL );

	// See if the directory is empty
	WIN32_FIND_DATA	fd;
	TCHAR achFileMask[_MAX_PATH + 1];

	if( szDirectory[ _tcslen(szDirectory)-1 ] != _T('\\') )
	{
		wsprintf( achFileMask, "%s\\*.*", szDirectory );
	}
	else
	{
		wsprintf( achFileMask, "%s*.*", szDirectory );
	}
	
	HANDLE hFind = FindFirstFile( achFileMask, &fd );
	if( hFind != INVALID_HANDLE_VALUE )
	{
		do{
			if( !_tcsicmp(fd.cFileName, _T("."))
			||  !_tcsicmp(fd.cFileName, _T("..")) )
			{
				continue;
			}

			fEmpty = FALSE;
			break;
		}
		while( FindNextFile( hFind, &fd ) );

		FindClose( hFind );
	}

	return fEmpty;
}


/////////////////////////////////////////////////////////////////////////////
// CJazzApp::CreateTheDirectory

BOOL CJazzApp::CreateTheDirectory( LPCTSTR szDirectory )
{
	LPCTSTR szTheDirectory = szDirectory;

	ASSERT( szDirectory != NULL );

	BOOL fSuccess = TRUE;

	// Make sure the directory exists
	DWORD dwAttributes = GetFileAttributes( szDirectory );

	if( (dwAttributes == 0xFFFFFFFF)
	|| !(dwAttributes & FILE_ATTRIBUTE_DIRECTORY) )
	{
		// Directory does not exist
		TCHAR achCurrentDir[_MAX_PATH + 1];
		TCHAR achOldDir[_MAX_PATH + 1];
		int	nIndex = 0;

		memset( achCurrentDir, 0, _MAX_PATH + 1 );
		memset( achOldDir, 0, _MAX_PATH + 1 );

		GetCurrentDirectory( _MAX_PATH, achOldDir );
		
		if( szDirectory[1] == _T(':') )
		{
			_tcsncpy( achCurrentDir, szDirectory, 3 );
			szDirectory += 3;
			nIndex = 3;
		}
		
		if( szDirectory[0] == _T('\\') 
		&&  szDirectory[1] == _T('\\') )
		{
			// Set current directory to \\server\share when UNC pathname
			_tcsncpy( achCurrentDir, szDirectory, 2 );
			szDirectory += 2;
			nIndex = 2;
			for( ;  (*szDirectory) && (*szDirectory!='\\') ;  nIndex++, szDirectory++ )
			{
				achCurrentDir[nIndex] = *szDirectory;
			}
			achCurrentDir[nIndex] = *szDirectory;
			nIndex++;
			szDirectory++;
			for( ;  (*szDirectory) && (*szDirectory!='\\') ;  nIndex++, szDirectory++ )
			{
				achCurrentDir[nIndex] = *szDirectory;
			}
		}

		SetCurrentDirectory( achCurrentDir );
		memset( achCurrentDir, 0, _MAX_PATH + 1 );
		nIndex = 0;
		if( szDirectory[0] == _T('\\') )
		{
			szDirectory++;
		}

		while( *szDirectory )
		{
			// Get the new directory...
			for( ;  (*szDirectory) && (*szDirectory!='\\') ;  nIndex++, szDirectory++ )
			{
				achCurrentDir[nIndex] = *szDirectory;
			}
			
			dwAttributes = GetFileAttributes( achCurrentDir );

			if( (dwAttributes == 0xFFFFFFFF)
			|| !(dwAttributes & FILE_ATTRIBUTE_DIRECTORY) )
			{
				// Doesn't exist so Create the directory
				if( CreateDirectory( achCurrentDir, 0 ) == FALSE )
				{
					DisplaySystemError( IDS_SYSERR_CREATEDIR, szTheDirectory );
					fSuccess = FALSE;
					break;
				}
			}
			SetCurrentDirectory( achCurrentDir );

			memset( achCurrentDir, 0, _MAX_PATH + 1 );
			nIndex = 0;
			if( *szDirectory )
			{
				szDirectory++;
			}
		}

		SetCurrentDirectory( achOldDir );
	}

	return fSuccess;
}


/////////////////////////////////////////////////////////////////////////////
// CJazzApp::MakeTheDirectory

BOOL CJazzApp::MakeTheDirectory( CString& strDir, LPCTSTR szDirectory, UINT nResourceID )
{
	BOOL fSuccess = TRUE;
	
	CString strDirectory = szDirectory; 

	if( strDirectory.Right(1) != _T("\\") )
	{
		strDirectory += _T("\\");
	}

	if( nResourceID )
	{
		CString strLastNode;
		strLastNode.LoadString( nResourceID );
		strDirectory += strLastNode;
		if( strDirectory.Right(1) != _T("\\") )
		{
			strDirectory += _T("\\");
		}
	}

	// Make sure the directory exists
	fSuccess =  CreateTheDirectory( strDirectory );

	if( fSuccess )
	{
		strDir = strDirectory;
	}
	else
	{
		strDir.Empty();
	}

	return fSuccess;
}


/////////////////////////////////////////////////////////////////////////////
// CJazzApp::OnWindowCloseAll

void CJazzApp::OnWindowCloseAll() 
{
	CDocTemplate* pTemplate;
	CComponentDoc* pDocument;

	// no doc manager - no templates
	if( m_pDocManager == NULL )
	{
		return;
	}

	theApp.m_nShowNodeInTree++;

	// walk all templates in the application
	POSITION pos = m_pDocManager->GetFirstDocTemplatePosition();
	while( pos != NULL )
	{
		pTemplate = m_pDocManager->GetNextDocTemplate( pos );
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
				pDocument->CloseAllViews( FALSE );
			}
		}
	}

	theApp.m_nShowNodeInTree--;
}


/////////////////////////////////////////////////////////////////////////////
// CJazzApp::ExitInstance

int CJazzApp::ExitInstance() 
{
	ASSERT( m_lstProjects.IsEmpty() );
	ASSERT( m_lstFileGUIDs.IsEmpty() );
	ASSERT( m_lstFileNamesToOpen.IsEmpty() );

	// Delete Bookmarks
	CBookmark* pBookmark;

	while( !m_lstBookmarks.IsEmpty() )
	{
		pBookmark = static_cast<CBookmark*>( m_lstBookmarks.RemoveHead() );
		delete pBookmark;
	}

	CleanupUrlmonStubs(); 

	// Release Jazz framework
	if( m_pFramework )
	{
		m_pFramework->Release();
		m_pFramework = NULL;
	}

	// Release Producer's loader
	if( m_pLoader )
	{
		m_pLoader->Release();
		m_pLoader = NULL;
	}

    if( g_bClassRegistered )
	{
		::UnregisterClass( lpszUniqueClass, AfxGetInstanceHandle() );
	}

	::CoUninitialize();
	
	return CWinApp::ExitInstance();
}


/////////////////////////////////////////////////////////////////////////////
// AppendFilterSuffix

static void AppendFilterSuffix( CString& filter, OPENFILENAME_NT4& ofn,
								CDocTemplate* pTemplate, CString* pstrDefaultExt )
{
	ASSERT_VALID(pTemplate);
	ASSERT_KINDOF(CDocTemplate, pTemplate);

	CString strFilterExt, strFilterName;

	if( pTemplate->GetDocString(strFilterExt, CDocTemplate::filterExt)
	&&  !strFilterExt.IsEmpty()
	&&  pTemplate->GetDocString(strFilterName, CDocTemplate::filterName)
	&&  !strFilterName.IsEmpty() )
	{
		// a file based document template - add to filter list
		ASSERT(strFilterExt[0] == '.');

		if( pstrDefaultExt != NULL )
		{
			// set the default extension
			*pstrDefaultExt = ((LPCTSTR)strFilterExt) + 1;  // skip the '.'
			ofn.lpstrDefExt = (LPTSTR)(LPCTSTR)(*pstrDefaultExt);
			ofn.nFilterIndex = ofn.nMaxCustFilter + 1;  // 1 based number
		}

		// add to filter
		filter += strFilterName;
		ASSERT(!filter.IsEmpty());  // must have a file type name
		filter += (TCHAR)'\0';		// next string please
		filter += (TCHAR)'*';
		filter += strFilterExt;
		filter += (TCHAR)'\0';		// next string please
		ofn.nMaxCustFilter++;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CJazzApp::DoPromptFileSave

BOOL CJazzApp::DoPromptFileSave( CJazzDocTemplate* pTheTemplate, CString& fileName )
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );
	ASSERT( pTheTemplate != NULL );
	ASSERT_VALID( pTheTemplate );

	CFileDialog dlgFile( FALSE );

	CString strTitle;
	VERIFY( strTitle.LoadString( AFX_IDS_SAVEFILE ) );

	dlgFile.m_ofn.Flags |= ( OFN_HIDEREADONLY | OFN_PATHMUSTEXIST );

	CString strFilter;
	CString strDefault;

	AppendFilterSuffix( strFilter, dlgFile.m_ofn, pTheTemplate, &strDefault );

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

	GetDefaultDirForFileSave( dlgFile.m_ofn.lpstrFile, strDefaultDir );
	if( !strDefaultDir.IsEmpty() )
	{
		dlgFile.m_ofn.lpstrInitialDir = strDefaultDir;
	}

	BOOL bResult = dlgFile.DoModal() == IDOK ? TRUE : FALSE;
	fileName.ReleaseBuffer();

	// Store the default directory for the next File Open/Save dialog
	if( bResult )
	{
		SetDefaultDirForFileSave( dlgFile.m_ofn.lpstrFile, NULL );
	}

	return bResult;
}


/////////////////////////////////////////////////////////////////////////////
// CJazzApp::DoPromptFileOpenSingle

BOOL CJazzApp::DoPromptFileOpenSingle( CString& fileName, CDocTemplate* pTheTemplate, LPCTSTR szTitle )
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );
	CString strTitle;
	CString strExt;
	CString strFilter;
	CString strDefault;

	ASSERT( m_pDocManager != NULL );
	ASSERT( pTheTemplate != NULL );

	CFileDialog dlgFile( TRUE );

	// Get the default directory for the File Open dialog
	CProject* pProject = GetActiveProject();
	if( pProject )
	{
		dlgFile.m_ofn.lpstrInitialDir = pProject->m_strProjectDir;
		pProject->Release();
		pProject = NULL;
	}

	if( szTitle )
	{
		strTitle = szTitle;
	}
	else
	{
		VERIFY( strTitle.LoadString( AFX_IDS_OPENFILE ) );
	}

	dlgFile.m_ofn.Flags |= ( OFN_HIDEREADONLY | OFN_FILEMUSTEXIST );

	if( pTheTemplate )
	{
		ASSERT_VALID( pTheTemplate );
		AppendFilterSuffix( strFilter, dlgFile.m_ofn, pTheTemplate, &strDefault );
	}

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

	// Display the File Open dialog
	BOOL bResult = dlgFile.DoModal() == IDOK ? TRUE : FALSE;
	fileName.ReleaseBuffer();

	return bResult;
}


/////////////////////////////////////////////////////////////////////////////
// CJazzApp::DoPromptFileOpenMulti

BOOL CJazzApp::DoPromptFileOpenMulti( CString& fileName, CFileDialog* pFileDlg, CDocTemplate* pTheTemplate, LPCTSTR szTitle )
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );
	CString strTitle;
	CString strExt;
	CString strFilter;
	CString strDefault;
	POSITION pos;

	ASSERT( m_pDocManager != NULL );

	CDocTemplate* pFileOpenDefaultTemplate = NULL;

	// Get the default directory for the File Open dialog
	CString strDefaultDir;
	GetDefaultDirForFileOpen( strExt, strDefaultDir );
	if( strExt.Compare( _T("*") ) == 0 )
	{
		pFileOpenDefaultTemplate = (CDocTemplate *)-1;
	}
	else
	{
		IDMUSProdDocType* pIDocType;
		BSTR bstrExt = strExt.AllocSysString();

		if( SUCCEEDED ( m_pFramework->FindDocTypeByExtension( bstrExt, &pIDocType ) ) )
		{
			pFileOpenDefaultTemplate = (CDocTemplate *)FindDocTemplate( pIDocType );
			pIDocType->Release();
		}
	}
	if( !strDefaultDir.IsEmpty() )
	{
		pFileDlg->m_ofn.lpstrInitialDir = strDefaultDir;
	}

	if( szTitle )
	{
		strTitle = szTitle;
	}
	else
	{
		VERIFY( strTitle.LoadString( AFX_IDS_OPENFILE ) );
	}

	pFileDlg->m_ofn.Flags |= ( OFN_HIDEREADONLY | OFN_FILEMUSTEXIST | OFN_ALLOWMULTISELECT );

	// Get the Project File document template
	CDocTemplate* pProjectTemplate = (CDocTemplate *)FindProjectDocTemplate();

	if( pTheTemplate )
	{
		ASSERT_VALID( pTheTemplate );
		if( pFileOpenDefaultTemplate == NULL )
		{
			pFileOpenDefaultTemplate = pTheTemplate;
		}
		AppendFilterSuffix( strFilter, pFileDlg->m_ofn, pTheTemplate, &strDefault );
	}
	else
	{
		// Do for all doc templates except Project
		pos = m_pDocManager->GetFirstDocTemplatePosition();
		while( pos != NULL )
		{
			CDocTemplate* pTemplate = m_pDocManager->GetNextDocTemplate( pos );

			if( pTemplate != pProjectTemplate )
			{
				if( pFileOpenDefaultTemplate == NULL )
				{
					pFileOpenDefaultTemplate = pTemplate;
				}
				AppendFilterSuffix( strFilter, pFileDlg->m_ofn, pTemplate,
									(pTemplate == pFileOpenDefaultTemplate) ? &strDefault : NULL );
			}
		}
	}


	// Append the "*.*" all files filter
	CString allFilter;
	VERIFY( allFilter.LoadString( AFX_IDS_ALLFILTER ) );
	strFilter += allFilter;
	strFilter += (TCHAR)'\0';   // next string please
	strFilter += _T("*.*");
	strFilter += (TCHAR)'\0';   // last string
	pFileDlg->m_ofn.nMaxCustFilter++;

	// Set the default document template (if default is *.*)
	if( pFileOpenDefaultTemplate == (CDocTemplate *)-1 )
	{
		pFileDlg->m_ofn.lpstrDefExt = _T("*.*");
		pFileDlg->m_ofn.nFilterIndex = pFileDlg->m_ofn.nMaxCustFilter;  // 1 based number
	}

	pFileDlg->m_ofn.lpstrFilter = strFilter;
	pFileDlg->m_ofn.lpstrTitle = strTitle;
	pFileDlg->m_ofn.lpstrFile = fileName.GetBuffer( 2048 );
	pFileDlg->m_ofn.nMaxFile = 2048; // Max filename buffer size for NT 4.0

	// Display the File Open dialog
	BOOL bResult = pFileDlg->DoModal() == IDOK ? TRUE : FALSE;
	fileName.ReleaseBuffer();

	// Determine the default document template
	if( pFileDlg->m_ofn.nFilterIndex == pFileDlg->m_ofn.nMaxCustFilter )
	{
		pFileOpenDefaultTemplate = (CDocTemplate *)-1;
	}
	else
	{
		if( pTheTemplate )
		{
			pFileOpenDefaultTemplate = pTheTemplate;
		}
		else
		{
			DWORD dwIndex = 0;
			pos = m_pDocManager->GetFirstDocTemplatePosition();
			while( pos != NULL )
			{
				CDocTemplate* pTemplate = m_pDocManager->GetNextDocTemplate( pos );
				if( pTemplate != pProjectTemplate )
				{
					if( ++dwIndex == pFileDlg->m_ofn.nFilterIndex )
					{
						pFileOpenDefaultTemplate = pTemplate;
						break;
					}
				}
			}
		}
	}

	// Store the default directory for the next File Open/Save dialog
	if( bResult )
	{
		SetDefaultDirForFileOpen( pFileOpenDefaultTemplate, pFileDlg->m_ofn.lpstrFile );
	}

	return bResult;
}


/////////////////////////////////////////////////////////////////////////////
// CJazzApp::DoPromptProjectOpen

BOOL CJazzApp::DoPromptProjectOpen( CString& fileName )
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );
	CString strDefaultDir;
	CString strTitle;
	CString strDefault;
	CString strFilter;

	CDocTemplate* pTheTemplate = FindProjectDocTemplate();
	if( pTheTemplate == NULL )
	{
		return FALSE;
	}
	ASSERT_VALID( pTheTemplate );

	CFileDialog dlgFile( TRUE );

	GetProjectDirForFileOpen( strDefaultDir );
	if( !strDefaultDir.IsEmpty() )
	{
		dlgFile.m_ofn.lpstrInitialDir = strDefaultDir;
	}

	VERIFY( strTitle.LoadString( IDS_OPEN_PROJECT ) );

	dlgFile.m_ofn.Flags |= ( OFN_HIDEREADONLY | OFN_FILEMUSTEXIST );

	AppendFilterSuffix( strFilter, dlgFile.m_ofn, pTheTemplate, &strDefault );

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

	// Display the File Open dialog
	BOOL bResult = dlgFile.DoModal() == IDOK ? TRUE : FALSE;
	fileName.ReleaseBuffer();

	if( bResult )
	{
		SetProjectDirForFileOpen( fileName );
	}

	return bResult;
}


/////////////////////////////////////////////////////////////////////////////
// CJazzApp::OnUpdateFileOpen

void CJazzApp::OnUpdateFileOpen( CCmdUI* pCmdUI ) 
{
	if( m_lstProjects.IsEmpty() )
	{
		pCmdUI->Enable( FALSE );
	}
	else
	{
		pCmdUI->Enable( TRUE );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CJazzApp::OnFileNew

void CJazzApp::OnFileNew() 
{
	CFileNewDlg dlgFileNew;

	if( dlgFileNew.DoModal() == IDOK )
	{
		if( dlgFileNew.m_nNbrSelItems )
		{
			theApp.m_strNewFileName = dlgFileNew.m_strDefaultName;

			for( int i = 0;  i < dlgFileNew.m_nNbrSelItems ;  i++ )
			{
				CDocTemplate* pTemplate = dlgFileNew.m_apTemplate[i];
			
				if( pTemplate
				&&  pTemplate != (CDocTemplate *)0xFFFFFFFF )
				{
					pTemplate->OpenDocumentFile( NULL );
				}
			}

			theApp.m_strNewFileName.Empty();
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CJazzApp::OnFileOpen

void CJazzApp::OnFileOpen()
{
	CString strFileNames;
	CString strFileName;
	CString strPrompt;
	CFileDialog dlgFile( TRUE );

	strPrompt.LoadString( IDS_ADD_FILE );

	// Prompt user for filename (with all document templates except Project)
	if( DoPromptFileOpenMulti(strFileNames, &dlgFile, NULL, strPrompt) )
	{
		CJzFileName* pJzFileName;
		
		POSITION pos = dlgFile.GetStartPosition();
		while( pos )
		{
			strFileName = dlgFile.GetNextPathName( pos );

			if( IsValidProjectFile( strFileName, FALSE ) )
			{
				CString strMsg;
				AfxFormatString1( strMsg, IDS_ERR_IS_PROJECT_FILE, strFileName );
				AfxMessageBox( strMsg );
			}
			else
			{
				IDMUSProdDocType* pIDocType = GetDocType( strFileName );
				if( pIDocType )
				{
					pJzFileName = new CJzFileName( strFileName );
					if( pJzFileName )
					{
						m_lstFileNamesToOpen.AddTail( pJzFileName );
					}
					else
					{
						OpenTheFile( strFileName, TGT_SELECTEDNODE );
					}

					pIDocType->Release();
				}
				else
				{
					// Non-Producer file so copy it into the Project
					InsertForeignFile( strFileName );
				}
			}
		}

		POSITION posRemove;
		short nPriority = 1;

		while( m_lstFileNamesToOpen.IsEmpty() == FALSE )
		{
			pos = m_lstFileNamesToOpen.GetHeadPosition();
			while( pos )
			{
				posRemove = pos;
				pJzFileName = m_lstFileNamesToOpen.GetNext( pos );

				if( pJzFileName->m_nPriority == nPriority
				||  pJzFileName->m_nPriority < 1
				||  pJzFileName->m_nPriority > MAX_FILE_OPEN_PRIORITY )
				{
					if( pJzFileName->m_fBeingLoaded == false )
					{
						pJzFileName->m_fBeingLoaded = true;
						OpenTheFile( pJzFileName->m_strFileName, TGT_SELECTEDNODE );
					}

					m_lstFileNamesToOpen.RemoveAt( posRemove );
					delete pJzFileName;
				}
			}

			nPriority++;
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CJazzApp::OnFileOpenProject

void CJazzApp::OnFileOpenProject() 
{
	CString strFileName;

	// Prompt user for Project filename 
	if( DoPromptProjectOpen( strFileName ) )
	{
		if( IsValidProjectFile( strFileName, TRUE ) )
		{
			// Project files do not use targets (TGT_XXX)
			OpenDocumentFile( strFileName );
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CJazzApp::OnUpdateFileSaveProject

void CJazzApp::OnUpdateFileSaveProject(CCmdUI* pCmdUI) 
{
	CString strProjectName;
	CString strMenuText;

	strMenuText.LoadString( IDS_SAVE_PROJECT_MENU_TEXT );

	CProject* pProject = GetActiveProject();
	if( pProject )
	{
		BSTR bstrProjectName;

		if( SUCCEEDED ( pProject->GetNodeName ( &bstrProjectName ) ) )
		{
			strProjectName = bstrProjectName;
			::SysFreeString( bstrProjectName );
		}

		pProject->Release();
	}

	if( strProjectName.IsEmpty() )
	{
		pCmdUI->Enable( FALSE );
	}
	else
	{
		strMenuText += _T(" ");
		strMenuText += strProjectName;
		pCmdUI->Enable( TRUE );
	}

	pCmdUI->SetText( strMenuText );
}


/////////////////////////////////////////////////////////////////////////////
// CJazzApp::OnFileSaveProject

void CJazzApp::OnFileSaveProject() 
{
	CProject* pProject = GetActiveProject();
	if( pProject )
	{
		ASSERT( pProject->m_pProjectDoc != NULL );

		if( pProject->m_pProjectDoc->DoFileSave() )
		{
			SaveAppState();
		}

		pProject->Release();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CJazzApp::OnUpdateFileDuplicateProject

void CJazzApp::OnUpdateFileDuplicateProject( CCmdUI* pCmdUI ) 
{
	CString strProjectName;
	CString strMenuText;

	CProject* pProject = GetActiveProject();
	if( pProject )
	{
		BSTR bstrProjectName;

		if( SUCCEEDED ( pProject->GetNodeName ( &bstrProjectName ) ) )
		{
			strProjectName = bstrProjectName;
			::SysFreeString( bstrProjectName );
		}

		pProject->Release();
	}

	if( strProjectName.IsEmpty() )
	{
		strMenuText.LoadString( IDS_DUPLICATE_PROJECT_MENU_TEXT2 );
		pCmdUI->Enable( FALSE );
	}
	else
	{
		AfxFormatString1( strMenuText, IDS_DUPLICATE_PROJECT_MENU_TEXT, strProjectName );
		pCmdUI->Enable( TRUE );
	}

	pCmdUI->SetText( strMenuText );
}


/////////////////////////////////////////////////////////////////////////////
// CJazzApp::GetNewGUIDForDuplicateFile

BOOL CJazzApp::GetNewGUIDForDuplicateFile( GUID guidExistingFile, GUID* pguidNewFile )
{
	ASSERT( pguidNewFile != NULL );

	CJzFileGUIDs* pJzFileGUIDs;

	POSITION pos = m_lstFileGUIDs.GetHeadPosition();
	while( pos )
	{
		pJzFileGUIDs = m_lstFileGUIDs.GetNext( pos );

		if( ::IsEqualGUID( guidExistingFile, pJzFileGUIDs->m_guidExistingFile ) )
		{
			*pguidNewFile = pJzFileGUIDs->m_guidNewFile;
			return TRUE;
		}
	}

	return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CJazzApp::GetNewGUIDForDuplicateFile

BOOL CJazzApp::GetNewGUIDForDuplicateFile( LPCTSTR pszRelativePathName, GUID* pguidNewFile )
{
	ASSERT( pguidNewFile != NULL );

	CJzFileGUIDs* pJzFileGUIDs;

	POSITION pos = m_lstFileGUIDs.GetHeadPosition();
	while( pos )
	{
		pJzFileGUIDs = m_lstFileGUIDs.GetNext( pos );

		if( pJzFileGUIDs->m_strRelativePathName.Compare(pszRelativePathName) == 0 )
		{
			*pguidNewFile = pJzFileGUIDs->m_guidNewFile;
			return TRUE;
		}
	}

	return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CJazzApp::OnFileDuplicateProject

void CJazzApp::OnFileDuplicateProject() 
{
	CJazzDocTemplate* pDocTemplate = theApp.FindProjectDocTemplate();
	if( pDocTemplate )
	{
		CJzFileGUIDs* pJzFileGUIDs;
		CString strRelativePathName;

		// Create list with new GUIDs for all of the files in active Project
		CProject* pActiveProject = GetActiveProject();
		if( pActiveProject )
		{
			CFileNode* pFileNode;

			POSITION pos = pActiveProject->m_lstFiles.GetHeadPosition();
			while( pos )
			{
				pFileNode = pActiveProject->m_lstFiles.GetNext( pos );

				pFileNode->ConstructRelativePath( strRelativePathName );
				if( !strRelativePathName.IsEmpty() )
				{
					if( strRelativePathName.Right(1) != _T("\\") )
					{
						strRelativePathName += _T("\\");
					}
				}
				strRelativePathName += pFileNode->m_strName;

				pJzFileGUIDs = new CJzFileGUIDs( strRelativePathName, pFileNode->m_guid );
				if( pJzFileGUIDs )
				{
					m_lstFileGUIDs.AddTail( pJzFileGUIDs );
				}
			}

			pActiveProject->Release();
		}

		// Create the new Project
		m_fInDuplicateProject = TRUE;
		pDocTemplate->OpenDocumentFile( NULL );
		m_fInDuplicateProject = FALSE;

		// Free list of new GUIDs
		while( !m_lstFileGUIDs.IsEmpty() )
		{
			pJzFileGUIDs = static_cast<CJzFileGUIDs*>( m_lstFileGUIDs.RemoveHead() );
			delete pJzFileGUIDs;
		}
	}

	// Save the app state
	SaveAppState();
}


/////////////////////////////////////////////////////////////////////////////
// CJazzApp::OnUpdateFileCloseProject

void CJazzApp::OnUpdateFileCloseProject( CCmdUI* pCmdUI ) 
{
	CString strProjectName;
	CString strMenuText;

	strMenuText.LoadString( IDS_CLOSE_PROJECT_MENU_TEXT );

	CProject* pProject = GetActiveProject();
	if( pProject )
	{
		BSTR bstrProjectName;

		if( SUCCEEDED ( pProject->GetNodeName ( &bstrProjectName ) ) )
		{
			strProjectName = bstrProjectName;
			::SysFreeString( bstrProjectName );
		}

		pProject->Release();
	}

	if( strProjectName.IsEmpty() )
	{
		pCmdUI->Enable( FALSE );
	}
	else
	{
		strMenuText += _T(" ");
		strMenuText += strProjectName;
		pCmdUI->Enable( TRUE );
	}

	pCmdUI->SetText( strMenuText );
}


/////////////////////////////////////////////////////////////////////////////
// CJazzApp::OnFileCloseProject

void CJazzApp::OnFileCloseProject() 
{
	CProject* pProject = GetActiveProject();
	if( pProject )
	{
		ASSERT( pProject->m_pProjectDoc != NULL );

		m_fCloseProject = TRUE;
		if( pProject->CanClose() )
		{
			if( pProject->m_pProjectDoc->SaveAndCloseDoc() )
			{
				SaveAppState();
			}
		}
		m_fCloseProject = FALSE;

		pProject->Release();

		// Project gone so clean up bookmarks
		CleanUpBookmarks();

		// Make sure property sheet does NOT have focus
		m_pMainWnd->SetFocus();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CJazzApp::SaveAllModified

BOOL CJazzApp::SaveAllModified() 
{
	CJazzDocTemplate* pTemplate = FindProjectDocTemplate();
	if( pTemplate )
	{
		if( !pTemplate->SaveAllModified() )
		{
			return FALSE;
		}
	}

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CJazzApp::AddToRecentFileList

void CJazzApp::AddToRecentFileList( LPCTSTR szFileName )
{
	ASSERT_VALID( this );
	ASSERT( szFileName != NULL );
	ASSERT( AfxIsValidString( szFileName ) );

	TCHAR achExt[_MAX_EXT];

	_tsplitpath( szFileName, NULL, NULL, NULL, achExt );

	// Does file extension match Project file extension?
	CJazzDocTemplate* pTemplate = FindProjectDocTemplate();
	if( pTemplate )
	{
		CString strFilterExt;

		if( pTemplate->GetDocString(strFilterExt, CDocTemplate::filterExt)
		&&  !strFilterExt.IsEmpty() )
		{
			if( _tcsicmp( strFilterExt, achExt ) == 0 )
			{
				if( m_pRecentFileList != NULL )
				{
					// Fully qualify the path name
					TCHAR szTemp[_MAX_PATH];
					AfxFullPath( szTemp, szFileName );

					// Then add to recent file list
					m_pRecentFileList->Add( szTemp );
				}
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CJazzApp::OpenDocumentFile

CDocument* CJazzApp::OpenDocumentFile( LPCTSTR szFileName ) 
{
// Had to override CDocManager::OpenDocumentFile() to change the
// way MFC handles opening files that are already open!!!

// Functionality was not changed even though code had to be slightly
// modified so that it would run from within CJazzApp instead of
// CDocManager

	// no doc manager - no templates
	if( m_pDocManager == NULL )
	{
		AfxMessageBox( AFX_IDP_FAILED_TO_OPEN_DOC );
		return NULL;
	}

// Make sure the DirectMusic Producer application window is on top
	CMainFrame* pMainFrame = (CMainFrame *)m_pMainWnd;
	if( pMainFrame )
	{
		pMainFrame->SetForegroundWindow();
	}


// START of code copied from CDocManager::OpenDocumentFile()
	POSITION pos = m_pDocManager->GetFirstDocTemplatePosition();

	CDocTemplate::Confidence bestMatch = CDocTemplate::noAttempt;
	CDocTemplate* pBestTemplate = NULL;
	CDocument* pOpenDocument = NULL;

	TCHAR achPath[_MAX_PATH];
	ASSERT(lstrlen(szFileName) < _countof(achPath));

	TCHAR achTemp[_MAX_PATH];
	if( szFileName[0] == '\"' )
	{
		++szFileName;
	}

	lstrcpyn( achTemp, szFileName, _MAX_PATH );
	LPTSTR szLast = _tcsrchr( achTemp, '\"' );
	if( szLast != NULL )
	{
		*szLast = 0;
	}

	AfxFullPath( achPath, achTemp );
	TCHAR szLinkName[_MAX_PATH];
	if( AfxResolveShortcut(AfxGetMainWnd(), achPath, szLinkName, _MAX_PATH) )
	{
		lstrcpy( achPath, szLinkName );
	}

	while( pos != NULL )
	{
		CDocTemplate* pTemplate = m_pDocManager->GetNextDocTemplate( pos );
		ASSERT_VALID( pTemplate );
		ASSERT_KINDOF( CDocTemplate, pTemplate );

		CDocTemplate::Confidence match;
		ASSERT( pOpenDocument == NULL );

		match = pTemplate->MatchDocType( achPath, pOpenDocument );

		if( match > bestMatch )
		{ 
			bestMatch = match;
			pBestTemplate = pTemplate;
		}

		if( match == CDocTemplate::yesAlreadyOpen )
		{
			break;      // stop here
		}
	}
// END of code copied from CDocManager::OpenDocumentFile()

// START of changed behavior
// We will not display one of the document's views....
// we will simply return if the document is already open!!!
	if( pOpenDocument != NULL )
	{
		CString strTargetPath = achPath;

		if( pOpenDocument->IsKindOf( RUNTIME_CLASS(CComponentDoc) ) )
		{
			// Determine target path
			CDirectoryNode* pParentDirNode; 
				
			pParentDirNode = (CDirectoryNode *)m_pFramework->DetermineParentNode( achPath );
			if( pParentDirNode )
			{
				CString strOrigFileName;
				CString strNewFileName;

				// Split out name of file
				SplitOutFileName( achPath, TRUE, strOrigFileName );

				// Make sure it is the design-time name
				AdjustFileName( FT_DESIGN, strOrigFileName, strNewFileName );

				// Get new directory
				pParentDirNode->ConstructPath( strTargetPath );

				// Append name of file to directory
				strTargetPath += strNewFileName;
			}
		}

		// Simply return if document is already open and ( strTargetPath == achPath )
		if( _tcsicmp( achPath, strTargetPath ) == 0 )
		{
			if( ((CComponentDoc *)pOpenDocument)->m_pFileNode ) 
			{
				m_pFramework->ShowTreeNode( ((CComponentDoc *)pOpenDocument)->m_pFileNode );
			}

			return pOpenDocument;
		}
	}
// END of changed behavior

// START of code copied from CDocManager::OpenDocumentFile()
	if( pBestTemplate == NULL )
	{
		AfxMessageBox( AFX_IDP_FAILED_TO_OPEN_DOC );
		return NULL;
	}

	CDocument* pDocument = pBestTemplate->OpenDocumentFile( achPath );
// END of code copied from CDocManager::OpenDocumentFile()

// START of changed behavior
	// Make sure a copy of the file is placed in the project
	if( pDocument
	&&  pDocument->IsKindOf( RUNTIME_CLASS(CComponentDoc) ) )
	{
		CComponentDoc* pDoc = (CComponentDoc *)pDocument;
		CString strDocFileName = pDoc->GetPathName();
		
		CString strDesignFileName;
		CString strOrigFileName;
		CString strRuntimeFileName;

		ASSERT( pDoc->m_pFileNode != NULL );
		ASSERT( pDoc->m_pFileNode->m_pProject != NULL );
		pDoc->m_pFileNode->ConstructFileName( strOrigFileName );

		AdjustFileName( FT_DESIGN, strOrigFileName, strDesignFileName );
		AdjustFileName( FT_RUNTIME, strDocFileName, strRuntimeFileName );

		// Did we just open a "DirectMusic" file?
		IDMUSProdDocType* pIDocType = GetDocType( strDocFileName );
		if( pIDocType )
		{
			// Was it a "runtime" file?
			if( strRuntimeFileName.CompareNoCase( strDocFileName ) == 0 )
			{
				// Was the "runtime" file located within the Project?
				if( strDocFileName.Find( pDoc->m_pFileNode->m_pProject->m_strProjectDir ) == 0 )
				{
					// Make sure the design filename is unique
					pDoc->m_pFileNode->m_pProject->GetUniqueFileName( strDesignFileName );

					// Sync the runtime filename with the design filename
					CString strDesignFName;
					CString strRuntimeFName;
					theApp.SplitOutFileName( strDesignFileName, TRUE, strDesignFName );
					AdjustFileName( FT_RUNTIME, strDesignFName, strRuntimeFName );
					int nFindPos = strRuntimeFileName.ReverseFind( (TCHAR)'\\' );
					if( nFindPos != -1 )
					{
						strRuntimeFileName = strRuntimeFileName.Left( nFindPos + 1 );
						strRuntimeFileName += strRuntimeFName;
					}

					// If the sync caused the runtime filename to change, rename the runtime flie
					if( strRuntimeFileName.CompareNoCase( strDocFileName ) != 0 )
					{
						if( RenameThePath( strDocFileName, strRuntimeFileName, strDocFileName ) == FALSE )
						{
							strRuntimeFileName = strDocFileName;
						}
					}

					// Link the design file to the runtime file we just opened
					pDoc->m_pFileNode->LinkToRuntimeFile( strRuntimeFileName );
				}
			}

			pIDocType->Release();
		}

		// May have to save a design file in the Project
		if( strDesignFileName.CompareNoCase( strDocFileName ) != 0 )
		{
			short nOrigRuntimeSave = m_nRuntimeSave;
			m_nRuntimeSave = DOC_NOT_RUNTIME_SAVE;

			if( pDoc->DoSave(strDesignFileName) == FALSE )
			{
				// Delete node
				m_fDeleteFromTree = TRUE;
				pDoc->m_pFileNode->DeleteNode( FALSE );
				m_fDeleteFromTree = FALSE;

				pDocument = NULL;
			}

			m_nRuntimeSave = nOrigRuntimeSave;	
		}
	}
// END of changed behavior

	return pDocument;
}


/////////////////////////////////////////////////////////////////////////////
// CJazzApp::OnFileRuntimeSaveAllFiles

void CJazzApp::OnFileRuntimeSaveAllFiles() 
{
	// Runtime files are separate entities that exist outside
	// of the Project.
	// This method saves all files in the Project as separate
	// files in their most compressed format.
	// This method does not save the .pro Project file 

	CProject* pProject = GetActiveProject();
	if( pProject )
	{
		// Warn user of duplicate "Runtime" file targets
		BOOL fContinue = pProject->HandleRuntimeDuplicates();

		if( fContinue )
		{
			// Get number of files that are going to be saved
			int nNbrFiles = pProject->GetNbrFiles( GNF_NOFLAGS );

			// Start the "Runtime" save
			m_nRuntimeSave = DOC_RUNTIME_SAVE_ALL;

			// Start progress bar
			CString strPrompt;
			strPrompt.LoadString( IDS_SAVING_RUNTIME_FILES );
			BSTR bstrPrompt = strPrompt.AllocSysString();
			HANDLE hKey;
			if( SUCCEEDED ( m_pFramework->StartProgressBar( 0, nNbrFiles, bstrPrompt, &hKey ) ) )
			{
				theApp.m_hKeyProgressBar = hKey;
			}
			m_pFramework->SetProgressBarPos( theApp.m_hKeyProgressBar, 0 );
			m_pFramework->SetProgressBarStep( theApp.m_hKeyProgressBar, 1 );

			// Save all files
			pProject->RuntimeSaveAllFiles();

			// Remove progress bar
			m_pFramework->SetProgressBarPos( theApp.m_hKeyProgressBar, 100 );
			Sleep( 10 );
			m_pFramework->EndProgressBar( theApp.m_hKeyProgressBar );

			m_nRuntimeSave = DOC_NOT_RUNTIME_SAVE;
		}

		pProject->Release();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CJazzApp::OnUpdateFileRuntimeSaveAllFiles

void CJazzApp::OnUpdateFileRuntimeSaveAllFiles( CCmdUI* pCmdUI ) 
{
	CString strProjectName;
	CString strMenuText;

	strMenuText.LoadString( IDS_SAVE_RUNTIME_ALL_FILES_TEXT );

	int nNbrFiles = 0;

	CProject* pProject = GetActiveProject();
	if( pProject )
	{
		BSTR bstrProjectName;
		if( SUCCEEDED ( pProject->GetNodeName ( &bstrProjectName ) ) )
		{
			strProjectName = bstrProjectName;
			::SysFreeString( bstrProjectName );
		}

		nNbrFiles = pProject->GetNbrFiles( GNF_NOFLAGS );
		if( nNbrFiles > 0 )
		{
			strMenuText.LoadString( IDS_SAVE_RUNTIME_ALL_FILES_PROJECT_TEXT );
		}

		pProject->Release();
	}

	if( nNbrFiles == 0 )
	{
		pCmdUI->Enable( FALSE );
	}
	else
	{
		strMenuText += _T(" ");
		strMenuText += strProjectName;
		pCmdUI->Enable( TRUE );
	}

	pCmdUI->SetText( strMenuText );
}


/////////////////////////////////////////////////////////////////////////////
// CJazzApp::OnUpdateBookmarkCreate

void CJazzApp::OnUpdateBookmarkCreate( CCmdUI* pCmdUI ) 
{
	// Only enable if an editor window is open
	CDocTemplate* pTemplate;
	CComponentDoc* pComponentDoc;
	CComponentView* pComponentView;

	if( m_pDocManager )
	{
		// walk all templates in the application
		POSITION pos = m_pDocManager->GetFirstDocTemplatePosition();
		while( pos != NULL )
		{
			pTemplate = m_pDocManager->GetNextDocTemplate( pos );
			ASSERT_VALID( pTemplate );
			ASSERT_KINDOF( CDocTemplate, pTemplate );

			// walk all documents in the template
			POSITION pos2 = pTemplate->GetFirstDocPosition();
			while( pos2 )
			{
				pComponentDoc = (CComponentDoc *)pTemplate->GetNextDoc( pos2 );
				ASSERT_VALID( pComponentDoc );

				if( pComponentDoc->IsKindOf( RUNTIME_CLASS(CComponentDoc) ) )
				{
					POSITION pos3 = pComponentDoc->GetFirstViewPosition();
					while( pos3 )
					{
						pComponentView = (CComponentView *)pComponentDoc->GetNextView( pos3 );
						ASSERT_VALID( pComponentView );

						if( pComponentView
						&&  pComponentView->IsKindOf( RUNTIME_CLASS(CComponentView) ) )
						{
							if( pComponentView->m_fSeed == FALSE )
							{
								pCmdUI->Enable( TRUE );
								return;
							}
						}
					}
				}
			}
		}
	}

	pCmdUI->Enable( FALSE );
}


/////////////////////////////////////////////////////////////////////////////
// CJazzApp::OnBookmarkCreate

void CJazzApp::OnBookmarkCreate() 
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	CMainFrame* pMainFrame = (CMainFrame *)m_pMainWnd;
	ASSERT( pMainFrame != NULL );

	// Put up 'New Bookmark' dialog
	CNewBookmarkDlg dlgNewBookmark;
	if( dlgNewBookmark.DoModal() == IDCANCEL )
	{	
		return;
	}

	// Create the Bookmark
	CBookmark* pBookmark = new CBookmark;
	if( pBookmark )
	{
		pBookmark->m_strName = dlgNewBookmark.m_strName;
		pBookmark->Create();

		if( pMainFrame->m_wndBookmarkToolBar.AddBookmark( pBookmark, TRUE ) == FALSE )
		{
			delete pBookmark;
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CJazzApp::OnUpdateBookmarkRemoveAll

void CJazzApp::OnUpdateBookmarkRemoveAll( CCmdUI* pCmdUI ) 
{
	if( m_lstBookmarks.IsEmpty() )
	{
		pCmdUI->Enable( FALSE );
	}
	else
	{
		pCmdUI->Enable( TRUE );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CJazzApp::OnBookmarkRemoveAll

void CJazzApp::OnBookmarkRemoveAll() 
{
	CMainFrame* pMainFrame = (CMainFrame *)m_pMainWnd;
	ASSERT( pMainFrame != NULL );

	pMainFrame->m_wndBookmarkToolBar.RemoveAllBookmarks();
}


/////////////////////////////////////////////////////////////////////////////
// CJazzApp::OnUpdateBookmarkCombo

void CJazzApp::OnUpdateBookmarkCombo( CCmdUI* pCmdUI ) 
{
	if( m_lstBookmarks.IsEmpty() )
	{
		pCmdUI->Enable( FALSE );
	}
	else
	{
		pCmdUI->Enable( TRUE );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CJazzApp::OnUpdateBookmarkRemove

void CJazzApp::OnUpdateBookmarkRemove( CCmdUI* pCmdUI ) 
{
	CMainFrame* pMainFrame = (CMainFrame *)m_pMainWnd;

	if( pMainFrame
	&&  pMainFrame->m_wndBookmarkToolBar.IsBookmarkSelected() )
	{
		pCmdUI->Enable( TRUE );
	}
	else
	{
		pCmdUI->Enable( FALSE );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CJazzApp::OnBookmarkRemove

void CJazzApp::OnBookmarkRemove() 
{
	CMainFrame* pMainFrame = (CMainFrame *)m_pMainWnd;
	ASSERT( pMainFrame != NULL );

	pMainFrame->m_wndBookmarkToolBar.OnBookmarkRemove();
}


/////////////////////////////////////////////////////////////////////////////
// CJazzApp::CleanUpBookmarks

void CJazzApp::CleanUpBookmarks( void ) 
{
	CMainFrame* pMainFrame = (CMainFrame *)m_pMainWnd;
	ASSERT( pMainFrame != NULL );

	CBookmark* pBookmark;

    POSITION pos = m_lstBookmarks.GetHeadPosition();
    while( pos )
    {
        pBookmark = m_lstBookmarks.GetNext( pos );

		if( pBookmark->IsValid() == FALSE )
		{
			pMainFrame->m_wndBookmarkToolBar.RemoveBookmark( pBookmark );
		}
    }
}


/////////////////////////////////////////////////////////////////////////////
// CJazzApp::OnCmdMsg

BOOL CJazzApp::OnCmdMsg( UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo ) 
{
	switch( nID )
	{
		case IDC_BOOKMARK_COMBO:
		{
			switch( nCode )
			{
				case CBN_SELCHANGE:
				{
					CMainFrame* pMainFrame = (CMainFrame *)m_pMainWnd;
					ASSERT( pMainFrame != NULL );

					pMainFrame->m_wndBookmarkToolBar.ApplySelectedBookmark();
					return TRUE;
				}
			}
		}
	}
	
	return CWinApp::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}


/////////////////////////////////////////////////////////////////////////////
// CJazzApp::OnUpdateImportMid

void CJazzApp::OnUpdateImportMid( CCmdUI* pCmdUI ) 
{ 
	BOOL fEnable = FALSE;

	if( !m_lstProjects.IsEmpty() )
	{
		if( m_pFramework )
		{
			IDMUSProdComponent* pIComponent;

			if( SUCCEEDED ( m_pFramework->FindComponent( CLSID_SegmentComponent, &pIComponent ) ) )
			{
				fEnable = TRUE;
				pIComponent->Release();
			}
		}
	}

	pCmdUI->Enable( fEnable );
}


/////////////////////////////////////////////////////////////////////////////
// CJazzApp::OnImportMid

void CJazzApp::OnImportMid() 
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	CString strFileNames;
	CString strFileName;
	CString strPrompt;
	CString	strMIDIFileExt;
	CString	strMIDIFilterExt;
	CString strFilter;
	CString strAllFilter;
	CString strDefaultDir;

	VERIFY( strPrompt.LoadString( IDS_IMPORT_MIDIFILE ) );
	VERIFY( strMIDIFileExt.LoadString( IDS_MIDI_FILE_EXT ) );
	VERIFY( strMIDIFilterExt.LoadString( IDS_MIDI_FILTER_EXT ) );

	// Contruct *.mid filter
	VERIFY( strFilter.LoadString( IDS_MIDI_FILEDESC ) );
	strFilter += (TCHAR)'\0';
	strFilter += strMIDIFilterExt;
	strFilter += (TCHAR)'\0';
	
	// Append *.* filter
	VERIFY( strAllFilter.LoadString( AFX_IDS_ALLFILTER ) );
	strFilter += strAllFilter;
	strFilter += (TCHAR)'\0';
	strFilter += _T("*.*");
	strFilter += (TCHAR)'\0';

	// Get the default directory for this extension
	GetDefaultDirForFileSave( strMIDIFileExt, strDefaultDir );

	// Prepare File open dialog
	CFileDialog dlg( TRUE );
	
	dlg.m_ofn.Flags |= ( OFN_HIDEREADONLY | OFN_FILEMUSTEXIST | OFN_ALLOWMULTISELECT );
	dlg.m_ofn.lpstrTitle = strPrompt;
	dlg.m_ofn.lpstrFilter = strFilter;
	dlg.m_ofn.lpstrDefExt = strMIDIFileExt;
	dlg.m_ofn.nFilterIndex = 0;
	dlg.m_ofn.nMaxCustFilter = 2;	// 1 based number
	dlg.m_ofn.lpstrFile = strFileNames.GetBuffer( 2048 );
	dlg.m_ofn.nMaxFile = 2048;		// Max filename buffer size for NT 4.0
	if( strDefaultDir.IsEmpty() == FALSE )
	{
		dlg.m_ofn.lpstrInitialDir = strDefaultDir;
	}

	// Display File open dialog
	int nAnswer = dlg.DoModal();
	strFileNames.ReleaseBuffer();

	// Process results of File Open dialog
	HRESULT hr = E_FAIL;

	if( nAnswer == IDOK )
	{
		IDMUSProdDocType* pIDocType;
		IDMUSProdNode* pIDocRootNode;

		POSITION pos = dlg.GetStartPosition();
		while( pos )
		{
			strFileName = dlg.GetNextPathName( pos );
			
			SetDefaultDirForFileSave( strFileName, strMIDIFileExt );

			if( SUCCEEDED ( m_pFramework->FindDocTypeByNodeId( GUID_SegmentNode, &pIDocType ) ) )
			{
				m_nFileTarget = TGT_SELECTEDNODE;
				IDMUSProdNode* pIDirNode = m_pFramework->DetermineParentNode( NULL );
				m_nFileTarget = TGT_PROJECT;

				m_strImportFileName = strFileName;
				if( SUCCEEDED ( m_pFramework->CreateNewFile( pIDocType, pIDirNode, &pIDocRootNode ) ) )
				{
					hr = S_OK;
					pIDocRootNode->Release();
				}
				m_strImportFileName.Empty();

				pIDocType->Release();
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CJazzApp::OnUpdateImportWav

void CJazzApp::OnUpdateImportWav( CCmdUI* pCmdUI ) 
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	BOOL fEnable = FALSE;

	if( !m_lstProjects.IsEmpty() )
	{
		if( m_pFramework )
		{
			IDMUSProdComponent* pIComponent;

			if( SUCCEEDED ( m_pFramework->FindComponent( CLSID_SegmentComponent, &pIComponent ) ) )
			{
				pIComponent->Release();

				if( SUCCEEDED ( m_pFramework->FindComponent( CLSID_DLSComponent, &pIComponent ) ) )
				{
					fEnable = TRUE;
					pIComponent->Release();
				}
			}
		}
	}

	pCmdUI->Enable( fEnable );
}


/////////////////////////////////////////////////////////////////////////////
// CJazzApp::OnImportWav

void CJazzApp::OnImportWav( void ) 
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	CString strFileNames;
	CString strFileName;
	CString strPrompt;
	CString	strWaveFileExt;
	CString	strWaveFilterExt;
	CString strFilter;
	CString strAllFilter;
	CString strDefaultDir;

	VERIFY( strPrompt.LoadString( IDS_IMPORT_WAVEFILE ) );
	VERIFY( strWaveFileExt.LoadString( IDS_WAVE_FILE_EXT ) );
	VERIFY( strWaveFilterExt.LoadString( IDS_WAVE_FILTER_EXT ) );

	// Contruct *.wav filter
	VERIFY( strFilter.LoadString( IDS_WAVE_FILEDESC ) );
	strFilter += (TCHAR)'\0';
	strFilter += strWaveFilterExt;
	strFilter += (TCHAR)'\0';
	
	// Append *.* filter
	VERIFY( strAllFilter.LoadString( AFX_IDS_ALLFILTER ) );
	strFilter += strAllFilter;
	strFilter += (TCHAR)'\0';
	strFilter += _T("*.*");
	strFilter += (TCHAR)'\0';

	// Get the default directory for this extension
	GetDefaultDirForFileSave( strWaveFileExt, strDefaultDir );

	// Prepare File open dialog
	CFileDialog dlg( TRUE );
	
	dlg.m_ofn.Flags |= ( OFN_HIDEREADONLY | OFN_FILEMUSTEXIST | OFN_ALLOWMULTISELECT );
	dlg.m_ofn.lpstrTitle = strPrompt;
	dlg.m_ofn.lpstrFilter = strFilter;
	dlg.m_ofn.lpstrDefExt = strWaveFileExt;
	dlg.m_ofn.nFilterIndex = 0;
	dlg.m_ofn.nMaxCustFilter = 2;	// 1 based number
	dlg.m_ofn.lpstrFile = strFileNames.GetBuffer( 2048 );
	dlg.m_ofn.nMaxFile = 2048;		// Max filename buffer size for NT 4.0
	if( strDefaultDir.IsEmpty() == FALSE )
	{
		dlg.m_ofn.lpstrInitialDir = strDefaultDir;
	}

	// Display File open dialog
	int nAnswer = dlg.DoModal();
	strFileNames.ReleaseBuffer();

	// Process results of File Open dialog
	if( nAnswer == IDOK )
	{
		CWaitCursor wait;
		IDMUSProdDocType* pIWaveDocType;
		IDMUSProdNode* pIWaveDocRootNode;

		POSITION pos = dlg.GetStartPosition();
		while( pos )
		{
			strFileName = dlg.GetNextPathName( pos );
			
			SetDefaultDirForFileSave( strFileName, strWaveFileExt );

			if( SUCCEEDED ( m_pFramework->FindDocTypeByNodeId( GUID_WaveNode, &pIWaveDocType ) ) )
			{
				m_nFileTarget = TGT_SELECTEDNODE;
				IDMUSProdNode* pIDirNode = m_pFramework->DetermineParentNode( NULL );
				m_nFileTarget = TGT_PROJECT;

				m_fOpenEditorWindow = FALSE;
				m_strImportFileName = strFileName;

				// Try and file the wave file in an already open project
				pIWaveDocRootNode = theApp.FindFileNode( strFileName );

				// If we found a wave file node
				if( pIWaveDocRootNode )
				{
					// Get its parent
					IDMUSProdNode *pTmpParent = NULL;
					if( SUCCEEDED( pIWaveDocRootNode->GetParentNode( &pTmpParent ) ) )
					{
						// If its parent is not the one we're inserting into
						if( pTmpParent != pIDirNode )
						{
							// Set pIWaveDocRootNode to NULL, so we create a new wave file
							pIWaveDocRootNode = NULL;
						}
						else
						{
							// Otherwise, get the docroot node from the file node
							GUID guidFile;
							m_pFramework->GetNodeFileGUID( pIWaveDocRootNode, &guidFile );
							pIWaveDocRootNode = NULL;
							m_pFramework->FindDocRootNodeByFileGUID( guidFile, &pIWaveDocRootNode );
						}
					}
					else
					{
						pIWaveDocRootNode = NULL;
					}
					if( pTmpParent )
					{
						pTmpParent->Release();
					}
				}

				if( pIWaveDocRootNode
				||	SUCCEEDED ( m_pFramework->CreateNewFile( pIWaveDocType, pIDirNode, &pIWaveDocRootNode ) ) )
				{
					IDMUSProdDocType* pISegmentDocType;
					if( SUCCEEDED ( m_pFramework->FindDocTypeByNodeId( GUID_SegmentNode, &pISegmentDocType ) ) )
					{
						IDMUSProdDocTypeP* pISegmentDocTypeP;
						if( SUCCEEDED ( pISegmentDocType->QueryInterface( IID_IDMUSProdDocTypeP, (void**)&pISegmentDocTypeP ) ) )
						{
							IUnknown* punkISegmentDocRootNode;
							if( SUCCEEDED ( pISegmentDocTypeP->ImportNode( pIWaveDocRootNode, pIDirNode, &punkISegmentDocRootNode ) ) )
							{
								IDMUSProdNode* pISegmentDocRootNode;
								if( SUCCEEDED ( punkISegmentDocRootNode->QueryInterface( IID_IDMUSProdNode, (void**)&pISegmentDocRootNode ) ) )
								{
									// Open editor for the newly created segment file
									theApp.m_pFramework->OpenEditor( pISegmentDocRootNode );
	
									pISegmentDocRootNode->Release();
								}

								punkISegmentDocRootNode->Release();
							}

							pISegmentDocTypeP->Release();
						}

						pISegmentDocType->Release();
					}

					pIWaveDocRootNode->Release();
				}
				m_strImportFileName.Empty();
				m_fOpenEditorWindow = TRUE;

				pIWaveDocType->Release();
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CJazzApp::OnUpdateImportWavVariations

void CJazzApp::OnUpdateImportWavVariations( CCmdUI* pCmdUI ) 
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	BOOL fEnable = FALSE;

	if( !m_lstProjects.IsEmpty() )
	{
		if( m_pFramework )
		{
			IDMUSProdComponent* pIComponent;

			if( SUCCEEDED ( m_pFramework->FindComponent( CLSID_SegmentComponent, &pIComponent ) ) )
			{
				pIComponent->Release();

				if( SUCCEEDED ( m_pFramework->FindComponent( CLSID_DLSComponent, &pIComponent ) ) )
				{
					fEnable = TRUE;
					pIComponent->Release();
				}
			}
		}
	}

	pCmdUI->Enable( fEnable );
}


/////////////////////////////////////////////////////////////////////////////
// CJazzApp::OnImportWavVariations

void CJazzApp::OnImportWavVariations( void ) 
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	CString strFileNames;
	CString strFileName;
	CString strPrompt;
	CString	strWaveFileExt;
	CString	strWaveFilterExt;
	CString strFilter;
	CString strAllFilter;
	CString strDefaultDir;

	VERIFY( strPrompt.LoadString( IDS_IMPORT_WAVEFILE_VARIATIONS ) );
	VERIFY( strWaveFileExt.LoadString( IDS_WAVE_FILE_EXT ) );
	VERIFY( strWaveFilterExt.LoadString( IDS_WAVE_FILTER_EXT ) );

	// Contruct *.wav filter
	VERIFY( strFilter.LoadString( IDS_WAVE_FILEDESC ) );
	strFilter += (TCHAR)'\0';
	strFilter += strWaveFilterExt;
	strFilter += (TCHAR)'\0';
	
	// Append *.* filter
	VERIFY( strAllFilter.LoadString( AFX_IDS_ALLFILTER ) );
	strFilter += strAllFilter;
	strFilter += (TCHAR)'\0';
	strFilter += _T("*.*");
	strFilter += (TCHAR)'\0';

	// Get the default directory for this extension
	GetDefaultDirForFileSave( strWaveFileExt, strDefaultDir );

	// Prepare File open dialog
	CFileDialog dlg( TRUE );
	
	dlg.m_ofn.Flags |= ( OFN_HIDEREADONLY | OFN_FILEMUSTEXIST | OFN_ALLOWMULTISELECT );
	dlg.m_ofn.lpstrTitle = strPrompt;
	dlg.m_ofn.lpstrFilter = strFilter;
	dlg.m_ofn.lpstrDefExt = strWaveFileExt;
	dlg.m_ofn.nFilterIndex = 0;
	dlg.m_ofn.nMaxCustFilter = 2;	// 1 based number
	dlg.m_ofn.lpstrFile = strFileNames.GetBuffer( 2048 );
	dlg.m_ofn.nMaxFile = 2048;		// Max filename buffer size for NT 4.0
	if( strDefaultDir.IsEmpty() == FALSE )
	{
		dlg.m_ofn.lpstrInitialDir = strDefaultDir;
	}

	// Display File open dialog
	int nAnswer = dlg.DoModal();
	strFileNames.ReleaseBuffer();

	// Process results of File Open dialog
	if( nAnswer == IDOK )
	{
		CWaitCursor wait;
		IDMUSProdDocType* pIWaveDocType;
		IDMUSProdNode* pIWaveDocRootNode;

		// List of all wave nodes opened/found
		CTypedPtrList<CPtrList, IDMUSProdNode *> lstWaveDocRootNode;

		POSITION pos = dlg.GetStartPosition();
		while( pos )
		{
			strFileName = dlg.GetNextPathName( pos );
			
			SetDefaultDirForFileSave( strFileName, strWaveFileExt );

			if( SUCCEEDED ( m_pFramework->FindDocTypeByNodeId( GUID_WaveNode, &pIWaveDocType ) ) )
			{
				m_nFileTarget = TGT_SELECTEDNODE;
				IDMUSProdNode* pIDirNode = m_pFramework->DetermineParentNode( NULL );
				m_nFileTarget = TGT_PROJECT;

				m_fOpenEditorWindow = FALSE;
				m_strImportFileName = strFileName;

				// Try and find the wave file in an already open project
				pIWaveDocRootNode = theApp.FindFileNode( strFileName );

				// If we found a wave file node
				if( pIWaveDocRootNode )
				{
					// Get its parent
					IDMUSProdNode *pTmpParent = NULL;
					if( SUCCEEDED( pIWaveDocRootNode->GetParentNode( &pTmpParent ) ) )
					{
						// If its parent is not the one we're inserting into
						if( pTmpParent != pIDirNode )
						{
							// Set pIWaveDocRootNode to NULL, so we create a new wave file
							pIWaveDocRootNode = NULL;
						}
						else
						{
							// Otherwise, get the docroot node from the file node
							GUID guidFile;
							m_pFramework->GetNodeFileGUID( pIWaveDocRootNode, &guidFile );
							pIWaveDocRootNode = NULL;
							m_pFramework->FindDocRootNodeByFileGUID( guidFile, &pIWaveDocRootNode );
						}
					}
					else
					{
						pIWaveDocRootNode = NULL;
					}
					if( pTmpParent )
					{
						pTmpParent->Release();
					}
				}

				if( pIWaveDocRootNode
				||	SUCCEEDED ( m_pFramework->CreateNewFile( pIWaveDocType, pIDirNode, &pIWaveDocRootNode ) ) )
				{
					lstWaveDocRootNode.AddTail( pIWaveDocRootNode );
				}
				m_strImportFileName.Empty();
				m_fOpenEditorWindow = TRUE;

				pIWaveDocType->Release();
			}
		}

		IDMUSProdDocType* pISegmentDocType;
		if( !lstWaveDocRootNode.IsEmpty()
		&&	SUCCEEDED ( m_pFramework->FindDocTypeByNodeId( GUID_SegmentNode, &pISegmentDocType ) ) )
		{
			ISegmentDocTypeWaveVarImport* pISegmentDocTypeWaveVarImport;
			if( SUCCEEDED ( pISegmentDocType->QueryInterface( IID_ISegmentDocTypeWaveVarImport, (void**)&pISegmentDocTypeWaveVarImport ) ) )
			{
				IDMUSProdNode **apIWavesToImport = new IDMUSProdNode *[lstWaveDocRootNode.GetCount()];
				if( apIWavesToImport)
				{
					int nIndex = 0;
					pos = lstWaveDocRootNode.GetHeadPosition();
					while( pos )
					{
						apIWavesToImport[nIndex++] = lstWaveDocRootNode.GetNext(pos);
					}

					// Sort the array of wave nodes
					bool fDone = false;
					while( !fDone )
					{
						fDone = true;
						for( int nIndex = 0; nIndex < lstWaveDocRootNode.GetCount() - 1; nIndex++ )
						{
							BSTR bstrName1 = NULL;
							BSTR bstrName2 = NULL;
							apIWavesToImport[nIndex]->GetNodeName( &bstrName1);
							apIWavesToImport[nIndex + 1]->GetNodeName( &bstrName2);

							CString strName1 = bstrName1;
							CString strName2 = bstrName2;
							strName1.MakeUpper();
							strName2.MakeUpper();

							if( strName1.Compare( strName2 ) > 0 )
							{
								fDone = false;
								IDMUSProdNode *pNodeTmp = apIWavesToImport[nIndex];
								apIWavesToImport[nIndex] = apIWavesToImport[nIndex + 1];
								apIWavesToImport[nIndex + 1] = pNodeTmp;
							}

							if( bstrName1 )
							{
								::SysFreeString( bstrName1 );
							}
							if( bstrName2 )
							{
								::SysFreeString( bstrName2 );
							}
						}
					}

					m_nFileTarget = TGT_SELECTEDNODE;
					IDMUSProdNode* pIDirNode = m_pFramework->DetermineParentNode( NULL );
					m_nFileTarget = TGT_PROJECT;

					IUnknown* punkISegmentDocRootNode;
					if( SUCCEEDED ( pISegmentDocTypeWaveVarImport->ImportWaveNodes( lstWaveDocRootNode.GetCount(), apIWavesToImport, pIDirNode, &punkISegmentDocRootNode ) ) )
					{
						IDMUSProdNode* pISegmentDocRootNode;
						if( SUCCEEDED ( punkISegmentDocRootNode->QueryInterface( IID_IDMUSProdNode, (void**)&pISegmentDocRootNode ) ) )
						{
							// Open editor for the newly created segment file
							theApp.m_pFramework->OpenEditor( pISegmentDocRootNode );

							pISegmentDocRootNode->Release();
						}

						punkISegmentDocRootNode->Release();
					}

					delete []apIWavesToImport;
				}

				pISegmentDocTypeWaveVarImport->Release();
			}

			pISegmentDocType->Release();
		}

		while( !lstWaveDocRootNode.IsEmpty() )
		{
			lstWaveDocRootNode.RemoveHead()->Release();
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CJazzApp::OnUpdateImportSec

void CJazzApp::OnUpdateImportSec( CCmdUI* pCmdUI ) 
{
	BOOL fEnable = FALSE;

	if( !m_lstProjects.IsEmpty() )
	{
		if( m_pFramework )
		{
			IDMUSProdComponent* pIComponent;

			if( SUCCEEDED ( m_pFramework->FindComponent( CLSID_SegmentComponent, &pIComponent ) ) )
			{
				fEnable = TRUE;
				pIComponent->Release();
			}
		}
	}

	pCmdUI->Enable( fEnable );
}


/////////////////////////////////////////////////////////////////////////////
// CJazzApp::OnImportSec

void CJazzApp::OnImportSec() 
{
	AFX_MANAGE_STATE( AfxGetAppModuleState() );

	CString strFileNames;
	CString strFileName;
	CString strPrompt;
	CString	strExt;
	CString strFilter;
	CString strAllFilter;
	CString strDefaultDir;

	VERIFY( strPrompt.LoadString( IDS_IMPORT_SECFILE ) );
	VERIFY( strExt.LoadString( IDS_SEC_FILEEXT ) );

	// Contruct *.sec filter
	VERIFY( strFilter.LoadString( IDS_SEC_FILEDESC ) );
	strFilter += (TCHAR)'\0';
	strFilter += strExt;
	strFilter += (TCHAR)'\0';
	
	// Append *.* filter
	VERIFY( strAllFilter.LoadString( AFX_IDS_ALLFILTER ) );
	strFilter += strAllFilter;
	strFilter += (TCHAR)'\0';
	strFilter += _T("*.*");
	strFilter += (TCHAR)'\0';

	// Get the default directory for this extension
	GetDefaultDirForFileSave( strExt, strDefaultDir );

	// Prepare File open dialog
	CFileDialog dlg( TRUE );
	
	dlg.m_ofn.Flags |= ( OFN_HIDEREADONLY | OFN_FILEMUSTEXIST | OFN_ALLOWMULTISELECT );
	dlg.m_ofn.lpstrTitle = strPrompt;
	dlg.m_ofn.lpstrFilter = strFilter;
	dlg.m_ofn.lpstrDefExt = strExt;
	dlg.m_ofn.nFilterIndex = 0;
	dlg.m_ofn.nMaxCustFilter = 2;	// 1 based number
	dlg.m_ofn.lpstrFile = strFileNames.GetBuffer( 2048 );
	dlg.m_ofn.nMaxFile = 2048;		// Max filename buffer size for NT 4.0
	if( strDefaultDir.IsEmpty() == FALSE )
	{
		dlg.m_ofn.lpstrInitialDir = strDefaultDir;
	}

	// Display File open dialog
	int nAnswer = dlg.DoModal();
	strFileNames.ReleaseBuffer();

	// Process results of File Open dialog
	HRESULT hr = E_FAIL;

	if( nAnswer == IDOK )
	{
		IDMUSProdDocType* pIDocType;
		IDMUSProdNode* pIDocRootNode;

		POSITION pos = dlg.GetStartPosition();
		while( pos )
		{
			strFileName = dlg.GetNextPathName( pos );

			SetDefaultDirForFileSave( strFileName, NULL );

			if( SUCCEEDED ( m_pFramework->FindDocTypeByNodeId( GUID_SegmentNode, &pIDocType ) ) )
			{
				m_nFileTarget = TGT_SELECTEDNODE;
				IDMUSProdNode* pIDirNode = m_pFramework->DetermineParentNode( NULL );
				m_nFileTarget = TGT_PROJECT;

				m_strImportFileName = strFileName;
				if( SUCCEEDED ( m_pFramework->CreateNewFile( pIDocType, pIDirNode, &pIDocRootNode ) ) )
				{
					hr = S_OK;
					pIDocRootNode->Release();
				}
				m_strImportFileName.Empty();

				pIDocType->Release();
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CJazzApp::GetHelpFileName

BOOL CJazzApp::GetHelpFileName( CString& strHelpFileName )
{
	TCHAR achHelpFileName[FILENAME_MAX];
	TCHAR achDrive[_MAX_DRIVE];
	TCHAR achDir[_MAX_DIR];
	TCHAR achFName[_MAX_FNAME];
	TCHAR achExt[_MAX_EXT];

	if( GetModuleFileName(NULL, achHelpFileName, FILENAME_MAX) > 0 )
	{
		_tsplitpath( achHelpFileName, achDrive, achDir, achFName, NULL );
		::LoadString( theApp.m_hInstance, IDS_HELP_FILE_EXT, achExt, _MAX_EXT );
		_tmakepath( achHelpFileName, achDrive, achDir, achFName, achExt );

		strHelpFileName = achHelpFileName;
		return TRUE;
	}

	return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CJazzApp DuplicateBookmarks

void CJazzApp::DuplicateBookmarks( CProject* pFromProject, CProject* pToProject )
{
	if( theApp.m_fInDuplicateProject == FALSE )
	{
		// Only call when we are duplicating a Project
		// because CBookmark::Duplicate depends on existence
		// of m_lstFileGUIDs.
		ASSERT( 0 );
		return;
	}

	// Search through all bookmarks
	POSITION pos = m_lstBookmarks.GetHeadPosition();
	while( pos )
	{
        CBookmark* pBookmark = m_lstBookmarks.GetNext( pos );

		if( pBookmark->IsForThisProject( pFromProject ) )
		{
			// Duplicate the Bookmark
			pBookmark->Duplicate();
		}
	}
}
