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
* Module: VIDCLRKY.C                                                        *
*   This module implements the NV_VIDEO_COLOR_KEY object class and its      *
*   corresponding methods.                                                  *
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
#include <nv32.h>
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

RM_STATUS class65SetNotifyCtxDma
(
	PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    U032    Data
)
{
    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: class65SetNotifyCtxDma\r\n");
    //
    // Set the notify DMA context.
    //
    return (mthdSetNotifyCtxDma(pDev, Object, Method, Offset, Data));
}
RM_STATUS class65SetNotify
(
	PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    U032    Data
)
{
    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: class65SetNotify\r\n");
    //
    // Set the notification style.
    //
    return (mthdSetNotify(pDev, Object, Method, Offset, Data));
}
RM_STATUS class65SetVideoOutput
(
	PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    V032    Data
)
{
    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: class65SetVideoOutput\r\n");

    return (RM_OK);
}

RM_STATUS class65SetVideoInput
(
	PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    V032    Data
)
{
    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: class65SetVideoInput\r\n");

    return (RM_OK);
}

RM_STATUS class65SetColorFormat
(
    PHWINFO pDev,
    POBJECT Object, 
    PMETHOD Method, 
    U032    Offset, 
    V032    Data
)
{
    PVIDEOCOLORKEYOBJECT    pVidColorKey;
    RM_STATUS               status;

    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: class65SetColorFormat\r\n");
    
    //
    // A nice cast to make the code more readable.
    //
    pVidColorKey = (PVIDEOCOLORKEYOBJECT)Object;

    (void) nvHalVideoMethod(pDev, NV_VIDEO_COLOR_KEY, &pVidColorKey->HalObject, NVFF6_SET_COLOR_FORMAT(0), Data, &status);

    return (status);
}

RM_STATUS class65SetColorKey
(
    PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    V032    Data
)
{
    PVIDEOCOLORKEYOBJECT    pVidColorKey;
    RM_STATUS               status;

    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: class65SetColorKey\r\n");

    //
    // A nice cast to make the code more readable.
    //
    pVidColorKey = (PVIDEOCOLORKEYOBJECT)Object;

    (void) nvHalVideoMethod(pDev, NV_VIDEO_COLOR_KEY, &pVidColorKey->HalObject, NVFF6_SET_COLOR_KEY(0), Data, &status);

    return (status);
}

RM_STATUS class65SetPoint
(
    PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    V032    Data
)
{
    PVIDEOCOLORKEYOBJECT    pVidColorKey;
    RM_STATUS               status;

    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: class65SetPoint\r\n");

    //
    // A nice cast to make the code more readable.
    //
    pVidColorKey = (PVIDEOCOLORKEYOBJECT)Object;

    (void) nvHalVideoMethod(pDev, NV_VIDEO_COLOR_KEY, &pVidColorKey->HalObject, NVFF6_SET_POINT(0), Data, &status);

    return (status);
}

RM_STATUS class65SetSize
(
    PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    V032    Data
)
{
    PVIDEOCOLORKEYOBJECT    pVidColorKey;
    RM_STATUS               status;

    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: class65SetSize\r\n");

    //
    // A nice cast to make the code more readable.
    //
    pVidColorKey = (PVIDEOCOLORKEYOBJECT)Object;
    //
    // Initialize the color key rectangle.
    //
    // Set the width and height of the color key rectangle.
    //
    if (pDev->Video.HalInfo.CustomizationCode & CUSTOMER_CANOPUS)
    {
        U032 tempData;

        tempData = (Data & 0x0000FFFF);
        if (tempData > 0x00007FFF)
            tempData = 0x00007FFF;
        Data &= 0xFFFF0000;
        Data |= tempData;
    }

    (void) nvHalVideoMethod(pDev, NV_VIDEO_COLOR_KEY, &pVidColorKey->HalObject, NVFF6_SET_SIZE(0), Data, &status);

    return (status);
}


