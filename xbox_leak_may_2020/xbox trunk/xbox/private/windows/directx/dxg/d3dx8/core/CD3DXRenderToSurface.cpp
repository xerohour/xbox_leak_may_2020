///////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 1999 Microsoft Corporation.  All Rights Reserved.
//
//  File:       CD3DXRenderToSurface.cpp
//  Content:    RenderToSurface object
//
///////////////////////////////////////////////////////////////////////////

#include "pchcore.h"


CD3DXRenderToSurface::CD3DXRenderToSurface()
{
    m_cRef         = 1;
    m_pDevice      = NULL;
    m_pColor       = NULL;
    m_pColorOld    = NULL;
    m_pZStencil    = NULL;
    m_pZStencilOld = NULL;
    m_pSurface     = NULL;
    m_dwStateBlock = 0;
}


CD3DXRenderToSurface::~CD3DXRenderToSurface()
{
    RELEASE(m_pColor);
    RELEASE(m_pColorOld);
    RELEASE(m_pZStencil);
    RELEASE(m_pZStencilOld);
    RELEASE(m_pSurface);

    if(m_dwStateBlock)
        m_pDevice->DeleteStateBlock(m_dwStateBlock);

    RELEASE(m_pDevice);
}


HRESULT 
CD3DXRenderToSurface::Init(LPDIRECT3DDEVICE8 pDevice, UINT Width, UINT Height,
        D3DFORMAT Format, BOOL DepthStencil, D3DFORMAT DepthStencilFormat)
{
    if(!pDevice)
    {
        DPF(0, "pDevice pointer is invalid");
        return D3DERR_INVALIDCALL;
    }
    if(m_pDevice)
        return E_FAIL;

    m_pDevice = pDevice;
    m_pDevice->AddRef();

    m_Desc.Width              = Width;
    m_Desc.Height             = Height;
    m_Desc.Format             = Format;
    m_Desc.DepthStencil       = DepthStencil;
    m_Desc.DepthStencilFormat = DepthStencilFormat;

    return S_OK;
}


// IUnknown
STDMETHODIMP
CD3DXRenderToSurface::QueryInterface(REFIID riid, LPVOID *ppv)
{
    *ppv=NULL;
    if (riid == IID_IUnknown)
        *ppv=(IUnknown*)this;
    else if (riid == IID_ID3DXRenderToSurface)
        *ppv=(ID3DXRenderToSurface*)this;
    else
        return E_NOINTERFACE;

    ((LPUNKNOWN)*ppv)->AddRef();

    return S_OK;
}


ULONG STDMETHODCALLTYPE
CD3DXRenderToSurface::AddRef()
{
    m_cRef += 1;
    return m_cRef;
}


ULONG STDMETHODCALLTYPE
CD3DXRenderToSurface::Release()
{
    ULONG cRef = m_cRef;
    m_cRef -= 1;

    if (cRef == 1)
        delete this;

    return cRef-1;
}


// ID3DXRenderToSurface
STDMETHODIMP
CD3DXRenderToSurface::GetDevice(LPDIRECT3DDEVICE8* ppDevice)
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
CD3DXRenderToSurface::GetDesc(D3DXRTS_DESC* pDesc)
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
CD3DXRenderToSurface::BeginScene(LPDIRECT3DSURFACE8 pSurface, CONST D3DVIEWPORT8* pViewport)
{
    HRESULT hr;

    if(!pSurface)
    {
        DPF(0, "pSurface pointer is invalid");
        return D3DERR_INVALIDCALL;
    }

    if(m_pSurface)
    {
        DPF(0, "BeginScene called inside a scene");
        return E_FAIL;
    }


    // Get surface desc
    D3DSURFACE_DESC desc;
    pSurface->GetDesc(&desc);

    if(desc.Format != m_Desc.Format)
    {
        DPF(0, "Surface format does not match render target");
        return E_FAIL;
    }

    if(desc.Width != m_Desc.Width || desc.Height != m_Desc.Height)
    {
        DPF(0, "Surface size does not match render target");
        return E_FAIL;
    }

    m_bRenderTarget = (desc.Usage & D3DUSAGE_RENDERTARGET);



    // Compute viewport
    if(pViewport)
    {
        m_Viewport = *pViewport;

        if(m_Viewport.X + m_Viewport.Width  > m_Desc.Width ||
           m_Viewport.Y + m_Viewport.Height > m_Desc.Height)
        {
            DPF(0, "Viewport outside surface limits");
            return D3DERR_INVALIDCALL;
        }
    }
    else
    {
        m_Viewport.X      = 0;
        m_Viewport.Y      = 0;
        m_Viewport.Width  = m_Desc.Width;
        m_Viewport.Height = m_Desc.Height;
        m_Viewport.MinZ   = 0.0f;
        m_Viewport.MaxZ   = 1.0f;
    }


    // Create Color buffer if needed
    if(!m_pColor && !m_bRenderTarget)
    {
        if(FAILED(hr = m_pDevice->CreateRenderTarget(m_Desc.Width, m_Desc.Height, 
            m_Desc.Format, D3DMULTISAMPLE_NONE, TRUE, &m_pColor)))
        {
            DPF(0, "CreateRenderTarget failed");
            return hr;
        }
    }


    // Create ZStencil buffer if needed
    if(!m_pZStencil && m_Desc.DepthStencil)
    {
        if(FAILED(hr = m_pDevice->CreateDepthStencilSurface(m_Desc.Width,
            m_Desc.Height,
            m_Desc.DepthStencilFormat, D3DMULTISAMPLE_NONE, &m_pZStencil)))
        {
            DPF(0, "CreateDepthStencilSurface failed");
            return hr;
        }
    }

    // Create StateBlock if needed
    if(!m_dwStateBlock)
    {
        m_pDevice->BeginStateBlock();
        m_pDevice->SetViewport(&m_Viewport);
        m_pDevice->EndStateBlock(&m_dwStateBlock);
    }



    // Set new render target
    RELEASE(m_pColorOld);
    RELEASE(m_pZStencilOld);

    m_pDevice->CaptureStateBlock(m_dwStateBlock);
    m_pDevice->GetRenderTarget(&m_pColorOld);
    m_pDevice->GetDepthStencilSurface(&m_pZStencilOld);


    if(FAILED(hr = m_pDevice->SetRenderTarget(m_bRenderTarget ? pSurface : m_pColor, m_pZStencil)))
    {
        DPF(0, "SetRenderTarget failed");
        return hr;
    }

    if(FAILED(hr = m_pDevice->SetViewport(&m_Viewport)))
    {
        DPF(0, "SetViewport failed");
        return hr;
    }

    if(FAILED(hr = m_pDevice->BeginScene()))
    {
        DPF(0, "BeginScene failed");
        return hr;
    }

    m_pSurface = pSurface;
    m_pSurface->AddRef();

    return S_OK;
}


STDMETHODIMP
CD3DXRenderToSurface::EndScene()
{
    if(!m_pSurface)
    {
        DPF(0, "EndScene called outside a scene");
        return E_FAIL;
    }

    m_pDevice->EndScene();
    
    // Blit, if we weren't rendering directly to texture
    if(!m_bRenderTarget)
    {
        RECT rect;

        rect.left   = (LONG) m_Viewport.X;
        rect.top    = (LONG) m_Viewport.Y;
        rect.right  = (LONG) (m_Viewport.X + m_Viewport.Width);
        rect.bottom = (LONG) (m_Viewport.Y + m_Viewport.Height);

        m_pDevice->CopyRects(m_pColor, &rect, 1, m_pSurface, (POINT *) &rect);
    }


    // Put render target back
    m_pDevice->SetRenderTarget(m_pColorOld, m_pZStencilOld);
    m_pDevice->ApplyStateBlock(m_dwStateBlock);

    RELEASE(m_pColorOld);
    RELEASE(m_pZStencilOld);
    RELEASE(m_pSurface);

    return S_OK;
}
