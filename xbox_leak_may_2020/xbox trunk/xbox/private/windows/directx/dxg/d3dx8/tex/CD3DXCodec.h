///////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 1999 Microsoft Corporation.  All Rights Reserved.
//
//  File:       CD3DXCodec.h
//  Content:    Pixel format codecs
//
///////////////////////////////////////////////////////////////////////////

#ifndef __CD3DXCodec_H__
#define __CD3DXCodec_H__


#define CODEC_RGB 0x01
#define CODEC_P   0x02
#define CODEC_UV  0x03
#define CODEC_ZS  0x04

struct D3DX_BLT
{
    VOID*               pData;
    D3DFORMAT           Format;

    UINT                RowPitch;
    UINT                SlicePitch;

    D3DBOX              Region;
    D3DBOX              SubRegion;

    BOOL                bDither;

    D3DCOLOR            ColorKey;
    CONST PALETTEENTRY* pPalette;
};


///////////////////////////////////////////////////////////////////////////
// CD3DXCodec /////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

class CD3DXCodec
{
public:
    D3DFORMAT  m_Format;
    DWORD      m_dwType;
    BOOL       m_bLinear;
    BOOL       m_bColorKey;
    BOOL       m_bPalettized;

    BYTE*      m_pbData;
    D3DXCOLOR  m_ColorKey;
    FLOAT*     m_pfDither;
    D3DXCOLOR  m_pPalette[256];
    D3DBOX     m_Box;

    UINT       m_uPitch;
    UINT       m_uSlice;
    UINT       m_uWidth;
    UINT       m_uHeight;
    UINT       m_uDepth;
    UINT       m_uWidthBytes;
    UINT       m_uBytesPerPixel;

public:
    static CD3DXCodec* Create(D3DX_BLT *pBlt);

    virtual ~CD3DXCodec();

    virtual void Decode(UINT uRow, UINT uSlice, D3DXCOLOR *pColors);
    virtual void Encode(UINT uRow, UINT uSlice, D3DXCOLOR *pColors);

protected:
    static D3DCOLOR s_pPalette[256];

    void ColorKey(D3DXCOLOR *pColors);

    CD3DXCodec(D3DX_BLT *pBlt, UINT uBPP, DWORD dwType);
};


///////////////////////////////////////////////////////////////////////////
// CD3DXCodecYUV //////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

class CD3DXCodecYUV : public CD3DXCodec
{
public: 
    CD3DXCodecYUV(D3DX_BLT *pBlt);
    virtual ~CD3DXCodecYUV();

    HRESULT Commit();
    HRESULT Fetch(UINT uRow, UINT uSlice, BOOL bRead);

    virtual void Encode(UINT uRow, UINT uSlice, D3DXCOLOR *pColors);
    virtual void Decode(UINT uRow, UINT uSlice, D3DXCOLOR *pColors);

protected:
    D3DXCOLOR *m_pCache;
    D3DBOX     m_CacheBox;
    UINT       m_uCacheWidth;
    BOOL       m_bCacheDirty;
    BOOL       m_bCacheAllocated;
    UINT       m_uYShift;
    UINT       m_uUVShift;
};



///////////////////////////////////////////////////////////////////////////
// CD3DXCodecDXT //////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

class CD3DXCodecDXT : public CD3DXCodec
{
public: 
    CD3DXCodecDXT(D3DX_BLT *pBlt);
    virtual ~CD3DXCodecDXT();

    HRESULT Commit();
    HRESULT Fetch(UINT uRow, UINT uSlice, BOOL bRead);

    virtual void Encode(UINT uRow, UINT uSlice, D3DXCOLOR *pColors);
    virtual void Decode(UINT uRow, UINT uSlice, D3DXCOLOR *pColors);

protected:
    D3DXCOLOR *m_ppCache[4];
    D3DBOX     m_CacheBox;
    UINT       m_uCacheWidth;
    BOOL       m_bCacheDirty;
    BOOL       m_bCacheAllocated;

    BOOL       m_bRepeat;
    UINT       m_uXRepeat;
    UINT       m_uYRepeat;
    UINT       m_uTopMax;
};



///////////////////////////////////////////////////////////////////////////
// Specific CD3DXCodecs ///////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////


#define DECLARE_CODEC(name, bits, type) \
    class CD3DXCodec_##name : public CD3DXCodec \
    { \
    public: \
        CD3DXCodec_##name(D3DX_BLT *pBlt) \
            : CD3DXCodec(pBlt, (bits), (type)) {} \
    \
        virtual void Encode(UINT uRow, UINT uSlice, D3DXCOLOR *pColors); \
        virtual void Decode(UINT uRow, UINT uSlice, D3DXCOLOR *pColors); \
    }


#define DECLARE_CODEC_YUV(name) \
    class CD3DXCodec_##name : public CD3DXCodecYUV \
    { \
    public: \
        CD3DXCodec_##name(D3DX_BLT *pBlt) \
            : CD3DXCodecYUV(pBlt) {} \
    }


#define DECLARE_CODEC_DXT(name) \
    class CD3DXCodec_##name : public CD3DXCodecDXT \
    { \
    public: \
        CD3DXCodec_##name(D3DX_BLT *pBlt) \
            : CD3DXCodecDXT(pBlt) {} \
    }


DECLARE_CODEC(R8G8B8,    24, CODEC_RGB);
DECLARE_CODEC(A8R8G8B8,  32, CODEC_RGB);
DECLARE_CODEC(X8R8G8B8,  32, CODEC_RGB);
DECLARE_CODEC(R5G6B5,    16, CODEC_RGB);
DECLARE_CODEC(X1R5G5B5,  16, CODEC_RGB);
DECLARE_CODEC(A1R5G5B5,  16, CODEC_RGB);
DECLARE_CODEC(A4R4G4B4,  16, CODEC_RGB);
DECLARE_CODEC(R3G3B2,     8, CODEC_RGB);
DECLARE_CODEC(A8,         8, CODEC_RGB);
DECLARE_CODEC(A8R3G3B2,  16, CODEC_RGB);
DECLARE_CODEC(X4R4G4B4,  16, CODEC_RGB);
DECLARE_CODEC(A8P8,      16, CODEC_P);
DECLARE_CODEC(P8,         8, CODEC_P);
DECLARE_CODEC(L8,         8, CODEC_RGB);
DECLARE_CODEC(A8L8,      16, CODEC_RGB);
DECLARE_CODEC(A4L4,       8, CODEC_RGB);

DECLARE_CODEC(V8U8,      16, CODEC_UV);
DECLARE_CODEC(L6V5U5,    16, CODEC_UV);
DECLARE_CODEC(X8L8V8U8,  32, CODEC_UV);
DECLARE_CODEC(Q8W8V8U8,  32, CODEC_UV);
DECLARE_CODEC(V16U16,    32, CODEC_UV);
DECLARE_CODEC(W11V11U10, 32, CODEC_UV);

DECLARE_CODEC_YUV(UYVY);
DECLARE_CODEC_YUV(YUY2);
DECLARE_CODEC_DXT(DXT1);
DECLARE_CODEC_DXT(DXT2);
DECLARE_CODEC_DXT(DXT3);
DECLARE_CODEC_DXT(DXT4);
DECLARE_CODEC_DXT(DXT5);

#endif
