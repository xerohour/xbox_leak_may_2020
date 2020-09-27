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
* Module: GRMDNV04.C                                                        *
*   The graphics chip dependent HAL GR method routines are kept here.       *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*                                                                           *
\***************************************************************************/

#include <nv_ref.h>
#include <nv4_ref.h>
#include <nvrm.h>
#include <nv4_hal.h>
#include "nvhw.h"
#include "nvhalcpp.h"

//
// First level method handler.
//
RM_STATUS nvHalGrClassSoftwareMethod_NV04(PHALHWINFO, PGREXCEPTIONDATA);

//
// NV4_CONTEXT_SURFACES_ARGB_ZS methods handled in the HAL.
//
static RM_STATUS _nvHalClass053SetClipHorizontal(PHALHWINFO, PGREXCEPTIONDATA);
static RM_STATUS _nvHalClass053SetClipVertical(PHALHWINFO, PGREXCEPTIONDATA);

HALGRMETHOD Nv4ContextSurfacesArgbZsHalMethods[] =
{
    {_nvHalClass053SetClipHorizontal,           0x02f8, 0x02fb},
    {_nvHalClass053SetClipVertical,             0x02fc, 0x02ff},
};

//
// NV4_INDEXED_IMAGE_FROM_CPU methods handled in the HAL.
//
static RM_STATUS _nvHalClass060SetColorConversion(PHALHWINFO, PGREXCEPTIONDATA);

HALGRMETHOD Nv4IndexedImageFromCpuHalMethods[] =
{
    {_nvHalClass060SetColorConversion,           0x03e0, 0x03e3}
};

//
// HAL graphics method table.
//
HALGRMETHODS nvHalGrMethods_NV04[] =
{
    { NV4_CONTEXT_SURFACES_ARGB_ZS,     Nv4ContextSurfacesArgbZsHalMethods,
                                        sizeof (Nv4ContextSurfacesArgbZsHalMethods) / sizeof (HALGRMETHOD) },

    { NV4_INDEXED_IMAGE_FROM_CPU,       Nv4IndexedImageFromCpuHalMethods,
                                        sizeof (Nv4IndexedImageFromCpuHalMethods) / sizeof (HALGRMETHOD) },
};

#define NUM_HALGRMETHODS                sizeof (nvHalGrMethods_NV04) / sizeof(HALGRMETHODS)


RM_STATUS
nvHalGrClassSoftwareMethod_NV04(PHALHWINFO pHalHwInfo, PGREXCEPTIONDATA pGrExceptionData)
{
    U032 i, j;

    for (i = 0; i < NUM_HALGRMETHODS; i++)
    {
        //
        // Find out if this class has any HAL methods.
        //
        if (nvHalGrMethods_NV04[i].classNum == pGrExceptionData->classNum)
        {
            //
            // See if this particular method is a HAL method.
            //
            for (j = 0; j < nvHalGrMethods_NV04[i].methodMax; j++)
            {
                if ((pGrExceptionData->Offset >= nvHalGrMethods_NV04[i].pMethods[j].Low) &&
                    (pGrExceptionData->Offset < nvHalGrMethods_NV04[i].pMethods[j].High))
                {
                    //
                    // Idle graphics.
                    //
                    GR_DONE();

                    //
                    // Route the method.
                    //
                    pGrExceptionData->MethodStatus = nvHalGrMethods_NV04[i].pMethods[j].Proc(pHalHwInfo, pGrExceptionData);
                    return (RM_OK);
                }
            }
        }
    }

    //
    // HAL didn't service this method.
    // This isn't really an error, since the RM may still want
    // to do something.
    //
    return (RM_ERROR);
}

static RM_STATUS
_nvHalClass053SetClipHorizontal(PHALHWINFO pHalHwInfo, PGREXCEPTIONDATA pGrExceptionData)
{
    U032 savedContext;
    
    GR_PRINTF((DBG_LEVEL_INFO, "NVRM: nvHalClass053SetClipHorizontal\r\n"));

    //
    // ERROR CHECKING??
    //
    
    //
    // HW HACK!!  NV4 contains internal error checking that limits the clip origin to zero
    // if the current object is the 3D_SURFACE.  We need to temporarily remove the current
    // context object, put in a dummy value, then place the original back.  Yuck.
    //
    savedContext = REG_RD32(NV_PGRAPH_CTX_SWITCH1);
    REG_WR32(NV_PGRAPH_CTX_SWITCH1, 0x0100004A);

    //
    // Force this X clip information into the hardware.  Note that MIN must be written
    // before MAX.
    //
    REG_WR32(NV_PGRAPH_ABS_UCLIP_XMIN, pGrExceptionData->Data & 0x7FFF);
    REG_WR32(NV_PGRAPH_ABS_UCLIP_XMAX, ((pGrExceptionData->Data >> 16) & 0x7FFF) + (pGrExceptionData->Data & 0x7FFF));
    
    //
    // KJK: Need to update the valid bits here as well.
    //
    
    //
    // HW HACK!! Now put back the original context for this subchannel
    //
    REG_WR32(NV_PGRAPH_CTX_SWITCH1, savedContext);
    
    return (RM_OK);
}

static RM_STATUS
_nvHalClass053SetClipVertical(PHALHWINFO pHalHwInfo, PGREXCEPTIONDATA pGrExceptionData)

{
    U032 savedContext;

    GR_PRINTF((DBG_LEVEL_INFO, "NVRM: nvHalClass053SetClipVertical\r\n"));    

    //
    // ERROR CHECKING??
    //
    
    //
    // HW HACK!!  NV4 contains internal error checking that limits the clip origin to zero
    // if the current object is the 3D_SURFACE.  We need to temporarily remove the current
    // context object, put in a dummy value, then place the original back.  Yuck.
    //
    savedContext = REG_RD32(NV_PGRAPH_CTX_SWITCH1);
    REG_WR32(NV_PGRAPH_CTX_SWITCH1, 0x0100004A);
    
    //
    // Force this Y clip information into the hardware.  Note that MIN must be written
    // before MAX.
    //
    REG_WR32(NV_PGRAPH_ABS_UCLIP_YMIN, pGrExceptionData->Data & 0x7FFF);
    REG_WR32(NV_PGRAPH_ABS_UCLIP_YMAX, ((pGrExceptionData->Data >> 16) & 0x7FFF) + (pGrExceptionData->Data & 0x7FFF));    
    
    //
    // KJK: Need to update the valid bits here as well.
    //
    
    //
    // HW HACK!! Now put back the original context for this subchannel
    //
    REG_WR32(NV_PGRAPH_CTX_SWITCH1, savedContext);
    
    return (RM_OK);
}

static RM_STATUS
_nvHalClass060SetColorConversion(PHALHWINFO pHalHwInfo, PGREXCEPTIONDATA pGrExceptionData)

{
    PGRHALINFO_NV04 pGrHalPvtInfo;
    PGRAPHICSCHANNEL_NV04 pGrChannel;

    GR_PRINTF((DBG_LEVEL_INFO, "NVRM: nvHalClass060SetColorConversion\r\n"));    

    pGrHalPvtInfo = (PGRHALINFO_NV04)pHalHwInfo->pGrHalPvtInfo;
    pGrChannel = &pGrHalPvtInfo->grChannels[pGrExceptionData->ChID];

    //
    // Set the dither mode.
    //
    // Since this comes in as a synchronous graphics exception, we can
    // assume we own the channel and can change the dither mode.
    //
    // The resource manager will handle error checking, so all we're
    // concerned with here is updating the hardware.
    // 
    switch (pGrExceptionData->Data)
    {
        case NV064_SET_COLOR_CONVERSION_DITHER:
            pGrChannel->Debug3 |= DRF_DEF(_PGRAPH, _DEBUG_3, _POSTDITHER_2D, _ENABLED);
            FLD_WR_DRF_DEF(_PGRAPH, _DEBUG_3, _POSTDITHER_2D, _ENABLED);
            break;
        case NV064_SET_COLOR_CONVERSION_TRUNCATE:
            pGrChannel->Debug3 &= ~DRF_MASK(NV_PGRAPH_DEBUG_3_POSTDITHER_2D);
            FLD_WR_DRF_DEF(_PGRAPH, _DEBUG_3, _POSTDITHER_2D, _DISABLED);
            break;
        default:
            //
            // Data invalid.
            // Resource manager will catch this error and handle it
            // appropriately.
            //
            break;
    }
    
    return (RM_OK);
}
