///////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 1999 Microsoft Corporation.  All Rights Reserved.
//
//  File:       CD3DXSprite.cpp
//  Content:    Bitmap font support
//
///////////////////////////////////////////////////////////////////////////

#include "pchcore.h"


struct SPRITE_VERTEX
{
    D3DXVECTOR4 m_Pos;
    D3DCOLOR    m_Color;
    D3DXVECTOR2 m_Tex;
};

#define SPRITE_VERTEX_FVF (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1)




//////////////////////////////////////////////////////////////////////////////
// CD3DXSprite ///////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

CD3DXSprite::CD3DXSprite()
{
    m_uRef       = 1;
    m_pDevice    = NULL;
    m_bBegin     = FALSE;
    m_dwOldState = NULL;
    m_dwNewState = NULL;
}


CD3DXSprite::~CD3DXSprite()
{
    if(m_pDevice)
    {
        if(m_dwNewState)
            m_pDevice->DeleteStateBlock(m_dwNewState);

        if(m_dwOldState)
            m_pDevice->DeleteStateBlock(m_dwOldState);

        m_pDevice->Release();
    }
}


STDMETHODIMP
CD3DXSprite::QueryInterface(REFIID iid, LPVOID *ppv)
{
#if DBG
    if(!ppv)
    {
        DPF(0, "ppv pointer is invalid");
        return D3DERR_INVALIDCALL;
    }
#endif

    if(IsEqualIID(iid, IID_ID3DXSprite))
        *ppv = (ID3DXSprite *) this;
    else if(IsEqualIID(iid, IID_IUnknown))
        *ppv = (IUnknown *) this;
    else
        return E_NOINTERFACE;

    AddRef();
    return S_OK;
}


STDMETHODIMP_(ULONG)
CD3DXSprite::AddRef()
{
    return ++m_uRef;
}


STDMETHODIMP_(ULONG)
CD3DXSprite::Release()
{
    if(--m_uRef)
        return m_uRef;

    delete this;
    return 0;
}


HRESULT
CD3DXSprite::Initialize(LPDIRECT3DDEVICE8 pDevice)
{
    HRESULT hr;

    if(!pDevice)
    {
        DPF(0, "pDevice pointer is invalid");
        return D3DERR_INVALIDCALL;
    }

    m_pDevice = pDevice;
    m_pDevice->AddRef();


    // Get device caps
    D3DCAPS8 caps;

    if(FAILED(hr = m_pDevice->GetDeviceCaps(&caps)))
        return hr;


    for(UINT i = 0; i < 2; i++)
    {
        m_pDevice->BeginStateBlock();

        m_pDevice->SetVertexShader(SPRITE_VERTEX_FVF);

        // Random device state
        m_pDevice->SetRenderState(D3DRS_WRAP0, 0);
        m_pDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
        m_pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
        m_pDevice->SetRenderState(D3DRS_VERTEXBLEND, FALSE);
        m_pDevice->SetRenderState(D3DRS_PATCHSEGMENTS, 1);

        // Modulate color and alpha
        m_pDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
        m_pDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
        m_pDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
        m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
        m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
        m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
        m_pDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
        m_pDevice->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

        // Setup texture addressing and filtering
        m_pDevice->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
        m_pDevice->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
        m_pDevice->SetTextureStageState(0, D3DTSS_MIPFILTER, D3DTEXF_NONE);
        m_pDevice->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);
        m_pDevice->SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
        m_pDevice->SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);
        m_pDevice->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);

        // Use alphatest on cards which dont support alphablend
        if((caps.SrcBlendCaps  & D3DPBLENDCAPS_SRCALPHA) &&
           (caps.DestBlendCaps & D3DPBLENDCAPS_INVSRCALPHA))
        {
            m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
            m_pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
            m_pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
        }
        else
        {
            m_pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
            m_pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
            m_pDevice->SetRenderState(D3DRS_ALPHAREF, 0x7f);
        }

        if(0 == i)
        {
            m_pDevice->SetTexture(0, NULL);
            m_pDevice->SetStreamSource(0, NULL, 0);
        }

        m_pDevice->EndStateBlock(i ? &m_dwNewState : &m_dwOldState);
    }

    return S_OK;
}


STDMETHODIMP
CD3DXSprite::GetDevice(LPDIRECT3DDEVICE8* ppDevice)
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


STDMETHODIMP CD3DXSprite::Begin()
{
    if(m_bBegin)
    {
        DPF(0, "BeginScene called inside a scene");
        return E_FAIL;
    }
    m_pDevice->CaptureStateBlock(m_dwOldState);
    m_pDevice->ApplyStateBlock(m_dwNewState);

    m_bBegin = TRUE;
    return S_OK;
}


STDMETHODIMP CD3DXSprite::Draw(LPDIRECT3DTEXTURE8 pSrcTexture,
    CONST RECT* pSrcRect, CONST D3DXVECTOR2 *pScaling,
    CONST D3DXVECTOR2* pRotationCenter, FLOAT Rotation,
    CONST D3DXVECTOR2* pTranslation, D3DCOLOR Color)
{
    D3DXVECTOR3 vS, vRC, vT;
    D3DXQUATERNION qR;
    D3DXMATRIX mat;

    if(pScaling)
    {
        vS.x = pScaling->x;
        vS.y = pScaling->y;
        vS.z = 1.0f;
    }

    if(pRotationCenter)
    {
        vRC.x = pRotationCenter->x;
        vRC.y = pRotationCenter->y;
        vRC.z = 0.0f;
    }

    if(Rotation != 0.0f)
    {
        qR.x = 0.0f;
        qR.y = 0.0f;
        qR.z = sinf(-0.5f * Rotation);
        qR.w = cosf(-0.5f * Rotation);
    }

    if(pTranslation)
    {
        vT.x = pTranslation->x;
        vT.y = pTranslation->y;
        vT.z = 0.0f;
    }

    D3DXMatrixTransformation(
        &mat,
        NULL,
        NULL,
        pScaling ? &vS : NULL,
        pRotationCenter ? &vRC : NULL,
        (Rotation != 0.0f) ? &qR : NULL,
        pTranslation ? &vT : NULL);


    return DrawTransform(pSrcTexture, pSrcRect, &mat, Color);
}


STDMETHODIMP CD3DXSprite::DrawTransform(LPDIRECT3DTEXTURE8 pSrcTexture,
    CONST RECT* pSrcRect, CONST D3DXMATRIX* pTransformation,
    D3DCOLOR Color)
{
    HRESULT hr;

    if(!pSrcTexture)
    {
        DPF(0, "pSrcTexture pointer is invalid");
        return D3DERR_INVALIDCALL;
    }

    // Validate rect
    RECT rect;
    D3DSURFACE_DESC desc;
    pSrcTexture->GetLevelDesc(0, &desc);

    if(pSrcRect)
    {
        rect = *pSrcRect;

        if((rect.left < 0) || (rect.left > rect.right) || (rect.left > (LONG) desc.Width) ||
           (rect.top < 0) || (rect.top > rect.bottom) || (rect.top > (LONG) desc.Height))
        {
            DPF(0, "Source rect is outside surface limits");
            return D3DERR_INVALIDCALL;
        }
    }
    else
    {
        rect.left   = 0;
        rect.top    = 0;
        rect.right  = (LONG) desc.Width;
        rect.bottom = (LONG) desc.Height;
    }



    // Setup vertices
    SPRITE_VERTEX v[4];

    float fWidth  = (float) (rect.right - rect.left);
    float fHeight = (float) (rect.bottom - rect.top);
    float fWidthInv  = 1.0f / (float) desc.Width;
    float fHeightInv = 1.0f / (float) desc.Height;

    float fMinX = fWidthInv  * (float) rect.left;
    float fMaxX = fWidthInv  * (float) rect.right;
    float fMinY = fHeightInv * (float) rect.top;
    float fMaxY = fHeightInv * (float) rect.bottom;


    v[0].m_Pos = D3DXVECTOR4(0.0f, 0.0f, 0.0f, 1.0f);
    v[0].m_Color = Color;
    v[0].m_Tex = D3DXVECTOR2(fMinX, fMinY);

    v[1].m_Pos = D3DXVECTOR4(0.0f, fHeight, 0.0f, 1.0f);
    v[1].m_Color = Color;
    v[1].m_Tex = D3DXVECTOR2(fMinX, fMaxY);

    v[2].m_Pos = D3DXVECTOR4(fWidth, fHeight, 0.0f, 1.0f);
    v[2].m_Color = Color;
    v[2].m_Tex = D3DXVECTOR2(fMaxX, fMaxY);

    v[3].m_Pos = D3DXVECTOR4(fWidth, 0.0f, 0.0f, 1.0f);
    v[3].m_Color = Color;
    v[3].m_Tex = D3DXVECTOR2(fMaxX, fMinY);

    if(pTransformation)
    {
        for(UINT i = 0; i < 4; i++)
            D3DXVec4Transform(&v[i].m_Pos, &v[i].m_Pos, pTransformation);
    }

    // Offset x and y by half a pixel, and convert w into rhw
    for(UINT i = 0; i < 4; i++)
    {
        float f = 0.5f * v[i].m_Pos.w;

        v[i].m_Pos.x -= f;
        v[i].m_Pos.y -= f;
        v[i].m_Pos.w = 1.0f / v[i].m_Pos.w;
    }



    // Draw stuff
    BOOL bBegin = m_bBegin;
    BOOL bAlpha = ((Color >> 24) != 0xff);

    if(!bAlpha)
    {
        switch(desc.Format)
        {
        case D3DFMT_A8R8G8B8:
        case D3DFMT_A1R5G5B5:
        case D3DFMT_A4R4G4B4:
        case D3DFMT_A8:
#if 0
        case D3DFMT_A8R3G3B2:
        case D3DFMT_A8P8:
        case D3DFMT_A4L4:
        case D3DFMT_DXT3:
        case D3DFMT_DXT5:
#endif
        case D3DFMT_P8:
        case D3DFMT_A8L8:
        case D3DFMT_DXT2:
        case D3DFMT_DXT4:
            bAlpha = TRUE;
            break;
        }
    }

    if(!m_bBegin)
    {
        if(FAILED(hr = Begin()))
            return hr;
    }


    m_pDevice->SetTexture(0, pSrcTexture);
    m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, bAlpha);
    m_pDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, (LPCVOID) &v, sizeof(SPRITE_VERTEX));

    if(!bBegin)
    {
        if(FAILED(hr = End()))
            return hr;
    }

    return S_OK;
}


STDMETHODIMP CD3DXSprite::End()
{
    if(!m_bBegin)
    {
        DPF(0, "Begin was not yet called");
        return E_FAIL;
    }
    m_pDevice->ApplyStateBlock(m_dwOldState);

    m_bBegin = FALSE;
    return S_OK;
}




