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
* Module: VIDSCALR.C                                                        *
*   This module implements the NV_VIDEO_SCALER methods.                     *
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
#include <gr.h>   
#include <dma.h>   
#include <modular.h>
#include <os.h>
#include <nv32.h>
#include "nvhw.h"
#include "smp.h"

//---------------------------------------------------------------------------
//
//  Create/Destroy object routines.
//
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//
//  Miscellaneous class support routines.
//
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//
//  Class method routines.
//
//---------------------------------------------------------------------------

RM_STATUS class64SetNotifyCtxDma
(
	PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    U032    Data
)
{
    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: class64SetNotifyCtxDma\r\n");
    //
    // Set the notify DMA context.
    //
    return (mthdSetNotifyCtxDma(pDev, Object, Method, Offset, Data));
}
RM_STATUS class64SetNotify
(
	PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    U032    Data
)
{
    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: class64SetNotify\r\n");
    //
    // Set the notification style.
    //
    return (mthdSetNotify(pDev, Object, Method, Offset, Data));
}

RM_STATUS class64SetVideoOutput
(
	PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    V032    Data
)
{
    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: class64SetVideoOutput\r\n");

    return (RM_OK);
}

RM_STATUS class64SetVideoInput
(
	PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    V032    Data
)
{
    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: class64SetVideoInput\r\n");

    return (RM_OK);
}

RM_STATUS class64SetDeltaDuDx
(
    PHWINFO pDev,
    POBJECT Object, 
    PMETHOD Method, 
    U032    Offset, 
    V032    Data
)
{
    PVIDEOSCALEROBJECT  pVidScaler;
    RM_STATUS           status;

    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: class64SetDeltaDuDx\r\n");

    //
    // A nice cast to make the code more readable.
    //
    pVidScaler = (PVIDEOSCALEROBJECT)Object;

    (void) nvHalVideoMethod(pDev, NV_VIDEO_SCALER, &pVidScaler->HalObject, NVFF7_SET_DELTA_DU_DX(0), Data, &status);

    return (status);
}

RM_STATUS class64SetDeltaDvDy
(
	PHWINFO pDev,
    POBJECT Object, 
    PMETHOD Method, 
    U032    Offset, 
    V032    Data
)
{
    PVIDEOSCALEROBJECT  pVidScaler;
    RM_STATUS           status;

    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: class64SetDeltaDvDy\r\n");

    //
    // A nice cast to make the code more readable.
    //
    pVidScaler = (PVIDEOSCALEROBJECT)Object;

    (void) nvHalVideoMethod(pDev, NV_VIDEO_SCALER, &pVidScaler->HalObject, NVFF7_SET_DELTA_DV_DY(0), Data, &status);

    return (status);
}

RM_STATUS class64SetPoint
(
    PHWINFO pDev,
    POBJECT Object, 
    PMETHOD Method, 
    U032    Offset, 
    V032    Data
)
{
    PVIDEOSCALEROBJECT  pVidScaler;
    RM_STATUS           status;

    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: class64SetPoint\r\n");

    //
    // A nice cast to make the code more readable.
    //
    pVidScaler = (PVIDEOSCALEROBJECT)Object;

    (void) nvHalVideoMethod(pDev, NV_VIDEO_SCALER, &pVidScaler->HalObject, NVFF7_SET_POINT(0), Data, &status);

    return (status);
}
