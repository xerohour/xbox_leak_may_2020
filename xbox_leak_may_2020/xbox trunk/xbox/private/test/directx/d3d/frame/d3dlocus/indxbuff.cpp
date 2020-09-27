/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    indxbuff.cpp

Author:

    Matt Bronder

Description:

    CIndexBuffer methods.

*******************************************************************************/

#include "d3dlocus.h"

//******************************************************************************
// CIndexBuffer
//******************************************************************************

//******************************************************************************
CIndexBuffer::CIndexBuffer() {
}

//******************************************************************************
CIndexBuffer::~CIndexBuffer() {
}

//******************************************************************************
// CIndexBuffer8
//******************************************************************************

//******************************************************************************
CIndexBuffer8::CIndexBuffer8() {

    m_pd3di = NULL;
}

//******************************************************************************
CIndexBuffer8::~CIndexBuffer8() {
}

//******************************************************************************
BOOL CIndexBuffer8::Create(CDevice8* pDevice, LPDIRECT3DINDEXBUFFER8 pd3di) {

    if (m_pd3di) {
        return FALSE;
    }

    if (pDevice && pd3di) {

        if (CResource8::Create(pDevice, (LPDIRECT3DRESOURCE8)pd3di)) {
            m_pd3di = pd3di;
        }
    }

    return (BOOL)m_pd3di;
}

//******************************************************************************
LPDIRECT3DINDEXBUFFER8 CIndexBuffer8::GetIDirect3DIndexBuffer8() {

    return m_pd3di;
}

//******************************************************************************
HRESULT CIndexBuffer8::GetDesc(D3DINDEXBUFFER_DESC* pd3dibd) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3di) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    return m_pd3di->GetDesc(pd3dibd);
}

//******************************************************************************
HRESULT CIndexBuffer8::Lock(UINT uOffsetToLock, UINT uSizeToLock, BYTE** ppdata, DWORD dwFlags) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3di) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    return m_pd3di->Lock(uOffsetToLock, uSizeToLock, ppdata, dwFlags);
}

//******************************************************************************
HRESULT CIndexBuffer8::Unlock() {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3di) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    return m_pd3di->Unlock();
}

//******************************************************************************
// CCIndexBuffer8
//******************************************************************************

//******************************************************************************
CCIndexBuffer8::CCIndexBuffer8() {
}

//******************************************************************************
CCIndexBuffer8::~CCIndexBuffer8() {
}

//******************************************************************************
BOOL CCIndexBuffer8::Create(CClient* pClient, CDevice8* pDevice, LPDIRECT3DINDEXBUFFER8 pd3di) {

    if (!pClient) {
        return FALSE;
    }
    m_pClient = pClient;
    return CIndexBuffer8::Create(pDevice, pd3di);
}

//******************************************************************************
ULONG CCIndexBuffer8::AddRef() {

    return CCResource8_AddRef(this, m_pClient);
}

//******************************************************************************
ULONG CCIndexBuffer8::Release() {

    return CCResource8_Release(this, m_pClient);
}

//******************************************************************************
D3DRESOURCETYPE CCIndexBuffer8::GetType() {

    return CCResource8_GetType(this, m_pClient);
}

//******************************************************************************
HRESULT CCIndexBuffer8::GetDevice(CDevice8** ppDevice) {

    return CCResource8_GetDevice(this, m_pClient, ppDevice);
}

//******************************************************************************
HRESULT CCIndexBuffer8::GetPrivateData(REFGUID refguid, void* pvData, DWORD* pdwSizeOfData) {

    return CCResource8_GetPrivateData(this, m_pClient, refguid, pvData, pdwSizeOfData);
}

//******************************************************************************
HRESULT CCIndexBuffer8::SetPrivateData(REFGUID refguid, void* pvData, DWORD dwSizeOfData, DWORD dwFlags) {

    return CCResource8_SetPrivateData(this, m_pClient, refguid, pvData, dwSizeOfData, dwFlags);
}

//******************************************************************************
HRESULT CCIndexBuffer8::FreePrivateData(REFGUID refguid) {

    return CCResource8_FreePrivateData(this, m_pClient, refguid);
}

//******************************************************************************
DWORD CCIndexBuffer8::GetPriority() {

    return CCResource8_GetPriority(this, m_pClient);
}

//******************************************************************************
DWORD CCIndexBuffer8::SetPriority(DWORD dwNewPriority) {

    return CCResource8_SetPriority(this, m_pClient, dwNewPriority);
}

//******************************************************************************
void CCIndexBuffer8::PreLoad() {

    CCResource8_PreLoad(this, m_pClient);
}

//******************************************************************************
HRESULT CCIndexBuffer8::GetDesc(D3DINDEXBUFFER_DESC* pd3dibd) {

#ifndef UNDER_XBOX
    D3DINDEXBUFFER_DESC         d3dibd;
#else
    WINX_D3DINDEXBUFFER_DESC    d3dibd;
#endif // UNDER_XBOX
    HRESULT                     hrC, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3di) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    hrC = m_pd3di->GetDesc(pd3dibd);

#ifndef UNDER_XBOX
    if (m_pClient->CallMethod(MID_IXB8_GetDesc, this, &hrS, &d3dibd, sizeof(D3DINDEXBUFFER_DESC))) {
#else
    if (m_pClient->CallMethod(MID_IXB8_GetDesc, this, &hrS, &d3dibd, sizeof(WINX_D3DINDEXBUFFER_DESC))) {
#endif // UNDER_XBOX
        if (hrC != hrS) {
            Log(LOG_WARN, TEXT("IDirect3DIndexBuffer8::GetDesc result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
        }
        else {
#ifndef UNDER_XBOX
            if (memcmp(pd3dibd, &d3dibd, sizeof(D3DINDEXBUFFER_DESC))) {
#else
            if (pd3dibd->Format != d3dibd.Format || pd3dibd->Type != d3dibd.Format) {
#endif // UNDER_XBOX
                Log(LOG_WARN, TEXT("IDirect3DIndexBuffer8::GetDesc returned an inconsistent description"));
            }
        }
    }

    return hrC;
}

//******************************************************************************
HRESULT CCIndexBuffer8::Lock(UINT uOffsetToLock, UINT uSizeToLock, BYTE** ppdata, DWORD dwFlags) {

    HRESULT hrC, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3di) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    hrC = m_pd3di->Lock(uOffsetToLock, uSizeToLock, ppdata, dwFlags);

    if (dwFlags & D3DLOCK_READONLY) {
        m_ilockd.pData = NULL;
        m_ilockd.uSize = 0;
    }
    else {
        m_ilockd.pData = *ppdata;
        if (uSizeToLock) {
            m_ilockd.uSize = uSizeToLock;
        }
        else {
#ifndef UNDER_XBOX
            D3DINDEXBUFFER_DESC d3dibd;
            m_pd3di->GetDesc(&d3dibd);
#else
            WINX_D3DINDEXBUFFER_DESC d3dibd;
            if (!m_pClient->CallMethod(MID_IXB8_GetDesc, this, &hrS, &d3dibd, sizeof(WINX_D3DINDEXBUFFER_DESC)) || FAILED(hrS)) {
                d3dibd.Size = 0;
            }
#endif // UNDER_XBOX
            m_ilockd.uSize = d3dibd.Size;
        }
    }

    if (m_pClient->CallMethod(MID_IXB8_Lock, this, &hrS, uOffsetToLock, uSizeToLock, &m_ilockd.pDataSrv, dwFlags)) {
        if (hrC != hrS) {
            Log(LOG_WARN, TEXT("IDirect3DIndexBuffer8::Lock result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
        }
    }

    return hrC;
}

//******************************************************************************
HRESULT CCIndexBuffer8::Unlock() {

    HRESULT hrC, hrS;
    BOOL    bRet;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3di) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    bRet = m_pClient->CallMethod(MID_IXB8_Unlock, this, &hrS, m_ilockd.pDataSrv, m_ilockd.uSize, m_ilockd.pData, m_ilockd.uSize);

    hrC = m_pd3di->Unlock();

    if (bRet) {
        if (hrC != hrS) {
            Log(LOG_WARN, TEXT("IDirect3DIndexBuffer8::Unlock result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
        }
    }

    return hrC;
}
