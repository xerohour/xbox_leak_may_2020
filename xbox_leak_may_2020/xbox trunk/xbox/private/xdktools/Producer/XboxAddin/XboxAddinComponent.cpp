// XboxAddinComponent.cpp : implementation file
//

#include "stdafx.h"

#include "XboxAddin.h"
#include "XboxAddinDlg.h"
#include <initguid.h>
#include "XboxAddinComponent.h"
#include <dmpprivate.h>
#include "Segment.h"
#include "Audiopath.h"
#include "OtherFile.h"
#pragma warning ( push )
#pragma warning ( disable : 4201 )
#include <dmusici.h>
#pragma warning ( pop )
#include <dmksctrl.h>
#include "XboxSynthMenu.h"


/////////////////////////////////////////////////////////////////////////////
// CXboxAddinComponent constructor/destructor 

CXboxAddinComponent::CXboxAddinComponent()
{
    m_dwRef = 0;
	m_pIFramework = NULL;
	m_pIConductor = NULL;
	m_pIDMPerformance = NULL;
	m_fMenuWasAdded = FALSE;
	m_fSynthMenuWasAdded = FALSE;
	m_pXboxAddinDlg = NULL;
	m_pXboxAddinDlgWP = NULL;
	m_pXboxSynthMenu = NULL;
}

CXboxAddinComponent::~CXboxAddinComponent()
{
	if( m_pXboxAddinDlgWP )
	{
		delete m_pXboxAddinDlgWP;
		m_pXboxAddinDlgWP = NULL;
	}

	ReleaseAll();
}


/////////////////////////////////////////////////////////////////////////////
// CXboxAddinComponent::ReleaseAll

void CXboxAddinComponent::ReleaseAll( void )
{
	RELEASE( m_pIDMPerformance );
	RELEASE( m_pIConductor );
	RELEASE( m_pIFramework );

	CleanUp();
}


/////////////////////////////////////////////////////////////////////////////
// CXboxAddinComponent IUnknown implementation

HRESULT CXboxAddinComponent::QueryInterface( REFIID riid, LPVOID FAR* ppvObj )
{
    if( ::IsEqualIID(riid, IID_IDMUSProdComponent)
	||  ::IsEqualIID(riid, IID_IUnknown) )
    {
        *ppvObj = (IDMUSProdComponent *)this;
    }
    else if( ::IsEqualIID(riid, IID_IDMUSProdMenu) )
    {
        *ppvObj = (IDMUSProdMenu *)this;
    }
	else
	{
	    *ppvObj = NULL;
		return E_NOINTERFACE;
	}

	AddRef();
	return S_OK;
}

ULONG CXboxAddinComponent::AddRef()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	AfxOleLockApp(); 
    return ++m_dwRef;
}

ULONG CXboxAddinComponent::Release()
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
// CXboxAddinComponent IDMUSProdComponent implementation

/////////////////////////////////////////////////////////////////////////////
// CXboxAddinComponent IDMUSProdComponent::Initialize

HRESULT CXboxAddinComponent::Initialize( IDMUSProdFramework* pIFramework, BSTR* pbstrErrMsg )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pIFramework )		// already initialized
	{
		return S_OK;
	}

	CString strErrMsg;

	ASSERT( pIFramework != NULL );
	ASSERT( pbstrErrMsg != NULL );

	if( pbstrErrMsg == NULL )
	{
		return E_POINTER;
	}

	if( pIFramework == NULL )
	{
		strErrMsg.LoadString( IDS_ERR_INVALIDARG );
		*pbstrErrMsg = strErrMsg.AllocSysString();
		return E_POINTER;
	}

	IDMUSProdComponent* pIComponent = NULL;
	IUnknown* punkPerformance = NULL;

	// Get IConductor and IDirectMusicPerformance interface pointers 
	if( !SUCCEEDED ( pIFramework->FindComponent( CLSID_CConductor,  &pIComponent ) )
	||  !SUCCEEDED ( pIComponent->QueryInterface( IID_IDMUSProdConductor8, (void**)&m_pIConductor ) )
	||  !SUCCEEDED ( m_pIConductor->GetPerformanceEngine( (IUnknown**)&punkPerformance ) )
	||	!SUCCEEDED ( punkPerformance->QueryInterface( IID_IDirectMusicPerformance8, (void**)&m_pIDMPerformance ) ) )
	{
		ReleaseAll();
		if( pIComponent )
		{
			pIComponent->Release();
		}
		if( punkPerformance )
		{
			punkPerformance->Release();
		}
		strErrMsg.LoadString( IDS_ERR_MISSING_CONDUCTOR );
		*pbstrErrMsg = strErrMsg.AllocSysString();
		return E_FAIL;
	}

	RELEASE( pIComponent );
	RELEASE( punkPerformance );

	pIFramework->QueryInterface( IID_IDMUSProdFramework8, (void **)&m_pIFramework );

	theApp.m_pXboxAddinComponent = this;
//	theApp.m_pXboxAddinComponent->AddRef();	intentionally missing

	// Add "Message Window" item to Add-Ins menu 
	if( FAILED ( pIFramework->AddMenuItem( (IDMUSProdMenu *)this ) ) )
	{
		ReleaseAll();
		strErrMsg.LoadString( IDS_ERR_ADD_MENUITEM );
		*pbstrErrMsg = strErrMsg.AllocSysString();
		return E_FAIL;
	}
	else
	{
		m_fMenuWasAdded = TRUE;
	}

	ASSERT( !m_pXboxSynthMenu );
	m_pXboxSynthMenu = new CXboxSynthMenu;
	if( m_pXboxSynthMenu )
	{
		if( SUCCEEDED( pIFramework->AddMenuItem( (IDMUSProdMenu *)m_pXboxSynthMenu ) ) )
		{
			m_fSynthMenuWasAdded = TRUE;
		}
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CXboxAddinComponent IDMUSProdComponent::CleanUp

HRESULT CXboxAddinComponent::CleanUp( void )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	// Close "Experimenter Window"
	if( m_pXboxAddinDlg )
	{
		m_pXboxAddinDlg->DestroyWindow();
		delete m_pXboxAddinDlg;
		m_pXboxAddinDlg = NULL;
	}

	// Delete items from the lists
	while( !m_lstPrimarySegments.IsEmpty() )
	{
		delete m_lstPrimarySegments.RemoveHead();
	}
	while( !m_lstSecondarySegments.IsEmpty() )
	{
		delete m_lstSecondarySegments.RemoveHead();
	}
	while( !m_lstOtherFiles.IsEmpty() )
	{
		delete m_lstOtherFiles.RemoveHead();
	}
	while( !m_lstAudiopaths.IsEmpty() )
	{
		delete m_lstAudiopaths.RemoveHead();
	}


	// Remove "Experimenter Window" menu item
	if( m_fMenuWasAdded )
	{
		m_pIFramework->RemoveMenuItem( (IDMUSProdMenu *)this );
		m_fMenuWasAdded = FALSE;
	}

	if( m_fSynthMenuWasAdded )
	{
		m_pIFramework->RemoveMenuItem( (IDMUSProdMenu *)m_pXboxSynthMenu );
		m_fSynthMenuWasAdded = FALSE;
	}

	RELEASE(m_pXboxSynthMenu);

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CXboxAddinComponent IDMUSProdComponent::GetName

HRESULT CXboxAddinComponent::GetName( BSTR* pbstrName )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CString strName;
	if( strName.LoadString( IDS_XBOXADDIN_COMPONENT_NAME ) )
	{
	    *pbstrName = strName.AllocSysString();
		return S_OK;
	}
	
	return E_FAIL;
}


/////////////////////////////////////////////////////////////////////////////
// CXboxAddinComponent::IDMUSProdComponent::AllocReferenceNode

HRESULT CXboxAddinComponent::AllocReferenceNode( GUID guidRefNodeId, IDMUSProdNode** ppIRefNode )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(guidRefNodeId);
	UNREFERENCED_PARAMETER(ppIRefNode);
	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CXboxAddinComponent IDMUSProdComponent::OnActivateApp

HRESULT CXboxAddinComponent::OnActivateApp( BOOL fActivate )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(fActivate);

	return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CXboxAddinComponent IDMUSProdMenu implementation

/////////////////////////////////////////////////////////////////////////////
// CXboxAddinComponent IDMUSProdMenu::GetMenuText

HRESULT CXboxAddinComponent::GetMenuText( BSTR* pbstrText )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CString strName;
	if( strName.LoadString( IDS_EXPERIMENTER_WINDOW_MENU_TEXT ) )
	{
	    *pbstrText = strName.AllocSysString();
		return S_OK;
	}
	
	return E_FAIL;
}


/////////////////////////////////////////////////////////////////////////////
// CXboxAddinComponent IDMUSProdMenu::GetMenuHelpText

HRESULT CXboxAddinComponent::GetMenuHelpText( BSTR* pbstrHelpText )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CString strName;
	if( strName.LoadString( IDS_EXPERIMENTER_WINDOW_MENU_HELP_TEXT ) )
	{
	    *pbstrHelpText = strName.AllocSysString();
		return S_OK;
	}
	
	return E_FAIL;
}


/////////////////////////////////////////////////////////////////////////////
// CXboxAddinComponent IDMUSProdMenu::OnMenuInit

HRESULT CXboxAddinComponent::OnMenuInit( HMENU hMenu, UINT nMenuID )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	::EnableMenuItem( hMenu, nMenuID, MF_ENABLED );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CXboxAddinComponent IDMUSProdMenu::OnMenuSelect

HRESULT CXboxAddinComponent::OnMenuSelect()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if( m_pXboxAddinDlg )
	{
		// Window is already open
		if( m_pXboxAddinDlg->IsIconic() ) 
		{
			m_pXboxAddinDlg->ShowWindow( SW_RESTORE );     // If iconic, restore the main window
		}
		m_pXboxAddinDlg->SetForegroundWindow();
		return S_OK;
	}

	// Create "Experimenter Window"
	m_pXboxAddinDlg = new CXboxAddinDlg;
	if( m_pXboxAddinDlg == NULL )
	{
		return E_OUTOFMEMORY;
	}

	CString strWindowTitle;
	strWindowTitle.LoadString( IDS_EXPERIMENTER_WINDOW_MENU_TEXT );
	if( m_pXboxAddinDlg->CreateEx( WS_EX_TOPMOST, AfxRegisterWndClass(0), strWindowTitle,
							   (WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_CLIPCHILDREN),
							   0, 0,
							   200, 400,
							   NULL	/*pParentWnd*/,
							   NULL,
							   NULL ) )
	{
		HICON hIcon = ::LoadIcon( theApp.m_hInstance, MAKEINTRESOURCE(IDR_MAINFRAME) );
		if( hIcon )
		{
			m_pXboxAddinDlg->SetIcon( hIcon, TRUE );
		}
		if( m_pXboxAddinDlgWP )
		{
			m_pXboxAddinDlg->SetWindowPlacement( m_pXboxAddinDlgWP );
		}
		m_pXboxAddinDlg->OnInitDialog();
		return S_OK;
	}

	delete m_pXboxAddinDlg;
	m_pXboxAddinDlg = NULL;
	return E_FAIL;
}


/////////////////////////////////////////////////////////////////////////////
// CXboxAddinComponent additional functions

HRESULT CXboxAddinComponent::EnsureNodeIsCopied( IDMUSProdNode *pIDMUSProdNode )
{
	if( pIDMUSProdNode == NULL )
	{
		return E_POINTER;
	}

	if( m_pXboxAddinDlg->IsNodeDisplayed( pIDMUSProdNode ) )
	{
		return S_OK;
	}

	return m_pXboxAddinDlg->AddNodeToDisplay( pIDMUSProdNode );
}

HRESULT CXboxAddinComponent::DoXboxSynthState( bool fSet, BOOL *pbXboxEnabled, BOOL *pbPCEnabled )
{
    if( fSet && (!pbXboxEnabled || !pbPCEnabled) )
    {
        return E_POINTER;
    }

	BOOL bXboxEnabled, bPCEnabled;
	if( !pbXboxEnabled )
	{
		pbXboxEnabled = &bXboxEnabled;
	}
	if( !pbPCEnabled )
	{
		pbPCEnabled = &bPCEnabled;
	}

	IDirectMusicAudioPath *pIDirectMusicAudioPath = NULL;
	HRESULT hr = m_pIDMPerformance->GetDefaultAudioPath( &pIDirectMusicAudioPath );

	IKsControl *pIKsControl = NULL;
	if( SUCCEEDED(hr) )
	{
		hr = pIDirectMusicAudioPath->GetObjectInPath( DMUS_PCHANNEL_ALL, DMUS_PATH_PORT, 0, CLSID_XboxSynth, 0, IID_IKsControl, (void **)&pIKsControl );
	}

	if( SUCCEEDED(hr) )
	{
		KSPROPERTY ksProperty;
		ksProperty.Set = GUID_Xbox_PROP_XboxSynth;
        ksProperty.Flags = fSet ? KSPROPERTY_TYPE_SET : KSPROPERTY_TYPE_GET;
		ksProperty.Id = 0;
		DWORD dwDataSize = 0;
		hr = pIKsControl->KsProperty( &ksProperty, sizeof(KSPROPERTY), pbXboxEnabled, sizeof(BOOL), &dwDataSize ); 
	}

	if( SUCCEEDED(hr) )
	{
		KSPROPERTY ksProperty;
		ksProperty.Set = GUID_Xbox_PROP_PCSynth;
		ksProperty.Flags = fSet ? KSPROPERTY_TYPE_SET : KSPROPERTY_TYPE_GET;
		ksProperty.Id = 0;
		DWORD dwDataSize = 0;
		hr = pIKsControl->KsProperty( &ksProperty, sizeof(KSPROPERTY), pbPCEnabled, sizeof(BOOL), &dwDataSize ); 
	}
	
	if( pIKsControl )
	{
		pIKsControl->Release();
	}

	if( pIDirectMusicAudioPath )
	{
		pIDirectMusicAudioPath->Release();
	}

	return hr;
}
