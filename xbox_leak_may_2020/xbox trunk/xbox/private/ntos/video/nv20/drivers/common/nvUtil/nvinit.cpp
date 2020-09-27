/*
 * Copyright 1993-1998 NVIDIA, Corporation.  All rights reserved.
 *
 * THE INFORMATION CONTAINED HEREIN IS PROPRIETARY AND CONFIDENTIAL TO
 * NVIDIA, CORPORATION.  USE, REPRODUCTION OR DISCLOSURE TO ANY THIRD PARTY
 * IS SUBJECT TO WRITTEN PRE-APPROVAL BY NVIDIA, CORPORATION.
 */
/********************************* Direct 3D *******************************\
*                                                                           *
* Module: NVSWIZ.CPP                                                        *
*   CPU Texture swizzling routines                                          *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       Ben de Waal                 10/20/98 - wrote it                     *
*                                                                           *
\***************************************************************************/
#include "precomp.h"
#include "nvutil.h"
#include "nvdetect.h"

/*
 * globals
 */
DWORD g_dwCPUFeatureSet = 0;
DWORD g_dwCPUClockSpeed = 0;

/*
 * nvUtilCreate
 *
 * initializes the nvutil component
 */
void nvUtilCreate
(
    void
)
{
    /*
     * feature set
     */
    g_dwCPUFeatureSet = (nvDetectMMX()    ? NV_FS_MMX    : 0)
                      | (nvDetectKATMAI() ? NV_FS_KATMAI : 0);

    MEMORYSTATUS ms;
    ms.dwLength = sizeof(ms);
    GlobalMemoryStatus (&ms);

    if (ms.dwTotalPhys >= 250*1024*1024)      g_dwCPUFeatureSet |= NV_FS_64MB | NV_FS_128MB | NV_FS_256MB;
    else if (ms.dwTotalPhys >= 120*1024*1024) g_dwCPUFeatureSet |= NV_FS_64MB | NV_FS_128MB;
    else if (ms.dwTotalPhys >=  60*1024*1024) g_dwCPUFeatureSet |= NV_FS_64MB;

    /*
     * clock speed
     */
    g_dwCPUClockSpeed = nvGetCPUSpeed();
}

/*
 * nvUtilDestroy
 *
 * destroys the nvutil component
 */
void nvUtilDestroy
(
    void
)
{
}

/*
 * nvGetCPUCaps
 *
 * returns CPU caps
 */
void nvGetCPUCaps
(
    NVCPUINFO *pInfo
)
{
    pInfo->dwCPUClockSpeed = g_dwCPUClockSpeed;
    pInfo->dwCPUFeatureSet = g_dwCPUFeatureSet;
}