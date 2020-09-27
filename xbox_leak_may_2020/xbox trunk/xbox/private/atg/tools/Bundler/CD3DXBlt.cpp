//-----------------------------------------------------------------------------
// File: CD3DXBlt.cpp
//
// Desc: Contains general purpose blitting routines
//
// Hist: 01.29.02 - Updated to avoid using D3D
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "CD3DXBlt.h"

#define DPF(x,y)
#define D3DXASSERT(x)

///////////////////////////////////////////////////////////////////////////
// CXD3DXBlt ///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

CXD3DXBlt::CXD3DXBlt()
{
    m_pDest = NULL;
    m_pSrc  = NULL;
}


CXD3DXBlt::~CXD3DXBlt()
{
    if(m_pDest)
        delete m_pDest;

    if(m_pSrc)
        delete m_pSrc;
}

HRESULT CXD3DXBlt::Blt(D3DX_BLT* pDestBlt, D3DX_BLT* pSrcBlt, DWORD dwFilter)
{
    HRESULT hr;

    m_pDest = NULL;
    m_pSrc  = NULL;
    m_dwFilter = dwFilter;


    // Validate filter
    switch(dwFilter & 0xffff)
    {
    case D3DX_FILTER_NONE:
    case D3DX_FILTER_POINT:
    case D3DX_FILTER_LINEAR:
    case D3DX_FILTER_TRIANGLE:
    case D3DX_FILTER_BOX:
        break;

    default:
        DPF(0, "Invalid image filter");
        return D3DERR_INVALIDCALL;
    }

    if(dwFilter & (0xffff0000 & ~(D3DX_FILTER_MIRROR | D3DX_FILTER_DITHER)))
    {
        DPF(0, "Invalid image filter");
        return D3DERR_INVALIDCALL;
    }

    pDestBlt->bDither = (dwFilter & D3DX_FILTER_DITHER);


    // Find codecs
    if(!(m_pDest = CXD3DXCodec::Create(pDestBlt)) ||
       !(m_pSrc  = CXD3DXCodec::Create(pSrcBlt)))
    {
        DPF(0, "Unsupported image format");
        hr = E_FAIL;
        goto LDone;
    }


    // Make sure compatible image types
    if(m_pDest->m_dwType != m_pSrc->m_dwType)
    {
        DPF(0, "No conversion exists between source and destination image formats");
        hr = E_FAIL;
        goto LDone;
    }


    // Find blitter
    if(FAILED(BltSame()) &&
       FAILED(BltCopy()) &&
       FAILED(BltNone()) &&
       FAILED(BltPoint()) &&
       FAILED(BltBox2D()) &&
       FAILED(BltBox3D()) &&
       FAILED(BltLinear2D()) &&
       FAILED(BltLinear3D()) &&
       FAILED(BltTriangle2D()) &&
       FAILED(BltTriangle3D()))
    {
        DPF(0, "No suitable image filter found");
        hr = E_FAIL;
        goto LDone;
    }

    hr = S_OK;
    goto LDone;

LDone:
    if(m_pDest)
    {
        delete m_pDest;
        m_pDest = NULL;
    }

    if(m_pSrc)
    {
        delete m_pSrc;
        m_pSrc = NULL;
    }

    return hr;
}



HRESULT CXD3DXBlt::BltSame()
{
    if(m_pDest->m_Format != m_pSrc->m_Format)
        return E_FAIL;

    if(m_pSrc->m_bColorKey)
        return E_FAIL;

    if(m_pDest->m_uWidth  != m_pSrc->m_uWidth || 
       m_pDest->m_uHeight != m_pSrc->m_uHeight ||
       m_pDest->m_uDepth  != m_pSrc->m_uDepth)
    {
        return E_FAIL;
    }

#ifdef SUPPORT_DXT
    if(!m_pDest->m_bLinear)
        return BltSame_DXTn();
#endif // SUPPORT_DXT

    if(m_pDest->m_bPalettized && m_pDest->m_pPalette != m_pSrc->m_pPalette && 
        memcmp(m_pDest->m_pPalette, m_pSrc->m_pPalette, 256 * sizeof(PALETTEENTRY)))
    {
        return E_FAIL;
    }



    for(UINT uZ = 0; uZ < m_pDest->m_uDepth; uZ++)
    {
        BYTE *pbDest = m_pDest->m_pbData + uZ * m_pDest->m_uSlice;
        BYTE *pbSrc  = m_pSrc ->m_pbData + uZ * m_pSrc ->m_uSlice;

        for(UINT uY = 0; uY < m_pDest->m_uHeight; uY++)
        {
            memcpy(pbDest, pbSrc, m_pDest->m_uWidthBytes);

            pbDest += m_pDest->m_uPitch;
            pbSrc  += m_pSrc ->m_uPitch;
        }
    }

    return S_OK;
}


HRESULT CXD3DXBlt::BltCopy()
{
    D3DXCOLOR *pColor;

    if(m_pDest->m_uWidth  != m_pSrc->m_uWidth || 
       m_pDest->m_uHeight != m_pSrc->m_uHeight ||
       m_pDest->m_uDepth  != m_pSrc->m_uDepth)
    {
        return E_FAIL;
    }

    if(!(pColor = new D3DXCOLOR[m_pDest->m_uWidth]))
        return E_FAIL;

//  DPF(0, "blt copy %d->%d", m_pSrc->m_Format, m_pDest->m_Format);

    for(UINT uZ = 0; uZ < m_pDest->m_uDepth; uZ++)
    {
        for(UINT uY = 0; uY < m_pDest->m_uHeight; uY++)
        {
            m_pSrc ->Decode(uY, uZ, pColor);
            m_pDest->Encode(uY, uZ, pColor);
        }
    }

    delete [] pColor;
    return S_OK;
}


HRESULT CXD3DXBlt::BltNone()
{
    UINT uWidthMax, uWidthMin, uHeightMin, uDepthMin, uY, uZ;

    D3DXCOLOR *pColor;
    D3DXCOLOR *pBlack;

    if((m_dwFilter & 0xff) != D3DX_FILTER_NONE)
        return E_FAIL;

//  DPF(0, "blt none %d->%d", m_pSrc->m_Format, m_pDest->m_Format);

    uWidthMax  = (m_pDest->m_uWidth  > m_pSrc->m_uWidth)  ? m_pDest->m_uWidth  : m_pSrc->m_uWidth;
    uWidthMin  = (m_pDest->m_uWidth  < m_pSrc->m_uWidth)  ? m_pDest->m_uWidth  : m_pSrc->m_uWidth;
    uHeightMin = (m_pDest->m_uHeight < m_pSrc->m_uHeight) ? m_pDest->m_uHeight : m_pSrc->m_uHeight;
    uDepthMin  = (m_pDest->m_uDepth  < m_pSrc->m_uDepth)  ? m_pDest->m_uDepth  : m_pSrc->m_uDepth;

    if(!(pColor = new D3DXCOLOR[uWidthMax]))
        return E_FAIL;

    if(!(pBlack = new D3DXCOLOR[m_pDest->m_uWidth]))
    {
        delete [] pColor;
        return E_FAIL;
    }

    memset(pColor, 0x00, uWidthMax * sizeof(D3DXCOLOR));
    memset(pBlack, 0x00, m_pDest->m_uWidth * sizeof(D3DXCOLOR));


    for(uZ = 0; uZ < uDepthMin; uZ++)
    {
        for(uY = 0; uY < uHeightMin; uY++)
        {
            m_pSrc ->Decode(uY, uZ, pColor);
            m_pDest->Encode(uY, uZ, pColor);
        }

        for(uY = uHeightMin; uY < m_pDest->m_uHeight; uY++)
            m_pDest->Encode(uY, uZ, pBlack);
    }

    for(uZ = uDepthMin; uZ < m_pDest->m_uDepth; uZ++)
    {
        for(uY = 0; uY < m_pDest->m_uHeight; uY++)
            m_pDest->Encode(uY, uZ, pBlack);
    }

    delete [] pColor;
    delete [] pBlack;
    return S_OK;
}


HRESULT CXD3DXBlt::BltPoint()
{
    D3DXCOLOR *pSrc;
    D3DXCOLOR *pDest;

    if((m_dwFilter & 0xff) != D3DX_FILTER_POINT)
        return E_FAIL;

    if(!(pSrc = new D3DXCOLOR[m_pSrc->m_uWidth]))
        return E_OUTOFMEMORY;

    if(!(pDest = new D3DXCOLOR[m_pDest->m_uWidth]))
    {
        delete []pSrc;
        return E_OUTOFMEMORY;
    }


//  DPF(0, "blt point %d->%d", m_pSrc->m_Format, m_pDest->m_Format);

    UINT uSrcXInc = (m_pSrc->m_uWidth  << 16) / m_pDest->m_uWidth;
    UINT uSrcYInc = (m_pSrc->m_uHeight << 16) / m_pDest->m_uHeight;
    UINT uSrcZInc = (m_pSrc->m_uDepth  << 16) / m_pDest->m_uDepth;

    UINT uSrcZ  = 0;
    UINT uDestZ = 0;

    while(uDestZ < m_pDest->m_uDepth)
    {
        UINT uSrcY  = 0;
        UINT uDestY = 0;
        UINT uSrcYLast = (UINT) -1;

        while(uDestY < m_pDest->m_uHeight)
        {
            UINT uSrcX  = 0;
            UINT uDestX = 0;

            if((uSrcYLast ^ uSrcY) >> 16)
            {
                m_pSrc->Decode(uSrcY >> 16, uSrcZ >> 16, pSrc);
                uSrcYLast = uSrcY;
            }

            while(uDestX < m_pDest->m_uWidth)
            {
                pDest[uDestX] = pSrc[uSrcX >> 16];

                uSrcX += uSrcXInc;
                uDestX++;
            }

            m_pDest->Encode(uDestY, uDestZ, pDest);

            uSrcY += uSrcYInc;
            uDestY++;
        }

        uSrcZ += uSrcZInc;
        uDestZ++;
    }

    delete [] pSrc;
    delete [] pDest;

    return S_OK;
}


HRESULT CXD3DXBlt::BltBox2D()
{
    HRESULT hr;

    D3DXCOLOR *pxyz, *pxYz, *pXyz, *pXYz;
    D3DXCOLOR *pSrc, *pDest;

    if((m_dwFilter & 0xff) != D3DX_FILTER_BOX)
        return E_FAIL;

    if(m_pDest->m_dwType != CODEC_RGB || m_pSrc->m_dwType != CODEC_RGB)
        return E_FAIL;

    if(!(m_pDest->m_uWidth == (m_pSrc->m_uWidth >> 1)) && 
       !(1 == m_pDest->m_uWidth && 1 == m_pSrc->m_uWidth))
    {
        return E_FAIL;
    }

    if(!(m_pDest->m_uHeight == (m_pSrc->m_uHeight >> 1)) && 
       !(1 == m_pDest->m_uHeight && 1 == m_pSrc->m_uHeight))
    {
        return E_FAIL;
    }

    if(!(1 == m_pDest->m_uDepth && 1 == m_pSrc->m_uDepth))
        return E_FAIL;

//  DPF(0, "blt box %d->%d", m_pSrc->m_Format, m_pDest->m_Format);



    // Optimized filters
    if(!(m_dwFilter & D3DX_FILTER_DITHER) && (m_pSrc->m_Format == m_pDest->m_Format) && 
        (m_pSrc->m_uWidth >= 2) && (m_pSrc->m_uHeight >= 2))
    {
        hr = E_NOTIMPL;

        switch(m_pSrc->m_Format)
        {
#if 0
        case D3DFMT_R8G8B8:   hr = BltBox2D_R8G8B8();   break;
        case D3DFMT_R3G3B2:   hr = BltBox2D_R3G3B2();   break;
        case D3DFMT_A8R3G3B2: hr = BltBox2D_A8R3G3B2(); break;
        case D3DFMT_X4R4G4B4: hr = BltBox2D_X4R4G4B4(); break;
        case D3DFMT_A8P8:     hr = BltBox2D_A8P8();     break;
        case D3DFMT_A4L4:     hr = BltBox2D_A4L4();     break;
#endif
        case D3DFMT_A8R8G8B8: hr = BltBox2D_A8R8G8B8(); break;
        case D3DFMT_X8R8G8B8: hr = BltBox2D_X8R8G8B8(); break;
        case D3DFMT_R5G6B5:   hr = BltBox2D_R5G6B5();   break;
        case D3DFMT_X1R5G5B5: hr = BltBox2D_X1R5G5B5(); break;
        case D3DFMT_A1R5G5B5: hr = BltBox2D_A1R5G5B5(); break;
        case D3DFMT_A4R4G4B4: hr = BltBox2D_A4R4G4B4(); break;
        case D3DFMT_A8:       hr = BltBox2D_A8();       break;
        case D3DFMT_P8:       hr = BltBox2D_P8();       break;
        case D3DFMT_L8:       hr = BltBox2D_A8();       break;
        case D3DFMT_A8L8:     hr = BltBox2D_A8L8();     break;
        }

        if(SUCCEEDED(hr))
            return S_OK;
    }


    // Generic filter
    if(!(pDest = new D3DXCOLOR[m_pDest->m_uWidth]))
        return E_OUTOFMEMORY;

    if(1 == m_pSrc->m_uHeight)
    {
        pSrc = new D3DXCOLOR[m_pSrc->m_uWidth];

        pxyz = pSrc;
        pxYz = pxyz;
    }
    else
    {
        pSrc = new D3DXCOLOR[m_pSrc->m_uWidth * 2];

        pxyz = pSrc + m_pSrc->m_uWidth * 0;
        pxYz = pSrc + m_pSrc->m_uWidth * 1;
    }

    if(!pSrc)
    {
        delete [] pDest;
        return E_OUTOFMEMORY;
    }

    if(1 == m_pSrc->m_uWidth)
    {
        pXyz = pxyz;
        pXYz = pxYz;
    }
    else
    {
        pXyz = pxyz + 1;
        pXYz = pxYz + 1;
    }


    for(UINT uY = 0; uY < m_pDest->m_uHeight; uY++)
    {
        UINT uY2 = uY << 1;

        m_pSrc->Decode(uY2 + 0, 0, pxyz);

        if(pxYz != pxyz)
            m_pSrc->Decode(uY2 + 1, 0, pxYz);


        for(UINT uX = 0; uX < m_pDest->m_uWidth; uX++)
        {
            UINT uX2 = uX << 1;
            pDest[uX] = (pxyz[uX2] + pXyz[uX2] + pxYz[uX2] + pXYz[uX2]) * 0.25f;
        }

        m_pDest->Encode(uY, 0, pDest);
    }


    delete [] pSrc;
    delete [] pDest;

    return S_OK;
}



HRESULT CXD3DXBlt::BltBox3D()
{
    D3DXCOLOR *pxyz, *pxyZ, *pxYz, *pxYZ, *pXyz, *pXyZ, *pXYz, *pXYZ;
    D3DXCOLOR *pSrc, *pDest;

    if((m_dwFilter & 0xff) != D3DX_FILTER_BOX)
        return E_FAIL;

    if(m_pDest->m_dwType != CODEC_RGB || m_pSrc->m_dwType != CODEC_RGB)
        return E_FAIL;

    if(!(m_pDest->m_uWidth == (m_pSrc->m_uWidth >> 1)) && 
       !(1 == m_pDest->m_uWidth && 1 == m_pSrc->m_uWidth))
    {
        return E_FAIL;
    }

    if(!(m_pDest->m_uHeight == (m_pSrc->m_uHeight >> 1)) && 
       !(1 == m_pDest->m_uHeight && 1 == m_pSrc->m_uHeight))
    {
        return E_FAIL;
    }

    if(!(m_pDest->m_uDepth == (m_pSrc->m_uDepth >> 1)))
        return E_FAIL;

//  DPF(0, "blt box %d->%d", m_pSrc->m_Format, m_pDest->m_Format);

    if(!(pDest = new D3DXCOLOR[m_pDest->m_uWidth]))
        return E_OUTOFMEMORY;

    if(1 == m_pSrc->m_uHeight)
    {
        pSrc = new D3DXCOLOR[m_pSrc->m_uWidth * 2];

        pxyz = pSrc + m_pSrc->m_uWidth * 0;
        pxyZ = pSrc + m_pSrc->m_uWidth * 1;
        pxYz = pxyz;
        pxYZ = pxyZ;
    }
    else
    {
        pSrc = new D3DXCOLOR[m_pSrc->m_uWidth * 4];

        pxyz = pSrc + m_pSrc->m_uWidth * 0;
        pxyZ = pSrc + m_pSrc->m_uWidth * 1;
        pxYz = pSrc + m_pSrc->m_uWidth * 2;
        pxYZ = pSrc + m_pSrc->m_uWidth * 3;
    }

    if(!pSrc)
    {
        delete [] pDest;
        return E_OUTOFMEMORY;
    }

    if(1 == m_pSrc->m_uWidth)
    {
        pXyz = pxyz;
        pXyZ = pxyZ;
        pXYz = pxYz;
        pXYZ = pxYZ;
    }
    else
    {
        pXyz = pxyz + 1;
        pXyZ = pxyZ + 1;
        pXYz = pxYz + 1;
        pXYZ = pxYZ + 1;
    }



    for(UINT uZ = 0; uZ < m_pDest->m_uDepth; uZ++)
    {
        UINT uZ2 = uZ << 1;

        for(UINT uY = 0; uY < m_pDest->m_uHeight; uY++)
        {
            UINT uY2 = uY << 1;

            m_pSrc->Decode(uY2 + 0, uZ2 + 0, pxyz);

            if(pxyZ != pxyz)
                m_pSrc->Decode(uY2 + 0, uZ2 + 1, pxyZ);

            if(pxYz != pxyz)
                m_pSrc->Decode(uY2 + 1, uZ2 + 0, pxYz);

            if(pxYZ != pxyZ && pxYZ != pxYz)
                m_pSrc->Decode(uY2 + 1, uZ2 + 1, pxYZ);


            for(UINT uX = 0; uX < m_pDest->m_uWidth; uX++)
            {
                UINT uX2 = uX << 1;

                pDest[uX] = (pxyz[uX2] + pXyz[uX2] + pxyZ[uX2] + pXyZ[uX2] +
                             pxYz[uX2] + pXYz[uX2] + pxYZ[uX2] + pXYZ[uX2]) * 0.125f;
            }

            m_pDest->Encode(uY, uZ, pDest);
        }
    }

    delete [] pSrc;
    delete [] pDest;

    return S_OK;
}



// ---------------------------------------------------------------------------
//
// LF - Linear filter
//
// This section of the code defines a bunch of structures, macros, and
// functions which are used for the linear filter.
//
// ---------------------------------------------------------------------------

struct LF_From
{
    UINT uFrom;
    float fWeight;
};

struct LF_To
{
    LF_From pFrom[2];
};


static UINT8 *
LF_SetupLinear(UINT uSrcLim, UINT uDstLim, BOOL bRepeat)
{
    LF_To *pTo;
    UINT8 *pbFilter;
    
    if(!(pTo = new LF_To[uDstLim]))
        return NULL;

    pbFilter = (UINT8 *) pTo;
    float fScale = (float) uSrcLim / (float) uDstLim;

    F2IBegin();

    for(UINT u = 0; u < uDstLim; u++)
    {
        float fSrc = (float) u * fScale - 0.5f;
        float fSrcFloor = floorf(fSrc);

        int iSrcA = F2I(fSrcFloor);
        int iSrcB = iSrcA + 1;

        if(iSrcA < 0)
            iSrcA = bRepeat ? uSrcLim - 1 : 0;

        if((UINT) iSrcB >= uSrcLim)
            iSrcB = bRepeat ? 0 : uSrcLim - 1;

        pTo->pFrom[0].uFrom   = (UINT) iSrcA;
        pTo->pFrom[0].fWeight = 1.0f - (fSrc - fSrcFloor);

        pTo->pFrom[1].uFrom   = (UINT) iSrcB;
        pTo->pFrom[1].fWeight = 1.0f - pTo->pFrom[0].fWeight;

        pTo++;
    }

    F2IEnd();

    return pbFilter;
}


HRESULT CXD3DXBlt::BltLinear2D()
{
    HRESULT hr;

    BOOL bRepeatX;
    BOOL bRepeatY;

    BYTE *pbXFilter = NULL;
    BYTE *pbYFilter = NULL;

    LF_To *pToX;
    LF_To *pToY;

    D3DXCOLOR *pDest = NULL;
    D3DXCOLOR *pSrc  = NULL;
    D3DXCOLOR *pSrcT;
    D3DXCOLOR *pxyz, *pxYz;

    UINT uX;
    UINT uY;
    UINT uFrom0;
    UINT uFrom1;


    if(m_pDest->m_dwType != CODEC_RGB || m_pSrc->m_dwType != CODEC_RGB)
        return E_FAIL;

    if((m_dwFilter & 0xff) != D3DX_FILTER_LINEAR)
        return E_FAIL;

//  DPF(0, "blt linear %d->%d", m_pSrc->m_Format, m_pDest->m_Format);


    // Create linear filters
    bRepeatX = !(m_dwFilter & D3DX_FILTER_MIRROR_U);
    bRepeatY = !(m_dwFilter & D3DX_FILTER_MIRROR_V);

    pbXFilter = LF_SetupLinear(m_pSrc->m_uWidth,  m_pDest->m_uWidth,  bRepeatX);
    pbYFilter = LF_SetupLinear(m_pSrc->m_uHeight, m_pDest->m_uHeight, bRepeatY);

    if(!pbXFilter || !pbYFilter)
        goto LOutOfMemory;


    // Create temp color space
    if(!(pDest = new D3DXCOLOR[m_pDest->m_uWidth]))
        goto LOutOfMemory;

    if(!(pSrc = new D3DXCOLOR[m_pSrc->m_uWidth * 2]))
        goto LOutOfMemory;

    pxyz = pSrc + m_pSrc->m_uWidth * 0;
    pxYz = pSrc + m_pSrc->m_uWidth * 1;



    uY = 0;
    pToY = (LF_To *) pbYFilter;

    uFrom0 = (UINT) -1;
    uFrom1 = (UINT) -1;

    while(uY < m_pDest->m_uHeight)
    {
        uX = 0;
        pToX = (LF_To *) pbXFilter;

        if(pToY->pFrom[0].uFrom != uFrom0)
        {
            if(pToY->pFrom[0].uFrom != uFrom1)
            {
                uFrom0 = pToY->pFrom[0].uFrom;
                m_pSrc->Decode(uFrom0, 0, pxyz);
            }
            else
            {
                uFrom0 = uFrom1;
                uFrom1 = (UINT) -1;

                pSrcT = pxyz; pxyz = pxYz; pxYz = pSrcT;
            }
        }

        if(pToY->pFrom[1].uFrom != uFrom1)
        {
            uFrom1 = pToY->pFrom[1].uFrom;
            m_pSrc->Decode(uFrom1, 0, pxYz);
        }

        while(uX < m_pDest->m_uWidth)
        {
            pDest[uX] = ((pxyz[pToX->pFrom[0].uFrom] * pToX->pFrom[0].fWeight + 
                          pxyz[pToX->pFrom[1].uFrom] * pToX->pFrom[1].fWeight) * pToY->pFrom[0].fWeight +
                         (pxYz[pToX->pFrom[0].uFrom] * pToX->pFrom[0].fWeight + 
                          pxYz[pToX->pFrom[1].uFrom] * pToX->pFrom[1].fWeight) * pToY->pFrom[1].fWeight);

            pToX++;
            uX++;
        }

        m_pDest->Encode(uY, 0, pDest);

        pToY++;
        uY++;
    }


    hr = S_OK;
    goto LDone;

LOutOfMemory:
    hr = E_OUTOFMEMORY;
    goto LDone;

LDone:
    delete [] pbXFilter;
    delete [] pbYFilter;

    delete [] pDest;
    delete [] pSrc;

    return hr;
}




HRESULT CXD3DXBlt::BltLinear3D()
{
    HRESULT hr;

    BOOL bRepeatX;
    BOOL bRepeatY;
    BOOL bRepeatZ;

    BYTE *pbXFilter = NULL;
    BYTE *pbYFilter = NULL;
    BYTE *pbZFilter = NULL;

    LF_To *pToX;
    LF_To *pToY;
    LF_To *pToZ;

    D3DXCOLOR *pDest = NULL;
    D3DXCOLOR *pSrc  = NULL;
    D3DXCOLOR *pSrcT;
    D3DXCOLOR *pxyz, *pxYz, *pxyZ, *pxYZ;

    UINT uX;
    UINT uY;
    UINT uZ;
    UINT uFrom0;
    UINT uFrom1;


    if(m_pDest->m_dwType != CODEC_RGB || m_pSrc->m_dwType != CODEC_RGB)
        return E_FAIL;

    if((m_dwFilter & 0xff) != D3DX_FILTER_LINEAR)
        return E_FAIL;

//  DPF(0, "blt linear %d->%d", m_pSrc->m_Format, m_pDest->m_Format);


    // Create linear filters
    bRepeatX = !(m_dwFilter & D3DX_FILTER_MIRROR_U);
    bRepeatY = !(m_dwFilter & D3DX_FILTER_MIRROR_V);
    bRepeatZ = !(m_dwFilter & D3DX_FILTER_MIRROR_W);

    pbXFilter = LF_SetupLinear(m_pSrc->m_uWidth,  m_pDest->m_uWidth,  bRepeatX);
    pbYFilter = LF_SetupLinear(m_pSrc->m_uHeight, m_pDest->m_uHeight, bRepeatY);
    pbZFilter = LF_SetupLinear(m_pSrc->m_uDepth,  m_pDest->m_uDepth,  bRepeatZ);

    if(!pbXFilter || !pbYFilter || !pbZFilter)
        goto LOutOfMemory;


    // Create temp color space
    if(!(pDest = new D3DXCOLOR[m_pDest->m_uWidth]))
        goto LOutOfMemory;

    if(!(pSrc = new D3DXCOLOR[m_pSrc->m_uWidth * 4]))
        goto LOutOfMemory;

    pxyz = pSrc + m_pSrc->m_uWidth * 0;
    pxYz = pSrc + m_pSrc->m_uWidth * 1;
    pxyZ = pSrc + m_pSrc->m_uWidth * 2;
    pxYZ = pSrc + m_pSrc->m_uWidth * 3;



    uZ = 0;
    pToZ = (LF_To *) pbZFilter;

    while(uZ < m_pDest->m_uDepth)
    {
        uY = 0;
        pToY = (LF_To *) pbYFilter;
        uFrom0 = (UINT) -1;
        uFrom1 = (UINT) -1;


        while(uY < m_pDest->m_uHeight)
        {
            uX = 0;
            pToX = (LF_To *) pbXFilter;

            if(pToY->pFrom[0].uFrom != uFrom0)
            {
                if(pToY->pFrom[0].uFrom != uFrom1)
                {
                    uFrom0 = pToY->pFrom[0].uFrom;

                    m_pSrc->Decode(uFrom0, pToZ->pFrom[0].uFrom, pxyz);
                    m_pSrc->Decode(uFrom0, pToZ->pFrom[1].uFrom, pxyZ);
                }
                else
                {
                    uFrom0 = uFrom1;
                    uFrom1 = (UINT) -1;

                    pSrcT = pxyz; pxyz = pxYz; pxYz = pSrcT;
                    pSrcT = pxyZ; pxyZ = pxYZ; pxYZ = pSrcT;
                }
            }

            if(pToY->pFrom[1].uFrom != uFrom1)
            {
                uFrom1 = pToY->pFrom[1].uFrom;

                m_pSrc->Decode(uFrom1, pToZ->pFrom[0].uFrom, pxYz);
                m_pSrc->Decode(uFrom1, pToZ->pFrom[1].uFrom, pxYZ);
            }


            while(uX < m_pDest->m_uWidth)
            {
                pDest[uX] = ((pxyz[pToX->pFrom[0].uFrom] * pToX->pFrom[0].fWeight + 
                              pxyz[pToX->pFrom[1].uFrom] * pToX->pFrom[1].fWeight) * pToY->pFrom[0].fWeight +
                             (pxYz[pToX->pFrom[0].uFrom] * pToX->pFrom[0].fWeight + 
                              pxYz[pToX->pFrom[1].uFrom] * pToX->pFrom[1].fWeight) * pToY->pFrom[1].fWeight) * pToZ->pFrom[0].fWeight +

                            ((pxyZ[pToX->pFrom[0].uFrom] * pToX->pFrom[0].fWeight + 
                              pxyZ[pToX->pFrom[1].uFrom] * pToX->pFrom[1].fWeight) * pToY->pFrom[0].fWeight +
                             (pxYZ[pToX->pFrom[0].uFrom] * pToX->pFrom[0].fWeight + 
                              pxYZ[pToX->pFrom[1].uFrom] * pToX->pFrom[1].fWeight) * pToY->pFrom[1].fWeight) * pToZ->pFrom[1].fWeight;

                pToX++;
                uX++;
            }

            m_pDest->Encode(uY, uZ, pDest);

            pToY++;
            uY++;
        }

        pToZ++;
        uZ++;
    }


    hr = S_OK;
    goto LDone;

LOutOfMemory:
    hr = E_OUTOFMEMORY;
    goto LDone;

LDone:
    delete [] pbXFilter;
    delete [] pbYFilter;
    delete [] pbZFilter;

    delete [] pDest;
    delete [] pSrc;

    return hr;
}




// ---------------------------------------------------------------------------
//
// TF - Triangle filter
//
// This section of the code defines a bunch of structures, macros, and
// functions which are used for the triangle filter.
//
// ---------------------------------------------------------------------------

#define TF_EPSILON 0.00001f

struct TF_To
{
    UINT uTo;           // mapping
    FLOAT fWeight;      // mapping weight
};

struct TF_From
{
    UINT uSize;         // size, in bytes
    TF_To pTo[1];       // may continue past 1
};

struct TF_Filter
{
    UINT uSize;         // size, in bytes
    TF_From pFrom[1];   // may continue past 1
};

struct TF_Row
{
    D3DXCOLOR *pclr;    // row data
    FLOAT fWeight;
    TF_Row *pNext;

    ~TF_Row() {
        delete [] pclr;
        delete pNext;
    }
};


static const UINT TF_uFilterSize = sizeof(TF_Filter) - sizeof(TF_From);
static const UINT TF_uFromSize   = sizeof(TF_From) - sizeof(TF_To);
static const UINT TF_uToSize     = sizeof(TF_To);


//
// Setup a triangle filter.  Each source pixel contriubtes equally to the
// destination image.
//

static UINT8 *
TF_SetupTriangle(UINT uSrcLim, UINT uDstLim, BOOL bRepeat)
{
    if(!uSrcLim || !uDstLim)
        return NULL;


    // Compute scale
    FLOAT fScale = (FLOAT) uDstLim / (FLOAT) uSrcLim;
    FLOAT f2ScaleInv = 0.5f / fScale;



    // Allocate memory needed to describe the filter.
    UINT uSizeMax = TF_uFilterSize + TF_uFromSize + TF_uToSize;
    UINT uRepeat = bRepeat ? 1 : 0;

    for(UINT uSrc = 0; uSrc < uSrcLim; uSrc++)
    {
        FLOAT fSrc = (FLOAT) uSrc - 0.5f;
        FLOAT fDstMin = fSrc * fScale;
        FLOAT fDstLim = fDstMin + fScale;

        uSizeMax += TF_uFromSize + TF_uToSize +
            (UINT) (fDstLim - fDstMin + uRepeat + 1) * TF_uToSize * 2;
    }

    UINT8 *pbFilter = new UINT8[uSizeMax];

    if(!pbFilter)
        return NULL;


    // Initialize filter
    UINT uSize = TF_uFilterSize;

    // Initialize accumulators
    UINT uAccumDst = 0;
    FLOAT fAccumWeight = 0.0f;



    for(uSrc = 0; uSrc < uSrcLim; uSrc++)
    {
        // Initialize TF_From block
        UINT uSizeFrom = uSize;
        TF_From *pFrom = (TF_From *) (pbFilter + uSize);
        uSize += TF_uFromSize;
        D3DXASSERT(uSize <= uSizeMax);


        // For each source pixel, we perform two passes.  During the first
        // pass, we record the influences due to this source pixel as we
        // approach it.  During the second pass, we record the influences
        // as we move away from it.

        for(UINT uPass = 0; uPass < 2; uPass++)
        {
            FLOAT fSrc = ((FLOAT) uSrc + uPass) - 0.5f;

            // Start and end of source pixel, in destination coordinates
            FLOAT fDstMin = fSrc * fScale;
            FLOAT fDstLim = fDstMin + fScale;


            // Clamp source pixel to destination image.
            if(!bRepeat)
            {
                if(fDstMin < 0.0f)
                    fDstMin = 0.0f;
                if(fDstLim > (FLOAT) uDstLim)
                    fDstLim = (FLOAT) uDstLim;
            }

            // Start of first destination pixel
            INT nDst = (INT) floorf(fDstMin);



            while((FLOAT) nDst < fDstLim)
            {
                FLOAT fDst0 = (FLOAT) nDst;
                FLOAT fDst1 = fDst0 + 1.0f;

                UINT uDst;
                if(nDst < 0)
                    uDst = (UINT) nDst + uDstLim;
                else if(nDst >= (INT) uDstLim)
                    uDst = (UINT) nDst - uDstLim;
                else
                    uDst = (UINT) nDst;


                // Save the accumulated weight from uAccumDst if we have moved
                // on to accumulating for the next pixel.

                if(uDst != uAccumDst)
                {
                    if(fAccumWeight > TF_EPSILON)
                    {
                        TF_To *pTo  = (TF_To *) (pbFilter + uSize);
                        uSize += TF_uToSize;

                        D3DXASSERT(uSize <= uSizeMax);

                        pTo->uTo = uAccumDst;
                        pTo->fWeight = fAccumWeight;

                    }

                    fAccumWeight = 0.0f;
                    uAccumDst = uDst;
                }


                // Clip destination pixel to source pixel.
                if(fDst0 < fDstMin)
                    fDst0 = fDstMin;
                if(fDst1 > fDstLim)
                    fDst1 = fDstLim;


                // Calculate average weight over destination pixel.  Since the
                // weight varies linearly, when the average weight is
                // multiplied by the size of the destination pixel, the result
                // is equal to the integral of the weight over this portion of
                // the pixel.

                FLOAT fWeight;
                if(!bRepeat && fSrc < 0.0f)
                    fWeight = 1.0f;
                else if(!bRepeat && fSrc + 1.0f >= (FLOAT) uSrcLim)
                    fWeight = 0.0f;
                else
                    fWeight = (fDst0 + fDst1) * f2ScaleInv - fSrc;

                fAccumWeight += (fDst1 - fDst0) * (uPass ? 1.0f - fWeight : fWeight);
                nDst++;
            }
        }


        // If there is a valid value still in the accumulator, it needs to be
        // written out.

        if(fAccumWeight > TF_EPSILON)
        {
            TF_To *pTo = (TF_To *) (pbFilter + uSize);
            uSize += TF_uToSize;

            D3DXASSERT(uSize <= uSizeMax);

            pTo->uTo = uAccumDst;
            pTo->fWeight = fAccumWeight;
        }

        fAccumWeight = 0.0f;


        // Record TF_From's size
        pFrom->uSize = uSize - uSizeFrom;
    }


    // Record TF_Filter's size
    ((TF_Filter *) pbFilter)->uSize = uSize;

    return pbFilter;
}




HRESULT
CXD3DXBlt::BltTriangle2D()
{
    HRESULT hr;
    UINT8 *pbXFilter, *pbYFilter;
    TF_Filter *pXFilter, *pYFilter;
    TF_From *pXFrom, *pYFrom, *pXFromLim, *pYFromLim;
    TF_To *pXTo, *pYTo, *pXToLim, *pYToLim;
    TF_Row *pRow, **ppRowActive, *pRowFree;
    BOOL bRepeatX, bRepeatY;
    UINT uRowsActive;
    UINT uSrcRow;
    D3DXCOLOR *pclrSrc;
    D3DXCOLOR *pclrPal;


    pbXFilter   = NULL;
    pbYFilter   = NULL;
    ppRowActive = NULL;
    pRowFree    = NULL;
    pclrSrc     = NULL;
    pclrPal     = NULL;


//  DPF(0, "blt triangle %d->%d", m_pSrc->m_Format, m_pDest->m_Format);

    // Create filters for each axis
    if(m_pDest->m_dwType != CODEC_RGB || m_pSrc->m_dwType != CODEC_RGB)
        return E_FAIL;

    if(m_pDest->m_uDepth != 1 || m_pSrc->m_uDepth != 1)
        return E_FAIL;

    bRepeatX = !(m_dwFilter & D3DX_FILTER_MIRROR_U);
    bRepeatY = !(m_dwFilter & D3DX_FILTER_MIRROR_V);

    if(!(pbXFilter = TF_SetupTriangle(m_pSrc->m_uWidth,  m_pDest->m_uWidth,  bRepeatX)) ||
       !(pbYFilter = TF_SetupTriangle(m_pSrc->m_uHeight, m_pDest->m_uHeight, bRepeatY)))
    {
        hr = E_FAIL;
        goto LDone;
    }



    pXFilter = (TF_Filter *) pbXFilter;
    pYFilter = (TF_Filter *) pbYFilter;

    pXFromLim = (TF_From *) ((UINT8 *) pXFilter + pXFilter->uSize);
    pYFromLim = (TF_From *) ((UINT8 *) pYFilter + pYFilter->uSize);


    // Initialize accumulation rows
    if(!(ppRowActive = new TF_Row* [m_pDest->m_uHeight]))
        goto LOutOfMemory;

    memset(ppRowActive, 0, m_pDest->m_uHeight * sizeof(TF_Row *));
    uRowsActive = 0;

    if(!(pclrSrc = new D3DXCOLOR[m_pSrc->m_uWidth]))
        goto LOutOfMemory;



    // Filter image
    uSrcRow = 0;

    for(pYFrom = pYFilter->pFrom; pYFrom < pYFromLim; pYFrom = (TF_From *) pYToLim)
    {
        D3DXCOLOR *pclrSrcX;

        pYToLim = (TF_To *) ((UINT8 *) pYFrom + pYFrom->uSize);

        if(pYFrom->pTo < pYToLim)
        {

            // Create necessary accumulation rows
            for(pYTo = pYFrom->pTo; pYTo < pYToLim; pYTo++)
            {
                if(!(pRow = ppRowActive[pYTo->uTo]))
                {
                    if(pRowFree)
                    {
                        // Reuse a row
                        pRow = pRowFree;
                        pRowFree = pRow->pNext;
                    }
                    else
                    {
                        // Allocate a new row
                        if(!(pRow = new TF_Row))
                            goto LOutOfMemory;

                        if(!(pRow->pclr = new D3DXCOLOR[m_pDest->m_uWidth]))
                        {
                            delete pRow;
                            goto LOutOfMemory;
                        }
                    }

                    memset(pRow->pclr, 0, m_pDest->m_uWidth * sizeof(D3DXCOLOR));
                    pRow->fWeight = 0.0f;
                    pRow->pNext = NULL;

                    ppRowActive[pYTo->uTo] = pRow;
                    uRowsActive++;
                }
            }


            // Read source pixels
            m_pSrc->Decode(uSrcRow, 0, pclrSrc);


            // Process a row from the source image
            pclrSrcX = pclrSrc;
            for(pXFrom = pXFilter->pFrom; pXFrom < pXFromLim; pXFrom = (TF_From *) pXToLim)
            {
                pXToLim = (TF_To *) ((UINT8 *) pXFrom + pXFrom->uSize);

                for(pYTo = pYFrom->pTo; pYTo < pYToLim; pYTo++)
                {
                    pRow = ppRowActive[pYTo->uTo];

                    for(pXTo = pXFrom->pTo; pXTo < pXToLim; pXTo++)
                    {
                        FLOAT fWeight =  pYTo->fWeight * pXTo->fWeight;

                        pRow->pclr[pXTo->uTo].r += pclrSrcX->r * fWeight;
                        pRow->pclr[pXTo->uTo].g += pclrSrcX->g * fWeight;
                        pRow->pclr[pXTo->uTo].b += pclrSrcX->b * fWeight;
                        pRow->pclr[pXTo->uTo].a += pclrSrcX->a * fWeight;
                    }
                }

                pclrSrcX++;
            }


            // Write completed accumulation rows
            for(pYTo = pYFrom->pTo; pYTo < pYToLim; pYTo++)
            {
                pRow = ppRowActive[pYTo->uTo];
                pRow->fWeight += pYTo->fWeight;

                if(pRow->fWeight + TF_EPSILON >= 1.0f)
                {
                    // Write and free row
                    m_pDest->Encode(pYTo->uTo, 0, pRow->pclr);

                    ppRowActive[pYTo->uTo] = NULL;
                    pRow->pNext = pRowFree;
                    pRowFree = pRow;

                    uRowsActive--;
                }
            }
        }

        uSrcRow++;
    }


    // Make sure that all accumulation rows have been written out.  This should
    // only happen if the filter was not constructed correctly.
    if(uRowsActive)
    {
        for(UINT uRow = 0; uRow < m_pDest->m_uHeight; uRow++)
        {
            if(ppRowActive[uRow])
            {
                // Write and delete row
                m_pDest->Encode(uRow, 0, ppRowActive[uRow]->pclr);
                delete ppRowActive[uRow];

                if(--uRowsActive == 0)
                    break;
            }
        }
    }


    hr = S_OK;
    goto LDone;

LOutOfMemory:
    hr = E_OUTOFMEMORY;
    goto LDone;

LDone:
    // Clean up
    delete[] ppRowActive;
    delete pRowFree;

    delete [] pbYFilter;
    delete [] pbXFilter;
    delete [] pclrSrc;
    delete [] pclrPal;

    return hr;
}



HRESULT
CXD3DXBlt::BltTriangle3D()
{
    HRESULT hr;
    UINT8 *pbXFilter, *pbYFilter, *pbZFilter;
    TF_Filter *pXFilter, *pYFilter, *pZFilter;
    TF_From *pXFrom, *pYFrom, *pZFrom, *pXFromLim, *pYFromLim, *pZFromLim;
    TF_To *pXTo, *pYTo, *pZTo, *pXToLim, *pYToLim, *pZToLim;
    TF_Row *pSlice, **ppSliceActive, *pSliceFree;
    BOOL bRepeatX, bRepeatY, bRepeatZ;
    UINT uSlicesActive;
    UINT uSrcRow, uSrcSlice;
    D3DXCOLOR *pclrSrc;
    D3DXCOLOR *pclrPal;


    pbXFilter     = NULL;
    pbYFilter     = NULL;
    pbZFilter     = NULL;
    ppSliceActive = NULL;
    pSliceFree    = NULL;
    pclrSrc       = NULL;
    pclrPal       = NULL;


//  DPF(0, "blt triangle %d->%d", m_pSrc->m_Format, m_pDest->m_Format);

    // Create filters for each axis
    if(m_pDest->m_dwType != CODEC_RGB || m_pSrc->m_dwType != CODEC_RGB)
        return E_FAIL;

    bRepeatX = !(m_dwFilter & D3DX_FILTER_MIRROR_U);
    bRepeatY = !(m_dwFilter & D3DX_FILTER_MIRROR_V);
    bRepeatZ = !(m_dwFilter & D3DX_FILTER_MIRROR_W);

    if(!(pbXFilter = TF_SetupTriangle(m_pSrc->m_uWidth,  m_pDest->m_uWidth,  bRepeatX)) ||
       !(pbYFilter = TF_SetupTriangle(m_pSrc->m_uHeight, m_pDest->m_uHeight, bRepeatY)) ||
       !(pbZFilter = TF_SetupTriangle(m_pSrc->m_uDepth,  m_pDest->m_uDepth,  bRepeatZ)))
    {
        hr = E_FAIL;
        goto LDone;
    }



    pXFilter = (TF_Filter *) pbXFilter;
    pYFilter = (TF_Filter *) pbYFilter;
    pZFilter = (TF_Filter *) pbZFilter;

    pXFromLim = (TF_From *) ((UINT8 *) pXFilter + pXFilter->uSize);
    pYFromLim = (TF_From *) ((UINT8 *) pYFilter + pYFilter->uSize);
    pZFromLim = (TF_From *) ((UINT8 *) pZFilter + pZFilter->uSize);


    // Initialize accumulation slices
    if(!(ppSliceActive = new TF_Row* [m_pDest->m_uDepth]))
        goto LOutOfMemory;

    memset(ppSliceActive, 0, m_pDest->m_uDepth * sizeof(TF_Row *));
    uSlicesActive = 0;

    if(!(pclrSrc = new D3DXCOLOR[m_pSrc->m_uWidth]))
        goto LOutOfMemory;



    // Filter image
    uSrcSlice = 0;
    for(pZFrom = pZFilter->pFrom; pZFrom < pZFromLim; pZFrom = (TF_From *) pZToLim)
    {
        pZToLim = (TF_To *) ((UINT8 *) pZFrom + pZFrom->uSize);

        // Create necessary accumulation slices
        for(pZTo = pZFrom->pTo; pZTo < pZToLim; pZTo++)
        {
            if(!(pSlice = ppSliceActive[pZTo->uTo]))
            {
                if(pSliceFree)
                {
                    // Reuse a row
                    pSlice = pSliceFree;
                    pSliceFree = pSlice->pNext;
                }
                else
                {
                    // Allocate a new row
                    if(!(pSlice = new TF_Row))
                        goto LOutOfMemory;

                    if(!(pSlice->pclr = new D3DXCOLOR[m_pDest->m_uWidth * m_pDest->m_uHeight]))
                    {
                        delete pSlice;
                        goto LOutOfMemory;
                    }
                }

                memset(pSlice->pclr, 0, m_pDest->m_uWidth * m_pDest->m_uHeight * sizeof(D3DXCOLOR));
                pSlice->fWeight = 0.0f;
                pSlice->pNext = NULL;

                ppSliceActive[pZTo->uTo] = pSlice;
                uSlicesActive++;
            }
        }

        uSrcRow = 0;
        for(pYFrom = pYFilter->pFrom; pYFrom < pYFromLim; pYFrom = (TF_From *) pYToLim)
        {

            pYToLim = (TF_To *) ((UINT8 *) pYFrom + pYFrom->uSize);

            // Read source pixels
            m_pSrc->Decode(uSrcRow, uSrcSlice, pclrSrc);


            // Process a row from the source image
            D3DXCOLOR *pclrSrcX, *pclrDest;
            pclrSrcX = pclrSrc;

            for(pXFrom = pXFilter->pFrom; pXFrom < pXFromLim; pXFrom = (TF_From *) pXToLim)
            {
                pXToLim = (TF_To *) ((UINT8 *) pXFrom + pXFrom->uSize);

                for(pZTo = pZFrom->pTo; pZTo < pZToLim; pZTo++)
                {
                    for(pYTo = pYFrom->pTo; pYTo < pYToLim; pYTo++)
                    {
                        pclrDest = ppSliceActive[pZTo->uTo]->pclr + pYTo->uTo * m_pDest->m_uWidth;

                        for(pXTo = pXFrom->pTo; pXTo < pXToLim; pXTo++)
                        {
                            FLOAT fWeight = pZTo->fWeight * pYTo->fWeight * pXTo->fWeight;

                            pclrDest[pXTo->uTo].r += pclrSrcX->r * fWeight;
                            pclrDest[pXTo->uTo].g += pclrSrcX->g * fWeight;
                            pclrDest[pXTo->uTo].b += pclrSrcX->b * fWeight;
                            pclrDest[pXTo->uTo].a += pclrSrcX->a * fWeight;
                        }
                    }
                }

                pclrSrcX++;
            }

            uSrcRow++;
        }


        // Write completed accumulation slices
        for(pZTo = pZFrom->pTo; pZTo < pZToLim; pZTo++)
        {
            pSlice = ppSliceActive[pZTo->uTo];
            pSlice->fWeight += pZTo->fWeight;

            if(pSlice->fWeight + TF_EPSILON >= 1.0f)
            {
                // Write and free slice                
                for(UINT uRow = 0; uRow < m_pDest->m_uHeight; uRow++)
                    m_pDest->Encode(uRow, pZTo->uTo, pSlice->pclr + uRow * m_pDest->m_uWidth);

                ppSliceActive[pZTo->uTo] = NULL;
                pSlice->pNext = pSliceFree;
                pSliceFree = pSlice;

                uSlicesActive--;
            }
        }

        uSrcSlice++;
    }


    // Make sure that all accumulation slices have been written out.  This should
    // only happen if the filter was not constructed correctly.
    if(uSlicesActive)
    {
        for(UINT uSlice = 0; uSlice < m_pDest->m_uDepth; uSlice++)
        {
            if(ppSliceActive[uSlice])
            {
                // Write and delete slice
                for(UINT uRow = 0; uRow < m_pDest->m_uHeight; uRow++)
                    m_pDest->Encode(uRow, uSlice, pSlice->pclr + uRow * m_pDest->m_uWidth);

                delete ppSliceActive[uSlice];

                if(--uSlicesActive == 0)
                    break;
            }
        }
    }


    hr = S_OK;
    goto LDone;

LOutOfMemory:
    hr = E_OUTOFMEMORY;
    goto LDone;

LDone:
    // Clean up
    delete[] ppSliceActive;
    delete pSliceFree;

    delete [] pbZFilter;
    delete [] pbYFilter;
    delete [] pbXFilter;
    delete [] pclrSrc;
    delete [] pclrPal;

    return hr;
}




// ---------------------------------------------------------------------------
//
// Optimized filters
//
// ---------------------------------------------------------------------------

#ifdef SUPPORT_DXT
HRESULT CXD3DXBlt::BltSame_DXTn()
{
    if((m_pSrc->m_Box.Left  | m_pSrc->m_Box.Top  | m_pSrc->m_Box.Right  | m_pSrc->m_Box.Bottom)  & 3)
        return E_FAIL;

    if((m_pDest->m_Box.Left | m_pDest->m_Box.Top | m_pDest->m_Box.Right | m_pDest->m_Box.Bottom) & 3)
        return E_FAIL;


    // Calculate block size and width
    UINT cbBlock, cbWidth;

    switch(m_pDest->m_Format)
    {
    case D3DFMT_DXT1:
        cbBlock = sizeof(S3TCBlockRGB);
        break;

    case D3DFMT_DXT2:
#if 0
    case D3DFMT_DXT3:
#endif
        cbBlock = sizeof(S3TCBlockAlpha4);
        break;

    case D3DFMT_DXT4:
#if 0
    case D3DFMT_DXT5:
#endif
        cbBlock = sizeof(S3TCBlockAlpha3);
        break;

    default:
        return E_FAIL;
    }


    cbWidth = (m_pDest->m_uWidth >> 2) * cbBlock;


    // Copy DXTn blocks
    BYTE *pbSliceDest = m_pDest->m_pbData + (m_pDest->m_Box.Left >> 2) * cbBlock + 
        (m_pDest->m_Box.Top >> 2) * m_pDest->m_uPitch + m_pDest->m_Box.Front * m_pDest->m_uSlice;

    BYTE *pbSliceSrc = m_pSrc->m_pbData + (m_pSrc->m_Box.Left >> 2) * cbBlock + 
        (m_pSrc->m_Box.Top >> 2) * m_pSrc->m_uPitch + m_pSrc->m_Box.Front * m_pSrc->m_uSlice;


    for(UINT uZ = 0; uZ < m_pDest->m_uDepth; uZ++)
    {
        BYTE *pbDest = pbSliceDest;
        BYTE *pbSrc  = pbSliceSrc;

        for(UINT uY = 0; uY < m_pDest->m_uHeight; uY += 4)
        {
            memcpy(pbDest, pbSrc, cbWidth);

            pbDest += m_pDest->m_uPitch;
            pbSrc  += m_pSrc ->m_uPitch;
        }

        pbSliceDest += m_pDest->m_uSlice;
        pbSliceDest += m_pSrc ->m_uSlice;
    }

    return S_OK;
}
#endif // SUPPORT_DXT


HRESULT 
CXD3DXBlt::BltBox2D_R8G8B8()
{
    return E_NOTIMPL;
}


HRESULT 
CXD3DXBlt::BltBox2D_A8R8G8B8()
{
    UINT32 *pulDest   = (UINT32 *) m_pDest->m_pbData;
    UINT32 *pulSrc    = (UINT32 *) m_pSrc->m_pbData;
    UINT32 *pulSrcLim = (UINT32 *) (m_pSrc->m_pbData + m_pSrc->m_uPitch * m_pSrc->m_uHeight);

    while(pulSrc < pulSrcLim)
    {
        UINT32 *pul     = pulDest;
        UINT32 *pulA    = pulSrc;
        UINT32 *pulB    = (UINT32 *) ((BYTE *) pulA + m_pSrc->m_uPitch);
        UINT32 *pulALim = pulA + m_pSrc->m_uWidth;

        while(pulA < pulALim)
        {
            *pul++ = (((((pulA[0] & 0x00ff00ff) + (pulA[1] & 0x00ff00ff) + 
                         (pulB[0] & 0x00ff00ff) + (pulB[1] & 0x00ff00ff)) + 0x00020002) >> 2) & 0x00ff00ff) |

                     (((((pulA[0] & 0xff00ff00) >> 2) + ((pulA[1] & 0xff00ff00) >> 2) +
                        ((pulB[0] & 0xff00ff00) >> 2) + ((pulB[1] & 0xff00ff00) >> 2)) + (0x02000200 >> 2)) & 0xff00ff00);

            pulA += 2;
            pulB += 2;
        }

        pulDest = (UINT32 *) ((BYTE *) pulDest + m_pDest->m_uPitch);
        pulSrc  = (UINT32 *) ((BYTE *) pulSrc + m_pSrc->m_uPitch + m_pSrc->m_uPitch);
    }

    return S_OK;
}


HRESULT 
CXD3DXBlt::BltBox2D_X8R8G8B8()
{
    UINT32 *pulDest   = (UINT32 *) m_pDest->m_pbData;
    UINT32 *pulSrc    = (UINT32 *) m_pSrc->m_pbData;
    UINT32 *pulSrcLim = (UINT32 *) (m_pSrc->m_pbData + m_pSrc->m_uPitch * m_pSrc->m_uHeight);

    while(pulSrc < pulSrcLim)
    {
        UINT32 *pul     = pulDest;
        UINT32 *pulA    = pulSrc;
        UINT32 *pulB    = (UINT32 *) ((BYTE *) pulA + m_pSrc->m_uPitch);
        UINT32 *pulALim = pulA + m_pSrc->m_uWidth;

        while(pulA < pulALim)
        {
            *pul++ = ((((pulA[0] & 0x00ff00ff) + (pulA[1] & 0x00ff00ff) + 
                        (pulB[0] & 0x00ff00ff) + (pulB[1] & 0x00ff00ff) + 0x00020002) & (0x00ff00ff << 2)) |

                      (((pulA[0] & 0x0000ff00) + (pulA[1] & 0x0000ff00) +
                        (pulB[0] & 0x0000ff00) + (pulB[1] & 0x0000ff00) + 0x00000200) & (0x0000ff00 << 2))) >> 2;

            pulA += 2;
            pulB += 2;
        }

        pulDest = (UINT32 *) ((BYTE *) pulDest + m_pDest->m_uPitch);
        pulSrc  = (UINT32 *) ((BYTE *) pulSrc + m_pSrc->m_uPitch + m_pSrc->m_uPitch);
    }

    return S_OK;
}


HRESULT 
CXD3DXBlt::BltBox2D_R5G6B5()
{
    UINT16 *pusDest   = (UINT16 *) m_pDest->m_pbData;
    UINT16 *pusSrc    = (UINT16 *) m_pSrc->m_pbData;
    UINT16 *pusSrcLim = (UINT16 *) (m_pSrc->m_pbData + m_pSrc->m_uPitch * m_pSrc->m_uHeight);

    while(pusSrc < pusSrcLim)
    {
        UINT16 *pus     = pusDest;
        UINT16 *pusA    = pusSrc;
        UINT16 *pusB    = (UINT16 *) ((BYTE *) pusA + m_pSrc->m_uPitch);
        UINT16 *pusALim = pusA + m_pSrc->m_uWidth;

        while(pusA < pusALim)
        {
            *pus++ = (UINT16) ((((((UINT32) pusA[0] & 0xf81f) + ((UINT32) pusA[1] & 0xf81f) + 
                                  ((UINT32) pusB[0] & 0xf81f) + ((UINT32) pusB[1] & 0xf81f) + 0x1002) & (0xf81f << 2)) |

                                ((((UINT32) pusA[0] & 0x07e0) + ((UINT32) pusA[1] & 0x07e0) + 
                                  ((UINT32) pusB[0] & 0x07e0) + ((UINT32) pusB[1] & 0x07e0) + 0x0040) & (0x07e0 << 2))) >> 2);

            pusA += 2;
            pusB += 2;
        }

        pusDest = (UINT16 *) ((BYTE *) pusDest + m_pDest->m_uPitch);
        pusSrc  = (UINT16 *) ((BYTE *) pusSrc + m_pSrc->m_uPitch + m_pSrc->m_uPitch);
    }

    return S_OK;
}


HRESULT 
CXD3DXBlt::BltBox2D_X1R5G5B5()
{
    UINT16 *pusDest   = (UINT16 *) m_pDest->m_pbData;
    UINT16 *pusSrc    = (UINT16 *) m_pSrc->m_pbData;
    UINT16 *pusSrcLim = (UINT16 *) (m_pSrc->m_pbData + m_pSrc->m_uPitch * m_pSrc->m_uHeight);

    while(pusSrc < pusSrcLim)
    {
        UINT16 *pus     = pusDest;
        UINT16 *pusA    = pusSrc;
        UINT16 *pusB    = (UINT16 *) ((BYTE *) pusA + m_pSrc->m_uPitch);
        UINT16 *pusALim = pusA + m_pSrc->m_uWidth;

        while(pusA < pusALim)
        {
            *pus++ = (UINT16) ((((((UINT32) pusA[0] & 0x7c1f) + ((UINT32) pusA[1] & 0x7c1f) + 
                                  ((UINT32) pusB[0] & 0x7c1f) + ((UINT32) pusB[1] & 0x7c1f) + 0x0802) & (0x7c1f << 2)) |

                                ((((UINT32) pusA[0] & 0x03e0) + ((UINT32) pusA[1] & 0x03e0) + 
                                  ((UINT32) pusB[0] & 0x03e0) + ((UINT32) pusB[1] & 0x03e0) + 0x0040) & (0x03e0 << 2))) >> 2);

            pusA += 2;
            pusB += 2;
        }

        pusDest = (UINT16 *) ((BYTE *) pusDest + m_pDest->m_uPitch);
        pusSrc  = (UINT16 *) ((BYTE *) pusSrc + m_pSrc->m_uPitch + m_pSrc->m_uPitch);
    }

    return S_OK;
}


HRESULT 
CXD3DXBlt::BltBox2D_A1R5G5B5()
{
    UINT16 *pusDest   = (UINT16 *) m_pDest->m_pbData;
    UINT16 *pusSrc    = (UINT16 *) m_pSrc->m_pbData;
    UINT16 *pusSrcLim = (UINT16 *) (m_pSrc->m_pbData + m_pSrc->m_uPitch * m_pSrc->m_uHeight);

    while(pusSrc < pusSrcLim)
    {
        UINT16 *pus     = pusDest;
        UINT16 *pusA    = pusSrc;
        UINT16 *pusB    = (UINT16 *) ((BYTE *) pusA + m_pSrc->m_uPitch);
        UINT16 *pusALim = pusA + m_pSrc->m_uWidth;

        while(pusA < pusALim)
        {
            *pus++ = (UINT16) ((((((UINT32) pusA[0] & 0x7c1f) + ((UINT32) pusA[1] & 0x7c1f) + 
                                  ((UINT32) pusB[0] & 0x7c1f) + ((UINT32) pusB[1] & 0x7c1f) + 0x0802) & (0x7c1f << 2)) |

                                ((((UINT32) pusA[0] & 0x83e0) + ((UINT32) pusA[1] & 0x83e0) + 
                                  ((UINT32) pusB[0] & 0x83e0) + ((UINT32) pusB[1] & 0x83e0) + 0x10040) & (0x83e0 << 2))) >> 2);

            pusA += 2;
            pusB += 2;
        }

        pusDest = (UINT16 *) ((BYTE *) pusDest + m_pDest->m_uPitch);
        pusSrc  = (UINT16 *) ((BYTE *) pusSrc + m_pSrc->m_uPitch + m_pSrc->m_uPitch);
    }

    return S_OK;
}


HRESULT 
CXD3DXBlt::BltBox2D_A4R4G4B4()
{
    UINT16 *pusDest   = (UINT16 *) m_pDest->m_pbData;
    UINT16 *pusSrc    = (UINT16 *) m_pSrc->m_pbData;
    UINT16 *pusSrcLim = (UINT16 *) (m_pSrc->m_pbData + m_pSrc->m_uPitch * m_pSrc->m_uHeight);

    while(pusSrc < pusSrcLim)
    {
        UINT16 *pus     = pusDest;
        UINT16 *pusA    = pusSrc;
        UINT16 *pusB    = (UINT16 *) ((BYTE *) pusA + m_pSrc->m_uPitch);
        UINT16 *pusALim = pusA + m_pSrc->m_uWidth;

        while(pusA < pusALim)
        {
            *pus++ = (UINT16) ((((((UINT32) pusA[0] & 0x0f0f) + ((UINT32) pusA[1] & 0x0f0f) + 
                                  ((UINT32) pusB[0] & 0x0f0f) + ((UINT32) pusB[1] & 0x0f0f) + 0x0202) & (0x0f0f << 2)) |

                                ((((UINT32) pusA[0] & 0xf0f0) + ((UINT32) pusA[1] & 0xf0f0) + 
                                  ((UINT32) pusB[0] & 0xf0f0) + ((UINT32) pusB[1] & 0xf0f0) + 0x2020) & (0xf0f0 << 2))) >> 2);

            pusA += 2;
            pusB += 2;
        }

        pusDest = (UINT16 *) ((BYTE *) pusDest + m_pDest->m_uPitch);
        pusSrc  = (UINT16 *) ((BYTE *) pusSrc + m_pSrc->m_uPitch + m_pSrc->m_uPitch);
    }

    return S_OK;
}


HRESULT 
CXD3DXBlt::BltBox2D_R3G3B2() 
{
#if 0
    UINT8 *pubDest   = (UINT8 *) m_pDest->m_pbData;
    UINT8 *pubSrc    = (UINT8 *) m_pSrc->m_pbData;
    UINT8 *pubSrcLim = (UINT8 *) (m_pSrc->m_pbData + m_pSrc->m_uPitch * m_pSrc->m_uHeight);

    while(pubSrc < pubSrcLim)
    {
        UINT8 *pub     = pubDest;
        UINT8 *pubA    = pubSrc;
        UINT8 *pubB    = (UINT8 *) ((BYTE *) pubA + m_pSrc->m_uPitch);
        UINT8 *pubALim = pubA + m_pSrc->m_uWidth;

        while(pubA < pubALim)
        {
            *pub++ = (UINT8) ((((((UINT32) pubA[0] & 0xe3) + ((UINT32) pubA[1] & 0xe3) + 
                                 ((UINT32) pubB[0] & 0xe3) + ((UINT32) pubB[1] & 0xe3) + 0x42) & (0xe3 << 2)) |

                               ((((UINT32) pubA[0] & 0x1c) + ((UINT32) pubA[1] & 0x1c) + 
                                 ((UINT32) pubB[0] & 0x1c) + ((UINT32) pubB[1] & 0x1c) + 0x08) & (0x1c << 2))) >> 2);

            pubA += 2;
            pubB += 2;
        }

        pubDest = (UINT8 *) ((BYTE *) pubDest + m_pDest->m_uPitch);
        pubSrc  = (UINT8 *) ((BYTE *) pubSrc + m_pSrc->m_uPitch + m_pSrc->m_uPitch);
    }
    return S_OK;
#endif
    return E_NOTIMPL;
}


HRESULT 
CXD3DXBlt::BltBox2D_A8()
{
    UINT8 *pubDest   = (UINT8 *) m_pDest->m_pbData;
    UINT8 *pubSrc    = (UINT8 *) m_pSrc->m_pbData;
    UINT8 *pubSrcLim = (UINT8 *) (m_pSrc->m_pbData + m_pSrc->m_uPitch * m_pSrc->m_uHeight);

    while(pubSrc < pubSrcLim)
    {
        UINT8 *pub     = pubDest;
        UINT8 *pubA    = pubSrc;
        UINT8 *pubB    = (UINT8 *) ((BYTE *) pubA + m_pSrc->m_uPitch);
        UINT8 *pubALim = pubA + m_pSrc->m_uWidth;

        while(pubA < pubALim)
        {
            *pub++ = (UINT8) (((UINT32) pubA[0] + (UINT32) pubA[1] + 
                               (UINT32) pubB[0] + (UINT32) pubB[1] + 0x02) >> 2);

            pubA += 2;
            pubB += 2;
        }

        pubDest = (UINT8 *) ((BYTE *) pubDest + m_pDest->m_uPitch);
        pubSrc  = (UINT8 *) ((BYTE *) pubSrc + m_pSrc->m_uPitch + m_pSrc->m_uPitch);
    }

    return S_OK;
}


HRESULT 
CXD3DXBlt::BltBox2D_A8R3G3B2()
{
#if 0
    UINT16 *pusDest   = (UINT16 *) m_pDest->m_pbData;
    UINT16 *pusSrc    = (UINT16 *) m_pSrc->m_pbData;
    UINT16 *pusSrcLim = (UINT16 *) (m_pSrc->m_pbData + m_pSrc->m_uPitch * m_pSrc->m_uHeight);

    while(pusSrc < pusSrcLim)
    {
        UINT16 *pus     = pusDest;
        UINT16 *pusA    = pusSrc;
        UINT16 *pusB    = (UINT16 *) ((BYTE *) pusA + m_pSrc->m_uPitch);
        UINT16 *pusALim = pusA + m_pSrc->m_uWidth;

        while(pusA < pusALim)
        {
            *pus++ = (UINT16) ((((((UINT32) pusA[0] & 0x00e3) + ((UINT32) pusA[1] & 0x00e3) + 
                                  ((UINT32) pusB[0] & 0x00e3) + ((UINT32) pusB[1] & 0x00e3) + 0x0042) & (0x00e3 << 2)) |

                                ((((UINT32) pusA[0] & 0xff1c) + ((UINT32) pusA[1] & 0xff1c) + 
                                  ((UINT32) pusB[0] & 0xff1c) + ((UINT32) pusB[1] & 0xff1c) + 0x0208) & (0xff1c << 2))) >> 2);

            pusA += 2;
            pusB += 2;
        }

        pusDest = (UINT16 *) ((BYTE *) pusDest + m_pDest->m_uPitch);
        pusSrc  = (UINT16 *) ((BYTE *) pusSrc + m_pSrc->m_uPitch + m_pSrc->m_uPitch);
    }
    return S_OK;
#endif
    return E_NOTIMPL;
}


HRESULT 
CXD3DXBlt::BltBox2D_X4R4G4B4()
{
#if 0
    UINT16 *pusDest   = (UINT16 *) m_pDest->m_pbData;
    UINT16 *pusSrc    = (UINT16 *) m_pSrc->m_pbData;
    UINT16 *pusSrcLim = (UINT16 *) (m_pSrc->m_pbData + m_pSrc->m_uPitch * m_pSrc->m_uHeight);

    while(pusSrc < pusSrcLim)
    {
        UINT16 *pus     = pusDest;
        UINT16 *pusA    = pusSrc;
        UINT16 *pusB    = (UINT16 *) ((BYTE *) pusA + m_pSrc->m_uPitch);
        UINT16 *pusALim = pusA + m_pSrc->m_uWidth;

        while(pusA < pusALim)
        {
            *pus++ = (UINT16) ((((((UINT32) pusA[0] & 0x0f0f) + ((UINT32) pusA[1] & 0x0f0f) + 
                                  ((UINT32) pusB[0] & 0x0f0f) + ((UINT32) pusB[1] & 0x0f0f) + 0x0202) & (0x0f0f << 2)) |

                                ((((UINT32) pusA[0] & 0x00f0) + ((UINT32) pusA[1] & 0x00f0) + 
                                  ((UINT32) pusB[0] & 0x00f0) + ((UINT32) pusB[1] & 0x00f0) + 0x0020) & (0x00f0 << 2))) >> 2);

            pusA += 2;
            pusB += 2;
        }

        pusDest = (UINT16 *) ((BYTE *) pusDest + m_pDest->m_uPitch);
        pusSrc  = (UINT16 *) ((BYTE *) pusSrc + m_pSrc->m_uPitch + m_pSrc->m_uPitch);
    }
    return S_OK;
#endif
    return E_NOTIMPL;
}


HRESULT 
CXD3DXBlt::BltBox2D_A8P8()
{
    return E_NOTIMPL;
}


HRESULT 
CXD3DXBlt::BltBox2D_P8()
{
    return E_NOTIMPL;
}


HRESULT 
CXD3DXBlt::BltBox2D_A8L8()
{
    UINT16 *pusDest   = (UINT16 *) m_pDest->m_pbData;
    UINT16 *pusSrc    = (UINT16 *) m_pSrc->m_pbData;
    UINT16 *pusSrcLim = (UINT16 *) (m_pSrc->m_pbData + m_pSrc->m_uPitch * m_pSrc->m_uHeight);

    while(pusSrc < pusSrcLim)
    {
        UINT16 *pus     = pusDest;
        UINT16 *pusA    = pusSrc;
        UINT16 *pusB    = (UINT16 *) ((BYTE *) pusA + m_pSrc->m_uPitch);
        UINT16 *pusALim = pusA + m_pSrc->m_uWidth;

        while(pusA < pusALim)
        {
            *pus++ = (UINT16) ((((((UINT32) pusA[0] & 0x00ff) + ((UINT32) pusA[1] & 0x00ff) + 
                                  ((UINT32) pusB[0] & 0x00ff) + ((UINT32) pusB[1] & 0x00ff) + 0x0002) & (0x00ff << 2)) |

                                ((((UINT32) pusA[0] & 0xff00) + ((UINT32) pusA[1] & 0xff00) + 
                                  ((UINT32) pusB[0] & 0xff00) + ((UINT32) pusB[1] & 0xff00) + 0x0200) & (0xff00 << 2))) >> 2);

            pusA += 2;
            pusB += 2;
        }

        pusDest = (UINT16 *) ((BYTE *) pusDest + m_pDest->m_uPitch);
        pusSrc  = (UINT16 *) ((BYTE *) pusSrc + m_pSrc->m_uPitch + m_pSrc->m_uPitch);
    }

    return S_OK;
}


HRESULT 
CXD3DXBlt::BltBox2D_A4L4()
{
#if 0
    UINT8 *pubDest   = (UINT8 *) m_pDest->m_pbData;
    UINT8 *pubSrc    = (UINT8 *) m_pSrc->m_pbData;
    UINT8 *pubSrcLim = (UINT8 *) (m_pSrc->m_pbData + m_pSrc->m_uPitch * m_pSrc->m_uHeight);

    while(pubSrc < pubSrcLim)
    {
        UINT8 *pub     = pubDest;
        UINT8 *pubA    = pubSrc;
        UINT8 *pubB    = (UINT8 *) ((BYTE *) pubA + m_pSrc->m_uPitch);
        UINT8 *pubALim = pubA + m_pSrc->m_uWidth;

        while(pubA < pubALim)
        {
            *pub++ = (UINT8) ((((((UINT32) pubA[0] & 0x0f) + ((UINT32) pubA[1] & 0x0f) + 
                                 ((UINT32) pubB[0] & 0x0f) + ((UINT32) pubB[1] & 0x0f) + 0x02) & (0x0f << 2)) |

                               ((((UINT32) pubA[0] & 0xf0) + ((UINT32) pubA[1] & 0xf0) + 
                                 ((UINT32) pubB[0] & 0xf0) + ((UINT32) pubB[1] & 0xf0) + 0x20) & (0xf0 << 2))) >> 2);

            pubA += 2;
            pubB += 2;
        }

        pubDest = (UINT8 *) ((BYTE *) pubDest + m_pDest->m_uPitch);
        pubSrc  = (UINT8 *) ((BYTE *) pubSrc + m_pSrc->m_uPitch + m_pSrc->m_uPitch);
    }
    return S_OK;
#endif
    return E_NOTIMPL;
}

