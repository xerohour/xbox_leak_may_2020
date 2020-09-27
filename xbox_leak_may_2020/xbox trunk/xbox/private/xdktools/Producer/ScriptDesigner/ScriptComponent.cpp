// ScriptComponent.cpp : implementation file
//

#include "stdafx.h"

#include "ScriptDesignerDLL.h"
#include "Script.h"
#include "ScriptRef.h"
#include <ContainerDesigner.h>
#include <dmusici.h>


/////////////////////////////////////////////////////////////////////////////
// CScriptComponent constructor/destructor 

CScriptComponent::CScriptComponent()
{
    m_dwRef = 0;
	
	m_pIFramework = NULL;
	m_pIDMPerformance = NULL;

	m_pIScriptDocType8 = NULL;
	m_pIConductor = NULL;
	m_pIContainerComponent = NULL;
	m_nFirstImage = 0;
	
	m_fMenuWasAdded = FALSE;
	m_pDebugDlg = NULL;
	m_pDebugDlgWP = NULL;
	m_pLogTool = NULL;

	m_nNextScript = 0;

	m_cfProducerFile = 0;
	m_cfScript = 0;
	m_cfScriptList = 0;

	m_fDisplayMessageText = FALSE;
	m_fEndMessageTextThread = FALSE;
	m_hEvent_WakeUpMessageTextThread = NULL;
	m_hEvent_ExitMessageTextThread = NULL;
	m_hMessageTextThread = NULL;

	// Initialize critical sections
	::InitializeCriticalSection( &m_CrSecMessageText );
	::InitializeCriticalSection( &m_CrSecDebugDlg );
}

CScriptComponent::~CScriptComponent()
{
	if( m_pDebugDlgWP )
	{
		delete m_pDebugDlgWP;
		m_pDebugDlgWP = NULL;
	}

	// Make sure MessageText thread is shut down properly
	// Should have happened in CleanUp()
	ASSERT( m_hEvent_WakeUpMessageTextThread == NULL );
	ASSERT( m_hEvent_ExitMessageTextThread == NULL );
	ASSERT( m_hMessageTextThread == NULL );
	ASSERT( m_lstMessageText.IsEmpty() );

	ReleaseAll();

	// Clean up the critical sections
	::DeleteCriticalSection( &m_CrSecMessageText );
	::DeleteCriticalSection( &m_CrSecDebugDlg );
}


/////////////////////////////////////////////////////////////////////////////
// CScriptComponent::ReleaseAll

void CScriptComponent::ReleaseAll( void )
{
 	CDirectMusicScript* pScript;
	while( !m_lstScripts.IsEmpty() )
	{
		pScript = static_cast<CDirectMusicScript*>( m_lstScripts.RemoveHead() );
		RELEASE( pScript );
	}

	RELEASE( m_pIFramework );
	RELEASE( m_pIScriptDocType8 );
	RELEASE( m_pLogTool );
	RELEASE( m_pIDMPerformance );
	RELEASE( m_pIConductor );
	RELEASE( m_pIContainerComponent );
}


/////////////////////////////////////////////////////////////////////////////
// CScriptComponent IUnknown implementation

HRESULT CScriptComponent::QueryInterface( REFIID riid, LPVOID FAR* ppvObj )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    if( ::IsEqualIID(riid, IID_IDMUSProdComponent)
	||  ::IsEqualIID(riid, IID_IUnknown) )
    {
        AddRef();
        *ppvObj = (IDMUSProdComponent *)this;
        return S_OK;
    }

    if( ::IsEqualIID(riid, IID_IDMUSProdRIFFExt) )
    {
        AddRef();
        *ppvObj = (IDMUSProdRIFFExt *)this;
        return S_OK;
    }

    if( ::IsEqualIID(riid, IID_IDMUSProdMenu) )
    {
        AddRef();
        *ppvObj = (IDMUSProdMenu *)this;
        return S_OK;
    }

    if( ::IsEqualIID(riid, IID_IDMUSProdDebugScript) )
    {
        AddRef();
        *ppvObj = (IDMUSProdDebugScript *)this;
        return S_OK;
    }

	if( ::IsEqualIID(riid, IID_IDMUSProdPortNotify))
	{
        AddRef();
		*ppvObj = (IDMUSProdPortNotify *)this;
        return S_OK;
	}
	
    *ppvObj = NULL;
    return E_NOINTERFACE;
}

ULONG CScriptComponent::AddRef()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	AfxOleLockApp(); 
    return ++m_dwRef;
}

ULONG CScriptComponent::Release()
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
// CScriptComponent IDMUSProdComponent implementation

/////////////////////////////////////////////////////////////////////////////
// CScriptComponent IDMUSProdComponent::Initialize

HRESULT CScriptComponent::Initialize( IDMUSProdFramework* pIFramework, BSTR* pbstrErrMsg )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	IDMUSProdComponent* pIComponent = NULL;
	TCHAR achErrMsg[MID_BUFFER];
	CString strErrMsg;

	if( m_pIFramework )		// already initialized
	{
		return S_OK;
	}

	ASSERT( pIFramework != NULL );
	ASSERT( pbstrErrMsg != NULL );

	if( pbstrErrMsg == NULL )
	{
		return E_POINTER;
	}

	if( pIFramework == NULL )
	{
		::LoadString( theApp.m_hInstance, IDS_ERR_INVALIDARG, achErrMsg, MID_BUFFER );
		strErrMsg = achErrMsg;
		*pbstrErrMsg = strErrMsg.AllocSysString();
		return E_INVALIDARG;
	}

	m_pIFramework = pIFramework;
	m_pIFramework->AddRef();

	theApp.m_pScriptComponent = this;
//	theApp.m_pScriptComponent->AddRef();	intentionally missing

	// Get IConductor and IDirectMusicPerformance interface pointers 
	if( FAILED ( pIFramework->FindComponent( CLSID_CConductor,  &pIComponent ) )
	||  FAILED ( pIComponent->QueryInterface( IID_IDMUSProdConductor, (void**)&m_pIConductor ) )
	||  FAILED ( m_pIConductor->GetPerformanceEngine( (IUnknown**)&m_pIDMPerformance ) ) )
	{
		ReleaseAll();
		if( pIComponent )
		{
			RELEASE( pIComponent );
		}
		::LoadString( theApp.m_hInstance, IDS_ERR_MISSING_CONDUCTOR, achErrMsg, MID_BUFFER );
		strErrMsg = achErrMsg;
		*pbstrErrMsg = strErrMsg.AllocSysString();
		return E_FAIL;
	}
	RELEASE( pIComponent );

	// Get IDMUSProdComponent for Container Designer
	if( FAILED ( pIFramework->FindComponent( CLSID_ContainerComponent, &m_pIContainerComponent ) ) )
	{
		ReleaseAll();
		::LoadString( theApp.m_hInstance, IDS_ERR_MISSING_CONTAINER_DESIGNER, achErrMsg, MID_BUFFER );
		strErrMsg = achErrMsg;
		*pbstrErrMsg = strErrMsg.AllocSysString();
		return E_FAIL;
	}

	// Create log tool
	m_pLogTool = new CLogTool( (IDMUSProdDebugScript *)this );
	if( m_pLogTool == NULL )
	{
		ReleaseAll();
		::LoadString( theApp.m_hInstance, IDS_ERR_MEMORY, achErrMsg, MID_BUFFER );
		strErrMsg = achErrMsg;
		*pbstrErrMsg = strErrMsg.AllocSysString();
		return E_FAIL;
	}

	// Add log tool to performance tool graph
	IDirectMusicGraph* pIDMGraph = NULL;
	if( FAILED ( m_pIDMPerformance->GetGraph( &pIDMGraph ) ) )
	{
		// No tool graph in the performance - we need to add one
		if( SUCCEEDED(CoCreateInstance(
				CLSID_DirectMusicGraph,
				NULL,
				CLSCTX_INPROC, 
				IID_IDirectMusicGraph,
				(void**)&pIDMGraph
			)))
		{
			if( FAILED( m_pIDMPerformance->SetGraph( pIDMGraph ) ) )
			{
				RELEASE( pIDMGraph );
			}
		}
	}

	// Check if we were unable to find or create a tool graph
	if( pIDMGraph == NULL )
	{
		ReleaseAll();
		::LoadString( theApp.m_hInstance, IDS_ERR_ADD_LOGTOOL, achErrMsg, MID_BUFFER );
		strErrMsg = achErrMsg;
		*pbstrErrMsg = strErrMsg.AllocSysString();
		return E_FAIL;
	}

	if( FAILED ( pIDMGraph->InsertTool( m_pLogTool, NULL, 0, 9999 ) ) )
	{
		pIDMGraph->Release();
		ReleaseAll();
		::LoadString( theApp.m_hInstance, IDS_ERR_ADD_LOGTOOL, achErrMsg, MID_BUFFER );
		strErrMsg = achErrMsg;
		*pbstrErrMsg = strErrMsg.AllocSysString();
		return E_FAIL;
	}
	RELEASE( pIDMGraph );

	// Add applicable images to the Project Tree control's image list 
	if( FAILED ( AddNodeImageLists() ) )
	{
		ReleaseAll();
		::LoadString( theApp.m_hInstance, IDS_ERR_ADD_IMAGELIST, achErrMsg, MID_BUFFER );
		strErrMsg = achErrMsg;
		*pbstrErrMsg = strErrMsg.AllocSysString();
		return E_FAIL;
	}

	// Register clipboard formats
	if( RegisterClipboardFormats() == FALSE )
	{
		ReleaseAll();
		::LoadString( theApp.m_hInstance, IDS_ERR_REGISTER_CF, achErrMsg, MID_BUFFER );
		strErrMsg = achErrMsg;
		*pbstrErrMsg = strErrMsg.AllocSysString();
		return E_FAIL;
	}

	// Register the clipboard format for an .spp file 
	CString strExt = _T(".spp");
	BSTR bstrExt = strExt.AllocSysString();
	if( FAILED ( pIFramework->RegisterClipFormatForFile(m_cfScript, bstrExt) ) )
	{
		ReleaseAll();
		::LoadString( theApp.m_hInstance, IDS_ERR_ADD_CLIPFORMAT, achErrMsg, MID_BUFFER );
		strErrMsg = achErrMsg;
		*pbstrErrMsg = strErrMsg.AllocSysString();
		return E_FAIL;
	}

	// Add "Message Window" item to Add-Ins menu 
	if( FAILED ( pIFramework->AddMenuItem( (IDMUSProdMenu *)this ) ) )
	{
		ReleaseAll();
		::LoadString( theApp.m_hInstance, IDS_ERR_ADD_MENUITEM, achErrMsg, MID_BUFFER );
		strErrMsg = achErrMsg;
		*pbstrErrMsg = strErrMsg.AllocSysString();
		return E_FAIL;
	}
	else
	{
		m_fMenuWasAdded = TRUE;
	}

	// Create MessageText thread
	if( StartMessageTextThread() == FALSE )
	{
		ReleaseAll();
		::LoadString( theApp.m_hInstance, IDS_ERR_MESSAGE_WINDOW_THREAD, achErrMsg, MID_BUFFER );
		strErrMsg = achErrMsg;
		*pbstrErrMsg = strErrMsg.AllocSysString();
		return E_FAIL;
	}

	// Register applicable doc types with the Framework 
	m_pIScriptDocType8 = new CScriptDocType;
    if( m_pIScriptDocType8 == NULL )
	{
		ReleaseAll();
		::LoadString( theApp.m_hInstance, IDS_ERR_MEMORY, achErrMsg, MID_BUFFER );
		strErrMsg = achErrMsg;
		*pbstrErrMsg = strErrMsg.AllocSysString();
		return E_OUTOFMEMORY;
	}

	m_pIScriptDocType8->AddRef();

	if( FAILED ( pIFramework->AddDocType(m_pIScriptDocType8) ) )
	{
		ReleaseAll();
		::LoadString( theApp.m_hInstance, IDS_ERR_ADD_DOCTYPE, achErrMsg, MID_BUFFER );
		strErrMsg = achErrMsg;
		*pbstrErrMsg = strErrMsg.AllocSysString();
		return E_FAIL;
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CScriptComponent IDMUSProdComponent::CleanUp

HRESULT CScriptComponent::CleanUp( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	m_fDisplayMessageText = FALSE;
	
	// Remove log tool from performance tool graph
	if( m_pLogTool )
	{
		IDirectMusicGraph* pIDMGraph;
		if( SUCCEEDED ( m_pIDMPerformance->GetGraph( &pIDMGraph ) ) )
		{
			pIDMGraph->RemoveTool( m_pLogTool );
			RELEASE( pIDMGraph );
		}
	}

	// Make sure MessageText thread is shut down properly
	EndMessageTextThread();

	// Cleanup m_lstMessageText
	::EnterCriticalSection( &m_CrSecMessageText );
	CString* pstrMessageText;
	while( !m_lstMessageText.IsEmpty() )
	{
		pstrMessageText = static_cast<CString*>( m_lstMessageText.RemoveHead() );
		delete pstrMessageText;
	}
	::LeaveCriticalSection( &m_CrSecMessageText );

	// Close "Message Window"
	if( m_pDebugDlg )
	{
		m_pDebugDlg->DestroyWindow();
		delete m_pDebugDlg;
		m_pDebugDlg = NULL;
	}

	// Remove "Message Window" menu item
	if( m_fMenuWasAdded )
	{
		m_pIFramework->RemoveMenuItem( (IDMUSProdMenu *)this );
		m_fMenuWasAdded = FALSE;
	}

	// Release references to scripts
	CDirectMusicScript *pScript;
	while( !m_lstScripts.IsEmpty() )
	{
		pScript = static_cast<CDirectMusicScript*>( m_lstScripts.RemoveHead() );
		RELEASE( pScript );
	}
	
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CScriptComponent IDMUSProdComponent::GetName

HRESULT CScriptComponent::GetName( BSTR* pbstrName )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CString strName;
	TCHAR achBuffer[MID_BUFFER];

	if( ::LoadString(theApp.m_hInstance, IDS_SCRIPT_COMPONENT_NAME, achBuffer, MID_BUFFER) )
	{
		strName = achBuffer;
	}

    *pbstrName = strName.AllocSysString();

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CScriptComponent::IDMUSProdComponent::AllocReferenceNode

HRESULT CScriptComponent::AllocReferenceNode( GUID guidRefNodeId, IDMUSProdNode** ppIRefNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( ppIRefNode == NULL )
	{
		return E_POINTER;
	}

	*ppIRefNode = NULL;

	// Make sure Component can create Nodes of type guidRefNodeId
	if( !( IsEqualGUID ( guidRefNodeId, GUID_ScriptRefNode ) ) )
	{
		return E_INVALIDARG;
	}

	// Create ScriptRefNode
	CScriptRef* pScriptRef = new CScriptRef;
	if( pScriptRef == NULL )
	{
		return E_OUTOFMEMORY ;
	}

	*ppIRefNode = (IDMUSProdNode *)pScriptRef;
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CScriptComponent IDMUSProdComponent::OnActivateApp

HRESULT CScriptComponent::OnActivateApp( BOOL fActivate )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(fActivate);

	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CScriptComponent IDMUSProdRIFFExt::LoadRIFFChunk

HRESULT CScriptComponent::LoadRIFFChunk( IStream* pIStream, IDMUSProdNode** ppINode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CDirectMusicScript* pScript;
	HRESULT hr;

	ASSERT( pIStream != NULL );
	ASSERT( m_pIFramework != NULL );

	if( ppINode == NULL )
	{
		return E_POINTER;
	}

	*ppINode = NULL;

	// Create a new Script 
	pScript = new CDirectMusicScript;
	if( pScript == NULL )
	{
		return E_OUTOFMEMORY ;
	}

	// Create the Undo Manager
	if( pScript->CreateUndoMgr() == FALSE )
	{
		pScript->Release();
		return E_OUTOFMEMORY;
	}

	// Load Script file
	hr = pScript->Load( pIStream );
	if( FAILED ( hr ) )
	{
		pScript->Release();
		return hr;
	}

	*ppINode = (IDMUSProdNode *)pScript;
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CScriptComponent IDMUSProdMenu implementation

/////////////////////////////////////////////////////////////////////////////
// CScriptComponent IDMUSProdMenu::GetMenuText

HRESULT CScriptComponent::GetMenuText( BSTR* pbstrText )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CString strText;
	strText.LoadString( IDS_DEBUG_WINDOW_MENU_TEXT );

	*pbstrText = strText.AllocSysString();

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CScriptComponent IDMUSProdMenu::GetMenuHelpText

HRESULT CScriptComponent::GetMenuHelpText( BSTR* pbstrHelpText )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CString strHelpText;
	strHelpText.LoadString( IDS_DEBUG_WINDOW_MENU_HELP_TEXT );

	*pbstrHelpText = strHelpText.AllocSysString();

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CScriptComponent IDMUSProdMenu::OnMenuInit

HRESULT CScriptComponent::OnMenuInit( HMENU hMenu, UINT nMenuID )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	::EnableMenuItem( hMenu, nMenuID, MF_ENABLED );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CScriptComponent IDMUSProdMenu::OnMenuSelect

HRESULT CScriptComponent::OnMenuSelect()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pDebugDlg )
	{
		// Window is already open
		if( m_pDebugDlg->IsIconic() ) 
		{
			m_pDebugDlg->ShowWindow( SW_RESTORE );     // If iconic, restore the main window
		}
		m_pDebugDlg->SetForegroundWindow();
		return S_OK;
	}

	// Create "Message Window"
	m_pDebugDlg = new CDebugDlg;
	if( m_pDebugDlg == NULL )
	{
		return E_OUTOFMEMORY;
	}

	if( m_pDebugDlg->CreateEx( WS_EX_TOPMOST, AfxRegisterWndClass(0), "Message Window",
							   (WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_CLIPCHILDREN),
							   0, 0,
							   400, 200,
							   NULL	/*pParentWnd*/,
							   NULL,
							   NULL ) )
	{
		HICON hIcon = ::LoadIcon( theApp.m_hInstance, MAKEINTRESOURCE(IDR_SCRIPT_DOCTYPE) );
		if( hIcon )
		{
			m_pDebugDlg->SetIcon( hIcon, TRUE );
		}
		if( m_pDebugDlgWP )
		{
			m_pDebugDlg->SetWindowPlacement( m_pDebugDlgWP );
		}
		return S_OK;
	}

	delete m_pDebugDlg;
	m_pDebugDlg = NULL;
	return E_FAIL;
}


/////////////////////////////////////////////////////////////////////////////
// CScriptComponent IDMUSProdDebugScript implementation

/////////////////////////////////////////////////////////////////////////////
// CScriptComponent IDMUSProdDebugScript::DisplayScriptError

HRESULT CScriptComponent::DisplayScriptError( void* pErrorInfo )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pErrorInfo == NULL )
	{
		return E_INVALIDARG;
	}

	if( m_fDisplayMessageText == FALSE
	||  m_pDebugDlg == NULL
	||  m_pDebugDlg->GetSafeHwnd() == NULL )
	{
		return E_ACCESSDENIED;
	}

	DMUS_SCRIPT_ERRORINFO* pScriptErrorInfo = (DMUS_SCRIPT_ERRORINFO *)pErrorInfo;

	CString strText;
	CString strTemp;
	CString strLineNbr;

	strLineNbr.Format( "<%04i> ", m_pDebugDlg->GetNextLineNbr() );

	strText = _T( "Script error in " );
	strText += pScriptErrorInfo->wszSourceFile;
	if( pScriptErrorInfo->wszSourceLineText[0] )
	{
		strTemp.Format( ", line %u, column %i.", pScriptErrorInfo->ulLineNumber, pScriptErrorInfo->ichCharPosition );
	}
	else
	{
		strTemp.Format( ", line %u.", pScriptErrorInfo->ulLineNumber );
	}
	CString* pstrText1 = new CString( strLineNbr + strText + strTemp );

	CString* pstrText2 = NULL;
	if( pScriptErrorInfo->wszSourceLineText[0] )
	{
		strText = _T( "Near: " );
		strText += pScriptErrorInfo->wszSourceLineText;
		pstrText2 = new CString( strLineNbr + strText );
	}

	CString* pstrText3 = NULL;
	if( pScriptErrorInfo->wszSourceComponent[0] 
	||  pScriptErrorInfo->wszDescription[0] )
	{
		strText = pScriptErrorInfo->wszSourceComponent;
		strText += _T( ": " );
		strText += pScriptErrorInfo->wszDescription;
		pstrText3 = new CString( strLineNbr + strText );
	}

	strText.Format( "Error Code: 0x%08X", pScriptErrorInfo->hr );
	CString* pstrText4 = new CString( strLineNbr + strText );

	// Place text in m_lstMessageText
	// Will be displayed by MessageTextThreadProc()
	::EnterCriticalSection( &m_CrSecMessageText );
	if( pstrText1 )
	{
		m_lstMessageText.AddTail( pstrText1 );
	}
	if( pstrText2 )
	{
		m_lstMessageText.AddTail( pstrText2 );
	}
	if( pstrText3 )
	{
		m_lstMessageText.AddTail( pstrText3 );
	}
	if( pstrText4 )
	{
		m_lstMessageText.AddTail( pstrText4 );
	}
	::LeaveCriticalSection( &m_CrSecMessageText );

	return E_FAIL;
}


/////////////////////////////////////////////////////////////////////////////
// CScriptComponent IDMUSProdDebugScript::DisplayText

HRESULT CScriptComponent::DisplayText( WCHAR* pwszText )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_fDisplayMessageText == FALSE
	||  m_pDebugDlg == NULL
	||  m_pDebugDlg->GetSafeHwnd() == NULL )
	{
		return E_ACCESSDENIED;
	}

	CString* pstrText = new CString;
	
	if( pstrText )
	{
		pstrText->Format( "<%04i> ", m_pDebugDlg->GetNextLineNbr() );
		*pstrText += pwszText;
		
		// Place text in m_lstMessageText
		// Will be displayed by MessageTextThreadProc()
		::EnterCriticalSection( &m_CrSecMessageText );
		m_lstMessageText.AddTail( pstrText );
		::LeaveCriticalSection( &m_CrSecMessageText );
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CScriptComponent IDMUSProdPortNotify implementation

/////////////////////////////////////////////////////////////////////////////
// CScriptComponent IDMUSProdPortNotify::OnOutputPortsChanged

HRESULT CScriptComponent::OnOutputPortsChanged( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	HRESULT hr = E_FAIL;

	// Add log tool to performance tool graph
	if( m_pIDMPerformance
	&&  m_pLogTool )
	{
		IDirectMusicGraph* pIDMGraph = NULL;

		if( FAILED ( m_pIDMPerformance->GetGraph( &pIDMGraph ) ) )
		{
			// No tool graph in the performance - we need to add one
			if( SUCCEEDED ( CoCreateInstance( CLSID_DirectMusicGraph, NULL, CLSCTX_INPROC, 
											  IID_IDirectMusicGraph, (void**)&pIDMGraph ) ) )
			{
				if( FAILED( m_pIDMPerformance->SetGraph( pIDMGraph ) ) )
				{
					RELEASE( pIDMGraph );
				}
			}
		}

		// Do we have a toolgraph?
		if( pIDMGraph )
		{
			// If yes, insert the log tool
			hr = pIDMGraph->InsertTool( m_pLogTool, NULL, 0, 9999 );
			
			RELEASE( pIDMGraph );
		}

	}

    // Re-synchronize the scripts with the new audiopath
	POSITION pos = m_lstScripts.GetHeadPosition();
	while( pos )
	{
		m_lstScripts.GetNext( pos )->SyncScriptWithDirectMusic();
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CScriptComponent IDMUSProdPortNotify::OnOutputPortsRemoved

HRESULT CScriptComponent::OnOutputPortsRemoved( void )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CScriptComponent additional functions

/////////////////////////////////////////////////////////////////////////////
// MessageTextThread routines

/////////////////////////////////////////////////////////////////////////////
// MessageTextThreadProc

UINT AFX_CDECL MessageTextThreadProc( LPVOID pParam )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(pParam);

	if ( theApp.m_pScriptComponent == NULL )
	{
		ASSERT( 0 );
		return UINT_MAX;    // illegal parameter
	}

	CString* pstrMessageText;

	BOOL fInCritSeq = FALSE;

	try {
	while( 1 )
	{
		DWORD dwRes;
		dwRes = WaitForSingleObject( theApp.m_pScriptComponent->m_hEvent_WakeUpMessageTextThread, 300 );

		// Time to exit?
		if( theApp.m_pScriptComponent->m_fEndMessageTextThread )
		{
			//TRACE( "MessageTextThreadProc: m_fEndMessageTextThread is TRUE.\n" );
			break;
		}

		// Display text in Message Window
		if( (dwRes == WAIT_OBJECT_0)
		||  (dwRes == WAIT_TIMEOUT) )
		{
			// See if m_lstMessageText contains text to be displayed
			if( !theApp.m_pScriptComponent->m_lstMessageText.IsEmpty() )
			{
				int i = 1;
				int iMax = theApp.m_pScriptComponent->m_fDisplayMessageText ? 100 : 250;

				// Create the "list"
				CTypedPtrList<CPtrList, CString*>* plstText = new CTypedPtrList<CPtrList, CString*>;

				// Transfer text from m_lstMessageText to "plstText"
				::EnterCriticalSection( &theApp.m_pScriptComponent->m_CrSecMessageText );
				fInCritSeq = TRUE;
				while( !theApp.m_pScriptComponent->m_lstMessageText.IsEmpty() )
				{
					pstrMessageText = static_cast<CString*>( theApp.m_pScriptComponent->m_lstMessageText.RemoveHead() );
					
					if( theApp.m_pScriptComponent->m_fDisplayMessageText )
					{
						plstText->AddTail( pstrMessageText );
					}
					else
					{
						delete pstrMessageText;
					}

					if( ++i >= iMax )
					{
						break;
					}
				}
				::LeaveCriticalSection( &theApp.m_pScriptComponent->m_CrSecMessageText );
				fInCritSeq = FALSE;

				::EnterCriticalSection( &theApp.m_pScriptComponent->m_CrSecDebugDlg );
				if( !plstText->IsEmpty() )
				{
					// Display text in "list"
					::PostMessage( theApp.m_pScriptComponent->m_pDebugDlg->GetSafeHwnd(), WM_DISPLAY_MESSAGE_TEXT, 0, (LPARAM)plstText );
				}
				else
				{
					delete plstText;
				}
				::LeaveCriticalSection( &theApp.m_pScriptComponent->m_CrSecDebugDlg );
			}
		}
		else
		{
			//TRACE( "MessageTextThreadProc: break out early.\n" );
			break;
		}
		// Go back and WaitForSingleObject again.
	}
	}
	catch( ... )
	{
		TRACE("MessageTextThreadProc: caught exception. exiting.\n");
	}

	if( fInCritSeq )
	{
		::LeaveCriticalSection( &theApp.m_pScriptComponent->m_CrSecMessageText );
	}

	if( theApp.m_pScriptComponent->m_hEvent_ExitMessageTextThread )
	{
		//TRACE( "MessageTextThreadProc: setting exit event.\n" );
		::SetEvent( theApp.m_pScriptComponent->m_hEvent_ExitMessageTextThread );
	}

	//TRACE( "MessageTextThreadProc exiting.\n" );
	return 0;
}


/////////////////////////////////////////////////////////////////////////////
// CScriptComponent::StartMessageTextThread

BOOL CScriptComponent::StartMessageTextThread( void )
{
	// Make sure MessageText thread is shut down properly
	EndMessageTextThread();

	// Make sure MessageText list is empty
	::EnterCriticalSection( &m_CrSecMessageText );
	while( !m_lstMessageText.IsEmpty() )
	{
		CString* pstrMessageText = static_cast<CString*>( m_lstMessageText.RemoveHead() );
		delete pstrMessageText;
	}
	::LeaveCriticalSection( &m_CrSecMessageText );

	// Create m_hEvent_WakeUpMessageTextThread event
	if( m_hEvent_WakeUpMessageTextThread == NULL )
	{
		m_hEvent_WakeUpMessageTextThread = ::CreateEvent( NULL, FALSE, FALSE, NULL );
		if( m_hEvent_WakeUpMessageTextThread == NULL )
		{
			return FALSE;
		}
	}

	// Create m_hEvent_ExitMessageTextThread event
	if( m_hEvent_ExitMessageTextThread == NULL )
	{
		m_hEvent_ExitMessageTextThread = ::CreateEvent( NULL, FALSE, FALSE, NULL );
		if( m_hEvent_ExitMessageTextThread == NULL )
		{
			return FALSE;
		}
	}

	// Start Message Window thread
	ASSERT( m_hMessageTextThread == NULL );
	m_fEndMessageTextThread = FALSE;
	CWinThread* pThread = ::AfxBeginThread( MessageTextThreadProc, this, THREAD_PRIORITY_LOWEST );
	if( pThread == NULL )
	{
		return FALSE;
	}
	m_hMessageTextThread = pThread->m_hThread;

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CScriptComponent::EndMessageTextThread

BOOL CScriptComponent::EndMessageTextThread( void )
{
	if( m_hMessageTextThread )
	{
		DWORD dwExitCode;
		if( GetExitCodeThread( m_hMessageTextThread, &dwExitCode ) )
		{
			if( dwExitCode == STILL_ACTIVE )
			{
				ASSERT( m_hEvent_WakeUpMessageTextThread != NULL );
				ASSERT( m_hEvent_ExitMessageTextThread != NULL );

				// Signal the MessageText thread so it can exit
				m_fEndMessageTextThread = TRUE;
				::SetEvent( m_hEvent_WakeUpMessageTextThread );

				// Wait for the MessageText thread to exit
				::WaitForSingleObject( m_hEvent_ExitMessageTextThread, 5000 );
			}
		}

		m_hMessageTextThread = NULL;
	}

	// Close m_hEvent_WakeUpMessageTextThread
	if( m_hEvent_WakeUpMessageTextThread )
	{
		::CloseHandle( m_hEvent_WakeUpMessageTextThread );
		m_hEvent_WakeUpMessageTextThread = NULL;
	}

	// Close m_hEvent_ExitMessageTextThread
	if( m_hEvent_ExitMessageTextThread )
	{
		::CloseHandle( m_hEvent_ExitMessageTextThread );
		m_hEvent_ExitMessageTextThread = NULL;
	}

	// Reinitialize to FALSE
	m_fEndMessageTextThread = FALSE;

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CScriptComponent::AddNodeImageLists

HRESULT CScriptComponent::AddNodeImageLists( void )
{
	CImageList lstImages;
	HICON hIcon;

	lstImages.Create( 16, 16, ILC_COLOR16, 4, 0 );
	lstImages.SetBkColor( GetSysColor(COLOR_WINDOW) );

	hIcon = ::LoadIcon( theApp.m_hInstance, MAKEINTRESOURCE(IDR_SCRIPT_DOCTYPE) );
	lstImages.Add( hIcon );
	::DestroyIcon( hIcon );
	hIcon = ::LoadIcon( theApp.m_hInstance, MAKEINTRESOURCE(IDI_SCRIPT_DOCTYPE_SEL) );
	lstImages.Add( hIcon );
	::DestroyIcon( hIcon );

	hIcon = ::LoadIcon( theApp.m_hInstance, MAKEINTRESOURCE(IDI_SCRIPTREF) );
	lstImages.Add( hIcon );
	::DestroyIcon( hIcon );
	hIcon = ::LoadIcon( theApp.m_hInstance, MAKEINTRESOURCE(IDI_SCRIPTREF_SEL) );
	lstImages.Add( hIcon );
	::DestroyIcon( hIcon );

	if( FAILED (m_pIFramework->AddNodeImageList( lstImages.Detach(), &m_nFirstImage ) ) )
	{
		return E_FAIL;
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CScriptComponent::RegisterClipboardFormats

BOOL CScriptComponent::RegisterClipboardFormats( void )
{
	m_cfProducerFile = ::RegisterClipboardFormat( CF_DMUSPROD_FILE );
	m_cfScript = ::RegisterClipboardFormat( CF_SCRIPT );
	m_cfScriptList = ::RegisterClipboardFormat( CF_SCRIPTLIST );

	if( m_cfProducerFile == 0
	||  m_cfScript == 0
	||  m_cfScriptList == 0 )
	{
		return FALSE;
	}

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CScriptComponent::GetScriptImageIndex

HRESULT CScriptComponent::GetScriptImageIndex( short* pnFirstImage )
{
	*pnFirstImage = (short)(m_nFirstImage + FIRST_SCRIPT_IMAGE);
	
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CScriptComponent::GetScriptRefImageIndex

HRESULT CScriptComponent::GetScriptRefImageIndex( short* pnFirstImage )
{
	*pnFirstImage = (short)(m_nFirstImage + FIRST_SCRIPTREF_IMAGE);
	
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CScriptComponent::AddToScriptFileList

void CScriptComponent::AddToScriptFileList( CDirectMusicScript* pScript )
{
	if( pScript )
	{
		GUID guidScript;
		pScript->GetGUID( &guidScript );

		// Prevent duplicate object GUIDs
		GUID guidScriptList;
		POSITION pos = m_lstScripts.GetHeadPosition();
		while( pos )
		{
			CDirectMusicScript* pScriptList = m_lstScripts.GetNext( pos );

			pScriptList->GetGUID( &guidScriptList );
			if( ::IsEqualGUID( guidScriptList, guidScript ) )
			{
				::CoCreateGuid( &guidScript );
				pScript->SetGUID( guidScript );
				break;
			}
		}

		// Add to list
		pScript->AddRef();
		m_lstScripts.AddTail( pScript );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CScriptComponent::RemoveFromScriptFileList

void CScriptComponent::RemoveFromScriptFileList( CDirectMusicScript* pScript )
{
	if( pScript )
	{
		// Remove from list
		POSITION pos = m_lstScripts.Find( pScript );
		if( pos )
		{
			m_lstScripts.RemoveAt( pos );
			pScript->Release();
		}
	}
}
