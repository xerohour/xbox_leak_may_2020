 /***************************************************************************\
|*                                                                           *|
|*       Copyright 1993-1999 NVIDIA, Corporation.  All rights reserved.      *|
|*                                                                           *|
|*     NOTICE TO USER:   The source code  is copyrighted under  U.S. and     *|
|*     international laws.  Users and possessors of this source code are     *|
|*     hereby granted a nonexclusive,  royalty-free copyright license to     *|
|*     use this code in individual and commercial software.                  *|
|*                                                                           *|
|*     Any use of this source code must include,  in the user documenta-     *|
|*     tion and  internal comments to the code,  notices to the end user     *|
|*     as follows:                                                           *|
|*                                                                           *|
|*       Copyright 1993-1999 NVIDIA, Corporation.  All rights reserved.      *|
|*                                                                           *|
|*     NVIDIA, CORPORATION MAKES NO REPRESENTATION ABOUT THE SUITABILITY     *|
|*     OF  THIS SOURCE  CODE  FOR ANY PURPOSE.  IT IS  PROVIDED  "AS IS"     *|
|*     WITHOUT EXPRESS OR IMPLIED WARRANTY OF ANY KIND.  NVIDIA, CORPOR-     *|
|*     ATION DISCLAIMS ALL WARRANTIES  WITH REGARD  TO THIS SOURCE CODE,     *|
|*     INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY, NONINFRINGE-     *|
|*     MENT,  AND FITNESS  FOR A PARTICULAR PURPOSE.   IN NO EVENT SHALL     *|
|*     NVIDIA, CORPORATION  BE LIABLE FOR ANY SPECIAL,  INDIRECT,  INCI-     *|
|*     DENTAL, OR CONSEQUENTIAL DAMAGES,  OR ANY DAMAGES  WHATSOEVER RE-     *|
|*     SULTING FROM LOSS OF USE,  DATA OR PROFITS,  WHETHER IN AN ACTION     *|
|*     OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,  ARISING OUT OF     *|
|*     OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOURCE CODE.     *|
|*                                                                           *|
|*     U.S. Government  End  Users.   This source code  is a "commercial     *|
|*     item,"  as that  term is  defined at  48 C.F.R. 2.101 (OCT 1995),     *|
|*     consisting  of "commercial  computer  software"  and  "commercial     *|
|*     computer  software  documentation,"  as such  terms  are  used in     *|
|*     48 C.F.R. 12.212 (SEPT 1995)  and is provided to the U.S. Govern-     *|
|*     ment only as  a commercial end item.   Consistent with  48 C.F.R.     *|
|*     12.212 and  48 C.F.R. 227.7202-1 through  227.7202-4 (JUNE 1995),     *|
|*     all U.S. Government End Users  acquire the source code  with only     *|
|*     those rights set forth herein.                                        *|
|*                                                                           *|
 \***************************************************************************/
#ifndef _nvSwiz_h
#define _nvSwiz_h

#ifdef __cplusplus
extern "C"
{
#else
#error C++ Compiler required
#endif

//
// forward declarations
//
class CTexture;

/*
 * exports
 */

// cpu or gpu swizzle routines
void nvSwizBlt_lin2swz (PNVD3DCONTEXT pContext, 
                        DWORD dwSrcAddr, DWORD dwSrcPitch, DWORD dwSrcSlicePitch,
                        DWORD dwSrcX,DWORD dwSrcY,DWORD dwSrcZ,
                        CTexture *pDstTexture, DWORD dwDstAddr,
                        DWORD dwDstX, DWORD dwDstY, DWORD dwDstZ,
                        DWORD dwDstLogWidth, DWORD dwDstLogHeight, DWORD dwDstLogDepth,
                        DWORD dwWidth, DWORD dwHeight, DWORD dwDepth,
                        DWORD dwBPP);

// cpu swizzle routines
void nvSwizBlt_cpu_lin2lin (DWORD dwSrcAddr,DWORD dwSrcPitch,DWORD dwSrcX,DWORD dwSrcY,
                            DWORD dwDstAddr,DWORD dwDstPitch,DWORD dwDstX,DWORD dwDstY,
                            DWORD dwWidth,DWORD dwHeight,DWORD dwBPP);

void nvSwizBlt_cpu_lin2swz (DWORD dwSrcAddr,DWORD dwSrcPitch,DWORD dwSrcX,DWORD dwSrcY,
                            DWORD dwDstAddr,DWORD dwDstX,DWORD dwDstY,DWORD dwDstLogWidth,DWORD dwDstLogHeight,
                            DWORD dwWidth,DWORD dwHeight,DWORD dwBPP);

void nvSwizBlt_cpu_swz2lin (DWORD dwSrcAddr,DWORD dwSrcX,DWORD dwSrcY,DWORD dwSrcLogWidth,DWORD dwSrcLogHeight,
                            DWORD dwDstAddr,DWORD dwDstPitch,DWORD dwDstX,DWORD dwDstY,
                            DWORD dwWidth,DWORD dwHeight,DWORD dwBPP);

void nvSwizBlt_cpu_swz2swz (DWORD dwSrcAddr,DWORD dwSrcX,DWORD dwSrcY,DWORD dwSrcLogWidth,DWORD dwSrcLogHeight,
                            DWORD dwDstAddr,DWORD dwDstX,DWORD dwDstY,DWORD dwDstLogWidth,DWORD dwDstLogHeight,
                            DWORD dwWidth,DWORD dwHeight,DWORD dwBPP);

// gpu swizzle routines - todo
/*
void nvSwizBlt_gpu_lin2lin
*/
void nvSwizBlt_gpu_lin2swz (DWORD dwSrcOffset,DWORD dwSrcHeap,DWORD dwSrcPitch,DWORD dwSrcX,DWORD dwSrcY,
                            DWORD dwDstOffset,DWORD dwDstX,DWORD dwDstY,DWORD dwDstLogWidth,DWORD dwDstLogHeight,
                            DWORD dwWidth,DWORD dwHeight,DWORD dwFormat);

/*
void nvSwizBlt_gpu_swz2lin
*/
/*
void nvSwizBlt_gpu_swz2swz
*/

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////

#define PT_8888     0
#define PT_X888     1
#define PT_4444     2
#define PT_1555     3
#define PT_X555     4
#define PT_565      5

////////////////////////////////////////////////////////////////////////////////////////

class CPaletteBuilder
{
    //
    // create palette
    //
public:
    BOOL initTable  (DWORD dwPalType);
    BOOL buildTable (DWORD dwPalType,DWORD dwSource,DWORD dwWidth,DWORD dwHeight);
    BOOL palettize  (DWORD dwPalType,BYTE *pbPalette);
    BOOL apply      (DWORD dwPalType,DWORD dwSource,DWORD dwDest,DWORD dwWidth,DWORD dwLogWidth,DWORD dwHeight,DWORD dwLogHeight);
    BOOL freeTable  (DWORD dwPalType);

    //
    // depalettize
    //
public:
    BOOL depalettizeInPlace      (DWORD dwPalType,DWORD dwSource,DWORD dwDest,DWORD dwWidth,DWORD dwHeight,DWORD *pPalette);
    BOOL depalettizeAndDeswizzle (DWORD dwPalType,DWORD dwSource,DWORD dwDest,DWORD dwWidth,DWORD dwLogWidth,DWORD dwHeight,DWORD dwLogHeight,DWORD dwDeviceMemPaletteAddr);
};

#ifdef __cplusplus
}
#endif

#endif // _nvSwiz_h
