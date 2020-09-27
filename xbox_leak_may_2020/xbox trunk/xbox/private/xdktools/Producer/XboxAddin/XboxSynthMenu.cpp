// XboxAddinComponent.cpp : implementation file
//

#include "stdafx.h"
#include "XboxAddin.h"
#include "XboxAddinComponent.h"
#include "XboxSynthMenu.h"
#include "XboxSynthConfigDlg.h"
#pragma warning ( push )
#pragma warning ( disable : 4201 )
#include <dmusici.h>
#pragma warning ( pop )


/////////////////////////////////////////////////////////////////////////////
// CXboxSynthMenu constructor/destructor 

CXboxSynthMenu::CXboxSynthMenu()
{
    m_dwRef = 0;
	AddRef();
}

CXboxSynthMenu::~CXboxSynthMenu()
{
}


/////////////////////////////////////////////////////////////////////////////
// CXboxSynthMenu IUnknown implementation

HRESULT CXboxSynthMenu::QueryInterface( REFIID riid, LPVOID FAR* ppvObj )
{
    if( ::IsEqualIID(riid, IID_IDMUSProdMenu)
	||  ::IsEqualIID(riid, IID_IUnknown) )
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

ULONG CXboxSynthMenu::AddRef()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	AfxOleLockApp(); 
    return ++m_dwRef;
}

ULONG CXboxSynthMenu::Release()
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
// CXboxSynthMenu IDMUSProdMenu implementation

/////////////////////////////////////////////////////////////////////////////
// CXboxSynthMenu IDMUSProdMenu::GetMenuText

HRESULT CXboxSynthMenu::GetMenuText( BSTR* pbstrText )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CString strName;
	if( strName.LoadString( IDS_XBOX_SYNTH_MENU_TEXT ) )
	{
	    *pbstrText = strName.AllocSysString();
		return S_OK;
	}
	
	return E_FAIL;
}


/////////////////////////////////////////////////////////////////////////////
// CXboxSynthMenu IDMUSProdMenu::GetMenuHelpText

HRESULT CXboxSynthMenu::GetMenuHelpText( BSTR* pbstrHelpText )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CString strName;
	if( strName.LoadString( IDS_XBOX_SYNTH_MENU_HELP_TEXT ) )
	{
	    *pbstrHelpText = strName.AllocSysString();
		return S_OK;
	}
	
	return E_FAIL;
}


/////////////////////////////////////////////////////////////////////////////
// CXboxSynthMenu IDMUSProdMenu::OnMenuInit

HRESULT CXboxSynthMenu::OnMenuInit( HMENU hMenu, UINT nMenuID )
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	::EnableMenuItem( hMenu, nMenuID, MF_ENABLED );

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CXboxSynthMenu IDMUSProdMenu::OnMenuSelect

HRESULT CXboxSynthMenu::OnMenuSelect()
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	CXboxSynthConfigDlg xboxSynthConfigDlg;
	if( SUCCEEDED( theApp.m_pXboxAddinComponent->DoXboxSynthState( false, &xboxSynthConfigDlg.m_bXboxSynth, &xboxSynthConfigDlg.m_bPCSynth ) ) )
	{
		xboxSynthConfigDlg.DoModal();
	}
	else
	{
		// Check if we even have an audiopath
		IDirectMusicAudioPath *pIDirectMusicAudioPath = NULL;
		theApp.m_pXboxAddinComponent->m_pIDMPerformance->GetDefaultAudioPath( &pIDirectMusicAudioPath );

		CString strErrorMsg;
		if( pIDirectMusicAudioPath )
		{
			pIDirectMusicAudioPath->Release();
			strErrorMsg.LoadString( IDS_ERR_NO_XBOX_SYNTH );
		}
		else
		{
			strErrorMsg.LoadString( IDS_ERR_NO_AUDIOPATH );
		}

		CString strWindowTitle;
		strWindowTitle.LoadString( IDS_XBOX_SYNTH_TITLE );
		XboxAddinMessageBox( NULL, strErrorMsg, MB_ICONERROR | MB_OK, strWindowTitle );
	}

	return S_OK;
}
