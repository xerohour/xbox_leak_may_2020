///////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 1999 Microsoft Corporation.  All Rights Reserved.
//
//  File:       CBuffer.cpp
//  Content:    Buffer object
//
///////////////////////////////////////////////////////////////////////////

#include "pchcore.h"

CD3DXBuffer::CD3DXBuffer()
{
    m_pbBuffer = NULL;
    m_dwSize = 0;
    m_cRef = 1;
}


CD3DXBuffer::~CD3DXBuffer()
{
    if(m_pbBuffer)
        delete [] m_pbBuffer;
}


HRESULT
CD3DXBuffer::Init(DWORD dwSize)
{
    m_dwSize = dwSize;

    m_pbBuffer = new BYTE[dwSize];
    if (m_pbBuffer == NULL)
        return E_OUTOFMEMORY;
    else
        return S_OK;
}


// IUnknown
STDMETHODIMP
CD3DXBuffer::QueryInterface(REFIID riid, LPVOID *ppv)
{
    *ppv=NULL;
    if (riid == IID_IUnknown)
        *ppv=(IUnknown*)this;
    else if (riid == IID_ID3DXBuffer)
        *ppv=(ID3DXBuffer*)this;
    else
        return E_NOINTERFACE;

    ((LPUNKNOWN)*ppv)->AddRef();

    return S_OK;
}


ULONG STDMETHODCALLTYPE
CD3DXBuffer::AddRef()
{
    m_cRef += 1;
    return m_cRef;
}


ULONG STDMETHODCALLTYPE
CD3DXBuffer::Release()
{
    ULONG cRef = m_cRef;
    m_cRef -= 1;

    if (cRef == 1)
        delete this;

    return cRef-1;
}


PVOID STDMETHODCALLTYPE
CD3DXBuffer::GetBufferPointer()
{
    return m_pbBuffer;
}


DWORD STDMETHODCALLTYPE
CD3DXBuffer::GetBufferSize()
{
    return m_dwSize;
}

HRESULT CD3DXBuffer::Grow(DWORD dwSize)
{
    if (dwSize > m_dwSize)
    {
        // Allocate new buffer
        PBYTE pBuffer = new BYTE[dwSize];
        if (pBuffer == NULL)
            return E_OUTOFMEMORY;

        // Copy contents
        memcpy(pBuffer, m_pbBuffer, m_dwSize);

        // Delete old buffer
        delete [] m_pbBuffer;
        m_pbBuffer = pBuffer;
        m_dwSize = dwSize;
    }
    return S_OK;
}

CD3DXStringBuffer::CD3DXStringBuffer()
{
    m_pStringLow = NULL;
    m_pStringHigh = NULL;
}

HRESULT CD3DXStringBuffer::Init(DWORD size)
{
    HRESULT hr = CD3DXBuffer::Init(size);
    if (FAILED(hr))
        return hr;
    m_pStringLow = reinterpret_cast<char**>(m_pbBuffer);
    m_pStringHigh = reinterpret_cast<char*>(m_pbBuffer + m_dwSize);
    return S_OK;
}

HRESULT D3DXCreateStringBuffer(DWORD size, CD3DXStringBuffer** ppBuffer)
{
    CD3DXStringBuffer *pBuffer = NULL;
    HRESULT hr = S_OK;

    // first allocate the container
    pBuffer = new CD3DXStringBuffer();
    if (pBuffer == NULL)
    {
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }

    // now allocate the buffer
    hr = pBuffer->Init(size);
    if (FAILED(hr))
        goto e_Exit;

    *ppBuffer = pBuffer;
    pBuffer = NULL;

e_Exit:
    delete pBuffer;
    return hr;
}

HRESULT CD3DXStringBuffer::AddString(char* pString)
{
    DWORD size = strlen(pString);
    if (reinterpret_cast<char*>(m_pStringLow + 1) > m_pStringHigh - size - 1)
    {
        DWORD newSize = max(m_dwSize + size, static_cast<DWORD>(m_dwSize * 1.2f));
        // Allocate new buffer
        PBYTE pBuffer = new BYTE[newSize];
        if (pBuffer == NULL)
            return E_OUTOFMEMORY;

        // Compute new pointers
        DWORD stringSize = m_dwSize - (DWORD)(((LPBYTE)m_pStringHigh) - m_pbBuffer);
        char* pStringHigh = reinterpret_cast<char*>(pBuffer + newSize - stringSize);

        // Copy strings
        memcpy(pStringHigh, m_pStringHigh, stringSize);

        // Update string pointers
        int offset = (int)(pStringHigh - m_pStringHigh);
        char** pStringLow = reinterpret_cast<char**>(pBuffer);
        for (char** t = reinterpret_cast<char**>(m_pbBuffer); t < m_pStringLow; ++t)
        {
            *pStringLow = *t + offset;
            pStringLow++;
        }

        // Delete old buffer & update data
        delete [] m_pbBuffer;
        m_pbBuffer = pBuffer;
        m_dwSize = newSize;
        m_pStringLow = pStringLow;
        m_pStringHigh = pStringHigh;
    }
    m_pStringHigh -= size + 1;
    strcpy(m_pStringHigh, pString);
    *m_pStringLow = m_pStringHigh;
    m_pStringLow++;
    return S_OK;
}