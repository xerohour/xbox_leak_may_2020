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
 *  Copyright (C) 2000 NVidia Corporation.  All Rights Reserved.
 *
 *  File:       MoCompExKelvin.cpp
 *  Content:    Windows98 DirectDraw 32 bit driver
 *
 ***************************************************************************/
#include "nvprecomp.h"

#if (NVARCH >= 0x10)

#include "dxshare.h"

extern NVMOCOMPPARAMS           nvMoCompParams;

#ifndef WINNT
    extern DISPDRVDIRECTXCOMMON    *pDXShare;
#endif // WINNT

extern BOOL __fastcall nvMoCompCheckFieldPredictionEqual(LPNVDECODEMACROBLOCK pMacroblock0,
                                                         LPNVDECODEMACROBLOCK pMacroblock1, BYTE macroblockType);

extern DWORD __stdcall nvInitKelvinForMoComp(void);
extern DWORD __stdcall nvInitKelvinForMoCompPrediction(void);
extern DWORD __stdcall nvInitKelvinForMoCompCorrection();
extern void __stdcall nvDoKelvinBidirectionalDVDFieldPrediction(BOOL);
extern void __stdcall nvDoTwoKelvinBidirectionalDVDFieldPredictions(BOOL);
extern void __stdcall nvDoKelvinForwardDVDFieldPrediction(BOOL);
extern void __stdcall nvDoTwoKelvinForwardDVDFieldPredictions(BOOL);
extern void __stdcall nvDoKelvinBackwardDVDFieldPrediction(BOOL);
extern void __stdcall nvDoTwoKelvinBackwardDVDFieldPredictions(BOOL);
extern void __stdcall nvDoAllKelvinIntraDVDFieldCorrections(long correctionYIncr, long numBlocks);
extern void __stdcall nvDoAllKelvinDVDFieldCorrections(long correctionYIncr, long numBlocks);
extern void __stdcall nvDoKelvinIntraDVDFieldCorrection(void);
extern void __stdcall nvDoOptimizedKelvinIntraDVDFieldCorrection(void);
extern void __stdcall nvDoKelvinDVDFieldCorrection(void);
extern void __stdcall nvDoAllKelvinIntraFrameCorrections(void);
extern void __stdcall nvDoAllKelvinFrameCorrections(void);
extern void __stdcall nvDoKelvinIntraFrameCorrection(void);
extern void __stdcall nvDoKelvinFrameCorrection(void);
extern DWORD __stdcall nvPredictKelvinFrameLumaMacroblock(LPNVDECODEMACROBLOCK pMacroblock, WORD numCombinedMacroblocks);
extern DWORD __stdcall nvPredictKelvinFieldLumaMacroblock(LPNVDECODEMACROBLOCK pMacroblock, WORD numCombinedMacroblocks);
extern DWORD __stdcall nvPredictKelvinFrameChromaMacroblock(LPNVDECODEMACROBLOCK pMacroblock, WORD numCombinedMacroblocks);
extern DWORD __stdcall nvPredictKelvinFieldChromaMacroblock(LPNVDECODEMACROBLOCK pMacroblock, WORD numCombinedMacroblocks);
extern void __stdcall nvCorrectKelvinIntraFrame(LPNVDECODEMACROBLOCK pMacroblock, long macroblockCount);
extern void __stdcall nvCorrectKelvinFrameLumaMacroblock(LPNVDECODEMACROBLOCK pMacroblock, DWORD correctionType);
extern void __stdcall nvCorrectKelvinFullFrameLumaMacroblock(LPNVDECODEMACROBLOCK pMacroblock, DWORD correctionType);
extern void __stdcall nvCorrectKelvinFrameChromaMacroblock(LPNVDECODEMACROBLOCK pMacroblock, DWORD correctionType);
extern void __stdcall nvCorrectKelvinFullFrameChromaMacroblock(LPNVDECODEMACROBLOCK pMacroblock, DWORD correctionType);
extern void __stdcall nvCorrectKelvinIntraField(LPNVDECODEMACROBLOCK pMacroblock, long macroblockCount);
extern void __stdcall nvCorrectKelvinFieldMacroblock(LPNVDECODEMACROBLOCK pMacroblock, DWORD correctionType);

extern PNVMCPREDFUNC nvDoBidirectionalFieldPrediction;
extern PNVMCPREDFUNC nvDoTwoBidirectionalFieldPredictions;
extern PNVMCPREDFUNC nvDoForwardFieldPrediction;
extern PNVMCPREDFUNC nvDoTwoForwardFieldPredictions;
extern PNVMCPREDFUNC nvDoBackwardFieldPrediction;
extern PNVMCPREDFUNC nvDoTwoBackwardFieldPredictions;

extern PNVMCPREDFUNC2 nvPredictFrameLumaMacroblock;
extern PNVMCPREDFUNC2 nvPredictFieldLumaMacroblock;
extern PNVMCPREDFUNC2 nvPredictFrameChromaMacroblock;
extern PNVMCPREDFUNC2 nvPredictFieldChromaMacroblock;

extern PNVMCCORRFUNC2 nvDoAllIntraFieldCorrections;
extern PNVMCCORRFUNC2 nvDoAllFieldCorrections;
extern PNVMCCORRFUNC nvDoAllIntraFrameCorrections;
extern PNVMCCORRFUNC nvDoAllFrameCorrections;
extern PNVMCCORRFUNC nvDoIntraFieldCorrection;
extern PNVMCCORRFUNC nvDoOptimizedIntraFieldCorrection;
extern PNVMCCORRFUNC nvDoFieldCorrection;
extern PNVMCCORRFUNC nvDoIntraFrameCorrection;
extern PNVMCCORRFUNC nvDoFrameCorrection;


/*
 * nvMoCompProcessKelvinFrameMacroblocks
 *
 * Parses the current macroblock command stream and performs the necessary operations on
 * each macroblock.
 *
 *
 * Note that pDriverData must be set before calling this function
 *
 */
DWORD __stdcall nvMoCompProcessKelvinFrameMacroblocks(LPNVDECODEMACROBLOCK pMacroblocks,
                                                      DWORD numMacroblocks, BYTE formatConversionStatus)
{
    LPDDRAWI_DDRAWSURFACE_LCL   dstx = (LPDDRAWI_DDRAWSURFACE_LCL)pDriverData->dwMCDestinationSurface;
    LPDDRAWI_DDRAWSURFACE_GBL   dst = dstx->lpGbl;
    LPNVDECODEMACROBLOCK        pMacroblock;
    LPNVDECODEMACROBLOCK        pLastMacroblock;
    LPNVDECODEMACROBLOCK        pMacroblock1;
    unsigned long               dwFirstAlignedIDCTOffset;
    unsigned long               dwSurfaceOffset;
    unsigned long               surfacePitch;
    unsigned long               moCompCorrectionICW;
    unsigned long               moCompNonPrebiasedCorrectionICW;
    long                        macroblockLimit;
    long                        n;
    long                        macroblockIndex;
    BYTE                        surfaceDecodeStatus = 0;
    BYTE                        numCombinedMacroblocks[256];
    short                       maxHeight = (short)dst->wHeight - 16;
    short                       maxWidth = (short)dst->wWidth - 16;
    BOOL                        hasFieldPredictions = FALSE;
    int                         index, i;
    int                         firstAlignedIDCTIndex;

    nvMoCompParams.vertexMode = 0; // Initialize vertexMode to an invalid value
    nvMoCompParams.surfaceLumaPitch = dst->lPitch;
    nvMoCompParams.surfaceLumaHeight = (dst->wHeight + 1) & ~1;
    nvMoCompParams.surfaceOffset = VIDMEM_OFFSET(dst->fpVidMem);
    nvMoCompParams.surfaceChromaOffset = nvMoCompParams.surfaceLumaPitch * nvMoCompParams.surfaceLumaHeight;
    nvMoCompParams.surfaceBiasedIntraBlockOffset =
        (dst->lPitch * dst->wHeight) + (dst->lPitch * (dst->wHeight >> 1) + (8 * dst->lPitch));

    if (pDriverData->bMCPictureCodingType != PICTURE_TYPE_I) {

        if (!(nvInitKelvinForMoCompPrediction()))
            return FALSE;

        nvDoBidirectionalFieldPrediction = nvDoKelvinBidirectionalDVDFieldPrediction;
        nvDoTwoBidirectionalFieldPredictions = nvDoTwoKelvinBidirectionalDVDFieldPredictions;
        nvDoForwardFieldPrediction = nvDoKelvinForwardDVDFieldPrediction;
        nvDoTwoForwardFieldPredictions = nvDoTwoKelvinForwardDVDFieldPredictions;
        nvDoBackwardFieldPrediction = nvDoKelvinBackwardDVDFieldPrediction;
        nvDoTwoBackwardFieldPredictions = nvDoTwoKelvinBackwardDVDFieldPredictions;

        nvPredictFrameLumaMacroblock = nvPredictKelvinFrameLumaMacroblock;
        nvPredictFrameChromaMacroblock = nvPredictKelvinFrameChromaMacroblock;
        nvPredictFieldLumaMacroblock = nvPredictKelvinFieldLumaMacroblock;
        nvPredictFieldChromaMacroblock = nvPredictKelvinFieldChromaMacroblock;

        nvMoCompParams.currentForwardField = 0;
        nvMoCompParams.currentBackwardField = 0;
        nvMoCompParams.currentDestinationField = 0;

        pMacroblock = pMacroblocks;
        n = numMacroblocks;
        macroblockIndex = 0;

        surfacePitch = (nvMoCompParams.surfaceLumaPitch << 16) | nvMoCompParams.surfaceLumaPitch;

        // Must temporarily set destination surface pitch back to it's normal frame pitch
        nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                      NV097_NO_OPERATION | 0x40000);
        nvPushData(1, 0);

        nvPushData(2, dDrawSubchannelOffset(NV_DD_KELVIN) +
                      NV097_SET_SURFACE_PITCH | 0x40000);
        nvPushData(3, surfacePitch);

        // Must also change texture pitches back to normal frame pitch so that quarter pel addressing works correctly
        nvPushData(4, dDrawSubchannelOffset(NV_DD_KELVIN) +
                       NV097_SET_TEXTURE_CONTROL1(0) | 0x40000);
        nvPushData(5, surfacePitch << 16);
        nvPushData(6, dDrawSubchannelOffset(NV_DD_KELVIN) +
                       NV097_SET_TEXTURE_CONTROL1(1) | 0x40000);
        nvPushData(7, surfacePitch << 16);

        nvPusherAdjust(8);

        // Perform luma prediction pass
        while (n > 0) {
            numCombinedMacroblocks[macroblockIndex] = 1;
            if (pMacroblock->macroblockType == MB_MOTION_FORWARD) {
                if (n > 1) {
                    pMacroblock1 = pMacroblock + 1;
                    if (pMacroblock->motionType == FRAME_STRUCTURE_MOTION_FRAME){
						//combine up to 5 adjacent macroblocks together
						for( i = 1; i <= 5; i++){
							if ((pMacroblock1->macroblockType == MB_MOTION_FORWARD) &&
								(pMacroblock1->motionType == FRAME_STRUCTURE_MOTION_FRAME) &&
								((pMacroblock1->hOffset - pMacroblock->hOffset) == 16 * i) &&
								(*(long *)&pMacroblock->PMV[0][0][0] == *(long *)&pMacroblock1->PMV[0][0][0])) {
									numCombinedMacroblocks[macroblockIndex]++;
							}else
								break;
							if(n <= i+1)
								break;	//no more macroblock left
							pMacroblock1++;	

						}
					} else if (pMacroblock->motionType == FRAME_STRUCTURE_MOTION_FIELD){
						//combine up to 5 adjacent macroblocks together
						for( i = 1; i <= 5; i++){
							if ((pMacroblock1->macroblockType == MB_MOTION_FORWARD) &&
								(pMacroblock1->motionType == FRAME_STRUCTURE_MOTION_FIELD) &&
								((pMacroblock1->hOffset - pMacroblock->hOffset) == 16 * i) &&
								(nvMoCompCheckFieldPredictionEqual(pMacroblock, pMacroblock1, pMacroblock->macroblockType))){
									numCombinedMacroblocks[macroblockIndex]++;
							}else
								break;
	
							if(n <= i+1)
								break;
							pMacroblock1++;							
						}
                    }
                }

                if (pMacroblock->motionType == FRAME_STRUCTURE_MOTION_FRAME)
                    nvPredictFrameLumaMacroblock(pMacroblock, numCombinedMacroblocks[macroblockIndex]);
                else
                    hasFieldPredictions = TRUE;

                pMacroblock += numCombinedMacroblocks[macroblockIndex];
                n -= numCombinedMacroblocks[macroblockIndex];
            } else if (pMacroblock->macroblockType == MB_MOTION_BACKWARD) {
                if (n > 1) {
                    pMacroblock1 = pMacroblock + 1;
                    if (pMacroblock->motionType == FRAME_STRUCTURE_MOTION_FRAME) {
                    	//combine up to 5 adjacent macroblocks together
						for( i = 1; i <= 5; i++){
							if ((pMacroblock1->macroblockType == MB_MOTION_BACKWARD) &&
								(pMacroblock1->motionType == FRAME_STRUCTURE_MOTION_FRAME) &&
								((pMacroblock1->hOffset - pMacroblock->hOffset) == 16 * i) &&
								(*(long *)&pMacroblock->PMV[0][1][0] == *(long *)&pMacroblock1->PMV[0][1][0])) {
									numCombinedMacroblocks[macroblockIndex]++;
							}else
								break;
							if(n <= i+1)
								break;
							pMacroblock1++;
						}
                    } else if (pMacroblock->motionType == FRAME_STRUCTURE_MOTION_FIELD) {
  						for( i = 1; i <= 5; i++){
							if ((pMacroblock1->macroblockType == MB_MOTION_BACKWARD) &&
								(pMacroblock1->motionType == FRAME_STRUCTURE_MOTION_FIELD) &&
								((pMacroblock1->hOffset - pMacroblock->hOffset) == 16 * i) &&
								(nvMoCompCheckFieldPredictionEqual(pMacroblock, pMacroblock1, pMacroblock->macroblockType))){
									numCombinedMacroblocks[macroblockIndex]++;
							}else
								break;
							if(n <= i+1)
								break;
							pMacroblock1 ++;
						}
                    }
                }

                if (pMacroblock->motionType == FRAME_STRUCTURE_MOTION_FRAME)
                    nvPredictFrameLumaMacroblock(pMacroblock, numCombinedMacroblocks[macroblockIndex]);
                else
                    hasFieldPredictions = TRUE;

                pMacroblock += numCombinedMacroblocks[macroblockIndex];
                n -= numCombinedMacroblocks[macroblockIndex];
            } else if (pMacroblock->macroblockType == (MB_MOTION_FORWARD | MB_MOTION_BACKWARD)) {
                if (n > 1) {
                    pMacroblock1 = pMacroblock + 1;
                    if (pMacroblock->motionType == FRAME_STRUCTURE_MOTION_FRAME) {
						//combine up to 5 adjacent macroblocks together
						for( i = 1; i <= 5; i++){
							if ((pMacroblock1->macroblockType == (MB_MOTION_FORWARD | MB_MOTION_BACKWARD)) &&
								(pMacroblock1->motionType == FRAME_STRUCTURE_MOTION_FRAME) &&
								((pMacroblock1->hOffset - pMacroblock->hOffset) == 16 * i) &&
								(*(long *)&pMacroblock->PMV[0][0][0] == *(long *)&pMacroblock1->PMV[0][0][0]) &&
								(*(long *)&pMacroblock->PMV[0][1][0] == *(long *)&pMacroblock1->PMV[0][1][0])) {
									numCombinedMacroblocks[macroblockIndex]++;
							}else
								break;
							if(n <= i+1)
								break;
							pMacroblock1 ++;
						}                        
                    } else if (pMacroblock->motionType == FRAME_STRUCTURE_MOTION_FIELD) {
						for( i = 1; i <= 5; i++){
							if ((pMacroblock1->macroblockType == (MB_MOTION_FORWARD | MB_MOTION_BACKWARD)) &&
								(pMacroblock1->motionType == FRAME_STRUCTURE_MOTION_FIELD) &&
								((pMacroblock1->hOffset - pMacroblock->hOffset) == 16 * i) &&
								(nvMoCompCheckFieldPredictionEqual(pMacroblock, pMacroblock1, pMacroblock->macroblockType))){
									numCombinedMacroblocks[macroblockIndex]++;
							}else
								break;
							if(n <= i+1)
								break;
							pMacroblock1 ++;
						}
                    }
                }

                if (pMacroblock->motionType == FRAME_STRUCTURE_MOTION_FRAME)
                    nvPredictFrameLumaMacroblock(pMacroblock, numCombinedMacroblocks[macroblockIndex]);
                else
                    hasFieldPredictions = TRUE;

                pMacroblock += numCombinedMacroblocks[macroblockIndex];
                n -= numCombinedMacroblocks[macroblockIndex];
            } else {
                if (pMacroblock->macroblockType != MB_INTRA) {

                    if (pMacroblock->motionType == FRAME_STRUCTURE_MOTION_FRAME)
                        nvPredictFrameLumaMacroblock(pMacroblock, numCombinedMacroblocks[macroblockIndex]);
                    else
                        hasFieldPredictions = TRUE;

                }
                pMacroblock++;
                n--;
            }
            macroblockIndex++;
        }

        nvPusherStart(TRUE);

        pMacroblock = pMacroblocks;
        n = numMacroblocks;
        macroblockIndex = 0;

#ifdef  DEBUG
        if (moCompDebugLevel < 3) {
#endif  // DEBUG

        // Change surface offsets to chroma portion of surface
        nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                      NV097_NO_OPERATION | 0x40000);
        nvPushData(1, 0);

        nvPushData(2, dDrawSubchannelOffset(NV_DD_KELVIN) +
                      NV097_SET_SURFACE_COLOR_OFFSET | 0x40000);
        nvPushData(3, nvMoCompParams.surfaceOffset + nvMoCompParams.surfaceChromaOffset);

        nvPushData(4, dDrawSubchannelOffset(NV_DD_KELVIN) +
                      NV097_SET_TEXTURE_OFFSET(0) | 0x40000);
        nvPushData(5, pDriverData->dwMCForwardSurfaceOffset + nvMoCompParams.surfaceChromaOffset);

        nvPushData(6, dDrawSubchannelOffset(NV_DD_KELVIN) +
                      NV097_SET_TEXTURE_OFFSET(1) | 0x40000);
        nvPushData(7, pDriverData->dwMCBackwardSurfaceOffset + nvMoCompParams.surfaceChromaOffset);

        nvPusherAdjust(8);

        nvMoCompParams.currentForwardField = 0;
        nvMoCompParams.currentBackwardField = 0;
        nvMoCompParams.currentDestinationField = 0;

        // Perform chroma prediction pass
        while (n > 0) {
            if (pMacroblock->macroblockType != MB_INTRA) {

                if (pMacroblock->motionType == FRAME_STRUCTURE_MOTION_FRAME)
                    nvPredictFrameChromaMacroblock(pMacroblock, numCombinedMacroblocks[macroblockIndex]);

            }
            pMacroblock += numCombinedMacroblocks[macroblockIndex];
            n -= numCombinedMacroblocks[macroblockIndex];
            macroblockIndex++;
        }

        nvPusherStart(TRUE);

#ifdef  DEBUG
        }
#endif  // DEBUG

        // Now do non-frame structured predictions if any are necessary
        if (hasFieldPredictions) {

            // Restore offsets and pitches to field based pitches
            nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                          NV097_NO_OPERATION | 0x40000);
            nvPushData(1, 0);

            nvPushData(2, dDrawSubchannelOffset(NV_DD_KELVIN) +
                          NV097_SET_SURFACE_COLOR_OFFSET | 0x40000);
            nvPushData(3, nvMoCompParams.surfaceOffset);

            nvPushData(4, dDrawSubchannelOffset(NV_DD_KELVIN) +
                          NV097_SET_TEXTURE_OFFSET(0) | 0x40000);
            nvPushData(5, pDriverData->dwMCForwardSurfaceOffset);

            nvPushData(6, dDrawSubchannelOffset(NV_DD_KELVIN) +
                          NV097_SET_TEXTURE_OFFSET(1) | 0x40000);
            nvPushData(7, pDriverData->dwMCBackwardSurfaceOffset);

            nvPusherAdjust(8);

            nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                          NV097_SET_SURFACE_PITCH | 0x40000);
            nvPushData(1, (surfacePitch << 1));

            nvPushData(2, dDrawSubchannelOffset(NV_DD_KELVIN) +
                           NV097_SET_TEXTURE_CONTROL1(0) | 0x40000);
            nvPushData(3, surfacePitch << 17);
            nvPushData(4, dDrawSubchannelOffset(NV_DD_KELVIN) +
                           NV097_SET_TEXTURE_CONTROL1(1) | 0x40000);
            nvPushData(5, surfacePitch << 17);

            nvPusherAdjust(6);

            pMacroblock = pMacroblocks;
            n = numMacroblocks;
            macroblockIndex = 0;

            nvMoCompParams.currentForwardField = 0;
            nvMoCompParams.currentBackwardField = 0;
            nvMoCompParams.currentDestinationField = 0;
            nvMoCompParams.destinationFieldIs0 = TRUE; // First process all destination field 0 updates

            // Perform luma prediction pass
            while (n > 0) {
                if (pMacroblock->macroblockType != MB_INTRA) {

                    if (pMacroblock->motionType != FRAME_STRUCTURE_MOTION_FRAME)
                        nvPredictFrameLumaMacroblock(pMacroblock, numCombinedMacroblocks[macroblockIndex]);

                }
                pMacroblock += numCombinedMacroblocks[macroblockIndex];
                n -= numCombinedMacroblocks[macroblockIndex];
                macroblockIndex++;
            }

            nvPusherStart(TRUE);

            // Change surface offsets to chroma portion of surface
            nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                          NV097_NO_OPERATION | 0x40000);
            nvPushData(1, 0);

            nvPushData(2, dDrawSubchannelOffset(NV_DD_KELVIN) +
                          NV097_SET_SURFACE_COLOR_OFFSET | 0x40000);
            nvPushData(3, nvMoCompParams.surfaceOffset + nvMoCompParams.surfaceChromaOffset);

            nvPushData(4, dDrawSubchannelOffset(NV_DD_KELVIN) +
                          NV097_SET_TEXTURE_OFFSET(0) | 0x40000);
            nvPushData(5, pDriverData->dwMCForwardSurfaceOffset + nvMoCompParams.surfaceChromaOffset);

            nvPushData(6, dDrawSubchannelOffset(NV_DD_KELVIN) +
                          NV097_SET_TEXTURE_OFFSET(1) | 0x40000);
            nvPushData(7, pDriverData->dwMCBackwardSurfaceOffset + nvMoCompParams.surfaceChromaOffset);

            nvPusherAdjust(8);

            pMacroblock = pMacroblocks;
            n = numMacroblocks;
            macroblockIndex = 0;

            nvMoCompParams.currentForwardField = 0;
            nvMoCompParams.currentBackwardField = 0;
            nvMoCompParams.currentDestinationField = 0;
            nvMoCompParams.destinationFieldIs0 = TRUE; // First process all destination field 0 updates

            // Perform chroma prediction pass
            while (n > 0) {
                if (pMacroblock->macroblockType != MB_INTRA) {

                    if (pMacroblock->motionType != FRAME_STRUCTURE_MOTION_FRAME)
                        nvPredictFrameChromaMacroblock(pMacroblock, numCombinedMacroblocks[macroblockIndex]);

                }
                pMacroblock += numCombinedMacroblocks[macroblockIndex];
                n -= numCombinedMacroblocks[macroblockIndex];
                macroblockIndex++;
            }

            nvPusherStart(TRUE);
        }
    }

    if (!(nvInitKelvinForMoCompCorrection()))
        return FALSE;

    SET_KELVIN_BIDIRECTIONAL_VERTEX_ARRAY_MODE();

    nvDoAllIntraFieldCorrections = nvDoAllKelvinIntraDVDFieldCorrections;
    nvDoAllFieldCorrections = nvDoAllKelvinDVDFieldCorrections;
    nvDoIntraFieldCorrection = nvDoKelvinIntraDVDFieldCorrection;
    nvDoOptimizedIntraFieldCorrection = nvDoOptimizedKelvinIntraDVDFieldCorrection;
    nvDoFieldCorrection = nvDoKelvinDVDFieldCorrection;

    nvDoAllIntraFrameCorrections = nvDoAllKelvinIntraFrameCorrections;
    nvDoAllFrameCorrections = nvDoAllKelvinFrameCorrections;
    nvDoIntraFrameCorrection = nvDoKelvinIntraFrameCorrection;
    nvDoFrameCorrection = nvDoKelvinFrameCorrection;

    // Default motion comp correction ICW
    moCompCorrectionICW = (NV097_SET_COMBINER_COLOR_ICW_A_MAP_UNSIGNED_IDENTITY << 29) |
                          (NV097_SET_COMBINER_COLOR_ICW_A_ALPHA_FALSE << 28) |
                          (NV097_SET_COMBINER_COLOR_ICW_A_SOURCE_REG_1 << 24) |

                          (NV097_SET_COMBINER_COLOR_ICW_B_MAP_HALFBIAS_NORMAL << 21) |
                          (NV097_SET_COMBINER_COLOR_ICW_B_ALPHA_FALSE << 20) |
                          (NV097_SET_COMBINER_COLOR_ICW_B_SOURCE_REG_8 << 16) |

                          (NV097_SET_COMBINER_COLOR_ICW_C_MAP_UNSIGNED_IDENTITY << 13) |
                          (NV097_SET_COMBINER_COLOR_ICW_C_ALPHA_FALSE << 12) |
                          (NV097_SET_COMBINER_COLOR_ICW_C_SOURCE_REG_1 << 8) |

                          (NV097_SET_COMBINER_COLOR_ICW_D_MAP_UNSIGNED_IDENTITY << 5) |
                          (NV097_SET_COMBINER_COLOR_ICW_D_ALPHA_FALSE << 4) |
                           NV097_SET_COMBINER_COLOR_ICW_D_SOURCE_REG_9;

    // Non-prebiased motion comp correction ICW
    moCompNonPrebiasedCorrectionICW = (NV097_SET_COMBINER_COLOR_ICW_A_MAP_UNSIGNED_IDENTITY << 29) |
                                      (NV097_SET_COMBINER_COLOR_ICW_A_ALPHA_FALSE << 28) |
                                      (NV097_SET_COMBINER_COLOR_ICW_A_SOURCE_REG_1 << 24) |

                                      (NV097_SET_COMBINER_COLOR_ICW_B_MAP_UNSIGNED_IDENTITY << 21) |
                                      (NV097_SET_COMBINER_COLOR_ICW_B_ALPHA_FALSE << 20) |
                                      (NV097_SET_COMBINER_COLOR_ICW_B_SOURCE_REG_8 << 16) |

                                      (NV097_SET_COMBINER_COLOR_ICW_C_MAP_UNSIGNED_IDENTITY << 13) |
                                      (NV097_SET_COMBINER_COLOR_ICW_C_ALPHA_FALSE << 12) |
                                      (NV097_SET_COMBINER_COLOR_ICW_C_SOURCE_REG_0 << 8) |

                                      (NV097_SET_COMBINER_COLOR_ICW_D_MAP_UNSIGNED_IDENTITY << 5) |
                                      (NV097_SET_COMBINER_COLOR_ICW_D_ALPHA_FALSE << 4) |
                                       NV097_SET_COMBINER_COLOR_ICW_D_SOURCE_REG_0;

    nvMoCompParams.currentDestinationField = 0;
    nvMoCompParams.destinationField = 0;

    pMacroblock = pMacroblocks;
    n = numMacroblocks;

    // This assumes that the first IDCT index of the first macroblock in the command stream is the
    // lowest index in the entire command stream for this call.

    dwSurfaceOffset = pDriverData->dwMCDestinationSurfaceOffset;

    macroblockIndex = 0;
    index = 0;

    while (n > 0) {

        macroblockLimit = n;

        if (macroblockLimit > 32)
            macroblockLimit = 32;

        pDriverData->dwMCFirstAlignedIDCTIndex = pMacroblock->firstIDCTIndex & ~1;

        // Attempt to sanity check index
        if ((pDriverData->dwMCFirstAlignedIDCTIndex << 6) > pDriverData->dwMCIDCTAGPCtxDmaSize[0])
            pDriverData->dwMCFirstAlignedIDCTIndex = 0;

        dwFirstAlignedIDCTOffset = pDriverData->dwMCFirstAlignedIDCTIndex * 64; // 64 bytes per IDCT block

        // Force both texture caches to flush
        nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                      NV097_NO_OPERATION | 0x40000);
        nvPushData(1, 0);

        nvPushData(2, dDrawSubchannelOffset(NV_DD_KELVIN) +
                      NV097_SET_SURFACE_COLOR_OFFSET | 0x40000);
        nvPushData(3, dwSurfaceOffset);

        nvPushData(4, dDrawSubchannelOffset(NV_DD_KELVIN) +
                      NV097_SET_TEXTURE_OFFSET(0) | 0x40000);
        nvPushData(5, dwFirstAlignedIDCTOffset);
        nvPushData(6, dDrawSubchannelOffset(NV_DD_KELVIN) +
                      NV097_SET_TEXTURE_OFFSET(1) | 0x40000);
        nvPushData(7, dwSurfaceOffset);

        nvPusherAdjust(8);

        if (pDriverData->bMCCurrentExecuteFunction ==
            EXECUTE_NON_PREBIASED_MOTION_COMPENSATION) {
            // Modify combiners to perform non-prebiased intrablock correction
            nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                          NV097_SET_COMBINER_COLOR_ICW(0) | 0x40000);
            nvPushData(1, moCompNonPrebiasedCorrectionICW);

            nvPusherAdjust(2);

            SET_KELVIN_UNBIASED_CORRECTION_DATA_FORMAT();
        } else {
            SET_KELVIN_CORRECTION_DATA_FORMAT();
        }   

        if ((pDriverData->bMCPictureCodingType == PICTURE_TYPE_I) &&
            (pMacroblock->codedBlockPattern == 0xFF)) {
            nvCorrectKelvinIntraFrame(pMacroblock, macroblockLimit);
            pMacroblock += macroblockLimit;
            n -= macroblockLimit;
        } else {
            long saveMacroblockLimit = macroblockLimit;
            long fieldTypeCount = 0;
            BOOL hasPartialCorrections = FALSE;

            pMacroblock1 = pMacroblock;

            nvMoCompParams.destinationFieldIs0 = TRUE;

            // Change texture1 surface offset to point to block of half bias values (0x80)
            nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                          NV097_SET_TEXTURE_OFFSET(1) | 0x40000);
            nvPushData(1, nvMoCompParams.surfaceOffset + nvMoCompParams.surfaceBiasedIntraBlockOffset);

            // Change texture0 pitch to 16 bytes
            nvPushData(2, dDrawSubchannelOffset(NV_DD_KELVIN) +
                           NV097_SET_TEXTURE_CONTROL1(0) | 0x40000);
            nvPushData(3, 16 << 16);
            nvPushData(4, dDrawSubchannelOffset(NV_DD_KELVIN) +
                           NV097_SET_TEXTURE_CONTROL1(1) | 0x40000);
            nvPushData(5, 16 << 16); // half bias Intra block pitch

            nvPusherAdjust(6);

            // Perform first correction pass
            while (--macroblockLimit >= 0) {
                if ((pMacroblock->overflowCodedBlockPattern != 0) && (pMacroblock->macroblockType != MB_INTRA))
                    numCombinedMacroblocks[macroblockIndex++] = (BYTE) index;
                index++;
                if ((pMacroblock->DCTType != DCT_FIELD) && (pMacroblock->macroblockType == MB_INTRA)) {
                    if (pMacroblock->codedBlockPattern >= 0xFC) {
                        nvCorrectKelvinFullFrameLumaMacroblock(pMacroblock, CORRECTION_FIRST_PASS);
                    } else if (pMacroblock->codedBlockPattern != 0) {
                        // Change texture0 pitch to it's default value of 8 bytes
                        nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                                       NV097_SET_TEXTURE_CONTROL1(0) | 0x40000);
                        nvPushData(1, 8 << 16);

                        nvPusherAdjust(2);

                        nvCorrectKelvinFrameLumaMacroblock(pMacroblock, CORRECTION_FIRST_PASS);

                        // Change texture0 pitch to 16 bytes
                        nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                                       NV097_SET_TEXTURE_CONTROL1(0) | 0x40000);
                        nvPushData(1, 16 << 16);

                        nvPusherAdjust(2);

                    }
                } else
                    fieldTypeCount++;

                pMacroblock++;
                --n;
            }

            nvPusherStart(TRUE);

            if (pDriverData->bMCCurrentExecuteFunction == EXECUTE_NON_PREBIASED_MOTION_COMPENSATION) {
                SET_KELVIN_UNBIASED_CHROMA_CORRECTION_DATA_FORMAT();
            } else {
                SET_KELVIN_CHROMA_CORRECTION_DATA_FORMAT();
            }   

            pMacroblock = pMacroblock1;
            macroblockLimit = saveMacroblockLimit;

            while (--macroblockLimit >= 0) {
                if (pMacroblock->macroblockType == MB_INTRA) {
                    if (pMacroblock->codedBlockPattern == 0xFF) {
                        nvCorrectKelvinFullFrameChromaMacroblock(pMacroblock, CORRECTION_FIRST_PASS);
                    } else if (pMacroblock->codedBlockPattern != 0) {
                        nvCorrectKelvinFrameChromaMacroblock(pMacroblock, CORRECTION_FIRST_PASS);
                    }
                }
                pMacroblock++;
            }

            nvPusherStart(TRUE);

            // Force both texture caches to flush
            nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                          NV097_NO_OPERATION | 0x40000);
            nvPushData(1, 0);

            nvPushData(2, dDrawSubchannelOffset(NV_DD_KELVIN) +
                          NV097_SET_TEXTURE_OFFSET(1) | 0x40000);
            nvPushData(3, dwSurfaceOffset);

            nvPusherAdjust(4);

            // Change texture0 pitch to 16 bytes
            nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                           NV097_SET_TEXTURE_CONTROL1(0) | 0x40000);
            nvPushData(1, 16 << 16);
            nvPushData(2, dDrawSubchannelOffset(NV_DD_KELVIN) +
                           NV097_SET_TEXTURE_CONTROL1(1) | 0x40000);
            nvPushData(3, nvMoCompParams.surfaceLumaPitch << 16);

            nvPusherAdjust(4);

            nvMoCompParams.destinationFieldIs0 = TRUE;

            pMacroblock = pMacroblock1;
            macroblockLimit = saveMacroblockLimit;

            if (pDriverData->bMCCurrentExecuteFunction ==
                EXECUTE_NON_PREBIASED_MOTION_COMPENSATION) {
                // Modify combiners to perform interblock correction
                nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                              NV097_SET_COMBINER_COLOR_ICW(0) | 0x40000);
                nvPushData(1, moCompCorrectionICW);

                nvPusherAdjust(2);

            }    

            SET_KELVIN_CORRECTION_DATA_FORMAT();

            // Perform first correction pass
            while (--macroblockLimit >= 0) {
                if ((pMacroblock->DCTType != DCT_FIELD) && (pMacroblock->macroblockType != MB_INTRA)) {
                    if (pMacroblock->codedBlockPattern >= 0xFC) {
                        nvCorrectKelvinFullFrameLumaMacroblock(pMacroblock, CORRECTION_FIRST_PASS);
                    } else if (pMacroblock->codedBlockPattern != 0) {
                        // Change texture0 pitch to it's default value of 8 bytes
                        nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                                       NV097_SET_TEXTURE_CONTROL1(0) | 0x40000);
                        nvPushData(1, 8 << 16);

                        nvPusherAdjust(2);

                        nvCorrectKelvinFrameLumaMacroblock(pMacroblock, CORRECTION_FIRST_PASS);

                        // Change texture0 pitch to 16 bytes
                        nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                                       NV097_SET_TEXTURE_CONTROL1(0) | 0x40000);
                        nvPushData(1, 16 << 16);

                        nvPusherAdjust(2);

                    }
                }
                pMacroblock++;
            }

            nvPusherStart(TRUE);

            SET_KELVIN_CHROMA_CORRECTION_DATA_FORMAT();

            pMacroblock = pMacroblock1;
            macroblockLimit = saveMacroblockLimit;

            while (--macroblockLimit >= 0) {
                if (pMacroblock->macroblockType != MB_INTRA) {
                    if (pMacroblock->codedBlockPattern == 0xFF) {
                        nvCorrectKelvinFullFrameChromaMacroblock(pMacroblock, CORRECTION_FIRST_PASS);
                    } else if (pMacroblock->codedBlockPattern != 0) {
                        nvCorrectKelvinFrameChromaMacroblock(pMacroblock, CORRECTION_FIRST_PASS);
                    }
                }
                pMacroblock++;
            }

            if (fieldTypeCount > 0) {

                nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                              NV097_NO_OPERATION | 0x40000);
                nvPushData(1, 0);

                // Change destination pitch to field pitch
                nvPushData(2, dDrawSubchannelOffset(NV_DD_KELVIN) +
                              NV097_SET_SURFACE_PITCH | 0x40000);
                nvPushData(3, ((nvMoCompParams.surfaceLumaPitch << 17) | (nvMoCompParams.surfaceLumaPitch << 1)));

                // Change texture1 surface offset to point to block of half bias values (0x80)
                nvPushData(4, dDrawSubchannelOffset(NV_DD_KELVIN) +
                              NV097_SET_TEXTURE_OFFSET(1) | 0x40000);
                nvPushData(5, nvMoCompParams.surfaceOffset + nvMoCompParams.surfaceBiasedIntraBlockOffset);

                // Change texture0 pitch to 16 bytes
                nvPushData(6, dDrawSubchannelOffset(NV_DD_KELVIN) +
                               NV097_SET_TEXTURE_CONTROL1(0) | 0x40000);
                nvPushData(7, 16 << 16);
                nvPushData(8, dDrawSubchannelOffset(NV_DD_KELVIN) +
                               NV097_SET_TEXTURE_CONTROL1(1) | 0x40000);
                nvPushData(9, 16 << 16); // half bias Intra block pitch

                nvPusherAdjust(10);

                if (pDriverData->bMCCurrentExecuteFunction ==
                    EXECUTE_NON_PREBIASED_MOTION_COMPENSATION) {
                    // Modify combiners to perform non-prebiased intrablock correction
                    nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                                  NV097_SET_COMBINER_COLOR_ICW(0) | 0x40000);
                    nvPushData(1, moCompNonPrebiasedCorrectionICW);

                    nvPusherAdjust(2);

                    SET_KELVIN_UNBIASED_CORRECTION_DATA_FORMAT();
                } else {
                    SET_KELVIN_CORRECTION_DATA_FORMAT();
                }    

                pMacroblock = pMacroblock1;
                macroblockLimit = saveMacroblockLimit;

                while (--macroblockLimit >= 0) {
                    if ((pMacroblock->DCTType == DCT_FIELD) && (pMacroblock->macroblockType == MB_INTRA)) {
                        if (pMacroblock->codedBlockPattern >= 0xFC) {
                            nvCorrectKelvinFullFrameLumaMacroblock(pMacroblock, CORRECTION_FIRST_PASS);
                        } else if (pMacroblock->codedBlockPattern != 0) {
                            // Change texture0 pitch to it's default value of 8 bytes
                            nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                                           NV097_SET_TEXTURE_CONTROL1(0) | 0x40000);
                            nvPushData(1, 8 << 16);

                            nvPusherAdjust(2);

                            hasPartialCorrections = TRUE;

                            nvCorrectKelvinFrameLumaMacroblock(pMacroblock, CORRECTION_FIRST_PASS);

                            // Change texture0 pitch to 16 bytes
                            nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                                           NV097_SET_TEXTURE_CONTROL1(0) | 0x40000);
                            nvPushData(1, 16 << 16);

                            nvPusherAdjust(2);
                        }
                    }
                    pMacroblock++;
                }

                // Force both texture caches to flush
                nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                              NV097_NO_OPERATION | 0x40000);
                nvPushData(1, 0);

                nvPushData(2, dDrawSubchannelOffset(NV_DD_KELVIN) +
                              NV097_SET_TEXTURE_OFFSET(0) | 0x40000);
                nvPushData(3, dwFirstAlignedIDCTOffset);
                nvPushData(4, dDrawSubchannelOffset(NV_DD_KELVIN) +
                              NV097_SET_TEXTURE_OFFSET(1) | 0x40000);
                nvPushData(5, dwSurfaceOffset);

                nvPusherAdjust(6);

                // Change texture0 pitch to 16 bytes
                nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                               NV097_SET_TEXTURE_CONTROL1(0) | 0x40000);
                nvPushData(1, 16 << 16);
                nvPushData(2, dDrawSubchannelOffset(NV_DD_KELVIN) +
                               NV097_SET_TEXTURE_CONTROL1(1) | 0x40000);
                nvPushData(3, nvMoCompParams.surfaceLumaPitch << 16);

                nvPusherAdjust(4);

                if (pDriverData->bMCCurrentExecuteFunction ==
                    EXECUTE_NON_PREBIASED_MOTION_COMPENSATION) {
                    // Modify combiners to perform interblock correction
                    nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                                  NV097_SET_COMBINER_COLOR_ICW(0) | 0x40000);
                    nvPushData(1, moCompCorrectionICW);

                    nvPusherAdjust(2);
                }    

                SET_KELVIN_CORRECTION_DATA_FORMAT();

                nvMoCompParams.destinationFieldIs0 = TRUE;

                pMacroblock = pMacroblock1;
                macroblockLimit = saveMacroblockLimit;

                while (--macroblockLimit >= 0) {
                    if ((pMacroblock->DCTType == DCT_FIELD) && (pMacroblock->macroblockType != MB_INTRA)) {
                        if (pMacroblock->codedBlockPattern >= 0xFC) {
                            nvCorrectKelvinFullFrameLumaMacroblock(pMacroblock, CORRECTION_FIRST_PASS);
                        } else if (pMacroblock->codedBlockPattern != 0) {
                            // Change texture0 pitch to it's default value of 8 bytes
                            nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                                           NV097_SET_TEXTURE_CONTROL1(0) | 0x40000);
                            nvPushData(1, 8 << 16);

                            nvPusherAdjust(2);

                            hasPartialCorrections = TRUE;

                            nvCorrectKelvinFrameLumaMacroblock(pMacroblock, CORRECTION_FIRST_PASS);

                            // Change texture0 pitch to 16 bytes
                            nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                                           NV097_SET_TEXTURE_CONTROL1(0) | 0x40000);
                            nvPushData(1, 16 << 16);

                            nvPusherAdjust(2);
                        }
                    }
                    pMacroblock++;
                }

                // Restore surface pitch
                nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                              NV097_SET_SURFACE_PITCH | 0x40000);
                nvPushData(1, ((nvMoCompParams.surfaceLumaPitch << 16) | nvMoCompParams.surfaceLumaPitch));

                nvPusherAdjust(2);
            }
        }
        nvPusherStart(TRUE);
    }

    pLastMacroblock = pMacroblock - 1;

    if ((pDriverData->bMCPictureCodingType != PICTURE_TYPE_I) && (macroblockIndex > 0)) {
        long saveN = macroblockIndex;

        n = macroblockIndex;
        macroblockIndex = 0;
        pMacroblock1 = pMacroblocks;
        index = -32;

        nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                      NV097_NO_OPERATION | 0x40000);
        nvPushData(1, 0);

        // DO NOT REMOVE: This is necessary to work around an NV10 texture cache bug
        nvPushData(2, dDrawSubchannelOffset(NV_DD_KELVIN) +
                      NV097_SET_SURFACE_COLOR_OFFSET | 0x40000);
        nvPushData(3, dwSurfaceOffset);

        nvPusherAdjust(4);

        nvMoCompParams.currentDestinationField = 0;
        nvMoCompParams.destinationField = 0;

        // Change destination pitch back to frame pitch
        nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                      NV097_SET_SURFACE_PITCH | 0x40000);
        nvPushData(1, ((nvMoCompParams.surfaceLumaPitch << 16) | nvMoCompParams.surfaceLumaPitch));

        nvPushData(2, dDrawSubchannelOffset(NV_DD_KELVIN) +
                       NV097_SET_TEXTURE_OFFSET(1) | 0x40000);
        nvPushData(3, nvMoCompParams.surfaceOffset);

        // Change texture0 pitch to 16 bytes
        nvPushData(4, dDrawSubchannelOffset(NV_DD_KELVIN) +
                       NV097_SET_TEXTURE_CONTROL1(0) | 0x40000);
        nvPushData(5, 16 << 16);
        nvPushData(6, dDrawSubchannelOffset(NV_DD_KELVIN) +
                       NV097_SET_TEXTURE_CONTROL1(1) | 0x40000);
        nvPushData(7, nvMoCompParams.surfaceLumaPitch << 16);

        nvPusherAdjust(8);

        if (pDriverData->bMCCurrentExecuteFunction ==
            EXECUTE_NON_PREBIASED_MOTION_COMPENSATION) {
            // Modify combiners to perform interblock correction
            nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                          NV097_SET_COMBINER_COLOR_ICW(0) | 0x40000);
            nvPushData(1, moCompCorrectionICW);

            nvPusherAdjust(2);
        }    

        SET_KELVIN_CHROMA_CORRECTION_DATA_FORMAT();

        while (--n >= 0) {

            pMacroblock = &pMacroblock1[numCombinedMacroblocks[macroblockIndex]];

            firstAlignedIDCTIndex = pMacroblock->firstIDCTIndex & ~1;

            if (firstAlignedIDCTIndex - index >= 32) {
                pDriverData->dwMCFirstAlignedIDCTIndex = firstAlignedIDCTIndex;

                index = firstAlignedIDCTIndex;

                // Attempt to sanity check index
                if ((pDriverData->dwMCFirstAlignedIDCTIndex << 6) > pDriverData->dwMCIDCTAGPCtxDmaSize[0])
                    pDriverData->dwMCFirstAlignedIDCTIndex = 0;

                dwFirstAlignedIDCTOffset = pDriverData->dwMCFirstAlignedIDCTIndex * 64; // 64 bytes per IDCT block

                // Force both texture caches to flush
                nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                              NV097_SET_TEXTURE_OFFSET(0) | 0x40000);
                nvPushData(1, dwFirstAlignedIDCTOffset);
                nvPushData(2, dDrawSubchannelOffset(NV_DD_KELVIN) +
                              NV097_SET_TEXTURE_OFFSET(1) | 0x40000);
                nvPushData(3, dwSurfaceOffset);

                nvPusherAdjust(4);

                nvMoCompParams.currentDestinationField = 0;
                nvMoCompParams.destinationField = 0;
            }

            // Perform chroma overflow correction pass
            nvCorrectKelvinFrameChromaMacroblock(pMacroblock, CORRECTION_OVERFLOW_PASS);

            macroblockIndex++;
        }

        nvPusherStart(TRUE);

        n = saveN;
        macroblockIndex = 0;
        index = -32;

        // Change texture0 pitch to it's default value of 8 bytes
        nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                      NV097_SET_TEXTURE_CONTROL1(0) | 0x40000);
        nvPushData(1, 8 << 16);
        nvPushData(2, dDrawSubchannelOffset(NV_DD_KELVIN) +
                      NV097_SET_TEXTURE_CONTROL1(1) | 0x40000);
        nvPushData(3, nvMoCompParams.surfaceLumaPitch << 16);

        nvPusherAdjust(4);

        SET_KELVIN_CORRECTION_DATA_FORMAT();

        while (--n >= 0) {

            pMacroblock = &pMacroblock1[numCombinedMacroblocks[macroblockIndex]];

            firstAlignedIDCTIndex = pMacroblock->firstIDCTIndex & ~1;

            if (firstAlignedIDCTIndex - index >= 32) {
                pDriverData->dwMCFirstAlignedIDCTIndex = firstAlignedIDCTIndex;

                index = firstAlignedIDCTIndex;

                // Attempt to sanity check index
                if ((pDriverData->dwMCFirstAlignedIDCTIndex << 6) > pDriverData->dwMCIDCTAGPCtxDmaSize[0])
                    pDriverData->dwMCFirstAlignedIDCTIndex = 0;

                dwFirstAlignedIDCTOffset = pDriverData->dwMCFirstAlignedIDCTIndex * 64; // 64 bytes per IDCT block

                // Force both texture caches to flush
                nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                              NV097_SET_TEXTURE_OFFSET(0) | 0x40000);
                nvPushData(1, dwFirstAlignedIDCTOffset);
                nvPushData(2, dDrawSubchannelOffset(NV_DD_KELVIN) +
                              NV097_SET_TEXTURE_OFFSET(1) | 0x40000);
                nvPushData(3, dwSurfaceOffset);

                nvPusherAdjust(4);

                nvMoCompParams.currentDestinationField = 0;
                nvMoCompParams.destinationField = 0;
            }


            // Perform luma overflow correction pass
            if (pMacroblock->DCTType == DCT_FIELD) {

                nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                              NV097_NO_OPERATION | 0x40000);
                nvPushData(1, 0);

                nvPushData(2, dDrawSubchannelOffset(NV_DD_KELVIN) +
                              NV097_SET_SURFACE_PITCH | 0x40000);
                nvPushData(3, ((nvMoCompParams.surfaceLumaPitch << 17) | (nvMoCompParams.surfaceLumaPitch << 1)));

                // Change texture0 pitch to default value of 8 bytes
                nvPushData(4, dDrawSubchannelOffset(NV_DD_KELVIN) +
                              NV097_SET_TEXTURE_CONTROL1(0) | 0x40000);
                nvPushData(5, 8 << 16);
                nvPushData(6, dDrawSubchannelOffset(NV_DD_KELVIN) +
                              NV097_SET_TEXTURE_CONTROL1(1) | 0x40000);
                nvPushData(7, nvMoCompParams.surfaceLumaPitch << 17);

                nvPusherAdjust(8);

                nvCorrectKelvinFrameLumaMacroblock(pMacroblock, CORRECTION_OVERFLOW_PASS);

                nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                              NV097_NO_OPERATION | 0x40000);
                nvPushData(1, 0);

                // Restore surface pitch
                nvPushData(2, dDrawSubchannelOffset(NV_DD_KELVIN) +
                              NV097_SET_SURFACE_PITCH | 0x40000);
                nvPushData(3, ((nvMoCompParams.surfaceLumaPitch << 16) | nvMoCompParams.surfaceLumaPitch));

                // Change texture0 pitch to default value of 8 bytes
                nvPushData(4, dDrawSubchannelOffset(NV_DD_KELVIN) +
                               NV097_SET_TEXTURE_CONTROL1(0) | 0x40000);
                nvPushData(5, 8 << 16);
                nvPushData(6, dDrawSubchannelOffset(NV_DD_KELVIN) +
                               NV097_SET_TEXTURE_CONTROL1(1) | 0x40000);
                nvPushData(7, nvMoCompParams.surfaceLumaPitch << 16);

                nvPusherAdjust(8);

            } else
                nvCorrectKelvinFrameLumaMacroblock(pMacroblock, CORRECTION_OVERFLOW_PASS);

            macroblockIndex++;
        }

        nvPusherStart(TRUE);
    }

    if ((pLastMacroblock->vOffset >= maxHeight) && (pLastMacroblock->hOffset >= maxWidth))
        surfaceDecodeStatus = 15;
    else if ((unsigned long)pLastMacroblock->vOffset > (nvMoCompParams.surfaceLumaHeight - (nvMoCompParams.surfaceLumaHeight >> 2)))
        surfaceDecodeStatus = 7;
    else if ((unsigned long)pLastMacroblock->vOffset > (nvMoCompParams.surfaceLumaHeight >> 1))
        surfaceDecodeStatus = 3;
    else if ((unsigned long)pLastMacroblock->vOffset > (nvMoCompParams.surfaceLumaHeight >> 2))
        surfaceDecodeStatus = 1;

    if (surfaceDecodeStatus > formatConversionStatus) {
        LPNVMCSURFACEFLAGS lpSurfaceFlags;

        nvMoCompConvertSurfaceFormat(dst->fpVidMem, TRUE, (DWORD)surfaceDecodeStatus);

        lpSurfaceFlags = (LPNVMCSURFACEFLAGS)&pDriverData->nvMCSurfaceFlags[0];

        while ((lpSurfaceFlags->dwMCSurfaceBase != 0) &&
               (lpSurfaceFlags->dwMCSurfaceBase != dst->fpVidMem))
            lpSurfaceFlags++;

            if ((lpSurfaceFlags->bMCFrameAlreadyFormatConverted == 15) &&
                (lpSurfaceFlags->bMCFrameIsFiltered == FALSE) &&
                (pDriverData->bMCTemporalFilterDisabled == FALSE))
                nvMoCompTemporalFilter(dst->fpVidMem, pDriverData->bMCPercentCurrentField);
    }

    return TRUE;

} /* nvMoCompProcessKelvinFrameMacroblocks */


/*
 * nvMoCompProcessKelvinFieldMacroblocks
 *
 * Parses the current macroblock command stream and performs the necessary operations on
 * each macroblock.
 *
 *
 * Note that pDriverData must be set before calling this function
 *
 */
DWORD __stdcall nvMoCompProcessKelvinFieldMacroblocks(LPNVDECODEMACROBLOCK pMacroblocks,
                                                      DWORD numMacroblocks, BYTE formatConversionStatus)
{
    LPDDRAWI_DDRAWSURFACE_LCL   dstx = (LPDDRAWI_DDRAWSURFACE_LCL)pDriverData->dwMCDestinationSurface;
    LPDDRAWI_DDRAWSURFACE_GBL   dst = dstx->lpGbl;
    LPNVDECODEMACROBLOCK        pMacroblock;
    LPNVDECODEMACROBLOCK        pLastMacroblock;
    LPNVDECODEMACROBLOCK        pMacroblock1;
    unsigned long               dwFirstAlignedIDCTOffset;
    unsigned long               dwSurfaceOffset;
    unsigned long               moCompCorrectionICW;
    unsigned long               moCompNonPrebiasedCorrectionICW;
    long                        macroblockLimit;
    long                        n;
    long                        macroblockIndex;
    BYTE                        surfaceDecodeStatus = 0;
    BYTE                        numCombinedMacroblocks[256];
    short                       maxHeight = (short)(dst->wHeight >> 1) - 16;
    short                       maxWidth = (short)dst->wWidth - 16;
    int                         index,i;
    int                         firstAlignedIDCTIndex;

    nvMoCompParams.vertexMode = 0; // Initialize vertexMode to an invalid value
    nvMoCompParams.surfaceLumaPitch = dst->lPitch;
    nvMoCompParams.surfaceLumaHeight = (dst->wHeight + 1) & ~1;
    nvMoCompParams.surfaceOffset = VIDMEM_OFFSET(dst->fpVidMem);
    nvMoCompParams.surfaceChromaOffset = nvMoCompParams.surfaceLumaPitch * nvMoCompParams.surfaceLumaHeight;
    nvMoCompParams.surfaceBiasedIntraBlockOffset =
        (dst->lPitch * dst->wHeight) + (dst->lPitch * (dst->wHeight >> 1) + (8 * dst->lPitch));

    if (pDriverData->bMCPictureCodingType != PICTURE_TYPE_I) {

        if (!(nvInitKelvinForMoCompPrediction()))
            return FALSE;

        nvDoBidirectionalFieldPrediction = nvDoKelvinBidirectionalDVDFieldPrediction;
        nvDoTwoBidirectionalFieldPredictions = nvDoTwoKelvinBidirectionalDVDFieldPredictions;
        nvDoForwardFieldPrediction = nvDoKelvinForwardDVDFieldPrediction;
        nvDoTwoForwardFieldPredictions = nvDoTwoKelvinForwardDVDFieldPredictions;
        nvDoBackwardFieldPrediction = nvDoKelvinBackwardDVDFieldPrediction;
        nvDoTwoBackwardFieldPredictions = nvDoTwoKelvinBackwardDVDFieldPredictions;

        nvPredictFrameLumaMacroblock = nvPredictKelvinFrameLumaMacroblock;
        nvPredictFrameChromaMacroblock = nvPredictKelvinFrameChromaMacroblock;
        nvPredictFieldLumaMacroblock = nvPredictKelvinFieldLumaMacroblock;
        nvPredictFieldChromaMacroblock = nvPredictKelvinFieldChromaMacroblock;

        nvMoCompParams.currentForwardField = 0;
        nvMoCompParams.currentBackwardField = 0;
        nvMoCompParams.currentDestinationField = 0;

        if (pDriverData->bMCPictureStructure == PICTURE_STRUCTURE_BOTTOM_FIELD) {

            nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                          NV097_NO_OPERATION | 0x40000);
            nvPushData(1, 0);

            nvPushData(2, dDrawSubchannelOffset(NV_DD_KELVIN) +
                          NV097_SET_SURFACE_COLOR_OFFSET | 0x40000);
            nvPushData(3, nvMoCompParams.surfaceOffset + nvMoCompParams.surfaceLumaPitch);

            nvPusherAdjust(4);

            nvMoCompParams.currentDestinationField = 1;
        }

        pMacroblock = pMacroblocks;
        n = numMacroblocks;
        macroblockIndex = 0;

        // Perform luma prediction pass
        while (n > 0) {
            numCombinedMacroblocks[macroblockIndex] = 1;
            if (pMacroblock->macroblockType == MB_MOTION_FORWARD) {
                if (n > 1) {
                    pMacroblock1 = pMacroblock + 1;
                    if (pMacroblock->motionType == FIELD_STRUCTURE_MOTION_FIELD) {
						//combine up to 5 adjacent macroblocks together
						for( i = 1; i <= 5; i++){
							if ((pMacroblock1->macroblockType == MB_MOTION_FORWARD) &&
								(pMacroblock1->motionType == FIELD_STRUCTURE_MOTION_FIELD) &&
                                ((pMacroblock1->hOffset - pMacroblock->hOffset) == 16 * i) &&
                                (pMacroblock->fieldSelect[0][0] == pMacroblock1->fieldSelect[0][0]) &&
								(*(long *)&pMacroblock->PMV[0][0][0] == *(long *)&pMacroblock1->PMV[0][0][0])) {
									numCombinedMacroblocks[macroblockIndex]++;
							}else
								break;
							if(n <= i+1)
								break;		//no more macroblock left
							pMacroblock1++;
                        }
					}else if (pMacroblock->motionType == FIELD_STRUCTURE_MOTION_16X8) {
						//combine up to 5 adjacent macroblocks together
						for( i = 1; i <= 5; i++){
							if ((pMacroblock1->macroblockType == MB_MOTION_FORWARD) &&
								(pMacroblock1->motionType == FIELD_STRUCTURE_MOTION_16X8) &&
								((pMacroblock1->hOffset - pMacroblock->hOffset) == 16 * i) &&
								(nvMoCompCheckFieldPredictionEqual(pMacroblock, pMacroblock1, pMacroblock->macroblockType))){
									numCombinedMacroblocks[macroblockIndex]++;
							}else
								break;
							if(n <= i+1)
								break;
							pMacroblock1++;
						}
					}
                }
                nvPredictFieldLumaMacroblock(pMacroblock, numCombinedMacroblocks[macroblockIndex]);
                pMacroblock += numCombinedMacroblocks[macroblockIndex];
                n -= numCombinedMacroblocks[macroblockIndex];
            } else if (pMacroblock->macroblockType == MB_MOTION_BACKWARD) {
                if (n > 1) {
                    pMacroblock1 = pMacroblock + 1;
                    if (pMacroblock->motionType == FIELD_STRUCTURE_MOTION_FIELD) {
						//combine up to 5 adjacent macroblocks together
						for( i = 1; i <= 5; i++){
							if ((pMacroblock1->macroblockType == MB_MOTION_BACKWARD) &&
								(pMacroblock1->motionType == FIELD_STRUCTURE_MOTION_FIELD) &&
								((pMacroblock1->hOffset - pMacroblock->hOffset) == 16 * i) &&
								(pMacroblock->fieldSelect[0][1] == pMacroblock1->fieldSelect[0][1]) &&
								(*(long *)&pMacroblock->PMV[0][1][0] == *(long *)&pMacroblock1->PMV[0][1][0])) {
									numCombinedMacroblocks[macroblockIndex]++;
							}else
								break;
	
							if(n <= i+1)
								break;
							pMacroblock1++;
						}
                            
					} else if (pMacroblock->motionType == FIELD_STRUCTURE_MOTION_16X8) {
						//combine up to 5 adjacent macroblocks together
						for( i = 1; i <= 5; i++){
							if ((pMacroblock1->macroblockType == MB_MOTION_BACKWARD) &&
								(pMacroblock1->motionType == FIELD_STRUCTURE_MOTION_16X8) &&
								((pMacroblock1->hOffset - pMacroblock->hOffset) == 16 * i) &&
								(nvMoCompCheckFieldPredictionEqual(pMacroblock, pMacroblock1, pMacroblock->macroblockType))){
									numCombinedMacroblocks[macroblockIndex]++;
							}else
								break;
	
							if(n <= i+1)
								break;
							pMacroblock1++;
						}
                    }
                }
                nvPredictFieldLumaMacroblock(pMacroblock, numCombinedMacroblocks[macroblockIndex]);
                pMacroblock += numCombinedMacroblocks[macroblockIndex];
                n -= numCombinedMacroblocks[macroblockIndex];
            } else if (pMacroblock->macroblockType == (MB_MOTION_FORWARD | MB_MOTION_BACKWARD)) {
                if (n > 1) {
                    pMacroblock1 = pMacroblock + 1;
                    if (pMacroblock->motionType == FIELD_STRUCTURE_MOTION_FIELD) {
						//combine up to 5 adjacent macroblocks together
						for( i = 1; i <= 5; i++){
							if ((pMacroblock1->macroblockType == (MB_MOTION_FORWARD | MB_MOTION_BACKWARD)) &&
								(pMacroblock1->motionType == FIELD_STRUCTURE_MOTION_FIELD) &&
								((pMacroblock1->hOffset - pMacroblock->hOffset) == 16 * i) &&
								(nvMoCompCheckFieldPredictionEqual(pMacroblock, pMacroblock1, pMacroblock->macroblockType))) {
									numCombinedMacroblocks[macroblockIndex]++;
							}else
								break;
	
							if(n <= i+1)
								break;
							pMacroblock1++;
						}
                            
                    } else if (pMacroblock->motionType == FIELD_STRUCTURE_MOTION_16X8) {
						//combine up to 5 adjacent macroblocks together
						for( i = 1; i <= 5; i++){
							if ((pMacroblock1->macroblockType == (MB_MOTION_FORWARD | MB_MOTION_BACKWARD)) &&
								(pMacroblock1->motionType == FIELD_STRUCTURE_MOTION_16X8) &&
								((pMacroblock1->hOffset - pMacroblock->hOffset) == 16 * i) &&
								(nvMoCompCheckFieldPredictionEqual(pMacroblock, pMacroblock1, pMacroblock->macroblockType))){
									numCombinedMacroblocks[macroblockIndex]++;
							}else
								break;
	
							if(n <= i+1)
								break;
							pMacroblock1++;
						}
                    }
                }
                nvPredictFieldLumaMacroblock(pMacroblock, numCombinedMacroblocks[macroblockIndex]);
                pMacroblock += numCombinedMacroblocks[macroblockIndex];
                n -= numCombinedMacroblocks[macroblockIndex];
            } else {
                if (pMacroblock->macroblockType != MB_INTRA)
                    nvPredictFieldLumaMacroblock(pMacroblock, numCombinedMacroblocks[macroblockIndex]);
                pMacroblock++;
                n--;
            }
            macroblockIndex++;
        }

        nvPusherStart(TRUE);

        pMacroblock = pMacroblocks;
        n = numMacroblocks;
        macroblockIndex = 0;

#ifdef  DEBUG
        if (moCompDebugLevel < 3) {
#endif  // DEBUG

        // Change surface offsets to chroma portion of surface
        nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                      NV097_NO_OPERATION | 0x40000);
        nvPushData(1, 0);

        nvPushData(2, dDrawSubchannelOffset(NV_DD_KELVIN) +
                      NV097_SET_SURFACE_COLOR_OFFSET | 0x40000);

        if (pDriverData->bMCPictureStructure == PICTURE_STRUCTURE_BOTTOM_FIELD) {
            nvMoCompParams.currentDestinationField = 1;
            nvPushData(3, nvMoCompParams.surfaceOffset + nvMoCompParams.surfaceChromaOffset + nvMoCompParams.surfaceLumaPitch);
        } else {
            nvMoCompParams.currentDestinationField = 0;
            nvPushData(3, nvMoCompParams.surfaceOffset + nvMoCompParams.surfaceChromaOffset);
        }

        nvPushData(4, dDrawSubchannelOffset(NV_DD_KELVIN) +
                      NV097_SET_TEXTURE_OFFSET(0) | 0x40000);
        nvPushData(5, pDriverData->dwMCForwardSurfaceOffset + nvMoCompParams.surfaceChromaOffset);

        nvPushData(6, dDrawSubchannelOffset(NV_DD_KELVIN) +
                      NV097_SET_TEXTURE_OFFSET(1) | 0x40000);
        nvPushData(7, pDriverData->dwMCBackwardSurfaceOffset + nvMoCompParams.surfaceChromaOffset);

        nvPusherAdjust(8);

        nvMoCompParams.currentForwardField = 0;
        nvMoCompParams.currentBackwardField = 0;

        // Perform chroma prediction pass
        while (n > 0) {
            if (pMacroblock->macroblockType != MB_INTRA)
                nvPredictFieldChromaMacroblock(pMacroblock, numCombinedMacroblocks[macroblockIndex]);
            pMacroblock += numCombinedMacroblocks[macroblockIndex];
            n -= numCombinedMacroblocks[macroblockIndex];
            macroblockIndex++;
        }

        nvPusherStart(TRUE);

#ifdef  DEBUG
        }
#endif  // DEBUG
    }

    if (!(nvInitKelvinForMoCompCorrection()))
        return FALSE;

    SET_KELVIN_BIDIRECTIONAL_VERTEX_ARRAY_MODE();

    nvDoAllIntraFieldCorrections = nvDoAllKelvinIntraDVDFieldCorrections;
    nvDoAllFieldCorrections = nvDoAllKelvinDVDFieldCorrections;
    nvDoIntraFieldCorrection = nvDoKelvinIntraDVDFieldCorrection;
    nvDoOptimizedIntraFieldCorrection = nvDoOptimizedKelvinIntraDVDFieldCorrection;
    nvDoFieldCorrection = nvDoKelvinDVDFieldCorrection;

    nvDoAllIntraFrameCorrections = nvDoAllKelvinIntraFrameCorrections;
    nvDoAllFrameCorrections = nvDoAllKelvinFrameCorrections;
    nvDoIntraFrameCorrection = nvDoKelvinIntraFrameCorrection;
    nvDoFrameCorrection = nvDoKelvinFrameCorrection;

    // Default motion comp correction ICW
    moCompCorrectionICW = (NV097_SET_COMBINER_COLOR_ICW_A_MAP_UNSIGNED_IDENTITY << 29) |
                          (NV097_SET_COMBINER_COLOR_ICW_A_ALPHA_FALSE << 28) |
                          (NV097_SET_COMBINER_COLOR_ICW_A_SOURCE_REG_1 << 24) |

                          (NV097_SET_COMBINER_COLOR_ICW_B_MAP_HALFBIAS_NORMAL << 21) |
                          (NV097_SET_COMBINER_COLOR_ICW_B_ALPHA_FALSE << 20) |
                          (NV097_SET_COMBINER_COLOR_ICW_B_SOURCE_REG_8 << 16) |

                          (NV097_SET_COMBINER_COLOR_ICW_C_MAP_UNSIGNED_IDENTITY << 13) |
                          (NV097_SET_COMBINER_COLOR_ICW_C_ALPHA_FALSE << 12) |
                          (NV097_SET_COMBINER_COLOR_ICW_C_SOURCE_REG_1 << 8) |

                          (NV097_SET_COMBINER_COLOR_ICW_D_MAP_UNSIGNED_IDENTITY << 5) |
                          (NV097_SET_COMBINER_COLOR_ICW_D_ALPHA_FALSE << 4) |
                           NV097_SET_COMBINER_COLOR_ICW_D_SOURCE_REG_9;

    // Non-prebiased motion comp correction ICW
    moCompNonPrebiasedCorrectionICW = (NV097_SET_COMBINER_COLOR_ICW_A_MAP_UNSIGNED_IDENTITY << 29) |
                                      (NV097_SET_COMBINER_COLOR_ICW_A_ALPHA_FALSE << 28) |
                                      (NV097_SET_COMBINER_COLOR_ICW_A_SOURCE_REG_1 << 24) |

                                      (NV097_SET_COMBINER_COLOR_ICW_B_MAP_UNSIGNED_IDENTITY << 21) |
                                      (NV097_SET_COMBINER_COLOR_ICW_B_ALPHA_FALSE << 20) |
                                      (NV097_SET_COMBINER_COLOR_ICW_B_SOURCE_REG_8 << 16) |

                                      (NV097_SET_COMBINER_COLOR_ICW_C_MAP_UNSIGNED_IDENTITY << 13) |
                                      (NV097_SET_COMBINER_COLOR_ICW_C_ALPHA_FALSE << 12) |
                                      (NV097_SET_COMBINER_COLOR_ICW_C_SOURCE_REG_0 << 8) |

                                      (NV097_SET_COMBINER_COLOR_ICW_D_MAP_UNSIGNED_IDENTITY << 5) |
                                      (NV097_SET_COMBINER_COLOR_ICW_D_ALPHA_FALSE << 4) |
                                       NV097_SET_COMBINER_COLOR_ICW_D_SOURCE_REG_0;

    nvMoCompParams.currentDestinationField = 0;

    pMacroblock = pMacroblocks;
    n = numMacroblocks;

    // This assumes that the first IDCT index of the first macroblock in the command stream is the
    // lowest index in the entire command stream for this call.

    dwSurfaceOffset = pDriverData->dwMCDestinationSurfaceOffset;

    macroblockIndex = 0;

    index = 0;

    while (n > 0) {

        macroblockLimit = n;

        if (macroblockLimit > 32)
            macroblockLimit = 32;

        pDriverData->dwMCFirstAlignedIDCTIndex = pMacroblock->firstIDCTIndex & ~1;

        dwFirstAlignedIDCTOffset = pDriverData->dwMCFirstAlignedIDCTIndex * 64; // 64 bytes per IDCT block

        // Force both texture caches to flush
        nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                      NV097_SET_TEXTURE_OFFSET(0) | 0x40000);
        nvPushData(1, dwFirstAlignedIDCTOffset);
        nvPushData(2, dDrawSubchannelOffset(NV_DD_KELVIN) +
                      NV097_SET_TEXTURE_OFFSET(1) | 0x40000);
        nvPushData(3, dwSurfaceOffset);

        nvPusherAdjust(4);

        if (pDriverData->bMCCurrentExecuteFunction ==
            EXECUTE_NON_PREBIASED_MOTION_COMPENSATION) {
            // Modify combiners to perform non-prebiased intrablock correction
            nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                          NV097_SET_COMBINER_COLOR_ICW(0) | 0x40000);
            nvPushData(1, moCompNonPrebiasedCorrectionICW);

            nvPusherAdjust(2);

            SET_KELVIN_UNBIASED_CORRECTION_DATA_FORMAT();
        } else {
            SET_KELVIN_CORRECTION_DATA_FORMAT();
        }    

        // Perform first correction pass
        if (pDriverData->bMCPictureCodingType == PICTURE_TYPE_I) {
            nvCorrectKelvinIntraField(pMacroblock, macroblockLimit);
            pMacroblock += macroblockLimit;
            n -= macroblockLimit;
        } else {
            long saveMacroblockLimit = macroblockLimit;

            pMacroblock1 = pMacroblock;

            // Change texture1 surface offset to point to block of half bias values (0x80)
            nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                          NV097_SET_TEXTURE_OFFSET(1) | 0x40000);
            nvPushData(1, nvMoCompParams.surfaceOffset + nvMoCompParams.surfaceBiasedIntraBlockOffset);

            // Change texture0 pitch to 16 bytes
            nvPushData(2, dDrawSubchannelOffset(NV_DD_KELVIN) +
                           NV097_SET_TEXTURE_CONTROL1(0) | 0x40000);
            nvPushData(3, 16 << 16);
            nvPushData(4, dDrawSubchannelOffset(NV_DD_KELVIN) +
                           NV097_SET_TEXTURE_CONTROL1(1) | 0x40000);
            nvPushData(5, 16 << 16); // half bias Intra block pitch

            nvPusherAdjust(6);

            while (--macroblockLimit >= 0) {
                if ((pMacroblock->overflowCodedBlockPattern != 0) && (pMacroblock->macroblockType != MB_INTRA))
                    numCombinedMacroblocks[macroblockIndex++] = (BYTE) index;
                index++;
                if ((pMacroblock->codedBlockPattern != 0) && (pMacroblock->macroblockType == MB_INTRA))
                     nvCorrectKelvinFieldMacroblock(pMacroblock, CORRECTION_FIRST_PASS);
                pMacroblock++;
                --n;
            }

            nvPusherStart(TRUE);

            // Force both texture caches to flush
            nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                          NV097_NO_OPERATION | 0x40000);
            nvPushData(1, 0);

            nvPushData(2, dDrawSubchannelOffset(NV_DD_KELVIN) +
                          NV097_SET_TEXTURE_OFFSET(0) | 0x40000);
            nvPushData(3, dwFirstAlignedIDCTOffset);
            nvPushData(4, dDrawSubchannelOffset(NV_DD_KELVIN) +
                          NV097_SET_TEXTURE_OFFSET(1) | 0x40000);
            nvPushData(5, dwSurfaceOffset);

            nvPusherAdjust(6);

            // Change texture0 pitch to 16 bytes
            nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                          NV097_SET_TEXTURE_CONTROL1(0) | 0x40000);
            nvPushData(1, 16 << 16);
            nvPushData(2, dDrawSubchannelOffset(NV_DD_KELVIN) +
                          NV097_SET_TEXTURE_CONTROL1(1) | 0x40000);
            nvPushData(3, nvMoCompParams.surfaceLumaPitch << 17);

            nvPusherAdjust(4);

            if (pDriverData->bMCCurrentExecuteFunction ==
                EXECUTE_NON_PREBIASED_MOTION_COMPENSATION) {
                // Modify combiners to perform interblock correction
                nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                              NV097_SET_COMBINER_COLOR_ICW(0) | 0x40000);
                nvPushData(1, moCompCorrectionICW);

                nvPusherAdjust(2);
            }    

            SET_KELVIN_CORRECTION_DATA_FORMAT();

            nvMoCompParams.currentDestinationField = 0;

            pMacroblock = pMacroblock1;
            macroblockLimit = saveMacroblockLimit;

            while (--macroblockLimit >= 0) {
                if ((pMacroblock->codedBlockPattern != 0) && (pMacroblock->macroblockType != MB_INTRA))
                     nvCorrectKelvinFieldMacroblock(pMacroblock, CORRECTION_FIRST_PASS);
                pMacroblock++;
            }
        }

        nvPusherStart(TRUE);
    }

    pLastMacroblock = pMacroblock - 1;

    if ((pDriverData->bMCPictureCodingType != PICTURE_TYPE_I) && (macroblockIndex > 0)) {

        pMacroblock = pMacroblocks;
        n = macroblockIndex;
        macroblockIndex = 0;
        index = -32;

        nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                      NV097_NO_OPERATION | 0x40000);
        nvPushData(1, 0);

        // DO NOT REMOVE: This is necessary to work around an NV10 texture cache bug
        nvPushData(2, dDrawSubchannelOffset(NV_DD_KELVIN) +
                      NV097_SET_SURFACE_COLOR_OFFSET | 0x40000);
        nvPushData(3, dwSurfaceOffset);

        nvPusherAdjust(4);

        if (pDriverData->bMCCurrentExecuteFunction ==
            EXECUTE_NON_PREBIASED_MOTION_COMPENSATION) {
            // Modify combiners to perform interblock correction
            nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                          NV097_SET_COMBINER_COLOR_ICW(0) | 0x40000);
            nvPushData(1, moCompCorrectionICW);

            nvPusherAdjust(2);
        }    

        SET_KELVIN_CORRECTION_DATA_FORMAT();

        while (--n >= 0) {

            firstAlignedIDCTIndex = (&pMacroblock[numCombinedMacroblocks[macroblockIndex]])->firstIDCTIndex & ~1;

            if (firstAlignedIDCTIndex - index >= 32) {
                pDriverData->dwMCFirstAlignedIDCTIndex = firstAlignedIDCTIndex;

                index = firstAlignedIDCTIndex;

                // Attempt to sanity check index
                if ((pDriverData->dwMCFirstAlignedIDCTIndex << 6) > pDriverData->dwMCIDCTAGPCtxDmaSize[0])
                    pDriverData->dwMCFirstAlignedIDCTIndex = 0;

                dwFirstAlignedIDCTOffset = pDriverData->dwMCFirstAlignedIDCTIndex * 64; // 64 bytes per IDCT block

                // Force both texture caches to flush
                nvPushData(0, dDrawSubchannelOffset(NV_DD_KELVIN) +
                              NV097_SET_TEXTURE_OFFSET(0) | 0x40000);
                nvPushData(1, dwFirstAlignedIDCTOffset);
                nvPushData(2, dDrawSubchannelOffset(NV_DD_KELVIN) +
                              NV097_SET_TEXTURE_OFFSET(1) | 0x40000);
                nvPushData(3, dwSurfaceOffset);

                nvPusherAdjust(4);

                nvMoCompParams.currentDestinationField = 0;
            }

            // Perform overflow correction pass
            nvCorrectKelvinFieldMacroblock((&pMacroblock[numCombinedMacroblocks[macroblockIndex]]), CORRECTION_OVERFLOW_PASS);

            macroblockIndex++;
        }

        nvPusherStart(TRUE);
    }

    if (pDriverData->bMCPictureStructure == PICTURE_STRUCTURE_TOP_FIELD) {
        if ((pLastMacroblock->vOffset >= maxHeight) && (pLastMacroblock->hOffset >= maxWidth))
            surfaceDecodeStatus = 3;
        else if ((unsigned long)pLastMacroblock->vOffset > (nvMoCompParams.surfaceLumaHeight >> 2))
            surfaceDecodeStatus = 1;
    } else {
        if ((pLastMacroblock->vOffset >= maxHeight) && (pLastMacroblock->hOffset >= maxWidth))
            surfaceDecodeStatus = 12;
        else if ((unsigned long)pLastMacroblock->vOffset > (nvMoCompParams.surfaceLumaHeight >> 2))
            surfaceDecodeStatus = 4;
    }

    if (surfaceDecodeStatus > formatConversionStatus) {
        LPNVMCSURFACEFLAGS lpSurfaceFlags;

        nvMoCompConvertSurfaceFormat(dst->fpVidMem, TRUE, (DWORD)surfaceDecodeStatus);

        lpSurfaceFlags = (LPNVMCSURFACEFLAGS)&pDriverData->nvMCSurfaceFlags[0];

        while ((lpSurfaceFlags->dwMCSurfaceBase != 0) &&
               (lpSurfaceFlags->dwMCSurfaceBase != dst->fpVidMem))
            lpSurfaceFlags++;

            if ((lpSurfaceFlags->bMCFrameAlreadyFormatConverted == 15) &&
                (lpSurfaceFlags->bMCFrameIsFiltered == FALSE) &&
                (pDriverData->bMCTemporalFilterDisabled == FALSE))
                nvMoCompTemporalFilter(dst->fpVidMem, pDriverData->bMCPercentCurrentField);
    }

    return TRUE;

} /* nvMoCompProcessKelvinFieldMacroblocks */


#endif  // NVARCH >= 0x10
