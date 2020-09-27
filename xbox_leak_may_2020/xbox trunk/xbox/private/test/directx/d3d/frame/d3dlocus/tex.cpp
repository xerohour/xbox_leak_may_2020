/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    tex.cpp

Author:

    Matt Bronder

Description:

    CTexture methods.

*******************************************************************************/

#include "d3dlocus.h"

//******************************************************************************
// CTexture
//******************************************************************************

//******************************************************************************
CTexture::CTexture() {
}

//******************************************************************************
CTexture::~CTexture() {
}

//******************************************************************************
// CTexture8
//******************************************************************************

//******************************************************************************
CTexture8::CTexture8() {

    m_pd3dt = NULL;
}

//******************************************************************************
CTexture8::~CTexture8() {
}

//******************************************************************************
BOOL CTexture8::Create(CDevice8* pDevice, LPDIRECT3DTEXTURE8 pd3dt) {

    if (m_pd3dt) {
        return FALSE;
    }

    if (CBaseTexture8::Create(pDevice, (LPDIRECT3DBASETEXTURE8)pd3dt)) {
        m_pd3dt = pd3dt;
    }

    return (BOOL)m_pd3dt;
}

//******************************************************************************
LPDIRECT3DTEXTURE8 CTexture8::GetIDirect3DTexture8() {

    return m_pd3dt;
}

//******************************************************************************
HRESULT CTexture8::GetLevelDesc(UINT uLevel, D3DSURFACE_DESC* pd3dsd) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dt) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    return m_pd3dt->GetLevelDesc(uLevel, pd3dsd);
}

//******************************************************************************
HRESULT CTexture8::LockRect(UINT uLevel, D3DLOCKED_RECT* pd3dlr, RECT* prect, DWORD dwFlags) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dt) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    return m_pd3dt->LockRect(uLevel, pd3dlr, prect, dwFlags);
}

//******************************************************************************
HRESULT CTexture8::UnlockRect(UINT uLevel) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dt) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    return m_pd3dt->UnlockRect(uLevel);
}

//******************************************************************************
HRESULT CTexture8::AddDirtyRect(RECT* prectDirty) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dt) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

#ifndef UNDER_XBOX
    return m_pd3dt->AddDirtyRect(prectDirty);
#else
    Log(LOG_FAIL, TEXT("IDirect3DTexture8::AddDirtyRect is not implemented on XBox"));
    return E_NOTIMPL;
#endif // UNDER_XBOX
}

//******************************************************************************
HRESULT CTexture8::GetSurfaceLevel(UINT uLevel, CSurface8** ppSurfaceLevel) {

    LPDIRECT3DSURFACE8  pd3ds;
    CSurface8*          pSurface;
    HRESULT             hr;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dt) {
        return ERR_NOTCREATED;
    }

    if (!ppSurfaceLevel) {
        return D3DERR_INVALIDCALL;
    }
#endif // D3DLOCUS_DEBUG

    *ppSurfaceLevel = NULL;

    hr = m_pd3dt->GetSurfaceLevel(uLevel, &pd3ds);
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

        *ppSurfaceLevel = pSurface;
    }

    return hr;
}

//******************************************************************************
// CCTexture8
//******************************************************************************

//******************************************************************************
CCTexture8::CCTexture8() {

    m_pslockd = NULL;
}

//******************************************************************************
CCTexture8::~CCTexture8() {

    if (m_pslockd) {
        MemFree(m_pslockd);
    }
}

//******************************************************************************
BOOL CCTexture8::Create(CClient* pClient, CDevice8* pDevice, LPDIRECT3DTEXTURE8 pd3dt) {

    if (!pClient) {
        return FALSE;
    }
    m_pClient = pClient;
    if (!CTexture8::Create(pDevice, pd3dt)) {
        return FALSE;
    }

    m_pslockd = (PSLOCKDESC)MemAlloc(pd3dt->GetLevelCount() * sizeof(SLOCKDESC));
    if (!m_pslockd) {
        return FALSE;
    }

    return TRUE;
}

//******************************************************************************
ULONG CCTexture8::AddRef() {

    return CCResource8_AddRef(this, m_pClient);
}

//******************************************************************************
ULONG CCTexture8::Release() {

    return CCResource8_Release(this, m_pClient);
}

//******************************************************************************
D3DRESOURCETYPE CCTexture8::GetType() {

    return CCResource8_GetType(this, m_pClient);
}

//******************************************************************************
HRESULT CCTexture8::GetDevice(CDevice8** ppDevice) {

    return CCResource8_GetDevice(this, m_pClient, ppDevice);
}

//******************************************************************************
HRESULT CCTexture8::GetPrivateData(REFGUID refguid, void* pvData, DWORD* pdwSizeOfData) {

    return CCResource8_GetPrivateData(this, m_pClient, refguid, pvData, pdwSizeOfData);
}

//******************************************************************************
HRESULT CCTexture8::SetPrivateData(REFGUID refguid, void* pvData, DWORD dwSizeOfData, DWORD dwFlags) {

    return CCResource8_SetPrivateData(this, m_pClient, refguid, pvData, dwSizeOfData, dwFlags);
}

//******************************************************************************
HRESULT CCTexture8::FreePrivateData(REFGUID refguid) {

    return CCResource8_FreePrivateData(this, m_pClient, refguid);
}

//******************************************************************************
DWORD CCTexture8::GetPriority() {

    return CCResource8_GetPriority(this, m_pClient);
}

//******************************************************************************
DWORD CCTexture8::SetPriority(DWORD dwNewPriority) {

    return CCResource8_SetPriority(this, m_pClient, dwNewPriority);
}

//******************************************************************************
void CCTexture8::PreLoad() {

    CCResource8_PreLoad(this, m_pClient);
}

//******************************************************************************
DWORD CCTexture8::GetLevelCount() {

    return CCBaseTexture8_GetLevelCount(this, m_pClient);
}

//******************************************************************************
DWORD CCTexture8::GetLOD() {

    return CCBaseTexture8_GetLOD(this, m_pClient);
}

//******************************************************************************
DWORD CCTexture8::SetLOD(DWORD dwNewLOD) {

    return CCBaseTexture8_SetLOD(this, m_pClient, dwNewLOD);
}

//******************************************************************************
HRESULT CCTexture8::GetLevelDesc(UINT uLevel, D3DSURFACE_DESC* pd3dsd) {

#ifndef UNDER_XBOX
    D3DSURFACE_DESC         d3dsd;
#else
    WINX_D3DSURFACE_DESC    d3dsd;
#endif // UNDER_XBOX
    HRESULT                 hrC, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dt) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    hrC = m_pd3dt->GetLevelDesc(uLevel, pd3dsd);

#ifndef UNDER_XBOX
    if (m_pClient->CallMethod(MID_TEX8_GetLevelDesc, this, &hrS, uLevel, &d3dsd, sizeof(D3DSURFACE_DESC))) {
#else
    if (m_pClient->CallMethod(MID_TEX8_GetLevelDesc, this, &hrS, uLevel, &d3dsd, sizeof(WINX_D3DSURFACE_DESC))) {
#endif // UNDER_XBOX
        if (hrC != hrS) {
            Log(LOG_WARN, TEXT("IDirect3DTexture8::GetLevelDesc result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
        }
        else {
#ifndef UNDER_XBOX
            if (memcmp(pd3dsd, &d3dsd, sizeof(D3DSURFACE_DESC))) {
#else
            if (pd3dsd->Type != d3dsd.Type ||
                pd3dsd->Width != d3dsd.Width ||
                pd3dsd->Height != d3dsd.Height ||
                pd3dsd->MultiSampleType != d3dsd.MultiSampleType)
            {
#endif // UNDER_XBOX
                Log(LOG_WARN, TEXT("IDirect3DTexture8::GetLevelDesc returned an inconsistent description"));
            }
        }
    }

    return hrC;
}

//******************************************************************************
HRESULT CCTexture8::LockRect(UINT uLevel, D3DLOCKED_RECT* pd3dlr, RECT* prect, DWORD dwFlags) {

    RECT    rectNULL = {-1, -1, -1, -1};
    HRESULT hrC, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dt) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    hrC = m_pd3dt->LockRect(uLevel, pd3dlr, prect, dwFlags);

    if (pd3dlr) {
        memcpy(&m_pslockd[uLevel].d3dlr, pd3dlr, sizeof(D3DLOCKED_RECT));
    }
    else {
        m_pslockd[uLevel].d3dlr.pBits = 0;
    }
    if (!prect) {
        prect = &rectNULL;
    }
    memcpy(&m_pslockd[uLevel].rect, prect, sizeof(RECT));
    m_pslockd[uLevel].dwFlags = dwFlags;

    if (m_pClient->CallMethod(MID_TEX8_LockRect, this, &hrS, uLevel, &m_pslockd[uLevel].d3dlrSrv, sizeof(D3DLOCKED_RECT), &m_pslockd[uLevel].rect, sizeof(RECT), dwFlags)) {
        if (hrC != hrS) {
            Log(LOG_WARN, TEXT("IDirect3DTexture8::LockRect result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
        }
    }

    return hrC;
}

//******************************************************************************
HRESULT CCTexture8::UnlockRect(UINT uLevel) {

    LPBYTE  pBits;
    DWORD   dwSize;
    HRESULT hrC, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dt) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    if (m_pslockd[uLevel].dwFlags & D3DLOCK_READONLY) {
        pBits = NULL;
        dwSize = 0;
    }
    else {
        D3DSURFACE_DESC d3dsd;
        DWORD           dwWidth, dwHeight, dwBitDepth;
        UINT            i;

        m_pd3dt->GetLevelDesc(uLevel, &d3dsd);
        if (m_pslockd[uLevel].rect.left == -1) {
            dwWidth = d3dsd.Width;
            dwHeight = d3dsd.Height;
        }
        else {
            dwWidth = m_pslockd[uLevel].rect.right - m_pslockd[uLevel].rect.left;
            dwHeight = m_pslockd[uLevel].rect.bottom - m_pslockd[uLevel].rect.top;
        }
        dwBitDepth = FormatToBitDepth(d3dsd.Format);
        dwSize = dwHeight * dwWidth * dwBitDepth / 8;
        pBits = (LPBYTE)MemAlloc(dwSize);

#ifdef UNDER_XBOX
        if (!XGIsSwizzledFormat(d3dsd.Format)) {
#endif
            if (IsCompressedFormat(d3dsd.Format)) {
                memcpy(pBits, m_pslockd[uLevel].d3dlr.pBits, dwWidth * dwHeight * dwBitDepth / 8);
            }
            else {
                for (i = 0; i < dwHeight; i++) {
                    memcpy(pBits + i * dwWidth * dwBitDepth / 8, (LPBYTE)m_pslockd[uLevel].d3dlr.pBits + i * m_pslockd[uLevel].d3dlr.Pitch, dwWidth * dwBitDepth / 8);
                }
            }
#ifdef UNDER_XBOX
        }
        else {
            // Since locking a subrect on a swizzled surface is unsupported on Xbox, simply unswizzle the entire surface
            XGUnswizzleRect(m_pslockd[uLevel].d3dlr.pBits, dwWidth, dwHeight, NULL, pBits, 
                            dwWidth * dwBitDepth / 8, NULL, dwBitDepth / 8);
        }
#endif
    }

    hrC = m_pd3dt->UnlockRect(uLevel);

    if (m_pClient->CallMethod(MID_TEX8_UnlockRect, this, &hrS, uLevel, &m_pslockd[uLevel].d3dlrSrv, sizeof(D3DLOCKED_RECT), &m_pslockd[uLevel].rect, sizeof(RECT), m_pslockd[uLevel].dwFlags, pBits, dwSize)) {
        if (hrC != hrS) {
            Log(LOG_WARN, TEXT("IDirect3DTexture8::UnlockRect result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
        }
    }

    if (pBits) {
        MemFree(pBits);
    }

    return hrC;
}

//******************************************************************************
HRESULT CCTexture8::AddDirtyRect(RECT* prectDirty) {

    RECT    rectNULL = {-1, -1, -1, -1};
    HRESULT hrC, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dt) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

#ifndef UNDER_XBOX
    hrC = m_pd3dt->AddDirtyRect(prectDirty);

    if (!prectDirty) {
        prectDirty = &rectNULL;
    }

    if (m_pClient->CallMethod(MID_TEX8_AddDirtyRect, this, &hrS, prectDirty, sizeof(RECT))) {
        if (hrC != hrS) {
            Log(LOG_WARN, TEXT("IDirect3DTexture8::AddDirtyRect result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
        }
    }

    return hrC;
#else
    Log(LOG_FAIL, TEXT("IDirect3DTexture8::AddDirtyRect is not implemented on XBox"));
    return E_NOTIMPL;
#endif // UNDER_XBOX
}

//******************************************************************************
HRESULT CCTexture8::GetSurfaceLevel(UINT uLevel, CSurface8** ppSurfaceLevel) {

    LPDIRECT3DSURFACE8  pd3ds;
    CCSurface8*         pSurface;
    HRESULT             hrC, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dt) {
        return ERR_NOTCREATED;
    }

    if (!ppSurfaceLevel) {
        return D3DERR_INVALIDCALL;
    }
#endif // D3DLOCUS_DEBUG

    *ppSurfaceLevel = NULL;

    hrC = m_pd3dt->GetSurfaceLevel(uLevel, &pd3ds);
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

        *ppSurfaceLevel = pSurface;
    }
    else {
        pSurface = NULL;
    }

    if (m_pClient->CallMethod(MID_TEX8_GetSurfaceLevel, this, &hrS, uLevel, &pSurface)) {
        if (hrC != hrS) {
            Log(LOG_WARN, TEXT("IDirect3DTexture8::GetSurfaceLevel result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
        }
    }

    return hrC;
}
