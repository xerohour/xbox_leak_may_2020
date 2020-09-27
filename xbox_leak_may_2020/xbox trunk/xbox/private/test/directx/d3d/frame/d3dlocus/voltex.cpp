/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    voltex.cpp

Author:

    Matt Bronder

Description:

    CVolumeTexture methods.

*******************************************************************************/

#include "d3dlocus.h"

//******************************************************************************
// CVolumeTexture
//******************************************************************************

//******************************************************************************
CVolumeTexture::CVolumeTexture() {
}

//******************************************************************************
CVolumeTexture::~CVolumeTexture() {
}

//******************************************************************************
// CVolumeTexture8
//******************************************************************************

//******************************************************************************
CVolumeTexture8::CVolumeTexture8() {

    m_pd3dtv = NULL;
}

//******************************************************************************
CVolumeTexture8::~CVolumeTexture8() {
}

//******************************************************************************
BOOL CVolumeTexture8::Create(CDevice8* pDevice, LPDIRECT3DVOLUMETEXTURE8 pd3dtv) {

    if (m_pd3dtv) {
        return FALSE;
    }

    if (CBaseTexture8::Create(pDevice, (LPDIRECT3DBASETEXTURE8)pd3dtv)) {
        m_pd3dtv = pd3dtv;
    }

    return (BOOL)m_pd3dtv;
}

//******************************************************************************
LPDIRECT3DVOLUMETEXTURE8 CVolumeTexture8::GetIDirect3DVolumeTexture8() {

    return m_pd3dtv;
}

//******************************************************************************
HRESULT CVolumeTexture8::GetLevelDesc(UINT uLevel, D3DVOLUME_DESC* pd3dvd) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dtv) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    return m_pd3dtv->GetLevelDesc(uLevel, pd3dvd);
}

//******************************************************************************
HRESULT CVolumeTexture8::LockBox(UINT uLevel, D3DLOCKED_BOX* pd3dlb, D3DBOX* pbox, DWORD dwFlags) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dtv) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    return m_pd3dtv->LockBox(uLevel, pd3dlb, pbox, dwFlags);
}

//******************************************************************************
HRESULT CVolumeTexture8::UnlockBox(UINT uLevel) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dtv) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    return m_pd3dtv->UnlockBox(uLevel);
}

//******************************************************************************
HRESULT CVolumeTexture8::AddDirtyBox(D3DBOX* pboxDirty) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dtv) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

#ifndef UNDER_XBOX
    return m_pd3dtv->AddDirtyBox(pboxDirty);
#else
    Log(LOG_FAIL, TEXT("IDirect3DVolumeTexture8::AddDirtyBox is not implemented on XBox"));
    return E_NOTIMPL;
#endif // UNDER_XBOX
}

//******************************************************************************
HRESULT CVolumeTexture8::GetVolumeLevel(UINT uLevel, CVolume8** ppVolumeLevel) {

    LPDIRECT3DVOLUME8   pd3dv;
    CVolume8*           pVolume;
    HRESULT             hr;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dtv) {
        return ERR_NOTCREATED;
    }

    if (!ppVolumeLevel) {
        return D3DERR_INVALIDCALL;
    }
#endif // D3DLOCUS_DEBUG

    *ppVolumeLevel = NULL;

    hr = m_pd3dtv->GetVolumeLevel(uLevel, &pd3dv);
    if (SUCCEEDED(hr)) {
        
        if (m_pDevice->FindObject((LPVOID)pd3dv, (LPVOID*)&pVolume)) {
            pVolume->CObject::AddRef();
        }
        else {
            pVolume = new CVolume8();
            if (!pVolume) {
                pd3dv->Release();
                return E_OUTOFMEMORY;
            }

            if (!pVolume->Create(m_pDevice, pd3dv)) {
                pd3dv->Release();
                return ERR_LOCUSFAIL;
            }
        }

        *ppVolumeLevel = pVolume;
    }

    return hr;
}

//******************************************************************************
// CCVolumeTexture8
//******************************************************************************

//******************************************************************************
CCVolumeTexture8::CCVolumeTexture8() {

    m_pvlockd = NULL;
}

//******************************************************************************
CCVolumeTexture8::~CCVolumeTexture8() {

    if (m_pvlockd) {
        MemFree(m_pvlockd);
    }
}

//******************************************************************************
BOOL CCVolumeTexture8::Create(CClient* pClient, CDevice8* pDevice, LPDIRECT3DVOLUMETEXTURE8 pd3dtv) {

    if (!pClient) {
        return FALSE;
    }
    m_pClient = pClient;
    if (!CVolumeTexture8::Create(pDevice, pd3dtv)) {
        return FALSE;
    }

    m_pvlockd = (PVLOCKDESC)MemAlloc(pd3dtv->GetLevelCount() * sizeof(VLOCKDESC));
    if (!m_pvlockd) {
        return FALSE;
    }

    return TRUE;
}

//******************************************************************************
ULONG CCVolumeTexture8::AddRef() {

    return CCResource8_AddRef(this, m_pClient);
}

//******************************************************************************
ULONG CCVolumeTexture8::Release() {

    return CCResource8_Release(this, m_pClient);
}

//******************************************************************************
D3DRESOURCETYPE CCVolumeTexture8::GetType() {

    return CCResource8_GetType(this, m_pClient);
}

//******************************************************************************
HRESULT CCVolumeTexture8::GetDevice(CDevice8** ppDevice) {

    return CCResource8_GetDevice(this, m_pClient, ppDevice);
}

//******************************************************************************
HRESULT CCVolumeTexture8::GetPrivateData(REFGUID refguid, void* pvData, DWORD* pdwSizeOfData) {

    return CCResource8_GetPrivateData(this, m_pClient, refguid, pvData, pdwSizeOfData);
}

//******************************************************************************
HRESULT CCVolumeTexture8::SetPrivateData(REFGUID refguid, void* pvData, DWORD dwSizeOfData, DWORD dwFlags) {

    return CCResource8_SetPrivateData(this, m_pClient, refguid, pvData, dwSizeOfData, dwFlags);
}

//******************************************************************************
HRESULT CCVolumeTexture8::FreePrivateData(REFGUID refguid) {

    return CCResource8_FreePrivateData(this, m_pClient, refguid);
}

//******************************************************************************
DWORD CCVolumeTexture8::GetPriority() {

    return CCResource8_GetPriority(this, m_pClient);
}

//******************************************************************************
DWORD CCVolumeTexture8::SetPriority(DWORD dwNewPriority) {

    return CCResource8_SetPriority(this, m_pClient, dwNewPriority);
}

//******************************************************************************
void CCVolumeTexture8::PreLoad() {

    CCResource8_PreLoad(this, m_pClient);
}

//******************************************************************************
DWORD CCVolumeTexture8::GetLevelCount() {

    return CCBaseTexture8_GetLevelCount(this, m_pClient);
}

//******************************************************************************
DWORD CCVolumeTexture8::GetLOD() {

    return CCBaseTexture8_GetLOD(this, m_pClient);
}

//******************************************************************************
DWORD CCVolumeTexture8::SetLOD(DWORD dwNewLOD) {

    return CCBaseTexture8_SetLOD(this, m_pClient, dwNewLOD);
}

//******************************************************************************
HRESULT CCVolumeTexture8::GetLevelDesc(UINT uLevel, D3DVOLUME_DESC* pd3dvd) {

#ifndef UNDER_XBOX
    D3DVOLUME_DESC      d3dvd;
#else
    WINX_D3DVOLUME_DESC d3dvd;
#endif // UNDER_XBOX
    HRESULT             hrC, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dtv) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    hrC = m_pd3dtv->GetLevelDesc(uLevel, pd3dvd);

#ifndef UNDER_XBOX
    if (m_pClient->CallMethod(MID_VTX8_GetLevelDesc, this, &hrS, uLevel, &d3dvd, sizeof(D3DVOLUME_DESC))) {
#else
    if (m_pClient->CallMethod(MID_VTX8_GetLevelDesc, this, &hrS, uLevel, &d3dvd, sizeof(WINX_D3DVOLUME_DESC))) {
#endif // UNDER_XBOX
        if (hrC != hrS) {
            Log(LOG_WARN, TEXT("IDirect3DVolumeTexture8::GetLevelDesc result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
        }
        else {
#ifndef UNDER_XBOX
            if (memcmp(pd3dvd, &d3dvd, sizeof(D3DVOLUME_DESC))) {
#else
            if (pd3dvd->Type != d3dvd.Type ||
                pd3dvd->Width != d3dvd.Width ||
                pd3dvd->Height != d3dvd.Height ||
                pd3dvd->Depth != d3dvd.Depth)
            {
#endif // UNDER_XBOX
                Log(LOG_WARN, TEXT("IDirect3DVolumeTexture8::GetLevelDesc returned an inconsistent description"));
            }
        }
    }

    return hrC;
}

//******************************************************************************
HRESULT CCVolumeTexture8::LockBox(UINT uLevel, D3DLOCKED_BOX* pd3dlb, D3DBOX* pbox, DWORD dwFlags) {

    HRESULT hrC, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dtv) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    hrC = m_pd3dtv->LockBox(uLevel, pd3dlb, pbox, dwFlags);

    if (pd3dlb) {
        memcpy(&m_pvlockd[uLevel].d3dlb, pd3dlb, sizeof(D3DLOCKED_BOX));
    }
    else {
        m_pvlockd[uLevel].d3dlb.pBits = 0;
    }
    if (pbox) {
        memcpy(&m_pvlockd[uLevel].box, pbox, sizeof(D3DBOX));
    }
    else {
        m_pvlockd[uLevel].box.Left = m_pvlockd[uLevel].box.Right = m_pvlockd[uLevel].box.Top = m_pvlockd[uLevel].box.Bottom = m_pvlockd[uLevel].box.Front = m_pvlockd[uLevel].box.Back = -1;
    }
    m_pvlockd[uLevel].dwFlags = dwFlags;

    if (m_pClient->CallMethod(MID_VTX8_LockBox, this, &hrS, uLevel, &m_pvlockd[uLevel].d3dlbSrv, sizeof(D3DLOCKED_BOX), &m_pvlockd[uLevel].box, sizeof(D3DBOX), dwFlags)) {
        if (hrC != hrS) {
            Log(LOG_WARN, TEXT("IDirect3DVolumeTexture8::LockBox result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
        }
    }

    return hrC;
}

//******************************************************************************
HRESULT CCVolumeTexture8::UnlockBox(UINT uLevel) {

    LPBYTE  pBits;
    DWORD   dwSize;
    HRESULT hrC, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dtv) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    if (m_pvlockd[uLevel].dwFlags & D3DLOCK_READONLY) {
        pBits = NULL;
        dwSize = 0;
    }
    else {
        D3DVOLUME_DESC  d3dvd;
        DWORD           dwWidth, dwHeight, dwDepth, dwBitDepth;
        UINT            i, j;

        m_pd3dtv->GetLevelDesc(uLevel, &d3dvd);
        if (m_pvlockd[uLevel].box.Left == -1) {
            dwWidth = d3dvd.Width;
            dwHeight = d3dvd.Height;
            dwDepth = d3dvd.Depth;
        }
        else {
            dwWidth = m_pvlockd[uLevel].box.Right - m_pvlockd[uLevel].box.Left;
            dwHeight = m_pvlockd[uLevel].box.Bottom - m_pvlockd[uLevel].box.Top;
            dwDepth = m_pvlockd[uLevel].box.Back - m_pvlockd[uLevel].box.Front;
        }
        dwBitDepth = FormatToBitDepth(d3dvd.Format);
        dwSize = dwHeight * dwWidth * dwDepth * dwBitDepth / 8;
        pBits = (LPBYTE)MemAlloc(dwSize);

#ifdef UNDER_XBOX
        if (!XGIsSwizzledFormat(d3dvd.Format)) {
#endif
            if (IsCompressedFormat(d3dvd.Format)) {
                memcpy(pBits, m_pvlockd[uLevel].d3dlb.pBits, dwWidth * dwHeight * dwDepth * dwBitDepth / 8);
            }
            else {
                for (j = 0; j < dwDepth; j++) {
                    for (i = 0; i < dwHeight; i++) {
                        memcpy(pBits + j * dwHeight * dwWidth * dwBitDepth / 8 + i * dwWidth * dwBitDepth / 8, (LPBYTE)m_pvlockd[uLevel].d3dlb.pBits + j * m_pvlockd[uLevel].d3dlb.SlicePitch + i * m_pvlockd[uLevel].d3dlb.RowPitch, dwWidth * dwBitDepth / 8);
                    }
                }
            }
#ifdef UNDER_XBOX
        }
        else {
            // Since locking a sub-box on a swizzled volume is unsupported on Xbox, simply unswizzle the entire volume
            XGUnswizzleBox(m_pvlockd[uLevel].d3dlb.pBits, dwWidth, dwHeight, dwDepth, NULL, pBits, 
                           dwWidth * dwBitDepth / 8, dwWidth * dwHeight * dwBitDepth / 8, NULL, dwBitDepth / 8);
        }
#endif
    }

    hrC = m_pd3dtv->UnlockBox(uLevel);

    if (m_pClient->CallMethod(MID_VTX8_UnlockBox, this, &hrS, uLevel, &m_pvlockd[uLevel].d3dlbSrv, sizeof(D3DLOCKED_BOX), &m_pvlockd[uLevel].box, sizeof(D3DBOX), m_pvlockd[uLevel].dwFlags, pBits, dwSize)) {
        if (hrC != hrS) {
            Log(LOG_WARN, TEXT("IDirect3DVolumeTexture8::UnlockBox result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
        }
    }

    if (pBits) {
        MemFree(pBits);
    }

    return hrC;
}

//******************************************************************************
HRESULT CCVolumeTexture8::AddDirtyBox(D3DBOX* pboxDirty) {

    D3DBOX  boxNULL = {-1, -1, -1, -1, -1, -1};
    HRESULT hrC, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dtv) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

#ifndef UNDER_XBOX
    hrC = m_pd3dtv->AddDirtyBox(pboxDirty);

    if (!pboxDirty) {
        pboxDirty = &boxNULL;
    }

    if (m_pClient->CallMethod(MID_VTX8_AddDirtyBox, this, &hrS, pboxDirty, sizeof(D3DBOX))) {
        if (hrC != hrS) {
            Log(LOG_WARN, TEXT("IDirect3DVolumeTexture8::AddDirtyBox result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
        }
    }

    return hrC;
#else
    Log(LOG_FAIL, TEXT("IDirect3DVolumeTexture8::AddDirtyBox is not implemented on XBox"));
    return E_NOTIMPL;
#endif // UNDER_XBOX
}

//******************************************************************************
HRESULT CCVolumeTexture8::GetVolumeLevel(UINT uLevel, CVolume8** ppVolumeLevel) {

    LPDIRECT3DVOLUME8   pd3dv;
    CCVolume8*          pVolume;
    HRESULT             hrC, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dtv) {
        return ERR_NOTCREATED;
    }

    if (!ppVolumeLevel) {
        return D3DERR_INVALIDCALL;
    }
#endif // D3DLOCUS_DEBUG

    *ppVolumeLevel = NULL;

    hrC = m_pd3dtv->GetVolumeLevel(uLevel, &pd3dv);
    if (SUCCEEDED(hrC)) {
        
        if (m_pDevice->FindObject((LPVOID)pd3dv, (LPVOID*)&pVolume)) {
            pVolume->CObject::AddRef();
        }
        else {
            pVolume = new CCVolume8();
            if (!pVolume) {
                pd3dv->Release();
                return E_OUTOFMEMORY;
            }

            if (!pVolume->Create(m_pClient, m_pDevice, pd3dv)) {
                pd3dv->Release();
                return ERR_LOCUSFAIL;
            }
        }

        *ppVolumeLevel = pVolume;
    }
    else {
        pVolume = NULL;
    }

    if (m_pClient->CallMethod(MID_VTX8_GetVolumeLevel, this, &hrS, uLevel, &pVolume)) {
        if (hrC != hrS) {
            Log(LOG_WARN, TEXT("IDirect3DVolumeTexture8::GetVolumeLevel result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
        }
    }

    return hrC;
}
