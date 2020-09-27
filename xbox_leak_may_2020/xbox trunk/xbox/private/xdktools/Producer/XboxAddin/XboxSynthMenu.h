#ifndef __XBOXSYNTHMENU_H__
#define __XBOXSYNTHMENU_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "dmusprod.h"

// XboxSynthMenu.h : header file
//

////////////////////////////////////////////////////////////////////////////////
class CXboxSynthMenu : public IDMUSProdMenu
{
public:
	CXboxSynthMenu();
	~CXboxSynthMenu();

    // IUnknown functions
    HRESULT STDMETHODCALLTYPE QueryInterface( REFIID riid, LPVOID *ppv );
    ULONG STDMETHODCALLTYPE AddRef();
    ULONG STDMETHODCALLTYPE Release();

    // IDMUSProdMenu functions
    HRESULT STDMETHODCALLTYPE GetMenuText( BSTR* pbstrText );
    HRESULT STDMETHODCALLTYPE GetMenuHelpText( BSTR* pbstrHelpText );
    HRESULT STDMETHODCALLTYPE OnMenuInit( HMENU hMenu, UINT nMenuID );
    HRESULT STDMETHODCALLTYPE OnMenuSelect();

private:
    DWORD						m_dwRef;
};


#endif // __XBOXSYNTHMENU_H__
