/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    swapchn.cpp

Author:

    Matt Bronder

Description:

    CSwapChain methods.

*******************************************************************************/

#ifndef UNDER_XBOX

#include "d3dlocus.h"

//******************************************************************************
// CSwapChain
//******************************************************************************

//******************************************************************************
CSwapChain::CSwapChain() {
}

//******************************************************************************
CSwapChain::~CSwapChain() {
}

//******************************************************************************
// CSwapChain8
//******************************************************************************

//******************************************************************************
CSwapChain8::CSwapChain8() {

    m_pd3dsw = NULL;
}

//******************************************************************************
CSwapChain8::~CSwapChain8() {

    if (m_pDevice) {
        m_pDevice->RemoveObject(m_pd3dsw);
    }
}

//******************************************************************************
BOOL CSwapChain8::Create(CDevice8* pDevice, LPDIRECT3DSWAPCHAIN8 pd3dsw) {

    if (m_pd3dsw) {
        return FALSE;
    }

    if (pDevice && pd3dsw) {

        if (!pDevice->AddObject(pd3dsw, this)) {
            return FALSE;
        }

        m_pDevice = pDevice;

        m_pd3dsw = pd3dsw;
    }

    return (BOOL)m_pd3dsw;
}

//******************************************************************************
ULONG CSwapChain8::AddRef() {

    ULONG uRef = 0;

    if (m_pd3dsw) {
        uRef = m_pd3dsw->AddRef();
        CObject::AddRef();
    }

    return uRef;
}

//******************************************************************************
ULONG CSwapChain8::Release() {

    ULONG uRef = 0;

    if (m_pd3dsw) {
        uRef = m_pd3dsw->Release();
        CObject::Release();
    }

    return uRef;
}

//******************************************************************************
LPDIRECT3DSWAPCHAIN8 CSwapChain8::GetIDirect3DSwapChain8() {

    return m_pd3dsw;
}

//******************************************************************************
HRESULT CSwapChain8::Present(RECT* prectSrc, RECT* prectDst, HWND hDstWindowOverride, RGNDATA* prgnDirtyRegion) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dsw) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    return m_pd3dsw->Present(prectSrc, prectDst, hDstWindowOverride, prgnDirtyRegion);
}

//******************************************************************************
HRESULT CSwapChain8::GetBackBuffer(UINT uBackBuffer, D3DBACKBUFFER_TYPE bbt, CSurface8** ppBackBuffer) {

    LPDIRECT3DSURFACE8  pd3ds;
    CSurface8*          pSurface;
    HRESULT             hr;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dsw || !m_pDevice) {
        return ERR_NOTCREATED;
    }

    if (!ppBackBuffer) {
        return D3DERR_INVALIDCALL;
    }
#endif // D3DLOCUS_DEBUG

    *ppBackBuffer = NULL;

    hr = m_pd3dsw->GetBackBuffer(uBackBuffer, bbt, &pd3ds);
    if (SUCCEEDED(hr)) {
        
        if (m_pDevice->FindObject((LPVOID)pd3ds, (LPVOID*)&pSurface)) {
            pSurface->CObject::AddRef();
        }
        else {
            pSurface = new CSurface8();
            if (!pSurface) {
                pd3ds->Release();
                return E_OUTOFMEMORY;
            }

            if (!pSurface->Create(m_pDevice, pd3ds)) {
                pd3ds->Release();
                return ERR_LOCUSFAIL;
            }
        }

        *ppBackBuffer = pSurface;
    }

    return hr;
}

//******************************************************************************
// CCSwapChain8
//******************************************************************************

//******************************************************************************
CCSwapChain8::CCSwapChain8() {
}

//******************************************************************************
CCSwapChain8::~CCSwapChain8() {
}

//******************************************************************************
BOOL CCSwapChain8::Create(CClient* pClient, CDevice8* pDevice, LPDIRECT3DSWAPCHAIN8 pd3dsw) {

    if (!pClient) {
        return FALSE;
    }
    m_pClient = pClient;
    return CSwapChain8::Create(pDevice, pd3dsw);
}

//******************************************************************************
ULONG CCSwapChain8::AddRef() {

    ULONG uRefC = 0, uRefS;

    if (m_pd3dsw) {
        uRefC = m_pd3dsw->AddRef();
        if (m_pClient->CallMethod(MID_SWC8_AddRef, this, &uRefS)) {
            if (uRefC != uRefS) {
                Log(LOG_WARN, TEXT("IDirect3DSwapChain8::AddRef result inconsistent [C: %d, S: %d]"), uRefC, uRefS);
            }
        }
        CObject::AddRef();
    }

    return uRefC;
}

//******************************************************************************
ULONG CCSwapChain8::Release() {

    ULONG uRefC = 0, uRefS;

    if (m_pd3dsw) {
        uRefC = m_pd3dsw->Release();
        if (m_pClient->CallMethod(MID_SWC8_Release, this, &uRefS)) {
            if (uRefC != uRefS) {
                Log(LOG_WARN, TEXT("IDirect3DSwapChain8::Release result inconsistent [C: %d, S: %d]"), uRefC, uRefS);
            }
        }
        CObject::Release();
    }

    return uRefC;
}

//******************************************************************************
HRESULT CCSwapChain8::Present(RECT* prectSrc, RECT* prectDst, HWND hDstWindowOverride, RGNDATA* prgnDirtyRegion) {

    static RECT     rectNULL = {-1, -1, -1, -1};
    static RGNDATA  rgnNULL = {{sizeof(RGNDATAHEADER), RDH_RECTANGLES, 0, 0, {-1, -1, -1, -1}}, '\0'};
    HRESULT         hrC, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dsw) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    hrC = m_pd3dsw->Present(prectSrc, prectDst, hDstWindowOverride, prgnDirtyRegion);

    if (!prectSrc) {
        prectSrc = &rectNULL;
    }
    if (!prectDst) {
        prectDst = &rectNULL;
    }
    if (!prgnDirtyRegion) {
        prgnDirtyRegion = &rgnNULL;
    }

    if (m_pClient->CallMethod(MID_SWC8_Present, this, &hrS, prectSrc, sizeof(RECT), prectDst, sizeof(RECT), hDstWindowOverride, prgnDirtyRegion, sizeof(RGNDATA))) {
        if (hrC != hrS) {
            Log(LOG_WARN, TEXT("IDirect3DSwapChain8::Present result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
        }
    }

    return hrC;
}

//******************************************************************************
HRESULT CCSwapChain8::GetBackBuffer(UINT uBackBuffer, D3DBACKBUFFER_TYPE bbt, CSurface8** ppBackBuffer) {

    LPDIRECT3DSURFACE8  pd3ds;
    CCSurface8*         pSurface;
    HRESULT             hrC, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dsw || !m_pDevice) {
        return ERR_NOTCREATED;
    }

    if (!ppBackBuffer) {
        return D3DERR_INVALIDCALL;
    }
#endif // D3DLOCUS_DEBUG

    *ppBackBuffer = NULL;

    hrC = m_pd3dsw->GetBackBuffer(uBackBuffer, bbt, &pd3ds);
    if (SUCCEEDED(hrC)) {
        
        if (m_pDevice->FindObject((LPVOID)pd3ds, (LPVOID*)&pSurface)) {
            pSurface->CObject::AddRef();
        }
        else {
            pSurface = new CCSurface8();
            if (!pSurface) {
                pd3ds->Release();
                return E_OUTOFMEMORY;
            }

            if (!pSurface->Create(m_pClient, m_pDevice, pd3ds)) {
                pd3ds->Release();
                return ERR_LOCUSFAIL;
            }
        }

        *ppBackBuffer = pSurface;
    }
    else {
        pSurface = NULL;
    }

    if (m_pClient->CallMethod(MID_SWC8_GetBackBuffer, this, &hrS, uBackBuffer, bbt, &pSurface)) {
        if (hrC != hrS) {
            Log(LOG_WARN, TEXT("IDirect3DSwapChain8::GetBackBuffer result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
        }
    }

    return hrC;
}

#endif // !UNDER_XBOX
