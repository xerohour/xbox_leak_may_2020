/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    surface.cpp

Author:

    Matt Bronder

Description:

    CSurface methods.

*******************************************************************************/

#include "d3dlocus.h"

//******************************************************************************
// CSurface
//******************************************************************************

//******************************************************************************
CSurface::CSurface() {
}

//******************************************************************************
CSurface::~CSurface() {
}

//******************************************************************************
// CSurface8
//******************************************************************************

//******************************************************************************
CSurface8::CSurface8() {

    m_pd3ds = NULL;
}

//******************************************************************************
CSurface8::~CSurface8() {

    if (m_pDevice) {
        m_pDevice->RemoveObject(m_pd3ds);
    }
}

//******************************************************************************
BOOL CSurface8::Create(CDevice8* pDevice, LPDIRECT3DSURFACE8 pd3ds) {

    if (m_pd3ds) {
        return FALSE;
    }

    if (pDevice && pd3ds) {

        if (!pDevice->AddObject(pd3ds, this)) {
            return FALSE;
        }

        m_pDevice = pDevice;

        m_pd3ds = pd3ds;
    }

    return (BOOL)m_pd3ds;
}

//******************************************************************************
ULONG CSurface8::AddRef() {

    ULONG uRef = 0;

    if (m_pd3ds) {
        uRef = m_pd3ds->AddRef();
        CObject::AddRef();
    }

    return uRef;
}

//******************************************************************************
ULONG CSurface8::Release() {

    ULONG uRef = 0;

    if (m_pd3ds) {
        uRef = m_pd3ds->Release();
        CObject::Release();
    }

    return uRef;
}

//******************************************************************************
LPDIRECT3DSURFACE8 CSurface8::GetIDirect3DSurface8() {

    return m_pd3ds;
}

//******************************************************************************
HRESULT CSurface8::GetContainer(REFIID riid, CBaseTexture8** ppContainer) {

    LPDIRECT3DBASETEXTURE8 pd3dtb;
    CBaseTexture8*         pBaseTexture;
    HRESULT                hr = ERR_NOTCREATED;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3ds) {
        return ERR_NOTCREATED;
    }
#ifndef UNDER_XBOX
    if (!(riid == IID_IDirect3DTexture8 || riid == IID_IDirect3DCubeTexture8)) {
        return D3DERR_INVALIDCALL;
    }
#endif // !UNDER_XBOX
    if (!ppContainer) {
        return D3DERR_INVALIDCALL;
    }
#endif // D3DLOCUS_DEBUG

    *ppContainer = NULL;

#ifndef UNDER_XBOX
    hr = m_pd3ds->GetContainer(riid, (LPVOID*)&pd3dtb);
#else
    hr = m_pd3ds->GetContainer(&pd3dtb);
#endif // UNDER_XBOX
    if (SUCCEEDED(hr)) {
        if (!m_pDevice->FindObject(pd3dtb, (LPVOID*)&pBaseTexture)) {
            pd3dtb->Release();
            return ERR_NOTCREATED;
        }
        else {
            pBaseTexture->CObject::AddRef();
            *ppContainer = pBaseTexture;
        }
    }

    return hr;
}

//******************************************************************************
HRESULT CSurface8::GetDevice(CDevice8** ppDevice) {

    LPDIRECT3DDEVICE8 pd3dd;
    HRESULT           hr = ERR_NOTCREATED;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3ds || !m_pDevice) {
        return ERR_NOTCREATED;
    }
    if (!ppDevice) {
        return D3DERR_INVALIDCALL;
    }
#endif // D3DLOCUS_DEBUG

    *ppDevice = NULL;

    hr = m_pd3ds->GetDevice(&pd3dd);
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
HRESULT CSurface8::GetDesc(D3DSURFACE_DESC* pd3dsd) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3ds) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    return m_pd3ds->GetDesc(pd3dsd);
}

//******************************************************************************
HRESULT CSurface8::LockRect(D3DLOCKED_RECT* pd3dlr, RECT* prect, DWORD dwFlags) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3ds) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    return m_pd3ds->LockRect(pd3dlr, prect, dwFlags);
}

//******************************************************************************
HRESULT CSurface8::UnlockRect() {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3ds) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    return m_pd3ds->UnlockRect();
}

//******************************************************************************
HRESULT CSurface8::GetPrivateData(REFGUID refguid, void* pvData, DWORD* pdwSizeOfData) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3ds) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    return m_pd3ds->GetPrivateData(refguid, pvData, pdwSizeOfData);
}

//******************************************************************************
HRESULT CSurface8::SetPrivateData(REFGUID refguid, void* pvData, DWORD dwSizeOfData, DWORD dwFlags) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3ds) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    return m_pd3ds->SetPrivateData(refguid, pvData, dwSizeOfData, dwFlags);
}

//******************************************************************************
HRESULT CSurface8::FreePrivateData(REFGUID refguid) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3ds) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    return m_pd3ds->FreePrivateData(refguid);
}

//******************************************************************************
// CCSurface8
//******************************************************************************

//******************************************************************************
CCSurface8::CCSurface8() {
}

//******************************************************************************
CCSurface8::~CCSurface8() {
}

//******************************************************************************
BOOL CCSurface8::Create(CClient* pClient, CDevice8* pDevice, LPDIRECT3DSURFACE8 pd3ds) {

    if (!pClient) {
        return FALSE;
    }
    m_pClient = pClient;
    return CSurface8::Create(pDevice, pd3ds);
}

//******************************************************************************
ULONG CCSurface8::AddRef() {

    ULONG uRefC = 0, uRefS;

    if (m_pd3ds) {
        uRefC = m_pd3ds->AddRef();
        if (m_pClient->CallMethod(MID_SUR8_AddRef, this, &uRefS)) {
#ifndef UNDER_XBOX
            if (uRefC != uRefS) {
                Log(LOG_WARN, TEXT("IDirect3DSurface8::AddRef result inconsistent [C: %d, S: %d]"), uRefC, uRefS);
            }
#endif // !UNDER_XBOX
        }
        CObject::AddRef();
    }

    return uRefC;
}

//******************************************************************************
ULONG CCSurface8::Release() {

    ULONG uRefC = 0, uRefS;

    if (m_pd3ds) {
        uRefC = m_pd3ds->Release();
        if (m_pClient->CallMethod(MID_SUR8_Release, this, &uRefS)) {
#ifndef UNDER_XBOX
            if (uRefC != uRefS) {
                Log(LOG_WARN, TEXT("IDirect3DSurface8::Release result inconsistent [C: %d, S: %d]"), uRefC, uRefS);
            }
#endif // !UNDER_XBOX
        }
        CObject::Release();
    }

    return uRefC;
}

//******************************************************************************
HRESULT CCSurface8::GetContainer(REFIID riid, CBaseTexture8** ppContainer) {

    LPDIRECT3DBASETEXTURE8 pd3dtb;
    CBaseTexture8*         pBaseTexture;
    HRESULT                hrC = ERR_NOTCREATED, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3ds) {
        return ERR_NOTCREATED;
    }
#ifndef UNDER_XBOX
    if (!(riid == IID_IDirect3DTexture8 || riid == IID_IDirect3DCubeTexture8)) {
        return D3DERR_INVALIDCALL;
    }
#endif // !UNDER_XBOX
    if (!ppContainer) {
        return D3DERR_INVALIDCALL;
    }
#endif // D3DLOCUS_DEBUG

    *ppContainer = NULL;

#ifndef UNDER_XBOX
    hrC = m_pd3ds->GetContainer(riid, (LPVOID*)&pd3dtb);
#else
    hrC = m_pd3ds->GetContainer(&pd3dtb);
#endif // UNDER_XBOX
    if (SUCCEEDED(hrC)) {
        if (!m_pDevice->FindObject(pd3dtb, (LPVOID*)&pBaseTexture)) {
            pd3dtb->Release();
            return ERR_NOTCREATED;
        }
        else {
            pBaseTexture->CObject::AddRef();
            *ppContainer = pBaseTexture;
            if (m_pClient->CallMethod(MID_SUR8_GetContainer, this, &hrS, &riid, sizeof(GUID), &pBaseTexture)) {
                if (hrC != hrS) {
                    Log(LOG_WARN, TEXT("IDirect3DSurface8::GetContainer result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
                }
            }
        }
    }

    return hrC;
}

//******************************************************************************
HRESULT CCSurface8::GetDevice(CDevice8** ppDevice) {

    LPDIRECT3DDEVICE8 pd3dd;
    CDevice8*         pDevice;
    HRESULT           hrC = ERR_NOTCREATED, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3ds || !m_pDevice) {
        return ERR_NOTCREATED;
    }
    if (!ppDevice) {
        return D3DERR_INVALIDCALL;
    }
#endif // D3DLOCUS_DEBUG

    *ppDevice = NULL;

    hrC = m_pd3ds->GetDevice(&pd3dd);
    if (SUCCEEDED(hrC)) {
        if (pd3dd != m_pDevice->GetIDirect3DDevice8()) {
            DebugString(TEXT("Interface pointers differ for IDirect3DDevice8"));
            DebugBreak();
        }
        m_pDevice->CObject::AddRef();
        *ppDevice = m_pDevice;
        pDevice = m_pDevice;
    }
    else {
        pDevice = NULL;
    }

    if (m_pClient->CallMethod(MID_SUR8_GetDevice, this, &hrS, &pDevice)) {
        if (hrC != hrS) {
            Log(LOG_WARN, TEXT("IDirect3DSurface8::GetDevice result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
        }
    }

    return hrC;
}

//******************************************************************************
HRESULT CCSurface8::GetDesc(D3DSURFACE_DESC* pd3dsd) {

#ifndef UNDER_XBOX
    D3DSURFACE_DESC         d3dsd;
#else
    WINX_D3DSURFACE_DESC    d3dsd;
#endif // UNDER_XBOX
    HRESULT                 hrC, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3ds) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    hrC = m_pd3ds->GetDesc(pd3dsd);

#ifndef UNDER_XBOX
    if (m_pClient->CallMethod(MID_SUR8_GetDesc, this, &hrS, &d3dsd, sizeof(D3DSURFACE_DESC))) {
#else
    if (m_pClient->CallMethod(MID_SUR8_GetDesc, this, &hrS, &d3dsd, sizeof(WINX_D3DSURFACE_DESC))) {
#endif // UNDER_XBOX
        if (hrC != hrS) {
            Log(LOG_WARN, TEXT("IDirect3DSurface8::GetDesc result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
        }
        else if (SUCCEEDED(hrC)) {
#ifndef UNDER_XBOX
            if (memcmp(pd3dsd, &d3dsd, sizeof(D3DSURFACE_DESC))) {
#else
            if (pd3dsd->Type != d3dsd.Type ||
                pd3dsd->Width != d3dsd.Width ||
                pd3dsd->Height != d3dsd.Height ||
                pd3dsd->MultiSampleType != d3dsd.MultiSampleType)
            {
#endif // UNDER_XBOX
                Log(LOG_WARN, TEXT("IDirect3DSurface8::GetDesc returned an inconsistent description"));
            }
        }
    }

    return hrC;
}

//******************************************************************************
HRESULT CCSurface8::LockRect(D3DLOCKED_RECT* pd3dlr, RECT* prect, DWORD dwFlags) {

    RECT    rectNULL = {-1, -1, -1, -1};
    HRESULT hrC, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3ds) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    hrC = m_pd3ds->LockRect(pd3dlr, prect, dwFlags);

    if (pd3dlr) {
        memcpy(&m_slockd.d3dlr, pd3dlr, sizeof(D3DLOCKED_RECT));
    }
    else {
        m_slockd.d3dlr.pBits = 0;
    }
    if (!prect) {
        prect = &rectNULL;
    }
    memcpy(&m_slockd.rect, prect, sizeof(RECT));
    m_slockd.dwFlags = dwFlags;

    if (m_pClient->CallMethod(MID_SUR8_LockRect, this, &hrS, &m_slockd.d3dlrSrv, sizeof(D3DLOCKED_RECT), &m_slockd.rect, sizeof(RECT), dwFlags)) {
        if (hrC != hrS) {
            Log(LOG_WARN, TEXT("IDirect3DSurface8::LockRect result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
        }
    }

    return hrC;
}

//******************************************************************************
HRESULT CCSurface8::UnlockRect() {

    LPBYTE  pBits;
    DWORD   dwSize;
    HRESULT hrC, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3ds) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    if (m_slockd.dwFlags & D3DLOCK_READONLY) {
        pBits = NULL;
        dwSize = 0;
    }
    else {
        D3DSURFACE_DESC d3dsd;
        DWORD           dwWidth, dwHeight, dwBitDepth;
        UINT            i;

        m_pd3ds->GetDesc(&d3dsd);
        if (m_slockd.rect.left == -1) {
            dwWidth = d3dsd.Width;
            dwHeight = d3dsd.Height;
        }
        else {
            dwWidth = m_slockd.rect.right - m_slockd.rect.left;
            dwHeight = m_slockd.rect.bottom - m_slockd.rect.top;
        }
        dwBitDepth = FormatToBitDepth(d3dsd.Format);
        dwSize = dwHeight * dwWidth * dwBitDepth / 8;
        pBits = (LPBYTE)MemAlloc(dwSize);

#ifdef UNDER_XBOX
        if (!XGIsSwizzledFormat(d3dsd.Format)) {
#endif
            if (IsCompressedFormat(d3dsd.Format)) {
                memcpy(pBits, m_slockd.d3dlr.pBits, dwWidth * dwHeight * dwBitDepth / 8);
            }
            else {
                for (i = 0; i < dwHeight; i++) {
                    memcpy(pBits + i * dwWidth * dwBitDepth / 8, (LPBYTE)m_slockd.d3dlr.pBits + i * m_slockd.d3dlr.Pitch, dwWidth * dwBitDepth / 8);
                }
            }
#ifdef UNDER_XBOX
        }
        else {
            // Since locking a subrect on a swizzled surface is unsupported on Xbox, simply unswizzle the entire surface
            XGUnswizzleRect(m_slockd.d3dlr.pBits, dwWidth, dwHeight, NULL, pBits, 
                            dwWidth * dwBitDepth / 8, NULL, dwBitDepth / 8);
        }
#endif
    }

    hrC = m_pd3ds->UnlockRect();

    if (m_pClient->CallMethod(MID_SUR8_UnlockRect, this, &hrS, &m_slockd.d3dlrSrv, sizeof(D3DLOCKED_RECT), &m_slockd.rect, sizeof(RECT), m_slockd.dwFlags, pBits, dwSize)) {
        if (hrC != hrS) {
            Log(LOG_WARN, TEXT("IDirect3DSurface8::UnlockRect result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
        }
    }

    if (pBits) {
        MemFree(pBits);
    }

    return hrC;
}

//******************************************************************************
HRESULT CCSurface8::GetPrivateData(REFGUID refguid, void* pvData, DWORD* pdwSizeOfData) {

    HRESULT hrC, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3ds) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    hrC = m_pd3ds->GetPrivateData(refguid, pvData, pdwSizeOfData);

    if (m_pClient->CallMethod(MID_SUR8_GetPrivateData, this, &hrS, &refguid, sizeof(GUID), pvData, *pdwSizeOfData, pdwSizeOfData)) {
        if (hrC != hrS) {
            Log(LOG_WARN, TEXT("IDirect3DSurface8::GetPrivateData result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
        }
    }

    return hrC;
}

//******************************************************************************
HRESULT CCSurface8::SetPrivateData(REFGUID refguid, void* pvData, DWORD dwSizeOfData, DWORD dwFlags) {

    HRESULT hrC, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3ds) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    hrC = m_pd3ds->SetPrivateData(refguid, pvData, dwSizeOfData, dwFlags);

    if (m_pClient->CallMethod(MID_SUR8_SetPrivateData, this, &hrS, &refguid, sizeof(GUID), pvData, dwSizeOfData, dwSizeOfData, dwFlags)) {
        if (hrC != hrS) {
            Log(LOG_WARN, TEXT("IDirect3DSurface8::SetPrivateData result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
        }
    }

    return hrC;
}

//******************************************************************************
HRESULT CCSurface8::FreePrivateData(REFGUID refguid) {

    HRESULT hrC, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3ds) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    hrC = m_pd3ds->FreePrivateData(refguid);

    if (m_pClient->CallMethod(MID_SUR8_FreePrivateData, this, &hrS, &refguid, sizeof(GUID))) {
        if (hrC != hrS) {
            Log(LOG_WARN, TEXT("IDirect3DSurface8::FreePrivateData result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
        }
    }

    return hrC;
}
