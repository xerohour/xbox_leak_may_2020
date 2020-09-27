/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    volume.cpp

Author:

    Matt Bronder

Description:

    CVolume methods.

*******************************************************************************/

#include "d3dlocus.h"

#ifdef UNDER_XBOX
#define DEMO_HACK
#endif // UNDER_XBOX

//******************************************************************************
// CVolume
//******************************************************************************

//******************************************************************************
CVolume::CVolume() {
}

//******************************************************************************
CVolume::~CVolume() {
}

//******************************************************************************
// CVolume8
//******************************************************************************

//******************************************************************************
CVolume8::CVolume8() {

    m_pd3dv = NULL;
}

//******************************************************************************
CVolume8::~CVolume8() {

    if (m_pDevice) {
        m_pDevice->RemoveObject(m_pd3dv);
    }
}

//******************************************************************************
BOOL CVolume8::Create(CDevice8* pDevice, LPDIRECT3DVOLUME8 pd3dv) {

    if (m_pd3dv) {
        return FALSE;
    }

    if (pDevice && pd3dv) {

        if (!pDevice->AddObject(pd3dv, this)) {
            return FALSE;
        }

        m_pDevice = pDevice;

        m_pd3dv = pd3dv;
    }

    return (BOOL)m_pd3dv;
}

//******************************************************************************
ULONG CVolume8::AddRef() {

    ULONG uRef = 0;

    if (m_pd3dv) {
        uRef = m_pd3dv->AddRef();
        CObject::AddRef();
    }

    return uRef;
}

//******************************************************************************
ULONG CVolume8::Release() {

    ULONG uRef = 0;

    if (m_pd3dv) {
        uRef = m_pd3dv->Release();
        CObject::Release();
    }

    return uRef;
}

//******************************************************************************
LPDIRECT3DVOLUME8 CVolume8::GetIDirect3DVolume8() {

    return m_pd3dv;
}

//******************************************************************************
HRESULT CVolume8::GetContainer(REFIID riid, CVolumeTexture8** ppContainer) {

    LPDIRECT3DVOLUMETEXTURE8 pd3dtv;
    CVolumeTexture8*         pVolumeTexture;
    HRESULT                  hr = ERR_NOTCREATED;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dv) {
        return ERR_NOTCREATED;
    }
#ifndef UNDER_XBOX
    if (riid != IID_IDirect3DVolumeTexture8) {
        return D3DERR_INVALIDCALL;
    }
#endif // !UNDER_XBOX
    if (!ppContainer) {
        return D3DERR_INVALIDCALL;
    }
#endif // D3DLOCUS_DEBUG

    *ppContainer = NULL;

#ifndef UNDER_XBOX
    hr = m_pd3dv->GetContainer(riid, (LPVOID*)&pd3dtv);
#else
#ifndef DEMO_HACK
    hr = m_pd3dv->GetContainer(&pd3dtv);
#else
    hr = m_pd3dv->GetContainer((LPDIRECT3DBASETEXTURE8*)&pd3dtv);
#endif
#endif // UNDER_XBOX
    if (SUCCEEDED(hr)) {
        if (!m_pDevice->FindObject(pd3dtv, (LPVOID*)&pVolumeTexture)) {
            pd3dtv->Release();
            return ERR_NOTCREATED;
        }
        else {
            pVolumeTexture->CObject::AddRef();
            *ppContainer = pVolumeTexture;
        }
    }

    return hr;
}

//******************************************************************************
HRESULT CVolume8::GetDevice(CDevice8** ppDevice) {

    LPDIRECT3DDEVICE8 pd3dd;
    HRESULT           hr = ERR_NOTCREATED;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dv || !m_pDevice) {
        return ERR_NOTCREATED;
    }
    if (!ppDevice) {
        return D3DERR_INVALIDCALL;
    }
#endif // D3DLOCUS_DEBUG

    *ppDevice = NULL;

    hr = m_pd3dv->GetDevice(&pd3dd);
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
HRESULT CVolume8::GetDesc(D3DVOLUME_DESC* pd3dvd) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dv) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    return m_pd3dv->GetDesc(pd3dvd);
}

//******************************************************************************
HRESULT CVolume8::LockBox(D3DLOCKED_BOX* pd3dlb, D3DBOX* pbox, DWORD dwFlags) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dv) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    return m_pd3dv->LockBox(pd3dlb, pbox, dwFlags);
}

//******************************************************************************
HRESULT CVolume8::UnlockBox() {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dv) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    return m_pd3dv->UnlockBox();
}

//******************************************************************************
HRESULT CVolume8::GetPrivateData(REFGUID refguid, void* pvData, DWORD* pdwSizeOfData) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dv) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

#ifndef DEMO_HACK
    return m_pd3dv->GetPrivateData(refguid, pvData, pdwSizeOfData);
#else
    return E_NOTIMPL;
#endif // DEMO_HACK
}

//******************************************************************************
HRESULT CVolume8::SetPrivateData(REFGUID refguid, void* pvData, DWORD dwSizeOfData, DWORD dwFlags) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dv) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

#ifndef DEMO_HACK
    return m_pd3dv->SetPrivateData(refguid, pvData, dwSizeOfData, dwFlags);
#else
    return E_NOTIMPL;
#endif // DEMO_HACK
}

//******************************************************************************
HRESULT CVolume8::FreePrivateData(REFGUID refguid) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dv) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

#ifndef DEMO_HACK
    return m_pd3dv->FreePrivateData(refguid);
#else
    return E_NOTIMPL;
#endif // DEMO_HACK
}

//******************************************************************************
// CCVolume8
//******************************************************************************

//******************************************************************************
CCVolume8::CCVolume8() {
}

//******************************************************************************
CCVolume8::~CCVolume8() {
}

//******************************************************************************
BOOL CCVolume8::Create(CClient* pClient, CDevice8* pDevice, LPDIRECT3DVOLUME8 pd3dv) {

    if (!pClient) {
        return FALSE;
    }
    m_pClient = pClient;
    return CVolume8::Create(pDevice, pd3dv);
}

//******************************************************************************
ULONG CCVolume8::AddRef() {

    ULONG uRefC = 0, uRefS;

    if (m_pd3dv) {
        uRefC = m_pd3dv->AddRef();
        if (m_pClient->CallMethod(MID_VOL8_AddRef, this, &uRefS)) {
#ifndef UNDER_XBOX
            if (uRefC != uRefS) {
                Log(LOG_WARN, TEXT("IDirect3DVolume8::AddRef result inconsistent [C: %d, S: %d]"), uRefC, uRefS);
            }
#endif // !UNDER_XBOX
        }
        CObject::AddRef();
    }

    return uRefC;
}

//******************************************************************************
ULONG CCVolume8::Release() {

    ULONG uRefC = 0, uRefS;

    if (m_pd3dv) {
        uRefC = m_pd3dv->Release();
        if (m_pClient->CallMethod(MID_VOL8_Release, this, &uRefS)) {
#ifndef UNDER_XBOX
            if (uRefC != uRefS) {
                Log(LOG_WARN, TEXT("IDirect3DVolume8::Release result inconsistent [C: %d, S: %d]"), uRefC, uRefS);
            }
#endif // !UNDER_XBOX
        }
        CObject::Release();
    }

    return uRefC;
}

//******************************************************************************
HRESULT CCVolume8::GetContainer(REFIID riid, CVolumeTexture8** ppContainer) {

    LPDIRECT3DVOLUMETEXTURE8 pd3dtv;
    CVolumeTexture8*         pVolumeTexture;
    HRESULT                  hrC = ERR_NOTCREATED, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dv) {
        return ERR_NOTCREATED;
    }
#ifndef UNDER_XBOX
    if (riid != IID_IDirect3DVolumeTexture8) {
        return D3DERR_INVALIDCALL;
    }
#endif // !UNDER_XBOX
    if (!ppContainer) {
        return D3DERR_INVALIDCALL;
    }
#endif // D3DLOCUS_DEBUG

    *ppContainer = NULL;

#ifndef UNDER_XBOX
    hrC = m_pd3dv->GetContainer(riid, (LPVOID*)&pd3dtv);
#else
#ifndef DEMO_HACK
    hrC = m_pd3dv->GetContainer(&pd3dtv);
#else
    hrC = m_pd3dv->GetContainer((LPDIRECT3DBASETEXTURE8*)&pd3dtv);
#endif
#endif // UNDER_XBOX
    if (SUCCEEDED(hrC)) {
        if (!m_pDevice->FindObject(pd3dtv, (LPVOID*)&pVolumeTexture)) {
            pd3dtv->Release();
            return ERR_NOTCREATED;
        }
        else {
            pVolumeTexture->CObject::AddRef();
            *ppContainer = pVolumeTexture;
            if (m_pClient->CallMethod(MID_VOL8_GetContainer, this, &hrS, &riid, sizeof(GUID), &pVolumeTexture)) {
                if (hrC != hrS) {
                    Log(LOG_WARN, TEXT("IDirect3DVolume8::GetContainer result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
                }
            }
        }
    }

    return hrC;
}

//******************************************************************************
HRESULT CCVolume8::GetDevice(CDevice8** ppDevice) {

    LPDIRECT3DDEVICE8 pd3dd;
    CDevice8*         pDevice;
    HRESULT           hrC = ERR_NOTCREATED, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dv || !m_pDevice) {
        return ERR_NOTCREATED;
    }
    if (!ppDevice) {
        return D3DERR_INVALIDCALL;
    }
#endif // D3DLOCUS_DEBUG

    *ppDevice = NULL;

    hrC = m_pd3dv->GetDevice(&pd3dd);
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

    if (m_pClient->CallMethod(MID_VOL8_GetDevice, this, &hrS, &pDevice)) {
        if (hrC != hrS) {
            Log(LOG_WARN, TEXT("IDirect3DVolume8::GetDevice result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
        }
    }

    return hrC;
}

//******************************************************************************
HRESULT CCVolume8::GetDesc(D3DVOLUME_DESC* pd3dvd) {

#ifndef UNDER_XBOX
    D3DVOLUME_DESC      d3dvd;
#else
    WINX_D3DVOLUME_DESC d3dvd;
#endif // UNDER_XBOX
    HRESULT             hrC, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dv) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    hrC = m_pd3dv->GetDesc(pd3dvd);

#ifndef UNDER_XBOX
    if (m_pClient->CallMethod(MID_VOL8_GetDesc, this, &hrS, &d3dvd, sizeof(D3DVOLUME_DESC))) {
#else
    if (m_pClient->CallMethod(MID_VOL8_GetDesc, this, &hrS, &d3dvd, sizeof(WINX_D3DVOLUME_DESC))) {
#endif // UNDER_XBOX
        if (hrC != hrS) {
            Log(LOG_WARN, TEXT("IDirect3DVolume8::GetDesc result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
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
                Log(LOG_WARN, TEXT("IDirect3DVolume8::GetDesc returned an inconsistent description"));
            }
        }
    }

    return hrC;
}

//******************************************************************************
HRESULT CCVolume8::LockBox(D3DLOCKED_BOX* pd3dlb, D3DBOX* pbox, DWORD dwFlags) {

    HRESULT hrC, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dv) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    hrC = m_pd3dv->LockBox(pd3dlb, pbox, dwFlags);

    if (pd3dlb) {
        memcpy(&m_vlockd.d3dlb, pd3dlb, sizeof(D3DLOCKED_BOX));
    }
    else {
        m_vlockd.d3dlb.pBits = 0;
    }
    if (pbox) {
        memcpy(&m_vlockd.box, pbox, sizeof(D3DBOX));
    }
    else {
        m_vlockd.box.Left = m_vlockd.box.Right = m_vlockd.box.Top = m_vlockd.box.Bottom = m_vlockd.box.Front = m_vlockd.box.Back = -1;
    }
    m_vlockd.dwFlags = dwFlags;

    if (m_pClient->CallMethod(MID_VOL8_LockBox, this, &hrS, &m_vlockd.d3dlbSrv, sizeof(D3DLOCKED_BOX), &m_vlockd.box, sizeof(D3DBOX), dwFlags)) {
        if (hrC != hrS) {
            Log(LOG_WARN, TEXT("IDirect3DVolume8::LockBox result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
        }
    }

    return hrC;
}

//******************************************************************************
HRESULT CCVolume8::UnlockBox() {

    LPBYTE  pBits;
    DWORD   dwSize;
    HRESULT hrC, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dv) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    if (m_vlockd.dwFlags & D3DLOCK_READONLY) {
        pBits = NULL;
        dwSize = 0;
    }
    else {
        D3DVOLUME_DESC  d3dvd;
        DWORD           dwWidth, dwHeight, dwDepth, dwBitDepth;
        UINT            i, j;

        m_pd3dv->GetDesc(&d3dvd);
        if (m_vlockd.box.Left == -1) {
            dwWidth = d3dvd.Width;
            dwHeight = d3dvd.Height;
            dwDepth = d3dvd.Depth;
        }
        else {
            dwWidth = m_vlockd.box.Right - m_vlockd.box.Left;
            dwHeight = m_vlockd.box.Bottom - m_vlockd.box.Top;
            dwDepth = m_vlockd.box.Back - m_vlockd.box.Front;
        }
        dwBitDepth = FormatToBitDepth(d3dvd.Format);
        dwSize = dwHeight * dwWidth * dwDepth * dwBitDepth / 8;
        pBits = (LPBYTE)MemAlloc(dwSize);

#ifdef UNDER_XBOX
        if (!XGIsSwizzledFormat(d3dvd.Format)) {
#endif
            if (IsCompressedFormat(d3dvd.Format)) {
                memcpy(pBits, m_vlockd.d3dlb.pBits, dwWidth * dwHeight * dwDepth * dwBitDepth / 8);
            }
            else {
                for (j = 0; j < dwDepth; j++) {
                    for (i = 0; i < dwHeight; i++) {
                        memcpy(pBits + j * dwHeight * dwWidth * dwBitDepth / 8 + i * dwWidth * dwBitDepth / 8, (LPBYTE)m_vlockd.d3dlb.pBits + j * m_vlockd.d3dlb.SlicePitch + i * m_vlockd.d3dlb.RowPitch, dwWidth * dwBitDepth / 8);
                    }
                }
            }
#ifdef UNDER_XBOX
        }
        else {
            // Since locking a sub-box on a swizzled volume is unsupported on Xbox, simply unswizzle the entire volume
            XGUnswizzleBox(m_vlockd.d3dlb.pBits, dwWidth, dwHeight, dwDepth, NULL, pBits, 
                           dwWidth * dwBitDepth / 8, dwWidth * dwHeight * dwBitDepth / 8, NULL, dwBitDepth / 8);
        }
#endif
    }

    hrC = m_pd3dv->UnlockBox();

    if (m_pClient->CallMethod(MID_VOL8_UnlockBox, this, &hrS, &m_vlockd.d3dlbSrv, sizeof(D3DLOCKED_BOX), &m_vlockd.box, sizeof(D3DBOX), m_vlockd.dwFlags, pBits, dwSize)) {
        if (hrC != hrS) {
            Log(LOG_WARN, TEXT("IDirect3DVolume8::UnlockBox result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
        }
    }

    if (pBits) {
        MemFree(pBits);
    }

    return hrC;
}

//******************************************************************************
HRESULT CCVolume8::GetPrivateData(REFGUID refguid, void* pvData, DWORD* pdwSizeOfData) {

    HRESULT hrC, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dv) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    hrC = m_pd3dv->GetPrivateData(refguid, pvData, pdwSizeOfData);

    if (m_pClient->CallMethod(MID_VOL8_GetPrivateData, this, &hrS, &refguid, sizeof(GUID), pvData, *pdwSizeOfData, pdwSizeOfData)) {
        if (hrC != hrS) {
            Log(LOG_WARN, TEXT("IDirect3DVolume8::GetPrivateData result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
        }
    }

    return hrC;
}

//******************************************************************************
HRESULT CCVolume8::SetPrivateData(REFGUID refguid, void* pvData, DWORD dwSizeOfData, DWORD dwFlags) {

    HRESULT hrC, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dv) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    hrC = m_pd3dv->SetPrivateData(refguid, pvData, dwSizeOfData, dwFlags);

    if (m_pClient->CallMethod(MID_VOL8_SetPrivateData, this, &hrS, &refguid, sizeof(GUID), pvData, dwSizeOfData, dwSizeOfData, dwFlags)) {
        if (hrC != hrS) {
            Log(LOG_WARN, TEXT("IDirect3DVolume8::SetPrivateData result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
        }
    }

    return hrC;
}

//******************************************************************************
HRESULT CCVolume8::FreePrivateData(REFGUID refguid) {

    HRESULT hrC, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dv) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    hrC = m_pd3dv->FreePrivateData(refguid);

    if (m_pClient->CallMethod(MID_VOL8_FreePrivateData, this, &hrS, &refguid, sizeof(GUID))) {
        if (hrC != hrS) {
            Log(LOG_WARN, TEXT("IDirect3DVolume8::FreePrivateData result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
        }
    }

    return hrC;
}
