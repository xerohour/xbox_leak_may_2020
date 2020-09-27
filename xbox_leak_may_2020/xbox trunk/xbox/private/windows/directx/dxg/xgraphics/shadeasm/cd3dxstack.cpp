///////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 1999 Microsoft Corporation.  All Rights Reserved.
//
//  File:       stacks.cpp
//  Content:    Stacks
//
///////////////////////////////////////////////////////////////////////////

#include "pchshadeasm.h"

namespace XGRAPHICS {

//----------------------------------------------------------------------------
// XAsmCD3DXDwStack - DWORD stack
//----------------------------------------------------------------------------


XAsmCD3DXDwStack::XAsmCD3DXDwStack()
{
    m_pdw = NULL;
    m_cdw = 0;
    m_cdwLim = 0;
    m_hr = S_OK;
}

XAsmCD3DXDwStack::~XAsmCD3DXDwStack()
{
    if(m_pdw)
        delete [] m_pdw;
}

HRESULT
XAsmCD3DXDwStack::Push(DWORD dw)
{
    if(m_cdw == m_cdwLim)
    {
        DWORD *pdw;
        
        if(!(pdw = new DWORD[m_cdwLim + 16]))
            return m_hr = E_OUTOFMEMORY;

        if(m_pdw)
        {
            memcpy(pdw, m_pdw, m_cdw * sizeof(DWORD));
            delete [] m_pdw;
        }

        m_pdw = pdw;
        m_cdwLim += 16;
    }

    m_pdw[m_cdw++] = dw;
    return S_OK;
}

HRESULT
XAsmCD3DXDwStack::Pop(DWORD *pdw)
{
    if(m_cdw == 0)
    {
        if(pdw)
            *pdw = 0;
		DXGRIP("Stack Empty");
        return m_hr = E_FAIL;
    }

    m_cdw--;

    if(pdw)
        *pdw = m_pdw[m_cdw];

    return S_OK;
}

HRESULT
XAsmCD3DXDwStack::GetLastError()
{
    HRESULT hr = m_hr;
    m_hr = S_OK;
    return hr;
}


//----------------------------------------------------------------------------
// XAsmCD3DXSzStack - String stack
//----------------------------------------------------------------------------

XAsmCD3DXSzStack::XAsmCD3DXSzStack()
{
    m_ppsz = NULL;
    m_cpsz = 0;
    m_cpszLim = 0;
    m_hr = S_OK;
}

XAsmCD3DXSzStack::~XAsmCD3DXSzStack()
{
    if(m_ppsz)
    {
        for(UINT ipsz = 0; ipsz < m_cpsz; ipsz++)
        {
            if(m_ppsz[ipsz])
                delete [] m_ppsz[ipsz];
        }

        delete [] m_ppsz;
    }
}

HRESULT
XAsmCD3DXSzStack::Push(char *psz)
{
    if(m_cpsz == m_cpszLim)
    {
        char **ppsz;
        
        if(!(ppsz = new char* [m_cpszLim + 16]))
            return m_hr = E_OUTOFMEMORY;

        if(m_ppsz)
        {
            memcpy(ppsz, m_ppsz, m_cpsz * sizeof(char *));
            delete [] m_ppsz;
        }

        m_ppsz = ppsz;
        m_cpszLim += 16;
    }

    if(psz)
    {
        UINT cch;
        char *pszCopy;

        cch = strlen(psz);

        if(!(pszCopy = new char[cch + 1]))
            return m_hr = E_OUTOFMEMORY;

        memcpy(pszCopy, psz, cch * sizeof(char));
        pszCopy[cch] = 0;

        m_ppsz[m_cpsz++] = pszCopy;
    }
    else
    {
        m_ppsz[m_cpsz++] = NULL;
    }

    return S_OK;
}

HRESULT
XAsmCD3DXSzStack::Pop(char **ppsz)
{
    if(m_cpsz == 0)
    {
        if(ppsz)
            *ppsz = NULL;

        DXGRIP("Stack Empty");
        return m_hr = E_FAIL;
    }

    m_cpsz--;

    if(ppsz)
        *ppsz = m_ppsz[m_cpsz];

    return S_OK;
}

HRESULT
XAsmCD3DXSzStack::GetLastError()
{
    HRESULT hr = m_hr;
    m_hr = S_OK;
    return hr;
}

} // namespace XGRAPHICS

