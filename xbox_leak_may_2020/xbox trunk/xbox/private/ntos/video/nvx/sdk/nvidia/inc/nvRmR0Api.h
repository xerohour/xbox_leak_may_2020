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
 * NvRmR0Api.h
 *
 * NVidia resource manager API header file exported to drivers which are operating in Ring0
 *
 */

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

// NOTE NOTE NOTE
// Those functions which are commented out are not yet implemented for the Ring0 interface

//HANDLE __stdcall NvRmR0Open(VOID);
//VOID   __stdcall NvRmR0Close(VOID);
ULONG  __stdcall NvRmR0AllocRoot(ULONG *phClient);
ULONG  __stdcall NvRmR0AllocDevice(ULONG hClient, ULONG hDevice, ULONG hClass, PUCHAR szName);
ULONG  __stdcall NvRmR0AllocContextDma(ULONG hClient, ULONG hDma, ULONG hClass, ULONG flags, PVOID base, ULONG limit);
ULONG  __stdcall NvRmR0AllocChannelPio(ULONG hClient, ULONG hDevice, ULONG hChannel, ULONG hClass, ULONG hErrorCtx, PVOID *ppChannel, ULONG flags);
ULONG  __stdcall NvRmR0AllocChannelDma(ULONG hClient, ULONG hDevice, ULONG hChannel, ULONG hClass, ULONG hErrorCtx, ULONG hDataCtx, ULONG offset, PVOID *ppChannel);
ULONG  __stdcall NvRmR0AllocMemory(ULONG hClient, ULONG hParent, ULONG hMemory, ULONG hClass, ULONG flags, PVOID *ppAddress, ULONG *pLimit);
ULONG  __stdcall NvRmR0AllocObject(ULONG hClient, ULONG hChannel, ULONG hObject, ULONG hClass);
ULONG  __stdcall NvRmR0Alloc(ULONG hClient, ULONG hChannel, ULONG hObject, ULONG hClass, PVOID pAllocParms);
ULONG  __stdcall NvRmR0Free(ULONG hClient, ULONG hParent, ULONG hObject);
//ULONG  __stdcall Nv3RmR0GetDmaPushInfo(ULONG hClient, ULONG hDevice, ULONG hChannel, ULONG hDmaContext, ULONG get);
ULONG  __stdcall NvRmR0AllocEvent(ULONG hClient, ULONG hParent, ULONG object, ULONG hClass, ULONG hIndex, PVOID hEvent);
//ULONG  __stdcall NvRmR0ConfigVersion(ULONG hClient, ULONG hDevice, ULONG *pVersion);
//ULONG  __stdcall NvRmR0ConfigGet(ULONG hClient, ULONG hDevice, ULONG index, ULONG *pValue);
//ULONG  __stdcall NvRmR0ConfigSet(ULONG hClient, ULONG hDevice, ULONG index, ULONG newValue, ULONG *pOldValue);
//ULONG  __stdcall NvRmR0ConfigUpdate(ULONG hClient, ULONG hDevice, ULONG flags);
ULONG  __stdcall NvRmR0Interrupt(ULONG hClient, ULONG hDevice);
ULONG  __stdcall NvRmR0I2CAccess(ULONG hClient, ULONG hDevice, VOID* pCtrlStruct );

#ifdef __cplusplus
}
#endif //__cplusplus
