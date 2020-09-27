/*******************************************************************************

Copyright (c) 2000 Microsoft Corporation.  All rights reserved.

File Name:

    device.cpp

Author:

    Matt Bronder

Description:

    CDevice methods.

*******************************************************************************/

#include "d3dlocus.h"

#ifdef UNDER_XBOX
#define DEMO_HACK
#endif // UNDER_XBOX

#define FBSCALE 0.3f

//******************************************************************************
#ifdef UNDER_XBOX

#define RDH_RECTANGLES  1

typedef struct _RGNDATAHEADER {
    DWORD   dwSize;
    DWORD   iType;
    DWORD   nCount;
    DWORD   nRgnSize;
    RECT    rcBound;
} RGNDATAHEADER, *PRGNDATAHEADER;

typedef struct _RGNDATA {
    RGNDATAHEADER   rdh;
    char            Buffer[1];
} RGNDATA, *PRGNDATA, NEAR *NPRGNDATA, FAR *LPRGNDATA;

#endif // UNDER_XBOX

//******************************************************************************
// CDevice
//******************************************************************************

//******************************************************************************
CDevice::CDevice() {
}

//******************************************************************************
CDevice::~CDevice() {
}

//******************************************************************************
// CDevice8
//******************************************************************************

//******************************************************************************
CDevice8::CDevice8() {

    m_pd3dd = NULL;
    m_pObjectMap = NULL;
}

//******************************************************************************
CDevice8::~CDevice8() {

    if (m_pObjectMap) {
        delete m_pObjectMap;
    }
}

//******************************************************************************
BOOL CDevice8::Create(CDirect3D8* pDirect3D, LPDIRECT3DDEVICE8 pd3dd) {

    if (m_pd3dd) {
        return FALSE;
    }

    if (pDirect3D && pd3dd) {

        m_pObjectMap = new CMap32();
        if (!m_pObjectMap) {
            return FALSE;
        }

        m_pDirect3D = pDirect3D;

        m_pd3dd = pd3dd;
    }

    return (BOOL)m_pd3dd;
}

//******************************************************************************
ULONG CDevice8::AddRef() {

    ULONG uRef = 0;

    if (m_pd3dd) {
        uRef = m_pd3dd->AddRef();
        CObject::AddRef();
    }

    return uRef;
}

//******************************************************************************
ULONG CDevice8::Release() {

    ULONG uRef = 0;

    if (m_pd3dd) {
        uRef = m_pd3dd->Release();
        CObject::Release();
    }

    return uRef;
}

//******************************************************************************
LPDIRECT3DDEVICE8 CDevice8::GetIDirect3DDevice8() {

    return m_pd3dd;
}

//******************************************************************************
BOOL CDevice8::AddObject(LPVOID pvInterface, LPVOID pvObject) {

#ifdef D3DLOCUS_DEBUG
    if (!pvInterface || !pvObject || !m_pObjectMap) {
        return FALSE;
    }
#endif // D3DLOCUS_DEBUG

    return m_pObjectMap->Add((DWORD)pvInterface, (DWORD)pvObject);
}

//******************************************************************************
BOOL CDevice8::RemoveObject(LPVOID pvInterface) {

#ifdef D3DLOCUS_DEBUG
    if (!pvInterface || !m_pObjectMap) {
        return FALSE;
    }
#endif // D3DLOCUS_DEBUG

    return m_pObjectMap->Remove((DWORD)pvInterface);
}

//******************************************************************************
BOOL CDevice8::FindObject(LPVOID pvInterface, LPVOID* ppvObject) {

    LPVOID  pvObject;
    BOOL    bRet;

#ifdef D3DLOCUS_DEBUG
    if (!pvInterface) {
        return FALSE;
    }
#endif // D3DLOCUS_DEBUG

    bRet = m_pObjectMap->Find((DWORD)pvInterface, (LPDWORD)&pvObject);
    if (bRet && ppvObject) {
        *ppvObject = pvObject;
    }

    return bRet;
}

#ifndef UNDER_XBOX

//******************************************************************************
HRESULT CDevice8::CreateAdditionalSwapChain(D3DPRESENT_PARAMETERS* pd3dpp, CSwapChain8** ppSwapChain) {

    CSwapChain8*         pSwapChain;
    LPDIRECT3DSWAPCHAIN8 pd3dsw;
    HRESULT              hr;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }

    if (!ppSwapChain) {
        return D3DERR_INVALIDCALL;
    }
#endif // D3DLOCUS_DEBUG

    *ppSwapChain = NULL;

#ifndef DEMO_HACK
    hr = m_pd3dd->CreateAdditionalSwapChain(pd3dpp, &pd3dsw);
#else
    hr = E_NOTIMPL;
    pd3dsw = NULL;
#endif
    if (SUCCEEDED(hr)) {
        pSwapChain = new CSwapChain8();
        if (!pSwapChain) {
            pd3dsw->Release();
            return E_OUTOFMEMORY;
        }

        if (!pSwapChain->Create(this, pd3dsw)) {
            pd3dsw->Release();
            return ERR_LOCUSFAIL;
        }

        *ppSwapChain = pSwapChain;
    }

    return hr;
}

#endif // !UNDER_XBOX

//******************************************************************************
HRESULT CDevice8::CreateCubeTexture(UINT uEdgeLength, UINT uLevels, DWORD dwUsage, D3DFORMAT fmt, D3DPOOL pool, CCubeTexture8** ppCubeTexture) {

    CCubeTexture8*         pCubeTexture;
    LPDIRECT3DCUBETEXTURE8 pd3dtc;
    HRESULT                hr;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }

    if (!ppCubeTexture) {
        return D3DERR_INVALIDCALL;
    }
#endif // D3DLOCUS_DEBUG

    *ppCubeTexture = NULL;

    hr = m_pd3dd->CreateCubeTexture(uEdgeLength, uLevels, dwUsage, fmt, pool, &pd3dtc);
    if (SUCCEEDED(hr)) {
        pCubeTexture = new CCubeTexture8();
        if (!pCubeTexture) {
            pd3dtc->Release();
            return E_OUTOFMEMORY;
        }

        if (!pCubeTexture->Create(this, pd3dtc)) {
            pd3dtc->Release();
            return ERR_LOCUSFAIL;
        }

        *ppCubeTexture = pCubeTexture;
    }

    return hr;
}

//******************************************************************************
HRESULT CDevice8::CreateDepthStencilSurface(UINT uWidth, UINT uHeight, D3DFORMAT fmt, D3DMULTISAMPLE_TYPE mst, CSurface8** ppSurface) {

    CSurface8*         pSurface;
    LPDIRECT3DSURFACE8 pd3ds;
    HRESULT            hr;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }

    if (!ppSurface) {
        return D3DERR_INVALIDCALL;
    }
#endif // D3DLOCUS_DEBUG

    *ppSurface = NULL;

    hr = m_pd3dd->CreateDepthStencilSurface(uWidth, uHeight, fmt, mst, &pd3ds);
    if (SUCCEEDED(hr)) {
        pSurface = new CSurface8();
        if (!pSurface) {
            pd3ds->Release();
            return E_OUTOFMEMORY;
        }

        if (!pSurface->Create(this, pd3ds)) {
            pd3ds->Release();
            return ERR_LOCUSFAIL;
        }

        *ppSurface = pSurface;
    }

    return hr;
}

//******************************************************************************
HRESULT CDevice8::CreateImageSurface(UINT uWidth, UINT uHeight, D3DFORMAT fmt, CSurface8** ppSurface) {

    CSurface8*         pSurface;
    LPDIRECT3DSURFACE8 pd3ds;
    HRESULT            hr;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }

    if (!ppSurface) {
        return D3DERR_INVALIDCALL;
    }
#endif // D3DLOCUS_DEBUG

    *ppSurface = NULL;

    hr = m_pd3dd->CreateImageSurface(uWidth, uHeight, fmt, &pd3ds);
    if (SUCCEEDED(hr)) {
        pSurface = new CSurface8();
        if (!pSurface) {
            pd3ds->Release();
            return E_OUTOFMEMORY;
        }

        if (!pSurface->Create(this, pd3ds)) {
            pd3ds->Release();
            return ERR_LOCUSFAIL;
        }

        *ppSurface = pSurface;
    }

    return hr;
}

//******************************************************************************
HRESULT CDevice8::CreateIndexBuffer(UINT uLength, DWORD dwUsage, D3DFORMAT fmt, D3DPOOL pool, CIndexBuffer8** ppIndexBuffer) {

    CIndexBuffer8*         pIndexBuffer;
    LPDIRECT3DINDEXBUFFER8 pd3di;
    HRESULT                hr;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }

    if (!ppIndexBuffer) {
        return D3DERR_INVALIDCALL;
    }
#endif // D3DLOCUS_DEBUG

    *ppIndexBuffer = NULL;

    hr = m_pd3dd->CreateIndexBuffer(uLength, dwUsage, fmt, pool, &pd3di);
    if (SUCCEEDED(hr)) {
        pIndexBuffer = new CIndexBuffer8();
        if (!pIndexBuffer) {
            pd3di->Release();
            return E_OUTOFMEMORY;
        }

        if (!pIndexBuffer->Create(this, pd3di)) {
            pd3di->Release();
            return ERR_LOCUSFAIL;
        }

        *ppIndexBuffer = pIndexBuffer;
    }

    return hr;
}

//******************************************************************************
HRESULT CDevice8::CreateRenderTarget(UINT uWidth, UINT uHeight, D3DFORMAT fmt, D3DMULTISAMPLE_TYPE mst, BOOL bLockable, CSurface8** ppSurface) {

    CSurface8*         pSurface;
    LPDIRECT3DSURFACE8 pd3ds;
    HRESULT            hr;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }

    if (!ppSurface) {
        return D3DERR_INVALIDCALL;
    }
#endif // D3DLOCUS_DEBUG

    *ppSurface = NULL;

    hr = m_pd3dd->CreateRenderTarget(uWidth, uHeight, fmt, mst, bLockable, &pd3ds);
    if (SUCCEEDED(hr)) {
        pSurface = new CSurface8();
        if (!pSurface) {
            pd3ds->Release();
            return E_OUTOFMEMORY;
        }

        if (!pSurface->Create(this, pd3ds)) {
            pd3ds->Release();
            return ERR_LOCUSFAIL;
        }

        *ppSurface = pSurface;
    }

    return hr;
}

//******************************************************************************
HRESULT CDevice8::CreateTexture(UINT uWidth, UINT uHeight, UINT uLevels, DWORD dwUsage, D3DFORMAT fmt, D3DPOOL pool, CTexture8** ppTexture) {

    CTexture8*         pTexture;
    LPDIRECT3DTEXTURE8 pd3dt;
    HRESULT            hr;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }

    if (!ppTexture) {
        return D3DERR_INVALIDCALL;
    }
#endif // D3DLOCUS_DEBUG

    *ppTexture = NULL;

    hr = m_pd3dd->CreateTexture(uWidth, uHeight, uLevels, dwUsage, fmt, pool, &pd3dt);
    if (SUCCEEDED(hr)) {
        pTexture = new CTexture8();
        if (!pTexture) {
            pd3dt->Release();
            return E_OUTOFMEMORY;
        }

        if (!pTexture->Create(this, pd3dt)) {
            pd3dt->Release();
            return ERR_LOCUSFAIL;
        }

        *ppTexture = pTexture;
    }

    return hr;
}

//******************************************************************************
HRESULT CDevice8::CreateVertexBuffer(UINT uLength, DWORD dwUsage, DWORD dwFVF, D3DPOOL pool, CVertexBuffer8** ppVertexBuffer) {

    CVertexBuffer8*         pVertexBuffer;
    LPDIRECT3DVERTEXBUFFER8 pd3dr;
    HRESULT                 hr;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }

    if (!ppVertexBuffer) {
        return D3DERR_INVALIDCALL;
    }
#endif // D3DLOCUS_DEBUG

    *ppVertexBuffer = NULL;

    hr = m_pd3dd->CreateVertexBuffer(uLength, dwUsage, dwFVF, pool, &pd3dr);
    if (SUCCEEDED(hr)) {
        pVertexBuffer = new CVertexBuffer8();
        if (!pVertexBuffer) {
            pd3dr->Release();
            return E_OUTOFMEMORY;
        }

        if (!pVertexBuffer->Create(this, pd3dr)) {
            pd3dr->Release();
            return ERR_LOCUSFAIL;
        }

        *ppVertexBuffer = pVertexBuffer;
    }

    return hr;
}

//******************************************************************************
HRESULT CDevice8::CreateVolumeTexture(UINT uWidth, UINT uHeight, UINT uDepth, UINT uLevels, DWORD dwUsage, D3DFORMAT fmt, D3DPOOL pool, CVolumeTexture8** ppVolumeTexture) {

    CVolumeTexture8*         pVolumeTexture;
    LPDIRECT3DVOLUMETEXTURE8 pd3dtv;
    HRESULT                  hr;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }

    if (!ppVolumeTexture) {
        return D3DERR_INVALIDCALL;
    }
#endif // D3DLOCUS_DEBUG

    *ppVolumeTexture = NULL;

    hr = m_pd3dd->CreateVolumeTexture(uWidth, uHeight, uDepth, uLevels, dwUsage, fmt, pool, &pd3dtv);
    if (SUCCEEDED(hr)) {
        pVolumeTexture = new CVolumeTexture8();
        if (!pVolumeTexture) {
            pd3dtv->Release();
            return E_OUTOFMEMORY;
        }

        if (!pVolumeTexture->Create(this, pd3dtv)) {
            pd3dtv->Release();
            return ERR_LOCUSFAIL;
        }

        *ppVolumeTexture = pVolumeTexture;
    }

    return hr;
}

#ifdef UNDER_XBOX
//******************************************************************************
HRESULT CDevice8::CreatePalette(D3DPALETTESIZE Size, CPalette8** ppPalette) {

    CPalette8*         pPalette;
    LPDIRECT3DPALETTE8 pd3dp;
    HRESULT            hr;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }

    if (!ppPalette) {
        return D3DERR_INVALIDCALL;
    }
#endif // D3DLOCUS_DEBUG

    *ppPalette = NULL;

    hr = m_pd3dd->CreatePalette(Size, &pd3dp);
    if (SUCCEEDED(hr)) {
        pPalette = new CPalette8();
        if (!pPalette) {
            pd3dp->Release();
            return E_OUTOFMEMORY;
        }

        if (!pPalette->Create(this, pd3dp)) {
            pd3dp->Release();
            return ERR_LOCUSFAIL;
        }

        *ppPalette = pPalette;
    }

    return hr;
}
#endif // UNDER_XBOX

//******************************************************************************
HRESULT CDevice8::ApplyStateBlock(DWORD dwToken) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    return m_pd3dd->ApplyStateBlock(dwToken);
}

//******************************************************************************
HRESULT CDevice8::BeginStateBlock() {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    return m_pd3dd->BeginStateBlock();
}

//******************************************************************************
HRESULT CDevice8::EndStateBlock(DWORD* pdwToken) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    return m_pd3dd->EndStateBlock(pdwToken);
}

//******************************************************************************
HRESULT CDevice8::CreateStateBlock(D3DSTATEBLOCKTYPE sbt, DWORD* pdwToken) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    return m_pd3dd->CreateStateBlock(sbt, pdwToken);
}

//******************************************************************************
HRESULT CDevice8::DeleteStateBlock(DWORD dwToken) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    return m_pd3dd->DeleteStateBlock(dwToken);
}

//******************************************************************************
HRESULT CDevice8::CaptureStateBlock(DWORD dwToken) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    return m_pd3dd->CaptureStateBlock(dwToken);
}

#ifndef UNDER_XBOX

//******************************************************************************
HRESULT CDevice8::GetClipStatus(D3DCLIPSTATUS8* pd3dcs) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

#ifndef UNDER_XBOX
    return m_pd3dd->GetClipStatus(pd3dcs);
#else
    Log(LOG_FAIL, TEXT("IDirect3DDevice8::GetClipStatus is not implemented on Xbox"));
    return E_NOTIMPL;
#endif // UNDER_XBOX
}

//******************************************************************************
HRESULT CDevice8::SetClipStatus(D3DCLIPSTATUS8* pd3dcs) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

#ifndef UNDER_XBOX
    return m_pd3dd->SetClipStatus(pd3dcs);
#else
    Log(LOG_FAIL, TEXT("IDirect3DDevice8::SetClipStatus is not implemented on Xbox"));
    return E_NOTIMPL;
#endif // UNDER_XBOX
}

#endif // !UNDER_XBOX

//******************************************************************************
HRESULT CDevice8::GetRenderState(D3DRENDERSTATETYPE rst, DWORD* pdwValue) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    return m_pd3dd->GetRenderState(rst, pdwValue);
}

//******************************************************************************
HRESULT CDevice8::SetRenderState(D3DRENDERSTATETYPE rst, DWORD dwValue) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    return m_pd3dd->SetRenderState(rst, dwValue);
}

//******************************************************************************
HRESULT CDevice8::GetRenderTarget(CSurface8** ppRenderTarget) {

    LPDIRECT3DSURFACE8  pd3ds;
    CSurface8*          pSurface;
    HRESULT             hr;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }

    if (!ppRenderTarget) {
        return D3DERR_INVALIDCALL;
    }
#endif // D3DLOCUS_DEBUG

    *ppRenderTarget = NULL;

    hr = m_pd3dd->GetRenderTarget(&pd3ds);
    if (SUCCEEDED(hr)) {
        
        if (!m_pObjectMap->Find((DWORD)pd3ds, (LPDWORD)&pSurface)) {

            pSurface = new CSurface8();
            if (!pSurface) {
                pd3ds->Release();
                return E_OUTOFMEMORY;
            }

            if (!pSurface->Create(this, pd3ds)) {
                pd3ds->Release();
                return ERR_LOCUSFAIL;
            }
        }

        pSurface->CObject::AddRef();

        *ppRenderTarget = pSurface;
    }

    return hr;
}

//******************************************************************************
HRESULT CDevice8::SetRenderTarget(CSurface8* pRenderTarget, CSurface8* pDepthStencil) {

    LPDIRECT3DSURFACE8 pd3dsRenderTarget = NULL, pd3dsDepthStencil = NULL;
    LPDIRECT3DSURFACE8 pd3dsOldTarget, pd3dsOldDepth;
    CSurface8*         pSurface;
    HRESULT            hr;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    if (pRenderTarget) {
        pd3dsRenderTarget = pRenderTarget->GetIDirect3DSurface8();
        if (FAILED(m_pd3dd->GetRenderTarget(&pd3dsOldTarget))) {
            return ERR_LOCUSFAIL;
        }
        pd3dsOldTarget->Release();
    }
    if (pDepthStencil) {
        pd3dsDepthStencil = pDepthStencil->GetIDirect3DSurface8();
        if (FAILED(m_pd3dd->GetDepthStencilSurface(&pd3dsOldDepth))) {
            pd3dsOldDepth = NULL;
        }
        else {
            pd3dsOldDepth->Release();
        }
    }

    hr = m_pd3dd->SetRenderTarget(pd3dsRenderTarget, pd3dsDepthStencil);

    if (SUCCEEDED(hr)) {

        if (pRenderTarget) {
            pRenderTarget->CObject::AddRef();
            if (m_pObjectMap->Find((DWORD)pd3dsOldTarget, (LPDWORD)&pSurface)) {
                pSurface->CObject::Release();
            }
        }
        if (pDepthStencil) {
            pDepthStencil->CObject::AddRef();
            if (pd3dsOldDepth && m_pObjectMap->Find((DWORD)pd3dsOldDepth, (LPDWORD)&pSurface)) {
                pSurface->CObject::Release();
            }
        }
    }

    return hr;
}

//******************************************************************************
HRESULT CDevice8::GetTransform(D3DTRANSFORMSTATETYPE tst, D3DMATRIX* pm) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    return m_pd3dd->GetTransform(tst, pm);
}

//******************************************************************************
HRESULT CDevice8::SetTransform(D3DTRANSFORMSTATETYPE tst, D3DMATRIX* pm) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    return m_pd3dd->SetTransform(tst, pm);
}

//******************************************************************************
HRESULT CDevice8::MultiplyTransform(D3DTRANSFORMSTATETYPE tst, D3DMATRIX* pm) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    return m_pd3dd->MultiplyTransform(tst, pm);
}

//******************************************************************************
HRESULT CDevice8::Clear(DWORD dwCount, D3DRECT* prc, DWORD dwFlags, D3DCOLOR c, float fZ, DWORD dwStencil) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    return m_pd3dd->Clear(dwCount, prc, dwFlags, c, fZ, dwStencil);
}

//******************************************************************************
HRESULT CDevice8::GetViewport(D3DVIEWPORT8* pviewport) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    return m_pd3dd->GetViewport(pviewport);
}

//******************************************************************************
HRESULT CDevice8::SetViewport(D3DVIEWPORT8* pviewport) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    return m_pd3dd->SetViewport(pviewport);
}

//******************************************************************************
HRESULT CDevice8::GetClipPlane(DWORD dwIndex, float* pfPlane) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

#ifndef UNDER_XBOX
    return m_pd3dd->GetClipPlane(dwIndex, pfPlane);
#else
    Log(LOG_FAIL, TEXT("IDirect3DDevice8::GetClipPlane is not implemented on Xbox"));
    return E_NOTIMPL;
#endif // UNDER_XBOX
}

//******************************************************************************
HRESULT CDevice8::SetClipPlane(DWORD dwIndex, float* pfPlane) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

#ifndef UNDER_XBOX
    return m_pd3dd->SetClipPlane(dwIndex, pfPlane);
#else
    Log(LOG_FAIL, TEXT("IDirect3DDevice8::GetClipPlane is not implemented on Xbox"));
    return E_NOTIMPL;
#endif // UNDER_XBOX
}

//******************************************************************************
HRESULT CDevice8::GetLight(DWORD dwIndex, D3DLIGHT8* plight) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    return m_pd3dd->GetLight(dwIndex, plight);
}

//******************************************************************************
HRESULT CDevice8::SetLight(DWORD dwIndex, D3DLIGHT8* plight) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    return m_pd3dd->SetLight(dwIndex, plight);
}

//******************************************************************************
HRESULT CDevice8::LightEnable(DWORD dwIndex, BOOL bEnable) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    return m_pd3dd->LightEnable(dwIndex, bEnable);
}

//******************************************************************************
HRESULT CDevice8::GetLightEnable(DWORD dwIndex, BOOL* pbEnable) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    return m_pd3dd->GetLightEnable(dwIndex, pbEnable);
}

//******************************************************************************
HRESULT CDevice8::GetMaterial(D3DMATERIAL8* pmaterial) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    return m_pd3dd->GetMaterial(pmaterial);
}

//******************************************************************************
HRESULT CDevice8::SetMaterial(D3DMATERIAL8* pmaterial) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    return m_pd3dd->SetMaterial(pmaterial);
}

#ifdef UNDER_XBOX

//******************************************************************************
HRESULT CDevice8::GetBackMaterial(D3DMATERIAL8* pmaterial) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    return m_pd3dd->GetBackMaterial(pmaterial);
}

//******************************************************************************
HRESULT CDevice8::SetBackMaterial(D3DMATERIAL8* pmaterial) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    return m_pd3dd->SetBackMaterial(pmaterial);
}

#endif // UNDER_XBOX

//******************************************************************************
HRESULT CDevice8::GetTexture(DWORD dwStage, CBaseTexture8** ppTexture) {

    CBaseTexture8*         pTexture;
    LPDIRECT3DBASETEXTURE8 pd3dt;
    HRESULT                hr;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }

    if (!ppTexture) {
        return D3DERR_INVALIDCALL;
    }
#endif // D3DLOCUS_DEBUG

    *ppTexture = NULL;

    hr = m_pd3dd->GetTexture(dwStage, &pd3dt);
    if (SUCCEEDED(hr)) {
        if (!m_pObjectMap->Find((DWORD)pd3dt, (LPDWORD)&pTexture)) {
            return ERR_LOCUSFAIL;
        }
        pTexture->CObject::AddRef();
        *ppTexture = pTexture;
    }

    return hr;
}

//******************************************************************************
HRESULT CDevice8::SetTexture(DWORD dwStage, CBaseTexture8* pTexture) {

    LPDIRECT3DBASETEXTURE8 pd3dt = NULL;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG
    
    if (pTexture) {
        pd3dt = pTexture->GetIDirect3DBaseTexture8();
    }
    return m_pd3dd->SetTexture(dwStage, pd3dt);
}

//******************************************************************************
HRESULT CDevice8::GetTextureStageState(DWORD dwStage, D3DTEXTURESTAGESTATETYPE txsst, DWORD* pdwValue) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    return m_pd3dd->GetTextureStageState(dwStage, txsst, pdwValue);
}

//******************************************************************************
HRESULT CDevice8::SetTextureStageState(DWORD dwStage, D3DTEXTURESTAGESTATETYPE txsst, DWORD dwValue) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    return m_pd3dd->SetTextureStageState(dwStage, txsst, dwValue);
}

//******************************************************************************
HRESULT CDevice8::UpdateTexture(CBaseTexture8* pSourceTexture, CBaseTexture8* pDestinationTexture) {

    LPDIRECT3DBASETEXTURE8 pd3dtSrc = NULL, pd3dtDst = NULL;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

#ifndef UNDER_XBOX

    if (pSourceTexture) {
        pd3dtSrc = pSourceTexture->GetIDirect3DBaseTexture8();
    }
    if (pDestinationTexture) {
        pd3dtDst = pDestinationTexture->GetIDirect3DBaseTexture8();
    }

#ifdef D3DLOCUS_DEBUG
    if (!pd3dtSrc || !pd3dtDst) {
        return D3DERR_INVALIDCALL;
    }
#endif // D3DLOCUS_DEBUG

    return m_pd3dd->UpdateTexture(pd3dtSrc, pd3dtDst);

#else
    Log(LOG_FAIL, TEXT("IDirect3DDevice8::UpdateTexture is not implemented on Xbox"));
    return E_NOTIMPL;
#endif // UNDER_XBOX
}

//******************************************************************************
HRESULT CDevice8::ValidateDevice(DWORD* pdwNumPasses) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

#ifndef UNDER_XBOX
    return m_pd3dd->ValidateDevice(pdwNumPasses);
#else
    Log(LOG_FAIL, TEXT("IDirect3DDevice8::ValidateDevice is not implemented on Xbox"));
    return E_NOTIMPL;
#endif // UNDER_XBOX
}

//******************************************************************************
HRESULT CDevice8::GetCurrentTexturePalette(UINT* puPaletteNumber) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    return E_NOTIMPL;
}

//******************************************************************************
HRESULT CDevice8::SetCurrentTexturePalette(UINT uPaletteNumber) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    return E_NOTIMPL;
}

//******************************************************************************
HRESULT CDevice8::GetPaletteEntries(UINT uPaletteNumber, PALETTEENTRY* ppe) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    return E_NOTIMPL;
}

//******************************************************************************
HRESULT CDevice8::SetPaletteEntries(UINT uPaletteNumber, PALETTEENTRY* ppe) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    return E_NOTIMPL;
}

#ifdef UNDER_XBOX

//******************************************************************************
HRESULT CDevice8::GetPalette(DWORD dwStage, CPalette8** ppPalette) {

    CPalette8*         pPalette;
    LPDIRECT3DPALETTE8 pd3dp;
    HRESULT            hr;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }

    if (!ppPalette) {
        return D3DERR_INVALIDCALL;
    }
#endif // D3DLOCUS_DEBUG

    *ppPalette = NULL;

    hr = m_pd3dd->GetPalette(dwStage, &pd3dp);
    if (SUCCEEDED(hr)) {
        if (!m_pObjectMap->Find((DWORD)pd3dp, (LPDWORD)&pPalette)) {
            return ERR_LOCUSFAIL;
        }
        pPalette->CObject::AddRef();
        *ppPalette = pPalette;
    }

    return hr;
}

//******************************************************************************
HRESULT CDevice8::SetPalette(DWORD dwStage, CPalette8* pPalette) {

    LPDIRECT3DPALETTE8 pd3dp = NULL;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG
    
    if (pPalette) {
        pd3dp = pPalette->GetIDirect3DPalette8();
    }
    return m_pd3dd->SetPalette(dwStage, pd3dp);
}

#endif // UNDER_XBOX

//******************************************************************************
HRESULT CDevice8::CreateVertexShader(DWORD* pdwDeclaration, DWORD* pdwFunction, DWORD* pdwHandle, DWORD dwUsage) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    return m_pd3dd->CreateVertexShader(pdwDeclaration, pdwFunction, pdwHandle, dwUsage);
}

//******************************************************************************
HRESULT CDevice8::CreateVertexShaderTok(DWORD* pdwDeclaration, DWORD* pdwFunction, DWORD* pdwHandle, DWORD dwUsage) {

#ifdef UNDER_XBOX
    LPXGBUFFER  pxgbufShader, pxgbufErrors;
#endif // UNDER_XBOX
    DWORD       dwFuncSize;
    LPDWORD     pdw;
    HRESULT     hr;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

#ifndef UNDER_XBOX

    hr = m_pd3dd->CreateVertexShader(pdwDeclaration, pdwFunction, pdwHandle, dwUsage);

#else

    if (!pdwFunction) {
        hr = m_pd3dd->CreateVertexShader(pdwDeclaration, pdwFunction, pdwHandle, dwUsage);
    }
    else {

        for (pdw = pdwFunction, dwFuncSize = 4; *pdw != D3DVS_END(); pdw++, dwFuncSize += 4);

        hr = AssembleShader("<token-stream>", (LPVOID)pdwFunction, dwFuncSize, SASM_INPUT_VERTEXSHADER_TOKENS, NULL, 
                            &pxgbufShader, &pxgbufErrors, NULL, NULL, NULL, NULL);
        if (FAILED(hr)) {
            ResultFailed(hr, TEXT("AssembleShader"));
            if (pxgbufErrors) {
                if (pxgbufErrors->GetBufferPointer()) {
                    DebugString(TEXT("Assembly errors:\n%S"), (LPSTR)pxgbufErrors->GetBufferPointer());
                }
                pxgbufErrors->Release();
            }
            return hr;
        }

        pxgbufErrors->Release();

        hr = m_pd3dd->CreateVertexShader(pdwDeclaration, (LPDWORD)pxgbufShader->GetBufferPointer(), pdwHandle, dwUsage);

        pxgbufShader->Release();
    }

#endif // UNDER_XBOX

    return hr;
}

//******************************************************************************
HRESULT CDevice8::CreateVertexShaderAsm(DWORD* pdwDeclaration, LPVOID pvSourceCode, UINT uCodeLength, DWORD* pdwHandle, DWORD dwUsage) {

#ifndef UNDER_XBOX
    LPD3DXBUFFER pxbufShader, pxbufErrors;
#else
    LPXGBUFFER   pxgbufShader, pxgbufErrors;
#endif // UNDER_XBOX
    HRESULT      hr;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

#ifndef UNDER_XBOX

    hr = D3DXAssembleShader(pvSourceCode, uCodeLength, 0, NULL, &pxbufShader, &pxbufErrors);
    if (FAILED(hr)) {
        ResultFailed(hr, TEXT("D3DXAssembleShader"));
        if (pxbufErrors) {
            if (!pxbufErrors->GetBufferPointer()) {
                __asm int 3;
            }
            DebugString(TEXT("Assembly errors:\n%s"), (LPSTR)pxbufErrors->GetBufferPointer());
            pxbufErrors->Release();
        }
        return hr;
    }

    pxbufErrors->Release();

    hr = m_pd3dd->CreateVertexShader(pdwDeclaration, (LPDWORD)pxbufShader->GetBufferPointer(), pdwHandle, dwUsage);

    pxbufShader->Release();

#else

    hr = AssembleShader("<memory>", pvSourceCode, uCodeLength, 0, NULL, 
                        &pxgbufShader, &pxgbufErrors, NULL, NULL, NULL, NULL);
    if (FAILED(hr)) {
        ResultFailed(hr, TEXT("AssembleShader"));
        if (pxgbufErrors) {
            if (pxgbufErrors->GetBufferPointer()) {
                DebugString(TEXT("Assembly errors:\n%S"), (LPSTR)pxgbufErrors->GetBufferPointer());
            }
            pxgbufErrors->Release();
        }
        return hr;
    }

    pxgbufErrors->Release();

    hr = m_pd3dd->CreateVertexShader(pdwDeclaration, (LPDWORD)pxgbufShader->GetBufferPointer(), pdwHandle, dwUsage);

    pxgbufShader->Release();

#endif // UNDER_XBOX

    return hr;
}

//******************************************************************************
HRESULT CDevice8::DeleteVertexShader(DWORD dwHandle) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    return m_pd3dd->DeleteVertexShader(dwHandle);
}

//******************************************************************************
HRESULT CDevice8::GetVertexShader(DWORD* pdwHandle) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    return m_pd3dd->GetVertexShader(pdwHandle);
}

//******************************************************************************
HRESULT CDevice8::SetVertexShader(DWORD dwHandle) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    return m_pd3dd->SetVertexShader(dwHandle);
}

//******************************************************************************
HRESULT CDevice8::GetVertexShaderConstant(DWORD dwRegister, void* pvConstantData, DWORD dwConstantCount) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    return m_pd3dd->GetVertexShaderConstant(dwRegister, pvConstantData, dwConstantCount);
}

//******************************************************************************
HRESULT CDevice8::SetVertexShaderConstant(DWORD dwRegister, void* pvConstantData, DWORD dwConstantCount) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    return m_pd3dd->SetVertexShaderConstant(dwRegister, pvConstantData, dwConstantCount);
}

//******************************************************************************
HRESULT CDevice8::GetVertexShaderDeclaration(DWORD dwHandle, void* pvData, DWORD* pdwSizeOfData) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    return m_pd3dd->GetVertexShaderDeclaration(dwHandle, pvData, pdwSizeOfData);
}

//******************************************************************************
HRESULT CDevice8::GetVertexShaderFunction(DWORD dwHandle, void* pvData, DWORD* pdwSizeOfData) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    return m_pd3dd->GetVertexShaderFunction(dwHandle, pvData, pdwSizeOfData);
}

#ifdef UNDER_XBOX

//******************************************************************************
HRESULT CDevice8::SetShaderConstantMode(D3DSHADERCONSTANTMODE Mode) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    return m_pd3dd->SetShaderConstantMode(Mode);
}

//******************************************************************************
HRESULT CDevice8::GetShaderConstantMode(D3DSHADERCONSTANTMODE *pMode) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    return m_pd3dd->GetShaderConstantMode(pMode);
}

//******************************************************************************
HRESULT CDevice8::LoadVertexShader(DWORD dwHandle, DWORD dwAddress) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    return m_pd3dd->LoadVertexShader(dwHandle, dwAddress);
}

//******************************************************************************
HRESULT CDevice8::SelectVertexShader(DWORD dwHandle, DWORD dwAddress) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    return m_pd3dd->SelectVertexShader(dwHandle, dwAddress);
}

//******************************************************************************
HRESULT CDevice8::RunVertexStateShader(DWORD dwAddress, CONST float* pfData) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    return m_pd3dd->RunVertexStateShader(dwAddress, pfData);
}

//******************************************************************************
HRESULT CDevice8::GetVertexShaderSize(DWORD dwHandle, UINT* puSize) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    return m_pd3dd->GetVertexShaderSize(dwHandle, puSize);
}

//******************************************************************************
HRESULT CDevice8::GetVertexShaderType(DWORD dwHandle, DWORD* pdwType) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    return m_pd3dd->GetVertexShaderType(dwHandle, pdwType);
}

#endif // UNDER_XBOX

//******************************************************************************
#ifndef UNDER_XBOX
HRESULT CDevice8::CreatePixelShader(DWORD* pdwFunction, DWORD* pdwHandle) {
#else
HRESULT CDevice8::CreatePixelShader(const D3DPIXELSHADERDEF* pd3dpsdFunction, DWORD* pdwHandle) {
#endif // UNDER_XBOX

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

#ifndef UNDER_XBOX
    return m_pd3dd->CreatePixelShader(pdwFunction, pdwHandle);
#else
    return m_pd3dd->CreatePixelShader(pd3dpsdFunction, pdwHandle);
#endif // UNDER_XBOX
}

//******************************************************************************
HRESULT CDevice8::CreatePixelShaderTok(DWORD* pdwFunction, DWORD* pdwHandle) {

#ifdef UNDER_XBOX
    LPXGBUFFER  pxgbufShader, pxgbufErrors;
#endif // UNDER_XBOX
    DWORD       dwFuncSize;
    LPDWORD     pdw;
    HRESULT     hr;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

#ifndef UNDER_XBOX

    hr = m_pd3dd->CreatePixelShader(pdwFunction, pdwHandle);

#else

    for (pdw = pdwFunction, dwFuncSize = 4; *pdw != D3DPS_END(); pdw++, dwFuncSize += 4);

    hr = AssembleShader("<token-stream>", (LPVOID)pdwFunction, dwFuncSize, SASM_INPUT_PIXELSHADER_TOKENS, NULL, 
                        &pxgbufShader, &pxgbufErrors, NULL, NULL, NULL, NULL);
    if (FAILED(hr)) {
        ResultFailed(hr, TEXT("AssembleShader"));
        if (pxgbufErrors) {
            if (pxgbufErrors->GetBufferPointer()) {
                DebugString(TEXT("Assembly errors:\n%S"), (LPSTR)pxgbufErrors->GetBufferPointer());
            }
            pxgbufErrors->Release();
        }
        return hr;
    }

    pxgbufErrors->Release();

    hr = m_pd3dd->CreatePixelShader((D3DPIXELSHADERDEF*)pxgbufShader->GetBufferPointer(), pdwHandle);

    pxgbufShader->Release();

#endif // UNDER_XBOX

    return hr;
}

//******************************************************************************
HRESULT CDevice8::CreatePixelShaderAsm(LPVOID pvSourceCode, UINT uCodeLength, DWORD* pdwHandle) {

#ifndef UNDER_XBOX
    LPD3DXBUFFER pxbufShader, pxbufErrors;
#else
    LPXGBUFFER   pxgbufShader, pxgbufErrors;
#endif // UNDER_XBOX
    HRESULT      hr;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

#ifndef UNDER_XBOX

    hr = D3DXAssembleShader(pvSourceCode, uCodeLength, 0, NULL, &pxbufShader, &pxbufErrors);
    if (FAILED(hr)) {
        ResultFailed(hr, TEXT("D3DXAssembleShader"));
        DebugString(TEXT("Assembly errors:\n%s"), (LPSTR)pxbufErrors->GetBufferPointer());
        pxbufErrors->Release();
        return hr;
    }

    pxbufErrors->Release();

    hr = m_pd3dd->CreatePixelShader((LPDWORD)pxbufShader->GetBufferPointer(), pdwHandle);

    pxbufShader->Release();

#else

    hr = AssembleShader("<memory>", pvSourceCode, uCodeLength, 0, NULL, 
                        &pxgbufShader, &pxgbufErrors, NULL, NULL, NULL, NULL);
    if (FAILED(hr)) {
        ResultFailed(hr, TEXT("AssembleShader"));
        if (pxgbufErrors) {
            if (pxgbufErrors->GetBufferPointer()) {
                DebugString(TEXT("Assembly errors:\n%S"), (LPSTR)pxgbufErrors->GetBufferPointer());
            }
            pxgbufErrors->Release();
        }
        return hr;
    }

    pxgbufErrors->Release();

    hr = m_pd3dd->CreatePixelShader((D3DPIXELSHADERDEF*)pxgbufShader->GetBufferPointer(), pdwHandle);

    pxgbufShader->Release();

#endif // UNDER_XBOX

    return hr;
}

//******************************************************************************
HRESULT CDevice8::DeletePixelShader(DWORD dwHandle) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    return m_pd3dd->DeletePixelShader(dwHandle);
}

//******************************************************************************
HRESULT CDevice8::GetPixelShader(DWORD* pdwHandle) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    return m_pd3dd->GetPixelShader(pdwHandle);
}

//******************************************************************************
HRESULT CDevice8::SetPixelShader(DWORD dwHandle) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    return m_pd3dd->SetPixelShader(dwHandle);
}

//******************************************************************************
HRESULT CDevice8::GetPixelShaderConstant(DWORD dwRegister, void* pvConstantData, DWORD dwConstantCount) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    return m_pd3dd->GetPixelShaderConstant(dwRegister, pvConstantData, dwConstantCount);
}

//******************************************************************************
HRESULT CDevice8::SetPixelShaderConstant(DWORD dwRegister, void* pvConstantData, DWORD dwConstantCount) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    return m_pd3dd->SetPixelShaderConstant(dwRegister, pvConstantData, dwConstantCount);
}

//******************************************************************************
#ifndef UNDER_XBOX
HRESULT CDevice8::GetPixelShaderFunction(DWORD dwHandle, void* pvData, DWORD* pdwSizeOfData) {
#else
HRESULT CDevice8::GetPixelShaderFunction(DWORD dwHandle, D3DPIXELSHADERDEF* pd3dpsd) {
#endif // UNDER_XBOX

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

#ifndef UNDER_XBOX
    return m_pd3dd->GetPixelShaderFunction(dwHandle, pvData, pdwSizeOfData);
#else
    return m_pd3dd->GetPixelShaderFunction(dwHandle, pd3dpsd);
#endif // UNDER_XBOX
}

/*
#ifdef UNDER_XBOX
//******************************************************************************
HRESULT CDevice8::GetPixelShaderFunction(DWORD dwHandle, D3DPIXELSHADERDEF* pd3dpsd, void* pvData, DWORD* pdwSizeOfData) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    if (pdwSizeOfData) {
        *pdwSizeOfData = 0;
    }
    return m_pd3dd->GetPixelShaderFunction(dwHandle, pd3dpsd);
}
#endif // UNDER_XBOX
*/

//******************************************************************************
#ifndef UNDER_XBOX
HRESULT CDevice8::Present(RECT* prectSrc, RECT* prectDst, HWND hDestWindowOverride, RGNDATA* prgnDirtyRegion)
#else
HRESULT CDevice8::Present(RECT* prectSrc, RECT* prectDst, void* hDestWindowOverride, void* prgnDirtyRegion)
#endif // UNDER_XBOX
{
#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    return m_pd3dd->Present(prectSrc, prectDst, hDestWindowOverride, prgnDirtyRegion);
}

//******************************************************************************
HRESULT CDevice8::Reset(D3DPRESENT_PARAMETERS* pd3dpp) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    return m_pd3dd->Reset(pd3dpp);
}

//******************************************************************************
HRESULT CDevice8::BeginScene() {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    return m_pd3dd->BeginScene();
}

//******************************************************************************
HRESULT CDevice8::EndScene() {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

// ##DELETE
// ##HACK to make diffuse modulated blending work on Inspiron 5000
#ifndef UNDER_XBOX
    typedef struct _DUMMYVERTEX {
        D3DXVECTOR3 vPosition;
        float       fRHW;
        D3DCOLOR    cDiffuse;
        D3DCOLOR    cSpecular;
        float       u0, v0;

        _DUMMYVERTEX() {}
        _DUMMYVERTEX(const D3DVECTOR& v, float _fRHW, D3DCOLOR _cDiffuse, 
                  D3DCOLOR _cSpecular, float _u0, float _v0) 
        { 
            vPosition.x = v.x; vPosition.y = v.y; vPosition.z = v.z; fRHW = _fRHW;
            cDiffuse = _cDiffuse; cSpecular = _cSpecular;
            u0 = _u0; v0 = _v0;
        }
    } DUMMYVERTEX;
    DUMMYVERTEX rDummy[3];
    rDummy[0] = DUMMYVERTEX(D3DXVECTOR3(0.0f, (float)0.0f, 0.000009f), 110000.0f, 0xFF000000, 0, 0.0f, 1.0f);
    rDummy[1] = DUMMYVERTEX(D3DXVECTOR3(0.0f, 0.0f, 0.000009f), 110000.0f, 0xFF000000, 0, 0.0f, 0.0f);
    rDummy[2] = DUMMYVERTEX(D3DXVECTOR3((float)0.0f, 0.0f, 0.000009f), 110000.0f, 0xFF000000, 0, 1.0f, 0.0f);
    m_pd3dd->SetVertexShader(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX1);
    m_pd3dd->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG2);
    m_pd3dd->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
    m_pd3dd->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2);
    m_pd3dd->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
    m_pd3dd->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 1, rDummy, sizeof(DUMMYVERTEX));
    m_pd3dd->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
    m_pd3dd->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
    m_pd3dd->SetVertexShader(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1);
#endif // !UNDER_XBOX
// ##END HACK
// ##END DELETE

    return m_pd3dd->EndScene();
}

//******************************************************************************
HRESULT CDevice8::DrawIndexedPrimitive(D3DPRIMITIVETYPE d3dpt, UINT uMinVertexToShade, UINT uNumVerticesToShade, UINT uStartIndex, UINT uPrimitiveCount) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    return m_pd3dd->DrawIndexedPrimitive(d3dpt, uMinVertexToShade, uNumVerticesToShade, uStartIndex, uPrimitiveCount);
}

//******************************************************************************
HRESULT CDevice8::DrawIndexedPrimitiveUP(D3DPRIMITIVETYPE d3dpt, UINT uMinVertexToShade, UINT uNumVerticesToShade, UINT uPrimitiveCount, void* pvIndices, D3DFORMAT fmtIndex, void* pvVertices, UINT uStride) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    return m_pd3dd->DrawIndexedPrimitiveUP(d3dpt, uMinVertexToShade, uNumVerticesToShade, uPrimitiveCount, pvIndices, fmtIndex, pvVertices, uStride);
}

//******************************************************************************
HRESULT CDevice8::DrawPrimitive(D3DPRIMITIVETYPE d3dpt, UINT uStartVertex, UINT uPrimitiveCount) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    return m_pd3dd->DrawPrimitive(d3dpt, uStartVertex, uPrimitiveCount);
}

//******************************************************************************
HRESULT CDevice8::DrawPrimitiveUP(D3DPRIMITIVETYPE d3dpt, UINT uPrimitiveCount, void* pvVertices, UINT uStride) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    return m_pd3dd->DrawPrimitiveUP(d3dpt, uPrimitiveCount, pvVertices, uStride);
}

#ifdef UNDER_XBOX

//******************************************************************************
HRESULT CDevice8::DrawVertices(D3DPRIMITIVETYPE d3dpt, UINT uStartVertex, UINT uVertexCount) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    return m_pd3dd->DrawVertices(d3dpt, uStartVertex, uVertexCount);
}

//******************************************************************************
HRESULT CDevice8::DrawIndexedVertices(D3DPRIMITIVETYPE d3dpt, UINT uStartIndex, CONST WORD* pwIndices) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    return m_pd3dd->DrawIndexedVertices(d3dpt, uStartIndex, pwIndices);
}

//******************************************************************************
HRESULT CDevice8::DrawVerticesUP(D3DPRIMITIVETYPE d3dpt, UINT uVertexCount, CONST void* pvVertices, UINT uStride) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    return m_pd3dd->DrawVerticesUP(d3dpt, uVertexCount, pvVertices, uStride);
}

//******************************************************************************
HRESULT CDevice8::DrawIndexedVerticesUP(D3DPRIMITIVETYPE d3dpt, UINT uVertexCount, CONST void* pvIndices, CONST void* pvVertices, UINT uStride) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    return m_pd3dd->DrawIndexedVerticesUP(d3dpt, uVertexCount, pvIndices, pvVertices, uStride);
}

//******************************************************************************
HRESULT CDevice8::PrimeVertexCache(UINT VertexCount, CONST WORD *pIndexData) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    return m_pd3dd->PrimeVertexCache(VertexCount, pIndexData);
}

//******************************************************************************
HRESULT CDevice8::Begin(D3DPRIMITIVETYPE d3dpt) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    return m_pd3dd->Begin(d3dpt);
}

//******************************************************************************
HRESULT CDevice8::End() {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    return m_pd3dd->End();
}

//******************************************************************************
void CDevice8::KickPushBuffer() {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return;
    }
#endif // D3DLOCUS_DEBUG

    m_pd3dd->KickPushBuffer();
}

#endif // UNDER_XBOX

//******************************************************************************
HRESULT CDevice8::DeletePatch(UINT uHandle) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    return m_pd3dd->DeletePatch(uHandle);
}

//******************************************************************************
HRESULT CDevice8::DrawRectPatch(UINT uHandle, float* pfNumSegs, D3DRECTPATCH_INFO* prpi) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    return m_pd3dd->DrawRectPatch(uHandle, pfNumSegs, prpi);
}

//******************************************************************************
HRESULT CDevice8::DrawTriPatch(UINT uHandle, float* pfNumSegs, D3DTRIPATCH_INFO* ptsi) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    return m_pd3dd->DrawTriPatch(uHandle, pfNumSegs, ptsi);
}

//******************************************************************************
HRESULT CDevice8::GetStreamSource(UINT uStream, CVertexBuffer8** ppVertexData, UINT* puStride) {

    CVertexBuffer8*         pVertexBuffer;
    LPDIRECT3DVERTEXBUFFER8 pd3dr;
    HRESULT                 hr;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }

    if (!ppVertexData) {
        return D3DERR_INVALIDCALL;
    }
#endif // D3DLOCUS_DEBUG

    *ppVertexData = NULL;

    hr = m_pd3dd->GetStreamSource(uStream, &pd3dr, puStride);
    if (SUCCEEDED(hr)) {
        if (!m_pObjectMap->Find((DWORD)pd3dr, (LPDWORD)&pVertexBuffer)) {
            return ERR_LOCUSFAIL;
        }
        pVertexBuffer->CObject::AddRef();
        *ppVertexData = pVertexBuffer;
    }

    return hr;
}

//******************************************************************************
HRESULT CDevice8::SetStreamSource(UINT uStream, CVertexBuffer8* pVertexData, UINT uStride) {

    LPDIRECT3DVERTEXBUFFER8 pd3dr = NULL;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    if (pVertexData) {
        pd3dr = pVertexData->GetIDirect3DVertexBuffer8();
    }
    return m_pd3dd->SetStreamSource(uStream, pd3dr, uStride);
}

//******************************************************************************
HRESULT CDevice8::GetIndices(CIndexBuffer8** ppIndexData, UINT* puBaseIndex) {

    CIndexBuffer8*         pIndexBuffer;
    LPDIRECT3DINDEXBUFFER8 pd3di;
    HRESULT                hr;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }

    if (!ppIndexData) {
        return D3DERR_INVALIDCALL;
    }
#endif // D3DLOCUS_DEBUG

    *ppIndexData = NULL;

    hr = m_pd3dd->GetIndices(&pd3di, puBaseIndex);
    if (SUCCEEDED(hr)) {
        if (!m_pObjectMap->Find((DWORD)pd3di, (LPDWORD)&pIndexBuffer)) {
            return ERR_LOCUSFAIL;
        }
        pIndexBuffer->CObject::AddRef();
        *ppIndexData = pIndexBuffer;
    }

    return hr;
}

//******************************************************************************
HRESULT CDevice8::SetIndices(CIndexBuffer8* pIndexData, UINT uBaseIndex) {

    LPDIRECT3DINDEXBUFFER8 pd3di = NULL;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    if (pIndexData) {
        pd3di = pIndexData->GetIDirect3DIndexBuffer8();
    }
    return m_pd3dd->SetIndices(pd3di, uBaseIndex);
}

//******************************************************************************
UINT CDevice8::GetAvailableTextureMem() {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return 0;
    }
#endif // D3DLOCUS_DEBUG

#ifndef UNDER_XBOX
    return m_pd3dd->GetAvailableTextureMem();
#else
    Log(LOG_FAIL, TEXT("IDirect3DDevice8::GetAvailableTextureMem is not implemented on XBox"));
    return 0;
#endif // UNDER_XBOX
}

//******************************************************************************
HRESULT CDevice8::GetCreationParameters(D3DDEVICE_CREATION_PARAMETERS* pd3dcp) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

#ifndef DEMO_HACK
    return m_pd3dd->GetCreationParameters(pd3dcp);
#else
    return E_NOTIMPL;
#endif // DEMO_HACK
}

//******************************************************************************
HRESULT CDevice8::GetDeviceCaps(D3DCAPS8* pd3dcaps) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    return m_pd3dd->GetDeviceCaps(pd3dcaps);
}

//******************************************************************************
HRESULT CDevice8::GetDirect3D(CDirect3D8** ppDirect3D8) {

    LPDIRECT3D8 pd3d;
    HRESULT     hr;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
    if (!ppDirect3D8) {
        return D3DERR_INVALIDCALL;
    }
#endif // D3DLOCUS_DEBUG

    *ppDirect3D8 = NULL;

    hr = m_pd3dd->GetDirect3D(&pd3d);
    if (SUCCEEDED(hr)) {
        if (pd3d != m_pDirect3D->GetIDirect3D8()) {
            DebugString(TEXT("Interface pointers differ for IDirect3D8"));
            DebugBreak();
        }
        m_pDirect3D->CObject::AddRef();
        *ppDirect3D8 = m_pDirect3D;
    }

    return hr;
}

//******************************************************************************
HRESULT CDevice8::GetInfo(DWORD dwDevInfoType, void* pvDevInfo, DWORD dwDevInfoSize) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

#ifndef UNDER_XBOX
    return m_pd3dd->GetInfo(dwDevInfoType, pvDevInfo, dwDevInfoSize);
#else
    Log(LOG_FAIL, TEXT("IDirect3DDevice8::GetInfo is not implemented on Xbox"));
    return E_NOTIMPL;
#endif // UNDER_XBOX
}

//******************************************************************************
HRESULT CDevice8::GetRasterStatus(D3DRASTER_STATUS* prast) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    return m_pd3dd->GetRasterStatus(prast);
}

//******************************************************************************
HRESULT CDevice8::GetDisplayMode(D3DDISPLAYMODE* pMode) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    return m_pd3dd->GetDisplayMode(pMode);
}

//******************************************************************************
HRESULT CDevice8::GetBackBuffer(UINT uBackBuffer, D3DBACKBUFFER_TYPE bbt, CSurface8** ppBackBuffer) {

    LPDIRECT3DSURFACE8  pd3ds;
    CSurface8*          pSurface;
    HRESULT             hr;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }

    if (!ppBackBuffer) {
        return D3DERR_INVALIDCALL;
    }
#endif // D3DLOCUS_DEBUG

    *ppBackBuffer = NULL;

    hr = m_pd3dd->GetBackBuffer(uBackBuffer, bbt, &pd3ds);
    if (SUCCEEDED(hr)) {
        
        if (!m_pObjectMap->Find((DWORD)pd3ds, (LPDWORD)&pSurface)) {

            pSurface = new CSurface8();
            if (!pSurface) {
                pd3ds->Release();
                return E_OUTOFMEMORY;
            }

            if (!pSurface->Create(this, pd3ds)) {
                pd3ds->Release();
                return ERR_LOCUSFAIL;
            }
        }

        pSurface->CObject::AddRef();

        *ppBackBuffer = pSurface;
    }

    return hr;
}

//******************************************************************************
HRESULT CDevice8::GetDepthStencilSurface(CSurface8** ppZStencilSurface) {

    LPDIRECT3DSURFACE8  pd3ds;
    CSurface8*          pSurface;
    HRESULT             hr;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }

    if (!ppZStencilSurface) {
        return D3DERR_INVALIDCALL;
    }
#endif // D3DLOCUS_DEBUG

    *ppZStencilSurface = NULL;

    hr = m_pd3dd->GetDepthStencilSurface(&pd3ds);
    if (SUCCEEDED(hr)) {
        
        if (!m_pObjectMap->Find((DWORD)pd3ds, (LPDWORD)&pSurface)) {

            pSurface = new CSurface8();
            if (!pSurface) {
                pd3ds->Release();
                return E_OUTOFMEMORY;
            }

            if (!pSurface->Create(this, pd3ds)) {
                pd3ds->Release();
                return ERR_LOCUSFAIL;
            }
        }

        pSurface->CObject::AddRef();

        *ppZStencilSurface = pSurface;
    }

    return hr;
}

//******************************************************************************
void CDevice8::GetGammaRamp(D3DGAMMARAMP* pgrRamp) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return;
    }
#endif // D3DLOCUS_DEBUG

    m_pd3dd->GetGammaRamp(pgrRamp);
}

//******************************************************************************
void CDevice8::SetGammaRamp(DWORD dwFlags, D3DGAMMARAMP* pgrRamp) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return;
    }
#endif // D3DLOCUS_DEBUG

    m_pd3dd->SetGammaRamp(dwFlags, pgrRamp);
}

//******************************************************************************
HRESULT CDevice8::CopyRects(CSurface8* pSrcSurface, RECT* prectSrcRects, UINT uNumSrcRects, CSurface8* pDstSurface, POINT* ppntDstPoints) {

#ifdef D3DLOCUS_DEBUG
    if (!pSrcSurface || !pDstSurface) {
        return D3DERR_INVALIDCALL;
    }
    if (!m_pd3dd || !pSrcSurface->GetIDirect3DSurface8() || !pDstSurface->GetIDirect3DSurface8()) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    return m_pd3dd->CopyRects(pSrcSurface->GetIDirect3DSurface8(), prectSrcRects, uNumSrcRects, pDstSurface->GetIDirect3DSurface8(), ppntDstPoints);
}

//******************************************************************************
HRESULT CDevice8::GetFrontBuffer(CSurface8* pDstSurface) {

#ifdef D3DLOCUS_DEBUG
    if (!pDstSurface) {
        return D3DERR_INVALIDCALL;
    }
    if (!m_pd3dd || !pDstSurface->GetIDirect3DSurface8()) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

#ifndef UNDER_XBOX
    return m_pd3dd->GetFrontBuffer(pDstSurface->GetIDirect3DSurface8());
#else
    Log(LOG_FAIL, TEXT("IDirect3DDevice8::GetFrontBuffer is not implemented on Xbox"));
    return E_NOTIMPL;
#endif // UNDER_XBOX
}

//******************************************************************************
HRESULT CDevice8::ProcessVertices(UINT uStartVertexSrc, UINT uStartVertexDst, UINT uNumVertices, CVertexBuffer8* pDstBuffer, DWORD dwFlags) {

#ifdef D3DLOCUS_DEBUG
    if (!pDstBuffer) {
        return D3DERR_INVALIDCALL;
    }
    if (!m_pd3dd || !pDstBuffer->GetIDirect3DVertexBuffer8()) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

#ifndef UNDER_XBOX
    return m_pd3dd->ProcessVertices(uStartVertexSrc, uStartVertexDst, uNumVertices, pDstBuffer->GetIDirect3DVertexBuffer8(), dwFlags);
#else
    Log(LOG_FAIL, TEXT("IDirect3DDevice8::ProcessVertices is not implemented on Xbox"));
    return E_NOTIMPL;
#endif // UNDER_XBOX
}

//******************************************************************************
HRESULT CDevice8::ResourceManagerDiscardBytes(DWORD dwNumBytes) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

#ifndef UNDER_XBOX
    return m_pd3dd->ResourceManagerDiscardBytes(dwNumBytes);
#else
    Log(LOG_FAIL, TEXT("IDirect3DDevice8::ResourceManagerDiscardBytes is not implemented on XBox"));
    return E_NOTIMPL;
#endif // UNDER_XBOX
}

//******************************************************************************
HRESULT CDevice8::TestCooperativeLevel() {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

#ifndef UNDER_XBOX
    return m_pd3dd->TestCooperativeLevel();
#else
    return D3D_OK;
#endif // UNDER_XBOX
}

#ifdef UNDER_XBOX

//******************************************************************************
HRESULT CDevice8::PersistDisplay() {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    return m_pd3dd->PersistDisplay();
}

//******************************************************************************
HRESULT CDevice8::SetTile(DWORD dwIndex, D3DTILE* pTile) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    return m_pd3dd->SetTile(dwIndex, pTile);
}

//******************************************************************************
HRESULT CDevice8::GetTile(DWORD dwIndex, D3DTILE* pTile) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    return m_pd3dd->GetTile(dwIndex, pTile);
}

#endif // UNDER_XBOX

//******************************************************************************
void CDevice8::SetCursorPosition(UINT uSX, UINT uSY, DWORD dwFlags) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return;
    }
#endif // D3DLOCUS_DEBUG

#ifndef UNDER_XBOX
    m_pd3dd->SetCursorPosition(uSX, uSY, dwFlags);
#else
    Log(LOG_FAIL, TEXT("IDirect3DDevice8::SetCursorPosition is not implemented on Xbox"));
#endif // UNDER_XBOX
}

//******************************************************************************
HRESULT CDevice8::SetCursorProperties(UINT uHotSpotX, UINT uHotSpotY, CSurface8* pCursorBitmap) {

    LPDIRECT3DSURFACE8 pd3ds = NULL;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

#ifndef UNDER_XBOX
    if (pCursorBitmap) {
        pd3ds = pCursorBitmap->GetIDirect3DSurface8();
    }
    return m_pd3dd->SetCursorProperties(uHotSpotX, uHotSpotY, pd3ds);
#else
    Log(LOG_FAIL, TEXT("IDirect3DDevice8::SetCursorProperties is not implemented on Xbox"));
    return E_NOTIMPL;
#endif // UNDER_XBOX
}

//******************************************************************************
BOOL CDevice8::ShowCursor(BOOL bShow) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return FALSE;
    }
#endif // D3DLOCUS_DEBUG

#ifndef UNDER_XBOX
    return m_pd3dd->ShowCursor(bShow);
#else
    Log(LOG_FAIL, TEXT("IDirect3DDevice8::ShowCursor is not implemented on Xbox"));
    return E_NOTIMPL;
#endif // UNDER_XBOX
}

#ifdef UNDER_XBOX

//******************************************************************************
HRESULT CDevice8::BeginVisibilityTest() {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    return m_pd3dd->BeginVisibilityTest();
}

//******************************************************************************
HRESULT CDevice8::EndVisibilityTest(DWORD dwIndex) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    return m_pd3dd->EndVisibilityTest(dwIndex);
}

//******************************************************************************
HRESULT CDevice8::GetVisibilityTestResult(DWORD dwIndex, UINT* puResult, ULONGLONG* puuTimeStamp) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    return m_pd3dd->GetVisibilityTestResult(dwIndex, puResult, puuTimeStamp);
}

//******************************************************************************
HRESULT CDevice8::SetVertexData2f(int nRegister, float a, float b) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    return m_pd3dd->SetVertexData2f(nRegister, a, b);
}

//******************************************************************************
HRESULT CDevice8::SetVertexData4f(int nRegister, float a, float b, float c, float d) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    return m_pd3dd->SetVertexData4f(nRegister, a, b, c, d);
}

//******************************************************************************
HRESULT CDevice8::SetVertexData2s(int nRegister, short a, short b) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    return m_pd3dd->SetVertexData2s(nRegister, a, b);
}

//******************************************************************************
HRESULT CDevice8::SetVertexData4s(int nRegister, short a, short b, short c, short d) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    return m_pd3dd->SetVertexData4s(nRegister, a, b, c, d);
}

//******************************************************************************
HRESULT CDevice8::SetVertexData4ub(int nRegister, BYTE a, BYTE b, BYTE c, BYTE d) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    return m_pd3dd->SetVertexData4ub(nRegister, a, b, c, d);
}

//******************************************************************************
BOOL CDevice8::IsBusy() {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return FALSE;
    }
#endif // D3DLOCUS_DEBUG

    return m_pd3dd->IsBusy();
}

//******************************************************************************
void CDevice8::BlockUntilIdle() {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return;
    }
#endif // D3DLOCUS_DEBUG

    m_pd3dd->BlockUntilIdle();
}

//******************************************************************************
#ifndef UNDER_XBOX
void CDevice8::SetVerticalBlankCallback(D3DCALLBACK pCallback) {
#else
void CDevice8::SetVerticalBlankCallback(D3DVBLANKCALLBACK pCallback) {
#endif

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return;
    }
#endif // D3DLOCUS_DEBUG

    m_pd3dd->SetVerticalBlankCallback(pCallback);
}

//******************************************************************************
void CDevice8::BlockUntilVerticalBlank() {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return;
    }
#endif // D3DLOCUS_DEBUG

    m_pd3dd->BlockUntilVerticalBlank();
}

//******************************************************************************
DWORD CDevice8::InsertFence() {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return -1;
    }
#endif // D3DLOCUS_DEBUG

    return m_pd3dd->InsertFence();
}

//******************************************************************************
BOOL CDevice8::IsFencePending(DWORD dwFence) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return FALSE;
    }
#endif // D3DLOCUS_DEBUG

    return m_pd3dd->IsFencePending(dwFence);
}

//******************************************************************************
void CDevice8::BlockOnFence(DWORD dwFence) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return;
    }
#endif // D3DLOCUS_DEBUG

    m_pd3dd->BlockOnFence(dwFence);
}

//******************************************************************************
void CDevice8::InsertCallback(D3DCALLBACKTYPE Type, D3DCALLBACK pCallback, DWORD dwContext) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return;
    }
#endif // D3DLOCUS_DEBUG

    m_pd3dd->InsertCallback(Type, pCallback, dwContext);
}

//******************************************************************************
HRESULT CDevice8::BeginPushBuffer(void* pvBuffer, UINT uSize) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    return S_OK; // !!! m_pd3dd->BeginPushBuffer(pvBuffer, uSize);
}

//******************************************************************************
HRESULT CDevice8::EndPushBuffer(D3DPushBuffer* pPushBuffer) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    return S_OK; // !!! m_pd3dd->EndPushBuffer(pPushBuffer);
}

//******************************************************************************
HRESULT CDevice8::RunPushBuffer(D3DPushBuffer* pPushBuffer, DWORD* pdwFixUps) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    return S_OK; // !!! m_pd3dd->RunPushBuffer(pPushBuffer, pdwFixUps);
}

//******************************************************************************
HRESULT CDevice8::GetPushBufferOffset(DWORD* pdwOffset) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    return m_pd3dd->GetPushBufferOffset(pdwOffset);
}

//******************************************************************************
HRESULT CDevice8::GetProjectionViewportMatrix(D3DMATRIX* pmMatrix) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    return m_pd3dd->GetProjectionViewportMatrix(pmMatrix);
}

//******************************************************************************
HRESULT CDevice8::SetModelView(CONST D3DMATRIX* pmModelView, CONST D3DMATRIX* pmInverseModelView, CONST D3DMATRIX* pmComposite) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    return m_pd3dd->SetModelView(pmModelView, pmInverseModelView, pmComposite);
}

//******************************************************************************
HRESULT CDevice8::GetModelView(D3DMATRIX* pmModelView) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    return m_pd3dd->GetModelView(pmModelView);
}

//******************************************************************************
HRESULT CDevice8::SetVertexBlendModelView(UINT uCount, CONST D3DMATRIX* pmModelViews, CONST D3DMATRIX* pmInverseModelViews, CONST D3DMATRIX* pProjectionViewport) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    return m_pd3dd->SetVertexBlendModelView(uCount, pmModelViews, pmInverseModelViews, pProjectionViewport);
}

//******************************************************************************
HRESULT CDevice8::GetVertexBlendModelView(UINT uCount, D3DMATRIX* pmModelViews, D3DMATRIX* pProjectionViewport) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    return m_pd3dd->GetVertexBlendModelView(uCount, pmModelViews, pProjectionViewport);
}

//******************************************************************************
HRESULT CDevice8::SetVertexInput(ULONG uCount, CONST DWORD* pdwAddresses, CONST DWORD* pdwFormats) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    return S_OK; // !!! m_pd3dd->SetVertexInput(uCount, pdwAddresses, pdwFormats);
}

//******************************************************************************
HRESULT CDevice8::GetVertexInput(ULONG uCount, DWORD* pdwAddresses, DWORD* pdwFormats) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    return S_OK; // !!! m_pd3dd->GetVertexInput(uCount, pdwAddresses, pdwFormats);
}

//******************************************************************************
void CDevice8::FlushVertexCache() {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return;
    }
#endif // D3DLOCUS_DEBUG

    m_pd3dd->FlushVertexCache();
}

#endif // UNDER_XBOX

//******************************************************************************
// CCDevice8
//******************************************************************************

//******************************************************************************
CCDevice8::CCDevice8() {

    DWORD dwCRC;
    UINT  i, j;

    m_bServerAffinity = FALSE;
    m_bVerifyFrames = TRUE;
    m_fThreshold = 0.9f;
    m_fFrameMatch = 1.0f;
    m_uFramesPresented = 0;
    m_pd3dtFrameSrc = NULL;
    m_pd3dtFrameRef = NULL;
    m_pd3dtFrameVar = NULL;
    m_uShowVerification = 0;

    // Initialize the CRC table
    for (i = 0; i < 256; i++) {
        dwCRC = i;
        for (j = 0; j < 8; j++) {
            dwCRC = (dwCRC & 1) ? ((dwCRC >> 1) ^ 0xEDB88320L) : (dwCRC >> 1);
        }
        m_dwCRCTable[i] = dwCRC;
    }
}

//******************************************************************************
CCDevice8::~CCDevice8() {

    EnableVerificationDisplay(FALSE);
}

//******************************************************************************
BOOL CCDevice8::Create(CClient* pClient, CDirect3D8* pDirect3D, LPDIRECT3DDEVICE8 pd3dd) {

    if (!pClient) {
        return FALSE;
    }

    m_pClient = pClient;

    if (!CDevice8::Create(pDirect3D, pd3dd)) {
        return FALSE;
    }

    if (!pClient->CheckDeviceAffinity(pDirect3D, &m_bServerAffinity)) {
        m_bServerAffinity = FALSE;
    }

    return TRUE;
}

//******************************************************************************
void CCDevice8::EnableFrameVerification(BOOL bEnable) {

    m_bVerifyFrames = bEnable;
}

//******************************************************************************
void CCDevice8::SetVerificationThreshold(float fThreshold) {

    m_fThreshold = fThreshold;
}

//******************************************************************************
void CCDevice8::EnableVerificationDisplay(BOOL bEnable) {

    if (bEnable && m_uShowVerification) {
        return;
    }

    if (bEnable) {

        HRESULT hr;

        hr = CreateTexture(256, 256, 1, 0, D3DFMT_A8R8G8B8, POOL_DEFAULT, &m_pd3dtFrameSrc);
        if (FAILED(hr)) {
            return;
        }

        hr = CreateTexture(256, 256, 1, 0, D3DFMT_A8R8G8B8, POOL_DEFAULT, &m_pd3dtFrameRef);
        if (FAILED(hr)) {
            m_pd3dtFrameSrc->Release();
            m_pd3dtFrameSrc = NULL;
            return;
        }

        hr = CreateTexture(256, 256, 1, 0, D3DFMT_A8R8G8B8, POOL_DEFAULT, &m_pd3dtFrameVar);
        if (FAILED(hr)) {
            m_pd3dtFrameSrc->Release();
            m_pd3dtFrameSrc = NULL;
            m_pd3dtFrameRef->Release();
            m_pd3dtFrameRef = NULL;
            return;
        }
    }
    else {
        if (m_pd3dtFrameSrc) {
            m_pd3dtFrameSrc->Release();
            m_pd3dtFrameSrc = NULL;
        }
        if (m_pd3dtFrameRef) {
            m_pd3dtFrameRef->Release();
            m_pd3dtFrameRef = NULL;
        }
        if (m_pd3dtFrameVar) {
            m_pd3dtFrameVar->Release();
            m_pd3dtFrameVar = NULL;
        }
    }

    m_uShowVerification = bEnable ? 1 : 0;
}

//******************************************************************************
void CCDevice8::ToggleVerificationDisplay() {

    if (m_uShowVerification == 1) {
        m_uShowVerification = 2;
    }
    else {
        EnableVerificationDisplay(!(BOOL)m_uShowVerification);
    }
}

//******************************************************************************
LPDIRECT3DSURFACE8 CCDevice8::GetFrontBufferContents() {

    LPDIRECT3DSURFACE8 pd3ds;
#ifndef UNDER_XBOX
    D3DDISPLAYMODE     d3ddm;
    D3DSURFACE_DESC    d3dsd;
    HRESULT            hr;

    hr = m_pd3dd->GetDisplayMode(&d3ddm);
    if (ResultFailed(hr, TEXT("IDirect3DDevice8::GetDisplayMode"))) {
        return NULL;
    }

    hr = m_pd3dd->CreateImageSurface(d3ddm.Width, d3ddm.Height, D3DFMT_A8R8G8B8, &pd3ds);
    if (FAILED(hr)) {
        return NULL;
    }

    hr = m_pd3dd->GetFrontBuffer(pd3ds);
    if (FAILED(hr)) {
        pd3ds->Release();
        return NULL;
    }

#else
    m_pd3dd->GetBackBuffer(-1, D3DBACKBUFFER_TYPE_MONO, &pd3ds);
#endif // UNDER_XBOX

    return pd3ds;
}

//******************************************************************************
DWORD CCDevice8::ComputeSurfaceCRC32(LPDIRECT3DSURFACE8 pd3ds) {

    D3DSURFACE_DESC d3dsd;
    D3DLOCKED_RECT  d3dlr;
    DWORD           dwCRC = 0;
    LPBYTE          pbData;
    UINT            i, j, k;
    HRESULT         hr;

    hr = pd3ds->GetDesc(&d3dsd);
    if (FAILED(hr)) {
        return 0;
    }

#ifndef UNDER_XBOX
    hr = pd3ds->LockRect(&d3dlr, NULL, 0);
#else
    hr = pd3ds->LockRect(&d3dlr, NULL, D3DLOCK_TILED);
#endif
    if (FAILED(hr)) {
        return 0;
    }

    switch (d3dsd.Format) {

#ifdef UNDER_XBOX
        case D3DFMT_LIN_R5G6B5:
#endif
        case D3DFMT_R5G6B5: {
            DWORD  dwPixel;
            LPWORD pwPixel = (LPWORD)d3dlr.pBits;
            for (i = 0; i < d3dsd.Height; i++) {
                for (j = 0; j < d3dsd.Width; j++) {
                    dwPixel = (pwPixel[j] & 0xF800) << 8 |
                              (pwPixel[j] & 0x07E0) << 5 |
                              (pwPixel[j] & 0x001F) << 3;
                    pbData = (LPBYTE)&dwPixel;
                    for (k = 0; k < 3; k++) {
                        dwCRC = m_dwCRCTable[(dwCRC ^ pbData[k]) & 0xFF] ^ (dwCRC >> 8);
                    }
                }
                pwPixel += d3dlr.Pitch >> 1;
            }
            break;
        }

#ifdef UNDER_XBOX
        case D3DFMT_LIN_X1R5G5B5:
        case D3DFMT_LIN_A1R5G5B5:
#endif
        case D3DFMT_X1R5G5B5:
        case D3DFMT_A1R5G5B5: {
            DWORD  dwPixel;
            LPWORD pwPixel = (LPWORD)d3dlr.pBits;
            for (i = 0; i < d3dsd.Height; i++) {
                for (j = 0; j < d3dsd.Width; j++) {
                    dwPixel = (pwPixel[j] & 0x7C00) << 9 |
                              (pwPixel[j] & 0x03E0) << 6 |
                              (pwPixel[j] & 0x001F) << 3;
                    pbData = (LPBYTE)&dwPixel;
                    for (k = 0; k < 3; k++) {
                        dwCRC = m_dwCRCTable[(dwCRC ^ pbData[k]) & 0xFF] ^ (dwCRC >> 8);
                    }
                }
                pwPixel += d3dlr.Pitch >> 1;
            }
            break;
        }

#ifdef UNDER_XBOX
        case D3DFMT_LIN_X8R8G8B8:
        case D3DFMT_LIN_A8R8G8B8:
#endif
        case D3DFMT_X8R8G8B8:
        case D3DFMT_A8R8G8B8: {
            LPDWORD pdwPixel = (LPDWORD)d3dlr.pBits;
            for (i = 0; i < d3dsd.Height; i++) {
                for (j = 0; j < d3dsd.Width; j++) {
                    pbData = (LPBYTE)&pdwPixel[j];
                    for (k = 0; k < 3; k++) {
                        dwCRC = m_dwCRCTable[(dwCRC ^ pbData[k]) & 0xFF] ^ (dwCRC >> 8);
                    }
                }
                pdwPixel += d3dlr.Pitch >> 2;
            }
            break;
        }
    }

    pd3ds->UnlockRect();

    return dwCRC;
}

//******************************************************************************
void CCDevice8::OverlayVerificationResults() {

    DWORD           dwBlock;
    D3DDISPLAYMODE  d3ddm;
    float           fWidth, fHeight, fX, fY, fOffsetX, fOffsetY;
    struct _FBOVERTEX {
        D3DXVECTOR3 vPosition;
        float       fRHW;
        float       u0;
        float       v0;
    } prOverlay[12];

    CreateStateBlock(D3DSBT_ALL, &dwBlock);

    GetDisplayMode(&d3ddm);
    fWidth = (float)d3ddm.Width * FBSCALE;
    fHeight = (float)d3ddm.Height * FBSCALE;
    fX = (float)d3ddm.Width / 2.0f;
    fY = (float)d3ddm.Height / 2.0f;
    fOffsetX = (fX - fWidth) / 2.0f;
    fOffsetY = (fY - fHeight) / 2.0f;

    prOverlay[0].vPosition = D3DXVECTOR3(fX - fWidth - fOffsetX, fY - fOffsetY, 0.0000085f);
    prOverlay[0].fRHW = 120000.0f;
    prOverlay[0].u0 = 0.0f;
    prOverlay[0].v0 = 1.0f;
    prOverlay[1].vPosition = D3DXVECTOR3(fX - fWidth - fOffsetX, fY - fHeight - fOffsetY, 0.0000085f);
    prOverlay[1].fRHW = 120000.0f;
    prOverlay[1].u0 = 0.0f;
    prOverlay[1].v0 = 0.0f;
    prOverlay[2].vPosition = D3DXVECTOR3(fX - fOffsetX, fY - fHeight - fOffsetY, 0.0000085f);
    prOverlay[2].fRHW = 120000.0f;
    prOverlay[2].u0 = 1.0f;
    prOverlay[2].v0 = 0.0f;
    prOverlay[3].vPosition = D3DXVECTOR3(fX - fOffsetX, fY - fOffsetY, 0.0000085f);
    prOverlay[3].fRHW = 120000.0f;
    prOverlay[3].u0 = 1.0f;
    prOverlay[3].v0 = 1.0f;

    prOverlay[4].vPosition = D3DXVECTOR3(fX + fOffsetX, fY - fOffsetY, 0.0000085f);
    prOverlay[4].fRHW = 120000.0f;
    prOverlay[4].u0 = 0.0f;
    prOverlay[4].v0 = 1.0f;
    prOverlay[5].vPosition = D3DXVECTOR3(fX + fOffsetX, fY - fHeight - fOffsetY, 0.0000085f);
    prOverlay[5].fRHW = 120000.0f;
    prOverlay[5].u0 = 0.0f;
    prOverlay[5].v0 = 0.0f;
    prOverlay[6].vPosition = D3DXVECTOR3(fX + fWidth + fOffsetX, fY - fHeight - fOffsetY, 0.0000085f);
    prOverlay[6].fRHW = 120000.0f;
    prOverlay[6].u0 = 1.0f;
    prOverlay[6].v0 = 0.0f;
    prOverlay[7].vPosition = D3DXVECTOR3(fX + fWidth + fOffsetX, fY - fOffsetY, 0.0000085f);
    prOverlay[7].fRHW = 120000.0f;
    prOverlay[7].u0 = 1.0f;
    prOverlay[7].v0 = 1.0f;

    prOverlay[8].vPosition = D3DXVECTOR3(fX - fWidth / 2.0f, fY + fHeight + fOffsetY, 0.0000085f);
    prOverlay[8].fRHW = 120000.0f;
    prOverlay[8].u0 = 0.0f;
    prOverlay[8].v0 = 1.0f;
    prOverlay[9].vPosition = D3DXVECTOR3(fX - fWidth / 2.0f, fY + fOffsetY, 0.0000085f);
    prOverlay[9].fRHW = 120000.0f;
    prOverlay[9].u0 = 0.0f;
    prOverlay[9].v0 = 0.0f;
    prOverlay[10].vPosition = D3DXVECTOR3(fX + fWidth / 2.0f, fY + fOffsetY, 0.0000085f);
    prOverlay[10].fRHW = 120000.0f;
    prOverlay[10].u0 = 1.0f;
    prOverlay[10].v0 = 0.0f;
    prOverlay[11].vPosition = D3DXVECTOR3(fX + fWidth / 2.0f, fY + fHeight + fOffsetY, 0.0000085f);
    prOverlay[11].fRHW = 120000.0f;
    prOverlay[11].u0 = 1.0f;
    prOverlay[11].v0 = 1.0f;

    BeginScene();
    SetVertexShader(D3DFVF_XYZRHW | D3DFVF_TEX1);
    SetRenderState(D3DRS_LIGHTING, FALSE);
    SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
    SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    if (m_uShowVerification == 2) {
        SetTexture(0, m_pd3dtFrameSrc);
        DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, prOverlay, sizeof(prOverlay[0]));
        SetTexture(0, m_pd3dtFrameRef);
        DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, prOverlay + 4, sizeof(prOverlay[0]));
    }
    if (m_uShowVerification) {
        SetTexture(0, m_pd3dtFrameVar);
        DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, prOverlay + 8, sizeof(prOverlay[0]));
    }
    EndScene();

    ApplyStateBlock(dwBlock);
    DeleteStateBlock(dwBlock);
}

//******************************************************************************
BOOL CCDevice8::UpdateFBTexture(CTexture8* pd3dtDst, LPDIRECT3DSURFACE8 pd3dsSrc) {

    D3DSURFACE_DESC d3dsdSrc, d3dsdDst;
    D3DLOCKED_RECT  d3dlrSrc, d3dlrDst;
    LPDWORD         pdwDst;
    float           fWidthRatio, fHeightRatio;
    UINT            i, j;
    HRESULT         hr;

    hr = pd3dtDst->GetLevelDesc(0, &d3dsdDst);
    if (ResultFailed(hr, TEXT("IDirect3DTexture8::GetLevelDesc"))) {
        return FALSE;
    }

    hr = pd3dsSrc->GetDesc(&d3dsdSrc);
    if (ResultFailed(hr, TEXT("IDirect3DSurface8::GetDesc"))) {
        return FALSE;
    }

    fWidthRatio = (float)d3dsdSrc.Width / (float)d3dsdDst.Width;
    fHeightRatio = (float)d3dsdSrc.Height / (float)d3dsdDst.Height;

    hr = pd3dtDst->LockRect(0, &d3dlrDst, NULL, 0);
    if (ResultFailed(hr, TEXT("IDirect3DTexture8::LockRect"))) {
        return FALSE;
    }

#ifndef UNDER_XBOX
    hr = pd3dsSrc->LockRect(&d3dlrSrc, NULL, 0);
#else
    if (d3dsdSrc.Usage == D3DUSAGE_RENDERTARGET) {
        hr = pd3dsSrc->LockRect(&d3dlrSrc, NULL, D3DLOCK_TILED);
    }
    else {
        hr = pd3dsSrc->LockRect(&d3dlrSrc, NULL, 0);
    }
#endif
    if (ResultFailed(hr, TEXT("IDirect3DSurface8::LockRect"))) {
        pd3dtDst->UnlockRect(0);
        return FALSE;
    }

    pdwDst = (LPDWORD)d3dlrDst.pBits;

    // The source surface has a linear format so it does not need to be deswizzled
#ifdef UNDER_XBOX
    if (!XGIsSwizzledFormat(d3dsdDst.Format)) {
#endif
        for (i = 0; i < d3dsdDst.Height; i++) {

            for (j = 0; j < d3dsdDst.Width; j++) {

                pdwDst[j] = *((LPDWORD)((LPBYTE)d3dlrSrc.pBits + (UINT)((float)i * fHeightRatio) * d3dlrSrc.Pitch) + (UINT)((float)j * fWidthRatio));
            }

            pdwDst += d3dlrDst.Pitch >> 2;
        }
#ifdef UNDER_XBOX
    }
    else {

        Swizzler swz(d3dsdDst.Width, d3dsdDst.Height, 1);
        swz.SetU(0);
        swz.SetV(0);

        for (i = 0; i < d3dsdDst.Height; i++, swz.IncV()) {
            for (j = 0; j < d3dsdDst.Width; j++, swz.IncU()) {
                pdwDst[swz.Get2D()] = *((LPDWORD)((LPBYTE)d3dlrSrc.pBits + (UINT)((float)i * fHeightRatio) * d3dlrSrc.Pitch) + (UINT)((float)j * fWidthRatio));
            }
        }
    }
#endif

    pd3dsSrc->UnlockRect();
    pd3dtDst->UnlockRect(0);

    return TRUE;
}

//******************************************************************************
float CCDevice8::CompareFrames(LPDIRECT3DSURFACE8 pd3dsSrc, LPDIRECT3DSURFACE8 pd3dsRef, LPDIRECT3DSURFACE8 pd3dsVar) {

    D3DSURFACE_DESC d3dsd, d3dsdVar;
    D3DLOCKED_RECT  d3dlrSrc, d3dlrRef, d3dlrVar;
    int             r, g, b;
    float           fPixelMagSq, fMaxMagSq;
    float           fDelta, fMaxDelta = 0.0f;
    LPDWORD         pdwSrc, pdwRef;
    float           fWidthRatio, fHeightRatio;
    UINT            i, j;
    HRESULT         hr;

    fMaxMagSq = D3DXVec3LengthSq(&(D3DXVECTOR3(255.0f, 255.0f, 255.0f) - D3DXVECTOR3(0.0f, 0.0f, 0.0f)));

    hr = pd3dsSrc->GetDesc(&d3dsd);
    if (ResultFailed(hr, TEXT("IDirect3DSurface8::GetDesc"))) {
        return 0.0f;
    }

    if (pd3dsVar) {
        hr = pd3dsVar->GetDesc(&d3dsdVar);
        if (ResultFailed(hr, TEXT("IDirect3DSurface8::GetDesc"))) {
            return 0.0f;
        }

        fWidthRatio = (float)d3dsdVar.Width / (float)d3dsd.Width;
        fHeightRatio = (float)d3dsdVar.Height / (float)d3dsd.Height;
    }

#ifndef UNDER_XBOX
    hr = pd3dsSrc->LockRect(&d3dlrSrc, NULL, 0);
#else
    hr = pd3dsSrc->LockRect(&d3dlrSrc, NULL, D3DLOCK_TILED);
#endif
    if (ResultFailed(hr, TEXT("IDirect3DSurface8::LockRect"))) {
        return 0.0f;
    }

    hr = pd3dsRef->LockRect(&d3dlrRef, NULL, 0);
    if (ResultFailed(hr, TEXT("IDirect3DSurface8::LockRect"))) {
        pd3dsSrc->UnlockRect();
        return 0.0f;
    }

    if (pd3dsVar) {
        hr = pd3dsVar->LockRect(&d3dlrVar, NULL, 0);
        if (ResultFailed(hr, TEXT("IDirect3DSurface8::LockRect"))) {
            pd3dsSrc->UnlockRect();
            pd3dsRef->UnlockRect();
            return 0.0f;
        }
    }

    pdwSrc = (LPDWORD)d3dlrSrc.pBits;
    pdwRef = (LPDWORD)d3dlrRef.pBits;

#ifdef UNDER_XBOX
    Swizzler swz(d3dsdVar.Width, d3dsdVar.Height, 1);
    BOOL bSwizzled = XGIsSwizzledFormat(d3dsdVar.Format);
#endif

    for (i = 0; i < d3dsd.Height; i++) {

        for (j = 0; j < d3dsd.Width; j++) {

            r = (int)RGBA_GETRED(pdwSrc[j]) - (int)RGBA_GETRED(pdwRef[j]);
            g = (int)RGBA_GETGREEN(pdwSrc[j]) - (int)RGBA_GETGREEN(pdwRef[j]);
            b = (int)RGBA_GETBLUE(pdwSrc[j]) - (int)RGBA_GETBLUE(pdwRef[j]);;
            fPixelMagSq = D3DXVec3LengthSq(&D3DXVECTOR3((float)r, (float)g, (float)b));
            fDelta = fPixelMagSq / fMaxMagSq;
            if (fDelta > fMaxDelta) {
                fMaxDelta = fDelta;
            }
            if (pd3dsVar) {
#ifdef UNDER_XBOX
                if (!bSwizzled) {
#endif
                    *((LPDWORD)((LPBYTE)d3dlrVar.pBits + (UINT)((float)i * fHeightRatio) * d3dlrVar.Pitch) + (UINT)((float)j * fWidthRatio)) = RGBA_MAKE((r < 0 ? -r : r), (g < 0 ? -g : g), (b < 0 ? -b : b), 255);
#ifdef UNDER_XBOX
                }
                else {
                    *((LPDWORD)d3dlrVar.pBits + (swz.SwizzleU((UINT)((float)j * fWidthRatio)) | swz.SwizzleV((UINT)((float)i * fHeightRatio)))) = RGBA_MAKE((r < 0 ? -r : r), (g < 0 ? -g : g), (b < 0 ? -b : b), 255);
                }
#endif
            }
        }

        pdwSrc += d3dlrSrc.Pitch >> 2;
        pdwRef += d3dlrRef.Pitch >> 2;
    }
    
    if (pd3dsVar) {
        pd3dsVar->UnlockRect();
    }
    pd3dsRef->UnlockRect();
    pd3dsSrc->UnlockRect();
    
    return 1.0f - fMaxDelta;    
}

//******************************************************************************
ULONG CCDevice8::AddRef() {

    ULONG uRefC = 0, uRefS;

    if (m_pd3dd) {
        uRefC = m_pd3dd->AddRef();
        if (m_pClient->CallMethod(MID_DEV8_AddRef, this, &uRefS)) {
#ifndef UNDER_XBOX
            if (uRefC != uRefS) {
                Log(LOG_WARN, TEXT("IDirect3DDevice8::AddRef result inconsistent [C: %d, S: %d]"), uRefC, uRefS);
            }
#endif // !UNDER_XBOX
        }
        CObject::AddRef();
    }

    return uRefC;
}

//******************************************************************************
ULONG CCDevice8::Release() {

    ULONG uRefC = 0, uRefS;

    if (m_pd3dd) {

        uRefC = m_pd3dd->Release();
        if (m_pClient->CallMethod(MID_DEV8_Release, this, &uRefS)) {
#ifndef UNDER_XBOX
            if (uRefC != uRefS) {
                Log(LOG_WARN, TEXT("IDirect3DDevice8::Release result inconsistent [C: %d, S: %d]"), uRefC, uRefS);
            }
#endif // !UNDER_XBOX
        }
        CObject::Release();
    }

    return uRefC;
}

#ifndef UNDER_XBOX

//******************************************************************************
HRESULT CCDevice8::CreateAdditionalSwapChain(D3DPRESENT_PARAMETERS* pd3dpp, CSwapChain8** ppSwapChain) {

    CCSwapChain8*        pSwapChain;
    LPDIRECT3DSWAPCHAIN8 pd3dsw;
    HRESULT              hrC, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }

    if (!ppSwapChain) {
        return D3DERR_INVALIDCALL;
    }
#endif // D3DLOCUS_DEBUG

    *ppSwapChain = NULL;

#ifndef DEMO_HACK
    hrC = m_pd3dd->CreateAdditionalSwapChain(pd3dpp, &pd3dsw);
#else
    hrC = E_NOTIMPL;
    pd3dsw = NULL;
#endif
    if (SUCCEEDED(hrC)) {
        pSwapChain = new CCSwapChain8();
        if (!pSwapChain) {
            pd3dsw->Release();
            return E_OUTOFMEMORY;
        }

        if (!pSwapChain->Create(m_pClient, this, pd3dsw)) {
            pd3dsw->Release();
            return ERR_LOCUSFAIL;
        }

        *ppSwapChain = pSwapChain;
    }
    else {
        pSwapChain = NULL;
    }

    if (m_pClient->CallMethod(MID_DEV8_CreateAdditionalSwapChain, this, &hrS, pd3dpp, sizeof(D3DPRESENT_PARAMETERS), &pSwapChain)) {
        if (hrC != hrS) {
            Log(LOG_WARN, TEXT("IDirect3DDevice8::CreateAdditionalSwapChain result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
        }
    }

    return hrC;
}

#endif // !UNDER_XBOX

//******************************************************************************
HRESULT CCDevice8::CreateCubeTexture(UINT uEdgeLength, UINT uLevels, DWORD dwUsage, D3DFORMAT fmt, D3DPOOL pool, CCubeTexture8** ppCubeTexture) {

    CCCubeTexture8*        pCubeTexture;
    LPDIRECT3DCUBETEXTURE8 pd3dtc;
    HRESULT                hrC, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }

    if (!ppCubeTexture) {
        return D3DERR_INVALIDCALL;
    }
#endif // D3DLOCUS_DEBUG

    *ppCubeTexture = NULL;

    hrC = m_pd3dd->CreateCubeTexture(uEdgeLength, uLevels, dwUsage, fmt, pool, &pd3dtc);
    if (SUCCEEDED(hrC)) {
        pCubeTexture = new CCCubeTexture8();
        if (!pCubeTexture) {
            pd3dtc->Release();
            return E_OUTOFMEMORY;
        }

        if (!pCubeTexture->Create(m_pClient, this, pd3dtc)) {
            pd3dtc->Release();
            return ERR_LOCUSFAIL;
        }

        *ppCubeTexture = pCubeTexture;
    }
    else {
        pCubeTexture = NULL;
    }

    if (m_pClient->CallMethod(MID_DEV8_CreateCubeTexture, this, &hrS, uEdgeLength, uLevels, dwUsage, fmt, pool, &pCubeTexture)) {
        if (hrC != hrS) {
            Log(LOG_WARN, TEXT("IDirect3DDevice8::CreateCubeTexture result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
        }
    }

    return hrC;
}

//******************************************************************************
HRESULT CCDevice8::CreateDepthStencilSurface(UINT uWidth, UINT uHeight, D3DFORMAT fmt, D3DMULTISAMPLE_TYPE mst, CSurface8** ppSurface) {

    CCSurface8*        pSurface;
    LPDIRECT3DSURFACE8 pd3ds;
    HRESULT            hrC, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }

    if (!ppSurface) {
        return D3DERR_INVALIDCALL;
    }
#endif // D3DLOCUS_DEBUG

    *ppSurface = NULL;

    hrC = m_pd3dd->CreateDepthStencilSurface(uWidth, uHeight, fmt, mst, &pd3ds);
    if (SUCCEEDED(hrC)) {
        pSurface = new CCSurface8();
        if (!pSurface) {
            pd3ds->Release();
            return E_OUTOFMEMORY;
        }

        if (!pSurface->Create(m_pClient, this, pd3ds)) {
            pd3ds->Release();
            return ERR_LOCUSFAIL;
        }

        *ppSurface = pSurface;
    }
    else {
        pSurface = NULL;
    }

    if (m_pClient->CallMethod(MID_DEV8_CreateDepthStencilSurface, this, &hrS, uWidth, uHeight, fmt, mst, &pSurface)) {
        if (hrC != hrS) {
            Log(LOG_WARN, TEXT("IDirect3DDevice8::CreateDepthStencilSurface result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
        }
    }

    return hrC;
}

//******************************************************************************
HRESULT CCDevice8::CreateImageSurface(UINT uWidth, UINT uHeight, D3DFORMAT fmt, CSurface8** ppSurface) {

    CCSurface8*        pSurface;
    LPDIRECT3DSURFACE8 pd3ds;
    HRESULT            hrC, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }

    if (!ppSurface) {
        return D3DERR_INVALIDCALL;
    }
#endif // D3DLOCUS_DEBUG

    *ppSurface = NULL;

    hrC = m_pd3dd->CreateImageSurface(uWidth, uHeight, fmt, &pd3ds);
    if (SUCCEEDED(hrC)) {
        pSurface = new CCSurface8();
        if (!pSurface) {
            pd3ds->Release();
            return E_OUTOFMEMORY;
        }

        if (!pSurface->Create(m_pClient, this, pd3ds)) {
            pd3ds->Release();
            return ERR_LOCUSFAIL;
        }

        *ppSurface = pSurface;
    }
    else {
        pSurface = NULL;
    }

    if (m_pClient->CallMethod(MID_DEV8_CreateImageSurface, this, &hrS, uWidth, uHeight, fmt, &pSurface)) {
        if (hrC != hrS) {
            Log(LOG_WARN, TEXT("IDirect3DDevice8::CreateImageSurface result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
        }
    }

    return hrC;
}

//******************************************************************************
HRESULT CCDevice8::CreateIndexBuffer(UINT uLength, DWORD dwUsage, D3DFORMAT fmt, D3DPOOL pool, CIndexBuffer8** ppIndexBuffer) {

    CCIndexBuffer8*        pIndexBuffer;
    LPDIRECT3DINDEXBUFFER8 pd3di;
    HRESULT                hrC, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }

    if (!ppIndexBuffer) {
        return D3DERR_INVALIDCALL;
    }
#endif // D3DLOCUS_DEBUG

    *ppIndexBuffer = NULL;

    hrC = m_pd3dd->CreateIndexBuffer(uLength, dwUsage, fmt, pool, &pd3di);
    if (SUCCEEDED(hrC)) {
        pIndexBuffer = new CCIndexBuffer8();
        if (!pIndexBuffer) {
            pd3di->Release();
            return E_OUTOFMEMORY;
        }

        if (!pIndexBuffer->Create(m_pClient, this, pd3di)) {
            pd3di->Release();
            return ERR_LOCUSFAIL;
        }

        *ppIndexBuffer = pIndexBuffer;
    }
    else {
        pIndexBuffer = NULL;
    }

    if (m_pClient->CallMethod(MID_DEV8_CreateIndexBuffer, this, &hrS, uLength, dwUsage, fmt, pool, &pIndexBuffer)) {
        if (hrC != hrS) {
            Log(LOG_WARN, TEXT("IDirect3DDevice8::CreateIndexBuffer result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
        }
    }

    return hrC;
}

//******************************************************************************
HRESULT CCDevice8::CreateRenderTarget(UINT uWidth, UINT uHeight, D3DFORMAT fmt, D3DMULTISAMPLE_TYPE mst, BOOL bLockable, CSurface8** ppSurface) {

    CCSurface8*        pSurface;
    LPDIRECT3DSURFACE8 pd3ds;
    HRESULT            hrC, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }

    if (!ppSurface) {
        return D3DERR_INVALIDCALL;
    }
#endif // D3DLOCUS_DEBUG

    *ppSurface = NULL;

    hrC = m_pd3dd->CreateRenderTarget(uWidth, uHeight, fmt, mst, bLockable, &pd3ds);
    if (SUCCEEDED(hrC)) {
        pSurface = new CCSurface8();
        if (!pSurface) {
            pd3ds->Release();
            return E_OUTOFMEMORY;
        }

        if (!pSurface->Create(m_pClient, this, pd3ds)) {
            pd3ds->Release();
            return ERR_LOCUSFAIL;
        }

        *ppSurface = pSurface;
    }
    else {
        pSurface = NULL;
    }

    if (m_pClient->CallMethod(MID_DEV8_CreateRenderTarget, this, &hrS, uWidth, uHeight, fmt, mst, bLockable, &pSurface)) {
        if (hrC != hrS) {
            Log(LOG_WARN, TEXT("IDirect3DDevice8::CreateRenderTarget result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
        }
    }

    return hrC;
}

//******************************************************************************
HRESULT CCDevice8::CreateTexture(UINT uWidth, UINT uHeight, UINT uLevels, DWORD dwUsage, D3DFORMAT fmt, D3DPOOL pool, CTexture8** ppTexture) {

    CCTexture8*        pTexture;
    LPDIRECT3DTEXTURE8 pd3dt;
    HRESULT            hrC, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }

    if (!ppTexture) {
        return D3DERR_INVALIDCALL;
    }
#endif // D3DLOCUS_DEBUG

    *ppTexture = NULL;

    hrC = m_pd3dd->CreateTexture(uWidth, uHeight, uLevels, dwUsage, fmt, pool, &pd3dt);
    if (SUCCEEDED(hrC)) {
        pTexture = new CCTexture8();
        if (!pTexture) {
            pd3dt->Release();
            return E_OUTOFMEMORY;
        }

        if (!pTexture->Create(m_pClient, this, pd3dt)) {
            pd3dt->Release();
            return ERR_LOCUSFAIL;
        }

        *ppTexture = pTexture;
    }
    else {
        pTexture = NULL;
    }

    if (m_pClient->CallMethod(MID_DEV8_CreateTexture, this, &hrS, uWidth, uHeight, uLevels, dwUsage, fmt, pool, &pTexture)) {
        if (hrC != hrS) {
            Log(LOG_WARN, TEXT("IDirect3DDevice8::CreateTexture result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
        }
    }

    return hrC;
}

//******************************************************************************
HRESULT CCDevice8::CreateVertexBuffer(UINT uLength, DWORD dwUsage, DWORD dwFVF, D3DPOOL pool, CVertexBuffer8** ppVertexBuffer) {

    CCVertexBuffer8*        pVertexBuffer;
    LPDIRECT3DVERTEXBUFFER8 pd3dr;
    HRESULT                 hrC, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }

    if (!ppVertexBuffer) {
        return D3DERR_INVALIDCALL;
    }
#endif // D3DLOCUS_DEBUG

    *ppVertexBuffer = NULL;

    hrC = m_pd3dd->CreateVertexBuffer(uLength, dwUsage, dwFVF, pool, &pd3dr);
    if (SUCCEEDED(hrC)) {
        pVertexBuffer = new CCVertexBuffer8();
        if (!pVertexBuffer) {
            pd3dr->Release();
            return E_OUTOFMEMORY;
        }

        if (!pVertexBuffer->Create(m_pClient, this, pd3dr)) {
            pd3dr->Release();
            return ERR_LOCUSFAIL;
        }

        *ppVertexBuffer = pVertexBuffer;
    }
    else {
        pVertexBuffer = NULL;
    }

    if (m_pClient->CallMethod(MID_DEV8_CreateVertexBuffer, this, &hrS, uLength, dwUsage, dwFVF, pool, &pVertexBuffer)) {
        if (hrC != hrS) {
            Log(LOG_WARN, TEXT("IDirect3DDevice8::CreateVertexBuffer result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
        }
    }

    return hrC;
}

//******************************************************************************
HRESULT CCDevice8::CreateVolumeTexture(UINT uWidth, UINT uHeight, UINT uDepth, UINT uLevels, DWORD dwUsage, D3DFORMAT fmt, D3DPOOL pool, CVolumeTexture8** ppVolumeTexture) {

    CCVolumeTexture8*        pVolumeTexture;
    LPDIRECT3DVOLUMETEXTURE8 pd3dtv;
    HRESULT                  hrC, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }

    if (!ppVolumeTexture) {
        return D3DERR_INVALIDCALL;
    }
#endif // D3DLOCUS_DEBUG

    *ppVolumeTexture = NULL;

    hrC = m_pd3dd->CreateVolumeTexture(uWidth, uHeight, uDepth, uLevels, dwUsage, fmt, pool, &pd3dtv);
    if (SUCCEEDED(hrC)) {
        pVolumeTexture = new CCVolumeTexture8();
        if (!pVolumeTexture) {
            pd3dtv->Release();
            return E_OUTOFMEMORY;
        }

        if (!pVolumeTexture->Create(m_pClient, this, pd3dtv)) {
            pd3dtv->Release();
            return ERR_LOCUSFAIL;
        }

        *ppVolumeTexture = pVolumeTexture;
    }
    else {
        pVolumeTexture = NULL;
    }

    if (m_pClient->CallMethod(MID_DEV8_CreateVolumeTexture, this, &hrS, uWidth, uHeight, uDepth, uLevels, dwUsage, fmt, pool, &pVolumeTexture)) {
        if (hrC != hrS) {
            Log(LOG_WARN, TEXT("IDirect3DDevice8::CreateVolumeTexture result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
        }
    }

    return hrC;
}

//******************************************************************************
HRESULT CCDevice8::ApplyStateBlock(DWORD dwToken) {

    HRESULT hrC, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    hrC = m_pd3dd->ApplyStateBlock(dwToken);

    if (m_pClient->CallMethod(MID_DEV8_ApplyStateBlock, this, &hrS, dwToken)) {
        if (hrC != hrS) {
            Log(LOG_WARN, TEXT("IDirect3DDevice8::ApplyStateBlock result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
        }
    }

    return hrC;
}

//******************************************************************************
HRESULT CCDevice8::BeginStateBlock() {

    HRESULT hrC, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    hrC = m_pd3dd->BeginStateBlock();

    if (m_pClient->CallMethod(MID_DEV8_BeginStateBlock, this, &hrS)) {
        if (hrC != hrS) {
            Log(LOG_WARN, TEXT("IDirect3DDevice8::BeginStateBlock result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
        }
    }

    return hrC;
}

//******************************************************************************
HRESULT CCDevice8::EndStateBlock(DWORD* pdwToken) {

    HRESULT hrC, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    hrC = m_pd3dd->EndStateBlock(pdwToken);

    if (m_pClient->CallMethod(MID_DEV8_EndStateBlock, this, &hrS, pdwToken)) {
        if (hrC != hrS) {
            Log(LOG_WARN, TEXT("IDirect3DDevice8::EndStateBlock result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
        }
    }

    return hrC;
}

//******************************************************************************
HRESULT CCDevice8::CreateStateBlock(D3DSTATEBLOCKTYPE sbt, DWORD* pdwToken) {

    HRESULT hrC, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    hrC = m_pd3dd->CreateStateBlock(sbt, pdwToken);

    if (m_pClient->CallMethod(MID_DEV8_CreateStateBlock, this, &hrS, sbt, pdwToken)) {
        if (hrC != hrS) {
            Log(LOG_WARN, TEXT("IDirect3DDevice8::CreateStateBlock result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
        }
    }

    return hrC;
}

//******************************************************************************
HRESULT CCDevice8::DeleteStateBlock(DWORD dwToken) {

    HRESULT hrC, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    hrC = m_pd3dd->DeleteStateBlock(dwToken);

    if (m_pClient->CallMethod(MID_DEV8_DeleteStateBlock, this, &hrS, dwToken)) {
        if (hrC != hrS) {
            Log(LOG_WARN, TEXT("IDirect3DDevice8::DeleteStateBlock result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
        }
    }

    return hrC;
}

//******************************************************************************
HRESULT CCDevice8::CaptureStateBlock(DWORD dwToken) {

    HRESULT hrC, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    hrC = m_pd3dd->CaptureStateBlock(dwToken);

    if (m_pClient->CallMethod(MID_DEV8_CaptureStateBlock, this, &hrS, dwToken)) {
        if (hrC != hrS) {
            Log(LOG_WARN, TEXT("IDirect3DDevice8::CaptureStateBlock result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
        }
    }

    return hrC;
}

#ifndef UNDER_XBOX

//******************************************************************************
HRESULT CCDevice8::GetClipStatus(D3DCLIPSTATUS8* pd3dcs) {

    D3DCLIPSTATUS8  d3dcs;
    HRESULT         hrC, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

#ifndef UNDER_XBOX

    hrC = m_pd3dd->GetClipStatus(pd3dcs);

    if (m_pClient->CallMethod(MID_DEV8_GetClipStatus, this, &hrS, &d3dcs, sizeof(D3DCLIPSTATUS8))) {
        if (hrC != hrS) {
            Log(LOG_WARN, TEXT("IDirect3DDevice8::GetClipStatus result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
        }
        else if (SUCCEEDED(hrC)) {
            if (memcmp(pd3dcs, &d3dcs, sizeof(D3DCLIPSTATUS8))) {
                Log(LOG_WARN, TEXT("IDirect3DDevice8::GetClipStatus returned an inconsistent clip status"));
            }
        }
    }

#else

    Log(LOG_FAIL, TEXT("IDirect3DDevice8::GetClipStatus is not implemented on Xbox"));
    return E_NOTIMPL;

#endif // UNDER_XBOX

    return hrC;
}

//******************************************************************************
HRESULT CCDevice8::SetClipStatus(D3DCLIPSTATUS8* pd3dcs) {

    HRESULT hrC, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

#ifndef UNDER_XBOX

    hrC = m_pd3dd->SetClipStatus(pd3dcs);

    if (m_pClient->CallMethod(MID_DEV8_SetClipStatus, this, &hrS, pd3dcs, sizeof(D3DCLIPSTATUS8))) {
        if (hrC != hrS) {
            Log(LOG_WARN, TEXT("IDirect3DDevice8::SetClipStatus result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
        }
    }

#else

    Log(LOG_FAIL, TEXT("IDirect3DDevice8::SetClipStatus is not implemented on Xbox"));
    return E_NOTIMPL;

#endif // UNDER_XBOX

    return hrC;
}

#endif // !UNDER_XBOX

//******************************************************************************
HRESULT CCDevice8::GetRenderState(D3DRENDERSTATETYPE rst, DWORD* pdwValue) {

    DWORD   dwState;
    HRESULT hrC, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    hrC = m_pd3dd->GetRenderState(rst, pdwValue);

    if (m_pClient->CallMethod(MID_DEV8_GetRenderState, this, &hrS, rst, &dwState)) {
        if (hrC != hrS) {
            Log(LOG_WARN, TEXT("IDirect3DDevice8::GetRenderState result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
        }
        else if (SUCCEEDED(hrC)) {
            if (*pdwValue != dwState) {
                Log(LOG_WARN, TEXT("IDirect3DDevice8::GetRenderState returned an inconsistent state [C: %d, S: %d]"), *pdwValue, dwState);
            }
        }
    }

    return hrC;
}

//******************************************************************************
HRESULT CCDevice8::SetRenderState(D3DRENDERSTATETYPE rst, DWORD dwValue) {

    HRESULT hrC, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    hrC = m_pd3dd->SetRenderState(rst, dwValue);

    if (m_pClient->CallMethod(MID_DEV8_SetRenderState, this, &hrS, rst, dwValue)) {
        if (hrC != hrS) {
            Log(LOG_WARN, TEXT("IDirect3DDevice8::SetRenderState result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
        }
    }

    return hrC;
}

//******************************************************************************
HRESULT CCDevice8::GetRenderTarget(CSurface8** ppRenderTarget) {

    LPDIRECT3DSURFACE8  pd3ds;
    CCSurface8*         pSurface;
    HRESULT             hrC, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }

    if (!ppRenderTarget) {
        return D3DERR_INVALIDCALL;
    }
#endif // D3DLOCUS_DEBUG

    *ppRenderTarget = NULL;

    hrC = m_pd3dd->GetRenderTarget(&pd3ds);
    if (SUCCEEDED(hrC)) {
        
        if (!m_pObjectMap->Find((DWORD)pd3ds, (LPDWORD)&pSurface)) {

            pSurface = new CCSurface8();
            if (!pSurface) {
                pd3ds->Release();
                return E_OUTOFMEMORY;
            }

            if (!pSurface->Create(m_pClient, this, pd3ds)) {
                pd3ds->Release();
                return ERR_LOCUSFAIL;
            }
        }

        pSurface->CObject::AddRef();

        *ppRenderTarget = pSurface;
    }
    else {
        pSurface = NULL;
    }

    if (m_pClient->CallMethod(MID_DEV8_GetRenderTarget, this, &hrS, &pSurface)) {
        if (hrC != hrS) {
            Log(LOG_WARN, TEXT("IDirect3DDevice8::GetRenderTarget result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
        }
    }

    return hrC;
}

//******************************************************************************
HRESULT CCDevice8::SetRenderTarget(CSurface8* pRenderTarget, CSurface8* pDepthStencil) {

    LPDIRECT3DSURFACE8 pd3dsRenderTarget = NULL, pd3dsDepthStencil = NULL;
    LPDIRECT3DSURFACE8 pd3dsOldTarget, pd3dsOldDepth;
    CCSurface8*        pSurface;
    HRESULT            hrC, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    if (pRenderTarget) {
        pd3dsRenderTarget = pRenderTarget->GetIDirect3DSurface8();
        if (FAILED(m_pd3dd->GetRenderTarget(&pd3dsOldTarget))) {
            return ERR_LOCUSFAIL;
        }
        pd3dsOldTarget->Release();
    }
    if (pDepthStencil) {
        pd3dsDepthStencil = pDepthStencil->GetIDirect3DSurface8();
        if (FAILED(m_pd3dd->GetDepthStencilSurface(&pd3dsOldDepth))) {
            pd3dsOldDepth = NULL;
        }
        else {
            pd3dsOldDepth->Release();
        }
    }

    hrC = m_pd3dd->SetRenderTarget(pd3dsRenderTarget, pd3dsDepthStencil);

    if (SUCCEEDED(hrC)) {

        if (pRenderTarget) {
            pRenderTarget->CObject::AddRef();
            if (m_pObjectMap->Find((DWORD)pd3dsOldTarget, (LPDWORD)&pSurface)) {
                pSurface->CObject::Release();
            }
        }
        if (pDepthStencil) {
            pDepthStencil->CObject::AddRef();
            if (pd3dsOldDepth && m_pObjectMap->Find((DWORD)pd3dsOldDepth, (LPDWORD)&pSurface)) {
                pSurface->CObject::Release();
            }
        }
    }

    if (m_pClient->CallMethod(MID_DEV8_SetRenderTarget, this, &hrS, pRenderTarget, pDepthStencil)) {
        if (hrC != hrS) {
            Log(LOG_WARN, TEXT("IDirect3DDevice8::SetRenderTarget result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
        }
    }

    return hrC;
}

//******************************************************************************
HRESULT CCDevice8::GetTransform(D3DTRANSFORMSTATETYPE tst, D3DMATRIX* pm) {

    D3DMATRIX   m;
    HRESULT     hrC, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    hrC = m_pd3dd->GetTransform(tst, pm);

    if (m_pClient->CallMethod(MID_DEV8_GetTransform, this, &hrS, tst, &m, sizeof(D3DMATRIX))) {
        if (hrC != hrS) {
            Log(LOG_WARN, TEXT("IDirect3DDevice8::GetTransform result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
        }
        else if (SUCCEEDED(hrC)) {
            if (memcmp(pm, &m, sizeof(D3DMATRIX))) {
                Log(LOG_WARN, TEXT("IDirect3DDevice8::GetTransform returned an inconsistent matrix"));
            }
        }
    }

    return hrC;
}

//******************************************************************************
HRESULT CCDevice8::SetTransform(D3DTRANSFORMSTATETYPE tst, D3DMATRIX* pm) {

    HRESULT hrC, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    hrC = m_pd3dd->SetTransform(tst, pm);

    if (m_pClient->CallMethod(MID_DEV8_SetTransform, this, &hrS, tst, pm, sizeof(D3DMATRIX))) {
        if (hrC != hrS) {
            Log(LOG_WARN, TEXT("IDirect3DDevice8::SetTransform result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
        }
    }

    return hrC;
}

//******************************************************************************
HRESULT CCDevice8::MultiplyTransform(D3DTRANSFORMSTATETYPE tst, D3DMATRIX* pm) {

    HRESULT hrC, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    hrC = m_pd3dd->MultiplyTransform(tst, pm);

    if (m_pClient->CallMethod(MID_DEV8_MultiplyTransform, this, &hrS, tst, pm, sizeof(D3DMATRIX))) {
        if (hrC != hrS) {
            Log(LOG_WARN, TEXT("IDirect3DDevice8::MultiplyTransform result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
        }
    }

    return hrC;
}

//******************************************************************************
HRESULT CCDevice8::Clear(DWORD dwCount, D3DRECT* prc, DWORD dwFlags, D3DCOLOR c, float fZ, DWORD dwStencil) {

    D3DRECT rcNULL = {-1, -1, -1, -1};
    HRESULT hrC, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    hrC = m_pd3dd->Clear(dwCount, prc, dwFlags, c, fZ, dwStencil);

    if (!prc) {
        prc = &rcNULL;
    }

    if (m_pClient->CallMethod(MID_DEV8_Clear, this, &hrS, dwCount, prc, sizeof(D3DRECT), dwFlags, c, F2DW(fZ), dwStencil)) {
        if (hrC != hrS) {
            Log(LOG_WARN, TEXT("IDirect3DDevice8::Clear result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
        }
    }

    return hrC;
}

//******************************************************************************
HRESULT CCDevice8::GetViewport(D3DVIEWPORT8* pviewport) {

    D3DVIEWPORT8 viewport;
    HRESULT      hrC, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    hrC = m_pd3dd->GetViewport(pviewport);

    if (m_pClient->CallMethod(MID_DEV8_GetViewport, this, &hrS, &viewport, sizeof(D3DVIEWPORT8))) {
        if (hrC != hrS) {
            Log(LOG_WARN, TEXT("IDirect3DDevice8::GetViewport result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
        }
        else if (SUCCEEDED(hrC)) {
            if (memcmp(pviewport, &viewport, sizeof(D3DVIEWPORT8))) {
                Log(LOG_WARN, TEXT("IDirect3DDevice8::GetViewport returned an inconsistent viewport"));
            }
        }
    }

    return hrC;
}

//******************************************************************************
HRESULT CCDevice8::SetViewport(D3DVIEWPORT8* pviewport) {

    HRESULT hrC, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    hrC = m_pd3dd->SetViewport(pviewport);

    if (m_pClient->CallMethod(MID_DEV8_SetViewport, this, &hrS, pviewport, sizeof(D3DVIEWPORT8))) {
        if (hrC != hrS) {
            Log(LOG_WARN, TEXT("IDirect3DDevice8::SetViewport result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
        }
    }

    return hrC;
}

//******************************************************************************
HRESULT CCDevice8::GetClipPlane(DWORD dwIndex, float* pfPlane) {

    float   pfPlaneS[4];
    HRESULT hrC, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

#ifndef UNDER_XBOX

    hrC = m_pd3dd->GetClipPlane(dwIndex, pfPlane);

    if (m_pClient->CallMethod(MID_DEV8_GetClipPlane, this, &hrS, dwIndex, pfPlaneS, 4 * sizeof(float))) {
        if (hrC != hrS) {
            Log(LOG_WARN, TEXT("IDirect3DDevice8::GetClipPlane result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
        }
        else if (SUCCEEDED(hrC)) {
            if (memcmp(pfPlane, pfPlaneS, 4 * sizeof(float))) {
                Log(LOG_WARN, TEXT("IDirect3DDevice8::GetClipPlane returned an inconsistent plane"));
            }
        }
    }

#else

    Log(LOG_FAIL, TEXT("IDirect3DDevice8::GetClipPlane is not implemented on Xbox"));
    return E_NOTIMPL;

#endif // UNDER_XBOX

    return hrC;
}

//******************************************************************************
HRESULT CCDevice8::SetClipPlane(DWORD dwIndex, float* pfPlane) {

    HRESULT hrC, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

#ifndef UNDER_XBOX

    hrC = m_pd3dd->SetClipPlane(dwIndex, pfPlane);

    if (m_pClient->CallMethod(MID_DEV8_SetClipPlane, this, &hrS, dwIndex, pfPlane, 4 * sizeof(float))) {
        if (hrC != hrS) {
            Log(LOG_WARN, TEXT("IDirect3DDevice8::SetClipPlane result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
        }
    }

#else

    Log(LOG_FAIL, TEXT("IDirect3DDevice8::SetClipPlane is not implemented on Xbox"));
    return E_NOTIMPL;

#endif // UNDER_XBOX

    return hrC;
}

//******************************************************************************
HRESULT CCDevice8::GetLight(DWORD dwIndex, D3DLIGHT8* plight) {

    D3DLIGHT8   light;
    HRESULT     hrC, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    hrC = m_pd3dd->GetLight(dwIndex, plight);

    if (m_pClient->CallMethod(MID_DEV8_GetLight, this, &hrS, dwIndex, &light, sizeof(D3DLIGHT8))) {
        if (hrC != hrS) {
            Log(LOG_WARN, TEXT("IDirect3DDevice8::GetLight result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
        }
        else if (SUCCEEDED(hrC)) {
            if (memcmp(plight, &light, sizeof(D3DLIGHT8))) {
                Log(LOG_WARN, TEXT("IDirect3DDevice8::GetLight returned an inconsistent light"));
            }
        }
    }

    return hrC;
}

//******************************************************************************
HRESULT CCDevice8::SetLight(DWORD dwIndex, D3DLIGHT8* plight) {

    HRESULT hrC, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    hrC = m_pd3dd->SetLight(dwIndex, plight);

    if (m_pClient->CallMethod(MID_DEV8_SetLight, this, &hrS, dwIndex, plight, sizeof(D3DLIGHT8))) {
        if (hrC != hrS) {
            Log(LOG_WARN, TEXT("IDirect3DDevice8::SetLight result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
        }
    }

    return hrC;
}

//******************************************************************************
HRESULT CCDevice8::LightEnable(DWORD dwIndex, BOOL bEnable) {

    HRESULT hrC, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    hrC = m_pd3dd->LightEnable(dwIndex, bEnable);

    if (m_pClient->CallMethod(MID_DEV8_LightEnable, this, &hrS, dwIndex, bEnable)) {
        if (hrC != hrS) {
            Log(LOG_WARN, TEXT("IDirect3DDevice8::LightEnable result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
        }
    }

    return hrC;
}

//******************************************************************************
HRESULT CCDevice8::GetLightEnable(DWORD dwIndex, BOOL* pbEnable) {

    BOOL    bEnable;
    HRESULT hrC, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    hrC = m_pd3dd->GetLightEnable(dwIndex, pbEnable);

    if (m_pClient->CallMethod(MID_DEV8_GetLightEnable, this, &hrS, dwIndex, &bEnable)) {
        if (hrC != hrS) {
            Log(LOG_WARN, TEXT("IDirect3DDevice8::GetLightEnable result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
        }
        else if (SUCCEEDED(hrC)) {
            if (*pbEnable != bEnable) {
                Log(LOG_WARN, TEXT("IDirect3DDevice8::GetLightEnable returned an inconsistent state"));
            }
        }
    }

    return hrC;
}

//******************************************************************************
HRESULT CCDevice8::GetMaterial(D3DMATERIAL8* pmaterial) {

    D3DMATERIAL8    material;
    HRESULT         hrC, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    hrC = m_pd3dd->GetMaterial(pmaterial);

    if (m_pClient->CallMethod(MID_DEV8_GetMaterial, this, &hrS, &material, sizeof(D3DMATERIAL8))) {
        if (hrC != hrS) {
            Log(LOG_WARN, TEXT("IDirect3DDevice8::GetMaterial result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
        }
        else if (SUCCEEDED(hrC)) {
            if (memcmp(pmaterial, &material, sizeof(D3DMATERIAL8))) {
                Log(LOG_WARN, TEXT("IDirect3DDevice8::GetMaterial returned an inconsistent material"));
            }
        }
    }

    return hrC;
}

//******************************************************************************
HRESULT CCDevice8::SetMaterial(D3DMATERIAL8* pmaterial) {

    HRESULT hrC, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    hrC = m_pd3dd->SetMaterial(pmaterial);

    if (m_pClient->CallMethod(MID_DEV8_SetMaterial, this, &hrS, pmaterial, sizeof(D3DMATERIAL8))) {
        if (hrC != hrS) {
            Log(LOG_WARN, TEXT("IDirect3DDevice8::SetMaterial result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
        }
    }

    return hrC;
}

//******************************************************************************
HRESULT CCDevice8::GetTexture(DWORD dwStage, CBaseTexture8** ppTexture) {

    CBaseTexture8*         pTexture;
    LPDIRECT3DBASETEXTURE8 pd3dt;
    HRESULT                hrC, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }

    if (!ppTexture) {
        return D3DERR_INVALIDCALL;
    }
#endif // D3DLOCUS_DEBUG

    *ppTexture = NULL;

    hrC = m_pd3dd->GetTexture(dwStage, &pd3dt);
    if (SUCCEEDED(hrC)) {
        if (!m_pObjectMap->Find((DWORD)pd3dt, (LPDWORD)&pTexture)) {
            return ERR_LOCUSFAIL;
        }
        pTexture->CObject::AddRef();
        *ppTexture = pTexture;
    }
    else {
        pTexture = NULL;
    }

    if (m_pClient->CallMethod(MID_DEV8_GetTexture, this, &hrS, dwStage, &pTexture)) {
        if (hrC != hrS) {
            Log(LOG_WARN, TEXT("IDirect3DDevice8::GetTexture result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
        }
    }

    return hrC;
}

//******************************************************************************
HRESULT CCDevice8::SetTexture(DWORD dwStage, CBaseTexture8* pTexture) {

    LPDIRECT3DBASETEXTURE8  pd3dt = NULL;
    HRESULT                 hrC, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG
    
    if (pTexture) {
        pd3dt = pTexture->GetIDirect3DBaseTexture8();
    }

    hrC = m_pd3dd->SetTexture(dwStage, pd3dt);

    if (m_pClient->CallMethod(MID_DEV8_SetTexture, this, &hrS, dwStage, pTexture)) {
        if (hrC != hrS) {
            Log(LOG_WARN, TEXT("IDirect3DDevice8::SetTexture result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
        }
    }

    return hrC;
}

//******************************************************************************
HRESULT CCDevice8::GetTextureStageState(DWORD dwStage, D3DTEXTURESTAGESTATETYPE txsst, DWORD* pdwValue) {

    DWORD   dwState;
    HRESULT hrC, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    hrC = m_pd3dd->GetTextureStageState(dwStage, txsst, pdwValue);

    if (m_pClient->CallMethod(MID_DEV8_GetTextureStageState, this, &hrS, dwStage, txsst, &dwState)) {
        if (hrC != hrS) {
            Log(LOG_WARN, TEXT("IDirect3DDevice8::GetTextureStageState result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
        }
        else if (SUCCEEDED(hrC)) {
            if (*pdwValue != dwState) {
                Log(LOG_WARN, TEXT("IDirect3DDevice8::GetTextureStageState returned an inconsistent state"));
            }
        }
    }

    return hrC;
}

//******************************************************************************
HRESULT CCDevice8::SetTextureStageState(DWORD dwStage, D3DTEXTURESTAGESTATETYPE txsst, DWORD dwValue) {

    HRESULT hrC, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    hrC = m_pd3dd->SetTextureStageState(dwStage, txsst, dwValue);

    if (m_pClient->CallMethod(MID_DEV8_SetTextureStageState, this, &hrS, dwStage, txsst, dwValue)) {
        if (hrC != hrS) {
            Log(LOG_WARN, TEXT("IDirect3DDevice8::SetTextureStageState result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
        }
    }

    return hrC;
}

//******************************************************************************
HRESULT CCDevice8::UpdateTexture(CBaseTexture8* pSourceTexture, CBaseTexture8* pDestinationTexture) {

    LPDIRECT3DBASETEXTURE8 pd3dtSrc = NULL, pd3dtDst = NULL;
    HRESULT                hrC, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

#ifndef UNDER_XBOX

    if (pSourceTexture) {
        pd3dtSrc = pSourceTexture->GetIDirect3DBaseTexture8();
    }
    if (pDestinationTexture) {
        pd3dtDst = pDestinationTexture->GetIDirect3DBaseTexture8();
    }

#ifdef D3DLOCUS_DEBUG
    if (!pd3dtSrc || !pd3dtDst) {
        return D3DERR_INVALIDCALL;
    }
#endif // D3DLOCUS_DEBUG

    hrC = m_pd3dd->UpdateTexture(pd3dtSrc, pd3dtDst);

    if (m_pClient->CallMethod(MID_DEV8_UpdateTexture, this, &hrS, pSourceTexture, pDestinationTexture)) {
        if (hrC != hrS) {
            Log(LOG_WARN, TEXT("IDirect3DDevice8::UpdateTexture result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
        }
    }

    return hrC;

#else

    Log(LOG_FAIL, TEXT("IDirect3DDevice8::UpdateTexture is not implemented on Xbox"));
    return E_NOTIMPL;

#endif // UNDER_XBOX
}

//******************************************************************************
HRESULT CCDevice8::ValidateDevice(DWORD* pdwNumPasses) {

    DWORD   dwNumPasses;
    HRESULT hrC, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

#ifndef UNDER_XBOX

    hrC = m_pd3dd->ValidateDevice(pdwNumPasses);

    if (m_pClient->CallMethod(MID_DEV8_ValidateDevice, this, &hrS, &dwNumPasses)) {
        if (hrC != hrS) {
            Log(LOG_WARN, TEXT("IDirect3DDevice8::ValidateDevice result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
        }
        else if (SUCCEEDED(hrC)) {
            if (*pdwNumPasses != dwNumPasses) {
                Log(LOG_WARN, TEXT("IDirect3DDevice8::ValidateDevice returned an inconsistent number of passes"));
            }
        }
    }

#else

    Log(LOG_FAIL, TEXT("IDirect3DDevice8::ValidateDevice is not implemented on Xbox"));
    return E_NOTIMPL;

#endif // UNDER_XBOX

    return hrC;
}

//******************************************************************************
HRESULT CCDevice8::GetCurrentTexturePalette(UINT* puPaletteNumber) {

    UINT    uPaletteNumber;
    HRESULT hrC, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    hrC = E_NOTIMPL;

    if (m_pClient->CallMethod(MID_DEV8_GetCurrentTexturePalette, this, &hrS, &uPaletteNumber)) {
        if (hrC != hrS) {
            Log(LOG_WARN, TEXT("IDirect3DDevice8::GetCurrentTexturePalette result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
        }
        else if (SUCCEEDED(hrC)) {
            if (*puPaletteNumber != uPaletteNumber) {
                Log(LOG_WARN, TEXT("IDirect3DDevice8::GetCurrentTexturePalette returned an inconsistent palette number"));
            }
        }
    }

    return hrC;
}

//******************************************************************************
HRESULT CCDevice8::SetCurrentTexturePalette(UINT uPaletteNumber) {

    HRESULT hrC, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    hrC = E_NOTIMPL;

    if (m_pClient->CallMethod(MID_DEV8_SetCurrentTexturePalette, this, &hrS, uPaletteNumber)) {
        if (hrC != hrS) {
            Log(LOG_WARN, TEXT("IDirect3DDevice8::SetCurrentTexturePalette result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
        }
    }

    return hrC;
}

//******************************************************************************
HRESULT CCDevice8::GetPaletteEntries(UINT uPaletteNumber, PALETTEENTRY* ppe) {

    PALETTEENTRY ppeS[256];
    HRESULT      hrC, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    hrC = E_NOTIMPL;

    if (m_pClient->CallMethod(MID_DEV8_GetPaletteEntries, this, &hrS, uPaletteNumber, ppeS, 256 * sizeof(PALETTEENTRY))) {
        if (hrC != hrS) {
            Log(LOG_WARN, TEXT("IDirect3DDevice8::GetPaletteEntries result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
        }
        else if (SUCCEEDED(hrC)) {
            if (memcmp(ppe, ppeS, 256 * sizeof(PALETTEENTRY))) {
                Log(LOG_WARN, TEXT("IDirect3DDevice8::GetPaletteEntries returned an inconsistent palette"));
            }
        }
    }

    return hrC;
}

//******************************************************************************
HRESULT CCDevice8::SetPaletteEntries(UINT uPaletteNumber, PALETTEENTRY* ppe) {

    HRESULT hrC, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    hrC = E_NOTIMPL;

    if (m_pClient->CallMethod(MID_DEV8_SetPaletteEntries, this, &hrS, uPaletteNumber, ppe, 256 * sizeof(PALETTEENTRY))) {
        if (hrC != hrS) {
            Log(LOG_WARN, TEXT("IDirect3DDevice8::SetPaletteEntries result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
        }
    }

    return hrC;
}

//******************************************************************************
HRESULT CCDevice8::CreateVertexShader(DWORD* pdwDeclaration, DWORD* pdwFunction, DWORD* pdwHandle, DWORD dwUsage) {

    DWORD   dwDeclSize = 0;
    DWORD   dwFuncSize = 0;
    LPDWORD pdw;
    HRESULT hrC, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    hrC = m_pd3dd->CreateVertexShader(pdwDeclaration, pdwFunction, pdwHandle, dwUsage);

#ifndef UNDER_XBOX

    if (pdwDeclaration) {
        for (pdw = pdwDeclaration, dwDeclSize = 4; *pdw != D3DVSD_END(); pdw++, dwDeclSize += 4);
    }
    if (pdwFunction) {
        for (pdw = pdwFunction, dwFuncSize = 4; *pdw != D3DVS_END(); pdw++, dwFuncSize += 4);
    }

    if (m_pClient->CallMethod(MID_DEV8_CreateVertexShader, this, &hrS, pdwDeclaration, dwDeclSize, pdwFunction, dwFuncSize, pdwHandle, dwUsage)) {
        if (hrC != hrS) {
            Log(LOG_WARN, TEXT("IDirect3DDevice8::CreateVertexShader result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
        }
    }

#endif // !UNDER_XBOX

    return hrC;
}

//******************************************************************************
HRESULT CCDevice8::CreateVertexShaderTok(DWORD* pdwDeclaration, DWORD* pdwFunction, DWORD* pdwHandle, DWORD dwUsage) {

#ifdef UNDER_XBOX
    LPXGBUFFER  pxgbufShader, pxgbufErrors;
#endif // UNDER_XBOX
    DWORD       dwDeclSize = 0;
    DWORD       dwFuncSize = 0;
    LPDWORD     pdw;
    HRESULT     hrC, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

#ifndef UNDER_XBOX

    hrC = CreateVertexShader(pdwDeclaration, pdwFunction, pdwHandle, dwUsage);

#else

    if (!pdwFunction) {
        hrC = m_pd3dd->CreateVertexShader(pdwDeclaration, pdwFunction, pdwHandle, dwUsage);
    }
    else {

        for (pdw = pdwFunction, dwFuncSize = 4; *pdw != D3DVS_END(); pdw++, dwFuncSize += 4);

        hrC = AssembleShader("<token-stream>", (LPVOID)pdwFunction, dwFuncSize, SASM_INPUT_VERTEXSHADER_TOKENS, NULL, 
                            &pxgbufShader, &pxgbufErrors, NULL, NULL, NULL, NULL);
        if (FAILED(hrC)) {
            ResultFailed(hrC, TEXT("AssembleShader"));
            DebugString(TEXT("Assembly errors:\n%S"), (LPSTR)pxgbufErrors->GetBufferPointer());
            pxgbufErrors->Release();
            return hrC;
        }

        pxgbufErrors->Release();

        hrC = m_pd3dd->CreateVertexShader(pdwDeclaration, (LPDWORD)pxgbufShader->GetBufferPointer(), pdwHandle, dwUsage);

        pxgbufShader->Release();
    }

    if (pdwDeclaration) {
        for (pdw = pdwDeclaration, dwDeclSize = 4; *pdw != D3DVSD_END(); pdw++, dwDeclSize += 4);
    }

    if (m_pClient->CallMethod(MID_DEV8_CreateVertexShader, this, &hrS, pdwDeclaration, dwDeclSize, pdwFunction, dwFuncSize, pdwHandle, dwUsage)) {
        if (hrC != hrS) {
            Log(LOG_WARN, TEXT("IDirect3DDevice8::CreateVertexShader result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
        }
    }

#endif // UNDER_XBOX

    return hrC;
}

//******************************************************************************
HRESULT CCDevice8::CreateVertexShaderAsm(DWORD* pdwDeclaration, LPVOID pvSourceCode, UINT uCodeLength, DWORD* pdwHandle, DWORD dwUsage) {

#ifndef UNDER_XBOX
    LPD3DXBUFFER pxbufShader, pxbufErrors;
#else
    LPXGBUFFER   pxgbufShader, pxgbufErrors;
#endif // UNDER_XBOX
    DWORD        dwDeclSize = 0;
    DWORD        dwFuncSize = 0;
    LPDWORD      pdw;
    HRESULT      hrC, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

#ifndef UNDER_XBOX

    hrC = D3DXAssembleShader(pvSourceCode, uCodeLength, 0, NULL, &pxbufShader, &pxbufErrors);
    if (FAILED(hrC)) {
        ResultFailed(hrC, TEXT("D3DXAssembleShader"));
        DebugString(TEXT("Assembly errors:\n%s"), (LPSTR)pxbufErrors->GetBufferPointer());
        pxbufErrors->Release();
        return hrC;
    }

    pxbufErrors->Release();

    hrC = m_pd3dd->CreateVertexShader(pdwDeclaration, (LPDWORD)pxbufShader->GetBufferPointer(), pdwHandle, dwUsage);

    if (pdwDeclaration) {
        for (pdw = pdwDeclaration, dwDeclSize = 4; *pdw != D3DVSD_END(); pdw++, dwDeclSize += 4);
    }
    for (pdw = (LPDWORD)pxbufShader->GetBufferPointer(), dwFuncSize = 4; *pdw != D3DVS_END(); pdw++, dwFuncSize += 4);

    if (m_pClient->CallMethod(MID_DEV8_CreateVertexShader, this, &hrS, pdwDeclaration, dwDeclSize, (LPDWORD)pxbufShader->GetBufferPointer(), dwFuncSize, pdwHandle, dwUsage)) {
        if (hrC != hrS) {
            Log(LOG_WARN, TEXT("IDirect3DDevice8::CreateVertexShader result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
        }
    }

    pxbufShader->Release();

#else

    hrC = AssembleShader("<memory>", pvSourceCode, uCodeLength, 0, NULL, 
                        &pxgbufShader, &pxgbufErrors, NULL, NULL, NULL, NULL);
    if (FAILED(hrC)) {
        ResultFailed(hrC, TEXT("AssembleShader"));
        DebugString(TEXT("Assembly errors:\n%S"), (LPSTR)pxgbufErrors->GetBufferPointer());
        pxgbufErrors->Release();
        return hrC;
    }

    pxgbufErrors->Release();

    hrC = m_pd3dd->CreateVertexShader(pdwDeclaration, (LPDWORD)pxgbufShader->GetBufferPointer(), pdwHandle, dwUsage);

    pxgbufShader->Release();

    hrS = AssembleShader("<memory>", pvSourceCode, uCodeLength, SASM_OUTPUTTOKENS, NULL, 
                        &pxgbufShader, &pxgbufErrors, NULL, NULL, NULL, NULL);
    if (FAILED(hrS)) {
        ResultFailed(hrS, TEXT("AssembleShader(..., SASM_OUTPUTTOKENS, ...)"));
        DebugString(TEXT("Assembly errors:\n%S"), (LPSTR)pxgbufErrors->GetBufferPointer());
        pxgbufErrors->Release();
        return hrC;
    }

    pxgbufErrors->Release();

    if (pdwDeclaration) {
        for (pdw = pdwDeclaration, dwDeclSize = 4; *pdw != D3DVSD_END(); pdw++, dwDeclSize += 4);
    }
    for (pdw = (LPDWORD)pxgbufShader->GetBufferPointer(), dwFuncSize = 4; *pdw != D3DVS_END(); pdw++, dwFuncSize += 4);

    if (m_pClient->CallMethod(MID_DEV8_CreateVertexShader, this, &hrS, pdwDeclaration, dwDeclSize, (LPDWORD)pxgbufShader->GetBufferPointer(), dwFuncSize, pdwHandle, dwUsage)) {
        if (hrC != hrS) {
            Log(LOG_WARN, TEXT("IDirect3DDevice8::CreateVertexShader result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
        }
    }

    pxgbufShader->Release();

#endif // UNDER_XBOX

    return hrC;
}

//******************************************************************************
HRESULT CCDevice8::DeleteVertexShader(DWORD dwHandle) {

    HRESULT hrC, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    hrC = m_pd3dd->DeleteVertexShader(dwHandle);

    if (m_pClient->CallMethod(MID_DEV8_DeleteVertexShader, this, &hrS, dwHandle)) {
        if (hrC != hrS) {
            Log(LOG_WARN, TEXT("IDirect3DDevice8::DeleteVertexShader result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
        }
    }

    return hrC;
}

//******************************************************************************
HRESULT CCDevice8::GetVertexShader(DWORD* pdwHandle) {

    HRESULT hrC, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    hrC = m_pd3dd->GetVertexShader(pdwHandle);

    if (m_pClient->CallMethod(MID_DEV8_GetVertexShader, this, &hrS, pdwHandle)) {
        if (hrC != hrS) {
            Log(LOG_WARN, TEXT("IDirect3DDevice8::GetVertexShader result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
        }
    }

    return hrC;
}

//******************************************************************************
HRESULT CCDevice8::SetVertexShader(DWORD dwHandle) {

    HRESULT hrC, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    hrC = m_pd3dd->SetVertexShader(dwHandle);

    if (m_pClient->CallMethod(MID_DEV8_SetVertexShader, this, &hrS, dwHandle)) {
        if (hrC != hrS) {
            Log(LOG_WARN, TEXT("IDirect3DDevice8::SetVertexShader result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
        }
    }

    return hrC;
}

//******************************************************************************
HRESULT CCDevice8::GetVertexShaderConstant(DWORD dwRegister, void* pvConstantData, DWORD dwConstantCount) {

    LPDWORD pdwData;
    HRESULT hrC, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    hrC = m_pd3dd->GetVertexShaderConstant(dwRegister, pvConstantData, dwConstantCount);

    pdwData = (LPDWORD)MemAlloc32(dwConstantCount * 4 * sizeof(float));
    if (!pdwData) {
        return hrC;
    }

    if (m_pClient->CallMethod(MID_DEV8_GetVertexShaderConstant, this, &hrS, dwRegister, pdwData, dwConstantCount * 4 * sizeof(float), dwConstantCount)) {
        if (hrC != hrS) {
            Log(LOG_WARN, TEXT("IDirect3DDevice8::GetVertexShaderConstant result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
        }
        else if (SUCCEEDED(hrC)) {
            if (memcmp(pvConstantData, pdwData, dwConstantCount * 4 * sizeof(float))) {
                Log(LOG_WARN, TEXT("IDirect3DDevice8::GetVertexShaderConstant returned inconsistent constant data"));
            }
        }
    }

    MemFree32(pdwData);

    return hrC;
}

//******************************************************************************
HRESULT CCDevice8::SetVertexShaderConstant(DWORD dwRegister, void* pvConstantData, DWORD dwConstantCount) {

    HRESULT hrC, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    hrC = m_pd3dd->SetVertexShaderConstant(dwRegister, pvConstantData, dwConstantCount);

    if (m_pClient->CallMethod(MID_DEV8_SetVertexShaderConstant, this, &hrS, dwRegister, pvConstantData, dwConstantCount * 4 * sizeof(float), dwConstantCount)) {
        if (hrC != hrS) {
            Log(LOG_WARN, TEXT("IDirect3DDevice8::SetVertexShaderConstant result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
        }
    }

    return hrC;
}

//******************************************************************************
HRESULT CCDevice8::GetVertexShaderDeclaration(DWORD dwHandle, void* pvData, DWORD* pdwSizeOfData) {

    LPBYTE  pData;
    DWORD   dwSize, dwInitSize;
    HRESULT hrC, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

#ifndef UNDER_XBOX

    dwInitSize = *pdwSizeOfData;
    dwSize = *pdwSizeOfData;

    hrC = m_pd3dd->GetVertexShaderDeclaration(dwHandle, pvData, pdwSizeOfData);

    if (dwInitSize && pvData) {
        pData = (LPBYTE)MemAlloc32(dwInitSize);
        if (!pData) {
            return hrC;
        }
    }
    else {
        pData = 0;
        dwInitSize = 0;
    }

    if (m_pClient->CallMethod(MID_DEV8_GetVertexShaderDeclaration, this, &hrS, dwHandle, pData, dwInitSize, &dwSize)) {
        if (hrC != hrS) {
            Log(LOG_WARN, TEXT("IDirect3DDevice8::GetVertexShaderDeclaration result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
        }
        else if (hrC == D3DERR_MOREDATA) {
            if (*pdwSizeOfData != dwSize) {
                Log(LOG_WARN, TEXT("IDirect3DDevice8::GetVertexShaderDeclaration returned an inconsistent size"));
            }
        }
        else if (SUCCEEDED(hrC)) {
            if (memcmp(pvData, pData, dwInitSize)) {
                Log(LOG_WARN, TEXT("IDirect3DDevice8::GetVertexShaderDeclaration returned an inconsistent declaration"));
            }
        }
    }

    if (dwInitSize) {
        MemFree32(pData);
    }

#else

    Log(LOG_FAIL, TEXT("IDirect3DDevice8::GetVertexShaderDeclaration is not implemented on Xbox"));
    hrC = E_NOTIMPL;

#endif // UNDER_XBOX

    return hrC;
}

//******************************************************************************
HRESULT CCDevice8::GetVertexShaderFunction(DWORD dwHandle, void* pvData, DWORD* pdwSizeOfData) {

    LPBYTE  pData;
    DWORD   dwSize, dwInitSize;
    HRESULT hrC, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

#ifndef UNDER_XBOX

    dwInitSize = *pdwSizeOfData;
    dwSize = *pdwSizeOfData;

    hrC = m_pd3dd->GetVertexShaderFunction(dwHandle, pvData, pdwSizeOfData);

    if (dwInitSize && pvData) {
        pData = (LPBYTE)MemAlloc32(dwInitSize);
        if (!pData) {
            return hrC;
        }
    }
    else {
        pData = 0;
        dwInitSize = 0;
    }

    if (m_pClient->CallMethod(MID_DEV8_GetVertexShaderFunction, this, &hrS, dwHandle, pData, dwInitSize, &dwSize)) {
        if (hrC != hrS) {
            Log(LOG_WARN, TEXT("IDirect3DDevice8::GetVertexShaderFunction result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
        }
        else if (hrC == D3DERR_MOREDATA) {
            if (*pdwSizeOfData != dwSize) {
                Log(LOG_WARN, TEXT("IDirect3DDevice8::GetVertexShaderFunction returned an inconsistent size"));
            }
        }
        else if (SUCCEEDED(hrC)) {
            if (memcmp(pvData, pData, dwInitSize)) {
                Log(LOG_WARN, TEXT("IDirect3DDevice8::GetVertexShaderFunction returned an inconsistent function"));
            }
        }
    }

    if (dwInitSize) {
        MemFree32(pData);
    }

#else

    Log(LOG_FAIL, TEXT("IDirect3DDevice8::GetVertexShaderFunction is not implemented on Xbox"));
    hrC = E_NOTIMPL;

#endif // UNDER_XBOX

    return hrC;
}

//******************************************************************************
#ifndef UNDER_XBOX
HRESULT CCDevice8::CreatePixelShader(DWORD* pdwFunction, DWORD* pdwHandle) {
#else
HRESULT CCDevice8::CreatePixelShader(const D3DPIXELSHADERDEF* pd3dpsdFunction, DWORD* pdwHandle) {
#endif // UNDER_XBOX

    LPDWORD pdw;
    DWORD   dwFuncSize;
    HRESULT hrC, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

#ifndef UNDER_XBOX

    hrC = m_pd3dd->CreatePixelShader(pdwFunction, pdwHandle);

    for (pdw = pdwFunction, dwFuncSize = 4; *pdw != D3DPS_END(); pdw++, dwFuncSize += 4);

    if (m_pClient->CallMethod(MID_DEV8_CreatePixelShader, this, &hrS, pdwFunction, dwFuncSize, pdwHandle)) {
        if (hrC != hrS) {
            Log(LOG_WARN, TEXT("IDirect3DDevice8::CreatePixelShader result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
        }
    }

#else

    hrC = m_pd3dd->CreatePixelShader(pd3dpsdFunction, pdwHandle);

    Log(LOG_WARN, TEXT("CCDevice8::CreatePixelShader called without specifying token function.  Server pixel shader will not be created"));

#endif // UNDER_XBOX

    return hrC;
}

//******************************************************************************
HRESULT CCDevice8::CreatePixelShaderTok(DWORD* pdwFunction, DWORD* pdwHandle) {

#ifdef UNDER_XBOX
    LPXGBUFFER  pxgbufShader, pxgbufErrors;
#endif // UNDER_XBOX
    DWORD       dwDeclSize = 0;
    DWORD       dwFuncSize = 0;
    LPDWORD     pdw;
    HRESULT     hrC, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

#ifndef UNDER_XBOX

    hrC = CreatePixelShader(pdwFunction, pdwHandle);

#else

    for (pdw = pdwFunction, dwFuncSize = 4; *pdw != D3DPS_END(); pdw++, dwFuncSize += 4);

    hrC = AssembleShader("<token-stream>", (LPVOID)pdwFunction, dwFuncSize, SASM_INPUT_PIXELSHADER_TOKENS, NULL, 
                        &pxgbufShader, &pxgbufErrors, NULL, NULL, NULL, NULL);
    if (FAILED(hrC)) {
        ResultFailed(hrC, TEXT("AssembleShader"));
        DebugString(TEXT("Assembly errors:\n%S"), (LPSTR)pxgbufErrors->GetBufferPointer());
        pxgbufErrors->Release();
        return hrC;
    }

    pxgbufErrors->Release();

    hrC = m_pd3dd->CreatePixelShader((D3DPIXELSHADERDEF*)pxgbufShader->GetBufferPointer(), pdwHandle);

    pxgbufShader->Release();

    if (m_pClient->CallMethod(MID_DEV8_CreatePixelShader, this, &hrS, pdwFunction, dwFuncSize, pdwHandle)) {
        if (hrC != hrS) {
            Log(LOG_WARN, TEXT("IDirect3DDevice8::CreatePixelShader result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
        }
    }

#endif // UNDER_XBOX

    return hrC;
}

//******************************************************************************
HRESULT CCDevice8::CreatePixelShaderAsm(LPVOID pvSourceCode, UINT uCodeLength, DWORD* pdwHandle) {

#ifndef UNDER_XBOX
    LPD3DXBUFFER pxbufShader, pxbufErrors;
#else
    LPXGBUFFER   pxgbufShader, pxgbufErrors;
#endif // UNDER_XBOX
    DWORD        dwFuncSize = 0;
    LPDWORD      pdw;
    HRESULT      hrC, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

#ifndef UNDER_XBOX

    hrC = D3DXAssembleShader(pvSourceCode, uCodeLength, 0, NULL, &pxbufShader, &pxbufErrors);
    if (FAILED(hrC)) {
        ResultFailed(hrC, TEXT("D3DXAssembleShader"));
        DebugString(TEXT("Assembly errors:\n%s"), (LPSTR)pxbufErrors->GetBufferPointer());
        pxbufErrors->Release();
        return hrC;
    }

    pxbufErrors->Release();

    hrC = m_pd3dd->CreatePixelShader((LPDWORD)pxbufShader->GetBufferPointer(), pdwHandle);

    for (pdw = (LPDWORD)pxbufShader->GetBufferPointer(), dwFuncSize = 4; *pdw != D3DPS_END(); pdw++, dwFuncSize += 4);

    if (m_pClient->CallMethod(MID_DEV8_CreatePixelShader, this, &hrS, (LPDWORD)pxbufShader->GetBufferPointer(), dwFuncSize, pdwHandle)) {
        if (hrC != hrS) {
            Log(LOG_WARN, TEXT("IDirect3DDevice8::CreatePixelShader result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
        }
    }

    pxbufShader->Release();

#else

    hrC = AssembleShader("<memory>", pvSourceCode, uCodeLength, 0, NULL, 
                        &pxgbufShader, &pxgbufErrors, NULL, NULL, NULL, NULL);
    if (FAILED(hrC)) {
        ResultFailed(hrC, TEXT("AssembleShader"));
        DebugString(TEXT("Assembly errors:\n%S"), (LPSTR)pxgbufErrors->GetBufferPointer());
        pxgbufErrors->Release();
        return hrC;
    }

    pxgbufErrors->Release();

    hrC = m_pd3dd->CreatePixelShader((D3DPIXELSHADERDEF*)pxgbufShader->GetBufferPointer(), pdwHandle);

    pxgbufShader->Release();

    hrS = AssembleShader("<memory>", pvSourceCode, uCodeLength, SASM_OUTPUTTOKENS, NULL, 
                        &pxgbufShader, &pxgbufErrors, NULL, NULL, NULL, NULL);
    if (FAILED(hrS)) {
        ResultFailed(hrS, TEXT("AssembleShader(..., SASM_OUTPUTTOKENS, ...)"));
        DebugString(TEXT("Assembly errors:\n%S"), (LPSTR)pxgbufErrors->GetBufferPointer());
        pxgbufErrors->Release();
        return hrC;
    }

    pxgbufErrors->Release();

    for (pdw = (LPDWORD)pxgbufShader->GetBufferPointer(), dwFuncSize = 4; *pdw != D3DPS_END(); pdw++, dwFuncSize += 4);

    if (m_pClient->CallMethod(MID_DEV8_CreatePixelShader, this, &hrS, (LPDWORD)pxgbufShader->GetBufferPointer(), dwFuncSize, pdwHandle)) {
        if (hrC != hrS) {
            Log(LOG_WARN, TEXT("IDirect3DDevice8::CreatePixelShader result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
        }
    }

    pxgbufShader->Release();

#endif // UNDER_XBOX

    return hrC;
}

//******************************************************************************
HRESULT CCDevice8::DeletePixelShader(DWORD dwHandle) {

    HRESULT hrC, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    hrC = m_pd3dd->DeletePixelShader(dwHandle);

    if (m_pClient->CallMethod(MID_DEV8_DeletePixelShader, this, &hrS, dwHandle)) {
        if (hrC != hrS) {
            Log(LOG_WARN, TEXT("IDirect3DDevice8::DeletePixelShader result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
        }
    }

    return hrC;
}

//******************************************************************************
HRESULT CCDevice8::GetPixelShader(DWORD* pdwHandle) {

    HRESULT hrC, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    hrC = m_pd3dd->GetPixelShader(pdwHandle);

    if (m_pClient->CallMethod(MID_DEV8_GetPixelShader, this, &hrS, pdwHandle)) {
        if (hrC != hrS) {
            Log(LOG_WARN, TEXT("IDirect3DDevice8::GetPixelShader result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
        }
    }

    return hrC;
}

//******************************************************************************
HRESULT CCDevice8::SetPixelShader(DWORD dwHandle) {

    HRESULT hrC, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    hrC = m_pd3dd->SetPixelShader(dwHandle);

    if (m_pClient->CallMethod(MID_DEV8_SetPixelShader, this, &hrS, dwHandle)) {
        if (hrC != hrS) {
            Log(LOG_WARN, TEXT("IDirect3DDevice8::SetPixelShader result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
        }
    }

    return hrC;
}

//******************************************************************************
HRESULT CCDevice8::GetPixelShaderConstant(DWORD dwRegister, void* pvConstantData, DWORD dwConstantCount) {

    LPDWORD pdwData;
    HRESULT hrC, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    hrC = m_pd3dd->GetPixelShaderConstant(dwRegister, pvConstantData, dwConstantCount);

    pdwData = (LPDWORD)MemAlloc32(dwConstantCount * 4 * sizeof(float));
    if (!pdwData) {
        return hrC;
    }

    if (m_pClient->CallMethod(MID_DEV8_GetPixelShaderConstant, this, &hrS, dwRegister, pdwData, dwConstantCount * 4 * sizeof(float), dwConstantCount)) {
        if (hrC != hrS) {
            Log(LOG_WARN, TEXT("IDirect3DDevice8::GetPixelShaderConstant result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
        }
        else if (SUCCEEDED(hrC)) {
            if (memcmp(pvConstantData, pdwData, dwConstantCount * 4 * sizeof(float))) {
                Log(LOG_WARN, TEXT("IDirect3DDevice8::GetPixelShaderConstant returned inconsistent constant data"));
            }
        }
    }

    MemFree32(pdwData);

    return hrC;
}

//******************************************************************************
HRESULT CCDevice8::SetPixelShaderConstant(DWORD dwRegister, void* pvConstantData, DWORD dwConstantCount) {

    HRESULT hrC, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    hrC = m_pd3dd->SetPixelShaderConstant(dwRegister, pvConstantData, dwConstantCount);

    if (m_pClient->CallMethod(MID_DEV8_SetPixelShaderConstant, this, &hrS, dwRegister, pvConstantData, dwConstantCount * 4 * sizeof(float), dwConstantCount)) {
        if (hrC != hrS) {
            Log(LOG_WARN, TEXT("IDirect3DDevice8::SetPixelShaderConstant result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
        }
    }

    return hrC;
}

//******************************************************************************
#ifndef UNDER_XBOX
HRESULT CCDevice8::GetPixelShaderFunction(DWORD dwHandle, void* pvData, DWORD* pdwSizeOfData) {
#else
HRESULT CCDevice8::GetPixelShaderFunction(DWORD dwHandle, D3DPIXELSHADERDEF* pd3dpsd) {
#endif // UNDER_XBOX

    LPBYTE  pData;
    DWORD   dwSize, dwInitSize;
    HRESULT hrC, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

#ifndef UNDER_XBOX

    dwInitSize = *pdwSizeOfData;
    dwSize = *pdwSizeOfData;

    hrC = m_pd3dd->GetPixelShaderFunction(dwHandle, pvData, pdwSizeOfData);

    if (dwInitSize && pvData) {
        pData = (LPBYTE)MemAlloc32(dwInitSize);
        if (!pData) {
            return hrC;
        }
    }
    else {
        pData = 0;
        dwInitSize = 0;
    }

    if (m_pClient->CallMethod(MID_DEV8_GetPixelShaderFunction, this, &hrS, dwHandle, pData, dwInitSize, &dwSize)) {
        if (hrC != hrS) {
            Log(LOG_WARN, TEXT("IDirect3DDevice8::GetPixelShaderFunction result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
        }
        else if (hrC == D3DERR_MOREDATA) {
            if (*pdwSizeOfData != dwSize) {
                Log(LOG_WARN, TEXT("IDirect3DDevice8::GetPixelShaderFunction returned an inconsistent size"));
            }
        }
        else if (SUCCEEDED(hrC)) {
            if (memcmp(pvData, pData, dwInitSize)) {
                Log(LOG_WARN, TEXT("IDirect3DDevice8::GetPixelShaderFunction returned an inconsistent function"));
            }
        }
    }

    if (dwInitSize) {
        MemFree32(pData);
    }

#else

    hrC = m_pd3dd->GetPixelShaderFunction(dwHandle, pd3dpsd);

#endif // UNDER_XBOX

    return hrC;
}

/*
#ifdef UNDER_XBOX
//******************************************************************************
HRESULT CCDevice8::GetPixelShaderFunction(DWORD dwHandle, D3DPIXELSHADERDEF* pd3dpsd, void* pvData, DWORD* pdwSizeOfData) {

    LPBYTE  pData;
    DWORD   dwSize, dwInitSize;
    HRESULT hrC, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    dwInitSize = *pdwSizeOfData;
    dwSize = *pdwSizeOfData;

    hrC = m_pd3dd->GetPixelShaderFunction(dwHandle, pd3dpsd);

    if (dwInitSize && pvData) {
        pData = (LPBYTE)MemAlloc32(dwInitSize);
        if (!pData) {
            return hrC;
        }
    }
    else {
        pData = 0;
        dwInitSize = 0;
    }

    if (m_pClient->CallMethod(MID_DEV8_GetPixelShaderFunction, this, &hrS, dwHandle, pData, dwInitSize, &dwSize)) {
        if (hrC != hrS) {
            Log(LOG_WARN, TEXT("IDirect3DDevice8::GetPixelShaderFunction result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
        }
        else if (hrC == D3DERR_MOREDATA) {
            if (*pdwSizeOfData != dwSize) {
                Log(LOG_WARN, TEXT("IDirect3DDevice8::GetPixelShaderFunction returned an inconsistent size"));
            }
        }
        else if (SUCCEEDED(hrC)) {
            if (memcmp(pvData, pData, dwInitSize)) {
                Log(LOG_WARN, TEXT("IDirect3DDevice8::GetPixelShaderFunction returned an inconsistent function"));
            }
        }
    }

    if (dwInitSize) {
        MemFree32(pData);
    }

    return hrC;
}
#endif // UNDER_XBOX
*/

//******************************************************************************
#ifndef UNDER_XBOX
HRESULT CCDevice8::Present(RECT* prectSrc, RECT* prectDst, HWND hDestWindowOverride, RGNDATA* prgnDirtyRegion)
#else
HRESULT CCDevice8::Present(RECT* prectSrc, RECT* prectDst, void* hDestWindowOverride, void* prgnDirtyRegion)
#endif // UNDER_XBOX
{
    static RECT     rectNULL = {-1, -1, -1, -1};
    static RGNDATA  rgnNULL = {{sizeof(RGNDATAHEADER), RDH_RECTANGLES, 0, 0, {-1, -1, -1, -1}}, '\0'};
    HRESULT         hrC, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    if (m_uShowVerification) {
        OverlayVerificationResults();
    }

    hrC = m_pd3dd->Present(prectSrc, prectDst, hDestWindowOverride, prgnDirtyRegion);

    if (!prectSrc) {
        prectSrc = &rectNULL;
    }
    if (!prectDst) {
        prectDst = &rectNULL;
    }
#ifndef UNDER_XBOX
    if (!prgnDirtyRegion) {
        prgnDirtyRegion = &rgnNULL;
    }
#else
    prgnDirtyRegion = &rgnNULL;
#endif // UNDER_XBOX

    if (m_pClient->CallMethod(MID_DEV8_Present, this, &hrS, prectSrc, sizeof(RECT), prectDst, sizeof(RECT), hDestWindowOverride, prgnDirtyRegion, sizeof(RGNDATA))) {
        if (hrC != hrS) {
            Log(LOG_WARN, TEXT("IDirect3DDevice8::Present result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
        }
    }

    m_uFramesPresented++;

    // Verify the frame
    if (m_bVerifyFrames && m_pClient->IsConnected()) {

        LPDIRECT3DSURFACE8 pd3ds;
        BOOL bPerPixelCompare = TRUE;

        // Temporarily disable remote method calls
        m_pClient->EnableMethodCalls(FALSE);

#ifndef UNDER_XBOX
        pd3ds = GetFrontBufferContents();
#else
        m_pd3dd->GetBackBuffer(-1, D3DBACKBUFFER_TYPE_MONO, &pd3ds);
#endif // UNDER_XBOX

        if (m_bServerAffinity && !(BOOL)m_uShowVerification) {

            DWORD dwSrcCRC, dwRefCRC;

            dwSrcCRC = ComputeSurfaceCRC32(pd3ds);

            if (!m_pClient->GetRefFrameCRC32(this, &dwRefCRC)) {
                Log(LOG_FAIL, TEXT("Unable to obtain the CRC of the reference frame for image validation"));
                m_fFrameMatch = 0.0f;
            }
            else {
                if (dwSrcCRC == dwRefCRC) {
                    m_fFrameMatch = 1.0f;
                    bPerPixelCompare = FALSE;
                }
            }
        }

        if (bPerPixelCompare) {

            LPDIRECT3DSURFACE8  pd3dsRef;
            D3DSURFACE_DESC     d3dsd;
            D3DLOCKED_RECT      d3dlr;
            BOOL                bResult;
            HRESULT             hr;

            pd3ds->GetDesc(&d3dsd);

#ifndef UNDER_XBOX
            hr = m_pd3dd->CreateImageSurface(d3dsd.Width, d3dsd.Height, D3DFMT_X8R8G8B8, &pd3dsRef);
#else
            hr = m_pd3dd->CreateImageSurface(d3dsd.Width, d3dsd.Height, D3DFMT_LIN_X8R8G8B8, &pd3dsRef);
#endif // UNDER_XBOX
            if (ResultFailed(hr, TEXT("IDirect3DDevice8::CreateImageSurface"))) {
                Log(LOG_FAIL, TEXT("Unable to obtain the contents of the reference frame for image validation"));
                m_fFrameMatch = 0.0f;
            }
            else {

                pd3dsRef->LockRect(&d3dlr, NULL, 0);
                bResult = m_pClient->GetRefFrame(this, d3dsd.Width, d3dsd.Height, d3dlr.Pitch, d3dlr.pBits);
                pd3dsRef->UnlockRect();
                if (!bResult) {
                    Log(LOG_FAIL, TEXT("Unable to obtain the contents of the reference frame for image validation"));
                    m_fFrameMatch = 0.0f;
                }
                else {

                    float fThreshold;

                    m_pClient->EnableMethodCalls(TRUE);

                    if (m_pd3dtFrameVar) {
                        CSurface8* pd3dsFrameVar;
                        m_pd3dtFrameVar->GetSurfaceLevel(0, &pd3dsFrameVar);
                        m_fFrameMatch = CompareFrames(pd3ds, pd3dsRef, pd3dsFrameVar->GetIDirect3DSurface8());
                        pd3dsFrameVar->LockRect(&d3dlr, NULL, 0);
                        pd3dsFrameVar->UnlockRect();
                        pd3dsFrameVar->Release();
                    }
                    else {
                        m_fFrameMatch = CompareFrames(pd3ds, pd3dsRef, NULL);
                    }

                    if (m_bServerAffinity) {
                        fThreshold = 1.0f;
                    }
                    else {
                        fThreshold = m_fThreshold;
                    }
                    if (m_fFrameMatch < fThreshold) {

                        TCHAR szFile[128];
                        LPDIRECT3DSURFACE8 pd3dsVariance;

                        Log(LOG_FAIL, TEXT("The composition of frame %d differs from the composition of the reference frame (%3.3f variance)"), m_uFramesPresented, 1.0f - m_fFrameMatch);

                        GetModuleName(GetTestInstance(), szFile, 128);
                        wsprintf(szFile + _tcslen(szFile), TEXT("_%d_s"), m_uFramesPresented);
                        CaptureSurface(pd3ds, szFile, TRUE);
                        *(szFile + _tcslen(szFile) - 1) = TEXT('r');
                        CaptureSurface(pd3dsRef, szFile);
                        m_pClient->EnableMethodCalls(FALSE);
#ifndef UNDER_XBOX
                        hr = m_pd3dd->CreateImageSurface(d3dsd.Width, d3dsd.Height, D3DFMT_X8R8G8B8, &pd3dsVariance);
#else
                        hr = m_pd3dd->CreateImageSurface(d3dsd.Width, d3dsd.Height, D3DFMT_LIN_X8R8G8B8, &pd3dsVariance);
#endif // UNDER_XBOX
                        if (SUCCEEDED(hr)) {
                            CompareFrames(pd3ds, pd3dsRef, pd3dsVariance);
                            *(szFile + _tcslen(szFile) - 1) = TEXT('v');
                            CaptureSurface(pd3dsVariance, szFile);
                            pd3dsVariance->Release();
                        }
                        m_pClient->EnableMethodCalls(TRUE);
                    }

                    if (m_pd3dtFrameSrc) {
                        UpdateFBTexture(m_pd3dtFrameSrc, pd3ds);
                    }

                    if (m_pd3dtFrameRef) {
                        UpdateFBTexture(m_pd3dtFrameRef, pd3dsRef);
                    }

                    m_pClient->EnableMethodCalls(FALSE);
                }

                pd3dsRef->Release();
            }
        }

        pd3ds->Release();

        // Restore remote method calls
        m_pClient->EnableMethodCalls(TRUE);
    }

    return hrC;
}

//******************************************************************************
HRESULT CCDevice8::Reset(D3DPRESENT_PARAMETERS* pd3dpp) {

#ifndef UNDER_XBOX
    LPDIRECT3DSURFACE8 pd3ds;
    D3DSURFACE_DESC    d3dsd;
#endif
    HRESULT hrC, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    hrC = m_pd3dd->Reset(pd3dpp);

    if (m_pClient->CallMethod(MID_DEV8_Reset, this, &hrS, pd3dpp, sizeof (D3DPRESENT_PARAMETERS))) {
        if (hrC != hrS) {
            Log(LOG_WARN, TEXT("IDirect3DDevice8::Reset result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
        }
    }

    return hrC;
}

//******************************************************************************
HRESULT CCDevice8::BeginScene() {

    HRESULT hrC, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    hrC = m_pd3dd->BeginScene();

    if (m_pClient->CallMethod(MID_DEV8_BeginScene, this, &hrS)) {
        if (hrC != hrS) {
            Log(LOG_WARN, TEXT("IDirect3DDevice8::BeginScene result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
        }
    }

    return hrC;
}

//******************************************************************************
HRESULT CCDevice8::EndScene() {

    HRESULT hrC, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

// ##DELETE
// ##HACK to make diffuse modulated blending work on Inspiron 5000
#ifndef UNDER_XBOX
    typedef struct _DUMMYVERTEX {
        D3DXVECTOR3 vPosition;
        float       fRHW;
        D3DCOLOR    cDiffuse;
        D3DCOLOR    cSpecular;
        float       u0, v0;

        _DUMMYVERTEX() {}
        _DUMMYVERTEX(const D3DVECTOR& v, float _fRHW, D3DCOLOR _cDiffuse, 
                  D3DCOLOR _cSpecular, float _u0, float _v0) 
        { 
            vPosition.x = v.x; vPosition.y = v.y; vPosition.z = v.z; fRHW = _fRHW;
            cDiffuse = _cDiffuse; cSpecular = _cSpecular;
            u0 = _u0; v0 = _v0;
        }
    } DUMMYVERTEX;
    DUMMYVERTEX rDummy[3];
    rDummy[0] = DUMMYVERTEX(D3DXVECTOR3(0.0f, (float)0.0f, 0.000009f), 110000.0f, 0xFF000000, 0, 0.0f, 1.0f);
    rDummy[1] = DUMMYVERTEX(D3DXVECTOR3(0.0f, 0.0f, 0.000009f), 110000.0f, 0xFF000000, 0, 0.0f, 0.0f);
    rDummy[2] = DUMMYVERTEX(D3DXVECTOR3((float)0.0f, 0.0f, 0.000009f), 110000.0f, 0xFF000000, 0, 1.0f, 0.0f);
    m_pd3dd->SetVertexShader(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX1);
    m_pd3dd->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG2);
    m_pd3dd->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
    m_pd3dd->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2);
    m_pd3dd->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
    m_pd3dd->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 1, rDummy, sizeof(DUMMYVERTEX));
    m_pd3dd->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
    m_pd3dd->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
    m_pd3dd->SetVertexShader(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1);
#endif // !UNDER_XBOX
// ##END HACK
// ##END DELETE

    hrC = m_pd3dd->EndScene();

    if (m_pClient->CallMethod(MID_DEV8_EndScene, this, &hrS)) {
        if (hrC != hrS) {
            Log(LOG_WARN, TEXT("IDirect3DDevice8::EndScene result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
        }
    }

    return hrC;
}

//******************************************************************************
HRESULT CCDevice8::DrawIndexedPrimitive(D3DPRIMITIVETYPE d3dpt, UINT uMinVertexToShade, UINT uNumVerticesToShade, UINT uStartIndex, UINT uPrimitiveCount) {

    HRESULT hrC, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    hrC = m_pd3dd->DrawIndexedPrimitive(d3dpt, uMinVertexToShade, uNumVerticesToShade, uStartIndex, uPrimitiveCount);

    if (m_pClient->CallMethod(MID_DEV8_DrawIndexedPrimitive, this, &hrS, d3dpt, uMinVertexToShade, uNumVerticesToShade, uStartIndex, uPrimitiveCount)) {
        if (hrC != hrS) {
            Log(LOG_WARN, TEXT("IDirect3DDevice8::DrawIndexedPrimitive result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
        }
    }

    return hrC;
}

//******************************************************************************
HRESULT CCDevice8::DrawIndexedPrimitiveUP(D3DPRIMITIVETYPE d3dpt, UINT uMinVertexToShade, UINT uNumVerticesToShade, UINT uPrimitiveCount, void* pvIndices, D3DFORMAT fmtIndex, void* pvVertices, UINT uStride) {

    UINT    uNumIndices;
    HRESULT hrC, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    hrC = m_pd3dd->DrawIndexedPrimitiveUP(d3dpt, uMinVertexToShade, uNumVerticesToShade, uPrimitiveCount, pvIndices, fmtIndex, pvVertices, uStride);

    switch (d3dpt) {
        case D3DPT_POINTLIST:
            uNumIndices = uPrimitiveCount;
            break;
        case D3DPT_LINELIST:
            uNumIndices = uPrimitiveCount * 2;
            break;
        case D3DPT_LINESTRIP:
            uNumIndices = uPrimitiveCount + 1;
            break;
        case D3DPT_TRIANGLELIST:
            uNumIndices = uPrimitiveCount * 3;
            break;
        case D3DPT_TRIANGLESTRIP:
        case D3DPT_TRIANGLEFAN:
            uNumIndices = uPrimitiveCount + 2;
            break;
    }

    if (m_pClient->CallMethod(MID_DEV8_DrawIndexedPrimitiveUP, this, &hrS, d3dpt, uMinVertexToShade, uNumVerticesToShade, uPrimitiveCount, pvIndices, uNumIndices * (fmtIndex == D3DFMT_INDEX16 ? 2 : 4), fmtIndex, pvVertices, (uMinVertexToShade + uNumVerticesToShade) * uStride, uStride)) {
        if (hrC != hrS) {
            Log(LOG_WARN, TEXT("IDirect3DDevice8::DrawIndexedPrimitiveUP result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
        }
    }

    return hrC;
}

//******************************************************************************
HRESULT CCDevice8::DrawPrimitive(D3DPRIMITIVETYPE d3dpt, UINT uStartVertex, UINT uPrimitiveCount) {

    HRESULT hrC, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    hrC = m_pd3dd->DrawPrimitive(d3dpt, uStartVertex, uPrimitiveCount);

    if (m_pClient->CallMethod(MID_DEV8_DrawPrimitive, this, &hrS, d3dpt, uStartVertex, uPrimitiveCount)) {
        if (hrC != hrS) {
            Log(LOG_WARN, TEXT("IDirect3DDevice8::DrawPrimitive result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
        }
    }

    return hrC;
}

//******************************************************************************
HRESULT CCDevice8::DrawPrimitiveUP(D3DPRIMITIVETYPE d3dpt, UINT uPrimitiveCount, void* pvVertices, UINT uStride) {

    UINT    uNumVertices;
    HRESULT hrC, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    hrC = m_pd3dd->DrawPrimitiveUP(d3dpt, uPrimitiveCount, pvVertices, uStride);

    switch (d3dpt) {
        case D3DPT_POINTLIST:
            uNumVertices = uPrimitiveCount;
            break;
        case D3DPT_LINELIST:
            uNumVertices = uPrimitiveCount * 2;
            break;
        case D3DPT_LINESTRIP:
            uNumVertices = uPrimitiveCount + 1;
            break;
        case D3DPT_TRIANGLELIST:
            uNumVertices = uPrimitiveCount * 3;
            break;
        case D3DPT_TRIANGLESTRIP:
        case D3DPT_TRIANGLEFAN:
            uNumVertices = uPrimitiveCount + 2;
            break;
    }

    if (m_pClient->CallMethod(MID_DEV8_DrawPrimitiveUP, this, &hrS, d3dpt, uPrimitiveCount, pvVertices, uNumVertices * uStride, uStride)) {
        if (hrC != hrS) {
            Log(LOG_WARN, TEXT("IDirect3DDevice8::DrawPrimitiveUP result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
        }
    }

    return hrC;
}

//******************************************************************************
HRESULT CCDevice8::DeletePatch(UINT uHandle) {

    HRESULT hrC, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    hrC = m_pd3dd->DeletePatch(uHandle);

    if (m_pClient->CallMethod(MID_DEV8_DeletePatch, this, &hrS, uHandle)) {
        if (hrC != hrS) {
            Log(LOG_WARN, TEXT("IDirect3DDevice8::DeletePatch result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
        }
    }

    return hrC;
}

//******************************************************************************
HRESULT CCDevice8::DrawRectPatch(UINT uHandle, float* pfNumSegs, D3DRECTPATCH_INFO* prpi) {

    float   fNumSegs;
    HRESULT hrC, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    fNumSegs = *pfNumSegs;

    hrC = m_pd3dd->DrawRectPatch(uHandle, pfNumSegs, prpi);

    if (m_pClient->CallMethod(MID_DEV8_DrawRectPatch, this, &hrS, uHandle, &fNumSegs, prpi, sizeof(D3DRECTPATCH_INFO))) {
        if (hrC != hrS) {
            Log(LOG_WARN, TEXT("IDirect3DDevice8::DrawRectPatch result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
        }
        else if (SUCCEEDED(hrC)) {
            if (*pfNumSegs != fNumSegs) {
                Log(LOG_WARN, TEXT("IDirect3DDevice8::DrawRectPatch returned an inconsistent number of segments"));
            }
        }
    }

    return hrC;
}

//******************************************************************************
HRESULT CCDevice8::DrawTriPatch(UINT uHandle, float* pfNumSegs, D3DTRIPATCH_INFO* ptsi) {

    float   fNumSegs;
    HRESULT hrC, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    hrC = m_pd3dd->DrawTriPatch(uHandle, pfNumSegs, ptsi);

    if (m_pClient->CallMethod(MID_DEV8_DrawTriPatch, this, &hrS, uHandle, &fNumSegs, ptsi, sizeof(D3DTRIPATCH_INFO))) {
        if (hrC != hrS) {
            Log(LOG_WARN, TEXT("IDirect3DDevice8::DrawTriPatch result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
        }
        else if (SUCCEEDED(hrC)) {
            if (*pfNumSegs != fNumSegs) {
                Log(LOG_WARN, TEXT("IDirect3DDevice8::DrawTriPatch returned an inconsistent number of segments"));
            }
        }
    }

    return hrC;
}

//******************************************************************************
HRESULT CCDevice8::GetStreamSource(UINT uStream, CVertexBuffer8** ppVertexData, UINT* puStride) {

    CVertexBuffer8*         pVertexBuffer;
    LPDIRECT3DVERTEXBUFFER8 pd3dr;
    UINT                    uStride;
    HRESULT                 hrC, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }

    if (!ppVertexData) {
        return D3DERR_INVALIDCALL;
    }
#endif // D3DLOCUS_DEBUG

    *ppVertexData = NULL;

    hrC = m_pd3dd->GetStreamSource(uStream, &pd3dr, puStride);
    if (SUCCEEDED(hrC)) {
        if (!m_pObjectMap->Find((DWORD)pd3dr, (LPDWORD)&pVertexBuffer)) {
            return ERR_LOCUSFAIL;
        }
        pVertexBuffer->CObject::AddRef();
        *ppVertexData = pVertexBuffer;
    }
    else {
        pVertexBuffer = NULL;
    }

    if (m_pClient->CallMethod(MID_DEV8_GetStreamSource, this, &hrS, uStream, &pVertexBuffer, &uStride)) {
        if (hrC != hrS) {
            Log(LOG_WARN, TEXT("IDirect3DDevice8::GetStreamSource result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
        }
        if (SUCCEEDED(hrC)) {
            if (*puStride != uStride) {
                Log(LOG_WARN, TEXT("IDirect3DDevice8::GetStreamSource returned an inconsistent stride"));
            }
        }
    }

    return hrC;
}

//******************************************************************************
HRESULT CCDevice8::SetStreamSource(UINT uStream, CVertexBuffer8* pVertexData, UINT uStride) {

    LPDIRECT3DVERTEXBUFFER8 pd3dr = NULL;
    HRESULT                 hrC, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    if (pVertexData) {
        pd3dr = pVertexData->GetIDirect3DVertexBuffer8();
    }

    hrC = m_pd3dd->SetStreamSource(uStream, pd3dr, uStride);

    if (m_pClient->CallMethod(MID_DEV8_SetStreamSource, this, &hrS, uStream, pVertexData, uStride)) {
        if (hrC != hrS) {
            Log(LOG_WARN, TEXT("IDirect3DDevice8::SetStreamSource result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
        }
    }

    return hrC;
}

//******************************************************************************
HRESULT CCDevice8::GetIndices(CIndexBuffer8** ppIndexData, UINT* puBaseIndex) {

    CIndexBuffer8*         pIndexBuffer;
    LPDIRECT3DINDEXBUFFER8 pd3di;
    UINT                   uBaseIndex;
    HRESULT                hrC, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }

    if (!ppIndexData) {
        return D3DERR_INVALIDCALL;
    }
#endif // D3DLOCUS_DEBUG

    *ppIndexData = NULL;

    hrC = m_pd3dd->GetIndices(&pd3di, puBaseIndex);
    if (SUCCEEDED(hrC)) {
        if (!m_pObjectMap->Find((DWORD)pd3di, (LPDWORD)&pIndexBuffer)) {
            return ERR_LOCUSFAIL;
        }
        pIndexBuffer->CObject::AddRef();
        *ppIndexData = pIndexBuffer;
    }
    else {
        pIndexBuffer = NULL;
    }

    if (m_pClient->CallMethod(MID_DEV8_GetIndices, this, &hrS, &pIndexBuffer, &uBaseIndex)) {
        if (hrC != hrS) {
            Log(LOG_WARN, TEXT("IDirect3DDevice8::GetIndices result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
        }
        if (SUCCEEDED(hrC)) {
            if (*puBaseIndex != uBaseIndex) {
                Log(LOG_WARN, TEXT("IDirect3DDevice8::GetIndices returned an inconsistent base index"));
            }
        }
    }

    return hrC;
}

//******************************************************************************
HRESULT CCDevice8::SetIndices(CIndexBuffer8* pIndexData, UINT uBaseIndex) {

    LPDIRECT3DINDEXBUFFER8 pd3di = NULL;
    HRESULT                hrC, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    if (pIndexData) {
        pd3di = pIndexData->GetIDirect3DIndexBuffer8();
    }

    hrC = m_pd3dd->SetIndices(pd3di, uBaseIndex);

    if (m_pClient->CallMethod(MID_DEV8_SetIndices, this, &hrS, pIndexData, uBaseIndex)) {
        if (hrC != hrS) {
            Log(LOG_WARN, TEXT("IDirect3DDevice8::SetIndices result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
        }
    }

    return hrC;
}

//******************************************************************************
UINT CCDevice8::GetAvailableTextureMem() {

    UINT uMemC, uMemS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return 0;
    }
#endif // D3DLOCUS_DEBUG

#ifndef UNDER_XBOX
    uMemC = m_pd3dd->GetAvailableTextureMem();

    if (m_pClient->CallMethod(MID_DEV8_GetAvailableTextureMem, this, &uMemS)) {
        if (uMemC != uMemS) {
            Log(LOG_WARN, TEXT("IDirect3DDevice8::GetAvailableTextureMem returned an inconsistent amount of available memory"));
        }
    }

    return uMemC;
#else
    Log(LOG_FAIL, TEXT("IDirect3DDevice8::GetAvailableTextureMem is not implemented on XBox"));
    return 0;
#endif // UNDER_XBOX
}

//******************************************************************************
HRESULT CCDevice8::GetCreationParameters(D3DDEVICE_CREATION_PARAMETERS* pd3dcp) {

    D3DDEVICE_CREATION_PARAMETERS   d3dcp;
    HRESULT                         hrC, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

#ifndef DEMO_HACK
    hrC = m_pd3dd->GetCreationParameters(pd3dcp);

    if (m_pClient->CallMethod(MID_DEV8_GetCreationParameters, this, &hrS, &d3dcp, sizeof(D3DDEVICE_CREATION_PARAMETERS))) {
        if (hrC != hrS) {
            Log(LOG_WARN, TEXT("IDirect3DDevice8::GetCreationParameters result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
        }
        else if (SUCCEEDED(hrC)) {
            if (memcmp(pd3dcp, &d3dcp, sizeof(D3DDEVICE_CREATION_PARAMETERS))) {
                Log(LOG_WARN, TEXT("IDirect3DDevice8::GetCreationParameters returned inconsistent parameters"));
            }
        }
    }

    return hrC;
#else
    return E_NOTIMPL;
#endif // DEMO_HACK
}

//******************************************************************************
HRESULT CCDevice8::GetDeviceCaps(D3DCAPS8* pd3dcaps) {
    
    D3DCAPS8 d3dcaps;
    HRESULT  hrC, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    hrC = m_pd3dd->GetDeviceCaps(pd3dcaps);

    if (m_pClient->CallMethod(MID_DEV8_GetDeviceCaps, this, &hrS, &d3dcaps, sizeof(D3DCAPS8))) {
        if (hrC != hrS) {
            Log(LOG_WARN, TEXT("IDirect3DDevice8::GetDeviceCaps result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
        }
        else if (SUCCEEDED(hrC)) {
#ifndef UNDER_XBOX
            if (memcmp(pd3dcaps, &d3dcaps, sizeof(D3DCAPS8))) {
                Log(LOG_WARN, TEXT("IDirect3DDevice8::GetDeviceCaps returned inconsistent capabilities"));
            }
#endif // !UNDER_XBOX
        }
    }

    return hrC;
}

//******************************************************************************
HRESULT CCDevice8::GetDirect3D(CDirect3D8** ppDirect3D8) {

    CDirect3D8* pDirect3D;
    LPDIRECT3D8 pd3d;
    HRESULT     hrC, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
    if (!ppDirect3D8) {
        return D3DERR_INVALIDCALL;
    }
#endif // D3DLOCUS_DEBUG

    *ppDirect3D8 = NULL;

    hrC = m_pd3dd->GetDirect3D(&pd3d);
    if (SUCCEEDED(hrC)) {
        if (pd3d != m_pDirect3D->GetIDirect3D8()) {
            DebugString(TEXT("Interface pointers differ for IDirect3D8"));
            DebugBreak();
        }
        m_pDirect3D->CObject::AddRef();
        *ppDirect3D8 = m_pDirect3D;
        pDirect3D = m_pDirect3D;
    }
    else {
        pDirect3D = NULL;
    }

    if (m_pClient->CallMethod(MID_DEV8_GetDirect3D, this, &hrS, &pDirect3D)) {
        if (hrC != hrS) {
            Log(LOG_WARN, TEXT("IDirect3DDevice8::GetDirect3D result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
        }
    }

    return hrC;
}

//******************************************************************************
HRESULT CCDevice8::GetInfo(DWORD dwDevInfoType, void* pvDevInfo, DWORD dwDevInfoSize) {

    LPBYTE  pDevInfo;
    HRESULT hrC, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

#ifndef UNDER_XBOX

    hrC = m_pd3dd->GetInfo(dwDevInfoType, pvDevInfo, dwDevInfoSize);

    pDevInfo = (LPBYTE)MemAlloc(dwDevInfoSize);
    if (!pDevInfo) {
        return hrC;
    }

    if (m_pClient->CallMethod(MID_DEV8_GetInfo, this, &hrS, dwDevInfoType, pDevInfo, dwDevInfoSize, dwDevInfoSize)) {
        if (hrC != hrS) {
            Log(LOG_WARN, TEXT("IDirect3DDevice8::GetInfo result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
        }
        else if (SUCCEEDED(hrC)) {
            if (memcmp(pvDevInfo, pDevInfo, dwDevInfoSize)) {
                Log(LOG_WARN, TEXT("IDirect3DDevice8::GetInfo returned inconsistent info"));
            }
        }
    }

    MemFree(pDevInfo);

#else

    Log(LOG_FAIL, TEXT("IDirect3DDevice8::GetInfo is not implemented on Xbox"));
    return E_NOTIMPL;

#endif // UNDER_XBOX

    return hrC;
}

//******************************************************************************
HRESULT CCDevice8::GetRasterStatus(D3DRASTER_STATUS* prast) {

    D3DRASTER_STATUS    d3drs;
    HRESULT             hrC, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    hrC = m_pd3dd->GetRasterStatus(prast);

    if (m_pClient->CallMethod(MID_DEV8_GetRasterStatus, this, &hrS, &d3drs, sizeof(D3DRASTER_STATUS))) {
        if (hrC != hrS) {
            Log(LOG_WARN, TEXT("IDirect3DDevice8::GetRasterStatus result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
        }
    }

    return hrC;
}

//******************************************************************************
HRESULT CCDevice8::GetDisplayMode(D3DDISPLAYMODE* pMode) {

    D3DDISPLAYMODE  d3ddm;
    HRESULT         hrC, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    hrC = m_pd3dd->GetDisplayMode(pMode);

    if (m_pClient->CallMethod(MID_DEV8_GetDisplayMode, this, &hrS, &d3ddm, sizeof(D3DDISPLAYMODE))) {
        if (hrC != hrS) {
            Log(LOG_WARN, TEXT("IDirect3DDevice8::GetDisplayMode result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
        }
        else if (SUCCEEDED(hrC)) {
            if (memcmp(pMode, &d3ddm, sizeof(D3DDISPLAYMODE))) {
                Log(LOG_WARN, TEXT("IDirect3DDevice8::GetDisplayMode returned an inconsistent mode"));
            }
        }
    }

    return hrC;
}

//******************************************************************************
HRESULT CCDevice8::GetBackBuffer(UINT uBackBuffer, D3DBACKBUFFER_TYPE bbt, CSurface8** ppBackBuffer) {

    LPDIRECT3DSURFACE8  pd3ds;
    CCSurface8*         pSurface;
    HRESULT             hrC, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }

    if (!ppBackBuffer) {
        return D3DERR_INVALIDCALL;
    }
#endif // D3DLOCUS_DEBUG

    *ppBackBuffer = NULL;

    hrC = m_pd3dd->GetBackBuffer(uBackBuffer, bbt, &pd3ds);
    if (SUCCEEDED(hrC)) {
        
        if (!m_pObjectMap->Find((DWORD)pd3ds, (LPDWORD)&pSurface)) {

            pSurface = new CCSurface8();
            if (!pSurface) {
                pd3ds->Release();
                return E_OUTOFMEMORY;
            }

            if (!pSurface->Create(m_pClient, this, pd3ds)) {
                pd3ds->Release();
                return ERR_LOCUSFAIL;
            }
        }

        pSurface->CObject::AddRef();

        *ppBackBuffer = pSurface;
    }
    else {
        pSurface = NULL;
    }

    if (m_pClient->CallMethod(MID_DEV8_GetBackBuffer, this, &hrS, uBackBuffer, bbt, &pSurface)) {
        if (hrC != hrS) {
            Log(LOG_WARN, TEXT("IDirect3DDevice8::GetBackBuffer result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
        }
    }

    return hrC;
}

//******************************************************************************
HRESULT CCDevice8::GetDepthStencilSurface(CSurface8** ppZStencilSurface) {

    LPDIRECT3DSURFACE8  pd3ds;
    CCSurface8*         pSurface;
    HRESULT             hrC, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }

    if (!ppZStencilSurface) {
        return D3DERR_INVALIDCALL;
    }
#endif // D3DLOCUS_DEBUG

    *ppZStencilSurface = NULL;

    hrC = m_pd3dd->GetDepthStencilSurface(&pd3ds);
    if (SUCCEEDED(hrC)) {
        
        if (!m_pObjectMap->Find((DWORD)pd3ds, (LPDWORD)&pSurface)) {

            pSurface = new CCSurface8();
            if (!pSurface) {
                pd3ds->Release();
                return E_OUTOFMEMORY;
            }

            if (!pSurface->Create(m_pClient, this, pd3ds)) {
                pd3ds->Release();
                return ERR_LOCUSFAIL;
            }
        }

        pSurface->CObject::AddRef();

        *ppZStencilSurface = pSurface;
    }
    else {
        pSurface = NULL;
    }

    if (m_pClient->CallMethod(MID_DEV8_GetDepthStencilSurface, this, &hrS, &pSurface)) {
        if (hrC != hrS) {
            Log(LOG_WARN, TEXT("IDirect3DDevice8::GetDepthStencilSurface result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
        }
    }

    return hrC;
}

//******************************************************************************
void CCDevice8::GetGammaRamp(D3DGAMMARAMP* pgrRamp) {

    D3DGAMMARAMP d3dgr;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return;
    }
#endif // D3DLOCUS_DEBUG

    m_pd3dd->GetGammaRamp(pgrRamp);

    if (m_pClient->CallMethod(MID_DEV8_GetGammaRamp, this, &d3dgr, sizeof(D3DGAMMARAMP))) {
        if (memcmp(pgrRamp, &d3dgr, sizeof(D3DGAMMARAMP))) {
            Log(LOG_WARN, TEXT("IDirect3DDevice8::GetGammaRamp returned an inconsistent ramp"));
        }
    }
}

//******************************************************************************
void CCDevice8::SetGammaRamp(DWORD dwFlags, D3DGAMMARAMP* pgrRamp) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return;
    }
#endif // D3DLOCUS_DEBUG

    m_pd3dd->SetGammaRamp(dwFlags, pgrRamp);

    m_pClient->CallMethod(MID_DEV8_SetGammaRamp, this, dwFlags, pgrRamp, sizeof(D3DGAMMARAMP));
}

//******************************************************************************
HRESULT CCDevice8::CopyRects(CSurface8* pSrcSurface, RECT* prectSrcRects, UINT uNumSrcRects, CSurface8* pDstSurface, POINT* ppntDstPoints) {

    DWORD   dwSize;
    HRESULT hrC, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!pSrcSurface || !pDstSurface) {
        return D3DERR_INVALIDCALL;
    }
    if (!m_pd3dd || !pSrcSurface->GetIDirect3DSurface8() || !pDstSurface->GetIDirect3DSurface8()) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

    hrC = m_pd3dd->CopyRects(pSrcSurface->GetIDirect3DSurface8(), prectSrcRects, uNumSrcRects, pDstSurface->GetIDirect3DSurface8(), ppntDstPoints);

    if (ppntDstPoints) {
        dwSize = uNumSrcRects * sizeof(POINT);
    }
    else {
        dwSize = 0;
    }

    if (m_pClient->CallMethod(MID_DEV8_CopyRects, this, &hrS, pSrcSurface, prectSrcRects, uNumSrcRects * sizeof(RECT), uNumSrcRects, pDstSurface, ppntDstPoints, dwSize)) {
        if (hrC != hrS) {
            Log(LOG_WARN, TEXT("IDirect3DDevice8::CopyRects result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
        }
    }

    return hrC;
}

//******************************************************************************
HRESULT CCDevice8::GetFrontBuffer(CSurface8* pDstSurface) {

    HRESULT hrC, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!pDstSurface) {
        return D3DERR_INVALIDCALL;
    }
    if (!m_pd3dd || !pDstSurface->GetIDirect3DSurface8()) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

#ifndef UNDER_XBOX

    hrC = m_pd3dd->GetFrontBuffer(pDstSurface->GetIDirect3DSurface8());

    if (m_pClient->CallMethod(MID_DEV8_GetFrontBuffer, this, &hrS, pDstSurface)) {
        if (hrC != hrS) {
            Log(LOG_WARN, TEXT("IDirect3DDevice8::GetFrontBuffer result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
        }
    }

    return hrC;

#else

    Log(LOG_FAIL, TEXT("IDirect3DDevice8::GetFrontBuffer is not implemented on Xbox"));
    return E_NOTIMPL;

#endif // UNDER_XBOX
}

//******************************************************************************
HRESULT CCDevice8::ProcessVertices(UINT uStartVertexSrc, UINT uStartVertexDst, UINT uNumVertices, CVertexBuffer8* pDstBuffer, DWORD dwFlags) {

    HRESULT hrC, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!pDstBuffer) {
        return D3DERR_INVALIDCALL;
    }
    if (!m_pd3dd || !pDstBuffer->GetIDirect3DVertexBuffer8()) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

#ifndef UNDER_XBOX

    hrC = m_pd3dd->ProcessVertices(uStartVertexSrc, uStartVertexDst, uNumVertices, pDstBuffer->GetIDirect3DVertexBuffer8(), dwFlags);

    if (m_pClient->CallMethod(MID_DEV8_ProcessVertices, this, &hrS, uStartVertexSrc, uStartVertexDst, uNumVertices, pDstBuffer, dwFlags)) {
        if (hrC != hrS) {
            Log(LOG_WARN, TEXT("IDirect3DDevice8::ProcessVertices result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
        }
    }

#else

    Log(LOG_FAIL, TEXT("IDirect3DDevice8::ProcessVertices is not implemented on Xbox"));
    hrC = E_NOTIMPL;

#endif // UNDER_XBOX

    return hrC;
}

//******************************************************************************
HRESULT CCDevice8::ResourceManagerDiscardBytes(DWORD dwNumBytes) {

    HRESULT hrC, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

#ifndef UNDER_XBOX
    hrC = m_pd3dd->ResourceManagerDiscardBytes(dwNumBytes);

    if (m_pClient->CallMethod(MID_DEV8_ResourceManagerDiscardBytes, this, &hrS, dwNumBytes)) {
        if (hrC != hrS) {
            Log(LOG_WARN, TEXT("IDirect3DDevice8::ResourceManagerDiscardBytes result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
        }
    }

    return hrC;
#else
    Log(LOG_FAIL, TEXT("IDirect3DDevice8::ResourceManagerDiscardBytes is not implemented on XBox"));
    return E_NOTIMPL;
#endif // UNDER_XBOX
}

//******************************************************************************
HRESULT CCDevice8::TestCooperativeLevel() {

    HRESULT hrC, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

#ifndef UNDER_XBOX
    hrC = m_pd3dd->TestCooperativeLevel();

    if (m_pClient->CallMethod(MID_DEV8_TestCooperativeLevel, this, &hrS)) {
        if (hrC != hrS) {
            Log(LOG_WARN, TEXT("IDirect3DDevice8::TestCooperativeLevel result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
        }
    }

    return hrC;
#else
    return D3D_OK;
#endif // UNDER_XBOX
}

//******************************************************************************
void CCDevice8::SetCursorPosition(UINT uSX, UINT uSY, DWORD dwFlags) {

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return;
    }
#endif // D3DLOCUS_DEBUG

#ifndef UNDER_XBOX

    m_pd3dd->SetCursorPosition(uSX, uSY, dwFlags);

    m_pClient->CallMethod(MID_DEV8_SetCursorPosition, this, uSX, uSY, dwFlags);

#else

    Log(LOG_FAIL, TEXT("IDirect3DDevice8::SetCursorPosition is not implemented on Xbox"));

#endif // UNDER_XBOX
}

//******************************************************************************
HRESULT CCDevice8::SetCursorProperties(UINT uHotSpotX, UINT uHotSpotY, CSurface8* pCursorBitmap) {

    LPDIRECT3DSURFACE8 pd3ds = NULL;
    HRESULT            hrC, hrS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return ERR_NOTCREATED;
    }
#endif // D3DLOCUS_DEBUG

#ifndef UNDER_XBOX

    if (pCursorBitmap) {
        pd3ds = pCursorBitmap->GetIDirect3DSurface8();
    }
    hrC = m_pd3dd->SetCursorProperties(uHotSpotX, uHotSpotY, pd3ds);

    if (m_pClient->CallMethod(MID_DEV8_SetCursorProperties, this, &hrS, uHotSpotX, uHotSpotY, pCursorBitmap)) {
        if (hrC != hrS) {
            Log(LOG_WARN, TEXT("IDirect3DDevice8::SetCursorProperties result inconsistent [C: 0x%X, S: 0x%X]"), hrC, hrS);
        }
    }

#else

    Log(LOG_FAIL, TEXT("IDirect3DDevice8::SetCursorProperties is not implemented on Xbox"));
    hrC = E_NOTIMPL;

#endif // UNDER_XBOX

    return hrC;
}

//******************************************************************************
BOOL CCDevice8::ShowCursor(BOOL bShow) {

    BOOL bVisibleC, bVisibleS;

#ifdef D3DLOCUS_DEBUG
    if (!m_pd3dd) {
        return FALSE;
    }
#endif // D3DLOCUS_DEBUG

#ifndef UNDER_XBOX

    bVisibleC = m_pd3dd->ShowCursor(bShow);

    if (m_pClient->CallMethod(MID_DEV8_ShowCursor, this, &bVisibleS, bShow)) {
        if (bVisibleC != bVisibleS) {
            Log(LOG_WARN, TEXT("IDirect3DDevice8::ShowCursor result inconsistent"));
        }
    }

#else

    Log(LOG_FAIL, TEXT("IDirect3DDevice8::ShowCursor is not implemented on Xbox"));
    bVisibleC = FALSE;

#endif // UNDER_XBOX

    return bVisibleC;
}
