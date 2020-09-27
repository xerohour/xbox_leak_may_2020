 /***************************************************************************\
|*                                                                           *|
|*       Copyright 1993-2000 NVIDIA, Corporation.  All rights reserved.      *|
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
|*       Copyright 1993-2000 NVIDIA, Corporation.  All rights reserved.      *|
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

/*==========================================================================;
 *
 *  Copyright (C) 1999-2000 NVidia Corporation.  All Rights Reserved.
 *
 *  File:       MoCorr.cpp
 *  Content:    Windows98 DirectDraw 32 bit driver
 *
 ***************************************************************************/
#include "nvprecomp.h"

#if (NVARCH >= 0x10)


extern NVMOCOMPPARAMS nvMoCompParams;

extern PNVMCCORRFUNC2 nvDoAllIntraFieldCorrections;
extern PNVMCCORRFUNC2 nvDoAllFieldCorrections;
extern PNVMCCORRFUNC nvDoIntraFieldCorrection;
extern PNVMCCORRFUNC nvDoOptimizedIntraFieldCorrection;
extern PNVMCCORRFUNC nvDoFieldCorrection;

extern PNVMCCORRFUNC nvDoAllIntraFrameCorrections;
extern PNVMCCORRFUNC nvDoAllFrameCorrections;
extern PNVMCCORRFUNC nvDoIntraFrameCorrection;
extern PNVMCCORRFUNC nvDoFrameCorrection;

#ifdef  DEBUG
extern unsigned long moCompDebugLevel;
#endif  // DEBUG

/*
 * nvDoCelsiusIntraFrameCorrection
 *
 * Performs frame correction on a macroblock
 *
 * Note: All destination geometry coordinates are in quarter pel units.
 *       All texture coordinates are in quarter pel units.
 *
 */
void __stdcall nvDoCelsiusIntraFrameCorrection(void)
{
    // Texture1 points to a field full of 0x80s
    long                        texture1BlockX0 = 0;
    long                        texture1BlockY0 = 0;
    long                        texture1BlockX1;
    long                        texture1BlockY1;
    long                        texture0BlockX0 = 0;
    long                        texture0BlockY0 = (nvMoCompParams.correctionY << 2); // Assumes textures with 8 byte pitch
    long                        texture0BlockX1;
    long                        texture0BlockY1;
    long                        blockX0;
    long                        blockX1;
    long                        blockY0;
    long                        blockY1;
    
    texture1BlockX1 = nvMoCompParams.blockWidth;
    texture1BlockY1 = nvMoCompParams.blockHeight;
    texture1BlockY1 <<= 16;

    texture0BlockX1 = texture0BlockX0 + nvMoCompParams.blockWidth;
    texture0BlockY1 = texture0BlockY0 + nvMoCompParams.blockHeight;
    texture0BlockY1 <<= 16;
    texture0BlockY0 <<= 16;

    blockX0 = nvMoCompParams.blockX - nvMoCompParams.celsiusDestinationViewportXOffset;
    blockY0 = nvMoCompParams.blockY;
    blockY0 <<= 16;
    blockX1 = blockX0 + nvMoCompParams.blockWidth;
    blockX1 &= 0x0000FFFF;
    blockX0 &= 0x0000FFFF;
    blockY1 = nvMoCompParams.blockY + nvMoCompParams.blockHeight;
    blockY1 <<= 16;

    // Change texture1 surface offset to point to block of half bias values (0x80)
    nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_TEXTURE_OFFSET(1) | 0x40000);
    nvPushData(1, nvMoCompParams.surfaceOffset + nvMoCompParams.surfaceBiasedIntraBlockOffset);

    // Change texture1 pitch to 16 bytes
    nvPushData(2, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_TEXTURE_CONTROL1(1) | 0x40000);
    nvPushData(3, 16 << 16); // half bias Intra block pitch

    nvPushData(4, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_BEGIN_END | 0x40000);
    nvPushData(5, NV056_SET_BEGIN_END_OP_QUADS);

    nvPushData(6, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_INLINE_ARRAY(0) | 0x300000);

    nvPushMoCompQuadData(7);

    nvPushData(19, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                   NV056_SET_BEGIN_END | 0x40000);
    nvPushData(20, NV056_SET_BEGIN_END_OP_END);

    nvPusherAdjust(21);

#ifdef  DEBUG
    if (moCompDebugLevel > 1)
        nvPusherStart(TRUE);
#endif  // DEBUG

} /* nvDoCelsiusIntraFrameCorrection */


/*
 * nvDoAllCelsiusIntraFrameCorrections
 *
 * Performs frame correction on all blocks in a macroblock
 *
 * Note: All destination geometry coordinates are in quarter pel units.
 *       All texture coordinates are in quarter pel units.
 *
 */
void __stdcall nvDoAllCelsiusIntraFrameCorrections(void)
{
    // Texture1 points to a field full of 0x80s
    long                        texture1BlockX0 = 0;
    long                        texture1BlockY0 = 0;
    long                        texture1BlockX1;
    long                        texture1BlockY1;
    long                        texture0BlockX0 = 0;
    long                        texture0BlockY0 = (nvMoCompParams.correctionY << 2); // Assumes textures with 8 byte pitch
    long                        texture0BlockX1;
    long                        texture0BlockY1;
    long                        blockX0;
    long                        blockX1;
    long                        blockY0;
    long                        blockY1;
    long                        nextBlockYOffset = 32 << 16;
    
    texture1BlockX1 = nvMoCompParams.blockWidth;
    texture1BlockY1 = nvMoCompParams.blockHeight;
    texture1BlockY1 <<= 16;

    texture0BlockX1 = texture0BlockX0 + nvMoCompParams.blockWidth;
    texture0BlockY1 = texture0BlockY0 + nvMoCompParams.blockHeight;
    texture0BlockY1 <<= 16;
    texture0BlockY0 <<= 16;

    blockX0 = nvMoCompParams.blockX - nvMoCompParams.celsiusDestinationViewportXOffset;
    blockY0 = nvMoCompParams.blockY;
    blockY0 <<= 16;
    blockX1 = blockX0 + nvMoCompParams.blockWidth;
    blockX1 &= 0x0000FFFF;
    blockX0 &= 0x0000FFFF;
    blockY1 = nvMoCompParams.blockY + nvMoCompParams.blockHeight;
    blockY1 <<= 16;

    // Change texture1 surface offset to point to block of half bias values (0x80)
    nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_TEXTURE_OFFSET(1) | 0x40000);
    nvPushData(1, nvMoCompParams.surfaceOffset + nvMoCompParams.surfaceBiasedIntraBlockOffset);

    // Change texture1 pitch to 16 bytes
    nvPushData(2, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_TEXTURE_CONTROL1(1) | 0x40000);
    nvPushData(3, 16 << 16); // half bias Intra block pitch

    nvPushData(4, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_BEGIN_END | 0x40000);
    nvPushData(5, NV056_SET_BEGIN_END_OP_QUADS);

    nvPushData(6, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_INLINE_ARRAY(0) | 0xC00000);

    nvPushMoCompQuadData(7);

    texture0BlockY0 += nextBlockYOffset;
    texture0BlockY1 += nextBlockYOffset;
    blockX0 += 32;
    blockX1 += 32;
    blockX1 &= 0x0000FFFF;
    blockX0 &= 0x0000FFFF;
    
    nvPushMoCompQuadData(19);

    texture0BlockY0 += nextBlockYOffset;
    texture0BlockY1 += nextBlockYOffset;
    blockX0 -= 32;
    blockX1 -= 32;
    blockX1 &= 0x0000FFFF;
    blockX0 &= 0x0000FFFF;
    blockY0 += nextBlockYOffset;
    blockY1 += nextBlockYOffset;

    nvPushMoCompQuadData(31);

    texture0BlockY0 += nextBlockYOffset;
    texture0BlockY1 += nextBlockYOffset;
    blockX0 += 32;
    blockX1 += 32;
    blockX1 &= 0x0000FFFF;
    blockX0 &= 0x0000FFFF;

    nvPushMoCompQuadData(43);

    nvPushData(55, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                   NV056_SET_BEGIN_END | 0x40000);
    nvPushData(56, NV056_SET_BEGIN_END_OP_END);

    nvPusherAdjust(57);

#ifdef  DEBUG
    if (moCompDebugLevel > 1)
        nvPusherStart(TRUE);
#endif  // DEBUG

} /* nvDoAllCelsiusIntraFrameCorrections */



/*
 * nvDoCelsiusFrameCorrection
 *
 * Performs frame correction on a macroblock
 *
 * Note: All destination geometry coordinates are in quarter pel units.
 *       All texture coordinates are in quarter pel units.
 *
 */
void __stdcall nvDoCelsiusFrameCorrection(void)
{
    // Texture1 and the destination surface are the same surface
    long texture1BlockX0;
    long texture1BlockY0;
    long texture1BlockX1;
    long texture1BlockY1;
    long texture0BlockX0 = 0;
    long texture0BlockY0 = nvMoCompParams.correctionY << 2; // Assumes textures with 8 byte pitch
    long texture0BlockX1;
    long texture0BlockY1;
    long blockX0;
    long blockX1;
    long blockY0;
    long blockY1;
    
    texture1BlockX0 = nvMoCompParams.blockX;
    texture1BlockY0 = nvMoCompParams.blockY;
    texture1BlockY0 <<= 16;
    texture1BlockX1 = texture1BlockX0 + nvMoCompParams.blockWidth;
    texture1BlockY1 = nvMoCompParams.blockY + nvMoCompParams.blockHeight;
    texture1BlockY1 <<= 16;

    texture0BlockX1 = texture0BlockX0 + nvMoCompParams.blockWidth;
    texture0BlockY1 = texture0BlockY0 + nvMoCompParams.blockHeight;
    texture0BlockY1 <<= 16;
    texture0BlockY0 <<= 16;

    blockX0 = nvMoCompParams.blockX - nvMoCompParams.celsiusDestinationViewportXOffset;
    blockY0 = nvMoCompParams.blockY;
    blockY0 <<= 16;
    blockX1 = blockX0 + nvMoCompParams.blockWidth;
    blockX1 &= 0x0000FFFF;
    blockX0 &= 0x0000FFFF;
    blockY1 = nvMoCompParams.blockY + nvMoCompParams.blockHeight;
    blockY1 <<= 16;

    nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_BEGIN_END | 0x40000);
    nvPushData(1, NV056_SET_BEGIN_END_OP_QUADS);

    nvPushData(2, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_INLINE_ARRAY(0) | 0x300000);

    nvPushMoCompQuadData(3);

    nvPushData(15, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                   NV056_SET_BEGIN_END | 0x40000);
    nvPushData(16, NV056_SET_BEGIN_END_OP_END);

    nvPusherAdjust(17);

#ifdef  DEBUG
    if (moCompDebugLevel > 1)
        nvPusherStart(TRUE);
#endif  // DEBUG

} /* nvDoCelsiusFrameCorrection */


/*
 * nvDoAllCelsiusFrameCorrections
 *
 * Performs frame correction on all blocks in a macroblock
 *
 * Note: All destination geometry coordinates are in quarter pel units.
 *       All texture coordinates are in quarter pel units.
 *
 */
void __stdcall nvDoAllCelsiusFrameCorrections(void)
{
    // Texture1 and the destination surface are the same surface
    long texture1BlockX0;
    long texture1BlockY0;
    long texture1BlockX1;
    long texture1BlockY1;
    long texture0BlockX0 = 0;
    long texture0BlockY0 = nvMoCompParams.correctionY << 2; // Assumes textures with 8 byte pitch
    long texture0BlockX1;
    long texture0BlockY1;
    long blockX0;
    long blockX1;
    long blockY0;
    long blockY1;
    long nextBlockYOffset = 32 << 16;
    
    texture1BlockX0 = nvMoCompParams.blockX;
    texture1BlockY0 = nvMoCompParams.blockY;
    texture1BlockY0 <<= 16;
    texture1BlockX1 = texture1BlockX0 + nvMoCompParams.blockWidth;
    texture1BlockY1 = nvMoCompParams.blockY + nvMoCompParams.blockHeight;
    texture1BlockY1 <<= 16;

    texture0BlockX1 = texture0BlockX0 + nvMoCompParams.blockWidth;
    texture0BlockY1 = texture0BlockY0 + nvMoCompParams.blockHeight;
    texture0BlockY1 <<= 16;
    texture0BlockY0 <<= 16;
    
    blockX0 = nvMoCompParams.blockX - nvMoCompParams.celsiusDestinationViewportXOffset;
    blockY0 = nvMoCompParams.blockY;
    blockY0 <<= 16;
    blockX1 = blockX0 + nvMoCompParams.blockWidth;
    blockX1 &= 0x0000FFFF;
    blockX0 &= 0x0000FFFF;
    blockY1 = nvMoCompParams.blockY + nvMoCompParams.blockHeight;
    blockY1 <<= 16;

    nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_BEGIN_END | 0x40000);
    nvPushData(1, NV056_SET_BEGIN_END_OP_QUADS);

    nvPushData(2, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_INLINE_ARRAY(0) | 0xC00000);

    nvPushMoCompQuadData(3);

    texture1BlockX0 += 32;
    texture1BlockX1 += 32;
    texture0BlockY0 += nextBlockYOffset;
    texture0BlockY1 += nextBlockYOffset;
    blockX0 += 32;
    blockX1 += 32;
    blockX1 &= 0x0000FFFF;
    blockX0 &= 0x0000FFFF;
    
    nvPushMoCompQuadData(15);

    texture1BlockX0 -= 32;
    texture1BlockX1 -= 32;
    texture1BlockY0 += nextBlockYOffset;
    texture1BlockY1 += nextBlockYOffset;
    texture0BlockY0 += nextBlockYOffset;
    texture0BlockY1 += nextBlockYOffset;
    blockX0 -= 32;
    blockX1 -= 32;
    blockX1 &= 0x0000FFFF;
    blockX0 &= 0x0000FFFF;
    blockY0 += nextBlockYOffset;
    blockY1 += nextBlockYOffset;

    nvPushMoCompQuadData(27);

    texture1BlockX0 += 32;
    texture1BlockX1 += 32;
    texture0BlockY0 += nextBlockYOffset;
    texture0BlockY1 += nextBlockYOffset;
    blockX0 += 32;
    blockX1 += 32;
    blockX1 &= 0x0000FFFF;
    blockX0 &= 0x0000FFFF;

    nvPushMoCompQuadData(39);

    nvPushData(51, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                   NV056_SET_BEGIN_END | 0x40000);
    nvPushData(52, NV056_SET_BEGIN_END_OP_END);

    nvPusherAdjust(53);

#ifdef  DEBUG
    if (moCompDebugLevel > 1)
        nvPusherStart(TRUE);
#endif  // DEBUG

} /* nvDoAllCelsiusFrameCorrections */


/*
 * nvCorrectCelsiusIntraFrame                                             
 *
 * Processes LUMA blocks in an IntraFrame then makes a 2nd pass and processes chroma blocks
 *
 * Note: All destination geometry sent to correction functions is in quarter pel units.  This is to avoid
 *       the need to update the composite transform as we alternate between prediction and correction.
 *       All correction (IDCT) texture geometry is in quarter pel units.
 *
 *       Due to celcius class limitations, the maximum post-transformed texture y coordinate is 2047 which
 *       corresponds to a maximum (IDCTIndex - firstAlignedIDCTIndex) value of 255 assuming all blocks are 8 texels high (by 8 texels wide).
 *
 */
void __stdcall nvCorrectCelsiusIntraFrame(LPNVDECODEMACROBLOCK pFirstMacroblock, long macroblockCount)
{
    unsigned long               firstAlignedIndex = pDriverData->dwMCFirstAlignedIDCTIndex;
    LPNVDECODEMACROBLOCK        pMacroblock;
    unsigned long               IDCTIndex;
    long                        chromaVOffset;
    BYTE                        blockPattern;
    unsigned long               dstSurfacePitch;
    // Texture1 points to a field full of 0x80s
    long                        texture1BlockX0 = 0;
    long                        texture1BlockY0 = 0;
    long                        texture1BlockX1 = 64;
    long                        texture1BlockY1 = 32 << 16;
    long                        texture0BlockX0 = 0;
    long                        texture0BlockY0;
    long                        texture0BlockX1 = 64;
    long                        texture0BlockY1;
    long                        blockX0;
    long                        blockX1;
    long                        blockY0;
    long                        blockY1;
    int                         n;
    int                         i;
    
    dstSurfacePitch = (nvMoCompParams.surfaceLumaPitch << 16) | nvMoCompParams.surfaceLumaPitch;
            
    chromaVOffset = nvMoCompParams.surfaceLumaHeight << 2; // surfaceLumaHeight * 4 (for quarter pel coords)

    pMacroblock = pFirstMacroblock;
    n = macroblockCount;

    if (pMacroblock->codedBlockPattern == 0xFF) {
        // Change texture0 pitch to 16 bytes
        nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                       NV056_SET_TEXTURE_CONTROL1(0) | 0x80000);
        nvPushData(1, 16 << 16);
        nvPushData(2, 16 << 16); // half bias Intra block pitch

        nvPusherAdjust(3);
    }   
    
    i = 0;
    
    nvMoCompParams.destinationField = 0;

    while (--n >= 0) {
    
        IDCTIndex = pMacroblock->firstIDCTIndex;
        blockPattern = pMacroblock->codedBlockPattern;

        if (pMacroblock->DCTType != DCT_FIELD) {
            nvMoCompParams.correctionField = 0;
            nvMoCompParams.destinationField = 0;
            nvMoCompParams.blockX = pMacroblock->hOffset << 2;
            nvMoCompParams.blockY = pMacroblock->vOffset << 2;

            if (blockPattern >= 0xFC) {
                nvMoCompParams.blockWidth = 64;
                nvMoCompParams.blockHeight = 64;
                nvMoCompParams.correctionY = (IDCTIndex - firstAlignedIndex) << 2; // IDCT blocks are 4 16 byte scanlines high
                nvDoIntraFrameCorrection();
            } else {
                // Change texture0 pitch to 8 bytes
                nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                               NV056_SET_TEXTURE_CONTROL1(0) | 0x80000);
                nvPushData(1, 8 << 16);
                nvPushData(2, 8 << 16); // half bias Intra block pitch

                nvPusherAdjust(3);

                nvMoCompParams.blockWidth = 32;
                nvMoCompParams.blockHeight = 32;
                nvMoCompParams.correctionY = (IDCTIndex - firstAlignedIndex) << 3; // IDCT blocks are 8 8 byte scanlines high
                nvDoAllIntraFrameCorrections();

                // Change texture0 pitch to 16 bytes
                nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                               NV056_SET_TEXTURE_CONTROL1(0) | 0x80000);
                nvPushData(1, 16 << 16);
                nvPushData(2, 16 << 16); // half bias Intra block pitch

                nvPusherAdjust(3);
            }

        } else
            i++;

        pMacroblock++;
    }
    
    nvPusherStart(TRUE);

    if (i > 0) { // if there were any DCT_FIELD macroblocks

        pMacroblock = pFirstMacroblock;
        n = macroblockCount;

        // Must temporarily set destination surface pitch back to double it's normal frame pitch
        nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                      NV056_SET_SURFACE_PITCH | 0x40000);
        nvPushData(1, dstSurfacePitch << 1);

        nvPusherAdjust(2);
                
        while (--n >= 0) {
        
            IDCTIndex = pMacroblock->firstIDCTIndex;
            blockPattern = pMacroblock->codedBlockPattern;

            if (pMacroblock->DCTType == DCT_FIELD) {
                nvMoCompParams.correctionField = 0;
                nvMoCompParams.destinationField = 0;
                if (blockPattern >= 0xFC) {
                    nvMoCompParams.blockWidth = 64;
                    nvMoCompParams.correctionY = (IDCTIndex - firstAlignedIndex) << 2; // IDCT blocks are 4 16 byte scanlines high
                } else {    
                    nvMoCompParams.blockWidth = 32;
                    nvMoCompParams.correctionY = (IDCTIndex - firstAlignedIndex) << 3; // IDCT blocks are 8 8 byte blocks high
                }
                nvMoCompParams.blockHeight = 32;
                nvMoCompParams.blockX = pMacroblock->hOffset << 2;
                nvMoCompParams.blockY = pMacroblock->vOffset << 1;

                if (blockPattern >= 0xFC) {
                    nvDoIntraFieldCorrection();
                } else {
                    // Change texture0 pitch to 8 bytes
                    nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                                   NV056_SET_TEXTURE_CONTROL1(0) | 0x80000);
                    nvPushData(1, 8 << 16);
                    nvPushData(2, 8 << 16); // half bias Intra block pitch

                    nvPusherAdjust(3);

                    nvDoAllIntraFieldCorrections(8, 2);

                    // Change texture0 pitch to 16 bytes
                    nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                                   NV056_SET_TEXTURE_CONTROL1(0) | 0x80000);
                    nvPushData(1, 16 << 16);
                    nvPushData(2, 16 << 16); // half bias Intra block pitch

                    nvPusherAdjust(3);
                }
            }

            pMacroblock++;
        }

        nvPusherStart(TRUE);

        pMacroblock = pFirstMacroblock;
        n = macroblockCount;

        while (--n >= 0) {
        
            IDCTIndex = pMacroblock->firstIDCTIndex;
            blockPattern = pMacroblock->codedBlockPattern;

            if (pMacroblock->DCTType == DCT_FIELD) {
                nvMoCompParams.blockHeight = 32;
                nvMoCompParams.blockX = pMacroblock->hOffset << 2;
                nvMoCompParams.blockY = pMacroblock->vOffset << 1;

                if (blockPattern >= 0xFC) {
                    nvMoCompParams.blockWidth = 64;
                    nvMoCompParams.destinationField = 1;
                    nvMoCompParams.correctionY = (IDCTIndex + 2 - firstAlignedIndex) << 2; // IDCT blocks are 4 16 byte scanlines high
                    nvDoIntraFieldCorrection();
                } else {
                    // Change texture0 pitch to 8 bytes
                    nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                                   NV056_SET_TEXTURE_CONTROL1(0) | 0x80000);
                    nvPushData(1, 8 << 16);
                    nvPushData(2, 8 << 16); // half bias Intra block pitch

                    nvPusherAdjust(3);

                    nvMoCompParams.blockWidth = 32;
                    nvMoCompParams.destinationField = 1;
                    nvMoCompParams.correctionY = (IDCTIndex + 2 - firstAlignedIndex) << 3; // IDCT blocks are 8 8 byte blocks high
                    nvDoAllIntraFieldCorrections(8, 2);

                    // Change texture0 pitch to 16 bytes
                    nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                                   NV056_SET_TEXTURE_CONTROL1(0) | 0x80000);
                    nvPushData(1, 16 << 16);
                    nvPushData(2, 16 << 16); // half bias Intra block pitch

                    nvPusherAdjust(3);
                }
            }

            pMacroblock++;
        }

        nvPusherStart(TRUE);

    }
    

#ifdef  DEBUG
    if (moCompDebugLevel < 3) {
#endif  // DEBUG

    if (pDriverData->bMCCurrentExecuteFunction == EXECUTE_NON_PREBIASED_MOTION_COMPENSATION) {
        SET_UNBIASED_CHROMA_CORRECTION_DATA_FORMAT();
    } else {
        SET_CHROMA_CORRECTION_DATA_FORMAT();
    }    
  
    // Chroma IDCTs are always frame based
    pMacroblock = pFirstMacroblock;
    n = macroblockCount;

    nvMoCompParams.destinationField = 0;

    nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_SURFACE_PITCH | 0x40000);
    nvPushData(1, dstSurfacePitch);

    // Change texture1 surface offset to point to block of half bias values (0x80)
    nvPushData(2, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_TEXTURE_OFFSET(1) | 0x40000);
    nvPushData(3, nvMoCompParams.surfaceOffset + nvMoCompParams.surfaceBiasedIntraBlockOffset);

    // Temporarily change texture0 pitch to 16 bytes
    nvPushData(4, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                   NV056_SET_TEXTURE_CONTROL1(0) | 0x80000);
    nvPushData(5, 16 << 16);
    nvPushData(6, 16 << 16); // half bias Intra block pitch

    nvPusherAdjust(7);

    texture0BlockX1 = 32;
    texture1BlockX1 = 32;
 
    while (n >= 8) {

        n -= 8;
        
        nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                      NV056_SET_BEGIN_END | 0x40000);
        nvPushData(1, NV056_SET_BEGIN_END_OP_QUADS);

        nvPushData(2, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                      NV056_INLINE_ARRAY(0) | 0x1800000);

        i = 3;
        
        while (i < 99) {        
            IDCTIndex = pMacroblock->firstIDCTIndex + 4;

            texture0BlockY0 = (IDCTIndex - firstAlignedIndex) << 4; // Assumes textures with 8 byte pitch
            texture0BlockY1 = texture0BlockY0 + 32;
            texture0BlockY1 <<= 16;
            texture0BlockY0 <<= 16;

            nvMoCompParams.blockY = chromaVOffset + (pMacroblock->vOffset << 1);

            blockX0 = (pMacroblock->hOffset << 1) - nvMoCompParams.celsiusDestinationViewportXOffset;
            blockY0 = nvMoCompParams.blockY;
            blockY0 <<= 16;
            blockX1 = blockX0 + 32;
            blockX1 &= 0x0000FFFF;
            blockX0 &= 0x0000FFFF;
            blockY1 = nvMoCompParams.blockY + 32;
            blockY1 <<= 16;

            nvPushMoCompQuadData(i);
            
            i += 12;

            pMacroblock++;
        }
        
        nvPushData(i, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                       NV056_SET_BEGIN_END | 0x40000);
        i++;
                       
        nvPushData(i, NV056_SET_BEGIN_END_OP_END);
        
        i++;

        nvPusherAdjust(i);

#ifdef  DEBUG
        if (moCompDebugLevel > 1)
            nvPusherStart(TRUE);
#endif  // DEBUG
    }

    while (--n >= 0) {
    
        IDCTIndex = pMacroblock->firstIDCTIndex + 4;

        texture0BlockY0 = (IDCTIndex - firstAlignedIndex) << 4; // Assumes textures with 8 byte pitch
        texture0BlockY1 = texture0BlockY0 + 32;
        texture0BlockY1 <<= 16;
        texture0BlockY0 <<= 16;

        nvMoCompParams.blockY = chromaVOffset + (pMacroblock->vOffset << 1);

        blockX0 = (pMacroblock->hOffset << 1) - nvMoCompParams.celsiusDestinationViewportXOffset;
        blockY0 = nvMoCompParams.blockY;
        blockY0 <<= 16;
        blockX1 = blockX0 + 32;
        blockX1 &= 0x0000FFFF;
        blockX0 &= 0x0000FFFF;
        blockY1 = nvMoCompParams.blockY + 32;
        blockY1 <<= 16;

        nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                      NV056_SET_BEGIN_END | 0x40000);
        nvPushData(1, NV056_SET_BEGIN_END_OP_QUADS);

        nvPushData(2, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                      NV056_INLINE_ARRAY(0) | 0x300000);

        nvPushMoCompQuadData(3);

        nvPushData(15, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                       NV056_SET_BEGIN_END | 0x40000);
        nvPushData(16, NV056_SET_BEGIN_END_OP_END);

        nvPusherAdjust(17);

#ifdef  DEBUG
        if (moCompDebugLevel > 1)
            nvPusherStart(TRUE);
#endif  // DEBUG

        pMacroblock++;
    }

#ifdef  DEBUG
    }
#endif  // DEBUG

} /* nvCorrectCelsiusIntraFrame */


/*
 * nvCorrectCelsiusFullFrameLumaMacroblock
 *
 * Performs correction on all blocks in macroblock assuming Y0/Y1/Y2/Y3 are in NV optimized format
 *
 * Note: All destination geometry sent to correction functions is in quarter pel units.  This is to avoid
 *       the need to update the composite transform as we alternate between prediction and correction.
 *       All correction (IDCT) texture geometry is in quarter pel units.
 *
 *       Due to celcius class limitations, the maximum post-transformed texture y coordinate is 2047 which
 *       corresponds to a maximum (IDCTIndex - firstAlignedIDCTIndex) value of 255 assuming all blocks are 8 texels high (by 8 texels wide).
 *
 */
void __stdcall nvCorrectCelsiusFullFrameLumaMacroblock(LPNVDECODEMACROBLOCK pMacroblock, DWORD correctionType)
{
    unsigned long IDCTIndex = pMacroblock->firstIDCTIndex;
    unsigned long firstAlignedIndex = pDriverData->dwMCFirstAlignedIDCTIndex;
    unsigned long dstSurfacePitch;
    
    if (pMacroblock->DCTType == DCT_FIELD) {

        dstSurfacePitch = (nvMoCompParams.surfaceLumaPitch << 16) | nvMoCompParams.surfaceLumaPitch;

        nvMoCompParams.correctionField = 0;
        nvMoCompParams.destinationField = 0;
        nvMoCompParams.blockWidth = 64;
        nvMoCompParams.blockHeight = 32;
        nvMoCompParams.blockX = pMacroblock->hOffset << 2;
        nvMoCompParams.blockY = pMacroblock->vOffset << 1;

        if (pMacroblock->macroblockType != MB_INTRA) {           

            // Temporarily change texture1 pitch to match destination surface pitch
            nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                           NV056_SET_TEXTURE_CONTROL1(1) | 0x40000);
            nvPushData(1, dstSurfacePitch << 17);

            nvPusherAdjust(2);

            nvMoCompParams.correctionY = (IDCTIndex - firstAlignedIndex) << 2; // IDCT blocks are 4 16 byte scanlines high
            nvDoFieldCorrection();
            nvMoCompParams.destinationField = 1;
            nvMoCompParams.correctionY = (IDCTIndex + 2 - firstAlignedIndex) << 2; // IDCT blocks are 4 16 byte scanlines high
            nvDoFieldCorrection();
            
            // Restore texture1 pitch to it's default value
            nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                           NV056_SET_TEXTURE_CONTROL1(1) | 0x40000);
            nvPushData(1, nvMoCompParams.surfaceLumaPitch << 16);

            nvPusherAdjust(2);

        } else {
            nvMoCompParams.correctionY = (IDCTIndex - firstAlignedIndex) << 2; // IDCT blocks are 4 16 byte scanlines high
            nvDoIntraFieldCorrection();
            nvMoCompParams.destinationField = 1;
            nvMoCompParams.correctionY = (IDCTIndex + 2 - firstAlignedIndex) << 2; // IDCT blocks are 4 16 byte scanlines high
            nvDoIntraFieldCorrection();
        }

    } else {
        nvMoCompParams.correctionField = 0;
        nvMoCompParams.destinationField = 0;
        nvMoCompParams.blockX = pMacroblock->hOffset << 2;
        nvMoCompParams.blockY = pMacroblock->vOffset << 2;

        nvMoCompParams.blockWidth = 64;
        nvMoCompParams.blockHeight = 64;
        nvMoCompParams.correctionY = (IDCTIndex - firstAlignedIndex) << 2; // IDCT blocks are 4 16 byte scanlines high

        nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                       NV056_SET_TEXTURE_OFFSET(1) | 0x40000);
        nvPushData(1, nvMoCompParams.surfaceOffset);

        nvPusherAdjust(2);

        if (pMacroblock->macroblockType != MB_INTRA) {
            nvDoFrameCorrection();
        } else
            nvDoIntraFrameCorrection();

    }

} /* nvCorrectCelsiusFullFrameLumaMacroblock */


/*
 * nvCorrectCelsiusFullFrameChromaMacroblock
 *
 * Performs correction on all blocks in macroblock assuming Y0/Y1/Y2/Y3 are in NV optimized format
 *
 * Note: All destination geometry sent to correction functions is in quarter pel units.  This is to avoid
 *       the need to update the composite transform as we alternate between prediction and correction.
 *       All correction (IDCT) texture geometry is in quarter pel units.
 *
 *       Due to celcius class limitations, the maximum post-transformed texture y coordinate is 2047 which
 *       corresponds to a maximum (IDCTIndex - firstAlignedIDCTIndex) value of 255 assuming all blocks are 8 texels high (by 8 texels wide).
 *
 */
void __stdcall nvCorrectCelsiusFullFrameChromaMacroblock(LPNVDECODEMACROBLOCK pMacroblock, DWORD correctionType)
{
    unsigned long IDCTIndex = pMacroblock->firstIDCTIndex;
    unsigned long firstAlignedIndex = pDriverData->dwMCFirstAlignedIDCTIndex;
    long chromaVOffset;
    
    chromaVOffset = nvMoCompParams.surfaceLumaHeight << 2; // surfaceLumaHeight * 4 (for quarter pel coords)

    IDCTIndex += 4;

#ifdef  DEBUG
    if (moCompDebugLevel < 3) {
#endif  // DEBUG

    // Chroma IDCTs are always frame based
    nvMoCompParams.correctionField = 0;
    nvMoCompParams.destinationField = 0;
    nvMoCompParams.blockWidth = 32;
    nvMoCompParams.blockHeight = 32;
    nvMoCompParams.blockX = pMacroblock->hOffset << 1;
    nvMoCompParams.blockY = chromaVOffset + (pMacroblock->vOffset << 1);
    nvMoCompParams.correctionY = (IDCTIndex - firstAlignedIndex) << 2; // IDCT blocks are 4 16 byte scanlines high

    if (pMacroblock->macroblockType != MB_INTRA)           
        nvDoFrameCorrection();
    else
        nvDoIntraFrameCorrection();

#ifdef  DEBUG
    }
#endif  // DEBUG

} /* nvCorrectCelsiusFullFrameChromaMacroblock */


/*
 * nvCorrectCelsiusFrameLumaMacroblock
 *
 * Determines which blocks in the block pattern need correction and calls the appropriate function
 *
 * Note: All destination geometry sent to correction functions is in quarter pel units.  This is to avoid
 *       the need to update the composite transform as we alternate between prediction and correction.
 *       All correction (IDCT) texture geometry is in quarter pel units.
 *
 *       Due to celcius class limitations, the maximum post-transformed texture y coordinate is 2047 which
 *       corresponds to a maximum (IDCTIndex - firstAlignedIDCTIndex) value of 255 assuming all blocks are 8 texels high (by 8 texels wide).
 *
 */
void __stdcall nvCorrectCelsiusFrameLumaMacroblock(LPNVDECODEMACROBLOCK pMacroblock, DWORD correctionType)
{
    unsigned long IDCTIndex = pMacroblock->firstIDCTIndex;
    unsigned long firstAlignedIndex = pDriverData->dwMCFirstAlignedIDCTIndex;
    BYTE blockPattern;
    unsigned long dstSurfacePitch;
    
    if (correctionType == CORRECTION_FIRST_PASS) {

        blockPattern = pMacroblock->codedBlockPattern;

    } else {

        blockPattern = pMacroblock->codedBlockPattern;
        if (blockPattern & CBP_Y0)
            IDCTIndex++;
        if (blockPattern & CBP_Y1)
            IDCTIndex++;
        if (blockPattern & CBP_Y2)
            IDCTIndex++;
        if (blockPattern & CBP_Y3)
            IDCTIndex++;
        if (blockPattern & (CBP_CR | CBP_CB))
            IDCTIndex += 2;
        blockPattern = pMacroblock->overflowCodedBlockPattern;
    }

    if (pMacroblock->DCTType == DCT_FIELD) {
        // Must temporarily set destination surface pitch back to double it's normal frame pitch
        dstSurfacePitch = (nvMoCompParams.surfaceLumaPitch << 16) | nvMoCompParams.surfaceLumaPitch;

        if (pMacroblock->macroblockType != MB_INTRA) {           
            // Temporarily change texture1 pitch to match destination surface pitch
            nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                           NV056_SET_TEXTURE_CONTROL1(1) | 0x40000);
            nvPushData(1, dstSurfacePitch << 17);

            nvPusherAdjust(2);
        }
    }

    if ((blockPattern & (CBP_Y0 | CBP_Y1 | CBP_Y2 | CBP_Y3)) == (CBP_Y0 | CBP_Y1 | CBP_Y2 | CBP_Y3)) {

        if (pMacroblock->DCTType == DCT_FIELD) {
            nvMoCompParams.correctionField = 0;
            nvMoCompParams.destinationField = 0;
            nvMoCompParams.blockHeight = 32;
            nvMoCompParams.blockX = pMacroblock->hOffset << 2;
            nvMoCompParams.blockY = pMacroblock->vOffset << 1;

            if (pMacroblock->macroblockType != MB_INTRA) {           

                if (blockPattern >= 0xFC) {

                    // Temporarily change texture0 pitch to 16 bytes
                    nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                                   NV056_SET_TEXTURE_CONTROL1(0) | 0x40000);
                    nvPushData(1, 16 << 16);

                    nvPusherAdjust(2);

                    nvMoCompParams.blockWidth = 64;

                    if ((nvMoCompParams.surfaceLumaPitch >= 1024) && (correctionType == CORRECTION_FIRST_PASS)) {
                        if (nvMoCompParams.destinationFieldIs0) {
                            nvMoCompParams.correctionY = (IDCTIndex - firstAlignedIndex) << 2; // IDCT blocks are 4 16 byte scanlines high
                            nvDoFieldCorrection();
                        } else {
                            nvMoCompParams.destinationField = 1;
                            nvMoCompParams.correctionY = (IDCTIndex + 2 - firstAlignedIndex) << 2; // IDCT blocks are 4 16 byte scanlines high
                            nvDoFieldCorrection();
                        }
                    } else {
                        nvMoCompParams.correctionY = (IDCTIndex - firstAlignedIndex) << 2; // IDCT blocks are 4 16 byte scanlines high
                        nvDoFieldCorrection();
                        nvMoCompParams.destinationField = 1;
                        nvMoCompParams.correctionY = (IDCTIndex + 2 - firstAlignedIndex) << 2; // IDCT blocks are 4 16 byte scanlines high
                        nvDoFieldCorrection();
                    }

                    // Restore texture0 pitch to default value
                    nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                                   NV056_SET_TEXTURE_CONTROL1(0) | 0x40000);
                    nvPushData(1, 8 << 16);

                    nvPusherAdjust(2);

                } else {

                    nvMoCompParams.blockWidth = 32;

                    if ((nvMoCompParams.surfaceLumaPitch >= 1024) && (correctionType == CORRECTION_FIRST_PASS)) {
                        if (nvMoCompParams.destinationFieldIs0) {
                            nvMoCompParams.correctionY = (IDCTIndex - firstAlignedIndex) << 3; // IDCT blocks are 8 8 byte blocks high
                            nvDoAllFieldCorrections(8, 2);
                        } else {
                            nvMoCompParams.destinationField = 1;
                            nvMoCompParams.correctionY = (IDCTIndex + 2 - firstAlignedIndex) << 3; // IDCT blocks are 8 8 byte blocks high
                            nvDoAllFieldCorrections(8, 2);
                        }    
                    } else {
                        nvMoCompParams.correctionY = (IDCTIndex - firstAlignedIndex) << 3; // IDCT blocks are 8 8 byte blocks high
                        nvDoAllFieldCorrections(8, 2);
                        nvMoCompParams.destinationField = 1;
                        nvMoCompParams.correctionY = (IDCTIndex + 2 - firstAlignedIndex) << 3; // IDCT blocks are 8 8 byte blocks high
                        nvDoAllFieldCorrections(8, 2);
                    }
                }
                
            } else {
                if (blockPattern >= 0xFC) {
                    // Temporarily change texture0 pitch to 16 bytes
                    nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                                   NV056_SET_TEXTURE_CONTROL1(0) | 0x40000);
                    nvPushData(1, 16 << 16);

                    nvPusherAdjust(2);

                    nvMoCompParams.blockWidth = 64;

                    if ((nvMoCompParams.surfaceLumaPitch >= 1024) && (correctionType == CORRECTION_FIRST_PASS)) {
                        if (nvMoCompParams.destinationFieldIs0) {
                            nvMoCompParams.correctionY = (IDCTIndex - firstAlignedIndex) << 2; // IDCT blocks are 4 16 byte scanlines high
                            nvDoIntraFieldCorrection();
                        } else {    
                            nvMoCompParams.destinationField = 1;
                            nvMoCompParams.correctionY = (IDCTIndex + 2 - firstAlignedIndex) << 2; // IDCT blocks are 4 16 byte scanlines high
                            nvDoIntraFieldCorrection();
                        }    
                    } else {
                        nvMoCompParams.correctionY = (IDCTIndex - firstAlignedIndex) << 2; // IDCT blocks are 4 16 byte scanlines high
                        nvDoIntraFieldCorrection();
                        nvMoCompParams.destinationField = 1;
                        nvMoCompParams.correctionY = (IDCTIndex + 2 - firstAlignedIndex) << 2; // IDCT blocks are 4 16 byte scanlines high
                        nvDoIntraFieldCorrection();
                    }

                    // Restore texture0 pitch to default value
                    nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                                   NV056_SET_TEXTURE_CONTROL1(0) | 0x40000);
                    nvPushData(1, 8 << 16);

                    nvPusherAdjust(2);
                } else {
                    nvMoCompParams.blockWidth = 32;

                    if ((nvMoCompParams.surfaceLumaPitch >= 1024) && (correctionType == CORRECTION_FIRST_PASS)) {
                        if (nvMoCompParams.destinationFieldIs0) {
                            nvMoCompParams.correctionY = (IDCTIndex - firstAlignedIndex) << 3; // IDCT blocks are 8 8 byte blocks high
                            nvDoAllIntraFieldCorrections(8, 2);
                        } else {
                            nvMoCompParams.destinationField = 1;
                            nvMoCompParams.correctionY = (IDCTIndex + 2 - firstAlignedIndex) << 3; // IDCT blocks are 8 8 byte blocks high
                            nvDoAllIntraFieldCorrections(8, 2);
                        }
                    } else {
                        nvMoCompParams.correctionY = (IDCTIndex - firstAlignedIndex) << 3; // IDCT blocks are 8 8 byte blocks high
                        nvDoAllIntraFieldCorrections(8, 2);
                        nvMoCompParams.destinationField = 1;
                        nvMoCompParams.correctionY = (IDCTIndex + 2 - firstAlignedIndex) << 3; // IDCT blocks are 8 8 byte blocks high
                        nvDoAllIntraFieldCorrections(8, 2);
                    }    
                }
            }
            
            if (pMacroblock->macroblockType != MB_INTRA) {           
                nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                               NV056_SET_TEXTURE_CONTROL1(1) | 0x40000);
                nvPushData(1, nvMoCompParams.surfaceLumaPitch << 16);

                nvPusherAdjust(2);
            }
        } else {
            nvMoCompParams.correctionField = 0;
            nvMoCompParams.destinationField = 0;
            nvMoCompParams.blockX = pMacroblock->hOffset << 2;
            nvMoCompParams.blockY = pMacroblock->vOffset << 2;

            nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                           NV056_SET_TEXTURE_OFFSET(1) | 0x40000);
            nvPushData(1, nvMoCompParams.surfaceOffset);

            nvPusherAdjust(2);

            if (blockPattern >= 0xFC) {
                nvMoCompParams.blockWidth = 64;
                nvMoCompParams.blockHeight = 64;
                nvMoCompParams.correctionY = (IDCTIndex - firstAlignedIndex) << 2; // IDCT blocks are 4 16 byte scanlines high

                // Temporarily change texture0 pitch to 16 bytes
                nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                               NV056_SET_TEXTURE_CONTROL1(0) | 0x40000);
                nvPushData(1, 16 << 16);

                nvPusherAdjust(2);

                if (pMacroblock->macroblockType != MB_INTRA) {          
                    nvDoFrameCorrection();
                } else
                    nvDoIntraFrameCorrection();

                // Restore texture0 pitch to default value
                nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                               NV056_SET_TEXTURE_CONTROL1(0) | 0x40000);
                nvPushData(1, 8 << 16);

                nvPusherAdjust(2);
            } else {
                nvMoCompParams.blockWidth = 32;
                nvMoCompParams.blockHeight = 32;
                nvMoCompParams.correctionY = (IDCTIndex - firstAlignedIndex) << 3; // IDCT blocks are 8 8 byte scanlines high

                if (pMacroblock->macroblockType != MB_INTRA) {          
                    nvDoAllFrameCorrections();
                } else
                    nvDoAllIntraFrameCorrections();
            }
        }

        IDCTIndex += 4;

    } else {

        nvMoCompParams.blockWidth = 32;
        nvMoCompParams.blockHeight = 32;

        if ((pMacroblock->DCTType == DCT_FIELD) && (nvMoCompParams.surfaceLumaPitch >= 1024) && (correctionType == CORRECTION_FIRST_PASS)) {

            if (blockPattern & CBP_Y0) {

                if (nvMoCompParams.destinationFieldIs0) {
                    nvMoCompParams.correctionField = 0;
                    nvMoCompParams.destinationField = 0; // Component Y0 is in destination field 0
                    nvMoCompParams.blockX = pMacroblock->hOffset << 2;
                    nvMoCompParams.blockY = pMacroblock->vOffset << 1;
                    nvMoCompParams.correctionY = (IDCTIndex - firstAlignedIndex) << 3; // IDCT blocks are 8 8 byte blocks high

                    if (pMacroblock->macroblockType != MB_INTRA)           
                        nvDoFieldCorrection();
                    else
                        nvDoIntraFieldCorrection();
                }
                
                IDCTIndex++;
            }

            if (blockPattern & CBP_Y1) {

                if (nvMoCompParams.destinationFieldIs0) {
                    nvMoCompParams.correctionField = 0;
                    nvMoCompParams.destinationField = 0; // Component Y1 is in destination field 0
                    nvMoCompParams.blockX = (pMacroblock->hOffset + 8) << 2;
                    nvMoCompParams.blockY = pMacroblock->vOffset << 1;
                    nvMoCompParams.correctionY = (IDCTIndex - firstAlignedIndex) << 3; // IDCT blocks are 8 8 byte blocks high
                    
                    if (pMacroblock->macroblockType != MB_INTRA)           
                        nvDoFieldCorrection();
                    else
                        nvDoIntraFieldCorrection();
                }
                
                IDCTIndex++;
            }

            if (blockPattern & CBP_Y2) {

                if (!nvMoCompParams.destinationFieldIs0) {
                    nvMoCompParams.correctionField = 0;
                    nvMoCompParams.destinationField = 1; // Component Y2 is in destination field 1
                    nvMoCompParams.blockX = pMacroblock->hOffset << 2;
                    nvMoCompParams.blockY = pMacroblock->vOffset << 1;
                    nvMoCompParams.correctionY = (IDCTIndex - firstAlignedIndex) << 3; // IDCT blocks are 8 8 byte scanlines high

                    if (pMacroblock->macroblockType != MB_INTRA)           
                        nvDoFieldCorrection();
                    else
                        nvDoIntraFieldCorrection();
                }
                
                IDCTIndex++;
            }

            if (blockPattern & CBP_Y3) {

                if (!nvMoCompParams.destinationFieldIs0) {
                    nvMoCompParams.correctionField = 0;
                    nvMoCompParams.destinationField = 1; // Component Y3 is in destination field 1
                    nvMoCompParams.blockX = (pMacroblock->hOffset + 8) << 2;
                    nvMoCompParams.blockY = pMacroblock->vOffset << 1;
                    nvMoCompParams.correctionY = (IDCTIndex - firstAlignedIndex) << 3; // IDCT blocks are 8 8 byte scanlines high

                    if (pMacroblock->macroblockType != MB_INTRA)           
                        nvDoFieldCorrection();
                    else
                        nvDoIntraFieldCorrection();
                }
                
                IDCTIndex++;
            }

        } else {

            if (blockPattern & CBP_Y0) {

                nvMoCompParams.correctionField = 0;
                nvMoCompParams.destinationField = 0; // Component Y0 is in destination field 0
                nvMoCompParams.blockX = pMacroblock->hOffset << 2;
                if (pMacroblock->DCTType == DCT_FIELD)
                    nvMoCompParams.blockY = pMacroblock->vOffset << 1;
                else
                    nvMoCompParams.blockY = pMacroblock->vOffset << 2;
                nvMoCompParams.correctionY = (IDCTIndex - firstAlignedIndex) << 3; // IDCT blocks are 8 8 byte blocks high

                if (pMacroblock->macroblockType != MB_INTRA)           
                    nvDoFieldCorrection();
                else
                    nvDoIntraFieldCorrection();

                IDCTIndex++;
            }

            if (blockPattern & CBP_Y1) {
                nvMoCompParams.correctionField = 0;
                nvMoCompParams.destinationField = 0; // Component Y1 is in destination field 0
                nvMoCompParams.blockX = (pMacroblock->hOffset + 8) << 2;
                if (pMacroblock->DCTType == DCT_FIELD)
                    nvMoCompParams.blockY = pMacroblock->vOffset << 1;
                else
                    nvMoCompParams.blockY = pMacroblock->vOffset << 2;
                nvMoCompParams.correctionY = (IDCTIndex - firstAlignedIndex) << 3; // IDCT blocks are 8 8 byte blocks high
                
                if (pMacroblock->macroblockType != MB_INTRA)           
                    nvDoFieldCorrection();
                else
                    nvDoIntraFieldCorrection();

                IDCTIndex++;
            }

            if (blockPattern & CBP_Y2) {
                nvMoCompParams.correctionField = 0;
                nvMoCompParams.blockX = pMacroblock->hOffset << 2;
                if (pMacroblock->DCTType == DCT_FIELD) {
                    nvMoCompParams.destinationField = 1; // Component Y2 is in destination field 1
                    nvMoCompParams.blockY = pMacroblock->vOffset << 1;
                } else {
                    nvMoCompParams.destinationField = 0;
                    nvMoCompParams.blockY = (pMacroblock->vOffset + 8) << 2;
                }
                nvMoCompParams.correctionY = (IDCTIndex - firstAlignedIndex) << 3; // IDCT blocks are 8 8 byte scanlines high

                if (pMacroblock->macroblockType != MB_INTRA)           
                    nvDoFieldCorrection();
                else
                    nvDoIntraFieldCorrection();

                IDCTIndex++;
            }

            if (blockPattern & CBP_Y3) {
                nvMoCompParams.correctionField = 0;
                nvMoCompParams.blockX = (pMacroblock->hOffset + 8) << 2;
                if (pMacroblock->DCTType == DCT_FIELD) {
                    nvMoCompParams.destinationField = 1; // Component Y3 is in destination field 1
                    nvMoCompParams.blockY = pMacroblock->vOffset << 1;
                } else {
                    nvMoCompParams.destinationField = 0;
                    nvMoCompParams.blockY = (pMacroblock->vOffset + 8) << 2;
                }
                nvMoCompParams.correctionY = (IDCTIndex - firstAlignedIndex) << 3; // IDCT blocks are 8 8 byte scanlines high

                if (pMacroblock->macroblockType != MB_INTRA)           
                    nvDoFieldCorrection();
                else
                    nvDoIntraFieldCorrection();

                IDCTIndex++;
            }
        }

        if (pMacroblock->DCTType == DCT_FIELD) {
            // Must temporarily set destination surface pitch back to double it's normal frame pitch
            dstSurfacePitch = (nvMoCompParams.surfaceLumaPitch << 16) | nvMoCompParams.surfaceLumaPitch;

            if (pMacroblock->macroblockType != MB_INTRA) {           
                // Temporarily change texture1 pitch to match destination surface pitch
                nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                               NV056_SET_TEXTURE_CONTROL1(1) | 0x40000);
                nvPushData(1, nvMoCompParams.surfaceLumaPitch << 16);

                nvPusherAdjust(2);
            }
        }
    }

} /* nvCorrectCelsiusFrameLumaMacroblock */


/*
 * nvCorrectCelsiusFrameChromaMacroblock
 *
 * Determines which blocks in the block pattern need correction and calls the appropriate function
 *
 * Note: All destination geometry sent to correction functions is in quarter pel units.  This is to avoid
 *       the need to update the composite transform as we alternate between prediction and correction.
 *       All correction (IDCT) texture geometry is in quarter pel units.
 *
 *       Due to celcius class limitations, the maximum post-transformed texture y coordinate is 2047 which
 *       corresponds to a maximum (IDCTIndex - firstAlignedIDCTIndex) value of 255 assuming all blocks are 8 texels high (by 8 texels wide).
 *
 */
void __stdcall nvCorrectCelsiusFrameChromaMacroblock(LPNVDECODEMACROBLOCK pMacroblock, DWORD correctionType)
{
    unsigned long IDCTIndex = pMacroblock->firstIDCTIndex;
    unsigned long firstAlignedIndex = pDriverData->dwMCFirstAlignedIDCTIndex;
    long chromaVOffset;
    BYTE blockPattern;
    
    chromaVOffset = nvMoCompParams.surfaceLumaHeight << 2; // surfaceLumaHeight * 4 (for quarter pel coords)

    if (correctionType == CORRECTION_FIRST_PASS) {

        blockPattern = pMacroblock->codedBlockPattern;

    } else {

        blockPattern = pMacroblock->codedBlockPattern;
        if (blockPattern & CBP_Y0)
            IDCTIndex++;
        if (blockPattern & CBP_Y1)
            IDCTIndex++;
        if (blockPattern & CBP_Y2)
            IDCTIndex++;
        if (blockPattern & CBP_Y3)
            IDCTIndex++;
        if (blockPattern & (CBP_CR | CBP_CB))
            IDCTIndex += 2;
        blockPattern = pMacroblock->overflowCodedBlockPattern;
    }

    if ((blockPattern & (CBP_Y0 | CBP_Y1 | CBP_Y2 | CBP_Y3)) == (CBP_Y0 | CBP_Y1 | CBP_Y2 | CBP_Y3)) {

        IDCTIndex += 4;

    } else {

        if (blockPattern & CBP_Y0)
            IDCTIndex++;

        if (blockPattern & CBP_Y1)
            IDCTIndex++;

        if (blockPattern & CBP_Y2)
            IDCTIndex++;

        if (blockPattern & CBP_Y3)
            IDCTIndex++;
    }

#ifdef  DEBUG
    if (moCompDebugLevel < 3) {
#endif  // DEBUG

    // Chroma IDCTs are always frame based

    if (blockPattern & (CBP_CB | CBP_CR)) {

        nvMoCompParams.correctionField = 0;
        nvMoCompParams.destinationField = 0;
        nvMoCompParams.blockWidth = 32;
        nvMoCompParams.blockHeight = 32;
        nvMoCompParams.blockX = pMacroblock->hOffset << 1;
        nvMoCompParams.blockY = chromaVOffset + (pMacroblock->vOffset << 1);
        nvMoCompParams.correctionY = (IDCTIndex - firstAlignedIndex) << 2; // IDCT blocks are 4 16 byte scanlines high

        if (pMacroblock->macroblockType != MB_INTRA)           
            nvDoFrameCorrection();
        else
            nvDoIntraFrameCorrection();
    }

#ifdef  DEBUG
    }
#endif  // DEBUG

} /* nvCorrectCelsiusFrameChromaMacroblock */


/*
 * nvDoAllCelsiusIntraDVDFieldCorrections
 *
 * Performs field correction on all blocks within a macroblock
 *
 * Note: All destination geometry coordinates are in quarter pel units.
 *       All texture coordinates are in quarter pel units.
 *
 */
void __stdcall nvDoAllCelsiusIntraDVDFieldCorrections(long correctionYIncr, long numBlocks)
{
    // Texture1 points to a field full of 0x80s
    long                        texture1BlockX0 = 0;
    long                        texture1BlockY0 = 0;
    long                        texture1BlockX1;
    long                        texture1BlockY1;
    long                        texture0BlockX0 = 0;
    long                        texture0BlockY0 = (nvMoCompParams.correctionY << 2); // Assumes textures with 8 byte pitch
    long                        texture0BlockX1;
    long                        texture0BlockY1;
    long                        blockX0;
    long                        blockX1;
    long                        blockY0;
    long                        blockY1;
    long                        yIncr = correctionYIncr << 18;
    long                        nextBlockYOffset = 32 << 16;
    
    if (nvMoCompParams.correctionField != 0)
        texture0BlockX0 = nvMoCompParams.blockWidth; // Start on 2nd scanline of block

    texture1BlockX1 = nvMoCompParams.blockWidth;
    texture1BlockY1 = nvMoCompParams.blockHeight;
    texture1BlockY1 <<= 16;

    texture0BlockX1 = texture0BlockX0 + nvMoCompParams.blockWidth;
    texture0BlockY1 = texture0BlockY0 + nvMoCompParams.blockHeight;
    texture0BlockY1 <<= 16;
    texture0BlockY0 <<= 16;

    blockX0 = nvMoCompParams.blockX - nvMoCompParams.celsiusDestinationViewportXOffset;

    if (nvMoCompParams.destinationField != 0) 
        blockX0 += nvMoCompParams.celsiusFieldXOffset;
    
    blockY0 = nvMoCompParams.blockY;
    blockY0 <<= 16;
    blockX1 = blockX0 + nvMoCompParams.blockWidth;
    blockX1 &= 0x0000FFFF;
    blockX0 &= 0x0000FFFF;
    blockY1 = nvMoCompParams.blockY + nvMoCompParams.blockHeight;
    blockY1 <<= 16;
    
    // Change texture1 surface offset to point to block of half bias values (0x80)
    nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_TEXTURE_OFFSET(1) | 0x40000);
    nvPushData(1, nvMoCompParams.surfaceOffset + nvMoCompParams.surfaceBiasedIntraBlockOffset);

    // Change texture1 pitch to 16 bytes
    nvPushData(2, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_TEXTURE_CONTROL1(1) | 0x40000);
    nvPushData(3, 16 << 16); // half bias Intra block pitch

    nvPushData(4, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_BEGIN_END | 0x40000);
    nvPushData(5, NV056_SET_BEGIN_END_OP_QUADS);

    if (numBlocks == 2)
        nvPushData(6, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                      NV056_INLINE_ARRAY(0) | 0x600000);
    else
        nvPushData(6, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                      NV056_INLINE_ARRAY(0) | 0xC00000);
    
    nvPushMoCompQuadData(7);

    texture0BlockY0 += yIncr;
    texture0BlockY1 += yIncr;
    blockX0 += 32;
    blockX1 += 32;
    blockX1 &= 0x0000FFFF;
    blockX0 &= 0x0000FFFF;
    
    nvPushMoCompQuadData(19);

    nvPusherAdjust(31);

    if (numBlocks == 4) {

        texture0BlockY0 += yIncr;
        texture0BlockY1 += yIncr;
        blockX0 -= 32;
        blockX1 -= 32;
        blockX1 &= 0x0000FFFF;
        blockX0 &= 0x0000FFFF;
        blockY0 += nextBlockYOffset;
        blockY1 += nextBlockYOffset;

        nvPushMoCompQuadData(0);

        texture0BlockY0 += yIncr;
        texture0BlockY1 += yIncr;
        blockX0 += 32;
        blockX1 += 32;
        blockX1 &= 0x0000FFFF;
        blockX0 &= 0x0000FFFF;

        nvPushMoCompQuadData(12);

        nvPusherAdjust(24);
    }

    nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                   NV056_SET_BEGIN_END | 0x40000);
    nvPushData(1, NV056_SET_BEGIN_END_OP_END);

    nvPusherAdjust(2);

#ifdef  DEBUG
    if (moCompDebugLevel > 1)
        nvPusherStart(TRUE);
#endif  // DEBUG

} /* nvDoAllCelsiusIntraDVDFieldCorrections */



/*
 * nvDoAllCelsiusDVDFieldCorrections
 *
 * Performs field correction on all blocks within a macroblock
 *
 * Note: All destination geometry coordinates are in quarter pel units.
 *       All texture coordinates are in quarter pel units.
 *
 */
void __stdcall nvDoAllCelsiusDVDFieldCorrections(long correctionYIncr, long numBlocks)
{
    // Texture1 and the destination surface are the same surface
    long texture1BlockX0;
    long texture1BlockY0;
    long texture1BlockX1;
    long texture1BlockY1;
    long texture0BlockX0 = 0;
    long texture0BlockY0 = nvMoCompParams.correctionY << 2; // Assumes textures with 8 byte pitch
    long texture0BlockX1;
    long texture0BlockY1;
    long blockX0;
    long blockX1;
    long blockY0;
    long blockY1;
    long yIncr = correctionYIncr << 18;
    long nextBlockYOffset = 32 << 16;
    
    texture1BlockX0 = nvMoCompParams.blockX;

    blockX0 = nvMoCompParams.blockX - nvMoCompParams.celsiusDestinationViewportXOffset;

    if (nvMoCompParams.destinationField != 0) {
        texture1BlockX0 += nvMoCompParams.celsiusFieldXOffset;
        blockX0 += nvMoCompParams.celsiusFieldXOffset;
    }
    
    texture1BlockY0 = nvMoCompParams.blockY;
    texture1BlockY0 <<= 16;
    texture1BlockX1 = texture1BlockX0 + nvMoCompParams.blockWidth;
    texture1BlockY1 = nvMoCompParams.blockY + nvMoCompParams.blockHeight;
    texture1BlockY1 <<= 16;

    if (nvMoCompParams.correctionField != 0)
        texture0BlockX0 = nvMoCompParams.blockWidth; // Start on 2nd scanline of block

    texture0BlockX1 = texture0BlockX0 + nvMoCompParams.blockWidth;
    texture0BlockY1 = texture0BlockY0 + nvMoCompParams.blockHeight;
    texture0BlockY1 <<= 16;
    texture0BlockY0 <<= 16;
    
    blockY0 = nvMoCompParams.blockY;
    blockY0 <<= 16;
    blockX1 = blockX0 + nvMoCompParams.blockWidth;
    blockX1 &= 0x0000FFFF;
    blockX0 &= 0x0000FFFF;
    blockY1 = nvMoCompParams.blockY + nvMoCompParams.blockHeight;
    blockY1 <<= 16;
    
    nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_BEGIN_END | 0x40000);
    nvPushData(1, NV056_SET_BEGIN_END_OP_QUADS);

    if (numBlocks == 2)
        nvPushData(2, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                      NV056_INLINE_ARRAY(0) | 0x600000);
    else
        nvPushData(2, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                      NV056_INLINE_ARRAY(0) | 0xC00000);

    nvPushMoCompQuadData(3);

    texture1BlockX0 += 32;
    texture1BlockX1 += 32;
    texture0BlockY0 += yIncr;
    texture0BlockY1 += yIncr;
    blockX0 += 32;
    blockX1 += 32;
    blockX1 &= 0x0000FFFF;
    blockX0 &= 0x0000FFFF;
    
    nvPushMoCompQuadData(15);

    nvPusherAdjust(27);

    if (numBlocks == 4) {

        texture1BlockX0 -= 32;
        texture1BlockX1 -= 32;
        texture1BlockY0 += nextBlockYOffset;
        texture1BlockY1 += nextBlockYOffset;
        texture0BlockY0 += yIncr;
        texture0BlockY1 += yIncr;
        blockX0 -= 32;
        blockX1 -= 32;
        blockX1 &= 0x0000FFFF;
        blockX0 &= 0x0000FFFF;
        blockY0 += nextBlockYOffset;
        blockY1 += nextBlockYOffset;

        nvPushMoCompQuadData(0);

        texture1BlockX0 += 32;
        texture1BlockX1 += 32;
        texture0BlockY0 += yIncr;
        texture0BlockY1 += yIncr;
        blockX0 += 32;
        blockX1 += 32;
        blockX1 &= 0x0000FFFF;
        blockX0 &= 0x0000FFFF;

        nvPushMoCompQuadData(12);

        nvPusherAdjust(24);
    }
    
    nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_BEGIN_END | 0x40000);
    nvPushData(1, NV056_SET_BEGIN_END_OP_END);

    nvPusherAdjust(2);

#ifdef  DEBUG
    if (moCompDebugLevel > 1)
        nvPusherStart(TRUE);
#endif  // DEBUG

} /* nvDoAllCelsiusDVDFieldCorrections */


/*
 * nvDoCelsiusIntraDVDFieldCorrection
 *
 * Performs field correction on a macroblock
 *
 * Note: All destination geometry coordinates are in quarter pel units.
 *       All texture coordinates are in quarter pel units.
 *
 */
void __stdcall nvDoCelsiusIntraDVDFieldCorrection()
{
    // Texture1 points to a field full of 0x80s
    long                        texture1BlockX0 = 0;
    long                        texture1BlockY0 = 0;
    long                        texture1BlockX1;
    long                        texture1BlockY1;
    long                        texture0BlockX0 = 0;
    long                        texture0BlockY0 = (nvMoCompParams.correctionY << 2); // Assumes textures with 8 byte pitch
    long                        texture0BlockX1;
    long                        texture0BlockY1;
    long                        blockX0;
    long                        blockX1;
    long                        blockY0;
    long                        blockY1;

    if (nvMoCompParams.correctionField != 0)
        texture0BlockX0 = nvMoCompParams.blockWidth; // Start on 2nd scanline of block

    texture1BlockX1 = nvMoCompParams.blockWidth;
    texture1BlockY1 = nvMoCompParams.blockHeight;
    texture1BlockY1 <<= 16;

    texture0BlockX1 = texture0BlockX0 + nvMoCompParams.blockWidth;
    texture0BlockY1 = texture0BlockY0 + nvMoCompParams.blockHeight;
    texture0BlockY1 <<= 16;
    texture0BlockY0 <<= 16;

    blockX0 = nvMoCompParams.blockX - nvMoCompParams.celsiusDestinationViewportXOffset;

    if (nvMoCompParams.destinationField != 0)
        blockX0 += nvMoCompParams.celsiusFieldXOffset;
    
    blockY0 = nvMoCompParams.blockY;
    blockY0 <<= 16;
    blockX1 = blockX0 + nvMoCompParams.blockWidth;
    blockX1 &= 0x0000FFFF;
    blockX0 &= 0x0000FFFF;
    blockY1 = nvMoCompParams.blockY + nvMoCompParams.blockHeight;
    blockY1 <<= 16;

    // Change texture1 surface offset to point to block of half bias values (0x80)
    nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_TEXTURE_OFFSET(1) | 0x40000);
    nvPushData(1, nvMoCompParams.surfaceOffset + nvMoCompParams.surfaceBiasedIntraBlockOffset);

    // Change texture1 pitch to 16 bytes
    nvPushData(2, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_TEXTURE_CONTROL1(1) | 0x40000);
    nvPushData(3, 16 << 16); // half bias Intra block pitch

    nvPusherAdjust(4);
    
    nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_BEGIN_END | 0x40000);
    nvPushData(1, NV056_SET_BEGIN_END_OP_QUADS);

    nvPushData(2, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_INLINE_ARRAY(0) | 0x300000);

    nvPushMoCompQuadData(3);

    nvPushData(15, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                   NV056_SET_BEGIN_END | 0x40000);
    nvPushData(16, NV056_SET_BEGIN_END_OP_END);

    nvPusherAdjust(17);

#ifdef  DEBUG
    if (moCompDebugLevel > 1)
        nvPusherStart(TRUE);
#endif  // DEBUG

} /* nvDoCelsiusIntraDVDFieldCorrection */


/*
 * nvDoOptimizedCelsiusIntraDVDFieldCorrection
 *
 * Performs field correction on a macroblock
 *
 * Note: All destination geometry coordinates are in quarter pel units.
 *       All texture coordinates are in quarter pel units.
 *
 */
void __stdcall nvDoOptimizedCelsiusIntraDVDFieldCorrection()
{
    // Texture1 points to a field full of 0x80s
    long                        texture1BlockX0 = 0;
    long                        texture1BlockY0 = 0;
    long                        texture1BlockX1;
    long                        texture1BlockY1;
    long                        texture0BlockX0 = 0;
    long                        texture0BlockY0 = (nvMoCompParams.correctionY << 2); // Assumes textures with 8 byte pitch
    long                        texture0BlockX1;
    long                        texture0BlockY1;
    long                        blockX0;
    long                        blockX1;
    long                        blockY0;
    long                        blockY1;

    if (nvMoCompParams.correctionField != 0)
        texture0BlockX0 = nvMoCompParams.blockWidth; // Start on 2nd scanline of block

    texture1BlockX1 = nvMoCompParams.blockWidth;
    texture1BlockY1 = nvMoCompParams.blockHeight;
    texture1BlockY1 <<= 16;

    texture0BlockX1 = texture0BlockX0 + nvMoCompParams.blockWidth;
    texture0BlockY1 = texture0BlockY0 + nvMoCompParams.blockHeight;
    texture0BlockY1 <<= 16;
    texture0BlockY0 <<= 16;

    blockX0 = nvMoCompParams.blockX - nvMoCompParams.celsiusDestinationViewportXOffset;

    if (nvMoCompParams.destinationField != 0)
        blockX0 += nvMoCompParams.celsiusFieldXOffset;
    
    blockY0 = nvMoCompParams.blockY;
    blockY0 <<= 16;
    blockX1 = blockX0 + nvMoCompParams.blockWidth;
    blockX1 &= 0x0000FFFF;
    blockX0 &= 0x0000FFFF;
    blockY1 = nvMoCompParams.blockY + nvMoCompParams.blockHeight;
    blockY1 <<= 16;

    nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_BEGIN_END | 0x40000);
    nvPushData(1, NV056_SET_BEGIN_END_OP_QUADS);

    nvPushData(2, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_INLINE_ARRAY(0) | 0x300000);

    nvPushMoCompQuadData(3);

    nvPushData(15, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                   NV056_SET_BEGIN_END | 0x40000);
    nvPushData(16, NV056_SET_BEGIN_END_OP_END);

    nvPusherAdjust(17);

#ifdef  DEBUG
    if (moCompDebugLevel > 1)
        nvPusherStart(TRUE);
#endif  // DEBUG

} /* nvDoOptimizedCelsiusIntraDVDFieldCorrection */


/*
 * nvDoCelsiusDVDFieldCorrection
 *
 * Performs field correction on a macroblock
 *
 * Note: All destination geometry coordinates are in quarter pel units.
 *       All texture coordinates are in quarter pel units.
 *
 */
void __stdcall nvDoCelsiusDVDFieldCorrection(void)
{
    // Texture1 and the destination surface are the same surface
    long texture1BlockX0;
    long texture1BlockY0;
    long texture1BlockX1;
    long texture1BlockY1;
    long texture0BlockX0 = 0;
    long texture0BlockY0 = nvMoCompParams.correctionY << 2; // Assumes textures with 8 byte pitch
    long texture0BlockX1;
    long texture0BlockY1;
    long blockX0;
    long blockX1;
    long blockY0;
    long blockY1;
    
    texture1BlockX0 = nvMoCompParams.blockX;

    blockX0 = nvMoCompParams.blockX - nvMoCompParams.celsiusDestinationViewportXOffset;

    if (nvMoCompParams.destinationField != 0) {
        texture1BlockX0 += nvMoCompParams.celsiusFieldXOffset;
        blockX0 += nvMoCompParams.celsiusFieldXOffset;
    }
    
    texture1BlockY0 = nvMoCompParams.blockY;
    texture1BlockY0 <<= 16;
    texture1BlockX1 = texture1BlockX0 + nvMoCompParams.blockWidth;
    texture1BlockY1 = nvMoCompParams.blockY + nvMoCompParams.blockHeight;
    texture1BlockY1 <<= 16;

    if (nvMoCompParams.correctionField != 0)
        texture0BlockX0 = nvMoCompParams.blockWidth; // Start on 2nd scanline of block

    texture0BlockX1 = texture0BlockX0 + nvMoCompParams.blockWidth;
    texture0BlockY1 = texture0BlockY0 + nvMoCompParams.blockHeight;
    texture0BlockY1 <<= 16;
    texture0BlockY0 <<= 16;

    blockY0 = nvMoCompParams.blockY;
    blockY0 <<= 16;
    blockX1 = blockX0 + nvMoCompParams.blockWidth;
    blockX1 &= 0x0000FFFF;
    blockX0 &= 0x0000FFFF;
    blockY1 = nvMoCompParams.blockY + nvMoCompParams.blockHeight;
    blockY1 <<= 16;

    nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_BEGIN_END | 0x40000);
    nvPushData(1, NV056_SET_BEGIN_END_OP_QUADS);

    nvPushData(2, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_INLINE_ARRAY(0) | 0x300000);

    nvPushMoCompQuadData(3);

    nvPushData(15, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                   NV056_SET_BEGIN_END | 0x40000);
    nvPushData(16, NV056_SET_BEGIN_END_OP_END);

    nvPusherAdjust(17);

#ifdef  DEBUG
    if (moCompDebugLevel > 1)
        nvPusherStart(TRUE);
#endif  // DEBUG

} /* nvDoCelsiusDVDFieldCorrection */



/*
 * nvDoAllCelsiusIntraHDFieldCorrections
 *
 * Performs field correction on all blocks within a macroblock
 *
 * Note: All destination geometry coordinates are in quarter pel units.
 *       All texture coordinates are in quarter pel units.
 *
 */
void __stdcall nvDoAllCelsiusIntraHDFieldCorrections(long correctionYIncr, long numBlocks)
{
    // Texture1 points to a field full of 0x80s
    long                        texture1BlockX0 = 0;
    long                        texture1BlockY0 = 0;
    long                        texture1BlockX1;
    long                        texture1BlockY1;
    long                        texture0BlockX0 = 0;
    long                        texture0BlockY0 = (nvMoCompParams.correctionY << 2); // Assumes textures with 8 byte pitch
    long                        texture0BlockX1;
    long                        texture0BlockY1;
    long                        blockX0;
    long                        blockX1;
    long                        blockY0;
    long                        blockY1;
    long                        yIncr = correctionYIncr << 18;
    long                        nextBlockYOffset = 32 << 16;
    
    if (nvMoCompParams.correctionField != 0)
        texture0BlockX0 = nvMoCompParams.blockWidth; // Start on 2nd scanline of block

    texture1BlockX1 = nvMoCompParams.blockWidth;
    texture1BlockY1 = nvMoCompParams.blockHeight;
    texture1BlockY1 <<= 16;

    texture0BlockX1 = texture0BlockX0 + nvMoCompParams.blockWidth;
    texture0BlockY1 = texture0BlockY0 + nvMoCompParams.blockHeight;
    texture0BlockY1 <<= 16;
    texture0BlockY0 <<= 16;

    blockX0 = nvMoCompParams.blockX - nvMoCompParams.celsiusDestinationViewportXOffset;

    if (nvMoCompParams.destinationField != 0)
        blockX0 += nvMoCompParams.celsiusFieldXOffset;

    blockY0 = nvMoCompParams.blockY;
    blockY0 <<= 16;
    blockX1 = blockX0 + nvMoCompParams.blockWidth;
    blockX1 &= 0x0000FFFF;
    blockX0 &= 0x0000FFFF;
    blockY1 = nvMoCompParams.blockY + nvMoCompParams.blockHeight;
    blockY1 <<= 16;
    
    // Change texture1 surface offset to point to block of half bias values (0x80)
    nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_TEXTURE_OFFSET(1) | 0x40000);
    nvPushData(1, nvMoCompParams.surfaceOffset + nvMoCompParams.surfaceBiasedIntraBlockOffset);

    // Change texture1 pitch to 16 bytes
    nvPushData(2, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_TEXTURE_CONTROL1(1) | 0x40000);
    nvPushData(3, 16 << 16); // half bias Intra block pitch

    nvPushData(4, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_BEGIN_END | 0x40000);
    nvPushData(5, NV056_SET_BEGIN_END_OP_QUADS);

    if (numBlocks == 2)
        nvPushData(6, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                      NV056_INLINE_ARRAY(0) | 0x600000);
    else
        nvPushData(6, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                      NV056_INLINE_ARRAY(0) | 0xC00000);
    
    nvPushMoCompQuadData(7);

    texture0BlockY0 += yIncr;
    texture0BlockY1 += yIncr;
    blockX0 += 32;
    blockX1 += 32;
    blockX1 &= 0x0000FFFF;
    blockX0 &= 0x0000FFFF;
    
    nvPushMoCompQuadData(19);

    nvPusherAdjust(31);

    if (numBlocks == 4) {

        texture0BlockY0 += yIncr;
        texture0BlockY1 += yIncr;
        blockX0 -= 32;
        blockX1 -= 32;
        blockX1 &= 0x0000FFFF;
        blockX0 &= 0x0000FFFF;
        blockY0 += nextBlockYOffset;
        blockY1 += nextBlockYOffset;

        nvPushMoCompQuadData(0);

        texture0BlockY0 += yIncr;
        texture0BlockY1 += yIncr;
        blockX0 += 32;
        blockX1 += 32;
        blockX1 &= 0x0000FFFF;
        blockX0 &= 0x0000FFFF;

        nvPushMoCompQuadData(12);

        nvPusherAdjust(24);
    }

    nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                   NV056_SET_BEGIN_END | 0x40000);
    nvPushData(1, NV056_SET_BEGIN_END_OP_END);

    nvPusherAdjust(2);

#ifdef  DEBUG
    if (moCompDebugLevel > 1)
        nvPusherStart(TRUE);
#endif  // DEBUG

} /* nvDoAllCelsiusIntraHDFieldCorrections */



/*
 * nvDoAllCelsiusHDFieldCorrections
 *
 * Performs field correction on all blocks within a macroblock
 *
 * Note: All destination geometry coordinates are in quarter pel units.
 *       All texture coordinates are in quarter pel units.
 *
 */
void __stdcall nvDoAllCelsiusHDFieldCorrections(long correctionYIncr, long numBlocks)
{
    // Texture1 and the destination surface are the same surface
    long texture1BlockX0;
    long texture1BlockY0;
    long texture1BlockX1;
    long texture1BlockY1;
    long texture0BlockX0 = 0;
    long texture0BlockY0 = nvMoCompParams.correctionY << 2; // Assumes textures with 8 byte pitch
    long texture0BlockX1;
    long texture0BlockY1;
    long blockX0;
    long blockX1;
    long blockY0;
    long blockY1;
    long yIncr = correctionYIncr << 18;
    long nextBlockYOffset = 32 << 16;
    
    texture1BlockX0 = nvMoCompParams.blockX;

    blockX0 = nvMoCompParams.blockX - nvMoCompParams.celsiusDestinationViewportXOffset;

    if (nvMoCompParams.destinationField != 0)
        blockX0 += nvMoCompParams.celsiusFieldXOffset;

    if (nvMoCompParams.destinationField != nvMoCompParams.currentDestinationField) {

        if (nvMoCompParams.destinationField != 0) {
            nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                          NV056_SET_TEXTURE_OFFSET(1) | 0x40000);
            nvPushData(1, nvMoCompParams.surfaceOffset + nvMoCompParams.surfaceLumaPitch);
        } else {
            nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                          NV056_SET_TEXTURE_OFFSET(1) | 0x40000);
            nvPushData(1, nvMoCompParams.surfaceOffset);
        }
        nvPusherAdjust(2);

        nvMoCompParams.currentDestinationField = nvMoCompParams.destinationField;
    }

    
    texture1BlockY0 = nvMoCompParams.blockY;
    texture1BlockY0 <<= 16;
    texture1BlockX1 = texture1BlockX0 + nvMoCompParams.blockWidth;
    texture1BlockY1 = nvMoCompParams.blockY + nvMoCompParams.blockHeight;
    texture1BlockY1 <<= 16;

    if (nvMoCompParams.correctionField != 0)
        texture0BlockX0 = nvMoCompParams.blockWidth; // Start on 2nd scanline of block

    texture0BlockX1 = texture0BlockX0 + nvMoCompParams.blockWidth;
    texture0BlockY1 = texture0BlockY0 + nvMoCompParams.blockHeight;
    texture0BlockY1 <<= 16;
    texture0BlockY0 <<= 16;
    
    blockY0 = nvMoCompParams.blockY;
    blockY0 <<= 16;
    blockX1 = blockX0 + nvMoCompParams.blockWidth;
    blockX1 &= 0x0000FFFF;
    blockX0 &= 0x0000FFFF;
    blockY1 = nvMoCompParams.blockY + nvMoCompParams.blockHeight;
    blockY1 <<= 16;
    
    nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_BEGIN_END | 0x40000);
    nvPushData(1, NV056_SET_BEGIN_END_OP_QUADS);

    if (numBlocks == 2)
        nvPushData(2, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                      NV056_INLINE_ARRAY(0) | 0x600000);
    else
        nvPushData(2, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                      NV056_INLINE_ARRAY(0) | 0xC00000);

    nvPushMoCompQuadData(3);

    texture1BlockX0 += 32;
    texture1BlockX1 += 32;
    texture0BlockY0 += yIncr;
    texture0BlockY1 += yIncr;
    blockX0 += 32;
    blockX1 += 32;
    blockX1 &= 0x0000FFFF;
    blockX0 &= 0x0000FFFF;
    
    nvPushMoCompQuadData(15);

    nvPusherAdjust(27);

    if (numBlocks == 4) {

        texture1BlockX0 -= 32;
        texture1BlockX1 -= 32;
        texture1BlockY0 += nextBlockYOffset;
        texture1BlockY1 += nextBlockYOffset;
        texture0BlockY0 += yIncr;
        texture0BlockY1 += yIncr;
        blockX0 -= 32;
        blockX1 -= 32;
        blockX1 &= 0x0000FFFF;
        blockX0 &= 0x0000FFFF;
        blockY0 += nextBlockYOffset;
        blockY1 += nextBlockYOffset;

        nvPushMoCompQuadData(0);

        texture1BlockX0 += 32;
        texture1BlockX1 += 32;
        texture0BlockY0 += yIncr;
        texture0BlockY1 += yIncr;
        blockX0 += 32;
        blockX1 += 32;
        blockX1 &= 0x0000FFFF;
        blockX0 &= 0x0000FFFF;

        nvPushMoCompQuadData(12);

        nvPusherAdjust(24);
    }
    
    nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_BEGIN_END | 0x40000);
    nvPushData(1, NV056_SET_BEGIN_END_OP_END);

    nvPusherAdjust(2);

#ifdef  DEBUG
    if (moCompDebugLevel > 1)
        nvPusherStart(TRUE);
#endif  // DEBUG

} /* nvDoAllCelsiusHDFieldCorrections */


/*
 * nvDoCelsiusIntraHDFieldCorrection
 *
 * Performs field correction on a macroblock
 *
 * Note: All destination geometry coordinates are in quarter pel units.
 *       All texture coordinates are in quarter pel units.
 *
 */
void __stdcall nvDoCelsiusIntraHDFieldCorrection()
{
    // Texture1 points to a field full of 0x80s
    long                        texture1BlockX0 = 0;
    long                        texture1BlockY0 = 0;
    long                        texture1BlockX1;
    long                        texture1BlockY1;
    long                        texture0BlockX0 = 0;
    long                        texture0BlockY0 = (nvMoCompParams.correctionY << 2); // Assumes textures with 8 byte pitch
    long                        texture0BlockX1;
    long                        texture0BlockY1;
    long                        blockX0;
    long                        blockX1;
    long                        blockY0;
    long                        blockY1;

    if (nvMoCompParams.correctionField != 0)
        texture0BlockX0 = nvMoCompParams.blockWidth; // Start on 2nd scanline of block

    texture1BlockX1 = nvMoCompParams.blockWidth;
    texture1BlockY1 = nvMoCompParams.blockHeight;
    texture1BlockY1 <<= 16;

    texture0BlockX1 = texture0BlockX0 + nvMoCompParams.blockWidth;
    texture0BlockY1 = texture0BlockY0 + nvMoCompParams.blockHeight;
    texture0BlockY1 <<= 16;
    texture0BlockY0 <<= 16;

    blockX0 = nvMoCompParams.blockX - nvMoCompParams.celsiusDestinationViewportXOffset;

    if (nvMoCompParams.destinationField != 0)
        blockX0 += nvMoCompParams.celsiusFieldXOffset;

    blockY0 = nvMoCompParams.blockY;
    blockY0 <<= 16;
    blockX1 = blockX0 + nvMoCompParams.blockWidth;
    blockX1 &= 0x0000FFFF;
    blockX0 &= 0x0000FFFF;
    blockY1 = nvMoCompParams.blockY + nvMoCompParams.blockHeight;
    blockY1 <<= 16;

    // Change texture1 surface offset to point to block of half bias values (0x80)
    nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_TEXTURE_OFFSET(1) | 0x40000);
    nvPushData(1, nvMoCompParams.surfaceOffset + nvMoCompParams.surfaceBiasedIntraBlockOffset);

    // Change texture1 pitch to 16 bytes
    nvPushData(2, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_TEXTURE_CONTROL1(1) | 0x40000);
    nvPushData(3, 16 << 16); // half bias Intra block pitch

    nvPusherAdjust(4);
    
    nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_BEGIN_END | 0x40000);
    nvPushData(1, NV056_SET_BEGIN_END_OP_QUADS);

    nvPushData(2, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_INLINE_ARRAY(0) | 0x300000);

    nvPushMoCompQuadData(3);

    nvPushData(15, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                   NV056_SET_BEGIN_END | 0x40000);
    nvPushData(16, NV056_SET_BEGIN_END_OP_END);

    nvPusherAdjust(17);

#ifdef  DEBUG
    if (moCompDebugLevel > 1)
        nvPusherStart(TRUE);
#endif  // DEBUG

} /* nvDoCelsiusIntraHDFieldCorrection */


/*
 * nvDoOptimizedCelsiusIntraHDFieldCorrection
 *
 * Performs field correction on a macroblock
 *
 * Note: All destination geometry coordinates are in quarter pel units.
 *       All texture coordinates are in quarter pel units.
 *
 */
void __stdcall nvDoOptimizedCelsiusIntraHDFieldCorrection()
{
    // Texture1 points to a field full of 0x80s
    long                        texture1BlockX0 = 0;
    long                        texture1BlockY0 = 0;
    long                        texture1BlockX1;
    long                        texture1BlockY1;
    long                        texture0BlockX0 = 0;
    long                        texture0BlockY0 = (nvMoCompParams.correctionY << 2); // Assumes textures with 8 byte pitch
    long                        texture0BlockX1;
    long                        texture0BlockY1;
    long                        blockX0;
    long                        blockX1;
    long                        blockY0;
    long                        blockY1;

    if (nvMoCompParams.correctionField != 0)
        texture0BlockX0 = nvMoCompParams.blockWidth; // Start on 2nd scanline of block

    texture1BlockX1 = nvMoCompParams.blockWidth;
    texture1BlockY1 = nvMoCompParams.blockHeight;
    texture1BlockY1 <<= 16;

    texture0BlockX1 = texture0BlockX0 + nvMoCompParams.blockWidth;
    texture0BlockY1 = texture0BlockY0 + nvMoCompParams.blockHeight;
    texture0BlockY1 <<= 16;
    texture0BlockY0 <<= 16;

    blockX0 = nvMoCompParams.blockX - nvMoCompParams.celsiusDestinationViewportXOffset;

    if (nvMoCompParams.destinationField != 0)
        blockX0 += nvMoCompParams.celsiusFieldXOffset;

    blockY0 = nvMoCompParams.blockY;
    blockY0 <<= 16;
    blockX1 = blockX0 + nvMoCompParams.blockWidth;
    blockX1 &= 0x0000FFFF;
    blockX0 &= 0x0000FFFF;
    blockY1 = nvMoCompParams.blockY + nvMoCompParams.blockHeight;
    blockY1 <<= 16;

    nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_BEGIN_END | 0x40000);
    nvPushData(1, NV056_SET_BEGIN_END_OP_QUADS);

    nvPushData(2, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_INLINE_ARRAY(0) | 0x300000);

    nvPushMoCompQuadData(3);

    nvPushData(15, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                   NV056_SET_BEGIN_END | 0x40000);
    nvPushData(16, NV056_SET_BEGIN_END_OP_END);

    nvPusherAdjust(17);

#ifdef  DEBUG
    if (moCompDebugLevel > 1)
        nvPusherStart(TRUE);
#endif  // DEBUG

} /* nvDoOptimizedCelsiusIntraHDFieldCorrection */


/*
 * nvDoCelsiusHDFieldCorrection
 *
 * Performs field correction on a macroblock
 *
 * Note: All destination geometry coordinates are in quarter pel units.
 *       All texture coordinates are in quarter pel units.
 *
 */
void __stdcall nvDoCelsiusHDFieldCorrection(void)
{
    // Texture1 and the destination surface are the same surface
    long texture1BlockX0;
    long texture1BlockY0;
    long texture1BlockX1;
    long texture1BlockY1;
    long texture0BlockX0 = 0;
    long texture0BlockY0 = nvMoCompParams.correctionY << 2; // Assumes textures with 8 byte pitch
    long texture0BlockX1;
    long texture0BlockY1;
    long blockX0;
    long blockX1;
    long blockY0;
    long blockY1;
    
    texture1BlockX0 = nvMoCompParams.blockX;

    blockX0 = nvMoCompParams.blockX - nvMoCompParams.celsiusDestinationViewportXOffset;

    if (nvMoCompParams.destinationField != 0)
        blockX0 += nvMoCompParams.celsiusFieldXOffset;

    if (nvMoCompParams.destinationField != nvMoCompParams.currentDestinationField) {

        if (nvMoCompParams.destinationField != 0) {
            nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                          NV056_SET_TEXTURE_OFFSET(1) | 0x40000);
            nvPushData(1, nvMoCompParams.surfaceOffset + nvMoCompParams.surfaceLumaPitch);
        } else {
            nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                          NV056_SET_TEXTURE_OFFSET(1) | 0x40000);
            nvPushData(1, nvMoCompParams.surfaceOffset);
        }
        nvPusherAdjust(2);

        nvMoCompParams.currentDestinationField = nvMoCompParams.destinationField;
    }
    
    texture1BlockY0 = nvMoCompParams.blockY;
    texture1BlockY0 <<= 16;
    texture1BlockX1 = texture1BlockX0 + nvMoCompParams.blockWidth;
    texture1BlockY1 = nvMoCompParams.blockY + nvMoCompParams.blockHeight;
    texture1BlockY1 <<= 16;

    if (nvMoCompParams.correctionField != 0)
        texture0BlockX0 = nvMoCompParams.blockWidth; // Start on 2nd scanline of block

    texture0BlockX1 = texture0BlockX0 + nvMoCompParams.blockWidth;
    texture0BlockY1 = texture0BlockY0 + nvMoCompParams.blockHeight;
    texture0BlockY1 <<= 16;
    texture0BlockY0 <<= 16;

    blockY0 = nvMoCompParams.blockY;
    blockY0 <<= 16;
    blockX1 = blockX0 + nvMoCompParams.blockWidth;
    blockX1 &= 0x0000FFFF;
    blockX0 &= 0x0000FFFF;
    blockY1 = nvMoCompParams.blockY + nvMoCompParams.blockHeight;
    blockY1 <<= 16;

    nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_BEGIN_END | 0x40000);
    nvPushData(1, NV056_SET_BEGIN_END_OP_QUADS);

    nvPushData(2, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_INLINE_ARRAY(0) | 0x300000);

    nvPushMoCompQuadData(3);

    nvPushData(15, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                   NV056_SET_BEGIN_END | 0x40000);
    nvPushData(16, NV056_SET_BEGIN_END_OP_END);

    nvPusherAdjust(17);

#ifdef  DEBUG
    if (moCompDebugLevel > 1)
        nvPusherStart(TRUE);
#endif  // DEBUG

} /* nvDoCelsiusHDFieldCorrection */



/*
 * nvCorrectCelsiusIntraField
 *
 * Processes LUMA blocks in an IntraField then makes a 2nd pass and processes chroma blocks
 *
 * Note: All destination geometry sent to correction functions is in quarter pel units.  This is to avoid
 *       the need to update the composite transform as we alternate between prediction and correction.
 *       All correction (IDCT) texture geometry is in quarter pel units.
 *
 *       Due to celcius class limitations, the maximum post-transformed texture y coordinate is 2047 which
 *       corresponds to a maximum (IDCTIndex - firstAlignedIDCTIndex) value of 255 assuming all blocks are 8 texels high (by 8 texels wide).
 *
 */
void __stdcall nvCorrectCelsiusIntraField(LPNVDECODEMACROBLOCK pFirstMacroblock, long macroblockCount)
{
    unsigned long               firstAlignedIndex = pDriverData->dwMCFirstAlignedIDCTIndex;
    LPNVDECODEMACROBLOCK        pMacroblock;
    unsigned long               IDCTIndex;
    unsigned long               dwSurfaceOffset;
    long                        chromaVOffset;
    BYTE                        blockPattern;
    // Texture1 points to a field full of 0x80s
    long                        texture1BlockX0 = 0;
    long                        texture1BlockY0 = 0;
    long                        texture1BlockX1 = 64;
    long                        texture1BlockY1 = 32 << 16;
    long                        texture0BlockX0 = 0;
    long                        texture0BlockY0;
    long                        texture0BlockX1 = 64;
    long                        texture0BlockY1;
    long                        blockX0;
    long                        blockX1;
    long                        blockY0;
    long                        blockY1;
    int                         n;
    int                         i;
    
    chromaVOffset = nvMoCompParams.surfaceLumaHeight << 1;

    // Change texture1 surface offset to point to block of half bias values (0x80)
    nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_TEXTURE_OFFSET(1) | 0x40000);
    nvPushData(1, nvMoCompParams.surfaceOffset + nvMoCompParams.surfaceBiasedIntraBlockOffset);

    // Change texture1 pitch to 16 bytes
    nvPushData(2, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                  NV056_SET_TEXTURE_CONTROL1(1) | 0x40000);
    nvPushData(3, 16 << 16); // half bias Intra block pitch

    nvPusherAdjust(4);
    
    if (pDriverData->bMCPictureStructure == PICTURE_STRUCTURE_TOP_FIELD)
        nvMoCompParams.destinationField = 0;
    else    
        nvMoCompParams.destinationField = 1;

    pMacroblock = pFirstMacroblock;
    n = macroblockCount;

    while (--n >= 0) {
    
        IDCTIndex = pMacroblock->firstIDCTIndex;
        blockPattern = pMacroblock->codedBlockPattern;

        if (pMacroblock->DCTType == DCT_FIELD) {
            if (blockPattern >= 0xFC) {
                nvMoCompParams.blockWidth = 64;
                nvMoCompParams.blockHeight = 16;
                nvMoCompParams.blockX = pMacroblock->hOffset << 2;

                nvMoCompParams.correctionField = 0;
                nvMoCompParams.blockY = pMacroblock->vOffset << 2;
                nvMoCompParams.correctionY = (IDCTIndex - firstAlignedIndex) << 2; // IDCT blocks are 4 16-byte wide scanlines high

                nvDoOptimizedIntraFieldCorrection();
                
                nvMoCompParams.correctionField = 1;
                nvMoCompParams.blockY += 16;

                nvDoOptimizedIntraFieldCorrection();

                // IDCTIndex += 2;
                
                nvMoCompParams.correctionField = 0;
                nvMoCompParams.blockY += 16;
                nvMoCompParams.correctionY += 8; // (IDCTIndex - firstAlignedIndex) << 2; // IDCT blocks are 4 16-byte wide scanlines high

                nvDoOptimizedIntraFieldCorrection();
                
                nvMoCompParams.correctionField = 1;
                nvMoCompParams.blockY += 16;

                nvDoOptimizedIntraFieldCorrection();
                
            } else {
                nvMoCompParams.blockWidth = 32;
                nvMoCompParams.blockHeight = 16;
                nvMoCompParams.blockX = pMacroblock->hOffset << 2;
                nvMoCompParams.correctionY = (IDCTIndex - firstAlignedIndex) << 2; // IDCT blocks are 4 16-byte wide scanlines high

                nvMoCompParams.correctionField = 0;
                nvMoCompParams.blockY = pMacroblock->vOffset << 2;

                nvDoAllIntraFieldCorrections(8, 4);
                
                nvMoCompParams.correctionField = 1;
                nvMoCompParams.blockY += 16;

                nvDoAllIntraFieldCorrections(8, 4);
            }
        } else {
            if (blockPattern >= 0xFC) {
                nvMoCompParams.correctionField = 0;
                nvMoCompParams.blockWidth = 64;
                nvMoCompParams.blockHeight = 64;
                nvMoCompParams.blockX = pMacroblock->hOffset << 2;
                nvMoCompParams.blockY = pMacroblock->vOffset << 2;
                nvMoCompParams.correctionY = (IDCTIndex - firstAlignedIndex) << 2; // IDCT blocks are 4 16 byte scanlines high
                
                nvDoOptimizedIntraFieldCorrection();
            } else {
                nvMoCompParams.blockWidth = 32;
                nvMoCompParams.blockHeight = 32;

                // Temporarily change texture0 pitch to 8 bytes so that each scanline in IDCT block will be used
                nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                               NV056_SET_TEXTURE_CONTROL1(0) | 0x40000);
                nvPushData(1, 8 << 16);

                nvPusherAdjust(2);
                
                nvMoCompParams.correctionField = 0;
                nvMoCompParams.blockX = pMacroblock->hOffset << 2;
                nvMoCompParams.blockY = pMacroblock->vOffset << 2;
                nvMoCompParams.correctionY = (IDCTIndex - firstAlignedIndex) << 3; // IDCT blocks are 8 8 byte blocks high

                nvDoOptimizedIntraFieldCorrection();

                // IDCTIndex++;

                nvMoCompParams.blockX += 32;
                nvMoCompParams.correctionY += 8; // (IDCTIndex - firstAlignedIndex) << 3; // IDCT blocks are 8 8 byte blocks high

                nvDoOptimizedIntraFieldCorrection();

                IDCTIndex++;

                nvMoCompParams.blockX -= 32;
                nvMoCompParams.blockY += 32;
                nvMoCompParams.correctionY += 8; // (IDCTIndex - firstAlignedIndex) << 3; // IDCT blocks are 8 8 byte scanlines high

                nvDoOptimizedIntraFieldCorrection();

                IDCTIndex++;

                nvMoCompParams.blockX += 32;
                nvMoCompParams.correctionY += 8; // (IDCTIndex - firstAlignedIndex) << 3; // IDCT blocks are 8 8 byte scanlines high

                nvDoOptimizedIntraFieldCorrection();

                // Restore texture0 pitch to it's default value of 16 bytes
                nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                               NV056_SET_TEXTURE_CONTROL1(0) | 0x40000);
                nvPushData(1, 16 << 16);

                nvPusherAdjust(2);
            }    
        }
        
        pMacroblock++;
    }
    
    nvPusherStart(TRUE);

    // Chroma IDCTs are always frame based

    dwSurfaceOffset = 0; // Assume destination field 0
    
    if (nvMoCompParams.destinationField != 0)
        dwSurfaceOffset += nvMoCompParams.surfaceLumaPitch << 2; // Geometry is referenced in quarter pel coords

    pMacroblock = pFirstMacroblock;
    n = macroblockCount;

    while (n >= 8) {

        n -= 8;
        
        nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                      NV056_SET_BEGIN_END | 0x40000);
        nvPushData(1, NV056_SET_BEGIN_END_OP_QUADS);

        nvPushData(2, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                      NV056_INLINE_ARRAY(0) | 0x1800000);
        
        i = 3;
        
        while (i < 99) {        
            IDCTIndex = pMacroblock->firstIDCTIndex + 4;

            texture0BlockY0 = (IDCTIndex - firstAlignedIndex) << 4; // Assumes textures with 8 byte pitch
            texture0BlockY1 = texture0BlockY0 + 32;
            texture0BlockY1 <<= 16;
            texture0BlockY0 <<= 16;

            nvMoCompParams.blockY = chromaVOffset + (pMacroblock->vOffset << 1);

            blockX0 = dwSurfaceOffset + (pMacroblock->hOffset << 2) - nvMoCompParams.celsiusDestinationViewportXOffset;
            blockY0 = nvMoCompParams.blockY;
            blockY0 <<= 16;
            blockX1 = blockX0 + 64;
            blockX1 &= 0x0000FFFF;
            blockX0 &= 0x0000FFFF;
            blockY1 = nvMoCompParams.blockY + 32;
            blockY1 <<= 16;

            nvPushMoCompQuadData(i);
            
            i += 12;

            pMacroblock++;
        }
        
        nvPushData(i, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                       NV056_SET_BEGIN_END | 0x40000);
        i++;

        nvPushData(i, NV056_SET_BEGIN_END_OP_END);

        i++;
    
        nvPusherAdjust(i);
    }

    while (--n >= 0) {
    
        IDCTIndex = pMacroblock->firstIDCTIndex + 4;

        texture0BlockY0 = (IDCTIndex - firstAlignedIndex) << 4; // Assumes textures with 8 byte pitch
        texture0BlockY1 = texture0BlockY0 + 32;
        texture0BlockY1 <<= 16;
        texture0BlockY0 <<= 16;

        nvMoCompParams.blockY = chromaVOffset + (pMacroblock->vOffset << 1);

        blockX0 = dwSurfaceOffset + (pMacroblock->hOffset << 2) - nvMoCompParams.celsiusDestinationViewportXOffset;
        blockY0 = nvMoCompParams.blockY;
        blockY0 <<= 16;
        blockX1 = blockX0 + 64;
        blockX1 &= 0x0000FFFF;
        blockX0 &= 0x0000FFFF;
        blockY1 = nvMoCompParams.blockY + 32;
        blockY1 <<= 16;

        nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                      NV056_SET_BEGIN_END | 0x40000);
        nvPushData(1, NV056_SET_BEGIN_END_OP_QUADS);

        nvPushData(2, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                      NV056_INLINE_ARRAY(0) | 0x300000);

        nvPushMoCompQuadData(3);

        nvPushData(15, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                       NV056_SET_BEGIN_END | 0x40000);
        nvPushData(16, NV056_SET_BEGIN_END_OP_END);

        nvPusherAdjust(17);

        pMacroblock++;
    }

} /* nvCorrectCelsiusIntraField */


/*
 * nvCorrectCelsiusFieldMacroblock
 *
 * Determines which blocks in the block pattern need correction and calls the appropriate function
 *
 * Note: All destination geometry sent to correction functions is in quarter pel units.  This is to avoid
 *       the need to update the composite transform as we alternate between prediction and correction.
 *       All correction (IDCT) texture geometry is in quarter pel units.
 *
 *       Due to celcius class limitations, the maximum post-transformed texture y coordinate is 2047 which
 *       corresponds to a maximum (IDCTIndex - firstAlignedIDCTIndex) value of 511 assuming all blocks are 4 texels high (by 16 texels wide with 8 texels per field).
 *
 */
void __stdcall nvCorrectCelsiusFieldMacroblock(LPNVDECODEMACROBLOCK pMacroblock, DWORD correctionType)
{
    unsigned long               IDCTIndex = pMacroblock->firstIDCTIndex;
    unsigned long               firstAlignedIndex = pDriverData->dwMCFirstAlignedIDCTIndex;
    long                        chromaVOffset;
    BYTE                        blockPattern;
    
    chromaVOffset = nvMoCompParams.surfaceLumaHeight << 1;

    if (correctionType == CORRECTION_FIRST_PASS) {

        blockPattern = pMacroblock->codedBlockPattern;

    } else {

        blockPattern = pMacroblock->codedBlockPattern;
        if (blockPattern & CBP_Y0)
            IDCTIndex++;
        if (blockPattern & CBP_Y1)
            IDCTIndex++;
        if (blockPattern & CBP_Y2)
            IDCTIndex++;
        if (blockPattern & CBP_Y3)
            IDCTIndex++;
        if (blockPattern & (CBP_CR | CBP_CB))
            IDCTIndex += 2;
        blockPattern = pMacroblock->overflowCodedBlockPattern;
    }

    if (pDriverData->bMCPictureStructure == PICTURE_STRUCTURE_TOP_FIELD)
        nvMoCompParams.destinationField = 0;
    else    
        nvMoCompParams.destinationField = 1;

    if (pMacroblock->macroblockType != MB_INTRA) {          
        if  ((blockPattern >= 0xFC) || 
            ((pMacroblock->DCTType == DCT_FIELD) &&
            ((blockPattern & (CBP_Y0 | CBP_Y1 | CBP_Y2 | CBP_Y3)) == (CBP_Y0 | CBP_Y1 | CBP_Y2 | CBP_Y3)))) {

            if (pMacroblock->DCTType == DCT_FIELD) {
                if (blockPattern >= 0xFC) {
                    nvMoCompParams.blockWidth = 64;
                    nvMoCompParams.blockHeight = 16;
                    nvMoCompParams.blockX = pMacroblock->hOffset << 2;
                    nvMoCompParams.correctionY = (IDCTIndex - firstAlignedIndex) << 2; // IDCT blocks are 4 16-byte wide scanlines high

                    nvMoCompParams.correctionField = 0;
                    nvMoCompParams.blockY = pMacroblock->vOffset << 2;

                    nvDoFieldCorrection();
                    
                    nvMoCompParams.correctionField = 1;
                    nvMoCompParams.blockY += 16;

                    nvDoFieldCorrection();

                    // IDCTIndex += 2;
                    
                    nvMoCompParams.correctionY += 8; // (IDCTIndex - firstAlignedIndex) << 2; // IDCT blocks are 4 16-byte wide scanlines high

                    nvMoCompParams.correctionField = 0;
                    nvMoCompParams.blockY += 16;

                    nvDoFieldCorrection();

                    nvMoCompParams.correctionField = 1;
                    nvMoCompParams.blockY += 16;

                    nvDoFieldCorrection();
                    
                } else {
                    nvMoCompParams.blockWidth = 32;
                    nvMoCompParams.blockHeight = 16;
                    nvMoCompParams.blockX = pMacroblock->hOffset << 2;
                    nvMoCompParams.correctionY = (IDCTIndex - firstAlignedIndex) << 2; // IDCT blocks are 4 16-byte wide scanlines high

                    nvMoCompParams.correctionField = 0;
                    nvMoCompParams.blockY = pMacroblock->vOffset << 2;

                    nvDoAllFieldCorrections(8, 4);
                    
                    nvMoCompParams.correctionField = 1;
                    nvMoCompParams.blockY += 16;

                    nvDoAllFieldCorrections(8, 4);
                }
            } else {
                nvMoCompParams.correctionField = 0;
                nvMoCompParams.blockWidth = 64;
                nvMoCompParams.blockHeight = 64;
                nvMoCompParams.blockX = pMacroblock->hOffset << 2;
                nvMoCompParams.blockY = pMacroblock->vOffset << 2;
                nvMoCompParams.correctionY = (IDCTIndex - firstAlignedIndex) << 2; // IDCT blocks are 4 16 byte scanlines high
                
                nvDoFieldCorrection();
            }
            
            IDCTIndex += 4;

        } else {

            if (pMacroblock->DCTType == DCT_FRAME) {
                // Temporarily change texture0 pitch to 8 bytes so that each scanline in IDCT block will be used
                nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                               NV056_SET_TEXTURE_CONTROL1(0) | 0x40000);
                nvPushData(1, 8 << 16);

                nvPusherAdjust(2);

                nvMoCompParams.blockWidth = 32;
                nvMoCompParams.blockHeight = 32;
            } else {
                nvMoCompParams.blockWidth = 32;
                nvMoCompParams.blockHeight = 16;
            }

            if (blockPattern & CBP_Y0) {
                nvMoCompParams.correctionField = 0;
                nvMoCompParams.blockX = pMacroblock->hOffset << 2;
                nvMoCompParams.blockY = pMacroblock->vOffset << 2;

                if (pMacroblock->DCTType == DCT_FRAME) {
                    nvMoCompParams.correctionY = (IDCTIndex - firstAlignedIndex) << 3; // IDCT blocks are 8 8 byte blocks high
                    nvDoFieldCorrection();
                } else {
                    nvMoCompParams.correctionY = (IDCTIndex - firstAlignedIndex) << 2; // IDCT blocks are 4 16-byte wide scanlines high
                    nvDoFieldCorrection();
                    nvMoCompParams.correctionField = 1;
                    nvMoCompParams.blockY += 16;
                    nvDoFieldCorrection();
                }

                IDCTIndex++;
            }

            if (blockPattern & CBP_Y1) {
                nvMoCompParams.correctionField = 0;
                nvMoCompParams.blockX = (pMacroblock->hOffset + 8) << 2;
                nvMoCompParams.blockY = pMacroblock->vOffset << 2;

                if (pMacroblock->DCTType == DCT_FRAME) {
                    nvMoCompParams.correctionY = (IDCTIndex - firstAlignedIndex) << 3; // IDCT blocks are 8 8 byte blocks high
                    nvDoFieldCorrection();
                } else {
                    nvMoCompParams.correctionY = (IDCTIndex - firstAlignedIndex) << 2; // IDCT blocks are 4 16 byte scanlines high
                    nvDoFieldCorrection();
                    nvMoCompParams.correctionField = 1;
                    nvMoCompParams.blockY = 16;
                    nvDoFieldCorrection();
                }            

                IDCTIndex++;
            }

            if (blockPattern & CBP_Y2) {
                nvMoCompParams.correctionField = 0;
                nvMoCompParams.blockX = pMacroblock->hOffset << 2;
                nvMoCompParams.blockY = (pMacroblock->vOffset + 8) << 2;

                if (pMacroblock->DCTType == DCT_FRAME) {
                    nvMoCompParams.correctionY = (IDCTIndex - firstAlignedIndex) << 3; // IDCT blocks are 8 8 byte scanlines high
                    nvDoFieldCorrection();
                } else {
                    nvMoCompParams.correctionY = (IDCTIndex - firstAlignedIndex) << 2; // IDCT blocks are 4 16 byte scanlines high
                    nvDoFieldCorrection();
                    nvMoCompParams.correctionField = 1;
                    nvMoCompParams.blockY += 16;
                    nvDoFieldCorrection();
                }

                IDCTIndex++;
            }

            if (blockPattern & CBP_Y3) {
                nvMoCompParams.correctionField = 0;
                nvMoCompParams.blockX = (pMacroblock->hOffset + 8) << 2;
                nvMoCompParams.blockY = (pMacroblock->vOffset + 8) << 2;

                if (pMacroblock->DCTType == DCT_FRAME) {
                    nvMoCompParams.correctionY = (IDCTIndex - firstAlignedIndex) << 3; // IDCT blocks are 8 8 byte scanlines high
                    nvDoFieldCorrection();
                } else {
                    nvMoCompParams.correctionY = (IDCTIndex - firstAlignedIndex) << 2; // IDCT blocks are 4 16 byte scanlines high
                    nvDoFieldCorrection();
                    nvMoCompParams.correctionField = 1;
                    nvMoCompParams.blockY += 16;
                    nvDoFieldCorrection();
                }

                IDCTIndex++;
            }

            if (pMacroblock->DCTType == DCT_FRAME) {
                // Restore texture0 pitch to it's default value of 16 bytes
                nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                               NV056_SET_TEXTURE_CONTROL1(0) | 0x40000);
                nvPushData(1, 16 << 16);

                nvPusherAdjust(2);
            }
        }
        
#ifdef  DEBUG
        if (moCompDebugLevel < 3) {
#endif  // DEBUG

        // Chroma IDCTs are always frame based

        if (blockPattern & (CBP_CB | CBP_CR)) {

            nvMoCompParams.correctionField = 0;
            nvMoCompParams.blockWidth = 64;
            nvMoCompParams.blockHeight = 32;
            nvMoCompParams.blockX = pMacroblock->hOffset << 2;
            nvMoCompParams.blockY = chromaVOffset + (pMacroblock->vOffset << 1);
            nvMoCompParams.correctionY = (IDCTIndex - firstAlignedIndex) << 2; // IDCT blocks are 4 16 byte scanlines high

            nvDoFieldCorrection();
        }

#ifdef  DEBUG
        }
#endif  // DEBUG

    } else { // MB_INTRA

        // Change texture1 surface offset to point to block of half bias values (0x80)
        nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                      NV056_SET_TEXTURE_OFFSET(1) | 0x40000);
        nvPushData(1, nvMoCompParams.surfaceOffset + nvMoCompParams.surfaceBiasedIntraBlockOffset);

        // Change texture1 pitch to 16 bytes
        nvPushData(2, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                      NV056_SET_TEXTURE_CONTROL1(1) | 0x40000);
        nvPushData(3, 16 << 16); // half bias Intra block pitch

        nvPusherAdjust(4);
        
        if  ((blockPattern >= 0xFC) || 
            ((pMacroblock->DCTType == DCT_FIELD) &&
            ((blockPattern & (CBP_Y0 | CBP_Y1 | CBP_Y2 | CBP_Y3)) == (CBP_Y0 | CBP_Y1 | CBP_Y2 | CBP_Y3)))) {

            if (pMacroblock->DCTType == DCT_FIELD) {
                if (blockPattern >= 0xFC) {
                    nvMoCompParams.blockWidth = 64;
                    nvMoCompParams.blockHeight = 16;
                    nvMoCompParams.blockX = pMacroblock->hOffset << 2;
                    nvMoCompParams.correctionY = (IDCTIndex - firstAlignedIndex) << 2; // IDCT blocks are 4 16-byte wide scanlines high

                    nvMoCompParams.correctionField = 0;
                    nvMoCompParams.blockY = pMacroblock->vOffset << 2;

                    nvDoOptimizedIntraFieldCorrection();
                    
                    nvMoCompParams.correctionField = 1;
                    nvMoCompParams.blockY += 16;

                    nvDoOptimizedIntraFieldCorrection();

                    // IDCTIndex += 2;
                    
                    nvMoCompParams.correctionY += 8; // (IDCTIndex - firstAlignedIndex) << 2; // IDCT blocks are 4 16-byte wide scanlines high

                    nvMoCompParams.correctionField = 0;
                    nvMoCompParams.blockY += 16;

                    nvDoOptimizedIntraFieldCorrection();
                    
                    nvMoCompParams.correctionField = 1;
                    nvMoCompParams.blockY += 16;

                    nvDoOptimizedIntraFieldCorrection();
                    
                } else {
                    nvMoCompParams.blockWidth = 32;
                    nvMoCompParams.blockHeight = 16;
                    nvMoCompParams.blockX = pMacroblock->hOffset << 2;
                    nvMoCompParams.correctionY = (IDCTIndex - firstAlignedIndex) << 2; // IDCT blocks are 4 16-byte wide scanlines high

                    nvMoCompParams.correctionField = 0;
                    nvMoCompParams.blockY = pMacroblock->vOffset << 2;

                    nvDoAllIntraFieldCorrections(8, 4);
                    
                    nvMoCompParams.correctionField = 1;
                    nvMoCompParams.blockY += 16;

                    nvDoAllIntraFieldCorrections(8, 4);
                }
            } else {
                nvMoCompParams.correctionField = 0;
                nvMoCompParams.blockWidth = 64;
                nvMoCompParams.blockHeight = 64;
                nvMoCompParams.blockX = pMacroblock->hOffset << 2;
                nvMoCompParams.blockY = pMacroblock->vOffset << 2;
                nvMoCompParams.correctionY = (IDCTIndex - firstAlignedIndex) << 2; // IDCT blocks are 4 16 byte scanlines high
                
                nvDoOptimizedIntraFieldCorrection();
            }
            
            IDCTIndex += 4;

        } else {

            if (pMacroblock->DCTType == DCT_FRAME) {
                // Temporarily change texture0 pitch to 8 bytes so that each scanline in IDCT block will be used
                nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                               NV056_SET_TEXTURE_CONTROL1(0) | 0x40000);
                nvPushData(1, 8 << 16);

                nvPusherAdjust(2);

                nvMoCompParams.blockWidth = 32;
                nvMoCompParams.blockHeight = 32;
            } else {
                nvMoCompParams.blockWidth = 32;
                nvMoCompParams.blockHeight = 16;
            }
    
            if (blockPattern & CBP_Y0) {
                nvMoCompParams.correctionField = 0;
                nvMoCompParams.blockX = pMacroblock->hOffset << 2;
                nvMoCompParams.blockY = pMacroblock->vOffset << 2;

                if (pMacroblock->DCTType == DCT_FRAME) {
                    nvMoCompParams.correctionY = (IDCTIndex - firstAlignedIndex) << 3; // IDCT blocks are 8 8 byte blocks high
                    nvDoOptimizedIntraFieldCorrection();
                } else {
                    nvMoCompParams.correctionY = (IDCTIndex - firstAlignedIndex) << 2; // IDCT blocks are 4 16-byte wide scanlines high
                    nvDoOptimizedIntraFieldCorrection();
                    nvMoCompParams.correctionField = 1;
                    nvMoCompParams.blockY += 16;
                    nvDoOptimizedIntraFieldCorrection();
                }

                IDCTIndex++;
            }

            if (blockPattern & CBP_Y1) {
                nvMoCompParams.correctionField = 0;
                nvMoCompParams.blockX = (pMacroblock->hOffset + 8) << 2;
                nvMoCompParams.blockY = pMacroblock->vOffset << 2;

                if (pMacroblock->DCTType == DCT_FRAME) {
                    nvMoCompParams.correctionY = (IDCTIndex - firstAlignedIndex) << 3; // IDCT blocks are 8 8 byte blocks high
                    nvDoOptimizedIntraFieldCorrection();
                } else {
                    nvMoCompParams.correctionY = (IDCTIndex - firstAlignedIndex) << 2; // IDCT blocks are 4 16 byte scanlines high
                    nvDoOptimizedIntraFieldCorrection();
                    nvMoCompParams.correctionField = 1;
                    nvMoCompParams.blockY = 16;
                    nvDoOptimizedIntraFieldCorrection();
                }            

                IDCTIndex++;
            }

            if (blockPattern & CBP_Y2) {
                nvMoCompParams.correctionField = 0;
                nvMoCompParams.blockX = pMacroblock->hOffset << 2;
                nvMoCompParams.blockY = (pMacroblock->vOffset + 8) << 2;

                if (pMacroblock->DCTType == DCT_FRAME) {
                    nvMoCompParams.correctionY = (IDCTIndex - firstAlignedIndex) << 3; // IDCT blocks are 8 8 byte scanlines high
                    nvDoOptimizedIntraFieldCorrection();
                } else {
                    nvMoCompParams.correctionY = (IDCTIndex - firstAlignedIndex) << 2; // IDCT blocks are 4 16 byte scanlines high
                    nvDoOptimizedIntraFieldCorrection();
                    nvMoCompParams.correctionField = 1;
                    nvMoCompParams.blockY += 16;
                    nvDoOptimizedIntraFieldCorrection();
                }

                IDCTIndex++;
            }

            if (blockPattern & CBP_Y3) {
                nvMoCompParams.correctionField = 0;
                nvMoCompParams.blockX = (pMacroblock->hOffset + 8) << 2;
                nvMoCompParams.blockY = (pMacroblock->vOffset + 8) << 2;

                if (pMacroblock->DCTType == DCT_FRAME) {
                    nvMoCompParams.correctionY = (IDCTIndex - firstAlignedIndex) << 3; // IDCT blocks are 8 8 byte scanlines high
                    nvDoOptimizedIntraFieldCorrection();
                } else {
                    nvMoCompParams.correctionY = (IDCTIndex - firstAlignedIndex) << 2; // IDCT blocks are 4 16 byte scanlines high
                    nvDoOptimizedIntraFieldCorrection();
                    nvMoCompParams.correctionField = 1;
                    nvMoCompParams.blockY += 16;
                    nvDoOptimizedIntraFieldCorrection();
                }

                IDCTIndex++;
            }

            if (pMacroblock->DCTType == DCT_FRAME) {
                // Restore texture0 pitch to it's default value of 16 bytes
                nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                               NV056_SET_TEXTURE_CONTROL1(0) | 0x40000);
                nvPushData(1, 16 << 16);

                nvPusherAdjust(2);
            }
        
        }
        
    #ifdef  DEBUG
        if (moCompDebugLevel < 3) {
    #endif  // DEBUG

        // Chroma IDCTs are always frame based

        if (blockPattern & (CBP_CB | CBP_CR)) {

            nvMoCompParams.correctionField = 0;
            nvMoCompParams.blockWidth = 64;
            nvMoCompParams.blockHeight = 32;
            nvMoCompParams.blockX = pMacroblock->hOffset << 2;
            nvMoCompParams.blockY = chromaVOffset + (pMacroblock->vOffset << 1);
            nvMoCompParams.correctionY = (IDCTIndex - firstAlignedIndex) << 2; // IDCT blocks are 4 16 byte scanlines high

            nvDoOptimizedIntraFieldCorrection();
        }

#ifdef  DEBUG
        }
#endif  // DEBUG
    }
} /* nvCorrectCelsiusFieldMacroblock */


#endif  // NVARCH >= 0x10
