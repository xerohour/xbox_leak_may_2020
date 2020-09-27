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

// pack the structure as we're reading it from the disk
#pragma pack(1)

// disable C4200: nonstandard extension used : zero-sized array in struct/union
#pragma warning(push)
#pragma warning(disable:4200)


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

    //----------------------------------------------------------------------------
    // Load a BitFont font from a file
    bool Load(const char* filename);

    //----------------------------------------------------------------------------
    // Returns pixel height and width of string
    void GetTextStringLength(DWORD *pdwWidth, DWORD *pdwHeight, 
        const char* Message);

    //----------------------------------------------------------------------------
    // Draw the text to a surface
    void BitFont::DrawText
    (
        IDirect3DSurface8 *pSurface,
        const char*     str,
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

#endif // _BITFONT_H_

