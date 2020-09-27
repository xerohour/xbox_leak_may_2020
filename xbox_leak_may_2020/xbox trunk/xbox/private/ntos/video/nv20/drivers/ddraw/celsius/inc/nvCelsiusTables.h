#ifndef _NVCELSIUSTABLES_H
#define _NVCELSIUSTABLES_H

/*
 * Copyright 1993-1998 NVIDIA, Corporation.  All rights reserved.
 *
 * THE INFORMATION CONTAINED HEREIN IS PROPRIETARY AND CONFIDENTIAL TO
 * NVIDIA, CORPORATION.  USE, REPRODUCTION OR DISCLOSURE TO ANY THIRD PARTY
 * IS SUBJECT TO WRITTEN PRE-APPROVAL BY NVIDIA, CORPORATION.
 */
/********************************* Direct 3D *******************************\
*                                                                           *
* Module: nvCelsiusTables.h                                                 *
*     Constants and macros associated with nvCelsiusTables.cpp              *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       Craig Duttweiler    (bertrem)    23June99    created                *
*                                                                           *
\***************************************************************************/

#if (NVARCH >= 0x010)

// table for looking up pre-computed combiner settings for legacy texture blends
extern DWORD celsiusTextureBlendSettings[D3D_TBLEND_MAX+1][4];
extern DWORD celsiusTextureBlendDefault[4];

// table of dependencies between d3d renderstates and celsius state
extern DWORD celsiusDirtyBitsFromRenderState[D3D_RENDERSTATE_MAX+1];

// table of dependencies between d3d texture stage states and celsius state
extern DWORD celsiusDirtyBitsFromTextureStageState[D3D_TEXSTAGESTATE_MAX+1];

// table mapping dp2ops to the celsius method values associated with them.
extern DWORD celsiusBeginEndOp[D3D_DP2OP_MAX+1];

// table mapping dp2ops to the scale and bias required to calculate
// the number of vertices from the number of primitives.
extern DWORD celsiusPrimitiveToPrimitiveMagic[D3D_DP2OP_MAX+1];

#ifdef DEBUG
// table for primitive names
extern char* celsiusPrimitiveName[D3D_DP2OP_MAX+1];
#endif

#endif  // NVARCH >= 0x010

#endif  // _NVCELSIUSTABLES_H

