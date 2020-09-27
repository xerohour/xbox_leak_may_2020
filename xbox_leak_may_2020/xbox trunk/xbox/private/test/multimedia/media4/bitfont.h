//-----------------------------------------------------------------------------
// FILE: BITFONT.H
//
// Desc: bit font header file
//
// Copyright (c) 1999-2000 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#pragma once
#ifndef _BITFONT_H_
#define _BITFONT_H_


#ifdef XBOX
#include <xtl.h>
#else
#include <windows.h>
#include <d3d8.h>
#endif

// disable C4200: nonstandard extension used : zero-sized array in struct/union
#pragma warning(push)
#pragma warning(disable:4200)

// pack the structure as we're reading it from the disk
#pragma pack(1)

namespace Media4 {

//----------------------------------------------------------------------------
// BXF file signature
#define BITFONT_SIG     '00BF'

//----------------------------------------------------------------------------
struct BitFontCharInfo
{
    BYTE bWidth;
    DWORD dwOffset;
};

//----------------------------------------------------------------------------
struct BitFontInfo
{
    DWORD   dwSig;
    DWORD   dwFontHeight;
    DWORD   cBfis;
    BitFontCharInfo rgBfi[];
};

#pragma warning(pop)
#pragma pack()


//----------------------------------------------------------------------------
// Draw flags
const DWORD DRAWTEXT_TRANSPARENTBKGND =     0x00000001;


//----------------------------------------------------------------------------
// A BitFont class that can be used with the DrawText routines above to
//  render text to various surfaces.
class BitFont
{
public:
    //----------------------------------------------------------------------------
    BitFont();

    //----------------------------------------------------------------------------
    ~BitFont();

#if 0

    //----------------------------------------------------------------------------
    // Load a BitFont font from a file
    bool Load(const WCHAR* filename);

#endif 0

    //----------------------------------------------------------------------------
    // Returns pixel height and width of string
    void GetTextStringLength(DWORD *pdwWidth, DWORD *pdwHeight, 
        const WCHAR* Message);

    //----------------------------------------------------------------------------
    // Draw the text to a surface
    void BitFont::DrawText
    (
        IDirect3DSurface8 *pSurface,
        const WCHAR*    str,
        int             iX,
        int             iY,
        DWORD           dwFlags,
        D3DCOLOR        colFore,
        D3DCOLOR        colBack
    );

private:
    //----------------------------------------------------------------------------
    // various pointers to font data, index tables and width tables
    BitFontInfo     *m_pBitFontInfo;
};
}
#endif // _BITFONT_H_

