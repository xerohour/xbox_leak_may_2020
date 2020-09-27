/*
 * Copyright 1993-1998 NVIDIA, Corporation.  All rights reserved.
 *
 * THE INFORMATION CONTAINED HEREIN IS PROPRIETARY AND CONFIDENTIAL TO
 * NVIDIA, CORPORATION.  USE, REPRODUCTION OR DISCLOSURE TO ANY THIRD PARTY
 * IS SUBJECT TO WRITTEN PRE-APPROVAL BY NVIDIA, CORPORATION.
 */
/********************************* Direct 3D *******************************\
*                                                                           *
* Module: nvMip.cpp                                                         *
*   Mipmapping routines.                                                    *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       Ben de Waal                 04/20/98 - wrote it                     *
*                                                                           *
\***************************************************************************/
#include "nvprecomp.h"

#if (NVARCH >= 0x04)

static int nvMipMapTable[64] = {
// make, copy idx for 1, copy idx for 0
    0, 0, 2, -1, // 0 = 00 00  ....
    0, 0, 1, -1, // 1 = 10 00  0...
    0, 1, 0, -1, // 2 = 01 00  .0..
    1, 0, 2, -1, // 3 = 11 00  1...   bottom
    0, 2, 3, -1, // 4 = 00 10  ..0.
    0, 2, 1, -1, // 5 = 10 10  ..1.   left
    1, 1, 3, -1, // 6 = 01 10  .1..   equal - look at error
    1, 0, 3, -1, // 7 = 11 10  2...
    0, 3, 0, -1, // 8 = 00 01  ...0
    1, 3, 1, -1, // 9 = 10 01  ...1   equal - look at error
    1, 1, 2, -1, // a = 01 01  .2..   right
    1, 3, 2, -1, // b = 11 01  ...2
    0, 2, 0, -1, // c = 00 11  ..2.   top
    1, 0, 1, -1, // d = 10 11  3...
    1, 1, 0, -1, // e = 01 11  .3..
    1, 2, 0, -1, // f = 11 11  ..3.
};

extern BYTE nvTexelScratchBuffer[8192];
DWORD nvTextureCalcMipMapSize (DWORD dwLogU,DWORD dwLogV,DWORD dwMipMapLevels);
DWORD _key;

typedef void (*NVCOMBINETEXELFPTR)(DWORD);

/****************************************************************************/
/* 16 bpp                                                                   */
/****************************************************************************/

/*
 * x1r5g5b5 no color key
 */
void nvCombineTexelsX1R5G5B5
(
    DWORD count
)
{
    DWORD src = (DWORD)nvTexelScratchBuffer;
    DWORD dst = (DWORD)nvTexelScratchBuffer;

    while (count)
    {
        DWORD REDBLU = 0x7c1f;
        DWORD GRN    = 0x03e0;
        DWORD a,b,c,d;
        DWORD rb,ag;

        a = *(DWORD*)(src+0);
        b = a >> 16;
        c = *(DWORD*)(src+4);
        d = c >> 16;

        rb = (((a & REDBLU) + (b & REDBLU) + (c & REDBLU) + (d & REDBLU)) >> 2) & REDBLU;
        ag = (((a & GRN   ) + (b & GRN   ) + (c & GRN   ) + (d & GRN   )) >> 2) & GRN   ;

        *(WORD*)dst = (WORD)(rb|ag);

        src   += 8;
        dst   += 2;
        count -= 4;
    }
}

/*
 * a1r5g5b5 no color key
 */
void nvCombineTexelsA1R5G5B5
(
    DWORD count
)
{
    DWORD src = (DWORD)nvTexelScratchBuffer;
    DWORD dst = (DWORD)nvTexelScratchBuffer;

    while (count)
    {
        DWORD t00 = *(WORD*)(src+0);
        DWORD t01 = *(WORD*)(src+2);
        DWORD t10 = *(WORD*)(src+4);
        DWORD t11 = *(WORD*)(src+6);

        DWORD alp = ((t00 & 0x8000) >> 13)
                  | ((t01 & 0x8000) >> 12)
                  | ((t10 & 0x8000) >> 11)
                  | ((t11 & 0x8000) >> 10);

        *(WORD*)dst = *(WORD*)(src+(nvMipMapTable[alp + 1 + nvMipMapTable[alp]] << 1));

        src   += 8;
        dst   += 2;
        count -= 4;
    }
}

/*
 * a4r4g4b4 no color key
 */
void nvCombineTexelsA4R4G4B4
(
    DWORD count
)
{
    DWORD src = (DWORD)nvTexelScratchBuffer;
    DWORD dst = (DWORD)nvTexelScratchBuffer;

    while (count)
    {
        DWORD REDBLU = 0x0f0f;
        DWORD ALPGRN = 0xf0f0;
        DWORD a,b,c,d;
        DWORD rb,ag;

        a = *(DWORD*)(src+0);
        b = a >> 16;
        c = *(DWORD*)(src+4);
        d = c >> 16;

        rb = (((a & REDBLU) + (b & REDBLU) + (c & REDBLU) + (d & REDBLU)) >> 2) & REDBLU;
        ag = (((a & ALPGRN) + (b & ALPGRN) + (c & ALPGRN) + (d & ALPGRN)) >> 2) & ALPGRN;

        *(WORD*)dst = (WORD)(rb|ag);

        src   += 8;
        dst   += 2;
        count -= 4;
    }
}

/*
 * r5g6b5 no color key
 */
void nvCombineTexelsR5G6B5
(
    DWORD count
)
{
    DWORD src = (DWORD)nvTexelScratchBuffer;
    DWORD dst = (DWORD)nvTexelScratchBuffer;

    while (count)
    {
        DWORD REDBLU = 0xf81f;
        DWORD GRN    = 0x07e0;
        DWORD a,b,c,d;
        DWORD rb,ag;

        a = *(DWORD*)(src+0);
        b = a >> 16;
        c = *(DWORD*)(src+4);
        d = c >> 16;

        rb = (((a & REDBLU) + (b & REDBLU) + (c & REDBLU) + (d & REDBLU)) >> 2) & REDBLU;
        ag = (((a & GRN   ) + (b & GRN   ) + (c & GRN   ) + (d & GRN   )) >> 2) & GRN   ;

        *(WORD*)dst = (WORD)(rb|ag);

        src   += 8;
        dst   += 2;
        count -= 4;
    }
}

/*
 * y16 no color key
 */
void nvCombineTexelsY16CK
(
    DWORD count
)
{
    DWORD src = (DWORD)nvTexelScratchBuffer;
    DWORD dst = (DWORD)nvTexelScratchBuffer;

    while (count)
    {
        DWORD t00 = *(WORD*)(src+0);
        DWORD t01 = *(WORD*)(src+2);
        DWORD t10 = *(WORD*)(src+4);
        DWORD t11 = *(WORD*)(src+6);

        DWORD alp = ((t00 != _key) ?  4 : 0)
                  | ((t01 != _key) ?  8 : 0)
                  | ((t10 != _key) ? 16 : 0)
                  | ((t11 != _key) ? 32 : 0);

        if (nvMipMapTable[alp])
        {
            *(WORD*)dst = *(WORD*)(src+(nvMipMapTable[alp+1] << 1));
        }
        else
        {
            *(WORD*)dst = (WORD)_key;
        }

        src   += 8;
        dst   += 2;
        count -= 4;
    }
}

/****************************************************************************/
/* 32 bpp                                                                   */
/****************************************************************************/

/*
 * x8r8g8b8 no color key
 */
void nvCombineTexelsX8R8G8B8
(
    DWORD count
)
{
    DWORD src = (DWORD)nvTexelScratchBuffer;
    DWORD dst = (DWORD)nvTexelScratchBuffer;

    while (count)
    {
        DWORD REDBLU = 0x00ff00ff;
        DWORD GRN    = 0x0000ff00;
        DWORD a,b,c,d;
        DWORD rb,ag;

        a = *(DWORD*)(src+0);
        b = *(DWORD*)(src+4);
        c = *(DWORD*)(src+8);
        d = *(DWORD*)(src+12);

        rb = (((a & REDBLU) + (b & REDBLU) + (c & REDBLU) + (d & REDBLU)) >> 2) & REDBLU;
        ag = (((a & GRN   ) + (b & GRN   ) + (c & GRN   ) + (d & GRN   )) >> 2) & GRN   ;

        *(DWORD*)dst = rb|ag;

        src   += 16;
        dst   += 4;
        count -= 4;
    }
}

/*
 * a8r8g8b8 no color key
 */
void nvCombineTexelsA8R8G8B8
(
    DWORD count
)
{
    DWORD src = (DWORD)nvTexelScratchBuffer;
    DWORD dst = (DWORD)nvTexelScratchBuffer;

    while (count)
    {
        DWORD REDBLU = 0x00ff00ff;
        DWORD GRN    = 0x0000ff00;
        DWORD ALP    = 0xff000000;
        DWORD a,b,c,d;
        DWORD A,B,C,D;
        DWORD rb,ag;

        a = *(DWORD*)(src+0);  A = a & ALP;
        b = *(DWORD*)(src+4);  B = b & ALP;
        c = *(DWORD*)(src+8);  C = c & ALP;
        d = *(DWORD*)(src+12); D = d & ALP;

        rb = (((a & REDBLU) + (b & REDBLU) + (c & REDBLU) + (d & REDBLU)) >> 2) & REDBLU;
        ag = (((a & GRN   ) + (b & GRN   ) + (c & GRN   ) + (d & GRN   )) >> 2) & GRN   ;

        A = (A>>24) + (B>>24) + (C>>24) + (D>>24);
        A = (A >> 2) << 24;

        *(DWORD*)dst = rb|ag|A;

        src   += 16;
        dst   += 4;
        count -= 4;
    }
}

/*
 * y32 no color key
 */
void nvCombineTexelsY32CK
(
    DWORD count
)
{
    DWORD src = (DWORD)nvTexelScratchBuffer;
    DWORD dst = (DWORD)nvTexelScratchBuffer;

    while (count)
    {
        DWORD t00 = *(DWORD*)(src+ 0);
        DWORD t01 = *(DWORD*)(src+ 4);
        DWORD t10 = *(DWORD*)(src+ 8);
        DWORD t11 = *(DWORD*)(src+12);

        DWORD alp = ((t00 != _key) ?  4 : 0)
                  | ((t01 != _key) ?  8 : 0)
                  | ((t10 != _key) ? 16 : 0)
                  | ((t11 != _key) ? 32 : 0);

        if (nvMipMapTable[alp])
        {
            *(DWORD*)dst = *(DWORD*)(src+(nvMipMapTable[alp+1] << 2));
        }
        else
        {
            *(DWORD*)dst = _key;
        }

        src   += 16;
        dst   += 4;
        count -= 4;
    }
}

// BUGBUG no automipmaps. do we care?
#if 0

/*
 * nvTextureAutoMipMap
 *
 * automipmap given texture
 */
void nvTextureAutoMipMap
(
    CTexture *pTexture
)
{
    NVCOMBINETEXELFPTR fncCombineTexels;

    /*
     * sanity checks
     */
    nvAssert (!(pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_CELSIUS) &&
              !(pDriverData->nvD3DPerfData.dwNVClasses & NVCLASS_FAMILY_KELVIN));
    nvAssert (pTexture);

    /*
     * get proper reduction function
     */
    {
        LPDDRAWI_DDRAWSURFACE_LCL lpLcl;

        lpLcl = pTexture->getDDSurfaceLcl();

        if (lpLcl->dwFlags & DDRAWISURF_HASCKEYSRCBLT) // have colorkey and it matters
        {
            switch (pTexture->getFormat())
            {
                case NV_SURFACE_FORMAT_X1R5G5B5:
                case NV_SURFACE_FORMAT_R5G6B5:
                case NV_SURFACE_FORMAT_A1R5G5B5:
                case NV_SURFACE_FORMAT_A4R4G4B4:
                    fncCombineTexels = nvCombineTexelsY16CK;
                    _key  = lpLcl->ddckCKSrcBlt.dwColorSpaceHighValue & 0x00007FFF;
                    break;
                case NV_SURFACE_FORMAT_X8R8G8B8:
                case NV_SURFACE_FORMAT_A8R8G8B8:
                    fncCombineTexels = nvCombineTexelsY32CK;
                    _key  = lpLcl->ddckCKSrcBlt.dwColorSpaceHighValue;
                    break;
            }
        }
        else
        {
            switch (pTexture->getFormat())
            {
                case NV_SURFACE_FORMAT_X1R5G5B5: fncCombineTexels = nvCombineTexelsX1R5G5B5;
                                                 break;
                case NV_SURFACE_FORMAT_R5G6B5:   fncCombineTexels = nvCombineTexelsR5G6B5;
                                                 break;
                case NV_SURFACE_FORMAT_A1R5G5B5: fncCombineTexels = nvCombineTexelsA1R5G5B5;
                                                 break;
                case NV_SURFACE_FORMAT_A4R4G4B4: fncCombineTexels = nvCombineTexelsA4R4G4B4;
                                                 break;
                case NV_SURFACE_FORMAT_X8R8G8B8: fncCombineTexels = nvCombineTexelsX8R8G8B8;
                                                 break;
                case NV_SURFACE_FORMAT_A8R8G8B8: fncCombineTexels = nvCombineTexelsA8R8G8B8;
                                                 break;
            }
        }
    }

    /*
     * sync
     */
	DDLOCKINDEX(NVSTAT_LOCK_AUTO_MIP);
    pTexture->cpuLockSwz (CSimpleSurface::LOCK_NORMAL);

    {
        int   i,s,c;
        DWORD dwAddr[12];
        DWORD dwOffset[12];
        DWORD dwCount[12];
        DWORD dwMipMapLevel;

        DWORD dwLogU         = pTexture->getLogWidth();
        DWORD dwLogV         = pTexture->getLogHeight();
        DWORD dwMipMapLevels = pTexture->getMipMapCount();
        DWORD dwBPP          = (pTexture->getBPP() == 4) ? 2 : 1; // shift amount

        /*
         * setup mipmap base tables
         */
        s = pTexture->getSwizzled()->getAddress();
        c  = 1 << (dwLogU + dwLogV);
        for (i = dwMipMapLevels-1; i >= 0; i--)
        {
            dwAddr[i]   = s;
            dwOffset[i] = 0;
            dwCount[i]  = c;

            s  += c << dwBPP;
            c >>= 2;
        }

        /*
         * for all pending work
         */
        for (dwMipMapLevel = dwMipMapLevels - 1; dwMipMapLevel; )
        {
            /*
             * setup
             */
            DWORD dwMip  = dwMipMapLevel;
            DWORD dwToGo = min(dwCount[dwMip],1024);

            /*
             * read a line
             */
            nvMemCopy ((DWORD)nvTexelScratchBuffer,
                       dwAddr[dwMip] + (dwOffset[dwMip] << dwBPP),
                       dwToGo << dwBPP);

            /*
             * for as many mipmaps we can possibly do
             */
            for (; dwMip && (dwToGo >= 4); )
            {
                /*
                 * mipmap cached block of data
                 */
                (fncCombineTexels)(dwToGo);

                /*
                 * write dest
                 */
                nvMemCopy (dwAddr[dwMip-1] + ((dwOffset[dwMip] >> 2) << dwBPP),
                           (DWORD)nvTexelScratchBuffer,
                           (dwToGo >> 2) << dwBPP);

                /*
                 * advance to next mip level
                 */
                dwCount[dwMip]  -= dwToGo;
                dwOffset[dwMip] += dwToGo;
                dwToGo >>= 2;
                dwMip--;
            }

            /*
             * advance to next level
             */
            while (dwMipMapLevel && !dwCount[dwMipMapLevel])  dwMipMapLevel--;
        }
    }

    pTexture->cpuUnlockSwz();
}

#endif  // 0

#endif  // NVARCH >= 0x04

