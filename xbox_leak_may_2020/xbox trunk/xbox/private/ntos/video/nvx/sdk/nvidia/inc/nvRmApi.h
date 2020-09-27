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
 * nvRmApi.h
 *
 * NVidia resource manager API header file exported to drivers.
 *
 */

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

#include <nvos.h>

#ifdef NV_ICD_NT
#define NvRmAlloc NvRmAllocNt
#define NvRmAllocRoot NvRmAllocRootNt
#define NvRmAllocDevice NvRmAllocDeviceNt
#define NvRmAllocContextDma NvRmAllocContextDmaNt
#define NvRmAllocChannelPio NvRmAllocChannelPioNt
#define NvRmAllocChannelDma NvRmAllocChannelDmaNt
#define NvRmAllocMemory NvRmAllocMemoryNt
#define NvRmAllocObject NvRmAllocObjectNt
#define NvRmFree NvRmFreeNt
#define NvRmArchHeap NvRmArchHeapNt
#define NvRmConfigGet NvRmConfigGetNt
#define NvRmConfigGetEx NvRmConfigGetExNt
ULONG  NvRmAllocNt(ULONG hClient, ULONG hParent, ULONG hObject, ULONG hClass, PVOID pAllocParms);
ULONG  NvRmAllocRootNt(ULONG *phClient);
ULONG  NvRmAllocDeviceNt(ULONG hClient, ULONG hDevice, ULONG hClass, PUCHAR szName);
ULONG  NvRmAllocContextDmaNt(ULONG hClient, ULONG hDma, ULONG hClass, ULONG flags, PVOID base, ULONG limit);
ULONG  NvRmAllocChannelDmaNt(ULONG hClient, ULONG hDevice, ULONG hChannel, ULONG hClass, ULONG hErrorCtx, ULONG hDataCtx, ULONG offset, PVOID *ppChannel);
ULONG  NvRmAllocMemoryNt(ULONG hClient, ULONG hParent, ULONG hMemory, ULONG hClass, ULONG flags, PVOID *ppAddress, ULONG *pLimit);
ULONG  NvRmAllocObjectNt(ULONG hClient, ULONG hChannel, ULONG hObject, ULONG hClass);
ULONG  NvRmFreeNt(ULONG hClient, ULONG hParent, ULONG hObject);
ULONG  NvRmArchHeapNt(ULONG hClient, ULONG hParent, ULONG function, ULONG owner, ULONG type, ULONG height, ULONG size,
ULONG *pitch, ULONG *offset, ULONG *address, ULONG *limit, ULONG *free, ULONG *total);
ULONG  NvRmConfigGetNt(ULONG hClient, ULONG hDevice, ULONG index, ULONG *pValue);
ULONG  NvRmConfigGetExNt(ULONG hClient, ULONG hDevice, ULONG index, VOID * paramStructPtr, ULONG ParamSize);
#else /* NV_ICD_NT */
#ifndef WINNT
HANDLE NV_APIENTRY NvRmOpen(VOID);
VOID   NV_APIENTRY NvRmClose(VOID);
ULONG  NV_APIENTRY NvRmAlloc(ULONG hClient, ULONG hParent, ULONG hObject, ULONG hClass, PVOID pAllocParms);
ULONG  NV_APIENTRY NvRmAllocRoot(ULONG *phClient);
ULONG  NV_APIENTRY NvRmAllocDevice(ULONG hClient, ULONG hDevice, ULONG hClass, PUCHAR szName);
ULONG  NV_APIENTRY NvRmAllocContextDma(ULONG hClient, ULONG hDma, ULONG hClass, ULONG flags, PVOID base, ULONG limit);
ULONG  NV_APIENTRY NvRmAllocChannelPio(ULONG hClient, ULONG hDevice, ULONG hChannel, ULONG hClass, ULONG hErrorCtx, PVOID *ppChannel, ULONG flags);
ULONG  NV_APIENTRY NvRmAllocChannelDma(ULONG hClient, ULONG hDevice, ULONG hChannel, ULONG hClass, ULONG hErrorCtx, ULONG hDataCtx, ULONG offset, PVOID *ppChannel);
ULONG  NV_APIENTRY NvRmAllocMemory(ULONG hClient, ULONG hParent, ULONG hMemory, ULONG hClass, ULONG flags, PVOID *ppAddress, ULONG *pLimit);
ULONG  NV_APIENTRY NvRmAllocObject(ULONG hClient, ULONG hChannel, ULONG hObject, ULONG hClass);
ULONG  NV_APIENTRY NvRmFree(ULONG hClient, ULONG hParent, ULONG hObject);
ULONG  NV_APIENTRY Nv3RmGetDmaPushInfo(ULONG hClient, ULONG hDevice, ULONG hChannel, ULONG hDmaContext, ULONG get);
ULONG  NV_APIENTRY NvRmAllocEvent(ULONG hClient, ULONG hParent, ULONG object, ULONG hClass, ULONG hIndex, PVOID hEvent);
ULONG  NV_APIENTRY NvRmArchHeap(ULONG hClient, ULONG hParent, ULONG function, ULONG owner, ULONG type, ULONG height, ULONG size,
                              ULONG *pitch, ULONG *offset, ULONG *address, ULONG *limit, ULONG *free, ULONG *total);
ULONG  NV_APIENTRY NvRmConfigVersion(ULONG hClient, ULONG hDevice, ULONG *pVersion);
ULONG  NV_APIENTRY NvRmConfigGet(ULONG hClient, ULONG hDevice, ULONG index, ULONG *pValue);
ULONG  NV_APIENTRY NvRmConfigSet(ULONG hClient, ULONG hDevice, ULONG index, ULONG newValue, ULONG *pOldValue);
ULONG  NV_APIENTRY NvRmConfigUpdate(ULONG hClient, ULONG hDevice, ULONG flags);
ULONG  NV_APIENTRY NvRmInterrupt(ULONG hClient, ULONG hDevice);
ULONG  NV_APIENTRY NvRmRing0Callback(ULONG hClient, ULONG hDevice, ULONG procAddr, ULONG param1, ULONG param2);
ULONG  NV_APIENTRY NvRmConfigSetEx(ULONG hClient, ULONG hDevice, ULONG index, VOID * paramStructPtr, ULONG ParamSize);
ULONG  NV_APIENTRY NvRmConfigGetEx(ULONG hClient, ULONG hDevice, ULONG index, VOID * paramStructPtr, ULONG ParamSize);
ULONG  NV_APIENTRY NvRmI2CAccess(ULONG hClient, ULONG hDevice, VOID * ctrlStructPtr );
ULONG  NV_APIENTRY NvRmPowerManagement(ULONG hRoot, ULONG hDevice, ULONG head, ULONG command, VOID *paramStructPtr);
ULONG  NV_APIENTRY NvRmDebugControl(ULONG hRoot, ULONG command, VOID *pArgs);
ULONG  NV_APIENTRY NvRmIoFlush(VOID);
#ifdef LINUX
ULONG  NV_APIENTRY NvRmAGPInit(ULONG hClient, ULONG hDevice, ULONG agp_config);
ULONG  NV_APIENTRY NvRmAGPTeardown(ULONG hClient, ULONG hDevice);
#endif
#ifdef MACOS
ULONG  NV_APIENTRY NvRmOsConfigGet(ULONG hClient, ULONG hDevice, ULONG index, ULONG *pValue);
ULONG  NV_APIENTRY NvRmOsConfigSet(ULONG hClient, ULONG hDevice, ULONG index, ULONG newValue, ULONG *pOldValue);
ULONG  NV_APIENTRY NvRmOsConfigSetEx(ULONG hClient, ULONG hDevice, ULONG index, VOID * paramStructPtr, ULONG ParamSize);
ULONG  NV_APIENTRY NvRmOsConfigGetEx(ULONG hClient, ULONG hDevice, ULONG index, VOID * paramStructPtr, ULONG ParamSize);
#endif
#endif // #ifndef WINNT
#endif /* NV_ICD_NT */

#ifdef __cplusplus
}
#endif //__cplusplus
