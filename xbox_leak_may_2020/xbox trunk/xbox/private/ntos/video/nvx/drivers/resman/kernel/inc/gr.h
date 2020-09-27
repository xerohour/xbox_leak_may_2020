#ifndef _GR_H_
#define _GR_H_
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
* Module: GR.H                                                              *
*       Graphics engine management.                                         *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*                                                                           *
\***************************************************************************/
//
// Include balanced tree defines.
//
#ifndef _BTREE_H_
#include <btree.h>
#endif // _BTREE_H_
#ifndef _VBLANK_H_
#include <vblank.h>
#endif // _VBLANK_H_

//---------------------------------------------------------------------------
//
//  Defines.
//
//---------------------------------------------------------------------------

#define INVALID_COLOR_FORMAT    0xFFFFFFFF
#define INVALID_MONO_FORMAT     0xFFFFFFFF
#define INVALID_CHID            0xFFFFFFFF
#define INVALID_BUFFER          0xFFFFFFFF
#define BOTH_BUFFERS            3
#define BUFFER(b)               BIT(b)
#define PACK_XY(x,y)            ((((y)&0xFFFF)<<16)|((x)&0xFFFF))
#define PACK_WH(x,y)            ((((h)&0xFFFF)<<16)|((w)&0xFFFF))
#define MAX_GRPATCH_FANOUT      56
#define MAX_GRPATCH_INPUT       56
// If we are using a window manager, then we will allow complex canvas shapes
// If we are not, we will just support the CLIP REGIONS allowed in hardware
#ifdef USE_GUI
#define MAX_CLIP_REGIONS        16
#else
#define MAX_CLIP_REGIONS        2
#endif

#define NUM_GRAPHICS_DEVICES    0x1C
#define FIRST_RENDERING_DEVICE  0x07
#define GR_RAM_SIZE             32
#define ZFOG_RAM_SIZE           16

#define GR_DEVICE_INDEX(d)      ((((d)>>16)&0x7F)-0x41)
#define GR_IDLE()               while ((volatile U032)REG_RD32(NV_PGRAPH_STATUS)) {         \
                                    V032 pmc;                                               \
                                    pmc = REG_RD32(NV_PMC_INTR_0);                          \
                                    if (pmc & DRF_DEF(_PMC, _INTR_0, _PGRAPH, _PENDING)) {  \
                                        grService(pDev);                                    \
                                    }                                                       \
                                    if (pmc & DRF_DEF(_PMC, _INTR_0, _PCRTC, _PENDING)) {   \
                                        VBlank(pDev);                                       \
                                    }                                                       \
                                }
#define GR_DONE()				while ((volatile U032)REG_RD32(NV_PGRAPH_STATUS))   \
                                        ;
								   
#define GR_SAVE_STATE(misc)                                                         \
{                                                                                   \
        misc = REG_RD32(NV_PGRAPH_FIFO);                                            \
        REG_WR32(NV_PGRAPH_FIFO, DRF_DEF(_PGRAPH, _FIFO, _ACCESS, _DISABLED));      \
}

#define GR_RESTORE_STATE(misc)                                                      \
{                                                                                   \
        REG_WR32(NV_PGRAPH_FIFO, misc);                                             \
}

#define GR_ASSERT_CHID(chid)    (!(REG_RD32(NV_PGRAPH_INTR_0) & DRF_DEF(_PGRAPH, _INTR_0, _CONTEXT_SWITCH, _PENDING)) && (REG_RD_DRF(_PGRAPH, _CTX_USER, _CHID) == (chid)))

#define NV3_GR_OPERATIONS   3
#define NV4_GR_OPERATIONS   6

#define NV_SRCCOPY          NV_PGRAPH_CTX_SWITCH1_PATCH_CONFIG_SRCCOPY     
#define NV_SRCCOPY_AND      NV_PGRAPH_CTX_SWITCH1_PATCH_CONFIG_SRCCOPY_AND
#define NV_ROP_AND          NV_PGRAPH_CTX_SWITCH1_PATCH_CONFIG_ROP_AND    
#define NV_BLEND_AND        NV_PGRAPH_CTX_SWITCH1_PATCH_CONFIG_BLEND_AND  
#define NV_BLEND_PREMULT    NV_PGRAPH_CTX_SWITCH1_PATCH_CONFIG_BLEND_PRE  
#define NV_SRCCOPY_PREMULT  NV_PGRAPH_CTX_SWITCH1_PATCH_CONFIG_SRCCOPY_PRE

//---------------------------------------------------------------------------
//
// Miscellaneous structure definitions.
//
//---------------------------------------------------------------------------

#define REGION_UPDATE_CANVAS    0x00000001
#define REGION_UPDATE_CLIP      0x00000002
#define REGION_UPDATE_DISABLE   0x00000003
typedef struct _def_canvas_region
{
    U032 hwnd;
    S032 CanvasLeft;
    S032 CanvasRight;
    S032 CanvasTop;
    S032 CanvasBottom;
    U032 ClipCount;
    S032 ClipLeft[MAX_CLIP_REGIONS];
    S032 ClipRight[MAX_CLIP_REGIONS];
    S032 ClipTop[MAX_CLIP_REGIONS];
    S032 ClipBottom[MAX_CLIP_REGIONS];
    U032 ClipInOut[MAX_CLIP_REGIONS];
    U032 UpdateFlags;
} CANVASREGION, * PCANVASREGION;

//---------------------------------------------------------------------------
//
//  Graphics class defines.
//
//---------------------------------------------------------------------------

typedef struct _def_ordinal_object   ORDINALOBJECT,  *PORDINALOBJECT;
typedef struct _def_video_patchcord  VIDEOPATCHCORD, *PVIDEOPATCHCORD;
typedef struct _def_image_patchcord  IMAGEPATCHCORD, *PIMAGEPATCHCORD;
typedef struct _def_beta_patchcord   BETAPATCHCORD,  *PBETAPATCHCORD;
typedef struct _def_rop_patchcord    ROPPATCHCORD,   *PROPPATCHCORD;
typedef struct _def_image_in_mem_object IMAGEINMEMOBJECT, *PIMAGEINMEMOBJECT;
typedef struct _def_canvas
{
    NODE Node;
    U032 CanvasID;
    U032 UsageCount;
    U032 BufferToggle;
    U032 xyMin;
    U032 xyMax;
    U032 ClipCount;
    U032 ClipMisc;
    U032 xyClipMin[MAX_CLIP_REGIONS];
    U032 xyClipMax[MAX_CLIP_REGIONS];
    U032 InOutClip[MAX_CLIP_REGIONS];
    U032 CplxClipCount;
    U032 xyCplxClipMin[2];
    U032 xyCplxClipMax[2];
} CANVAS, *PCANVAS;

// Common fields between most graphics object structures.
typedef struct _def_common_object COMMONOBJECT, *PCOMMONOBJECT;
struct _def_common_object
{
    OBJECT      Base;
    U032        Valid;
    U032        NotifyPending;
    U032        Instance;
    PDMAOBJECT  NotifyObject;
    PDMAOBJECT  Dma0Object;
    PDMAOBJECT  Dma1Object;
    PCOMMONOBJECT Next;
};

// Macros to get to the common fields
#define CBase            Common.Base
#define CValid           Common.Valid
#define CNotifyPending   Common.NotifyPending
#define CInstance        Common.Instance
#define CNext            Common.Next
typedef struct _def_video_object
{
    OBJECT          Base;
    PVIDEOPATCHCORD VideoOutput;
    PVIDEOPATCHCORD VideoInput[MAX_GRPATCH_INPUT];
} VIDEOOBJECT, *PVIDEOOBJECT;
struct _def_video_patchcord
{
    COMMONOBJECT    Common;
    //OBJECT       Base;
    U032         FanOut;
    PVIDEOOBJECT Source;
    PVIDEOOBJECT Destination[MAX_GRPATCH_FANOUT];
};
typedef struct _def_video_switch_object
{
    OBJECT          Base;
    PVIDEOPATCHCORD VideoOutput;
    PVIDEOPATCHCORD VideoInput[2];
    VBLANKNOTIFY    VBlankNotify[2];
    PORDINALOBJECT  Ordinal;
    PCANVAS         Canvas;
} VIDEOSWITCHOBJECT, *PVIDEOSWITCHOBJECT;
struct _def_ordinal_object
{
    OBJECT             Base;
    U032               Value;
    PVIDEOSWITCHOBJECT Switch[MAX_GRPATCH_FANOUT];
    U032               FanOut;
};
typedef struct _def_video_sink
{
    COMMONOBJECT    Common;
    //OBJECT          Base;
    PDMAOBJECT      NameXlate;
    PVIDEOPATCHCORD VideoInput[MAX_GRPATCH_INPUT];
} VIDEOSINKOBJECT, *PVIDEOSINKOBJECT;
typedef struct _def_video_colormap_object
{
    COMMONOBJECT    Common;
    //OBJECT          Base;
    PVIDEOPATCHCORD VideoOutput;
    PVIDEOPATCHCORD VideoInput;
    U032            ColorFormat;
    PDMAOBJECT      Xlate;
    U032            Start;
    U032            Length;
    U032            DirtyStart;
    U032            DirtyLength;
    U032            ColorMap[256];
} VIDEOCOLORMAPOBJECT, *PVIDEOCOLORMAPOBJECT;
typedef struct _def_video_scaler_object
{
    COMMONOBJECT            Common;
    VIDEOSCALERHALOBJECT    HalObject;
} VIDEOSCALEROBJECT,     *PVIDEOSCALEROBJECT;
typedef struct _def_video_colorkey_object
{
    COMMONOBJECT            Common;
    VIDEOCOLORKEYHALOBJECT  HalObject;
#if 0
    U032                ColorFormat;
    U032                Color;      
    U032                xClipMin;   
    U032                xClipMax;   
    U032                yClipMin;   
    U032                yClipMax;   
#endif
} VIDEOCOLORKEYOBJECT,   *PVIDEOCOLORKEYOBJECT;
//
// Pixel source/destination objects.
//
typedef struct _def_render_common      RENDERCOMMON,     *PRENDERCOMMON;
typedef struct _def_render_common_object RENDERCOMMONOBJECT, *PRENDERCOMMONOBJECT;

//
// Graphics patch context representation
//
// This is a "snapshot" of the graphics context as it
// relates to a given patch.
//
typedef struct _def_patch_context
{
    U032                 VideoConfig;
    PVIDEOCOLORMAPOBJECT ColorMap0;
    PVIDEOCOLORMAPOBJECT ColorMap1;
    PVIDEOSCALEROBJECT   VideoScaler;		
    PVIDEOCOLORKEYOBJECT VideoColorKey;
    PCOMMONOBJECT        ColorKey;
    PCOMMONOBJECT        Pattern;
    PCOMMONOBJECT        Clip;
    PCOMMONOBJECT        Rop;
    PCOMMONOBJECT        Beta1;
    PCOMMONOBJECT        Beta4;
    PCOMMONOBJECT        Surface;
    PCOMMONOBJECT        SurfaceSource;
    PCOMMONOBJECT        SurfaceZeta;
    U032                 Operation;
} PATCHCONTEXT, *PPATCHCONTEXT;

// Common field within the various render objects
struct _def_render_common
{
    PATCHCONTEXT    PatchContext;
    PDMAOBJECT      Xlate;
};

// Macros to get to the common fields
//#define RCPatch            RenderCommon.Patch
#define RCPatchContext     RenderCommon.PatchContext
#define RCXlate            RenderCommon.Xlate

// Common structure to be used as template structure for various render objects
struct _def_render_common_object
{
	COMMONOBJECT    Common;
	RENDERCOMMON    RenderCommon;
};

/* typedef */ struct _def_image_in_mem_object
{
	COMMONOBJECT    Common;
	RENDERCOMMON    RenderCommon;
    PIMAGEPATCHCORD ImageOutput;
    PIMAGEPATCHCORD ImageInput[MAX_GRPATCH_INPUT];
    PDMAOBJECT      NameXlate;
    PCANVAS         Canvas;
    U032            Offset;
    U032            Pitch;
    U032            Buffer;
};

//---------------------------------------------------------------------------
//
//  Function prototypes.
//
//---------------------------------------------------------------------------
RM_STATUS grCreateObj(VOID*, PCLASSOBJECT, U032, POBJECT *, VOID*);
RM_STATUS grDestroyObj(VOID*, POBJECT);
VOID      grInitCommon(PCOMMONOBJECT, U032);
VOID      grInitRenderCommon(PRENDERCOMMON);
RM_STATUS grNotifyComplete(VOID);
RM_STATUS grValidateVideoPatchcord(PVIDEOPATCHCORD, PPATCHCONTEXT, U032);
RM_STATUS grInvalidateVideoPatchcord(PVIDEOPATCHCORD);
RM_STATUS grUnloadCurrentChannelContext(PHWINFO);
RM_STATUS grLoadChannelContext(PHWINFO, U032);
RM_STATUS grContextInstanceToObject(PHWINFO, U032, U032, POBJECT *);
RM_STATUS grWaitVBlank(PHWINFO, PVBLANKNOTIFY, U032);
RM_STATUS grCancelVBlank(PHWINFO, PVBLANKNOTIFY);
RM_STATUS grVBlank(PHWINFO);
V032      grService(PHWINFO);
V032      grDmaService(VOID);
RM_STATUS grVideoSwitchComplete(PHWINFO, POBJECT, U032, V032, RM_STATUS);
RM_STATUS grUpdateObjectInstance(PHWINFO, PCOMMONOBJECT);
RM_STATUS grDeleteObjectInstance(PHWINFO, PCOMMONOBJECT);
BOOL      grIsSetContextInHardware(PHWINFO);
#endif // _GR_H_
