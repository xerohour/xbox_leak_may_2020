/*++

Copyright (c) Microsoft Corporation


Module Name:

    objectwithsite.cpp

Abstract:
    
    Implementation of CShellObjectWithSite class that always
    queries for IShellFolderView.

    Classes that inherit from this class use only its site's IShellFolderView.
    Or require that the site implement IShellFolderView.  This is true of the
    context menu, view callback, and drop target.

Environment:

    Windows 2000
    Uses ATL

Notes:

Revision History:

    July 2001 - created by Mitchell Dernis (mitchd)

--*/

#include <stdafx.h>

HRESULT CShellObjectWithSite::SetSite(IUnknown* pUnkSite)
/*++
  Routine Description:
    
    Sets (or clears) our owner site.

  Arguments:
    
    pUnkSite -  the IUnknown of the owning site.

  Return Value:

    Docs say always S_OK.

--*/
{
    //
    //  If we already have a site, release it.
    //

    if(m_pUnknownSite)
    {
        m_pUnknownSite->Release();
        m_pUnknownSite= NULL;
    }

    //
    //  If pUnkSite is not NULL, QI for IShellFolderView, this has the
    //  effect of AddRef'ing too.
    m_pUnknownSite = pUnkSite;
    if(m_pUnknownSite)
        m_pUnknownSite->AddRef();

    return S_OK;
}


HRESULT CShellObjectWithSite::GetSite(REFIID riid, void **ppvSite)
/*++
  Routine Description:

    The CDefView calls us with its IUknown almost immediately.  We usually
    want the IShellFolderView interface of the site, we QI for that and store
    it, rather than the pUnkSite itself.

  Arguments:

    riid - desired interface.
    ppvSite - [OUT] pointer to receive interface pointer.

  Return Value:

    S_OK - success.
    E_FAIL - we don't have a site
    E_NOINTERFACE - the return code from QI if the site doesn't have the desired interface.

--*/
{
    HRESULT hr = E_FAIL;

    if(m_pUnknownSite)
    {
        hr = m_pUnknownSite->QueryInterface(riid, ppvSite);
    }

    return hr;
}


HRESULT CShellObjectWithSite::GetService(REFGUID guidService, REFIID riid, void **ppvService)
{
    HRESULT hr = E_FAIL;
    IServiceProvider *pServiceProvider;

    if(m_pUnknownSite)
    {
      hr = m_pUnknownSite->QueryInterface(IID_PPV_ARG(IServiceProvider, &pServiceProvider));
      if(SUCCEEDED(hr))
      {
        hr = pServiceProvider->QueryService(guidService, riid, ppvService);
        pServiceProvider->Release();
      }
    }
    return hr;
}