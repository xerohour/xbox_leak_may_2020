/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    direct3d.cpp

Author:

    Matt Bronder

Description:

    CDirect3D methods.

*******************************************************************************/

#include "d3dlocus.h"

//******************************************************************************
// CDirect3D
//******************************************************************************

//******************************************************************************
CDirect3D::CDirect3D() {
}

//******************************************************************************
CDirect3D::~CDirect3D() {
}

//******************************************************************************
// CDirect3D8
//******************************************************************************

//******************************************************************************
CDirect3D8::CDirect3D8() {

    m_pd3d = NULL;
}

//******************************************************************************
CDirect3D8::~CDirect3D8() {
}

//******************************************************************************
BOOL CDirect3D8::Create() {

    m_pd3d = Direct3DCreate8(D3D_SDK_VERSION);
    return (BOOL)m_pd3d;
}

//******************************************************************************
ULONG CDirect3D8::AddRef() {

    ULONG uRef = 0;

    if (m_pd3d) {
        uRef = m_pd3d->AddRef();
        CObject::AddRef();
    }

    return uRef;
}

//******************************************************************************
ULONG CDirect3D8::Release() {

    ULONG uRef = 0;

    if (m_pd3d) {
        uRef = m_pd3d->Release();
        CObject::Release();
    }

    return uRef;
}

//******************************************************************************
LPDIRECT3D8 CDirect3D8::GetIDirect3D8() {

    return m_pd3d;
}

//******************************************************************************
UINT CDirect3D8::GetAdapterCount() {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3d) {
        return 0;
    }
#endif // D3DLOCUS_DEBUG

    return m_pd3d->GetAdapterCount();
}

//******************************************************************************
HRESULT CDirect3D8::GetAdapterDisplayMode(UINT uAdapter, D3DDISPLAYMODE* pd3ddm) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3d) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    return m_pd3d->GetAdapterDisplayMode(uAdapter, pd3ddm);
}

//******************************************************************************
HRESULT CDirect3D8::GetAdapterIdentifier(UINT uAdapter, DWORD dwFlags, D3DADAPTER_IDENTIFIER8* pd3dai) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3d) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    return m_pd3d->GetAdapterIdentifier(uAdapter, dwFlags, pd3dai);
}

//******************************************************************************
UINT CDirect3D8::GetAdapterModeCount(UINT uAdapter) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3d) {
        return 0;
    }
#endif // D3DLOCUS_DEBUG

    return m_pd3d->GetAdapterModeCount(uAdapter);
}

//******************************************************************************
HMONITOR CDirect3D8::GetAdapterMonitor(UINT uAdapter) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3d) {
        return NULL;
    }
#endif // D3DLOCUS_DEBUG

#ifndef UNDER_XBOX
    return m_pd3d->GetAdapterMonitor(uAdapter);
#else
    Log(LOG_FAIL, TEXT("IDirect3D8::GetAdapterMonitor is not implemented on XBox"));
    return NULL;
#endif // UNDER_XBOX
}

//******************************************************************************
HRESULT CDirect3D8::GetDeviceCaps(UINT uAdapter, D3DDEVTYPE d3ddt, D3DCAPS8* pd3dcaps) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3d) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    return m_pd3d->GetDeviceCaps(uAdapter, d3ddt, pd3dcaps);
}

//******************************************************************************
HRESULT CDirect3D8::EnumAdapterModes(UINT uAdapter, UINT uMode, D3DDISPLAYMODE* pd3ddm) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3d) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    return m_pd3d->EnumAdapterModes(uAdapter, uMode, pd3ddm);
}

//******************************************************************************
HRESULT CDirect3D8::RegisterSoftwareDevice(void* pfnInitialize) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3d) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

#ifndef UNDER_XBOX
    return m_pd3d->RegisterSoftwareDevice(pfnInitialize);
#else
    Log(LOG_FAIL, TEXT("IDirect3D8::RegisterSoftwareDevice is not implemented on XBox"));
    return E_NOTIMPL;
#endif // UNDER_XBOX
}

//******************************************************************************
HRESULT CDirect3D8::CheckDeviceFormat(UINT uAdapter, D3DDEVTYPE d3ddt, D3DFORMAT fmtAdapter, DWORD dwUsage, D3DRESOURCETYPE d3drt, D3DFORMAT fmtCheck) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3d) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    return m_pd3d->CheckDeviceFormat(uAdapter, d3ddt, fmtAdapter, dwUsage, d3drt, fmtCheck);
}

//******************************************************************************
HRESULT CDirect3D8::CheckDeviceMultiSampleType(UINT uAdapter, D3DDEVTYPE d3ddt, D3DFORMAT fmtRenderTarget, BOOL bWindowed, D3DMULTISAMPLE_TYPE d3dmst) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3d) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    return m_pd3d->CheckDeviceMultiSampleType(uAdapter, d3ddt, fmtRenderTarget, bWindowed, d3dmst);
}

//******************************************************************************
HRESULT CDirect3D8::CheckDeviceType(UINT uAdapter, D3DDEVTYPE d3ddtCheck, D3DFORMAT fmtDisplayMode, D3DFORMAT fmtBackBuffer, BOOL bWindowed) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3d) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    return m_pd3d->CheckDeviceType(uAdapter, d3ddtCheck, fmtDisplayMode, fmtBackBuffer, bWindowed);
}

//******************************************************************************
HRESULT CDirect3D8::CheckDepthStencilMatch(UINT uAdapter, D3DDEVTYPE d3ddt, D3DFORMAT fmtAdapter, D3DFORMAT fmtrRenderTarget, D3DFORMAT fmtdDepthStencil) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3d) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    return m_pd3d->CheckDepthStencilMatch(uAdapter, d3ddt, fmtAdapter, fmtrRenderTarget, fmtdDepthStencil);
}

//******************************************************************************
HRESULT CDirect3D8::CreateDevice(UINT uAdapter, D3DDEVTYPE d3ddt, HWND hWndFocus, DWORD dwBehaviorFlags, D3DPRESENT_PARAMETERS* pd3dpp, CDevice8** ppDevice) {

    CDevice8*           pDevice;
    LPDIRECT3DDEVICE8   pd3dd;
    HRESULT             hr;

#ifdef D3DLOCUS_DEBUG

    if (!m_pd3d) {
        return ERR_NOTCREATED;
    }

    if (!ppDevice) {
        return D3DERR_INVALIDCALL;
    }

#endif // D3DLOCUS_DEBUG

    *ppDevice = NULL;

    hr = m_pd3d->CreateDevice(uAdapter, d3ddt, hWndFocus, dwBehaviorFlags, pd3dpp, &pd3dd);
    if (SUCCEEDED(hr)) {
        pDevice = new CDevice8();
        if (!pDevice) {
            pd3dd->Release();
            return E_OUTOFMEMORY;
        }

        if (!pDevice->Create(this, pd3dd)) {
            pd3dd->Release();
            return ERR_LOCUSFAIL;
        }

        *ppDevice = pDevice;
    }

    return hr;
}

#ifdef UNDER_XBOX
//******************************************************************************
HRESULT CDirect3D8::SetPushBufferSize(DWORD dwPushBufferSize, DWORD dwKickOffSize) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3d) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    return m_pd3d->SetPushBufferSize(dwPushBufferSize, dwKickOffSize);
}
#endif // UNDER_XBOX

//******************************************************************************
// CCDirect3D8
//******************************************************************************

//******************************************************************************
CCDirect3D8::CCDirect3D8() {

    m_pClient = NULL;
}

//******************************************************************************
CCDirect3D8::~CCDirect3D8() {
}

//******************************************************************************
BOOL CCDirect3D8::Create(CClient* pClient) {

    BOOL bRet;

    if (!pClient) {
        return FALSE;
    }
    m_pClient = pClient;

    bRet = CDirect3D8::Create();
    if (bRet) {
        m_pClient->CallDirect3DCreate8(this);
    }

    return bRet;
}

//******************************************************************************
ULONG CCDirect3D8::AddRef() {

    ULONG uRefC, uRefS;

    uRefC = CDirect3D8::AddRef();

    if (m_pClient->CallMethod(MID_D3D8_AddRef, this, &uRefS)) {
#ifndef UNDER_XBOX
        if (uRefC != uRefS) {
            Log(LOG_WARN, TEXT("IDirect3D8::AddRef result inconsistent [C: %d, S: %d]"), uRefC, uRefS);
        }
#endif // !UNDER_XBOX
    }

    return uRefC;
}

//******************************************************************************
ULONG CCDirect3D8::Release() {

    ULONG uRefC = 0, uRefS;

    if (m_pd3d) {

        uRefC = m_pd3d->Release();
        if (m_pClient->CallMethod(MID_D3D8_Release, this, &uRefS)) {
#ifndef UNDER_XBOX
            if (uRefC != uRefS) {
                Log(LOG_WARN, TEXT("IDirect3D8::Release result inconsistent [C: %d, S: %d]"), uRefC, uRefS);
            }
#endif // !UNDER_XBOX
        }
        CObject::Release();
    }

    return uRefC;
}

//******************************************************************************
UINT CCDirect3D8::GetAdapterCount() {

    UINT uCountC, uCountS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3d) {
        return 0;
    }
#endif // D3DLOCUS_DEBUG

    uCountC = m_pd3d->GetAdapterCount();

    m_pClient->CallMethod(MID_D3D8_GetAdapterCount, this, &uCountS);

    return uCountC;
}

//******************************************************************************
HRESULT CCDirect3D8::GetAdapterDisplayMode(UINT uAdapter, D3DDISPLAYMODE* pd3ddm) {

    D3DDISPLAYMODE  d3ddm;
    HRESULT         hrC, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3d) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    hrC = m_pd3d->GetAdapterDisplayMode(uAdapter, pd3ddm);

    m_pClient->CallMethod(MID_D3D8_GetAdapterDisplayMode, this, &hrS, uAdapter, &d3ddm, sizeof(D3DDISPLAYMODE));

    return hrC;
}

//******************************************************************************
HRESULT CCDirect3D8::GetAdapterIdentifier(UINT uAdapter, DWORD dwFlags, D3DADAPTER_IDENTIFIER8* pd3dai) {

    D3DADAPTER_IDENTIFIER8 d3dai;
    HRESULT                hrC, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3d) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    hrC = m_pd3d->GetAdapterIdentifier(uAdapter, dwFlags, pd3dai);

    m_pClient->CallMethod(MID_D3D8_GetAdapterIdentifier, this, &hrS, uAdapter, dwFlags, &d3dai, sizeof(D3DADAPTER_IDENTIFIER8));

    return hrC;
}

//******************************************************************************
UINT CCDirect3D8::GetAdapterModeCount(UINT uAdapter) {

    UINT uCountC, uCountS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3d) {
        return 0;
    }
#endif // D3DLOCUS_DEBUG

    uCountC = m_pd3d->GetAdapterModeCount(uAdapter);

    m_pClient->CallMethod(MID_D3D8_GetAdapterModeCount, this, &uCountS, uAdapter);

    return uCountC;
}

//******************************************************************************
HMONITOR CCDirect3D8::GetAdapterMonitor(UINT uAdapter) {

#ifndef UNDER_XBOX
    HMONITOR hmonC, hmonS;
#endif

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3d) {
        return NULL;
    }
#endif // D3DLOCUS_DEBUG

#ifndef UNDER_XBOX
    hmonC = m_pd3d->GetAdapterMonitor(uAdapter);
    m_pClient->CallMethod(MID_D3D8_GetAdapterMonitor, this, &hmonS, uAdapter);
    return hmonC;
#else
    Log(LOG_FAIL, TEXT("IDirect3D8::GetAdapterMonitor is not implemented on XBox"));
    return NULL;
#endif // UNDER_XBOX
}

//******************************************************************************
HRESULT CCDirect3D8::GetDeviceCaps(UINT uAdapter, D3DDEVTYPE d3ddt, D3DCAPS8* pd3dcaps) {

    D3DCAPS8    d3dcaps;
    HRESULT     hrC, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3d) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    hrC = m_pd3d->GetDeviceCaps(uAdapter, d3ddt, pd3dcaps);

    m_pClient->CallMethod(MID_D3D8_GetDeviceCaps, this, &hrS, uAdapter, d3ddt, &d3dcaps, sizeof(D3DCAPS8));

    return hrC;
}

//******************************************************************************
HRESULT CCDirect3D8::EnumAdapterModes(UINT uAdapter, UINT uMode, D3DDISPLAYMODE* pd3ddm) {

    D3DDISPLAYMODE d3ddm;
    HRESULT        hrC, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3d) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    hrC = m_pd3d->EnumAdapterModes(uAdapter, uMode, pd3ddm);

    m_pClient->CallMethod(MID_D3D8_EnumAdapterModes, this, &hrS, uAdapter, uMode, &d3ddm, sizeof(D3DDISPLAYMODE));

    return hrC;
}

//******************************************************************************
HRESULT CCDirect3D8::RegisterSoftwareDevice(void* pfnInitialize) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3d) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

#ifndef UNDER_XBOX
    return m_pd3d->RegisterSoftwareDevice(pfnInitialize);
#else
    Log(LOG_FAIL, TEXT("IDirect3D8::RegisterSoftwareDevice is not implemented on XBox"));
    return E_NOTIMPL;
#endif // UNDER_XBOX
}

//******************************************************************************
HRESULT CCDirect3D8::CheckDeviceFormat(UINT uAdapter, D3DDEVTYPE d3ddt, D3DFORMAT fmtAdapter, DWORD dwUsage, D3DRESOURCETYPE d3drt, D3DFORMAT fmtCheck) {

    HRESULT hrC, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3d) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    hrC = m_pd3d->CheckDeviceFormat(uAdapter, d3ddt, fmtAdapter, dwUsage, d3drt, fmtCheck);

    m_pClient->CallMethod(MID_D3D8_CheckDeviceFormat, this, &hrS, uAdapter, d3ddt, fmtAdapter, dwUsage, d3drt, fmtCheck);

    return hrC;
}

//******************************************************************************
HRESULT CCDirect3D8::CheckDeviceMultiSampleType(UINT uAdapter, D3DDEVTYPE d3ddt, D3DFORMAT fmtRenderTarget, BOOL bWindowed, D3DMULTISAMPLE_TYPE d3dmst) {

    HRESULT hrC, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3d) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    hrC = m_pd3d->CheckDeviceMultiSampleType(uAdapter, d3ddt, fmtRenderTarget, bWindowed, d3dmst);

    m_pClient->CallMethod(MID_D3D8_CheckDeviceMultiSampleType, this, &hrS, uAdapter, d3ddt, fmtRenderTarget, bWindowed, d3dmst);

    return hrC;
}

//******************************************************************************
HRESULT CCDirect3D8::CheckDeviceType(UINT uAdapter, D3DDEVTYPE d3ddtCheck, D3DFORMAT fmtDisplayMode, D3DFORMAT fmtBackBuffer, BOOL bWindowed) {

    HRESULT hrC, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3d) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    hrC = m_pd3d->CheckDeviceType(uAdapter, d3ddtCheck, fmtDisplayMode, fmtBackBuffer, bWindowed);

    m_pClient->CallMethod(MID_D3D8_CheckDeviceType, this, &hrS, uAdapter, d3ddtCheck, fmtDisplayMode, fmtBackBuffer, bWindowed);

    return hrC;
}

//******************************************************************************
HRESULT CCDirect3D8::CheckDepthStencilMatch(UINT uAdapter, D3DDEVTYPE d3ddt, D3DFORMAT fmtAdapter, D3DFORMAT fmtrRenderTarget, D3DFORMAT fmtdDepthStencil) {

    HRESULT hrC, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3d) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    hrC = m_pd3d->CheckDepthStencilMatch(uAdapter, d3ddt, fmtAdapter, fmtrRenderTarget, fmtdDepthStencil);

    m_pClient->CallMethod(MID_D3D8_CheckDepthStencilMatch, this, &hrS, uAdapter, d3ddt, fmtAdapter, fmtrRenderTarget, fmtdDepthStencil);

    return hrC;
}

//******************************************************************************
HRESULT CCDirect3D8::CreateDevice(UINT uAdapter, D3DDEVTYPE d3ddt, HWND hWndFocus, DWORD dwBehaviorFlags, D3DPRESENT_PARAMETERS* pd3dpp, CDevice8** ppDevice) {

    CCDevice8*          pDevice;
    LPDIRECT3DDEVICE8   pd3dd;
    HRESULT             hrC, hrS;

#ifdef D3DLOCUS_DEBUG

    if (!m_pd3d) {
        return ERR_NOTCREATED;
    }

    if (!ppDevice) {
        return D3DERR_INVALIDCALL;
    }

#endif // D3DLOCUS_DEBUG

    *ppDevice = NULL;

    hrC = m_pd3d->CreateDevice(uAdapter, d3ddt, hWndFocus, dwBehaviorFlags, pd3dpp, &pd3dd);
    if (SUCCEEDED(hrC)) {

        pDevice = new CCDevice8();
        if (!pDevice) {
            pd3dd->Release();
            return E_OUTOFMEMORY;
        }

        if (!pDevice->Create(m_pClient, this, pd3dd)) {
            pd3dd->Release();
            return ERR_LOCUSFAIL;
        }

        *ppDevice = pDevice;

        if (m_pClient->CallMethod(MID_D3D8_CreateDevice, this, &hrS, uAdapter, d3ddt, hWndFocus, dwBehaviorFlags, pd3dpp, sizeof(D3DPRESENT_PARAMETERS), &pDevice)) {
            if (hrC != hrS) {
                Log(LOG_WARN, TEXT("IDirect3D8::CreateDevice result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
                m_pClient->Disconnect();
            }
        }
    }

    return hrC;
}
