/*============================================================================
 *
 *  Copyright (C) Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       backend.c
 *  Content:    converts our YUV format into a bitmap.
 *
 ****************************************************************************/

#include <xtl.h>
#include <xdbg.h>
#include <xmv.h>

#include "decoder.h"

/*
 * Convert our internal YUV format into a standard YUY2 buffer.
 */

static
void RenderToYUY2
(
    DWORD MBWidth, 
    DWORD MBHeight, 
    BYTE *pY, 
    BYTE *pU, 
    BYTE *pV,
    BYTE *pDestination, 
    DWORD DestinationPitch
)
{
    DWORD iWidthY;
    DWORD iWidthUV;
    DWORD PitchAdjust;

    DWORD x;

    // Make sure that our parameters are all 8-byte aligned.
    ASSERT(((DWORD)pY) % 8 == 0); 
    ASSERT(((DWORD)pU) % 8 == 0); 
    ASSERT(((DWORD)pV) % 8 == 0); 
    ASSERT(((DWORD)pDestination) % 8 == 0); 
    ASSERT(DestinationPitch % 8 == 0);

    iWidthY  = MACROBLOCK_SIZE * MBWidth;
    iWidthUV = BLOCK_SIZE * MBWidth;

    PitchAdjust = DestinationPitch - MBWidth * MACROBLOCK_SIZE * 2;

    while(MBHeight--)
    {
        x = MBWidth;

        while (x--)
        {
            __asm
            {
                mov         esi, pY
                mov         edi, pDestination
                mov         ecx, pU
                mov         edx, pV
                mov         eax, BLOCK_SIZE

            L1: movq        mm2, [ecx]          ; 8 U values
                movq        mm3, [edx]          ; 8 V values
                pxor        mm4, mm4
                pxor        mm6, mm6
                pxor        mm0, mm0
                pxor        mm1, mm1
                punpcklbw   mm4, mm2
                punpckhbw   mm6, mm2
                movq        mm5, mm4
                movq        mm7, mm6
                punpckhwd   mm5, mm0            ; spread Us into 4 MMX registers
                punpckhwd   mm7, mm0            ;   ..u...u...u...u...u...u...u...u.
                punpcklwd   mm4, mm0
                punpcklwd   mm6, mm0

                pxor        mm2, mm2
                add         ecx, iWidthUV
                add         edx, iWidthUV

                punpcklbw   mm0, mm3            ; spread Vs and then OR them into Us 
                punpcklwd   mm1, mm0            ;   v.u.v.u.v.u.v.u.v.u.v.u.v.u.v.u.
                punpckhwd   mm2, mm0
                por         mm4, mm1
                por         mm5, mm2

                movq        mm0, [esi]          ; 16 Y values from the current line
                movq        mm1, [esi+8]

                pxor        mm2, mm2
                punpckhbw   mm2, mm3
                pxor        mm3, mm3
                punpcklwd   mm3, mm2
                por         mm6, mm3
                pxor        mm3, mm3
                punpckhwd   mm3, mm2
                por         mm7, mm3

                add         esi, iWidthY        ; spread Ys and output the final results
                pxor        mm2, mm2            ;   vyuyvyuyvyuyvyuyvyuyvyuyvyuyvyuy
                movq        mm3, mm0
                punpcklbw   mm0, mm2
                por         mm0, mm4
                movq        [edi], mm0          ; notice that we output 32 bytes (4 qwords)
                punpckhbw   mm3, mm2            ; without any intervening memory access
                por         mm3, mm5            ; to achieve maximum memory write perf
                movq        [edi+8], mm3
                movq        mm3, mm1
                punpcklbw   mm1, mm2
                por         mm1, mm6
                movq        [edi+16], mm1
                punpckhbw   mm3, mm2
                por         mm3, mm7
                movq        [edi+24], mm3

                movq        mm0, [esi]          ; 16 Y values from the next line
                movq        mm1, [esi+8]
                add         edi, DestinationPitch
                pxor        mm2, mm2
                movq        mm3, mm0
                punpcklbw   mm0, mm2
                por         mm0, mm4
                movq        [edi], mm0
                punpckhbw   mm3, mm2
                por         mm3, mm5
                movq        [edi+8], mm3
                movq        mm3, mm1
                punpcklbw   mm1, mm2
                por         mm1, mm6
                movq        [edi+16], mm1
                punpckhbw   mm3, mm2
                por         mm3, mm7
                movq        [edi+24], mm3

                add         esi, iWidthY
                add         edi, DestinationPitch
                dec         eax
                jnz         L1
            }

            pY += MACROBLOCK_SIZE;
            pU += BLOCK_SIZE;
            pV += BLOCK_SIZE;

            pDestination += MACROBLOCK_SIZE * 2;
        }

        pY += (MACROBLOCK_SIZE - 1) * iWidthY;
        pU += (BLOCK_SIZE - 1) * iWidthUV;
        pV += (BLOCK_SIZE - 1) * iWidthUV;

        pDestination += PitchAdjust + DestinationPitch * (MACROBLOCK_SIZE - 1);
    }

    __asm emms;
}

/*
 * Converts the current YUV buffer into the format we want to display.
 */

void RenderBitmap
(
    XMVDecoder *pDecoder,
    D3DSurface *pSurface
)
{
    D3DLOCKED_RECT Rect;
    D3DSURFACE_DESC Desc;

    D3DSurface_GetDesc(pSurface, &Desc);
    D3DSurface_LockRect(pSurface, &Rect, 0, D3DLOCK_TILED);

#if DBG

    if (Desc.Width != pDecoder->Width || Desc.Height != pDecoder->Height)
    {
        RIP("The target surface must have exactly the same size as the decoded video.");
    }

#endif DBG

    switch(Desc.Format)
    {
    case D3DFMT_YUY2:

        RenderToYUY2(pDecoder->MBWidth, 
                     pDecoder->MBHeight, 
                     pDecoder->pYDisplayed, 
                     pDecoder->pUDisplayed, 
                     pDecoder->pVDisplayed,
                     Rect.pBits, 
                     Rect.Pitch);

        break;

    default:
        RIP("Unsupported target surface format, only YUY2 is supported at this time.");
        break;
    }

#if DBG && 0

    // Draw a grid on top of the rendered frame so that we can figure out what
    // macroblocks contain the drawing errors.
    //
    {
        WORD *pPixel;
        DWORD x, y;

        for (y = 0; y < Desc.Height; y++)
        {
            pPixel = (WORD *)((BYTE *)Rect.pBits + y * Rect.Pitch);

            // Double the middle line.
            if (y % 16 == 0 || y == Desc.Height / 2 + 1)
            {
                for (x = 0; x < Desc.Width; x++)
                {
                    *pPixel = (*pPixel & 0xFF00) | 0x40;

                    pPixel++;
                }
            }
            else
            {
                for (x = 0; x < Desc.Width; x++)
                {
                    // Double the middle line.
                    if (x % 16 == 0 || x == Desc.Width / 2 + 1)
                    {
                        *pPixel = (*pPixel & 0xFF00) | 0x40;
                    }

                    pPixel++;
                }
            }
        }
    }

#endif DBG

    D3DSurface_UnlockRect(pSurface);
}

