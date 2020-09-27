//****************************************************************************
//
// XBox font drawer
//
// History:
//
//   07/06/00 [andrewso] - Created
//   08/04/00 [andrewso] - Added compressed glyph bitmaps
//
//****************************************************************************

// PERFORMANCE NOTE: This file defines a template that is instantiated
//   : for each color depth (1, 2, 3, or 4 bytes per pixel) * 3.  The code
//   : size for this file can be reduced by NULLING out the entries in the
//   : rgTextOutTable that you do not need.  The compiler/linker will
//   : not do this for you automatically.

#ifdef _XBOX

#include <xtl.h>
#include <xdbg.h>
#include "xfont.h"
#include "xfontformat.h"

#else 

#include <windows.h>
#include "xfont-pc.h"
#include "xfontformat-pc.h"

#endif

#include <assert.h>
#include "font.h"

extern "C"
HRESULT __fastcall XFONT_GetCharacterData(XFONT *, WCHAR, Glyph **, unsigned *);

// Describes a the display format.
struct FORMAT
{
    unsigned uPixelSize;

    unsigned uAlphaBits;
    unsigned uAlphaShift;
    unsigned uRedBits;
    unsigned uRedShift;
    unsigned uGreenBits;
    unsigned uGreenShift;
    unsigned uBlueBits;
    unsigned uBlueShift;
};

//============================================================================
// This template function manages drawing a string onto a surface.  It is 
// created as a template to allow the compiler to create and optimize 
// specialized version of this function for the different color depths and 
// antialiasing modes.  
//
// NOTE: We use a template class that contains a static function because
//   : VC6+ doesn't support specializing template functions by anything
//   : but type...so the "MODE" parameter to the template has no effect.
//   : Works fine with a template class.  
//============================================================================

template <class BASE>
class Paint
{
public:

//============================================================================
// Draws the background of a text string.  This just figures out how 
// wide the string is and blasts in the background color.
//============================================================================

static HRESULT DrawBackground
(
    Font *pFont,
    LPCWSTR wstr,                       // [in] String to render.
    unsigned cch,                       // [in] Length, or -1 for zero termination
    DWORD dwBackColor,                  // [in] the background color bit pattern
    long iCellX,                        // [in] what column to start drawing at
    long lCellMinY,                     // [in] what row to start drawing at
    long lCellMaxY,                     // [in] what row to stop drawing at (exclusive)
    unsigned uClipLeft,                 // [in] left clip
    unsigned uClipRight,                // [in] right clip
    unsigned uIntercharacterSpacing,    // [in] extra spaces between characters
    LPCVOID pBits,                      // [in] the bits to draw to
    unsigned Pitch                      // [in] the pitch to draw to.
)
{
    HRESULT hr;

    // The pitch, in BASE units.
    unsigned uPitch = Pitch / sizeof(BASE);
    assert(!(Pitch % sizeof(BASE)));

    BASE *pSurface = (BASE *)pBits;

    long lExtraSpace = (long)uIntercharacterSpacing;
    long lWidth = 0;

    // Calculate the width of the box.
    for (unsigned ich = 0; ich < cch && *wstr; ich++, wstr++)
    {
        Glyph *pGlyph;
        unsigned cbGlyphSize;

        // Get the character info.
        hr = XFONT_GetCharacterData((XFONT *)pFont, *wstr, &pGlyph, &cbGlyphSize);

        if (FAILED(hr)) 
        {
            return hr;
        }

        // If this is the last character in the string then do not draw any
        // extra spacing.  We may need to blank out a bit more if the character
        // draws beyond its character cell.
        //
        if (ich + 1 == cch || !*(wstr + 1))
        {
            lExtraSpace = max(pGlyph->uAdvance, pGlyph->uBitmapWidth + pGlyph->iBearingX) - pGlyph->uAdvance;
        }

        lWidth += pGlyph->uAdvance + lExtraSpace;
    }

    // Do the fill.
    const long lCellMinX = max((long)uClipLeft, iCellX);
    const long lCellMaxX = min((long)uClipRight, iCellX + lWidth);

    if (lCellMinX < lCellMaxX)
    {   
        unsigned       cRows = lCellMaxY - lCellMinY;
        const unsigned cCols = lCellMaxX - lCellMinX;
        const unsigned cBetweenLineAdvance = uPitch - cCols;

        BASE *pPixel = pSurface + uPitch * lCellMinY + lCellMinX;

        while (cRows--)
        {
            unsigned cDraw = cCols;
            
            while (cDraw--)
            {
                *(pPixel++) = (BASE)dwBackColor;
            }

            pPixel += cBetweenLineAdvance;
        }
    }

    return NOERROR;
}

//============================================================================
// Helper to decode the compressed image.  Real good inline candidate.
//============================================================================

__forceinline static unsigned DecodeWidth
(
    DWORD **ppdwBitmap,
    unsigned *puBitmap,
    unsigned *pcBits,
    const unsigned uPacketMask,
    const unsigned uRLEPacketWidth,
    const bool fRepeat
)
{
    unsigned uValue;
    unsigned uTotal = 0;

    DWORD *pdwBitmap = *ppdwBitmap;
    unsigned uBitmap = *puBitmap;
    unsigned cBits = *pcBits;

    do
    {
        uValue = uBitmap & uPacketMask;

        cBits += uRLEPacketWidth;
        
        if (cBits / 32)
        {
            uBitmap = *(++pdwBitmap);
            cBits %= 32;

            uValue |= ((uPacketMask >> (uRLEPacketWidth - cBits)) & uBitmap) << (uRLEPacketWidth - cBits);
            uBitmap >>= cBits;
        }
        else
        {
            uBitmap >>= uRLEPacketWidth;
        }

        uTotal += uValue;
    }
    while (fRepeat && uValue == uPacketMask);

    *ppdwBitmap = pdwBitmap;
    *puBitmap = uBitmap;
    *pcBits = cBits;

    return uTotal;
}

//============================================================================
// Draws a string of text on a surface that has no alpha support using
// a font with no antialiasing information.
//============================================================================

static HRESULT TextOutNoAlpha
(
    Font *pFont,
    LPCWSTR wstr,                       // [in] String to render.
    unsigned cch,                       // [in] Length, or -1 for zero termination
    long iCellX,                        // [in] X coordinate of the left of the character cell
    long iCellY,                        // [in] Y coordinate of the top of the characer cell
    unsigned uClipLeft,                 // [in] coordinates of the clipping rectangle
    unsigned uClipTop,
    unsigned uClipRight,
    unsigned uClipBottom,
    unsigned uIntercharacterSpacing,    // [in] extra spaces between characters
    unsigned uCellHeight,               // [in] total cell height
    unsigned uDescent,                  // [in] bottom of the cell to the baseline
    LPCVOID pBits,                      // [in] the bits to draw to
    unsigned Pitch,                     // [in] the pitch to draw to.
    BOOL fFillBackground,               // [in] draw the background color?
    DWORD dwTextColor,                  // [in] the foreground color bit pattern
    DWORD dwBackColor,                  // [in] the background color bit pattern
    unsigned uRLEPacketWidth,           // [in] the # of bits per RLE packet
    unsigned,                           // [in] the # of bits used to store antialiasing information
    FORMAT *                            // [in] describes the color format of a pixel
)
{
    HRESULT hr;

    // The pitch, in BASE units.
    unsigned uPitch = Pitch / sizeof(BASE);
    assert(!(Pitch % sizeof(BASE)));

    BASE *pSurface = (BASE *)pBits;
    const long oBaseline = uCellHeight - uDescent;

    const long lCellMinY = max((long)uClipTop, iCellY);
    const long lCellMaxY = min((long)uClipBottom, iCellY + (long)uCellHeight);

    const unsigned uPacketMask = (unsigned)0xFFFFFFFF >> (32 - uRLEPacketWidth);

    //_asm int 3;

    // We need to pre-draw the background for this string because the characters
    // may draw outside of their character cell.  If we drew the backgrounds with
    // the character then the background for the next character might overwrite
    // the graphics for the character we just drew.  This actually happens 
    // quite a bit with italics fonts.
    //
    // This will really blow chunks if the font is loaded with either no 
    // cache or a cache that is too small to handle the string being processed.
    // Otherwise the first background-drawing pass will just pre-load the cache
    // and no work will be wasted.
    //
    if (fFillBackground && lCellMinY < lCellMaxY)
    {
        hr = DrawBackground(pFont,
                            wstr,   
                            cch,
                            dwBackColor,
                            iCellX,
                            lCellMinY,
                            lCellMaxY,
                            uClipLeft,
                            uClipRight,
                            uIntercharacterSpacing,
                            pBits,
                            Pitch);

        if (FAILED(hr))
        {
            return hr;
        }
    }

    // _asm int 3;

    // Walk the string and draw the characters.
    for (unsigned ich = 0; ich < cch && *wstr; ich++, wstr++)
    {
        Glyph *pGlyph;
        unsigned cbGlyphSize;

        DWORD *pdwBitmap;

        // Get the character info.
        hr = XFONT_GetCharacterData((XFONT *)pFont, *wstr, &pGlyph, &cbGlyphSize);

        if (FAILED(hr)) 
        {
            return hr;
        }

        //_asm int 3;

        pdwBitmap = (DWORD *)(pGlyph + 1);      // May be unaligned...

        // 
        // Walk the character and draw its bitmap.
        //

        const unsigned uBitmapWidth = pGlyph->uBitmapWidth;

        const long lBitmapX = iCellX + pGlyph->iBearingX;
        const long lBitmapY = iCellY + oBaseline - pGlyph->iBearingY;

        const long lMinX = (unsigned)max((long)uClipLeft, lBitmapX);
        const long lMaxX = (unsigned)min((long)uClipRight, lBitmapX + (long)uBitmapWidth);

        BASE *pMinY = pSurface + max((long)uClipTop, lBitmapY) * uPitch;
        BASE *pMaxY = pSurface + min((long)uClipBottom, lBitmapY + pGlyph->uBitmapHeight) * uPitch;

        if (lMinX < lMaxX && pMinY < pMaxY)
        {
            BASE *pFirstRow = pSurface + lBitmapY * uPitch;

            unsigned iPixel = 0;
            unsigned cPixels = uBitmapWidth * pGlyph->uBitmapHeight;
            unsigned cBits = 0;
            unsigned cToDraw;

            unsigned uBitmap = *pdwBitmap;
            unsigned uValue;

            bool fForeground = false;

            while (iPixel < cPixels)
            {
                cToDraw = DecodeWidth(&pdwBitmap,
                                      &uBitmap,
                                      &cBits,
                                      uPacketMask,
                                      uRLEPacketWidth,
                                      true);

                if (fForeground && cToDraw)
                {
                    BASE *pRow = pFirstRow + uPitch * (iPixel / uBitmapWidth);
                    unsigned uPixelInBitmapX = iPixel % uBitmapWidth;
                    unsigned cDrawing = cToDraw;

                    for (;;)
                    {
                        const unsigned cMove = min(cDrawing, uBitmapWidth - uPixelInBitmapX);
                        const long lPixelX = (long)uPixelInBitmapX + lBitmapX;
                        const long lPixelMin = max(lPixelX, lMinX);
                        const long lPixelMax = min(lPixelX + (long)cMove, lMaxX);

                        if (lPixelMin < lPixelMax && pRow >= pMinY && pRow <= pMaxY)
                        {
                            long cCols = lPixelMax - lPixelMin;
                            BASE *pPixel = pRow + lPixelMin;

                            while (cCols--)
                            {
                                *(pPixel++) = (BASE)dwTextColor;
                            }
                        }

                        cDrawing -= cMove;

                        if (!cDrawing)
                        {
                            break;
                        }

                        pRow += uPitch;
                        uPixelInBitmapX = 0;
                    }
                }


                fForeground = !fForeground;
                iPixel += cToDraw;                
            }
        }

        // Next character.
        iCellX += pGlyph->uAdvance + uIntercharacterSpacing;
    }

    return NOERROR;
}

//============================================================================
// Draws a string to a texture that has an alpha channel.
//============================================================================

static HRESULT TextOutAlpha
(
    Font *pFont,
    LPCWSTR wstr,               // [in] String to render.
    unsigned cch,               // [in] Length, or -1 for zero termination
    long iCellX,                // [in] X coordinate of the left of the character cell
    long iCellY,                // [in] Y coordinate of the top of the characer cell
    unsigned uClipLeft,         // [in] coordinates of the clipping rectangle
    unsigned uClipTop,
    unsigned uClipRight,
    unsigned uClipBottom,
    unsigned uIntercharacterSpacing,    // [in] extra spaces between characters
    unsigned uCellHeight,       // [in] total cell height
    unsigned uDescent,          // [in] bottom of the cell to the baseline
    LPCVOID pBits,              // [in] the bits to draw to
    unsigned Pitch,             // [in] the pitch to draw to.
    BOOL fFillBackground,       // [in] draw the background color?
    DWORD dwTextColor,          // [in] the foreground color bit pattern
    DWORD dwBackColor,          // [in] the background color bit pattern
    unsigned uRLEPacketWidth,   // [in] the # of bits per RLE packet
    unsigned uAntialiasWidth,   // [in] the # of bits used to store antialiasing information
    FORMAT *pFormat             // [in] describes the color format of a pixel
)
{
    HRESULT hr;

    // The pitch, in BASE units.
    unsigned uPitch = Pitch / sizeof(BASE);
    assert(!(Pitch % sizeof(BASE)));

    BASE *pSurface = (BASE *)pBits;
    const long oBaseline = uCellHeight - uDescent;

    const long lCellMinY = max((long)uClipTop, iCellY);
    const long lCellMaxY = min((long)uClipBottom, iCellY + (long)uCellHeight);

    const unsigned uPacketMask = (unsigned)0xFFFFFFFF >> (32 - uRLEPacketWidth);
    const unsigned uAntialiasMask = (unsigned)0xFFFFFFFF >> (32 - uAntialiasWidth);

    // Create a version of the text color with a maxed alpha channel.  This
    // is only used for FULLALPHA mode.
    //
    DWORD dwAlphaFull = (dwTextColor >> pFormat->uAlphaShift) & ((1 << pFormat->uAlphaBits) - 1);
    DWORD dwNoAlphaColor = dwTextColor & ~(((1 << pFormat->uAlphaBits) - 1) << pFormat->uAlphaShift);

    //_asm int 3;

    // We need to pre-draw the background for this string because the characters
    // may draw outside of their character cell.  If we drew the backgrounds with
    // the character then the background for the next character might overwrite
    // the graphics for the character we just drew.  This actually happens 
    // quite a bit with italics fonts.
    //
    // This will really blow chunks if the font is loaded with either no 
    // cache or a cache that is too small to handle the string being processed.
    // Otherwise the first background-drawing pass will just pre-load the cache
    // and no work will be wasted.
    //
    if (fFillBackground && lCellMinY < lCellMaxY)
    {
        hr = DrawBackground(pFont,
                            wstr,   
                            cch,
                            dwBackColor,
                            iCellX,
                            lCellMinY,
                            lCellMaxY,
                            uClipLeft,
                            uClipRight,
                            uIntercharacterSpacing,
                            pBits,
                            Pitch);
    }

    // _asm int 3;

    // Walk the string and draw the characters.
    for (unsigned ich = 0; ich < cch && *wstr; ich++, wstr++)
    {
        Glyph *pGlyph;
        unsigned cbGlyphSize;

        DWORD *pdwBitmap;

        // Get the character info.
        hr = XFONT_GetCharacterData((XFONT *)pFont, *wstr, &pGlyph, &cbGlyphSize);

        if (FAILED(hr)) 
        {
            return hr;
        }

        //_asm int 3;

        pdwBitmap = (DWORD *)(pGlyph + 1);      // May be unaligned...

        // 
        // Walk the character and draw its bitmap.
        //

        const unsigned uBitmapWidth = pGlyph->uBitmapWidth;

        const long lBitmapX = iCellX + pGlyph->iBearingX;
        const long lBitmapY = iCellY + oBaseline - pGlyph->iBearingY;

        const long lMinX = (unsigned)max((long)uClipLeft, lBitmapX);
        const long lMaxX = (unsigned)min((long)uClipRight, lBitmapX + (long)uBitmapWidth);

        BASE *pMinY = pSurface + max((long)uClipTop, lBitmapY) * uPitch;
        BASE *pMaxY = pSurface + min((long)uClipBottom, lBitmapY + pGlyph->uBitmapHeight) * uPitch;

        if (lMinX < lMaxX && pMinY < pMaxY)
        {
            BASE *pFirstRow = pSurface + lBitmapY * uPitch;

            unsigned iPixel = 0;
            unsigned cPixels = uBitmapWidth * pGlyph->uBitmapHeight;
            unsigned cBits = 0;
            unsigned cToDraw;

            unsigned uBitmap = *pdwBitmap;
            unsigned uValue;

            bool fForeground = false;
            DWORD dwPartial;

            while (iPixel < cPixels)
            {
                dwPartial = 0;

                cToDraw = DecodeWidth(&pdwBitmap,
                                      &uBitmap,
                                      &cBits,
                                      uPacketMask,
                                      uRLEPacketWidth,
                                      true);

                // Handle escaped values.
                if (cToDraw == 0 && uAntialiasWidth > 0)
                {
                    // Get the value of the escaped value.
                    dwPartial = DecodeWidth(&pdwBitmap,
                                            &uBitmap,
                                            &cBits,
                                            uAntialiasMask,
                                            uAntialiasWidth,
                                            false);

                    // A zero here tells us to draw nothing, otherwise we're
                    // just drawing one escaped pixel.
                    //
                    if (dwPartial)
                    {
                        cToDraw = 1;
                    }
                }

                if (dwPartial || fForeground && cToDraw)
                {
                    BASE *pRow = pFirstRow + uPitch * (iPixel / uBitmapWidth);
                    unsigned uPixelInBitmapX = iPixel % uBitmapWidth;
                    unsigned cDrawing = cToDraw;

                    for (;;)
                    {
                        const unsigned cMove = min(cDrawing, uBitmapWidth - uPixelInBitmapX);
                        const long lPixelX = (long)uPixelInBitmapX + lBitmapX;
                        const long lPixelMin = max(lPixelX, lMinX);
                        const long lPixelMax = min(lPixelX + (long)cMove, lMaxX);

                        if (lPixelMin < lPixelMax && pRow >= pMinY && pRow <= pMaxY)
                        {
                            long cCols = lPixelMax - lPixelMin;
                            BASE *pPixel = pRow + lPixelMin;

                            if (!dwPartial)
                            {
                                while (cCols--)
                                {
                                    *(pPixel++) = (BASE)dwTextColor;
                                }
                            }
                            else
                            {
                                // Map the partial value to its final alpha value.
                                const DWORD dwAlpha = (dwPartial * dwAlphaFull) / (1 << uAntialiasWidth);
                                const DWORD dwTextColorPartial = dwNoAlphaColor | (dwAlpha << pFormat->uAlphaShift);

                                while (cCols--)
                                {
                                    *(pPixel++) = (BASE)dwTextColorPartial;
                                }
                            }
                        }

                        cDrawing -= cMove;

                        if (!cDrawing)
                        {
                            break;
                        }

                        pRow += uPitch;
                        uPixelInBitmapX = 0;
                    }
                }

                if (!dwPartial)
                {
                    fForeground = !fForeground;
                }

                iPixel += cToDraw;                
            }
        }

        // Next character.
        iCellX += pGlyph->uAdvance + uIntercharacterSpacing;
    }

    return NOERROR;
}
};

//============================================================================
// Given a surface format, return the number of alpha bits it supports.
//============================================================================

// Maps from a FORMAT to an entry in the following table.
_declspec(selectany) BYTE g_FormatOffset[] =
{
    -1, //D3DFMT_L8
    -1, //D3DFMT_AL8
    0,  //D3DFMT_A1R5G5B5
    1,  //D3DFMT_X1R5G5B5
    2,  //D3DFMT_A4R4G4B4
    3,  //D3DFMT_R5G6B5
    4,  //D3DFMT_A8R8G8B8
    5,  //D3DFMT_X8R8G8B8
    -1,
    -1,
    -1,
    6,  //D3DFMT_P8
    -1, //D3DFMT_DXT1
    -1,
    -1, //D3DFMT_DXT3
    -1, //D3DFMT_DXT5
    0,  //D3DFMT_LIN_A1R5G5B5
    3,  //D3DFMT_LIN_R5G6B5
    4,  //D3DFMT_LIN_A8R8G8B8
    -1, //D3DFMT_LIN_L8
    -1,
    -1,
    7,  //D3DFMT_LIN_R8B8
    8,  //D3DFMT_LIN_G8B8
    -1,
    9,  //D3DFMT_A8
    -1, //D3DFMT_A8L8
    -1, //D3DFMT_LIN_AL8
    1,  //D3DFMT_LIN_X1R5G5B5
    2,  //D3DFMT_LIN_A4R4G4B4
    5,  //D3DFMT_LIN_X8R8G8B8
    9,  //D3DFMT_LIN_A8
    -1, //D3DFMT_LIN_A8L8
    -1,
    -1,
    -1,
    -1, //D3DFMT_YUY2
    -1, //D3DFMT_UYVY
    -1,
    10, //D3DFMT_R6G5B5
    8,  //D3DFMT_G8B8
    7,  //D3DFMT_R8B8
    -1, //D3DFMT_D24S8
    -1, //D3DFMT_F24S8
    -1, //D3DFMT_D16
    -1, //D3DFMT_F16
    -1, //D3DFMT_LIN_D24S8
    -1, //D3DFMT_LIN_F24S8
    -1, //D3DFMT_LIN_D16
    -1, //D3DFMT_LIN_F16
    -1, //D3DFMT_L16
    -1, //D3DFMT_V16U16
    -1,
    -1, //D3DFMT_LIN_L16
    -1, //D3DFMT_LIN_V16U16
    10, //D3DFMT_LIN_R6G5B5
    11, //D3DFMT_R5G5B5A1
    12, //D3DFMT_R4G4B4A4
    13, //D3DFMT_A8B8G8R8
    14, //D3DFMT_B8G8R8A8
    15, //D3DFMT_R8G8B8A8
    11,  //D3DFMT_LIN_R5G5B5A1
    12,  //D3DFMT_LIN_R4G4B4A4
    13, //D3DFMT_LIN_A8B8G8R8
    14, //D3DFMT_LIN_B8G8R8A8
    15, //D3DFMT_LIN_R8G8B8A8
};

_declspec(selectany) FORMAT g_Formats[] =
{
    // format               bpp  abits ashift rbits rshift gbits gshift bbits bshift
    { /*  0 - A1R5G5B5 */   16,  1,    15,    5,    10,    5,    5,     5,    0  },
    { /*  1 - X1R5G5B5 */   16,  0,    0,     5,    10,    5,    5,     5,    0  },
    { /*  2 - A4R4G4B4 */   16,  4,    12,    4,    8,     4,    4,     4,    0  },
    { /*  3 - R5G6B5   */   16,  0,    0,     5,    11,    6,    5,     5,    0  },
    { /*  4 - A8R8G8B8 */   32,  8,    24,    8,    16,    8,    8,     8,    0  },
    { /*  5 - X8R8G8B8 */   32,  0,    0,     8,    16,    8,    8,     8,    0  },
    { /*  6 - P8       */   8,   0,    0,     0,    0,     0,    0,     0,    0  },
    { /*  7 - R8B8     */   16,  0,    0,     8,    8,     0,    0,     8,    0  },
    { /*  8 - G8B8     */   16,  0,    0,     0,    0,     8,    8,     8,    0  },
    { /*  9 - A8       */   8,   8,    0,     0,    0,     0,    0,     0,    0  },
    { /* 10 - R6G5B5   */   16,  0,    0,     6,    10,    5,    5,     5,    0  },
    { /* 11 - R5G5B5A1 */   16,  1,    0,     5,    11,    5,    6,     5,    1  },
    { /* 12 - R4G4B4A4 */   16,  4,    0,     4,    12,    4,    8,     4,    4  },
    { /* 13 - A8B8G8R8 */   32,  8,    24,    8,    0,     8,    8,     8,    16 },
    { /* 14 - B8G8R8A8 */   32,  8,    0,     8,    8,     8,    16,    8,    24 },
    { /* 15 - R8G8B8A8 */   32,  8,    0,     8,    24,    8,    16,    8,    8  },
};


static FORMAT *PixelLayoutFromFormat
(
    D3DFORMAT Format
)
{
    BYTE Index = g_FormatOffset[Format];

    XFASSERT(Index != -1, "XFONT - Unrecognized destination surface format.");

    return &g_Formats[Index];
}

//============================================================================
// Given a surface format and a color, return the pixel value for the color.
//============================================================================

static DWORD PixelColorFromD3DCOLOR
(
    FORMAT *pFormat,
    D3DCOLOR color
)
{
    DWORD dwColor;

    DWORD dwAlpha = (color >> 24) & 0xFF;
    DWORD dwRed = (color >> 16) & 0xFF;
    DWORD dwGreen = (color >> 8) & 0xFF;
    DWORD dwBlue = color & 0xFF;

    dwColor  = (dwAlpha >> (8 - pFormat->uAlphaBits)) << pFormat->uAlphaShift;
    dwColor |= (dwRed   >> (8 - pFormat->uRedBits)  ) << pFormat->uRedShift;
    dwColor |= (dwGreen >> (8 - pFormat->uGreenBits)) << pFormat->uGreenShift;
    dwColor |= (dwBlue  >> (8 - pFormat->uBlueBits) ) << pFormat->uBlueShift;

    return dwColor;
};

//============================================================================
// The whole purpose of this file, render a single character on the display.
//============================================================================

HRESULT __stdcall PaintText
(
    Font *pFont,                // [in] Description of the font to display
    LPCVOID pBits,              // [in] The memory to write to.
    unsigned Pitch,             // [in] The pitch of the surface.
    D3DFORMAT Format,           // [in] The format of the surface
    LPCWSTR wstr,               // [in] String to render.
    unsigned cch,               // [in] Length, or -1 for zero termination
    long iX,                    // [in] X coordinate of the left of the character cell
    long iY,                    // [in] Y coordinate of the top of the character cell
    unsigned uClipLeft,         // [in] Clipping rectangle
    unsigned uClipTop,
    unsigned uClipRight,
    unsigned uClipBottom
)
{
    HRESULT hr;

    FORMAT *pFormat;

    pFormat = PixelLayoutFromFormat(Format);

    DWORD dwTextColor = PixelColorFromD3DCOLOR(pFormat, pFont->TextColor);
    DWORD dwBackColor = PixelColorFromD3DCOLOR(pFormat, pFont->BackgroundColor);

    // Call the draw method.  Each call below will cause a new instance of the
    // method to be instantiated with a different base type and for a different
    // mode.  Removing elements from this table that you do not use will reduce
    // the code size of this library.
    //
    typedef HRESULT (*pfnTextOut)(Font *pFont,
                                  LPCWSTR wstr,
                                  unsigned cch,
                                  long iCellX,
                                  long iCellY,
                                  unsigned uClipLeft,
                                  unsigned uClipTop,
                                  unsigned uClipRight,
                                  unsigned uClipBottom,
                                  unsigned uIntercharacterSpacing,
                                  unsigned uCellHeight,
                                  unsigned uDescent,
                                  LPCVOID pBits,
                                  unsigned Pitch,
                                  BOOL fFillBackground,
                                  DWORD dwTextColor,
                                  DWORD dwBackColor,
                                  unsigned uRLEPacketWidth,
                                  unsigned uAntialiasWidth,
                                  FORMAT *pFormat);

    // Removing unused entries from this table will reduce the code
    // size of this file as it will cause fewer instances of the 
    // TextOut* methods to be instantiated.
    //
    const static pfnTextOut rgTextOutTable[4][2] =
    {
        { Paint<BYTE> ::TextOutNoAlpha, Paint<BYTE> ::TextOutAlpha },
        { Paint<WORD> ::TextOutNoAlpha, Paint<WORD> ::TextOutAlpha },
        { NULL,                         NULL,                      },
        { Paint<DWORD>::TextOutNoAlpha, Paint<DWORD>::TextOutAlpha },
    };

    unsigned iSize = pFormat->uPixelSize / 8 - 1;

    // Do the draw.
    hr = rgTextOutTable[iSize][pFont->uAntialiasLevel == 0 ? 0 : 1]
            (
                pFont,
                wstr,
                cch,
                iX,
                iY,
                uClipLeft,
                uClipTop,
                uClipRight,
                uClipBottom,
                pFont->uIntercharacterSpacing,
                pFont->uCellHeight,
                pFont->uDescent,
                pBits,
                Pitch,
                pFont->fPaintBackground,
                dwTextColor,
                dwBackColor,
                pFont->uRLEWidth,
                pFont->uAntialiasLevel,
                pFormat
            );

    return hr;
}
