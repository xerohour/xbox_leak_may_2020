/*****************************************************************************
 *
 *    xboxcf.cpp - IClassFactory interface
 *
 *****************************************************************************/

#include "priv.h"
#include "xbwebvw.h"
#include "xbnmspc.h"


/*****************************************************************************
 *
 *    CXboxFactory
 *
 *
 *****************************************************************************/

class CXboxFactory : public IClassFactory
{
public:
    //////////////////////////////////////////////////////
    // Public Interfaces
    //////////////////////////////////////////////////////
    
    // *** IUnknown ***
    virtual STDMETHODIMP_(ULONG) AddRef(void);
    virtual STDMETHODIMP_(ULONG) Release(void);
    virtual STDMETHODIMP QueryInterface(REFIID riid, LPVOID * ppvObj);
    
    // *** IClassFactory ***
    virtual STDMETHODIMP CreateInstance(IUnknown *pUnkOuter, REFIID riid, void **ppvObject);
    virtual STDMETHODIMP LockServer(BOOL fLock);

public:
    CXboxFactory(REFCLSID rclsid);
    ~CXboxFactory(void);

    // Friend Functions
    friend HRESULT CXboxFactory_Create(REFCLSID rclsid, REFIID riid, LPVOID * ppvObj);

protected:
    int                     m_cRef;
    CLSID                   m_rclsid;
};



/*****************************************************************************
 *    IClassFactory::CreateInstance
 *****************************************************************************/

HRESULT CXboxFactory::CreateInstance(IUnknown * punkOuter, REFIID riid, LPVOID * ppvObj)
{
    HRESULT hres = ResultFromScode(REGDB_E_CLASSNOTREG);

    if (!punkOuter)
    {
        if (IsEqualIID(m_rclsid, CLSID_XboxFolder))
            hres = CXboxFolder_Create(riid, ppvObj);
        else if (IsEqualIID(m_rclsid, CLSID_XboxWebView))
            hres = CXboxWebView_Create(riid, ppvObj);
        else if (IsEqualIID(m_rclsid, CLSID_XboxDataObject))
            hres = CXboxObj_Create(riid, ppvObj);
        else
            ASSERT(0);  // What are you looking for?
    }
    else
    {        // Does anybody support aggregation any more?
        hres = ResultFromScode(CLASS_E_NOAGGREGATION);
    }

    return hres;
}

/*****************************************************************************
 *
 *    IClassFactory::LockServer
 *
 *    What a stupid function.  Locking the server is identical to
 *    creating an object and not releasing it until you want to unlock
 *    the server.
 *
 *****************************************************************************/

HRESULT CXboxFactory::LockServer(BOOL fLock)
{
    if (fLock)
        DllAddRef();
    else
        DllRelease();

    return S_OK;
}

/*****************************************************************************
 *
 *    CXboxFactory_Create
 *
 *****************************************************************************/

HRESULT CXboxFactory_Create(REFCLSID rclsid, REFIID riid, LPVOID * ppvObj)
{
    HRESULT hres;

    if (GetShdocvwVersion() < 5)
    {
        // Check if we are running under older IE's and fail so that
        // side by side IE4, IE5 can work
        hres = ResultFromScode(E_NOINTERFACE);
    }
    else if (IsEqualIID(riid, IID_IClassFactory))
    {
        *ppvObj = (LPVOID) new CXboxFactory(rclsid);
        hres = (*ppvObj) ? S_OK : E_OUTOFMEMORY;
    }
    else
        hres = ResultFromScode(E_NOINTERFACE);

    return hres;
}





/****************************************************\
    Constructor
\****************************************************/
CXboxFactory::CXboxFactory(REFCLSID rclsid) : m_cRef(1)
{
    m_rclsid = rclsid;
    DllAddRef();
    LEAK_ADDREF(LEAK_CXboxFactory);
}


/****************************************************\
    Destructor
\****************************************************/
CXboxFactory::~CXboxFactory()
{
    DllRelease();
    LEAK_DELREF(LEAK_CXboxFactory);
}


//===========================
// *** IUnknown Interface ***
//===========================

ULONG CXboxFactory::AddRef()
{
    m_cRef++;
    return m_cRef;
}

ULONG CXboxFactory::Release()
{
    ASSERT(m_cRef > 0);
    m_cRef--;

    if (m_cRef > 0)
        return m_cRef;

    delete this;
    return 0;
}

HRESULT CXboxFactory::QueryInterface(REFIID riid, void **ppvObj)
{
    if (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_IClassFactory))
    {
        *ppvObj = SAFECAST(this, IClassFactory *);
    }
    else
    {
        TraceMsg(TF_XBOXQI, "CXboxFactory::QueryInterface() failed.");
        *ppvObj = NULL;
        return E_NOINTERFACE;
    }

    AddRef();
    return S_OK;
}
