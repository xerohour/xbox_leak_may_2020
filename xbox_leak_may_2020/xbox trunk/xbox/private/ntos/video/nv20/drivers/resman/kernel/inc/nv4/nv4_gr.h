#ifndef _NV4_GR_H_
#define _NV4_GR_H_
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
* Module: NV4_GR.H                                                          *
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

typedef struct _def_graphics_patch_nv4   GRAPHICSPATCH_NV4,  *PGRAPHICSPATCH_NV4;

//---------------------------------------------------------------------------
//
//  Patch database.
//
// This is all the information related to a given patch, which may contain
// multiple patch contexts.
//
//---------------------------------------------------------------------------

struct _def_graphics_patch_nv4
{
    //PGRAPHICSPATCH      Next;
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
    U032                Blend;
    U032                StoredFmt;
    U032                SourceColor;
    U032                AbsXRam[32];
    U032                AbsYRam[32];
    U032                AbsUClipXMin;
    U032                AbsUClipXMax;
    U032                AbsUClipYMin;
    U032                AbsUClipYMax;
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
    U032                URam[16];
    U032                VRam[16];
    U032                MRam[16];
    U032                Combine0Alpha;
    U032                Combine0Color;
    U032                Combine1Alpha;
    U032                Combine1Color;
    U032                Format0;
    U032                Format1;
    U032                Filter0;
    U032                Filter1;
    U032                D3D_XY;
    U032                D3D_U0;
    U032                D3D_V0;
    U032                D3D_U1;
    U032                D3D_V1;
    U032                D3D_ZETA;
    U032                D3D_RGB;
    U032                D3D_S;
    U032                D3D_M;
    U032                Passthru0;
    U032                Passthru1;
    U032                Passthru2;
    U032                DVDColorFormat;
    U032                ScaledFormat;
    U032                Misc24_0;
    U032                Misc24_1;
    U032                Misc24_2;
    U032                XMisc;
    U032                YMisc;
    U032                Valid1;
    U032                Valid2;
};

#endif // _NV4_GR_H_
