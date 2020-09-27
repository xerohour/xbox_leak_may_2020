// Script.cpp : implementation file
//

#include "stdafx.h"
#include "ScriptDesignerDLL.h"

#include "RiffStrm.h"
#include "Script.h"
#include <mmreg.h>
#include <math.h>
#include "TabScriptScript.h"
#include "TabScriptInfo.h"
#include "ScriptCtl.h"
#include <ContainerDesigner.h>
#include <dmusicf.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// {D5C2F18E-D180-4246-BB25-58154A64F122}
static const GUID GUID_ScriptUndo = 
{ 0xd5c2f18e, 0xd180, 0x4246, { 0xbb, 0x25, 0x58, 0x15, 0x4a, 0x64, 0xf1, 0x22 } };

// {F078ACC0-67AA-11d3-B45D-00105A2796DE}
static const GUID GUID_ScriptPropPageManager = 
{ 0xf078acc0, 0x67aa, 0x11d3, { 0xb4, 0x5d, 0x0, 0x10, 0x5a, 0x27, 0x96, 0xde } };

short CScriptPropPageManager::sm_nActiveTab = 0;


//////////////////////////////////////////////////////////////////////
// CScriptPropPageManager Construction/Destruction
//////////////////////////////////////////////////////////////////////

CScriptPropPageManager::CScriptPropPageManager()
{
    m_dwRef = 0;
	AddRef();

	m_pIPropPageObject = NULL;
	m_pIPropSheet = NULL;

	m_pTabScript = NULL;
	m_pTabInfo = NULL;
}

CScriptPropPageManager::~CScriptPropPageManager()
{
	RELEASE( m_pIPropSheet );

	if( m_pTabScript )
	{
		delete m_pTabScript;
	}

	if( m_pTabInfo )
	{
		delete m_pTabInfo;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CScriptPropPageManager::RemoveCurrentObject

void CScriptPropPageManager::RemoveCurrentObject( void )
{
	if( m_pIPropPageObject == NULL )
	{
		return;
	}

	m_pIPropPageObject->OnRemoveFromPageManager();
	m_pIPropPageObject = NULL;
}


/////////////////////////////////////////////////////////////////////////////
// CScriptPropPageManager IUnknown implementation

HRESULT CScriptPropPageManager::QueryInterface( REFIID riid, LPVOID FAR* ppvObj )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

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

ULONG CScriptPropPageManager::AddRef()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    return ++m_dwRef;
}

ULONG CScriptPropPageManager::Release()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

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
// CScriptPropPageManager IDMUSProdPropPageManager implementation

/////////////////////////////////////////////////////////////////////////////
// CScriptPropPageManager IDMUSProdPropPageManager::GetPropertySheetTitle

HRESULT CScriptPropPageManager::GetPropertySheetTitle( BSTR* pbstrTitle, BOOL* pfAddPropertiesText )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( (pbstrTitle == NULL)
	||  (pfAddPropertiesText == NULL) )
	{
		return E_POINTER;
	}

	*pfAddPropertiesText = TRUE;

	CString strTitle;
	strTitle.LoadString( IDS_SCRIPT_TEXT );

	CDirectMusicScript *pScript;

	if( m_pIPropPageObject
	&& (SUCCEEDED (m_pIPropPageObject->GetData((void **)&pScript))) )
	{
		CString strNodeName;
		BSTR bstrNodeName;

		if( SUCCEEDED ( pScript->GetNodeName( &bstrNodeName ) ) )
		{
			strNodeName = bstrNodeName;
			::SysFreeString( bstrNodeName );
		}
		strTitle = strNodeName + _T(" ") + strTitle;
	}

	*pbstrTitle = strTitle.AllocSysString();

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CScriptPropPageManager IDMUSProdPropPageManager::GetPropertySheetPages

HRESULT CScriptPropPageManager::GetPropertySheetPages( IDMUSProdPropSheet* pIPropSheet, LONG* hPropSheetPage[], short* pnNbrPages )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

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

	HPROPSHEETPAGE hPage;
	short nNbrPages = 0;

	// Add Script tab
	m_pTabScript = new CTabScriptScript( this );
	if( m_pTabScript )
	{
		hPage = ::CreatePropertySheetPage( (LPPROPSHEETPAGE)&m_pTabScript->m_psp );
		if( hPage )
		{
			hPropSheetPage[nNbrPages] = (LONG *)hPage;
			nNbrPages++;
		}			
	}

	// Add Info tab
	m_pTabInfo = new CTabScriptInfo( this );
	if( m_pTabInfo )
	{
		hPage = ::CreatePropertySheetPage( (LPPROPSHEETPAGE)&m_pTabInfo->m_psp );
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
// CScriptPropPageManager IDMUSProdPropPageManager::OnRemoveFromPropertySheet

HRESULT CScriptPropPageManager::OnRemoveFromPropertySheet()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( m_pIPropSheet != NULL );
	m_pIPropSheet->GetActivePage( &CScriptPropPageManager::sm_nActiveTab );

	RemoveCurrentObject();
	theApp.m_pIPageManager = NULL;

	Release();	// delete myself

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CScriptPropPageManager IDMUSProdPropPageManager::SetObject

HRESULT CScriptPropPageManager::SetObject( IDMUSProdPropPageObject* pINewPropPageObject )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

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
// CScriptPropPageManager IDMUSProdPropPageManager::RemoveObject

HRESULT CScriptPropPageManager::RemoveObject( IDMUSProdPropPageObject* pIPropPageObject )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

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
// CScriptPropPageManager  IDMUSProdPropPageManager::IsEqualObject

HRESULT FAR EXPORT CScriptPropPageManager::IsEqualObject( IDMUSProdPropPageObject* pIPropPageObject )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

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
// CScriptPropPageManager IDMUSProdPropPageManager::RefreshData

HRESULT CScriptPropPageManager::RefreshData( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CDirectMusicScript* pScript;
	
	if( m_pIPropPageObject == NULL )
	{
		pScript = NULL;
	}
	else if( FAILED ( m_pIPropPageObject->GetData( (void **)&pScript ) ) )
	{
		return E_FAIL;
	}

	// Make sure changes to current Script are processed in OnKillFocus
	// messages before setting the new Script
	CWnd* pWndHadFocus = CWnd::GetFocus();
	CWnd* pWnd = pWndHadFocus;
	CWnd* pWndParent = m_pTabScript->GetParent();

	while( pWnd )
	{
		if( pWnd == pWndParent )
		{
			::SetFocus( NULL );
			break;
		}
		pWnd = pWnd->GetParent();
	}

	// Set Property tabs to display the new Script
	m_pTabScript->SetScript( pScript);
	m_pTabInfo->SetScript( pScript);

	// Restore focus
	if( pWndHadFocus
	&&  pWndHadFocus != CWnd::GetFocus() )
	{
		pWndHadFocus->SetFocus();
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CScriptPropPageManager  IDMUSProdPropPageManager::IsEqualPageManagerGUID

HRESULT CScriptPropPageManager::IsEqualPageManagerGUID( REFGUID rguidPageManager )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( ::IsEqualGUID(rguidPageManager, GUID_ScriptPropPageManager) )
	{
		return S_OK;
	}

	return S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CDirectMusicScript constructor/destructor

CDirectMusicScript::CDirectMusicScript()
{
	ASSERT( theApp.m_pScriptComponent != NULL );

	// Script needs Script Component
	theApp.m_pScriptComponent->AddRef();

    m_dwRef = 0;
	AddRef();

	m_pUndoMgr = NULL;
	m_fModified = FALSE;

	m_pIDocRootNode = NULL;
	m_pIParentNode = NULL;
	m_pIContainerNode = NULL;
	m_pIDMScript = NULL;

	m_hWndEditor = NULL;
	m_pScriptCtrl = NULL;
	m_pINodeBeingDeleted = NULL;
	m_pIProject = NULL;
	m_fDeletingContainer = false;
	m_fInitializingVariables = false;

	m_dwFlagsDM = (DMUS_SCRIPTIOF_LOAD_ALL_CONTENT | DMUS_SCRIPTIOF_DOWNLOAD_ALL_SEGMENTS);

// Script info block
	TCHAR achName[SMALL_BUFFER];
	::LoadString( theApp.m_hInstance, IDS_SCRIPT_TEXT, achName, SMALL_BUFFER );
    m_strName.Format( "%s%d", achName, ++theApp.m_pScriptComponent->m_nNextScript );

	m_strLanguage.LoadString( IDS_SCRIPT_LANGUAGE_DEFAULT );

	CoCreateGuid( &m_guidScript ); 
	m_vVersion.dwVersionMS = 0x00010000;
	m_vVersion.dwVersionLS = 0x00000000;
	m_vDirectMusicVersion.dwVersionMS = 0x00080000;
	m_vDirectMusicVersion.dwVersionLS = 0x00000000;
}

CDirectMusicScript::~CDirectMusicScript()
{
	ASSERT( theApp.m_pScriptComponent != NULL );
	ASSERT( theApp.m_pScriptComponent->m_pIFramework != NULL );

	// Remove Script from clipboard
	theApp.FlushClipboard( this );

	// Remove Script from property sheet
	IDMUSProdPropSheet* pIPropSheet;
	if( SUCCEEDED ( theApp.m_pScriptComponent->m_pIFramework->QueryInterface( IID_IDMUSProdPropSheet, (void**)&pIPropSheet ) ) )
	{
		pIPropSheet->RemovePageManagerByObject( this );
		RELEASE( pIPropSheet );
	}

	// Cleanup the Undo Manager
	if( m_pUndoMgr )
	{
		delete m_pUndoMgr;
		m_pUndoMgr = NULL;
	}

	// Make sure that Containers cleanup properly.  Otherwise
	// Container that happens to be in the Framework's notification
	// list will never have its destructor called.
	if( m_pIContainerNode )
	{
		// Messing with m_dwRef to prevent destructor from being called twice
		ASSERT( m_dwRef == 0 );
		m_dwRef = 1;
		m_fDeletingContainer = true;
		m_pIContainerNode->DeleteNode( FALSE );
		m_fDeletingContainer = false;
		ASSERT( m_dwRef == 1 );
		m_dwRef = 0;
	}

	// Cleanup references
	if( m_pIDMScript )
	{
		IDirectMusicObjectP* pIDMObjectP;
		if( SUCCEEDED ( m_pIDMScript->QueryInterface( IID_IDirectMusicObjectP, (void**)&pIDMObjectP ) ) )
		{
			pIDMObjectP->Zombie();
			RELEASE( pIDMObjectP );
		}
	}
	RELEASE( m_pIDMScript );
	RELEASE( m_pIContainerNode );

	// Cleanup VariableState list
 	VariableState* pVariableState;
	while( !m_lstLastKnownVariableStates.IsEmpty() )
	{
		pVariableState = static_cast<VariableState*>( m_lstLastKnownVariableStates.RemoveHead() );
		delete pVariableState;
	}

	// Script no longer needs Script Component
	theApp.m_pScriptComponent->Release();
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicScript::CreateEmptyContainer

HRESULT CDirectMusicScript::CreateEmptyContainer( void )
{
	IDMUSProdNode* pINode;
	IDMUSProdDocType* pIDocType;

	ASSERT( theApp.m_pScriptComponent != NULL );
	ASSERT( theApp.m_pScriptComponent->m_pIFramework != NULL );

	// Do not call if Script already has a container!
	ASSERT( m_pIContainerNode == NULL );
	if( m_pIContainerNode )
	{
		return S_OK;
	}

	// Create the Container
	HRESULT hr = theApp.m_pScriptComponent->m_pIFramework->FindDocTypeByNodeId( GUID_ContainerNode, &pIDocType );
	if( SUCCEEDED ( hr ) ) 
	{
		hr = pIDocType->AllocNode( GUID_ContainerNode, &pINode );
		if( SUCCEEDED ( hr ) )
		{
			m_pIContainerNode = pINode;

			// Set parent
			m_pIContainerNode->SetParentNode( (IDMUSProdNode *)this );

			// Set name
			CString strName;
			strName.LoadString( IDS_CONTAINER_NODE_NAME );
			BSTR bstrName = strName.AllocSysString();
			m_pIContainerNode->SetNodeName( bstrName );
		}

		RELEASE( pIDocType );
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicScript::CreateUndoMgr

BOOL CDirectMusicScript::CreateUndoMgr()
{
	// Should only be called once - after Script first created
	ASSERT( m_pUndoMgr == NULL );

	ASSERT( theApp.m_pScriptComponent != NULL ); 
	ASSERT( theApp.m_pScriptComponent->m_pIFramework != NULL ); 

	m_pUndoMgr = new CJazzUndoMan( theApp.m_pScriptComponent->m_pIFramework );
	if( m_pUndoMgr )
	{
		m_pUndoMgr->SetStreamAttributes( FT_DESIGN, GUID_ScriptUndo );
		m_pUndoMgr->SetUndoLevel( 16 );
		return TRUE;
	}

	return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicScript::SyncScriptEditor

void CDirectMusicScript::SyncScriptEditor( DWORD dwFlags  )
{
	if( m_pScriptCtrl
	&&  m_pScriptCtrl->m_pScriptDlg )
	{
		m_pScriptCtrl->m_pScriptDlg->RefreshControls( dwFlags );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicScript::SyncScriptWithDirectMusic

HRESULT CDirectMusicScript::SyncScriptWithDirectMusic( void )
{
	CWaitCursor wait;
	IStream* pIMemStream;
	IPersistStream* pIPersistStream;
	HRESULT hr;

	ASSERT( theApp.m_pScriptComponent != NULL );
	ASSERT( theApp.m_pScriptComponent->m_pIFramework != NULL );

	// Make sure the parent of m_pIContainerNode has been set to the script
	if( m_pIContainerNode )
	{
		m_pIContainerNode->SetParentNode( (IDMUSProdNode *)this );
	}

	// Make sure we have a DirectMusic Engine Script
	if( m_pIDMScript == NULL )
	{
		if( FAILED ( ::CoCreateInstance( CLSID_DirectMusicScript, NULL, CLSCTX_INPROC_SERVER,
										 IID_IDirectMusicScript, (void**)&m_pIDMScript ) ) )
		{
			RELEASE( m_pIDMScript );
		}
	}

	if( m_pIDMScript == NULL )
	{
		// Nothing to do
		return S_OK;
	}

	// Must be FT_DESIGN so that objects aren't embedded in the container
	hr = theApp.m_pScriptComponent->m_pIFramework->AllocMemoryStream( FT_DESIGN, GUID_DirectMusicObject, &pIMemStream );

	if( SUCCEEDED ( hr ) )
	{
		hr = Save( pIMemStream, FALSE );
		if( SUCCEEDED ( hr ) )
		{
			m_pIDMScript->QueryInterface( IID_IPersistStream, (void **)&pIPersistStream );
			if( pIPersistStream )
			{
				// If m_lstLastKnownVariableStates is not empty,
				// the last call to m_pIDMScript->Init must have failed
				// and we need to use the existing list
				if( m_lstLastKnownVariableStates.IsEmpty() )
				{
					// Get current state of variables
					GetVariableState( m_lstLastKnownVariableStates );
				}

				StreamSeek( pIMemStream, 0, STREAM_SEEK_SET );
				hr = pIPersistStream->Load( pIMemStream );
				if( SUCCEEDED ( hr ) )
				{
					// Set DirectMusic object's filename
					BSTR bstrFileName;
					if( SUCCEEDED ( theApp.m_pScriptComponent->m_pIFramework->GetNodeFileName( this, &bstrFileName ) ) )
					{
						CString strFileName = bstrFileName;
						::SysFreeString( bstrFileName );

						int nFindPos = strFileName.ReverseFind( (TCHAR)'\\' );
						if( nFindPos != -1 )
						{
							strFileName = strFileName.Right( strFileName.GetLength() - nFindPos - 1 );
						}

						IDirectMusicObject* pIDMObject;
						if( SUCCEEDED ( m_pIDMScript->QueryInterface( IID_IDirectMusicObject, (void **)&pIDMObject ) ) )
						{
							DMUS_OBJECTDESC dmusObjectDesc;
							ZeroMemory( &dmusObjectDesc, sizeof(DMUS_OBJECTDESC) );
							dmusObjectDesc.dwSize = sizeof(DMUS_OBJECTDESC);
							dmusObjectDesc.dwValidData = DMUS_OBJ_FILENAME;
							MultiByteToWideChar( CP_ACP, 0, strFileName, -1, dmusObjectDesc.wszFileName, MAX_PATH );

							pIDMObject->SetDescriptor( &dmusObjectDesc );

							RELEASE( pIDMObject );
						}
					}

					// Initialize DirectMusic Script
					DMUS_SCRIPT_ERRORINFO ErrorInfo;
					memset( &ErrorInfo, 0, sizeof(DMUS_SCRIPT_ERRORINFO) );
					ErrorInfo.dwSize = sizeof(DMUS_SCRIPT_ERRORINFO);

					hr = m_pIDMScript->Init( theApp.m_pScriptComponent->m_pIDMPerformance, &ErrorInfo );
					if( SUCCEEDED ( hr ) )
					{
						// Assign values to variables 
						if( m_fInitializingVariables == false )
						{
							SetVariableState( m_lstLastKnownVariableStates );
						}

						// Cleanup VariableState list
 						VariableState* pVariableState;
						while( !m_lstLastKnownVariableStates.IsEmpty() )
						{
							pVariableState = static_cast<VariableState*>( m_lstLastKnownVariableStates.RemoveHead() );
							delete pVariableState;
						}
					}
					else 
					{
						DisplayScriptError( &ErrorInfo, hr );
					}

					// Rebuild Script editor (when open)
					SyncScriptEditor( SSE_ROUTINES | SSE_VARIABLES );
				}

				RELEASE( pIPersistStream );
			}
		}

		RELEASE( pIMemStream );
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicScript::SetModified

void CDirectMusicScript::SetModified( BOOL fModified )
{
	m_fModified = fModified;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicScript IUnknown implementation

HRESULT CDirectMusicScript::QueryInterface( REFIID riid, LPVOID* ppvObj )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    if( ::IsEqualIID(riid, IID_IDMUSProdNode)
	||  ::IsEqualIID(riid, IID_IUnknown) )
    {
        *ppvObj = (IDMUSProdNode *)this;
    }
    else if( ::IsEqualIID(riid, IID_IPersist) )
    {
        *ppvObj = (IPersist *)this;
    }
    else if( ::IsEqualIID(riid, IID_IPersistStream) )
    {
        *ppvObj = (IPersistStream *)this;
    }
    else if( ::IsEqualIID(riid, IID_IDMUSProdPropPageObject) )
    {
        *ppvObj = (IDMUSProdPropPageObject *)this;
    }
	else if( ::IsEqualIID(riid, IID_IDMUSProdNotifySink) )
	{
		*ppvObj = (IDMUSProdNotifySink*) this;
	}
	else
	{
		*ppvObj = NULL;
		return E_NOINTERFACE;
	}

	AddRef();
	return S_OK;
}

ULONG CDirectMusicScript::AddRef()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	AfxOleLockApp(); 
    return ++m_dwRef;
}

ULONG CDirectMusicScript::Release()
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
// CDirectMusicScript IDMUSProdNode implementation

/////////////////////////////////////////////////////////////////////////////
// CDirectMusicScript IDMUSProdNode::GetNodeImageIndex

HRESULT CDirectMusicScript::GetNodeImageIndex( short* pnFirstImage )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( theApp.m_pScriptComponent != NULL );

	return( theApp.m_pScriptComponent->GetScriptImageIndex(pnFirstImage) );
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicScript IDMUSProdNode::GetFirstChild

HRESULT CDirectMusicScript::GetFirstChild( IDMUSProdNode** ppIFirstChildNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( ppIFirstChildNode == NULL )
	{
		return E_POINTER;
	}
	
	if( m_pIContainerNode )
	{
		m_pIContainerNode->AddRef();
		*ppIFirstChildNode = m_pIContainerNode;
	}
	else
	{
		*ppIFirstChildNode = NULL;
	}

    return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicScript IDMUSProdNode::GetNextChild

HRESULT CDirectMusicScript::GetNextChild( IDMUSProdNode* pIChildNode, IDMUSProdNode** ppINextChildNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( ppINextChildNode == NULL )
	{
		return E_POINTER;
	}

	if( pIChildNode == NULL )
	{
		return E_INVALIDARG;
	}
	
	*ppINextChildNode = NULL;
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicScript IDMUSProdNode::GetComponent

HRESULT CDirectMusicScript::GetComponent( IDMUSProdComponent** ppIComponent )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( theApp.m_pScriptComponent != NULL );

	return theApp.m_pScriptComponent->QueryInterface( IID_IDMUSProdComponent, (void**)ppIComponent );
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicScript IDMUSProdNode::GetDocRootNode

HRESULT CDirectMusicScript::GetDocRootNode( IDMUSProdNode** ppIDocRootNode )
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
// CDirectMusicScript IDMUSProdNode::SetDocRootNode

HRESULT CDirectMusicScript::SetDocRootNode( IDMUSProdNode* pIDocRootNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( pIDocRootNode != NULL );

	m_pIDocRootNode = pIDocRootNode;
//	m_pIDocRootNode->AddRef();		intentionally missing

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicScript IDMUSProdNode::GetParentNode

HRESULT CDirectMusicScript::GetParentNode( IDMUSProdNode** ppIParentNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( ppIParentNode == NULL )
	{
		return E_POINTER;
	}

	*ppIParentNode = m_pIParentNode;

	if( m_pIParentNode )
	{
		m_pIParentNode->AddRef();
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicScript IDMUSProdNode::SetParentNode

HRESULT CDirectMusicScript::SetParentNode( IDMUSProdNode* pIParentNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	m_pIParentNode = pIParentNode;
//	m_pIParentNode->AddRef();		intentionally missing

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicScript IDMUSProdNode::GetNodeId

HRESULT CDirectMusicScript::GetNodeId( GUID* pguid )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pguid == NULL )
	{
		return E_POINTER;
	}

	*pguid = GUID_ScriptNode;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicScript IDMUSProdNode::GetNodeName

HRESULT CDirectMusicScript::GetNodeName( BSTR* pbstrName )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    *pbstrName = m_strName.AllocSysString();

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicScript IDMUSProdNode::GetNodeNameMaxLength

HRESULT CDirectMusicScript::GetNodeNameMaxLength( short* pnMaxLength )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	*pnMaxLength = DMUS_MAX_NAME;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicScript IDMUSProdNode::ValidateNodeName

HRESULT CDirectMusicScript::ValidateNodeName( BSTR bstrName )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CString strName;

	strName = bstrName;
	::SysFreeString( bstrName );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicScript IDMUSProdNode::SetNodeName

HRESULT CDirectMusicScript::SetNodeName( BSTR bstrName )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( theApp.m_pScriptComponent != NULL );
	ASSERT( theApp.m_pScriptComponent->m_pIFramework != NULL );

	CString strName = bstrName;
	::SysFreeString( bstrName );

	if( m_strName == strName )
	{
		return S_OK;
	}

	m_pUndoMgr->SaveState( this, theApp.m_hInstance, IDS_UNDO_SCRIPT_NAME );
	m_strName = strName;

	// Sync change with property sheet
	IDMUSProdPropSheet* pIPropSheet;
	if( SUCCEEDED ( theApp.m_pScriptComponent->m_pIFramework->QueryInterface( IID_IDMUSProdPropSheet, (void**)&pIPropSheet ) ) )
	{
		pIPropSheet->RefreshTitleByObject( this );
		pIPropSheet->RefreshActivePageByObject( this );
		RELEASE( pIPropSheet );
	}
	
	SetModified( TRUE );
	SyncScriptWithDirectMusic();

	// Notify connected nodes that Script name has changed
	theApp.m_pScriptComponent->m_pIFramework->NotifyNodes( this, SCRIPT_NameChange, NULL );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicScript IDMUSProdNode::GetNodeListInfo

HRESULT CDirectMusicScript::GetNodeListInfo( DMUSProdListInfo* pListInfo )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CString strDescriptor;

    pListInfo->bstrName = m_strName.AllocSysString();
    pListInfo->bstrDescriptor = strDescriptor.AllocSysString();
	memcpy( &pListInfo->guidObject, &m_guidScript, sizeof(GUID) );

	// Must check pListInfo->wSize before populating additional fields
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicScript IDMUSProdNode::GetEditorClsId

HRESULT CDirectMusicScript::GetEditorClsId( CLSID* pClsId )
{
 	AFX_MANAGE_STATE(_afxModuleAddrThis);

   *pClsId = CLSID_ScriptEditor;
	
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicScript IDMUSProdNode::GetEditorTitle

HRESULT CDirectMusicScript::GetEditorTitle( BSTR* pbstrTitle )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CString strTitle;
	TCHAR achTitle[MID_BUFFER];

	::LoadString( theApp.m_hInstance, IDS_SCRIPT_TEXT, achTitle, MID_BUFFER );
	strTitle  = achTitle;
	strTitle += _T(": " );
	strTitle += m_strName;

    *pbstrTitle = strTitle.AllocSysString();

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicScript IDMUSProdNode::GetEditorWindow

HRESULT CDirectMusicScript::GetEditorWindow( HWND* hWndEditor )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	*hWndEditor = m_hWndEditor;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicScript IDMUSProdNode::SetEditorWindow

HRESULT CDirectMusicScript::SetEditorWindow( HWND hWndEditor )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	m_hWndEditor = hWndEditor;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicScript IDMUSProdNode::UseOpenCloseImages

HRESULT CDirectMusicScript::UseOpenCloseImages( BOOL* pfUseOpenCloseImages )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	*pfUseOpenCloseImages = FALSE;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicScript IDMUSProdNode::GetRightClickMenuId

HRESULT CDirectMusicScript::GetRightClickMenuId( HINSTANCE* phInstance, UINT* pnMenuId )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	*phInstance = theApp.m_hInstance;
	*pnMenuId   = IDM_SCRIPT_NODE_RMENU;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicScript IDMUSProdNode::OnRightClickMenuInit

HRESULT CDirectMusicScript::OnRightClickMenuInit( HMENU hMenu )
{
	//AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(hMenu);

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicScript IDMUSProdNode::OnRightClickMenuSelect

HRESULT CDirectMusicScript::OnRightClickMenuSelect( long lCommandId )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( theApp.m_pScriptComponent != NULL );
	ASSERT( theApp.m_pScriptComponent->m_pIFramework != NULL );

	HRESULT hr = E_FAIL;

	switch( lCommandId )
	{
		case IDM_RENAME:
			hr = theApp.m_pScriptComponent->m_pIFramework->EditNodeLabel( this );
			break;

		case IDM_PROPERTIES:
			OnShowProperties();
			hr = S_OK;
			break;
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicScript IDMUSProdNode::DeleteChildNode

HRESULT CDirectMusicScript::DeleteChildNode( IDMUSProdNode* pIChildNode, BOOL fPromptUser )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( theApp.m_pScriptComponent != NULL );
	ASSERT( theApp.m_pScriptComponent->m_pIFramework != NULL );

	if( m_fDeletingContainer )
	{
		// Nothing to do
		return S_OK;
	}

	CWaitCursor wait;

	if( pIChildNode == NULL 
	||  pIChildNode != m_pIContainerNode )
	{
		ASSERT( 0 );
		return E_INVALIDARG;
	}

	// Remove node from Project Tree
	if( theApp.m_pScriptComponent->m_pIFramework->RemoveNode( pIChildNode, fPromptUser ) == S_FALSE )
	{
		return E_FAIL;
	}

	// Make sure that Containers cleanup properly.  Otherwise
	// Container that happens to be in the Framework's notification
	// list will never have its destructor called.
	if( m_pIContainerNode )
	{
		m_fDeletingContainer = true;
		m_pIContainerNode->DeleteNode( FALSE );
		m_fDeletingContainer = false;
	}

	// Remove from Script
	RELEASE( m_pIContainerNode );

	SetModified( TRUE );

	// TODO - Remove this code!!
	// Update the DirectMusic Style object
//	HRESULT hr = m_pStyle->SyncStyleWithDirectMusic();
//	ASSERT( SUCCEEDED ( hr ) );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicScript IDMUSProdNode::InsertChildNode

HRESULT CDirectMusicScript::InsertChildNode( IDMUSProdNode* pIChildNode )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( theApp.m_pScriptComponent != NULL );
	ASSERT( theApp.m_pScriptComponent->m_pIFramework != NULL );

	if( pIChildNode == NULL )
	{
		return E_INVALIDARG;
	}

	pIChildNode->AddRef();

	// Scripts can only have one Container
	if( m_pIContainerNode )
	{
		DeleteChildNode( m_pIContainerNode, FALSE );
	}

	// Set the Script's Container
	ASSERT( m_pIContainerNode == NULL );
	m_pIContainerNode = pIChildNode;

	// Set root and parent node of ALL children
	theApp.SetNodePointers( m_pIContainerNode, (IDMUSProdNode *)this, (IDMUSProdNode *)this );

	// Add node to Project Tree
	if( !SUCCEEDED ( theApp.m_pScriptComponent->m_pIFramework->AddNode(m_pIContainerNode, (IDMUSProdNode *)this) ) )
	{
		DeleteChildNode( m_pIContainerNode, FALSE );
		return E_FAIL;
	}

	SetModified( TRUE );

	// TODO - Remove this code!!
	// Update the DirectMusic Style object
//	HRESULT hr = m_pStyle->SyncStyleWithDirectMusic();
//	ASSERT( SUCCEEDED ( hr ) );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicScript IDMUSProdNode::DeleteNode

HRESULT CDirectMusicScript::DeleteNode( BOOL fPromptUser )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CWaitCursor wait;

	// No parent so we will delete ourself
	ASSERT( theApp.m_pScriptComponent != NULL );
	ASSERT( theApp.m_pScriptComponent->m_pIFramework != NULL );

	// Remove from Project Tree
	if( theApp.m_pScriptComponent->m_pIFramework->RemoveNode( this, fPromptUser ) == S_FALSE )
	{
		// User cancelled delete
		return E_FAIL;
	}

	// Make sure that Containers cleanup properly.  Otherwise
	// Container that happens to be in the Framework's notification
	// list will never have its destructor called.
	if( m_pIContainerNode )
	{
		m_fDeletingContainer = true;
		m_pIContainerNode->DeleteNode( FALSE );
		m_fDeletingContainer = false;
	}

	// Remove from Script
	RELEASE( m_pIContainerNode );

	// Remove from Component Script list
	theApp.m_pScriptComponent->RemoveFromScriptFileList( this );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicScript IDMUSProdNode::OnNodeSelChanged

HRESULT CDirectMusicScript::OnNodeSelChanged( BOOL fSelected )
{
	//AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(fSelected);

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicScript IDMUSProdNode::CreateDataObject

HRESULT CDirectMusicScript::CreateDataObject( IDataObject** ppIDataObject )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( theApp.m_pScriptComponent != NULL );
	ASSERT( theApp.m_pScriptComponent->m_pIFramework != NULL );

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

	// Save Script into stream
	HRESULT hr = E_FAIL;

	if( SUCCEEDED ( theApp.m_pScriptComponent->m_pIFramework->AllocMemoryStream(FT_DESIGN, GUID_CurrentVersion, &pIStream) ) )
	{
		if( SUCCEEDED ( Save( pIStream, FALSE ) ) )
		{
			// Place CF_SCRIPT into CDllJazzDataObject
			if( SUCCEEDED ( pDataObject->AddClipFormat( theApp.m_pScriptComponent->m_cfScript, pIStream ) ) )
			{
				hr = S_OK;
			}
		}

		RELEASE( pIStream );
	}

	if( SUCCEEDED ( hr ) )
	{
		hr = E_FAIL;

		// Create a stream in CF_DMUSPROD_FILE format
		if( SUCCEEDED ( theApp.m_pScriptComponent->m_pIFramework->SaveClipFormat( theApp.m_pScriptComponent->m_cfProducerFile, this, &pIStream ) ) )
		{
			// Script nodes represent files so we must also
			// place CF_DMUSPROD_FILE into CDllJazzDataObject
			if( SUCCEEDED ( pDataObject->AddClipFormat( theApp.m_pScriptComponent->m_cfProducerFile, pIStream ) ) )
			{
				hr = S_OK;
			}

			RELEASE( pIStream );
		}
	}

	if( SUCCEEDED ( hr ) )
	{
		*ppIDataObject = pDataObject;	// already AddRef'd
	}
	else
	{
		RELEASE( pDataObject );
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicScript IDMUSProdNode::CanCut

HRESULT CDirectMusicScript::CanCut( void )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
 
	return CanDelete();
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicScript IDMUSProdNode::CanCopy

HRESULT CDirectMusicScript::CanCopy( void )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
 
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicScript IDMUSProdNode::CanDelete

HRESULT CDirectMusicScript::CanDelete( void )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicScript IDMUSProdNode::CanDeleteChildNode

HRESULT CDirectMusicScript::CanDeleteChildNode( IDMUSProdNode* pIChildNode )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(pIChildNode);

	return E_NOTIMPL;	// Cannot delete Containers in a script
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicScript IDMUSProdNode::CanPasteFromData

HRESULT CDirectMusicScript::CanPasteFromData( IDataObject* pIDataObject, BOOL* pfWillSetReference )
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
	
	return S_FALSE;		// Can't paste anything on a Script node
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicScript IDMUSProdNode::PasteFromData

HRESULT CDirectMusicScript::PasteFromData( IDataObject* pIDataObject )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(pIDataObject);

	ASSERT( 0 );
	
	return E_NOTIMPL;	// Can't paste anything on a Script node
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicScript IDMUSProdNode::CanChildPasteFromData

HRESULT CDirectMusicScript::CanChildPasteFromData( IDataObject* pIDataObject, IDMUSProdNode* pIChildNode,
												  BOOL* pfWillSetReference )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(pIDataObject);
	UNREFERENCED_PARAMETER(pIChildNode);
	UNREFERENCED_PARAMETER(pfWillSetReference);

	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicScript IDMUSProdNode::ChildPasteFromData

HRESULT CDirectMusicScript::ChildPasteFromData( IDataObject* pIDataObject, IDMUSProdNode* pIChildNode )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(pIDataObject);
	UNREFERENCED_PARAMETER(pIChildNode);

	ASSERT( 0 );

	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicScript IDMUSProdNode::GetObject

HRESULT CDirectMusicScript::GetObject( REFCLSID rclsid, REFIID riid, void** ppvObject )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Must be asking for an interface to a CLSID_DMScript object
	if( ::IsEqualCLSID( rclsid, CLSID_DirectMusicScript ) )
	{
		if( m_pIDMScript )
		{
			return m_pIDMScript->QueryInterface( riid, ppvObject );
		}
	}

    *ppvObject = NULL;
    return E_NOINTERFACE;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicScript IDMUSProdPropPageObject implementation

/////////////////////////////////////////////////////////////////////////////
// CDirectMusicScript IDMUSProdPropPageObject::GetData

HRESULT CDirectMusicScript::GetData( void** ppData )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( ppData == NULL )
	{
		return E_POINTER;
	}

	*ppData = this;

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicScript IDMUSProdPropPageObject::SetData

HRESULT CDirectMusicScript::SetData( void* pData )
{
	//AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(pData);

	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicScript IDMUSProdPropPageObject::OnShowProperties

HRESULT CDirectMusicScript::OnShowProperties( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	ASSERT( theApp.m_pScriptComponent != NULL );
	ASSERT( theApp.m_pScriptComponent->m_pIFramework != NULL );

	// Get the Script page manager
	CScriptPropPageManager* pPageManager;

	if( theApp.m_pIPageManager
	&&  theApp.m_pIPageManager->IsEqualPageManagerGUID( GUID_ScriptPropPageManager ) == S_OK )
	{
		pPageManager = (CScriptPropPageManager *)theApp.m_pIPageManager;
	}
	else
	{
		pPageManager = new CScriptPropPageManager();
	}

	if( pPageManager == NULL )
	{
		return E_FAIL;
	}

	// Show the Script properties
	IDMUSProdPropSheet* pIPropSheet;

	if( SUCCEEDED ( theApp.m_pScriptComponent->m_pIFramework->QueryInterface( IID_IDMUSProdPropSheet, (void**)&pIPropSheet ) ) )
	{
		short nActiveTab = CScriptPropPageManager::sm_nActiveTab;

		if( SUCCEEDED ( pIPropSheet->SetPageManager(pPageManager) ) )
		{
			theApp.m_pIPageManager = pPageManager;
			pPageManager->SetObject( this );
			pIPropSheet->SetActivePage( nActiveTab ); 
		}

		pIPropSheet->Show( TRUE );
		RELEASE( pIPropSheet );
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicScript IDMUSProdPropPageObject::OnRemoveFromPageManager

HRESULT CDirectMusicScript::OnRemoveFromPageManager( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicScript IPersist implementation

/////////////////////////////////////////////////////////////////////////////
// CDirectMusicScript IPersist::GetClassID

HRESULT CDirectMusicScript::GetClassID( CLSID* pClsId )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    ASSERT( pClsId != NULL );

    memset( pClsId, 0, sizeof( CLSID ) );

    return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicScript IPersistStream implementation

/////////////////////////////////////////////////////////////////////////////
// CDirectMusicScript IPersistStream::IsDirty

HRESULT CDirectMusicScript::IsDirty()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

// AMC - Will want to change way data syncs are handled!
	if( m_pScriptCtrl
	&&  m_pScriptCtrl->m_pScriptDlg )
	{
		m_pScriptCtrl->m_pScriptDlg->m_editSource.GetWindowText( m_strSource );
	}

	if( m_fModified )
	{
		return S_OK;
	}

	return S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicScript::LoadScript

HRESULT CDirectMusicScript::LoadScript( IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain )
{
    IStream*      pIStream;
	MMCKINFO	  ck;
	MMCKINFO	  ckList;
	DWORD		  dwByteCount;
	DWORD		  dwSize;
	DWORD		  dwPos;
    HRESULT       hr = S_OK;

	ASSERT( theApp.m_pScriptComponent != NULL );

	// Following strings only saved when they have values
	// So make sure they are initialized!!
	m_strAuthor.Empty();
	m_strCopyright.Empty();
	m_strSubject.Empty();

    pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

    dwPos = StreamTell( pIStream );

    while( pIRiffStream->Descend( &ck, pckMain, 0 ) == 0 )
	{
        switch( ck.ckid )
		{
			case DMUS_FOURCC_SCRIPT_CHUNK:
			{
			    DMUS_IO_SCRIPT_HEADER dmusScriptIO;

				dwSize = min( ck.cksize, sizeof( DMUS_IO_SCRIPT_HEADER ) );
				hr = pIStream->Read( &dmusScriptIO, dwSize, &dwByteCount );
				if( FAILED( hr )
				||  dwByteCount != dwSize )
				{
					hr = E_FAIL;
					goto ON_ERROR;
				}

				m_dwFlagsDM = dmusScriptIO.dwFlags;
				break;
			}

			case DMUS_FOURCC_GUID_CHUNK:
				dwSize = min( ck.cksize, sizeof( GUID ) );
				hr = pIStream->Read( &m_guidScript, dwSize, &dwByteCount );
				if( FAILED( hr )
				||  dwByteCount != dwSize )
				{
					hr = E_FAIL;
					goto ON_ERROR;
				}
				break;

			case DMUS_FOURCC_VERSION_CHUNK:
			{
			    DMUS_IO_VERSION dmusVersionIO;

				dwSize = min( ck.cksize, sizeof( DMUS_IO_VERSION ) );
				hr = pIStream->Read( &dmusVersionIO, dwSize, &dwByteCount );
				if( FAILED( hr )
				||  dwByteCount != dwSize )
				{
					hr = E_FAIL;
					goto ON_ERROR;
				}

				m_vVersion.dwVersionMS = dmusVersionIO.dwVersionMS;
				m_vVersion.dwVersionLS = dmusVersionIO.dwVersionLS;
				break;
			}

			case DMUS_FOURCC_SCRIPTVERSION_CHUNK:
			{
			    DMUS_IO_VERSION dmusVersionIO;

				dwSize = min( ck.cksize, sizeof( DMUS_IO_VERSION ) );
				hr = pIStream->Read( &dmusVersionIO, dwSize, &dwByteCount );
				if( FAILED( hr )
				||  dwByteCount != dwSize )
				{
					hr = E_FAIL;
					goto ON_ERROR;
				}

				m_vDirectMusicVersion.dwVersionMS = dmusVersionIO.dwVersionMS;
				m_vDirectMusicVersion.dwVersionLS = dmusVersionIO.dwVersionLS;
				break;
			}

			case DMUS_FOURCC_SCRIPTLANGUAGE_CHUNK:
				ReadMBSfromWCS( pIStream, ck.cksize, &m_strLanguage);
				break;

			case DMUS_FOURCC_SCRIPTSOURCE_CHUNK:
				ReadMBSfromWCS( pIStream, ck.cksize, &m_strSource);
				break;

			case FOURCC_RIFF:
				switch( ck.fccType )
				{
					case DMUS_FOURCC_CONTAINER_FORM: 
					{
						IDMUSProdRIFFExt* pIRIFFExt;
						IDMUSProdNode* pINode;

						hr = theApp.m_pScriptComponent->m_pIContainerComponent->QueryInterface( IID_IDMUSProdRIFFExt, (void**)&pIRIFFExt );
						if( FAILED( hr ) )
						{
			                goto ON_ERROR;
						}
					    StreamSeek( pIStream, dwPos, STREAM_SEEK_SET );
						hr = pIRIFFExt->LoadRIFFChunk( pIStream, &pINode );
						RELEASE( pIRIFFExt );
						if( FAILED( hr ) )
						{
			                goto ON_ERROR;
						}
						if( m_pIContainerNode )
						{
							InsertChildNode( pINode);
							pINode->Release();
						}
						else
						{
							m_pIContainerNode = pINode;
						}
						break;
					}
				}
				break;

			case FOURCC_LIST:
				switch( ck.fccType )
				{
					case DMUS_FOURCC_UNFO_LIST:
						while( pIRiffStream->Descend( &ckList, &ck, 0 ) == 0 )
						{
							switch( ckList.ckid )
							{
								case DMUS_FOURCC_UNAM_CHUNK:
								{
									CString strName;

									ReadMBSfromWCS( pIStream, ckList.cksize, &strName );
									if( strName.CompareNoCase( m_strName ) != 0 )
									{
										m_strName = strName;
										theApp.m_pScriptComponent->m_pIFramework->RefreshNode( this );

										// Notify connected nodes that the Script name has changed
										theApp.m_pScriptComponent->m_pIFramework->NotifyNodes( this, SCRIPT_NameChange, NULL );
									}
									break;
								}

								case DMUS_FOURCC_UART_CHUNK:
									ReadMBSfromWCS( pIStream, ckList.cksize, &m_strAuthor );
									break;

								case DMUS_FOURCC_UCOP_CHUNK:
									ReadMBSfromWCS( pIStream, ckList.cksize, &m_strCopyright );
									break;

								case DMUS_FOURCC_USBJ_CHUNK:
									ReadMBSfromWCS( pIStream, ckList.cksize, &m_strSubject );
									break;
							}
					        pIRiffStream->Ascend( &ckList, 0 );
						}
						break;
				}
				break;
        }

        pIRiffStream->Ascend( &ck, 0 );
	    dwPos = StreamTell( pIStream );
    }

ON_ERROR:
    RELEASE( pIStream );
    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicScript IPersistStream::Load

HRESULT CDirectMusicScript::Load( IStream* pIStream )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    ASSERT( pIStream != NULL );
	if( pIStream == NULL )
	{
		return E_INVALIDARG;
	}

    IDMUSProdRIFFStream* pIRiffStream;
    MMCKINFO ckMain;
    HRESULT hr = E_FAIL;

    DWORD dwPos = StreamTell( pIStream );

	// Check for Direct Music format
	BOOL fFoundFormat = FALSE;
	if( SUCCEEDED( AllocRIFFStream( pIStream, &pIRiffStream ) ) )
	{
		ckMain.fccType = DMUS_FOURCC_SCRIPT_FORM;

		if( pIRiffStream->Descend( &ckMain, NULL, MMIO_FINDRIFF ) == 0 )
		{
			fFoundFormat = TRUE;
			hr = LoadScript( pIRiffStream, &ckMain );
		}

		RELEASE( pIRiffStream );
	}

	if( fFoundFormat == FALSE )
	{
		// Check for legacy ascii text file format
	    StreamSeek( pIStream, dwPos, STREAM_SEEK_SET );
		hr = ReadSourceFromStream( pIStream );
		if( FAILED( hr ) )
		{
			return hr;
		}
	}

	if( m_pIDocRootNode )
	{
		// Sync change with property sheet
		IDMUSProdPropSheet* pIPropSheet;
		if( SUCCEEDED ( theApp.m_pScriptComponent->m_pIFramework->QueryInterface( IID_IDMUSProdPropSheet, (void**)&pIPropSheet ) ) )
		{
			pIPropSheet->RefreshTitle();
			pIPropSheet->RefreshActivePage();
			RELEASE( pIPropSheet );
		}
	}

	//	Persist Script to the DirectMusic DLLs.
	if( SUCCEEDED( hr ) )
	{
		if( m_pIContainerNode == NULL )
		{
			CreateEmptyContainer();
		}

		SyncScriptWithDirectMusic();
	}

    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicScript SaveSource
    
HRESULT CDirectMusicScript::SaveSource( IDMUSProdRIFFStream* pIRiffStream )
{
	IStream* pIStream;
	HRESULT hr;
    MMCKINFO ck;

    pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	// Write Script source
	ck.ckid = DMUS_FOURCC_SCRIPTSOURCE_CHUNK;
	if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	hr = SaveMBStoWCS( pIStream, &m_strSource );
	if( FAILED( hr ) )
	{
		goto ON_ERROR;
	}

	if( pIRiffStream->Ascend(&ck, 0) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

ON_ERROR:
    RELEASE( pIStream );
    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicScript SaveLanguage
    
HRESULT CDirectMusicScript::SaveLanguage( IDMUSProdRIFFStream* pIRiffStream )
{
	IStream* pIStream;
	HRESULT hr;
    MMCKINFO ck;

    pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	// Write Script laguage
	ck.ckid = DMUS_FOURCC_SCRIPTLANGUAGE_CHUNK;
	if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	hr = SaveMBStoWCS( pIStream, &m_strLanguage );
	if( FAILED( hr ) )
	{
		goto ON_ERROR;
	}

	if( pIRiffStream->Ascend(&ck, 0) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

ON_ERROR:
    RELEASE( pIStream );
    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicScript SaveDirectMusicVersion
    
HRESULT CDirectMusicScript::SaveDirectMusicVersion( IDMUSProdRIFFStream* pIRiffStream )
{
	IStream* pIStream;
	HRESULT hr;
	MMCKINFO ck;
	DWORD dwBytesWritten;
	DMUS_IO_VERSION dmusVersionIO;

    pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	// Write DirectMusic Version chunk header
	ck.ckid = DMUS_FOURCC_SCRIPTVERSION_CHUNK;
	if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Prepare DMUS_IO_VERSION structure
	memset( &dmusVersionIO, 0, sizeof(DMUS_IO_VERSION) );

	dmusVersionIO.dwVersionMS = m_vDirectMusicVersion.dwVersionMS ;
	dmusVersionIO.dwVersionLS = m_vDirectMusicVersion.dwVersionLS;

	// Write Version chunk data
	hr = pIStream->Write( &dmusVersionIO, sizeof(DMUS_IO_VERSION), &dwBytesWritten);
	if( FAILED( hr )
	||  dwBytesWritten != sizeof(DMUS_IO_VERSION) )
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
    RELEASE( pIStream );
    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicScript SaveInfoList
    
HRESULT CDirectMusicScript::SaveInfoList( IDMUSProdRIFFStream* pIRiffStream )
{
	IStream* pIStream;
	HRESULT hr;
    MMCKINFO ckMain;
    MMCKINFO ck;

	if( m_strName.IsEmpty()
	&&  m_strAuthor.IsEmpty()
	&&  m_strCopyright.IsEmpty()
	&&  m_strSubject.IsEmpty() )
	{
		return S_OK;
	}

    pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	// Write UNFO LIST header
	ckMain.fccType = DMUS_FOURCC_UNFO_LIST;
	if( pIRiffStream->CreateChunk(&ckMain, MMIO_CREATELIST) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Write Script name
	if( !m_strName.IsEmpty() )
	{
		ck.ckid = DMUS_FOURCC_UNAM_CHUNK;
		if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}

		hr = SaveMBStoWCS( pIStream, &m_strName );
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

	// Write script author
	if( !m_strAuthor.IsEmpty() )
	{
		ck.ckid = DMUS_FOURCC_UART_CHUNK;
		if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}

		hr = SaveMBStoWCS( pIStream, &m_strAuthor );
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

	// Write Script copyright
	if( !m_strCopyright.IsEmpty() )
	{
		ck.ckid = DMUS_FOURCC_UCOP_CHUNK;
		if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}

		hr = SaveMBStoWCS( pIStream, &m_strCopyright );
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

	// Write script subject
	if( !m_strSubject.IsEmpty() )
	{
		ck.ckid = DMUS_FOURCC_USBJ_CHUNK;
		if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}

		hr = SaveMBStoWCS( pIStream, &m_strSubject );
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
    RELEASE( pIStream );
    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicScript SaveVersion
    
HRESULT CDirectMusicScript::SaveVersion( IDMUSProdRIFFStream* pIRiffStream )
{
	IStream* pIStream;
	HRESULT hr;
	MMCKINFO ck;
	DWORD dwBytesWritten;
	DMUS_IO_VERSION dmusVersionIO;

    pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	// Write Version chunk header
	ck.ckid = DMUS_FOURCC_VERSION_CHUNK;
	if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Prepare DMUS_IO_VERSION structure
	memset( &dmusVersionIO, 0, sizeof(DMUS_IO_VERSION) );

	dmusVersionIO.dwVersionMS = m_vVersion.dwVersionMS ;
	dmusVersionIO.dwVersionLS = m_vVersion.dwVersionLS;

	// Write Version chunk data
	hr = pIStream->Write( &dmusVersionIO, sizeof(DMUS_IO_VERSION), &dwBytesWritten);
	if( FAILED( hr )
	||  dwBytesWritten != sizeof(DMUS_IO_VERSION) )
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
    RELEASE( pIStream );
    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicScript SaveGUID
    
HRESULT CDirectMusicScript::SaveGUID( IDMUSProdRIFFStream* pIRiffStream )
{
	IStream* pIStream;
	HRESULT hr;
	MMCKINFO ck;
	DWORD dwBytesWritten;

    pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	// Write GUID chunk header
	ck.ckid = DMUS_FOURCC_GUID_CHUNK;
	if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Write Script GUID
	hr = pIStream->Write( &m_guidScript, sizeof(GUID), &dwBytesWritten);
	if( FAILED( hr )
	||  dwBytesWritten != sizeof(GUID) )
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
    RELEASE( pIStream );
    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicScript SaveHeader
    
HRESULT CDirectMusicScript::SaveHeader( IDMUSProdRIFFStream* pIRiffStream )
{
	IStream* pIStream;
	HRESULT hr;
	MMCKINFO ck;
	DWORD dwBytesWritten;
	DMUS_IO_SCRIPT_HEADER dmusScriptIO;

    pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	// Write Script chunk header
	ck.ckid = DMUS_FOURCC_SCRIPT_CHUNK;
	if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Prepare DMUS_IO_SCRIPT_HEADER structure
	memset( &dmusScriptIO, 0, sizeof(DMUS_IO_SCRIPT_HEADER) );

	dmusScriptIO.dwFlags = m_dwFlagsDM;

	// Write Script chunk data
	hr = pIStream->Write( &dmusScriptIO, sizeof(DMUS_IO_SCRIPT_HEADER), &dwBytesWritten);
	if( FAILED( hr )
	||  dwBytesWritten != sizeof(DMUS_IO_SCRIPT_HEADER) )
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
    RELEASE( pIStream );
    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicScript SaveScript
    
HRESULT CDirectMusicScript::SaveScript( IDMUSProdRIFFStream* pIRiffStream, BOOL fClearDirty )
{
	HRESULT hr = S_OK;

    IStream* pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	DMUSProdStreamInfo StreamInfo;
	StreamInfo.ftFileType = FT_RUNTIME;
	StreamInfo.guidDataFormat = GUID_CurrentVersion;
	StreamInfo.pITargetDirectoryNode = NULL;

	// Get additional stream information
	IDMUSProdPersistInfo* pPersistInfo;
	if( SUCCEEDED ( pIStream->QueryInterface( IID_IDMUSProdPersistInfo, (void **)&pPersistInfo ) ) )
	{
		pPersistInfo->GetStreamInfo( &StreamInfo );
		RELEASE( pPersistInfo );
	}

// Save Script header chunk
	hr = SaveHeader( pIRiffStream );
	if( FAILED ( hr ) )
	{
		goto ON_ERROR;
	}

// Save Script GUID
	hr = SaveGUID( pIRiffStream );
	if( FAILED ( hr ) )
	{
		goto ON_ERROR;
	}

// Save Script version
	hr = SaveVersion( pIRiffStream );
	if( FAILED ( hr ) )
	{
		goto ON_ERROR;
	}

// Save Script info
	hr = SaveInfoList( pIRiffStream );
	if( FAILED ( hr ) )
	{
		goto ON_ERROR;
	}

// Save Script DirectMusic version
	hr = SaveDirectMusicVersion( pIRiffStream );
	if( FAILED ( hr ) )
	{
		goto ON_ERROR;
	}

// Save Script's Container
	if( m_pIContainerNode )
	{
		// Don't save container contents in undo stream
		if( IsEqualGUID( StreamInfo.guidDataFormat, GUID_ScriptUndo ) == FALSE )
		{
			IPersistStream* pIPersistStream;

			hr = m_pIContainerNode->QueryInterface( IID_IPersistStream, (void**)&pIPersistStream );
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
	}

// Save Script Language
	hr = SaveLanguage( pIRiffStream );
	if( FAILED ( hr ) )
	{
		goto ON_ERROR;
	}

// Save Script's Source Code
	hr = SaveSource( pIRiffStream );
	if( FAILED ( hr ) )
	{
		goto ON_ERROR;
	}

ON_ERROR:
    RELEASE( pIStream );
    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicScript IPersistStream::Save

HRESULT CDirectMusicScript::Save( IStream* pIStream, BOOL fClearDirty )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pIStream == NULL )
	{
		return E_INVALIDARG;
	}

	DMUSProdStreamInfo StreamInfo;
	StreamInfo.ftFileType = FT_RUNTIME;
	StreamInfo.guidDataFormat = GUID_CurrentVersion;
	StreamInfo.pITargetDirectoryNode = NULL;

	// Get additional stream information
	IDMUSProdPersistInfo* pPersistInfo;

	if( SUCCEEDED ( pIStream->QueryInterface( IID_IDMUSProdPersistInfo, (void **)&pPersistInfo ) ) )
	{
		pPersistInfo->GetStreamInfo( &StreamInfo );
		RELEASE( pPersistInfo );
	}

    IDMUSProdRIFFStream* pIRiffStream;
    MMCKINFO ckMain;

    HRESULT hr = E_FAIL;

	if( IsEqualGUID( StreamInfo.guidDataFormat, GUID_ScriptUndo ) == FALSE )
	{
		// Make sure we have the latest text
		// Necessary because sync takes place in KillFocus 
		// and save may have been initiated by Ctrl+S
		if( m_pScriptCtrl
		&&  m_pScriptCtrl->m_pScriptDlg )
		{
			m_pScriptCtrl->m_pScriptDlg->SyncSource();
		}
	}

	// Validate requested data format
	if( IsEqualGUID( StreamInfo.guidDataFormat, GUID_CurrentVersion )
	||  IsEqualGUID( StreamInfo.guidDataFormat, GUID_DirectMusicObject ) 
	||  IsEqualGUID( StreamInfo.guidDataFormat, GUID_ScriptUndo ) )
	{
		// DirectMusic format
		if( SUCCEEDED( AllocRIFFStream( pIStream, &pIRiffStream ) ) )
		{
			ckMain.fccType = DMUS_FOURCC_SCRIPT_FORM;

			if( pIRiffStream->CreateChunk( &ckMain, MMIO_CREATERIFF ) == 0
			&&  SUCCEEDED( SaveScript( pIRiffStream, fClearDirty ) )
			&&  pIRiffStream->Ascend( &ckMain, 0 ) == 0 )
			{
				if( fClearDirty )
				{
					SetModified( FALSE );
				}

				hr = S_OK;
			}
			RELEASE( pIRiffStream );
		}
	}

    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicScript IPersistStream::GetSizeMax

HRESULT CDirectMusicScript::GetSizeMax( ULARGE_INTEGER FAR* /*pcbSize*/ )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);

    return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicScript IDMUSProdNotifySink implementation

///////////////////////////////////////////////////////////////////////////
// CDirectMusicScript  IDMUSProdNotifySink::OnUpdate

HRESULT CDirectMusicScript::OnUpdate( IDMUSProdNode* pIDocRootNode, GUID guidUpdateType, VOID* pData )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( pIDocRootNode == m_pIContainerNode )
	{
		// CONTAINER_ChangeNotification
		if( ::IsEqualGUID(guidUpdateType, CONTAINER_ChangeNotification ) )
		{
			SetModified( TRUE );
			SyncScriptWithDirectMusic();
			return S_OK;
		}

		// CONTAINER_FileLoadFinished
		if( ::IsEqualGUID(guidUpdateType, CONTAINER_FileLoadFinished ) )
		{
			SyncScriptWithDirectMusic();
			return S_OK;
		}
	}

	if( pIDocRootNode == this )
	{
		if( ::IsEqualGUID(guidUpdateType, FRAMEWORK_FileNameChange)  )
		{
			SyncScriptWithDirectMusic();
			return S_OK;
		}
		if( ::IsEqualGUID(guidUpdateType, FRAMEWORK_AfterFileOpen)  )
		{
			if( m_pIDMScript )
			{
				WCHAR* pwszFileName = (WCHAR*)pData;
				CString strFileName = pwszFileName;

				// Set DirectMusic object's filename
				int nFindPos = strFileName.ReverseFind( (TCHAR)'\\' );
				if( nFindPos != -1 )
				{
					strFileName = strFileName.Right( strFileName.GetLength() - nFindPos - 1 );
				}

				IDirectMusicObject* pIDMObject;
				if( SUCCEEDED ( m_pIDMScript->QueryInterface( IID_IDirectMusicObject, (void **)&pIDMObject ) ) )
				{
					DMUS_OBJECTDESC dmusObjectDesc;
					ZeroMemory( &dmusObjectDesc, sizeof(DMUS_OBJECTDESC) );
					dmusObjectDesc.dwSize = sizeof(DMUS_OBJECTDESC);
					dmusObjectDesc.dwValidData = DMUS_OBJ_FILENAME;
					MultiByteToWideChar( CP_ACP, 0, strFileName, -1, dmusObjectDesc.wszFileName, MAX_PATH );

					pIDMObject->SetDescriptor( &dmusObjectDesc );

					RELEASE( pIDMObject );
				}
			}
			return S_OK;
		}
	}

	// GUID_DMCollectionResync 
	if( ::IsEqualGUID(guidUpdateType, GUID_DMCollectionResync ) )
	{
		SyncScriptWithDirectMusic();
		return S_OK;
	}

	return E_INVALIDARG;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicScript Additional functions

/////////////////////////////////////////////////////////////////////////////
// CDirectMusicScript IPersistStream::SaveScriptSource

HRESULT CDirectMusicScript::SaveScriptSource( IStream* pIStream )
{
	DWORD dwBytesWritten;
	HRESULT hr;

	// Source
	hr = pIStream->Write( m_strSource, m_strSource.GetLength(), &dwBytesWritten );
	if( FAILED( hr ) 
	||  dwBytesWritten != (DWORD)m_strSource.GetLength() )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

ON_ERROR:
	return hr;
}


//////////////////////////////////////////////////////////////////////
// Functions for parsing the script header block

void ParseSkipWhitespace( const WCHAR** ppwsz )
{
	ASSERT( !iswspace(L'\0') );
	
	while( iswspace(**ppwsz) )
	{
		++*ppwsz;
	}
}

UINT ParseContinuousString( const WCHAR* pwsz )
{
	const WCHAR *pwszEnd = pwsz;

	while( !iswspace(*pwszEnd) && *pwszEnd != L'>' && *pwszEnd != L'=' && *pwszEnd )
	{
		++pwszEnd;
	}

	return pwszEnd - pwsz;
}

UINT ParseQuotedString( const WCHAR* pwsz )
{
	const WCHAR *pwszEnd = pwsz;

	while( *++pwszEnd )
	{
		if( *pwszEnd == L'"' )
		{
			if( *++pwszEnd != L'"' )
			{
				break;
			}
		}
		else if( *pwszEnd == L'>'
			 ||  *pwszEnd == L'=' )
		{
			break;
		}
	}

	return pwszEnd - pwsz;
}

HRESULT ReformatQuotedString( const WCHAR* pwsz, WCHAR* pwszTarget )
{
	const WCHAR *pwszSource = pwsz;
	
	if( *pwszSource++ != L'"' )
	{
		return E_FAIL;
	}

	while( *pwszSource )	// note: successful exit is via return statement inside loop
	{
		if( *pwszSource == L'"' )
		{
			if( *++pwszSource == L'"' )
			{
				// Two quotes is an escape sequence that is replaced with a single quote.
				// All other characters are directly appended.
				*pwszTarget++ = *pwszSource++;
			}
			else
			{
				// A single quote completes the string.
				*pwszTarget = L'\0'; 
				return S_OK;
			}
		}
		else
		{
			*pwszTarget++ = *pwszSource++;
		}
	}

	// Must have hit end of string without closing quote.
	return E_FAIL;
}

HRESULT ParseNumericWord( const WCHAR** ppwsz, WORD* pw )
{
	const WCHAR *pwsz = *ppwsz;

	// Read the word
	int i = _wtoi( pwsz );
	if( i < 0
	||  i > MAXWORD )
	{
		return E_FAIL;
	}

	// Skip one digit
	if( !iswdigit(*pwsz++) )
	{
		return E_FAIL;
	}

	// Skip any other contiguous digits
	while( iswdigit(*pwsz) )
	{
		++pwsz;
	}

	*pw = i;
	*ppwsz = pwsz;
	return S_OK;
}

HRESULT ParseVersion( const WCHAR *pwszVersionField, DMUS_VERSION *pvVersion )
{
	const WCHAR *pwsz = pwszVersionField;

	// Read in four words separated by periods.
	WORD wVals[4];
	for( int i = 0; i < 4; ++i )
	{
		if( FAILED ( ::ParseNumericWord(&pwsz, &wVals[i]) )
		||  ( i < 3  &&  *pwsz++ != L'.' ) )
		{
			return E_FAIL;
		}
	}

	// Pack the word values into the version structure
	pvVersion->dwVersionMS = MAKELONG( wVals[1], wVals[0] );
	pvVersion->dwVersionLS = MAKELONG( wVals[3], wVals[2] );
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicScript::FoundField

HRESULT CDirectMusicScript::FoundField( const WCHAR* pwszName, const WCHAR* pwszValue, DMUSProdListInfo* pListInfo )
{
	if( 0 == _wcsicmp(pwszName, L"NAME") )
	{
		m_strName = pwszValue;
		if( m_strName.IsEmpty() == TRUE )
		{
			return E_OUTOFMEMORY;
		}
		if( pListInfo )
		{
			pListInfo->bstrName = m_strName.AllocSysString();
		}
	}
	else if( 0 == _wcsicmp(pwszName, L"GUID") )
	{
		if( FAILED ( ::CLSIDFromString(const_cast<WCHAR *>(pwszValue), &m_guidScript) ) )
		{
			return E_FAIL;
		}
		if( pListInfo )
		{
			memcpy( &pListInfo->guidObject, &m_guidScript, sizeof(GUID) );
		}
	}
	else if( 0 == _wcsicmp(pwszName, L"VERSION") )
	{
		if( FAILED ( ::ParseVersion (pwszValue, &m_vVersion) ) )
		{
			return E_FAIL;
		}
	}
	else if( 0 == _wcsicmp(pwszName, L"DIRECTMUSICVERSION") )
	{
		if( FAILED ( ::ParseVersion(pwszValue, &m_vDirectMusicVersion) ) )
		{
			return E_FAIL;
		}
	}
	else if( 0 == _wcsicmp(pwszName, L"LANGUAGE") )
	{
		m_strLanguage = pwszValue;
		if( m_strLanguage.IsEmpty() == TRUE )
		{
			return E_OUTOFMEMORY;
		}
	}
	else if( 0 == _wcsicmp(pwszName, L"AUTHOR") )
	{
		m_strAuthor = pwszValue;
		if( m_strAuthor.IsEmpty() == TRUE )
		{
			return E_OUTOFMEMORY;
		}
	}
	else if( 0 == _wcsicmp(pwszName, L"COPYRIGHT") )
	{
		m_strCopyright = pwszValue;
		if( m_strCopyright.IsEmpty() == TRUE )
		{
			return E_OUTOFMEMORY;
		}
	}
	else if( 0 == _wcsicmp(pwszName, L"SUBJECT") )
	{
		m_strSubject = pwszValue;
		if( m_strSubject.IsEmpty() == TRUE )
		{
			return E_OUTOFMEMORY;
		}
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicScript::ParseField

HRESULT CDirectMusicScript::ParseField( const WCHAR** ppwsz, DMUSProdListInfo* pListInfo )
{
	const WCHAR *pwsz = *ppwsz;
	::ParseSkipWhitespace( &pwsz );

	// Read name
	UINT cwch = ::ParseContinuousString( pwsz );
	if( !cwch )
	{
		*ppwsz = pwsz;
		return S_FALSE;
	}

	WCHAR* wstrName = new wchar_t[cwch + 1];
	if( !wstrName )
	{
		return E_OUTOFMEMORY;
	}
	wcsncpy( wstrName, pwsz, cwch );
	wstrName[cwch] = L'\0';
	pwsz += cwch;

	// Skip =
	::ParseSkipWhitespace( &pwsz );
	if( *pwsz++ != L'=' )
	{
		return E_FAIL;
	}
	::ParseSkipWhitespace( &pwsz );

	// Read value
	HRESULT hr = S_OK;
	if( *pwsz == L'"' )
	{
		cwch = ::ParseQuotedString( pwsz );
		if( !cwch )
		{
			return E_FAIL;
		}
		WCHAR* wstrValue = new wchar_t[cwch + 1];
		if( !wstrValue )
		{
			return E_OUTOFMEMORY;
		}
		hr = ::ReformatQuotedString( pwsz, wstrValue );
		if( SUCCEEDED ( hr ) )
		{
			wstrValue[cwch] = L'\0';
			pwsz += cwch;

			hr = FoundField( wstrName, wstrValue, pListInfo );
		}
		delete[] wstrValue;
	}
	else
	{
		cwch = ::ParseContinuousString( pwsz );
		if( !cwch )
		{
			return E_FAIL;
		}
		WCHAR* wstrValue = new wchar_t[cwch + 1];
		if( !wstrValue )
		{
			return E_OUTOFMEMORY;
		}
		wcsncpy( wstrValue, pwsz, cwch );
		wstrValue[cwch] = L'\0';
		pwsz += cwch;
		hr = FoundField( wstrName, wstrValue, pListInfo );
		delete[] wstrValue;
	}

	delete[] wstrName;

	if( SUCCEEDED ( hr ) )
	{
		*ppwsz = pwsz;
	}

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicScript::ParseHeaderBlock

HRESULT CDirectMusicScript::ParseHeaderBlock( const WCHAR** ppwszSource, DMUSProdListInfo* pListInfo )
{
	if( !ppwszSource )
	{
		ASSERT( 0 );
		return E_FAIL;
	}
	const WCHAR *pwsz = *ppwszSource;

	::ParseSkipWhitespace( &pwsz );

	// Find opening tag
	static const WCHAR wszOpenBlock[] = L"<SCRIPT";
	static const UINT cwchOpenBlock = wcslen( wszOpenBlock );
	if( 0 != _wcsnicmp( pwsz, wszOpenBlock, cwchOpenBlock ) )
	{
		return E_FAIL;
	}
	pwsz += cwchOpenBlock;

	// Read fields
	HRESULT hr = S_OK;
	do
	{
		hr = ParseField( &pwsz, pListInfo );
	} while( hr == S_OK );

	if( FAILED( hr ) )
	{
		return hr;
	}

	// Find closing brace
	if( *pwsz++ != L'>' )
	{
		return E_FAIL;
	}

	*ppwszSource = pwsz;
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicScript::ReadSourceFromStream

HRESULT CDirectMusicScript::ReadSourceFromStream( IStream* pIStream )
{
	WCHAR* wstrSource;
	const WCHAR* wstrSourceAfterHeader;

	// Record the stream's current position
	LARGE_INTEGER li;
	ULARGE_INTEGER ulStart;
	ULARGE_INTEGER ulEnd;
	li.HighPart = 0;
	li.LowPart = 0;

	HRESULT hr = pIStream->Seek( li, STREAM_SEEK_CUR, &ulStart );
	if( FAILED ( hr ) )
	{
		return hr;
	}

	ASSERT( ulStart.HighPart == 0 );	// We don't expect streams that big.
	DWORD dwSavedPos = ulStart.LowPart;

	// Get the stream's end and record the total size
	hr = pIStream->Seek( li, STREAM_SEEK_END, &ulEnd );
	if( FAILED ( hr ) )
	{
		return hr;
	}

	ASSERT( ulEnd.HighPart == 0 );
	ASSERT( ulEnd.LowPart > dwSavedPos );
	DWORD cch = ulEnd.LowPart - dwSavedPos;

	// Go back to the start and copy the characters
	li.HighPart = 0;
	li.LowPart = dwSavedPos;
	hr = pIStream->Seek( li, STREAM_SEEK_SET, &ulStart );
	if( FAILED ( hr ) )
	{
		return hr;
	}
	ASSERT( ulStart.LowPart == dwSavedPos );

	CHAR *paszSource = new CHAR[cch + 1];
	if( !paszSource )
	{
		return E_OUTOFMEMORY;
	}

	DWORD cbRead;
	hr = pIStream->Read( paszSource, cch, &cbRead );
	if( FAILED ( hr ) )
	{
		ASSERT( 0 );
		return hr;
	}

	paszSource[cch] = '\0';

	// Convert the script source to wide characters
	wstrSource = new wchar_t[cch + 1];
	if( !wstrSource )
	{
		delete[] paszSource;
		return E_OUTOFMEMORY;
	}
	MultiByteToWideChar( CP_ACP, 0, paszSource, -1, wstrSource, cch + 1 );
	delete[] paszSource;

	// Store original name
	CString strOrigName = m_strName;

	// Initialize some fields
	m_strName.Empty();
	m_strAuthor.Empty();
	m_strSubject.Empty();
	m_strCopyright.Empty();

	// Parse the script header block
	wstrSourceAfterHeader = wstrSource;
	hr = ParseHeaderBlock( &wstrSourceAfterHeader, NULL );
	if( m_vDirectMusicVersion.dwVersionMS < 0x00080000 )
	{
		return E_FAIL;
	}

	if( m_strName.IsEmpty() )
	{
		// Script file does not contain name so use script's filename
		IDMUSProdPersistInfo* pIPersistInfo;

		if( SUCCEEDED ( pIStream->QueryInterface( IID_IDMUSProdPersistInfo, (void **)&pIPersistInfo ) ) )
		{
			BSTR bstrFileName;

			if( SUCCEEDED ( pIPersistInfo->GetFileName( &bstrFileName ) ) )
			{
				TCHAR achFName[_MAX_FNAME];

				CString strFileName = bstrFileName;
				::SysFreeString( bstrFileName );

				_tsplitpath( strFileName, NULL, NULL, achFName, NULL );
				m_strName = achFName;
			}
			RELEASE( pIPersistInfo );
		}

		if( m_strName.IsEmpty() )
		{
			// Can't get filename so put back orig name
			m_strName = strOrigName;
		}
	}

	if( strOrigName.CompareNoCase( m_strName ) != 0 )
	{
		theApp.m_pScriptComponent->m_pIFramework->RefreshNode( this );

		// Notify connected nodes that Script name has changed
		theApp.m_pScriptComponent->m_pIFramework->NotifyNodes( this, SCRIPT_NameChange, NULL );
	}

	while( (*wstrSourceAfterHeader == L'\r')  || (*wstrSourceAfterHeader == L'\n') )
	{
		wstrSourceAfterHeader++;
	}
	
	m_strSource = wstrSourceAfterHeader;
	delete[] wstrSource;

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicScript::ReadListInfoFromStream

HRESULT CDirectMusicScript::ReadListInfoFromStream( IStream* pIStream, DMUSProdListInfo* pListInfo )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

    IDMUSProdRIFFStream* pIRiffStream;
    MMCKINFO ckMain;
	MMCKINFO ck;
	DWORD dwSize;
	DWORD dwByteCount;
	CString strName;
	CString strDescriptor;
	GUID guidContainer;

	ASSERT( pIStream != NULL );
    ASSERT( pListInfo != NULL );

	if( pIStream == NULL
	||  pListInfo == NULL )
	{
		return E_INVALIDARG;
	}

	memset( &guidContainer, 0, sizeof(GUID) );

    DWORD dwPos = StreamTell( pIStream );

	// Check for Direct Music format
	BOOL fFoundFormat = FALSE;
	if( SUCCEEDED( AllocRIFFStream( pIStream, &pIRiffStream ) ) )
	{
		ckMain.fccType = DMUS_FOURCC_SCRIPT_FORM;

		if( pIRiffStream->Descend( &ckMain, NULL, MMIO_FINDRIFF ) == 0 )
		{
			fFoundFormat = TRUE;

			// Get Script GUID
			ck.ckid = DMUS_FOURCC_GUID_CHUNK;

			if( pIRiffStream->Descend( &ck, &ckMain, MMIO_FINDCHUNK ) == 0 )
			{
				dwSize = min( ck.cksize, sizeof( GUID ) );
				if( FAILED ( pIStream->Read( &guidContainer, dwSize, &dwByteCount ) )
				||  dwByteCount != dwSize )
				{
					memset( &guidContainer, 0, sizeof(GUID) );
				}
			}

			// Get Script name
			ck.fccType = DMUS_FOURCC_UNFO_LIST;

			if( pIRiffStream->Descend( &ck, &ckMain, MMIO_FINDLIST ) == 0 )
			{
				ck.ckid = DMUS_FOURCC_UNAM_CHUNK;
				if( pIRiffStream->Descend( &ck, &ckMain, MMIO_FINDCHUNK ) == 0 )
				{
					ReadMBSfromWCS( pIStream, ck.cksize, &strName );
				}
			}
		}

		RELEASE( pIRiffStream );
	}

	if( fFoundFormat == FALSE )
	{
		// Check for legacy ascii text file format
	    StreamSeek( pIStream, dwPos, STREAM_SEEK_SET );
		return ReadListInfoFromLegacyStream( pIStream, pListInfo );
	}

	if( !strName.IsEmpty() )
	{
		pListInfo->bstrName = strName.AllocSysString();
		pListInfo->bstrDescriptor = strDescriptor.AllocSysString();
		memcpy( &pListInfo->guidObject, &guidContainer, sizeof(GUID) );

		// Must check pListInfo->wSize before populating additional fields
		return S_OK;
	}

	return E_FAIL;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicScript::ReadListInfoFromLegacyStream

HRESULT CDirectMusicScript::ReadListInfoFromLegacyStream( IStream* pIStream, DMUSProdListInfo* pListInfo )
{
	ASSERT( pIStream != NULL );
    ASSERT( pListInfo != NULL );

	if( pIStream == NULL
	||  pListInfo == NULL )
	{
		return E_INVALIDARG;
	}

	// Initialize pListInfo fields
	pListInfo->bstrName = NULL;
	pListInfo->bstrDescriptor = NULL;
	memset( &pListInfo->guidObject, 0, sizeof(GUID) );

	WCHAR* wstrSource;
	const WCHAR* wstrSourceAfterHeader;

	// Record the stream's current position
	LARGE_INTEGER li;
	ULARGE_INTEGER ulStart;
	ULARGE_INTEGER ulEnd;
	li.HighPart = 0;
	li.LowPart = 0;

	HRESULT hr = pIStream->Seek( li, STREAM_SEEK_CUR, &ulStart );
	if( FAILED ( hr ) )
	{
		return hr;
	}

	ASSERT( ulStart.HighPart == 0 );	// We don't expect streams that big.
	DWORD dwSavedPos = ulStart.LowPart;

	// Get the stream's end and record the total size
	hr = pIStream->Seek( li, STREAM_SEEK_END, &ulEnd );
	if( FAILED ( hr ) )
	{
		return hr;
	}

	ASSERT( ulEnd.HighPart == 0 );
	ASSERT( ulEnd.LowPart > dwSavedPos );
	DWORD cch = ulEnd.LowPart - dwSavedPos;

	// Go back to the start and copy the characters
	li.HighPart = 0;
	li.LowPart = dwSavedPos;
	hr = pIStream->Seek( li, STREAM_SEEK_SET, &ulStart );
	if( FAILED ( hr ) )
	{
		return hr;
	}
	ASSERT( ulStart.LowPart == dwSavedPos );

	CHAR *paszSource = new CHAR[cch + 1];
	if( !paszSource )
	{
		return E_OUTOFMEMORY;
	}

	DWORD cbRead;
	hr = pIStream->Read( paszSource, cch, &cbRead );
	if( FAILED ( hr ) )
	{
		ASSERT( 0 );
		return hr;
	}

	paszSource[cch] = '\0';

	// Convert the script source to wide characters
	wstrSource = new wchar_t[cch + 1];
	if( !wstrSource )
	{
		delete[] paszSource;
		return E_OUTOFMEMORY;
	}
	MultiByteToWideChar( CP_ACP, 0, paszSource, -1, wstrSource, cch + 1 );
	delete[] paszSource;

	// Initialize name
	m_strName.Empty();

	// Parse the script header block
	wstrSourceAfterHeader = wstrSource;
	hr = ParseHeaderBlock( &wstrSourceAfterHeader, pListInfo );
	if( m_vDirectMusicVersion.dwVersionMS < 0x00080000 )
	{
		return E_FAIL;
	}

	if( m_strName.IsEmpty() )
	{
		// Script file does not contain name so use script's filename
		IDMUSProdPersistInfo* pIPersistInfo;

		if( SUCCEEDED ( pIStream->QueryInterface( IID_IDMUSProdPersistInfo, (void **)&pIPersistInfo ) ) )
		{
			BSTR bstrFileName;

			if( SUCCEEDED ( pIPersistInfo->GetFileName( &bstrFileName ) ) )
			{
				TCHAR achFName[_MAX_FNAME];

				CString strFileName = bstrFileName;
				::SysFreeString( bstrFileName );

				_tsplitpath( strFileName, NULL, NULL, achFName, NULL );
				m_strName = achFName;
				pListInfo->bstrName = m_strName.AllocSysString();
			}

			RELEASE( pIPersistInfo );
		}
	}

	// Can't get a name!
	if( m_strName.IsEmpty() )
	{
		ASSERT( 0 );	// Should not happen
		hr = E_FAIL;
	}

	delete[] wstrSource;

	return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicScript::GetObjectDescriptor

HRESULT CDirectMusicScript::GetObjectDescriptor( void* pObjectDesc )
{
	// Make sure method was passed a valid DMUS_OBJECTDESC pointer
	if( pObjectDesc == NULL ) 
	{
		ASSERT( FALSE );
		return E_POINTER;
	}

	DMUS_OBJECTDESC *pDMObjectDesc = (DMUS_OBJECTDESC *)pObjectDesc;

	if( pDMObjectDesc->dwSize == 0 ) 
	{
		ASSERT( FALSE );
		return E_INVALIDARG;
	}

	// Initialize DMUS_OBJECTDESC structure
	DWORD dwOrigSize = pDMObjectDesc->dwSize;
	memset( pDMObjectDesc, 0, dwOrigSize );
	pDMObjectDesc->dwSize = dwOrigSize;

	// Set values in DMUS_OBJECTDESC structure
	pDMObjectDesc->dwValidData = (DMUS_OBJ_OBJECT | DMUS_OBJ_CLASS | DMUS_OBJ_VERSION | DMUS_OBJ_NAME );
	
	memcpy( &pDMObjectDesc->guidObject, &m_guidScript, sizeof(GUID) );
	memcpy( &pDMObjectDesc->guidClass, &CLSID_DirectMusicScript, sizeof(CLSID) );
	pDMObjectDesc->vVersion.dwVersionMS = m_vVersion.dwVersionMS;
	pDMObjectDesc->vVersion.dwVersionLS = m_vVersion.dwVersionLS;
	MultiByteToWideChar( CP_ACP, 0, m_strName, -1, pDMObjectDesc->wszName, DMUS_MAX_NAME );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicScript::GetGUID

void CDirectMusicScript::GetGUID( GUID* pguidScript )
{
	if( pguidScript )
	{
		*pguidScript = m_guidScript;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicScript::SetGUID

void CDirectMusicScript::SetGUID( GUID guidScript )
{
	m_pUndoMgr->SaveState( this, theApp.m_hInstance, IDS_UNDO_SCRIPT_GUID );
	m_guidScript = guidScript;

	// Sync Script with DirectMusic
	SetModified( TRUE );
	SyncScriptWithDirectMusic();

	// Notify connected nodes that Script GUID has changed
	theApp.m_pScriptComponent->m_pIFramework->NotifyNodes( this, DOCROOT_GuidChange, NULL );
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicScript::GetVariableState

void CDirectMusicScript::GetVariableState( CTypedPtrList<CPtrList, VariableState*>& list )
{
	if( m_pIDMScript == NULL )
	{
		// Nothing to do
		return;
	}

	// Make sure the list is empty
	ASSERT( list.IsEmpty() );	// Should be empty!
	while( !list.IsEmpty() )
	{
		VariableState* pVariableState = static_cast<VariableState*>( list.RemoveHead() );
		delete pVariableState;
	}

	WCHAR awchVariableName[MAX_PATH];
	DMUS_SCRIPT_ERRORINFO ErrorInfo;

	int i = 0;
	CString strVariableName;
	BSTR bstrVariableName;
	bool fBypassVariable;

	while( m_pIDMScript->EnumVariable( i++, awchVariableName ) == S_OK )
	{
		// Bypass variables that are actually items in the Script's Container
		fBypassVariable = false;
		if( m_pIContainerNode )
		{
			IDMUSProdContainerInfo* pIContainerInfo;
			if( SUCCEEDED ( m_pIContainerNode->QueryInterface( IID_IDMUSProdContainerInfo, (void**)&pIContainerInfo ) ) )
			{
				strVariableName = awchVariableName;
				bstrVariableName = strVariableName.AllocSysString();

				IUnknown* pIDocRootNode;
				if( pIContainerInfo->FindDocRootFromScriptAlias(bstrVariableName, &pIDocRootNode) == S_OK )
				{
					fBypassVariable = true;
					RELEASE( pIDocRootNode );
				}

				RELEASE( pIContainerInfo );
			}
		}
		if( fBypassVariable )
		{
			continue;
		}

		VariableState* pVariableState = new VariableState;
		if( pVariableState )
		{
			// Store variable name
			pVariableState->m_strName = awchVariableName;

			// Initialize ErrorInfo struct
			memset( &ErrorInfo, 0, sizeof(DMUS_SCRIPT_ERRORINFO) );
			ErrorInfo.dwSize = sizeof(DMUS_SCRIPT_ERRORINFO);

			// Get current value
			HRESULT hr = m_pIDMScript->GetVariableVariant( awchVariableName, &pVariableState->m_Variant, &ErrorInfo );
			if( SUCCEEDED ( hr ) )
			{
				list.AddTail( pVariableState );
			}
			else 
			{
				DisplayScriptError( &ErrorInfo, hr );
				delete pVariableState;
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicScript::SetVariableState

void CDirectMusicScript::SetVariableState( CTypedPtrList<CPtrList, VariableState*>& list )
{
	if( m_pIDMScript == NULL )
	{
		// Nothing to do
		return;
	}

	HRESULT hr;

	WCHAR awchVariableName[MAX_PATH];
	DMUS_SCRIPT_ERRORINFO ErrorInfo;

	POSITION pos = list.GetHeadPosition();
	while( pos )
	{
		VariableState* pVariableState = list.GetNext( pos );

		// Convert the variable name to wide characters
		MultiByteToWideChar( CP_ACP, 0, pVariableState->m_strName, -1, awchVariableName, MAX_PATH );

		// Initialize ErrorInfo struct
		memset( &ErrorInfo, 0, sizeof(DMUS_SCRIPT_ERRORINFO) );
		ErrorInfo.dwSize = sizeof(DMUS_SCRIPT_ERRORINFO);

		BOOL fSetRef = FALSE;
		if( pVariableState->m_Variant.vt == VT_UNKNOWN
		||  pVariableState->m_Variant.vt == VT_DISPATCH )
		{
			fSetRef = TRUE;
		}

		hr = m_pIDMScript->SetVariableVariant( awchVariableName, pVariableState->m_Variant, fSetRef, &ErrorInfo );
		if( hr == DMUS_E_SCRIPT_ERROR_IN_SCRIPT )
		{
			// At this time only report DMUS_E_SCRIPT_ERROR_IN_SCRIPT errors
			DisplayScriptError( &ErrorInfo, hr );
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicScript::DisplayScriptError

void CDirectMusicScript::DisplayScriptError( DMUS_SCRIPT_ERRORINFO* pErrorInfo, HRESULT hrFromScript )
{
	ASSERT( pErrorInfo != NULL );

	if( hrFromScript != DMUS_E_SCRIPT_ERROR_IN_SCRIPT )
	{
		// Set FileName
		BSTR bstrFileName;
		if( SUCCEEDED ( theApp.m_pScriptComponent->m_pIFramework->GetNodeFileName( this, &bstrFileName ) ) )
		{
			CString strFileName = bstrFileName;
			::SysFreeString( bstrFileName );
			int nFindPos = strFileName.ReverseFind( (TCHAR)'\\' );
			if( nFindPos != -1 )
			{
				strFileName = strFileName.Right( strFileName.GetLength() - nFindPos - 1 );
			}
			MultiByteToWideChar( CP_ACP, 0, strFileName, -1, pErrorInfo->wszSourceFile, DMUS_MAX_FILENAME );
		}

		// Set HRESULT
		pErrorInfo->hr = hrFromScript;
	}

	IDMUSProdComponent* pIComponent;
	if( SUCCEEDED ( theApp.m_pScriptComponent->m_pIFramework->FindComponent( CLSID_ScriptComponent,  &pIComponent ) ) )
	{
		IDMUSProdDebugScript* pIDebugScript;
		if( SUCCEEDED ( pIComponent->QueryInterface( IID_IDMUSProdDebugScript, (void**)&pIDebugScript ) ) )
		{
			pIDebugScript->DisplayScriptError( pErrorInfo );

			RELEASE( pIDebugScript );
		}
	
		RELEASE( pIComponent );
	}
}
