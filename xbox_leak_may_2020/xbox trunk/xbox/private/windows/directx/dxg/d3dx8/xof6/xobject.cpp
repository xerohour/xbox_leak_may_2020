//**************************************************************************
//
//  Copyright (C) 1998 Microsoft Corporation. All Rights Reserved.
//
//  File:   xobject.cpp
//
//  Description:    Implement XObject class.
//
//  History:
//      01/14/98    CongpaY     Created
//
//**************************************************************************

#include "precomp.h"

STDMETHODIMP_(ULONG)
XObject::AddRef()
{
    m_cRef++;

    return m_cRef;
}

STDMETHODIMP_(ULONG)
XObject::Release()
{
    ULONG cRef;

    cRef = --m_cRef;

    if (!cRef)
        delete this;

    return cRef;
}

#undef DPF_MODNAME
#define DPF_MODNAME "XObject::GetName"

STDMETHODIMP
XObject::GetName(LPSTR szName, LPDWORD pdwSize)
{
    ASSERT(pdwSize != NULL);

    if (!m_szName) {
        if (*pdwSize > 0 && szName)
            *szName = 0;
        *pdwSize = 0;
    } else {
        DWORD dwSize;

        dwSize = xstrlen(m_szName) + 1;

        if (szName) {
            if (dwSize > *pdwSize) {
                DPF_ERR("szName buffer not big enough.");
                return DXFILEERR_BADVALUE;
            }

            memcpy(szName, m_szName, dwSize * sizeof(CHAR));
        }

        *pdwSize = dwSize;
    }

    return DXFILE_OK;
}

STDMETHODIMP
XObject::GetId(LPGUID pId)
{
    ASSERT(pId != NULL);

    memcpy(pId, &m_id, sizeof(GUID));

    return DXFILE_OK;
}

#undef DPF_MODNAME
#define DPF_MODNAME "XObject::ProcessSelf"

HRESULT XObject::ProcessSelf(XDataObj *pParent)
{
    HRESULT hr;

    if (ValidChild(pParent))
        hr = pParent->AddChild(this);
    else {
        DPF_ERR("Invalid child object existed.");
        hr = DXFILEERR_BADOBJECT;
    }

    return hr;
}
