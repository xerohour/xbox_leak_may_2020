//-----------------------------------------------------------------------------
// File: CD3DXCodec.cpp
//
// Desc: Contains general purpose blitting routines
//
// Hist: 01.29.02 - Updated to avoid using D3D
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "CD3DXCodec.h"
#include "basetexture.h"

extern BOOL D3DX__Premult;

static FLOAT g_fDitherOff[] =
{
    0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f,
    0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f,
    0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f,
    0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f
};

static FLOAT g_fDitherOn[] =
{
    0.96875f, 0.46875f, 0.84375f, 0.34375f, 0.96875f, 0.46875f, 0.84375f, 0.34375f,
    0.21875f, 0.71875f, 0.09375f, 0.59375f, 0.21875f, 0.71875f, 0.09375f, 0.59375f,
    0.78125f, 0.28125f, 0.90625f, 0.40625f, 0.78125f, 0.28125f, 0.90625f, 0.40625f,
    0.03125f, 0.53125f, 0.15625f, 0.65625f, 0.03125f, 0.53125f, 0.15625f, 0.65625f
};



///////////////////////////////////////////////////////////////////////////
// CXD3DXCodec /////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////


CXD3DXCodec* CXD3DXCodec::Create(D3DX_BLT *pBlt)
{

#define CASE_FORMAT(format) \
    case D3DFMT_##format : \
        return new CXD3DXCodec_##format(pBlt)


    switch(pBlt->Format)
    {
//    CASE_FORMAT(R8G8B8);
#if 0
    CASE_FORMAT(R3G3B2);
    CASE_FORMAT(A8R3G3B2);
    CASE_FORMAT(X4R4G4B4);
    CASE_FORMAT(A8P8);
    CASE_FORMAT(A4L4);
    CASE_FORMAT(X8L8V8U8);
    CASE_FORMAT(W11V11U10);
    CASE_FORMAT(DXT3);
    CASE_FORMAT(DXT5);
#endif
    CASE_FORMAT(A8R8G8B8);
    CASE_FORMAT(X8R8G8B8);
    CASE_FORMAT(R5G6B5);
    CASE_FORMAT(X1R5G5B5);
    CASE_FORMAT(A1R5G5B5);
    CASE_FORMAT(A4R4G4B4);
    CASE_FORMAT(A8);
    CASE_FORMAT(P8);
    CASE_FORMAT(L8);
    CASE_FORMAT(A8L8);

    CASE_FORMAT(V8U8);
    CASE_FORMAT(L6V5U5);
    CASE_FORMAT(Q8W8V8U8);
    CASE_FORMAT(V16U16);

//    CASE_FORMAT(UYVY);
//    CASE_FORMAT(YUY2);
//    CASE_FORMAT(DXT1);
//    CASE_FORMAT(DXT2);
//    CASE_FORMAT(DXT4);
    }

    return NULL;
}

CXD3DXCodec::CXD3DXCodec(D3DX_BLT *pBlt, UINT uBPP, DWORD dwType)
{
    m_pbData = (BYTE *) pBlt->pData;
    m_Format = pBlt->Format;
    m_uPitch = pBlt->RowPitch;
    m_uSlice = pBlt->SlicePitch;
    m_Box = pBlt->SubRegion;
    m_ColorKey = pBlt->ColorKey;
    m_bColorKey = pBlt->ColorKey != 0;
    m_pfDither = pBlt->bDither ? g_fDitherOn : g_fDitherOff;
    m_uBytesPerPixel = uBPP >> 3;
    m_bLinear = uBPP != 0;
    m_dwType = dwType;

    if(CODEC_P == m_dwType)
    {
        m_dwType = CODEC_RGB;
        m_bPalettized = TRUE;

        if(pBlt->pPalette)
        {
            for(UINT i = 0; i < 256; i++)
            {
                m_pPalette[i].r = (FLOAT) pBlt->pPalette[i].peRed   * (1.0f / 255.0f);
                m_pPalette[i].g = (FLOAT) pBlt->pPalette[i].peGreen * (1.0f / 255.0f);
                m_pPalette[i].b = (FLOAT) pBlt->pPalette[i].peBlue  * (1.0f / 255.0f);
                m_pPalette[i].a = (FLOAT) pBlt->pPalette[i].peFlags * (1.0f / 255.0f);
            }
        }
        else
        {
            for(UINT i = 0; i < 256; i++)
            {
                m_pPalette[i].r = m_pPalette[i].g = m_pPalette[i].b = m_pPalette[i].a = 1.0f;
            }
        }
    }
    else
    {
        m_bPalettized = FALSE;
    }


    m_uWidth  = (UINT) (m_Box.Right - m_Box.Left);
    m_uHeight = (UINT) (m_Box.Bottom - m_Box.Top);
    m_uDepth  = (UINT) (m_Box.Back - m_Box.Front);
    m_uWidthBytes = m_uWidth * m_uBytesPerPixel;

    if(m_bLinear)
    {
        m_pbData += m_Box.Front * m_uSlice + m_Box.Top * m_uPitch + m_Box.Left * m_uBytesPerPixel;

        m_Box.Left   = 0;
        m_Box.Top    = 0;
        m_Box.Front  = 0;
        m_Box.Right  = m_uWidth;
        m_Box.Bottom = m_uHeight;
        m_Box.Back   = m_uDepth;
    }
}

CXD3DXCodec::~CXD3DXCodec()
{
    // Do nothing
}

void CXD3DXCodec::Decode(UINT uRow, UINT uSlice, D3DXCOLOR *pColors)
{
    // Do nothing
}

void CXD3DXCodec::Encode(UINT uRow, UINT uSlice, D3DXCOLOR *pColors)
{
    // Do nothing
}

void CXD3DXCodec::ColorKey(D3DXCOLOR *pColors)
{
    D3DXCOLOR *pColor;
    D3DXCOLOR *pColorLim = pColors + m_uWidth;

    for(pColor = pColors; pColor < pColorLim; pColor++)
    {
        if(*pColor == m_ColorKey)
            pColor->r = pColor->g = pColor->b = pColor->a = 0.0f;
    }
}


///////////////////////////////////////////////////////////////////////////
// Specific codecs ////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////


#define ENCODE(format) \
    void CXD3DXCodec_##format::Encode(UINT uRow, UINT uSlice, D3DXCOLOR *pColors)


ENCODE(R8G8B8)
{
    UINT8 *pub = (UINT8 *) (m_pbData + uRow * m_uPitch + uSlice * m_uSlice);
    FLOAT *pfDither = m_pfDither + (uSlice & 3) + ((uRow & 3) * 8);

    F2IBegin();

    for(UINT i = 0; i < m_uWidth; i++)
    {
        FLOAT fDither = pfDither[i & 3];

        pub[0] = (UINT8) F2I(pColors[i].b * 255.0f + fDither);
        pub[1] = (UINT8) F2I(pColors[i].g * 255.0f + fDither);
        pub[2] = (UINT8) F2I(pColors[i].r * 255.0f + fDither);

        pub += 3;
    }

    F2IEnd();
}


ENCODE(A8R8G8B8)
{
    UINT32 *pul = (UINT32 *) (m_pbData + uRow * m_uPitch + uSlice * m_uSlice);
    FLOAT *pfDither = m_pfDither + (uSlice & 3) + ((uRow & 3) * 8);

    F2IBegin();

    for(UINT i = 0; i < m_uWidth; i++)
    {
        FLOAT fDither = pfDither[i & 3];

        *pul = (UINT32) ((F2I(pColors[i].r * 255.0f + fDither) << 16) |
                         (F2I(pColors[i].g * 255.0f + fDither) <<  8) |
                         (F2I(pColors[i].b * 255.0f + fDither) <<  0) |
                         (F2I(pColors[i].a * 255.0f + fDither) << 24));

        pul++;
    }

    F2IEnd();
}


ENCODE(X8R8G8B8)
{
    UINT32 *pul = (UINT32 *) (m_pbData + uRow * m_uPitch + uSlice * m_uSlice);
    FLOAT *pfDither = m_pfDither + (uSlice & 3) + ((uRow & 3) * 8);

    F2IBegin();

    for(UINT i = 0; i < m_uWidth; i++)
    {
        FLOAT fDither = pfDither[i & 3];

        *pul = (UINT32) ((F2I(pColors[i].r * 255.0f + fDither) << 16) |
                         (F2I(pColors[i].g * 255.0f + fDither) <<  8) |
                         (F2I(pColors[i].b * 255.0f + fDither) <<  0));

        pul++;
    }

    F2IEnd();
}


ENCODE(R5G6B5)
{
    UINT16 *pus = (UINT16 *) (m_pbData + uRow * m_uPitch + uSlice * m_uSlice);
    FLOAT *pfDither = m_pfDither + (uSlice & 3) + ((uRow & 3) * 8);

    F2IBegin();

    for(UINT i = 0; i < m_uWidth; i++)
    {
        FLOAT fDither = pfDither[i & 3];

        *pus = (UINT16) ((F2I(pColors[i].r * 31.0f + fDither) << 11) |
                         (F2I(pColors[i].g * 63.0f + fDither) <<  5) |
                         (F2I(pColors[i].b * 31.0f + fDither) <<  0));

        pus++;
    }

    F2IEnd();
}


ENCODE(X1R5G5B5)
{
    UINT16 *pus = (UINT16 *) (m_pbData + uRow * m_uPitch + uSlice * m_uSlice);
    FLOAT *pfDither = m_pfDither + (uSlice & 3) + ((uRow & 3) * 8);

    F2IBegin();

    for(UINT i = 0; i < m_uWidth; i++)
    {
        FLOAT fDither = pfDither[i & 3];

        *pus = (UINT16) ((F2I(pColors[i].r * 31.0f + fDither) << 10) |
                         (F2I(pColors[i].g * 31.0f + fDither) <<  5) |
                         (F2I(pColors[i].b * 31.0f + fDither) <<  0));

        pus++;
    }

    F2IEnd();
}


ENCODE(A1R5G5B5)
{
    UINT16 *pus = (UINT16 *) (m_pbData + uRow * m_uPitch + uSlice * m_uSlice);
    FLOAT *pfDither = m_pfDither + (uSlice & 3) + ((uRow & 3) * 8);

    F2IBegin();

    for(UINT i = 0; i < m_uWidth; i++)
    {
        FLOAT fDither = pfDither[i & 3];

        *pus = (UINT16) ((F2I(pColors[i].r * 31.0f + fDither) << 10) |
                         (F2I(pColors[i].g * 31.0f + fDither) <<  5) |
                         (F2I(pColors[i].b * 31.0f + fDither) <<  0) |
                         (F2I(pColors[i].a *  1.0f + fDither) << 15));

        pus++;
    }

    F2IEnd();
}


ENCODE(A4R4G4B4)
{
    UINT16 *pus = (UINT16 *) (m_pbData + uRow * m_uPitch + uSlice * m_uSlice);
    FLOAT *pfDither = m_pfDither + (uSlice & 3) + ((uRow & 3) * 8);

    F2IBegin();

    for(UINT i = 0; i < m_uWidth; i++)
    {
        FLOAT fDither = pfDither[i & 3];

        *pus = (UINT16) ((F2I(pColors[i].r * 15.0f + fDither) <<  8) |
                         (F2I(pColors[i].g * 15.0f + fDither) <<  4) |
                         (F2I(pColors[i].b * 15.0f + fDither) <<  0) |
                         (F2I(pColors[i].a * 15.0f + fDither) << 12));

        pus++;
    }

    F2IEnd();
}


ENCODE(R3G3B2)
{
    UINT8 *pub = (UINT8 *) (m_pbData + uRow * m_uPitch + uSlice * m_uSlice);
    FLOAT *pfDither = m_pfDither + (uSlice & 3) + ((uRow & 3) * 8);

    F2IBegin();

    for(UINT i = 0; i < m_uWidth; i++)
    {
        FLOAT fDither = pfDither[i & 3];

        *pub = (UINT8) ((F2I(pColors[i].r * 7.0f + fDither) <<  5) |
                        (F2I(pColors[i].g * 7.0f + fDither) <<  2) |
                        (F2I(pColors[i].b * 3.0f + fDither) <<  0));

        pub++;
    }

    F2IEnd();
}


ENCODE(A8)
{
    UINT8 *pub = (UINT8 *) (m_pbData + uRow * m_uPitch + uSlice * m_uSlice);
    FLOAT *pfDither = m_pfDither + (uSlice & 3) + ((uRow & 3) * 8);

    F2IBegin();

    for(UINT i = 0; i < m_uWidth; i++)
    {
        FLOAT fDither = pfDither[i & 3];

        *pub = (UINT8) F2I(pColors[i].a * 255.0f + fDither);

        pub++;
    }

    F2IEnd();
}


ENCODE(A8R3G3B2)
{
    UINT16 *pus = (UINT16 *) (m_pbData + uRow * m_uPitch + uSlice * m_uSlice);
    FLOAT *pfDither = m_pfDither + (uSlice & 3) + ((uRow & 3) * 8);

    F2IBegin();

    for(UINT i = 0; i < m_uWidth; i++)
    {
        FLOAT fDither = pfDither[i & 3];

        *pus = (UINT16) ((F2I(pColors[i].r *   7.0f + fDither) <<  5) |
                         (F2I(pColors[i].g *   7.0f + fDither) <<  2) |
                         (F2I(pColors[i].b *   3.0f + fDither) <<  0) |
                         (F2I(pColors[i].a * 255.0f + fDither) <<  8));

        pus++;
    }

    F2IEnd();
}


ENCODE(X4R4G4B4)
{
    UINT16 *pus = (UINT16 *) (m_pbData + uRow * m_uPitch + uSlice * m_uSlice);
    FLOAT *pfDither = m_pfDither + (uSlice & 3) + ((uRow & 3) * 8);

    F2IBegin();

    for(UINT i = 0; i < m_uWidth; i++)
    {
        FLOAT fDither = pfDither[i & 3];

        *pus = (UINT16) ((F2I(pColors[i].r * 15.0f + fDither) <<  8) |
                         (F2I(pColors[i].g * 15.0f + fDither) <<  4) |
                         (F2I(pColors[i].b * 15.0f + fDither) <<  0));

        pus++;
    }

    F2IEnd();
}


ENCODE(A8P8)
{
    UINT16 *pus = (UINT16 *) (m_pbData + uRow * m_uPitch + uSlice * m_uSlice);
    FLOAT *pfDither = m_pfDither + (uSlice & 3) + ((uRow & 3) * 8);

    F2IBegin();

    for(UINT i = 0; i < m_uWidth; i++)
    {
        FLOAT fDither = pfDither[i & 3];

        UINT uMin = 0;
        FLOAT fDistMin = FLT_MAX;

        for(UINT u = 0; u < 256; u++)
        {
            D3DXCOLOR color = *pColors - m_pPalette[u];
            FLOAT fDist = color.r * color.r + color.g * color.g + color.b * color.b;

            if(fDist < fDistMin)
                uMin = u;
        }

        *pus = (UINT16) (uMin | (F2I(pColors[i].a * 255.0f + fDither) << 8));

        pus++;
    }

    F2IEnd();
}


ENCODE(P8)
{
    UINT8 *pub = (UINT8 *) (m_pbData + uRow * m_uPitch + uSlice * m_uSlice);


    for(UINT i = 0; i < m_uWidth; i++)
    {
        UINT uMin = 0;
        FLOAT fDistMin = FLT_MAX;

        for(UINT u = 0; u < 256; u++)
        {
            D3DXCOLOR color = pColors[i] - m_pPalette[u];
            FLOAT fDist = color.r * color.r + color.g * color.g + color.b * color.b + color.a * color.a;

            if(fDist < fDistMin)
            {
                uMin = u;
                fDistMin = fDist;
            }
        }

        *pub = (UINT8) uMin;

        pub++;
    }
}


ENCODE(L8)
{
    UINT8 *pub = (UINT8 *) (m_pbData + uRow * m_uPitch + uSlice * m_uSlice);
    FLOAT *pfDither = m_pfDither + (uSlice & 3) + ((uRow & 3) * 8);

    F2IBegin();

    for(UINT i = 0; i < m_uWidth; i++)
    {
        FLOAT fDither = pfDither[i & 3];

        *pub = (UINT8) F2I((pColors[i].r * 0.2125f +
                            pColors[i].g * 0.7154f +
                            pColors[i].b * 0.0721f) * 255.0f + fDither);

        pub++;
    }

    F2IEnd();
}


ENCODE(A8L8)
{
    UINT16 *pus = (UINT16 *) (m_pbData + uRow * m_uPitch + uSlice * m_uSlice);
    FLOAT *pfDither = m_pfDither + (uSlice & 3) + ((uRow & 3) * 8);

    F2IBegin();

    for(UINT i = 0; i < m_uWidth; i++)
    {
        FLOAT fDither = pfDither[i & 3];

        *pus = (UINT16) ((F2I((pColors[i].r * 0.2125f +
                               pColors[i].g * 0.7154f +
                               pColors[i].b * 0.0721f) * 255.0f + fDither) << 0) |

                         (F2I(pColors[i].a * 255.0f + fDither) << 8));

        pus++;
    }

    F2IEnd();
}


ENCODE(A4L4)
{
    UINT8 *pub = (UINT8 *) (m_pbData + uRow * m_uPitch + uSlice * m_uSlice);
    FLOAT *pfDither = m_pfDither + (uSlice & 3) + ((uRow & 3) * 8);

    F2IBegin();

    for(UINT i = 0; i < m_uWidth; i++)
    {
        FLOAT fDither = pfDither[i & 3];

        *pub = (UINT8) ((F2I((pColors[i].r * 0.2125f +
                              pColors[i].g * 0.7154f +
                              pColors[i].b * 0.0721f) * 15.0f + fDither) << 0) |

                        (F2I(pColors[i].a * 15.0f + fDither) << 4));

        pub++;
    }

    F2IEnd();
}


ENCODE(V8U8)
{
    UINT16 *pus = (UINT16 *) (m_pbData + uRow * m_uPitch + uSlice * m_uSlice);
    FLOAT *pfDither = m_pfDither + (uSlice & 3) + ((uRow & 3) * 8);

    F2IBegin();

    for(UINT i = 0; i < m_uWidth; i++)
    {
        FLOAT fDither = pfDither[i & 3];

        *pus = (UINT16) ((F2I(pColors[i].r * 128.0f + fDither) & 255) << 0) |
                        ((F2I(pColors[i].g * 128.0f + fDither) & 255) << 8);
                      
        pus++;
    }

    F2IEnd();
}


ENCODE(L6V5U5)
{
    UINT16 *pus = (UINT16 *) (m_pbData + uRow * m_uPitch + uSlice * m_uSlice);
    FLOAT *pfDither = m_pfDither + (uSlice & 3) + ((uRow & 3) * 8);

    F2IBegin();

    for(UINT i = 0; i < m_uWidth; i++)
    {
        FLOAT fDither = pfDither[i & 3];

        *pus = (UINT32) ((F2I(pColors[i].r * 16.0f + fDither) & 31) <<  0) |
                        ((F2I(pColors[i].g * 16.0f + fDither) & 31) <<  5) |
                        ((F2I(pColors[i].a * 63.0f + fDither) & 63) << 10);

        pus++;
    }

    F2IEnd();
}


ENCODE(X8L8V8U8)
{
    UINT32 *pul = (UINT32 *) (m_pbData + uRow * m_uPitch + uSlice * m_uSlice);
    FLOAT *pfDither = m_pfDither + (uSlice & 3) + ((uRow & 3) * 8);

    F2IBegin();

    for(UINT i = 0; i < m_uWidth; i++)
    {
        FLOAT fDither = pfDither[i & 3];

        *pul = (UINT32) ((F2I(pColors[i].r * 128.0f + fDither) & 255) <<  0) |
                        ((F2I(pColors[i].g * 128.0f + fDither) & 255) <<  8) |
                        ((F2I(pColors[i].a * 255.0f + fDither) & 255) << 16);

        pul++;
    }

    F2IEnd();
}


ENCODE(Q8W8V8U8)
{
    UINT32 *pul = (UINT32 *) (m_pbData + uRow * m_uPitch + uSlice * m_uSlice);
    FLOAT *pfDither = m_pfDither + (uSlice & 3) + ((uRow & 3) * 8);

    F2IBegin();

    for(UINT i = 0; i < m_uWidth; i++)
    {
        FLOAT fDither = pfDither[i & 3];

        *pul = (UINT32) ((F2I(pColors[i].r * 128.0f + fDither) & 255) <<  0) |
                        ((F2I(pColors[i].g * 128.0f + fDither) & 255) <<  8) |
                        ((F2I(pColors[i].b * 128.0f + fDither) & 255) << 16) |
                        ((F2I(pColors[i].a * 128.0f + fDither) & 255) << 24);

        pul++;
    }

    F2IEnd();
}


ENCODE(V16U16)
{
    UINT32 *pul = (UINT32 *) (m_pbData + uRow * m_uPitch + uSlice * m_uSlice);
    FLOAT *pfDither = m_pfDither + (uSlice & 3) + ((uRow & 3) * 8);

    F2IBegin();

    for(UINT i = 0; i < m_uWidth; i++)
    {
        FLOAT fDither = pfDither[i & 3];

        *pul = (UINT32) ((F2I(pColors[i].r * 32768.0f + fDither) & 65535) <<  0) |
                        ((F2I(pColors[i].g * 32768.0f + fDither) & 65535) << 16);

        pul++;
    }

    F2IEnd();
}


ENCODE(W11V11U10)
{
    UINT32 *pul = (UINT32 *) (m_pbData + uRow * m_uPitch + uSlice * m_uSlice);
    FLOAT *pfDither = m_pfDither + (uSlice & 3) + ((uRow & 3) * 8);

    F2IBegin();

    for(UINT i = 0; i < m_uWidth; i++)
    {
        FLOAT fDither = pfDither[i & 3];

        *pul = (UINT32) ((F2I(pColors[i].r *  512.0f + fDither) & 1023) <<  0) |
                        ((F2I(pColors[i].g * 1024.0f + fDither) & 2047) << 10) |
                        ((F2I(pColors[i].b * 1024.0f + fDither) & 2046) << 21);

        pul++;
    }

    F2IEnd();
}




#define DECODE(format) \
    void CXD3DXCodec_##format::Decode(UINT uRow, UINT uSlice, D3DXCOLOR *pColors)


DECODE(R8G8B8)
{
    UINT8 *pub = (UINT8 *) (m_pbData + uRow * m_uPitch + uSlice * m_uSlice);
    UINT8 *pubLim = pub + m_uWidthBytes;

    while(pub < pubLim)
    {
        pColors->r = (FLOAT) pub[2] * (1.0f / 255.0f);
        pColors->g = (FLOAT) pub[1] * (1.0f / 255.0f);
        pColors->b = (FLOAT) pub[0] * (1.0f / 255.0f);
        pColors->a = 1.0f;

        pub += 3;
        pColors++;
    }

    if(m_bColorKey)
        ColorKey(pColors - m_uWidth);
}


DECODE(A8R8G8B8)
{
    UINT32 *pul = (UINT32 *) (m_pbData + uRow * m_uPitch + uSlice * m_uSlice);
    UINT32 *pulLim = pul + m_uWidth;

    while(pul < pulLim)
    {
        pColors->r = (FLOAT) ((*pul >> 16) & 255) * (1.0f / 255.0f);
        pColors->g = (FLOAT) ((*pul >>  8) & 255) * (1.0f / 255.0f);
        pColors->b = (FLOAT) ((*pul >>  0) & 255) * (1.0f / 255.0f);
        pColors->a = (FLOAT) ((*pul >> 24) & 255) * (1.0f / 255.0f);

        pul++;
        pColors++;
    }

    if(m_bColorKey)
        ColorKey(pColors - m_uWidth);
}


DECODE(X8R8G8B8)
{
    UINT32 *pul = (UINT32 *) (m_pbData + uRow * m_uPitch + uSlice * m_uSlice);
    UINT32 *pulLim = pul + m_uWidth;

    while(pul < pulLim)
    {
        pColors->r = (FLOAT) ((*pul >> 16) & 255) * (1.0f / 255.0f);
        pColors->g = (FLOAT) ((*pul >>  8) & 255) * (1.0f / 255.0f);
        pColors->b = (FLOAT) ((*pul >>  0) & 255) * (1.0f / 255.0f);
        pColors->a = 1.0f;

        pul++;
        pColors++;
    }

    if(m_bColorKey)
        ColorKey(pColors - m_uWidth);
}


DECODE(R5G6B5)
{
    UINT16 *pus = (UINT16 *) (m_pbData + uRow * m_uPitch + uSlice * m_uSlice);
    UINT16 *pusLim = pus + m_uWidth;

    while(pus < pusLim)
    {
        pColors->r = (FLOAT) ((*pus >> 11) & 31) * (1.0f / 31.0f);
        pColors->g = (FLOAT) ((*pus >>  5) & 63) * (1.0f / 63.0f);
        pColors->b = (FLOAT) ((*pus >>  0) & 31) * (1.0f / 31.0f);
        pColors->a = 1.0f;

        pus++;
        pColors++;
    }

    if(m_bColorKey)
        ColorKey(pColors - m_uWidth);
}


DECODE(X1R5G5B5)
{
    UINT16 *pus = (UINT16 *) (m_pbData + uRow * m_uPitch + uSlice * m_uSlice);
    UINT16 *pusLim = pus + m_uWidth;

    while(pus < pusLim)
    {
        pColors->r = (FLOAT) ((*pus >> 10) & 31) * (1.0f / 31.0f);
        pColors->g = (FLOAT) ((*pus >>  5) & 31) * (1.0f / 31.0f);
        pColors->b = (FLOAT) ((*pus >>  0) & 31) * (1.0f / 31.0f);
        pColors->a = 1.0f;

        pus++;
        pColors++;
    }

    if(m_bColorKey)
        ColorKey(pColors - m_uWidth);
}


DECODE(A1R5G5B5)
{
    UINT16 *pus = (UINT16 *) (m_pbData + uRow * m_uPitch + uSlice * m_uSlice);
    UINT16 *pusLim = pus + m_uWidth;

    while(pus < pusLim)
    {
        pColors->r = (FLOAT) ((*pus >> 10) & 31) * (1.0f / 31.0f);
        pColors->g = (FLOAT) ((*pus >>  5) & 31) * (1.0f / 31.0f);
        pColors->b = (FLOAT) ((*pus >>  0) & 31) * (1.0f / 31.0f);
        pColors->a = (FLOAT) ((*pus >> 15) &  1) * (1.0f /  1.0f);

        pus++;
        pColors++;
    }

    if(m_bColorKey)
        ColorKey(pColors - m_uWidth);
}


DECODE(A4R4G4B4)
{
    UINT16 *pus = (UINT16 *) (m_pbData + uRow * m_uPitch + uSlice * m_uSlice);
    UINT16 *pusLim = pus + m_uWidth;

    while(pus < pusLim)
    {
        pColors->r = (FLOAT) ((*pus >>  8) & 15) * (1.0f / 15.0f);
        pColors->g = (FLOAT) ((*pus >>  4) & 15) * (1.0f / 15.0f);
        pColors->b = (FLOAT) ((*pus >>  0) & 15) * (1.0f / 15.0f);
        pColors->a = (FLOAT) ((*pus >> 12) & 15) * (1.0f / 15.0f);

        pus++;
        pColors++;
    }

    if(m_bColorKey)
        ColorKey(pColors - m_uWidth);
}


DECODE(R3G3B2)
{
    UINT8 *pub = (UINT8 *) (m_pbData + uRow * m_uPitch + uSlice * m_uSlice);
    UINT8 *pubLim = pub + m_uWidth;

    while(pub < pubLim)
    {
        pColors->r = (FLOAT) ((*pub >>  5) & 7) * (1.0f / 7.0f);
        pColors->g = (FLOAT) ((*pub >>  2) & 7) * (1.0f / 7.0f);
        pColors->b = (FLOAT) ((*pub >>  0) & 3) * (1.0f / 3.0f);
        pColors->a = 1.0f;

        pub++;
        pColors++;
    }

    if(m_bColorKey)
        ColorKey(pColors - m_uWidth);
}


DECODE(A8)
{
    UINT8 *pub = (UINT8 *) (m_pbData + uRow * m_uPitch + uSlice * m_uSlice);
    UINT8 *pubLim = pub + m_uWidth;

    while(pub < pubLim)
    {
        pColors->r = 1.0f;
        pColors->g = 1.0f;
        pColors->b = 1.0f;
        pColors->a = (FLOAT) *pub * (1.0f / 255.0f);

        pub++;
        pColors++;
    }

    if(m_bColorKey)
        ColorKey(pColors - m_uWidth);
}


DECODE(A8R3G3B2)
{
    UINT16 *pus = (UINT16 *) (m_pbData + uRow * m_uPitch + uSlice * m_uSlice);
    UINT16 *pusLim = pus + m_uWidth;

    while(pus < pusLim)
    {
        pColors->r = (FLOAT) ((*pus >>  5) &   7) * (1.0f /   7.0f);
        pColors->g = (FLOAT) ((*pus >>  2) &   7) * (1.0f /   7.0f);
        pColors->b = (FLOAT) ((*pus >>  0) &   3) * (1.0f /   3.0f);
        pColors->a = (FLOAT) ((*pus >>  8) & 255) * (1.0f / 255.0f);

        pus++;
        pColors++;
    }

    if(m_bColorKey)
        ColorKey(pColors - m_uWidth);
}


DECODE(X4R4G4B4)
{
    UINT16 *pus = (UINT16 *) (m_pbData + uRow * m_uPitch + uSlice * m_uSlice);
    UINT16 *pusLim = pus + m_uWidth;

    while(pus < pusLim)
    {
        pColors->r = (FLOAT) ((*pus >>  8) & 15) * (1.0f / 15.0f);
        pColors->g = (FLOAT) ((*pus >>  4) & 15) * (1.0f / 15.0f);
        pColors->b = (FLOAT) ((*pus >>  0) & 15) * (1.0f / 15.0f);
        pColors->a = 1.0f;

        pus++;
        pColors++;
    }

    if(m_bColorKey)
        ColorKey(pColors - m_uWidth);
}


DECODE(A8P8)
{
    UINT16 *pus = (UINT16 *) (m_pbData + uRow * m_uPitch + uSlice * m_uSlice);
    UINT16 *pusLim = pus + m_uWidth;

    while(pus < pusLim)
    {
        *pColors = m_pPalette[*pus & 255];
        pColors->a = (FLOAT) ((*pus >>  8) & 255) * (1.0f / 255.0f);

        pus++;
        pColors++;
    }

    if(m_bColorKey)
        ColorKey(pColors - m_uWidth);
}


DECODE(P8)
{
    UINT8 *pub = (UINT8 *) (m_pbData + uRow * m_uPitch + uSlice * m_uSlice);
    UINT8 *pubLim = pub + m_uWidth;

    while(pub < pubLim)
    {
        *pColors = m_pPalette[*pub];

        pub++;
        pColors++;
    }

    if(m_bColorKey)
        ColorKey(pColors - m_uWidth);
}


DECODE(L8)
{
    UINT8 *pub = (UINT8 *) (m_pbData + uRow * m_uPitch + uSlice * m_uSlice);
    UINT8 *pubLim = pub + m_uWidth;

    while(pub < pubLim)
    {
        pColors->r = pColors->g = pColors->b = (FLOAT) *pub * (1.0f / 255.0f);
        pColors->a = 1.0f;

        pub++;
        pColors++;
    }

    if(m_bColorKey)
        ColorKey(pColors - m_uWidth);
}


DECODE(A8L8)
{
    UINT16 *pus = (UINT16 *) (m_pbData + uRow * m_uPitch + uSlice * m_uSlice);
    UINT16 *pusLim = pus + m_uWidth;

    while(pus < pusLim)
    {
        pColors->r = pColors->g = pColors->b = (FLOAT) ((*pus >>  0) & 255) * (1.0f / 255.0f);
        pColors->a = (FLOAT) ((*pus >> 8) & 255) * (1.0f / 255.0f);

        pus++;
        pColors++;
    }

    if(m_bColorKey)
        ColorKey(pColors - m_uWidth);
}


DECODE(A4L4)
{
    UINT8 *pub = (UINT8 *) (m_pbData + uRow * m_uPitch + uSlice * m_uSlice);
    UINT8 *pubLim = pub + m_uWidth;

    while(pub < pubLim)
    {
        pColors->r = pColors->g = pColors->b = (FLOAT) ((*pub >>  0) & 15) * (1.0f / 15.0f);
        pColors->a = (FLOAT) ((*pub >> 4) & 15) * (1.0f / 15.0f);

        pub++;
        pColors++;
    }

    if(m_bColorKey)
        ColorKey(pColors - m_uWidth);
}

DECODE(V8U8)
{
    UINT16 *pus = (UINT16 *) (m_pbData + uRow * m_uPitch + uSlice * m_uSlice);
    UINT16 *pusLim = pus + m_uWidth;

    while(pus < pusLim)
    {
        pColors->r = (FLOAT) (INT8) ((*pus >> 0) & 255) * (1.0f / 128.0f);
        pColors->g = (FLOAT) (INT8) ((*pus >> 8) & 255) * (1.0f / 128.0f);
        pColors->b = 0.0f;
        pColors->a = 1.0f;

        pus++;
        pColors++;
    }
}


DECODE(L6V5U5)
{
    UINT16 *pus = (UINT16 *) (m_pbData + uRow * m_uPitch + uSlice * m_uSlice);
    UINT16 *pusLim = pus + m_uWidth;

    while(pus < pusLim)
    {
        INT8 iU = (INT8) ((*pus >>  0) & 31);
        INT8 iV = (INT8) ((*pus >>  5) & 31);

        // Sign extension
        iU <<= 3; iU >>= 3;
        iV <<= 3; iV >>= 3; 

        pColors->r = (FLOAT) iU * (1.0f / 16.0f);
        pColors->g = (FLOAT) iV * (1.0f / 16.0f);
        pColors->b = 0.0f;
        pColors->a = (FLOAT) ((*pus >> 10) & 63) * (1.0f / 63.0f);

        pus++;
        pColors++;
    }
}


DECODE(X8L8V8U8)
{
    UINT32 *pul = (UINT32 *) (m_pbData + uRow * m_uPitch + uSlice * m_uSlice);
    UINT32 *pulLim = pul + m_uWidth;

    while(pul < pulLim)
    {
        pColors->r = (FLOAT) (INT8) ((*pul >>  0) & 255) * (1.0f / 128.0f);
        pColors->g = (FLOAT) (INT8) ((*pul >>  8) & 255) * (1.0f / 128.0f);
        pColors->b = 0.0f;
        pColors->a = (FLOAT) ((*pul >> 16) & 255) * (1.0f / 255.0f);

        pul++;
        pColors++;
    }
}


DECODE(Q8W8V8U8)
{
    UINT32 *pul = (UINT32 *) (m_pbData + uRow * m_uPitch + uSlice * m_uSlice);
    UINT32 *pulLim = pul + m_uWidth;

    while(pul < pulLim)
    {
        pColors->r = (FLOAT) (INT8) ((*pul >>  0) & 255) * (1.0f / 128.0f);
        pColors->g = (FLOAT) (INT8) ((*pul >>  8) & 255) * (1.0f / 128.0f);
        pColors->b = (FLOAT) (INT8) ((*pul >> 16) & 255) * (1.0f / 128.0f);
        pColors->a = (FLOAT) (INT8) ((*pul >> 24) & 255) * (1.0f / 128.0f);

        pul++;
        pColors++;
    }
}


DECODE(V16U16)
{
    UINT32 *pul = (UINT32 *) (m_pbData + uRow * m_uPitch + uSlice * m_uSlice);
    UINT32 *pulLim = pul + m_uWidth;

    while(pul < pulLim)
    {
        pColors->r = (FLOAT) (INT16) ((*pul >>  0) & 65535) * (1.0f / 32768.0f);
        pColors->g = (FLOAT) (INT16) ((*pul >> 16) & 65535) * (1.0f / 32768.0f);
        pColors->b = 0.0f;
        pColors->a = 1.0f;

        pul++;
        pColors++;
    }
}


DECODE(W11V11U10)
{
    UINT32 *pul = (UINT32 *) (m_pbData + uRow * m_uPitch + uSlice * m_uSlice);
    UINT32 *pulLim = pul + m_uWidth;

    while(pul < pulLim)
    {
        INT16 iU = (INT16) ((*pul >>  0) & 1023);
        INT16 iV = (INT16) ((*pul >> 10) & 2047);
        INT16 iW = (INT16) ((*pul >> 21) & 2047);

        // Sign extension
        iU <<= 6; iU >>= 6;
        iV <<= 5; iV >>= 5;
        iW <<= 5; iW >>= 5;

        pColors->r = (FLOAT) iU * (1.0f /  512.0f);
        pColors->g = (FLOAT) iV * (1.0f / 1024.0f);
        pColors->b = (FLOAT) iW * (1.0f / 1024.0f);
        pColors->a = 1.0f;

        pul++;
        pColors++;
    }
}





///////////////////////////////////////////////////////////////////////////
// CXD3DXCodecYUV //////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

CXD3DXCodecYUV::CXD3DXCodecYUV(D3DX_BLT *pBlt)
    : CXD3DXCodec(pBlt, 0, CODEC_RGB)
{
    m_CacheBox.Left   = m_Box.Left & ~1;
    m_CacheBox.Top    = 0;
    m_CacheBox.Front  = 0;
    m_CacheBox.Right  = (m_Box.Right + 1) & ~1;
    m_CacheBox.Bottom = 0;
    m_CacheBox.Back   = 0;

    m_uCacheWidth = m_CacheBox.Right - m_CacheBox.Left;

    m_bCacheDirty     = FALSE;
    m_bCacheAllocated = TRUE;

    if(!(m_pCache = new D3DXCOLOR[m_uCacheWidth]))
        m_bCacheAllocated = FALSE;

    if(D3DFMT_UYVY == pBlt->Format)
    {
        m_uYShift  = 8;
        m_uUVShift = 0;
    }
    else
    {
        m_uYShift  = 0;
        m_uUVShift = 8;
    }
}


CXD3DXCodecYUV::~CXD3DXCodecYUV()
{
    Commit();

    if(m_pCache)
        delete [] m_pCache;
}


HRESULT CXD3DXCodecYUV::Commit()
{
    if(!m_bCacheDirty || !m_bCacheAllocated)
        return S_OK;


    // Write data
    UINT16 *pus = (UINT16 *) (m_pbData + m_CacheBox.Left * 2 + m_CacheBox.Top * m_uPitch + m_CacheBox.Front * m_uSlice);
    D3DXCOLOR *pCache = m_pCache;

    for(UINT uLeft = m_CacheBox.Left; uLeft < m_CacheBox.Right; uLeft += 2)
    {
        FLOAT fY0 =  65.481f * pCache[0].r + 128.553f * pCache[0].g +  24.966f * pCache[0].b;
        FLOAT fY1 =  65.481f * pCache[1].r + 128.553f * pCache[1].g +  24.966f * pCache[1].b;

        FLOAT fU  = -37.797f * pCache[0].r + -74.203f * pCache[0].g + 112.000f * pCache[0].b;
        FLOAT fV  = 112.000f * pCache[0].r + -93.786f * pCache[0].g + -18.214f * pCache[0].b;

        int nY0 = F2I(fY0 + 0.5f) + 16;
        int nY1 = F2I(fY1 + 0.5f) + 16;
        int nU  = F2I(fU + 0.5f) + 128;
        int nV  = F2I(fV + 0.5f) + 128;

        nY0 = (nY0 < 0) ? 0 : ((nY0 > 0xff) ? 0xff : nY0);
        nY1 = (nY1 < 0) ? 0 : ((nY1 > 0xff) ? 0xff : nY1);
        nU  = (nU  < 0) ? 0 : ((nU  > 0xff) ? 0xff : nU );
        nV  = (nV  < 0) ? 0 : ((nV  > 0xff) ? 0xff : nV );

        pus[0] = (UINT16) ((nY0 << m_uYShift) | (nU << m_uUVShift));
        pus[1] = (UINT16) ((nY1 << m_uYShift) | (nV << m_uUVShift));

        pCache += 2;
        pus += 2;
    }

    m_bCacheDirty = FALSE;
    return S_OK;
}


HRESULT CXD3DXCodecYUV::Fetch(UINT uRow, UINT uSlice, BOOL bRead)
{
    HRESULT hr;

    if(!m_bCacheAllocated)
        return E_OUTOFMEMORY;

    if(uRow   >= m_CacheBox.Top   && uRow   < m_CacheBox.Bottom &&
       uSlice >= m_CacheBox.Front && uSlice < m_CacheBox.Back)
    {
        return S_OK;
    }

    // Flush dirty data in cache, if any
    if(FAILED(hr = Commit()))
        return hr;

    // Update cache rect
    m_CacheBox.Top    = uRow;
    m_CacheBox.Bottom = uRow + 1;
    m_CacheBox.Front  = uSlice;
    m_CacheBox.Back   = uSlice + 1;

    if(!bRead)
        return S_OK;


    // Read data
    UINT16 *pus = (UINT16 *) (m_pbData + m_CacheBox.Left * 2 + m_CacheBox.Top * m_uPitch + m_CacheBox.Front * m_uSlice);
    D3DXCOLOR *pCache = m_pCache;

    for(UINT uLeft = m_CacheBox.Left; uLeft < m_CacheBox.Right; uLeft += 2)
    {
        FLOAT fY0 = (FLOAT) ((pus[0] >> m_uYShift)  & 0xff) -  16.0f;
        FLOAT fU  = (FLOAT) ((pus[0] >> m_uUVShift) & 0xff) - 128.0f;

        FLOAT fY1 = (FLOAT) ((pus[1] >> m_uYShift)  & 0xff) -  16.0f;
        FLOAT fV  = (FLOAT) ((pus[1] >> m_uUVShift) & 0xff) - 128.0f;

        pCache[0].r = 0.00456621f * fY0                    + 0.00625893f * fV;
        pCache[0].g = 0.00456621f * fY0 - 0.00153632f * fU - 0.00318811f * fV;
        pCache[0].b = 0.00456621f * fY0 + 0.00791071f * fU;
        pCache[0].a = 1.0f;

        pCache[0].r = (pCache[0].r < 0.0f) ? 0.0f : ((pCache[0].r > 1.0f) ? 1.0f : pCache[0].r);
        pCache[0].g = (pCache[0].g < 0.0f) ? 0.0f : ((pCache[0].g > 1.0f) ? 1.0f : pCache[0].g);
        pCache[0].b = (pCache[0].b < 0.0f) ? 0.0f : ((pCache[0].b > 1.0f) ? 1.0f : pCache[0].b);

        pCache[1].r = 0.00456621f * fY1                    + 0.00625893f * fV;
        pCache[1].g = 0.00456621f * fY1 - 0.00153632f * fU - 0.00318811f * fV;
        pCache[1].b = 0.00456621f * fY1 + 0.00791071f * fU;
        pCache[1].a = 1.0f;

        pCache[1].r = (pCache[1].r < 0.0f) ? 0.0f : ((pCache[1].r > 1.0f) ? 1.0f : pCache[1].r);
        pCache[1].g = (pCache[1].g < 0.0f) ? 0.0f : ((pCache[1].g > 1.0f) ? 1.0f : pCache[1].g);
        pCache[1].b = (pCache[1].b < 0.0f) ? 0.0f : ((pCache[1].b > 1.0f) ? 1.0f : pCache[1].b);

        pCache += 2;
        pus += 2;
    }

    return S_OK;
}


void CXD3DXCodecYUV::Encode(UINT uRow, UINT uSlice, D3DXCOLOR *pColors)
{
    uRow += m_Box.Top;
    uSlice += m_Box.Front;

    if(FAILED(Fetch(uRow, uSlice, m_uCacheWidth != m_uWidth)))
        return;

    memcpy(m_pCache + m_Box.Left - m_CacheBox.Left, pColors, m_uWidth * sizeof(D3DXCOLOR));
    m_bCacheDirty = TRUE;
}


void CXD3DXCodecYUV::Decode(UINT uRow, UINT uSlice, D3DXCOLOR *pColors)
{
    uRow += m_Box.Top;
    uSlice += m_Box.Front;

    if(FAILED(Fetch(uRow, uSlice, TRUE)))
        return;

    memcpy(pColors, m_pCache + m_Box.Left - m_CacheBox.Left, m_uWidth * sizeof(D3DXCOLOR));

    if(m_bColorKey)
        ColorKey(pColors);
}



#ifdef SUPPORT_DXT
///////////////////////////////////////////////////////////////////////////
// CXD3DXCodecDXT //////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////


CXD3DXCodecDXT::CXD3DXCodecDXT(D3DX_BLT *pBlt)
    : CXD3DXCodec(pBlt, 0, CODEC_RGB)
{
    m_bRepeat = FALSE;

    switch(pBlt->Region.Right)
    {
    case 1:
        m_uXRepeat = 0;
        m_bRepeat = TRUE;
        break;

    case 2:
        m_uXRepeat = 1;
        m_bRepeat = TRUE;
        break;

    default:
        m_uXRepeat = 3;
        break;
    }


    switch(pBlt->Region.Bottom)
    {
    case 1:
        m_uYRepeat = 0;
        m_bRepeat = TRUE;
        break;

    case 2:
        m_uYRepeat = 1;
        m_bRepeat = TRUE;
        break;

    default:
        m_uYRepeat = 3;
        break;
    }


    m_CacheBox.Left   = m_Box.Left & ~3;
    m_CacheBox.Top    = 0;
    m_CacheBox.Front  = 0;
    m_CacheBox.Right  = (m_Box.Right + 3) & ~3;
    m_CacheBox.Bottom = 0;
    m_CacheBox.Back   = 0;

    m_uTopMax     = 0;
    m_uCacheWidth = m_CacheBox.Right - m_CacheBox.Left;

    m_bCacheDirty     = FALSE;
    m_bCacheAllocated = TRUE;

    for(UINT i = 0; i < 4; i++)
    {
        if(!(m_ppCache[i] = new D3DXCOLOR[m_uCacheWidth]))
            m_bCacheAllocated = FALSE;
    }
}


CXD3DXCodecDXT::~CXD3DXCodecDXT()
{
    Commit();

    for(UINT i = 0; i < 4; i++)
    {
        if(m_ppCache[i])
            delete [] m_ppCache[i];
    }
}


HRESULT CXD3DXCodecDXT::Commit()
{
    if(!m_bCacheDirty || !m_bCacheAllocated)
        return S_OK;

    BOOL bPremult = D3DX__Premult;

    for(UINT uLeft = m_CacheBox.Left; uLeft < (UINT) m_CacheBox.Right; uLeft += 4)
    {
        UINT32 ulBlock[16];
        LPBYTE pCell;

        // Copy cache data into block
        F2IBegin();
        UINT32 *pul = &ulBlock[0];

        if(bPremult)
        {
            for(UINT uY = 0; uY < 4; uY++)
            {
                D3DXCOLOR *pColor = m_ppCache[uY] + uLeft - m_CacheBox.Left;

                for(UINT uX = 0; uX < 4; uX++)
                {
                    *pul = (UINT32) ((F2I(pColor->r * pColor->a * 255.0f + 0.5f) << 16) |
                                     (F2I(pColor->g * pColor->a * 255.0f + 0.5f) <<  8) |
                                     (F2I(pColor->b * pColor->a * 255.0f + 0.5f) <<  0) |
                                     (F2I(pColor->a             * 255.0f + 0.5f) << 24));

                    pColor++;
                    pul++;
                }
            }
        }
        else
        {
            for(UINT uY = 0; uY < 4; uY++)
            {
                D3DXCOLOR *pColor = m_ppCache[uY] + uLeft - m_CacheBox.Left;

                for(UINT uX = 0; uX < 4; uX++)
                {
                    *pul = (UINT32) ((F2I(pColor->r * 255.0f + 0.5f) << 16) |
                                     (F2I(pColor->g * 255.0f + 0.5f) <<  8) |
                                     (F2I(pColor->b * 255.0f + 0.5f) <<  0) |
                                     (F2I(pColor->a * 255.0f + 0.5f) << 24));


                    pColor++;
                    pul++;
                }
            }
        }

        F2IEnd();


        // Repeat data for degenerate case
        if(m_bRepeat)
        {
            for(UINT uY = 0; uY < 4; uY++)
            {
                for(UINT uX = 0; uX < 4; uX++)
                {
                    ulBlock[(uY << 2) + uX] = ulBlock[((uY & m_uYRepeat) << 2) + (uX & m_uXRepeat)];
                }
            }
        }


        // Encode DXT block
        switch (m_Format)
        {
        case D3DFMT_DXT1:
            pCell = m_pbData + m_CacheBox.Front * m_uSlice + (m_CacheBox.Top >> 2) * m_uPitch + (uLeft >> 2) * sizeof(S3TCBlockRGB);
            XXEncodeBlockRGB((S3TC_COLOR *) &ulBlock[0], (S3TCBlockRGB *) pCell);
            break;

        case D3DFMT_DXT2:
#if 0
        case D3DFMT_DXT3:
#endif
            pCell = m_pbData + m_CacheBox.Front * m_uSlice + (m_CacheBox.Top >> 2) * m_uPitch + (uLeft >> 2) * sizeof(S3TCBlockAlpha4);
            EncodeBlockAlpha4((S3TC_COLOR *) &ulBlock[0], (S3TCBlockAlpha4 *) pCell);
            break;

        case D3DFMT_DXT4:
#if 0
        case D3DFMT_DXT5:
#endif
            pCell = m_pbData + m_CacheBox.Front * m_uSlice + (m_CacheBox.Top >> 2) * m_uPitch + (uLeft >> 2) * sizeof(S3TCBlockAlpha3);
            EncodeBlockAlpha3((S3TC_COLOR *) &ulBlock[0], (S3TCBlockAlpha3 *) pCell);
            pCell = pCell;
            break;
        }
    }

    m_bCacheDirty = FALSE;
    return S_OK;
}


HRESULT CXD3DXCodecDXT::Fetch(UINT uRow, UINT uSlice, BOOL bRead)
{
    HRESULT hr;

    if(!m_bCacheAllocated)
        return E_OUTOFMEMORY;

    if(uRow   >= m_CacheBox.Top   && uRow   < m_CacheBox.Bottom &&
       uSlice >= m_CacheBox.Front && uSlice < m_CacheBox.Back)
    {
        return S_OK;
    }

    // Flush dirty data in cache, if any
    if(FAILED(hr = Commit()))
        return hr;



    // Update cache rect
    m_CacheBox.Top    = uRow & ~3;
    m_CacheBox.Bottom = m_CacheBox.Top + 4;
    m_CacheBox.Front  = uSlice;
    m_CacheBox.Back   = uSlice + 1;

    if(m_uTopMax <= m_CacheBox.Top)
    {
        m_uTopMax = m_CacheBox.Top;

        if(!bRead)
            return S_OK;
    }



    // Read data
    BOOL bPremult = D3DX__Premult;

    for(UINT uLeft = m_CacheBox.Left; uLeft < (UINT) m_CacheBox.Right; uLeft += 4)
    {
        UINT32 ulBlock[16];
        LPBYTE pCell;

        // Decode DXT block
        switch (m_Format)
        {
        case D3DFMT_DXT1:
            pCell = m_pbData + uSlice * m_uSlice + (uRow >> 2) * m_uPitch + (uLeft >> 2) * sizeof(S3TCBlockRGB);
            XXDecodeBlockRGB((S3TCBlockRGB *) pCell, (S3TC_COLOR *) &ulBlock[0]);
            break;

        case D3DFMT_DXT2:
#if 0
        case D3DFMT_DXT3:
#endif
            pCell = m_pbData + uSlice * m_uSlice + (uRow >> 2) * m_uPitch + (uLeft >> 2) * sizeof(S3TCBlockAlpha4);
            DecodeBlockAlpha4((S3TCBlockAlpha4 *) pCell, (S3TC_COLOR *) &ulBlock[0]);
            break;

        case D3DFMT_DXT4:
#if 0
        case D3DFMT_DXT5:
#endif
            pCell = m_pbData + uSlice * m_uSlice + (uRow >> 2) * m_uPitch + (uLeft >> 2) * sizeof(S3TCBlockAlpha3);
            DecodeBlockAlpha3((S3TCBlockAlpha3 *) pCell, (S3TC_COLOR *) &ulBlock[0]);
            break;
        }


        // Copy block data into cache
        UINT32 *pul = &ulBlock[0];

        for(UINT uY = 0; uY < 4; uY++)
        {
            D3DXCOLOR *pColor = m_ppCache[uY] + uLeft - m_CacheBox.Left;

            for(UINT uX = 0; uX < 4; uX++)
            {
                pColor->a = (FLOAT) ((*pul >> 24) & 255) * (1.0f / 255.0f);
                pColor->r = (FLOAT) ((*pul >> 16) & 255) * (1.0f / 255.0f);
                pColor->g = (FLOAT) ((*pul >>  8) & 255) * (1.0f / 255.0f);
                pColor->b = (FLOAT) ((*pul >>  0) & 255) * (1.0f / 255.0f);

                if(bPremult)
                {
                    if(0.0f == pColor->a)
                    {
                        pColor->r = 0.0f;
                        pColor->g = 0.0f;
                        pColor->b = 0.0f;
                    }
                    else if(1.0f > pColor->a)
                    {
                        pColor->r = (pColor->r < pColor->a) ? (pColor->r / pColor->a) : 1.0f;
                        pColor->g = (pColor->g < pColor->a) ? (pColor->g / pColor->a) : 1.0f;
                        pColor->b = (pColor->b < pColor->a) ? (pColor->b / pColor->a) : 1.0f;
                    }
                }

                pColor++;
                pul++;
            }
        }
    }

    return S_OK;
}


void CXD3DXCodecDXT::Encode(UINT uRow, UINT uSlice, D3DXCOLOR *pColors)
{
    uRow += m_Box.Top;
    uSlice += m_Box.Front;

    if(FAILED(Fetch(uRow, uSlice, m_uCacheWidth != m_uWidth)))
        return;

    memcpy(m_ppCache[uRow - m_CacheBox.Top] + m_Box.Left - m_CacheBox.Left, pColors, m_uWidth * sizeof(D3DXCOLOR));
    m_bCacheDirty = TRUE;
}


void CXD3DXCodecDXT::Decode(UINT uRow, UINT uSlice, D3DXCOLOR *pColors)
{
    uRow += m_Box.Top;
    uSlice += m_Box.Front;

    if(FAILED(Fetch(uRow, uSlice, TRUE)))
        return;

    memcpy(pColors, m_ppCache[uRow - m_CacheBox.Top] + m_Box.Left - m_CacheBox.Left, m_uWidth * sizeof(D3DXCOLOR));

    if(m_bColorKey)
        ColorKey(pColors);
}
#endif // SUPPORT_DXT