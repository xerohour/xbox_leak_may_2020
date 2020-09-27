#ifndef _NV10_GR_H_
#define _NV10_GR_H_
 /***************************************************************************\
|*                                                                           *|
|*        Copyright (c) 1993-1997 NVIDIA, Corp.  All rights reserved.        *|
|*                                                                           *|
|*     NOTICE TO USER:   The source code  is copyrighted under  U.S. and     *|
|*     international laws.   NVIDIA, Corp. of Sunnyvale, California owns     *|
|*     the copyright  and as design patents  pending  on the design  and     *|
|*     interface  of the NV chips.   Users and possessors of this source     *|
|*     code are hereby granted  a nonexclusive,  royalty-free  copyright     *|
|*     and  design  patent license  to use this code  in individual  and     *|
|*     commercial software.                                                  *|
|*                                                                           *|
|*     Any use of this source code must include,  in the user documenta-     *|
|*     tion and  internal comments to the code,  notices to the end user     *|
|*     as follows:                                                           *|
|*                                                                           *|
|*     Copyright (c) 1993-1997  NVIDIA, Corp.    NVIDIA  design  patents     *|
|*     pending in the U.S. and foreign countries.                            *|
|*                                                                           *|
|*     NVIDIA, CORP.  MAKES  NO REPRESENTATION ABOUT  THE SUITABILITY OF     *|
|*     THIS SOURCE CODE FOR ANY PURPOSE.  IT IS PROVIDED "AS IS" WITHOUT     *|
|*     EXPRESS OR IMPLIED WARRANTY OF ANY KIND.  NVIDIA, CORP. DISCLAIMS     *|
|*     ALL WARRANTIES  WITH REGARD  TO THIS SOURCE CODE,  INCLUDING  ALL     *|
|*     IMPLIED   WARRANTIES  OF  MERCHANTABILITY  AND   FITNESS   FOR  A     *|
|*     PARTICULAR  PURPOSE.   IN NO EVENT SHALL NVIDIA, CORP.  BE LIABLE     *|
|*     FOR ANY SPECIAL, INDIRECT, INCIDENTAL,  OR CONSEQUENTIAL DAMAGES,     *|
|*     OR ANY DAMAGES  WHATSOEVER  RESULTING  FROM LOSS OF USE,  DATA OR     *|
|*     PROFITS,  WHETHER IN AN ACTION  OF CONTRACT,  NEGLIGENCE OR OTHER     *|
|*     TORTIOUS ACTION, ARISING OUT  OF OR IN CONNECTION WITH THE USE OR     *|
|*     PERFORMANCE OF THIS SOURCE CODE.                                      *|
|*                                                                           *|
 \***************************************************************************/
/**************************** Graphics Manager *****************************\
*                                                                           *
* Module: NV10_GR.H                                                         *
*       Graphics engine management.                                         *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*                                                                           *
\***************************************************************************/

//---------------------------------------------------------------------------
//
//  Defines.
//
//---------------------------------------------------------------------------

typedef struct _def_prim_assm_state         PRIM_ASSM_STATE,     *PPRIM_ASSM_STATE;
typedef struct _def_graphics_pipe           GRAPHICSPIPE,        *PGRAPHICSPIPE;
typedef struct _def_graphics_patch_nv10     GRAPHICSPATCH_NV10,  *PGRAPHICSPATCH_NV10;

//---------------------------------------------------------------------------
//
//  Patch database.
//
// This is all the information related to a given patch, which may contain
// multiple patch contexts.
//
//---------------------------------------------------------------------------

#define PIPE_FORMAT_BASE	0x00000000	// 8x base,format registers
#define PIPE_PRIM_BASE		0x00000040	// primType, edgeFlag, context
#define PIPE_ASSM_BASE		0x00000200	// primitive assembly vertices
#define PIPE_FILE_BASE		0x00000800	// vertex file vertices
#define PIPE_VAB_BASE		0x00004400	// VAB in XF engine
#define PIPE_CHEOPS_BASE	0x00006400	// XF cheops state
#define PIPE_ZOSER_BASE		0x00006800	// XF zozer state
#define PIPE_ZOSER_C0_BASE	0x00006c00	// XF zozer C0 state
#define PIPE_ZOSER_C1_BASE	0x00007000	// XF zozer C1 state
#define PIPE_ZOSER_C2_BASE	0x00007400	// XF zozer C2 state
#define PIPE_ZOSER_C3_BASE	0x00007800	// XF zozer C3 state

#define PRIM_ASSM_COUNT			(3*16)
#define VERTEX_FILE_COUNT		(16*16)
#define XF_VAB_COUNT			(8*4)
#define XF_CHEOPS_COUNT			(59*4)
#define XF_ZOSER_COUNT			(47*4)
#define XF_ZOSER_C0_COUNT		(3*4)
#define XF_ZOSER_C1_COUNT		(19*4)
#define XF_ZOSER_C2_COUNT		(12*4)
#define XF_ZOSER_C3_COUNT		(12*4)
#define INDEX_DMA_PRIM_COUNT		4
#define INDEX_DMA_FORMAT_COUNT		16

struct _def_prim_assm_state
{
    U032                IndexDmaPrim[INDEX_DMA_PRIM_COUNT];
    U032                PrimAssm[PRIM_ASSM_COUNT];
};

struct _def_graphics_pipe
{
    U032                PrimAssm[PRIM_ASSM_COUNT];
    U032                VertexFile[VERTEX_FILE_COUNT];
    U032                XF_VAB[XF_VAB_COUNT];
    U032                XF_CHEOPS[XF_CHEOPS_COUNT];
    U032                XF_ZOSER[XF_ZOSER_COUNT];
    U032                XF_ZOSER_C0[XF_ZOSER_C0_COUNT];
    U032                XF_ZOSER_C1[XF_ZOSER_C1_COUNT];
    U032                XF_ZOSER_C2[XF_ZOSER_C2_COUNT];
    U032                XF_ZOSER_C3[XF_ZOSER_C3_COUNT];
    U032                IndexDmaPrim[INDEX_DMA_PRIM_COUNT];
    U032                IndexDmaFormat[INDEX_DMA_FORMAT_COUNT];
};

struct _def_graphics_patch_nv10
{
    U032                ValidContext;
    U032                BufferOffset[6];
    U032                BufferBase[6];
    U032                BufferLimit[6];
    U032                BufferPitch[5];
    U032                Surface;
    U032                State;
    U032                BufferSwizzle[2];
    U032                BufferPixel;
    U032                Notify;
    U032                PatternColor0;
    U032                PatternColor1;
    U032                PatternColorRam[64];
    U032                Pattern[2];
    U032                PatternShape;
    U032                MonoColor0;
    U032                Rop3;
    U032                Chroma;
    U032                BetaAnd;
    U032                BetaPremult;
    U032                Control0;
    U032                Control1;
    U032                Control2;
    U032                Control3;
    U032                Blend;
    U032                BlendColor;
    U032                SetupRaster;
    U032                FogColor;
    U032                ColorKeyColor0;
    U032                ColorKeyColor1;
    U032                PointSize;
    U032                ZOffsetFactor;
    U032                ZOffsetBias;
    U032                ZClipMin;
    U032                ZClipMax;
    U032                WinClipHorz[8];
    U032                WinClipVert[8];
    U032                XFMode[2];
    U032                GlobalState0;
    U032                GlobalState1;
    U032                StoredFmt;
    U032                SourceColor;
    U032                AbsXRam[32];
    U032                AbsYRam[32];
    U032                AbsUClipXMin;
    U032                AbsUClipXMax;
    U032                AbsUClipYMin;
    U032                AbsUClipYMax;
    U032                AbsUClip3DXMin;
    U032                AbsUClip3DXMax;
    U032                AbsUClip3DYMin;
    U032                AbsUClip3DYMax;
    U032                AbsUClipAXMin;
    U032                AbsUClipAXMax;
    U032                AbsUClipAYMin;
    U032                AbsUClipAYMax;
    U032                AbsIClipXMax;
    U032                AbsIClipYMax;
    U032                XYLogicMisc0;
    U032                XYLogicMisc1;
    U032                XYLogicMisc2;
    U032                XYLogicMisc3;
    U032                ClipX0;
    U032                ClipX1;
    U032                ClipY0;
    U032                ClipY1;
    U032                Combine0AlphaICW;
    U032                Combine0ColorICW;
    U032                Combine1AlphaICW;
    U032                Combine1ColorICW;
    U032                Combine0Factor;
    U032                Combine1Factor;
    U032                Combine0AlphaOCW;
    U032                Combine0ColorOCW;
    U032                Combine1AlphaOCW;
    U032                Combine1ColorOCW;
    U032                CombineSpecFogCW0;
    U032                CombineSpecFogCW1;
    U032                TextureOffset0;
    U032                TextureOffset1;
    U032                TexturePalette0;
    U032                TexturePalette1;
    U032                TexControl0_0;
    U032                TexControl0_1;
    U032                TexControl1_0;
    U032                TexControl1_1;
    U032                TexControl2_0;
    U032                TexControl2_1;
    U032                TexFormat0;
    U032                TexFormat1;
    U032                ImageRect0;
    U032                ImageRect1;
    U032                Filter0;
    U032                Filter1;
    U032                Passthru0;
    U032                Passthru1;
    U032                Passthru2;
    U032                DimxTexture;
    U032                WdimxTexture;
    U032                DVDColorFormat;
    U032                ScaledFormat;
    U032                Misc24_0;
    U032                Misc24_1;
    U032                Misc24_2;
    U032                XMisc;
    U032                YMisc;
    U032                Valid1;
    U032                Valid2;
    GRAPHICSPIPE        CurrentPipe;
};

#endif // _NV10_GR_H_
