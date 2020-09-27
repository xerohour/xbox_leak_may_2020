/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    resource.cpp

Author:

    Matt Bronder

Description:

    CResource methods.

*******************************************************************************/

#include "d3dlocus.h"

//******************************************************************************
// CResource
//******************************************************************************

//******************************************************************************
CResource::CResource() {
}

//******************************************************************************
CResource::~CResource() {
}

//******************************************************************************
// CResource8
//******************************************************************************

//******************************************************************************
CResource8::CResource8() {

    m_pd3dres = NULL;
}

//******************************************************************************
CResource8::~CResource8() {

    if (m_pDevice) {
        m_pDevice->RemoveObject(m_pd3dres);
    }
}

//******************************************************************************
BOOL CResource8::Create(CDevice8* pDevice, LPDIRECT3DRESOURCE8 pd3dres) {

    if (m_pd3dres) {
        return FALSE;
    }

    if (pDevice && pd3dres) {

        if (!pDevice->AddObject(pd3dres, this)) {
            return FALSE;
        }

        m_pDevice = pDevice;

        m_pd3dres = pd3dres;
    }

    return (BOOL)m_pd3dres;
}

//******************************************************************************
ULONG CResource8::AddRef() {

    ULONG uRef = 0;

    if (m_pd3dres) {
        uRef = m_pd3dres->AddRef();
        CObject::AddRef();
    }

    return uRef;
}

//******************************************************************************
ULONG CResource8::Release() {

    ULONG uRef = 0;

    if (m_pd3dres) {
        uRef = m_pd3dres->Release();
        CObject::Release();
    }

    return uRef;
}

//******************************************************************************
LPDIRECT3DRESOURCE8 CResource8::GetIDirect3DResource8() {

    return m_pd3dres;
}

//******************************************************************************
CDevice8* CResource8::GetDevice8() {

    return m_pDevice;
}

//******************************************************************************
D3DRESOURCETYPE CResource8::GetType() {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dres) {
        return (D3DRESOURCETYPE)0;
    }
#endif // D3DLOCUS_DEBUG

    return m_pd3dres->GetType();
}

//******************************************************************************
HRESULT CResource8::GetDevice(CDevice8** ppDevice) {

    LPDIRECT3DDEVICE8 pd3dd;
    HRESULT           hr;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dres) {
        return ERR_NOTCREATED;
    }
    if (!ppDevice) {
        return D3DERR_INVALIDCALL;
    }
#endif // D3DLOCUS_DEBUG

    *ppDevice = NULL;

    hr = m_pd3dres->GetDevice(&pd3dd);
    if (SUCCEEDED(hr)) {
        if (pd3dd != m_pDevice->GetIDirect3DDevice8()) {
            DebugString(TEXT("Interface pointers differ for IDirect3DDevice8"));
            DebugBreak();
        }
        m_pDevice->CObject::AddRef();
        *ppDevice = m_pDevice;
    }

    return hr;
}

//******************************************************************************
HRESULT CResource8::GetPrivateData(REFGUID refguid, void* pvData, DWORD* pdwSizeOfData) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dres) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    return m_pd3dres->GetPrivateData(refguid, pvData, pdwSizeOfData);
}

//******************************************************************************
HRESULT CResource8::SetPrivateData(REFGUID refguid, void* pvData, DWORD dwSizeOfData, DWORD dwFlags) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dres) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    return m_pd3dres->SetPrivateData(refguid, pvData, dwSizeOfData, dwFlags);
}

//******************************************************************************
HRESULT CResource8::FreePrivateData(REFGUID refguid) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dres) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    return m_pd3dres->FreePrivateData(refguid);
}

//******************************************************************************
DWORD CResource8::GetPriority() {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dres) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

#ifndef UNDER_XBOX
    return m_pd3dres->GetPriority();
#else
    Log(LOG_FAIL, TEXT("IDirect3DResource8::GetPriority is not implemented on XBox"));
    return 0;
#endif // UNDER_XBOX
}

//******************************************************************************
DWORD CResource8::SetPriority(DWORD dwNewPriority) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dres) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

#ifndef UNDER_XBOX
    return m_pd3dres->SetPriority(dwNewPriority);
#else
    Log(LOG_FAIL, TEXT("IDirect3DResource8::SetPriority is not implemented on XBox"));
    return 0;
#endif // UNDER_XBOX
}

//******************************************************************************
void CResource8::PreLoad() {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dres) {
        return;
    }
#endif // D3DLOCUS_DEBUG

#ifndef UNDER_XBOX
    m_pd3dres->PreLoad();
#else
    Log(LOG_FAIL, TEXT("IDirect3DResource8::PreLoad is not implemented on XBox"));
    return;
#endif // UNDER_XBOX
}

//******************************************************************************
// CCResource8
//******************************************************************************

//******************************************************************************
ULONG CCResource8_AddRef(CResource8* pResource, CClient* pClient) {

    LPDIRECT3DRESOURCE8 pd3dres = pResource->GetIDirect3DResource8();
    ULONG               uRefC = 0, uRefS;

    if (pd3dres) {
        uRefC = pd3dres->AddRef();
        if (pClient->CallMethod(MID_RES8_AddRef, pResource, &uRefS)) {
#ifndef UNDER_XBOX
            if (uRefC != uRefS) {
                Log(LOG_WARN, TEXT("IDirect3DResource8::AddRef result inconsistent [C: %d, S: %d]"), uRefC, uRefS);
            }
#endif // !UNDER_XBOX
        }
        pResource->CObject::AddRef();
    }

    return uRefC;
}

//******************************************************************************
ULONG CCResource8_Release(CResource8* pResource, CClient* pClient) {

    LPDIRECT3DRESOURCE8 pd3dres = pResource->GetIDirect3DResource8();
    ULONG               uRefC = 0, uRefS;

    if (pd3dres) {
        uRefC = pd3dres->Release();
        if (pClient->CallMethod(MID_RES8_Release, pResource, &uRefS)) {
#ifndef UNDER_XBOX
            if (uRefC != uRefS) {
                Log(LOG_WARN, TEXT("IDirect3DResource8::Release result inconsistent [C: %d, S: %d]"), uRefC, uRefS);
            }
#endif // !UNDER_XBOX
        }
        pResource->CObject::Release();
    }

    return uRefC;
}

//******************************************************************************
D3DRESOURCETYPE CCResource8_GetType(CResource8* pResource, CClient* pClient) {

    LPDIRECT3DRESOURCE8 pd3dres = pResource->GetIDirect3DResource8();
    D3DRESOURCETYPE     d3drtC, d3drtS;

#ifdef D3DLOCUS_DEBUG
    if (!pd3dres) {
        return (D3DRESOURCETYPE)0;
    }
#endif // D3DLOCUS_DEBUG

    d3drtC = pd3dres->GetType();

    if (pClient->CallMethod(MID_RES8_GetType, pResource, &d3drtS)) {
        if (d3drtC != d3drtS) {
            Log(LOG_WARN, TEXT("IDirect3DResource8::GetType result inconsistent [C: %d, S: %d]"), d3drtC, d3drtS);
        }
    }

    return d3drtC;
}

//******************************************************************************
HRESULT CCResource8_GetDevice(CResource8* pResource, CClient* pClient, CDevice8** ppDevice) {

    LPDIRECT3DRESOURCE8 pd3dres = pResource->GetIDirect3DResource8();
    LPDIRECT3DDEVICE8   pd3dd;
    CDevice8*           pDevice;
    HRESULT             hrC, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!pd3dres) {
        return ERR_NOTCREATED;
    }
    if (!ppDevice) {
        return D3DERR_INVALIDCALL;
    }
#endif // D3DLOCUS_DEBUG

    *ppDevice = NULL;

    hrC = pd3dres->GetDevice(&pd3dd);
    if (SUCCEEDED(hrC)) {
        pDevice = pResource->GetDevice8();
        if (pd3dd != pDevice->GetIDirect3DDevice8()) {
            DebugString(TEXT("Interface pointers differ for IDirect3DDevice8"));
            DebugBreak();
        }
        pDevice->CObject::AddRef();
        *ppDevice = pDevice;
    }
    else {
        pDevice = NULL;
    }

    if (pClient->CallMethod(MID_RES8_GetDevice, pResource, &hrS, &pDevice)) {
        if (hrC != hrS) {
            Log(LOG_WARN, TEXT("IDirect3DResource8::GetDevice result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
        }
    }

    return hrC;
}

//******************************************************************************
HRESULT CCResource8_GetPrivateData(CResource8* pResource, CClient* pClient, REFGUID refguid, void* pvData, DWORD* pdwSizeOfData) {

    LPDIRECT3DRESOURCE8 pd3dres = pResource->GetIDirect3DResource8();
    HRESULT             hrC, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!pd3dres) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    hrC = pd3dres->GetPrivateData(refguid, pvData, pdwSizeOfData);

    if (pClient->CallMethod(MID_RES8_GetPrivateData, pResource, &hrS, &refguid, sizeof(GUID), pvData, *pdwSizeOfData, pdwSizeOfData)) {
        if (hrC != hrS) {
            Log(LOG_WARN, TEXT("IDirect3DResource8::GetPrivateData result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
        }
    }

    return hrC;
}

//******************************************************************************
HRESULT CCResource8_SetPrivateData(CResource8* pResource, CClient* pClient, REFGUID refguid, void* pvData, DWORD dwSizeOfData, DWORD dwFlags) {

    LPDIRECT3DRESOURCE8 pd3dres = pResource->GetIDirect3DResource8();
    HRESULT             hrC, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!pd3dres) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    hrC = pd3dres->SetPrivateData(refguid, pvData, dwSizeOfData, dwFlags);

    if (pClient->CallMethod(MID_RES8_SetPrivateData, pResource, &hrS, &refguid, sizeof(GUID), pvData, dwSizeOfData, dwSizeOfData, dwFlags)) {
        if (hrC != hrS) {
            Log(LOG_WARN, TEXT("IDirect3DResource8::SetPrivateData result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
        }
    }

    return hrC;
}

//******************************************************************************
HRESULT CCResource8_FreePrivateData(CResource8* pResource, CClient* pClient, REFGUID refguid) {

    LPDIRECT3DRESOURCE8 pd3dres = pResource->GetIDirect3DResource8();
    HRESULT             hrC, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!pd3dres) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    hrC = pd3dres->FreePrivateData(refguid);

    if (pClient->CallMethod(MID_RES8_FreePrivateData, pResource, &hrS, &refguid, sizeof(GUID))) {
        if (hrC != hrS) {
            Log(LOG_WARN, TEXT("IDirect3DResource8::FreePrivateData result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
        }
    }

    return hrC;
}

//******************************************************************************
DWORD CCResource8_GetPriority(CResource8* pResource, CClient* pClient) {

    LPDIRECT3DRESOURCE8 pd3dres = pResource->GetIDirect3DResource8();
    DWORD               dwPriorityC, dwPriorityS;

#ifdef D3DLOCUS_DEBUG
    if (!pd3dres) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

#ifndef UNDER_XBOX
    dwPriorityC = pd3dres->GetPriority();
    if (pClient->CallMethod(MID_RES8_GetPriority, pResource, &dwPriorityS)) {
        if (dwPriorityC != dwPriorityS) {
            Log(LOG_WARN, TEXT("IDirect3DResource8::GetPriority result inconsistent [C: 0x%X, S: 0x%X]"), dwPriorityC, dwPriorityS);
        }
    }
    return dwPriorityC;
#else
    Log(LOG_FAIL, TEXT("IDirect3DResource8::GetPriority is not implemented on XBox"));
    return 0;
#endif // UNDER_XBOX
}

//******************************************************************************
DWORD CCResource8_SetPriority(CResource8* pResource, CClient* pClient, DWORD dwNewPriority) {

    LPDIRECT3DRESOURCE8 pd3dres = pResource->GetIDirect3DResource8();
    DWORD               dwPriorityC, dwPriorityS;

#ifdef D3DLOCUS_DEBUG
    if (!pd3dres) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

#ifndef UNDER_XBOX
    dwPriorityC = pd3dres->SetPriority(dwNewPriority);
    if (pClient->CallMethod(MID_RES8_SetPriority, pResource, &dwPriorityS, dwNewPriority)) {
        if (dwPriorityC != dwPriorityS) {
            Log(LOG_WARN, TEXT("IDirect3DResource8::SetPriority result inconsistent [C: 0x%X, S: 0x%X]"), dwPriorityC, dwPriorityS);
        }
    }
    return dwPriorityC;
#else
    Log(LOG_FAIL, TEXT("IDirect3DResource8::SetPriority is not implemented on XBox"));
    return 0;
#endif // UNDER_XBOX
}

//******************************************************************************
void CCResource8_PreLoad(CResource8* pResource, CClient* pClient) {

    LPDIRECT3DRESOURCE8 pd3dres = pResource->GetIDirect3DResource8();

#ifdef D3DLOCUS_DEBUG
    if (!pd3dres) {
        return;
    }
#endif // D3DLOCUS_DEBUG

#ifndef UNDER_XBOX
    pd3dres->PreLoad();
    pClient->CallMethod(MID_RES8_PreLoad, pResource);
#else
    Log(LOG_FAIL, TEXT("IDirect3DResource8::PreLoad is not implemented on XBox"));
    return;
#endif // UNDER_XBOX
}

#ifdef UNDER_XBOX

//******************************************************************************
BOOL CResource8::IsBusy() {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dres) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    return m_pd3dres->IsBusy();
}

//******************************************************************************
void CResource8::BlockUntilNotBusy() {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dres) {
        return;
    }
#endif // D3DLOCUS_DEBUG

    m_pd3dres->BlockUntilNotBusy();
}

//******************************************************************************
void CResource8::MoveResourceMemory(D3DMEMORY d3dmem) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dres) {
        return;
    }
#endif // D3DLOCUS_DEBUG

    m_pd3dres->MoveResourceMemory(d3dmem);
}

//******************************************************************************
void CResource8::Register(void *pvBase) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dres) {
        return;
    }
#endif // D3DLOCUS_DEBUG

    m_pd3dres->Register(pvBase);
}

#endif // UNDER_XBOX
