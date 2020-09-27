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

/******************************* Video Manager *****************************\
*                                                                           *
* Module: video.c                                                           *
*   The video engine is managed here.                                       *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*                                                                           *
\***************************************************************************/

#include <nv_ref.h>
#include <nvrm.h>
#include "nvhw.h"

#define DEBUGLEVEL_TRACEINFO_SCOTTL DEBUGLEVEL_ERRORS

//
// Change Video hardware state.
//
RM_STATUS stateVideo
(
    PHWINFO pDev,
    U032    msg
)
{
    switch (msg)
    {
        case STATE_UPDATE:
            nvHalVideoControl(pDev, VIDEO_CONTROL_UPDATE);
            break;
        case STATE_LOAD:
            nvHalVideoControl(pDev, VIDEO_CONTROL_LOAD);
            break;
        case STATE_UNLOAD:
            nvHalVideoControl(pDev, VIDEO_CONTROL_UNLOAD);
            break;
        case STATE_INIT:
        {
            extern char strDevNodeRM[];
            U032 data32;

            if (OS_READ_REGISTRY_DWORD(pDev, strDevNodeRM, "FlipOnHsync", &data32) == RM_OK)
                pDev->Video.FlipOnHsync = data32;
            else
                pDev->Video.FlipOnHsync = 0x0;
            nvHalVideoControl(pDev, VIDEO_CONTROL_INIT);
            pDev->Video.HalInfo.Enabled = 0;
            break;
        }
        case STATE_DESTROY:
            nvHalVideoControl(pDev, VIDEO_CONTROL_DESTROY);
            break;
    }
    return (RM_OK);
}

//
// Top-level video engine service handler.
V032 videoService
(
    PHWINFO pDev
)
{
    //
    // Even when pDev->ActiveVideoOverlayObject is NULL, don't opt out early, still
    // give the underlying chip specific ISR a chance to quiesce the video HW.
    //
    return pDev->pOverlayServiceHandler(pDev, (POBJECT) pDev->Video.HalInfo.ActiveVideoOverlayObject);
}

