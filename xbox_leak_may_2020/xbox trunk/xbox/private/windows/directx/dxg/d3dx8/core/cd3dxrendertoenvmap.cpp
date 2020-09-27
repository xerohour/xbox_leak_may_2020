///////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 1999 Microsoft Corporation.  All Rights Reserved.
//
//  File:       CD3DXRenderToEnvMap.cpp
//  Content:    RenderToEnvMap object
//
///////////////////////////////////////////////////////////////////////////

#include "pchcore.h"


#define MODE_NONE           0
#define MODE_CUBE           1
#define MODE_SPHERE         2
#define MODE_HEMISPHERE     3
#define MODE_PARABOLIC      4



CD3DXRenderToEnvMap::CD3DXRenderToEnvMap()
{
    UINT i;

    m_cRef      = 1;
    m_pDevice   = NULL;
    m_pColor    = NULL;
    m_pDepth    = NULL;
    m_pColorOld = NULL;
    m_pDepthOld = NULL;
    m_pCubeTex  = NULL;

    for(i = 0; i < 6; i++)
        m_pFace[i] = NULL;

    for(i = 0; i < 2; i++)
        m_pTex[i] = NULL;

    m_dwMode = 0;
    m_dwStateBlock = 0;
    m_bInScene = FALSE;
}


CD3DXRenderToEnvMap::~CD3DXRenderToEnvMap()
{
    UINT i;

    if(m_pDevice && m_dwStateBlock)
        m_pDevice->DeleteStateBlock(m_dwStateBlock);

    RELEASE(m_pDevice);
    RELEASE(m_pColor);
    RELEASE(m_pDepth);
    RELEASE(m_pColorOld);
    RELEASE(m_pDepthOld);
    RELEASE(m_pCubeTex);

    for(i = 0; i < 6; i++)
        RELEASE(m_pFace[i]);

    for(i = 0; i < 2; i++)
        RELEASE(m_pTex[i]);
}


HRESULT 
CD3DXRenderToEnvMap::Init(LPDIRECT3DDEVICE8 pDevice, UINT Size, D3DFORMAT Format, 
    BOOL DepthStencil, D3DFORMAT DepthStencilFormat)
{
    if(!pDevice)
    {
        DPF(0, "pDevice pointer is invalid");
        return D3DERR_INVALIDCALL;
    }

    D3DXASSERT(m_pDevice == NULL);

    m_pDevice = pDevice;
    m_pDevice->AddRef();

    m_Desc.Size               = Size;
    m_Desc.Format             = Format;
    m_Desc.DepthStencil       = DepthStencil;
    m_Desc.DepthStencilFormat = DepthStencilFormat;

    m_Viewport.X      = 0;
    m_Viewport.Y      = 0;
    m_Viewport.Width  = Size;
    m_Viewport.Height = Size;
    m_Viewport.MinZ   = 0.0f;
    m_Viewport.MaxZ   = 1.0f;

    return S_OK;
}



HRESULT
CD3DXRenderToEnvMap::Begin()
{
    // Create StateBlock if needed
    if(!m_dwStateBlock)
    {
        m_pDevice->BeginStateBlock();
        m_pDevice->SetViewport(&m_Viewport);
        m_pDevice->EndStateBlock(&m_dwStateBlock);
    }

    m_pDevice->CaptureStateBlock(m_dwStateBlock);
    m_pDevice->GetRenderTarget(&m_pColorOld);
    m_pDevice->GetDepthStencilSurface(&m_pDepthOld);
    m_pDevice->SetViewport(&m_Viewport);

    return S_OK;
}


HRESULT
CD3DXRenderToEnvMap::BeginScene(D3DCUBEMAP_FACES Face)
{
    HRESULT hr;
    D3DSURFACE_DESC desc;
    LPDIRECT3DSURFACE8 pSurf = NULL;


    if(MODE_CUBE == m_dwMode)
    {
        m_pCubeTex->GetLevelDesc(0, &desc);
    }
    else
    {
        // Create face texture, if needed
        if(!m_pFace[Face])
        {
            if(FAILED(hr = m_pDevice->CreateTexture(m_Desc.Size, m_Desc.Size, 1, D3DUSAGE_RENDERTARGET, m_Desc.Format, D3DPOOL_DEFAULT, &m_pFace[Face])) &&
               FAILED(hr = m_pDevice->CreateTexture(m_Desc.Size, m_Desc.Size, 1, 0, m_Desc.Format, D3DPOOL_DEFAULT, &m_pFace[Face])))
            {
                DPF(0, "CreateTexture failed");
                return hr;
            }               
        }

        m_pFace[Face]->GetLevelDesc(0, &desc);
    }


    m_bRenderTarget = (desc.Usage & D3DUSAGE_RENDERTARGET);

    if(!m_bRenderTarget)
    {
        // Create color buffer, if needed
        if(!m_pColor)
        {
            if(FAILED(hr = m_pDevice->CreateRenderTarget(m_Desc.Size, m_Desc.Size, 
                m_Desc.Format, D3DMULTISAMPLE_NONE, TRUE, &m_pColor)))
            {
                DPF(0, "CreateRenderTarget failed");
                return hr;
            }
        }

        pSurf = m_pColor;
    }
    else 
    {
        if(MODE_CUBE == m_dwMode)
            m_pCubeTex->GetCubeMapSurface(Face, 0, &pSurf);
        else
            m_pFace[Face]->GetSurfaceLevel(0, &pSurf);
    }


    // Create depth-stencil, if needed
    if(!m_pDepth && m_Desc.DepthStencil)
    {
        if(FAILED(hr = m_pDevice->CreateDepthStencilSurface(m_Desc.Size,
            m_Desc.Size, 
            m_Desc.DepthStencilFormat, D3DMULTISAMPLE_NONE, &m_pDepth)))
        {
            DPF(0, "CreateDepthStencilSurface failed");
            return hr;
        }
    }


    // Set rendertarget
    m_pDevice->SetRenderTarget(pSurf, m_pDepth);

    if(m_bRenderTarget)
        pSurf->Release();


    if(FAILED(hr = m_pDevice->BeginScene()))
        return hr;

    m_bInScene = TRUE;
    m_Face = Face;
    return S_OK;
}


HRESULT
CD3DXRenderToEnvMap::EndScene()
{
    HRESULT hr;

    if(!m_bInScene)
        return S_OK;

    if(FAILED(hr = m_pDevice->EndScene()))
        return hr;
    
    if(!m_bRenderTarget)
    {
        LPDIRECT3DSURFACE8 pSurf;

        if(MODE_CUBE == m_dwMode)
            hr = m_pCubeTex->GetCubeMapSurface(m_Face, 0, &pSurf);
        else
            hr = m_pFace[m_Face]->GetSurfaceLevel(0, &pSurf);

        if(FAILED(hr))
            return hr;

        m_pDevice->CopyRects(m_pColor, NULL, 0, pSurf, NULL);
        pSurf->Release();
    }

    m_bInScene = FALSE;
    return S_OK;
}


HRESULT 
CD3DXRenderToEnvMap::RenderSphere()
{

    return E_NOTIMPL;
}


HRESULT 
CD3DXRenderToEnvMap::RenderHemisphere()
{
    return E_NOTIMPL;
}


HRESULT 
CD3DXRenderToEnvMap::RenderParabolic()
{
    return E_NOTIMPL;
}





// IUnknown
STDMETHODIMP
CD3DXRenderToEnvMap::QueryInterface(REFIID riid, LPVOID *ppv)
{
    *ppv=NULL;
    if (riid == IID_IUnknown)
        *ppv=(IUnknown*)this;
    else if (riid == IID_ID3DXRenderToEnvMap)
        *ppv=(ID3DXRenderToEnvMap*)this;
    else
        return E_NOINTERFACE;

    ((LPUNKNOWN)*ppv)->AddRef();

    return S_OK;
}


ULONG STDMETHODCALLTYPE
CD3DXRenderToEnvMap::AddRef()
{
    m_cRef += 1;
    return m_cRef;
}


ULONG STDMETHODCALLTYPE
CD3DXRenderToEnvMap::Release()
{
    ULONG cRef = m_cRef;
    m_cRef -= 1;

    if (cRef == 1)
        delete this;

    return cRef-1;
}


// ID3DXRenderToEnvMap
STDMETHODIMP
CD3DXRenderToEnvMap::GetDevice(LPDIRECT3DDEVICE8* ppDevice)
{
    if(!ppDevice)
    {
        DPF(0, "ppDevice pointer is invalid");
        return D3DERR_INVALIDCALL;
    }
    *ppDevice = m_pDevice;
    m_pDevice->AddRef();
    return S_OK;
}


STDMETHODIMP
CD3DXRenderToEnvMap::GetDesc(D3DXRTE_DESC* pDesc)
{
    if(!pDesc)
    {
        DPF(0, "pDesc pointer is invalid");
        return D3DERR_INVALIDCALL;
    }
    *pDesc = m_Desc;
    return S_OK;
}


STDMETHODIMP 
CD3DXRenderToEnvMap::BeginCube(LPDIRECT3DCUBETEXTURE8 pCubeTex)
{
    HRESULT hr;
    D3DSURFACE_DESC desc;

    // Validate texture
    if(!pCubeTex)
    {
        DPF(0, "pCubeTex pointer is invalid");
        return D3DERR_INVALIDCALL;
    }
    if(m_dwMode)
    {
        DPF(0, "Begin called inside a scene");
        return D3DERR_INVALIDCALL;
    }

    
    // Cube map must be same size/format as EnvMap
    pCubeTex->GetLevelDesc(0, &desc);

    if(desc.Format != m_Desc.Format)
    {
        DPF(0, "Surface format does not match render target");
        return D3DERR_INVALIDCALL;
    }

    if(desc.Width != m_Desc.Size || desc.Height != m_Desc.Size)
    {
        DPF(0, "Surface size does not match render target");
        return D3DERR_INVALIDCALL;
    }


    // Begin
    if(FAILED(hr = Begin()))
        return hr;

    m_pCubeTex = pCubeTex;
    m_pCubeTex->AddRef();

    m_dwMode = MODE_CUBE;
    return S_OK;
}


STDMETHODIMP 
CD3DXRenderToEnvMap::BeginSphere(LPDIRECT3DTEXTURE8 pTex)
{
    HRESULT hr;
    D3DSURFACE_DESC desc;

    // Validate texture
    if(!pTex)
    {
        DPF(0, "pTex pointer is invalid");
        return D3DERR_INVALIDCALL;
    }
    if(m_dwMode)
    {
        DPF(0, "Begin called inside a scene");
        return E_FAIL;
    }
    
    // Begin
    if(FAILED(hr = Begin()))
        return hr;

    m_pTex[0] = pTex;
    m_pTex[0]->AddRef();

    m_dwMode = MODE_SPHERE;
    return S_OK;
}


STDMETHODIMP 
CD3DXRenderToEnvMap::BeginHemisphere(LPDIRECT3DTEXTURE8 pTexZPos, LPDIRECT3DTEXTURE8 pTexZNeg)
{
    HRESULT hr;
    D3DSURFACE_DESC desc;

    // Validate texture
    if(!pTexZPos && !pTexZNeg)
    {
        DPF(0, "pTexZPos and pTexZNeg pointers are both invalid");
        return D3DERR_INVALIDCALL;
    }
    if(m_dwMode)
    {
        DPF(0, "Begin called inside a scene");
        return E_FAIL;
    }
    
    // Begin
    if(FAILED(hr = Begin()))
        return hr;

    m_pTex[0] = pTexZPos;
    m_pTex[1] = pTexZNeg;

    for(UINT i = 0; i < 2; i++)
    {
        if(m_pTex[i])
            m_pTex[i]->AddRef();
    }

    m_dwMode = MODE_HEMISPHERE;
    return S_OK;
}


STDMETHODIMP 
CD3DXRenderToEnvMap::BeginParabolic(LPDIRECT3DTEXTURE8 pTexZPos, LPDIRECT3DTEXTURE8 pTexZNeg)
{
    HRESULT hr;
    D3DSURFACE_DESC desc;

    // Validate texture
    if(!pTexZPos && !pTexZNeg)
    {
        DPF(0, "pTexZPos and pTexZNeg pointers are both invalid");
        return D3DERR_INVALIDCALL;
    }
    if(m_dwMode)
    {
        DPF(0, "Begin called inside a scene");
        return E_FAIL;
    }
    
    // Begin
    if(FAILED(hr = Begin()))
        return hr;

    m_pTex[0] = pTexZPos;
    m_pTex[1] = pTexZNeg;

    for(UINT i = 0; i < 2; i++)
    {
        if(m_pTex[i])
            m_pTex[i]->AddRef();
    }

    m_dwMode = MODE_PARABOLIC;
    return S_OK;
}


STDMETHODIMP
CD3DXRenderToEnvMap::Face(D3DCUBEMAP_FACES Face)
{
    HRESULT hr;

    if(FAILED(hr = EndScene()))
        return hr;

    if(FAILED(hr = BeginScene(Face)))
        return hr;

    return S_OK;
}


STDMETHODIMP 
CD3DXRenderToEnvMap::End()
{
    HRESULT hr;

    // End current scene
    if(FAILED(hr = EndScene()))
        return hr;

    // Restore state
    m_pDevice->SetRenderTarget(m_pColorOld, m_pDepthOld);
    m_pDevice->ApplyStateBlock(m_dwStateBlock);

    RELEASE(m_pColorOld);
    RELEASE(m_pDepthOld);

    
    // Calculate maps
    if(MODE_CUBE == m_dwMode)
        hr = S_OK;
    else if(MODE_SPHERE == m_dwMode)
        hr = RenderSphere();
    else if(MODE_HEMISPHERE == m_dwMode)
        hr = RenderHemisphere();
    else if(MODE_PARABOLIC == m_dwMode)
        hr = RenderParabolic();
    else
        hr = E_FAIL;

    RELEASE(m_pCubeTex);
    RELEASE(m_pTex[0]);
    RELEASE(m_pTex[1]);

    m_dwMode = 0;
    return hr;
}
