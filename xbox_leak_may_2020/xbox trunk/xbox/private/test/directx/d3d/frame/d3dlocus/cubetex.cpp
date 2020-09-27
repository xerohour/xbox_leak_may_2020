/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    cubetex.cpp

Author:

    Matt Bronder

Description:

    CCubeTexture methods.

*******************************************************************************/

#include "d3dlocus.h"

//******************************************************************************
// CCubeTexture
//******************************************************************************

//******************************************************************************
CCubeTexture::CCubeTexture() {
}

//******************************************************************************
CCubeTexture::~CCubeTexture() {
}

//******************************************************************************
// CCubeTexture8
//******************************************************************************

//******************************************************************************
CCubeTexture8::CCubeTexture8() {

    m_pd3dtc = NULL;
}

//******************************************************************************
CCubeTexture8::~CCubeTexture8() {
}

//******************************************************************************
BOOL CCubeTexture8::Create(CDevice8* pDevice, LPDIRECT3DCUBETEXTURE8 pd3dtc) {

    if (m_pd3dtc) {
        return FALSE;
    }

    if (CBaseTexture8::Create(pDevice, (LPDIRECT3DBASETEXTURE8)pd3dtc)) {
        m_pd3dtc = pd3dtc;
    }

    return (BOOL)m_pd3dtc;
}

//******************************************************************************
LPDIRECT3DCUBETEXTURE8 CCubeTexture8::GetIDirect3DCubeTexture8() {

    return m_pd3dtc;
}

//******************************************************************************
HRESULT CCubeTexture8::GetLevelDesc(UINT uLevel, D3DSURFACE_DESC* pd3dsd) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dtc) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    return m_pd3dtc->GetLevelDesc(uLevel, pd3dsd);
}

//******************************************************************************
HRESULT CCubeTexture8::LockRect(D3DCUBEMAP_FACES d3dcf, UINT uLevel, D3DLOCKED_RECT* pd3dlr, RECT* prect, DWORD dwFlags) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dtc) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    return m_pd3dtc->LockRect(d3dcf, uLevel, pd3dlr, prect, dwFlags);
}

//******************************************************************************
HRESULT CCubeTexture8::UnlockRect(D3DCUBEMAP_FACES d3dcf, UINT uLevel) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dtc) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    return m_pd3dtc->UnlockRect(d3dcf, uLevel);
}

//******************************************************************************
HRESULT CCubeTexture8::AddDirtyRect(D3DCUBEMAP_FACES d3dcf, RECT* prectDirty) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dtc) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

#ifndef UNDER_XBOX
    return m_pd3dtc->AddDirtyRect(d3dcf, prectDirty);
#else
    Log(LOG_FAIL, TEXT("IDirect3DCubeTexture8::AddDirtyRect is not implemented on XBox"));
    return E_NOTIMPL;
#endif // UNDER_XBOX
}

//******************************************************************************
HRESULT CCubeTexture8::GetCubeMapSurface(D3DCUBEMAP_FACES d3dcf, UINT uLevel, CSurface8** ppCubeMapSurface) {

    LPDIRECT3DSURFACE8  pd3ds;
    CSurface8*          pSurface;
    HRESULT             hr;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dtc) {
        return ERR_NOTCREATED;
    }

    if (!ppCubeMapSurface) {
        return D3DERR_INVALIDCALL;
    }
#endif // D3DLOCUS_DEBUG

    *ppCubeMapSurface = NULL;

    hr = m_pd3dtc->GetCubeMapSurface(d3dcf, uLevel, &pd3ds);
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

        *ppCubeMapSurface = pSurface;
    }

    return hr;
}

//******************************************************************************
// CCCubeTexture8
//******************************************************************************

//******************************************************************************
CCCubeTexture8::CCCubeTexture8() {

    memset(m_pslockd, 0, 6 * sizeof(PSLOCKDESC));
}

//******************************************************************************
CCCubeTexture8::~CCCubeTexture8() {

    UINT i;
    for (i = 0; i < 6; i++) {
        if (m_pslockd[i]) {
            MemFree(m_pslockd[i]);
        }
    }
}

//******************************************************************************
BOOL CCCubeTexture8::Create(CClient* pClient, CDevice8* pDevice, LPDIRECT3DCUBETEXTURE8 pd3dtc) {

    UINT i;

    if (!pClient) {
        return FALSE;
    }
    m_pClient = pClient;
    if (!CCubeTexture8::Create(pDevice, pd3dtc)) {
        return FALSE;
    }

    for (i = 0; i < 6; i++) {
        m_pslockd[i] = (PSLOCKDESC)MemAlloc(pd3dtc->GetLevelCount() * sizeof(SLOCKDESC));
        if (!m_pslockd[i]) {
            return FALSE;
        }
    }

    return TRUE;
}

//******************************************************************************
ULONG CCCubeTexture8::AddRef() {

    return CCResource8_AddRef(this, m_pClient);
}

//******************************************************************************
ULONG CCCubeTexture8::Release() {

    return CCResource8_Release(this, m_pClient);
}

//******************************************************************************
D3DRESOURCETYPE CCCubeTexture8::GetType() {

    return CCResource8_GetType(this, m_pClient);
}

//******************************************************************************
HRESULT CCCubeTexture8::GetDevice(CDevice8** ppDevice) {

    return CCResource8_GetDevice(this, m_pClient, ppDevice);
}

//******************************************************************************
HRESULT CCCubeTexture8::GetPrivateData(REFGUID refguid, void* pvData, DWORD* pdwSizeOfData) {

    return CCResource8_GetPrivateData(this, m_pClient, refguid, pvData, pdwSizeOfData);
}

//******************************************************************************
HRESULT CCCubeTexture8::SetPrivateData(REFGUID refguid, void* pvData, DWORD dwSizeOfData, DWORD dwFlags) {

    return CCResource8_SetPrivateData(this, m_pClient, refguid, pvData, dwSizeOfData, dwFlags);
}

//******************************************************************************
HRESULT CCCubeTexture8::FreePrivateData(REFGUID refguid) {

    return CCResource8_FreePrivateData(this, m_pClient, refguid);
}

//******************************************************************************
DWORD CCCubeTexture8::GetPriority() {

    return CCResource8_GetPriority(this, m_pClient);
}

//******************************************************************************
DWORD CCCubeTexture8::SetPriority(DWORD dwNewPriority) {

    return CCResource8_SetPriority(this, m_pClient, dwNewPriority);
}

//******************************************************************************
void CCCubeTexture8::PreLoad() {

    CCResource8_PreLoad(this, m_pClient);
}

//******************************************************************************
DWORD CCCubeTexture8::GetLevelCount() {

    return CCBaseTexture8_GetLevelCount(this, m_pClient);
}

//******************************************************************************
DWORD CCCubeTexture8::GetLOD() {

    return CCBaseTexture8_GetLOD(this, m_pClient);
}

//******************************************************************************
DWORD CCCubeTexture8::SetLOD(DWORD dwNewLOD) {

    return CCBaseTexture8_SetLOD(this, m_pClient, dwNewLOD);
}

//******************************************************************************
HRESULT CCCubeTexture8::GetLevelDesc(UINT uLevel, D3DSURFACE_DESC* pd3dsd) {

#ifndef UNDER_XBOX
    D3DSURFACE_DESC         d3dsd;
#else
    WINX_D3DSURFACE_DESC    d3dsd;
#endif // UNDER_XBOX
    HRESULT                 hrC, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dtc) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    hrC = m_pd3dtc->GetLevelDesc(uLevel, pd3dsd);

#ifndef UNDER_XBOX
    if (m_pClient->CallMethod(MID_CTX8_GetLevelDesc, this, &hrS, uLevel, &d3dsd, sizeof(D3DSURFACE_DESC))) {
#else
    if (m_pClient->CallMethod(MID_CTX8_GetLevelDesc, this, &hrS, uLevel, &d3dsd, sizeof(WINX_D3DSURFACE_DESC))) {
#endif // UNDER_XBOX
        if (hrC != hrS) {
            Log(LOG_WARN, TEXT("IDirect3DCubeTexture8::GetLevelDesc result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
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
                Log(LOG_WARN, TEXT("IDirect3DCubeTexture8::GetLevelDesc returned an inconsistent description"));
            }
        }
    }

    return hrC;
}

//******************************************************************************
HRESULT CCCubeTexture8::LockRect(D3DCUBEMAP_FACES d3dcf, UINT uLevel, D3DLOCKED_RECT* pd3dlr, RECT* prect, DWORD dwFlags) {

    RECT    rectNULL = {-1, -1, -1, -1};
    HRESULT hrC, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dtc) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    hrC = m_pd3dtc->LockRect(d3dcf, uLevel, pd3dlr, prect, dwFlags);

    if (pd3dlr) {
        memcpy(&m_pslockd[d3dcf][uLevel].d3dlr, pd3dlr, sizeof(D3DLOCKED_RECT));
    }
    else {
        m_pslockd[d3dcf][uLevel].d3dlr.pBits = 0;
    }
    if (!prect) {
        prect = &rectNULL;
    }
    memcpy(&m_pslockd[d3dcf][uLevel].rect, prect, sizeof(RECT));
    m_pslockd[d3dcf][uLevel].dwFlags = dwFlags;

    if (m_pClient->CallMethod(MID_CTX8_LockRect, this, &hrS, d3dcf, uLevel, &m_pslockd[d3dcf][uLevel].d3dlrSrv, sizeof(D3DLOCKED_RECT), &m_pslockd[d3dcf][uLevel].rect, sizeof(RECT), dwFlags)) {
        if (hrC != hrS) {
            Log(LOG_WARN, TEXT("IDirect3DCubeTexture8::LockRect result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
        }
    }

    return hrC;
}

//******************************************************************************
HRESULT CCCubeTexture8::UnlockRect(D3DCUBEMAP_FACES d3dcf, UINT uLevel) {

    LPBYTE  pBits;
    DWORD   dwSize;
    HRESULT hrC, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dtc) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    if (m_pslockd[d3dcf][uLevel].dwFlags & D3DLOCK_READONLY) {
        pBits = NULL;
        dwSize = 0;
    }
    else {
        D3DSURFACE_DESC d3dsd;
        DWORD           dwWidth, dwHeight, dwBitDepth;
        UINT            i;

        m_pd3dtc->GetLevelDesc(uLevel, &d3dsd);
        if (m_pslockd[d3dcf][uLevel].rect.left == -1) {
            dwWidth = d3dsd.Width;
            dwHeight = d3dsd.Height;
        }
        else {
            dwWidth = m_pslockd[d3dcf][uLevel].rect.right - m_pslockd[d3dcf][uLevel].rect.left;
            dwHeight = m_pslockd[d3dcf][uLevel].rect.bottom - m_pslockd[d3dcf][uLevel].rect.top;
        }
        dwBitDepth = FormatToBitDepth(d3dsd.Format);
        dwSize = dwHeight * dwWidth * dwBitDepth / 8;
        pBits = (LPBYTE)MemAlloc(dwSize);

#ifdef UNDER_XBOX
        if (!XGIsSwizzledFormat(d3dsd.Format)) {
#endif
            if (IsCompressedFormat(d3dsd.Format)) {
                memcpy(pBits, m_pslockd[d3dcf][uLevel].d3dlr.pBits, dwWidth * dwHeight * dwBitDepth / 8);
            }
            else {
                for (i = 0; i < dwHeight; i++) {
                    memcpy(pBits + i * dwWidth * dwBitDepth / 8, (LPBYTE)m_pslockd[d3dcf][uLevel].d3dlr.pBits + i * m_pslockd[d3dcf][uLevel].d3dlr.Pitch, dwWidth * dwBitDepth / 8);
                }
            }
#ifdef UNDER_XBOX
        }
        else {
            // Since locking a subrect on a swizzled surface is unsupported on Xbox, simply unswizzle the entire surface
            XGUnswizzleRect(m_pslockd[d3dcf][uLevel].d3dlr.pBits, dwWidth, dwHeight, NULL, pBits, 
                            dwWidth * dwBitDepth / 8, NULL, dwBitDepth / 8);
        }
#endif
    }

    hrC = m_pd3dtc->UnlockRect(d3dcf, uLevel);

    if (m_pClient->CallMethod(MID_CTX8_UnlockRect, this, &hrS, d3dcf, uLevel, &m_pslockd[d3dcf][uLevel].d3dlrSrv, sizeof(D3DLOCKED_RECT), &m_pslockd[d3dcf][uLevel].rect, sizeof(RECT), m_pslockd[d3dcf][uLevel].dwFlags, pBits, dwSize)) {
        if (hrC != hrS) {
            Log(LOG_WARN, TEXT("IDirect3DCubeTexture8::UnlockRect result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
        }
    }

    if (pBits) {
        MemFree(pBits);
    }

    return hrC;
}

//******************************************************************************
HRESULT CCCubeTexture8::AddDirtyRect(D3DCUBEMAP_FACES d3dcf, RECT* prectDirty) {

    RECT    rectNULL = {-1, -1, -1, -1};
    HRESULT hrC, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dtc) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

#ifndef UNDER_XBOX
    hrC = m_pd3dtc->AddDirtyRect(d3dcf, prectDirty);

    if (!prectDirty) {
        prectDirty = &rectNULL;
    }

    if (m_pClient->CallMethod(MID_CTX8_AddDirtyRect, this, &hrS, d3dcf, prectDirty, sizeof(RECT))) {
        if (hrC != hrS) {
            Log(LOG_WARN, TEXT("IDirect3DCubeTexture8::AddDirtyRect result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
        }
    }

    return hrC;
#else
    Log(LOG_FAIL, TEXT("IDirect3DCubeTexture8::AddDirtyRect is not implemented on XBox"));
    return E_NOTIMPL;
#endif // UNDER_XBOX
}

//******************************************************************************
HRESULT CCCubeTexture8::GetCubeMapSurface(D3DCUBEMAP_FACES d3dcf, UINT uLevel, CSurface8** ppCubeMapSurface) {

    LPDIRECT3DSURFACE8  pd3ds;
    CCSurface8*         pSurface;
    HRESULT             hrC, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dtc) {
        return ERR_NOTCREATED;
    }

    if (!ppCubeMapSurface) {
        return D3DERR_INVALIDCALL;
    }
#endif // D3DLOCUS_DEBUG

    *ppCubeMapSurface = NULL;

    hrC = m_pd3dtc->GetCubeMapSurface(d3dcf, uLevel, &pd3ds);
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

        *ppCubeMapSurface = pSurface;
    }
    else {
        pSurface = NULL;
    }

    if (m_pClient->CallMethod(MID_CTX8_GetCubeMapSurface, this, &hrS, d3dcf, uLevel, &pSurface)) {
        if (hrC != hrS) {
            Log(LOG_WARN, TEXT("IDirect3DCubeTexture8::GetCubeMapSurface result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
        }
    }

    return hrC;
}
