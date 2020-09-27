//**************************************************************************
//
//  Copyright (C) 1998 Microsoft Corporation. All Rights Reserved.
//
//  File:   ximplapi.cpp
//
//  Description:    Implement interface implementation classes.
//
//  History:
//      01/13/98    CongpaY     Created
//
//**************************************************************************

#include "precomp.h"

long g_cInstances = 0;

STDAPI
DirectXFileCreate(LPDIRECTXFILE *ppDXFile)
{
    HRESULT hr;

    ASSERT(ppDXFile != NULL);

    *ppDXFile = new XDirectXFile;

    if (*ppDXFile) {
        g_cInstances++;
        hr = DXFILE_OK;
    } else {
        hr = DXFILEERR_BADALLOC;
    }

    return hr;
}

STDMETHODIMP_(ULONG)
XDirectXFile::AddRef()
{
    m_cRef++;

    return m_cRef;
}

STDMETHODIMP_(ULONG)
XDirectXFile::Release()
{
    ULONG cRef;

    cRef = --m_cRef;

    if (!cRef) {
        delete this;
        g_cInstances--;
    }

    return cRef;
}

STDMETHODIMP
XDirectXFile::CreateEnumObject(LPVOID pvSource,
                              DWORD loadflags,
                              IDirectXFileEnumObject **ppEnumObj)
{
    HRESULT hr;

    ASSERT(ppEnumObj != NULL && pvSource != NULL);

    *ppEnumObj = 0;

    XStreamRead *pStream;

    hr = XStreamRead::Create(pvSource, loadflags, &pStream);

    if (hr == DXFILE_OK) {
        *ppEnumObj = new XEnumObject(pStream);

        if (!*ppEnumObj) {
            delete pStream;
            hr = DXFILEERR_BADALLOC;
        }
    }

    return hr;
}

STDMETHODIMP
XDirectXFile::CreateSaveObject(LPCSTR szFilename, DWORD format,
                              IDirectXFileSaveObject **ppSaveObj)
{
    DPF_ERR("Save is not supported.");
    return D3DERR_INVALIDCALL;
}

STDMETHODIMP
XDirectXFile::QueryInterface(REFIID riid, LPVOID *ppvObj)
{
    HRESULT hr;

    ASSERT(ppvObj != NULL);

    *ppvObj = NULL;

    if (riid == IID_IUnknown || riid == IID_IDirectXFile) {
        *ppvObj = (LPVOID)this;
        AddRef();
        hr = S_OK;
    } else {
        hr = E_NOINTERFACE;
    }

    return hr;
}

STDMETHODIMP
XDirectXFile::RegisterTemplates(LPVOID data, DWORD cbSize)
{
    if (!data || !cbSize) {
        return DXFILEERR_BADVALUE;
    }

    XStreamRead *pStream;

    DXFILELOADMEMORY mData;
    mData.lpMemory = data;
    mData.dSize = cbSize;

    HRESULT hr = XStreamRead::Create(&mData, DXFILELOAD_FROMMEMORY, &pStream);

    if (hr == DXFILE_OK) {
        XDataObj *obj;

        hr = pStream->GetNextObject(&obj); // Parse the templates.

        if (hr == DXFILEERR_NOMOREOBJECTS)
            hr = DXFILE_OK;

        delete pStream;
    }

    return hr;
}

STDMETHODIMP
XEnumObject::QueryInterface(REFIID riid, LPVOID *ppvObj)
{
    HRESULT hr;

    ASSERT(ppvObj != NULL);

    *ppvObj = NULL;

    if (riid == IID_IUnknown || riid == IID_IDirectXFileEnumObject) {
        *ppvObj = (LPVOID)this;
        AddRef();
        hr = S_OK;
    } else {
        hr = E_NOINTERFACE;
    }

    return hr;

}

STDMETHODIMP_(ULONG)
XEnumObject::AddRef()
{
    m_cRef++;

    return m_cRef;
}

STDMETHODIMP_(ULONG)
XEnumObject::Release()
{
    ULONG cRef;

    cRef = --m_cRef;

    if (!cRef) {
        delete m_pStream;
        delete this;
    }

    return cRef;
}

STDMETHODIMP
XEnumObject::GetNextDataObject(IDirectXFileData **ppObj)
{
    HRESULT hr;

    ASSERT(ppObj != NULL);

    *ppObj = NULL;

    XDataObj *pObj;

    hr = m_pStream->GetNextObject(&pObj);

    if (SUCCEEDED(hr)) {
        pObj->AddRef();
        *ppObj = pObj;
    }

    return hr;
}

STDMETHODIMP
XEnumObject::GetDataObjectById(REFGUID id, IDirectXFileData **ppObj)
{
    HRESULT hr;

    ASSERT(ppObj != NULL);

    *ppObj = NULL;

    XDataObj *pObj;

    hr = m_pStream->GetObjectById(id, &pObj);

    if (SUCCEEDED(hr))
        *ppObj = pObj;

    return hr;
}

STDMETHODIMP
XEnumObject::GetDataObjectByName(LPCSTR name, IDirectXFileData **ppObj)
{
    HRESULT hr;

    ASSERT(ppObj != NULL);

    *ppObj = NULL;

    XDataObj *pObj;

    hr = m_pStream->GetObjectByName(name, &pObj);

    if (SUCCEEDED(hr))
        *ppObj = pObj;

    return hr;
}

