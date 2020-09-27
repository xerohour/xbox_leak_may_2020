/*****************************************************************************\
    FILE: xbwebvw.h

    DESCRIPTION:
        This file exists so WebView can automate the Xbox Shell Extension and get
    information like the MessageOfTheDay.
\*****************************************************************************/

#include "priv.h"
#include "xbwebvw.h"
#include "xbnmspc.h"
#include <shlguid.h>

//===========================
// *** IDispatch Interface ***
//===========================

// BUGBUG: Cane we nuke this?

STDMETHODIMP CXboxWebView::GetTypeInfoCount(UINT * pctinfo)
{ 
    return CImpIDispatch::GetTypeInfoCount(pctinfo); 
}

STDMETHODIMP CXboxWebView::GetTypeInfo(UINT itinfo, LCID lcid, ITypeInfo * * pptinfo)
{ 
    return CImpIDispatch::GetTypeInfo(itinfo, lcid, pptinfo); 
}

STDMETHODIMP CXboxWebView::GetIDsOfNames(REFIID riid, OLECHAR * * rgszNames, UINT cNames, LCID lcid, DISPID * rgdispid)
{ 
    return CImpIDispatch::GetIDsOfNames(riid, rgszNames, cNames, lcid, rgdispid); 
}

STDMETHODIMP CXboxWebView::Invoke(DISPID dispidMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pdispparams, VARIANT * pvarResult, EXCEPINFO * pexcepinfo, UINT * puArgErr)
{
    return CImpIDispatch::Invoke(dispidMember, riid, lcid, wFlags, pdispparams, pvarResult, pexcepinfo, puArgErr);
}



//===========================
// *** IXboxWebView Interface ***
//===========================

/*****************************************************************************\
    FUNCTION: _GetIXboxWebView

    DESCRIPTION:
\*****************************************************************************/
HRESULT CXboxWebView::_GetIXboxWebView(IXboxWebView ** ppfwb)
{
    IShellFolderViewCB * psfvcb = NULL;
    HRESULT hr = S_FALSE;

    ASSERT(_punkSite);
    if (EVAL(ppfwb))
        *ppfwb = NULL;

    IUnknown_QueryService(_punkSite, SID_ShellFolderViewCB, IID_IShellFolderViewCB, (LPVOID *) &psfvcb);
    // IE4's shell32 doesn't support QS(SID_ShellFolderViewCB, IID_IShellFolderViewCB), so we need to
    // QS(SID_ShellFolderViewCB, IShellFolderView) and then use IShellFolderView::SetCallback()
    if (!psfvcb)
    {
        IDefViewFrame * pdvf = NULL;

        IUnknown_QueryService(_punkSite, SID_DefView, IID_IDefViewFrame, (LPVOID *) &pdvf);
        if (EVAL(pdvf))
        {
            IShellFolderView * psfv = NULL;

            pdvf->QueryInterface(IID_IShellFolderView, (LPVOID *) &psfv);
            if (EVAL(psfv))
            {
                if (EVAL(SUCCEEDED(psfv->SetCallback(NULL, &psfvcb))))
                {
                    IShellFolderViewCB * psfvcbTemp = NULL;

                    if (SUCCEEDED(psfv->SetCallback(psfvcb, &psfvcbTemp)) && psfvcbTemp)
                    {
                        // We should get NULL back but if not, release the ref instead of leaking.
                        psfvcbTemp->Release();
                    }
                }

                psfv->Release();
            }

            pdvf->Release();
        }
    }
    
    if (EVAL(psfvcb))
    {
        if (EVAL(SUCCEEDED(psfvcb->QueryInterface(IID_IXboxWebView, (LPVOID *) ppfwb))))
            hr = S_OK;

        psfvcb->Release();
    }

    return hr;
}


/*****************************************************************************\
    FUNCTION: IXboxWebView::get_MessageOfTheDay

    DESCRIPTION:
\*****************************************************************************/
HRESULT CXboxWebView::get_MessageOfTheDay(BSTR * pbstr)
{
    IXboxWebView * pfwb;
    HRESULT hr = _GetIXboxWebView(&pfwb);

    if (EVAL(S_OK == hr))
    {
        ASSERT(pfwb);
        hr = pfwb->get_MessageOfTheDay(pbstr);
        pfwb->Release();
    }

    return hr;
}


/*****************************************************************************\
    FUNCTION: IXboxWebView::get_UserName

    DESCRIPTION:
\*****************************************************************************/
HRESULT CXboxWebView::get_UserName(BSTR * pbstr)
{
    IXboxWebView * pfwb;
    HRESULT hr = _GetIXboxWebView(&pfwb);

    if (EVAL(S_OK == hr))
    {
        ASSERT(pfwb);
        hr = pfwb->get_UserName(pbstr);
        pfwb->Release();
    }

    return hr;
}


/*****************************************************************************\
    FUNCTION: IXboxWebView::get_Server

    DESCRIPTION:
\*****************************************************************************/
HRESULT CXboxWebView::get_Server(BSTR * pbstr)
{
    IXboxWebView * pfwb;
    HRESULT hr = _GetIXboxWebView(&pfwb);

    if (EVAL(S_OK == hr))
    {
        ASSERT(pfwb);
        hr = pfwb->get_Server(pbstr);
        pfwb->Release();
    }

    return hr;
}


/*****************************************************************************\
    FUNCTION: IXboxWebView::get_Directory

    DESCRIPTION:
\*****************************************************************************/
HRESULT CXboxWebView::get_Directory(BSTR * pbstr)
{
    IXboxWebView * pfwb;
    HRESULT hr = _GetIXboxWebView(&pfwb);

    if (EVAL(S_OK == hr))
    {
        ASSERT(pfwb);
        hr = pfwb->get_Directory(pbstr);
        pfwb->Release();
    }

    return hr;
}


/*****************************************************************************\
    FUNCTION: IXboxWebView::get_PasswordLength

    DESCRIPTION:
\*****************************************************************************/
HRESULT CXboxWebView::get_PasswordLength(long * plLength)
{
    IXboxWebView * pfwb;
    HRESULT hr = _GetIXboxWebView(&pfwb);

    if (EVAL(S_OK == hr))
    {
        ASSERT(pfwb);
        hr = pfwb->get_PasswordLength(plLength);
        pfwb->Release();
    }

    return hr;
}


/*****************************************************************************\
    FUNCTION: IXboxWebView::get_EmailAddress

    DESCRIPTION:
\*****************************************************************************/
HRESULT CXboxWebView::get_EmailAddress(BSTR * pbstr)
{
    IXboxWebView * pfwb;
    HRESULT hr = _GetIXboxWebView(&pfwb);

    if (EVAL(S_OK == hr))
    {
        ASSERT(pfwb);
        hr = pfwb->get_EmailAddress(pbstr);
        pfwb->Release();
    }

    return hr;
}


/*****************************************************************************\
    FUNCTION: IXboxWebView::put_EmailAddress

    DESCRIPTION:
\*****************************************************************************/
HRESULT CXboxWebView::put_EmailAddress(BSTR bstr)
{
    IXboxWebView * pfwb;
    HRESULT hr = _GetIXboxWebView(&pfwb);

    if (EVAL(S_OK == hr))
    {
        ASSERT(pfwb);
        hr = pfwb->put_EmailAddress(bstr);
        pfwb->Release();
    }

    return hr;
}


/*****************************************************************************\
    FUNCTION: IXboxWebView::get_CurrentLoginAnonymous

    DESCRIPTION:
\*****************************************************************************/
HRESULT CXboxWebView::get_CurrentLoginAnonymous(VARIANT_BOOL * pfAnonymousLogin)
{
    IXboxWebView * pfwb;
    HRESULT hr = _GetIXboxWebView(&pfwb);

    if (EVAL(S_OK == hr))
    {
        ASSERT(pfwb);
        hr = pfwb->get_CurrentLoginAnonymous(pfAnonymousLogin);
        pfwb->Release();
    }

    return hr;
}


/*****************************************************************************\
    FUNCTION: IXboxWebView::LoginAnonymously

    DESCRIPTION:
\*****************************************************************************/
HRESULT CXboxWebView::LoginAnonymously(void)
{
    IXboxWebView * pfwb;
    HRESULT hr = _GetIXboxWebView(&pfwb);

    if (EVAL(S_OK == hr))
    {
        ASSERT(pfwb);
        hr = pfwb->LoginAnonymously();
        pfwb->Release();
    }

    return hr;
}


/*****************************************************************************\
    FUNCTION: IXboxWebView::LoginWithPassword

    DESCRIPTION:
\*****************************************************************************/
HRESULT CXboxWebView::LoginWithPassword(BSTR bUserName, BSTR bPassword)
{
    IXboxWebView * pfwb;
    HRESULT hr = _GetIXboxWebView(&pfwb);

    if (EVAL(S_OK == hr))
    {
        ASSERT(pfwb);
        hr = pfwb->LoginWithPassword(bUserName, bPassword);
        pfwb->Release();
    }

    return hr;
}


/*****************************************************************************\
    FUNCTION: IXboxWebView::LoginWithoutPassword

    DESCRIPTION:
\*****************************************************************************/
HRESULT CXboxWebView::LoginWithoutPassword(BSTR bUserName)
{
    IXboxWebView * pfwb;
    HRESULT hr = _GetIXboxWebView(&pfwb);

    if (EVAL(S_OK == hr))
    {
        ASSERT(pfwb);
        hr = pfwb->LoginWithoutPassword(bUserName);
        pfwb->Release();
    }

    return hr;
}


/*****************************************************************************\
    FUNCTION: IXboxWebView::InvokeHelp

    DESCRIPTION:
\*****************************************************************************/
HRESULT CXboxWebView::InvokeHelp(void)
{
    IXboxWebView * pfwb;
    HRESULT hr = _GetIXboxWebView(&pfwb);

    if (EVAL(S_OK == hr))
    {
        ASSERT(pfwb);
        hr = pfwb->InvokeHelp();
        pfwb->Release();
    }

    return hr;
}


/*****************************************************************************\
    FUNCTION: CXboxWebView_Create

    DESCRIPTION:
\*****************************************************************************/
HRESULT CXboxWebView_Create(REFIID riid, LPVOID * ppv)
{
    HRESULT hr = E_OUTOFMEMORY;
    CXboxWebView * pfwv = new CXboxWebView();

    if (EVAL(pfwv))
    {
        hr = pfwv->QueryInterface(riid, ppv);
        pfwv->Release();
    }

    return hr;
}



/****************************************************\
    Constructor
\****************************************************/
CXboxWebView::CXboxWebView() : m_cRef(1), CImpIDispatch(&IID_IXboxWebView)
{
    DllAddRef();

    // This needs to be allocated in Zero Inited Memory.
    // Assert that all Member Variables are inited to Zero.

    LEAK_ADDREF(LEAK_CXboxWebView);
}


/****************************************************\
    Destructor
\****************************************************/
/*****************************************************************************
 *
 *      XboxView_OnRelease (from shell32.IShellView)
 *
 *      When the view is released, clean up various stuff.
 *
 *      BUGBUG -- (Note that there is a race here, because this->hwndOwner
 *      doesn't get zero'd out on the OnWindowDestroy because the shell
 *      doesn't give us a pdvsci...)
 *
 *      We release the psf before triggering the timeout, which is a
 *      signal to the trigger not to do anything.
 *
 *      _UNDOCUMENTED_: This callback and its parameters are not documented.
 *
 *****************************************************************************/
CXboxWebView::~CXboxWebView()
{
    DllRelease();
    LEAK_DELREF(LEAK_CXboxWebView);
}


//===========================
// *** IUnknown Interface ***
//===========================

ULONG CXboxWebView::AddRef()
{
    m_cRef++;
    return m_cRef;
}

ULONG CXboxWebView::Release()
{
    ASSERT(m_cRef > 0);
    m_cRef--;

    if (m_cRef > 0)
        return m_cRef;

    delete this;
    return 0;
}

HRESULT CXboxWebView::QueryInterface(REFIID riid, void **ppvObj)
{
    static const QITAB qit[] = {
        QITABENT(CXboxWebView, IObjectWithSite),
        QITABENT(CXboxWebView, IDispatch),
        QITABENT(CXboxWebView, IObjectWithSite),
        QITABENT(CXboxWebView, IObjectSafety),
        QITABENT(CXboxWebView, IXboxWebView),
        { 0 },
    };

    return QISearch(this, qit, riid, ppvObj);
}
