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
* Module: grmdnv20.c                                                        *
*   The graphics chip dependent method routines are kept here.              *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*                                                                           *
\***************************************************************************/

#include <nv_ref.h>
#include <nv20_ref.h>
#include <nvrm.h>
#include <nv20_hal.h>
#include "nvhw.h"
#include "nvhalcpp.h"

//
// First level method handler.
//
RM_STATUS nvHalGrClassSoftwareMethod_NV20(PHALHWINFO, PGREXCEPTIONDATA);

//
// Externs.
//
extern RM_STATUS nvHalGrGetState_NV20(PHALHWINFO, PGREXCEPTIONDATA, U032);
extern RM_STATUS nvHalGrPutState_NV20(PHALHWINFO, PGREXCEPTIONDATA, U032);
extern RM_STATUS nvHalGrSetEyeDirection_NV20(PHALHWINFO, PGREXCEPTIONDATA);

//
// NV10_CELSIUS methods handled in the HAL.
//
static RM_STATUS _nvHalClass056GetState(PHALHWINFO, PGREXCEPTIONDATA);
static RM_STATUS _nvHalClass056SetEyeDirection(PHALHWINFO, PGREXCEPTIONDATA);

static HALGRMETHOD Nv10CelsiusPrimitiveHalMethods[] =
{
    {_nvHalClass056GetState,             0x010c, 0x010f},
    {_nvHalClass056SetEyeDirection,      0x072c, 0x0737},
};

//
// NV15_CELSIUS / NV11_CELSIUS methods handled in the HAL.
//
static RM_STATUS _nvHalClass096GetState(PHALHWINFO, PGREXCEPTIONDATA);
static RM_STATUS _nvHalClass096SetEyeDirection(PHALHWINFO, PGREXCEPTIONDATA);

static HALGRMETHOD Nv15CelsiusPrimitiveHalMethods[] =
{
    {_nvHalClass096GetState,             0x010c, 0x010f},
    {_nvHalClass096SetEyeDirection,      0x072c, 0x0737},
};

//
// HAL graphics method table.
//
HALGRMETHODS nvHalGrMethods_NV20[] =
{
    { NV10_CELSIUS_PRIMITIVE,           Nv10CelsiusPrimitiveHalMethods,
                                        sizeof (Nv10CelsiusPrimitiveHalMethods) / sizeof(HALGRMETHOD)},

    { NV15_CELSIUS_PRIMITIVE,           Nv15CelsiusPrimitiveHalMethods,
                                        sizeof (Nv15CelsiusPrimitiveHalMethods) / sizeof(HALGRMETHOD)},

    // NV11 and NV15 Celsius share the same method struct.
    { NV11_CELSIUS_PRIMITIVE,           Nv15CelsiusPrimitiveHalMethods,
                                        sizeof (Nv15CelsiusPrimitiveHalMethods) / sizeof(HALGRMETHOD)}
};
#define NUM_HALGRMETHODS                sizeof (nvHalGrMethods_NV20) / sizeof (HALGRMETHODS)

RM_STATUS
nvHalGrClassSoftwareMethod_NV20(PHALHWINFO pHalHwInfo, PGREXCEPTIONDATA pGrExceptionData)
{
    U032 i, j;
    PHWREG nvAddr;

    nvAddr = pHalHwInfo->nvBaseAddr;
    for (i = 0; i < NUM_HALGRMETHODS; i++)
    {
        //
        // Find out if this class has any HAL methods.
        //
        if (nvHalGrMethods_NV20[i].classNum == pGrExceptionData->classNum)
        {
            //
            // See if this particular method is a HAL method.
            //
            for (j = 0; j < nvHalGrMethods_NV20[i].methodMax; j++)
            {
                if ((pGrExceptionData->Offset >= nvHalGrMethods_NV20[i].pMethods[j].Low) &&
                    (pGrExceptionData->Offset < nvHalGrMethods_NV20[i].pMethods[j].High))
                {
                    //
                    // Idle graphics.
                    //
                    GR_DONE();

                    //
                    // Route the method.
                    //
                    pGrExceptionData->MethodStatus = nvHalGrMethods_NV20[i].pMethods[j].Proc(pHalHwInfo, pGrExceptionData);
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
_nvHalClass056GetState(PHALHWINFO pHalHwInfo, PGREXCEPTIONDATA pGrExceptionData)
{
    RM_STATUS retval;

    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: nvHalClass056GetState\r\n");

    switch (pGrExceptionData->Data)
    {
        case NV056_GET_STATE_GETSTATE_ALL_STATE:
            retval = nvHalGrGetState_NV20(pHalHwInfo, pGrExceptionData, TRUE);
            break;

        case NV056_GET_STATE_PUTSTATE_ALL_STATE:
            retval = nvHalGrPutState_NV20(pHalHwInfo, pGrExceptionData, TRUE);
            break;

        case NV056_GET_STATE_GETSTATE_SNAPSHOT_PRIM_ASSM:
            retval = nvHalGrGetState_NV20(pHalHwInfo, pGrExceptionData, FALSE);
            break;

        case NV056_GET_STATE_GETSTATE_RELOAD_PRIM_ASSM:
            retval = nvHalGrPutState_NV20(pHalHwInfo, pGrExceptionData, FALSE);
            break;

        default:
            retval = RM_ERR_BAD_ARGUMENT;
    }    

    return retval;
}

static RM_STATUS
_nvHalClass096GetState(PHALHWINFO pHalHwInfo, PGREXCEPTIONDATA pGrExceptionData)
{
    RM_STATUS retval;

    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: nvHalClass056PutState\r\n");

    switch (pGrExceptionData->Data)
    {
        case NV096_GET_STATE_GETSTATE_ALL_STATE:
            retval = nvHalGrGetState_NV20(pHalHwInfo, pGrExceptionData, TRUE);
            break;

        case NV096_GET_STATE_PUTSTATE_ALL_STATE:
            retval = nvHalGrPutState_NV20(pHalHwInfo, pGrExceptionData, TRUE);
            break;

        case NV096_GET_STATE_GETSTATE_SNAPSHOT_PRIM_ASSM:
            retval = nvHalGrGetState_NV20(pHalHwInfo, pGrExceptionData, FALSE);
            break;

        case NV096_GET_STATE_GETSTATE_RELOAD_PRIM_ASSM:
            retval = nvHalGrPutState_NV20(pHalHwInfo, pGrExceptionData, FALSE);
            break;

        default:
            retval = RM_ERR_BAD_ARGUMENT;
    }    

    return retval;
}

static RM_STATUS
_nvHalClass056SetEyeDirection(PHALHWINFO pHalHwInfo, PGREXCEPTIONDATA pGrExceptionData)
{
    nvHalGrSetEyeDirection_NV20(pHalHwInfo, pGrExceptionData);
    return RM_OK;
}

static RM_STATUS
_nvHalClass096SetEyeDirection(PHALHWINFO pHalHwInfo, PGREXCEPTIONDATA pGrExceptionData)
{
    nvHalGrSetEyeDirection_NV20(pHalHwInfo, pGrExceptionData);
    return RM_OK;
}

