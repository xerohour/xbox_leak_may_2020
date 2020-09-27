/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    vertbuff.cpp

Author:

    Matt Bronder

Description:

    CVertexBuffer methods.

*******************************************************************************/

#include "d3dlocus.h"

//******************************************************************************
// CVertexBuffer
//******************************************************************************

//******************************************************************************
CVertexBuffer::CVertexBuffer() {
}

//******************************************************************************
CVertexBuffer::~CVertexBuffer() {
}

//******************************************************************************
// CVertexBuffer8
//******************************************************************************

//******************************************************************************
CVertexBuffer8::CVertexBuffer8() {

    m_pd3dr = NULL;
}

//******************************************************************************
CVertexBuffer8::~CVertexBuffer8() {
}

//******************************************************************************
BOOL CVertexBuffer8::Create(CDevice8* pDevice, LPDIRECT3DVERTEXBUFFER8 pd3dr) {

    if (m_pd3dr) {
        return FALSE;
    }

    if (pDevice && pd3dr) {

        if (CResource8::Create(pDevice, (LPDIRECT3DRESOURCE8)pd3dr)) {
            m_pd3dr = pd3dr;
        }
    }

    return (BOOL)m_pd3dr;
}

//******************************************************************************
LPDIRECT3DVERTEXBUFFER8 CVertexBuffer8::GetIDirect3DVertexBuffer8() {

    return m_pd3dr;
}

//******************************************************************************
HRESULT CVertexBuffer8::GetDesc(D3DVERTEXBUFFER_DESC* pd3dvbd) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dr) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    return m_pd3dr->GetDesc(pd3dvbd);
}

//******************************************************************************
HRESULT CVertexBuffer8::Lock(UINT uOffsetToLock, UINT uSizeToLock, BYTE** ppdata, DWORD dwFlags) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dr) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    return m_pd3dr->Lock(uOffsetToLock, uSizeToLock, ppdata, dwFlags);
}

//******************************************************************************
HRESULT CVertexBuffer8::Unlock() {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dr) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    return m_pd3dr->Unlock();
}

//******************************************************************************
// CCVertexBuffer8
//******************************************************************************

//******************************************************************************
CCVertexBuffer8::CCVertexBuffer8() {
}

//******************************************************************************
CCVertexBuffer8::~CCVertexBuffer8() {
}

//******************************************************************************
BOOL CCVertexBuffer8::Create(CClient* pClient, CDevice8* pDevice, LPDIRECT3DVERTEXBUFFER8 pd3dr) {

    if (!pClient) {
        return FALSE;
    }
    m_pClient = pClient;
    return CVertexBuffer8::Create(pDevice, pd3dr);
}

//******************************************************************************
ULONG CCVertexBuffer8::AddRef() {

    return CCResource8_AddRef(this, m_pClient);
}

//******************************************************************************
ULONG CCVertexBuffer8::Release() {

    return CCResource8_Release(this, m_pClient);
}

//******************************************************************************
D3DRESOURCETYPE CCVertexBuffer8::GetType() {

    return CCResource8_GetType(this, m_pClient);
}

//******************************************************************************
HRESULT CCVertexBuffer8::GetDevice(CDevice8** ppDevice) {

    return CCResource8_GetDevice(this, m_pClient, ppDevice);
}

//******************************************************************************
HRESULT CCVertexBuffer8::GetPrivateData(REFGUID refguid, void* pvData, DWORD* pdwSizeOfData) {

    return CCResource8_GetPrivateData(this, m_pClient, refguid, pvData, pdwSizeOfData);
}

//******************************************************************************
HRESULT CCVertexBuffer8::SetPrivateData(REFGUID refguid, void* pvData, DWORD dwSizeOfData, DWORD dwFlags) {

    return CCResource8_SetPrivateData(this, m_pClient, refguid, pvData, dwSizeOfData, dwFlags);
}

//******************************************************************************
HRESULT CCVertexBuffer8::FreePrivateData(REFGUID refguid) {

    return CCResource8_FreePrivateData(this, m_pClient, refguid);
}

//******************************************************************************
DWORD CCVertexBuffer8::GetPriority() {

    return CCResource8_GetPriority(this, m_pClient);
}

//******************************************************************************
DWORD CCVertexBuffer8::SetPriority(DWORD dwNewPriority) {

    return CCResource8_SetPriority(this, m_pClient, dwNewPriority);
}

//******************************************************************************
void CCVertexBuffer8::PreLoad() {

    CCResource8_PreLoad(this, m_pClient);
}

//******************************************************************************
HRESULT CCVertexBuffer8::GetDesc(D3DVERTEXBUFFER_DESC* pd3dvbd) {

#ifndef UNDER_XBOX
    D3DVERTEXBUFFER_DESC        d3dvbd;
#else
    WINX_D3DVERTEXBUFFER_DESC   d3dvbd;
#endif // UNDER_XBOX
    HRESULT                     hrC, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dr) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    hrC = m_pd3dr->GetDesc(pd3dvbd);

#ifndef UNDER_XBOX
    if (m_pClient->CallMethod(MID_VRB8_GetDesc, this, &hrS, &d3dvbd, sizeof(D3DVERTEXBUFFER_DESC))) {
#else
    if (m_pClient->CallMethod(MID_VRB8_GetDesc, this, &hrS, &d3dvbd, sizeof(WINX_D3DVERTEXBUFFER_DESC))) {
#endif // UNDER_XBOX
        if (hrC != hrS) {
            Log(LOG_WARN, TEXT("IDirect3DVertexBuffer8::GetDesc result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
        }
        else {
#ifndef UNDER_XBOX
            if (memcmp(pd3dvbd, &d3dvbd, sizeof(D3DVERTEXBUFFER_DESC))) {
#else
            if (pd3dvbd->Format != d3dvbd.Format || pd3dvbd->Type != d3dvbd.Format) {
#endif // UNDER_XBOX
                Log(LOG_WARN, TEXT("IDirect3DVertexBuffer8::GetDesc returned an inconsistent description"));
            }
        }
    }

    return hrC;
}

//******************************************************************************
HRESULT CCVertexBuffer8::Lock(UINT uOffsetToLock, UINT uSizeToLock, BYTE** ppdata, DWORD dwFlags) {

    HRESULT hrC, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dr) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    hrC = m_pd3dr->Lock(uOffsetToLock, uSizeToLock, ppdata, dwFlags);

    if (dwFlags & D3DLOCK_READONLY) {
        m_rlockd.pData = NULL;
        m_rlockd.uSize = 0;
    }
    else {
        m_rlockd.pData = *ppdata;
        if (uSizeToLock) {
            m_rlockd.uSize = uSizeToLock;
        }
        else {
#ifndef UNDER_XBOX
            D3DVERTEXBUFFER_DESC d3dvbd;
            m_pd3dr->GetDesc(&d3dvbd);
#else
            WINX_D3DVERTEXBUFFER_DESC d3dvbd;
            if (!m_pClient->CallMethod(MID_VRB8_GetDesc, this, &hrS, &d3dvbd, sizeof(WINX_D3DVERTEXBUFFER_DESC)) || FAILED(hrS)) {
                d3dvbd.Size = 0;
            }
#endif // UNDER_XBOX
            m_rlockd.uSize = d3dvbd.Size;
        }
    }

    if (m_pClient->CallMethod(MID_VRB8_Lock, this, &hrS, uOffsetToLock, uSizeToLock, &m_rlockd.pDataSrv, dwFlags)) {
        if (hrC != hrS) {
            Log(LOG_WARN, TEXT("IDirect3DVertexBuffer8::Lock result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
        }
    }

    return hrC;
}

//******************************************************************************
HRESULT CCVertexBuffer8::Unlock() {

    HRESULT hrC, hrS;
    BOOL    bRet;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dr) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    bRet = m_pClient->CallMethod(MID_VRB8_Unlock, this, &hrS, m_rlockd.pDataSrv, m_rlockd.uSize, m_rlockd.pData, m_rlockd.uSize);

    hrC = m_pd3dr->Unlock();

    if (bRet) {
        if (hrC != hrS) {
            Log(LOG_WARN, TEXT("IDirect3DVertexBuffer8::Unlock result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
        }
    }

    return hrC;
}

