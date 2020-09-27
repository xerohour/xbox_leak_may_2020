// **************************************************************************
//
//       Copyright 1993-1999 NVIDIA, Corporation.  All rights reserved.
//
//     NOTICE TO USER:   The source code  is copyrighted under  U.S. and
//     international laws.  Users and possessors of this source code are
//     hereby granted a nonexclusive,  royalty-free copyright license to
//     use this code in individual and commercial software.
//
//     Any use of this source code must include,  in the user documenta-
//     tion and  internal comments to the code,  notices to the end user
//     as follows:
//
//       Copyright 1993-1999 NVIDIA, Corporation.  All rights reserved.
//
//     NVIDIA, CORPORATION MAKES NO REPRESENTATION ABOUT THE SUITABILITY
//     OF  THIS SOURCE  CODE  FOR ANY PURPOSE.  IT IS  PROVIDED  "AS IS"
//     WITHOUT EXPRESS OR IMPLIED WARRANTY OF ANY KIND.  NVIDIA, CORPOR-
//     ATION DISCLAIMS ALL WARRANTIES  WITH REGARD  TO THIS SOURCE CODE,
//     INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY, NONINFRINGE-
//     MENT,  AND FITNESS  FOR A PARTICULAR PURPOSE.   IN NO EVENT SHALL
//     NVIDIA, CORPORATION  BE LIABLE FOR ANY SPECIAL,  INDIRECT,  INCI-
//     DENTAL, OR CONSEQUENTIAL DAMAGES,  OR ANY DAMAGES  WHATSOEVER RE-
//     SULTING FROM LOSS OF USE,  DATA OR PROFITS,  WHETHER IN AN ACTION
//     OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,  ARISING OUT OF
//     OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOURCE CODE.
//
//     U.S. Government  End  Users.   This source code  is a "commercial
//     item,"  as that  term is  defined at  48 C.F.R. 2.101 (OCT 1995),
//     consisting  of "commercial  computer  software"  and  "commercial
//     computer  software  documentation,"  as such  terms  are  used in
//     48 C.F.R. 12.212 (SEPT 1995)  and is provided to the U.S. Govern-
//     ment only as  a commercial end item.   Consistent with  48 C.F.R.
//     12.212 and  48 C.F.R. 227.7202-1 through  227.7202-4 (JUNE 1995),
//     all U.S. Government End Users  acquire the source code  with only
//     those rights set forth herein.
//
// ********************************* Direct 3D ******************************
//
//  Module: nvKelvinTables.h
//      tables of values and constants related to the kelvin object
//
// **************************************************************************
//
//  History:
//       Craig Duttweiler        28Apr2000         NV20 development
//
// **************************************************************************

#ifndef _NVKELVINTABLES_H
#define _NVKELVINTABLES_H

#if (NVARCH >= 0x020)

// table for looking up pre-computed combiner settings for legacy texture blends
extern DWORD kelvinTextureBlendSettings[D3D_TBLEND_MAX+1][4];
extern DWORD kelvinTextureBlendDefault[4];

// table of dependencies between d3d renderstates and kelvin state
extern DWORD kelvinDirtyBitsFromRenderState[D3D_RENDERSTATE_MAX+1];

// table of dependencies between d3d texture stage states and kelvin state
extern DWORD kelvinDirtyBitsFromTextureStageState[D3D_TEXSTAGESTATE_MAX+1];

// table mapping dp2ops to the kelvin method values associated with them.
extern DWORD kelvinBeginEndOp[D3D_DP2OP_MAX+1];

// table mapping d3d vertex types to kelvin vertex sizes
extern DWORD kelvinVertexSize[8];

// table mapping d3d vertex types to kelvin vertex types
extern DWORD kelvinVertexType[8];

// table mapping dp2ops to the scale and bias required to calculate
// the number of vertices from the number of primitives.
extern DWORD kelvinPrimitiveToPrimitiveMagic[D3D_DP2OP_MAX+1];

// table for primitive names
#ifdef DEBUG
extern char* kelvinPrimitiveName[D3D_DP2OP_MAX+1];
#endif

#endif  // NVARCH >= 0x020

#endif  // _NVKELVINTABLES_H

