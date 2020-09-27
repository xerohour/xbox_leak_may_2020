/**************************************************************\
    FILE: xboxglob.cpp

    DESCRIPTION:
        Refcounted HGLOBAL.
\**************************************************************/

#include "priv.h"
#include "xboxglob.h"


/****************************************************\
    Constructor
\****************************************************/
CXboxGlob::CXboxGlob() : m_cRef(1)
{
    DllAddRef();

    // This needs to be allocated in Zero Inited Memory.
    // Assert that all Member Variables are inited to Zero.
    ASSERT(!m_hglob);

    LEAK_ADDREF(LEAK_CXboxGlob);
}


/****************************************************\
    Destructor
\****************************************************/
CXboxGlob::~CXboxGlob()
{
    if (m_hglob)
        GlobalFree(m_hglob);

    DllRelease();
    LEAK_DELREF(LEAK_CXboxGlob);
}


//===========================
// *** IUnknown Interface ***
ULONG CXboxGlob::AddRef()
{
    m_cRef++;
    return m_cRef;
}

ULONG CXboxGlob::Release()
{
    ASSERT(m_cRef > 0);
    m_cRef--;

    if (m_cRef > 0)
        return m_cRef;

    delete this;
    return 0;
}

HRESULT CXboxGlob::QueryInterface(REFIID riid, void **ppvObj)
{
    if (IsEqualIID(riid, IID_IUnknown))
    {
        *ppvObj = SAFECAST(this, IUnknown*);
    }
    else
    {
        TraceMsg(TF_XBOXQI, "CXboxGlob::QueryInterface() failed.");
        *ppvObj = NULL;
        return E_NOINTERFACE;
    }

    AddRef();
    return S_OK;
}



/****************************************************\
    FUNCTION: CXboxGlob_Create
  
    DESCRIPTION:
        This function will create an instance of the
    CXboxGlob object.
\****************************************************/
IUnknown * CXboxGlob_Create(HGLOBAL hglob)
{
    IUnknown * punk = NULL;
    CXboxGlob * pfg = new CXboxGlob();

    if (pfg)
    {
        pfg->m_hglob = hglob;
        pfg->QueryInterface(IID_IUnknown, (LPVOID *)&punk);
        pfg->Release();
    }

    return punk;
}



/****************************************************\
    FUNCTION: CXboxGlob_CreateStr
  
    DESCRIPTION:
        This function will create an instance of the
    CXboxGlob object.
\****************************************************/
CXboxGlob * CXboxGlob_CreateStr(LPCTSTR pszStr)
{
    CXboxGlob * pfg = new CXboxGlob();

    if (EVAL(pfg))
        pfg->m_hglob = (HGLOBAL) pszStr;

    return pfg;
}
