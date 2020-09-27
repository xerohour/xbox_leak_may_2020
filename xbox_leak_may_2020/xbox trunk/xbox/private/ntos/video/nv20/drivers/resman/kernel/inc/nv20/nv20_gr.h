#ifndef _NV20_GR_H_
#define _NV20_GR_H_

 /***************************************************************************\
|*                                                                           *|
|*        Copyright (c) 1993-2000 NVIDIA, Corp.  All rights reserved.        *|
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
|*     Copyright (c) 1993-2000  NVIDIA, Corp.    NVIDIA  design  patents     *|
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
* Module: NV20_GR.H                                                         *
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
// #define NV20_SW_CTX_SWITCH

//
// On nv20, the graphics channel context is partitioned into 2d and 3d
// state sets.  These data structures help the RM encapsulate what regs
// go where.
//
typedef struct _def_prim_assm_state_nv20    PRIM_ASSM_STATE_NV20, *PPRIM_ASSM_STATE_NV20;
typedef struct _def_grctxpipe_nv20          GRCTXPIPE_NV20, *PGRCTXPIPE_NV20;
typedef struct _def_grctxcmn_nv20           GRCTXCMN_NV20, *PGRCTXCMN_NV20;
typedef struct _def_grctx3d_nv20            GRCTX3D_NV20, *PGRCTX3D_NV20;
typedef struct _def_graphics_channel_nv20   GRAPHICSCHANNEL_NV20, *PGRAPHICSCHANNEL_NV20;
typedef struct _def_graphics_state_nv20     GRAPHICSSTATE_NV20, *PGRAPHICSSTATE_NV20;

//
// Context switching is done in hardware in nv20, so all we
// need to do is reserve space for both 2d/3d state at fifo
// alloc time.  The following macros are used in the hal class
// descriptor table.
//
//#define NV20_GR_2D_CONTEXT_SIZE             (4 * 207)
//#define NV20_GR_3D_CONTEXT_SIZE             (4 * 3457)
//#define NV20_GR_CONTEXT_SIZE                (NV20_GR_2D_CONTEXT_SIZE + NV20_GR_3D_CONTEXT_SIZE)
#define NV20_GR_CONTEXT_SIZE                (sizeof (GRAPHICSCHANNEL_NV20))

//
// The Kelvin class on nv20 requires an additional 200 words
// of "parking" (instance) space with a default alignment of 16.
//
#define NV20_KELVIN_CONTEXT_SIZE            (4 * 200)
#define NV20_KELVIN_CONTEXT_ALIGN           (16)

//---------------------------------------------------------------------------
//
//  Graphics channel context.
//
//---------------------------------------------------------------------------

//
// The format of the graphics channel context is derived
// from /hw/nv20/docs/context_switching.doc.
//

//
// RDI ram selects (pulled from nv20 diags).
//
#define RDI_RAMSEL_IDX_FMT         0x00000026
#define RDI_RAMSEL_CAS0            0x0000002c
#define RDI_RAMSEL_CAS1            0x0000002d
#define RDI_RAMSEL_CAS2            0x0000002e
#define RDI_RAMSEL_ASSM_STATE      0x0000002f
#define RDI_RAMSEL_IDX_CACHE0      0x00000020
#define RDI_RAMSEL_IDX_CACHE1      0x00000021
#define RDI_RAMSEL_IDX_CACHE2      0x00000022
#define RDI_RAMSEL_IDX_CACHE3      0x00000023
#define RDI_RAMSEL_VTX_FILE0       0x00000028
#define RDI_RAMSEL_VTX_FILE1       0x00000029
#define RDI_RAMSEL_XL_P            0x00000010
#define RDI_RAMSEL_XL_VAB          0x00000015
#define RDI_RAMSEL_XL_XFREG        0x0000001a
#define RDI_RAMSEL_XL_XFCTX        0x00000017
#define RDI_RAMSEL_XL_LTCTX        0x00000018
#define RDI_RAMSEL_XL_LTC          0x00000019
#define RDI_RAMSEL_CRSTR_STIPP     0x00000045
#define RDI_RAMSEL_FD_CTRL         0x0000003d
#define RDI_RAMSEL_ROP_REGS        0x00000050

#define RDI_ADDR_ZERO              0x00000000

//
// Pipe state.
//
#define XL_VAB_BASE         NV_IGRAPH_XF2PIPE(NV_IGRAPH_XF_CMD_VAB,0)
#define XL_XFCTX_BASE       NV_IGRAPH_XF2PIPE(NV_IGRAPH_XF_CMD_XFCTX,0)
#define XL_LTCTX_BASE       NV_IGRAPH_XF2PIPE(NV_IGRAPH_XF_CMD_LTCTX,0)
#define XL_LTC0_BASE        NV_IGRAPH_XF2PIPE(NV_IGRAPH_XF_CMD_LTC0,0)
#define XL_LTC1_BASE        NV_IGRAPH_XF2PIPE(NV_IGRAPH_XF_CMD_LTC1,0)
#define XL_LTC2_BASE        NV_IGRAPH_XF2PIPE(NV_IGRAPH_XF_CMD_LTC2,0)
#define XL_LTC3_BASE        NV_IGRAPH_XF2PIPE(NV_IGRAPH_XF_CMD_LTC3,0)

#define IDX_FMT_COUNT       (16+16+3)       // Vertex Array Regs+State
#define CAS0_COUNT		    (6*4+5)         // Primitive Assembly Vtx0
#define CAS1_COUNT          (6*4+5)         // Primitive Assembly Vtx1
#define CAS2_COUNT          (6*4+5)         // Primitive Assembly Vtx2
#define ASSM_STATE_COUNT    (2)             // Primitive Assembly State
#define IDX_CACHE0_COUNT    (32*4)          // FD Coefficients (EvenLeft)
#define IDX_CACHE1_COUNT    (32*4)          // FD Coefficients (OddLeft)
#define IDX_CACHE2_COUNT    (32*4)          // FD Coefficients (EvenRight)
#define IDX_CACHE3_COUNT    (32*4)          // FD Coefficients (OddRight)
#define VTX_FILE0_COUNT		(48*4)          // Vertex File0
#define VTX_FILE1_COUNT		(144*4)         // Vertex File1
#define XL_P_COUNT          (136*4)         // Cheops Program
#define XL_XFCTX_COUNT      (192*4)         // Cheops Context
#define XL_LTCTX_COUNT      (74*4)          // Zoser Context
#define XL_LTC0_COUNT       (4)             // Zoser Context 0
#define XL_LTC1_COUNT       (20)            // Zoser Context 1
#define XL_LTC2_COUNT       (15)            // Zoser Context 2
#define XL_LTC3_COUNT       (14)            // Zoser Context 3
#define XL_LTC_COUNT        (XL_LTC0_COUNT+XL_LTC1_COUNT+XL_LTC2_COUNT+XL_LTC3_COUNT)
#define XL_VAB_COUNT        (17*4)          // VAB
#define CRSTR_STIPP_COUNT   (32)            // RSTR stipple
#define FD_CTRL_COUNT       (15)            // FD
#define ROP_REGS_COUNT      (1)             // ROP Regs

struct _def_grctxpipe_nv20
{
    U032 IdxFmt[IDX_FMT_COUNT];
    U032 Cas0[CAS0_COUNT];
    U032 Cas1[CAS1_COUNT];
    U032 Cas2[CAS2_COUNT];
    U032 AssmState[ASSM_STATE_COUNT];
    U032 IdxCache0[IDX_CACHE0_COUNT];
    U032 IdxCache1[IDX_CACHE1_COUNT];
    U032 IdxCache2[IDX_CACHE2_COUNT];
    U032 IdxCache3[IDX_CACHE3_COUNT];
    U032 VtxFile0[VTX_FILE0_COUNT];
    U032 VtxFile1[VTX_FILE1_COUNT];
    U032 XlP[XL_P_COUNT];
    U032 XlXfCtx[XL_XFCTX_COUNT];
    U032 XlLtCtx[XL_LTCTX_COUNT];
    U032 XlLtc[XL_LTC_COUNT];
    U032 XlVab[XL_VAB_COUNT];
    U032 CrstrStipp[CRSTR_STIPP_COUNT];
    U032 FdCtrl[FD_CTRL_COUNT];
    U032 RopRegs[ROP_REGS_COUNT];
    U032 Pad[4];
};

//
// Because graphics channel context switching is done in hw
// on nv20, we aren't in control of the format of this data
// structure (be careful!).
//

//
// This structure represents the channel context state
// that is either 2d-only or shared between 2d/3d.
//
struct _def_grctxcmn_nv20
{
    U032 ContextUser;
    U032 ContextSwitch1;
    U032 ContextSwitch2;
    U032 ContextSwitch3;
    U032 ContextSwitch4;
    U032 ContextSwitch5;
    U032 ContextCache1[8];
    U032 ContextCache2[8];
    U032 ContextCache3[8];
    U032 ContextCache4[8];
    U032 ContextCache5[8];

    U032 MonoColor0;
    U032 BufferSwizzle2;
    U032 BufferSwizzle5;
    U032 BufferOffset[6];
    U032 BufferBase[6];
    U032 BufferPitch[5];
    U032 BufferLimit[6];
    U032 Chroma;
    U032 Surface;
    U032 State;
    U032 Notify;
    U032 BufferPixel;
    U032 DmaPitch;
    U032 DvdColorFmt;
    U032 ScaledFormat;
    U032 PatternColor0;
    U032 PatternColor1;
    U032 Pattern[2];
    U032 PatternShape;
    U032 PattColorRam[64];
    U032 Rop3;
    U032 BetaAnd;
    U032 BetaPreMult;
    U032 StoredFmt;
    U032 AbsXRam[10];
    U032 AbsYRam[10];
    U032 AbsUClipXMin;
    U032 AbsUClipXMax;
    U032 AbsUClipYMin;
    U032 AbsUClipYMax;
    U032 AbsUClipAXMin;
    U032 AbsUClipAXMax;
    U032 AbsUClipAYMin;
    U032 AbsUClipAYMax;
    U032 SourceColor;
    U032 Misc24_0;
    U032 XYLogicMisc0;
    U032 XYLogicMisc1;
    U032 XYLogicMisc2;
    U032 XYLogicMisc3;
    U032 ClipX0;
    U032 ClipX1;
    U032 ClipY0;
    U032 ClipY1;
    U032 AbsIClipYMax;
    U032 AbsIClipXMax;
    U032 Passthru0;
    U032 Passthru1;
    U032 Passthru2;
    U032 DimxTexture;
    U032 WdimxTexture;
    U032 DmaStart0;
    U032 DmaStart1;
    U032 DmaLength;
    U032 DmaMisc;
    U032 Misc24_1;
    U032 Misc24_2;
    U032 XMisc;
    U032 YMisc;
    U032 Valid1;
};

//
// This structure represents 3d channel context state.
//
struct _def_grctx3d_nv20
{
    U032 AntiAliasing;
    U032 Blend;
    U032 BlendColor;
    U032 BorderColor0;
    U032 BorderColor1;
    U032 BorderColor2;
    U032 BorderColor3;
    U032 BumpMat00_1;
    U032 BumpMat00_2;
    U032 BumpMat00_3;
    U032 BumpMat01_1;
    U032 BumpMat01_2;
    U032 BumpMat01_3;
    U032 BumpMat10_1;
    U032 BumpMat10_2;
    U032 BumpMat10_3;
    U032 BumpMat11_1;
    U032 BumpMat11_2;
    U032 BumpMat11_3;
    U032 BumpOffset1;
    U032 BumpOffset2;
    U032 BumpOffset3;
    U032 BumpScale1;
    U032 BumpScale2;
    U032 BumpScale3;
    U032 ClearRectX;
    U032 ClearRectY;
    U032 ColorClearValue;
    U032 ColorKeyColor0;
    U032 ColorKeyColor1;
    U032 ColorKeyColor2;
    U032 ColorKeyColor3;
    U032 CombineFactor0_0;
    U032 CombineFactor0_1;
    U032 CombineFactor0_2;
    U032 CombineFactor0_3;
    U032 CombineFactor0_4;
    U032 CombineFactor0_5;
    U032 CombineFactor0_6;
    U032 CombineFactor0_7;
    U032 CombineFactor1_0;
    U032 CombineFactor1_1;
    U032 CombineFactor1_2;
    U032 CombineFactor1_3;
    U032 CombineFactor1_4;
    U032 CombineFactor1_5;
    U032 CombineFactor1_6;
    U032 CombineFactor1_7;
    U032 CombineAlphaI0;
    U032 CombineAlphaI1;
    U032 CombineAlphaI2;
    U032 CombineAlphaI3;
    U032 CombineAlphaI4;
    U032 CombineAlphaI5;
    U032 CombineAlphaI6;
    U032 CombineAlphaI7;
    U032 CombineAlphaO0;
    U032 CombineAlphaO1;
    U032 CombineAlphaO2;
    U032 CombineAlphaO3;
    U032 CombineAlphaO4;
    U032 CombineAlphaO5;
    U032 CombineAlphaO6;
    U032 CombineAlphaO7;
    U032 CombineColorI0;
    U032 CombineColorI1;
    U032 CombineColorI2;
    U032 CombineColorI3;
    U032 CombineColorI4;
    U032 CombineColorI5;
    U032 CombineColorI6;
    U032 CombineColorI7;
    U032 CombineColorO0;
    U032 CombineColorO1;
    U032 CombineColorO2;
    U032 CombineColorO3;
    U032 CombineColorO4;
    U032 CombineColorO5;
    U032 CombineColorO6;
    U032 CombineColorO7;
    U032 CombineCtl;
    U032 CombineSpecFog0;
    U032 CombineSpecFog1;
    U032 Control0;
    U032 Control1;
    U032 Control2;
    U032 Control3;
    U032 FogColor;
    U032 FogParam0;
    U032 FogParam1;
    U032 PointSize;
    U032 SetupRaster;
    U032 ShaderClipMode;
    U032 ShaderCtl;
    U032 ShaderProg;
    U032 SemaphoreOffset;
    U032 ShadowCtl;
    U032 ShadowZSlopeThreshold;
    U032 SpecFogFactor0;
    U032 SpecFogFactor1;
    U032 SurfaceClipX;
    U032 SurfaceClipY;
    U032 TexAddress0;
    U032 TexAddress1;
    U032 TexAddress2;
    U032 TexAddress3;
    U032 TexCtl0_0;
    U032 TexCtl0_1;
    U032 TexCtl0_2;
    U032 TexCtl0_3;
    U032 TexCtl1_0;
    U032 TexCtl1_1;
    U032 TexCtl1_2;
    U032 TexCtl1_3;
    U032 TexCtl2_0;
    U032 TexCtl2_1;
    U032 TexFilter0;
    U032 TexFilter1;
    U032 TexFilter2;
    U032 TexFilter3;
    U032 TexFormat0;
    U032 TexFormat1;
    U032 TexFormat2;
    U032 TexFormat3;
    U032 TexImageRect0;
    U032 TexImageRect1;
    U032 TexImageRect2;
    U032 TexImageRect3;
    U032 TexOffset0;
    U032 TexOffset1;
    U032 TexOffset2;
    U032 TexOffset3;
    U032 TexPallete0;
    U032 TexPallete1;
    U032 TexPallete2;
    U032 TexPallete3;
    U032 WindowClipX[8];
    U032 WindowClipY[8];
    U032 ZCompressOcclude;
    U032 ZStencilClearValue;
    U032 ZClipMax;
    U032 ZClipMin;
    U032 ContextDmaA;
    U032 ContextDmaB;
    U032 ContextDmaVtxA;
    U032 ContextDmaVtxB;
    U032 ZOffsetBias;
    U032 ZOffsetFactor;
    U032 Eyevec0;
    U032 Eyevec1;
    U032 Eyevec2;
    U032 Shadow;
    U032 FdData;
    U032 FdSwatch;
    U032 FdExtras;
    U032 Emission0;
    U032 Emission1;
    U032 Emission2;
    U032 SceneAmb0;
    U032 SceneAmb1;
    U032 SceneAmb2;
    U032 ContextDmaGetState;
    U032 BeginPatch0;
    U032 BeginPatch1;
    U032 BeginPatch2;
    U032 BeginPatch3;
    U032 BeginCurve;
    U032 BeginTrans0;
    U032 BeginTrans1;
    U032 Csv0_D;
    U032 Csv0_C;
    U032 Csv1_A;
    U032 Csv1_B;
    U032 CheopsOffset;
};

struct _def_graphics_channel_nv20
{
    GRCTXCMN_NV20   grCtxCmn;
    GRCTX3D_NV20    grCtx3d;
    GRCTXPIPE_NV20  grCtxPipe;
};

//
// Snapshot/reload state.
//
// Because this state is never exposed to the (OpenGL) driver,
// we can make it chip-dependent.
//
// Must include at least the following:
//
//  - per-fragment state
//  - per-vertex state
//  - vertex array state
//  - primitive assembly state
//
struct _def_graphics_state_nv20 {
    struct PerFragmentState {
        U032 Control0;
        U032 Control1;
        U032 Control2;
        U032 Control3;
        U032 Blend;
    } PerFragmentState;
    struct PerVertexState {
        U032 XlVab[XL_VAB_COUNT];
    } PerVertexState;
    NvU32 glBeginEndState;
    struct VertexArrayState {
        U032 IdxFmt[IDX_FMT_COUNT];
    } VertexArrayState;
    struct PrimitiveAssmState {
        U032 Cas0[CAS0_COUNT];
        U032 Cas1[CAS1_COUNT];
        U032 Cas2[CAS2_COUNT];
        U032 AssmState[ASSM_STATE_COUNT];
    } PrimitiveAssmState;
};

#endif // _NV20_GR_H_
