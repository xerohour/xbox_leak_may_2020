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

/************************** HAL Engine Manager *****************************\
*                                                                           *
* Module: haleng.c                                                          *
*   The HAL engine services are in this module.                             *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*                                                                           *
\***************************************************************************/

#include <nv_ref.h>
#include <nvrm.h>
#include <class.h>
#include <gr.h>
#include <nvhal.h>
#include "nvhw.h"

//
// Default per-engine method lists.
//
METHOD tmrDefaultMethods[] =
{
    {mthdNoOperation,               0x0100, 0x0103},
    {mthdSetNotify,                 0x0104, 0x0107},
    {tmrStopTransfer,               0x0108, 0x010b},
    {mthdSetNotifyCtxDma,           0x0180, 0x0183},
    {mthdSetAlarmTime,              0x0300, 0x0307},
    {mthdSetAlarmNotify,            0x030c, 0x030f}
};

extern RM_STATUS grHWMethod(PHWINFO, POBJECT, PMETHOD, U032, U032);

METHOD grDefaultMethods[] =
{
    {grHWMethod,                    0x0100, 0x1fff},
};

//
// EngineInfos[]
//
// This table contains the per-engine stuff needed for
// class object operations such as create, destroy, and method
// handling.
//
ENGINEINFO EngineInfos[] =
{
    { MC_ENGINE_TAG, NULL, 0, NULL, NULL },

    { FB_ENGINE_TAG, NULL, 0, NULL, NULL },

    { DMA_ENGINE_TAG, NULL, 0, dmaCreate, dmaDelete },

    { FIFO_ENGINE_TAG, 0, NULL, NULL, },

    { TMR_ENGINE_TAG,
      tmrDefaultMethods, sizeof (tmrDefaultMethods) / sizeof (METHOD),
      tmrCreateTimer, tmrDeleteTimer },

    { GR_ENGINE_TAG,
      grDefaultMethods, sizeof (grDefaultMethods) / sizeof (METHOD),
      grCreateObj, grDestroyObj },

    { DAC_ENGINE_TAG, 
      NULL, 0,
      dacCreateObj, dacDestroyObj },

    { MP_ENGINE_TAG,
      NULL, 0,
      mpCreateObj, mpDestroyObj },

    { VIDEO_ENGINE_TAG,
      NULL, 0,
      videoCreateObj, videoDestroyObj },

    // NV_CLASS pseudo engine
    { CLASS_PSEUDO_ENGINE_TAG,
      NULL, 0,
      nopCreate, classDeleteWellKnownObjects },

    { NO_ENGINE, NULL, 0, NULL, NULL },
};

#define NUM_ENGINEINFOS         (sizeof (EngineInfos) / sizeof (ENGINEINFO))

U032 nvClassEngineDescriptor = MKENGDECL(CLASS_PSEUDO_ENGINE_TAG, 0, 0);
U032 nvNoEngineDescriptor = MKENGDECL(NO_ENGINE, 0, 0);

//
// engineGetInfo
//
// Given an engine tag, return pointers to the corresponding
// ENGINEINFO and ENGINEDESCRIPTOR data.
//
RM_STATUS
engineGetInfo(PENGINEDB pEngineDB, U032 engineTag, PENGINEINFO *pEngineInfo, PENGINEDESCRIPTOR *pEngineDecl)
{
    U032 i;

    for (i = 0; i < NUM_ENGINEINFOS; i++)
    {
        if (EngineInfos[i].Tag == engineTag)
        {
            *pEngineInfo = &EngineInfos[i];
            break;
        }
    }

    if (i == NUM_ENGINEINFOS)
    {
        if (engineTag == NO_ENGINE)
            *pEngineInfo = 0;
        else
        {
            *pEngineInfo = 0; *pEngineDecl = 0;
            return RM_ERROR;
        }
    }

    for (i = 0; i < pEngineDB->numEngineDescriptors; i++)
    {
        if ((ENGDECL_FIELD(pEngineDB->pEngineDescriptors[i], _OPCODE) == ENGDECL_INSTR) &&
            (engineTag == ENGDECL_FIELD(pEngineDB->pEngineDescriptors[i], _TAG)))
        {
            *pEngineDecl = &pEngineDB->pEngineDescriptors[i];
            return RM_OK;
        }
    }

    //
    // Special case the NV_CLASS engine (used to create the
    // master classes in the fifo db for method-driven object
    // instantiation).
    //
    if (engineTag == CLASS_PSEUDO_ENGINE_TAG)
    {
        *pEngineDecl = &nvClassEngineDescriptor;
        return RM_OK;
    }

    //
    // Special case classes that bypass the engine info stuff
    // entirely (e.g. NV01_MEMOR_LOCAL_USER).  These classes are
    // typically instantiated by separate API calls (i.e. !RmAllocObject).
    if (engineTag == NO_ENGINE)
    {
        *pEngineDecl = &nvNoEngineDescriptor;
        return RM_OK;
    }


    *pEngineDecl = 0x0;
    return RM_ERROR;
}

//
// engineGetBufferSize
//
// This routine walks through the engine descriptor table to
// find a descriptor for the specified engine/type that lets
// us know how much (if any) "scratch" memory the resource
// manager should allocate/free.
//
U032
engineGetBufferSize(PENGINEDB pEngineDB, U032 tag, U032 type)
{
    U032 i;

    for (i = 0; i < pEngineDB->numEngineDescriptors; i++)
    {
        if ((ENGDECL_FIELD(pEngineDB->pEngineDescriptors[i], _OPCODE) == ENGMALLOC_INSTR) &&
            (ENGMALLOC_FIELD(pEngineDB->pEngineDescriptors[i], _TAG) == tag) &&
            (ENGMALLOC_FIELD(pEngineDB->pEngineDescriptors[i], _TYPE) == type))
        {
            return (ENGMALLOC_FIELD(pEngineDB->pEngineDescriptors[i], _SIZE) * 4);
        }
    }

    return (0);
}

//
// engineGetInstMemSize
// 
// This routine walks through the engine descriptor table to
// find a descriptor for the specified engine that lets us 
// know how much (if any) instance memory the resource 
// manager should allocate/free.
//
U032
engineGetInstMemSize(PENGINEDB pEngineDB, U032 tag)
{
    U032 i;

    for (i = 0; i < pEngineDB->numEngineDescriptors; i++)
    {
        if ((ENGDECL_FIELD(pEngineDB->pEngineDescriptors[i], _OPCODE) == ENGINSTMALLOC_INSTR) &&
            (ENGINSTMALLOC_FIELD(pEngineDB->pEngineDescriptors[i], _TAG) == tag))
        {
            return (ENGINSTMALLOC_FIELD(pEngineDB->pEngineDescriptors[i], _SIZE) * 4);
        }
    }

    return (0);
}
