//-----------------------------------------------------------------------------
// FILE: BITFONT.CPP
//
// Desc: bit font rasterizer
//
// Copyright (c) 2001 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include <xtl.h>
#include "bitfont.h"




//----------------------------------------------------------------------------
DWORD BitsPerPixelOfD3DFORMAT( DWORD Format );
DWORD dwColorFromD3DXColor( D3DFORMAT dwSurfaceFormat, D3DCOLOR color );




//----------------------------------------------------------------------------
// default font
static const DWORD rgSmallFont[] =
{
    0x30304246, 0x0000000f, 0x00000060, 0x00000008,
    0x000f0800, 0x1e080000, 0x08000000, 0x0000002d,
    0x00003c08, 0x004b0800, 0x5a080000, 0x08000000,
    0x00000069, 0x00007808, 0x00870800, 0x96080000,
    0x08000000, 0x000000a5, 0x0000b408, 0x00c30800,
    0xd2080000, 0x08000000, 0x000000e1, 0x0000f008,
    0x00ff0800, 0x0e080000, 0x08000001, 0x0000011d,
    0x00012c08, 0x013b0800, 0x4a080000, 0x08000001,
    0x00000159, 0x00016808, 0x01770800, 0x86080000,
    0x08000001, 0x00000195, 0x0001a408, 0x01b30800,
    0xc2080000, 0x08000001, 0x000001d1, 0x0001e008,
    0x01ef0800, 0xfe080000, 0x08000001, 0x0000020d,
    0x00021c08, 0x022b0800, 0x3a080000, 0x08000002,
    0x00000249, 0x00025808, 0x02670800, 0x76080000,
    0x08000002, 0x00000285, 0x00029408, 0x02a30800,
    0xb2080000, 0x08000002, 0x000002c1, 0x0002d008,
    0x02df0800, 0xee080000, 0x08000002, 0x000002fd,
    0x00030c08, 0x031b0800, 0x2a080000, 0x08000003,
    0x00000339, 0x00034808, 0x03570800, 0x66080000,
    0x08000003, 0x00000375, 0x00038408, 0x03930800,
    0xa2080000, 0x08000003, 0x000003b1, 0x0003c008,
    0x03cf0800, 0xde080000, 0x08000003, 0x000003ed,
    0x0003fc08, 0x040b0800, 0x1a080000, 0x08000004,
    0x00000429, 0x00043808, 0x04470800, 0x56080000,
    0x08000004, 0x00000465, 0x00047408, 0x04830800,
    0x92080000, 0x08000004, 0x000004a1, 0x0004b008,
    0x04bf0800, 0xce080000, 0x08000004, 0x000004dd,
    0x0004ec08, 0x04fb0800, 0x0a080000, 0x08000005,
    0x00000519, 0x00052808, 0x05370800, 0x46080000,
    0x08000005, 0x00000555, 0x00056408, 0x05730800,
    0x82080000, 0x08000005, 0x00000591, 0xffffffff,
    0xffffffff, 0xffffffff, 0xffffffff, 0xc3e7ffff,
    0xe7e7c3c3, 0xffe7e7ff, 0xffffffff, 0x999999ff,
    0xffffffff, 0xffffffff, 0xffffffff, 0xc980c9c9,
    0xc980c9c9, 0xffffffc9, 0xc3e7e7ff, 0xe7cf9f99,
    0xc399f9f3, 0xffffe7e7, 0x25278fff, 0xcfe7f389,
    0xf1e4a491, 0xffffffff, 0x9393c7ff, 0x99909fc7,
    0xffffc499, 0xffffffff, 0xffe7e7e7, 0xffffffff,
    0xffffffff, 0xf3ffffff, 0xcfcfe7e7, 0xe7cfcfcf,
    0xfffff3e7, 0xe7cfffff, 0xf3f3f3e7, 0xe7e7f3f3,
    0xffffffcf, 0xc9ffffff, 0xc9e380e3, 0xffffffff,
    0xffffffff, 0xe7e7ffff, 0xffe7e781, 0xffffffff,
    0xffffffff, 0xffffffff, 0xe3e3ffff, 0xffffe7f3,
    0xffffffff, 0xff81ffff, 0xffffffff, 0xffffffff,
    0xffffffff, 0xffffffff, 0xffffe3e3, 0xffffffff,
    0xf3f3f9f9, 0xcfcfe7e7, 0xffff9f9f, 0xe1ffffff,
    0xccc8c8cc, 0xe1ccc4c4, 0xffffffff, 0xe3f3ffff,
    0xf3f3f383, 0xfff3f3f3, 0xffffffff, 0x9999c3ff,
    0xcfe7f3f9, 0xffff819f, 0xffffffff, 0xf99999c3,
    0x9999f9e3, 0xffffffc3, 0xcfffffff, 0xc9c9c9cf,
    0xf9f98099, 0xffffffff, 0x9f81ffff, 0xf9839f9f,
    0xff87f3f9, 0xffffffff, 0xcfe7e3ff, 0x99999983,
    0xffffc399, 0xffffffff, 0xf3f3f981, 0xcfcfe7e7,
    0xffffffcf, 0xc3ffffff, 0xc3899999, 0xc3999991,
    0xffffffff, 0x99c3ffff, 0xc1999999, 0xffc7e7f3,
    0xffffffff, 0xe3ffffff, 0xffffffe3, 0xffffe3e3,
    0xffffffff, 0xe3e3ffff, 0xe3ffffff, 0xffe7f3e3,
    0xf9ffffff, 0x9fcfe7f3, 0xf9f3e7cf, 0xffffffff,
    0xffffffff, 0x81ff81ff, 0xffffffff, 0xffffffff,
    0xe7cf9fff, 0xe7f3f9f3, 0xffff9fcf, 0xffffffff,
    0xf39999c3, 0xe7ffe7e7, 0xffffffe7, 0x81ffffff,
    0x24303c3c, 0x803f3024, 0xffffffff, 0xc3e7ffff,
    0x81999999, 0xff999999, 0xffffffff, 0x999983ff,
    0x99998399, 0xffff8399, 0xffffffff, 0x9f9999c3,
    0x99999f9f, 0xffffffc3, 0x87ffffff, 0x99999993,
    0x87939999, 0xffffffff, 0x9f81ffff, 0x9f839f9f,
    0xff819f9f, 0xffffffff, 0x9f9f81ff, 0x9f9f839f,
    0xffff9f9f, 0xffffffff, 0x9f9999c3, 0x9999919f,
    0xffffffc1, 0x99ffffff, 0x81999999, 0x99999999,
    0xffffffff, 0xe7c3ffff, 0xe7e7e7e7, 0xffc3e7e7,
    0xffffffff, 0xf9f9f9ff, 0x99f9f9f9, 0xffffc399,
    0xffffffff, 0x93939999, 0x99939387, 0xffffff99,
    0x9fffffff, 0x9f9f9f9f, 0x819f9f9f, 0xffffffff,
    0x9c9cffff, 0x94949488, 0xff9c9c9c, 0xffffffff,
    0x8c9c9cff, 0x9c989084, 0xffff9c9c, 0xffffffff,
    0x999999c3, 0x99999999, 0xffffffc3, 0x83ffffff,
    0x83999999, 0x9f9f9f9f, 0xffffffff, 0x99c3ffff,
    0x99999999, 0xf3c39999, 0xfffffff9, 0x999983ff,
    0x99938399, 0xffff9999, 0xffffffff, 0xcf9f99c3,
    0x99f9f3e7, 0xffffffc3, 0x81ffffff, 0xe7e7e7e7,
    0xe7e7e7e7, 0xffffffff, 0x9999ffff, 0x99999999,
    0xffc39999, 0xffffffff, 0x999999ff, 0x99999999,
    0xffffe7c3, 0xffffffff, 0x949c9c9c, 0xc9c99494,
    0xffffffc9, 0x99ffffff, 0xe7e7cb99, 0x999999d3,
    0xffffffff, 0x9999ffff, 0xe7c39999, 0xffe7e7e7,
    0xffffffff, 0xf9f981ff, 0x9fcfe7f3, 0xffff819f,
    0xffffffff, 0xcfcfcfc3, 0xcfcfcfcf, 0xc3cfcfcf,
    0x9fffffff, 0xe7cfcf9f, 0xf9f3f3e7, 0xfffffff9,
    0xf3c3ffff, 0xf3f3f3f3, 0xf3f3f3f3, 0xe7ffc3f3,
    0xffff99c3, 0xffffffff, 0xffffffff, 0xffffffff,
    0xffffffff, 0xffffffff, 0x00ffffff, 0xf3e7c7ff,
    0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
    0xc1f9f9c3, 0xffc19999, 0xffffffff, 0x839f9fff,
    0x99999999, 0xffff8399, 0xffffffff, 0x99c3ffff,
    0x999f9f9f, 0xffffffc3, 0xf9ffffff, 0x9999c1f9,
    0xc1999999, 0xffffffff, 0xffffffff, 0x819999c3,
    0xffc39f9f, 0xffffffff, 0xcfcfe1ff, 0xcfcf81cf,
    0xffffcfcf, 0xffffffff, 0x99c1ffff, 0x99999999,
    0x83f9f9c1, 0x9fffffff, 0x9999839f, 0x99999999,
    0xffffffff, 0xffe7e7ff, 0xe7e7e787, 0xff81e7e7,
    0xffffffff, 0xc3fff3f3, 0xf3f3f3f3, 0xf3f3f3f3,
    0xffffff87, 0x99999f9f, 0x99938793, 0xffffff99,
    0x87ffffff, 0xe7e7e7e7, 0x81e7e7e7, 0xffffffff,
    0xffffffff, 0x94949481, 0xff9c9494, 0xffffffff,
    0x83ffffff, 0x99999999, 0xffff9999, 0xffffffff,
    0x99c3ffff, 0x99999999, 0xffffffc3, 0xffffffff,
    0x999983ff, 0x83999999, 0xff9f9f9f, 0xffffffff,
    0x999999c1, 0xf9c19999, 0xfffff9f9, 0x99ffffff,
    0x9f9f8f91, 0xffff9f9f, 0xffffffff, 0x9fc1ffff,
    0xf9f9c39f, 0xffffff83, 0xcfffffff, 0xcfcf81cf,
    0xe1cfcfcf, 0xffffffff, 0xffffffff, 0x99999999,
    0xffc19999, 0xffffffff, 0x99ffffff, 0x99999999,
    0xffffe7c3, 0xffffffff, 0x949cffff, 0xc9949494,
    0xffffffc9, 0xffffffff, 0xc39999ff, 0x9999c3e7,
    0xffffffff, 0xffffffff, 0x99999999, 0xf3c39999,
    0xffff0fe7, 0x81ffffff, 0xcfe7f3f9, 0xffff819f,
    0xffffffff, 0xe7e7e7f3, 0xe7cf9fcf, 0xfff3e7e7,
    0xe7ffffff, 0xe7e7e7e7, 0xe7e7e7e7, 0xffe7e7e7,
    0xe7cfffff, 0xf9f3e7e7, 0xe7e7e7f3, 0xffffffcf,
    0x71248eff, 0xffffffff, 0xffffffff, 0xffffffff,
    0x81818181, 0x81818181, 0xffffff81
};




//----------------------------------------------------------------------------
// Name: Constructor
//
// Desc: 
//----------------------------------------------------------------------------
BitFont::BitFont( )
{
    // start out with our default font
    m_pBitFontInfo = (BitFontInfo *)rgSmallFont;
}




//----------------------------------------------------------------------------
// Name: Destructor
//
// Desc: 
//----------------------------------------------------------------------------
BitFont::~BitFont()
{
    if( m_pBitFontInfo != (BitFontInfo *)rgSmallFont )
        delete m_pBitFontInfo;
}




//----------------------------------------------------------------------------
// Name: Load
//
// Desc: Intialize a BitFont from a specified filename -- DELETE THIS??
//----------------------------------------------------------------------------
bool BitFont::Load( const char* filename )
{
#ifdef NEVER
    HFILE hf = _lopen( filename, OF_READ );

    if( hf != HFILE_ERROR )
    {
        // get the file size and move back to the start
        LONG lSize = _llseek( hf, 0, FILE_END );
        _llseek( hf, 0, FILE_BEGIN );

        // need at least a BITFONT and one BITFONTINFO
        if(lSize < sizeof( BitFontInfo ) + sizeof( BitFontCharInfo ) )
            goto err;

        // malloc our BITFONT block and read it in
        m_pBitFontInfo = (BitFontInfo *)new BYTE[lSize];
        if( !m_pBitFontInfo )
            goto err;

        if(_lread( hf, m_pBitFontInfo, (UINT)lSize ) != (UINT)lSize )
            goto err;

        // if the sig doesn't match or we don't have any bitfontinfos then bail
        if( m_pBitFontInfo->dwSig != BITFONT_SIG || !m_pBitFontInfo->cBfis )
            goto err;

        _lclose( hf );
        return true;
    }

err:
    if( hf != HFILE_ERROR )
        _lclose( hf );

    delete [] m_pBitFontInfo;
    m_pBitFontInfo = NULL;
#endif
    return false;
}




//----------------------------------------------------------------------------
// Name: GetTextStringLength
// 
// Desc: Returns pixel height and width of string
//----------------------------------------------------------------------------
void BitFont::GetTextStringLength( DWORD *pdwWidth, DWORD *pdwHeight, 
                                   const char *str )
{
    if( pdwHeight )
        *pdwHeight = m_pBitFontInfo->dwFontHeight;

    if( pdwWidth )
    {
        DWORD dwWidth = 0;

        // go through the string adding up the widths
        for( const char *sz = str; *sz; sz++ )
        {
            DWORD iBfi = *sz - 32;

            if( iBfi >= m_pBitFontInfo->cBfis )
                iBfi = 0;

            dwWidth += m_pBitFontInfo->rgBfi[iBfi].bWidth;
        }

        *pdwWidth = dwWidth;
    }
}




//----------------------------------------------------------------------------
// Name: DrawText16
//
// Desc: Renders the font to a 16-bit surface
//----------------------------------------------------------------------------
void DrawText16( D3DSURFACE_DESC* pDesc, D3DLOCKED_RECT* pLock,
                 BitFontInfo* m_pBitFontInfo, const char* str, int iX, int iY,
                 DWORD dwFlags, DWORD dwcolFore,DWORD dwcolBack )
{
    bool fdrawBkgnd = !( dwFlags & DRAWTEXT_TRANSPARENTBKGND );

    // rgColor[0] is background color
    WORD rgColor[2] = { LOWORD( dwcolBack ), LOWORD( dwcolFore ) };

    // pointer to font bytestream data
    BYTE *lpData = (BYTE *)&m_pBitFontInfo->rgBfi[m_pBitFontInfo->cBfis];

    // pointer to end of dest surface
    WORD *pwSurfaceMaxY = (WORD *)( (BYTE *)pLock->pBits + pLock->Pitch * 
                                    pDesc->Height );

    // go through the string
    for( const char *sz = str; *sz; sz++ )
    {
        DWORD iBfi = *sz - 32;

        if( iBfi >= m_pBitFontInfo->cBfis )
            iBfi = 0;

        // get data for this char
        BitFontCharInfo *pbfi = &m_pBitFontInfo->rgBfi[iBfi];
        BYTE *lpBits = &lpData[pbfi->dwOffset];

        // where we're drawing
        WORD *pwSurface = (WORD *)( (BYTE *)pLock->pBits + pLock->Pitch * iY )
                                             + iX;

        // max x for the line we're on
        WORD *pwSurfaceMaxX = (WORD *)( (BYTE *)pLock->pBits + pLock->Pitch 
                                                * iY ) + pDesc->Width;

        // go through bitfont drawing each line
        for( DWORD y = 0; y < m_pBitFontInfo->dwFontHeight; y++ )
        {
            DWORD dwWidth = pbfi->bWidth;

            while( dwWidth )
            {
                DWORD dwByte = *lpBits++;
                DWORD dwNumBits = min( dwWidth, 8 );

                dwWidth -= dwNumBits;

                // check number of bits and clipping
                while( dwNumBits-- && ( pwSurface < pwSurfaceMaxX ) )
                {
                    DWORD iIndex = !( dwByte & 0x80 );

                    if( fdrawBkgnd || iIndex )
                        *pwSurface = rgColor[iIndex];

                    pwSurface++;
                    dwByte <<= 1;
                }
            }

            // get next line data
            pwSurface = (WORD *)( (BYTE *)( pwSurface - pbfi->bWidth - dwWidth ) 
                                            + pLock->Pitch );
            pwSurfaceMaxX = (WORD *)( (BYTE *)pwSurfaceMaxX + pLock->Pitch );

            // check clipping
            if( pwSurface >= pwSurfaceMaxY )
                break;
        }

        // move to next char position
        iX += pbfi->bWidth;
    }
}




//----------------------------------------------------------------------------
// Name: DrawText32
//
// Desc: Renders the font to a 32-bit surface
//----------------------------------------------------------------------------
void DrawText32( D3DSURFACE_DESC* pDesc, D3DLOCKED_RECT* pLock,
                 BitFontInfo* m_pBitFontInfo, const char* str, int iX, int iY,
                 DWORD dwFlags, DWORD dwcolFore, DWORD dwcolBack)
{
    bool fdrawBkgnd = !( dwFlags & DRAWTEXT_TRANSPARENTBKGND );
    DWORD rgColor[2] = { dwcolBack, dwcolFore };

    BYTE *lpData = (BYTE *)&m_pBitFontInfo->rgBfi[m_pBitFontInfo->cBfis];
    DWORD *pdwSurfaceMaxY = (DWORD *)( (BYTE *)pLock->pBits + pLock->Pitch 
                                                * pDesc->Height );

    for( const char *sz = str; *sz; sz++ )
    {
        DWORD iBfi = *sz - 32;

        if( iBfi >= m_pBitFontInfo->cBfis )
            iBfi = 0;

        BitFontCharInfo *pbfi = &m_pBitFontInfo->rgBfi[iBfi];
        BYTE *lpBits = &lpData[pbfi->dwOffset];

        DWORD *pdwSurface = (DWORD *)( (BYTE *)pLock->pBits + pLock->Pitch * iY )
                                                 + iX;
        DWORD *pdwSurfaceMaxX = (DWORD *)( (BYTE *)pLock->pBits + pLock->Pitch 
                                             * iY ) + pDesc->Width;

        for( DWORD y = 0; y < m_pBitFontInfo->dwFontHeight; y++ )
        {
            DWORD dwWidth = pbfi->bWidth;

            while( dwWidth )
            {
                DWORD dwByte = *lpBits++;
                DWORD dwNumBits = min( dwWidth, 8 );

                dwWidth -= dwNumBits;

                while( dwNumBits-- && ( pdwSurface < pdwSurfaceMaxX ) )
                {
                    DWORD iIndex = !( dwByte & 0x80 );

                    if( fdrawBkgnd || iIndex )
                        *pdwSurface = rgColor[iIndex];

                    pdwSurface++;
                    dwByte <<= 1;
                }
            }

            pdwSurface = (DWORD *)( (BYTE *)(pdwSurface - pbfi->bWidth - dwWidth ) 
                                            + pLock->Pitch );
            pdwSurfaceMaxX = (DWORD *)( (BYTE *)pdwSurfaceMaxX + pLock->Pitch );
            if( pdwSurface >= pdwSurfaceMaxY )
                break;
        }

        iX += pbfi->bWidth;
    }
}




//----------------------------------------------------------------------------
// Name: DrawText
// 
// Desc: Draw the specified text to a surface
//----------------------------------------------------------------------------
void BitFont::DrawText( IDirect3DSurface8 *pSurface, const char* str, int iX,
                        int iY, DWORD dwFlags, D3DCOLOR colFore, 
                        D3DCOLOR colBack )
{
    typedef void ( *PFNDRAWTEXT )( D3DSURFACE_DESC* pDesc, D3DLOCKED_RECT* pLock,
                                   BitFontInfo* m_pBitFontInfo, const char* str,
                                   int iX, int iY, DWORD dwFlags, DWORD dwcolFore,
                                   DWORD dwcolBack);

    PFNDRAWTEXT rgpfnDrawText[] =
    {
        DrawText16,
        DrawText32
    };

    D3DLOCKED_RECT lockRect;
    if( pSurface->LockRect( &lockRect, NULL, D3DLOCK_TILED ) != S_OK )
    {
        _asm int 3;
        return;
    }

    D3DSURFACE_DESC desc;
    if( FAILED( pSurface->GetDesc( &desc ) ) )
    {
        _asm int 3;
        return;
    }

    UINT BitsPerPixel = BitsPerPixelOfD3DFORMAT( desc.Format );

    // use the correct draw routine for the job
    rgpfnDrawText[BitsPerPixel == 32]( &desc, &lockRect,m_pBitFontInfo, str, iX,
                                       iY, dwFlags, 
                                       dwColorFromD3DXColor( desc.Format, colFore ),
                                       dwColorFromD3DXColor( desc.Format, colBack ) );

    // Relase the lock on the surface
    pSurface->UnlockRect();
}




//----------------------------------------------------------------------------
// Name: dwColorFromD3DXColor
//
// Desc: Given a surfaceformat and color, return the color as a native pixel
DWORD dwColorFromD3DXColor( D3DFORMAT dwSurfaceFormat, D3DCOLOR color )
{
    DWORD dwColor;
    D3DXCOLOR col( color );

    switch( dwSurfaceFormat )
    {
        // 32-bit
        case D3DFMT_A8R8G8B8:
        case D3DFMT_LIN_A8R8G8B8:
            dwColor = ( ( (long)( col.a * 255 ) ) << 24 ) |
                      ( ( (long)( col.r * 255 ) ) << 16 ) |
                      ( ( (long)( col.g * 255) ) << 8 ) |
                      (long)( col.b * 255 );
        break;

        // 32-bit
        case D3DFMT_X8R8G8B8:
        case D3DFMT_LIN_X8R8G8B8:
            dwColor = 0xff000000L |
                      ( ( (long)( col.r * 255 ) ) << 16 ) |
                      ( ( (long)( col.g * 255 ) ) << 8 ) |
                      (long)( col.b * 255 );
        break;

        // 16-bit
        case D3DFMT_R5G6B5:
        case D3DFMT_LIN_R5G6B5:
            dwColor = ( ( (long)( col.r * 255 ) ) << 11 ) |
                      ( ( (long)( col.g * 255 ) ) << 5 ) |
                      (long)( col.b * 255 );
        break;

        case D3DFMT_X1R5G5B5:
        case D3DFMT_LIN_X1R5G5B5:
            dwColor = ( 1 << 15 ) |
                      ( ( (long)( col.r * 255 ) ) << 10 ) |
                      ( ( (long)( col.g * 255) ) << 5 ) |
                      (long)( col.b * 255 );
        break;

        case D3DFMT_A4R4G4B4:
        case D3DFMT_LIN_R4G4B4A4:
            dwColor = ( ( (long)( col.a * 15 ) ) << 12 ) |
                      ( ( (long)( col.r * 15 ) ) << 8 ) |
                      ( ( (long)( col.g * 15 ) ) << 4 ) |
                      (long)( col.b * 15 );
        break;

        case D3DFMT_A1R5G5B5:
        case D3DFMT_LIN_A1R5G5B5:
            dwColor = ( ( (long)( col.a > .9f ) ) << 15 ) |
                      ( ( (long)( col.r * 31 ) ) << 10 ) |
                      ( ( (long)( col.g * 31 ) ) << 5 ) |
                      (long)( col.b * 31 );
        break;

        default:
            dwColor = 0;
            _asm int 3;
        break;
    }

    return dwColor;
};

