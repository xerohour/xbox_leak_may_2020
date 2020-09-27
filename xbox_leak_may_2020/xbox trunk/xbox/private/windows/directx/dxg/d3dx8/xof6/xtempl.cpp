//**************************************************************************
//
//  Copyright (C) 1998 Microsoft Corporation. All Rights Reserved.
//
//  File:   xtempl.cpp
//
//  Description:    Implement XTemplateList class.
//
//  History:
//      01/09/98    CongpaY     Created
//
//**************************************************************************

#include "precomp.h"

#define TEMPLATE_LIST_CHUNK_SIZE 100

typedef XPtrArray<XTemplate, TEMPLATE_LIST_CHUNK_SIZE> XTemplateListImpl;

static XTemplateListImpl *spTemplateList;

////////////////////////////////////////////////////////////////////////////
//
//  Implement XIdentifier class
//
////////////////////////////////////////////////////////////////////////////

XIdentifier *XIdentifier::Create(LPCSTR szName, REFGUID id)
{
    XTemplate *pTemplate;

    if (szName && XTemplateList::Find(szName, &pTemplate)) {
        return new XIdentifier(szName, pTemplate->id());
    }

    if (XTemplateList::Find(id, &pTemplate)) {
        return new XIdentifier(pTemplate->name(), id);
    }

    if (szName && !xstricmp(szName, SZBINARY))
        return new XIdentifier(SZBINARY, GUID_NULL);

    return NULL;
}

////////////////////////////////////////////////////////////////////////////
//
//  Implement XTemplateList class
//
////////////////////////////////////////////////////////////////////////////

BOOL XTemplateList::Init()
{
    if (spTemplateList == NULL)
    {
        spTemplateList = new XTemplateListImpl();
    }

    return spTemplateList != NULL;
}

int XTemplateList::Add(XTemplate *pSrc)
{
    if (Find(pSrc->name())) {
        delete pSrc;
        return 0;
    }

    return spTemplateList->Add(pSrc);
}

BOOL XTemplateList::Find(LPCSTR szName, XTemplate **ppTemplate)
{
    for (DWORD i = 0; i < spTemplateList->count(); i++) {
        if (!xstricmp((*spTemplateList)[i]->name(), szName)) {
            if (ppTemplate)
                *ppTemplate = (*spTemplateList)[i];
            return TRUE;
        }
    }

    return FALSE;
}

BOOL XTemplateList::Find(REFGUID id, XTemplate **ppTemplate)
{
    for (DWORD i = 0; i < spTemplateList->count(); i++) {
        if ((*spTemplateList)[i]->id() == id) {
            if (ppTemplate)
                *ppTemplate = (*spTemplateList)[i];
            return TRUE;
        }
    }

    return FALSE;
}

void XTemplateList::Free()
{
}

////////////////////////////////////////////////////////////////////////////
//
//  Implement XTemplateMember class
//
////////////////////////////////////////////////////////////////////////////

XTemplateMember::XTemplateMember(LPCSTR szName,
                                 const XPrimitive *pPrim,
                                 XTemplate *pTemplate,
                                 XDimensionList *plDim)
  : m_szName(szName),
    m_pPrim(pPrim),
    m_pTemplate(pTemplate),
    m_plDim(plDim)
{
}

XTemplateMember *
XTemplateMember::Create(LPCSTR szType, LPCSTR szName, XDimensionList *plDim)
{
    const XPrimitive *pPrim;
    XTemplate *pTemplate;
    XTemplateMember *pMember;

    if (pPrim = XPrimitiveFromName(szType))
        pMember = new XTemplateMember(szName, pPrim, NULL, plDim);
    else if (XTemplateList::Find(szType, &pTemplate))
        pMember = new XTemplateMember(szName, NULL, pTemplate, plDim);
    else
        pMember = NULL;

    return pMember;
}

XTemplateMember::~XTemplateMember()
{
    if (m_plDim)
        delete m_plDim;
}

LPCSTR XTemplateMember::type()
{
    if (m_pPrim)
        return m_pPrim->name;
    else if (m_pTemplate)
        return m_pTemplate->name();
    else
        return NULL;
}

BOOL XTemplateMember::NeedRepack()
{
    if (m_pPrim)
        return m_pPrim->size != 4;
    else
        return m_pTemplate->NeedRepack();
}

HRESULT XTemplateMember::ResolveArraySize(XTemplate *pParent,
                                          DWORD *pcArraySize)
{
    DWORD cArraySize = 1;

    for (DWORD i = 0; i < m_plDim->count(); i++) {
        XDimension *pDim = (*m_plDim)[i];

        if (pDim->m_cElements) {
            cArraySize *= pDim->m_cElements;
        } else if (pDim->m_szElements) {
            HRESULT hr;
            LPVOID pData;

            hr = pParent->GetMemberData(this, pDim->m_szElements, &pData);

            if (FAILED(hr))
                return hr;

            cArraySize *= *((DWORD *)pData);
        } else {
            return DXFILEERR_BADARRAYSIZE;
        }
    }

    *pcArraySize = cArraySize;
    return DXFILE_OK;
}

HRESULT XTemplateMember::ModulateSingleData(LPVOID *ppvData,
                                            DWORD *pcbSize,
                                            LPVOID *ppvRet)
{
    HRESULT hr;
    LPCSTR pData = (LPCSTR)*ppvData;

    m_pData = *ppvData;

    if (m_pTemplate) {
        hr = m_pTemplate->ModulateData(ppvData, pcbSize, ppvRet);
        *ppvData = (LPVOID)pData;
        return hr;
    }

    if (FAILED(hr = XRealloc(ppvRet, *pcbSize + m_pPrim->size)))
        return hr;

    LPSTR p = (LPSTR)*ppvRet;
    p += *pcbSize;

    switch (m_pPrim->type) {
    case X_DWord:
    case X_SDWord:
        *(DWORD *)p = *(DWORD *)pData;
        break;
    case X_Word:
    case X_SWord:
        *(WORD *)p = (WORD)*(DWORD *)pData;
        break;
    case X_Char:
    case X_UChar:
        *p = (char)*(DWORD *)pData;
        break;
    case X_Float:
        *(float *)p = *(float *)pData;
        break;
    case X_Double:
        *(double *)p = (double)*(float *)pData;
        break;
    case X_Lpstr:
        *(LPSTR *)p = *(LPSTR *)pData;
        break;
    default:
        hr = DXFILEERR_NOTDONEYET;
        break;
    }

    *pcbSize += m_pPrim->size;
    *ppvData = (LPVOID)(pData + 4);

    return hr;
}

HRESULT XTemplateMember::ModulateData(LPVOID *ppvData,
                                     DWORD *pcbSize,
                                     LPVOID *ppvRet,
                                     XTemplate *pParent)
{
    if (!m_plDim)
        return ModulateSingleData(ppvData, pcbSize, ppvRet);

    // It's an array. Resolve array size.

    HRESULT hr;
    DWORD cArraySize;

    hr = ResolveArraySize(pParent, &cArraySize);

    if (FAILED(hr))
        return hr;

    // Modulate array data.

    for (DWORD i = 0; i < cArraySize; i++) {
        hr = ModulateSingleData(ppvData, pcbSize, ppvRet);

        if (FAILED(hr))
            break;
    }

    return hr;
}

DWORD XTemplateMember::GetSize(XTemplate *pParent)
{
    DWORD cbSize;

    if (m_pPrim)
        cbSize = m_pPrim->size;
    else
        cbSize = m_pTemplate->GetSize(m_pData);

    if (m_plDim) {
        DWORD cArraySize;
        ResolveArraySize(pParent, &cArraySize);
        cbSize *= cArraySize;
    }

    return cbSize;
}

////////////////////////////////////////////////////////////////////////////
//
//  Implement XTemplate class
//
////////////////////////////////////////////////////////////////////////////

XTemplate::XTemplate(LPCSTR szName, REFGUID id, XTemplateInfo *pTemplInfo)
  : m_szName(szName),
    m_id(id),
    m_plMembers(pTemplInfo->m_plMembers),
    m_plOptions(pTemplInfo->m_plOptions),
    m_type(pTemplInfo->m_type),
    m_fRepack(FALSE)
{
    if (m_plMembers) {
        for (DWORD i = 0; i < m_plMembers->count(); i++) {
            m_fRepack |= (*m_plMembers)[i]->NeedRepack();
        }
    }
}

XTemplate::~XTemplate()
{
    if (m_plMembers)
        delete m_plMembers;

    if (m_plOptions)
        delete m_plOptions;
}

HRESULT XTemplate::GetMemberData(XTemplateMember *pMemberCur,
                                 LPCSTR szMember,
                                 LPVOID *ppvData)
{
    for (DWORD i = 0; i < m_plMembers->count(); i++) {
        XTemplateMember *pMember = (*m_plMembers)[i];

        if (pMember == pMemberCur)
            break;

        if (!xstricmp(pMember->name(), szMember)) {
            *ppvData = pMember->m_pData;
            return DXFILE_OK;
        }
    }
    return DXFILEERR_BADDATAREFERENCE;
}

HRESULT XTemplate::ModulateData(LPVOID *ppvData, DWORD *pcbSize, LPVOID *ppvRet)
{
    *pcbSize = 0;
    *ppvRet = NULL;

    if (!m_plMembers)
        return DXFILE_OK;

    HRESULT hr;

    for (DWORD i = 0; i < m_plMembers->count(); i++) {
        hr = (*m_plMembers)[i]->ModulateData(ppvData, pcbSize, ppvRet, this);

        if (hr != DXFILE_OK)
            break;
    }

    return hr;
}

DWORD XTemplate::GetSize(LPVOID pvSrc)
{
    LPCSTR pData = (LPCSTR)pvSrc;

    for (DWORD i = 0; i < m_plMembers->count(); i++) {
        DWORD cbInc;
        XTemplateMember *pMember = (*m_plMembers)[i];

        pMember->m_pData = (LPVOID)pData;
        pData += pMember->GetSize(this);
    }

    return (DWORD)(pData - (LPCSTR)pvSrc);
}

HRESULT XTemplate::GetSubData(LPVOID pvSrc, LPCSTR szMember,
                              DWORD *pcbSize, LPVOID *ppvRet)
{
    LPCSTR pData = (LPCSTR)pvSrc;
    DWORD cbSize;

    for (DWORD i = 0; i < m_plMembers->count(); i++) {
        XTemplateMember *pMember = (*m_plMembers)[i];

        pMember->m_pData = (LPVOID)pData;
        cbSize = pMember->GetSize(this);

        if (!xstricmp(pMember->name(), szMember)) {
            *ppvRet = (LPVOID)pData;
            *pcbSize = cbSize;
            return DXFILE_OK;
        }

        pData += cbSize;
    }
    return DXFILEERR_BADDATAREFERENCE;
}

