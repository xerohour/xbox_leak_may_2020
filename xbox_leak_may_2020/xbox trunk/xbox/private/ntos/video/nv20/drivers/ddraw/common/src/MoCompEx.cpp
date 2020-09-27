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
 *  File:       MoCompEx.cpp
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

extern DWORD __stdcall nvInitCelsiusForMoComp(void);
extern DWORD __stdcall nvInitCelsiusForMoCompPrediction(void);
extern DWORD __stdcall nvInitCelsiusForMoCompCorrection();
extern void __stdcall nvDoCelsiusBidirectionalDVDFieldPrediction(BOOL);
extern void __stdcall nvDoTwoCelsiusBidirectionalDVDFieldPredictions(BOOL);
extern void __stdcall nvDoCelsiusForwardDVDFieldPrediction(BOOL);
extern void __stdcall nvDoTwoCelsiusForwardDVDFieldPredictions(BOOL);
extern void __stdcall nvDoCelsiusBackwardDVDFieldPrediction(BOOL);
extern void __stdcall nvDoTwoCelsiusBackwardDVDFieldPredictions(BOOL);
extern void __stdcall nvDoCelsiusBidirectionalHDFieldPrediction(BOOL);
extern void __stdcall nvDoCelsiusForwardHDFieldPrediction(BOOL);
extern void __stdcall nvDoCelsiusBackwardHDFieldPrediction(BOOL);
extern void __stdcall nvDoAllCelsiusIntraDVDFieldCorrections(long correctionYIncr, long numBlocks);
extern void __stdcall nvDoAllCelsiusDVDFieldCorrections(long correctionYIncr, long numBlocks);
extern void __stdcall nvDoCelsiusIntraDVDFieldCorrection(void);
extern void __stdcall nvDoOptimizedCelsiusIntraDVDFieldCorrection(void);
extern void __stdcall nvDoCelsiusDVDFieldCorrection(void);
extern void __stdcall nvDoAllCelsiusIntraHDFieldCorrections(long correctionYIncr, long numBlocks);
extern void __stdcall nvDoAllCelsiusHDFieldCorrections(long correctionYIncr, long numBlocks);
extern void __stdcall nvDoCelsiusIntraHDFieldCorrection(void);
extern void __stdcall nvDoOptimizedCelsiusIntraHDFieldCorrection(void);
extern void __stdcall nvDoCelsiusHDFieldCorrection(void);
extern void __stdcall nvDoAllCelsiusIntraFrameCorrections(void);
extern void __stdcall nvDoAllCelsiusFrameCorrections(void);
extern void __stdcall nvDoCelsiusIntraFrameCorrection(void);
extern void __stdcall nvDoCelsiusFrameCorrection(void);
extern DWORD __stdcall nvPredictCelsiusFrameLumaMacroblock(LPNVDECODEMACROBLOCK pMacroblock, WORD numCombinedMacroblocks);
extern DWORD __stdcall nvPredictCelsiusFieldLumaMacroblock(LPNVDECODEMACROBLOCK pMacroblock, WORD numCombinedMacroblocks);
extern DWORD __stdcall nvPredictCelsiusFrameChromaMacroblock(LPNVDECODEMACROBLOCK pMacroblock, WORD numCombinedMacroblocks);
extern DWORD __stdcall nvPredictCelsiusFieldChromaMacroblock(LPNVDECODEMACROBLOCK pMacroblock, WORD numCombinedMacroblocks);
extern void __stdcall nvCorrectCelsiusIntraFrame(LPNVDECODEMACROBLOCK pMacroblock, long macroblockCount);
extern void __stdcall nvCorrectCelsiusFrameLumaMacroblock(LPNVDECODEMACROBLOCK pMacroblock, DWORD correctionType);
extern void __stdcall nvCorrectCelsiusFullFrameLumaMacroblock(LPNVDECODEMACROBLOCK pMacroblock, DWORD correctionType);
extern void __stdcall nvCorrectCelsiusFrameChromaMacroblock(LPNVDECODEMACROBLOCK pMacroblock, DWORD correctionType);
extern void __stdcall nvCorrectCelsiusFullFrameChromaMacroblock(LPNVDECODEMACROBLOCK pMacroblock, DWORD correctionType);
extern void __stdcall nvCorrectCelsiusIntraField(LPNVDECODEMACROBLOCK pMacroblock, long macroblockCount);
extern void __stdcall nvCorrectCelsiusFieldMacroblock(LPNVDECODEMACROBLOCK pMacroblock, DWORD correctionType);

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
 * nvMoCompCheckFieldPredictionEqual
 *
 * Tests whether the field predictions are equal on 2 macroblocks.
 *
 *
 * Note that pDriverData must be set before calling this function
 *
 */
BOOL __fastcall nvMoCompCheckFieldPredictionEqual(LPNVDECODEMACROBLOCK pMacroblock0,
                                                  LPNVDECODEMACROBLOCK pMacroblock1, BYTE macroblockType)
{
    if (macroblockType & MB_MOTION_FORWARD) {
        if ((pMacroblock0->fieldSelect[0][0] != pMacroblock1->fieldSelect[0][0]) ||
            (pMacroblock0->fieldSelect[1][0] != pMacroblock1->fieldSelect[1][0]) ||
            (*(long *)&pMacroblock0->PMV[0][0][0] != *(long *)&pMacroblock1->PMV[0][0][0]) ||
            (*(long *)&pMacroblock0->PMV[1][0][0] != *(long *)&pMacroblock1->PMV[1][0][0]))
            return FALSE;
    }
    if (macroblockType & MB_MOTION_BACKWARD) {
        if ((pMacroblock0->fieldSelect[0][1] != pMacroblock1->fieldSelect[0][1]) ||
            (pMacroblock0->fieldSelect[1][1] != pMacroblock1->fieldSelect[1][1]) ||
            (*(long *)&pMacroblock0->PMV[0][1][0] != *(long *)&pMacroblock1->PMV[0][1][0]) ||
            (*(long *)&pMacroblock0->PMV[1][1][0] != *(long *)&pMacroblock1->PMV[1][1][0]))
            return FALSE;
    }
    return TRUE;

} /* nvMoCompCheckFieldPredictionEqual */


/*
 * nvMoCompProcessCelsiusFrameMacroblocks
 *
 * Parses the current macroblock command stream and performs the necessary operations on
 * each macroblock.
 *
 *
 * Note that pDriverData must be set before calling this function
 *
 */
DWORD __stdcall nvMoCompProcessCelsiusFrameMacroblocks(LPNVDECODEMACROBLOCK pMacroblocks,
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
    long                        n,i;
    long                        macroblockIndex;
    BYTE                        surfaceDecodeStatus = 0;
    BYTE                        numCombinedMacroblocks[256];
    short                       maxHeight = (short)dst->wHeight - 16;
    short                       maxWidth = (short)dst->wWidth - 16;
    BOOL                        hasFieldPredictions = FALSE;
    int                         index;
    int                         firstAlignedIDCTIndex;

    nvMoCompParams.vertexMode = 0; // Initialize vertexMode to an invalid value
    nvMoCompParams.surfaceLumaPitch = dst->lPitch;
    nvMoCompParams.surfaceLumaHeight = (dst->wHeight + 1) & ~1;
    nvMoCompParams.surfaceOffset = VIDMEM_OFFSET(dst->fpVidMem);
    nvMoCompParams.surfaceChromaOffset = nvMoCompParams.surfaceLumaPitch * nvMoCompParams.surfaceLumaHeight;
    nvMoCompParams.surfaceBiasedIntraBlockOffset =
        (dst->lPitch * dst->wHeight) + (dst->lPitch * (dst->wHeight >> 1) + (8 * dst->lPitch));
    if (pDriverData->bMCHorizontallyDownscale1080i) {
        nvMoCompParams.celsiusDestinationViewportXOffset = SCALED_CELSIUS_DESTINATION_VIEWPORT_XOFFSET;
        if (nvMoCompParams.surfaceLumaPitch > 1280)
            nvMoCompParams.celsiusFieldXOffset = 10240; // (1920 / 0.75) << 2, MUST CHANGE IF IN TILED MEMORY
        else
            nvMoCompParams.celsiusFieldXOffset = 6827; // (1280 / 0.75) << 2; MUST CHANGE IF IN TILED MEMORY
    } else {
        nvMoCompParams.celsiusDestinationViewportXOffset = CELSIUS_DESTINATION_VIEWPORT_XOFFSET;
        nvMoCompParams.celsiusFieldXOffset = (short)nvMoCompParams.surfaceLumaPitch << 2;
    }
    
    if (pDriverData->bMCPictureCodingType != PICTURE_TYPE_I) {

        if (!(nvInitCelsiusForMoCompPrediction()))
            return FALSE;

        if (nvMoCompParams.surfaceLumaPitch >= 1024) {
            nvDoBidirectionalFieldPrediction = nvDoCelsiusBidirectionalHDFieldPrediction;
            nvDoForwardFieldPrediction = nvDoCelsiusForwardHDFieldPrediction;
            nvDoBackwardFieldPrediction = nvDoCelsiusBackwardHDFieldPrediction;
        } else {
            nvDoBidirectionalFieldPrediction = nvDoCelsiusBidirectionalDVDFieldPrediction;
            nvDoTwoBidirectionalFieldPredictions = nvDoTwoCelsiusBidirectionalDVDFieldPredictions;
            nvDoForwardFieldPrediction = nvDoCelsiusForwardDVDFieldPrediction;
            nvDoTwoForwardFieldPredictions = nvDoTwoCelsiusForwardDVDFieldPredictions;
            nvDoBackwardFieldPrediction = nvDoCelsiusBackwardDVDFieldPrediction;
            nvDoTwoBackwardFieldPredictions = nvDoTwoCelsiusBackwardDVDFieldPredictions;
        }

        nvPredictFrameLumaMacroblock = nvPredictCelsiusFrameLumaMacroblock;
        nvPredictFrameChromaMacroblock = nvPredictCelsiusFrameChromaMacroblock;
        nvPredictFieldLumaMacroblock = nvPredictCelsiusFieldLumaMacroblock;
        nvPredictFieldChromaMacroblock = nvPredictCelsiusFieldChromaMacroblock;

        nvMoCompParams.currentForwardField = 0;
        nvMoCompParams.currentBackwardField = 0;
        nvMoCompParams.currentDestinationField = 0;

        pMacroblock = pMacroblocks;
        n = numMacroblocks;
        macroblockIndex = 0;

        surfacePitch = (nvMoCompParams.surfaceLumaPitch << 16) | nvMoCompParams.surfaceLumaPitch;

        // Must temporarily set destination surface pitch back to it's normal frame pitch
        nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                      NV056_NO_OPERATION | 0x40000);
        nvPushData(1, 0);

        nvPushData(2, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                      NV056_SET_SURFACE_PITCH | 0x40000);
        nvPushData(3, surfacePitch);

        // Must also change texture pitches back to normal frame pitch so that quarter pel addressing works correctly
        nvPushData(4, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                       NV056_SET_TEXTURE_CONTROL1(0) | 0x80000);
        nvPushData(5, surfacePitch << 16);
        nvPushData(6, surfacePitch << 16);

        nvPusherAdjust(7);

        // Perform luma prediction pass
        while (n > 0) {
            numCombinedMacroblocks[macroblockIndex] = 1;
			if (pMacroblock->macroblockType == MB_MOTION_FORWARD) {
				if(n > 1){
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
								break;	//no more macroblck left
							pMacroblock1++;	

						}
					}
					else if (pMacroblock->motionType == FRAME_STRUCTURE_MOTION_FIELD){
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
				if(n > 1){
					pMacroblock1 = pMacroblock + 1;
					if (pMacroblock->motionType == FRAME_STRUCTURE_MOTION_FRAME){
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
					}
					else if (pMacroblock->motionType == FRAME_STRUCTURE_MOTION_FIELD){
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
				 if(n > 1){
					 pMacroblock1 = pMacroblock + 1;
					 if (pMacroblock->motionType == FRAME_STRUCTURE_MOTION_FRAME){
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
					}
					else if (pMacroblock->motionType == FRAME_STRUCTURE_MOTION_FIELD){
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

        // Change surface offsets to chroma portion of surface
        nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                      NV056_NO_OPERATION | 0x40000);
        nvPushData(1, 0);

        nvPushData(2, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                      NV056_SET_SURFACE_COLOR_OFFSET | 0x40000);
        nvPushData(3, nvMoCompParams.surfaceOffset + nvMoCompParams.surfaceChromaOffset);

        nvPushData(4, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                      NV056_SET_TEXTURE_OFFSET(0) | 0x40000);
        nvPushData(5, pDriverData->dwMCForwardSurfaceOffset + nvMoCompParams.surfaceChromaOffset);

        nvPushData(6, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                      NV056_SET_TEXTURE_OFFSET(1) | 0x40000);
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

        // Now do non-frame structured predictions if any are necessary
        if (hasFieldPredictions) {

            // Restore offsets and pitches to field based pitches
            nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                          NV056_NO_OPERATION | 0x40000);
            nvPushData(1, 0);

            nvPushData(2, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                          NV056_SET_SURFACE_COLOR_OFFSET | 0x40000);
            nvPushData(3, nvMoCompParams.surfaceOffset);

            nvPushData(4, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                          NV056_SET_TEXTURE_OFFSET(0) | 0x40000);
            nvPushData(5, pDriverData->dwMCForwardSurfaceOffset);

            nvPushData(6, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                          NV056_SET_TEXTURE_OFFSET(1) | 0x40000);
            nvPushData(7, pDriverData->dwMCBackwardSurfaceOffset);

            nvPusherAdjust(8);

            nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                          NV056_SET_SURFACE_PITCH | 0x40000);
            nvPushData(1, (surfacePitch << 1));

            nvPushData(2, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                           NV056_SET_TEXTURE_CONTROL1(0) | 0x80000);
            nvPushData(3, surfacePitch << 17);
            nvPushData(4, surfacePitch << 17);

            nvPusherAdjust(5);

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
            nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                          NV056_NO_OPERATION | 0x40000);
            nvPushData(1, 0);

            nvPushData(2, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                          NV056_SET_SURFACE_COLOR_OFFSET | 0x40000);
            nvPushData(3, nvMoCompParams.surfaceOffset + nvMoCompParams.surfaceChromaOffset);

            nvPushData(4, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                          NV056_SET_TEXTURE_OFFSET(0) | 0x40000);
            nvPushData(5, pDriverData->dwMCForwardSurfaceOffset + nvMoCompParams.surfaceChromaOffset);

            nvPushData(6, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                          NV056_SET_TEXTURE_OFFSET(1) | 0x40000);
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

            if (nvMoCompParams.surfaceLumaPitch >= 1024) {

                nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                              NV056_NO_OPERATION | 0x40000);
                nvPushData(1, 0);

                nvPushData(2, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                              NV056_SET_SURFACE_COLOR_OFFSET | 0x40000);
                nvPushData(3, nvMoCompParams.surfaceOffset);

                nvPushData(4, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                              NV056_SET_TEXTURE_OFFSET(0) | 0x40000);
                nvPushData(5, pDriverData->dwMCForwardSurfaceOffset);

                nvPushData(6, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                              NV056_SET_TEXTURE_OFFSET(1) | 0x40000);
                nvPushData(7, pDriverData->dwMCBackwardSurfaceOffset);

                nvPusherAdjust(8);

                pMacroblock = pMacroblocks;
                n = numMacroblocks;
                macroblockIndex = 0;

                nvMoCompParams.currentForwardField = 0;
                nvMoCompParams.currentBackwardField = 0;
                nvMoCompParams.currentDestinationField = 0;
                nvMoCompParams.destinationFieldIs0 = FALSE; // Then process all destination field 1 updates

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
                nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                              NV056_NO_OPERATION | 0x40000);
                nvPushData(1, 0);

                nvPushData(2, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                              NV056_SET_SURFACE_COLOR_OFFSET | 0x40000);
                nvPushData(3, nvMoCompParams.surfaceOffset + nvMoCompParams.surfaceChromaOffset);

                nvPushData(4, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                              NV056_SET_TEXTURE_OFFSET(0) | 0x40000);
                nvPushData(5, pDriverData->dwMCForwardSurfaceOffset + nvMoCompParams.surfaceChromaOffset);

                nvPushData(6, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                              NV056_SET_TEXTURE_OFFSET(1) | 0x40000);
                nvPushData(7, pDriverData->dwMCBackwardSurfaceOffset + nvMoCompParams.surfaceChromaOffset);

                nvPusherAdjust(8);

                pMacroblock = pMacroblocks;
                n = numMacroblocks;
                macroblockIndex = 0;

                nvMoCompParams.currentForwardField = 0;
                nvMoCompParams.currentBackwardField = 0;
                nvMoCompParams.currentDestinationField = 0;

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
    }

    if (!(nvInitCelsiusForMoCompCorrection()))
        return FALSE;

    SET_BIDIRECTIONAL_VERTEX_ARRAY_MODE();

    if (nvMoCompParams.surfaceLumaPitch >= 1024) {
        nvDoAllIntraFieldCorrections = nvDoAllCelsiusIntraHDFieldCorrections;
        nvDoAllFieldCorrections = nvDoAllCelsiusHDFieldCorrections;
        nvDoIntraFieldCorrection = nvDoCelsiusIntraHDFieldCorrection;
        nvDoOptimizedIntraFieldCorrection = nvDoOptimizedCelsiusIntraHDFieldCorrection;
        nvDoFieldCorrection = nvDoCelsiusHDFieldCorrection;
    } else {
        nvDoAllIntraFieldCorrections = nvDoAllCelsiusIntraDVDFieldCorrections;
        nvDoAllFieldCorrections = nvDoAllCelsiusDVDFieldCorrections;
        nvDoIntraFieldCorrection = nvDoCelsiusIntraDVDFieldCorrection;
        nvDoOptimizedIntraFieldCorrection = nvDoOptimizedCelsiusIntraDVDFieldCorrection;
        nvDoFieldCorrection = nvDoCelsiusDVDFieldCorrection;
    }

    nvDoAllIntraFrameCorrections = nvDoAllCelsiusIntraFrameCorrections;
    nvDoAllFrameCorrections = nvDoAllCelsiusFrameCorrections;
    nvDoIntraFrameCorrection = nvDoCelsiusIntraFrameCorrection;
    nvDoFrameCorrection = nvDoCelsiusFrameCorrection;

    // Default motion comp correction ICW
    moCompCorrectionICW = (NV056_SET_COMBINER_COLOR_ICW_A_MAP_UNSIGNED_IDENTITY << 29) |
                          (NV056_SET_COMBINER_COLOR_ICW_A_ALPHA_FALSE << 28) |
                          (NV056_SET_COMBINER_COLOR_ICW_A_SOURCE_REG_1 << 24) |

                          (NV056_SET_COMBINER_COLOR_ICW_B_MAP_HALFBIAS_NORMAL << 21) |
                          (NV056_SET_COMBINER_COLOR_ICW_B_ALPHA_FALSE << 20) |
                          (NV056_SET_COMBINER_COLOR_ICW_B_SOURCE_REG_8 << 16) |

                          (NV056_SET_COMBINER_COLOR_ICW_C_MAP_UNSIGNED_IDENTITY << 13) |
                          (NV056_SET_COMBINER_COLOR_ICW_C_ALPHA_FALSE << 12) |
                          (NV056_SET_COMBINER_COLOR_ICW_C_SOURCE_REG_1 << 8) |

                          (NV056_SET_COMBINER_COLOR_ICW_D_MAP_UNSIGNED_IDENTITY << 5) |
                          (NV056_SET_COMBINER_COLOR_ICW_D_ALPHA_FALSE << 4) |
                           NV056_SET_COMBINER_COLOR_ICW_D_SOURCE_REG_9;

    // Non-prebiased motion comp correction ICW
    moCompNonPrebiasedCorrectionICW = (NV056_SET_COMBINER_COLOR_ICW_A_MAP_UNSIGNED_IDENTITY << 29) |
                                      (NV056_SET_COMBINER_COLOR_ICW_A_ALPHA_FALSE << 28) |
                                      (NV056_SET_COMBINER_COLOR_ICW_A_SOURCE_REG_1 << 24) |

                                      (NV056_SET_COMBINER_COLOR_ICW_B_MAP_UNSIGNED_IDENTITY << 21) |
                                      (NV056_SET_COMBINER_COLOR_ICW_B_ALPHA_FALSE << 20) |
                                      (NV056_SET_COMBINER_COLOR_ICW_B_SOURCE_REG_8 << 16) |

                                      (NV056_SET_COMBINER_COLOR_ICW_C_MAP_UNSIGNED_IDENTITY << 13) |
                                      (NV056_SET_COMBINER_COLOR_ICW_C_ALPHA_FALSE << 12) |
                                      (NV056_SET_COMBINER_COLOR_ICW_C_SOURCE_REG_0 << 8) |

                                      (NV056_SET_COMBINER_COLOR_ICW_D_MAP_UNSIGNED_IDENTITY << 5) |
                                      (NV056_SET_COMBINER_COLOR_ICW_D_ALPHA_FALSE << 4) |
                                       NV056_SET_COMBINER_COLOR_ICW_D_SOURCE_REG_0;
 
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

        if (macroblockLimit > 24)
            macroblockLimit = 24;

        pDriverData->dwMCFirstAlignedIDCTIndex = pMacroblock->firstIDCTIndex & ~1;

        // Attempt to sanity check index
        if ((pDriverData->dwMCFirstAlignedIDCTIndex << 6) > pDriverData->dwMCIDCTAGPCtxDmaSize[0])
            pDriverData->dwMCFirstAlignedIDCTIndex = 0;

        dwFirstAlignedIDCTOffset = pDriverData->dwMCFirstAlignedIDCTIndex * 64; // 64 bytes per IDCT block

        // Force both texture caches to flush
        nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                      NV056_NO_OPERATION | 0x40000);
        nvPushData(1, 0);

        nvPushData(2, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                      NV056_SET_SURFACE_COLOR_OFFSET | 0x40000);
        nvPushData(3, dwSurfaceOffset);

        nvPushData(4, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                      NV056_SET_TEXTURE_OFFSET(0) | 0x80000);
        nvPushData(5, dwFirstAlignedIDCTOffset);
        nvPushData(6, dwSurfaceOffset);

        nvPusherAdjust(7);

        nvMoCompParams.destinationField = 0;
        nvMoCompParams.currentDestinationField = nvMoCompParams.destinationField;
        nvMoCompParams.destinationFieldIs0 = TRUE;

        if (pDriverData->bMCCurrentExecuteFunction ==
            EXECUTE_NON_PREBIASED_MOTION_COMPENSATION) {
            // Modify combiners to perform non-prebiased intrablock correction
            nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                          NV056_SET_COMBINER_COLOR_ICW(0) | 0x40000);
            nvPushData(1, moCompNonPrebiasedCorrectionICW);

            nvPusherAdjust(2);

            SET_UNBIASED_CORRECTION_DATA_FORMAT();
        } else {
            SET_CORRECTION_DATA_FORMAT();
        }    

        if ((pDriverData->bMCPictureCodingType == PICTURE_TYPE_I) &&
            (pMacroblock->codedBlockPattern == 0xFF)) {
            nvCorrectCelsiusIntraFrame(pMacroblock, macroblockLimit);
            pMacroblock += macroblockLimit;
            n -= macroblockLimit;
        } else {
            long saveMacroblockLimit = macroblockLimit;
            long fieldTypeCount = 0;
            BOOL hasPartialCorrections = FALSE;

            pMacroblock1 = pMacroblock;

            nvMoCompParams.destinationFieldIs0 = TRUE;

             // Change texture1 surface offset to point to block of half bias values (0x80)
            nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                          NV056_SET_TEXTURE_OFFSET(1) | 0x40000);
            nvPushData(1, nvMoCompParams.surfaceOffset + nvMoCompParams.surfaceBiasedIntraBlockOffset);

            // Change texture0 pitch to 16 bytes
            nvPushData(2, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                           NV056_SET_TEXTURE_CONTROL1(0) | 0x80000);
            nvPushData(3, 16 << 16);
            nvPushData(4, 16 << 16); // half bias Intra block pitch

            nvPusherAdjust(5);

            // Perform first correction pass
            while (--macroblockLimit >= 0) {
                if ((pMacroblock->overflowCodedBlockPattern != 0) && (pMacroblock->macroblockType != MB_INTRA))
                    numCombinedMacroblocks[macroblockIndex++] = (BYTE) index;
                index++;
                if ((pMacroblock->DCTType != DCT_FIELD) && (pMacroblock->macroblockType == MB_INTRA)) {
                    if (pMacroblock->codedBlockPattern >= 0xFC) {
                        nvCorrectCelsiusFullFrameLumaMacroblock(pMacroblock, CORRECTION_FIRST_PASS);
                    } else if (pMacroblock->codedBlockPattern != 0) {
                        // Change texture0 pitch to it's default value of 8 bytes
                        nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                                       NV056_SET_TEXTURE_CONTROL1(0) | 0x40000);
                        nvPushData(1, 8 << 16);

                        nvPusherAdjust(2);

                        nvCorrectCelsiusFrameLumaMacroblock(pMacroblock, CORRECTION_FIRST_PASS);

                        // Change texture0 pitch to 16 bytes
                        nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                                       NV056_SET_TEXTURE_CONTROL1(0) | 0x40000);
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
                SET_UNBIASED_CHROMA_CORRECTION_DATA_FORMAT();
            } else {
                SET_CHROMA_CORRECTION_DATA_FORMAT();
            }    

            pMacroblock = pMacroblock1;
            macroblockLimit = saveMacroblockLimit;

            while (--macroblockLimit >= 0) {
                if (pMacroblock->macroblockType == MB_INTRA) {
                    if (pMacroblock->codedBlockPattern == 0xFF) {
                        nvCorrectCelsiusFullFrameChromaMacroblock(pMacroblock, CORRECTION_FIRST_PASS);
                    } else if (pMacroblock->codedBlockPattern != 0) {
                        nvCorrectCelsiusFrameChromaMacroblock(pMacroblock, CORRECTION_FIRST_PASS);
                    }
                }
                pMacroblock++;
            }

            nvPusherStart(TRUE);

            // Force both texture caches to flush
            nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                          NV056_NO_OPERATION | 0x40000);
            nvPushData(1, 0);

            nvPushData(2, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                          NV056_SET_TEXTURE_OFFSET(1) | 0x40000);
            nvPushData(3, dwSurfaceOffset);

            nvPusherAdjust(4);

            // Change texture0 pitch to 16 bytes
            nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                           NV056_SET_TEXTURE_CONTROL1(0) | 0x80000);
            nvPushData(1, 16 << 16);
            nvPushData(2, nvMoCompParams.surfaceLumaPitch << 16);

            nvPusherAdjust(3);

            if (pDriverData->bMCCurrentExecuteFunction ==
                EXECUTE_NON_PREBIASED_MOTION_COMPENSATION) {
                // Modify combiners to perform interblock correction
                nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                              NV056_SET_COMBINER_COLOR_ICW(0) | 0x40000);
                nvPushData(1, moCompCorrectionICW);

                nvPusherAdjust(2);
            }    

            SET_CORRECTION_DATA_FORMAT();

            nvMoCompParams.destinationFieldIs0 = TRUE;

            pMacroblock = pMacroblock1;
            macroblockLimit = saveMacroblockLimit;

            // Perform first correction pass
            while (--macroblockLimit >= 0) {
                if ((pMacroblock->DCTType != DCT_FIELD) && (pMacroblock->macroblockType != MB_INTRA)) {
                    if (pMacroblock->codedBlockPattern >= 0xFC) {
                        nvCorrectCelsiusFullFrameLumaMacroblock(pMacroblock, CORRECTION_FIRST_PASS);
                    } else if (pMacroblock->codedBlockPattern != 0) {
                        // Change texture0 pitch to it's default value of 8 bytes
                        nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                                       NV056_SET_TEXTURE_CONTROL1(0) | 0x40000);
                        nvPushData(1, 8 << 16);

                        nvPusherAdjust(2);

                        nvCorrectCelsiusFrameLumaMacroblock(pMacroblock, CORRECTION_FIRST_PASS);

                        // Change texture0 pitch to 16 bytes
                        nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                                       NV056_SET_TEXTURE_CONTROL1(0) | 0x40000);
                        nvPushData(1, 16 << 16);

                        nvPusherAdjust(2);
                    }
                }
                pMacroblock++;
            }

            nvPusherStart(TRUE);

            SET_CHROMA_CORRECTION_DATA_FORMAT();

            pMacroblock = pMacroblock1;
            macroblockLimit = saveMacroblockLimit;

            while (--macroblockLimit >= 0) {
                if (pMacroblock->macroblockType != MB_INTRA) {
                    if (pMacroblock->codedBlockPattern == 0xFF) {
                        nvCorrectCelsiusFullFrameChromaMacroblock(pMacroblock, CORRECTION_FIRST_PASS);
                    } else if (pMacroblock->codedBlockPattern != 0) {
                        nvCorrectCelsiusFrameChromaMacroblock(pMacroblock, CORRECTION_FIRST_PASS);
                    }
                }
                pMacroblock++;
            }

            nvPusherStart(TRUE);

            if (fieldTypeCount > 0) {

                nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                              NV056_NO_OPERATION | 0x40000);
                nvPushData(1, 0);

                // Change destination pitch to field pitch
                nvPushData(2, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                              NV056_SET_SURFACE_PITCH | 0x40000);
                nvPushData(3, ((nvMoCompParams.surfaceLumaPitch << 17) | (nvMoCompParams.surfaceLumaPitch << 1)));

                // Change texture1 surface offset to point to block of half bias values (0x80)
                nvPushData(4, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                              NV056_SET_TEXTURE_OFFSET(1) | 0x40000);
                nvPushData(5, nvMoCompParams.surfaceOffset + nvMoCompParams.surfaceBiasedIntraBlockOffset);

                // Change texture0 pitch to 16 bytes
                nvPushData(6, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                               NV056_SET_TEXTURE_CONTROL1(0) | 0x80000);
                nvPushData(7, 16 << 16);
                nvPushData(8, 16 << 16); // half bias Intra block pitch

                nvPusherAdjust(9);

                if (pDriverData->bMCCurrentExecuteFunction ==
                    EXECUTE_NON_PREBIASED_MOTION_COMPENSATION) {
                    // Modify combiners to perform non-prebiased intrablock correction
                    nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                                  NV056_SET_COMBINER_COLOR_ICW(0) | 0x40000);
                    nvPushData(1, moCompNonPrebiasedCorrectionICW);

                    nvPusherAdjust(2);

                    SET_UNBIASED_CORRECTION_DATA_FORMAT();
                } else {
                    SET_CORRECTION_DATA_FORMAT();
                }    

                pMacroblock = pMacroblock1;
                macroblockLimit = saveMacroblockLimit;

                while (--macroblockLimit >= 0) {
                    if ((pMacroblock->DCTType == DCT_FIELD) && (pMacroblock->macroblockType == MB_INTRA)) {
                        if (pMacroblock->codedBlockPattern >= 0xFC) {
                            nvCorrectCelsiusFullFrameLumaMacroblock(pMacroblock, CORRECTION_FIRST_PASS);
                        } else if (pMacroblock->codedBlockPattern != 0) {
                            // Change texture0 pitch to it's default value of 8 bytes
                            nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                                           NV056_SET_TEXTURE_CONTROL1(0) | 0x40000);
                            nvPushData(1, 8 << 16);

                            nvPusherAdjust(2);

                            hasPartialCorrections = TRUE;

                            nvCorrectCelsiusFrameLumaMacroblock(pMacroblock, CORRECTION_FIRST_PASS);

                            // Change texture0 pitch to 16 bytes
                            nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                                           NV056_SET_TEXTURE_CONTROL1(0) | 0x40000);
                            nvPushData(1, 16 << 16);

                            nvPusherAdjust(2);
                        }
                    }
                    pMacroblock++;
                }

                if ((nvMoCompParams.surfaceLumaPitch >= 1024) && (hasPartialCorrections)) {

                    nvMoCompParams.destinationFieldIs0 = FALSE;

                    pMacroblock = pMacroblock1;
                    macroblockLimit = saveMacroblockLimit;

                    while (--macroblockLimit >= 0) {
                        if ((pMacroblock->DCTType == DCT_FIELD) && (pMacroblock->macroblockType == MB_INTRA)) {
                             if ((pMacroblock->codedBlockPattern < 0xFC) &&
                                (pMacroblock->codedBlockPattern != 0)) {
                                // Change texture0 pitch to it's default value of 8 bytes
                                nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                                               NV056_SET_TEXTURE_CONTROL1(0) | 0x40000);
                                nvPushData(1, 8 << 16);

                                nvPusherAdjust(2);

                                nvCorrectCelsiusFrameLumaMacroblock(pMacroblock, CORRECTION_FIRST_PASS);

                                // Change texture0 pitch to 16 bytes
                                nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                                               NV056_SET_TEXTURE_CONTROL1(0) | 0x40000);
                                nvPushData(1, 16 << 16);

                                nvPusherAdjust(2);
                            }
                        }
                        pMacroblock++;
                    }

                    // Restore frame offsets
                    nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                                  NV056_NO_OPERATION | 0x40000);
                    nvPushData(1, 0);

                    nvPushData(2, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                                  NV056_SET_SURFACE_COLOR_OFFSET | 0x40000);
                    nvPushData(3, nvMoCompParams.surfaceOffset);

                    nvPushData(4, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                                  NV056_SET_TEXTURE_OFFSET(1) | 0x40000);
                    nvPushData(5, nvMoCompParams.surfaceOffset);

                    nvPusherAdjust(6);

                    nvMoCompParams.destinationField = 0;
                    nvMoCompParams.currentDestinationField = nvMoCompParams.destinationField;
                }

                // Force both texture caches to flush
                nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                              NV056_NO_OPERATION | 0x40000);
                nvPushData(1, 0);

                nvPushData(2, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                              NV056_SET_TEXTURE_OFFSET(0) | 0x80000);
                nvPushData(3, dwFirstAlignedIDCTOffset);
                nvPushData(4, dwSurfaceOffset);

                nvPusherAdjust(5);

                // Change texture0 pitch to 16 bytes
                nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                               NV056_SET_TEXTURE_CONTROL1(0) | 0x80000);
                nvPushData(1, 16 << 16);
                nvPushData(2, nvMoCompParams.surfaceLumaPitch << 16);

                nvPusherAdjust(3);

                if (pDriverData->bMCCurrentExecuteFunction ==
                    EXECUTE_NON_PREBIASED_MOTION_COMPENSATION) {
                    // Modify combiners to perform interblock correction
                    nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                                  NV056_SET_COMBINER_COLOR_ICW(0) | 0x40000);
                    nvPushData(1, moCompCorrectionICW);

                    nvPusherAdjust(2);
                }    

                SET_CORRECTION_DATA_FORMAT();

                nvMoCompParams.destinationFieldIs0 = TRUE;

                pMacroblock = pMacroblock1;
                macroblockLimit = saveMacroblockLimit;

                while (--macroblockLimit >= 0) {
                    if ((pMacroblock->DCTType == DCT_FIELD) && (pMacroblock->macroblockType != MB_INTRA)) {
                        if (pMacroblock->codedBlockPattern >= 0xFC) {
                            nvCorrectCelsiusFullFrameLumaMacroblock(pMacroblock, CORRECTION_FIRST_PASS);
                        } else if (pMacroblock->codedBlockPattern != 0) {
                            // Change texture0 pitch to it's default value of 8 bytes
                            nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                                           NV056_SET_TEXTURE_CONTROL1(0) | 0x40000);
                            nvPushData(1, 8 << 16);

                            nvPusherAdjust(2);

                            hasPartialCorrections = TRUE;

                            nvCorrectCelsiusFrameLumaMacroblock(pMacroblock, CORRECTION_FIRST_PASS);

                            // Change texture0 pitch to 16 bytes
                            nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                                           NV056_SET_TEXTURE_CONTROL1(0) | 0x40000);
                            nvPushData(1, 16 << 16);

                            nvPusherAdjust(2);
                        }
                    }
                    pMacroblock++;
                }

                if ((nvMoCompParams.surfaceLumaPitch >= 1024) && (hasPartialCorrections)) {

                    nvMoCompParams.destinationFieldIs0 = FALSE;

                    pMacroblock = pMacroblock1;
                    macroblockLimit = saveMacroblockLimit;

                    while (--macroblockLimit >= 0) {
                        if ((pMacroblock->DCTType == DCT_FIELD) && (pMacroblock->macroblockType != MB_INTRA)) {
                            if ((pMacroblock->codedBlockPattern < 0xFC) &&
                                (pMacroblock->codedBlockPattern != 0)) {
                                // Change texture0 pitch to it's default value of 8 bytes
                                nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                                               NV056_SET_TEXTURE_CONTROL1(0) | 0x40000);
                                nvPushData(1, 8 << 16);

                                nvPusherAdjust(2);

                                nvCorrectCelsiusFrameLumaMacroblock(pMacroblock, CORRECTION_FIRST_PASS);

                                // Change texture0 pitch to 16 bytes
                                nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                                               NV056_SET_TEXTURE_CONTROL1(0) | 0x40000);
                                nvPushData(1, 16 << 16);

                                nvPusherAdjust(2);
                            }
                        }
                        pMacroblock++;
                    }

                    // Restore frame offsets
                    nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                                  NV056_NO_OPERATION | 0x40000);
                    nvPushData(1, 0);

                    nvPushData(2, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                                  NV056_SET_SURFACE_COLOR_OFFSET | 0x40000);
                    nvPushData(3, nvMoCompParams.surfaceOffset);

                    nvPushData(4, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                                  NV056_SET_TEXTURE_OFFSET(1) | 0x40000);
                    nvPushData(5, nvMoCompParams.surfaceOffset);

                    nvPusherAdjust(6);

                    nvMoCompParams.destinationField = 0;
                    nvMoCompParams.currentDestinationField = nvMoCompParams.destinationField;
                }

                // Restore surface pitch
                nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                              NV056_SET_SURFACE_PITCH | 0x40000);
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
        index = -24;

        nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                      NV056_NO_OPERATION | 0x40000);
        nvPushData(1, 0);

        // DO NOT REMOVE: This is necessary to work around an NV10 texture cache bug
        nvPushData(2, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                      NV056_SET_SURFACE_COLOR_OFFSET | 0x40000);
        nvPushData(3, dwSurfaceOffset);

        nvPusherAdjust(4);

        nvMoCompParams.currentDestinationField = 0;
        nvMoCompParams.destinationField = 0;

        // Change destination pitch back to frame pitch
        nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                      NV056_SET_SURFACE_PITCH | 0x40000);
        nvPushData(1, ((nvMoCompParams.surfaceLumaPitch << 16) | nvMoCompParams.surfaceLumaPitch));

        nvPushData(2, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                       NV056_SET_TEXTURE_OFFSET(1) | 0x40000);
        nvPushData(3, nvMoCompParams.surfaceOffset);

        // Change texture0 pitch to 16 bytes
        nvPushData(4, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                       NV056_SET_TEXTURE_CONTROL1(0) | 0x80000);
        nvPushData(5, 16 << 16);
        nvPushData(6, nvMoCompParams.surfaceLumaPitch << 16);

        nvPusherAdjust(7);

        if (pDriverData->bMCCurrentExecuteFunction ==
            EXECUTE_NON_PREBIASED_MOTION_COMPENSATION) {
            // Modify combiners to perform interblock correction
            nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                          NV056_SET_COMBINER_COLOR_ICW(0) | 0x40000);
            nvPushData(1, moCompCorrectionICW);

            nvPusherAdjust(2);
        }    

        SET_CHROMA_CORRECTION_DATA_FORMAT();

        while (--n >= 0) {

            pMacroblock = &pMacroblock1[numCombinedMacroblocks[macroblockIndex]];

            firstAlignedIDCTIndex = pMacroblock->firstIDCTIndex & ~1;

            if (firstAlignedIDCTIndex - index >= 24) {
                pDriverData->dwMCFirstAlignedIDCTIndex = firstAlignedIDCTIndex;

                index = firstAlignedIDCTIndex;

                // Attempt to sanity check index
                if ((pDriverData->dwMCFirstAlignedIDCTIndex << 6) > pDriverData->dwMCIDCTAGPCtxDmaSize[0])
                    pDriverData->dwMCFirstAlignedIDCTIndex = 0;

                dwFirstAlignedIDCTOffset = pDriverData->dwMCFirstAlignedIDCTIndex * 64; // 64 bytes per IDCT block

                // Force both texture caches to flush
                nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                              NV056_SET_TEXTURE_OFFSET(0) | 0x80000);
                nvPushData(1, dwFirstAlignedIDCTOffset);
                nvPushData(2, dwSurfaceOffset);

                nvPusherAdjust(3);

                nvMoCompParams.currentDestinationField = 0;
                nvMoCompParams.destinationField = 0;
            }

            // Perform chroma overflow correction pass
            nvCorrectCelsiusFrameChromaMacroblock(pMacroblock, CORRECTION_OVERFLOW_PASS);

            macroblockIndex++;
        }

        nvPusherStart(TRUE);

        n = saveN;
        macroblockIndex = 0;
        index = -24;

        // Change texture0 pitch to it's default value of 8 bytes
        nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                       NV056_SET_TEXTURE_CONTROL1(0) | 0x80000);
        nvPushData(1, 8 << 16);
        nvPushData(2, nvMoCompParams.surfaceLumaPitch << 16);

        nvPusherAdjust(3);

        SET_CORRECTION_DATA_FORMAT();

        while (--n >= 0) {

            pMacroblock = &pMacroblock1[numCombinedMacroblocks[macroblockIndex]];

            firstAlignedIDCTIndex = pMacroblock->firstIDCTIndex & ~1;

            if (firstAlignedIDCTIndex - index >= 24) {
                pDriverData->dwMCFirstAlignedIDCTIndex = firstAlignedIDCTIndex;

                index = firstAlignedIDCTIndex;

                // Attempt to sanity check index
                if ((pDriverData->dwMCFirstAlignedIDCTIndex << 6) > pDriverData->dwMCIDCTAGPCtxDmaSize[0])
                    pDriverData->dwMCFirstAlignedIDCTIndex = 0;

                dwFirstAlignedIDCTOffset = pDriverData->dwMCFirstAlignedIDCTIndex * 64; // 64 bytes per IDCT block

                // Force both texture caches to flush
                nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                              NV056_SET_TEXTURE_OFFSET(0) | 0x80000);
                nvPushData(1, dwFirstAlignedIDCTOffset);
                nvPushData(2, dwSurfaceOffset);

                nvPusherAdjust(3);

                nvMoCompParams.currentDestinationField = 0;
                nvMoCompParams.destinationField = 0;

                // For 1080I, reset offset.
                if (nvMoCompParams.surfaceLumaPitch >= 1024) {
                    nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                                  NV056_SET_SURFACE_COLOR_OFFSET | 0x40000);
                    nvPushData(1, nvMoCompParams.surfaceOffset);
                    nvPusherAdjust(2);
                }
            }


            // Perform luma overflow correction pass
            if (pMacroblock->DCTType == DCT_FIELD) {

                nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                              NV056_NO_OPERATION | 0x40000);
                nvPushData(1, 0);

                nvPushData(2, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                              NV056_SET_SURFACE_PITCH | 0x40000);
                nvPushData(3, ((nvMoCompParams.surfaceLumaPitch << 17) | (nvMoCompParams.surfaceLumaPitch << 1)));

                // Change texture0 pitch to default value of 8 bytes
                nvPushData(4, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                               NV056_SET_TEXTURE_CONTROL1(0) | 0x80000);
                nvPushData(5, 8 << 16);
                nvPushData(6, nvMoCompParams.surfaceLumaPitch << 17);

                nvPusherAdjust(7);

                nvCorrectCelsiusFrameLumaMacroblock(pMacroblock, CORRECTION_OVERFLOW_PASS);

                nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                              NV056_NO_OPERATION | 0x40000);
                nvPushData(1, 0);

                // Restore surface pitch
                nvPushData(2, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                              NV056_SET_SURFACE_PITCH | 0x40000);
                nvPushData(3, ((nvMoCompParams.surfaceLumaPitch << 16) | nvMoCompParams.surfaceLumaPitch));

                // Change texture0 pitch to default value of 8 bytes
                nvPushData(4, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                               NV056_SET_TEXTURE_CONTROL1(0) | 0x80000);
                nvPushData(5, 8 << 16);
                nvPushData(6, nvMoCompParams.surfaceLumaPitch << 16);

                nvPusherAdjust(7);

            } else
                nvCorrectCelsiusFrameLumaMacroblock(pMacroblock, CORRECTION_OVERFLOW_PASS);

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

} /* nvMoCompProcessCelsiusFrameMacroblocks */


/*
 * nvMoCompProcessCelsiusFieldMacroblocks
 *
 * Parses the current macroblock command stream and performs the necessary operations on
 * each macroblock.
 *
 *
 * Note that pDriverData must be set before calling this function
 *
 */
DWORD __stdcall nvMoCompProcessCelsiusFieldMacroblocks(LPNVDECODEMACROBLOCK pMacroblocks,
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
    long                        n,i;
    long                        macroblockIndex;
    BYTE                        surfaceDecodeStatus = 0;
    BYTE                        numCombinedMacroblocks[256];
    short                       maxHeight = (short)(dst->wHeight >> 1) - 16;
    short                       maxWidth = (short)dst->wWidth - 16;
    int                         index;
    int                         firstAlignedIDCTIndex;

    nvMoCompParams.vertexMode = 0; // Initialize vertexMode to an invalid value
    nvMoCompParams.surfaceLumaPitch = dst->lPitch;
    nvMoCompParams.surfaceLumaHeight = (dst->wHeight + 1) & ~1;
    nvMoCompParams.surfaceOffset = VIDMEM_OFFSET(dst->fpVidMem);
    nvMoCompParams.surfaceChromaOffset = nvMoCompParams.surfaceLumaPitch * nvMoCompParams.surfaceLumaHeight;
    nvMoCompParams.surfaceBiasedIntraBlockOffset =
        (dst->lPitch * dst->wHeight) + (dst->lPitch * (dst->wHeight >> 1) + (8 * dst->lPitch));
    nvMoCompParams.celsiusDestinationViewportXOffset = CELSIUS_DESTINATION_VIEWPORT_XOFFSET;
    nvMoCompParams.celsiusFieldXOffset = (short)nvMoCompParams.surfaceLumaPitch << 2;

    if (pDriverData->bMCPictureCodingType != PICTURE_TYPE_I) {

        if (!(nvInitCelsiusForMoCompPrediction()))
            return FALSE;

        if (nvMoCompParams.surfaceLumaPitch >= 1024) {
            nvDoBidirectionalFieldPrediction = nvDoCelsiusBidirectionalHDFieldPrediction;
            nvDoForwardFieldPrediction = nvDoCelsiusForwardHDFieldPrediction;
            nvDoBackwardFieldPrediction = nvDoCelsiusBackwardHDFieldPrediction;
        } else {
            nvDoBidirectionalFieldPrediction = nvDoCelsiusBidirectionalDVDFieldPrediction;
            nvDoTwoBidirectionalFieldPredictions = nvDoTwoCelsiusBidirectionalDVDFieldPredictions;
            nvDoForwardFieldPrediction = nvDoCelsiusForwardDVDFieldPrediction;
            nvDoTwoForwardFieldPredictions = nvDoTwoCelsiusForwardDVDFieldPredictions;
            nvDoBackwardFieldPrediction = nvDoCelsiusBackwardDVDFieldPrediction;
            nvDoTwoBackwardFieldPredictions = nvDoTwoCelsiusBackwardDVDFieldPredictions;
        }

        nvPredictFrameLumaMacroblock = nvPredictCelsiusFrameLumaMacroblock;
        nvPredictFrameChromaMacroblock = nvPredictCelsiusFrameChromaMacroblock;
        nvPredictFieldLumaMacroblock = nvPredictCelsiusFieldLumaMacroblock;
        nvPredictFieldChromaMacroblock = nvPredictCelsiusFieldChromaMacroblock;

        nvMoCompParams.currentForwardField = 0;
        nvMoCompParams.currentBackwardField = 0;
        nvMoCompParams.currentDestinationField = 0;

        if (pDriverData->bMCPictureStructure == PICTURE_STRUCTURE_BOTTOM_FIELD) {

            nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                          NV056_NO_OPERATION | 0x40000);
            nvPushData(1, 0);

            nvPushData(2, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                          NV056_SET_SURFACE_COLOR_OFFSET | 0x40000);
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
					if (pMacroblock->motionType == FIELD_STRUCTURE_MOTION_FIELD){
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
					}
					else if (pMacroblock->motionType == FIELD_STRUCTURE_MOTION_16X8){
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
					if (pMacroblock->motionType == FIELD_STRUCTURE_MOTION_FIELD){
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
					}
					else if (pMacroblock->motionType == FIELD_STRUCTURE_MOTION_16X8){
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
					if (pMacroblock->motionType == FIELD_STRUCTURE_MOTION_FIELD){
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
					}
					else if (pMacroblock->motionType == FIELD_STRUCTURE_MOTION_16X8){
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

        // Change surface offsets to chroma portion of surface
        nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                      NV056_NO_OPERATION | 0x40000);
        nvPushData(1, 0);

        nvPushData(2, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                      NV056_SET_SURFACE_COLOR_OFFSET | 0x40000);

        if (pDriverData->bMCPictureStructure == PICTURE_STRUCTURE_BOTTOM_FIELD) {
            nvMoCompParams.currentDestinationField = 1;
            nvPushData(3, nvMoCompParams.surfaceOffset + nvMoCompParams.surfaceChromaOffset + nvMoCompParams.surfaceLumaPitch);
        } else {
            nvMoCompParams.currentDestinationField = 0;
            nvPushData(3, nvMoCompParams.surfaceOffset + nvMoCompParams.surfaceChromaOffset);
        }

        nvPushData(4, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                      NV056_SET_TEXTURE_OFFSET(0) | 0x40000);
        nvPushData(5, pDriverData->dwMCForwardSurfaceOffset + nvMoCompParams.surfaceChromaOffset);

        nvPushData(6, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                      NV056_SET_TEXTURE_OFFSET(1) | 0x40000);
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
    }

    if (!(nvInitCelsiusForMoCompCorrection()))
        return FALSE;

    SET_BIDIRECTIONAL_VERTEX_ARRAY_MODE();

    if (nvMoCompParams.surfaceLumaPitch >= 1024) {
        nvDoAllIntraFieldCorrections = nvDoAllCelsiusIntraHDFieldCorrections;
        nvDoAllFieldCorrections = nvDoAllCelsiusHDFieldCorrections;
        nvDoIntraFieldCorrection = nvDoCelsiusIntraHDFieldCorrection;
        nvDoOptimizedIntraFieldCorrection = nvDoOptimizedCelsiusIntraHDFieldCorrection;
        nvDoFieldCorrection = nvDoCelsiusHDFieldCorrection;
    } else {
        nvDoAllIntraFieldCorrections = nvDoAllCelsiusIntraDVDFieldCorrections;
        nvDoAllFieldCorrections = nvDoAllCelsiusDVDFieldCorrections;
        nvDoIntraFieldCorrection = nvDoCelsiusIntraDVDFieldCorrection;
        nvDoOptimizedIntraFieldCorrection = nvDoOptimizedCelsiusIntraDVDFieldCorrection;
        nvDoFieldCorrection = nvDoCelsiusDVDFieldCorrection;
    }

    nvDoAllIntraFrameCorrections = nvDoAllCelsiusIntraFrameCorrections;
    nvDoAllFrameCorrections = nvDoAllCelsiusFrameCorrections;
    nvDoIntraFrameCorrection = nvDoCelsiusIntraFrameCorrection;
    nvDoFrameCorrection = nvDoCelsiusFrameCorrection;

    // Default motion comp correction ICW
    moCompCorrectionICW = (NV056_SET_COMBINER_COLOR_ICW_A_MAP_UNSIGNED_IDENTITY << 29) |
                          (NV056_SET_COMBINER_COLOR_ICW_A_ALPHA_FALSE << 28) |
                          (NV056_SET_COMBINER_COLOR_ICW_A_SOURCE_REG_1 << 24) |

                          (NV056_SET_COMBINER_COLOR_ICW_B_MAP_HALFBIAS_NORMAL << 21) |
                          (NV056_SET_COMBINER_COLOR_ICW_B_ALPHA_FALSE << 20) |
                          (NV056_SET_COMBINER_COLOR_ICW_B_SOURCE_REG_8 << 16) |

                          (NV056_SET_COMBINER_COLOR_ICW_C_MAP_UNSIGNED_IDENTITY << 13) |
                          (NV056_SET_COMBINER_COLOR_ICW_C_ALPHA_FALSE << 12) |
                          (NV056_SET_COMBINER_COLOR_ICW_C_SOURCE_REG_1 << 8) |

                          (NV056_SET_COMBINER_COLOR_ICW_D_MAP_UNSIGNED_IDENTITY << 5) |
                          (NV056_SET_COMBINER_COLOR_ICW_D_ALPHA_FALSE << 4) |
                           NV056_SET_COMBINER_COLOR_ICW_D_SOURCE_REG_9;

    // Non-prebiased motion comp correction ICW
    moCompNonPrebiasedCorrectionICW = (NV056_SET_COMBINER_COLOR_ICW_A_MAP_UNSIGNED_IDENTITY << 29) |
                                      (NV056_SET_COMBINER_COLOR_ICW_A_ALPHA_FALSE << 28) |
                                      (NV056_SET_COMBINER_COLOR_ICW_A_SOURCE_REG_1 << 24) |

                                      (NV056_SET_COMBINER_COLOR_ICW_B_MAP_UNSIGNED_IDENTITY << 21) |
                                      (NV056_SET_COMBINER_COLOR_ICW_B_ALPHA_FALSE << 20) |
                                      (NV056_SET_COMBINER_COLOR_ICW_B_SOURCE_REG_8 << 16) |

                                      (NV056_SET_COMBINER_COLOR_ICW_C_MAP_UNSIGNED_IDENTITY << 13) |
                                      (NV056_SET_COMBINER_COLOR_ICW_C_ALPHA_FALSE << 12) |
                                      (NV056_SET_COMBINER_COLOR_ICW_C_SOURCE_REG_0 << 8) |

                                      (NV056_SET_COMBINER_COLOR_ICW_D_MAP_UNSIGNED_IDENTITY << 5) |
                                      (NV056_SET_COMBINER_COLOR_ICW_D_ALPHA_FALSE << 4) |
                                       NV056_SET_COMBINER_COLOR_ICW_D_SOURCE_REG_0;

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

        if (macroblockLimit > 24)
            macroblockLimit = 24;

        pDriverData->dwMCFirstAlignedIDCTIndex = pMacroblock->firstIDCTIndex & ~1;

        dwFirstAlignedIDCTOffset = pDriverData->dwMCFirstAlignedIDCTIndex * 64; // 64 bytes per IDCT block

        // Force both texture caches to flush
        nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                      NV056_SET_TEXTURE_OFFSET(0) | 0x80000);
        nvPushData(1, dwFirstAlignedIDCTOffset);
        nvPushData(2, dwSurfaceOffset);

        nvPusherAdjust(3);

        if (pDriverData->bMCCurrentExecuteFunction ==
            EXECUTE_NON_PREBIASED_MOTION_COMPENSATION) {
            // Modify combiners to perform non-prebiased intrablock correction
            nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                          NV056_SET_COMBINER_COLOR_ICW(0) | 0x40000);
            nvPushData(1, moCompNonPrebiasedCorrectionICW);

            nvPusherAdjust(2);

            SET_UNBIASED_CORRECTION_DATA_FORMAT();
        } else {
            SET_CORRECTION_DATA_FORMAT();
        }    

        // Perform first correction pass
        if (pDriverData->bMCPictureCodingType == PICTURE_TYPE_I) {
            nvCorrectCelsiusIntraField(pMacroblock, macroblockLimit);
            pMacroblock += macroblockLimit;
            n -= macroblockLimit;
        } else {
            long saveMacroblockLimit = macroblockLimit;

            pMacroblock1 = pMacroblock;

            // Change texture1 surface offset to point to block of half bias values (0x80)
            nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                          NV056_SET_TEXTURE_OFFSET(1) | 0x40000);
            nvPushData(1, nvMoCompParams.surfaceOffset + nvMoCompParams.surfaceBiasedIntraBlockOffset);

            // Change texture0 pitch to 16 bytes
            nvPushData(2, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                           NV056_SET_TEXTURE_CONTROL1(0) | 0x80000);
            nvPushData(3, 16 << 16);
            nvPushData(4, 16 << 16); // half bias Intra block pitch

            nvPusherAdjust(5);

            while (--macroblockLimit >= 0) {
                if ((pMacroblock->overflowCodedBlockPattern != 0) && (pMacroblock->macroblockType != MB_INTRA))
                    numCombinedMacroblocks[macroblockIndex++] = (BYTE) index;
                index++;
                if ((pMacroblock->codedBlockPattern != 0) && (pMacroblock->macroblockType == MB_INTRA))
                     nvCorrectCelsiusFieldMacroblock(pMacroblock, CORRECTION_FIRST_PASS);
                pMacroblock++;
                --n;
            }

            nvPusherStart(TRUE);

            // Force both texture caches to flush
            nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                          NV056_NO_OPERATION | 0x40000);
            nvPushData(1, 0);

            nvPushData(2, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                          NV056_SET_TEXTURE_OFFSET(0) | 0x80000);
            nvPushData(3, dwFirstAlignedIDCTOffset);
            nvPushData(4, dwSurfaceOffset);

            nvPusherAdjust(5);

            // Change texture0 pitch to 16 bytes
            nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                           NV056_SET_TEXTURE_CONTROL1(0) | 0x80000);
            nvPushData(1, 16 << 16);
            nvPushData(2, nvMoCompParams.surfaceLumaPitch << 17);

            nvPusherAdjust(3);

            if (pDriverData->bMCCurrentExecuteFunction ==
                EXECUTE_NON_PREBIASED_MOTION_COMPENSATION) {
                // Modify combiners to perform interblock correction
                nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                              NV056_SET_COMBINER_COLOR_ICW(0) | 0x40000);
                nvPushData(1, moCompCorrectionICW);

                nvPusherAdjust(2);
            }    

            SET_CORRECTION_DATA_FORMAT();

            nvMoCompParams.currentDestinationField = 0;

            pMacroblock = pMacroblock1;
            macroblockLimit = saveMacroblockLimit;

            while (--macroblockLimit >= 0) {
                if ((pMacroblock->codedBlockPattern != 0) && (pMacroblock->macroblockType != MB_INTRA))
                     nvCorrectCelsiusFieldMacroblock(pMacroblock, CORRECTION_FIRST_PASS);
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
        index = -24;

        nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                      NV056_NO_OPERATION | 0x40000);
        nvPushData(1, 0);

        // DO NOT REMOVE: This is necessary to work around an NV10 texture cache bug
        nvPushData(2, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                      NV056_SET_SURFACE_COLOR_OFFSET | 0x40000);
        nvPushData(3, dwSurfaceOffset);

        nvPusherAdjust(4);

        if (pDriverData->bMCCurrentExecuteFunction ==
            EXECUTE_NON_PREBIASED_MOTION_COMPENSATION) {
            // Modify combiners to perform interblock correction
            nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                          NV056_SET_COMBINER_COLOR_ICW(0) | 0x40000);
            nvPushData(1, moCompCorrectionICW);

            nvPusherAdjust(2);
        }    

        SET_CORRECTION_DATA_FORMAT();

        while (--n >= 0) {

            firstAlignedIDCTIndex = (&pMacroblock[numCombinedMacroblocks[macroblockIndex]])->firstIDCTIndex & ~1;

            if (firstAlignedIDCTIndex - index >= 24) {
                pDriverData->dwMCFirstAlignedIDCTIndex = firstAlignedIDCTIndex;

                index = firstAlignedIDCTIndex;

                // Attempt to sanity check index
                if ((pDriverData->dwMCFirstAlignedIDCTIndex << 6) > pDriverData->dwMCIDCTAGPCtxDmaSize[0])
                    pDriverData->dwMCFirstAlignedIDCTIndex = 0;

                dwFirstAlignedIDCTOffset = pDriverData->dwMCFirstAlignedIDCTIndex * 64; // 64 bytes per IDCT block

                // Force both texture caches to flush
                nvPushData(0, dDrawSubchannelOffset(NV_DD_CELSIUS) +
                              NV056_SET_TEXTURE_OFFSET(0) | 0x80000);
                nvPushData(1, dwFirstAlignedIDCTOffset);
                nvPushData(2, dwSurfaceOffset);

                nvPusherAdjust(3);

                nvMoCompParams.currentDestinationField = 0;
            }

            // Perform overflow correction pass
            nvCorrectCelsiusFieldMacroblock((&pMacroblock[numCombinedMacroblocks[macroblockIndex]]), CORRECTION_OVERFLOW_PASS);

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

} /* nvMoCompProcessCelsiusFieldMacroblocks */


#endif  // NVARCH >= 0x10
