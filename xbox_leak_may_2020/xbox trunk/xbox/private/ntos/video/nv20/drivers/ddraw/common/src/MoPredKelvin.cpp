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
 *  File:       MoPredKelvin.cpp
 *  Content:    Windows98 DirectDraw 32 bit driver
 *
 ***************************************************************************/
#include "nvprecomp.h"

#if (NVARCH >= 0x10)

extern NVMOCOMPPARAMS nvMoCompParams;

extern PNVMCPREDFUNC nvDoBidirectionalFieldPrediction;
extern PNVMCPREDFUNC nvDoTwoBidirectionalFieldPredictions;
extern PNVMCPREDFUNC nvDoForwardFieldPrediction;
extern PNVMCPREDFUNC nvDoTwoForwardFieldPredictions;
extern PNVMCPREDFUNC nvDoBackwardFieldPrediction;
extern PNVMCPREDFUNC nvDoTwoBackwardFieldPredictions;

#ifdef  DEBUG
extern unsigned long moCompDebugLevel;
#endif  // DEBUG


/*
 * nvDoKelvinBidirectionalDVDFieldPrediction
 *
 * Performs bidirectional field prediction on a macroblock
 *
 * Note: All geometry coordinates are in quarter pel units
 *
 */
void __stdcall nvDoKelvinBidirectionalDVDFieldPrediction(BOOL isChroma)
{
    float           fBuffer = 0.5;
    unsigned long   *ftol = (unsigned long *)&fBuffer;
    unsigned long   dwSurfacePitch;
    unsigned long   dwSurfaceOffset = 0;
    long            texture1BlockX0;
    long            texture1BlockY0;
    long            texture1BlockX1;
    long            texture1BlockY1;
    long            texture0BlockX0;
    long            texture0BlockY0;
    long            texture0BlockX1;
    long            texture0BlockY1;
    long            blockX0;
    long            blockX1;
    long            blockY0;
    long            blockY1;
    
    SET_KELVIN_BIDIRECTIONAL_VERTEX_ARRAY_MODE();

    texture0BlockX0 = nvMoCompParams.blockX + nvMoCompParams.forwardHorizontalVector;

    texture0BlockY0 = nvMoCompParams.blockY + nvMoCompParams.forwardVerticalVector;

    texture1BlockX0 = nvMoCompParams.blockX + nvMoCompParams.backwardHorizontalVector;

    texture1BlockY0 = nvMoCompParams.blockY + nvMoCompParams.backwardVerticalVector;

    blockX0 = nvMoCompParams.blockX;

    if (isChroma)
        dwSurfacePitch = nvMoCompParams.surfaceLumaPitch << 1;
    else
        dwSurfacePitch = nvMoCompParams.surfaceLumaPitch << 2;

    if (nvMoCompParams.forwardField != 0)
        texture0BlockX0 += dwSurfacePitch;

    if (nvMoCompParams.backwardField != 0)
        texture1BlockX0 += dwSurfacePitch;
    
    if (nvMoCompParams.destinationField != 0) 
        blockX0 += dwSurfacePitch;
    
    texture1BlockX1 = texture1BlockX0 + nvMoCompParams.blockWidth;
    texture1BlockY1 = texture1BlockY0 + nvMoCompParams.blockHeight;
    texture1BlockY1 <<= 16;
    texture1BlockY0 <<= 16;

    texture0BlockX1 = texture0BlockX0 + nvMoCompParams.blockWidth;
    texture0BlockY1 = texture0BlockY0 + nvMoCompParams.blockHeight;
    texture0BlockY1 <<= 16;
    texture0BlockY0 <<= 16;

    blockY0 = nvMoCompParams.blockY;
    blockY0 <<= 16;
    blockX1 = blockX0 + nvMoCompParams.blockWidth;
    blockY1 = nvMoCompParams.blockY + nvMoCompParams.blockHeight;
    blockY1 <<= 16;

    nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_BEGIN_END | 0x40000);
    nvPushData(1, NV097_SET_BEGIN_END_OP_QUADS);

    nvPushData(2, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_FOG1F | 0x40000);
    nvPushData(3, ftol[0]);

    nvPushData(4, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_INLINE_ARRAY | 0x40300000);

    nvPushKelvinMoCompQuadData(5);

    nvPushData(17, dDrawSubchannelOffset(NV_DD_KELVIN) +
                   NV097_SET_BEGIN_END | 0x40000);
    nvPushData(18, NV097_SET_BEGIN_END_OP_END);

    nvPusherAdjust(19);

#ifdef  DEBUG
    if (moCompDebugLevel > 1)
        nvPusherStart(TRUE);
#endif  // DEBUG

} /* nvDoKelvinBidirectionalDVDFieldPrediction */


/*
 * nvDoTwoKelvinBidirectionalDVDFieldPredictions
 *
 * Performs two bidirectional field predictions on a macroblock
 *
 * Note: All geometry coordinates are in quarter pel units
 *
 */
void __stdcall nvDoTwoKelvinBidirectionalDVDFieldPredictions(BOOL isChroma)
{
    float           fBuffer = 0.5;
    unsigned long   *ftol = (unsigned long *)&fBuffer;
    unsigned long   dwSurfacePitch;
    long            texture1BlockX0;
    long            texture1BlockY0;
    long            texture1BlockX1;
    long            texture1BlockY1;
    long            texture0BlockX0;
    long            texture0BlockY0;
    long            texture0BlockX1;
    long            texture0BlockY1;
    long            blockX0;
    long            blockX1;
    long            blockY0;
    long            blockY1;
    
    SET_KELVIN_BIDIRECTIONAL_VERTEX_ARRAY_MODE();

    if (isChroma)
        dwSurfacePitch = nvMoCompParams.surfaceLumaPitch << 1;
    else
        dwSurfacePitch = nvMoCompParams.surfaceLumaPitch << 2;

    texture0BlockX0 = nvMoCompParams.blockX + nvMoCompParams.forwardHorizontalVector;

    texture0BlockY0 = nvMoCompParams.blockY + nvMoCompParams.forwardVerticalVector;

    texture1BlockX0 = nvMoCompParams.blockX + nvMoCompParams.backwardHorizontalVector;

    texture1BlockY0 = nvMoCompParams.blockY + nvMoCompParams.backwardVerticalVector;

    if (nvMoCompParams.forwardField != 0)
        texture0BlockX0 += dwSurfacePitch;

    if (nvMoCompParams.backwardField != 0)
        texture1BlockX0 += dwSurfacePitch;
    
    blockX0 = nvMoCompParams.blockX;

    if (nvMoCompParams.destinationField != 0) 
        blockX0 += dwSurfacePitch;
    
    texture1BlockX1 = texture1BlockX0 + nvMoCompParams.blockWidth;
    texture1BlockY1 = texture1BlockY0 + nvMoCompParams.blockHeight;
    texture1BlockY1 <<= 16;
    texture1BlockY0 <<= 16;

    texture0BlockX1 = texture0BlockX0 + nvMoCompParams.blockWidth;
    texture0BlockY1 = texture0BlockY0 + nvMoCompParams.blockHeight;
    texture0BlockY1 <<= 16;
    texture0BlockY0 <<= 16;

    blockY0 = nvMoCompParams.blockY;
    blockY0 <<= 16;
    blockX1 = blockX0 + nvMoCompParams.blockWidth;
    blockY1 = nvMoCompParams.blockY + nvMoCompParams.blockHeight;
    blockY1 <<= 16;

    nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_BEGIN_END | 0x40000);
    nvPushData(1, NV097_SET_BEGIN_END_OP_QUADS);

    nvPushData(2, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_FOG1F | 0x40000);
    nvPushData(3, ftol[0]);

    nvPushData(4, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_INLINE_ARRAY | 0x40600000);

    nvPushKelvinMoCompQuadData(5);

    texture0BlockX0 = nvMoCompParams.blockX + nvMoCompParams.forwardHorizontalVector2;

    texture0BlockY0 = nvMoCompParams.blockY2 + nvMoCompParams.forwardVerticalVector2;

    texture1BlockX0 = nvMoCompParams.blockX + nvMoCompParams.backwardHorizontalVector2;

    texture1BlockY0 = nvMoCompParams.blockY2 + nvMoCompParams.backwardVerticalVector2;

    if (nvMoCompParams.forwardField2 != 0)
        texture0BlockX0 += dwSurfacePitch;

    if (nvMoCompParams.backwardField2 != 0)
        texture1BlockX0 += dwSurfacePitch;
    
    blockX0 = nvMoCompParams.blockX;

    if (nvMoCompParams.destinationField2 != 0) 
        blockX0 += dwSurfacePitch;
    
    texture1BlockX1 = texture1BlockX0 + nvMoCompParams.blockWidth;
    texture1BlockY1 = texture1BlockY0 + nvMoCompParams.blockHeight;
    texture1BlockY1 <<= 16;
    texture1BlockY0 <<= 16;

    texture0BlockX1 = texture0BlockX0 + nvMoCompParams.blockWidth;
    texture0BlockY1 = texture0BlockY0 + nvMoCompParams.blockHeight;
    texture0BlockY1 <<= 16;
    texture0BlockY0 <<= 16;

    blockY0 = nvMoCompParams.blockY2;
    blockY0 <<= 16;
    blockX1 = blockX0 + nvMoCompParams.blockWidth;
    blockY1 = nvMoCompParams.blockY2 + nvMoCompParams.blockHeight;
    blockY1 <<= 16;

    nvPushKelvinMoCompQuadData(17);

    nvPushData(29, dDrawSubchannelOffset(NV_DD_KELVIN) +
                   NV097_SET_BEGIN_END | 0x40000);
    nvPushData(30, NV097_SET_BEGIN_END_OP_END);

    nvPusherAdjust(31);

#ifdef  DEBUG
    if (moCompDebugLevel > 1)
        nvPusherStart(TRUE);
#endif  // DEBUG

} /* nvDoTwoKelvinBidirectionalDVDFieldPredictions */


/*
 * nvDoKelvinForwardDVDFieldPrediction
 *
 * Performs forward field prediction on a macroblock
 *
 * Note: All geometry coordinates are in quarter pel units
 *
 */
void __stdcall nvDoKelvinForwardDVDFieldPrediction(BOOL isChroma)
{
    float           fBuffer = 0.0;
    unsigned long   *ftol = (unsigned long *)&fBuffer;
    unsigned long   dwSurfacePitch;
    unsigned long   dwSurfaceOffset = 0;
    long            texture0BlockX0;
    long            texture0BlockY0;
    long            texture0BlockX1;
    long            texture0BlockY1;
    long            blockX0;
    long            blockX1;
    long            blockY0;
    long            blockY1;
    
    SET_KELVIN_FORWARD_VERTEX_ARRAY_MODE();

    texture0BlockX0 = nvMoCompParams.blockX + nvMoCompParams.forwardHorizontalVector;

    texture0BlockY0 = nvMoCompParams.blockY + nvMoCompParams.forwardVerticalVector;

    blockX0 = nvMoCompParams.blockX;

    if (isChroma)
        dwSurfacePitch = nvMoCompParams.surfaceLumaPitch << 1;
    else
        dwSurfacePitch = nvMoCompParams.surfaceLumaPitch << 2;

    if (nvMoCompParams.forwardField != 0)
        texture0BlockX0 += dwSurfacePitch;

    if (nvMoCompParams.destinationField != 0)
        blockX0 += dwSurfacePitch;
    
    texture0BlockX1 = texture0BlockX0 + nvMoCompParams.blockWidth;
    texture0BlockY1 = texture0BlockY0 + nvMoCompParams.blockHeight;
    texture0BlockY1 <<= 16;
    texture0BlockY0 <<= 16;

    blockY0 = nvMoCompParams.blockY;
    blockY0 <<= 16;
    blockX1 = blockX0 + nvMoCompParams.blockWidth;
    blockY1 = nvMoCompParams.blockY + nvMoCompParams.blockHeight;
    blockY1 <<= 16;

    nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_BEGIN_END | 0x40000);
    nvPushData(1, NV097_SET_BEGIN_END_OP_QUADS);

    nvPushData(2, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_FOG1F | 0x40000);
    nvPushData(3, ftol[0]);

    nvPushData(4, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_INLINE_ARRAY | 0x40200000);

    nvPushData(5, blockY0 | blockX0);
    nvPushData(6, texture0BlockY0 | texture0BlockX0);
    nvPushData(7, blockY1 | blockX0);
    nvPushData(8, texture0BlockY1 | texture0BlockX0);
    nvPushData(9, blockY1 | blockX1);
    nvPushData(10, texture0BlockY1 | texture0BlockX1);
    nvPushData(11, blockY0 | blockX1);
    nvPushData(12, texture0BlockY0 | texture0BlockX1);

    nvPushData(13, dDrawSubchannelOffset(NV_DD_KELVIN) +
                   NV097_SET_BEGIN_END | 0x40000);
    nvPushData(14, NV097_SET_BEGIN_END_OP_END);

    nvPusherAdjust(15);

#ifdef  DEBUG
    if (moCompDebugLevel > 1)
        nvPusherStart(TRUE);
#endif  // DEBUG

} /* nvDoKelvinForwardDVDFieldPrediction */


/*
 * nvDoTwoKelvinForwardDVDFieldPredictions
 *
 * Performs two forward field predictions on a macroblock
 *
 * Note: All geometry coordinates are in quarter pel units
 *
 */
void __stdcall nvDoTwoKelvinForwardDVDFieldPredictions(BOOL isChroma)
{
    float           fBuffer = 0.0;
    unsigned long   *ftol = (unsigned long *)&fBuffer;
    unsigned long   dwSurfacePitch;
    long            texture0BlockX0;
    long            texture0BlockY0;
    long            texture0BlockX1;
    long            texture0BlockY1;
    long            blockX0;
    long            blockX1;
    long            blockY0;
    long            blockY1;
    
    SET_KELVIN_FORWARD_VERTEX_ARRAY_MODE();

    if (isChroma)
        dwSurfacePitch = nvMoCompParams.surfaceLumaPitch << 1;
    else
        dwSurfacePitch = nvMoCompParams.surfaceLumaPitch << 2;

    texture0BlockX0 = nvMoCompParams.blockX + nvMoCompParams.forwardHorizontalVector;

    texture0BlockY0 = nvMoCompParams.blockY + nvMoCompParams.forwardVerticalVector;

    if (nvMoCompParams.forwardField != 0)
        texture0BlockX0 += dwSurfacePitch;

    blockX0 = nvMoCompParams.blockX;

    if (nvMoCompParams.destinationField != 0)
        blockX0 += dwSurfacePitch;
    
    texture0BlockX1 = texture0BlockX0 + nvMoCompParams.blockWidth;
    texture0BlockY1 = texture0BlockY0 + nvMoCompParams.blockHeight;
    texture0BlockY1 <<= 16;
    texture0BlockY0 <<= 16;

    blockY0 = nvMoCompParams.blockY;
    blockY0 <<= 16;
    blockX1 = blockX0 + nvMoCompParams.blockWidth;
    blockY1 = nvMoCompParams.blockY + nvMoCompParams.blockHeight;
    blockY1 <<= 16;

    nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_BEGIN_END | 0x40000);
    nvPushData(1, NV097_SET_BEGIN_END_OP_QUADS);

    nvPushData(2, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_FOG1F | 0x40000);
    nvPushData(3, ftol[0]);

    nvPushData(4, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_INLINE_ARRAY | 0x40400000);

    nvPushData(5, blockY0 | blockX0);
    nvPushData(6, texture0BlockY0 | texture0BlockX0);
    nvPushData(7, blockY1 | blockX0);
    nvPushData(8, texture0BlockY1 | texture0BlockX0);
    nvPushData(9, blockY1 | blockX1);
    nvPushData(10, texture0BlockY1 | texture0BlockX1);
    nvPushData(11, blockY0 | blockX1);
    nvPushData(12, texture0BlockY0 | texture0BlockX1);

    texture0BlockX0 = nvMoCompParams.blockX + nvMoCompParams.forwardHorizontalVector2;

    texture0BlockY0 = nvMoCompParams.blockY2 + nvMoCompParams.forwardVerticalVector2;

    if (nvMoCompParams.forwardField2 != 0)
        texture0BlockX0 += dwSurfacePitch;

    blockX0 = nvMoCompParams.blockX;

    if (nvMoCompParams.destinationField2 != 0)
        blockX0 += dwSurfacePitch;
    
    texture0BlockX1 = texture0BlockX0 + nvMoCompParams.blockWidth;
    texture0BlockY1 = texture0BlockY0 + nvMoCompParams.blockHeight;
    texture0BlockY1 <<= 16;
    texture0BlockY0 <<= 16;

    blockY0 = nvMoCompParams.blockY2;
    blockY0 <<= 16;
    blockX1 = blockX0 + nvMoCompParams.blockWidth;
    blockY1 = nvMoCompParams.blockY2 + nvMoCompParams.blockHeight;
    blockY1 <<= 16;

    nvPushData(13, blockY0 | blockX0);
    nvPushData(14, texture0BlockY0 | texture0BlockX0);
    nvPushData(15, blockY1 | blockX0);
    nvPushData(16, texture0BlockY1 | texture0BlockX0);
    nvPushData(17, blockY1 | blockX1);
    nvPushData(18, texture0BlockY1 | texture0BlockX1);
    nvPushData(19, blockY0 | blockX1);
    nvPushData(20, texture0BlockY0 | texture0BlockX1);

    nvPushData(21, dDrawSubchannelOffset(NV_DD_KELVIN) +
                   NV097_SET_BEGIN_END | 0x40000);
    nvPushData(22, NV097_SET_BEGIN_END_OP_END);

    nvPusherAdjust(23);

#ifdef  DEBUG
    if (moCompDebugLevel > 1)
        nvPusherStart(TRUE);
#endif  // DEBUG

} /* nvDoTwoKelvinForwardDVDFieldPredictions */



/*
 * nvDoKelvinBackwardDVDFieldPrediction
 *
 * Performs backward field prediction on a macroblock
 *
 * Note: All geometry coordinates are in quarter pel units
 *
 */
void __stdcall nvDoKelvinBackwardDVDFieldPrediction(BOOL isChroma)
{
    float           fBuffer = 1.0;
    unsigned long   *ftol = (unsigned long *)&fBuffer;
    unsigned long   dwSurfacePitch;
    unsigned long   dwSurfaceOffset = 0;
    long            texture0BlockX0;
    long            texture0BlockY0;
    long            texture0BlockX1;
    long            texture0BlockY1;
    long            blockX0;
    long            blockX1;
    long            blockY0;
    long            blockY1;
    
    SET_KELVIN_BACKWARD_VERTEX_ARRAY_MODE();

    texture0BlockX0 = nvMoCompParams.blockX + nvMoCompParams.backwardHorizontalVector;

    texture0BlockY0 = nvMoCompParams.blockY + nvMoCompParams.backwardVerticalVector;

    blockX0 = nvMoCompParams.blockX;

    if (isChroma)
        dwSurfacePitch = nvMoCompParams.surfaceLumaPitch << 1;
    else
        dwSurfacePitch = nvMoCompParams.surfaceLumaPitch << 2;

    if (nvMoCompParams.backwardField != 0)
        texture0BlockX0 += dwSurfacePitch;

    if (nvMoCompParams.destinationField != 0)
        blockX0 += dwSurfacePitch;
    
    texture0BlockX1 = texture0BlockX0 + nvMoCompParams.blockWidth;
    texture0BlockY1 = texture0BlockY0 + nvMoCompParams.blockHeight;
    texture0BlockY1 <<= 16;
    texture0BlockY0 <<= 16;

    blockY0 = nvMoCompParams.blockY;
    blockY0 <<= 16;
    blockX1 = blockX0 + nvMoCompParams.blockWidth;
    blockY1 = nvMoCompParams.blockY + nvMoCompParams.blockHeight;
    blockY1 <<= 16;

    nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_BEGIN_END | 0x40000);
    nvPushData(1, NV097_SET_BEGIN_END_OP_QUADS);

    nvPushData(2, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_FOG1F | 0x40000);
    nvPushData(3, ftol[0]);

    nvPushData(4, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_INLINE_ARRAY | 0x40200000);

    nvPushData(5, blockY0 | blockX0);
    nvPushData(6, texture0BlockY0 | texture0BlockX0);
    nvPushData(7, blockY1 | blockX0);
    nvPushData(8, texture0BlockY1 | texture0BlockX0);
    nvPushData(9, blockY1 | blockX1);
    nvPushData(10, texture0BlockY1 | texture0BlockX1);
    nvPushData(11, blockY0 | blockX1);
    nvPushData(12, texture0BlockY0 | texture0BlockX1);

    nvPushData(13, dDrawSubchannelOffset(NV_DD_KELVIN) +
                   NV097_SET_BEGIN_END | 0x40000);
    nvPushData(14, NV097_SET_BEGIN_END_OP_END);

    nvPusherAdjust(15);

#ifdef  DEBUG
    if (moCompDebugLevel > 1)
        nvPusherStart(TRUE);
#endif  // DEBUG

} /* nvDoKelvinBackwardDVDFieldPrediction */


/*
 * nvDoTwoKelvinBackwardDVDFieldPredictions
 *
 * Performs two backward field predictions on a macroblock
 *
 * Note: All geometry coordinates are in quarter pel units
 *
 */
void __stdcall nvDoTwoKelvinBackwardDVDFieldPredictions(BOOL isChroma)
{
    float           fBuffer = 1.0;
    unsigned long   *ftol = (unsigned long *)&fBuffer;
    unsigned long   dwSurfacePitch;
    long            texture0BlockX0;
    long            texture0BlockY0;
    long            texture0BlockX1;
    long            texture0BlockY1;
    long            blockX0;
    long            blockX1;
    long            blockY0;
    long            blockY1;
    
    SET_KELVIN_BACKWARD_VERTEX_ARRAY_MODE();

    if (isChroma)
        dwSurfacePitch = nvMoCompParams.surfaceLumaPitch << 1;
    else
        dwSurfacePitch = nvMoCompParams.surfaceLumaPitch << 2;

    texture0BlockX0 = nvMoCompParams.blockX + nvMoCompParams.backwardHorizontalVector;

    texture0BlockY0 = nvMoCompParams.blockY + nvMoCompParams.backwardVerticalVector;

    if (nvMoCompParams.backwardField != 0)
        texture0BlockX0 += dwSurfacePitch;

    blockX0 = nvMoCompParams.blockX;

    if (nvMoCompParams.destinationField != 0)
        blockX0 += dwSurfacePitch;

    texture0BlockX1 = texture0BlockX0 + nvMoCompParams.blockWidth;
    texture0BlockY1 = texture0BlockY0 + nvMoCompParams.blockHeight;
    texture0BlockY1 <<= 16;
    texture0BlockY0 <<= 16;

    blockY0 = nvMoCompParams.blockY;
    blockY0 <<= 16;
    blockX1 = blockX0 + nvMoCompParams.blockWidth;
    blockY1 = nvMoCompParams.blockY + nvMoCompParams.blockHeight;
    blockY1 <<= 16;

    nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_BEGIN_END | 0x40000);
    nvPushData(1, NV097_SET_BEGIN_END_OP_QUADS);

    nvPushData(2, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_SET_FOG1F | 0x40000);
    nvPushData(3, ftol[0]);

    nvPushData(4, dDrawSubchannelOffset(NV_DD_KELVIN) +
                  NV097_INLINE_ARRAY | 0x40400000);

    nvPushData(5, blockY0 | blockX0);
    nvPushData(6, texture0BlockY0 | texture0BlockX0);
    nvPushData(7, blockY1 | blockX0);
    nvPushData(8, texture0BlockY1 | texture0BlockX0);
    nvPushData(9, blockY1 | blockX1);
    nvPushData(10, texture0BlockY1 | texture0BlockX1);
    nvPushData(11, blockY0 | blockX1);
    nvPushData(12, texture0BlockY0 | texture0BlockX1);

    texture0BlockX0 = nvMoCompParams.blockX + nvMoCompParams.backwardHorizontalVector2;

    texture0BlockY0 = nvMoCompParams.blockY2 + nvMoCompParams.backwardVerticalVector2;

    if (nvMoCompParams.backwardField2 != 0)
        texture0BlockX0 += dwSurfacePitch;

    blockX0 = nvMoCompParams.blockX;

    if (nvMoCompParams.destinationField2 != 0)
        blockX0 += dwSurfacePitch;

    texture0BlockX1 = texture0BlockX0 + nvMoCompParams.blockWidth;
    texture0BlockY1 = texture0BlockY0 + nvMoCompParams.blockHeight;
    texture0BlockY1 <<= 16;
    texture0BlockY0 <<= 16;

    blockY0 = nvMoCompParams.blockY2;
    blockY0 <<= 16;
    blockX1 = blockX0 + nvMoCompParams.blockWidth;
    blockY1 = nvMoCompParams.blockY2 + nvMoCompParams.blockHeight;
    blockY1 <<= 16;

    nvPushData(13, blockY0 | blockX0);
    nvPushData(14, texture0BlockY0 | texture0BlockX0);
    nvPushData(15, blockY1 | blockX0);
    nvPushData(16, texture0BlockY1 | texture0BlockX0);
    nvPushData(17, blockY1 | blockX1);
    nvPushData(18, texture0BlockY1 | texture0BlockX1);
    nvPushData(19, blockY0 | blockX1);
    nvPushData(20, texture0BlockY0 | texture0BlockX1);

    nvPushData(21, dDrawSubchannelOffset(NV_DD_KELVIN) +
                   NV097_SET_BEGIN_END | 0x40000);
    nvPushData(22, NV097_SET_BEGIN_END_OP_END);

    nvPusherAdjust(23);

#ifdef  DEBUG
    if (moCompDebugLevel > 1)
        nvPusherStart(TRUE);
#endif  // DEBUG

} /* nvDoTwoKelvinBackwardFieldPredictions */


#if 0
/*
 * nvDualPrimeArithmetic
 *
 * Performs new prediction motion vectors from original and differential vectors
 *
 */
void __stdcall nvDualPrimeArithmetic(short DMV[][2],
                                     short *differentialMotionVector,
                                     short horizontalMotionVector,
                                     short verticalMotionVector)
{

    if (pDriverData->bMCPictureStructure == PICTURE_STRUCTURE_FRAME) {
        if (pDriverData->bMCTopFieldFirst) {
            // Vector for prediction of top field from bottom field
            DMV[0][0] = ((horizontalMotionVector + (horizontalMotionVector > 0)) >> 1) +
                          differentialMotionVector[0];
            DMV[0][1] = ((verticalMotionVector + (verticalMotionVector > 0)) >> 1) +
                          differentialMotionVector[1] - 1;

            // Vector for prediction of bottom field from top field
            DMV[1][0] = ((3*horizontalMotionVector + (horizontalMotionVector > 0)) >> 1) +
                          differentialMotionVector[0];
            DMV[1][1] = ((3*verticalMotionVector + (verticalMotionVector > 0)) >> 1) +
                          differentialMotionVector[1] + 1;
        } else {
            // Vector for prediction of top field from bottom field
            DMV[0][0] = ((3*horizontalMotionVector + (horizontalMotionVector > 0)) >> 1) +
                          differentialMotionVector[0];
            DMV[0][1] = ((3*verticalMotionVector + (verticalMotionVector > 0)) >> 1) +
                          differentialMotionVector[1] - 1;

            // Vector for prediction of bottom field from top field
            DMV[1][0] = ((horizontalMotionVector + (horizontalMotionVector > 0)) >> 1) +
                          differentialMotionVector[0];
            DMV[1][1] = ((verticalMotionVector + (verticalMotionVector > 0)) >> 1) +
                          differentialMotionVector[1] + 1;
        }
    } else { // Vector for prediction for field of opposite 'parity'
        DMV[0][0] = ((horizontalMotionVector + (horizontalMotionVector > 0)) >> 1) +
                      differentialMotionVector[0];
        DMV[0][1] = ((verticalMotionVector + (verticalMotionVector > 0)) >> 1) +
                      differentialMotionVector[1];

        // Correct for vertical field shift
        if (pDriverData->bMCPictureStructure == PICTURE_STRUCTURE_TOP_FIELD)
            DMV[0][1]--;
        else
            DMV[0][1]++;
    }

} /* nvDualPrimeArithmetic */

#endif

/*
 * nvPredictKelvinFrameLumaMacroblock
 *
 * Determines what type of prediction to apply to the macroblock and
 * calls the appropriate function.
 *
 * Note: All geometry sent to prediction functions is in quarter pel units
 *
 */
DWORD __stdcall nvPredictKelvinFrameLumaMacroblock(LPNVDECODEMACROBLOCK pMacroblock, WORD numCombinedMacroblocks)
{
    LPDDRAWI_DDRAWSURFACE_LCL   pSurf_lcl;
//    short dmvector[2];
//    short DMV[2][2];

    SET_KELVIN_LUMA_DATA_FORMAT();
    
    // We don't currently support spatial scalability

    nvMoCompParams.blockWidth = 64 * numCombinedMacroblocks;
    nvMoCompParams.blockX = pMacroblock->hOffset << 2;

    if ((pDriverData->bMCPictureCodingType == PICTURE_TYPE_B) &&
        ((pMacroblock->macroblockType & (MB_MOTION_FORWARD | MB_MOTION_BACKWARD)) ==
        (MB_MOTION_FORWARD | MB_MOTION_BACKWARD))) {

        if (pMacroblock->motionType == FRAME_STRUCTURE_MOTION_FRAME) { // Frame based prediction

            nvMoCompParams.forwardField = 0;
            nvMoCompParams.backwardField = 0;
            nvMoCompParams.destinationField = 0;
            nvMoCompParams.blockHeight = 128;
            nvMoCompParams.blockY = pMacroblock->vOffset << 3;
            nvMoCompParams.forwardHorizontalVector = pMacroblock->PMV[0][0][0] << 1;
            nvMoCompParams.forwardVerticalVector = pMacroblock->PMV[0][0][1] << 2;
            nvMoCompParams.backwardHorizontalVector = pMacroblock->PMV[0][1][0] << 1;
            nvMoCompParams.backwardVerticalVector = pMacroblock->PMV[0][1][1] << 2;

            nvDoBidirectionalFieldPrediction(FALSE);

        } else if (pMacroblock->motionType == FRAME_STRUCTURE_MOTION_FIELD) { // Field based prediction

            nvMoCompParams.blockHeight = 64;
            nvMoCompParams.blockY = pMacroblock->vOffset << 2;

            // Top field prediction
            nvMoCompParams.forwardField = pMacroblock->fieldSelect[0][0];
            nvMoCompParams.backwardField = pMacroblock->fieldSelect[0][1];
            nvMoCompParams.destinationField = 0;
            *(long *)&nvMoCompParams.forwardHorizontalVector = (*(long *)&pMacroblock->PMV[0][0][0] << 1) & 0xFFFEFFFF;
            *(long *)&nvMoCompParams.backwardHorizontalVector = (*(long *)&pMacroblock->PMV[0][1][0] << 1) & 0xFFFEFFFF;

            nvMoCompParams.blockY2 = nvMoCompParams.blockY;

            // Bottom field prediction
            nvMoCompParams.forwardField2 = pMacroblock->fieldSelect[1][0];
            nvMoCompParams.backwardField2 = pMacroblock->fieldSelect[1][1];
            nvMoCompParams.destinationField2 = 1;
            *(long *)&nvMoCompParams.forwardHorizontalVector2 = (*(long *)&pMacroblock->PMV[1][0][0] << 1) & 0xFFFEFFFF;
            *(long *)&nvMoCompParams.backwardHorizontalVector2 = (*(long *)&pMacroblock->PMV[1][1][0] << 1) & 0xFFFEFFFF;

            nvDoTwoBidirectionalFieldPredictions(FALSE);
            
        } else {
            // Error
            return FALSE;
        }
    } else {
        if ((pMacroblock->macroblockType & MB_MOTION_FORWARD) ||
            (pDriverData->bMCPictureCodingType == PICTURE_TYPE_P)) {

            if ((pMacroblock->motionType == FRAME_STRUCTURE_MOTION_FRAME) ||
                (!(pMacroblock->macroblockType & MB_MOTION_FORWARD))) {
                
                nvMoCompParams.forwardField = 0;
                nvMoCompParams.destinationField = 0;
                nvMoCompParams.blockHeight = 128;
                nvMoCompParams.blockY = pMacroblock->vOffset << 3;
                nvMoCompParams.forwardHorizontalVector = pMacroblock->PMV[0][0][0] << 1;
                nvMoCompParams.forwardVerticalVector = pMacroblock->PMV[0][0][1] << 2;

                nvDoForwardFieldPrediction(FALSE);

            } else if (pMacroblock->motionType == FRAME_STRUCTURE_MOTION_FIELD) { // Field based prediction

                nvMoCompParams.blockHeight = 64;
                nvMoCompParams.blockY = pMacroblock->vOffset << 2;

                // Top field prediction
                nvMoCompParams.forwardField = pMacroblock->fieldSelect[0][0];
                nvMoCompParams.destinationField = 0;
                *(long *)&nvMoCompParams.forwardHorizontalVector = (*(long *)&pMacroblock->PMV[0][0][0] << 1) & 0xFFFEFFFF;

                nvMoCompParams.blockY2 = nvMoCompParams.blockY;

                // Bottom field prediction
                nvMoCompParams.forwardField2 = pMacroblock->fieldSelect[1][0];
                nvMoCompParams.destinationField2 = 1;
                *(long *)&nvMoCompParams.forwardHorizontalVector2 = (*(long *)&pMacroblock->PMV[1][0][0] << 1) & 0xFFFEFFFF;

                nvDoTwoForwardFieldPredictions(FALSE);
                
            } else if (pMacroblock->motionType == FRAME_STRUCTURE_MOTION_DP) { // Dual prime prediction

                // Calculate derived motion vectors
                // nvDualPrimeArithmetic(DMV,
                //                      dmvector,
                //                      pMacroblock->PMV[0][0][0],
                //                      (short)(pMacroblock->PMV[0][0][1] >> 1));

                pSurf_lcl = (LPDDRAWI_DDRAWSURFACE_LCL)pDriverData->dwMCDualPrimeSurface;

                nvMoCompParams.blockHeight = 64;
                nvMoCompParams.blockY = pMacroblock->vOffset << 2;

                nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                              NV097_SET_TEXTURE_OFFSET(1) | 0x40000);
                nvPushData(1, VIDMEM_OFFSET(pSurf_lcl->lpGbl->fpVidMem));

                nvPusherAdjust(2);

                // Predict top field from top field
                // and add to top field from bottom field
                nvMoCompParams.forwardField = 0;
                nvMoCompParams.backwardField = 1;
                nvMoCompParams.destinationField = 0;
                *(long *)&nvMoCompParams.forwardHorizontalVector = (*(long *)&pMacroblock->PMV[0][0][0] << 1) & 0xFFFEFFFF;
                *(long *)&nvMoCompParams.backwardHorizontalVector = (*(long *)&pMacroblock->PMV[1][0][0] << 1) & 0xFFFEFFFF;

                nvDoBidirectionalFieldPrediction(FALSE);

                // Predict bottom field from bottom field
                // and add to bottom field from top field
                nvMoCompParams.forwardField = 1;
                nvMoCompParams.backwardField = 0;
                nvMoCompParams.destinationField = 1;
                *(long *)&nvMoCompParams.forwardHorizontalVector = (*(long *)&pMacroblock->PMV[0][0][0] << 1) & 0xFFFEFFFF;
                *(long *)&nvMoCompParams.backwardHorizontalVector = (*(long *)&pMacroblock->PMV[1][1][0] << 1) & 0xFFFEFFFF;

                nvDoBidirectionalFieldPrediction(FALSE);

                pSurf_lcl = (LPDDRAWI_DDRAWSURFACE_LCL)pDriverData->dwMCBackwardSurface;

                nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                              NV097_SET_TEXTURE_OFFSET(1) | 0x40000);
                nvPushData(1, VIDMEM_OFFSET(pSurf_lcl->lpGbl->fpVidMem));

                nvPusherAdjust(2);
                
            } else {
                // Error
                return FALSE;
            }
        }

        if (pMacroblock->macroblockType & MB_MOTION_BACKWARD) {

            if (pMacroblock->motionType == FRAME_STRUCTURE_MOTION_FRAME) { // Frame based prediction

                nvMoCompParams.backwardField = 0;
                nvMoCompParams.destinationField = 0;
                nvMoCompParams.blockHeight = 128;
                nvMoCompParams.blockY = pMacroblock->vOffset << 3;
                nvMoCompParams.backwardHorizontalVector = pMacroblock->PMV[0][1][0] << 1;
                nvMoCompParams.backwardVerticalVector = pMacroblock->PMV[0][1][1] << 2;

                nvDoBackwardFieldPrediction(FALSE);

            } else if (pMacroblock->motionType == FRAME_STRUCTURE_MOTION_FIELD) { // Field based prediction

                nvMoCompParams.blockHeight = 64;
                nvMoCompParams.blockY = pMacroblock->vOffset << 2;

                // Top field prediction
                nvMoCompParams.backwardField = pMacroblock->fieldSelect[0][1];
                nvMoCompParams.destinationField = 0;
                *(long *)&nvMoCompParams.backwardHorizontalVector = (*(long *)&pMacroblock->PMV[0][1][0] << 1) & 0xFFFEFFFF;

                nvMoCompParams.blockY2 = nvMoCompParams.blockY;

                // Bottom field prediction
                nvMoCompParams.backwardField2 = pMacroblock->fieldSelect[1][1];
                nvMoCompParams.destinationField2 = 1;
                *(long *)&nvMoCompParams.backwardHorizontalVector2 = (*(long *)&pMacroblock->PMV[1][1][0] << 1) & 0xFFFEFFFF;

                nvDoTwoBackwardFieldPredictions(FALSE);
            }
        }
    }

    return TRUE;

} /* nvPredictKelvinFrameLumaMacroblock */


/*
 * nvPredictKelvinFrameChromaMacroblock
 *
 * Determines what type of prediction to apply to the macroblock and
 * calls the appropriate function.
 *
 * Note: All geometry sent to prediction functions is in quarter pel units.
 *       This function assumes chroma format is interleaved CrCb so predictions are
 *       performed at twice the horizontal offset and width so that only 1 pass is required.
 *       This 16 bit pixel format also requires that the prediction functions be notified
 *       that they must use a pitch of half the default size when attempting to access field 1.
 *
 */
DWORD __stdcall nvPredictKelvinFrameChromaMacroblock(LPNVDECODEMACROBLOCK pMacroblock, WORD numCombinedMacroblocks)
{
    LPDDRAWI_DDRAWSURFACE_LCL   pSurf_lcl;
//    short dmvector[2];
//    short DMV[2][2];

    SET_KELVIN_CHROMA_DATA_FORMAT();
    
    // We don't currently support spatial scalability

    nvMoCompParams.destinationField = 0;
    nvMoCompParams.forwardField = 0;
    nvMoCompParams.backwardField = 0;
    nvMoCompParams.blockWidth = 32 * numCombinedMacroblocks;
    nvMoCompParams.blockX = pMacroblock->hOffset << 1;

    if ((pDriverData->bMCPictureCodingType == PICTURE_TYPE_B) &&
        ((pMacroblock->macroblockType & (MB_MOTION_FORWARD | MB_MOTION_BACKWARD)) ==
        (MB_MOTION_FORWARD | MB_MOTION_BACKWARD))) {

        if (pMacroblock->motionType == FRAME_STRUCTURE_MOTION_FRAME) { // Frame based prediction

            if (pMacroblock->PMV[0][0][0] < 0) 
                pMacroblock->PMV[0][0][0] += 1; // round toward 0

            if (pMacroblock->PMV[0][0][1] < 0) 
                pMacroblock->PMV[0][0][1] += 1; // round toward 0

            if (pMacroblock->PMV[0][1][0] < 0) 
                pMacroblock->PMV[0][1][0] += 1; // round toward 0

            if (pMacroblock->PMV[0][1][1] < 0) 
                pMacroblock->PMV[0][1][1] += 1; // round toward 0

            nvMoCompParams.blockHeight = 64;
            nvMoCompParams.blockY = pMacroblock->vOffset << 2;
            nvMoCompParams.forwardHorizontalVector = pMacroblock->PMV[0][0][0] & 0xFFFE;
            nvMoCompParams.forwardVerticalVector = (pMacroblock->PMV[0][0][1] & 0xFFFE) << 1;
            nvMoCompParams.backwardHorizontalVector = pMacroblock->PMV[0][1][0] & 0xFFFE;
            nvMoCompParams.backwardVerticalVector = (pMacroblock->PMV[0][1][1] & 0xFFFE) << 1;

            nvDoBidirectionalFieldPrediction(TRUE);

        } else if (pMacroblock->motionType == FRAME_STRUCTURE_MOTION_FIELD) { // Field based prediction

            nvMoCompParams.blockHeight = 32;
            nvMoCompParams.blockY = pMacroblock->vOffset << 1;

            if (pMacroblock->PMV[0][0][0] < 0) 
                pMacroblock->PMV[0][0][0] += 1; // round toward 0

            if (pMacroblock->PMV[0][0][1] < 0) 
                pMacroblock->PMV[0][0][1] += 1; // round toward 0

            if (pMacroblock->PMV[0][1][0] < 0) 
                pMacroblock->PMV[0][1][0] += 1; // round toward 0

            if (pMacroblock->PMV[0][1][1] < 0) 
                pMacroblock->PMV[0][1][1] += 1; // round toward 0

            if (pMacroblock->PMV[1][0][0] < 0) 
                pMacroblock->PMV[1][0][0] += 1; // round toward 0

            if (pMacroblock->PMV[1][0][1] < 0) 
                pMacroblock->PMV[1][0][1] += 1; // round toward 0

            if (pMacroblock->PMV[1][1][0] < 0) 
                pMacroblock->PMV[1][1][0] += 1; // round toward 0

            if (pMacroblock->PMV[1][1][1] < 0) 
                pMacroblock->PMV[1][1][1] += 1; // round toward 0

            // Top field prediction
            nvMoCompParams.forwardField = pMacroblock->fieldSelect[0][0];
            nvMoCompParams.backwardField = pMacroblock->fieldSelect[0][1];
            nvMoCompParams.destinationField = 0;
            *(long *)&nvMoCompParams.forwardHorizontalVector = (*(long *)&pMacroblock->PMV[0][0][0]) & 0xFFFEFFFE;
            *(long *)&nvMoCompParams.backwardHorizontalVector = (*(long *)&pMacroblock->PMV[0][1][0]) & 0xFFFEFFFE;

            nvMoCompParams.blockY2 = nvMoCompParams.blockY;

            // Bottom field prediction
            nvMoCompParams.forwardField2 = pMacroblock->fieldSelect[1][0];
            nvMoCompParams.backwardField2 = pMacroblock->fieldSelect[1][1];
            nvMoCompParams.destinationField2 = 1;
            *(long *)&nvMoCompParams.forwardHorizontalVector2 = (*(long *)&pMacroblock->PMV[1][0][0]) & 0xFFFEFFFE;
            *(long *)&nvMoCompParams.backwardHorizontalVector2 = (*(long *)&pMacroblock->PMV[1][1][0]) & 0xFFFEFFFE;

            nvDoTwoBidirectionalFieldPredictions(TRUE);
        } else {
            // Error
            return FALSE;
        }
    } else {
        if ((pMacroblock->macroblockType & MB_MOTION_FORWARD) ||
            (pDriverData->bMCPictureCodingType == PICTURE_TYPE_P)) {

            if ((pMacroblock->motionType == FRAME_STRUCTURE_MOTION_FRAME) ||
                (!(pMacroblock->macroblockType & MB_MOTION_FORWARD))) {
            
                if (pMacroblock->PMV[0][0][0] < 0) 
                    pMacroblock->PMV[0][0][0] += 1; // round toward 0

                if (pMacroblock->PMV[0][0][1] < 0) 
                    pMacroblock->PMV[0][0][1] += 1; // round toward 0

                nvMoCompParams.blockHeight = 64;
                nvMoCompParams.blockY = pMacroblock->vOffset << 2;
                nvMoCompParams.forwardHorizontalVector = pMacroblock->PMV[0][0][0] & 0xFFFE;
                nvMoCompParams.forwardVerticalVector = (pMacroblock->PMV[0][0][1] & 0xFFFE) << 1;

                nvDoForwardFieldPrediction(TRUE);

            } else if (pMacroblock->motionType == FRAME_STRUCTURE_MOTION_FIELD) { // Field based prediction

                nvMoCompParams.blockHeight = 32;
                nvMoCompParams.blockY = pMacroblock->vOffset << 1;

                if (pMacroblock->PMV[0][0][0] < 0) 
                    pMacroblock->PMV[0][0][0] += 1; // round toward 0

                if (pMacroblock->PMV[0][0][1] < 0) 
                    pMacroblock->PMV[0][0][1] += 1; // round toward 0

                if (pMacroblock->PMV[1][0][0] < 0) 
                    pMacroblock->PMV[1][0][0] += 1; // round toward 0
                    
                if (pMacroblock->PMV[1][0][1] < 0) 
                    pMacroblock->PMV[1][0][1] += 1; // round toward 0
                
                // Top field prediction
                nvMoCompParams.forwardField = pMacroblock->fieldSelect[0][0];
                nvMoCompParams.destinationField = 0;
                *(long *)&nvMoCompParams.forwardHorizontalVector = (*(long *)&pMacroblock->PMV[0][0][0]) & 0xFFFEFFFE;

                nvMoCompParams.blockY2 = nvMoCompParams.blockY;

                // Bottom field prediction
                nvMoCompParams.forwardField2 = pMacroblock->fieldSelect[1][0];
                nvMoCompParams.destinationField2 = 1;
                *(long *)&nvMoCompParams.forwardHorizontalVector2 = (*(long *)&pMacroblock->PMV[1][0][0]) & 0xFFFEFFFE;

                nvDoTwoForwardFieldPredictions(TRUE);

            } else if (pMacroblock->motionType == FRAME_STRUCTURE_MOTION_DP) { // Dual prime prediction

                // Calculate derived motion vectors
                // nvDualPrimeArithmetic(DMV,
                //                      dmvector,
                //                      pMacroblock->PMV[0][0][0],
                //                      (short)(pMacroblock->PMV[0][0][1] >> 1));

                pSurf_lcl = (LPDDRAWI_DDRAWSURFACE_LCL)pDriverData->dwMCDualPrimeSurface;

                nvMoCompParams.blockHeight = 32;
                nvMoCompParams.blockY = pMacroblock->vOffset << 1;

                nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                              NV097_SET_TEXTURE_OFFSET(1) | 0x40000);
                nvPushData(1, VIDMEM_OFFSET((pSurf_lcl->lpGbl->fpVidMem + nvMoCompParams.surfaceChromaOffset)));

                nvPusherAdjust(2);

                if (pMacroblock->PMV[0][0][0] < 0) 
                    pMacroblock->PMV[0][0][0] += 1; // round toward 0

                if (pMacroblock->PMV[0][0][1] < 0) 
                    pMacroblock->PMV[0][0][1] += 1; // round toward 0

                if (pMacroblock->PMV[1][0][0] < 0) 
                    pMacroblock->PMV[1][0][0] += 1; // round toward 0

                if (pMacroblock->PMV[1][0][1] < 0) 
                    pMacroblock->PMV[1][0][1] += 1; // round toward 0

                if (pMacroblock->PMV[1][1][0] < 0) 
                    pMacroblock->PMV[1][1][0] += 1; // round toward 0

                if (pMacroblock->PMV[1][1][1] < 0) 
                    pMacroblock->PMV[1][1][1] += 1; // round toward 0

                // Predict top field from top field
                // and add to top field from bottom field
                nvMoCompParams.forwardField = 0;
                nvMoCompParams.backwardField = 1;
                nvMoCompParams.destinationField = 0;
                *(long *)&nvMoCompParams.forwardHorizontalVector = (*(long *)&pMacroblock->PMV[0][0][0]) & 0xFFFEFFFE;
                *(long *)&nvMoCompParams.backwardHorizontalVector = (*(long *)&pMacroblock->PMV[1][0][0]) & 0xFFFEFFFE;

                nvDoBidirectionalFieldPrediction(TRUE);

                // Predict bottom field from bottom field
                // and add to bottom field from top field
                nvMoCompParams.forwardField = 1;
                nvMoCompParams.backwardField = 0;
                nvMoCompParams.destinationField = 1;
                *(long *)&nvMoCompParams.forwardHorizontalVector = (*(long *)&pMacroblock->PMV[0][0][0]) & 0xFFFEFFFE;
                *(long *)&nvMoCompParams.backwardHorizontalVector = (*(long *)&pMacroblock->PMV[1][1][0]) & 0xFFFEFFFE;

                nvDoBidirectionalFieldPrediction(TRUE);

                pSurf_lcl = (LPDDRAWI_DDRAWSURFACE_LCL)pDriverData->dwMCBackwardSurface;

                nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                              NV097_SET_TEXTURE_OFFSET(1) | 0x40000);
                nvPushData(1, VIDMEM_OFFSET((pSurf_lcl->lpGbl->fpVidMem + nvMoCompParams.surfaceChromaOffset)));

                nvPusherAdjust(2);
            } else {
                // Error
                return FALSE;
            }
        }

        if (pMacroblock->macroblockType & MB_MOTION_BACKWARD) {

            if (pMacroblock->motionType == FRAME_STRUCTURE_MOTION_FRAME) { // Frame based prediction

                if (pMacroblock->PMV[0][1][0] < 0) 
                    pMacroblock->PMV[0][1][0] += 1; // round toward 0

                if (pMacroblock->PMV[0][1][1] < 0) 
                    pMacroblock->PMV[0][1][1] += 1; // round toward 0

                nvMoCompParams.blockHeight = 64;
                nvMoCompParams.blockY = pMacroblock->vOffset << 2;
                nvMoCompParams.backwardHorizontalVector = pMacroblock->PMV[0][1][0] & 0xFFFE;
                nvMoCompParams.backwardVerticalVector = (pMacroblock->PMV[0][1][1] & 0xFFFE) << 1;

                nvDoBackwardFieldPrediction(TRUE);

            } else if (pMacroblock->motionType == FRAME_STRUCTURE_MOTION_FIELD) { // Field based prediction

                nvMoCompParams.blockHeight = 32;
                nvMoCompParams.blockY = pMacroblock->vOffset << 1;

                if (pMacroblock->PMV[0][1][0] < 0) 
                    pMacroblock->PMV[0][1][0] += 1; // round toward 0

                if (pMacroblock->PMV[0][1][1] < 0) 
                    pMacroblock->PMV[0][1][1] += 1; // round toward 0

                if (pMacroblock->PMV[1][1][0] < 0) 
                    pMacroblock->PMV[1][1][0] += 1; // round toward 0

                if (pMacroblock->PMV[1][1][1] < 0) 
                    pMacroblock->PMV[1][1][1] += 1; // round toward 0

                // Top field prediction
                nvMoCompParams.backwardField = pMacroblock->fieldSelect[0][1];
                nvMoCompParams.destinationField = 0;
                *(long *)&nvMoCompParams.backwardHorizontalVector = (*(long *)&pMacroblock->PMV[0][1][0]) & 0xFFFEFFFE;

                nvMoCompParams.blockY2 = nvMoCompParams.blockY;

                // Bottom field prediction
                nvMoCompParams.backwardField2 = pMacroblock->fieldSelect[1][1];
                nvMoCompParams.destinationField2 = 1;
                *(long *)&nvMoCompParams.backwardHorizontalVector2 = (*(long *)&pMacroblock->PMV[1][1][0]) & 0xFFFEFFFE;

                nvDoTwoBackwardFieldPredictions(TRUE);
            }
        }
    }

    return TRUE;

} /* nvPredictKelvinFrameChromaMacroblock */


/*
 * nvPredictKelvinFieldLumaMacroblock
 *
 * Determines what type of prediction to apply to the macroblock and
 * calls the appropriate function.
 *
 * Note: All geometry sent to prediction functions is in quarter pel units
 *
 */
DWORD __stdcall nvPredictKelvinFieldLumaMacroblock(LPNVDECODEMACROBLOCK pMacroblock, WORD numCombinedMacroblocks)
{
    LPDDRAWI_DDRAWSURFACE_LCL   pSurf_lcl;
    unsigned long currentField;
    unsigned long doBackwardPrediction;
//    short dmvector[2];
//    short DMV[2][2];

    SET_KELVIN_LUMA_DATA_FORMAT();
    
    // We don't currently support spatial scalability

    // Destination field offset already set at init time
    nvMoCompParams.destinationField = 0;

    nvMoCompParams.blockWidth = 64 * numCombinedMacroblocks;
    nvMoCompParams.blockX = pMacroblock->hOffset << 2;
    nvMoCompParams.blockY = pMacroblock->vOffset << 2;

    if ((pDriverData->bMCPictureCodingType == PICTURE_TYPE_B) &&
        ((pMacroblock->macroblockType & (MB_MOTION_FORWARD | MB_MOTION_BACKWARD)) ==
        (MB_MOTION_FORWARD | MB_MOTION_BACKWARD))) {

        // Field based prediction
        if (pMacroblock->motionType == FIELD_STRUCTURE_MOTION_FIELD) {

            nvMoCompParams.forwardField = pMacroblock->fieldSelect[0][0];
            nvMoCompParams.backwardField = pMacroblock->fieldSelect[0][1];
            nvMoCompParams.blockHeight = 64;
            *(long *)&nvMoCompParams.forwardHorizontalVector = (*(long *)&pMacroblock->PMV[0][0][0] << 1) & 0xFFFEFFFF;
            *(long *)&nvMoCompParams.backwardHorizontalVector = (*(long *)&pMacroblock->PMV[0][1][0] << 1) & 0xFFFEFFFF;

            nvDoBidirectionalFieldPrediction(FALSE);

        } else if (pMacroblock->motionType == FIELD_STRUCTURE_MOTION_16X8) {

            nvMoCompParams.blockHeight = 32;

            // Top half
            nvMoCompParams.forwardField = pMacroblock->fieldSelect[0][0];
            nvMoCompParams.backwardField = pMacroblock->fieldSelect[0][1];
            *(long *)&nvMoCompParams.forwardHorizontalVector = (*(long *)&pMacroblock->PMV[0][0][0] << 1) & 0xFFFEFFFF;
            *(long *)&nvMoCompParams.backwardHorizontalVector = (*(long *)&pMacroblock->PMV[0][1][0] << 1) & 0xFFFEFFFF;

            // Bottom half
            nvMoCompParams.destinationField2 = nvMoCompParams.destinationField;
            nvMoCompParams.forwardField2 = pMacroblock->fieldSelect[1][0];
            nvMoCompParams.backwardField2 = pMacroblock->fieldSelect[1][1];
            nvMoCompParams.blockY2 = nvMoCompParams.blockY + 32;
            *(long *)&nvMoCompParams.forwardHorizontalVector2 = (*(long *)&pMacroblock->PMV[1][0][0] << 1) & 0xFFFEFFFF;
            *(long *)&nvMoCompParams.backwardHorizontalVector2 = (*(long *)&pMacroblock->PMV[1][1][0] << 1) & 0xFFFEFFFF;

            nvDoTwoBidirectionalFieldPredictions(FALSE);
        } else {
            // Error
            return FALSE;
        }
    } else {
        if ((pMacroblock->macroblockType & MB_MOTION_FORWARD) ||
            (pDriverData->bMCPictureCodingType == PICTURE_TYPE_P)) {

            if (pDriverData->bMCPictureStructure == PICTURE_STRUCTURE_TOP_FIELD)
                currentField = 0;
            else
                currentField = 1;

            doBackwardPrediction = 0; // previous frame

            // Determine which frame to use for prediction
            if ((pDriverData->bMCPictureCodingType == PICTURE_TYPE_P) &&
                 pDriverData->bMCSecondField &&
                (currentField != pMacroblock->fieldSelect[0][0]))
                doBackwardPrediction = 1; // same frame

            // Field based prediction
            if ((pMacroblock->motionType == FIELD_STRUCTURE_MOTION_FIELD) ||
                (!(pMacroblock->macroblockType & MB_MOTION_FORWARD))) {

                nvMoCompParams.blockHeight = 64;

                if (doBackwardPrediction) {
                    nvMoCompParams.backwardField = pMacroblock->fieldSelect[0][0];
                    *(long *)&nvMoCompParams.backwardHorizontalVector = (*(long *)&pMacroblock->PMV[0][0][0] << 1) & 0xFFFEFFFF;

                    nvDoBackwardFieldPrediction(FALSE);
                } else {
                    nvMoCompParams.forwardField = pMacroblock->fieldSelect[0][0];
                    *(long *)&nvMoCompParams.forwardHorizontalVector = (*(long *)&pMacroblock->PMV[0][0][0] << 1) & 0xFFFEFFFF;

                    nvDoForwardFieldPrediction(FALSE);
                }
            } else if (pMacroblock->motionType == FIELD_STRUCTURE_MOTION_16X8) {

                nvMoCompParams.blockHeight = 32;

                if (doBackwardPrediction) {
                    nvMoCompParams.backwardField = pMacroblock->fieldSelect[0][0];
                    *(long *)&nvMoCompParams.backwardHorizontalVector = (*(long *)&pMacroblock->PMV[0][0][0] << 1) & 0xFFFEFFFF;

                    nvDoBackwardFieldPrediction(FALSE);
                } else {
                    nvMoCompParams.forwardField = pMacroblock->fieldSelect[0][0];
                    *(long *)&nvMoCompParams.forwardHorizontalVector = (*(long *)&pMacroblock->PMV[0][0][0] << 1) & 0xFFFEFFFF;

                    nvDoForwardFieldPrediction(FALSE);
                }

                doBackwardPrediction = 0; // previous frame

                // Determine which frame to use for lower prediction
                if ((pDriverData->bMCPictureCodingType == PICTURE_TYPE_P) &&
                     pDriverData->bMCSecondField &&
                    (currentField != pMacroblock->fieldSelect[1][0]))
                    doBackwardPrediction = 1; // same frame

                nvMoCompParams.blockY += 32;

                if (doBackwardPrediction) {
                    nvMoCompParams.backwardField = pMacroblock->fieldSelect[1][0];
                    *(long *)&nvMoCompParams.backwardHorizontalVector = (*(long *)&pMacroblock->PMV[1][0][0] << 1) & 0xFFFEFFFF;

                    nvDoBackwardFieldPrediction(FALSE);
                } else {
                    nvMoCompParams.forwardField = pMacroblock->fieldSelect[1][0];
                    *(long *)&nvMoCompParams.forwardHorizontalVector = (*(long *)&pMacroblock->PMV[1][0][0] << 1) & 0xFFFEFFFF;

                    nvDoForwardFieldPrediction(FALSE);
                }
            } else if (pMacroblock->motionType == FIELD_STRUCTURE_MOTION_DP) { // Dual prime prediction

                if (pDriverData->bMCSecondField)
                    doBackwardPrediction = 1; // same frame
                else
                    doBackwardPrediction = 0; // previous frame

                // Calculate derived motion vectors
                // nvDualPrimeArithmetic(DMV,
                //                      dmvector,
                //                      pMacroblock->PMV[0][0][0],
                //                      pMacroblock->PMV[0][0][1]);


                if (doBackwardPrediction)
                    pSurf_lcl = (LPDDRAWI_DDRAWSURFACE_LCL)pDriverData->dwMCDestinationSurface;
                else
                    pSurf_lcl = (LPDDRAWI_DDRAWSURFACE_LCL)pDriverData->dwMCDualPrimeSurface;

                nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                              NV097_SET_TEXTURE_OFFSET(1) | 0x40000);
                nvPushData(1, VIDMEM_OFFSET(pSurf_lcl->lpGbl->fpVidMem));

                nvPusherAdjust(2);
                
                // Predict from field of same parity
                // added to field of opposite parity
                nvMoCompParams.forwardField = (BYTE)currentField;
                nvMoCompParams.backwardField = (BYTE)currentField ^ 1;
                nvMoCompParams.blockHeight = 64;
                *(long *)&nvMoCompParams.forwardHorizontalVector = (*(long *)&pMacroblock->PMV[0][0][0] << 1) & 0xFFFEFFFF;
                // nvMoCompParams.backwardHorizontalVector = DMV[0][0];
                // nvMoCompParams.backwardVerticalVector = DMV[0][1];
                *(long *)&nvMoCompParams.backwardHorizontalVector = (*(long *)&pMacroblock->PMV[1][0][0] << 1) & 0xFFFEFFFF;

                nvDoBidirectionalFieldPrediction(FALSE);

                pSurf_lcl = (LPDDRAWI_DDRAWSURFACE_LCL)pDriverData->dwMCBackwardSurface;

                nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                              NV097_SET_TEXTURE_OFFSET(1) | 0x40000);
                nvPushData(1, VIDMEM_OFFSET(pSurf_lcl->lpGbl->fpVidMem));

                nvPusherAdjust(2);

            } else {
                // Error
                return FALSE;
            }
        }

        if (pMacroblock->macroblockType & MB_MOTION_BACKWARD) {

            // Field based prediction
            if (pMacroblock->motionType == FIELD_STRUCTURE_MOTION_FIELD) {

                nvMoCompParams.backwardField = pMacroblock->fieldSelect[0][1];
                nvMoCompParams.blockHeight = 64;
                *(long *)&nvMoCompParams.backwardHorizontalVector = (*(long *)&pMacroblock->PMV[0][1][0] << 1) & 0xFFFEFFFF;

                nvDoBackwardFieldPrediction(FALSE);

            } else if (pMacroblock->motionType == FIELD_STRUCTURE_MOTION_16X8) {

                nvMoCompParams.blockHeight = 32;

                nvMoCompParams.backwardField = pMacroblock->fieldSelect[0][1];
                *(long *)&nvMoCompParams.backwardHorizontalVector = (*(long *)&pMacroblock->PMV[0][1][0] << 1) & 0xFFFEFFFF;

                nvMoCompParams.destinationField2 = nvMoCompParams.destinationField;
                nvMoCompParams.backwardField2 = pMacroblock->fieldSelect[1][1];
                nvMoCompParams.blockY2 = nvMoCompParams.blockY + 32;
                *(long *)&nvMoCompParams.backwardHorizontalVector2 = (*(long *)&pMacroblock->PMV[1][1][0] << 1) & 0xFFFEFFFF;

                nvDoTwoBackwardFieldPredictions(FALSE);

            } else {
                // Error
                return FALSE;
            }
        }
    }

    return TRUE;

} /* nvPredictKelvinFieldLumaMacroblock */


/*
 * nvPredictKelvinFieldChromaMacroblock
 *
 * Determines what type of prediction to apply to the macroblock and
 * calls the appropriate function.
 *
 * Note: All geometry sent to prediction functions is in quarter pel units.
 *       This function assumes chroma format is interleaved CrCb so predictions are
 *       performed at twice the horizontal offset and width so that only 1 pass is required.
 *       This 16 bit pixel format also requires that the prediction functions be notified
 *       that they must use a pitch of half the default size when attempting to access field 1.
 *
 */
DWORD __stdcall nvPredictKelvinFieldChromaMacroblock(LPNVDECODEMACROBLOCK pMacroblock, WORD numCombinedMacroblocks)
{
    LPDDRAWI_DDRAWSURFACE_LCL   pSurf_lcl;
    unsigned long currentField;
    unsigned long doBackwardPrediction;
//    short dmvector[2];
//    short DMV[2][2];

    SET_KELVIN_CHROMA_DATA_FORMAT();
    
    // We don't currently support spatial scalability

    // Destination field offset already set at init time
    nvMoCompParams.destinationField = 0;

    nvMoCompParams.blockWidth = 32 * numCombinedMacroblocks;
    nvMoCompParams.blockX = pMacroblock->hOffset << 1;
    nvMoCompParams.blockY = pMacroblock->vOffset << 1;

    if ((pDriverData->bMCPictureCodingType == PICTURE_TYPE_B) &&
        ((pMacroblock->macroblockType & (MB_MOTION_FORWARD | MB_MOTION_BACKWARD)) ==
        (MB_MOTION_FORWARD | MB_MOTION_BACKWARD))) {

        // Field based prediction
        if (pMacroblock->motionType == FIELD_STRUCTURE_MOTION_FIELD) {

            nvMoCompParams.forwardField = pMacroblock->fieldSelect[0][0];
            nvMoCompParams.backwardField = pMacroblock->fieldSelect[0][1];
            nvMoCompParams.blockHeight = 32;
            *(long *)&nvMoCompParams.forwardHorizontalVector = (*(long *)&pMacroblock->PMV[0][0][0]);
            *(long *)&nvMoCompParams.backwardHorizontalVector = (*(long *)&pMacroblock->PMV[0][1][0]);

            nvDoBidirectionalFieldPrediction(TRUE);

        } else if (pMacroblock->motionType == FIELD_STRUCTURE_MOTION_16X8) {

            nvMoCompParams.blockHeight = 16;

            // Top half
            nvMoCompParams.forwardField = pMacroblock->fieldSelect[0][0];
            nvMoCompParams.backwardField = pMacroblock->fieldSelect[0][1];
            *(long *)&nvMoCompParams.forwardHorizontalVector = (*(long *)&pMacroblock->PMV[0][0][0]);
            *(long *)&nvMoCompParams.backwardHorizontalVector = (*(long *)&pMacroblock->PMV[0][1][0]);

            // Bottom half
            nvMoCompParams.destinationField2 = nvMoCompParams.destinationField;
            nvMoCompParams.forwardField2 = pMacroblock->fieldSelect[1][0];
            nvMoCompParams.backwardField2 = pMacroblock->fieldSelect[1][1];
            nvMoCompParams.blockY2 = nvMoCompParams.blockY + 16;
            *(long *)&nvMoCompParams.forwardHorizontalVector2 = (*(long *)&pMacroblock->PMV[1][0][0]);
            *(long *)&nvMoCompParams.backwardHorizontalVector2 = (*(long *)&pMacroblock->PMV[1][1][0]);

            nvDoTwoBidirectionalFieldPredictions(TRUE);

        } else {
            // Error
            return FALSE;
        }
    } else {
        if ((pMacroblock->macroblockType & MB_MOTION_FORWARD) ||
            (pDriverData->bMCPictureCodingType == PICTURE_TYPE_P)) {

            if (pDriverData->bMCPictureStructure == PICTURE_STRUCTURE_TOP_FIELD)
                currentField = 0;
            else
                currentField = 1;

            doBackwardPrediction = 0; // previous frame

            // Determine which frame to use for prediction
            if ((pDriverData->bMCPictureCodingType == PICTURE_TYPE_P) &&
                 pDriverData->bMCSecondField &&
                (currentField != pMacroblock->fieldSelect[0][0]))
                doBackwardPrediction = 1; // same frame

            // Field based prediction
            if ((pMacroblock->motionType == FIELD_STRUCTURE_MOTION_FIELD) ||
                (!(pMacroblock->macroblockType & MB_MOTION_FORWARD))) {

                nvMoCompParams.blockHeight = 32;

                if (doBackwardPrediction) {

                    nvMoCompParams.backwardField = pMacroblock->fieldSelect[0][0];
                    *(long *)&nvMoCompParams.backwardHorizontalVector = (*(long *)&pMacroblock->PMV[0][0][0]);

                    nvDoBackwardFieldPrediction(TRUE);
                } else {
                    nvMoCompParams.forwardField = pMacroblock->fieldSelect[0][0];
                    *(long *)&nvMoCompParams.forwardHorizontalVector = (*(long *)&pMacroblock->PMV[0][0][0]);

                    nvDoForwardFieldPrediction(TRUE);
                }
            } else if (pMacroblock->motionType == FIELD_STRUCTURE_MOTION_16X8) {

                nvMoCompParams.blockHeight = 16;

                if (doBackwardPrediction) {
                    nvMoCompParams.backwardField = pMacroblock->fieldSelect[0][0];
                    *(long *)&nvMoCompParams.backwardHorizontalVector = (*(long *)&pMacroblock->PMV[0][0][0]);

                    nvDoBackwardFieldPrediction(TRUE);
                } else {
                    nvMoCompParams.forwardField = pMacroblock->fieldSelect[0][0];
                    *(long *)&nvMoCompParams.forwardHorizontalVector = (*(long *)&pMacroblock->PMV[0][0][0]);

                    nvDoForwardFieldPrediction(TRUE);
                }

                doBackwardPrediction = 0; // previous frame

                // Determine which frame to use for lower prediction
                if ((pDriverData->bMCPictureCodingType == PICTURE_TYPE_P) &&
                     pDriverData->bMCSecondField &&
                    (currentField != pMacroblock->fieldSelect[1][0]))
                    doBackwardPrediction = 1; // same frame

                nvMoCompParams.blockY += 16;

                if (doBackwardPrediction) {
                    nvMoCompParams.backwardField = pMacroblock->fieldSelect[1][0];
                    *(long *)&nvMoCompParams.backwardHorizontalVector = (*(long *)&pMacroblock->PMV[1][0][0]);

                    nvDoBackwardFieldPrediction(TRUE);
                } else {
                    nvMoCompParams.forwardField = pMacroblock->fieldSelect[1][0];
                    *(long *)&nvMoCompParams.forwardHorizontalVector = (*(long *)&pMacroblock->PMV[1][0][0]);

                    nvDoForwardFieldPrediction(TRUE);
                }
            } else if (pMacroblock->motionType == FIELD_STRUCTURE_MOTION_DP) { // Dual prime prediction

                if (pDriverData->bMCSecondField)
                    doBackwardPrediction = 1; // same frame
                else
                    doBackwardPrediction = 0; // previous frame

                // Calculate derived motion vectors
                // nvDualPrimeArithmetic(DMV,
                //                      dmvector,
                //                      pMacroblock->PMV[0][0][0],
                //                      pMacroblock->PMV[0][0][1]);

                if (doBackwardPrediction)
                    pSurf_lcl = (LPDDRAWI_DDRAWSURFACE_LCL)pDriverData->dwMCDestinationSurface;
                else
                    pSurf_lcl = (LPDDRAWI_DDRAWSURFACE_LCL)pDriverData->dwMCDualPrimeSurface;

                nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                              NV097_SET_TEXTURE_OFFSET(1) | 0x40000);
                nvPushData(1, VIDMEM_OFFSET((pSurf_lcl->lpGbl->fpVidMem + nvMoCompParams.surfaceChromaOffset)));

                nvPusherAdjust(2);

                // Predict from field of same parity
                // added to field of opposite parity
                nvMoCompParams.forwardField = (BYTE)currentField;
                nvMoCompParams.backwardField = (BYTE)currentField ^ 1;
                nvMoCompParams.blockHeight = 32;
                *(long *)&nvMoCompParams.forwardHorizontalVector = (*(long *)&pMacroblock->PMV[0][0][0]);
                // nvMoCompParams.backwardHorizontalVector = DMV[0][0] >> 1;
                // nvMoCompParams.backwardVerticalVector = DMV[0][1] >> 1;
                *(long *)&nvMoCompParams.backwardHorizontalVector = (*(long *)&pMacroblock->PMV[1][0][0]);

                nvDoBidirectionalFieldPrediction(TRUE);

                pSurf_lcl = (LPDDRAWI_DDRAWSURFACE_LCL)pDriverData->dwMCBackwardSurface;

                nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                              NV097_SET_TEXTURE_OFFSET(1) | 0x40000);
                nvPushData(1, VIDMEM_OFFSET((pSurf_lcl->lpGbl->fpVidMem + nvMoCompParams.surfaceChromaOffset)));

                nvPusherAdjust(2);

            } else {
                // Error
                return FALSE;
            }
        }

        if (pMacroblock->macroblockType & MB_MOTION_BACKWARD) {

            // Field based prediction
            if (pMacroblock->motionType == FIELD_STRUCTURE_MOTION_FIELD) {

                nvMoCompParams.backwardField = pMacroblock->fieldSelect[0][1];
                nvMoCompParams.blockHeight = 32;
                *(long *)&nvMoCompParams.backwardHorizontalVector = (*(long *)&pMacroblock->PMV[0][1][0]);

                nvDoBackwardFieldPrediction(TRUE);

            } else if (pMacroblock->motionType == FIELD_STRUCTURE_MOTION_16X8) {

                nvMoCompParams.blockHeight = 16;

                nvMoCompParams.backwardField = pMacroblock->fieldSelect[0][1];
                *(long *)&nvMoCompParams.backwardHorizontalVector = (*(long *)&pMacroblock->PMV[0][1][0]);

                nvMoCompParams.destinationField2 = nvMoCompParams.destinationField;
                nvMoCompParams.backwardField2 = pMacroblock->fieldSelect[1][1];
                nvMoCompParams.blockY2 = nvMoCompParams.blockY + 16;
                *(long *)&nvMoCompParams.backwardHorizontalVector2 = (*(long *)&pMacroblock->PMV[1][1][0]);

                nvDoTwoBackwardFieldPredictions(TRUE);

            } else {
                // Error
                return FALSE;
            }
        }
    }

    return TRUE;

} /* nvPredictKelvinFieldChromaMacroblock */


#endif  // NVARCH >= 0x10
