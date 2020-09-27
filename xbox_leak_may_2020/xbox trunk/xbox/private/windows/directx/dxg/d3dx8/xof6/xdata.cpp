//**************************************************************************
//
//  Copyright (C) 1998 Microsoft Corporation. All Rights Reserved.
//
//  File:   xdata.cpp
//
//  Description:    Implement data object classes.
//
//  History:
//      01/10/98    CongpaY     Created
//
//**************************************************************************

#include "precomp.h"

////////////////////////////////////////////////////////////////////////////
//
//  Implemenat XDataObj class.
//
////////////////////////////////////////////////////////////////////////////

XDataObj::XDataObj(XTemplate *pTemplate, LPCSTR szName, REFGUID id,
                   DWORD cbSize, LPVOID pData)
  : XObject(szName, id),
    m_pTemplate(pTemplate),
    m_plChildren(NULL),
    m_plDataPart(NULL),
    m_cbSize(0),
    m_pData(NULL)
{
    if (cbSize && XMalloc(&m_pData, cbSize) == DXFILE_OK) {
        memcpy(m_pData, pData, cbSize);
        m_cbSize = cbSize;
    }
}

XDataObj::XDataObj(XTemplate *pTemplate, LPCSTR szName, REFGUID id,
                   XDataPartList *plDataPart)
  : XObject(szName, id),
    m_pTemplate(pTemplate),
    m_plChildren(NULL),
    m_plDataPart(plDataPart),
    m_cbSize(0),
    m_pData(NULL)
{
}

HRESULT XDataObj::Create(REFGUID idType, LPCSTR szName, REFGUID id,
                         DWORD cbSize, LPVOID pData, XDataObj **ppObj)
{
    if (!cbSize && pData || cbSize && !pData)
        return DXFILEERR_BADVALUE;

    XTemplate *pTemplate;

    if (!XTemplateList::Find(idType, &pTemplate))
        return DXFILEERR_BADVALUE;

    *ppObj = new XDataObj(pTemplate, szName, id, cbSize, pData);

    if (!*ppObj)
        return DXFILEERR_BADALLOC;

    return DXFILE_OK;
}

XDataObj *
XDataObj::Create(LPCSTR szType, LPCSTR szName, REFGUID id, XDataPartList *plDataPart)
{
    XDataObj *pDataObj;
    XTemplate *pTemplate;

    if (!XTemplateList::Find(szType, &pTemplate)) {
        DPF(("Unknown object type %s.", szType));
        return NULL;
    }

    if (!plDataPart && pTemplate->ml()) {
        DPF(("No data for %s object.", szType));
        return NULL;
    }

    pDataObj = new XDataObj(pTemplate, szName, id, plDataPart);

    if (!pDataObj)
        return NULL;

    if(pDataObj->ProcessDataPartList() != DXFILE_OK) {
        DPF(("Bad data list for %s data object.", szType));
        pDataObj->Release();
        return NULL;
    }

    return pDataObj;
}

XDataObj::~XDataObj()
{
    if (m_plDataPart)
        delete m_plDataPart;

    if (m_pData)
        XFree(m_pData);

    if (m_plChildren)
        delete m_plChildren;
}

HRESULT XDataObj::ProcessDataPartList()
{
    HRESULT hr = DXFILE_OK;

    m_cbSize = 0;
    m_pData = NULL;

    if (!m_plDataPart)
        return hr;

    for (DWORD i = 0; i < m_plDataPart->count(); i++)
        (*m_plDataPart)[i]->ProcessSelf(this);

    if (SUCCEEDED(hr) && m_pTemplate->NeedRepack()) {
        LPVOID pMData, pData = m_pData;
        DWORD cbSize;

        hr = m_pTemplate->ModulateData(&pData, &cbSize, &pMData);

        if (SUCCEEDED(hr)) {
            XFree(m_pData);
            m_cbSize = cbSize;
            m_pData = pMData;
        }
    }

    return hr;
}

BOOL XDataObj::Allow(LPCSTR szType)
{
    if (m_pTemplate->IsOpen())
        return TRUE;

    XTemplateOptionList *plOptions = m_pTemplate->ol();

    if (plOptions) {
        for (DWORD i = 0; i < plOptions->count(); i++) {
            if (!xstricmp((*plOptions)[i]->name(), szType))
                return TRUE;
        }
    }

    return FALSE;
}

HRESULT XDataObj::AddChild(XObject *pChild)
{
    if (!m_plChildren) {
        m_plChildren = new XObjectList();

        if (!m_plChildren)
            return DXFILEERR_BADALLOC;
    }

    if (m_plChildren->Add(pChild) < 0)
        return DXFILEERR_BADALLOC;

    pChild->AddRef();

    return DXFILE_OK;
}

HRESULT XDataObj::AddData(LPVOID pData, DWORD cbData)
{
    HRESULT hr;

    if ((hr = XRealloc(&m_pData, m_cbSize + cbData)) != DXFILE_OK)
        return hr;

    memcpy((LPVOID)(((char *)m_pData) + m_cbSize), pData, cbData);

    m_cbSize += cbData;

    return DXFILE_OK;
}

XDataObj *XDataObj::FindDataObj(LPCSTR szName)
{
    if (name() && !xstrcmp(name(), szName))
        return this;

    if (m_plChildren) {
        for (DWORD i = 0; i < m_plChildren->count(); i++) {
            XDataObj *pObj;

            if (pObj = (*m_plChildren)[i]->FindDataObj(szName))
                return pObj;
        }
    }

    return NULL;
}

XDataObj *XDataObj::FindDataObj(REFGUID idSrc)
{
    if (id() == idSrc)
        return this;

    if (m_plChildren) {
        for (DWORD i = 0; i < m_plChildren->count(); i++) {
            XDataObj *pObj;

            if (pObj = (*m_plChildren)[i]->FindDataObj(idSrc))
                return pObj;
        }
    }

    return NULL;
}

STDMETHODIMP
XDataObj::QueryInterface(REFIID riid, LPVOID *ppvObj)
{
    HRESULT hr;

    ASSERT(ppvObj != NULL);

    *ppvObj = NULL;

    if (riid == IID_IUnknown || riid == IID_IDirectXFileData) {
        *ppvObj = (LPVOID)this;
        AddRef();
        hr = S_OK;
    } else {
        hr = E_NOINTERFACE;
    }

    return hr;
}

STDMETHODIMP
XDataObj::GetData(const CHAR *szMember, DWORD *pcbSize, void **ppvData)
{
    HRESULT hr;

    ASSERT(pcbSize != NULL && ppvData != NULL);

    if (szMember) {
        hr = m_pTemplate->GetSubData(m_pData, szMember, pcbSize, ppvData);
    } else {
        *pcbSize = m_cbSize;
        *ppvData = m_pData;
        hr = DXFILE_OK;
    }

    return hr;
}

STDMETHODIMP
XDataObj::GetType(const CLSID ** type)
{
    HRESULT hr;

    ASSERT(type != NULL);

    *type = NULL;

    if (m_pTemplate) {
        *type = &(m_pTemplate->id());
        hr = DXFILE_OK;
    } else {
        *type = NULL;
        hr = DXFILEERR_BADOBJECT;
    }

    return hr;
}

STDMETHODIMP
XDataObj::GetNextObject(IDirectXFileObject **ppObj)
{
    HRESULT hr;

    if (!ppObj) {
        return DXFILEERR_BADVALUE;
    }

    if (m_plChildren && m_iChild < m_plChildren->count()) {
        *ppObj = (IDirectXFileObject *)(*m_plChildren)[m_iChild];
        (*ppObj)->AddRef();
        m_iChild++;
        hr = DXFILE_OK;
    } else {
        *ppObj = NULL;
        hr = DXFILEERR_NOMOREOBJECTS;
    }

    return hr;
}

STDMETHODIMP
XDataObj::AddDataObject(IDirectXFileData *pChild)
{
    HRESULT hr;

    if (!pChild) {
        return DXFILEERR_BADVALUE;
    }

    if (!m_plChildren) {
        m_iChild = 0;
        m_plChildren = new XObjectList();
    }

    XDataObj *pDataObj = (XDataObj *)pChild;
    XObject *pObj = pDataObj;

    if (m_plChildren && m_plChildren->Add(pObj)) {
        pObj->AddRef();
        hr = DXFILE_OK;
    } else {
        hr = DXFILEERR_BADALLOC;
    }

    return hr;
}

STDMETHODIMP
XDataObj::AddDataReference(LPCSTR name, const GUID *uuid)
{
    HRESULT hr;

    if ((!name || !*name) && (!uuid || *uuid == GUID_NULL)) {
        DPF_ERR("No name or uuid specified.");
        return DXFILEERR_BADVALUE;
    }

    if (!m_plChildren) {
        m_iChild = 0;
        m_plChildren = new XObjectList();
    }

    XDataRef *pDataRef = new XDataRef(name, uuid? *uuid : GUID_NULL, 0);
    XObject *pObj = pDataRef;

    if (m_plChildren && pObj && m_plChildren->Add(pObj)) {
        hr = DXFILE_OK;
    } else {
        hr = DXFILEERR_BADALLOC;
    }

    return hr;
}

STDMETHODIMP
XDataObj::AddBinaryObject(LPCSTR name, const GUID *uuid,
                          LPCSTR mimeType, LPVOID data, DWORD cbSize)
{
    HRESULT hr;

    if (!data || !cbSize) {
        return DXFILEERR_BADVALUE;
    }

    if (!m_plChildren) {
        m_iChild = 0;
        m_plChildren = new XObjectList();
    }

    XBinaryData *pBinaryObj;

    pBinaryObj = new XBinaryData(name, uuid? *uuid : GUID_NULL,
                                 mimeType, data, cbSize);

    XObject *pObj = pBinaryObj;

    if (m_plChildren && pObj && m_plChildren->Add(pObj)) {
        hr = DXFILE_OK;
    } else {
        hr = DXFILEERR_BADALLOC;
    }

    return hr;
}

////////////////////////////////////////////////////////////////////////////
//
//  Implemenat XDataRef class
//
////////////////////////////////////////////////////////////////////////////

XDataRef::XDataRef(LPCSTR szRef, REFGUID idRef, XStreamRead *pStream)
  : XObject(NULL, GUID_NULL),
    m_szRef(szRef),
    m_idRef(idRef),
    m_pStream(pStream)
{
}


STDMETHODIMP
XDataRef::QueryInterface(REFIID riid, LPVOID *ppvObj)
{
    HRESULT hr;

    ASSERT(ppvObj != NULL);

    *ppvObj = NULL;

    if (riid == IID_IUnknown || riid == IID_IDirectXFileDataReference) {
        *ppvObj = (LPVOID)this;
        AddRef();
        hr = S_OK;
    } else {
        hr = E_NOINTERFACE;
    }

    return hr;
}

STDMETHODIMP
XDataRef::Resolve(IDirectXFileData **ppDataObj)
{
    HRESULT hr;

    ASSERT(ppDataObj != NULL);

    *ppDataObj = NULL;

    XDataObj *pObj;

    if (m_szRef) {
        if (m_idRef == GUID_NULL)
            hr = m_pStream->GetObjectByName(m_szRef, &pObj);
        else
            hr = m_pStream->GetObjectByNameAndId(m_szRef, m_idRef, &pObj);
    } else if (m_idRef != GUID_NULL)
        hr = m_pStream->GetObjectById(m_idRef, &pObj);
    else
        hr = DXFILEERR_BADOBJECT;

    if (hr == DXFILE_OK) {
        pObj->ResetChildIndex();
        *ppDataObj = pObj;
    }

    return hr;
}

STDMETHODIMP_(ULONG)
stringList::Release()
{
    for (DWORD i = 0; i < count(); i++)
        XFree((LPVOID)(*this)[i]);

    delete this;
    return 0;
}
