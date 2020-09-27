// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:     cfilecontainerformats.cpp
// Contents: UNDONE-WARN: Add file description
//
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ INCLUDE FILES +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// "stdafx.h"       -- Precompiled header file
#include "stdafx.h"
#include "CFileContainerFormats.h"


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ FUNCTIONS +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

CFileContainerFormats::CFileContainerFormats(UINT cfmt, const FORMATETC afmt[]) : m_cRef(1)
{
    m_iFmt = 0;
    m_cFmt = cfmt;
    m_aFmt = new FORMATETC[cfmt];
    if (m_aFmt)
        CopyMemory(m_aFmt, afmt, cfmt*sizeof(afmt[0]));
}

CFileContainerFormats::~CFileContainerFormats()
{
    if (m_aFmt)
        delete m_aFmt;
}

// ==== IUnknown methods ====
STDMETHODIMP CFileContainerFormats::QueryInterface( REFIID riid, void **ppvObject)
{
    *ppvObject = NULL;

    if (riid == IID_IUnknown) 
        *ppvObject = this;
    else if (riid == IID_IEnumFORMATETC) 
        *ppvObject = static_cast<IEnumFORMATETC*>(this);
    else
        return E_NOINTERFACE;

    if (*ppvObject)
    {
        static_cast<LPUNKNOWN>(*ppvObject)->AddRef();
        return S_OK;
    }

   return E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) CFileContainerFormats::AddRef(void)
{
    return (ULONG)InterlockedIncrement(&m_cRef);
}

STDMETHODIMP_(ULONG) CFileContainerFormats::Release(void)
{
    if (0 == InterlockedDecrement(&m_cRef))
    {
        delete this;
        return 0;
    }

    return (ULONG)m_cRef;
}

// ==== IEnumFORMATETC functions===
STDMETHODIMP CFileContainerFormats::Next(ULONG celt, FORMATETC *rgelt, ULONG *pceltFethed)
{
    UINT cfetch; 
    HRESULT hres = S_FALSE;
    
    // assume less numbers  
    if (m_iFmt < m_cFmt) 
    {
        cfetch = m_cFmt - m_iFmt; 
        if (cfetch >= celt) 
        { 
            cfetch = celt;
            hres = S_OK;
        }  
        
        CopyMemory(rgelt, &m_aFmt[m_iFmt], cfetch * sizeof(FORMATETC)); 
        m_iFmt += cfetch; 
    }
    else
    { 
        cfetch = 0;
    }
    
    if (pceltFethed) 
    { 
        *pceltFethed = cfetch;
    }
    return hres; 
}  

STDMETHODIMP CFileContainerFormats::Skip(ULONG celt)
{
    m_iFmt += celt;
    if (m_iFmt > m_cFmt) 
    { 
        m_iFmt = m_cFmt; 
        return S_FALSE; 
    } 
    return S_OK; 
}  

STDMETHODIMP CFileContainerFormats::Reset() 
{ 
    m_iFmt = 0; 
    return S_OK; 
}  

STDMETHODIMP CFileContainerFormats::Clone(IEnumFORMATETC ** ppenum)
{ 
    return(E_NOTIMPL); 
}
