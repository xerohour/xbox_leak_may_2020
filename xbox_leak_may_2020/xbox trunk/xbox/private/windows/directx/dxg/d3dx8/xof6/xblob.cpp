//**************************************************************************
//
//  Copyright (C) 1998 Microsoft Corporation. All Rights Reserved.
//
//  File:   xblob.cpp
//
//  Description:    Implement XBinaryData class.
//
//  History:
//      01/13/98    CongpaY     Created
//
//**************************************************************************

#include "precomp.h"

////////////////////////////////////////////////////////////////////////////
//
//  Implemenat XBlobData class
//
////////////////////////////////////////////////////////////////////////////

XBlobData::XBlobData(DWORD dwOffset, LPCSTR szMimeType, DWORD cbSize)
  : m_dwOffset(dwOffset),
    m_szMimeType(szMimeType),
    m_cbSize(cbSize)
{
}

////////////////////////////////////////////////////////////////////////////
//
//  Implemenat XBinaryData class
//
////////////////////////////////////////////////////////////////////////////

XBinaryData::XBinaryData(LPCSTR szName, REFGUID id, XBlobData *pBlobData,
                         XStreamRead *pStream)
  : XObject(szName, id),
    m_szMimeType((LPCSTR)pBlobData->m_szMimeType),
    m_pvData(NULL),
    m_cbSize(pBlobData->m_cbSize),
    m_cbRead(0),
    m_pStream(pStream),
    m_dwOffset(pBlobData->m_dwOffset)
{
}

XBinaryData::XBinaryData(LPCSTR szName, REFGUID id, LPCSTR mimeType,
                         LPVOID data, DWORD cbSize)
  : XObject(szName, id),
    m_szMimeType(mimeType),
    m_pvData(data),
    m_cbSize(cbSize),
    m_cbRead(0),
    m_pStream(NULL),
    m_dwOffset(0)
{
}

XBinaryData::~XBinaryData()
{
    if (m_pStream && m_pvData)
        XFree(m_pvData);
}

HRESULT XBinaryData::Save(XStreamWrite *pStream)
{
    DPF_ERR("Save is not supported.");
    return D3DERR_INVALIDCALL;
}

STDMETHODIMP
XBinaryData::QueryInterface(REFIID riid, LPVOID *ppvObj)
{
    HRESULT hr;

    ASSERT(ppvObj != NULL);

    *ppvObj = NULL;

    if (riid == IID_IUnknown || riid == IID_IDirectXFileBinary) {
        *ppvObj = (LPVOID)this;
        AddRef();
        hr = S_OK;
    } else {
        hr = E_NOINTERFACE;
    }

    return hr;
}

STDMETHODIMP
XBinaryData::GetSize(DWORD *pcbSize)
{
    ASSERT(pcbSize != NULL);

    *pcbSize = m_cbSize;

    return DXFILE_OK;
}

STDMETHODIMP
XBinaryData::GetMimeType(LPCSTR *pszMimeType)
{
    ASSERT(pszMimeType);

    *pszMimeType = m_szMimeType;

    return DXFILE_OK;
}

STDMETHODIMP
XBinaryData::Read(LPVOID pvData, DWORD cbSize, LPDWORD pcbRead)
{
    HRESULT hr;

    ASSERT(pvData != NULL && pcbRead != NULL);

    if (m_cbRead >= m_cbSize) {
        return DXFILEERR_NOMOREDATA;
    }

    if (cbSize > m_cbSize - m_cbRead)
        cbSize = m_cbSize - m_cbRead;

    if (m_pvData) {
        memcpy(pvData, (LPBYTE)m_pvData + m_cbRead, cbSize);

        m_cbRead += cbSize;
        *pcbRead = cbSize;

        hr = DXFILE_OK;

    } else {
        hr = m_pStream->Read(pvData, m_dwOffset + m_cbRead, cbSize, pcbRead);

        if (SUCCEEDED(hr))
            m_cbRead += *pcbRead;
    }

    return hr;
}
