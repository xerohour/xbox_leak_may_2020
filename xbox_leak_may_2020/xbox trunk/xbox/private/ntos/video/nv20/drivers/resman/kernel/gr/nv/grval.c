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
* Module: GRVAL.C                                                           *
*   The graphics engine is managed in this module.  Graphics patches are    *
*   validated here.                                                         *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*                                                                           *
\***************************************************************************/
#include <nv_ref.h>
#include <nvrm.h>
#include <state.h>
#include <class.h>
#include <vblank.h>
#include <fifo.h>
#include <dma.h>
#include <dac.h>
#include <gr.h>
#include <modular.h>
#include <os.h>
#include <nv32.h>
#include "nvhw.h"
//
// Externs.
//
// extern PGRAPHICSCHANNEL grTable;

//---------------------------------------------------------------------------
//
//  Validate graphics patchcords.
//
//---------------------------------------------------------------------------

RM_STATUS grValidateVideoPatchcord
(
    // PHWINFO pDev,
    PVIDEOPATCHCORD   VideoPatchcord,
    PPATCHCONTEXT     PatchContext,
    U032              DestCount
)
{
    PVIDEOOBJECT VideoObject = (void *) 0;
    U032         i;
    
    //
    // Initialize context configuration.
    //
    PatchContext->VideoConfig  = BOTH_BUFFERS;
    PatchContext->ColorMap0    = NULL;
    PatchContext->ColorMap1    = NULL;
    PatchContext->VideoScaler  = NULL;
    PatchContext->VideoColorKey = NULL;

    //
    // Find output of patchcord.
    //
    if (VideoPatchcord && VideoPatchcord->FanOut)
    {
        //
        // Search for the first destination
        //
        DBG_VAL_PTR(VideoPatchcord);
        for (i = 0; (i < MAX_GRPATCH_FANOUT) && DestCount; i++)
        {
            if ((VideoObject = VideoPatchcord->Destination[i]))
            {
                DBG_VAL_PTR(VideoPatchcord->Destination[i]);
                DestCount--;
            }
        }
#if DEBUG
        if (DestCount)
        {
            GR_PRINTF((DBG_LEVEL_ERRORS, "NVRM: Invalid patchcord in VidPatchValidation!\n\r"));
            DBG_BREAKPOINT();
            return (RM_ERROR);
        }
#endif // DEBUG
    }
    else
        return (RM_ERR_INCOMPLETE_PATCH);
    //
    // Accumulate video patch configuration.
    //
    while ( VideoObject && (VideoObject->Base.Class->Type != NV_VIDEO_SINK))
    {
        DBG_VAL_PTR(VideoObject);
        /* KJK
        if (VideoObject->Base.Class->Type == NV_VIDEO_SWITCH)
        {
            //
            // Check which buffers to read/write to.
            //
            if (PatchContext->VideoConfig != BOTH_BUFFERS)
                return (RM_ERR_INVALID_PATCH);
            //
            // Ordinal.
            //
            if (((PVIDEOSWITCHOBJECT)VideoObject)->Ordinal)
                PatchContext->VideoSwitch = (PVIDEOSWITCHOBJECT)VideoObject;
            else    
                return (RM_ERR_INCOMPLETE_PATCH);
            //
            // Initialize the buffer toggle for this patch if it hasn't
            //  already been done.
            //
            if (PatchContext->Canvas->BufferToggle == INVALID_BUFFER)
            {
                if (pDev->Framebuffer.ActiveCount == 1)
                    //
                    // If this is the only double buffered application, then buffer
                    // toggle is always 0 because the toggle does not actually
                    // toggle for single double bufferd clients.
                    //
                    PatchContext->Canvas->BufferToggle = 0;
                else
                    //
                    // When multiple double buffered sessions are active set it to be
                    // whatever the current buffer is.
                    //
                    PatchContext->Canvas->BufferToggle = pDev->Framebuffer.Current;
            }
            PatchContext->VideoConfig = 0;
            if (PatchContext->ColorMap0)
            {
                //
                // Video switch occurs after the colormap.
                //
                if (VideoObject->VideoInput[0] == PatchContext->ColorMap0->VideoOutput)
                    PatchContext->VideoConfig = 1;
                if (VideoObject->VideoInput[1] == PatchContext->ColorMap1->VideoOutput)
                    PatchContext->VideoConfig += 2;
            }
            else
            {
                //
                // Video switch occurs after imagevideo.
                //
                if (VideoObject->VideoInput[0] == ((PIMAGEVIDEOOBJECT)PatchContext->ImageObject)->VideoOutput)
                    PatchContext->VideoConfig = 1;
                if (VideoObject->VideoInput[1] == ((PIMAGEVIDEOOBJECT)PatchContext->ImageObject)->VideoOutput)
                    PatchContext->VideoConfig += 2;
            }
            if (PatchContext->VideoConfig == 1)
                PatchContext->ColorMap1 = NULL;
            else if (PatchContext->VideoConfig == 2)
                PatchContext->ColorMap0 = NULL;
            if (!VideoObject->VideoOutput)
                return (RM_ERR_INCOMPLETE_PATCH);
            //VideoObject = VideoObject->VideoOutput->Destination;
        }
        else*/
        if (VideoObject->Base.Class->Type == NV_VIDEO_COLORMAP)
        {
            //
            // Add the color map to the patch configuration.
            //
            PatchContext->ColorMap0 = (PVIDEOCOLORMAPOBJECT)VideoObject;
            PatchContext->ColorMap1 = (PVIDEOCOLORMAPOBJECT)VideoObject;
            if (!VideoObject->VideoOutput)
                return (RM_ERR_INCOMPLETE_PATCH);
            //VideoObject = VideoObject->VideoOutput->Destination;
        }
        else if (VideoObject->Base.Class->Type == NV_VIDEO_SCALER)
        {
            //
            // Add the scaler to the patch configuration.
            //
            PatchContext->VideoScaler = (PVIDEOSCALEROBJECT)VideoObject;
            if (!VideoObject->VideoOutput)
                return (RM_ERR_INCOMPLETE_PATCH);
        }
        else if (VideoObject->Base.Class->Type == NV_VIDEO_COLOR_KEY)
        {
            //
            // Add the color key to the patch configuration.
            //
            PatchContext->VideoColorKey = (PVIDEOCOLORKEYOBJECT)VideoObject;
            if (!VideoObject->VideoOutput)
                return (RM_ERR_INCOMPLETE_PATCH);
        }
        else
            return (RM_ERR_INVALID_PATCH);

        //
        // Follow patchcord to next video object.
        //
        VideoPatchcord = VideoObject->VideoOutput;
        if (VideoPatchcord && VideoPatchcord->FanOut)
        {
            DBG_VAL_PTR(VideoPatchcord);
            if (VideoPatchcord->FanOut > 1)
                return (RM_ERR_PATCH_TOO_COMPLEX);
            //
            // Search for the first destination.
            //    
            for (i = 0; !(VideoObject = VideoPatchcord->Destination[i]); i++)
			    ;
#if DEBUG
            if (!VideoObject)
            {
                GR_PRINTF((DBG_LEVEL_ERRORS, "NVRM: Invalid patchcord in VideoPatchcordValidation!\n\r"));
                DBG_BREAKPOINT();
                return (RM_ERROR);
            }
#endif // DEBUG
        }
        else
            return (RM_ERR_INCOMPLETE_PATCH);

    }
    return (VideoObject ? RM_OK : RM_ERR_INCOMPLETE_PATCH);
}


//---------------------------------------------------------------------------
//
//  Invalidate graphics patchcords.
//
//---------------------------------------------------------------------------

RM_STATUS grInvalidateVideoPatchcord
(
    PVIDEOPATCHCORD VideoPatchcord
)
{
    U032            i;
    
    //
    // Find input of patchcord.
    //
    if (!VideoPatchcord || !VideoPatchcord->Source)
        return (RM_OK);
    DBG_VAL_PTR(VideoPatchcord);
    DBG_VAL_PTR(VideoPatchcord->Source);
    switch (VideoPatchcord->Source->Base.Class->Type)
    {
        case NV_VIDEO_COLORMAP:
            grInvalidateVideoPatchcord(((PVIDEOCOLORMAPOBJECT)VideoPatchcord->Source)->VideoInput);
            break;
        case NV_VIDEO_SINK:
            for (i = 0; i < MAX_GRPATCH_INPUT; i++)
                grInvalidateVideoPatchcord(((PVIDEOSINKOBJECT)VideoPatchcord->Source)->VideoInput[i]);
            break;
#ifdef DEBUG
        default:
            GR_PRINTF((DBG_LEVEL_ERRORS, "NVRM:Invalid object type for InvalidateVideoPatchcord.\n\r"));
            DBG_BREAKPOINT();
#endif
    }
    return (RM_OK);
}

//---------------------------------------------------------------------------
//
//  Validate object context.
//
//---------------------------------------------------------------------------

