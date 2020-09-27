/*****************************************************************************
 *
 *    xboxicon.cpp - IExtractIcon interface
 *
 *****************************************************************************/

#include "priv.h"
#include "xboxicon.h"
#include "xboxurl.h"




INT GetXboxIcon(UINT uFlags, BOOL fIsRoot)
{
    INT nIcon = (uFlags & GIL_OPENICON) ? IDI_XBOXOPENFOLDER : IDI_XBOXFOLDER;

    if (fIsRoot)
        nIcon = IDI_XBOXSERVER;      // This is an XBOX Server Icon.

    return nIcon;
}



#ifndef UNICODE
#define PathFindExtensionA PathFindExtension
#endif

//===========================
// *** IExtractIconA Interface ***
//===========================

/*****************************************************************************\
    FUNCTION: GetIconLocation

    DESCRIPTION:
        Get the icon location from the registry.

    _UNDOCUMENTED_:  Not mentioned is that if you return GIL_NOTFILENAME,
    you should take steps to ensure uniqueness of the non-filename
    return value, to avoid colliding with non-filenames from other
    shell extensions.

    _UNDOCUMENTED_:  The inability of SHGetFileInfo to work properly
    on "magic internal" cached association icons like "*23" is not
    documented.  As a result of this "feature", the SHGFI_ICONLOCATION
    flag is useless.


    Actually, we can still use SHGetFileInfo; we'll use the shell's own
    feature against it.  We'll do a SHGFI_SYSICONINDEX and return that
    as the icon index, with "*" as the GIL_NOTFILENAME.


    _BUGBUG_: We don't handle the cases where we ought to use
    GIL_SIMULATEDOC.
\*****************************************************************************/
HRESULT CXboxIcon::GetIconLocation(UINT uFlags, LPSTR szIconFile, UINT cchMax, int *piIndex, UINT *pwFlags)
{
    static CHAR szXbnmspc[MAX_PATH] = "";

    if (0 == szXbnmspc[0])
        GetModuleFileNameA(HINST_THISDLL, szXbnmspc, ARRAYSIZE(szXbnmspc));

    // NOTE: This is negative because it's a resource index.
    *piIndex = (0 - GetXboxIcon(uFlags, m_nRoot));

    if (pwFlags)
        *pwFlags = GIL_PERCLASS; //(uFlags & GIL_OPENICON);

    StrCpyNA(szIconFile, szXbnmspc, cchMax);

    return S_OK;
}


//===========================
// *** IExtractIconW Interface ***
//===========================
HRESULT CXboxIcon::GetIconLocation(UINT uFlags, LPWSTR wzIconFile, UINT cchMax, int *piIndex, UINT *pwFlags)
{
    HRESULT hres;
    CHAR szIconFile[MAX_PATH];

    ASSERT_SINGLE_THREADED;
    hres = GetIconLocation(uFlags, szIconFile, ARRAYSIZE(szIconFile), piIndex, pwFlags);
    if (EVAL(SUCCEEDED(hres)))
        SHAnsiToUnicode(szIconFile, wzIconFile, cchMax);

    return hres;
}


//===========================
// *** IQueryInfo Interface ***
//===========================
HRESULT CXboxIcon::GetInfoTip(DWORD dwFlags, WCHAR **ppwszTip)
{
    ASSERT_SINGLE_THREADED;
    if (ppwszTip)       // The shell is stupid and doesn't check the return value.
        *ppwszTip = NULL;

//        SHStrDupW(L"", ppwszTip);

    return E_NOTIMPL;

/**************
    // This InfoTip will appear when the user hovers over an item in defview.
    // We don't want to support this now because it isn't needed and looks different
    // than the shell.

    HRESULT hr = E_FAIL;
    LPITEMIDLIST pidl;

    if (!ppwszTip)
        return E_INVALIDARG;

    *ppwszTip = NULL;
    if (m_pflHfpl && (pidl = m_pflHfpl->GetPidl(0)))
    {
        WCHAR wzToolTip[MAX_URL_STRING];

        hr = XboxPidl_GetDisplayName(pidl, wzItemName, ARRAYSIZE(wzItemName));
        if (EVAL(SUCCEEDED(hr)))
            hr = SHStrDupW(wzToolTip, ppwszTip);
    }

    return hr;
***********/
}

HRESULT CXboxIcon::GetInfoFlags(DWORD *pdwFlags)
{
    *pdwFlags = 0;
    return S_OK;
}




/*****************************************************************************
 *    CXboxIcon_Create
 *
 *    We just stash away the pflHfpl; the real work happens on the
 *    GetIconLocation call.
 *
 *    _HACKHACK_: psf = 0 if we are being called by the property sheet code.
 *****************************************************************************/
HRESULT CXboxIcon_Create(CXboxFolder * pff, CXboxPidlList * pflHfpl, REFIID riid, LPVOID * ppvObj)
{
    HRESULT hres;
    CXboxIcon * pfi;

    *ppvObj = NULL;

    hres = CXboxIcon_Create(pff, pflHfpl, &pfi);
    if (EVAL(SUCCEEDED(hres)))
    {
        hres = pfi->QueryInterface(riid, ppvObj);
        pfi->Release();
    }

    return hres;
}


/*****************************************************************************
 *    CXboxIcon_Create
 *
 *    We just stash away the m_pflHfpl; the real work happens on the
 *    GetIconLocation call.
 *
 *    _HACKHACK_: psf = 0 if we are being called by the property sheet code.
 *****************************************************************************/
HRESULT CXboxIcon_Create(CXboxFolder * pff, CXboxPidlList * pflHfpl, CXboxIcon ** ppfi)
{
    HRESULT hres= E_OUTOFMEMORY;

    *ppfi = new CXboxIcon();
    if (EVAL(*ppfi))
    {
        IUnknown_Set(&(*ppfi)->m_pflHfpl, pflHfpl);
        if (pff && pff->IsRoot())
        {
            (*ppfi)->m_nRoot++;
        }
        hres = S_OK;
    }

    return hres;
}


/****************************************************\
    Constructor
\****************************************************/
CXboxIcon::CXboxIcon() : m_cRef(1)
{
    DllAddRef();

    // This needs to be allocated in Zero Inited Memory.
    // Assert that all Member Variables are inited to Zero.
    ASSERT(!m_pflHfpl);
    ASSERT(!m_nRoot);

    INIT_SINGLE_THREADED_ASSERT;
    LEAK_ADDREF(LEAK_CXboxIcon);
}


/****************************************************\
    Destructor
\****************************************************/
CXboxIcon::~CXboxIcon()
{
    ATOMICRELEASE(m_pflHfpl);

    DllRelease();
    LEAK_DELREF(LEAK_CXboxIcon);
}


//===========================
// *** IUnknown Interface ***
//===========================

ULONG CXboxIcon::AddRef()
{
    m_cRef++;
    return m_cRef;
}

ULONG CXboxIcon::Release()
{
    ASSERT(m_cRef > 0);
    m_cRef--;

    if (m_cRef > 0)
        return m_cRef;

    delete this;
    return 0;
}

HRESULT CXboxIcon::QueryInterface(REFIID riid, void **ppvObj)
{
    static const QITAB qit[] = {
        QITABENT(CXboxIcon, IExtractIconW),
        QITABENT(CXboxIcon, IExtractIconA),
        QITABENT(CXboxIcon, IQueryInfo),
        { 0 },
    };
    
    return QISearch(this, qit, riid, ppvObj);
}
