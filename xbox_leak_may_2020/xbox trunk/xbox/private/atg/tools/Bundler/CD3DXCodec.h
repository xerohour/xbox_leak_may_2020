//-----------------------------------------------------------------------------
// File: CD3DXCodec.cpp
//
// Desc: Contains general purpose blitting routines
//
// Hist: 01.29.02 - Updated to avoid using D3D
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#ifndef __CXD3DXCodec_H__
#define __CXD3DXCodec_H__

#include <windows.h>
#include <d3dx8.h>
#include <xgraphics.h>

#define CODEC_RGB 0x01
#define CODEC_P   0x02
#define CODEC_UV  0x03
#define CODEC_ZS  0x04

typedef signed char  INT8;
typedef short INT16;

typedef unsigned char  UINT8;
typedef unsigned short UINT16;

//----------------------------------------------------------------------------
//
// F2IBegin, F2I, F2IEnd
//
// Fast FLOAT->INT conversion.  F2IBegin sets and F2IEnd restores the FPU
// rounding mode.  F2I converts a float to an int.  You need to be careful of
// what other floating point code resides between F2IBegin and F2IEnd:  If
// something messes with the rounding mode, you could get unpredicted results.
//
//----------------------------------------------------------------------------


static UINT32 g_ulFPU;

static VOID
F2IBegin() {
    UINT32 ulFPU, ul;

    __asm {
        fnstcw WORD PTR [ulFPU]        // Get FPU control word
        mov    eax, DWORD PTR [ulFPU]
        or     eax, 0x0C00             // Rounding mode = CLAMP
        mov    DWORD PTR [ul], eax
        fldcw  WORD PTR [ul]           // Set FPU control word
    }

    // Save old FPU control word in thread-local storage
    g_ulFPU = ulFPU;
}


static inline INT
F2I(FLOAT f)
{
    volatile INT n;

    __asm {
        fld   f   // Load fload
        fistp n   // Store integer (and pop)
    }

    return n;
}

static VOID
F2IEnd() {
    // Get old FPU control word from thread-local storage
    UINT32 ulFPU = g_ulFPU;

    __asm {
        fldcw WORD PTR [ulFPU]    // Set FPU control word
    }
}


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
// CXD3DXCodec /////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

class CXD3DXCodec
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
    static CXD3DXCodec* Create(D3DX_BLT *pBlt);

    virtual ~CXD3DXCodec();

    virtual void Decode(UINT uRow, UINT uSlice, D3DXCOLOR *pColors);
    virtual void Encode(UINT uRow, UINT uSlice, D3DXCOLOR *pColors);

protected:
    static D3DCOLOR s_pPalette[256];

    void ColorKey(D3DXCOLOR *pColors);

    CXD3DXCodec(D3DX_BLT *pBlt, UINT uBPP, DWORD dwType);
};


///////////////////////////////////////////////////////////////////////////
// CXD3DXCodecYUV //////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

class CXD3DXCodecYUV : public CXD3DXCodec
{
public: 
    CXD3DXCodecYUV(D3DX_BLT *pBlt);
    virtual ~CXD3DXCodecYUV();

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
// CXD3DXCodecDXT //////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

class CXD3DXCodecDXT : public CXD3DXCodec
{
public: 
    CXD3DXCodecDXT(D3DX_BLT *pBlt);
    virtual ~CXD3DXCodecDXT();

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
// Specific CXD3DXCodecs ///////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////


#define DECLARE_CODEC(name, bits, type) \
    class CXD3DXCodec_##name : public CXD3DXCodec \
    { \
    public: \
        CXD3DXCodec_##name(D3DX_BLT *pBlt) \
            : CXD3DXCodec(pBlt, (bits), (type)) {} \
    \
        virtual void Encode(UINT uRow, UINT uSlice, D3DXCOLOR *pColors); \
        virtual void Decode(UINT uRow, UINT uSlice, D3DXCOLOR *pColors); \
    }


#define DECLARE_CODEC_YUV(name) \
    class CXD3DXCodec_##name : public CXD3DXCodecYUV \
    { \
    public: \
        CXD3DXCodec_##name(D3DX_BLT *pBlt) \
            : CXD3DXCodecYUV(pBlt) {} \
    }


#define DECLARE_CODEC_DXT(name) \
    class CXD3DXCodec_##name : public CXD3DXCodecDXT \
    { \
    public: \
        CXD3DXCodec_##name(D3DX_BLT *pBlt) \
            : CXD3DXCodecDXT(pBlt) {} \
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
