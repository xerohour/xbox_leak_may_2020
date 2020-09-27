/***************************************************************************\
|*                                                                           *|
|*       Copyright 1993-1998 NVIDIA, Corporation.  All rights reserved.      *|
|*                                                                           *|
|*     NOTICE TO USER:   The source code  is copyrighted under  U.S. and     *|
|*     international laws.  NVIDIA, Corp. of Sunnyvale,  California owns     *|
|*     copyrights, patents, and has design patents pending on the design     *|
|*     and  interface  of the NV chips.   Users and  possessors  of this     *|
|*     source code are hereby granted a nonexclusive, royalty-free copy-     *|
|*     right  and design patent license  to use this code  in individual     *|
|*     and commercial software.                                              *|
|*                                                                           *|
|*     Any use of this source code must include,  in the user documenta-     *|
|*     tion and  internal comments to the code,  notices to the end user     *|
|*     as follows:                                                           *|
|*                                                                           *|
|*     Copyright  1993-1998  NVIDIA,  Corporation.   NVIDIA  has  design     *|
|*     patents and patents pending in the U.S. and foreign countries.        *|
|*                                                                           *|
|*     NVIDIA, CORPORATION MAKES NO REPRESENTATION ABOUT THE SUITABILITY     *|
|*     OF THIS SOURCE CODE FOR ANY PURPOSE. IT IS PROVIDED "AS IS" WITH-     *|
|*     OUT EXPRESS OR IMPLIED WARRANTY OF ANY KIND.  NVIDIA, CORPORATION     *|
|*     DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOURCE CODE, INCLUD-     *|
|*     ING ALL IMPLIED WARRANTIES  OF MERCHANTABILITY  AND FITNESS FOR A     *|
|*     PARTICULAR  PURPOSE.  IN NO EVENT  SHALL NVIDIA,  CORPORATION  BE     *|
|*     LIABLE FOR ANY SPECIAL,  INDIRECT,  INCIDENTAL,  OR CONSEQUENTIAL     *|
|*     DAMAGES, OR ANY DAMAGES  WHATSOEVER  RESULTING  FROM LOSS OF USE,     *|
|*     DATA OR PROFITS,  WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR     *|
|*     OTHER TORTIOUS ACTION,  ARISING OUT OF OR IN CONNECTION  WITH THE     *|
|*     USE OR PERFORMANCE OF THIS SOURCE CODE.                               *|
|*                                                                           *|
|*     RESTRICTED RIGHTS LEGEND:  Use, duplication, or disclosure by the     *|
|*     Government is subject  to restrictions  as set forth  in subpara-     *|
|*     graph (c) (1) (ii) of the Rights  in Technical Data  and Computer     *|
|*     Software  clause  at DFARS  52.227-7013 and in similar clauses in     *|
|*     the FAR and NASA FAR Supplement.                                      *|
|*                                                                           *|
\***************************************************************************/

/*
* AHal.h
*
* Chip-independent functions exported by the RM kernel
*
*/


#if !defined _AHALH_
#define _AHALH_

#if defined __cplusplus
extern "C"
{
#endif

// can be max 255
#define MAX_CLIENTS		            8

typedef struct
{
    PISRCALLBACK	pFn;
    INTR_MASK		Mask;
    VOID			*pContext;
} MCP1_CLIENT_INFO, *PMCP1_CLIENT_INFO;


RM_STATUS InitAudioHal(U008 uRevID, HWINFO_COMMON_FIELDS *pDev);
VOID DestroyAudioHal(HWINFO_COMMON_FIELDS *pDev);

RM_STATUS AllocDevice(HWINFO_COMMON_FIELDS *pDev);
RM_STATUS FreeDevice(HWINFO_COMMON_FIELDS *pDev);

RM_STATUS DeviceAddClient(HWINFO_COMMON_FIELDS *pDev, VOID *pParam, U008 *pClientRef);
RM_STATUS DeviceRemoveClient(HWINFO_COMMON_FIELDS *pDev, U008 uClientRef);

RM_STATUS AllocContextDma(HWINFO_COMMON_FIELDS *pDev, VOID *pParam);
RM_STATUS FreeContextDma(HWINFO_COMMON_FIELDS *pDev, VOID *pParam);

RM_STATUS AllocObject(HWINFO_COMMON_FIELDS *pDev, VOID *pParam);
RM_STATUS FreeObject(HWINFO_COMMON_FIELDS *pDev, VOID *pParam);

RM_STATUS InterruptService(HWINFO_COMMON_FIELDS *pDev);

#if defined __cplusplus
};
#endif

#endif